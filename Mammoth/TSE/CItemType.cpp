//	CItemType.cpp
//
//	CItemType class

#include "PreComp.h"

#define ARMOR_TAG								CONSTLIT("Armor")
#define AUTO_DEFENSE_CLASS_TAG					CONSTLIT("AutoDefenseDevice")
#define CARGO_HOLD_CLASS_TAG					CONSTLIT("CargoHoldDevice")
#define COMPONENTS_TAG							CONSTLIT("Components")
#define CYBER_DECK_CLASS_TAG					CONSTLIT("CyberDeckDevice")
#define DOCK_SCREENS_TAG						CONSTLIT("DockScreens")
#define DRIVE_CLASS_TAG							CONSTLIT("DriveDevice")
#define ENHANCER_CLASS_TAG						CONSTLIT("EnhancerDevice")
#define ENHANCER_ITEM_TAG						CONSTLIT("EnhancerItem")
#define EVENTS_TAG								CONSTLIT("Events")
#define GLOBAL_DATA_TAG							CONSTLIT("GlobalData")
#define IMAGE_TAG								CONSTLIT("Image")
#define COCKPIT_USE_TAG							CONSTLIT("Invoke")
#define LANGUAGE_TAG							CONSTLIT("Language")
#define MISCELLANEOUS_CLASS_TAG					CONSTLIT("MiscellaneousDevice")
#define MISSILE_TAG								CONSTLIT("Missile")
#define NAMES_TAG								CONSTLIT("Names")
#define REACTOR_CLASS_TAG						CONSTLIT("ReactorDevice")
#define REPAIRER_CLASS_TAG						CONSTLIT("RepairerDevice")
#define SHIELD_CLASS_TAG						CONSTLIT("Shields")
#define STATIC_DATA_TAG							CONSTLIT("StaticData")
#define SOLAR_DEVICE_CLASS_TAG					CONSTLIT("SolarDevice")
#define WEAPON_CLASS_TAG						CONSTLIT("Weapon")

#define AMMO_CHARGES_ATTRIB						CONSTLIT("ammoCharges")
#define AS_ARMOR_SET_ATTRIB						CONSTLIT("asArmorSet")
#define INSTANCE_DATA_ATTRIB					CONSTLIT("charges")
#define COMPLETE_ARMOR_ONLY_ATTRIB				CONSTLIT("completeArmorOnly")
#define DATA_ATTRIB								CONSTLIT("data")
#define ENHANCEMENT_ATTRIB						CONSTLIT("enhancement")
#define FREQUENCY_ATTRIB						CONSTLIT("frequency")
#define ENABLED_ONLY_ATTRIB						CONSTLIT("enabledOnly")
#define INSTALLED_ONLY_ATTRIB					CONSTLIT("installedOnly")
#define KEY_ATTRIB								CONSTLIT("key")
#define LEVEL_ATTRIB							CONSTLIT("level")
#define MASS_BONUS_PER_CHARGE_ATTRIB			CONSTLIT("massBonusPerCharge")
#define MAX_CHARGES_ATTRIB						CONSTLIT("maxCharges")
#define NO_SALE_IF_USED_ATTRIB					CONSTLIT("noSaleIfUsed")
#define NUMBER_APPEARING_ATTRIB					CONSTLIT("numberAppearing")
#define PLURAL_NAME_ATTRIB						CONSTLIT("pluralName")
#define REVERSE_ARTICLE_ATTRIB					CONSTLIT("reverseArticle")
#define ROLE_ATTRIB								CONSTLIT("role")
#define SECOND_PLURAL_ATTRIB					CONSTLIT("secondPlural")
#define SHOW_CHARGES_IN_USE_MENU				CONSTLIT("showChargesInUseMenu")
#define SHOW_REFERENCE_ATTRIB					CONSTLIT("showReference")
#define SORT_NAME_ATTRIB						CONSTLIT("sortName")
#define UNID_ATTRIB								CONSTLIT("UNID")
#define UNINSTALLED_ONLY_ATTRIB					CONSTLIT("uninstalledOnly")
#define UNKNOWN_TYPE_ATTRIB						CONSTLIT("unknownType")
#define USE_AS_ARMOR_SET_ATTRIB					CONSTLIT("useAsArmorSet")
#define USE_COMPLETE_ARMOR_ONLY_ATTRIB			CONSTLIT("useCompleteArmorOnly")
#define USE_ENABLED_ONLY_ATTRIB					CONSTLIT("useEnabledOnly")
#define USE_INSTALLED_ONLY_ATTRIB				CONSTLIT("useInstalledOnly")
#define USE_KEY_ATTRIB							CONSTLIT("useKey")
#define USE_SCREEN_ATTRIB						CONSTLIT("useScreen")
#define USE_UNINSTALLED_ONLY_ATTRIB				CONSTLIT("useUninstalledOnly")
#define VALUE_ATTRIB							CONSTLIT("value")
#define VALUE_BONUS_PER_CHARGE_ATTRIB			CONSTLIT("valueBonusPerCharge")
#define VALUE_CHARGES_ATTRIB					CONSTLIT("valueCharges")
#define VIRTUAL_ATTRIB							CONSTLIT("virtual")

#define GET_TRADE_PRICE_EVENT					CONSTLIT("GetTradePrice")
#define ON_ENABLED_EVENT						CONSTLIT("OnEnable")
#define ON_INSTALL_EVENT						CONSTLIT("OnInstall")
#define ON_REFUEL_TAG							CONSTLIT("OnRefuel")

#define STR_FUEL								CONSTLIT("Fuel")
#define STR_MISSILE								CONSTLIT("Missile")

#define CATEGORY_AMMO							CONSTLIT("ammo")
#define CATEGORY_ARMOR							CONSTLIT("armor")
#define CATEGORY_CARGO_HOLD						CONSTLIT("cargoHold")
#define CATEGORY_DRIVE							CONSTLIT("drive")
#define CATEGORY_FUEL							CONSTLIT("fuelItem")
#define CATEGORY_LAUNCHER						CONSTLIT("launcher")
#define CATEGORY_MISC_DEVICE					CONSTLIT("miscDevice")
#define CATEGORY_MISC_ITEM						CONSTLIT("miscItem")
#define CATEGORY_REACTOR						CONSTLIT("reactor")
#define CATEGORY_SHIELDS						CONSTLIT("shields")
#define CATEGORY_USABLE_ITEM					CONSTLIT("usableItem")
#define CATEGORY_WEAPON							CONSTLIT("weapon")

#define FIELD_AVERAGE_COUNT						CONSTLIT("averageCount")
#define FIELD_CATEGORY							CONSTLIT("category")
#define FIELD_COST								CONSTLIT("cost")
#define FIELD_DESCRIPTION						CONSTLIT("description")
#define FIELD_DEVICE_SLOTS						CONSTLIT("deviceSlots")
#define FIELD_FREQUENCY							CONSTLIT("frequency")
#define FIELD_FUEL_CAPACITY						CONSTLIT("fuelCapacity")
#define FIELD_IMAGE_DESC						CONSTLIT("imageDesc")
#define FIELD_INSTALL_COST						CONSTLIT("installCost")
#define FIELD_LEVEL								CONSTLIT("level")
#define FIELD_MASS								CONSTLIT("mass")
#define FIELD_NAME								CONSTLIT("name")
#define FIELD_SHORT_NAME						CONSTLIT("shortName")
#define FIELD_SLOT_CATEGORY						CONSTLIT("slotCategory")
#define FIELD_TREASURE_VALUE					CONSTLIT("treasureValue")
#define FIELD_UNKNOWN_TYPE						CONSTLIT("unknownType")
#define FIELD_USE_KEY							CONSTLIT("useKey")

#define PROPERTY_CATEGORY						CONSTLIT("category")
#define PROPERTY_COMPONENT_PRICE				CONSTLIT("componentPrice")
#define PROPERTY_COMPONENTS						CONSTLIT("components")
#define PROPERTY_CURRENCY						CONSTLIT("currency")
#define PROPERTY_CURRENCY_NAME					CONSTLIT("currencyName")
#define PROPERTY_DESCRIPTION					CONSTLIT("description")
#define PROPERTY_FREQUENCY 						CONSTLIT("frequency")
#define PROPERTY_KNOWN							CONSTLIT("known")
#define PROPERTY_LEVEL  						CONSTLIT("level")
#define PROPERTY_MAX_CHARGES  					CONSTLIT("maxCharges")
#define PROPERTY_MAX_LEVEL  					CONSTLIT("maxLevel")
#define PROPERTY_MIN_LEVEL  					CONSTLIT("minLevel")
#define PROPERTY_MASS_BONUS_PER_CHARGE			CONSTLIT("massBonusPerCharge")
#define PROPERTY_ROLE							CONSTLIT("role")
#define PROPERTY_USE_SCREEN						CONSTLIT("useScreen")
#define PROPERTY_VALUE_BONUS_PER_CHARGE			CONSTLIT("valueBonusPerCharge")
#define PROPERTY_WEAPON_TYPES					CONSTLIT("weaponTypes")

#define ROLE_CARGO_HOLD							CONSTLIT("cargoHold")
#define ROLE_DRIVE								CONSTLIT("drive")
#define ROLE_LAUNCHER							CONSTLIT("launcher")
#define ROLE_REACTOR							CONSTLIT("reactor")

#define SPECIAL_CAN_BE_DAMAGED					CONSTLIT("canBeDamaged:")
#define SPECIAL_CAN_BE_DISRUPTED				CONSTLIT("canBeDisrupted:")
#define SPECIAL_DAMAGE_TYPE						CONSTLIT("damageType:")
#define SPECIAL_HAS_COMPONENTS					CONSTLIT("hasComponents:")
#define SPECIAL_IS_LAUNCHER						CONSTLIT("isLauncher:")
#define SPECIAL_LAUNCHED_BY						CONSTLIT("launchedBy:")
#define SPECIAL_UNKNOWN_TYPE					CONSTLIT("unknownType:")

#define SPECIAL_TRUE							CONSTLIT("true")

static char g_NameAttrib[] = "name";
static char g_ObjectAttrib[] = "object";
static char g_MassAttrib[] = "mass";
static char g_DescriptionAttrib[] = "description";
static char g_RandomDamagedAttrib[] = "randomDamaged";

const int FLOTSAM_IMAGE_WIDTH =					32;

CItemType::SStdStats CItemType::m_Stats[MAX_ITEM_LEVEL] = 
    {
        //  Treasure Value
        {             750, },
        {           1'000, },
        {           1'500, },
        {           2'000, },
        {           4'000, },

        {           8'000, },
        {          16'000, },
        {          32'000, },
        {          64'000, },
        {         128'000, },

        {         256'000, },
        {         512'000, },
        {       1'000'000, },
        {       2'000'000, },
        {       4'100'000, },

        {       8'200'000, },
        {      16'400'000, },
        {      32'800'000, },
        {      65'500'000, },
        {     131'000'000, },

        {     262'000'000, },
        {     524'000'000, },
        {   1'000'000'000, },
        {   2'100'000'000, },
        {   4'200'000'000, },
    };

static char *CACHED_EVENTS[CItemType::evtCount] =
	{
		"GetDescription",
		"GetDisplayAttributes",
		"GetName",
		"GetReferenceText",
		"GetTradePrice",
		"OnDestroyCheck",
		"OnDocked",
		"OnEnable",
		"OnInstall",
		"OnRefuel",
		"GetHUDName",
	};

static TStaticStringTable<TStaticStringEntry<ItemFates>, 5> FATE_TABLE = {
	"alwaysComponetized",	fateComponetized,
	"alwaysDamaged",		fateDamaged,
	"alwaysDestroyed",		fateDestroyed,
	"alwaysSurvives",		fateSurvives,
	"default",				fateNone,
	};

static CStationType *g_pFlotsamStationType = NULL;

CItemType::CItemType (void) : 
		m_dwSpare(0),
		m_pComponents(NULL),
		m_pUseCode(NULL),
		m_pArmor(NULL),
		m_pDevice(NULL),
		m_pMissile(NULL)

//	CItemType constructor

	{
	}

CItemType::~CItemType (void)

//	CItemType destructor

	{
	if (m_pComponents)
		delete m_pComponents;

	if (m_pUseCode)
		m_pUseCode->Discard();

	if (m_pArmor)
		delete m_pArmor;

	if (m_pDevice)
		delete m_pDevice;

	if (m_pMissile)
		delete m_pMissile;
	}

bool CItemType::IsAmmunition (void) const

//	IsAmmunition
//
//	Determines whether this is a missile for a launcher or just ammunition for a normal weapon.

	{
	if (!IsMissile())
		return false;

	//	If any of the weapons that launches is is a missile launcher, then we
	//	count as a missile.

	for (int i = 0; i < m_Weapons.GetCount(); i++)
		if (m_Weapons[i]->GetItemType()->GetCategory() == itemcatLauncher)
			return false;

	//	Otherwise, we count as ammo

	return true;
	}

void CItemType::CreateEmptyFlotsam (CSystem &System, 
									const CVector &vPos, 
									const CVector &vVel, 
									CSovereign *pSovereign, 
									CStation **retpFlotsam)

//	CreateEmptyFlotsam
//
//	Creates an empty flotsam station

	{
	SSystemCreateCtx Ctx(System);

	SObjCreateCtx CreateCtx(Ctx);
	CreateCtx.vPos = vPos;
	CreateCtx.vVel = vVel;

	CStation *pFlotsam;
	CStation::CreateFromType(System,
			GetFlotsamStationType(),
			CreateCtx,
			&pFlotsam);

	//	Set properties

	pFlotsam->SetSovereign(pSovereign);
	pFlotsam->SetFlotsamImage(this);

	DWORD dwFlags;
	CString sName = GetNamePattern(0, &dwFlags);
	pFlotsam->SetName(sName, dwFlags);

	//	Done

	if (retpFlotsam)
		*retpFlotsam = pFlotsam;
	}

void CItemType::CreateFlotsamImage (void)

//	CreateFlotsamImage
//
//	Creates a 32x32 image used when the item is floating in space.

	{
	if (!m_Image.IsLoaded())
		return;

	RECT rcImage = m_Image.GetImageRect();
	int cxWidth = RectWidth(rcImage);
	int cyHeight = RectHeight(rcImage);

	//	Create a resized version of the image

	Metric rScale = (Metric)FLOTSAM_IMAGE_WIDTH / (Metric)cxWidth;
	m_FlotsamBitmap.CreateFromImageTransformed(m_Image.GetImage(CONSTLIT("Flotsam image")),
			rcImage.left,
			rcImage.top,
			cxWidth,
			cyHeight,
			rScale,
			rScale,
			0.0);

	//	Initialize an image

	RECT rcRect;
	rcRect.left = 0;
	rcRect.top = 0;
	rcRect.right = m_FlotsamBitmap.GetWidth();
	rcRect.bottom = m_FlotsamBitmap.GetHeight();
	m_FlotsamImage.InitFromBitmap(&m_FlotsamBitmap, rcRect, 0, 0, false);
	}

bool CItemType::FindDataField (const CString &sField, CString *retsValue) const

//	FindDataField
//
//	Returns meta-data

	{
	CItem Item(const_cast<CItemType *>(this), 1);

	//	Deal with the meta-data that we know about

	if (strEquals(sField, FIELD_LEVEL))
		*retsValue = strFromInt(GetLevel());

	else if (strEquals(sField, FIELD_CATEGORY))
		*retsValue = GetItemCategory(GetCategory());
	
	else if (strEquals(sField, FIELD_SLOT_CATEGORY))
		*retsValue = GetItemCategory(GetCategory());
	
	else if (strEquals(sField, FIELD_FREQUENCY))
		{
		switch (GetFrequency())
			{
			case ftCommon:
				*retsValue = CONSTLIT("C");
				break;

			case ftUncommon:
				*retsValue = CONSTLIT("UC");
				break;

			case ftRare:
				*retsValue = CONSTLIT("R");
				break;

			case ftVeryRare:
				*retsValue = CONSTLIT("VR");
				break;

			default:
				*retsValue = CONSTLIT("NR");
			}
		}
	else if (strEquals(sField, FIELD_FUEL_CAPACITY) && IsFuel())
		{
		Metric rFuelPerItem = strToInt(GetData(), 0);
		*retsValue = strFromInt((int)(rFuelPerItem / FUEL_UNITS_PER_STD_ROD));

		//	NOTE: When IsFuel() is FALSE we fall through to the else
		//	case, which asks reactor devices.
		}
	else if (strEquals(sField, FIELD_NAME))
		*retsValue = GetNounPhrase();

	else if (strEquals(sField, FIELD_MASS))
		*retsValue = strFromInt(CItem(const_cast<CItemType *>(this), 1).GetMassKg());
	
	else if (strEquals(sField, FIELD_SHORT_NAME))
		{
		CString sName = GetNounPhrase();
		int iPos = strFind(sName, CONSTLIT(" of "));
		if (iPos != -1)
			{
			CString sLead = strSubString(sName, 0, iPos);
			if (strEquals(sLead, CONSTLIT("barrel"))
					|| strEquals(sLead, CONSTLIT("bolt"))
					|| strEquals(sLead, CONSTLIT("case"))
					|| strEquals(sLead, CONSTLIT("chest"))
					|| strEquals(sLead, CONSTLIT("container"))
					|| strEquals(sLead, CONSTLIT("crate"))
					|| strEquals(sLead, CONSTLIT("keg"))
					|| strEquals(sLead, CONSTLIT("kilo"))
					|| strEquals(sLead, CONSTLIT("nodule"))
					|| strEquals(sLead, CONSTLIT("pair"))
					|| strEquals(sLead, CONSTLIT("ROM"))
					|| strEquals(sLead, CONSTLIT("segment"))
					|| strEquals(sLead, CONSTLIT("tank"))
					|| strEquals(sLead, CONSTLIT("tin"))
					|| strEquals(sLead, CONSTLIT("ton"))
					|| strEquals(sLead, CONSTLIT("vial")))
				*retsValue = strSubString(sName, iPos+4, -1);
			else
				*retsValue = sName;
			}
		else
			*retsValue = sName;
		}

	else if (strEquals(sField, FIELD_COST))
		*retsValue = strFromInt(GetValue(CItemCtx()));

	else if (strEquals(sField, FIELD_INSTALL_COST))
		{
		int iCost = Item.GetInstallCost();
		if (iCost == -1)
			*retsValue = NULL_STR;
		else
			*retsValue = strFromInt(iCost);
		}
	else if (strEquals(sField, FIELD_AVERAGE_COUNT))
		*retsValue = strFromInt(m_NumberAppearing.GetAveValue());

	else if (strEquals(sField, FIELD_DESCRIPTION))
		*retsValue = m_sDescription;

	else if (strEquals(sField, FIELD_IMAGE_DESC))
		{
		const RECT &rcImage = m_Image.GetImageRect();
		*retsValue = strPatternSubst(CONSTLIT("%s (%d,%d)"),
				m_Image.GetFilename(),
				rcImage.left,
				rcImage.top);
		}
	else if (strEquals(sField, FIELD_DEVICE_SLOTS))
		{
		if (m_pDevice)
			*retsValue = strFromInt(m_pDevice->GetSlotsRequired());
		else
			*retsValue = NULL_STR;
		}
	else if (strEquals(sField, FIELD_TREASURE_VALUE))
		*retsValue = strFromInt((int)CEconomyType::ExchangeToCredits(GetCurrencyType(), GetValue(CItemCtx(), true)));

	else if (strEquals(sField, FIELD_UNKNOWN_TYPE))
		{
		CItemType *pUnknownType = Item.GetUnknownType();
		*retsValue = (pUnknownType ? strPatternSubst(CONSTLIT("0x%x"), pUnknownType->GetUNID()) : NULL_STR);
		}

	else if (strEquals(sField, FIELD_USE_KEY))
		*retsValue = m_sUseKey;

	//	Otherwise, see if the device class knows

	else
		{
		bool bHandled;

		if (m_pDevice)
			bHandled = m_pDevice->FindDataField(sField, retsValue);
		else if (m_pArmor)
			bHandled = m_pArmor->FindDataField(sField, retsValue);

		//	If this is a missile, then find the device that launches
		//	it and ask it for the properties.

		else if (IsMissile())
			bHandled = CDeviceClass::FindAmmoDataField(const_cast<CItemType *>(this), sField, retsValue);
		else
			bHandled = false;
		
		//	If someone handled it, then we're done. Otherwise, pass it back to
		//	the design type.
		
		if (bHandled)
			return true;

		return CDesignType::FindDataField(sField, retsValue);
		}

	return true;
	}

ICCItemPtr CItemType::FindItemTypeBaseProperty (CCodeChainCtx &Ctx, const CString &sProperty) const

//	FindItemTypeBaseProperty
//
//	Returns a property specific to a generic type. We DO NOT return device/armor
//	properties, nor do we return CItem properties. But we DO return CDesignType
//	properties and we DO return old-style data fields (which DO have device/armor
//	parameters).
//
//	We return NULL if we do not understand the property.

	{
	CCodeChain &CC = GetUniverse().GetCC();
	ICCItemPtr pResultPtr;
	int i;

	if (strEquals(sProperty, PROPERTY_CATEGORY))
		return ICCItemPtr(GetItemCategoryID(GetCategory()));

	else if (strEquals(sProperty, PROPERTY_COMPONENT_PRICE))
		{
		int iTotalPrice = 0;
		for (i = 0; i < GetComponents().GetCount(); i++)
			{
			const CItem &Component = GetComponents().GetItem(i);
			iTotalPrice += Component.GetTradePrice(NULL, true) * Component.GetCount();
			}

		return (iTotalPrice > 0 ? ICCItemPtr(iTotalPrice) : ICCItemPtr(ICCItem::Nil));
		}

	else if (strEquals(sProperty, PROPERTY_COMPONENTS))
		{
		const CItemList &Components = GetComponents();
		if (Components.GetCount() == 0)
			return ICCItemPtr(ICCItem::Nil);

		ICCItem *pList = CC.CreateLinkedList();
		for (i = 0; i < Components.GetCount(); i++)
			{
			ICCItem *pEntry = CreateListFromItem(Components.GetItem(i));
			pList->Append(pEntry);
			pEntry->Discard();
			}

		return ICCItemPtr(pList);
		}

	else if (strEquals(sProperty, PROPERTY_CURRENCY))
		return ICCItemPtr(GetCurrencyType()->GetUNID());

	else if (strEquals(sProperty, PROPERTY_CURRENCY_NAME))
		return ICCItemPtr(GetCurrencyType()->GetSID());

	else if (strEquals(sProperty, PROPERTY_DESCRIPTION))
		return ICCItemPtr(GetDesc());

	else if (strEquals(sProperty, PROPERTY_FREQUENCY))
		return ICCItemPtr(GetFrequencyName((FrequencyTypes)GetFrequency()));

    else if (strEquals(sProperty, PROPERTY_KNOWN))
		{
		CItem Item(const_cast<CItemType *>(this), 1);
        return ICCItemPtr(Item.IsKnown());
		}

    else if (strEquals(sProperty, PROPERTY_LEVEL))
        return ICCItemPtr(GetLevel());

	else if (strEquals(sProperty, PROPERTY_MASS_BONUS_PER_CHARGE))
		return ICCItemPtr(GetMassBonusPerCharge());

	else if (strEquals(sProperty, PROPERTY_MAX_CHARGES))
		return ICCItemPtr(GetMaxCharges());

    else if (strEquals(sProperty, PROPERTY_MAX_LEVEL))
        return ICCItemPtr(GetMaxLevel());

    else if (strEquals(sProperty, PROPERTY_MIN_LEVEL))
        return ICCItemPtr(GetLevel());

	else if (strEquals(sProperty, PROPERTY_ROLE))
		return (!m_sRole.IsBlank() ? ICCItemPtr(m_sRole) : ICCItemPtr(ICCItem::Nil));

	else if (strEquals(sProperty, PROPERTY_USE_SCREEN))
		return (m_pUseScreen.GetUNID() ? ICCItemPtr(m_pUseScreen.GetUNID()) : ICCItemPtr::Nil());

	else if (strEquals(sProperty, PROPERTY_VALUE_BONUS_PER_CHARGE))
		return ICCItemPtr(GetValueBonusPerCharge());

	else if (strEquals(sProperty, PROPERTY_WEAPON_TYPES))
		{
		if (GetLaunchWeapons().GetCount() == 0)
			return ICCItemPtr(ICCItem::Nil);
		else
			{
			ICCItem *pResult = CC.CreateLinkedList();
			for (i = 0; i < GetLaunchWeapons().GetCount(); i++)
				pResult->AppendInteger(GetLaunchWeapons()[i]->GetUNID());

			return ICCItemPtr(pResult);
			}
		}

	else if (ICCItem *pResult = FindBaseProperty(Ctx, sProperty))
		return ICCItemPtr(pResult);

	else
		return NULL;
	}

CString CItemType::GenerateRandomName (const CString &sTemplate, const TArray<CString> &RetiredNames, bool *retbTempletized) const

//	GenerateRandomName
//
//	Generates a random name from a template, making sure that we don't repeat
//	any of the names in ioRetiredNames.

	{
	//	See if the template has replaceable parameters.

	char *pPos = sTemplate.GetASCIIZPointer();
	while (*pPos != '\0' && *pPos != '%')
		pPos++;

	bool bTempletized = (*pPos == '%');
	if (retbTempletized)
		*retbTempletized = bTempletized;

	//	If the template has replaceable parameters, then generate
	//	a unique name

	if (bTempletized)
		{
		int iMaxLoops = 100;

		CString sTry;
		bool bDuplicate;
		do
			{
			sTry = ::GenerateRandomNameFromTemplate(sTemplate);
			bDuplicate = false;

			for (int k = 0; k < RetiredNames.GetCount(); k++)
				if (strEquals(sTry, RetiredNames[k]))
					{
					bDuplicate = true;
					break;
					}
			}
		while (bDuplicate && iMaxLoops-- > 0);

		return sTry;
		}
	else
		return sTemplate;
	}

CDeviceClass *CItemType::GetAmmoLauncher (int *retiVariant) const

//	GetAmmoLauncher
//
//	Returns a weapon that can launch this ammo (or NULL)

	{
	return (m_Weapons.GetCount() > 0 ? m_Weapons[0] : NULL);
	}

ItemCategories CItemType::GetCategory (void) const

//	GetCategory
//
//	Returns the category of the item

	{
	if (m_pArmor)
		return itemcatArmor;
	else if (m_pDevice)
		return m_pDevice->GetCategory();
	else
		{
		if (IsUsable())
			return itemcatUseful;
		else if (IsFuel())
			return itemcatFuel;
		else if (IsMissile())
			return itemcatMissile;
		else
			return itemcatMisc;
		}
	}

ItemCategories CItemType::GetCategoryForNamedDevice (DeviceNames iDev)

//	GetCategoryForNamedDevice
//
//	Returns the device category that is selected for
//	this named device slot

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
		}

	return itemcatMiscDevice;
	}

CCurrencyAndValue CItemType::GetCurrencyAndValue (CItemCtx &Ctx, bool bActual) const

//  GetCurrencyAndValue
//
//  Returns the value of the item and its currency.

    {
	//	NOTE: We have got that guaranteed m_pUnknownType is non-NULL if IsKnown is FALSE.

	int iUnknownIndex;
	if (!Ctx.GetItem().IsKnown(&iUnknownIndex) && !bActual)
		return m_UnknownTypes[iUnknownIndex].pUnknownType->GetCurrencyAndValue(CItemCtx());

	//	If this is a scalable item, then we need to ask the class

    CurrencyValue iValue;
    if (IsScalable())
        {
        if (m_pArmor)
            iValue = CurrencyValue(m_iValue.GetValue() * m_pArmor->GetScaledCostAdj(Ctx));
        else if (m_pDevice)
            iValue = CurrencyValue(m_iValue.GetValue() * m_pDevice->GetScaledCostAdj(Ctx));
        else
            iValue = m_iValue.GetValue();
        }
    else
        iValue = m_iValue.GetValue();

	//	If we need to account for charges, then do it

	if (m_iExtraValuePerCharge != 0)
		{
		if (Ctx.IsItemNull())
			iValue = Max((CurrencyValue)0, iValue + (m_InitDataValue.GetAveValue() * m_iExtraValuePerCharge));
		else
			iValue = Max((CurrencyValue)0, iValue + (Ctx.GetItem().GetCharges() * m_iExtraValuePerCharge));
		}

	else if (m_fValueCharges && !Ctx.IsItemNull())
		{
		int iMaxCharges = GetMaxCharges();

		if (iMaxCharges > 0)
			iValue = (iValue * (1 + Ctx.GetItem().GetCharges())) / (1 + iMaxCharges);
		}

    //  Done

    return CCurrencyAndValue(iValue, m_iValue.GetCurrencyType());
    }

const CString &CItemType::GetDesc (bool bActual) const

//	GetDesc
//
//	Get description for the item
	
	{
	CItem Item(const_cast<CItemType *>(this), 1);

	int iUnknownIndex;
	if (!bActual && !Item.IsKnown(&iUnknownIndex))
		return m_UnknownTypes[iUnknownIndex].pUnknownType->GetDesc();

	return m_sDescription; 
	}

CStationType *CItemType::GetFlotsamStationType (void)

//	GetFlotsamStationType
//
//	Returns the station type to use for flotsam

	{
	if (g_pFlotsamStationType == NULL)
		g_pFlotsamStationType = GetUniverse().FindStationType(FLOTSAM_UNID);

	return g_pFlotsamStationType;
	}

int CItemType::GetFrequencyByLevel (int iLevel)

//	GetFrequencyByLevel
//
//	Returns the frequency of the item at the given system level.

	{
	int iFrequency = GetFrequency();
	int iItemLevel = GetLevel();
	int iDiff = Absolute(iItemLevel - iLevel);

	switch (iDiff)
		{
		case 0:
			return iFrequency;

		case 1:
			return iFrequency * ftUncommon / ftCommon;

		case 2:
			return iFrequency * ftRare / ftCommon;

		case 3:
			return iFrequency * ftVeryRare / ftCommon;

		default:
			return 0;
		}
	}

CString CItemType::GetItemCategory (ItemCategories iCategory)

//	GetItemCategory
//
//	Returns the identifier for an item category

	{
	switch (iCategory)
		{
		case itemcatArmor:
			return CATEGORY_ARMOR;

		case itemcatCargoHold:
			return CATEGORY_CARGO_HOLD;

		case itemcatDrive:
			return CATEGORY_DRIVE;

		case itemcatFuel:
			return CATEGORY_FUEL;

		case itemcatLauncher:
			return CATEGORY_LAUNCHER;

		case itemcatMisc:
			return CATEGORY_MISC_ITEM;

		case itemcatMissile:
			return CATEGORY_AMMO;

		case itemcatMiscDevice:
			return CATEGORY_MISC_DEVICE;

		case itemcatReactor:
			return CATEGORY_REACTOR;

		case itemcatShields:
			return CATEGORY_SHIELDS;

		case itemcatUseful:
			return CATEGORY_USABLE_ITEM;

		case itemcatWeapon:
			return CATEGORY_WEAPON;

		default:
			ASSERT(false);
			return CONSTLIT("unknown");
		}
	}

int CItemType::GetMassKg (CItemCtx &Ctx) const

//	GetMassKg
//
//	Returns the mass of the item in kilograms

	{
	if (m_iExtraMassPerCharge)
		{
		if (Ctx.IsItemNull())
			return m_iMass + (m_InitDataValue.GetAveValue() * m_iExtraMassPerCharge);
		else
			return m_iMass + (Ctx.GetItem().GetCharges() * m_iExtraMassPerCharge);
		}
	else
		return m_iMass;
	}

int CItemType::GetMaxHPBonus (void) const

//	GetMaxHPBonus
//
//	Returns the maximum possible HP bonus for this item.

	{
	if (m_pArmor)
		return m_pArmor->GetMaxHPBonus();
	else if (m_pDevice)
		return m_pDevice->GetMaxHPBonus();
	else
		return 150;
	}

int CItemType::GetRandomUnknownTypeIndex (void) const

//	GetRandomUnknownTypeIndex
//
//	Generate a random index for an unknown type (or -1 if this item type is not
//	unknown).

	{
	if (m_UnknownTypes.GetCount() == 0)
		return -1;

	//	LATER: Probability should be based on frequency of unknown items.

	return mathRandom(0, m_UnknownTypes.GetCount() - 1);
	}

CString CItemType::GetReference (CItemCtx &Ctx, const CItem &Ammo, DWORD dwFlags) const

//	GetReference
//
//	Returns reference string

	{
	CArmorClass *pArmor;
	CDeviceClass *pDevice;

	//	No reference if unknown

	if (!Ctx.GetItem().IsKnown() && !(dwFlags & FLAG_ACTUAL_ITEM))
		return NULL_STR;

	//	Return armor reference, if this is armor

	if (pArmor = GetArmorClass())
		return pArmor->GetReference(Ctx);

	//	Return device reference, if this is a device

	else if (pDevice = GetDeviceClass())
		return pDevice->GetReference(Ctx, Ammo);

	//	If a missile, then get the reference from the weapon

	else if (IsMissile() && (pDevice = GetAmmoLauncher()))
		return pDevice->GetReference(Ctx, CItem(const_cast<CItemType *>(this), 1));

	//	Otherwise, nothing

	else
		return NULL_STR;
	}

CString CItemType::GetSortName (CItemCtx &Ctx) const

//	GetSortName
//
//	Returns the sort name

	{
	int iUnknownIndex;
	if (!Ctx.GetItem().IsKnown(&iUnknownIndex) && !m_UnknownTypes[iUnknownIndex].sUnknownName.IsBlank())
		return m_UnknownTypes[iUnknownIndex].sUnknownName;
	else
		return m_sSortName;
	}

const CItemType::SStdStats &CItemType::GetStdStats (int iLevel)

//  GetStdStats
//
//  Returns standard stats for the level.

    {
    ASSERT(iLevel >= 1 && iLevel <= MAX_ITEM_LEVEL);
    return m_Stats[iLevel - 1];
    }

CCurrencyAndValue CItemType::GetTradePrice (const CSpaceObject *pObj, bool bActual) const

//	GetTradePrice
//
//	Returns the trade price.

	{
	CItem Item(const_cast<CItemType *>(this), 1);
	return CCurrencyAndValue(Item.GetTradePrice(pObj, bActual), GetCurrencyType());
	}

CString CItemType::GenerateUnknownName (int iIndex, DWORD *retdwFlags)

//	GenerateUnknownName
//
//	Returns the unknown name of the item

	{
	if (iIndex >= 0 && iIndex < m_UnknownNames.GetCount())
		return GenerateRandomName(m_UnknownNames[iIndex]);

	return GetNamePattern(0, retdwFlags);
	}

bool CItemType::GetUseDesc (SUseDesc *retDesc) const

//	GetUseDesc
//
//	Returns data about how the item can be used.

	{
	CDesignType *pInherit;

	//	If we have information about usage, we return what we have.

	if ((m_pUseCode != NULL) || (!m_pUseScreen.IsEmpty()))
		{
		if (retDesc)
			{
			retDesc->pExtension = GetExtension();
			retDesc->pCode = m_pUseCode;
			retDesc->pScreenRoot = GetUseScreen(&retDesc->sScreenName);
			retDesc->sUseKey = m_sUseKey;
			retDesc->bUsable = true;
			retDesc->bUsableInCockpit = (m_pUseCode != NULL);
			retDesc->bOnlyIfEnabled = m_fUseEnabled;
			retDesc->bOnlyIfInstalled = m_fUseInstalled;
			retDesc->bOnlyIfUninstalled = m_fUseUninstalled;
			retDesc->bOnlyIfCompleteArmor = m_fUseCompleteArmor;
			retDesc->bAsArmorSet = m_fUseAsArmorSet;
			}

		return true;
		}

	//	Otherwise, see if our ancestor handles this

	else if (pInherit = GetInheritFrom())
		{
		CItemType *pInheritItem = CItemType::AsType(pInherit);
		if (pInheritItem == NULL)
			return false;

		return pInheritItem->GetUseDesc(retDesc);
		}

	//	Otherwise, nothing

	else
		return false;
	}

CWeaponFireDesc *CItemType::GetWeaponFireDesc (CItemCtx &Ctx, CString *retsError) const

//  GetWeaponFireDesc
//
//  Returns the weapon fire descriptor for the item.
//
//  The recommended usage pattern is to have Ctx based on the installed device
//  and an variant item representing the missile. In that case, we get the same
//  result whether invoked on the weapon or the missile.
//
//  The result depends on this item and Ctx as follows:
//
//                              this->IsMissile()           this->IsWeapon()
//  ----------------------------------------------------------------------------
//	Ctx=null					missile desc, for			weapon desc (use first
//									first weapon that			variant for 
//									fires this missile.			launchers).
//
//  Ctx=installed weapon        missile desc, if            weapon desc (use variant
//                                  fired by weapon.            item for missile).
//
//  Ctx=weapon item             missile desc, if            weapon desc (use variant
//                                  fired by weapon.            item for missile).
//
//  Ctx=missile item            missile desc, for           missile desc, if fired
//                                  first weapon that           by weapon.
//                                  fires this missile.
//
//  If anything goes wrong (e.g., if we don't have enough information) we return
//  NULL and an optional error explanation.

    {
    //  We need to end up with the following pieces of data:
    //
    //  1.  A CItemCtx for the installed weapon (or at least just the weapon
    //      item).
    //  2.  A CWeaponClass to call.
    //  3.  An optional CItem for the missile/ammo being fired.

    CItemCtx *pWeaponCtx = NULL;
    CWeaponClass *pWeaponClass = NULL;
    const CItem *pAmmo = NULL;

    CItemCtx WeaponCtx;
    CItem Ammo;

    //  We go through different paths depending on the item context that was
    //  passed in.
    //
    //  If the item context is a weapon (installed or not)

    if (Ctx.GetDeviceClass())
        {
        pWeaponCtx = &Ctx;

        pWeaponClass = pWeaponCtx->GetDeviceClass()->AsWeaponClass();
        if (pWeaponClass == NULL)
            {
            if (retsError)
                *retsError = strPatternSubst("Item %08x is not a weapon or missile.", pWeaponCtx->GetDeviceClass()->GetUNID());
            return NULL;
            }

        //  If we have ammo, then get the ammo item.

        if (pWeaponClass->GetAmmoItemCount() > 0)
            {
            //  If we're a missile, then we cons up an item

            if (GetDeviceClass() == NULL
                    && pWeaponClass->GetAmmoVariant(const_cast<CItemType *>(this)))
                {
                Ammo = CItem(const_cast<CItemType *>(this), 1);
                pAmmo = &Ammo;
                }

            //  If we have an installed device, then we omit the ammo, because
            //  GetWeaponFireDesc below will get the ammo from the device.

            else if (Ctx.GetDevice())
                pAmmo = NULL;

            //  We prefer the item from the context. NOTE: We assume that the
            //  context has already validated that this ammo is compatible.

#if 0
            else if (!(pAmmo = &Ctx.GetVariantItem())->IsEmpty())
                { }
#endif

            //  If we are the same weapon, then we use the first ammo item

            else if (GetUNID() == pWeaponClass->GetUNID())
                {
                Ammo = CItem(pWeaponClass->GetAmmoItem(0), 1);
                pAmmo = &Ammo;
                }

            //  Otherwise, error

            else
                {
                if (retsError)
                    *retsError = strPatternSubst("Item %08x is not compatible with weapon %08x.", GetUNID(), pWeaponClass->GetUNID());
                return NULL;
                }
            }
        }

    //  If the item context is an item, then we expect it to be ammo for us.
    //  (It can't be the weapon, because otherwise we'd be in the code above.)

    else if (!Ctx.GetItem().IsEmpty())
        {
        WeaponCtx = CItemCtx(const_cast<CItemType *>(this));
        pWeaponCtx = &WeaponCtx;

        pWeaponClass = (GetDeviceClass() ? GetDeviceClass()->AsWeaponClass() : NULL);
        if (pWeaponClass == NULL)
            {
            if (retsError)
                *retsError = strPatternSubst("Item %08x is not a weapon.", GetUNID());
            return NULL;
            }

        //  Make sure the context is ammo for the weapon.

        if (pWeaponClass->GetAmmoVariant(Ctx.GetItem().GetType()) != -1)
            pAmmo = &Ctx.GetItem();
        else
            {
            if (retsError)
                *retsError = strPatternSubst("Item %08x is not compatible with weapon %08x.", Ctx.GetItem().GetType()->GetUNID(), pWeaponClass->GetUNID());
            return NULL;
            }
        }

    //  Otherwise, if we're a weapon, then we cons something up

    else if (GetDeviceClass() && (pWeaponClass = GetDeviceClass()->AsWeaponClass()))
        {
        WeaponCtx = CItemCtx(const_cast<CItemType *>(this));
        pWeaponCtx = &WeaponCtx;

        //  Choose the first ammo for this weapon (if necessary)

        if (pWeaponClass->GetAmmoItemCount() > 0)
            {
            Ammo = CItem(pWeaponClass->GetAmmoItem(0), 1);
            pAmmo = &Ammo;
            }
        }

    //  Otherwise we expect to be ammo for some weapon.

    else
        {
        Ammo = CItem(const_cast<CItemType *>(this), 1);
        pAmmo = &Ammo;

	    CDeviceClass *pDeviceClass;
        if (!CDeviceClass::FindWeaponFor(Ammo.GetType(), &pDeviceClass)
                || (pWeaponClass = pDeviceClass->AsWeaponClass()) == NULL)
            {
            if (retsError)
                *retsError = strPatternSubst("Item %08x is not a weapon or missile.", GetUNID());
            return NULL;
            }

        WeaponCtx = CItemCtx(pWeaponClass->GetItemType());
        pWeaponCtx = &WeaponCtx;
        }

    //  Now we have all the pieces.

    return pWeaponClass->GetWeaponFireDesc(*pWeaponCtx, (pAmmo ? *pAmmo : CItem()));
    }

void CItemType::InitComponents (void)

//	InitComponents
//
//	Initializes m_Components

	{
	DEBUG_TRY

	m_Components.DeleteAll();
	if (m_pComponents)
		{
		CItemListManipulator ItemList(m_Components);
		SItemAddCtx Ctx(ItemList);
		Ctx.iLevel = GetLevel();

		m_pComponents->AddItems(Ctx);
		}

	DEBUG_CATCH
	}

void CItemType::InitRandomNames (void)

//	InitRandomNames
//
//	Initialize random names. NOTE: This is called from inside BindDesign, so we
//	need to deal with the fact that other item types might not yet be bound.

	{
	DEBUG_TRY

	//	If we don't have random names for other items then we're done

	int iCount = m_UnknownNames.GetCount();
	if (iCount == 0)
		return;

	//	Randomize the names

	TArray<int> Randomize;
	Randomize.InsertEmpty(iCount);
	for (int i = 0; i < iCount; i++)
		Randomize[i] = i;

	Randomize.Shuffle();

	//	Keep track of randomly generated names

	TArray<CString> RetiredNames;

	//	Loop over all items and assign each item that has us as the
	//	unknown placeholder.
	//
	//	NOTE: This code must work even if some of the items are not yet bound.

	int j = 0;
	for (int i = 0; i < GetUniverse().GetItemTypeCount(); i++)
		{
		CItemType *pType = GetUniverse().GetItemType(i);
		int iUnknownIndex;
		if (pType->IsUnknownType(GetUNID(), &iUnknownIndex))
			{
			CString sTemplate = m_UnknownNames[Randomize[j % iCount]];

			bool bTempletized;
			CString sName = GenerateRandomName(sTemplate, RetiredNames, &bTempletized);

			pType->SetUnknownName(iUnknownIndex, sName);

			//	If the template had randomized parameters, then make sure we 
			//	don't repeat them.

			if (bTempletized)
				RetiredNames.Insert(sName);

			j++;
			}
		}

	DEBUG_CATCH
	}

bool CItemType::IsFuel (void) const

//	IsFuel
//
//	Returns TRUE if this is fuel

	{
	return HasLiteralAttribute(STR_FUEL);
	}

bool CItemType::IsMissile (void) const

//	IsMissile
//
//	Returns TRUE if this is a missile/ammo fired by some weapon.

	{
	if (m_pMissile)
		return true;

	else if (m_Weapons.GetCount() > 0)
		return true;

	else
		return HasLiteralAttribute(STR_MISSILE);
	}

bool CItemType::IsUnknownType (DWORD dwUNID, int *retiUnknownIndex) const

//	IsUnknownType
//
//	Returns TRUE if this has the given UNID as one of its unknown item types.

	{
	for (int i = 0; i < m_UnknownTypes.GetCount(); i++)
		if (m_UnknownTypes[i].pUnknownType.GetUNID() == dwUNID)
			{
			if (retiUnknownIndex)
				*retiUnknownIndex = i;
			return true;
			}

	return false;
	}

void CItemType::OnAccumulateXMLMergeFlags (TSortMap<DWORD, DWORD> &MergeFlags) const

//	OnAccumulateXMLMergeFlags
//
//	Set merge flags for XML inheritance.

	{
	//	We can handle inherited invoke.

	MergeFlags.SetAt(CXMLElement::GetKeywordID(COCKPIT_USE_TAG), CXMLElement::MERGE_OVERRIDE);
	}

void CItemType::OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed)

//	OnAddTypesUsed
//
//	Adds to the list of types used by this type.

	{
	retTypesUsed->SetAt(m_Image.GetBitmapUNID(), true);

	for (int i = 0; i < m_UnknownTypes.GetCount(); i++)
		retTypesUsed->SetAt(m_UnknownTypes[i].pUnknownType.GetUNID(), true);

	retTypesUsed->SetAt(strToInt(m_pUseScreen.GetUNID(), 0), true);

	if (m_pComponents)
		m_pComponents->AddTypesUsed(retTypesUsed);

	if (m_pArmor)
		m_pArmor->AddTypesUsed(retTypesUsed);

	if (m_pDevice)
		m_pDevice->AddTypesUsed(retTypesUsed);

	if (m_pMissile)
		m_pMissile->AddTypesUsed(retTypesUsed);
	}

ALERROR CItemType::OnBindDesign (SDesignLoadCtx &Ctx)

//	OnBindDesign
//
//	Bind all pointers to other design elements

	{
	ALERROR error;

	InitRandomNames();

	//	Images

	if (error = m_Image.OnDesignLoadComplete(Ctx))
		return error;

	//	Cache some events

	InitCachedEvents(evtCount, CACHED_EVENTS, m_CachedEvents);

	//	Resolve some pointers

	for (int i = 0; i < m_UnknownTypes.GetCount(); i++)
		if (error = m_UnknownTypes[i].pUnknownType.Bind(Ctx))
			return error;

	//	Others

	if (error = m_pUseScreen.Bind(Ctx, GetLocalScreens()))
		return error;

	if (error = m_iValue.Bind(Ctx))
		return error;

	//	Call contained objects

	if (m_pComponents)
		if (error = m_pComponents->OnDesignLoadComplete(Ctx))
			return error;

	if (m_pDevice)
		if (error = m_pDevice->Bind(Ctx))
			return error;

	if (m_pArmor)
		if (error = m_pArmor->OnBindDesign(Ctx))
			return error;

	if (m_pMissile)
		if (error = m_pMissile->OnDesignLoadComplete(Ctx))
			return error;

	return NOERROR;
	}

ALERROR CItemType::OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	OnCreateFromXML
//
//	Load data from XML

	{
	ALERROR error;
	int i;

    //  Load the level. If this is a range, then it is a scalable item.

    m_fScalable = pDesc->GetAttributeIntegerRange(LEVEL_ATTRIB, &m_iLevel, &m_iMaxLevel, 1, MAX_ITEM_LEVEL, 1, 1);

	//	Initialize basic info

	if (!pDesc->FindAttribute(PLURAL_NAME_ATTRIB, &m_sName))
		m_sName = pDesc->GetAttribute(CONSTLIT(g_NameAttrib));

	m_dwNameFlags = CLanguage::LoadNameFlags(pDesc);

	m_sSortName = pDesc->GetAttribute(SORT_NAME_ATTRIB);
	if (m_sSortName.IsBlank())
		m_sSortName = m_sName;
	m_sRole = pDesc->GetAttribute(ROLE_ATTRIB);

	m_iMass = pDesc->GetAttributeInteger(CONSTLIT(g_MassAttrib));

	if (error = m_iValue.InitFromXML(Ctx, pDesc->GetAttribute(VALUE_ATTRIB)))
		return ComposeLoadError(Ctx, Ctx.sError);

	//	Initialize frequency

	m_Frequency = (FrequencyTypes)::GetFrequency(pDesc->GetAttribute(FREQUENCY_ATTRIB));
	CString sNumberAppearing = pDesc->GetAttribute(NUMBER_APPEARING_ATTRIB);
	if (sNumberAppearing.IsBlank())
		m_NumberAppearing = DiceRange(0, 0, 1);
	else
		{
		if (error = m_NumberAppearing.LoadFromXML(sNumberAppearing))
			return ComposeLoadError(Ctx, CONSTLIT("Unable to parse numberAppearing"));
		}

	//	Get the unknown type info

	CString sUNID;
	if (pDesc->FindAttribute(UNKNOWN_TYPE_ATTRIB, &sUNID))
		{
		TArray<CString> UNIDs;
		ParseStringList(sUNID, 0, &UNIDs);

		m_UnknownTypes.GrowToFit(UNIDs.GetCount());
		for (int i = 0; i < UNIDs.GetCount(); i++)
			{
			int iIndex = m_UnknownTypes.GetCount();
			m_UnknownTypes.InsertEmpty(1);

			if (error = m_UnknownTypes[iIndex].pUnknownType.LoadUNID(Ctx, UNIDs[i]))
				return error;

			if (m_UnknownTypes[iIndex].pUnknownType.GetUNID() == 0)
				m_UnknownTypes.Delete(iIndex);
			}
		}

	//	Get the reference info

	m_fDefaultReference = pDesc->GetAttributeBool(SHOW_REFERENCE_ATTRIB);
	m_fReference = m_fDefaultReference;

	//	More fields

	m_sDescription = strCEscapeCodes(pDesc->GetAttribute(CONSTLIT(g_DescriptionAttrib)));
	m_sData = pDesc->GetAttribute(DATA_ATTRIB);
	m_dwModCode = pDesc->GetAttributeInteger(ENHANCEMENT_ATTRIB);

	//	Handle charges

	CString sInstData;
	if (m_fInstanceData = pDesc->FindAttribute(INSTANCE_DATA_ATTRIB, &sInstData))
		m_InitDataValue.LoadFromXML(sInstData);

	m_iMaxCharges = pDesc->GetAttributeIntegerBounded(MAX_CHARGES_ATTRIB, 0, -1, -1);

	m_iExtraMassPerCharge = pDesc->GetAttributeIntegerBounded(MASS_BONUS_PER_CHARGE_ATTRIB, 0, -1, 0);
	m_iExtraValuePerCharge = pDesc->GetAttributeInteger(VALUE_BONUS_PER_CHARGE_ATTRIB);	//	May be negative
	m_fAmmoCharges = pDesc->GetAttributeBool(AMMO_CHARGES_ATTRIB);
	m_fValueCharges = pDesc->GetAttributeBool(VALUE_CHARGES_ATTRIB);

	//	Flags

	m_fNoSaleIfUsed = pDesc->GetAttributeBool(NO_SALE_IF_USED_ATTRIB);
	m_fRandomDamaged = pDesc->GetAttributeBool(CONSTLIT(g_RandomDamagedAttrib));
	m_fVirtual = pDesc->GetAttributeBool(VIRTUAL_ATTRIB) || m_sName.IsBlank();
	if (m_fVirtual)
		m_Frequency = ftNotRandom;

	//	Use screen

	m_pUseScreen.LoadUNID(Ctx, pDesc->GetAttribute(USE_SCREEN_ATTRIB));
	m_sUseKey = pDesc->GetAttribute(USE_KEY_ATTRIB);
	m_fUseInstalled = pDesc->GetAttributeBool(USE_INSTALLED_ONLY_ATTRIB);
	m_fUseUninstalled = pDesc->GetAttributeBool(USE_UNINSTALLED_ONLY_ATTRIB);
	m_fUseEnabled = pDesc->GetAttributeBool(USE_ENABLED_ONLY_ATTRIB);
	m_fUseCompleteArmor = pDesc->GetAttributeBool(USE_COMPLETE_ARMOR_ONLY_ATTRIB);
	m_fUseAsArmorSet = pDesc->GetAttributeBool(USE_AS_ARMOR_SET_ATTRIB);
	m_fShowChargesInUseMenu = pDesc->GetAttributeBool(SHOW_CHARGES_IN_USE_MENU);
	
	//	Process sub-elements

	for (i = 0; i < pDesc->GetContentElementCount(); i++)
		{
		CXMLElement *pSubDesc = pDesc->GetContentElement(i);

		//	Process image

		if (strEquals(pSubDesc->GetTag(), IMAGE_TAG))
			{
			if (error = m_Image.InitFromXML(Ctx, *pSubDesc))
				return ComposeLoadError(Ctx, CONSTLIT("Unable to load image"));
			}

		//	Process unknown names

		else if (strEquals(pSubDesc->GetTag(), NAMES_TAG))
			strDelimitEx(pSubDesc->GetContentText(0), ';', DELIMIT_TRIM_WHITESPACE, 0, &m_UnknownNames);

		//	Process use code

		else if (strEquals(pSubDesc->GetTag(), COCKPIT_USE_TAG))
			{
			m_pUseCode = GetUniverse().GetCC().Link(pSubDesc->GetContentText(0));
			if (m_pUseCode->IsError())
				return ComposeLoadError(Ctx, strPatternSubst(CONSTLIT("<Invoke> event: %s"), m_pUseCode->GetStringValue()));

			//	These are also set in the main desc; we only override if we find them here

			pSubDesc->FindAttribute(KEY_ATTRIB, &m_sUseKey);

			bool bValue;
			if (pSubDesc->FindAttributeBool(INSTALLED_ONLY_ATTRIB, &bValue))
				m_fUseInstalled = bValue;

			if (pSubDesc->FindAttributeBool(UNINSTALLED_ONLY_ATTRIB, &bValue))
				m_fUseUninstalled = bValue;

			if (pSubDesc->FindAttributeBool(ENABLED_ONLY_ATTRIB, &bValue))
				m_fUseEnabled = bValue;

			if (pSubDesc->FindAttributeBool(COMPLETE_ARMOR_ONLY_ATTRIB, &bValue))
				m_fUseCompleteArmor = bValue;

			if (pSubDesc->FindAttributeBool(AS_ARMOR_SET_ATTRIB, &bValue))
				m_fUseAsArmorSet = bValue;
			}

		//	Process on refuel code

		else if (strEquals(pSubDesc->GetTag(), ON_REFUEL_TAG))
			{
			if (error = AddEventHandler(ON_REFUEL_TAG, pSubDesc->GetContentText(0), &Ctx.sError))
				return ComposeLoadError(Ctx, CONSTLIT("Unable to load OnRefuel event"));
			}

		//	Components

		else if (strEquals(pSubDesc->GetTag(), COMPONENTS_TAG))
			{
			if (error = IItemGenerator::CreateFromXML(Ctx, pSubDesc, &m_pComponents))
				return ComposeLoadError(Ctx, strPatternSubst(CONSTLIT("Unable to load Components: %s"), Ctx.sError));
			}

		//	Armor

		else if (strEquals(pSubDesc->GetTag(), ARMOR_TAG))
			{
			if (error = CArmorClass::CreateFromXML(Ctx, pSubDesc, this, &m_pArmor))
				return ComposeLoadError(Ctx, strPatternSubst(CONSTLIT("Unable to load %s: %s"), pSubDesc->GetTag(), Ctx.sError));
			}

		//	Devices

		else if (strEquals(pSubDesc->GetTag(), WEAPON_CLASS_TAG))
			{
			if (error = CWeaponClass::CreateFromXML(Ctx, pSubDesc, this, &m_pDevice))
				return ComposeLoadError(Ctx, strPatternSubst(CONSTLIT("Unable to load %s: %s"), pSubDesc->GetTag(), Ctx.sError));
			}
		else if (strEquals(pSubDesc->GetTag(), SHIELD_CLASS_TAG))
			{
			CShieldClass::SInitCtx InitCtx;
			InitCtx.pType = this;

			if (error = CShieldClass::CreateFromXML(Ctx, InitCtx, pSubDesc, &m_pDevice))
				return ComposeLoadError(Ctx, strPatternSubst(CONSTLIT("Unable to load %s: %s"), pSubDesc->GetTag(), Ctx.sError));

			//	For backwards compatibility, CShieldClass can set our max charges.

			if (InitCtx.iMaxCharges != -1)
				m_iMaxCharges = InitCtx.iMaxCharges;
			}
		else if (strEquals(pSubDesc->GetTag(), DRIVE_CLASS_TAG))
			{
			if (error = CDriveClass::CreateFromXML(Ctx, pSubDesc, this, &m_pDevice))
				return ComposeLoadError(Ctx, strPatternSubst(CONSTLIT("Unable to load %s: %s"), pSubDesc->GetTag(), Ctx.sError));
			}
		else if (strEquals(pSubDesc->GetTag(), CARGO_HOLD_CLASS_TAG))
			{
			if (error = CCargoSpaceClass::CreateFromXML(Ctx, pSubDesc, this, &m_pDevice))
				return ComposeLoadError(Ctx, strPatternSubst(CONSTLIT("Unable to load %s: %s"), pSubDesc->GetTag(), Ctx.sError));
			}
		else if (strEquals(pSubDesc->GetTag(), ENHANCER_CLASS_TAG))
			{
			if (error = CEnhancerClass::CreateFromXML(Ctx, pSubDesc, this, &m_pDevice))
				return ComposeLoadError(Ctx, strPatternSubst(CONSTLIT("Unable to load %s: %s"), pSubDesc->GetTag(), Ctx.sError));
			}
		else if (strEquals(pSubDesc->GetTag(), REPAIRER_CLASS_TAG))
			{
			if (error = CRepairerClass::CreateFromXML(Ctx, pSubDesc, this, &m_pDevice))
				return ComposeLoadError(Ctx, strPatternSubst(CONSTLIT("Unable to load %s: %s"), pSubDesc->GetTag(), Ctx.sError));
			}
		else if (strEquals(pSubDesc->GetTag(), SOLAR_DEVICE_CLASS_TAG))
			{
			if (error = CSolarDeviceClass::CreateFromXML(Ctx, pSubDesc, this, &m_pDevice))
				return ComposeLoadError(Ctx, strPatternSubst(CONSTLIT("Unable to load %s: %s"), pSubDesc->GetTag(), Ctx.sError));
			}
		else if (strEquals(pSubDesc->GetTag(), CYBER_DECK_CLASS_TAG))
			{
			if (error = CCyberDeckClass::CreateFromXML(Ctx, pSubDesc, this, &m_pDevice))
				return ComposeLoadError(Ctx, strPatternSubst(CONSTLIT("Unable to load %s: %s"), pSubDesc->GetTag(), Ctx.sError));
			}
		else if (strEquals(pSubDesc->GetTag(), AUTO_DEFENSE_CLASS_TAG))
			{
			if (error = CAutoDefenseClass::CreateFromXML(Ctx, pSubDesc, this, &m_pDevice))
				return ComposeLoadError(Ctx, strPatternSubst(CONSTLIT("Unable to load %s: %s"), pSubDesc->GetTag(), Ctx.sError));
			}
		else if (strEquals(pSubDesc->GetTag(), MISCELLANEOUS_CLASS_TAG))
			{
			if (error = CMiscellaneousClass::CreateFromXML(Ctx, pSubDesc, this, &m_pDevice))
				return ComposeLoadError(Ctx, strPatternSubst(CONSTLIT("Unable to load %s: %s"), pSubDesc->GetTag(), Ctx.sError));
			}
		else if (strEquals(pSubDesc->GetTag(), REACTOR_CLASS_TAG))
			{
			if (error = CReactorClass::CreateFromXML(Ctx, pSubDesc, this, &m_pDevice))
				return ComposeLoadError(Ctx, strPatternSubst(CONSTLIT("Unable to load %s: %s"), pSubDesc->GetTag(), Ctx.sError));
			}

		//	Missiles

		else if (strEquals(pSubDesc->GetTag(), MISSILE_TAG))
			{
			CWeaponFireDesc::SInitOptions Options;
			Options.sUNID = strPatternSubst(CONSTLIT("%d"), GetUNID());
			Options.iLevel = GetLevel();

			m_pMissile = new CWeaponFireDesc;
			if (error = m_pMissile->InitFromMissileXML(Ctx, pSubDesc, this, Options))
				return ComposeLoadError(Ctx, strPatternSubst(CONSTLIT("Unable to load %s: %s"), pSubDesc->GetTag(), Ctx.sError));
			}

		//	Enhancer

		else if (strEquals(pSubDesc->GetTag(), ENHANCER_ITEM_TAG))
			{
			CItemEnhancement Enhancement;
			if (error = Enhancement.InitFromDesc(Ctx, pSubDesc->GetAttribute(ENHANCEMENT_ATTRIB)))
				return ComposeLoadError(Ctx, strPatternSubst(CONSTLIT("Unable to load %s: %s"), pSubDesc->GetTag(), Ctx.sError));

			m_dwModCode = Enhancement.GetModCode();
			}

		//	Other elements

		else if (IsValidLoadXML(pSubDesc->GetTag()))
			;

		//	Otherwise, error

		else
			kernelDebugLogPattern("%s (%x): Unknown sub-element for ItemType: %s", GetNounPhrase(), GetUNID(), pSubDesc->GetTag());
		}

	//	Initialize role default, if necessary. We only initialize for some 
	//	specialized device types.

	if (m_pDevice && m_sRole.IsBlank())
		{
		switch (m_pDevice->GetCategory())
			{
			case itemcatCargoHold:
				m_sRole = ROLE_CARGO_HOLD;
				break;

			case itemcatDrive:
				m_sRole = ROLE_DRIVE;
				break;

			case itemcatReactor:
				m_sRole = ROLE_REACTOR;
				break;

			case itemcatLauncher:
				m_sRole = ROLE_LAUNCHER;
				break;
			}
		}

	//	Done

	return NOERROR;
	}

CEffectCreator *CItemType::OnFindEffectCreator (const CString &sUNID)

//	OnFindEffectCreator
//
//	Find the effect creator. sUNID is the remainder after the item type UNID has been removed

	{
	if (m_pDevice)
		return m_pDevice->FindEffectCreator(sUNID);
	else if (m_pMissile)
		return m_pMissile->FindEffectCreator(sUNID);
	else
		return NULL;
	}

const CEconomyType &CItemType::OnGetDefaultCurrency (void) const

//	OnGetDefaultCurrency
//
//	Returns default currency.

	{
	const CEconomyType *pCurrency = GetCurrencyType();
	if (pCurrency == NULL)
		return GetUniverse().GetDefaultCurrency();

	return *pCurrency;
	}

ICCItemPtr CItemType::OnGetProperty (CCodeChainCtx &Ctx, const CString &sProperty) const

//	OnGetProperty
//
//	Return property.

	{
	CItem Item(const_cast<CItemType *>(this), 1);
	CItemCtx ItemCtx(Item);

	return ICCItemPtr(Item.GetItemProperty(Ctx, ItemCtx, sProperty, true));
	}

bool CItemType::OnSetTypeProperty (const CString &sProperty, const ICCItem &Value)

//	OnSetTypeProperty
//
//	Sets a property.

	{
	CItem Item(this, 1);
	CItemCtx ItemCtx(Item);

	ESetPropertyResult iResult = Item.SetProperty(ItemCtx, sProperty, &Value, true);
	return (iResult == ESetPropertyResult::set);
	}

bool CItemType::OnHasSpecialAttribute (const CString &sAttrib) const

//	OnHasSpecialAttribute
//
//	Returns TRUE if we have the special attribute

	{
	if (strStartsWith(sAttrib, SPECIAL_DAMAGE_TYPE))
		{
		//	Get the device

		CDeviceClass *pDevice;
        CItem Ammo;
        if (IsMissile())
            {
			pDevice = GetAmmoLauncher();
            Ammo = CItem(const_cast<CItemType *>(this), 1);
            }
		else
			pDevice = GetDeviceClass();

		if (pDevice == NULL)
			return false;

		DamageTypes iType = LoadDamageTypeFromXML(strSubString(sAttrib, SPECIAL_DAMAGE_TYPE.GetLength(), -1));
		if (iType == damageError)
			return false;

		return (iType == pDevice->GetDamageType(CItemCtx(), Ammo));
		}
	else if (strStartsWith(sAttrib, SPECIAL_CAN_BE_DAMAGED))
		{
		bool bValue = strEquals(strSubString(sAttrib, SPECIAL_CAN_BE_DAMAGED.GetLength(), -1), SPECIAL_TRUE);

		CDeviceClass *pDevice;
		if (pDevice = GetDeviceClass())
			return (pDevice->CanBeDamaged() == bValue);
		else if (IsArmor())
			return (true == bValue);
		else
			return (false == bValue);
		}
	else if (strStartsWith(sAttrib, SPECIAL_CAN_BE_DISRUPTED))
		{
		bool bValue = strEquals(strSubString(sAttrib, SPECIAL_CAN_BE_DISRUPTED.GetLength(), -1), SPECIAL_TRUE);

		CDeviceClass *pDevice;
		if (pDevice = GetDeviceClass())
			return (pDevice->CanBeDisrupted() == bValue);
		else if (IsArmor())
			return (true == bValue);
		else
			return (false == bValue);
		}
	else if (strStartsWith(sAttrib, SPECIAL_HAS_COMPONENTS))
		{
		bool bValue = strEquals(strSubString(sAttrib, SPECIAL_HAS_COMPONENTS.GetLength(), -1), SPECIAL_TRUE);
		return ((m_Components.GetCount() > 0) == bValue);
		}
	else if (strStartsWith(sAttrib, SPECIAL_IS_LAUNCHER))
		{
		bool bValue = strEquals(strSubString(sAttrib, SPECIAL_IS_LAUNCHER.GetLength(), -1), SPECIAL_TRUE);
		return ((GetCategory() == itemcatLauncher) == bValue);
		}
	else if (strStartsWith(sAttrib, SPECIAL_LAUNCHED_BY))
		{
		DWORD dwLauncher = strToInt(strSubString(sAttrib, SPECIAL_LAUNCHED_BY.GetLength(), -1), 0);
		if (dwLauncher == 0 || !IsMissile())
			return false;

		CDeviceClass *pDevice = GetUniverse().FindDeviceClass(dwLauncher);
		if (pDevice == NULL)
			return false;

		return (pDevice->GetAmmoVariant(this) != -1);
		}
	else if (strStartsWith(sAttrib, SPECIAL_UNKNOWN_TYPE))
		{
		CString sType = strSubString(sAttrib, SPECIAL_UNKNOWN_TYPE.GetLength());
		DWORD dwType = strToInt(sType, 0);
		return IsUnknownType(dwType);
		}
	else
		return false;
	}

ALERROR CItemType::OnPrepareBindDesign (SDesignLoadCtx &Ctx)

//	OnPrepareBindDesign
//
//	Get ready to bind

	{
	if (m_pDevice)
		if (ALERROR error = m_pDevice->PrepareBind(Ctx))
			return error;

	//	Done

	return NOERROR;
	}

void CItemType::OnReadFromStream (SUniverseLoadCtx &Ctx)

//	OnReadFromStream
//
//	Read from stream
//
//	DWORD		flags
//
//	DWORD		Unknown Types count
//	CString		sUnknownName
//	DWORD		flags

	{
	DWORD dwLoad;
	Ctx.pStream->Read(dwLoad);
	
	bool bKnown =	(((dwLoad & 0x00000001) && Ctx.dwVersion < 36) ? true : false);
	m_fReference =	((dwLoad & 0x00000002) ? true : false);

	if (Ctx.dwVersion >= 36)
		{
		int iCount;
		Ctx.pStream->Read(iCount);

		TSortMap<DWORD, SUnknownTypeDesc> Unknown;
		Unknown.GrowToFit(iCount);

		for (int i = 0; i < iCount; i++)
			{
			DWORD dwUNID;
			Ctx.pStream->Read(dwUNID);

			SUnknownTypeDesc *pEntry = Unknown.SetAt(dwUNID);
			pEntry->sUnknownName.ReadFromStream(Ctx.pStream);

			DWORD dwFlags;
			Ctx.pStream->Read(dwFlags);
			pEntry->bKnown = ((dwFlags & 0x00000001) ? true : false);
			}

		for (int i = 0; i < m_UnknownTypes.GetCount(); i++)
			{
			SUnknownTypeDesc *pSrc = Unknown.GetAt(m_UnknownTypes[i].pUnknownType->GetUNID());
			if (pSrc == NULL)
				{
				//	If a new entry got added in between saves, we mark it to 
				//	known, so we don't have to generate a name.

				m_UnknownTypes[i].bKnown = true;
				continue;
				}

			m_UnknownTypes[i].sUnknownName = pSrc->sUnknownName;
			m_UnknownTypes[i].bKnown = pSrc->bKnown;
			}
		}
	else
		{
		for (int i = 0; i < m_UnknownTypes.GetCount(); i++)
			m_UnknownTypes[i].bKnown = true;

		if (Ctx.dwVersion >= 10)
			{
			CString sUnknownName;
			sUnknownName.ReadFromStream(Ctx.pStream);

			if (m_UnknownTypes.GetCount() > 0)
				{
				m_UnknownTypes[0].sUnknownName = sUnknownName;
				m_UnknownTypes[0].bKnown = bKnown;
				}
			}
		else
			{
			int iUnknownItem;
			Ctx.pStream->Read(iUnknownItem);

			if (iUnknownItem != -1 && m_UnknownTypes.GetCount() > 0)
				{
				m_UnknownTypes[0].sUnknownName = m_UnknownTypes[0].pUnknownType->GenerateUnknownName(iUnknownItem);
				m_UnknownTypes[0].bKnown = bKnown;
				}
			}
		}

	//	Load components

	if (Ctx.dwSystemVersion >= 117)
		{
		m_Components.DeleteAll();
		m_Components.ReadFromStream(SLoadCtx(Ctx));
		}
	}

void CItemType::OnReinit (void)

//	Reinit
//
//	Reinitialize when the game starts again

	{
	g_pFlotsamStationType = NULL;

	SetAllKnown(false);
	m_fReference = m_fDefaultReference;

	//	Initialize some random elements

	InitRandomNames();
	InitComponents();
	}

void CItemType::OnUnbindDesign (void)

//	OnUnbindDesign
//
//	Reset

	{
	//	Delete the weapon relationship until we've bound everything and know
	//	what we've got.

	m_Weapons.DeleteAll();
	}

void CItemType::OnWriteToStream (IWriteStream *pStream)

//	OnWriteToStream
//
//	Writes out the variable portions of the item type
//
//	DWORD		flags
//
//	DWORD		Unknown Types count
//	DWORD		UNID of unknown type
//	CString		sUnknownName
//	DWORD		flags
//
//	CItemList	m_Components

	{
	DWORD dwSave;

	dwSave = 0;
	//	0x00000001 UNUSED
	dwSave |= (m_fReference ?	0x00000002 : 0);
	pStream->Write(dwSave);

	//	Unknown types info

	dwSave = m_UnknownTypes.GetCount();
	pStream->Write(dwSave);
	for (int i = 0; i < m_UnknownTypes.GetCount(); i++)
		{
		pStream->Write(m_UnknownTypes[i].pUnknownType->GetUNID());

		m_UnknownTypes[i].sUnknownName.WriteToStream(pStream);

		dwSave = 0;
		dwSave |= (m_UnknownTypes[i].bKnown ? 0x00000001 : 0);
		pStream->Write(dwSave);
		}

	//	Components

	m_Components.WriteToStream(pStream);
	}

ALERROR CItemType::ParseFate (SDesignLoadCtx &Ctx, const CString &sDesc, ItemFates *retiFate)

//	ParseFate
//
//	Parses a fate string

	{
	int iPos;

	if (sDesc.IsBlank())
		*retiFate = fateNone;

	else if (FATE_TABLE.FindPos(sDesc, &iPos))
		*retiFate = FATE_TABLE[iPos].Value;

	else
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Invalid fate option: %s"), sDesc);
		return ERR_FAIL;
		}

	return NOERROR;
	}

bool CItemType::ParseItemCategory (const CString &sCategory, ItemCategories *retCategory)

//	ParseItemCategory
//
//	Parses an item category

	{
	ItemCategories iCat;

	if (strEquals(sCategory, CATEGORY_AMMO))
		iCat = itemcatMissile;
	else if (strEquals(sCategory, CATEGORY_ARMOR))
		iCat = itemcatArmor;
	else if (strEquals(sCategory, CATEGORY_CARGO_HOLD))
		iCat = itemcatCargoHold;
	else if (strEquals(sCategory, CATEGORY_DRIVE))
		iCat = itemcatDrive;
	else if (strEquals(sCategory, CATEGORY_FUEL))
		iCat = itemcatFuel;
	else if (strEquals(sCategory, CATEGORY_LAUNCHER))
		iCat = itemcatLauncher;
	else if (strEquals(sCategory, CATEGORY_MISC_DEVICE))
		iCat = itemcatMiscDevice;
	else if (strEquals(sCategory, CATEGORY_MISC_ITEM))
		iCat = itemcatMisc;
	else if (strEquals(sCategory, CATEGORY_REACTOR))
		iCat = itemcatReactor;
	else if (strEquals(sCategory, CATEGORY_SHIELDS))
		iCat = itemcatShields;
	else if (strEquals(sCategory, CATEGORY_USABLE_ITEM))
		iCat = itemcatUseful;
	else if (strEquals(sCategory, CATEGORY_WEAPON))
		iCat = itemcatWeapon;
	else
		return false;

	if (retCategory)
		*retCategory = iCat;

	return true;
	}

void CItemType::SetAllKnown (bool bKnown)

//	SetAllKnown
//
//	Sets known state for all unknown types.

	{
	for (int i = 0; i < m_UnknownTypes.GetCount(); i++)
		m_UnknownTypes[i].bKnown = bKnown;
	}

