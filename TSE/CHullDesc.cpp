//	CHullDesc.cpp
//
//	CHullDesc class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define HULL_TAG								CONSTLIT("Hull")

#define ARMOR_CRITERIA_ATTRIB					CONSTLIT("armorCriteria")
#define CARGO_SPACE_ATTRIB						CONSTLIT("cargoSpace")
#define COUNTER_INCREMENT_RATE_ATTRIB			CONSTLIT("counterIncrementRate")
#define CYBER_DEFENSE_LEVEL_ATTRIB				CONSTLIT("cyberDefenseLevel")
#define DEVICE_CRITERIA_ATTRIB					CONSTLIT("deviceCriteria")
#define HULL_VALUE_ATTRIB						CONSTLIT("hullValue")
#define MASS_ATTRIB								CONSTLIT("mass")
#define MAX_CARGO_SPACE_ATTRIB					CONSTLIT("maxCargoSpace")
#define MAX_COUNTER_ATTRIB						CONSTLIT("maxCounter")
#define MAX_DEVICES_ATTRIB						CONSTLIT("maxDevices")
#define MAX_NON_WEAPONS_ATTRIB					CONSTLIT("maxNonWeapons")
#define MAX_REACTOR_POWER_ATTRIB				CONSTLIT("maxReactorPower")
#define MAX_WEAPONS_ATTRIB						CONSTLIT("maxWeapons")
#define SIZE_ATTRIB								CONSTLIT("size")
#define TIME_STOP_IMMUNE_ATTRIB					CONSTLIT("timeStopImmune")
#define VALUE_ATTRIB							CONSTLIT("value")
#define VALUE_ADJ_ATTRIB						CONSTLIT("valueAdj")

ALERROR CHullDesc::Bind (SDesignLoadCtx &Ctx)

//	Bind
//
//	Bind design

	{
	ALERROR error;

	if (error = m_Value.Bind(Ctx))
		return error;

	return NOERROR;
	}

ALERROR CHullDesc::InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, int iMaxSpeed)

//	InitFromXML
//
//	Initializes from a ship class XML or from a <Hull> sub-element.

	{
	ALERROR error;

	CXMLElement *pHull = pDesc->GetContentElementByTag(HULL_TAG);
	if (pHull == NULL)
		pHull = pDesc;

	//	Mass and size

	m_iMass = pHull->GetAttributeInteger(MASS_ATTRIB);
	m_iSize = pHull->GetAttributeIntegerBounded(SIZE_ATTRIB, 1, -1, 0);
    m_iCargoSpace = pHull->GetAttributeIntegerBounded(CARGO_SPACE_ATTRIB, 0, -1, 0);

	//	Hull value

	CString sAttrib;
	if (pHull->FindAttribute(VALUE_ATTRIB, &sAttrib) || pHull->FindAttribute(HULL_VALUE_ATTRIB, &sAttrib))
		{
		if (error = m_Value.InitFromXML(Ctx, sAttrib))
			return error;
		}

	//	Armor and device criteria

	CString sCriteria;
	if (pHull->FindAttribute(ARMOR_CRITERIA_ATTRIB, &sCriteria))
		CItem::ParseCriteria(sCriteria, &m_ArmorCriteria);
	else
		CItem::InitCriteriaAll(&m_ArmorCriteria);

	if (pHull->FindAttribute(DEVICE_CRITERIA_ATTRIB, &sCriteria))
		CItem::ParseCriteria(sCriteria, &m_DeviceCriteria);
	else
		CItem::InitCriteriaAll(&m_DeviceCriteria);

	m_iMaxCargoSpace = pHull->GetAttributeIntegerBounded(MAX_CARGO_SPACE_ATTRIB, m_iCargoSpace, -1, m_iCargoSpace);
	m_iMaxReactorPower = pHull->GetAttributeInteger(MAX_REACTOR_POWER_ATTRIB);

	//	Armor limits

	if (error = m_ArmorLimits.InitFromXML(Ctx, pHull, iMaxSpeed))
		return error;

	//	Device limits

	m_iMaxDevices = pHull->GetAttributeIntegerBounded(MAX_DEVICES_ATTRIB, 0, -1, -1);
	m_iMaxWeapons = pHull->GetAttributeIntegerBounded(MAX_WEAPONS_ATTRIB, 0, m_iMaxDevices, m_iMaxDevices);
	m_iMaxNonWeapons = pHull->GetAttributeIntegerBounded(MAX_NON_WEAPONS_ATTRIB, 0, m_iMaxDevices, m_iMaxDevices);

	//	Miscellaneous defensive systems.

	m_bTimeStopImmune = pHull->GetAttributeBool(TIME_STOP_IMMUNE_ATTRIB);
	m_iCyberDefenseLevel = pHull->GetAttributeIntegerBounded(CYBER_DEFENSE_LEVEL_ATTRIB, 1, MAX_ITEM_LEVEL, -1);

	//	Extra points

	m_rExtraPoints = pHull->GetAttributeDouble(VALUE_ADJ_ATTRIB);

	//	In API 40 we still allow some values to be defined in the header.

	if (pHull != pDesc && Ctx.GetAPIVersion() < 41)
		{
		int iValue;
		if (m_iCyberDefenseLevel == -1 && pDesc->FindAttributeInteger(CYBER_DEFENSE_LEVEL_ATTRIB, &iValue))
			m_iCyberDefenseLevel = iValue;

		bool bValue;
		if (!m_bTimeStopImmune && pDesc->FindAttributeBool(TIME_STOP_IMMUNE_ATTRIB, &bValue))
			m_bTimeStopImmune = bValue;
		}

	// Counter limit and increment rate
	
	m_iCounterIncrementRate = pHull->GetAttributeInteger(COUNTER_INCREMENT_RATE_ATTRIB);
	m_iMaxCounter = pHull->GetAttributeInteger(MAX_COUNTER_ATTRIB);

	//	Done

	return NOERROR;
	}
