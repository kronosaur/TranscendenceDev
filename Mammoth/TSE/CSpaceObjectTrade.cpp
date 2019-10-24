//	CSpaceObjectTrade.cpp
//
//	CSpaceObject class
//	Copyright (c) 2014 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define STR_STATION_DESTROYED					CONSTLIT("abandoned")

const int DAYS_TO_REFRESH_INVENTORY =	5;

void CSpaceObject::AddBuyOrder (CItemType *pType, const CString &sCriteria, int iPriceAdj)

//	AddBuyOrder
//
//	Adds an override buy order

	{
	CTradingDesc *pTrade = AllocTradeDescOverride();
	pTrade->AddBuyOrder(pType, sCriteria, iPriceAdj);
	}

void CSpaceObject::AddSellOrder (CItemType *pType, const CString &sCriteria, int iPriceAdj)

//	AddSellOrder
//
//	Adds an override sell order

	{
	CTradingDesc *pTrade = AllocTradeDescOverride();
	pTrade->AddSellOrder(pType, sCriteria, iPriceAdj);
	}

void CSpaceObject::AddTradeDesc (const CTradingDesc &Trade)

//	AddTrade
//
//	Adds trading entries

	{
	CTradingDesc *pTrade = AllocTradeDescOverride();
	pTrade->AddOrders(Trade);
	}

void CSpaceObject::AddTradeOrder (ETradeServiceTypes iService, const CString &sCriteria, CItemType *pItemType, int iPriceAdj)

//	AddTradeOrder
//
//	Adds a trade order to the override

	{
	CTradingDesc *pTrade = AllocTradeDescOverride();
	pTrade->AddOrder(iService, sCriteria, pItemType, iPriceAdj);
	}

CurrencyValue CSpaceObject::ChargeMoney (DWORD dwEconomyUNID, CurrencyValue iValue)

//	ChargeMoney
//
//	Subtract money. We return the remaining station balance or -1 if there
//	isn't enough.

	{
	CCurrencyBlock *pMoney = GetCurrencyBlock();
	if (pMoney == NULL)
		return -1;

	if (dwEconomyUNID == 0)
		dwEconomyUNID = GetDefaultEconomyUNID();

	if (iValue < 0 || pMoney->GetCredits(dwEconomyUNID) < iValue)
		return -1;

	return pMoney->IncCredits(dwEconomyUNID, -iValue);
	}

CurrencyValue CSpaceObject::CreditMoney (DWORD dwEconomyUNID, CurrencyValue iValue)

//	CreditMoney
//
//	Add money to the account. We return the new balance or -1 if we failed
//	somehow.

	{
	CCurrencyBlock *pMoney = GetCurrencyBlock(true);
	if (pMoney == NULL)
		return -1;

	if (dwEconomyUNID == 0)
		dwEconomyUNID = GetDefaultEconomyUNID();

	if (iValue > 0)
		return pMoney->IncCredits(dwEconomyUNID, iValue);
	else
		return pMoney->GetCredits(dwEconomyUNID);
	}

bool CSpaceObject::GetArmorInstallPrice (const CItem &Item, DWORD dwFlags, int *retiPrice, CString *retsReason) const

//	GetArmorInstallPrice
//
//	Returns the price to install the given armor

	{
	if (IsAbandoned())
		return false;

	//	See if we have an override

	CTradingDesc *pTradeOverride = GetTradeDescOverride();
	if (pTradeOverride && pTradeOverride->GetArmorInstallPrice(*this, Item, dwFlags, retiPrice))
		return true;

	//	Otherwise, ask our design type

	CDesignType *pType = GetType();
	if (pType == NULL)
		return false;

	CTradingDesc *pTrade = pType->GetTradingDesc();
	if (pTrade && pTrade->GetArmorInstallPrice(*this, Item, dwFlags, retiPrice, retsReason))
		return true;

	//	For compatibility, any ship prior to version 23 has a default.
	//	[For API Version 23 and above, ships must have a <Trade> descriptor.]

	if (pType->GetAPIVersion() < 23
			&& pType->GetType() == designShipClass)
		{
		if (retiPrice)
			*retiPrice = CTradingDesc::CalcPriceForService(serviceReplaceArmor, this, Item, 1, dwFlags);

		return true;
		}

	//	Otherwise, we do not repair

	return false;
	}

bool CSpaceObject::GetArmorRepairPrice (CSpaceObject *pSource, const CItem &Item, int iHPToRepair, DWORD dwFlags, int *retiPrice)

//	GetArmorRepairPrice
//
//	Returns the price to repair the given number of HP for the given armor item.

	{
	if (IsAbandoned())
		return false;

	//	See if we have an override

	CTradingDesc *pTradeOverride = GetTradeDescOverride();
	if (pTradeOverride && pTradeOverride->GetArmorRepairPrice(*this, pSource, Item, iHPToRepair, dwFlags, retiPrice))
		return true;

	//	Otherwise, ask our design type

	CDesignType *pType = GetType();
	if (pType == NULL)
		return false;

	CTradingDesc *pTrade = pType->GetTradingDesc();
	if (pTrade && pTrade->GetArmorRepairPrice(*this, pSource, Item, iHPToRepair, dwFlags, retiPrice))
		return true;

	//	For compatibility, any ship prior to version 23 has a default.
	//	[For API Version 23 and above, ships must have a <Trade> descriptor.]

	if (pType->GetAPIVersion() < 23
			&& pType->GetType() == designShipClass)
		{
		if (retiPrice)
			*retiPrice = CTradingDesc::CalcPriceForService(serviceRepairArmor, this, Item, iHPToRepair, dwFlags);

		return true;
		}

	//	Otherwise, we do not repair

	return false;
	}

CurrencyValue CSpaceObject::GetBalance (DWORD dwEconomyUNID) const

//	GetBalance
//
//	Returns the current balance.

	{
	const CCurrencyBlock *pMoney = GetCurrencyBlock();
	if (pMoney == NULL)
		return 0;

	if (dwEconomyUNID == 0)
		dwEconomyUNID = GetDefaultEconomyUNID();

	return pMoney->GetCredits(dwEconomyUNID);
	}

int CSpaceObject::GetBuyPrice (const CItem &Item, DWORD dwFlags, int *retiMaxCount)

//	GetBuyPrice
//
//	Returns the price at which the station will buy the given
//	item. Also returns the max number of items that the station
//	will buy at that price.
//
//	Returns -1 if the station will not buy the item.

	{
	int iPrice;

	if (IsAbandoned())
		return -1;

	//	First see if we have an override

	CTradingDesc *pTradeOverride = GetTradeDescOverride();
	if (pTradeOverride && pTradeOverride->Buys(this, Item, dwFlags, &iPrice, retiMaxCount))
		return iPrice;

	//	See if our type has a price

	CDesignType *pType = GetType();
	if (pType == NULL)
		return -1;

	CTradingDesc *pTrade = pType->GetTradingDesc();
	if (pTrade && pTrade->Buys(this, Item, dwFlags, &iPrice, retiMaxCount))
		return iPrice;

	//	Otherwise, we will not buy the item

	return -1;
	}

const CEconomyType *CSpaceObject::GetDefaultEconomy (void) const

//	GetDefaultEconomy
//
//	Returns the default economy

	{
	CTradingDesc *pTradeOverride = GetTradeDescOverride();
	if (pTradeOverride)
		return pTradeOverride->GetEconomyType();

	CDesignType *pType = GetType();
	if (pType)
		return pType->GetEconomyType();

	return &GetUniverse().GetDefaultCurrency();
	}

DWORD CSpaceObject::GetDefaultEconomyUNID (void) const

//	GetDefaultEconomyUNID
//
//	Returns the default economy
	
	{
	const CEconomyType *pCurrency = GetDefaultEconomy();
	if (pCurrency)
		return pCurrency->GetUNID();

	return DEFAULT_ECONOMY_UNID;
	}

bool CSpaceObject::GetDeviceInstallPrice (const CItem &Item, DWORD dwFlags, int *retiPrice, CString *retsReason, DWORD *retdwPriceFlags) const

//	GetDeviceInstallPrice
//
//	Returns the price to install the given device

	{
	//	Defaults to no reason

	if (retsReason)
		*retsReason = NULL_STR;

	if (IsAbandoned())
		return false;

	//	See if we have an override

	CTradingDesc *pTradeOverride = GetTradeDescOverride();
	if (pTradeOverride && pTradeOverride->GetDeviceInstallPrice(*this, Item, dwFlags, retiPrice, retsReason, retdwPriceFlags))
		return true;

	//	Otherwise, ask our design type

	CDesignType *pType = GetType();
	if (pType == NULL)
		{
		if (retsReason)
			*retsReason = CONSTLIT("Unknown item.");
		return false;
		}

	CTradingDesc *pTrade = pType->GetTradingDesc();
	if (pTrade && pTrade->GetDeviceInstallPrice(*this, Item, dwFlags, retiPrice, retsReason, retdwPriceFlags))
		return true;

	//	Otherwise, we do not install

	return false;
	}

bool CSpaceObject::GetDeviceRemovePrice (const CItem &Item, DWORD dwFlags, int *retiPrice, DWORD *retdwPriceFlags) const

//	GetDeviceRemovePrice
//
//	Returns the price to install the given device

	{
	if (IsAbandoned())
		return false;

	//	See if we have an override

	CTradingDesc *pTradeOverride = GetTradeDescOverride();
	if (pTradeOverride && pTradeOverride->GetDeviceRemovePrice(*this, Item, dwFlags, retiPrice, retdwPriceFlags))
		return true;

	//	Otherwise, ask our design type

	CDesignType *pType = GetType();
	if (pType == NULL)
		return false;

	CTradingDesc *pTrade = pType->GetTradingDesc();
	if (pTrade && pTrade->GetDeviceRemovePrice(*this, Item, dwFlags, retiPrice, retdwPriceFlags))
		return true;

	//	Otherwise, we do not remove

	return false;
	}

bool CSpaceObject::GetRefuelItemAndPrice (CSpaceObject *pObjToRefuel, CItemType **retpItemType, int *retiPrice)

//	GetRefuelItemAndPrice
//
//	Returns the appropriate item to use for refueling (based on the trading
//	directives).

	{
	int i;

	if (IsAbandoned())
		return false;

	//	See if we have an override

	CTradingDesc *pTradeOverride = GetTradeDescOverride();
	if (pTradeOverride && pTradeOverride->GetRefuelItemAndPrice(*this, pObjToRefuel, 0, retpItemType, retiPrice))
		return true;

	//	Otherwise, ask our design type

	CDesignType *pType = GetType();
	if (pType == NULL)
		return false;

	CTradingDesc *pTrade = pType->GetTradingDesc();
	if (pTrade && pTrade->GetRefuelItemAndPrice(*this, pObjToRefuel, 0, retpItemType, retiPrice))
		return true;

	//	For compatibility, any ship prior to version 23 has a default.
	//	[For API Version 23 and above, ships must have a <Trade> descriptor.]

	if (pType->GetAPIVersion() < 23
			&& pType->GetType() == designShipClass)
		{
		//	Get the ship

		CShip *pShipToRefuel = pObjToRefuel->AsShip();
		if (pShipToRefuel == NULL)
			return false;

		//	Find the lowest-level item that can be used by the ship. The lowest
		//	level item will be the cheapest item per fuel unit.

		int iBestLevel = 0;
		int iBestPrice = 0;
		CItemType *pBestItem = NULL;

		for (i = 0; i < GetUniverse().GetItemTypeCount(); i++)
			{
			CItemType *pType = GetUniverse().GetItemType(i);
			CItem Item(pType, 1);

			if (pShipToRefuel->IsFuelCompatible(Item))
				{
				if (pBestItem == NULL || pType->GetLevel() < iBestLevel)
					{
					//	Compute the price, because if we don't sell it, then we
					//	skip it.
					//
					//	NOTE: Unlike selling, we allow 0 prices because some 
					//	stations give fuel for free.

					int iPrice = CTradingDesc::CalcPriceForService(serviceRefuel, this, Item, 1, 0);
					if (iPrice >= 0)
						{
						pBestItem = pType;
						iBestLevel = pType->GetLevel();
						iBestPrice = iPrice;
						}
					}
				}
			}

		if (pBestItem == NULL)
			return false;

		//	Done

		if (retpItemType)
			*retpItemType = pBestItem;

		if (retiPrice)
			*retiPrice = iBestPrice;

		return true;
		}

	//	Otherwise, we do not refuel

	return false;
	}

int CSpaceObject::GetSellPrice (const CItem &Item, DWORD dwFlags)

//	GetSellPrice
//
//	Returns the price at which the station will sell the given
//	item. Returns 0 if the station cannot or will not sell the
//	item.

	{
	bool bHasTradeDirective = false;
	int iPrice = -1;

	if (IsAbandoned())
		return 0;

	//	See if we have an override price

	CTradingDesc *pTradeOverride = GetTradeDescOverride();
	if (pTradeOverride)
		{
		pTradeOverride->Sells(this, Item, dwFlags, &iPrice);
		bHasTradeDirective = true;
		}

	//	See if our type has a price

	CDesignType *pType = GetType();
	if (iPrice == -1)
		{
		CTradingDesc *pTrade = (pType ? pType->GetTradingDesc() : NULL);
		if (pTrade)
			{
			pTrade->Sells(this, Item, dwFlags, &iPrice);
			bHasTradeDirective = true;
			}
		}

	//	If we still have no price, then try to figure out a default.

	if (iPrice == -1)
		{
		//	If we have Trade directives and they specify no price, then we can't
		//	sell any.

		if (bHasTradeDirective)
			return 0;

		//	For compatibility, any ship prior to version 23 has a default.
		//	[For API Version 23 and above, ships must have a <Trade> descriptor.]

		else if (pType 
				&& pType->GetAPIVersion() < 23
				&& pType->GetType() == designShipClass)
			iPrice = CTradingDesc::CalcPriceForService(serviceSell, this, Item, 1, dwFlags);

		//	Otherwise, get the price from the item itself

		else
			iPrice = (int)GetDefaultEconomy()->Exchange(Item.GetCurrencyType(), Item.GetTradePrice(this));
		}

	//	If we don't have any of the item, then we don't sell any

	if (!(dwFlags & CTradingDesc::FLAG_NO_INVENTORY_CHECK))
		{
		CItemListManipulator ItemList(GetItemList());
		if (!ItemList.SetCursorAtItem(Item))
			return 0;
		}

	//	Return the price

	return iPrice;
	}

bool CSpaceObject::GetShipBuyPrice (CShipClass *pClass, DWORD dwFlags, int *retiPrice)

//	GetShipBuyPrice
//
//	Returns the price for which we'd sell the given ship.

	{
	if (IsAbandoned())
		return false;

	//	See if we have an override price

	CTradingDesc *pTradeOverride = GetTradeDescOverride();
	if (pTradeOverride && pTradeOverride->BuysShip(this, pClass, dwFlags, retiPrice))
		return true;

	//	Otherwise, ask our design type

	CDesignType *pType = GetType();
	if (pType == NULL)
		return false;

	CTradingDesc *pTrade = pType->GetTradingDesc();
	if (pTrade && pTrade->BuysShip(this, pClass, dwFlags, retiPrice))
		return true;

	//	Otherwise, we do not sell.

	return false;
	}

bool CSpaceObject::GetShipBuyPrice (CSpaceObject *pShip, DWORD dwFlags, int *retiPrice)

//	GetShipBuyPrice
//
//	Returns the price for which we'd sell the given ship.

	{
	if (IsAbandoned())
		return false;

	//	See if we have an override price

	CTradingDesc *pTradeOverride = GetTradeDescOverride();
	if (pTradeOverride && pTradeOverride->BuysShip(this, pShip, dwFlags, retiPrice))
		return true;

	//	Otherwise, ask our design type

	CDesignType *pType = GetType();
	if (pType == NULL)
		return false;

	CTradingDesc *pTrade = pType->GetTradingDesc();
	if (pTrade && pTrade->BuysShip(this, pShip, dwFlags, retiPrice))
		return true;

	//	Otherwise, we do not sell.

	return false;
	}

bool CSpaceObject::GetShipSellPrice (CShipClass *pClass, DWORD dwFlags, int *retiPrice)

//	GetShipSellPrice
//
//	Returns the price for which we'd sell the given ship.

	{
	if (IsAbandoned())
		return false;

	//	See if we have an override price

	CTradingDesc *pTradeOverride = GetTradeDescOverride();
	if (pTradeOverride && pTradeOverride->SellsShip(this, pClass, dwFlags, retiPrice))
		return true;

	//	Otherwise, ask our design type

	CDesignType *pType = GetType();
	if (pType == NULL)
		return false;

	CTradingDesc *pTrade = pType->GetTradingDesc();
	if (pTrade && pTrade->SellsShip(this, pClass, dwFlags, retiPrice))
		return true;

	//	Otherwise, we do not sell.

	return false;
	}

bool CSpaceObject::GetShipSellPrice (CSpaceObject *pShip, DWORD dwFlags, int *retiPrice)

//	GetShipSellPrice
//
//	Returns the price for which we'd sell the given ship.

	{
	if (IsAbandoned())
		return false;

	//	See if we have an override price

	CTradingDesc *pTradeOverride = GetTradeDescOverride();
	if (pTradeOverride && pTradeOverride->SellsShip(this, pShip, dwFlags, retiPrice))
		return true;

	//	Otherwise, ask our design type

	CDesignType *pType = GetType();
	if (pType == NULL)
		return false;

	CTradingDesc *pTrade = pType->GetTradingDesc();
	if (pTrade && pTrade->SellsShip(this, pShip, dwFlags, retiPrice))
		return true;

	//	Otherwise, we do not sell.

	return false;
	}

int CSpaceObject::GetTradeMaxLevel (ETradeServiceTypes iService)

//	GetTradeMaxLevel
//
//	Returns the max tech level for this service.

	{
	int iMaxLevel = -1;

	//	See if we have an override

	CTradingDesc *pTradeOverride = GetTradeDescOverride();
	if (pTradeOverride)
		{
		int iLevel = pTradeOverride->GetMaxLevelMatched(iService);
		if (iLevel > iMaxLevel)
			iMaxLevel = iLevel;
		}

	//	Ask base type

	CDesignType *pType = GetType();
	CTradingDesc *pTrade = (pType ? pType->GetTradingDesc() : NULL);
	if (pTrade)
		{
		int iLevel = pTrade->GetMaxLevelMatched(iService);
		if (iLevel > iMaxLevel)
			iMaxLevel = iLevel;
		}

	return iMaxLevel;
	}

bool CSpaceObject::HasTradeService (ETradeServiceTypes iService, const CTradingDesc::SHasServiceOptions &Options)

//	HasTradeService
//
//	Returns TRUE if we provide the given service.

	{
	//	See if we have an override

	CTradingDesc *pTradeOverride = GetTradeDescOverride();
	if (pTradeOverride && pTradeOverride->HasService(iService, Options))
		return true;

	//	Ask base type

	CDesignType *pType = GetType();
	CTradingDesc *pTrade = (pType ? pType->GetTradingDesc() : NULL);
	if (pTrade && pTrade->HasService(iService, Options))
		return true;

	//	No service

	return false;
	}

bool CSpaceObject::HasTradeUpgradeOnly (ETradeServiceTypes iService)

//	HasTradeUpgradeOnly
//
//	Returns TRUE if we provide the service for upgrade only.

	{
	//	See if we have an override

	CTradingDesc *pTradeOverride = GetTradeDescOverride();
	if (pTradeOverride && pTradeOverride->HasServiceUpgradeOnly(iService))
		return true;

	//	Ask base type

	CDesignType *pType = GetType();
	CTradingDesc *pTrade = (pType ? pType->GetTradingDesc() : NULL);
	if (pTrade && pTrade->HasServiceUpgradeOnly(iService))
		return true;

	//	No service

	return false;
	}

void CSpaceObject::SetTradeDesc (const CEconomyType *pCurrency, int iMaxCurrency, int iReplenishCurrency)

//	SetTradeDesc
//
//	Overrides trade desc

	{
	CTradingDesc *pTradeOverride = AllocTradeDescOverride();
	if (pTradeOverride == NULL)
		return;

	pTradeOverride->SetEconomyType(pCurrency);
	pTradeOverride->SetDefaultMaxBalance(iMaxCurrency);
	pTradeOverride->SetDefaultReplenish(iReplenishCurrency);

	//	This call will set up the currency.

	pTradeOverride->OnCreate(this);
	}

void CSpaceObject::UpdateTrade (SUpdateCtx &Ctx, int iInventoryRefreshed)

//	UpdateTrade
//
//	Updates trading directives. This creates new inventory, if necessary.

	{
	DEBUG_TRY

	//	Update override first

	CTradingDesc *pTradeOverride = GetTradeDescOverride();
	if (pTradeOverride)
		{
		pTradeOverride->OnUpdate(this);

		if (!IsPlayerDocked())
			pTradeOverride->RefreshInventory(this, iInventoryRefreshed);
		}

	//	Handle base

	CDesignType *pType = GetType();
	CTradingDesc *pTrade = (pType ? pType->GetTradingDesc() : NULL);
	if (pTrade)
		{
		//	If we have a trade desc override, then don't update. [Otherwise
		//	we will replenish currency at double the rate.]

		if (pTradeOverride == NULL)
			pTrade->OnUpdate(this);

		//	But we still need to refresh inventory, since the base 
		//	may contain items not in the override.
		//
		//	LATER: Note that this doesn't handle the case where we try
		//	to override a specific item. The fix is to add the concept
		//	of overriding directly into the class.

		if (!IsPlayerDocked())
			pTrade->RefreshInventory(this, iInventoryRefreshed);
		}

	DEBUG_CATCH
	}

void CSpaceObject::UpdateTradeExtended (const CTimeSpan &ExtraTime)

//	UpdateTradeExtended
//
//	Update trade after a long time.

	{
	//	Refresh inventory, if necessary

	CTradingDesc *pTradeOverride = GetTradeDescOverride();
	CDesignType *pType = GetType();
	CTradingDesc *pTrade = (pType ? pType->GetTradingDesc() : NULL);
	if ((pTrade || pTradeOverride) && ExtraTime.Days() > 0 && !IsAbandoned())
		{
		//	Compute the percent of the inventory that need to refresh
		//	(If % is >100 then we increase availability).

		int iRefreshPercent = Min(300, 100 * ExtraTime.Days() / DAYS_TO_REFRESH_INVENTORY);

		//	Do it

		if (pTradeOverride)
			pTradeOverride->RefreshInventory(this, iRefreshPercent);

		if (pTrade)
			pTrade->RefreshInventory(this, iRefreshPercent);
		}
	}
