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

