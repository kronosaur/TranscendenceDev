//	CHullPointsCalculator.cpp
//
//	CHullPointsCalculator class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

static LPCSTR FIELD_NAME_TABLE[CHullPointsCalculator::fieldCount] = 
	{
	"deviceSlots",
	"armor",
	"drive",
	"cargo",
	"misc",

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

static constexpr Metric PARTIAL_SLOT_FACTOR = 0.75;
static constexpr Metric CARGO_SCALE_K0 = -7.0;
static constexpr Metric CARGO_SCALE_K0_MAX = -10.0;
static constexpr Metric CARGO_SCALE_K1 = 0.15;
static constexpr Metric CARGO_SCALE_K1_MAX = 0.15;
static constexpr Metric CARGO_SCALE_K2 = 0.5;
static constexpr int MAX_CARGO_SPACE = 1000;
static constexpr int STD_ARMOR_SEGMENTS = 4;
static constexpr Metric POINTS_PER_ARMOR_SEGMENT = 0.125;
static constexpr Metric STD_ARMOR_FREQUENCY_FACTOR = 1.0;
static constexpr Metric MAX_ARMOR_FREQUENCY_FACTOR = 0.5;
static constexpr Metric ARMOR_SPEED_BONUS_FACTOR = 0.2;
static constexpr Metric ARMOR_SPEED_PENALTY_FACTOR = 0.1;
static constexpr Metric MIN_ARMOR_FREQUENCY = 0.5;
static constexpr Metric MIN_SPEED = 15.0;
static constexpr Metric SPEED_PER_POINT = 8.0;
static constexpr Metric THRUST_RATIO_PER_POINT = 25.0;
static constexpr Metric MAX_ROTATION_PER_POINT = 18.0;
static constexpr Metric POINTS_PER_DRIVE_POWER_USE = 0.25;

static constexpr Metric POINT_BIAS = -5.0;
static constexpr Metric POINT_EXP = 3.0;

CHullPointsCalculator::CHullPointsCalculator (const CShipClass &Class, const CShipStandard &Standard)

//	CHullPointsCalculator constructor

	{
	m_rPricePerPoint = Standard.GetValue(CShipStandard::fieldPricePerHullPoint);
	if (m_rPricePerPoint <= 0.0)
		return;

	m_rMinPrice = Standard.GetValue(CShipStandard::fieldMinHullPrice);

	//	Start by initializing points for device slots

	const CHullDesc &Hull = Class.GetHullDesc();
	int iFullSlots = Min(Hull.GetMaxWeapons(), Hull.GetMaxNonWeapons());
	int iPartialSlots = Hull.GetMaxDevices() - iFullSlots;

	m_Data[fieldFullSlots] = iFullSlots;
	m_Data[fieldPartialSlots] = (iPartialSlots * PARTIAL_SLOT_FACTOR);

	//	Points for cargo space

	m_Data[fieldCargoSpace] = Max(0.0, CARGO_SCALE_K1 * (pow((Metric)Min(MAX_CARGO_SPACE, Hull.GetCargoSpace()), CARGO_SCALE_K2) + CARGO_SCALE_K0));
	m_Data[fieldMaxCargoSpace] = Max(0.0, CARGO_SCALE_K1_MAX * (pow((Metric)Min(MAX_CARGO_SPACE, Hull.GetMaxCargoSpace()), CARGO_SCALE_K2) + CARGO_SCALE_K0_MAX));

	//	For ships that have a large built-in cargo hold, we ignore the max 
	//	cargo space rating.

	if (m_Data[fieldCargoSpace] > m_Data[fieldMaxCargoSpace])
		m_Data[fieldMaxCargoSpace] = 0.0;

	//	Points for the number of armor segments and for max armor

	const CShipArmorDesc &Armor = Class.GetArmorDesc();
	m_Data[fieldArmorCount] = (Armor.GetCount() - STD_ARMOR_SEGMENTS) * POINTS_PER_ARMOR_SEGMENT;

	//	Compute points for armor limits

	const CArmorLimits &ArmorLimits = Hull.GetArmorLimits();
	CArmorLimits::SSummary ArmorSummary;
	ArmorLimits.CalcSummary(Class.GetUniverse().GetDesignCollection().GetArmorMassDefinitions(), ArmorSummary);

	//	These factors control how important it is to have lots of armor choices.
	//	If we get a speed bonus below std armor mass, then we get a bonus. 
	//	Similarly, if we have a penalty for armor above standard, then we get
	//	less of a benefit.

	Metric rStdArmorFactor = STD_ARMOR_FREQUENCY_FACTOR * (1.0 + (ArmorSummary.iMaxSpeedBonus * ARMOR_SPEED_BONUS_FACTOR));
	Metric rMaxArmorFactor = MAX_ARMOR_FREQUENCY_FACTOR * (1.0 + (ArmorSummary.iMaxSpeedPenalty * ARMOR_SPEED_PENALTY_FACTOR));

	//	Frequency is the fraction of armor types that are available to the ship
	//	class. We focus on levels from 0.5 to 1.0 and scale it to 0 to 1.0).

	Metric rAdjStdFrequency = (1.0 / (1.0 - MIN_ARMOR_FREQUENCY)) * Max(0.0, ArmorSummary.rStdArmorFrequency - MIN_ARMOR_FREQUENCY);
	Metric rAdjMaxFrequency = (1.0 / (1.0 - MIN_ARMOR_FREQUENCY)) * Max(0.0, ArmorSummary.rMaxArmorFrequency - MIN_ARMOR_FREQUENCY);

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

	Metric rStdDrivePowerUse = Standard.GetValue(CShipStandard::fieldDrivePowerUse);
	if (rStdDrivePowerUse > 0.0)
		m_Data[fieldDrivePowerUse] = ((rStdDrivePowerUse - Drive.GetPowerUse()) / rStdDrivePowerUse) * POINTS_PER_DRIVE_POWER_USE;
	else
		m_Data[fieldDrivePowerUse] = 0.0;

	//	Points for maneuverability

	const CRotationDesc &Rotation = Class.GetHullRotationDesc();
	m_Data[fieldManeuverability] = Rotation.GetMaxRotationPerTick() / MAX_ROTATION_PER_POINT;

	//	Loop over all slots and see if we have special slots

	IDeviceGenerator *pDeviceSlots = Class.GetDeviceSlots();
	if (pDeviceSlots)
		m_Data[fieldDeviceSlots] = pDeviceSlots->CalcHullPoints();

	//	Add any extra points added manually.

	m_Data[fieldExtra] = Hull.GetExtraPoints();

	//	Add up sums

	m_Data[fieldSlotSum] = m_Data[fieldFullSlots] + m_Data[fieldPartialSlots];
	m_Data[fieldCargoSum] = m_Data[fieldCargoSpace] + m_Data[fieldMaxCargoSpace];
	m_Data[fieldArmorSum] = m_Data[fieldArmorCount] + m_Data[fieldStdArmorMass] + m_Data[fieldMaxArmorMass];
	m_Data[fieldDriveSum] = m_Data[fieldMaxSpeed] + m_Data[fieldManeuverability] + m_Data[fieldThrustRatio];
	m_Data[fieldOtherSum] = m_Data[fieldDrivePowerUse] + m_Data[fieldExtra] + m_Data[fieldDeviceSlots];

	//	Compute total points

	m_rTotalPoints = 0.0;
	for (int i = fieldFullSlots; i < fieldCount; i++)
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
	//	Scale points

	Metric rScaledPoints = Max(0.0, pow(GetTotalPoints() + POINT_BIAS, POINT_EXP));

	//	Compute price

	Metric rPrice = m_rMinPrice + (rScaledPoints * m_rPricePerPoint);

	//	Done

	return (CurrencyValue)round(rPrice);
	}
