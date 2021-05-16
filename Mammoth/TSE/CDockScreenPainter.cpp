//	CDockScreenPainter.cpp
//
//	CDockScreenPainter class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CDockScreenPainter::PaintDisplayBackground (CG32bitImage &Dest, const RECT &rcRect) const

//	PaintDisplayBackground
//
//	Paint the background.

	{
	CG32bitPixel rgbFadeBackColor = CG32bitPixel(CG32bitPixel::Darken(m_Theme.GetTextBackgroundColor(), 220), 220);
	CGDraw::RoundedRect(Dest,
			rcRect.left,
			rcRect.top + m_cyTabRegion,
			RectWidth(rcRect),
			RectHeight(rcRect) - m_cyTabRegion,
			m_Theme.GetBorderRadius() + 1,	//	+1 because frame is outside.
			rgbFadeBackColor);
	}

void CDockScreenPainter::PaintDisplayFrame (CG32bitImage &Dest, const RECT &rcRect) const

//	PaintDisplayFrame
//
//	Paints a display frame around the rect.

	{
	CGDraw::RoundedRectOutline(Dest,
			rcRect.left,
			rcRect.top,
			RectWidth(rcRect),
			RectHeight(rcRect),
			m_Theme.GetBorderRadius(),
			1,
			CG32bitPixel(80,80,80));
	}
