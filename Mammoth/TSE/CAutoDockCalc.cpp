//	CAutoDockCalc.cpp
//
//	CAutoDockCalc class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

const CSpaceObject *CAutoDockCalc::GetDockObj (void) const

//	GetDockObj
//
//	Returns the object to dock with.

	{
	//	NOTE: We need to check again to see if the target is destroyed because 
	//	it could have gotten destroyed after it	was picked.

	if (m_pDockingObj && !m_pDockingObj->IsDestroyed())
		return m_pDockingObj;
	else
		return NULL;
	}

void CAutoDockCalc::Update (const CSpaceObject &PlayerObj, const CSpaceObject &Source, const CVector &vPortPos, int iPortIndex, Metric rMaxDist2)

//	Update
//
//	Figure out whether the given dock port is closer to the player than the
//	currently determined port. If so, we accept it as the current port.

	{
	Metric rDist2 = (vPortPos - PlayerObj.GetPos()).Length2();

	//	If this is a better port, then replace the existing 
	//	solution.

	if (rDist2 <= rMaxDist2
			&& (m_pDockingObj == NULL || rDist2 < m_rDockingPortDist2))
		{
		m_pDockingObj = &Source;
		m_iDockingPort = iPortIndex;
		m_rDockingPortDist2 = rDist2;
		m_vDockingPort = vPortPos;
		}
	}
