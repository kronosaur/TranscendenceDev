//	CItemInfoTable.cpp
//
//	CItemInfoTable class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CItemInfoTable::AddItem (const CItem &Item)

//	AddItem
//
//	Adds an item to the table.

	{
	const CItemType *pType = Item.GetType();
	if (pType == NULL)
		return;

	bool bNew;
	SItemInfo *pEntry = m_Table.SetAt(pType->GetUNID(), &bNew);
	if (bNew)
		{
		pEntry->pType = pType;
		}

	pEntry->iTotalCount += Item.GetCount();
	pEntry->iInstanceCount += 1;
	}
