//	CGDraw.cpp
//
//	CGDraw Class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

const Metric PIXEL_THRESHOLD = 2.0;

void CGDraw::QuadCurve (CG32bitImage &Dest, int x1, int y1, int x2, int y2, int xMid, int yMid, int iLineWidth, CG32bitPixel rgbColor)

//	QuadCurve
//
//	Draws a quadratic curve from x1,y1 to x2,y2, with xMid,yMid as the mid-point
//	of the curve.

	{
	//	A quadratic curve consists of two end-points and a single control-point.
	//	[In constrast to a classical Bezier curve, which has two control-
	//	points.] See: https://en.wikipedia.org/wiki/B%C3%A9zier_curve
	//
	//	We convert to vectors for ease of manipulation.

	CVector vP1 = CVector(x1, y1);
	CVector vP2 = CVector(x2, y2);
	CVector vMid = CVector(xMid, yMid);

	//	We start by computing the position of the control-point based on the
	//	input mid-point.
	//
	//	We derive this equation by starting with the equation that yields the
	//	mid-point based on the control-point, and then solve for the 
	//	control-point.

	CVector vC1 = 0.5 * (4.0 * vMid - vP1 - vP2);

	//	We recursively split up the curve into halves until we can just draw
	//	straight lines.

	QuadCurve(Dest, vP1, vP2, vC1, iLineWidth, rgbColor);
	}

void CGDraw::QuadCurve (CG32bitImage &Dest, const CVector &vP1, const CVector &vP2, const CVector &vC1, int iLineWidth, CG32bitPixel rgbColor)

//	QuadCurve
//
//	Draws a quadratic curve from vP1 to vP2 using vC1 as the control-point.
//	We recursively split up the curve into halves until we can just draw 
//	straight lines.

	{
	//	If the distance from vP1 to vP2 is less than our threshold (pixel 
	//	resolution), then we just draw a line.

	if ((vP1 - vP2).Longest() < PIXEL_THRESHOLD)
		Line(Dest, (int)vP1.GetX(), (int)vP1.GetY(), (int)vP2.GetX(), (int)vP2.GetY(), iLineWidth, rgbColor);

	//	Otherwise, we split the curve

	else
		{
		//	Compute the mid-point between vP1 and vC1. This will end up being 
		//	the new control point for the left curve.

		CVector vLC1 = vP1 + 0.5 * (vC1 - vP1);

		//	Compute the mid-point between vC1 and vP2. This will end up being
		//	the new control point for the right curve.

		CVector vRC1 = vC1 + 0.5 * (vP2 - vC1);

		//	Compute the mid-point between the left and right control points.
		//	This will be the mid-point on the curve, and will become one of
		//	the end-points of both sub-curves.

		CVector vM1 = vLC1 + 0.5 * (vRC1 - vLC1);

		//	Recurse

		QuadCurve(Dest, vP1, vM1, vLC1, iLineWidth, rgbColor);
		QuadCurve(Dest, vM1, vP2, vRC1, iLineWidth, rgbColor);
		}
	}
