//	CSphericalTextureMapper.cpp
//
//	CSphericalTextureMapper class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "SFXFractalImpl.h"

void CSphericalTextureMapper::Init (CFractalTextureLibrary::ETextureTypes iTexture, int iFrame, int iRadius, int iAngleRange)

//	Init
//
//	Initializes the arrays

	{
	int i;

	m_pTexture = &g_pUniverse->GetFractalTextureLibrary().GetTexture(iTexture, iFrame);

	if (m_AngleToX.GetCount() != iAngleRange)
		{
		m_AngleToX.DeleteAll();
		m_AngleToX.InsertEmpty(iAngleRange);

		for (i = 0; i < iAngleRange; i++)
			{
			Metric rLong = (Metric)i / iAngleRange;
			m_AngleToX[i] = (int)(rLong * m_pTexture->GetWidth());
			}
		}

	if (m_RadiusToY.GetCount() != iRadius)
		{
		m_RadiusToY.DeleteAll();
		m_RadiusToY.InsertEmpty(iRadius);

		for (i = 0; i < iRadius; i++)
			{
			//	Map this point to an offset into a sphere map.
			//
			//	Lat goes from 0.0 (north pole) to 1.0 (south pole).
			//	Note, however, that we expect our texture to only cover the 
			//	north pole, since we're only painting half. Thus we multiply the
			//	texture height times two.

			Metric rRadius = (Metric)i / iRadius;
			Metric rACosR = acos(rRadius);
			Metric rLat = 0.5 - (rACosR / PI);
			m_RadiusToY[i] = (int)(rLat * (2 * m_pTexture->GetHeight()));
			}
		}
	}
