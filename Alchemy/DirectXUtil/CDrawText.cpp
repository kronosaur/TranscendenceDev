//	CDrawText.cpp
//
//	CDrawText class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CDrawText::WithAccelerator (CG32bitImage &Dest, int x, int y, const CString &sText, int iAcceleratorPos, const CG16bitFont &Font, CG32bitPixel rgbTextColor, CG32bitPixel rgbAcceleratorColor)

//	WithAccelerator
//
//	Draws a line of text with a character highlighted.

	{
	const char *pPos = sText.GetASCIIZPointer();

	if (iAcceleratorPos > 0)
		Dest.DrawText(x, y, Font, rgbTextColor, CString(pPos, iAcceleratorPos, true), 0, &x);

	Dest.DrawText(x, y, Font, rgbAcceleratorColor, CString(pPos + iAcceleratorPos, 1, true), 0, &x);
	Dest.DrawText(x, y, Font, rgbTextColor, CString(pPos + iAcceleratorPos + 1, sText.GetLength() - iAcceleratorPos - 1, true));
	}
