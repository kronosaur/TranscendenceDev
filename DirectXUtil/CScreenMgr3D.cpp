//	CScreenMgr3D.cpp
//
//	CScreenMgr3D class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include <windows.h>
#include <dsound.h>
#include "Alchemy.h"
#include "DirectXUtil.h"
#include "DX3DImpl.h"

CScreenMgr3D::CScreenMgr3D (void) :
		m_hWnd(NULL),
		m_bWindowedMode(false),
		m_bMultiMonitorMode(false),
		m_bMinimized(false),
		m_bDebugVideo(false),
		m_rScale(1.0),
		m_Blitter(m_DX),
		m_iDefaultLayer(-1)

//	CScreenMgr3D constructor

	{
	}

CScreenMgr3D::~CScreenMgr3D (void)

//	CScreenMgr3D destructor

	{
	CleanUp();
	}

bool CScreenMgr3D::CheckIsReady (void)

//	CheckIsReady
//
//	Returns TRUE if we are ready

	{
	return true;
	}

void CScreenMgr3D::CleanUp (void)

//	CleanUp
//
//	Clean up the screen

	{
	m_Blitter.CleanUp();
	m_DX.CleanUp();
	}

void CScreenMgr3D::ClientToScreen (int x, int y, int *retx, int *rety)

//	ClientToScreen
//
//	Convert from client to screen coordinates

	{
	if (m_rScale != 1.0)
		{
		x = (int)(x * m_rScale);
		y = (int)(y * m_rScale);
		}

	if (m_bWindowedMode)
		{
		RECT rcClient;
		::GetClientRect(m_hWnd, &rcClient);

		int cxClient = RectWidth(rcClient);
		int cyClient = RectHeight(rcClient);

		//	In DX we stretch the screen to fit into the client area.

		if (cxClient > 0 && cyClient > 0)
			{
			*retx = m_cxScreen * x / cxClient;
			*rety = m_cyScreen * y / cyClient;
			}
		else
			{
			*retx = x;
			*rety = y;
			}
		}
	else
		{
		//	Compensate for the fact that we center the screen rect
		//	in the window client area.

		*retx = x;
		*rety = y;
		}
	}

void CScreenMgr3D::Flip (void)

//	Flip
//
//	Flip between the two buffers

	{
	}

ALERROR CScreenMgr3D::Init (SScreenMgrOptions &Options, CString *retsError)

//	Init
//
//	Initialize screen.

	{
	//	Some options

	m_hWnd = Options.m_hWnd;
	m_bWindowedMode = Options.m_bWindowedMode;
	m_bMultiMonitorMode = Options.m_bMultiMonitorMode;
	m_bMinimized = false;
	m_bDebugVideo = Options.m_bDebugVideo;

	//	Compute the size of the screen buffer.
	//
	//	If we're in windowed mode or if we're forcing a specific size, then take
	//	the size passed in.

	if (m_bWindowedMode || Options.m_bForceScreenSize)
		{
		m_cxScreen = Options.m_cxScreen;
		m_cyScreen = Options.m_cyScreen;
		m_bMultiMonitorMode = false;
		m_rScale = 1.0;
		}

	//	If we want multiple monitors, then use all the space.

	else if (m_bMultiMonitorMode)
		{
		m_cxScreen = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
		m_cyScreen = ::GetSystemMetrics(SM_CYVIRTUALSCREEN);
		m_rScale = 1.0;
		}

	//	Otherwise, full screen

	else
		{
		m_cxScreen = ::GetSystemMetrics(SM_CXSCREEN);
		m_cyScreen = ::GetSystemMetrics(SM_CYSCREEN);

		//	If the screen is bigger than 1920 x 1080, we scale everything
		//	(because the game is not really well-suited to super high-res.)

		if (Options.m_cyMaxScreen != -1 && m_cyScreen > Options.m_cyMaxScreen)
			{
			m_rScale = (Metric)Options.m_cyMaxScreen / m_cyScreen;

			m_cxScreen = (int)((m_rScale * m_cxScreen) + 0.5);
			m_cyScreen = Options.m_cyMaxScreen;
			}
		else
			m_rScale = 1.0;
		}

	//	In windowed mode, make sure we have the right styles
	//	(We don't do anything if the window is a child so that we can handle screen savers
	//	in preview window mode).

	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	if (!(dwStyle & WS_CHILD))
		{
		if (m_bWindowedMode)
			::SetWindowLong(m_hWnd, GWL_STYLE, (dwStyle & ~WS_POPUP) | WS_OVERLAPPEDWINDOW);
		else
			::SetWindowLong(m_hWnd, GWL_STYLE, (dwStyle & ~WS_OVERLAPPEDWINDOW) | WS_POPUP);
		}

	//	If in windowed mode, position the window properly

	if (m_bWindowedMode)
		{
		DWORD dwStyles = WS_OVERLAPPEDWINDOW;

		//	Compute how big the window needs to be (this grows the rect
		//	to account for title bar, etc.)

		RECT rcRect;
		rcRect.left = 0;
		rcRect.top = 0;
		rcRect.right = m_cxScreen;
		rcRect.bottom = m_cyScreen;
		::AdjustWindowRect(&rcRect, dwStyles, FALSE);

		//	Center the window on the screen

		uiGetCenteredWindowRect(RectWidth(rcRect), RectHeight(rcRect), &rcRect);

		//	Move/resize the window

		::MoveWindow(m_hWnd, rcRect.left, rcRect.top, RectWidth(rcRect), RectHeight(rcRect), FALSE);
		}

	//	Initialize DX

	if (!m_DX.Init(m_hWnd, m_cxScreen, m_cyScreen, retsError))
		return ERR_FAIL;

	//	Create an off-screen bitmap

	if (!m_Screen.Create(m_cxScreen, m_cyScreen))
		{
		if (retsError) *retsError = CONSTLIT("Unable to create off-screen bitmap.");
		CleanUp();
		return ERR_FAIL;
		}

	//	Initialize the blitter

	if (!m_Blitter.Init(m_cxScreen, m_cyScreen, retsError))
		{
		CleanUp();
		return ERR_FAIL;
		}

	//	Create a default background layer

	SDXLayerCreate NewLayer;
	NewLayer.cxWidth = m_cxScreen;
	NewLayer.cyHeight = m_cyScreen;
	if (!m_DX.CreateLayer(NewLayer, &m_iDefaultLayer, retsError))
		{
		CleanUp();
		return ERR_FAIL;
		}

	//	Done

	return NOERROR;
	}

void CScreenMgr3D::Invalidate (void)

//	Invalidate
//
//	Invalidate the entire screen

	{
	}

void CScreenMgr3D::Invalidate (const RECT &rcRect)

//	Invalidate
//
//	Invalidate a rect

	{
	}

void CScreenMgr3D::OnWMActivateApp (bool bActivate)

//	OnWMActivateApp
//
//	Handle WM_ACTIVATE_APP message

	{
	}

void CScreenMgr3D::OnWMDisplayChange (int iBitDepth, int cxWidth, int cyHeight)

//	OnWMDisplayChange
//
//	Handle WM_DISPLAY_CHANGE message

	{
	}

void CScreenMgr3D::OnWMMove (int x, int y)

//	OnWMMove
//
//	Handle WM_MOVE message

	{
	}

void CScreenMgr3D::OnWMSize (int cxWidth, int cyHeight, int iSize)

//	OnWMSize
//
//	Handle WM_SIZE message

	{
	}

void CScreenMgr3D::Render (void)

//	Render
//
//	Blt the current image to the screen

	{
	m_Blitter.Render();
	}

void CScreenMgr3D::StopDX (void)

//	StopDX
//
//	Stop DX mode

	{
	}

void CScreenMgr3D::Validate (void)

//	Validate
//
//	Validate the entire screen

	{
	}
