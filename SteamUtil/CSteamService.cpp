//	CSteamService.cpp
//
//	CSteamService class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

#define ENABLED_ATTRIB							CONSTLIT("enabled")

#define CMD_SERVICE_ACCOUNT_CHANGED				CONSTLIT("serviceAccountChanged")

#define TAG_STEAM								CONSTLIT("Steam")

CSteamService::CSteamService (CHumanInterface &HI) : ICIService(HI)

//	CSteamService constructor

	{
	//	Boot up steam

	if (!SteamAPI_Init())
		{
		::kernelDebugLogMessage("Unable to initialize Steam API.");
		m_bConnected = false;
		return;
		}

	//	We're enabled

	m_sUsername = CString(CString::csUTF8, SteamFriends()->GetPersonaName());
	m_bConnected = true;
	}

CSteamService::~CSteamService (void)

//	CSteamService destructor

	{
	if (m_bConnected)
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
	switch (dwCapability)
		{
		case registerUser:
		case userProfile:
		case modExchange:
		case canLoadNews:
		case canPostCrashReport:
			return false;

		case autoLoginUser:
		case canGetUserProfile:
		case loginUser:
			return m_bConnected;

		case cachedUser:
			return false;

		case canDownloadExtension:
		case canLoadUserCollection:
		case canPostGameRecord:
			return false;

		default:
			return false;
		}
	}

ALERROR CSteamService::InitFromXML (CXMLElement *pDesc, bool *retbModified)

//	InitFromXML
//
//	Initializes

	{
	//	Load parameters

	if (pDesc)
		{
		*retbModified = false;
		SetEnabled(pDesc->GetAttributeBool(ENABLED_ATTRIB));
		}
	else
		{
		SetEnabled(true);
		*retbModified = true;
		}

	return NOERROR;
	}

ALERROR CSteamService::PostGameRecord (ITaskProcessor *pProcessor, const CGameRecord &Record, const CGameStats &Stats, CString *retsResult)

//	PostGameRecord
//
//	Posts a game record

	{
	return NOERROR;
	}

ALERROR CSteamService::SignInUser (ITaskProcessor *pProcessor, const CString &sUsername, const CString &sPassword, bool bAutoSignIn, CString *retsResult)

//	SignInUser
//
//	Make sure the user is signed in.

	{
	m_HI.HIPostCommand(CMD_SERVICE_ACCOUNT_CHANGED);
	return NOERROR;
	}

ALERROR CSteamService::WriteAsXML (IWriteStream *pOutput)

//	WriteAsXML
//
//	Writes to settings

	{
	return NOERROR;
	}
