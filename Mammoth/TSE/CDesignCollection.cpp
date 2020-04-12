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

#define PROPERTY_CORE_GAME_STATS				CONSTLIT("core.gameStats")

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
		"OnGlobalObjGateCheck",

		"OnGlobalPlayerBoughtItem",
		"OnGlobalPlayerSoldItem",
		"OnGlobalRunDiagnostics",
		"OnGlobalStartDiagnostics",
		"OnGlobalSystemDiagnostics",

		"OnGlobalSystemStarted",
		"OnGlobalSystemStopped",
		"OnGlobalUniverseCreated",
		"OnGlobalUniverseLoad",
		"OnGlobalUniverseSave",

		"OnGlobalUpdate",
	};

static constexpr char *CACHED_PROPERTIES[] =
	{
		"core.gameStats",
	};

static constexpr int CACHED_PROPERTIES_COUNT = (sizeof(CACHED_PROPERTIES) / sizeof(CACHED_PROPERTIES[0]));

CDesignCollection::CDesignCollection (void) :
		m_Base(true)	//	m_Base owns its types and will free them at the end

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
		Ctx.pDesign = this;
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
		}

	return NOERROR;
	}

ALERROR CDesignCollection::BindDesign (CUniverse &Universe, const TArray<CExtension *> &BindOrder, const TSortMap<DWORD, bool> &TypesUsed, const SBindOptions &Options, CString *retsError)

//	BindDesign
//
//	Binds the design collection to the set of design types in the given list of
//	extensions.

	{
	DEBUG_TRY

	ALERROR error;

	//	Remember that we're in bind design

	m_bInBindDesign = true;

	//	Reset everything

	Unbind();

	//	Generate a table of extension UNIDs that we're including

	TArray<DWORD> ExtensionsIncluded;
	ExtensionsIncluded.InsertEmpty(BindOrder.GetCount());
	for (int i = 0; i < BindOrder.GetCount(); i++)
		ExtensionsIncluded[i] = BindOrder[i]->GetUNID();

	//	Minimum API version
	//
	//	If we're loading an old game, and if we don't have the TypesUsed array, then it
	//	means we need to load all obsolete types. In that case we use API = 0.

	if (!Options.bNewGame && TypesUsed.GetCount() == 0)
		m_dwMinAPIVersion = 0;
	else
		m_dwMinAPIVersion = Options.dwAPIVersion;

	//	Create a design load context

	SDesignLoadCtx Ctx;
	Ctx.pDesign = this;
	Ctx.bBindAsNewGame = Options.bNewGame;
	Ctx.bNoResources = Options.bNoResources;
	Ctx.bTraceBind = Options.bTraceBind;

	//	Loop over the bind list in order and add appropriate types to m_AllTypes
	//	(The order guarantees that the proper types override)

	for (int i = 0; i < BindOrder.GetCount(); i++)
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
			return BindDesignError(Ctx, retsError);

		//	Add the types

		m_AllTypes.Merge(Types, m_OverrideTypes, ExtensionsIncluded, TypesUsed, m_dwMinAPIVersion);

		//	If this is the adventure, then remember it

		if (pExtension->GetType() == extAdventure)
			m_pAdventureExtension = pExtension;

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

	if (Options.bNewGame)
		{
		m_DynamicUNIDs.DeleteAll();
		m_DynamicTypes.DeleteAll();

		if (error = FireOnGlobalTypesInit(Ctx))
			return BindDesignError(Ctx, retsError);

		if (error = CreateTemplateTypes(Ctx))
			return BindDesignError(Ctx, retsError);
		}

	//	Add all the dynamic types. These came either from the saved game file or
	//	from the Template types above.

	m_AllTypes.Merge(m_DynamicTypes, &m_OverrideTypes);

	//	Resolve inheritance and overrides types.

	if (error = ResolveTypeHierarchy(Ctx))
		return BindDesignError(Ctx, retsError);

	m_AllTypes.Merge(m_HierarchyTypes, &m_OverrideTypes);

	//	Now resolve all overrides and inheritance

	if (error = ResolveOverrides(Ctx, TypesUsed))
		return BindDesignError(Ctx, retsError);

	//	Initialize the byType lists

	for (int i = 0; i < m_AllTypes.GetCount(); i++)
		{
		CDesignType *pEntry = m_AllTypes.GetEntry(i);
		m_ByType[pEntry->GetType()].AddEntry(pEntry);
		}

	//	Prepare to bind. This is used by design elements that need two passes
	//	to bind.

	for (int i = 0; i < m_AllTypes.GetCount(); i++)
		{
		CDesignType *pEntry = m_AllTypes.GetEntry(i);
		if (error = pEntry->PrepareBindDesign(Ctx))
			return BindDesignError(Ctx, retsError);

		//	We take this opportunity to build a list of display attributes
		//	defined by each type.

		const CArmorMassDefinitions &ArmorDefinitions = pEntry->GetArmorMassDefinitions();
		if (!ArmorDefinitions.IsEmpty())
			m_ArmorDefinitions.Append(ArmorDefinitions);

		const CDisplayAttributeDefinitions &Attribs = pEntry->GetDisplayAttributes();
		if (!Attribs.IsEmpty())
			m_DisplayAttribs.Append(Attribs);
		}

	//	Tell our armor mass definitions that we're done so that we can calculate
	//	some indices. This must be called before Bind for armor.

	m_ArmorDefinitions.OnInitDone();

	//	Cache a map between currency name and economy type
	//	We need to do this before Bind because some types will lookup
	//	a currency name during Bind.

	if (!InitEconomyTypes(Ctx))
		return BindDesignError(Ctx, retsError);

	//	Initialize the adventure descriptor. This will also initialize various
	//	engine options. We do this after PrepareBind because we will bind the
	//	adventure here.

	if (!InitAdventure(Ctx))
		return BindDesignError(Ctx, retsError);

	//	Now call Bind on all active design entries. We may proceed recursively.
	//	That is, when binding a type, that type might depend on other types, and
	//	we recursively bind those types.

	for (int i = 0; i < m_AllTypes.GetCount(); i++)
		{
		CDesignType *pEntry = m_AllTypes.GetEntry(i);

		//	Bind if necessary.

		if (!pEntry->IsBound())
			{
			if (error = pEntry->BindDesign(Ctx))
				return BindDesignError(Ctx, retsError);
			}

		//	Cache some global events. We keep track of the global events for
		//	all types so that we can access them faster.

		CacheGlobalEvents(pEntry);
		}

	//	Remember what we bound

	m_BoundExtensions = BindOrder;
	m_bInBindDesign = false;

	return NOERROR;

	DEBUG_CATCH
	}

ALERROR CDesignCollection::BindDesignError (SDesignLoadCtx &Ctx, CString *retsError)

//	BindDesignError
//
//	Helper to return an error from inside of BindDesign.

	{
	m_bInBindDesign = false;
	if (retsError) *retsError = Ctx.sError;
	return ERR_FAIL;
	}

void CDesignCollection::CacheGlobalEvents (CDesignType *pType)

//	CacheGlobalEvents
//
//	Caches global events for the given type

	{
	DEBUG_TRY

	const CEventHandler *pEvents;
	TSortMap<CString, SEventHandlerDesc> FullEvents;
	pType->GetEventHandlers(&pEvents, &FullEvents);
	if (pEvents)
		{
		SEventHandlerDesc Event;
		Event.pExtension = pType->GetExtension();

		for (int i = 0; i < pEvents->GetCount(); i++)
			{
			CString sEvent = pEvents->GetEvent(i, &Event.pCode);

			for (int j = 0; j < evtCount; j++)
				if (m_EventsCache[j]->Insert(pType, sEvent, Event))
					break;
			}
		}
	else
		{
		for (int i = 0; i < FullEvents.GetCount(); i++)
			{
			CString sEvent = FullEvents.GetKey(i);
			const SEventHandlerDesc &Event = FullEvents[i];

			for (int j = 0; j < evtCount; j++)
				if (m_EventsCache[j]->Insert(pType, sEvent, Event))
					break;
			}
		}

	//	Cache global properties

	for (int i = 0; i < CACHED_PROPERTIES_COUNT; i++)
		{
		CString sProperty(CACHED_PROPERTIES[i]);

		ICCItemPtr pResult;
		if (pType->FindCustomProperty(sProperty, pResult))
			{
			auto *pList = m_PropertyCache.SetAt(sProperty);
			pList->Insert(pType);
			}
		}

	DEBUG_CATCH
	}

void CDesignCollection::CleanUp (void)

//	CleanUp
//
//	Free all entries so that we don't hold on to any resources.

	{
	m_CreatedTypes.DeleteAll();
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

	CCodeChainCtx CCCtx(GetUniverse());

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

const CDesignType *CDesignCollection::FindEntry (DWORD dwUNID) const

//	FindEntry
//
//	Finds an entry and returns it (or NULL).

	{
	const CDesignType *pType = m_AllTypes.FindByUNID(dwUNID);
	if (pType == NULL)
		return NULL;

#ifdef DEBUG
	if (!m_bInBindDesign && !pType->IsBound())
		{
		throw CException(ERR_FAIL);
		}
#endif

	return pType;
	}

CDesignType *CDesignCollection::FindEntry (DWORD dwUNID)

//	FindEntry
//
//	Finds an entry and returns it (or NULL).

	{
	CDesignType *pType = m_AllTypes.FindByUNID(dwUNID);
	if (pType == NULL)
		return NULL;

#ifdef DEBUG
	if (!m_bInBindDesign && !pType->IsBound())
		{
		throw CException(ERR_FAIL);
		}
#endif

	return pType;
	}

CDesignType *CDesignCollection::FindEntryBound (SDesignLoadCtx &Ctx, DWORD dwUNID)

//	FindEntryBound
//
//	Returns the entry, making sure to call Bind if necessary. This is designed
//	to be called from inside a different type's OnBind.

	{
	CDesignType *pType = m_AllTypes.FindByUNID(dwUNID);
	if (pType == NULL)
		return NULL;

	if (!pType->IsBound())
		{
		if (pType->BindDesign(Ctx) != NOERROR)
			{
			Ctx.GetUniverse().LogOutput(strPatternSubst(CONSTLIT("Bind: %s"), Ctx.sError));
			return NULL;
			}
		}

	return pType;
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
	//	Add achievements from core.gameStats property.

	auto *pList = m_PropertyCache.GetAt(PROPERTY_CORE_GAME_STATS);
	if (pList)
		{
		for (int i = 0; i < pList->GetCount(); i++)
			{
			CDesignType *pType = pList->GetAt(i);

			ICCItemPtr pResult;
			if (pType->FindCustomProperty(PROPERTY_CORE_GAME_STATS, pResult)
					&& !pResult->IsNil())
				{
				if (pResult->IsError())
					pType->ReportEventError(PROPERTY_CORE_GAME_STATS, pResult);
				else
					Stats.InsertFromCCItem(*pType, *pResult);
				}
			}
		}

	//	Add achievements from <GetGlobalAchievements>

	for (int i = 0; i < m_EventsCache[evtGetGlobalAchievements]->GetCount(); i++)
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

void CDesignCollection::FireOnGlobalEndDiagnostics (SDiagnosticsCtx &Ctx)

//	FireOnGlobalEndDiagnostics
//
//	Done running diagnostics. Output results.

	{
	int iCount = m_EventsCache[evtOnGlobalEndDiagnostics]->GetCount();
	Ctx.iTotalTests += iCount;

	//	Fire all events

	for (int i = 0; i < iCount; i++)
		{
		SEventHandlerDesc Event;
		CDesignType *pType = m_EventsCache[evtOnGlobalEndDiagnostics]->GetEntry(i, &Event);

		if (!pType->FireOnGlobalEndDiagnostics(Event))
			Ctx.iTotalErrors++;
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

bool CDesignCollection::FireOnGlobalObjGateCheck (CSpaceObject *pObj, CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pGateObj)

//	FireOnGlobalObjGateCheck
//
//	Asks all types whether they will allow the player to gate.

	{
	bool bResult = true;

	for (int i = 0; i < m_EventsCache[evtOnGlobalObjGateCheck]->GetCount(); i++)
		{
		SEventHandlerDesc Event;
		CDesignType *pType = m_EventsCache[evtOnGlobalObjGateCheck]->GetEntry(i, &Event);

		if (!pType->FireOnGlobalObjGateCheck(Event, pObj, pDestNode, sDestEntryPoint, pGateObj))
			bResult = false;
		}

	return bResult;
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

void CDesignCollection::FireOnGlobalRunDiagnostics (SDiagnosticsCtx &Ctx)

//	FireOnGlobalRunDiagnostics
//
//	Called after StartDiagnostics

	{
	int iCount = m_EventsCache[evtOnGlobalRunDiagnostics]->GetCount();
	Ctx.iTotalTests += iCount;

	//	Fire all events

	for (int i = 0; i < iCount; i++)
		{
		SEventHandlerDesc Event;
		CDesignType *pType = m_EventsCache[evtOnGlobalRunDiagnostics]->GetEntry(i, &Event);

		if (!pType->FireOnGlobalRunDiagnostics(Event))
			Ctx.iTotalErrors++;
		}
	}

void CDesignCollection::FireOnGlobalStartDiagnostics (SDiagnosticsCtx &Ctx)

//	FireOnGlobalStartDiagnostics
//
//	Called after all systems created.

	{
	int iCount = m_EventsCache[evtOnGlobalStartDiagnostics]->GetCount();
	Ctx.iTotalTests += iCount;

	//	Fire all events

	for (int i = 0; i < iCount; i++)
		{
		SEventHandlerDesc Event;
		CDesignType *pType = m_EventsCache[evtOnGlobalStartDiagnostics]->GetEntry(i, &Event);

		if (!pType->FireOnGlobalStartDiagnostics(Event))
			Ctx.iTotalErrors++;
		}
	}

void CDesignCollection::FireOnGlobalSystemDiagnostics (SDiagnosticsCtx &Ctx)

//	FireOnGlobalSystemDiagnostics
//
//	Called for each system.

	{
	int iCount = m_EventsCache[evtOnGlobalSystemDiagnostics]->GetCount();
	Ctx.iTotalTests += iCount;

	//	Fire all events

	for (int i = 0; i < iCount; i++)
		{
		SEventHandlerDesc Event;
		CDesignType *pType = m_EventsCache[evtOnGlobalSystemDiagnostics]->GetEntry(i, &Event);

		if (!pType->FireOnGlobalSystemDiagnostics(Event))
			Ctx.iTotalErrors++;
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
			{
			SDesignLoadCtx LoadCtx;
			pImage->Lock(LoadCtx);
			}

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

bool CDesignCollection::InitAdventure (SDesignLoadCtx &Ctx)

//	InitAdventure
//
//	Initializes the adventure descriptor from inside of BindDesign.

	{
	//	Set our adventure desc as current; since adventure descs are always 
	//	loaded this is the only thing that we can use to tell if we should
	//	call global events.
	//
	//	This must happen after Unbind (because that clears it) and after
	//	PrepareBindDesign.
	//
	//	NOTE: m_pAdventureDesc can be NULL (e.g., in the intro screen).

	if (m_ByType[designAdventureDesc].GetCount() > 0)
		{
		m_pAdventureDesc = CAdventureDesc::AsType(m_ByType[designAdventureDesc].GetEntry(0));
		m_pAdventureDesc->SetCurrentAdventure();

		//	Bind the adventure

		if (m_pAdventureDesc->BindDesign(Ctx) != NOERROR)
			return false;

		//	Let the adventure override encounter desc

		if (!OverrideEncounterDesc(Ctx, m_pAdventureDesc->GetEncounterOverrideXML()))
			return false;

		//	Let the universe initialize engine options based on the adventure
		//	description. We need to do this before we start calling Bind because
		//	specific types might need it.

		Ctx.GetUniverse().SetEngineOptions(m_pAdventureDesc->GetEngineOptions());
		}

	//	Done

	return true;
	}

bool CDesignCollection::InitEconomyTypes (SDesignLoadCtx &Ctx)

//	InitEconomyTypes
//
//	Cache a map between currency name and economy type
//	We need to do this before Bind because some types will lookup
//	a currency name during Bind.

	{
	for (int i = 0; i < GetCount(designEconomyType); i++)
		{
		CEconomyType *pEcon = CEconomyType::AsType(GetEntry(designEconomyType, i));
		if (!pEcon->IsBound())
			{
			if (ALERROR error = pEcon->BindDesign(Ctx))
				return false;
			}

		const CString &sName = pEcon->GetSID();

		bool bUnique;
		const CEconomyType **ppDest = m_EconomyIndex.SetAt(sName, &bUnique);
		if (!bUnique)
			{
			pEcon->ComposeLoadError(Ctx, CONSTLIT("Currency ID must be unique"));
			return false;
			}

		*ppDest = pEcon;
		}

	return true;
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

bool CDesignCollection::OverrideEncounterDesc (SDesignLoadCtx &Ctx, const CXMLElement &OverridesXML)

//	OverrideEncounterDesc
//
//	Overrides encounter descriptors.

	{
	//	Loop over all overrides

	for (int i = 0; i < OverridesXML.GetContentElementCount(); i++)
		{
		const CXMLElement *pOverride = OverridesXML.GetContentElement(i);

		//	Get the UNID that we're overriding

		DWORD dwUNID;
		if (::LoadUNID(Ctx, pOverride->GetAttribute(UNID_ATTRIB), &dwUNID) != NOERROR)
			return false;

		if (dwUNID == 0)
			{
			Ctx.sError = CONSTLIT("Encounter override must specify UNID to override.");
			return false;
			}

		//	Get the station type. If we don't find the station, skip it. We 
		//	assume that this is an optional type.

		CStationType *pType = CStationType::AsType(FindUnboundEntry(dwUNID));
		if (pType == NULL)
			{
			if (GetUniverse().InDebugMode())
				GetUniverse().LogOutput(strPatternSubst("Skipping encounter override %08x because type is not found.", dwUNID));
			continue;
			}

		//	Override

		if (!pType->OverrideEncounterDesc(*pOverride, &Ctx.sError))
			return false;
		}

	return true;
	}

bool CDesignCollection::ParseShipClassUNID (const CString &sType, CShipClass **retpClass) const

//	ParseShipClassUNID
//
//	Parses an UNID for a ship class.

	{
	DWORD dwUNID;
	if (!ParseUNID(sType, &dwUNID))
		return false;

	const CShipClass *pClass = CShipClass::AsType(FindEntry(dwUNID));
	if (!pClass)
		return false;

	if (retpClass)
		*retpClass = const_cast<CShipClass *>(pClass);

	return true;
	}

bool CDesignCollection::ParseUNID (const CString &sType, DWORD *retdwUNID) const

//	ParseUNID
//
//	Parses an UNID as either a number or an entity.

	{
	DWORD dwUNID = strParseInt(sType.GetASCIIZPointer(), 0);
	if (dwUNID)
		{
		if (retdwUNID)
			*retdwUNID = dwUNID;

		return true;
		}
	else
		{
		for (int i = 0; i < m_BoundExtensions.GetCount(); i++)
			{
			bool bFound;
			CString sValue = m_BoundExtensions[i]->GetEntities()->ResolveExternalEntity(sType, &bFound);
			if (bFound)
				{
				DWORD dwUNID = strParseInt(sValue.GetASCIIZPointer(), 0);
				if (dwUNID)
					{
					if (retdwUNID)
						*retdwUNID = dwUNID;

					return true;
					}
				else
					return false;
				}
			}

		return false;
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

ALERROR CDesignCollection::ResolveInheritingType (SDesignLoadCtx &Ctx, CDesignType *pType, CDesignType **retpNewType)

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
		if (error = ResolveInheritingType(Ctx, pAncestor, &pAncestor))
			return error;
		}

	//	If our ancestor is a generic type, then we don't need to do anything 
	//	else. We just set up the pointer and we're done.

	if (pAncestor->GetType() == designGenericType)
		{
		pType->SetInheritFrom(pAncestor);
		if (retpNewType) *retpNewType = pType;
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
		if (retpNewType) *retpNewType = pType;
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
	if (retpNewType) *retpNewType = pNewType;

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
	//	Apply all overrides

	for (int i = 0; i < m_OverrideTypes.GetCount(); i++)
		{
		CDesignType *pOverride = m_OverrideTypes.GetEntry(i);

		//	Find the type that we are trying to override. If we can't find it
		//	then just continue without error (it means we're trying to override
		//	a type that doesn't currently exist).

		CDesignType *pAncestor = m_AllTypes.FindByUNID(pOverride->GetUNID());
		if (pAncestor == NULL)
			{
			//	In previous versions, we sometimes saved these overrides, so if
			//	we find it in TypesUsed, then it means we need it.

			if (TypesUsed.Find(pOverride->GetUNID()))
				{
				m_AllTypes.AddOrReplaceEntry(pOverride);
				}

			continue;
			}

		//	If the ancestor is a different type, then this is an error.

		if (pAncestor->GetType() != designGenericType && pAncestor->GetType() != pOverride->GetType())
			return pOverride->ComposeLoadError(Ctx, CONSTLIT("Cannot override a different type."));

		//	Generate a merged XML so that we inherit appropriate XML from our
		//	ancestor.

		CXMLElement *pNewXML = new CXMLElement;
		bool bMerged;
		pNewXML->InitFromMerge(*pAncestor->GetXMLElement(), *pOverride->GetXMLElement(), pOverride->GetXMLMergeFlags(), &bMerged);

		//	If we did not merge anything from our ancestor then no need to create
		//	a new type.

		if (!bMerged)
			{
			delete pNewXML;
			continue;
			}

		//	Define the type (m_CreatedTypes takes ownership of pNewXML).
		//
		//	NOTE: We need to support the case where we define multiple types for
		//	the same UNID. We never use m_CreatedTypes to lookup by UNID, so
		//	this should work as long as DefineType keeps both UNIDs.
		//
		//	LATER: m_CreatedTypes should be a normal CDesignList.

		CDesignType *pNewType;
		if (m_CreatedTypes.DefineType(pOverride->GetExtension(), pOverride->GetUNID(), pNewXML, &pNewType, &Ctx.sError) != NOERROR)
			{
			delete pNewXML;
			return pOverride->ComposeLoadError(Ctx, Ctx.sError);
			}

		//	We set inheritence on the new type

		pNewType->SetModification();
		pNewType->SetMerged();
		pNewType->SetInheritFrom(pAncestor);

		//	Now replace the original

		m_AllTypes.AddOrReplaceEntry(pNewType);
		}

	//	Done

	return NOERROR;
	}

CDesignType *CDesignCollection::ResolveDockScreen (CDesignType *pLocalScreen, const CString &sScreen, CString *retsScreenActual, bool *retbIsLocal)

//	ResolveDockScreen
//
//	Resolves a string screen name.
//
//	If the screen name is an UNID then we return the docks screen type
//	Otherwise, we return pLocalScreen and the screen name.
//
//	NULL means that the screen was not found.

	{
	bool bNotANumber;
	DWORD dwUNID = (DWORD)strToInt(sScreen, 0, &bNotANumber);

	if (bNotANumber)
		{
		if (retbIsLocal)
			*retbIsLocal = true;

		if (retsScreenActual)
			*retsScreenActual = sScreen;

		return pLocalScreen;
		}
	else
		{
		if (retbIsLocal)
			*retbIsLocal = false;

		if (retsScreenActual)
			*retsScreenActual = NULL_STR;

		return CDockScreenType::AsType(FindEntry(dwUNID));
		}
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

void CDesignCollection::Unbind (void)

//	Unbind
//
//	Reset everything.

	{
	//	Unbind everything

	CShipClass::UnbindGlobal();

	for (int i = 0; i < m_AllTypes.GetCount(); i++)
		m_AllTypes.GetEntry(i)->UnbindDesign();
	m_AllTypes.DeleteAll();

	//	Reset the bind tables

	for (int i = 0; i < designCount; i++)
		m_ByType[i].DeleteAll();

	m_CreatedTypes.DeleteAll();
	m_OverrideTypes.DeleteAll();
	m_ArmorDefinitions.DeleteAll();
	m_DisplayAttribs.DeleteAll();
	m_EconomyIndex.DeleteAll();

	//	Reset

	for (int i = 0; i < evtCount; i++)
		m_EventsCache[i]->DeleteAll();

	m_PropertyCache.DeleteAll();

	m_pTopology = NULL;
	m_pAdventureExtension = NULL;
	m_pAdventureDesc = NULL;
	m_EmptyAdventure = CAdventureDesc();
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
