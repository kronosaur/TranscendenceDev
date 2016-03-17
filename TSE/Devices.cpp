//	Devices.cpp
//
//	Ship devices

#include "PreComp.h"

#define ENHANCE_ABILITIES_TAG					CONSTLIT("EnhancementAbilities")

#define CATEGORY_ATTRIB							CONSTLIT("category")
#define CRITERIA_ATTRIB							CONSTLIT("criteria")
#define DEVICE_SLOT_CATEGORY_ATTRIB				CONSTLIT("deviceSlotCategory")
#define DEVICE_SLOTS_ATTRIB						CONSTLIT("deviceSlots")
#define ENHANCEMENT_ATTRIB						CONSTLIT("enhancement")
#define EXTERNAL_ATTRIB							CONSTLIT("external")
#define ITEM_ID_ATTRIB							CONSTLIT("itemID")
#define MAX_HP_BONUS_ATTRIB						CONSTLIT("maxHPBonus")
#define OMNIDIRECTIONAL_ATTRIB					CONSTLIT("omnidirectional")
#define OVERLAY_TYPE_ATTRIB						CONSTLIT("overlayType")
#define POS_ANGLE_ATTRIB						CONSTLIT("posAngle")
#define POS_RADIUS_ATTRIB						CONSTLIT("posRadius")
#define POS_Z_ATTRIB							CONSTLIT("posZ")
#define SECONDARY_WEAPON_ATTRIB					CONSTLIT("secondaryWeapon")
#define TYPE_ATTRIB								CONSTLIT("type")
#define UNID_ATTRIB								CONSTLIT("UNID")

#define GET_OVERLAY_TYPE_EVENT					CONSTLIT("GetOverlayType")

#define LINKED_FIRE_ALWAYS						CONSTLIT("always")
#define LINKED_FIRE_ENEMY						CONSTLIT("whenInFireArc")
#define LINKED_FIRE_TARGET						CONSTLIT("targetInRange")

#define PROPERTY_CAN_BE_DISABLED				CONSTLIT("canBeDisabled")
#define PROPERTY_ENABLED						CONSTLIT("enabled")
#define PROPERTY_FIRE_ARC						CONSTLIT("fireArc")
#define PROPERTY_HP								CONSTLIT("hp")
#define PROPERTY_LINKED_FIRE_OPTIONS			CONSTLIT("linkedFireOptions")
#define PROPERTY_OMNIDIRECTIONAL				CONSTLIT("omnidirectional")
#define PROPERTY_POS							CONSTLIT("pos")
#define PROPERTY_POWER							CONSTLIT("power")
#define PROPERTY_SECONDARY						CONSTLIT("secondary")

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

bool CDeviceClass::AccumulateEnhancements (CItemCtx &Device, CInstalledDevice *pTarget, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements)

//	AccumulateEnhancements
//
//	If this device can enhance pTarget, then we add to the list of enhancements.

	{
	int i;
	bool bEnhanced = false;

	CInstalledDevice *pDevice = Device.GetDevice();
	CSpaceObject *pSource = Device.GetSource();

	//	See if we can enhance the target device

	if (pDevice == NULL 
			|| (pDevice->IsEnabled() && !pDevice->IsDamaged()))
		{
		for (i = 0; i < m_Enhancements.GetCount(); i++)
			{
			//	If this type of enhancement has already been applied, skip it

			if (!m_Enhancements[i].sType.IsBlank()
					&& EnhancementIDs.Find(m_Enhancements[i].sType))
				continue;

			//	If we don't match the criteria, skip it.

			if (pSource 
					&& pTarget
					&& !pSource->GetItemForDevice(pTarget).MatchesCriteria(m_Enhancements[i].Criteria))
				continue;

			//	Add the enhancement

			pEnhancements->Insert(m_Enhancements[i].Enhancement);
			bEnhanced = true;

			//	Remember that we added this enhancement class

			if (!m_Enhancements[i].sType.IsBlank())
				EnhancementIDs.Insert(m_Enhancements[i].sType);
			}
		}

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

		CCodeChainCtx Ctx;
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

		return COverlayType::AsType(g_pUniverse->FindDesignType(dwUNID));
		}
	else
		return GetOverlayType();
	}

Metric CDeviceClass::GetAmmoItemPropertyDouble (CItemCtx &Ctx, const CItem &Ammo, const CString &sProperty)

//  GetAmmoItemPropertyDouble
//
//  Wrapper around properties that discard ICCItem appropriately.
    
    {
    ICCItem *pValue = GetAmmoItemProperty(Ctx, Ammo, sProperty); 
    Metric rValue = pValue->GetDoubleValue(); 
    pValue->Discard(&g_pUniverse->GetCC());
    return rValue; 
    }

int CDeviceClass::GetInstallCost (void)

//	GetInstallCost
//
//	Returns the standard install cost (in the default currency of the item).

	{
	if (m_pItemType == NULL)
		return -1;

	const SStdDeviceStats *pStats = GetStdDeviceStats(m_pItemType->GetApparentLevel());
	if (pStats == NULL)
		return -1;

	return (int)m_pItemType->GetCurrencyType()->Exchange(CEconomyType::Default(), pStats->iInstallCost);
	}

ALERROR CDeviceClass::InitDeviceFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType)

//	InitDeviceFromXML
//
//	Initializes the device class base

	{
	int i;
	ALERROR error;

	m_pItemType = pType;

	//	Number of slots that the device takes up (if the attribute is missing
	//	then we assume 1)

	CString sAttrib;
	if (pDesc->FindAttribute(DEVICE_SLOTS_ATTRIB, &sAttrib))
		m_iSlots = strToInt(sAttrib, 1, NULL);
	else
		m_iSlots = 1;

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
		m_Enhancements.InsertEmpty(pEnhanceList->GetContentElementCount());

		for (i = 0; i < pEnhanceList->GetContentElementCount(); i++)
			{
			CXMLElement *pEnhancement = pEnhanceList->GetContentElement(i);

			m_Enhancements[i].sType = pEnhancement->GetAttribute(TYPE_ATTRIB);

			//	Load the item criteria

			CString sCriteria;
			if (!pEnhancement->FindAttribute(CRITERIA_ATTRIB, &sCriteria))
				sCriteria = CONSTLIT("*");

			CItem::ParseCriteria(sCriteria, &m_Enhancements[i].Criteria);

			//	Parse the enhancement itself

			if (error = m_Enhancements[i].Enhancement.InitFromDesc(Ctx, pEnhancement->GetAttribute(ENHANCEMENT_ATTRIB)))
				return error;
			}
		}

	return NOERROR;
	}

bool CDeviceClass::FindAmmoDataField (CItemType *pItem, const CString &sField, CString *retsValue)

//	FindAmmoDataField
//
//	Finds the device that fires this item and returns the given field

	{
	int i;
    CItem Ammo(pItem, 1);

	for (i = 0; i < g_pUniverse->GetItemTypeCount(); i++)
		{
		CItemType *pType = g_pUniverse->GetItemType(i);
		CDeviceClass *pWeapon;

		if (pType->IsDevice() 
				&& (pWeapon = pType->GetDeviceClass())
                && pWeapon->FindAmmoDataField(Ammo, sField, retsValue))
			return true;
		}

	return false;
	}

bool CDeviceClass::FindWeaponFor (CItemType *pItem, CDeviceClass **retpWeapon, int *retiVariant, CWeaponFireDesc **retpDesc)

//	FindWeaponFor
//
//	Returns weapon data for the given item (which may be a weapon or a missile).

	{
	int i;
	CDeviceClass *pDevice;
	int iVariant;

	//	Get the device and variant

    CItem Ammo;
	if (pItem->IsMissile())
		{
		iVariant = -1;

		for (i = 0; i < g_pUniverse->GetItemTypeCount(); i++)
			{
			CItemType *pType = g_pUniverse->GetItemType(i);

			if (pDevice = pType->GetDeviceClass())
				{
				iVariant = pDevice->GetAmmoVariant(pItem);
                if (iVariant != -1)
                    {
                    Ammo = CItem(pItem, 1);
					break;
                    }
				}
			}

		if (iVariant == -1)
			return false;
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

ICCItem *CDeviceClass::GetItemProperty (CItemCtx &Ctx, const CString &sName)

//	GetItemProperty
//
//	Returns the item property. Subclasses should call this if they do not
//	understand the property.

	{
	CCodeChain &CC = g_pUniverse->GetCC();

	//	Get the device

	CInstalledDevice *pDevice = Ctx.GetDevice();

	//	Get the property

	if (strEquals(sName, PROPERTY_CAN_BE_DISABLED))
		return (pDevice ? CC.CreateBool(pDevice->CanBeDisabled(Ctx)) : CC.CreateBool(CanBeDisabled(Ctx)));

	else if (strEquals(sName, PROPERTY_ENABLED))
		return (pDevice ? CC.CreateBool(pDevice->IsEnabled()) : CC.CreateNil());

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

		pList->AppendInteger(CC, pDevice->GetPosAngle());
		pList->AppendInteger(CC, pDevice->GetPosRadius());
		if (pDevice->GetPosZ() != 0)
			pList->AppendInteger(CC, pDevice->GetPosZ());

		//	Done

		return pResult;
		}

	else if (strEquals(sName, PROPERTY_POWER))
		return CC.CreateInteger(GetPowerRating(Ctx) * 100);

	else if (strEquals(sName, PROPERTY_SECONDARY))
		return (pDevice ? CC.CreateBool(pDevice->IsSecondaryWeapon()) : CC.CreateNil());

	else if (m_pItemType)
		return CreateResultFromDataField(CC, m_pItemType->GetDataField(sName));

	else
		return CC.CreateNil();
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
			AppendReferenceString(&sReference, strPatternSubst(CONSTLIT("%d Slots"), GetSlotsRequired()));

		//	External devices

		if (IsExternal() || (pDevice && pDevice->IsExternal()))
			AppendReferenceString(&sReference, CONSTLIT("External"));
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

	//	Compute the power units

	CString sUnit;
	if (iPower == 0)
		return NULL_STR;
	else if (iPower >= 10000)
		{
		sUnit = CONSTLIT("GW");
		iPower = iPower / 1000;
		}
	else
		sUnit = CONSTLIT("MW");

	//	Decimal

	int iMW = iPower / 10;
	int iMWDecimal = iPower % 10;

	if (iMW >= 100 || iMWDecimal == 0)
		return strPatternSubst(CONSTLIT("%d %s"), iMW, sUnit);
	else
		return strPatternSubst(CONSTLIT("%d.%d %s"), iMW, iMWDecimal, sUnit);
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
	CCodeChain &CC = g_pUniverse->GetCC();

	//	Get the installed device. We need this to set anything.

	CInstalledDevice *pDevice = Ctx.GetDevice();
	if (pDevice == NULL)
		{
		*retsError = CONSTLIT("Item is not an installed device on object.");
		return false;
		}

	//	Figure out what to set

	if (strEquals(sName, PROPERTY_FIRE_ARC))
		{
		//	A value of nil means no fire arc (and no omni)

		if (pValue == NULL || pValue->IsNil())
			{
			pDevice->SetOmniDirectional(false);
			pDevice->SetFireArc(0, 0);
			}

		//	A value of "omnidirectional" counts

		else if (strEquals(pValue->GetStringValue(), PROPERTY_OMNIDIRECTIONAL))
			{
			pDevice->SetOmniDirectional(true);
			pDevice->SetFireArc(0, 0);
			}

		//	A single value means that we just point in a direction

		else if (pValue->GetCount() == 1)
			{
			int iMinFireArc = AngleMod(pValue->GetElement(0)->GetIntegerValue());
			pDevice->SetOmniDirectional(false);
			pDevice->SetFireArc(iMinFireArc, iMinFireArc);
			}

		//	Otherwise we expect a list with two elements

		else if (pValue->GetCount() >= 2)
			{
			int iMinFireArc = AngleMod(pValue->GetElement(0)->GetIntegerValue());
			int iMaxFireArc = AngleMod(pValue->GetElement(1)->GetIntegerValue());
			pDevice->SetOmniDirectional(false);
			pDevice->SetFireArc(iMinFireArc, iMaxFireArc);
			}

		//	Invalid

		else
			{
			*retsError = CONSTLIT("Invalid fireArc parameter.");
			return false;
			}
		}

	else if (strEquals(sName, PROPERTY_LINKED_FIRE_OPTIONS))
		{
		//	Parse the options

		DWORD dwOptions;
		if (!::GetLinkedFireOptions(pValue, &dwOptions, retsError))
			return false;

		//	Set

		pDevice->SetLinkedFireOptions(dwOptions);
		}

	else if (strEquals(sName, PROPERTY_POS))
		{
		//	Get the parameters. We accept a single list parameter with angle/radius/z.
		//	(The latter is compatible with the return of objGetDevicePos.)

		int iPosAngle;
		int iPosRadius;
		int iZ;
		if (pValue == NULL || pValue->IsNil())
			{
			iPosAngle = 0;
			iPosRadius = 0;
			iZ = 0;
			}
		else if (pValue->GetCount() >= 2)
			{
			iPosAngle = pValue->GetElement(0)->GetIntegerValue();
			iPosRadius = pValue->GetElement(1)->GetIntegerValue();

			if (pValue->GetCount() >= 3)
				iZ = pValue->GetElement(2)->GetIntegerValue();
			else
				iZ = 0;
			}
		else
			{
			*retsError = CONSTLIT("Invalid angle and radius");
			return false;
			}

		//	Set it

		pDevice->SetPosAngle(iPosAngle);
		pDevice->SetPosRadius(iPosRadius);
		pDevice->SetPosZ(iZ);
		}

	else if (strEquals(sName, PROPERTY_SECONDARY))
		{
		if (pValue == NULL || !pValue->IsNil())
			pDevice->SetSecondary(true);
		else
			pDevice->SetSecondary(false);
		}

	else
		{
		*retsError = strPatternSubst(CONSTLIT("Unknown item property: %s."), sName);
		return false;
		}

	return true;
	}

