//	CArmorLimits.cpp
//
//	CArmorLimits class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define MAX_ARMOR_ATTRIB						CONSTLIT("maxArmor")
#define MAX_ARMOR_SPEED_ATTRIB					CONSTLIT("maxArmorSpeed")
#define MAX_ARMOR_SPEED_PENALTY_ATTRIB			CONSTLIT("maxArmorSpeedAdj")
#define MIN_ARMOR_SPEED_ATTRIB					CONSTLIT("minArmorSpeed")
#define MIN_ARMOR_SPEED_BONUS_ATTRIB			CONSTLIT("minArmorSpeedAdj")
#define STD_ARMOR_ATTRIB						CONSTLIT("stdArmor")

int CArmorLimits::CalcArmorSpeedBonus (int iSegmentCount, int iTotalArmorMass) const

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

ICCItem *CArmorLimits::CalcMaxSpeedByArmorMass (CCodeChainCtx &Ctx, int iStdSpeed) const

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

int CArmorLimits::CalcMinArmorMassForSpeed (int iSpeed, int iStdSpeed) const

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

void CArmorLimits::InitDefaultArmorLimits (int iMass, int iMaxSpeed, Metric rThrustRatio)

//	InitDefaultArmorLimits
//
//	If no armor limits are specified, we initialize them here based on mass, 
//	speed, and thrust

	{
	//	If we're 1000 tons or more, then no limits

	if (iMass >= 1000)
		return;

	//	Compute the heaviest segment of armor we can install.

	const Metric MAX_ARMOR_POWER = 0.7;
	const Metric MAX_ARMOR_FACTOR = 0.6;
	const Metric STD_THRUST_RATIO = 7.0;
	const int MAX_ARMOR_MAX = 50;

	int iMaxArmorTons = Min(MAX_ARMOR_MAX, mathRound(MAX_ARMOR_FACTOR * pow((Metric)iMass, MAX_ARMOR_POWER) * Max(1.0, rThrustRatio / STD_THRUST_RATIO)));
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

ALERROR CArmorLimits::InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, int iMaxSpeed)

//	InitFromXML
//
//	Initializes from <Hull> element.

	{
	m_iMaxArmorMass = pDesc->GetAttributeInteger(MAX_ARMOR_ATTRIB);
	m_iStdArmorMass = pDesc->GetAttributeIntegerBounded(STD_ARMOR_ATTRIB, 0, m_iMaxArmorMass, m_iMaxArmorMass / 2);

	//	Speed bonus/penalty

	int iValue;
	if (pDesc->FindAttributeInteger(MAX_ARMOR_SPEED_PENALTY_ATTRIB, &iValue))
		m_iMaxArmorSpeedPenalty = Max(Min(iValue, 0), -iMaxSpeed);
	else
		m_iMaxArmorSpeedPenalty = pDesc->GetAttributeIntegerBounded(MAX_ARMOR_SPEED_ATTRIB, 0, iMaxSpeed, iMaxSpeed) - iMaxSpeed;

	if (pDesc->FindAttributeInteger(MIN_ARMOR_SPEED_BONUS_ATTRIB, &iValue))
		m_iMinArmorSpeedBonus = Min(Max(0, iValue), 100 - iMaxSpeed);
	else
		m_iMinArmorSpeedBonus = pDesc->GetAttributeIntegerBounded(MIN_ARMOR_SPEED_ATTRIB, iMaxSpeed, 100, iMaxSpeed) - iMaxSpeed;

	return NOERROR;
	}
