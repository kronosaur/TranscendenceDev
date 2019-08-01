//	CSortedEdgeList.cpp
//
//	CSortedEdgeList class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CSortedEdgeList::Advance (void)

//	Advance
//
//	Advances by one pixel (down), adjusting x values and removing any edges
//	that are done.

	{
	SEdgeState *pCurrentEdge = m_pFirst;
	SEdgeState *pPrev = NULL;

	//	Count down and remove or advance each edge in the AET

	while (pCurrentEdge)
		{
		//	Count off one scan line for this edge

		if ((--(pCurrentEdge->iCount)) == 0)
			{
			//	This edge is finished, so remove it from the AET

			pCurrentEdge = pCurrentEdge->pNextEdge;

			if (pPrev)
				pPrev->pNextEdge = pCurrentEdge;
			else
				m_pFirst = pCurrentEdge;
			}
		else 
			{
			//	Advance the edge's X coordinate by minimum move

			pCurrentEdge->x += pCurrentEdge->xWholePixelMove;

			//	Determine whether it's time for X to advance one extra
			
			if ((pCurrentEdge->iErrorTerm +=
					pCurrentEdge->iErrorTermAdjUp) > 0)
				{
				pCurrentEdge->x += pCurrentEdge->xDirection;
				pCurrentEdge->iErrorTerm -= pCurrentEdge->iErrorTermAdjDown;
				}

			pPrev = pCurrentEdge;
			pCurrentEdge = pCurrentEdge->pNextEdge;
			}
		}
	}

void CSortedEdgeList::Insert (SEdgeState *pEdge)

//	Insert
//
//	Inserts sorted by x coordinate. NOTE: We assume that pEdge is not part of 
//	any other list.
	
	{
	SEdgeState *pNext = m_pFirst;
	SEdgeState *pPrev = NULL;

	while (true)
		{
		if (pNext == NULL
				|| pNext->x >= pEdge->x)
			{
			if (pPrev)
				pPrev->pNextEdge = pEdge;
			else
				m_pFirst = pEdge;

			pEdge->pNextEdge = pNext;
			break;
			}

		pPrev = pNext;
		pNext = pNext->pNextEdge;
		}
	}

void CSortedEdgeList::Insert (CSortedEdgeList &EdgeList)

//	Insert
//
//	Destructively inserts the edges from EdgeList and adds them to this list
//	preserving sorted order.

	{
	SEdgeState *pDestNext = m_pFirst;
	SEdgeState *pDestPrev = NULL;
	SEdgeState *pSrcNext = EdgeList.m_pFirst;

	while (pSrcNext)
		{
		//	If this is the right spot, insert it

		if (pDestNext == NULL
				|| pDestNext->x >= pSrcNext->x)
			{
			if (pDestPrev)
				pDestPrev->pNextEdge = pSrcNext;
			else
				m_pFirst = pSrcNext;

			//	Get the next source

			SEdgeState *pInserted = pSrcNext;
			pSrcNext = pSrcNext->pNextEdge;

			pInserted->pNextEdge = pDestNext;

			//	Update previous

			pDestPrev = pInserted;
			}

		//	Otherwise, advance

		else
			{
			pDestPrev = pDestNext;
			pDestNext = pDestNext->pNextEdge;
			}
		}

	EdgeList.m_pFirst = NULL;
	}

void CSortedEdgeList::Sort (void)

//	Sort
//
//	Sorts the list by x values

	{
	if (m_pFirst == NULL)
		return;

	//	Scan through the AET and swap any adjacent edges for which the
	//	second edge is at a lower current X coord than the first edge.
	//	Repeat until no further swapping is needed

	bool bSwapOccurred;

	do
		{
		bSwapOccurred = false;

		SEdgeState *pCurrentEdge = m_pFirst;
		SEdgeState *pPrev = NULL;
		SEdgeState *pNext = pCurrentEdge->pNextEdge;

		while (pNext != NULL)
			{
			if (pCurrentEdge->x > pNext->x)
				{
				//	The second edge has a lower X than the first;
				//	swap them in the AET

				SEdgeState *pTempEdge = pNext->pNextEdge;
				if (pPrev)
					pPrev->pNextEdge = pNext;
				else
					m_pFirst = pNext;

				pNext->pNextEdge = pCurrentEdge;
				pCurrentEdge->pNextEdge = pTempEdge;

				pPrev = pNext;
				pNext = pCurrentEdge->pNextEdge;

				bSwapOccurred = true;
				}
			else
				{
				pPrev = pCurrentEdge;
				pCurrentEdge = pCurrentEdge->pNextEdge;
				pNext = pCurrentEdge->pNextEdge;
				}
			}
		}
	while (bSwapOccurred);
	}
