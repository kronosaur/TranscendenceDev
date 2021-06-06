//	CSystemType.cpp
//
//	CSystemType class
//	Copyright (c) 2021 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define ENCOUNTER_OVERRIDES_TAG					CONSTLIT("EncounterOverrides")
#define ENHANCE_ABILITIES_TAG					CONSTLIT("EnhancementAbilities")
#define IMAGE_FILTERS_TAG						CONSTLIT("ImageFilters")
#define SYSTEM_GROUP_TAG						CONSTLIT("SystemGroup")
#define TABLES_TAG								CONSTLIT("Tables")

#define BACKGROUND_ID_ATTRIB					CONSTLIT("backgroundID")
#define CRITERIA_ATTRIB							CONSTLIT("criteria")
#define NAME_ATTRIB								CONSTLIT("name")
#define NO_EXTRA_ENCOUNTERS_ATTRIB				CONSTLIT("noExtraEncounters")
#define NO_RANDOM_ENCOUNTERS_ATTRIB				CONSTLIT("noRandomEncounters")
#define SPACE_COLOR_ATTRIB						CONSTLIT("spaceColor")
#define SPACE_SCALE_ATTRIB						CONSTLIT("spaceScale")
#define SPACE_ENVIRONMENT_TILE_SIZE_ATTRIB		CONSTLIT("spaceEnvironmentTileSize")
#define TIME_SCALE_ATTRIB						CONSTLIT("timeScale")

#define ON_CREATE_EVENT							CONSTLIT("OnCreate")
#define ON_SYSTEM_STARTED_EVENT					CONSTLIT("OnSystemStarted")
#define ON_SYSTEM_STOPPED_EVENT					CONSTLIT("OnSystemStopped")

#define SIZE_SMALL								CONSTLIT("small")
#define SIZE_MEDIUM								CONSTLIT("medium")
#define SIZE_LARGE								CONSTLIT("large")
#define SIZE_HUGE								CONSTLIT("huge")

#define STR_NONE								CONSTLIT("none")

static const char *CACHED_EVENTS[CSystemType::evtCount] =
	{
		"OnObjJumpPosAdj",
	};

static TStaticStringTable<TStaticStringEntry<Metric>, 7> SCALE_TABLE = {
	"au",					g_AU,
	"light-minute",			LIGHT_MINUTE,
	"light-second",			LIGHT_SECOND,
	"lm",					LIGHT_MINUTE,
	"ls",					LIGHT_SECOND,
	"mls",					LIGHT_SECOND / 1000.0,
	"pixel",				g_KlicksPerPixel,
	};

CSystemType::CSystemType (void) : 
		m_pDesc(NULL),
		m_pLocalTables(NULL)

//	CSystemType constructor

	{
	}

CSystemType::~CSystemType (void)

//	CSystemType destructor

	{
	}

ALERROR CSystemType::FireOnCreate (SSystemCreateCtx &SysCreateCtx, CString *retsError)

//	FireOnCreate
//
//	Fire OnCreate event

	{
	SEventHandlerDesc Event;
	if (FindEventHandler(ON_CREATE_EVENT, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());
		Ctx.SetSystemCreateCtx(&SysCreateCtx);

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			{
			if (retsError)
				*retsError = strPatternSubst(CONSTLIT("System OnCreate: %s"), pResult->GetStringValue());
			return ERR_FAIL;
			}

		Ctx.Discard(pResult);
		}

	return NOERROR;
	}

bool CSystemType::FireOnObjJumpPosAdj (CSpaceObject *pObj, CVector *iovPos)

//	FireOnObjJumpPosAdj
//
//	Fires a system event to adjust the position of an object that jumped
//	Returns TRUE if the event adjusted the position

	{
	SEventHandlerDesc Event;
	if (FindEventHandlerSystemType(evtOnObjJumpPosAdj, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());

		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(pObj);
		Ctx.DefineVector(CONSTLIT("aJumpPos"), *iovPos);

		ICCItem *pResult = Ctx.Run(Event);

		if (pResult->IsError())
			{
			kernelDebugLogPattern("System OnObjJumpPosAdj: %s", pResult->GetStringValue());
			Ctx.Discard(pResult);
			return false;
			}
		else if (pResult->IsNil())
			{
			Ctx.Discard(pResult);
			return false;
			}
		else
			{
			CVector vNewPos = Ctx.AsVector(pResult);
			Ctx.Discard(pResult);

			if (vNewPos == *iovPos)
				return false;

			*iovPos = vNewPos;
			return true;
			}
		}

	return false;
	}

void CSystemType::FireOnSystemStarted (DWORD dwElapsedTime)

//	FireOnSystemStarted
//
//	Fires <OnSystemStarted> event.

	{
	SEventHandlerDesc Event;
	if (!FindEventHandler(ON_SYSTEM_STARTED_EVENT, &Event))
		return;

	CCodeChainCtx CCX(GetUniverse());

	CCX.DefineContainingType(this);
	CCX.DefineInteger(CONSTLIT("aElapsedTime"), dwElapsedTime);

	//	Run code

	ICCItemPtr pResult = CCX.RunCode(Event);
	if (pResult->IsError())
		ReportEventError(ON_SYSTEM_STARTED_EVENT, pResult);
	}

void CSystemType::FireOnSystemStopped ()

//	FireOnSystemStopped
//
//	Fires <OnSystemStopped> event.

	{
	SEventHandlerDesc Event;
	if (!FindEventHandler(ON_SYSTEM_STOPPED_EVENT, &Event))
		return;

	CCodeChainCtx CCX(GetUniverse());

	CCX.DefineContainingType(this);

	//	Run code

	ICCItemPtr pResult = CCX.RunCode(Event);
	if (pResult->IsError())
		ReportEventError(ON_SYSTEM_STOPPED_EVENT, pResult);
	}

ALERROR CSystemType::FireSystemCreateCode (SSystemCreateCtx &SysCreateCtx, ICCItem *pCode, const COrbit &OrbitDesc, CString *retsError)

//	FireSystemCreateCode
//
//	Runs a bit of code at system create time.

	{
	CCodeChainCtx Ctx(GetUniverse());
	Ctx.SetExtension(GetExtension());	//	This code always comes from the SystemType (never inherited).
	Ctx.SetSystemCreateCtx(&SysCreateCtx);
	Ctx.DefineOrbit(CONSTLIT("aOrbit"), OrbitDesc);

	ICCItem *pResult = Ctx.Run(pCode);
	if (pResult->IsError())
		{
		if (retsError)
			*retsError = strPatternSubst(CONSTLIT("System: %s"), pResult->GetStringValue());
		return ERR_FAIL;
		}

	Ctx.Discard(pResult);
	return NOERROR;
	}

ALERROR CSystemType::OnBindDesign (SDesignLoadCtx &Ctx)

//	OnBindDesign
//
//	Bind design

	{
	if (ALERROR error = m_Enhancements.Bind(Ctx))
		return error;

	InitCachedEvents(evtCount, CACHED_EVENTS, m_CachedEvents);

	return NOERROR;
	}

ALERROR CSystemType::OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	OnCreateFromXML
//
//	Create from XML

	{
	ALERROR error;

	if (!pDesc->FindAttribute(NAME_ATTRIB, &m_sName))
		m_sName = strPatternSubst(CONSTLIT("SystemType %08x"), GetUNID());

	//	Load the background image UNID. If we have an explicit definition, then
	//	take it, even if it is 0. Otherwise, we default to the core background.

	CString sAttrib;
	if (pDesc->FindAttribute(BACKGROUND_ID_ATTRIB, &sAttrib))
		{
		if (strEquals(sAttrib, STR_NONE)
				|| strToInt(sAttrib, -1) == 0)
			m_dwBackgroundUNID = 0;
		else
			{
			if (error = ::LoadUNID(Ctx, sAttrib, &m_dwBackgroundUNID))
				return error;
			}
		}
	else
		m_dwBackgroundUNID = UNID_DEFAULT_SYSTEM_BACKGROUND;

	//	Tile size

	if (pDesc->FindAttribute(SPACE_ENVIRONMENT_TILE_SIZE_ATTRIB, &sAttrib))
		{
		if (strEquals(sAttrib, SIZE_SMALL))
			m_iTileSize = sizeSmall;
		else if (strEquals(sAttrib, SIZE_MEDIUM))
			m_iTileSize = sizeMedium;
		else if (strEquals(sAttrib, SIZE_LARGE))
			m_iTileSize = sizeLarge;
		else if (strEquals(sAttrib, SIZE_HUGE))
			m_iTileSize = sizeHuge;
		else
			return ComposeLoadError(Ctx, strPatternSubst(CONSTLIT("Invalid spaceEnvironmentTileSize: %s."), sAttrib));
		}
	else
		m_iTileSize = (GetAPIVersion() >= 14 ? sizeSmall : sizeLarge);

	m_rgbSpace = ::LoadRGBColor(pDesc->GetAttribute(SPACE_COLOR_ATTRIB), DEFAULT_SPACE_COLOR);

	//	Options

	m_bNoExtraEncounters = pDesc->GetAttributeBool(NO_EXTRA_ENCOUNTERS_ATTRIB);
	m_bNoRandomEncounters = pDesc->GetAttributeBool(NO_RANDOM_ENCOUNTERS_ATTRIB);

	if (!pDesc->FindAttributeDouble(SPACE_SCALE_ATTRIB, &m_rSpaceScale) || m_rSpaceScale <= 0.0)
		m_rSpaceScale = KLICKS_PER_PIXEL;

	if (!pDesc->FindAttributeDouble(TIME_SCALE_ATTRIB, &m_rTimeScale) || m_rTimeScale <= 0.0)
		m_rTimeScale = TIME_SCALE;

	//	We keep the XML around for system definitions.
	//
	//	NOTE: OK if this is NULL since we can also create a system procedurally
	//	with <OnCreate>.

	m_pDesc = pDesc->GetContentElementByTag(SYSTEM_GROUP_TAG);

	//	We also need to keep the local tables

	m_pLocalTables = pDesc->GetContentElementByTag(TABLES_TAG);

	//	Image filters

	CXMLElement *pFilters = pDesc->GetContentElementByTag(IMAGE_FILTERS_TAG);
	if (pFilters)
		{
		if (error = m_ImageFilters.InitFromXML(Ctx, *pFilters))
			return ComposeLoadError(Ctx, Ctx.sError);

		m_ImageFilterCriteria.Init(pFilters->GetAttribute(CRITERIA_ATTRIB));
		}

	//	Enhancements

	CXMLElement *pEnhanceList = pDesc->GetContentElementByTag(ENHANCE_ABILITIES_TAG);
	if (pEnhanceList)
		{
		if (error = m_Enhancements.InitFromXML(Ctx, pEnhanceList, NULL))
			return ComposeLoadError(Ctx, Ctx.sError);
		}

	//	Encounter overrides

	m_pEncounterOverrides = pDesc->GetContentElementByTag(ENCOUNTER_OVERRIDES_TAG);

	return NOERROR;
	}

void CSystemType::OnMarkImages (void)

//	OnMarkImages
//
//	Mark images for the system

	{
	if (m_dwBackgroundUNID != 0)
		{
		TSharedPtr<CObjectImage> pImage = GetUniverse().FindLibraryImage(m_dwBackgroundUNID);
		if (pImage)
			pImage->Mark();
		}
	}

Metric CSystemType::ParseScale (const CString &sValue)

//	ParseScale
//
//	Parses a scale unit and returns the unit length in kilometers.

	{
	if (sValue.IsBlank())
		return LIGHT_SECOND;
	else
		{
		if (auto pEntry = SCALE_TABLE.GetAt(sValue))
			return pEntry->Value;
		else
			return LIGHT_SECOND;
		}
	}
