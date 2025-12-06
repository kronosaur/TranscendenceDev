//	CMassDestructionDesc.cpp
//
//	CMassDestructionDesc class
//	Copyright (c) 2012 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define WMD_ADJ_ATTRIB						CONSTLIT("wmdAdj")
#define WMD_DISPLAY_ATTRIB					CONSTLIT("wmdDisplay")
#define WMD_DISPLAY_PREFIX_ATTRIB			CONSTLIT("wmdDisplayPrefix")
#define WMD_MIN_DAMAGE_ATTRIB				CONSTLIT("wmdMinDamage")

//	GetWMDAdj
// 
//	Get raw WMD adj as a floating point number
//
Metric CMassDestructionDesc::GetWMDAdj (int iLevel) const
	{
	iLevel = max(min(iLevel, MAX_WMD_LEVEL), 0);
	return m_Desc[iLevel].rAdj;
	}

//	GetStochasticWMDAdj
//
//	Get Stochastically Rounded WMD as integer Adj
//	Use GetWMDAdj for accurate Adj math.
//	Use GetStochasticWMDAdj for accurate Adj balance on legacy algorithms.
//
int CMassDestructionDesc::GetRoundedWMDAdj (int iLevel) const
	{
	return mathRoundStochastic(GetWMDAdj(iLevel) * 100);
	}

//	GetRoundedWMDAdj
//
//	Get Rounded WMD as integer Adj
//	Use GetWMDAdj for accurate Adj math.
//	Use GetStochasticWMDAdj for accurate Adj balance on legacy algorithms.
//	Recommended only if you need to use WMD Adj in legacy algorithms that expect consistent output.
//
int CMassDestructionDesc::GetRoundedWMDAdj (int iLevel) const
	{
	return mathRound(GetWMDAdj(iLevel) * 100);
	}

//	GetWMDLabel
//	
//	Get Integer Suffix to display for this WMD label
//
CString CMassDestructionDesc::GetWMDLabel (int iLevel) const
	{
	iLevel = max(min(iLevel, MAX_WMD_LEVEL), 0);
	return m_Desc[iLevel].sLabel;
	}

//	InitFromArray
//
//	Initializes from an array. The array must be at least damageCount entries
//	long.
//
//	In this path there is no need for Bind.
//
ALERROR CMassDestructionDesc::InitFromArray (const TArray<double>& Adj, const TArray<const char*>& Labels, int iMinDamage, CString sAttribPrefix)

	{
	//	Ensure our arrays are valid

	if (Adj.GetCount() != MAX_WMD_LEVEL_COUNT || Labels.GetCount() != MAX_WMD_LEVEL_COUNT)
		return ERR_FAIL;

	//	Ensure that for all mathematical relevancy, WMD 7 == 1.0 Adj

	if (!(Adj[MAX_WMD_LEVEL] + g_Epsilon > 1.0 && Adj[MAX_WMD_LEVEL] - g_Epsilon < 1.0))
		return ERR_FAIL;

	m_iMinDamage = iMinDamage;
	m_sAttribPrefix = sAttribPrefix;

	for (int i = 0; i < MAX_WMD_LEVEL_COUNT; i++)
		{
		m_Desc[i].rAdj = Adj[i];
		m_Desc[i].sLabel = CString(Labels[i]);
		}

	return NOERROR;
	}

//	InitFromMiningDamageLevel
//
//	Loads a WMD descriptor as follows:
//
//	WMD LEVEL:	0		1		2		3		4		5		6		7
//	sAdj:		0.1		0.2		0.3		0.4		0.55	0.7		0.85	1.0
//	or
//	sAdj:		10		20		30		40		55		70		85		100
//	and
//	sLabel:		10		20		30		40		55		70		85		100
//
ALERROR CMassDestructionDesc::InitFromWMDLevel (SDesignLoadCtx &Ctx, const CString &sAdj, const CString &sLabel, int iMinDamage, CString sAttribPrefix)

	{
	ALERROR error;

	//	Short-circuit
	//	We just throw an error here because we are not designed to get the tables here.

	if (sAdj.IsBlank())
		{
		Ctx.sError = CONSTLIT("Invalid WMDLevels definition: wmdAdj cannot be blank.");
		return error;
		}

	//	We expect a list of per damage max ore level values, either with a damageType
	//	label or ordered by damageType.

	TArray<CString> DamageAdj;
	if (error = ParseWMDList(sAttrib, &DamageAdj))
		{
		Ctx.sError = CONSTLIT("Invalid wmdAdj definition.");
		return error;
		}

	//	Get level

	for (int i = 0; i < damageCount; i++)
		{

		//	If we have nothing

		if (DamageAdj[i].IsBlank())
			{
			m_Desc[i].iAdjType = levelRelative;
			m_Desc[i].iLevelValue = 0;
			}
		else
			{
			int iValue = strToInt(DamageAdj[i], 0);

			//	If we have a positive item level offset

			if (strStartsWith(DamageAdj[i], CONSTLIT("+")))
				{

				//	Negative values should not be reachable in this code

				if (iValue > MAX_ITEM_LEVEL)
					{
					Ctx.sError = strPatternSubst(CONSTLIT("miningMaxOreLevel value is out of range: %d."), iValue);
					return ERR_FAIL;
					}

				m_Desc[i].iAdjType = levelRelative;
				m_Desc[i].iLevelValue = iValue;
				}

			//	If we have a negative item level offset

			else if (strStartsWith(DamageAdj[i], CONSTLIT("-")))
				{

				//	Positive values should not be reachable in this code

				if (iValue < -1 * MAX_ITEM_LEVEL)
					{
					Ctx.sError = strPatternSubst(CONSTLIT("miningMaxOreLevel value is out of range: %d."), iValue);
					return ERR_FAIL;
					}

				m_Desc[i].iAdjType = levelRelative;
				m_Desc[i].iLevelValue = iValue;
				}

			//	If we have an absolute item level

			else
				{

				//	Negative values should not be reachable in this code

				if (iValue > MAX_ITEM_LEVEL)
					{
					Ctx.sError = strPatternSubst(CONSTLIT("miningMaxOreLevel value is out of range: %d."), iValue);
					return ERR_FAIL;
					}

				m_Desc[i].iAdjType = levelAbsolute;
				m_Desc[i].iLevelValue = iValue;
				}
			}
		}

	//	Done

	return NOERROR;
	}

//	InitFromXML
//
//	Initialize from XML.
//
//	If bIsDefault is TRUE then we don't need to bind.
//
ALERROR CMassDestructionDesc::InitFromXML (SDesignLoadCtx &Ctx, const CXMLElement &XMLDesc)

	{
	ALERROR error;
	CString sAdj;
	CString sLabels;
	CString sPrefix;

	if (XMLDesc.FindAttribute(MINING_DAMAGE_LEVEL_ATTRIB, &sAdj))
		{
		if (error = InitFromWMDLevel(Ctx, sValue))
			return error;
		}
	else
		{
		Ctx.sError = CONSTLIT("Invalid WMDLevels definition: wmdAdj missing.");
		return ERR_FAIL;
		}

	//	Done

	return NOERROR;
	}
