//	CGRunList.cpp
//
//	CGRungList Class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CGRunList::CombineRun (int xStart, int cxLength, int iSamples)

//	CombineRun
//
//	Combines a new run with all existing ones.

	{
	ASSERT(cxLength > 0);
	ASSERT(iSamples > 0);

	int xEnd = xStart + cxLength;
	int iRun = 0;
	while (cxLength > 0)
		{
		//	We've reached the end

		if (iRun == m_Runs.GetCount())
			{
			InsertRun(xStart, xEnd, cxLength, iSamples);
			cxLength = 0;
			}

		//	Otherwise, check to see if we intersect this run.

		else
			{
			SRun &Run = m_Runs[iRun];

			//	     Run.xEnd
			//	     v
			//	|----|
			//	        |-----?
			//	        ^
			//	        xStart

			if (xStart >= Run.xEnd)
				iRun++;

			//		  Run.xStart
			//		  v
			//	      |----|
			//	|-----?
			//	^
			//	xStart

			else if (xStart < Run.xStart)
				{
				//	      |----|
				//	|----|

				if (xEnd <= Run.xStart)
					{
					InsertRun(xStart, xEnd, cxLength, iSamples, iRun);
					cxLength = 0;
					}

				//	      |----|
				//	|----------?

				else
					{
					int cxNewLength = Run.xStart - xStart;
					InsertRun(xStart, xStart + cxNewLength, cxNewLength, iSamples, iRun);

					xStart += cxNewLength;
					cxLength -= cxNewLength;
					iRun++;
					}
				}

			//	|----|
			//	|-----?

			else if (xStart == Run.xStart)
				{
				//	|----|
				//	|----?

				if (xEnd >= Run.xEnd)
					{
					Run.iSamples += iSamples;

					xStart += Run.cxLength;
					cxLength -= Run.cxLength;
					iRun++;
					}

				//	|----|
				//	|--|

				else
					{
					int iOriginalSamples = Run.iSamples;
					int cxAdditionalLength = Run.cxLength - cxLength;

					Run.xEnd = xEnd;
					Run.cxLength = cxLength;
					Run.iSamples += iSamples;

					iRun++;
					InsertRun(xEnd, xEnd + cxAdditionalLength, cxAdditionalLength, iOriginalSamples, iRun);
					cxLength = 0;
					}
				}

			//	|-----|
			//	  |-|

			else if (xEnd < Run.xEnd)
				{
				int iASamples = Run.iSamples;
				int iAStart = Run.xStart;
				int iALength = xStart - Run.xStart;

				int iBSamples = Run.iSamples + iSamples;
				int iBStart = xStart;
				int iBLength = cxLength;

				int iCSamples = Run.iSamples;
				int iCStart = xEnd;
				int iCLength = Run.xEnd - xEnd;

				Run.xEnd = iAStart + iALength;
				Run.cxLength = iALength;

				InsertRun(iBStart, iBStart + iBLength, iBLength, iBSamples, iRun + 1);
				InsertRun(iCStart, iCStart + iCLength, iCLength, iCSamples, iRun + 2);

				cxLength = 0;
				}

			//	|-----|
			//	  |----?

			else
				{
				int iASamples = Run.iSamples;
				int iAStart = Run.xStart;
				int iALength = xStart - Run.xStart;

				int iBSamples = Run.iSamples + iSamples;
				int iBStart = xStart;
				int iBLength = Run.xEnd - xStart;

				Run.xEnd = iAStart + iALength;
				Run.cxLength = iALength;

				InsertRun(iBStart, iBStart + iBLength, iBLength, iBSamples, iRun + 1);

				xStart += iBLength;
				cxLength -= iBLength;
				iRun += 2;
				}
			}
		}
	}

void CGRunList::GetRun (int iIndex, int *retxStart, int *retcxLength, int *retiSamples) const

//	GetRun
//
//	Returns the run

	{
	const SRun &Run = m_Runs[iIndex];
	*retxStart = Run.xStart;
	*retcxLength = Run.cxLength;
	*retiSamples = Run.iSamples;
	}

void CGRunList::InsertRun (int xStart, int xEnd, int cxLength, int iSamples, int iPos)

//	InsertRun
//
//	Inserts a new run

	{
	SRun *pRun = m_Runs.InsertAt(iPos);
	pRun->xStart = xStart;
	pRun->xEnd = xEnd;
	pRun->cxLength = cxLength;
	pRun->iSamples = iSamples;
	}

void CGRunList::InsertSampled (int xStartSampled, int cxLengthSampled, int iSamples)

//	InsertSampled
//
//	Inserts an anti-aliased line and combines it with the existing set of runs.

	{
	//	Align to samples

	int xStartAligned = AlignUpSigned(xStartSampled, iSamples);
	int xEndAligned = AlignDownSigned(xStartSampled + cxLengthSampled, iSamples);
	int cxLengthAligned = (xEndAligned - xStartAligned);
	int xEndSampled = xStartSampled + cxLengthSampled;

	if (xStartAligned > xStartSampled)
		CombineRun((xStartAligned / iSamples) - 1, 1, xStartAligned - xStartSampled);

	if (cxLengthAligned > 0)
		CombineRun(xStartAligned / iSamples, cxLengthAligned / iSamples, iSamples);

	if (xEndAligned < xEndSampled)
		CombineRun(xEndAligned / iSamples, 1, xEndSampled - xEndAligned);
	}
