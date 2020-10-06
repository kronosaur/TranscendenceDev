//  Diagnostics.h
//
//  Classes to implement diagnostics
//  Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CRunDiagnostics
	{
	public:
		struct SSystemCreatePerf
			{
			DWORD dwTotalSystemCreateTime = 0;
			int iSystemsCreated = 0;
			};

		CRunDiagnostics (CUniverse &Universe) :
				m_Universe(Universe)
			{ }

		const CDesignCollection::SDiagnosticsCtx &GetDiagnostics (void) const { return m_DiagnosticsCtx; }
		const CSystem &GetSystem (int iIndex) const;
		int GetSystemCount (void) const { return m_NodeOrder.GetCount(); }
		const SSystemCreatePerf &GetSystemCreatePerformance (void) const { return m_SystemCreatePerf; }
		void RunSystem (int iIndex, int iUpdates = 0);
		bool Start (ILog *pProgress = NULL, CString *retsError = NULL);
		void Stop (void);

	private:
		CUniverse &m_Universe;

		//	Accumulate over each run

		int m_iSamples = 0;
		CDesignCollection::SDiagnosticsCtx m_DiagnosticsCtx;
		SSystemCreatePerf m_SystemCreatePerf;

		//	State for a specific run

		TSortMap<CString, CSystem *> m_AllSystems;
		TArray<CString> m_NodeOrder;
		bool m_bStartDiagnosticsCalled = false;
		bool m_bUniverseCreated = false;
	};

class CDiagnosticsCommand : public ITransDataCommand
	{
	public:
		CDiagnosticsCommand (CUniverse &Universe, ILog &Console) :
				ITransDataCommand(Console),
				m_Universe(Universe)
			{ }

		virtual bool Run (void) override;
		virtual bool SetOptions (const CXMLElement &CmdLine) override;

	private:
		CUniverse &m_Universe;
	};
