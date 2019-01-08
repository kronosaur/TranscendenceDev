//	DXFractal.h
//
//	Implements fractal image procedures
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

struct SGCloudDesc
	{
	SGCloudDesc (void) :
			iScale(16),
			rDetail(1.0),
			rContrast(1.0),
			rOffset(0.0),
			byMin(0),
			byMax(255)
		{ }

	int iScale;
	Metric rDetail;
	Metric rContrast;
	Metric rOffset;
	BYTE byMin;
	BYTE byMax;
	};

class CGCloudGenerator3D
	{
	public:
		CGCloudGenerator3D (int iScale, int iFrames = 256, int iMaxLevels = -1);

		Metric GetAtPeriodic (int x, int y, int z) const;
		inline int GetFrames (void) const { return m_iFrames; }
		inline int GetMaxLevels (void) const { return m_iMaxLevels; }
		inline Metric GetMaxValue (void) const { return m_rMaxValue; }
		inline int GetScale (void) const { return m_iScale; }

	private:
		struct SFreq
			{
			SFreq (void) : pNoise(NULL)
				{ }

			~SFreq (void)
				{
				if (pNoise)
					delete pNoise;
				}

			int iFreq;
			Metric rAmplitude;
			CNoiseGenerator *pNoise;
			SNoisePos x;
			SNoisePos y;
			SNoisePos z;
			};

		int m_iScale;
		int m_iFrames;
		int m_iMaxLevels;
		TArray<SFreq> m_Frequencies;

		Metric m_rMaxValue;
	};

class CGFractal
	{
	public:
		//	Clouds

		static void FillClouds (CG8bitImage &Dest, int xDest, int yDest, int cxWidth, int cyHeight, const SGCloudDesc &Desc);

		//	Spherical Textures

		static void CreateSphericalCloudAnimation (int cxWidth, int cyHeight, const SGCloudDesc &Desc, int iFrames, bool bHemisphere, TArray<CG8bitImage> *retFrames);
		static void CreateSphericalCloudMap (int cxWidth, int cyHeight, const SGCloudDesc &Desc, CGCloudGenerator3D &Generator, bool bHemisphere, CG8bitImage *retImage);
	};