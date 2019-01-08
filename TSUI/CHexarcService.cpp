//	CHexarcService.cpp
//
//	CHexarcService class
//	Copyright (c) 2011 by George Moromisato. All Rights Reserved.
//
//	INTERFACE
//
//	1.	After initializing everything, check for autoLoginUser capability. If
//		available, call AutoSignInUser

#include "stdafx.h"

#define HEXARC_TAG								CONSTLIT("Hexarc")

#define ENABLED_ATTRIB							CONSTLIT("enabled")
#define HOST_ATTRIB								CONSTLIT("host")
#define ROOT_URL_ATTRIB							CONSTLIT("rootURL")
#define USERNAME_ATTRIB							CONSTLIT("username")

#define DEFAULT_HOST							CONSTLIT("multiverse.kronosaur.com")
#define DEFAULT_URL								CONSTLIT("/api/")
#define OLD_HOST								CONSTLIT("transcendence.hierosteel.com")

#define AUTH_TYPE_SHA1							CONSTLIT("SHA1")

#define GAME_TYPE								CONSTLIT("Trans")

#define CMD_SERVICE_ACCOUNT_CHANGED				CONSTLIT("serviceAccountChanged")
#define CMD_SERVICE_DOWNLOADS_COMPLETE			CONSTLIT("serviceDownloadsComplete")
#define CMD_SERVICE_DOWNLOADS_IN_PROGRESS		CONSTLIT("serviceDownloadsInProgress")
#define CMD_SERVICE_FILE_DOWNLOADED				CONSTLIT("serviceFileDownloaded")

#define ERR_CANNOT_DIGEST_PASSWORD				CONSTLIT("Error creating a password hash.")
#define ERR_CANT_DOWNLOAD						CONSTLIT("Error downloading file: %s")
#define ERR_SERVER_COMMAND_ERROR				CONSTLIT("Error from Multiverse: %s")
#define ERR_INTERNAL_ERROR						CONSTLIT("Internal programming error.")
#define ERR_INVALID_RESPONSE					CONSTLIT("Invalid response from server at %s.")
#define ERR_NOT_SIGNED_IN						CONSTLIT("Not signed in.")
#define ERR_UNABLE_TO_SIGN						CONSTLIT("Unable to sign communications: probably out of memory.")
#define ERR_SIGN_IN_FAILED						CONSTLIT("Unable to sign in to Multiverse.")
#define ERR_SERVER_RESPONSE_ERROR				CONSTLIT("Unable to understand Multiverse response.")

#define STATUS_LOAD_COLLECTION					CONSTLIT("Downloading user collection...")
#define STATUS_LOAD_NEWS						CONSTLIT("Downloading news...")
#define STATUS_POSTING_GAME_RECORD				CONSTLIT("Posting game record to Multiverse...")
#define STATUS_SIGNING_IN						CONSTLIT("Signing in...")
#define STATUS_POST_SUCCESSFUL					CONSTLIT("Game record posted to Multiverse.")
#define STATUS_DOWNLOADING_FILE					CONSTLIT("Downloading %s...%d%%")
#define STATUS_FILE_DOWNLOADED					CONSTLIT("Downloaded %s.")

#define STR_POSTING_GAME_RECORD					CONSTLIT("Posting game record to Multiverse...")
#define STR_LOADING_COLLECTION					CONSTLIT("Downloading user collection from Multiverse...")

#define FIELD_ADVENTURE							CONSTLIT("adventure")
#define FIELD_ADVENTURES						CONSTLIT("adventures")
#define FIELD_ACTUAL							CONSTLIT("actual")
#define FIELD_ACTUAL_REQUIRED					CONSTLIT("actualRequired")
#define FIELD_API_VERSION						CONSTLIT("apiVersion")
#define FIELD_AUTH_TOKEN						CONSTLIT("authToken")
#define FIELD_CHALLENGE							CONSTLIT("challenge")
#define FIELD_CHALLENGE_CREDENTIALS				CONSTLIT("challengeCredentials")
#define FIELD_CLIENT_ID							CONSTLIT("clientID")
#define FIELD_CLIENT_VERSION					CONSTLIT("clientVersion")
#define FIELD_CREDENTIALS						CONSTLIT("credentials")
#define FIELD_DEBUG_LOG							CONSTLIT("debugLog")
#define FIELD_EMAIL								CONSTLIT("email")
#define FIELD_GAME								CONSTLIT("game")
#define FIELD_GAMES								CONSTLIT("games")
#define FIELD_GAME_ID							CONSTLIT("gameID")
#define FIELD_GAME_RECORD						CONSTLIT("gameRecord")
#define FIELD_GAME_STATS						CONSTLIT("gameStats")
#define FIELD_INCLUDE_LEADERBOARD				CONSTLIT("includeLeaderboard")
#define FIELD_LEADERBOARD						CONSTLIT("leaderboard")
#define FIELD_LEAGUE							CONSTLIT("league")
#define FIELD_MAX_GAMES							CONSTLIT("maxGames")
#define FIELD_MOST_RECENT						CONSTLIT("mostRecent")
#define FIELD_NEW_CREDENTIALS					CONSTLIT("newCredentials")
#define FIELD_OLD_CREDENTIALS					CONSTLIT("oldCredentials")
#define FIELD_ONLY_IN_COLLECTION				CONSTLIT("onlyInCollection")
#define FIELD_PERSONAL_BEST						CONSTLIT("personalBest")
#define FIELD_PROTOCOL_VERSION					CONSTLIT("protocolVersion")
#define FIELD_SCOPED_AUTH_TOKEN					CONSTLIT("scopedAuthToken")
#define FIELD_SCOPED_CREDENTIALS				CONSTLIT("scopedCredentials")
#define FIELD_SIGNATURE							CONSTLIT("signature")
#define FIELD_STATS								CONSTLIT("stats")
#define FIELD_TRANS								CONSTLIT("trans")
#define FIELD_TRANSCENDENCE_VERSION				CONSTLIT("transcendenceVersion")
#define FIELD_TYPE								CONSTLIT("type")
#define FIELD_UNID								CONSTLIT("unid")
#define FIELD_USERNAME							CONSTLIT("username")

#define FUNC_CHANGE_PASSWORD					CONSTLIT("changePassword")
#define FUNC_CONNECT_CLIENT						CONSTLIT("connectClient")
#define FUNC_DOWNLOAD_CATALOG_ENTRY_FILE		CONSTLIT("downloadCatalogEntryFile")
#define FUNC_GET_PROFILE_RECORDS				CONSTLIT("getProfileRecords")
#define FUNC_LIST_ARTICLES						CONSTLIT("listArticles")
#define FUNC_LIST_CATALOG						CONSTLIT("listCatalog")
#define FUNC_HIGH_SCORE_GAMES					CONSTLIT("listHighScores")
#define FUNC_RECENT_GAMES						CONSTLIT("listRecentGames")
#define FUNC_LOGIN								CONSTLIT("login")
#define FUNC_POST_CRASH_LOG						CONSTLIT("postCrashLog")
#define FUNC_POST_GAME							CONSTLIT("postGame")
#define FUNC_REGISTER							CONSTLIT("register")
#define FUNC_REQUEST_LOGIN						CONSTLIT("requestLogin")

#define LEAGUE_REGISTERED						CONSTLIT("registered")

#define METHOD_OPTIONS							CONSTLIT("OPTIONS")
#define METHOD_POST								CONSTLIT("POST")

#define PROTOCOL_HTTP							CONSTLIT("http")

#define REGISTRY_COMPANY_NAME					CONSTLIT("Kronosaur Productions")
#define REGISTRY_PRODUCT_NAME					CONSTLIT("Transcendence")
#define REGISTRY_CLIENT_ID_OPTION				CONSTLIT("ClientID")
#define REGISTRY_SAVED_KEY_OPTION				CONSTLIT("SavedKey")

#define TYPE_GAME_ENGINE						CONSTLIT("gameEngine")
#define TYPE_TRANSCENDENCE_ADVENTURE			CONSTLIT("transcendenceAdventure")
#define TYPE_TRANSCENDENCE_EXPANSION			CONSTLIT("transcendenceExpansion")
#define TYPE_TRANSCENDENCE_LIBRARY				CONSTLIT("transcendenceLibrary")
#define TYPE_TRANSCENDENCE_NEWS					CONSTLIT("transcendenceNews")

#ifdef HEXARC_SANDBOX

const int KEY_SIZE = 64;

static BYTE g_TransClient01[] =
	{
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	};

#else
#include "HexarcKeys.h"
#endif

class CHexarcService : public ICIService
	{
	public:
		CHexarcService (CHumanInterface &HI) : ICIService(HI),
				m_bCrashReported(false)
			{ }

		virtual ALERROR ChangePassword (ITaskProcessor *pProcessor, const CString &sUsername, const CString &sOldPassword, const CString &sNewPassword, CString *retsResult = NULL) override;
		virtual CString GetTag (void) override;
		virtual CString GetUsername (void) override { return m_sUsername; }
		virtual bool HasCapability (DWORD dwCapability) override;
		virtual ALERROR Housekeeping (ITaskProcessor *pProcessor) override;
		virtual ALERROR InitFromXML (CXMLElement *pDesc, bool *retbModified) override;
		virtual ALERROR InitPrivateData (void) override;
		virtual ALERROR LoadNews (ITaskProcessor *pProcessor, CMultiverseModel &Multiverse, const SFileVersionInfo &AppVersion, const CString &sCacheFilespec, CString *retsResult = NULL) override;
		virtual ALERROR LoadUserCollection (ITaskProcessor *pProcessor, CExtensionCollection &Extensions, CMultiverseModel &Multiverse, CString *retsResult = NULL) override;
		virtual ALERROR PostCrashReport (ITaskProcessor *pProcessor, const CString &sCrash, CString *retsResult = NULL) override;
		virtual ALERROR PostGameRecord (ITaskProcessor *pProcessor, const CGameRecord &Record, const CGameStats &Stats, CString *retsResult = NULL) override;
		virtual ALERROR ProcessDownloads (ITaskProcessor *pProcessor, CString *retsResult = NULL) override;
		virtual ALERROR ReadHighScoreList (ITaskProcessor *pProcessor, DWORD dwAdventure, CAdventureHighScoreList *retHighScores, CString *retsResult = NULL) override;
		virtual ALERROR ReadProfile (ITaskProcessor *pProcessor, CUserProfile *retProfile, CString *retsResult = NULL) override;
		virtual ALERROR RegisterUser (ITaskProcessor *pProcessor, const CString &sUsername, const CString &sPassword, const CString &sEmail, bool bAutoSignIn, CString *retsResult = NULL) override;
		virtual ALERROR RequestExtensionDownload (const CString &sFilePath, const CString &sFilespec, const CIntegerIP &FileDigest) override;
		virtual ALERROR SignInUser (ITaskProcessor *pProcessor, const CString &sUsername, const CString &sPassword, bool bAutoSignIn, CString *retsResult = NULL) override;
		virtual ALERROR SignOutUser (ITaskProcessor *pProcessor, CString *retsError = NULL) override;
		virtual ALERROR WriteAsXML (IWriteStream *pOutput) override;
		virtual ALERROR WritePrivateData (void) override;

	private:
		bool IsLoggedIn (void) { return !m_UserToken.IsNull(); }
		bool HasCachedUsername (void) { return !m_sUsername.IsBlank(); }
		bool HasCachedCredentials (void) { return !m_Credentials.IsNull(); }
		bool Sign (const CJSONValue &Value, CIntegerIP *retSignature);

		CString m_sClientID;						//	Unique clientID
		CString m_sUsername;						//	Username
		CJSONValue m_Credentials;					//	Saved service password (not the user password)
		CJSONValue m_UserToken;						//	Obtained after register/login

//		bool m_bActualRequired;						//	Require actual username password
		CJSONValue m_Challenge;						//	Challenge

		CHexarcSession m_Session;					//	Connection
		CHexarcDownloader m_Downloader;				//	Keep track of files to download

		bool m_bCrashReported;						//	TRUE if we've already reported a crash
	};

ALERROR CHexarcService::ChangePassword (ITaskProcessor *pProcessor, const CString &sUsername, const CString &sOldPassword, const CString &sNewPassword, CString *retsResult)

//	ChangePassword
//
//	Change the user's password

	{
	//	Generate a hash of the username and password

	CJSONValue OldPasswordHash;
	if (!CHexarc::CreateCredentials(sUsername, sOldPassword, &OldPasswordHash))
		{
		*retsResult = ERR_CANNOT_DIGEST_PASSWORD;
		return ERR_FAIL;
		}

	CJSONValue NewPasswordHash;
	if (!CHexarc::CreateCredentials(sUsername, sNewPassword, &NewPasswordHash))
		{
		*retsResult = ERR_CANNOT_DIGEST_PASSWORD;
		return ERR_FAIL;
		}

	//	Put together a payload for posting

	CJSONValue Result;
	CJSONValue Payload(CJSONValue::typeObject);
	Payload.InsertHandoff(FIELD_API_VERSION, CJSONValue((int)API_VERSION));
	Payload.InsertHandoff(FIELD_USERNAME, CJSONValue(sUsername));
	Payload.InsertHandoff(FIELD_OLD_CREDENTIALS, OldPasswordHash);
	Payload.InsertHandoff(FIELD_NEW_CREDENTIALS, NewPasswordHash);
	if (m_Session.ServerCommand(METHOD_POST, FUNC_CHANGE_PASSWORD, Payload, &Result) != NOERROR)
		{
		*retsResult = Result.AsString();
		return ERR_FAIL;
		}

	//	We get back an auth token

	m_sUsername = sUsername;
	m_Credentials = Result.GetElement(FIELD_CREDENTIALS);
	m_UserToken = Result.GetElement(FIELD_AUTH_TOKEN);
	SetModified();

	m_HI.HIPostCommand(CMD_SERVICE_ACCOUNT_CHANGED);

	return NOERROR;
	}

CString CHexarcService::GetTag (void)

//	GetTag
//
//	Returns the element tag used to save settings

	{
	return HEXARC_TAG;
	}

bool CHexarcService::HasCapability (DWORD dwCapability)

//	HasCapability
//
//	Returns TRUE if the service has the given capability

	{
	switch (dwCapability)
		{
		case loginUser:
		case registerUser:
		case userProfile:
		case modExchange:
		case canLoadNews:
		case canPostCrashReport:
		case canGetHighScores:
			return true;

		case autoLoginUser:
			return HasCachedCredentials();

		case cachedUser:
			return HasCachedUsername();

		case canDownloadExtension:
		case canGetUserProfile:
		case canLoadUserCollection:
		case canPostGameRecord:
			return IsLoggedIn();

		default:
			return false;
		}
	}

ALERROR CHexarcService::Housekeeping (ITaskProcessor *pProcessor)

//	Housekeeping
//
//	Do periodic tasks. This function is called roughly once per minute.

	{
	m_Session.Housekeeping();

	//	Done

	return NOERROR;
	}

ALERROR CHexarcService::InitFromXML (CXMLElement *pDesc, bool *retbModified)

//	InitFromXML
//
//	Initializes from XML settings

	{
	CString sHost;
	CString sPort;
	CString sRootURL;

	//	Load parameters

	if (pDesc)
		{
		*retbModified = false;

		SetEnabled(pDesc->GetAttributeBool(ENABLED_ATTRIB));

		CString sHostspec = pDesc->GetAttribute(HOST_ATTRIB);
		urlParseHostspec(sHostspec, &sHost, &sPort);
		if (sHost.IsBlank() || strEquals(sHost, OLD_HOST))
			{
			sPort = NULL_STR;
			sHost = DEFAULT_HOST;
			*retbModified = true;
			}

		sRootURL = pDesc->GetAttribute(ROOT_URL_ATTRIB);
		if (sRootURL.IsBlank())
			{
			sRootURL = DEFAULT_URL;
			*retbModified = true;
			}

		m_sUsername = pDesc->GetAttribute(USERNAME_ATTRIB);
		}
	else
		{
		SetEnabled(true);

		sPort = NULL_STR;
		sHost = DEFAULT_HOST;
		sRootURL = DEFAULT_URL;

		*retbModified = true;
		}

	//	Initialize session

	m_Session.SetServer(sHost, sPort, sRootURL);
	m_Session.SetUsername(m_sUsername);

	return NOERROR;
	}

ALERROR CHexarcService::InitPrivateData (void)

//	InitPrivateData
//
//	Loads private data

	{
	//	Load the clientID and credentials from the registry

	CRegKey Key;
	if (CRegKey::OpenUserAppKey(REGISTRY_COMPANY_NAME,
			REGISTRY_PRODUCT_NAME,
			&Key) == NOERROR)
		{
		Key.FindStringValue(REGISTRY_CLIENT_ID_OPTION, &m_sClientID);

		CString sCredentials;
		Key.FindStringValue(REGISTRY_SAVED_KEY_OPTION, &sCredentials);

		if (!sCredentials.IsBlank())
			{
			CString sError;
			if (CJSONValue::Deserialize(sCredentials, &m_Credentials, &sError) != NOERROR)
				m_Credentials = CJSONValue();
			}
		else
			m_Credentials = CJSONValue();
		}

	//	If we could not find the clientID, then generate a new one.

	if (m_sClientID.IsBlank())
		{
		CIntegerIP RandomID;
		cryptoRandom(32, &RandomID);
		m_sClientID = RandomID.AsBase64();

		//	Remember to save it out.

		SetModified();
		}

	//	Initialize session

	m_Session.SetClientID(m_sClientID);

	//	Done

	return NOERROR;
	}

ALERROR CHexarcService::LoadNews (ITaskProcessor *pProcessor, 
								  CMultiverseModel &Multiverse, 
								  const SFileVersionInfo &AppVersion, 
								  const CString &sCacheFilespec, 
								  CString *retsResult)

//	LoadNews
//
//	Loads news from the Multiverse

	{
	DEBUG_TRY

	int i;

	if (!Multiverse.IsLoadNewsNeeded())
		return NOERROR;

	SendServiceStatus(STATUS_LOAD_NEWS);

	//	Generate a string for the version number

	CString sAppVersion = strPatternSubst(CONSTLIT("%d.%d.%d.%d"), 
			HIWORD((DWORD)(AppVersion.dwProductVersion >> 32)), 
			LOWORD((DWORD)(AppVersion.dwProductVersion >> 32)),
			HIWORD((DWORD)(AppVersion.dwProductVersion & 0xffffffff)),
			LOWORD((DWORD)(AppVersion.dwProductVersion & 0xffffffff)));

	//	Put together a request

	CJSONValue Payload(CJSONValue::typeObject);
	Payload.InsertHandoff(FIELD_API_VERSION, CJSONValue((int)API_VERSION));
	Payload.InsertHandoff(FIELD_TRANSCENDENCE_VERSION, CJSONValue(sAppVersion));
	Payload.InsertHandoff(FIELD_TYPE, CJSONValue(TYPE_TRANSCENDENCE_NEWS));
	if (IsLoggedIn())
		Payload.InsertHandoff(FIELD_USERNAME, CJSONValue(m_sUsername));

	CJSONValue Result;
	if (m_Session.ServerCommand(METHOD_POST, FUNC_LIST_ARTICLES, Payload, &Result) != NOERROR)
		{
		*retsResult = Result.AsString();
		SendServiceError(strPatternSubst(ERR_SERVER_COMMAND_ERROR, *retsResult));
		return ERR_FAIL;
		}

	//	Let the multiverse load the data and get back a list of files that we
	//	need to download.

	TSortMap<CString, CString> Downloads;
	if (Multiverse.SetNews(Result, sCacheFilespec, &Downloads, retsResult) != NOERROR)
		{
		SendServiceError(ERR_SERVER_RESPONSE_ERROR);
		return ERR_FAIL;
		}

	//	Make sure we have all images

	for (i = 0; i < Downloads.GetCount(); i++)
		{
		const CString &sURL = urlCompose(PROTOCOL_HTTP, m_Session.GetHostspec(), Downloads.GetKey(i));
		const CString &sDestFilespec = Downloads.GetValue(i);

		if (DownloadFile(sURL, sDestFilespec, NULL, retsResult) != NOERROR)
			{
			SendServiceError(strPatternSubst(ERR_SERVER_COMMAND_ERROR, *retsResult));
			return ERR_FAIL;
			}
		}

	SendServiceStatus(NULL_STR);

	return NOERROR;

	DEBUG_CATCH
	}

ALERROR CHexarcService::LoadUserCollection (ITaskProcessor *pProcessor, CExtensionCollection &Extensions, CMultiverseModel &Multiverse, CString *retsResult)

//	LoadUserCollection
//
//	Loads the user's collection from the cloud.

	{
	//	Set state on Multiverse object so that everyone else knows what's
	//	going on.

	Multiverse.OnCollectionLoading();

	SendServiceStatus(STATUS_LOAD_COLLECTION);

	//	Must be signed in

	if (!IsLoggedIn())
		{
		Multiverse.OnCollectionLoadFailed();
		*retsResult = ERR_NOT_SIGNED_IN;
		SendServiceError(ERR_SIGN_IN_FAILED);
		return ERR_FAIL;
		}

	//	Put together a request

	CJSONValue TypesRequested(CJSONValue::typeArray);
	TypesRequested.InsertHandoff(CJSONValue(TYPE_TRANSCENDENCE_ADVENTURE));
	TypesRequested.InsertHandoff(CJSONValue(TYPE_TRANSCENDENCE_EXPANSION));
	TypesRequested.InsertHandoff(CJSONValue(TYPE_TRANSCENDENCE_LIBRARY));
	TypesRequested.InsertHandoff(CJSONValue(TYPE_GAME_ENGINE));

	CJSONValue Payload(CJSONValue::typeObject);
	Payload.InsertHandoff(FIELD_API_VERSION, CJSONValue((int)API_VERSION));
	Payload.Insert(FIELD_AUTH_TOKEN, m_UserToken);
	Payload.InsertHandoff(FIELD_ONLY_IN_COLLECTION, CJSONValue(CJSONValue::typeTrue));
	Payload.InsertHandoff(FIELD_TYPE, TypesRequested);

	CJSONValue Result;
	if (m_Session.ServerCommand(METHOD_POST, FUNC_LIST_CATALOG, Payload, &Result) != NOERROR)
		{
		Multiverse.OnCollectionLoadFailed();
		*retsResult = Result.AsString();
		SendServiceError(strPatternSubst(ERR_SERVER_COMMAND_ERROR, *retsResult));
		return ERR_FAIL;
		}

	//	Let the multiverse load the data

	if (Multiverse.SetCollection(Result, Extensions, retsResult) != NOERROR)
		{
		SendServiceError(ERR_SERVER_RESPONSE_ERROR);
		return ERR_FAIL;
		}

	SendServiceStatus(NULL_STR);

	return NOERROR;
	}

ALERROR CHexarcService::PostCrashReport (ITaskProcessor *pProcessor, const CString &sCrash, CString *retsResult)

//	PostCrashReport
//
//	Posts a crash report to the cloud

	{
	ALERROR error;

	//	If we've already reported a crash this session, then don't report a second one.
	//	This can happen sometimes if we leave things in a bad state and try to paint
	//	the crash screen.

	if (m_bCrashReported)
		return NOERROR;

	m_bCrashReported = true;

	//	Put together a payload for posting

	CJSONValue Result;
	CJSONValue Payload(CJSONValue::typeObject);
	if (IsLoggedIn())
		Payload.Insert(FIELD_AUTH_TOKEN, m_UserToken);
	Payload.InsertHandoff(FIELD_API_VERSION, CJSONValue((int)API_VERSION));
	Payload.InsertHandoff(FIELD_CLIENT_ID, CJSONValue(m_sClientID));
	Payload.InsertHandoff(FIELD_CLIENT_VERSION, CJSONValue(m_Session.GetClientVersion()));
	Payload.InsertHandoff(FIELD_DEBUG_LOG, CJSONValue(sCrash));

	if (error = m_Session.ServerCommand(METHOD_POST, FUNC_POST_CRASH_LOG, Payload, &Result))
		{
		*retsResult = Result.AsString();
		return ERR_FAIL;
		}

	//	Done

	return NOERROR;
	}

ALERROR CHexarcService::PostGameRecord (ITaskProcessor *pProcessor, const CGameRecord &Record, const CGameStats &Stats, CString *retsResult)

//	PostGameRecord
//
//	Posts a game record to the cloud

	{
	ALERROR error;

	//	Don't bother is we got a score of 0 or if the game is in debug mode

	if (Record.GetScore() == 0 || Record.IsDebug())
		return NOERROR;

	SendServiceStatus(STATUS_POSTING_GAME_RECORD);

	//	Must be signed in

	if (!IsLoggedIn())
		{
		*retsResult = ERR_NOT_SIGNED_IN;
		SendServiceError(ERR_SIGN_IN_FAILED);
		return ERR_FAIL;
		}

	//	Convert the game record to a JSON value

	CJSONValue RecordJSON;
	Record.SaveToJSON(&RecordJSON);

	//	Add the stats to it

	CJSONValue StatsJSON;
	Stats.SaveToJSON(&StatsJSON);
	RecordJSON.InsertHandoff(FIELD_STATS, StatsJSON);

	//	Sign the record with the game's private key

	CIntegerIP Signature;
	if (!Sign(RecordJSON, &Signature))
		{
		*retsResult = ERR_UNABLE_TO_SIGN;
		SendServiceError(ERR_INTERNAL_ERROR);
		return ERR_FAIL;
		}

	CJSONValue SignatureJSON;
	CHexarc::ConvertToJSON(Signature, &SignatureJSON);

	//	Put together a payload for posting

	CJSONValue Result;
	CJSONValue Payload(CJSONValue::typeObject);
	Payload.InsertHandoff(FIELD_API_VERSION, CJSONValue((int)API_VERSION));
	Payload.InsertHandoff(FIELD_GAME, CJSONValue(GAME_TYPE));
	Payload.InsertHandoff(FIELD_GAME_STATS, RecordJSON);
	Payload.InsertHandoff(FIELD_SIGNATURE, SignatureJSON);
	Payload.Insert(FIELD_AUTH_TOKEN, m_UserToken);
	if (error = m_Session.ServerCommand(METHOD_POST, FUNC_POST_GAME, Payload, &Result))
		{
		//	An out of date error is expected if we post a score that is lower
		//	than the user's current high score.

		if (error == ERR_OUTOFDATE)
			;
		else
			{
			*retsResult = Result.AsString();
			SendServiceError(strPatternSubst(ERR_SERVER_COMMAND_ERROR, *retsResult));
			return ERR_FAIL;
			}
		}

	//	Done

	SendServiceStatus(STATUS_POST_SUCCESSFUL);
	return NOERROR;
	}

ALERROR CHexarcService::ProcessDownloads (ITaskProcessor *pProcessor, CString *retsResult)

//	ProcessDownloads
//
//	Process downloads.

	{
	ALERROR error;
	CHexarcDownloader::SStatus Status;

	//	Get the current status

	m_Downloader.GetStatus(&Status);
	SendServiceStatus(strPatternSubst(STATUS_DOWNLOADING_FILE, pathGetFilename(Status.sFilespec), Status.iProgress));

	if (error = m_Downloader.Update(m_Session, &Status, retsResult))
		{
		//	If there are no more files then nothing to do.

		if (error == ERR_NOTFOUND)
			{
			SendServiceStatus(NULL_STR);
			m_HI.HIPostCommand(CMD_SERVICE_DOWNLOADS_COMPLETE);
			::kernelDebugLogPattern("All downloads complete.");
			return NOERROR;
			}

		//	If we need more then ask the controller for more.

		else if (error == ERR_MORE)
			{
			SendServiceStatus(strPatternSubst(STATUS_DOWNLOADING_FILE, pathGetFilename(Status.sFilespec), Status.iProgress));
			m_HI.HIPostCommand(CMD_SERVICE_DOWNLOADS_IN_PROGRESS);
			return NOERROR;
			}

		//	Otherwise this is a real error

		else
			{
			SendServiceError(strPatternSubst(ERR_CANT_DOWNLOAD, *retsResult));
			::kernelDebugLogPattern("Error downloading: %s", (retsResult ? *retsResult : NULL_STR));
			return ERR_FAIL;
			}
		}

	//	We're done downloading a file. Tell the controller.

	SendServiceStatus(strPatternSubst(STATUS_FILE_DOWNLOADED, pathGetFilename(Status.sFilespec)));
	m_HI.HIPostCommand(CMD_SERVICE_FILE_DOWNLOADED, new CHexarcDownloader::SStatus(Status));
	::kernelDebugLogPattern("Downloaded: %s", Status.sFilespec);

	return NOERROR;
	}

ALERROR CHexarcService::ReadHighScoreList (ITaskProcessor *pProcessor, DWORD dwAdventure, CAdventureHighScoreList *retHighScores, CString *retsResult)

//	ReadHighScoreList
//
//	Returns the high score list for the given adventure. We return up to 100
//	entries.

	{
	//	Ask the server.

	CJSONValue Result;
	CJSONValue Payload(CJSONValue::typeObject);
	Payload.InsertHandoff(FIELD_API_VERSION, CJSONValue((int)API_VERSION));
	Payload.InsertHandoff(FIELD_GAME, CJSONValue(GAME_TYPE));
	Payload.InsertHandoff(FIELD_LEAGUE, CJSONValue(LEAGUE_REGISTERED));
	Payload.InsertHandoff(FIELD_MAX_GAMES, CJSONValue(100));
	Payload.InsertHandoff(FIELD_UNID, CJSONValue(strPatternSubst(CONSTLIT("%08x"), dwAdventure)));
	if (m_Session.ServerCommand(METHOD_OPTIONS, FUNC_HIGH_SCORE_GAMES, Payload, &Result) != NOERROR)
		{
		*retsResult = Result.AsString();
		return ERR_FAIL;
		}

	//	Initialize the results.

	return retHighScores->InitFromJSON(dwAdventure, Result);
	}

ALERROR CHexarcService::ReadProfile (ITaskProcessor *pProcessor, CUserProfile *retProfile, CString *retsResult)

//	ReadProfile
//
//	Reads the user profile from the server

	{
	int i, j;

	//	Initialize profile

	retProfile->Init(m_sUsername);

	//	Get the profile from the server. We get back a structure that looks like
	//	this:
	//	{
	//		id: The username key.
	//		trans: {
	//			adventures: [
	//				{
	//					unid: adventure UNID key (a string)
	//					personalBest: game record for personal best
	//					mostRecent: game record for most recent by player
	//					leaderboard: array of top 10 game records
	//					}
	//				]
	//			}
	//		}

	CJSONValue Result;
	CJSONValue Payload(CJSONValue::typeObject);
	Payload.InsertHandoff(FIELD_API_VERSION, CJSONValue((int)API_VERSION));
	Payload.InsertHandoff(FIELD_INCLUDE_LEADERBOARD, CJSONValue(CJSONValue::typeTrue));
	Payload.InsertHandoff(FIELD_GAMES, CJSONValue(GAME_TYPE));
	Payload.InsertHandoff(FIELD_USERNAME, CJSONValue(m_sUsername));
	if (m_Session.ServerCommand(METHOD_OPTIONS, FUNC_GET_PROFILE_RECORDS, Payload, &Result) != NOERROR)
		{
		*retsResult = Result.AsString();
		return ERR_FAIL;
		}

	//	Get the list of adventures

	CJSONValue TransRecord = Result.GetElement(FIELD_TRANS);
	CJSONValue AdventureList = TransRecord.GetElement(FIELD_ADVENTURES);

	//	Loop over all adventure records

	for (i = 0; i < AdventureList.GetCount(); i++)
		{
		CJSONValue AdventureEntry = AdventureList.GetElement(i);
		CString sAdventureKey = AdventureEntry.GetElement(FIELD_UNID).AsString();
		DWORD dwAdventure = strParseIntOfBase(sAdventureKey.GetASCIIZPointer(), 16, 0);
		if (dwAdventure == 0)
			continue;

		CAdventureRecord &DestRecord = retProfile->InsertAdventureRecord(dwAdventure);

		//	Add personal best

		CJSONValue PersonalBest = AdventureEntry.GetElement(FIELD_PERSONAL_BEST);
		if (!PersonalBest.IsNull())
			{
			CGameRecord &GameRecord = DestRecord.GetRecordAt(CAdventureRecord::personalBest);
			GameRecord.InitFromJSON(PersonalBest);
			}

		//	Add most recent game

		CJSONValue MostRecent = AdventureEntry.GetElement(FIELD_MOST_RECENT);
		if (!MostRecent.IsNull())
			{
			CGameRecord &GameRecord = DestRecord.GetRecordAt(CAdventureRecord::mostRecent);
			GameRecord.InitFromJSON(MostRecent);
			}

		//	Add adventure high scores

		CJSONValue Leaderboard = AdventureEntry.GetElement(FIELD_LEADERBOARD);
		for (j = 0; j < Leaderboard.GetCount(); j++)
			{
			CAdventureRecord &DestRecord = retProfile->InsertAdventureRecord(dwAdventure);
			CGameRecord &GameRecord = DestRecord.InsertHighScore();
			GameRecord.InitFromJSON(Leaderboard.GetElement(j));
			}
		}

#if 0
	//	Read the list of high scores for this user for all registered adventures

	CJSONValue Result;
	CJSONValue Payload(CJSONValue::typeObject);
	Payload.InsertHandoff(FIELD_API_VERSION, CJSONValue((int)API_VERSION));
	Payload.InsertHandoff(FIELD_GAME, CJSONValue(GAME_TYPE));
	Payload.InsertHandoff(FIELD_USERNAME, CJSONValue(m_sUsername));
	Payload.InsertHandoff(FIELD_MAX_GAMES, CJSONValue(10));
	Payload.InsertHandoff(FIELD_LEAGUE, CJSONValue(LEAGUE_REGISTERED));
	if (m_Session.ServerCommand(METHOD_OPTIONS, FUNC_HIGH_SCORE_GAMES, Payload, &Result) != NOERROR)
		{
		*retsResult = Result.AsString();
		return ERR_FAIL;
		}

	//	For each adventure, add it to our personal best

	for (i = 0; i < Result.GetCount(); i++)
		{
		const CJSONValue &Record = Result.GetElement(i);
		DWORD dwAdventure = (DWORD)Record.GetElement(FIELD_ADVENTURE).AsInt32();
		if (dwAdventure == 0)
			continue;

		CAdventureRecord &DestRecord = retProfile->InsertAdventureRecord(dwAdventure);
		CGameRecord &GameRecord = DestRecord.GetRecordAt(CAdventureRecord::personalBest);
		GameRecord.InitFromJSON(Record);
		}

	//	Read the list of recent games for this user for all registered adventures

	Payload = CJSONValue(CJSONValue::typeObject);
	Payload.InsertHandoff(FIELD_API_VERSION, CJSONValue((int)API_VERSION));
	Payload.InsertHandoff(FIELD_GAME, CJSONValue(GAME_TYPE));
	Payload.InsertHandoff(FIELD_USERNAME, CJSONValue(m_sUsername));
	Payload.InsertHandoff(FIELD_MAX_GAMES, CJSONValue(10));
	Payload.InsertHandoff(FIELD_LEAGUE, CJSONValue(LEAGUE_REGISTERED));
	if (m_Session.ServerCommand(METHOD_OPTIONS, FUNC_RECENT_GAMES, Payload, &Result) != NOERROR)
		{
		*retsResult = Result.AsString();
		return ERR_FAIL;
		}

	//	For each adventure, add it to our recent games

	for (i = 0; i < Result.GetCount(); i++)
		{
		const CJSONValue &Record = Result.GetElement(i);
		DWORD dwAdventure = (DWORD)Record.GetElement(FIELD_ADVENTURE).AsInt32();
		if (dwAdventure == 0)
			continue;

		CAdventureRecord &DestRecord = retProfile->InsertAdventureRecord(dwAdventure);

		//	If we've already got this game record, then skip it.

		if (DestRecord.FindRecordByGameID(Record.GetElement(FIELD_GAME_ID).AsString()))
			continue;

		CGameRecord &GameRecord = DestRecord.GetRecordAt(CAdventureRecord::mostRecent);
		GameRecord.InitFromJSON(Record);
		}

	//	Read the list of high scores

	Payload = CJSONValue(CJSONValue::typeObject);
	Payload.InsertHandoff(FIELD_API_VERSION, CJSONValue((int)API_VERSION));
	Payload.InsertHandoff(FIELD_GAME, CJSONValue(GAME_TYPE));
	Payload.InsertHandoff(FIELD_MAX_GAMES, CJSONValue(10));
	Payload.InsertHandoff(FIELD_LEAGUE, CJSONValue(LEAGUE_REGISTERED));
	if (m_Session.ServerCommand(METHOD_OPTIONS, FUNC_HIGH_SCORE_GAMES, Payload, &Result) != NOERROR)
		{
		*retsResult = Result.AsString();
		return ERR_FAIL;
		}

	//	For each adventure, add it to the high score

	for (i = 0; i < Result.GetCount(); i++)
		{
		const CJSONValue &Record = Result.GetElement(i);
		DWORD dwAdventure = (DWORD)Record.GetElement(FIELD_ADVENTURE).AsInt32();
		if (dwAdventure == 0)
			continue;

		//	If the adventure hasn't already been added, then don't add it
		//	(we only show adventures that the user has played)

		if (!retProfile->FindAdventureRecord(dwAdventure))
			continue;

		CAdventureRecord &DestRecord = retProfile->InsertAdventureRecord(dwAdventure);
		CGameRecord &GameRecord = DestRecord.InsertHighScore();
		GameRecord.InitFromJSON(Record);
		}
#endif

	//	Done

	return NOERROR;
	}

ALERROR CHexarcService::RegisterUser (ITaskProcessor *pProcessor, const CString &sUsername, const CString &sPassword, const CString &sEmail, bool bAutoSignIn, CString *retsResult)

//	RegisterUser
//
//	Register a new user

	{
	//	Generate a hash of the username and password

	CJSONValue PasswordHash;
	if (!CHexarc::CreateCredentials(sUsername, sPassword, &PasswordHash))
		{
		*retsResult = ERR_CANNOT_DIGEST_PASSWORD;
		return ERR_FAIL;
		}

	//	Put together a payload for registration

	CJSONValue Result;
	CJSONValue Payload(CJSONValue::typeObject);
	Payload.InsertHandoff(FIELD_API_VERSION, CJSONValue((int)API_VERSION));
	Payload.InsertHandoff(FIELD_USERNAME, CJSONValue(sUsername));
	Payload.InsertHandoff(FIELD_CREDENTIALS, PasswordHash);
	Payload.InsertHandoff(FIELD_EMAIL, CJSONValue(sEmail));
	if (m_Session.ServerCommand(METHOD_POST, FUNC_REGISTER, Payload, &Result) != NOERROR)
		{
		*retsResult = Result.AsString();
		return ERR_FAIL;
		}

	//	If successful, then we initialize our data

	m_sUsername = sUsername;
	m_Credentials = (bAutoSignIn ? Result.GetElement(FIELD_CREDENTIALS) : CJSONValue());
	m_UserToken = Result.GetElement(FIELD_AUTH_TOKEN);
	SetModified();

	m_HI.HIPostCommand(CMD_SERVICE_ACCOUNT_CHANGED);

	return NOERROR;
	}

ALERROR CHexarcService::RequestExtensionDownload (const CString &sFilePath, const CString &sFilespec, const CIntegerIP &FileDigest)

//	RequestExtensionDownload
//
//	Requests a download of the given extension file.

	{
	m_Downloader.AddRequest(FUNC_DOWNLOAD_CATALOG_ENTRY_FILE, sFilePath, m_UserToken, sFilespec, FileDigest);
	return NOERROR;
	}

bool CHexarcService::Sign (const CJSONValue &Value, CIntegerIP *retSignature)

//	Sign
//
//	Signs the given JSON value using the Transcendence client secret key and the
//	algorithm (MAC) used by Hexarc.
//
//	NOTE: We keep this here to obfuscate it (CHexarcService is not open-source)

	{
	//	Get the secret key

	CIntegerIP SecretKey(KEY_SIZE, g_TransClient01);

	//	Generate a stream

	CMemoryWriteStream Document(8192);
	if (Document.Create() != NOERROR)
		return false;

	//	Serialize the JSON value in AEON format

	CHexarc::WriteAsAeon(Value, Document);

	//	Sign

	cryptoCreateMAC(CMemoryReadBlockWrapper(Document), SecretKey, retSignature);
	return true;
	}

ALERROR CHexarcService::SignInUser (ITaskProcessor *pProcessor, const CString &sUsername, const CString &sPassword, bool bAutoSignIn, CString *retsResult)

//	SignInUser
//
//	Signs in a user

	{
	//	Figure out if we should use cached credentials

	bool bUseCachedCredentials = sUsername.IsBlank();
	if (bUseCachedCredentials)
		{
		if (m_sUsername.IsBlank())
			return ERR_FAIL;

		if (m_Credentials.IsNull())
			return ERR_FAIL;
		}

	//	Otherwise, set the username because the Connect method uses it.

	else
		{
		m_sUsername = sUsername;
		m_Session.SetUsername(m_sUsername);
		}

	SendServiceStatus(STATUS_SIGNING_IN);

	//	Ask for the challenge

	CJSONValue Result;
	CJSONValue GetChallenge(CJSONValue::typeObject);
	GetChallenge.InsertHandoff(FIELD_USERNAME, CJSONValue(m_sUsername));
	if (!bUseCachedCredentials)
		GetChallenge.Insert(FIELD_ACTUAL, CJSONValue(CJSONValue::typeTrue));

	if (m_Session.ServerCommand(METHOD_POST, FUNC_REQUEST_LOGIN, GetChallenge, &Result) != NOERROR)
		{
		*retsResult = Result.AsString();
		SendServiceError(strPatternSubst(ERR_SERVER_COMMAND_ERROR, *retsResult));
		return ERR_FAIL;
		}

	//	Convert the challenge to a string

	CString sChallenge;
	if (!CHexarc::ConvertIPIntegerToString(Result, &sChallenge))
		{
		*retsResult = strPatternSubst(ERR_INVALID_RESPONSE, m_Session.GetHostspec());
		SendServiceError(ERR_SERVER_RESPONSE_ERROR);
		return ERR_FAIL;
		}

	//	Hash the password (or credentials)

	CString sPasswordHash;
	if (bUseCachedCredentials)
		CHexarc::ConvertIPIntegerToString(m_Credentials, &sPasswordHash);
	else
		CHexarc::CreateCredentials(m_sUsername, sPassword, &sPasswordHash);

	//	Generate the proper response

	CJSONValue ChallengeCredentials;
	if (!CHexarc::ConvertToJSON(CDigest(CBufferReadBlock(strPatternSubst("%s:%s", sPasswordHash, sChallenge))), &ChallengeCredentials))
		{
		*retsResult = ERR_CANNOT_DIGEST_PASSWORD;
		SendServiceError(ERR_SERVER_RESPONSE_ERROR);
		return ERR_FAIL;
		}

	//	Login

	CJSONValue Login(CJSONValue::typeObject);
	if (!bUseCachedCredentials)
		Login.InsertHandoff(FIELD_ACTUAL, CJSONValue(CJSONValue::typeTrue));
	Login.InsertHandoff(FIELD_USERNAME, CJSONValue(m_sUsername));
	Login.InsertHandoff(FIELD_CHALLENGE_CREDENTIALS, ChallengeCredentials);
	if (m_Session.ServerCommand(METHOD_POST, FUNC_LOGIN, Login, &Result) != NOERROR)
		{
		//	If we fail, then we clear the stored credentials so that we don't
		//	try to sign in again.

		if (bUseCachedCredentials)
			{
			m_Credentials = CJSONValue();
			SetModified();
			}

		//	If this is autologin in the fail silently, since our credentials 
		//	might have expired.

		if (bAutoSignIn)
			{
			SendServiceStatus(NULL_STR);
			return NOERROR;
			}

		//	Otherwise, return the error.

		*retsResult = Result.AsString();
		SendServiceError(strPatternSubst(ERR_SERVER_COMMAND_ERROR, *retsResult));
		return ERR_FAIL;
		}

	//	We get back an auth token and credentials

	m_sUsername = Result.GetElement(FIELD_USERNAME).AsString();
	m_UserToken = Result.GetElement(FIELD_AUTH_TOKEN);
	if (!bUseCachedCredentials)
		m_Credentials = (bAutoSignIn ? Result.GetElement(FIELD_SCOPED_CREDENTIALS) : CJSONValue());

	//	Tell the session that we have a new username

	m_Session.SetUsername(m_sUsername);

	//	Done

	SetModified();

	m_HI.HIPostCommand(CMD_SERVICE_ACCOUNT_CHANGED);
	SendServiceStatus(NULL_STR);

	return NOERROR;
	}

ALERROR CHexarcService::SignOutUser (ITaskProcessor *pProcessor, CString *retsResult)

//	SignOutUser
//
//	Signs out a user

	{
	//	Clear out the user token

	m_UserToken = CJSONValue();
	m_Credentials = CJSONValue();
	m_HI.HIPostCommand(CMD_SERVICE_ACCOUNT_CHANGED);
	SendServiceStatus(NULL_STR);

	return NOERROR;
	}

ALERROR CHexarcService::WriteAsXML (IWriteStream *pOutput)

//	WriteAsXML
//
//	Writes settings to XML file

	{
	ALERROR error;

	CString sData = strPatternSubst(CONSTLIT(
				"\t\t<Hexarc enabled=\"%s\"\r\n"
				"\t\t\tusername=\"%s\"\r\n"
				"\t\t\thost=\"%s\"\r\n"
				"\t\t\trootURL=\"%s\"\r\n"
				"\t\t\t/>\r\n"), 
			(IsEnabled() ? CONSTLIT("true") : CONSTLIT("false")),
			strToXMLText(m_sUsername),
			strToXMLText(m_Session.GetHostspec()),
			strToXMLText(m_Session.GetRootURL())
			);
	if (error = pOutput->Write(sData.GetPointer(), sData.GetLength(), NULL))
		return error;

	return NOERROR;
	}

ALERROR CHexarcService::WritePrivateData (void)

//	WritePrivateData
//
//	Writes private data.

	{
	ALERROR error;

	//	Write the clientID to the registry.

	CRegKey Key;
	if (error = CRegKey::OpenUserAppKey(REGISTRY_COMPANY_NAME,
			REGISTRY_PRODUCT_NAME,
			&Key))
		return error;

	//	Save the clientID

	Key.SetStringValue(REGISTRY_CLIENT_ID_OPTION, m_sClientID);

	//	Save the credentials, if we have it.

	CString sCredentials;
	if (!m_Credentials.IsNull())
		{
		CMemoryWriteStream Output(4096);
		if (error = Output.Create())
			return error;

		m_Credentials.Serialize(&Output);

		sCredentials = CString(Output.GetPointer(), Output.GetLength());
		}

	Key.SetStringValue(REGISTRY_SAVED_KEY_OPTION, sCredentials);

	//	Done

	return NOERROR;
	}

//	CHexarcServiceFactory ------------------------------------------------------

ICIService *CHexarcServiceFactory::Create (CHumanInterface &HI)
	{
	return new CHexarcService(HI);
	}

