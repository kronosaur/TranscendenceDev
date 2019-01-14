//	CLabelArranger.cpp
//
//	CLabelArranger class
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Euclid.h"

static CVector DIR[8] =
    {
        CVector(0.0, -1.0),
        CVector(1.0, -1.0),
        CVector(1.0, 0.0),
        CVector(1.0, 1.0),
        CVector(0.0, 1.0),
        CVector(-1.0, 1.0),
        CVector(-1.0, 0.0),
        CVector(-1.0, -1.0),
    };

CLabelArranger::CLabelArranger (void) :
        m_iStyle(styleSideColumns),
        m_xCenter(0),
        m_yCenter(0),
        m_iRadius(0),
        m_cxGrid(4),
        m_cyGrid(4)

//  CLabelArranger constructor

    {
    m_rcBounds.left = 0;
    m_rcBounds.top = 0;
    m_rcBounds.right = 0;
    m_rcBounds.bottom = 0;
    }

void CLabelArranger::Arrange (TArray<SLabelDesc> &Labels) const

//  Arrange
//
//  Arrange labels

    {
    //  Arrange based on style

    switch (m_iStyle)
        {
        case styleSideColumns:
            ArrangeSideColumns(Labels);
            break;

        default:
            ASSERT(false);
            break;
        }
    }

void CLabelArranger::ArrangeSideColumn (TArray<SLabelDesc> &Labels, const TSortMap<Metric, int> &Sort, int cxColumn, bool bLeftSide) const

//  ArrangeSideColumn
//
//  Arranges a column either to the left or right

    {
    int i;
    bool bFullHeight = false;

    //  Compute the height of the column.

    int cyHeight = 0;
    int cyInc = 0;
    for (i = 0; i < Sort.GetCount(); i++)
        {
        const SLabelDesc &Label = Labels[Sort[i]];
        cyHeight += AlignUp(Label.cyHeight, m_cyGrid);
        }

    //  If our height exceeds the bounds, then we degrade down to cramming all
    //  entries into the height.

    if (cyHeight > RectHeight(m_rcBounds)
            || bFullHeight)
        {
        cyHeight = RectHeight(m_rcBounds);
        cyInc = cyHeight / (Sort.GetCount() > 0 ? Sort.GetCount() : 1);
        }

    //  Left columns are right aligned; Right columns are left aligned.

    int xCol = (bLeftSide ? m_rcBounds.left + cxColumn : m_rcBounds.right - cxColumn);
    int yCol = m_rcBounds.top + (RectHeight(m_rcBounds) - cyHeight) / 2;

    //  Now position every column

    int yPos = yCol;
    for (i = 0; i < Sort.GetCount(); i++)
        {
        SLabelDesc &Label = Labels[Sort[i]];

        //  If we have a radius, we offset the columns a bit

        int xOffset = 0;
        if (m_iRadius > 0)
            {
            int yCirclePos = abs((yPos + Label.cyHeight / 2) - m_yCenter);
            Metric rTheta = asin((Metric)yCirclePos / (Metric)m_iRadius);
            Metric rX = (Metric)m_iRadius * cos(rTheta);
            xOffset = Max(0, m_iRadius - (int)ceil(rX));
            }

        //  Set label

        Label.rcRect.left = (bLeftSide ? xCol - Label.cxWidth + xOffset : xCol - xOffset);
        Label.rcRect.top = yPos;
        Label.rcRect.right = Label.rcRect.left + Label.cxWidth;
        Label.rcRect.bottom = Label.rcRect.top + Label.cyHeight;

        if (cyInc > 0)
            yPos += cyInc;
        else
            yPos += AlignUp(Label.cyHeight, m_cyGrid);
        }
    }

void CLabelArranger::ArrangeSideColumns (TArray<SLabelDesc> &Labels) const

//  ArrangeSideColumns
//
//  Arrange labels around a circle

    {
    int i;
    bool bBalance = false;

    struct SContext
        {
        Metric rOrder;                      //  Use to order from top to bottom (highest first)
        bool bLeftCol;                      //  TRUE if we're on the left
        };

    //  Generate two sorted columns, left and right

    TSortMap<Metric, int> LeftCol(DescendingSort);
    LeftCol.GrowToFit(Labels.GetCount());

    TSortMap<Metric, int> RightCol(DescendingSort);
    RightCol.GrowToFit(Labels.GetCount());

	//	Keep track of unbound labels, sorted alphabetically

	TSortMap<CString, int> Unbound;

    int cxColumn = 0;
    for (i = 0; i < Labels.GetCount(); i++)
        {
        const SLabelDesc &Label = Labels[i];

		//	Labels that are at the center are unbound

		if (Label.xDest == m_xCenter && Label.yDest == m_yCenter)
			Unbound.Insert(Label.sLabel, i);
		else
			{
			int yDiff = m_yCenter - Label.yDest;
			Metric rYAdj = pow((Metric)abs(yDiff) / (RectHeight(m_rcBounds) / 2), 1.5);

			CVector vPos(Label.xDest - m_xCenter, rYAdj * yDiff);
			Metric rAngle = vPos.Polar();

			if (cos(rAngle) < 0.0)
				LeftCol.Insert(sin(rAngle), i);
			else
				RightCol.Insert(sin(rAngle), i);
			}

		if (Label.cxWidth > cxColumn)
			cxColumn = Label.cxWidth;
        }

    cxColumn = AlignUp(cxColumn, m_cxGrid);

	//	If we have unbound labels, add them around the two columns.

	if (Unbound.GetCount() > 0)
		{
		int iRemaining = Unbound.GetCount();
		int iAddLeftTop = 0;
		int iAddLeftBottom = 0;
		int iAddRightTop = 0;
		int iAddRightBottom = 0;

		if (LeftCol.GetCount() >= RightCol.GetCount())
			{
			int iDiff = Min(iRemaining, LeftCol.GetCount() - RightCol.GetCount());
			int iDiffHalf = iDiff / 2;
			iAddRightTop += iDiffHalf;
			iAddRightBottom += (iDiff - iDiffHalf);
			iRemaining -= iDiff;
			}
		else
			{
			int iDiff = Min(iRemaining, RightCol.GetCount() - LeftCol.GetCount());
			int iDiffHalf = iDiff / 2;
			iAddLeftTop += iDiffHalf;
			iAddLeftBottom += (iDiff - iDiffHalf);
			iRemaining -= iDiff;
			}

		int iAddLeft = iRemaining / 2;
		int iLeftHalf = iAddLeft / 2;
		iAddLeftTop += iLeftHalf;
		iAddLeftBottom += (iAddLeft - iLeftHalf);

		int iAddRight = iRemaining - iAddLeft;
		int iRightHalf = iAddRight / 2;
		iAddRightTop += iRightHalf;
		iAddRightBottom += (iAddRight - iRightHalf);

		int iNext = 0;

		Metric rPos = (LeftCol.GetCount() > 0 ? LeftCol.GetKey(0) + 100.0 : 100.0);
		Metric rInc = -0.01;
		for (i = 0; i < iAddLeftTop; i++, iNext++, rPos += rInc)
			LeftCol.Insert(rPos, Unbound[iNext]);

		rPos = (LeftCol.GetCount() > 0 ? LeftCol.GetKey(LeftCol.GetCount() - 1) - 0.01 : 0.0);
		for (i = 0; i < iAddLeftBottom; i++, iNext++, rPos += rInc)
			LeftCol.Insert(rPos, Unbound[iNext]);

		rPos = (RightCol.GetCount() > 0 ? RightCol.GetKey(0) + 100.0 : 100.0);
		for (i = 0; i < iAddRightTop; i++, iNext++, rPos += rInc)
			RightCol.Insert(rPos, Unbound[iNext]);

		rPos = (RightCol.GetCount() > 0 ? RightCol.GetKey(RightCol.GetCount() - 1) - 0.01 : 0.0);
		for (i = 0; i < iAddRightBottom; i++, iNext++, rPos += rInc)
			RightCol.Insert(rPos, Unbound[iNext]);
		}

    //  If left and right are unbalanced, move entries around

    if (bBalance)
        {
        while (true)
            {
            int iDiff = Absolute(LeftCol.GetCount() - RightCol.GetCount());
            if (iDiff <= 3)
                break;

            if (LeftCol.GetCount() < RightCol.GetCount())
                {
                if (!BalanceColumns(RightCol, LeftCol))
                    break;
                }
            else
                {
                if (!BalanceColumns(LeftCol, RightCol))
                    break;
                }
            }
        }

    //  Arrange each column

    ArrangeSideColumn(Labels, LeftCol, cxColumn, true);
    ArrangeSideColumn(Labels, RightCol, cxColumn, false);
    }

bool CLabelArranger::BalanceColumns (TSortMap<Metric, int> &From, TSortMap<Metric, int> &To) const

//  BalanceColumns
//
//  Moves a single item from one column to the other. We return FALSE if we cannot
//  move any more items.

    {
    if (From.GetCount() < 2)
        return false;

    //  Figure out which label to move. We move the one with the largest absolute value.

    if (From.GetKey(0) > From.GetKey(From.GetCount() - 1))
        {
        To.Insert(To.GetKey(0) + 1.0, From[0]);
        From.Delete(0);
        }
    else
        {
        To.Insert(To.GetKey(To.GetCount() - 1) - 1.0, From[From.GetCount() - 1]);
        From.Delete(From.GetCount() - 1);
        }

    return true;
    }

void CLabelArranger::SetBounds (const RECT &rcRect)

//  SetBounds
//
//  Set the bounds of the arrangement.

    {
    m_rcBounds = rcRect;
    RectCenter(m_rcBounds, &m_xCenter, &m_yCenter);
    }
