//	CListEntryPainter.cpp
//
//	CListEntryPainter class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

void CListEntryPainter::Init (const IListData::SEntry &Entry, int cxWidth, const SOptions &Options)

//	Init
//
//	Initializes the painter.

	{
	const CG16bitFont &LargeBold = m_VI.GetFont(fontLargeBold);
	const CG16bitFont &Medium = m_VI.GetFont(fontMedium);

	m_Entry = Entry;
	m_Options = Options;

	m_cxWidth = cxWidth;

	//	Compute the rect for the text region

	m_rcText.left = m_Options.cxImage + PADDING_X;
	m_rcText.right = m_cxWidth - PADDING_X;
	m_rcText.top = PADDING_Y;

	//	Compute text height

	int cyText = LargeBold.GetHeight();
	int iLines = Medium.BreakText(m_Entry.sDesc, RectWidth(m_rcText), NULL, 0);
	cyText += iLines * Medium.GetHeight();
	m_rcText.bottom = m_rcText.top + cyText;

	//	If the row specifies its own row height, then use that.

	if (Entry.cyHeight)
		m_cyHeight = Entry.cyHeight;

	//	If we have a valid default from the list, then use that.

	else if (Options.cyDefaultRow)
		m_cyHeight = Options.cyDefaultRow;

	//	Otherwise, compute the height based on icon and description.

	else
		{
		if (m_Entry.pImage)
			m_cyHeight = Max(m_Options.cyImage, cyText + 2 * PADDING_Y);
		else
			m_cyHeight = cyText + 2 * PADDING_Y;
		}
	}

void CListEntryPainter::Paint (CG32bitImage &Dest, int x, int y, CG32bitPixel rgbTextColor, DWORD dwOptions) const

//	Paint
//
//	Paints the entry.

	{
	bool bSelected = ((dwOptions & OPTION_SELECTED) ? true : false);
	const CG32bitPixel rgbColorDescSel = CG32bitPixel(200,200,200);
	const CG32bitPixel rgbColorDesc = CG32bitPixel(128,128,128);

	const CG16bitFont &LargeBold = m_VI.GetFont(fontLargeBold);
	const CG16bitFont &Medium = m_VI.GetFont(fontMedium);

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
	LargeBold.DrawText(Dest,
			rcDrawRect,
			rgbTextColor,
			m_Entry.sTitle,
			0,
			CG16bitFont::SmartQuotes | CG16bitFont::TruncateLine,
			&cyHeight);

	rcDrawRect.top += cyHeight;

	//	Paint the description

	Medium.DrawText(Dest, 
			rcDrawRect,
			(bSelected ? rgbColorDescSel : rgbColorDesc),
			m_Entry.sDesc,
			0,
			CG16bitFont::SmartQuotes,
			&cyHeight);

	rcDrawRect.top += cyHeight;
	}

void CListEntryPainter::PaintImage (CG32bitImage &Dest, int x, int y) const

//	PaintImage
//
//	Paints the image.

	{
	if (m_Entry.pImage == NULL)
		return;

	Metric rScale = (m_Entry.rImageScale != 1.0 ? m_Entry.rImageScale : m_Options.rImageScale);
	CPaintHelper::PaintScaledImage(Dest, x, y, m_Options.cxImage, Min(m_cyHeight, m_Options.cyImage), *m_Entry.pImage, m_Entry.rcImageSrc, rScale);
	}
