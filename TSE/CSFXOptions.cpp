//	CSFXOptions.cpp
//
//	CSFXOptions class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CSFXOptions::SetSFXQuality (ESFXQuality iQuality)

//	SetSFXQuality
//
//	Sets options to the given quality defaults.

	{
	//	These options are independent of quality

	m_iQuality = iQuality;
	m_b3DSystemMap = true;
	m_bManeuveringEffect = false;

	//	Set options based on quality

	switch (iQuality)
		{
		case sfxMaximum:
			m_bSpaceBackground = true;
			m_bStarGlow = true;
			m_bStarshine = true;
			break;

		case sfxStandard:
			m_bSpaceBackground = true;
			m_bStarGlow = true;
			m_bStarshine = false;
			break;

		case sfxMinimum:
			m_bSpaceBackground = false;
			m_bStarGlow = false;
			m_bStarshine = false;
			break;

		default:
			ASSERT(false);
		}
	}

void CSFXOptions::SetSFXQualityAuto (void)

//	SetSFXQualityAuto
//
//	Tests the graphics performance of the system and then sets the graphics 
//	quality based on the result. This call takes a few 100 ms to run.

	{
	int i;
	const int IMAGE_SIZE = 1000;
	const int ITERATION_COUNT = 6;

	//	Create a 1 megapixel image and do tests on it.

	CG32bitImage TestImage;
	TestImage.Create(IMAGE_SIZE, IMAGE_SIZE, CG32bitImage::alpha8);

	//	Start timing

	DWORDLONG dwStart = ::GetTickCount64();

	//	Paint some gradient circles to test speed

	for (i = 0; i < ITERATION_COUNT; i++)
		CGDraw::CircleGradient(TestImage, IMAGE_SIZE / 2, IMAGE_SIZE / 2, IMAGE_SIZE / 2, CG32bitPixel(255, 255, 255, 128));

	//	Timing

	DWORDLONG dwElapsed = ::GetTickCount64() - dwStart;
	int iProcCount = ::sysGetProcessorCount();

	//	If more than 500 ms, then we go to minimum quality

	if (dwElapsed > 500)
		{
		SetSFXQuality(sfxMinimum);
		::kernelDebugLogMessage("Graphics quality minimum (max performance): %d ms; %d cores.", (DWORD)dwElapsed, iProcCount);
		}

	//	If more than 200 ms, or if we have less than four processors, then
	//	go to standard quality.

	else if (dwElapsed > 200 || iProcCount < 4)
		{
		SetSFXQuality(sfxStandard);
		::kernelDebugLogMessage("Graphics quality standard (balanced performance): %d ms; %d cores.", (DWORD)dwElapsed, iProcCount);
		}

	//	Otherwise, we go to 11

	else
		{
		SetSFXQuality(sfxMaximum);
		::kernelDebugLogMessage("Graphics quality maximum: %d ms; %d cores.", (DWORD)dwElapsed, iProcCount);
		}
	}

