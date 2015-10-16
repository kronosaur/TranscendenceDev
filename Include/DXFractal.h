//	DXFractal.h
//
//	Implements fractal image procedures
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CGCloudGenerator3D
	{
	public:
		CGCloudGenerator3D (int iScale, int iFrames = 256, int iMaxLevels = -1);

		Metric GetAtPeriodic (int x, int y, int z) const;
		inline Metric GetMaxValue (void) const { return m_rMaxValue; }

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
		TArray<SFreq> m_Frequencies;

		Metric m_rMaxValue;
	};

class CGFractal
	{
	public:
		//	Clouds

		static void FillClouds (CG8bitImage &Dest, int xDest, int yDest, int cxWidth, int cyHeight, int iScale, BYTE byMin = 0, BYTE byMax = 255);

		//	Spherical Textures

		static void CreateSphericalCloudAnimation (int cxWidth, int cyHeight, int iScale, int iDetail, int iFrames, TArray<CG8bitImage> *retFrames);
		inline static void CreateSphericalCloudMap (int cxWidth, int cyHeight, int iScale, int iDetail, CG8bitImage *retImage) { return CreateSphericalCloudMap(cxWidth, cyHeight, CGCloudGenerator3D(iScale), iDetail, 0.0, retImage); }

	private:

		//	Helpers

		static void CreateSphericalCloudMap (int cxWidth, int cyHeight, CGCloudGenerator3D &Generator, int iDetail, Metric rOffset, CG8bitImage *retImage);
	};