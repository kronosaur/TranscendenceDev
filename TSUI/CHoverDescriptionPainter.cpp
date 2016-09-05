//	CHoverDescriptionPainter.cpp
//
//	CHoverDescriptionPainter class
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

const int DEFAULT_BORDER_RADIUS =					4;
const int DEFAULT_LINE_SPACING =					0;
const int DEFAULT_PADDING_BOTTOM =					8;
const int DEFAULT_PADDING_LEFT =					8;
const int DEFAULT_PADDING_TOP =						8;
const int DEFAULT_PADDING_RIGHT =					8;

CHoverDescriptionPainter::CHoverDescriptionPainter (const CVisualPalette &VI) :
		m_VI(VI),
		m_cxWidth(0),
		m_rcRect({ 0, 0, 0, 0 }),
		m_rgbBack(VI.GetColor(colorAreaDialog)),
		m_rgbTitle(VI.GetColor(colorTextHighlight)),
		m_rgbDescription(VI.GetColor(colorTextNormal))

	//	CHoverDescriptionPainter constructor

	{
	}

void CHoverDescriptionPainter::Format (void) const

//	Format
//
//	Make sure text is formatted. Initializes:
//
//		m_DescriptionRTF
//		m_rcRect
//		m_rcText

	{
	//	If we're already valid, or we're invisible, then exit

	if (!IsInvalid()
			|| !IsVisible())
		return;

	//	Compute the inner width
	
	int cxContent = m_cxWidth - (DEFAULT_PADDING_LEFT + DEFAULT_PADDING_RIGHT);

	//	Format the RTF description

	if (!m_sDescription.IsBlank())
		{
		SBlockFormatDesc BlockFormat;

		BlockFormat.cxWidth = cxContent;
		BlockFormat.cyHeight = -1;
		BlockFormat.iHorzAlign = alignLeft;
		BlockFormat.iVertAlign = alignTop;
		BlockFormat.iExtraLineSpacing = DEFAULT_LINE_SPACING;

		BlockFormat.DefaultFormat.rgbColor = m_rgbDescription;
		BlockFormat.DefaultFormat.pFont = &m_VI.GetFont(fontMedium);

		m_DescriptionRTF.InitFromRTF(m_sDescription, m_VI, BlockFormat);
		}
	else
		m_DescriptionRTF.DeleteAll();

	//	Compute the height of the content

	int cyContent = 0;

	//	Title

	if (!m_sTitle.IsBlank())
		cyContent += m_VI.GetFont(fontHeader).GetHeight();

	//	Description

	if (!m_DescriptionRTF.IsEmpty())
		{
		RECT rcBounds;
		m_DescriptionRTF.GetBounds(&rcBounds);
		cyContent += RectHeight(rcBounds);
		}

	//	Now compute the outer rect dimensions

	int cxRect = m_cxWidth;
	int cyRect = DEFAULT_PADDING_TOP + cyContent + DEFAULT_PADDING_BOTTOM;

	//	Figure out the position of the rect. Start at the desired position.

	m_rcRect.left = m_xPos;
	m_rcRect.top = m_yPos;

	//	Make sure we fit horizontally

	if (m_rcRect.left < m_rcContainer.left)
		m_rcRect.left = m_rcContainer.left;
	else if (m_rcRect.left + cxRect > m_rcContainer.right)
		m_rcRect.left = Max(m_rcContainer.left, m_rcContainer.right - cxRect);

	//	If we don't fit below the position, and we can fit above it, adjust

	if (m_rcRect.top + cyRect > m_rcContainer.bottom
			&& m_rcRect.top - cyRect >= m_rcContainer.top)
		m_rcRect.top = m_rcRect.top - cyRect;

	//	Complete the rect

	m_rcRect.right = m_rcRect.left + cxRect;
	m_rcRect.bottom = m_rcRect.top + cyRect;

	//	Now initialize the inner text rect

	m_rcText.left = m_rcRect.left + DEFAULT_PADDING_LEFT;
	m_rcText.top = m_rcRect.top + DEFAULT_PADDING_TOP;
	m_rcText.right = m_rcRect.right - DEFAULT_PADDING_RIGHT;
	m_rcText.bottom = m_rcRect.bottom - DEFAULT_PADDING_BOTTOM;
	}

void CHoverDescriptionPainter::Paint (CG32bitImage &Dest) const

//	Paint
//
//	Paint the description

	{
	if (!IsVisible())
		return;

	//	Format, if necessary

	Format();

	//	Paint the background

	CGDraw::RoundedRect(Dest, m_rcRect.left, m_rcRect.top, RectWidth(m_rcRect), RectHeight(m_rcRect), DEFAULT_BORDER_RADIUS, m_rgbBack);

	//	Paint the title

	int y = m_rcText.top;
	if (!m_sTitle.IsBlank())
		{
		m_VI.GetFont(fontHeader).DrawText(Dest, m_rcText, m_rgbTitle, m_sTitle, 0, CG16bitFont::TruncateLine);

		y += m_VI.GetFont(fontHeader).GetHeight();
		}

	//	Paint the description

	m_DescriptionRTF.Paint(Dest, m_rcText.left, y);
	}

void CHoverDescriptionPainter::SetDescription (const CString &sValue)

//	SetDescription
//
//	Sets the description (which may be RTF).

	{
	CUIHelper UIHelper(*g_pHI);
	UIHelper.GenerateDockScreenRTF(sValue, &m_sDescription);

	Invalidate();
	}

void CHoverDescriptionPainter::Show (int x, int y, int cxWidth, const RECT &rcContainer)

//	Show
//
//	Show the description with the given settings.

	{
	//	Width must not be zero.

	if (cxWidth <= (DEFAULT_PADDING_LEFT + DEFAULT_PADDING_RIGHT))
		return;

	m_xPos = x;
	m_yPos = y;
	m_cxWidth = cxWidth;
	m_rcContainer = rcContainer;

	Invalidate();
	}
