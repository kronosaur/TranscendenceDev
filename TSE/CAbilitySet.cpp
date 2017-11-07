//	CAbilitySet.cpp
//
//	CAbilitySet class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define EQUIPMENT_ATTRIB						CONSTLIT("equipment")

#define ERR_UNKNOWN_EQUIPMENT					CONSTLIT("unknown equipment: %s")

ALERROR CAbilitySet::InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)


//	InitFromXML
//
//	Reads an equipment set that looks like this:
//
//	<Equipment>
//		<Element equipment="..."/>
//      ...
//	</Equipment>

	{
	int i;

	ClearAll();

	for (i = 0; i < pDesc->GetContentElementCount(); i++)
		{
		CXMLElement *pItem = pDesc->GetContentElement(i);
		Abilities iEquipment = AbilityDecode(pItem->GetAttribute(EQUIPMENT_ATTRIB));
		if (iEquipment == ::ablUnknown)
			{
			Ctx.sError = strPatternSubst(ERR_UNKNOWN_EQUIPMENT, pItem->GetAttribute(EQUIPMENT_ATTRIB));
			return ERR_FAIL;
			}

		Set(iEquipment);
		}

	return NOERROR;
	}

void CAbilitySet::Set (const CAbilitySet &Abilities)

//	Set
//
//	Sets all the given abilities.

	{
	if (Abilities.IsEmpty())
		return;

	m_dwSet |= Abilities.m_dwSet;
	}
