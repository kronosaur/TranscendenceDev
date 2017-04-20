//	CSteamService.cpp
//
//	CSteamService class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

#define ENABLED_ATTRIB							CONSTLIT("enabled")

#define CMD_SERVICE_ACCOUNT_CHANGED				CONSTLIT("serviceAccountChanged")

#define STATUS_POSTING_GAME_RECORD				CONSTLIT("Posting score to Steam...")
#define STATUS_POST_SUCCESSFUL					CONSTLIT("Score posted to Steam.")

#define TAG_STEAM								CONSTLIT("Steam")

const AppId_t TRANSCENDENCE_APP_ID = 364510;

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
			"The Stars of the Pilgrim HD", extExtension, CMultiverseCatalogEntry::licenseSteam,

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
		::kernelDebugLogPattern("Unable to initialize Steam API.");
		m_bConnected = false;
		return;
		}

	//	Ensure that the user has logged into Steam. This will always return true if the game is launched
	//	from Steam, but if Steam is at the login prompt when you run your game from the debugger, it
	//	will return false.

	if (!SteamUser()->BLoggedOn())
		{
		::kernelDebugLogPattern("Steam user is not logged in.");
		m_bConnected = false;
		return;
		}

	//	We're enabled

	m_sUsername = CString(CString::csUTF8, SteamFriends()->GetPersonaName());
	m_bConnected = true;

#ifdef DEBUG
	::kernelDebugLogPattern("Username: %s", m_sUsername);
#endif
	}

CSteamService::~CSteamService (void)

//	CSteamService destructor

	{
	if (m_bConnected)
		SteamAPI_Shutdown();
	}

void CSteamService::AccumulateExtensionFolders (TArray<CString> &Folders)

//  AccumulateExtensionFolders
//
//  Append list of folders in which to find extension.

    {
	if (!m_bConnected)
		return;

    int i;
    int iCount = (int)SteamUGC()->GetNumSubscribedItems();
    TArray<PublishedFileId_t> FileIds;
    if (iCount)
        {
        FileIds.InsertEmpty(iCount);
        SteamUGC()->GetSubscribedItems(&FileIds[0], iCount);
        }

    Folders.GrowToFit(iCount);
    for (i = 0; i < FileIds.GetCount(); i++)
        {
	    uint32 unItemState = SteamUGC()->GetItemState(FileIds[i]);
        if (!(unItemState & k_EItemStateInstalled))
            continue;

	    uint32 unTimeStamp = 0;
	    uint64 unSizeOnDisk = 0;
	    char szItemFolder[1024] = { 0 };
	
        if (!SteamUGC()->GetItemInstallInfo(FileIds[i], &unSizeOnDisk, szItemFolder, sizeof(szItemFolder), &unTimeStamp))
            continue;

        Folders.Insert(CString(szItemFolder));
        }
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
		case canGetHighScores:
		case canGetUserProfile:
		case canLoadUserCollection:
		case canPostGameRecord:
		case modExchange:
			return m_bConnected;

		case cachedUser:
			return false;

		case canDownloadExtension:
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

    //  Add workshop items

    CString sError;
    CUGCGetUserContent::SQuery Query;
    Query.nAppId = TRANSCENDENCE_APP_ID;
    Query.eListType = k_EUserUGCList_Subscribed;
    Query.unAccountID = SteamUser()->GetSteamID().GetAccountID();

    TArray<CUGCGetUserContent::SDetails> Results;
    if (CUGCGetUserContent().Call(Query, Results, &sError))
        {
        for (i = 0; i < Results.GetCount(); i++)
            {
            CUGCGetUserContent::SDetails &Details = Results[i];

		    //	Add the entry to the catalog.

		    CMultiverseCatalogEntry *pEntry;
		    CMultiverseCatalogEntry::SEntryCreate Create;
            Create.dwUNID = Details.dwUNID;
            Create.sName = CString(Details.Basic.m_rgchTitle);
            Create.iType = Details.iType;
            Create.iLicense = CMultiverseCatalogEntry::licenseSteamUGC;
            Create.sDesc = CString(Details.Basic.m_rgchDescription);

		    CMultiverseCatalogEntry::CreateBasicEntry(Create, &pEntry);
		    Collection.Insert(pEntry);
            }
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
	//	Don't bother is we got a score of 0 or if the game is in debug mode

	if (Record.GetScore() == 0 || Record.IsDebug() || !m_bConnected)
		return NOERROR;

	SendServiceStatus(STATUS_POSTING_GAME_RECORD);

	//	Generate a well-known name for the leaderboard

	CString sName = strPatternSubst(CONSTLIT("unid-%08x"), Record.GetAdventureUNID());

	//	Find or create the leaderboard

	SteamLeaderboard_t hLeaderboard;
	if (!CFindOrCreateLeaderboard().Call(sName, k_ELeaderboardSortMethodDescending, k_ELeaderboardDisplayTypeNumeric, &hLeaderboard))
		{
		if (retsResult) *retsResult = strPatternSubst(CONSTLIT("Unable to find leaderboard for adventure: %08x"), Record.GetAdventureUNID());
		return ERR_FAIL;
		}

	//	Upload the score

	if (!CUploadLeaderboardScore().Call(hLeaderboard, Record.GetScore()))
		{
		if (retsResult) *retsResult = strPatternSubst(CONSTLIT("Unable to upload score for adventure: %08x"), Record.GetAdventureUNID());
		return ERR_FAIL;
		}

	//	Done

	SendServiceStatus(STATUS_POST_SUCCESSFUL);
	return NOERROR;
	}

ALERROR CSteamService::ReadHighScoreList (ITaskProcessor *pProcessor, DWORD dwAdventure, CAdventureHighScoreList *retHighScores, CString *retsResult)

//	ReadHighScoreList
//
//	Returns the high score list for the given adventure. We return up to 100
//	entries.

	{
	if (!m_bConnected)
		return NOERROR;

	//	Generate a well-known name for the leaderboard

	CString sName = strPatternSubst(CONSTLIT("unid-%08x"), dwAdventure);

	//	Find or create the leaderboard

	SteamLeaderboard_t hLeaderboard;
	if (!CFindOrCreateLeaderboard().Call(sName, k_ELeaderboardSortMethodDescending, k_ELeaderboardDisplayTypeNumeric, &hLeaderboard))
		{
		if (retsResult) *retsResult = strPatternSubst(CONSTLIT("Unable to find leaderboard for adventure: %08x"), dwAdventure);
		return ERR_FAIL;
		}

	//	Create a high score list

	*retHighScores = CAdventureHighScoreList(dwAdventure);

	//	Download scores into it

	if (!CDownloadLeaderboardScores().Call(hLeaderboard, retHighScores))
		{
		if (retsResult) *retsResult = strPatternSubst(CONSTLIT("Unable to download high scores for adventure: %08x"), dwAdventure);
		return ERR_FAIL;
		}

	//	Done

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
