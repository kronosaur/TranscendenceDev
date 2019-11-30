//	CEngineOptions.cpp
//
//	CEngineOptions class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define LEVEL_ATTRIB							CONSTLIT("level")

#define PROPERTY_CORE_DEFAULT_INTERACTION		CONSTLIT("core.defaultInteraction")
#define PROPERTY_CORE_DEFAULT_SHOT_HP			CONSTLIT("core.defaultShotHP")

static int g_StdArmorDamageAdj[MAX_ITEM_LEVEL][damageCount] =
	{
		//	lsr knt par blt ion thr pos pls am  nan grv sng dac dst dlg dfr
		{	100,100,100,100,100,100,100,100,125,125,200,200,300,300,500,500 },
		{	 80, 80,100,100,100,100,100,100,125,125,200,200,300,300,500,500 },
		{	 60, 60,100,100,100,100,100,100,125,125,200,200,300,300,500,500 },
		{	 40, 40,100,100,100,100,100,100,100,100,125,125,200,200,300,300 },
		{	 25, 25, 80, 80,100,100,100,100,100,100,125,125,200,200,300,300 },

		{	 14, 14, 60, 60,100,100,100,100,100,100,125,125,200,200,300,300 },
		{	  8,  8, 40, 40,100,100,100,100,100,100,100,100,125,125,200,200 },
		{	  4,  4, 25, 25, 80, 80,100,100,100,100,100,100,125,125,200,200 },
		{	  2,  2, 14, 14, 60, 60,100,100,100,100,100,100,125,125,200,200 },
		{	  1,  1,  8,  8, 40, 40,100,100,100,100,100,100,100,100,125,125 },

		{	  0,  0,  4,  4, 25, 25, 80, 80,100,100,100,100,100,100,125,125 },
		{	  0,  0,  2,  2, 14, 14, 60, 60,100,100,100,100,100,100,125,125 },
		{	  0,  0,  1,  1,  8,  8, 40, 40,100,100,100,100,100,100,100,100 },
		{	  0,  0,  0,  0,  4,  4, 25, 25, 80, 80,100,100,100,100,100,100 },
		{	  0,  0,  0,  0,  2,  2, 14, 14, 60, 60,100,100,100,100,100,100 },

		{	  0,  0,  0,  0,  1,  1,  8,  8, 40, 40,100,100,100,100,100,100 },
		{	  0,  0,  0,  0,  0,  0,  4,  4, 25, 25, 80, 80,100,100,100,100 },
		{	  0,  0,  0,  0,  0,  0,  2,  2, 14, 14, 60, 60,100,100,100,100 },
		{	  0,  0,  0,  0,  0,  0,  1,  1,  8,  8, 40, 40,100,100,100,100 },
		{	  0,  0,  0,  0,  0,  0,  0,  0,  4,  4, 25, 25, 80, 80,100,100 },

		{	  0,  0,  0,  0,  0,  0,  0,  0,  2,  2, 14, 14, 60, 60,100,100 },
		{	  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  8,  8, 40, 40,100,100 },
		{	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,  4, 25, 25, 80, 80 },
		{	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2,  2, 14, 14, 60, 60 },
		{	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  8,  8, 40, 40 },
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

CEngineOptions::CEngineOptions (void)

//	CEngineOptions constructor

	{
	//	Initialize armor and shield damage adjustment tables

	InitDefaultDamageAdj();
	for (int i = 1; i <= MAX_ITEM_LEVEL; i++)
		{
		m_ArmorDamageAdj[i - 1] = g_ArmorDamageAdj[i - 1];
		m_ShieldDamageAdj[i - 1] = g_ShieldDamageAdj[i - 1];
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

	if (m_ArmorDamageAdj[iLevel - 1].InitFromXML(Ctx, XMLDesc, true) != NOERROR)
		return false;

	//	Success!

	return true;
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
	ICCItemPtr pValue;

	pValue = Type.GetProperty(CCX, PROPERTY_CORE_DEFAULT_INTERACTION);
	m_iDefaultInteraction = (pValue->IsNil() ? -1 : Min(Max(0, pValue->GetIntegerValue()), 100));

	pValue = Type.GetProperty(CCX, PROPERTY_CORE_DEFAULT_SHOT_HP);
	m_iDefaultShotHP = (pValue->IsNil() ? -1 : Max(0, pValue->GetIntegerValue()));

	return true;
	}

void CEngineOptions::Merge (const CEngineOptions &Src)

//	Merge
//
//	Merges with source.

	{
	if (Src.m_bCustomArmorDamageAdj)
		{
		for (int i = 0; i < MAX_ITEM_LEVEL; i++)
			m_ArmorDamageAdj[i] = Src.m_ArmorDamageAdj[i];
		}

	if (Src.m_bCustomShieldDamageAdj)
		{
		for (int i = 0; i < MAX_ITEM_LEVEL; i++)
			m_ShieldDamageAdj[i] = Src.m_ShieldDamageAdj[i];
		}

	if (Src.m_iDefaultInteraction != -1)
		m_iDefaultInteraction = Src.m_iDefaultInteraction;

	if (Src.m_iDefaultShotHP != -1)
		m_iDefaultShotHP = Src.m_iDefaultShotHP;
	}
