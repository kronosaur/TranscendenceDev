//	CRangeTypeEvent.cpp
//
//	CRangeTypeEvent class
//	Copyright (c) 2017 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define FIELD_OBJ							CONSTLIT("obj")

const int CHECK_INTERVAL =					3;

CRangeTypeEvent::CRangeTypeEvent (CDesignType *pType, const CString &sEvent, CSpaceObject *pCenter, Metric rRadius, const CString &sCriteria) : CSystemEvent(0),
		m_pType(pType),
		m_sEvent(sEvent),
		m_pCenter(pCenter),
		m_rRadius(rRadius),
		m_sCriteria(sCriteria),
		m_rRadius2(rRadius * rRadius),
		m_bCriteriaInit(false)

//	CRangeTypeEvent constructor

	{
	}

CRangeTypeEvent::CRangeTypeEvent (CDesignType *pType, const CString &sEvent, const CVector &vCenter, Metric rRadius, const CString &sCriteria) : CSystemEvent(0),
		m_pType(pType),
		m_sEvent(sEvent),
		m_pCenter(NULL),
		m_vCenter(vCenter),
		m_rRadius(rRadius),
		m_sCriteria(sCriteria),
		m_rRadius2(rRadius * rRadius),
		m_bCriteriaInit(false)

//	CRangeTypeEvent constructor

	{
	}

CRangeTypeEvent::CRangeTypeEvent (SLoadCtx &Ctx) : CSystemEvent(Ctx)

//	CRangeTypeEvent constructor

	{
	DWORD dwLoad;
	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	m_pType = Ctx.GetUniverse().FindDesignType(dwLoad);

	m_sEvent.ReadFromStream(Ctx.pStream);

	CSystem::ReadObjRefFromStream(Ctx, &m_pCenter);
	Ctx.pStream->Read((char *)&m_vCenter, sizeof(CVector));
	Ctx.pStream->Read((char *)&m_rRadius, sizeof(Metric));
	m_sCriteria.ReadFromStream(Ctx.pStream);

	m_rRadius2 = m_rRadius * m_rRadius;

	//	NOTE: We cannot parse the criteria yet because we may not have loaded
	//	the object yet. We parse it later.

	m_bCriteriaInit = false;
	}

CString CRangeTypeEvent::DebugCrashInfo (void)

//	DebugCrashInfo
//
//	Output debug info

	{
	CString sResult;

	sResult.Append(CONSTLIT("CRangeTypeEvent\r\n"));
	return sResult;
	}

void CRangeTypeEvent::DoEvent (DWORD dwTick, CSystem &System)

//	DoEvent
//
//	Called on tick expiration

	{
	CSpaceObject *pFound = NULL;
	CVector vCenter = (m_pCenter ? m_pCenter->GetPos() : m_vCenter);

	//	If we have a criteria, then we need to look for all objects that might
	//	match.

	if (!m_sCriteria.IsBlank())
		{
		//	Parse the criteria if we haven't already

		if (!m_bCriteriaInit)
			{
			m_Criteria.Init(m_pCenter, m_sCriteria);
			m_bCriteriaInit = true;
			}

		CCriteriaObjSelector Selector(m_Criteria);
		CNearestInRadiusRange Range(vCenter, m_rRadius);

		pFound = CSpaceObjectEnum::FindObjInRange(System, Range, Selector);
		}

	//	Otherwise, all we care about is the player ship.

	else
		{
		CSpaceObject *pPlayerShip = System.GetPlayerShip();
		if (pPlayerShip)
			{
			Metric rDist2 = (pPlayerShip->GetPos() - vCenter).Length2();
			if (rDist2 < m_rRadius2)
				pFound = pPlayerShip;
			}
		}

	//	If we did not find an object, then we try again in a little while

	if (pFound == NULL)
		{
		SetTick(dwTick + CHECK_INTERVAL);
		return;
		}

	//	Otherwise, we fire the event and destroy ourselves

	if (m_pType)
		{
		CCodeChain &CC = g_pUniverse->GetCC();
		ICCItem *pData = CC.CreateSymbolTable();
		pData->SetIntegerAt(FIELD_OBJ, (int)pFound);

		m_pType->FireCustomEvent(m_sEvent, eventNone, pData);

		pData->Discard();
		}

	SetDestroyed();
	}

bool CRangeTypeEvent::OnObjDestroyed (CSpaceObject *pObj)

//	OnObjDestroyed
//
//	Handle object being destroyed.

	{
	if (pObj == m_pCenter)
		{
		m_pCenter = NULL;

		//	If our center object is destroyed, then we cancel the event

		return true;
		}

	return false;
	}

void CRangeTypeEvent::OnWriteToStream (CSystem *pSystem, IWriteStream *pStream)

//	OnWriteToStream
//
//	DWORD			m_pType
//	CString			m_sEvent
//	CSpaceObject *	m_pCenter
//	CVector			m_vCenter
//	Metric			m_rRadius
//	CString			m_sCriteria

	{
	DWORD dwSave;

	dwSave = (m_pType ? m_pType->GetUNID() : 0);
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	m_sEvent.WriteToStream(pStream);

	pSystem->WriteObjRefToStream(m_pCenter, pStream);
	pStream->Write((char *)&m_vCenter, sizeof(CVector));
	pStream->Write((char *)&m_rRadius, sizeof(Metric));
	m_sCriteria.WriteToStream(pStream);
	}
