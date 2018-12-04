//	CItem.cpp
//
//	CItem class

#include "PreComp.h"

#ifdef DEBUG
//#define DEBUG_ITEM_NAME_MATCH
#endif

#define CAN_BE_INSTALLED_EVENT					CONSTLIT("CanBeInstalled")
#define CAN_BE_UNINSTALLED_EVENT				CONSTLIT("CanBeUninstalled")
#define ON_ADDED_AS_ENHANCEMENT_EVENT			CONSTLIT("OnAddedAsEnhancement")
#define ON_DISABLED_EVENT						CONSTLIT("OnDisable")
#define ON_ENABLED_EVENT						CONSTLIT("OnEnable")
#define ON_INSTALL_EVENT						CONSTLIT("OnInstall")
#define ON_OBJ_DESTROYED_EVENT					CONSTLIT("OnObjDestroyed")
#define ON_REACTOR_OVERLOAD_EVENT				CONSTLIT("OnReactorOverload")
#define ON_REMOVED_AS_ENHANCEMENT_EVENT			CONSTLIT("OnRemovedAsEnhancement")
#define ON_UNINSTALL_EVENT						CONSTLIT("OnUninstall")

#define PROPERTY_CAN_BE_USED					CONSTLIT("canBeUsed")
#define PROPERTY_CHARGES						CONSTLIT("charges")
#define PROPERTY_COMPONENTS						CONSTLIT("components")
#define PROPERTY_DAMAGED						CONSTLIT("damaged")
#define PROPERTY_DESCRIPTION					CONSTLIT("description")
#define PROPERTY_DISRUPTED						CONSTLIT("disrupted")
#define PROPERTY_HAS_USE_SCREEN					CONSTLIT("hasUseScreen")
#define PROPERTY_INC_CHARGES					CONSTLIT("incCharges")
#define PROPERTY_INSTALLED						CONSTLIT("installed")
#define PROPERTY_KNOWN							CONSTLIT("known")
#define PROPERTY_LEVEL  						CONSTLIT("level")
#define PROPERTY_MAX_CHARGES  					CONSTLIT("maxCharges")
#define PROPERTY_MAX_LEVEL  					CONSTLIT("maxLevel")
#define PROPERTY_MIN_LEVEL  					CONSTLIT("minLevel")
#define PROPERTY_MASS_BONUS_PER_CHARGE			CONSTLIT("massBonusPerCharge")
#define PROPERTY_REFERENCE						CONSTLIT("reference")
#define PROPERTY_ROOT_NAME						CONSTLIT("rootName")
#define PROPERTY_TRADE_ID						CONSTLIT("tradeID")
#define PROPERTY_VALUE_BONUS_PER_CHARGE			CONSTLIT("valueBonusPerCharge")
#define PROPERTY_USED							CONSTLIT("used")
#define PROPERTY_WEAPON_TYPES					CONSTLIT("weaponTypes")

#define LANGID_CORE_CHARGES						CONSTLIT("core.charges")
#define LANGID_CORE_REFERENCE					CONSTLIT("core.reference")

CItemEnhancement CItem::m_NullMod;
CItem CItem::m_NullItem;

CItem::CItem (void) : 
		m_pItemType(NULL), 
		m_dwCount(0),
		m_dwFlags(0),
		m_dwInstalled(0xff),
		m_pExtra(NULL)

//	CItem constructor

	{
	}

CItem::CItem (const CItem &Copy)

//	CItem constructor

	{
	m_pItemType = Copy.m_pItemType;
	m_dwCount = Copy.m_dwCount;
	m_dwFlags = Copy.m_dwFlags;
	m_dwInstalled = Copy.m_dwInstalled;

	if (Copy.m_pExtra)
		m_pExtra = new SExtra(*Copy.m_pExtra);
	else
		m_pExtra = NULL;
	}

CItem::CItem (CItemType *pItemType, int iCount)

//	CItem constructor

	{
	ASSERT(pItemType);

	m_pItemType = pItemType;
	m_dwCount = (DWORD)iCount;
	m_dwFlags = 0;
	m_dwInstalled = 0xff;
	m_pExtra = NULL;

	int iCharges = pItemType->GetCharges();
	if (iCharges)
		SetCharges(iCharges);
	}

CItem::~CItem (void)

//	CItem destructor

	{
	if (m_pExtra)
		delete m_pExtra;
	}

CItem &CItem::operator= (const CItem &Copy)

//	CItem equals operator

	{
	if (m_pExtra)
		delete m_pExtra;

	m_pItemType = Copy.m_pItemType;
	m_dwCount = Copy.m_dwCount;
	m_dwFlags = Copy.m_dwFlags;
	m_dwInstalled = Copy.m_dwInstalled;

	if (Copy.m_pExtra)
		m_pExtra = new SExtra(*Copy.m_pExtra);
	else
		m_pExtra = NULL;

	return *this;
	}

void CItem::AccumulateCustomAttributes (CItemCtx &ItemCtx, TArray<SDisplayAttribute> *retList, ICCItem *pData) const

//	AccumulateCustomAttributes
//
//	Adds display attributes defined by <GetDisplayAttributes>

	{
	int i;

	//	See if we have <GetDisplayAttributes> event. If not, we're done.

	SEventHandlerDesc Event;
	if (!m_pItemType->FindEventHandlerItemType(CItemType::evtGetDisplayAttributes, &Event))
		return;

	CCodeChainCtx Ctx;

	Ctx.SetItemType(GetType());
	Ctx.DefineContainingType(m_pItemType);
	Ctx.SaveAndDefineSourceVar(ItemCtx.GetSource());
	Ctx.SaveAndDefineItemVar(*this);
	Ctx.SaveAndDefineDataVar(pData);

	ICCItem *pResult = Ctx.Run(Event);
	if (pResult->IsError())
		::kernelDebugLogPattern("[%08x] <GetDisplayAttributes>: %s", GetType()->GetUNID(), pResult->GetStringValue());
	else if (!pResult->IsNil())
		{
		//	We expect a list of attributes.

		for (i = 0; i < pResult->GetCount(); i++)
			{
			ICCItem *pEntry = pResult->GetElement(i);

			SDisplayAttribute Entry;
			if (CDisplayAttributeDefinitions::InitFromCCItem(pEntry, Entry))
				retList->Insert(Entry);
			}
		}

	Ctx.Discard(pResult);
	}

DWORD CItem::AddEnhancement (const CItemEnhancement &Enhancement)

//	AddEnhancement
//
//	Adds the enhancement and returns the ID

	{
	//	Deal with NOP

	if (Enhancement.IsEmpty() && GetMods().IsEmpty())
		return OBJID_NULL;

	//	Allocate extra and assign the enhancement

	Extra();
	m_pExtra->m_Mods = Enhancement;

	//	When removing a mod by setting to modcode 0, clear everything

	if (m_pExtra->m_Mods.IsEmpty())
		m_pExtra->m_Mods.SetID(OBJID_NULL);

	//	Otherwise, if we don't have an ID, set it now

	else if (m_pExtra->m_Mods.GetID() == OBJID_NULL)
		m_pExtra->m_Mods.SetID(g_pUniverse->CreateGlobalID());

	return m_pExtra->m_Mods.GetID();
	}

CString CItem::CalcSortKey (void) const

//	CalcSortKey
//
//	Returns a sort key

	{
	CItemType *pType = GetType();
	if (pType == NULL)
		return NULL_STR;

	//	All installed items first

	CString sInstalled;
	if (IsInstalled())
		sInstalled = CONSTLIT("0");
	else
		sInstalled = CONSTLIT("1");

	//	Next, sort on category

	CString sCat;
	switch (pType->GetCategory())
		{
		case itemcatWeapon:
		case itemcatLauncher:
			sCat = CONSTLIT("0");
			break;

		case itemcatMissile:
			sCat = CONSTLIT("1");
			break;

		case itemcatShields:
			sCat = CONSTLIT("2");
			break;

		case itemcatReactor:
			sCat = CONSTLIT("3");
			break;

		case itemcatDrive:
			sCat = CONSTLIT("4");
			break;

		case itemcatCargoHold:
			sCat = CONSTLIT("5");
			break;

		case itemcatMiscDevice:
			sCat = CONSTLIT("6");
			break;

		case itemcatArmor:
			sCat = CONSTLIT("7");
			break;

		case itemcatFuel:
		case itemcatUseful:
			sCat = CONSTLIT("8");
			break;

		default:
			sCat = CONSTLIT("9");
		}

	//	Next, sort by install location

	if (IsInstalled() && pType->GetArmorClass())
		sCat.Append(strPatternSubst(CONSTLIT("%03d%08x"), GetInstalled(), pType->GetUNID()));
	else
		sCat.Append(CONSTLIT("99900000000"));

	//	Within category, sort by level (highest first)

	sCat.Append(strPatternSubst(CONSTLIT("%02d"), MAX_ITEM_LEVEL - pType->GetApparentLevel(CItemCtx(*this))));

	//	Enhanced items before others

	if (IsEnhanced())
		sCat.Append(CONSTLIT("0"));
	else if (IsDamaged())
		sCat.Append(CONSTLIT("2"));
	else
		sCat.Append(CONSTLIT("1"));

	CString sName = pType->GetSortName();

	//	For ammo charges, sort used magazines first

	CString sCharges;
	if (pType->AreChargesAmmo())
		sCharges = strPatternSubst(CONSTLIT("-%08d"), GetCharges());

	return strPatternSubst(CONSTLIT("%s%s%s%s%d"), sInstalled, sCat, sName, sCharges, ((DWORD)(int)this) % 0x10000);
	}

bool CItem::CanBeUsed (CItemCtx &ItemCtx, CString *retsUseKey) const

//	CanBeUsed
//
//	Return TRUE if the item can be used, and optionally returns the key to 
//	invoke to use it.

	{
	if (m_pItemType == NULL)
		return false;

	//	Get the use descriptor and see if we can use this item.

	CItemType::SUseDesc UseDesc;
	if (!m_pItemType->GetUseDesc(&UseDesc))
		return false;

	if (UseDesc.bOnlyIfInstalled && !IsInstalled())
		return false;

	if (UseDesc.bOnlyIfUninstalled && IsInstalled())
		return false;

	if (UseDesc.bOnlyIfEnabled)
		{
		CInstalledDevice *pDevice = ItemCtx.GetDevice();
		if (pDevice == NULL || !pDevice->IsEnabled())
			return false;
		}

	//	Some options only trigger if item is installed. This allows 
	//	items that can be used BOTH when uninstalled and when complete.
	//	That is, bOnlyIfInstalled and bOnlyIfCompleteArmor are
	//	orthogonal.

	if (IsInstalled())
		{
		if (UseDesc.bOnlyIfCompleteArmor)
			{
			CInstalledArmor *pArmor = ItemCtx.GetArmor();
			if (pArmor == NULL || !pArmor->IsComplete() || !pArmor->IsPrime())
				return false;
			}
		else if (UseDesc.bAsArmorSet)
			{
			CInstalledArmor *pArmor = ItemCtx.GetArmor();
			if (pArmor == NULL || !pArmor->IsPrime())
				return false;
			}
		}

	//	Return the use key, if required

	if (retsUseKey)
		*retsUseKey = UseDesc.sUseKey;

	return true;
	}

CItem CItem::CreateItemByName (const CString &sName, const CItemCriteria &Criteria, bool bActualName)

//	CreateItemByName
//
//	Generates an item structure based on natural language
//	name. If there is ambiguity, a random item is chosen.

	{
	int i;

	TArray<CItemType *> List;

	//	Prepare the input name

	TArray<CString> Input;
	NLSplit(sName, &Input);

	//	If we've got nothing, then nothing matches

	if (Input.GetCount() == 0)
		return CItem();

	//	Loop over all items

	int iBestMatch = -1000;
	int iBestNonCriteriaMatch = -1000;
	for (i = 0; i < g_pUniverse->GetItemTypeCount(); i++)
		{
		CItemType *pType = g_pUniverse->GetItemType(i);
		CItem Item(pType, 1);

		//	Compare names

		TArray<CString> ItemName;
		NLSplit(pType->GetNounPhrase(bActualName ? nounActual : 0), &ItemName);
		int iMatch = NLCompare(Input, ItemName);

		//	If this is not in the criteria, then continue

		if (!Item.MatchesCriteria(Criteria))
			{
			if (iMatch > iBestNonCriteriaMatch)
				iBestNonCriteriaMatch = iMatch;
			continue;
			}

		//	If we have a match, add to the list

		if (iMatch >= iBestMatch)
			{
			if (iMatch > iBestMatch)
				List.DeleteAll();

			List.Insert(pType);
			iBestMatch = iMatch;
			}
		}

#ifdef DEBUG_ITEM_NAME_MATCH
	{
	for (i = 0; i < List.GetCount(); i++)
		{
		CItemType *pItem = (CItemType *)List.GetElement(i);
		char szBuffer[1024];
		wsprintf(szBuffer, "%s\n", pItem->GetNounPhrase().GetASCIIZPointer());
		::OutputDebugString(szBuffer);
		}
	}
#endif

	//	If we don't have a good match and there is a better match not in the criteria, then
	//	we don't match anything

	if (iBestMatch == 0 && List.GetCount() > 1)
		List.DeleteAll();
	else if (iBestNonCriteriaMatch > iBestMatch && List.GetCount() > 1)
		List.DeleteAll();

	//	Pick a random item

	if (List.GetCount())
		{
		CItemType *pBest = List[mathRandom(0, List.GetCount()-1)];
		return CItem(pBest, pBest->GetNumberAppearing().Roll());
		}
	else
		return CItem();
	}

void CItem::Extra (void)

//	Extra
//
//	Make sure that Extra data structure is allocated

	{
	if (m_pExtra == NULL)
		{
		m_pExtra = new SExtra;
		m_pExtra->m_dwCharges = 0;
		m_pExtra->m_dwVariant = 0;
		m_pExtra->m_dwDisruptedTime = 0;
		}
	}

bool CItem::FireCanBeInstalled (CSpaceObject *pSource, int iSlot, CString *retsError) const

//	FireCanBeInstalled
//
//	CanBeInstalled event

	{
	SEventHandlerDesc Event;
	if (m_pItemType->FindEventHandler(CAN_BE_INSTALLED_EVENT, &Event))
		{
		CCodeChainCtx Ctx;

		Ctx.DefineContainingType(m_pItemType);
		Ctx.SaveAndDefineSourceVar(pSource);
		Ctx.SaveAndDefineItemVar(*this);

		if (iSlot == -1 || pSource == NULL)
			{
			Ctx.DefineNil(CONSTLIT("aArmorSeg"));
			Ctx.DefineNil(CONSTLIT("aItemToReplace"));
			}
		else if (IsArmor())
			{
			Ctx.DefineInteger(CONSTLIT("aArmorSeg"), iSlot);
			Ctx.DefineNil(CONSTLIT("aItemToReplace"));
			}
		else
			{
			CItemListManipulator ItemList(pSource->GetItemList());
			if (pSource->SetCursorAtDevice(ItemList, iSlot))
				{
				const CItem &ItemToReplace = ItemList.GetItemAtCursor();
				Ctx.DefineItem(CONSTLIT("aItemToReplace"), ItemToReplace);
				}
			else
				Ctx.DefineNil(CONSTLIT("aItemToReplace"));

			Ctx.DefineNil(CONSTLIT("aArmorSeg"));
			}

		ICCItem *pResult = Ctx.Run(Event);

		bool bCanBeInstalled;
		if (pResult->IsError())
			{
			*retsError = pResult->GetStringValue();
			pSource->ReportEventError(strPatternSubst(CONSTLIT("Item %x CanBeInstalled"), m_pItemType->GetUNID()), pResult);
			bCanBeInstalled = false;
			}
		else if (!pResult->IsTrue())
			{
			*retsError = pResult->GetStringValue();
			bCanBeInstalled = false;
			}
		else
			bCanBeInstalled = true;

		Ctx.Discard(pResult);

		return bCanBeInstalled;
		}
	else
		return true;
	}

bool CItem::FireCanBeUninstalled (CSpaceObject *pSource, CString *retsError) const

//	FireCanBeUninstalled
//
//	CanBeInstalled event

	{
	SEventHandlerDesc Event;
	if (m_pItemType->FindEventHandler(CAN_BE_UNINSTALLED_EVENT, &Event))
		{
		CCodeChainCtx Ctx;

		Ctx.DefineContainingType(m_pItemType);
		Ctx.SaveAndDefineSourceVar(pSource);
		Ctx.SaveAndDefineItemVar(*this);

		ICCItem *pResult = Ctx.Run(Event);

		bool bCanBeUninstalled;
		if (pResult->IsError())
			{
			*retsError = pResult->GetStringValue();
			pSource->ReportEventError(strPatternSubst(CONSTLIT("Item %x CanBeUninstalled"), m_pItemType->GetUNID()), pResult);
			bCanBeUninstalled = false;
			}
		else if (!pResult->IsTrue())
			{
			*retsError = pResult->GetStringValue();
			bCanBeUninstalled = false;
			}
		else
			bCanBeUninstalled = true;

		Ctx.Discard(pResult);

		return bCanBeUninstalled;
		}
	else
		return true;
	}

void CItem::FireCustomEvent (CItemCtx &ItemCtx, const CString &sEvent, ICCItem *pData, ICCItem **retpResult) const

//	FireCustomEvent
//
//	Triggers the given event.

	{
	CCodeChainCtx Ctx;

	SEventHandlerDesc Event;
	if (GetType()->FindEventHandler(sEvent, &Event))
		{
		//	Define some globals

		Ctx.DefineContainingType(m_pItemType);
		Ctx.SaveAndDefineSourceVar(ItemCtx.GetSource());
		Ctx.SaveAndDefineItemVar(*this);
		Ctx.SaveAndDefineDataVar(pData);

		//	Run code

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			{
			if (ItemCtx.GetSource())
				ItemCtx.GetSource()->ReportEventError(strPatternSubst(CONSTLIT("Item %x %s"), GetType()->GetUNID(), sEvent), pResult);
			else
				::kernelDebugLogPattern("[%08x] <%s>: %s", GetType()->GetUNID(), sEvent, pResult->GetStringValue());
			}

		//	Either return the event result or discard it

		if (retpResult)
			*retpResult = pResult;
		else
			Ctx.Discard(pResult);
		}
	else
		{
		if (retpResult)
			*retpResult = Ctx.CreateNil();
		}
	}

void CItem::FireOnAddedAsEnhancement (CSpaceObject *pSource, const CItem &ItemEnhanced, EnhanceItemStatus iStatus) const

//	FireOnAddedAsEnhancement
//
//	OnAddedAsEnhancement event

	{
	SEventHandlerDesc Event;
	if (m_pItemType->FindEventHandler(ON_ADDED_AS_ENHANCEMENT_EVENT, &Event))
		{
		CCodeChainCtx Ctx;

		Ctx.DefineContainingType(m_pItemType);
		Ctx.SaveAndDefineSourceVar(pSource);
		Ctx.SaveAndDefineItemVar(ItemEnhanced);
		Ctx.DefineInteger(CONSTLIT("aResult"), (int)iStatus);

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			pSource->ReportEventError(strPatternSubst(CONSTLIT("Item %x OnAddedAsEnhancement"), m_pItemType->GetUNID()), pResult);
		Ctx.Discard(pResult);
		}
	}

bool CItem::FireOnDestroyCheck (CItemCtx &ItemCtx, DestructionTypes iCause, const CDamageSource &Attacker) const

//	FireOnDestroyCheck
//
//	See if this item can prevent the source from being destroyed. Returns FALSE
//	if we avoid destruction.

	{
	CCodeChainCtx Ctx;

	//	If we have code, call it.

	SEventHandlerDesc Event;
	if (m_pItemType->FindEventHandlerItemType(CItemType::evtOnDestroyCheck, &Event)
			&& !Ctx.InEvent(eventOnDestroyCheck))
		{
		Ctx.SetEvent(eventOnDestroyCheck);
		Ctx.SetItemType(GetType());
		Ctx.DefineContainingType(m_pItemType);
		Ctx.SaveAndDefineSourceVar(ItemCtx.GetSource());
		Ctx.SaveAndDefineItemVar(*this);
		Ctx.DefineSpaceObject(CONSTLIT("aDestroyer"), Attacker.GetObj());
		Ctx.DefineSpaceObject(CONSTLIT("aOrderGiver"), Attacker.GetOrderGiver());
		Ctx.DefineBool(CONSTLIT("aDestroy"), (iCause != enteredStargate && iCause != ascended));
		Ctx.DefineString(CONSTLIT("aDestroyReason"), GetDestructionName(iCause));

		ICCItem *pResult = Ctx.Run(Event);
		bool bResult = (pResult->IsNil() ? false : true);
		Ctx.Discard(pResult);

		return bResult;
		}

	//	Otherwise, destruction succeeds

	return true;
	}

void CItem::FireOnDisabled (CSpaceObject *pSource) const

//	FireOnDisabled
//
//	OnDisabled event

	{
	SEventHandlerDesc Event;
	if (m_pItemType->FindEventHandler(ON_DISABLED_EVENT, &Event))
		{
		CCodeChainCtx Ctx;

		Ctx.DefineContainingType(m_pItemType);
		Ctx.SaveAndDefineSourceVar(pSource);
		Ctx.SaveAndDefineItemVar(*this);

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			pSource->ReportEventError(strPatternSubst(CONSTLIT("Item %x OnDisable"), m_pItemType->GetUNID()), pResult);
		Ctx.Discard(pResult);
		}
	}

void CItem::FireOnDocked (CSpaceObject *pSource, CSpaceObject *pDockedAt) const

//	FireOnDocked
//
//	OnDocked event

	{
	SEventHandlerDesc Event;
	if (m_pItemType->FindEventHandlerItemType(CItemType::evtOnDocked, &Event))
		{
		CCodeChainCtx Ctx;

		Ctx.DefineContainingType(m_pItemType);
		Ctx.SaveAndDefineSourceVar(pSource);
		Ctx.SaveAndDefineItemVar(*this);
		Ctx.DefineSpaceObject(CONSTLIT("aObjDocked"), pSource);
		Ctx.DefineSpaceObject(CONSTLIT("aDockTarget"), pDockedAt);

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			pSource->ReportEventError(strPatternSubst(CONSTLIT("Item %x OnDocked"), m_pItemType->GetUNID()), pResult);
		Ctx.Discard(pResult);
		}
	}

void CItem::FireOnEnabled (CSpaceObject *pSource) const

//	FireOnEnabled
//
//	OnEnabled event

	{
	SEventHandlerDesc Event;
	if (m_pItemType->FindEventHandlerItemType(CItemType::evtOnEnabled, &Event))
		{
		CCodeChainCtx Ctx;

		Ctx.DefineContainingType(m_pItemType);
		Ctx.SaveAndDefineSourceVar(pSource);
		Ctx.SaveAndDefineItemVar(*this);

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			pSource->ReportEventError(strPatternSubst(CONSTLIT("Item %x OnEnable"), m_pItemType->GetUNID()), pResult);
		Ctx.Discard(pResult);
		}
	}

void CItem::FireOnInstall (CSpaceObject *pSource) const

//	FireOnInstall
//
//	OnInstall event

	{
	SEventHandlerDesc Event;
	if (m_pItemType->FindEventHandlerItemType(CItemType::evtOnInstall, &Event))
		{
		CCodeChainCtx Ctx;

		Ctx.DefineContainingType(m_pItemType);
		Ctx.SaveAndDefineSourceVar(pSource);
		Ctx.SaveAndDefineItemVar(*this);

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			pSource->ReportEventError(strPatternSubst(CONSTLIT("Item %x OnInstall"), m_pItemType->GetUNID()), pResult);
		Ctx.Discard(pResult);
		}
	}

void CItem::FireOnObjDestroyed (CSpaceObject *pSource, const SDestroyCtx &Ctx) const

//	FireOnObjDestroyed
//
//	OnObjDestroyed event

	{
	SEventHandlerDesc Event;

	if (m_pItemType->FindEventHandler(CDesignType::evtOnObjDestroyed, &Event))
		{
		CCodeChainCtx CCCtx;

		CCCtx.DefineContainingType(m_pItemType);
		CCCtx.SaveAndDefineSourceVar(pSource);
		CCCtx.SaveAndDefineItemVar(*this);
		CCCtx.DefineSpaceObject(CONSTLIT("aObjDestroyed"), Ctx.pObj);
		CCCtx.DefineSpaceObject(CONSTLIT("aDestroyer"), Ctx.Attacker.GetObj());
		CCCtx.DefineSpaceObject(CONSTLIT("aOrderGiver"), Ctx.GetOrderGiver());
		CCCtx.DefineSpaceObject(CONSTLIT("aWreckObj"), Ctx.pWreck);
		CCCtx.DefineBool(CONSTLIT("aDestroy"), Ctx.WasDestroyed());
		CCCtx.DefineString(CONSTLIT("aDestroyReason"), GetDestructionName(Ctx.iCause));

		ICCItem *pResult = CCCtx.Run(Event);
		if (pResult->IsError())
			pSource->ReportEventError(strPatternSubst(CONSTLIT("Item %x OnObjDestroyed"), m_pItemType->GetUNID()), pResult);
		CCCtx.Discard(pResult);
		}
	}

bool CItem::FireOnReactorOverload (CSpaceObject *pSource) const

//	FireOnReactorOverload
//
//	OnReactorOverload event

	{
	SEventHandlerDesc Event;
	if (m_pItemType->FindEventHandler(ON_REACTOR_OVERLOAD_EVENT, &Event))
		{
		CCodeChainCtx Ctx;
		bool bHandled = false;

		Ctx.DefineContainingType(m_pItemType);
		Ctx.SaveAndDefineSourceVar(pSource);
		Ctx.SaveAndDefineItemVar(*this);

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			pSource->ReportEventError(strPatternSubst(CONSTLIT("Item %x OnReactorOverload"), m_pItemType->GetUNID()), pResult);
		else
			bHandled = !pResult->IsNil();

		Ctx.Discard(pResult);
		return bHandled;
		}
	else
		return false;
	}

void CItem::FireOnRemovedAsEnhancement (CSpaceObject *pSource, const CItem &ItemEnhanced) const

//	FireOnRemovedAsEnhancement
//
//	OnRemovedAsEnhancement event

	{
	SEventHandlerDesc Event;
	if (m_pItemType->FindEventHandler(ON_REMOVED_AS_ENHANCEMENT_EVENT, &Event))
		{
		CCodeChainCtx Ctx;

		Ctx.DefineContainingType(m_pItemType);
		Ctx.SaveAndDefineSourceVar(pSource);
		Ctx.SaveAndDefineItemVar(ItemEnhanced);

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			pSource->ReportEventError(strPatternSubst(CONSTLIT("Item %x OnRemovedAsEnhancement"), m_pItemType->GetUNID()), pResult);
		Ctx.Discard(pResult);
		}
	}

void CItem::FireOnUninstall (CSpaceObject *pSource) const

//	FireOnUninstall
//
//	OnUninstall event

	{
	SEventHandlerDesc Event;
	if (m_pItemType->FindEventHandler(ON_UNINSTALL_EVENT, &Event))
		{
		CCodeChainCtx Ctx;

		Ctx.DefineContainingType(m_pItemType);
		Ctx.SaveAndDefineSourceVar(pSource);
		Ctx.SaveAndDefineItemVar(*this);

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			pSource->ReportEventError(strPatternSubst(CONSTLIT("Item %x OnUninstall"), m_pItemType->GetUNID()), pResult);
		Ctx.Discard(pResult);
		}
	}

const CItemList &CItem::GetComponents (void) const

//	GetComponents
//
//	Returns the list of components which make up this item.

	{
	return m_pItemType->GetComponents();
	}

ICCItemPtr CItem::GetDataAsItem (const CString &sAttrib) const

//	GetDataAsItem
//
//	Returns data

	{
	if (m_pExtra)
		return m_pExtra->m_Data.GetDataAsItem(sAttrib);

	return ICCItemPtr(g_pUniverse->GetCC().CreateNil());
	}

CString CItem::GetDesc (CItemCtx &ItemCtx, bool bActual) const

//	GetDesc
//
//	Gets the item description
	
	{
	if (!m_pItemType->IsKnown() && !bActual)
		return m_pItemType->GetUnknownType()->GetDesc();

	//	If we have code, call it to generate the description

	SEventHandlerDesc Event;
	if (m_pItemType->FindEventHandlerItemType(CItemType::evtGetDescription, &Event))
		{
		CCodeChainCtx Ctx;

		Ctx.SetEvent(eventGetDescription);
		Ctx.SetItemType(GetType());
		Ctx.DefineContainingType(m_pItemType);
		Ctx.SaveAndDefineSourceVar(ItemCtx.GetSource());
		Ctx.SaveAndDefineItemVar(*this);

		ICCItem *pResult = Ctx.Run(Event);
		CString sDesc = pResult->GetStringValue();
		Ctx.Discard(pResult);

		return sDesc;
		}

	//	Otherwise, get it from the item

	else
		return m_pItemType->GetDesc(bActual); 
	}

bool CItem::GetDisplayAttributes (CItemCtx &Ctx, TArray<SDisplayAttribute> *retList, ICCItem *pData, bool bActual) const

//	GetDisplayAttributes
//
//	Returns a list of display attributes.

	{
	//	Empty and check for edge cases

	retList->DeleteAll();
	if (m_pItemType == NULL)
		return false;

	//	Always add level

	retList->Insert(SDisplayAttribute(attribNeutral, strPatternSubst(CONSTLIT("level %d"), m_pItemType->GetApparentLevel(Ctx))));

	//	Add additional custom attributes

	if (m_pItemType->IsKnown() || bActual)
		{
		g_pUniverse->GetAttributeDesc().AccumulateAttributes(*this, retList);

		CArmorClass *pArmor;
		CDeviceClass *pDevice;

		//	Armor attributes

		if (pArmor = m_pItemType->GetArmorClass())
			pArmor->AccumulateAttributes(Ctx, retList);

		//	Device attributes

		else if (pDevice = m_pItemType->GetDeviceClass())
			pDevice->AccumulateAttributes(Ctx, CItem(), retList);

		//	Missile attributes

		else if (m_pItemType->IsMissile() && (pDevice = m_pItemType->GetAmmoLauncher()))
			pDevice->AccumulateAttributes(Ctx, *this, retList);

		//	Useable attribute

		if (m_pItemType->GetUseDesc())
			retList->Insert(SDisplayAttribute(attribNeutral, CONSTLIT("useable")));

		//	Add custom attributes

		AccumulateCustomAttributes(Ctx, retList, pData);

		//	Military and Illegal attributes

		if (m_pItemType->HasLiteralAttribute(CONSTLIT("Military")))
			retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("military")));

		if (m_pItemType->HasLiteralAttribute(CONSTLIT("Illegal")))
			retList->Insert(SDisplayAttribute(attribNegative, CONSTLIT("illegal")));

		//	Add any enhancements

		const CItemEnhancementStack *pEnhancements = Ctx.GetEnhancementStack();
		if (pEnhancements)
			pEnhancements->AccumulateAttributes(Ctx, retList);

		if (IsEnhanced())
			retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("+enhanced"), true));

		//	Charges

		CString sNoun;
		if (m_pItemType->TranslateText(*this, LANGID_CORE_CHARGES, NULL, &sNoun))
			retList->Insert(SDisplayAttribute(attribNeutral, CLanguage::ComposeNounPhrase(sNoun, GetCharges(), NULL_STR, 0, nounCountAlways)));
		}

	//	Add various engine-based attributes (these are shown even if the item 
	//	type is unknown).

	if (IsDamaged())
		retList->Insert(SDisplayAttribute(attribNegative, CONSTLIT("damaged")));

	if (IsDisrupted())
		retList->Insert(SDisplayAttribute(attribNegative, CONSTLIT("ionized")));

	//	Done

	return (retList->GetCount() > 0);
	}

DWORD CItem::GetDisruptedDuration (void) const

//	GetDisruptedDuration
//
//	Returns how long the item will be disrupted for

	{
	if (m_pExtra == NULL || m_pExtra->m_dwDisruptedTime == 0)
		return 0;
	else if (m_pExtra->m_dwDisruptedTime == INFINITE_TICK)
		return INFINITE_TICK;
	else
		{
		DWORD dwNow = (DWORD)g_pUniverse->GetTicks();
		if (m_pExtra->m_dwDisruptedTime <= dwNow)
			return 0;
		else
			return (m_pExtra->m_dwDisruptedTime - dwNow);
		}
	}

bool CItem::GetDisruptedStatus (DWORD *retdwTimeLeft, bool *retbRepairedEvent) const

//	GetDisruptedStatus
//
//	Returns TRUE if the item is disrupted. Optionally returns the number of 
//	ticks left until repair. retbRepairedEvent is set to TRUE if the item was
//	repaired on the previous tick.

	{
	bool bDisrupted = false;
	bool bRepairedEvent = false;
	DWORD dwTimeLeft = 0;

	if (m_pExtra == NULL || m_pExtra->m_dwDisruptedTime == 0)
		;
	else if (m_pExtra->m_dwDisruptedTime == INFINITE_TICK)
		{
		bDisrupted = true;
		dwTimeLeft = INFINITE_TICK;
		}
	else
		{
		DWORD dwNow = (DWORD)g_pUniverse->GetTicks();
		if (m_pExtra->m_dwDisruptedTime < dwNow)
			{
			bRepairedEvent = (m_pExtra->m_dwDisruptedTime + 1 == dwNow);
			}
		else
			{
			bDisrupted = true;
			dwTimeLeft = (m_pExtra->m_dwDisruptedTime - dwNow);
			}
		}

	//	Return

	if (retdwTimeLeft)
		*retdwTimeLeft = dwTimeLeft;

	if (retbRepairedEvent)
		*retbRepairedEvent = bRepairedEvent;

	return bDisrupted;
	}

CString CItem::GetEnhancedDesc (CSpaceObject *pInstalled) const

//	GetEnhancedDesc
//
//	Returns a string that describes the kind of enhancement
//	for this item.

	{
	CItemCtx ItemCtx(this, pInstalled);

	TArray<SDisplayAttribute> Attribs;
	if (!ItemCtx.GetEnhancementDisplayAttributes(&Attribs))
		return NULL_STR;

	CString sResult = Attribs[0].sText;
	for (int i = 1; i < Attribs.GetCount(); i++)
		{
		sResult.Append(CONSTLIT(" "));
		sResult.Append(Attribs[i].sText);
		}

	return sResult;
	}

int CItem::GetMassKg (void) const

//	GetMass
//
//	Returns the mass of a single unit of the item type.

	{
	return m_pItemType->GetMassKg(CItemCtx(*this));
	}

int CItem::GetMaxCharges (void) const

//	GetMaxCharges
//
//	Returns the maximum charges for the item.

	{
	return (m_pItemType ? m_pItemType->GetMaxCharges() : 0);
	}

CString CItem::GetNounPhrase (CItemCtx &Ctx, DWORD dwFlags) const

//	GetNounPhrase
//
//	Returns a noun phrase that represents this item

	{
	CString sName;
	CString sModifier;
	DWORD dwNounFlags = 0;

	//	If we have code, call it to generate the name

	SEventHandlerDesc Event;
	if (m_pItemType->FindEventHandlerItemType(CItemType::evtGetName, &Event)
			&& !(dwFlags & nounNoEvent))
		{
		CCodeChainCtx Ctx;

		Ctx.SetEvent(eventGetName);
		Ctx.SetItemType(GetType());
		Ctx.DefineContainingType(m_pItemType);
		Ctx.SaveAndDefineSourceVar(NULL);
		Ctx.SaveAndDefineItemVar(*this);
		Ctx.DefineVar(CONSTLIT("aFlags"), CLanguage::GetNounFlags(dwFlags));

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			{
			sName = pResult->GetStringValue();
			dwNounFlags = 0;
			}
		else if (pResult->IsList() && pResult->GetCount() >= 2)
			{
			sName = pResult->GetElement(0)->GetStringValue();
			dwNounFlags = pResult->GetElement(1)->GetIntegerValue();
			}
		else
			{
			sName = pResult->GetStringValue();
			dwNounFlags = 0;
			}
		Ctx.Discard(pResult);
		}

	//	Otherwise, get it from the item type

	else
		{
		sName = m_pItemType->GetNamePattern(dwFlags, &dwNounFlags);

		//	Modifiers

		if ((dwFlags & nounInstalledState)
				&& IsInstalled())
			sModifier = CONSTLIT("installed ");

		if (!(dwFlags & nounNoModifiers))
			{
			if (IsDamaged() || (GetMods().IsDisadvantage()))
				sModifier.Append(CONSTLIT("damaged "));
			else if (IsEnhanced() || GetMods().IsEnhancement())
				sModifier.Append(CONSTLIT("enhanced "));
			}

		if (dwFlags & nounDuplicateModifier)
			{
			CInstalledDevice *pDevice;
			if ((pDevice = Ctx.GetDevice())
					&& pDevice->IsDuplicate())
				{
				if (m_pItemType->IsScalable())
					sModifier.Append(strPatternSubst(CONSTLIT("level %d "), GetLevel()));
				}
			}
		}

	//	Compose phrase

	return CLanguage::ComposeNounPhrase(sName, (int)m_dwCount, sModifier, dwNounFlags, dwFlags);
	}

ICCItem *CItem::GetItemProperty (CCodeChainCtx &CCCtx, CItemCtx &Ctx, const CString &sProperty) const

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
	CCodeChain &CC = g_pUniverse->GetCC();
	ICCItem *pResult;
	int i;

	//	First we handle all properties that are specific to the item instance.

	if (strEquals(sProperty, PROPERTY_CAN_BE_USED))
		return CC.CreateBool(CanBeUsed(Ctx));

	else if (strEquals(sProperty, PROPERTY_CHARGES))
		return CC.CreateInteger(GetCharges());

	else if (strEquals(sProperty, PROPERTY_DAMAGED))
		return CC.CreateBool(IsDamaged());

	else if (strEquals(sProperty, PROPERTY_DESCRIPTION))
		{
		if (CCCtx.InEvent(eventGetDescription))
			return CC.CreateString(GetType()->GetDesc());
		else
			return CC.CreateString(GetDesc(Ctx));
		}

	else if (strEquals(sProperty, PROPERTY_DISRUPTED))
		return CC.CreateBool(IsDisrupted());

	else if (strEquals(sProperty, PROPERTY_HAS_USE_SCREEN))
		return CC.CreateBool(HasUseItemScreen());

	else if (strEquals(sProperty, PROPERTY_INSTALLED))
		return CC.CreateBool(IsInstalled());

    else if (strEquals(sProperty, PROPERTY_LEVEL))
        return CC.CreateInteger(GetType()->GetLevel(Ctx));

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

		CLanguage::ParseItemName(GetNounPhrase(Ctx, nounShort | nounNoModifiers), &sRoot, &sModifier);

		if (sModifier.IsBlank())
			return CC.CreateString(sRoot);
		else
			return CC.CreateString(strPatternSubst(CONSTLIT("%s, %s"), sRoot, sModifier));
		}

	else if (strEquals(sProperty, PROPERTY_TRADE_ID))
		{
		TArray<SDisplayAttribute> Attribs;
		if (!GetDisplayAttributes(Ctx, &Attribs, NULL, true))
			return CC.CreateNil();

		for (i = 0; i < Attribs.GetCount(); i++)
			if (!Attribs[i].sID.IsBlank())
				return CC.CreateString(Attribs[i].sID);

		return CC.CreateNil();
		}

	else if (strEquals(sProperty, PROPERTY_USED))
		return CC.CreateBool(IsUsed());

	//	Next we handle all properties for devices, armor, etc. Note that this
	//	includes both installed properties (e.g., armor segment) and static
	//	properties (e.g., armor HP). But it DOES NOT include item type 
	//	properties common to all items (e.g., mass).

	else
		{
		CDeviceClass *pDevice;
		CArmorClass *pArmor;

		//	If this is a device, then pass it on

		if (pDevice = GetType()->GetDeviceClass())
			{
			if (pResult = pDevice->FindItemProperty(Ctx, sProperty))
				return pResult;
			}

		//	If this is armor, then pass it on

		else if (pArmor = GetType()->GetArmorClass())
			{
			if (pResult = pArmor->FindItemProperty(Ctx, sProperty))
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
		return pResult;

	//	Otherwise, we've got nothing

	else
		return CC.CreateNil();
	}

Metric CItem::GetItemPropertyDouble (CCodeChainCtx &CCCtx, CItemCtx &Ctx, const CString &sProperty) const

//	GetItemPropertyDouble
//
//	Returns a double.

	{
	CCodeChain &CC = g_pUniverse->GetCC();
	ICCItem *pResult = GetItemProperty(CCCtx, Ctx, sProperty);
	if (pResult == NULL)
		return 0.0;

	Metric rValue = pResult->GetDoubleValue();
	pResult->Discard(&CC);
	return rValue;
	}

int CItem::GetItemPropertyInteger (CCodeChainCtx &CCCtx, CItemCtx &Ctx, const CString &sProperty) const

//	GetItemPropertyInteger
//
//	Returns an integer property.

	{
	CCodeChain &CC = g_pUniverse->GetCC();
	ICCItem *pResult = GetItemProperty(CCCtx, Ctx, sProperty);
	if (pResult == NULL)
		return 0;

	int iValue = pResult->GetIntegerValue();
	pResult->Discard(&CC);
	return iValue;
	}

CString CItem::GetItemPropertyString (CCodeChainCtx &CCCtx, CItemCtx &Ctx, const CString &sProperty) const

//	GetItemPropertyString
//
//	Returns a string property

	{
	CCodeChain &CC = g_pUniverse->GetCC();
	ICCItem *pResult = GetItemProperty(CCCtx, Ctx, sProperty);
	if (pResult == NULL)
		return 0;

	CString sValue;
	if (pResult->IsNil())
		sValue = NULL_STR;
	else
		sValue = pResult->Print(&g_pUniverse->GetCC(), PRFLAG_NO_QUOTES | PRFLAG_ENCODE_FOR_DISPLAY);

	pResult->Discard(&CC);
	return sValue;
	}

CString CItem::GetReference (CItemCtx &ItemCtx, const CItem &Ammo, DWORD dwFlags) const

//	GetReference
//
//	Returns a string that describes the basic stats for this
//	item

	{
	if (m_pItemType == NULL)
		return NULL_STR;

	//	No reference if unknown

	if (!m_pItemType->IsKnown() && !(dwFlags & CItemType::FLAG_ACTUAL_ITEM))
		return NULL_STR;

	//	If we've got a standard language item, then use that.

	CString sReference;
	if (m_pItemType->TranslateText(*this, LANGID_CORE_REFERENCE, NULL, &sReference))
		{ }

	//	Otherwise, if we have code, call it to generate some reference text

	else
		{
		SEventHandlerDesc Event;
		if (m_pItemType->FindEventHandlerItemType(CItemType::evtGetReferenceText, &Event))
			{
			CCodeChainCtx Ctx;

			Ctx.SetEvent(eventGetReferenceText);
			Ctx.SetItemType(GetType());
			Ctx.DefineContainingType(m_pItemType);
			Ctx.SaveAndDefineSourceVar(ItemCtx.GetSource());
			Ctx.SaveAndDefineItemVar(*this);

			ICCItem *pResult = Ctx.Run(Event);
			sReference = pResult->GetStringValue();
			Ctx.Discard(pResult);
			}
		}

	//	If we got some custom reference, we combine it with the built-in reference.

	if (!sReference.IsBlank())
		{
		CString sAdditionalRef = m_pItemType->GetReference(ItemCtx, Ammo, dwFlags);
		if (!sAdditionalRef.IsBlank())
			return strPatternSubst(CONSTLIT("%s\n%s"), sReference, sAdditionalRef);
		else
			return sReference;
		}

	//	Otherwise, just the built-in reference

	else
		return m_pItemType->GetReference(ItemCtx, Ammo, dwFlags);
	}

bool CItem::GetReferenceDamageAdj (CSpaceObject *pInstalled, DWORD dwFlags, int *retiHP, int *retArray) const

//	GetReferenceDamageAdj
//
//	Sets the damage type array with values for the item. Returns TRUE if any
//	values are filled

	{
	CArmorClass *pArmor;
	CDeviceClass *pDevice;

	//	No reference if unknown

	if (!m_pItemType->IsKnown() && !(dwFlags & CItemType::FLAG_ACTUAL_ITEM))
		return false;

	//	Return armor reference, if this is armor

	if (pArmor = m_pItemType->GetArmorClass())
		return pArmor->GetReferenceDamageAdj(this, pInstalled, retiHP, retArray);

	//	Return device reference, if this is a device

	else if (pDevice = m_pItemType->GetDeviceClass())
		return pDevice->GetReferenceDamageAdj(this, pInstalled, retiHP, retArray);

	//	Otherwise, nothing

	else
		return false;
	}

bool CItem::GetReferenceDamageType (CItemCtx &Ctx, const CItem &Ammo, DWORD dwFlags, DamageTypes *retiDamage, CString *retsReference) const

//	GetReferenceDamageType
//
//	Returns the damage type and text reference

	{
	CDeviceClass *pDevice;

	//	No reference if unknown

	if (!m_pItemType->IsKnown() && !(dwFlags & CItemType::FLAG_ACTUAL_ITEM))
		return false;

	//	Return device reference, if this is a device

	if (pDevice = m_pItemType->GetDeviceClass())
		{
		return pDevice->GetReferenceDamageType(Ctx, 
				Ammo, 
				retiDamage, 
				retsReference);
		}

	//	If a missile, then get the damage from the weapon

	else if (m_pItemType->IsMissile() && (pDevice = m_pItemType->GetAmmoLauncher()))
		{
		return pDevice->GetReferenceDamageType(Ctx, 
				*this, 
				retiDamage, 
				retsReference);
		}

	//	Otherwise, nothing

	else
		return false;
	}

bool CItem::GetReferenceSpeedBonus (CItemCtx &Ctx, DWORD dwFlags, int *retiSpeedBonus) const

//	GetReferenceSpeedBonus
//
//	Returns TRUE we this armor item gives a speed bonus or penalty to the given
//	ship class.
//
//	If the armor is too heavy to be used by the ship class, we return TRUE but
//	retiSpeedBonus is set to 0.

	{
	CArmorClass *pArmor;

	//	If this item is unknown, then we don't include reference

	if (!m_pItemType->IsKnown() && !(dwFlags & CItemType::FLAG_ACTUAL_ITEM))
		return false;

	//	If this is armor, ask the armor

	else if (pArmor = m_pItemType->GetArmorClass())
		return pArmor->GetReferenceSpeedBonus(Ctx, retiSpeedBonus);

	//	Otherwise, nothing

	else
		return false;
	}

int CItem::GetTradePrice (CSpaceObject *pObj, bool bActual) const

//	GetTradePrice
//
//	Returns the price that pObj would pay to buy the item
//	(or the price that pObj would accept to sell the item)
//
//	Normally this is identical to GetValue(), but this call allows
//	items to compute a different value based on other factors.

	{
	SEventHandlerDesc Event;
	if (m_pItemType->FindEventHandlerItemType(CItemType::evtGetTradePrice, &Event))
		{
		CCodeChainCtx Ctx;

		Ctx.SetEvent(eventGetTradePrice);
		Ctx.SetItemType(GetType());
		Ctx.DefineContainingType(m_pItemType);
		Ctx.SaveAndDefineSourceVar(pObj);
		Ctx.SaveAndDefineItemVar(*this);
		Ctx.DefineString(CONSTLIT("aPriceType"), (bActual ? CONSTLIT("actual") : CONSTLIT("normal")));

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError() && pObj)
			pObj->ReportEventError(strPatternSubst(CONSTLIT("Item %x GetTradePrice"), m_pItemType->GetUNID()), pResult);
		int iPrice = pResult->GetIntegerValue();
		Ctx.Discard(pResult);

		return iPrice;
		}
	else
		return GetValue(bActual);
	}

int CItem::GetValue (bool bActual) const

//	GetValue
//
//	Returns the value of a single item in this group. Caller must multiply by
//	GetCount() to find the value of the whole item group. The value takes into
//	account damage, enhancements, and charges.

	{
	CItemCtx Ctx(*this);

	//	Compute value (this includes charges)

	int iValue = m_pItemType->GetValue(Ctx, bActual);

	//	Adjust value based on enhancements

	int iAdj;

	//	Deal with enhancements

	if (GetMods().IsNotEmpty())
		iAdj = GetMods().GetValueAdj(*this);

	//	Otherwise, generic enhancement

	else
		iAdj = (IsEnhanced() ? 75 : 0);

	//	Increase value

	iValue += iValue * iAdj / 100;

	//	If the item is damaged, value is only 20% of full value

	if (IsDamaged())
		iValue = iValue * 20 / 100;

	return iValue;
	}

bool CItem::HasComponents (void) const

//	HasComponents
//
//	Returns TRUE if we have components.

	{
	return (m_pItemType->GetComponents().GetCount() > 0);
	}

bool CItem::HasSpecialAttribute (const CString &sAttrib) const

//	HasSpecialAttribute
//
//	Returns TRUE if we have the given special attribute

	{
	return m_pItemType->HasSpecialAttribute(sAttrib);
	}

bool CItem::HasUseItemScreen (void) const

//	HasUseItemScreen
//
//	Returns TRUE if this item brings up a screen when used.

	{
	CItemType::SUseDesc Desc;
	if (m_pItemType == NULL)
		return false;

	return (m_pItemType->GetUseDesc(&Desc) && !Desc.bUsableInCockpit && Desc.pScreenRoot != NULL);
	}

bool CItem::IsDisruptionEqual (DWORD dwD1, DWORD dwD2)

//	IsDisruptionEqual
//
//	Returns TRUE if the two disruption time values are effectively equal.

	{
	if (dwD1 == dwD2)
		return true;

	DWORD dwNow = (DWORD)g_pUniverse->GetTicks();
	return (dwD1 < dwNow && dwD2 < dwNow);
	}

bool CItem::IsEqual (const CItem &Item, DWORD dwFlags) const

//	IsEqual
//
//	Returns TRUE if this item is the same as the given
//	item except for the count

	{
	const bool bIgnoreInstalled = (dwFlags & FLAG_IGNORE_INSTALLED ? true : false);
	const bool bIgnoreEnhancements = (dwFlags & FLAG_IGNORE_ENHANCEMENTS ? true : false);

	return (m_pItemType == Item.m_pItemType
			&& ((bIgnoreEnhancements && (m_dwFlags & ~flagEnhanced) == (Item.m_dwFlags & ~flagEnhanced)) || m_dwFlags == Item.m_dwFlags)
			&& (bIgnoreInstalled || m_dwInstalled == Item.m_dwInstalled)
			&& IsExtraEqual(Item.m_pExtra, dwFlags));
	}

bool CItem::IsExtraEmpty (const SExtra *pExtra, DWORD dwFlags)

//	IsExtraEmpty
//
//	Returns TRUE is the given pExtra struct is empty
//	(set to initial values)

	{
	const bool bIgnoreCharges = (dwFlags & FLAG_IGNORE_CHARGES ? true : false);
	const bool bIgnoreData = (dwFlags & FLAG_IGNORE_DATA ? true : false);
	const bool bIgnoreDisrupted = (dwFlags & FLAG_IGNORE_DISRUPTED ? true : false);
	const bool bIgnoreEnhancements = (dwFlags & FLAG_IGNORE_ENHANCEMENTS ? true : false);

	return ((bIgnoreCharges || pExtra->m_dwCharges == 0)
			&& pExtra->m_dwVariant == 0
			&& (bIgnoreDisrupted || (pExtra->m_dwDisruptedTime == 0 || pExtra->m_dwDisruptedTime < (DWORD)g_pUniverse->GetTicks()))
			&& (bIgnoreEnhancements || pExtra->m_Mods.IsEmpty())
			&& (bIgnoreData || pExtra->m_Data.IsEmpty()));
	}

bool CItem::IsExtraEqual (SExtra *pSrc, DWORD dwFlags) const

//	IsExtraEqual
//
//	Returns TRUE if this item's Extra struct is the same as the source

	{
	//	Both have extra struct

	if (m_pExtra && pSrc)
		{
		const bool bIgnoreCharges = (dwFlags & FLAG_IGNORE_CHARGES ? true : false);
		const bool bIgnoreData = (dwFlags & FLAG_IGNORE_DATA ? true : false);
		const bool bIgnoreDisrupted = (dwFlags & FLAG_IGNORE_DISRUPTED ? true : false);
		const bool bIgnoreEnhancements = (dwFlags & FLAG_IGNORE_ENHANCEMENTS ? true : false);

		return ((bIgnoreCharges || m_pExtra->m_dwCharges == pSrc->m_dwCharges)
				&& m_pExtra->m_dwVariant == pSrc->m_dwVariant
				&& (bIgnoreDisrupted || IsDisruptionEqual(m_pExtra->m_dwDisruptedTime, pSrc->m_dwDisruptedTime))
				&& (bIgnoreEnhancements || m_pExtra->m_Mods.IsEqual(pSrc->m_Mods))
				&& (bIgnoreData || m_pExtra->m_Data.IsEqual(pSrc->m_Data)));
		}

	//	Neither has extra struct

	else if (m_pExtra == pSrc)
		return true;

	//	One has extra struct, but it's empty

	else if (m_pExtra == NULL)
		return IsExtraEmpty(pSrc, dwFlags);
	else if (pSrc == NULL)
		return IsExtraEmpty(m_pExtra, dwFlags);

	//	Extra structs don't match at all

	else
		return false;
	}

bool CItem::IsUsed (void) const

//	IsUsed
//
//	Returns TRUE if the item has been used.

	{
	//	For now, the only case that we handle is when charges have been used.

	if (GetCharges() < m_pItemType->GetMaxInitialCharges())
		return true;

	//	Not used

	return false;
	}

bool CItem::IsVirtual (void) const

//	IsVirtual
//
//	Returns TRUE if item is virtual

	{
	return m_pItemType->IsVirtual();
	}

bool CItem::MatchesCriteria (const CItemCriteria &Criteria) const

//	MatchesCriteria
//
//	Return TRUE if this item matches the given criteria

	{
	int i;
    CItemCtx ItemCtx(*this);

	if (m_pItemType == NULL)
		return false;

	//	If we've got a filter, then use that

	if (Criteria.pFilter)
		{
		CCodeChain &CC = g_pUniverse->GetCC();

		//	Create a list representing this item

		ICCItem *pItem = ::CreateListFromItem(CC, *this);

		//	Create an argument list consisting of the item

		ICCItem *pArgs = CC.CreateLinkedList();
		if (pArgs->IsError())
			{
			ASSERT(false);
			return false;
			}

		CCLinkedList *pList = (CCLinkedList *)pArgs;
		pList->Append(CC, pItem);

		//	Apply the function to the arg list

		ICCItem *pResult = CC.Apply(Criteria.pFilter, pArgs, &g_pUniverse);
		bool bMatches = !pResult->IsNil();

		//	Done

		pResult->Discard(&CC);
		pArgs->Discard(&CC);
		pItem->Discard(&CC);

		return bMatches;
		}

	//	If we've got a lookup, then recurse

	else if (!Criteria.sLookup.IsBlank())
		{
		const CItemCriteria *pCriteria = g_pUniverse->GetDesignCollection().GetDisplayAttributes().FindCriteriaByID(Criteria.sLookup);
		if (pCriteria == NULL)
			return false;

		return MatchesCriteria(*pCriteria);
		}

	//	If we're looking for anything, then continue

	if (Criteria.dwItemCategories == 0xFFFFFFFF)
		NULL;

	//	If we're looking for fuel and this item is fuel, then
	//	we continue

	else if ((Criteria.dwItemCategories & itemcatFuel)
			&& m_pItemType->IsFuel())
		NULL;

	//	If we're looking for missiles and this item is a
	//	missile, then we continue.

	else if ((Criteria.dwItemCategories & itemcatMissile)
			&& m_pItemType->IsMissile())
		NULL;

	//	If we're looking for usable items and this item is
	//	isable, then we continue

	else if ((Criteria.dwItemCategories & itemcatUseful)
			&& m_pItemType->IsUsable())
		NULL;

	//	Otherwise, if this is not one of the required categories, bail out

	else if (!(Criteria.dwItemCategories & m_pItemType->GetCategory()))
		return false;

	//	Deal with exclusion

	if (Criteria.dwExcludeCategories == 0)
		NULL;
	else if ((Criteria.dwExcludeCategories & itemcatFuel) && m_pItemType->IsFuel())
		return false;
	else if ((Criteria.dwExcludeCategories & itemcatMissile) && m_pItemType->IsMissile())
		return false;
	else if ((Criteria.dwExcludeCategories & itemcatUseful) && m_pItemType->IsUsable())
		return false;
	else if (Criteria.dwExcludeCategories & m_pItemType->GetCategory())
		return false;

	//	Deal with must have

	if (Criteria.dwMustHaveCategories != 0)
		{
		if ((Criteria.dwMustHaveCategories & itemcatFuel) && !m_pItemType->IsFuel())
			return false;
		if ((Criteria.dwMustHaveCategories & itemcatMissile) && !m_pItemType->IsMissile())
			return false;
		if ((Criteria.dwMustHaveCategories & itemcatUseful) && !m_pItemType->IsUsable())
			return false;

		if ((Criteria.dwMustHaveCategories & itemcatDeviceMask) == itemcatDeviceMask)
			{
			if (!(m_pItemType->GetCategory() & itemcatDeviceMask))
				return false;
			}
		else if ((Criteria.dwMustHaveCategories & itemcatWeaponMask) == itemcatWeaponMask)
			{
			if (!(m_pItemType->GetCategory() & itemcatWeaponMask))
				return false;
			}
		else
			{
			DWORD dwCat = 1;
			for (i = 0; i < itemcatCount; i++)
				{
				if (dwCat != itemcatFuel && dwCat != itemcatMissile && dwCat != itemcatUseful
						&& (Criteria.dwMustHaveCategories & dwCat)
						&& m_pItemType->GetCategory() != dwCat)
					return false;
				
				dwCat = dwCat << 1;
				}
			}
		}

	//	If any of the flags that must be set is not set, then
	//	we do not match.

	if ((Criteria.wFlagsMustBeSet & m_dwFlags) != Criteria.wFlagsMustBeSet)
		return false;

	//	If any of the flags that must be cleared is set, then
	//	we do not match.

	if ((Criteria.wFlagsMustBeCleared & m_dwFlags) != 0)
		return false;

	//	Check installed flags

	if (Criteria.bInstalledOnly && !IsInstalled())
		return false;

	if (Criteria.bNotInstalledOnly && IsInstalled())
		return false;

	//	Check miscellaneous flags

	CItemType::SUseDesc UseDesc;
	if (Criteria.bUsableItemsOnly && (!m_pItemType->GetUseDesc(&UseDesc) || UseDesc.pScreenRoot == NULL))
		return false;

	if (Criteria.bExcludeVirtual && m_pItemType->IsVirtual())
		return false;

	if (Criteria.bLauncherMissileOnly && m_pItemType->IsAmmunition())
		return false;

	//	Check required modifiers

	for (i = 0; i < Criteria.ModifiersRequired.GetCount(); i++)
		if (!m_pItemType->HasLiteralAttribute(Criteria.ModifiersRequired[i]))
			return false;

	//	Check modifiers not allowed

	for (i = 0; i < Criteria.ModifiersNotAllowed.GetCount(); i++)
		if (m_pItemType->HasLiteralAttribute(Criteria.ModifiersNotAllowed[i]))
			return false;

	//	Check required special attributes

	for (i = 0; i < Criteria.SpecialAttribRequired.GetCount(); i++)
		if (!HasSpecialAttribute(Criteria.SpecialAttribRequired[i]))
			return false;

	//	Check special attributes not allowed

	for (i = 0; i < Criteria.SpecialAttribNotAllowed.GetCount(); i++)
		if (HasSpecialAttribute(Criteria.SpecialAttribNotAllowed[i]))
			return false;

	//	Check frequency range

	if (!Criteria.Frequency.IsBlank())
		{
		int iFreq = m_pItemType->GetFrequency();
		char *pPos = Criteria.Frequency.GetASCIIZPointer();
		bool bMatch = false;
		while (*pPos != '\0' && !bMatch)
			{
			switch (*pPos)
				{
				case 'c':
				case 'C':
					if (iFreq == ftCommon)
						bMatch = true;
					break;

				case 'u':
				case 'U':
					if (iFreq == ftUncommon)
						bMatch = true;
					break;

				case 'r':
				case 'R':
					if (iFreq == ftRare)
						bMatch = true;
					break;

				case 'v':
				case 'V':
					if (iFreq == ftVeryRare)
						bMatch = true;
					break;

				case '-':
				case 'n':
				case 'N':
					if (iFreq == ftNotRandom)
						bMatch = true;
					break;
				}

			pPos++;
			}

		if (!bMatch)
			return false;
		}

	//	Check for level modifiers

	if (Criteria.iEqualToLevel != -1 && m_pItemType->GetLevel(ItemCtx) != Criteria.iEqualToLevel)
		return false;

	if (Criteria.iGreaterThanLevel != -1 && m_pItemType->GetLevel(ItemCtx) <= Criteria.iGreaterThanLevel)
		return false;

	if (Criteria.iLessThanLevel != -1 && m_pItemType->GetLevel(ItemCtx) >= Criteria.iLessThanLevel)
		return false;

	//	Check for price modifiers

	if (Criteria.iEqualToPrice != -1 && GetValue() != Criteria.iEqualToPrice)
		return false;

	if (Criteria.iGreaterThanPrice != -1 && GetValue() <= Criteria.iGreaterThanPrice)
		return false;

	if (Criteria.iLessThanPrice != -1 && GetValue() >= Criteria.iLessThanPrice)
		return false;

	//	Check for mass modifiers

	if (Criteria.iEqualToMass != -1 && GetMassKg() != Criteria.iEqualToMass)
		return false;

	if (Criteria.iGreaterThanMass != -1 && GetMassKg() <= Criteria.iGreaterThanMass)
		return false;

	if (Criteria.iLessThanMass != -1 && GetMassKg() >= Criteria.iLessThanMass)
		return false;

	//	Otherwise, we match completely

	return true;
	}

void CItem::InitCriteriaAll (CItemCriteria *retCriteria)

//	InitCriteriaAll
//
//	Initializes criteria set to all

	{
	retCriteria->dwItemCategories = 0xffffffff;
	retCriteria->dwExcludeCategories = 0;
	retCriteria->dwMustHaveCategories = 0;
	retCriteria->wFlagsMustBeSet = 0;
	retCriteria->wFlagsMustBeCleared = 0;

	retCriteria->bUsableItemsOnly = false;
	retCriteria->bExcludeVirtual = true;
	retCriteria->bInstalledOnly = false;
	retCriteria->bNotInstalledOnly = false;
	retCriteria->bLauncherMissileOnly = false;
	retCriteria->ModifiersRequired.DeleteAll();
	retCriteria->ModifiersNotAllowed.DeleteAll();
	retCriteria->SpecialAttribRequired.DeleteAll();
	retCriteria->SpecialAttribNotAllowed.DeleteAll();
	retCriteria->Frequency = NULL_STR;

	retCriteria->iEqualToLevel = -1;
	retCriteria->iGreaterThanLevel = -1;
	retCriteria->iLessThanLevel = -1;

	retCriteria->iEqualToPrice = -1;
	retCriteria->iGreaterThanPrice = -1;
	retCriteria->iLessThanPrice = -1;

	retCriteria->iEqualToMass = -1;
	retCriteria->iGreaterThanMass = -1;
	retCriteria->iLessThanMass = -1;

	retCriteria->sLookup = NULL_STR;
	retCriteria->pFilter = NULL;
	}

void WriteCategoryFlags (CMemoryWriteStream &Output, DWORD dwCategories)
	{
	if (dwCategories & itemcatArmor)
		Output.Write("a", 1);
	if (dwCategories & itemcatMiscDevice)
		Output.Write("b", 1);
	if (dwCategories & itemcatCargoHold)
		Output.Write("c", 1);
	if (dwCategories & itemcatFuel)
		Output.Write("f", 1);
	if (dwCategories & itemcatLauncher)
		Output.Write("l", 1);
	if (dwCategories & itemcatMissile)
		Output.Write("m", 1);
	if (dwCategories & itemcatWeapon)
		Output.Write("p", 1);
	if (dwCategories & itemcatReactor)
		Output.Write("r", 1);
	if (dwCategories & itemcatShields)
		Output.Write("s", 1);
	if (dwCategories & itemcatMisc)
		Output.Write("t", 1);
	if (dwCategories & itemcatUseful)
		Output.Write("u", 1);
	if (dwCategories & itemcatDrive)
		Output.Write("v", 1);
	}

CString CItem::GenerateCriteria (const CItemCriteria &Criteria)

//	GenerateCriteria
//
//	Returns a criteria string from a criteria structure

	{
	int i;
	CMemoryWriteStream Output(64 * 1024);
	CString sTerm;

	if (!Criteria.sLookup.IsBlank())
		return strPatternSubst(CONSTLIT("{%s}"), Criteria.sLookup);
	
	if (Output.Create() != NOERROR)
		return NULL_STR;

	if (Criteria.dwItemCategories == 0xFFFFFFFF)
		Output.Write("*", 1);
	else
		WriteCategoryFlags(Output, Criteria.dwItemCategories);

	if (Criteria.dwExcludeCategories != 0)
		{
		Output.Write("~", 1);
		WriteCategoryFlags(Output, Criteria.dwExcludeCategories);
		}

	if (Criteria.dwMustHaveCategories != 0)
		{
		Output.Write("&", 1);
		WriteCategoryFlags(Output, Criteria.dwMustHaveCategories);
		}

	if (Criteria.wFlagsMustBeSet & flagDamaged)
		Output.Write("D", 1);

	if (Criteria.wFlagsMustBeCleared & flagDamaged)
		Output.Write("N", 1);

	if (Criteria.bUsableItemsOnly)
		Output.Write("S", 1);

	if (Criteria.bInstalledOnly)
		Output.Write("I", 1);

	if (Criteria.bNotInstalledOnly)
		Output.Write("U", 1);

	if (Criteria.bLauncherMissileOnly)
		Output.Write("M", 1);

	if (!Criteria.bExcludeVirtual)
		Output.Write("V", 1);

	for (i = 0; i < Criteria.ModifiersRequired.GetCount(); i++)
		{
		sTerm = strPatternSubst(CONSTLIT(" +%s;"), Criteria.ModifiersRequired[i]);
		Output.Write(sTerm.GetPointer(), sTerm.GetLength());
		}

	for (i = 0; i < Criteria.ModifiersNotAllowed.GetCount(); i++)
		{
		sTerm = strPatternSubst(CONSTLIT(" -%s;"), Criteria.ModifiersNotAllowed[i]);
		Output.Write(sTerm.GetPointer(), sTerm.GetLength());
		}

	for (i = 0; i < Criteria.SpecialAttribRequired.GetCount(); i++)
		{
		sTerm = strPatternSubst(CONSTLIT(" +%s;"), Criteria.SpecialAttribRequired[i]);
		Output.Write(sTerm.GetPointer(), sTerm.GetLength());
		}

	for (i = 0; i < Criteria.SpecialAttribNotAllowed.GetCount(); i++)
		{
		sTerm = strPatternSubst(CONSTLIT(" +%s;"), Criteria.SpecialAttribNotAllowed[i]);
		Output.Write(sTerm.GetPointer(), sTerm.GetLength());
		}

	if (!Criteria.Frequency.IsBlank())
		{
		sTerm = strPatternSubst(CONSTLIT(" F:%s;"), Criteria.Frequency);
		Output.Write(sTerm.GetPointer(), sTerm.GetLength());
		}

	sTerm = NULL_STR;
	if (Criteria.iEqualToLevel != -1)
		sTerm = strPatternSubst(CONSTLIT(" =%d"), Criteria.iEqualToLevel);
	else if (Criteria.iGreaterThanLevel != -1 && Criteria.iLessThanLevel != -1)
		sTerm = strPatternSubst(CONSTLIT(" L:%d-%d;"), Criteria.iGreaterThanLevel + 1, Criteria.iLessThanLevel - 1);
	else
		{
		if (Criteria.iGreaterThanLevel != -1)
			sTerm = strPatternSubst(CONSTLIT(" >%d"), Criteria.iGreaterThanLevel);
		if (Criteria.iLessThanLevel != -1)
			sTerm = strPatternSubst(CONSTLIT(" <%d"), Criteria.iLessThanLevel);
		}

	if (!sTerm.IsBlank())
		Output.Write(sTerm.GetPointer(), sTerm.GetLength());

	sTerm = NULL_STR;
	if (Criteria.iEqualToPrice != -1)
		sTerm = strPatternSubst(CONSTLIT(" =$%d"), Criteria.iEqualToPrice);
	else
		{
		if (Criteria.iGreaterThanPrice != -1)
			sTerm = strPatternSubst(CONSTLIT(" >=$%d"), Criteria.iGreaterThanPrice + 1);
		if (Criteria.iLessThanPrice != -1)
			sTerm = strPatternSubst(CONSTLIT(" <=$%d"), Criteria.iLessThanPrice - 1);
		}

	if (!sTerm.IsBlank())
		Output.Write(sTerm.GetPointer(), sTerm.GetLength());

	sTerm = NULL_STR;
	if (Criteria.iEqualToMass != -1)
		sTerm = strPatternSubst(CONSTLIT(" =#%d"), Criteria.iEqualToMass);
	else
		{
		if (Criteria.iGreaterThanMass != -1)
			sTerm = strPatternSubst(CONSTLIT(" >=#%d"), Criteria.iGreaterThanMass + 1);
		if (Criteria.iLessThanMass != -1)
			sTerm = strPatternSubst(CONSTLIT(" <=#%d"), Criteria.iLessThanMass - 1);
		}

	if (!sTerm.IsBlank())
		Output.Write(sTerm.GetPointer(), sTerm.GetLength());

	//	Done

	return CString(Output.GetPointer(), Output.GetLength());
	}

void CItem::ParseCriteria (const CString &sCriteria, CItemCriteria *retCriteria)

//	ParseCriteria
//
//	Parses a criteria string and initializes the criteria structure

	{
	//	Initialize

	retCriteria->dwItemCategories = 0;
	retCriteria->dwExcludeCategories = 0;
	retCriteria->dwMustHaveCategories = 0;
	retCriteria->wFlagsMustBeSet = 0;
	retCriteria->wFlagsMustBeCleared = 0;

	retCriteria->bUsableItemsOnly = false;
	retCriteria->bExcludeVirtual = true;
	retCriteria->bInstalledOnly = false;
	retCriteria->bNotInstalledOnly = false;
	retCriteria->bLauncherMissileOnly = false;
	retCriteria->ModifiersRequired.DeleteAll();
	retCriteria->ModifiersNotAllowed.DeleteAll();
	retCriteria->SpecialAttribRequired.DeleteAll();
	retCriteria->SpecialAttribNotAllowed.DeleteAll();
	retCriteria->Frequency = NULL_STR;

	retCriteria->iEqualToLevel = -1;
	retCriteria->iGreaterThanLevel = -1;
	retCriteria->iLessThanLevel = -1;

	retCriteria->iEqualToPrice = -1;
	retCriteria->iGreaterThanPrice = -1;
	retCriteria->iLessThanPrice = -1;

	retCriteria->iEqualToMass = -1;
	retCriteria->iGreaterThanMass = -1;
	retCriteria->iLessThanMass = -1;

	retCriteria->sLookup = NULL_STR;
	retCriteria->pFilter = NULL;

	bool bExclude = false;
	bool bMustHave = false;

	//	Parse string

	char *pPos = sCriteria.GetASCIIZPointer();

	//	If we start with a brace, then this is a lookup.

	if (*pPos == '{')
		{
		pPos++;

		char *pStart = pPos;
		while (*pPos != '\0' && *pPos != '}' )
			pPos++;

		retCriteria->sLookup = CString(pStart, (int)(pPos - pStart));

		//	No other directives are allowed

		return;
		}

	//	Otherwise, parse a criteria.

	while (*pPos != '\0')
		{
		switch (*pPos)
			{
			case '*':
				retCriteria->dwItemCategories = 0xFFFFFFFF;
				break;

			case 'a':
				if (bExclude)
					retCriteria->dwExcludeCategories |= itemcatArmor;
				else if (bMustHave)
					retCriteria->dwMustHaveCategories |= itemcatArmor;
				else
					retCriteria->dwItemCategories |= itemcatArmor;
				break;

			case 'b':
				if (bExclude)
					retCriteria->dwExcludeCategories |= itemcatMiscDevice;
				else if (bMustHave)
					retCriteria->dwMustHaveCategories |= itemcatMiscDevice;
				else
					retCriteria->dwItemCategories |= itemcatMiscDevice;
				break;

			case 'c':
				if (bExclude)
					retCriteria->dwExcludeCategories |= itemcatCargoHold;
				else if (bMustHave)
					retCriteria->dwMustHaveCategories |= itemcatCargoHold;
				else
					retCriteria->dwItemCategories |= itemcatCargoHold;
				break;

			case 'd':
				if (bExclude)
					retCriteria->dwExcludeCategories |= itemcatMiscDevice
							| itemcatWeapon
							| itemcatLauncher
							| itemcatReactor
							| itemcatShields
							| itemcatCargoHold
							| itemcatDrive;
				else if (bMustHave)
					retCriteria->dwMustHaveCategories |= itemcatMiscDevice
							| itemcatWeapon
							| itemcatLauncher
							| itemcatReactor
							| itemcatShields
							| itemcatCargoHold
							| itemcatDrive;
				else
					retCriteria->dwItemCategories |= itemcatMiscDevice
							| itemcatWeapon
							| itemcatLauncher
							| itemcatReactor
							| itemcatShields
							| itemcatCargoHold
							| itemcatDrive;
				break;

			case 'f':
				if (bExclude)
					retCriteria->dwExcludeCategories |= itemcatFuel;
				else if (bMustHave)
					retCriteria->dwMustHaveCategories |= itemcatFuel;
				else
					retCriteria->dwItemCategories |= itemcatFuel;
				break;

			case 'l':
				if (bExclude)
					retCriteria->dwExcludeCategories |= itemcatLauncher;
				else if (bMustHave)
					retCriteria->dwMustHaveCategories |= itemcatLauncher;
				else
					retCriteria->dwItemCategories |= itemcatLauncher;
				break;

			case 'm':
				if (bExclude)
					retCriteria->dwExcludeCategories |= itemcatMissile;
				else if (bMustHave)
					retCriteria->dwMustHaveCategories |= itemcatMissile;
				else
					retCriteria->dwItemCategories |= itemcatMissile;
				break;

			case 'p':
				if (bExclude)
					retCriteria->dwExcludeCategories |= itemcatWeapon;
				else if (bMustHave)
					retCriteria->dwMustHaveCategories |= itemcatWeapon;
				else
					retCriteria->dwItemCategories |= itemcatWeapon;
				break;

			case 'r':
				if (bExclude)
					retCriteria->dwExcludeCategories |= itemcatReactor;
				else if (bMustHave)
					retCriteria->dwMustHaveCategories |= itemcatReactor;
				else
					retCriteria->dwItemCategories |= itemcatReactor;
				break;

			case 's':
				if (bExclude)
					retCriteria->dwExcludeCategories |= itemcatShields;
				else if (bMustHave)
					retCriteria->dwMustHaveCategories |= itemcatShields;
				else
					retCriteria->dwItemCategories |= itemcatShields;
				break;

			case 't':
				if (bExclude)
					retCriteria->dwExcludeCategories |= itemcatMisc;
				else if (bMustHave)
					retCriteria->dwMustHaveCategories |= itemcatMisc;
				else
					retCriteria->dwItemCategories |= itemcatMisc;
				break;

			case 'u':
				if (bExclude)
					retCriteria->dwExcludeCategories |= itemcatUseful;
				else if (bMustHave)
					retCriteria->dwMustHaveCategories |= itemcatUseful;
				else
					retCriteria->dwItemCategories |= itemcatUseful;
				break;

			case 'v':
				if (bExclude)
					retCriteria->dwExcludeCategories |= itemcatDrive;
				else if (bMustHave)
					retCriteria->dwMustHaveCategories |= itemcatDrive;
				else
					retCriteria->dwItemCategories |= itemcatDrive;
				break;

			case 'w':
				if (bExclude)
					retCriteria->dwExcludeCategories |= itemcatWeapon | itemcatLauncher;
				else if (bMustHave)
					retCriteria->dwMustHaveCategories |= itemcatWeapon | itemcatLauncher;
				else
					retCriteria->dwItemCategories |= itemcatWeapon | itemcatLauncher;
				break;

			case 'I':
				retCriteria->bInstalledOnly = true;
				break;

			case 'D':
				retCriteria->wFlagsMustBeSet |= flagDamaged;
				break;

			case 'F':
				retCriteria->Frequency = ParseCriteriaParam(&pPos);
				break;

			case 'L':
				{
				int iHigh;
				int iLow;

				if (ParseCriteriaParamLevelRange(&pPos, &iLow, &iHigh))
					{
					if (iHigh == -1)
						retCriteria->iEqualToLevel = iLow;
					else
						{
						retCriteria->iGreaterThanLevel = iLow - 1;
						retCriteria->iLessThanLevel = iHigh + 1;
						}
					}

				break;
				}

			case 'M':
				retCriteria->bLauncherMissileOnly = true;
				retCriteria->dwItemCategories |= itemcatMissile;
				break;

			case 'N':
				retCriteria->wFlagsMustBeCleared |= flagDamaged;
				break;

			case 'S':
				retCriteria->bUsableItemsOnly = true;
				break;

			case 'U':
				retCriteria->bNotInstalledOnly = true;
				break;

			case 'V':
				retCriteria->bExcludeVirtual = false;
				break;

			case '+':
			case '-':
				{
				bool bSpecialAttrib = false;
				char chChar = *pPos;
				pPos++;

				//	Deal with category exclusion

				//	Get the modifier

				char *pStart = pPos;
				while (*pPos != '\0' && *pPos != ';' && *pPos != ' ' && *pPos != '\t')
					{
					if (*pPos == ':')
						bSpecialAttrib = true;
					pPos++;
					}

				CString sModifier = CString(pStart, pPos - pStart);

				//	Required or Not Allowed

				if (chChar == '+')
					{
					if (bSpecialAttrib)
						retCriteria->SpecialAttribRequired.Insert(sModifier);
					else
						retCriteria->ModifiersRequired.Insert(sModifier);
					}
				else
					{
					if (bSpecialAttrib)
						retCriteria->SpecialAttribNotAllowed.Insert(sModifier);
					else
						retCriteria->ModifiersNotAllowed.Insert(sModifier);
					}

				//	No trailing semi

				if (*pPos == '\0')
					pPos--;

				break;
				}

			case '~':
				bExclude = true;
				break;

			case '^':
				bMustHave = true;
				break;

			case '=':
			case '>':
			case '<':
				{
				char chChar = *pPos;
				pPos++;

				//	<= or >=

				int iEqualAdj;
				if (*pPos == '=')
					{
					pPos++;
					iEqualAdj = 1;
					}
				else
					iEqualAdj = 0;

				//	Is this price?

				char comparison;
				if (*pPos == '$' || *pPos == '#')
					comparison = *pPos++;
				else
					comparison = '\0';

				//	Get the number

				char *pNewPos;
				int iValue = strParseInt(pPos, 0, &pNewPos);

				//	Back up one because we will increment at the bottom
				//	of the loop.

				if (pPos != pNewPos)
					pPos = pNewPos - 1;

				//	Price limits

				if (comparison == '$')
					{
					if (chChar == '=')
						retCriteria->iEqualToPrice = iValue;
					else if (chChar == '>')
						retCriteria->iGreaterThanPrice = iValue - iEqualAdj;
					else if (chChar == '<')
						retCriteria->iLessThanPrice = iValue + iEqualAdj;
					}

				//	Mass limits

				else if (comparison == '#')
					{
					if (chChar == '=')
						retCriteria->iEqualToMass = iValue;
					else if (chChar == '>')
						retCriteria->iGreaterThanMass = iValue - iEqualAdj;
					else if (chChar == '<')
						retCriteria->iLessThanMass = iValue + iEqualAdj;
					}

				//	Level limits

				else
					{
					if (chChar == '=')
						retCriteria->iEqualToLevel = iValue;
					else if (chChar == '>')
						retCriteria->iGreaterThanLevel = iValue - iEqualAdj;
					else if (chChar == '<')
						retCriteria->iLessThanLevel = iValue + iEqualAdj;
					}

				break;
				}
			}

		pPos++;
		}
	}

DWORD CItem::ParseFlags (ICCItem *pItem)

//	ParseFlags
//
//	Parse flags for objHasItem and others.

	{
	DWORD dwFlags = 0;

	if (pItem)
		{
		dwFlags |= (pItem->GetBooleanAt(CONSTLIT("ignoreCharges")) ? CItem::FLAG_IGNORE_CHARGES : 0);
		dwFlags |= (pItem->GetBooleanAt(CONSTLIT("ignoreData")) ? CItem::FLAG_IGNORE_DATA : 0);
		dwFlags |= (pItem->GetBooleanAt(CONSTLIT("ignoreDisrupted")) ? CItem::FLAG_IGNORE_DISRUPTED : 0);
		dwFlags |= (pItem->GetBooleanAt(CONSTLIT("ignoreEnhancements")) ? CItem::FLAG_IGNORE_ENHANCEMENTS : 0);
		dwFlags |= (pItem->GetBooleanAt(CONSTLIT("ignoreInstalled")) ? CItem::FLAG_IGNORE_INSTALLED : 0);
		}

	return dwFlags;
	}

void CItem::ReadFromCCItem (CCodeChain &CC, ICCItem *pBuffer)

//	ReadFromCCItem
//
//	Reads an item from ICCItem
//
//	The first version stored 3 ints
//	The second version stored 2 ints + 4 elements if m_pExtra was allocated
//	The third version stores 2 ints + 5 elements (1 int version + 4 elements for extra)

	{
	*this = CItem();

	if (!pBuffer->IsNil())
		{
		//	Load the item type

		DWORD dwUNID = (DWORD)pBuffer->GetElement(0)->GetIntegerValue();
		m_pItemType = g_pUniverse->FindItemType(dwUNID);
		if (m_pItemType == NULL)
			return;

		//	Next is the count, flags, and installed

		DWORD *pDest = (DWORD *)this;
		pDest[1] = (DWORD)pBuffer->GetElement(1)->GetIntegerValue();

		//	Previous version

		if (pBuffer->GetCount() == 3)
			{
			DWORD dwLoad = (DWORD)pBuffer->GetElement(2)->GetIntegerValue();
			if (dwLoad)
				{
				Extra();
				m_pExtra->m_dwCharges = LOBYTE(LOWORD(dwLoad));
				m_pExtra->m_dwVariant = 0;
				m_pExtra->m_dwDisruptedTime = 0;
				m_pExtra->m_Mods = CItemEnhancement(HIWORD(dwLoad));
				}
			}

		//	Otherwise

		else if (pBuffer->GetCount() > 3)
			{
			int iStart = 2;
			CString sData;

			Extra();

			//	Load the version, if we have it

			DWORD dwVersion = 45;
			if (pBuffer->GetCount() >= 7)
				{
				dwVersion = (DWORD)pBuffer->GetElement(iStart)->GetIntegerValue();
				iStart++;
				}

			//	Charges

			m_pExtra->m_dwCharges = (DWORD)pBuffer->GetElement(iStart)->GetIntegerValue();
			m_pExtra->m_dwVariant = (DWORD)pBuffer->GetElement(iStart+1)->GetIntegerValue();

			//	Mods

			if (pBuffer->GetElement(iStart+2)->IsIdentifier())
				{
				sData = pBuffer->GetElement(iStart+2)->GetStringValue();
				CMemoryReadStream Stream(sData.GetPointer(), sData.GetLength());
				Stream.Open();
				m_pExtra->m_Mods.ReadFromStream(dwVersion, &Stream);
				Stream.Close();
				}
			else
				m_pExtra->m_Mods = CItemEnhancement((DWORD)pBuffer->GetElement(iStart+2)->GetIntegerValue());

			//	Attribute data block

			sData = pBuffer->GetElement(iStart+3)->GetStringValue();
			CMemoryReadStream Stream(sData.GetPointer(), sData.GetLength());
			Stream.Open();
			m_pExtra->m_Data.ReadFromStream(&Stream);
			Stream.Close();

			//	Ionization

			if (pBuffer->GetCount() >= 8)
				m_pExtra->m_dwDisruptedTime = (DWORD)pBuffer->GetElement(iStart+4)->GetIntegerValue();
			}
		}
	}

void CItem::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Reads from a stream

	{
	DWORD dwLoad;
	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	m_pItemType = g_pUniverse->FindItemType(dwLoad);

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	m_dwCount = LOWORD(dwLoad);
	m_dwFlags = LOBYTE(HIWORD(dwLoad));
	m_dwInstalled = HIBYTE(HIWORD(dwLoad));

	//	Load SExtra struct

	if (Ctx.dwVersion >= 23)
		{
		Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
		if (dwLoad)
			{
			m_pExtra = new SExtra;

			if (Ctx.dwVersion >= 71)
				{
				Ctx.pStream->Read((char *)&m_pExtra->m_dwCharges, sizeof(DWORD));
				Ctx.pStream->Read((char *)&m_pExtra->m_dwVariant, sizeof(DWORD));
				}
			else
				{
				Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
				m_pExtra->m_dwCharges = LOWORD(dwLoad);
				m_pExtra->m_dwVariant = 0;
				}

			if (Ctx.dwVersion >= 58)
				Ctx.pStream->Read((char *)&m_pExtra->m_dwDisruptedTime, sizeof(DWORD));
			else
				m_pExtra->m_dwDisruptedTime = 0;

			m_pExtra->m_Mods.ReadFromStream(Ctx);
			m_pExtra->m_Data.ReadFromStream(Ctx);
			}
		else
			m_pExtra = NULL;
		}
	else
		{
		Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
		DWORD dwCharges = LOBYTE(LOWORD(dwLoad));
		DWORD dwCondition = HIBYTE(LOWORD(dwLoad));
		DWORD dwMods = HIWORD(dwLoad);

		if (dwCharges || dwCondition || dwMods)
			{
			m_pExtra = new SExtra;
			m_pExtra->m_dwCharges = dwCharges;
			m_pExtra->m_dwVariant = 0;
			m_pExtra->m_dwDisruptedTime = 0;
			}
		else
			m_pExtra = NULL;
		}
	}

bool CItem::RemoveEnhancement (DWORD dwID)

//	RemoveEnhancement
//
//	Removes the given enhancement by ID. Returns TRUE if the enhancement was
//	removed.

	{
	if (dwID == OBJID_NULL || m_pExtra == NULL || m_pExtra->m_Mods.GetID() != dwID)
		return false;

	m_pExtra->m_Mods = CItemEnhancement();

	return true;
	}

void CItem::SetCharges (int iCharges)

//	SetCharges
//
//	Sets the current charges

	{
	Extra();

	int iChargesLimit = m_pItemType->GetChargesLimit();
	if (iChargesLimit != -1)
		m_pExtra->m_dwCharges = Min(iCharges, iChargesLimit);
	else
		m_pExtra->m_dwCharges = iCharges;
	}

void CItem::SetDisrupted (DWORD dwDuration)

//	SetDisrupted
//
//	Ionizes the item for the given number of ticks

	{
	Extra();

	if (m_pExtra->m_dwDisruptedTime == INFINITE_TICK)
		;
	else if (dwDuration == INFINITE_TICK)
		m_pExtra->m_dwDisruptedTime = INFINITE_TICK;
	else
		{
		//	Disruption time is cumulative

		DWORD dwNow = (DWORD)g_pUniverse->GetTicks();
		if (m_pExtra->m_dwDisruptedTime <= dwNow)
			m_pExtra->m_dwDisruptedTime = dwNow + dwDuration;
		else
			m_pExtra->m_dwDisruptedTime += dwDuration;
		}
	}

bool CItem::SetLevel (int iLevel, CString *retsError)

//  SetLevel
//
//  Sets the level for scalable items. Returns TRUE if successful.

    {
    if (!GetType()->IsScalable())
        {
        if (retsError)
            *retsError = strPatternSubst(CONSTLIT("Item [%08x] is not scalable."), GetType()->GetUNID());
        return false;
        }

    if (iLevel < GetType()->GetLevel() || iLevel > GetType()->GetMaxLevel())
        {
        if (retsError)
            *retsError = strPatternSubst(CONSTLIT("Item [%08x] cannot be scaled to level %d."), GetType()->GetUNID(), iLevel);
        return false;
        }

    SetVariantHigh(iLevel - GetType()->GetLevel());
    return true;
    }

bool CItem::SetProperty (CItemCtx &Ctx, const CString &sName, ICCItem *pValue, CString *retsError)

//	SetProperty
//
//	Sets item property. If we cannot set the property we return an error. If
//	retsError is blank then we cannot set the property because the value is Nil.

	{
	CCodeChain &CC = g_pUniverse->GetCC();
	CInstalledDevice *pDevice;

	if (IsEmpty())
		{
		if (retsError) *retsError = CONSTLIT("Unable to set propery on a null item.");
		return false;
		}

	else if (strEquals(sName, PROPERTY_CHARGES))
		{
		if (pValue == NULL || pValue->IsNil())
			{
			if (retsError) *retsError = NULL_STR;
			return false;
			}
			
		SetCharges(pValue->GetIntegerValue());
		}
	else if (strEquals(sName, PROPERTY_DAMAGED))
		SetDamaged((pValue == NULL) || !pValue->IsNil());

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
			return false;
			}
		else
			SetCharges(Max(0, GetCharges() + pValue->GetIntegerValue()));
		}

	else if (strEquals(sName, PROPERTY_INSTALLED))
		{
		if (pValue && pValue->IsNil())
			SetInstalled(-1);
		else
			{
			if (retsError) *retsError = CONSTLIT("Unable to set installation flag on item.");
			return false;
			}
		}

    else if (strEquals(sName, PROPERTY_KNOWN))
		{
		m_pItemType->SetKnown(pValue && !pValue->IsNil());
		}

    else if (strEquals(sName, PROPERTY_LEVEL))
        {
        //  If this is armor, then we remember the current damaged state and
        //  carry that forward to the new level.

        CInstalledArmor *pArmor;
        int iArmorIntegrity;
        if (pArmor = Ctx.GetArmor())
            iArmorIntegrity = mathPercent(pArmor->GetHitPoints(), Ctx.GetArmorClass()->GetMaxHP(Ctx));

        //  Set the level

        if (!SetLevel((pValue ? pValue->GetIntegerValue() : 0), retsError))
            return false;

        //  Set armor HP

        if (pArmor)
            pArmor->SetHitPoints(mathAdjust(Ctx.GetArmorClass()->GetMaxHP(Ctx), iArmorIntegrity));

        return true;
        }

	//	If this is an installed device, then pass it on

	else if (pDevice = Ctx.GetDevice())
		return pDevice->SetProperty(Ctx, sName, pValue, retsError);

	//	Otherwise, nothing

	else
		{
		if (retsError) *retsError = strPatternSubst(CONSTLIT("Unknown item property: %s."), sName);
		return false;
		}

	return true;
	}

ICCItem *CItem::WriteToCCItem (CCodeChain &CC) const

//	WriteToCCItem
//
//	Converts item to a ICCItem

	{
	ICCItem *pResult = CC.CreateLinkedList();
	if (pResult->IsError())
		return pResult;

	CCLinkedList *pList = (CCLinkedList *)pResult;
	ICCItem *pInt;

	//	Next integer is the item UNID

	pInt = CC.CreateInteger(GetType()->GetUNID());
	pList->Append(CC, pInt);
	pInt->Discard(&CC);

	//	Next is the count, flags, and installed

	DWORD *pSource = (DWORD *)this;
	pInt = CC.CreateInteger(pSource[1]);
	pList->Append(CC, pInt);
	pInt->Discard(&CC);

	//	Save extra

	if (m_pExtra)
		{
		//	Save the version (starting in v45)

		pInt = CC.CreateInteger(CSystem::GetSaveVersion());
		pList->Append(CC, pInt);
		pInt->Discard(&CC);

		//	Charges

		pInt = CC.CreateInteger(m_pExtra->m_dwCharges);
		pList->Append(CC, pInt);
		pInt->Discard(&CC);

		//	Condition

		pInt = CC.CreateInteger(m_pExtra->m_dwVariant);
		pList->Append(CC, pInt);
		pInt->Discard(&CC);

		//	Mods

		CMemoryWriteStream Stream;
		Stream.Create();
		m_pExtra->m_Mods.WriteToStream(&Stream);
		Stream.Close();

		pInt = CC.CreateString(CString(Stream.GetPointer(), Stream.GetLength()));
		pList->Append(CC, pInt);
		pInt->Discard(&CC);

		//	Attribute data block

		Stream.Create();
		m_pExtra->m_Data.WriteToStream(&Stream);
		Stream.Close();

		pInt = CC.CreateString(CString(Stream.GetPointer(), Stream.GetLength()));
		pList->Append(CC, pInt);
		pInt->Discard(&CC);

		//	Disrupted time

		pInt = CC.CreateInteger(m_pExtra->m_dwDisruptedTime);
		pList->Append(CC, pInt);
		pInt->Discard(&CC);
		}

	return pResult;
	}

void CItem::WriteToStream (IWriteStream *pStream)

//	WriteToStream
//
//	Writes to a stream
//
//	DWORD		item type UNID
//	DWORD		low = m_dwCount; high = (low = m_dwFlags; high = m_dwInstalled)
//	DWORD		non-zero if SExtra follows
//
//	SExtra
//	DWORD		m_dwCharges
//	DWORD		m_dwCondition
//	DWORD		m_dwDisruptedTime
//	CItemEnhancement
//	CAttributeDataBlock

	{
	DWORD dwSave = m_pItemType->GetUNID();
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	dwSave = MAKELONG(m_dwCount, MAKEWORD(m_dwFlags, m_dwInstalled));
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	//	Save SExtra

	dwSave = (m_pExtra ? 0xffffffff : 0);
	pStream->Write((char *)&dwSave, sizeof(DWORD));
	if (m_pExtra)
		{
		pStream->Write((char *)&m_pExtra->m_dwCharges, sizeof(DWORD));
		pStream->Write((char *)&m_pExtra->m_dwVariant, sizeof(DWORD));
		pStream->Write((char *)&m_pExtra->m_dwDisruptedTime, sizeof(DWORD));

		m_pExtra->m_Mods.WriteToStream(pStream);

		//	Note: Currently does not support saving object references

		m_pExtra->m_Data.WriteToStream(pStream);
		}
	}

//	CItemCriteria ------------------------------------------------------------

CItemCriteria::CItemCriteria (const CItemCriteria &Copy)

//	Criteria copy constructor

	{
	dwItemCategories = Copy.dwItemCategories;
	dwExcludeCategories = Copy.dwExcludeCategories;
	dwMustHaveCategories = Copy.dwMustHaveCategories;
	wFlagsMustBeSet = Copy.wFlagsMustBeSet;
	wFlagsMustBeCleared = Copy.wFlagsMustBeCleared;

	bUsableItemsOnly = Copy.bUsableItemsOnly;
	bExcludeVirtual = Copy.bExcludeVirtual;
	bInstalledOnly = Copy.bInstalledOnly;
	bNotInstalledOnly = Copy.bNotInstalledOnly;
	bLauncherMissileOnly = Copy.bLauncherMissileOnly;

	ModifiersRequired = Copy.ModifiersRequired;
	ModifiersNotAllowed = Copy.ModifiersNotAllowed;
	SpecialAttribRequired = Copy.SpecialAttribRequired;
	SpecialAttribNotAllowed = Copy.SpecialAttribNotAllowed;
	Frequency = Copy.Frequency;

	iEqualToLevel = Copy.iEqualToLevel;
	iGreaterThanLevel = Copy.iGreaterThanLevel;
	iLessThanLevel = Copy.iLessThanLevel;

	iEqualToPrice = Copy.iEqualToPrice;
	iGreaterThanPrice = Copy.iGreaterThanPrice;
	iLessThanPrice = Copy.iLessThanPrice;

	iEqualToMass = Copy.iEqualToMass;
	iGreaterThanMass = Copy.iGreaterThanMass;
	iLessThanMass = Copy.iLessThanMass;

	sLookup = Copy.sLookup;
	pFilter = Copy.pFilter;
	if (pFilter)
		pFilter->Reference();
	}

CItemCriteria &CItemCriteria::operator= (const CItemCriteria &Copy)

//	Criteria equals operator

	{
	if (pFilter)
		pFilter->Discard(&g_pUniverse->GetCC());

	dwItemCategories = Copy.dwItemCategories;
	dwExcludeCategories = Copy.dwExcludeCategories;
	dwMustHaveCategories = Copy.dwMustHaveCategories;
	wFlagsMustBeSet = Copy.wFlagsMustBeSet;
	wFlagsMustBeCleared = Copy.wFlagsMustBeCleared;

	bUsableItemsOnly = Copy.bUsableItemsOnly;
	bExcludeVirtual = Copy.bExcludeVirtual;
	bInstalledOnly = Copy.bInstalledOnly;
	bNotInstalledOnly = Copy.bNotInstalledOnly;
	bLauncherMissileOnly = Copy.bLauncherMissileOnly;

	ModifiersRequired = Copy.ModifiersRequired;
	ModifiersNotAllowed = Copy.ModifiersNotAllowed;
	SpecialAttribRequired = Copy.SpecialAttribRequired;
	SpecialAttribNotAllowed = Copy.SpecialAttribNotAllowed;
	Frequency = Copy.Frequency;

	iEqualToLevel = Copy.iEqualToLevel;
	iGreaterThanLevel = Copy.iGreaterThanLevel;
	iLessThanLevel = Copy.iLessThanLevel;

	iEqualToPrice = Copy.iEqualToPrice;
	iGreaterThanPrice = Copy.iGreaterThanPrice;
	iLessThanPrice = Copy.iLessThanPrice;

	iEqualToMass = Copy.iEqualToMass;
	iGreaterThanMass = Copy.iGreaterThanMass;
	iLessThanMass = Copy.iLessThanMass;

	sLookup = Copy.sLookup;
	pFilter = Copy.pFilter;
	if (pFilter)
		pFilter->Reference();

	return *this;
	}

CItemCriteria::~CItemCriteria (void)

	{
	if (pFilter)
		pFilter->Discard(&g_pUniverse->GetCC());
	}

bool CItemCriteria::GetExplicitLevelMatched (int *retiMin, int *retiMax) const

//	GetExplicitLevelMatched
//
//	Returns the min and max levels that this criteria matches, assuming that
//	there are explicit level criteria. If not, we return FALSE. If min or max
//	criteria are missing, we return -1.

	{
	if (!sLookup.IsBlank())
		{
		const CItemCriteria *pCriteria = g_pUniverse->GetDesignCollection().GetDisplayAttributes().FindCriteriaByID(sLookup);
		if (pCriteria == NULL)
			return false;

		return pCriteria->GetExplicitLevelMatched(retiMin, retiMax);
		}
	else if (iEqualToLevel != -1)
		{
		*retiMin = iEqualToLevel;
		*retiMax = iEqualToLevel;
		return true;
		}
	else if (iLessThanLevel == -1 && iGreaterThanLevel == -1)
		{ 
		*retiMin = -1;
		*retiMax = -1;
		return false;
		}
	else
		{
		if (iLessThanLevel != -1)
			*retiMax = iLessThanLevel - 1;
		else
			*retiMax = -1;

		if (iGreaterThanLevel != -1)
			*retiMin = iGreaterThanLevel + 1;
		else
			*retiMin = -1;

		return true;
		}
	}

int CItemCriteria::GetMaxLevelMatched (void) const

//	GetMaxLevelMatches
//
//	Returns the maximum item level that this criteria matches. If there is no
//	explicit level match, then we laboriously check for every single
//	item type that matches and return the max level.

	{
	int i;

	if (!sLookup.IsBlank())
		{
		const CItemCriteria *pCriteria = g_pUniverse->GetDesignCollection().GetDisplayAttributes().FindCriteriaByID(sLookup);
		if (pCriteria == NULL)
			return -1;

		return pCriteria->GetMaxLevelMatched();
		}

	if (iEqualToLevel != -1)
		return iEqualToLevel;

	if (iLessThanLevel != -1)
		return iLessThanLevel - 1;

	//	Look at every single item that might match

	int iMaxLevel = -1;
	for (i = 0; i < g_pUniverse->GetItemTypeCount(); i++)
		{
		CItemType *pType = g_pUniverse->GetItemType(i);
		CItem Item(pType, 1);

		if (Item.MatchesCriteria(*this))
			iMaxLevel = Max(iMaxLevel, pType->GetLevel());
		}

	return iMaxLevel;
	}

CString CItemCriteria::GetName (void) const

//	GetName
//
//	Returns the name of this criteria. We return NULL_STR if we cannot find the name.

	{
	//	If this is a lookup, we get the name from that.

	if (!sLookup.IsBlank())
		{
		CString sName;
		if (!g_pUniverse->GetDesignCollection().GetDisplayAttributes().FindCriteriaNameByID(sLookup, &sName))
			return NULL_STR;

		return sName;
		}

	//	Otherwise, we see if this is a shared criteria with a name.

	else
		{
		CString sName;
		CString sCriteria = CItem::GenerateCriteria(*this);
		if (!g_pUniverse->GetDesignCollection().GetDisplayAttributes().FindCriteriaName(sCriteria, &sName))
			return NULL_STR;

		return sName;
		}
	}

bool CItemCriteria::Intersects (const CItemCriteria &Src) const

//	Intersects
//
//	Returns TRUE if the two criterias match at least one item in common.

	{
	for (int i = 0; i < g_pUniverse->GetItemTypeCount(); i++)
		{
		CItemType *pType = g_pUniverse->GetItemType(i);
		CItem Item(pType, 1);

		if (Item.MatchesCriteria(*this) && Item.MatchesCriteria(Src))
			return true;
		}

	return false;
	}
