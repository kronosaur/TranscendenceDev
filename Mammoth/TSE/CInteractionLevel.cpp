//	CInteractionLevel.cpp
//
//	CInteractionLevel class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

bool CInteractionLevel::CalcCanInteractWith (const CInteractionLevel &Other, bool bTargetingOther, int *retiChance) const

//	CalcCanInteractWith
//
//	Returns TRUE if were able to interact.

	{
	//	If either is set to always interact, then we're done.

	if (AlwaysInteracts() || Other.AlwaysInteracts())
		{
		if (retiChance) *retiChance = 100;
		return true;
		}

	//	If neither ever interacts, then we never interact, even if we targeting.

	if (NeverInteracts() && Other.NeverInteracts())
		{
		if (retiChance) *retiChance = 0;
		return false;
		}

	//	Combine the interaction values.

	int iResultInteraction;
	if (bTargetingOther)
		iResultInteraction = Max(m_iInteraction, Other.m_iInteraction);
	else
		iResultInteraction = Max(1, Min(m_iInteraction, Other.m_iInteraction));

	if (retiChance)
		*retiChance = iResultInteraction;

	if (iResultInteraction >= 100)
		return true;
	else if (iResultInteraction <= 0)
		return false;
	else
		return (mathRandom(1, 100) <= iResultInteraction);
	}

int CInteractionLevel::CalcInteractionChanceWith (const CInteractionLevel &Other, bool bTargetingOther) const

//	CalcInteractionChanceWith
//
//	Returns the probability that an object with our interaction level can 
//	interact with another object of the given interaction level.
//
//	bTargetingOther is TRUE if we are targeting the other object (which will
//	change the interaction probability).

	{
	int iChance;
	CalcCanInteractWith(Other, bTargetingOther, &iChance);
	return iChance;
	}
