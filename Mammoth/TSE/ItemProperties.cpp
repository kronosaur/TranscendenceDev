//	ItemProperties.cpp
//
//	CItem class
//	Copyright (c) 2021 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define PROPERTY_CAN_BE_USED					CONSTLIT("canBeUsed")
#define PROPERTY_COMPONENTS						CONSTLIT("components")
#define PROPERTY_DAMAGED						CONSTLIT("damaged")
#define PROPERTY_DEFECTIVE_ENHANCEMENT			CONSTLIT("defectiveEnhancement")
#define PROPERTY_DESCRIPTION					CONSTLIT("description")
#define PROPERTY_DISRUPTED						CONSTLIT("disrupted")
#define PROPERTY_ENHANCEMENT					CONSTLIT("enhancement")
#define PROPERTY_HAS_USE_SCREEN					CONSTLIT("hasUseScreen")
#define PROPERTY_INC_CHARGES					CONSTLIT("incCharges")
#define PROPERTY_INSTALLED						CONSTLIT("installed")
#define PROPERTY_KNOWN							CONSTLIT("known")
#define PROPERTY_LEVEL  						CONSTLIT("level")
#define PROPERTY_MAX_CHARGES  					CONSTLIT("maxCharges")
#define PROPERTY_MAX_LEVEL  					CONSTLIT("maxLevel")
#define PROPERTY_MIN_LEVEL  					CONSTLIT("minLevel")
#define PROPERTY_MASS_BONUS_PER_CHARGE			CONSTLIT("massBonusPerCharge")
#define PROPERTY_PRICE							CONSTLIT("price")
#define PROPERTY_REFERENCE						CONSTLIT("reference")
#define PROPERTY_ROOT_NAME						CONSTLIT("rootName")
#define PROPERTY_SLOT_INDEX						CONSTLIT("slotIndex")
#define PROPERTY_TRADE_ID						CONSTLIT("tradeID")
#define PROPERTY_VALUE_BONUS_PER_CHARGE			CONSTLIT("valueBonusPerCharge")
#define PROPERTY_VARIANT						CONSTLIT("variant")
#define PROPERTY_UNKNOWN_TYPE					CONSTLIT("unknownType")
#define PROPERTY_UNKNOWN_TYPE_INDEX				CONSTLIT("unknownTypeIndex")
#define PROPERTY_USED							CONSTLIT("used")
#define PROPERTY_WEAPON_TYPES					CONSTLIT("weaponTypes")

TPropertyHandler<CItem> CItem::m_PropertyTable = std::array<TPropertyHandler<CItem>::SPropertyDef, 1> {{
		{
		"charges",						"Charges left in item.",
		[](const CItem &Item, const CString &sProperty) 
			{
			return ICCItemPtr(Item.GetCharges());
			},
		[](CItem &Item, const CString &sProperty, const ICCItem &Value, CString *retsError)
			{
			Item.SetCharges(Value.GetIntegerValue());
			return true;
			},
		},
	}};

bool CItem::FindCustomProperty (const CString &sProperty, ICCItemPtr &pResult) const

//	FindCustomProperty
//
//	Finds a custom property.

	{
	if (IsEmpty())
		return false;

	EPropertyType iType;
	if (!m_pItemType->FindCustomProperty(sProperty, pResult, &iType))
		return false;

	switch (iType)
		{
		case EPropertyType::propData:
		case EPropertyType::propItemData:
		case EPropertyType::propVariant:
			pResult = GetDataAsItem(sProperty);
			return true;

		case EPropertyType::propDynamicData:
			{
			CCodeChainCtx RunCtx(GetUniverse());

			RunCtx.SetItemType(GetType());
			RunCtx.DefineContainingType(m_pItemType);
			RunCtx.SaveAndDefineSourceVar(GetSource());
			RunCtx.SaveAndDefineItemVar(*this);

			pResult = RunCtx.RunCode(pResult);
			return true;
			}

		case EPropertyType::propObjData:
			return false;

		default:
			return true;
		}
	}

ICCItem *CItem::GetItemProperty (CCodeChainCtx &CCCtx, CItemCtx &Ctx, const CString &sProperty, bool bOnType) const

//	GetItemProperty
//
//	Returns an item property. Caller must free ICCItem.
//
//	This is one of the main entry points for item properties (the other is 
//	CDesignType::GetProperty). All other item property functions are helpers and
//	should not be called directly.
//
//	This function assumes that the item specified by CItemCtx is the SAME as 
//	this item. Behavior is undefined otherwise. However, we DO support an empty
//	CItemCtx.
//
//	ARCHITECTURE
//
//	There are two entry points for item properties. CDesignType::GetProperty 
//	returns all properties available to uninstalled items, including any device
//	or armor specific properties.
//
//	CItem::GetItemProperty is a superset of CDesignType::GetProperty. It also
//	returns properties available to concrete and installed items. For example, 
//	it returns the damaged state of an item and the charges on a weapon.
//
//	Importantly, the properties returned by CDesignType::GetProperty are 
//	identical to those returned by CItem::GetItemProperty on a newly cons'ed up
//	item.
//
//	All other item properties are helpers and should not be called directly
//	other than from one of the two methods above.
//
//	IMPLEMENTATION NOTES
//
//	CDeviceClass::FindItemProperty: We take both CCodeChainCtx and CItemCtx. This
//		method ONLY returns device properties. It will never return CItem or
//		CItemType properties (because otherwise we'd recurse infinitely). It
//		returns NULL if it does not know about the property.
//
//	CDesignType::FindBaseProperty: This method handles generic type properties 
//		and it also converts any old-style fields into properties. When getting
//		fields, we might again call out to the device class (to get its fields).
//		We return NULL if we do not know about the property.
//
//	CDesignType::GetProperty: This method calls OnGetProperty on its subclasses
//		to let them handle it. If the subclass does not handle it, then we call
//		CDesignType::FindBaseProperty.
//
//	CItemType::OnGetProperty: We call GetItemProperty with an empty CItemCtx.
//		Since GetItemProperty ends up calling CDesignType::FindBaseProperty, we
//		never return NULL (we always handle it).

	{
	CCodeChain &CC = GetUniverse().GetCC();
	ICCItemPtr pResult;
	int i;

	if (m_pItemType == NULL)
		return CC.CreateNil();

	//	Handle custom properties first.

	else if (FindCustomProperty(sProperty, pResult))
		return pResult->Reference();

	//	Look in our table

	else if (m_PropertyTable.FindProperty(*this, sProperty, pResult))
		return pResult->Reference();

	//	These are properties implemented in the old ways

	else if (strEquals(sProperty, PROPERTY_CAN_BE_USED))
		return CC.CreateBool(CanBeUsed());

	else if (strEquals(sProperty, PROPERTY_DAMAGED))
		return CC.CreateBool(IsDamaged());

	else if (strEquals(sProperty, PROPERTY_DEFECTIVE_ENHANCEMENT))
		return CC.CreateBool(GetMods().IsDisadvantage());

	else if (strEquals(sProperty, PROPERTY_DESCRIPTION))
		{
		if (CCCtx.InEvent(eventGetDescription))
			return CC.CreateString(GetType()->GetDesc());
		else
			return CC.CreateString(GetDesc());
		}

	else if (strEquals(sProperty, PROPERTY_DISRUPTED))
		{
		DWORD dwTime = GetDisruptedDuration();
		if (dwTime == 0)
			return CC.CreateNil();
		else if (dwTime == INFINITE_TICK)
			return CC.CreateTrue();
		else
			return CC.CreateInteger(dwTime);
		}

	else if (strEquals(sProperty, PROPERTY_ENHANCEMENT))
		return GetMods().AsDesc(GetUniverse())->Reference();

	else if (strEquals(sProperty, PROPERTY_HAS_USE_SCREEN))
		return CC.CreateBool(HasUseItemScreen());

	else if (strEquals(sProperty, PROPERTY_INSTALLED))
		return CC.CreateBool(IsInstalled());

	else if (strEquals(sProperty, PROPERTY_KNOWN))
		{
		if (bOnType)
			{
			//	If asking about the type, we return True if all unknown types 
			//	are known.

			for (int i = 0; i < m_pItemType->GetUnknownTypeCount(); i++)
				if (!m_pItemType->IsKnown(i))
					return CC.CreateNil();

			return CC.CreateTrue();
			}
		else
			return CC.CreateBool(IsKnown());
		}

	else if (strEquals(sProperty, PROPERTY_LEVEL))
		return CC.CreateInteger(GetLevel());

	else if (strEquals(sProperty, PROPERTY_PRICE))
		return CC.CreateInteger(GetTradePrice(NULL, true));

	else if (strEquals(sProperty, PROPERTY_REFERENCE))
		{
		if (CCCtx.InEvent(eventGetReferenceText))
			return CC.CreateString(GetType()->GetReference(Ctx));
		else
			return CC.CreateString(GetReference(Ctx));
		}

	else if (strEquals(sProperty, PROPERTY_ROOT_NAME))
		{
		CString sRoot;
		CString sModifier;

		CLanguage::ParseItemName(GetNounPhrase(nounShort | nounNoModifiers), &sRoot, &sModifier);

		if (sModifier.IsBlank())
			return CC.CreateString(sRoot);
		else
			return CC.CreateString(strPatternSubst(CONSTLIT("%s, %s"), sRoot, sModifier));
		}

	else if (strEquals(sProperty, PROPERTY_SLOT_INDEX))
		{
		if (IsInstalled())
			return CC.CreateInteger(GetInstalled());
		else
			return CC.CreateNil();
		}
	else if (strEquals(sProperty, PROPERTY_TRADE_ID))
		{
		TArray<SDisplayAttribute> Attribs;
		if (!GetDisplayAttributes(&Attribs, NULL, true))
			return CC.CreateNil();

		for (i = 0; i < Attribs.GetCount(); i++)
			if (!Attribs[i].sID.IsBlank())
				return CC.CreateString(Attribs[i].sID);

		return CC.CreateNil();
		}

	else if (strEquals(sProperty, PROPERTY_UNKNOWN_TYPE))
		{
		CItemType *pType = GetUnknownType();
		if (pType == NULL)
			return CC.CreateNil();
		else
			return CC.CreateInteger(pType->GetUNID());
		}

	else if (strEquals(sProperty, PROPERTY_UNKNOWN_TYPE_INDEX))
		{
		int iUnknownIndex = GetUnknownIndex();
		if (iUnknownIndex < 0)
			return CC.CreateNil();
		else
			return CC.CreateInteger(iUnknownIndex);
		}

	else if (strEquals(sProperty, PROPERTY_USED))
		return CC.CreateBool(IsUsed());

	else if (strEquals(sProperty, PROPERTY_VARIANT))
		return CC.CreateInteger(GetVariantNumber());

	//	Handle any armor item properties

	else if (IsArmor())
		{
		if (const CArmorItem ArmorItem = AsArmorItem())
			{
			ICCItemPtr pResult = ArmorItem.FindProperty(sProperty);
			if (pResult)
				return pResult->Reference();
			}
		}

	//	Next we handle all properties for devices, armor, etc. Note that this
	//	includes both installed properties (e.g., armor segment) and static
	//	properties (e.g., armor HP). But it DOES NOT include item type 
	//	properties common to all items (e.g., mass).

	else
		{
		CDeviceClass *pDevice;

		//	If this is a device, then pass it on

		if (pDevice = GetType()->GetDeviceClass())
			{
			if (pResult = pDevice->FindItemProperty(Ctx, sProperty))
				return pResult;
			}

		//	If this is a missile, then pass it to the weapon.

		else if (GetType()->IsMissile())
			{
			if (Ctx.ResolveVariant()
					&& (pResult = Ctx.GetVariantDevice()->FindItemProperty(Ctx, sProperty)))
				return pResult;
			}
		}

	//	If we get this far, then we ask the item type for its properties. This 
	//	will also get any design type properties and will check the old-style
	//	data fields.

	if (pResult = GetType()->FindItemTypeBaseProperty(CCCtx, sProperty))
		return pResult->Reference();

	//	Otherwise, we've got nothing

	else
		return CC.CreateNil();
	}

Metric CItem::GetItemPropertyDouble (CCodeChainCtx &CCCtx, CItemCtx &Ctx, const CString &sProperty) const

//	GetItemPropertyDouble
//
//	Returns a double.

	{
	CCodeChain &CC = GetUniverse().GetCC();
	ICCItem *pResult = GetItemProperty(CCCtx, Ctx, sProperty, false);
	if (pResult == NULL)
		return 0.0;

	Metric rValue = pResult->GetDoubleValue();
	pResult->Discard();
	return rValue;
	}

int CItem::GetItemPropertyInteger (CCodeChainCtx &CCCtx, CItemCtx &Ctx, const CString &sProperty) const

//	GetItemPropertyInteger
//
//	Returns an integer property.

	{
	CCodeChain &CC = GetUniverse().GetCC();
	ICCItem *pResult = GetItemProperty(CCCtx, Ctx, sProperty, false);
	if (pResult == NULL)
		return 0;

	int iValue = pResult->GetIntegerValue();
	pResult->Discard();
	return iValue;
	}

CString CItem::GetItemPropertyString (CCodeChainCtx &CCCtx, CItemCtx &Ctx, const CString &sProperty) const

//	GetItemPropertyString
//
//	Returns a string property

	{
	CCodeChain &CC = GetUniverse().GetCC();
	ICCItem *pResult = GetItemProperty(CCCtx, Ctx, sProperty, false);
	if (pResult == NULL)
		return 0;

	CString sValue;
	if (pResult->IsNil())
		sValue = NULL_STR;
	else
		sValue = pResult->Print(PRFLAG_NO_QUOTES | PRFLAG_ENCODE_FOR_DISPLAY);

	pResult->Discard();
	return sValue;
	}

bool CItem::SetCustomProperty (const CString &sProperty, const ICCItem &Value)

//	SetCustomProperty
//
//	Sets a custom property. Returns TRUE if the property was found.

	{
	if (IsEmpty())
		return false;

	ICCItemPtr pDummy;
	EPropertyType iType;
	if (!m_pItemType->FindCustomProperty(sProperty, pDummy, &iType))
		return false;

	switch (iType)
		{
		case EPropertyType::propGlobal:
			m_pItemType->SetGlobalData(sProperty, &Value);
			return true;

		case EPropertyType::propData:
		case EPropertyType::propItemData:
			Extra();
			m_pExtra->m_Data.SetData(sProperty, &Value);
			return true;

		default:
			return false;
		}
	}

ESetPropertyResult CItem::SetProperty (CItemCtx &Ctx, const CString &sName, const ICCItem *pValue, bool bOnType, CString *retsError)

//	SetProperty
//
//	Sets item property. If we cannot set the property we return an error. If
//	retsError is blank then we cannot set the property because the value is Nil.

	{
	CString sError;

	if (IsEmpty())
		{
		if (retsError) *retsError = CONSTLIT("Unable to set propery on a null item.");
		return ESetPropertyResult::error;
		}

	else if (SetCustomProperty(sName, (pValue ? *pValue : *ICCItemPtr::Nil())))
		return ESetPropertyResult::set;

	else if (m_PropertyTable.SetProperty(*this, sName, (pValue ? *pValue : *ICCItemPtr::Nil()), &sError)
			|| !sError.IsBlank())
		{
		if (sError.IsBlank())
			return ESetPropertyResult::set;
		else
			{
			if (retsError) *retsError = sError;
			return ESetPropertyResult::error;
			}
		}

	else if (strEquals(sName, PROPERTY_DAMAGED))
		{
		if (pValue == NULL)
			SetDamaged(true);
		else if (pValue->IsInteger())
			SetDamaged(pValue->GetIntegerValue());
		else
			SetDamaged(!pValue->IsNil());
		}

	else if (strEquals(sName, PROPERTY_DISRUPTED))
		{
		if (pValue == NULL)
			SetDisrupted(INFINITE_TICK);
		else if (pValue->IsNil())
			ClearDisrupted();
		else if (pValue->IsInteger())
			SetDisrupted(pValue->GetIntegerValue());
		else
			SetDisrupted(INFINITE_TICK);
		}

	else if (strEquals(sName, PROPERTY_INC_CHARGES))
		{
		if (pValue == NULL)
			SetCharges(GetCharges() + 1);
		else if (pValue->IsNil())
			{
			if (retsError) *retsError = NULL_STR;
			return ESetPropertyResult::error;
			}
		else
			SetCharges(Max(0, GetCharges() + pValue->GetIntegerValue()));
		}

	else if (strEquals(sName, PROPERTY_ENHANCEMENT))
		{
		if (pValue == NULL || pValue->IsNil())
			AddEnhancement(CItemEnhancement());
		else
			{
			CItemEnhancement NewEnhancement;
			if (NewEnhancement.InitFromDesc(GetUniverse(), *pValue, retsError) != NOERROR)
				return ESetPropertyResult::error;

			AddEnhancement(NewEnhancement);
			}
		}

	else if (strEquals(sName, PROPERTY_INSTALLED))
		{
		if (pValue && pValue->IsNil())
			ClearInstalled();
		else
			{
			if (retsError) *retsError = CONSTLIT("Unable to set installation flag on item.");
			return ESetPropertyResult::error;
			}
		}

	else if (strEquals(sName, PROPERTY_KNOWN))
		{
		if (bOnType)
			m_pItemType->SetAllKnown(!pValue->IsNil());
		else
			SetKnown(pValue && !pValue->IsNil());
		}

	else if (strEquals(sName, PROPERTY_LEVEL))
		{
		//  If this is armor, then we remember the current damaged state and
		//  carry that forward to the new level.

		if (CArmorClass *pArmor = GetType()->GetArmorClass())
			return pArmor->SetItemProperty(Ctx, *this, sName, (pValue ? *pValue : *ICCItemPtr::Nil()), retsError);

		//	Otherwise, we just set the item level.

		if (!SetLevel((pValue ? pValue->GetIntegerValue() : 0), retsError))
			return ESetPropertyResult::error;
		}
	else if (strEquals(sName, PROPERTY_UNKNOWN_TYPE_INDEX))
		{
		int iUnknownTypeCount = m_pItemType->GetUnknownTypeCount();
		if (iUnknownTypeCount == 0)
			;

		else if (pValue == NULL || pValue->IsNil())
			SetUnknownIndex(m_pItemType->GetRandomUnknownTypeIndex());

		else
			{
			int iIndex = Max(0, Min(pValue->GetIntegerValue(), iUnknownTypeCount - 1));
			SetUnknownIndex(iIndex);
			}
		}
	else if (strEquals(sName, PROPERTY_VARIANT))
		{
		if (pValue == NULL || pValue->IsNil())
			{
			if (retsError) *retsError = NULL_STR;
			return ESetPropertyResult::error;
			}

		SetVariantNumber(pValue->GetIntegerValue());
		}

	//	If this is armor, then pass it on.

	else if (CArmorClass *pArmor = GetType()->GetArmorClass())
		return pArmor->SetItemProperty(Ctx, *this, sName, (pValue ? *pValue : *ICCItemPtr::Nil()), retsError);

	//	If this is an installed device, then pass it on

	else if (CInstalledDevice *pDevice = Ctx.GetDevice())
		return pDevice->SetProperty(Ctx, sName, pValue, retsError);

	//	Otherwise, nothing

	else
		{
		if (retsError) *retsError = strPatternSubst(CONSTLIT("Unknown item property: %s."), sName);
		return ESetPropertyResult::notFound;
		}

	return ESetPropertyResult::set;
	}

