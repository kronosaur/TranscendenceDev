//	CItemTypeProbabilityTable.cpp
//
//	CItemTypeProbabilityTable class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CItemTypeProbabilityTable::Add (CItemType *pType, Metric rProbability)

//	Add
//
//	Add probability

	{
	bool bNew;
	Metric *pChance = m_Table.SetAt(pType, &bNew);

	if (bNew)
		*pChance = rProbability;
	else
		*pChance = Min(*pChance + rProbability, 1.0);
	}

void CItemTypeProbabilityTable::Add (const CItemTypeProbabilityTable &Src)

//	Add
//
//	Adds probabilities

	{
	int i;

	//	Short-circuit

	if (m_Table.GetCount() == 0)
		{
		m_Table = Src.m_Table;
		return;
		}

	//	Loop over all entries in Src.

	for (i = 0; i < Src.m_Table.GetCount(); i++)
		Add(Src.m_Table.GetKey(i), Src.m_Table[i]);
	}

void CItemTypeProbabilityTable::Scale (Metric rProbability)

//	Scale
//
//	Scale probabilities by the given adjustment.

	{
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		m_Table[i] *= rProbability;
	}

void CItemTypeProbabilityTable::Union (CItemType *pType, Metric rProbability)

//	Union
//
//	Adds an item.

	{
	bool bNew;
	Metric *pChance = m_Table.SetAt(pType, &bNew);

	if (bNew)
		*pChance = rProbability;
	else
		*pChance = 1.0 - ((1.0 - *pChance) * (1.0 - rProbability));
	}

void CItemTypeProbabilityTable::Union (const CItemTypeProbabilityTable &Src)

//	Union
//
//	If an item in this table has probability A and the same item in Src has 
//	probability B, then the union is:
//
//	1 - (1-A * 1-B)
//
//	That is, the inverse probability of the item NOT appearing.

	{
	int i;

	//	Short-circuit

	if (m_Table.GetCount() == 0)
		{
		m_Table = Src.m_Table;
		return;
		}

	//	Loop over all entries in Src.

	for (i = 0; i < Src.m_Table.GetCount(); i++)
		Union(Src.m_Table.GetKey(i), Src.m_Table[i]);
	}
