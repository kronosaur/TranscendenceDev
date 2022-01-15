//	AchivementsUtil.cpp
//
//	Utilities for achievements
//	Copyright (c) 2021 by Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

void CRequestCurrentStats::OnResult (UserStatsReceived_t *pCallback)
	{
	m_bComplete = true;
	}

void CStoreStats::OnStatsStored (UserStatsStored_t *pCallback)
	{
	m_bStatsStored = true;
	}

void CStoreStats::OnAchievementStored (UserAchievementStored_t *pCallback)
	{
	m_bAchievementStored = true;
	}
