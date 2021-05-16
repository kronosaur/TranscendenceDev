//	CConditionSet.cpp
//
//	CConditionSet class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

static TStaticStringTable<TStaticStringEntry<ECondition>, (int)ECondition::count> CONDITION_TABLE = {
	"blind",				ECondition::blind,
	"disarmed",				ECondition::disarmed,
	"dragged",				ECondition::dragged,
	"fouled",				ECondition::fouled,
	"lrsBlind",				ECondition::LRSBlind,
	"paralyzed",			ECondition::paralyzed,
	"radioactive",			ECondition::radioactive,
	"shieldBlocked",		ECondition::shieldBlocked,
	"shipScreenDisabled",	ECondition::shipScreenDisabled,
	"spinning",				ECondition::spinning,
	"timeStopped",			ECondition::timeStopped,
	};

static TStaticStringTable<TStaticStringEntry<EConditionResult>, 6> CONDITION_RESULT_TABLE = {
	"alreadyApplied",		EConditionResult::alreadyApplied,
	"alreadyRemoved",		EConditionResult::alreadyRemoved,
	"noEffect",				EConditionResult::noEffect,
	"ok",					EConditionResult::ok,
	"stillApplied",			EConditionResult::stillApplied,
	"unknown",				EConditionResult::unknown,
	};

CString CConditionSet::AsID (EConditionResult iResult)

//	AsID
//
//	Return the ID.

	{
	for (int i = 0; i < CONDITION_RESULT_TABLE.GetCount(); i++)
		if (CONDITION_RESULT_TABLE[i].Value == iResult)
			return CString(CONDITION_RESULT_TABLE[i].pszKey);

	return NULL_STR;
	}

bool CConditionSet::Diff (const CConditionSet &OldSet, TArray<ECondition> &Added, TArray<ECondition> &Removed) const

//	Diff
//
//	Compares all flags against the previous set and returns TRUE if anything
//	changed. Added and Removed are initialized with the changes.

	{
	int i;

	if (m_dwSet == OldSet.m_dwSet)
		return false;

	DWORD dwFlag = 1;
	for (i = 0; i < (int)ECondition::count; i++)
		{
		//	If we have this condition, but the old set does not, then it got
		//	added.

		if ((m_dwSet & dwFlag) && !(OldSet.m_dwSet & dwFlag))
			Added.Insert((ECondition)dwFlag);

		//	If the old set had this condition but we don't, it got removed

		else if ((OldSet.m_dwSet & dwFlag) && !(m_dwSet & dwFlag))
			Removed.Insert((ECondition)dwFlag);

		//	Next flag

		dwFlag = dwFlag << 1;
		}

	return true;
	}

bool CConditionSet::IsSuccessResult (EConditionResult iResult)

//	IsSuccessResult
//
//	Returns TRUE if this is a result the means the condition was removed
//	successfully.

	{
	switch (iResult)
		{
		case EConditionResult::alreadyApplied:
		case EConditionResult::alreadyRemoved:
		case EConditionResult::ok:
			return true;

		default:
			return false;
		}
	}

ECondition CConditionSet::ParseCondition (const CString &sCondition)

//	ParseCondition
//
//	Parses a condition.

	{
	auto pEntry = CONDITION_TABLE.GetAt(sCondition);
	if (pEntry == NULL)
		return ECondition::none;

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
	ICCItemPtr pResult(ICCItem::List);

	for (int i = 0; i < CONDITION_TABLE.GetCount(); i++)
		{
		if (m_dwSet & (DWORD)CONDITION_TABLE[i].Value)
			pResult->AppendString(CString(CONDITION_TABLE.GetKey(i)));
		}

	if (pResult->GetCount() == 0)
		return ICCItemPtr(ICCItem::Nil);
	else
		return pResult;
	}
