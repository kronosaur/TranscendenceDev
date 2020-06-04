//	CInputKeyPainter.cpp
//
//	CInputKeyPainter class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CInputKeyPainter::Init (DWORD dwVirtKey, int cyHeight, const CG16bitFont &LargeFont, const CG16bitFont &SmallFont)

//	Init
//
//	Initializes based on virtual key.

	{
	switch (dwVirtKey)
		{
		case VK_LBUTTON:
			m_iType = keyLeftMouseButton;
			m_cyHeight = cyHeight;
			m_cxWidth = 3 * cyHeight / 4;
			break;

		case VK_MBUTTON:
			m_iType = keyScrollWheel;
			m_cyHeight = cyHeight;
			m_cxWidth = 3 * cyHeight / 4;
			break;

		case VK_RBUTTON:
			m_iType = keyRightMouseButton;
			m_cyHeight = cyHeight;
			m_cxWidth = 3 * cyHeight / 4;
			break;

		default:
			{
			CString sLabel = CVirtualKeyData::GetKeyLabel(dwVirtKey);
			if (sLabel.GetLength() > 1)
				Init(sLabel, cyHeight, SmallFont);
			else
				Init(sLabel, cyHeight, LargeFont);
			}
		}
	}

void CInputKeyPainter::Init (const CString &sKey, int cyHeight, const CG16bitFont &Font, int cxWidth)

//	Init
//
//	Initialize metrics.

	{
	m_iType = keyLabel;
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

void CInputKeyPainter::Paint (CG32bitImage &Dest, int x, int y) const

//	Paint
//
//	Paint key

	{
	if (m_cyHeight <= 0 || m_cxWidth <= 0)
		return;

	PaintBackground(Dest, x, y);
	PaintLabel(Dest, x, y);
	}

void CInputKeyPainter::PaintArrowSymbol (CG32bitImage &Dest, int x, int y) const

//	PaintArrowSymbol
//
//	Paints one of the directional arrows.

	{
	const int INSET = Max(1, m_cxWidth / 4);

	const int yTop = y + INSET;
	const int xLeft = x + INSET;
	const int xRight = x + m_cxWidth - INSET;
	const int yBottom = y + m_cyHeight - INSET;
	const int cxWidth = xRight - xLeft;
	const int cyHeight = yBottom - yTop;
	const int cxHalfWidth = cxWidth / 2;
	const int cyHalfHeight = cyHeight / 2;
	const int cxQuarterWidth = cxHalfWidth / 2;
	const int cyQuarterHeight = cyHalfHeight / 2;

	switch (m_iType)
		{
		case keyArrowUp:
			CGDraw::TriangleCorner(Dest, xLeft + cxHalfWidth, yTop, 2, cyHalfHeight, m_rgbTextColor);
			Dest.Fill(xLeft + cxQuarterWidth + 1, yTop + cyHalfHeight, cxHalfWidth - 1, cyHalfHeight, m_rgbTextColor);
			break;

		case keyArrowDown:
			CGDraw::TriangleCorner(Dest, xLeft + cxHalfWidth, yBottom - 1, 6, cyHalfHeight, m_rgbTextColor);
			Dest.Fill(xLeft + cxQuarterWidth + 1, yTop, cxHalfWidth - 1, cyHalfHeight, m_rgbTextColor);
			break;

		case keyArrowLeft:
			CGDraw::TriangleCorner(Dest, xLeft, yTop + cyHalfHeight, 4, cxHalfWidth, m_rgbTextColor);
			Dest.Fill(xLeft + cxHalfWidth, yTop + cyQuarterHeight + 1, cxHalfWidth, cyHalfHeight - 1, m_rgbTextColor);
			break;

		case keyArrowRight:
			CGDraw::TriangleCorner(Dest, xRight - 1, yTop + cyHalfHeight, 0, cxHalfWidth, m_rgbTextColor);
			Dest.Fill(xLeft, yTop + cyQuarterHeight + 1, cxHalfWidth, cyHalfHeight - 1, m_rgbTextColor);
			break;
		}
	}

void CInputKeyPainter::PaintBackground (CG32bitImage &Dest, int x, int y) const

//	PaintBackground
//
//	Paint the key background.

	{
	switch (m_iType)
		{
		case keyLeftMouseButton:
		case keyRightMouseButton:
		case keyScrollWheel:
			break;

		default:
			CGDraw::RoundedRect(Dest, x, y, m_cxWidth, m_cyHeight, BORDER_RADIUS, m_rgbBackColor);
			CGDraw::RoundedRectOutline(Dest, x, y, m_cxWidth, m_cyHeight, BORDER_RADIUS, BORDER_WIDTH, m_rgbTextColor);
			break;
		}
	}

void CInputKeyPainter::PaintLabel (CG32bitImage &Dest, int x, int y) const

//	PaintLabel
//
//	Paint the button label.

	{
	switch (m_iType)
		{
		case keyArrowDown:
		case keyArrowLeft:
		case keyArrowRight:
		case keyArrowUp:
			PaintArrowSymbol(Dest, x, y);
			break;

		case keyLeftMouseButton:
		case keyRightMouseButton:
		case keyScrollWheel:
			PaintMouse(Dest, x, y);
			break;

		default:
			if (!m_pFont)
				return;

			m_pFont->DrawText(Dest, 
					x + (m_cxWidth - m_cxKey) / 2,
					y + (m_cyHeight - m_pFont->GetHeight()) / 2,
					m_rgbTextColor, 
					m_sKey,
					0);
			break;
		}
	}

void CInputKeyPainter::PaintMouse (CG32bitImage &Dest, int x, int y) const

//	PaintMouse
//
//	Paints a mouse button symbol.

	{
	const int GAP = Max(1, m_cxWidth / 10);
	const int BUTTON_SIZE = Max(1, (m_cxWidth - GAP) / 2);
	const int UPPER_HEIGHT = BUTTON_SIZE + GAP;
	const int LOWER_WIDTH = 2 * BUTTON_SIZE + GAP;
	const int LOWER_HEIGHT = Max(1, m_cyHeight - (BUTTON_SIZE + GAP));
	const int MOUSE_RADIUS = Min(LOWER_HEIGHT, BUTTON_SIZE);

	const int SMALL_BUTTON_WIDTH = Max(1, (LOWER_WIDTH - (3 * GAP)) / 2);
	const int MIDDLE_BUTTON_WIDTH = Max(1, LOWER_WIDTH - (2 * SMALL_BUTTON_WIDTH) - (2 * GAP));

	const CG32bitPixel rgbMouse = CG32bitPixel::Blend(m_rgbBackColor, m_rgbTextColor, (BYTE)0x80);

	//	Paint the lower part of the mouse

	Dest.Fill(x, y, LOWER_WIDTH, UPPER_HEIGHT, m_rgbTextColor);
	CGDraw::RoundedRectBottom(Dest, x, y + UPPER_HEIGHT, LOWER_WIDTH, LOWER_HEIGHT, MOUSE_RADIUS, rgbMouse);

	//	Paint the button part

	switch (m_iType)
		{
		case keyLeftMouseButton:
			Dest.Fill(x, y, BUTTON_SIZE, BUTTON_SIZE, m_rgbBackColor);
			Dest.Fill(x + GAP + BUTTON_SIZE, y, BUTTON_SIZE, BUTTON_SIZE, rgbMouse);
			break;

		case keyRightMouseButton:
			Dest.Fill(x, y, BUTTON_SIZE, BUTTON_SIZE, rgbMouse);
			Dest.Fill(x + GAP + BUTTON_SIZE, y, BUTTON_SIZE, BUTTON_SIZE, m_rgbBackColor);
			break;

		case keyScrollWheel:
			Dest.Fill(x, y, SMALL_BUTTON_WIDTH, BUTTON_SIZE, rgbMouse);
			Dest.Fill(x + SMALL_BUTTON_WIDTH + GAP, y, MIDDLE_BUTTON_WIDTH, BUTTON_SIZE, m_rgbBackColor);
			Dest.Fill(x + SMALL_BUTTON_WIDTH + GAP + MIDDLE_BUTTON_WIDTH + GAP, y, SMALL_BUTTON_WIDTH, BUTTON_SIZE, rgbMouse);
			break;
		}
	}
