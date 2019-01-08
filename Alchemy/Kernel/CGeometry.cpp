//	CGeometry.cpp
//
//	Implements various geometric methods
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "Kernel.h"
#include "KernelObjID.h"

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

CGeometry::EIntersectResults CGeometry::IntersectLineCircle (const CVector &vFrom, const CVector &vTo, const CVector &vCenter, Metric rRadius, CVector *retvP1, CVector *retvP2)

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
			return intersectNone;
		else if (rD > rRadius - EPSILON)
			{
			if (retvP1)
				*retvP1 = vCenter + CVector(rRadius, 0.0);

			return intersectPoint;
			}
		else if (-rD > rRadius - EPSILON)
			{
			if (retvP1)
				*retvP1 = vCenter - CVector(rRadius, 0.0);

			return intersectPoint;
			}
		else
			{
			Metric rH = sqrt(rD * rD + rRadius * rRadius);

			if (retvP1)
				*retvP1 = vCenter + CVector(rD, rH);

			if (retvP2)
				*retvP2 = vCenter + CVector(rD, -rH);

			return intersect2Points;
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
			return intersectNone;

		//	If 0, then the line is tangent

		else if (rD < EPSILON)
			{
			if (retvP1)
				{
				Metric rX = -rB / (2.0 * rA);
				Metric rY = rLineM * rX + rLineC;

				*retvP1 = CVector(rX, rY);
				}

			return intersectPoint;
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

			return intersect2Points;
			}
		}
	}
