//	CPerceptionCalc.cpp
//
//	CPerceptionCalc class
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

//	If we attacked in the last 45 ticks, then we can be detected enough to be
//	attacked.

const DWORD LAST_ATTACK_THRESHOLD =					45;

bool CPerceptionCalc::m_bRangeTableInitialized = false;
Metric CPerceptionCalc::m_rRange[RANGE_ARRAY_SIZE] = { 0.0 };
Metric CPerceptionCalc::m_rRange2[RANGE_ARRAY_SIZE] = { 0.0 };

CPerceptionCalc::CPerceptionCalc (int iPerception) :
		m_iPerception(iPerception)

//	CPerceptionCalc constructor

	{
	m_dwLastAttackThreshold = (DWORD)g_pUniverse->GetTicks() - LAST_ATTACK_THRESHOLD;

	if (!m_bRangeTableInitialized)
		InitRangeTable();
	}

int CPerceptionCalc::AdjPerception (int iValue, int iAdj)

//	AdjPerception
//
//	Adjusts perception, keeping result in range.

	{
	return Max((int)CSpaceObject::perceptMin, Min(iValue + iAdj, (int)CSpaceObject::perceptMax));
	}

int CPerceptionCalc::AdjStealth (int iValue, int iAdj)

//	AdjStealth
//
//	Adjusts stealth, keeping result in range.

	{
	return Max((int)CSpaceObject::stealthMin, Min(iValue + iAdj, (int)CSpaceObject::stealthMax));
	}

DWORD CPerceptionCalc::CalcSRSShimmer (const CSpaceObject &TargetObj, Metric rTargetDist) const

//	CalcSRSShimmer
//
//	Calculates the SRS opacity to paint with.
//
//	0 = Fully visible.
//	1-255 = varying degrees of visibility (1 = lowest, 255 = highest).

	{
	int iRangeIndex = GetRangeIndex(TargetObj.GetStealth());
	if (iRangeIndex < 6)
		return 0;

	Metric rRange = GetRange(iRangeIndex);
	if (rTargetDist <= rRange)
		return 0;

	return 255 - Min(254, (int)((rTargetDist - rRange) / g_KlicksPerPixel) * 2);
	}

bool CPerceptionCalc::CanBeTargeted (const CSpaceObject *pTarget, Metric rTargetDist2) const

//	CanBeTargeted
//
//	Returns TRUE if we can target the given object with our current perception,
//	taking into account whether or not the target recently fired.

	{
	if (IsVisibleDueToAttack(pTarget))
		return (rTargetDist2 < m_rRange2[GetRangeIndex(Min(pTarget->GetStealth(), (int)CSpaceObject::stealthNormal))]);
	else
		return (rTargetDist2 < GetMaxDist2(pTarget));
	}

bool CPerceptionCalc::CanVisuallyScan (const CSpaceObject &TargetObj, Metric rTargetDist2) const

//	CanVisuallyScan
//
//	Returns TRUE if the given target object is in visual scanning range, for
//	purposes of setting the scanned flag (etc.).

	{
	//	If outside of fixed maximum visual range, then no scan.

	if (rTargetDist2 > GetStdVisualRange2())
		return false;

	//	If the target is stealthy enough, then we cannot see it even if closer.

	else if (rTargetDist2 > GetMaxDist2(&TargetObj))
		return false;

	//	Otherwise, it is visible

	else
		return true;
	}

Metric CPerceptionCalc::GetMaxDist (int iPerception)

//	GetMaxDist
//
//	Returns the maximum range at with the given perception can detect an object
//	with normal stealth.

	{
	return GetRange(GetRangeIndex(CSpaceObject::stealthNormal, iPerception));
	}

Metric CPerceptionCalc::GetMaxDist (const CSpaceObject *pTarget) const

//	GetMaxDist
//
//	Returns the maximum distance at which we can see the given object.

	{
	return m_rRange[GetRangeIndex(pTarget->GetStealth())];
	}

Metric CPerceptionCalc::GetMaxDist2 (const CSpaceObject *pTarget) const

//	GetMaxDist2
//
//	Returns the maximum distance (squared) at which we can see the given object.

	{
	return m_rRange2[GetRangeIndex(pTarget->GetStealth())];
	}

int CPerceptionCalc::GetRangeIndex (int iStealth, int iPerception)

//	GetRangeIndex
//
//	Returns the range index at which we can detect an object with the given 
//	stealth.

	{
	int iResult = (iStealth - iPerception) + RANGE_ARRAY_BASE_RANGE_INDEX;

	//	We are easily visible at any range

	if (iResult <= 0)
		return 0;

	//	Otherwise, we could be invisible

	return Min(iResult, RANGE_ARRAY_SIZE - 1);
	}

void CPerceptionCalc::InitRangeTable (void)

//	InitRangeTable
//
//	Initialize range table

	{
	int i;
	for (i = 0; i < RANGE_ARRAY_SIZE; i++)
		{
		m_rRange[i] = CalcPerceptionRange(RANGE_ARRAY_SIZE - RANGE_ARRAY_BASE_RANGE_INDEX, RANGE_ARRAY_SIZE - i) * LIGHT_SECOND;
		m_rRange2[i] = m_rRange[i] * m_rRange[i];
		}

	m_bRangeTableInitialized = true;
	}

bool CPerceptionCalc::IsVisibleDueToAttack (const CSpaceObject *pTarget) const

//	IsVisibleDueToAttack
//
//	Returns TRUE if we have recently fired.
	
	{
	return ((DWORD)pTarget->GetLastFireTime() >= m_dwLastAttackThreshold);
	}

bool CPerceptionCalc::IsVisibleInLRS (const CSpaceObject *pSource, const CSpaceObject *pTarget) const

//	IsVisibleInLRS
//
//	Returns TRUE if pTarget is visible in LRS to pSource.

	{
	Metric rDist2 = pSource->GetDistance2(pTarget);
	return (rDist2 <= GetMaxDist2(pTarget) && rDist2 <= (g_LRSRange * g_LRSRange));
	}
