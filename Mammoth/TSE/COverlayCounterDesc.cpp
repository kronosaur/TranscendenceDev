//	COverlayCounterDesc.cpp
//
//	COverlayCounterDesc class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define COLOR_ATTRIB							CONSTLIT("color")
#define LABEL_ATTRIB							CONSTLIT("label")
#define MAX_ATTRIB								CONSTLIT("max")
#define SHOW_ON_MAP_ATTRIB						CONSTLIT("showOnMap")
#define STYLE_ATTRIB							CONSTLIT("style")

#define COUNTER_COMMAND_BAR_PROGRESS			CONSTLIT("commandBarProgress")
#define COUNTER_FLAG							CONSTLIT("flag")
#define COUNTER_PROGRESS						CONSTLIT("progress")
#define COUNTER_RADIUS							CONSTLIT("radius")
#define COUNTER_TEXT_FLAG						CONSTLIT("textFlag")

ALERROR COverlayCounterDesc::InitFromXML (SDesignLoadCtx &Ctx, const CXMLElement &Desc)

//	InitFromXML
//
//	Initializes from a <Counter> element.

	{
	CString sStyle = Desc.GetAttribute(STYLE_ATTRIB);

	if (strEquals(sStyle, COUNTER_COMMAND_BAR_PROGRESS))
		m_iType = counterCommandBarProgress;

	else if (strEquals(sStyle, COUNTER_FLAG))
		m_iType = counterFlag;

	else if (strEquals(sStyle, COUNTER_PROGRESS))
		m_iType = counterProgress;

	else if (strEquals(sStyle, COUNTER_RADIUS))
		m_iType = counterRadius;

	else if (strEquals(sStyle, COUNTER_TEXT_FLAG))
		m_iType = counterTextFlag;

	else
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Unknown counter style: %s"), sStyle);
		return ERR_FAIL;
		}

	m_sLabel = Desc.GetAttribute(LABEL_ATTRIB);
	m_iMaxValue = Desc.GetAttributeIntegerBounded(MAX_ATTRIB, 0, -1, 100);
	m_rgbPrimary = ::LoadRGBColor(Desc.GetAttribute(COLOR_ATTRIB));
	m_bShowOnMap = Desc.GetAttributeBool(SHOW_ON_MAP_ATTRIB);

	return NOERROR;
	}
