//	CSpaceObjectPool.cpp
//
//	CSpaceObjectPool class
//	Copyright (c) 2018 Kronosaur Productions, LLC.

#include "PreComp.h"

CSpaceObjectPool::SNode *CSpaceObjectPool::AllocObj (SNode *pList, CSpaceObject *pObj)

//	AllocObj
//
//	Adds a new object to the given list and returns the newly allocated node,
//	which becomes the new head of the list. pList may be NULL.

	{
	if (m_iNextNode == m_iSize)
		throw CException(ERR_MEMORY);

	SNode *pNewNode = &m_Pool[m_iNextNode++];
	pNewNode->pObj = pObj;
	pNewNode->pNext = pList;

	return pNewNode;
	}

void CSpaceObjectPool::DeleteAll (void)

//	DeleteAll
//
//	Delete the pool. Callers must call Init again before allocating.

	{
	if (m_Pool)
		{
		delete [] m_Pool;
		m_Pool = NULL;
		}

	m_iSize = 0;
	m_iNextNode = 0;
	}

CSpaceObjectPool::SNode *CSpaceObjectPool::DeleteObj (SNode *pList, CSpaceObject *pObj, bool *retbDeleted)

//	DeleteObj
//
//	Deletes the given object from the given list. Returns the new head of the 
//	list.
//
//	LATER: For now, deleted nodes are leaked, but in the future we could add 
//	them to a free list.

	{
	SNode *pPrev = NULL;
	SNode *pNext = pList;
	while (pNext)
		{
		if (pNext->pObj == pObj)
			{
			if (pPrev)
				{
				pPrev->pNext = pNext->pNext;
				if (retbDeleted) *retbDeleted = true;
				return pList;
				}
			else
				{
				if (retbDeleted) *retbDeleted = true;
				return pNext->pNext;
				}
			}

		pPrev = pNext;
		pNext = pNext->pNext;
		}

	//	Not found; just return the original list so that callers don'g have to
	//	do any special checking.

	if (retbDeleted) *retbDeleted = false;
	return pList;
	}

bool CSpaceObjectPool::FindObj (SNode *pList, CSpaceObject *pObj) const

//	FindObj
//
//	Find object in the given list

	{
	SNode *pNext = pList;
	while (pNext)
		{
		if (pNext->pObj == pObj)
			return true;

		pNext = pNext->pNext;
		}

	return false;
	}

void CSpaceObjectPool::Init (int iSize)

//	Init
//
//	Initializes the pool to hold at least the given number of objects. If we try
//	to allocate more than this amount, we throw ERR_MEMORY.

	{
	ASSERT(iSize >= 0);

	DeleteAll();
	if (iSize > 0)
		{
		m_Pool = new SNode [iSize];
		m_iSize = iSize;
		m_iNextNode = 0;
		}
	}
