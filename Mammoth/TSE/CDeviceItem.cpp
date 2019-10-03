//	CDeviceItem.cpp
//
//	CDeviceItem class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CDeviceItem::AccumulateAttributes (const CItem &Ammo, TArray<SDisplayAttribute> *retList) const

//	AccumulateAttributes
//
//	Accumulate display attributes.

	{
	GetDeviceClass().AccumulateAttributes(*this, Ammo, retList);
	}

