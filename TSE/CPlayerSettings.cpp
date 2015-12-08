//	CPlayerSettings.cpp
//
//	CPlayerSettings class
//	Copyright (c) 2011 by George Moromisato. All Rights Reserved.

#include "PreComp.h"

#define ARMOR_DISPLAY_TAG						CONSTLIT("ArmorDisplay")
#define ARMOR_SECTION_TAG						CONSTLIT("ArmorSection")
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

CPlayerSettings &CPlayerSettings::operator= (const CPlayerSettings &Source)

//	CPlayerSettings operator =

	{
	CleanUp();

	m_sDesc = Source.m_sDesc;
	m_dwLargeImage = Source.m_dwLargeImage;
	m_iSortOrder = Source.m_iSortOrder;

	//	Miscellaneous

	m_StartingCredits = Source.m_StartingCredits;		//	Starting credits
	m_sStartNode = Source.m_sStartNode;						//	Starting node (may be blank)
	m_sStartPos = Source.m_sStartPos;						//	Label of starting position (may be blank)
	m_pShipScreen = Source.m_pShipScreen;			//	Ship screen
	m_pDockServicesScreen = Source.m_pDockServicesScreen;
	m_pShipConfigScreen = Source.m_pShipConfigScreen;
	m_HullValue = Source.m_HullValue;

	//	Armor

	m_fOwnArmorDesc = Source.m_fOwnArmorDesc;
	if (m_fOwnArmorDesc)
		m_pArmorDesc = Source.m_pArmorDesc->OrphanCopy();
	else
		m_pArmorDesc = Source.m_pArmorDesc;

	//	Shields

	m_fOwnShieldDesc = Source.m_fOwnShieldDesc;
	if (m_fOwnShieldDesc)
		m_pShieldDesc = Source.m_pShieldDesc->OrphanCopy();
	else
		m_pShieldDesc = Source.m_pShieldDesc;

	//	Weapons

	m_fOwnWeaponDesc = Source.m_fOwnWeaponDesc;
	if (m_fOwnWeaponDesc)
		m_pWeaponDesc = Source.m_pWeaponDesc->OrphanCopy();
	else
		m_pWeaponDesc = Source.m_pWeaponDesc;

	//	Reactor

	m_fHasReactorDesc = Source.m_fHasReactorDesc;
	m_ReactorDesc = Source.m_ReactorDesc;

	//	Flags

	m_fInitialClass = Source.m_fInitialClass;					//	Use ship class at game start
	m_fDebug = Source.m_fDebug;
	m_fIncludeInAllAdventures = Source.m_fIncludeInAllAdventures;

	//	Don

	return *this;
	}

void CPlayerSettings::AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed)

//	AddTypesUsed
//
//	Adds types used

	{
	retTypesUsed->SetAt(m_dwLargeImage, true);
	retTypesUsed->SetAt(strToInt(m_pShipScreen.GetUNID(), 0), true);
	retTypesUsed->SetAt(strToInt(m_pDockServicesScreen.GetUNID(), 0), true);
	retTypesUsed->SetAt(strToInt(m_pShipConfigScreen.GetUNID(), 0), true);

	//	LATER: Add armor images, etc.
	}

ALERROR CPlayerSettings::Bind (SDesignLoadCtx &Ctx, CShipClass *pClass)

//	Bind
//
//	Bind design

	{
	DEBUG_TRY

	ALERROR error;

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

	//	Armor display

	if (!m_fOwnArmorDesc
			&& (m_pArmorDesc = pClass->GetArmorDescInherited()) == NULL)
		return ComposeLoadError(Ctx, ERR_ARMOR_DISPLAY_NEEDED);

	//	Shields

	if (!m_fOwnShieldDesc
			&& (m_pShieldDesc = pClass->GetShieldDescInherited()) == NULL)
		return ComposeLoadError(Ctx, ERR_SHIELD_DISPLAY_NEEDED);

	//	Reactor

	if (m_fHasReactorDesc)
		{
		if (error = m_ReactorDesc.ReactorImage.OnDesignLoadComplete(Ctx))
			return error;

		if (error = m_ReactorDesc.PowerLevelImage.OnDesignLoadComplete(Ctx))
			return error;

		if (error = m_ReactorDesc.FuelLevelImage.OnDesignLoadComplete(Ctx))
			return error;

		if (error = m_ReactorDesc.FuelLowLevelImage.OnDesignLoadComplete(Ctx))
			return error;
		}
	else if (m_pReactorDescInherited = pClass->GetReactorDescInherited())
		;
	else
		return ComposeLoadError(Ctx, ERR_REACTOR_DISPLAY_NEEDED);

	//	Weapon

	if (!m_fOwnWeaponDesc)
		m_pWeaponDesc = pClass->GetWeaponDescInherited();

	//	Done

	return NOERROR;

	DEBUG_CATCH
	}

void CPlayerSettings::CleanUp (void)

//	CleanUp
//
//	Clean up our structures

	{
	if (m_fOwnArmorDesc)
		{
		delete m_pArmorDesc;
		m_pArmorDesc = NULL;
		}

	if (m_fOwnShieldDesc)
		{
		delete m_pShieldDesc;
		m_pShieldDesc = NULL;
		}

	if (m_fOwnWeaponDesc)
		{
		delete m_pWeaponDesc;
		m_pWeaponDesc = NULL;
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

CEffectCreator *CPlayerSettings::FindEffectCreator (const CString &sUNID)

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

	//	Figure out what

	switch (*pPos)
		{
		//	No longer needed
#if 0
		case 's':
			return m_ShieldDesc.pShieldEffect;
#endif

		default:
			return NULL;
		}
	}

ALERROR CPlayerSettings::InitFromXML (SDesignLoadCtx &Ctx, CShipClass *pClass, CXMLElement *pDesc)

//	InitFromXML
//
//	Initialize from an XML element

	{
	ALERROR error;

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

	m_fHasReactorDesc = false;

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

	//	Load the armor display data

	m_pArmorDesc = pDesc->GetContentElementByTag(ARMOR_DISPLAY_TAG);
	if (m_pArmorDesc)
		{
		m_pArmorDesc = m_pArmorDesc->OrphanCopy();
		m_fOwnArmorDesc = true;
		}

	//	Load shield display data

	m_pShieldDesc = pDesc->GetContentElementByTag(SHIELD_DISPLAY_TAG);
	if (m_pShieldDesc)
		{
		m_pShieldDesc = m_pShieldDesc->OrphanCopy();
		m_fOwnShieldDesc = true;
		}

	//	Load reactor display data

	CXMLElement *pReactorDisplay = pDesc->GetContentElementByTag(REACTOR_DISPLAY_TAG);
	if (pReactorDisplay)
		{
		if (error = m_ReactorDesc.ReactorImage.InitFromXML(Ctx,
				pReactorDisplay->GetContentElementByTag(IMAGE_TAG)))
			return ComposeLoadError(Ctx, ERR_REACTOR_DISPLAY_NEEDED);

		CXMLElement *pImage = pReactorDisplay->GetContentElementByTag(POWER_LEVEL_IMAGE_TAG);
		if (pImage == NULL || (error = m_ReactorDesc.PowerLevelImage.InitFromXML(Ctx, pImage)))
			return ComposeLoadError(Ctx, ERR_REACTOR_DISPLAY_NEEDED);

		m_ReactorDesc.xPowerLevelImage = pImage->GetAttributeInteger(DEST_X_ATTRIB);
		m_ReactorDesc.yPowerLevelImage = pImage->GetAttributeInteger(DEST_Y_ATTRIB);

		pImage = pReactorDisplay->GetContentElementByTag(FUEL_LEVEL_IMAGE_TAG);
		if (pImage == NULL || (error = m_ReactorDesc.FuelLevelImage.InitFromXML(Ctx, pImage)))
			return ComposeLoadError(Ctx, ERR_REACTOR_DISPLAY_NEEDED);

		m_ReactorDesc.xFuelLevelImage = pImage->GetAttributeInteger(DEST_X_ATTRIB);
		m_ReactorDesc.yFuelLevelImage = pImage->GetAttributeInteger(DEST_Y_ATTRIB);

		pImage = pReactorDisplay->GetContentElementByTag(FUEL_LOW_LEVEL_IMAGE_TAG);
		if (pImage == NULL || (error = m_ReactorDesc.FuelLowLevelImage.InitFromXML(Ctx, pImage)))
			return ComposeLoadError(Ctx, ERR_REACTOR_DISPLAY_NEEDED);

		if (error = InitRectFromElement(pReactorDisplay->GetContentElementByTag(REACTOR_TEXT_TAG),
				&m_ReactorDesc.rcReactorText))
			return ComposeLoadError(Ctx, ERR_REACTOR_DISPLAY_NEEDED);

		if (error = InitRectFromElement(pReactorDisplay->GetContentElementByTag(POWER_LEVEL_TEXT_TAG),
				&m_ReactorDesc.rcPowerLevelText))
			return ComposeLoadError(Ctx, ERR_REACTOR_DISPLAY_NEEDED);

		if (error = InitRectFromElement(pReactorDisplay->GetContentElementByTag(FUEL_LEVEL_TEXT_TAG),
				&m_ReactorDesc.rcFuelLevelText))
			return ComposeLoadError(Ctx, ERR_REACTOR_DISPLAY_NEEDED);

		m_fHasReactorDesc = true;
		}

	//	Load weapon display data

	m_pWeaponDesc = pDesc->GetContentElementByTag(WEAPON_DISPLAY_TAG);
	if (m_pWeaponDesc)
		{
		m_pWeaponDesc = m_pWeaponDesc->OrphanCopy();
		m_fOwnWeaponDesc = true;
		}

	//	Done

	return NOERROR;
	}

void CPlayerSettings::MergeFrom (const CPlayerSettings &Src)

//	MergeFrom
//
//	Merges from the source

	{
	if (Src.m_pArmorDesc)
		{
		if (m_fOwnArmorDesc)
			delete m_pArmorDesc;

		m_pArmorDesc = Src.m_pArmorDesc;
		m_fOwnArmorDesc = false;
		}

	if (Src.m_pShieldDesc)
		{
		if (m_fOwnShieldDesc)
			delete m_pShieldDesc;

		m_pShieldDesc = Src.m_pShieldDesc;
		m_fOwnShieldDesc = false;
		}

	if (Src.m_fHasReactorDesc)
		{
		m_ReactorDesc = Src.m_ReactorDesc;
		m_fHasReactorDesc = true;
		}

	if (Src.m_pWeaponDesc)
		{
		if (m_fOwnWeaponDesc)
			delete m_pWeaponDesc;

		m_pWeaponDesc = Src.m_pWeaponDesc;
		m_fOwnWeaponDesc = false;
		}
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

