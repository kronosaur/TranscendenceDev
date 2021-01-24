//	CProgramDesc.cpp
//
//	CProgramDesc class
//	Copyright (c) 2021 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

int CProgramDesc::CalcLevel (int iLevel, int iAdj)

//	CalcLevel
//
//	Calculates level from base and adjustment.

	{
	return Max(1, Min(iLevel + iAdj, MAX_ITEM_LEVEL));
	}
