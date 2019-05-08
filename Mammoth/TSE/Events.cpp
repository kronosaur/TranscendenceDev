//	Events.cpp
//
//	Event classes
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define SEPARATION_CRITERIA						CONSTLIT("sTA")

static const Metric MIN_SEPARATION =			20.0 * LIGHT_SECOND;
static const Metric MIN_PLAYER_SEPARATION =		100.0 * LIGHT_SECOND;
static const Metric MIN_PLAYER_SEPARATION2 =	MIN_PLAYER_SEPARATION * MIN_PLAYER_SEPARATION;

CSystemEvent::CSystemEvent (SLoadCtx &Ctx)

//	CSystemEvent constructo

	{
	DWORD dwLoad;

	//	Load stuff

	Ctx.pStream->Read((char *)&m_dwTick, sizeof(DWORD));

	//	Load flags

	if (Ctx.dwVersion >= 52)
		{
		Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
		m_bDestroyed = ((dwLoad & 0x00000001) ? true : false);
		}
	else
		m_bDestroyed = false;
	}

void CSystemEvent::CreateFromStream (SLoadCtx &Ctx, CSystemEvent **retpEvent)

//	CreateFromStream
//
//	Creates a timed event from a stream
//
//	DWORD			class
//	DWORD			m_dwTick
//	DWORD			flags

	{
	//	Create the appropriate class

	DWORD dwLoad;
	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	CSystemEvent *pEvent;
	switch (dwLoad)
		{
		case cTimedEncounterEvent:
			pEvent = new CTimedEncounterEvent(Ctx);
			break;

		case cTimedCustomEvent:
			pEvent = new CTimedCustomEvent(Ctx);
			break;

		case cTimedRecurringEvent:
			pEvent = new CTimedRecurringEvent(Ctx);
			break;

		case cTimedTypeEvent:
			pEvent = new CTimedTypeEvent(Ctx);
			break;

		case cTimedMissionEvent:
			pEvent = new CTimedMissionEvent(Ctx);
			break;

		case cRangeTypeEvent:
			pEvent = new CRangeTypeEvent(Ctx);
			break;

		default:
			throw CException(ERR_FAIL, CONSTLIT("Invalid event type"));
		}

	//	Done

	*retpEvent = pEvent;
	}

void CSystemEvent::WriteToStream (CSystem *pSystem, IWriteStream *pStream)

//	WriteToStream
//
//	Writes the event to a stream
//
//	DWORD		class
//	DWORD		m_dwTick
//	DWORD		flags
//	Subclass data

	{
	DWORD dwClass = GetClass();
	pStream->Write((char *)&dwClass, sizeof(DWORD));
	pStream->Write((char *)&m_dwTick, sizeof(DWORD));

	//	Flags

	DWORD dwSave = 0;
	dwSave |= (m_bDestroyed ? 0x00000001 : 0);
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	//	Sub class

	OnWriteToStream(pSystem, pStream);
	}

//	CTimedEncounterEvent ------------------------------------------------------

CTimedEncounterEvent::CTimedEncounterEvent (int iTick,
											CSpaceObject *pTarget,
											DWORD dwEncounterTableUNID,
											CSpaceObject *pGate,
											const CVector &vPos,
											Metric rDistance) :
		CSystemEvent(iTick),
		m_dwEncounterTableUNID(dwEncounterTableUNID),
		m_pGate(pGate),
		m_vPos(vPos),
		m_rDistance(rDistance)

//	CTimedEncounterEvent constructor

	{
	if (pTarget)
		m_Targets.FastAdd(pTarget);
	}

CTimedEncounterEvent::CTimedEncounterEvent (int iTick,
											const CSpaceObjectList &Targets,
											DWORD dwEncounterTableUNID,
											CSpaceObject *pGate,
											const CVector &vPos,
											Metric rDistance) :
		CSystemEvent(iTick),
		m_Targets(Targets),
		m_dwEncounterTableUNID(dwEncounterTableUNID),
		m_pGate(pGate),
		m_vPos(vPos),
		m_rDistance(rDistance)

//	CTimedEncounterEvent constructor

	{
	}

CTimedEncounterEvent::CTimedEncounterEvent (SLoadCtx &Ctx) : CSystemEvent(Ctx)

//	CTimedEvencounterEvent constructor

	{
	Ctx.pStream->Read(m_dwEncounterTableUNID);

	if (Ctx.dwVersion >= 166)
		m_Targets.ReadFromStream(Ctx);
	else
		m_Targets.ReadFromStreamSingle(Ctx);

	CSystem::ReadObjRefFromStream(Ctx, &m_pGate);
	Ctx.pStream->Read(m_rDistance);

	if (Ctx.dwVersion >= 161)
		m_vPos.ReadFromStream(*Ctx.pStream);
	}

CVector CTimedEncounterEvent::CalcEncounterPos (CSpaceObject *pTarget, Metric rDistance) const

//	CalcEncounterPos
//
//	Generates a random position for the encounter.

	{
	//	Short-circuit

	if (pTarget == NULL)
		return CVector();

	CSystem *pSystem = pTarget->GetSystem();
	if (pSystem == NULL)
		return CVector();

	return pSystem->CalcRandomEncounterPos(*pTarget, rDistance);
	}

CString CTimedEncounterEvent::DebugCrashInfo (void)

//	DebugCrashInfo

	{
	CString sResult;

	sResult.Append(CONSTLIT("CTimedEncounterEvent\r\n"));
	return sResult;
	}

void CTimedEncounterEvent::DoEvent (DWORD dwTick, CSystem &System)

//	DoEvent
//
//	Do the actual event

	{
	DEBUG_TRY

	CShipTable *pTable = g_pUniverse->FindEncounterTable(m_dwEncounterTableUNID);
	if (pTable == NULL)
		{
		SetDestroyed();
		return;
		}

	CSpaceObject *pTarget = m_Targets.GetRandomObj();

	SShipCreateCtx Ctx;
	Ctx.pSystem = &System;
	Ctx.pTarget = pTarget;
	Ctx.dwFlags = SShipCreateCtx::ATTACK_NEAREST_ENEMY;

	//	Figure out where the encounter will come from

	if (m_rDistance > 0.0)
		{
		if (pTarget)
			Ctx.vPos = CalcEncounterPos(pTarget, m_rDistance);
		Ctx.PosSpread = DiceRange(3, 1, 2);
		}
	else if (m_pGate && m_pGate->IsActiveStargate())
		Ctx.pGate = m_pGate;
	else if (m_pGate)
		{
		Ctx.vPos = m_pGate->GetPos();
		Ctx.PosSpread = DiceRange(2, 1, 2);
		}
	else if (!m_vPos.IsNull())
		{
		Ctx.vPos = m_vPos;
		Ctx.PosSpread = DiceRange(2, 1, 2);
		}
	else if (pTarget)
		Ctx.pGate = pTarget->GetNearestStargate(true);

	//	Create ships

	pTable->CreateShips(Ctx);

	//	Delete the event

	SetDestroyed();

	DEBUG_CATCH
	}

bool CTimedEncounterEvent::OnObjDestroyed (CSpaceObject *pObj)

//	OnObjDestroyed
//
//	Returns TRUE if the event should be destroyed

	{
	m_Targets.Delete(pObj);
	return (m_Targets.GetCount() == 0);
	}

bool CTimedEncounterEvent::OnStationDestroyed (CSpaceObject *pObj)

//	OnStationDestroyed
//
//	Returns TRUE if the event should be destroyed

	{
	//	If this is one of our targets, then we remove it.

	m_Targets.Delete(pObj);
	return (m_Targets.GetCount() == 0);
	}

void CTimedEncounterEvent::OnWriteToStream (CSystem *pSystem, IWriteStream *pStream)

//	OnWriteToStream
//
//	Writes the obj data to stream
//
//	DWORD				m_dwEncounterTableUNID
//	CSpaceObjectList	m_Targets
//	DWORD				m_pGate (CSpaceObject ref)
//	Metric				m_rDistance
//	CVector				m_vPos

	{
	pStream->Write(m_dwEncounterTableUNID);
	m_Targets.WriteToStream(pSystem, pStream);
	pSystem->WriteObjRefToStream(m_pGate, pStream);
	pStream->Write(m_rDistance);
	m_vPos.WriteToStream(*pStream);
	}

//	CTimedCustomEvent class --------------------------------------------------

CTimedCustomEvent::CTimedCustomEvent (int iTick,
									  CSpaceObject *pObj,
									  const CString &sEvent) :
		CSystemEvent(iTick),
		m_pObj(pObj),
		m_sEvent(sEvent)

//	CTimedCustomEvent constructor

	{
	}

CTimedCustomEvent::CTimedCustomEvent (SLoadCtx &Ctx) : CSystemEvent(Ctx)

//	CTimedCustomEvent constructor

	{
	CSystem::ReadObjRefFromStream(Ctx, (CSpaceObject **)&m_pObj);
	m_sEvent.ReadFromStream(Ctx.pStream);
	}

CString CTimedCustomEvent::DebugCrashInfo (void)

//	DebugCrashInfo

	{
	CString sResult;

	sResult.Append(CONSTLIT("CTimedCustomEvent\r\n"));
	sResult.Append(strPatternSubst(CONSTLIT("m_pObj: %s\r\n"), CSpaceObject::DebugDescribe(m_pObj)));
	sResult.Append(strPatternSubst(CONSTLIT("event: %s\r\n"), m_sEvent));
	return sResult;
	}

void CTimedCustomEvent::DoEvent (DWORD dwTick, CSystem &System)

//	DoEvent
//
//	Do the actual event

	{
	DEBUG_TRY

	if (m_pObj)
		m_pObj->FireCustomEvent(m_sEvent, eventDoEvent);

	SetDestroyed();

	DEBUG_CATCH
	}

bool CTimedCustomEvent::OnObjChangedSystems (CSpaceObject *pObj)

//	OnObjChangedSystems
//
//	Returns TRUE if the event should be carried into the new system

	{
	return (m_pObj == pObj);
	}

bool CTimedCustomEvent::OnObjDestroyed (CSpaceObject *pObj)

//	OnObjDestroyed
//
//	Returns TRUE if the event should be destroyed

	{
	return (m_pObj == pObj);
	}

void CTimedCustomEvent::OnWriteToStream (CSystem *pSystem, IWriteStream *pStream)

//	OnWriteToStream
//
//	Writes the obj data to stream
//
//	DWORD		m_pObj (CSpaceObject ref)
//	CString		m_sEvent

	{
	pSystem->WriteObjRefToStream(m_pObj, pStream);
	m_sEvent.WriteToStream(pStream);
	}

//	CTimedRecurringEvent class --------------------------------------------------

CTimedRecurringEvent::CTimedRecurringEvent (int iInterval,
											CSpaceObject *pObj,
											const CString &sEvent) :
		CSystemEvent(pObj->GetSystem()->GetTick() + mathRandom(0, iInterval)),
		m_iInterval(iInterval),
		m_pObj(pObj),
		m_sEvent(sEvent)

//	CTimedRecurringEvent constructor

	{
	}

CTimedRecurringEvent::CTimedRecurringEvent (SLoadCtx &Ctx) : CSystemEvent(Ctx)

//	CTimedRecurringEvent constructor

	{
	CSystem::ReadObjRefFromStream(Ctx, (CSpaceObject **)&m_pObj);
	m_sEvent.ReadFromStream(Ctx.pStream);
	Ctx.pStream->Read((char *)&m_iInterval, sizeof(DWORD));
	}

CString CTimedRecurringEvent::DebugCrashInfo (void)

//	DebugCrashInfo

	{
	CString sResult;

	sResult.Append(CONSTLIT("CTimedRecurringEvent\r\n"));
	sResult.Append(strPatternSubst(CONSTLIT("m_pObj: %s\r\n"), CSpaceObject::DebugDescribe(m_pObj)));
	sResult.Append(strPatternSubst(CONSTLIT("event: %s\r\n"), m_sEvent));
	return sResult;
	}

void CTimedRecurringEvent::DoEvent (DWORD dwTick, CSystem &System)

//	DoEvent
//
//	Do the actual event

	{
	DEBUG_TRY

	if (m_pObj)
		m_pObj->FireCustomEvent(m_sEvent, eventDoEvent);

	SetTick(dwTick + m_iInterval);

	DEBUG_CATCH
	}

bool CTimedRecurringEvent::OnObjChangedSystems (CSpaceObject *pObj)

//	OnObjChangedSystems
//
//	Returns TRUE if the event should be carried into the new system

	{
	return (m_pObj == pObj);
	}

bool CTimedRecurringEvent::OnObjDestroyed (CSpaceObject *pObj)

//	OnObjDestroyed
//
//	Returns TRUE if the event should be destroyed

	{
	return (m_pObj == pObj);
	}

void CTimedRecurringEvent::OnWriteToStream (CSystem *pSystem, IWriteStream *pStream)

//	OnWriteToStream
//
//	Writes the obj data to stream
//
//	DWORD		m_pObj (CSpaceObject ref)
//	CString		m_sEvent
//	DWORD		m_iInterval

	{
	pSystem->WriteObjRefToStream(m_pObj, pStream);
	m_sEvent.WriteToStream(pStream);
	pStream->Write((char *)&m_iInterval, sizeof(DWORD));
	}

//	CTimedTypeEvent ------------------------------------------------------------

CTimedTypeEvent::CTimedTypeEvent (int iTick,
								  int iInterval,
								  CDesignType *pType,
								  const CString &sEvent) :
		CSystemEvent(iTick),
		m_iInterval(iInterval),
		m_pType(pType),
		m_sEvent(sEvent)

//	CTimedTypeEvent constructor

	{
	}

CTimedTypeEvent::CTimedTypeEvent (SLoadCtx &Ctx) : CSystemEvent(Ctx)

//	CTimedTypeEvent constructor

	{
	DWORD dwLoad;
	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	m_pType = Ctx.GetUniverse().FindDesignType(dwLoad);

	m_sEvent.ReadFromStream(Ctx.pStream);
	Ctx.pStream->Read((char *)&m_iInterval, sizeof(DWORD));
	}

CString CTimedTypeEvent::DebugCrashInfo (void)

//	DebugCrashInfo
//
//	Dump crash info

	{
	CString sResult;

	sResult.Append(CONSTLIT("CTimedTypeEvent\r\n"));
	sResult.Append(strPatternSubst(CONSTLIT("m_pType: 0x%x\r\n"), (m_pType ? m_pType->GetUNID() : 0)));
	sResult.Append(strPatternSubst(CONSTLIT("event: %s\r\n"), m_sEvent));
	return sResult;
	}

void CTimedTypeEvent::DoEvent (DWORD dwTick, CSystem &System)

//	DoEvent
//
//	Run the event

	{
	DEBUG_TRY

	if (m_pType)
		m_pType->FireCustomEvent(m_sEvent, eventDoEvent);

	if (m_iInterval)
		SetTick(dwTick + m_iInterval);
	else
		SetDestroyed();

	DEBUG_CATCH
	}

void CTimedTypeEvent::OnWriteToStream (CSystem *pSystem, IWriteStream *pStream)

//	OnWriteToStream
//
//	Write to stream
//
//	DWORD		m_pType (UNID)
//	CString		m_sEvent
//	DWORD		m_iInterval

	{
	DWORD dwSave;

	dwSave = (m_pType ? m_pType->GetUNID() : 0);
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	m_sEvent.WriteToStream(pStream);
	pStream->Write((char *)&m_iInterval, sizeof(DWORD));
	}
