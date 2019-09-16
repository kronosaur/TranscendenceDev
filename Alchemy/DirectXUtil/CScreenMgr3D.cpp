//	CScreenMgr3D.cpp
//
//	CScreenMgr3D class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

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
	return m_DX.IsReady();
	}

void CScreenMgr3D::CleanUp (void)

//	CleanUp
//
//	Clean up the screen

	{
	m_Blitter.CleanUp();
	m_DX.CleanUp();
	}

void CScreenMgr3D::ClientToLocal (int x, int y, int *retx, int *rety) const

//	ClientToLocal
//
//	Convert from client to (local) screen manager coordinates

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

void CScreenMgr3D::DebugOutputStats (void)

//	DebugOutputStats
//
//	Output video status

	{
	::kernelDebugLogPattern("Direct3D");
	::kernelDebugLogPattern("Screen: %d x %d%s", m_cxScreen, m_cyScreen, (m_bWindowedMode ? CONSTLIT(" (windowed)") : NULL_STR));
	::kernelDebugLogPattern("Scale: %d.%02d", (int)m_rScale, (int)(m_rScale * 100.0) % 100);
	::kernelDebugLogPattern("Background blt: %s", (m_Blitter.IsEnabled() ? CONSTLIT("Enabled") : CONSTLIT("Disabled")));
	::kernelDebugLogPattern("Use Direct X: %s", (m_DX.IsUsingDirectX() ? CONSTLIT("Enabled") : CONSTLIT("Disabled")));
	::kernelDebugLogPattern("Use OpenGL: %s", (m_DX.IsUsingOpenGL() ? CONSTLIT("Enabled") : CONSTLIT("Disabled")));
	::kernelDebugLogPattern("Use GPU compositing: %s", (m_DX.IsUsingTextures() ? CONSTLIT("Enabled") : CONSTLIT("Disabled")));

	HDC hDC = ::GetDC(m_hWnd);
	kernelDebugLogPattern("LogPixelsY: %d", ::GetDeviceCaps(hDC, LOGPIXELSY));
	::ReleaseDC(m_hWnd, hDC);

	m_DX.DebugOutputStats();
	}

void CScreenMgr3D::Flip (void)

//	Flip
//
//	Flip between the two buffers

	{
	}

void CScreenMgr3D::GlobalToLocal (int x, int y, int *retx, int *rety) const

//	GlobalToLocal
//
//	Converts global (screen) coordinates to local (screen manager)

	{
	POINT pt;
	pt.x = x;
	pt.y = y;

	::ScreenToClient(m_hWnd, &pt);

	ClientToLocal(pt.x, pt.y, retx, rety);
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

			m_cxScreen = mathRound(m_rScale * m_cxScreen);
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

	DWORD dwFlags = 0;
	dwFlags |= (Options.m_bNoGPUAcceleration ? CDXScreen::FLAG_NO_TEXTURES : 0);
	dwFlags |= (Options.m_bForceNonDX ? CDXScreen::FLAG_FORCE_GDI : 0);
	dwFlags |= (Options.m_bForceOpenGL ? CDXScreen::FLAG_FORCE_OPENGL : 0);

	if (!m_DX.Init(m_hWnd, m_cxScreen, m_cyScreen, dwFlags, retsError))
		return ERR_FAIL;

	//	Initialize the background blitter

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

	//	Debug stats for troubleshooting

	DebugOutputStats();

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

void CScreenMgr3D::LocalToClient (int x, int y, int *retx, int *rety) const

//	LocalToClient
//
//	Converts local (screen manager) to client coordinates

	{
	if (m_rScale != 1.0)
		{
		x = (int)(x / m_rScale);
		y = (int)(y / m_rScale);
		}

	if (m_bWindowedMode)
		{
		RECT rcClient;
		::GetClientRect(m_hWnd, &rcClient);

		int cxClient = RectWidth(rcClient);
		int cyClient = RectHeight(rcClient);

		//	In DX we stretch the screen to fit into the client area.

		if (m_cxScreen > 0 && m_cyScreen > 0)
			{
			*retx = cxClient * x / m_cxScreen;
			*rety = cxClient * y / m_cyScreen;
			}
		else
			{
			*retx = x;
			*rety = y;
			}
		}
	else
		{
		*retx = x;
		*rety = y;
		}
	}

void CScreenMgr3D::LocalToGlobal (int x, int y, int *retx, int *rety) const

//	LocalToGlobal
//
//	Converts local (screen manager) to global (screen) coordinates.

	{
	POINT pt;
	LocalToClient(x, y, (int *)&pt.x, (int *)&pt.y);

	::ClientToScreen(m_hWnd, &pt);

	*retx = (int)pt.x;
	*rety = (int)pt.y;
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
	m_Blitter.Resize(cxWidth, cyHeight);
	}

void CScreenMgr3D::Render (void)

//	Render
//
//	Blt the current image to the screen

	{
	DEBUG_TRY

	m_Blitter.Render();

	DEBUG_CATCH
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
