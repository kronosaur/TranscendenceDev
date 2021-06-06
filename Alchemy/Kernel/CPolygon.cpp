//	CPolygon.cpp
//
//	Implements CPolygon object
//	Copyright (c) 2021 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Euclid.h"

bool CPolygon::PointIntersects (const CVector &vA, bool bIncludeHoles) const

//	PointIntersects
//
//	Returns TRUE if the given point is inside the polygon. If bIncludeHoles is 
//	TRUE, then we check to see if the point is in a hole, which counts as NOT
//	inside the polygon.

	{
	bool bInside = false;

	for (int i = 0, j = m_vPoints.GetCount() - 1; i < m_vPoints.GetCount(); j = i++)
		{
		if ((m_vPoints[i].Y() >= vA.Y()) != (m_vPoints[j].Y() >= vA.Y())
				&& (vA.X() <= (m_vPoints[j].X() - m_vPoints[i].X()) * (vA.Y() - m_vPoints[i].Y()) / (m_vPoints[j].Y() - m_vPoints[i].Y()) + m_vPoints[i].X()))
			bInside = !bInside;
		}

	//	If we're not inside the outer bounds, then we're done

	if (!bInside)
		return false;

	//	Otherwise, check to see if we're inside a hole. If so, then we're not
	//	inside.

	if (bIncludeHoles
			&& PointIntersectsHole(vA))
		return false;

	return true;
	}

bool CPolygon::PointIntersectsHole (const CVector &vA) const

//	PointIntersectsHole
//
//	Returns TRUE if the given point is inside any of the polygon's holes

	{
	for (int i = 0; i < m_Holes.GetCount(); i++)
		if (m_Holes[i]->PointIntersects(vA, false))
			return true;

	return false;
	}
