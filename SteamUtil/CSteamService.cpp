//	CSteamService.cpp
//
//	CSteamService class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

#define TAG_STEAM								CONSTLIT("Steam")

CSteamService::CSteamService (CHumanInterface &HI) : ICIService(m_HI)

//	CSteamService constructor

	{
	//	Boot up steam

	if (!SteamAPI_Init())
		{
		::kernelDebugLogMessage("Unable to initialize Steam API.");
		m_bEnabled = false;
		return;
		}

	m_sUsername = CString(CString::csUTF8, SteamFriends()->GetPersonaName());

	//	We're enabled

	m_bEnabled = true;
	}

CSteamService::~CSteamService (void)

//	CSteamService destructor

	{
	if (m_bEnabled)
		SteamAPI_Shutdown();
	}

CString CSteamService::GetTag (void)

//	GetTag
//
//	Returns the XML tag to use to store setting

	{
	return TAG_STEAM;
	}

bool CSteamService::HasCapability (DWORD dwCapability)

//	HasCapability
//
//	Returns TRUE if we have the given capability.

	{
	return false;
	}

ALERROR CSteamService::InitFromXML (CXMLElement *pDesc, bool *retbModified)

//	InitFromXML
//
//	Initializes

	{
	*retbModified = false;
	return NOERROR;
	}

ALERROR CSteamService::PostGameRecord (ITaskProcessor *pProcessor, const CGameRecord &Record, const CGameStats &Stats, CString *retsResult)

//	PostGameRecord
//
//	Posts a game record

	{
	return NOERROR;
	}

ALERROR CSteamService::WriteAsXML (IWriteStream *pOutput)

//	WriteAsXML
//
//	Writes to settings

	{
	return NOERROR;
	}
