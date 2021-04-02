//	CFireEventOrder.cpp
//
//	CFireEventOrder class implementation
//	Copyright (c) 2014 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CFireEventOrder::OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx)

//	OnBehavior
//
//	Do it

	{
	DEBUG_TRY

	//	Get the current order data (we need to make copies because we're about
	//	to delete the order).

	auto &OrderDesc = pShip->GetCurrentOrderDesc();
	CSpaceObject *pListener = OrderDesc.GetTarget();
	CString sEvent = OrderDesc.GetDataString();

	//	Done with the order.
	//
	//	NOTE: This will delete our object, so we can't access any member variables after this.

	pShip->CancelCurrentOrder();

	//	Fire the event

	if (pListener && !sEvent.IsBlank())
		pListener->FireCustomShipOrderEvent(sEvent, pShip);

	DEBUG_CATCH
	}

