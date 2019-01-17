//	IHISession.cpp
//
//	IHISession class
//	Copyright (c) 2010 by George Moromisato. All Rights Reserved.

#include "stdafx.h"

#define CMD_SOUNDTRACK_NEXT						CONSTLIT("cmdSoundtrackNext")
#define CMD_SOUNDTRACK_PLAY_PAUSE				CONSTLIT("cmdSoundtrackPlayPause")
#define CMD_SOUNDTRACK_PREV						CONSTLIT("cmdSoundtrackPrev")
#define CMD_SOUNDTRACK_STOP						CONSTLIT("cmdSoundtrackStop")

#define PROP_ENABLED							CONSTLIT("enabled")

IHISession::IHISession (CHumanInterface &HI) : IHICommand(HI),
		m_bNoCursor(false),
		m_bTransparent(false),
		m_bSessionCapture(false)

//	IHISession constructor
	
	{
	}

IHISession::~IHISession (void)

//	IHISession destructor

	{
	}

void IHISession::DefaultOnAnimate (CG32bitImage &Screen, bool bTopMost)

//	DefaultOnAnimate
//
//	Only if session does not override OnAnimate

	{
	bool bHasAnimation = false;

	//	Let the session paint the screen

	m_HI.BeginSessionPaint(Screen);

	//	Session itself paints

	HIPaint(Screen);

	//	Paint animations on top

	if (OnPaintReanimator(Screen))
		bHasAnimation = true;

	//	Done

	m_HI.EndSessionPaint(Screen, bTopMost);

	//	Update

	m_HI.BeginSessionUpdate();
	HIUpdate(bTopMost);
	m_HI.EndSessionUpdate(bTopMost);

	//	If we've got animations, then we need to invalidate
	//	the whole screen.

	if (bHasAnimation)
		HIInvalidate();
	}

void IHISession::GetRect (RECT &rcRect)

//  GetRect
//
//  Get the full session rect (usually the entire screen).

    {
    rcRect.left = 0;
    rcRect.top = 0;
    rcRect.right = g_pHI->GetScreenWidth();
    rcRect.bottom = g_pHI->GetScreenHeight();
    }

bool IHISession::HandlePageScrollKeyDown (const CString &sScroller, int iVirtKey, DWORD dwKeyData)

//	HandlePageScrollKeyDown
//
//	Handle keyboard interface for scrolling a simple page.

	{
	EScrollTypes iScroll;
	int iScrollDist;

	switch (iVirtKey)
		{
		case VK_DOWN:
			iScroll = scrollRelative;
			iScrollDist = 50;
			break;

		case VK_UP:
			iScroll = scrollRelative;
			iScrollDist = -50;
			break;

		case VK_NEXT:
			iScroll = scrollRelative;
			iScrollDist = 512;
			break;

		case VK_PRIOR:
			iScroll = scrollRelative;
			iScrollDist = -512;
			break;

		case VK_END:
			iScroll = scrollToEnd;
			iScrollDist = 0;
			break;

		case VK_HOME:
			iScroll = scrollToHome;
			iScrollDist = 0;
			break;

		default:
			iScroll = scrollNone;
			iScrollDist = 0;
			break;
		}

	//	Scroll

	if (iScroll != scrollNone)
		{
		IAnimatron *pList = GetPerformance(sScroller);
		if (pList)
			pList->Scroll(iScroll, iScrollDist);

		return true;
		}
	else
		return false;
	}

bool IHISession::HandlePageScrollMouseWheel (const CString &sScroller, int iDelta)

//	HandlePageScrollMouseWheel
//
//	Handle mouse wheel

	{
	if (iDelta != 0)
		{
		IAnimatron *pList = GetPerformance(sScroller);
		if (pList)
			pList->Scroll(scrollRelative, -iDelta);

		return true;
		}
	else
		return false;
	}

void IHISession::HIChar (char chChar, DWORD dwKeyData)

//	HIChar
//
//	Handle character input
	
	{
	//	See if the animator will handle it

	if (m_Reanimator.HandleChar(chChar, dwKeyData))
		return;

	//	Otherwise, the subclass can handle it.

	OnChar(chChar, dwKeyData); 
	}

void IHISession::HIKeyDown (int iVirtKey, DWORD dwKeyData)

//	HIKeyDown
//
//	Handle character input
	
	{
	//	See if the animator will handle it

	if (m_Reanimator.HandleKeyDown(iVirtKey, dwKeyData))
		return;

	//	Handle some system level keys

	switch (iVirtKey)
		{
		case VK_MEDIA_NEXT_TRACK:
			m_HI.HICommand(CMD_SOUNDTRACK_NEXT);
			return;

		case VK_MEDIA_PLAY_PAUSE:
			m_HI.HICommand(CMD_SOUNDTRACK_PLAY_PAUSE);
			return;

		case VK_MEDIA_PREV_TRACK:
			m_HI.HICommand(CMD_SOUNDTRACK_PREV);
			return;

		case VK_MEDIA_STOP:
			m_HI.HICommand(CMD_SOUNDTRACK_STOP);
			return;

		//	Fall through
		}

	//	Let our subclass handle it.

	OnKeyDown(iVirtKey, dwKeyData);
	}

void IHISession::HIKeyUp (int iVirtKey, DWORD dwKeyData)

//	HIKeyUp
//
//	Handle character input

	{
	//	See if the animator will handle it

	if (m_Reanimator.HandleKeyUp(iVirtKey, dwKeyData))
		return;

	OnKeyUp(iVirtKey, dwKeyData);
	}
	
void IHISession::HILButtonDblClick (int x, int y, DWORD dwFlags)

//	HILButtonDblClick
//
//	Handle mouse input
	
	{
	//	See if the animator will handle it

	bool bCapture = false;
	if (m_Reanimator.HandleLButtonDblClick(x, y, dwFlags, &bCapture))
		return;

	OnLButtonDblClick(x, y, dwFlags); 
	}

void IHISession::HILButtonDown (int x, int y, DWORD dwFlags)

//	HILButtonDown
//
//	Handle mouse input
	
	{
	m_bSessionCapture = false;

	//	See if the animator will handle it

	bool bCapture = false;
	if (m_Reanimator.HandleLButtonDown(x, y, dwFlags, &bCapture))
		return;

	bool bSessionCapture = false;
	OnLButtonDown(x, y, dwFlags, &bSessionCapture);

	//	If the session wants to capture the mouse, set it.
	//
	//	NOTE: We can't assume that the session will be valid after OnLButtonDown,
	//	so we use a temporary variable. We do assume that if a session captures
	//	the mouse, it doesn't also destroy itself.

	if (bSessionCapture)
		m_bSessionCapture = true;
	}

void IHISession::HILButtonUp (int x, int y, DWORD dwFlags)

//	HILButtonUp
//
//	Handle mouse input
	
	{
	//	See if the animator will handle it

	if (!m_bSessionCapture 
			&& m_Reanimator.HandleLButtonUp(x, y, dwFlags))
		return;

	m_bSessionCapture = false;
	OnLButtonUp(x, y, dwFlags);
	}

void IHISession::HIMButtonDblClick (int x, int y, DWORD dwFlags)

//	HIMButtonDblClick
//
//	Handle mouse input
	
	{
	//	See if the animator will handle it

#ifdef LATER
	bool bCapture = false;
	if (m_Reanimator.HandleMButtonDblClick(x, y, dwFlags, &bCapture))
		{
		if (bCapture)
			::SetCapture(m_HI.GetHWND());
		return;
		}
#endif

	OnMButtonDblClick(x, y, dwFlags); 
	}

void IHISession::HIMButtonDown (int x, int y, DWORD dwFlags)

//	HIMButtonDown
//
//	Handle mouse input
	
	{
	//	See if the animator will handle it

#ifdef LATER
	bool bCapture = false;
	if (m_Reanimator.HandleMButtonDown(x, y, dwFlags, &bCapture))
		{
		if (bCapture)
			::SetCapture(m_HI.GetHWND());
		return;
		}
#endif

	bool bSessionCapture = false;
	OnMButtonDown(x, y, dwFlags, &bSessionCapture);
	}

void IHISession::HIMButtonUp (int x, int y, DWORD dwFlags)

//	HIMButtonUp
//
//	Handle mouse input
	
	{
	//	See if the animator will handle it

#ifdef LATER
	if (!m_bCapture
			&& m_Reanimator.HandleMButtonUp(x, y, dwFlags))
		return;
#endif

	OnMButtonUp(x, y, dwFlags);
	}

void IHISession::HIMouseMove (int x, int y, DWORD dwFlags)

//	HIMouseMove
//
//	Handle mouse input
	
	{
	//	See if the animator will handle it

	if (!m_bSessionCapture
			&& m_Reanimator.HandleMouseMove(x, y, dwFlags))
		return;

	OnMouseMove(x, y, dwFlags);
	}

void IHISession::HIMouseWheel (int iDelta, int x, int y, DWORD dwFlags)

//	HIMouseWheel
//
//	Handle mouse wheel

	{
	//	See if the animator will handle it

	if (m_Reanimator.HandleMouseWheel(iDelta, x, y, dwFlags))
		return;

	OnMouseWheel(iDelta, x, y, dwFlags);
	}

void IHISession::HIPaint (CG32bitImage &Screen)

//	HIPaint
//
//	Paint the session screen
	
	{
	RECT rcInvalid;
	if (m_HI.GetScreenMgr().GetInvalidRect(&rcInvalid))
		{
		//	Prepare

		Screen.SetClipRect(rcInvalid);

		//	Paint

		OnPaint(Screen, rcInvalid);

		//	Done

		m_HI.GetScreenMgr().Validate();
		Screen.ResetClipRect();
		}
	}

void IHISession::HIRButtonDblClick (int x, int y, DWORD dwFlags)

//	HIRButtonDblClick
//
//	Handle mouse input
	
	{
	//	See if the animator will handle it

	bool bCapture = false;
	if (m_Reanimator.HandleRButtonDblClick(x, y, dwFlags, &bCapture))
		return;

	OnRButtonDblClick(x, y, dwFlags); 
	}

void IHISession::HIRButtonDown (int x, int y, DWORD dwFlags)

//	HIRButtonDown
//
//	Handle mouse input
	
	{
	//	See if the animator will handle it

	bool bCapture = false;
	if (m_Reanimator.HandleRButtonDown(x, y, dwFlags, &bCapture))
		return;

	OnRButtonDown(x, y, dwFlags);
	}

void IHISession::HIRButtonUp (int x, int y, DWORD dwFlags)

//	HIRButtonUp
//
//	Handle mouse input
	
	{
	//	See if the animator will handle it

	if (m_Reanimator.HandleRButtonUp(x, y, dwFlags))
		return;

	OnRButtonUp(x, y, dwFlags);
	}

bool IHISession::IsElementEnabled (const CString &sID)

//	IsElementEnabled
//
//	Returns TRUE if the given element exists and is enabled

	{
	IAnimatron *pAni = GetElement(sID);
	if (pAni == NULL)
		return false;

	return pAni->GetPropertyBool(PROP_ENABLED);
	}

void IHISession::OnAniCommand (const CString &sID, const CString &sEvent, const CString &sCmd, DWORD dwData)

//	OnAniCommand
//
//	Handle event from reanimator

	{
	SPerformanceEvent Ctx;
	Ctx.sID = sID;
	Ctx.sEvent = sEvent;

	HICommand(sCmd, &Ctx);
	}

void IHISession::RegisterPerformanceEvent (IAnimatron *pAni, const CString &sEvent, const CString &sCmd)

//	RegisterPerformanceEvent
//
//	Register to get events from an animatron control

	{
	pAni->AddListener(sEvent, this, sCmd);
	}

void IHISession::StartPerformance (IAnimatron *pAni, const CString &sID, DWORD dwFlags)

//	StartPerformance
//
//	Start an animation performance

	{
	DWORD dwPerf = m_Reanimator.AddPerformance(pAni, sID);
	m_Reanimator.StartPerformance(dwPerf, dwFlags);
	}
