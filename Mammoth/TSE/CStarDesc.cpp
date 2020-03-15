//	CStarDesc.cpp
//
//	CStarDesc class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define GRAVITY_RADIUS_ATTRIB					CONSTLIT("gravityRadius")
#define MAX_LIGHT_DISTANCE						CONSTLIT("maxLightRadius")
#define SPACE_COLOR_ATTRIB						CONSTLIT("spaceColor")

ALERROR CStarDesc::InitFromStationTypeXML (SDesignLoadCtx &Ctx, const CXMLElement &Desc)

//	InitFromStationTypeXML
//
//	Initializes from XML.

	{
	m_rgbSpaceColor = ::LoadRGBColor(Desc.GetAttribute(SPACE_COLOR_ATTRIB));
	m_iMaxLightDistance = Desc.GetAttributeIntegerBounded(MAX_LIGHT_DISTANCE, 1, -1, 500);

	int iGravity;
	if (Desc.FindAttributeInteger(GRAVITY_RADIUS_ATTRIB, &iGravity))
		m_rGravityRadius = iGravity * LIGHT_SECOND;
	else
		m_rGravityRadius = 0.0;

	return NOERROR;
	}
