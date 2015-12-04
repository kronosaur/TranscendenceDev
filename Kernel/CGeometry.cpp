//	CGeometry.cpp
//
//	Implements various geometric methods
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "Kernel.h"
#include "KernelObjID.h"

#include "Euclid.h"

const Metric EPSILON = 1e-10;

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
