//	CStatusBarPainter.cpp
//
//	CStatusBarPainter class
//	Copyright (c) 2021 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CStatusBarPainter::Init (int iStatus, const CString &sLabel, CG32bitPixel rgbColor, const SOptions &Options)

//	Init
//
//	Initialize the painter.

	{
	m_iStatus = iStatus;
	m_sLabel = sLabel;
	m_rgbColor = rgbColor;
	m_Options = Options;

	//	Use default font

	if (!m_Options.pLabelFont)
		m_Options.pLabelFont = &m_Universe.GetNamedFont(CUniverse::fontSRSObjCounter);
	}

void CStatusBarPainter::Paint (CG32bitImage &Dest, int x, int y) const

//	Paint
//
//	Paint the bar. x, y are the coordinates for the top-center of the bar.

	{
	if (m_Options.iMaxStatus <= 0)
		return;

	CG32bitPixel rgbFill = CG32bitPixel(m_rgbColor, m_Options.byFillOpacity);
	CG32bitPixel rgbOutline = CG32bitPixel(m_rgbColor, m_Options.byOutlineOpacity);
	CG32bitPixel rgbBlack = CG32bitPixel(CG32bitPixel(0), m_Options.byFillOpacity);

	int xStart = x - (m_Options.cxBar / 2);
	int yStart = y;

	int iFill = Max(1, m_iStatus * m_Options.cxBar / m_Options.iMaxStatus);

	//	Draw

	Dest.Fill(xStart, yStart + 1, iFill, m_Options.cyBar - 2, rgbFill);
	Dest.Fill(xStart + iFill, yStart + 1, m_Options.cxBar - iFill, m_Options.cyBar - 2, rgbBlack);

	Dest.DrawLine(xStart, yStart, xStart + m_Options.cxBar, yStart, 1, rgbOutline);
	Dest.DrawLine(xStart, yStart + m_Options.cyBar - 1, xStart + m_Options.cxBar, yStart + m_Options.cyBar - 1, 1, rgbOutline);
	Dest.DrawLine(xStart - 1, yStart, xStart - 1, yStart + m_Options.cyBar, 1, rgbOutline);
	Dest.DrawLine(xStart + m_Options.cxBar, yStart, xStart + m_Options.cxBar, yStart + m_Options.cyBar, 1, rgbOutline);

	//	Draw the label

	if (!m_sLabel.IsBlank() && m_Options.pLabelFont)
		{
		CG32bitPixel rgbLabelColor = CG32bitPixel(255, 255, 255);
		m_Options.pLabelFont->DrawText(Dest, x, y, rgbLabelColor, m_sLabel, CG16bitFont::AlignCenter);
		}
	}
