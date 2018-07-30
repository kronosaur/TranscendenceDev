//	CConditionSet.cpp
//
//	CConditionSet class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CConditionSet::Set (const CConditionSet &Conditions)

//	Set
//
//	Sets all the given conditions.

	{
	if (Conditions.IsEmpty())
		return;

	m_dwSet |= Conditions.m_dwSet;
	}
