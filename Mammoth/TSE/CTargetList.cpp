//	CTargetList.cpp
//
//	CTargetList class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "TargetListImpl.h"

void CTargetList::AddTarget (const STargetPriority &Priority, ETargetType iType, CSpaceObject &Obj) const

//	AddTarget
//
//	Adds a target

	{
	STargetDesc *pEntry = m_Targets.Insert(Priority);
	pEntry->iType = iType;
	pEntry->pObj = &Obj;
	}

CTargetList::EPriorityClass CTargetList::CalcPriority (ETargetType iType, CSpaceObject &Obj)

//	CalcPriority
//
//	Calculates the priority based on type.

	{
	switch (iType)
		{
		case ETargetType::AttackerCompatible:
		case ETargetType::Missile:
		case ETargetType::TargetableMissile:
		case ETargetType::AggressiveShip:
		case ETargetType::Station:
			return prioritySecondary;

		case ETargetType::Fortification:
		case ETargetType::Minable:
		case ETargetType::NonAggressiveShip:
			return priorityTertiary;

		default:
			return priorityLowest;
		}
	}

CTargetList::ETargetType CTargetList::CalcType (const CSpaceObject &Obj)

//	CalcType
//
//	Calculates the type of the given object.

	{
	if (Obj.GetScale() == scaleStructure)
		return ETargetType::Station;
	else
		{
		int iAggressorThreshold = (Obj.GetUniverse().GetTicks() - CSpaceObject::AGGRESSOR_THRESHOLD);

		if (Obj.GetLastFireTime() > iAggressorThreshold)
			return ETargetType::AggressiveShip;
		else
			return ETargetType::NonAggressiveShip;
		}
	}

bool CTargetList::CanDetonate (const CSpaceObject &SourceObj, const CSpaceObject *pTarget, const STargetOptions &Options, const CSpaceObject &Obj)

//	CanDetonate
//
//	Returns TRUE if Obj can detonate a missile.

	{
	if (Options.bIncludeMinable && Obj.CanBeMined())
		{
		return (!Obj.IsExplored() || Obj.HasMinableItem())
				&& !Obj.IsUnreal()
				&& Obj != Options.pExcludeObj;
		}
	else if (Obj.GetScale() == scaleShip || (Options.bIncludeStations && Obj.GetScale() == scaleStructure))
		{
		return SourceObj.IsAngryAt(&Obj)	
				&& (Obj.CanBeAttacked() || Obj == pTarget);
		}
	else
		return false;
	}

void CTargetList::Delete (CSpaceObject &Obj)

//	Delete
//
//	Delete an entry with the given object.

	{
	if (Obj == m_pSourceObj)
		{
		CleanUp();
		}
	else
		{
		if (m_Options.pExcludeObj == Obj)
			m_Options.pExcludeObj = NULL;

		if (m_Options.pPerceptionObj == Obj)
			m_Options.pPerceptionObj = NULL;

		for (int i = 0; i < m_Targets.GetCount(); i++)
			{
			if (m_Targets[i].pObj == Obj)
				{
				m_Targets.Delete(i);
				i--;
				}
			}
		}
	}

CSpaceObject *CTargetList::FindBestTarget (DWORD dwTargetTypes, Metric rMaxRange) const

//	FindBestTarget
//
//	Returns the best target in the list that matches the type and range.

	{
	Realize();

	Metric rMaxRange2 = rMaxRange * rMaxRange;

	for (int i = 0; i < GetCount(); i++)
		{
		if (((DWORD)GetTargetType(i) & dwTargetTypes) == 0)
			continue;

		if (rMaxRange > 0.0 && GetTargetDist2(i) > rMaxRange2)
			continue;

		return GetTarget(i);
		}

	return NULL;
	}

void CTargetList::Init (CSpaceObject &SourceObj, const STargetOptions &Options)

//	Init
//
//	Initialize the target list with options. We do not necessarily search for 
//	targets until actually needed.

	{
	m_pSourceObj = &SourceObj;
	m_Options = Options;
	Invalidate();
	}

void CTargetList::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Reads from a stream.
//
//	DWORD		m_pSourceObj
//	DWORD		m_Options.iMaxTargets
//	Metric		m_Options.rMaxDist
//	DWORD		m_Options.pExcludeObj
//	DWORD		m_Options.pPerceptionObj
//	DWORD		flags
//
//	DWORD		count
//	For each entry:
//	DWORD		priority and type
//	Metric		rDist2
//	DWORD		objID

	{
	CSystem::ReadObjRefFromStream(Ctx, &m_pSourceObj);

	Ctx.pStream->Read(m_Options.iMaxTargets);
	Ctx.pStream->Read(m_Options.rMaxDist);
	CSystem::ReadObjRefFromStream(Ctx, &m_Options.pExcludeObj);
	CSystem::ReadObjRefFromStream(Ctx, &m_Options.pPerceptionObj);

	DWORD dwFlags;
	Ctx.pStream->Read(dwFlags);
	m_Options.bIncludeMinable =				((dwFlags & 0x00000001) ? true : false);
	m_Options.bIncludeMissiles =			((dwFlags & 0x00000002) ? true : false);
	m_Options.bIncludeNonAggressors =		((dwFlags & 0x00000004) ? true : false);
	m_Options.bIncludePlayer =				((dwFlags & 0x00000008) ? true : false);
	m_Options.bIncludeSourceTarget =		((dwFlags & 0x00000010) ? true : false);
	m_Options.bIncludeStations =			((dwFlags & 0x00000020) ? true : false);
	m_Options.bIncludeTargetableMissiles =	((dwFlags & 0x00000040) ? true : false);
	m_Options.bNoLineOfFireCheck =			((dwFlags & 0x00000080) ? true : false);
	m_Options.bNoRangeCheck =				((dwFlags & 0x00000100) ? true : false);

	DWORD dwCount;
	Ctx.pStream->Read(dwCount);
	if (dwCount == SPECIAL_INVALID_COUNT)
		{
		m_bValid = false;
		m_Targets.DeleteAll();
		}
	else
		{
		m_bValid = true;
		m_Targets.GrowToFit(dwCount);

		for (int i = 0; i < (int)dwCount; i++)
			{
			DWORD dwLoad;
			Ctx.pStream->Read(dwLoad);
			ETargetType iTargetType = (ETargetType)(dwLoad & 0xffff);
			EPriorityClass iPriority = (EPriorityClass)(dwLoad >> 16);

			Metric rDist2;
			Ctx.pStream->Read(rDist2);

			m_Targets.InsertSorted(STargetPriority(iPriority, rDist2), STargetDesc());
			m_Targets.GetValue(i).iType = iTargetType;
			CSystem::ReadObjRefFromStream(Ctx, &m_Targets.GetValue(i).pObj);
			}
		}
	}

void CTargetList::Realize (void) const

//	Realize
//
//	Initialize the list of targets given the options.

	{
	if (m_bValid)
		{
		//	Nothing to do
		}
	else if (!m_pSourceObj || m_pSourceObj->GetSystem() == NULL)
		{
		//	Valid, but no targets

		m_Targets.DeleteAll();
		m_bValid = true;
		}
	else
		{
		m_Targets.DeleteAll();

		CNearestInRadiusRange Range(m_pSourceObj->GetPos(), m_Options.rMaxDist);
		CTargetListSelector Selector(*m_pSourceObj, m_Options);

		const CSystem &System = *m_pSourceObj->GetSystem();
		const CSpaceObjectGrid &Grid = System.GetObjectGrid();

		CSpaceObject *pAddPlayer = (m_Options.bIncludePlayer ? System.GetPlayerShip() : NULL);
		CSpaceObject *pAddSourceTarget = (m_Options.bIncludeSourceTarget ? m_pSourceObj->GetTarget() : NULL);

		SSpaceObjectGridEnumerator i;
		Grid.EnumStart(i, Range.GetUR(), Range.GetLL(), gridNoBoxCheck);

		while (Grid.EnumHasMore(i))
			{
			CSpaceObject *pObj = Grid.EnumGetNextFast(i);

			Metric rDist2;
			ETargetType iTargetType;
			if (Selector.MatchesCategory(*pObj)
					&& Range.Matches(*pObj, &rDist2)
					&& Selector.Matches(*pObj, rDist2, &iTargetType))
				{
				EPriorityClass iPriority;

				//	See if this is one of our special objects and calculate
				//	priority.

				if (pObj == pAddPlayer)
					{
					iPriority = priorityPrimary;
					pAddPlayer = NULL;
					}
				else if (pObj == pAddSourceTarget)
					{
					iPriority = priorityPrimary;
					pAddSourceTarget = NULL;
					}
				else
					{
					iPriority = CalcPriority(iTargetType, *pObj);
					}

				//	Add the target

				AddTarget(STargetPriority(iPriority, rDist2), iTargetType, *pObj);
				}
			}

		//	If we haven't yet added required targets, do it now

		if (pAddPlayer)
			{
			Metric rDist2 = (pAddPlayer->GetPos() - m_pSourceObj->GetPos()).Length2();
			if (Selector.MatchesCanBeTargeted(*pAddPlayer, rDist2))
				{
				AddTarget(STargetPriority(priorityPrimary, rDist2), ETargetType::AggressiveShip, *pAddPlayer);
				}
			}

		if (pAddSourceTarget)
			{
			Metric rDist2 = (pAddSourceTarget->GetPos() - m_pSourceObj->GetPos()).Length2();
			if (Selector.MatchesCanBeTargeted(*pAddSourceTarget, rDist2))
				{
				AddTarget(STargetPriority(priorityPrimary, rDist2), CalcType(*pAddSourceTarget), *pAddSourceTarget);
				}
			}

		m_bValid = true;
		}
	}

void CTargetList::WriteToStream (IWriteStream &Stream) const

//	WriteToStream
//
//	Write to a stream.
//
//	DWORD		m_pSourceObj
//	DWORD		m_Options.iMaxTargets
//	Metric		m_Options.rMaxDist
//	DWORD		m_Options.pExcludeObj
//	DWORD		m_Options.pPerceptionObj
//	DWORD		flags
//
//	DWORD		count
//	For each entry:
//	DWORD		priority and type
//	Metric		rDist2
//	DWORD		objID

	{
	CSystem::WriteObjRefToStream(Stream, m_pSourceObj);

	Stream.Write(m_Options.iMaxTargets);
	Stream.Write(m_Options.rMaxDist);
	CSystem::WriteObjRefToStream(Stream, m_Options.pExcludeObj);
	CSystem::WriteObjRefToStream(Stream, m_Options.pPerceptionObj);

	DWORD dwFlags = 0;
	dwFlags |= (m_Options.bIncludeMinable				? 0x00000001 : 0);
	dwFlags |= (m_Options.bIncludeMissiles				? 0x00000002 : 0);
	dwFlags |= (m_Options.bIncludeNonAggressors			? 0x00000004 : 0);
	dwFlags |= (m_Options.bIncludePlayer				? 0x00000008 : 0);
	dwFlags |= (m_Options.bIncludeSourceTarget			? 0x00000010 : 0);
	dwFlags |= (m_Options.bIncludeStations				? 0x00000020 : 0);
	dwFlags |= (m_Options.bIncludeTargetableMissiles	? 0x00000040 : 0);
	dwFlags |= (m_Options.bNoLineOfFireCheck			? 0x00000080 : 0);
	dwFlags |= (m_Options.bNoRangeCheck					? 0x00000100 : 0);
	Stream.Write(dwFlags);

	if (m_bValid)
		{
		Stream.Write(m_Targets.GetCount());

		for (int i = 0; i < m_Targets.GetCount(); i++)
			{
			DWORD dwSave = ((DWORD)m_Targets.GetKey(i).iType << 16) | ((DWORD)m_Targets[i].iType & 0xffff);
			Stream.Write(dwSave);

			Stream.Write(m_Targets.GetKey(i).rDist2);

			CSystem::WriteObjRefToStream(Stream, m_Targets[i].pObj);
			}
		}
	else
		Stream.Write(SPECIAL_INVALID_COUNT);
	}
