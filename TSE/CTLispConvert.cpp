//	CTLispConvert.cpp
//
//	CTLispConvert class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define TYPE_SHIP_CLASS							CONSTLIT("shipClass")
#define TYPE_SPACE_OBJECT						CONSTLIT("spaceObject")

const int IMAGE_UNID_INDEX =					0;
const int IMAGE_X_INDEX =						1;
const int IMAGE_Y_INDEX =						2;
const int IMAGE_ELEMENTS =						3;

const int IMAGE_WIDTH_INDEX =					3;
const int IMAGE_HEIGHT_INDEX =					4;
const int IMAGE_DESC_ELEMENTS =					5;

const int ICON_WIDTH =							96;
const int ICON_HEIGHT =							96;

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

DWORD CTLispConvert::AsImageDesc (ICCItem *pItem, RECT *retrcRect)

//	AsImageDesc
//
//	Returns an image UNID and rect from an image descriptor. 0 means we have no
//	image (or we have an error).

	{
	//	Get the image descriptor

	if (pItem->GetCount() < IMAGE_ELEMENTS)
		return 0;

	retrcRect->left = pItem->GetElement(IMAGE_X_INDEX)->GetIntegerValue();
	retrcRect->top = pItem->GetElement(IMAGE_Y_INDEX)->GetIntegerValue();

	//	See if we have a width/height

	if (pItem->GetCount() < IMAGE_DESC_ELEMENTS)
		{
		retrcRect->right = retrcRect->left + ICON_WIDTH;
		retrcRect->bottom = retrcRect->top + ICON_HEIGHT;
		}
	else
		{
		retrcRect->right = retrcRect->left + pItem->GetElement(IMAGE_WIDTH_INDEX)->GetIntegerValue();
		retrcRect->bottom = retrcRect->top + pItem->GetElement(IMAGE_HEIGHT_INDEX)->GetIntegerValue();
		}

	return pItem->GetElement(IMAGE_UNID_INDEX)->GetIntegerValue();
	}
