//	CWeaponClass.cpp
//
//	CWeaponClass class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "WeaponClassImpl.h"

#define DAMAGE_FAILURE_TAG						CONSTLIT("DamageFailure")
#define MISSILES_TAG							CONSTLIT("Missiles")
#define OVERHEAT_FAILURE_TAG					CONSTLIT("OverheatFailure")
#define VARIANTS_TAG							CONSTLIT("Variants")

#define AMMO_ID_ATTRIB							CONSTLIT("ammoID")
#define ANGLE_ATTRIB							CONSTLIT("angle")
#define BURST_TRACKS_TARGETS_ATTRIB				CONSTLIT("burstTracksTargets")
#define CHARGES_ATTRIB							CONSTLIT("charges")
#define CONTINUOUS_CONSUME_PERSHOT_ATTRIB		CONSTLIT("consumeAmmoPerRepeatingShot")
#define COOLING_RATE_ATTRIB						CONSTLIT("coolingRate")
#define COUNTER_ATTRIB							CONSTLIT("counter")
#define COUNTER_ACTIVATE_ATTRIB					CONSTLIT("counterActivate")
#define COUNTER_UPDATE_ATTRIB					CONSTLIT("counterUpdate")
#define COUNTER_UPDATE_RATE_ATTRIB				CONSTLIT("counterUpdateRate")
#define FAILURE_CHANCE_ATTRIB					CONSTLIT("failureChance")
#define FIRE_RATE_ATTRIB						CONSTLIT("fireRate")
#define HEATING_ATTRIB							CONSTLIT("heating")
#define IDLE_POWER_USE_ATTRIB					CONSTLIT("idlePowerUse")
#define LAUNCHER_ATTRIB							CONSTLIT("launcher")
#define LINKED_FIRE_ATTRIB						CONSTLIT("linkedFire")
#define MAX_FIRE_ARC_ATTRIB						CONSTLIT("maxFireArc")
#define MIN_FIRE_ARC_ATTRIB						CONSTLIT("minFireArc")
#define MULTI_TARGET_ATTRIB						CONSTLIT("multiTarget")
#define CAN_FIRE_WHEN_BLIND_ATTRIB				CONSTLIT("canFireWhenBlind")
#define OMNIDIRECTIONAL_ATTRIB					CONSTLIT("omnidirectional")
#define POS_ANGLE_ATTRIB						CONSTLIT("posAngle")
#define POS_RADIUS_ATTRIB						CONSTLIT("posRadius")
#define POWER_USE_ATTRIB						CONSTLIT("powerUse")
#define RECOIL_ATTRIB							CONSTLIT("recoil")
#define REPEATING_ATTRIB						CONSTLIT("repeating")
#define REPEATING_DELAY_ATTRIB					CONSTLIT("repeatingDelay")
#define REPORT_AMMO_ATTRIB						CONSTLIT("reportAmmo")
#define SHIP_COUNTER_PER_SHOT_ATTRIB			CONSTLIT("shipCounterPerShot")
#define TARGET_STATIONS_ONLY_ATTRIB				CONSTLIT("targetStationsOnly")
#define TYPE_ATTRIB								CONSTLIT("type")
#define USES_LAUNCHER_CONTROLS_ATTRIB			CONSTLIT("usesLauncherControls")

#define COUNTER_TYPE_TEMPERATURE				CONSTLIT("temperature")
#define COUNTER_TYPE_CAPACITOR					CONSTLIT("capacitor")

#define ON_FIRE_WEAPON_EVENT					CONSTLIT("OnFireWeapon")
#define GET_AMMO_TO_CONSUME_EVENT				CONSTLIT("GetAmmoToConsume")
#define GET_AMMO_COUNT_TO_DISPLAY_EVENT			CONSTLIT("GetAmmoCountToDisplay")

#define FIELD_AMMO_TYPE							CONSTLIT("ammoType")
#define FIELD_AVERAGE_DAMAGE					CONSTLIT("averageDamage")	//	Average damage (1000x hp)
#define FIELD_BALANCE							CONSTLIT("balance")
#define FIELD_BALANCE_DAMAGE					CONSTLIT("balanceDamage")	//	Computed damage (per 180 ticks) based on balance 
#define FIELD_CONFIGURATION						CONSTLIT("configuration")
#define FIELD_DAMAGE_180						CONSTLIT("damage")			//	HP damage per 180 ticks
#define FIELD_DAMAGE_TYPE						CONSTLIT("damageType")
#define FIELD_HP								CONSTLIT("hp")
#define FIELD_IS_ALTERNATING					CONSTLIT("isAlternating")
#define FIELD_MAX_DAMAGE						CONSTLIT("maxDamage")
#define FIELD_MIN_DAMAGE						CONSTLIT("minDamage")
#define FIELD_FIRE_DELAY						CONSTLIT("fireDelay")		//	Delay (ticks)
#define FIELD_FIRE_RATE							CONSTLIT("fireRate")
#define FIELD_POWER								CONSTLIT("power")
#define FIELD_POWER_PER_SHOT					CONSTLIT("powerPerShot")	//	Power used per shot (1000x Megawatt minutes)
#define FIELD_RANGE								CONSTLIT("range")
#define FIELD_RECOIL							CONSTLIT("recoil")
#define FIELD_REPEAT_COUNT						CONSTLIT("repeatCount")
#define FIELD_SPEED								CONSTLIT("speed")
#define FIELD_VARIANT_COUNT						CONSTLIT("variantCount")

#define PROPERTY_AMMO_TYPES						CONSTLIT("ammoTypes")
#define PROPERTY_AVERAGE_DAMAGE					CONSTLIT("averageDamage")	//	Average damage
#define PROPERTY_BALANCE    					CONSTLIT("balance")
#define PROPERTY_BALANCE_DAMAGE					CONSTLIT("balanceDamage")
#define PROPERTY_BALANCE_COST 					CONSTLIT("balanceCost")
#define PROPERTY_BALANCE_EXCLUDE_COST			CONSTLIT("balanceExcludeCost")
#define PROPERTY_CAN_FIRE_WHEN_BLIND			CONSTLIT("canFireWhenBlind")
#define PROPERTY_DAMAGE_180						CONSTLIT("damage")			//	HP damage per 180 ticks
#define PROPERTY_DAMAGE_PER_PROJECTILE			CONSTLIT("damagePerProjectile")
#define PROPERTY_DAMAGE_TYPE_ID					CONSTLIT("damageTypeID")
#define PROPERTY_DAMAGE_WMD_180					CONSTLIT("damageWMD180")
#define PROPERTY_DAMAGED						CONSTLIT("damaged")
#define PROPERTY_EFFECTIVE_RANGE				CONSTLIT("effectiveRange")
#define PROPERTY_FIRE_ARC						CONSTLIT("fireArc")
#define PROPERTY_FIRE_DELAY						CONSTLIT("fireDelay")
#define PROPERTY_FIRE_RATE						CONSTLIT("fireRate")
#define PROPERTY_LINKED_FIRE_OPTIONS			CONSTLIT("linkedFireOptions")
#define PROPERTY_MAX_DAMAGE						CONSTLIT("maxDamage")
#define PROPERTY_MIN_DAMAGE						CONSTLIT("minDamage")
#define PROPERTY_MULTI_SHOT						CONSTLIT("multiShot")
#define PROPERTY_OMNIDIRECTIONAL				CONSTLIT("omnidirectional")
#define PROPERTY_REPEATING						CONSTLIT("repeating")
#define PROPERTY_SECONDARY						CONSTLIT("secondary")
#define PROPERTY_SHIP_COUNTER_PER_SHOT			CONSTLIT("shipCounterPerShot")
#define PROPERTY_SINGLE_POINT_ORIGIN			CONSTLIT("singlePointOrigin")
#define PROPERTY_STD_COST						CONSTLIT("stdCost")
#define PROPERTY_STD_MASS						CONSTLIT("stdMass")
#define PROPERTY_TRACKING						CONSTLIT("tracking")

#define VARIANT_TYPE_COUNTER					CONSTLIT("counter")
#define VARIANT_TYPE_CHARGES					CONSTLIT("charges")
#define VARIANT_TYPE_LEVELS						CONSTLIT("levels")
#define VARIANT_TYPE_MISSILES					CONSTLIT("missiles")

const int MAX_SHOT_COUNT =				100;

const Metric g_DualShotSeparation =		12;					//	Radius of dual shot (pixels)
const int TEMP_DECREASE =				-1;					//	Decrease in temp per cooling rate
const int MAX_TEMP =					120;				//	Max temperature
const int OVERHEAT_TEMP =				100;				//	Weapon overheats
const Metric MAX_TARGET_RANGE =			(24.0 * LIGHT_SECOND);
const int MAX_COUNTER =					100;

const Metric STD_FIRE_RATE_SECS =		16.0;				//	Standard fire rate (secs)
const Metric STD_FIRE_DELAY_TICKS =     8.0;

const Metric STD_AMMO_BALANCE =         -100.0;             //  Balance adj from having ammo
const Metric STD_AMMO_MASS =            10.0;               //  Std ammo mass (kg)
const Metric BALANCE_AMMO_COST_RATIO =  -0.25;              //  Each percent of ammo price above std is
															//      0.25% balance penalty.
const Metric BALANCE_AMMO_MASS_RATIO =  -0.25;              //  Heavier ammo is a balance penalty.

const Metric BALANCE_OMNI_POWER =       0.5;                //  f(x) = factor * (x/360)^power
const Metric BALANCE_OMNI_FACTOR =      100.0;              //      This function maps from a swivel
															//      arc (0-360) to its effect on weapon
															//      balance: 0 = none; 100.0 = +100%

const Metric BALANCE_TRACKING_BONUS =   90.0;               //  Bonus to balance if weapon has tracking.
const Metric BALANCE_LINKED_FIRE_BONUS = 25.0;              //  Bonus to balance if weapon is linked-fire.

const Metric STD_RANGE =                60.0 * LIGHT_SECOND;//  Standard range
const Metric MAX_BALANCE_RANGE =        120.0 * LIGHT_SECOND;   //  Range above this doesn't count for balance purposes
const Metric BALANCE_RANGE_OVER_FACTOR = 0.35;              //  Bonus to balance for each ls above standard
const Metric BALANCE_RANGE_UNDER_FACTOR = 1.0;              //  Penalty to balance for each ls below standard

const Metric BALANCE_SPEED_POWER =      0.5;                //  f(x) = factor * (1 - (x/lightspeed)^power)
const Metric BALANCE_SPEED_FACTOR =     -25.0;

const Metric BALANCE_INTERACTION_FACTOR = 5.0;              //  Bonus to balance for 0 interaction (linear
															//      decrease as interaction reaches 100).
const Metric BALANCE_HP_FACTOR =        10.0;               //  Bonus to balance for projectile HP equal to
															//      standard weapon damage for the level
															//      (linear decrease with fewer HP).
const Metric BALANCE_POWER_AMMO_FACTOR = 0.1;               //  Power used by ammo weapons relative to non-ammo
const Metric BALANCE_POWER_RATIO =      -0.5;               //  Each percent of power consumption above standard
															//      is 0.5% balance penalty.
const Metric BALANCE_COST_RATIO =       -0.5;               //  Each percent of cost above standard is a 0.5%
															//      balance penalty.
const Metric BALANCE_SLOT_FACTOR =      -40.0;              //  Penalty to balance for every extra slot used 
															//      above 1.
const Metric BALANCE_NO_SLOT =          20.0;               //  Bonus to balance if no slot is used.
const Metric BALANCE_EXTERNAL =         -40.0;              //  Penalty to balance if weapon is external
const Metric BALANCE_RECOIL_FACTOR =    -2.5;               //  Penalty to balance per point of recoil
const Metric BALANCE_RADIATION =        30.0;               //  Bonus to balance if we have radiation damage
const Metric BALANCE_DEVICE_DISRUPT_FACTOR = 3.0;           //  Bonus to balance for per % chance of disruption
const Metric BALANCE_DEVICE_DAMAGE_FACTOR = 3.0;            //  Bonus to balance for per % chance of damage
const Metric BALANCE_DISINTEGRATION =   100.0;              //  Bonus to balance for disintegration
const Metric BALANCE_SHATTER_FACTOR =   12.0;               //  Bonus to balance for shatter
const Metric BALANCE_SHIELD_PENETRATE_FACTOR = 0.25;        //  Bonus to balance for each % of shield penetrate chance
const Metric BALANCE_SHIELD_LEVEL_FACTOR =      10;         //  Bonus to balance proportional to shield damage level
const Metric BALANCE_ARMOR_LEVEL_FACTOR =       20;         //  Bonus to balance proportional to armor damage level
const Metric BALANCE_MINING_FACTOR =            0.1;        //  Bonus to balance for each % of mining adj
const Metric BALANCE_WMD_FACTOR =               0.5;        //  Bonust to balance for each % of WMD
const Metric BALANCE_TIME_STOP_LEVEL_FACTOR =	50;         //  Bonus to balance proportional to time stop level

const Metric PARTICLE_CLOUD_DAMAGE_FACTOR =		0.75;
const Metric SHOCKWAVE_DAMAGE_FACTOR =			4.0;

static CWeaponClass::SStdStats STD_WEAPON_STATS[MAX_ITEM_LEVEL] =
	{
		//	Damage	   Power	          Cost     Ammo Cost   Over	Under
		{	   4,	     10,	           350,         0.6,     0,	    0, },
		{	   5,	     20,	           800,         1.2,     0,	    -20, },
		{	   7,	     50,	         1'850,         3.0,     0,     -50, },
		{	   9,	    100,	         4'000,         6.0,     0,     -100, },
		{	  12,	    200,	         8'500,        12.0,     5,     -170, },
			
		{	  16,	    300,	        18'500,        18.0,     5,     -250, },
		{	  21,	    500,	        39'000,        30.0,     5,     -300, },
		{	  27,	  1'000,	        78'500,        60.0,     10,    -300, },
		{	  35,	  2'000,	       158'000,       120.0,     10,    -300, },
		{	  46,	  3'000,	       320'000,       180.0,     10,    -300, },
			
		{	  60,	  5'000,	       640'000,       300.0,     25,    -300, },
		{	  78,	  8'000,	     1'300'000,       480.0,     25,    -300, },
		{	  101,	 12'000,	     2'700'000,       720.0,     25,    -300, },
		{	  131,	 18'000,	     5'300'000,     1'080.0,     100,   -300, },
		{	  170,	 28'000,	    10'600'000,     1'680.0,     100,   -300, },
			
		{	  221,	 40'000,	    21'300'000,     2'400.0,     100,   -300, },
		{	  287,	 60'000,	    42'600'000,     3'600.0,     200,   -300, },
		{	  373,	 90'000,	    85'200'000,     5'400.0,     200,   -300, },
		{	  485,	120'000,	   170'000'000,     7'200.0,     200,   -300, },
		{	  631,	160'000,	   341'000'000,     9'600.0,     200,   -300, },
			
		{	  820,	220'000,	   682'000'000,    13'200.0,     200,   -300, },
		{	1'066,	300'000,	 1'400'000'000,    18'000.0,     200,   -300, },
		{	1'386,	400'000,	 2'700'000'000,    24'000.0,     200,   -300, },
		{	1'802,	500'000,	 5'500'000'000,    30'000.0,     200,   -300, },
		{	2'343,	600'000,	10'900'000'000,    36'000.0,     200,   -300, },
	};

static const char *CACHED_EVENTS[CWeaponClass::evtCount] =
	{
		"OnFireWeapon",
		"GetAmmoToConsume",
		"GetAmmoCountToDisplay",
	};

CFailureDesc CWeaponClass::g_DefaultFailure(CFailureDesc::profileWeaponFailure);

CWeaponClass::CWeaponClass (void)
	{
	}

CWeaponClass::~CWeaponClass (void)
	{
	for (int i = 0; i < m_ShotData.GetCount(); i++)
		if (m_ShotData[i].bOwned)
			delete m_ShotData[i].pDesc;
	}

bool CWeaponClass::Activate (CInstalledDevice &Device, SActivateCtx &ActivateCtx)

//	Activate
//
//	Activates the device (in this case, fires the weapon)

	{
	DEBUG_TRY

	CSpaceObject &SourceObj = Device.GetSourceOrThrow();
	const CWeaponFireDesc *pShotDesc = GetWeaponFireDesc(Device);

	ActivateCtx.iRepeatingCount = 0;
	ActivateCtx.bConsumedItems = false;

	//	If not enabled, no firing

	if (pShotDesc == NULL || !Device.IsEnabled())
		{
		Device.SetLastActivateSuccessful(false);
		return false;
		}

	//  Set the target to NULL if we're blind and we can't fire when blind

	if (!m_bCanFireWhenBlind && SourceObj.IsBlind())
		ActivateCtx.pTarget = NULL;

	//	Fire the weapon

	bool bSuccess = FireWeapon(Device, *pShotDesc, ActivateCtx);

	//	If firing the weapon destroyed the ship, then we bail out

	if (SourceObj.IsDestroyed())
		return false;

	//	Keep track of whether we succeeded or not so that we know whether to consume power

	Device.SetLastActivateSuccessful(bSuccess);

	//	If we did not succeed, then we're done

	if (!bSuccess)
		return false;

	//	If this is a continuous fire weapon then set the device data
	//	We set to -1 because we skip the first Update after the call
	//	to Activate (since it happens on the same tick)

	if (GetContinuous(*pShotDesc) > 0)
		SetContinuousFire(&Device, CONTINUOUS_START);

	//	Player-specific code

	if (SourceObj.IsPlayer())
		{
		//	Track statistics for the player

		CShip *pShip = SourceObj.AsShip();
		if (pShip)
			{
			CItem WeaponItem(GetItemType(), 1);
			pShip->GetController()->OnItemFired(WeaponItem);

			if ((IsLauncher() || m_bReportAmmo) && pShotDesc->GetAmmoType())
				{
				CItem AmmoItem(pShotDesc->GetAmmoType(), 1);
				pShip->GetController()->OnItemFired(AmmoItem);
				}
			}

		//	Identify the weapon

		Device.GetItem()->SetKnown();
		}

	//	Consume power

	return true;

	DEBUG_CATCH
	}

int CWeaponClass::CalcActivateDelay (CItemCtx &ItemCtx) const

//	CalcActivateDelay
//
//	Computes the activation delay, optionally dealing with enhancements.

	{
	TSharedPtr<CItemEnhancementStack> pEnhancements = ItemCtx.GetEnhancementStack();
	if (pEnhancements)
		return pEnhancements->CalcActivateDelay(ItemCtx);

	return GetActivateDelay(ItemCtx);
	}

int CWeaponClass::CalcBalance (const CItem &Ammo, SBalance &retBalance) const

//	CalcBalance
//
//	Calculates weapon balance relative to level. +100 is 100% overpowered.

	{
	const CItem Item(GetItemType(), 1);
	const CDeviceItem DeviceItem = Item.AsDeviceItem();
	if (!DeviceItem)
		return 0;

	CItemCtx ItemCtx(Item);

	//  Initialize

	retBalance.rBalance = 0.0;

	const CWeaponFireDesc *pShotDesc = GetWeaponFireDesc(DeviceItem, Ammo);
	if (pShotDesc == NULL)
		return 0;

	//	Compute the level. For launchers we take either the missile level or the
	//	launcher level (whichever is higher).

	retBalance.iLevel = CalcLevel(*pShotDesc);

	//  Get the standard stats for this level

	const SStdStats &Stats = STD_WEAPON_STATS[retBalance.iLevel - 1];
	retBalance.rStdDamage180 = Stats.iDamage * (180.0 / STD_FIRE_DELAY_TICKS);

	//  Compute how much damage we do in 180 ticks.

	retBalance.rDamageHP = CalcDamage(*pShotDesc);
	retBalance.rDamageMult = CalcConfigurationMultiplier(pShotDesc, false);
	Metric rDamagePerShot = retBalance.rDamageMult * retBalance.rDamageHP;
	Metric rFireDelay = (Metric)Max(GetFireDelay(*pShotDesc), 1);
	retBalance.rDamage180 = rDamagePerShot * 180.0 / rFireDelay;

	//  Compute the number of balance points (BP) of the damage. +100 = double
	//  damage relative to standard. -100 = half-damage relative to standard.

	retBalance.rDamage = 100.0 * mathLog2(retBalance.rDamage180 / retBalance.rStdDamage180);
	retBalance.rBalance = retBalance.rDamage;

	//  If our damage type is higher or lower than appropriate for this level,
	//  adjust our balance. Positive nunmbers mean that the weapon has a lower 
	//  level than the minimum for its damage type.

	DamageTypes iDamageType = pShotDesc->GetDamageType();
	int iTypeDelta = DamageDesc::GetDamageLevel(iDamageType) - retBalance.iLevel;
	retBalance.rDamageType = (iTypeDelta >= 0 ? STD_WEAPON_STATS[iTypeDelta].iOverTierAdj : STD_WEAPON_STATS[-iTypeDelta].iUnderTierAdj);
	retBalance.rBalance += retBalance.rDamageType;

	//  Ammo weapons have the inherent disadvantage of needing ammo

	bool bAmmo = !Ammo.IsEmpty();
	bool bUsesAmmo;
	if (CItemType *pAmmoType = pShotDesc->GetAmmoType())
		{
		CItemCtx AmmoItemCtx(Ammo);
		bUsesAmmo = true;

		//  Having ammo is a 100% penalty

		retBalance.rAmmo = STD_AMMO_BALANCE;

		//  Adjust the standard ammo cost and standard ammo mass for fire rate.

		retBalance.rStdAmmoCost = Stats.rAmmoCost * rFireDelay / STD_FIRE_DELAY_TICKS;
		retBalance.rStdAmmoMass = STD_AMMO_MASS * rFireDelay / STD_FIRE_DELAY_TICKS;

		//  Compute the standard ammo cost at this level and figure out the 
		//  percent cost difference. +1 = ammo is 1% more expensive than 
		//  standard. -1 = ammo is 1% cheaper than standard.

		Metric rAmmoCost = (Metric)CEconomyType::ExchangeToCredits(pAmmoType->GetCurrencyAndValue(AmmoItemCtx, true));
		if (pAmmoType->AreChargesAmmo() && pAmmoType->GetMaxCharges() > 0)
			rAmmoCost /= (Metric)pAmmoType->GetMaxCharges();

		Metric rAmmoCostDelta = 100.0 * (rAmmoCost - retBalance.rStdAmmoCost) / retBalance.rStdAmmoCost;
		retBalance.rAmmo += rAmmoCostDelta * BALANCE_AMMO_COST_RATIO;

		//  Compute the ammo mass bonus

		Metric rAmmoMass = pAmmoType->GetMassKg(AmmoItemCtx);
		if (pAmmoType->AreChargesAmmo() && pAmmoType->GetMaxCharges() > 0)
			rAmmoMass /= (Metric)pAmmoType->GetMaxCharges();

		Metric rAmmoMassDelta = 100.0 * (rAmmoMass - retBalance.rStdAmmoMass) / retBalance.rStdAmmoMass;
		retBalance.rAmmo += rAmmoMassDelta * BALANCE_AMMO_MASS_RATIO;

		//  Add up to total balance

		retBalance.rBalance += retBalance.rAmmo;
		}

	//  Weapons with charges count as needing ammo

	else if (m_bCharges || IsLauncher())
		{
		bUsesAmmo = true;
		retBalance.rAmmo = STD_AMMO_BALANCE;
		retBalance.rBalance += retBalance.rAmmo;
		}
	else
		bUsesAmmo = false;

	//  Tracking weapons are a bonus

	if (IsTracking(DeviceItem, pShotDesc))
		{
		retBalance.rTracking = BALANCE_TRACKING_BONUS;
		retBalance.rBalance += retBalance.rTracking;
		}

	//  Omni and swivel weapons are a bonus

	Metric rSwivelRange = DeviceItem.GetFireArc() / 360.0;
	if (rSwivelRange > 0.0)
		{
		Metric rOmni = BALANCE_OMNI_FACTOR * pow(rSwivelRange, BALANCE_OMNI_POWER);

		//	NOTE: If we have tracking, then omni is less important.

		retBalance.rOmni = Max(0.0, rOmni - retBalance.rTracking);
		retBalance.rBalance += retBalance.rOmni;
		}

	//  Range

	Metric rRange = Min(MAX_BALANCE_RANGE, pShotDesc->GetMaxRange());
	if (rRange >= STD_RANGE)
		retBalance.rRange = BALANCE_RANGE_OVER_FACTOR * (rRange - STD_RANGE) / LIGHT_SECOND;
	else
		retBalance.rRange = BALANCE_RANGE_UNDER_FACTOR * (rRange - STD_RANGE) / LIGHT_SECOND;

	retBalance.rBalance += retBalance.rRange;

	//  Speed

	retBalance.rSpeed = BALANCE_SPEED_FACTOR * (1.0 - pow(pShotDesc->GetRatedSpeed() / LIGHT_SPEED, BALANCE_SPEED_POWER));
	retBalance.rBalance += retBalance.rSpeed;

	//  Projectile HP and interaction

	retBalance.rProjectileHP = 0.0;
	if (pShotDesc->GetInteraction() < 100)
		retBalance.rProjectileHP += BALANCE_INTERACTION_FACTOR * (100.0 - pShotDesc->GetInteraction()) / 100.0;

	if (pShotDesc->GetHitPoints() > 0)
		retBalance.rProjectileHP += BALANCE_HP_FACTOR * pShotDesc->GetHitPoints() / (Metric)Stats.iDamage;

	retBalance.rBalance += retBalance.rProjectileHP;

	//  Power use (ignore if we're balancing a launcher's missile).

	if (!bAmmo || !IsLauncher())
		{
		//  Note: We divide by Stats.iPower (not rStdPower) because we care 
		//  about power use relative to what we generate. That is, launcher
		//  power use is so small that we don't care much if it doubles.

		Metric rStdPower = (bUsesAmmo ? BALANCE_POWER_AMMO_FACTOR * Stats.iPower : Stats.iPower);
		Metric rPowerDelta = 100.0 * (GetPowerRating(ItemCtx) - rStdPower) / (Metric)Stats.iPower;

		retBalance.rPower = BALANCE_POWER_RATIO * rPowerDelta;
		retBalance.rBalance += retBalance.rPower;
		}

	//  Cost

	if (!bAmmo || !IsLauncher())
		{
		Metric rCost = (Metric)CEconomyType::ExchangeToCredits(GetItemType()->GetCurrencyAndValue(ItemCtx, true));
		Metric rCostDelta = 100.0 * (rCost - (Metric)Stats.Cost) / (Metric)Stats.Cost;
		retBalance.rCost = BALANCE_COST_RATIO * rCostDelta;
		retBalance.rBalance += retBalance.rCost;
		}

	//  Slots

	if (GetSlotsRequired() == 0)
		retBalance.rSlots = BALANCE_NO_SLOT;
	else if (GetSlotsRequired() > 1)
		retBalance.rSlots = BALANCE_SLOT_FACTOR * (GetSlotsRequired() - 1.0);
	else
		retBalance.rSlots = 0.0;

	retBalance.rBalance += retBalance.rSlots;

	//  Linked-fire

	if (m_dwLinkedFireOptions)
		{
		retBalance.rLinkedFire = BALANCE_LINKED_FIRE_BONUS;
		retBalance.rBalance += retBalance.rLinkedFire;
		}

	//  Recoil

	if (m_iRecoil)
		{
		retBalance.rRecoil = m_iRecoil * BALANCE_RECOIL_FACTOR;
		retBalance.rBalance += retBalance.rRecoil;
		}

	//  External

	if (IsExternal())
		{
		retBalance.rExternal = BALANCE_EXTERNAL;
		retBalance.rBalance += retBalance.rExternal;
		}

	//  Radiation

	int iDamage;
	if (iDamage = pShotDesc->GetSpecialDamage(specialRadiation))
		{
		retBalance.rRadiation = BALANCE_RADIATION;
		retBalance.rBalance += retBalance.rRadiation;
		}

	//  Device disrupt and damage

	if (iDamage = pShotDesc->GetSpecialDamage(specialDeviceDisrupt, DamageDesc::flagSpecialAdj))
		{
		retBalance.rDeviceDisrupt = BALANCE_DEVICE_DISRUPT_FACTOR * Min(iDamage, 50);
		retBalance.rBalance += retBalance.rDeviceDisrupt;
		}

	if (iDamage = pShotDesc->GetSpecialDamage(specialDeviceDamage, DamageDesc::flagSpecialAdj))
		{
		retBalance.rDeviceDamage = BALANCE_DEVICE_DAMAGE_FACTOR * Min(iDamage, 50);
		retBalance.rBalance += retBalance.rDeviceDamage;
		}

	//  Disintegration

	if (iDamage = pShotDesc->GetSpecialDamage(specialDisintegration))
		{
		retBalance.rDisintegration = BALANCE_DISINTEGRATION;
		retBalance.rBalance += retBalance.rDisintegration;
		}

	//  Shatter

	if (iDamage = pShotDesc->GetSpecialDamage(specialShatter))
		{
		retBalance.rShatter = BALANCE_SHATTER_FACTOR * iDamage;
		retBalance.rBalance += retBalance.rShatter;
		}

	//  Shield penetrate

	if (iDamage = pShotDesc->GetSpecialDamage(specialShieldPenetrator, DamageDesc::flagSpecialAdj))
		{
		retBalance.rShieldPenetrate = BALANCE_SHIELD_PENETRATE_FACTOR * iDamage;
		retBalance.rBalance += retBalance.rShieldPenetrate;
		}

	//  Extra shield damage

	if (iDamage = pShotDesc->GetSpecialDamage(specialShieldDisrupt))
		{
		int iEffectLevel = Max(0, 3 + iDamage - retBalance.iLevel);
		retBalance.rShield = BALANCE_SHIELD_LEVEL_FACTOR * iEffectLevel;
		retBalance.rBalance += retBalance.rShield;
		}

	//  Extra armor damage

	if (iDamage = pShotDesc->GetSpecialDamage(specialArmor))
		{
		int iEffectLevel = Max(0, 3 + iDamage - retBalance.iLevel);
		retBalance.rArmor = BALANCE_ARMOR_LEVEL_FACTOR * iEffectLevel;
		retBalance.rBalance += retBalance.rArmor;
		}

	//	Time stop

	if (iDamage = pShotDesc->GetSpecialDamage(specialTimeStop))
		{
		int iEffectLevel = Max(0, 3 + iDamage - retBalance.iLevel);
		retBalance.rTimeStop = BALANCE_TIME_STOP_LEVEL_FACTOR * iEffectLevel;
		retBalance.rBalance += retBalance.rTimeStop;
		}

	//  Mining

	if (iDamage = pShotDesc->GetSpecialDamage(specialMining, DamageDesc::flagSpecialAdj))
		{
		retBalance.rMining = BALANCE_MINING_FACTOR * iDamage;
		retBalance.rBalance += retBalance.rMining;
		}

	//  WMD
	//
	//  All weapons have some degree of WMD, but we only count the ones that 
	//  have non-default WMD.

	if (pShotDesc->GetSpecialDamage(specialWMD, DamageDesc::flagSpecialLevel))
		{
		retBalance.rWMD = BALANCE_WMD_FACTOR * pShotDesc->GetSpecialDamage(specialWMD, DamageDesc::flagSpecialAdj);
		retBalance.rBalance += retBalance.rWMD;
		}

	//  Done

	return mathRound(retBalance.rBalance);
	}

CSpaceObject *CWeaponClass::CalcBestTarget (CInstalledDevice &Device, const CTargetList &TargetList, Metric *retrDist2, int *retiFireAngle) const

//	CalcBestTarget
//
//	Returns the nearest appropriate target for the given weapon (or NULL if none
//	is found).

	{
	CSpaceObject &SourceObj = Device.GetSourceOrThrow();
	CDeviceItem DeviceItem = Device.GetDeviceItem();

	bool bCheckLineOfFire = !TargetList.NoLineOfFireCheck();
	bool bCheckRange = !TargetList.NoRangeCheck() || (Device.GetMaxFireRangeLS() != 0);
	DWORD dwTargetTypes = DeviceItem.GetTargetTypes();

	Metric rMaxRange = DeviceItem.GetMaxEffectiveRange();
	Metric rMaxRange2 = rMaxRange * rMaxRange;
	TargetList.Realize();

	for (int i = 0; i < TargetList.GetCount(); i++)
		{
		int iFireAngle;
		CSpaceObject *pTarget = TargetList.GetTarget(i);
		Metric rDist2 = TargetList.GetTargetDist2(i);

		if ((!bCheckRange || rDist2 < rMaxRange2)
				&& (TargetList.GetTargetType(i) & dwTargetTypes)
				&& DeviceItem.GetWeaponEffectiveness(pTarget) >= 0
				&& IsTargetReachable(Device, *pTarget, -1, &iFireAngle)
				&& (!bCheckLineOfFire || SourceObj.IsLineOfFireClear(&Device, pTarget, iFireAngle, rMaxRange)))
			{
			if (retiFireAngle) *retiFireAngle = iFireAngle;
			if (retrDist2) *retrDist2 = rDist2;

			return pTarget;
			}
		}

	return NULL;
	}

TArray<CTargetList::STargetResult> CWeaponClass::CalcMIRVTargets (CInstalledDevice &Device, const CTargetList &TargetList, int iMaxCount) const

//	CalcMIRVTargets
//
//	Returns a list of targets for the given weapon.

	{
	TArray<CTargetList::STargetResult> Targets;
	Targets.GrowToFit(iMaxCount);

	CSpaceObject &SourceObj = Device.GetSourceOrThrow();
	CDeviceItem DeviceItem = Device.GetDeviceItem();

	bool bCheckLineOfFire = !TargetList.NoLineOfFireCheck();
	bool bCheckRange = !TargetList.NoRangeCheck();
	DWORD dwTargetTypes = DeviceItem.GetTargetTypes();

	Metric rMaxRange = DeviceItem.GetMaxEffectiveRange();
	Metric rMaxRange2 = rMaxRange * rMaxRange;
	TargetList.Realize();

	for (int i = 0; i < TargetList.GetCount(); i++)
		{
		int iFireAngle;
		CSpaceObject *pTarget = TargetList.GetTarget(i);
		Metric rDist2 = TargetList.GetTargetDist2(i);

		if ((!bCheckRange || rDist2 < rMaxRange2)
				&& (TargetList.GetTargetType(i) & dwTargetTypes)
				&& DeviceItem.GetWeaponEffectiveness(pTarget) >= 0
				&& IsTargetReachable(Device, *pTarget, -1, &iFireAngle)
				&& (!bCheckLineOfFire || SourceObj.IsLineOfFireClear(&Device, pTarget, iFireAngle, rMaxRange)))
			{
			auto *pEntry = Targets.Insert();
			pEntry->pObj = pTarget;
			pEntry->iFireAngle = iFireAngle;
			pEntry->rDist2 = rDist2;

			//	Done?

			if (Targets.GetCount() >= iMaxCount)
				break;
			}
		}

	return Targets;
	}

CShotArray CWeaponClass::CalcConfiguration (const CDeviceItem &DeviceItem, const CWeaponFireDesc &ShotDesc, int iFireAngle) const

//	CalcConfiguration
//
//	Returns the number of shots in the configuration and initializes
//	ShotPos and ShotDir appropriately.
//
//	If ItemCtx has pSource and pDevice then ShotPos and ShotDir are absolute (source position
//	and direction has been applied). Otherwise, they are relative (caller must add to source pos and direction)
//
//	If iFireAngle is -1 and we have pSource and pDevice, we compute the current facing of the ship and device.
//
//	Returns the number of shots.

	{
	CSpaceObject *pSource = DeviceItem.GetSource();
	const CInstalledDevice *pDevice = DeviceItem.GetInstalledDevice();
	double rShotSeparationScale = (pDevice ? pDevice->GetShotSeparationScale() : 1.0);
	int iPolarity = (pDevice ? GetAlternatingPos(pDevice) : -1);

	//	Compute the source position

	CVector vSource;
	if (pSource && pDevice)
		vSource = pDevice->GetPos(pSource);

	//	Compute the fire direction

	if (iFireAngle == -1)
		iFireAngle = GetDefaultFireAngle(DeviceItem);

	//	Compute the array

	const CConfigurationDesc &Configuration = GetConfiguration(ShotDesc);
	return Configuration.CalcShots<CShotArray>(vSource, iFireAngle, iPolarity, rShotSeparationScale);
	}

Metric CWeaponClass::CalcConfigurationMultiplier (const CWeaponFireDesc *pShot, bool bIncludeFragments) const

//	CalcConfigurationMultiplier
//
//	Calculates multiplier

	{
	if (pShot == NULL)
		{
		CItemCtx ItemCtx;
		pShot = GetWeaponFireDesc(ItemCtx);
		if (pShot == NULL)
			return 1.0;
		}

	Metric rMult = GetConfiguration(*pShot).GetMultiplier();

	if (int iRepeating = GetContinuous(*pShot))
		rMult *= (iRepeating + 1.0);

	//	Include passthrough.
	//
	//  If the probability of passthrough is P, then the expected number of hits
	//  is:
	//
	//  hits = Sum(n=1-infinity) n * (P^(n-1) - P^n)
	//
	//  Empirically this converges to:
	//
	//  hits = 1/(1-P)
	//
	//  Note, however, that since targets are finite size, we put a limit on the
	//  number of passthroughs we expect.

	if (pShot->GetPassthrough() > 0)
		{
		Metric rPassthroughProb = Min(0.99, pShot->GetPassthrough() / 100.0);
		rMult *= Min(1.0 / (1.0 - rPassthroughProb), MAX_EXPECTED_PASSTHROUGH);
		}

	//	Compute fragment count

	if (bIncludeFragments
			&& pShot->HasFragments()
			&& pShot->GetFirstFragment()->Count.GetMaxValue() > 1)
		{
		//	Multiply by the average

		rMult *= pShot->GetFirstFragment()->Count.GetAveValue();

		//	If tracking, assume all will hit. Otherwise, we assume that
		//	only a fraction will hit.

		if (!pShot->GetFirstFragment()->pDesc->IsTracking())
			rMult *= EXPECTED_FRAGMENT_HITS;
		}

	return rMult;
	}

Metric CWeaponClass::CalcDamage (const CWeaponFireDesc &ShotDesc, const CItemEnhancementStack *pEnhancements, DWORD dwDamageFlags) const

//	CalcDamage
//
//	Computes damage for the given weapon fire desc.

	{
	//	Compute the damage for the shot.

	Metric rDamage = ShotDesc.CalcDamage(dwDamageFlags);

	//	If we have a capacitor, adjust damage

	switch (m_Counter)
		{
		case cntCapacitor:
			{
			//	Compute the number of ticks until we discharge the capacitor

			Metric rFireTime = (MAX_COUNTER / (Metric)-m_iCounterActivate) * GetFireDelay(ShotDesc);

			//	Compute the number of ticks to recharge

			Metric rRechargeTime = (MAX_COUNTER / (Metric)m_iCounterUpdate) * m_iCounterUpdateRate;

			//	Adjust damage by the fraction of time that we spend firing.
			//  Remember that we're recharging even while firing, so we 
			//  only care about the ratio.

			if (rRechargeTime > rFireTime)
				rDamage *= rFireTime / rRechargeTime;

			break;
			}

		case cntTemperature:
			{
			//  Compute the number of ticks until we reach max temp

			Metric rFireTime = (MAX_COUNTER / (Metric)m_iCounterActivate) * GetFireDelay(ShotDesc);

			//  Compute the number of ticks to cool down

			Metric rCoolDownTime = (MAX_COUNTER / (Metric)-m_iCounterUpdate) * m_iCounterUpdateRate;

			//  Adjust damage by the fraction of time that we spend firing.

			if (rCoolDownTime > rFireTime)
				rDamage *= rFireTime / rCoolDownTime;

			break;
			}
		}

	//	Adjust for enhancements

	if (pEnhancements)
		rDamage += (rDamage * pEnhancements->GetBonus() / 100.0);

	//	Done

	return rDamage;
	}

Metric CWeaponClass::CalcDamagePerShot (const CWeaponFireDesc &ShotDesc, const CItemEnhancementStack *pEnhancements, DWORD dwDamageFlags) const

//	CalcDamagePerShot
//
//	Returns average damage per shot

	{
	return CalcConfigurationMultiplier(&ShotDesc, false) * CalcDamage(ShotDesc, pEnhancements, dwDamageFlags);
	}

int CWeaponClass::CalcFireAngle (CItemCtx &ItemCtx, Metric rSpeed, CSpaceObject *pTarget, bool *retbSetDeviceAngle) const

//	CalcFireAngle
//
//	Calculates the default fire angle for the weapon

	{
	if (retbSetDeviceAngle)
		*retbSetDeviceAngle = false;

	CSpaceObject *pSource = ItemCtx.GetSource();
	if (pSource == NULL)
		return -1;

	CInstalledDevice *pDevice = ItemCtx.GetDevice();
	if (pDevice == NULL)
		return -1;

	const CDeviceItem DeviceItem = ItemCtx.GetDeviceItem();

	//	Get the swivel/turret parameters

	int iMinFireArc, iMaxFireArc;
	CDeviceRotationDesc::ETypes iType = GetRotationType(DeviceItem, &iMinFireArc, &iMaxFireArc);

	//	If we're firing straight, then we just fire straight

	if (iType == CDeviceRotationDesc::rotNone)
		return AngleMod(pSource->GetRotation() + iMinFireArc);

	//	If we don't have a target, then we fire straight also, but we need to 
	//	compute that based on the fire arc.

	else if (pTarget == NULL)
		return AngleMod(pSource->GetRotation() + AngleMiddle(iMinFireArc, iMaxFireArc));

	//	Otherwise, we need to compute a firing solution.

	else
		{
		int iFireAngle;

		CVector vSource = pDevice->GetPos(pSource);

		//	Get the position and velocity of the target

		CVector vTarget = pTarget->GetPos() - vSource;
		CVector vTargetVel = pTarget->GetVel() - pSource->GetVel();

		//	Figure out which direction to fire in

		Metric rTimeToIntercept = CalcInterceptTime(vTarget, vTargetVel, rSpeed);
		CVector vInterceptPoint = (rTimeToIntercept > 0.0 ? vTarget + vTargetVel * rTimeToIntercept : vTarget);
		iFireAngle = VectorToPolar(vInterceptPoint, NULL);

		//	If this is a directional weapon make sure we are in-bounds

		if (iType == CDeviceRotationDesc::rotSwivel)
			{
			int iMin = AngleMod(pSource->GetRotation() + iMinFireArc);
			int iMax = AngleMod(pSource->GetRotation() + iMaxFireArc);

			if (iMin < iMax)
				{
				if (iFireAngle < iMin)
					iFireAngle = iMin;
				else if (iFireAngle > iMax)
					iFireAngle = iMax;
				}
			else
				{
				if (iFireAngle < iMin && iFireAngle > iMax)
					{
					int iToMax = iFireAngle - iMax;
					int iToMin = iMin - iFireAngle;

					if (iToMax > iToMin)
						iFireAngle = iMin;
					else
						iFireAngle = iMax;
					}
				}
			}

		//	Remember the fire angle (we need it later if this is a continuous
		//	fire device)

		if (retbSetDeviceAngle)
			*retbSetDeviceAngle = true;

		return iFireAngle;
		}
	}

bool CWeaponClass::CalcFireSolution (const CInstalledDevice &Device, CSpaceObject &Target, int *retiAimAngle, Metric *retrDist) const

//	CalcFireSolution
//
//	Computes the direction to fire to hit the target. Returns FALSE if there is 
//	no solution, but retiAimAngle is still initialized with a best effort.

	{
	//	Init in case of early exit. By contract, we always define the resulting
	//	aim angle, even if we return false.

	if (retiAimAngle) *retiAimAngle = 0;
	if (retrDist) *retrDist = 0.0;

	const CWeaponFireDesc *pShot = GetWeaponFireDesc(Device);
	if (pShot == NULL)
		return false;

	const CSpaceObject *pSource = Device.GetSource();
	if (pSource == NULL)
		return false;

	//	We need to calculate the intercept solution.

	Metric rWeaponSpeed = pShot->GetRatedSpeed();
	CVector vTarget = Target.GetPos() - Device.GetPos(pSource);
	CVector vTargetVel = Target.GetVel() - pSource->GetVel();

	//	Figure out intercept time

	Metric rDist;
	Metric rTimeToIntercept = CalcInterceptTime(vTarget, vTargetVel, rWeaponSpeed, &rDist);

	if (retrDist) *retrDist = rDist;

	if (rTimeToIntercept < 0.0)
		{
		//	Aiming at the target is the best we can do.

		if (retiAimAngle) *retiAimAngle = VectorToPolar(vTarget);

		//	But we won't intercept...

		return false;
		}

	//	Compute direction

	CVector vInterceptPoint = vTarget + vTargetVel * rTimeToIntercept;
	if (retiAimAngle)
		*retiAimAngle = VectorToPolar(vInterceptPoint, NULL);

	return true;
	}

int CWeaponClass::CalcLevel (const CWeaponFireDesc &ShotDesc) const

//  CalcLevel
//
//  Returns the level of the given shot.

	{
	return Max(1, Min(ShotDesc.GetLevel(), MAX_ITEM_LEVEL));
	}

TArray<CTargetList::STargetResult> CWeaponClass::CalcMIRVFragmentationTargets (CSpaceObject &Source, const CWeaponFireDesc &ShotDesc, int iMaxCount)

//	CalcMIRVFragmentationTargets
//
//	Returns a list of targets for a MIRVed fragmentation shot.

	{
	TArray<CTargetList::STargetResult> Targets;
	Targets.GrowToFit(iMaxCount);

	//	Create a target list

	CTargetList::STargetOptions Options;
	Options.iMaxTargets = iMaxCount;
	Options.rMaxDist = MAX_MIRV_TARGET_RANGE;
	Options.bIncludeNonAggressors = true;
	Options.bIncludeStations = true;

	CTargetList TargetList(Source, Options);
	TargetList.Realize();

	//	Get targets.

	Metric rSpeed = ShotDesc.GetInitialSpeed();

	for (int i = 0; i < Min(iMaxCount, TargetList.GetCount()); i++)
		{
		CSpaceObject *pTarget = TargetList.GetTarget(i);
		Metric rDist2 = TargetList.GetTargetDist2(i);

		//	Calc firing solution
		//
		//	NOTE: We omit the source velocity since this is a MIRV launch and
		//	we always start from 0 velocity.

		CVector vPos = pTarget->GetPos() - Source.GetPos();
		CVector vVel = pTarget->GetVel();

		Metric rTimeToIntercept = CalcInterceptTime(vPos, vVel, rSpeed);
		if (rTimeToIntercept < 0.0)
			continue;

		CVector vInterceptPoint = vPos + vVel * rTimeToIntercept;
		int iFireAngle = VectorToPolar(vInterceptPoint);

		//	Add entry

		auto *pEntry = Targets.Insert();
		pEntry->pObj = pTarget;
		pEntry->iFireAngle = iFireAngle;
		pEntry->rDist2 = rDist2;
		}

	return Targets;
	}

int CWeaponClass::CalcPowerUsed (SUpdateCtx &UpdateCtx, CInstalledDevice *pDevice, CSpaceObject *pSource)

//	CalcPowerUsed
//
//	Returns the power consumed

	{
	CItemCtx Ctx(pSource, pDevice);

	if (!pDevice->IsEnabled())
		return 0;

	//	We consume less power when we are fully charged

	int iIdlePower;
	int iPower = GetPowerRating(Ctx, &iIdlePower);
	if (pDevice->IsReady() && (!pDevice->IsTriggered() || !pDevice->IsLastActivateSuccessful()))
		return iIdlePower;
	else
		return iPower;
	}

int CWeaponClass::CalcReachableFireAngle (const CInstalledDevice &Device, int iDesiredAngle, int iDefaultAngle) const

//	CalcReachableFireAngle
//
//	Returns the closest to iDesiredAngle that we can fire.

	{
	const CDeviceItem DeviceItem = Device.GetDeviceItem();
	const CSpaceObject &Source = Device.GetSourceOrThrow();

	int iMinFireArc, iMaxFireArc;
	CDeviceRotationDesc::ETypes iType = GetRotationType(DeviceItem, &iMinFireArc, &iMaxFireArc);

	//	If we're firing straight, then we just fire straight

	if (iType == CDeviceRotationDesc::rotNone)
		return (iDefaultAngle == -1 ? AngleMod(Source.GetRotation() + iMinFireArc) : iDefaultAngle);

	//	If we're omni, then we can always hit the desired angle.

	else if (iType == CDeviceRotationDesc::rotOmnidirectional)
		return iDesiredAngle;

	//	Otherwise, we need to see if we're in the swivel

	else
		{
		//	If this is a directional weapon make sure we are in-bounds

		int iMin = AngleMod(Source.GetRotation() + iMinFireArc);
		int iMax = AngleMod(Source.GetRotation() + iMaxFireArc);

		if (iMin < iMax)
			{
			if (iDesiredAngle < iMin)
				return iMin;
			else if (iDesiredAngle > iMax)
				return iMax;
			}
		else
			{
			if (iDesiredAngle < iMin && iDesiredAngle > iMax)
				{
				int iToMax = iDesiredAngle - iMax;
				int iToMin = iMin - iDesiredAngle;

				if (iToMax > iToMin)
					return iMin;
				else
					return iMax;
				}
			}

		//	If we get this far, then we're inside the swivel arc

		return iDesiredAngle;
		}
	}

CShotArray CWeaponClass::CalcShotsFired (CInstalledDevice &Device, const CWeaponFireDesc &ShotDesc, SActivateCtx &ActivateCtx, int &retiFireAngle, bool &retbSetFireAngle) const

//	CalcShotsFired
//
//	Figure out the configuration and targets for all shots fired.

	{
	//	MIRVed shots are different because we have a different target for each 
	//	one.

	if (IsMIRV(ShotDesc))
		{
		//	We never set the fire angle on the device because we already
		//	recalculate.

		retiFireAngle = -1;
		retbSetFireAngle = false;

		//	Figure out how many shots to create

		CDeviceItem DeviceItem = Device.GetDeviceItem();
		CShotArray Shots = CalcConfiguration(DeviceItem, ShotDesc, retiFireAngle);
		if (Shots.GetCount() == 0)
			return Shots;

		//	Get a list of targets

		auto Targets = CalcMIRVTargets(Device, ActivateCtx.TargetList, Shots.GetCount());

		//	Depending on our linked-fire state, we might not fire if we have no
		//	targets.

		switch (DeviceItem.CalcTargetType())
			{
			case CDeviceItem::calcNoTarget:
				return CShotArray();

			case CDeviceItem::calcControllerTarget:
				break;

			case CDeviceItem::calcWeaponTarget:
				{
				if (Targets.GetCount() == 0)
					return CShotArray();
				break;
				}

			default:
				return CShotArray();
			}

		//	Give each shot a target.

		if (Targets.GetCount() > 0)
			{
			bool bCanRotate = (GetRotationType(DeviceItem) != CDeviceRotationDesc::rotNone);

			for (int i = 0; i < Shots.GetCount(); i++)
				{
				const auto &Target = Targets[i % Targets.GetCount()];

				Shots[i].pTarget = Target.pObj;
				if (bCanRotate)
					Shots[i].iDir = Target.iFireAngle;
				}
			}

		return Shots;
		}

	//	Otherwise we have a single target

	else
		{
		CSpaceObject *pTarget;

		if (!CalcSingleTarget(Device, ShotDesc, ActivateCtx, retiFireAngle, pTarget, retbSetFireAngle))
			return CShotArray();

		//	Figure out how many shots to create

		CDeviceItem DeviceItem = Device.GetDeviceItem();
		CShotArray Shots = CalcConfiguration(DeviceItem, ShotDesc, retiFireAngle);
		if (Shots.GetCount() == 0)
			return Shots;

		//	Set the target

		Shots.SetTarget(pTarget);

		//	Done

		return Shots;
		}
	}

bool CWeaponClass::CalcSingleTarget (CInstalledDevice &Device, 
									 const CWeaponFireDesc &ShotDesc, 
									 SActivateCtx &ActivateCtx, 
									 int &retiFireAngle, 
									 CSpaceObject *&retpTarget, 
									 bool &retbSetFireAngle) const

//	CalcSingleTarget
//
//	Calculates a target for non-MIRV weapons. If we cannot find an appropriate 
//	target, and if the weapon requires it, we return FALSE.
//
//	NOTE: If we return FALSE, all other return variables are undefined.

	{
	ASSERT(!IsMIRV(ShotDesc));

	//	For repeating weapons, we need the target stored in the device

	if (ActivateCtx.iRepeatingCount != 0)
		{
		CDeviceItem DeviceItem = Device.GetDeviceItem();
		CSpaceObject &Source = Device.GetSourceOrThrow();

		//	No need to set it back because we either compute it each time or we 
		//	use the same value already stored.

		retbSetFireAngle = false;

		//	If we need a target, then get it from the device.

		if ((IsTracking(DeviceItem, &ShotDesc) || m_bBurstTracksTargets) && !(Source.IsBlind() && !(m_bCanFireWhenBlind)))
			{
			bool bCanReacquireTarget = ShotDesc.CanAutoTarget();
			retpTarget = Device.GetTarget(&Source);
			retiFireAngle = Device.GetFireAngle();

			//	If necessary, we recompute the fire angle

			if (retiFireAngle == -1 || m_bBurstTracksTargets)
				{
				if (retpTarget)
					{
					CItemCtx ItemCtx(&Source, &Device);
					Metric rSpeed = ShotDesc.GetInitialSpeed();
					retiFireAngle = CalcFireAngle(ItemCtx, rSpeed, retpTarget);
					}
				else
					{
					//  Reacquire target if we can do so

					if (ShotDesc.CanAutoTarget())
						{
						retpTarget = CalcBestTarget(Device, ActivateCtx.TargetList, NULL, &retiFireAngle);
						if (!retpTarget)
							retiFireAngle = -1;
						}
					else
						retiFireAngle = -1;
					}
				}
			else if (!retpTarget && (retiFireAngle == -1 || m_bBurstTracksTargets))
				{
				//  Reacquire target if we can do so
				if (bCanReacquireTarget)
					retpTarget = CalcBestTarget(Device, ActivateCtx.TargetList, NULL, &retiFireAngle);

				if (retpTarget)
					{
					CItemCtx ItemCtx(&Source, &Device);
					Metric rSpeed = ShotDesc.GetInitialSpeed();
					retiFireAngle = CalcFireAngle(ItemCtx, rSpeed, retpTarget);
					}
				else
				    retiFireAngle = -1;
				}
			}

		//	No need for a target because we just fire 

		else
			{
			retpTarget = NULL;
			retiFireAngle = Device.GetFireAngle();
			}
		}

	//	Otherwise we check the linked-fire type to see what kind of target we 
	//	need.

	else
		{
		CDeviceItem DeviceItem = Device.GetDeviceItem();

		switch (DeviceItem.CalcTargetType())
			{
			case CDeviceItem::calcNoTarget:
				return false;

			case CDeviceItem::calcControllerTarget:
				{
				CSpaceObject &Source = Device.GetSourceOrThrow();
				retpTarget = ActivateCtx.pTarget;

				if (ActivateCtx.iFireAngle != -1)
					retiFireAngle = ActivateCtx.iFireAngle;
				else if (retpTarget)
					{
					CItemCtx ItemCtx(&Source, &Device);
					Metric rSpeed = ShotDesc.GetInitialSpeed();
					retiFireAngle = CalcFireAngle(ItemCtx, rSpeed, retpTarget, &retbSetFireAngle);
					}
				else
					{
					retiFireAngle = -1;
					retbSetFireAngle = false;
					}

				break;
				}

			case CDeviceItem::calcWeaponTarget:
				{
				retpTarget = CalcBestTarget(Device, ActivateCtx.TargetList, NULL, &retiFireAngle);
				if (retpTarget == NULL)
					return false;

				//	Remember the fire angle for future bursts.

				retbSetFireAngle = true;
				break;
				}

			default:
				return false;
			}
		}

	//	Fire!

	return true;
	}

CWeaponClass::EFireResults CWeaponClass::Consume (CDeviceItem &DeviceItem, const CWeaponFireDesc &ShotDesc, int iRepeatingCount, bool *retbConsumedItems)

//	Consume
//
//	Attempts to consume resources to fire the weapon. Returns result.

	{
	if (retbConsumedItems)
		*retbConsumedItems = false;

	CSpaceObject *pSource = DeviceItem.GetSource();
	if (pSource == NULL)
		return resFailure;

	CInstalledDevice *pDevice = DeviceItem.GetInstalledDevice();
	if (pDevice == NULL)
		return resFailure;

	CItemCtx ItemCtx(pSource, pDevice);

	//	There are various ways in which we can fail.

	CFailureDesc::EFailureTypes iFailureMode = CFailureDesc::failNone;

	//	Update capacitor counters

	if (m_Counter == cntCapacitor)
		{
		if (!ConsumeCapacitor(ItemCtx, ShotDesc))
			return resFailure;
		}

	//	Update temperature counters

	else if (m_Counter == cntTemperature)
		{
		bool bSourceDestroyed;
		if (!UpdateTemperature(ItemCtx, ShotDesc, &iFailureMode, &bSourceDestroyed))
			{
			if (bSourceDestroyed || pSource->IsDestroyed())
				return resSourceDestroyed;
			else
				//	We always consume energy, regardless of	outcome.
				return resNoEffect;
			}
		}

	//  Update the ship energy/heat counter.

	if (m_iCounterPerShot != 0)
		{
		if (!UpdateShipCounter(ItemCtx, ShotDesc))
			return resFailure;
		}
	
	//	We can fail to fire but still update temperature and consume power.

	if (iFailureMode == CFailureDesc::failNoFire)
		return resNoEffect;
	
	//	See if we have enough ammo/charges to proceed. If we don't then we 
	//	cannot continue.

	if (!ConsumeAmmo(ItemCtx, ShotDesc, iRepeatingCount, retbConsumedItems))
		return resFailure;

	//	If we're damaged, disabled, or badly designed, we have a chance of 
	//	failure.

	if (pDevice->IsDamaged()
			|| pDevice->IsDisrupted()
			|| (m_iFailureChance > 0
				&& (mathRandom(1, 100) <= m_iFailureChance)))
		{
		const CFailureDesc &DamageFailure = (m_DamageFailure.IsEmpty() ? g_DefaultFailure : m_DamageFailure);
		CFailureDesc::EFailureTypes iDamageFailure = DamageFailure.Failure(pSource, pDevice);
		switch (iDamageFailure)
			{
			case CFailureDesc::failNone:
				break;

			case CFailureDesc::failExplosion:
				{
				bool bSourceDestroyed;
				FailureExplosion(ItemCtx, ShotDesc, &bSourceDestroyed);
				if (bSourceDestroyed || pSource->IsDestroyed())
					return resSourceDestroyed;
				else
					return resNoEffect;
				}

			case CFailureDesc::failMisfire:
				iFailureMode = CFailureDesc::failMisfire;
				break;

			//	For other failure modes, nothing happens (but we still consume power).

			default:
				return resNoEffect;
			}
		}

	//	If some energy field on the ship prevents us from firing, then we're 
	//	done.

	if (pSource->AbsorbWeaponFire(pDevice))
		return resNoEffect;

	//	If we get this far, then we fire something.

	if (iFailureMode == CFailureDesc::failMisfire)
		return resMisfire;
	else
		return resNormal;
	}

bool CWeaponClass::ConsumeAmmo (CItemCtx &ItemCtx, const CWeaponFireDesc &ShotDesc, int iRepeatingCount, bool *retbConsumed)

//	ConsumeAmmo
//
//	Consumes ammunition from the source. Returns TRUE if we were able to consume
//	all ammo. If no ammo needs to be consumed, we still return TRUE.
//
//	retbConsumed is set to TRUE if we consumed either ammo or charges.

	{
	//	Default to no consumption.

	if (retbConsumed)
		*retbConsumed = false;

	//	Get source and device

	CSpaceObject *pSource = ItemCtx.GetSource();
	if (pSource == NULL)
		return false;

	CInstalledDevice *pDevice = ItemCtx.GetDevice();
	if (pDevice == NULL)
		return false;

	//	If this is a repeating weapon then we only consume ammo on the first
	//	shot (unless otherwise specified).

	if (iRepeatingCount > 0 && !m_bContinuousConsumePerShot)
		return true;

	//	Figure out how much ammo we consume per shot.

	int iAmmoConsumed = FireGetAmmoToConsume(ItemCtx, ShotDesc, iRepeatingCount);

	//	Check based on the type of ammo

	bool bNextVariant = false;
	if (ShotDesc.GetAmmoType())
		{
		CItemListManipulator ItemList(pSource->GetItemList());
		CItem Item(ShotDesc.GetAmmoType(), iAmmoConsumed);

		//	We look for the ammo item. If we're using magazines, then look for
		//	the item with the least charges (use those up first).

		DWORD dwFlags = CItem::FLAG_IGNORE_CHARGES;
		if (ShotDesc.GetAmmoType()->AreChargesAmmo())
			dwFlags |= CItem::FLAG_FIND_MIN_CHARGES;

		//	Select the ammo. If we could not select it, then it means that we
		//	have none, so we fail.

		if (!ItemList.SetCursorAtItem(Item, dwFlags))
			return false;

		if (ItemList.GetItemAtCursor().GetCount() < iAmmoConsumed)
			return false;

		//	If the ammo uses charges, then we need a different algorithm.

		if (ShotDesc.GetAmmoType()->AreChargesAmmo())
			{
			const CItem &AmmoItem = ItemList.GetItemAtCursor();

			//	If we only have 1 charge left, we need to delete the item
			//
			//	(We should never have 0 charges because we delete items when we 
			//	use up the last charge. But if somehow we get it, we destroy it
			//	here too.)

			if (AmmoItem.GetCharges() <= iAmmoConsumed)
				{
				ItemList.DeleteAtCursor(AmmoItem.GetCharges());

				//	See if we have any other items with charges. If not, then
				//	we need to select the next weapon.

				if (!ItemList.SetCursorAtItem(Item, CItem::FLAG_IGNORE_CHARGES))
					bNextVariant = true;
				}

			//	Otherwise, we decrement.

			else
				ItemList.SetChargesAtCursor(AmmoItem.GetCharges() - iAmmoConsumed);
			}

		//	Otherwise, consume an item

		else
			{
			//	If we've exhausted our ammunition, remember to
			//	select the next variant

			if (ItemList.GetItemAtCursor().GetCount() == iAmmoConsumed)
				bNextVariant = true;

			ItemList.DeleteAtCursor(iAmmoConsumed);
			}

		//	We consumed an item

		if (retbConsumed)
			*retbConsumed = true;
		}
	else if (m_bCharges)
		{
		//	If no charges left, then we cannot consume

		if (pDevice->GetCharges(pSource) < iAmmoConsumed)
			return false;

		//	Consume charges

		pDevice->IncCharges(pSource, -iAmmoConsumed);
		if (retbConsumed)
			*retbConsumed = true;
		}

	//	Switch to the next variant if necessary

	if (bNextVariant)
		{
		//	For repeating weapons, we set a flag and switch variants only after
		//	we've shot the last projectile.

		if (GetContinuous(ShotDesc) > 0)
			pDevice->SetOnUsedLastAmmoFlag(true);

		//	Otherwise, switch now.

		else
			pSource->OnDeviceStatus(pDevice, statusUsedLastAmmo);
		}

	//	Success!

	return true;
	}

bool CWeaponClass::ConsumeCapacitor (CItemCtx &ItemCtx, const CWeaponFireDesc &ShotDesc)

//	ConsumeCapacitor
//
//	If we have enough capacitor power, we consume it and return TRUE. Otherwise,
//	we return FALSE.

	{
	//	Get source and device

	CSpaceObject *pSource = ItemCtx.GetSource();
	if (pSource == NULL)
		return false;

	CInstalledDevice *pDevice = ItemCtx.GetDevice();
	if (pDevice == NULL)
		return false;

	//	If we don't have enough capacitor power, then we can't fire

	if (pDevice->GetTemperature() < m_iCounterActivate)
		return false;

	//	Consume capacitor

	pDevice->IncTemperature(m_iCounterActivate);
	pSource->OnComponentChanged(comDeviceCounter);

	return true;
	}

ALERROR CWeaponClass::CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, CDeviceClass **retpWeapon)

//	CreateFromXML
//
//	Creates from an XML element

	{
	ALERROR error;
	CWeaponClass *pWeapon;

	pWeapon = new CWeaponClass;
	if (pWeapon == NULL)
		return ERR_MEMORY;

	if (error = pWeapon->InitDeviceFromXML(Ctx, pDesc, pType))
		return error;

	//	Basics

	int iFireRateSecs = pDesc->GetAttributeIntegerBounded(FIRE_RATE_ATTRIB, 0, -1, 16);
	pWeapon->m_iFireRate = mathRound(iFireRateSecs / STD_SECONDS_PER_UPDATE);
	pWeapon->m_iPowerUse = pDesc->GetAttributeIntegerBounded(POWER_USE_ATTRIB, 0, -1, 0);
	pWeapon->m_iIdlePowerUse = pDesc->GetAttributeIntegerBounded(IDLE_POWER_USE_ATTRIB, 0, -1, pWeapon->m_iPowerUse / 10);
	pWeapon->m_iRecoil = pDesc->GetAttributeInteger(RECOIL_ATTRIB);
	pWeapon->m_iFailureChance = pDesc->GetAttributeInteger(FAILURE_CHANCE_ATTRIB);
	pWeapon->m_iMinFireArc = AngleMod(pDesc->GetAttributeInteger(MIN_FIRE_ARC_ATTRIB));
	pWeapon->m_iMaxFireArc = AngleMod(pDesc->GetAttributeInteger(MAX_FIRE_ARC_ATTRIB));

	pWeapon->m_bCharges = pDesc->GetAttributeBool(CHARGES_ATTRIB);
	pWeapon->m_bOmnidirectional = pDesc->GetAttributeBool(OMNIDIRECTIONAL_ATTRIB);
	pWeapon->m_bMIRV = pDesc->GetAttributeBool(MULTI_TARGET_ATTRIB);
	pWeapon->m_bReportAmmo = pDesc->GetAttributeBool(REPORT_AMMO_ATTRIB);
	pWeapon->m_bTargetStationsOnly = pDesc->GetAttributeBool(TARGET_STATIONS_ONLY_ATTRIB);
	pWeapon->m_bContinuousConsumePerShot = pDesc->GetAttributeBool(CONTINUOUS_CONSUME_PERSHOT_ATTRIB);
	pWeapon->m_iCounterPerShot = pDesc->GetAttributeIntegerBounded(SHIP_COUNTER_PER_SHOT_ATTRIB, 0, -1, 0);
	pWeapon->m_bBurstTracksTargets = pDesc->GetAttributeBool(BURST_TRACKS_TARGETS_ATTRIB);
	pWeapon->m_bCanFireWhenBlind = pDesc->GetAttributeBool(CAN_FIRE_WHEN_BLIND_ATTRIB);
	pWeapon->m_bUsesLauncherControls = pDesc->GetAttributeBool(USES_LAUNCHER_CONTROLS_ATTRIB);

	//	Configuration

	if (ALERROR error = pWeapon->m_Configuration.InitFromWeaponClassXML(Ctx, *pDesc))
		return error;

	//	Repeat fire

	pWeapon->m_iContinuous = pDesc->GetAttributeIntegerBounded(REPEATING_ATTRIB, 0, -1, 0);
	pWeapon->m_iContinuousFireDelay = pDesc->GetAttributeIntegerBounded(REPEATING_DELAY_ATTRIB, 0, -1, 0);

	//	NOTE: For now we don't support a combination of repeating fire and 
	//	repeating delay that exceeds 254.

	if (pWeapon->m_iContinuous > CONTINUOUS_DATA_LIMIT
			|| pWeapon->m_iContinuous * pWeapon->m_iContinuousFireDelay > CONTINUOUS_DATA_LIMIT)
		{
		Ctx.sError = CONSTLIT("Unfortunately, that combination of repeating= and repeatingDelay= is too high for the engine.");
		return ERR_FAIL;
		}

	//	Counter

	CString sConfig = pDesc->GetAttribute(COUNTER_ATTRIB);
	if (!sConfig.IsBlank())
		{
		if (strEquals(sConfig, COUNTER_TYPE_TEMPERATURE))
			pWeapon->m_Counter = cntTemperature;
		else if (strEquals(sConfig, COUNTER_TYPE_CAPACITOR))
			pWeapon->m_Counter = cntCapacitor;
		else
			{
			Ctx.sError = CONSTLIT("Invalid weapon counter type");
			return ERR_FAIL;
			}

		pWeapon->m_iCounterActivate = pDesc->GetAttributeInteger(COUNTER_ACTIVATE_ATTRIB);
		pWeapon->m_iCounterUpdate = pDesc->GetAttributeInteger(COUNTER_UPDATE_ATTRIB);
		pWeapon->m_iCounterUpdateRate = pDesc->GetAttributeInteger(COUNTER_UPDATE_RATE_ATTRIB);
		if (pWeapon->m_iCounterUpdateRate <= 0)
			pWeapon->m_iCounterUpdateRate = 1;
		}
	else if ((pWeapon->m_iCounterActivate = pDesc->GetAttributeInteger(HEATING_ATTRIB)) > 0)
		{
		//	Backward compatibility

		pWeapon->m_Counter = cntTemperature;
		pWeapon->m_iCounterUpdate = TEMP_DECREASE;
		pWeapon->m_iCounterUpdateRate = pDesc->GetAttributeInteger(COOLING_RATE_ATTRIB);
		if (pWeapon->m_iCounterUpdateRate <= 0)
			pWeapon->m_iCounterUpdateRate = 1;
		}
	else
		{
		pWeapon->m_Counter = cntNone;
		pWeapon->m_iCounterActivate = 0;
		pWeapon->m_iCounterUpdate = 0;
		pWeapon->m_iCounterUpdateRate = 0;
		}

	//	Linked fire options

	CString sLinkedFire;
	if (pDesc->FindAttribute(LINKED_FIRE_ATTRIB, &sLinkedFire))
		{
		if (error = ParseLinkedFireOptions(Ctx, sLinkedFire, &pWeapon->m_dwLinkedFireOptions))
			return error;
		}
	else
		pWeapon->m_dwLinkedFireOptions = 0;

	//	Failure modes.

	CXMLElement *pFailure = pDesc->GetContentElementByTag(DAMAGE_FAILURE_TAG);
	if (pFailure)
		{
		if (error = pWeapon->m_DamageFailure.InitFromXML(Ctx, pFailure))
			return error;
		}

	if (pWeapon->m_Counter == cntTemperature)
		{
		if (error = pWeapon->m_OverheatFailure.InitFromXML(Ctx, pDesc->GetContentElementByTag(OVERHEAT_FAILURE_TAG), CFailureDesc::profileWeaponOverheat))
			return error;
		}

	//	Initialize the variants, whether from missiles, level scaling, or 
	//	charges.

	if (error = pWeapon->InitVariantsFromXML(Ctx, pDesc, pType))
		return error;

	//	Done

	*retpWeapon = pWeapon;

	return NOERROR;
	}

void CWeaponClass::FailureExplosion (CItemCtx &ItemCtx, const CWeaponFireDesc &ShotDesc, bool *retbSourceDestroyed)

//	FailureExplosion
//
//	Weapon explodes.

	{
	//	Get source and device

	CSpaceObject *pSource = ItemCtx.GetSource();
	if (pSource == NULL || pSource->IsDestroyed())
		return;

	CInstalledDevice *pDevice = ItemCtx.GetDevice();
	if (pDevice == NULL)
		return;

	SDamageCtx DamageCtx(pSource,
			const_cast<CWeaponFireDesc &>(ShotDesc),
			NULL,
			CDamageSource(pSource, killedByWeaponMalfunction),
			pSource,
			0.0,
			AngleMod(pDevice->GetPosAngle() + mathRandom(0, 30) - 15),
			pDevice->GetPos(pSource));

	//	Damage source is inside shields/overlays

	DamageCtx.bIgnoreOverlays = true;
	DamageCtx.bIgnoreShields = true;

	EDamageResults iResult = pSource->Damage(DamageCtx);

	if (retbSourceDestroyed)
		{
		if (iResult == damageDestroyed 
				|| iResult == damageDisintegrated
				|| iResult == damageShattered
				|| iResult == damagePassthroughDestroyed)
			*retbSourceDestroyed = true;
		else
			*retbSourceDestroyed = false;
		}
	}

bool CWeaponClass::FindAmmoDataField (const CItem &Ammo, const CString &sField, CString *retsValue) const

//	FindAmmoDataField
//
//	Returns meta-data

	{
	int i;

	//	Make sure we actually fire the given ammo. We need to check here because
	//	GetWeaponFireDesc doesn't check.

	ASSERT(Ammo.IsEmpty() || GetAmmoVariant(Ammo.GetType()) != -1);

	CItemCtx ItemCtx;
	CWeaponFireDesc *pShot = GetWeaponFireDesc(ItemCtx, Ammo);
	if (pShot == NULL)
		return false;

	CItem Item(GetItemType(), 1);
	CDeviceItem DeviceItem = Item.AsDeviceItemOrThrow();

	if (strEquals(sField, FIELD_AMMO_TYPE))
		{
		CItemType *pAmmoType = pShot->GetAmmoType();
		*retsValue = (pAmmoType ? strFromInt(pAmmoType->GetUNID()) : NULL_STR);
		}
	else if (strEquals(sField, FIELD_HP))
		*retsValue = strFromInt(pShot->GetHitPoints());
	else if (strEquals(sField, FIELD_MIN_DAMAGE))
		*retsValue = strFromInt(mathRound(CalcConfigurationMultiplier(pShot) * pShot->GetDamage().GetMinDamage()));
	else if (strEquals(sField, FIELD_MAX_DAMAGE))
		*retsValue = strFromInt(mathRound(CalcConfigurationMultiplier(pShot) * pShot->GetDamage().GetMaxDamage()));
	else if (strEquals(sField, FIELD_DAMAGE_TYPE))
		*retsValue = strFromInt(pShot->GetDamageType());
	else if (strEquals(sField, FIELD_FIRE_DELAY))
		*retsValue = strFromInt(GetFireDelay(*pShot));
	else if (strEquals(sField, FIELD_FIRE_RATE))
		{
		int iFireRate = GetFireDelay(*pShot);
		if (iFireRate)
			*retsValue = strFromInt(1000 / iFireRate);
		else
			return false;
		}
	else if (strEquals(sField, FIELD_AVERAGE_DAMAGE))
		*retsValue = strFromInt(mathRound(CalcDamagePerShot(*pShot) * 1000.0));
	else if (strEquals(sField, FIELD_DAMAGE_180))
		{
		Metric rDamagePerShot = CalcDamagePerShot(*pShot);
		int iFireRate = GetFireDelay(*pShot);
		*retsValue = (iFireRate > 0 ? strFromInt(mathRound(rDamagePerShot * 180.0 / iFireRate)) : strFromInt(mathRound(rDamagePerShot)));
		}
	else if (strEquals(sField, FIELD_POWER))
		*retsValue = strFromInt(m_iPowerUse * 100);
	else if (strEquals(sField, FIELD_POWER_PER_SHOT))
		*retsValue = strFromInt(mathRound(((Metric)GetFireDelay(*pShot) * (Metric)m_iPowerUse * STD_SECONDS_PER_UPDATE * 1000.0) / 600.0));
	else if (strEquals(sField, FIELD_BALANCE))
		{
		SBalance Balance;
		*retsValue = strFromInt(CalcBalance(Ammo, Balance));
		}
	else if (strEquals(sField, FIELD_RANGE))
		*retsValue = strFromInt(mathRound(pShot->GetMaxRange() / LIGHT_SECOND));
	else if (strEquals(sField, FIELD_RECOIL))
		*retsValue = (m_iRecoil ? strFromInt(mathRound((Metric)m_iRecoil * m_iRecoil * 10.0 * g_MomentumConstant / g_SecondsPerUpdate)) : NULL_STR);
	else if (strEquals(sField, FIELD_SPEED))
		*retsValue = strFromInt(mathRound(100.0 * pShot->GetRatedSpeed() / LIGHT_SECOND));
	else if (strEquals(sField, FIELD_VARIANT_COUNT))
		*retsValue = strFromInt(GetSelectVariantCount());
	else if (strEquals(sField, FIELD_REPEAT_COUNT))
		*retsValue = strFromInt(GetContinuous(*pShot) + 1);
	else if (strEquals(sField, FIELD_CONFIGURATION))
		{
		CCodeChain &CC = GetUniverse().GetCC();
		CShotArray Config = CalcConfiguration(DeviceItem, *pShot, 0);

		CMemoryWriteStream Output(5000);
		if (Output.Create() != NOERROR)
			return false;

		Output.Write("='(", 3);
		for (i = 0; i < Config.GetCount(); i++)
			{
			ICCItem *pPos = CreateListFromVector(Config[i].vPos);
			if (pPos == NULL || pPos->IsError())
				return false;

			Output.Write("(", 1);
			CString sItem = pPos->Print();
			Output.Write(sItem.GetASCIIZPointer(), sItem.GetLength());
			Output.Write(" ", 1);

			sItem = strFromInt(Config[i].iDir);
			Output.Write(sItem.GetASCIIZPointer(), sItem.GetLength());
			Output.Write(") ", 2);

			pPos->Discard();
			}
		Output.Write(")", 1);
		Output.Close();

		*retsValue = CString(Output.GetPointer(), Output.GetLength());
		return true;
		}
	else if (strEquals(sField, FIELD_IS_ALTERNATING))
		*retsValue = (GetConfiguration(*pShot).IsAlternating() ? CString("True") : NULL_STR);
	else
		return pShot->FindDataField(sField, retsValue);

	return true;
	}

bool CWeaponClass::FindDataField (const CString &sField, CString *retsValue)

//	FindDataField
//
//	Returns meta-data

	{
	CString sRootField = sField;
	int iVariant = 0;

	//	Look for a :nn suffix specifying a variant

	const char *pPos = sField.GetASCIIZPointer();
	while (*pPos != '\0')
		{
		if (*pPos == ':')
			{
			sRootField.Truncate(pPos - sField.GetASCIIZPointer());
			iVariant = strParseInt(pPos + 1, 0);
			break;
			}
		pPos++;
		}

	CItem Ammo;
	if (iVariant >= 0 && iVariant < GetAmmoItemCount())
		Ammo = CItem(GetAmmoItem(iVariant), 1);

	return FindAmmoDataField(Ammo, sRootField, retsValue);
	}

bool CWeaponClass::FireAllShots (CInstalledDevice &Device, const CWeaponFireDesc &ShotDesc, CShotArray &Shots, int iRepeatingCount, SShotFireResult &retResult)

//	FireAllShots
//
//	Fires all shots and returns an aggregated SShotFireResult structure.

	{
	CSpaceObject &Source = Device.GetSourceOrThrow();
	CItemCtx ItemCtx(&Source, &Device);
	Metric rSpeed = ShotDesc.GetInitialSpeed();

	retResult.vRecoil = CVector();
	retResult.bRecoil = false;
	retResult.bShotFired = false;
	retResult.bSoundEffect = false;

	for (int i = 0; i < Shots.GetCount(); i++)
		{
		SShotFireResult Result;

		//	Fire out to event, if the weapon has one.
		//	Otherwise, we create weapon fire

		if (FireOnFireWeapon(ItemCtx, 
				ShotDesc, 
				Shots[i].vPos, 
				Shots[i].pTarget,
				Shots[i].iDir, 
				iRepeatingCount,
				Result))
			{
			//	Did we destroy the source?

			if (Source.IsDestroyed())
				return false;
			}

		//	Otherwise, fire default

		else
			FireWeaponShot(&Source, &Device, ShotDesc, Shots[i].vPos, Shots[i].iDir, rSpeed, Shots[i].pTarget, iRepeatingCount, i);

		//	Create the barrel flash effect, unless canceled

		if (Result.bFireEffect)
			ShotDesc.CreateFireEffect(Source.GetSystem(), &Source, Shots[i].vPos, CVector(), Shots[i].iDir);

		//	Create the sound effect, if necessary

		if (Result.bSoundEffect)
			retResult.bSoundEffect = true;

		//	Recoil

		if (m_iRecoil && Result.bRecoil)
			{
			retResult.bRecoil = true;
			retResult.vRecoil = retResult.vRecoil + PolarToVector(Shots[i].iDir, 1.0);
			}

		//	If we fired, then remember that.

		if (Result.bShotFired)
			retResult.bShotFired = true;
		}

	return retResult.bShotFired;
	}

bool CWeaponClass::FireGetAmmoCountToDisplay (const CDeviceItem &DeviceItem, const CWeaponFireDesc &Shot, int *retiAmmoCount) const

//	FireGetAmmoCountToDisplay
//
//	Fires <GetAmmoCountToDisplay> and returns the ammo count.
//
//	NOTE: This is only used to change the display value, it does not affect the
//	actual ammo count available. See: https://github.com/kronosaur/TranscendenceDev/pull/18

	{
	SEventHandlerDesc Event;
	if (!FindEventHandlerWeaponClass(evtGetAmmoCountToDisplay, &Event))
		return false;

	CCodeChainCtx CCX(GetUniverse());
	CCX.DefineContainingType(GetItemType());
	CCX.SaveAndDefineSourceVar(DeviceItem.GetSource());
	CCX.SaveAndDefineItemVar(DeviceItem);
	CCX.DefineItemType(CONSTLIT("aWeaponType"), Shot.GetWeaponType());

	ICCItemPtr pResult = CCX.RunCode(Event);

	if (pResult->IsError())
		{
		DeviceItem.ReportEventError(GET_AMMO_COUNT_TO_DISPLAY_EVENT, *pResult);
		return false;
		}
	else if (pResult->IsNumber() && !pResult->IsNil())
		{
		if (retiAmmoCount)
			*retiAmmoCount = pResult->GetIntegerValue();

		return true;
		}
	else
		return false;
	}

int CWeaponClass::FireGetAmmoToConsume (CItemCtx &ItemCtx, const CWeaponFireDesc &ShotDesc, int iRepeatingCount)

//	FireOnFireWeapon
//
//	Fires OnFireWeapon event
//
//	default (or Nil) = fire weapon as normal
//	noShot = do not consume power or ammo
//	shotFired (or True) = consume power and ammo normally

	{
	SEventHandlerDesc Event;
	if (FindEventHandlerWeaponClass(evtGetAmmoToConsume, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());
		int iResult;
	
		Ctx.DefineContainingType(GetItemType());
		Ctx.SaveAndDefineSourceVar(ItemCtx.GetSource());
		Ctx.SaveAndDefineItemVar(ItemCtx);
		Ctx.DefineInteger(CONSTLIT("aFireRepeat"), iRepeatingCount);
		Ctx.DefineItemType(CONSTLIT("aWeaponType"), ShotDesc.GetWeaponType());

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ItemCtx.GetSource()->ReportEventError(GET_AMMO_TO_CONSUME_EVENT, pResult);

		if (pResult->IsInteger())
			{
			iResult = pResult->GetIntegerValue();
			}
		else
			{
			iResult = 1;
			}

		Ctx.Discard(pResult);

		//	Done

		return iResult;
		}
	else
		return 1;
	}

bool CWeaponClass::FireOnFireWeapon (CItemCtx &ItemCtx, 
									 const CWeaponFireDesc &ShotDesc,
									 const CVector &vSource,
									 CSpaceObject *pTarget,
									 int iFireAngle,
									 int iRepeatingCount,
									 SShotFireResult &retResult)

//	FireOnFireWeapon
//
//	Fires OnFireWeapon event. We return FALSE if we should still fire the
//	weapon as normal. TRUE means that we handled the weapon and that retResult
//	is initialized with further options.
//
//	default (or Nil) = fire weapon as normal
//	noShot = do not consume power or ammo
//	shotFired (or True) = consume power and ammo normally

	{
	SEventHandlerDesc Event;
	if (!FindEventHandlerWeaponClass(evtOnFireWeapon, &Event))
		return false;

	retResult = SShotFireResult();

	CCodeChainCtx Ctx(GetUniverse());
	TSharedPtr<CItemEnhancementStack> pEnhancements = ItemCtx.GetEnhancementStack();

	Ctx.DefineContainingType(GetItemType());
	Ctx.SaveAndDefineSourceVar(ItemCtx.GetSource());
	Ctx.SaveAndDefineItemVar(ItemCtx);
	Ctx.DefineInteger(CONSTLIT("aFireAngle"), iFireAngle);
	Ctx.DefineVector(CONSTLIT("aFirePos"), vSource);
	Ctx.DefineInteger(CONSTLIT("aFireRepeat"), iRepeatingCount);
	Ctx.DefineSpaceObject(CONSTLIT("aTargetObj"), pTarget);
	Ctx.DefineInteger(CONSTLIT("aWeaponBonus"), (pEnhancements ? pEnhancements->GetBonus() : 0));
	Ctx.DefineItemType(CONSTLIT("aWeaponType"), ShotDesc.GetWeaponType());

	ICCItemPtr pResult = Ctx.RunCode(Event);
	if (pResult->IsError())
		{
		ItemCtx.GetSource()->ReportEventError(ON_FIRE_WEAPON_EVENT, pResult);
		return true;
		}
	else if (pResult->IsNil())
		return false;

	else if (pResult->IsTrue() || pResult->GetBooleanAt(CONSTLIT("shotFired")))
		return true;

	else
		{
		retResult.bShotFired = !pResult->GetBooleanAt(CONSTLIT("noShot"));
		retResult.bFireEffect = retResult.bShotFired && !pResult->GetBooleanAt("noFireEffect");
		retResult.bSoundEffect = retResult.bShotFired && !pResult->GetBooleanAt("noSoundEffect");
		retResult.bRecoil = retResult.bShotFired && !pResult->GetBooleanAt("noRecoil");

		return true;
		}

	//	Done

	return true;
	}

bool CWeaponClass::FireWeapon (CInstalledDevice &Device, 
							   const CWeaponFireDesc &ShotDesc, 
							   SActivateCtx &ActivateCtx)

//	FireWeapon
//
//	Fires the weapon. Returns TRUE if we should consume power, etc.

	{
	CSpaceObject &Source = Device.GetSourceOrThrow();
	CDeviceItem DeviceItem = Device.GetDeviceItem();

	//	Compute the shots to fire

	bool bSetFireAngle;
	int iFireAngle;
	CShotArray Shots = CalcShotsFired(Device, ShotDesc, ActivateCtx, iFireAngle, bSetFireAngle);
	if (Shots.GetCount() == 0)
		return false;

	//	Figure out when happens when we try to consume ammo, etc.

	EFireResults iResult = Consume(DeviceItem, ShotDesc, ActivateCtx.iRepeatingCount, &ActivateCtx.bConsumedItems);
	switch (iResult)
		{
		case resFailure:
		case resSourceDestroyed:
			return false;

		case resNoEffect:
			//	TRUE means we consume power
			return true;

		case resMisfire:
			{
			int iAngleAdj = mathRandom(-60, 60);
			Shots.AdjustFireAngle(iAngleAdj);
			iFireAngle = AngleMod(iFireAngle + iAngleAdj);
			break;
			}

		default:
			break;
		}

	//	After this point, we will always fire a shot. iFailureMode is either 
	//	failNone or failMisfire.

	//	Set the device angle so that repeating weapons can get access to it.

	if (bSetFireAngle)
		{
		Device.SetTarget(Shots[0].pTarget);
		Device.SetFireAngle(iFireAngle);
		}
	else if (ActivateCtx.iRepeatingCount == 0)
		{
		Device.SetFireAngle(-1);
		}

	//	Increment polarity, if necessary

	int iNewPolarity;
	if ((ActivateCtx.iRepeatingCount == GetContinuous(ShotDesc))
			&& GetConfiguration(ShotDesc).IncPolarity(GetAlternatingPos(&Device), &iNewPolarity))
		SetAlternatingPos(&Device, iNewPolarity);

	//	Remember last shot count

	if (ShotDesc.GetFireType() == CWeaponFireDesc::ftContinuousBeam && !Device.HasLastShots())
		Device.SetLastShotCount(Shots.GetCount());

	//	Create all the shots

	SShotFireResult Result;
	if (!FireAllShots(Device, ShotDesc, Shots, ActivateCtx.iRepeatingCount, Result))
		return false;

	//	Sound effect

	if (Result.bSoundEffect)
		ShotDesc.PlayFireSound(&Source);

	//	Recoil

	if (Result.bRecoil)
		{
		CVector vAccel = Result.vRecoil.Normal() * (Metric)(-10.0 * m_iRecoil * m_iRecoil);
		Source.AddForce((g_MomentumConstant / g_SecondsPerUpdate) * vAccel);
		}

	//	Done!

	return true;
	}

void CWeaponClass::FireWeaponShot (CSpaceObject *pSource, 
								   CInstalledDevice *pDevice, 
								   const CWeaponFireDesc &ShotDesc, 
								   const CVector &vShotPos, 
								   int iShotDir, 
								   Metric rShotSpeed, 
								   CSpaceObject *pTarget,
								   int iRepeatingCount,
								   int iShotNumber)

//	FireWeaponShot
//
//	Fires an actual shot.

	{
	CDamageSource Source(pSource, killedByDamage);
	CSpaceObject *pNewObj;

	//	If this shot was created by automated weapon fire, then set flag

	if (pDevice->IsAutomatedWeapon())
		Source.SetAutomatedWeapon();

	//	Flags for the type of shot

	DWORD dwFlags = SShotCreateCtx::CWF_WEAPON_FIRE;
	if (iShotNumber == 0)
		dwFlags |= SShotCreateCtx::CWF_PRIMARY;

	if (iRepeatingCount != 0)
		dwFlags |= SShotCreateCtx::CWF_REPEAT;

	//	If this is a continuous beam, then we need special code.

	if (ShotDesc.GetFireType() == CWeaponFireDesc::ftContinuousBeam
			&& (pNewObj = pDevice->GetLastShot(pSource, iShotNumber))
			&& (ShotDesc.IsCurvedBeam() || pNewObj->GetRotation() == iShotDir))
		{
		pNewObj->AddContinuousBeam(vShotPos,
				pSource->GetVel() + PolarToVector(iShotDir, rShotSpeed),
				iShotDir);

		//	Fire system events, which are normally fired inside CreateWeaponFireDesc

		pSource->GetSystem()->FireSystemWeaponEvents(pNewObj, const_cast<CWeaponFireDesc *>(&ShotDesc), Source, iRepeatingCount, dwFlags);
		}

	//	Otherwise, create a shot

	else
		{
		//	Create

		SShotCreateCtx Ctx;
		Ctx.pDesc = const_cast<CWeaponFireDesc *>(&ShotDesc);
		Ctx.pEnhancements = pDevice->GetEnhancementStack();
		Ctx.Source = Source;
		Ctx.vPos = vShotPos;
		Ctx.vVel = pSource->GetVel() + PolarToVector(iShotDir, rShotSpeed);
		Ctx.iDirection = iShotDir;
		Ctx.iRepeatingCount = iRepeatingCount;
		Ctx.pTarget = pTarget;
		Ctx.dwFlags = dwFlags;
		pSource->GetSystem()->CreateWeaponFire(Ctx, &pNewObj);

		//	Remember the shot, if necessary

		if (ShotDesc.GetFireType() == CWeaponFireDesc::ftContinuousBeam)
			pDevice->SetLastShot(pNewObj, iShotNumber);
		}
	}

int CWeaponClass::GetActivateDelay (CItemCtx &ItemCtx) const

//	GetActivateDelay
//
//	Returns the number of ticks between shots
//	NOTE: We do not adjust for enhancements.

	{
	const CWeaponFireDesc *pShot = GetWeaponFireDesc(ItemCtx);
	if (pShot == NULL)

		//	If no shot then it could be that we have a launcher with no 
		//	missiles. In that case we just return the launcher fire rate.

		return m_iFireRate;

	return GetFireDelay(*pShot);
	}

CItemType *CWeaponClass::GetAmmoItem (int iIndex) const

//  GetAmmoItem
//
//  Returns the nth ammo item compatible with this weapon.

	{
	ASSERT(iIndex >= 0 && iIndex < GetAmmoItemCount());
	return m_ShotData[iIndex].pDesc->GetAmmoType();
	}

int CWeaponClass::GetAmmoItemCount (void) const

//  GetAmmoItemCount
//
//  Returns the number of ammo items that we are compatible with.

	{
	if (!UsesAmmo())
		return 0;

	//  Launchers use the m_ShotData array to track each missile. We can assume
	//	that each entry is a missile.

	else if (IsLauncherWithAmmo())
		return m_ShotData.GetCount();

	//	NOTE: We can't assume that m_ShotData only has one entry because we 
	//	could be a scaling weapon.

	else
		return 1;
	}

const CConfigurationDesc &CWeaponClass::GetConfiguration (const CWeaponFireDesc &ShotDesc) const

//	GetConfiguration
//
//	Returns the configuration to use.

	{
	const CConfigurationDesc &ShotConfiguration = ShotDesc.GetConfiguration();
	if (!ShotConfiguration.IsEmpty())
		return ShotConfiguration;

	return m_Configuration;
	}

int CWeaponClass::GetContinuous (const CWeaponFireDesc &Shot) const

//	GetContinous
//
//	If this is a repeating fire weapon, we return the number of additional 
//	shots. 0 means no repeat fire.

	{
	//	Check the shot first, which can override the weapon.

	int iRepeating = Shot.GetContinuous();
	if (iRepeating != -1)
		return iRepeating;

	//	Check the weapon

	return m_iContinuous;
	}

int CWeaponClass::GetContinuousFireDelay (const CWeaponFireDesc &Shot) const

//	GetContinuousFireDelay
//
//	Returns the number of ticks between repeating shots. 0 means that shots come
//	each tick with no delay in between.

	{
	//	Check the shot first, which can override the weapon

	int iDelay = Shot.GetContinuousFireDelay();
	if (iDelay != -1)
		return iDelay;

	//	Check the weapon

	return m_iContinuousFireDelay;
	}

int CWeaponClass::GetFireDelay (const CWeaponFireDesc &ShotDesc) const

//	GetFireDelay
//
//	Returns number of ticks to wait before we can shoot again.

	{
	//	See if the shot overrides fire rate

	int iShotFireRate;
	if ((iShotFireRate = ShotDesc.GetFireDelay()) != -1)
		return iShotFireRate;

	//	Otherwise, based on weapon

	return m_iFireRate;
	}

DWORD CWeaponClass::GetTargetTypes (const CDeviceItem &DeviceItem) const

//	GetTargetTypes
//
//	Returns the type of targets that we need. These are bitflags from
//	CTargetList::ETargetTypes.

	{
	const CWeaponFireDesc *pShotDesc = GetWeaponFireDesc(DeviceItem);
	if (pShotDesc == NULL)
		return 0;

	//	We can always hit ships, etc.

	DWORD dwTargetTypes = CTargetList::typeAttacker | CTargetList::typeFortification;

	//	If we swivel or have tracking, then check to see if there are other
	//	targets that we need.

	if (GetRotationType(DeviceItem) != CDeviceRotationDesc::rotNone
			|| IsTracking(DeviceItem, pShotDesc)
			|| IsMIRV(*pShotDesc))
		{
		//	See if we can target missiles

		const CItemEnhancementStack &Enhancements = DeviceItem.GetEnhancements();
		if (Enhancements.IsMissileDefense())
			dwTargetTypes |= CTargetList::typeMissile;

		//	Check targetable missiles (compatibility)

		if (const CInstalledDevice *pInstalled = DeviceItem.GetInstalledDevice())
			{
			if (pInstalled->CanTargetMissiles())
				dwTargetTypes |= CTargetList::typeTargetableMissile;
			}

		//	See if we have mining capability

		if (pShotDesc->GetDamage().GetMiningDamage() > 0
				|| Enhancements.HasSpecialDamage(specialMining))
			dwTargetTypes |= CTargetList::typeMinable;
		}

	return dwTargetTypes;
	}

bool CWeaponClass::HasAmmoLeft (CItemCtx &ItemCtx, const CWeaponFireDesc *pShot) const

//	HasAmmoLeft
//
//	Returns TRUE if we can fire at least one shot.

	{
	CSpaceObject *pSource = ItemCtx.GetSource();
	if (pSource == NULL)
		return false;

	CInstalledDevice *pDevice = ItemCtx.GetDevice();
	if (pDevice == NULL)
		return false;

	//	If ammo items...

	if (pShot->GetAmmoType())
		{
		CItemListManipulator ItemList(pSource->GetItemList());
		CItem Item(pShot->GetAmmoType(), 1);
		if (!ItemList.SetCursorAtItem(Item, CItem::FLAG_IGNORE_CHARGES))
			return false;
		}

	//	If charges...

	else if (m_bCharges)
		{
		if (pDevice->GetCharges(pSource) <= 0)
			return false;
		}

	//	Otherwise, we can fire.

	return true;
	}

ICCItem *CWeaponClass::FindAmmoItemProperty (CItemCtx &Ctx, const CItem &Ammo, const CString &sProperty)

//  FindAmmoItemProperty
//
//  Returns a property of the weapon when the given ammo item is selected.

	{
	int i;
	CCodeChain &CC = GetUniverse().GetCC();

	//	Make sure we actually fire the given ammo. We need to check here because
	//	GetWeaponFireDesc doesn't check.

	ASSERT(Ammo.IsEmpty() || GetAmmoVariant(Ammo.GetType()) != -1);

	//	Get the shot

	CWeaponFireDesc *pShot = GetWeaponFireDesc(Ctx, Ammo);
	if (pShot == NULL)
		return CDeviceClass::FindAmmoItemProperty(Ctx, Ammo, sProperty);

	const CDeviceItem DeviceItem = Ctx.GetDeviceItem();
	const CItem AmmoItem = (Ctx.GetItem().IsMissile() ? Ctx.GetItem() : CItem());

	//	Enhancements

	TSharedPtr<CItemEnhancementStack> pEnhancements = Ctx.GetEnhancementStack();

	//	Get the property

	ICCItem *pResult;
	if (strEquals(sProperty, PROPERTY_AMMO_TYPES))
		{
		if (UsesAmmo() && !AmmoItem)
			{
			pResult = CC.CreateLinkedList();
			for (i = 0; i < m_ShotData.GetCount(); i++)
				if (m_ShotData[i].pDesc->GetAmmoType())
					pResult->AppendInteger(m_ShotData[i].pDesc->GetAmmoType()->GetUNID());

			return pResult;
			}
		else
			return CC.CreateNil();
		}

	else if (strEquals(sProperty, PROPERTY_AVERAGE_DAMAGE))
		return CC.CreateDouble(CalcDamagePerShot(*pShot, pEnhancements));

	else if (strEquals(sProperty, PROPERTY_BALANCE))
		{
		SBalance Balance;
		CalcBalance(AmmoItem, Balance);
		if (Balance.iLevel == 0)
			return CC.CreateNil();

		return CC.CreateInteger(mathRound(Balance.rBalance));
		}
	else if (strEquals(sProperty, PROPERTY_BALANCE_DAMAGE))
		{
		SBalance Balance;
		CalcBalance(AmmoItem, Balance);
		if (Balance.iLevel == 0)
			return CC.CreateNil();

		const SStdStats &Stats = STD_WEAPON_STATS[Balance.iLevel - 1];

		//  Compute the balance assuming that damage is standard.

		Metric rDelta = Balance.rBalance - Balance.rDamage;

		//  Compute cost to compensate for this balance

		Metric rDamage = Balance.rStdDamage180 - (Balance.rStdDamage180 * rDelta / 100.0);

		return CC.CreateInteger(mathRound(rDamage));
		}
	else if (strEquals(sProperty, PROPERTY_BALANCE_COST))
		{
		SBalance Balance;
		CalcBalance(AmmoItem, Balance);
		if (Balance.iLevel == 0)
			return CC.CreateNil();

		const SStdStats &Stats = STD_WEAPON_STATS[Balance.iLevel - 1];

		//  Compute the balance assuming that cost is standard.

		Metric rBalance = Balance.rBalance - Balance.rCost;

		//  Compute cost to compensate for this balance

		Metric rCostDelta = -rBalance / BALANCE_COST_RATIO;
		Metric rCostCredits = (Metric)Stats.Cost + (Stats.Cost * rCostDelta / 100.0);

		return CC.CreateInteger((int)GetItemType()->GetCurrencyType()->Exchange(NULL, (CurrencyValue)rCostCredits));
		}
	else if (strEquals(sProperty, PROPERTY_BALANCE_EXCLUDE_COST))
		{
		SBalance Balance;
		CalcBalance(AmmoItem, Balance);
		if (Balance.iLevel == 0)
			return CC.CreateNil();

		return CC.CreateInteger(mathRound(Balance.rBalance - Balance.rCost));
		}
	else if (strEquals(sProperty, PROPERTY_DAMAGE_180))
		{
		Metric rDamagePerShot = CalcDamagePerShot(*pShot, pEnhancements);
		int iDelay = CalcActivateDelay(Ctx);
		return CC.CreateInteger(iDelay > 0 ? mathRound(rDamagePerShot * 180.0 / iDelay) : mathRound(rDamagePerShot));
		}

	else if (strEquals(sProperty, PROPERTY_DAMAGE_PER_PROJECTILE))
		return CC.CreateDouble(CalcDamage(*pShot, pEnhancements));

	else if (strEquals(sProperty, PROPERTY_DAMAGE_TYPE_ID))
		return CC.CreateString(::GetDamageType(pShot->GetDamageType()));

	else if (strEquals(sProperty, PROPERTY_DAMAGE_WMD_180))
		{
		Metric rDamagePerShot = CalcDamagePerShot(*pShot, pEnhancements, DamageDesc::flagWMDAdj);
		int iDelay = CalcActivateDelay(Ctx);
		return CC.CreateInteger(iDelay > 0 ? mathRound(rDamagePerShot * 180.0 / iDelay) : mathRound(rDamagePerShot));
		}

	else if (strEquals(sProperty, PROPERTY_EFFECTIVE_RANGE))
		return CC.CreateInteger(mathRound(pShot->GetEffectiveRange() / LIGHT_SECOND));

	else if (strEquals(sProperty, PROPERTY_FIRE_ARC))
		{
		int iMinFireArc;
		int iMaxFireArc;

		switch (GetRotationType(DeviceItem, &iMinFireArc, &iMaxFireArc))
			{
			case CDeviceRotationDesc::rotOmnidirectional:
				return CC.CreateString(PROPERTY_OMNIDIRECTIONAL);

			case CDeviceRotationDesc::rotSwivel:
				{
				//	Create a list

				ICCItem *pResult = CC.CreateLinkedList();
				if (pResult->IsError())
					return pResult;

				pResult->AppendInteger(iMinFireArc);
				pResult->AppendInteger(iMaxFireArc);

				return pResult;
				}

			default:
				{
				if (iMinFireArc == 0)
					return CC.CreateNil();
				else
					return CC.CreateInteger(iMinFireArc);
				}
			}
		}

	else if (strEquals(sProperty, PROPERTY_FIRE_DELAY))
		return CC.CreateInteger(CalcActivateDelay(Ctx));

	else if (strEquals(sProperty, PROPERTY_FIRE_RATE))
		{
		Metric rDelay = CalcActivateDelay(Ctx);
		if (rDelay <= 0.0)
			return CC.CreateNil();

		return CC.CreateInteger(mathRound(1000.0 / rDelay));
		}

	else if (strEquals(sProperty, PROPERTY_LINKED_FIRE_OPTIONS))
		{
		//	Get the options from the device

		DWORD dwOptions = DeviceItem.GetLinkedFireOptions();
		if (dwOptions == 0)
			return CC.CreateNil();

		//	Create a list

		ICCItem *pResult = CC.CreateLinkedList();
		if (pResult->IsError())
			return pResult;

		CCLinkedList *pList = (CCLinkedList *)pResult;

		//	Add options

		if (dwOptions & CDeviceClass::lkfAlways)
			pList->AppendString(CDeviceClass::GetLinkedFireOptionString(CDeviceClass::lkfAlways));
		else if (dwOptions & CDeviceClass::lkfTargetInRange)
			pList->AppendString(CDeviceClass::GetLinkedFireOptionString(CDeviceClass::lkfTargetInRange));
		else if (dwOptions & CDeviceClass::lkfEnemyInRange)
			pList->AppendString(CDeviceClass::GetLinkedFireOptionString(CDeviceClass::lkfEnemyInRange));
		else if (dwOptions & CDeviceClass::lkfSelected)
			pList->AppendString(CDeviceClass::GetLinkedFireOptionString(CDeviceClass::lkfSelected));
		else if (dwOptions & CDeviceClass::lkfSelectedVariant)
			pList->AppendString(CDeviceClass::GetLinkedFireOptionString(CDeviceClass::lkfSelectedVariant));
		else if (dwOptions & CDeviceClass::lkfNever)
			pList->AppendString(CDeviceClass::GetLinkedFireOptionString(CDeviceClass::lkfNever));

		//	Done

		return pResult;
		}

	else if (strEquals(sProperty, PROPERTY_MAX_DAMAGE))
		return CC.CreateDouble(CalcDamagePerShot(*pShot, pEnhancements, DamageDesc::flagMaxDamage));

	else if (strEquals(sProperty, PROPERTY_MIN_DAMAGE))
		return CC.CreateDouble(CalcDamagePerShot(*pShot, pEnhancements, DamageDesc::flagMinDamage));

	else if (strEquals(sProperty, PROPERTY_MULTI_SHOT))
		return CC.CreateBool(GetConfiguration(*pShot).GetType() != CConfigurationDesc::ctSingle);

	else if (strEquals(sProperty, PROPERTY_CAN_FIRE_WHEN_BLIND))
		return CC.CreateBool(m_bCanFireWhenBlind);

	else if (strEquals(sProperty, PROPERTY_OMNIDIRECTIONAL))
		return CC.CreateBool(GetRotationType(DeviceItem) == CDeviceRotationDesc::rotOmnidirectional);

	else if (strEquals(sProperty, PROPERTY_REPEATING))
		return CC.CreateInteger(GetContinuous(*pShot));

	else if (strEquals(sProperty, PROPERTY_SHIP_COUNTER_PER_SHOT))
		{
		return CC.CreateInteger(m_iCounterPerShot);
		}

	else if (strEquals(sProperty, PROPERTY_SINGLE_POINT_ORIGIN))
		return CC.CreateBool(IsSinglePointOrigin());

	else if (strEquals(sProperty, PROPERTY_STD_COST))
		{
		if (AmmoItem)
			{
			SBalance Balance;
			CalcBalance(AmmoItem, Balance);
			if (Balance.iLevel == 0)
				return CC.CreateNil();

			return CC.CreateInteger(mathRound(Balance.rStdAmmoCost));
			}
		else
			{
			const SStdStats &Stats = STD_WEAPON_STATS[CalcLevel(*pShot) - 1];
			return CC.CreateDouble((Metric)Stats.Cost);
			}
		}

	else if (strEquals(sProperty, PROPERTY_STD_MASS))
		{
		if (AmmoItem)
			{
			SBalance Balance;
			CalcBalance(AmmoItem, Balance);
			if (Balance.iLevel == 0)
				return CC.CreateNil();

			return CC.CreateInteger(mathRound(Balance.rStdAmmoMass));
			}
		else
			return CC.CreateNil();
		}

	//	See if the shot has the property

	else if (pResult = pShot->FindProperty(sProperty))
		return pResult;

	//	If the item is a missile, then we default to items properties

	else if (Ctx.GetItem().GetType() && Ctx.GetItem().GetType()->IsMissile())
		{
		CString sValue;
		if (FindAmmoDataField(Ctx.GetItem(), sProperty, &sValue))
			return CreateResultFromDataField(CC, sValue);

		else if (Ctx.GetItem().GetType()->FindDataField(sProperty, &sValue))
			return CreateResultFromDataField(CC, sValue);

		else
			return NULL;
		}

	//	Otherwise, just get the property from the base class

	else
		return CDeviceClass::FindAmmoItemProperty(Ctx, Ammo, sProperty);
	}

int CWeaponClass::GetAmmoVariant (const CItemType *pItem) const

//	GetAmmoVariant
//
//	Returns the variant that fires the given ammo (or -1 if the weapons
//	does not fire the ammo)

	{
	int i;

	DWORD dwItemUNID = (pItem ? pItem->GetUNID() : 0);
	if (dwItemUNID == 0)
		return -1;

	//	Find it

	for (i = 0; i < m_ShotData.GetCount(); i++)
		{
		if (m_ShotData[i].pDesc->GetAmmoType()
				&& m_ShotData[i].pDesc->GetAmmoType()->GetUNID() == dwItemUNID)

			//	Launchers take multiple ammo; others do not.

			return (IsLauncher() ? i : 0);
		}

	return -1;
	}

ItemCategories CWeaponClass::GetImplCategory (void) const

//	GetImplCategory
//
//	Returns the weapon class category

	{
	if (IsLauncher())
		return itemcatLauncher;
	else
		return itemcatWeapon;
	}

int CWeaponClass::GetCounter (CInstalledDevice *pDevice, CSpaceObject *pSource, CounterTypes *retiType, int *retiLevel)

//	GetCounter
//
//	Return counter

	{
	//	Return the type

	if (retiType)
		*retiType = m_Counter;

	if (m_Counter == cntNone || pDevice == NULL || pSource == NULL)
		{
		if (retiLevel)
			*retiLevel = 0;

		return 0;
		}

	//	If we're a capacitor, then don't show the counter if we are full

	if (m_Counter == cntCapacitor && pDevice->GetTemperature() >= MAX_COUNTER)
		{
		if (retiLevel)
			*retiLevel = MAX_COUNTER;

		return 0;
		}

	//	Otherwise, return the current value

	if (retiLevel)
		*retiLevel = pDevice->GetTemperature();

	return pDevice->GetTemperature();
	}

int CWeaponClass::GetAlternatingPos (const CInstalledDevice *pDevice) const
	{
	return (int)(DWORD)HIBYTE(LOWORD(pDevice->GetData()));
	}

DWORD CWeaponClass::GetContinuousFire (const CInstalledDevice *pDevice) const
	{
	return pDevice->GetContinuousFire();
	}

int CWeaponClass::GetCurrentVariant (const CInstalledDevice *pDevice) const
	{
	return (int)(short)HIWORD(pDevice->GetData()); 
	}

const DamageDesc *CWeaponClass::GetDamageDesc (CItemCtx &Ctx)

//	GetDamageDesc
//
//	Returns the kind of damage caused by this weapon

	{
	CWeaponFireDesc *pShot = GetWeaponFireDesc(Ctx);
	if (pShot == NULL)
		return NULL;

	return &pShot->GetDamage();
	}

DamageTypes CWeaponClass::GetDamageType (CItemCtx &Ctx, const CItem &Ammo) const

//	GetDamageType
//
//	Returns the kind of damage caused by this weapon

	{
	CWeaponFireDesc *pShot = GetWeaponFireDesc(Ctx, Ammo);

	//	OK if we don't find shot--could be a launcher with no ammo

	if (pShot == NULL)
		return damageGeneric;

	//	Get the damage type

	return pShot->GetDamageType();
	}

int CWeaponClass::GetDefaultFireAngle (const CDeviceItem &DeviceItem) const

//	GetDefaultFireAngle
//
//	Gets the natural fire direction (not counting omni or swivel mounts)

	{
	if (const CInstalledDevice *pDevice = DeviceItem.GetInstalledDevice())
		{
		CSpaceObject &Source = pDevice->GetSourceOrThrow();
		return (Source.GetRotation() + pDevice->GetRotation() + AngleMiddle(m_iMinFireArc, m_iMaxFireArc)) % 360;
		}
	else
		return AngleMiddle(m_iMinFireArc, m_iMaxFireArc);
	}

ICCItem *CWeaponClass::FindItemProperty (CItemCtx &Ctx, const CString &sName)

//	FindItemProperty
//
//	Returns the item property. Subclasses should call this if they do not
//	understand the property.

	{
	//  See if we specified a specific variant. If so, get the ammo for it.

	CString sProperty = sName;
	CItem Ammo;
	int iAmmoIndex = ParseVariantFromPropertyName(sName, &sProperty);
	if (iAmmoIndex != -1 && iAmmoIndex < GetAmmoItemCount())
		Ammo = CItem(GetAmmoItem(iAmmoIndex), 1);

	//	Get the property

	return FindAmmoItemProperty(Ctx, Ammo, sProperty);
	}

Metric CWeaponClass::GetMaxEffectiveRange (CSpaceObject *pSource, const CInstalledDevice *pDevice, CSpaceObject *pTarget) const

//	GetMaxEffectiveRange
//
//	Returns the greatest range at which the weapon is still
//	effective.

	{
	CItemCtx Ctx(pSource, pDevice);
	CWeaponFireDesc *pShot = GetWeaponFireDesc(Ctx);
	if (pShot == NULL)
		return 0.0;

	//	Compute the range based on weapon characteristics.

	Metric rRange;
	if (pTarget && !pTarget->CanThrust())
		{
		Metric rSpeed = pShot->GetAveSpeed();
		rRange = (rSpeed * (pShot->GetMaxLifetime() * 90 / 100)) + (128.0 * g_KlicksPerPixel);
		}
	else
		rRange = pShot->GetEffectiveRange();

	//	If the device slot has range limits, apply them

	if (pDevice)
		{
		if (int iMaxFireRange = pDevice->GetMaxFireRangeLS())
			rRange = Min(rRange, iMaxFireRange * LIGHT_SECOND);
		}

	return rRange;
	}

Metric CWeaponClass::GetMaxRange (CItemCtx &ItemCtx)

//	GetMaxRange
//
//	Returns the greatest range for the weapon.

	{
	CWeaponFireDesc *pShot = GetWeaponFireDesc(ItemCtx);
	if (pShot == NULL)
		return 0.0;

	return pShot->GetMaxRange();
	}

int CWeaponClass::GetPowerRating (CItemCtx &Ctx, int *retiIdlePowerUse) const

//	GetPowerRating
//
//	Returns the rated power

	{
	int iPower = m_iPowerUse;
	int iIdlePower = m_iIdlePowerUse;

	//	If the weapon fire descriptor overrides power use, then use that.

	CWeaponFireDesc *pShot = GetWeaponFireDesc(Ctx);
	if (pShot)
		{
		if (pShot->GetPowerUse() != -1)
			iPower = pShot->GetPowerUse();

		if (pShot->GetIdlePowerUse() != -1)
			iIdlePower = pShot->GetIdlePowerUse();
		}

	//	Adjust if we have an enhancement.

	TSharedPtr<CItemEnhancementStack> pEnhancements = Ctx.GetEnhancementStack();
	if (pEnhancements)
		{
		int iAdj = pEnhancements->GetPowerAdj();
		iPower = iPower * iAdj / 100;
		iIdlePower = iIdlePower * iAdj / 100;
		}

	if (retiIdlePowerUse)
		*retiIdlePowerUse = iIdlePower;

	return iPower;
	}

CString GetReferenceFireRate (int iFireRate)
	{
	if (iFireRate <= 0)
		return NULL_STR;

	return strPatternSubst(CONSTLIT(" @ %s"), CLanguage::ComposeNumber(CLanguage::numberFireRate, iFireRate));
	}

bool CWeaponClass::GetReferenceDamageType (CItemCtx &Ctx, const CItem &Ammo, DamageTypes *retiDamage, CString *retsReference) const

//	GetReferenceDamageType
//
//	Returns the damage type done by the weapon

	{
	const CItem &Item = Ctx.GetItem();
	CSpaceObject *pSource = Ctx.GetSource();
	CInstalledDevice *pDevice = Ctx.GetDevice();
	const CDeviceItem DeviceItem = Item.AsDeviceItem();

	DamageTypes iDamageType;
	CString sReference;

	//	Fire rate

	CString sFireRate = GetReferenceFireRate(CalcActivateDelay(Ctx));

	//	Compute the damage string and special string

	if (IsLauncherWithAmmo() && Ammo.IsEmpty())
		{
		sReference = CONSTLIT("missile launcher");

		sReference.Append(sFireRate);
		iDamageType = damageGeneric;
		}
	else
		{
		//	Get the damage

		int iFragments;
		CWeaponFireDesc *pRootShot = GetWeaponFireDesc(Ctx, Ammo);
		const CWeaponFireDesc *pShot = GetReferenceShotData(pRootShot, &iFragments);
		DamageDesc Damage = pShot->GetDamage();
		iDamageType = Damage.GetDamageType();

		//	Modify the damage based on any enhancements that the ship may have

		TSharedPtr<CItemEnhancementStack> pEnhancements = Ctx.GetEnhancementStack();
		if (pEnhancements)
			Damage.AddEnhancements(pEnhancements);

		//	For shockwaves...

		if (pShot->GetType() == CWeaponFireDesc::ftArea)
			{
			//	Compute total damage. NOTE: For particle weapons the damage 
			//	specified is the total damage if ALL particle were to hit.

			Metric rDamage = SHOCKWAVE_DAMAGE_FACTOR * Damage.GetDamageValue(DamageDesc::flagIncludeBonus);

			//	Calculate the radius of the shockwave

			int iRadius = mathRound((pShot->GetAveExpansionSpeed() * pShot->GetAveLifetime() * g_SecondsPerUpdate) / LIGHT_SECOND);

			//	Compute result

			int iDamage10 = mathRound(rDamage * 10.0);
			int iDamage = iDamage10 / 10;
			int iDamageTenth = iDamage10 % 10;

			if (iDamageTenth == 0)
				sReference = strPatternSubst(CONSTLIT("%s shockwave %d hp in %d ls radius%s"), GetDamageShortName(Damage.GetDamageType()), iDamage, iRadius, sFireRate);
			else
				sReference = strPatternSubst(CONSTLIT("%s shockwave %d.%d hp in %d ls radius%s"), GetDamageShortName(Damage.GetDamageType()), iDamage, iDamageTenth, iRadius, sFireRate);
			}

		//	For area weapons...

		else if (pShot->GetType() == CWeaponFireDesc::ftRadius)
			{
			CString sDamage = Damage.GetDesc(DamageDesc::flagAverageDamage);

			//	Calculate the radius

			int iRadius = mathRound(pShot->GetMaxRadius() / LIGHT_SECOND);

			//	Compute result

			sReference = strPatternSubst(CONSTLIT("%s in %d ls radius%s"), sDamage, iRadius, sFireRate);
			}

		//	For particles...

		else if (pShot->GetType() == CWeaponFireDesc::ftParticles)
			{
			//	Some weapons fire multiple shots (e.g., Avalanche cannon)

			CString sMult;
			int iMult = mathRound(CalcConfigurationMultiplier(pRootShot, false));
			if (iMult != 1)
				sMult = strPatternSubst(CONSTLIT(" (x%d)"), iMult);

			//	Compute total damage. NOTE: For particle weapons the damage 
			//	specified is the total damage if ALL particle were to hit.

			Metric rDamage = PARTICLE_CLOUD_DAMAGE_FACTOR * Damage.GetDamageValue(DamageDesc::flagIncludeBonus);

			//	Compute result

			int iDamage10 = mathRound(rDamage * 10.0);
			int iDamage = iDamage10 / 10;
			int iDamageTenth = iDamage10 % 10;

			if (iDamageTenth == 0)
				sReference = strPatternSubst(CONSTLIT("%s cloud %d hp%s%s"), GetDamageShortName(Damage.GetDamageType()), iDamage, sMult, sFireRate);
			else
				sReference = strPatternSubst(CONSTLIT("%s cloud %d.%d hp%s%s"), GetDamageShortName(Damage.GetDamageType()), iDamage, iDamageTenth, sMult, sFireRate);
			}

		//	For large number of fragments, we have a special description

		else if (iFragments >= 8 && !IsTracking(DeviceItem, pShot))
			{
			//	Compute total damage

			Metric rDamage = EXPECTED_FRAGMENT_HITS * iFragments * Damage.GetDamageValue(DamageDesc::flagIncludeBonus);

			//	Compute radius

			int iRadius = mathRound(pShot->GetRatedSpeed() * pShot->GetAveLifetime() * g_SecondsPerUpdate / LIGHT_SECOND);

			//	Compute result

			int iDamage10 = mathRound(rDamage * 10.0);
			int iDamage = iDamage10 / 10;
			int iDamageTenth = iDamage10 % 10;

			if (iDamageTenth == 0)
				sReference = strPatternSubst(CONSTLIT("%s fragmentation %d hp in %d ls radius%s"), GetDamageShortName(Damage.GetDamageType()), iDamage, iRadius, sFireRate);
			else
				sReference = strPatternSubst(CONSTLIT("%s fragmentation %d.%d hp in %d ls radius%s"), GetDamageShortName(Damage.GetDamageType()), iDamage, iDamageTenth, iRadius, sFireRate);
			}

		//	Otherwise, a normal description

		else
			{
			CString sDamage = Damage.GetDesc(DamageDesc::flagAverageDamage);

			//	Add the multiplier

			int iMult = mathRound(CalcConfigurationMultiplier(pRootShot));
			if (iMult > 1)
				sDamage.Append(strPatternSubst(CONSTLIT(" (x%d)"), iMult));

			sReference.Append(sDamage);

			//	Compute fire rate

			sReference.Append(sFireRate);
			}
		}

	//	Done

	if (retiDamage)
		*retiDamage = iDamageType;

	if (retsReference)
		*retsReference = sReference;

	return true;
	}

const CWeaponFireDesc *CWeaponClass::GetReferenceShotData (const CWeaponFireDesc *pShot, int *retiFragments) const

//	GetReferenceShotData
//
//	If a fragment of pShot does more damage than the shot itself, then
//	return the fragment (since it is more representative)

	{
	const CWeaponFireDesc *pBestShot = pShot;

	//	NOTE: We want fragment damage to take precendence. And we start with
	//	best damage type as generic in case we have generic-damage fragments.

	Metric rBestDamage = 0.0;
	int iBestFragments = 1;
	DamageTypes iBestDamageType = damageGeneric;

	CWeaponFireDesc::SFragmentDesc *pFragDesc = pShot->GetFirstFragment();
	while (pFragDesc)
		{
		int iOriginalFragments = pFragDesc->Count.GetAveValue();
		int iFragments = iOriginalFragments;
		switch (pFragDesc->pDesc->GetType())
			{
			//	Area damage counts as multiple hits

			case CWeaponFireDesc::ftArea:
				iFragments = mathRound(iFragments * SHOCKWAVE_DAMAGE_FACTOR);
				break;

			//	Radius damage always hits (if in range)

			case CWeaponFireDesc::ftRadius:
				break;

			//	For others, only some of the fragments will hit

			default:
				if (!pFragDesc->pDesc->IsTracking())
					iFragments = mathRound(iFragments * SHOCKWAVE_DAMAGE_FACTOR);
				break;
			}

		Metric rDamage = pFragDesc->pDesc->GetDamage().GetDamageValue() * iFragments;
		DamageTypes iDamageType = pFragDesc->pDesc->GetDamage().GetDamageType();
		if (iDamageType > iBestDamageType 
				|| (iDamageType == iBestDamageType && rDamage >= rBestDamage))
			{
			pBestShot = pFragDesc->pDesc;
			rBestDamage = rDamage;
			iBestFragments = iOriginalFragments;
			iBestDamageType = iDamageType;
			}

		pFragDesc = pFragDesc->pNext;
		}

	if (retiFragments)
		*retiFragments = iBestFragments;

	return pBestShot;
	}

CDeviceRotationDesc::ETypes CWeaponClass::GetRotationType (const CDeviceItem &DeviceItem, int *retiMinArc, int *retiMaxArc) const

//	GetRotationType
//
//	Returns information about the weapon's rotation.
//
//	If the weapon is omnidirectional, then we return rotOmnidirectional and 
//	retiMinArc and retiMaxArc are equal to the fire direction of the device.
//
//	If the weapon swivels, then we return rotSwivel and retiMinArc and 
//	retiMaxArc are defined.
//
//	If the weapon is fixed, then we return rotNone and retiMinArc and
//	retiMaxArc are both equal to the fire direction (if the device slot sets 
//	it).

	{
	const CInstalledDevice *pDevice = DeviceItem.GetInstalledDevice();
	const CItemEnhancementStack &Enhancements = DeviceItem.GetEnhancements();
	int iEnhancedFireArc = Enhancements.GetFireArc();

	//	If the device has a fire arc, then we use that for a direction

	int iDeviceMinFireArc = (pDevice ? pDevice->GetMinFireArc() : 0);
	int iDeviceMaxFireArc = (pDevice ? pDevice->GetMaxFireArc() : 0);

	//	If we're stuck, then we're not directional

	if (iEnhancedFireArc == -1)
		{
		if (retiMinArc && retiMaxArc)
			{
			int iFireAngle = AngleMiddle(iDeviceMinFireArc, iDeviceMaxFireArc);
			*retiMinArc = iFireAngle;
			*retiMaxArc = iFireAngle;
			}

		return CDeviceRotationDesc::rotNone;
		}

	//	If the weapon is omnidirectional then we don't need directional 
	//	calculations.

	else if (m_bOmnidirectional || (pDevice && pDevice->IsOmniDirectional()) || iEnhancedFireArc == 360)
		{
		if (retiMinArc && retiMaxArc)
			{
			int iFireAngle = AngleMiddle(iDeviceMinFireArc, iDeviceMaxFireArc);
			*retiMinArc = iFireAngle;
			*retiMaxArc = iFireAngle;
			}

		return CDeviceRotationDesc::rotOmnidirectional;
		}

	//	If we're fixed then we're done

	else if (iEnhancedFireArc == 0 && m_iMinFireArc == m_iMaxFireArc && iDeviceMinFireArc == iDeviceMaxFireArc)
		{
		if (retiMinArc && retiMaxArc)
			{
			int iFireAngle = AngleMiddle(iDeviceMinFireArc, iDeviceMaxFireArc);
			*retiMinArc = iFireAngle;
			*retiMaxArc = iFireAngle;
			}

		return CDeviceRotationDesc::rotNone;
		}

	//	Otherwise, we try to figure out the largest fire arc and use that.

	else
		{
		if (retiMinArc && retiMaxArc)
			{
			int iFireAngle = AngleMiddle(iDeviceMinFireArc, iDeviceMaxFireArc);
			int iFireArc = Max(Max(iEnhancedFireArc, AngleRange(iDeviceMinFireArc, iDeviceMaxFireArc)), AngleRange(m_iMinFireArc, m_iMaxFireArc));
			int iHalfFireArc = iFireArc / 2;

			*retiMinArc = AngleMod(iFireAngle - iHalfFireArc);
			*retiMaxArc = AngleMod(iFireAngle + iHalfFireArc);
			}

		return CDeviceRotationDesc::rotSwivel;
		}
	}

int CWeaponClass::GetSelectVariantCount (void) const

//	GetSelectVariantCount
//
//	Returns the number of selectable variants for this weapon. For example, for
//	missile launchers, this is the number of compatible missiles. This isn't
//	always the same as m_ShotData.GetCount().

	{
	switch (m_iVariantType)
		{
		case varLauncher:
			return m_ShotData.GetCount();

		default:
			return (m_ShotData.GetCount() > 0 ? 1 : 0);
		}
	}

void CWeaponClass::GetSelectedVariantInfo (const CSpaceObject *pSource, 
										   const CInstalledDevice *pDevice,
										   CString *retsLabel,
										   int *retiAmmoLeft,
										   CItemType **retpType,
										   bool bUseCustomAmmoCountHandler)

//	GetSelectedVariantInfo
//
//	Returns information about the selected variant

	{
	CItemCtx Ctx(pSource, pDevice);
	CWeaponFireDesc *pShot = GetWeaponFireDesc(Ctx);

	//  Short-circuit if we don't have enough data

	if (pShot == NULL || pSource == NULL || pDevice == NULL)
		{
		if (retsLabel)
			*retsLabel = CString();
		if (retiAmmoLeft)
			*retiAmmoLeft = 0;
		if (retpType)
			*retpType = NULL;
		}

	//  If we use ammo, return that

	else if (pShot->GetAmmoType())
		{
		CItem Item(pShot->GetAmmoType(), 1);

		//	Calc ammo left

		if (retiAmmoLeft)
			{
			//	See if we use an event

			if (bUseCustomAmmoCountHandler && FireGetAmmoCountToDisplay(Ctx.GetDeviceItem(), *pShot, retiAmmoLeft))
				{ }

			//	If each ammo item uses charges, then we need a different method.

			else if (pShot->GetAmmoType()->AreChargesAmmo())
				{
				int iCharges = 0;
				CItemListManipulator ItemList(const_cast<CSpaceObject *>(pSource)->GetItemList());
				while (ItemList.MoveCursorForward())
					{
					const CItem &Item = ItemList.GetItemAtCursor();
					if (Item.GetType() == pShot->GetAmmoType())
						iCharges += Item.GetCount() * Item.GetCharges();
					}

				*retiAmmoLeft = iCharges;
				}

			//	Otherwise, we just return the number of items

			else
				{
				CItemListManipulator ItemList(const_cast<CSpaceObject *>(pSource)->GetItemList());
				CItem Item(pShot->GetAmmoType(), 1);
				if (ItemList.SetCursorAtItem(Item, CItem::FLAG_IGNORE_CHARGES))
					*retiAmmoLeft = ItemList.GetItemAtCursor().GetCount();
				else
					*retiAmmoLeft = 0;
				}
			}

		if (retsLabel)
			{
			if (GetCategory() == itemcatLauncher)
				*retsLabel = Item.GetNounPhrase(nounCapitalize);
			else
				*retsLabel = CString();
			}

		if (retpType)
			*retpType = pShot->GetAmmoType();
		}

	//	Else if we use charges, return that

	else if (m_bCharges)
		{
		if (retsLabel)
			*retsLabel = CString();

		if (retiAmmoLeft)
			{
			if (bUseCustomAmmoCountHandler && FireGetAmmoCountToDisplay(Ctx.GetDeviceItem(), *pShot, retiAmmoLeft))
				{ }
			else
				*retiAmmoLeft = pDevice->GetCharges(pSource);
			}

		if (retpType)
			*retpType = GetItemType();
		}

	//	Otherwise, we don't use ammo

	else
		{
		if (retsLabel)
			*retsLabel = CString();

		if (retiAmmoLeft)
			{
			if (bUseCustomAmmoCountHandler && FireGetAmmoCountToDisplay(Ctx.GetDeviceItem(), *pShot, retiAmmoLeft))
				{ }
			else
				*retiAmmoLeft = -1;
			}

		if (retpType)
			*retpType = GetItemType();
		}
	}

Metric CWeaponClass::GetShotSpeed (CItemCtx &Ctx) const

//	GetShotSpeed
//
//	Returns the average initial speed of the shot.

	{
	CWeaponFireDesc *pShot = GetWeaponFireDesc(Ctx);
	if (pShot == NULL)
		return 0.0;

	return pShot->GetAveInitialSpeed();
	}

int CWeaponClass::GetStdDamage (int iLevel)

//	GetStdDamage
//
//	Returns standard damage at this level.

	{
	return GetStdStats(iLevel).iDamage;
	}

Metric CWeaponClass::GetStdDamage180 (int iLevel)

//	GetStdDamage180
//
//	Returns the standard weapon damage at this level.

	{
	return GetStdStats(iLevel).iDamage * (180.0 / STD_FIRE_DELAY_TICKS);
	}

const CWeaponClass::SStdStats &CWeaponClass::GetStdStats (int iLevel)

//  GetStdStats
//
//  Returns standard stats for level.

	{
	if (iLevel >= 1 && iLevel <= MAX_ITEM_LEVEL)
		return STD_WEAPON_STATS[iLevel - 1];
	else
		{
		ASSERT(false);
		return STD_WEAPON_STATS[0];
		}
	}

int CWeaponClass::GetValidVariantCount (CSpaceObject *pSource, CInstalledDevice *pDevice)

//	GetValidVariantCount
//
//	Returns the number of valid variants for this weapons

	{
	//	Special handling for scalable weapons

	if (m_iVariantType == varLevelScaling || m_iVariantType == varCharges || m_iVariantType == varCounter)
		{
		CItemCtx ItemCtx(pSource, pDevice);
		CWeaponFireDesc *pShot = GetWeaponFireDesc(ItemCtx);
		if (pShot && VariantIsValid(pSource, pDevice, *pShot))
			return 1;
		else
			return 0;
		}

	//	Otherwise, count them

	else
		{
		int iCount = 0;

		for (int i = 0; i < m_ShotData.GetCount(); i++)
			{
			if (VariantIsValid(pSource, pDevice, *m_ShotData[i].pDesc))
				iCount++;
			}

		return iCount;
		}
	}

int CWeaponClass::GetWeaponEffectiveness (const CDeviceItem &DeviceItem, CSpaceObject *pTarget) const

//	GetWeaponEffectiveness
//
//	Returns:
//
//	< 0		If the weapon is ineffective against the target
//	0		If the weapon has normal effect on target
//	1-100	If the weapon is particularly effective against the target
//
//	This call is used to figure out whether we should use an EMP or blinder
//	cannon against the target.

	{
	int iScore = 0;

	CSpaceObject *pSource = DeviceItem.GetSource();
	const CWeaponFireDesc *pShot = GetWeaponFireDesc(DeviceItem);
	if (pShot == NULL)
		return -100;

	//	If we don't enough ammo, clearly we will not be effective

	if (pSource && pShot->GetAmmoType())
		{
		CItemListManipulator ItemList(pSource->GetItemList());
		CItem Item(pShot->GetAmmoType(), 1);
		if (!ItemList.SetCursorAtItem(Item))
			return -100;
		}

	//	Check some targeting options

	if (m_bTargetStationsOnly 
			&& (pTarget == NULL || pTarget->GetCategory() != CSpaceObject::catStation))
		return -100;

	//	If the target is a missile, then we need to be able to hit missiles.

	if (pTarget && pTarget->GetCategory() == CSpaceObject::catMissile)
		{
		if (!(DeviceItem.GetTargetTypes() & (CTargetList::typeMissile | CTargetList::typeTargetableMissile)))
			return -100;
		}

	//	Check our state

	if (const CInstalledDevice *pDevice = DeviceItem.GetInstalledDevice())
		{
		switch (m_Counter)
			{
			//	If we're overheating, we will not be effective

			case cntTemperature:
				if (pDevice->IsWaiting() && pDevice->GetTemperature() > 0)
					return -100;

				if (pDevice->GetTemperature() + m_iCounterActivate >= MAX_COUNTER)
					{
					pDevice->SetWaiting(true);
					return -100;
					}

				pDevice->SetWaiting(false);
				break;

			//	If our capacitor is discharged, we will not be effective

			case cntCapacitor:
				if (pDevice->IsWaiting() && pDevice->GetTemperature() < MAX_COUNTER)
					return -100;

				if (pDevice->GetTemperature() < m_iCounterActivate)
					{
					pDevice->SetWaiting(true);
					return -100;
					}

				pDevice->SetWaiting(false);
				break;
			}
		}

	//  If we're blind and this weapon doesn't have the canFireWhenBlind attribute, then
	//  weapon is not effective.

	if (pSource && pSource->IsBlind() && (!m_bCanFireWhenBlind))
		{
		return -100;
		}

	//	If the weapon has EMP damage and the target has no shields and is not paralysed then
	//	this is very effective.

	if (pShot->GetDamage().GetEMPDamage() > 0 && pTarget && !pTarget->GetDeviceSystem().HasShieldsUp())
		{
		//	If the target is already paralyzed, or if the target is immune
		//	or is a station (which cannot be paralyzed) then don't use this weapon.

		if (pTarget->IsParalyzed() 
				|| pTarget->GetCategory() != CSpaceObject::catShip
				|| pTarget->IsImmuneTo(CConditionSet::cndParalyzed))
			return -100;

		iScore += 100;
		}

	//	If the weapon has blinding damage and the target is not blind then
	//	this is very effective

	if (pShot->GetDamage().GetBlindingDamage() > 0 && pTarget && !pTarget->GetDeviceSystem().HasShieldsUp())
		{
		//	If the target is already blind, or if the target is immune or is
		//	a station, then don't bother with this weapon.

		if (pTarget->IsBlind()
				|| pTarget->GetCategory() != CSpaceObject::catShip
				|| pTarget->IsImmuneTo(CConditionSet::cndBlind))
			return -100;

		iScore += 100;
		}

	//	Score

	return iScore;
	}

const CWeaponFireDesc *CWeaponClass::GetWeaponFireDesc (const CDeviceItem &DeviceItem, const CItem &Ammo) const

//  GetWeaponFireDesc
//
//  Get weapon fire descriptor for the weapon when shooting the given ammo.
//  ItemCtx may optionally refer to the installed weapon (it must not contain
//  a different item, though it may be empty).
//
//  Ammo may be specified either through the ammo item or through ItemCtx
//  (as an installed device selection).
//
//	NOTE: If Ammo is passed in, we expect it to be fired by this weapon. Check
//	before calling if not sure.

	{
	DEBUG_TRY

	if (m_ShotData.GetCount() == 0)
		return NULL;

	//	Handle scalable levels

	else if (m_iVariantType == varLevelScaling)
		{
		//	We assume that all levels are represented in m_ShotData.

		int iIndex = Min(Max(0, DeviceItem.GetLevel() - m_ShotData[0].pDesc->GetLevel()), m_ShotData.GetCount() - 1);
		return m_ShotData[iIndex].pDesc;
		}

	//	Handle counter variants

	else if (m_iVariantType == varCounter)
		{
		//	We assume that all charge values are represented in m_ShotData.

		int iIndex = Min(Max(0, DeviceItem.GetVariantNumber()), m_ShotData.GetCount() - 1);
		return m_ShotData[iIndex].pDesc;
		}

	//	Handle charge variants

	else if (m_iVariantType == varCharges)
		{
		//	We assume that all charge values are represented in m_ShotData.

		int iIndex = Min(Max(0, DeviceItem.GetCharges()), m_ShotData.GetCount() - 1);
		return m_ShotData[iIndex].pDesc;
		}

	//  If we need ammo, then we have extra work to do.
	//  NOTE: Currently, if one variant uses ammo, all need to use ammo.
	//	NOTE 2: This only applies to launchers. By definition, non-launchers
	//	never have more than one type of ammo. [But some launchers do not
	//	have ammo, so we need to check that they use ammo.]

	else if (IsLauncherWithAmmo())
		{
		CWeaponFireDesc *pRoot = NULL;
		int iLevel = -1;

		//  If we have ammo, use it (this overrides whatever item is selected 
		//  in ItemCtx).

		if (!Ammo.IsEmpty())
			{
			int iSelection = GetAmmoVariant(Ammo.GetType());
			if (iSelection != -1)
				pRoot = m_ShotData[iSelection].pDesc;
			else
				return NULL;
			}

		//  If we have a device, we ask it for the variant
		//
		//  NOTE: We need to make sure we check that the source exists because
		//  some code (like CStationType::OnBindDesign) will call this with a
		//  valid CInstalledDevice but a NULL source.

		else if (const CInstalledDevice *pDevice = DeviceItem.GetInstalledDevice())
			{
			int iSelection = GetCurrentVariant(pDevice);
			if (iSelection != -1 && iSelection < m_ShotData.GetCount())
				pRoot = m_ShotData[iSelection].pDesc;
			else
				return NULL;
			}

		//  Otherwise, just return the first entry

		else
			pRoot = m_ShotData[0].pDesc;

		//  For now, the scaling for ammo weapons always comes from the weapon
		//  (we can't have scalable ammo).

		iLevel = DeviceItem.GetLevel();
		return (iLevel == 0 ? pRoot : pRoot->GetScaledDesc(iLevel));
		}
	else
		{
		CWeaponFireDesc *pRoot = m_ShotData[0].pDesc;
		int iLevel = DeviceItem.GetLevel();
		return (iLevel == 0 ? pRoot : pRoot->GetScaledDesc(iLevel));
		}

	DEBUG_CATCH
	}

CWeaponFireDesc *CWeaponClass::GetWeaponFireDesc (CItemCtx &ItemCtx, const CItem &Ammo) const

//  GetWeaponFireDesc
//
//  Get weapon fire descriptor for the weapon when shooting the given ammo.
//  ItemCtx may optionally refer to the installed weapon (it must not contain
//  a different item, though it may be empty).
//
//  Ammo may be specified either through the ammo item or through ItemCtx
//  (as an installed device selection).
//
//	NOTE: If Ammo is passed in, we expect it to be fired by this weapon. Check
//	before calling if not sure.

	{
	DEBUG_TRY

	if (m_ShotData.GetCount() == 0)
		return NULL;

	//	Handle scalable levels

	if (m_iVariantType == varLevelScaling)
		{
		//	We assume that all levels are represented in m_ShotData.

		int iIndex = (ItemCtx.GetItem().IsEmpty() ? 0 : Min(Max(0, ItemCtx.GetItem().GetLevel() - m_ShotData[0].pDesc->GetLevel()), m_ShotData.GetCount() - 1));
		return m_ShotData[iIndex].pDesc;
		}

	//	Handle counter variants

	else if (m_iVariantType == varCounter)
		{
		//	We assume that all charge values are represented in m_ShotData.

		int iIndex = Min(Max(0, ItemCtx.GetItemVariantNumber()), m_ShotData.GetCount() - 1);
		return m_ShotData[iIndex].pDesc;
		}

	//	Handle charge variants

	else if (m_iVariantType == varCharges)
		{
		//	We assume that all charge values are represented in m_ShotData.

		int iIndex = Min(Max(0, ItemCtx.GetItemCharges()), m_ShotData.GetCount() - 1);
		return m_ShotData[iIndex].pDesc;
		}

	//  If we need ammo, then we have extra work to do.
	//  NOTE: Currently, if one variant uses ammo, all need to use ammo.
	//	NOTE 2: This only applies to launchers. By definition, non-launchers
	//	never have more than one type of ammo. [But some launchers do not
	//	have ammo, so we need to check that they use ammo.]

	else if (IsLauncherWithAmmo())
		{
		CWeaponFireDesc *pRoot = NULL;
		int iLevel = -1;

		//  If we have ammo, use it (this overrides whatever item is selected 
		//  in ItemCtx).

		if (!Ammo.IsEmpty())
			{
			int iSelection = GetAmmoVariant(Ammo.GetType());
			if (iSelection != -1)
				pRoot = m_ShotData[iSelection].pDesc;
			else
				return NULL;
			}

		//  If we have a device, we ask it for the variant
		//
		//  NOTE: We need to make sure we check that the source exists because
		//  some code (like CStationType::OnBindDesign) will call this with a
		//  valid CInstalledDevice but a NULL source.

		else if (ItemCtx.GetDevice() && ItemCtx.GetSource())
			{
			int iSelection = GetCurrentVariant(ItemCtx.GetDevice());
			if (iSelection != -1 && iSelection < m_ShotData.GetCount())
				pRoot = m_ShotData[iSelection].pDesc;
			else
				return NULL;
			}

#if 0
		else if (!ItemCtx.GetVariantItem().IsEmpty())
			{
			int iSelection = GetAmmoVariant(ItemCtx.GetVariantItem().GetType());
			if (iSelection != -1)
				pRoot = m_ShotData[iSelection].pDesc;
			else
				return NULL;
			}
#endif

		else if (ItemCtx.GetVariant() != -1 && ItemCtx.GetVariant() < m_ShotData.GetCount())
			{
			pRoot = m_ShotData[ItemCtx.GetVariant()].pDesc;
			}

		//  Otherwise, just return the first entry

		else
			pRoot = m_ShotData[0].pDesc;

		//  For now, the scaling for ammo weapons always comes from the weapon
		//  (we can't have scalable ammo).

		iLevel = (ItemCtx.GetItem().IsEmpty() ? -1 : ItemCtx.GetItem().GetLevel());
		return (iLevel == -1 ? pRoot : pRoot->GetScaledDesc(iLevel));
		}
	else
		{
		CWeaponFireDesc *pRoot = m_ShotData[0].pDesc;
		int iLevel = (ItemCtx.GetItem().IsEmpty() ? -1 : ItemCtx.GetItem().GetLevel());
		return (iLevel == -1 ? pRoot : pRoot->GetScaledDesc(iLevel));
		}

	DEBUG_CATCH
	}

const CWeaponFireDesc &CWeaponClass::GetWeaponFireDescForVariant (const CDeviceItem &DeviceItem, int iVariant) const

//	GetWeaponFireDescForVariant
//
//	Returns the fire descriptor for the given variant.

	{
	DEBUG_TRY

	if (iVariant < 0 || iVariant >= m_ShotData.GetCount())
		{
		ASSERT(false);
		return CWeaponFireDesc::Null();
		}

	else if (m_iVariantType == varLevelScaling)
		{
		if (iVariant != 0)
			return CWeaponFireDesc::Null();

		//	We assume that all levels are represented in m_ShotData.

		int iIndex = Min(Max(0, DeviceItem.GetLevel() - m_ShotData[0].pDesc->GetLevel()), m_ShotData.GetCount() - 1);
		return *m_ShotData[iIndex].pDesc;
		}

	else if (m_iVariantType == varCounter
			|| m_iVariantType == varCharges)
		{
		return *m_ShotData[iVariant].pDesc;
		}

	//  If we need ammo, then we have extra work to do.
	//  NOTE: Currently, if one variant uses ammo, all need to use ammo.
	//	NOTE 2: This only applies to launchers. By definition, non-launchers
	//	never have more than one type of ammo. [But some launchers do not
	//	have ammo, so we need to check that they use ammo.]

	else if (IsLauncherWithAmmo())
		{
		const CWeaponFireDesc *pRoot = m_ShotData[iVariant].pDesc;
		int iLevel = -1;

		//  For now, the scaling for ammo weapons always comes from the weapon
		//  (we can't have scalable ammo).

		iLevel = DeviceItem.GetLevel();
		return *(iLevel == 0 ? pRoot : pRoot->GetScaledDesc(iLevel));
		}
	else
		{
		const CWeaponFireDesc *pRoot = m_ShotData[0].pDesc;
		int iLevel = DeviceItem.GetLevel();
		return *(iLevel == 0 ? pRoot : pRoot->GetScaledDesc(iLevel));
		}

	DEBUG_CATCH
	}

int CWeaponClass::GetWeaponVariantCount (const CDeviceItem &DeviceItem) const

//	GetWeaponVariantCount
//
//	Returns the total number of weapon variants. Some of these variants might 
//	not be valid (e.g., because we don't have ammo).

	{
	switch (m_iVariantType)
		{
		//	Level scaling weapons only have one variant.

		case varLevelScaling:
			return (m_ShotData.GetCount() > 0 ? 1 : 0);

		//	Otherwise...

		default:
			return m_ShotData.GetCount();
		}
	}

ALERROR CWeaponClass::InitVariantsFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType)

//	InitVariantsFromXML
//
//	Loads the variant descriptions, whether from missiles, scaling, or charges.

	{
	int i;
	ALERROR error;

	//	If we have a Missiles tag then this weapon has ammunition; otherwise,
	//	if we have a Variants tag, then we have multiple descriptors.

	CXMLElement *pVariants = pDesc->GetContentElementByTag(MISSILES_TAG);
	if (pVariants == NULL)
		pVariants = pDesc->GetContentElementByTag(VARIANTS_TAG);

	if (pVariants)
		{
		//	Get the type of variants

		CString sType = pVariants->GetAttribute(TYPE_ATTRIB);
		if (sType.IsBlank())
			{
			if (pVariants->GetContentElementCount() > 1
					|| GetDefinedSlotCategory() == itemcatLauncher
					|| pDesc->GetAttributeBool(LAUNCHER_ATTRIB))
				m_iVariantType = varLauncher;
			else
				m_iVariantType = varSingle;
			}
		else if (strEquals(sType, VARIANT_TYPE_CHARGES))
			m_iVariantType = varCharges;
		else if (strEquals(sType, VARIANT_TYPE_MISSILES))
			m_iVariantType = varLauncher;
		else if (strEquals(sType, VARIANT_TYPE_LEVELS))
			m_iVariantType = varLevelScaling;
		else if (strEquals(sType, VARIANT_TYPE_COUNTER))
			m_iVariantType = varCounter;
		else
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Unknown variant type: %s"), sType);
			return ERR_FAIL;
			}

		//	Load each variant

		m_ShotData.InsertEmpty(pVariants->GetContentElementCount());
		for (i = 0; i < m_ShotData.GetCount(); i++)
			{
			CXMLElement *pItem = pVariants->GetContentElement(i);

			//	If this entry defines the missile, then we own it.

			if (pItem->FindAttribute(TYPE_ATTRIB))
				{
				m_ShotData[i].bOwned = true;
				m_ShotData[i].pDesc = new CWeaponFireDesc;

				//	For scalable levels, we assume each entry is a level

				int iLevel = GetLevel();
				if (m_iVariantType == varLevelScaling)
					iLevel += i;

				//	Initialize

				CWeaponFireDesc::SInitOptions Options;
				Options.sUNID = strPatternSubst(CONSTLIT("%d/%d"), GetUNID(), i);
				Options.iLevel = iLevel;

				if (error = m_ShotData[i].pDesc->InitFromXML(Ctx, pItem, Options))
					return error;
				}

			//	Otherwise we expect the definition to be in the ItemType.
			//	This is best for missiles.

			else
				{
				//	In this case we need to load the ammo type because it we 
				//  will later load the missile (at bind time).

				if (error = m_ShotData[i].pAmmoType.LoadUNID(Ctx, pItem->GetAttribute(AMMO_ID_ATTRIB)))
					return error;

				//	AmmoID is required in this case.

				if (m_ShotData[i].pAmmoType.GetUNID() == 0)
					{
					Ctx.sError = CONSTLIT("Expected either type= or ammoID= definition.");
					return ERR_FAIL;
					}

				m_ShotData[i].bOwned = false;
				m_ShotData[i].pDesc = NULL;
				}
			}
		}
	else
		{
		//	Figure out the type of variants

		if (pType->IsScalable())
			m_iVariantType = varSingleLevelScaling;

		else if (GetDefinedSlotCategory() == itemcatLauncher
				|| pDesc->GetAttributeBool(LAUNCHER_ATTRIB))
			m_iVariantType = varLauncher;

		else
			m_iVariantType = varSingle;

		//	Add a single entry

		CWeaponFireDesc::SInitOptions Options;
		Options.sUNID = strPatternSubst(CONSTLIT("%d/0"), GetUNID());
		Options.iLevel = GetLevel();

		m_ShotData.InsertEmpty(1);
		m_ShotData[0].bOwned = true;
		m_ShotData[0].pDesc = new CWeaponFireDesc;

		//	Load the definition from the root element

		if (error = m_ShotData[0].pDesc->InitFromXML(Ctx, pDesc, Options))
			return error;

		//  Initialize scaled stats, if necessary

		if (pType->IsScalable())
			{
			if (error = m_ShotData[0].pDesc->InitScaledStats(Ctx, pDesc, pType, this))
				return error;
			}
		}

	//	Done

	return NOERROR;
	}

bool CWeaponClass::IsAmmoWeapon (void)

//	IsAmmoWeapon
//
//	Returns TRUE if weapon uses ammo or other expendables

	{
	return (UsesAmmo() || m_bCharges);
	}

bool CWeaponClass::IsAreaWeapon (const CDeviceItem &DeviceItem) const

//	IsAreaWeapon
//
//	Is this a weapon with an area of effect

	{
	const CWeaponFireDesc *pShot = GetWeaponFireDesc(DeviceItem);
	if (pShot == NULL)
		return false;

	if (pShot->GetType() == CWeaponFireDesc::ftArea)
		return true;

	if (pShot->HasFragments() && pShot->GetFirstFragment()->pDesc->GetType() == CWeaponFireDesc::ftArea)
		return true;

	return false;
	}

bool CWeaponClass::IsFirstVariantSelected(CSpaceObject *pSource, CInstalledDevice *pDevice)

//	IsVariantSelected
//
//	Returns TRUE if we've selected some variant (i.e., we haven't selected 0xffff)
//  TODO: Make sure that this is the first variant WITH AVAILABLE AMMO that is selected.
//  We can do this by making sure that the previous variant has an ID greater than the current one.

	{
	bool bFirstVariantSelected = true;
	for (int iTry = GetCurrentVariant(pDevice) - 1; iTry >= 0; iTry--)
		{
		//  If any variant with lower ID than our current one is valid, then the first variant is NOT currently selected.
		bFirstVariantSelected = bFirstVariantSelected && !(VariantIsValid(pSource, pDevice, *m_ShotData[iTry].pDesc));
		}
	return ((GetCurrentVariant(pDevice) == -1) || bFirstVariantSelected);
	}

bool CWeaponClass::IsLastVariantSelected(CSpaceObject *pSource, CInstalledDevice *pDevice)

//	IsVariantSelected
//
//	Returns TRUE if we've selected some variant (i.e., we haven't selected 0xffff)
//  TODO: Make sure that this is the last variant WITH AVAILABLE AMMO that is selected.
//  We can do this by making sure that the next variant has an ID less than the current one.

	{
	bool bLastVariantSelected = true;
	for (int iTry = GetCurrentVariant(pDevice) + 1; iTry < m_ShotData.GetCount(); iTry++)
		{
		//  If any variant with higher ID than our current one is valid, then the last variant is NOT currently selected.
		bLastVariantSelected = bLastVariantSelected && !(VariantIsValid(pSource, pDevice, *m_ShotData[iTry].pDesc));
		}
	return ((GetCurrentVariant(pDevice) == -1) || bLastVariantSelected);
	}

bool CWeaponClass::IsStdDamageType (DamageTypes iDamageType, int iLevel)

//	IsStdDamageType
//
//	Returns TRUE if the given damage type is standard at the given level.

	{
	int iTierLevel = ::GetDamageTypeLevel(iDamageType);

	return (iLevel >= iTierLevel && iLevel < iTierLevel + 3);
	}

bool CWeaponClass::IsTargetReachable (const CInstalledDevice &Device, CSpaceObject &Target, int iDefaultFireAngle, int *retiFireAngle) const

//	IsTargetReachable
//
//	Returns TRUE if we can hit the given target. If we return TRUE, 
//	retiFireAngle is initialized with the weapon's best fire angle to hit the
//	target.

	{
	const CWeaponFireDesc *pShotDesc = GetWeaponFireDesc(Device);
	const CSpaceObject *pSource = Device.GetSource();
	if (pShotDesc == NULL || pSource == NULL)
		return false;

	//	Get rotation info

	int iMinFireArc, iMaxFireArc;
	CDeviceRotationDesc::ETypes iType = GetRotationType(Device, &iMinFireArc, &iMaxFireArc);

	//	Compute the fire solution. NOTE: iAim and rDist are always computed, 
	//	even if we don't have a firing solution.

	int iAim;
	Metric rDist;
	bool bNoFireSolution = !CalcFireSolution(Device, Target, &iAim, &rDist);

	//	Figure out how close we can get

	int iFireAngle = CalcReachableFireAngle(Device, iAim, iDefaultFireAngle);
	if (retiFireAngle)
		*retiFireAngle = iFireAngle;

	//	Area weapons are always aligned (we try even if we don't have a
	//	firing solution).

	if (pShotDesc->GetType() == CWeaponFireDesc::ftArea)
		return true;

	//	If we get this far and we don't have a firing solution, then we cannot
	//	reach the target.

	else if (bNoFireSolution)
		return false;

	//	Omnidirectional weapons are always aligned.

	else if (iType == CDeviceRotationDesc::rotOmnidirectional)
		return true;

	//	Tracking weapons are always aligned.

	else if (IsTracking(Device, pShotDesc))
		return true;

	//	Figure out our aim tolerance

	else
		{
		int iAimTolerance = GetConfiguration(*pShotDesc).GetAimTolerance(GetFireDelay(*pShotDesc));

		//	Compute the angular size of the target

		int iHalfAngularSize = (int)(20.0 * Target.GetHitSize() / Max(1.0, rDist));

		//	Figure out how far off we are from the direction that we
		//	want to fire in.

		int iAimOffset = AngleOffset(iFireAngle, iAim);

		//	If we're facing in the direction that we want to fire, 
		//	then we're aligned...

		return (iAimOffset <= Max(iAimTolerance, iHalfAngularSize));
		}
	}

bool CWeaponClass::IsTracking (const CDeviceItem &DeviceItem, const CWeaponFireDesc *pShot) const

//	IsTracking
//
//	Returns TRUE if we're a tracking weapon.

	{
	if (pShot == NULL)
		return false;

	if (pShot->IsTrackingOrHasTrackingFragments())
		return true;

	const CItemEnhancementStack &Enhancements = DeviceItem.GetEnhancements();
	if (Enhancements.IsTracking())
		return true;

	return false;
	}

bool CWeaponClass::IsTrackingWeapon (const CDeviceItem &DeviceItem) const

//	IsTrackingWeapon
//
//	Returns TRUE if we're a tracking weapon
	
	{
	return IsTracking(DeviceItem, GetWeaponFireDesc(DeviceItem));
	}

bool CWeaponClass::IsVariantSelected (CSpaceObject *pSource, CInstalledDevice *pDevice)

//	IsVariantSelected
//
//	Returns TRUE if we've selected some variant (i.e., we haven't selected 0xffff)

	{
	return (GetCurrentVariant(pDevice) != -1);
	}

bool CWeaponClass::IsWeaponAligned (CSpaceObject *pShip, 
									const CInstalledDevice *pDevice, 
									CSpaceObject *pTarget, 
									int *retiAimAngle, 
									int *retiFireAngle) const

//	IsWeaponAligned
//
//	Return TRUE if weapon is aligned on target.
//
//	Note: If the weapon is invalid, we return an aim angle of -1

	{
	CItemCtx Ctx(pShip, pDevice);
	const CDeviceItem DeviceItem = Ctx.GetDeviceItem();

	const CWeaponFireDesc *pShot = GetWeaponFireDesc(DeviceItem);
	if (pShot == NULL || pShip == NULL || pDevice == NULL)
		{
		if (retiAimAngle) *retiAimAngle = -1;
		if (retiFireAngle) *retiFireAngle = -1;
		return false;
		}

	ASSERT(pTarget);
	if (!pTarget)
		return false;

	//	Get rotation info

	int iMinFireArc, iMaxFireArc;
	CDeviceRotationDesc::ETypes iType = GetRotationType(DeviceItem, &iMinFireArc, &iMaxFireArc);

	int iFacingAngle = AngleMod(pShip->GetRotation() + AngleMiddle(iMinFireArc, iMaxFireArc));

	//	Compute the fire solution

	int iAim;
	Metric rDist;
	if (!CalcFireSolution(*pDevice, *pTarget, &iAim, &rDist))
		{
		if (retiAimAngle) *retiAimAngle = iAim;
		if (retiFireAngle) *retiFireAngle = iFacingAngle;
		return false;
		}

	if (retiAimAngle)
		*retiAimAngle = iAim;

	//	Omnidirectional weapons are always aligned

	if (iType == CDeviceRotationDesc::rotOmnidirectional)
		{
		if (retiFireAngle)
			*retiFireAngle = iAim;
		return true;
		}

	//	Area weapons are always aligned

	if (pShot->GetType() == CWeaponFireDesc::ftArea)
		{
		if (retiFireAngle)
			*retiFireAngle = iFacingAngle;
		return true;
		}

	//	Figure out our aim tolerance

	int iAimTolerance = GetConfiguration(*pShot).GetAimTolerance(GetFireDelay(*pShot));

	//	Tracking weapons behave like directional weapons with 120 degree field

	if (iType != CDeviceRotationDesc::rotSwivel && IsTracking(DeviceItem, pShot))
		{
		int iDeviceAngle = AngleMiddle(iMinFireArc, iMaxFireArc);
		iMinFireArc = AngleMod(iDeviceAngle - 60);
		iMaxFireArc = AngleMod(iDeviceAngle + 60);
		iType = CDeviceRotationDesc::rotSwivel;
		}

	if (iType == CDeviceRotationDesc::rotSwivel)
		{
		int iMin = AngleMod(pShip->GetRotation() + iMinFireArc - iAimTolerance);
		int iMax = AngleMod(pShip->GetRotation() + iMaxFireArc + iAimTolerance);

		//	Are we in the fire arc?

		bool bInArc;
		if (iMin < iMax)
			{
			//	e.g., iMin == 0; iMax == 180

			bInArc = (iAim >= iMin && iAim <= iMax);
			}
		else
			{
			//	e.g., iMin == 315; iMax == 45

			bInArc = (iAim >= iMin || iAim <= iMax);
			}

		//	Compute the fire direction

		if (retiFireAngle)
			{
			if (bInArc)
				*retiFireAngle = iAim;
			else if (AngleOffset(iAim, iMin) < AngleOffset(iAim, iMax))
				*retiFireAngle = iMin;
			else
				*retiFireAngle = iMax;
			}

		return bInArc;
		}

	//	Fire angle

	if (retiFireAngle)
		*retiFireAngle = iFacingAngle;

	//	Compute the angular size of the target

	int iHalfAngularSize = (int)(20.0 * pTarget->GetHitSize() / Max(1.0, rDist));

	//	Figure out how far off we are from the direction that we
	//	want to fire in.

	int iAimOffset = AngleOffset(iFacingAngle, iAim);

	//	If we're facing in the direction that we want to fire, 
	//	then we're aligned...

	if (iAimOffset <= Max(iAimTolerance, iHalfAngularSize))
		return true;

	//	Otherwise, we're not and we need to return the aim direction

	else
		return false;
	}

bool CWeaponClass::IsWeaponVariantValid (const CDeviceItem &DeviceItem, int iVariant) const

//	IsWeaponVariantValid
//
//	Returns TRUE if the given variant is valid. If the device is not installed,
//	then all variants are valid.

	{
	const CSpaceObject *pSource = DeviceItem.GetSource();
	if (pSource == NULL)
		return (iVariant >= 0 && iVariant < GetWeaponVariantCount(DeviceItem));

	const CInstalledDevice *pDevice = DeviceItem.GetInstalledDevice();
	if (pDevice == NULL)
		return (iVariant >= 0 && iVariant < GetWeaponVariantCount(DeviceItem));

	const CWeaponFireDesc &ShotDesc = GetWeaponFireDescForVariant(DeviceItem, iVariant);
	if (!ShotDesc)
		return false;

	return VariantIsValid(pSource, pDevice, ShotDesc);
	}

bool CWeaponClass::NeedsAutoTarget (const CDeviceItem &DeviceItem, int *retiMinFireArc, int *retiMaxFireArc) const

//	NeedsAutoTarget
//
//	Returns TRUE if this weapon is either a swivel weapon, an omniweapon, or a
//	tracking weapon.
//
//	If necessary we return the (absolute) fire arc where we should look for 
//	targets (if we're a swivel weapon).

	{
	//	If we're a tracking weapon then we have no swivel restrictions

	if (IsTrackingWeapon(DeviceItem))
		{
		if (retiMinFireArc) *retiMinFireArc = 0;
		if (retiMaxFireArc) *retiMaxFireArc = 0;
		return true;
		}

	//	If we're an omni or swivel weapon, adjust our fire arc

	int iMinFireArc, iMaxFireArc;
	switch (GetRotationType(DeviceItem, &iMinFireArc, &iMaxFireArc))
		{
		case CDeviceRotationDesc::rotOmnidirectional:
			{
			if (retiMinFireArc) *retiMinFireArc = 0;
			if (retiMaxFireArc) *retiMaxFireArc = 0;
			return true;
			}

		case CDeviceRotationDesc::rotSwivel:
			{
			if (const CSpaceObject *pSource = DeviceItem.GetSource())
				{
				iMinFireArc = AngleMod(pSource->GetRotation() + iMinFireArc);
				iMaxFireArc = AngleMod(pSource->GetRotation() + iMaxFireArc);
				}
			if (retiMinFireArc) *retiMinFireArc = iMinFireArc;
			if (retiMaxFireArc) *retiMaxFireArc = iMaxFireArc;
			return true;
			}

		default:
			return false;
		}
	}

void CWeaponClass::OnAccumulateAttributes (const CDeviceItem &DeviceItem, const CItem &Ammo, TArray<SDisplayAttribute> *retList) const

//	OnAccumulateAttributes
//
//	Adds attributes of the weapon type

	{
	//	Add omnidirectional and arc attributes

	int iMinArc, iMaxArc;
	switch (GetRotationType(DeviceItem, &iMinArc, &iMaxArc))
		{
		case CDeviceRotationDesc::rotOmnidirectional:
			retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("omnidirectional")));
			break;

		case CDeviceRotationDesc::rotSwivel:
			int iArc = AngleRange(iMinArc, iMaxArc);
			if (iArc >= 150)
				retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("hemi-directional")));
			else
				retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("swivel")));
			break;
		}

	//	These properties are valid either for an ammo-less weapon, or a specific
	//	ammo/missile.

	const CWeaponFireDesc *pRootShot;
	if ((GetSelectVariantCount() == 1 || !Ammo.IsEmpty())
			&& (pRootShot = GetWeaponFireDesc(DeviceItem, Ammo)))
		{
		//	Sometimes the fragments do all the damage. In that case, we take 
		//	the properties from the fragment.

		int iFragments;
		const CWeaponFireDesc *pShot = GetReferenceShotData(pRootShot, &iFragments);
		DamageDesc Damage = pShot->GetDamage();

		//	Apply enhancements

		const CItemEnhancementStack &Enhancements = DeviceItem.GetEnhancements();
		Enhancements.ApplySpecialDamage(&Damage);

		//	Compute special abilities.

		if (pRootShot->IsTracking() || pShot->IsTracking() || Enhancements.IsTracking())
			retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("tracking")));

		//	Special damage delivery

		if (pShot->GetType() == CWeaponFireDesc::ftArea)
			retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("shockwave")));

		else if (pShot->GetType() == CWeaponFireDesc::ftRadius)
			retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("radius")));

		//	For continuous beam

		else if (pShot->GetType() == CWeaponFireDesc::ftContinuousBeam)
			retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("beam")));

		//	For particles...

		else if (pShot->GetType() == CWeaponFireDesc::ftParticles)
			retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("cloud")));

		//	For large number of fragments, we have a special description

		else if (iFragments >= 8 && !pShot->IsTracking())
			retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("fragmentation")));

		//	Passthrough
		//	
		//	(We ignore passthrough for shockwaves, since that's already a 
		//	property of them.)

		if (pShot->GetPassthrough() >= 20 && pShot->GetType() != CWeaponFireDesc::ftArea)
			retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("passthrough")));

		//	Stealth

		if (pRootShot->GetStealth() > CSpaceObject::stealthNormal)
			retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("stealth")));

		//	Blinding

		if (Damage.GetBlindingDamage() > 0)
			retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("blinding")));

		//	Radiation

		if (Damage.GetRadiationDamage() > 0)
			retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("radiation")));

		//	EMP

		if (Damage.GetEMPDamage() > 0)
			retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("EMP")));

		//	Device damage

		if (Damage.GetDeviceDamage() > 0)
			retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("device damage")));

		if (Damage.GetDeviceDisruptDamage() > 0)
			retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("device ionization")));

		//	Disintegration

		if (Damage.GetDisintegrationDamage() > 0)
			retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("disintegration")));

		//	Shields

		if (Damage.GetShieldDamageLevel() > 0)
			retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("shield buster")));

		//	Armor

		if (Damage.GetArmorDamageLevel() > 0)
			retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("armor penetrate")));

		//	Mining

		if (Damage.HasMiningDamage())
			{
			EMiningMethod iMethod = CAsteroidDesc::CalcMiningMethod(*pShot);
			switch (iMethod)
				{
				case EMiningMethod::ablation:
					retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("ablative mining")));
					break;

				case EMiningMethod::drill:
					retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("drill mining")));
					break;

				case EMiningMethod::explosion:
					retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("explosive mining")));
					break;

				case EMiningMethod::shockwave:
					retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("shockwave mining")));
					break;
				}
			}

		//	WMD

		if (Damage.GetMassDestructionLevel() > 0)
			retList->Insert(SDisplayAttribute(attribPositive, strPatternSubst(CONSTLIT("WMD %d"), Damage.GetMassDestructionLevel())));
		}

	//	A launcher with no ammo selected.

	else
		{
		const CItemEnhancementStack &Enhancements = DeviceItem.GetEnhancements();

		if (Enhancements.IsTracking())
			retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("tracking")));

		}
	}

void CWeaponClass::OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed)

//	OnAddTypesUsed
//
//	Adds types used by this class

	{
	int i;

	for (i = 0; i < m_ShotData.GetCount(); i++)
		if (m_ShotData[i].bOwned)
			m_ShotData[i].pDesc->AddTypesUsed(retTypesUsed);
	}

ALERROR CWeaponClass::OnDesignLoadComplete (SDesignLoadCtx &Ctx)

//	OnDesignLoadComplete
//
//	Done loading all design elements

	{
	DEBUG_TRY

	ALERROR error;

	//	Shots

	for (int i = 0; i < m_ShotData.GetCount(); i++)
		{
		//	If we own this definition, then we need to bind it.

		if (m_ShotData[i].bOwned)
			{
			if (error = m_ShotData[i].pDesc->OnDesignLoadComplete(Ctx))
				return error;
			}

		//	Otherwise, all we have to do is point to the descriptor from
		//	the item.

		else
			{
			//	Bind the ammoID

			if (error = m_ShotData[i].pAmmoType.Bind(Ctx))
				return error;
			}
		}

	//	Events

	GetItemType()->InitCachedEvents(evtCount, CACHED_EVENTS, m_CachedEvents);

	return NOERROR;

	DEBUG_CATCH
	}

CEffectCreator *CWeaponClass::OnFindEffectCreator (const CString &sUNID)

//	OnFindEffectCreator
//
//	Find the effect. We start after the weapon class UNID.

	{
	//	We start after the weapon class UNID

	const char *pPos = sUNID.GetASCIIZPointer();
	if (*pPos != '/')
		return NULL;

	pPos++;

	//	Parse the variant

	int iOrdinal = strParseInt(pPos, 0, &pPos);
	if (iOrdinal < 0 || iOrdinal >= m_ShotData.GetCount())
		return NULL;

	//	Now ask the weapon fire desc to parse the remaining UNID

	CItem Ammo;
	if (iOrdinal < GetAmmoItemCount())
		Ammo = CItem(GetAmmoItem(iOrdinal), 1);

	CItemCtx ItemCtx;
	CWeaponFireDesc *pDesc = GetWeaponFireDesc(ItemCtx, Ammo);
	return pDesc->FindEffectCreator(CString(pPos));
	}

CString CWeaponClass::OnGetReference (CItemCtx &Ctx, const CItem &Ammo, DWORD dwFlags)

//	OnGetReference
//
//	Return reference

	{
	CString sReference;

	//	For weapons

	if (Ammo.IsEmpty())
		{
		//	For ammo weapons, we describe the kind of ammo we need.

		CItemType *pAmmo;
		if (!IsLauncher()
				&& GetAmmoItemCount() == 1
				&& (pAmmo = GetAmmoItem(0)))
			{
			AppendReferenceString(&sReference, strPatternSubst(CONSTLIT("requires %s"), pAmmo->GetNounPhrase(0x02)));
			}
		}

	return sReference;
	}

void CWeaponClass::OnMarkImages (void)

//	OnMarkImages
//
//	Load all images that we need

	{
	int i;

	for (i = 0; i < m_ShotData.GetCount(); i++)
		m_ShotData[i].pDesc->MarkImages();
	}

ALERROR CWeaponClass::OnPrepareBind (SDesignLoadCtx &Ctx)

//	OnPrepareBind
//
//	Get ready for bind.

	{
	DEBUG_TRY

	//	We need to connect the pointers to the missile items BEFORE we bind 
	//	because sometimes we don't know whether we'll bind weapon or missile

	TSortMap<CItemType *, bool> UniqueAmmo;
	for (int i = 0; i < m_ShotData.GetCount(); i++)
		{
		CItemType *pAmmoType = NULL;

		//	Resolve in case we have ammo

		if (m_ShotData[i].bOwned)
			{
			DWORD dwAmmoUNID = m_ShotData[i].pDesc->GetAmmoTypeUNID();
			if (dwAmmoUNID)
				pAmmoType = Ctx.GetUniverse().FindItemTypeUnbound(dwAmmoUNID);
			}

		//	If we have a reference to a missile we need to connect its weapon
		//	fire descriptor. We need to do this before Bind.

		else
			{
			//	Resolve the item pointer, but don't bind yet.

			DWORD dwAmmoUNID = m_ShotData[i].pAmmoType.GetUNID();
			pAmmoType = Ctx.GetUniverse().FindItemTypeUnbound(dwAmmoUNID);
			if (pAmmoType == NULL)
				{
				Ctx.sError = strPatternSubst(CONSTLIT("Unable to find ItemType for ammoID: %x"), dwAmmoUNID);
				return ERR_FAIL;
				}

			//	Initialize the descriptor

			m_ShotData[i].pDesc = pAmmoType->GetMissileDesc();
			if (m_ShotData[i].pDesc == NULL)
				{
				Ctx.sError = strPatternSubst(CONSTLIT("ItemType must have <Missile> definition: %x"), dwAmmoUNID);
				return ERR_FAIL;
				}
			}

		//	While we're looping, make a list of all unique ammo items.

		if (pAmmoType)
			UniqueAmmo.SetAt(pAmmoType, true);
		}

	//	For each ammo item that we fire, give it a pointer to us so that it can
	//	refer back.

	for (int i = 0; i < UniqueAmmo.GetCount(); i++)
		{
		CItemType *pAmmoType = UniqueAmmo.GetKey(i);
		pAmmoType->AddWeapon(this);
		}

	return NOERROR;

	DEBUG_CATCH
	}

bool CWeaponClass::RequiresItems (void) const

//	RequiresItems
//
//	Returns TRUE if this weapon requires ammo

	{
	return UsesAmmo();
	}

bool CWeaponClass::SelectFirstVariant (CSpaceObject *pSource, CInstalledDevice *pDevice)

//	SelectFirstVariant
//
//	Selects the first valid variant for this weapon

	{
	SetCurrentVariant(pDevice, -1);
	return SelectNextVariant(pSource, pDevice);
	}

bool CWeaponClass::SelectNextVariant (CSpaceObject *pSource, CInstalledDevice *pDevice, int iDir)

//	SelectNextVariant
//
//	Selects the next valid variant for this weapon

	{
	//	For scaling, the descriptor to use is based on something other than the
	//	selection, so we only one selection.

	if (m_iVariantType == varLevelScaling || m_iVariantType == varCharges || m_iVariantType == varCounter)
		{
		CItemCtx ItemCtx(pSource, pDevice);
		CWeaponFireDesc *pShot = GetWeaponFireDesc(ItemCtx);
		if (pShot && VariantIsValid(pSource, pDevice, *pShot))
			{
			SetCurrentVariant(pDevice, 0);
			return true;
			}
		else
			{
			SetCurrentVariant(pDevice, -1);
			return false;
			}
		}
	else
		{
		int iStart = GetCurrentVariant(pDevice);
		if (iStart == -1)
			iStart = m_ShotData.GetCount() + (iDir == 1 ? 0 : -1);
		else
			iStart = m_ShotData.GetCount() + iStart + iDir;

		//	Loop over all variants

		int iSelection = -1;
		if (m_ShotData.GetCount() > 0)
			{
			for (int i = 0; i < m_ShotData.GetCount(); i++)
				{
				int iTry = (iStart + (i * iDir)) % m_ShotData.GetCount();

				//	If this variant is valid, then we're done

				if (VariantIsValid(pSource, pDevice, *m_ShotData[iTry].pDesc))
					{
					iSelection = iTry;
					break;
					}
				}
			}

		//	If we found a selection, then select the variant

		SetCurrentVariant(pDevice, iSelection);
		return (iSelection != -1);
		}
	}

void CWeaponClass::SetAlternatingPos (CInstalledDevice *pDevice, int iAlternatingPos) const

//	SetAlternatingPos
//
//	Sets the alternating position

	{
	pDevice->SetData((pDevice->GetData() & 0xFFFF00FF) | (((DWORD)iAlternatingPos & 0xFF) << 8));
	}

void CWeaponClass::SetContinuousFire (CInstalledDevice *pDevice, DWORD dwContinuous) const

//	SetContinuousFire
//
//	Sets the continuous fire counter for the device

	{
	pDevice->SetData((pDevice->GetData() & 0xFFFFFF00) | (dwContinuous & 0xFF));
	}

bool CWeaponClass::SetCounter (CInstalledDevice *pDevice, CSpaceObject *pSource, CounterTypes iCounter, int iLevel)

//  SetCounter
//
//  Sets the counter to the given level. Returns FALSE if we cannot set it.

	{
	if (m_Counter != iCounter || pDevice == NULL || pSource == NULL)
		return false;

	pDevice->SetTemperature(Max(0, Min(iLevel, MAX_COUNTER)));
	pSource->OnComponentChanged(comDeviceCounter);

	return true;
	}

void CWeaponClass::SetCurrentVariant (CInstalledDevice *pDevice, int iVariant) const

//	SetCurrentVariant
//
//	Sets the current variant for the device

	{
	//	NOTE: We also clear the repeating counter; if we switch missiles, we
	//	want to stop firing the previous missile.

	SetContinuousFire(pDevice, 0);
	pDevice->SetOnUsedLastAmmoFlag(false);

	//	Set the new variant

	pDevice->SetData((((DWORD)(WORD)(short)iVariant) << 16) | LOWORD(pDevice->GetData()));
	}

void CWeaponClass::Update (CInstalledDevice *pDevice, CSpaceObject *pSource, SDeviceUpdateCtx &Ctx)

//	Update
//
//	Update

	{
	DEBUG_TRY

	CItemCtx ItemCtx(pSource, pDevice);

	//	Change counter on update

	if (IsCounterEnabled() && (Ctx.iTick % m_iCounterUpdateRate) == 0)
		{
		if (m_iCounterUpdate > 0)
			{
			if (pDevice->GetTemperature() < MAX_COUNTER)
				{
				pDevice->IncTemperature(Min(m_iCounterUpdate, MAX_COUNTER - pDevice->GetTemperature()));
				pSource->OnComponentChanged(comDeviceCounter);
				}
			}
		else
			{
			if (pDevice->GetTemperature() > 0)
				{
				pDevice->IncTemperature(Max(m_iCounterUpdate, -pDevice->GetTemperature()));
				pSource->OnComponentChanged(comDeviceCounter);
				}
			}
		}

	//	Done if we're disabled

	if (!pDevice->IsEnabled())
		return;

	//	See if we continue to fire

	DWORD dwContinuous = GetContinuousFire(pDevice);
	if (dwContinuous == CONTINUOUS_START)
		{
		CWeaponFireDesc *pShot = GetWeaponFireDesc(ItemCtx);
		if (pShot)
			{
			int iContinuous = GetContinuous(*pShot);
			int iContinuousDelay = Max(1, GetContinuousFireDelay(*pShot) + 1);

			//	-1 is used to skip the first update cycle
			//	(which happens on the same tick as Activate)

			if (iContinuousDelay > 1)
				{
				SetContinuousFire(pDevice, ((iContinuous + 1) * iContinuousDelay) - 1);
				}
			else
				{
				SetContinuousFire(pDevice, iContinuous);
				}
			}
		else
			{
			SetContinuousFire(pDevice, 0);
			pDevice->SetOnUsedLastAmmoFlag(false);
			}
		}
	else if (dwContinuous > 0)
		{
		CWeaponFireDesc *pShot = GetWeaponFireDesc(ItemCtx);
		if (pShot)
			{
			int iContinuous = GetContinuous(*pShot);
			int iContinuousDelay = Max(1, GetContinuousFireDelay(*pShot) + 1);

			SActivateCtx ActivateCtx(NULL, Ctx.TargetList);

			if ((dwContinuous % iContinuousDelay) == 0)
				{
				if (ActivateCtx.TargetList.IsEmpty())
					ActivateCtx.TargetList = pSource->GetTargetList();

				ActivateCtx.iRepeatingCount = 1 + iContinuous - (dwContinuous / iContinuousDelay);

				FireWeapon(*pDevice, *pShot, ActivateCtx);

				if (pSource->IsDestroyed())
					return;
				}
			}

		dwContinuous--;
		SetContinuousFire(pDevice, dwContinuous);

		//	If we've fired the last round, then see if we need to notify the UI
		//	that we're out of ammo.

		if (dwContinuous == 0 && pDevice->IsOnUsedLastAmmoFlagSet())
			{
			pSource->OnDeviceStatus(pDevice, statusUsedLastAmmo);
			pDevice->SetOnUsedLastAmmoFlag(false);
			}
		}
	else if (pDevice->HasLastShots()
			&& (!pDevice->IsTriggered() || pDevice->GetTimeUntilReady() > 1))
		pDevice->SetLastShotCount(0);

	DEBUG_CATCH
	}

bool CWeaponClass::UpdateShipCounter(CItemCtx &ItemCtx, const CWeaponFireDesc &ShotDesc)

//	UpdateShipCounter
//
//	If ship counter is within bounds, we update it and return TRUE. Otherwise,
//	we return FALSE.

{
	//	Get source and device

	CSpaceObject *pSource = ItemCtx.GetSource();
	if (pSource == NULL)
		return false;

	CInstalledDevice *pDevice = ItemCtx.GetDevice();
	if (pDevice == NULL)
		return false;

	//  If we update the ship's counter, make sure that after increase/decrease we're
	//  below/above the maximum/minimum counter, respectively.

	if (m_iCounterPerShot > 0)
	{
		if (pSource->GetCounterValue() + m_iCounterPerShot > pSource->GetMaxCounterValue())
		{
			return false;
		}
	}
	else if (m_iCounterPerShot < 0)
	{
		if (pSource->GetCounterValue() + m_iCounterPerShot < 0)
		{
			return false;
		}
	}
	pSource->IncCounterValue(m_iCounterPerShot);
	return true;
}

bool CWeaponClass::UpdateTemperature (CItemCtx &ItemCtx, const CWeaponFireDesc &ShotDesc, CFailureDesc::EFailureTypes *retiFailureMode, bool *retbSourceDestroyed)

//	UpdateTemperature
//
//	Updates the temperature. We return FALSE if no shot was created and we need 
//	to skip out on the rest of firing.
//
//	Otherwise, TRUE means that retiFailureMode is valid. It is either failNone,
//	which means we succeeded, or failMisfire, which means we fire in the wrong
//	direction.

	{
	if (retbSourceDestroyed)
		*retbSourceDestroyed = false;

	//	Get source and device

	CSpaceObject *pSource = ItemCtx.GetSource();
	if (pSource == NULL)
		return false;

	CInstalledDevice *pDevice = ItemCtx.GetDevice();
	if (pDevice == NULL)
		return false;

	//	If we're overheating, then something happens

	CFailureDesc::EFailureTypes iFailure = CFailureDesc::failNone;
	if (pDevice->GetTemperature() >= OVERHEAT_TEMP)
		{
		//	If we're already past our max temperature, then the weapon does 
		//	nothing.

		if (pDevice->GetTemperature() >= MAX_TEMP)
			{
			pSource->OnDeviceStatus(pDevice, failWeaponJammed);
			return false;
			}

		//	Otherwise, see what kind of failure we might have.

		iFailure = m_OverheatFailure.Failure(pSource, pDevice);
		switch (iFailure)
			{
			//	If nothing happens, or if we misfire, we continue on. The
			//	temperature will increase and we return the failure mode.

			case CFailureDesc::failNone:
			case CFailureDesc::failMisfire:
			case CFailureDesc::failNoFire:
				break;

			//	For an explosion, we cause damage and the temperature does not
			//	continue to go up.

			case CFailureDesc::failExplosion:
				FailureExplosion(ItemCtx, ShotDesc, retbSourceDestroyed);
				return false;

			//	For other failure modes, nothing happens, and the temperature
			//	does not continue to go up.

			default:
				return false;
			}
		}

	//	Update temperature

	pDevice->IncTemperature(m_iCounterActivate);
	pSource->OnComponentChanged(comDeviceCounter);

	//	Done

	if (retiFailureMode)
		*retiFailureMode = iFailure;

	return true;
	}

bool CWeaponClass::UsesLauncherControls (void) const

//	UsesLauncherControls
//
//	Returns TRUE if weapon is selected and fired as a launcher instead of a primary gun (but isn't a launcher)

	{
	return m_bUsesLauncherControls;
	}

bool CWeaponClass::ValidateSelectedVariant (CSpaceObject *pSource, CInstalledDevice *pDevice)

//	ValidateSelectedVariant
//
//	If the selected variant is valid, then it returns TRUE. Otherwise,
//	it selects a different valid variant. If not valid variants are found,
//	it returns FALSE

	{
	CItemCtx Ctx(pSource, pDevice);
	CWeaponFireDesc *pShot = GetWeaponFireDesc(Ctx);
	if (pShot && VariantIsValid(pSource, pDevice, *pShot))
		return true;

	if (SelectNextVariant(pSource, pDevice))
		return true;

	if (SelectFirstVariant(pSource, pDevice))
		return true;

	return false;
	}

bool CWeaponClass::VariantIsValid (const CSpaceObject *pSource, const CInstalledDevice *pDevice, const CWeaponFireDesc &ShotData) const

//	VariantIsValid
//
//	Returns TRUE if the variant is valid

	{
	CItemCtx ItemCtx(pSource, pDevice);

	//	If no ammo, then variant is not valid

	if (!HasAmmoLeft(ItemCtx, &ShotData))
		return false;

	//	Otherwise, valid

	return true;
	}

