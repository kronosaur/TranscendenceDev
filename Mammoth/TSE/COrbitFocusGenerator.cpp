//	COrbitFocusGenerator.cpp
//
//	COrbitFocusGenerator class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CVector3D COrbitFocusGenerator::GetValue (int iIndex) const

//	GetValue
//
//	Returns the focus.

	{
	if (m_ZOffset.IsEmpty())
		return m_vCenter;
	else
		{
		CVector3D vOffset(0.0, 0.0, m_ZOffset.Roll() * m_rScale);
		return m_vCenter + vOffset;
		}
	}

ALERROR COrbitFocusGenerator::InitFromXML (SSystemCreateCtx &Ctx, const CXMLElement &XMLDesc, const CVector3D &vCenter, int iCount)

//	InitFromXML
//
//	Initializes.

	{
	m_vCenter = vCenter;
	m_ZOffset = Ctx.ZAdjust.ZOffset;
	m_rScale = Ctx.ZAdjust.rScale;

	//	Since we handled the adjustment here, we don't pass it down to our
	//	children. Otherwise, it might get applied twice (e.g., if we have
	//	sibling directives).

	Ctx.ZAdjust.ZOffset = DiceRange();

	return NOERROR;
	}
