//	CInputKeyPainter.cpp
//
//	CInputKeyPainter class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CInputKeyPainter::Init (const CString &sKey, int cyHeight, const CG16bitFont &Font, int cxWidth)

//	Init
//
//	Initialize metrics.

	{
	m_sKey = sKey;
	m_pFont = &Font;
	m_cyHeight = (cyHeight > 0 ? cyHeight : DEFAULT_HEIGHT);
	m_cxKey = m_pFont->MeasureText(m_sKey);

	//	If width is valid, then we use that.

	if (cxWidth > 0)
		m_cxWidth = cxWidth;

	//	Otherwise, we calculate it based on the font size.

	else
		{
		//	If the text fits in a square key, then that's good enough.

		if (m_cxKey <= cyHeight)
			m_cxWidth = m_cyHeight;

		//	Otherwise, we grow the key.

		else
			{
			const int cxHalf = Max(1, cyHeight / 2);
			const int cxMinWidth = m_cxKey + 2 * HORZ_PADDING;
			m_cxWidth = AlignUp(cxMinWidth, cxHalf);
			}
		}
	}

void CInputKeyPainter::Paint (CG32bitImage &Dest, int x, int y)

//	Paint
//
//	Paint key

	{
	if (!m_pFont || m_cyHeight <= 0 || m_cxWidth <= 0)
		return;

	CGDraw::RoundedRect(Dest, x, y, m_cxWidth, m_cyHeight, BORDER_RADIUS, m_rgbBackColor);
	CGDraw::RoundedRectOutline(Dest, x, y, m_cxWidth, m_cyHeight, BORDER_RADIUS, BORDER_WIDTH, m_rgbTextColor);

	m_pFont->DrawText(Dest, 
			x + (m_cxWidth - m_cxKey) / 2,
			y + (m_cyHeight - m_pFont->GetHeight()) / 2,
			m_rgbTextColor, 
			m_sKey,
			0);
	}
