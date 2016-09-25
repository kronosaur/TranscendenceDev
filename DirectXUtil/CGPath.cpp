//	CGPath.cpp
//
//	CGPath class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

const Metric POLYGON_POINT_EPSILON =					1.0e-06;
const Metric POLYGON_POINT_EPSILON2 =					POLYGON_POINT_EPSILON * POLYGON_POINT_EPSILON;

void CGPath::AddPolygonHandoff (TArray<CVector> &Points)

//	AddPolygonHandoff
//
//	Adds a closed polygon to the path

	{
	//	If empty, initialize

	if (m_SubPaths.GetCount() == 0)
		{
		InitTakeHandoff(Points);
		return;
		}

	//	If we have an open path with a single point, then we replace that.

	SSubPath &LastPath = m_SubPaths[m_SubPaths.GetCount() - 1];
	if (!LastPath.bClosed && LastPath.Lines.GetCount() == 1)
		{
		LastPath.Lines.TakeHandoff(Points);
		Close();
		return;
		}

	//	Otherwise, we close the path and add a new sub path

	if (!LastPath.bClosed)
		Close();

	SSubPath *pNewPath = m_SubPaths.Insert();
	pNewPath->Lines.TakeHandoff(Points);
	Close();
	}

void CGPath::CalcCornerPoints (const CVector &From, const CVector &Center, const CVector &To, Metric rHalfWidth, CVector *retInner, CVector *retOuter)

//	CalcCornerPoints
//
//	Given two line segments joined at a common point (Center) and whose 
//	endpoints are From and To, we return two points, colinear with Center,
//	which define the corner points (where the line bends) for a line of
//	the given width.

	{
	//	Imagine two line segment: AB and BC (where A = From, B = Center,
	//	C = To).
	//
	//	GreatAngle is the angle formed from the line BC to BA

	Metric rBCAngle = (To - Center).Polar();
	Metric rBAAngle = (From - Center).Polar();
	Metric rGreatAngle = mathAngleDiff(rBCAngle, rBAAngle);

	//	CornerAngle is the angle from B (Center) to the outer corner. If is
	//	halfway between BC and BA.

	Metric rCornerAngle = mathAngleMod(rBCAngle + (0.5 * rGreatAngle));

	//	If GreatAngle is PI then A, B, and C are colinear

	if (rGreatAngle == PI || rGreatAngle == 0.0)
		{
		CVector ToCorner = CVector::FromPolar(rCornerAngle, rHalfWidth);
		if (retOuter)
			*retOuter = Center + ToCorner;
		if (retInner)
			*retInner = Center - ToCorner;
		}

	//	Otherwise, we need some more trig to compute the points

	else
		{
		//	Imagine a line parallel to BC and rHalfWidth away in the outer
		//	direction. This line is the outer outline. Imagine a point P
		//	on that line such that BCP form a right-triangle with B at the
		//	right-angle.
		//
		//	That is, the line BP is tangential to BC.
		//
		//	Thus the angle of the BP line is just 90 degrees (PI/2) more than
		//	the BC angle.
		//
		//	NOTE: If GreatAngle is less than 180 degrees then we have a
		//	concave angle. In that case, we flip P to a line parallel to BA.
		//	Everything else will work out the same.

		Metric rBPAngle = (rGreatAngle > PI ? rBCAngle + HALF_PI : rBAAngle - HALF_PI);

		//	Extending this parallel line will eventually get us to the corner,
		//	which we'll call point X. Thus BPX form a right-triangle with P at
		//	the right-angle and BX as the hypotenuse.
		//
		//	We eventually want to find the length of BX, so we try to figure out
		//	the angle formed by BXP.
		//
		//	We can easily compute the PBX angle because BX is along the 
		//	CornerAngle line.

		Metric rPBXAngle = mathAngleDiff(rBPAngle, rCornerAngle);

		//	Since we have a right-triangle, the BXP angle is just 90 - PBX.

		Metric rBXPAngle = HALF_PI - rPBXAngle;

		//	Now we can compute the distance from BX using SOH-CAH-TOA.

		Metric rSinBXP = sin(rBXPAngle);
		Metric rBX = (rSinBXP != 0.0 ? (rHalfWidth / rSinBXP) : rHalfWidth);

		//	We're done! The outer angle is positive; the inner one negative.

		CVector ToCorner = CVector::FromPolar(rCornerAngle, rBX);
		if (retOuter)
			*retOuter = Center + ToCorner;
		if (retInner)
			*retInner = Center - ToCorner;
		}
	}

void CGPath::Close (void)

//	Close
//
//	Closes the current sub-path

	{
	//	If no sub-paths, then nothing to do

	if (m_SubPaths.GetCount() == 0)
		return;

	//	Otherwise, get the last sub-path and close it.

	SSubPath &SubPath = m_SubPaths[m_SubPaths.GetCount() - 1];

	//	Remember the first point on the sub-path, before we close it

	CVector LastPoint;
	if (SubPath.Lines.GetCount() > 0)
		LastPoint = SubPath.Lines[0];

	//	Close the sub-path

	SubPath.bClosed = true;

	//	Create a new sub path
	//	NOTE: SubPath may be invalid after the insert, because it might have
	//	to realloc the array.

	SSubPath *pNewSubPath = m_SubPaths.Insert();
	pNewSubPath->Lines.Insert(LastPoint);
	}

void CGPath::CreateSubPathStrokePath (int iSubPath, Metric rLineWidth, CGPath *retPath)

//	CreateSubPathStrokePath
//
//	Generates a path for the given subpath representing a polygon to fill to
//	stroke the subpath with the given width.

	{
	int i;

	ASSERT(iSubPath >= 0 && iSubPath < m_SubPaths.GetCount());
	const SSubPath &SubPath = m_SubPaths[iSubPath];

	retPath->Init();

	//	Calc some stuff

	Metric rHalfWidth = 0.5 * rLineWidth;

	//	Ignore if subpath has less than 2 points

	if (SubPath.Lines.GetCount() < 2)
		return;

	//	If this is a closed subpath then we end up with TWO closed subpaths
	//	(one inside the other).

	else if (SubPath.bClosed && SubPath.Lines.GetCount() > 2)
		{
		retPath->m_SubPaths.InsertEmpty(2);
		SSubPath *pOuter = &retPath->m_SubPaths[0];
		pOuter->bClosed = true;
		SSubPath *pInner = &retPath->m_SubPaths[1];
		pInner->bClosed = true;

		int iPrev = SubPath.Lines.GetCount() - 1;
		int iCur = 0;
		int iNext = 1;

		while (iCur < SubPath.Lines.GetCount())
			{
			//	Compute the inner and outer corner points

			CVector *pOuterCorner = pOuter->Lines.Insert();
			CVector *pInnerCorner = pInner->Lines.Insert();

			CalcCornerPoints(SubPath.Lines[iPrev],
					SubPath.Lines[iCur],
					SubPath.Lines[iNext],
					rHalfWidth,
					pInnerCorner,
					pOuterCorner);

			//	Next

			iPrev = iCur;
			iCur++;
			iNext++;
			if (iNext == SubPath.Lines.GetCount())
				iNext = 0;
			}
		}

	//	Otherwise we end up with a single closed subpath.

	else
		{
		SSubPath *pNewPath = retPath->m_SubPaths.Insert();
		pNewPath->bClosed = true;

		//	We end up with twice as many points

		int iNewPointCount = SubPath.Lines.GetCount() * 2;
		pNewPath->Lines.InsertEmpty(iNewPointCount);

		//	Loop over all points

		int iPrev = -1;
		int iCur = 0;
		int iNext = 1;

		for (i = 0; i < SubPath.Lines.GetCount(); i++)
			{
			//	The first point has a closed end

			if (iPrev == -1)
				{
				const CVector &Center = SubPath.Lines[iCur];
				const CVector &Next = SubPath.Lines[iNext];

				Metric rLine = (Next - Center).Polar();
				Metric rTangent = rLine + HALF_PI;

				CVector Line = CVector::FromPolar(rLine, rHalfWidth);
				CVector Tangent = CVector::FromPolar(rTangent, rHalfWidth);
				pNewPath->Lines[0] = Center - Line - Tangent;
				pNewPath->Lines[1] = Center - Line + Tangent;
				}

			//	The last point is closed

			else if (iNext == -1)
				{
				const CVector &Center = SubPath.Lines[iCur];
				const CVector &Prev = SubPath.Lines[iPrev];

				Metric rLine = (Prev - Center).Polar();
				Metric rTangent = rLine + HALF_PI;

				CVector Line = CVector::FromPolar(rLine, rHalfWidth);
				CVector Tangent = CVector::FromPolar(rTangent, rHalfWidth);
				pNewPath->Lines[iCur + 1] = Center - Line - Tangent;
				pNewPath->Lines[iNewPointCount - iCur] = Center - Line + Tangent;
				}

			//	Otherwise this is a point in between

			else
				{
				const CVector &Center = SubPath.Lines[iCur];
				const CVector &Prev = SubPath.Lines[iPrev];
				const CVector &Next = SubPath.Lines[iNext];

				CalcCornerPoints(Prev,
						Center,
						Next,
						rHalfWidth,
						&pNewPath->Lines[iNewPointCount - iCur],
						&pNewPath->Lines[iCur + 1]);
				}

			//	Next

			iPrev = iCur;
			iCur++;
			iNext++;
			if (iNext == SubPath.Lines.GetCount())
				iNext = -1;
			}
		}
	}

int CGPath::GetSubPathPolygonPoints (int iSubPath, TArray<CVector> *retPoints) const

//	GetSubPathPolygonPoints
//
//	Returns a set of points for the given subpath. If necessary, this will convert
//	the path to a polygon.

	{
	int i;

	ASSERT(iSubPath >= 0 && iSubPath < m_SubPaths.GetCount());

	if (retPoints)
		retPoints->DeleteAll();

	const SSubPath &SubPath = m_SubPaths[iSubPath];
	int iCount = SubPath.Lines.GetCount();

	if (retPoints)
		{
		retPoints->InsertEmpty(iCount);
		for (i = 0; i < iCount; i++)
			retPoints->GetAt(i) = SubPath.Lines[i];
		}

	return iCount;
	}

int CGPath::GetVertexCount (void) const

//	GetVertexCount
//
//	Returns the total number of vertices in all subpaths

	{
	int i;
	int iTotal = 0;

	for (i = 0; i < m_SubPaths.GetCount(); i++)
		iTotal += m_SubPaths[i].Lines.GetCount();

	return iTotal;
	}

void CGPath::Init (void)

//	Init
//
//	Initializes to empty path

	{
	m_SubPaths.DeleteAll();
	}

void CGPath::Init (const TArray<CVector> &Points, bool bLeaveOpen)

//	Init
//
//	Initializes to a single closed path

	{
	int i;

	m_SubPaths.DeleteAll();
	for (i = 0; i < Points.GetCount(); i++)
		LineTo(Points[i]);

	if (!bLeaveOpen)
		Close();
	}

void CGPath::InitTakeHandoff (TArray<CVector> &Points)

//	InitTakeHandoff
//
//	Initializes to a single close path from the given points.

	{
	m_SubPaths.DeleteAll();

	SSubPath *pPath = m_SubPaths.Insert();
	pPath->Lines.TakeHandoff(Points);

	Close();
	}

void CGPath::LineTo (const CVector &Pos)

//	LineTo
//
//	Adds a straight line to the end of the last sub-path

	{
	//	If no sub-paths, add a new point

	if (m_SubPaths.GetCount() == 0)
		MoveTo(Pos);

	//	Otherwise, add a line from the last point in the last
	//	sub-path.

	else
		{
		SSubPath &SubPath = m_SubPaths[m_SubPaths.GetCount() - 1];
		SubPath.Lines.Insert(Pos);
		}
	}

void CGPath::MoveTo (const CVector &Pos)

//	MoveTo
//
//	Begins a new sub-path

	{
	SSubPath *pNewSubPath = m_SubPaths.Insert();
	pNewSubPath->Lines.Insert(Pos);
	}

void CGPath::Rasterize (CGRegion *retRegion, int iAntiAlias)

//	Rasterize
//
//	Rasterize to a region

	{
	CPolygonRasterizer Rasterizer(*this, iAntiAlias);
	Rasterizer.Rasterize(retRegion);
	}
