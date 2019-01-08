//	CGRegion.cpp
//
//	CGRegion class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CGRegion::~CGRegion (void)

//	CGRegion destructor

	{
	CleanUp();
	}

void CGRegion::CleanUp (void)

//	CleanUp
//
//	Free all

	{
	int i;

	for (i = 0; i < m_Rows.GetCount(); i++)
		delete m_Rows[i];

	m_Rows.DeleteAll();
	}

void CGRegion::InitFromRect (int x, int y, int cxWidth, int cyHeight, DWORD dwAlpha)

//	InitFromRect
//
//	Initializes from a rectangle.
//	NOTE: We expect these coordinates to be in bounds.

	{
	ASSERT(x >= 0 && y >= 0 && cxWidth > 0 && cyHeight > 0);

	CleanUp();
	m_xOffset = 0;

	int yRow;

	int yEnd = y + cyHeight;
	for (yRow = y; yRow < yEnd; yRow++)
		{
		//	Allocate either one or two runs, depending.

		DWORD *pRow;
		DWORD *pRun;
		if (x == 0)
			{
			pRow = new DWORD [2];
			pRun = pRow;
			*pRun++ = MakeRun(cxWidth, MakeOpacityFromAlpha(dwAlpha));
			}
		else
			{
			pRow = new DWORD [3];
			pRun = pRow;
			*pRun++ = MakeRun(x, 0);
			*pRun++ = MakeRun(cxWidth, MakeOpacityFromAlpha(dwAlpha));
			}

		//	Terminate

		*pRun++ = MakeRun(0, 0);

		//	Add to the row list

		m_Rows.SetAt(yRow, pRow);
		}
	}

void CGRegion::InitFromRunList (const TArray<CGRunList> &Runs, int iSamples, bool bSorted)

//	InitFromRunList
//
//	Initializes from a list of runs.

	{
	int i, j;

	CleanUp();
	if (Runs.GetCount() == 0)
		return;

	//	First figure out the lowest x value

	m_xOffset = Runs[0].GetMinX();
	for (i = 1; i < Runs.GetCount(); i++)
		{
		int xNewOffset = Runs[i].GetMinX();
		if (xNewOffset < m_xOffset)
			m_xOffset = xNewOffset;
		}

	//	Loop over all lines

	DWORD dwMaxSamples = iSamples * iSamples;
	m_Rows.GrowToFit(Runs.GetCount());

	for (i = 0; i < Runs.GetCount(); i++)
		{
		const CGRunList &RunList = Runs[i];

		//	Allocate enough DWORDs for each run plus a spacing run in between.

		DWORD *pRow = new DWORD [2 * RunList.GetCount() + 1];
		DWORD *pRun = pRow;
		int xCurrent = m_xOffset;

		//	Loop over all input runs and construct the row.

		for (j = 0; j < RunList.GetCount(); j++)
			{
			int xStart;
			int cxLength;
			int iSamples;
			RunList.GetRun(j, &xStart, &cxLength, &iSamples);

			//	Advance to the appropriate x

			if (xStart > xCurrent)
				*pRun++ = MakeRun(xStart - xCurrent, 0);

			//	Add the run

			*pRun++ = MakeRun(cxLength, MakeOpacity(iSamples, dwMaxSamples));
			xCurrent = xStart + cxLength;
			}

		//	Terminate

		*pRun++ = MakeRun(0, 0);

		//	Add to the row list. If the caller says the array is already sorted, 
		//	then we add it directly.

		if (bSorted)
			m_Rows.InsertSorted(RunList.Y(), pRow);
		else
			{
			bool bNew;
			m_Rows.SetAt(RunList.Y(), pRow, &bNew);
			ASSERT(bNew);
			}
		}
	}
