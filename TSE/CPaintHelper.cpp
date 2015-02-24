//	CPaintHelper.cpp
//
//	CPaintHelper class
//	Copyright (c) 2014 by Kronosaur Productions, LLC.

#include "PreComp.h"

const int ARROW_WIDTH =							6;

const int STATUS_BAR_WIDTH =					100;
const int STATUS_BAR_HEIGHT =					15;


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

void CPaintHelper::PaintStatusBar (CG32bitImage &Dest, int x, int y, int iTick, CG32bitPixel rgbColor, const CString &sLabel, int iPos, int iMaxPos, int *retcyHeight)

//	PaintStatusBar
//
//	Paints a status bar

	{
	if (iMaxPos == 0)
		return;

	CG32bitPixel rgbNormal = CG32bitPixel(rgbColor, (BYTE)220);
	CG32bitPixel rgbDark = CG32bitPixel(rgbColor, (BYTE)128);
	CG32bitPixel rgbBlack = CG32bitPixel(CG32bitPixel(0), (BYTE)128);

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
