//	CExplosionCirclePainter.cpp
//
//	CExplosionCirclePainter class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "SFXExplosionImpl.h"

const int TEXTURE_WIDTH =					512;
const int TEXTURE_HEIGHT =					256;
const int TEXTURE_SCALE =					16;
const int TEXTURE_DETAIL_LEVELS =			64;
Metric TEXTURE_DETAIL_MAX =					500.0;
Metric TEXTURE_DETAIL_MIN =					50.0;
Metric TEXTURE_DETAIL_RANGE =				(TEXTURE_DETAIL_MAX - TEXTURE_DETAIL_MIN);
Metric TEXTURE_DETAIL_INC =					(TEXTURE_DETAIL_RANGE / TEXTURE_DETAIL_LEVELS);
Metric TEXTURE_CONTRAST =					2.0;

TArray<CG8bitImage> CExplosionCirclePainter::m_CloudTextures;

CExplosionCirclePainter::CExplosionCirclePainter (void) :
		m_iCurFrame(0),
		m_pColorTable(NULL)

//	CExplosionCirclePainter constructor

	{
	Init();
	}

void CExplosionCirclePainter::DrawFrame (CG32bitImage &Dest, int x, int y, int iRadius, Metric rDetail, const TArray<CG32bitPixel> &ColorTable)

//	DrawFrame
//
//	Draws a single frame

	{
	ASSERT(ColorTable.GetCount() >= iRadius);

	//	Initialize members needed by the template

	m_pDest = &Dest;
	m_rcClip = &Dest.GetClipRect();
	m_xDest = x;
	m_yDest = y;
	m_iRadius = iRadius;
	m_iAngleRange = (int)(iRadius * 2.0 * g_Pi);

	//	Initialize some of our own members.

	m_iCurFrame = Max(0, Min((int)(rDetail * TEXTURE_DETAIL_LEVELS), TEXTURE_DETAIL_LEVELS - 1));
	m_pColorTable = &ColorTable;

	//	Draw (we get called at GetColorAt)

	DrawCircle();

	//	Restore

	m_pColorTable = NULL;
	m_iCurFrame = 0;
	m_pDest = NULL;
	m_rcClip = NULL;
	}

void CExplosionCirclePainter::Init (void)

//	Init
//
//	Initializes all cloud textures (which require some compute time to 
//	generate).

	{
	int i;

	//	Create several cloud textures, each at a different detail level.

	if (m_CloudTextures.GetCount() == 0)
		{
		m_CloudTextures.InsertEmpty(TEXTURE_DETAIL_LEVELS);

		//	Create a generator for use in all frames. We use the same generator
		//	so that we get a consistent look.
		//
		//	NOTE: We don't specify a frame count because that is only important
		//	for periodic animations. In this case, we just want an explosion.

		CGCloudGenerator3D Generator(TEXTURE_SCALE);

		//	Build all frames. We decrease the detail at every step to simulate the
		//	expansion of the explosion.

		Metric rDetail = TEXTURE_DETAIL_MIN;

		//	Generate all frames

		for (i = 0; i < m_CloudTextures.GetCount(); i++)
			{
			CGFractal::CreateSphericalCloudMap(TEXTURE_WIDTH, TEXTURE_HEIGHT, Generator, (int)rDetail, TEXTURE_CONTRAST, 0.0, &m_CloudTextures[i]);
			rDetail += TEXTURE_DETAIL_INC;
			}
		}
	}
