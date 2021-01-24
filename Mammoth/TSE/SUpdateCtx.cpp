//	SUpdateCtx.cpp
//
//	SUpdateCtx structure
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

int SUpdateCtx::GetLightIntensity (CSpaceObject *pObj) const
	{
	if (pObj == m_pCacheObj && m_iLightIntensity != -1)
		return m_iLightIntensity;

	m_pCacheObj = pObj;
	m_iLightIntensity = pSystem->CalculateLightIntensity(pObj->GetPos());
	return m_iLightIntensity;
	}

CTargetList &SUpdateCtx::GetTargetList ()

//	GetTargetList
//
//	Initializes and returns the target list.

	{
	if (!m_bTargetListValid && m_pObj)
		{
		m_TargetList = m_pObj->GetTargetList();
		m_bTargetListValid = true;
		}

	return m_TargetList;
	}

void SUpdateCtx::OnStartUpdate (CSpaceObject &Obj)
	{
	m_pObj = &Obj;
	m_bTimeStopped = Obj.IsTimeStopped();
	m_bTargetListValid = false;
	}
