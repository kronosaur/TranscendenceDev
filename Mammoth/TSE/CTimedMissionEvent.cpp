//	CTimedMissionEvent.cpp
//
//	CTimeMissionEvent class
//	Copyright (c) 2012 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CTimedMissionEvent::CTimedMissionEvent (int iTick,
										int iInterval,
										CMission *pMission,
										const CString &sEvent,
										const CString &sNode) :
		CSystemEvent(iTick),
		m_iInterval(iInterval),
		m_pMission(pMission),
		m_sEvent(sEvent),
		m_sNode(sNode)

//	CTimedMissionEvent constructor

	{
	}

CTimedMissionEvent::CTimedMissionEvent (SLoadCtx &Ctx) : CSystemEvent(Ctx)

//	CTimedMissionEvent constructor

	{
	DWORD dwLoad;

	Ctx.pStream->Read(dwLoad);
	m_pMission = Ctx.GetUniverse().FindMission(dwLoad);

	m_sEvent.ReadFromStream(Ctx.pStream);
	Ctx.pStream->Read(m_iInterval);

	if (Ctx.dwVersion >= 171)
		m_sNode.ReadFromStream(Ctx.pStream);
	}

CString CTimedMissionEvent::DebugCrashInfo (void)

//	DebugCrashInfo
//
//	Dump crash info

	{
	CString sResult;

	sResult.Append(CONSTLIT("CTimedMissionEvent\r\n"));
	sResult.Append(strPatternSubst(CONSTLIT("m_pMission: 0x%x\r\n"), (m_pMission ? m_pMission->GetID() : 0)));
	sResult.Append(strPatternSubst(CONSTLIT("event: %s\r\n"), m_sEvent));
	return sResult;
	}

void CTimedMissionEvent::DoEvent (DWORD dwTick, CSystem &System)

//	DoEvent
//
//	Run the event

	{
	DEBUG_TRY

	//	If we're not in the right system, then we don't run.

	if (!m_sNode.IsBlank() 
			&& System.GetTopology() 
			&& !strEquals(System.GetTopology()->GetID(), m_sNode))
		{
		//	Reset our time so that we're not firing every tick

		SetTick(dwTick + 30);
		return;
		}

	//	Run

	if (m_pMission)
		m_pMission->FireCustomEvent(m_sEvent, NULL);

	//	Next run

	if (m_iInterval)
		SetTick(dwTick + m_iInterval);
	else
		SetDestroyed();

	DEBUG_CATCH
	}

bool CTimedMissionEvent::OnIsEqual (CSystemEvent &SrcArg) const

//	OnIsEqual
//
//	Returns TRUE if we're equal. Our callers guarantee that Src is of the same
//	class.

	{
	CTimedMissionEvent &Src = reinterpret_cast<CTimedMissionEvent &>(SrcArg);
	return (Src.m_pMission == m_pMission && strEquals(Src.m_sEvent, m_sEvent));
	}

void CTimedMissionEvent::OnWriteToStream (CSystem *pSystem, IWriteStream *pStream)

//	OnWriteToStream
//
//	Write to stream
//
//	DWORD		m_pType (UNID)
//	CString		m_sEvent
//	DWORD		m_iInterval
//	CString		m_sNode

	{
	DWORD dwSave;

	dwSave = (m_pMission ? m_pMission->GetID() : 0);
	pStream->Write(dwSave);

	m_sEvent.WriteToStream(pStream);
	pStream->Write(m_iInterval);
	m_sNode.WriteToStream(pStream);
	}
