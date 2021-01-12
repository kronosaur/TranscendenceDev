//	CSendMessageOrder.cpp
//
//	CSendMessageOrder class implementation
//	Copyright (c) 2014 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CSendMessageOrder::OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx)

//	OnBehavior
//
//	Do it

	{
	//	Get the current order data

	const COrderDesc &OrderDesc = pShip->GetCurrentOrderDesc();
	CSpaceObject *pRecipient = OrderDesc.GetTarget();

	//	Send the message

	if (pRecipient && !OrderDesc.GetDataString().IsBlank())
		pRecipient->SendMessage(pShip, OrderDesc.GetDataString());

	//	Done with the order.

	pShip->CancelCurrentOrder();
	}

