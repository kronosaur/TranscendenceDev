//	CDockScreenPainter.cpp
//
//	CDockScreenPainter class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

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
			BORDER_RADIUS,
			1,
			CG32bitPixel(80,80,80));
	}
