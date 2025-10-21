//	CEngineOptions.cpp
//
//	CEngineOptions class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define LEVEL_ATTRIB							CONSTLIT("level")

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

//	Mining

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

int GetAPIForMiningMaxOreLevel (int apiVersion)

	//	Translates actual API versions into the known versions
	//	used for the MiningMaxOreLevel system

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

CEngineOptions::CEngineOptions (int apiVersion)

//	CEngineOptions constructor

	{
	//	Set api version we are loading defaults for

	m_iDefaultForAPIVersion = apiVersion;

	//	Initialize globals

	InitDefaultGlobals();
	}

void CEngineOptions::InitDefaultGlobals (void)

//	Initialize default globals based on whatever API version we need

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
	}

void CEngineOptions::InitDefaultDescs (void)

//	Initiate defaults where necessary based on whatever API version we need
//	Assumes that InitDefaultGlobals was called during our constructor

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
	}

bool CEngineOptions::HidesArmorImmunity (SpecialDamageTypes iSpecial) const

//	HidesArmorImmunity
//
//	Returns TRUE if we should hide the given immunity from armor UI displays.
//	We do this on high-level adventures (e.g., VotG) when all armors are immune
//	to a particular special damage.

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

bool CEngineOptions::InitDamageAdjFromXML (SDesignLoadCtx &Ctx, const CXMLElement &XMLDesc, CDamageAdjDesc *DestTable)

//	InitDamageAdjFromXML
//
//	Initializes from XML.

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

bool CEngineOptions::InitMiningMaxOreLevelsFromXML (SDesignLoadCtx& Ctx, const CXMLElement& XMLDesc)

//	InitDamageAdjFromXML
//
//	Initializes from XML.

	{
	m_bCustomMiningMaxOreLevels = true;

	if (m_MiningDamageMaxOreLevels.InitFromXML(Ctx, XMLDesc) != NOERROR)
		return false;

	//	Success!

	return true;
	}

CMiningDamageLevelDesc CEngineOptions::GetDefaultMiningMaxOreLevels (int apiVersion)

//	GetDefaultMiningMaxOreLevels
//
//	Returns the default table basedon API version

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

void CEngineOptions::InitDefaultDamageAdj (void)

//	InitDefaultDamageAdj
//
//	Initialize default tables

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

bool CEngineOptions::InitFromProperties (SDesignLoadCtx &Ctx, const CDesignType &Type)

//	InitFromProperties
//
//	Initializes from properties (usually from the adventure descriptor).

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

	return true;
	}

