//	CTLispConvert.cpp
//
//	CTLispConvert class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define FIELD_DEVICE_SLOT						CONSTLIT("deviceSlot")
#define FIELD_SLOT_ID							CONSTLIT("slotID")
#define FIELD_SLOT_POS_INDEX					CONSTLIT("slotPosIndex")

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

DWORD CTLispConvert::AsImageDesc (const ICCItem *pItem, RECT *retrcRect)

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

CSpaceObject *CTLispConvert::AsObject (const ICCItem *pItem)

//	AsObject
//
//	Converts to object.

	{
	if (pItem)
		return ::CreateObjFromItem(pItem, CCUTIL_FLAG_CHECK_DESTROYED);
	else
		return NULL;
	}

CG32bitPixel CTLispConvert::AsRGB (const ICCItem *pItem, CG32bitPixel rgbDefault)

//	AsRGB
//
//	Converts to an RGB value.

	{
	if (!pItem || pItem->IsNil())
		return rgbDefault;
	else
		return ::LoadRGBColor(pItem->GetStringValue(), rgbDefault);
	}

bool CTLispConvert::AsScreenSelector (ICCItem *pItem, CDockScreenSys::SSelector *retSelector)

//	AsScreen
//
//	Converts pItem into a dock screen call. Returns TRUE if we have a valid dock
//	screen reference.

	{
	//	Load

	if (pItem->IsNil())
		return false;

	else if (pItem->IsSymbolTable())
		{
		if (retSelector)
			{
			*retSelector = CDockScreenSys::SSelector();
			
			retSelector->sScreen = pItem->GetStringAt(CONSTLIT("screen"));
			retSelector->iPriority = pItem->GetIntegerAt(CONSTLIT("priority"), 0);
			retSelector->bOverrideOnly = pItem->GetBooleanAt(CONSTLIT("overrideOnly"));

			//	If we don't have a data element, then we treat the selector item
			//	as data. This saves us from having an extra data struct.

			ICCItem *pData = pItem->GetElement(CONSTLIT("data"));
			if (pData == NULL)
				retSelector->pData = ICCItemPtr(pItem->Reference());
			else
				retSelector->pData = ICCItemPtr(pData->Reference());
			}

		return true;
		}

	else if (pItem->IsList() && pItem->GetCount() >= 2)
		{
		if (retSelector)
			{
			*retSelector = CDockScreenSys::SSelector();

			retSelector->sScreen = pItem->GetElement(0)->GetStringValue();
			if (pItem->GetCount() >= 3)
				{
				retSelector->pData = ICCItemPtr(pItem->GetElement(1)->Reference());
				retSelector->iPriority = pItem->GetElement(2)->GetIntegerValue();
				}
			else
				{
				retSelector->iPriority = pItem->GetElement(1)->GetIntegerValue();
				}
			}

		return true;
		}
	else if (pItem->GetCount() > 0)
		{
		if (retSelector)
			{
			*retSelector = CDockScreenSys::SSelector();
			retSelector->sScreen = pItem->GetElement(0)->GetStringValue();
			}

		return true;
		}
	else
		return false;
	}

bool CTLispConvert::AsSlotDesc (const ICCItem &Value, CDeviceSystem::SSlotDesc &retSlot)

//	AsSlotDesc
//
//	Interprets the value as a SSlotDesc. Return TRUE if no error.

	{
	retSlot = CDeviceSystem::SSlotDesc();

	if (Value.IsNil())
		{ }
	else if (Value.IsInteger())
		{
		retSlot.iIndex = Value.GetIntegerValue();
		}
	else if (Value.IsIdentifier())
		{
		retSlot.sID = Value.GetStringValue();
		}
	else if (Value.IsSymbolTable())
		{
		retSlot.iIndex = Value.GetIntegerAt(FIELD_DEVICE_SLOT, -1);
		retSlot.sID = Value.GetStringAt(FIELD_SLOT_ID);
		retSlot.iPos = Value.GetIntegerAt(FIELD_SLOT_POS_INDEX, -1);
		}
	else
		return false;

	return true;
	}

CVector CTLispConvert::AsVector (const ICCItem *pItem)

//	AsVector
//
//	Converts to vector (or null).

	{
	if (!pItem)
		return NullVector;
	else if (pItem->IsList())
		{
		CVector vVec;
		CreateBinaryFromList(NULL_STR, *pItem, &vVec);
		return vVec;
		}
	else if (pItem->IsInteger())
		{
		CSpaceObject *pObj = CreateObjFromItem(pItem);
		if (pObj)
			return pObj->GetPos();
		}

	return NullVector;
	}

ICCItemPtr CTLispConvert::CreateCurrencyValue (CurrencyValue Value)

//	CreateCurrencyValue
//
//	Creates a currency value

	{
	if (Value > (CurrencyValue)0x7fffffff)
		return ICCItemPtr((double)Value);
	else
		return ICCItemPtr((int)Value);
	}

ICCItemPtr CTLispConvert::CreateItem (const CItem &Value)

//	CreateItem
//
//	Creates an item value.

	{
	if (!Value.IsEmpty())
		return ICCItemPtr(::CreateListFromItem(Value));
	else
		return ICCItemPtr::Nil();
	}

ICCItemPtr CTLispConvert::CreateObject (const CSpaceObject *pObj)

//	CreateObject
//
//	Returns an object pointer.

	{
	if (pObj)
		return ICCItemPtr((int)pObj);
	else
		return ICCItemPtr::Nil();
	}

ICCItemPtr CTLispConvert::CreateObjectList (const CSpaceObjectList &List)

//	CreateObjectList
//
//	Returns a list of object IDs

	{
	if (List.GetCount() == 0)
		return ICCItemPtr(ICCItem::Nil);

	else if (List.GetCount() == 1)
		return CreateObject(List.GetObj(0));
	else
		{
		ICCItemPtr pResult(ICCItem::List);

		for (int i = 0; i < List.GetCount(); i++)
			pResult->Append(CreateObject(List.GetObj(i)));

		return pResult;
		}
	}

ICCItemPtr CTLispConvert::CreatePowerResultMW (int iPower)

//	CreatePowerResultMW
//
//	Returns power in MWs. iPower is the power in 1/10th MWs.

	{
	if (iPower % 10)
		return ICCItemPtr((double)iPower / 10.0);
	else
		return ICCItemPtr(iPower / 10);
	}

ICCItemPtr CTLispConvert::CreateIntegerList (const TArray<int> &List)

//	CreateIntegerList
//
//	Creates a list of integers.

	{
	if (List.GetCount() == 0)
		return ICCItemPtr::Nil();

	ICCItemPtr pResult(ICCItem::List);
	for (int i = 0; i < List.GetCount(); i++)
		pResult->Append(ICCItemPtr(List[i]));

	return pResult;
	}

ICCItemPtr CTLispConvert::CreateStringList (const TArray<CString> &List)

//	CreateStringList
//
//	Creates a list of strings.

	{
	if (List.GetCount() == 0)
		return ICCItemPtr::Nil();

	ICCItemPtr pResult(ICCItem::List);
	for (int i = 0; i < List.GetCount(); i++)
		pResult->Append(ICCItemPtr(List[i]));

	return pResult;
	}

ICCItemPtr CTLispConvert::CreateVector (const CVector &vValue)

//	CreateVector
//
//	Create vector value.

	{
	return ICCItemPtr(::CreateListFromVector(vValue));
	}

ICCItemPtr CTLispConvert::GetElementAt (ICCItem *pItem, const CString &sField)

//	GetElementAt
//
//	Returns a field in a struct.

	{
	if (pItem == NULL)
		return ICCItemPtr();

	ICCItem *pResult = pItem->GetElement(sField);
	if (pResult == NULL)
		return ICCItemPtr();
	else
		return ICCItemPtr(pResult->Reference());
	}
