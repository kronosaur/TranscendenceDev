//	Devices.cpp
//
//	Ship devices

#include "PreComp.h"

#define ENHANCE_ABILITIES_TAG					CONSTLIT("EnhancementAbilities")
#define EQUIPMENT_TAG							CONSTLIT("Equipment")

#define CAN_BE_DAMAGED_ATTRIB					CONSTLIT("canBeDamaged")
#define CAN_BE_DISABLED_ATTRIB					CONSTLIT("canBeDisabled")
#define CAN_BE_DISRUPTED_ATTRIB					CONSTLIT("canBeDisrupted")
#define CATEGORY_ATTRIB							CONSTLIT("category")
#define CRITERIA_ATTRIB							CONSTLIT("criteria")
#define DEVICE_DAMAGE_IMMUNE_ATTRIB				CONSTLIT("deviceDamageImmune")
#define DEVICE_DISRUPT_IMMUNE_ATTRIB			CONSTLIT("deviceDisruptImmune")
#define DEVICE_SLOT_CATEGORY_ATTRIB				CONSTLIT("deviceSlotCategory")
#define DEVICE_SLOTS_ATTRIB						CONSTLIT("deviceSlots")
#define ENHANCEMENT_ATTRIB						CONSTLIT("enhancement")
#define EXTERNAL_ATTRIB							CONSTLIT("external")
#define ITEM_ID_ATTRIB							CONSTLIT("itemID")
#define MAX_HP_BONUS_ATTRIB						CONSTLIT("maxHPBonus")
#define OMNIDIRECTIONAL_ATTRIB					CONSTLIT("omnidirectional")
#define OVERLAY_TYPE_ATTRIB						CONSTLIT("overlayType")
#define SECONDARY_WEAPON_ATTRIB					CONSTLIT("secondaryWeapon")
#define TYPE_ATTRIB								CONSTLIT("type")
#define UNID_ATTRIB								CONSTLIT("UNID")

#define GET_OVERLAY_TYPE_EVENT					CONSTLIT("GetOverlayType")

#define LINKED_FIRE_ALWAYS						CONSTLIT("always")
#define LINKED_FIRE_ENEMY						CONSTLIT("whenInFireArc")
#define LINKED_FIRE_TARGET						CONSTLIT("targetInRange")

#define PROPERTY_CAN_BE_DAMAGED					CONSTLIT("canBeDamaged")
#define PROPERTY_CAN_BE_DISABLED				CONSTLIT("canBeDisabled")
#define PROPERTY_CAN_BE_DISRUPTED				CONSTLIT("canBeDisrupted")
#define PROPERTY_CAPACITOR      				CONSTLIT("capacitor")
#define PROPERTY_DEVICE_SLOTS					CONSTLIT("deviceSlots")
#define PROPERTY_ENABLED						CONSTLIT("enabled")
#define PROPERTY_EXTERNAL						CONSTLIT("external")
#define PROPERTY_EXTRA_POWER_USE				CONSTLIT("extraPowerUse")
#define PROPERTY_FIRE_ARC						CONSTLIT("fireArc")
#define PROPERTY_HP								CONSTLIT("hp")
#define PROPERTY_LINKED_FIRE_OPTIONS			CONSTLIT("linkedFireOptions")
#define PROPERTY_OMNIDIRECTIONAL				CONSTLIT("omnidirectional")
#define PROPERTY_POS							CONSTLIT("pos")
#define PROPERTY_POWER							CONSTLIT("power")
#define PROPERTY_POWER_OUTPUT					CONSTLIT("powerOutput")
#define PROPERTY_POWER_USE						CONSTLIT("powerUse")
#define PROPERTY_SECONDARY						CONSTLIT("secondary")
#define PROPERTY_SLOT_ID						CONSTLIT("slotID")
#define PROPERTY_TEMPERATURE      				CONSTLIT("temperature")

const int MAX_COUNTER =					        100;

struct SStdDeviceStats
	{
	int iInstallCost;							//	Cost to install (credits)
	};

static SStdDeviceStats STD_DEVICE_STATS[MAX_ITEM_LEVEL] =
	{
		//	Install
		{	60,		}, 
 		{	160,		},  
 		{	360,		},  
 		{	760,		},  
 		{	1500,		},  
 		{	3200,		},  
 		{	6500,		},  
 		{	13000,		},  
 		{	26000,		},  
 		{	53000,		},  
 		{	100000,		},  
 		{	210000,		},  
 		{	450000,		},  
 		{	880000,		},  
 		{	1700000,	},  
 		{	3500000,	},  
 		{	7100000,	},  
 		{	14000000,	},  
 		{	28000000,	}, 
 		{	56000000,	},  
 		{	110000000,	},  
 		{	230000000,	},  
 		{	450000000,	},  
 		{	910000000,	},  
 		{	1800000000,	},
	};

static char *CACHED_EVENTS[CDeviceClass::evtCount] =
	{
		"GetOverlayType",
	};

inline const SStdDeviceStats *GetStdDeviceStats (int iLevel)
	{
	if (iLevel >= 1 && iLevel <= MAX_ITEM_LEVEL)
		return &STD_DEVICE_STATS[iLevel - 1];
	else
		return NULL;
	}

void CDeviceClass::AccumulateAttributes (CItemCtx &ItemCtx, const CItem &Ammo, TArray<SDisplayAttribute> *retList)

//	AccumulateAttributes
//
//	Add display attributes to the list.

	{
	//	Add general device attributes. If we have a variant, then it means 
	//	we're interested in the attributes for a missile/ammo of the device 
	//	(not the device itself).

	if (Ammo.IsEmpty())
		{
		CInstalledDevice *pDevice = ItemCtx.GetDevice();

		//	Linked-fire

		DWORD dwOptions = GetLinkedFireOptions(ItemCtx);
		if (dwOptions != 0)
			retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("linked-fire")));
		}

	//	Let our subclasses add their own attributes

	OnAccumulateAttributes(ItemCtx, Ammo, retList);
	}

bool CDeviceClass::AccumulateEnhancements (CItemCtx &Device, CInstalledArmor *pTarget, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements)

//	AccumulateEnhancements
//
//	If this device can enhance the given armor, then we add to the list of enhancements.
//	We return TRUE if we enhanced the target.

	{
	bool bEnhanced = false;

	//	See if we can enhance the target device

	if (Device.IsDeviceWorking())
		bEnhanced = m_Enhancements.Accumulate(Device, *pTarget->GetItem(), EnhancementIDs, pEnhancements);

	//	Let sub-classes add their own

	if (OnAccumulateEnhancements(Device, pTarget, EnhancementIDs, pEnhancements))
		bEnhanced = true;

	//	Done

	return bEnhanced;
	}

bool CDeviceClass::AccumulateEnhancements (CItemCtx &Device, CInstalledDevice *pTarget, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements)

//	AccumulateEnhancements
//
//	If this device can enhance pTarget, then we add to the list of enhancements.

	{
	bool bEnhanced = false;

	//	See if we can enhance the target device

	if (Device.IsDeviceWorking())
		bEnhanced = m_Enhancements.Accumulate(Device, *pTarget->GetItem(), EnhancementIDs, pEnhancements);

	//	Let sub-classes add their own

	if (OnAccumulateEnhancements(Device, pTarget, EnhancementIDs, pEnhancements))
		bEnhanced = true;

	//	Done

	return bEnhanced;
	}

bool CDeviceClass::AccumulatePerformance (CItemCtx &ItemCtx, SShipPerformanceCtx &Ctx) const

//  AccumulatePerformance
//
//  If this device alters a ship's performance, then we modified the ship 
//  performance context. We return TRUE if we modified the context.

    {
    bool bModified = false;

	//	If we install equipment, then add it.

	if (ItemCtx.IsDeviceWorking())
		Ctx.Abilities.Set(m_Equipment);

    //  Let sub-classes handle it

    if (OnAccumulatePerformance(ItemCtx, Ctx))
        bModified = true;

    //  Done

    return bModified;
    }

void CDeviceClass::AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed)

//	AddTypesUsed
//
//	Adds types used by this class.

	{
	retTypesUsed->SetAt(m_pOverlayType.GetUNID(), true);

	OnAddTypesUsed(retTypesUsed);
	}

ALERROR CDeviceClass::Bind (SDesignLoadCtx &Ctx)

//	Bind
//
//	Bind the design

	{
	DEBUG_TRY

	ALERROR error;

	if (error = m_pOverlayType.Bind(Ctx))
		return error;

	m_pItemType->InitCachedEvents(evtCount, CACHED_EVENTS, m_CachedEvents);

	return OnDesignLoadComplete(Ctx);

	DEBUG_CATCH
	}

COverlayType *CDeviceClass::FireGetOverlayType (CItemCtx &ItemCtx) const

//	FireGetOverlayType
//
//	Fire GetOverlayType event

	{
	SEventHandlerDesc Event;
	if (FindEventHandlerDeviceClass(evtGetOverlayType, &Event))
		{
		//	Setup arguments

		CCodeChainCtx Ctx(GetUniverse());
		Ctx.DefineContainingType(GetItemType());
		Ctx.SaveAndDefineSourceVar(ItemCtx.GetSource());
		Ctx.SaveAndDefineItemVar(ItemCtx);

		ICCItem *pResult = Ctx.Run(Event);

		DWORD dwUNID = 0;
		if (pResult->IsError())
			ItemCtx.GetSource()->ReportEventError(GET_OVERLAY_TYPE_EVENT, pResult);
		else if (!pResult->IsNil())
			dwUNID = pResult->GetIntegerValue();

		Ctx.Discard(pResult);

		//	Done

		return COverlayType::AsType(GetUniverse().FindDesignType(dwUNID));
		}
	else
		return GetOverlayType();
	}

bool CDeviceClass::GetAmmoItemPropertyBool (CItemCtx &Ctx, const CItem &Ammo, const CString &sProperty)

//  GetAmmoItemPropertyBool
//
//  Wrapper around properties that discard ICCItem appropriately.
    
    {
    ICCItem *pValue = FindAmmoItemProperty(Ctx, Ammo, sProperty); 
    if (pValue == NULL)
        return false;

    bool bValue = !pValue->IsNil();
    pValue->Discard();
    return bValue;
    }

Metric CDeviceClass::GetAmmoItemPropertyDouble (CItemCtx &Ctx, const CItem &Ammo, const CString &sProperty)

//  GetAmmoItemPropertyDouble
//
//  Wrapper around properties that discard ICCItem appropriately.
    
    {
    ICCItem *pValue = FindAmmoItemProperty(Ctx, Ammo, sProperty); 
    if (pValue == NULL)
        return 0.0;

    Metric rValue = pValue->GetDoubleValue(); 
    pValue->Discard();
    return rValue; 
    }

int CDeviceClass::GetInstallCost (CItemCtx &ItemCtx)

//	GetInstallCost
//
//	Returns the standard install cost (in the default currency of the item).

	{
	if (m_pItemType == NULL)
		return -1;

	const SStdDeviceStats *pStats = GetStdDeviceStats(m_pItemType->GetApparentLevel(ItemCtx));
	if (pStats == NULL)
		return -1;

	return (int)m_pItemType->GetCurrencyType()->Exchange(NULL, pStats->iInstallCost);
	}

CUniverse &CDeviceClass::GetUniverse (void) const

//	GetUniverse
//
//	Returns the universe object.

	{
	return (m_pItemType ? m_pItemType->GetUniverse() : *g_pUniverse);
	}

ALERROR CDeviceClass::InitDeviceFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType)

//	InitDeviceFromXML
//
//	Initializes the device class base

	{
	ALERROR error;

	m_pItemType = pType;

	//	Number of slots that the device takes up (if the attribute is missing
	//	then we assume 1)

	CString sAttrib;
	if (pDesc->FindAttribute(DEVICE_SLOTS_ATTRIB, &sAttrib))
		m_iSlots = strToInt(sAttrib, 1, NULL);
	else
		m_iSlots = 1;

	//	Is this device immune to damage and disrupt?
	//	Note: We assume that damageable and disruptable do not mean the same thing.
	//	There could be a device that is immune to disrupt but vulnerable to other damaging effects.
	//	There could also be a device that is disruptable but not damageable.

	m_fDeviceDamageImmune = pDesc->GetAttributeBool(DEVICE_DAMAGE_IMMUNE_ATTRIB);
	m_fDeviceDisruptImmune = pDesc->GetAttributeBool(DEVICE_DISRUPT_IMMUNE_ATTRIB);

	//	Sometimes we override the basic properties of a device category.

	m_fCanBeDamagedOverride = pDesc->GetAttributeBool(CAN_BE_DAMAGED_ATTRIB);
	m_fCanBeDisabledOverride = pDesc->GetAttributeBool(CAN_BE_DISABLED_ATTRIB);
	m_fCanBeDisruptedOverride = pDesc->GetAttributeBool(CAN_BE_DISRUPTED_ATTRIB);

	//	Slot type

	CString sSlotType;
	if (pDesc->FindAttribute(CATEGORY_ATTRIB, &sSlotType) 
			|| pDesc->FindAttribute(DEVICE_SLOT_CATEGORY_ATTRIB, &sSlotType))
		{
		if (!CItemType::ParseItemCategory(sSlotType, &m_iSlotCategory))
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Invalid deviceSlotCategory: %s."), sSlotType);
			return ERR_FAIL;
			}

		//	Make sure it is a valid device

		switch (m_iSlotCategory)
			{
			//	OK
			case itemcatCargoHold:
			case itemcatDrive:
			case itemcatLauncher:
			case itemcatMiscDevice:
			case itemcatReactor:
			case itemcatShields:
			case itemcatWeapon:
				break;

			default:
				{
				Ctx.sError = strPatternSubst(CONSTLIT("Not a valid device category: %s."), sSlotType);
				return ERR_FAIL;
				}
			}
		}
	else
		//	itemcatNone means use the actual item category
		m_iSlotCategory = itemcatNone;

	//	Overlay

	if (error = m_pOverlayType.LoadUNID(Ctx, pDesc->GetAttribute(OVERLAY_TYPE_ATTRIB)))
		return error;

	//	Other settings

	m_iMaxHPBonus = pDesc->GetAttributeIntegerBounded(MAX_HP_BONUS_ATTRIB, 0, -1, 150);
	m_fExternal = pDesc->GetAttributeBool(EXTERNAL_ATTRIB);

	//	Does this device enhance other items?

	CXMLElement *pEnhanceList = pDesc->GetContentElementByTag(ENHANCE_ABILITIES_TAG);
	if (pEnhanceList)
		{
		if (error = m_Enhancements.InitFromXML(Ctx, pEnhanceList))
			return error;
		}

	//	Does this device provide some equipment when installed?

	CXMLElement *pEquipmentList = pDesc->GetContentElementByTag(EQUIPMENT_TAG);
	if (pEquipmentList)
		{
		if (error = m_Equipment.InitFromXML(Ctx, pEquipmentList))
			return error;
		}

	return NOERROR;
	}

bool CDeviceClass::FindAmmoDataField (CItemType *pItem, const CString &sField, CString *retsValue)

//	FindAmmoDataField
//
//	Finds the device that fires this item and returns the given field

	{
	CDeviceClass *pWeapon = (pItem->GetLaunchWeapons().GetCount() ? pItem->GetLaunchWeapons()[0] : NULL);
	if (pWeapon == NULL)
		return false;

	return pWeapon->FindAmmoDataField(CItem(pItem, 1), sField, retsValue);
	}

ICCItem *CDeviceClass::FindItemProperty (CItemCtx &Ctx, const CString &sName)

//	FindItemProperty
//
//	Returns the item property. Subclasses should call this if they do not
//	understand the property.
//
//  We return NULL if the property is not found. Otherwise, the caller is 
//  responsible for freeing.
//
//	NOTE: We only return device-specific properties. We DO NOT return generic
//	item properties.

	{
	CCodeChain &CC = GetUniverse().GetCC();
    CString sFieldValue;

	//	Get the device

	CInstalledDevice *pDevice = Ctx.GetDevice();

	//	Get the property

	if (strEquals(sName, PROPERTY_CAN_BE_DAMAGED))
		return (pDevice ? CC.CreateBool(pDevice->CanBeDamaged()) : CC.CreateBool(CanBeDamaged()));
    else if (strEquals(sName, PROPERTY_CAN_BE_DISABLED))
        return (pDevice ? CC.CreateBool(pDevice->CanBeDisabled(Ctx)) : CC.CreateBool(CanBeDisabled(Ctx)));
	else if (strEquals(sName, PROPERTY_CAN_BE_DISRUPTED))
		return (pDevice ? CC.CreateBool(pDevice->CanBeDisrupted()) : CC.CreateBool(CanBeDisrupted()));
    else if (strEquals(sName, PROPERTY_CAPACITOR))
        {
        CSpaceObject *pSource = Ctx.GetSource();
        CounterTypes iType;
        int iLevel;
        GetCounter(pDevice, pSource, &iType, &iLevel);
        if (iType != cntCapacitor || pDevice == NULL || pSource == NULL)
            return CC.CreateNil();

        return CC.CreateInteger(iLevel);
        }

    else if (strEquals(sName, PROPERTY_DEVICE_SLOTS))
        return CC.CreateInteger(GetSlotsRequired());

    else if (strEquals(sName, PROPERTY_ENABLED))
        return (pDevice ? CC.CreateBool(pDevice->IsEnabled()) : CC.CreateNil());

	else if (strEquals(sName, PROPERTY_EXTERNAL))
		return CC.CreateBool(pDevice ? pDevice->IsExternal() : IsExternal());

	else if (strEquals(sName, PROPERTY_EXTRA_POWER_USE))
		{
		if (pDevice == NULL)
			return CC.CreateNil();

		return CC.CreateInteger(pDevice->GetExtraPowerUse());
		}

    else if (strEquals(sName, PROPERTY_POS))
        {
        if (pDevice == NULL)
            return CC.CreateNil();

        //	Create a list

        ICCItem *pResult = CC.CreateLinkedList();
        if (pResult->IsError())
            return pResult;

        CCLinkedList *pList = (CCLinkedList *)pResult;

        //	List contains angle, radius, and optional z

        pList->AppendInteger(pDevice->GetPosAngle());
        pList->AppendInteger(pDevice->GetPosRadius());
        if (pDevice->GetPosZ() != 0)
            pList->AppendInteger(pDevice->GetPosZ());

        //	Done

        return pResult;
        }

	else if (strEquals(sName, PROPERTY_POWER))
		{
		if (GetCategory() == itemcatReactor)
			return CTLispConvert::CreatePowerResultMW(GetPowerOutput(Ctx))->Reference();
		else
			return CTLispConvert::CreatePowerResultMW(GetPowerRating(Ctx))->Reference();
		}

	else if (strEquals(sName, PROPERTY_POWER_OUTPUT))
		return CreatePowerResult(GetPowerOutput(Ctx) * 100.0);

	else if (strEquals(sName, PROPERTY_POWER_USE))
		return CreatePowerResult(GetPowerRating(Ctx) * 100.0);

    else if (strEquals(sName, PROPERTY_SECONDARY))
        return (pDevice ? CC.CreateBool(pDevice->IsSecondaryWeapon()) : CC.CreateNil());

	else if (strEquals(sName, PROPERTY_SLOT_ID))
		return (pDevice ? CC.CreateString(pDevice->GetID()) : CC.CreateNil());

    else if (strEquals(sName, PROPERTY_TEMPERATURE))
        {
        CSpaceObject *pSource = Ctx.GetSource();
        CounterTypes iType;
        int iLevel;
        GetCounter(pDevice, pSource, &iType, &iLevel);
        if (iType != cntTemperature || pDevice == NULL || pSource == NULL)
            return CC.CreateNil();

        return CC.CreateInteger(iLevel);
        }

    else
        return NULL;
	}

bool CDeviceClass::FindWeaponFor (CItemType *pItem, CDeviceClass **retpWeapon, int *retiVariant, CWeaponFireDesc **retpDesc)

//	FindWeaponFor
//
//	Returns weapon data for the given item (which may be a weapon or a missile).

	{
	CDeviceClass *pDevice;
	int iVariant;

	//	Get the device and variant

    CItem Ammo;
	if (pItem->IsMissile())
		{
		pDevice = (pItem->GetLaunchWeapons().GetCount() ? pItem->GetLaunchWeapons()[0] : NULL);
		iVariant = (pDevice ? pDevice->GetAmmoVariant(pItem) : -1);
		if (iVariant == -1)
			return false;

		Ammo = CItem(pItem, 1);
		}
	else
		{
		pDevice = pItem->GetDeviceClass();
		if (pDevice == NULL)
			return false;

		iVariant = 0;
		}

	CWeaponClass *pWeapon = pDevice->AsWeaponClass();
	if (pWeapon == NULL)
		return false;

	//	Done

	if (retpWeapon)
		*retpWeapon = pDevice;

	if (retiVariant)
		*retiVariant = iVariant;

	if (retpDesc)
		*retpDesc = pWeapon->GetWeaponFireDesc(CItemCtx(), Ammo);

	return true;
	}

int CDeviceClass::GetFireArc (CItemCtx &Ctx) const

//	GetFireArc
//
//	Returns the fire arc for swivel weapons and turrets.

	{
	int iMinArc, iMaxArc;
	switch (GetRotationType(Ctx, &iMinArc, &iMaxArc))
		{
		case rotOmnidirectional:
			return 360;

		case rotSwivel:
			return AngleRange(iMinArc, iMaxArc);

		default:
			return 0;
		}
	}

ItemCategories CDeviceClass::GetItemCategory (DeviceNames iDev)

//	GetItemCategory
//
//	Gets the item category that corresponds to this device name.

	{
	switch (iDev)
		{
		case devPrimaryWeapon:
			return itemcatWeapon;

		case devMissileWeapon:
			return itemcatLauncher;

		case devShields:
			return itemcatShields;

		case devDrive:
			return itemcatDrive;

		case devCargo:
			return itemcatCargoHold;

		case devReactor:
			return itemcatReactor;

		default:
			ASSERT(false);
			return itemcatNone;
		}
	}

CString CDeviceClass::GetLinkedFireOptionString (DWORD dwOptions)

//	GetLinkedFireOptionString
//
//	Returns the string representing the given (single) option

	{
	switch ((LinkedFireOptions)dwOptions)
		{
		case lkfAlways:
			return LINKED_FIRE_ALWAYS;

		case lkfTargetInRange:
			return LINKED_FIRE_TARGET;

		case lkfEnemyInRange:
			return LINKED_FIRE_ENEMY;

		default:
			ASSERT(false);
			return NULL_STR;
		}
	}

CString CDeviceClass::GetReference (CItemCtx &Ctx, const CItem &Ammo, DWORD dwFlags)

//	GetReference
//
//	Returns reference string

	{
	CString sReference;
	
	//	For a device we always add power and other properties.
	//	(If iVariant != -1 then it means that we're looking for reference on a
	//	missile or someting).
	
	if (Ammo.IsEmpty())
		{
		CInstalledDevice *pDevice = Ctx.GetDevice();

		//	Start with power requirements

		AppendReferenceString(&sReference, GetReferencePower(Ctx));

		//	Non-standard slots

		if (GetSlotsRequired() != 1)
			AppendReferenceString(&sReference, strPatternSubst(CONSTLIT("%d slots"), GetSlotsRequired()));

		//	External devices

		if (IsExternal() || (pDevice && pDevice->IsExternal()))
			AppendReferenceString(&sReference, CONSTLIT("external"));
		}

	//	Combine with our subclass

	AppendReferenceString(&sReference, OnGetReference(Ctx, Ammo, dwFlags));
	return sReference;
	}

CString CDeviceClass::GetReferencePower (CItemCtx &Ctx)

//	GetReferencePower
//
//	Returns a string for the reference relating to the power required for
//	this device.

	{
	int iPower = GetPowerRating(Ctx);

	//	Callers rely on the fact that we return NULL_STR for power if we don't
	//	consume any power (so they know not to show the reference).

	if (iPower == 0)
		return NULL_STR;

	//	GetPowerRating returns power in 1/10ths of a MW, but ComposeNumber wants
	//	power in KWs.

	return CLanguage::ComposeNumber(CLanguage::numberPower, iPower * 100.0);
	}

bool CDeviceClass::OnDestroyCheck (CItemCtx &ItemCtx, DestructionTypes iCause, const CDamageSource &Attacker)

//	OnDestroyCheck
//
//	Returns FALSE if the device can prevent the source from getting destroyed.

	{
	CSpaceObject *pSource = ItemCtx.GetSource();
	CInstalledDevice *pDevice = ItemCtx.GetDevice();
	if (pDevice == NULL || !pDevice->IsEnabled())
		return true;

	//	NOTE: We give damaged and disrupted devices a chance to handle this
	//	event in case they want to do something special.

	return ItemCtx.GetItem().FireOnDestroyCheck(ItemCtx, iCause, Attacker);
	}

Metric CDeviceClass::OnGetScaledCostAdj (CItemCtx &Ctx) const

//  OnGetScaledCostAdj
//
//  Default implementation. Descendants may override.

    {
    if (Ctx.IsItemNull())
        return 1.0;

    int iLevel = Ctx.GetItem().GetLevel();
    if (iLevel <= 0)
        return 1.0;

    //  We use weapon price increases as a guide.

    return CWeaponClass::GetStdStats(iLevel).rCost / CWeaponClass::GetStdStats(m_pItemType->GetLevel()).rCost;
    }

ALERROR CDeviceClass::ParseLinkedFireOptions (SDesignLoadCtx &Ctx, const CString &sDesc, DWORD *retdwOptions)

//	ParseLinkedFireOptions
//
//	Parses a linked-fire options string.

	{
	DWORD dwOptions = 0;

	if (sDesc.IsBlank())
		;
	else if (strEquals(sDesc, LINKED_FIRE_ALWAYS))
		dwOptions |= CDeviceClass::lkfAlways;
	else if (strEquals(sDesc, LINKED_FIRE_TARGET))
		dwOptions |= CDeviceClass::lkfTargetInRange;
	else if (strEquals(sDesc, LINKED_FIRE_ENEMY))
		dwOptions |= CDeviceClass::lkfEnemyInRange;
	else
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Invalid linkedFire option: %s"), sDesc);
		return ERR_FAIL;
		}

	*retdwOptions = dwOptions;

	return NOERROR;
	}

int CDeviceClass::ParseVariantFromPropertyName (const CString &sName, CString *retsName)

//	ParseVariantFromPropertyName
//
//	If the name ends in :nn then nn is the variant. We also return the parsed
//	property name (without the variant).

	{
	//	Look for a :nn suffix specifying a variant

	char *pStart = sName.GetASCIIZPointer();
	char *pPos = pStart;
	while (*pPos != '\0')
		{
		if (*pPos == ':')
			{
			if (retsName)
				*retsName = CString(pStart, (int)(pPos - pStart));

			return strParseInt(pPos + 1, 0);
			}

		pPos++;
		}

	//	Not found

	return -1;
	}

bool CDeviceClass::SetItemProperty (CItemCtx &Ctx, const CString &sName, ICCItem *pValue, CString *retsError)

//	SetItemProperty
//
//	Sets a device property. Subclasses should call this if they do not 
//	understand the property.

	{
	*retsError = strPatternSubst(CONSTLIT("Unknown item property: %s."), sName);
	return false;
	}

