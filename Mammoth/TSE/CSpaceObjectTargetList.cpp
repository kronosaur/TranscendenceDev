//	CSpaceObjectTargetList.cpp
//
//	CSpaceObjectTargetList class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CSpaceObjectTargetList::Delete (const CSpaceObject &Obj)

//	Delete
//
//	Remove the object from the list.

	{
	for (int i = 0; i < m_List.GetCount(); i++)
		{
		if (m_List[i] == Obj)
			{
			m_List.Delete(i);
			i--;
			}
		}
	}

bool CSpaceObjectTargetList::FindTargetAligned (CSpaceObject &SourceObj, const CDeviceItem &WeaponItem, CSpaceObject **retpTarget, int *retiFireAngle) const

//	FindTargetAligned
//
//	Looks for the first target that we can hit.

	{
	for (int i = 0; i < m_List.GetCount(); i++)
		{
		int iFireAngle;
		CSpaceObject *pTarget = m_List[i];

		if (WeaponItem.IsWeaponAligned(pTarget, NULL, &iFireAngle))
			{
			if (retpTarget) *retpTarget = pTarget;
			if (retiFireAngle) *retiFireAngle = iFireAngle;
			return true;
			}
		}

	return false;
	}

bool CSpaceObjectTargetList::FindTargetInRange (const CSpaceObject &SourceObj, const CDeviceItem &WeaponItem, DWORD dwFlags, CSpaceObject **retpTarget, int *retiFireAngle, Metric *retrDist2) const

//	FindTargetInRange
//
//	Looks for the first target appropriate for the given weapon. We assume 
//	m_List has been initialized and is in sorted order.

	{
	bool bCheckLineOfFire = ((dwFlags & FLAG_NO_LINE_OF_FIRE_CHECK) ? false : true);
	bool bCheckRange = ((dwFlags & FLAG_NO_RANGE_CHECK) ? false : true);

	const CInstalledDevice &Device = *WeaponItem.GetInstalledDevice();

	Metric rMaxRange = WeaponItem.GetMaxEffectiveRange();
	Metric rMaxRange2 = rMaxRange * rMaxRange;
	for (int i = 0; i < m_List.GetCount(); i++)
		{
		int iFireAngle;
		CSpaceObject *pTarget = m_List[i];
		Metric rDist2 = (pTarget->GetPos() - SourceObj.GetPos()).Length2();

		if ((!bCheckRange || rDist2 < rMaxRange2)
				&& WeaponItem.GetWeaponEffectiveness(pTarget) >= 0
				&& WeaponItem.IsWeaponAligned(pTarget, NULL, &iFireAngle)
				&& (!bCheckLineOfFire || SourceObj.IsLineOfFireClear(&Device, pTarget, iFireAngle, rMaxRange)))
			{
			if (retpTarget) *retpTarget = pTarget;
			if (retiFireAngle) *retiFireAngle = iFireAngle;
			if (retrDist2) *retrDist2 = rDist2;

			return true;
			}
		}

	return false;
	}

void CSpaceObjectTargetList::InitWithNearestMissiles (CSpaceObject &SourceObj, int iMaxTargets, Metric rMaxDist, DWORD dwFlags)

//	InitWithNearestMissiles
//
//	Generates a list of missiles that are targets for missile defense, sorted by distance.

	{
	if (m_bValid)
		return;

	CSystem *pSystem = SourceObj.GetSystem();
	if (pSystem == NULL)
		return;

	m_List.DeleteAll();

	CNearestInRadiusRange Range(SourceObj.GetPos(), rMaxDist);
	CVisibleMissileObjSelector Selector(SourceObj);

	CSpaceObjectEnum::FindNearestEnemyObjs(*pSystem, SourceObj, Range, Selector, m_List, iMaxTargets);

	m_bValid = true;
	}

void CSpaceObjectTargetList::InitWithNearestTargetableMissiles (CSpaceObject &SourceObj, int iMaxTargets, Metric rMaxDist, DWORD dwFlags)

//	Generates a list of targetable missiles, sorted by distance.

	{
	if (m_bValid)
		return;

	CSystem *pSystem = SourceObj.GetSystem();
	if (pSystem == NULL)
		return;

	m_List.DeleteAll();

	CNearestInRadiusRange Range(SourceObj.GetPos(), rMaxDist);
	CVisibleTargetableMissileObjSelector Selector(SourceObj);

	CSpaceObjectEnum::FindNearestEnemyObjs(*pSystem, SourceObj, Range, Selector, m_List, iMaxTargets);

	m_bValid = true;
	}

void CSpaceObjectTargetList::InitWithNearestVisibleEnemies (CSpaceObject &SourceObj, int iMaxTargets, Metric rMaxDist, CSpaceObject *pExcludeObj, DWORD dwFlags)

//	InitWithNearestVisibleEnemies
//
//	Generates a list of enemy targets, sorted by distance.

	{
	if (m_bValid)
		return;

	CSystem *pSystem = SourceObj.GetSystem();
	if (pSystem == NULL || iMaxTargets <= 0 || rMaxDist <= 0.0)
		{
		InitEmpty();
		return;
		}

	m_List.DeleteAll();

	//	If we want the source target, we always add it first

	if (dwFlags & FLAG_INCLUDE_SOURCE_TARGET)
		{
		CSpaceObject *pTarget = SourceObj.GetTarget(IShipController::FLAG_NO_AUTO_TARGET);
		if (pTarget)
			m_List.Insert(pTarget);
		}

	//	Add objects in range

	CNearestInRadiusRange Range(SourceObj.GetPos(), rMaxDist);

	if (dwFlags & FLAG_INCLUDE_NON_AGGRESSORS)
		{
		CVisibleObjSelector Selector(SourceObj);
		Selector.SetExcludeObj(pExcludeObj);

		if (dwFlags & FLAG_INCLUDE_STATIONS)
			Selector.SetIncludeStations();

		if (dwFlags & FLAG_INCLUDE_PLAYER)
			Selector.SetIncludePlayer();

		CSpaceObjectEnum::FindNearestEnemyObjs(*pSystem, SourceObj, Range, Selector, m_List, iMaxTargets);
		}
	else
		{
		CVisibleAggressorObjSelector Selector(SourceObj);
		Selector.SetExcludeObj(pExcludeObj);

		if (dwFlags & FLAG_INCLUDE_STATIONS)
			Selector.SetIncludeStations();

		if (dwFlags & FLAG_INCLUDE_PLAYER)
			Selector.SetIncludePlayer();

		CSpaceObjectEnum::FindNearestEnemyObjs(*pSystem, SourceObj, Range, Selector, m_List, iMaxTargets);
		}

	m_bValid = true;
	}

void CSpaceObjectTargetList::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Reads from the given stream.

	{
	DWORD dwCount;
	Ctx.pStream->Read(dwCount);
	if (dwCount == SPECIAL_INVALID_COUNT)
		{
		m_bValid = false;
		}
	else
		{
		m_List.InsertEmpty(dwCount);

		for (int i = 0; i < (int)dwCount; i++)
			{
			CSystem::ReadObjRefFromStream(Ctx, &m_List[i]);
			}

		m_bValid = true;
		}
	}

void CSpaceObjectTargetList::WriteToStream (CSystem &System, IWriteStream &Stream) const

//	WriteToStream
//
//	Writes out to a stream.

	{
	if (!m_bValid)
		Stream.Write(SPECIAL_INVALID_COUNT);
	else
		{
		DWORD dwSave = m_List.GetCount();
		Stream.Write(dwSave);

		for (int i = 0; i < m_List.GetCount(); i++)
			System.WriteObjRefToStream(m_List[i], &Stream);
		}
	}
