//	CDiagnosticsCommand.cpp
//
//	CDiagnosticsCommand Class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Diagnostics.h"

bool CDiagnosticsCommand::Run (void)
	{
	CRunDiagnostics Diagnostics(m_Universe);
	bool bShowSystemCreateDetails = false;
	if (bShowSystemCreateDetails)
		m_Universe.GetPerformanceCounters().SetEnabled();

	CString sError;
	if (!Diagnostics.Start(&GetConsole(), &sError))
		return Error(sError);

	//	Output system create performance

	if (bShowSystemCreateDetails)
		{
		const auto &AllCounters = m_Universe.GetPerformanceCounters();
		for (int i = 0; i < AllCounters.GetCount(); i++)
			{
			auto &Counter = AllCounters.GetCounter(i);
			if (strStartsWith(AllCounters.GetCounterID(i), CONSTLIT("create.")))
				{
				double rTime = Counter.iTotalTime / (double)Counter.iTotalCalls;
				Print(strPatternSubst(CONSTLIT("%s: %s calls @ %s ms [%s ms total]"), 
					AllCounters.GetCounterID(i), 
					strFormatInteger(Counter.iTotalCalls, -1, FORMAT_THOUSAND_SEPARATOR), 
					strFromDouble(rTime, 2),
					strFormatInteger(Counter.iTotalTime, -1, FORMAT_THOUSAND_SEPARATOR)));
				}
			}
		}

	auto &Perf = Diagnostics.GetSystemCreatePerformance();
	Print(strPatternSubst(CONSTLIT("SYSTEMS CREATED: %d"), Perf.iSystemsCreated));
	Print(strPatternSubst(CONSTLIT("TOTAL SYSTEM CREATE: %s secs"), strFromDouble(Perf.dwTotalSystemCreateTime / 1000.0, 1)));
	double rAveTime = (Perf.iSystemsCreated > 0 ? Perf.dwTotalSystemCreateTime / (double)Perf.iSystemsCreated : 0.0);
	Print(strPatternSubst(CONSTLIT("AVERAGE SYSTEM CREATE: %s ms"), strFromDouble(rAveTime, 1)));

	//	Run Diagnostics

	for (int i = 0; i < Diagnostics.GetSystemCount(); i++)
		{
		const CSystem &System = Diagnostics.GetSystem(i);
		Progress(strPatternSubst(CONSTLIT("Diagnostics: [%s] %s"), System.GetTopology()->GetID(), System.GetName()));

		Diagnostics.RunSystem(i);
		}

	Progress(NULL_STR);
	Diagnostics.Stop();

	//	Output diagnostic results

	auto &Diag = Diagnostics.GetDiagnostics();
	Print(strPatternSubst(CONSTLIT("TOTAL DIAGNOSTICS: %d"), Diag.iTotalTests));
	Print(strPatternSubst(CONSTLIT("TOTAL ERRORS: %d"), Diag.iTotalErrors));

	return true;
	}

bool CDiagnosticsCommand::SetOptions (const CXMLElement &CmdLine)
	{
	return true;
	}
