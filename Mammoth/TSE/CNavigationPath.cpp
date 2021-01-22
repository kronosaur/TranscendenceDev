//	CNavigationPath.cpp
//
//	CNavigationPath class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

const int MAX_PATH_RECURSION =					4;
const Metric MIN_SPEED =						(0.001 * LIGHT_SPEED);
const Metric MIN_SPEED2 =						(MIN_SPEED * MIN_SPEED);
const Metric MAX_SAFE_DIST =					(60.0 * LIGHT_SECOND);
const Metric MAX_SAFE_DIST2 =					(MAX_SAFE_DIST * MAX_SAFE_DIST);
const Metric AVOID_DIST =						(MAX_SAFE_DIST + (10.0 * LIGHT_SECOND));
const Metric EXTRA_DIST =						(5.0 * LIGHT_SECOND);

CNavigationPath::CNavigationPath (void) :
		m_dwID(0),
		m_pSovereign(NULL),
		m_iStartIndex(-1),
		m_iEndIndex(-1),
		m_iSuccesses(0),
		m_iFailures(0)

//	CNavigationPath constructor

	{
	}

CNavigationPath::~CNavigationPath (void)

//	CNavigationPath destructor

	{
	}

int CNavigationPath::ComputePath (CSystem *pSystem, CSovereign *pSovereign, const CVector &vFrom, const CVector &vTo, TArray<SWaypoint> &retWaypoints, TArray<CVector> *retPointsChecked)

//	ComputePath
//
//	This uses an A* algorithm to find a path from one place to another

	{
	CAStarPathFinder AStar;

	//	Add the obstacles that we need to avoid

	for (int i = 0; i < pSystem->GetObjectCount(); i++)
		{
		CSpaceObject *pObj = pSystem->GetObject(i);
		CSovereign *pObjSovereign;

		if (pObj == NULL)
			NULL;
		else if ((pObj->GetScale() == scaleStructure 
					|| ((pObj->GetScale() == scaleShip) && (pObj->GetVel().Length2() < MIN_SPEED2)))
				&& (pObjSovereign = pObj->GetSovereign())
				&& (pObjSovereign->IsEnemy(pSovereign))
				&& pObj->CanAttack())
			{
			CVector vUR = pObj->GetPos() + CVector(MAX_SAFE_DIST, MAX_SAFE_DIST);
			CVector vLL = pObj->GetPos() - CVector(MAX_SAFE_DIST, MAX_SAFE_DIST);

			//	Only add obstacles if start and end are outside the obstacle

			if (!IntersectRect(vUR, vLL, vFrom)
					&& !IntersectRect(vUR, vLL, vTo))
				AStar.AddObstacle(vUR, vLL);
			}
		else if (pObj->HasGravity())
			{
			CVector vUR = pObj->GetPos() + CVector(MAX_SAFE_DIST, MAX_SAFE_DIST);
			CVector vLL = pObj->GetPos() - CVector(MAX_SAFE_DIST, MAX_SAFE_DIST);

			//	Only add obstacles if start and end are outside the obstacle

			if (!IntersectRect(vUR, vLL, vFrom)
					&& !IntersectRect(vUR, vLL, vTo))
				AStar.AddObstacle(vUR, vLL);
			}
		else if (pObj->BlocksShips())
			{
			CVector vUR;
			CVector vLL;
			pObj->GetBoundingRect(&vUR, &vLL);

			//	Add a little extra space around obstacles so that we don't have 
			//	to be too precise following the nav path.

			CVector vExtra(EXTRA_DIST, EXTRA_DIST);
			vUR = vUR + vExtra;
			vLL = vLL - vExtra;

			//	Only add obstacles if start and end are outside the obstacle

			if (!IntersectRect(vUR, vLL, vFrom)
					&& !IntersectRect(vUR, vLL, vTo))
				AStar.AddObstacle(vUR, vLL);
			}
		}

	//	Compute a path

	CVector *pPathList;
	int iPathCount = AStar.FindPath(vFrom, vTo, &pPathList);

#ifdef DEBUG_ASTAR_PATH
	if (retPointsChecked)
		*retPointsChecked = AStar.GetDebugPointsChecked();
#endif

	retWaypoints.DeleteAll();
	if (iPathCount <= 0)
		{
		retWaypoints.InsertEmpty(1);
		retWaypoints[0].vPos = vTo;
		}
	else
		{
		retWaypoints.InsertEmpty(iPathCount);
		for (int i = 0; i < retWaypoints.GetCount(); i++)
			retWaypoints[i].vPos = pPathList[i];

		delete pPathList;
		}

	return retWaypoints.GetCount();
	}

Metric CNavigationPath::ComputePathLength (CSystem *pSystem) const

//	ComputePathLength
//
//	Returns the length of the path in Klicks

	{
	int i;
	Metric rDist = 0.0;
	CVector vPos = m_vStart;

	for (i = 0; i < m_Waypoints.GetCount(); i++)
		{
		rDist += (m_Waypoints[i].vPos - vPos).Length();
		vPos = m_Waypoints[i].vPos;
		}

	return rDist;
	}

CVector CNavigationPath::ComputePointOnPath (CSystem *pSystem, Metric rDist) const

//	ComputePointOnPath
//
//	Returns the point on the path a given distance from the start

	{
	CVector vPos = m_vStart;
	int iWaypoint = 0;

	while (rDist > 0.0 && iWaypoint < m_Waypoints.GetCount())
		{
		CVector vLeg = m_Waypoints[iWaypoint].vPos - vPos;

		Metric rLength;
		CVector vNormal = vLeg.Normal(&rLength);

		if (rLength > rDist)
			return vPos + (vNormal * rDist);
		else
			{
			rDist -= rLength;
			vPos = m_Waypoints[iWaypoint].vPos;
			iWaypoint++;
			}
		}

	return vPos;
	}

void CNavigationPath::ComputeWaypointRange (void)

//	ComputeWaypointRange
//
//	Computes how close we need to be to each waypoint to call it success.
//	We don't want it to be too close or else ships will struggle to maneuver,
//	but if it is too far away, they won't follow the path closely enough.

	{
	constexpr Metric MAX_DIST = DEFAULT_NAV_POINT_RADIUS;
	constexpr Metric MAX_DIST2 = MAX_DIST * MAX_DIST;
	constexpr Metric MIN_DIST = 4.0 * LIGHT_SECOND;
	constexpr Metric MIN_DIST2 = MIN_DIST * MIN_DIST;
	constexpr Metric ANGLE_THRESHOLD = mathDegreesToRadians(30);
	constexpr Metric PATH_FRACTION = 0.5;

	CVector vPrev = m_vStart;

	for (int i = 0; i < m_Waypoints.GetCount(); i++)
		{
		//	See if we've got three points to compare.

		if (i + 1 < m_Waypoints.GetCount())
			{
			//	Figure out the length from the previous waypoint to this 
			//	waypoint, and the bearing from the previous waypoint to this
			//	waypoint.

			CVector vFrom = m_Waypoints[i].vPos - vPrev;
			Metric rFromDist;
			Metric rFromAngle = vFrom.Polar(&rFromDist);

			//	Figure out the same for the path from this waypoint to the next
			//	waypoint.

			CVector vTo = m_Waypoints[i + 1].vPos - m_Waypoints[i].vPos;
			Metric rToDist;
			Metric rToAngle = vTo.Polar(&rToDist);

			//	If this is a sharp angle (> 30 degrees) then we use the minimum
			//	distance.

			if (mathAngleDiff(rFromAngle, rToAngle) > ANGLE_THRESHOLD)
				m_Waypoints[i].rRange2 = MIN_DIST2;

			//	Otherwise, we take a fraction of the minimum length

			else
				{
				Metric rDist = Max(MIN_DIST, Min(PATH_FRACTION * Min(rFromDist, rToDist), MAX_DIST));
				m_Waypoints[i].rRange2 = rDist * rDist;
				}
			}

		//	Else, if this is the last point, then the max distance is OK.

		else
			m_Waypoints[i].rRange2 = MAX_DIST2;
		}
	}

void CNavigationPath::Create (CSystem *pSystem, CSovereign *pSovereign, CSpaceObject *pStart, CSpaceObject *pEnd, CNavigationPath **retpPath)

//	Create
//
//	Create a path from pStart to pEnd that avoids enemies of pSovereign

	{
	ASSERT(pStart);
	ASSERT(pEnd);

	CUniverse &Universe = pSystem->GetUniverse();

	CVector vStart = pStart->GetPos();
	CVector vEnd = pEnd->GetPos();

	CNavigationPath *pNewPath = new CNavigationPath;
	pNewPath->m_dwID = Universe.CreateGlobalID();
	pNewPath->m_pSovereign = pSovereign;
	pNewPath->m_iStartIndex = pStart->GetID();
	pNewPath->m_iEndIndex = pEnd->GetID();

#ifdef DEBUG_ASTAR_PATH
	TArray<CVector> *retPointsChecked = &pNewPath->m_PointsChecked;
#else
	TArray<CVector> *retPointsChecked = NULL;
#endif

	//	Compute the path

	pNewPath->m_vStart = vStart;
	ComputePath(pSystem, pSovereign, vStart, vEnd, pNewPath->m_Waypoints, retPointsChecked);
	pNewPath->ComputeWaypointRange();

	//	Done

	*retpPath = pNewPath;
	}

void CNavigationPath::Create (CSystem *pSystem, CSovereign *pSovereign, const CVector &vStart, const CVector &vEnd, CNavigationPath **retpPath)

//	Create
//
//	Creates a path from vStart to vEnd

	{
	CUniverse &Universe = pSystem->GetUniverse();

	CNavigationPath *pNewPath = new CNavigationPath;
	pNewPath->m_dwID = Universe.CreateGlobalID();
	pNewPath->m_pSovereign = pSovereign;
	pNewPath->m_iStartIndex = -1;
	pNewPath->m_iEndIndex = -1;

#ifdef DEBUG_ASTAR_PATH
	TArray<CVector> *retPointsChecked = &pNewPath->m_PointsChecked;
#else
	TArray<CVector> *retPointsChecked = NULL;
#endif

	//	Compute the path

	pNewPath->m_vStart = vStart;
	ComputePath(pSystem, pSovereign, vStart, vEnd, pNewPath->m_Waypoints, retPointsChecked);
	pNewPath->ComputeWaypointRange();

	//	Done

	*retpPath = pNewPath;
	}

CString CNavigationPath::DebugDescribe (CSpaceObject *pObj, CNavigationPath *pNavPath)

//	DebugDescribe
//
//	Outputs debug info about the path

	{
	if (pNavPath == NULL)
		return CONSTLIT("none");

	//	Get the system

	CSystem *pSystem;
	try
		{
		pSystem = pObj->GetSystem();
		}
	catch (...)
		{
		return strPatternSubst(CONSTLIT("invalid system"));
		}

	//	Get the indeces for the objects

	int iFrom;
	int iTo;
	try
		{
		iFrom = pNavPath->m_iStartIndex;
		iTo = pNavPath->m_iEndIndex;
		}
	catch (...)
		{
		return strPatternSubst(CONSTLIT("%x [invalid]"), (DWORD)pNavPath);
		}

	//	Get the objects

	CSpaceObject *pFrom;
	CSpaceObject *pTo;
	try
		{
		pFrom = pSystem->GetObject(iFrom);
		pTo = pSystem->GetObject(iTo);
		}
	catch (...)
		{
		return strPatternSubst(CONSTLIT("invalid objects: %d, %d"), iFrom, iTo);
		}

	//	Return the object names

	try
		{
		return strPatternSubst(CONSTLIT("FROM: %s TO: %s"),
				CSpaceObject::DebugDescribe(pFrom),
				CSpaceObject::DebugDescribe(pTo));
		}
	catch (...)
		{
		return strPatternSubst(CONSTLIT("CObject::DebugDescribe failed"));
		}
	}

void CNavigationPath::DebugPaintInfo (CSystem &System, CG32bitImage &Dest, int x, int y, ViewportTransform &Xform)

//	DebugPaintInfo
//
//	Paint the nav path

	{
	CVector vFrom = m_vStart;

	int xFrom;
	int yFrom;
	Xform.Transform(vFrom, &xFrom, &yFrom);

	for (int i = 0; i < m_Waypoints.GetCount(); i++)
		{
#ifdef DEBUG_ASTAR_PATH
		//	See if this line passes through any blocking objects.

		CG32bitPixel rgbLine;
		if (IsLineBlocked(System, vFrom, m_Waypoints[i].vPos))
			rgbLine = CG32bitPixel(255, 0, 0);
		else
			rgbLine = CG32bitPixel(0, 255, 0);
#else
		constexpr CG32bitPixel rgbLine = CG32bitPixel(0, 255, 0);
#endif
			
		//	Paint

		int xTo;
		int yTo;
		Xform.Transform(m_Waypoints[i].vPos, &xTo, &yTo);

		Dest.DrawLine(xFrom, yFrom,
				xTo, yTo,
				3,
				rgbLine);

		//	Next

		vFrom = m_Waypoints[i].vPos;
		xFrom = xTo;
		yFrom = yTo;
		}

#ifdef DEBUG_ASTAR_PATH
	for (int i = 0; i < m_PointsChecked.GetCount(); i++)
		{
		int xPos;
		int yPos;
		Xform.Transform(m_PointsChecked[i], &xPos, &yPos);

		Dest.DrawDot(xPos, yPos, CG32bitPixel(0, 255, 0), markerMediumCross);
		}
#endif
	}

void CNavigationPath::DebugPaintInfo (CG32bitImage &Dest, int x, int y, const CMapViewportCtx &Ctx)

//	DebugPaintInfo
//
//	Paint the nav path

	{
	int i;
	int xFrom;
	int yFrom;

	Ctx.Transform(m_vStart, &xFrom, &yFrom);
	for (i = 0; i < m_Waypoints.GetCount(); i++)
		{
		int xTo;
		int yTo;

		Ctx.Transform(m_Waypoints[i].vPos, &xTo, &yTo);

		Dest.DrawLine(xFrom, yFrom,
				xTo, yTo,
				3,
				CG32bitPixel(0,255,0));

		xFrom = xTo;
		yFrom = yTo;
		}
	}

CVector CNavigationPath::GetNavPoint (int iIndex, Metric *retrDist2) const

//	GetNavPoint
//
//	Return the nav point at the given index

	{
	if (iIndex == -1)
		{
		CSpaceObject *pStart = g_pUniverse->FindObject(m_iStartIndex);
		if (pStart)
			{
			if (retrDist2)
				*retrDist2 = DEFAULT_NAV_POINT_RADIUS2;

			return pStart->GetPos();
			}
		else
			{
			if (retrDist2)
				*retrDist2 = m_Waypoints[0].rRange2;

			return m_Waypoints[0].vPos;
			}
		}
	else
		{
		iIndex = Min(iIndex, m_Waypoints.GetCount() - 1);

		if (retrDist2)
			*retrDist2 = m_Waypoints[iIndex].rRange2;

		return m_Waypoints[iIndex].vPos;
		}
	}

bool CNavigationPath::IsLineBlocked (CSystem &System, const CVector &vFrom, const CVector &vTo)

//	IsLineBlocked
//
//	Returns TRUE if the given line intersects any object that blocks ships.

	{
	for (int i = 0; i < System.GetObjectCount(); i++)
		{
		CSpaceObject *pObj = System.GetObject(i);
		if (pObj == NULL || pObj->IsDestroyed() || !pObj->BlocksShips())
			continue;

		CVector vDiag = pObj->GetBoundsDiag();
		CVector vUR = pObj->GetPos() + vDiag;
		CVector vLL = pObj->GetPos() - vDiag;

		if (CAStarPathFinder::LineIntersectsRect(vFrom, vTo, vUR, vLL))
			return true;
		}

	return false;
	}

bool CNavigationPath::Matches (CSovereign *pSovereign, CSpaceObject *pStart, CSpaceObject *pEnd)

//	Matches
//
//	Returns TRUE if this path matches the request

	{
	if (pSovereign && m_pSovereign && pSovereign != m_pSovereign)
		return false;

	if (m_iStartIndex == -1)
		return false;

	if (pStart->GetID() != m_iStartIndex)
		return false;

	if (pEnd->GetID() != m_iEndIndex)
		return false;

	return true;
	}

void CNavigationPath::OnReadFromStream (SLoadCtx &Ctx)

//	OnReadFromStream
//
//	Load a new nav path from a stream
//
//	DWORD		m_dwID
//	DWORD		sovereign UNID
//	DWORD		m_iStartIndex
//	DWORD		m_iEndIndex
//	DWORD		m_iSuccesses
//	DWORD		m_iFailures
//
//	CVector		m_vStart
//	DWORD		m_iWaypointCount
//	CVector		Waypoints

	{
	Ctx.pStream->Read(m_dwID);
	CSystem::ReadSovereignRefFromStream(Ctx, &m_pSovereign);
	Ctx.pStream->Read(m_iStartIndex);
	Ctx.pStream->Read(m_iEndIndex);
	Ctx.pStream->Read(m_iSuccesses);
	Ctx.pStream->Read(m_iFailures);

	if (Ctx.dwVersion >= 13)
		m_vStart.ReadFromStream(*Ctx.pStream);
	
	DWORD dwCount;
	Ctx.pStream->Read(dwCount);

	if (dwCount)
		{
		m_Waypoints.InsertEmpty(dwCount);
		for (int i = 0; i < m_Waypoints.GetCount(); i++)
			m_Waypoints[i].vPos.ReadFromStream(*Ctx.pStream);
		}

	//	We don't store waypoint ranges, so we recompute here.

	ComputeWaypointRange();
	}

void CNavigationPath::OnWriteToStream (IWriteStream *pStream) const

//	OnWriteToStream
//
//	Save navigation path
//
//	DWORD		m_dwID
//	DWORD		sovereign UNID
//	DWORD		m_iStartIndex
//	DWORD		m_iEndIndex
//	DWORD		m_iSuccesses
//	DWORD		m_iFailures
//
//	CVector		m_vStart
//	DWORD		m_iWaypointCount
//	CVector		Waypoints

	{
	pStream->Write(m_dwID);
	CSystem::WriteSovereignRefToStream(m_pSovereign, pStream);
	pStream->Write(m_iStartIndex);
	pStream->Write(m_iEndIndex);

	pStream->Write(m_iSuccesses);
	pStream->Write(m_iFailures);

	m_vStart.WriteToStream(*pStream);

	DWORD dwSave = m_Waypoints.GetCount();
	pStream->Write(dwSave);
	for (int i = 0; i < m_Waypoints.GetCount(); i++)
		m_Waypoints[i].vPos.WriteToStream(*pStream);
	}

bool CNavigationPath::PathIsClear (CSystem *pSystem,
								   CSovereign *pSovereign,
								   const CVector &vFrom, 
								   const CVector &vTo, 
								   CSpaceObject **retpEnemy, 
								   CVector *retvAway)

//	PathIsClear
//
//	Returns TRUE if the path from vFrom to vTo is free from enemy stations.
//	If FALSE, retpEnemy is initialized with the enemy that is blocking the
//	path and retvAway is a unit vector away from the enemy that avoids it

	{
	int i;

	//	Loop over all objects in the system

	CSpaceObject *pNearestEnemy = NULL;
	Metric rNearestDist = MAX_SAFE_DIST;
	CVector vNearestAway;

	for (i = 0; i < pSystem->GetObjectCount(); i++)
		{
		CSpaceObject *pObj = pSystem->GetObject(i);
		CSovereign *pObjSovereign;

		if (pObj
				&& (pObj->GetScale() == scaleStructure 
					|| ((pObj->GetScale() == scaleShip) && (pObj->GetVel().Length2() < MIN_SPEED2)))
				&& (pObjSovereign = pObj->GetSovereign())
				&& (pObjSovereign->IsEnemy(pSovereign))
				&& pObj->CanAttack())
			{
			CVector vAway;
			Metric rDist = CalcDistanceToPath(pObj->GetPos(), vFrom, vTo, NULL, &vAway);
			if (rDist < rNearestDist)
				{
				rNearestDist = rDist;
				pNearestEnemy = pObj;
				vNearestAway = vAway;
				}
			}
		}

	//	If we found a threatening object, return it

	if (pNearestEnemy)
		{
		if (retpEnemy)
			*retpEnemy = pNearestEnemy;
		if (retvAway)
			*retvAway = vNearestAway;
		return false;
		}

	//	Otherwise, the path is OK

	return true;
	}

