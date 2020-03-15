//	CShipUpdateSet.cpp
//
//	CShipUpdateSet class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CShipUpdateSet::Set (EUpdateTypes iType, bool bValue)

//	Set
//
//	Sets the given flag.

	{
	//	Set the flag

	m_bSet[iType] = bValue;

	//	Some flags also affect other flags

	if (bValue)
		{
		switch (iType)
			{
			case armorBonus:
				m_bSet[armorStatus] = true;
				break;

			case armorStatus:
			case deviceBonus:
			case overlays:
				m_bSet[performance] = true;
				break;
			}
		}
	}
