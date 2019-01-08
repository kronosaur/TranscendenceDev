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

        virtual void AccumulateExtensionFolders (TArray<CString> &Folders) override;
		virtual CString GetTag (void) override;
		virtual CString GetUsername (void) override { return m_sUsername; }
		virtual bool HasCapability (DWORD dwCapability) override;
		virtual ALERROR InitFromXML (CXMLElement *pDesc, bool *retbModified) override;
		virtual ALERROR LoadUserCollection (ITaskProcessor *pProcessor, CExtensionCollection &Extensions, CMultiverseModel &Multiverse, CString *retsResult = NULL) override;
		virtual ALERROR PostGameRecord (ITaskProcessor *pProcessor, const CGameRecord &Record, const CGameStats &Stats, CString *retsResult = NULL) override;
		virtual ALERROR ReadHighScoreList (ITaskProcessor *pProcessor, DWORD dwAdventure, CAdventureHighScoreList *retHighScores, CString *retsResult = NULL) override;
		virtual ALERROR SignInUser (ITaskProcessor *pProcessor, const CString &sUsername, const CString &sPassword, bool bAutoSignIn, CString *retsResult = NULL) override;
		virtual ALERROR WriteAsXML (IWriteStream *pOutput) override;

	private:
		bool m_bConnected;
		CString m_sUsername;
	};

