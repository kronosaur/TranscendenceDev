//	CGButtonArea.cpp
//
//	Implementation of CGButtonArea class
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define RGB_DISABLED_TEXT						(CG32bitPixel(128,128,128))

#define SPECIAL_ACCEL_LEFT_ARROW				CONSTLIT("LeftArrow")
#define SPECIAL_ACCEL_RIGHT_ARROW				CONSTLIT("RightArrow")

const int ACCEL_INNER_PADDING =					2;
const int ACCEL_BORDER_RADIUS =					2;

CGButtonArea::CGButtonArea (void) : 
		m_rgbLabelColor(CG32bitPixel(255,255,255)),
		m_pLabelFont(NULL),

		m_rgbAccelColor(CG32bitPixel(255,255,0)),
		m_iAccelerator(-1),

		m_rgbDescColor(CG32bitPixel(255,255,255)),
		m_pDescFont(NULL),
		m_cxJustifyWidth(0),

		m_iBorderRadius(0),
		m_rgbBorderColor(CG32bitPixel::Null()),
		m_rgbBackColor(CG32bitPixel::Null()),
		m_rgbBackColorHover(CG32bitPixel::Null()),

		m_bMouseOver(false),
		m_bDisabled(false)

//	CGButtonArea constructor

	{
	m_rcPadding.left = 0;
	m_rcPadding.top = 0;
	m_rcPadding.right = 0;
	m_rcPadding.bottom = 0;
	}

RECT CGButtonArea::CalcTextRect (const RECT &rcRect)

//	CalcTextRect
//
//	Calculates the text rect given the control rect

	{
	RECT rcText = rcRect;
	rcText.left += m_rcPadding.left;
	rcText.top += m_rcPadding.top;
	rcText.right -= m_rcPadding.right;
	rcText.bottom -= m_rcPadding.bottom;

	return rcText;
	}

int CGButtonArea::Justify (const RECT &rcRect)

//	Justify
//
//	Justify the text and return the height (in pixels)

	{
	RECT rcText = CalcTextRect(rcRect);

	//	For now, the label is always one line

	int cyHeight = (m_pLabelFont ? m_pLabelFont->GetHeight() : 0);

	//	Add the height of the description, if we have one

	if (!m_sDesc.IsBlank() && m_pDescFont)
		{
		if (m_cxJustifyWidth != RectWidth(rcText))
			{
			m_cxJustifyWidth = RectWidth(rcText);
			m_Lines.DeleteAll();
			m_pDescFont->BreakText(m_sDesc, m_cxJustifyWidth, &m_Lines, CG16bitFont::SmartQuotes);
			}

		cyHeight += (m_Lines.GetCount() * m_pDescFont->GetHeight());
		}

	//	Add padding

	cyHeight += m_rcPadding.top + m_rcPadding.bottom;

	return cyHeight;
	}

void CGButtonArea::LButtonUp (int x, int y)

//	LButtonUp
//
//	Mouse button up

	{
	if (m_bMouseOver && !m_bDisabled && IsVisible())
		SignalAction();
	}

void CGButtonArea::MouseEnter (void)

//	MouseEnter
//
//	Mouse has entered the area

	{
	m_bMouseOver = true;
	Invalidate();
	}

void CGButtonArea::MouseLeave (void)

//	MouseLeave
//
//	Mouse has left the area

	{
	m_bMouseOver = false;
	Invalidate();
	}

void CGButtonArea::OnSetRect (void)

//	OnSetRect
//
//	Handle moves

	{
	//	If the mouse is over our area, then set state
	}

CG32bitPixel CGButtonArea::CalcBackColor (void) const

//	CalcBackColor
//
//	Returns the color to use when painting the background.

	{
	if (m_bDisabled)
		{
		if (!m_rgbBackColor.IsNull())
			return CG32bitPixel(m_rgbBackColor, 0xC0);
		else
			return m_rgbBackColor;
		}
	else if (m_bMouseOver)
		return m_rgbBackColorHover;
	else
		return m_rgbBackColor;
	}

void CGButtonArea::Paint (CG32bitImage &Dest, const RECT &rcRect)

//	Paint
//
//	Handle paint

	{
	DEBUG_TRY

	int i;

	//	Paint the background

	CG32bitPixel rgbBackColor = CalcBackColor();
	if (!rgbBackColor.IsNull())
		{
		if (m_iBorderRadius > 0)
			{
			CGDraw::RoundedRect(Dest, rcRect.left, rcRect.top, RectWidth(rcRect), RectHeight(rcRect), m_iBorderRadius, rgbBackColor);
			CGDraw::RoundedRectOutline(Dest, rcRect.left, rcRect.top, RectWidth(rcRect), RectHeight(rcRect), m_iBorderRadius, 1, m_rgbBorderColor);
			}
		else
			Dest.Fill(rcRect.left, rcRect.top, RectWidth(rcRect), RectHeight(rcRect), rgbBackColor);
		}

	//	Paint the label

	int xPaint = rcRect.left + m_rcPadding.left;
	int yPaint = rcRect.top + m_rcPadding.top;
	if (m_pLabelFont)
		{
		//	If we have a prefix accelerator, draw that

		if (!m_sAccelerator.IsBlank())
			{
			CString sLabel;
			const CG16bitFont *pLabelFont;

			//	Colors

			CG32bitPixel rgbAccel = (m_bDisabled ? RGB_DISABLED_TEXT : m_rgbAccelColor);
			CG32bitPixel rgbText = (m_bDisabled ? RGB_DISABLED_TEXT : m_rgbLabelColor);

			//	Some accelerator names are special; we draw symbols

			if (strEquals(m_sAccelerator, SPECIAL_ACCEL_LEFT_ARROW))
				{
				sLabel = CSTR_WINGDING_LEFT_ARROW;
				pLabelFont = &GetScreen()->GetWingdingsFont();
				}
			else if (strEquals(m_sAccelerator, SPECIAL_ACCEL_RIGHT_ARROW))
				{
				sLabel = CSTR_WINGDING_RIGHT_ARROW;
				pLabelFont = &GetScreen()->GetWingdingsFont();
				}
			else
				{
				sLabel = m_sAccelerator;
				pLabelFont = m_pLabelFont;
				}

			//	Measure the size of the accelerator

			int cyAccel;
			int cxAccel = pLabelFont->MeasureText(sLabel, &cyAccel);

			//	We draw a rounded-rect box

			int cxBox = cxAccel + (2 * ACCEL_INNER_PADDING);
			int cyBox = cyAccel + 1;
			CGDraw::RoundedRect(Dest, xPaint, yPaint, cxBox, cyBox, ACCEL_BORDER_RADIUS, rgbAccel);

			//	Draw the text

			Dest.DrawText(xPaint + ACCEL_INNER_PADDING, yPaint, *pLabelFont, CG32bitPixel(0, 0, 0), sLabel);

			//	Now draw the rest of the label

			Dest.DrawText(xPaint + cxBox + (2 * ACCEL_INNER_PADDING), yPaint, *m_pLabelFont, rgbText, m_sLabel);
			}

		//	If we're disabled, paint gray

		else if (m_bDisabled)
			{
			Dest.DrawText(xPaint,
					yPaint,
					*m_pLabelFont,
					RGB_DISABLED_TEXT,
					m_sLabel);
			}

		//	If we have an accelerator, paint in pieces

		else if (m_iAccelerator != -1)
			{
			CDrawText::WithAccelerator(Dest, xPaint, yPaint, m_sLabel, m_iAccelerator, *m_pLabelFont, m_rgbLabelColor, m_rgbAccelColor);
			}
		else
			Dest.DrawText(xPaint,
					yPaint,
					*m_pLabelFont,
					m_rgbLabelColor,
					m_sLabel);

		yPaint += m_pLabelFont->GetHeight();
		}

	//	Paint the description

	if (m_pDescFont)
		{
		for (i = 0; i < m_Lines.GetCount(); i++)
			{
			Dest.DrawText(xPaint,
					yPaint,
					*m_pDescFont,
					m_rgbDescColor,
					m_Lines[i]);

			yPaint += m_pDescFont->GetHeight();
			}
		}

	DEBUG_CATCH
	}

void CGButtonArea::SetLabelAccelerator (const CString &sKey, int iKeyOffset)

//	SetLabelAccelerator
//
//	Sets the key to highlight as accelerator.

	{
	if (sKey.IsBlank())
		{
		m_sAccelerator = NULL_STR;
		m_iAccelerator = -1;
		}

	else if (sKey.GetLength() > 1)
		{
		m_sAccelerator = sKey;
		m_iAccelerator = -1;
		}

	else if (iKeyOffset != -1)
		{
		m_sAccelerator = NULL_STR;
		m_iAccelerator = iKeyOffset;
		}

	else
		{
		char *pStart = m_sLabel.GetASCIIZPointer();
		char *pPos = pStart;
		char *pKey = sKey.GetASCIIZPointer();
		bool bFirstLetter = true;

		//	First look for the accelerator as the first letter in a word

		while (*pPos != '\0')
			{
			if (bFirstLetter && CharLower((LPTSTR)(BYTE)(*pKey)) == CharLower((LPTSTR)(BYTE)(*pPos)))
				{
				m_iAccelerator = pPos - pStart;
				return;
				}

			if (bFirstLetter)
				{
				//	If we have a quote, then it still counts as the first letter
				if (*pPos != '\'' && *pPos != '\"' && *pPos != ' ')
					bFirstLetter = false;
				}
			else
				bFirstLetter = (*pPos == ' ');

			pPos++;
			}

		//	Otherwise, look for the any matching letter

		m_iAccelerator = strFind(m_sLabel, sKey);

		//	No long accelerator

		m_sAccelerator = NULL_STR;
		}
	}
