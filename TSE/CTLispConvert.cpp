//	CTLispConvert.cpp
//
//	CTLispConvert class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define TYPE_SHIP_CLASS							CONSTLIT("shipClass")
#define TYPE_SPACE_OBJECT						CONSTLIT("spaceObject")

CTLispConvert::ETypes CTLispConvert::ArgType (ICCItem *pItem, ETypes iDefaultType, ICCItem **retpValue)

//	ArgType
//
//	Returns the type of argument. We expect a struct with a single entry whose
//	type is the field name.

	{
	if (pItem->IsNil())
		{
		if (retpValue) *retpValue = pItem;
		return typeNil;
		}
	else if (pItem->IsSymbolTable())
		{
		if (pItem->GetCount() == 0)
			{
			if (retpValue) *retpValue = pItem;
			return typeNil;
			}

		ETypes iType;
		CString sKey = pItem->GetKey(0);
		if (strEquals(sKey, TYPE_SHIP_CLASS))
			iType = typeShipClass;
		else if (strEquals(sKey, TYPE_SPACE_OBJECT))
			iType = typeSpaceObject;
		else
			iType = typeNil;

		if (retpValue) *retpValue = pItem->GetElement(0);
		return iType;
		}
	else
		{
		if (retpValue) *retpValue = pItem;
		return iDefaultType;
		}
	}
