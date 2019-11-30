//	CDockScreenLayout.cpp
//
//	CDockScreenLayout class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Transcendence.h"

bool CDockScreenLayout::Init (const RECT &rcScreen, const CDockScreenVisuals &Visuals)

//	Init
//
//	Initializes basic metrics.

	{
	//	Compute RECTs.
	//
	//	m_rScreen is the RECT of the AGScreen relative to the HWND client area.
	//
	//	m_rcFrame is the full RECT of the background including the title bar.
	//	This RECT shrinks to fit the background image, and expands up to the 
	//	hard-coded limit. It is relative to the AGScreen.
	//
	//	m_rcFrameImage is the RECT of the frame image, relative to the AGScreen.
	//	It is generally the same as m_rcFrame but it excludes the title area.
	//
	//	m_rcContent is RECT limits for the controls. It is narrower than 
	//	m_rcFrameImage, and is relative to AGScreen.

	m_rcScreen = rcScreen;

    CG32bitImage &FrameImage = Visuals.GetBackground().GetImage(CONSTLIT("ShowScreen"));
	int cxMaxFrame = (FrameImage.IsEmpty() ? MAX_FRAME_IMAGE_WIDTH : FrameImage.GetWidth());
	int cyMaxFrame = (FrameImage.IsEmpty() ? MAX_FRAME_IMAGE_HEIGHT : FrameImage.GetHeight() + TITLE_HEIGHT);

	int cxFrame = Min(cxMaxFrame, RectWidth(rcScreen));
	int cyFrame = Min(cyMaxFrame, RectHeight(rcScreen));
	m_rcFrame.left = (RectWidth(rcScreen) - cxFrame) / 2;
	m_rcFrame.right = m_rcFrame.left + cxFrame;

	//	We center the screen on the display area (below the title)

	m_rcFrame.top = ((RectHeight(rcScreen) - (cyFrame - TITLE_HEIGHT)) / 2) - TITLE_HEIGHT;
	m_rcFrame.bottom = m_rcFrame.top + cyFrame;

	//	The frame image RECT is just below the title bar.

	m_rcFrameImage = m_rcFrame;
	m_rcFrameImage.top += TITLE_HEIGHT;

	//	The content

	int cxContent = Min(MAX_CONTENT_WIDTH, RectWidth(m_rcFrame));
	m_rcContent.left = (RectWidth(rcScreen) - cxContent) / 2;
	m_rcContent.top = m_rcFrame.top + TITLE_HEIGHT;
	m_rcContent.right = m_rcContent.left + cxContent;
	m_rcContent.bottom = m_rcFrame.bottom;

	//	Compute the width of the display and the panes

	m_cxPane = Max(MIN_DESC_PANE_WIDTH, cxContent / 3);

	//	The main display is starts below the title bar

	m_cxDisplay = cxContent - m_cxPane;

	//	Compute the center of the display (relative to the frame image RECT).

	m_xBackgroundFocus = (cxContent - m_cxPane) / 2;
	m_yBackgroundFocus = RectHeight(m_rcFrameImage) / 2;

	//	Other metrics

	m_cyStatusFont = Visuals.GetStatusFont().GetHeight();

	//	Done

	return true;
	}

RECT CDockScreenLayout::GetCanvasRect (void) const

//	GetCanvasRect
//
//	Returns RECT of canvas area, relative to AGScreen.

	{
	RECT rcRect;
	rcRect.left = m_rcContent.left;
	rcRect.right = rcRect.left + GetDisplayWidth();
	rcRect.top = m_rcContent.top;
	rcRect.bottom = m_rcContent.bottom;

	return rcRect;
	}

RECT CDockScreenLayout::GetCargoStatusLabelRect (void) const

//	GetCargoStatusLabelRect
//
//	Returns the RECT of the cargo status label, relative to AGScreen.

	{
	int cyOffset = (STATUS_BAR_HEIGHT - m_cyStatusFont) / 2;

	RECT rcRect;
	rcRect.left = m_rcContent.right - CARGO_STATUS_WIDTH;
	rcRect.right = m_rcContent.right;
	rcRect.top = m_rcContent.top - STATUS_BAR_HEIGHT + cyOffset;
	rcRect.bottom = rcRect.top + m_cyStatusFont;

	return rcRect;
	}

RECT CDockScreenLayout::GetCargoStatusRect (void) const

//	GetCargoStatusRect
//
//	Returns the RECT of the cargo status, relative to AGScreen.

	{
	int cyOffset = (STATUS_BAR_HEIGHT - m_cyStatusFont) / 2;

	RECT rcRect;
	rcRect.left = m_rcContent.right - CARGO_STATUS_WIDTH + CARGO_STATUS_LABEL_WIDTH;
	rcRect.right = m_rcContent.right;
	rcRect.top = m_rcContent.top - STATUS_BAR_HEIGHT + cyOffset;
	rcRect.bottom = rcRect.top + m_cyStatusFont;

	return rcRect;
	}

RECT CDockScreenLayout::GetDisplayRect (void) const

//	GetDisplayRect
//
//	Returns the RECT for the display area, relative to AGScreen.

	{
	RECT rcRect;
	rcRect.left = m_rcContent.left + CONTENT_PADDING_LEFT;
	rcRect.right = rcRect.left + RectWidth(m_rcContent) - (GetPaneWidth() + CONTENT_PADDING_LEFT + CONTENT_PADDING_RIGHT);
	rcRect.top = m_rcContent.top;
	rcRect.bottom = m_rcContent.bottom;

	return rcRect;
	}

RECT CDockScreenLayout::GetMoneyStatusRect (void) const

//	GetMoneyStatusRect
//
//	Returns the RECT of the money status, relative to AGScreen.

	{
	int cyOffset = (STATUS_BAR_HEIGHT - m_cyStatusFont) / 2;

	RECT rcRect;
	rcRect.left = m_rcContent.right - STATUS_AREA_WIDTH;
	rcRect.right = m_rcContent.right;
	rcRect.top = m_rcContent.top - STATUS_BAR_HEIGHT + cyOffset;
	rcRect.bottom = rcRect.top + m_cyStatusFont;

	return rcRect;
	}

RECT CDockScreenLayout::GetStatusBarRect (void) const

//	GetStatusBarRect
//
//	Returns the RECT of the status bar, relative to the AGScreen.

	{
	RECT rcRect;
	rcRect.left = m_rcFrame.left;
	rcRect.right = m_rcFrame.right;
	rcRect.top = m_rcContent.top - STATUS_BAR_HEIGHT;
	rcRect.bottom = m_rcContent.top;

	return rcRect;
	}

RECT CDockScreenLayout::GetTitleBarRect (void) const

//	GetTitleBarRect
//
//	Returns the RECT of the title bar, relative to the AGScreen.

	{
	RECT rcRect;
	rcRect.left = m_rcFrame.left;
	rcRect.right = m_rcFrame.right;
	rcRect.top = m_rcFrame.top;
	rcRect.bottom = m_rcContent.top - STATUS_BAR_HEIGHT;

	return rcRect;
	}

RECT CDockScreenLayout::GetTitleRect (void) const

//	GetTitleRect
//
//	Returns the RECT of the title control, relative to AGScreen.

	{
	RECT rcRect;
	rcRect.left = m_rcFrame.left + TITLE_PADDING_LEFT;
	rcRect.right = m_rcFrame.right;
	rcRect.top = m_rcFrame.top;

	//	Descenders paint into the status bar.

	rcRect.bottom = m_rcContent.top;

	return rcRect;
	}
