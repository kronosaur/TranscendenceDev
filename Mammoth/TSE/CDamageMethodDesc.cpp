//	CDamageMethodDesc.cpp
//
//	CDamageMethodDesc class
//	Copyright (c) 2012 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define WMD_ADJ_ATTRIB						CONSTLIT("wmdAdj")
#define WMD_DISPLAY_ATTRIB					CONSTLIT("wmdDisplay")
#define WMD_DISPLAY_PREFIX_ATTRIB			CONSTLIT("wmdDisplayPrefix")

//	GetWMDAdj
// 
//	Get raw WMD adj as a floating point number
//
Metric CDamageMethodDesc::GetWMDAdj (int iLevel) const
	{
	iLevel = max(min(iLevel, MAX_DAMAGE_METHOD_LEVEL), 0);
	return m_Desc[iLevel].rAdj;
	}

//	GetStochasticWMDAdj
//
//	Get Stochastically Rounded WMD as integer Adj
//	Use GetWMDAdj for accurate Adj math.
//	Use GetStochasticWMDAdj for accurate Adj balance on legacy algorithms.
//
int CDamageMethodDesc::GetStochasticWMDAdj (int iLevel) const
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
int CDamageMethodDesc::GetRoundedWMDAdj (int iLevel) const
	{
	return mathRound(GetWMDAdj(iLevel) * 100);
	}

//	GetWMDLabel
//	
//	Get Integer Suffix to display for this WMD label
//
CString CDamageMethodDesc::GetWMDLabel (int iLevel) const
	{
	iLevel = max(min(iLevel, MAX_DAMAGE_METHOD_LEVEL), 0);
	return m_Desc[iLevel].sLabel;
	}

//	InitFromArray
//
//	Initializes from an array. The array must be at least damageCount entries
//	long.
//
//	In this path there is no need for Bind.
//
ALERROR CDamageMethodDesc::InitFromArray (const TArray<double>& Adj, const TArray<const char*>& Labels, CString sAttribPrefix)

	{
	//	Ensure our arrays are valid

	if (Adj.GetCount() != MAX_DAMAGE_METHOD_LEVEL_COUNT || Labels.GetCount() != MAX_DAMAGE_METHOD_LEVEL_COUNT)
		return ERR_FAIL;

	//	Ensure that for all mathematical relevancy, WMD 7 == 1.0 Adj

	if (!(Adj[MAX_DAMAGE_METHOD_LEVEL] + g_Epsilon > 1.0 && Adj[MAX_DAMAGE_METHOD_LEVEL] - g_Epsilon < 1.0))
		return ERR_FAIL;

	m_sAttribPrefix = sAttribPrefix;

	for (int i = 0; i < MAX_DAMAGE_METHOD_LEVEL_COUNT; i++)
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
ALERROR CDamageMethodDesc::InitFromWMDLevel (SDesignLoadCtx &Ctx, const CString &sAdj, const CString &sLabels, CString sAttribPrefix)

	{
	//	Short-circuit
	//	We just throw an error here because we are not designed to get the tables here.

	if (sAdj.IsBlank())
		{
		Ctx.sError = CONSTLIT("Invalid WMDLevels definition: wmdAdj cannot be blank.");
		return ERR_FAIL;
		}

	//	We expect a list of per WMD level adjustments and labels

	ALERROR error;
	TArray<CString> aAdj;

	if (error = ParseWMDAdjList(sAdj, aAdj))
		{
		Ctx.sError = CONSTLIT("Invalid wmdAdj definition: must have 8 elements.");
		return error;
		}

	TArray<CString> aLabels;

	if (error = ParseWMDLabelList(sLabels, aLabels))
		{
		Ctx.sError = CONSTLIT("Invalid wmdDisplay definition: must have 8 elements.");
		return error;
		}

	//	Init the WMD levels

	for (int i = 0; i < MAX_DAMAGE_METHOD_LEVEL_COUNT; i++)
		{
		//	Insert label
		//	
		//	We only do this for WMD levels above 0, since WMD0 is an absense of WMD and should
		//	not have a label
		// 
		//	We allow "!" as an override to prevent a given level from printing a label.

		if (i && !strEquals(aLabels[i - 1], CONSTLIT("!")))
			{
			CString sLabel = aLabels[i - 1];
			m_Desc[i].sLabel = sLabel;
			}
		else
			m_Desc[i].sLabel = CONSTLIT("");

		//	Convert the adj value

		bool bFail;

		int iAdj = strParseInt(aAdj[i].GetPointer(), 100, NULL, &bFail);

		if (bFail || strFind(aAdj[i], CONSTLIT(".")) >= 0)
			{
			m_Desc[i].rAdj = strParseDouble(aAdj[i].GetPointer(), 1.0, NULL, &bFail);

			if (bFail)
				{
				Ctx.sError = strCat(CONSTLIT("Could not parse value in wmdAdj: "), aAdj[i]);
				return ERR_FAIL;
				}
			}
		else
			m_Desc[i].rAdj = ((double)iAdj) / 100.0;

		//	the max WMD element must be 1.0
		//	since everything is adjusted relative to WMD7

		if (i == MAX_DAMAGE_METHOD_LEVEL && (m_Desc[i].rAdj > 1.0 + g_Epsilon || m_Desc[i].rAdj < 1.0 - g_Epsilon))
			{
			Ctx.sError = CONSTLIT("The adj value of WMD7 must be equal to 1.0 (100 %)");
			return ERR_FAIL;
			}
		}

	//	Insert values

	m_sAttribPrefix = sAttribPrefix;

	//	Done

	return NOERROR;
	}

//	InitFromXML
//
//	Initialize from XML.
//
//	If bIsDefault is TRUE then we don't need to bind.
//
ALERROR CDamageMethodDesc::InitFromXML (SDesignLoadCtx &Ctx, const CXMLElement &XMLDesc)

	{
	CString sAdj;
	CString sLabels;
	CString sPrefix;

	//	Collect all attributes

	if (!XMLDesc.FindAttribute(WMD_ADJ_ATTRIB, &sAdj))
		{
		Ctx.sError = CONSTLIT("Invalid WMDLevels definition: wmdAdj missing.");
		return ERR_FAIL;
		}

	if (!XMLDesc.FindAttribute(WMD_DISPLAY_ATTRIB, &sLabels))
		{
		Ctx.sError = CONSTLIT("Invalid WMDLevels definition: wmdAdj missing.");
		return ERR_FAIL;
		}

	if (!XMLDesc.FindAttribute(WMD_DISPLAY_PREFIX_ATTRIB, &sPrefix))
		sPrefix = m_sAttribPrefix;

	return InitFromWMDLevel(Ctx, sAdj, sLabels, sPrefix);
	}

//	ParseWMDAdjList
//
//	Parses an eight element string list like the following
//	WMD LEVEL:	"0		1		2		3		4		5		6		7"
//	Whitespace , and ; are valid delimiters
// 
//	Returns ERR_FAIL if the count of elements is not 8.
//
ALERROR CDamageMethodDesc::ParseWMDAdjList(CString sAttrib, TArray<CString> &DamageAdj) const
	{
	ParseAttributes(sAttrib, &DamageAdj);

	if (DamageAdj.GetCount() != MAX_DAMAGE_METHOD_LEVEL_COUNT)
		return ERR_FAIL;

	return NOERROR;
	}

//	ParseWMDLabelList
//
//	Parses an seven element string list like the following (WMD0 never displays a label, as its an absense of WMD)
//	WMD LEVEL:	"1		2		3		4		5		6		7"
//	Whitespace , and ; are valid delimiters
// 
//	Returns ERR_FAIL if the count of elements is not 8.
//
ALERROR CDamageMethodDesc::ParseWMDLabelList(CString sAttrib, TArray<CString> &DamageAdj) const
	{
	ParseAttributes(sAttrib, &DamageAdj);

	if (DamageAdj.GetCount() != MAX_DAMAGE_METHOD_LEVEL_COUNT-1)
		return ERR_FAIL;

	return NOERROR;
	}
