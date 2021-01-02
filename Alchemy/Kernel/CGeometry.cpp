//	CGeometry.cpp
//
//	Implements various geometric methods
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Euclid.h"

const Metric EPSILON = 1e-10;
const Metric MIN_SEGMENT_LENGTH = 3.0;

void CGeometry::AccumulateBounds (const CVector &vPos, CVector &vLL, CVector &vUR)

//	AccumulateBounds
//
//	Updates vLL and vUR

	{
	if (vPos.GetX() < vLL.GetX())
		vLL.SetX(vPos.GetX());
	else if (vPos.GetX() > vUR.GetX())
		vUR.SetX(vPos.GetX());

	if (vPos.GetY() < vLL.GetY())
		vLL.SetY(vPos.GetY());
	else if (vPos.GetY() > vUR.GetY())
		vUR.SetY(vPos.GetY());
	}

void CGeometry::AddArcPoints (const CVector &vCenter, Metric rRadius, Metric rFromAngle, Metric rToAngle, TArray<CVector> *ioPoints, DWORD dwFlags)

//	AddArcPoints
//
//	Add a set of points to form an arc.

	{
	bool bScreenCoords = (dwFlags & FLAG_SCREEN_COORDS ? true : false);
	bool bClockwise = (dwFlags & FLAG_CLOCKWISE ? true : false);

	//	If angles are equal, then we have a circle

	if (rFromAngle == rToAngle)
		{
		//	Figure out how many segments

		Metric rCircle = TAU * rRadius;
		Metric rSegCount = Max(8.0, floor(rCircle / MIN_SEGMENT_LENGTH));
		Metric rSegAngle = TAU / rSegCount;

		//	Make the points

		ioPoints->GrowToFit((int)rSegCount + 1);

		Metric rAngle;
		if (bClockwise)
			{
			for (rAngle = 0.0; rAngle > -TAU; rAngle -= rSegAngle)
				ioPoints->Insert(vCenter + CVector::FromPolar(rAngle, rRadius));
			}
		else
			{
			for (rAngle = 0.0; rAngle < TAU; rAngle += rSegAngle)
				ioPoints->Insert(vCenter + CVector::FromPolar(rAngle, rRadius));
			}
		}

	//	Otherwise, we create an arc

	else
		{
		//	Compute angles for the curve

		Metric rArcAngle = ::mathAngleDiff(rFromAngle, rToAngle);
		Metric rArc = rArcAngle * rRadius;
		Metric rSegCount = floor(rArc / MIN_SEGMENT_LENGTH);
		Metric rSegAngle = rArcAngle / floor(rSegCount);

		//	Allocate an array with enough points for the arc

		ioPoints->GrowToFit((int)rSegCount + 1);

		//	Make the points

		Metric rAngle;

		if (bScreenCoords)
			{
			if (bClockwise)
				{
				Metric rAngleDone = rToAngle - rArcAngle;
				for (rAngle = rToAngle; rAngle > rAngleDone; rAngle -= rSegAngle)
					ioPoints->Insert(vCenter + CVector::FromPolarInv(rAngle, rRadius));

				ioPoints->Insert(vCenter + CVector::FromPolarInv(rFromAngle, rRadius));
				}
			else
				{
				Metric rAngleDone = rFromAngle + rArcAngle;
				for (rAngle = rFromAngle; rAngle < rAngleDone; rAngle += rSegAngle)
					ioPoints->Insert(vCenter + CVector::FromPolarInv(rAngle, rRadius));

				ioPoints->Insert(vCenter + CVector::FromPolarInv(rToAngle, rRadius));
				}
			}
		else
			{
			if (bClockwise)
				{
				Metric rAngleDone = rToAngle - rArcAngle;
				for (rAngle = rToAngle; rAngle > rAngleDone; rAngle -= rSegAngle)
					ioPoints->Insert(vCenter + CVector::FromPolar(rAngle, rRadius));

				ioPoints->Insert(vCenter + CVector::FromPolar(rFromAngle, rRadius));
				}
			else
				{
				Metric rAngleDone = rFromAngle + rArcAngle;
				for (rAngle = rFromAngle; rAngle < rAngleDone; rAngle += rSegAngle)
					ioPoints->Insert(vCenter + CVector::FromPolar(rAngle, rRadius));

				ioPoints->Insert(vCenter + CVector::FromPolar(rToAngle, rRadius));
				}
			}
		}
	}

int CGeometry::AngleArc (int iMinAngle, int iMaxAngle)

//	AngleArc
//
//	Returns the size of the arc.

	{
	if (iMinAngle > iMaxAngle)
		return (iMinAngle - iMaxAngle) % 360;
	else
		return (iMaxAngle - iMinAngle) % 360;
	}

bool CGeometry::ClipLineWithRect (const CVector &vLineStart, const CVector &vLineEnd, const CVector &vLL, const CVector &vUR, CVector *retvLineStart, CVector *retvLineEnd)

//	ClipLineWithRect
//
//	Clips the given line to the rect and return TRUE if we end up with a non-
//	empty line.

	{
	//	We handle the case where the line end is always at or above line start.

	if (vLineEnd.GetY() < vLineStart.GetY())
		return ClipLineWithRect(vLineEnd, vLineStart, vLL, vUR, retvLineEnd, retvLineStart);

	//	If the line start is above our rect, then the whole line is outside.

	else if (vLineStart.GetY() > vUR.GetY())
		return false;

	//	If the line end is below our rect, then the whole line is outside.

	else if (vLineEnd.GetY() < vLL.GetY())
		return false;

	//	Handle case where the line move up and to the right.

	else if (vLineEnd.GetX() >= vLineStart.GetX())
		{
		if (vLineStart.GetX() > vUR.GetX())
			return false;
		else if (vLineEnd.GetX() < vLL.GetX())
			return false;
		}

	//	Otherwise, the line moves up and to the left.

	else
		{
		if (vLineStart.GetX() < vLL.GetX())
			return false;
		else if (vLineEnd.GetX() > vUR.GetX())
			return false;
		}

	//	See if the start point is inside the rectangle.

	if (IntersectRect(vLL, vUR, vLineStart))
		{
		if (retvLineStart) *retvLineStart = vLineStart;

		//	If both points are inside, then no need to clip.

		if (IntersectRect(vLL, vUR, vLineEnd))
			{
			if (retvLineEnd) *retvLineEnd = vLineEnd;
			return true;
			}

		//	Otherwise see which edge we intersect with.

		else
			{
			return FindFirstIntersection(vLL, vUR, vLineStart, vLineEnd, retvLineEnd);
			}
		}

	//	See if the end point is inside the rectangle.

	else if (IntersectRect(vLL, vUR, vLineEnd))
		{
		if (retvLineEnd) *retvLineEnd = vLineEnd;

		//	If both points are inside, then no need to clip.

		if (IntersectRect(vLL, vUR, vLineStart))
			{
			if (retvLineStart) *retvLineStart = vLineStart;
			return true;
			}

		//	Otherwise, see which edge we intersect with.

		else
			{
			return FindFirstIntersection(vLL, vUR, vLineStart, vLineEnd, retvLineStart);
			}
		}

	//	Neither point is inside.

	else
		{
		//	Start of line is below the rectangle bottom edge

		if (vLineStart.GetY() < vLL.GetY())
			{
			//	Intersection with bottom

			if (IntersectLine(CVector(vUR.GetX(), vLL.GetY()), vLL, vLineStart, vLineEnd, retvLineStart))
				{
				//	Top

				if (IntersectLine(CVector(vLL.GetX(), vUR.GetY()), vUR, vLineStart, vLineEnd, retvLineEnd))
					return true;

				//	Left

				else if (IntersectLine(vLL, CVector(vLL.GetX(), vUR.GetY()), vLineStart, vLineEnd, retvLineEnd))
					return true;

				//	Right

				else if (IntersectLine(vUR, CVector(vUR.GetX(), vLL.GetY()), vLineStart, vLineEnd, retvLineEnd))
					return true;
				else
					return false;
				}

			//	Intersection with left

			else if (IntersectLine(vLL, CVector(vLL.GetX(), vUR.GetY()), vLineStart, vLineEnd, retvLineStart))
				{
				//	Top

				if (IntersectLine(CVector(vLL.GetX(), vUR.GetY()), vUR, vLineStart, vLineEnd, retvLineEnd))
					return true;

				//	Right

				else if (IntersectLine(vUR, CVector(vUR.GetX(), vLL.GetY()), vLineStart, vLineEnd, retvLineEnd))
					return true;
				else
					return false;
				}

			//	Intersection with right

			else if (IntersectLine(vUR, CVector(vUR.GetX(), vLL.GetY()), vLineStart, vLineEnd, retvLineStart))
				{
				//	Top

				if (IntersectLine(CVector(vLL.GetX(), vUR.GetY()), vUR, vLineStart, vLineEnd, retvLineEnd))
					return true;

				//	Left

				else if (IntersectLine(vLL, CVector(vLL.GetX(), vUR.GetY()), vLineStart, vLineEnd, retvLineEnd))
					return true;
				else
					return false;
				}
			else
				return false;
			}
		else
			{
			//	Intersection with left

			if (IntersectLine(vLL, CVector(vLL.GetX(), vUR.GetY()), vLineStart, vLineEnd, retvLineStart))
				{
				//	Top

				if (IntersectLine(CVector(vLL.GetX(), vUR.GetY()), vUR, vLineStart, vLineEnd, retvLineEnd))
					return true;

				//	Right

				else if (IntersectLine(vUR, CVector(vUR.GetX(), vLL.GetY()), vLineStart, vLineEnd, retvLineEnd))
					return true;
				else
					return false;
				}

			//	Intersection with right

			else if (IntersectLine(vUR, CVector(vUR.GetX(), vLL.GetY()), vLineStart, vLineEnd, retvLineStart))
				{
				//	Top

				if (IntersectLine(CVector(vLL.GetX(), vUR.GetY()), vUR, vLineStart, vLineEnd, retvLineEnd))
					return true;

				//	Left

				else if (IntersectLine(vLL, CVector(vLL.GetX(), vUR.GetY()), vLineStart, vLineEnd, retvLineEnd))
					return true;
				else
					return false;
				}
			else
				return false;
			}
		}
	}

void CGeometry::CombineArcs (int iMinAngle1, int iMaxAngle1, int iMinAngle2, int iMaxAngle2, int *retiMin, int *retiMax)

//	CombineArcs
//
//	Given two arcs, combine them into one large span.

	{
	//	If either is omni, then we're done.

	if (iMinAngle1 == iMaxAngle1
			|| iMinAngle2 == iMaxAngle2)
		{
		*retiMin = 0;
		*retiMax = 0;
		}

	//	If neither cross 0 or both cross 0, then it's simple

	else if ((iMinAngle1 < iMaxAngle1 && iMinAngle2 < iMaxAngle2)
			|| (iMinAngle1 > iMaxAngle1 && iMinAngle2 > iMaxAngle2))
		{
		*retiMin = Min(iMinAngle1, iMinAngle2);
		*retiMax = Max(iMaxAngle1, iMaxAngle2);
		}

	//	Otherwise, pick whichever arc is larger
	//	LATER: We should be smarter about combining them.

	else if (AngleArc(iMinAngle1, iMaxAngle1) > AngleArc(iMinAngle2, iMaxAngle2))
		{
		*retiMin = iMinAngle1;
		*retiMax = iMaxAngle1;
		}
	else
		{
		*retiMin = iMinAngle2;
		*retiMax = iMaxAngle2;
		}
	}

bool CGeometry::FindFirstIntersection (const CVector &vLL, const CVector &vUR, const CVector &vLineStart, const CVector &vLineEnd, CVector *retvIntersection)

//	FindFirstIntersection
//
//	Intersects a line segment with each side of an axis-aligned rect, and return
//	the first intersection point we find (or FALSE if we do not intersect).

	{
	//	Top

	if (IntersectLine(CVector(vLL.GetX(), vUR.GetY()), vUR, vLineStart, vLineEnd, retvIntersection))
		return true;
	
	//	Right

	else if (IntersectLine(vUR, CVector(vUR.GetX(), vLL.GetY()), vLineStart, vLineEnd, retvIntersection))
		return true;

	//	Bottom

	else if (IntersectLine(CVector(vUR.GetX(), vLL.GetY()), vLL, vLineStart, vLineEnd, retvIntersection))
		return true;

	//	Left

	else if (IntersectLine(vLL, CVector(vLL.GetX(), vUR.GetY()), vLineStart, vLineEnd, retvIntersection))
		return true;

	//	No intersection

	else
		return false;
	}

bool CGeometry::IntersectLine (const CVector &vStart1, const CVector &vEnd1, const CVector &vStart2, const CVector &vEnd2, CVector *retvIntersection, Metric *retIntersectFraction)

//	IntersectLine
//
//	Returns TRUE if the two line segments intersect and also returns
//	the intersection point.
//
//	Coincident lines do not intersect.
//
//	Based on sample by Damian Coventry
//	http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline2d/example.cpp

	{
	Metric denom = ((vEnd2.GetY() - vStart2.GetY())*(vEnd1.GetX() - vStart1.GetX())) -
			((vEnd2.GetX() - vStart2.GetX())*(vEnd1.GetY() - vStart1.GetY()));

	Metric nume_a = ((vEnd2.GetX() - vStart2.GetX())*(vStart1.GetY() - vStart2.GetY())) -
			((vEnd2.GetY() - vStart2.GetY())*(vStart1.GetX() - vStart2.GetX()));

	Metric nume_b = ((vEnd1.GetX() - vStart1.GetX())*(vStart1.GetY() - vStart2.GetY())) -
			((vEnd1.GetY() - vStart1.GetY())*(vStart1.GetX() - vStart2.GetX()));

	if (denom == 0.0)
		{
		if (nume_a == 0.0 && nume_b == 0.0)
			//	Coincident line
			return false;

		//	Parallel line
		return false;
		}

	Metric ua = nume_a / denom;
	Metric ub = nume_b / denom;

	if (ua >= 0.0 && ua <= 1.0 && ub >= 0.0 && ub <= 1.0)
		{
		if (retvIntersection)
			*retvIntersection = CVector(
					vStart1.GetX() + ua * (vEnd1.GetX() - vStart1.GetX()),
					vStart1.GetY() + ua * (vEnd1.GetY() - vStart1.GetY())
					);

		if (retIntersectFraction)
			*retIntersectFraction = ua;

		return true;
		}

	return false;
	}

CGeometry::EIntersect CGeometry::IntersectLineCircle (const CVector &vFrom, const CVector &vTo, const CVector &vCenter, Metric rRadius, CVector *retvP1, CVector *retvP2)

//	IntersectLineCircle
//
//	Returns the intersection between a line and circle

	{
	Metric rDX = vTo.GetX() - vFrom.GetX();
	Metric rDY = vTo.GetY() - vFrom.GetY();

	//	If this line is vertical, then we need a different algorithm

	if (rDX < EPSILON && rDX >= -EPSILON)
		{
		//	Distance between line and center

		Metric rD = vFrom.GetX() - vCenter.GetX();
		if (rD > rRadius || -rD > rRadius)
			return EIntersect::None;
		else if (rD > rRadius - EPSILON)
			{
			if (retvP1)
				*retvP1 = vCenter + CVector(rRadius, 0.0);

			return EIntersect::Point;
			}
		else if (-rD > rRadius - EPSILON)
			{
			if (retvP1)
				*retvP1 = vCenter - CVector(rRadius, 0.0);

			return EIntersect::Point;
			}
		else
			{
			Metric rH = sqrt(rD * rD + rRadius * rRadius);

			if (retvP1)
				*retvP1 = vCenter + CVector(rD, rH);

			if (retvP2)
				*retvP2 = vCenter + CVector(rD, -rH);

			return EIntersect::TwoPoints;
			}
		}

	//	Otherwise, we can solve the intersection equation

	else
		{
		//	Convert the line to y = mx + c form.

		Metric rLineM = (rDY / rDX);
		Metric rLineC = vFrom.GetY() - (rLineM * vFrom.GetX());

		//	Parameters for circle

		Metric rCircleX = vCenter.GetX();
		Metric rCircleY = vCenter.GetY();
		Metric rCircleR = rRadius;

		//	Compute the components of a quadratic equation

		Metric rA = (rLineM * rLineM) + 1.0;
		Metric rB = 2.0 * (rLineM * rLineC - rLineM * rCircleY - rCircleX);
		Metric rC = (rCircleY * rCircleY - rCircleR * rCircleR + rCircleX * rCircleX - 2.0 * rLineC * rCircleY + rLineC * rLineC);

		//	Compute the discriminant

		Metric rD = rB * rB - 4.0 * rA * rC;

		//	If imaginary, then we miss the circle

		if (rD < -EPSILON)
			return EIntersect::None;

		//	If 0, then the line is tangent

		else if (rD < EPSILON)
			{
			if (retvP1)
				{
				Metric rX = -rB / (2.0 * rA);
				Metric rY = rLineM * rX + rLineC;

				*retvP1 = CVector(rX, rY);
				}

			return EIntersect::Point;
			}

		//	Otherwise, two intersections

		else
			{
			if (retvP1)
				{
				Metric rX = (-rB + sqrt(rD)) / (2.0 * rA);
				Metric rY = rLineM * rX + rLineC;

				*retvP1 = CVector(rX, rY);
				}

			if (retvP2)
				{
				Metric rX = (-rB - sqrt(rD)) / (2.0 * rA);
				Metric rY = rLineM * rX + rLineC;

				*retvP2 = CVector(rX, rY);
				}

			return EIntersect::TwoPoints;
			}
		}
	}

bool CGeometry::IntersectRect (const CVector &vLL, const CVector &vUR, const CVector &vPoint)

//	IntersectRect
//
//	Returns TRUE if the point is inside the rect

	{
	return (vUR.GetX() > vPoint.GetX()
			&& vLL.GetX() < vPoint.GetX()
			&& vUR.GetY() > vPoint.GetY()
			&& vLL.GetY() < vPoint.GetY());
	}
