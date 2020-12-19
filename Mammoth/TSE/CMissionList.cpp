//	CMissionList.cpp
//
//	CMissionList class
//	Copyright (c) 2012 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

bool CMissionList::CanCreateMissionInArc (const CMissionType &NewMissionType, const CSpaceObject *pSource, const CMissionCriteria &CreateCriteria) const

//	CreateCreateMissionInArc
//
//	Returns TRUE if we can create a mission in the given arc at the given 
//	sequence.

	{
	const CString &sArc = NewMissionType.GetArc();
	int iSequence = NewMissionType.GetArcSequence();

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

		//	If this mission is not in the same arc, then it doesn't affect us.

		if (!strEquals(sArc, MissionType.GetArc()))
			continue;

		//	If we've got a create criteria and this is one of the missions in
		//	our criteria, then we always allow it. For example, if a mission in
		//	a mission arc requires the previous mission to be active, then we
		//	allow it, even if normally we would not allow another mission in the
		//	same arc to be active.

		else if (!NewMissionType.GetCreateCriteria().IsBlank()
				&& Mission.MatchesCriteria(pSource, CreateCriteria))
			continue;

		//	If we have an open (but not accepted) mission, then we continue.
		//	Later, if we accept the new mission, we can close out the old one.

		else if (Mission.IsOpen())
			continue;

		//	If we've got an active mission, then we can't create a new mission 
		//	in the same arc.

		else if (Mission.IsActive() && !MissionType.AllowsOtherArcMissions())
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

void CMissionList::CloseMissionsInArc (const CMissionType &NewMissionType)

//	CloseMissionsInArc
//
//	If there are any missions in the same arc as NewMissionType and if they are
//	open (but not accepted) and if they have an older sequence, then close them
//	out.

	{
	const CString &sArc = NewMissionType.GetArc();
	int iSequence = NewMissionType.GetArcSequence();

	//	If we don't have a sequence number, then nothing to do.

	if (iSequence < 0 || sArc.IsBlank())
		return;

	//	Check to see if we can create the given mission by looking at the 
	//	current and closed missions.

	for (int i = 0; i < m_List.GetCount(); i++)
		{
		CMission &Mission = *m_List[i];
		const CMissionType &MissionType = *CMissionType::AsType(Mission.GetType());

		//	If this mission is in the same arc and if it is open, and if it has
		//	an earlier sequence number, then we start it without the player.

		if (Mission.IsOpen() 
				&& strEquals(sArc, MissionType.GetArc())
				&& MissionType.GetArcSequence() < iSequence)
			Mission.SetUnavailable();
		}
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

CMissionList CMissionList::Filter (const CSpaceObject *pSource, const CMissionCriteria &Criteria) const

//	Filter
//
//	Filters the list.

	{
	CMissionList Result;

	for (int i = 0; i < GetCount(); i++)
		{
		CMission *pMission = GetMission(i);
		if (pMission->MatchesCriteria(pSource, Criteria) 
				&& !pMission->IsDestroyed())
			Result.m_List.Insert(pMission);
		}

	//	If we only want mission arcs, filter out the list

	if (Criteria.MatchesOnlyMissionArcs())
		return Result.FilterByArc();

	//	Otherwise, return the full results.

	else
		return Result;
	}

CMissionList CMissionList::FilterByArc (void) const

//	FilterByArc
//
//	Filters down to one mission (the latest) per mission arc.

	{
	TSortMap<CString, CMission *> ByArc;

	for (int i = 0; i < GetCount(); i++)
		{
		CMission *pMission = GetMission(i);
		int iSequence;
		const CString &sArc = pMission->GetArc(&iSequence);
		if (sArc.IsBlank())
			continue;

		//	Add the mission only if it is the latest in the arc.

		bool bNew;
		auto ppDest = ByArc.SetAt(sArc, &bNew);
		if (bNew || (*ppDest)->GetArcSequence() < iSequence)
			*ppDest = pMission;
		}

	CMissionList Result;
	Result.m_List.GrowToFit(ByArc.GetCount());

	for (int i = 0; i < ByArc.GetCount(); i++)
		Result.Insert(ByArc[i]);

	return Result;
	}

CMissionList CMissionList::FilterByType (CMissionType &Type) const

//	FilterByType
//
//	Returns a list of all missions of the given type.

	{
	CMissionList Result;

	for (int i = 0; i < GetCount(); i++)
		{
		CMission *pMission = GetMission(i);
		if (pMission->GetType() == &Type)
			Result.Insert(pMission);
		}

	return Result;
	}

CMission *CMissionList::FindAcceptedArcChapter (const CString &sTargetArc, const CString &sTargetTitle, CMission *pExclude) const

//	FindAcceptedArcChapter
//
//	Finds an accepted mission with the same arc and title.

	{
	for (int i = 0; i < GetCount(); i++)
		{
		CMission *pMission = GetMission(i);

		if (pMission != pExclude
				&& strEquals(sTargetArc, pMission->GetArc())
				&& strEquals(sTargetTitle, pMission->GetTitle())
				&& pMission->IsPlayerMission())
			return pMission;
		}

	return NULL;
	}

CMission *CMissionList::FindByArc (const CString &sTargetArc) const

//	FindByArc
//
//	Returns the latest mission in the given arc.

	{
	CMission *pLatest = NULL;

	for (int i = 0; i < GetCount(); i++)
		{
		CMission *pMission = GetMission(i);
		int iSequence;
		const CString &sArc = pMission->GetArc(&iSequence);
		if (!strEquals(sTargetArc, sArc))
			continue;

		//	Add the mission only if it is the latest in the arc.

		if (pLatest == NULL || pLatest->GetArcSequence() < iSequence)
			pLatest = pMission;
		}

	return pLatest;
	}

CMission *CMissionList::FindHighestPriority (void) const

//	FindHighestPriority
//
//	Returns the mission with the highest priority.

	{
	CMission *pBestMission = NULL;
	for (int i = 0; i < GetCount(); i++)
		if (pBestMission == NULL || GetMission(i)->GetPriority() > pBestMission->GetPriority())
			pBestMission = GetMission(i);
				
	return pBestMission;
	}

CMission *CMissionList::FindLatestActivePlayer (void) const

//	FindLatestActivePlayer
//
//	Returns the latest active player mission, or NULL if none is found.

	{
	CMission *pCurrentMission = NULL;
	for (int i = 0; i < GetCount(); i++)
		{
		CMission *pMission = GetMission(i);
		if (!pMission->IsActive() || !pMission->IsPlayerMission())
			continue;

		if (pCurrentMission == NULL 
				|| pMission->GetAcceptedOn() > pCurrentMission->GetAcceptedOn())
			pCurrentMission = pMission;
		}

	return pCurrentMission;
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

bool CMissionList::Matches (const CSpaceObject *pSource, const CMissionCriteria &Criteria) const

//	Matches
//
//	Returns TRUE if at least one mission on the list matches the criteria.

	{
	for (int i = 0; i < GetCount(); i++)
		{
		CMission *pMission = GetMission(i);
		if (pMission->MatchesCriteria(pSource, Criteria) 
				&& !pMission->IsDestroyed())
			return true;
		}

	return false;
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
	DeleteAll();

	DWORD dwCount;
	Ctx.pStream->Read((char *)&dwCount, sizeof(DWORD));
	m_List.InsertEmpty(dwCount);

	for (int i = 0; i < (int)dwCount; i++)
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

		//	If these are the same arc, then we order by sequence.
		//	
		//	NOTE: Callers must guarantee that all missions in the same arc have
		//	the same priority and the same shuffle value. Otherwise, the sort 
		//	order will be inconsistent (because ordering will not be 
		//	transitive).

		if (!pLeft->GetArc().IsBlank() && strEquals(pLeft->GetArc(), pRight->GetArc()))
			{
			if (pLeft->GetArcSequence() > pRight->GetArcSequence())
				return 1;
			else if (pLeft->GetArcSequence() < pRight->GetArcSequence())
				return -1;
			}

		//	Next, priority takes precedence
		
		if (pLeft->GetPriority() > pRight->GetPriority())
			return -1;
		else if (pLeft->GetPriority() < pRight->GetPriority())
			return 1;

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
