//	CMissionList.cpp
//
//	CMissionList class
//	Copyright (c) 2012 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

bool CMissionList::CanCreateMissionInArc (const CString &sArc, int iSequence) const

//	CreateCreateMissionInArc
//
//	Returns TRUE if we can create a mission in the given arc at the given 
//	sequence.

	{
	//	If we don't have a sequence number, then we can always create the 
	//	mission.

	if (iSequence < 0 || sArc.IsBlank())
		return true;

	//	Check to see if we can create the given mission by looking at the 
	//	current and closed missions.

	for (int i = 0; i < m_List.GetCount(); i++)
		{
		const CMission &Mission = *m_List[i];
		const CMissionType &MissionType = *CMissionType::AsType(Mission.GetType());
		if (!strEquals(sArc, MissionType.GetArc()))
			continue;

		//	If we've got an open or active mission, then we can't create a new
		//	mission in the same arc.

		if (Mission.IsActive() || Mission.IsOpen())
			return false;

		//	If we've got a completed mission with a higher sequence number, then
		//	we cannot create this mission.
		//
		//	Even if we've got a mission with the same sequence number, we skip,
		//	because this is how we handle mutually-exclusive missions in a 
		//	single arc.

		else if (MissionType.GetArcSequence() >= iSequence)
			return false;
		}

	//	If we get this far then we can create a mission.

	return true;
	}

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
	ASSERT(pMission);
	if (pMission == NULL)
		return;

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

void CMissionList::Sort (TArray<CMissionType *> &Missions)

//	Sort
//
//	Sort the list of missions by priority order.

	{
	//	Generate a random number for each mission so that we can do a random
	//	order.

	for (int i = 0; i < Missions.GetCount(); i++)
		Missions[i]->SetShuffle();

	//	Sort

	Missions.Sort([](auto pLeft, auto pRight) {

		constexpr DWORD TIME_BUCKET = 18000;	//	10 minutes real-time
		constexpr DWORD MAX_TIME_BUCKET = 6;

		//	Priority takes precedence, even for missions in the same arc.
		
		if (pLeft->GetPriority() > pRight->GetPriority())
			return -1;
		else if (pLeft->GetPriority() < pRight->GetPriority())
			return 1;

		//	If these are the same arc, then we order by sequence.

		if (strEquals(pLeft->GetArc(), pRight->GetArc()))
			{
			if (pLeft->GetArcSequence() > pRight->GetArcSequence())
				return 1;
			else if (pLeft->GetArcSequence() < pRight->GetArcSequence())
				return -1;
			}

		//	If these are different arcs, or if they are the same arc with the
		//	same sequence, then we prefer missions that have not been accepted
		//	recently.
		//
		//	We map the last time that we accepted a mission into one of N 
		//	buckets (each 10 minutes long). Missions accepted in an earlier
		//	bucket get priority over ones accepted in a later bucket.

		DWORD dwLeftAccepted = Min((pLeft->GetLastAcceptedOn() > 0 ? (pLeft->GetLastAcceptedOn() / TIME_BUCKET) + 1 : 0), MAX_TIME_BUCKET);
		DWORD dwRightAccepted = Min((pRight->GetLastAcceptedOn() > 0 ? (pRight->GetLastAcceptedOn() / TIME_BUCKET) + 1 : 0), MAX_TIME_BUCKET);

		if (dwLeftAccepted > dwRightAccepted)
			return 1;
		else if (dwLeftAccepted < dwRightAccepted)
			return -1;

		//	Lastly, we shuffle.

		if (pLeft->GetShuffle() > pRight->GetShuffle())
			return 1;
		else if (pLeft->GetShuffle() < pRight->GetShuffle())
			return -1;
		else
			return 0;
		});
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
