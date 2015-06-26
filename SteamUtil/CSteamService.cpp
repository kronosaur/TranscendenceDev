//	CSteamService.cpp
//
//	CSteamService class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

#define ENABLED_ATTRIB							CONSTLIT("enabled")

#define CMD_SERVICE_ACCOUNT_CHANGED				CONSTLIT("serviceAccountChanged")

#define TAG_STEAM								CONSTLIT("Steam")

struct SSteamEntryCreate
	{
	DWORD dwUNID;
	int iAppID;								//	AppID to check for ownership
	char *pszName;
	EExtensionTypes iType;
	CMultiverseCatalogEntry::ELicenseTypes iLicense;
	char *pszDesc;
	};

//	This is a list of all extensions available on the Steam platform. Not all 
//	of these extensions will be available to the current player. We check for
//	ownership of the appID before enabling this extension.

SSteamEntryCreate AVAILABLE_EXTENSIONS[] =
	{	
		{	0x00200000,		364510,
			"The Stars of the Pilgrim", extAdventure, CMultiverseCatalogEntry::licenseCore,

			"Called by Domina to journey to the Galactic Core, you leave the safe environs of your home system and explore the farthest reaches of Human Space.\n\n"
			"But dangers stalk your passage and even Domina's protection is no guarantee, for none who have been called have yet returned.\n\n"
			"Will your pilgrimage be more successful?"
			},

		{	0x00800000,		364510,
			"Corporate Command", extExtension, CMultiverseCatalogEntry::licenseSteam,

			"Journey to the Galactic Core in your own Manticore-class heavy gunship and run missions for CHOC, the Corporate Hierarchy Operations Command. "
			"As you explore the realm of the Corporate Hierarchy, you'll confront the mystery of the chimeras—a foe that could threaten all of Human Space."
			},

		{	0x00810000,		364510,
			"Corporate Hierarchy Volume 1", extLibrary, CMultiverseCatalogEntry::licenseSteam,

			""
			},

		{	0x00820000,		364510,
			"The Stars of the Pilgrim Soundtrack", extExtension, CMultiverseCatalogEntry::licenseSteam,

			"The Stars of the Pilgrim Soundtrack adds more than 40 minutes of orchestral music to the game."
			},

		{	0x00830000,		364510,
			"Human Space HD", extExtension, CMultiverseCatalogEntry::licenseSteam,

			"This extension adds full rotation frames for all large ships encountered in Human Space."
			},

		{	0x00900000,		383320,
			"Eternity Port", extAdventure, CMultiverseCatalogEntry::licenseSteam,

			"With a fast ship and the skills to fly it, you earn a living running dangerous missions for any sovereign who will hire you. "
			"But when a digital hyperintelligence threatens the entirety of Human Space, the only hope lies with the reclusive cyber-citizens of Eternity Port."
			},

		{	0x00910000,		383320,
			"Near Stars Volume 1", extLibrary, CMultiverseCatalogEntry::licenseSteam,

			""
			},
	};

const int AVAILABLE_EXTENSIONS_COUNT = (sizeof(AVAILABLE_EXTENSIONS) / sizeof(AVAILABLE_EXTENSIONS[0]));

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
		case canLoadNews:
		case canPostCrashReport:
			return false;

		case loginUser:
		case autoLoginUser:
		case canGetUserProfile:
		case canLoadUserCollection:
		case modExchange:
			return m_bConnected;

		case cachedUser:
			return false;

		case canDownloadExtension:
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

ALERROR CSteamService::LoadUserCollection (ITaskProcessor *pProcessor, CMultiverseModel &Multiverse, CString *retsResult)

//	LoadUserCollection
//
//	Loads the user's collection from the cloud.

	{
	int i;

	//	Steam client must be running

	ISteamApps *pSteamApps = SteamApps();
	if (pSteamApps == NULL)
		{
		*retsResult = CONSTLIT("Steam client is not running.");
		return ERR_FAIL;
		}

	//	Set state on Multiverse object so that everyone else knows what's
	//	going on.

	Multiverse.OnCollectionLoading();

	//	Generate our list of extensions

	TArray<CMultiverseCatalogEntry *> Collection;
	for (i = 0; i < AVAILABLE_EXTENSIONS_COUNT; i++)
		{
		//	See if the current player has this extension. If not, then we skip.

		if (!pSteamApps->BIsDlcInstalled(AVAILABLE_EXTENSIONS[i].iAppID))
			continue;

		//	Add the entry to the catalog.

		CMultiverseCatalogEntry *pEntry;
		CMultiverseCatalogEntry::SEntryCreate Create;
		Create.dwUNID = AVAILABLE_EXTENSIONS[i].dwUNID;
		Create.sName = CString(AVAILABLE_EXTENSIONS[i].pszName, -1, true);
		Create.iType = AVAILABLE_EXTENSIONS[i].iType;
		Create.iLicense = AVAILABLE_EXTENSIONS[i].iLicense;
		Create.sDesc = CString(AVAILABLE_EXTENSIONS[i].pszDesc, -1, true);

		CMultiverseCatalogEntry::CreateBasicEntry(Create, &pEntry);
		Collection.Insert(pEntry);
		}

	//	Set the collection

	Multiverse.SetCollection(Collection);
	SendServiceStatus(NULL_STR);

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
