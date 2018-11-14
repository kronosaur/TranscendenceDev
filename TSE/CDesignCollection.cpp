//	CDesignCollection.cpp
//
//	CDesignCollection class

#include "PreComp.h"

#define STAR_SYSTEM_TOPOLOGY_TAG				CONSTLIT("StarSystemTopology")
#define SYSTEM_TOPOLOGY_TAG						CONSTLIT("SystemTopology")

#define CREDITS_ATTRIB							CONSTLIT("credits")
#define DEBUG_ONLY_ATTRIB						CONSTLIT("debugOnly")
#define FILENAME_ATTRIB							CONSTLIT("filename")
#define NAME_ATTRIB								CONSTLIT("name")
#define UNID_ATTRIB								CONSTLIT("UNID")
#define VERSION_ATTRIB							CONSTLIT("version")

#define GET_TYPE_SOURCE_EVENT					CONSTLIT("GetTypeSource")

static char *CACHED_EVENTS[CDesignCollection::evtCount] =
	{
		"GetGlobalAchievements",
		"GetGlobalDockScreen",
		"GetGlobalPlayerPriceAdj",
		"OnGlobalPaneInit",
		"OnGlobalEndDiagnostics",

		"OnGlobalIntroCommand",
		"OnGlobalIntroStarted",

		"OnGlobalMarkImages",
		
		"OnGlobalObjDestroyed",

		"OnGlobalPlayerBoughtItem",
		"OnGlobalPlayerSoldItem",
		"OnGlobalStartDiagnostics",

		"OnGlobalSystemDiagnostics",
		"OnGlobalSystemStarted",
		"OnGlobalSystemStopped",

		"OnGlobalUniverseCreated",
		"OnGlobalUniverseLoad",
		"OnGlobalUniverseSave",

		"OnGlobalUpdate",
	};

CDesignCollection::CDesignCollection (void) :
		m_Base(true),	//	m_Base owns its types and will free them at the end
		m_pAdventureDesc(NULL),
		m_bInBindDesign(false)

//	CDesignCollection construtor

	{
	int i;

	for (i = 0; i < evtCount; i++)
		m_EventsCache[i] = new CGlobalEventCache(CString(CACHED_EVENTS[i], -1, true));
	}

CDesignCollection::~CDesignCollection (void)

//	CDesignCollection destructor

	{
	int i;

	for (i = 0; i < evtCount; i++)
		delete m_EventsCache[i];

	CleanUp();
	}

ALERROR CDesignCollection::AddDynamicType (CExtension *pExtension, DWORD dwUNID, ICCItem *pSource, bool bNewGame, CString *retsError)

//	AddDynamicType
//
//	Adds a dynamic type at runtime

	{
	ALERROR error;

	//	If we're past game-create, the UNID must not already exist

	if (!bNewGame && FindEntry(dwUNID))
		{
		if (retsError)
			*retsError = strPatternSubst(CONSTLIT("Type already exists: %x"), dwUNID);
		return ERR_FAIL;
		}

	//	Add it to the dynamics table

	CDesignType *pType;
	if (error = m_DynamicTypes.DefineType(pExtension, dwUNID, pSource, &pType, retsError))
		return error;

	//	Make sure that the type can be created at this point.
	//	For example, we can't create SystemMap types after the game has started.

	switch (pType->GetType())
		{
		case designAdventureDesc:
		case designGlobals:
		case designImage:
		case designMusic:
		case designSound:
		case designEconomyType:
		case designTemplateType:
			{
			m_DynamicTypes.Delete(dwUNID);
			if (retsError)
				*retsError = CONSTLIT("Dynamic design type not supported.");
			return ERR_FAIL;
			}

		case designSystemType:
		case designSystemTable:
		case designSystemMap:
			{
			if (!bNewGame)
				{
				m_DynamicTypes.Delete(dwUNID);
				if (retsError)
					*retsError = CONSTLIT("Dynamic design type not supported after new game created.");
				return ERR_FAIL;
				}
			}
		}

	//	If this is new game time, then it means that we are inside of BindDesign. In
	//	that case, we don't do anything more (since BindDesign will take care of
	//	it).

	if (!bNewGame)
		{
		//	Since we've already bound, we need to simulate that here (although we
		//	[obviously] don't allow existing types to bind to dynamic types).
		//
		//	We start by adding the type to the AllTypes list

		m_AllTypes.AddOrReplaceEntry(pType);

		//	Next we add it to the specific type tables

		m_ByType[pType->GetType()].AddEntry(pType);

		//	Bind

		SDesignLoadCtx Ctx;
		Ctx.pExtension = pExtension;
		Ctx.bBindAsNewGame = false;

		if (error = pType->PrepareBindDesign(Ctx))
			{
			m_AllTypes.Delete(dwUNID);
			m_ByType[pType->GetType()].Delete(dwUNID);
			m_DynamicTypes.Delete(dwUNID);
			if (retsError)
				*retsError = Ctx.sError;
			return error;
			}

		if (error = pType->BindDesign(Ctx))
			{
			m_AllTypes.Delete(dwUNID);
			m_ByType[pType->GetType()].Delete(dwUNID);
			m_DynamicTypes.Delete(dwUNID);
			if (retsError)
				*retsError = Ctx.sError;
			return error;
			}

		//	Cache some global events

		CacheGlobalEvents(pType);

		//	Done binding

		if (error = pType->FinishBindDesign(Ctx))
			{
			m_AllTypes.Delete(dwUNID);
			m_ByType[pType->GetType()].Delete(dwUNID);
			m_DynamicTypes.Delete(dwUNID);
			if (retsError)
				*retsError = Ctx.sError;
			return error;
			}
		}

	return NOERROR;
	}

ALERROR CDesignCollection::BindDesign (const TArray<CExtension *> &BindOrder, const TSortMap<DWORD, bool> &TypesUsed, DWORD dwAPIVersion, bool bNewGame, bool bNoResources, CString *retsError)

//	BindDesign
//
//	Binds the design collection to the set of design types in the given list of
//	extensions.

	{
	DEBUG_TRY

	ALERROR error;
	int i;

	//	Remeber that we're in bind design

	m_bInBindDesign = true;

	//	Generate a table of extension UNIDs that we're including

	TArray<DWORD> ExtensionsIncluded;
	ExtensionsIncluded.InsertEmpty(BindOrder.GetCount());
	for (i = 0; i < BindOrder.GetCount(); i++)
		ExtensionsIncluded[i] = BindOrder[i]->GetUNID();

	//	Unbind everything

	CShipClass::UnbindGlobal();

	for (i = 0; i < m_AllTypes.GetCount(); i++)
		m_AllTypes.GetEntry(i)->UnbindDesign();
	m_AllTypes.DeleteAll();

	//	Reset the bind tables

	for (i = 0; i < designCount; i++)
		m_ByType[i].DeleteAll();

	m_CreatedTypes.DeleteAll(true);
	m_OverrideTypes.DeleteAll();

	//	Reset

	m_pTopology = NULL;
	m_pAdventureExtension = NULL;

	//	Minimum API version
	//
	//	If we're loading an old game, and if we don't have the TypesUsed array, then it
	//	means we need to load all obsolete types. In that case we use API = 0.

	if (!bNewGame && TypesUsed.GetCount() == 0)
		m_dwMinAPIVersion = 0;
	else
		m_dwMinAPIVersion = dwAPIVersion;

	//	Create a design load context

	SDesignLoadCtx Ctx;
	Ctx.bBindAsNewGame = bNewGame;
	Ctx.bNoResources = bNoResources;

	//	Loop over the bind list in order and add appropriate types to m_AllTypes
	//	(The order guarantees that the proper types override)

	for (i = 0; i < BindOrder.GetCount(); i++)
		{
		CExtension *pExtension = BindOrder[i];

		try {

		const CDesignTable &Types = pExtension->GetDesignTypes();

#ifdef DEBUG_BIND
		::OutputDebugString(strPatternSubst(CONSTLIT("EXTENSION %s\n"), pExtension->GetName()));
		for (int j = 0; j < Types.GetCount(); j++)
			{
			::OutputDebugString(strPatternSubst(CONSTLIT("%08x: %s\n"), Types.GetEntry(j)->GetUNID(), Types.GetEntry(j)->GetNounPhrase()));
			}
#endif

		//	Run globals for the extension

		if (error = pExtension->ExecuteGlobals(Ctx))
			{
			m_bInBindDesign = false;
			*retsError = Ctx.sError;
			return error;
			}

		//	Add the types

		m_AllTypes.Merge(Types, m_OverrideTypes, ExtensionsIncluded, TypesUsed, m_dwMinAPIVersion);

		//	If this is the adventure, then remember it

		if (pExtension->GetType() == extAdventure)
			{
			m_pAdventureExtension = pExtension;
			m_pAdventureDesc = pExtension->GetAdventureDesc();
			}

		//	If this is an adventure or the base extension then take the 
		//	topology.

		if (pExtension->GetType() == extAdventure || pExtension->GetType() == extBase)
			m_pTopology = &pExtension->GetTopology();

		} catch (...)
			{
			m_bInBindDesign = false;
			::kernelDebugLogPattern("Crash processing extension:");
			CExtension::DebugDump(pExtension, true);
			throw;
			}
		}

	//	If this is a new game, then create all the Template types

	if (bNewGame)
		{
		m_DynamicUNIDs.DeleteAll();
		m_DynamicTypes.DeleteAll();

		if (error = FireOnGlobalTypesInit(Ctx))
			{
			m_bInBindDesign = false;
			*retsError = Ctx.sError;
			return error;
			}

		if (error = CreateTemplateTypes(Ctx))
			{
			m_bInBindDesign = false;
			*retsError = Ctx.sError;
			return error;
			}
		}

	//	Add all the dynamic types. These came either from the saved game file or
	//	from the Template types above.

	m_AllTypes.Merge(m_DynamicTypes, &m_OverrideTypes);

	//	Resolve inheritance and overrides types.

	if (error = ResolveTypeHierarchy(Ctx))
		{
		m_bInBindDesign = false;
		*retsError = Ctx.sError;
		return error;
		}

	m_AllTypes.Merge(m_HierarchyTypes, &m_OverrideTypes);

	//	Now resolve all overrides and inheritance

	if (error = ResolveOverrides(Ctx, TypesUsed))
		{
		m_bInBindDesign = false;
		*retsError = Ctx.sError;
		return error;
		}

	//	Initialize the byType lists

	for (i = 0; i < m_AllTypes.GetCount(); i++)
		{
		CDesignType *pEntry = m_AllTypes.GetEntry(i);
		m_ByType[pEntry->GetType()].AddEntry(pEntry);
		}

	//	Set our adventure desc as current; since adventure descs are always 
	//	loaded this is the only thing that we can use to tell if we should
	//	call global events.
	//
	//	This must happen after Unbind (because that clears it) and before
	//	PrepareBindDesign.
	//
	//	NOTE: m_pAdventureDesc can be NULL (e.g., in the intro screen).

	if (m_pAdventureDesc)
		m_pAdventureDesc->SetCurrentAdventure();

	//	Cache a map between currency name and economy type
	//	We need to do this before Bind because some types will lookup
	//	a currency name during Bind.

	m_EconomyIndex.DeleteAll();
	for (i = 0; i < GetCount(designEconomyType); i++)
		{
		CEconomyType *pEcon = CEconomyType::AsType(GetEntry(designEconomyType, i));
		const CString &sName = pEcon->GetSID();

		bool bUnique;
		CEconomyType **ppDest = m_EconomyIndex.SetAt(sName, &bUnique);
		if (!bUnique)
			{
			pEcon->ComposeLoadError(Ctx, CONSTLIT("Currency ID must be unique"));
			m_bInBindDesign = false;
			*retsError = Ctx.sError;
			return ERR_FAIL;
			}

		*ppDest = pEcon;
		}

	//	Prepare to bind. This is used by design elements that need two passes
	//	to bind. We also use it to set up the inheritence hierarchy, which means
	//	that we rely on the map from UNID to valid design type (m_AllTypes)

	m_DisplayAttribs.DeleteAll();

	for (i = 0; i < m_AllTypes.GetCount(); i++)
		{
		CDesignType *pEntry = m_AllTypes.GetEntry(i);
		if (error = pEntry->PrepareBindDesign(Ctx))
			{
			m_bInBindDesign = false;
			*retsError = Ctx.sError;
			return error;
			}

		//	We take this opportunity to build a list of display attributes
		//	defined by each type.

		const CDisplayAttributeDefinitions &Attribs = pEntry->GetDisplayAttributes();
		if (!Attribs.IsEmpty())
			m_DisplayAttribs.Append(Attribs);
		}

	//	Now call Bind on all active design entries

	for (i = 0; i < evtCount; i++)
		m_EventsCache[i]->DeleteAll();

	for (i = 0; i < m_AllTypes.GetCount(); i++)
		{
		CDesignType *pEntry = m_AllTypes.GetEntry(i);
		if (error = pEntry->BindDesign(Ctx))
			{
			m_bInBindDesign = false;
			*retsError = Ctx.sError;
			return error;
			}

		//	Cache some global events. We keep track of the global events for
		//	all types so that we can access them faster.

		CacheGlobalEvents(pEntry);
		}

	//	Finish binding. This pass is used by design elements
	//	that need to do stuff after all designs are bound.

	for (i = 0; i < m_AllTypes.GetCount(); i++)
		{
		CDesignType *pEntry = m_AllTypes.GetEntry(i);
		if (error = pEntry->FinishBindDesign(Ctx))
			{
			m_bInBindDesign = false;
			*retsError = Ctx.sError;
			return error;
			}
		}

	//	Remember what we bound

	m_BoundExtensions = BindOrder;
	m_bInBindDesign = false;

	return NOERROR;

	DEBUG_CATCH
	}

void CDesignCollection::CacheGlobalEvents (CDesignType *pType)

//	CacheGlobalEvents
//
//	Caches global events for the given type

	{
	DEBUG_TRY

	int i, j;

	const CEventHandler *pEvents;
	TSortMap<CString, SEventHandlerDesc> FullEvents;
	pType->GetEventHandlers(&pEvents, &FullEvents);
	if (pEvents)
		{
		SEventHandlerDesc Event;
		Event.pExtension = pType->GetExtension();

		for (i = 0; i < pEvents->GetCount(); i++)
			{
			CString sEvent = pEvents->GetEvent(i, &Event.pCode);

			for (j = 0; j < evtCount; j++)
				if (m_EventsCache[j]->Insert(pType, sEvent, Event))
					break;
			}
		}
	else
		{
		for (i = 0; i < FullEvents.GetCount(); i++)
			{
			CString sEvent = FullEvents.GetKey(i);
			const SEventHandlerDesc &Event = FullEvents[i];

			for (j = 0; j < evtCount; j++)
				if (m_EventsCache[j]->Insert(pType, sEvent, Event))
					break;
			}
		}

	DEBUG_CATCH
	}

void CDesignCollection::CleanUp (void)

//	CleanUp
//
//	Free all entries so that we don't hold on to any resources.

	{
	m_CreatedTypes.DeleteAll(true);
	m_DynamicTypes.DeleteAll();
	m_HierarchyTypes.DeleteAll();

	//	Some classes need to clean up global data
	//	(But we need to do this before we destroy the types)

	CShipClass::Reinit();
	CStationType::Reinit();
	}

void CDesignCollection::ClearImageMarks (void)

//	ClearImageMarks
//
//	Clears marks on all images

	{
	DEBUG_TRY

	int i;

	for (i = 0; i < GetCount(); i++)
		GetEntry(i)->ClearMark();

	DEBUG_CATCH
	}

ALERROR CDesignCollection::CreateTemplateTypes (SDesignLoadCtx &Ctx)

//	CreateTemplateTypes
//
//	This is called inside of BindDesign to create all template types

	{
	DEBUG_TRY

	ALERROR error;
	int i;

	//	Create an appropriate context for running code

	CCodeChainCtx CCCtx;

	//	Loop over all active types looking for templates.
	//	NOTE: We cannot use the type-specific arrays because they have not been
	//	set up yet (remember that we are inside of BindDesign).

	for (i = 0; i < m_AllTypes.GetCount(); i++)
		{
		CDesignType *pTemplate = m_AllTypes.GetEntry(i);
		if (pTemplate->GetType() != designTemplateType)
			continue;

		//	Get the function to generate the type source

		SEventHandlerDesc Event;
		if (pTemplate->FindEventHandler(GET_TYPE_SOURCE_EVENT, &Event))
			{
			ICCItem *pResult = CCCtx.Run(Event);
			if (pResult->IsError())
				{
				Ctx.sError = strPatternSubst(CONSTLIT("GetTypeSource (%x): %s"), pTemplate->GetUNID(), pResult->GetStringValue());
				return ERR_FAIL;
				}
			else if (!pResult->IsNil())
				{
				if (error = AddDynamicType(pTemplate->GetExtension(), pTemplate->GetUNID(), pResult, true, &Ctx.sError))
					return error;
				}

			CCCtx.Discard(pResult);
			}
		}

	return NOERROR;

	DEBUG_CATCH
	}

void CDesignCollection::DebugOutputExtensions (void) const

//	DebugOutputExtensions
//
//	Outputs the list of bound extensions.

	{
	for (int i = 0; i < GetExtensionCount(); i++)
		{
		CExtension *pExtension = GetExtension(i);
		if (pExtension->GetFolderType() == CExtension::folderBase)
			continue;

		CString sName = pExtension->GetName();
		if (sName.IsBlank())
			sName = strPatternSubst(CONSTLIT("%08x"), pExtension->GetUNID());

		CString sVersion = pExtension->GetVersion();
		if (!sVersion.IsBlank())
			sVersion = strPatternSubst(CONSTLIT(" [%s]"), sVersion);

		::kernelDebugLogPattern("Extension: %s%s", sName, sVersion);
		}

	::kernelDebugLogPattern("Using API version: %d", m_dwMinAPIVersion);
	}

CExtension *CDesignCollection::FindExtension (DWORD dwUNID) const

//	FindExtension
//
//	Find the entry for the given extension

	{
	int i;

	for (i = 0; i < m_BoundExtensions.GetCount(); i++)
		if (m_BoundExtensions[i]->GetUNID() == dwUNID)
			return m_BoundExtensions[i];

	return NULL;
	}

CXMLElement *CDesignCollection::FindSystemFragment (const CString &sName, CSystemTable **retpTable) const

//	FindSystemFragment
//
//	Looks up the given system fragment in all system tables

	{
	int i;

	for (i = 0; i < GetCount(designSystemTable); i++)
		{
		CSystemTable *pTable = CSystemTable::AsType(GetEntry(designSystemTable, i));
		if (pTable)
			{
			CXMLElement *pFragment = pTable->FindElement(sName);
			if (pFragment)
				{
				if (retpTable)
					*retpTable = pTable;

				return pFragment;
				}
			}
		}

	return NULL;
	}

void CDesignCollection::FireGetGlobalAchievements (CGameStats &Stats)

//	FireGetGlobalAchievements
//
//	Fire event to fill achievements

	{
	int i;

	for (i = 0; i < m_EventsCache[evtGetGlobalAchievements]->GetCount(); i++)
		{
		CDesignType *pType = m_EventsCache[evtGetGlobalAchievements]->GetEntry(i);

		pType->FireGetGlobalAchievements(Stats);
		}
	}

bool CDesignCollection::FireGetGlobalDockScreen (const CSpaceObject *pObj, DWORD dwFlags, CDockScreenSys::SSelector *retSelector) const

//	FireGetGlobalDockScreen
//
//	Allows types to override the dock screen for an object.

	{
	//	If we don't want override screens then we only return screens that DO NOT
	//	have the override only flag (even if they might be higher priority).
	//	We use this to tell if a station has any screens at all (excluding 
	//	override-only screens).

	bool bNoOverride = ((dwFlags & FLAG_NO_OVERRIDE) ? true : false);

	//	Loop over all types and get the highest priority screen

	CDockScreenSys::SSelector BestScreen;
	BestScreen.iPriority = -1;

	for (int i = 0; i < m_EventsCache[evtGetGlobalDockScreen]->GetCount(); i++)
		{
		SEventHandlerDesc Event;
		CDesignType *pType = m_EventsCache[evtGetGlobalDockScreen]->GetEntry(i, &Event);

		CDockScreenSys::SSelector EventScreen;
		if (pType->FireGetGlobalDockScreen(Event, pObj, EventScreen))
			{
			//	If this is an override only screen and we don't want override
			//	screens, then skip.

			if (bNoOverride && EventScreen.bOverrideOnly)
				continue;

			//	If we don't care about a specific screen then only want to know
			//	whether there is at least one global dock screen, so we take a
			//	short cut.

			else if (retSelector == NULL)
				return true;

			//	Otherwise, see if this is better.

			else if (EventScreen.iPriority > BestScreen.iPriority)
				BestScreen = EventScreen;
			}
		}

	//	If none found, then we're done

	if (BestScreen.iPriority == -1)
		return false;

	//	Otherwise, return screen

	if (retSelector)
		*retSelector = BestScreen;

	return true;
	}

bool CDesignCollection::FireGetGlobalPlayerPriceAdj (STradeServiceCtx &ServiceCtx, ICCItem *pData, int *retiPriceAdj)

//	FireGetGlobalPlayerPriceAdj
//
//	Returns a price adjustment for the player, given a service, a provider,
//	and an item.

	{
	int i;

	//	Fire all events

	int iPriceAdj = 100;
	for (i = 0; i < m_EventsCache[evtGetGlobalPlayerPriceAdj]->GetCount(); i++)
		{
		SEventHandlerDesc Event;
		CDesignType *pType = m_EventsCache[evtGetGlobalPlayerPriceAdj]->GetEntry(i, &Event);

		int iSinglePriceAdj;
		if (pType->FireGetGlobalPlayerPriceAdj(Event, ServiceCtx, pData, &iSinglePriceAdj))
			{
			if (iSinglePriceAdj < 0)
				{
				iPriceAdj = -1;
				break;
				}
			else
				iPriceAdj = iPriceAdj * iSinglePriceAdj / 100;
			}
		}

	//	Done

	if (retiPriceAdj)
		*retiPriceAdj = (iPriceAdj < 0 ? -1 : iPriceAdj);

	return (iPriceAdj != 100);
	}

void CDesignCollection::FireOnGlobalEndDiagnostics (void)

//	FireOnGlobalEndDiagnostics
//
//	Done running diagnostics. Output results.

	{
	int i;

	//	Fire all events

	for (i = 0; i < m_EventsCache[evtOnGlobalEndDiagnostics]->GetCount(); i++)
		{
		SEventHandlerDesc Event;
		CDesignType *pType = m_EventsCache[evtOnGlobalEndDiagnostics]->GetEntry(i, &Event);

		pType->FireOnGlobalEndDiagnostics(Event);
		}
	}

void CDesignCollection::FireOnGlobalIntroCommand(const CString &sCommand)

//	FireOnGlobalIntroCommand
//
//	Notify all types that the user has sent a command to the intro screen

	{
	int i;

	CString sError;
	for (i = 0; i < m_EventsCache[evtOnGlobalIntroCommand]->GetCount(); i++)
		{
		SEventHandlerDesc Event;
		CDesignType *pType = m_EventsCache[evtOnGlobalIntroCommand]->GetEntry(i, &Event);

		pType->FireOnGlobalIntroCommand(Event, sCommand);
		}
	}

void CDesignCollection::FireOnGlobalIntroStarted(void)

//	FireOnGlobalIntroStarted
//
//	Notify all types that the intro screen has started

	{
	int i;

	CString sError;
	for (i = 0; i < m_EventsCache[evtOnGlobalIntroStarted]->GetCount(); i++)
		{
		SEventHandlerDesc Event;
		CDesignType *pType = m_EventsCache[evtOnGlobalIntroStarted]->GetEntry(i, &Event);

		pType->FireOnGlobalIntroStarted(Event);
		}
	}

void CDesignCollection::FireOnGlobalMarkImages (void)

//	FireOnGlobalMarkImages
//
//	Allows types to mark images

	{
	int i;

	//	Fire all events

	for (i = 0; i < m_EventsCache[evtOnGlobalMarkImages]->GetCount(); i++)
		{
		SEventHandlerDesc Event;
		CDesignType *pType = m_EventsCache[evtOnGlobalMarkImages]->GetEntry(i, &Event);

		pType->FireOnGlobalMarkImages(Event);
		}
	}

void CDesignCollection::FireOnGlobalObjDestroyed (SDestroyCtx &Ctx)

//	FireOnGlobalObjDestroyed
//
//	Gives other types a notification when an object is destroyed

	{
	int i;

	//	Fire all events

	for (i = 0; i < m_EventsCache[evtOnGlobalObjDestroyed]->GetCount(); i++)
		{
		SEventHandlerDesc Event;
		CDesignType *pType = m_EventsCache[evtOnGlobalObjDestroyed]->GetEntry(i, &Event);

		pType->FireOnGlobalObjDestroyed(Event, Ctx);
		}
	}

void CDesignCollection::FireOnGlobalPaneInit (void *pScreen, CDesignType *pRoot, const CString &sScreen, const CString &sPane, ICCItem *pData)

//	FireOnGlobalPaneInit
//
//	Give other design types a way to override screens

	{
	int i;
	CString sError;

	//	Generate a screen UNID that contains both the screen UNID and a local screen

	CString sScreenUNID = CDockScreenType::GetStringUNID(pRoot, sScreen);
	DWORD dwRootUNID = (pRoot ? pRoot->GetUNID() : 0);

	//	Fire all events

	for (i = 0; i < m_EventsCache[evtOnGlobalDockPaneInit]->GetCount(); i++)
		{
		SEventHandlerDesc Event;
		CDesignType *pType = m_EventsCache[evtOnGlobalDockPaneInit]->GetEntry(i, &Event);

		if (pType->FireOnGlobalDockPaneInit(Event,
				pScreen,
				dwRootUNID,
				sScreenUNID,
				sScreen,
				sPane,
				pData,
				&sError) != NOERROR)
			kernelDebugLogString(sError);
		}
	}

void CDesignCollection::FireOnGlobalPlayerBoughtItem (CSpaceObject *pSellerObj, const CItem &Item, const CCurrencyAndValue &Price)

//	FireOnGlobalPlayerBoughtItem
//
//	Fire event

	{
	int i;
	CString sError;

	//	Fire all events

	for (i = 0; i < m_EventsCache[evtOnGlobalPlayerBoughtItem]->GetCount(); i++)
		{
		SEventHandlerDesc Event;
		CDesignType *pType = m_EventsCache[evtOnGlobalPlayerBoughtItem]->GetEntry(i, &Event);

		pType->FireOnGlobalPlayerBoughtItem(Event, pSellerObj, Item, Price);
		}
	}

void CDesignCollection::FireOnGlobalPlayerChangedShips (CSpaceObject *pOldShip)

//	FireOnGlobalPlayerChangedShips
//
//	Fire event

	{
	int i;

	CString sError;
	for (i = 0; i < GetCount(); i++)
		{
		if (GetEntry(i)->FireOnGlobalPlayerChangedShips(pOldShip, &sError) != NOERROR)
			kernelDebugLogString(sError);
		}
	}

void CDesignCollection::FireOnGlobalPlayerEnteredSystem (void)

//	FireOnGlobalPlayerEnteredSystem
//
//	Fire event

	{
	int i;

	CString sError;
	for (i = 0; i < GetCount(); i++)
		{
		if (GetEntry(i)->FireOnGlobalPlayerEnteredSystem(&sError) != NOERROR)
			kernelDebugLogString(sError);
		}
	}

void CDesignCollection::FireOnGlobalPlayerLeftSystem (void)

//	FireOnGlobalPlayerLeftSystem
//
//	Fire event

	{
	int i;

	CString sError;
	for (i = 0; i < GetCount(); i++)
		{
		if (GetEntry(i)->FireOnGlobalPlayerLeftSystem(&sError) != NOERROR)
			kernelDebugLogString(sError);
		}
	}

void CDesignCollection::FireOnGlobalPlayerSoldItem (CSpaceObject *pBuyerObj, const CItem &Item, const CCurrencyAndValue &Price)

//	FireOnGlobalPlayerSoldItem
//
//	Fire event

	{
	int i;
	CString sError;

	//	Fire all events

	for (i = 0; i < m_EventsCache[evtOnGlobalPlayerSoldItem]->GetCount(); i++)
		{
		SEventHandlerDesc Event;
		CDesignType *pType = m_EventsCache[evtOnGlobalPlayerSoldItem]->GetEntry(i, &Event);

		pType->FireOnGlobalPlayerSoldItem(Event, pBuyerObj, Item, Price);
		}
	}

void CDesignCollection::FireOnGlobalStartDiagnostics (void)

//	FireOnGlobalStartDiagnostics
//
//	Called after all systems created.

	{
	int i;

	//	Fire all events

	for (i = 0; i < m_EventsCache[evtOnGlobalStartDiagnostics]->GetCount(); i++)
		{
		SEventHandlerDesc Event;
		CDesignType *pType = m_EventsCache[evtOnGlobalStartDiagnostics]->GetEntry(i, &Event);

		pType->FireOnGlobalStartDiagnostics(Event);
		}
	}

void CDesignCollection::FireOnGlobalSystemDiagnostics (void)

//	FireOnGlobalSystemDiagnostics
//
//	Called for each system.

	{
	int i;

	//	Fire all events

	for (i = 0; i < m_EventsCache[evtOnGlobalSystemDiagnostics]->GetCount(); i++)
		{
		SEventHandlerDesc Event;
		CDesignType *pType = m_EventsCache[evtOnGlobalSystemDiagnostics]->GetEntry(i, &Event);

		pType->FireOnGlobalSystemDiagnostics(Event);
		}
	}

void CDesignCollection::FireOnGlobalSystemCreated (SSystemCreateCtx &SysCreateCtx)

//	FireOnGlobalSystemCreate
//
//	Notify all type that a system has been created

	{
	int i;

	CString sError;
	for (i = 0; i < GetCount(); i++)
		{
		if (GetEntry(i)->FireOnGlobalSystemCreated(SysCreateCtx, &sError) != NOERROR)
			kernelDebugLogString(sError);
		}
	}

void CDesignCollection::FireOnGlobalSystemStarted (DWORD dwElapsedTime)

//	FireOnGlobalSystemStarted
//
//	Notify all types that a system is starting

	{
	int i;

	//	Fire all events

	for (i = 0; i < m_EventsCache[evtOnGlobalSystemStarted]->GetCount(); i++)
		{
		SEventHandlerDesc Event;
		CDesignType *pType = m_EventsCache[evtOnGlobalSystemStarted]->GetEntry(i, &Event);

		pType->FireOnGlobalSystemStarted(Event, dwElapsedTime);
		}
	}

void CDesignCollection::FireOnGlobalSystemStopped (void)

//	FireOnGlobalSystemStopped
//
//	Notify all types that a system has stopped

	{
	int i;

	//	Fire all events

	for (i = 0; i < m_EventsCache[evtOnGlobalSystemStopped]->GetCount(); i++)
		{
		SEventHandlerDesc Event;
		CDesignType *pType = m_EventsCache[evtOnGlobalSystemStopped]->GetEntry(i, &Event);

		pType->FireOnGlobalSystemStopped(Event);
		}
	}

ALERROR CDesignCollection::FireOnGlobalTypesInit (SDesignLoadCtx &Ctx)

//	FireOnGlobalTypesInit
//
//	Give each type a chance to create dynamic types before we bind.

	{
	ALERROR error;
	int i;

	for (i = 0; i < m_AllTypes.GetCount(); i++)
		{
		CDesignType *pType = m_AllTypes.GetEntry(i);
		if (error = pType->FireOnGlobalTypesInit(Ctx))
			return error;
		}

	return NOERROR;
	}

void CDesignCollection::FireOnGlobalUniverseCreated (void)

//	FireOnGlobalUniverseCreated
//
//	Notify all types that the universe has been created

	{
	int i;

	CString sError;
	for (i = 0; i < m_EventsCache[evtOnGlobalUniverseCreated]->GetCount(); i++)
		{
		SEventHandlerDesc Event;
		CDesignType *pType = m_EventsCache[evtOnGlobalUniverseCreated]->GetEntry(i, &Event);

		pType->FireOnGlobalUniverseCreated(Event);
		}
	}

void CDesignCollection::FireOnGlobalUniverseLoad (void)

//	FireOnGlobalUniverseLoad
//
//	Notify all types that the universe has loaded

	{
	int i;

	CString sError;
	for (i = 0; i < m_EventsCache[evtOnGlobalUniverseLoad]->GetCount(); i++)
		{
		SEventHandlerDesc Event;
		CDesignType *pType = m_EventsCache[evtOnGlobalUniverseLoad]->GetEntry(i, &Event);

		pType->FireOnGlobalUniverseLoad(Event);
		}
	}

void CDesignCollection::FireOnGlobalUniverseSave (void)

//	FireOnGlobalUniverseSave
//
//	Notify all types that the universe is about to be saved to disk

	{
	int i;

	CString sError;
	for (i = 0; i < m_EventsCache[evtOnGlobalUniverseSave]->GetCount(); i++)
		{
		SEventHandlerDesc Event;
		CDesignType *pType = m_EventsCache[evtOnGlobalUniverseSave]->GetEntry(i, &Event);

		pType->FireOnGlobalUniverseSave(Event);
		}
	}

void CDesignCollection::FireOnGlobalUpdate (int iTick)

//	FireOnGlobalUpdate
//
//	Types get a chance to do whatever they want once every 15 ticks.

	{
	DEBUG_TRY

	CString sError;
	for (int i = 0; i < m_EventsCache[evtOnGlobalUpdate]->GetCount(); i++)
		{
		SEventHandlerDesc Event;
		CDesignType *pType = m_EventsCache[evtOnGlobalUpdate]->GetEntry(i, &Event);

		if ((((DWORD)iTick + pType->GetUNID()) % GLOBAL_ON_UPDATE_CYCLE) == 0)
			pType->FireOnGlobalUpdate(Event);
		}

	DEBUG_CATCH
	}

DWORD CDesignCollection::GetDynamicUNID (const CString &sName)

//	GetDynamicUNID
//
//	Returns an UNID for the given unique name.

	{
	//	First look in the table to see if the UNID already exists.

	DWORD dwAtom = m_DynamicUNIDs.atom_Find(sName);
	if (dwAtom == NULL_ATOM)
		m_DynamicUNIDs.atom_Insert(sName, &dwAtom);

	if (dwAtom >= 0x10000000)
		return 0;

	return 0xf0000000 + dwAtom;
	}

void CDesignCollection::GetEnabledExtensions (TArray<CExtension *> *retExtensionList)

//	GetEnabledExtensions
//
//	Returns the list of enabled extensions

	{
	int i;

	retExtensionList->DeleteAll();

	for (i = 0; i < GetExtensionCount(); i++)
		{
		CExtension *pEntry = GetExtension(i);
		if (pEntry->GetType() == extExtension)
			retExtensionList->Insert(pEntry);
		}
	}

CG32bitImage *CDesignCollection::GetImage (DWORD dwUNID, DWORD dwFlags)

//	GetImage
//
//	Returns an image

	{
	CDesignType *pType = m_AllTypes.FindByUNID(dwUNID);
	if (pType == NULL)
		return NULL;

	CObjectImage *pImage = CObjectImage::AsType(pType);
	if (pImage == NULL)
		return NULL;

	if (dwFlags & FLAG_IMAGE_COPY)
		return pImage->CreateCopy();
	else
		{
		CString sError;
		CG32bitImage *pRawImage = pImage->GetRawImage(strFromInt(dwUNID), &sError);

		if (pRawImage == NULL)
			kernelDebugLogString(sError);

		//	Lock, if requested. NOTE: Since we obtained the image above,
		//	this call is guaranteed to succeed.

		if (dwFlags & FLAG_IMAGE_LOCK)
			pImage->Lock(SDesignLoadCtx());

		//	Done

		return pRawImage;
		}
	}

CString CDesignCollection::GetStartingNodeID (void)

//	GetStartingNodeID
//
//	Gets the default starting node ID (if the player ship does not define it).

	{
	int i;

	if (!m_pTopology->GetFirstNodeID().IsBlank())
		return m_pTopology->GetFirstNodeID();

	//	See if any map defines a starting node ID

	for (i = 0; i < GetCount(designSystemMap); i++)
		{
		CSystemMap *pMap = CSystemMap::AsType(GetEntry(designSystemMap, i));

		if (!pMap->GetStartingNodeID().IsBlank())
			return pMap->GetStartingNodeID(); 
		}

	//	Not found

	return NULL_STR;
	}

void CDesignCollection::GetStats (SStats &Result) const

//	GetStats
//
//	Returns stats for the currently bound collection.

	{
	int i;

	Result = SStats();

	Result.Extensions = m_BoundExtensions;
	Result.iAllTypes = m_AllTypes.GetCount();
	Result.iDynamicTypes = m_DynamicTypes.GetCount();
	Result.iMergedTypes = m_HierarchyTypes.GetCount();

	Result.iItemTypes = m_ByType[designItemType].GetCount();
	Result.iShipClasses = m_ByType[designShipClass].GetCount();
	Result.iStationTypes = m_ByType[designStationType].GetCount();
	Result.iResourceTypes = m_ByType[designImage].GetCount() + m_ByType[designSound].GetCount() + m_ByType[designImageComposite].GetCount() + m_ByType[designMusic].GetCount();
	Result.iDockScreens = m_ByType[designDockScreen].GetCount();
	Result.iMissionTypes = m_ByType[designMissionType].GetCount();
	Result.iSovereigns = m_ByType[designSovereign].GetCount();
	Result.iOverlayTypes = m_ByType[designOverlayType].GetCount();
	Result.iSystemTypes = m_ByType[designSystemType].GetCount();
	Result.iEffectTypes = m_ByType[designEffectType].GetCount();
	Result.iSupportTypes = m_ByType[designItemTable].GetCount() + m_ByType[designShipTable].GetCount() + m_ByType[designSystemTable].GetCount() + m_ByType[designGenericType].GetCount()
			+ m_ByType[designEconomyType].GetCount() + m_ByType[designPower].GetCount() + m_ByType[designSpaceEnvironmentType].GetCount() + m_ByType[designSystemMap].GetCount();

	//	Add up all the type memory usage

	for (i = 0; i < m_BoundExtensions.GetCount(); i++)
		{
		CExtension::SStats ExtStats;
		m_BoundExtensions[i]->AccumulateStats(ExtStats);

		Result.dwBaseTypeMemory += ExtStats.dwBaseTypeMemory;
		Result.dwTotalTypeMemory += ExtStats.dwTotalTypeMemory;
		Result.dwTotalXMLMemory += ExtStats.dwTotalXMLMemory;
		Result.dwGraphicsMemory += ExtStats.dwGraphicsMemory;
		Result.dwWreckGraphicsMemory += ExtStats.dwWreckGraphicsMemory;
		}

	if (m_pAdventureExtension)
		Result.dwTotalXMLMemory += m_pAdventureExtension->GetXMLMemoryUsage();
	Result.dwTotalXMLMemory += m_DynamicTypes.GetXMLMemoryUsage();
	Result.dwTotalXMLMemory += m_HierarchyTypes.GetXMLMemoryUsage();
	}

bool CDesignCollection::IsAdventureExtensionBound (DWORD dwUNID)

//	IsAdventureExtensionBound
//
//	Returns TRUE if we have bound on the given adventure extension

	{
	if (m_pAdventureExtension)
		return (m_pAdventureExtension->GetUNID() == dwUNID);
	else
		return (dwUNID == 0);
	}

bool CDesignCollection::IsRegisteredGame (void)

//	IsRegisteredGame
//
//	Returns TRUE if all enabled extensions are registered.

	{
	int i;

	for (i = 0; i < m_BoundExtensions.GetCount(); i++)
		if (!m_BoundExtensions[i]->IsRegistrationVerified())
			return false;

	return true;
	}

void CDesignCollection::MarkGlobalImages (void)

//	MarkGlobalImages
//
//	Mark images needed by types. In general we only mark images that are global
//	(such as damage bitmap images used by ship classes).

	{
	}

void CDesignCollection::NotifyTopologyInit (void)

//	NotifyTopologyInit
//
//	Notify all types that the topology has been initialized.

	{
	int i;

	for (i = 0; i < m_AllTypes.GetCount(); i++)
		{
		CDesignType *pType = m_AllTypes.GetEntry(i);
		pType->TopologyInitialized();
		}
	}

void CDesignCollection::ReadDynamicTypes (SUniverseLoadCtx &Ctx)

//	ReadDynamicTypes
	
	{
	int i;

	//	We need to unbind because there may be some dynamic types from a previous
	//	game that we're about to delete.
	//
	//	This HACK is caused by the fact that Universe::Init is also calling 
	//	BindDesign but ReadDynamicTypes must be called before Init.
	//
	//	A better way to fix this is to load the dynamic types into a separate
	//	structure and pass them into Init to be added then.

	for (i = 0; i < m_AllTypes.GetCount(); i++)
		m_AllTypes.GetEntry(i)->UnbindDesign();
	m_AllTypes.DeleteAll();

	//	Read them

	m_DynamicTypes.ReadFromStream(Ctx);

	//	Read dynamic UNIDs

	m_DynamicUNIDs.DeleteAll();

	DWORD dwCount;
	Ctx.pStream->Read((char *)&dwCount, sizeof(DWORD));

	for (i = 0; i < (int)dwCount; i++)
		{
		CString sName;
		sName.ReadFromStream(Ctx.pStream);

		DWORD dwAtom;
		m_DynamicUNIDs.atom_Insert(sName, &dwAtom);
		ASSERT(dwAtom == (DWORD)i);
		}
	}

void CDesignCollection::Reinit (void)

//	Reinit
//
//	Reinitialize all types

	{
	DEBUG_TRY

	int i;

	//	Reinitialize some global classes

	CShipClass::Reinit();
	CStationType::Reinit();

	//	For reinit that requires two passes

	for (i = 0; i < GetCount(); i++)
		{
		CDesignType *pType = GetEntry(i);
		pType->PrepareReinit();
		}

	//	Reinit design

	for (i = 0; i < GetCount(); i++)
		{
		CDesignType *pType = GetEntry(i);
		pType->Reinit();
		}

	DEBUG_CATCH
	}

ALERROR CDesignCollection::ResolveInheritingType (SDesignLoadCtx &Ctx, CDesignType *pType)

//	ResolveInheritingType
//
//	Resolves a type that inherits from another type. We merge XML from the base
//	if necessary and set up the m_pInheritFrom pointer.

	{
	ALERROR error;

	DWORD dwAncestorUNID = pType->GetInheritFromUNID();

	//	Start by setting the inherit type to ourselves. This is a marker to
	//	indicate that we're in the middle of resolving (so we don't recurse
	//	infinitely).

	pType->SetInheritFrom(pType);

	//	Look for the type that we inherit from. We look first in the hierarchical 
	//	types that we've just created (since they haven't been merged in m_AllTypes
	//	yet).

	CDesignType *pAncestor = m_HierarchyTypes.FindType(dwAncestorUNID);

	//	If we can't find it, then look for it in all types

	if (pAncestor == NULL)
		{
		pAncestor = m_AllTypes.FindByUNID(dwAncestorUNID);
		if (pAncestor == NULL)
			return pType->ComposeLoadError(Ctx, strPatternSubst(CONSTLIT("Unknown inherit type: %0x8x"), dwAncestorUNID));
		}

	//	If our ancestor inherits from themselves, then it means that we've found
	//	an inheritance cycle.

	if (pAncestor->GetInheritFrom() == pAncestor)
		return pType->ComposeLoadError(Ctx, CONSTLIT("Cannot inherit from self."));

	//	If our ancestor also inherits from someone and we haven't yet resolved 
	//	it, then we need recurse.

	if (pAncestor->GetInheritFromUNID() && pAncestor->GetInheritFrom() == NULL)
		{
		if (error = ResolveInheritingType(Ctx, pAncestor))
			return error;
		}

	//	If our ancestor is a generic type, then we don't need to do anything 
	//	else. We just set up the pointer and we're done.

	if (pAncestor->GetType() == designGenericType)
		{
		pType->SetInheritFrom(pAncestor);
		return NOERROR;
		}

	//	If the ancestor is a differen type, then this is an error.

	if (pAncestor->GetType() != pType->GetType())
		return pType->ComposeLoadError(Ctx, CONSTLIT("Cannot inherit from a different type."));

	//	Generate a merged XML so that we inherit appropriate XML from our
	//	ancestor.

	CXMLElement *pNewXML = new CXMLElement;
	bool bMerged;
	pNewXML->InitFromMerge(*pAncestor->GetXMLElement(), *pType->GetXMLElement(), pType->GetXMLMergeFlags(), &bMerged);

	//	If we did not merge anything from our ancestor then no need to create
	//	a new type.

	if (!bMerged)
		{
		delete pNewXML;
		pType->SetInheritFrom(pAncestor);
		return NOERROR;
		}

	//	Define the type (m_HierarchyTypes takes ownership of pNewXML).

	CDesignType *pNewType;
	if (error = m_HierarchyTypes.DefineType(pType->GetExtension(), pType->GetUNID(), pNewXML, &pNewType, &Ctx.sError))
		{
		delete pNewXML;
		return pType->ComposeLoadError(Ctx, Ctx.sError);
		}

	//	We set inheritence on the new type

	pNewType->SetMerged();
	pNewType->SetInheritFrom(pAncestor);

	//	The original type will get replaced, but we need to reset the inherit
	//	pointer. [Since it will be removed from m_AllTypes, it will never get
	//	unbound, so we need to leave it in a pristine state.]

	pType->SetInheritFrom(NULL);

	//	Done

	return NOERROR;
	}

ALERROR CDesignCollection::ResolveOverrides (SDesignLoadCtx &Ctx, const TSortMap<DWORD, bool> &TypesUsed)

//	ResolveOverrides
//
//	Resolve all overrides

	{
	int i;

	//	Apply all overrides

	for (i = 0; i < m_OverrideTypes.GetCount(); i++)
		{
		CDesignType *pOverride = m_OverrideTypes.GetEntry(i);

		//	Find the type that we are trying to override. If we can't find it
		//	then just continue without error (it means we're trying to override
		//	a type that doesn't currently exist).

		CDesignType *pType = m_AllTypes.FindByUNID(pOverride->GetUNID());
		if (pType == NULL)
			{
			//	In previous versions, we sometimes saved these overrides, so if
			//	we find it in TypesUsed, then it means we need it.

			if (TypesUsed.Find(pOverride->GetUNID()))
				{
				m_AllTypes.AddOrReplaceEntry(pOverride);
				}

			continue;
			}

		//	If this type is not already a clone then we need to clone it first
		//	(Because we never modify the original loaded type).

		if (!pType->IsClone())
			{
			CDesignType *pClone;
			pType->CreateClone(&pClone);

			m_CreatedTypes.AddEntry(pClone);

			pType = pClone;
			}

		//	Now modify the type with the override

		pType->MergeType(pOverride);

		//	Replace the original

		m_AllTypes.AddOrReplaceEntry(pType);
		}

	//	Done

	return NOERROR;
	}

ALERROR CDesignCollection::ResolveTypeHierarchy (SDesignLoadCtx &Ctx)

//	ResolveTypeHierarchy
//
//	Creates any new types due to inheritance or override.

	{
	ALERROR error;
	int i;

	//	Start fresh

	m_HierarchyTypes.DeleteAll();

	//	Loop over all types and recursively resolve them.

	for (i = 0; i < m_AllTypes.GetCount(); i++)
		{
		CDesignType *pType = m_AllTypes.GetEntry(i);

		//	If we inherit from something, then resolve it.

		DWORD dwInheritFrom;
		if (dwInheritFrom = pType->GetInheritFromUNID()
				&& pType->GetInheritFrom() == NULL)
			{
			if (error = ResolveInheritingType(Ctx, pType))
				return error;
			}
		}

	//	Done

	return NOERROR;
	}

void CDesignCollection::SweepImages (void)

//	SweepImages
//
//	Frees any images that are not marked

	{
	DEBUG_TRY

	int i;

	CString sError;
	for (i = 0; i < GetCount(); i++)
		GetEntry(i)->Sweep();

	DEBUG_CATCH
	}

void CDesignCollection::WriteDynamicTypes (IWriteStream *pStream)

//	WriteDynamicTypes
//
//	Write dynamic types to the game file
	
	{
	int i;

	m_DynamicTypes.WriteToStream(pStream);

	//	Now write any dynamic UNIDs

	DWORD dwCount = m_DynamicUNIDs.GetCount();
	pStream->Write((char *)&dwCount, sizeof(DWORD));

	for (i = 0; i < (int)dwCount; i++)
		m_DynamicUNIDs.atom_GetKey((DWORD)i).WriteToStream(pStream);
	}

//	CDesignList ----------------------------------------------------------------

void CDesignList::Delete (DWORD dwUNID)

//	Delete
//
//	Deletes the entry by UNID

	{
	int i;

	for (i = 0; i < m_List.GetCount(); i++)
		if (m_List[i]->GetUNID() == dwUNID)
			{
			m_List.Delete(i);
			break;
			}
	}

void CDesignList::DeleteAll (bool bFree)

//	DeleteAll
//
//	Delete all entries
	
	{
	int i;

	if (bFree)
		{
		for (i = 0; i < m_List.GetCount(); i++)
			m_List[i]->Delete();
		}

	m_List.DeleteAll();
	}
