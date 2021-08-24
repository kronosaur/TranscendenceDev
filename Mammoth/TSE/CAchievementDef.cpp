//	CAchievementDef.cpp
//
//	CAchievementDef class
//	Copyright (c) 2021 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define IMAGE_TAG								CONSTLIT("Image")

#define ID_ATTRIB								CONSTLIT("id")
#define SORT_ORDER_ATTRIB						CONSTLIT("sortOrder")
#define STEAM_ID_ATTRIB							CONSTLIT("steamID")

#define STR_UNTITLED							CONSTLIT("Untitled: %s")

#define LANGID_DESC								CONSTLIT("%s.desc")
#define LANGID_NAME								CONSTLIT("%s.name")

ALERROR CAchievementDef::BindDesign (SDesignLoadCtx &Ctx)

//	BindDesign
//
//	Binds the design elements.

	{
	if (ALERROR error = m_Image.OnDesignLoadComplete(Ctx))
		return error;

	return NOERROR;
	}

CString CAchievementDef::GetName (const ICCItem *pData) const

//	GetName
//
//	Translates the achievement name.

	{
	CString sText;
	if (!m_Type.TranslateText(strPatternSubst(LANGID_NAME, GetID()), pData, &sText))
		return strPatternSubst(STR_UNTITLED, GetID());

	return sText;
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

	//	Parse Steam ID

	CString sSteamID;
	if (Entry.FindAttribute(STEAM_ID_ATTRIB, &sSteamID))
		{
		const char *pPos = sSteamID.GetASCIIZPointer();
		const char *pStart = pPos;
		while (*pPos != '\0' && *pPos != '=')
			pPos++;

		m_sSteamID = CString(pStart, pPos - pStart);

		if (*pPos == '=')
			{
			pPos++;
			m_iSteamID = strParseInt(pPos, 0);
			}
		}

	return NOERROR;
	}

