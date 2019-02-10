//	CMapLabelArranger.cpp
//
//	CMapLabelArranger class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

static constexpr int LABEL_SPACING_X =								8;
static constexpr int LABEL_SPACING_Y =								4;
static constexpr int LABEL_OVERLAP_Y =								1;

static constexpr Metric MAP_VERTICAL_ADJUST =						1.4;

void CMapLabelArranger::Arrange (CSystem *pSystem)

//	Arrange
//
//	Arranges labels for objects in the system.

	{
	if (pSystem == NULL)
		return;

	int i;
	const int MAX_LABELS = 100;
	int iLabelCount = 0;
	SLabelEntry Labels[MAX_LABELS];
	const CG16bitFont &MapLabelFont = pSystem->GetUniverse().GetNamedFont(CUniverse::fontMapLabel);

	//	Compute some font metrics

	int cxChar = MapLabelFont.GetAverageWidth();
	int cyChar = MapLabelFont.GetHeight();

	//	Compute a transform for map coordinate

	ViewportTransform Trans(CVector(), 
			g_MapKlicksPerPixel, 
			g_MapKlicksPerPixel * MAP_VERTICAL_ADJUST,
			0, 
			0);

	//	Loop over all objects and see if they have a map label

	for (i = 0; i < pSystem->GetObjectCount() && iLabelCount < MAX_LABELS; i++)
		{
		CSpaceObject *pObj = pSystem->GetObject(i);

		if (pObj && pObj->HasMapLabel())
			{
			Labels[iLabelCount].pObj = pObj;
			Trans.Transform(pObj->GetPos(), &Labels[iLabelCount].x, &Labels[iLabelCount].y);
			Labels[iLabelCount].cxLabel = MapLabelFont.MeasureText(pObj->GetNounPhrase(nounTitleCapitalize));

			SetLabelLeft(Labels[iLabelCount], cyChar);

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
				switch (Labels[i].iNewPosition)
					{
					case posRight:
						{
						SetLabelRight(Labels[i], cyChar);
						break;
						}

					case posLeft:
						{
						SetLabelLeft(Labels[i], cyChar);
						break;
						}

					case posBottom:
						{
						SetLabelBelow(Labels[i], cyChar);
						break;
						}
					}
				}

			iIteration++;
			}
		}
	while (bOverlap && iIteration < 10);

	//	Set the label position for all the objects

	for (i = 0; i < iLabelCount; i++)
		Labels[i].pObj->SetMapLabelPos(Labels[i].iPosition);
	}

void CMapLabelArranger::CalcLabelPos (const CString &sLabel, EPositions iPos, int &xMapLabel, int &yMapLabel)

//	CalcLabelPos
//
//	Calculate the position of the label relative to the object center.

	{
	const CG16bitFont &Font = g_pUniverse->GetNamedFont(CUniverse::fontMapLabel);
	int cyLabel = Font.GetHeight();

	switch (iPos)
		{
		case posLeft:
			{
			int cxLabel = Font.MeasureText(sLabel);
			xMapLabel = -(LABEL_SPACING_X + cxLabel);
			yMapLabel = -(cyLabel / 2);
			break;
			}

		case posBottom:
			{
			int cxLabel = Font.MeasureText(sLabel);
			xMapLabel = -(cxLabel / 2);
			yMapLabel = LABEL_SPACING_Y;
			break;
			}

		//	Defaults to posRight

		default:
			xMapLabel = LABEL_SPACING_X;
			yMapLabel = -(cyLabel / 2);
			break;
		}
	}

bool CMapLabelArranger::CalcOverlap (SLabelEntry *pEntries, int iCount)
	{
	bool bOverlap = false;
	int i, j;

	for (i = 0; i < iCount; i++)
		{
		pEntries[i].iNewPosition = posNone;

		for (j = 0; j < iCount; j++)
			if (i != j)
				{
				if (RectsIntersect(pEntries[i].rcLabel, pEntries[j].rcLabel))
					{
					int xDelta = pEntries[j].x - pEntries[i].x;
					int yDelta = pEntries[j].y - pEntries[i].y;

					switch (pEntries[i].iPosition)
						{
						case posRight:
							{
							if (xDelta > 0)
								pEntries[i].iNewPosition = posLeft;
							break;
							}

						case posLeft:
							{
							if (xDelta < 0)
								pEntries[i].iNewPosition = posBottom;
							break;
							}
						}

					bOverlap = true;
					break;
					}
				}
		}

	return bOverlap;
	}

void CMapLabelArranger::SetLabelBelow (SLabelEntry &Entry, int cyChar)
	{
	Entry.rcLabel.top = Entry.y + LABEL_SPACING_Y + LABEL_OVERLAP_Y;
	Entry.rcLabel.bottom = Entry.rcLabel.top + cyChar - (2 * LABEL_OVERLAP_Y);
	Entry.rcLabel.left = Entry.x - (Entry.cxLabel / 2);
	Entry.rcLabel.right = Entry.rcLabel.left + Entry.cxLabel;

	Entry.iPosition = posBottom;
	}

void CMapLabelArranger::SetLabelLeft (SLabelEntry &Entry, int cyChar)
	{
	Entry.rcLabel.left = Entry.x - (LABEL_SPACING_X + Entry.cxLabel);
	Entry.rcLabel.top = Entry.y - (cyChar / 2) + LABEL_OVERLAP_Y;
	Entry.rcLabel.right = Entry.rcLabel.left + Entry.cxLabel;
	Entry.rcLabel.bottom = Entry.rcLabel.top + cyChar - (2 * LABEL_OVERLAP_Y);

	Entry.iPosition = posLeft;
	}

void CMapLabelArranger::SetLabelRight (SLabelEntry &Entry, int cyChar)
	{
	Entry.rcLabel.left = Entry.x + LABEL_SPACING_X;
	Entry.rcLabel.top = Entry.y - (cyChar / 2) + LABEL_OVERLAP_Y;
	Entry.rcLabel.right = Entry.rcLabel.left + Entry.cxLabel;
	Entry.rcLabel.bottom = Entry.rcLabel.top + cyChar - (2 * LABEL_OVERLAP_Y);

	Entry.iPosition = posRight;
	}
