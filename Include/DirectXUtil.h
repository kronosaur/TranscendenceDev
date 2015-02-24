//	DirectXUtil.h
//
//	DirectX Utilities
//
//	We require:
//
//		windows.h
//		ddraw.h
//		Alchemy.h

#ifndef INCL_DIRECTXUTIL
#define INCL_DIRECTXUTIL

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


#define BLT_RANGE_CHECK

#define ALPHA_BITS				8
#define ALPHA_LEVELS			256

const int MAX_COORD =			1000000;
const int MIN_COORD =			-1000000;

//	We pick a color that is unlikely to be confused
//	(e.g., black is very common, so we can't use that).

const WORD DEFAULT_TRANSPARENT_COLOR = 0xfc1f;

typedef double REALPIXEL;

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

enum GradientDirections
	{
	gradientNone,
	gradientHorizontal,
	gradientVertical,
	};

enum MarkerTypes
	{
	markerPixel,
	markerSmallRound,
	markerSmallSquare,
	markerSmallCross,
	markerMediumCross,
	markerSmallFilledSquare,
	markerSmallCircle,
	markerTinyCircle,
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
	int x;
	int y;
	};

inline int PointsToPixels (int iPoints, int iDPI = 96) { return ::MulDiv(iPoints, iDPI, 72); }

class CGImagePlane
	{
	public:
		CGImagePlane (void);

		bool AdjustCoords (int *xSrc, int *ySrc, int cxSrc, int cySrc,
						   int *xDest, int *yDest,
						   int *cxWidth, int *cyHeight) const;
		bool AdjustScaledCoords (Metric *xSrc, Metric *ySrc, int cxSrc, int cySrc,
								 Metric xSrcInc, Metric ySrcInc,
								 int *xDest, int *yDest,
								 int *cxDest, int *cyDest);

		inline const RECT &GetClipRect (void) const { return m_rcClip; }
		inline int GetHeight (void) const { return m_cyHeight; }
		inline int GetWidth (void) const { return m_cxWidth; }
		void ResetClipRect (void);
		void SetClipRect (const RECT &rcClip);

	protected:
		int m_cxWidth;
		int m_cyHeight;

		RECT m_rcClip;
	};

#include "DXImage32.h"
#include "DXImageReal.h"
#include "DXImage16.h"
#include "DXImage8.h"

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

		inline float GetAt (SNoisePos &x, SNoisePos &y);
		inline void Next (SNoisePos &i) { if (++i.iFraction == m_iScale) { i.iInteger++; i.iFraction = 0; } }
		inline void Reset (SNoisePos &i, int iPos) { i.iInteger = iPos / m_iScale; i.iFraction = iPos % m_iScale; }

	private:
		int m_iScale;
		float *m_Frac;
		float *m_InvFrac;
		float *m_Smooth;
	};

float Noise2D (int x, float fx0, float fx1, float wx, int y, float fy0, float fy1, float wy);
float Noise3D (int x, float fx0, float fx1, float wx, int y, float fy0, float fy1, float wy, int z, float fz0, float fz1, float wz);
void NoiseInit (DWORD dwSeed = 0);
void NoiseReinit (DWORD dwSeed = 0);
float NoiseSmoothStep (float x);

inline float CNoiseGenerator::GetAt (SNoisePos &x, SNoisePos &y)
	{ return Noise2D(x.iInteger, m_Frac[x.iFraction], m_InvFrac[x.iFraction], m_Smooth[x.iFraction], y.iInteger, m_Frac[y.iFraction], m_InvFrac[y.iFraction], m_Smooth[y.iFraction]); }

#include "TextFormat.h"
#include "DXUI.h"

//	Utility Functions ---------------------------------------------------------

bool dxLoadImageFile (const CString &sFilespec, HBITMAP *rethDIB, EBitmapTypes *retiType = NULL, CString *retsError = NULL);

//	Screen Manager ------------------------------------------------------------

struct SScreenMgrOptions
	{
	SScreenMgrOptions (void) :
			m_hWnd(NULL),
			m_bWindowedMode(false),
			m_bMultiMonitorMode(false),
			m_bForceDX(false),
			m_bForceNonDX(false),
			m_bForceExclusiveMode(false),
			m_bForceNonExclusiveMode(false),
			m_bForceScreenSize(false),
			m_cxScreen(1024),
			m_cyScreen(768),
			m_iColorDepth(16),
			m_bDebugVideo(false)
		{ }

	HWND m_hWnd;

	bool m_bWindowedMode;
	bool m_bMultiMonitorMode;			//	If TRUE, screen spans all monitors

	bool m_bForceDX;
	bool m_bForceNonDX;
	bool m_bForceExclusiveMode;
	bool m_bForceNonExclusiveMode;
	bool m_bForceScreenSize;			//	If in exclusive mode, switch to desired res

	int m_cxScreen;						//	Used if exclusive or windowed
	int m_cyScreen;						//	Used if exclusive or windowed
	int m_iColorDepth;					//	Used if exclusive

	bool m_bDebugVideo;
	};

class CScreenMgr
	{
	public:
		CScreenMgr (void);
		~CScreenMgr (void);

		void Blt (void);
		bool CheckIsReady (void);
		void CleanUp (void);
		void ClientToScreen (int x, int y, int *retx, int *rety);
		inline void Flip (void) { if (m_bExclusiveMode && m_pDD) FlipInt(); }
		inline bool GetInvalidRect (RECT *retrcRect) { *retrcRect = m_rcInvalid; return (::IsRectEmpty(&m_rcInvalid) != TRUE); }
		inline CG32bitImage &GetScreen (void) { return m_Screen; }
		void GlobalToLocal (int x, int y, int *retx, int *rety);
		ALERROR Init (SScreenMgrOptions &Options, CString *retsError);
		void Invalidate (void) { SetInvalidAll(&m_rcInvalid); SetInvalidAll(&m_rcInvalidSecondary); }
		void Invalidate (const RECT &rcRect) { ::UnionRect(&m_rcInvalid, &m_rcInvalid, &rcRect); ::UnionRect(&m_rcInvalidSecondary, &m_rcInvalidSecondary, &rcRect); }
		inline bool IsMinimized (void) const { return m_bMinimized; }
		inline bool IsScreenPersistent (void) { return !m_bBackgroundBlt; }
		void LocalToGlobal (int x, int y, int *retx, int *rety);
		void OnWMActivateApp (bool bActivate);
		void OnWMDisplayChange (int iBitDepth, int cxWidth, int cyHeight);
		void OnWMMove (int x, int y);
		void OnWMSize (int cxWidth, int cyHeight, int iSize);
		void ScreenToClient (int x, int y, int *retx, int *rety);
		void StopDX (void);
		void Validate (void) { SetInvalidNone(&m_rcInvalid); }

		void BltToPrimary (CG32bitImage &Image);
		void BltToScreen (CG32bitImage &Image);

	private:
		void BackgroundCleanUp (void);
		void BackgroundInit (void);
		static DWORD WINAPI BackgroundThread (LPVOID pData);

		void DebugOutputStats (void);
		void FlipInt (void);
		void SetInvalidAll (RECT *rcRect) { rcRect->left = 0; rcRect->top = 0; rcRect->right = m_cxScreen; rcRect->bottom = m_cyScreen; }
		void SetInvalidNone (RECT *rcRect) { rcRect->left = 0; rcRect->top = 0; rcRect->right = 0; rcRect->bottom = 0; }

		HWND m_hWnd;

		CG32bitImage m_Screen;			//	This is the image that everyone will write to
		CG32bitImage m_Secondary;		//	This is used to swap, when doing background blts

		RECT m_rcInvalid;				//	Current invalid area (relative to m_Screen)
		RECT m_rcInvalidSecondary;		//	Invalid area of secondary screen

		RECT m_rcScreen;				//	RECT of screen area relative to window client coordinates
		int m_cxScreen;					//	Width of screen area in pixels (if windowed, this is client area)
		int m_cyScreen;					//	Height of screen area in pixels

		bool m_bDX;						//	TRUE if we're using DX
		bool m_bExclusiveMode;			//	TRUE if we're in exclusive mode
		bool m_bWindowedMode;			//	TRUE if we're in a window (as opposed to full screen)
		LPDIRECTDRAW7 m_pDD;
		LPDIRECTDRAWSURFACE7 m_pPrimary;
		SurfaceTypes m_PrimaryType;
		LPDIRECTDRAWSURFACE7 m_pBack;
		LPDIRECTDRAWSURFACE7 m_pCurrent;
		LPDIRECTDRAWCLIPPER m_pClipper;
		bool m_bDXReady;				//	TRUE if DX initialized
		int m_iColorDepth;				//	Color depth (if exclusive)
		bool m_bMinimized;				//	TRUE if we're minimized

		bool m_bBackgroundBlt;			//	TRUE if background blt is enabled
		HANDLE m_hBackgroundThread;		//	Background thread
		HANDLE m_hWorkEvent;			//	Blt
		HANDLE m_hQuitEvent;			//	Time to quit

		bool m_bDebugVideo;				//	TRUE if we're in debug video mode
	};

//	Inlines

inline void AGArea::Invalidate (const RECT &rcInvalid) { if (m_pScreen) m_pScreen->Invalidate(rcInvalid); }

//	Functions

void *SurfaceLock (LPDIRECTDRAWSURFACE7 pSurface, DDSURFACEDESC2 *retpDesc);
void SurfaceUnlock (LPDIRECTDRAWSURFACE7 pSurface);

void DebugOutputPixelFormat (DDPIXELFORMAT &format);
void DebugOutputSurfaceDesc (DDSURFACEDESC2 &surfacedesc);

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
		void Play (int iChannel, int iVolume, int iPan);
		inline void SetWaveVolume (int iVolumeLevel) { m_iSoundVolume = iVolumeLevel; }

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
			SChannel *pNext;
			};

		void AddMusicFolder (const CString &sFolder, TArray<CString> *retCatalog);
		int AllocChannel (void);
		void CleanUpChannel (SChannel &Channel);
		inline SChannel *GetChannel (int iChannel) { return &m_Channels[iChannel]; }
		inline int GetChannelCount (void) { return m_Channels.GetCount(); }
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

		float GetFrameRate (void);
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

#endif