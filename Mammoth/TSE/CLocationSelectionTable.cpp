//	CLocationSelectionTable.cpp
//
//	CLocationSelectionTable class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CLocationSelectionTable::DeleteInRange (const CVector &vCenter, Metric rRadius)

//	DeleteInRange
//
//	Deletes all entries in the given range.

	{
	const Metric rRadius2 = rRadius * rRadius;

	for (int i = 0; i < m_Table.GetCount(); i++)
		{
		const CLocationDef &Loc = m_System.GetLocation(m_Table[i]);
		const CVector vPos = Loc.GetOrbit().GetObjectPos();

		if ((vPos - vCenter).Length2() < rRadius2)
			{
			m_Table.Delete(i);
			i--;
			}
		}
	}

void CLocationSelectionTable::Fill (const CLocationDef &Loc, DWORD dwObjID)

//	Fill
//
//	Fills the given location.

	{
	const int iIndex = FindIndex(Loc);
	if (iIndex == -1)
		return;

	if (dwObjID)
		m_System.SetLocationObjID(m_Table[iIndex], dwObjID);

	m_Table.Delete(iIndex);
	}

int CLocationSelectionTable::FindIndex (const CLocationDef &LocToFind) const

//	FindIndex
//
//	Returns a table index (or -1).

	{
	for (int i = 0; i < m_Table.GetCount(); i++)
		{
		const CLocationDef &Loc = m_System.GetLocation(m_Table[i]);
		if (&Loc == &LocToFind)
			return i;
		}

	return -1;
	}

const CLocationDef *CLocationSelectionTable::GetRandom (void) const

//	GetRandom
//
//	Chooses a random location from the table and returns it.

	{
	if (m_Table.IsEmpty())
		return NULL;

	const int iRollPos = m_Table.RollPos();
	const int iLocID = m_Table[iRollPos];
	const CLocationDef &Loc = m_System.GetLocation(iLocID);

	return &Loc;
	}
