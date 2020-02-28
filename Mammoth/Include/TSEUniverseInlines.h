//	TSEUniverseInlines.h
//
//	Transcendence Space Engine
//	Copyright 2020 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CUsePerformanceCounter
	{
	public:
		CUsePerformanceCounter (CUniverse &Universe, const CString &sID) :
				m_Universe(Universe),
				m_sID(sID)
			{
#ifdef DEBUG_PERFORMANCE_COUNTERS
			m_Universe.GetPerformanceCounters().StartCounter(m_sID);
#endif
			}

		~CUsePerformanceCounter (void)
			{
#ifdef DEBUG_PERFORMANCE_COUNTERS
			m_Universe.GetPerformanceCounters().StopCounter(m_sID);
#endif
			}

	private:
		CUniverse &m_Universe;
		CString m_sID;
	};

class CUsePerformanceCounterForEvent
	{
	public:
		CUsePerformanceCounterForEvent (CUniverse &Universe, const CString &sEvent) :
				m_Universe(Universe),
				m_sID(strPatternSubst(CONSTLIT("event.%s"), sEvent))
			{
#ifdef DEBUG_PERFORMANCE_COUNTERS
			m_Universe.GetPerformanceCounters().StartCounter(m_sID);
#endif
			}

		~CUsePerformanceCounterForEvent (void)
			{
#ifdef DEBUG_PERFORMANCE_COUNTERS
			m_Universe.GetPerformanceCounters().StopCounter(m_sID);
#endif
			}

	private:
		CUniverse &m_Universe;
		CString m_sID;
	};

