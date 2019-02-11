//	CConditionSet.cpp
//
//	CConditionSet class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

static TStaticStringTable<TStaticStringEntry<CConditionSet::ETypes>, CConditionSet::cndCount> CONDITION_TABLE = {
	"blind",				CConditionSet::cndBlind,
	"disarmed",				CConditionSet::cndDisarmed,
	"dragged",				CConditionSet::cndDragged,
	"lrsBlind",				CConditionSet::cndLRSBlind,
	"paralyzed",			CConditionSet::cndParalyzed,
	"radioactive",			CConditionSet::cndRadioactive,
	"shipScreenDisabled",	CConditionSet::cndShipScreenDisabled,
	"spinning",				CConditionSet::cndSpinning,
	"timeStopped",			CConditionSet::cndTimeStopped,
	};

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

CConditionSet::ETypes CConditionSet::ParseCondition (const CString &sCondition)

//	ParseCondition
//
//	Parses a condition.

	{
	auto pEntry = CONDITION_TABLE.GetAt(sCondition);
	if (pEntry == NULL)
		return cndNone;

	return pEntry->Value;
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

ICCItemPtr CConditionSet::WriteAsCCItem (void) const

//	WriteAsCCItem
//
//	Returns a CC list with all conditions (or Nil if none).

	{
	CCodeChain &CC = g_pUniverse->GetCC();
	ICCItemPtr pResult(CC.CreateLinkedList());

	for (int i = 0; i < CONDITION_TABLE.GetCount(); i++)
		{
		if (m_dwSet & CONDITION_TABLE[i].Value)
			pResult->AppendString(CString(CONDITION_TABLE.GetKey(i)));
		}

	if (pResult->GetCount() == 0)
		return ICCItemPtr(ICCItem::Nil);
	else
		return pResult;
	}
