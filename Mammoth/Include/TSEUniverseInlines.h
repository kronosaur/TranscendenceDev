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
			m_bRunning = true;
#endif
			}

		CUsePerformanceCounter (CUniverse &Universe, const SEventHandlerDesc &Event) :
				m_Universe(Universe)
			{
#ifdef DEBUG_PERFORMANCE_COUNTERS
			if (m_Universe.GetPerformanceCounters().IsEnabled())
				{
				if (Event.sEvent.IsBlank())
					m_sID = CONSTLIT("event.UnknownEvent");
				else
					{
					m_sID = CONSTLIT("event.");
					m_sID.Append(Event.sEvent);
					}

				m_Universe.GetPerformanceCounters().StartCounter(m_sID);
				m_bRunning = true;
				}
#endif
			}

		~CUsePerformanceCounter (void)
			{
			StopCounter();
			}

		void StopCounter (void)
			{
#ifdef DEBUG_PERFORMANCE_COUNTERS
			if (m_bRunning)
				{
				m_Universe.GetPerformanceCounters().StopCounter(m_sID);
				m_bRunning = false;
				}
#endif
			}

	private:
		CUniverse &m_Universe;
		CString m_sID;
		bool m_bRunning = false;
	};

//	CCodeChainCtx --------------------------------------------------------------

inline void CCodeChainCtx::DefineType (const CDesignType *pType) { DefineType(pType ? pType->GetUNID() : 0); }
