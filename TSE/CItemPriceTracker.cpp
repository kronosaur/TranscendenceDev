//	CItemPriceTracker.cpp
//
//	CItemPriceTracker class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#ifdef LATER_PRICE_TRACKER

bool CItemPriceTracker::FindBestPlaceToBuy (const CItem &Item, CString *retsNodeID, DWORD *retdwObjID, int *retiPrice) const

//	FindBestPlaceToBuy
//
//	Looks for a place that will buy the given item from the player for the 
//	highest price.

	{
	int i;

	int iBestPrice = -1;
	CString sBestNodeID;
	DWORD dwBestObjID;

	for (i = 0; i < m_Systems.GetCount(); i++)
		{
		const SSystemEntry &Entry = m_Systems[i];
		const SPriceVector *pPrices = Entry.Items.GetAt(Item.GetType()->GetUNID());
		if (pPrices == NULL)
			continue;

		//	Figure out which table to use based on whether the item is damaged
		//	or not.

		const TSortMap<int, TArray<DWORD>> *pTable = (Item.IsDamaged() ? &pPrices->BuyDamagedPrices : &pPrices->BuyPrices);

		//	If no entries in this table, then skip it.

		int iIndex = pTable->GetCount() - 1;
		if (iIndex < 0)
			continue;

		//	See if we have a better price

		if (iBestPrice == -1 || iBestPrice < pTable->GetKey(iIndex))
			{
			sBestNodeID = m_Systems.GetKey(i);
			iBestPrice = pTable->GetKey(iIndex);
			dwBestObjID = pTable->GetValue(iIndex)[0];
			}
		}

	//	If we couldn't find a place that will buy the item, then we're done.

	if (iBestPrice == -1)
		return false;

	//	Otherwise, we found a place

	if (retsNodeID)
		*retsNodeID = sBestNodeID;

	if (retdwObjID)
		*retdwObjID = dwBestObjID;

	if (retiPrice)
		*retiPrice = iBestPrice;

	return true;
	}

bool CItemPriceTracker::FindBestPlaceToSell (const CItem &Item, CString *retsNodeID, DWORD *retdwObjID, int *retiPrice) const

//	FindBestPlaceToSell
//
//	Looks for a place that will sell the given item to the player for the
//	lowest price.

	{
	int i;

	int iBestPrice = -1;
	CString sBestNodeID;
	DWORD dwBestObjID;

	for (i = 0; i < m_Systems.GetCount(); i++)
		{
		const SSystemEntry &Entry = m_Systems[i];
		const SPriceVector *pPrices = Entry.Items.GetAt(Item.GetType()->GetUNID());
		if (pPrices == NULL)
			continue;

		//	Figure out which table to use. For now, we don't discriminate between 
		//	damaged and non-damaged.

		const TSortMap<int, TArray<DWORD>> *pTable = &pPrices->SellPrices;

		//	If no entries in this table, then skip it.

		int iIndex = pTable->GetCount() - 1;
		if (iIndex < 0)
			continue;

		//	See if we have a better price

		if (iBestPrice == -1 || iBestPrice > pTable->GetKey(iIndex))
			{
			sBestNodeID = m_Systems.GetKey(i);
			iBestPrice = pTable->GetKey(iIndex);
			dwBestObjID = pTable->GetValue(iIndex)[0];
			}
		}

	//	If we couldn't find a place that will buy the item, then we're done.

	if (iBestPrice == -1)
		return false;

	//	Otherwise, we found a place

	if (retsNodeID)
		*retsNodeID = sBestNodeID;

	if (retdwObjID)
		*retdwObjID = dwBestObjID;

	if (retiPrice)
		*retiPrice = iBestPrice;

	return true;
	}

void CItemPriceTracker::ReadFromStream (SUniverseLoadCtx &Ctx)

//	ReadFromStream
//
//	

	{
	}

void CItemPriceTracker::UpdatePrices (CSystem *pSystem)

//	UpdatePrices
//
//	Updates the prices from the given system.

	{
	int i;

	CSpaceObject *pPlayerShip = g_pUniverse->GetPlayerShip();
	if (pPlayerShip == NULL)
		return;

	CTopologyNode *pNode = pSystem->GetTopology();
	if (pNode == NULL)
		return;

	//	Clear all prices for this system

	SSystemEntry *pEntry = m_Systems.SetAt(pNode->GetID());
	pEntry->Items.DeleteAll();

	//	Loop over all stations in the system.

	for (i = 0; i < pSystem->GetObjectCount(); i++)
		{
		CSpaceObject *pObj = pSystem->GetObject(i);
		if (pObj == NULL
				|| pObj->IsDestroyed()
				|| pObj->IsEnemy(pPlayerShip)
				|| (!pObj->HasTradeService(serviceBuy) && !pObj->HasTradeService(serviceSell)))
			continue;

		//	Update prices for this object

		UpdatePrices(*pEntry, pSystem, pObj);
		}
	}

void CItemPriceTracker::UpdatePrices (SSystemEntry &Entry, CSystem *pSystem, CSpaceObject *pObj)

//	UpdatePrices
//
//	Updates the prices for this system and this object.

	{
	int i;

	for (i = 0; i < g_pUniverse->GetItemTypeCount(); i++)
		{
		CItemType *pType = g_pUniverse->GetItemType(i);
		CItem Item(pType, 1);
		CItem DamagedItem = Item;
		DamagedItem.SetDamaged();

		int iObjBuysAt = pObj->GetBuyPrice(Item, CTradingDesc::FLAG_NO_INVENTORY_CHECK);
		int iObjBuysDamagedAt = pObj->GetBuyPrice(DamagedItem, CTradingDesc::FLAG_NO_INVENTORY_CHECK);
		int iObjSellsAt = pObj->GetSellPrice(Item, CTradingDesc::FLAG_NO_INVENTORY_CHECK);

		//	Skip if the object doesn't buy or sell this item.

		if (iObjBuysAt <= 0 && iObjBuysDamagedAt <= 0 && iObjSellsAt <= 0)
			continue;

		//	Get an entry for this item.

		SPriceVector *pPrices = Entry.Items.SetAt(pType->GetUNID());

		//	Add the price at which the station will buy

		if (iObjBuysAt > 0)
			{
			TArray<DWORD> *pList = pPrices->BuyPrices.SetAt(iObjBuysAt);
			pList->Insert(pObj->GetID());
			}

		if (iObjBuysDamagedAt > 0)
			{
			TArray<DWORD> *pList = pPrices->BuyDamagedPrices.SetAt(iObjBuysAt);
			pList->Insert(pObj->GetID());
			}

		//	Add the price at which the station will sell

		if (iObjSellsAt > 0)
			{
			TArray<DWORD> *pList = pPrices->SellPrices.SetAt(iObjSellsAt);
			pList->Insert(pObj->GetID());
			}
		}
	}

void CItemPriceTracker::WriteToStream (IWriteStream *pStream)

//	WriteToStream
//
//	

	{
	}

#endif