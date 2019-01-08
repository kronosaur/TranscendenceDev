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

