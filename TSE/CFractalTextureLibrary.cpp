//	CFractalTextureLibrary.cpp
//
//	CFractalTextureLibrary class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

const int TEXTURE_WIDTH =					512;
const int TEXTURE_HEIGHT =					128;
const int TEXTURE_SCALE =					16;
const int TEXTURE_DETAIL_LEVELS =			64;
const Metric TEXTURE_DETAIL_MAX =			200.0;
const Metric TEXTURE_DETAIL_MIN =			20.0;
const Metric TEXTURE_CONTRAST =				1.25;

const Metric TEXTURE_BOILING_CONTRAST =		1.0;
const Metric TEXTURE_BOILING_DETAIL =		100.0;
const int TEXTURE_BOILING_FRAMES =			32;

static CG8bitImage NullImage;

const CG8bitImage &CFractalTextureLibrary::GetTexture (ETextureTypes iType, int iFrame) const

//	GetTexture
//
//	Returns the given texture

	{
	switch (iType)
		{
		case typeBoilingClouds:
			return ((iFrame >= 0 && iFrame < m_BoilingTextures.GetCount()) ? m_BoilingTextures[iFrame] : NullImage);

		case typeExplosion:
			return ((iFrame >= 0 && iFrame < m_ExplosionTextures.GetCount()) ? m_ExplosionTextures[iFrame] : NullImage);

		default:
			return NullImage;
		}
	}

int CFractalTextureLibrary::GetTextureCount (ETextureTypes iType) const

//	GetTextureCount
//
//	Returns the number of texture frames for this type.

	{
	switch (iType)
		{
		case typeBoilingClouds:
			return m_BoilingTextures.GetCount();

		case typeExplosion:
			return m_ExplosionTextures.GetCount();

		default:
			return 0;
		}
	}

void CFractalTextureLibrary::Init (void)

//	Init
//
//	Initialize

	{
	if (!m_bInitialized)
		{
		//	Explosion textures are a sequence of fractal clouds, suitable for
		//	mapping to a sphere, with increasing detail levels.

		InitExplosionTextures();

		//	Boiling clouds are a cyclical animation

		SGCloudDesc CloudDesc;
		CloudDesc.rContrast = TEXTURE_BOILING_CONTRAST;
		CloudDesc.rDetail = TEXTURE_BOILING_DETAIL;
		CGFractal::CreateSphericalCloudAnimation(TEXTURE_WIDTH, TEXTURE_HEIGHT, CloudDesc, TEXTURE_BOILING_FRAMES, true, &m_BoilingTextures);

		//	Done

		m_bInitialized = true;
		}
	}

void CFractalTextureLibrary::InitExplosionTextures (void)

//	InitExplosionTextures
//
//	Initialize explosion textures

	{
	int i;

	//	Create several cloud textures, each at a different detail level.

	if (m_ExplosionTextures.GetCount() == 0)
		{
		m_ExplosionTextures.InsertEmpty(TEXTURE_DETAIL_LEVELS);

		//	Create a generator for use in all frames. We use the same generator
		//	so that we get a consistent look.
		//
		//	NOTE: We don't specify a frame count because that is only important
		//	for periodic animations. In this case, we just want an explosion.

		CGCloudGenerator3D Generator(TEXTURE_SCALE);

		//	Create a cloud definition structure

		SGCloudDesc CloudDesc;
		CloudDesc.rContrast = TEXTURE_CONTRAST;

		//	Build all frames. We decrease the detail at every step to simulate the
		//	expansion of the explosion.

		CStepIncrementor Detail(CStepIncrementor::styleLinear, TEXTURE_DETAIL_MIN, TEXTURE_DETAIL_MAX, TEXTURE_DETAIL_LEVELS);

		//	Generate all frames

		for (i = 0; i < m_ExplosionTextures.GetCount(); i++)
			{
			CloudDesc.rDetail = Detail.GetAt(i);
			CGFractal::CreateSphericalCloudMap(TEXTURE_WIDTH, TEXTURE_HEIGHT, CloudDesc, Generator, true, &m_ExplosionTextures[i]);
			}
		}
	}
