//	CPlayerSettings.cpp
//
//	CPlayerSettings class
//	Copyright (c) 2011 by George Moromisato. All Rights Reserved.

#include "PreComp.h"

#define ARMOR_DISPLAY_TAG						CONSTLIT("ArmorDisplay")
#define ARMOR_SECTION_TAG						CONSTLIT("ArmorSection")
#define DOCK_SCREEN_DISPLAY_TAG 				CONSTLIT("DockScreenDisplay")
#define FUEL_LEVEL_IMAGE_TAG					CONSTLIT("FuelLevelImage")
#define FUEL_LEVEL_TEXT_TAG						CONSTLIT("FuelLevelText")
#define FUEL_LOW_LEVEL_IMAGE_TAG				CONSTLIT("FuelLowLevelImage")
#define IMAGE_TAG								CONSTLIT("Image")
#define POWER_LEVEL_IMAGE_TAG					CONSTLIT("PowerLevelImage")
#define POWER_LEVEL_TEXT_TAG					CONSTLIT("PowerLevelText")
#define REACTOR_DISPLAY_TAG						CONSTLIT("ReactorDisplay")
#define REACTOR_TEXT_TAG						CONSTLIT("ReactorText")
#define SHIELD_DISPLAY_TAG						CONSTLIT("ShieldDisplay")
#define SHIELD_EFFECT_TAG						CONSTLIT("ShieldLevelEffect")
#define SHIP_IMAGE_TAG							CONSTLIT("ShipImage")
#define WEAPON_DISPLAY_TAG						CONSTLIT("WeaponDisplay")

#define AUTOPILOT_ATTRIB						CONSTLIT("autopilot")
#define ARMOR_ID_ATTRIB							CONSTLIT("armorID")
#define DEBUG_ONLY_ATTRIB						CONSTLIT("debugOnly")
#define DESC_ATTRIB								CONSTLIT("desc")
#define DEST_X_ATTRIB							CONSTLIT("destX")
#define DEST_Y_ATTRIB							CONSTLIT("destY")
#define DOCK_SERVICES_SCREEN_ATTRIB				CONSTLIT("dockServicesScreen")
#define HEIGHT_ATTRIB							CONSTLIT("height")
#define HP_X_ATTRIB								CONSTLIT("hpX")
#define HP_Y_ATTRIB								CONSTLIT("hpY")
#define HULL_VALUE_ATTRIB						CONSTLIT("hullValue")
#define INITIAL_CLASS_ATTRIB					CONSTLIT("initialClass")
#define LARGE_IMAGE_ATTRIB						CONSTLIT("largeImage")
#define NAME_ATTRIB								CONSTLIT("name")
#define NAME_BREAK_WIDTH						CONSTLIT("nameBreakWidth")
#define NAME_DEST_X_ATTRIB						CONSTLIT("nameDestX")
#define NAME_DEST_Y_ATTRIB						CONSTLIT("nameDestY")
#define NAME_Y_ATTRIB							CONSTLIT("nameY")
#define SEGMENT_ATTRIB							CONSTLIT("segment")
#define SHIELD_EFFECT_ATTRIB					CONSTLIT("shieldLevelEffect")
#define SHIP_SCREEN_ATTRIB						CONSTLIT("shipScreen")
#define SHIP_CONFIG_SCREEN_ATTRIB				CONSTLIT("shipConfigScreen")
#define SORT_ORDER_ATTRIB						CONSTLIT("sortOrder")
#define STARTING_CREDITS_ATTRIB					CONSTLIT("startingCredits")
#define STARTING_MAP_ATTRIB						CONSTLIT("startingMap")
#define STARTING_POS_ATTRIB						CONSTLIT("startingPos")
#define STARTING_SYSTEM_ATTRIB					CONSTLIT("startingSystem")
#define STYLE_ATTRIB							CONSTLIT("style")
#define WIDTH_ATTRIB							CONSTLIT("width")
#define X_ATTRIB								CONSTLIT("x")
#define Y_ATTRIB								CONSTLIT("y")

#define SEGMENT_AFT								CONSTLIT("aft")
#define SEGMENT_AFT_PORT						CONSTLIT("aft-port")
#define SEGMENT_AFT_STARBOARD					CONSTLIT("aft-starboard")
#define SEGMENT_FORE_PORT						CONSTLIT("fore-port")
#define SEGMENT_FORE_STARBOARD					CONSTLIT("fore-starboard")
#define SEGMENT_FORWARD							CONSTLIT("forward")
#define SEGMENT_PORT							CONSTLIT("port")
#define SEGMENT_STARBOARD						CONSTLIT("starboard")

#define STYLE_DEFAULT							CONSTLIT("default")
#define STYLE_RING_SEGMENTS						CONSTLIT("ringSegments")

#define ERR_SHIELD_DISPLAY_NEEDED				CONSTLIT("missing or invalid <ShieldDisplay> element")
#define ERR_ARMOR_DISPLAY_NEEDED				CONSTLIT("missing or invalid <ArmorDisplay> element")
#define ERR_REACTOR_DISPLAY_NEEDED				CONSTLIT("missing or invalid <ReactorDisplay> element")
#define ERR_INVALID_STARTING_CREDITS			CONSTLIT("invalid starting credits")
#define ERR_SHIP_IMAGE_NEEDED					CONSTLIT("invalid <ShipImage> element")
#define ERR_MUST_HAVE_SHIP_IMAGE				CONSTLIT("<ShipImage> in <ArmorDisplay> required if using shield level effect")
#define ERR_WEAPON_DISPLAY_NEEDED				CONSTLIT("missing or invalid <WeaponDisplay> element")

ALERROR InitRectFromElement (CXMLElement *pItem, RECT *retRect);

void CPlayerSettings::AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) const

//	AddTypesUsed
//
//	Adds types used

	{
	retTypesUsed->SetAt(m_dwLargeImage, true);
	retTypesUsed->SetAt(strToInt(m_pShipScreen.GetUNID(), 0), true);
	retTypesUsed->SetAt(strToInt(m_pDockServicesScreen.GetUNID(), 0), true);
	retTypesUsed->SetAt(strToInt(m_pShipConfigScreen.GetUNID(), 0), true);

    GetDockScreenVisuals().AddTypesUsed(retTypesUsed);

	//	LATER: Add armor images, etc.
	}

ALERROR CPlayerSettings::Bind (SDesignLoadCtx &Ctx, CShipClass *pClass)

//	Bind
//
//	Bind design

	{
	DEBUG_TRY

	ALERROR error;
	int i;
    bool bMissingElements = false;

	//	Bind basic stuff

	if (error = m_pShipScreen.Bind(Ctx, pClass->GetLocalScreens()))
		return error;

	if (error = m_pDockServicesScreen.Bind(Ctx, pClass->GetLocalScreens()))
		return error;

	if (error = m_pShipConfigScreen.Bind(Ctx, pClass->GetLocalScreens()))
		return error;

	if (error = m_StartingCredits.Bind(Ctx))
		return error;

	if (error = m_HullValue.Bind(Ctx))
		return error;

    //  If we own our visuals, bind them

    if (m_fOwnDockScreenDesc)
        {
        if (error = m_pDockScreenDesc->Bind(Ctx))
            return error;
        }
    else
        {
        m_pDockScreenDesc = NULL;
        bMissingElements = true;
        }

	//	HUDs

	for (i = 0; i < hudCount; i++)
		{
        if (!m_HUDDesc[i].bOwned)
			{
            m_HUDDesc[i].pDesc = NULL;
            bMissingElements = true;
			}
		}

    //  If we have all elements, then consider us resolved. Otherwise, we need
    //  to inherit from some base class.

    m_fResolved = (!bMissingElements);

	//	Done

	return NOERROR;

	DEBUG_CATCH
	}

void CPlayerSettings::CleanUp (void)

//	CleanUp
//
//	Clean up our structures

	{
	int i;

	for (i = 0; i < hudCount; i++)
		if (m_HUDDesc[i].bOwned)
			{
			delete m_HUDDesc[i].pDesc;
			m_HUDDesc[i].pDesc = NULL;
			m_HUDDesc[i].bOwned = false;
			}
	}

ALERROR CPlayerSettings::ComposeLoadError (SDesignLoadCtx &Ctx, const CString &sError)

//	ComposeLoadError
//
//	Composes an error while loading from XML.

	{
	Ctx.sError = sError;
	return ERR_FAIL;
	}

void CPlayerSettings::Copy (const CPlayerSettings &Src)

//  Copy
//
//  Copy from source

    {
	int i;

	m_sDesc = Src.m_sDesc;
	m_iSortOrder = Src.m_iSortOrder;

    //  Images

	m_dwLargeImage = Src.m_dwLargeImage;
    if (m_fOwnDockScreenDesc = Src.m_fOwnDockScreenDesc)
        m_pDockScreenDesc = new CDockScreenVisuals(*Src.m_pDockScreenDesc);
    else
        m_pDockScreenDesc = NULL;

	//	Miscellaneous

	m_StartingCredits = Src.m_StartingCredits;		//	Starting credits
	m_sStartNode = Src.m_sStartNode;						//	Starting node (may be blank)
	m_sStartPos = Src.m_sStartPos;						//	Label of starting position (may be blank)
	m_pShipScreen = Src.m_pShipScreen;			//	Ship screen
	m_pDockServicesScreen = Src.m_pDockServicesScreen;
	m_pShipConfigScreen = Src.m_pShipConfigScreen;
	m_HullValue = Src.m_HullValue;

	//	HUDs

	for (i = 0; i < hudCount; i++)
		{
        if (Src.m_HUDDesc[i].bOwned)
            {
			m_HUDDesc[i].pDesc = m_HUDDesc[i].pDesc->OrphanCopy();
            m_HUDDesc[i].bOwned = true;
            }
		}

	//	Flags

	m_fInitialClass = Src.m_fInitialClass;					//	Use ship class at game start
	m_fDebug = Src.m_fDebug;
	m_fIncludeInAllAdventures = Src.m_fIncludeInAllAdventures;
    }

CEffectCreator *CPlayerSettings::FindEffectCreator (const CString &sUNID) const

//	FindEffectCreator
//
//	Finds an effect creator of the following form:
//
//	{unid}:p:s
//          ^

	{
	char *pPos = sUNID.GetASCIIZPointer();
	if (*pPos != ':')
		return NULL;
	
	pPos++;

    //	No longer needed

#if 0

	//	Figure out what

	switch (*pPos)
		{
		case 's':
			return m_ShieldDesc.pShieldEffect;

		default:
			return NULL;
		}
#endif

    return NULL;
	}

ALERROR CPlayerSettings::InitFromXML (SDesignLoadCtx &Ctx, CShipClass *pClass, CXMLElement *pDesc)

//	InitFromXML
//
//	Initialize from an XML element

	{
	ALERROR error;
	int i;

	m_sDesc = pDesc->GetAttribute(DESC_ATTRIB);
	if (error = LoadUNID(Ctx, pDesc->GetAttribute(LARGE_IMAGE_ATTRIB), &m_dwLargeImage))
		return error;

	m_fDebug = pDesc->GetAttributeBool(DEBUG_ONLY_ATTRIB);
	CString sInitialClass = pDesc->GetAttribute(INITIAL_CLASS_ATTRIB);
	if (strEquals(sInitialClass, CONSTLIT("always")))
		{
		m_fInitialClass = true;
		m_fIncludeInAllAdventures = true;
		}
	else
		{
		m_fInitialClass = CXMLElement::IsBoolTrueValue(sInitialClass);
		m_fIncludeInAllAdventures = false;
		}

	m_iSortOrder = pDesc->GetAttributeIntegerBounded(SORT_ORDER_ATTRIB, 0, -1, 100);

	//	Load some miscellaneous data

	CString sAttrib;
	if (!pDesc->FindAttribute(STARTING_CREDITS_ATTRIB, &sAttrib))
		sAttrib = CONSTLIT("5d20+200");

	if (error = m_StartingCredits.InitFromXML(Ctx, sAttrib))
		return error;

	if (error = ::LoadUNID(Ctx, pDesc->GetAttribute(STARTING_MAP_ATTRIB), &m_dwStartMap))
		return error;

	m_sStartNode = pDesc->GetAttribute(STARTING_SYSTEM_ATTRIB);
	m_sStartPos = pDesc->GetAttribute(STARTING_POS_ATTRIB);
	if (m_sStartPos.IsBlank())
		m_sStartPos = CONSTLIT("Start");

	if (error = m_HullValue.InitFromXML(Ctx, pDesc->GetAttribute(HULL_VALUE_ATTRIB)))
		return ComposeLoadError(Ctx, Ctx.sError);

	//	Load the ship screen

	CString sShipScreenUNID = pDesc->GetAttribute(SHIP_SCREEN_ATTRIB);
	if (sShipScreenUNID.IsBlank())
		sShipScreenUNID = strFromInt(DEFAULT_SHIP_SCREEN_UNID, false);
	m_pShipScreen.LoadUNID(Ctx, sShipScreenUNID);

	//	Load dock services screen

	CString sUNID;
	if (pDesc->FindAttribute(DOCK_SERVICES_SCREEN_ATTRIB, &sUNID))
		m_pDockServicesScreen.LoadUNID(Ctx, sUNID);
	else
		{
		if (pClass->GetAPIVersion() >= 23)
			sUNID = strFromInt(DEFAULT_DOCK_SERVICES_SCREEN, false);
		else
			sUNID = strFromInt(COMPATIBLE_DOCK_SERVICES_SCREEN, false);

		m_pDockServicesScreen.LoadUNID(Ctx, sUNID);
		}

	if (pDesc->FindAttribute(SHIP_CONFIG_SCREEN_ATTRIB, &sUNID))
		m_pShipConfigScreen.LoadUNID(Ctx, sUNID);
	else
		{
		if (pClass->GetAPIVersion() >= 27)
			sUNID = strFromInt(DEFAULT_DOCK_SERVICES_SCREEN, false);
		else
			sUNID = strFromInt(COMPATIBLE_SHIP_CONFIG_SCREEN, false);

		m_pShipConfigScreen.LoadUNID(Ctx, sUNID);
		}

    //  Dock screen visuals

    m_pDockScreenDesc = NULL;
    m_fOwnDockScreenDesc = false;
    CXMLElement *pVisuals = pDesc->GetContentElementByTag(DOCK_SCREEN_DISPLAY_TAG);
    if (pVisuals)
        {
        m_pDockScreenDesc = new CDockScreenVisuals;
        m_fOwnDockScreenDesc = true;
        if (error = m_pDockScreenDesc->InitFromXML(Ctx, pVisuals))
            return ComposeLoadError(Ctx, Ctx.sError);
        }

    //	Load the displays data

	m_HUDDesc[hudArmor].pDesc = pDesc->GetContentElementByTag(ARMOR_DISPLAY_TAG);
	m_HUDDesc[hudReactor].pDesc = pDesc->GetContentElementByTag(REACTOR_DISPLAY_TAG);
	m_HUDDesc[hudShields].pDesc = pDesc->GetContentElementByTag(SHIELD_DISPLAY_TAG);
	m_HUDDesc[hudTargeting].pDesc = pDesc->GetContentElementByTag(WEAPON_DISPLAY_TAG);
	for (i = 0; i < hudCount; i++)
		{
		if (m_HUDDesc[i].pDesc)
			{
			m_HUDDesc[i].pDesc = m_HUDDesc[i].pDesc->OrphanCopy();
			m_HUDDesc[i].bOwned = true;
			}
		}

	//	Done

	return NOERROR;
	}

void CPlayerSettings::MergeFrom (const CPlayerSettings &Src)

//	MergeFrom
//
//	Merges from the source

	{
	int i;

	for (i = 0; i < hudCount; i++)
		{
        if (Src.m_HUDDesc[i].bOwned)
            {
            if (m_HUDDesc[i].bOwned)
                delete m_HUDDesc[i].pDesc;

            m_HUDDesc[i].pDesc = Src.m_HUDDesc[i].pDesc->OrphanCopy();
            m_HUDDesc[i].bOwned = true;
            }
		}

    if (Src.m_fOwnDockScreenDesc)
        {
        if (m_fOwnDockScreenDesc)
            delete m_pDockScreenDesc;

        m_pDockScreenDesc = new CDockScreenVisuals(*Src.m_pDockScreenDesc);
        m_fOwnDockScreenDesc = true;
        }
	}

void CPlayerSettings::Resolve (const CPlayerSettings *pSrc)

//  Resolve
//
//  Resolves inheritance

    {
    int i;

    //  If we have a source, we use it to inherit elements

    if (pSrc)
        {
        //  Inherit dock screen visuals

        if (!m_fOwnDockScreenDesc
                && pSrc->m_pDockScreenDesc)
            m_pDockScreenDesc = pSrc->m_pDockScreenDesc;

        //  Inherit any HUD elements

	    for (i = 0; i < hudCount; i++)
		    {
            if (!m_HUDDesc[i].bOwned
                    && pSrc->m_HUDDesc[i].pDesc)
                m_HUDDesc[i].pDesc = pSrc->m_HUDDesc[i].pDesc;
		    }
        }

    //  If we still don't have a dock screen visual, get the default.

    if (m_pDockScreenDesc == NULL)
        m_pDockScreenDesc = &CDockScreenVisuals::GetDefault();

    //  No need to resolve again, until we re-bind

    m_fResolved = true;
    }

ALERROR InitRectFromElement (CXMLElement *pItem, RECT *retRect)

//	InitRectFromElement
//
//	Initializes the rect from x, y, width, height attributes

	{
	if (pItem == NULL)
		return ERR_FAIL;

	retRect->left = pItem->GetAttributeInteger(X_ATTRIB);
	retRect->top = pItem->GetAttributeInteger(Y_ATTRIB);
	retRect->right = retRect->left + pItem->GetAttributeInteger(WIDTH_ATTRIB);
	retRect->bottom = retRect->top + pItem->GetAttributeInteger(HEIGHT_ATTRIB);

	return NOERROR;
	}

