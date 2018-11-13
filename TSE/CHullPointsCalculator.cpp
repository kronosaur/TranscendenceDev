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
	//	We need to have max reactor powe defined, or else we can't compute the
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

	m_Data[fieldCargoSpace] = Hull.GetCargoSpace() / CARGO_PER_POINT;
	m_Data[fieldMaxCargoSpace] = (Hull.GetMaxCargoSpace() - Hull.GetCargoSpace()) / MAX_CARGO_PER_POINT;

	//	Points for the number of armor segments and for max armor

	const CShipArmorDesc &Armor = Class.GetArmorDesc();
	m_Data[fieldArmorCount] = (Armor.GetCount() - STD_ARMOR_SEGMENTS) * POINTS_PER_ARMOR_SEGMENT;
	m_Data[fieldStdArmorMass] = Hull.GetStdArmorMass() / ARMOR_PER_POINT;
	m_Data[fieldMaxArmorMass] = Min(MAX_ARMOR_MASS, Hull.GetMaxArmorMass()) / MAX_ARMOR_PER_POINT;

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
