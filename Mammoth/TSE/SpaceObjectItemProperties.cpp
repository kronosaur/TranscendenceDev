//	SpaceObjectItemProperties.cpp
//
//	CSpaceObject class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define FIELD_DESC_ID							CONSTLIT("descID")
#define FIELD_CAN_INSTALL						CONSTLIT("canInstall")
#define FIELD_CAN_REMOVE						CONSTLIT("canRemove")
#define FIELD_PRICE								CONSTLIT("price")
#define FIELD_UPGRADE_INSTALL_ONLY				CONSTLIT("upgradeInstallOnly")

#define PROPERTY_CAN_BE_FIRED					CONSTLIT("canBeFired")
#define PROPERTY_DAMAGED						CONSTLIT("damaged")
#define PROPERTY_ENABLED						CONSTLIT("enabled")
#define PROPERTY_HP								CONSTLIT("hp")
#define PROPERTY_INSTALL_DEVICE_PRICE			CONSTLIT("installDevicePrice")
#define PROPERTY_INSTALL_DEVICE_STATUS			CONSTLIT("installDeviceStatus")
#define PROPERTY_INSTALL_ITEM_STATUS			CONSTLIT("installItemStatus")
#define PROPERTY_REMOVE_DEVICE_PRICE			CONSTLIT("removeDevicePrice")
#define PROPERTY_REMOVE_ITEM_STATUS				CONSTLIT("removeItemStatus")

ICCItemPtr CSpaceObject::GetItemProperty (CCodeChainCtx &CCX, const CItem &Item, const CString &sName) const

//	GetItemProperty
//
//	Returns an item property

	{
	//	Return the property

	if (strEquals(sName, PROPERTY_CAN_BE_FIRED))
		{
		CMissileItem MissileItem = Item.AsMissileItem();
		if (!MissileItem)
			return ICCItemPtr::Nil();

		TArray<CItem> Launchers = MissileItem.GetLaunchWeapons();

		const CDeviceSystem &Devices = GetDeviceSystem();
		for (CDeviceItem DeviceItem : Devices)
			{
			for (int i = 0; i < Launchers.GetCount(); i++)
				if (Launchers[i].GetType() == &DeviceItem.GetType())
					return ICCItemPtr::True();
			}

		return ICCItemPtr::Nil();
		}
	else if (strEquals(sName, PROPERTY_INSTALL_DEVICE_PRICE))
		{
		CTradingServices Services(*this);
		int iPrice;
		if (!Services.GetDeviceInstallPrice(Item.AsDeviceItem(), 0, &iPrice))
			return ICCItemPtr::Nil();

		return ICCItemPtr(iPrice);
		}
	else if (strEquals(sName, PROPERTY_INSTALL_DEVICE_STATUS))
		{
		CTradingServices Services(*this);

		//	We return a structure with the following fields:
		//
		//	canInstall: True or Nil
		//	price: Install price
		//	descID: Message ID for description of install attempt
		//	upgradeInstallOnly: True if we only install on upgrade

		CString sMessageID;
		int iPrice;
		DWORD dwPriceFlags;

		bool bCanInstall = Services.GetDeviceInstallPrice(Item.AsDeviceItem(), 0, &iPrice, &sMessageID, &dwPriceFlags);

		//	Create the structure

		ICCItemPtr pResult = ICCItemPtr(ICCItem::SymbolTable);
		pResult->SetBooleanAt(FIELD_CAN_INSTALL, bCanInstall);
		pResult->SetIntegerAt(FIELD_PRICE, (bCanInstall ? iPrice : -1));
		if (bCanInstall && (dwPriceFlags & CTradingDesc::PRICE_UPGRADE_INSTALL_ONLY))
			pResult->SetBooleanAt(FIELD_UPGRADE_INSTALL_ONLY, true);

		if (!sMessageID.IsBlank())
			pResult->SetStringAt(FIELD_DESC_ID, sMessageID);

		return pResult;
		}
	else if (strEquals(sName, PROPERTY_INSTALL_ITEM_STATUS))
		{
		CTradingServices Services(*this);

		//	We return a structure with the following fields:
		//
		//	canInstall: True or Nil
		//	price: Install price
		//	descID: Message ID for description of install attempt
		//	upgradeInstallOnly: True if we only install on upgrade

		CString sMessageID;
		int iPrice;
		DWORD dwPriceFlags;

		bool bCanInstall = Services.GetItemInstallPrice(Item, 0, &iPrice, &sMessageID, &dwPriceFlags);

		//	Create the structure

		ICCItemPtr pResult = ICCItemPtr(ICCItem::SymbolTable);
		pResult->SetBooleanAt(FIELD_CAN_INSTALL, bCanInstall);
		pResult->SetIntegerAt(FIELD_PRICE, (bCanInstall ? iPrice : -1));
		if (bCanInstall 
				&& (dwPriceFlags & CTradingDesc::PRICE_UPGRADE_INSTALL_ONLY))
			pResult->SetBooleanAt(FIELD_UPGRADE_INSTALL_ONLY, true);

		//	NOTE: Message is valid even if we cannot install

		if (!sMessageID.IsBlank())
			pResult->SetStringAt(FIELD_DESC_ID, sMessageID);

		return pResult;
		}
	else if (strEquals(sName, PROPERTY_REMOVE_DEVICE_PRICE))
		{
		CTradingServices Services(*this);

		int iPrice;
		if (!Services.GetDeviceRemovePrice(Item.AsDeviceItem(), 0, &iPrice))
			return ICCItemPtr::Nil();

		return ICCItemPtr(iPrice);
		}
	else if (strEquals(sName, PROPERTY_REMOVE_ITEM_STATUS))
		{
		CTradingServices Services(*this);

		//	We return a structure with the following fields:
		//
		//	canRemove: True or Nil
		//	price: Remove price
		//	descID: Message ID for description of remove attempt
		//	upgradeInstallOnly: True if we only install/remove on upgrade

		CString sMessageID;
		int iPrice;
		DWORD dwPriceFlags = 0;

		bool bCanRemove = Services.GetItemRemovePrice(Item, 0, &iPrice, &dwPriceFlags);

		//	Create the structure

		ICCItemPtr pResult = ICCItemPtr(ICCItem::SymbolTable);
		pResult->SetBooleanAt(FIELD_CAN_REMOVE, bCanRemove);
		pResult->SetIntegerAt(FIELD_PRICE, (bCanRemove ? iPrice : -1));
		if (bCanRemove 
				&& (dwPriceFlags & CTradingDesc::PRICE_UPGRADE_INSTALL_ONLY))
			pResult->SetBooleanAt(FIELD_UPGRADE_INSTALL_ONLY, true);

		if (!sMessageID.IsBlank())
			pResult->SetStringAt(FIELD_DESC_ID, sMessageID);

		return pResult;
		}
	else
		{
		//	Select the item (to make sure that it is part of the object)
		//
		//	LATER: We need a const-version of CItemListManupulator

		CItemListManipulator ItemList(const_cast<CItemList &>(GetItemList()));
		if (!ItemList.SetCursorAtItem(Item))
			return ICCItemPtr::Error(CONSTLIT("Item not found on object."));

		//	LATER: Eventually we will get rid of CItemCtx.

		CItemCtx Ctx(&Item, const_cast<CSpaceObject *>(this));
		return ICCItemPtr(Item.GetItemProperty(CCX, Ctx, sName, false));
		}
	}

bool CSpaceObject::IncItemProperty (const CItem &Item, const CString &sProperty, const ICCItem &Value, int iCount, CItem *retItem, ICCItemPtr *retNewValue, CString *retsError)

//	IncItemProperty
//
//	Increments an item property.

	{
	CCodeChainCtx CCX(GetUniverse());

	ICCItemPtr pCurValue = GetItemProperty(CCX, Item, sProperty);
	ICCItemPtr pNewValue = CCodeChain::IncValue(pCurValue, &Value);
	if (pNewValue->IsError())
		{
		if (retsError) *retsError = pNewValue->GetStringValue();
		return false;
		}

	if (!SetItemProperty(Item, sProperty, pNewValue, iCount, retItem, retsError))
		return false;

	if (retNewValue)
		*retNewValue = pNewValue;

	return true;
	}

bool CSpaceObject::SetItemProperty (const CItem &Item, const CString &sName, ICCItem *pValue, int iCount, CItem *retItem, CString *retsError)

//	SetItemProperty
//
//	Sets the item property.
//
//	NOTE: pValue may be NULL.

	{
	//	Select the item to make sure it exists on this object.

	CItemListManipulator ItemList(GetItemList());
	if (!ItemList.SetCursorAtItem(Item))
		{
		if (retsError) *retsError = CONSTLIT("Item not found on object.");
		return false;
		}

	//	Notify any dock screens that we might modify an item

	IDockScreenUI::SModifyItemCtx ModifyCtx;
	OnModifyItemBegin(ModifyCtx, Item);

	//	We handle damage differently because we may need to remove enhancements,
	//	etc.

	if (strEquals(sName, PROPERTY_DAMAGED))
		{
		if (pValue && pValue->IsNil())
			RepairItem(ItemList);
		else
			DamageItem(ItemList);
		}

	//	Enabling/disabling needs special handling

	else if (strEquals(sName, PROPERTY_ENABLED))
		{
		CShip *pShip = AsShip();
		if (pShip == NULL)
			{
			if (retsError) *retsError = CONSTLIT("Property not supported.");
			return false;
			}

		if (!ItemList.GetItemAtCursor().IsInstalled())
			{
			if (retsError) *retsError = CONSTLIT("Device is not installed on object.");
			return false;
			}

		//	Parse the parameter

		bool bEnabled;
		bool bSilent;
		if (pValue == NULL)
			{
			bEnabled = true;
			bSilent = false;
			}
		else if (pValue->IsIdentifier())
			{
			if (strEquals(pValue->GetStringValue(), CONSTLIT("silentDisabled")))
				{
				bEnabled = false;
				bSilent = true;
				}
			else if (strEquals(pValue->GetStringValue(), CONSTLIT("silentEnabled")))
				{
				bEnabled = true;
				bSilent = true;
				}
			else if (strEquals(pValue->GetStringValue(), CONSTLIT("disabled")))
				{
				bEnabled = false;
				bSilent = false;
				}
			else
				{
				bEnabled = true;
				bSilent = false;
				}
			}
		else
			{
			bEnabled = !pValue->IsNil();
			bSilent = false;
			}

		//	Do it.

		pShip->EnableDevice(ItemList.GetItemAtCursor().GetInstalled(), bEnabled, bSilent);
		}

	//	Otherwise, just set the property, but pass enough context (this object)
	//	so that it can find the appropriate device.

	else
		{
		//	Set the data

		if (!ItemList.SetPropertyAtCursor(this, sName, pValue, iCount, retsError))
			return false;

		//	Update the object

		ItemEnhancementModified(ItemList);
		}

	//	Return the newly changed item. We do this before the notification 
	//	because the notification might change the underlying item list (because
	//	it sorts).

	if (retItem)
		*retItem = ItemList.GetItemAtCursor();

	//	Update the object

	OnModifyItemComplete(ModifyCtx, ItemList.GetItemAtCursor());

	return true;
	}
