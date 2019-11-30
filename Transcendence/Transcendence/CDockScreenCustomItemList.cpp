//	CDockScreenCustomItemList.cpp
//
//	CDockScreenCustomItemList class
//	Copyright (c) 2014 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Transcendence.h"

ALERROR CDockScreenCustomItemList::OnInitList (SInitCtx &Ctx, const SDisplayOptions &Options, CString *retsError)

//	OnInitList
//
//	Initialize list

	{
	if (Ctx.pDockScreen == NULL)
		return ERR_FAIL;

	//	Get the list to show

	ICCItem *pExp = CCodeChain::Link(Options.sCode);

	//	Evaluate the function

	CCodeChainCtx CCCtx(GetUniverse());
	CCCtx.SetScreen(Ctx.pDockScreen);
	CCCtx.DefineContainingType(Ctx.pDockScreen->GetRoot());
	CCCtx.SaveAndDefineSourceVar(Ctx.pLocation);
	CCCtx.SaveAndDefineDataVar(Ctx.pData);

	ICCItem *pResult = CCCtx.Run(pExp);	//	LATER:Event
	CCCtx.Discard(pExp);

	if (pResult->IsError())
		{
		*retsError = pResult->GetStringValue();
		return ERR_FAIL;
		}

	//	We expect a list of item structures. Load them into an item list

	m_CustomItems.DeleteAll();
	for (int i = 0; i < pResult->GetCount(); i++)
		{
		ICCItem *pItem = pResult->GetElement(i);

		CCodeChainCtx CCCtx(Ctx.pDockScreen->GetUniverse());
		CItem NewItem = CCCtx.AsItem(pItem);
		if (NewItem.GetType() != NULL)
			m_CustomItems.AddItem(NewItem);
		}

	//	Done with result

	CCCtx.Discard(pResult);

	//	Make sure items are sorted

	m_CustomItems.SortItems();

	//	Set the list control

	m_pItemListControl->SetList(m_CustomItems);

	//	Position the cursor on the next relevant item

	SelectNextItem();

	return NOERROR;
	}
