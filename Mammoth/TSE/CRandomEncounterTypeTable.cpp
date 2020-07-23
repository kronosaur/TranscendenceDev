//	CRandomEncounterTypeTable.cpp
//
//	CRandomEncounterTypeTable class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

ICCItemPtr CRandomEncounterTypeTable::AsCCItem (const CSystem &System) const

//	AsCCItem
//
//	Output the table as an item (generally for debugging purposes).

	{
	Realize(System);
	if (m_Table.IsEmpty())
		return ICCItemPtr::Nil();

	ICCItemPtr pResult(ICCItem::List);
	for (int i = 0; i < m_Table.GetCount(); i++)
		{
		ICCItemPtr pEntry(ICCItem::SymbolTable);
		pEntry->SetIntegerAt(CONSTLIT("type"), m_Table[i].pType->GetUNID());
		pEntry->SetIntegerAt(CONSTLIT("frequency"), m_Table.GetChance(i));

		pResult->Append(pEntry);
		}

	return pResult;
	}

CRandomEncounterDesc CRandomEncounterTypeTable::CalcEncounter (const CSystem &System, CSpaceObject &PlayerObj) const

//	CalcEncounter
//
//	Picks a random encounter.

	{
	Realize(System);

	if (m_Table.IsEmpty())
		return CRandomEncounterDesc();

	//	Roll and return value

	int iPos = m_Table.RollPos();
	const SEntry &Result = m_Table[iPos];

	if (Result.pTable)
		return CRandomEncounterDesc(*Result.pTable, *Result.pType, Result.pBaseSovereign);
	else
		return CRandomEncounterDesc(*Result.pType, Result.pBaseSovereign);
	}

void CRandomEncounterTypeTable::Realize (const CSystem &System) const

//	Realize
//
//	Generate a table.

	{
	if (m_bValid)
		return;

	//	Get some data

	CUniverse &Universe = System.GetUniverse();
	const CTopologyNode *pNode = System.GetTopology();

	//	Loop over all encounter types and add them, if appropriate.

	m_Table.DeleteAll();
	for (int i = 0; i < Universe.GetStationTypeCount(); i++)
		{
		CStationType &Type = *Universe.GetStationType(i);
		const CStationEncounterDesc &EncounterDesc = Type.GetEncounterDescConst();

		//	Figure out the frequency of an encounter from this station based
		//	on the frequency of the station at this level and the frequency
		//	of encounters for this station.

		int iEncounterFreq = Type.GetEncounterFrequency();
		int iStationFreq = (pNode ? EncounterDesc.CalcFrequencyForNode(*pNode) : ftCommon);
		int iFreq = iEncounterFreq * iStationFreq / ftCommon;

		//	Add to the table

		if (iFreq > 0)
			{
			SEntry Entry;
			Entry.pType = &Type;
			Entry.pTable = Type.GetEncountersTable();
			Entry.pBaseSovereign = Type.GetSovereign();

			m_Table.Insert(Entry, iFreq);
			}
		}

	m_bValid = true;
	}
