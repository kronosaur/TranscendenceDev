//	CHumanInterface.cpp
//
//	CHumanInterface class
//	Copyright (c) 2010 by George Moromisato. All Rights Reserved.

#include "stdafx.h"

#define CMD_SOUNDTRACK_DONE						CONSTLIT("cmdSoundtrackDone")

const DWORD ID_BACKGROUND_PROCESSOR =				1;
const DWORD ID_LOW_PRIORITY_BACKGROUND_PROCESSOR =	2;

static CReanimator g_DefaultReanimator;
CHumanInterface *g_pHI = NULL;

CHumanInterface::CHumanInterface (void) : 
		m_pController(NULL),
		m_pCurSession(NULL),
		m_hWnd(NULL),
        m_bLButtonDown(false),
        m_bRButtonDown(false),
		m_chKeyDown('\0'),
		m_iShutdownCode(HIShutdownUnknown)

//	CHumanInterface constructor

	{
	}

CHumanInterface::~CHumanInterface (void)

//	CHumanInterface destructor

	{
	CleanUp(HIShutdownDestructor);
	}

void CHumanInterface::AddBackgroundTask (IHITask *pTask, DWORD dwFlags, IHICommand *pListener, const CString &sCmd)

//	AddBackgroundTask
//
//	Adds the task

	{
	if (dwFlags & FLAG_LOW_PRIORITY)
		m_BackgroundLowPriority.AddTask(pTask, pListener, sCmd);
	else
		m_Background.AddTask(pTask, pListener, sCmd);
	}

DWORD CHumanInterface::AddTimer (DWORD dwMilliseconds, IHICommand *pListener, const CString &sCmd, bool bRecurring)

//	AddTimer
//
//	Adds a timer

	{
	return m_Timers.AddTimer(m_hWnd,
			dwMilliseconds,
			(pListener ? pListener : m_pController),
			(!sCmd.IsBlank() ? sCmd : CONSTLIT("cmdTimer")),
			bRecurring);
	}

void CHumanInterface::BeginSessionPaint (CG32bitImage &Screen)

//	BeginSessionPaint
//
//	Called in DefaultOnAnimate

	{
	}

void CHumanInterface::BeginSessionUpdate (void)

//	BeginSessionUpdate
//
//	Called in DefaultOnAnimate

	{
	}

void CHumanInterface::CalcBackgroundSessions (void)

//	CalcBackgroundSessions
//
//	Initializes the m_BackgroundSessions array.

	{
	int i;

	m_BackgroundSessions.DeleteAll();

	//	If the current session is fully opaque, then we don't need to paint
	//	any previous sessions.

	if (!m_pCurSession->IsTransparent())
		return;

	//	Make a list of all the sessions that we need to paint before we paint
	//	the current session (in paint order).

	for (i = m_SavedSessions.GetCount() - 1; i >= 0; i--)
		{
		//	Add the session to the list to paint; we add it at the beginning
		//	because previous sessions should paint on top of us.

		m_BackgroundSessions.Insert(m_SavedSessions[i], 0);

		//	If this session is fully opaque, then we're done. 

		if (!m_SavedSessions[i]->IsTransparent())
			return;
		}
	}

void CHumanInterface::CleanUp (EHIShutdownReasons iShutdownCode)

//	CleanUp
//
//	Clean up interface

	{
	int i;

	//	Wait for all background tasks to end

	m_Background.StopAll();
	m_BackgroundLowPriority.StopAll();

	//	Clean up session

	if (m_pCurSession)
		{
		m_pCurSession->HICleanUp();
		delete m_pCurSession;
		m_pCurSession = NULL;
		}

	//	Clean up any saved session

	for (i = 0; i < m_SavedSessions.GetCount(); i++)
		{
		m_SavedSessions[i]->HICleanUp();
		delete m_SavedSessions[i];
		}
	m_SavedSessions.DeleteAll();

	//	Clean up controller

	if (m_pController)
		{
		m_pController->HIShutdown(iShutdownCode);
		m_pController->HICleanUp();
		delete m_pController;
		m_pController = NULL;
		}

	m_Visuals.CleanUp();
	m_Background.CleanUp();
	m_BackgroundLowPriority.CleanUp();
	m_ScreenMgr.CleanUp();
	m_SoundMgr.CleanUp();
	}

void CHumanInterface::ClosePopupSession (void)

//	ClosePopupSession
//
//	Closes the current session and restore the most
//	recent saved session.

	{
	//	Close the current session

	if (m_pCurSession)
		{
		if (m_pCurSession->IsCursorShown())
			::ShowCursor(false);

		//	Remove the session from any background tasks and timers

		m_Background.ListenerDestroyed(m_pCurSession);
		m_BackgroundLowPriority.ListenerDestroyed(m_pCurSession);
		m_Timers.ListenerDestroyed(m_hWnd, m_pCurSession);

		//	Clean up

		m_pCurSession->HICleanUp();
		delete m_pCurSession;
		m_pCurSession = NULL;
		}

	//	Restore the previous session

	int iIndex = m_SavedSessions.GetCount() - 1;
	if (iIndex >= 0)
		{
		m_pCurSession = m_SavedSessions[iIndex];

		//	Send size message

		m_pCurSession->HISize(GetScreenWidth(), GetScreenHeight());

		//	Remove from saved array

		m_SavedSessions.Delete(iIndex);
		}

	//	Recompute paint order

	CalcBackgroundSessions();
	}

bool CHumanInterface::Create (void)

//	Create
//
//	Creates the global structure.

	{
	if (g_pHI == NULL)
		g_pHI = new CHumanInterface;

	return true;
	}

void CHumanInterface::Destroy (void)

//	Destroy
//
//	Destroys

	{
	if (g_pHI)
		{
		delete g_pHI;
		g_pHI = NULL;
		}
	}

void CHumanInterface::EndSessionPaint (CG32bitImage &Screen, bool bTopMost)

//	EndSessionPatin
//
//	Called in DefaultOnAnimate

	{
	//	If this is the top-most session, then do some housekeeping

	if (bTopMost)
		{
		//	Paint frame rate

		if (m_Options.m_bDebugVideo)
			{
			m_FrameRate.MarkFrame();
			PaintFrameRate();
			}

		//	Blt

		BltScreen();
		}
	}

void CHumanInterface::EndSessionUpdate (bool bTopMost)

//	EndSessionUpdate
//
//	Called in DefaultOnAnimate

	{
	//	If necessary, flip

	if (bTopMost)
		FlipScreen();
	}

void CHumanInterface::Exit (void)

//	Exit
//
//	Exit the game

	{
	::SendMessage(GetHWND(), WM_CLOSE, 0, 0);
	}

bool CHumanInterface::GetMousePos (int *retx, int *rety) const

//  GetMousePos
//
//  Returns the mouse position relative to the current session. Returns FALSE 
//  if we fail for some reason.

    {
    POINT ptMouse;
    if (!::GetCursorPos(&ptMouse))
        return false;

    m_ScreenMgr.GlobalToLocal(ptMouse.x, ptMouse.y, retx, rety);
    return true;
    }

CReanimator &CHumanInterface::GetReanimator (void)

//	GetReanimator
//
//	Returns the current session's reanimator

	{
	if (m_pCurSession)
		return m_pCurSession->HIGetReanimator();
	else
		return g_DefaultReanimator;
	}

IHISession *CHumanInterface::GetTopSession (bool bNonTransparentOnly)

//	GetTopSession
//
//	Returns the top-most session.

	{
	int i;

	//	If we allow transparent sessions then just return the top-most session

	if (!bNonTransparentOnly || (m_pCurSession == NULL) || !m_pCurSession->IsTransparent())
		return m_pCurSession;

	//	Otherwise, look for the session

	else
		{
		for (i = m_SavedSessions.GetCount() - 1; i >= 0; i--)
			{
			//	If this session is fully opaque, then we're done. 

			if (!m_SavedSessions[i]->IsTransparent())
				return m_SavedSessions[i];
			}

		//	Otherwise, not found

		return NULL;
		}
	}

void CHumanInterface::HardCrash (const CString &sProgramState)

//	HardCrash
//
//	Report an error

	{
	CString sSessionMessage;
	try
		{
		if (m_pCurSession)
			m_pCurSession->HIReportHardCrash(&sSessionMessage);
		}
	catch (...)
		{
		sSessionMessage = CONSTLIT("Unable to obtain crash report from session.");
		}

	CString sMessage = strPatternSubst(CONSTLIT(
			"Unable to continue due to program error.\r\n\r\n"
			"program state: %s\r\n"
			"%s"
			"\r\n\r\nPlease contact transcendence@kronosaur.com with a copy of Debug.log and your save file. "
			"We are sorry for the inconvenience.\r\n"),
			sProgramState,
			sSessionMessage
			);

	kernelDebugLogMessage(sMessage);
	ShowHardCrashSession(CONSTLIT("Transcendence System Crash"), sMessage);

	//	Ask the controller to post a crash report

	CString *pCrashReport = new CString(::kernelGetSessionDebugLog());
	if (HICommand(CONSTLIT("cmdPostCrashReport"), pCrashReport) == ERR_NOTFOUND)
		delete pCrashReport;
	}

void CHumanInterface::HIPostCommand (const CString &sCmd, void *pData)

//	HIPostCommand
//
//	This may be called by a different thread to post a command to the main thread.

	{
	SPostCommand *pMsg = new SPostCommand;
	pMsg->sCmd = sCmd;
	pMsg->pData = pData;

	if (!::PostMessage(m_hWnd, WM_HI_COMMAND, 0, (LPARAM)pMsg))
		::kernelDebugLogMessage("Unable to send post HICommand (PostMessage failed): %s.", sCmd);
	}

void CHumanInterface::OnAnimate (void)

//	OnAnimate
//
//	Paint an animation frame

	{
	int i;

	//	If minimized, bail out

	if (m_ScreenMgr.IsMinimized())
		return;

	//	DirectDraw

	if (!m_ScreenMgr.CheckIsReady())
		return;

	//	Get the screen. Remember that we need to release it when we're done.

	CG32bitImage &Screen = GetScreen();

	//	Paint the current session

	if (m_pCurSession)
		{
		bool bCrash = false;

		try
			{
			//	Paint the background sessions

			for (i = 0; i < m_BackgroundSessions.GetCount(); i++)
				m_BackgroundSessions[i]->HIAnimate(Screen, false);

			//	Paint the current session

			m_pCurSession->HIAnimate(Screen, true);
			}
		catch (...)
			{
			bCrash = true;
			}

		//	If we crashed, report it

		if (bCrash)
			HardCrash(CONSTLIT("OnAnimate"));
		}
	else
		{
		Screen.Set(CG32bitPixel(0, 0, 0));
		BltScreen();
		FlipScreen();
		}
	}

void CHumanInterface::OnPostCommand (LPARAM pData)

//	OnPostCommand
//
//	Handle a posted command

	{
	SPostCommand *pMsg = (SPostCommand *)pData;
	HICommand(pMsg->sCmd, pMsg->pData);
	delete pMsg;
	}

void CHumanInterface::OnTaskComplete (DWORD dwID, LPARAM pData)

//	OnTaskComplete
//
//	Process a message that a given task is complete.
	
	{
	switch (dwID)
		{
		case ID_BACKGROUND_PROCESSOR:
			m_Background.OnTaskComplete(pData); 
			break;

		case ID_LOW_PRIORITY_BACKGROUND_PROCESSOR:
			m_BackgroundLowPriority.OnTaskComplete(pData);
			break;

		default:
			ASSERT(false);
		}
	}

ALERROR CHumanInterface::OpenPopupSession (IHISession *pSession, CString *retsError)

//	OpenPopupSession
//
//	Pushes the current session on the stack and makes the given
//	session public

	{
	ALERROR error;
	CString sError;

	ASSERT(pSession);

	//	Save the current session

	IHISession *pOldSession = m_pCurSession;

	//	Show the new session

	m_pCurSession = pSession;
	if (error = m_pCurSession->HIInit(&sError))
		{
		if (retsError)
			*retsError = sError;

		//	Restore the previous session

		m_pCurSession = pOldSession;
		return error;
		}

	//	Now that we know that the new session will be shown, add the old 
	//	session to the stack.

	if (pOldSession)
		m_SavedSessions.Insert(pOldSession);

	//	Send size message

	m_pCurSession->HISize(GetScreenWidth(), GetScreenHeight());

	//	Recompute paint order

	CalcBackgroundSessions();

	//	Show the cursor

	if (m_pCurSession->IsCursorShown())
		::ShowCursor(true);

	return NOERROR;
	}

void CHumanInterface::PaintFrameRate (void)

//	PaintFrameRate
//
//	Paints the current frame rate

	{
	CG32bitImage &Screen = GetScreen();

	int iRate = (int)(m_FrameRate.GetFrameRate() + 0.5f);
	CString sText = strPatternSubst(CONSTLIT("Frame rate: %d"), iRate);

	const CG16bitFont &TitleFont = m_Visuals.GetFont(fontSubTitle);
	RECT rcRect;
	rcRect.left = 0;
	rcRect.right = GetScreenWidth();
	rcRect.top = 0;
	rcRect.bottom = TitleFont.GetHeight();
	TitleFont.DrawText(Screen, rcRect, CG32bitPixel(128,128,128), sText);

	//	LATER: This should move to EndSessionUpdate
	if (m_pCurSession)
		m_pCurSession->HIInvalidate(rcRect);
	}

ALERROR CHumanInterface::ShowSession (IHISession *pSession, CString *retsError)

//	ShowSession
//
//	Takes ownership of the session object and gives it control
//	of the screen.

	{
	ALERROR error;
	CString sError;

	//	Initialize the new session

	if (pSession)
		{
		if (error = pSession->HIInit(&sError))
			{
			if (retsError)
				*retsError = sError;
			return error;
			}
		}

	//	We always replace the bottom most session

	if (m_SavedSessions.GetCount() > 0)
		{
		//	Clean up previous session

		m_SavedSessions[0]->HICleanUp();
		delete m_SavedSessions[0];

		//	Replace

		m_SavedSessions[0] = pSession;

		//	Recompute paint order

		CalcBackgroundSessions();
		}
	else
		{
		//	Clean up previous session

		if (m_pCurSession)
			{
			if (m_pCurSession->IsCursorShown())
				::ShowCursor(false);

			m_pCurSession->HICleanUp();
			delete m_pCurSession;
			}

		m_pCurSession = pSession;
		if (m_pCurSession)
			{
			//	Send size message

			m_pCurSession->HISize(GetScreenWidth(), GetScreenHeight());

			//	Show the cursor

			if (m_pCurSession->IsCursorShown())
				::ShowCursor(true);
			}
		}

	return NOERROR;
	}

int CHumanInterface::SetSoundVolume (int iVolume)

//	SetSoundVolume
//
//	Sets the volume

	{
	if (iVolume < 0 || iVolume > 10 || iVolume == m_Options.m_iSoundVolume)
		return m_Options.m_iSoundVolume;

	//	Set it

	m_Options.m_iSoundVolume = iVolume;
	m_SoundMgr.SetWaveVolume(iVolume);
	m_SoundMgr.SetMusicVolume(iVolume);

	//	Done

	return m_Options.m_iSoundVolume;
	}

