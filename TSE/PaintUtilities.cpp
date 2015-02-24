//	PaintUtilities.cpp
//
//	Paint Utility classes

#include "PreComp.h"

SPoint g_BlasterShape[8] = 
	{
		{    0,    0	},
		{   30,    6	},
		{   50,   20	},
		{   35,   50	},
		{    0,  100	},
		{  -35,   50	},
		{  -50,   20	},
		{  -30,    6	},
	};

void CreateBlasterShape (int iAngle, int iLength, int iWidth, SPoint *Poly)

//	CreateBlasterShape
//
//	Creates a blaster-shaped polygon

	{
	//	Define a transformation for this shape

	CXForm Trans(xformScale, ((Metric)iWidth)/100.0, ((Metric)iLength)/100.0);
	Trans = Trans * CXForm(xformRotate, iAngle + 270);

	//	Generate the points

	for (int i = 0; i < 8; i++)
		{
		Metric x, y;
		Trans.Transform(g_BlasterShape[i].x, g_BlasterShape[i].y, &x, &y);
		Poly[i].x = (int)x;
		Poly[i].y = (int)y;
		}
	}

void ComputeLightningPoints (int iCount, Metric *pxPoint, Metric *pyPoint, Metric rChaos)

//	ComputeLightningPoints
//
//	Computes points for lightning using a simple fractal algorithm. We assume
//	that pxPoint[0],pyPoint[0] is the starting point and pxPoint[iCount-1],pyPoint[iCount-1]
//	is the ending point.

	{
	ASSERT(iCount > 2);

	//	Half the delta

	Metric dx2 = (pxPoint[iCount-1] - pxPoint[0]) / 2;
	Metric dy2 = (pyPoint[iCount-1] - pyPoint[0]) / 2;

	//	Center point

	Metric xCenter = pxPoint[0] + dx2;
	Metric yCenter = pyPoint[0] + dy2;

	//	Fractal offset

	Metric rOffset = (mathRandom(-100, 100) / 100.0) * rChaos;

	//	Index of middle of array

	int iMiddle = iCount / 2;

	pxPoint[iMiddle] = xCenter + dy2 * rOffset;
	pyPoint[iMiddle] = yCenter - dx2 * rOffset;

	//	Recurse

	if (iMiddle > 1)
		ComputeLightningPoints(iMiddle+1, pxPoint, pyPoint, rChaos);

	if (iCount - iMiddle > 2)
		ComputeLightningPoints(iCount - iMiddle, pxPoint + iMiddle, pyPoint + iMiddle, rChaos);
	}

void ComputeLightningPoints (int iCount, CVector *pPoints, Metric rChaos)

//	ComputeLightningPoints
//
//	Computes points for lightning using a simple fractal algorithm. We assume
//	that pxPoint[0],pyPoint[0] is the starting point and pxPoint[iCount-1],pyPoint[iCount-1]
//	is the ending point.

	{
	ASSERT(iCount > 2);

	//	Line

	CVector vLine = (pPoints[iCount - 1] - pPoints[0]);

	//	Half the line

	CVector vLineHalf = vLine / 2.0;

	//	Perpendicular half-line

	CVector vDelta = vLineHalf.Perpendicular();

	//	Center point

	CVector vCenter = pPoints[0] + vLineHalf;

	//	Perturb the line

	Metric rOffset = (mathRandom(-100, 100) / 100.0) * rChaos;

	int iMiddle = iCount / 2;
	pPoints[iMiddle] = vCenter + vDelta * rOffset;

	//	Recurse

	if (iMiddle > 1)
		ComputeLightningPoints(iMiddle+1, pPoints, rChaos);

	if (iCount - iMiddle > 2)
		ComputeLightningPoints(iCount - iMiddle, pPoints + iMiddle, rChaos);
	}

void DrawItemTypeIcon (CG32bitImage &Dest, int x, int y, CItemType *pType, int cxSize, int cySize, bool bGray)

//	DrawItemTypeIcon
//
//	Draws the item type icon at the given position

	{
	const CObjectImageArray &Image = pType->GetImage();
	if (Image.IsLoaded())
		{
		RECT rcImage = Image.GetImageRect();

		if (cxSize <= 0 || cySize <= 0)
			{
			if (bGray)
				CGDraw::BltGray(Dest,
						x,
						y,
						Image.GetImage(NULL_STR),
						rcImage.left,
						rcImage.top,
						RectWidth(rcImage),
						RectHeight(rcImage),
						128);
			else
				Dest.Blt(rcImage.left,
						rcImage.top,
						RectWidth(rcImage),
						RectHeight(rcImage),
						255,
						Image.GetImage(NULL_STR),
						x,
						y);
			}
		else
			{
			if (bGray)
				CGDraw::BltTransformedGray(Dest,
						x + (cxSize / 2),
						y + (cySize / 2),
						(Metric)cxSize / (Metric)RectWidth(rcImage),
						(Metric)cySize / (Metric)RectHeight(rcImage),
						0.0,
						Image.GetImage(NULL_STR),
						rcImage.left,
						rcImage.top,
						RectWidth(rcImage),
						RectHeight(rcImage),
						128);
			else
				CGDraw::BltTransformed(Dest,
						x + (cxSize / 2),
						y + (cySize / 2),
						(Metric)cxSize / (Metric)RectWidth(rcImage),
						(Metric)cySize / (Metric)RectHeight(rcImage),
						0.0,
						Image.GetImage(NULL_STR),
						rcImage.left,
						rcImage.top,
						RectWidth(rcImage),
						RectHeight(rcImage));
			}
		}
	}

void DrawLightning (CG32bitImage &Dest,
					int xFrom, int yFrom,
					int xTo, int yTo,
					CG32bitPixel rgbColor,
					int iPoints,
					Metric rChaos)

//	DrawLightning
//
//	Draw a lightning line

	{
	ASSERT(iPoints >= 0);

	Metric *pxPos = new Metric [iPoints];
	Metric *pyPos = new Metric [iPoints];

	pxPos[0] = xFrom;
	pyPos[0] = yFrom;
	pxPos[iPoints-1] = xTo;
	pyPos[iPoints-1] = yTo;

	ComputeLightningPoints(iPoints, pxPos, pyPos, rChaos);

	//	Draw lightning

	for (int i = 0; i < iPoints-1; i++)
		{
		Dest.DrawLine((int)pxPos[i], (int)pyPos[i],
				(int)pxPos[i+1], (int)pyPos[i+1],
				1,
				rgbColor);
		}

	//	Done

	delete [] pxPos;
	delete [] pyPos;
	}

void DrawParticle (CG32bitImage &Dest,
				   int x, int y,
				   CG32bitPixel rgbColor,
				   int iSize,
				   DWORD byOpacity)

//	DrawParticle
//
//	Draws a single particle

	{
	DWORD byOpacity2 = byOpacity / 2;

	switch (iSize)
		{
		case 0:
			Dest.SetPixelTrans(x, y, rgbColor, (BYTE)byOpacity);
			break;

		case 1:
			Dest.SetPixelTrans(x, y, rgbColor, (BYTE)byOpacity);
			Dest.SetPixelTrans(x + 1, y, rgbColor, (BYTE)byOpacity2);
			Dest.SetPixelTrans(x, y + 1, rgbColor, (BYTE)byOpacity2);
			break;

		case 2:
			Dest.SetPixelTrans(x, y, rgbColor, (BYTE)byOpacity);
			Dest.SetPixelTrans(x + 1, y, rgbColor, (BYTE)byOpacity2);
			Dest.SetPixelTrans(x, y + 1, rgbColor, (BYTE)byOpacity2);
			Dest.SetPixelTrans(x - 1, y, rgbColor, (BYTE)byOpacity2);
			Dest.SetPixelTrans(x, y - 1, rgbColor, (BYTE)byOpacity2);
			break;

		case 3:
			Dest.SetPixelTrans(x, y, rgbColor, (BYTE)byOpacity);
			Dest.SetPixelTrans(x + 1, y, rgbColor, (BYTE)byOpacity);
			Dest.SetPixelTrans(x, y + 1, rgbColor, (BYTE)byOpacity);
			Dest.SetPixelTrans(x - 1, y, rgbColor, (BYTE)byOpacity);
			Dest.SetPixelTrans(x, y - 1, rgbColor, (BYTE)byOpacity);
			Dest.SetPixelTrans(x + 1, y + 1, rgbColor, (BYTE)byOpacity2);
			Dest.SetPixelTrans(x + 1, y - 1, rgbColor, (BYTE)byOpacity2);
			Dest.SetPixelTrans(x - 1, y + 1, rgbColor, (BYTE)byOpacity2);
			Dest.SetPixelTrans(x - 1, y - 1, rgbColor, (BYTE)byOpacity2);
			break;

		case 4:
		default:
			{
			Dest.Fill(x - 1,
					y - 1,
					3,
					3,
					CG32bitPixel(rgbColor, (BYTE)byOpacity));

			Dest.SetPixelTrans(x + 2, y, rgbColor, (BYTE)byOpacity2);
			Dest.SetPixelTrans(x, y + 2, rgbColor, (BYTE)byOpacity2);
			Dest.SetPixelTrans(x - 2, y, rgbColor, (BYTE)byOpacity2);
			Dest.SetPixelTrans(x, y - 2, rgbColor, (BYTE)byOpacity2);
			}
		}
	}
