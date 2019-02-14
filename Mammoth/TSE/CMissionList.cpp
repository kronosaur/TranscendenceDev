//	CMissionList.cpp
//
//	CMissionList class
//	Copyright (c) 2012 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CMissionList::Delete (int iIndex)

//	Delete
//
//	Delete the given mission

	{
	if (m_bFree)
		delete m_List[iIndex];

	m_List.Delete(iIndex);
	}

void CMissionList::Delete (CMission *pMission)

//	Delete
//
//	Delete the given mission

	{
	int iIndex;
	if (m_List.Find(pMission, &iIndex))
		Delete(iIndex);
	}

void CMissionList::DeleteAll (void)

//	DeleteAll
//
//	Delete all missions

	{
	int i;

	if (m_bFree)
		{
		for (i = 0; i < m_List.GetCount(); i++)
			delete m_List[i];
		}

	m_List.DeleteAll();
	}

void CMissionList::FireOnSystemStarted (DWORD dwElapsedTime)

//	FireOnSystemStarted
//
//	Invoke <OnSystemStarted> event for all active missions.

	{
	for (int i = 0; i < GetCount(); i++)
		{
		CMission *pMission = GetMission(i);
		if (!pMission->IsClosed())
			pMission->FireOnSystemStarted(dwElapsedTime);
		}
	}

void CMissionList::FireOnSystemStopped (void)

//	FireOnSystemStopped
//
//	Invoke <OnSystemStopped> event for all active missions.

	{
	for (int i = 0; i < GetCount(); i++)
		{
		CMission *pMission = GetMission(i);
		if (!pMission->IsClosed())
			pMission->FireOnSystemStopped();
		}
	}

CMission *CMissionList::GetMissionByID (DWORD dwID) const

//	GetMissionByID
//
//	Returns a mission of the given ID (or NULL if not found)

	{
	int i;

	for (i = 0; i < m_List.GetCount(); i++)
		if (m_List[i]->GetID() == dwID
				&& !m_List[i]->IsDestroyed())
			return m_List[i];

	return NULL;
	}

void CMissionList::Insert (CMission *pMission)

//	Insert
//
//	Insert a new mission

	{
	m_List.Insert(pMission);
	}

void CMissionList::NotifyOnNewSystem (CSystem *pSystem)

//	NotifyOnNewSystem
//
//	Call OnNewSystem for all missions.

	{
	for (int i = 0; i < GetCount(); i++)
		{
		CMission *pMission = GetMission(i);

		if (!pMission->IsDestroyed())
			pMission->OnNewSystem(pSystem);
		}
	}

void CMissionList::NotifyOnPlayerEnteredSystem (CSpaceObject *pPlayerShip)

//	NotifyOnPlayerEnteredSystem
//
//	Call OnPlayerEnteredSystem for all active missions.

	{
	for (int i = 0; i < GetCount(); i++)
		{
		CMission *pMission = GetMission(i);
		if (!pMission->IsClosed())
			pMission->OnPlayerEnteredSystem(pPlayerShip);
		}
	}

ALERROR CMissionList::ReadFromStream (SLoadCtx &Ctx, CString *retsError)

//	ReadFromStream
//
//	Reads from stream
//
//	DWORD		Count
//	CMission

	{
	int i;

	DeleteAll();

	DWORD dwCount;
	Ctx.pStream->Read((char *)&dwCount, sizeof(DWORD));
	m_List.InsertEmpty(dwCount);

	for (i = 0; i < (int)dwCount; i++)
		{
		CSpaceObject *pObj;
		try
			{
			CSpaceObject::CreateFromStream(Ctx, &pObj);
			}
		catch (...)
			{
			*retsError = CSpaceObject::DebugLoadError(Ctx);
			return ERR_FAIL;
			}

		//	Add to global missions

		m_List[i] = pObj->AsMission();
		}

	return NOERROR;
	}

ALERROR CMissionList::WriteToStream (IWriteStream *pStream, CString *retsError)

//	WriteToStream
//
//	Writes to a stream
//
//	DWORD		Count
//	CMission

	{
	int i;

	//	Make a list of all non-destroyed missions

	DWORD dwCount = 0;
	for (i = 0; i < m_List.GetCount(); i++)
		if (!m_List[i]->IsDestroyed())
			dwCount++;

	pStream->Write((char *)&dwCount, sizeof(DWORD));

	//	Save all non-destroyed missions

	for (i = 0; i < m_List.GetCount(); i++)
		{
		try
			{
			if (!m_List[i]->IsDestroyed())
				m_List[i]->WriteToStream(pStream);
			}
		catch (...)
			{
			CString sError = CONSTLIT("Unable to save mission:\r\n");
			ReportCrashObj(&sError, m_List[i]);
			kernelDebugLogString(sError);
			return ERR_FAIL;
			}
		}

	return NOERROR;
	}
