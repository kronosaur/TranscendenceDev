//	CReactorDesc.cpp
//
//	CReactorDesc class
//  Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define REACTOR_TAG								CONSTLIT("Reactor")

#define FUEL_CAPACITY_ATTRIB					CONSTLIT("fuelCapacity")
#define FUEL_CRITERIA_ATTRIB					CONSTLIT("fuelCriteria")
#define FUEL_EFFICIENCY_ATTRIB					CONSTLIT("fuelEfficiency")
#define MAX_FUEL_ATTRIB							CONSTLIT("maxFuel")
#define MAX_FUEL_TECH_ATTRIB					CONSTLIT("maxFuelTech")
#define MIN_FUEL_TECH_ATTRIB					CONSTLIT("minFuelTech")
#define MAX_POWER_ATTRIB						CONSTLIT("maxPower")
#define NO_FUEL_ATTRIB							CONSTLIT("noFuel")
#define REACTOR_EFFICIENCY_ATTRIB				CONSTLIT("reactorEfficiency")
#define REACTOR_POWER_ATTRIB					CONSTLIT("reactorPower")

#define PROPERTY_FUEL_CAPACITY					CONSTLIT("fuelCapacity")
#define PROPERTY_FUEL_CAPACITY_EXACT			CONSTLIT("fuelCapacityExact")
#define PROPERTY_FUEL_CRITERIA					CONSTLIT("fuelCriteria")
#define PROPERTY_FUEL_EFFICIENCY				CONSTLIT("fuelEfficiency")
#define PROPERTY_FUEL_EFFICIENCY_BONUS			CONSTLIT("fuelEfficiencyBonus")
#define PROPERTY_POWER							CONSTLIT("power")

CReactorDesc::SStdStats CReactorDesc::m_Stats[MAX_ITEM_LEVEL] = 
    {
        //  Max         Fuel        Fuel
        //  Power       Density     Cost
        {   50,          0.5,        1.0 },
        {   100,         1.0,        1.1 },
        {   250,         2.5,        1.2 },
        {   500,         5.0,        1.3 },
        {   1000,       10.6,        1.5 },

        {   1500,       15.0,        1.6 },
        {   2500,       25.0,        1.8 },
        {   5000,       50.0,        1.9 },
        {   10000,     100.0,        2.1 },
        {   15000,     150.0,        2.4 },

        {   25000,     250.0,        2.6 },
        {   40000,     400.0,        2.9 },
        {   60000,     600.0,        3.1 },
        {   90000,     900.0,        3.5 },
        {   140000,   1400.0,        3.8 },

        {   200000,   2000.0,        4.2 },
        {   300000,   3000.0,        4.6 },
        {   450000,   4500.0,        5.1 },
        {   600000,   6000.0,        5.6 },
        {   800000,   8000.0,        6.1 },

        {   1100000, 11000.0,        6.7 },
        {   1500000, 15000.0,        7.4 },
        {   2000000, 20000.0,        8.1 },
        {   2500000, 25000.0,        9.0 },
        {   3000000, 30000.0,        9.8 },
    };

CReactorDesc::CReactorDesc (void) : 
		m_dwUNID(0),
		m_iMaxPower(0),
		m_rMaxFuel(0.0),
		m_rPowerPerFuelUnit(0.0),
		m_pFuelCriteria(NULL),
		m_iMinFuelLevel(-1),
		m_iMaxFuelLevel(-1),
		m_fFreeFuelCriteria(false),
		m_fNoFuel(false)

//	CReactorDesc constructor

	{
	}

int CReactorDesc::AdjMaxPower (Metric rAdj)

//  AdjMaxPower
//
//  Adjusts max power by the given factor

    {
    if (rAdj > 0.0)
        m_iMaxPower = mathRound(m_iMaxPower * rAdj);

    return m_iMaxPower;
    }

Metric CReactorDesc::AdjEfficiency (Metric rAdj)

//  AdjEfficiency
//
//  Adjusts efficiency by the given factor

    {
    if (rAdj > 0.0)
        m_rPowerPerFuelUnit = m_rPowerPerFuelUnit * rAdj;

    return m_rPowerPerFuelUnit;
    }

void CReactorDesc::CleanUp (void)

//  CleanUp
//
//  Free allocations

    {
	if (m_pFuelCriteria && m_fFreeFuelCriteria)
		delete m_pFuelCriteria;
    }

void CReactorDesc::Copy (const CReactorDesc &Src)

//  Copy
//
//  Make a copy. We assume that we are clean.

    {
	m_dwUNID = Src.m_dwUNID;
	m_iMaxPower = Src.m_iMaxPower;
	m_rMaxFuel = Src.m_rMaxFuel;
	m_rPowerPerFuelUnit = Src.m_rPowerPerFuelUnit;

	m_pFuelCriteria = (Src.m_pFuelCriteria ? new CItemCriteria(*Src.m_pFuelCriteria) : NULL);
	m_iMinFuelLevel = Src.m_iMinFuelLevel;
	m_iMaxFuelLevel = Src.m_iMaxFuelLevel;

	m_fNoFuel = Src.m_fNoFuel;

	m_fFreeFuelCriteria = (m_pFuelCriteria != NULL);
    }

bool CReactorDesc::FindDataField (const CString &sField, CString *retsValue) const

//  FindDataField
//
//  Returns a data field, for backwards compatibility. New code should call
//  FindProperty.

    {
    if (strEquals(sField, PROPERTY_POWER))
        *retsValue = strFromInt(GetMaxPower() * 100);
    else if (strEquals(sField, PROPERTY_FUEL_CRITERIA))
        *retsValue = GetFuelCriteriaString();
	else if (strEquals(sField, PROPERTY_FUEL_EFFICIENCY))
		*retsValue = strFromInt((int)GetEfficiency());
	else if (strEquals(sField, PROPERTY_FUEL_CAPACITY))
		*retsValue = strFromInt((int)(GetFuelCapacity() / FUEL_UNITS_PER_STD_ROD));
	else
		return false;

	return true;
    }

bool CReactorDesc::IsExportedProperty (const CString &sProperty)

//	IsExportedProperty
//
//	Returns TRUE if this property should be exported to a ship property.

	{
	return (strEquals(sProperty, PROPERTY_POWER)
			|| strEquals(sProperty, PROPERTY_FUEL_CRITERIA)
			|| strEquals(sProperty, PROPERTY_FUEL_EFFICIENCY)
			|| strEquals(sProperty, PROPERTY_FUEL_EFFICIENCY_BONUS)
			|| strEquals(sProperty, PROPERTY_FUEL_CAPACITY)
			|| strEquals(sProperty, PROPERTY_FUEL_CAPACITY_EXACT));
	}

ICCItem *CReactorDesc::FindProperty (const CString &sProperty) const

//  FindProperty
//
//  Returns the given property. If not found, return NULL. NOTE: Caller must
//  discard the result if non-NULL.

    {
	CCodeChain &CC = g_pUniverse->GetCC();

	if (strEquals(sProperty, PROPERTY_POWER))
		return CreatePowerResult(CC, 100.0 * m_iMaxPower);

	else if (strEquals(sProperty, PROPERTY_FUEL_CRITERIA))
		{
		if (m_pFuelCriteria)
			return CC.CreateString(CItem::GenerateCriteria(*m_pFuelCriteria));
		else
			return CC.CreateString(strPatternSubst(CONSTLIT("f L:%d-%d;"), m_iMinFuelLevel, m_iMaxFuelLevel));
		}
	else if (strEquals(sProperty, PROPERTY_FUEL_EFFICIENCY))
		return CC.CreateInteger(mathRound(m_rPowerPerFuelUnit));

	else if (strEquals(sProperty, PROPERTY_FUEL_EFFICIENCY_BONUS))
		return CC.CreateInteger(GetEfficiencyBonus());

    else if (strEquals(sProperty, PROPERTY_FUEL_CAPACITY))
        return CC.CreateInteger(mathRound(m_rMaxFuel / FUEL_UNITS_PER_STD_ROD));

    else if (strEquals(sProperty, PROPERTY_FUEL_CAPACITY_EXACT))
        return CC.CreateDouble(m_rMaxFuel);

    else
        return NULL;
    }

int CReactorDesc::GetEfficiencyBonus (void) const

//  GetEfficiencyBonus
//
//	Returns the efficiency of the reactor relative to the standard in percent
//	terms. We round to multiple of 5.

	{
	if (m_rPowerPerFuelUnit != g_MWPerFuelUnit)
		{
		int iBonus = (int)(100.0 * ((m_rPowerPerFuelUnit / g_MWPerFuelUnit) - 1.0));
		if (iBonus > 0)
			return 5 * ((iBonus + 2) / 5);
		else
			return 5 * ((iBonus - 2) / 5);
		}
	else
		return 0;
	}

CString CReactorDesc::GetFuelCriteriaString (void) const

//  GetFuelCriteriaString
//
//  Returns a string describing the fuel criteria for the reactor.

    {
	if (m_pFuelCriteria)
		return CItem::GenerateCriteria(*m_pFuelCriteria);
	else
		return strPatternSubst(CONSTLIT("f L:%d-%d;"), m_iMinFuelLevel, m_iMaxFuelLevel);
    }

void CReactorDesc::GetFuelLevel (int *retiMin, int *retiMax) const

//  GetFuelLevel
//
//  Returns the minimum and maximum level fuel compatible with reactor.

    {
	if (m_pFuelCriteria)
		m_pFuelCriteria->GetExplicitLevelMatched(retiMin, retiMax);
	else
		{
		*retiMin = m_iMinFuelLevel;
		*retiMax = m_iMaxFuelLevel;
		}
    }

const CReactorDesc::SStdStats &CReactorDesc::GetStdStats (int iLevel)

//  GetStdStats
//
//  Returns standard stats for the level.

    {
    ASSERT(iLevel >= 0 && iLevel <= MAX_ITEM_LEVEL);
    return m_Stats[iLevel];
    }

ALERROR CReactorDesc::InitFromShipClassXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, DWORD dwUNID)

//	InitFromShipClassXML
//
//	Initializes from a <ShipClass> element.

	{
    CleanUp();

    m_dwUNID = dwUNID;
	m_fNoFuel = false;

	//	If we have a <Reactor> element, use that.

	CXMLElement *pReactor = pDesc->GetContentElementByTag(REACTOR_TAG);
	if (pReactor)
		pDesc = pReactor;

	//	Load basic values

	m_iMaxPower = pDesc->GetAttributeIntegerBounded(REACTOR_POWER_ATTRIB, 0, -1, 0);
	m_rMaxFuel = pDesc->GetAttributeDoubleBounded(FUEL_CAPACITY_ATTRIB, 0.0, -1.0, m_iMaxPower * 250.0);
	m_rPowerPerFuelUnit = pDesc->GetAttributeDoubleBounded(REACTOR_EFFICIENCY_ATTRIB, 0.0, -1.0, g_MWPerFuelUnit);

	//	Load the fuel criteria

	CString sCriteria;
	if (pDesc->FindAttribute(FUEL_CRITERIA_ATTRIB, &sCriteria))
		{
		m_pFuelCriteria = new CItemCriteria;
		m_fFreeFuelCriteria = true;

		CItem::ParseCriteria(sCriteria, m_pFuelCriteria);

		m_iMinFuelLevel = -1;
		m_iMaxFuelLevel = -1;

		//	Warn if older method is used

		if (pDesc->AttributeExists(MIN_FUEL_TECH_ATTRIB) || pDesc->AttributeExists(MAX_FUEL_TECH_ATTRIB))
			::kernelDebugLogPattern("Warning: minFuelTech and maxFuelTech ignored if fuelCriteria specified.");
		}

	//	If we have no fuel criteria, then use the older method

	else
		{
		m_iMinFuelLevel = pDesc->GetAttributeIntegerBounded(MIN_FUEL_TECH_ATTRIB, 1, MAX_ITEM_LEVEL, 1);
		m_iMaxFuelLevel = pDesc->GetAttributeIntegerBounded(MAX_FUEL_TECH_ATTRIB, m_iMinFuelLevel, MAX_ITEM_LEVEL, 3);
		}

	return NOERROR;
	}

ALERROR CReactorDesc::InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, DWORD dwUNID, bool bShipClass)

//  InitFromXML
//
//  Initializes from an XML element

    {
    CleanUp();

    m_dwUNID = dwUNID;

	//	Load some values

	if (bShipClass)
		{
		m_iMaxPower = pDesc->GetAttributeIntegerBounded(REACTOR_POWER_ATTRIB, 0, -1, 0);
		m_rMaxFuel = pDesc->GetAttributeDoubleBounded(FUEL_CAPACITY_ATTRIB, 0.0, -1.0, m_iMaxPower * 250.0);
		m_rPowerPerFuelUnit = pDesc->GetAttributeDoubleBounded(REACTOR_EFFICIENCY_ATTRIB, 0.0, -1.0, g_MWPerFuelUnit);

		m_fNoFuel = false;
		}
	else
		{
		m_iMaxPower = pDesc->GetAttributeIntegerBounded(MAX_POWER_ATTRIB, 0, -1, 100);
		m_rMaxFuel = pDesc->GetAttributeDoubleBounded(MAX_FUEL_ATTRIB, 0.0, -1.0, m_iMaxPower * 250.0);
		m_rPowerPerFuelUnit = pDesc->GetAttributeDoubleBounded(FUEL_EFFICIENCY_ATTRIB, 0.0, -1.0, g_MWPerFuelUnit);

		//	Load some options (only valid for reactor items)

		m_fNoFuel = pDesc->GetAttributeBool(NO_FUEL_ATTRIB);
		}

	//	Load the fuel criteria

	CString sCriteria;
	if (pDesc->FindAttribute(FUEL_CRITERIA_ATTRIB, &sCriteria))
		{
		m_pFuelCriteria = new CItemCriteria;
		m_fFreeFuelCriteria = true;

		CItem::ParseCriteria(sCriteria, m_pFuelCriteria);

		m_iMinFuelLevel = -1;
		m_iMaxFuelLevel = -1;

		//	Warn if older method is used

		if (pDesc->AttributeExists(MIN_FUEL_TECH_ATTRIB) || pDesc->AttributeExists(MAX_FUEL_TECH_ATTRIB))
			::kernelDebugLogPattern("Warning: minFuelTech and maxFuelTech ignored if fuelCriteria specified.");
		}

	//	If we have no fuel criteria, then use the older method

	else
		{
		m_iMinFuelLevel = pDesc->GetAttributeIntegerBounded(MIN_FUEL_TECH_ATTRIB, 1, MAX_ITEM_LEVEL, 1);
		m_iMaxFuelLevel = pDesc->GetAttributeIntegerBounded(MAX_FUEL_TECH_ATTRIB, m_iMinFuelLevel, MAX_ITEM_LEVEL, (bShipClass ? 3 : MAX_ITEM_LEVEL));
		}

	return NOERROR;
    }

ALERROR CReactorDesc::InitScaled (SDesignLoadCtx &Ctx, const CReactorDesc &Src, int iBaseLevel, int iScaledLevel)

//  InitScaled
//
//  Initializes as a scaled version of Src.

    {
    *this = Src;
    if (iBaseLevel > iScaledLevel)
        return NOERROR;

    const SStdStats &BaseStats = GetStdStats(iBaseLevel);
    const SStdStats &ScaledStats = GetStdStats(iScaledLevel);

    //  We increase power output proportionally to scale

    Metric rPowerAdj = (Metric)ScaledStats.iMaxPower / (Metric)BaseStats.iMaxPower;
    m_iMaxPower = mathRound(m_iMaxPower * rPowerAdj);

    //  We increase fuel capacity in the same way.

    m_rMaxFuel = m_rMaxFuel * rPowerAdj;

    //  Done

    return NOERROR;
    }

bool CReactorDesc::IsFuelCompatible (const CItem &FuelItem) const

//  IsFuelCompatible
//
//  Returns TRUE if the given fuel item is compatible with us.

    {
	if (m_pFuelCriteria)
		return FuelItem.MatchesCriteria(*m_pFuelCriteria);
	else
		{
		//	Must be fuel

		if (!FuelItem.GetType()->IsFuel())
			return false;

		//	Make sure we're the correct level

		int iLevel = FuelItem.GetLevel();
		return (iLevel >= m_iMinFuelLevel 
				&& iLevel <= m_iMaxFuelLevel);
		}
    }
