//	DXScreenMgr.h
//
//	Implements CScreenMgr class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

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
			m_bNoGPUAcceleration(false),
			m_cyMaxScreen(-1),
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
	bool m_bNoGPUAcceleration;

	int m_cyMaxScreen;					//	Scales screen if we're bigger than this (-1 = no scaling)
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
		void ClientToLocal (int x, int y, int *retx, int *rety);
		inline void Flip (void) { if (m_bExclusiveMode && m_pDD) FlipInt(); }
		inline int GetHeight (void) const { return m_cyScreen; }
		inline bool GetInvalidRect (RECT *retrcRect) { *retrcRect = m_rcInvalid; return (::IsRectEmpty(&m_rcInvalid) != TRUE); }
		inline CG32bitImage &GetScreen (void) { return m_Screen; }
		inline int GetWidth (void) const { return m_cxScreen; }
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

		Metric m_rScale;				//	Downsample resolution for super hi-res screens (1.0 = normal)
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

		CCriticalSection m_cs;
		bool m_bBackgroundBlt;			//	TRUE if background blt is enabled
		HANDLE m_hBackgroundThread;		//	Background thread
		HANDLE m_hWorkEvent;			//	Blt
		HANDLE m_hQuitEvent;			//	Time to quit

		bool m_bDebugVideo;				//	TRUE if we're in debug video mode
	};

//	Functions

void *SurfaceLock (LPDIRECTDRAWSURFACE7 pSurface, DDSURFACEDESC2 *retpDesc);
void SurfaceUnlock (LPDIRECTDRAWSURFACE7 pSurface);

void DebugOutputPixelFormat (DDPIXELFORMAT &format);
void DebugOutputSurfaceDesc (DDSURFACEDESC2 &surfacedesc);
