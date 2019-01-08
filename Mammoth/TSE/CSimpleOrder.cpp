//	CSimpleOrder.cpp
//
//	Implementation of CSimpleOrder class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CSimpleOrder::OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx)

//	OnBehavior
//
//	Carry out order.

	{
	DEBUG_TRY

	switch (m_iOrder)
		{
		case IShipController::orderFireWeapon:
			OrderFireWeapon(pShip, Ctx);
			break;

		case IShipController::orderUseItem:
			OrderUseItem(pShip, Ctx);
			break;

		default:
			ASSERT(false);
			pShip->CancelCurrentOrder();
			break;
		}

	DEBUG_CATCH
	}

void CSimpleOrder::OrderFireWeapon (CShip *pShip, CAIBehaviorCtx &Ctx) const

//	OrderFireWeapon
//
//	Fire weapon.

	{
	//	Get the current order data

	CSpaceObject *pTarget;
	IShipController::SData Data;
	pShip->GetCurrentOrder(&pTarget, &Data);
	const CItem &WeaponItem = Data.AsItem();

	//	Select the specified weapon.

	DeviceNames iDev = pShip->SelectWeapon(WeaponItem);
	CInstalledDevice *pWeapon;
	if (iDev == devNone
			|| (pWeapon = pShip->GetNamedDevice(iDev)) == NULL)
		{
		pShip->CancelCurrentOrder();
		return;
		}

	//	If the weapon is not yet ready, then wait

	if (!pWeapon->IsReady())
		return;

	//	Fire the weapon

	pShip->SetWeaponTriggered(pWeapon);

	//	Done

	pShip->CancelCurrentOrder();
	}

void CSimpleOrder::OrderUseItem (CShip *pShip, CAIBehaviorCtx &Ctx) const

//	OrderUseItem
//
//	Use the given item.

	{
	//	Get the current order data

	CSpaceObject *pTarget;
	IShipController::SData Data;
	pShip->GetCurrentOrder(&pTarget, &Data);
	const CItem &Item = Data.AsItem();

	//	Validate the item

	CItemType *pType = Item.GetType();
	if (pType == NULL)
		{
		pShip->CancelCurrentOrder();
		return;
		}

	//	Make sure the item is on the ship

	CItemListManipulator Items(pShip->GetItemList());
	if (!Items.SetCursorAtItem(Item))
		{
		pShip->CancelCurrentOrder();
		return;
		}

	//	Make sure the item can be used.

	CItemType::SUseDesc UseDesc;
	if (!pType->GetUseDesc(&UseDesc)
			|| !UseDesc.bUsableInCockpit)
		{
		pShip->CancelCurrentOrder();
		return;
		}

	//	Use the item

	CString sError;
	if (!pShip->UseItem(Item, &sError))
		{
		::kernelDebugLogPattern("[%s %s Invoke]: %s", pShip->GetNounPhrase(), pType->GetNounPhrase(), sError);
		}

	//	Done

	pShip->CancelCurrentOrder();
	}
