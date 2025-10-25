//	CSFXOptions.cpp
//
//	CSFXOptions class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

constexpr DWORD OPTIMAL_MAX_BKRND_PAINT_THREADS = 16;		//	This has not been tested, this is just the original legacy value
constexpr DWORD OPTIMAL_MAX_SPRITE_PAINT_THREADS = 32;		//	Based on testing, this appears to be a number around 32-48 for per-sprite parallelism

void CSFXOptions::SetSFXQuality (ESFXQuality iQuality)

//	SetSFXQuality
//
//	Sets options to the given quality defaults.

	{
	//	These options are independent of quality

	m_iQuality = iQuality;
	m_b3DSystemMap = true;
	m_bManeuveringEffect = false;

	CalcPaintThreads();

	//	Set options based on quality

	switch (iQuality)
		{
		case sfxMaximum:
			m_b3DExtras = true;
			m_bDockScreenTransparent = true;
			m_bHUDTransparent = true;
			m_bSpaceBackground = true;
			m_bStargateTravelEffect = true;
			m_bStarGlow = true;
			m_bStarshine = true;
			break;

		case sfxStandard:
			m_b3DExtras = true;
			m_bDockScreenTransparent = true;
			m_bHUDTransparent = true;
			m_bSpaceBackground = true;
			m_bStargateTravelEffect = true;
			m_bStarGlow = true;
			m_bStarshine = false;
			break;

		case sfxMinimum:
			m_b3DExtras = false;
			m_bDockScreenTransparent = false;
			m_bHUDTransparent = false;
			m_bSpaceBackground = false;
			m_bStargateTravelEffect = false;
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
	//	Calculate paint workers

	CalcPaintThreads();

	//	Prepare to run a simple benchmark (eventually there should be some MT testing in CalcPaintThreads)

	int i;
	const int IMAGE_SIZE = 1000;
	const int ITERATION_COUNT = 6;

	//	Create a 1 megapixel image and do tests on it.

	CG32bitImage TestImage;
	TestImage.Create(IMAGE_SIZE, IMAGE_SIZE, CG32bitImage::alpha8);

	//	Start timing

	DWORD dwStart = ::GetTickCount();

	//	Paint some gradient circles to test speed

	for (i = 0; i < ITERATION_COUNT; i++)
		CGDraw::CircleGradient(TestImage, IMAGE_SIZE / 2, IMAGE_SIZE / 2, IMAGE_SIZE / 2, CG32bitPixel(255, 255, 255, 128));

	//	Timing

	DWORD dwElapsed = sysGetTicksElapsed(dwStart);
	int iProcCount = ::sysGetProcessorCount();

	//	If more than 500 ms, then we go to minimum quality

	if (dwElapsed > 500)
		{
		SetSFXQuality(sfxMinimum);
		::kernelDebugLogPattern("Graphics quality minimum (max performance): %d ms; %d cores.", (DWORD)dwElapsed, iProcCount);
		}

	//	If more than 200 ms, or if we have less than four processors, then
	//	go to standard quality.

	else if (dwElapsed > 200 || iProcCount < 2)
		{
		SetSFXQuality(sfxStandard);
		::kernelDebugLogPattern("Graphics quality standard (balanced performance): %d ms; %d cores.", (DWORD)dwElapsed, iProcCount);
		}

	//	Otherwise, we go to 11

	else
		{
		SetSFXQuality(sfxMaximum);
		::kernelDebugLogPattern("Graphics quality maximum: %d ms; %d cores.", (DWORD)dwElapsed, iProcCount);
		}
	}

// SetMinSpriteChunkSize
// 
//	Sets the min sprite chunk size, rounded down to the
//  nearest power of two.
//	
//	This is expressed as an exponent 
//
void CSFXOptions::SetMinSpriteChunkSize(int iMinSize)
	{
	m_dwMinSpriteChunkSizePow = 0;
	DWORD dwMinSize = (DWORD)iMinSize;
	if (dwMinSize)
		{
		for (m_dwMinSpriteChunkSizePow = 0; dwMinSize > 1; m_dwMinSpriteChunkSizePow++)
			dwMinSize = dwMinSize >> 1;
		}
	}

//	CalcPaintThreads
//
//	Determine the maximum threads to use for
//  different paint operations.
//  If you add more MT paint options, ensure that you
//  add appropriate member variables to control them.
//  
//	Initializes:
//		m_iMaxBkrndPaintThreads
//		m_iMaxSpritePaintThreads
// 
//	Note: as of 2.0 Alpha 7, multithreaded painting is
//	still a synchronous operation. We will need to do more
//  testing & modify this function to be more appropriately
//  sophisticated if we move to an asynchronous painting
//  model
//	
void CSFXOptions::CalcPaintThreads(void)
	{
	kernelDebugLogPattern("Found %d logical cores and %d physical cores across %d processor groups", m_sProcessorInfo.dwNumLogical, m_sProcessorInfo.dwNumPhysical, m_sProcessorInfo.dwNumProcessorGroups);
	
	//	configure m_iMaxBkrndPaintThreads - this is used for constructing the background image.

	//	small processors with few physical cores tend to benefit more from using logical processors
	if (m_sProcessorInfo.dwNumPhysical < 8)
		m_iMaxBkrndPaintWorkers = min(OPTIMAL_MAX_BKRND_PAINT_THREADS, m_sProcessorInfo.dwNumLogical);

	//	big processors with many physical cores tend to benefit more from allocating one thread per core
	else
		m_iMaxBkrndPaintWorkers = min(OPTIMAL_MAX_BKRND_PAINT_THREADS, m_sProcessorInfo.dwNumPhysical);

	kernelDebugLogPattern("Calculated number of background-painting threads to use: %d", m_iMaxBkrndPaintWorkers);

	//	configure m_iMaxSpritePaintThreads - this is used for constructing individual sprites.

	//	small processors with few physical cores tend to benefit more from using logical processors
	if (m_sProcessorInfo.dwNumPhysical < 8)
		m_iMaxSpritePaintWorkers = min(OPTIMAL_MAX_SPRITE_PAINT_THREADS, m_sProcessorInfo.dwNumLogical);

	//	big processors with many physical cores tend to benefit more from allocating one thread per core
	else
		m_iMaxSpritePaintWorkers = min(OPTIMAL_MAX_SPRITE_PAINT_THREADS, m_sProcessorInfo.dwNumPhysical);

	kernelDebugLogPattern("Calculated number of sprite-painting threads to use: %d", m_iMaxSpritePaintWorkers);

	//	Set the default state of sprite painting. This may be modified by settings later

	if (m_iMaxSpritePaintWorkers < 2)
		{
		m_bUseMTSpritePaint = false;
		kernelDebugLogString(CONSTLIT("Insufficient logical cores available for multithreaded sprite painting."));
		}
	else
		m_bUseMTSpritePaint = true;

	//	Set the default state of background painting. This may be modified by settings later

	if (m_iMaxBkrndPaintWorkers < 2)
		{
		m_bUseMTBkrndPaint = false;
		kernelDebugLogString(CONSTLIT("Insufficient logical cores available for multithreaded background painting."));
		}
	else
		m_bUseMTBkrndPaint = true;
	}

