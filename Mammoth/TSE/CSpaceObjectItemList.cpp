//	CSpaceObjectItemList.cpp
//
//	CSpaceObject class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

bool CSpaceObject::AddItem (const CItem &Item, CItem *retResult, CString *retsError)

//	AddItem
//
//	Adds an item to the object. Returns TRUE if we were successful.

	{
	//	Notify any dock screens that we might modify an item
	//	Null item means preserve current selection.

	IDockScreenUI::SModifyItemCtx ModifyCtx;
	OnModifyItemBegin(ModifyCtx, CItem());

	//	Add the item

	CItemListManipulator ObjList(GetItemList());
	ObjList.AddItem(Item);

	//	Return the item.

	if (retResult)
		*retResult = ObjList.GetItemAtCursor();

	//	Update the object

	OnComponentChanged(comCargo);
	OnModifyItemComplete(ModifyCtx, Item);

	return true;
	}

ALERROR CSpaceObject::CreateRandomItems (IItemGenerator *pItems, CSystem *pSystem)

//	CreateRandomItems
//
//	Creates items based on item table
//
//	NOTE: We cannot call GetSystem() because it is not yet set up at the time
//	that we call this (on create). Thus we rely on pSystem being passed in.

	{
	DEBUG_TRY

	//	Out of plane objects don't get inventory; this handles the case of out
	//	of plane asteroids getting ore.

	if (pItems 
			&& !IsOutOfPlaneObj())
		{
		//	Notify any dock screens that we might modify an item
		//	Null item means preserve current selection.

		IDockScreenUI::SModifyItemCtx ModifyCtx;
		OnModifyItemBegin(ModifyCtx, CItem());

		//	Add the items

		CItemListManipulator ItemList(GetItemList());
		SItemAddCtx Ctx(ItemList);
		Ctx.pSystem = pSystem;
		Ctx.pDest = this;
		Ctx.vPos = GetPos();
		Ctx.iLevel = (Ctx.pSystem ? Ctx.pSystem->GetLevel() : 1);

		pItems->AddItems(Ctx);

		//	Update the object

		OnComponentChanged(comCargo);
		OnModifyItemComplete(ModifyCtx, CItem());
		}

	return NOERROR;

	DEBUG_CATCH
	}

ALERROR CSpaceObject::CreateRandomItems (CXMLElement *pItems, CSystem *pSystem)

//	CreateRandomItems
//
//	Creates items based on lookup

	{
	ALERROR error;

	//	Load the table

	SDesignLoadCtx Ctx;

	IItemGenerator *pGenerator;
	if (error = IItemGenerator::CreateFromXML(Ctx, pItems, &pGenerator))
		return error;

	if (error = pGenerator->OnDesignLoadComplete(Ctx))
		{
		CString sError = strPatternSubst(CONSTLIT("ERROR: Unable to create random items: %s\r\n"), Ctx.sError);
		ReportCrashObj(&sError, this);
		kernelDebugLogString(sError);
		ASSERT(false);
		return error;
		}

	//	Add the items

	CreateRandomItems(pGenerator, pSystem);

	//	Done

	delete pGenerator;
	return NOERROR;
	}

CItem CSpaceObject::GetItemForDevice (CInstalledDevice *pDevice)

//	GetItemForDevice
//
//	Returns the item for the given device

	{
	CItemListManipulator ItemList(GetItemList());
	SetCursorAtDevice(ItemList, pDevice);
	return ItemList.GetItemAtCursor();
	}

CMenuData CSpaceObject::GetUsableItems (const SUsableItemOptions &Options) const

//	GetUsableItems
//
//	Returns a list of items to use appropriate for showing to the UI.
//
//	The dwData member is the index of the item in the ItemList; thus we rely on
//	the fact that the list does not change while the menu is up.

	{
	CMenuData MenuData;

	const CItemList &List = GetItemList();

	//	Generate a sorted list of items

	TSortMap<CString, int> SortedList;
	for (int i = 0; i < List.GetCount(); i++)
		{
		const CItem &Item = List.GetItem(i);
		CItemType *pType = Item.GetType();

		//	See if we can use this item, and get the use key

		CString sUseKey;
		if (!Item.CanBeUsed(&sUseKey))
			continue;

		//	Add to the list

		bool bHasUseKey = (Item.IsKnown() && !sUseKey.IsBlank() && (*sUseKey.GetASCIIZPointer() != Options.chUseKey));

		//	Any items without use keys sort first (so that they are easier
		//	to access).
		//
		//	Then we sort by level (higher-level first)
		//
		//	Then we sort by natural order
		//
		//	For items that use charges, we expand if there are multiple

		if (pType->ShowChargesInUseMenu() && Item.IsKnown())
			{
			for (int j = 0; j < Item.GetCount(); j++)
				{
				SortedList.Insert(strPatternSubst(CONSTLIT("%d%s%04d%04d"),
							(bHasUseKey ? 1 : 0),
							(bHasUseKey ? strPatternSubst(CONSTLIT("%s0"), sUseKey) : strPatternSubst(CONSTLIT("%02d"), MAX_ITEM_LEVEL - Item.GetLevel())),
							i, j),
						i);
				}
			}
		else
			SortedList.Insert(strPatternSubst(CONSTLIT("%d%s%04d%04d"),
						(bHasUseKey ? 1 : 0),
						(bHasUseKey ? strPatternSubst(CONSTLIT("%s0"), sUseKey) : strPatternSubst(CONSTLIT("%02d"), MAX_ITEM_LEVEL - Item.GetLevel())),
						i, 0),
					i);
		}

	//	Now add all the items to the menu

	for (int i = 0; i < SortedList.GetCount(); i++)
		{
		CString sSort = SortedList.GetKey(i);
		const CItem &Item = List.GetItem(SortedList.GetValue(i));
		CItemType *pType = Item.GetType();

		CItemType::SUseDesc UseDesc;
		if (!pType->GetUseDesc(&UseDesc))
			continue;

		int iCount;
		if (pType->ShowChargesInUseMenu() && Item.IsKnown())
			iCount = Item.GetCharges();
		else if (Item.GetCount() > 1)
			iCount = Item.GetCount();
		else
			iCount = 0;

		//	Installed

		CString sExtra;
		if (Item.IsInstalled())
			sExtra = CONSTLIT("Installed");

		//	Show the key only if the item is identified

		CString sKey;
		if (Item.IsKnown() && (*UseDesc.sUseKey.GetASCIIZPointer() != Options.chUseKey))
			sKey = UseDesc.sUseKey;

		//	Name of item

		CString sName = Item.GetNounPhrase();
		sName = strPatternSubst(CONSTLIT("Use %s"), sName);

		//	Add the item

		MenuData.AddMenuItem(NULL_STR,
				sKey,
				sName,
				&pType->GetImage(),
				iCount,
				sExtra,
				NULL_STR,
				0,
				SortedList.GetValue(i));
		}

	//	Done!

	return MenuData;
	}

bool CSpaceObject::RemoveItem (const CItem &Item, DWORD dwItemMatchFlags, int iCount, int *retiCountRemoved, CString *retsError)

//	RemoveItem
//
//	Removes the given item.

	{
	if (retiCountRemoved) *retiCountRemoved = 0;

	if (iCount == -1)
		iCount = Item.GetCount();

	//	If not found, then we succeed, but leave count removed as 0.

	CItemListManipulator ObjList(GetItemList());
	if (!ObjList.SetCursorAtItem(Item, dwItemMatchFlags))
		return true;

	//	Can't be installed

	if (ObjList.GetItemAtCursor().IsInstalled())
		{
		if (retsError) *retsError = CONSTLIT("Installed items cannot be removed");
		return false;
		}

	//	Notify any dock screens that we might modify an item

	IDockScreenUI::SModifyItemCtx ModifyCtx;
	OnModifyItemBegin(ModifyCtx, ObjList.GetItemAtCursor());

	//	Remove

	if (retiCountRemoved) *retiCountRemoved = Min(ObjList.GetItemAtCursor().GetCount(), iCount);
	ObjList.DeleteAtCursor(iCount);

	//	Update (empty item indicates that we deleted some items).

	OnComponentChanged(comCargo);
	OnModifyItemComplete(ModifyCtx, CItem());

	return true;
	}

bool CSpaceObject::SetItemData (const CItem &Item, const CString &sField, ICCItem *pValue, int iCount, CItem *retItem, CString *retsError)

//	SetItemData
//
//	Sets the item data. pValue may be NULL, indicating that we should delete the
//	value.

	{
	//	Select the item

	CItemListManipulator ItemList(GetItemList());
	if (!ItemList.SetCursorAtItem(Item))
		{
		*retsError = CONSTLIT("Item not found on object.");
		return false;
		}

	//	Notify any dock screens that we might modify an item

	IDockScreenUI::SModifyItemCtx ModifyCtx;
	OnModifyItemBegin(ModifyCtx, Item);

	//	Set the data

	ItemList.SetDataAtCursor(sField, pValue, iCount);

	//	Return the newly changed item. We do this before the notification 
	//	because the notification might change the underlying item list (because
	//	it sorts).

	if (retItem)
		*retItem = ItemList.GetItemAtCursor();

	//	Update the object

	OnModifyItemComplete(ModifyCtx, ItemList.GetItemAtCursor());

	return true;
	}

