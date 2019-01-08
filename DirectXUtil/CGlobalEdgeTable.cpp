//	CGlobalEdgeTable.cpp
//
//	CGlobalEdgeTable class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CGlobalEdgeTable::~CGlobalEdgeTable (void)

//	CGlobalEdgeTable destructor

	{
	CSortedEdgeList *pList = m_pFirst;
	while (pList)
		{
		CSortedEdgeList *pToDelete = pList;
		pList = pList->GetNext();
		delete pToDelete;
		}
	}

void CGlobalEdgeTable::Insert (SEdgeState *pEdge)

//	Insert
//
//	Inserts an edge to the appropriate sorted edge list.

	{
	//	Loop until to find where to put this edge

	CSortedEdgeList *pList = m_pFirst;
	CSortedEdgeList *pLastList = NULL;
	while (pList)
		{
		//	If this is the list for this coordinate, then add it.

		if (pList->GetFirst()->yStart == pEdge->yStart)
			{
			pList->Insert(pEdge);
			return;
			}

		//	Otherwise, if this list is less than the edge list,
		//	then remember it and continue to the next one.

		else if (pList->GetFirst()->yStart < pEdge->yStart)
			{
			pLastList = pList;
			pList = pList->GetNext();
			}

		//	Otherwise, we've gone too far and we're done

		else
			break;
		}

	//	If we did not find a list, then we need to add a new one.

	CSortedEdgeList *pNewList = new CSortedEdgeList;
	pNewList->Insert(pEdge);

	//	Add it to our linked list

	if (pLastList)
		{
		pNewList->SetNext(pLastList->GetNext());
		pLastList->SetNext(pNewList);
		}
	else
		{
		pNewList->SetNext(m_pFirst);
		m_pFirst = pNewList;
		}

	if (pNewList->GetNext() == NULL)
		m_pLast = pNewList;
	}
