//	CSpaceObjectGrid.cpp
//
//	CSpaceObjectGrid class
//	Copyright (c) 2018 Kronosaur Productions, LLC.

#include "PreComp.h"

CSpaceObjectGrid::CSpaceObjectGrid (int iGridSize, Metric rCellSize, Metric rCellBorder)

//	CSpaceObjectGrid constructor

	{
	ASSERT(iGridSize > 0);
	ASSERT(rCellSize > 0.0);
	ASSERT(rCellBorder > 0.0);

	int iTotal = iGridSize * iGridSize;
	m_pGrid = new SList [iTotal];
	utlMemSet(m_pGrid, sizeof(SList) * iTotal, 0);

	m_Outer.pList = NULL;

	m_iGridSize = iGridSize;
	m_rCellSize = rCellSize;
	m_rCellBorder = rCellBorder;

	Metric rGridSize = iGridSize * m_rCellSize;
	m_vGridSize = CVector(rGridSize, rGridSize);
	m_vLL = CVector(-(rGridSize / 2.0), -(rGridSize / 2.0));
	m_vUR = CVector(rGridSize / 2.0, rGridSize / 2.0);
	}

CSpaceObjectGrid::~CSpaceObjectGrid (void)

//	CSpaceObjectGrid destructor

	{
	delete [] m_pGrid;
	}

void CSpaceObjectGrid::AddObject (CSpaceObject *pObj)

//	AddObject
//
//	Adds an object to the list.
	
	{
	ASSERT(pObj->GetID() != 0xdddddddd);
	
	SList &List = GetList(pObj->GetPos());
	List.pList = m_Pool.AllocObj(List.pList, pObj);
	}

void CSpaceObjectGrid::DebugObjDeleted (CSpaceObject *pObj) const

//	DebugObjDeleted
//
//	pObj has been deleted. Make sure that it is no longer part of the grid. This
//	is used to debug a problem.

	{
	int iTotal = m_iGridSize * m_iGridSize;
	for (int i = 0; i < iTotal; i++)
		{
		if (m_Pool.FindObj(m_pGrid[i].pList, pObj))
			{
			ASSERT(false);
			}
		}
	}

void CSpaceObjectGrid::Delete (CSpaceObject *pObj)

//	Delete
//
//	Delete the given object from the grid. NOTE: This is a relatively expensive
//	operation, so we only do it when we really need to.

	{
	DEBUG_TRY

	int iTotal = m_iGridSize * m_iGridSize;
	for (int i = 0; i < iTotal; i++)
		{
		bool bDeleted;
		m_pGrid[i].pList = m_Pool.DeleteObj(m_pGrid[i].pList, pObj, &bDeleted);
		if (bDeleted)
			{
			//	We assume that an object can only be on one list in the grid.
			return;
			}
		}

	//	If we get this far, then try to delete from the outer list.

	m_Outer.pList = m_Pool.DeleteObj(m_Outer.pList, pObj);

	DEBUG_CATCH
	}

void CSpaceObjectGrid::DeleteAll (void)

//	DeleteAll
//
//	Remove all objects

	{
	int iTotal = m_iGridSize * m_iGridSize;
	utlMemSet(m_pGrid, sizeof(SList) * iTotal, 0);

	m_Outer.pList = NULL;
	}

bool CSpaceObjectGrid::GetGridCoord (const CVector &vPos, int *retx, int *rety) const

//	GetGridCoord
//
//	Returns the grid cell coordinates of vPos (which is in grid-origin coords).
//	We return TRUE if inside the valid area of the grid

	{
	int x = (int)(vPos.GetX() / m_rCellSize);
	int y = (int)(vPos.GetY() / m_rCellSize);

	*retx = x;
	*rety = y;

	return (x >= 0 && y >= 0 && x < m_iGridSize && y < m_iGridSize);
	}

const CSpaceObjectGrid::SList &CSpaceObjectGrid::GetList (const CVector &vPos) const

//	GetList
//
//	Returns the object list at the given position

	{
	CVector vGridPos = vPos - m_vLL;
	int x = (int)(vGridPos.GetX() / m_rCellSize);
	int y = (int)(vGridPos.GetY() / m_rCellSize);

	if (x < 0 || y < 0 || x >= m_iGridSize || y >= m_iGridSize)
		return m_Outer;
	else
		return GetList(x, y);
	}

CSpaceObjectGrid::SList &CSpaceObjectGrid::GetList (const CVector &vPos)

//	GetList
//
//	Returns the object list at the given position

	{
	CVector vGridPos = vPos - m_vLL;
	int x = (int)(vGridPos.GetX() / m_rCellSize);
	int y = (int)(vGridPos.GetY() / m_rCellSize);

	if (x < 0 || y < 0 || x >= m_iGridSize || y >= m_iGridSize)
		return m_Outer;
	else
		return GetList(x, y);
	}

void CSpaceObjectGrid::EnumStart (SSpaceObjectGridEnumerator &i, const CVector &vUR, const CVector &vLL, DWORD dwFlags) const

//	EnumStart
//
//	Begins enumeration

	{
	DEBUG_TRY

	//	Init params and options

	i.vLL = vLL;
	i.vUR = vUR;
	i.bCheckBox = ((dwFlags & gridNoBoxCheck) ? false : true);
	if (i.pGridIndexList)
		{
		delete [] i.pGridIndexList;
		i.pGridIndexList = NULL;
		}

	//	Figure out coordinates relative to the origin of the grid

	CVector vGridLL = vLL - m_vLL;
	CVector vGridUR = vUR - m_vLL;

	//	If requested we add an extra border to the search square because we want
	//	to find objects if their center of just outside the search square.

	if (!(dwFlags & gridNoCellBorder))
		{
		CVector vBorder(m_rCellBorder, m_rCellBorder);
		vGridLL = vGridLL - vBorder;
		vGridUR = vGridUR + vBorder;
		}

	//	Convert to integral grid coordinates

	int xStart = (int)(vGridLL.GetX() / m_rCellSize);
	int yStart = (int)(vGridLL.GetY() / m_rCellSize);

	int xEnd = (int)(vGridUR.GetX() / m_rCellSize);
	int yEnd = (int)(vGridUR.GetY() / m_rCellSize);

	//	If we're completely off the grid, then we just add the outer cell.

	if (xStart >= m_iGridSize || yStart >= m_iGridSize || xEnd < 0 || yEnd < 0)
		{
		i.pGridIndexList = new const SList * [1];
		i.pGridIndexList[0] = &m_Outer;
		i.iGridIndexCount = 1;

		i.bMore = (m_Outer.pList != NULL);
		}

	//	Otherwise we allocate each grid to the list

	else
		{
		int iMaxSize;
		bool bNeedOuter;

		//	Remember the center cell, because we start there.

		int xCenter = xStart + (xEnd - xStart) / 2;
		int yCenter = yStart + (yEnd - yStart) / 2;

		//	Clip the search square to the grid, and if we're partly outside the
		//	grid, remember to include the outer cell.

		if (xStart >= 0 && xEnd < m_iGridSize && yStart >= 0 && yEnd < m_iGridSize)
			{
			iMaxSize = (xEnd - xStart + 1) * (yEnd - yStart + 1);
			bNeedOuter = false;
			}
		else
			{
			xStart = Max(0, xStart);
			xEnd = Min(xEnd, m_iGridSize - 1);
			yStart = Max(0, yStart);
			yEnd = Min(yEnd, m_iGridSize - 1);

			//	+1 because we need outer cell

			iMaxSize = (xEnd - xStart + 1) * (yEnd - yStart + 1) + 1;
			bNeedOuter = true;

			//	We should also adjust the center cell to be in bounds.

			xCenter = Max(0, Min(xCenter, m_iGridSize - 1));
			yCenter = Max(0, Min(yCenter, m_iGridSize - 1));
			}

		//	Generate a list of all grid cells to traverse

		i.pGridIndexList = new const SList * [iMaxSize];
		i.iGridIndexCount = 0;

		const CSpaceObjectGrid::SList *pList = &GetList(xCenter, yCenter);
		if (pList->pList)
			i.pGridIndexList[i.iGridIndexCount++] = pList;

		//	Add the remaining cells

		for (int y = yStart; y <= yEnd; y++)
			for (int x = xStart; x <= xEnd; x++)
				if (x == xCenter && y == yCenter)
					NULL;
				else
					{
					const CSpaceObjectGrid::SList *pList = &GetList(x, y);
					if (pList->pList)
						i.pGridIndexList[i.iGridIndexCount++] = pList;
					}

		//	Add the outer cell last

		if (bNeedOuter && m_Outer.pList)
			{
			i.pGridIndexList[i.iGridIndexCount++] = &m_Outer;
			}

		//	Initialize

		ASSERT(i.iGridIndexCount <= iMaxSize);
		i.bMore = (i.iGridIndexCount > 0);
		}

	//	Initialize

	if (i.bMore)
		{
		i.iGridIndex = 0;
		i.pList = i.pGridIndexList[0];
		i.pNode = NULL;
		i.pObj = NULL;

		//	Start with the first

		EnumGetNext(i);
		}

	DEBUG_CATCH
	}

CSpaceObject *CSpaceObjectGrid::EnumGetNext (SSpaceObjectGridEnumerator &i) const

//	EnumGetNext
//
//	Returns the current object in the enumeration and advances
//	to the next one

	{
	DEBUG_TRY

	//	Get the current object

	CSpaceObject *pCurrentObj = i.pObj;

	//	Next

	do
		{
		//	Advance

		if (i.pNode == NULL)
			i.pNode = i.pList->pList;
		else
			i.pNode = i.pNode->pNext;

		//	If we're still on the current list, then return the node

		if (i.pNode)
			{
			i.pObj = i.pNode->pObj;

			if (!i.pObj->IsDestroyed() 
					&& (!i.bCheckBox || i.pObj->InBox(i.vUR, i.vLL)))
				return pCurrentObj;
			}

		//	Otherwise, next list.

		else
			{
			i.iGridIndex++;
			if (i.iGridIndex < i.iGridIndexCount)
				{
				ASSERT(i.iGridIndex >= 0);

				i.pList = i.pGridIndexList[i.iGridIndex];
				i.pNode = NULL;
				}
			else
				{
				i.bMore = false;
				return pCurrentObj;
				}
			}
		}
	while (true);

	DEBUG_CATCH
	}

CSpaceObject *CSpaceObjectGrid::EnumGetNextFast (SSpaceObjectGridEnumerator &i) const

//	EnumGetNextFast
//
//	Returns the next object in the enumeration

	{
	ASSERT(i.pNode != NULL);

	CSpaceObject *pCurObj = i.pNode->pObj;
	i.pNode = i.pNode->pNext;
	if (i.pNode == NULL)
		EnumGetNextList(i);

	return pCurObj;
	}

CSpaceObject *CSpaceObjectGrid::EnumGetNextInBoxPoint (SSpaceObjectGridEnumerator &i) const

//	EnumGetNextInBoxPoint
//
//	Returns the next object whose center fits inside the defined box

	{
	//	Get the current object

	CSpaceObject *pCurrentObj = i.pObj;

	//	Next

	do
		{
		//	Advance

		if (i.pNode == NULL)
			i.pNode = i.pList->pList;
		else
			i.pNode = i.pNode->pNext;

		//	If we're still on the current list, then return the node

		if (i.pNode)
			{
			i.pObj = i.pNode->pObj;

			if (!i.pObj->IsDestroyed() && i.pObj->InBoxPoint(i.vUR, i.vLL))
				return pCurrentObj;
			}

		//	Otherwise, next list.

		else
			{
			i.iGridIndex++;
			if (i.iGridIndex < i.iGridIndexCount)
				{
				ASSERT(i.iGridIndex >= 0);

				i.pList = i.pGridIndexList[i.iGridIndex];
				i.pNode = NULL;
				}
			else
				{
				i.bMore = false;
				return pCurrentObj;
				}
			}
		}
	while (true);
	}

bool CSpaceObjectGrid::EnumGetNextList (SSpaceObjectGridEnumerator &i) const

//	EnumGetNextList
//
//	Sets up the next list. Returns FALSE if we have reached the end

	{
	i.iGridIndex++;
	if (i.iGridIndex >= i.iGridIndexCount)
		{
		i.bMore = false;
		return false;
		}

	i.pList = i.pGridIndexList[i.iGridIndex];
	i.pNode = i.pList->pList;

	return true;
	}

void CSpaceObjectGrid::GetObjectsInBox (const CVector &vUR, const CVector &vLL, CSpaceObjectList &Result)

//	GetObjectsInBox
//
//	Returns all the objects in the given box 
//
//	Note: We do not check for duplicates before adding to Result

	{
	int x, y;

	//	First we need to generate a box that will contain enough grid cells
	//	so that we can find the objects even if their center is outside
	//	the input range

	CVector vGridLL = vLL - m_vLL - CVector(m_rCellBorder, m_rCellBorder);
	CVector vGridUR = vUR - m_vLL + CVector(m_rCellBorder, m_rCellBorder);

	int xLL = (int)(vGridLL.GetX() / m_rCellSize);
	int yLL = (int)(vGridLL.GetY() / m_rCellSize);

	int xUR = (int)(vGridUR.GetX() / m_rCellSize);
	int yUR = (int)(vGridUR.GetY() / m_rCellSize);

	bool bCheckOuter = true;
	for (y = yLL; y <= yUR; y++)
		for (x = xLL; x <= xUR; x++)
			{
			const SList *pList;
			if (x >= 0 && y >= 0 && x < m_iGridSize && y < m_iGridSize)
				pList = &GetList(x, y);
			else if (bCheckOuter)
				{
				pList = &m_Outer;
				bCheckOuter = false;
				}
			else
				pList = NULL;

			const CSpaceObjectPool::SNode *pNode = pList->pList;
			while (pNode)
				{
				Result.FastAdd(pNode->pObj);
				pNode = pNode->pNext;
				}
			}
	}

void CSpaceObjectGrid::Init (CSystem *pSystem, SUpdateCtx &Ctx)

//	Init
//
//	Initialize the object grid from the system.

	{
	DeleteAll();
	m_Pool.Init(pSystem->GetObjectCount());

	for (int i = 0; i < pSystem->GetObjectCount(); i++)
		{
		CSpaceObject *pObj = pSystem->GetObject(i);
		if (pObj == NULL)
			continue;

		if (pObj->CanBeHit())
			{
			AddObject(pObj);

			//	If this is an object that can block ships, then we remember it
			//	so that we can optimize systems without it.
			//
			//	LATER: We should implement this as a system variable that we
			//	change in create/delete object.

			if (pObj->BlocksShips())
				Ctx.bHasShipBarriers = true;
			}
		}
	}
