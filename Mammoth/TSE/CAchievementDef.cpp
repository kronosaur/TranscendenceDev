//	CAchievementDef.cpp
//
//	CAchievementDef class
//	Copyright (c) 2021 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define IMAGE_TAG								CONSTLIT("Image")

#define ID_ATTRIB								CONSTLIT("id")
#define SORT_ORDER_ATTRIB						CONSTLIT("sortOrder")

ALERROR CAchievementDef::BindDesign (SDesignLoadCtx &Ctx)

//	BindDesign
//
//	Binds the design elements.

	{
	if (ALERROR error = m_Image.OnDesignLoadComplete(Ctx))
		return error;

	return NOERROR;
	}

ALERROR CAchievementDef::InitFromXML (SDesignLoadCtx &Ctx, const CXMLElement &Entry)

//	InitFromXML
//
//	Initialize from XML.

	{
	m_sID = Entry.GetAttribute(ID_ATTRIB);
	if (m_sID.IsBlank())
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Achievement must have an ID."));
		return ERR_FAIL;
		}

	m_iSortOrder = Entry.GetAttributeInteger(SORT_ORDER_ATTRIB);

	if (const CXMLElement *pImageXML = Entry.GetContentElementByTag(IMAGE_TAG))
		{
		if (ALERROR error = m_Image.InitFromXML(*pImageXML))
			return error;
		}

	return NOERROR;
	}
