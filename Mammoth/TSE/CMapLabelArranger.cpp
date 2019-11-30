//	CMapLabelArranger.cpp
//
//	CMapLabelArranger class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

static constexpr int LABEL_SPACING_X =								8;
static constexpr int LABEL_SPACING_Y =								4;
static constexpr int LABEL_OVERLAP_Y =								1;

static constexpr Metric MAP_VERTICAL_ADJUST =						1.4;

void CMapLabelArranger::Arrange (CSystem &System)

//	Arrange
//
//	Arranges labels for objects in the system.

	{
	int i;
	const int MAX_LABELS = 100;
	int iLabelCount = 0;
	SLabelEntry Labels[MAX_LABELS];

	//	Compute a transform for map coordinate

	ViewportTransform Trans(CVector(), 
			g_MapKlicksPerPixel, 
			g_MapKlicksPerPixel * MAP_VERTICAL_ADJUST,
			0, 
			0);

	//	Loop over all objects and see if they have a map label

	for (i = 0; i < System.GetObjectCount() && iLabelCount < MAX_LABELS; i++)
		{
		CSpaceObject *pObj = System.GetObject(i);

		int cxLabel, cyLabel;
		if (pObj 
				&& pObj->ShowMapLabel(&cxLabel, &cyLabel)
				&& cxLabel > 0)
			{
			Labels[iLabelCount].pObj = pObj;
			Trans.Transform(pObj->GetPos(), &Labels[iLabelCount].x, &Labels[iLabelCount].y);
			Labels[iLabelCount].cxLabel = cxLabel;
			Labels[iLabelCount].cyLabel = cyLabel;

			if (pObj->GetPos().GetX() > 0.0)
				SetLabelPos(Labels[iLabelCount], CMapLabelPainter::posRight);
			else
				SetLabelPos(Labels[iLabelCount], CMapLabelPainter::posLeft);

			iLabelCount++;
			}
		}

	//	Keep looping until we minimize overlap

	bool bOverlap;
	int iIteration = 0;

	do
		{
		bOverlap = CalcOverlap(Labels, iLabelCount);
		if (bOverlap)
			{
			//	Modify the label location of any overlapping labels

			for (i = 0; i < iLabelCount; i++)
				{
				if (Labels[i].iNewPosition != CMapLabelPainter::posNone)
					SetLabelPos(Labels[i], Labels[i].iNewPosition);
				}

			iIteration++;
			}
		}
	while (bOverlap && iIteration < 10);

	//	Set the label position for all the objects

	for (i = 0; i < iLabelCount; i++)
		Labels[i].pObj->SetMapLabelPos(Labels[i].iPosition);
	}

bool CMapLabelArranger::CalcOverlap (SLabelEntry *pEntries, int iCount)
	{
	bool bOverlap = false;
	int i, j;

	for (i = 0; i < iCount; i++)
		{
		for (j = i + 1; j < iCount; j++)
			{
			if (RectsIntersect(pEntries[i].rcLabel, pEntries[j].rcLabel))
				{
				int xDelta = pEntries[j].x - pEntries[i].x;
				int yDelta = pEntries[j].y - pEntries[i].y;

				switch (pEntries[i].iPosition)
					{
					case CMapLabelPainter::posRight:
						{
						if (xDelta > 0)
							{
							if (pEntries[i].iNewPosition == CMapLabelPainter::posNone)
								{
								pEntries[i].iNewPosition = CMapLabelPainter::posLeft;
								bOverlap = true;
								}
							}
						else
							{
							if (pEntries[j].iNewPosition == CMapLabelPainter::posNone)
								{
								pEntries[j].iNewPosition = CMapLabelPainter::posLeft;
								bOverlap = true;
								}
							}
						break;
						}

					case CMapLabelPainter::posLeft:
						{
						if (xDelta < 0)
							{
							if (pEntries[i].iNewPosition == CMapLabelPainter::posNone)
								{
								pEntries[i].iNewPosition = CMapLabelPainter::posRight;
								bOverlap = true;
								}
							}
						else
							{
							if (pEntries[j].iNewPosition == CMapLabelPainter::posNone)
								{
								pEntries[j].iNewPosition = CMapLabelPainter::posRight;
								bOverlap = true;
								}
							}
						break;
						}
					}

				break;
				}
			}
		}

	return bOverlap;
	}

void CMapLabelArranger::SetLabelPos (SLabelEntry &Entry, CMapLabelPainter::EPositions iPos)
	{
	CMapLabelPainter::CalcLabelRect(Entry.x, Entry.y, Entry.cxLabel, Entry.cyLabel, iPos, Entry.rcLabel);

	//	Shrink the RECT a bit because we tolerate a little bit of overlap.

	Entry.rcLabel.top += LABEL_OVERLAP_Y;
	Entry.rcLabel.bottom -= LABEL_OVERLAP_Y;

	Entry.iPosition = iPos;
	}
