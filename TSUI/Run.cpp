//	CHumanInterface.cpp
//
//	CHumanInterface class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

const DWORD ID_BACKGROUND_PROCESSOR =				1;
const DWORD ID_LOW_PRIORITY_BACKGROUND_PROCESSOR =	2;

const int MAX_SCREEN_HEIGHT =						1080;

const int TIMER_RESOLUTION =						1;
const int TICKS_PER_SECOND =						30;
const int FRAME_DELAY =								(1000 / TICKS_PER_SECOND);		//	Delay in mSecs

void CHumanInterface::Run (IHIController *pController, HINSTANCE hInst, int nCmdShow, LPSTR lpCmdLine)

//	Run
//
//	Runs the app. Note that we take ownership of the controller.

	{
	ALERROR error;
	CString sError;

	ASSERT(pController);

	//	Make sure we have HI

	Create();

	//	Boot the controller and get options from it.

	g_pHI->m_pController = pController;
	if (error = pController->HIBoot(lpCmdLine, &g_pHI->m_Options, &sError))
		{
		if (error != ERR_CANCEL)
			::MessageBox(NULL, sError.GetASCIIZPointer(), g_pHI->m_Options.sAppName.GetASCIIZPointer(), MB_OK);

		delete g_pHI->m_pController;
		g_pHI->m_pController = NULL;
		return;
		}

	//	Create the main window

	if (!g_pHI->CreateMainWindow(hInst, nCmdShow, lpCmdLine, &sError))
		{
		::MessageBox(NULL, sError.GetASCIIZPointer(), g_pHI->m_Options.sAppName.GetASCIIZPointer(), MB_OK);
		::DestroyWindow(g_pHI->m_hWnd);
		return;
		}

	//	Initialize controller (Note: At this point, we need to be
	//	fully initialized because the controller is free to call us back
	//	to set up sessions, tasks, etc.)

	if (g_pHI->m_pController->HIInit(&sError) != NOERROR)
		{
		::MessageBox(g_pHI->m_hWnd, sError.GetASCIIZPointer(), g_pHI->m_Options.sAppName.GetASCIIZPointer(), MB_OK);
		::DestroyWindow(g_pHI->m_hWnd);
		return;
		}

	//	Event loop until we quit.

	g_pHI->MainLoop();

	//	Done

	Destroy();
	}

bool CHumanInterface::CreateMainWindow (HINSTANCE hInst, int nCmdShow, LPSTR lpCmdLine, CString *retsError)

//	CreateMainWindow
//
//	Creates the main application window.

	{
	CString sError;
    WNDCLASSEX wc;

	//	Register window

	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize = sizeof(wc);
	wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = MainWndProc;
    wc.hInstance = hInst;
	wc.hIcon = m_Options.hIcon;
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszClassName = m_Options.sClassName.GetASCIIZPointer();
	if (!RegisterClassEx(&wc))
		return ERR_FAIL;

	//	Create the window
	//
	//	NOTE: Inside WM_CREATE we set m_hWnd to the window. But since we don't
	//	know if we got that far, we use the return value of CreateWindow to
	//	check for errors.

    HWND hWnd = ::CreateWindowEx(0,	// WS_EX_TOPMOST,
			m_Options.sClassName.GetASCIIZPointer(), 
			m_Options.sAppName.GetASCIIZPointer(),
			WS_POPUP,
			0,
			0, 
			GetSystemMetrics(SM_CXSCREEN),
			GetSystemMetrics(SM_CYSCREEN), 
			NULL,
			NULL,
			hInst,
			&sError);

	//	If WM_CREATE set an error, then we abort

	if (!sError.IsBlank())
		{
		if (retsError) *retsError = sError;
		return false;
		}

	//	If creation itself failed, then we abort.

	else if (hWnd == NULL)
		{
		if (retsError) *retsError = CONSTLIT("Unable to create window.");
		return false;
		}

	//	Show the window

	if (m_Options.m_bWindowedMode)
		ShowWindow(hWnd, nCmdShow);
	else
		ShowWindow(hWnd, SW_SHOWMAXIMIZED);

    UpdateWindow(hWnd);

	//	Done

	return true;
	}

void CHumanInterface::MainLoop (void)

//	MainLoop
//
//	Main application loop

	{
	MSG msg;

	timeBeginPeriod(TIMER_RESOLUTION);

	//	Start main loop

	DWORD dwStartTime = timeGetTime();

	while (TRUE)
		{
		//	Tell the main window that we're animating

		OnAnimate();

		//	Process all events

		while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE|PM_NOYIELD))
			{
			BOOL bHandled = FALSE;

			//	Check to see if we're quitting

			if (msg.message == WM_QUIT)
				return;

			//	If we haven't handled it yet, handle it now

			if (!bHandled)
				{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
				}
			}

		//	Figure out how long until our next animation

		DWORD dwNow = timeGetTime();
		DWORD dwNextFrame = dwStartTime + FRAME_DELAY;

		//	Wait

		if (dwNextFrame > dwNow)
			{
			::Sleep(dwNextFrame - dwNow);

			dwStartTime = dwNextFrame;
			}
		else
			dwStartTime = dwNow;
		}

	timeEndPeriod(TIMER_RESOLUTION);
	}

LONG APIENTRY CHumanInterface::MainWndProc (HWND hWnd, UINT message, UINT wParam, LONG lParam)

//	MainWndProc
//
//	Main window proc

	{
	switch (message)
		{
		case WM_ACTIVATEAPP:
			return g_pHI->WMActivateApp(wParam ? true : false);

		case WM_CHAR:
			return g_pHI->WMChar((char)wParam, (DWORD)lParam);

		case WM_CLOSE:
			return g_pHI->WMClose();

		case WM_CREATE:
			{
			LPCREATESTRUCT pCreate = (LPCREATESTRUCT)lParam;
			CString *retsError = (CString *)pCreate->lpCreateParams;

			if (!g_pHI->WMCreate(hWnd, retsError))
				//	NOTE: We don't return -1 on failure because we still want to 
				//	be able to show a message box on the window.
				return 0;

			return 0;
			}

		case WM_DESTROY:
			{
			long iResult = 0;

			//	Clean up window

			try
				{
				if (g_pHI)
					g_pHI->WMDestroy();
				}
			catch (...)
				{
				}

			//	Quit the app 
			//	
			//	NOTE: We do this last because we want to wait until all
			//	windows are done processing. In particular, MCI playback windows
			//	need to process messages while quitting.

			::PostQuitMessage(0);

			return iResult;
			}

		case WM_DISPLAYCHANGE:
			return g_pHI->WMDisplayChange((int)wParam, (int)LOWORD(lParam), (int)HIWORD(lParam));

		case WM_HI_COMMAND:
			g_pHI->OnPostCommand(lParam);
			return 0;

		case WM_HI_TASK_COMPLETE:
			g_pHI->OnTaskComplete(wParam, lParam);
			return 0;

		case WM_KEYDOWN:
			return g_pHI->WMKeyDown((int)wParam, lParam);

		case WM_KEYUP:
			return (g_pHI ? g_pHI->WMKeyUp((int)wParam, lParam) : 0);

		case WM_LBUTTONDBLCLK:
			return g_pHI->WMLButtonDblClick((int)LOWORD(lParam), (int)HIWORD(lParam), wParam);

		case WM_LBUTTONDOWN:
			return g_pHI->WMLButtonDown((int)LOWORD(lParam), (int)HIWORD(lParam), wParam);

		case WM_LBUTTONUP:
			return (g_pHI ? g_pHI->WMLButtonUp((int)LOWORD(lParam), (int)HIWORD(lParam), wParam) : 0);

		case WM_MOUSEMOVE:
			return g_pHI->WMMouseMove((int)LOWORD(lParam), (int)HIWORD(lParam), wParam);

		case WM_MOUSEWHEEL:
			return g_pHI->WMMouseWheel((int)(short)HIWORD(wParam), (int)LOWORD(lParam), (int)HIWORD(lParam), (DWORD)LOWORD(wParam));

		case WM_MOVE:
			return g_pHI->WMMove((int)LOWORD(lParam), (int)HIWORD(lParam));

		case WM_PAINT:
			{
			PAINTSTRUCT ps;
			HDC hDC = ::BeginPaint(hWnd, &ps);

			::EndPaint(hWnd, &ps);
			return 0;
			}

		case WM_RBUTTONDBLCLK:
			return g_pHI->WMRButtonDblClick((int)LOWORD(lParam), (int)HIWORD(lParam), wParam);

		case WM_RBUTTONDOWN:
			return g_pHI->WMRButtonDown((int)LOWORD(lParam), (int)HIWORD(lParam), wParam);

		case WM_RBUTTONUP:
			return (g_pHI ? g_pHI->WMRButtonUp((int)LOWORD(lParam), (int)HIWORD(lParam), wParam) : 0);

		case WM_SIZE:
			return g_pHI->WMSize((int)LOWORD(lParam), (int)HIWORD(lParam), (int)wParam);

		case WM_TIMER:
			return g_pHI->WMTimer((DWORD)wParam);

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}

LONG CHumanInterface::WMActivateApp (bool bActivate)

//	WMActivateApp
//
//	Handle WM_ACTIVATEAPP message

	{
	m_ScreenMgr.OnWMActivateApp(bActivate);

	return 0;
	}

LONG CHumanInterface::WMChar (char chChar, DWORD dwKeyData)

//	WMChar
//
//	Handle WM_CHAR message

	{
	//	We may need to ignore a character

	if (m_chKeyDown && (m_chKeyDown == chChar || m_chKeyDown == (chChar - ('a' - 'A'))))
		{
		m_chKeyDown = '\0';
		return 0;
		}
	else
		m_chKeyDown = '\0';

	//	Pass to session

	if (m_pCurSession)
		m_pCurSession->HIChar(chChar, dwKeyData);

	return 0;
	}

LONG CHumanInterface::WMClose (void)

//	WMClose
//
//	Handle WM_CLOSE message

	{
	if (m_pController && !m_pController->HIClose())
		return 0;

	Shutdown(HIShutdownByUser);
	return 0;
	}

bool CHumanInterface::WMCreate (HWND hWnd, CString *retsError)

//	WMCreate
//
//	Initializes the class.
//
//	pController will be owned by this object and will be freed when on WMDestroy

	{
	//	Init

	ASSERT(hWnd);
	m_hWnd = hWnd;

	//	Always start with a hidden cursor. Sessions will show it if necessary.
	//	(Remember that the Windows cursor is refcounted. Each call to 
	//	ShowCursor(true) must be paired with a ShowCursor(false).

	::ShowCursor(false);

	//	Initialize sound manager

	if (m_SoundMgr.Init(m_hWnd) != NOERROR)
		{
		//	Report error, but don't abort

		kernelDebugLogMessage("Unable to initialize sound manager.");
		}

	m_SoundMgr.SetWaveVolume(m_Options.m_iSoundVolume);

	//	Initialize screen

	SScreenMgrOptions ScreenOptions;
	ScreenOptions.m_hWnd = m_hWnd;
	ScreenOptions.m_cyMaxScreen = m_Options.m_cyMaxScreen;
	ScreenOptions.m_cxScreen = m_Options.m_cxScreenDesired;
	ScreenOptions.m_cyScreen = m_Options.m_cyScreenDesired;
	ScreenOptions.m_iColorDepth = m_Options.m_iColorDepthDesired;
	ScreenOptions.m_bWindowedMode = m_Options.m_bWindowedMode;
	ScreenOptions.m_bMultiMonitorMode = m_Options.m_bMultiMonitorMode;
	ScreenOptions.m_bForceDX = m_Options.m_bForceDX;
	ScreenOptions.m_bForceNonDX = m_Options.m_bForceNonDX;
	ScreenOptions.m_bForceExclusiveMode = m_Options.m_bForceExclusiveMode;
	ScreenOptions.m_bForceNonExclusiveMode = m_Options.m_bForceNonExclusiveMode;
	ScreenOptions.m_bForceScreenSize = m_Options.m_bForceScreenSize;
	ScreenOptions.m_bDebugVideo = m_Options.m_bDebugVideo;

	if (m_ScreenMgr.Init(ScreenOptions, retsError) != NOERROR)
		return false;

	//	Initialize the background processors

	if (m_Background.Init(m_hWnd, ID_BACKGROUND_PROCESSOR) != NOERROR)
		{
		if (retsError) *retsError = CONSTLIT("Unable to initialize background processor.");
		return false;
		}

	if (m_BackgroundLowPriority.Init(m_hWnd, ID_LOW_PRIORITY_BACKGROUND_PROCESSOR) != NOERROR)
		{
		if (retsError) *retsError = CONSTLIT("Unable to initialize background processor.");
		return false;
		}

	//	Initialize our visual styles. It's OK to pass NULL for hModule since we
	//	always expect to load resources from the current process.

	if (m_Visuals.Init(NULL, retsError) != NOERROR)
		return false;

	//	Done

	return true;
	}

void CHumanInterface::WMDestroy (void)

//	WMDestroy
//
//	Shut down

	{
	//	If we don't know why we're shutting down, then it was probably
	//	because of the user

	if (m_iShutdownCode == HIShutdownUnknown)
		m_iShutdownCode = HIShutdownByUser;

	//	Clean up

	CleanUp(m_iShutdownCode);
	}

LONG CHumanInterface::WMDisplayChange (int iBitDepth, int cxWidth, int cyHeight)

//	WMDisplayChange
//
//	Handle WM_DISPLAYCHANGE message

	{
	m_ScreenMgr.OnWMDisplayChange(iBitDepth, cxWidth, cyHeight);

	return 0;
	}

LONG CHumanInterface::WMKeyDown (int iVirtKey, DWORD dwKeyData)

//	WMKeyDown
//
//	Handle WM_KEYDOWN message

	{
	if (m_pCurSession)
		{
		//	Remember our current session

		IHISession *pOldSession = m_pCurSession;

		//	Pass it to the session

		m_pCurSession->HIKeyDown(iVirtKey, dwKeyData);

		//	If we're dead, then exit

		if (g_pHI == NULL)
			return 0;

		//	If the session changed, then we need to ignore a
		//	subsequent WM_CHAR

		if (m_pCurSession != pOldSession)
			m_chKeyDown = iVirtKey;
		}

	return 0;
	}

LONG CHumanInterface::WMKeyUp (int iVirtKey, DWORD dwKeyData)

//	WMKeyUp
//
//	Handle WM_KEYUP message

	{
	if (m_pCurSession)
		m_pCurSession->HIKeyUp(iVirtKey, dwKeyData);

	return 0;
	}

LONG CHumanInterface::WMLButtonDblClick (int x, int y, DWORD dwFlags)

//	WMLButtonDblClick
//
//	Handle WM_LBUTTONDBLCLICK message

	{
	if (m_pCurSession)
		{
		int xLocal, yLocal;

		m_ScreenMgr.ClientToScreen(x, y, &xLocal, &yLocal);
		m_pCurSession->HILButtonDblClick(xLocal, yLocal, dwFlags);
		}

	return 0;
	}
LONG CHumanInterface::WMLButtonDown (int x, int y, DWORD dwFlags)

//	WMLButtonDown
//
//	Handle WM_LBUTTONDOWN message

	{
	if (m_pCurSession)
		{
		int xLocal, yLocal;

		m_ScreenMgr.ClientToScreen(x, y, &xLocal, &yLocal);
		m_pCurSession->HILButtonDown(xLocal, yLocal, dwFlags);
		}

	return 0;
	}

LONG CHumanInterface::WMLButtonUp (int x, int y, DWORD dwFlags)

//	WMLButtonUp
//
//	Handle WM_LBUTTONUP message

	{
	if (m_pCurSession)
		{
		int xLocal, yLocal;

		m_ScreenMgr.ClientToScreen(x, y, &xLocal, &yLocal);
		m_pCurSession->HILButtonUp(xLocal, yLocal, dwFlags);
		}

	return 0;
	}

LONG CHumanInterface::WMMouseMove (int x, int y, DWORD dwFlags)

//	WMMouseMove
//
//	Handle WM_MOUSEMOVE message

	{
	if (m_pCurSession)
		{
		int xLocal, yLocal;

		m_ScreenMgr.ClientToScreen(x, y, &xLocal, &yLocal);
		m_pCurSession->HIMouseMove(xLocal, yLocal, dwFlags);
		}

	return 0;
	}

LONG CHumanInterface::WMMouseWheel (int iDelta, int x, int y, DWORD dwFlags)

//	WMMouseWheel
//
//	Handle WM_MOUSEWHEEL message

	{
	if (m_pCurSession)
		{
		int xLocal, yLocal;

		m_ScreenMgr.ClientToScreen(x, y, &xLocal, &yLocal);
		m_pCurSession->HIMouseWheel(iDelta, xLocal, yLocal, dwFlags);
		}

	return 0;
	}

LONG CHumanInterface::WMMove (int x, int y)

//	WMMove
//
//	Handle WM_MOVE

	{
	m_ScreenMgr.OnWMMove(x, y);

	if (m_pCurSession)
		m_pCurSession->HISize(GetScreen().GetWidth(), GetScreen().GetHeight());

	return 0;
	}

LONG CHumanInterface::WMRButtonDblClick (int x, int y, DWORD dwFlags)

//	WMRButtonDblClick
//
//	Handle WM_RBUTTONDBLCLICK message

	{
	if (m_pCurSession)
		{
		int xLocal, yLocal;

		m_ScreenMgr.ClientToScreen(x, y, &xLocal, &yLocal);
		m_pCurSession->HIRButtonDblClick(xLocal, yLocal, dwFlags);
		}

	return 0;
	}
LONG CHumanInterface::WMRButtonDown (int x, int y, DWORD dwFlags)

//	WMRButtonDown
//
//	Handle WM_RBUTTONDOWN message

	{
	if (m_pCurSession)
		{
		int xLocal, yLocal;

		m_ScreenMgr.ClientToScreen(x, y, &xLocal, &yLocal);
		m_pCurSession->HIRButtonDown(xLocal, yLocal, dwFlags);
		}

	return 0;
	}

LONG CHumanInterface::WMRButtonUp (int x, int y, DWORD dwFlags)

//	WMRButtonUp
//
//	Handle WM_RBUTTONUP message

	{
	if (m_pCurSession)
		{
		int xLocal, yLocal;

		m_ScreenMgr.ClientToScreen(x, y, &xLocal, &yLocal);
		m_pCurSession->HIRButtonUp(xLocal, yLocal, dwFlags);
		}

	return 0;
	}

LONG CHumanInterface::WMSize (int cxWidth, int cyHeight, int iSize)

//	WMSize
//
//	Handle WM_SIZE

	{
	m_ScreenMgr.OnWMSize(cxWidth, cyHeight, iSize);

	if (m_pCurSession)
		m_pCurSession->HISize(GetScreen().GetWidth(), GetScreen().GetHeight());

	return 0;
	}

LONG CHumanInterface::WMTimer (DWORD dwID)

//	WMTimer
//
//	Handle WM_TIMER

	{
	m_Timers.FireTimer(m_hWnd, dwID);

	return 0;
	}
