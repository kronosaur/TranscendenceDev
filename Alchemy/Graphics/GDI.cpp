//	GDI.cpp
//
//	Contains routines to manipulate basic GDI calls
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void gdiFillRect (HDC hDC, const RECT *pRect, COLORREF Color)

//	gdiFillRect
//
//	Fill the rectangle with the given color

	{
	COLORREF OldColor = SetBkColor(hDC, Color);
	ExtTextOut(hDC, pRect->left, pRect->top, ETO_OPAQUE, pRect, "", 0, NULL);
	SetBkColor(hDC, OldColor);
	}
