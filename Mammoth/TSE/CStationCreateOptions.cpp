//	CStationCreateOptions.cpp
//
//	CStationCreateOptions class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define SATELLITES_TAG					CONSTLIT("Satellites")
#define SHIPS_TAG						CONSTLIT("Ships")
#define TRADE_TAG						CONSTLIT("Trade")

#define IMAGE_VARIANT_ATTRIB			CONSTLIT("imageVariant")
#define NO_CONSTRUCTION_ATTRIB			CONSTLIT("noConstruction")
#define NO_MAP_LABEL_ATTRIB				CONSTLIT("noMapLabel")
#define NO_REINFORCEMENTS_ATTRIB		CONSTLIT("noReinforcements")
#define OBJ_NAME_ATTRIB					CONSTLIT("objName")
#define PAINT_LAYER_ATTRIB				CONSTLIT("paintLayer")
#define SHOW_MAP_LABEL_ATTRIB			CONSTLIT("showMapLabel")
#define SHOW_ORBIT_ATTRIB				CONSTLIT("showOrbit")

#define ON_CREATE_EVENT					CONSTLIT("OnCreate")

void CStationCreateOptions::CopyOnWrite (bool &bCopied, CStationCreateOptions *retCopy)

//	CopyOnWrite
//
//	Makes a copy if necessary.

	{
	if (!bCopied && retCopy)
		{
		*retCopy = *this;
		bCopied = true;
		}
	}

ALERROR CStationCreateOptions::InitFromXML (SSystemCreateCtx &Ctx, const CXMLElement &XMLDesc)

//	InitFromXML
//
//	Initializes station create options from an XML element, e.g., a <Station>
//	element in a system type definition.

	{
	//	Station name

	if (ALERROR error = m_Name.InitFromXMLRoot(SDesignLoadCtx(), &XMLDesc))
		return error;

	m_sObjName = XMLDesc.GetAttribute(OBJ_NAME_ATTRIB);

	//	Image and painting options

	m_iImageVariant = XMLDesc.GetAttributeIntegerBounded(IMAGE_VARIANT_ATTRIB, 0, -1, -1);
	m_sPaintLayer = XMLDesc.GetAttribute(PAINT_LAYER_ATTRIB);

	//	Map options

	if (XMLDesc.FindAttributeBool(SHOW_ORBIT_ATTRIB, &m_bForceMapOrbit))
		m_bSuppressMapOrbit = !m_bForceMapOrbit;

	if (XMLDesc.FindAttributeBool(SHOW_MAP_LABEL_ATTRIB, &m_bForceMapLabel))
		m_bSuppressMapLabel = !m_bForceMapLabel;
	else if (XMLDesc.GetAttributeBool(NO_MAP_LABEL_ATTRIB))
		{
		m_bSuppressMapLabel = true;
		m_bForceMapLabel = false;
		}

	//	Trade

	m_pTrade = XMLDesc.GetContentElementByTag(TRADE_TAG);

	//	Other options

	m_bSuppressConstruction = XMLDesc.GetAttributeBool(NO_CONSTRUCTION_ATTRIB);
	m_bSuppressReinforcements = XMLDesc.GetAttributeBool(NO_REINFORCEMENTS_ATTRIB);

	//	Additional objects

	m_pSatellites = XMLDesc.GetContentElementByTag(SATELLITES_TAG);
	m_pShips = XMLDesc.GetContentElementByTag(SHIPS_TAG);

	//	Events

	m_pOnCreate = XMLDesc.GetContentElementByTag(ON_CREATE_EVENT);

	//	Done

	return NOERROR;
	}

void CStationCreateOptions::Merge (const CStationCreateOptions &Src)

//	Merge
//
//	Merge the source on top of our options.

	{
	if (!Src.m_Name.IsEmpty())
		m_Name = Src.m_Name;

	if (!Src.m_sObjName.IsBlank())
		m_sObjName = Src.m_sObjName;

	if (Src.m_iImageVariant != -1)
		m_iImageVariant = Src.m_iImageVariant;

	if (!Src.m_sPaintLayer.IsBlank())
		m_sPaintLayer = Src.m_sPaintLayer;

	if (Src.m_bForceMapLabel)
		{
		m_bForceMapLabel = true;
		m_bSuppressMapLabel = false;
		}
	else if (Src.m_bSuppressMapLabel)
		{
		m_bForceMapLabel = false;
		m_bSuppressMapLabel = true;
		}

	if (Src.m_bForceMapOrbit)
		{
		m_bForceMapOrbit = true;
		m_bSuppressMapOrbit = false;
		}
	else if (Src.m_bSuppressMapOrbit)
		{
		m_bForceMapOrbit = false;
		m_bSuppressMapOrbit = true;
		}

	if (Src.m_pTrade)
		m_pTrade = Src.m_pTrade;

	if (Src.m_bSuppressConstruction)
		m_bSuppressConstruction = true;

	if (Src.m_bSuppressReinforcements)
		m_bSuppressReinforcements = true;

	if (Src.m_pSatellites)
		m_pSatellites = Src.m_pSatellites;

	if (Src.m_pShips)
		m_pShips = Src.m_pShips;

	if (Src.m_pOnCreate)
		m_pOnCreate = Src.m_pOnCreate;
	}

bool CStationCreateOptions::MergeFromSimpleXML (const CXMLElement &XMLDesc, CStationCreateOptions *retOld)

//	MergeFromSimpleXML
//
//	Merges the options in the XML with ours. If we merged anything, we return
//	TRUE and retOld is the original options.

	{
	bool bMerged = false;

	bool bValue;
	if (XMLDesc.FindAttributeBool(SHOW_ORBIT_ATTRIB, &bValue))
		{
		CopyOnWrite(bMerged, retOld);
		m_bForceMapOrbit = bValue;
		m_bSuppressMapOrbit = !bValue;
		}

	if (XMLDesc.FindAttributeBool(SHOW_MAP_LABEL_ATTRIB, &bValue))
		{
		CopyOnWrite(bMerged, retOld);
		m_bForceMapLabel = bValue;
		m_bSuppressMapLabel = !bValue;
		}
	else if (XMLDesc.FindAttributeBool(NO_MAP_LABEL_ATTRIB, &bValue) && bValue)
		{
		CopyOnWrite(bMerged, retOld);
		m_bForceMapLabel = false;
		m_bSuppressMapLabel = true;
		}

	return bMerged;
	}

