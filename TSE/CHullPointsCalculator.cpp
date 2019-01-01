//	CHullPointsCalculator.cpp
//
//	CHullPointsCalculator class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

static LPCSTR FIELD_NAME_TABLE[CHullPointsCalculator::fieldCount] = 
	{
	"fullSlots",
	"partialSlots",
	"cargoSpace",
	"maxCargoSpace",
	"armorSegments",
	"stdArmorMass",
	"maxArmorMass",
	"maxSpeed",
	"thrustRatio",
	"drivePowerUse",
	"maneuverability",
	"specialDeviceSlots",
	"extra"
	};

CHullPointsCalculator::CHullPointsCalculator (const CShipClass &Class)

//	CHullPointsCalculator constructor

	{
	//	We need to have max reactor power defined, or else we can't compute the
	//	values.

	const CHullDesc &Hull = Class.GetHullDesc();
	m_iMaxReactorPower = Hull.GetMaxReactorPower();
	if (m_iMaxReactorPower == 0)
		return;

	//	Start by initializing points for device slots

	int iFullSlots = Min(Hull.GetMaxWeapons(), Hull.GetMaxNonWeapons());
	int iPartialSlots = Hull.GetMaxDevices() - iFullSlots;

	m_Data[fieldFullSlots] = iFullSlots;
	m_Data[fieldPartialSlots] = (iPartialSlots * PARTIAL_SLOT_FACTOR);

	//	Points for cargo space

	m_Data[fieldCargoSpace] = CARGO_SCALE_K1 * (pow((Metric)Min(MAX_CARGO_SPACE, Hull.GetCargoSpace()), CARGO_SCALE_K2) + CARGO_SCALE_K0);
	m_Data[fieldMaxCargoSpace] = CARGO_SCALE_K1_MAX * (pow((Metric)Min(MAX_CARGO_SPACE, Hull.GetMaxCargoSpace()), CARGO_SCALE_K2) + CARGO_SCALE_K0_MAX);

	//	Points for the number of armor segments and for max armor

	const CShipArmorDesc &Armor = Class.GetArmorDesc();
	m_Data[fieldArmorCount] = (Armor.GetCount() - STD_ARMOR_SEGMENTS) * POINTS_PER_ARMOR_SEGMENT;

	//	Compute points for armor limits

	CArmorLimits::SSummary ArmorLimits;
	Hull.CalcArmorLimitsSummary(g_pUniverse->GetDesignCollection().GetArmorMassDefinitions(), ArmorLimits);

	//	These factors control how important it is to have lots of armor choices.
	//	If we get a speed bonus below std armor mass, then we get a bonus. 
	//	Similarly, if we have a penalty for armor above standard, then we get
	//	less of a benefit.

	Metric rStdArmorFactor = STD_ARMOR_FREQUENCY_FACTOR * (1.0 + (ArmorLimits.iMaxSpeedBonus * ARMOR_SPEED_BONUS_FACTOR));
	Metric rMaxArmorFactor = MAX_ARMOR_FREQUENCY_FACTOR * (1.0 + (ArmorLimits.iMaxSpeedPenalty * ARMOR_SPEED_PENALTY_FACTOR));

	//	Frequency is the fraction of armor types that are available to the ship
	//	class. We focus on levels from 0.5 to 1.0 and scale it to 0 to 1.0).

	Metric rAdjStdFrequency = (1.0 / (1.0 - MIN_ARMOR_FREQUENCY)) * Max(0.0, ArmorLimits.rStdArmorFrequency - MIN_ARMOR_FREQUENCY);
	Metric rAdjMaxFrequency = (1.0 / (1.0 - MIN_ARMOR_FREQUENCY)) * Max(0.0, ArmorLimits.rMaxArmorFrequency - MIN_ARMOR_FREQUENCY);

	m_Data[fieldStdArmorMass] = rAdjStdFrequency * rStdArmorFactor;
	m_Data[fieldMaxArmorMass] = (rAdjMaxFrequency - rAdjStdFrequency) * rMaxArmorFactor;

	//	Points for max speed

	const CDriveDesc &Drive = Class.GetHullDriveDesc();
	m_Data[fieldMaxSpeed] = Max(0.0, ((100.0 * Drive.GetMaxSpeed() / LIGHT_SPEED) - MIN_SPEED)) / SPEED_PER_POINT;

	//	Points for thrust ratio

	Metric rThrustRatio = Class.GetThrustRatio();
	if (rThrustRatio <= 0.0)
		rThrustRatio = CDriveDesc::CalcThrustRatio(Drive.GetThrust(), Hull.GetMass());

	m_Data[fieldThrustRatio] = rThrustRatio / THRUST_RATIO_PER_POINT;

	//	Points for drive power consumption

	m_Data[fieldDrivePowerUse] = (STD_DRIVE_POWER_USE - Drive.GetPowerUse()) * POINTS_PER_DRIVE_POWER_USE;

	//	Points for maneuverability

	const CRotationDesc &Rotation = Class.GetHullRotationDesc();
	m_Data[fieldManeuverability] = Rotation.GetMaxRotationPerTick() / MAX_ROTATION_PER_POINT;

	//	Loop over all slots and see if we have special slots

	IDeviceGenerator *pDeviceSlots = Class.GetDeviceSlots();
	if (pDeviceSlots)
		m_Data[fieldDeviceSlots] = pDeviceSlots->CalcHullPoints();

	//	Add any extra points added manually.

	m_Data[fieldExtra] = Hull.GetExtraPoints();

	//	Compute total points

	m_rTotalPoints = 0.0;
	for (int i = 0; i < fieldCount; i++)
		m_rTotalPoints += m_Data[i];
	}

CString CHullPointsCalculator::GetFieldName (int iIndex)

//	GetFieldName
//
//	Returns the field name.

	{
	if (iIndex < 0 || iIndex >= fieldCount)
		return NULL_STR;

	return CString(FIELD_NAME_TABLE[iIndex]);
	}

CurrencyValue CHullPointsCalculator::GetValueInCredits (void) const

//	GetValueInCredits
//
//	Returns the credit value based on hull points.

	{
	//	Compute a price unit based on the maximum reactor power

	Metric rUnitPrice = PRICE_PER_TENTH_MW * m_iMaxReactorPower;

	//	Scale points

	Metric rScaledPoints = pow(GetTotalPoints() + POINT_BIAS, POINT_EXP);

	//	Compute price

	Metric rPrice = rScaledPoints * rUnitPrice;

	//	Done

	return (CurrencyValue)round(rPrice);
	}
