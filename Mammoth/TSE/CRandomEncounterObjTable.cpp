//	CRandomEncounterObjTable.cpp
//
//	CRandomEncounterObjTable class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CRandomEncounterDesc CRandomEncounterObjTable::CalcEncounter (CSystem &System, CSpaceObject &PlayerObj) const

//	CalcEncounter
//
//	Picks a random encounter and returns it.

	{
	//	Create a probability table. This is dynamic because encounter 
	//	probability depends on player position.

	TProbabilityTable<const SEntry *> Table = CalcEncounterTable(System, PlayerObj);
	if (Table.IsEmpty())
		return CRandomEncounterDesc();

	//	Roll and return value

	int iPos = Table.RollPos();
	const SEntry *pResult = Table[iPos];

	if (pResult->pTable)
		return CRandomEncounterDesc(*pResult->pTable, *pResult->pBase);
	else
		return CRandomEncounterDesc(*pResult->pBase);
	}

TProbabilityTable<const CRandomEncounterObjTable::SEntry *> CRandomEncounterObjTable::CalcEncounterTable (CSystem &System, CSpaceObject &PlayerObj) const

//	CalcEncounterTable
//
//	Calculates encounter table.

	{
	RealizeAllObjs(System);

	//	Create a probability table. This is dynamic because encounter 
	//	probability depends on player position.

	TProbabilityTable<const SEntry *> Table;
	Table.GrowToFit(m_AllObjs.GetCount());
	for (int i = 0; i < m_AllObjs.GetCount(); i++)
		{
		const SEntry &Entry = m_AllObjs[i];

		//	Adjust frequency to account for the player's distance from the object

		Metric rDist = Max(LIGHT_MINUTE, PlayerObj.GetDistance(Entry.pBase));
		Metric rDistAdj = (rDist <= MAX_ENCOUNTER_DIST ? LIGHT_MINUTE / rDist : 0.0);
		int iWeight = (int)(Entry.iFreq * 10.0 * rDistAdj);

		//	Add

		if (iWeight > 0)
			Table.Insert(&Entry, iWeight);
		}

	return Table;
	}

void CRandomEncounterObjTable::RealizeAllObjs (CSystem &System) const

//	RealizeAllObjs
//
//	Loop through all objects in the system and get their encounter tables. Cache
//	it so we don't have to compute it every time.

	{
	if (m_bValid)
		return;

	m_AllObjs.DeleteAll();
	for (int i = 0; i < System.GetObjectCount(); i++)
		{
		CSpaceObject *pObj = System.GetObject(i);

		if (pObj 
				&& pObj->HasRandomEncounters())
			{
			SEntry &Entry = *m_AllObjs.Insert();
			Entry.pBase = pObj;

			//	NOTE: If is OK if this returns NULL for a table.

			Entry.pTable = pObj->GetRandomEncounterTable(&Entry.iFreq);
			}
		}

	m_bValid = true;
	}
