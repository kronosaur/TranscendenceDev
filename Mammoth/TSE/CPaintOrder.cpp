//	CPaintOrder.cpp
//
//	CPaintOrder class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define ORDER_BRING_TO_FRONT					CONSTLIT("bringToFront")
#define ORDER_OVERHANG							CONSTLIT("overhang")
#define ORDER_SEND_TO_BACK						CONSTLIT("sendToBack")

CPaintOrder::Types CPaintOrder::Parse (const CString &sValue)

//	Parse
//
//	Parses a paint order string. Returns CPaintOrder::error if we cannot parse.

	{
	if (sValue.IsBlank())
		return none;
	else if (strEquals(sValue, ORDER_BRING_TO_FRONT))
		return bringToFront;
	else if (strEquals(sValue, ORDER_OVERHANG))
		return paintOverhang;
	else if (strEquals(sValue, ORDER_SEND_TO_BACK))
		return sendToBack;
	else
		return error;
	}

CString CPaintOrder::GetID (Types iType)

//	GetID
//
//	Returns the ID for the given paint order.

	{
	switch (iType)
		{
		case bringToFront:
			return ORDER_BRING_TO_FRONT;

		case paintOverhang:
			return ORDER_OVERHANG;

		case sendToBack:
			return ORDER_SEND_TO_BACK;

		default:
			return NULL_STR;
		}
	}
