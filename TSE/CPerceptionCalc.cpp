//	CPerceptionCalc.cpp
//
//	CPerceptionCalc class
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

//	If we attacked in the last 45 ticks, then we can be detected enough to be
//	attacked.

const DWORD LAST_ATTACK_THRESHOLD =					45;

bool CPerceptionCalc::m_bRangeTableInitialized = false;

Metric CPerceptionCalc::m_rRange[RANGE_ARRAY_SIZE] = 
	{
	(500.0 * LIGHT_SECOND),
	(340.0 * LIGHT_SECOND),
	(225.0 * LIGHT_SECOND),
	(150.0 * LIGHT_SECOND),
	(100.0 * LIGHT_SECOND),
	(50.0 * LIGHT_SECOND),
	(25.0 * LIGHT_SECOND),
	(13.0 * LIGHT_SECOND),
	(6.0 * LIGHT_SECOND),
	};

Metric CPerceptionCalc::m_rRange2[RANGE_ARRAY_SIZE];

CPerceptionCalc::CPerceptionCalc (int iPerception = -1) :
		m_iPerception(iPerception)

//	CPerceptionCalc constructor

	{
	m_dwLastAttackThreshold = (DWORD)g_pUniverse->GetTicks() - LAST_ATTACK_THRESHOLD;

	if (!m_bRangeTableInitialized)
		InitRangeTable();
	}

bool CPerceptionCalc::CanBeTargeted (CSpaceObject *pTarget, Metric rTargetDist2) const

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

bool CPerceptionCalc::CanBeTargetedAtDist (CSpaceObject *pTarget, Metric rTargetDist) const

//	CanBeTargetedAtDist
//
//	Same as CanBeTargeted, but using a true distance instead of distance 
//	squared.

	{
	if (IsVisibleDueToAttack(pTarget))
		return (rTargetDist < m_rRange[GetRangeIndex(Min(pTarget->GetStealth(), (int)CSpaceObject::stealthNormal))]);
	else
		return (rTargetDist < GetMaxDist(pTarget));
	}

Metric CPerceptionCalc::GetMaxDist (CSpaceObject *pTarget) const

//	GetMaxDist2
//
//	Returns the maximum distance at which we can see the given object.

	{
	return m_rRange[pTarget->GetDetectionRangeIndex(m_iPerception)];
	}

Metric CPerceptionCalc::GetMaxDist2 (CSpaceObject *pTarget) const

//	GetMaxDist2
//
//	Returns the maximum distance (squared) at which we can see the given object.

	{
	return m_rRange2[pTarget->GetDetectionRangeIndex(m_iPerception)];
	}

int CPerceptionCalc::GetRangeIndex (int iStealth) const

//	GetRangeIndex
//
//	Returns the range index at which we can detect an object with the given 
//	stealth.

	{
	int iResult = (iStealth - m_iPerception) + 4;

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
		m_rRange2[i] = m_rRange[i] * m_rRange[i];

	m_bRangeTableInitialized = true;
	}

bool CPerceptionCalc::IsVisibleDueToAttack (CSpaceObject *pTarget) const

//	IsVisibleDueToAttack
//
//	Returns TRUE if we have recently fired.
	
	{
	return ((DWORD)pTarget->GetLastFireTime() >= m_dwLastAttackThreshold);
	}
