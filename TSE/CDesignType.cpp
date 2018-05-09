//	CDesignType.cpp
//
//	CDesignType class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define ADVENTURE_DESC_TAG						CONSTLIT("AdventureDesc")
#define ATTRIBUTE_DESC_TAG						CONSTLIT("AttributeDesc")
#define DATA_TAG						    	CONSTLIT("Data")
#define DISPLAY_ATTRIBUTES_TAG					CONSTLIT("DisplayAttributes")
#define DOCK_SCREEN_TAG							CONSTLIT("DockScreen")
#define DOCK_SCREENS_TAG						CONSTLIT("DockScreens")
#define ECONOMY_TYPE_TAG						CONSTLIT("EconomyType")
#define EFFECT_TAG								CONSTLIT("Effect")
#define EFFECT_TYPE_TAG							CONSTLIT("EffectType")
#define ENCOUNTER_TABLE_TAG						CONSTLIT("EncounterTable")
#define ENCOUNTER_TYPE_TAG						CONSTLIT("EncounterType")
#define EVENTS_TAG								CONSTLIT("Events")
#define GLOBAL_DATA_TAG							CONSTLIT("GlobalData")
#define IMAGE_TAG								CONSTLIT("Image")
#define IMAGE_COMPOSITE_TAG						CONSTLIT("ImageComposite")
#define INITIAL_DATA_TAG						CONSTLIT("InitialData")
#define ITEM_TABLE_TAG							CONSTLIT("ItemTable")
#define ITEM_TYPE_TAG							CONSTLIT("ItemType")
#define LANGUAGE_TAG							CONSTLIT("Language")
#define LOOKUP_TAG								CONSTLIT("Lookup")
#define MISSION_TYPE_TAG						CONSTLIT("MissionType")
#define OVERLAY_TYPE_TAG						CONSTLIT("OverlayType")
#define POWER_TAG								CONSTLIT("Power")
#define SHIP_CLASS_TAG							CONSTLIT("ShipClass")
#define SHIP_CLASS_OVERRIDE_TAG					CONSTLIT("ShipClassOverride")
#define SHIP_ENERGY_FIELD_TYPE_TAG				CONSTLIT("ShipEnergyFieldType")
#define SHIP_TABLE_TAG							CONSTLIT("ShipTable")
#define SOUND_TAG								CONSTLIT("Sound")
#define SOUNDTRACK_TAG							CONSTLIT("Soundtrack")
#define SOVEREIGN_TAG							CONSTLIT("Sovereign")
#define SPACE_ENVIRONMENT_TYPE_TAG				CONSTLIT("SpaceEnvironmentType")
#define STATIC_DATA_TAG							CONSTLIT("StaticData")
#define STATION_TYPE_TAG						CONSTLIT("StationType")
#define SYSTEM_MAP_TAG							CONSTLIT("SystemMap")
#define SYSTEM_FRAGMENT_TABLE_TAG				CONSTLIT("SystemPartTable")
#define SYSTEM_TYPE_TAG							CONSTLIT("SystemType")
#define TEMPLATE_TYPE_TAG						CONSTLIT("TemplateType")
#define TYPE_TAG								CONSTLIT("Type")

#define ATTRIBUTES_ATTRIB						CONSTLIT("attributes")
#define EFFECT_ATTRIB							CONSTLIT("effect")
#define EXCLUDES_ATTRIB							CONSTLIT("excludes")
#define EXTENDS_ATTRIB							CONSTLIT("extends")
#define INHERIT_ATTRIB							CONSTLIT("inherit")
#define MODIFIERS_ATTRIB						CONSTLIT("modifiers")
#define OBSOLETE_ATTRIB							CONSTLIT("obsolete")
#define UNID_ATTRIB								CONSTLIT("UNID")

#define GET_CREATE_POS_EVENT					CONSTLIT("GetCreatePos")
#define GET_GLOBAL_ACHIEVEMENTS_EVENT			CONSTLIT("GetGlobalAchievements")
#define GET_GLOBAL_DOCK_SCREEN_EVENT			CONSTLIT("GetGlobalDockScreen")
#define GET_GLOBAL_PLAYER_PRICE_ADJ_EVENT		CONSTLIT("GetGlobalPlayerPriceAdj")
#define GET_GLOBAL_RESURRECT_POTENTIAL_EVENT	CONSTLIT("GetGlobalResurrectPotential")
#define ON_GLOBAL_END_DIAGNOSTICS_EVENT			CONSTLIT("OnGlobalEndDiagnostics")
#define ON_GLOBAL_INTRO_COMMAND_EVENT			CONSTLIT("OnGlobalIntroCommand")
#define ON_GLOBAL_INTRO_STARTED_EVENT			CONSTLIT("OnGlobalIntroStarted")
#define ON_GLOBAL_MARK_IMAGES_EVENT				CONSTLIT("OnGlobalMarkImages")
#define ON_GLOBAL_OBJ_DESTROYED_EVENT			CONSTLIT("OnGlobalObjDestroyed")
#define ON_GLOBAL_DOCK_PANE_INIT_EVENT			CONSTLIT("OnGlobalPaneInit")
#define ON_GLOBAL_PLAYER_BOUGHT_ITEM_EVENT		CONSTLIT("OnGlobalPlayerBoughtItem")
#define ON_GLOBAL_PLAYER_CHANGED_SHIPS_EVENT	CONSTLIT("OnGlobalPlayerChangedShips")
#define ON_GLOBAL_PLAYER_ENTERED_SYSTEM_EVENT	CONSTLIT("OnGlobalPlayerEnteredSystem")
#define ON_GLOBAL_PLAYER_LEFT_SYSTEM_EVENT		CONSTLIT("OnGlobalPlayerLeftSystem")
#define ON_GLOBAL_PLAYER_SOLD_ITEM_EVENT		CONSTLIT("OnGlobalPlayerSoldItem")
#define ON_GLOBAL_RESURRECT_EVENT				CONSTLIT("OnGlobalResurrect")
#define ON_GLOBAL_TOPOLOGY_CREATED_EVENT		CONSTLIT("OnGlobalTopologyCreated")
#define ON_GLOBAL_START_DIAGNOSTICS_EVENT		CONSTLIT("OnGlobalStartDiagnostics")
#define ON_GLOBAL_SYSTEM_DIAGNOSTICS_EVENT		CONSTLIT("OnGlobalSystemDiagnostics")
#define ON_GLOBAL_SYSTEM_CREATED_EVENT			CONSTLIT("OnGlobalSystemCreated")
#define ON_GLOBAL_SYSTEM_STARTED_EVENT			CONSTLIT("OnGlobalSystemStarted")
#define ON_GLOBAL_SYSTEM_STOPPED_EVENT			CONSTLIT("OnGlobalSystemStopped")
#define ON_GLOBAL_UNIVERSE_CREATED_EVENT		CONSTLIT("OnGlobalUniverseCreated")
#define ON_GLOBAL_UNIVERSE_LOAD_EVENT			CONSTLIT("OnGlobalUniverseLoad")
#define ON_GLOBAL_UNIVERSE_SAVE_EVENT			CONSTLIT("OnGlobalUniverseSave")
#define ON_GLOBAL_UPDATE_EVENT					CONSTLIT("OnGlobalUpdate")
#define ON_RANDOM_ENCOUNTER_EVENT				CONSTLIT("OnRandomEncounter")

#define SPECIAL_EVENT							CONSTLIT("event:")
#define SPECIAL_EXTENSION						CONSTLIT("extension:")
#define SPECIAL_INHERIT							CONSTLIT("inherit:")
#define SPECIAL_PROPERTY						CONSTLIT("property:")
#define SPECIAL_SYSTEM_LEVEL					CONSTLIT("systemLevel:")
#define SPECIAL_UNID							CONSTLIT("unid:")

#define LANGID_CORE_MAP_DESC                    CONSTLIT("core.mapDesc")
#define LANGID_CORE_MAP_DESC_ABANDONED          CONSTLIT("core.mapDescAbandoned")
#define LANGID_CORE_MAP_DESC_EXTRA              CONSTLIT("core.mapDescExtra")
#define LANGID_CORE_MAP_DESC_MAIN				CONSTLIT("core.mapDescMain")

#define PROPERTY_API_VERSION					CONSTLIT("apiVersion")
#define PROPERTY_ATTRIBUTES						CONSTLIT("attributes")
#define PROPERTY_CLASS							CONSTLIT("class")
#define PROPERTY_EXTENSION						CONSTLIT("extension")
#define PROPERTY_MAP_DESCRIPTION				CONSTLIT("mapDescription")
#define PROPERTY_MERGED							CONSTLIT("merged")
#define PROPERTY_NAME_PATTERN					CONSTLIT("namePattern")

#define FIELD_ENTITY							CONSTLIT("entity")
#define FIELD_EXTENSION_UNID					CONSTLIT("extensionUNID")
#define FIELD_NAME								CONSTLIT("name")
#define FIELD_UNID								CONSTLIT("unid")
#define FIELD_VERSION							CONSTLIT("version")

#define FIELD_ATTRIB_PREFIX						CONSTLIT("attrib-")

static char DESIGN_CHAR[designCount] =
	{
		'i',
		'b',
		's',
		'f',
		'y',
		't',
		'v',
		'd',
		'c',
		'p',

		'e',
		'h',
		'a',
		'g',
		'm',
		'u',
		'n',
		'q',
		'z',
		'w',

		'$',
		'_',
		'x',
		'o',
	};

static char *DESIGN_CLASS_NAME[designCount] =
	{
		"ItemType",
		"ItemTable",
		"ShipClass",
		"OverlayType",
		"SystemType",
		"StationType",
		"Sovereign",
		"DockScreen",
		"EffectType",
		"Power",

		"SpaceEnvironment",
		"ShipTable",
		"AdventureDesc",
		"Globals",
		"Image",
		"Sound",
		"MissionType",
		"SystemTable",
		"SystemMap",
		"NameGenerator",

		"EconomyType",
		"TemplateType",
		"Type",
		"ImageComposite",
	};

static char *CACHED_EVENTS[CDesignType::evtCount] =
	{
		"CanInstallItem",
		"CanRemoveItem",
		"OnGlobalTypesInit",
		"OnObjDestroyed",
		"OnSystemObjAttacked",
		"OnSystemWeaponFire",
		"OnUpdate",
	};

CString ParseAchievementSection (ICCItem *pItem);
CString ParseAchievementSort (ICCItem *pItem);
CString ParseAchievementValue (ICCItem *pItem);

CDesignType::CDesignType (void) : 
		m_dwUNID(0), 
		m_pExtension(NULL),
		m_dwObsoleteVersion(0),
		m_pXML(NULL),
		m_dwInheritFrom(0), 
		m_pInheritFrom(NULL),
		m_bIsModification(false),
		m_bIsClone(false),
		m_bIsMerged(false)
	{
	}

CDesignType::~CDesignType (void)

//	CDesignType destructor

	{
	}

void CDesignType::AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed)

//	AddTypesUsed
//
//	Adds types used by this type to the list

	{
	if (m_dwInheritFrom)
		retTypesUsed->SetAt(m_dwInheritFrom, true);

	OnAddTypesUsed(retTypesUsed);
	}

ALERROR CDesignType::BindDesign (SDesignLoadCtx &Ctx)

//	BindDesign
//
//	Bind design elements
	
	{
	ALERROR error;
	int i;

	Ctx.pType = this;

	//	Now that we've connected to our based classes, update the event cache
	//	with events from our ancestors.

	if (m_pInheritFrom)
		{
		ASSERT(m_pInheritFrom->GetUNID() != GetUNID());
		for (i = 0; i < evtCount; i++)
			{
			if (!HasCachedEvent((ECachedHandlers)i))
				{
				SEventHandlerDesc *pInherit = m_pInheritFrom->GetInheritedCachedEvent((ECachedHandlers)i);
				if (pInherit)
					SetExtra()->EventsCache[i] = *pInherit;
				}
			}
		}

	//	Type-specific

	try
		{
		error = OnBindDesign(Ctx);
		}
	catch (...)
		{
		::kernelDebugLogPattern("Crash in OnBindDesign [UNID: %08x]", m_dwUNID);
		Ctx.pType = NULL;
		throw;
		}

	Ctx.pType = NULL;
	return error;
	}

ALERROR CDesignType::ComposeLoadError (SDesignLoadCtx &Ctx, const CString &sError)

//	ComposeLoadError
//
//	Sets Ctx.sError appropriately and returns ERR_FAIL

	{
	Ctx.sError = strPatternSubst("%s (%x): %s", GetNounPhrase(), GetUNID(), sError);
	return ERR_FAIL;
	}

void CDesignType::CreateClone (CDesignType **retpType)

//	CreateClone
//
//	Creates a clone of this type. Caller is responsible for freeing it.

	{
	CDesignType *pClone;

	switch (GetType())
		{
		case designItemType:
			pClone = new CItemType;
			break;

		case designItemTable:
			pClone = new CItemTable;
			break;

		case designShipClass:
			pClone = new CShipClass;
			break;

		case designOverlayType:
			pClone = new COverlayType;
			break;

		case designSystemType:
			pClone = new CSystemType;
			break;

		case designStationType:
			pClone = new CStationType;
			break;

		case designSovereign:
			pClone = new CSovereign;
			break;

		case designDockScreen:
			pClone = new CDockScreenType;
			break;

		case designEffectType:
			{
			CEffectCreator *pEffectType = CEffectCreator::AsType(this);
			if (pEffectType == NULL)
				{
				ASSERT(false);
				return;
				}

			CEffectCreator *pEffectClone;
			if (CEffectCreator::CreateFromTag(pEffectType->GetTag(), &pEffectClone) != NOERROR)
				{
				ASSERT(false);
				return;
				}

			pClone = pEffectClone;
			break;
			}

		case designPower:
			pClone = new CPower;
			break;

		case designSpaceEnvironmentType:
			pClone = new CSpaceEnvironmentType;
			break;

		case designShipTable:
			pClone = new CShipTable;
			break;

		case designAdventureDesc:
			pClone = new CAdventureDesc;
			break;

		case designImage:
			pClone = new CObjectImage;
			break;

		case designMusic:
			pClone = new CMusicResource;
			break;

		case designMissionType:
			pClone = new CMissionType;
			break;

		case designSound:
			pClone = new CSoundResource;
			break;

		case designSystemTable:
			pClone = new CSystemTable;
			break;

		case designSystemMap:
			pClone = new CSystemMap;
			break;

		case designEconomyType:
			pClone = new CEconomyType;
			break;

		case designTemplateType:
			pClone = new CTemplateType;
			break;

		case designGenericType:
			pClone = new CGenericType;
			break;

		default:
			ASSERT(false);
			return;
		}

	//	Initialize

	pClone->m_bIsClone = true;

	pClone->m_dwUNID = m_dwUNID;
	pClone->m_pExtension = m_pExtension;
	pClone->m_dwVersion = m_dwVersion;
	pClone->m_dwInheritFrom = m_dwInheritFrom;
	pClone->m_pInheritFrom = m_pInheritFrom;
	pClone->m_sAttributes = m_sAttributes;
	pClone->m_Events = m_Events;

	if (m_pExtra)
		pClone->m_pExtra = m_pExtra;

	//	Let our subclass initialize

	pClone->OnInitFromClone(this);

	//	Done

	*retpType = pClone;
	}

ALERROR CDesignType::CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CDesignType **retpType)

//	CreateFromXML
//
//	Creates a design type from an XML element

	{
	try
		{
		ALERROR error;
		CDesignType *pType = NULL;
		bool bOverride = false;

		if (strEquals(pDesc->GetTag(), ITEM_TYPE_TAG))
			pType = new CItemType;
		else if (strEquals(pDesc->GetTag(), ITEM_TABLE_TAG))
			pType = new CItemTable;
		else if (strEquals(pDesc->GetTag(), SHIP_CLASS_TAG))
			pType = new CShipClass;
		else if (strEquals(pDesc->GetTag(), SHIP_ENERGY_FIELD_TYPE_TAG))
			pType = new COverlayType;
		else if (strEquals(pDesc->GetTag(), MISSION_TYPE_TAG))
			pType = new CMissionType;
		else if (strEquals(pDesc->GetTag(), OVERLAY_TYPE_TAG))
			pType = new COverlayType;
		else if (strEquals(pDesc->GetTag(), SYSTEM_TYPE_TAG))
			pType = new CSystemType;
		else if (strEquals(pDesc->GetTag(), STATION_TYPE_TAG)
				|| strEquals(pDesc->GetTag(), ENCOUNTER_TYPE_TAG))
			pType = new CStationType;
		else if (strEquals(pDesc->GetTag(), SOUNDTRACK_TAG))
			pType = new CMusicResource;
		else if (strEquals(pDesc->GetTag(), SOVEREIGN_TAG))
			pType = new CSovereign;
		else if (strEquals(pDesc->GetTag(), DOCK_SCREEN_TAG))
			pType = new CDockScreenType;
		else if (strEquals(pDesc->GetTag(), POWER_TAG))
			pType = new CPower;
		else if (strEquals(pDesc->GetTag(), SPACE_ENVIRONMENT_TYPE_TAG))
			pType = new CSpaceEnvironmentType;
		else if (strEquals(pDesc->GetTag(), ENCOUNTER_TABLE_TAG))
			pType = new CShipTable;
		else if (strEquals(pDesc->GetTag(), SHIP_TABLE_TAG))
			pType = new CShipTable;
		else if (strEquals(pDesc->GetTag(), SOUND_TAG))
			pType = new CSoundResource;
		else if (strEquals(pDesc->GetTag(), SYSTEM_FRAGMENT_TABLE_TAG))
			pType = new CSystemTable;
		else if (strEquals(pDesc->GetTag(), SYSTEM_MAP_TAG))
			pType = new CSystemMap;
		else if (strEquals(pDesc->GetTag(), IMAGE_TAG))
			pType = new CObjectImage;
		else if (strEquals(pDesc->GetTag(), ECONOMY_TYPE_TAG))
			pType = new CEconomyType;
		else if (strEquals(pDesc->GetTag(), TEMPLATE_TYPE_TAG))
			pType = new CTemplateType;
		else if (strEquals(pDesc->GetTag(), TYPE_TAG))
			pType = new CGenericType;
		else if (strEquals(pDesc->GetTag(), IMAGE_COMPOSITE_TAG))
			pType = new CCompositeImageType;
		else if (strEquals(pDesc->GetTag(), ADVENTURE_DESC_TAG))
			{
			//	Only valid if we are inside an Adventure

			if (Ctx.pExtension != NULL && !Ctx.pExtension->CanHaveAdventureDesc())
				{
				Ctx.sError = CONSTLIT("<AdventureDesc> element is only valid for Adventures");
				return ERR_FAIL;
				}

			pType = new CAdventureDesc;
			}
		else if (strEquals(pDesc->GetTag(), EFFECT_TAG))
			{
			//	Load UNID

			DWORD dwUNID;
			if (error = ::LoadUNID(Ctx, pDesc->GetAttribute(UNID_ATTRIB), &dwUNID))
				return error;

			//	This is an old-style CEffectCreator for compatibility

			if (error = CEffectCreator::CreateFromXML(Ctx, pDesc, NULL_STR, (CEffectCreator **)&pType))
				return error;

			pType->m_dwUNID = dwUNID;

			if (!pDesc->FindAttribute(ATTRIBUTES_ATTRIB, &pType->m_sAttributes))
				pType->m_sAttributes = pDesc->GetAttribute(MODIFIERS_ATTRIB);

			if (retpType)
				*retpType = pType;

			//	We skip the normal initialization for backwards compatibility

			return NOERROR;
			}
		else if (strEquals(pDesc->GetTag(), EFFECT_TYPE_TAG))
			{
			//	This is a full effect type. The actual class depends on the content

			if (error = CEffectCreator::CreateTypeFromXML(Ctx, pDesc, (CEffectCreator **)&pType))
				return error;
			}
		else if (strEquals(pDesc->GetTag(), SHIP_CLASS_OVERRIDE_TAG))
			{
			pType = new CShipClass;
			bOverride = true;
			}
		else
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Unknown design element: <%s>"), pDesc->GetTag());
			return ERR_FAIL;
			}

		//	Result

		if (retpType)
			*retpType = pType;

		//	Initialize

		return pType->InitFromXML(Ctx, pDesc, bOverride);
		}
	catch (...)
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Crash loading: %x"), pDesc->GetAttributeInteger(CONSTLIT("UNID")));
		return ERR_FAIL;
		}
	}

ICCItem *CDesignType::FindBaseProperty (CCodeChainCtx &Ctx, const CString &sProperty) const

//	FindBaseProperty
//
//	Returns base property of all design types. We also check to see if we have
//	an old-style data field.
//
//	If we don't have the property, we return NULL.

	{
	int i;
	CCodeChain &CC = g_pUniverse->GetCC();
	CString sValue;
	ICCItem *pResult;
	ICCItemPtr pValue;

	if (strEquals(sProperty, PROPERTY_API_VERSION))
		return CC.CreateInteger(GetAPIVersion());

	else if (strEquals(sProperty, PROPERTY_ATTRIBUTES))
		{
		TArray<CString> Attribs;
		ParseAttributes(GetAttributes(), &Attribs);
		if (Attribs.GetCount() == 0)
			return CC.CreateNil();

		pResult = CC.CreateLinkedList();
		for (i = 0; i < Attribs.GetCount(); i++)
			pResult->AppendString(CC, Attribs[i]);

		return pResult;
		}

	else if (strEquals(sProperty, PROPERTY_CLASS))
		return CC.CreateString(GetTypeClassName());

	else if (strEquals(sProperty, PROPERTY_EXTENSION))
		{
		if (m_pExtension)
			return CC.CreateInteger(m_pExtension->GetUNID());
		else
			return CC.CreateNil();
		}

    else if (strEquals(sProperty, PROPERTY_MAP_DESCRIPTION))
        return CC.CreateString(GetMapDescription(SMapDescriptionCtx()));

    else if (strEquals(sProperty, PROPERTY_MERGED))
        return CC.CreateBool(m_bIsMerged);

    else if (strEquals(sProperty, PROPERTY_NAME_PATTERN))
		{
		pResult = CC.CreateSymbolTable();
		DWORD dwFlags;
		pResult->SetStringAt(CC, CONSTLIT("pattern"), GetNamePattern(0, &dwFlags));
		pResult->SetIntegerAt(CC, CONSTLIT("flags"), dwFlags);
		return pResult;
		}

	//	Otherwise, we see if there is a data field

	else if (FindDataField(sProperty, &sValue))
		return CreateResultFromDataField(CC, sValue);

	//	Lastly, see if there is static data

	else if (m_pExtra && (m_pExtra->StaticData.FindDataAsItem(sProperty, pValue)))
		return pValue->Reference();

	//	Not found

	else
		return NULL;
	}

bool CDesignType::FindDataField (const CString &sField, CString *retsValue) const

//	FindDataField
//
//	Subclasses should call this method when they do not know the field.
//	[Normally we would do an OnFindDataField pattern, but we're too lazy
//	to change all the instances.]

	{
	int i;

	if (strEquals(sField, FIELD_ENTITY))
		*retsValue = GetEntityName();
	else if (strEquals(sField, FIELD_EXTENSION_UNID))
		*retsValue = strPatternSubst("0x%08x", (m_pExtension ? m_pExtension->GetUNID() : 0));
	else if (strEquals(sField, FIELD_NAME))
		*retsValue = strPatternSubst("%s 0x%08x", GetTypeClassName(), m_dwUNID);
	else if (strEquals(sField, FIELD_UNID))
		*retsValue = strPatternSubst("0x%08x", m_dwUNID);
	else if (strEquals(sField, FIELD_VERSION))
		{
		//	Starting in version 12 we start numbering api versions independently 
		//	of release version.

		if (m_dwVersion >= 12)
			*retsValue = strFromInt(m_dwVersion);
		else
			*retsValue = strPatternSubst("%d", ExtensionVersionToInteger(m_dwVersion));
		}
	else if (strStartsWith(sField, FIELD_ATTRIB_PREFIX))
		{
		CString sParam = strSubString(sField, FIELD_ATTRIB_PREFIX.GetLength());

		//	Parse into a list of attributes

		TArray<CString> Attribs;
		char *pPos = sParam.GetASCIIZPointer();
		char *pStart = pPos;
		while (true)
			{
			if (*pPos == '\0' || *pPos == '-')
				{
				CString sAttrib = CString(pStart, (int)(pPos - pStart));
				if (!sAttrib.IsBlank())
					Attribs.Insert(sAttrib);

				if (*pPos == '\0')
					break;
				else
					pStart = pPos + 1;
				}

			pPos++;
			}

		//	See which attribute we have

		CString sValue;
		for (i = 0; i < Attribs.GetCount(); i++)
			if (HasAttribute(Attribs[i]))
				{
				if (sValue.IsBlank())
					sValue = Attribs[i];
				else
					sValue = strPatternSubst(CONSTLIT("%s, %s"), sValue, Attribs[i]);
				}

		//	If not found, return false

		if (sValue.IsBlank())
			return false;

		*retsValue = sValue;
		}
	else
		return false;

	return true;
	}

bool CDesignType::FindEventHandler (const CString &sEvent, SEventHandlerDesc *retEvent) const

//	FindEventHandler
//
//	Returns an event handler

	{
	//	Ask subclasses

	if (OnFindEventHandler(sEvent, retEvent))
		return true;

	//	If we have it, great

	ICCItem *pCode;
	if (m_Events.FindEvent(sEvent, &pCode))
		{
		if (retEvent)
			{
			retEvent->pExtension = m_pExtension;
			retEvent->pCode = pCode;
			}

		return true;
		}

	//	Otherwise, see if we inherit

	if (m_pInheritFrom)
		return m_pInheritFrom->FindEventHandler(sEvent, retEvent);

	//	Otherwise, nothing

	return false;
	}

bool CDesignType::FindStaticData (const CString &sAttrib, ICCItemPtr &pData) const

//	FindStaticData
//
//	Returns static data

	{
	if (m_pExtra && m_pExtra->StaticData.FindDataAsItem(sAttrib, pData))
		return true;

	if (m_pInheritFrom)
		return m_pInheritFrom->FindStaticData(sAttrib, pData);

	return false;
	}

void CDesignType::FireCustomEvent (const CString &sEvent, ECodeChainEvents iEvent, ICCItem *pData, ICCItem **retpResult)

//	FireCustomEvent
//
//	Fires a custom event

	{
	CCodeChainCtx Ctx;

	SEventHandlerDesc Event;
	if (FindEventHandler(sEvent, &Event))
		{
		Ctx.SetEvent(iEvent);
		Ctx.SaveAndDefineDataVar(pData);

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(sEvent, pResult);

		//	Either return the event result or discard it

		if (retpResult)
			*retpResult = pResult;
		else
			Ctx.Discard(pResult);
		}
	else
		{
		if (retpResult)
			*retpResult = Ctx.CreateNil();
		}
	}

bool CDesignType::FireGetCreatePos (CSpaceObject *pBase, CSpaceObject *pTarget, CSpaceObject **retpGate, CVector *retvPos)

//	FireGetCreatePos
//
//	Fire GetCreatePos event

	{
	DEBUG_TRY

	SEventHandlerDesc Event;

	if (FindEventHandler(GET_CREATE_POS_EVENT, &Event))
		{
		CCodeChainCtx Ctx;

		Ctx.DefineSpaceObject(CONSTLIT("aBaseObj"), pBase);
		Ctx.DefineSpaceObject(CONSTLIT("aTargetObj"), pTarget);

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(GET_CREATE_POS_EVENT, pResult);

		bool bResult;
		if (pResult->IsInteger())
			{
			*retpGate = Ctx.AsSpaceObject(pResult);
			*retvPos = (*retpGate)->GetPos();
			bResult = true;
			}
		else if (pResult->IsList())
			{
			*retpGate = NULL;
			*retvPos = Ctx.AsVector(pResult);
			bResult = true;
			}
		else
			bResult = false;

		Ctx.Discard(pResult);
		return bResult;
		}

	return false;

	DEBUG_CATCH
	}

void CDesignType::FireGetGlobalAchievements (CGameStats &Stats)

//	FireGetGlobalAchievements
//
//	Fires GetGlobalAchievements event

	{
	int i;

	SEventHandlerDesc Event;
	if (FindEventHandler(GET_GLOBAL_ACHIEVEMENTS_EVENT, &Event))
		{
		CCodeChainCtx Ctx;

		//	Run code

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(GET_GLOBAL_ACHIEVEMENTS_EVENT, pResult);
		else if (pResult->IsNil())
			;
		else if (pResult->IsList())
			{
			//	If we have a list of lists, then we have 
			//	a list of achievements

			if (pResult->GetCount() > 0 && pResult->GetElement(0)->IsList())
				{
				for (i = 0; i < pResult->GetCount(); i++)
					{
					ICCItem *pAchievement = pResult->GetElement(i);
					if (pAchievement->GetCount() > 0)
						{
						CString sName = pAchievement->GetElement(0)->GetStringValue();
						CString sValue = ParseAchievementValue(pAchievement->GetElement(1));
						CString sSection = ParseAchievementSection(pAchievement->GetElement(2));
						CString sSort = ParseAchievementSort(pAchievement->GetElement(3));

						if (!sName.IsBlank())
							Stats.Insert(sName, sValue, sSection, sSort);
						}
					}
				}

			//	Otherwise, we have a single achievement

			else if (pResult->GetCount() > 0)
				{
				CString sName = pResult->GetElement(0)->GetStringValue();
				CString sValue = ParseAchievementValue(pResult->GetElement(1));
				CString sSection = ParseAchievementSection(pResult->GetElement(2));
				CString sSort = ParseAchievementSort(pResult->GetElement(3));

				if (!sName.IsBlank())
					Stats.Insert(sName, sValue, sSection, sSort);
				}
			}

		Ctx.Discard(pResult);
		}
	}

bool CDesignType::FireGetGlobalDockScreen (const SEventHandlerDesc &Event, CSpaceObject *pObj, CString *retsScreen, ICCItemPtr *retpData, int *retiPriority)

//	FireGetGlobalDockScreen
//
//	Asks a type to see if it wants to override an object's
//	dock screen.
//
//	NOTE: If we return TRUE and *retpData is non-NULL then the caller is responsible 
//	for discarding data.

	{
	//	Set up

	CCodeChainCtx Ctx;
	Ctx.SaveAndDefineSourceVar(pObj);

	//	Run

	ICCItemPtr pResult = Ctx.RunCode(Event);
	if (pResult->IsError())
		{
		ReportEventError(GET_GLOBAL_DOCK_SCREEN_EVENT, pResult);
		return false;
		}

	return CTLispConvert::AsScreen(pResult, retsScreen, retpData, retiPriority);
	}

bool CDesignType::FireGetGlobalPlayerPriceAdj (const SEventHandlerDesc &Event, STradeServiceCtx &ServiceCtx, ICCItem *pData, int *retiPriceAdj)

//	FireGetGlobalPlayerPriceAdj
//
//	Fires event to allow types to modify prices charged to the player.

	{
	CCodeChainCtx Ctx;

	//	Set up

	Ctx.SetEvent(eventGetGlobalPlayerPriceAdj);
	if (ServiceCtx.pItem)
		{
		Ctx.SaveAndDefineItemVar(*ServiceCtx.pItem);
		Ctx.SetItemType(ServiceCtx.pItem->GetType());
		}
	else if (ServiceCtx.pObj)
		{
		Ctx.DefineSpaceObject(CONSTLIT("aObj"), ServiceCtx.pObj);
		}

	Ctx.DefineString(CONSTLIT("aService"), CTradingDesc::ServiceToString(ServiceCtx.iService));
	Ctx.DefineSpaceObject(CONSTLIT("aProviderObj"), ServiceCtx.pProvider);
	if (pData)
		Ctx.SaveAndDefineDataVar(pData);

	//	Run

	ICCItem *pResult = Ctx.Run(Event);

	int iPriceAdj = 100;
	if (pResult->IsError())
		ReportEventError(GET_GLOBAL_PLAYER_PRICE_ADJ_EVENT, pResult);
	else if (pResult->IsNil())
		;
	else
		iPriceAdj = pResult->GetIntegerValue();

	//	Done

	Ctx.Discard(pResult);

	if (retiPriceAdj)
		*retiPriceAdj = iPriceAdj;

	return (iPriceAdj != 100);
	}

int CDesignType::FireGetGlobalResurrectPotential (void)

//	FireGetGlobalResurrectPotential
//
//	Returns the potential that this design type will resurrect the player
//	(0 = no chance; 100 = greatest potential)

	{
	int iResult = 0;

	SEventHandlerDesc Event;
	if (FindEventHandler(GET_GLOBAL_RESURRECT_POTENTIAL_EVENT, &Event))
		{
		CCodeChainCtx Ctx;

		//	Run code

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(GET_GLOBAL_RESURRECT_POTENTIAL_EVENT, pResult);
		else
			iResult = pResult->GetIntegerValue();

		Ctx.Discard(pResult);
		}

	return iResult;
	}

void CDesignType::FireObjCustomEvent (const CString &sEvent, CSpaceObject *pObj, ICCItem **retpResult)

//	FireObjCustomEvent
//
//	Fires a named event and optionally returns result

	{
	CCodeChainCtx Ctx;

	SEventHandlerDesc Event;
	if (FindEventHandler(sEvent, &Event))
		{
		Ctx.SaveAndDefineSourceVar(pObj);

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			pObj->ReportEventError(sEvent, pResult);

		//	Either return the event result or discard it

		if (retpResult)
			*retpResult = pResult;
		else
			Ctx.Discard(pResult);
		}
	else
		{
		if (retpResult)
			*retpResult = Ctx.CreateNil();
		}
	}

ALERROR CDesignType::FireOnGlobalDockPaneInit (const SEventHandlerDesc &Event, void *pScreen, DWORD dwScreenUNID, const CString &sScreen, const CString &sPane, CString *retsError)

//	FireOnGlobalDockPaneInit
//
//	Dock pane initialized

	{
	CCodeChainCtx Ctx;

	//	Set up

	Ctx.SetScreen(pScreen);
	Ctx.DefineInteger(CONSTLIT("aScreenUNID"), dwScreenUNID);
	Ctx.DefineString(CONSTLIT("aScreen"), sScreen);
	Ctx.DefineString(CONSTLIT("aPane"), sPane);

	//	Run

	ICCItem *pResult = Ctx.Run(Event);
	if (pResult->IsError())
		ReportEventError(ON_GLOBAL_DOCK_PANE_INIT_EVENT, pResult);

	//	Done

	Ctx.Discard(pResult);
	return NOERROR;
	}

void CDesignType::FireOnGlobalIntroCommand(const SEventHandlerDesc &Event, const CString &sCommand)

//	FireOnGlobalIntroCommand
//
//	Fire event

{
	CCodeChainCtx Ctx;
	Ctx.DefineString(CONSTLIT("aCommand"), sCommand);

	//	Run code

	ICCItem *pResult = Ctx.Run(Event);
	if (pResult->IsError())
		ReportEventError(ON_GLOBAL_INTRO_COMMAND_EVENT, pResult);

	//	Done

	Ctx.Discard(pResult);
}

void CDesignType::FireOnGlobalIntroStarted (const SEventHandlerDesc &Event)

//	FireOnGlobalIntroStarted
//
//	Fire event

{
	CCodeChainCtx Ctx;

	//	Run code

	ICCItem *pResult = Ctx.Run(Event);
	if (pResult->IsError())
		ReportEventError(ON_GLOBAL_INTRO_STARTED_EVENT, pResult);

	//	Done

	Ctx.Discard(pResult);
}

void CDesignType::FireOnGlobalPlayerBoughtItem (const SEventHandlerDesc &Event, CSpaceObject *pSellerObj, const CItem &Item, const CCurrencyAndValue &Price)

//	FireOnGlobalPlayerBoughtItem
//
//	Player bought an item

	{
	CCodeChainCtx Ctx;

	//	Set up

	Ctx.DefineSpaceObject(CONSTLIT("aSellerObj"), pSellerObj);
	Ctx.SaveAndDefineItemVar(Item);
	Ctx.DefineString(CONSTLIT("aCurrency"), Price.GetCurrencyType()->GetSID());
	Ctx.DefineInteger(CONSTLIT("aPrice"), (int)Price.GetValue());

	//	Run

	ICCItem *pResult = Ctx.Run(Event);
	if (pResult->IsError())
		ReportEventError(ON_GLOBAL_PLAYER_BOUGHT_ITEM_EVENT, pResult);

	Ctx.Discard(pResult);
	}

void CDesignType::FireOnGlobalPlayerSoldItem (const SEventHandlerDesc &Event, CSpaceObject *pBuyerObj, const CItem &Item, const CCurrencyAndValue &Price)

//	FireOnGlobalPlayerSoldItem
//
//	Player sold an item

	{
	CCodeChainCtx Ctx;

	//	Set up

	Ctx.DefineSpaceObject(CONSTLIT("aBuyerObj"), pBuyerObj);
	Ctx.SaveAndDefineItemVar(Item);
	Ctx.DefineString(CONSTLIT("aCurrency"), Price.GetCurrencyType()->GetSID());
	Ctx.DefineInteger(CONSTLIT("aPrice"), (int)Price.GetValue());

	//	Run

	ICCItem *pResult = Ctx.Run(Event);
	if (pResult->IsError())
		ReportEventError(ON_GLOBAL_PLAYER_SOLD_ITEM_EVENT, pResult);

	Ctx.Discard(pResult);
	}

void CDesignType::FireOnGlobalEndDiagnostics (const SEventHandlerDesc &Event)

//	FireOnGlobalEndDiagnostics
//
//	Fires OnGlobalEndDiagnostics

	{
	CCodeChainCtx CCCtx;

	//	Run code

	ICCItem *pResult = CCCtx.Run(Event);
	if (pResult->IsError())
		ReportEventError(ON_GLOBAL_END_DIAGNOSTICS_EVENT, pResult);

	CCCtx.Discard(pResult);
	}

void CDesignType::FireOnGlobalMarkImages (const SEventHandlerDesc &Event)

//	FireOnGlobalMarkImages
//
//	Fires OnGlobalMarkImages

	{
	CCodeChainCtx CCCtx;

	//	Run code

	ICCItem *pResult = CCCtx.Run(Event);
	if (pResult->IsError())
		ReportEventError(ON_GLOBAL_MARK_IMAGES_EVENT, pResult);

	CCCtx.Discard(pResult);
	}

void CDesignType::FireOnGlobalObjDestroyed (const SEventHandlerDesc &Event, SDestroyCtx &Ctx)

//	FireOnGlobalObjDestroyed
//
//	Fires OnGlobalObjDestroyed

	{
	CCodeChainCtx CCCtx;

	CCCtx.DefineSpaceObject(CONSTLIT("aObjDestroyed"), Ctx.pObj);
	CCCtx.DefineSpaceObject(CONSTLIT("aDestroyer"), Ctx.Attacker.GetObj());
	CCCtx.DefineSpaceObject(CONSTLIT("aOrderGiver"), Ctx.GetOrderGiver());
	CCCtx.DefineSpaceObject(CONSTLIT("aWreckObj"), Ctx.pWreck);
	CCCtx.DefineString(CONSTLIT("aDestroyReason"), GetDestructionName(Ctx.iCause));

	//	Run code

	ICCItem *pResult = CCCtx.Run(Event);
	if (pResult->IsError())
		ReportEventError(ON_GLOBAL_OBJ_DESTROYED_EVENT, pResult);

	CCCtx.Discard(pResult);
	}

ALERROR CDesignType::FireOnGlobalPlayerChangedShips (CSpaceObject *pOldShip, CString *retsError)

//	FireOnGlobalPlayerChangedShips
//
//	Player changed ships

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_GLOBAL_PLAYER_CHANGED_SHIPS_EVENT, &Event))
		{
		CCodeChainCtx Ctx;

		Ctx.DefineSpaceObject(CONSTLIT("aOldPlayerShip"), pOldShip);

		//	Run code

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_GLOBAL_PLAYER_CHANGED_SHIPS_EVENT, pResult);

		Ctx.Discard(pResult);
		}

	return NOERROR;
	}

ALERROR CDesignType::FireOnGlobalPlayerEnteredSystem (CString *retsError)

//	FireOnGlobalPlayerEnteredSystem
//
//	Player entered the system

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_GLOBAL_PLAYER_ENTERED_SYSTEM_EVENT, &Event))
		{
		CCodeChainCtx Ctx;

		//	Run code

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_GLOBAL_PLAYER_ENTERED_SYSTEM_EVENT, pResult);

		Ctx.Discard(pResult);
		}

	return NOERROR;
	}

ALERROR CDesignType::FireOnGlobalPlayerLeftSystem (CString *retsError)

//	FireOnGlobalPlayerLeftSystem
//
//	Player left the system

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_GLOBAL_PLAYER_LEFT_SYSTEM_EVENT, &Event))
		{
		CCodeChainCtx Ctx;

		//	Run code

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_GLOBAL_PLAYER_LEFT_SYSTEM_EVENT, pResult);

		Ctx.Discard(pResult);
		}

	return NOERROR;
	}

ALERROR CDesignType::FireOnGlobalResurrect (CString *retsError)

//	FireOnGlobalResurrect
//
//	Resurrection code invoked

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_GLOBAL_RESURRECT_EVENT, &Event))
		{
		CCodeChainCtx Ctx;

		//	Run code

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_GLOBAL_RESURRECT_EVENT, pResult);

		Ctx.Discard(pResult);
		}

	return NOERROR;
	}

void CDesignType::FireOnGlobalStartDiagnostics (const SEventHandlerDesc &Event)

//	FireOnGlobalStartDiagnostics
//
//	Fires OnGlobalStartDiagnostics

	{
	CCodeChainCtx CCCtx;

	//	Run code

	ICCItem *pResult = CCCtx.Run(Event);
	if (pResult->IsError())
		ReportEventError(ON_GLOBAL_START_DIAGNOSTICS_EVENT, pResult);

	CCCtx.Discard(pResult);
	}

void CDesignType::FireOnGlobalSystemDiagnostics (const SEventHandlerDesc &Event)

//	FireOnGlobalSystemDiagnostics
//
//	Fires OnGlobalSystemDiagnostics

	{
	CCodeChainCtx CCCtx;

	//	Run code

	ICCItem *pResult = CCCtx.Run(Event);
	if (pResult->IsError())
		ReportEventError(ON_GLOBAL_SYSTEM_DIAGNOSTICS_EVENT, pResult);

	CCCtx.Discard(pResult);
	}

ALERROR CDesignType::FireOnGlobalSystemCreated (SSystemCreateCtx &SysCreateCtx, CString *retsError)

//	FireOnGlobalSystemCreated
//
//	Fire event

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_GLOBAL_SYSTEM_CREATED_EVENT, &Event))
		{
		CCodeChainCtx Ctx;
		Ctx.SetSystemCreateCtx(&SysCreateCtx);

		//	Run code

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_GLOBAL_SYSTEM_CREATED_EVENT, pResult);

		Ctx.Discard(pResult);
		}

	return NOERROR;
	}

void CDesignType::FireOnGlobalSystemStarted (const SEventHandlerDesc &Event, DWORD dwElapsedTime)

//	FireOnGlobalSystemStarted
//
//	System has started

	{
	CCodeChainCtx CCCtx;
	CCCtx.DefineInteger(CONSTLIT("aElapsedTime"), dwElapsedTime);

	//	Run code

	ICCItem *pResult = CCCtx.Run(Event);
	if (pResult->IsError())
		ReportEventError(ON_GLOBAL_SYSTEM_STARTED_EVENT, pResult);

	CCCtx.Discard(pResult);
	}

void CDesignType::FireOnGlobalSystemStopped (const SEventHandlerDesc &Event)

//	FireOnGlobalSystemStopped
//
//	System has stopped

	{
	CCodeChainCtx CCCtx;

	//	Run code

	ICCItem *pResult = CCCtx.Run(Event);
	if (pResult->IsError())
		ReportEventError(ON_GLOBAL_SYSTEM_STOPPED_EVENT, pResult);

	CCCtx.Discard(pResult);
	}

ALERROR CDesignType::FireOnGlobalTopologyCreated (CString *retsError)

//	FireOnGlobalTopologyCreated
//
//	Fire event

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_GLOBAL_TOPOLOGY_CREATED_EVENT, &Event))
		{
		CCodeChainCtx Ctx;

		//	Run code

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_GLOBAL_TOPOLOGY_CREATED_EVENT, pResult);

		Ctx.Discard(pResult);
		}

	return NOERROR;
	}

ALERROR CDesignType::FireOnGlobalTypesInit (SDesignLoadCtx &Ctx)

//	FireOnGlobalTypesInit
//
//	Give this type a chance to create dynamic types.

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(evtOnGlobalTypesInit, &Event))
		{
		CCodeChainCtx CCCtx;
		CCCtx.SetEvent(eventOnGlobalTypesInit);

		ICCItem *pResult = CCCtx.Run(Event);
		if (pResult->IsError())
			{
			Ctx.sError = strPatternSubst(CONSTLIT("OnGlobalTypesInit (%x): %s"), GetUNID(), pResult->GetStringValue());
			return ERR_FAIL;
			}

		CCCtx.Discard(pResult);
		}

	return NOERROR;
	}

ALERROR CDesignType::FireOnGlobalUniverseCreated (const SEventHandlerDesc &Event)

//	FireOnGlobalUniverseCreated
//
//	Fire event

	{
	CCodeChainCtx Ctx;

	//	Run code

	ICCItem *pResult = Ctx.Run(Event);
	if (pResult->IsError())
		ReportEventError(ON_GLOBAL_UNIVERSE_CREATED_EVENT, pResult);

	//	Done

	Ctx.Discard(pResult);
	return NOERROR;
	}

ALERROR CDesignType::FireOnGlobalUniverseLoad (const SEventHandlerDesc &Event)

//	FireOnGlobalUniverseLoad
//
//	Fire event

	{
	CCodeChainCtx Ctx;
	if (g_pUniverse->InResurrectMode())
		Ctx.DefineString(CONSTLIT("aReason"), CONSTLIT("resurrect"));
	else
		Ctx.DefineNil(CONSTLIT("aReason"));

	//	Run code

	ICCItem *pResult = Ctx.Run(Event);
	if (pResult->IsError())
		ReportEventError(ON_GLOBAL_UNIVERSE_LOAD_EVENT, pResult);

	//	Done

	Ctx.Discard(pResult);
	return NOERROR;
	}

ALERROR CDesignType::FireOnGlobalUniverseSave (const SEventHandlerDesc &Event)

//	FireOnGlobalUniverseSave
//
//	Fire event

	{
	CCodeChainCtx Ctx;

	//	Run code

	ICCItem *pResult = Ctx.Run(Event);
	if (pResult->IsError())
		ReportEventError(ON_GLOBAL_UNIVERSE_SAVE_EVENT, pResult);

	//	Done

	Ctx.Discard(pResult);
	return NOERROR;
	}

void CDesignType::FireOnGlobalUpdate (const SEventHandlerDesc &Event)

//	FireOnGlobalUpdate
//
//	Fire event

	{
	CCodeChainCtx Ctx;

	//	Run code

	ICCItem *pResult = Ctx.Run(Event);
	if (pResult->IsError())
		ReportEventError(ON_GLOBAL_UPDATE_EVENT, pResult);

	//	Done

	Ctx.Discard(pResult);
	}

void CDesignType::FireOnRandomEncounter (CSpaceObject *pObj)

//	FireOnRandomEncounter
//
//	Fire OnRandomEncounter event

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_RANDOM_ENCOUNTER_EVENT, &Event))
		{
		CCodeChainCtx Ctx;

		Ctx.SaveAndDefineSourceVar(pObj);

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_RANDOM_ENCOUNTER_EVENT, pResult);

		Ctx.Discard(pResult);
		}
	}

DWORDLONG CDesignType::GetAllocMemoryUsage (void) const

//	GetAllocMemoryUsage
//
//	Returns the total memory allocated by CDesignType, not counting any memory
//	allocated by derived classes, and not counting memory for CDesignType class
//	itself.

	{
	DWORDLONG dwTotal = 0;

	if (m_pExtra)
		dwTotal += sizeof(SExtra);

	//	LATER: Include allocated memory from member variables.

	return dwTotal;
	}

CString CDesignType::GetEntityName (void) const

//	GetEntityName
//
//	Returns the entity name of this type

	{
	//	If we don't have an extension, then we can't figure it out

	if (m_pExtension == NULL)
		return NULL_STR;

	//	Ask the extension

	return m_pExtension->GetEntityName(GetUNID());
	}

ICCItem *CDesignType::GetEventHandler (const CString &sEvent) const

//	GetEventHandler
//
//	Returns an event handler (or NULL)

	{
	SEventHandlerDesc Event;
	if (FindEventHandler(sEvent, &Event))
		return Event.pCode;
	else
		return NULL;
	}

ICCItemPtr CDesignType::GetGlobalData (const CString &sAttrib) const

//	GetGlobalData
//
//	Returns global data

	{
	if (m_pExtra)
		return m_pExtra->GlobalData.GetDataAsItem(sAttrib);

	return ICCItemPtr(g_pUniverse->GetCC().CreateNil());
	}

SEventHandlerDesc *CDesignType::GetInheritedCachedEvent (ECachedHandlers iEvent) const

//	GetInheritedCachedEvent
//
//	Returns the event handler, inheriting from parents, if necessary.

	{
	if (m_pExtra && m_pExtra->EventsCache[iEvent].pCode)
		return &m_pExtra->EventsCache[iEvent];
	else if (m_pInheritFrom)
		return m_pInheritFrom->GetInheritedCachedEvent(iEvent);
	else
		return NULL;
	}

CString CDesignType::GetMapDescription (SMapDescriptionCtx &Ctx) const

//  GetMapDescription
//
//  Returns a description of the type, suitable for a summary. We look at the
//  services provided. If we cannot come up with a suitable description, we
//  return NULL_STR.

    {
    CString sDesc;

    //  If we're abandoned, then we go through a totally different path.

    if (Ctx.bShowDestroyed)
        {
        //  If we've got a specific language element, use that.

        if (!TranslateText(NULL, LANGID_CORE_MAP_DESC_ABANDONED, NULL, &sDesc))
            sDesc = CONSTLIT("Abandoned");

        return sDesc;
        }

    //  If we have a specific language element for this, then we return it.

    else if (TranslateText(NULL, LANGID_CORE_MAP_DESC, NULL, &sDesc))
        return sDesc;

    //  Otherwise, check the trade descriptors and compose our own.

    else
        {
        //  See if we have a language element for the main service. If we don't,
        //  ask the subclass

        CString sMainDesc;
        if (!TranslateText(NULL, LANGID_CORE_MAP_DESC_MAIN, NULL, &sMainDesc))
            sMainDesc = OnGetMapDescriptionMain(Ctx);

        //  Get the trade descriptor

        CString sTradeDesc;
        CTradingDesc *pTrade = GetTradingDesc();
        if (pTrade)
            pTrade->ComposeDescription(&sTradeDesc);

        //  If we have both main and trade descriptors, combine them.

        if (!sMainDesc.IsBlank() && !sTradeDesc.IsBlank())
            return strPatternSubst(CONSTLIT("%s — %s"), sMainDesc, sTradeDesc);

        //  If all we have is main desc, return that.

        else if (!sMainDesc.IsBlank())
            return sMainDesc;

        //  If all we have is trade desc, return that.

        else if (!sTradeDesc.IsBlank())
            return sTradeDesc;

        //  Otherwise, we have neither and we need to compose it. If we're an 
        //  enemy, say so.

        else if (Ctx.bEnemy)
            return CONSTLIT("Hostile");

        //  Otherwise, we report no services.

        else
            return CONSTLIT("No services available");
        }
    }

CCurrencyAndValue CDesignType::GetTradePrice (CSpaceObject *pObj, bool bActual) const

//	GetTradePrice
//
//	Default implementation.

	{
	return CCurrencyAndValue();
	}

TSortMap<DWORD, DWORD> CDesignType::GetXMLMergeFlags (void) const

//	GetXMLMergeFlags
//
//	Returns merge flags to determine how to merge two XML elements of the given
//	type.

	{
	TSortMap<DWORD, DWORD> MergeFlags;

	//	The following attributes are never inherited.

	MergeFlags.SetAt(CXMLElement::GetKeywordID(CONSTLIT("attrib.inherit")), CXMLElement::MERGE_OVERRIDE);
	MergeFlags.SetAt(CXMLElement::GetKeywordID(CONSTLIT("attrib.unid")), CXMLElement::MERGE_OVERRIDE);
	MergeFlags.SetAt(CXMLElement::GetKeywordID(CONSTLIT("attrib.virtual")), CXMLElement::MERGE_OVERRIDE);

	//	These elements are never merged because we can handle the inheritance
	//	hierarchy.

	MergeFlags.SetAt(CXMLElement::GetKeywordID(EVENTS_TAG), CXMLElement::MERGE_OVERRIDE);
	MergeFlags.SetAt(CXMLElement::GetKeywordID(STATIC_DATA_TAG), CXMLElement::MERGE_OVERRIDE);
	MergeFlags.SetAt(CXMLElement::GetKeywordID(DOCK_SCREENS_TAG), CXMLElement::MERGE_OVERRIDE);
	MergeFlags.SetAt(CXMLElement::GetKeywordID(LANGUAGE_TAG), CXMLElement::MERGE_OVERRIDE);
	MergeFlags.SetAt(CXMLElement::GetKeywordID(ATTRIBUTE_DESC_TAG), CXMLElement::MERGE_OVERRIDE);
	MergeFlags.SetAt(CXMLElement::GetKeywordID(DISPLAY_ATTRIBUTES_TAG), CXMLElement::MERGE_OVERRIDE);

	//	Let our sub-class add their own flags.

	OnAccumulateXMLMergeFlags(MergeFlags);

	//	Done

	return MergeFlags;
	}

CString CDesignType::GetNounPhrase (DWORD dwFlags) const

//  GetNounPhrase
//
//  Composes a noun phrase
    
    {
    DWORD dwNameFlags;
    CString sName = GetNamePattern(dwFlags, &dwNameFlags); 
    return CLanguage::ComposeNounPhrase(sName, 1, NULL_STR, dwNameFlags, dwFlags);
    }

ICCItem *CDesignType::GetProperty (CCodeChainCtx &Ctx, const CString &sProperty) const

//	GetProperty
//
//	Returns the value of the given property. We return an allocated CC item (which 
//	must be discarded by the caller).

	{
	CCodeChain &CC = g_pUniverse->GetCC();
	ICCItem *pResult;
	ICCItemPtr pResultPtr;

	//	Let our subclass handle this first

	if (pResultPtr = OnGetProperty(Ctx, sProperty))
		return pResultPtr->Reference();

	//	If not, then see if we handle it.

	else if (pResult = FindBaseProperty(Ctx, sProperty))
		return pResult;

	//	Nobody handled it, so just return Nil

	else
		return CC.CreateNil();
	}

int CDesignType::GetPropertyInteger (const CString &sProperty)

//	GetPropertyInteger
//
//	Returns an integer property

	{
	CCodeChain &CC = g_pUniverse->GetCC();
	CCodeChainCtx Ctx;

	ICCItem *pItem = GetProperty(Ctx, sProperty);
	if (pItem == NULL || pItem->IsNil())
		return 0;

	int iResult = pItem->GetIntegerValue();
	pItem->Discard(&CC);
	return iResult;
	}

CString CDesignType::GetPropertyString (const CString &sProperty)

//	GetPropertyString
//
//	Returns a string property

	{
	CCodeChain &CC = g_pUniverse->GetCC();
	CCodeChainCtx Ctx;

	ICCItem *pItem = GetProperty(Ctx, sProperty);
	if (pItem == NULL || pItem->IsNil())
		return NULL_STR;

	CString sResult = pItem->GetStringValue();
	pItem->Discard(&CC);
	return sResult;
	}

bool CDesignType::HasAllUNIDs (const TArray<DWORD> &DesiredUNIDs, const TArray<DWORD> &AvailableUNIDs)

//	HasAllUNIDs
//
//	Returns TRUE if ALL of the UNIDs in DesiredUNIDs are in AvailableUNIDs.

	{
	int i;

	for (i = 0; i < DesiredUNIDs.GetCount(); i++)
		if (!AvailableUNIDs.Find(DesiredUNIDs[i]))
			return false;

	return true;
	}

bool CDesignType::HasAnyUNIDs (const TArray<DWORD> &DesiredUNIDs, const TArray<DWORD> &AvailableUNIDs)

//	HasAnyUNIDs
//
//	Returns TRUE if ANY of the UNIDs in DesiredUNIDs are in AvailableUNIDs.

	{
	int i;

	for (i = 0; i < DesiredUNIDs.GetCount(); i++)
		if (AvailableUNIDs.Find(DesiredUNIDs[i]))
			return true;

	return false;
	}

bool CDesignType::InheritsFrom (DWORD dwUNID) const

//	InheritsFrom
//
//	Returns TRUE if we inherit from the given UNID

	{
	if (m_pInheritFrom == NULL)
		return false;

	if (m_pInheritFrom->GetUNID() == dwUNID)
		return true;

	//	Recurse

	return m_pInheritFrom->InheritsFrom(dwUNID);
	}

bool CDesignType::IsIncluded (const TArray<DWORD> &ExtensionsIncluded) const

//	IsIncluded
//
//	Returns TRUE if this type needs to be included (bound) in a game with the 
//	given set of extensions/libraries.

	{
	//	If no extra block, then this is a standard type with no special
	//	instructions.

	if (!m_pExtra)
		return true;

	//	If this type has a list of required UNIDs, then make sure they exist.

	if (m_pExtra->Extends.GetCount() > 0 
			&& !HasAllUNIDs(m_pExtra->Extends, ExtensionsIncluded))
		return false;

	//	If this type has a list of excluded UNIDs, then make sure none of the
	//	listed extensions are loaded.

	if (m_pExtra->Excludes.GetCount() > 0
			&& HasAnyUNIDs(m_pExtra->Excludes, ExtensionsIncluded))
		return false;

	//	If we get this far, then include the type.

	return true;
	}

bool CDesignType::IsValidLoadXML (const CString &sTag)

//	IsValidLoadXML
//
//	Returns TRUE if the given XML element tag is a valid tag for all design
//	types.

	{
	return (strEquals(sTag, EVENTS_TAG)
			|| strEquals(sTag, DOCK_SCREENS_TAG)
			|| strEquals(sTag, GLOBAL_DATA_TAG)
			|| strEquals(sTag, LANGUAGE_TAG)
			|| strEquals(sTag, STATIC_DATA_TAG)
			|| strEquals(sTag, DISPLAY_ATTRIBUTES_TAG)
			|| strEquals(sTag, ATTRIBUTE_DESC_TAG));
	}

void CDesignType::AddUniqueHandlers (TSortMap<CString, SEventHandlerDesc> *retInheritedHandlers)

//	AddUniqueHandlers
//
//	Adds this type's handlers to the given array (as long as they are not 
//	already there).

	{
	int i;

	for (i = 0; i < m_Events.GetCount(); i++)
		{
		ICCItem *pCode;
		const CString &sEvent = m_Events.GetEvent(i, &pCode);

		if (!retInheritedHandlers->Find(sEvent))
			{
			SEventHandlerDesc *pDesc = retInheritedHandlers->Insert(sEvent);
			pDesc->pExtension = m_pExtension;
			pDesc->pCode = pCode;
			}
		}

	//	Add base classes

	if (m_pInheritFrom)
		m_pInheritFrom->AddUniqueHandlers(retInheritedHandlers);
	}

CEconomyType *CDesignType::GetEconomyType (void) const

//	GetEconomyType
//
//	Returns the economy type used by this type. Subclasses should override this
//	if necessary.

	{
	CTradingDesc *pTrade = GetTradingDesc();
	if (pTrade)
		return pTrade->GetEconomyType();

	return CEconomyType::AsType(g_pUniverse->FindDesignType(DEFAULT_ECONOMY_UNID));
	}

void CDesignType::GetEventHandlers (const CEventHandler **retpHandlers, TSortMap<CString, SEventHandlerDesc> *retInheritedHandlers)

//	GetEventHandlers
//
//	If this type has base classes, then it returns a list of events in the 
//	retInheritedHandlers array. Otherwise, retInheritedHandlers is undefined and
//	retpHandlers points to a CEventHandler object with all handlers
//	(which are all defined by the type).

	{
	//	If we don't inherit from anything, then we are the only ones who define
	//	events.

	if (m_pInheritFrom == NULL)
		{
		*retpHandlers = &m_Events;
		return;
		}

	//	Otherwise, we need to build the array of handlers.

	*retpHandlers = NULL;
	retInheritedHandlers->DeleteAll();
	AddUniqueHandlers(retInheritedHandlers);
	}

CXMLElement *CDesignType::GetScreen (const CString &sUNID)

//	GetScreen
//
//	Returns the given screen (either globally or from the local screens)

	{
	CDockScreenTypeRef Screen;
	Screen.LoadUNID(sUNID);
	Screen.Bind(GetLocalScreens());
	return Screen.GetDesc();
	}

ICCItemPtr CDesignType::GetStaticData (const CString &sAttrib) const

//	GetStaticData
//
//	Returns static data
	
	{
	ICCItemPtr pResult;
	if (m_pExtra && m_pExtra->StaticData.FindDataAsItem(sAttrib, pResult))
		return pResult;

	if (m_pInheritFrom)
		return m_pInheritFrom->GetStaticData(sAttrib);

	return ICCItemPtr(g_pUniverse->GetCC().CreateNil());
	}

CString CDesignType::GetTypeChar (DesignTypes iType)

//	GetTypeChar
//
//	Returns the character associated with the given type

	{
	return CString(&DESIGN_CHAR[iType], 1);
	}

CString CDesignType::GetTypeClassName (void) const

//	GetTypeClassName
//
//	Returns the class name of the type

	{
	return CString(DESIGN_CLASS_NAME[GetType()]);
	}

const CCompositeImageDesc &CDesignType::GetTypeImage (void) const

//  GetTypeImage
//
//  Default implementation
    
    {
    return CCompositeImageDesc::Null();
    }

bool CDesignType::HasAttribute (const CString &sAttrib) const

//	HasAttribute
//
//	Returns TRUE if we have the literal or special attribute.

	{
	if (HasLiteralAttribute(sAttrib))
		return true;

	return HasSpecialAttribute(sAttrib);
	}

bool CDesignType::HasSpecialAttribute (const CString &sAttrib) const

//	HasSpecialAttribute
//
//	Returns TRUE if we have the special attribute

	{
	if (strStartsWith(sAttrib, SPECIAL_EVENT))
		{
		CString sEvent = strSubString(sAttrib, SPECIAL_EVENT.GetLength());
		return FindEventHandler(sEvent);
		}
	else if (strStartsWith(sAttrib, SPECIAL_EXTENSION))
		{
		DWORD dwUNID = strToInt(strSubString(sAttrib, SPECIAL_EXTENSION.GetLength()), 0);
		return (m_pExtension && (m_pExtension->GetUNID() == dwUNID));
		}
	else if (strStartsWith(sAttrib, SPECIAL_INHERIT))
		{
		DWORD dwUNID = strToInt(strSubString(sAttrib, SPECIAL_INHERIT.GetLength()), 0);
		return InheritsFrom(dwUNID);
		}
	else if (strStartsWith(sAttrib, SPECIAL_PROPERTY))
		{
		CString sProperty = strSubString(sAttrib, SPECIAL_PROPERTY.GetLength());

		CCodeChainCtx Ctx;
		ICCItem *pValue = GetProperty(Ctx, sProperty);
		if (pValue == NULL)
			return false;
		else
			{
			bool bResult = !pValue->IsNil();
			pValue->Discard(&g_pUniverse->GetCC());
			return bResult;
			}
		}
	else if (strStartsWith(sAttrib, SPECIAL_SYSTEM_LEVEL))
		{
		//	Must have a current system.

		CSystem *pSystem = g_pUniverse->GetCurrentSystem();
		if (pSystem == NULL)
			return false;

		int iSystemLevel = pSystem->GetLevel();

		//	Parse from and to

		CString sParam = strSubString(sAttrib, SPECIAL_SYSTEM_LEVEL.GetLength());
		char *pPos = sParam.GetASCIIZPointer();
		bool bFailed;
		int iFrom = strParseInt(pPos, 0, &pPos, &bFailed);
		if (bFailed)
			return false;

		//	Get this type's min/max level

		int iMinLevel, iMaxLevel;
		GetLevel(&iMinLevel, &iMaxLevel);

		//	If we only have a single value, then we offset from the system level.

		if (*pPos != '-')
			{
			int iOffsetLevel = iSystemLevel + iFrom;
			return ((iMaxLevel >= iOffsetLevel) && (iMinLevel <= iOffsetLevel));
			}

		//	Otherwise, we have two values.

		else
			{
			pPos++;
			int iTo = strParseInt(pPos, 0, NULL, &bFailed);
			if (bFailed)
				return false;

			//	Can't have the lower-bound be less than the upper bound.

			if (iFrom > iTo)
				return false;

			return ((iMaxLevel >= iSystemLevel + iFrom) && (iMinLevel <= iSystemLevel + iTo));
			}
		}
	else if (strStartsWith(sAttrib, SPECIAL_UNID))
		{
		DWORD dwUNID = strToInt(strSubString(sAttrib, SPECIAL_UNID.GetLength()), 0);
		return (GetUNID() == dwUNID);
		}
	else
		return OnHasSpecialAttribute(sAttrib);
	}

void CDesignType::InitCachedEvents (void)

//	InitCachedEvents
//
//	Enumerates events and adds the appropriate ones to the cache

	{
	int i, j;

	for (i = 0; i < m_Events.GetCount(); i++)
		{
		ICCItem *pCode;
		const CString &sEvent = m_Events.GetEvent(i, &pCode);

		for (j = 0; j < evtCount; j++)
			{
			if (strEquals(sEvent, CString(CACHED_EVENTS[j], -1, true)))
				{
				SetExtra()->EventsCache[j].pExtension = m_pExtension;
				SetExtra()->EventsCache[j].pCode = pCode;
				break;
				}
			}
		}
	}

void CDesignType::InitCachedEvents (int iCount, char **pszEvents, SEventHandlerDesc *retEvents)

//	InitCachedEvents
//
//	Initializes events cached by subclassess.

	{
	int i;

	for (i = 0; i < iCount; i++)
		{
		if (!FindEventHandler(CString(pszEvents[i], -1, true), &retEvents[i]))
			{
			retEvents[i].pExtension = NULL;
			retEvents[i].pCode = NULL;
			}
		}
	}

ALERROR CDesignType::InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, bool bIsOverride)

//	InitFromXML
//
//	Creates a design type from an XML element

	{
	ALERROR error;
	int i;

	//	Remember the type we're loading

	Ctx.pType = this;

	//	Load UNID

	if (error = ::LoadUNID(Ctx, pDesc->GetAttribute(UNID_ATTRIB), &m_dwUNID))
		return error;

	if (m_dwUNID == 0)
		{
		Ctx.sError = strPatternSubst(CONSTLIT("<%s> must have a valid UNID."), pDesc->GetTag());
		Ctx.pType = NULL;
		return ERR_FAIL;
		}

	//	Extension information

	m_pExtension = Ctx.pExtension;
	m_dwVersion = Ctx.GetAPIVersion();

	//	Required libraries

	CString sExtends = pDesc->GetAttribute(EXTENDS_ATTRIB);
	if (!sExtends.IsBlank())
		ParseUNIDList(sExtends, 0, &SetExtra()->Extends);

	//	Excluded libraries

	CString sExcludes = pDesc->GetAttribute(EXCLUDES_ATTRIB);
	if (!sExcludes.IsBlank())
		ParseUNIDList(sExcludes, 0, &SetExtra()->Excludes);

	//	Remember XML; CExtension will always keep a copy of the XML (to which
	//	we're pointing) and we will get called appropriately if we need to 
	//	invalidate.

	m_pXML = pDesc;

	//	Inheritance

	m_bIsModification = bIsOverride;

	if (error = ::LoadUNID(Ctx, pDesc->GetAttribute(INHERIT_ATTRIB), &m_dwInheritFrom))
		{
		Ctx.pType = NULL;
		return error;
		}

	m_pInheritFrom = NULL;

	//	Obsolete

	m_dwObsoleteVersion = pDesc->GetAttributeIntegerBounded(OBSOLETE_ATTRIB, 0, -1, 0);

	//	Load attributes

	if (!pDesc->FindAttribute(ATTRIBUTES_ATTRIB, &m_sAttributes))
		m_sAttributes = pDesc->GetAttribute(MODIFIERS_ATTRIB);

	//	Load various elements

	for (i = 0; i < pDesc->GetContentElementCount(); i++)
		{
		CXMLElement *pItem = pDesc->GetContentElement(i);

		if (strEquals(pItem->GetTag(), EVENTS_TAG))
			{
			if (error = m_Events.InitFromXML(Ctx, pItem))
				{
				Ctx.pType = NULL;
				return ComposeLoadError(Ctx, Ctx.sError);
				}

			InitCachedEvents();
			}
		else if (strEquals(pItem->GetTag(), STATIC_DATA_TAG))
			SetExtra()->StaticData.SetFromXML(pItem);
		else if (strEquals(pItem->GetTag(), GLOBAL_DATA_TAG)
				|| ((GetType() == designSovereign || GetType() == designGenericType)
						&& strEquals(pItem->GetTag(), INITIAL_DATA_TAG)))
			{
			SetExtra()->InitGlobalData.SetFromXML(pItem);
			SetExtra()->GlobalData = m_pExtra->InitGlobalData;
			}
		else if (strEquals(pItem->GetTag(), DOCK_SCREENS_TAG))
			SetExtra()->pLocalScreens = pItem;
		else if (strEquals(pItem->GetTag(), LANGUAGE_TAG))
			{
			if (error = SetExtra()->Language.InitFromXML(Ctx, pItem))
				return ComposeLoadError(Ctx, Ctx.sError);
			}
		else if (strEquals(pItem->GetTag(), DISPLAY_ATTRIBUTES_TAG)
				|| strEquals(pItem->GetTag(), ATTRIBUTE_DESC_TAG))
			{
			if (error = SetExtra()->DisplayAttribs.InitFromXML(Ctx, pItem))
				{
				Ctx.pType = NULL;
				return ComposeLoadError(Ctx, Ctx.sError);
				}
			}

		//	Otherwise, it is some element that we don't understand.
		}

	//	Load specific data

	if (error = OnCreateFromXML(Ctx, pDesc))
		{
		Ctx.pType = NULL;
		return error;
		}

	//	Done

	Ctx.pType = NULL;
	return NOERROR;
	}

bool CDesignType::InSelfReference (CDesignType *pType)

//	InSelfReference
//
//	Returns TRUE if the inheritance chain loops above this type

	{
	CDesignType *pParent = g_pUniverse->FindDesignType(pType->m_dwInheritFrom);
	while (pParent)
		{
		if (pParent->m_dwUNID == pType->m_dwUNID)
			return true;

		if (InSelfReference(pParent))
			return true;

		pParent = g_pUniverse->FindDesignType(pParent->m_dwInheritFrom);
		}

	return false;
	}

bool CDesignType::MatchesCriteria (const CDesignTypeCriteria &Criteria)

//	MatchesCriteria
//
//	Returns TRUE if this type matches the given criteria

	{
	int i;

	//	If this type is not part of the criteria, then we're done

	if (!Criteria.MatchesDesignType(GetType()))
		return false;

    //  If structures only, and this is a stationtype, then exclude stars/planets

    CStationType *pStationType;
    if (Criteria.StructuresOnly()
            && (pStationType = CStationType::AsType(this))
            && (pStationType->GetScale() == scaleStar || pStationType->GetScale() == scaleWorld))
        return false;

	//	Skip virtual

	if (IsVirtual() && !Criteria.IncludesVirtual())
		return false;

	//	Check level

	if (Criteria.ChecksLevel())
		{
		int iMinLevel;
		int iMaxLevel;
		GetLevel(&iMinLevel, &iMaxLevel);

		if (iMinLevel == -1 || !Criteria.MatchesLevel(iMinLevel, iMaxLevel))
			return false;
		}

	//	Check required attributes

	for (i = 0; i < Criteria.GetRequiredAttribCount(); i++)
		if (!HasLiteralAttribute(Criteria.GetRequiredAttrib(i)))
			return false;

	for (i = 0; i < Criteria.GetRequiredSpecialAttribCount(); i++)
		if (!HasSpecialAttribute(Criteria.GetRequiredSpecialAttrib(i)))
			return false;

	//	Check excluded attributes

	for (i = 0; i < Criteria.GetExcludedAttribCount(); i++)
		if (HasLiteralAttribute(Criteria.GetExcludedAttrib(i)))
			return false;

	for (i = 0; i < Criteria.GetExcludedSpecialAttribCount(); i++)
		if (HasSpecialAttribute(Criteria.GetExcludedSpecialAttrib(i)))
			return false;

	//	If we get this far, then we match

	return true;
	}

void CDesignType::MergeType (CDesignType *pSource)

//	MergeType
//
//	Merges the info from that given type
	
	{
	//	We take the extension of the source because it has taken responsibility
	//	for the type.
	//
	//	If we didn't do this then it would be possible for someone to override
	//	a registered type and use its permissions.

	m_pExtension = pSource->m_pExtension;
	m_dwVersion = Max(m_dwVersion, pSource->m_dwVersion);

	//	Merge our variables

	m_Events.MergeFrom(pSource->m_Events);

	//	Merge extra data

	if (pSource->m_pExtra)
		{
		SetExtra();

		m_pExtra->StaticData.MergeFrom(pSource->m_pExtra->StaticData);
		m_pExtra->InitGlobalData.MergeFrom(pSource->m_pExtra->InitGlobalData);
		m_pExtra->Language.MergeFrom(pSource->m_pExtra->Language);
		}

	//	LATER: Merge local screens

	//	Let our subclass handle the rest.

	OnMergeType(pSource); 
	}

ALERROR CDesignType::PrepareBindDesign (SDesignLoadCtx &Ctx)

//	PrepareBindDesign
//
//	Do stuff that needs to happen before actual bind

	{
	return OnPrepareBindDesign(Ctx);
	}

void CDesignType::ReadFromStream (SUniverseLoadCtx &Ctx)

//	ReadFromStream
//
//	Reads the variant portions of the design type
	
	{
	//	Read flags

	DWORD dwFlags;
	if (Ctx.dwVersion >= 31)
		Ctx.pStream->Read(dwFlags);
	else if (Ctx.dwVersion >= 3)
		dwFlags = 0x00000001;
	else
		dwFlags = 0;

	bool bHasGlobalData = ((dwFlags & 0x00000001) ? true : false);

	//	Read global data

	if (bHasGlobalData)
		SetExtra()->GlobalData.ReadFromStream(Ctx.pStream);

	//	Allow sub-classes to load

	OnReadFromStream(Ctx);
	}

void CDesignType::ReadGlobalData (SUniverseLoadCtx &Ctx)

//	ReadGlobalData
//
//	For compatibility with older save versions

	{
	if (Ctx.dwVersion < 3)
		SetExtra()->GlobalData.ReadFromStream(Ctx.pStream);
	}

void CDesignType::Reinit (void)

//	Reinit
//
//	Reinitializes the variant portions of the design type
	
	{
	//	Reinit global data

	if (m_pExtra)
		m_pExtra->GlobalData = m_pExtra->InitGlobalData;

	//	Allow sub-classes to reinit

	OnReinit();
	}

void CDesignType::ReportEventError (const CString &sEvent, ICCItem *pError)

//	ReportEventError
//
//	Reports an event error

	{
	CString sError = strPatternSubst(CONSTLIT("%s [%x]: %s"), sEvent, m_dwUNID, pError->GetStringValue());
	CSpaceObject *pPlayer = g_pUniverse->GetPlayerShip();
	if (pPlayer)
		pPlayer->SendMessage(NULL, sError);

	g_pUniverse->LogOutput(sError);
	}

bool CDesignType::Translate (CSpaceObject *pObj, const CString &sID, ICCItem *pData, ICCItem **retpResult) const

//	Translate
//
//	Translate from a <Language> block to a CodeChain item.
//
//	NOTE: Caller is responsible for discarding the result (if we return TRUE).
	
	{
	if (m_pExtra && m_pExtra->Language.Translate(pObj, sID, pData, retpResult))
		return true;

	if (m_pInheritFrom && m_pInheritFrom->Translate(pObj, sID, pData, retpResult))
		return true;

	//	Backwards compatible translate

	if (GetVersion() <= 2 && TranslateVersion2(pObj, sID, retpResult))
		return true;

	//	Not found

	return false;
	}

bool CDesignType::TranslateText (CSpaceObject *pObj, const CString &sID, ICCItem *pData, CString *retsText) const

//	Translate
//
//	Translate from a <Language> block to text.

	{
	if (m_pExtra && m_pExtra->Language.Translate(pObj, sID, pData, retsText))
		return true;

	if (m_pInheritFrom && m_pInheritFrom->TranslateText(pObj, sID, pData, retsText))
		return true;

	//	Backwards compatible translate

	if (GetVersion() <= 2)
		{
		ICCItem *pItem;
		if (!TranslateVersion2(pObj, sID, &pItem))
			return false;

		if (retsText)
			*retsText = pItem->GetStringValue();

		pItem->Discard(&g_pUniverse->GetCC());
		return true;
		}

	//	Not found

	return false;
	}
	
bool CDesignType::TranslateText (const CItem &Item, const CString &sID, ICCItem *pData, CString *retsText) const

//	Translate
//
//	Translate from a <Language> block on an item to text.

	{
	if (m_pExtra && m_pExtra->Language.Translate(Item, sID, pData, retsText))
		return true;

	if (m_pInheritFrom && m_pInheritFrom->TranslateText(Item, sID, pData, retsText))
		return true;

	return false;
	}

bool CDesignType::TranslateVersion2 (CSpaceObject *pObj, const CString &sID, ICCItem **retpResult) const

//	TranslateVersion2
//
//	Translates using the old apiVersion="2" method, which relied on static data.

	{
	int i;

	if (GetVersion() > 2)
		return false;

	ICCItemPtr pValue = GetStaticData(CONSTLIT("Language"));
	if (!pValue->IsNil())
		{
		CCodeChainCtx Ctx;

		for (i = 0; i < pValue->GetCount(); i++)
			{
			ICCItem *pEntry = pValue->GetElement(i);
			if (pEntry->GetCount() == 2 && strEquals(sID, pEntry->GetElement(0)->GetStringValue()))
				{
				*retpResult = Ctx.Run(pEntry->GetElement(1));	//	LATER:Event
				return true;
				}
			}
		}

	return false;
	}

void CDesignType::WriteToStream (IWriteStream *pStream)

//	WriteToStream
//
//	Writes the variant portions of the design type

	{
	//	Figure out some flags

	DWORD dwFlags = 0;
	dwFlags |= (m_pExtra ? 0x00000001 : 0);

	pStream->Write(dwFlags);

	//	Write out global data

	if (m_pExtra)
		m_pExtra->GlobalData.WriteToStream(pStream, NULL);

	//	Allow sub-classes to write

	OnWriteToStream(pStream);
	}

//	CItemTypeRef --------------------------------------------------------------

ALERROR CItemTypeRef::Bind (SDesignLoadCtx &Ctx, ItemCategories iCategory)
	{
	ALERROR error;
	if (error = CDesignTypeRef<CItemType>::Bind(Ctx))
		return error;

	if (m_pType && m_pType->GetCategory() != iCategory)
		{
		Ctx.sError = strPatternSubst(CONSTLIT("%s item expected: %x"), ::GetItemCategoryName(iCategory), m_dwUNID);
		return ERR_FAIL;
		}

	return NOERROR;
	}

//	CArmorClassRef -----------------------------------------------------------

ALERROR CArmorClassRef::Bind (SDesignLoadCtx &Ctx)
	{
	if (m_dwUNID)
		{
		CDesignType *pBaseType = g_pUniverse->FindDesignType(m_dwUNID);
		if (pBaseType == NULL)
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Unknown armor design type: %x"), m_dwUNID);
			return ERR_FAIL;
			}

		CItemType *pItemType = CItemType::AsType(pBaseType);
		if (pItemType == NULL)
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Armor item type expected: %x"), m_dwUNID);
			return ERR_FAIL;
			}

		m_pType = pItemType->GetArmorClass();
		if (m_pType == NULL)
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Armor item type expected: %x"), m_dwUNID);
			return ERR_FAIL;
			}
		}

	return NOERROR;
	}

//	CDeviceClassRef -----------------------------------------------------------

ALERROR CDeviceClassRef::Bind (SDesignLoadCtx &Ctx)
	{
	if (m_dwUNID)
		{
		CDesignType *pBaseType = g_pUniverse->FindDesignType(m_dwUNID);
		if (pBaseType == NULL)
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Unknown device design type: %x"), m_dwUNID);
			return ERR_FAIL;
			}

		CItemType *pItemType = CItemType::AsType(pBaseType);
		if (pItemType == NULL)
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Device item type expected: %x"), m_dwUNID);
			return ERR_FAIL;
			}

		m_pType = pItemType->GetDeviceClass();
		if (m_pType == NULL)
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Device item type expected: %x"), m_dwUNID);
			return ERR_FAIL;
			}
		}

	return NOERROR;
	}

void CDeviceClassRef::Set (CDeviceClass *pDevice)
	{
	if (pDevice)
		{
		m_pType = pDevice;
		m_dwUNID = pDevice->GetUNID();
		}
	else
		{
		m_pType = NULL;
		m_dwUNID = 0;
		}
	}

//	CWeaponFireDescRef -----------------------------------------------------------

ALERROR CWeaponFireDescRef::Bind (SDesignLoadCtx &Ctx)
	{
	if (m_dwUNID)
		{
        CItemType *pItemType = g_pUniverse->FindItemType(m_dwUNID);
		if (pItemType == NULL)
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Weapon item type expected: %08x"), m_dwUNID);
			return ERR_FAIL;
			}

        m_pType = pItemType->GetWeaponFireDesc(CItemCtx(), &Ctx.sError);
        if (m_pType == NULL)
            return ERR_FAIL;
		}

	return NOERROR;
	}

//	CEffectCreatorRef ---------------------------------------------------------

CEffectCreatorRef::CEffectCreatorRef (const CEffectCreatorRef &Source)

//	CEffectCreatorRef copy constructor

	{
	m_dwUNID = Source.m_dwUNID;

	//	We make a reference

	m_pType = Source.m_pType;
	m_pSingleton = NULL;
	m_bDelete = false;
	}

CEffectCreatorRef::~CEffectCreatorRef (void)
	{
	if (m_bDelete && m_pType)
		delete m_pType;

	if (m_pSingleton)
		delete m_pSingleton;
	}

CEffectCreatorRef &CEffectCreatorRef::operator= (const CEffectCreatorRef &Source)

//	CEffectCreatorRef operator=

	{
	//	Free our current type, if necessary

	if (m_bDelete && m_pType)
		delete m_pType;

	m_dwUNID = Source.m_dwUNID;

	//	We make a reference

	m_pType = Source.m_pType;
	m_bDelete = false;

	return *this;
	}

ALERROR CEffectCreatorRef::Bind (SDesignLoadCtx &Ctx)
	{
	DEBUG_TRY

	//	Clean up, because we might want to recompute for next time.

	if (m_pSingleton)
		{
		try
			{
			delete m_pSingleton;
			}
		catch (...)
			{
			::kernelDebugLogPattern("Crash deleting singleton: %08x. UNID = %08x", (DWORD)m_pSingleton, m_dwUNID);
			throw;
			}

		m_pSingleton = NULL;
		}

	//	Bind

	if (m_dwUNID)
		return CDesignTypeRef<CEffectCreator>::Bind(Ctx);
	else if (m_pType)
		return m_pType->BindDesign(Ctx);

	return NOERROR;

	DEBUG_CATCH
	}

ALERROR CEffectCreatorRef::CreateBeamEffect (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID)
	{
	ALERROR error;

	if (error = CEffectCreator::CreateBeamEffect(Ctx, pDesc, sUNID, &m_pType))
		return error;

	m_dwUNID = 0;
	m_bDelete = true;

	return NOERROR;
	}

ALERROR CEffectCreatorRef::CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID)
	{
	ALERROR error;

    //  If we're specifying an effect type by reference, then we load it here.

    CString sAttrib;
    if (pDesc->FindAttribute(EFFECT_ATTRIB, &sAttrib))
        {
        if (error = LoadUNID(Ctx, sAttrib))
            return error;

        //  We can pass parameters to the effect type through a data block.

	    CXMLElement *pInitialData = pDesc->GetContentElementByTag(DATA_TAG);
	    if (pInitialData)
		    m_Data.SetFromXML(pInitialData);
        }

    //  Otherwise, we are defining an effect

    else
        {
	    if (error = CEffectCreator::CreateFromXML(Ctx, pDesc, sUNID, &m_pType))
		    return error;

	    m_dwUNID = 0;
	    m_bDelete = true;
        }

	return NOERROR;
	}

IEffectPainter *CEffectCreatorRef::CreatePainter (CCreatePainterCtx &Ctx, CEffectCreator *pDefaultCreator)

//	CreatePainter
//
//	Use this call when we want to use a per-owner singleton.

	{
    int i;

	//	If we have a singleton, then return that.

	if (m_pSingleton)
		return m_pSingleton;

	//	Figure out the creator

	CEffectCreator *pCreator = m_pType;
	if (pCreator == NULL)
		pCreator = pDefaultCreator;

	if (pCreator == NULL)
		return NULL;

    //  If we have some data, add it to the context

    for (i = 0; i < m_Data.GetDataCount(); i++)
        {
        //  LATER: We only handle integers for now. Later we should handle any
        //  ICCItem type.

        Ctx.AddDataInteger(m_Data.GetDataAttrib(i), strToInt(m_Data.GetData(i), 0));
        }

	//	Create the painter

	IEffectPainter *pPainter = pCreator->CreatePainter(Ctx);

	//	If we're an owner singleton then we only need to create this once.

	if (pCreator->GetInstance() == CEffectCreator::instOwner
			&& !pPainter->IsSingleton())
		{
		pPainter->SetSingleton(true);
		m_pSingleton = pPainter;

#ifdef DEBUG_SINGLETON_EFFECTS
		::kernelDebugLogPattern("Create singleton painter UNID = %08x: %08x", m_dwUNID, (DWORD)pPainter);
#endif
		}

	//	Done

	return pPainter;
	}

ALERROR CEffectCreatorRef::LoadEffect (SDesignLoadCtx &Ctx, const CString &sUNID, CXMLElement *pDesc, const CString &sAttrib)
	{
	ALERROR error;

	if (pDesc)
		{
		if (error = CreateFromXML(Ctx, pDesc, sUNID))
			return error;
		}
	else
		if (error = LoadUNID(Ctx, sAttrib))
			return error;

	return NOERROR;
	}

ALERROR CEffectCreatorRef::LoadSimpleEffect (SDesignLoadCtx &Ctx, const CString &sUNID, CXMLElement *pDesc)

//	LoadSimpleEffect
//
//	Loads an effect from XML.

	{
	ALERROR error;

	if (strEquals(pDesc->GetTag(), LOOKUP_TAG))
		{
		if (error = LoadUNID(Ctx, pDesc->GetAttribute(EFFECT_ATTRIB)))
			return error;
		}
	else
		{
		if (error = CEffectCreator::CreateSimpleFromXML(Ctx, pDesc, sUNID, &m_pType))
			return error;

		m_dwUNID = 0;
		m_bDelete = true;
		}

	return NOERROR;
	}

void CEffectCreatorRef::Set (CEffectCreator *pEffect)

//	Set
//
//	Sets the effect

	{
	if (m_bDelete && m_pType)
		delete m_pType;

	m_pType = pEffect;
	m_bDelete = false;
	if (m_pType)
		m_dwUNID = m_pType->GetUNID();
	else
		m_dwUNID = 0;
	}

//	Utility -------------------------------------------------------------------

CString ParseAchievementSection (ICCItem *pItem)
	{
	if (pItem == NULL)
		return NULL_STR;
	else if (pItem->IsNil())
		return NULL_STR;
	else
		return pItem->GetStringValue();
	}

CString ParseAchievementSort (ICCItem *pItem)
	{
	if (pItem == NULL)
		return NULL_STR;
	else if (pItem->IsNil())
		return NULL_STR;
	else if (pItem->IsInteger())
		return strPatternSubst(CONSTLIT("%08x"), pItem->GetIntegerValue());
	else
		return pItem->GetStringValue();
	}

CString ParseAchievementValue (ICCItem *pItem)
	{
	if (pItem == NULL)
		return NULL_STR;
	else if (pItem->IsNil())
		return NULL_STR;
	else if (pItem->IsInteger())
		return strFormatInteger(pItem->GetIntegerValue(), -1, FORMAT_THOUSAND_SEPARATOR | FORMAT_UNSIGNED);
	else
		return pItem->GetStringValue();
	}