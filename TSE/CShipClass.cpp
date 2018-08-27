//	CShipClass.cpp
//
//	CShipClass class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define ARMOR_TAG		        				CONSTLIT("Armor")
#define ARMOR_DISPLAY_TAG						CONSTLIT("ArmorDisplay")
#define COMMUNICATIONS_TAG						CONSTLIT("Communications")
#define DEVICE_SLOTS_TAG						CONSTLIT("DeviceSlots")
#define DEVICES_TAG								CONSTLIT("Devices")
#define DOCK_SCREENS_TAG						CONSTLIT("DockScreens")
#define DRIVE_IMAGES_TAG						CONSTLIT("DriveImages")
#define EFFECTS_TAG								CONSTLIT("Effects")
#define EQUIPMENT_TAG							CONSTLIT("Equipment")
#define ESCORTS_TAG								CONSTLIT("Escorts")
#define EVENTS_TAG								CONSTLIT("Events")
#define FUEL_LEVEL_IMAGE_TAG					CONSTLIT("FuelLevelImage")
#define FUEL_LEVEL_TEXT_TAG						CONSTLIT("FuelLevelText")
#define FUEL_LOW_LEVEL_IMAGE_TAG				CONSTLIT("FuelLowLevelImage")
#define HERO_IMAGE_TAG							CONSTLIT("HeroImage")
#define IMAGE_TAG								CONSTLIT("Image")
#define INITIAL_DATA_TAG						CONSTLIT("InitialData")
#define INSTALL_TAG								CONSTLIT("Install")
#define INTERIOR_TAG							CONSTLIT("Interior")
#define ITEMS_TAG								CONSTLIT("Items")
#define NAMES_TAG								CONSTLIT("Names")
#define NOZZLE_IMAGE_TAG						CONSTLIT("NozzleImage")
#define NOZZLE_POS_TAG							CONSTLIT("NozzlePos")
#define PLAYER_SETTINGS_TAG						CONSTLIT("PlayerSettings")
#define POWER_LEVEL_IMAGE_TAG					CONSTLIT("PowerLevelImage")
#define POWER_LEVEL_TEXT_TAG					CONSTLIT("PowerLevelText")
#define REACTOR_DISPLAY_TAG						CONSTLIT("ReactorDisplay")
#define REACTOR_TEXT_TAG						CONSTLIT("ReactorText")
#define REMOVE_TAG								CONSTLIT("Remove")
#define SHIELD_DISPLAY_TAG						CONSTLIT("ShieldDisplay")
#define TRADE_TAG								CONSTLIT("Trade")
#define WRECK_IMAGE_TAG							CONSTLIT("WreckImage")

#define ARMOR_CRITERIA_ATTRIB					CONSTLIT("armorCriteria")
#define AUTOPILOT_ATTRIB						CONSTLIT("autopilot")
#define CARGO_SPACE_ATTRIB						CONSTLIT("cargoSpace")
#define CHARACTER_ATTRIB						CONSTLIT("character")
#define CHARACTER_CLASS_ATTRIB					CONSTLIT("characterClass")
#define COUNT_ATTRIB							CONSTLIT("count")
#define CYBER_DEFENSE_LEVEL_ATTRIB				CONSTLIT("cyberDefenseLevel")
#define DEBUG_ONLY_ATTRIB						CONSTLIT("debugOnly")
#define DEFAULT_BACKGROUND_ID_ATTRIB			CONSTLIT("defaultBackgroundID")
#define DEFAULT_SOVEREIGN_ATTRIB				CONSTLIT("defaultSovereign")
#define DESC_ATTRIB								CONSTLIT("desc")
#define DEST_X_ATTRIB							CONSTLIT("destX")
#define DEST_Y_ATTRIB							CONSTLIT("destY")
#define DEVICE_CRITERIA_ATTRIB					CONSTLIT("deviceCriteria")
#define DOCK_SCREEN_ATTRIB						CONSTLIT("dockScreen")
#define DRIVE_POWER_USE_ATTRIB					CONSTLIT("drivePowerUse")
#define EQUIPMENT_ATTRIB						CONSTLIT("equipment")
#define EVENT_HANDLER_ATTRIB					CONSTLIT("eventHandler")
#define MAX_REACTOR_FUEL_ATTRIB					CONSTLIT("fuelCapacity")
#define HEIGHT_ATTRIB							CONSTLIT("height")
#define HP_X_ATTRIB								CONSTLIT("hpX")
#define HP_Y_ATTRIB								CONSTLIT("hpY")
#define HULL_VALUE_ATTRIB						CONSTLIT("hullValue")
#define INERTIALESS_DRIVE_ATTRIB				CONSTLIT("inertialessDrive")
#define LEVEL_ATTRIB							CONSTLIT("level")
#define MAX_ARMOR_ATTRIB						CONSTLIT("maxArmor")
#define MAX_ARMOR_SPEED_ATTRIB					CONSTLIT("maxArmorSpeed")
#define MAX_CARGO_SPACE_ATTRIB					CONSTLIT("maxCargoSpace")
#define MAX_DEVICES_ATTRIB						CONSTLIT("maxDevices")
#define MAX_NON_WEAPONS_ATTRIB					CONSTLIT("maxNonWeapons")
#define MAX_REACTOR_POWER_ATTRIB				CONSTLIT("maxReactorPower")
#define MAX_SPEED_ATTRIB						CONSTLIT("maxSpeed")
#define MAX_WEAPONS_ATTRIB						CONSTLIT("maxWeapons")
#define MIN_ARMOR_SPEED_ATTRIB					CONSTLIT("minArmorSpeed")
#define NAME_ATTRIB								CONSTLIT("name")
#define NAME_BREAK_WIDTH						CONSTLIT("nameBreakWidth")
#define NAME_DEST_X_ATTRIB						CONSTLIT("nameDestX")
#define NAME_DEST_Y_ATTRIB						CONSTLIT("nameDestY")
#define NAME_Y_ATTRIB							CONSTLIT("nameY")
#define PERCEPTION_ATTRIB						CONSTLIT("perception")
#define PLAYER_SHIP_ATTRIB						CONSTLIT("playerShip")
#define REACTOR_POWER_ATTRIB					CONSTLIT("reactorPower")
#define REVERSE_ARTICLE_ATTRIB					CONSTLIT("reverseArticle")
#define ROTATION_COUNT_ATTRIB					CONSTLIT("rotationCount")
#define SCORE_ATTRIB							CONSTLIT("score")
#define SIZE_ATTRIB								CONSTLIT("size")
#define SHIP_COMPARTMENT_ATTRIB					CONSTLIT("shipCompartment")
#define SHIP_SCREEN_ATTRIB						CONSTLIT("shipScreen")
#define START_AT_ATTRIB							CONSTLIT("startAt")
#define STARTING_CREDITS_ATTRIB					CONSTLIT("startingCredits")
#define STARTING_POS_ATTRIB						CONSTLIT("startingPos")
#define STARTING_SYSTEM_ATTRIB					CONSTLIT("startingSystem")
#define STD_ARMOR_ATTRIB						CONSTLIT("stdArmor")
#define THRUST_ATTRIB							CONSTLIT("thrust")
#define THRUST_RATIO_ATTRIB						CONSTLIT("thrustRatio")
#define TIME_STOP_IMMUNE_ATTRIB					CONSTLIT("timeStopImmune")
#define UNID_ATTRIB								CONSTLIT("UNID")
#define VIRTUAL_ATTRIB							CONSTLIT("virtual")
#define WIDTH_ATTRIB							CONSTLIT("width")
#define X_ATTRIB								CONSTLIT("x")
#define Y_ATTRIB								CONSTLIT("y")

#define FIELD_ARMOR_COUNT						CONSTLIT("armorCount")				//	Number of armor segments
#define FIELD_ARMOR_HP							CONSTLIT("armorHP")					//	HP of average armor segment
#define FIELD_ARMOR_ITEMS						CONSTLIT("armorItems")
#define FIELD_BALANCE_TYPE						CONSTLIT("balanceType")
#define FIELD_CARGO_SPACE						CONSTLIT("cargoSpace")
#define FIELD_COMBAT_STRENGTH					CONSTLIT("combatStrength")			//	value reflecting combat power (attack and defense)
#define FIELD_DAMAGE_RATE						CONSTLIT("damage")					//	damage per 180 ticks
#define FIELD_DEFENSE_RATE						CONSTLIT("defenseStrength")			//	value reflecting difficulty killing
#define FIELD_DEVICE_SLOTS						CONSTLIT("deviceSlots")
#define FIELD_DEVICE_SLOTS_NON_WEAPONS			CONSTLIT("deviceSlotsNonWeapons")
#define FIELD_DEVICE_SLOTS_WEAPONS				CONSTLIT("deviceSlotsWeapons")
#define FIELD_DEVICE_ITEMS						CONSTLIT("deviceItems")
#define FIELD_DOCK_SERVICES_SCREEN				CONSTLIT("dockServicesScreen")
#define FIELD_DODGE_RATE						CONSTLIT("dodgeRate")
#define FIELD_DRIVE_IMAGE						CONSTLIT("driveImage")
#define FIELD_DRIVE_POWER						CONSTLIT("drivePowerUse")
#define FIELD_EXPLOSION_TYPE					CONSTLIT("explosionType")
#define FIELD_FIRE_ACCURACY						CONSTLIT("fireAccuracy")
#define FIELD_FIRE_RANGE_ADJ					CONSTLIT("fireRangeAdj")
#define FIELD_FIRE_RATE_ADJ						CONSTLIT("fireRateAdj")
#define FIELD_GENERIC_NAME						CONSTLIT("genericName")
#define FIELD_HP								CONSTLIT("hp")
#define FIELD_HULL_MASS							CONSTLIT("hullMass")
#define FIELD_INSTALL_DEVICE_MAX_LEVEL			CONSTLIT("installDeviceMaxLevel")
#define FIELD_LAUNCHER							CONSTLIT("launcher")
#define FIELD_LAUNCHER_UNID						CONSTLIT("launcherUNID")
#define FIELD_LEVEL								CONSTLIT("level")
#define FIELD_MANEUVER							CONSTLIT("maneuver")
#define FIELD_MANUFACTURER						CONSTLIT("manufacturer")
#define FIELD_MASS								CONSTLIT("mass")
#define FIELD_MAX_ARMOR_MASS					CONSTLIT("maxArmorMass")
#define FIELD_MAX_CARGO_SPACE					CONSTLIT("maxCargoSpace")
#define FIELD_MAX_ROTATION						CONSTLIT("maxRotation")
#define FIELD_MAX_SPEED							CONSTLIT("maxSpeed")
#define FIELD_MAX_STRUCTURAL_HP					CONSTLIT("maxStructuralHP")
#define FIELD_NAME								CONSTLIT("name")
#define FIELD_PLAYER_DESC						CONSTLIT("playerDesc")
#define FIELD_PRIMARY_ARMOR						CONSTLIT("primaryArmor")
#define FIELD_PRIMARY_ARMOR_UNID				CONSTLIT("primaryArmorUNID")
#define FIELD_PRIMARY_WEAPON					CONSTLIT("primaryWeapon")
#define FIELD_PRIMARY_WEAPON_RANGE				CONSTLIT("primaryWeaponRange")
#define FIELD_PRIMARY_WEAPON_RANGE_ADJ			CONSTLIT("primaryWeaponRangeAdj")
#define FIELD_PRIMARY_WEAPON_UNID				CONSTLIT("primaryWeaponUNID")
#define FIELD_REGEN								CONSTLIT("regen")
#define FIELD_SCORE								CONSTLIT("score")
#define FIELD_SIZE								CONSTLIT("size")
#define FIELD_SHIELD							CONSTLIT("shield")
#define FIELD_SHIELD_UNID						CONSTLIT("shieldsUNID")
#define FIELD_SHIP_CONFIG_SCREEN				CONSTLIT("shipConfigScreen")
#define FIELD_SHIP_STATUS_SCREEN				CONSTLIT("shipStatusScreen")
#define FIELD_STARTING_SYSTEM					CONSTLIT("startingSystem")
#define FIELD_THRUST							CONSTLIT("thrust")
#define FIELD_THRUST_TO_WEIGHT					CONSTLIT("thrustToWeight")
#define FIELD_THRUSTER_POWER					CONSTLIT("thrusterPower")
#define FIELD_TREASURE_VALUE					CONSTLIT("treasureValue")
#define FIELD_WRECK_CHANCE						CONSTLIT("wreckChance")

#define ERR_OUT_OF_MEMORY						CONSTLIT("out of memory")
#define ERR_BAD_IMAGE							CONSTLIT("invalid ship image")
#define ERR_MISSING_ARMOR_TAG					CONSTLIT("missing <Armor> element")
#define ERR_MISSING_DEVICES_TAG					CONSTLIT("missing <Devices> element")
#define ERR_TOO_MANY_DEVICES					CONSTLIT("too many devices")
#define ERR_BAD_EXHAUST_IMAGE					CONSTLIT("invalid drive image")
#define ERR_DRIVE_IMAGE_FORMAT					CONSTLIT("invalid element in <DriveImages>")
#define ERR_DOCK_SCREEN_NEEDED					CONSTLIT("docking ports specified but no docking screen defined")
#define ERR_SHIELD_DISPLAY_NEEDED				CONSTLIT("missing or invalid <ShieldDisplay> element")
#define ERR_ARMOR_DISPLAY_NEEDED				CONSTLIT("missing or invalid <ArmorDisplay> element")
#define ERR_REACTOR_DISPLAY_NEEDED				CONSTLIT("missing or invalid <ReactorDisplay> element")
#define ERR_INVALID_STARTING_CREDITS			CONSTLIT("invalid starting credits")
#define ERR_UNKNOWN_EQUIPMENT					CONSTLIT("unknown equipment: %s")
#define ERR_UNKNOWN_EQUIPMENT_DIRECTIVE			CONSTLIT("unknown equipment directive: %s")

#define PROPERTY_CURRENCY						CONSTLIT("currency")
#define PROPERTY_CURRENCY_NAME					CONSTLIT("currencyName")
#define PROPERTY_DEFAULT_SOVEREIGN				CONSTLIT("defaultSovereign")
#define PROPERTY_DRIVE_POWER					CONSTLIT("drivePowerUse")
#define PROPERTY_HAS_TRADE_DESC					CONSTLIT("hasTradeDesc")
#define PROPERTY_HAS_VARIANTS					CONSTLIT("hasVariants")
#define PROPERTY_HULL_VALUE						CONSTLIT("hullValue")
#define PROPERTY_MAX_ARMOR_MASS					CONSTLIT("maxArmorMass")
#define PROPERTY_MAX_SPEED						CONSTLIT("maxSpeed")
#define PROPERTY_MAX_SPEED_AT_MAX_ARMOR			CONSTLIT("maxSpeedAtMaxArmor")
#define PROPERTY_MAX_SPEED_AT_MIN_ARMOR			CONSTLIT("maxSpeedAtMinArmor")
#define PROPERTY_MAX_SPEED_BY_ARMOR_MASS		CONSTLIT("maxSpeedByArmorMass")
#define PROPERTY_POWER							CONSTLIT("power")
#define PROPERTY_STD_ARMOR_MASS					CONSTLIT("stdArmorMass")
#define PROPERTY_THRUST							CONSTLIT("thrust")
#define PROPERTY_THRUST_RATIO					CONSTLIT("thrustRatio")
#define PROPERTY_THRUST_TO_WEIGHT				CONSTLIT("thrustToWeight")
#define PROPERTY_THRUSTER_POWER					CONSTLIT("thrusterPower")
#define PROPERTY_WRECK_STRUCTURAL_HP			CONSTLIT("wreckStructuralHP")

#define SPECIAL_IS_PLAYER_CLASS					CONSTLIT("isPlayerClass:")
#define SPECIAL_ITEM_ATTRIBUTE					CONSTLIT("itemAttribute:")
#define SPECIAL_MANUFACTURER					CONSTLIT("manufacturer:")

#define SPECIAL_VALUE_TRUE						CONSTLIT("true")

#define DEFAULT_POWER_USE						20

static char g_AISettingsTag[] = "AISettings";

static char g_ManufacturerAttrib[] = "manufacturer";
static char g_ClassAttrib[] = "class";
static char g_TypeAttrib[] = "type";

const int DOCK_OFFSET_STD_SIZE =				64;

const Metric DRIVE_POWER_EXP =					1.2;
const Metric DRIVE_POWER_FACTOR =				13.0;

const int MAX_HERO_IMAGE_HEIGHT =               528;

struct ScoreDesc
	{
	int iBaseXP;
	int iSpecialXP;
	int iExceptionalXP;
	int iDrawbackXP;
	int iScoreLevel;
	int iMinCapitalShipMass;
	};

static ScoreDesc g_XP[] =
	{
		//	Base Score
		//			Special Ability
		//					Exceptional Ability
		//							Drawback
		//									Level Score
		//												Min Capital
		//												Ship Mass

		{	30,		3,		20,		-2,		50,			5000		},		//	1
		{	55,		5,		40,		-3,		100,		5000		},		//	2
		{	110,	10,		75,		-6,		200,		5000		},		//	3
		{	200,	20,		130,	-10,	350,		7000		},		//	4
		{	340,	30,		230,	-20,	600,		7000		},		//	5

		{	510,	45,		340,	-25,	900,		7000		},		//	6
		{	800,	70,		530,	-40,	1400,		10000	},		//	7
		{	1100,	95,		720,	-55,	1900,		10000	},		//	8
		{	1500,	130,	990,	-80,	2600,		10000	},		//	9
		{	1850,	160,	1250,	-100,	3250,		10000	},		//	10

		{	2400,	210,	1600,	-130,	4200,		18000	},		//	11
		{	3150,	280,	2100,	-170,	5500,		20000	},		//	12
		{	3850,	340,	2550,	-200,	6750,		25000	},		//	13
		{	4700,	410,	3150,	-250,	8250,		36000	},		//	14
		{	5700,	500,	3800,	-300,	10000,		54000	},		//	15

		{	6550,	580,	4350,	-350,	11500,		90000	},		//	16
		{	7550,	660,	5050,	-400,	13250,		117000	},		//	17
		{	8550,	750,	5700,	-450,	15000,		144000	},		//	18
		{	9550,	840,	6350,	-500,	16750,		162000	},		//	19
		{	10500,	930,	7050,	-560,	18500,		180000	},		//	20

		{	11500,	1050,	7800,	-620,	20500,		200000	},		//	21
		{	13000,	1150,	8550,	-680,	22500,		200000	},		//	22
		{	14500,	1250,	9500,	-750,	25000,		200000	},		//	23
		{	15000,	1350,	10000,	-800,	26500,		200000	},		//	24
		{	17000,	1500,	11500,	-900,	30000,		200000	},		//	25
	};

#define SCORE_DESC_COUNT							(sizeof(g_XP) / sizeof(g_XP[0]))

CPlayerSettings CShipClass::m_DefaultPlayerSettings;
bool CShipClass::m_bDefaultPlayerSettingsBound = false;

CShipClass::CShipClass (void) : 
		m_pDeviceSlots(NULL),
		m_pDevices(NULL),
		m_pPlayerSettings(NULL),
		m_pItems(NULL),
		m_pEscorts(NULL),
		m_pTrade(NULL),
		m_fInheritedDevices(false),
		m_fInheritedItems(false),
		m_fInheritedEscorts(false),
		m_fInheritedTrade(false),
        m_fOwnPlayerSettings(false),
        m_fInheritedDeviceSlots(false)

//	CShipClass constructor

	{
	}

CShipClass::~CShipClass (void)

//	CShip destructor

	{
	if (m_pDeviceSlots && !m_fInheritedDeviceSlots)
		delete m_pDeviceSlots;

	if (m_pDevices && !m_fInheritedDevices)
		delete m_pDevices;

	if (m_fOwnPlayerSettings)
		delete m_pPlayerSettings;

	if (m_pItems && !m_fInheritedItems)
		delete m_pItems;

	if (m_pEscorts && !m_fInheritedEscorts)
		delete m_pEscorts;

	if (m_pTrade && !m_fInheritedTrade)
		delete m_pTrade;
	}

Metric CShipClass::GetStdCombatStrength (int iLevel)

//	GetStdCombatStrength
//
//	Returns the standard combat strength for a ship of the given level.

	{
	const Metric k0 = 1.4;
	const Metric k1 = 8.0;
	return k1 * pow(k0, iLevel - 1);
	}

int CShipClass::CalcArmorSpeedBonus (int iTotalArmorMass) const

//	CalcArmorSpeedBonus
//
//	Computes addition/penalty to max speed based on the total armor mass.

	{
	return m_Hull.CalcArmorSpeedBonus(m_Armor.GetCount(), iTotalArmorMass);
	}

CShipClass::EBalanceTypes CShipClass::CalcBalanceType (CString *retsDesc, Metric *retrCombatStrength) const

//	CalcBalanceType
//
//	Calculates the balance type for this ship (based on level and combat strength.

	{
	Metric rCombat = CalcCombatStrength();
	Metric rAttack = CalcDamageRate();
	Metric rDefense = CalcDefenseRate();
	Metric rAttackDefenseRatio = (rDefense > 0.0 ? rAttack / rDefense : 1000.0);

	int iLevel = GetLevel();

	if (retrCombatStrength)
		*retrCombatStrength = rCombat;

	//	Calculate the standard combat strength for ships of this level.

	Metric rStdCombat = GetStdCombatStrength(iLevel);

	//	If we're less than 1/4 standard then we're too weak to be a minion

	if (rCombat < 0.25 * rStdCombat
			&& iLevel > 1)
		{
		if (retsDesc) *retsDesc = CONSTLIT("too weak");
		return typeTooWeak;
		}

	//	If we're more than 4 times standard then we're too strong even for a boss

	else if (rCombat >= 4.0 * rStdCombat)
		{
		if (retsDesc) *retsDesc = CONSTLIT("too strong");
		return typeTooStrong;
		}

	//	See if this is a non-combatant

	else if (rAttackDefenseRatio < 0.5 && rAttack < 0.5 * rStdCombat)
		{
		if (retsDesc) *retsDesc = CONSTLIT("non-combatant");
		return typeNonCombatant;
		}

	//	If we're less than 3/4 standard and our defense is low, then we're a minion

	else if (rCombat < 0.75 * rStdCombat
			&& rAttackDefenseRatio > 0.5)
		{
		if (retsDesc) *retsDesc = CONSTLIT("minion");
		return typeMinion;
		}

	//	If we're less than 1.5 standard then we're a standard ship.

	else if (rCombat < 1.5 * rStdCombat)
		{
		if (retsDesc) *retsDesc = CONSTLIT("standard");
		return typeStandard;
		}

	//	If we're less than 2.5 standard then we're elite

	else if (rCombat < 2.5 * rStdCombat)
		{
		if (retsDesc) *retsDesc = CONSTLIT("elite");
		return typeElite;
		}

	//	If we're less than 4 times standard then we're a boss

	else
		{
		if (retsDesc) *retsDesc = CONSTLIT("boss");
		return typeBoss;
		}
	}

CItemType *CShipClass::CalcBestMissile (const SDeviceDesc &Device) const

//	CalcBestMissile
//
//	Computes the best missile item type available to the given device (which 
//	must be a launcher).

	{
	CItemType *pType = Device.Item.GetType();
	CDeviceClass *pDevice = (pType ? pType->GetDeviceClass() : NULL);
	if (pDevice == NULL)
		return NULL;

	//	Look for the highest missile level and use that as the
	//	weapon level.

	CItemType *pBestMissile = NULL;
	IItemGenerator *pItems = GetRandomItemTable();
	if (pItems)
		FindBestMissile(pDevice, pItems, &pBestMissile);

	FindBestMissile(pDevice, Device.ExtraItems, &pBestMissile);

	return pBestMissile;
	}

Metric CShipClass::CalcCombatStrength (void) const

//	CalcCombatStrength
//
//	Computes a rough value for combat strength of the ship.

	{
	//	Get the attack strength (and the best weapon level)

	int iWeaponLevel;
	Metric rAttack = CalcDamageRate(NULL, &iWeaponLevel);

	//	If we're using weapons for a damage type weaker than expected for our
	//	current level, then we adjust.

	int iExpectedDamageTier = (GetLevel() + 2) / 3;
	int iWeaponDamageTier = (iWeaponLevel + 2) / 3;
	switch (iExpectedDamageTier - iWeaponDamageTier)
		{
		case 1:
			rAttack *= 0.85;
			break;

		case 2:
			rAttack *= 0.5;
			break;

		case 3:
		case 4:
		case 5:
		case 6:
			rAttack *= 0.33;
			break;
		}

	//	Get the defense strength

	Metric rDefense = CalcDefenseRate();

	//	Combine attack and defense strengths

	const Metric ATTACK_FACTOR = 0.5;
	const Metric DEFENSE_FACTOR = 0.25;
	const Metric MAX_ATTACK_RATIO = 5.0;

	return (ATTACK_FACTOR * Min(rAttack, rDefense * MAX_ATTACK_RATIO))
		+ (DEFENSE_FACTOR * rDefense);
	}

Metric CShipClass::CalcDamageRate (int *retiAveWeaponLevel, int *retiMaxWeaponLevel) const

//	CalcDamageRate
//
//	Computes the rough number of HP of damage dealt by this ship in 180 ticks.
//	This is based on weapons and maneuverability.
//
//	NOTE: Damage rate is NOT adjusted for damage type because we want this to
//	follow the current weapon damage HP table (which also does not adjust for
//	damage type). Callers should adjust if necessary.

	{
	int i;

	const Metric MANEUVER_FACTOR = 0.5;

	Metric rTotalDamageRate = 0.0;
	Metric rTotalDamageLevels = 0.0;
	Metric rPrimaryDamageRate = 0.0;
	int iPrimaryLevel = 0;
	int iMaxWeaponLevel = 0;

	//	Loop over all weapons and add up their damage.

	for (i = 0; i < m_AverageDevices.GetCount(); i++)
		{
		const SDeviceDesc &Desc = m_AverageDevices.GetDeviceDesc(i);
        CItemCtx ItemCtx(Desc.Item);
        CDeviceClass *pDevice = ItemCtx.GetDeviceClass();
		bool bIsLauncher;

		//	Figure out if this is a weapon or a launcher; if neither, then we
		//	skip this device.

		if (pDevice == NULL)
			continue;
		else if (pDevice->GetCategory() == itemcatLauncher)
			bIsLauncher = true;
		else if (pDevice->GetCategory() == itemcatWeapon)
			bIsLauncher = false;
		else
			continue;

		CWeaponClass *pWeapon = pDevice->AsWeaponClass();
		if (pWeapon == NULL)
			continue;

		//	If this is a launcher then we need to figure out the best available
		//	variant.

        CItem Ammo;
		int iMissileLevel = 0;
		if (bIsLauncher)
			{
			CItemType *pBestMissile = CalcBestMissile(Desc);
			if (pBestMissile)
				{
                Ammo = CItem(pBestMissile, 1);
                iMissileLevel = Ammo.GetLevel();
				}
            else
                {
                //  If the launcher has no ammo, then skip to the next device.

                continue;
                }
			}

		//	Compute the damage rate for the weapon

        Metric rDamageRate = pDevice->GetAmmoItemPropertyDouble(ItemCtx, Ammo, CONSTLIT("damage"));
		bool bCanTrack = pDevice->GetAmmoItemPropertyBool(ItemCtx, Ammo, CONSTLIT("tracking"));

		//	Compute weapon level

		int iWeaponLevel = (bIsLauncher ? iMissileLevel : Desc.Item.GetLevel());

		//	Compute the total coverage of the weapon (360 = omnidirectional; 0 = fixed)

		int iDeviceCoverage;
		if (Desc.bOmnidirectional)
			iDeviceCoverage = 360;
		else if (Desc.iMaxFireArc != Desc.iMinFireArc)
			iDeviceCoverage = ::AngleRange(Desc.iMinFireArc, Desc.iMaxFireArc);
		else
			iDeviceCoverage = 0;

		int iMinFireArc;
		int iMaxFireArc;
		int iWeaponCoverage;
		if (!pWeapon->CanRotate(CItemCtx(), &iMinFireArc, &iMaxFireArc))
			iWeaponCoverage = 0;
		else if (iMinFireArc != iMaxFireArc)
			iWeaponCoverage = ::AngleRange(iMinFireArc, iMaxFireArc);
		else
			iWeaponCoverage = 360;

		if (bCanTrack)
			iWeaponCoverage = Max(iWeaponCoverage, 180);

		int iCoverage = Max(iDeviceCoverage, iWeaponCoverage);

		//	For area weapons we adjust damage rate based on coverage.

		if (iCoverage >= 90)
			rDamageRate *= ((Metric)iCoverage / 360.0);

		//	Otherwise, for fixed weapons we need to adjust based on the 
		//	maneuverability of the ship.

		else
			{
			int iManeuverDelay = Max(1, Min(GetManeuverDelay(), 4));
			Metric rManeuver = 1.0 / iManeuverDelay;

			rDamageRate *= rManeuver 
					* MANEUVER_FACTOR
					* (1.0 + (3.0 * iCoverage / 90.0));
			}

		//	Adjust by AI fire rate

		if (m_AISettings.GetFireRateAdj() > 0)
			rDamageRate *= 10.0 / m_AISettings.GetFireRateAdj();

		//	If this is a primary weapon, then keep track of the best rate.
		//	Otherwise, we just add to the total.

		if (bIsLauncher || Desc.bSecondary)
			{
			rTotalDamageRate += rDamageRate;
			rTotalDamageLevels += (rDamageRate * iWeaponLevel);
			}
		else
			{
			if (rDamageRate > rPrimaryDamageRate)
				{
				rPrimaryDamageRate = rDamageRate;
				iPrimaryLevel = iWeaponLevel;
				}
			}

		//	Keep track of max weapon level

		if (iWeaponLevel > iMaxWeaponLevel)
			iMaxWeaponLevel = iWeaponLevel;
		}

	//	Add up primary weapon

	rTotalDamageRate += rPrimaryDamageRate;
	rTotalDamageLevels += (rPrimaryDamageRate * iPrimaryLevel);

	//	Compute average level, if necessary

	if (retiAveWeaponLevel)
		*retiAveWeaponLevel = (rTotalDamageRate > 0.0 ? (int)((rTotalDamageLevels / rTotalDamageRate) + 0.5) : 0);

	if (retiMaxWeaponLevel)
		*retiMaxWeaponLevel = iMaxWeaponLevel;

	//	Done

	return rTotalDamageRate;
	}

int CShipClass::CalcDefaultSize (const CObjectImageArray &Image)

//	CalcDefaultSize
//
//	Computes the size of the ship (in meters) based on the image size.

	{
	const Metric K0 = 0.2;
	const Metric K1 = 1.0 / 0.68;

	//	Start with the size of the frame in pixels. Adjust a little because 
	//	ships don't usually go out to the edge.

	Metric rPixelSize = 0.8 * (Metric)Image.GetImageWidth();

	//	Convert to meters using our standard scale

	Metric rMeterSize = pow(K0 * rPixelSize, K1);

	//	Done

	return (int)mathRound(rMeterSize);
	}

Metric CShipClass::CalcDefenseRate (void) const

//	CalcDefenseRate
//
//	Calculates the smallest amount of damage per 180 ticks required to destroy
//	the ship in less than 6 * 180 ticks.

	{
	const Metric T = 6.0;	//	six 180 tick cycles

	//	Compute armor strength values

	Metric Ahp;
	Metric Aregen;
    const CShipArmorSegmentDesc *pSect = (m_Armor.GetCount() > 0 ? &m_Armor.GetSegment(0) : NULL);

	if (pSect)
		{
        CArmorClass *pArmor = pSect->GetArmorClass();
        CItem ArmorItem;
        pSect->CreateArmorItem(&ArmorItem);
        CItemCtx ItemCtx(ArmorItem);

		Ahp = pArmor->GetMaxHP(ItemCtx);
		Aregen = pArmor->GetItemType()->GetDataFieldInteger(FIELD_REGEN);

		//	Adjust for damage type adjustment (for armor that has more resistance
		//	than standard armor, e.g., meteorsteel).

		int iDamageAdj = pArmor->CalcAverageRelativeDamageAdj(ItemCtx);
		Metric rDamageAdj = (iDamageAdj > 0 ? 100.0 / iDamageAdj : 10.0);
		Ahp *= rDamageAdj;
		Aregen *= rDamageAdj;

		//	Adjust for number of segments

		Metric rAdj;
		switch (m_Armor.GetCount())
			{
			case 1:
				rAdj = 0.5;
				break;

			case 2:
			case 3:
				rAdj = 0.67;
				break;

			case 4:
			case 5:
			case 6:
			case 7:
				rAdj = 0.9;
				break;

			case 8:
			case 9:
			case 10:
			case 11:
			case 12:
				rAdj = 1.0;
				break;

			default:
				rAdj = Max(1.0, m_Armor.GetCount() / 12.0);
			}

		Ahp *= rAdj;
		Aregen *= rAdj;
		}
	else
		{
		Ahp = 0.0;
		Aregen = 0.0;
		}

	//	If we have interior structure, we add it to the armor hit points.
	//	NOTE: We don't try to this as a third term in the equation below because
	//	it is too hard to solve the equation. [And because it is rare for
	//	either armor or interior to have high regeneration.]

	if (!m_Interior.IsEmpty())
		{
		Metric Ihp = m_Interior.GetHitPoints();
		if (Ihp > 0.0)
			{
			//	Adjust because we need WMD damage

			Ihp *= 2.0;

			//	Adjust regen proportionally.

			Metric rTotal = Ahp + Ihp;
			Aregen = (Ahp * Aregen) / rTotal;

			//	Adjust armor hp

			Ahp += Ihp;
			}
		}

	//	Compute shield strength values

	Metric Shp;
	Metric Sregen;
	CDeviceClass *pDevice = m_AverageDevices.GetNamedDevice(devShields);
	if (pDevice)
		{
		CItemType *pShieldType = pDevice->GetItemType();
		Shp = pShieldType->GetDataFieldInteger(FIELD_HP);
		Sregen = pShieldType->GetDataFieldInteger(FIELD_REGEN);
		}
	else
		{
		Shp = 0.0;
		Sregen = 0.0;
		}

	//		    Shp			   Ahp
	//	T = ------------ + ------------
	//		 X - Sregen		X - Aregen
	//
	//	Solve for X (both roots)

	Metric i1 = pow(-Ahp - Aregen * T - Shp - Sregen * T, 2) - 4.0 * T * (Ahp * Sregen + Aregen * Shp + Aregen * Sregen * T);
	if (i1 < 0.0)
		return 0.0;

	Metric i2 = Ahp + Aregen * T + Shp + Sregen * T;

	Metric x1 = -(sqrt(i1) + i2) / (2 * T);
	Metric x2 = (sqrt(i1) + i2) / (2 * T);

	//	Take whichever is positive

	Metric rRate;
	if (x1 > 0.0)
		rRate = x1;
	else if (x2 > 0.0)
		rRate = x2;
	else
		return 0.0;

	//	Adjust rate based on maneuverability of ship

	Metric rHitRate = rRate * (1.0 - CalcDodgeRate());
	rRate = rRate * rRate / rHitRate;

	//	Done

	return rRate;
	}

CurrencyValue CShipClass::CalcHullValue (void) const

//	CalcHullValue
//
//	Computes the value of the hull based on its properties (in credits).

	{
	static const Metric PARTIAL_SLOT_FACTOR = 0.5;
	static const Metric CARGO_PER_POINT = 100.0;
	static const Metric MAX_CARGO_PER_POINT = 200.0;
	static const int STD_ARMOR_SEGMENTS = 4;
	static const Metric POINTS_PER_ARMOR_SEGMENT = 0.5;
	static const Metric ARMOR_PER_POINT = 6000.0;
	static const Metric MAX_ARMOR_PER_POINT = 12000.0;
	static const Metric MIN_SPEED = 15.0;
	static const Metric SPEED_PER_POINT = 6.0;
	static const Metric THRUST_RATIO_PER_POINT = 16.0;
	static const Metric MAX_ROTATION_PER_POINT = 9.0;

	static const Metric PRICE_PER_TENTH_MW = 0.5;
	static const Metric POINT_BIAS = -10.0;
	static const Metric POINT_EXP = 1.5;

	//	We need to have max reactor powe defined, or else we can't compute the
	//	values.

	if (m_Hull.GetMaxReactorPower() == 0)
		return 0;

	//	We use a point system to sum of the value of the hull properties.

	Metric rPoints = 0.0;

	//	Start by adding up points for device slots.

	int iFullSlots = Min(m_Hull.GetMaxWeapons(), m_Hull.GetMaxNonWeapons());
	int iPartialSlots = m_Hull.GetMaxDevices() - iFullSlots;

	rPoints += iFullSlots;
	rPoints += (iPartialSlots * PARTIAL_SLOT_FACTOR);

	//	Add up points for cargo space

	rPoints += m_Hull.GetCargoSpace() / CARGO_PER_POINT;
	rPoints += (m_Hull.GetMaxCargoSpace() - m_Hull.GetCargoSpace()) / MAX_CARGO_PER_POINT;

	//	Add points for the number of armor segments and for max armor

	rPoints += (m_Armor.GetCount() - STD_ARMOR_SEGMENTS) * POINTS_PER_ARMOR_SEGMENT;
	rPoints += m_Hull.GetStdArmorMass() / ARMOR_PER_POINT;
	rPoints += m_Hull.GetMaxArmorMass() / MAX_ARMOR_PER_POINT;

	//	Points for max speed

	rPoints += Max(0.0, ((100.0 * m_DriveDesc.GetMaxSpeed() / LIGHT_SPEED) - MIN_SPEED)) / SPEED_PER_POINT;

	//	Points for thrust ratio

	Metric rThrustRatio = m_rThrustRatio;
	if (rThrustRatio <= 0.0)
		rThrustRatio = CDriveDesc::CalcThrustRatio(m_DriveDesc.GetThrust(), m_Hull.GetMass());

	rPoints += rThrustRatio / THRUST_RATIO_PER_POINT;

	//	Points for maneuverability

	rPoints += m_RotationDesc.GetMaxRotationPerTick() / MAX_ROTATION_PER_POINT;

	//	Add any extra points added manually.

	rPoints += m_Hull.GetExtraPoints();

	//	Compute a price unit based on the maximum reactor power

	Metric rUnitPrice = PRICE_PER_TENTH_MW * m_Hull.GetMaxReactorPower();

	//	Scale points

	Metric rScaledPoints = pow(rPoints + POINT_BIAS, POINT_EXP);

	//	Compute price

	Metric rPrice = rScaledPoints * rUnitPrice;

	//	Done

	return (CurrencyValue)round(rPrice);
	}

int CShipClass::CalcLevel (void) const

//	CalcLevel
//
//	Calculate the ship level based on devices, etc.

	{
	int i;

	int iBestLevel = 1;

	//	Figure out what armor we've got

	int iArmorSections = GetHullSectionCount();
	int iArmorLevel = (iArmorSections > 0 ? GetHullSection(0).GetLevel() : 1);
	if (iArmorLevel > iBestLevel)
		iBestLevel = iArmorLevel;

	//	Loop over all devices and figure out the highest level.

	for (i = 0; i < m_AverageDevices.GetCount(); i++)
		{
		const SDeviceDesc &Desc = m_AverageDevices.GetDeviceDesc(i);
		CDeviceClass *pDevice = Desc.Item.GetType()->GetDeviceClass();

		//	Compute the device level

		int iDeviceLevel = ComputeDeviceLevel(Desc);

		//	Keep track of the best device level

		if (iDeviceLevel > iBestLevel)
			iBestLevel = iDeviceLevel;
		}

	//	Done

	return iBestLevel;
	}

Metric CShipClass::CalcManeuverValue (bool bDodge) const

//	CalcManeuverValue
//
//	Computes the rough probability (0.0-1.0) that the ship can dodge an enemy shot,
//	based on maneuverability, speed, and thrust.

	{
	const Metric PIVOT_SPEED = 12.0;
	const Metric PIVOT_DODGE = 0.5;
	const Metric K1 = 0.5;
	const Metric K2 = 1.0 / PI;
	const Metric K3 = 1.25 / PI;

	const Metric PIVOT_THRUST = 12.0;
	const Metric MAX_THRUST_ADJ = 8.0;

	const int SIZE_FACTOR = 16;

	//	Get some statistics

    const CDriveDesc &Drive = m_Perf.GetDriveDesc();

	Metric rMass = CalcMass(m_AverageDevices);
	Metric rThrustRatio = 2.0 * (rMass > 0.0 ? Drive.GetThrust() / rMass : 0.0);
	int iManeuverDelay = Max(1, GetManeuverDelay());
	Metric rSpeed = 100.0 * Drive.GetMaxSpeed() / LIGHT_SPEED;

	//	Speed of 12 is a pivot point. Above this speed it gets easy to dodge
	//	shots; below this speed it gets increasingly harder.

	Metric rDodge;
	if (rSpeed >= PIVOT_SPEED)
		rDodge = PIVOT_DODGE + (K2 * atan(K1 * (rSpeed - PIVOT_SPEED)));
	else
		rDodge = Max(0.0, PIVOT_DODGE + (K3 * atan(K1 * (rSpeed - PIVOT_SPEED))));

	//	Generate an adjustment based on maneuverability:
	//	1 = normal dodge rate; 10 = very low dodge rate

	int iManeuverAdj = iManeuverDelay;

	//	Generate an adjustment based on thrust-ratio:
	//	1 = normal dodge rate; 10 = very low dodge rate

	int iThrustAdj = (rThrustRatio < PIVOT_THRUST ? (int)(1.0 + (MAX_THRUST_ADJ * (PIVOT_THRUST - rThrustRatio) / PIVOT_THRUST)) : 1);

	//	Generate an adjustment based on ship size:
	//	1 = normal dodge rate; 10 = very low dodge rate

	int cxWidth = RectWidth(m_Image.GetSimpleImage().GetImageRect());
	int iSizeAdj = (bDodge ? Max(1, cxWidth / SIZE_FACTOR) : 1);

	//	Adjust dodge rate

	rDodge = pow(rDodge, Max(Max(iManeuverAdj, iThrustAdj), iSizeAdj));

	//	Done

	return rDodge;
	}

Metric CShipClass::CalcMass (const CDeviceDescList &Devices) const

//	CalcMass
//
//	Returns the total mass of the ship class, including devices and armor
//	(in tons).

	{
	int i;
	Metric rMass = m_Hull.GetMass();

    rMass += m_Armor.CalcMass();

	for (i = 0; i < Devices.GetCount(); i++)
		{
		CItem Item(Devices.GetDeviceClass(i)->GetItemType(), 1);
		rMass += Item.GetMass();
		}

	return rMass;
	}

ICCItem *CShipClass::CalcMaxSpeedByArmorMass (CCodeChainCtx &Ctx) const

//	CalcMaxSpeedByArmorMass
//
//	Returns a struct with entries for each value of max speed. Each entry has the
//	smallest armor mass which results in the given speed.
//
//	If there is no variation in speed, we return a single speed value.

	{
	int iStdSpeed = mathRound(100.0 * m_Perf.GetDriveDesc().GetMaxSpeed() / LIGHT_SPEED);
	return m_Hull.CalcMaxSpeedByArmorMass(Ctx, iStdSpeed);
	}

void CShipClass::CalcPerformance (void)

//  CalcPerformance
//
//  Calculates the basic stats including all devices. This function initializes
//  m_Perf and uses m_AverageDevices.

    {
	DEBUG_TRY

    int i;

    //  We generate a context block and accumulate performance stats from the
    //  class, armor, devices, etc.
    //
    //  These fields are context for the ship that we're computing.

    SShipPerformanceCtx Ctx(this);

    //  Start with parameters from the class

    InitPerformance(Ctx);

    //  Accumulate settings from armor

	for (i = 0; i < m_Armor.GetCount(); i++)
		{
        CItem ArmorItem;
        if (!m_Armor.GetSegment(i).CreateArmorItem(&ArmorItem)
                || ArmorItem.IsEmpty())
            continue;

        CItemCtx ItemCtx(ArmorItem);
        ItemCtx.GetArmorClass()->AccumulatePerformance(ItemCtx, Ctx);
		}

    //  Accumulate settings from devices

    for (i = 0; i < m_AverageDevices.GetCount(); i++)
        {
        const CItem &DeviceItem = m_AverageDevices.GetDeviceDesc(i).Item;
        if (DeviceItem.IsEmpty())
            continue;

        CItemCtx ItemCtx(DeviceItem);
        ItemCtx.GetDeviceClass()->AccumulatePerformance(ItemCtx, Ctx);
        }

    //  Now apply the performance parameters to the descriptor

    m_Perf.Init(Ctx);

	DEBUG_CATCH
    }

int CShipClass::CalcScore (void)

//	CalcScore
//
//	Computes the score of the ships class

	{
	//	Compute the movement stats

	int iSpeed, iThrust, iManeuver;
	ComputeMovementStats(&iSpeed, &iThrust, &iManeuver);

	//	Figure out what armor we've got

	int iArmorSections = GetHullSectionCount();
	int iArmorLevel = (iArmorSections > 0 ? GetHullSection(0).GetLevel() : 1);

	//	Figure out what devices we've got

	int iWeaponLevel = 0;
	int iWeapon = -1;
	int iLauncherLevel = 0;
	int iLauncher = -1;
	int iShieldLevel = 0;

	for (int j = 0; j < m_AverageDevices.GetCount(); j++)
		{
		CDeviceClass *pDevice = m_AverageDevices.GetDeviceDesc(j).Item.GetType()->GetDeviceClass();
		switch (pDevice->GetCategory())
			{
			case itemcatWeapon:
				{
				if (pDevice->GetLevel() > iWeaponLevel)
					{
					iWeaponLevel = pDevice->GetLevel();
					iWeapon = j;
					}
				break;
				}

			case itemcatLauncher:
				{
				iLauncherLevel = ComputeDeviceLevel(m_AverageDevices.GetDeviceDesc(j));
				iLauncher = j;
				break;
				}

			case itemcatShields:
				{
				iShieldLevel = pDevice->GetLevel();
				break;
				}
			}
		}

	int iPrimaryWeapon;
	if (iLauncherLevel > iWeaponLevel)
		iPrimaryWeapon = iLauncher;
	else
		iPrimaryWeapon = iWeapon;

	//	Compute score and level

	return ComputeScore(iArmorLevel, 
			iPrimaryWeapon, 
			iSpeed, 
			iThrust, 
			iManeuver,
			iLauncherLevel > iWeaponLevel);
	}

int CShipClass::ComputeDeviceLevel (const SDeviceDesc &Device) const

//	ComputeDeviceLevel
//
//	Computes the level of the given device

	{
	CItemType *pType = Device.Item.GetType();
	CDeviceClass *pDevice = (pType ? pType->GetDeviceClass() : NULL);
	if (pDevice == NULL)
		return 0;

	if (pDevice->GetCategory() == itemcatLauncher)
		{
		//	Look for the highest missile level and use that as the
		//	weapon level.

		CItemType *pBestMissile = CalcBestMissile(Device);
		return (pBestMissile ? pBestMissile->GetLevel() : 0);
		}
	else
		return pDevice->GetLevel();
	}

enum LowMediumHigh
	{
	enumLow = 0,
	enumMedium = 1,
	enumHigh = 2,
	};

void CShipClass::ComputeMovementStats (int *retiSpeed, int *retiThrust, int *retiManeuver)
	{
    const CDriveDesc &DriveDesc = m_Perf.GetDriveDesc();

	//	Figure out the speed of the ship

	if (DriveDesc.GetMaxSpeed() > 0.20 * LIGHT_SPEED)
		*retiSpeed = enumHigh;
	else if (DriveDesc.GetMaxSpeed() > 0.15 * LIGHT_SPEED)
		*retiSpeed = enumMedium;
	else
		*retiSpeed = enumLow;

	//	Figure out the mass of the ship (including all installed
	//	weapons and armor)

	Metric rFullMass = CalcMass(m_AverageDevices);

	//	Figure out the thrust of the ship

	Metric rRatio = (Metric)DriveDesc.GetThrust() / rFullMass;
	if (rRatio >= 7.0)
		*retiThrust = enumHigh;
	else if (rRatio >= 3.0)
		*retiThrust = enumMedium;
	else
		*retiThrust = enumLow;

	//	Figure out the maneuverability of the ship

    const CIntegralRotationDesc &RotationDesc = m_Perf.GetRotationDesc();
	if (RotationDesc.GetMaxRotationTimeTicks() >= 90)
		*retiManeuver = enumLow;
	else if (RotationDesc.GetMaxRotationTimeTicks() > 30)
		*retiManeuver = enumMedium;
	else
		*retiManeuver = enumHigh;
	}

int CShipClass::ComputeScore (int iArmorLevel,
							  int iPrimaryWeapon,
							  int iSpeed,
							  int iThrust,
							  int iManeuver,
							  bool bPrimaryIsLauncher)

//	ComputeScore
//
//	Compute the score of the class based on equipment

	{
	int i;
	int iSpecial = 0;
	int iExceptional = 0;
	int iDrawback = 0;
	int iStdLevel = iArmorLevel;
	int iWeaponLevel = (iPrimaryWeapon == -1 ? 0 : ComputeDeviceLevel(m_AverageDevices.GetDeviceDesc(iPrimaryWeapon)));

	//	If our weapon is better than our armor then adjust the level
	//	depending on the difference.

	if (iWeaponLevel > iArmorLevel)
		{
		switch (iWeaponLevel - iArmorLevel)
			{
			case 1:
				iStdLevel = iWeaponLevel;
				iDrawback++;
				break;

			case 3:
				iStdLevel = iWeaponLevel - 2;
				iSpecial += 2;
				break;

			default:
				iStdLevel = (iWeaponLevel + iArmorLevel) / 2;
			}
		}

	//	If our best weapon is 2 or more levels below our standard
	//	level then take drawbacks exponentially.

	if (iStdLevel > iWeaponLevel + 1)
		iDrawback += min(16, (1 << (iStdLevel - (iWeaponLevel + 2))));
	else if (iStdLevel > iWeaponLevel)
		iDrawback++;

	//	If all movement stats are high then this counts as an
	//	exceptional ability

	if (iSpeed == enumHigh && iThrust == enumHigh && iManeuver == enumHigh)
		iExceptional++;

	//	Otherwise, treat them as special abilities or drawbacks

	else
		{
		if (iSpeed == enumLow)
			iDrawback++;
		else if (iSpeed == enumHigh)
			iSpecial++;

		if (iThrust == enumLow)
			iDrawback++;
		else if (iThrust == enumHigh)
			iSpecial++;

		if (iManeuver == enumLow)
			iDrawback++;
		else if (iManeuver == enumHigh)
			iSpecial++;
		}

	//	1 armor segment is a drawback

	int iArmorSections = GetHullSectionCount();
	if (iArmorSections <= 1)
		iDrawback++;

	//	2-3 armor segments is normal

	else if (iArmorSections < 4)
		;

	//	4 or more armor segments is special

	else if (iArmorSections < 8 )
		iSpecial++;
	else if (iArmorSections < 16)
		iSpecial += 2;
	else if (iArmorSections < 32)
		iSpecial += 3;
	else if (iArmorSections < 64)
		iSpecial += 4;
	else
		iSpecial += 5;

	//	Checkout all the devices

	bool bDirectionalBonus = false;
	bool bGoodSecondary = false;
	int iDirectionalBonus = 0;
	for (i = 0; i < m_AverageDevices.GetCount(); i++)
		{
		const SDeviceDesc &Dev = m_AverageDevices.GetDeviceDesc(i);
		CDeviceClass *pDevice = Dev.Item.GetType()->GetDeviceClass();
		int iDeviceLevel = ComputeDeviceLevel(Dev);

		//	Specific devices

		switch (pDevice->GetCategory())
			{
			case itemcatWeapon:
			case itemcatLauncher:
				{
				int iWeaponAdj = (iDeviceLevel - iStdLevel);

				//	If this is a secondary weapon, then add it to the score

				if (i != iPrimaryWeapon)
					{
					//	Calculate any potential bonus based on the weapon level
					//	compared to the base level

					iSpecial += max(iWeaponAdj + 3, 0);
					}

				//	Compute fire arc

				int iFireArc = (Dev.bOmnidirectional ? 360 : AngleRange(Dev.iMinFireArc, Dev.iMaxFireArc));

				//	Adjust for turret-mount

				iDirectionalBonus += (max(iWeaponAdj + 3, 0) * iFireArc);
				break;
				}

			case itemcatReactor:
			case itemcatDrive:
			case itemcatCargoHold:
				//	Reactors don't count as improvements
				break;

			default:
				{
				//	Other devices are special abilities depending on level

				if (iDeviceLevel > iStdLevel+1)
					iExceptional++;
				else if (iDeviceLevel > iStdLevel)
					iSpecial += 4;
				else if (iDeviceLevel >= iStdLevel-1)
					iSpecial += 2;
				else
					iSpecial++;
				}
			}
		}

	//	If we have no weapons then we have some drawbacks

	if (iPrimaryWeapon == -1)
		iDrawback += 3;

	//	Add bonus if weapon is omnidirectional

	iSpecial += (int)((iDirectionalBonus / 270.0) + 0.5);

	//	Checkout AI settings

	const CAISettings &AI = GetAISettings();
	int iFireAccuracy = AI.GetFireAccuracy();
	int iFireRateAdj = AI.GetFireRateAdj();

	int iFireAccuracyScore, iFireRateScore;

	if (iFireAccuracy > 97)
		iFireAccuracyScore = 5;
	else if (iFireAccuracy >= 93)
		iFireAccuracyScore = 4;
	else if (iFireAccuracy >= 90)
		iFireAccuracyScore = 3;
	else if (iFireAccuracy < 75)
		iFireAccuracyScore = 1;
	else
		iFireAccuracyScore = 2;

	if (iFireRateAdj <= 10)
		iFireRateScore = 5;
	else if (iFireRateAdj <= 20)
		iFireRateScore = 4;
	else if (iFireRateAdj <= 30)
		iFireRateScore = 3;
	else if (iFireRateAdj >= 60)
		iFireRateScore = 1;
	else
		iFireRateScore = 2;

	int iFireControlScore = iFireRateScore * iFireAccuracyScore;
	if (iFireControlScore >= 20)
		iExceptional++;
	else if (iFireControlScore > 6)
		iSpecial += ((iFireControlScore - 5) / 2);
	else if (iFireControlScore < 2)
		iDrawback += 4;
	else if (iFireControlScore < 4)
		iDrawback += 2;

	//	Compute final score

	ScoreDesc *pBase = &g_XP[iStdLevel-1];
	int iScore = pBase->iBaseXP
			+ iSpecial * pBase->iSpecialXP
			+ iExceptional * pBase->iExceptionalXP
			+ iDrawback * pBase->iDrawbackXP;

	return iScore;
	}

bool CShipClass::CreateEmptyWreck (CSystem *pSystem, 
								   CShip *pShip,
								   const CVector &vPos, 
								   const CVector &vVel,
								   CSovereign *pSovereign, 
								   CStation **retpWreck)

//	CreateEmptyWreck
//
//	Create an empty wreck of the given ship class

	{
	return m_WreckDesc.CreateEmptyWreck(pSystem, this, pShip, vPos, vVel, pSovereign, retpWreck);
	}

void CShipClass::CreateImage (CG32bitImage &Dest, int iTick, int iRotation, Metric rScale)

//	CreateImage
//
//	Creates a scaled image of the given size, including all attached objects.

	{
	int i;

	if (!m_Image.GetSimpleImage().IsLoaded())
		return;

	//	Start by computing the total size of a normal image (and its origin).

	CVector vOrigin;
	int cxOriginalSize = m_Interior.CalcImageSize(this, &vOrigin);
	if (cxOriginalSize == 0)
		return;

	//	Compute the destination size

	int cxWidth = (int)mathRound(rScale * cxOriginalSize);
	int cyHeight = cxWidth;

	//	Create the destination

	Dest.Create(cxWidth, cyHeight, CG32bitImage::alpha8, CG32bitPixel::Null());
	int xCenter = cxWidth / 2;
	int yCenter = cyHeight / 2;

	//	Scale and rotate the origin

	vOrigin = rScale * (vOrigin.Rotate(iRotation));
	int xOrigin = xCenter + (int)mathRound(vOrigin.GetX());
	int yOrigin = yCenter -(int)mathRound(vOrigin.GetY());

	//	Get the positions of all attached components

	TArray<CVector> Pos;
	m_Interior.CalcCompartmentPositions(GetImageViewportSize(), Pos);

	//	Scale and rotate accordingly

	for (i = 0; i < Pos.GetCount(); i++)
		Pos[i] = rScale * Pos[i].Rotate(iRotation);

	//	Blt each compartment (in paint order)

	for (i = 0; i < m_Interior.GetPaintOrder().GetCount(); i++)
		{
		int iIndex = m_Interior.GetPaintOrder().GetAt(i);

		CShipClass *pClass = m_Interior.GetCompartment(iIndex).Class;
		if (pClass == NULL)
			continue;

		//	Paint the class on our image

		CVector vPos = vOrigin + Pos[iIndex];
		int xPos = xCenter + (int)mathRound(vPos.GetX());
		int yPos = yCenter -(int)mathRound(vPos.GetY());

		int cxScaledSize = (int)mathRound(rScale * pClass->m_Image.GetSimpleImage().GetImageWidth());
		int iDirection = pClass->GetRotationDesc().GetFrameIndex(iRotation);

		m_Image.GetSimpleImage().PaintScaledImage(Dest, xPos, yPos, iTick, iDirection, cxScaledSize, cxScaledSize, CObjectImageArray::FLAG_CACHED);
		}

	//	Blt the main image on top

	int cxScaledSize = (int)mathRound(rScale * m_Image.GetSimpleImage().GetImageWidth());
	int iDirection = GetRotationDesc().GetFrameIndex(iRotation);
	m_Image.GetSimpleImage().PaintScaledImage(Dest, xOrigin, yOrigin, iTick, iDirection, cxScaledSize, cxScaledSize, CObjectImageArray::FLAG_CACHED);
	}

void CShipClass::CreateScaledImage (CG32bitImage &Dest, int iTick, int iRotation, int cxWidth, int cyHeight)

//	CreateScaledImage
//
//	Creates a scaled image of the given size, including all attached objects.

	{
	int i;

	if (!m_Image.GetSimpleImage().IsLoaded())
		return;

	//	Create the destination

	Dest.Create(cxWidth, cyHeight, CG32bitImage::alpha8, CG32bitPixel::Null());
	int xCenter = cxWidth / 2;
	int yCenter = cyHeight / 2;

	//	Start by computing the total size of a normal image (and its origin).

	CVector vOrigin;
	int cxOriginalSize = m_Interior.CalcImageSize(this, &vOrigin);
	if (cxOriginalSize == 0)
		return;

	//	Compute the scale

	Metric rScale = Min(cxWidth, cyHeight) / (Metric)cxOriginalSize;

	//	Scale and rotate the origin

	vOrigin = rScale * (vOrigin.Rotate(iRotation));
	int xOrigin = xCenter + (int)mathRound(vOrigin.GetX());
	int yOrigin = yCenter -(int)mathRound(vOrigin.GetY());

	//	Get the positions of all attached components

	TArray<CVector> Pos;
	m_Interior.CalcCompartmentPositions(GetImageViewportSize(), Pos);

	//	Scale and rotate accordingly

	for (i = 0; i < Pos.GetCount(); i++)
		Pos[i] = rScale * Pos[i].Rotate(iRotation);

	//	Blt each compartment (in paint order)

	for (i = 0; i < m_Interior.GetPaintOrder().GetCount(); i++)
		{
		int iIndex = m_Interior.GetPaintOrder().GetAt(i);

		CShipClass *pClass = m_Interior.GetCompartment(iIndex).Class;
		if (pClass == NULL)
			continue;

		//	Paint the class on our image

		CVector vPos = vOrigin + Pos[iIndex];
		int xPos = xCenter + (int)mathRound(vPos.GetX());
		int yPos = yCenter -(int)mathRound(vPos.GetY());

		int cxScaledSize = (int)mathRound(rScale * pClass->m_Image.GetSimpleImage().GetImageWidth());
		int iDirection = pClass->GetRotationDesc().GetFrameIndex(iRotation);

		pClass->m_Image.GetSimpleImage().PaintScaledImage(Dest, xPos, yPos, iTick, iDirection, cxScaledSize, cxScaledSize, CObjectImageArray::FLAG_CACHED);
		}

	//	Blt the main image on top

	int cxScaledSize = (int)mathRound(rScale * m_Image.GetSimpleImage().GetImageWidth());
	int iDirection = GetRotationDesc().GetFrameIndex(iRotation);
	m_Image.GetSimpleImage().PaintScaledImage(Dest, xOrigin, yOrigin, iTick, iDirection, cxScaledSize, cxScaledSize, CObjectImageArray::FLAG_CACHED);
	}

bool CShipClass::CreateWreck (CShip *pShip, CSpaceObject **retpWreck)

//	CreateWreck
//
//	Creates a wreck for the given ship

	{
	return m_WreckDesc.CreateWreck(pShip, retpWreck);
	}

void CShipClass::FindBestMissile (CDeviceClass *pLauncher, IItemGenerator *pItems, CItemType **retpBestMissile) const

//	FindBestMissile
//
//	Finds the best missile in the table

	{
	int i;

	for (i = 0; i < pItems->GetItemTypeCount(); i++)
		{
		CItemType *pType = pItems->GetItemType(i);
		if (pType->GetCategory() == itemcatMissile
				&& (pLauncher->GetAmmoVariant(pType) != -1)
				&& (*retpBestMissile == NULL
					|| pType->GetLevel() > (*retpBestMissile)->GetLevel()))
			*retpBestMissile = pType;
		}

	//	Recurse

	for (i = 0; i < pItems->GetGeneratorCount(); i++)
		FindBestMissile(pLauncher, pItems->GetGenerator(i), retpBestMissile);
	}

void CShipClass::FindBestMissile (CDeviceClass *pLauncher, const CItemList &Items, CItemType **retpBestMissile) const

//	FindBestMissile
//
//	Finds the best missile in the item list

	{
	int i;

	for (i = 0; i < Items.GetCount(); i++)
		{
		CItemType *pType = Items.GetItem(i).GetType();
		if (pType->GetCategory() == itemcatMissile
				&& (pLauncher->GetAmmoVariant(pType) != -1)
				&& (*retpBestMissile == NULL
					|| pType->GetLevel() > (*retpBestMissile)->GetLevel()))
			*retpBestMissile = pType;
		}
	}

bool CShipClass::FindDataField (const CString &sField, CString *retsValue) const

//	FindDataField
//
//	Returns meta-data

	{
	int i;

	if (strEquals(sField, FIELD_ARMOR_COUNT))
		*retsValue = strFromInt(GetHullSectionCount());
	else if (strEquals(sField, FIELD_ARMOR_HP))
		{
		const CShipArmorSegmentDesc *pSection = (GetHullSectionCount() > 0 ? &GetHullSection(0) : NULL);
		if (pSection)
			pSection->GetArmorClass()->FindDataField(FIELD_HP, retsValue);
		else
			*retsValue = NULL_STR;
		return true;
		}
	else if (strEquals(sField, FIELD_BALANCE_TYPE))
		CalcBalanceType(retsValue);
	else if (strEquals(sField, FIELD_CARGO_SPACE))
		*retsValue = strFromInt(GetCargoDesc().GetCargoSpace());
	else if (strEquals(sField, FIELD_COMBAT_STRENGTH))
		*retsValue = strFromInt((int)(CalcCombatStrength() + 0.5));
	else if (strEquals(sField, FIELD_DAMAGE_RATE))
		*retsValue = strFromInt((int)(CalcDamageRate() + 0.5));
	else if (strEquals(sField, FIELD_DEFENSE_RATE))
		*retsValue = strFromInt((int)(CalcDefenseRate() + 0.5));
	else if (strEquals(sField, FIELD_DOCK_SERVICES_SCREEN))
		{
		const CPlayerSettings *pPlayer = GetPlayerSettings();
		if (pPlayer)
			*retsValue = pPlayer->GetDockServicesScreen().GetStringUNID(const_cast<CShipClass *>(this));
		else
			*retsValue = CONSTLIT("none");
		}
	else if (strEquals(sField, FIELD_DODGE_RATE))
		*retsValue = strFromInt((int)(100.0 * CalcDodgeRate()));

	else if (strEquals(sField, FIELD_INSTALL_DEVICE_MAX_LEVEL))
		{
		int iMaxLevel = (m_pTrade ? m_pTrade->GetMaxLevelMatched(serviceInstallDevice) : -1);
		*retsValue = (iMaxLevel != -1 ? strFromInt(iMaxLevel) : NULL_STR);
		}
	else if (strEquals(sField, FIELD_MANUFACTURER))
		*retsValue = m_sManufacturer;
	else if (strEquals(sField, FIELD_MASS))
		*retsValue = strFromInt(m_Hull.GetMass());
	else if (strEquals(sField, FIELD_MAX_ROTATION))
		*retsValue = strFromInt(mathRound(GetRotationDesc().GetMaxRotationSpeedDegrees()));
	else if (strEquals(sField, FIELD_MAX_SPEED))
		*retsValue = strFromInt((int)((100.0 * m_Perf.GetDriveDesc().GetMaxSpeed() / LIGHT_SPEED) + 0.5));
	else if (strEquals(sField, FIELD_MAX_STRUCTURAL_HP))
		*retsValue = strFromInt(m_Interior.GetHitPoints());
	else if (strEquals(sField, FIELD_NAME))
		*retsValue = GetNounPhrase(nounGeneric);
	else if (strEquals(sField, FIELD_PLAYER_DESC))
		{
		const CPlayerSettings *pPlayer = GetPlayerSettings();
		if (pPlayer)
			*retsValue = pPlayer->GetDesc();
		else
			*retsValue = CONSTLIT("none");
		}
	else if (strEquals(sField, FIELD_PRIMARY_ARMOR))
		{
		CItemType *pItem = g_pUniverse->FindItemType(strToInt(GetDataField(FIELD_PRIMARY_ARMOR_UNID), 0));
		if (pItem)
			*retsValue = pItem->GetNounPhrase(0x80);
		else
			*retsValue = CONSTLIT("none");
		}
	else if (strEquals(sField, FIELD_PRIMARY_ARMOR_UNID))
		{
		CArmorClass *pArmor = NULL;
        int iBestLevel;
		for (i = 0; i < GetHullSectionCount(); i++)
			{
			const CShipArmorSegmentDesc *pSeg = &GetHullSection(i);
            if (pArmor == NULL || pSeg->GetLevel() > iBestLevel)
                {
                pArmor = pSeg->GetArmorClass();
                iBestLevel = pSeg->GetLevel();
                }
			}
		if (pArmor)
			*retsValue = strFromInt(pArmor->GetItemType()->GetUNID());
		else
			*retsValue = CONSTLIT("none");
		}
	else if (strEquals(sField, FIELD_SHIELD))
		{
		CItemType *pItem = g_pUniverse->FindItemType(strToInt(GetDataField(FIELD_SHIELD_UNID), 0));
		if (pItem)
			*retsValue = pItem->GetNounPhrase(0x80);
		else
			*retsValue = CONSTLIT("none");
		}
	else if (strEquals(sField, FIELD_SHIELD_UNID))
		{
		CDeviceClass *pDevice = m_AverageDevices.GetNamedDevice(devShields);
		if (pDevice)
			*retsValue = strFromInt(pDevice->GetUNID());
		else
			*retsValue = CONSTLIT("none");
		}
	else if (strEquals(sField, FIELD_SHIP_CONFIG_SCREEN))
		{
		const CPlayerSettings *pPlayer = GetPlayerSettings();
		if (pPlayer)
			*retsValue = pPlayer->GetShipConfigScreen().GetStringUNID(const_cast<CShipClass *>(this));
		else
			*retsValue = CONSTLIT("none");
		}
	else if (strEquals(sField, FIELD_SHIP_STATUS_SCREEN))
		{
		const CPlayerSettings *pPlayer = GetPlayerSettings();
		if (pPlayer)
			*retsValue = pPlayer->GetShipScreen().GetStringUNID(const_cast<CShipClass *>(this));
		else
			*retsValue = CONSTLIT("none");
		}
	else if (strEquals(sField, FIELD_STARTING_SYSTEM))
		{
		const CPlayerSettings *pPlayer = GetPlayerSettings();
		if (pPlayer)
			{
			*retsValue = pPlayer->GetStartingNode();
			if (retsValue->IsBlank())
				*retsValue = g_pUniverse->GetCurrentAdventureDesc()->GetStartingNodeID();
			}
		else
			*retsValue = NULL_STR;
		}
	else if (strEquals(sField, FIELD_LAUNCHER))
		{
		CItemType *pItem = g_pUniverse->FindItemType(strToInt(GetDataField(FIELD_LAUNCHER_UNID), 0));
		if (pItem)
			*retsValue = pItem->GetNounPhrase(0x80);
		else
			*retsValue = CONSTLIT("none");
		}
	else if (strEquals(sField, FIELD_LAUNCHER_UNID))
		{
		CDeviceClass *pDevice = m_AverageDevices.GetNamedDevice(devMissileWeapon);
		if (pDevice)
			*retsValue = strFromInt(pDevice->GetUNID());
		else
			*retsValue = CONSTLIT("none");
		}
	else if (strEquals(sField, FIELD_PRIMARY_WEAPON))
		{
		CItemType *pItem = g_pUniverse->FindItemType(strToInt(GetDataField(FIELD_PRIMARY_WEAPON_UNID), 0));
		if (pItem)
			*retsValue = pItem->GetNounPhrase(0x80);
		else
			*retsValue = CONSTLIT("none");
		}
	else if (strEquals(sField, FIELD_PRIMARY_WEAPON_UNID))
		{
		CDeviceClass *pDevice = m_AverageDevices.GetNamedDevice(devPrimaryWeapon);
		if (pDevice)
			*retsValue = strFromInt(pDevice->GetUNID());
		else
			*retsValue = CONSTLIT("none");
		}
	else if (strEquals(sField, FIELD_EXPLOSION_TYPE))
		{
		CWeaponFireDesc *pExplosionType;
		if (pExplosionType = GetExplosionType(NULL))
			{
			CDeviceClass *pClass = g_pUniverse->FindDeviceClass((DWORD)strToInt(pExplosionType->GetUNID(), 0));
			CWeaponClass *pWeapon = (pClass ? pClass->AsWeaponClass() : NULL);
			if (pWeapon)
				{
				*retsValue = pWeapon->GetItemType()->GetNounPhrase();
				return true;
				}
			}

		*retsValue = CONSTLIT("none");
		}
	else if (strEquals(sField, FIELD_FIRE_ACCURACY))
		*retsValue = strFromInt(m_AISettings.GetFireAccuracy());
	else if (strEquals(sField, FIELD_FIRE_RANGE_ADJ))
		*retsValue = strFromInt(m_AISettings.GetFireRangeAdj());
	else if (strEquals(sField, FIELD_FIRE_RATE_ADJ))
		*retsValue = strFromInt((int)((1000.0 / m_AISettings.GetFireRateAdj()) + 0.5));
	else if (strEquals(sField, FIELD_MANEUVER))
		{
		Metric rManeuver = g_SecondsPerUpdate * GetRotationDesc().GetMaxRotationSpeedDegrees();
		*retsValue = strFromInt((int)((rManeuver * 1000.0) + 0.5));
		}
	else if (strEquals(sField, FIELD_THRUST))
		*retsValue = strFromInt(m_Perf.GetDriveDesc().GetThrust());
	else if (strEquals(sField, FIELD_THRUST_TO_WEIGHT))
		{
		Metric rMass = CalcMass(m_AverageDevices);
		int iRatio = (int)((200.0 * (rMass > 0.0 ? m_Perf.GetDriveDesc().GetThrust() / rMass : 0.0)) + 0.5);
		*retsValue = strFromInt(10 * iRatio);
		}
	else if (strEquals(sField, FIELD_TREASURE_VALUE))
		*retsValue = strFromInt(m_pItems ? (int)m_pItems->GetAverageValue(GetLevel()) : 0);

	else if (strEquals(sField, FIELD_WRECK_CHANCE))
		*retsValue = strFromInt(m_WreckDesc.GetWreckChance());

	else if (strEquals(sField, FIELD_PRIMARY_WEAPON_RANGE))
		{
		int iRange = 0;
		CItemType *pItem = g_pUniverse->FindItemType(strToInt(GetDataField(FIELD_PRIMARY_WEAPON_UNID), 0));
		if (pItem)
			{
			CDeviceClass *pDevice = pItem->GetDeviceClass();
			if (pDevice)
				{
				CWeaponClass *pWeapon = pDevice->AsWeaponClass();
				if (pWeapon)
					iRange = (int)((pWeapon->GetMaxEffectiveRange(NULL, NULL, NULL) / LIGHT_SECOND) + 0.5);
				}
			}
		*retsValue = strFromInt(iRange);
		}
	else if (strEquals(sField, FIELD_PRIMARY_WEAPON_RANGE_ADJ))
		{
		int iRange = 0;
		CItemType *pItem = g_pUniverse->FindItemType(strToInt(GetDataField(FIELD_PRIMARY_WEAPON_UNID), 0));
		if (pItem)
			{
			CDeviceClass *pDevice = pItem->GetDeviceClass();
			if (pDevice)
				{
				CWeaponClass *pWeapon = pDevice->AsWeaponClass();
				if (pWeapon)
					{
					iRange = (int)((pWeapon->GetMaxEffectiveRange(NULL, NULL, NULL) / LIGHT_SECOND) + 0.5);
					iRange = iRange * m_AISettings.GetFireRangeAdj() / 100;
					}
				}
			}
		*retsValue = strFromInt(iRange);
		}
	else if (strEquals(sField, FIELD_SCORE))
		*retsValue = strFromInt(m_iScore);
	else if (strEquals(sField, FIELD_SIZE))
		*retsValue = strFromInt(m_Hull.GetSize());
	else if (strEquals(sField, FIELD_LEVEL))
		*retsValue = strFromInt(m_iLevel);
	else if (strEquals(sField, FIELD_MAX_CARGO_SPACE))
		*retsValue = strFromInt(m_Hull.GetMaxCargoSpace());
	else if (strEquals(sField, FIELD_GENERIC_NAME))
		*retsValue = GetGenericName();
	else if (strEquals(sField, FIELD_MAX_ARMOR_MASS))
		*retsValue = strFromInt(m_Hull.GetMaxArmorMass());
	else if (strEquals(sField, FIELD_HULL_MASS))
		*retsValue = strFromInt(m_Hull.GetMass());
	else if (strEquals(sField, FIELD_DEVICE_SLOTS))
		{
		int iSlots = m_Hull.GetMaxDevices();
		if (iSlots == -1)
			iSlots = m_AverageDevices.GetCount();

		*retsValue = strFromInt(iSlots);
		}
	else if (strEquals(sField, FIELD_DEVICE_SLOTS_NON_WEAPONS))
		{
		int iSlots = m_Hull.GetMaxNonWeapons();
		if (iSlots == -1)
			return FindDataField(FIELD_DEVICE_SLOTS, retsValue);

		*retsValue = strFromInt(iSlots);
		}
	else if (strEquals(sField, FIELD_DEVICE_SLOTS_WEAPONS))
		{
		int iSlots = m_Hull.GetMaxWeapons();
		if (iSlots == -1)
			return FindDataField(FIELD_DEVICE_SLOTS, retsValue);

		*retsValue = strFromInt(iSlots);
		}
	else if (strEquals(sField, FIELD_ARMOR_ITEMS))
		{
		//	Generate a list of armor items

		TArray<CItem> Items;
		for (i = 0; i < GetHullSectionCount(); i++)
			{
			const CShipArmorSegmentDesc &Seg = GetHullSection(i);
            CItem theItem;
            Seg.CreateArmorItem(&theItem);

			Items.Insert(theItem);
			}

		//	Convert to a data field

		*retsValue = CreateDataFieldFromItemList(Items);
		}
	else if (strEquals(sField, FIELD_DEVICE_ITEMS))
		{
		//	Generate a list of device items

		TArray<CItem> Items;
		for (i = 0; i < m_AverageDevices.GetCount(); i++)
			{
			const SDeviceDesc &Desc = m_AverageDevices.GetDeviceDesc(i);
			Items.Insert(Desc.Item);
			}

		//	Convert to a data field

		*retsValue = CreateDataFieldFromItemList(Items);
		}
	else if (strEquals(sField, FIELD_DRIVE_IMAGE))
		{
		if (m_Exhaust.GetCount() == 0)
			*retsValue = CONSTLIT("none");
		else
			*retsValue = CONSTLIT("Image");
		}
	else if (strEquals(sField, FIELD_DRIVE_POWER))
		*retsValue = strFromInt(m_Perf.GetDriveDesc().GetPowerUse());

	else if (m_Perf.GetReactorDesc().FindDataField(sField, retsValue))
		return true;
	else
		return CDesignType::FindDataField(sField, retsValue);

	return true;
	}

bool CShipClass::FindDeviceSlotDesc (DeviceNames iDev, SDeviceDesc *retDesc) const

//	FindDeviceSlotDesc
//
//	Looks for a device slot descriptor

	{
	//	If we have a dedicated device slot object, then use that.

	if (m_pDeviceSlots)
		return m_pDeviceSlots->FindDefaultDesc(iDev, retDesc);

	//	Otherwise, for backwards compatibility we check the device generator.

	else if (m_pDevices)
		return m_pDevices->FindDefaultDesc(iDev, retDesc);

	//	Otherwise, not found

	return false;
	}

bool CShipClass::FindDeviceSlotDesc (CShip *pShip, const CItem &Item, SDeviceDesc *retDesc) const

//	FindDeviceSlotDesc
//
//	Looks for a device slot descriptor

	{
	//	If we have a dedicated device slot object, then use that.

	if (m_pDeviceSlots)
		return m_pDeviceSlots->FindDefaultDesc(pShip, Item, retDesc);

	//	Otherwise, for backwards compatibility we check the device generator.

	else if (m_pDevices)
		return m_pDevices->FindDefaultDesc(pShip, Item, retDesc);

	//	Otherwise, not found

	return false;
	}

void CShipClass::GenerateDevices (int iLevel, CDeviceDescList &Devices, DWORD dwFlags)

//	GenerateDevices
//
//	Generate a list of devices
	
	{
	DEBUG_TRY

	Devices.DeleteAll();

	if (m_pDevices)
		{
		SDeviceGenerateCtx Ctx;
		Ctx.iLevel = iLevel;

		if (!(dwFlags & GDFLAG_NO_DEVICE_SLOT_SEARCH))
			Ctx.pRoot = (m_pDeviceSlots ? m_pDeviceSlots : m_pDevices);

		Ctx.pResult = &Devices;

		m_pDevices->AddDevices(Ctx);
		}

	DEBUG_CATCH
	}

CString CShipClass::GenerateShipName (DWORD *retdwFlags) const

//	GenerateShipName
//
//	Generate a ship name

	{
	if (!m_sShipNames.IsBlank())
		{
		if (retdwFlags)
			*retdwFlags = m_dwShipNameFlags;

		if (m_ShipNamesIndices.GetCount() > 0)
			{
			CString sNameTemplate = strDelimitGet(
					m_sShipNames,
					';',
					DELIMIT_TRIM_WHITESPACE,
					m_ShipNamesIndices[m_iShipName++ % m_ShipNamesIndices.GetCount()]);

			return GenerateRandomNameFromTemplate(sNameTemplate);
			}
		else
			return GenerateRandomName(m_sShipNames, NULL_STR);
		}
	else
		{
		if (retdwFlags)
			*retdwFlags = 0;
		return NULL_STR;
		}
	}

const CCargoDesc &CShipClass::GetCargoDesc (const CItem **retpCargoItem) const

//  GetCargoDesc
//
//  Returns the computed cargo descriptor

    {
    if (retpCargoItem)
        {
        const SDeviceDesc *pCargo = m_AverageDevices.GetDeviceDescByName(devCargo);
        *retpCargoItem = (pCargo ? &pCargo->Item : NULL);
        }

    return m_Perf.GetCargoDesc();
    }

DWORD CShipClass::GetCategoryFlags (void) const

//	GetCategoryFlags
//
//	Returns flags describing various ship categories.

	{
	DWORD dwFlags = 0;

	//	Are we a capital ship?

	ScoreDesc *pBase = &g_XP[GetLevel() - 1];
	if (m_Hull.GetMass() >= pBase->iMinCapitalShipMass)
		dwFlags |= catCapitalShip;

	//	Done

	return dwFlags;
	}

CCommunicationsHandler *CShipClass::GetCommsHandler (void)

//	GetCommsHandler
//
//	Returns the comms handler to use

	{
	CDesignType *pParent = GetInheritFrom();
	CCommunicationsHandler *pParentHandler;

	if (pParent && (pParentHandler = pParent->GetCommsHandler()))
		{
		if (!m_fCommsHandlerInit)
			{
			m_CommsHandler.Merge(m_OriginalCommsHandler);
			m_CommsHandler.Merge(*pParentHandler);

			m_fCommsHandlerInit = true;
			}

		return (m_CommsHandler.GetCount() ? &m_CommsHandler : NULL);
		}
	else
		return (m_OriginalCommsHandler.GetCount() ? &m_OriginalCommsHandler : NULL);
	}

CVector CShipClass::GetDockingPortOffset (int iRotation)

//	GetDockingPortOffset
//
//	Returns an offset to the ship center from the desired position of the docking
//	port when the ship is docked.
//
//	[In other words, the resulting vector points from the station's docking port
//	to the ship center.]

	{
	//	For small ships we just go with the ship center.

    int iImageSize = RectWidth(m_Image.GetSimpleImage().GetImageRect());
	if (iImageSize <= DOCK_OFFSET_STD_SIZE)
		return NullVector;

	return PolarToVector(iRotation + 180, (0.8 * g_KlicksPerPixel * ((iImageSize - DOCK_OFFSET_STD_SIZE) / 2)));
	}

const CDriveDesc &CShipClass::GetDriveDesc (const CItem **retpDriveItem) const

//  GetDriveDesc
//
//  Returns the computed drive desc (including devices)

    {
    if (retpDriveItem)
        {
        const SDeviceDesc *pDrive = m_AverageDevices.GetDeviceDescByName(devDrive);
        *retpDriveItem = (pDrive ? &pDrive->Item : NULL);
        }

    return m_Perf.GetDriveDesc();
    }

CEconomyType *CShipClass::GetEconomyType (void) const

//	GetEconomyType
//
//	Returns the economy type used by this ship.

	{
	//	If we have a trading descriptor, use that.

	CTradingDesc *pTrade = GetTradingDesc();
	if (pTrade)
		return pTrade->GetEconomyType();

	//	Otherwise, see if we have a hull price from the player settings

	CEconomyType *pCurrency = m_Hull.GetValue().GetCurrencyType();
	if (pCurrency)
		return pCurrency;

	//	Otherwise, default to credits

	return CEconomyType::AsType(g_pUniverse->FindDesignType(DEFAULT_ECONOMY_UNID));
	}

CWeaponFireDesc *CShipClass::GetExplosionType (CShip *pShip) const

//	GetExplosionType
//
//	Returns the explosion type (or NULL if we have no default explosion for this
//	ship class).
	
	{
	//	If we've got a defined explosion, then return that

	if (m_WreckDesc.GetExplosionType())
		return m_WreckDesc.GetExplosionType();

	//	If no defined explosion, come up with an appropriate one.
	//
	//	NOTE: In general, this will only happen for smaller gunships.
	//	Capital ships should define their own explosion type.

	int iLevel = (pShip ? pShip->GetLevel() : GetLevel());
	int iMinMassLevel = 0;
	int iMaxMassLevel = 2;

	//	Adjust the level based on the balance type.

	switch (m_iLevelType)
		{
		case typeUnknown:
		case typeMinion:
		case typeTooWeak:
		case typeNonCombatant:
		case typeArmorTooWeak:
		case typeWeaponsTooWeak:
			iLevel = Max(1, iLevel - 3);
			iMaxMassLevel = 0;
			break;

		case typeStandard:
			if (m_Hull.GetMass() < 750)
				iMaxMassLevel = 0;
			else
				iMaxMassLevel = 1;
			break;

		case typeElite:
			if (m_Hull.GetMass() < 750)
				iMaxMassLevel = 1;
			else
				{
				iMinMassLevel = 1;
				iMaxMassLevel = 2;
				}
			break;
		}

	//	Compute the size of the explosion in the given tier based on where
	//	we are on the tier (afer adjustments).

	int iMassLevel = Max(iMinMassLevel, Min(iMaxMassLevel, (iLevel - 1) % 3));

	//	We figure out which explosion type based on the level.

	CWeaponFireDescRef ExplosionRef;
	switch (iLevel)
		{
		//	Tier 1

		case 0:
		case 1:
		case 2:
		case 3:
			ExplosionRef.SetUNID(UNID_KINETIC_EXPLOSION_1 + iMassLevel);
			break;

		//	Tier 2

		case 4:
		case 5:
		case 6:
			ExplosionRef.SetUNID(UNID_BLAST_EXPLOSION_1 + iMassLevel);
			break;

		//	Tier 3

		case 7:
		case 8:
		case 9:
			ExplosionRef.SetUNID(UNID_THERMO_EXPLOSION_1 + iMassLevel);
			break;

		//	Tier 4

		case 10:
		case 11:
		case 12:
			ExplosionRef.SetUNID(UNID_PLASMA_EXPLOSION_1 + iMassLevel);
			break;

		//	Tier 5

		case 13:
		case 14:
		case 15:
			ExplosionRef.SetUNID(UNID_ANTIMATTER_EXPLOSION_1 + iMassLevel);
			break;

		//	Tier 6

		case 16:
		case 17:
		case 18:
			ExplosionRef.SetUNID(UNID_GRAVITON_EXPLOSION_1 + iMassLevel);
			break;

		//	Tier 7+

		default:
			ExplosionRef.SetUNID(UNID_GRAVITON_EXPLOSION_1 + 3);
			break;
		}

	//	Bind. NOTE: It's OK if we don't find the given explosion and return
	//	NULL.

	SDesignLoadCtx LoadCtx;
	ExplosionRef.Bind(LoadCtx);
	return ExplosionRef;
	}

CString CShipClass::GetGenericName (DWORD *retdwFlags) const

//	GetGenericName
//
//	Returns the generic name of the ship class

	{
	if (retdwFlags)
		*retdwFlags = m_dwClassNameFlags;

	if (GetClassName().IsBlank())
		return strPatternSubst(CONSTLIT("%s %s"), GetManufacturerName(), GetShipTypeName());
	else
		{
		if (GetShipTypeName().IsBlank())
			return GetClassName();

		//	If we're asking for noun flags, then it likely means that we will 
		//	use ComposeNounPhrase. In that case, return the short-name syntax.

		else if (retdwFlags)
			{
			//	We need this flag because otherwise the code assumes that 
			//	long-forms are mass items (e.g., barrel of grain).

			*retdwFlags |= nounPluralizeLongForm;
			return strPatternSubst(CONSTLIT("%s[-class %s]"), GetClassName(), GetShipTypeName());
			}

		//	Otherwise, just the class name

		else
			return strPatternSubst(CONSTLIT("%s-class %s"), GetClassName(), GetShipTypeName());

		}
	}

const CObjectImageArray &CShipClass::GetHeroImage (void)

//  GetHeroImage
//
//  Returns the hero image, suitable for dock screens.

    {
    //  If we don't have a hero image, try to create one from the player setting's
    //  large image.

    const CPlayerSettings *pPlayerSettings;
    DWORD dwImageUNID;
	TSharedPtr<CObjectImage> pLargeImageObj;
    CG32bitImage *pLargeImage;
    if (m_HeroImage.IsEmpty()
            && (pPlayerSettings = GetPlayerSettings())
            && (dwImageUNID = pPlayerSettings->GetLargeImage())
            && (pLargeImageObj = g_pUniverse->FindLibraryImage(dwImageUNID))
			&& (pLargeImage = pLargeImageObj->GetRawImage(strPatternSubst(CONSTLIT("%08x hero image"), GetUNID())))
            && !pLargeImage->IsEmpty())
        {
        //  If necessary, we scale it down to fit the dock screen.

        if (pLargeImage->GetHeight() > MAX_HERO_IMAGE_HEIGHT)
            {
			Metric rScale = (Metric)MAX_HERO_IMAGE_HEIGHT / pLargeImage->GetHeight();
			CG32bitImage *pNewImage = new CG32bitImage;
			pNewImage->CreateFromImageTransformed(*pLargeImage,
					0,
					0,
					pLargeImage->GetWidth(),
					pLargeImage->GetHeight(),
					rScale,
					rScale,
					0.0);

            RECT rcImage;
            rcImage.left = 0;
            rcImage.top = 0;
            rcImage.right = pNewImage->GetWidth();
            rcImage.bottom = pNewImage->GetHeight();

            m_HeroImage.InitFromBitmap(pNewImage, rcImage, 1, 1, true);
            }
        else
            {
            RECT rcImage;
            rcImage.left = 0;
            rcImage.top = 0;
            rcImage.right = pLargeImage->GetWidth();
            rcImage.bottom = pLargeImage->GetHeight();

            m_HeroImage.Init(pLargeImageObj, rcImage, 1, 1);
            }
        }

    //  Return the hero image (even if blank)

    return m_HeroImage;
    }

int CShipClass::GetHullSectionAtAngle (int iAngle)

//	GetHullSectionAtAngle
//
//	Returns the index of the hull section at the given
//	angle.

	{
    return m_Armor.GetSegmentAtAngle(iAngle);
	}

CString CShipClass::GetHullSectionName (int iIndex) const

//	GetHullSectionName
//
//	Returns the name of this section:
//
//	forward
//	starboard
//	port
//	aft

	{
    return m_Armor.GetSegmentName(iIndex);
	}

CCurrencyAndValue CShipClass::GetHullValue (CShip *pShip) const

//	GetHullValue
//
//	Returns the value of just the hull.

	{
	CCodeChainCtx Ctx;
	SEventHandlerDesc Event;

	//	If we're already inside the <GetHullValue> event, of if we don't have 
	//	such an event, then just return the raw value.

	if (!FindEventHandler(CONSTLIT("GetHullValue"), &Event)
			|| Ctx.InEvent(eventGetHullPrice))
		return GetHullDesc().GetValue();

	//	We pass the raw value in

	CCurrencyAndValue HullValue = GetHullDesc().GetValue();

	//	Otherwise, if we run the event to get the value

	Ctx.DefineContainingType(pShip);
	Ctx.SaveAndDefineSourceVar(pShip);
	Ctx.SetEvent(eventGetHullPrice);
	Ctx.DefineString(CONSTLIT("aCurrency"), HullValue.GetSID());
	Ctx.DefineInteger(CONSTLIT("aPrice"), (int)HullValue.GetValue());

	//	Run

	ICCItem *pResult = Ctx.Run(Event);

	//	Interpret results

	if (pResult->IsError())
		::kernelDebugLogPattern("GetHullValue: %s", pResult->GetStringValue());

	else if (pResult->IsNumber())
		HullValue.SetValue(pResult->GetIntegerValue());

	//	Done

	Ctx.Discard(pResult);
	return HullValue;
	}

const CObjectImageArray &CShipClass::GetImage (const CImageFilterStack *pFilters) const

//	GetImage
//
//	Returns the image for the class.

	{
	if (pFilters == NULL)
		return m_Image.GetSimpleImage();
	
	CCompositeImageModifiers Modifiers;
	Modifiers.SetFullImage();
	Modifiers.SetFilters(pFilters);

	return m_Image.GetImage(CCompositeImageSelector::Null(), Modifiers);
	}

int CShipClass::GetMaxStructuralHitPoints (void) const

//	GetMaxStructuralHitPoints
//
//	Returns the HP of the wreck created by this class

	{
	//	If it is set, return that
	
	if (m_WreckDesc.GetStructuralHP())
		return m_WreckDesc.GetStructuralHP();

	//	Otherwise we have to compute it based on level and mass

	return (int)(pow(1.3, m_iLevel) * (sqrt(m_Hull.GetMass()) + 10.0));
	}

CString CShipClass::GetNamePattern (DWORD dwNounFormFlags, DWORD *retdwFlags) const

//	GetNamePattern
//
//	Returns the name pattern

	{
	if (dwNounFormFlags & nounShort)
		{
		if (retdwFlags)
			*retdwFlags = m_dwClassNameFlags;

		return GetShortName();
		}
	else if ((dwNounFormFlags & nounGeneric) || m_sShipNames.IsBlank())
		return GetGenericName(retdwFlags);
	else
		return GenerateShipName(retdwFlags);
	}

const CPlayerSettings *CShipClass::GetPlayerSettings (void) const

//  GetPlayerSettings
//
//  There are three possible cases for a given ship class:
//
//  1.  We inherit ALL our player settings from our base class (if any).
//      After loading, m_pPlayerSettings is NULL. Here, we set m_pPlayerSettings
//      to whatever our base class has, and we set m_fInheritedPlayerSettings
//      to TRUE. At bind-time, we undo this so that we can pull from the
//      proper base class.
//
//  2.  We inherit SOME of our player settings from our base class (if any).
//      m_pPlayerSettings is valid, but not yet resolved. Here we ask for our
//      base class player settings and resolve our m_pPlayerSettings object
//      (NOTE: Our caller has to deal with missing elements). At bind-time, we
//      unresolve.
//
//  3.  Our m_pPlayerSettings is valid and complete. In that case, we just 
//      return it.

    {
    //  If we have a resolved set of player settings, we're done.

    if (m_pPlayerSettings && m_pPlayerSettings->IsResolved())
        return m_pPlayerSettings;

    //  Get our base class because we're going to need it one way or the other.
    //  If we don't have a base class, then we just return whatever we have.
    //  (Which may be either NULL or incomplete).

    CShipClass *pBase = CShipClass::AsType(GetInheritFrom());
    const CPlayerSettings *pBaseSettings = (pBase ? pBase->GetPlayerSettings() : NULL);

    //  If we have player settings, then resolve against the base class so that
    //  we can inherit.
    //
    //  NOTE: It's OK if pBaseSettings is NULL--at least we will mark m_pPlayerSettings
    //  as resolved.

    if (m_fOwnPlayerSettings)
        {
        m_pPlayerSettings->Resolve(pBaseSettings);
        return m_pPlayerSettings;
        }

    //  Otherwise, we inherit all our settings from our base class

    else if (pBaseSettings)
        {
        m_pPlayerSettings = const_cast<CPlayerSettings *>(pBaseSettings);
        return m_pPlayerSettings;
        }

	//	Otherwise, we use default settings

	else
		{
        m_pPlayerSettings = const_cast<CPlayerSettings *>(&m_DefaultPlayerSettings);
        return m_pPlayerSettings;
		}
    }

CString CShipClass::GetPlayerSortString (void) const

//	GetPlayerSortString
//
//	Returns a sort string ordering the player ships by order in which they should
//	be shown to the player.
//
//	Sort should be ascending.

	{
	CExtension *pExtension = GetExtension();
	const CPlayerSettings *pPlayerSettings = GetPlayerSettings();

	//	Official extensions, followed by registered, followed by unregistered.

	int iDomain;
	if (IsDebugOnly())
		iDomain = 9;
	else if (pExtension)
		{
		if (pExtension->IsRegistered())
			{
			if (pExtension->IsOfficial())
				iDomain = 1;
			else
				iDomain = 2;
			}
		else
			iDomain = 5;
		}
	else
		iDomain = 9;

	//	Combine

	return strPatternSubst(CONSTLIT("%d-%06d-%s-%08x"), 
			iDomain, 
			(pPlayerSettings ? pPlayerSettings->GetSortOrder() : 10000),
			GetShortName(), 
			GetUNID());
	}

CVector CShipClass::GetPosOffset (int iAngle, int iRadius, int iPosZ, bool b3DPos)

//	GetPosOffset
//
//	Returns a vector offset

	{
	if (b3DPos)
		{
		int iScale = GetImageViewportSize();

		CVector vOffset;
		C3DConversion::CalcCoord(iScale, 90 + iAngle, iRadius, iPosZ, &vOffset);

		return vOffset;
		}
	else if (iRadius)
		{
		return PolarToVector((iAngle + 90) % 360,
					iRadius * g_KlicksPerPixel);
		}
	else
		return CVector();
	}

const CReactorDesc &CShipClass::GetReactorDesc (const CItem **retpReactorItem) const

//  GetReactorDesc
//
//  Returns the computed reactor desc (including devices)

    {
    if (retpReactorItem)
        {
        const SDeviceDesc *pDevice = m_AverageDevices.GetDeviceDescByName(devReactor);
        *retpReactorItem = (pDevice ? &pDevice->Item : NULL);
        }

    return m_Perf.GetReactorDesc();
    }

CString CShipClass::GetShortName (void) const

//	GetShortName
//
//	Returns the short name of the class

	{
	if (GetClassName().IsBlank())
		return strPatternSubst(LITERAL("%s %s"), GetManufacturerName(), GetShipTypeName());
	else
		return GetClassName();
	}

CCurrencyAndValue CShipClass::GetTradePrice (CSpaceObject *pObj, bool bActual) const

//	GetTradePrice
//
//	Returns the price computed by pObj for this ship class.

	{
	int i;

	//	Get the hull value

	CCurrencyAndValue Value = GetHullValue();

	//	Add up the value of all installed items

	for (i = 0; i < m_AverageDevices.GetCount(); i++)
		{
		const SDeviceDesc &Desc = m_AverageDevices.GetDeviceDesc(i);

		//	We use the raw (actual) value because not all stations sell all items.

		Value.Add(CCurrencyAndValue(Desc.Item.GetTradePrice(pObj, true), Desc.Item.GetCurrencyType()));

		//	Need to include install cost

		Value.Add(CCurrencyAndValue(Desc.Item.GetType()->GetInstallCost(CItemCtx(Desc.Item)), Desc.Item.GetCurrencyType()));
		}

	//	Done

	return Value;
	}

void CShipClass::InitEffects (CShip *pShip, CObjectEffectList *retEffects)

//	InitEffects
//
//	Initializes effects (like thrust, etc.)

	{
	DEBUG_TRY

	int i;

	const CObjectEffectDesc &Effects = GetEffectsDesc();
	if (Effects.GetEffectCount() > 0)
		{
		TArray<IEffectPainter *> Painters;
		Painters.InsertEmpty(Effects.GetEffectCount());

		//	Compute the thrust and max speed

		int iThrust;
		Metric rMaxSpeed;
		if (pShip)
			{
			iThrust = (int)pShip->GetThrust();
			rMaxSpeed = pShip->GetMaxSpeed();
			}
		else
			{
			iThrust = m_DriveDesc.GetThrust();
			rMaxSpeed = m_DriveDesc.GetMaxSpeed();
			}

		//	Compute power of maneuvering thrusters

		int iThrustersPerSide = Max(1, Effects.GetEffectCount(CObjectEffectDesc::effectThrustLeft));
		int iThrusterPower = Max(1, mathRound((m_Hull.GetMass() / iThrustersPerSide) * m_RotationDesc.GetRotationAccelPerTick()));

		//	Compute power of main thruster

		int iMainThrusters = Max(1, Effects.GetEffectCount(CObjectEffectDesc::effectThrustMain));
		int iMainPower = Max(1, iThrust / iMainThrusters);
		int iMaxSpeed = mathRound(100.0 * rMaxSpeed / LIGHT_SPEED);

		//	Create painters

		for (i = 0; i < Effects.GetEffectCount(); i++)
			{
			const CObjectEffectDesc::SEffectDesc &EffectDesc = Effects.GetEffectDesc(i);

			switch (EffectDesc.iType)
				{
				case CObjectEffectDesc::effectThrustLeft:
				case CObjectEffectDesc::effectThrustRight:
					{
					//	For now we only create maneuvering thruster effects on 
					//	player ships

					if (IsPlayerShip())
						{
						CCreatePainterCtx CreateCtx;
						CreateCtx.SetAPIVersion(GetAPIVersion());
						CreateCtx.AddDataInteger(FIELD_THRUSTER_POWER, iThrusterPower);
						CreateCtx.SetUseObjectCenter();
						Painters[i] = Effects.CreatePainter(CreateCtx, i);
						}
					else
						Painters[i] = NULL;
					break;
					}

				case CObjectEffectDesc::effectThrustMain:
					{
					CCreatePainterCtx CreateCtx;
					CreateCtx.SetAPIVersion(GetAPIVersion());
					CreateCtx.AddDataInteger(FIELD_MAX_SPEED, iMaxSpeed);
					CreateCtx.AddDataInteger(FIELD_THRUSTER_POWER, iMainPower);
					CreateCtx.SetUseObjectCenter();
					Painters[i] = Effects.CreatePainter(CreateCtx, i);
					break;
					}

				default:
					Painters[i] = NULL;
					break;
				}
			}

		//	Initialize
		//	NOTE: the m_Effects structure takes ownership of all painters.

		retEffects->Init(Effects, Painters);
		}

	DEBUG_CATCH
	}

void CShipClass::InitPerformance (SShipPerformanceCtx &Ctx) const

//  InitPerformance
//
//  Initializes the performance parameters from the ship class.

    {
    //  Initialize some values from the class

    Ctx.rSingleArmorFraction = (m_Armor.GetCount() > 0 ? 1.0 / m_Armor.GetCount() : 1.0);
    Ctx.iMaxCargoSpace = m_Hull.GetMaxCargoSpace();

    //  Initialize with performance params based on the class.

    Ctx.RotationDesc = m_RotationDesc;
	Ctx.ReactorDesc = m_ReactorDesc;
    Ctx.DriveDesc = m_DriveDesc;
    Ctx.CargoDesc = CCargoDesc(m_Hull.GetCargoSpace());

	//	Track maximum speed after bonuses. We start with the class speed; 
	//	devices and other items should increase this in their handling of
	//	AccumulatePerformance.

	Ctx.rMaxSpeedLimit = m_DriveDesc.GetMaxSpeed();
    }

void CShipClass::InitShipNamesIndices (void)

//	InitShipNamesIndices
//
//	Initializes m_ShipNamesIndices and m_iShipName from m_sShipNames

	{
	int i;
	int iCount = strDelimitCount(m_sShipNames, ';', DELIMIT_TRIM_WHITESPACE);

	m_ShipNamesIndices.DeleteAll();
	m_ShipNamesIndices.InsertEmpty(iCount);
	for (i = 0; i < iCount; i++)
		m_ShipNamesIndices[i] = i;

	m_ShipNamesIndices.Shuffle();

	m_iShipName = 0;
	}

void CShipClass::InstallEquipment (CShip *pShip)

//	InstallEquipment
//
//	Install initial equipment on the given ship
//
//	NOTE: We assume that the ship was just created, so we assume that equipment
//	is at default state.

	{
	int i;

	//	Give our base class a chance to install/remove

	CShipClass *pBase = CShipClass::AsType(GetInheritFrom());
	if (pBase)
		pBase->InstallEquipment(pShip);

	//	Handle our part

	for (i = 0; i < m_Equipment.GetCount(); i++)
		{
		if (m_Equipment[i].bInstall)
			pShip->SetAbility(m_Equipment[i].iEquipment, ablInstall, -1, 0);
		else
			pShip->SetAbility(m_Equipment[i].iEquipment, ablRemove, -1, 0);
		}
	}

void CShipClass::UnbindGlobal (void)

//	UnbindGlobal
//
//	We're unbinding, which means we have to release any resources.

	{
	CShipwreckDesc::UnbindGlobal();
	m_bDefaultPlayerSettingsBound = false;
	}

void CShipClass::MarkImages (bool bMarkDevices)

//	MarkImages
//
//	Marks images used by the ship

	{
	DEBUG_TRY

	int i;

	m_Image.MarkImage();
    m_HeroImage.MarkImage();

	//	If necessary mark images for all our installed devices

	if (bMarkDevices)
		{
		for (i = 0; i < m_AverageDevices.GetCount(); i++)
			{
			CDeviceClass *pDevice = m_AverageDevices.GetDeviceClass(i);
			pDevice->MarkImages();
			}
		}

	//	Effects

	m_Effects.MarkImages();

    //  Settings

    const CPlayerSettings *pPlayerSettings = GetPlayerSettings();
    if (pPlayerSettings)
        pPlayerSettings->MarkImages();

	DEBUG_CATCH
	}

void CShipClass::OnAccumulateXMLMergeFlags (TSortMap<DWORD, DWORD> &MergeFlags) const

//	OnAccumulateXMLMergeFlags
//
//	Returns flags to determine how we merge from inherited types.

	{
	//	We know how to handle these tags through the inheritance hierarchy.

	MergeFlags.SetAt(CXMLElement::GetKeywordID(COMMUNICATIONS_TAG), CXMLElement::MERGE_OVERRIDE);
	MergeFlags.SetAt(CXMLElement::GetKeywordID(PLAYER_SETTINGS_TAG), CXMLElement::MERGE_OVERRIDE);
	MergeFlags.SetAt(CXMLElement::GetKeywordID(EQUIPMENT_TAG), CXMLElement::MERGE_OVERRIDE);
	}

void CShipClass::OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed)

//	OnAddTypesUsed
//
//	Adds types used by the class

	{
	retTypesUsed->SetAt(m_pDefaultSovereign.GetUNID(), true);

    m_Armor.AddTypesUsed(retTypesUsed);
	m_WreckDesc.AddTypesUsed(retTypesUsed);
	m_Image.AddTypesUsed(retTypesUsed);

	if (m_pDeviceSlots)
		m_pDeviceSlots->AddTypesUsed(retTypesUsed);

	if (m_pDevices)
		m_pDevices->AddTypesUsed(retTypesUsed);

    const CPlayerSettings *pPlayerSettings = GetPlayerSettings();
	if (pPlayerSettings)
		pPlayerSettings->AddTypesUsed(retTypesUsed);

	if (m_pItems)
		m_pItems->AddTypesUsed(retTypesUsed);

	if (m_pEscorts)
		m_pEscorts->AddTypesUsed(retTypesUsed);

	retTypesUsed->SetAt(strToInt(m_pDefaultScreen.GetUNID(), 0), true);
	retTypesUsed->SetAt(m_dwDefaultBkgnd, true);
	retTypesUsed->SetAt(m_HeroImage.GetBitmapUNID(), true);
	retTypesUsed->SetAt(m_ExhaustImage.GetBitmapUNID(), true);
	retTypesUsed->SetAt(m_Hull.GetValue().GetCurrencyType()->GetUNID(), true);
	}

ALERROR CShipClass::OnBindDesign (SDesignLoadCtx &Ctx)

//	OnBindDesign
//
//	Bind design

	{
	ALERROR error;
	int i;

	//	If necessary, bind our static default player settings

	if (!m_bDefaultPlayerSettingsBound)
		{
		m_DefaultPlayerSettings.InitAsDefault();

		if (error = m_DefaultPlayerSettings.Bind(Ctx, NULL))
			{
			//	Log error, but continue

			::kernelDebugLogString(CONSTLIT("Unable to bind default player ship settings."));
			}

		m_bDefaultPlayerSettingsBound = true;
		}

	//	Bind sovereign

	if (error = m_pDefaultSovereign.Bind(Ctx))
		goto Fail;

	//	Image

	if (error = m_Image.OnDesignLoadComplete(Ctx))
		goto Fail;

    if (error = m_HeroImage.OnDesignLoadComplete(Ctx))
        goto Fail;

	//	Now that we have the image we can bind the rotation desc, because it needs
	//	the rotation count, etc.

	if (error = m_RotationDesc.Bind(Ctx, m_Image.GetSimpleImage()))
		goto Fail;

	//	Thruster effects

	if (error = m_Effects.Bind(Ctx, m_Image.GetSimpleImage()))
		goto Fail;

	//	Drive images

	if (m_Exhaust.GetCount() > 0)
		{
		int iRotationCount = m_RotationDesc.GetFrameCount();
		int iScale = GetImageViewportSize();

		m_ExhaustImage.SetRotationCount(iRotationCount);
		if (error = m_ExhaustImage.OnDesignLoadComplete(Ctx))
			goto Fail;

		for (i = 0; i < m_Exhaust.GetCount(); i++)
			m_Exhaust[i].PosCalc.InitComplete(iRotationCount, iScale, 180);
		}

	//	Hull

	if (error = m_Hull.Bind(Ctx))
		goto Fail;

    if (error = m_Armor.Bind(Ctx))
        goto Fail;

	if (error = m_WreckDesc.Bind(Ctx))
		goto Fail;

	//	More

	if (error = m_EventHandler.Bind(Ctx))
		goto Fail;

	if (error = m_Character.Bind(Ctx))
		goto Fail;

	if (error = m_CharacterClass.Bind(Ctx))
		goto Fail;

	if (error = m_pDefaultScreen.Bind(Ctx, GetLocalScreens()))
		goto Fail;

	if (m_pTrade)
		if (error = m_pTrade->OnDesignLoadComplete(Ctx))
			goto Fail;

	//	If we own the player settings, then bind them. Otherwise, we clear the
    //  pointer so that we can resolve at run-time.
    //
    //  NOTE: Bind will also unresolve the settings, in case it inherited some
    //  stuff.

    if (m_fOwnPlayerSettings)
        {
		if (error = m_pPlayerSettings->Bind(Ctx, this))
			goto Fail;
        }
    else
        m_pPlayerSettings = NULL;

	//	AI Settings

	if (m_AISettings.GetMinCombatSeparation() < 0.0)
		{
		if (m_Image.GetSimpleImage().IsLoaded())
			m_AISettings.SetMinCombatSeparation(RectWidth(m_Image.GetSimpleImage().GetImageRect()) * g_KlicksPerPixel);
		else
			m_AISettings.SetMinCombatSeparation(60.0 * g_KlicksPerPixel);
		}

	//	Items

	if (m_pItems)
		if (error = m_pItems->OnDesignLoadComplete(Ctx))
			goto Fail;

	if (m_pDeviceSlots)
		if (error = m_pDeviceSlots->OnDesignLoadComplete(Ctx))
			goto Fail;

	if (m_pDevices)
		if (error = m_pDevices->OnDesignLoadComplete(Ctx))
			goto Fail;

	//	Escorts

	if (m_pEscorts)
		if (error = m_pEscorts->OnDesignLoadComplete(Ctx))
			return error;

	//	Generate an average set of devices.
	//
	//	NOTE: When generating average devices we skip the part about placing
	//	the device in a slot. This is because we can't call item criteria
	//	functions inside Bind (because we're not set up yet).

	GenerateDevices(1, m_AverageDevices, GDFLAG_NO_DEVICE_SLOT_SEARCH);

	//	Initialize thrust, if necessary

	if (m_rThrustRatio != 0.0)
		{
		Metric rMass = CalcMass(m_AverageDevices);
		if (rMass > 0.0)
			m_DriveDesc.SetThrust(CDriveDesc::CalcThrust(m_rThrustRatio, rMass));
		}

	//	For later APIs compute the drive power usage, if not specified

	if (m_DriveDesc.GetPowerUse() < 0)
		{
		if (GetAPIVersion() >= 29)
			m_DriveDesc.SetPowerUse((int)Max(1.0, DRIVE_POWER_FACTOR * pow(m_DriveDesc.GetThrust() / 100.0, DRIVE_POWER_EXP)));

		//	Otherwise, use the default

		else
			m_DriveDesc.SetPowerUse(DEFAULT_POWER_USE);
		}

	//	Bind structures

	if (error = m_Interior.BindDesign(Ctx))
		goto Fail;

    //  Compute performance based on average devices

    CalcPerformance();

	return NOERROR;

Fail:

	return ComposeLoadError(Ctx, Ctx.sError);
	}

ALERROR CShipClass::OnFinishBindDesign (SDesignLoadCtx &Ctx)

//	OnFinishBindDesign
//
//	All types bound.

	{
	//	Compute score and level

	if (!m_fScoreOverride)
		m_iScore = CalcScore();

	if (!m_fLevelOverride)
		m_iLevel = CalcLevel();

	m_iLevelType = CalcBalanceType(NULL, &m_rCombatStrength);

	m_Hull.InitCyberDefenseLevel(m_iLevel);

	//	Done

	return NOERROR;
	}

void CShipClass::OnInitFromClone (CDesignType *pSource)

//	OnInitFromClone
//
//	Initializes from pSource

	{
	CShipClass *pClass = CShipClass::AsType(pSource);
	if (pClass == NULL)
		{
		ASSERT(false);
		return;
		}

	m_pDefaultSovereign = pClass->m_pDefaultSovereign;
	m_sManufacturer = pClass->m_sManufacturer;
	m_sName = pClass->m_sName;
	m_sTypeName = pClass->m_sTypeName;
	m_dwClassNameFlags = pClass->m_dwClassNameFlags;

	m_sShipNames = pClass->m_sShipNames;
	m_dwShipNameFlags = pClass->m_dwShipNameFlags;
	m_ShipNamesIndices = pClass->m_ShipNamesIndices;
	m_iShipName = pClass->m_iShipName;

	m_iScore = pClass->m_iScore;
	m_iLevel = pClass->m_iLevel;
	m_fScoreOverride = pClass->m_fScoreOverride;
	m_fLevelOverride = pClass->m_fLevelOverride;
	//	m_iLevelType and m_rCombatStrength will get computed during Bind.

	m_fShipCompartment = pClass->m_fShipCompartment;
	m_Hull = pClass->m_Hull;
	m_RotationDesc = pClass->m_RotationDesc;
	m_rThrustRatio = pClass->m_rThrustRatio;
	m_DriveDesc = pClass->m_DriveDesc;
	m_ReactorDesc = pClass->m_ReactorDesc;
	m_WreckDesc = pClass->m_WreckDesc;

	m_Armor = pClass->m_Armor;
	m_Interior = pClass->m_Interior;

	if (pClass->m_pDeviceSlots)
		{
		m_pDeviceSlots = pClass->m_pDeviceSlots;
		m_fInheritedDeviceSlots = true;
		}

	if (pClass->m_pDevices)
		{
		m_pDevices = pClass->m_pDevices;
		m_fInheritedDevices = true;
		}

	m_AverageDevices = pClass->m_AverageDevices;

	m_Equipment = pClass->m_Equipment;

	m_AISettings = pClass->m_AISettings;

    if (m_fOwnPlayerSettings = pClass->m_fOwnPlayerSettings)
        m_pPlayerSettings = new CPlayerSettings(*pClass->m_pPlayerSettings);
    else
        m_pPlayerSettings = NULL;

	if (pClass->m_pItems)
		{
		m_pItems = pClass->m_pItems;
		m_fInheritedItems = true;
		}

	if (pClass->m_pEscorts)
		{
		m_pEscorts = pClass->m_pEscorts;
		m_fInheritedEscorts = true;
		}

	m_EventHandler = pClass->m_EventHandler;
	m_CharacterClass = pClass->m_CharacterClass;
	m_Character = pClass->m_Character;

	m_DockingPorts = pClass->m_DockingPorts;
	m_pDefaultScreen = pClass->m_pDefaultScreen;
	m_dwDefaultBkgnd = pClass->m_dwDefaultBkgnd;
	m_fHasDockingPorts = pClass->m_fHasDockingPorts;

	if (pClass->m_pTrade)
		{
		m_pTrade = pClass->m_pTrade;
		m_fInheritedTrade = true;
		}

	m_OriginalCommsHandler = pClass->m_OriginalCommsHandler;
	m_CommsHandler = pClass->m_CommsHandler;
	m_fCommsHandlerInit = pClass->m_fCommsHandlerInit;

	m_Image = pClass->m_Image;
    m_HeroImage = pClass->m_HeroImage;
	m_Effects = pClass->m_Effects;

	//	No need to copy m_WreckImage or m_WreckBitmap because they are just
	//	caches.

	m_ExhaustImage = pClass->m_ExhaustImage;
	m_Exhaust = pClass->m_Exhaust;

	//	m_fHasOn... are computed during bind

	m_fVirtual = pClass->m_fVirtual;
	}

ALERROR CShipClass::OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	OnCreateFromXML
//
//	Create from XML

	{
	ALERROR error;
	int i;

	//	Initialize basic info

	CString sAttrib;
	m_sManufacturer = pDesc->GetAttribute(CONSTLIT(g_ManufacturerAttrib));
	m_sName = pDesc->GetAttribute(CONSTLIT(g_ClassAttrib));
	m_sTypeName = pDesc->GetAttribute(CONSTLIT(g_TypeAttrib));
	m_dwClassNameFlags = CLanguage::LoadNameFlags(pDesc);
	m_fVirtual = pDesc->GetAttributeBool(VIRTUAL_ATTRIB);
	m_fShipCompartment = pDesc->GetAttributeBool(SHIP_COMPARTMENT_ATTRIB);

	if (error = m_pDefaultSovereign.LoadUNID(Ctx, pDesc->GetAttribute(DEFAULT_SOVEREIGN_ATTRIB)))
		return error;

	//	Score and level

	m_fScoreOverride = pDesc->FindAttributeInteger(SCORE_ATTRIB, &m_iScore);
	m_fLevelOverride = pDesc->FindAttributeInteger(LEVEL_ATTRIB, &m_iLevel);

	//	Names

	CXMLElement *pNames = pDesc->GetContentElementByTag(NAMES_TAG);
	if (pNames)
		{
		m_sShipNames = pNames->GetContentText(0);
		m_dwShipNameFlags = CLanguage::LoadNameFlags(pNames);
		}
	else
		m_dwShipNameFlags = 0;

	InitShipNamesIndices();

	//	Load the images

	CXMLElement *pImage = pDesc->GetContentElementByTag(IMAGE_TAG);
	if (pImage)
		if (error = m_Image.InitSimpleFromXML(Ctx, pImage, false, STD_ROTATION_COUNT))
			return ComposeLoadError(Ctx, Ctx.sError);

    pImage = pDesc->GetContentElementByTag(HERO_IMAGE_TAG);
    if (pImage)
        if (error = m_HeroImage.InitFromXML(Ctx, pImage))
            return ComposeLoadError(Ctx, Ctx.sError);

	//	Maneuvering

	if (error = m_RotationDesc.InitFromXML(Ctx, pDesc))
		return ComposeLoadError(Ctx, Ctx.sError);

	//	Drive

	int iMaxSpeed;
	if (error = m_DriveDesc.InitFromShipClassXML(Ctx, pDesc, GetUNID(), &m_rThrustRatio, &iMaxSpeed))
		return ComposeLoadError(Ctx, Ctx.sError);

	//	Hull descriptor

	if (error = m_Hull.InitFromXML(Ctx, pDesc, iMaxSpeed))
		return ComposeLoadError(Ctx, Ctx.sError);

	//	If 0 size, come up with a reasonable default based on image size.

	if (m_Hull.GetSize() == 0 && !m_Image.IsEmpty())
		m_Hull.SetSize(CalcDefaultSize(m_Image.GetSimpleImage()));

	//	If we have no max armor limit, then we compute default values.

	if (m_Hull.GetMaxArmorMass() == 0)
		m_Hull.InitDefaultArmorLimits(iMaxSpeed, (m_rThrustRatio > 0.0 ? m_rThrustRatio : CDriveDesc::CalcThrustRatio(m_DriveDesc.GetThrust(), m_Hull.GetMass())));

	//	Wreck descriptor

	if (error = m_WreckDesc.InitFromXML(Ctx, pDesc, m_Hull.GetMass()))
		return ComposeLoadError(Ctx, Ctx.sError);

	//	Load effects

	CXMLElement *pEffects = pDesc->GetContentElementByTag(EFFECTS_TAG);
	if (pEffects)
		{
		if (error = m_Effects.InitFromXML(Ctx, 
				strPatternSubst(CONSTLIT("%d"), GetUNID()), 
				pEffects))
			return ComposeLoadError(Ctx, Ctx.sError);
		}

	//	Other devices

	if (error = m_ReactorDesc.InitFromXML(Ctx, pDesc, GetUNID(), true))
		return error;

	//	Load armor

	CXMLElement *pArmor = pDesc->GetContentElementByTag(ARMOR_TAG);
	if (pArmor)
		{
        if (error = m_Armor.InitFromXML(Ctx, pArmor))
            return ComposeLoadError(Ctx, Ctx.sError);
		}

	//	Load devices

	CXMLElement *pDeviceSlots = pDesc->GetContentElementByTag(DEVICE_SLOTS_TAG);
	if (pDeviceSlots)
		{
		if (error = IDeviceGenerator::CreateFromXML(Ctx, pDeviceSlots, &m_pDeviceSlots))
			return ComposeLoadError(Ctx, Ctx.sError);
		}

	CXMLElement *pDevices = pDesc->GetContentElementByTag(DEVICES_TAG);
	if (pDevices)
		{
		if (error = IDeviceGenerator::CreateFromXML(Ctx, pDevices, &m_pDevices))
			return ComposeLoadError(Ctx, Ctx.sError);
		}

	//	Load interior structure

	CXMLElement *pInterior = pDesc->GetContentElementByTag(INTERIOR_TAG);
	if (pInterior)
		{
		if (error = m_Interior.InitFromXML(Ctx, pInterior))
			return ComposeLoadError(Ctx, Ctx.sError);
		}

	//	Load AI settings

	CXMLElement *pAISettings = pDesc->GetContentElementByTag(CONSTLIT(g_AISettingsTag));
	if (pAISettings)
		{
		if (error = m_AISettings.InitFromXML(Ctx, pAISettings))
			return ComposeLoadError(Ctx, Ctx.sError);
		}
	else
		m_AISettings.InitToDefault();

	//	Load items

	CXMLElement *pItems = pDesc->GetContentElementByTag(ITEMS_TAG);
	if (pItems)
		{
		if (error = IItemGenerator::CreateFromXML(Ctx, pItems, &m_pItems))
			return ComposeLoadError(Ctx, Ctx.sError);
		}

	//	Load equipment

	CXMLElement *pEquipment = pDesc->GetContentElementByTag(EQUIPMENT_TAG);
	if (pEquipment)
		{
		for (i = 0; i < pEquipment->GetContentElementCount(); i++)
			{
			CXMLElement *pLine = pEquipment->GetContentElement(i);

			//	Allocate a descriptor

			SEquipmentDesc *pDesc = m_Equipment.Insert();

			//	Figure out what equipment we're talking about

			pDesc->iEquipment = AbilityDecode(pLine->GetAttribute(EQUIPMENT_ATTRIB));
			if (pDesc->iEquipment == ::ablUnknown)
				return ComposeLoadError(Ctx, strPatternSubst(ERR_UNKNOWN_EQUIPMENT, pLine->GetAttribute(EQUIPMENT_ATTRIB)));

			//	Figure out what to do with the equipment

			if (strEquals(pLine->GetTag(), INSTALL_TAG))
				pDesc->bInstall = true;
			else if (strEquals(pLine->GetTag(), REMOVE_TAG))
				pDesc->bInstall = false;
			else
				return ComposeLoadError(Ctx, strPatternSubst(ERR_UNKNOWN_EQUIPMENT_DIRECTIVE, pLine->GetTag()));
			}
		}

	//	Drive images

	CXMLElement *pDriveImages = pDesc->GetContentElementByTag(DRIVE_IMAGES_TAG);
	if (pDriveImages)
		{
		int iScale = GetImageViewportSize();

		for (i = 0; i < pDriveImages->GetContentElementCount(); i++)
			{
			CXMLElement *pItem = pDriveImages->GetContentElement(i);
			if (strEquals(pItem->GetTag(), NOZZLE_IMAGE_TAG))
				{
				if (error = m_ExhaustImage.InitFromXML(Ctx, pItem))
					return ComposeLoadError(Ctx, ERR_BAD_EXHAUST_IMAGE);
				}
			else if (strEquals(pItem->GetTag(), NOZZLE_POS_TAG))
				{
				//	Initialize structure

				SExhaustDesc *pExhaust = m_Exhaust.Insert();

				//	Load the position

				if (error = pExhaust->PosCalc.Init(pItem))
					return ComposeLoadError(Ctx, ERR_DRIVE_IMAGE_FORMAT);
				}
			else
				return ComposeLoadError(Ctx, ERR_DRIVE_IMAGE_FORMAT);
			}
		}

	//	Escorts

	CXMLElement *pEscorts = pDesc->GetContentElementByTag(ESCORTS_TAG);
	if (pEscorts)
		{
		if (error = IShipGenerator::CreateFromXMLAsGroup(Ctx, pEscorts, &m_pEscorts))
			return ComposeLoadError(Ctx, Ctx.sError);
		}
	else
		m_pEscorts = NULL;

	//	Characters

	if (error = m_EventHandler.LoadUNID(Ctx, pDesc->GetAttribute(EVENT_HANDLER_ATTRIB)))
		return error;

	if (error = m_Character.LoadUNID(Ctx, pDesc->GetAttribute(CHARACTER_ATTRIB)))
		return error;

	if (error = m_CharacterClass.LoadUNID(Ctx, pDesc->GetAttribute(CHARACTER_CLASS_ATTRIB)))
		return error;

	//	Initialize docking data

	m_DockingPorts.InitPortsFromXML(NULL, pDesc, GetImageViewportSize());
	if (m_DockingPorts.GetPortCount() > 0)
		{
		//	Load the default screen

		m_pDefaultScreen.LoadUNID(Ctx, pDesc->GetAttribute(DOCK_SCREEN_ATTRIB));
		if (m_pDefaultScreen.GetUNID().IsBlank())
			return ComposeLoadError(Ctx, ERR_DOCK_SCREEN_NEEDED);

		//	Background screens

		if (error = LoadUNID(Ctx, pDesc->GetAttribute(DEFAULT_BACKGROUND_ID_ATTRIB), &m_dwDefaultBkgnd))
			return error;

		m_fHasDockingPorts = true;
		}
	else
		{
		m_dwDefaultBkgnd = 0;
		m_fHasDockingPorts = false;
		}

	//	Load trade

	CXMLElement *pTrade = pDesc->GetContentElementByTag(TRADE_TAG);
	if (pTrade)
		{
		if (error = CTradingDesc::CreateFromXML(Ctx, pTrade, &m_pTrade))
			return error;
		}
	else
		m_pTrade = NULL;

	//	Load communications

	CXMLElement *pComms = pDesc->GetContentElementByTag(COMMUNICATIONS_TAG);
	if (pComms)
		if (error = m_OriginalCommsHandler.InitFromXML(pComms, &Ctx.sError))
			return ComposeLoadError(Ctx, Ctx.sError);

	m_fCommsHandlerInit = false;

    //  Load initial data, if available

    CXMLElement *pInitData = pDesc->GetContentElementByTag(INITIAL_DATA_TAG);
    if (pInitData)
        m_InitialData.SetFromXML(pInitData);

	//	Load player settings

    m_pPlayerSettings = NULL;
	m_fOwnPlayerSettings = false;
	CXMLElement *pPlayer = pDesc->GetContentElementByTag(PLAYER_SETTINGS_TAG);
    if (pPlayer)
        {
        m_pPlayerSettings = new CPlayerSettings;
        m_fOwnPlayerSettings = true;

        if (error = m_pPlayerSettings->InitFromXML(Ctx, this, pPlayer))
            return ComposeLoadError(Ctx, Ctx.sError);
        }

	//	If we don't have a hull value, compute it now.

	if (m_Hull.GetValue().IsEmpty())
		{
		//	Defaults to credits
		m_Hull.SetValue(CCurrencyAndValue(CalcHullValue()));
		}

	//	Done

	return NOERROR;
	}

CEffectCreator *CShipClass::OnFindEffectCreator (const CString &sUNID)

//	OnFindEffectCreator
//
//	Find the effect creator. sUNID is the remainder after the item type UNID has been removed
//
//	{unid}:p:s		Player settings shield effect
//	      ^

	{
	//	We start after the class UNID

	char *pPos = sUNID.GetASCIIZPointer();
	if (*pPos != ':')
		return NULL;
	
	pPos++;

	//	Figure out what

	switch (*pPos)
		{
		case 'p':
            {
            const CPlayerSettings *pPlayerSettings = GetPlayerSettings();
            if (pPlayerSettings == NULL)
                return NULL;

            return pPlayerSettings->FindEffectCreator(CString(pPos + 1));
            }

		default:
			return NULL;
		}
	}

CString CShipClass::OnGetMapDescriptionMain (SMapDescriptionCtx &Ctx) const

//  OnGetMapDescriptionMain
//
//  Returns a map description. This always gets prepended to any other descriptions.

    {
    if (Ctx.bEnemy)
        return CONSTLIT("Hostile");

	return GetNounPhrase(nounGeneric | nounCapitalize);
    }

ICCItemPtr CShipClass::OnGetProperty (CCodeChainCtx &Ctx, const CString &sProperty) const

//	OnGetProperty
//
//	Returns a property of the ship class

	{
	CCodeChain &CC = g_pUniverse->GetCC();

	if (strEquals(sProperty, PROPERTY_CURRENCY))
		return ICCItemPtr(CC.CreateInteger(GetEconomyType()->GetUNID()));
		
	else if (strEquals(sProperty, PROPERTY_CURRENCY_NAME))
		return ICCItemPtr(CC.CreateString(GetEconomyType()->GetSID()));
		
	else if (strEquals(sProperty, PROPERTY_DEFAULT_SOVEREIGN))
		return (m_pDefaultSovereign.GetUNID() ? ICCItemPtr(CC.CreateInteger(m_pDefaultSovereign.GetUNID())) : ICCItemPtr(CC.CreateNil()));

	else if (strEquals(sProperty, PROPERTY_HAS_TRADE_DESC))
		return ICCItemPtr(CC.CreateBool(m_pTrade != NULL));

	else if (strEquals(sProperty, PROPERTY_HAS_VARIANTS))
		return ICCItemPtr(CC.CreateBool(m_pDevices && m_pDevices->IsVariant()));

	else if (strEquals(sProperty, PROPERTY_HULL_VALUE))
		return CTLispConvert::CreateCurrencyValue(CC, GetEconomyType()->Exchange(m_Hull.GetValue()));

	else if (strEquals(sProperty, PROPERTY_MAX_ARMOR_MASS))
		return (m_Hull.GetMaxArmorMass() > 0 ? ICCItemPtr(CC.CreateInteger(m_Hull.GetMaxArmorMass())) : ICCItemPtr(CC.CreateNil()));

	else if (strEquals(sProperty, PROPERTY_MAX_SPEED_AT_MAX_ARMOR))
		return (m_Hull.GetMaxArmorSpeedPenalty() != 0 ? ICCItemPtr(CC.CreateInteger(m_Perf.GetDriveDesc().GetMaxSpeedFrac() + m_Hull.GetMaxArmorSpeedPenalty())) : ICCItemPtr(CC.CreateNil()));

	else if (strEquals(sProperty, PROPERTY_MAX_SPEED_AT_MIN_ARMOR))
		return (m_Hull.GetMinArmorSpeedBonus() != 0 ? ICCItemPtr(CC.CreateInteger(m_Perf.GetDriveDesc().GetMaxSpeedFrac() + m_Hull.GetMinArmorSpeedBonus())) : ICCItemPtr(CC.CreateNil()));

	else if (strEquals(sProperty, PROPERTY_MAX_SPEED_BY_ARMOR_MASS))
		return ICCItemPtr(CalcMaxSpeedByArmorMass(Ctx));

	else if (strEquals(sProperty, PROPERTY_STD_ARMOR_MASS))
		return (m_Hull.GetStdArmorMass() > 0 ? ICCItemPtr(CC.CreateInteger(m_Hull.GetStdArmorMass())) : ICCItemPtr(CC.CreateNil()));

	else if (strEquals(sProperty, PROPERTY_WRECK_STRUCTURAL_HP))
		return ICCItemPtr(CC.CreateInteger(GetMaxStructuralHitPoints()));

	//	Drive properties

	else if (strEquals(sProperty, PROPERTY_THRUST_TO_WEIGHT))
		{
		Metric rMass = CalcMass(m_AverageDevices);
		int iRatio = (int)((200.0 * (rMass > 0.0 ? m_Perf.GetDriveDesc().GetThrust() / rMass : 0.0)) + 0.5);
		return ICCItemPtr(CC.CreateInteger(10 * iRatio));
		}
	else if (strEquals(sProperty, PROPERTY_THRUST_RATIO))
		{
		Metric rMass = CalcMass(m_AverageDevices);
		Metric rRatio = 2.0 * (rMass > 0.0 ? m_Perf.GetDriveDesc().GetThrust() / rMass : 0.0);
		return ICCItemPtr(CC.CreateDouble(mathRound(rRatio * 10.0) / 10.0));
		}
	else if (strEquals(sProperty, PROPERTY_THRUSTER_POWER))
		{
		const CObjectEffectDesc &Effects = GetEffectsDesc();
		int iThrustersPerSide = Max(1, Effects.GetEffectCount(CObjectEffectDesc::effectThrustLeft));
		int iThrusterPower = Max(1, mathRound((m_Hull.GetMass() / iThrustersPerSide) * m_RotationDesc.GetRotationAccelPerTick()));
		return ICCItemPtr(CC.CreateInteger(iThrusterPower));
		}

	else if (strEquals(sProperty, PROPERTY_DRIVE_POWER)
			|| strEquals(sProperty, PROPERTY_MAX_SPEED)
			|| strEquals(sProperty, PROPERTY_THRUST))
		return ICCItemPtr(CDriveClass::GetDriveProperty(m_Perf.GetDriveDesc(), sProperty));

	//	Reactor properties

	else if (CReactorDesc::IsExportedProperty(sProperty))
		return ICCItemPtr(m_Perf.GetReactorDesc().FindProperty(sProperty));

	else
		return NULL;
	}

bool CShipClass::OnHasSpecialAttribute (const CString &sAttrib) const

//	OnHasSpecialAttribute
//
//	Returns TRUE if we have the special attribute

	{
	if (strStartsWith(sAttrib, SPECIAL_IS_PLAYER_CLASS))
		{
		CString sValue = strSubString(sAttrib, SPECIAL_IS_PLAYER_CLASS.GetLength());

		//	For purposes of this attribute, we only consider explicit (non-default)
		//	player ships. Otherwise, every ship is a player ship.

		const CPlayerSettings *pPlayer = GetPlayerSettings();
		bool bIsPlayerClass = (pPlayer && m_fOwnPlayerSettings);

		return (strEquals(sValue, SPECIAL_VALUE_TRUE) == bIsPlayerClass);
		}
	else if (strStartsWith(sAttrib, SPECIAL_ITEM_ATTRIBUTE))
		{
		CString sItemAttrib = strSubString(sAttrib, SPECIAL_ITEM_ATTRIBUTE.GetLength());

		if (m_pDevices && m_pDevices->HasItemAttribute(sItemAttrib))
			return true;

		if (m_pItems && m_pItems->HasItemAttribute(sItemAttrib))
			return true;

		return false;
		}
	else if (strStartsWith(sAttrib, SPECIAL_MANUFACTURER))
		{
		CString sValue = strSubString(sAttrib, SPECIAL_MANUFACTURER.GetLength());
		return strEquals(sValue, GetManufacturerName());
		}
	else
		return false;
	}

void CShipClass::OnMergeType (CDesignType *pSource)

//	OnMergeType
//
//	Merges the definitions from pSource into our class

	{
	CShipClass *pClass = CShipClass::AsType(pSource);
	if (pClass == NULL)
		{
		ASSERT(false);
		return;
		}

	//	Merge image

	if (!pClass->m_Image.IsEmpty())
		m_Image = pClass->m_Image;

    if (!pClass->m_HeroImage.IsEmpty())
        m_HeroImage = pClass->m_HeroImage;

	//	Merge player settings

	if (pClass->m_fOwnPlayerSettings)
		{
		if (m_fOwnPlayerSettings)
			m_pPlayerSettings->MergeFrom(*pClass->m_pPlayerSettings);
        else
            {
            m_pPlayerSettings = new CPlayerSettings(*pClass->m_pPlayerSettings);
            m_fOwnPlayerSettings = true;
            }
		}

	//	Comms handler
	//	LATER: Currently only adds new handlers; does not replace existing ones.

	if (pClass->m_OriginalCommsHandler.GetCount() > 0)
		m_OriginalCommsHandler.Merge(pClass->m_OriginalCommsHandler);
	}

void CShipClass::OnReadFromStream (SUniverseLoadCtx &Ctx)

//	ReadFromStream
//
//	Read from stream
//
//	DWORD		flags
//	DWORD		ship name indices count
//	DWORD[]		ship name index
//	DWORD		m_iShipName

	{
	int i;
	DWORD dwLoad;
	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));

	//	Load opaque data (for previous versions)

	ReadGlobalData(Ctx);

	//	Load random ship name indices

	if (Ctx.dwVersion >= 4)
		{
		int iCount;
		Ctx.pStream->Read((char *)&iCount, sizeof(DWORD));
		m_ShipNamesIndices.DeleteAll();
		m_ShipNamesIndices.InsertEmpty(iCount);
		for (i = 0; i < iCount; i++)
			{
			Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
			m_ShipNamesIndices[i] = dwLoad;
			}

		Ctx.pStream->Read((char *)&m_iShipName, sizeof(DWORD));
		}
	else
		{
		InitShipNamesIndices();
		}
	}

void CShipClass::OnReinit (void)

//	OnReinit
//
//	Reinitialize the class

	{
	DEBUG_TRY

	InitShipNamesIndices();
	m_WreckDesc.CleanUp();

    //  If we created the hero image, then free it.

    if (m_HeroImage.GetBitmapUNID() == 0)
        m_HeroImage.CleanUp();

	DEBUG_CATCH
	}

void CShipClass::OnSweep (void)

//  OnSweep
//
//  Free images that we're no longer using

    {
	DEBUG_TRY

    //  Clean up wreck image

	m_WreckDesc.SweepImages();

    //  If we created the hero image, then we free it.

    if (m_HeroImage.GetBitmapUNID() == 0
            && !m_HeroImage.IsMarked())
        m_HeroImage.CleanUp();

	DEBUG_CATCH
    }

void CShipClass::OnUnbindDesign (void)

//	OnUnbindDesign
//
//	Undo binding

	{
	//	Undo inheritance

	if (!m_fOwnPlayerSettings)
		m_pPlayerSettings = NULL;

	//	Reset comms handler because our inheritance chain might
	//	have changed.

	m_fCommsHandlerInit = false;
	m_CommsHandler.DeleteAll();
	}

void CShipClass::OnWriteToStream (IWriteStream *pStream)

//	OnWriteToStream
//
//	Writes out the variable portions of the item type
//
//	DWORD		flags
//	DWORD		ship name indices count
//	DWORD[]		ship name index
//	DWORD		m_iShipName

	{
	int i;
	DWORD dwSave = 0;
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	//	Write out ship name indices

	dwSave = m_ShipNamesIndices.GetCount();
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	for (i = 0; i < m_ShipNamesIndices.GetCount(); i++)
		{
		dwSave = (DWORD)m_ShipNamesIndices[i];
		pStream->Write((char *)&dwSave, sizeof(DWORD));
		}

	pStream->Write((char *)&m_iShipName, sizeof(DWORD));
	}

void CShipClass::Paint (CG32bitImage &Dest, 
						int x, 
						int y, 
						const ViewportTransform &Trans, 
						int iDirection, 
						int iTick,
						bool bThrusting,
						bool bRadioactive,
						DWORD byInvisible)

//	Paint
//
//	Paints the ship class

	{
	//	If we're facing down paint the thrust first.

	if (bThrusting)
		PaintThrust(Dest, x, y, Trans, iDirection, iTick, true /* bInFrontOnly */);

	//	Paint the body of the ship

	if (byInvisible)
		GetImage().PaintImageShimmering(Dest, x, y, iTick, iDirection, byInvisible);
	else if (bRadioactive)
		GetImage().PaintImageWithGlow(Dest, x, y, iTick, iDirection, CG32bitPixel(0, 255, 0));
	else
		GetImage().PaintImage(Dest, x, y, iTick, iDirection);

	//	If we need to paint the thrust (because we didn't earlier) do it now.

	if (bThrusting)
		PaintThrust(Dest, x, y, Trans, iDirection, iTick, false /* bInFrontOnly */);

#ifdef DEBUG_3D_ADJ
	{
	int i;
	int iScale = m_Image.GetImageViewportSize();
	for (i = 0; i < 360; i++)
		{
		int xP, yP;
		IntPolarToVector(i, 24, &xP, &yP);
		Dest.SetPixel(x + xP, y + yP, CG32bitPixel(0, 0, 255));

		C3DConversion::CalcCoord(iScale, i, 8, 0, &xP, &yP);
		Dest.SetPixel(x + xP, y + yP, CG32bitPixel(255, 255, 0));

		C3DConversion::CalcCoord(iScale, i, 16, 0, &xP, &yP);
		Dest.SetPixel(x + xP, y + yP, CG32bitPixel(255, 255, 0));

		C3DConversion::CalcCoord(iScale, i, 24, 0, &xP, &yP);
		Dest.SetPixel(x + xP, y + yP, CG32bitPixel(255, 255, 0));
		}
	}
#endif
	}

void CShipClass::PaintDevicePositions (CG32bitImage &Dest, int x, int y, const CDeviceDescList &Devices, int iShipRotation) const

//	PaintDevicePositions
//
//	Paint position and fire arc of devices.

	{
	int i;
	int iScale = GetImageViewportSize();

	for (i = 0; i < Devices.GetCount(); i++)
		{
		const SDeviceDesc &Desc = Devices.GetDeviceDesc(i);

		switch (Desc.Item.GetType()->GetCategory())
			{
			case itemcatWeapon:
			case itemcatLauncher:
				CInstalledDevice::PaintDevicePos(Desc, Dest, x, y, iScale, iShipRotation);
				break;
			}
		}
	}

void CShipClass::PaintDockPortPositions (CG32bitImage &Dest, int x, int y, int iShipRotation) const

//	PaintDockPortPositions
//
//	Paint docking ports

	{
	int iScale = GetImageViewportSize();
	m_DockingPorts.DebugPaint(Dest, x, y, iShipRotation, iScale);
	}

void CShipClass::PaintInteriorCompartments (CG32bitImage &Dest, int x, int y, int iShipRotation) const

//	PaintInteriorCompartments
//
//	Paints outline of interior compartments

	{
	int iScale = GetImageViewportSize();
	m_Interior.DebugPaint(Dest, x, y, iShipRotation, iScale);
	}

void CShipClass::PaintMap (CMapViewportCtx &Ctx, 
						CG32bitImage &Dest, 
						int x, 
						int y, 
						int iDirection, 
						int iTick,
						bool bThrusting,
						bool bRadioactive)

//	PaintMap
//
//	Paints the ship class on the map

	{
	m_Image.GetSimpleImage().PaintScaledImage(Dest, x, y, iTick, iDirection, 24, 24, CObjectImageArray::FLAG_CACHED);
	}

void CShipClass::PaintScaled (CG32bitImage &Dest,
							int x,
							int y,
							int cxWidth,
							int cyHeight,
							int iRotation,
							int iTick)

	//	PaintScaled
	//
	//	Paints a scaled image, including any attached compartments.

	{
	if (m_Interior.HasAttached())
		{
		CG32bitImage Image;
		CreateScaledImage(Image, iTick, iRotation, cxWidth, cyHeight);

		Dest.Blt(0, 0, Image.GetWidth(), Image.GetHeight(), Image, x - (Image.GetWidth() / 2), y - (Image.GetHeight() / 2));
		}
	else
		m_Image.GetSimpleImage().PaintScaledImage(Dest, x, y, iTick, GetRotationDesc().GetFrameIndex(iRotation), cxWidth, cyHeight, CObjectImageArray::FLAG_CACHED);
	}

void CShipClass::PaintThrust (CG32bitImage &Dest, 
							  int x, 
							  int y, 
							  const ViewportTransform &Trans, 
							  int iDirection, 
							  int iTick,
							  bool bInFrontOnly)

//	PaintThrust
//
//	Paints the thrust effect

	{
	int i;

	for (i = 0; i < m_Exhaust.GetCount(); i++)
		{
		if (m_Exhaust[i].PosCalc.PaintFirst(iDirection) == bInFrontOnly)
			{
			int xThrust;
			int yThrust;

			m_Exhaust[i].PosCalc.GetCoordFromDir(iDirection, &xThrust, &yThrust);
			m_ExhaustImage.PaintImage(Dest,
					x + xThrust,
					y + yThrust,
					iTick,
					iDirection);
			}
		}
	}

void CShipClass::Reinit (void)

//	Reinit
//
//	Clean up global data

	{
	m_DefaultPlayerSettings = CPlayerSettings();
	m_bDefaultPlayerSettingsBound = false;
	}
