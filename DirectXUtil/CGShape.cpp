//	CGShape.cpp
//
//	CGShape Class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

bool CGShape::Arc (const CVector &vCenter, Metric rRadius, Metric rStartAngle, Metric rEndAngle, Metric rArcWidth, CGPath *retPath, Metric rSpacing, DWORD dwFlags)

//	Arc
//
//	Generates an arc of the given (radial) width. Spacing truncates the ends of 
//	the arc by the given number of pixels.
//
//	ARC_INNER_RADIUS: If this flag is set, rRadius is the inner radius of the 
//		arc. Otherwise, it is the center.

	{
	retPath->Init();

	//	Short-circuit

	if (rRadius <= 0.0 || rArcWidth <= 0.0)
		return false;

	//	Options

	bool bCenterRadius = ((dwFlags & ARC_INNER_RADIUS ? false : true));

	//	We will create a path

	Metric rHalfWidth = 0.5 * rArcWidth;
	Metric rInnerRadius = (bCenterRadius ? Max(0.0, (Metric)rRadius - rHalfWidth) : rRadius);
	Metric rOuterRadius = (bCenterRadius ? (Metric)rRadius + rHalfWidth : rRadius + rArcWidth);

	//	If the start angle equals the end angle, then we create a full ring.

	if (rStartAngle == rEndAngle)
		{
		//	Add the points for the outer circle

		TArray<CVector> Points;
		CGeometry::AddArcPoints(vCenter, rOuterRadius, 0.0, 0.0, &Points);
		retPath->AddPolygonHandoff(Points);

		//	If we've got an inner radius, add that

		if (rInnerRadius > 0.0)
			{
			CGeometry::AddArcPoints(vCenter, rInnerRadius, 0.0, 0.0, &Points);
			retPath->AddPolygonHandoff(Points);
			}
		}

	//	Otherwise, an arc

	else
		{
		TArray<CVector> Points;

		//	Figure out how many radians the spacing is at the outer radius and the
		//	inner radius.

		Metric rOuterSpacing = rSpacing / rOuterRadius;

		//	Compute angles for outer curve

		Metric rStartOuter = ::mathAngleMod(rStartAngle + rOuterSpacing);
		Metric rEndOuter = ::mathAngleMod(rEndAngle - rOuterSpacing);

		//	Start at the outer curve

		CGeometry::AddArcPoints(vCenter, rOuterRadius, rStartOuter, rEndOuter, &Points, CGeometry::FLAG_SCREEN_COORDS);

		//	If the inner radius is 0 then we just need a single point at the center.

		if (rInnerRadius == 0.0)
			Points.Insert(vCenter);

		//	Otherwise, continue with the inner curve

		else
			{
			Metric rInnerSpacing = (rInnerRadius > 0.0 ? rSpacing / rInnerRadius : 0.0);
			Metric rStartInner = ::mathAngleMod(rStartAngle + rInnerSpacing);
			Metric rEndInner = ::mathAngleMod(rEndAngle - rInnerSpacing);

			CGeometry::AddArcPoints(vCenter, rInnerRadius, rStartInner, rEndInner, &Points, CGeometry::FLAG_SCREEN_COORDS | CGeometry::FLAG_CLOCKWISE);
			}

		//	Create a path and rasterize it.

		retPath->InitTakeHandoff(Points);
		}

	return true;
	}

bool CGShape::ArcSegment (const CVector &vCenter, Metric rRadius, Metric rAngle, Metric rWidth, CGPath *retPath)

//	ArcSegment
//
//	Generates a circle segment (the area defined by a chord).

	{
	retPath->Init();

	//	Make sure we're in bounds

	if (rRadius <= 0.0 || rWidth <= 0.0)
		return false;

	//	If width is greater than 2 radii, then we've got a full circle

	if (rWidth >= 2.0 * rRadius)
		{
		TArray<CVector> Points;
		CGeometry::AddArcPoints(vCenter, rRadius, 0.0, 0.0, &Points);
		retPath->InitTakeHandoff(Points);
		}

	//	Otherwise, a circle segment

	else
		{
		//	Let C be the center of the circle. For now we compute everything as if
		//	C were at the origin (and translate later).
		//
		//	Let P1 be the point at which the chord touches the circle. Let P2 be the
		//	second point such that P1 to P2 is the arc of the segment.
		//
		//	Let M be the mid point on the chord between P1 and P2.
		//
		//	Start by computing the distance between C and M along the rAngle. This
		//	will be negative if rWidth > rRadius.

		Metric rChordD = rRadius - rWidth;

		//	Next, compute the angle M-C-P2. If we were to add this angle to rAngle,
		//	we would end up with the angle of P2.

		Metric rHalfAngle = acos(rChordD / rRadius);

		//	Now compute the two angles for P1 and P2.

		Metric rP1Angle = mathAngleMod(rAngle - rHalfAngle);
		Metric rP2Angle = mathAngleMod(rAngle + rHalfAngle);

		//	Add the points along the arc

		TArray<CVector> Points;
		CGeometry::AddArcPoints(vCenter, rRadius, rP1Angle, rP2Angle, &Points, CGeometry::FLAG_SCREEN_COORDS);

		//	Create a path and rasterize it.

		retPath->InitTakeHandoff(Points);
		}

	return true;
	}

bool CGShape::ArcQuadrilateral (const CVector &vCenter, const CVector &vInnerPos, const CVector &vOuterPos, Metric rWidth, CGPath *retPath)

//	ArcQuadrilateral
//
//	Generates a shape in which two sides are straight and two sides are curved
//	with respect to a center point.

	{
	retPath->Init();

	//	Decompose the inner and outer positions

	Metric rInnerRadius;
	Metric rInnerAngle = vInnerPos.Polar(&rInnerRadius);

	Metric rOuterRadius;
	Metric rOuterAngle = vOuterPos.Polar(&rOuterRadius);

	//	Make sure we're in bounds

	if (rInnerRadius >= rOuterRadius)
		return false;

	//	Generate some metrics for the quadrilateral

	CVector vLength = vOuterPos - vInnerPos;
	CVector vHalfWidth = 0.5 * rWidth * vLength.Perpendicular().Normal();

	//	Generate the intersection between the inner circle and the top line.

	bool bNoInnerIntersect = false;
	CVector vP1;
	CVector vP2;
	CVector vInnerCornerUp;
	CGeometry::EIntersectResults iResult = CGeometry::IntersectLineCircle(vInnerPos + vHalfWidth, vOuterPos + vHalfWidth, CVector(), rInnerRadius, &vP1, &vP2);
	if (iResult == CGeometry::intersectNone)
		bNoInnerIntersect = true;
	else if (iResult == CGeometry::intersectPoint)
		vInnerCornerUp = vP1;
	else
		vInnerCornerUp = ((vP1 - vInnerPos).Length2() < (vP2 - vInnerPos).Length2() ? vP1 : vP2);

	//	Now the bottom line

	CVector vInnerCornerDown;
	iResult = CGeometry::IntersectLineCircle(vInnerPos - vHalfWidth, vOuterPos - vHalfWidth, CVector(), rInnerRadius, &vP1, &vP2);
	if (iResult == CGeometry::intersectNone)
		bNoInnerIntersect = true;
	else if (iResult == CGeometry::intersectPoint)
		vInnerCornerDown = vP1;
	else
		vInnerCornerDown = ((vP1 - vInnerPos).Length2() < (vP2 - vInnerPos).Length2() ? vP1 : vP2);

	//	If the inner side did not intersect, then we just use a straight line.

	if (bNoInnerIntersect)
		{
		vInnerCornerUp = vInnerPos + vHalfWidth;
		vInnerCornerDown = vInnerPos - vHalfWidth;
		}

	//	Generate the intersection between the outer circle and the top line.

	bool bNoOuterIntersect = false;
	CVector vOuterCornerUp;
	iResult = CGeometry::IntersectLineCircle(vInnerPos + vHalfWidth, vOuterPos + vHalfWidth, CVector(), rOuterRadius, &vP1, &vP2);
	if (iResult == CGeometry::intersectNone)
		bNoOuterIntersect = true;
	else if (iResult == CGeometry::intersectPoint)
		vOuterCornerUp = vP1;
	else
		vOuterCornerUp = ((vP1 - vOuterPos).Length2() < (vP2 - vOuterPos).Length2() ? vP1 : vP2);

	//	Now the bottom line

	CVector vOuterCornerDown;
	iResult = CGeometry::IntersectLineCircle(vInnerPos - vHalfWidth, vOuterPos - vHalfWidth, CVector(), rOuterRadius, &vP1, &vP2);
	if (iResult == CGeometry::intersectNone)
		bNoOuterIntersect = true;
	else if (iResult == CGeometry::intersectPoint)
		vOuterCornerDown = vP1;
	else
		vOuterCornerDown = ((vP1 - vOuterPos).Length2() < (vP2 - vOuterPos).Length2() ? vP1 : vP2);

	//	If no intersection, just use the two points

	if (bNoOuterIntersect)
		{
		vOuterCornerUp = vOuterPos + vHalfWidth;
		vOuterCornerDown = vOuterPos - vHalfWidth;
		}

	//	We will create a path

	TArray<CVector> Points;

	//	If no outer intersection, then just use the two points

	if (bNoOuterIntersect)
		{
		Points.Insert(vCenter + vOuterCornerDown);
		Points.Insert(vCenter + vOuterCornerUp);
		}

	//	Otherwise, start at the outer curve

	else
		{
		Metric rStartOuter = vOuterCornerDown.Polar();
		Metric rEndOuter = vOuterCornerUp.Polar();

		CGeometry::AddArcPoints(vCenter, rOuterRadius, rStartOuter, rEndOuter, &Points);
		}

	//	If the inner radius is 0 then we just need a single point at the center.

	if (rInnerRadius == 0.0)
		Points.Insert(vCenter);

	//	If no intersection, then just use the two points

	else if (bNoInnerIntersect)
		{
		Points.Insert(vCenter + vInnerCornerUp);
		Points.Insert(vCenter + vInnerCornerDown);
		}

	//	Otherwise, continue with the inner curve

	else
		{
		Metric rStartInner = vInnerCornerDown.Polar();
		Metric rEndInner = vInnerCornerUp.Polar();

		CGeometry::AddArcPoints(vCenter, rInnerRadius, rStartInner, rEndInner, &Points, CGeometry::FLAG_CLOCKWISE);
		}

	//	Done

	retPath->InitTakeHandoff(Points);

	return true;
	}