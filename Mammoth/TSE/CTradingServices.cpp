//	CTradingServices.cpp
//
//	CTradingServices class
//	Copyright (c) 2021 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CTradingServices::CTradingServices (const CSpaceObject &Source) :
		m_pType(Source.GetType()),
		m_pProvider(&Source),
		m_pOverride(Source.GetTradeDescOverride()),
		m_pDesc(Source.GetType() ? Source.GetType()->GetTradingDesc() : NULL)

//	CTradingServices constructor

	{
	}

CTradingServices::CTradingServices (const CDesignType &Type) :
		m_pType(&Type),
		m_pDesc(Type.GetTradingDesc())

//	CTradingServices constructor

	{
	}

bool CTradingServices::GetArmorInstallPrice (CArmorItem ArmorItem, DWORD dwFlags, int *retiPrice, CTradingDesc::SReasonText *retReason) const

//	GetArmorInstallPrice
//
//	Returns TRUE if we can install the given armor item.

	{
	if (!ArmorItem || (m_pProvider && m_pProvider->IsAbandoned()))
		return false;

	//	See if we have an override

	if (m_pOverride && m_pOverride->GetArmorInstallPrice(m_pProvider, ArmorItem, dwFlags, retiPrice))
		return true;

	//	Otherwise, ask base descriptor (if any)

	if (m_pDesc && m_pDesc->GetArmorInstallPrice(m_pProvider, ArmorItem, dwFlags, retiPrice))
		return true;

	//	For compatibility, any ship prior to version 23 has a default.
	//	[For API Version 23 and above, ships must have a <Trade> descriptor.]

	if (m_pType
			&& m_pType->GetAPIVersion() < 23
			&& m_pType->GetType() == designShipClass)
		{
		if (retiPrice)
			*retiPrice = CTradingDesc::CalcPriceForService(serviceReplaceArmor, m_pProvider, ArmorItem, 1, dwFlags);

		return true;
		}

	//	Otherwise, we do not repair

	return false;
	}

bool CTradingServices::GetArmorRepairPrice (CArmorItem ArmorItem, int iHPToRepair, DWORD dwFlags, int *retiPrice) const

//	GetArmorRepairPrice
//
//	Returns TRUE if we can repair the given armor item.

	{
	if (!ArmorItem || (m_pProvider && m_pProvider->IsAbandoned()))
		return false;

	//	See if we have an override

	if (m_pOverride && m_pOverride->GetArmorRepairPrice(m_pProvider, ArmorItem.GetSource(), ArmorItem, iHPToRepair, dwFlags, retiPrice))
		return true;

	//	Otherwise, ask our design type

	if (m_pDesc && m_pDesc->GetArmorRepairPrice(m_pProvider, ArmorItem.GetSource(), ArmorItem, iHPToRepair, dwFlags, retiPrice))
		return true;

	//	For compatibility, any ship prior to version 23 has a default.
	//	[For API Version 23 and above, ships must have a <Trade> descriptor.]

	if (m_pType
			&& m_pType->GetAPIVersion() < 23
			&& m_pType->GetType() == designShipClass)
		{
		if (retiPrice)
			*retiPrice = CTradingDesc::CalcPriceForService(serviceRepairArmor, m_pProvider, ArmorItem, iHPToRepair, dwFlags);

		return true;
		}

	//	Otherwise, we do not repair

	return false;
	}

CRegenDesc CTradingServices::GetArmorRepairRate (DWORD dwFlags, const CRegenDesc &Default) const

//	GetArmorRepairRate
//
//	Returns the rate at which we repair armor.

	{
	if (!Default.IsEmpty())
		return Default;

	else if (HasService(serviceRepairArmor))
		{
		//	Default to 10 hp per 180 ticks, scaled by level.

		Metric rScale = ::pow(REPAIR_PER_LEVEL_EXP, GetLevel() - 1);
		Metric rHPPer180 = mathRound(REPAIR_HP_AT_LEVEL_1 * rScale);

		CRegenDesc Regen;
		Regen.InitFromRegen(rHPPer180, DEFAULT_REPAIR_CYCLE);

		return Regen;
		}
	else
		return CRegenDesc::Null;
	}

bool CTradingServices::GetDeviceInstallPrice (CDeviceItem DeviceItem, DWORD dwFlags, int *retiPrice, CTradingDesc::SReasonText *retReason, DWORD *retdwPriceFlags) const

//	GetDeviceInstallPrice
//
//	Returns TRUE if we can install the given device item.

	{
	//	Defaults to no reason

	if (retReason)
		*retReason = CTradingDesc::SReasonText();

	if (!DeviceItem || (m_pProvider && m_pProvider->IsAbandoned()))
		return false;

	//	See if we have an override

	if (m_pOverride && m_pOverride->GetDeviceInstallPrice(m_pProvider, DeviceItem, dwFlags, retiPrice, retReason, retdwPriceFlags))
		return true;

	//	Otherwise, ask our design type

	if (m_pDesc && m_pDesc->GetDeviceInstallPrice(m_pProvider, DeviceItem, dwFlags, retiPrice, retReason, retdwPriceFlags))
		return true;

	//	Otherwise, we do not install

	return false;
	}

bool CTradingServices::GetDeviceRemovePrice (CDeviceItem DeviceItem, DWORD dwFlags, int *retiPrice, DWORD *retdwPriceFlags) const

//	GetDeviceRemovePrice
//
//	Returns TRUE if we can remove the given device.

	{
	if (!DeviceItem || (m_pProvider && m_pProvider->IsAbandoned()))
		return false;

	//	See if we have an override

	if (m_pOverride && m_pOverride->GetDeviceRemovePrice(m_pProvider, DeviceItem, dwFlags, retiPrice, retdwPriceFlags))
		return true;

	//	Otherwise, ask our design type

	if (m_pDesc && m_pDesc->GetDeviceRemovePrice(m_pProvider, DeviceItem, dwFlags, retiPrice, retdwPriceFlags))
		return true;

	//	Otherwise, we do not remove

	return false;
	}

bool CTradingServices::GetItemInstallPrice (const CItem &Item, DWORD dwFlags, int *retiPrice, CTradingDesc::SReasonText *retReason, DWORD *retdwPriceFlags) const

//	GetItemInstallPrice
//
//	Returns TRUE if we can install the given item (either armor or device).

	{
	if (Item.IsArmor())
		{
		if (retdwPriceFlags)
			*retdwPriceFlags = 0;

		return GetArmorInstallPrice(Item.AsArmorItem(), 0, retiPrice, retReason);
		}
	else if (Item.IsDevice())
		return GetDeviceInstallPrice(Item.AsDeviceItem(), 0, retiPrice, retReason, retdwPriceFlags);
	else
		{
		if (retReason)
			*retReason = CTradingDesc::SReasonText();

		return false;
		}
	}

bool CTradingServices::GetItemRemovePrice (const CItem &Item, DWORD dwFlags, int *retiPrice, DWORD *retdwPriceFlags) const

//	GetItemRemovePrice
//
//	Returns TRUE if we can remove the given item.

	{
	if (Item.IsDevice())
		return GetDeviceRemovePrice(Item.AsDeviceItem(), 0, retiPrice, retdwPriceFlags);
	else
		return false;
	}

int CTradingServices::GetLevel () const

//	GetLevel
//
//	Returns the generic level of the provider.

	{
	if (m_pProvider)
		return m_pProvider->GetLevel();
	else if (m_pType)
		return m_pType->GetLevel();
	else
		return 1;
	}

int CTradingServices::GetMaxLevel (ETradeServiceTypes iService) const

//	GetMaxLevel
//
//	Returns the maximum level that we provide the given service (usually install
//	or repair). If we do not provide the service, we return -1.

	{
	switch (iService)
		{
		case serviceRepairArmor:
			{
			int iMaxLevel = GetMaxLevelMatched(iService);
			if (iMaxLevel != -1)
				return iMaxLevel;

			//	For backwards compatibility, if a station has ship regen, then 
			//	we repair at its level +3.

			else if (const CStationType *pStationType = CStationType::AsType(m_pType))
				{
				if (!pStationType->GetShipRegenDesc().IsEmpty())
					return GetLevel() + 3;
				else
					return -1;
				}

			//	No repairs

			else
				return -1;
			}

		default:
			return GetMaxLevelMatched(iService);
		}
	}

int CTradingServices::GetMaxLevelMatched (ETradeServiceTypes iService) const

//	GetMaxLevelMatched
//
//	Returns the maximum level that we provide the given service (usually install
//	or repair). If we do not provide the service, we return -1.

	{
	int iMaxLevel = -1;

	//	See if we have an override

	if (m_pOverride)
		{
		int iLevel = m_pOverride->GetMaxLevelMatched(GetUniverse(), iService);
		if (iLevel > iMaxLevel)
			iMaxLevel = iLevel;
		}

	//	Ask base type

	if (m_pDesc)
		{
		int iLevel = m_pDesc->GetMaxLevelMatched(GetUniverse(), iService);
		if (iLevel > iMaxLevel)
			iMaxLevel = iLevel;
		}

	return iMaxLevel;
	}

bool CTradingServices::GetRemoveConditionPrice (const CShipClass &Class, ECondition iCondition, DWORD dwFlags, int *retiPrice) const

//	GetRemoveConditionPrice
//
//	Returns TRUE if we remove the given condition on the given class.

	{
	if (m_pProvider && m_pProvider->IsAbandoned())
		return false;

	//	See if we have an override price

	if (m_pOverride && m_pOverride->RemovesCondition(m_pProvider, Class, iCondition, dwFlags, retiPrice))
		return true;

	//	Otherwise, ask our design type

	if (m_pDesc && m_pDesc->RemovesCondition(m_pProvider, Class, iCondition, dwFlags, retiPrice))
		return true;

	//	Otherwise, we do not decontaminate.

	return false;
	}

bool CTradingServices::GetRemoveConditionPrice (const CSpaceObject &Ship, ECondition iCondition, DWORD dwFlags, int *retiPrice) const

//	GetRemoveConditionPrice
//
//	Returns TRUE if we remove the given condition on the given class.

	{
	if (m_pProvider && m_pProvider->IsAbandoned())
		return false;

	//	See if we have an override price

	if (m_pOverride && m_pOverride->RemovesCondition(m_pProvider, Ship, iCondition, dwFlags, retiPrice))
		return true;

	//	Otherwise, ask our design type

	if (m_pDesc && m_pDesc->RemovesCondition(m_pProvider, Ship, iCondition, dwFlags, retiPrice))
		return true;

	//	Otherwise, we do not decontaminate.

	return false;
	}

bool CTradingServices::GetServiceStatus (ETradeServiceTypes iService, CTradingDesc::SServiceStatus &retStatus) const

//	GetServiceStatus
//
//	Returns TRUE if we have this service and returns general properties.

	{
	if (m_pProvider && m_pProvider->IsAbandoned())
		return false;

	//	See if we have an override price

	if (m_pOverride && m_pOverride->GetServiceStatus(GetUniverse(), iService, retStatus))
		return true;

	//	Otherwise, ask our design type

	if (m_pDesc && m_pDesc->GetServiceStatus(GetUniverse(), iService, retStatus))
		return true;

	//	Otherwise, we do not have the service

	return false;
	}

CUniverse &CTradingServices::GetUniverse () const

//	GetUniverse
//
//	Returns the universe.

	{
	if (m_pProvider)
		return m_pProvider->GetUniverse();
	else if (m_pType)
		return m_pType->GetUniverse();
	else
		throw CException(ERR_FAIL);
	}

bool CTradingServices::HasService (ETradeServiceTypes iService, const CTradingDesc::SHasServiceOptions &Options) const

//	HasService
//
//	Returns TRUE if we have the given service.

	{
	//	See if we have an override

	if (m_pOverride && m_pOverride->HasService(GetUniverse(), iService, Options))
		return true;

	//	Ask base type

	if (m_pDesc && m_pDesc->HasService(GetUniverse(), iService, Options))
		return true;

	//	No service

	return false;
	}
