//	SteamUtil.h
//
//	Steam Client Integration Helpers
//	Copyright (c) 2015 by Kronosaur Productions. All Rights Reserved.

#pragma once

#include "TSUI.h"

class CSteamService : public ICIService
	{
	public:
		CSteamService (CHumanInterface &HI);
		~CSteamService (void);

		virtual CString GetTag (void);
		virtual bool HasCapability (DWORD dwCapability);
		virtual ALERROR InitFromXML (CXMLElement *pDesc, bool *retbModified);
		virtual ALERROR PostGameRecord (ITaskProcessor *pProcessor, const CGameRecord &Record, const CGameStats &Stats, CString *retsResult = NULL);
		virtual ALERROR WriteAsXML (IWriteStream *pOutput);

	private:
		bool m_bEnabled;					//	TRUE if this is enabled
		CString m_sUsername;
	};