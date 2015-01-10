//	TSUISessions.h
//
//	Transcendence UI Engine
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CMessageSession : public IHISession
	{
	public:
		CMessageSession (CHumanInterface &HI, const CString &sTitle, const CString &sMessage, const CString &sCommand = NULL_STR);

		//	IHISession virtuals
		virtual ALERROR OnCommand (const CString &sCmd, void *pData = NULL);
		virtual ALERROR OnInit (CString *retsError);
		virtual void OnKeyDown (int iVirtKey, DWORD dwKeyData);
		virtual void OnPaint (CG16bitImage &Screen, const RECT &rcInvalid);
		virtual void OnReportHardCrash (CString *retsMessage);

	private:
		void CmdDone (void);
		void CreateDlgMessage (IAnimatron **retpDlg);

		CString m_sTitle;
		CString m_sMessage;

		CString m_sCommand;					//	Command to issue after we're done
	};
