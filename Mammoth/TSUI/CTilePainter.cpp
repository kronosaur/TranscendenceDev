//	CTilePainter.cpp
//
//	CTilePainter class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

void CTilePainter::Init (const CTileData &Entry, int cxWidth, const SInitOptions &Options)

//	Init
//
//	Initializes the painter.

	{
	m_Entry = Entry;
	m_Options = Options;

	m_cxWidth = cxWidth;

	//	Choose default fonts, if the caller did not supply them

	if (!m_Options.pTitleFont)
		m_Options.pTitleFont = &m_VI.GetFont(fontLargeBold);

	if (!m_Options.pDescFont)
		m_Options.pDescFont = &m_VI.GetFont(fontMedium);

	//	Compute the rect for the text region

	m_rcText.left = m_Options.cxImage + PADDING_X;
	m_rcText.right = m_cxWidth - PADDING_X;
	m_rcText.top = PADDING_Y;

	//	Compute text height

	int cyText = m_Options.pTitleFont->GetHeight();
	int iLines = m_Options.pDescFont->BreakText(m_Entry.GetDesc(), RectWidth(m_rcText), NULL, 0);
	cyText += iLines * m_Options.pDescFont->GetHeight();
	m_rcText.bottom = m_rcText.top + cyText;

	//	If the row specifies its own row height, then use that.

	if (Entry.GetHeight())
		m_cyHeight = Entry.GetHeight();

	//	If we have a valid default from the list, then use that.

	else if (Options.cyDefaultRow)
		m_cyHeight = Options.cyDefaultRow;

	//	Otherwise, compute the height based on icon and description.

	else
		{
		if (m_Entry.GetImage())
			m_cyHeight = Max(m_Options.cyImage, cyText + 2 * PADDING_Y);
		else
			m_cyHeight = cyText + 2 * PADDING_Y;
		}
	}

void CTilePainter::Paint (CG32bitImage &Dest, int x, int y, const SPaintOptions &Options) const

//	Paint
//
//	Paints the entry.

	{
	if (IsEmpty())
		return;

	//	Paint the image

	PaintImage(Dest, x, y);

	RECT rcDrawRect;
	rcDrawRect.left = x + m_rcText.left;
	rcDrawRect.right = x + m_rcText.right;
	rcDrawRect.top = y + m_rcText.top;
	rcDrawRect.bottom = y + m_cyHeight - 2 * PADDING_Y;

	//	Measure the title and description

	int cyText = RectHeight(m_rcText);
	int yOffset = Max(0, (RectHeight(rcDrawRect) - cyText) / 2);

	//	Paint the title

	int cyHeight;
	rcDrawRect.top += yOffset;
	m_Options.pTitleFont->DrawText(Dest,
			rcDrawRect,
			Options.rgbTitleColor,
			m_Entry.GetTitle(),
			0,
			CG16bitFont::SmartQuotes | CG16bitFont::TruncateLine,
			&cyHeight);

	rcDrawRect.top += cyHeight;

	//	Paint the description

	m_Options.pDescFont->DrawText(Dest, 
			rcDrawRect,
			Options.rgbDescColor,
			m_Entry.GetDesc(),
			0,
			CG16bitFont::SmartQuotes,
			&cyHeight);

	rcDrawRect.top += cyHeight;
	}

void CTilePainter::PaintImage (CG32bitImage &Dest, int x, int y) const

//	PaintImage
//
//	Paints the image.

	{
	if (m_Entry.GetImage() == NULL)
		return;

	Metric rScale = (m_Entry.GetImageScale() != 1.0 ? m_Entry.GetImageScale() : m_Options.rImageScale);
	CPaintHelper::PaintScaledImage(Dest, x, y, m_Options.cxImage, Min(m_cyHeight, m_Options.cyImage), *m_Entry.GetImage(), m_Entry.GetImageSrc(), rScale);
	}
