//	CNarrativeDisplay.cpp
//
//	CNarrativeDisplay class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Transcendence.h"

void CNarrativeDisplay::Init (const RECT &rcRect)

//	Init
//
//	Initialize

	{
	const CVisualPalette &VI = m_HI.GetVisuals();
	const CG16bitFont &TitleFont = VI.GetFont(fontHeader);
	const CG16bitFont &BodyFont = VI.GetFont(fontMedium);

	m_iState = stateNone;
	m_iStartTick = 0;

	m_rcRect.left = rcRect.left + MARGIN_X;
	m_rcRect.right = m_rcRect.left + DISPLAY_WIDTH;
	m_rcRect.bottom = rcRect.bottom - MARGIN_Y;
	m_rcRect.top = m_rcRect.bottom - DISPLAY_HEIGHT;

	m_Buffer.Create(DISPLAY_WIDTH, DISPLAY_HEIGHT, CG32bitImage::alpha8);
	}

void CNarrativeDisplay::Paint (CG32bitImage &Screen, int iTick) const

//	Paint
//
//	Paint on screen.

	{
	const CVisualPalette &VI = m_HI.GetVisuals();

	switch (m_iState)
		{
		case stateFadeIn:
		case stateNormal:
		case stateFadeOut:
			Screen.Blt(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, m_byOpacity, m_Buffer, m_rcRect.left, m_rcRect.top);
			break;
		}
	}

void CNarrativeDisplay::Show (ENarrativeDisplayStyle iStyle, const CTileData &Data)

//	Show
//
//	Show a narrative element.

	{
	const CVisualPalette &VI = m_HI.GetVisuals();

	m_Buffer.Set(CG32bitPixel::PreMult(RGB_BACKGROUND_COLOR, BACKGROUND_OPACITY));

	//	Paint the content

	CTilePainter::SInitOptions InitOptions;
	InitOptions.cxImage = IMAGE_WIDTH;
	InitOptions.cyImage = IMAGE_HEIGHT;

	switch (iStyle)
		{
		case ENarrativeDisplayStyle::missionAccept:
			{
			InitOptions.pTitleFont = &VI.GetFont(fontLarge);
			InitOptions.pDescFont = &VI.GetFont(fontSubTitleBold);
			break;
			}
		}


	CTilePainter Painter;
	Painter.Init(Data, DISPLAY_WIDTH, InitOptions);

	CTilePainter::SPaintOptions PaintOptions;
	PaintOptions.rgbTitleColor = VI.GetColor(colorTextHighlight);
	PaintOptions.rgbDescColor = VI.GetColor(colorTextNormal);
	Painter.Paint(m_Buffer, 0, 0, PaintOptions);

	//	Mask out a rounded-rect

	CGDraw::MaskRoundedRect(m_Buffer, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, BACKGROUND_CORNER_RADIUS);

	//	Start

	m_iState = stateStart;
	m_iStartTick = 0;
	}

void CNarrativeDisplay::Update (int iTick)

//	Update
//
//	Update

	{
	switch (m_iState)
		{
		case stateStart:
			m_iStartTick = iTick;
			m_iState = stateFadeIn;
			m_byOpacity = 0;
			break;

		case stateFadeIn:
			{
			int iTime = iTick - m_iStartTick;
			if (iTime < FADE_IN_TIME)
				{
				m_byOpacity = iTime * 255 / FADE_IN_TIME;
				}
			else
				{
				m_iStartTick = iTick;
				m_iState = stateNormal;
				m_byOpacity = 255;
				}
			break;
			}

		case stateNormal:
			if (iTick - m_iStartTick >= NORMAL_TIME)
				{
				m_iStartTick = iTick;
				m_iState = stateFadeOut;
				}
			break;

		case stateFadeOut:
			{
			int iTime = iTick - m_iStartTick;
			if (iTime < FADE_OUT_TIME)
				{
				m_byOpacity = (FADE_OUT_TIME - iTime) * 255 / FADE_OUT_TIME;
				}
			else
				m_iState = stateNone;
			break;
			}
		}
	}
