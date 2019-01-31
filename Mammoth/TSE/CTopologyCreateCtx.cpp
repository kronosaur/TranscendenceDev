//	CTopologyCreateCtx.cpp
//
//	CTopologyCreateCtx class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CString STopologyCreateCtx::ExpandNodeID (const CString &sID) const

//	ExpandNodeID
//
//	Expands a Fragment-relative ID to a full ID

	{
	if (!sFragmentPrefix.IsBlank() && *sID.GetASCIIZPointer() == '+')
		return strPatternSubst(CONSTLIT("%s%s"), sFragmentPrefix, sID);
	else
		return sID;
	}

void STopologyCreateCtx::GetAbsoluteDisplayPos (int x, int y, int *retx, int *rety, int *retiRotation) const

//	GetAbsoluteDisplayPos
//
//	Converts to absolute coordinates if we're in a fragment

	{
	if (bInFragment)
		{
		*retiRotation = iRotation;
		if (*retiRotation != 0)
			{
			CVector vPos = CVector(x, y).Rotate(*retiRotation);
			x = mathRound(vPos.GetX());
			y = mathRound(vPos.GetY());
			}

		*retx = x + xOffset;
		*rety = y + yOffset;
		}
	else
		{
		*retiRotation = 0;
		*retx = x;
		*rety = y;
		}
	}

void STopologyCreateCtx::GetFragmentDisplayPos (CTopologyNode *pNode, int *retx, int *rety) const

//	GetFragmentDisplayPos
//
//	Returns the position of the given node in fragment coordinates

	{
	int x, y;
	pNode->GetDisplayPos(&x, &y);

	//	Convert to fragment coordinates

	if (bInFragment)
		{
		x -= xOffset;
		y -= yOffset;

		if (iRotation != 0)
			{
			CVector vPos = CVector(x, y).Rotate(360 - iRotation);
			x = mathRound(vPos.GetX());
			y = mathRound(vPos.GetY());
			}
		}

	//	Done

	*retx = x;
	*rety = y;
	}

void STopologyCreateCtx::GetFragmentEntranceDisplayPos (int *retx, int *rety) const

//	GetFragmentEntranceDisplayPos
//
//	Returns the position of the entrance to the fragment in the appropriate
//	coordinate.

	{
	//	For a true fragment, which uses coordinates local to the fragment, the
	//	entrance is always at 0,0.

	if (bInFragment)
		{
		*retx = 0;
		*rety = 0;
		}

	//	Otherwise, we use the previous node coordinates

	else if (pPrevNode)
		{
		pPrevNode->GetDisplayPos(retx, rety);
		}

	//	Otherwise, we don't know what to do, so we set to 0,0.

	else
		{
		*retx = 0;
		*rety = 0;
		}
	}
