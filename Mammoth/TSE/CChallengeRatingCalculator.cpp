//	CChallengeRatingCalculator.cpp
//
//	CChallengeRatingCalculator class
//	Copyright (c) 2021 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

Metric CChallengeRatingCalculator::CalcBalance (const CStation &StationObj)

//	CalcBalance
//
//	Calculates the balance rating for the given station.

	{
	int iLevel = StationObj.GetLevel();
	if (iLevel <= 0 || iLevel > MAX_TECH_LEVEL)
		return 0.0;

	return CalcLevelStrength(StationObj, iLevel);
	}

Metric CChallengeRatingCalculator::CalcBalanceHitsAdj (const CStation &StationObj, int iLevel)

//	CalcBalanceHitsAdj
//
//	Calculate adjustments to balance based on how many hits the station can
//	withstand.

	{
	return Min(MAX_HITS_ADJ, sqrt(CalcHitsToDestroy(StationObj, iLevel) / BALANCE_STD_HITS_TO_DESTROY));
	}

int CChallengeRatingCalculator::CalcChallengeRating (const CShip &ShipObj)

//	CalcChallengeRating
//
//	An integer value representing the combat strength of the ship.

	{
	int iLevel = ShipObj.GetLevel();
	if (iLevel < 1 || iLevel > MAX_SYSTEM_LEVEL)
		return 0;

	Metric rLevelStrength = CalcLevelStrength(ShipObj, iLevel);
	Metric rChallenge = pow(rLevelStrength / BALANCE_STD_DEFENDERS_PER_STATION, TREASURE_BALANCE_POWER) * CStationType::GetStdChallenge(iLevel);
	return Max(1, mathRound(mathLog(rChallenge, CHALLENGE_RATING_K1)));
	}

int CChallengeRatingCalculator::CalcChallengeRating (const CStation &StationObj)

//	CalcChallengeRating
//
//	An integer value representing the combat strength of the station.

	{
	int iLevel = StationObj.GetLevel();
	if (iLevel < 1 || iLevel > MAX_SYSTEM_LEVEL)
		return 0;

	Metric rChallenge = pow(CalcBalance(StationObj), TREASURE_BALANCE_POWER) * CStationType::GetStdChallenge(iLevel);
	return Max(1, mathRound(mathLog(rChallenge, CHALLENGE_RATING_K1)));
	}

Metric CChallengeRatingCalculator::CalcDefenderStrength (const CStation &StationObj, int iLevel)

//	CalcDefenderStrength
//
//	Returns the strength of station defenders
//	1.0 = 1 defender of station level.

	{
	Metric rTotal = 0.0;

	for (int i = 0; i < StationObj.GetSubordinateCount(); i++)
		{
		CSpaceObject *pObj = StationObj.GetSubordinate(i);

		//	Ship?

		if (const CShip *pShip = pObj->AsShip())
			{
			rTotal += CalcLevelStrength(*pShip, iLevel);
			}

		//	Satellite?

		else if (const CStation *pSatellite = pObj->AsStation())
			{
			rTotal += CalcLevelStrength(*pSatellite, iLevel);
			}
		}

	//	Done

	return rTotal;
	}

Metric CChallengeRatingCalculator::CalcHitsToDestroy (const CStation &StationObj, int iLevel)

//	CalcHitsToDestroy
//
//	Returns the number of hits that it would take to destroy the station when 
//	hit by a standard weapon of the given level.

	{
	Metric rTotalHits = CalcHitsToDestroySingle(StationObj, iLevel);

	//	Add hits to destroy satellites

	for (int i = 0; i < StationObj.GetSubordinateCount(); i++)
		{
		CSpaceObject *pObj = StationObj.GetSubordinate(i);
		CStation *pSatellite = pObj->AsStation();
		if (!pSatellite)
			continue;

		rTotalHits += CalcHitsToDestroy(*pSatellite, iLevel);
		}

	//	DOne

	return (int)rTotalHits;
	}

Metric CChallengeRatingCalculator::CalcHitsToDestroySingle (const CStation &StationObj, int iLevel)

//	CalcHitsToDestroySingle
//
//	Returns the number of hits that it would take to destroy the station when 
//	hit by a standard weapon of the given level.

	{
	const CStationHull &Hull = StationObj.GetHull();
	const CStationHullDesc &HullDesc = StationObj.GetHullDesc();

	//	If station cannot be destroyed, then 0

	if (Hull.IsImmutable()
			|| (Hull.GetMaxHitPoints() == 0 && Hull.GetMaxStructuralHP() == 0))
		return 0.0;

	//	Compute the weapon that we want to use.

	int iDamageAdj;
	if (CArmorItem ArmorItem = HullDesc.GetArmorItem().AsArmorItem())
		iDamageAdj = ArmorItem.GetArmorClass().GetDamageAdjForWeaponLevel(iLevel);
	else
		iDamageAdj = 100;

	Metric rWeaponDamage = (Metric)CWeaponClass::GetStdDamage(iLevel);

	//	If the station is multi-hulled, then assume WMD4 and adjust weapon damage.

	if (Hull.GetHullType() != CStationHullDesc::hullSingle)
		rWeaponDamage = Max(1.0, MULTI_HULL_WEAPON_DAMAGE_ADJ * rWeaponDamage);

	//	If we have hit points, then use that. Otherwise, we use structural 
	//	points.

	int iTotalHP;
	if (Hull.GetMaxHitPoints() > 0)
		{
		iTotalHP = Hull.GetHitPoints();

		//	Adjust weapon damage for station repairs. The standard fire rate is
		//	once per 8 ticks, and the repair rate is per 30 ticks.

		if (!HullDesc.GetRegenDesc().IsEmpty())
			rWeaponDamage = Max(0.0, rWeaponDamage - (8.0 * HullDesc.GetRegenDesc().GetHPPer180(STATION_REPAIR_FREQUENCY) / 180.0));
		}
	else
		iTotalHP = Hull.GetStructuralHP();

	//	Adjust weapon damage for armor

	rWeaponDamage = iDamageAdj * rWeaponDamage / 100.0;

	//	If weapon does no damage then we can never destroy the station

	if (rWeaponDamage <= 0.0)
		return 0.0;

	//	Otherwise, divide to figure out the number of hits to destroy.

	return Max(1.0, (Metric)iTotalHP / rWeaponDamage);
	}

Metric CChallengeRatingCalculator::CalcLevelStrength (const CShip &ShipObj, int iLevel)

//	CalcLevelStrength
//
//	Returns the level strength of the given ship (relative to iLevel).
//	1.0 = ship power appropriate to level.

	{
	const CShipClass &ShipClass = ShipObj.GetClass();

	//	Compute based on the level of the ship relative to the input level.
	//
	//	LATER: This should compute strength based on actual installed items, not
	//	on ship class.

	Metric rCombatLevel = ShipClass.GetCombatStrength() / CShipClass::GetStdCombatStrength(ShipClass.GetLevel());
	Metric rTotal = rCombatLevel * ::CalcLevelDiffStrength(ShipClass.GetLevel() - iLevel);

	//	LATER: Handle escorts. This is hard right now because we don't have any
	//	pointers from a ship to its escorts. We'd have to iterate over all ships
	//	in the system (or create an inverted tree and cache it somewhere).

	//	Done

	return rTotal;
	}

Metric CChallengeRatingCalculator::CalcLevelStrength (const CStation &StationObj, int iLevel)

//	CalcLevelStrength
//
//	Returns the level strength of the given station (relative to iLevel).
//	1.0 = station with level appropriate weapon and 25 hits to destroy.

	{

	Metric rHitsAdj = CalcBalanceHitsAdj(StationObj, iLevel);

	Metric rWeaponStrength = rHitsAdj * CalcWeaponStrength(StationObj, iLevel);
	Metric rDefenderStrength = CalcDefenderStrength(StationObj, iLevel);

	return (rWeaponStrength + rDefenderStrength) / BALANCE_STD_DEFENDERS_PER_STATION;
	}

Metric CChallengeRatingCalculator::CalcWeaponStrength (const CStation &StationObj, int iLevel)

//	CalcWeaponStrength
//
//	Computes the total strength of all station weapons.
//	1.0 = level weapon at 1/4 fire rate.

	{
	const CDeviceSystem &DeviceSystem = StationObj.GetDeviceSystem();

	//	Start by adding up all weapons.

	Metric rTotal = 0.0;
	int iWeaponCount = 0;
	for (CDeviceItem DeviceItem : DeviceSystem)
		{
		if (DeviceItem.GetCategory() != itemcatWeapon
				&& DeviceItem.GetCategory() != itemcatLauncher)
			continue;

		int iDevLevel = DeviceItem.GetLevel();

		//	Adjust if directional.

		Metric rFireArcAdj = DeviceItem.GetFireArc() / 360.0;
		if (iWeaponCount != 0 && rFireArcAdj <= MULTI_WEAPON_BONUS_THRESHOLD)
			rFireArcAdj = Min(rFireArcAdj * MULTI_WEAPON_BONUS, 1.0);

		//	Lower level weapons count less; higher level weapons count more.

		rTotal += rFireArcAdj * ::CalcLevelDiffStrength(iDevLevel - iLevel);
		iWeaponCount++;
		}

	//	Adjust for fire rate. Double the fire rate means double the strength.

	rTotal *= (STD_FIRE_RATE_ADJ / (Metric)StationObj.GetFireRateAdj());

	//	Done

	return rTotal;
	}

