//	CGFractal.cpp
//
//	CGDraw Class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CGFractal::CreateSphericalCloudAnimation (int cxWidth, int cyHeight, int iScale, int iDetail, int iFrames, TArray<CG8bitImage> *retFrames)

//	CreateSphericalCloudAnimation
//
//	Creates a set of spherical maps

	{
	int i;

	ASSERT(iFrames >= 0);

	retFrames->DeleteAll();
	if (iFrames <= 0)
		return;

	//	Create a generator

	int iRange = 2;
	CGCloudGenerator3D Generator(iScale, iRange);

	Metric rFactor = (Metric)(iScale * iRange) / iFrames;

	//	Build all frames

	retFrames->InsertEmpty(iFrames);
	for (i = 0; i < iFrames; i++)
		{
		Metric rOffset = (Metric)i * rFactor;
		CreateSphericalCloudMap(cxWidth, cyHeight, Generator, iDetail, 1.0, rOffset, &retFrames->GetAt(i));
		}
	}

void CGFractal::CreateSphericalCloudMap (int cxWidth, int cyHeight, CGCloudGenerator3D &Generator, int iDetail, Metric rContrast, Metric rOffset, CG8bitImage *retImage)

//	CreateSphericalCloudMap
//
//	Creates a image such that when wrapped on a sphere, it will display fractal
//	clouds.
//
//	See: Texture & Modeling: A Procedural Approach pp.119-120.

	{
	if (cxWidth <= 0 || cyHeight <= 0)
		return;

	//	Create the image

	retImage->Create(cxWidth, cyHeight);

	Metric rUInc = 1.0 / (Metric)cxWidth;
	Metric rVInc = 0.5 / (Metric)cyHeight;

	//	Compute the factor to adjust the pixel value

	Metric rRange = (Metric)(255) + 0.99999;
	Metric rFactor = rRange / (2.0 * Generator.GetMaxValue());
	
	//	The generator origin needs to be larger than the level of detail
	//	so that we can always end up with positive numbers in GetAtPeriodic.
	//	But we want it to be constant, if possible, so that different levels
	//	of detail are centered.

	int iOrigin = Max(10000, iDetail);

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
			
			Metric rValue = rContrast * Generator.GetAtPeriodic(iOrigin + (int)(rX * iDetail), iOrigin + (int)(rY * iDetail), iOrigin + (int)(rZ + rOffset));

			//	Map to our range

			*pDest++ = (BYTE)Clamp((DWORD)(rFactor * (Generator.GetMaxValue() + rValue)), (DWORD)0, (DWORD)255);

			//	Next

			rU += rUInc;
			}

		pDestRow = retImage->NextRow(pDestRow);
		rV += rVInc;
		}
	}

void CGFractal::FillClouds (CG8bitImage &Dest, int xDest, int yDest, int cxWidth, int cyHeight, int iScale, BYTE byMin, BYTE byMax)

//	FillClouds
//
//	Fills an 8-bit image with fractal clouds.

	{
	//	Make sure we're in bounds

	if (!Dest.AdjustCoords(NULL, NULL, 0, 0, 
			&xDest, &yDest, 
			&cxWidth, &cyHeight))
		return;

	//	Create a cloud generator

	CCloudGenerator Generator(iScale);

	//	Compute the factor to adjust the pixel value

	Metric rRange = (Metric)(byMax - byMin) + 0.99999;
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

			*pDest++ = (BYTE)Clamp((DWORD)byMin + (DWORD)(rFactor * (Generator.GetMaxValue() + rValue)), (DWORD)byMin, (DWORD)byMax);
			}

		pDestRow = Dest.NextRow(pDestRow);
		Generator.NextY();
		}
	}
