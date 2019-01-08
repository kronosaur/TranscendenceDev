//	CPaintHelper.cpp
//
//	CPaintHelper class
//	Copyright (c) 2014 by Kronosaur Productions, LLC.

#include "PreComp.h"

const int ARROW_WIDTH =							6;

const int STATUS_BAR_WIDTH =					100;
const int STATUS_BAR_HEIGHT =					15;

void CPaintHelper::CalcSmoothColorTable (int iRadius, int iIntensity, CG32bitPixel rgbPrimary, CG32bitPixel rgbSecondary, BYTE byOpacity, TArray<CG32bitPixel> *retColorTable)

//	CalcSmoothColorTable
//
//	Computes a color table.

	{
	int i;

	int iFringeMaxRadius = iRadius * iIntensity / 120;
	int iFringeWidth = iFringeMaxRadius / 8;
	int iBlownRadius = iFringeMaxRadius - iFringeWidth;
	int iFadeWidth = iRadius - iFringeMaxRadius;

	if (retColorTable->GetCount() < iRadius)
		retColorTable->InsertEmpty(iRadius - retColorTable->GetCount());

	//	Initialize table

	for (i = 0; i < iRadius; i++)
		{
		if (i < iBlownRadius)
			(*retColorTable)[i] = CG32bitPixel(255, 255, 255, byOpacity);

		else if (i < iFringeMaxRadius && iFringeWidth > 0)
			{
			int iStep = (i - iBlownRadius);
			DWORD dwOpacity = iStep * byOpacity / iFringeWidth;
			(*retColorTable)[i] = CG32bitPixel::Blend(CG32bitPixel(255, 255, 255), rgbPrimary, (BYTE)dwOpacity);
			}
		else if (iFadeWidth > 0)
			{
			int iStep = (i - iFringeMaxRadius);
			Metric rOpacity = 1.0 - ((Metric)iStep / iFadeWidth);
			rOpacity = (rOpacity * rOpacity) * byOpacity;
			(*retColorTable)[i] = CG32bitPixel(rgbSecondary, (BYTE)(DWORD)rOpacity);
			}
		else
			(*retColorTable)[i] = CG32bitPixel::Null();
		}
	}

void CPaintHelper::PaintArrow (CG32bitImage &Dest, int x, int y, int iDirection, CG32bitPixel rgbColor)

//	PaintArrow
//
//	Paints an arrow with the tip at x, y and pointing towards iDirection.

	{
	iDirection += 180;

	//	Generate a set of points for the directional indicator

	SPoint Poly[5];

	//	Start at the origin

	Poly[0].x = 0;
	Poly[0].y = 0;

	//	Do one side first

	CVector vPos = PolarToVector(iDirection + 30, ARROW_WIDTH);
	Poly[1].x = (int)vPos.GetX();
	Poly[1].y = -(int)vPos.GetY();

	vPos = vPos + PolarToVector(iDirection, 3 * ARROW_WIDTH);
	Poly[2].x = (int)vPos.GetX();
	Poly[2].y = -(int)vPos.GetY();

	//	The other side

	vPos = PolarToVector(iDirection + 330, ARROW_WIDTH);
	CVector vPos2 = vPos + PolarToVector(iDirection, 3 * ARROW_WIDTH);

	Poly[3].x = (int)vPos2.GetX();
	Poly[3].y = -(int)vPos2.GetY();

	Poly[4].x = (int)vPos.GetX();
	Poly[4].y = -(int)vPos.GetY();

	//	Paint the directional indicator

	CG16bitBinaryRegion Region;
	Region.CreateFromConvexPolygon(5, Poly);
	Region.Fill(Dest, x, y, rgbColor);
	}

void CPaintHelper::PaintArrowText (CG32bitImage &Dest, int x, int y, int iDirection, const CString &sText, const CG16bitFont &Font, CG32bitPixel rgbColor)

//	PaintArrowText
//
//	Paints some text associated with an arrow.

	{
	int i;

	TArray<CString> Text;
	Font.BreakText(sText, 200, &Text);

	CVector vPos = PolarToVector(iDirection, 30);
	int xText = x - (int)vPos.GetX();
	int yText = y + (int)vPos.GetY();

	DWORD dwFontFlags = CG16bitFont::AdjustToFit | CG16bitFont::AlignCenter;
	if (iDirection <= 180)
		yText += 16;
	else
		yText -= 16 + (Text.GetCount() * Font.GetHeight());

	for (i = 0; i < Text.GetCount(); i++)
		{
		Font.DrawText(Dest, xText, yText, rgbColor, Text[i], CG16bitFont::AlignCenter);
		yText += Font.GetHeight();
		}
	}

void CPaintHelper::PaintScaledImage (CG32bitImage &Dest, int xDest, int yDest, int cxDest, int cyDest, CG32bitImage &Src, const RECT &rcSrc, Metric rScale)

//	PaintScaledImage
//
//	Paints the given image and scales it to fit into the given destination.
//	(Or, if rScale is supplied, uses that scale).

	{
	int cxSrc = RectWidth(rcSrc);
	int cySrc = RectHeight(rcSrc);

	//	If we have a specific scale, then use it.

	if (rScale != 1.0)
		{
		Metric rX = xDest + (cxDest / 2);
		Metric rY = yDest + (cyDest / 2);

		CGDraw::BltTransformed(Dest, rX, rY, rScale, rScale, 0.0, Src, rcSrc.left, rcSrc.top, cxSrc, cySrc);
		}

	//	If the image fits, paint it at the normal size.

	else if (cxSrc <= cxDest && cySrc <= cyDest)
		{
		Dest.Blt(rcSrc.left,
			rcSrc.top,
			cxSrc,
			cySrc,
			255,
			Src,
			xDest + (cxDest - cxSrc) / 2,
			yDest + (cyDest - cySrc) / 2);
		}

	//	Otherwise we need to resize it.

	else
		{
		Metric rScale = Min((Metric)cxDest / (Metric)cxSrc, (Metric)cyDest / (Metric)cySrc);
		Metric rX = xDest + (cxDest / 2);
		Metric rY = yDest + (cyDest / 2);

		CGDraw::BltTransformed(Dest, rX, rY, rScale, rScale, 0.0, Src, rcSrc.left, rcSrc.top, cxSrc, cySrc);
		}
	}

void CPaintHelper::PaintStatusBar (CG32bitImage &Dest, int x, int y, int iTick, CG32bitPixel rgbColor, const CString &sLabel, int iPos, int iMaxPos, int *retcyHeight)

//	PaintStatusBar
//
//	Paints a status bar

	{
	if (iMaxPos == 0)
		return;

	CG32bitPixel rgbNormal = CG32bitPixel(rgbColor, 220);
	CG32bitPixel rgbDark = CG32bitPixel(rgbColor, 128);
	CG32bitPixel rgbBlack = CG32bitPixel(CG32bitPixel(0), 128);

	int xStart = x - (STATUS_BAR_WIDTH / 2);
	int yStart = y;

	int iFill = Max(1, iPos * STATUS_BAR_WIDTH / iMaxPos);

	//	Draw

	Dest.Fill(xStart, yStart + 1, iFill, STATUS_BAR_HEIGHT - 2, rgbNormal);
	Dest.Fill(xStart + iFill, yStart + 1, STATUS_BAR_WIDTH - iFill, STATUS_BAR_HEIGHT - 2, rgbBlack);

	Dest.DrawLine(xStart, yStart, xStart + STATUS_BAR_WIDTH, yStart, 1, rgbDark);
	Dest.DrawLine(xStart, yStart + STATUS_BAR_HEIGHT - 1, xStart + STATUS_BAR_WIDTH, yStart + STATUS_BAR_HEIGHT - 1, 1, rgbDark);
	Dest.DrawLine(xStart - 1, yStart, xStart - 1, yStart + STATUS_BAR_HEIGHT, 1, rgbDark);
	Dest.DrawLine(xStart + STATUS_BAR_WIDTH, yStart, xStart + STATUS_BAR_WIDTH, yStart + STATUS_BAR_HEIGHT, 1, rgbDark);

	//	Draw the label

	if (!sLabel.IsBlank())
		{
		const CG16bitFont &Font = g_pUniverse->GetNamedFont(CUniverse::fontSRSObjCounter);
		CG32bitPixel rgbLabelColor = CG32bitPixel(255, 255, 255);
		Font.DrawText(Dest, x, y, rgbLabelColor, sLabel, CG16bitFont::AlignCenter);
		}

	//	Return height

	if (retcyHeight)
		*retcyHeight = STATUS_BAR_HEIGHT;
	}

void CPaintHelper::PaintTargetHighlight (CG32bitImage &Dest, int x, int y, int iTick, int iRadius, int iRingSpacing, int iDelay, CG32bitPixel rgbColor)

//	PaintTargetHighlight
//
//	Paints an animating target highlight.

	{
	ASSERT(iDelay >= 1);

	int iExpand = ((iTick / iDelay) % iRingSpacing);
	int iOpacityStep = iExpand * (80 / iRingSpacing);
	CGDraw::RingGlowing(Dest, x, y, iRadius, 6, rgbColor);
	CGDraw::RingGlowing(Dest, x, y, iRadius + iExpand, 3, CG32bitPixel(rgbColor, (BYTE)(240 - iOpacityStep)));
	CGDraw::RingGlowing(Dest, x, y, iRadius + iRingSpacing + iExpand, 2, CG32bitPixel(rgbColor, (BYTE)(160 - iOpacityStep)));
	CGDraw::RingGlowing(Dest, x, y, iRadius + 2 * iRingSpacing + iExpand, 1, CG32bitPixel(rgbColor, (BYTE)(80 - iOpacityStep)));
	}
