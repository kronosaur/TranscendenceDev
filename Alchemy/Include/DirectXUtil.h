//	DirectXUtil.h
//
//	DirectX Utilities
//	Copyright 2012 by Kronosaur Productions, LLC. All Rights Reserved.
//
//	We require:
//
//		windows.h
//		Alchemy.h

#pragma once

#include <d3d9.h>

#ifndef __DDRAW_INCLUDED__
#include "ddraw.h"
#endif

#ifndef __DSOUND_INCLUDED__
#include "dsound.h"
#endif

#ifndef _INC_VFW
#include "vfw.h"
#endif

#ifndef INCL_EUCLID
#include "Euclid.h"
#endif

#ifdef DEBUG
//#define DEBUG_NO_DX_BLT_THREAD
#endif

#define BLT_RANGE_CHECK

#define ALPHA_BITS				8
#define ALPHA_LEVELS			256

const int MAX_COORD =			1000000;
const int MIN_COORD =			-1000000;

//	We pick a color that is unlikely to be confused
//	(e.g., black is very common, so we can't use that).

const WORD DEFAULT_TRANSPARENT_COLOR = 0xfc1f;

class CG16bitFont;
class CG16bitImage;

enum AlignmentStyles
	{
	alignLeft =				0x00000001,
	alignCenter =			0x00000002,
	alignRight =			0x00000004,
	alignTop =				0x00000008,
	alignMiddle =			0x00000010,
	alignBottom =			0x00000020,
	};

enum ChannelTypes
	{
	channelNone,
	channelAlpha,
	channelRed,
	channelGreen,
	channelBlue,
	};

//	ChannelMap is meant to be computed from two ChannelTypes
//	iChannelMap = iSrcChannel | (iDestChannel << 8);
enum ChannelMap
	{
	channelMapNone,				//0x0
	//	Mapping a channel to itself or none
	channelMapAlpha,				//0x1
	channelMapRed,					//0x2
	channelMapGreen,				//0x3
	channelMapBlue,					//0x4
	channelMapAlphaToAlpha =		0x0101,
	channelMapRedToRed =			0x0202,
	channelMapGreenToGreen =		0x0303,
	channelMapBlueToBlue =			0x0404,

	channelMapAlphaToRed =			0x0201,
	channelMapAlphaToGreen =		0x0301,
	channelMapAlphaToBlue =			0x0401,

	channelMapRedToAlpha =			0x0102,
	channelMapRedToGreen =			0x0302,
	channelMapRedToBlue =			0x0402,

	channelMapGreenToAlpha =		0x0103,
	channelMapGreenToRed =			0x0203,
	channelMapGreenToBlue =			0x0403,
	
	channelMapBlueToAlpha =			0x0104,
	channelMapBlueToRed =			0x0204,
	channelMapBlueToGreen =			0x0304,
	};

enum GradientDirections
	{
	gradientNone,
	gradientHorizontal,
	gradientVertical,
	};

enum MarkerTypes
	{
	markerPixel,
	markerRoundDot,
	markerSquareDot,
	markerDiamondDot,
	markerTriangleUpDot,
	markerTriangleDownDot,

	markerSmallCross,
	markerMediumCross,

	markerSmallSquare,
	markerSmallFilledSquare,

	markerTinyCircle,
	markerSmallCircle,
	markerSmallFilledCircle,

	markerSmallTriangleUp,
	markerSmallFilledTriangleUp,

	markerSmallTriangleDown,
	markerSmallFilledTriangleDown,

	markerSmallDiamond,
	markerSmallFilledDiamond,
	};

enum SurfaceTypes
	{
	r5g5b5,
	r5g6b5,
	r8g8b8,

	stUnknown,
	};

struct SPoint
	{
	SPoint (int xArg = 0, int yArg = 0) : 
			x(xArg), 
			y(yArg)
		{ }

	int x;
	int y;
	};

inline int PointsToPixels (int iPoints, int iDPI = 96) { return ::MulDiv(iPoints, iDPI, 72); }

#include "TImagePlane.h"
#include "DXImage32.h"
#include "DXImageReal.h"
#include "DXImage16.h"
#include "DXImage8.h"

#include "DXRegion.h"
#include "DXPath.h"

#include "TRegionPainter.h"

//	 8-bit drawing functions (for masks) ---------------------------------------

void DrawHorzLine8bit (CG16bitImage &Dest, int x, int y, int cxWidth, BYTE byValue);
void DrawFilledCircle8bit (CG16bitImage &Dest, int x, int y, int iRadius, BYTE byValue);
void DrawFilledRect8bit (CG16bitImage &Dest, int x, int y, int cxWidth, int cyHeight, BYTE byValue);
void DrawGradientCircle8bit (CG16bitImage &Dest, 
							 int xDest, 
							 int yDest, 
							 int iRadius,
							 BYTE byCenter,
							 BYTE byEdge);
void DrawNebulosity8bit (CG16bitImage &Dest, int x, int y, int cxWidth, int cyHeight, int iScale, BYTE byMin, BYTE byMax);
void DrawNoise8bit (CG16bitImage &Dest, int x, int y, int cxWidth, int cyHeight, int iScale, BYTE byMin, BYTE byMax);
void RasterizeQuarterCircle8bit (int iRadius, int *retSolid, BYTE *retEdge, DWORD byOpacity = 255);

//	Implementation Helpers -----------------------------------------------------

#include "TBlt.h"
#include "TFill.h"
#include "TCirclePainter.h"
#include "TLinePainter.h"
#include "TRoundedRectPainter.h"

//	Noise Functions ------------------------------------------------------------

struct SNoisePos
	{
	int iInteger;
	int iFraction;
	};

class CNoiseGenerator
	{
	public:
		CNoiseGenerator (int iScale);
		~CNoiseGenerator (void);

		inline Metric GetAt (SNoisePos &x, SNoisePos &y);
		inline Metric GetAt (SNoisePos &x, SNoisePos &y, SNoisePos &z);
		inline Metric GetAtPeriodic (int iPeriod, SNoisePos &x, SNoisePos &y, SNoisePos &z);
		void Next (SNoisePos &i) { if (++i.iFraction == m_iScale) { i.iInteger++; i.iFraction = 0; } }
		void Reset (SNoisePos &i, int iPos) { i.iInteger = iPos / m_iScale; i.iFraction = iPos % m_iScale; }

	private:
		int m_iScale;
		Metric *m_Frac;
		Metric *m_InvFrac;
		Metric *m_Smooth;
	};

Metric Noise2D (int x, Metric fx0, Metric fx1, Metric wx, int y, Metric fy0, Metric fy1, Metric wy);
Metric Noise3D (int x, Metric fx0, Metric fx1, Metric wx, int y, Metric fy0, Metric fy1, Metric wy, int z, Metric fz0, Metric fz1, Metric wz);
void NoiseInit (DWORD dwSeed = 0);
void NoiseReinit (DWORD dwSeed = 0);
Metric NoiseSmoothStep (Metric x);
Metric PeriodicNoise3D (int iPeriod, int x, Metric fx0, Metric fx1, Metric wx, int y, Metric fy0, Metric fy1, Metric wy, int z, Metric fz0, Metric fz1, Metric wz);

inline Metric CNoiseGenerator::GetAt (SNoisePos &x, SNoisePos &y)
	{ return Noise2D(x.iInteger, m_Frac[x.iFraction], m_InvFrac[x.iFraction], m_Smooth[x.iFraction], y.iInteger, m_Frac[y.iFraction], m_InvFrac[y.iFraction], m_Smooth[y.iFraction]); }

inline Metric CNoiseGenerator::GetAt (SNoisePos &x, SNoisePos &y, SNoisePos &z)
	{ return Noise3D(x.iInteger, m_Frac[x.iFraction], m_InvFrac[x.iFraction], m_Smooth[x.iFraction], y.iInteger, m_Frac[y.iFraction], m_InvFrac[y.iFraction], m_Smooth[y.iFraction], z.iInteger, m_Frac[z.iFraction], m_InvFrac[z.iFraction], m_Smooth[z.iFraction]); }

inline Metric CNoiseGenerator::GetAtPeriodic (int iPeriod, SNoisePos &x, SNoisePos &y, SNoisePos &z)
	{ return PeriodicNoise3D(iPeriod, x.iInteger, m_Frac[x.iFraction], m_InvFrac[x.iFraction], m_Smooth[x.iFraction], y.iInteger, m_Frac[y.iFraction], m_InvFrac[y.iFraction], m_Smooth[y.iFraction], z.iInteger, m_Frac[z.iFraction], m_InvFrac[z.iFraction], m_Smooth[z.iFraction]); }

#include "TextFormat.h"
#include "DXUI.h"
#include "DXSparseMask.h"
#include "DXFractal.h"

//	Utility Functions ---------------------------------------------------------

bool dxLoadImageFile (const CString &sFilespec, HBITMAP *rethDIB, EBitmapTypes *retiType = NULL, CString *retsError = NULL);

//	Screen Manager ------------------------------------------------------------

#include "DXScreenMgr3D.h"

//	Inlines

inline void AGArea::Invalidate (const RECT &rcInvalid) { if (m_pScreen) m_pScreen->Invalidate(rcInvalid); }

//	DirectSound Classes -------------------------------------------------------

class CSoundMgr
	{
	public:
		struct SMusicPlayState
			{
			CString sFilename;
			bool bPlaying;
			bool bPaused;
			int iLength;
			int iPos;
			};

		CSoundMgr (void);
		~CSoundMgr (void);

		ALERROR Init (HWND hWnd);
		void CleanUp (void);

		void Delete (int iChannel);
		ALERROR LoadWaveFile (const CString &sFilename, int *retiChannel);
		ALERROR LoadWaveFromBuffer (IReadBlock &Data, int *retiChannel);
		void Play (int iChannel, int iVolume, int iPan, bool bLoop = false);
		void SetWaveVolume (int iVolumeLevel) { m_iSoundVolume = iVolumeLevel; }
		void Stop (int iChannel);

		bool CanPlayMusic (const CString &sFilename);
		void GetMusicCatalog (const CString &sMusicFolder, TArray<CString> *retCatalog);
		bool GetMusicPlayState (SMusicPlayState *retState);
		bool PlayMusic (const CString &sFilename, int iPos = 0, CString *retsError = NULL);
		int SetMusicVolume (int iVolumeLevel);
		void StopMusic (void);
		void TogglePlayPaused (void);

	private:
		struct SChannel
			{
			LPDIRECTSOUNDBUFFER pBuffer;
			CString sFilename;
			bool bLooping = false;

			SChannel *pNext = NULL;
			};

		void AddMusicFolder (const CString &sFolder, TArray<CString> *retCatalog);
		int AllocChannel (void);
		void CleanUpChannel (SChannel &Channel);
		SChannel *GetChannel (int iChannel) { return &m_Channels[iChannel]; }
		int GetChannelCount (void) { return m_Channels.GetCount(); }
		ALERROR LoadWaveFile (HMMIO hFile, int *retiChannel);

		LPDIRECTSOUND m_pDS;
		TArray<SChannel> m_Channels;
		HWND m_hMusic;

		int m_iSoundVolume;
		int m_iMusicVolume;
	};

//	Miscellaneous Classes -----------------------------------------------------

class CFrameRateCounter
	{
	public:
		CFrameRateCounter (int iSize = 128);
		~CFrameRateCounter (void);

		Metric GetFrameRate (void);
		void MarkFrame (void);

	private:
		int m_iSize;
		DWORD *m_pTimes;
		int m_iCount;
		int m_iPos;
	};

typedef BYTE AlphaArray5 [32];
typedef BYTE AlphaArray6 [64];
extern AlphaArray5 g_Alpha5 [256];
extern AlphaArray6 g_Alpha6 [256];
