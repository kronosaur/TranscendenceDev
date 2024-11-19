//	CHullDesc.cpp
//
//	CHullDesc class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define ARMOR_LIMITS_TAG						CONSTLIT("ArmorLimits")
#define HULL_TAG								CONSTLIT("Hull")

#define CARGO_SPACE_ATTRIB						CONSTLIT("cargoSpace")
#define HEAT_INCREMENT_RATE_ATTRIB			CONSTLIT("heatIncrementRate")
#define CYBER_DEFENSE_LEVEL_ATTRIB				CONSTLIT("cyberDefenseLevel")
#define DEVICE_CRITERIA_ATTRIB					CONSTLIT("deviceCriteria")
#define HULL_VALUE_ATTRIB						CONSTLIT("hullValue")
#define LIFE_SUPPORT_POWER_USER_ATTRIB			CONSTLIT("lifeSupportPowerUse")
#define MASS_ATTRIB								CONSTLIT("mass")
#define MAX_CARGO_SPACE_ATTRIB					CONSTLIT("maxCargoSpace")
#define MAX_HEAT_ATTRIB						CONSTLIT("maxHeat")
#define MAX_DEVICES_ATTRIB						CONSTLIT("maxDevices")
#define MAX_NON_WEAPONS_ATTRIB					CONSTLIT("maxNonWeapons")
#define MAX_LAUNCHERS_ATTRIB					CONSTLIT("maxLaunchers")
#define MAX_REACTOR_POWER_ATTRIB				CONSTLIT("maxReactorPower")
#define MAX_WEAPONS_ATTRIB						CONSTLIT("maxWeapons")
#define SIZE_ATTRIB								CONSTLIT("size")
#define STEALTH_ADJ_ATTRIB						CONSTLIT("stealthAdj")
#define STEALTH_ADJ_AT_MAX_HEAT_ATTRIB			CONSTLIT("stealthAdjAtMaxHeat")
#define TIME_STOP_IMMUNE_ATTRIB					CONSTLIT("timeStopImmune")
#define VALUE_ATTRIB							CONSTLIT("value")
#define VALUE_ADJ_ATTRIB						CONSTLIT("valueAdj")

void CHullDesc::AdjustDamage (SDamageCtx &Ctx) const

//	AdjustDamage
//
//	Adjusts damage to deal with intrinsic immunities.

	{
	if (Ctx.IsTimeStopped() && IsImmuneTo(specialTimeStop))
		Ctx.SetTimeStopped(false);
	}

ALERROR CHullDesc::Bind (SDesignLoadCtx &Ctx)

//	Bind
//
//	Bind design

	{
	ALERROR error;

	if (error = m_Value.Bind(Ctx))
		return error;

	if (error = m_ArmorLimits.Bind(Ctx))
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
	m_iLifeSupportPowerUse = pHull->GetAttributeIntegerBounded(LIFE_SUPPORT_POWER_USER_ATTRIB, 0, -1, CPowerConsumption::DEFAULT_LIFESUPPORT_POWER_USE);

	//	Hull value

	CString sAttrib;
	if (pHull->FindAttribute(VALUE_ATTRIB, &sAttrib) || pHull->FindAttribute(HULL_VALUE_ATTRIB, &sAttrib))
		{
		if (error = m_Value.InitFromXML(Ctx, sAttrib))
			return error;
		}

	//	Device criteria

	m_DeviceCriteria.Init(pHull->GetAttribute(DEVICE_CRITERIA_ATTRIB), CItemCriteria::ALL);

	//	Always allow virtual items, because we often use them and we don't want
	//	to force the modder to always include them in the criteria.

	m_DeviceCriteria.SetExcludeVirtual(false);

	//	Maximums

	m_iMaxCargoSpace = pHull->GetAttributeIntegerBounded(MAX_CARGO_SPACE_ATTRIB, m_iCargoSpace, -1, m_iCargoSpace);
	m_iMaxReactorPower = pHull->GetAttributeInteger(MAX_REACTOR_POWER_ATTRIB);

	//	Device limits

	m_iMaxDevices = pHull->GetAttributeIntegerBounded(MAX_DEVICES_ATTRIB, 0, -1, -1);
	m_iMaxWeapons = pHull->GetAttributeIntegerBounded(MAX_WEAPONS_ATTRIB, 0, m_iMaxDevices, m_iMaxDevices);
	m_iMaxNonWeapons = pHull->GetAttributeIntegerBounded(MAX_NON_WEAPONS_ATTRIB, 0, m_iMaxDevices, m_iMaxDevices);
	m_iMaxLaunchers = pHull->GetAttributeIntegerBounded(MAX_LAUNCHERS_ATTRIB, 0, m_iMaxDevices, 1);

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
	
	m_iHeatIncrementRate = pHull->GetAttributeInteger(HEAT_INCREMENT_RATE_ATTRIB);
	m_iMaxHeat = pHull->GetAttributeInteger(MAX_HEAT_ATTRIB);
	m_iStealthAdj = pHull->GetAttributeInteger(STEALTH_ADJ_ATTRIB);
	m_iStealthAdjAtMaxHeat = pHull->GetAttributeInteger(STEALTH_ADJ_AT_MAX_HEAT_ATTRIB);

	//	If we've got our own element (<Hull>) then parse any children

	if (pHull != pDesc)
		{
		for (int i = 0; i < pHull->GetContentElementCount(); i++)
			{
			CXMLElement *pItem = pHull->GetContentElement(i);

			if (strEquals(pItem->GetTag(), ARMOR_LIMITS_TAG))
				{
				if (error = m_ArmorLimits.InitArmorLimitsFromXML(Ctx, pItem))
					return error;
				}
			}
		}

	//	Armor limits. We load this AFTER we've check to see if we have <ArmorLimits>
	//	because then we won't need to load the old-style armor checks.

	if (error = m_ArmorLimits.InitFromXML(Ctx, pHull, m_iMass, iMaxSpeed))
		return error;

	//	Done

	return NOERROR;
	}

bool CHullDesc::IsImmuneTo (SpecialDamageTypes iSpecialDamage) const

//	IsImmuneTo
//
//	Returns TRUE if we're immune to the given condition.

	{
	switch (iSpecialDamage)
		{
		case specialTimeStop:
			return m_bTimeStopImmune;

		default:
			return false;
		}
	}
