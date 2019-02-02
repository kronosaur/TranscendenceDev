//	CShipStandard.cpp
//
//	CShipStandard class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CShipStandard::SFieldDesc CShipStandard::m_FieldDesc[fieldCount] =
	{
		//	fieldID					Min			Max			Default
		{	"drivePowerUse",		0.0,		-1.0,		20.0	},
		{	"pricePerHullPoint",	0.0,		-1.0,		110.0	},
	};

CShipStandard CShipStandard::m_Default = CShipStandard(TInitDefaults());

CShipStandard::CShipStandard (const TInitDefaults &Src)

//	CShipStandard constructor

	{
	for (int i = 0; i < fieldCount; i++)
		m_rValue[i] = m_FieldDesc[i].rDefault;
	}

ALERROR CShipStandard::InitFromXML (SDesignLoadCtx &Ctx, CXMLElement &Desc)

//	InitFromXML
//
//	Initializes from a <ShipStandard> XML element.

	{
	for (int i = 0; i < fieldCount; i++)
		{
		const SFieldDesc &Field = m_FieldDesc[i];
		m_rValue[i] = Desc.GetAttributeDoubleBounded(Field.pszFieldID, Field.rMin, Field.rMax, Field.rDefault);
		}

	return NOERROR;
	}
