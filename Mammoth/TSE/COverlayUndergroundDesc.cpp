//	COverlayUndergroundDesc.cpp
//
//	COverlayUndergroundDesc class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define STRENGTH_ATTRIB							CONSTLIT("strength")
#define TYPE_ATTRIB								CONSTLIT("type")

#define TYPE_VAULT								CONSTLIT("vault")

int COverlayUndergroundDesc::GetHitPoints (int iLevel) const

//	GetHitPoints
//
//	Returns the number of hit points at the given system level.

	{
	if (IsEmpty())
		return 0;

	else
		//	Strength of 100 = standard armor hit points at the given level.

		return Max(1, mathAdjustRound(CArmorClass::GetStdHP(iLevel), m_iStrength));
	}

ALERROR COverlayUndergroundDesc::InitFromXML (SDesignLoadCtx &Ctx, const CXMLElement &Desc)

//	InitFromXML
//
//	Initializes from a <Underground> element.

	{
	CString sType = Desc.GetAttribute(TYPE_ATTRIB);

	if (strEquals(sType, TYPE_VAULT))
		m_iType = typeVault;

	else
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Unknown underground type: %s"), sType);
		return ERR_FAIL;
		}

	m_iStrength = Desc.GetAttributeIntegerBounded(STRENGTH_ATTRIB, 0, -1, 0);

	return NOERROR;
	}
