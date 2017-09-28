//	FilterThreshold.cpp
//
//	CGFilter Class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CGFilter::Threshold (CG32bitImage &Src, const RECT &rcSrc, BYTE byThreshold, CG8bitImage &Dest, int xDest, int yDest)

//	Threshold
//
//	Generates a new image in which every pixel is either 0 (if < threshold) or 255 (if >= threshold).

	{
	int xSrc = rcSrc.left;
	int ySrc = rcSrc.top;
	int cxWidth = RectWidth(rcSrc);
	int cyHeight = RectHeight(rcSrc);

	//	Make sure we're in bounds

	if (!Dest.AdjustCoords(&xSrc, &ySrc, Src.GetWidth(), Src.GetHeight(), 
			&xDest, &yDest,
			&cxWidth, &cyHeight))
		return;

	CG32bitPixel *pSrcRow = Src.GetPixelPos(xSrc, ySrc);
	CG32bitPixel *pSrcRowEnd = Src.GetPixelPos(xSrc, ySrc + cyHeight);
	BYTE *pDestRow = Dest.GetPixelPos(xDest, yDest);

	while (pSrcRow < pSrcRowEnd)
		{
		CG32bitPixel *pSrcPos = pSrcRow;
		CG32bitPixel *pSrcPosEnd = pSrcRow + cxWidth;
		BYTE *pDestPos = pDestRow;

		while (pSrcPos < pSrcPosEnd)
			{
			BYTE byValue = pSrcPos->Grayscale();
			if (byValue >= byThreshold)
				*pDestPos = 255;
			else
				*pDestPos = 0;

			pSrcPos++;
			pDestPos++;
			}

		pSrcRow = Src.NextRow(pSrcRow);
		pDestRow = Dest.NextRow(pDestRow);
		}
	}

void CGFilter::Threshold (CG32bitImage &Src, const RECT &rcSrc, BYTE byThreshold, CG32bitImage &Dest, int xDest, int yDest)

//	Threshold
//
//	Generates a new image in which every pixel is either 0 (if < threshold) or 255 (if >= threshold).

	{
	int xSrc = rcSrc.left;
	int ySrc = rcSrc.top;
	int cxWidth = RectWidth(rcSrc);
	int cyHeight = RectHeight(rcSrc);

	//	Make sure we're in bounds

	if (!Dest.AdjustCoords(&xSrc, &ySrc, Src.GetWidth(), Src.GetHeight(), 
			&xDest, &yDest,
			&cxWidth, &cyHeight))
		return;

	CG32bitPixel *pSrcRow = Src.GetPixelPos(xSrc, ySrc);
	CG32bitPixel *pSrcRowEnd = Src.GetPixelPos(xSrc, ySrc + cyHeight);
	CG32bitPixel *pDestRow = Dest.GetPixelPos(xDest, yDest);

	while (pSrcRow < pSrcRowEnd)
		{
		CG32bitPixel *pSrcPos = pSrcRow;
		CG32bitPixel *pSrcPosEnd = pSrcRow + cxWidth;
		CG32bitPixel *pDestPos = pDestRow;

		while (pSrcPos < pSrcPosEnd)
			{
			BYTE byValue = pSrcPos->Grayscale();
			if (byValue >= byThreshold)
				*pDestPos = CG32bitPixel(255, 255, 255, 255);
			else
				*pDestPos = CG32bitPixel(0, 0, 0, 0);

			pSrcPos++;
			pDestPos++;
			}

		pSrcRow = Src.NextRow(pSrcRow);
		pDestRow = Dest.NextRow(pDestRow);
		}
	}
