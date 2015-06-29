//	SteamUtil.h
//
//	Steam Client Integration Helpers
//	Copyright (c) 2015 by Kronosaur Productions. All Rights Reserved.

#pragma once

#include "TSUI.h"

//	CSteamService --------------------------------------------------------------

class CSteamService : public ICIService
	{
	public:
		CSteamService (CHumanInterface &HI);
		~CSteamService (void);

		virtual CString GetTag (void);
		virtual CString GetUsername (void) { return m_sUsername; }
		virtual bool HasCapability (DWORD dwCapability);
		virtual ALERROR InitFromXML (CXMLElement *pDesc, bool *retbModified);
		virtual ALERROR LoadUserCollection (ITaskProcessor *pProcessor, CMultiverseModel &Multiverse, CString *retsResult = NULL);
		virtual ALERROR PostGameRecord (ITaskProcessor *pProcessor, const CGameRecord &Record, const CGameStats &Stats, CString *retsResult = NULL);
		virtual ALERROR ReadHighScoreList (ITaskProcessor *pProcessor, DWORD dwAdventure, CAdventureHighScoreList *retHighScores, CString *retsResult = NULL);
		virtual ALERROR SignInUser (ITaskProcessor *pProcessor, const CString &sUsername, const CString &sPassword, bool bAutoSignIn, CString *retsResult = NULL);
		virtual ALERROR WriteAsXML (IWriteStream *pOutput);

	private:
		bool m_bConnected;
		CString m_sUsername;
	};

