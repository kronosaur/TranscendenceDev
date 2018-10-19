//	CSpaceObjectPool.cpp
//
//	CSpaceObjectPool class
//	Copyright (c) 2018 Kronosaur Productions, LLC.

#include "PreComp.h"

CSpaceObjectPool::SNode *CSpaceObjectPool::AllocList (CSpaceObject *pObj)

//	AllocList
//
//	Starts a new list with the given object and return the node.

	{
	if (m_iNextNode == m_iSize)
		throw CException(ERR_MEMORY);

	SNode *pList = &m_Pool[m_iNextNode++];
	pList->pObj = pObj;
	pList->pNext = NULL;

	return pList;
	}

CSpaceObjectPool::SNode *CSpaceObjectPool::AllocObj (SNode *pList, CSpaceObject *pObj)

//	AllocObj
//
//	Adds a new object to the given list and returns the newly allocated node,
//	which becomes the new head of the list.

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

CSpaceObjectPool::SNode *CSpaceObjectPool::DeleteObj (SNode *pList, CSpaceObject *pObj)

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
				return pList;
				}
			else
				{
				return pNext->pNext;
				}
			}

		pNext = pNext->pNext;
		}

	//	Not found; just return the original list so that callers don'g have to
	//	do any special checking.

	return pList;
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
