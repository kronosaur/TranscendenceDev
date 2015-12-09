//	CGDraw.cpp
//
//	CGDraw Class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CGDraw::Arc (CG32bitImage &Dest, const CVector &vCenter, Metric rRadius, Metric rStartAngle, Metric rEndAngle, Metric rArcWidth, CG32bitPixel rgbColor, EBlendModes iMode, int iSpacing, DWORD dwFlags)

//	Arc
//
//	Draws an arc

	{
	//	Create the shape

	CGPath ArcPath;
	if (!CGShape::Arc(CVector(), rRadius, rStartAngle, rEndAngle, rArcWidth, &ArcPath, iSpacing, dwFlags))
		return;

	//	Rasterize into a region

	CGRegion ArcRegion;
	ArcPath.Rasterize(&ArcRegion);
	
	//	Draw the region

	CGDraw::Region(Dest, (int)vCenter.GetX(), (int)vCenter.GetY(), ArcRegion, rgbColor, iMode);
	}

void CGDraw::Arc (CG32bitImage &Dest, int xCenter, int yCenter, int iRadius, int iStartAngle, int iEndAngle, int iLineWidth, CG32bitPixel rgbColor, EBlendModes iMode, int iSpacing, DWORD dwFlags)

//	Arc
//
//	Draws an arc

	{
	Arc(Dest, 
			CVector(xCenter, yCenter), 
			(Metric)iRadius, 
			mathDegreesToRadians(iStartAngle), 
			mathDegreesToRadians(iEndAngle), 
			(Metric)iLineWidth, 
			rgbColor, 
			iMode, 
			iSpacing, 
			dwFlags);
	}

void CGDraw::ArcCorner (CG32bitImage &Dest, int xCenter, int yCenter, int iRadius, int iStartAngle, int iEndAngle, int iLineWidth, CG32bitPixel rgbColor)

//	Arc
//
//	Draw an arc (90 degrees). 

	{
	//	Temporaries

	int iHalfWidth = iLineWidth / 2;
	Metric rOuterRadius = iRadius + iHalfWidth;
	Metric rInnerRadius = rOuterRadius - iLineWidth;
	Metric rOuterRadius2 = rOuterRadius * rOuterRadius;
	Metric rInnerRadius2 = rInnerRadius * rInnerRadius;

	//	Figure out which quadrants we paint

	bool bUpperRight;
	bool bUpperLeft;
	bool bLowerLeft;
	bool bLowerRight;
	if (iStartAngle <= iEndAngle)
		{
		bUpperRight = (iStartAngle < 90);
		bUpperLeft = (iStartAngle < 180) && (iEndAngle > 90);
		bLowerLeft = (iStartAngle < 270) && (iEndAngle > 180);
		bLowerRight = (iStartAngle < 360) && (iEndAngle > 270);
		}
	else
		{
		bUpperRight = (iEndAngle > 0);
		bUpperLeft = (iEndAngle > 90) || (iStartAngle < 180);
		bLowerLeft = (iEndAngle > 180) || (iStartAngle < 270);
		bLowerRight = (iEndAngle > 270) || (iStartAngle < 360);
		}

	//	Iterate from the center up (and use symmetry for the four quadrants)

	int iRow = 0;
	Metric rRow = 0.5;
	Metric rRowEnd = rOuterRadius;
	while (rRow < rRowEnd)
		{
		Metric rRow2 = rRow * rRow;

		Metric rOuterLen = sqrt(rOuterRadius2 - rRow2);
		Metric rInnerLen = (rRow < rInnerRadius ? sqrt(rInnerRadius2 - rRow2) : 0.0);
		Metric rWidth = rOuterLen - rInnerLen;

		int iSolidOuter = (int)rOuterLen;
		Metric rOuterFraction = rOuterLen - (Metric)iSolidOuter;
		int iSolidWidth = (int)(rWidth - rOuterFraction);
		Metric rInnerFraction = (rWidth - rOuterFraction) - (Metric)iSolidWidth;

		//	If we have a solid width, then paint the solid part of the edge

		if (iSolidWidth > 0)
			{
			if (bUpperRight)
				Dest.FillLine(xCenter + iSolidOuter - iSolidWidth, yCenter - 1 - iRow, iSolidWidth, rgbColor);
			if (bLowerRight)
				Dest.FillLine(xCenter + iSolidOuter - iSolidWidth, yCenter + iRow, iSolidWidth, rgbColor);
			if (bUpperLeft)
				Dest.FillLine(xCenter - iSolidOuter, yCenter - 1 - iRow, iSolidWidth, rgbColor);
			if (bLowerLeft)
				Dest.FillLine(xCenter - iSolidOuter, yCenter + iRow, iSolidWidth, rgbColor);
			}

		//	Paint the edges

		BYTE byOuterEdge = (BYTE)(DWORD)(255 * rOuterFraction);
		if (byOuterEdge)
			{
			if (bUpperRight)
				Dest.SetPixelTrans(xCenter + iSolidOuter, yCenter - 1 - iRow, rgbColor, byOuterEdge);
			if (bLowerRight)
				Dest.SetPixelTrans(xCenter + iSolidOuter, yCenter + iRow, rgbColor, byOuterEdge);
			if (bUpperLeft)
				Dest.SetPixelTrans(xCenter - iSolidOuter - 1, yCenter - 1 - iRow, rgbColor, byOuterEdge);
			if (bLowerLeft)
				Dest.SetPixelTrans(xCenter - iSolidOuter - 1, yCenter + iRow, rgbColor, byOuterEdge);
			}

		BYTE byInnerEdge = (BYTE)(DWORD)(255 * rInnerFraction);
		if (byInnerEdge)
			{
			if (bUpperRight)
				Dest.SetPixelTrans(xCenter + iSolidOuter - iSolidWidth - 1, yCenter - 1 - iRow, rgbColor, byInnerEdge);
			if (bLowerRight)
				Dest.SetPixelTrans(xCenter + iSolidOuter - iSolidWidth - 1, yCenter + iRow, rgbColor, byInnerEdge);
			if (bUpperLeft)
				Dest.SetPixelTrans(xCenter - iSolidOuter + iSolidWidth, yCenter - 1 - iRow, rgbColor, byInnerEdge);
			if (bLowerLeft)
				Dest.SetPixelTrans(xCenter - iSolidOuter + iSolidWidth, yCenter + iRow, rgbColor, byInnerEdge);
			}

		//	Next

		iRow++;
		rRow = (Metric)iRow + 0.5;
		}
	}

void CGDraw::ArcSegment (CG32bitImage &Dest, const CVector &vCenter, Metric rRadius, Metric rAngle, Metric rWidth, CG32bitPixel rgbColor, EBlendModes iMode)

//	ArcSegment
//
//	Draws a circle segment (the area defined by a chord).

	{
	CGPath ArcPath;
	if (!CGShape::ArcSegment(CVector(), rRadius, rAngle, rWidth, &ArcPath))
		return;

	//	Rasterize into a region

	CGRegion ArcRegion;
	ArcPath.Rasterize(&ArcRegion);
	
	//	Draw the region

	CGDraw::Region(Dest, (int)vCenter.GetX(), (int)vCenter.GetY(), ArcRegion, rgbColor, iMode);
	}

void CGDraw::ArcQuadrilateral (CG32bitImage &Dest, const CVector &vCenter, const CVector &vInnerPos, const CVector &vOuterPos, Metric rWidth, CG32bitPixel rgbColor, EBlendModes iMode)

//	ArcQuadrilateral
//
//	Draw a quadrilateral in which two sides are straight and two sides are 
//	curved with respect to a center point.

	{
	CGPath ArcPath;
	if (!CGShape::ArcQuadrilateral(CVector(), vInnerPos, vOuterPos, rWidth, &ArcPath))
		return;

	//	Rasterize into a region

	CGRegion ArcRegion;
	ArcPath.Rasterize(&ArcRegion);
	
	//	Draw the region

	CGDraw::Region(Dest, (int)vCenter.GetX(), (int)vCenter.GetY(), ArcRegion, rgbColor, iMode);
	}

void CGDraw::LineBresenham (CG32bitImage &Dest, int x1, int y1, int x2, int y2, int iWidth, CG32bitPixel rgbColor)

//	LineBresenham
//
//	Draws an anti-aliased line

	{
	//	Calculate the slope

	int dx = x2 - x1;
	int ax = Absolute(dx) * 2;
	int sx = (dx > 0 ? 1 : -1);

	int dy = y2 - y1;
	int ay = Absolute(dy) * 2;
	int sy = (dy > 0 ? 1 : -1);

	//	Handle special cases

	if (dx == 0 && dy == 0)
		{
		Dest.SetPixel(x1, y1, rgbColor);
		return;
		}

	const RECT &rcClip = Dest.GetClipRect();

	//	Do it

	int i, x, y;

	if (ax > ay)		//	x-axis dominant
		{
		//	Compute the distance from the mid-point of the line to
		//	the top edge along the y-axis.

		double rHalfHeight = (double)iWidth * sqrt((double)(dx*dx + dy*dy)) / (double)(2 * abs(dx));
		int cyHalfCount = (int)(((2 * rHalfHeight) - 1) / 2);
		int cyCount = 2 * cyHalfCount + 1;

		int d = ay - ax / 2;

		//	Compute information for anti-aliasing. rDist is the distance
		//	from the top solid point to the edge of the line. We update
		//	the value as we proceed along the x-axis and use it for
		//	anti-aliasing the edge of the line. A negative value means
		//	that the top point is outside the edge.

		double rSlope = (double)dy / (double)dx;
		double rDistTop = rHalfHeight - (double)cyHalfCount - 0.25;
		double rDistBottom = rDistTop;

		double rDistTopInc = sy;
		double rDistTopDec = rSlope * sx;
		double rDistBottomInc = rSlope * sx;
		double rDistBottomDec = sy;

		//	Draw!

		for (x = x1, y = y1; x != x2; x += sx)
			{
			int yTop = y - cyHalfCount;

			//	Draw anti-aliasing above the line

			if (rDistTop > 0)
				Dest.SetPixelTrans(x, yTop - 1, rgbColor, (BYTE)(255.0 * (rDistTop > 1.0 ? 1.0 : rDistTop)));

			//	Draw the solid part of the line

			if (x >= rcClip.left && x < rcClip.right)
				{
				CG32bitPixel *pPos = Dest.GetPixelPos(x, yTop);

				for (i = yTop; i < yTop + cyCount; i++)
					{
					if (i >= rcClip.top && i < rcClip.bottom)
						*pPos = rgbColor;

					pPos = Dest.NextRow(pPos);
					}
				}

			//	Draw anti-aliasing below the line

			if (rDistBottom > 0)
				Dest.SetPixelTrans(x, yTop + cyCount, rgbColor, (BYTE)(255.0 * (rDistBottom > 1.0 ? 1.0 : rDistBottom)));

			//	Next point

			if (d >= 0)
				{
				y = y + sy;
				d = d - ax;
				rDistTop += rDistTopInc;
				rDistBottom -= rDistBottomDec;
				}

			d = d + ay;
			rDistTop -= rDistTopDec;
			rDistBottom += rDistBottomInc;
			}
		}
	else				//	y-axis dominant
		{
		//	Compute the distance from the mid-point of the line to
		//	the left-edge of the x-axis

		double rHalfWidth = (double)iWidth * sqrt((double)(dx*dx + dy*dy)) / (double)(2 * abs(dy));
		int cxHalfCount = (int)(((2 * rHalfWidth) - 1) / 2);
		int cxCount = 2 * cxHalfCount + 1;

		int d = ax - ay / 2;

		//	Compute information for anti-aliasing. rDist is the distance
		//	from the top solid point to the edge of the line. We update
		//	the value as we proceed along the x-axis and use it for
		//	anti-aliasing the edge of the line. A negative value means
		//	that the top point is outside the edge.

		double rSlope = (double)dx / (double)dy;
		double rDistLeft = rHalfWidth - (double)cxHalfCount - 0.25;
		double rDistRight = rDistLeft;

		double rDistLeftInc = sx;
		double rDistRightDec = sx;
		double rDistLeftDec = rSlope * sy;
		double rDistRightInc = rSlope * sy;

		//	Draw!

		for (y = y1, x = x1; y != y2; y += sy)
			{
			int xTop = x - cxHalfCount;

			//	Draw anti-aliasing to the left

			if (rDistLeft > 0)
				Dest.SetPixelTrans(xTop - 1, y, rgbColor, (BYTE)(255.0 * (rDistLeft > 1.0 ? 1.0 : rDistLeft)));

			//	Draw the solid part of the line

			if (y >= rcClip.top && y < rcClip.bottom)
				{
				CG32bitPixel *pPos = Dest.GetPixelPos(xTop, y);

				for (i = xTop; i < xTop + cxCount; i++)
					if (i >= rcClip.left && i < rcClip.right)
						*pPos++ = rgbColor;
					else
						pPos++;
				}

			//	Draw anti-aliasing to the right of the line

			if (rDistRight > 0)
				Dest.SetPixelTrans(xTop + cxCount, y, rgbColor, (BYTE)(255.0 * (rDistRight > 1.0 ? 1.0 : rDistRight)));

			//	Next point

			if (d >= 0)
				{
				x = x + sx;
				d = d - ay;
				rDistLeft += rDistLeftInc;
				rDistRight -= rDistRightDec;
				}

			d = d + ax;
			rDistLeft -= rDistLeftDec;
			rDistRight += rDistRightInc;
			}
		}
	}

void CGDraw::LineBresenhamGradient (CG32bitImage &Dest, int x1, int y1, int x2, int y2, int iWidth, CG32bitPixel rgbColor1, CG32bitPixel rgbColor2)

//	LineBresenhamGradient
//
//	Paints a gradient line.

	{
	//	Calculate the slope

	int dx = x2 - x1;
	int ax = Absolute(dx) * 2;
	int sx = (dx > 0 ? 1 : -1);

	int dy = y2 - y1;
	int ay = Absolute(dy) * 2;
	int sy = (dy > 0 ? 1 : -1);

	//	Handle special cases

	if (dx == 0 && dy == 0)
		{
		Dest.SetPixel(x1, y1, rgbColor1);
		return;
		}

	//	Compute color fading scale

	int iRedStart = rgbColor1.GetRed();
	int iRedScale = rgbColor2.GetRed() - iRedStart;
	int iGreenStart = rgbColor1.GetGreen();
	int iGreenScale = rgbColor2.GetGreen() - iGreenStart;
	int iBlueStart = rgbColor1.GetBlue();
	int iBlueScale = rgbColor2.GetBlue() - iBlueStart;

	const RECT &rcClip = Dest.GetClipRect();

	//	Do it

	int i, x, y;

	if (ax > ay)		//	x-axis dominant
		{
		//	Compute the distance from the mid-point of the line to
		//	the top edge along the y-axis.

		double rHalfHeight = (double)iWidth * sqrt((double)(dx*dx + dy*dy)) / (double)(2 * abs(dx));
		int cyHalfCount = (int)(((2 * rHalfHeight) - 1) / 2);
		int cyCount = 2 * cyHalfCount + 1;

		int d = ay - ax / 2;

		//	Compute information for anti-aliasing. rDist is the distance
		//	from the top solid point to the edge of the line. We update
		//	the value as we proceed along the x-axis and use it for
		//	anti-aliasing the edge of the line. A negative value means
		//	that the top point is outside the edge.

		double rSlope = (double)dy / (double)dx;
		double rDistTop = rHalfHeight - (double)cyHalfCount - 0.25;
		double rDistBottom = rDistTop;

		//	Compute the information for fading the color from wColor1
		//	to wColor2.

		int iSteps = dx * sx;

		//	Draw!

		for (x = x1, y = y1; x != x2; x += sx)
			{
			int iStep = (x - x1) * sx;
			CG32bitPixel rgbColor = CG32bitPixel((BYTE)(iRedStart + (iStep * iRedScale / iSteps)),
					(BYTE)(iGreenStart + (iStep * iGreenScale / iSteps)),
					(BYTE)(iBlueStart + (iStep * iBlueScale / iSteps)));

			int yTop = y - cyHalfCount;

			//	Draw anti-aliasing above the line

			if (rDistTop > 0)
				Dest.SetPixelTrans(x, yTop - 1, rgbColor, (BYTE)(255.0 * (rDistTop > 1.0 ? 1.0 : rDistTop)));

			//	Draw the solid part of the line

			if (x >= rcClip.left && x < rcClip.right)
				{
				CG32bitPixel *pPos = Dest.GetPixelPos(x, yTop);

				for (i = yTop; i < yTop + cyCount; i++)
					{
					if (i >= rcClip.top && i < rcClip.bottom)
						*pPos = rgbColor;

					pPos = Dest.NextRow(pPos);
					}
				}

			//	Draw anti-aliasing below the line

			if (rDistBottom > 0)
				Dest.SetPixelTrans(x, yTop + cyCount, rgbColor, (BYTE)(255.0 * (rDistBottom > 1.0 ? 1.0 : rDistBottom)));

			//	Next point

			if (d >= 0)
				{
				y = y + sy;
				d = d - ax;
				rDistTop += sy;
				rDistBottom -= sy;
				}

			d = d + ay;
			rDistTop -= rSlope * sx;
			rDistBottom += rSlope * sx;
			}
		}
	else				//	y-axis dominant
		{
		//	Compute the distance from the mid-point of the line to
		//	the left-edge of the x-axis

		double rHalfWidth = (double)iWidth * sqrt((double)(dx*dx + dy*dy)) / (double)(2 * abs(dy));
		int cxHalfCount = (int)(((2 * rHalfWidth) - 1) / 2);
		int cxCount = 2 * cxHalfCount + 1;

		int d = ax - ay / 2;

		//	Compute information for anti-aliasing. rDist is the distance
		//	from the top solid point to the edge of the line. We update
		//	the value as we proceed along the x-axis and use it for
		//	anti-aliasing the edge of the line. A negative value means
		//	that the top point is outside the edge.

		double rSlope = (double)dx / (double)dy;
		double rDistLeft = rHalfWidth - (double)cxHalfCount - 0.25;
		double rDistRight = rDistLeft;

		//	Compute the information for fading the color from wColor1
		//	to wColor2.

		int iSteps = dy * sy;

		//	Draw!

		for (y = y1, x = x1; y != y2; y += sy)
			{
			int iStep = (y - y1) * sy;
			CG32bitPixel rgbColor = CG32bitPixel((BYTE)(iRedStart + (iStep * iRedScale / iSteps)),
					(BYTE)(iGreenStart + (iStep * iGreenScale / iSteps)),
					(BYTE)(iBlueStart + (iStep * iBlueScale / iSteps)));

			int xTop = x - cxHalfCount;

			//	Draw anti-aliasing to the left

			if (rDistLeft > 0)
				Dest.SetPixelTrans(xTop - 1, y, rgbColor, (BYTE)(255.0 * (rDistLeft > 1.0 ? 1.0 : rDistLeft)));

			//	Draw the solid part of the line

			if (y >= rcClip.top && y < rcClip.bottom)
				{
				CG32bitPixel *pPos = Dest.GetPixelPos(xTop, y);

				for (i = xTop; i < xTop + cxCount; i++)
					if (i >= rcClip.left && i < rcClip.right)
						*pPos++ = rgbColor;
					else
						pPos++;
				}

			//	Draw anti-aliasing to the right of the line

			if (rDistRight > 0)
				Dest.SetPixelTrans(xTop + cxCount, y, rgbColor, (BYTE)(255.0 * (rDistRight > 1.0 ? 1.0 : rDistRight)));

			//	Next point

			if (d >= 0)
				{
				x = x + sx;
				d = d - ay;
				rDistLeft += sx;
				rDistRight -= sx;
				}

			d = d + ax;
			rDistLeft -= rSlope * sy;
			rDistRight += rSlope * sy;
			}
		}
	}

void CGDraw::LineBresenhamGradientTrans (CG32bitImage &Dest, int x1, int y1, int x2, int y2, int iWidth, CG32bitPixel rgbColor1, CG32bitPixel rgbColor2)

//	LineBresenhamGradientTrans
//
//	Paints a gradient line in which rgbColor1 or rgbColor2 are transparent.

	{
	//	Calculate the slope

	int dx = x2 - x1;
	int ax = Absolute(dx) * 2;
	int sx = (dx > 0 ? 1 : -1);

	int dy = y2 - y1;
	int ay = Absolute(dy) * 2;
	int sy = (dy > 0 ? 1 : -1);

	//	Handle special cases

	if (dx == 0 && dy == 0)
		{
		Dest.SetPixel(x1, y1, rgbColor1);
		return;
		}

	//	Compute color fading scale

	int iRedStart = rgbColor1.GetRed();
	int iRedScale = rgbColor2.GetRed() - iRedStart;
	int iGreenStart = rgbColor1.GetGreen();
	int iGreenScale = rgbColor2.GetGreen() - iGreenStart;
	int iBlueStart = rgbColor1.GetBlue();
	int iBlueScale = rgbColor2.GetBlue() - iBlueStart;
	int iAlphaStart = rgbColor1.GetAlpha();
	int iAlphaScale = rgbColor2.GetAlpha() - iAlphaStart;

	const RECT &rcClip = Dest.GetClipRect();

	//	Do it

	int i, x, y;

	if (ax > ay)		//	x-axis dominant
		{
		//	Compute the distance from the mid-point of the line to
		//	the top edge along the y-axis.

		double rHalfHeight = (double)iWidth * sqrt((double)(dx*dx + dy*dy)) / (double)(2 * abs(dx));
		int cyHalfCount = (int)(((2 * rHalfHeight) - 1) / 2);
		int cyCount = 2 * cyHalfCount + 1;

		int d = ay - ax / 2;

		//	Compute information for anti-aliasing. rDist is the distance
		//	from the top solid point to the edge of the line. We update
		//	the value as we proceed along the x-axis and use it for
		//	anti-aliasing the edge of the line. A negative value means
		//	that the top point is outside the edge.

		double rSlope = (double)dy / (double)dx;
		double rDistTop = rHalfHeight - (double)cyHalfCount - 0.25;
		double rDistBottom = rDistTop;

		//	Compute the information for fading the color from wColor1
		//	to wColor2.

		int iSteps = dx * sx;

		//	Draw!

		for (x = x1, y = y1; x != x2; x += sx)
			{
			int iStep = (x - x1) * sx;
			CG32bitPixel rgbColor = CG32bitPixel((BYTE)(iRedStart + (iStep * iRedScale / iSteps)),
					(BYTE)(iGreenStart + (iStep * iGreenScale / iSteps)),
					(BYTE)(iBlueStart + (iStep * iBlueScale / iSteps)));

			BYTE byAlpha = (BYTE)(iAlphaStart + (iStep * iAlphaScale / iSteps));

			int yTop = y - cyHalfCount;

			//	Draw anti-aliasing above the line

			if (rDistTop > 0)
				{
				BYTE byAlphaComp = CG32bitPixel::BlendAlpha((BYTE)(255.0 * (rDistTop > 1.0 ? 1.0 : rDistTop)), byAlpha);
				Dest.SetPixelTrans(x, yTop - 1, rgbColor, byAlphaComp);
				}

			//	Draw the solid part of the line

			if (x >= rcClip.left && x < rcClip.right)
				{
				CG32bitPixel *pPos = Dest.GetPixelPos(x, yTop);

				for (i = yTop; i < yTop + cyCount; i++)
					{
					if (i >= rcClip.top && i < rcClip.bottom)
						*pPos = CG32bitPixel::Blend(*pPos, rgbColor, byAlpha);

					pPos = Dest.NextRow(pPos);
					}
				}

			//	Draw anti-aliasing below the line

			if (rDistBottom > 0)
				{
				BYTE byAlphaComp = CG32bitPixel::BlendAlpha((BYTE)(255.0 * (rDistBottom > 1.0 ? 1.0 : rDistBottom)), byAlpha);
				Dest.SetPixelTrans(x, yTop + cyCount, rgbColor, byAlphaComp);
				}

			//	Next point

			if (d >= 0)
				{
				y = y + sy;
				d = d - ax;
				rDistTop += sy;
				rDistBottom -= sy;
				}

			d = d + ay;
			rDistTop -= rSlope * sx;
			rDistBottom += rSlope * sx;
			}
		}
	else				//	y-axis dominant
		{
		//	Compute the distance from the mid-point of the line to
		//	the left-edge of the x-axis

		double rHalfWidth = (double)iWidth * sqrt((double)(dx*dx + dy*dy)) / (double)(2 * abs(dy));
		int cxHalfCount = (int)(((2 * rHalfWidth) - 1) / 2);
		int cxCount = 2 * cxHalfCount + 1;

		int d = ax - ay / 2;

		//	Compute information for anti-aliasing. rDist is the distance
		//	from the top solid point to the edge of the line. We update
		//	the value as we proceed along the x-axis and use it for
		//	anti-aliasing the edge of the line. A negative value means
		//	that the top point is outside the edge.

		double rSlope = (double)dx / (double)dy;
		double rDistLeft = rHalfWidth - (double)cxHalfCount - 0.25;
		double rDistRight = rDistLeft;

		//	Compute the information for fading the color from wColor1
		//	to wColor2.

		int iSteps = dy * sy;

		//	Draw!

		for (y = y1, x = x1; y != y2; y += sy)
			{
			int iStep = (y - y1) * sy;
			CG32bitPixel rgbColor = CG32bitPixel((BYTE)(iRedStart + (iStep * iRedScale / iSteps)),
					(BYTE)(iGreenStart + (iStep * iGreenScale / iSteps)),
					(BYTE)(iBlueStart + (iStep * iBlueScale / iSteps)));

			BYTE byAlpha = (BYTE)(iAlphaStart + (iStep * iAlphaScale / iSteps));

			int xTop = x - cxHalfCount;

			//	Draw anti-aliasing to the left

			if (rDistLeft > 0)
				{
				BYTE byAlphaComp = CG32bitPixel::BlendAlpha((BYTE)(255.0 * (rDistLeft > 1.0 ? 1.0 : rDistLeft)), byAlpha);
				Dest.SetPixelTrans(xTop - 1, y, rgbColor, byAlphaComp);
				}

			//	Draw the solid part of the line

			if (y >= rcClip.top && y < rcClip.bottom)
				{
				CG32bitPixel *pPos = Dest.GetPixelPos(xTop, y);

				for (i = xTop; i < xTop + cxCount; i++)
					{
					if (i >= rcClip.left && i < rcClip.right)
						*pPos = CG32bitPixel::Blend(*pPos, rgbColor, byAlpha);

					pPos++;
					}
				}

			//	Draw anti-aliasing to the right of the line

			if (rDistRight > 0)
				{
				BYTE byAlphaComp = CG32bitPixel::BlendAlpha((BYTE)(255.0 * (rDistRight > 1.0 ? 1.0 : rDistRight)), byAlpha);
				Dest.SetPixelTrans(xTop + cxCount, y, rgbColor, byAlphaComp);
				}

			//	Next point

			if (d >= 0)
				{
				x = x + sx;
				d = d - ay;
				rDistLeft += sx;
				rDistRight -= sx;
				}

			d = d + ax;
			rDistLeft -= rSlope * sy;
			rDistRight += rSlope * sy;
			}
		}
	}

void CGDraw::LineBresenhamTrans (CG32bitImage &Dest, int x1, int y1, int x2, int y2, int iWidth, CG32bitPixel rgbColor)

//	LineBresenhamTrans
//
//	Paints a line using rgbColor's alpha transparency

	{
	BYTE byOpacity = rgbColor.GetAlpha();
	double rOpacity = byOpacity;

	//	Calculate the slope

	int dx = x2 - x1;
	int ax = Absolute(dx) * 2;
	int sx = (dx > 0 ? 1 : -1);

	int dy = y2 - y1;
	int ay = Absolute(dy) * 2;
	int sy = (dy > 0 ? 1 : -1);

	//	Handle special cases

	if (dx == 0 && dy == 0)
		{
		Dest.SetPixelTrans(x1, y1, rgbColor, byOpacity);
		return;
		}

	const RECT &rcClip = Dest.GetClipRect();

	//	Do it

	int i, x, y;

	if (ax > ay)		//	x-axis dominant
		{
		//	Compute the distance from the mid-point of the line to
		//	the top edge along the y-axis.

		double rHalfHeight = (double)iWidth * sqrt((double)(dx*dx + dy*dy)) / (double)(2 * abs(dx));
		int cyHalfCount = (int)(((2 * rHalfHeight) - 1) / 2);
		int cyCount = 2 * cyHalfCount + 1;

		int d = ay - ax / 2;

		//	Compute information for anti-aliasing. rDist is the distance
		//	from the top solid point to the edge of the line. We update
		//	the value as we proceed along the x-axis and use it for
		//	anti-aliasing the edge of the line. A negative value means
		//	that the top point is outside the edge.

		double rSlope = (double)dy / (double)dx;
		double rDistTop = rHalfHeight - (double)cyHalfCount - 0.25;
		double rDistBottom = rDistTop;

		double rDistTopInc = sy;
		double rDistTopDec = rSlope * sx;
		double rDistBottomInc = rSlope * sx;
		double rDistBottomDec = sy;

		//	Draw!

		for (x = x1, y = y1; x != x2; x += sx)
			{
			int yTop = y - cyHalfCount;

			//	Draw anti-aliasing above the line

			if (rDistTop > 0)
				Dest.SetPixelTrans(x, yTop - 1, rgbColor, (BYTE)(rOpacity * (rDistTop > 1.0 ? 1.0 : rDistTop)));

			//	Draw the solid part of the line

			if (x >= rcClip.left && x < rcClip.right)
				{
				CG32bitPixel *pPos = Dest.GetPixelPos(x, yTop);

				for (i = yTop; i < yTop + cyCount; i++)
					{
					if (i >= rcClip.top && i < rcClip.bottom)
						*pPos = CG32bitPixel::Blend(*pPos, rgbColor, byOpacity);

					pPos = Dest.NextRow(pPos);
					}
				}

			//	Draw anti-aliasing below the line

			if (rDistBottom > 0)
				Dest.SetPixelTrans(x, yTop + cyCount, rgbColor, (BYTE)(rOpacity * (rDistBottom > 1.0 ? 1.0 : rDistBottom)));

			//	Next point

			if (d >= 0)
				{
				y = y + sy;
				d = d - ax;
				rDistTop += rDistTopInc;
				rDistBottom -= rDistBottomDec;
				}

			d = d + ay;
			rDistTop -= rDistTopDec;
			rDistBottom += rDistBottomInc;
			}
		}
	else				//	y-axis dominant
		{
		//	Compute the distance from the mid-point of the line to
		//	the left-edge of the x-axis

		double rHalfWidth = (double)iWidth * sqrt((double)(dx*dx + dy*dy)) / (double)(2 * abs(dy));
		int cxHalfCount = (int)(((2 * rHalfWidth) - 1) / 2);
		int cxCount = 2 * cxHalfCount + 1;

		int d = ax - ay / 2;

		//	Compute information for anti-aliasing. rDist is the distance
		//	from the top solid point to the edge of the line. We update
		//	the value as we proceed along the x-axis and use it for
		//	anti-aliasing the edge of the line. A negative value means
		//	that the top point is outside the edge.

		double rSlope = (double)dx / (double)dy;
		double rDistLeft = rHalfWidth - (double)cxHalfCount - 0.25;
		double rDistRight = rDistLeft;

		double rDistLeftInc = sx;
		double rDistRightDec = sx;
		double rDistLeftDec = rSlope * sy;
		double rDistRightInc = rSlope * sy;

		//	Draw!

		for (y = y1, x = x1; y != y2; y += sy)
			{
			int xTop = x - cxHalfCount;

			//	Draw anti-aliasing to the left

			if (rDistLeft > 0)
				Dest.SetPixelTrans(xTop - 1, y, rgbColor, (BYTE)(rOpacity * (rDistLeft > 1.0 ? 1.0 : rDistLeft)));

			//	Draw the solid part of the line

			if (y >= rcClip.top && y < rcClip.bottom)
				{
				CG32bitPixel *pPos = Dest.GetPixelPos(xTop, y);

				for (i = xTop; i < xTop + cxCount; i++)
					if (i >= rcClip.left && i < rcClip.right)
						*pPos = CG32bitPixel::Blend(*pPos, rgbColor, byOpacity);
					else
						pPos++;
				}

			//	Draw anti-aliasing to the right of the line

			if (rDistRight > 0)
				Dest.SetPixelTrans(xTop + cxCount, y, rgbColor, (BYTE)(rOpacity * (rDistRight > 1.0 ? 1.0 : rDistRight)));

			//	Next point

			if (d >= 0)
				{
				x = x + sx;
				d = d - ay;
				rDistLeft += rDistLeftInc;
				rDistRight -= rDistRightDec;
				}

			d = d + ax;
			rDistLeft -= rDistLeftDec;
			rDistRight += rDistRightInc;
			}
		}
	}

void CGDraw::LineBroken (CG32bitImage &Dest, int xSrc, int ySrc, int xDest, int yDest, int xyBreak, CG32bitPixel rgbColor)

//	LineBroken
//
//	Draws a connection between two points using only horizontal and vertical lines

	{
	if (xyBreak < 0)
		xyBreak = -xyBreak;

	//	Different algorithm for horizontal vs. vertical orientation

	if (abs(xDest - xSrc) > abs(yDest - ySrc))
		{
		int xDelta = xDest - xSrc;
		if (xDelta < 0)
			{
			Swap(xSrc, xDest);
			xDelta = -xDelta;
			}

		int yDelta = yDest - ySrc;

		//	First segment

		int iDist;
		if (ySrc != yDest)
			iDist = min(xyBreak, xDelta);
		else
			iDist = xDelta;

		Dest.FillLine(xSrc, ySrc, iDist, rgbColor);

		//	Connector

		if (ySrc != yDest)
			{
			if (yDelta > 0)
				Dest.FillColumn(xSrc + iDist, ySrc, yDelta, rgbColor);
			else
				Dest.FillColumn(xSrc + iDist, yDest, -yDelta, rgbColor);

			//	Last segment

			if (iDist < xDelta)
				Dest.FillLine(xSrc + iDist, yDest, xDelta - iDist, rgbColor);
			}
		}
	else
		{
		int yDelta = yDest - ySrc;
		if (yDelta < 0)
			{
			Swap(ySrc, yDest);
			yDelta = -yDelta;
			}

		int xDelta = xDest - xSrc;

		//	First segment

		int iDist;
		if (xSrc != xDest)
			iDist = min(xyBreak, yDelta);
		else
			iDist = yDelta;

		Dest.FillColumn(xSrc, ySrc, iDist, rgbColor);

		//	Connector

		if (xSrc != xDest)
			{
			if (xDelta > 0)
				Dest.FillLine(xSrc, ySrc + iDist, xDelta, rgbColor);
			else
				Dest.FillLine(xDest, ySrc + iDist, -xDelta, rgbColor);

			//	Last segment

			if (iDist < yDelta)
				Dest.FillColumn(xDest, ySrc + iDist, yDelta - iDist, rgbColor);
			}
		}
	}

void CGDraw::LineDotted (CG32bitImage &Dest, int x1, int y1, int x2, int y2, CG32bitPixel rgbColor)

//	LineDotted
//
//	Draw a dotted line

	{
	int dx = x2 - x1;
	int ax = Absolute(dx) * 2;
	int sx = (dx > 0 ? 1 : -1);

	int dy = y2 - y1;
	int ay = Absolute(dy) * 2;
	int sy = (dy > 0 ? 1 : -1);

	int x = x1;
	int y = y1;
	int d;

	if (ax > ay)		//	x dominant
		{
		d = ay - ax / 2;
		while (true)
			{
			if ((x % 2) == 0)
				Dest.SetPixel(x, y, rgbColor);

			if (x == x2)
				return;
			else if (d >= 0)
				{
				y = y + sy;
				d = d - ax;
				}

			x = x + sx;
			d = d + ay;
			}
		}
	else				//	y dominant
		{
		d = ax - ay / 2;
		while (true)
			{
			if ((y % 2) == 0)
				Dest.SetPixel(x, y, rgbColor);

			if (y == y2)
				return;
			else if (d >= 0)
				{
				x = x + sx;
				d = d - ay;
				}

			y = y + sy;
			d = d + ax;
			}
		}
	}

void CGDraw::LineGradient (CG32bitImage &Dest, int x1, int y1, int x2, int y2, int iWidth, CG32bitPixel rgbColor1, CG32bitPixel rgbColor2)

//	LineGradient
//
//	Draws a gradient line

	{
	if (rgbColor1.GetAlpha() == 0xff && rgbColor2.GetAlpha() == 0xff)
		LineBresenhamGradient(Dest, x1, y1, x2, y2, iWidth, rgbColor1, rgbColor2);
	else
		LineBresenhamGradientTrans(Dest, x1, y1, x2, y2, iWidth, rgbColor1, rgbColor2);
	}

void CGDraw::LineHD (CG32bitImage &Dest, int x1, int y1, int x2, int y2, int iWidth, CG32bitPixel rgbColor)

//	LineHD
//
//	Draw a high-quality line. [This is better for thicker lines.]

	{
	CLinePainter LinePainter;
	LinePainter.DrawSolid(Dest, x1, y1, x2, y2, iWidth, rgbColor);
	}
