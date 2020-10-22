//	CPerformanceCounters.cpp
//
//	CPerformanceCounters class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#ifdef DEBUG
//#define DEBUG_CCREFCOUNT
#endif

bool CPerformanceCounters::IsAnyCounterEnabled (void) const

//	IsAnyCounterEnabled
//
//	Returns TRUE if any counters is still enabled.

	{
	for (int i = 0; i < m_Counters.GetCount(); i++)
		if (m_Counters[i].bEnabled)
			return true;

	return false;
	}

void CPerformanceCounters::Paint (CG32bitImage &Dest, const RECT &rcRect, const CG16bitFont &Font) const

//	Paint
//
//	Paint the counters.

	{
	if (!m_bEnabled)
		return;

	constexpr CG32bitPixel RGB_TEXT_COLOR = CG32bitPixel(255, 255, 255);
	constexpr int START_Y = 200;

	int x = rcRect.left;
	int y = rcRect.top + START_Y;

	for (int i = 0; i < m_Counters.GetCount(); i++)
		{
		const SCounter &Counter = m_Counters[i];
		if (!Counter.bEnabled)
			continue;

		Metric rTime;
		if (Counter.iTotalCalls == 0)
			rTime = 0.0;
		else
			rTime = (Metric)Counter.iTotalTime / Counter.iTotalCalls;

		CString sLine = strPatternSubst(CONSTLIT("%s: %s ms [%s calls]"), m_Counters.GetKey(i), strFromDouble(rTime, 2), strFormatInteger(Counter.iTotalCalls, -1, FORMAT_THOUSAND_SEPARATOR));
		Font.DrawText(Dest, x, y, RGB_TEXT_COLOR, sLine);

		y += Font.GetHeight();
		}

#ifdef DEBUG_CCREFCOUNT
	CString sLine = strPatternSubst(CONSTLIT("CCNil: %d refCounts"), g_pUniverse->GetCC().GetNil()->GetRefCount());
	Font.DrawText(Dest, x, y, RGB_TEXT_COLOR, sLine);
	y += Font.GetHeight();
#endif
	}

bool CPerformanceCounters::SetEnabled (const CString &sID, bool bEnabled)

//	SetEnabled
//
//	Enable a specific performance counter.

	{
	SCounter *pCounter = m_Counters.GetAt(sID);
	if (pCounter == NULL)
		return false;

	if (pCounter->bEnabled != bEnabled)
		{
		pCounter->bEnabled = bEnabled;

		if (bEnabled)
			m_bEnabled = true;
		}

	return true;
	}

void CPerformanceCounters::StartTimer (const CString &sID)

//	StartTimer
//
//	Starts a performance counter.

	{
	SCounter *pCounter = m_Counters.SetAt(sID);
	if (pCounter->dwStartTime)
		return;

	pCounter->dwStartTime = ::GetTickCount();
	}

void CPerformanceCounters::StopTimer (const CString &sID)

//	StopTimer
//
//	Stops a performance counter.

	{
	DWORD dwStopTime = ::GetTickCount();

	SCounter *pCounter = m_Counters.SetAt(sID);
	if (pCounter->dwStartTime == 0)
		return;

	pCounter->iTotalCalls++;
	pCounter->iTotalTime += (dwStopTime - pCounter->dwStartTime);
	pCounter->dwStartTime = 0;
	}
