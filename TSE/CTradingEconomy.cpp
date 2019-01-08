//	CTradingEconomy.cpp
//
//	CTradingEconomy class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

const int MIN_IMPACT =				-33;
const int MAX_IMPACT =				50;

const int MIN_PRICE_ADJ =			50;

CTradingEconomy::EImpactCategories CTradingEconomy::CalcImpactCategory (int iImpact) const

//	CalcImpactDesc
//
//	Returns a description of the impact.

	{
	if (iImpact < -50)
		return impactMassiveGlut;
	else if (iImpact < -30)
		return impactMajorGlut;
	else if (iImpact < -15)
		return impactGlut;
	else if (iImpact < 0)
		return impactExports;
	else if (iImpact == 0)
		return impactBalanced;
	else if (iImpact <= 10)
		return impactImports;
	else if (iImpact <= 25)
		return impactShortage;
	else if (iImpact <= 50)
		return impactMajorShortage;
	else
		return impactDesperateShortage;
	}

CString CTradingEconomy::CalcImpactDesc (EImpactCategories iImpact) const

//	CalcImpactDesc
//
//	Returns a description of the impact.

	{
	switch (iImpact)
		{
		case impactMassiveGlut:
			return CONSTLIT("massive surplus of");

		case impactMajorGlut:
			return CONSTLIT("major surplus of");

		case impactGlut:
			return CONSTLIT("surplus of");

		case impactExports:
			return CONSTLIT("exports");

		case impactBalanced:
			return CONSTLIT("balanced use of");

		case impactImports:
			return CONSTLIT("imports");

		case impactShortage:
			return CONSTLIT("shortage of");

		case impactMajorShortage:
			return CONSTLIT("major shortage of");

		case impactDesperateShortage:
			return CONSTLIT("desperate shortage of");

		default:
			return NULL_STR;
		}
	}

CString CTradingEconomy::CalcImpactSortKey (int iImpact, const CString &sName) const

//	CalcImpactSortKey
//
//	Returns the sort key for the impact (shortages go first, followed by gluts)

	{
	return strPatternSubst(CONSTLIT("%d:%04d:%s"), (iImpact > 0 ? 1 : 0), Absolute(iImpact), sName);
	}

CString CTradingEconomy::GetDescription (void) const

//	GetDescription
//
//	Returns a human-readable description of price adjustments due to supply and
//	demand. Blank if all prices are normal.

	{
	int i;

	//	Categorize each impact, so we end up with a list of lists.

	TSortMap<int, TArray<CString>> List;

	//	Start by adding individual item prices. Note that these already include any 
	//	impact from item criteria.

	for (i = 0; i < m_ItemTypeImpact.GetCount(); i++)
		{
		int iImpact = m_ItemTypeImpact[i];
		if (Absolute(iImpact) < 5)
			continue;

		TArray<CString> *pCategory = List.SetAt(CalcImpactCategory(iImpact));
		pCategory->Insert(m_ItemTypeImpact.GetKey(i)->GetNounPhrase(nounShort));
		}

	//	Now add item criteria

	for (i = 0; i < m_CriteriaImpact.GetCount(); i++)
		{
		const SCriteriaEntry &Entry = m_CriteriaImpact[i];
		if (Absolute(Entry.iImpact) < 5)
			continue;

		CString sName = Entry.Criteria.GetName();
		if (sName.IsBlank())
			continue;

		if (g_pUniverse->InDebugMode())
			sName = strPatternSubst(CONSTLIT("%s (%d)"), sName, Entry.iImpact);

		TArray<CString> *pCategory = List.SetAt(CalcImpactCategory(Entry.iImpact));
		pCategory->Insert(sName);
		}

	//	Short-circuit

	if (List.GetCount() == 0)
		return NULL_STR;

	//	Concatenate them all together

	CString sResult;
	for (i = 0; i < List.GetCount(); i++)
		{
		TArray<CString> &Category = List[i];
		Category.Sort();

		if (i > 0)
			sResult.Append(CONSTLIT("; "));

		sResult.Append(strPatternSubst(CONSTLIT("%s %s"), CalcImpactDesc((EImpactCategories)List.GetKey(i)), strJoin(Category, CONSTLIT("oxfordComma"))));
		}

	//	Done

	return strCapitalize(sResult);
	}

bool CTradingEconomy::FindPriceAdj (CItemType *pItem, int *retiAdj) const

//	FindPriceAdj
//
//	Returns the price adjustment for the given item when a station is BUYING the
//	item from the player. Returns FALSE if there is no adjustment (normal price).

	{
	//	Find the impact for this item.

	int iImpact = GetPriceImpact(pItem);
	if (iImpact == 0)
		return false;

	//	If we don't need to compute the actual adjustment, then we're done.

	if (retiAdj == NULL)
		return true;

	//	For positive impact, the price goes up. For negative ones, it goes
	//	down.

	if (iImpact > 0)
		*retiAdj = 100 + iImpact;
	else
		*retiAdj = Max(MIN_PRICE_ADJ, 100 + iImpact);

	return true;
	}

int CTradingEconomy::GetPriceImpact (CItemType *pItem) const

//	GetPriceImpact
//
//	Returns the price impact of the given item. Positive numbers mean the price 
//	increases (due to demand); negative numbers mean the price decreases due
//	to supply. 0 means no change in price.
//
//	NOTE: If multiple criteria apply to this item, we take the largest impact.

	{
	int i;
	CItem Item(pItem, 1);

	int iBestImpact = 0;
	for (i = 0; i < m_CriteriaImpact.GetCount(); i++)
		{
		if (Item.MatchesCriteria(m_CriteriaImpact[i].Criteria))
			{
			if (Absolute(m_CriteriaImpact[i].iImpact) > Absolute(iBestImpact))
				iBestImpact = m_CriteriaImpact[i].iImpact;
			}
		}

	return iBestImpact;
	}

void CTradingEconomy::ReadFromStream (SUniverseLoadCtx &Ctx)

//	ReadFromStream
//
//	DWORD			Count of m_CriteriaImpact
//	CString				Criteria
//	DWORD				iImpact
//
//	DWORD			Count of m_ItemTypeImpact
//	DWORD				UNID
//	DWORD				iImpact
//
//	DWORD			COunt of m_TradeImpact
//	CString				Criteria
//	DWORD				iImpact

	{
	int i;
	DWORD dwLoad;

	Ctx.pStream->Read(dwLoad);
	for (i = 0; i < (int)dwLoad; i++)
		{
		CString sCriteria;
		sCriteria.ReadFromStream(Ctx.pStream);

		SCriteriaEntry *pEntry = m_CriteriaImpact.SetAt(sCriteria);
		CItem::ParseCriteria(sCriteria, &pEntry->Criteria);

		Ctx.pStream->Read(pEntry->iImpact);
		}

	Ctx.pStream->Read(dwLoad);
	for (i = 0; i < (int)dwLoad; i++)
		{
		DWORD dwUNID;
		Ctx.pStream->Read(dwUNID);
		CItemType *pType = g_pUniverse->FindItemType(dwUNID);

		int iImpact;
		Ctx.pStream->Read(iImpact);

		if (pType == NULL)
			continue;

		m_ItemTypeImpact.SetAt(pType, iImpact);
		}

	Ctx.pStream->Read(dwLoad);
	for (i = 0; i < (int)dwLoad; i++)
		{
		CString sCriteria;
		sCriteria.ReadFromStream(Ctx.pStream);

		SCriteriaEntry *pEntry = m_TradeImpact.SetAt(sCriteria);
		CItem::ParseCriteria(sCriteria, &pEntry->Criteria);

		Ctx.pStream->Read(pEntry->iImpact);
		}
	}

void CTradingEconomy::Refresh (CSystem *pSystem)

//	Refresh
//
//	Initializes supply and demand impact based on the current state of the 
//	system.

	{
	int i;

	//	Clean up

	m_CriteriaImpact.DeleteAll();
	m_ItemTypeImpact.DeleteAll();
	m_TradeImpact.DeleteAll();

    //  Loop over all objects and refresh them

    for (i = 0; i < pSystem->GetObjectCount(); i++)
        {
        CSpaceObject *pObj = pSystem->GetObject(i);
        if (pObj == NULL
                || pObj->IsDestroyed())
            continue;

		CDesignType *pType = pObj->GetType();
		if (pType == NULL)
			continue;

		CTradingDesc *pTrade = pType->GetTradingDesc();
		CTradingDesc *pOverride = pObj->GetTradeDescOverride();

		//	If we have two trade blocks, then we need to combine them.

		if (pTrade && pOverride)
			{
			CTradingDesc Combined = *pTrade;
			Combined.AddOrders(*pOverride);
			RefreshFromTradeDesc(pSystem, &Combined);
			}

		//	Otherwise, we incorpate one.

		else if (pTrade)
			RefreshFromTradeDesc(pSystem, pTrade);

		else if (pOverride)
			RefreshFromTradeDesc(pSystem, pOverride);
		}

	//	For each trade impact, adjust the impact for that criteria. Trade
	//	impact serves to reduce consumption/production imbalance.

	for (i = 0; i < m_TradeImpact.GetCount(); i++)
		{
		//	Look up the impact for the given criteria.

		SCriteriaEntry *pEntry = m_CriteriaImpact.GetAt(m_TradeImpact.GetKey(i));
		if (pEntry == NULL)
			continue;

		//	We balance prices based on trade

		if (pEntry->iImpact > 0)
			pEntry->iImpact -= Min(pEntry->iImpact, m_TradeImpact[i].iImpact);
		else if (pEntry->iImpact < 0)
			pEntry->iImpact += Min(-pEntry->iImpact, m_TradeImpact[i].iImpact);
		}

	//	Now make sure prices are not completely out of whack.

	for (i = 0; i < m_CriteriaImpact.GetCount(); i++)
		{
		m_CriteriaImpact[i].iImpact = Max(MIN_IMPACT, Min(m_CriteriaImpact[i].iImpact, MAX_IMPACT));
		}

	//	LATER: For now we don't support individual item type trade impact. To
	//	support it we would need to carry over the criteria impact to the
	//	individual item impact table (adjusting for balance, etc.).
	}

void CTradingEconomy::RefreshFromTradeDesc (CSystem *pSystem, CTradingDesc *pTrade)

//	RefreshFromTradeDesc
//
//	Adds impact based on the given trade desc.

	{
	int i;

	for (i = 0; i < pTrade->GetServiceCount(); i++)
		{
		CTradingDesc::SServiceInfo Service;
		pTrade->GetServiceInfo(i, Service);

		TSortMap<CString, SCriteriaEntry> *pDestTable = NULL;
		int iImpact = 0;

		if (Service.iService == serviceConsume)
			{
			//	If we're consuming, price is increased in this sytem due
			//	to demand.

			pDestTable = &m_CriteriaImpact;
			iImpact = Service.iPriceAdj;
			}
		else if (Service.iService == serviceProduce)
			{
			//	If we're producing, price is reduced in this system due to 
			//	supply.

			pDestTable = &m_CriteriaImpact;
			iImpact = -Service.iPriceAdj;
			}
		else if (Service.iService == serviceTrade)
			{
			//	Trading impact goes on a different able.

			pDestTable = &m_TradeImpact;
			iImpact = Service.iPriceAdj;

			//	NOTE: Trade impact on a single item is not yet supported

			if (Service.pItemType)
				continue;
			}
		else
			continue;

		//	If this only affects a single item, then add it to the table

		if (Service.pItemType)
			{
			bool bNew;
			int *pImpact = m_ItemTypeImpact.SetAt(Service.pItemType, &bNew);
			if (bNew)
				*pImpact = iImpact;
			else
				*pImpact += iImpact;
			}

		//	Otherwise, we set the impact for all items that match the given 
		//	criteria.

		else if (!Service.sItemCriteria.IsBlank())
			{
			bool bNew;
			SCriteriaEntry *pEntry = pDestTable->SetAt(Service.sItemCriteria, &bNew);
			if (bNew)
				CItem::ParseCriteria(Service.sItemCriteria, &pEntry->Criteria);

			pEntry->iImpact += iImpact;
			}
		}
	}

void CTradingEconomy::WriteToStream (IWriteStream *pStream) const

//	WriteToStream
//
//	DWORD			Count of m_CriteriaImpact
//	CString				Criteria
//	DWORD				iImpact
//
//	DWORD			Count of m_ItemTypeImpact
//	DWORD				UNID
//	DWORD				iImpact
//
//	DWORD			COunt of m_TradeImpact
//	CString				Criteria
//	DWORD				iImpact

	{
	int i;

	pStream->Write(m_CriteriaImpact.GetCount());
	for (i = 0; i < m_CriteriaImpact.GetCount(); i++)
		{
		m_CriteriaImpact.GetKey(i).WriteToStream(pStream);
		pStream->Write(m_CriteriaImpact[i].iImpact);
		}

	pStream->Write(m_ItemTypeImpact.GetCount());
	for (i = 0; i < m_ItemTypeImpact.GetCount(); i++)
		{
		pStream->Write(m_ItemTypeImpact.GetKey(i)->GetUNID());
		pStream->Write(m_ItemTypeImpact[i]);
		}

	pStream->Write(m_TradeImpact.GetCount());
	for (i = 0; i < m_TradeImpact.GetCount(); i++)
		{
		m_TradeImpact.GetKey(i).WriteToStream(pStream);
		pStream->Write(m_TradeImpact[i].iImpact);
		}
	}
