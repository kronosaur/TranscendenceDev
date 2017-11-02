//	CTradingEconomy.cpp
//
//	CTradingEconomy class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CString CTradingEconomy::CalcImpactDesc (int iImpact) const

//	CalcImpactDesc
//
//	Returns a description of the impact.

	{
	CString sDesc;

	if (iImpact < -50)
		sDesc = CONSTLIT("massive glut of");
	else if (iImpact < -30)
		sDesc = CONSTLIT("major glut of");
	else if (iImpact < -15)
		sDesc = CONSTLIT("glut of");
	else if (iImpact < 0)
		sDesc = CONSTLIT("exports");
	else if (iImpact == 0)
		sDesc = CONSTLIT("balanced use of");
	else if (iImpact <= 10)
		sDesc = CONSTLIT("imports");
	else if (iImpact <= 25)
		sDesc = CONSTLIT("shortage of");
	else if (iImpact <= 50)
		sDesc = CONSTLIT("major shortage of");
	else
		sDesc = CONSTLIT("desperate shortage of");

	if (g_pUniverse->InDebugMode())
		return strPatternSubst(CONSTLIT("%s (%d)"), sDesc, iImpact);
	else
		return sDesc;
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

	//	Generate separate descriptions for each price adjustment and sort by
	//	type and size of adjustment.

	TSortMap<CString, CString> List(DescendingSort);

	//	Start by adding individual item prices. Note that these already include any 
	//	impact from item criteria.

	for (i = 0; i < m_ItemTypeImpact.GetCount(); i++)
		{
		int iImpact = m_ItemTypeImpact[i];
		if (iImpact == 0)
			continue;

		CString sName = m_ItemTypeImpact.GetKey(i)->GetNounPhrase(nounShort);
		CString sDesc = strPatternSubst(CONSTLIT("%s %s"), CalcImpactDesc(iImpact), sName);

		List.SetAt(CalcImpactSortKey(iImpact, sName), sDesc);
		}

	//	Now add item criteria

	for (i = 0; i < m_CriteriaImpact.GetCount(); i++)
		{
		const SCriteriaEntry &Entry = m_CriteriaImpact[i];
		if (Entry.iImpact == 0)
			continue;

		CString sName = Entry.Criteria.GetName();
		if (sName.IsBlank())
			continue;

		CString sDesc = strPatternSubst(CONSTLIT("%s %s"), CalcImpactDesc(Entry.iImpact), sName);

		List.SetAt(CalcImpactSortKey(Entry.iImpact, sName), sDesc);
		}

	//	Short-circuit

	if (List.GetCount() == 0)
		return NULL_STR;

	//	Concatenate them all together

	CString sResult = strCapitalize(List[0]);
	for (i = 1; i < List.GetCount(); i++)
		{
		sResult.Append(CONSTLIT(", "));
		sResult.Append(List[i]);
		}

	//	Done

	return sResult;
	}

bool CTradingEconomy::FindBuyPriceAdj (CItemType *pItem, int *retiAdj) const

//	FindBuyPriceAdj
//
//	Returns the price adjustment for the given item when a station is BUYING the
//	item from the player. Returns FALSE if there is no adjustment (normal price).

	{
	return false;
	}

bool CTradingEconomy::FindSellPriceAdj (CItemType *pItem, int *retiAdj) const

//	FindSellPriceAdj
//
//	Returns the price adjustment for the given item when a station is SELLING the
//	item to the player. Returns FALSE if there is no adjustment (normal price).

	{
	return false;
	}

int CTradingEconomy::GetPriceImpact (CItemType *pItem) const

//	GetPriceImpact
//
//	Returns the price impact of the given item. Positive numbers mean the price 
//	increases (due to demand); negative numbers mean the price decreases due
//	to supply. 0 means no change in price.

	{
	return 0;
	}

void CTradingEconomy::ReadFromStream (SUniverseLoadCtx &Ctx)

//	ReadFromStream
//
//	

	{
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
		if (pTrade)
			RefreshFromTradeDesc(pSystem, pTrade);

		pTrade = pObj->GetTradeDescOverride();
		if (pTrade)
			RefreshFromTradeDesc(pSystem, pTrade);
		}
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

		int iImpact = 0;
		if (Service.iService == serviceConsume)
			{
			//	If we're consuming, price is increased in this sytem due
			//	to demand.

			iImpact = Service.iPriceAdj;
			}
		else if (Service.iService == serviceProduce)
			{
			//	If we're producing, price is reduced in this system due to 
			//	supply.

			iImpact = -Service.iPriceAdj;
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
			SCriteriaEntry *pEntry = m_CriteriaImpact.SetAt(Service.sItemCriteria, &bNew);
			if (bNew)
				CItem::ParseCriteria(Service.sItemCriteria, &pEntry->Criteria);

			pEntry->iImpact += iImpact;
			}
		}
	}

void CTradingEconomy::WriteToStream (IWriteStream *pStream) const

//	WriteToStream
//
//

	{
	}
