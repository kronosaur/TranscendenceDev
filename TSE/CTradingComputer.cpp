//	CTradingComputer.cpp
//
//	CTradingComputer class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

int CTradingComputer::GetItemEstimatedPrice (const CTopologyNode *pNode, CItemType *pItemType)

//	GetItemEstimatedPrice
//
//	Return the standard price for the item in the given node.

	{
	CItem Item(pItemType, 1);
	int iPrice = Item.GetTradePrice();

	int iAdj;
	if (!pNode->GetTradingEconomy().FindPriceAdj(pItemType, &iAdj))
		return iPrice;

	return iAdj * iPrice / 100;
	}

void CTradingComputer::GetItemEstimatedPriceList (const CUniverse &Universe, CItemType *pItemType, DWORD dwFlags, TSortMap<int, TArray<const CTopologyNode *>> &NodesAtPrice)

//	GetItemEstimatedPriceList
//
//	Returns a sorted list of prices. For each price, we associate an
//	array of nodes that sell the given item at that price. The price is returned
//	in the item's default currency.

	{
	int i;
	bool bKnownOnly = ((dwFlags & FLAG_KNOWN_ONLY) ? true : false);

	NodesAtPrice.DeleteAll();

	//	Get the price of the item

	CItem Item(pItemType, 1);
	int iPrice = Item.GetTradePrice();

	//	Loop over all nodes

	const CTopology &Topology = Universe.GetTopology();
	for (i = 0; i < Topology.GetTopologyNodeCount(); i++)
		{
		const CTopologyNode *pNode = Topology.GetTopologyNode(i);
		if (pNode->IsEndGame())
			continue;

		//	Skip unknown nodes, if required.

		if (bKnownOnly && !pNode->IsKnown())
			continue;

		//	Compute the actual price with the adjustment

		int iActualPrice = iPrice;
		int iAdj;
		if (pNode->GetTradingEconomy().FindPriceAdj(pItemType, &iAdj))
			iActualPrice = iAdj * iActualPrice / 100;

		//	Set this price in the result

		TArray<const CTopologyNode *> *pNodeList = NodesAtPrice.SetAt(iActualPrice);
		pNodeList->Insert(pNode);
		}
	}

int CTradingComputer::GetItemBuyPrice (const CUniverse &Universe, const CTopologyNode *pNode, const CItem &Item, DWORD *retdwObjID)

//	GetItemTradePrice
//
//	Return the best price available for the player to sell the given item in 
//	the given system. Optionally returns the Object ID of the buyer. If no 
//	station will buy the item, then we return a price <= 0.

	{
	int i;

	const CObjectTracker &Objects = Universe.GetGlobalObjects();
	TArray<CObjectTracker::SObjEntry> TradingObjs;
	Objects.GetTradingObjects(pNode, TradingObjs);

	STradeServiceCtx TradeCtx;
	TradeCtx.pNode = pNode;
	TradeCtx.pCurrency = Item.GetType()->GetCurrencyType();
	TradeCtx.pItem = &Item;
	TradeCtx.iCount = 1;

	//	Look for the trading object with the best price

	DWORD dwBestObjID = 0;
	int iBestPrice = 0;
	for (i = 0; i < TradingObjs.GetCount(); i++)
		{
		CDesignType *pType = TradingObjs[i].pType;
		CTradingDesc *pTrade = pType->GetTradingDesc();
		if (pTrade == NULL)
			continue;

		int iPrice;
		if (pTrade->Buys(TradeCtx, Item, CTradingDesc::FLAG_NO_DONATION, &iPrice) && iPrice > iBestPrice)
			{
			iBestPrice = iPrice;
			dwBestObjID = TradingObjs[i].dwObjID;
			}
		}

	//	Done

	if (retdwObjID)
		*retdwObjID = dwBestObjID;

	return iBestPrice;
	}
