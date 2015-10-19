//	CGFractal.cpp
//
//	CGDraw Class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CGFractal::CreateSphericalCloudAnimation (int cxWidth, int cyHeight, const SGCloudDesc &Desc, int iFrames, bool bHemisphere, TArray<CG8bitImage> *retFrames)

//	CreateSphericalCloudAnimation
//
//	Creates a set of spherical maps

	{
	int i;

	ASSERT(iFrames >= 0);

	retFrames->DeleteAll();
	if (iFrames <= 0)
		return;

	//	Create our own cloud descriptor

	SGCloudDesc CloudDesc(Desc);

	//	Always create our own generator

	int iRange = 2;
	CGCloudGenerator3D Generator(CloudDesc.iScale, iRange);
	Metric rFactor = (Metric)(CloudDesc.iScale * iRange) / iFrames;

	//	Build all frames

	retFrames->InsertEmpty(iFrames);
	for (i = 0; i < iFrames; i++)
		{
		CloudDesc.rOffset = (Metric)i * rFactor;
		CreateSphericalCloudMap(cxWidth, cyHeight, CloudDesc, Generator, bHemisphere, &retFrames->GetAt(i));
		}
	}

void CGFractal::CreateSphericalCloudMap (int cxWidth, int cyHeight, const SGCloudDesc &Desc, CGCloudGenerator3D &Generator, bool bHemisphere, CG8bitImage *retImage)

//	CreateSphericalCloudMap
//
//	Creates a image such that when wrapped on a sphere, it will display fractal
//	clouds.
//
//	NOTE: We only return one half of the sphere texture (one pole) since we are 
//	using this to paint to a 2D circle.
//
//	See: Texture & Modeling: A Procedural Approach pp.119-120.

	{
	if (cxWidth <= 0 || cyHeight <= 0)
		return;

	//	Create the image

	retImage->Create(cxWidth, cyHeight);

	//	NOTE: Since we only ever paint half a sphere, we don't need the entire
	//	bitmap, we only fill in the north half of the spehere (which is why we
	//	multiply the height * 2 below.

	Metric rUInc = 1.0 / (Metric)cxWidth;
	Metric rVInc = 0.5 / (Metric)((bHemisphere ? 2 : 1) * cyHeight);

	//	Compute the factor to adjust the pixel value

	Metric rRange = (Metric)(Desc.byMax - Desc.byMin) + 0.99999;
	Metric rFactor = rRange / (2.0 * Generator.GetMaxValue());
	
	//	The generator origin needs to be larger than the level of detail
	//	so that we can always end up with positive numbers in GetAtPeriodic.
	//	But we want it to be constant, if possible, so that different levels
	//	of detail are centered.

	int iOrigin = Max(10000, (int)Desc.rDetail);

	//	Loop

	BYTE *pDestRow = retImage->GetPixelPos(0, 0);
	BYTE *pDestRowEnd = retImage->GetPixelPos(0, 0 + cyHeight);
	Metric rV = 0.25;

	while (pDestRow < pDestRowEnd)
		{
		BYTE *pDest = pDestRow;
		BYTE *pDestEnd = pDestRow + cxWidth;
		Metric rU = 0.5;

		while (pDest < pDestEnd)
			{
			//	Convert this to a 3D point on a unit sphere with center at 0,0,0

			Metric rX = cos((rV - 0.5) * 2.0 * g_Pi) * sin(rU * 2.0 * g_Pi);
			Metric rY = cos((rV - 0.5) * 2.0 * g_Pi) * cos(rU * 2.0 * g_Pi);
			Metric rZ = sin((rV - 0.5) * 2.0 * g_Pi);

			//	Get a value from the generator. We use the periodic fuction, 
			//	which uses frames (passed in to the generator) as the period.
			//	The period is only on the Z-axis, so we don't multiply the Z
			//	by detail (because we want smaller changes).
			
			Metric rValue = Desc.rContrast * Generator.GetAtPeriodic(iOrigin + (int)(rX * Desc.rDetail), iOrigin + (int)(rY * Desc.rDetail), iOrigin + (int)(rZ + Desc.rOffset));

			//	Map to our range

			*pDest++ = (BYTE)Clamp((DWORD)Desc.byMin + (DWORD)(rFactor * (Generator.GetMaxValue() + rValue)), (DWORD)Desc.byMin, (DWORD)Desc.byMax);

			//	Next

			rU += rUInc;
			}

		pDestRow = retImage->NextRow(pDestRow);
		rV += rVInc;
		}
	}

void CGFractal::FillClouds (CG8bitImage &Dest, int xDest, int yDest, int cxWidth, int cyHeight, const SGCloudDesc &Desc)

//	FillClouds
//
//	Fills an 8-bit image with fractal clouds.

	{
	//	Make sure we're in bounds

	if (!Dest.AdjustCoords(NULL, NULL, 0, 0, 
			&xDest, &yDest, 
			&cxWidth, &cyHeight))
		return;

	//	Create a cloud generator, if necessary

	CCloudGenerator Generator(Desc.iScale);

	//	Compute the factor to adjust the pixel value

	Metric rRange = (Metric)(Desc.byMax - Desc.byMin) + 0.99999;
	Metric rFactor = rRange / (2.0 * Generator.GetMaxValue());

	//	Fill

	BYTE *pDestRow = Dest.GetPixelPos(xDest, yDest);
	BYTE *pDestRowEnd = Dest.GetPixelPos(xDest, yDest + cyHeight);
	Generator.ResetY(yDest);

	while (pDestRow < pDestRowEnd)
		{
		BYTE *pDest = pDestRow;
		BYTE *pDestEnd = pDestRow + cxWidth;
		Generator.ResetX(xDest);

		while (pDest < pDestEnd)
			{
			//	The generator will return a value between -maxValue and +maxValue

			Metric rValue = Generator.GetNext();

			//	Map to our range

			*pDest++ = (BYTE)Clamp((DWORD)Desc.byMin + (DWORD)(rFactor * (Generator.GetMaxValue() + rValue)), (DWORD)Desc.byMin, (DWORD)Desc.byMax);
			}

		pDestRow = Dest.NextRow(pDestRow);
		Generator.NextY();
		}
	}
