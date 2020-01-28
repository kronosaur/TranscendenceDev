//	WeaponClassImpl.h
//
//	Implementation classes for CWeaponClass
//	Copyright (c) 20202 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CTargetingSolution
	{
	public:
		CTargetingSolution (CDeviceItem &DeviceItem, const CTargetList &TargetList);

		bool Compute (void);
	};
