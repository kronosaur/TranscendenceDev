//	CHullDesc.cpp
//
//	CHullDesc class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define HULL_TAG								CONSTLIT("Hull")

#define ARMOR_CRITERIA_ATTRIB					CONSTLIT("armorCriteria")
#define CARGO_SPACE_ATTRIB						CONSTLIT("cargoSpace")
#define DEVICE_CRITERIA_ATTRIB					CONSTLIT("deviceCriteria")
#define HULL_VALUE_ATTRIB						CONSTLIT("hullValue")
#define MASS_ATTRIB								CONSTLIT("mass")
#define MAX_ARMOR_ATTRIB						CONSTLIT("maxArmor")
#define MAX_ARMOR_SPEED_ATTRIB					CONSTLIT("maxArmorSpeed")
#define MAX_CARGO_SPACE_ATTRIB					CONSTLIT("maxCargoSpace")
#define MAX_DEVICES_ATTRIB						CONSTLIT("maxDevices")
#define MAX_NON_WEAPONS_ATTRIB					CONSTLIT("maxNonWeapons")
#define MAX_REACTOR_POWER_ATTRIB				CONSTLIT("maxReactorPower")
#define MAX_WEAPONS_ATTRIB						CONSTLIT("maxWeapons")
#define MIN_ARMOR_SPEED_ATTRIB					CONSTLIT("minArmorSpeed")
#define SIZE_ATTRIB								CONSTLIT("size")
#define STD_ARMOR_ATTRIB						CONSTLIT("stdArmor")
#define VALUE_ATTRIB							CONSTLIT("value")

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

int CHullDesc::CalcArmorSpeedBonus (int iSegmentCount, int iTotalArmorMass) const

//	CalcArmorSpeedBonus
//
//	Compute the speed bonus based on the current armor.

	{
	int iStdTotalArmorMass = m_iStdArmorMass * iSegmentCount;

	//	Speed pentalty

	if (iTotalArmorMass >= iStdTotalArmorMass)
		{
		if (m_iMaxArmorSpeedPenalty < 0 
				&& m_iMaxArmorMass > m_iStdArmorMass)
			{
			int iMaxTotalArmorMass = m_iMaxArmorMass * iSegmentCount;
			int iRange = iMaxTotalArmorMass - iStdTotalArmorMass;
			int iMassPerTick = iRange / (1 - m_iMaxArmorSpeedPenalty);
			if (iMassPerTick <= 0)
				return 0;

			int iTicks = (iTotalArmorMass - iStdTotalArmorMass) / iMassPerTick;
			return Max(-iTicks, m_iMaxArmorSpeedPenalty);
			}
		else
			return 0;
		}

	//	Speed bonus

	else
		{
		if (m_iMinArmorSpeedBonus > 0)
			{
			int iMinTotalArmorMass = m_iStdArmorMass * iSegmentCount / 2;
			int iRange = iStdTotalArmorMass - iMinTotalArmorMass;
			int iMassPerTick = iRange / m_iMinArmorSpeedBonus;
			if (iMassPerTick <= 0)
				return 0;

			int iTicks = (iStdTotalArmorMass - iTotalArmorMass) / iMassPerTick;
			return Min(iTicks, m_iMinArmorSpeedBonus);
			}
		else
			return 0;
		}
	}

ICCItem *CHullDesc::CalcMaxSpeedByArmorMass (CCodeChainCtx &Ctx, int iStdSpeed) const

//	CalcMaxSpeedByArmorMass
//
//	Returns a struct with entries for each value of max speed. Each entry has the
//	smallest armor mass which results in the given speed.
//
//	If there is no variation in speed, we return a single speed value.

	{
	int i;

	CCodeChain &CC = g_pUniverse->GetCC();
	ICCItem *pResult = CC.CreateSymbolTable();

	//	If we don't change speed based on armor mass, then we just return one speed.

	if (m_iMaxArmorMass == 0 || (m_iMaxArmorSpeedPenalty == 0 && m_iMinArmorSpeedBonus == 0))
		pResult->SetAt(CC, strFromInt(iStdSpeed), CC.CreateNil());

	//	Otherwise, loop over every speed.

	else
		{
		int iMinSpeed = iStdSpeed + m_iMaxArmorSpeedPenalty;
		int iMaxSpeed = iStdSpeed + m_iMinArmorSpeedBonus;

		for (i = iMinSpeed; i <= iMaxSpeed; i++)
			{
			CString sLine;

			if (i == iMinSpeed)
				sLine = strPatternSubst(CONSTLIT("%d-%d"), CalcMinArmorMassForSpeed(i, iStdSpeed), m_iMaxArmorMass);
			else if (i == iMaxSpeed)
				{
				if (i == iStdSpeed && i > iMinSpeed)
					sLine = strPatternSubst(CONSTLIT("0-%d"), CalcMinArmorMassForSpeed(i - 1, iStdSpeed) - 1);
				else
					sLine = strPatternSubst(CONSTLIT("0-%d"), CalcMinArmorMassForSpeed(i, iStdSpeed));
				}
			else if (i > iStdSpeed)
				sLine = strPatternSubst(CONSTLIT("%d-%d"), CalcMinArmorMassForSpeed(i + 1, iStdSpeed) + 1, CalcMinArmorMassForSpeed(i, iStdSpeed));
			else
				sLine = strPatternSubst(CONSTLIT("%d-%d"), CalcMinArmorMassForSpeed(i, iStdSpeed), CalcMinArmorMassForSpeed(i - 1, iStdSpeed) - 1);

			pResult->SetStringAt(CC, strFromInt(i), sLine);
			}
		}

	return pResult;
	}

int CHullDesc::CalcMinArmorMassForSpeed (int iSpeed, int iStdSpeed) const

//	CalcMinArmorMassForSpeed
//
//	Returns the smallest armor mass that is compatible with the given speed.

	{
	int iMinSpeed = iStdSpeed + m_iMaxArmorSpeedPenalty;
	int iMaxSpeed = iStdSpeed + m_iMinArmorSpeedBonus;

	int iPenaltyRange = m_iMaxArmorMass - m_iStdArmorMass;
	int iPenaltyMassPerPoint = iPenaltyRange / (1 - m_iMaxArmorSpeedPenalty);

	int iMinArmorMass = m_iStdArmorMass / 2;
	int iBonusRange = m_iStdArmorMass - iMinArmorMass;
	int iBonusMassPerPoint = (m_iMinArmorSpeedBonus > 0 ? iBonusRange / m_iMinArmorSpeedBonus : 0);

	if (iSpeed < iStdSpeed)
		{
		int iDiff = iStdSpeed - iSpeed;
		return m_iStdArmorMass + (iPenaltyMassPerPoint * iDiff);
		}
	else if (iSpeed == iStdSpeed)
		{
		if (iMinSpeed == iMaxSpeed)
			return m_iStdArmorMass;
		else
			return (m_iStdArmorMass - iBonusMassPerPoint) + 1;
		}
	else
		{
		int iDiff = iSpeed - iStdSpeed;
		return m_iStdArmorMass - (iBonusMassPerPoint * iDiff);
		}
	}

void CHullDesc::InitDefaultArmorLimits (int iMaxSpeed, Metric rThrustRatio)

//	InitDefaultArmorLimits
//
//	If no armor limits are specified, we initialize them here based on mass, 
//	speed, and thrust

	{
	//	If we're more than 1000 tons, then no limits

	if (m_iMass > 1000)
		return;

	//	Compute the heaviest segment of armor we can install.

	const Metric MAX_ARMOR_POWER = 0.7;
	const Metric MAX_ARMOR_FACTOR = 0.6;
	const Metric STD_THRUST_RATIO = 7.0;
	const int MAX_ARMOR_MAX = 50;

	int iMaxArmorTons = Min(MAX_ARMOR_MAX, mathRound(MAX_ARMOR_FACTOR * pow((Metric)m_iMass, MAX_ARMOR_POWER) * Max(1.0, rThrustRatio / STD_THRUST_RATIO)));
	m_iMaxArmorMass = 1000 * iMaxArmorTons;

	//	Compute the mass of standard armor

	const Metric STD_ARMOR_POWER = 0.8;
	const Metric STD_ARMOR_FACTOR = 0.8;

	int iStdArmorTons = mathRound(STD_ARMOR_FACTOR * pow((Metric)iMaxArmorTons, STD_ARMOR_POWER));
	m_iStdArmorMass = 1000 * iStdArmorTons;

	//	Compute the max speed at maximum armor

	const Metric MAX_ARMOR_SPEED_ADJ = 0.1;

	int iSpeedDec = mathRound((Metric)iMaxSpeed * MAX_ARMOR_SPEED_ADJ);
	m_iMaxArmorSpeedPenalty = -iSpeedDec;

	//	Compute the max speed at minimum armor

	const int MIN_ARMOR_SPEED_OFFSET = 26;
	const Metric MIN_ARMOR_SPEED_ADJ = 0.25;
	const Metric MIN_ARMOR_THRUST_ADJ = 0.5;

	Metric rThrustRatioLimit = Max(1.0, MIN_ARMOR_THRUST_ADJ * rThrustRatio);
	int iSpeedInc = Max(0, mathRound(Min(rThrustRatioLimit, (MIN_ARMOR_SPEED_OFFSET - iMaxSpeed) * MIN_ARMOR_SPEED_ADJ)));
	m_iMinArmorSpeedBonus = iSpeedInc;
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

	m_iMaxArmorMass = pHull->GetAttributeInteger(MAX_ARMOR_ATTRIB);
	m_iStdArmorMass = pHull->GetAttributeIntegerBounded(STD_ARMOR_ATTRIB, 0, m_iMaxArmorMass, m_iMaxArmorMass / 2);
	m_iMaxArmorSpeedPenalty = pHull->GetAttributeIntegerBounded(MAX_ARMOR_SPEED_ATTRIB, 0, iMaxSpeed, iMaxSpeed) - iMaxSpeed;
	m_iMinArmorSpeedBonus = pHull->GetAttributeIntegerBounded(MIN_ARMOR_SPEED_ATTRIB, iMaxSpeed, 100, iMaxSpeed) - iMaxSpeed;

	//	Device limits

	m_iMaxDevices = pHull->GetAttributeIntegerBounded(MAX_DEVICES_ATTRIB, 0, -1, -1);
	m_iMaxWeapons = pHull->GetAttributeIntegerBounded(MAX_WEAPONS_ATTRIB, 0, m_iMaxDevices, m_iMaxDevices);
	m_iMaxNonWeapons = pHull->GetAttributeIntegerBounded(MAX_NON_WEAPONS_ATTRIB, 0, m_iMaxDevices, m_iMaxDevices);

	//	Done

	return NOERROR;
	}

