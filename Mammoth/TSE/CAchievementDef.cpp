//	CAchievementDef.cpp
//
//	CAchievementDef class
//	Copyright (c) 2021 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define IMAGE_TAG								CONSTLIT("Image")

#define DISABLED_ATTRIB							CONSTLIT("disabled")
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

	//	In debug mode, check to make sure we have the appropriate language 
	//	elements.

	if (Ctx.GetUniverse().InDebugMode()
			&& IsEnabled())
		{
		if (!m_Type.TranslateText(strPatternSubst(LANGID_DESC, GetID()), NULL, NULL))
			Ctx.GetUniverse().LogOutput(strPatternSubst("WARNING: Expected %s.desc language ID in type %08x.", GetID(), m_Type.GetUNID()));

		if (!m_Type.TranslateText(strPatternSubst(LANGID_NAME, GetID()), NULL, NULL))
			Ctx.GetUniverse().LogOutput(strPatternSubst("WARNING: Expected %s.name language ID in type %08x.", GetID(), m_Type.GetUNID()));
		}

	return NOERROR;
	}

bool CAchievementDef::CanPost () const

//	CanPost
//
//	Returns TRUE if we can post to the Multiverse.

	{
	const CExtension *pExtension = m_Type.GetExtension();
	if (!pExtension)
		return false;

	if (!pExtension->IsRegistered())
		return false;

	return true;
	}

bool CAchievementDef::CanPostToSteam () const

//	CanPostToSteam
//
//	Returns TRUE if we can post to Steam.

	{
	if (m_sSteamID.IsBlank())
		return false;

	const CExtension *pExtension = m_Type.GetExtension();
	if (!pExtension)
		return false;

#ifndef DEBUG
	if (!pExtension->IsOfficial())
		return false;
#endif

	return true;
	}

CString CAchievementDef::GetName () const

//	GetName
//
//	Translates the achievement name.

	{
	CString sText;
	if (!m_Type.TranslateText(strPatternSubst(LANGID_NAME, GetID()), NULL, &sText))
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

	m_sSortOrder = Entry.GetAttribute(SORT_ORDER_ATTRIB);

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

	//	Options

	m_bDisabled = Entry.GetAttributeBool(DISABLED_ATTRIB);

	return NOERROR;
	}
