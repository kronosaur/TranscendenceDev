//	CConditionSet.cpp
//
//	CConditionSet class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

bool CConditionSet::Diff (const CConditionSet &OldSet, TArray<ETypes> &Added, TArray<ETypes> &Removed) const

//	Diff
//
//	Compares all flags against the previous set and returns TRUE if anything
//	changed. Added and Removed are initialized with the changes.

	{
	int i;

	if (m_dwSet == OldSet.m_dwSet)
		return false;

	DWORD dwFlag = 1;
	for (i = 0; i < cndCount; i++)
		{
		//	If we have this condition, but the old set does not, then it got
		//	added.

		if ((m_dwSet & dwFlag) && !(OldSet.m_dwSet & dwFlag))
			Added.Insert((ETypes)dwFlag);

		//	If the old set had this condition but we don't, it got removed

		else if ((OldSet.m_dwSet & dwFlag) && !(m_dwSet & dwFlag))
			Removed.Insert((ETypes)dwFlag);

		//	Next flag

		dwFlag = dwFlag << 1;
		}

	return true;
	}

void CConditionSet::Set (const CConditionSet &Conditions)

//	Set
//
//	Sets all the given conditions.

	{
	if (Conditions.IsEmpty())
		return;

	m_dwSet |= Conditions.m_dwSet;
	}
