//	SUpdateCtx.cpp
//
//	SUpdateCtx structure
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

int SUpdateCtx::GetLightIntensity (CSpaceObject *pObj) const
	{
	if (pObj == pCacheObj && iLightIntensity != -1)
		return iLightIntensity;

	pCacheObj = pObj;
	iLightIntensity = pSystem->CalculateLightIntensity(pObj->GetPos());
	return iLightIntensity;
	}

