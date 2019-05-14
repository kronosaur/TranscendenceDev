//	TSETypes.h
//
//	Basic types
//	Copyright 2012 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

#define OBJID_CUNIVERSE				MakeOBJCLASSID(1)
#define OBJID_CSYSTEM				MakeOBJCLASSID(2)
#define OBJID_CSPACEOBJECT			MakeOBJCLASSID(3)
#define OBJID_CSHIPCLASS			MakeOBJCLASSID(4)
#define OBJID_CSHIP					MakeOBJCLASSID(5)
#define OBJID_COBJECTIMAGEARRAY		MakeOBJCLASSID(6)
#define OBJID_CSTATION				MakeOBJCLASSID(7)
#define OBJID_CSTANDARDSHIPAI		MakeOBJCLASSID(8)
#define OBJID_CBEAM					MakeOBJCLASSID(9)
#define OBJID_CEFFECT				MakeOBJCLASSID(10)
#define OBJID_CSOVEREIGN			MakeOBJCLASSID(11)
#define OBJID_CARMORCLASS			MakeOBJCLASSID(12)
#define OBJID_CWEAPONCLASS			MakeOBJCLASSID(13)
#define OBJID_CITEMTYPE				MakeOBJCLASSID(14)
#define OBJID_CMARKER				MakeOBJCLASSID(15)
#define OBJID_CMISSILE				MakeOBJCLASSID(16)
#define OBJID_CPARTICLEEFFECT		MakeOBJCLASSID(17)
#define OBJID_CPOVMARKER			MakeOBJCLASSID(18)
#define OBJID_CAREADAMAGE			MakeOBJCLASSID(19)
#define OBJID_CSTATICEFFECT			MakeOBJCLASSID(20)
#define OBJID_CFLEETSHIPAI			MakeOBJCLASSID(21)
#define OBJID_CPARTICLEDAMAGE		MakeOBJCLASSID(22)
#define OBJID_CFERIANSHIPAI			MakeOBJCLASSID(23)
#define OBJID_CAUTONAI				MakeOBJCLASSID(24)
#define OBJID_CDISINTEGRATIONEFFECT	MakeOBJCLASSID(25)
#define OBJID_CFRACTUREEFFECT		MakeOBJCLASSID(26)
#define OBJID_CGLADIATORAI			MakeOBJCLASSID(27)
#define OBJID_CFLEETCOMMANDAI		MakeOBJCLASSID(28)
#define OBJID_CGAIANPROCESSORAI		MakeOBJCLASSID(29)
#define OBJID_CITEMTABLE			MakeOBJCLASSID(30)
#define OBJID_CZOANTHROPEAI			MakeOBJCLASSID(31)
#define OBJID_CRADIUSDAMAGE			MakeOBJCLASSID(32)
#define OBJID_CSHIPTABLE			MakeOBJCLASSID(33)
#define OBJID_CBOUNDARYMARKER		MakeOBJCLASSID(34)
#define OBJID_CSEQUENCEREFFECT		MakeOBJCLASSID(35)
#define OBJID_CMISSION				MakeOBJCLASSID(36)
#define OBJID_CCONTINUOUSBEAM		MakeOBJCLASSID(37)

typedef LONGLONG CurrencyValue;

static constexpr int g_DestinyRange = 360;
static constexpr int g_RotationRange = 20;
static constexpr int STD_ROTATION_COUNT = 20;

static constexpr int g_RotationAngle = (360 / g_RotationRange);

//	Measurements
//
//	Distance: in kilometers (or "klicks")
//	Time: in seconds
//	Velocity: kilometers/second

static constexpr Metric g_Epsilon =						0.0000001;			//	Small value close to 0
static constexpr Metric LIGHT_SPEED =					299792.5;			//	c in Kilometers per second
static constexpr Metric LIGHT_SECOND =					LIGHT_SPEED;		//	1 light-second
static constexpr Metric LIGHT_MINUTE =					LIGHT_SECOND * 60;	//	1 light-minute
static constexpr Metric MAX_SYSTEM_SPEED =				10.0 * LIGHT_SPEED;	//	By default we clip above this
static constexpr Metric g_AU =							149600000.0;		//	1 AU in Kilometers

//	Scales

static constexpr Metric KLICKS_PER_PIXEL =				12500.0;			//	Default kilometers per pixel
static constexpr Metric TIME_SCALE =					60.0;				//	Seconds of game time per second of real time
static constexpr Metric STD_SECONDS_PER_UPDATE =		2.0;				//	Standard adjustment when converting to ticks
static constexpr Metric MAX_SOUND_DISTANCE =			(140.0 * LIGHT_SECOND);	//	Maximum distance at which sound is heard (in light-seconds)
																			//		In practice, this works out to about 100 ls, depending on
																			//		global volume, etc.

static constexpr int g_TicksPerSecond =					30;					//	Frames per second of time
static constexpr Metric g_LRSRange =					(100.0 * LIGHT_SECOND);	//	Range of LRS
static constexpr Metric g_MapKlicksPerPixel =			g_AU / 400.0;		//	400 pixels per AU
static constexpr Metric g_InfiniteDistance =			g_AU * g_AU * 1000000.0;	//	A very large number
static constexpr Metric g_InfiniteDistance2 =			g_InfiniteDistance * g_InfiniteDistance;	//	A very large number
static constexpr Metric g_SpaceDragFactor =				0.98;				//	Coefficient of drag in space (1.0 = no drag)
static constexpr Metric g_BounceCoefficient =			0.50;				//	Speed coefficient when bouncing
static constexpr Metric g_MomentumConstant =			25.0;				//	Arbitrary constant for momentum
static constexpr Metric g_MWPerFuelUnit =				15.0;				//	Tenth-MW-ticks produced per fuel unit
static constexpr Metric g_MinSpeed2 =					100.0;				//	If moving less than this speed, force to 0
static constexpr Metric FUEL_UNITS_PER_STD_ROD =		2500.0;				//	Fuel units in a standard fuel rod equivalent

static constexpr DWORD INFINITE_TICK =					0xffffffff;			//	Highest tick

static constexpr int MAX_TECH_LEVEL =					25;
static constexpr int MAX_SYSTEM_LEVEL =					25;

//	Event Timers

static constexpr int ENVIRONMENT_ON_UPDATE_CYCLE =		15;					//	OnObjUpdate in <SpaceEnvironmentType>
static constexpr int ENVIRONMENT_ON_UPDATE_OFFSET =		3;

static constexpr int ITEM_ON_AI_UPDATE_CYCLE =			30;					//	OnAIUpdate in <ItemType>
static constexpr int ITEM_ON_AI_UPDATE_OFFSET =			0;

static constexpr int ITEM_ON_UPDATE_CYCLE =				30;					//	OnUpdate in <ItemType>
static constexpr int ITEM_ON_UPDATE_OFFSET =			7;

static constexpr int OBJECT_ON_UPDATE_CYCLE =			30;					//	OnUpdate in space objects
static constexpr int OBJECT_ON_UPDATE_OFFSET =			13;

static constexpr int OVERLAY_ON_UPDATE_CYCLE =			15;					//	OnUpdate in <OverlayType>
static constexpr int OVERLAY_ON_UPDATE_OFFSET =			11;

static constexpr int GLOBAL_ON_UPDATE_CYCLE =			30;					//	OnGlobalUpdate for all types

static constexpr int ON_SCREEN_UPDATE_CYCLE =			15;					//	OnScreenUpdate for <DockScreen>

//	Special UNIDs
//	LATER: UNID should not be hard-coded

static constexpr DWORD INVALID_UNID =					0xFFFFFFFF;

static constexpr DWORD DEFAULT_SYSTEM_TABLE_UNID =		0x0000000B;	//	tbBasicFragments

static constexpr DWORD UNID_DAMAGED_SITE_SMALL =		0x00000050;	//	ovDamagedSiteSmall
static constexpr DWORD UNID_DAMAGED_SITE_MEDIUM =		0x00000051;	//	ovDamagedSiteMedium
static constexpr DWORD UNID_DAMAGED_SITE_LARGE =		0x00000052;	//	ovDamagedSiteSmall
static constexpr DWORD UNID_DEPREZ_SITE_SMALL =			0x00000053;	//	ovDeprezSiteSmall
static constexpr DWORD UNID_MANEUVERING_THRUSTER =		0x00000090;	//	efManeuveringThrusterDefault
static constexpr DWORD UNID_MAIN_THRUSTER =				0x00000091;	//	efMainThrusterDefault

static constexpr DWORD UNID_DEFAULT_ENEMY_SHIP_ALARM =	0x000000A0;
static constexpr DWORD UNID_DEFAULT_FUEL_LOW_ALARM =	0x000000A1;
static constexpr DWORD UNID_DEFAULT_RADIATION_ALARM =	0x000000A2;
static constexpr DWORD UNID_DEFAULT_HULL_BREACH_ALARM =	0x000000A3;
static constexpr DWORD UNID_DEFAULT_REACTOR_OVERLOAD_ALARM =	0x000000A4;
static constexpr DWORD UNID_DEFAULT_SHIELDS_DOWN_ALRAM =	0x000000A5;
static constexpr DWORD UNID_DEFAULT_BUTTON_CLICK =		0x000000A6;
static constexpr DWORD UNID_DEFAULT_SELECT =			0x000000A7;
static constexpr DWORD UNID_DEFAULT_GRAVITY_ALARM =		0x000000A8;

static constexpr DWORD g_PlayerSovereignUNID =			0x00001001;	//	svPlayer
static constexpr DWORD DEFAULT_ECONOMY_UNID =			0x00001017;	//	ecCreditEconomy
static constexpr DWORD UNID_UNKNOWN_ENEMY =				0x00001018;	//	svUnknownEnemy
static constexpr DWORD UNID_NEUTRAL_SOVEREIGN =			0x00001019;	//	svNeutral

static constexpr DWORD g_ShipWreckUNID =				0x00002001;	//	stShipwreck
static constexpr DWORD FLOTSAM_UNID =					0x0000200E;	//	stFlotsam

static constexpr DWORD PLAYER_EI500_FREIGHTER_UNID =	0x00003802;	//	scEI100XPlayer

static constexpr DWORD TRITIUM_PROPULSION_UPGRADE_UNID = 0x0000404B;	//	Tritium propulsion upgrade
static constexpr DWORD CARGO_HOLD_EXPANSION_UNID =		0x00004079;	//	Cargo hold expansion
static constexpr DWORD g_SuperconductingShieldsUNID =	0x0000407D;	//	Superconducting shield generator
static constexpr DWORD g_SuperconductingCoilUNID =		0x0000407E;	//	Superconducting shield generator
static constexpr DWORD NOVA25_REACTOR_UNID =			0x00004107;	//	Nova-25 reactor

static constexpr DWORD UNID_KINETIC_EXPLOSION_1 =		0x00005007;
static constexpr DWORD UNID_BLAST_EXPLOSION_1 =			0x0000500B;
static constexpr DWORD UNID_THERMO_EXPLOSION_1 =		0x0000500F;
static constexpr DWORD UNID_PLASMA_EXPLOSION_1 =		0x00005013;
static constexpr DWORD UNID_ANTIMATTER_EXPLOSION_1 =	0x00030030;
static constexpr DWORD UNID_GRAVITON_EXPLOSION_1 =		0x00030034;

static constexpr DWORD g_ExplosionUNID =				0x00009001;	//	efMediumExplosion1
static constexpr DWORD g_LargeExplosionUNID =			0x00009002;	//	efLargeExplosion1
static constexpr DWORD g_HitEffectUNID =				0x00009003;	//	efSmallExplosion1
static constexpr DWORD g_ShieldEffectUNID =				0x00009004;	//	efShieldHit1
static constexpr DWORD g_StargateInUNID =				0x00009005;	//	efStargateIn
static constexpr DWORD g_StargateOutUNID =				0x00009006;	//	efStargateOut
static constexpr DWORD g_StationDestroyedUNID =			0x00009009;	//	efStationExplosion
static constexpr DWORD LARGE_STATION_DAMAGE_UNID =		0x0000900E;	//	efLargeStationDamage
static constexpr DWORD MEDIUM_STATION_DAMAGE_UNID =		0x0000900F;	//	efMediumStationDamage

static constexpr DWORD DEFAULT_SHIP_SCREEN_UNID =		0x0000A001;	//	dsShipInterior

static constexpr DWORD g_DamageImageUNID =				0x0000F001;	//	rsMediumDamage
static constexpr DWORD SHIP_INTERIOR_IMAGE_UNID =		0x0000F002;	//	rsShipInterior
static constexpr DWORD g_LRSImageUNID =					0x0000F006;	//	rsZubrinLRS
static constexpr DWORD UNID_TARGETING_HUD_BACKGROUND =	0x0000F007;	//	rsZubrinTargeting
static constexpr DWORD g_SRSSnowImageUNID =				0x0000F008;	//	rsSRSSnow
static constexpr DWORD g_LRSBorderUNID =				0x0000F009;	//	rsZubrinLRSBorder
static constexpr DWORD DEFAULT_REACTOR_DISPLAY_IMAGE =	0x0000F00C;	//	rsZubrinReactor
static constexpr DWORD DEFAULT_DOCK_SCREEN_IMAGE_UNID = 0x0000F013;	//	Default dock screen background
static constexpr DWORD DEFAULT_DOCK_SCREEN_MASK_UNID =	0x0000F014;	//	Default dock screen mask
static constexpr DWORD g_ShipExplosionParticlesUNID =	0x0000F114;	//	rsDebris1
static constexpr DWORD g_ShipExplosionSoundUNID =		0x0000F204;	//	snShipExplosion1
static constexpr DWORD g_StationExplosionSoundUNID =	0x0000F205;	//	snStationExplosion1

static constexpr DWORD DEFAULT_DOCK_SERVICES_SCREEN =	0x00010029;
static constexpr DWORD COMPATIBLE_DOCK_SERVICES_SCREEN = 0x0001002A;
static constexpr DWORD COMPATIBLE_SHIP_CONFIG_SCREEN =	0x0000A01B;

static constexpr DWORD UNID_DEFAULT_SYSTEM_BACKGROUND =	0x00030001;
static constexpr DWORD UNID_DISINTEGRATION_EXPLOSION =	0x00030044;
static constexpr DWORD UNID_TIME_STOP_OVERLAY =			0x00030080;

#ifdef DEBUG_HENCHMAN
const DWORD g_DebugHenchmenShipUNID =		0x00103001;	//	scCenturion
#endif

static constexpr DWORD UNID_RPG_LIBRARY =						0x00010000;
static constexpr DWORD UNID_UNIVERSE_LIBRARY =					0x00020000;
static constexpr DWORD UNID_CORE_TYPES_LIBRARY =				0x00030000;
static constexpr DWORD UNID_HUMAN_SPACE_LIBRARY =				0x00100000;
static constexpr DWORD DEFAULT_ADVENTURE_EXTENSION_UNID =		0x00200000;
static constexpr DWORD UNID_PILGRIM_CHARACTER_CLASS =			0x00201002;
static constexpr DWORD DEFAULT_COMPATIBILITY_LIBRARY_UNID =	0x00700000;
static constexpr DWORD UNID_SOUNDTRACK_EXTENSION =				0x00820000;
static constexpr DWORD UNID_HD_EXTENSION =						0x00830000;

static constexpr DWORD UNID_DIAGNOSTICS_EXTENSION =			0xA0010000;

//	Object IDs

static constexpr DWORD OBJID_NULL =						0xFFFFFFFF;

//	Global constants

extern Metric g_KlicksPerPixel;
extern Metric g_TimeScale;
extern Metric g_SecondsPerUpdate;

//	Damage Types ---------------------------------------------------------------

enum DamageTypes
	{
	damageError			= -100,					//	invalid damage
	damageGeneric		= -1,					//	generic damage

	damageLaser			= 0,					//	standard lasers
	damageKinetic		= 1,					//	mass drivers
	damageParticle		= 2,					//	charged particle beam
	damageBlast			= 3,					//	chemical explosives
	damageIonRadiation	= 4,					//	ionizing radiation
	damageThermonuclear	= 5,					//	hydrogen bomb
	damagePositron		= 6,					//	anti-matter charged particles
	damagePlasma		= 7,					//	fusion weapons
	damageAntiMatter	= 8,					//	anti-matter torpedo
	damageNano			= 9,					//	nano-machines
	damageGravitonBeam	= 10,					//	graviton beam
	damageSingularity	= 11,					//	spacetime weapons
	damageDarkAcid		= 12,					//	exotics
	damageDarkSteel		= 13,					//	exotics
	damageDarkLightning	= 14,					//	exotics
	damageDarkFire		= 15,					//	exotics

	damageCount			= 16
	};

//	Utility types

enum FrequencyTypes
	{
	ftCommon				= 20,
	ftUncommon				= 10,
	ftRare					= 4,
	ftVeryRare				= 1,
	ftNotRandom				= 0
	};

//	Other types

enum GenomeTypes
	{
	genomeUnknown		= 0,					//	They, their, them
	genomeHumanMale		= 1,					//	He, his, him
	genomeHumanFemale	= 2,					//	She, her, her
	genderNeuter		= 3,					//	It, its, it

	genomeCount			= 4,
	};
