//	CEngineOptions.cpp
//
//	CEngineOptions class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define LEVEL_ATTRIB							CONSTLIT("level")

#define TAG_DAMAGE_METHOD_CURVE_CRUSH			CONSTLIT("crush")
#define TAG_DAMAGE_METHOD_CURVE_PIERCE			CONSTLIT("pierce")
#define TAG_DAMAGE_METHOD_CURVE_SHRED			CONSTLIT("shred")
#define TAG_DAMAGE_METHOD_CURVE_WMD				CONSTLIT("wmd")

//	Damage Adjustment

static int g_StdArmorDamageAdj[MAX_ITEM_LEVEL][damageCount] =
	{
		//	lsr knt par blt  ion thr pos pls  am  nan grv sng  dac dst dlg dfr
		{	100,100,100,100, 100,100,100,100, 125,125,200,200, 300,300,500,500 },
		{	 80, 80,100,100, 100,100,100,100, 125,125,200,200, 300,300,500,500 },
		{	 60, 60,100,100, 100,100,100,100, 125,125,200,200, 300,300,500,500 },
		{	 40, 40,100,100, 100,100,100,100, 100,100,125,125, 200,200,300,300 },
		{	 25, 25, 80, 80, 100,100,100,100, 100,100,125,125, 200,200,300,300 },

		{	 14, 14, 60, 60, 100,100,100,100, 100,100,125,125, 200,200,300,300 },
		{	  8,  8, 40, 40, 100,100,100,100, 100,100,100,100, 125,125,200,200 },
		{	  4,  4, 25, 25,  80, 80,100,100, 100,100,100,100, 125,125,200,200 },
		{	  2,  2, 14, 14,  60, 60,100,100, 100,100,100,100, 125,125,200,200 },
		{	  1,  1,  8,  8,  40, 40,100,100, 100,100,100,100, 100,100,125,125 },

		{	  0,  0,  4,  4,  25, 25, 80, 80, 100,100,100,100, 100,100,125,125 },
		{	  0,  0,  2,  2,  14, 14, 60, 60, 100,100,100,100, 100,100,125,125 },
		{	  0,  0,  1,  1,   8,  8, 40, 40, 100,100,100,100, 100,100,100,100 },
		{	  0,  0,  0,  0,   4,  4, 25, 25,  80, 80,100,100, 100,100,100,100 },
		{	  0,  0,  0,  0,   2,  2, 14, 14,  60, 60,100,100, 100,100,100,100 },

		{	  0,  0,  0,  0,   1,  1,  8,  8,  40, 40,100,100, 100,100,100,100 },
		{	  0,  0,  0,  0,   0,  0,  4,  4,  25, 25, 80, 80, 100,100,100,100 },
		{	  0,  0,  0,  0,   0,  0,  2,  2,  14, 14, 60, 60, 100,100,100,100 },
		{	  0,  0,  0,  0,   0,  0,  1,  1,   8,  8, 40, 40, 100,100,100,100 },
		{	  0,  0,  0,  0,   0,  0,  0,  0,   4,  4, 25, 25,  80, 80,100,100 },

		{	  0,  0,  0,  0,   0,  0,  0,  0,   2,  2, 14, 14,  60, 60,100,100 },
		{	  0,  0,  0,  0,   0,  0,  0,  0,   1,  1,  8,  8,  40, 40,100,100 },
		{	  0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  4,  4,  25, 25, 80, 80 },
		{	  0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  2,  2,  14, 14, 60, 60 },
		{	  0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  1,  1,   8,  8, 40, 40 },
	};

static int g_StdShieldDamageAdj[MAX_ITEM_LEVEL][damageCount] =
	{
		//	lsr knt par blt  ion thr pos pls  am  nan grv sng  dac dst dlg dfr
		{	100,100,100,100, 100,100,160,160, 225,225,300,300, 375,375,500,500 },
		{	 95, 95,100,100, 100,100,140,140, 200,200,275,275, 350,350,450,450 },
		{	 90, 90,100,100, 100,100,120,120, 180,180,250,250, 325,325,400,400 },
		{	 85, 85,100,100, 100,100,100,100, 160,160,225,225, 300,300,375,375 },
		{	 80, 80, 95, 95, 100,100,100,100, 140,140,200,200, 275,275,350,350 },

		{	 75, 75, 90, 90, 100,100,100,100, 120,120,180,180, 250,250,325,325 },
		{	 70, 70, 85, 85, 100,100,100,100, 100,100,160,160, 225,225,300,300 },
		{	 65, 65, 80, 80,  95, 95,100,100, 100,100,140,140, 200,200,275,275 },
		{	 60, 60, 75, 75,  90, 90,100,100, 100,100,120,120, 180,180,250,250 },
		{	 55, 55, 70, 70,  85, 85,100,100, 100,100,100,100, 160,160,225,225 },

		{	 50, 50, 65, 65,  80, 80, 95, 95, 100,100,100,100, 140,140,200,200 },
		{	 40, 40, 60, 60,  75, 75, 90, 90, 100,100,100,100, 120,120,180,180 },
		{	 30, 30, 55, 55,  70, 70, 85, 85, 100,100,100,100, 100,100,160,160 },
		{	 20, 20, 50, 50,  65, 65, 80, 80,  95, 95,100,100, 100,100,140,140 },
		{	 10, 10, 40, 40,  60, 60, 75, 75,  90, 90,100,100, 100,100,120,120 },

		{	  0,  0, 30, 30,  55, 55, 70, 70,  85, 85,100,100, 100,100,100,100 },
		{	  0,  0, 20, 20,  50, 50, 65, 65,  80, 80, 95, 95, 100,100,100,100 },
		{	  0,  0, 10, 10,  40, 40, 60, 60,  75, 75, 90, 90, 100,100,100,100 },
		{	  0,  0,  0,  0,  30, 30, 55, 55,  70, 70, 85, 85, 100,100,100,100 },
		{	  0,  0,  0,  0,  20, 20, 50, 50,  65, 65, 80, 80,  95, 95,100,100 },

		{	  0,  0,  0,  0,  10, 10, 40, 40,  60, 60, 75, 75,  90, 90,100,100 },
		{	  0,  0,  0,  0,   0,  0, 30, 30,  55, 55, 70, 70,  85, 85,100,100 },
		{	  0,  0,  0,  0,   0,  0, 20, 20,  50, 50, 65, 65,  80, 80, 95, 95 },
		{	  0,  0,  0,  0,   0,  0, 10, 10,  40, 40, 60, 60,  75, 75, 90, 90 },
		{	  0,  0,  0,  0,   0,  0,  0,  0,  30, 30, 55, 55,  70, 70, 85, 85 },
	};

static bool g_bDamageAdjInit = false;
static CDamageAdjDesc g_ArmorDamageAdj[MAX_ITEM_LEVEL];
static CDamageAdjDesc g_ShieldDamageAdj[MAX_ITEM_LEVEL];

//	Mining ----------------------------------------------------------------------

//	This table is used for API0-47 adventures

static const TArray<int> g_StdMiningMaxOreLevelsAPI0 =
		//	lsr knt par blt  ion thr pos pls  am  nan grv sng  dac dst dlg dfr
		{	 25, 25, 25, 25,  25, 25, 25, 25,  25, 25, 25, 25,  25, 25, 25, 25 };

//	This table is used for API48-52 adventures

static const TArray<int> g_StdMiningMaxOreLevelsAPI48 =
		//	lsr knt par blt  ion thr pos pls  am  nan grv sng  dac dst dlg dfr
		{	  4,  4,  7,  7,  10, 10, 13, 13,  16, 16, 19, 19,  22, 22, 25, 25 };

//	This table is used for API53-56 adventures

static const TArray<int> g_StdMiningMaxOreLevelsAPI53 =
		//	lsr knt par blt  ion thr pos pls  am  nan grv sng  dac dst dlg dfr
		{	  5,  5,  8,  8,  11, 11, 14, 14,  17, 17, 20, 20,  23, 23, 25, 25 };

//	This table is used for API57+ adventures

static const TArray<int> g_StdMiningMaxOreLevelsAPI57 =
	{
		MAX_ITEM_LEVEL,	//	laser
		MAX_ITEM_LEVEL,	//	kinetic
		MAX_ITEM_LEVEL,	//	particle
		MAX_ITEM_LEVEL,	//	blast

		MAX_ITEM_LEVEL,	//	ion
		MAX_ITEM_LEVEL,	//	thermo
		MAX_ITEM_LEVEL,	//	positron
		MAX_ITEM_LEVEL,	//	plasma

		MAX_ITEM_LEVEL,	//	antimatter
		MAX_ITEM_LEVEL,	//	nanite
		MAX_ITEM_LEVEL,	//	graviton
		MAX_ITEM_LEVEL,	//	singularity

		MAX_ITEM_LEVEL,	//	dark acid
		MAX_ITEM_LEVEL,	//	dark steel
		MAX_ITEM_LEVEL,	//	dark lightning
		MAX_ITEM_LEVEL,	//	dark fire
	};

//	External Device Damage --------------------------------------------------

//	This table is used for API0-57 adventures

static const TArray<int> g_StdExternalDeviceDamageLevelsAPI0 =
//		lsr knt par blt  ion thr pos pls  am  nan grv sng  dac dst dlg dfr
	{	  6,  6,  9,  9,  12, 12, 15, 15,  18, 18, 21, 21,  24, 24, 27, 27 };

//	This table is used for API58+ adventures

static const TArray<int> g_StdExternalDeviceDamageLevelsAPI58 =
	{
	MAX_ITEM_LEVEL,	//	laser
	MAX_ITEM_LEVEL,	//	kinetic
	MAX_ITEM_LEVEL,	//	particle
	MAX_ITEM_LEVEL,	//	blast

	MAX_ITEM_LEVEL,	//	ion
	MAX_ITEM_LEVEL,	//	thermo
	MAX_ITEM_LEVEL,	//	positron
	MAX_ITEM_LEVEL,	//	plasma

	MAX_ITEM_LEVEL,	//	antimatter
	MAX_ITEM_LEVEL,	//	nanite
	MAX_ITEM_LEVEL,	//	graviton
	MAX_ITEM_LEVEL,	//	singularity

	MAX_ITEM_LEVEL,	//	dark acid
	MAX_ITEM_LEVEL,	//	dark steel
	MAX_ITEM_LEVEL,	//	dark lightning
	MAX_ITEM_LEVEL,	//	dark fire
	};

//	This table is used for API0-57 adventures

static const TArray<int> g_StdExternalDeviceDamageModifierAPI0 =
//		lsr knt par blt  ion thr pos pls  am  nan grv sng  dac dst dlg dfr
	{	100,100,100,100, 120,100,100,100, 100,100, 75,100, 100,100,100,100 };

//	This table is used for API58+ adventures

static const TArray<int> g_StdExternalDeviceDamageModifierAPI58 =
//		lsr knt par blt  ion thr pos pls  am  nan grv sng  dac dst dlg dfr
	{	100,100,100,100, 100,100,100,100, 100,100,100,100, 100,100,100,100 };

static constexpr int g_iExternalChanceToHitAPI0 = 11;
static constexpr int g_iExternalChanceToHitAPI58 = 100;

//	Internal Device Damage --------------------------------------------------

//	This table is used for API0-57 adventures

static const TArray<int> g_StdInternalDeviceDamageLevelsAPI0 =
//		lsr knt par blt  ion thr pos pls  am  nan grv sng  dac dst dlg dfr
	{	 25,  25, 25, 25,  25, 25, 25, 25,  25, 25, 25, 25,  25, 25, 25, 25 };

//	This table is used for API58+ adventures

static const TArray<int> g_StdInternalDeviceDamageLevelsAPI58 =
	{
	MAX_ITEM_LEVEL,	//	laser
	MAX_ITEM_LEVEL,	//	kinetic
	MAX_ITEM_LEVEL,	//	particle
	MAX_ITEM_LEVEL,	//	blast

	MAX_ITEM_LEVEL,	//	ion
	MAX_ITEM_LEVEL,	//	thermo
	MAX_ITEM_LEVEL,	//	positron
	MAX_ITEM_LEVEL,	//	plasma

	MAX_ITEM_LEVEL,	//	antimatter
	MAX_ITEM_LEVEL,	//	nanite
	MAX_ITEM_LEVEL,	//	graviton
	MAX_ITEM_LEVEL,	//	singularity

	MAX_ITEM_LEVEL,	//	dark acid
	MAX_ITEM_LEVEL,	//	dark steel
	MAX_ITEM_LEVEL,	//	dark lightning
	MAX_ITEM_LEVEL,	//	dark fire
	};

//	This table is used for API0-57 adventures

static const TArray<int> g_StdInternalDeviceDamageModifierAPI0 =
//		lsr knt par blt  ion thr pos pls  am  nan grv sng  dac dst dlg dfr
	{	100,100,100,100, 100,100,100,100, 100,100,100,100, 100,100,100,100 };

//	This table is used for API58+ adventures

static const TArray<int> g_StdInternalDeviceDamageModifierAPI58 =
//		lsr knt par blt  ion thr pos pls  am  nan grv sng  dac dst dlg dfr
	{	100,100,100,100, 100,100,100,100, 100,100,100,100, 100,100,100,100 };

static constexpr int g_iInternalChanceToHitAPI0 = 50;
static constexpr int g_iInternalChanceToHitAPI58 = 100;

//	Mass Destruction (WMD) Adj ----------------------------------------------

static const TArray<double> g_StdWMDAdjAPI0 =
//		0		1		2		3		4		5		6		7
	{	0.0,	0.04,	0.1,	0.2,	0.34,	0.52,	0.74,	1.0};

static const TArray<const char*> g_StdWMDLabelsAPI0 =
//		0		1		2		3		4		5		6		7
	{	"",		"1",	"2",	"3",	"4",	"5",	"7",	"10"};

static constexpr int g_iStdWMDMinDamageAPI0 = 1;

static const TArray<double> g_StdWMDAdjAPI29 =
//		0		1		2		3		4		5		6		7
	{	0.10,	0.25,	0.32,	0.40,	0.50,	0.63,	0.80,	1.0};

static const TArray<const char*> g_StdWMDLabelsAPI29 =
//		0		1		2		3		4		5		6		7
	{	"",		"2",	"3",	"4",	"5",	"6",	"8",	"10"};

static constexpr int g_iStdWMDMinDamageAPI29 = 0;

//	Helpers -----------------------------------------------------------------

//	Translates actual API versions into the known versions
//	used for the MiningMaxOreLevel system
//
int GetAPIForMiningMaxOreLevel (int apiVersion)

	{
	if (apiVersion >= 57)
		return 57;
	else if (apiVersion >= 53)
		return 53;
	else if (apiVersion >= 48)
		return 48;
	else
		return 0;
	}

//	Translates actual API versions into the known versions
//	used for the MiningMaxOreLevel system
//
int GetAPIForExternalDeviceDamageMaxLevel (int apiVersion)

	{
	if (apiVersion >= 58)
		return 58;
	else
		return 0;
	}

//	Translates actual API versions into the known versions
//	used for the MiningMaxOreLevel system
//
int GetAPIForInternalDeviceDamageMaxLevel (int apiVersion)

	{
	if (apiVersion >= 58)
		return 58;
	else
		return 0;
	}

//	Translates actual API versions into the known versions
//	used for the WMD Adj system
//
int GetAPIForWMDAdj (int apiVersion)

	{
	if (apiVersion >= 29)
		return 29;
	else
		return 0;
	}

//	Class Methods -------------------------------------------------------------

//	CEngineOptions constructor
//
CEngineOptions::CEngineOptions (int apiVersion)

	{
	//	Set api version we are loading defaults for

	m_iDefaultForAPIVersion = apiVersion;

	//	Initialize globals

	InitDefaultGlobals();
	}

//	Initialize default globals based on whatever API version we need
//
void CEngineOptions::InitDefaultGlobals ()

	{
	//	Initialize armor and shield damage adjustment tables

	InitDefaultDamageAdj();
	for (int i = 1; i <= MAX_ITEM_LEVEL; i++)
		{
		m_ArmorDamageAdj[i - 1] = g_ArmorDamageAdj[i - 1];
		m_ShieldDamageAdj[i - 1] = g_ShieldDamageAdj[i - 1];
		}
	m_bCustomArmorDamageAdj = false;
	m_bCustomShieldDamageAdj = false;

	//	Initialize mining tables

	m_MiningDamageMaxOreLevels = GetDefaultMiningMaxOreLevels(m_iDefaultForAPIVersion);
	m_bCustomMiningMaxOreLevels = false;

	//	Initialize device damage tables

	m_ExternalDeviceDamageMaxLevels = GetDefaultExternalDeviceDamageLevels(m_iDefaultForAPIVersion);
	m_bCustomExternalDeviceDamageMaxLevels = false;
	m_InternalDeviceDamageMaxLevels = GetDefaultInternalDeviceDamageLevels(m_iDefaultForAPIVersion);
	m_bCustomInternalDeviceDamageMaxLevels = false;

	//	Initialize WMD adj tables

	m_MassDestruction = GetDefaultWMDAdj(m_iDefaultForAPIVersion);
	m_bCustomMassDestruction = false;
	}

//	Initiate defaults where necessary based on whatever API version we need
//	Assumes that InitDefaultGlobals was called during our constructor
//
void CEngineOptions::InitDefaultDescs ()

	{
	//	Initialize armor and shield damage adjustment tables

	for (int i = 1; i <= MAX_ITEM_LEVEL; i++)
		{
		if (!m_bCustomArmorDamageAdj)
			m_ArmorDamageAdj[i - 1] = g_ArmorDamageAdj[i - 1];
		if (!m_bCustomShieldDamageAdj)
			m_ShieldDamageAdj[i - 1] = g_ShieldDamageAdj[i - 1];
		}

	//	Initialize mining tables

	if (!m_bCustomMiningMaxOreLevels)
		m_MiningDamageMaxOreLevels = GetDefaultMiningMaxOreLevels(m_iDefaultForAPIVersion);

	//	Initialize device damage tables

	if (!m_bCustomExternalDeviceDamageMaxLevels)
		m_ExternalDeviceDamageMaxLevels = GetDefaultExternalDeviceDamageLevels(m_iDefaultForAPIVersion);

	if (!m_bCustomInternalDeviceDamageMaxLevels)
		m_InternalDeviceDamageMaxLevels = GetDefaultInternalDeviceDamageLevels(m_iDefaultForAPIVersion);

	}

//	InitDefaultDamageMethods
//
//	m_iDamageMethodSystem must be initialized to a valid value already
//
void CEngineOptions::InitDefaultDamageMethods()
	{

	switch (m_iDamageMethodSystem)
		{
		case EDamageMethodSystem::dmgMethodSysPhysicalized:
			{
			//	Items

			m_DamageMethodItemAdj.Armor.PhysicalizedAdj.Reset();
			m_DamageMethodItemAdj.Shield.PhysicalizedAdj.Reset();

			//	Ships

			m_DamageMethodShipAdj.Armor.Critical.PhysicalizedAdj.Reset();
			m_DamageMethodShipAdj.Armor.Critical.PhysicalizedAdj.rShred = 0.1;
			m_DamageMethodShipAdj.Armor.CriticalUncrewed.PhysicalizedAdj.Reset();
			m_DamageMethodShipAdj.Armor.CriticalUncrewed.PhysicalizedAdj.rShred = 0.1;
			m_DamageMethodShipAdj.Armor.NonCritical.PhysicalizedAdj.Reset();
			m_DamageMethodShipAdj.Armor.NonCritical.PhysicalizedAdj.rPierce = 0.1;
			m_DamageMethodShipAdj.Armor.NonCriticalDestruction.PhysicalizedAdj.Reset();
			m_DamageMethodShipAdj.Armor.NonCriticalDestruction.PhysicalizedAdj.rShred = 0.1;

			m_DamageMethodShipAdj.Compartment.General.PhysicalizedAdj.Reset();
			m_DamageMethodShipAdj.Compartment.General.PhysicalizedAdj.rShred = 0.1;
			m_DamageMethodShipAdj.Compartment.Cargo.PhysicalizedAdj.Reset();
			m_DamageMethodShipAdj.Compartment.Cargo.PhysicalizedAdj.rCrush = 0.1;
			m_DamageMethodShipAdj.Compartment.MainDrive.PhysicalizedAdj.Reset();
			m_DamageMethodShipAdj.Compartment.MainDrive.PhysicalizedAdj.rShred = 0.1;
			m_DamageMethodShipAdj.Compartment.Uncrewed.PhysicalizedAdj.Reset();
			m_DamageMethodShipAdj.Compartment.Uncrewed.PhysicalizedAdj.rCrush = 0.1;

			//	Stations

			m_DamageMethodStationAdj.Hull.Armor.PhysicalizedAdj.Reset();
			m_DamageMethodStationAdj.Hull.Armor.PhysicalizedAdj.rPierce = 0.1;
			m_DamageMethodStationAdj.Hull.Asteroid.PhysicalizedAdj.Reset();
			m_DamageMethodStationAdj.Hull.Asteroid.PhysicalizedAdj.rCrush = 0.1;
			m_DamageMethodStationAdj.Hull.Multi.PhysicalizedAdj.Reset();
			m_DamageMethodStationAdj.Hull.Multi.PhysicalizedAdj.rShred = 0.1;
			m_DamageMethodStationAdj.Hull.Single.PhysicalizedAdj.Reset();
			m_DamageMethodStationAdj.Hull.Uncrewed.PhysicalizedAdj.Reset();
			m_DamageMethodStationAdj.Hull.Uncrewed.PhysicalizedAdj.rCrush = 0.1;
			m_DamageMethodStationAdj.Hull.Underground.PhysicalizedAdj.Reset();
			m_DamageMethodStationAdj.Hull.Underground.PhysicalizedAdj.rCrush = 0.3;
			m_DamageMethodStationAdj.Hull.Underground.PhysicalizedAdj.rPierce = 0.3;
			}
		case EDamageMethodSystem::dmgMethodSysWMD:
			{
			//	Items

			m_DamageMethodItemAdj.Armor.WMDAdj.rWMD = 1.0;
			m_DamageMethodItemAdj.Shield.WMDAdj.rWMD = 1.0;

			//	Ships

			m_DamageMethodShipAdj.Armor.Critical.WMDAdj.rWMD = 0.1;
			m_DamageMethodShipAdj.Armor.CriticalUncrewed.WMDAdj.rWMD = 0.1;
			m_DamageMethodShipAdj.Armor.NonCritical.WMDAdj.rWMD = 0.1;
			m_DamageMethodShipAdj.Armor.NonCriticalDestruction.WMDAdj.rWMD = 0.1;

			m_DamageMethodShipAdj.Compartment.General.WMDAdj.rWMD = 0.1;
			m_DamageMethodShipAdj.Compartment.Cargo.WMDAdj.rWMD = 0.1;
			m_DamageMethodShipAdj.Compartment.MainDrive.WMDAdj.rWMD = 0.1;
			m_DamageMethodShipAdj.Compartment.Uncrewed.WMDAdj.rWMD = 0.1;

			//	Stations

			m_DamageMethodStationAdj.Hull.Armor.WMDAdj.rWMD = 0.1;
			m_DamageMethodStationAdj.Hull.Asteroid.WMDAdj.rWMD = 0.1;
			m_DamageMethodStationAdj.Hull.Multi.WMDAdj.rWMD = 0.1;
			m_DamageMethodStationAdj.Hull.Single.WMDAdj.rWMD = 1.0;
			m_DamageMethodStationAdj.Hull.Uncrewed.WMDAdj.rWMD = 0.1;
			m_DamageMethodStationAdj.Hull.Underground.WMDAdj.rWMD = 0.1;
			}
		default:
			ASSERT(false);
		}
	}

const CDamageMethodDesc* CEngineOptions::GetDamageMethodDesc(EDamageMethod iMethod) const
	{
	switch (m_iDamageMethodSystem)
		{
		case EDamageMethodSystem::dmgMethodSysWMD:
			{
			if (iMethod == EDamageMethod::methodWMD)
				return  &(m_DamageMethodDescs.WMD.WMD);
			ASSERT(false);
			return NULL;
			}
		case EDamageMethodSystem::dmgMethodSysPhysicalized:
			{
			switch (iMethod)
				{
				case EDamageMethod::methodCrush:
					return &(m_DamageMethodDescs.Physicalized.Crush);
				case EDamageMethod::methodPierce:
					return &(m_DamageMethodDescs.Physicalized.Pierce);
				case EDamageMethod::methodShred:
					return &(m_DamageMethodDescs.Physicalized.Shred);
				default:
					{
					ASSERT(false);
					return NULL;
					}
				}
			}
		default:
			{
			ASSERT(false);
			return NULL;
			}
		}
	}

//	HidesArmorImmunity
//
//	Returns TRUE if we should hide the given immunity from armor UI displays.
//	We do this on high-level adventures (e.g., VotG) when all armors are immune
//	to a particular special damage.
//
bool CEngineOptions::HidesArmorImmunity (SpecialDamageTypes iSpecial) const

	{
	switch (iSpecial)
		{
		case specialBlinding:
		case specialDeviceDamage:
		case specialDeviceDisrupt:
		case specialEMP:
			return m_bHideIonizeImmune;

		case specialDisintegration:
			return m_bHideDisintegrationImmune;

		case specialRadiation:
			return m_bHideRadiationImmune;

		case specialShatter:
			return m_bHideShatterImmune;

		default:
			return false;
		}
	}

//	InitDamageAdjFromXML
//
//	Initializes from XML.
//
bool CEngineOptions::InitDamageAdjFromXML (SDesignLoadCtx &Ctx, const CXMLElement &XMLDesc, CDamageAdjDesc *DestTable)

	{
	int iLevel = XMLDesc.GetAttributeInteger(LEVEL_ATTRIB);
	if (iLevel < 1 || iLevel > MAX_ITEM_LEVEL)
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Invalid level: %d."), iLevel);
		return false;
		}

	if (DestTable[iLevel - 1].InitFromXML(Ctx, XMLDesc, true) != NOERROR)
		return false;

	//	Success!

	return true;
	}

//	InitDamageMethodAdjFromCC
//
//	Use an ICCItem loaded from an adventure property to initialize
//	Damage Method Adj
// 
//	Note: Requires adj to be pre-configured to the appropriate defaults
//
bool CEngineOptions::InitDamageMethodAdjFromCC(SDesignLoadCtx& Ctx, UDamageMethodAdj& adj, ICCItem* pStruct)
	{
	switch (m_iDamageMethodSystem)
		{
		case EDamageMethodSystem::dmgMethodSysPhysicalized:
			{
			adj.PhysicalizedAdj.rCrush = min(pStruct->GetDoubleAt(KEY_CORE_DMG_METHOD_CRUSH, adj.PhysicalizedAdj.rCrush), 1.0);
			adj.PhysicalizedAdj.rPierce = min(pStruct->GetDoubleAt(KEY_CORE_DMG_METHOD_PIERCE, adj.PhysicalizedAdj.rPierce), 1.0);
			adj.PhysicalizedAdj.rShred = min(pStruct->GetDoubleAt(KEY_CORE_DMG_METHOD_SHRED, adj.PhysicalizedAdj.rShred), 1.0);
			return true;
			}
		case EDamageMethodSystem::dmgMethodSysWMD:
			{
			adj.WMDAdj.rWMD = min(pStruct->GetDoubleAt(KEY_CORE_DMG_METHOD_WMD, adj.WMDAdj.rWMD), 1.0);
			return true;
			}
		default:
			{
			ASSERT(false);
			Ctx.sError = CONSTLIT("Engine error occurred in initialing damage method adj: m_iDamageMethodSystem was not set to a valid value");
			return false;
			}
		}
	}

//	InitDamageAdjFromXML
//
//	Initializes from XML.
//
bool CEngineOptions::InitMiningMaxOreLevelsFromXML (SDesignLoadCtx& Ctx, const CXMLElement& XMLDesc)

	{
	m_bCustomMiningMaxOreLevels = true;

	if (m_MiningDamageMaxOreLevels.InitFromXML(Ctx, XMLDesc) != NOERROR)
		return false;

	//	Success!

	return true;
	}

//	InitExternalDeviceDamageMaxLevelsFromXML
//
//	Initializes from XML.
//
bool CEngineOptions::InitExternalDeviceDamageMaxLevelsFromXML (SDesignLoadCtx& Ctx, const CXMLElement& XMLDesc)

	{
	m_bCustomExternalDeviceDamageMaxLevels = true;

	if (m_ExternalDeviceDamageMaxLevels.InitFromXML(Ctx, XMLDesc) != NOERROR)
		return false;

	//	Success!

	return true;
	}

//	InitInternalDeviceDamageMaxLevelsFromXML
//
//	Initializes from XML.
//
bool CEngineOptions::InitInternalDeviceDamageMaxLevelsFromXML (SDesignLoadCtx& Ctx, const CXMLElement& XMLDesc)

	{
	m_bCustomInternalDeviceDamageMaxLevels = true;

	if (m_InternalDeviceDamageMaxLevels.InitFromXML(Ctx, XMLDesc) != NOERROR)
		return false;

	//	Success!

	return true;
	}

//	InitDamageMethodDescsFromXML
//
//	Initializes from XML.
//  Init from properties must have already run.
//
bool CEngineOptions::InitDamageMethodDescsFromXML(SDesignLoadCtx& Ctx, const CXMLElement& XMLDesc)
	{
	m_bCustomDamageMethodDescs = true;
	bool bSetPhysicalized = false;
	bool bSetWMD = false;

	for (int i = 0; i < XMLDesc.GetContentElementCount(); i++)
		{
		CXMLElement* pItem = XMLDesc.GetContentElement(i);

		if (strEquals(pItem->GetTag(), TAG_DAMAGE_METHOD_CURVE_CRUSH))
			{
			if (bSetWMD)
				{
				Ctx.sError = CONSTLIT("Cannot specify damage method curves from different damage method systems");
				return false;
				}
			if (m_DamageMethodDescs.Physicalized.Crush.InitFromXML(Ctx, *pItem) != NOERROR)
				return false;
			bSetPhysicalized = true;
			}
		else if (strEquals(pItem->GetTag(), TAG_DAMAGE_METHOD_CURVE_PIERCE))
			{
			if (bSetWMD)
				{
				Ctx.sError = CONSTLIT("Cannot specify damage method curves from different damage method systems");
				return false;
				}
			if (m_DamageMethodDescs.Physicalized.Pierce.InitFromXML(Ctx, *pItem) != NOERROR)
				return false;
			bSetPhysicalized = true;
			}
		else if (strEquals(pItem->GetTag(), TAG_DAMAGE_METHOD_CURVE_SHRED))
			{
			if (bSetWMD)
				{
				Ctx.sError = CONSTLIT("Cannot specify damage method curves from different damage method systems");
				return false;
				}
			if (m_DamageMethodDescs.Physicalized.Shred.InitFromXML(Ctx, *pItem) != NOERROR)
				return false;
			bSetPhysicalized = true;
			}
		else if (strEquals(pItem->GetTag(), TAG_DAMAGE_METHOD_CURVE_WMD))
			{
			if (bSetPhysicalized)
				{
				Ctx.sError = CONSTLIT("Cannot specify damage method curves from different damage method systems");
				return false;
				}
			if (m_DamageMethodDescs.WMD.WMD.InitFromXML(Ctx, *pItem) != NOERROR)
				return false;
			bSetWMD = true;
			}
		else
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Invalid DamageMethodAdj curve definition element: %s."), pItem->GetTag());
			return false;
			}
		}

	//	Success!

	return true;
	}

//	GetDefaultMiningMaxOreLevels
//
//	Returns the default table basedon API version
//
CMiningDamageLevelDesc CEngineOptions::GetDefaultMiningMaxOreLevels (int apiVersion)

	{
	CMiningDamageLevelDesc Desc;

	switch (GetAPIForMiningMaxOreLevel(apiVersion))
		{
		case 0:
			Desc.InitFromArray(g_StdMiningMaxOreLevelsAPI0);
			break;
		case 48:
			Desc.InitFromArray(g_StdMiningMaxOreLevelsAPI48);
			break;
		case 53:
			Desc.InitFromArray(g_StdMiningMaxOreLevelsAPI53);
			break;
		case 57:
		default:
			Desc.InitFromArray(g_StdMiningMaxOreLevelsAPI57);
		}

	return Desc;
	}

//	GetDefaultMiningMaxOreLevels
//
//	Returns the default table based on API version
//
CDeviceDamageLevelDesc CEngineOptions::GetDefaultExternalDeviceDamageLevels (int apiVersion)

	{
	CDeviceDamageLevelDesc Desc;

	switch (GetAPIForExternalDeviceDamageMaxLevel(apiVersion))
		{
		case 0:
			Desc.InitFromArray(g_StdExternalDeviceDamageLevelsAPI0, g_StdExternalDeviceDamageModifierAPI0, g_iExternalChanceToHitAPI0);
			break;
		case 58:
		default:
			Desc.InitFromArray(g_StdExternalDeviceDamageLevelsAPI58, g_StdExternalDeviceDamageModifierAPI58, g_iExternalChanceToHitAPI58);
		}

	return Desc;
	}

//	GetDefaultMiningMaxOreLevels
//
//	Returns the default table based on API version
//
CDeviceDamageLevelDesc CEngineOptions::GetDefaultInternalDeviceDamageLevels (int apiVersion)

	{
	CDeviceDamageLevelDesc Desc;

	switch (GetAPIForExternalDeviceDamageMaxLevel(apiVersion))
		{
		case 0:
			Desc.InitFromArray(g_StdInternalDeviceDamageLevelsAPI0, g_StdInternalDeviceDamageModifierAPI0, g_iInternalChanceToHitAPI0);
			break;
		case 58:
		default:
			Desc.InitFromArray(g_StdInternalDeviceDamageLevelsAPI58, g_StdInternalDeviceDamageModifierAPI58, g_iInternalChanceToHitAPI58);
		}

	return Desc;
	}

//	GetDefaultWMDAdj
//
//	Returns the default table based on API version
//
CDamageMethodDesc CEngineOptions::GetDefaultWMDAdj (int apiVersion)

	{
	CDamageMethodDesc Desc;

	switch (GetAPIForWMDAdj(apiVersion))
		{
		case 0:
			Desc.InitFromArray(g_StdWMDAdjAPI0, g_StdWMDLabelsAPI0, g_iStdWMDMinDamageAPI0);
			break;
		case 29:
		default:
			Desc.InitFromArray(g_StdWMDAdjAPI29, g_StdWMDLabelsAPI29, g_iStdWMDMinDamageAPI29);
		}

	return Desc;
	}

Metric CEngineOptions::GetDamageMethodAdj(const UDamageMethodAdj &adj, EDamageMethod iMethod) const
	{
	switch (m_iDamageMethodSystem)
		{
		case EDamageMethodSystem::dmgMethodSysPhysicalized:
			{
			switch (iMethod)
				{
				case EDamageMethod::methodCrush:
					return adj.PhysicalizedAdj.rCrush;
				case EDamageMethod::methodPierce:
					return adj.PhysicalizedAdj.rPierce;
				case EDamageMethod::methodShred:
					return adj.PhysicalizedAdj.rShred;
				default:
					{
					ASSERT(false);
					return 1.0;
					}
				}
			}
		case EDamageMethodSystem::dmgMethodSysWMD:
			{
			if (iMethod == EDamageMethod::methodWMD)
				return adj.WMDAdj.rWMD;
			ASSERT(false);
			return 1.0;
			}
		default:
			{
			ASSERT(false);
			return 1.0;
			}
		}
	}

//	InitDefaultDamageAdj
//
//	Initialize default tables
//
void CEngineOptions::InitDefaultDamageAdj ()

	{
	int i;

	if (!g_bDamageAdjInit)
		{
		for (i = 1; i <= MAX_ITEM_LEVEL; i++)
			{
			g_ArmorDamageAdj[i - 1].InitFromArray(g_StdArmorDamageAdj[i - 1]);
			g_ShieldDamageAdj[i - 1].InitFromArray(g_StdShieldDamageAdj[i - 1]);
			}

		g_bDamageAdjInit = true;
		}
	}

//	InitFromProperties
//
//	Initializes from properties (usually from the adventure descriptor).
//
bool CEngineOptions::InitFromProperties (SDesignLoadCtx &Ctx, const CDesignType &Type)

	{
	CCodeChainCtx CCX(Ctx.GetUniverse());
	m_iDefaultForAPIVersion = CCX.GetAPIVersion();

	//	Reinitialize global defaults as necessary for the correct API version
	InitDefaultDescs();

	ICCItemPtr pValue;

	pValue = Type.GetProperty(CCX, PROPERTY_CORE_DEFAULT_INTERACTION);
	m_iDefaultInteraction = (pValue->IsNil() ? -1 : Min(Max(0, pValue->GetIntegerValue()), 100));

	pValue = Type.GetProperty(CCX, PROPERTY_CORE_DEFAULT_SHOT_HP);
	m_iDefaultShotHP = (pValue->IsNil() ? -1 : Max(0, pValue->GetIntegerValue()));

	m_bHideDisintegrationImmune = !Type.GetProperty(CCX, PROPERTY_CORE_HIDE_DISINTEGRATION_IMMUNE)->IsNil();
	m_bHideIonizeImmune = !Type.GetProperty(CCX, PROPERTY_CORE_HIDE_IONIZE_IMMUNE)->IsNil();
	m_bHideRadiationImmune = !Type.GetProperty(CCX, PROPERTY_CORE_HIDE_RADIATION_IMMUNE)->IsNil();
	m_bHideShatterImmune = !Type.GetProperty(CCX, PROPERTY_CORE_HIDE_SHATTER_IMMUNE)->IsNil();

	if (Ctx.GetAPIVersion() >= 58)
		{

		//	Initialize the Damage Method System for this adventure
		//	This must be done before any initialization or access of the DamageMethod system

		pValue = Type.GetProperty(CCX, PROPERTY_CORE_DMG_METHOD_SYSTEM);
		if (pValue->IsNil())
			m_iDamageMethodSystem = EDamageMethodSystem::dmgMethodSysWMD;
		else
			{
			CString sDmgSys = pValue->GetStringValue();
			if (strEquals(sDmgSys, VALUE_CORE_DMG_METHOD_SYSTEM_DAMAGE_METHODS))
				m_iDamageMethodSystem = EDamageMethodSystem::dmgMethodSysPhysicalized;
			else if (strEquals(sDmgSys, VALUE_CORE_DMG_METHOD_SYSTEM_WMD))
				m_iDamageMethodSystem = EDamageMethodSystem::dmgMethodSysWMD;
			else
				{
				Ctx.sError = strPatternSubst(CONSTLIT("%s is not a valid value for adventure property %s"), sDmgSys, PROPERTY_CORE_DMG_METHOD_SYSTEM);
				return false;
				}
			}

		//	We need to initialize the engine defaults for this damage method before loading up any custom
		//	adventure-level defaults, since InitDamageMethodAdjFromCC requires that the engine default
		//	is already loaded up

		InitDefaultDamageMethods();

		//	Initialize default Damage Method Fortification Adj for this adventure
		// 
		//	Note: empty properties and properties with (double 'NaN) return Nil
		//	Nil initializes the legacy defaults which were hardcoded up through API57 (2.0a7)
		//
		//	We check for a real NaN just in case Tlisp doubles ever get updated to support
		//	explicit NaN. This code runs once per adventure init so the impact is negligible.
		//
		//	We do not support fortification above 1.0, since these cases will be handled by
		//	other mechanics

		pValue = Type.GetProperty(CCX, PROPERTY_CORE_DMG_METHOD_ITEM);

		//	Items default to 1.0 so their defaults are already correctly initialized

		ICCItem* pItmArmorStruct = pValue->GetElement(KEY_CORE_DMG_METHOD_ITEM_ARMOR);
		if (pItmArmorStruct)
			{
			if (!InitDamageMethodAdjFromCC(Ctx, m_DamageMethodItemAdj.Armor, pItmArmorStruct))
				return false;
			}

		ICCItem* pItmShieldStruct = pValue->GetElement(KEY_CORE_DMG_METHOD_ITEM_SHIELD);
		if (pItmShieldStruct)
			{
			if (!InitDamageMethodAdjFromCC(Ctx, m_DamageMethodItemAdj.Shield, pItmShieldStruct))
				return false;
			}

		pValue = Type.GetProperty(CCX, PROPERTY_CORE_DMG_METHOD_SHIP);
		ICCItem* pShipArmorStruct = pValue->GetElement(KEY_CORE_DMG_METHOD_SHIP_ARMOR);
		//	We use a nil here so that we can just use the same defaults handler later
		if (!pShipArmorStruct)
			pShipArmorStruct = CCX.CreateNil();

		ICCItem* pShipArmorCriticalStruct = pValue->GetElement(KEY_CORE_DMG_METHOD_SHIP_ARMOR_CRITICAL);
		if (pShipArmorCriticalStruct)
			{
			if (!InitDamageMethodAdjFromCC(Ctx, m_DamageMethodShipAdj.Armor.Critical, pShipArmorCriticalStruct))
				return false;
			}

		ICCItem* pShipArmorCriticalUncrewedStruct = pValue->GetElement(KEY_CORE_DMG_METHOD_SHIP_ARMOR_CRITICAL_UNCREWED);
		if (pShipArmorCriticalUncrewedStruct)
			{
			if (!InitDamageMethodAdjFromCC(Ctx, m_DamageMethodShipAdj.Armor.CriticalUncrewed, pShipArmorCriticalUncrewedStruct))
				return false;
			}

		ICCItem* pShipArmorNonCriticalStruct = pValue->GetElement(KEY_CORE_DMG_METHOD_SHIP_ARMOR_NONCRITICAL);
		if (pShipArmorNonCriticalStruct)
			{
			if (!InitDamageMethodAdjFromCC(Ctx, m_DamageMethodShipAdj.Armor.NonCritical, pShipArmorNonCriticalStruct))
				return false;
			}

		ICCItem* pShipArmorNonCriticalDestructionStruct = pValue->GetElement(KEY_CORE_DMG_METHOD_SHIP_ARMOR_NONCRITICAL_DESTRUCTION);
		if (pShipArmorNonCriticalDestructionStruct)
			{
			if (!InitDamageMethodAdjFromCC(Ctx, m_DamageMethodShipAdj.Armor.NonCriticalDestruction, pShipArmorNonCriticalDestructionStruct))
				return false;
			}

		ICCItem* pShipCompartmentStruct = pValue->GetElement(KEY_CORE_DMG_METHOD_SHIP_COMPARTMENT);
		//	We use a nil here so that we can just use the same defaults handler later
		if (!pShipCompartmentStruct)
			pShipCompartmentStruct = CCX.CreateNil();

		ICCItem* pShipCompartmentGeneralStruct = pValue->GetElement(KEY_CORE_DMG_METHOD_SHIP_COMPARTMENT_GENERAL);
		if (pShipCompartmentGeneralStruct)
			{
			if (!InitDamageMethodAdjFromCC(Ctx, m_DamageMethodShipAdj.Compartment.General, pShipCompartmentGeneralStruct))
				return false;
			}

		ICCItem* pShipCompartmentCargoStruct = pValue->GetElement(KEY_CORE_DMG_METHOD_SHIP_COMPARTMENT_CARGO);
		if (pShipCompartmentCargoStruct)
			{
			if (!InitDamageMethodAdjFromCC(Ctx, m_DamageMethodShipAdj.Compartment.Cargo, pShipCompartmentCargoStruct))
				return false;
			}

		ICCItem* pShipCompartmentMainDriveStruct = pValue->GetElement(KEY_CORE_DMG_METHOD_SHIP_COMPARTMENT_MAIN_DRIVE);
		if (pShipCompartmentMainDriveStruct)
			{
			if (!InitDamageMethodAdjFromCC(Ctx, m_DamageMethodShipAdj.Compartment.MainDrive, pShipCompartmentMainDriveStruct))
				return false;
			}

		ICCItem* pShipCompartmentUncrewedStruct = pValue->GetElement(KEY_CORE_DMG_METHOD_SHIP_COMPARTMENT_UNCREWED);
		if (pShipCompartmentUncrewedStruct)
			{
			if (!InitDamageMethodAdjFromCC(Ctx, m_DamageMethodShipAdj.Compartment.Uncrewed, pShipCompartmentUncrewedStruct))
				return false;
			}

		pValue = Type.GetProperty(CCX, PROPERTY_CORE_DMG_METHOD_STATION);
		ICCItem* pStationHullStruct = pValue->GetElement(KEY_CORE_DMG_METHOD_STATION_HULL);
		//	We use a nil here so that we can just use the same defaults handler later
		if (!pStationHullStruct)
			pStationHullStruct = CCX.CreateNil();

		ICCItem* pStationHullSingleStruct = pValue->GetElement(KEY_CORE_DMG_METHOD_STATION_HULL_SINGLE);
		if (pStationHullSingleStruct)
			{
			if (!InitDamageMethodAdjFromCC(Ctx, m_DamageMethodStationAdj.Hull.Single, pStationHullSingleStruct))
				return false;
			}

		ICCItem* pStationHullMultiStruct = pValue->GetElement(KEY_CORE_DMG_METHOD_STATION_HULL_MULTI);
		if (pStationHullMultiStruct)
			{
			if (!InitDamageMethodAdjFromCC(Ctx, m_DamageMethodStationAdj.Hull.Multi, pStationHullMultiStruct))
				return false;
			}

		ICCItem* pStationHullAsteroidStruct = pValue->GetElement(KEY_CORE_DMG_METHOD_STATION_HULL_ASTEROID);
		if (pStationHullAsteroidStruct)
			{
			if (!InitDamageMethodAdjFromCC(Ctx, m_DamageMethodStationAdj.Hull.Asteroid, pStationHullAsteroidStruct))
				return false;
			}

		ICCItem* pStationHullUndergroundStruct = pValue->GetElement(KEY_CORE_DMG_METHOD_STATION_HULL_UNDERGROUND);
		if (pStationHullUndergroundStruct)
			{
			if (!InitDamageMethodAdjFromCC(Ctx, m_DamageMethodStationAdj.Hull.Underground, pStationHullUndergroundStruct))
				return false;
			}

		ICCItem* pStationHullUncrewedStruct = pValue->GetElement(KEY_CORE_DMG_METHOD_STATION_HULL_UNCREWED);
		if (pStationHullUncrewedStruct)
			{
			if (!InitDamageMethodAdjFromCC(Ctx, m_DamageMethodStationAdj.Hull.Uncrewed, pStationHullUncrewedStruct))
				return false;
			}

		ICCItem* pStationHullArmorStruct = pValue->GetElement(KEY_CORE_DMG_METHOD_STATION_HULL_ARMOR);
		if (pStationHullArmorStruct)
			{
			if (!InitDamageMethodAdjFromCC(Ctx, m_DamageMethodStationAdj.Hull.Armor, pStationHullArmorStruct))
				return false;
			}

		//	Set minimum fortification

		pValue = Type.GetProperty(CCX, PROPERTY_CORE_DMG_METHOD_MIN_ADJ);
		Metric rValue = pValue->IsNil() ? 0.0 : pValue->GetDoubleValue();
		if (rValue < 0)
			rValue = 0.0;
		if (rValue > 1.0 + g_Epsilon)
			{
			Ctx.sError = strCat(PROPERTY_CORE_DMG_METHOD_MIN_ADJ, CONSTLIT(" cannot be greater than 1.0"));
			return false;
			}
		m_rMinFortificationAdj = rValue;

		}

	//	Legacy adventures can only use defaults because the older APIs dont support anything else

	else
		{
		m_rMinFortificationAdj = 0.0;
		m_iDamageMethodSystem = EDamageMethodSystem::dmgMethodSysWMD;

		InitDefaultDamageMethods();
		}

	return true;
	}

