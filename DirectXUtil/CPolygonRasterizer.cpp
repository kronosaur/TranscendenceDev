//	CPolygonRasterizer.cpp
//
//	CPolygonRasterizer class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CPolygonRasterizer::CPolygonRasterizer (const CGPath &Path, int iAntiAlias)

//	CPolygonRasterizer constructor

	{
	int i;

	ASSERT(iAntiAlias >= 1);

	//	Initialize these because they are used by AddPolygonToGET

	m_iAntiAlias = iAntiAlias;

	//	Loop over all sub-paths

	for (i = 0; i < Path.GetSubPathCount(); i++)
		{
		//	Get a closed polygon path for the sub-path

		TArray<CVector> Points;
		if (Path.GetSubPathPolygonPoints(i, &Points) < 3)
			continue;

		//	Add all the edges to the global edge table

		AddPolygonToGET(Points);
		}
	}

CPolygonRasterizer::~CPolygonRasterizer (void)

//	CPolygonRasterizer destructor

	{
	int i;

	for (i = 0; i < m_Edges.GetCount(); i++)
		delete [] m_Edges[i];
	}

void CPolygonRasterizer::AddPolygonToGET (const TArray<CVector> &Points)

//	AddPolygonToGET
//
//	Adds the polygon represented by the given set of points to the Global
//	Edge Table.

	{
	int i;

	ASSERT(Points.GetCount() >= 3);

	//	Allocate room in the backbone of edges

	SEdgeState *pNewEdgeArray = new SEdgeState[Points.GetCount()];
	m_Edges.Insert(pNewEdgeArray);
	SEdgeState *pNewEdge = pNewEdgeArray;

	Metric rAntiAlias = m_iAntiAlias;

	//	Loop over all points

	for (i = 0; i < Points.GetCount(); i++)
		{
		//	Calculate the two integer points of the edge (we use sub-pixel
		//	sampling for anti-aliasing).

		int xStart = (int)floor((Points[i].GetX() * rAntiAlias));
		int yStart = (int)floor((Points[i].GetY() * rAntiAlias));

		int iEndIndex = (i == 0 ? Points.GetCount() - 1 : i - 1);
		int xEnd = (int)floor((Points[iEndIndex].GetX() * rAntiAlias));
		int yEnd = (int)floor((Points[iEndIndex].GetY() * rAntiAlias));

		//	Make sure the edge runs top to bottom

		if (yStart > yEnd)
			{
			Swap(xStart, xEnd);
			Swap(yStart, yEnd);
			}

		//	Skip if this can’t ever be an active edge (has 0 height)

		int yDelta = yEnd - yStart;
		if (yDelta == 0)
			continue;

		int xDelta = xEnd - xStart;
		int cxEdgeWidth = Absolute(xDelta);

		//	Initialize the edge

		pNewEdge->xDirection = (xDelta > 0 ? 1 : -1);
		pNewEdge->x = xStart;
		pNewEdge->yStart = yStart;
		pNewEdge->iCount = yDelta;
		pNewEdge->iErrorTermAdjDown = yDelta;
		if (xDelta >= 0)
			//	Initial error term going L->R
			pNewEdge->iErrorTerm = 0;
		else
			//	Initial error term going R->L
			pNewEdge->iErrorTerm = -yDelta + 1;

		if (yDelta >= cxEdgeWidth)
			{
			//	Y-major edge
			pNewEdge->xWholePixelMove = 0;
			pNewEdge->iErrorTermAdjUp = cxEdgeWidth;
			}
		else
			{
			//	X-major edge
			pNewEdge->xWholePixelMove = (cxEdgeWidth / yDelta) * pNewEdge->xDirection;
			pNewEdge->iErrorTermAdjUp = cxEdgeWidth % yDelta;
			}

		//	Insert to the global edge table (in order)

		m_GET.Insert(pNewEdge);

		//	Next

		pNewEdge++;
		}
	}

void CPolygonRasterizer::Rasterize (CGRegion *retRegion)

//	Rasterize
//
//	Convert to scan lines

	{
	retRegion->CleanUp();
	if (m_GET.GetFirst() == NULL)
		return;

	//	Since GET is sorted, this is the smallest y value

	CSortedEdgeList *pNextList = m_GET.GetFirst();
	int yCurrent = pNextList->GetFirst()->yStart;
	int yMax = m_GET.GetLast()->GetFirst()->yStart;

	TArray<CGRunList> Lines(100);
	Lines.GrowToFit((yMax - yCurrent) / m_iAntiAlias);

	int yLine = (yCurrent - (yCurrent < 0 ? (m_iAntiAlias - 1) : 0)) / m_iAntiAlias;
	int yLineAntiAlias = yCurrent - (yLine * m_iAntiAlias);
	CGRunList *pCurLine = NULL;

	//	Keep looping over all scan lines

	while (pNextList || !m_AET.IsEmpty())
		{
		//	Move the edges for this scan line from the GET to the Active Edge
		//	Table.

		if (pNextList && yCurrent == pNextList->GetFirst()->yStart)
			{
			m_AET.Insert(*pNextList);
			pNextList = pNextList->GetNext();
			}

		//	Scan out lines

		if (pCurLine == NULL)
			{
			pCurLine = Lines.Insert();
			pCurLine->SetY(yLine);
			}

		RasterizeLine(*pCurLine);

		//	Advance AET edges 1 scan line

		m_AET.Advance();

		//	Resort now that we've changed x

		m_AET.Sort();

		//	Next

		yCurrent++;
		if (++yLineAntiAlias == m_iAntiAlias)
			{
			yLine++;
			yLineAntiAlias = 0;

			pCurLine = NULL;
			}
		}

	//	Convert the run list into a region. Lines is already sorted by y value,
	//	so we can enter it directly.

	retRegion->InitFromRunList(Lines, m_iAntiAlias, true);
	}

void CPolygonRasterizer::RasterizeLine (CGRunList &Runs)

//	RasterizeLine
//
//	Rasterizes the current line using the AET.

	{
	int xLeft;
	SEdgeState *pCurrentEdge;

	//	Scan through the AET, drawing line segments as each pair of edge
	//	crossings is encountered. The nearest pixel on or to the right
	//	of left edges is drawn, and the nearest pixel to the left of but
	//	not on right edges is drawn

	pCurrentEdge = m_AET.GetFirst();
	while (pCurrentEdge)
		{
		xLeft = pCurrentEdge->x;

		//	Advance to the next edge (we can guarantee that there are an even
		//	number of edges because the polygon is closed).

		pCurrentEdge = pCurrentEdge->pNextEdge;
		int xRight = pCurrentEdge->x;

		//	Add a new run

		if (xLeft != xRight)
			Runs.InsertSampled(xLeft, xRight - xLeft, m_iAntiAlias);

		//	Next

		pCurrentEdge = pCurrentEdge->pNextEdge;
		}
	}
