//	CGDraw.cpp
//
//	CGDraw Class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CGDraw::OctaRectOutline (CG32bitImage &Dest, int x, int y, int cxWidth, int cyHeight, int iCorner, int iLineWidth, CG32bitPixel rgbColor)

//	DrawOctaRectOutline
//
//	Draw the frame of an octagonal rectangle.

	{
	int i;

	//	Range checking

	if (iLineWidth <= 0)
		return;

	iCorner = Min(Min(Max(0, iCorner), cxWidth / 2), cyHeight / 2);

	//	Paint the straight edges first

	Dest.Fill(x + iCorner, y, cxWidth - (2 * iCorner), iLineWidth, rgbColor);
	Dest.Fill(x + iCorner, y + cyHeight - iLineWidth, cxWidth - (2 * iCorner), iLineWidth, rgbColor);
	Dest.Fill(x, y + iCorner, iLineWidth, cyHeight - (2 * iCorner), rgbColor);
	Dest.Fill(x + cxWidth - iLineWidth, y + iCorner, iLineWidth, cyHeight - (2 * iCorner), rgbColor);

	//	Now paint the corners

	int cxCornerLine = (int)((1.414 * iLineWidth) + 0.5);
	int cyRows = iCorner + (cxCornerLine - iLineWidth);
	for (i = 0; i < cyRows; i++)
		{
		int xOffset = iCorner - i;
		int xLineEnd = Min(xOffset + cxCornerLine, cxWidth - iCorner);
		xOffset = Max(0, xOffset);
		int cxLine = xLineEnd - xOffset;

		Dest.FillLine(x + xOffset, y + i, cxLine, rgbColor);
		Dest.FillLine(x + xOffset, y + cyHeight - 1 - i, cxLine, rgbColor);

		Dest.FillLine(x + cxWidth - cxLine - xOffset, y + i, cxLine, rgbColor);
		Dest.FillLine(x + cxWidth - cxLine - xOffset, y + cyHeight - 1 - i, cxLine, rgbColor);
		}
	}

void CGDraw::RectGradient (CG32bitImage &Dest, int xDest, int yDest, int cxDest, int cyDest, CG32bitPixel rgbStart, CG32bitPixel rgbEnd, GradientDirections iDir)

//	RectGradient
//
//	Draws a gradient

	{
	//	Make sure we're in bounds

	int x = xDest;
	int y = yDest;
	int cxWidth = cxDest;
	int cyHeight = cyDest;
	if (!Dest.AdjustCoords(NULL, NULL, 0, 0, 
			&x, &y, 
			&cxWidth, &cyHeight))
		return;

	ASSERT(cxDest > 0);
	ASSERT(x >= xDest);

	//	Draw the gradient horizontally, column by column, from left to right.
	//	The left-most column is the start color; the right-most column is the
	//	end color.

	Metric rStep = (cxDest == 1 ? 0.0 : 1.0 / (cxDest - 1));
	Metric rGradient = rStep * (x - xDest);

	BYTE byStartOpacity = rgbStart.GetAlpha();
	Metric rOpacityRange = ((Metric)rgbEnd.GetAlpha() - (Metric)byStartOpacity);

	//	Horizontal gradient

	if (iDir == gradientHorizontal)
		{
		CG32bitPixel *pPos = Dest.GetPixelPos(x, y);
		CG32bitPixel *pPosEnd = pPos + cxWidth;
		CG32bitPixel *pColEnd = Dest.GetPixelPos(x, y + cyHeight);
		int iRowSize = (Dest.GetPixelPos(0, 1) - Dest.GetPixelPos(0, 0));
		int iColSize = pColEnd - pPos;

		while (pPos < pPosEnd)
			{
			//	Compute the color of this column

			CG32bitPixel rgbColor = CG32bitPixel::Blend(rgbStart, rgbEnd, rGradient);
			BYTE byOpacity = (BYTE)(DWORD)(byStartOpacity + (rOpacityRange * rGradient));

			//	Draw the column

			CG32bitPixel *pCol = pPos;
			CG32bitPixel *pColEnd = pPos + iColSize;

			if (byOpacity == 0x00)
				;
			else if (byOpacity == 0xff)
				{
				while (pCol < pColEnd)
					{
					*pCol = rgbColor;
					pCol += iRowSize;
					}
				}
			else
				{
				BYTE *pAlpha = CG32bitPixel::AlphaTable(byOpacity);
				BYTE *pAlphaInv = CG32bitPixel::AlphaTable(byOpacity ^ 0xff);	//	Equivalent to 255 - byAlpha

				WORD bySrcRed = pAlpha[rgbColor.GetRed()];
				WORD bySrcGreen = pAlpha[rgbColor.GetGreen()];
				WORD bySrcBlue = pAlpha[rgbColor.GetBlue()];

				while (pCol < pColEnd)
					{
					BYTE byRedResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pCol->GetRed()] + bySrcRed));
					BYTE byGreenResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pCol->GetGreen()] + bySrcGreen));
					BYTE byBlueResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pCol->GetBlue()] + bySrcBlue));

					*pCol = CG32bitPixel(byRedResult, byGreenResult, byBlueResult);
					pCol += iRowSize;
					}
				}

			//	Next

			rGradient += rStep;
			pPos++;
			}
		}
	else if (iDir == gradientVertical)
		{
		ASSERT(false);
		//	Not Yet Implemented
		}
	}

void CGDraw::RectOutlineDotted (CG32bitImage &Dest, int x, int y, int cxWidth, int cyHeight, CG32bitPixel rgbColor)

//	RectDotted
//
//	Draw a dotted rect

	{
	LineDotted(Dest, x, y, x + cxWidth, y, rgbColor);
	LineDotted(Dest, x, y + cyHeight, x + cxWidth, y + cyHeight, rgbColor);
	LineDotted(Dest, x, y, x, y + cyHeight, rgbColor);
	LineDotted(Dest, x + cxWidth, y, x + cxWidth, y + cyHeight, rgbColor);
	}

void CGDraw::RoundedRect (CG32bitImage &Dest, int x, int y, int cxWidth, int cyHeight, int iRadius, CG32bitPixel rgbColor)

//	RoundedRect
//
//	Draws a filled rounded rect

	{
	int i;

	if (iRadius <= 0)
		{
		Dest.Fill(x, y, cxWidth, cyHeight, rgbColor);
		return;
		}

	BYTE byOpacity = rgbColor.GetAlpha();

	//	Generate a set of raster lines for the corner

	int *pSolid = new int [iRadius];
	BYTE *pEdge = new BYTE [iRadius];
	RasterizeQuarterCircle8bit(iRadius, pSolid, pEdge, byOpacity);

	//	Fill in each corner

	for (i = 0; i < iRadius; i++)
		{
		int xOffset = iRadius - pSolid[i];
		int cxLine = cxWidth - (iRadius * 2) + (pSolid[i] * 2);

		//	Top edge

		Dest.FillLine(x + xOffset, y + i, cxLine, rgbColor);
		Dest.SetPixelTrans(x + xOffset - 1, y + i, rgbColor, pEdge[i]);
		Dest.SetPixelTrans(x + cxWidth - xOffset, y + i, rgbColor, pEdge[i]);

		//	Bottom edge

		Dest.FillLine(x + xOffset, y + cyHeight - i - 1, cxLine, rgbColor);
		Dest.SetPixelTrans(x + xOffset - 1, y + cyHeight - i - 1, rgbColor, pEdge[i]);
		Dest.SetPixelTrans(x + cxWidth - xOffset, y + cyHeight - i - 1, rgbColor, pEdge[i]);
		}

	//	Fill the center

	Dest.Fill(x, y + iRadius, cxWidth, (cyHeight - 2 * iRadius), rgbColor);

	//	Done

	delete [] pSolid;
	delete [] pEdge;
	}

void CGDraw::RoundedRectOutline (CG32bitImage &Dest, int x, int y, int cxWidth, int cyHeight, int iRadius, int iLineWidth, CG32bitPixel rgbColor)

//	RoundedRectOutline
//
//	Draws a rounded rect outline

	{
	//	Range checking

	if (iLineWidth <= 0
			|| rgbColor.IsNull())
		return;

	iRadius = Min(Min(Max(0, iRadius), cxWidth / 2), cyHeight / 2);

	//	Intermediates

	int iHalfWidth = iLineWidth / 2;

	//	Paint the straight edges first.

	Dest.Fill(x + iRadius, y - iHalfWidth, cxWidth - (2 * iRadius), iLineWidth, rgbColor);
	Dest.Fill(x + iRadius, y + cyHeight - iHalfWidth, cxWidth - (2 * iRadius), iLineWidth, rgbColor);
	Dest.Fill(x - iHalfWidth, y + iRadius, iLineWidth, cyHeight - (2 * iRadius), rgbColor);
	Dest.Fill(x + cxWidth - iHalfWidth, y + iRadius, iLineWidth, cyHeight - (2 * iRadius), rgbColor);

	//	If no radius, then we're done

	if (iRadius == 0)
		return;

	//	Paint the corners

	ArcCorner(Dest, x + iRadius, y + iRadius, iRadius, 90, 180, iLineWidth, rgbColor);
	ArcCorner(Dest, x + cxWidth - iRadius, y + iRadius, iRadius, 0, 90, iLineWidth, rgbColor);
	ArcCorner(Dest, x + iRadius, y + cyHeight - iRadius, iRadius, 180, 270, iLineWidth, rgbColor);
	ArcCorner(Dest, x + cxWidth - iRadius, y + cyHeight - iRadius, iRadius, 270, 360, iLineWidth, rgbColor);
	}
