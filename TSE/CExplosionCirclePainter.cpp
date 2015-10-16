//	CExplosionCirclePainter.cpp
//
//	CExplosionCirclePainter class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "SFXExplosionImpl.h"

TArray<CG8bitImage> CExplosionCirclePainter::m_CloudTextures;

CExplosionCirclePainter::CExplosionCirclePainter (void) :
		m_iCurFrame(0)

//	CExplosionCirclePainter constructor

	{
	int iRadius = 100;
	int iScale = 16;
	int iFrames = 1;
	int iDetail = iRadius * 4;

	//	Create a cloud texture, if necessary

	if (m_CloudTextures.GetCount() == 0)
		{
		int cxTexture = iRadius * 4;
		int cyTexture = iRadius * 2;

		CGFractal::CreateSphericalCloudAnimation(cxTexture, cyTexture, iScale, iDetail, iFrames, &m_CloudTextures);
		}
	}

