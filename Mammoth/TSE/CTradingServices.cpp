//	CTradingServices.cpp
//
//	CTradingServices class
//	Copyright (c) 2021 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CTradingServices::CTradingServices (const CSpaceObject &Source) :
		m_pOverride(Source.GetTradeDescOverride()),
		m_pDesc(Source.GetType() ? Source.GetType()->GetTradingDesc() : NULL)

//	CTradingServices constructor

	{
	}

bool CTradingServices::GetArmorInstallPrice (const CSpaceObject &Source, CArmorItem ArmorItem, DWORD dwFlags, int *retiPrice, CString *retsReason) const

//	GetArmorInstallPrice
//
//	Returns TRUE if we can install the given armor item.

	{
	if (!ArmorItem || Source.IsAbandoned())
		return false;

	//	See if we have an override

	if (m_pOverride && m_pOverride->GetArmorInstallPrice(Source, ArmorItem, dwFlags, retiPrice))
		return true;

	//	Otherwise, ask base descriptor (if any)

	if (m_pDesc && m_pDesc->GetArmorInstallPrice(Source, ArmorItem, dwFlags, retiPrice))
		return true;

	//	For compatibility, any ship prior to version 23 has a default.
	//	[For API Version 23 and above, ships must have a <Trade> descriptor.]

	if (Source.GetType() 
			&& Source.GetType()->GetAPIVersion() < 23
			&& Source.GetType()->GetType() == designShipClass)
		{
		if (retiPrice)
			*retiPrice = CTradingDesc::CalcPriceForService(serviceReplaceArmor, &Source, ArmorItem, 1, dwFlags);

		return true;
		}

	//	Otherwise, we do not repair

	return false;
	}

bool CTradingServices::GetArmorRepairPrice (const CSpaceObject &Source, CArmorItem ArmorItem, int iHPToRepair, DWORD dwFlags, int *retiPrice) const

//	GetArmorRepairPrice
//
//	Returns TRUE if we can repair the given armor item.

	{
	if (!ArmorItem || Source.IsAbandoned())
		return false;

	//	See if we have an override

	if (m_pOverride && m_pOverride->GetArmorRepairPrice(Source, ArmorItem.GetSource(), ArmorItem, iHPToRepair, dwFlags, retiPrice))
		return true;

	//	Otherwise, ask our design type

	if (m_pDesc && m_pDesc->GetArmorRepairPrice(Source, ArmorItem.GetSource(), ArmorItem, iHPToRepair, dwFlags, retiPrice))
		return true;

	//	For compatibility, any ship prior to version 23 has a default.
	//	[For API Version 23 and above, ships must have a <Trade> descriptor.]

	if (Source.GetType()
			&& Source.GetType()->GetAPIVersion() < 23
			&& Source.GetType()->GetType() == designShipClass)
		{
		if (retiPrice)
			*retiPrice = CTradingDesc::CalcPriceForService(serviceRepairArmor, &Source, ArmorItem, iHPToRepair, dwFlags);

		return true;
		}

	//	Otherwise, we do not repair

	return false;
	}

bool CTradingServices::GetDeviceInstallPrice (const CSpaceObject &Source, CDeviceItem DeviceItem, DWORD dwFlags, int *retiPrice, CString *retsReason, DWORD *retdwPriceFlags) const

//	GetDeviceInstallPrice
//
//	Returns TRUE if we can install the given device item.

	{
	//	Defaults to no reason

	if (retsReason)
		*retsReason = NULL_STR;

	if (!DeviceItem || Source.IsAbandoned())
		return false;

	//	See if we have an override

	if (m_pOverride && m_pOverride->GetDeviceInstallPrice(Source, DeviceItem, dwFlags, retiPrice, retsReason, retdwPriceFlags))
		return true;

	//	Otherwise, ask our design type

	if (m_pDesc && m_pDesc->GetDeviceInstallPrice(Source, DeviceItem, dwFlags, retiPrice, retsReason, retdwPriceFlags))
		return true;

	//	Otherwise, we do not install

	return false;
	}

bool CTradingServices::GetDeviceRemovePrice (const CSpaceObject &Source, CDeviceItem DeviceItem, DWORD dwFlags, int *retiPrice, DWORD *retdwPriceFlags) const

//	GetDeviceRemovePrice
//
//	Returns TRUE if we can remove the given device.

	{
	if (!DeviceItem || Source.IsAbandoned())
		return false;

	//	See if we have an override

	if (m_pOverride && m_pOverride->GetDeviceRemovePrice(Source, DeviceItem, dwFlags, retiPrice, retdwPriceFlags))
		return true;

	//	Otherwise, ask our design type

	if (m_pDesc && m_pDesc->GetDeviceRemovePrice(Source, DeviceItem, dwFlags, retiPrice, retdwPriceFlags))
		return true;

	//	Otherwise, we do not remove

	return false;
	}

bool CTradingServices::GetItemInstallPrice (const CSpaceObject &Source, const CItem &Item, DWORD dwFlags, int *retiPrice, CString *retsReason, DWORD *retdwPriceFlags) const

//	GetItemInstallPrice
//
//	Returns TRUE if we can install the given item (either armor or device).

	{
	if (Item.IsArmor())
		{
		if (retdwPriceFlags)
			*retdwPriceFlags = 0;

		return GetArmorInstallPrice(Source, Item.AsArmorItem(), 0, retiPrice, retsReason);
		}
	else if (Item.IsDevice())
		return GetDeviceInstallPrice(Source, Item.AsDeviceItem(), 0, retiPrice, retsReason, retdwPriceFlags);
	else
		{
		if (retsReason)
			*retsReason = NULL_STR;

		return false;
		}
	}

bool CTradingServices::GetItemRemovePrice (const CSpaceObject &Source, const CItem &Item, DWORD dwFlags, int *retiPrice, DWORD *retdwPriceFlags) const

//	GetItemRemovePrice
//
//	Returns TRUE if we can remove the given item.

	{
	if (Item.IsDevice())
		return GetDeviceRemovePrice(Source, Item.AsDeviceItem(), 0, retiPrice, retdwPriceFlags);
	else
		return false;
	}
