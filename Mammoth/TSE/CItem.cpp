//	CItem.cpp
//
//	CItem class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#ifdef DEBUG
//#define DEBUG_ITEM_NAME_MATCH
#endif

#define CAN_BE_INSTALLED_EVENT					CONSTLIT("CanBeInstalled")
#define CAN_BE_UNINSTALLED_EVENT				CONSTLIT("CanBeUninstalled")
#define CAN_ENHANCE_ITEM_EVENT					CONSTLIT("CanEnhanceItem")
#define GET_ENHANCEMENT_EVENT					CONSTLIT("GetEnhancement")
#define GET_TRADE_SERVICES_EVENT				CONSTLIT("GetTradeServices")
#define ON_ADDED_AS_ENHANCEMENT_EVENT			CONSTLIT("OnAddedAsEnhancement")
#define ON_DISABLED_EVENT						CONSTLIT("OnDisable")
#define ON_ENABLED_EVENT						CONSTLIT("OnEnable")
#define ON_ENHANCE_ITEM_EVENT					CONSTLIT("OnEnhanceItem")
#define ON_INSTALL_EVENT						CONSTLIT("OnInstall")
#define ON_OBJ_DESTROYED_EVENT					CONSTLIT("OnObjDestroyed")
#define ON_REACTOR_OVERLOAD_EVENT				CONSTLIT("OnReactorOverload")
#define ON_REMOVED_AS_ENHANCEMENT_EVENT			CONSTLIT("OnRemovedAsEnhancement")
#define ON_UNINSTALL_EVENT						CONSTLIT("OnUninstall")

#define SPECIAL_PROPERTY						CONSTLIT("property:")

CItemEnhancement CItem::m_NullMod;
CItem CItem::m_NullItem;

CItem::CItem (void)

//	CItem constructor

	{
	}

CItem::CItem (const CItem &Copy)

//	CItem constructor

	{
	m_pItemType = Copy.m_pItemType;
	m_dwCount = Copy.m_dwCount;
	m_dwFlags = Copy.m_dwFlags;

	if (Copy.m_pExtra)
		m_pExtra = new SExtra(*Copy.m_pExtra);
	else
		m_pExtra = NULL;
	}

CItem::CItem (CItemType &ItemType, int iCount)

//	CItem constructor

	{
	m_pItemType = &ItemType;
	m_dwCount = (DWORD)iCount;
	m_dwFlags = 0;
	m_pExtra = NULL;

	int iCharges = ItemType.GetCharges();
	if (iCharges)
		SetCharges(iCharges);

	int iUnknownIndex = ItemType.GetRandomUnknownTypeIndex();
	if (iUnknownIndex != -1)
		SetUnknownIndex(iUnknownIndex);

	m_pItemType->InitItemData(*this);
	}

CItem::CItem (CItemType *pItemType, int iCount)

//	CItem constructor

	{
	if (!pItemType)
		throw CException(ERR_FAIL);

	m_pItemType = pItemType;
	m_dwCount = (DWORD)iCount;
	m_dwFlags = 0;
	m_pExtra = NULL;

	int iCharges = pItemType->GetCharges();
	if (iCharges)
		SetCharges(iCharges);

	int iUnknownIndex = pItemType->GetRandomUnknownTypeIndex();
	if (iUnknownIndex != -1)
		SetUnknownIndex(iUnknownIndex);

	m_pItemType->InitItemData(*this);
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

	if (Copy.m_pExtra)
		m_pExtra = new SExtra(*Copy.m_pExtra);
	else
		m_pExtra = NULL;

	return *this;
	}

void CItem::AccumulateCustomAttributes (TArray<SDisplayAttribute> *retList, ICCItem *pData) const

//	AccumulateCustomAttributes
//
//	Adds display attributes defined by <GetDisplayAttributes>

	{
	//	See if we have <GetDisplayAttributes> event. If not, we're done.

	SEventHandlerDesc Event;
	if (!m_pItemType->FindEventHandlerItemType(CItemType::evtGetDisplayAttributes, &Event))
		return;

	CCodeChainCtx Ctx(GetUniverse());

	Ctx.SetItemType(GetType());
	Ctx.DefineContainingType(m_pItemType);
	Ctx.SaveAndDefineSourceVar(GetSource());
	Ctx.SaveAndDefineItemVar(*this);
	Ctx.SaveAndDefineDataVar(pData);

	ICCItemPtr pResult = Ctx.RunCode(Event);
	if (pResult->IsError())
		::kernelDebugLogPattern("[%08x] <GetDisplayAttributes>: %s", GetType()->GetUNID(), pResult->GetStringValue());
	else if (!pResult->IsNil())
		{
		//	We expect a list of attributes.

		for (int i = 0; i < pResult->GetCount(); i++)
			{
			ICCItem *pEntry = pResult->GetElement(i);

			SDisplayAttribute Entry;
			if (CDisplayAttributeDefinitions::InitFromCCItem(pEntry, Entry))
				retList->Insert(Entry);
			}
		}
	}

bool CItem::AccumulateEnhancementDisplayAttributes (TArray<SDisplayAttribute> &retList) const

//	AccumulateEnhancementDisplayAttributes
//
//	Accumulates a list of display attributes that are currently enhancing the 
//	item. Returns FALSE if there are none.

	{
	DEBUG_TRY

	int iOriginalCount = retList.GetCount();

	TSharedPtr<CItemEnhancementStack> pEnhancements = GetEnhancementStack();
	if (pEnhancements)
		pEnhancements->AccumulateAttributes(*this, &retList);

	if (IsEnhanced())
		retList.Insert(SDisplayAttribute(attribPositive, CONSTLIT("+enhanced"), true));

	return (retList.GetCount() != iOriginalCount);

	DEBUG_CATCH
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
		m_pExtra->m_Mods.SetID(GetUniverse().CreateGlobalID());

	return m_pExtra->m_Mods.GetID();
	}

CString CItem::CalcSortKey (void) const

//	CalcSortKey
//
//	Returns a sort key

	{
	CItemCtx Ctx(this);

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

	if (IsInstalled() && IsArmor())
		sCat.Append(strPatternSubst(CONSTLIT("%03d%08x"), GetInstalled(), pType->GetUNID()));
	else
		sCat.Append(CONSTLIT("99900000000"));

	//	Within category, sort by level (highest first)

	sCat.Append(strPatternSubst(CONSTLIT("%02d"), MAX_ITEM_LEVEL - GetApparentLevel()));

	//	Enhanced items before others

	if (IsEnhanced())
		sCat.Append(CONSTLIT("0"));
	else if (IsDamaged())
		sCat.Append(CONSTLIT("2"));
	else
		sCat.Append(CONSTLIT("1"));

	CString sName = pType->GetSortName(Ctx);

	//	For ammo charges, sort used magazines first

	CString sCharges;
	if (pType->AreChargesAmmo())
		sCharges = strPatternSubst(CONSTLIT("-%08d"), GetCharges());

	return strPatternSubst(CONSTLIT("%s%s%s%s%d"), sInstalled, sCat, sName, sCharges, ((DWORD)(int)this) % 0x10000);
	}

bool CItem::CanBeUsed (CString *retsUseKey) const

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

	if (UseDesc.bOnlyIfUndamaged && IsDamaged())
		return false;

	if (UseDesc.bOnlyIfEnabled)
		{
		const CInstalledDevice *pDevice = GetInstalledDevice();
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
			const CInstalledArmor *pArmor = GetInstalledArmor();
			if (pArmor == NULL || !pArmor->IsComplete() || !pArmor->IsPrime())
				return false;
			}
		else if (UseDesc.bAsArmorSet)
			{
			const CInstalledArmor *pArmor = GetInstalledArmor();
			if (pArmor == NULL || !pArmor->IsPrime())
				return false;
			}
		}

	//	Return the use key, if required

	if (retsUseKey)
		*retsUseKey = UseDesc.sUseKey;

	return true;
	}

void CItem::ClearDamaged (void)

//	ClearDamaged
//
//	Remove damage

	{
	m_dwFlags &= ~flagDamaged;

	if (m_pExtra)
		m_pExtra->m_iDamagedHP = 0;
	}

void CItem::ClearInstalled (void)

//	ClearInstalled
//
//	Clear installed connection.

	{
	if (m_pExtra)
		{
		m_pExtra->m_pInstalled = NULL;
		m_pExtra->m_iInstalled = EInstalled::None;
		m_pExtra->m_iInstalledIndex = -1;
		}
	}

CItem CItem::CreateItemByName (CUniverse &Universe, const CString &sName, const CItemCriteria &Criteria, bool bActualName)

//	CreateItemByName
//
//	Generates an item structure based on natural language
//	name. If there is ambiguity, a random item is chosen.

	{
	if (!Universe.GetDesignCollection().IsBindComplete())
		{
		ASSERT(false);
		return CItem();
		}

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
	for (int i = 0; i < Universe.GetItemTypeCount(); i++)
		{
		CItemType *pType = Universe.GetItemType(i);
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
		m_pExtra = new SExtra;
	}

bool CItem::FireCanBeInstalled (CSpaceObject *pSource, int iSlot, CString *retsError) const

//	FireCanBeInstalled
//
//	CanBeInstalled event

	{
	SEventHandlerDesc Event;
	if (m_pItemType->FindEventHandler(CAN_BE_INSTALLED_EVENT, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());

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

		ICCItemPtr pResult = Ctx.RunCode(Event);

		bool bCanBeInstalled;
		if (pResult->IsError())
			{
			*retsError = pResult->GetStringValue();
			if (pSource)
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
		CCodeChainCtx Ctx(GetUniverse());

		Ctx.DefineContainingType(m_pItemType);
		Ctx.SaveAndDefineSourceVar(pSource);
		Ctx.SaveAndDefineItemVar(*this);

		ICCItemPtr pResult = Ctx.RunCode(Event);

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

		return bCanBeUninstalled;
		}
	else
		return true;
	}

bool CItem::FireCanEnhanceItem (const CSpaceObject &TargetObj, const CItem &TargetItem, SEnhanceItemResult &retResult, CString *retsError) const

//	FireCanEnhanceItem
//
//	Fires <CanEnhanceItem> event. If we do not handle this event, we return TRUE
//	and retResult.iResult is eisUnknown.
//
//	If we get an error, we return FALSE.
//
//	LATER: This should not require TargetObj. We need to be able to call this
//	from an item call (e.g., something like itmEnhance). Instead, we should take
//	an optional object pointer.

	{
	retResult = SEnhanceItemResult();

	SEventHandlerDesc Event;
	if (!m_pItemType->FindEventHandler(CAN_ENHANCE_ITEM_EVENT, &Event))
		{
		retResult.iResult = eisUnknown;
		return true;
		}

	CCodeChainCtx Ctx(GetUniverse());
	Ctx.DefineContainingType(m_pItemType);
	Ctx.SaveAndDefineSourceVar(&TargetObj);
	Ctx.SaveAndDefineItemVar(*this);

	ICCItemPtr pTargetItem(::CreateListFromItem(TargetItem));
	ICCItemPtr pData(ICCItem::SymbolTable);
	pData->SetAt(CONSTLIT("targetItem"), pTargetItem);

	Ctx.SaveAndDefineDataVar(pData);

	ICCItemPtr pResult = Ctx.RunCode(Event);
	if (pResult->IsError())
		{
		if (retsError)
			*retsError = strPatternSubst(CONSTLIT("[%08x %s] <CanEnhanceItem> error: %s"), GetType()->GetUNID(), GetNounPhrase(), pResult->GetStringValue());

		return false;
		}

	CString sResultCode = pResult->GetStringAt(CONSTLIT("resultCode"));
	retResult.iResult = CItemEnhancement::AsEnhanceItemStatus(sResultCode);
	if (retResult.iResult == eisUnknown)
		{
		if (retsError)
			*retsError = strPatternSubst(CONSTLIT("[%08x %s] <CanEnhanceItem> Unknown resultCode: %s"), GetType()->GetUNID(), GetNounPhrase(), sResultCode);

		return false;
		}

	retResult.sDesc = pResult->GetStringAt(CONSTLIT("desc"));

	//	Done

	return true;
	}

void CItem::FireCustomEvent (CItemCtx &ItemCtx, const CString &sEvent, ICCItem *pData, ICCItem **retpResult) const

//	FireCustomEvent
//
//	Triggers the given event.

	{
	CCodeChainCtx Ctx(GetUniverse());

	SEventHandlerDesc Event;
	if (GetType()->FindEventHandler(sEvent, &Event))
		{
		//	Define some globals

		Ctx.DefineContainingType(m_pItemType);
		Ctx.SaveAndDefineSourceVar(ItemCtx.GetSource());
		Ctx.SaveAndDefineItemVar(*this);
		Ctx.SaveAndDefineDataVar(pData);

		//	Run code

		ICCItemPtr pResult = Ctx.RunCode(Event);
		if (pResult->IsError())
			{
			if (ItemCtx.GetSource())
				ItemCtx.GetSource()->ReportEventError(strPatternSubst(CONSTLIT("Item %x %s"), GetType()->GetUNID(), sEvent), pResult);
			else
				::kernelDebugLogPattern("[%08x] <%s>: %s", GetType()->GetUNID(), sEvent, pResult->GetStringValue());
			}

		//	Either return the event result or discard it

		if (retpResult)
			*retpResult = pResult->Reference();
		}
	else
		{
		if (retpResult)
			*retpResult = Ctx.CreateNil();
		}
	}

ICCItemPtr CItem::FireGetTradeServices (const CSpaceObject &SourceObj) const

//	FireGetTradeServices
//
//	GetTradeServices event. Returns NULL if none found.

	{
	CCodeChainCtx CCX(GetUniverse());

	SEventHandlerDesc Event;
	if (!GetType()->FindEventHandler(GET_TRADE_SERVICES_EVENT, &Event))
		return NULL;

	CCX.DefineContainingType(GetType());
	CCX.SaveAndDefineSourceVar(&SourceObj);
	CCX.SaveAndDefineItemVar(*this);

	ICCItemPtr pResult = CCX.RunCode(Event);
	if (pResult->IsError())
		{
		SourceObj.ReportEventError(strPatternSubst(CONSTLIT("Item %x %s"), GetType()->GetUNID(), GET_TRADE_SERVICES_EVENT), pResult);
		return NULL;
		}

	return pResult;
	}

void CItem::FireOnAddedAsEnhancement (CSpaceObject *pSource, const CItem &ItemEnhanced, EnhanceItemStatus iStatus) const

//	FireOnAddedAsEnhancement
//
//	OnAddedAsEnhancement event

	{
	SEventHandlerDesc Event;
	if (m_pItemType->FindEventHandler(ON_ADDED_AS_ENHANCEMENT_EVENT, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());

		Ctx.DefineContainingType(m_pItemType);
		Ctx.SaveAndDefineSourceVar(pSource);
		Ctx.SaveAndDefineItemVar(ItemEnhanced);
		Ctx.DefineInteger(CONSTLIT("aResult"), (int)iStatus);

		ICCItemPtr pResult = Ctx.RunCode(Event);
		if (pResult->IsError())
			pSource->ReportEventError(strPatternSubst(CONSTLIT("Item %x OnAddedAsEnhancement"), m_pItemType->GetUNID()), pResult);
		}
	}

bool CItem::FireOnDestroyCheck (CItemCtx &ItemCtx, DestructionTypes iCause, const CDamageSource &Attacker) const

//	FireOnDestroyCheck
//
//	See if this item can prevent the source from being destroyed. Returns FALSE
//	if we avoid destruction.

	{
	CCodeChainCtx Ctx(GetUniverse());

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

		ICCItemPtr pResult = Ctx.RunCode(Event);
		bool bResult = (pResult->IsNil() ? false : true);

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
		CCodeChainCtx Ctx(GetUniverse());

		Ctx.DefineContainingType(m_pItemType);
		Ctx.SaveAndDefineSourceVar(pSource);
		Ctx.SaveAndDefineItemVar(*this);

		ICCItemPtr pResult = Ctx.RunCode(Event);
		if (pResult->IsError())
			pSource->ReportEventError(strPatternSubst(CONSTLIT("Item %x OnDisable"), m_pItemType->GetUNID()), pResult);
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
		CCodeChainCtx Ctx(GetUniverse());

		Ctx.DefineContainingType(m_pItemType);
		Ctx.SaveAndDefineSourceVar(pSource);
		Ctx.SaveAndDefineItemVar(*this);
		Ctx.DefineSpaceObject(CONSTLIT("aObjDocked"), pSource);
		Ctx.DefineSpaceObject(CONSTLIT("aDockTarget"), pDockedAt);

		ICCItemPtr pResult = Ctx.RunCode(Event);
		if (pResult->IsError())
			pSource->ReportEventError(strPatternSubst(CONSTLIT("Item %x OnDocked"), m_pItemType->GetUNID()), pResult);
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
		CCodeChainCtx Ctx(GetUniverse());

		Ctx.DefineContainingType(m_pItemType);
		Ctx.SaveAndDefineSourceVar(pSource);
		Ctx.SaveAndDefineItemVar(*this);

		ICCItemPtr pResult = Ctx.RunCode(Event);
		if (pResult->IsError())
			pSource->ReportEventError(strPatternSubst(CONSTLIT("Item %x OnEnable"), m_pItemType->GetUNID()), pResult);
		}
	}

bool CItem::FireOnEnhanceItem (const CSpaceObject &TargetObj, const CItem &TargetItem, SEnhanceItemResult &retResult, CString *retsError) const

//	FireOnEnhanceItem
//
//	Fires <OnEnhanceItem> event. If we do not handle this event, we return TRUE
//	and retResult.iResult is eisUnknown.
//
//	If we get an error, we return FALSE.
//
//	LATER: This should not require TargetObj. We need to be able to call this
//	from an item call (e.g., something like itmEnhance). Instead, we should take
//	an optional object pointer.

	{
	retResult = SEnhanceItemResult();

	SEventHandlerDesc Event;
	if (!m_pItemType->FindEventHandler(ON_ENHANCE_ITEM_EVENT, &Event))
		{
		retResult.iResult = eisUnknown;
		return true;
		}

	CCodeChainCtx Ctx(GetUniverse());
	Ctx.DefineContainingType(m_pItemType);
	Ctx.SaveAndDefineSourceVar(&TargetObj);
	Ctx.SaveAndDefineItemVar(*this);

	ICCItemPtr pTargetItem(::CreateListFromItem(TargetItem));
	ICCItemPtr pData(ICCItem::SymbolTable);
	pData->SetAt(CONSTLIT("targetItem"), pTargetItem);

	Ctx.SaveAndDefineDataVar(pData);

	ICCItemPtr pResult = Ctx.RunCode(Event);
	if (pResult->IsError())
		{
		if (retsError)
			*retsError = strPatternSubst(CONSTLIT("[%08x %s] <OnEnhanceItem> error: %s"), GetType()->GetUNID(), GetNounPhrase(), pResult->GetStringValue());

		return false;
		}
	else if (pResult->IsNil())
		{
		retResult.iResult = eisUnknown;
		return true;
		}
	else
		{
		CString sResultCode = pResult->GetStringAt(CONSTLIT("resultCode"));
		retResult.iResult = CItemEnhancement::AsEnhanceItemStatus(sResultCode);
		if (retResult.iResult == eisUnknown)
			{
			if (retsError)
				*retsError = strPatternSubst(CONSTLIT("[%08x %s] <OnEnhanceItem> Unknown resultCode: %s"), GetType()->GetUNID(), GetNounPhrase(), sResultCode);

			return false;
			}

		retResult.sDesc = pResult->GetStringAt(CONSTLIT("desc"));
		return true;
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
		CCodeChainCtx Ctx(GetUniverse());

		Ctx.DefineContainingType(m_pItemType);
		Ctx.SaveAndDefineSourceVar(pSource);
		Ctx.SaveAndDefineItemVar(*this);

		ICCItemPtr pResult = Ctx.RunCode(Event);
		if (pResult->IsError())
			pSource->ReportEventError(strPatternSubst(CONSTLIT("Item %x OnInstall"), m_pItemType->GetUNID()), pResult);
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
		CCodeChainCtx CCCtx(GetUniverse());

		CCCtx.DefineContainingType(m_pItemType);
		CCCtx.SaveAndDefineSourceVar(pSource);
		CCCtx.SaveAndDefineItemVar(*this);
		CCCtx.DefineSpaceObject(CONSTLIT("aObjDestroyed"), Ctx.Obj);
		CCCtx.DefineSpaceObject(CONSTLIT("aDestroyer"), Ctx.Attacker.GetObj());
		CCCtx.DefineSpaceObject(CONSTLIT("aOrderGiver"), Ctx.GetOrderGiver());
		CCCtx.DefineSpaceObject(CONSTLIT("aWreckObj"), Ctx.pWreck);
		CCCtx.DefineBool(CONSTLIT("aDestroy"), Ctx.WasDestroyed());
		CCCtx.DefineString(CONSTLIT("aDestroyReason"), GetDestructionName(Ctx.iCause));

		ICCItemPtr pResult = CCCtx.RunCode(Event);
		if (pResult->IsError())
			pSource->ReportEventError(strPatternSubst(CONSTLIT("Item %x OnObjDestroyed"), m_pItemType->GetUNID()), pResult);
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
		CCodeChainCtx Ctx(GetUniverse());
		bool bHandled = false;

		Ctx.DefineContainingType(m_pItemType);
		Ctx.SaveAndDefineSourceVar(pSource);
		Ctx.SaveAndDefineItemVar(*this);

		ICCItemPtr pResult = Ctx.RunCode(Event);
		if (pResult->IsError())
			pSource->ReportEventError(strPatternSubst(CONSTLIT("Item %x OnReactorOverload"), m_pItemType->GetUNID()), pResult);
		else
			bHandled = !pResult->IsNil();

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
		CCodeChainCtx Ctx(GetUniverse());

		Ctx.DefineContainingType(m_pItemType);
		Ctx.SaveAndDefineSourceVar(pSource);
		Ctx.SaveAndDefineItemVar(ItemEnhanced);

		ICCItemPtr pResult = Ctx.RunCode(Event);
		if (pResult->IsError())
			pSource->ReportEventError(strPatternSubst(CONSTLIT("Item %x OnRemovedAsEnhancement"), m_pItemType->GetUNID()), pResult);
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
		CCodeChainCtx Ctx(GetUniverse());

		Ctx.DefineContainingType(m_pItemType);
		Ctx.SaveAndDefineSourceVar(pSource);
		Ctx.SaveAndDefineItemVar(*this);

		ICCItemPtr pResult = Ctx.RunCode(Event);
		if (pResult->IsError())
			pSource->ReportEventError(strPatternSubst(CONSTLIT("Item %x OnUninstall"), m_pItemType->GetUNID()), pResult);
		}
	}

const CItemList &CItem::GetComponents (void) const

//	GetComponents
//
//	Returns the list of components which make up this item.

	{
	return m_pItemType->GetComponents();
	}

int CItem::GetDamagedHP (void) const

//	GetDamagedHP
//
//	Returns the number of damaged HPs.

	{
	if (m_pItemType == NULL)
		return 0;

	int iDamagedHP;
	if (!IsDamaged(&iDamagedHP))
		return 0;

	if (iDamagedHP == 0)
		{
		if (const CArmorItem ArmorItem = AsArmorItem())
			{
			int iMaxHP = ArmorItem.GetMaxHP();
			iDamagedHP = iMaxHP / 2;
			}
		}

	return iDamagedHP;
	}

ICCItemPtr CItem::GetDataAsItem (const CString &sAttrib) const

//	GetDataAsItem
//
//	Returns data

	{
	if (m_pExtra)
		return m_pExtra->m_Data.GetDataAsItem(sAttrib);

	return ICCItemPtr(GetUniverse().GetCC().CreateNil());
	}

CString CItem::GetDesc (bool bActual) const

//	GetDesc
//
//	Gets the item description
	
	{
	if (CItemType *pUnknownType = GetUnknownTypeIfUnknown(bActual))
		return pUnknownType->GetDesc();

	//	If we have code, call it to generate the description

	SEventHandlerDesc Event;
	if (m_pItemType->FindEventHandlerItemType(CItemType::evtGetDescription, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());

		Ctx.SetEvent(eventGetDescription);
		Ctx.SetItemType(GetType());
		Ctx.DefineContainingType(m_pItemType);
		Ctx.SaveAndDefineSourceVar(GetSource());
		Ctx.SaveAndDefineItemVar(*this);

		ICCItemPtr pResult = Ctx.RunCode(Event);
		CString sDesc = pResult->GetStringValue();
		return sDesc;
		}

	//	Otherwise, get it from the item
	//	NOTE: Always get the actual description because we checked up above.

	else
		return m_pItemType->GetDesc(true); 
	}

bool CItem::GetDisplayAttributes (TArray<SDisplayAttribute> *retList, ICCItem *pData, bool bActual) const

//	GetDisplayAttributes
//
//	Returns a list of display attributes.

	{
	//	Empty and check for edge cases

	retList->DeleteAll();
	if (m_pItemType == NULL)
		return false;

	//	Add additional custom attributes

	if (IsKnown() || bActual)
		{
		//	Add the level

		retList->Insert(SDisplayAttribute(attribNeutral, strPatternSubst(CONSTLIT("level %d"), GetLevel())));

		//	Add display attributes

		GetUniverse().GetAttributeDesc().AccumulateAttributes(*this, retList);

		CDeviceClass *pDevice;

		//	Armor attributes

		if (const CArmorItem ArmorItem = AsArmorItem())
			ArmorItem.AccumulateAttributes(retList);

		//	Device attributes

		else if (const CDeviceItem DeviceItem = AsDeviceItem())
			DeviceItem.AccumulateAttributes(CItem(), retList);

		//	Missile attributes

		else if (m_pItemType->IsMissile() && (pDevice = m_pItemType->GetAmmoLauncher()))
			{
			//	LATER: We should pass in the source object (optionally). For
			//	missiles we use it to figure out the actual weapon that launches
			//	it so that we can accurately add attributes.

			const CItem Launcher(pDevice->GetItemType(), 1);
			const CDeviceItem LauncherItem = Launcher.AsDeviceItem();
			pDevice->AccumulateAttributes(LauncherItem, *this, retList);
			}

		//	Useable attribute

		if (m_pItemType->GetUseDesc())
			retList->Insert(SDisplayAttribute(attribNeutral, CONSTLIT("useable")));

		//	Add custom attributes

		AccumulateCustomAttributes(retList, pData);

		//	Military and Illegal attributes

		if (m_pItemType->HasLiteralAttribute(CONSTLIT("Military")))
			retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("military")));

		if (m_pItemType->HasLiteralAttribute(CONSTLIT("Illegal")))
			retList->Insert(SDisplayAttribute(attribNegative, CONSTLIT("illegal")));

		//	Charges

		CString sNoun;
		if (m_pItemType->TranslateText(*this, LANGID_CORE_CHARGES, NULL, &sNoun))
			retList->Insert(SDisplayAttribute(attribNeutral, CLanguage::ComposeNounPhrase(sNoun, GetCharges(), NULL_STR, 0, nounCountAlways)));
		}
	else
		{
		//	We show the level, but indicate that we're not sure.

		retList->Insert(SDisplayAttribute(attribNeutral, strPatternSubst(CONSTLIT("level %d?"), GetApparentLevel())));
		}

	//	Add various engine-based attributes (these are shown even if the item 
	//	type is unknown).

	if (IsDamaged())
		{
		if (IsArmor())
			{
			const CArmorItem ArmorItem = AsArmorItem();
			int iMaxHP;
			int iHP = ArmorItem.GetHP(&iMaxHP);
			int iArmorIntegrity = CArmorClass::CalcIntegrity(iHP, iMaxHP);
			retList->Insert(SDisplayAttribute(attribNegative, strPatternSubst(CONSTLIT("integrity: %d%%"), iArmorIntegrity)));
			}
		else
			retList->Insert(SDisplayAttribute(attribNegative, CONSTLIT("damaged")));
		}

	if (DWORD dwDisrupted = GetDisruptedDuration())
		{
		if (dwDisrupted == INFINITE_TICK)
			retList->Insert(SDisplayAttribute(attribNegative, CONSTLIT("ionized")));
		else
			retList->Insert(SDisplayAttribute(attribNegative, 
					strPatternSubst(CONSTLIT("ionized: %s"), CLanguage::ComposeNumber(CLanguage::numberRealTimeTicks, dwDisrupted))
					));
		}

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
		DWORD dwNow = (DWORD)GetUniverse().GetTicks();
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
		DWORD dwNow = (DWORD)GetUniverse().GetTicks();
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

CString CItem::GetEnhancedDesc (void) const

//	GetEnhancedDesc
//
//	Returns a string that describes the kind of enhancement
//	for this item.

	{
	TArray<SDisplayAttribute> Attribs;
	if (!AccumulateEnhancementDisplayAttributes(Attribs))
		return NULL_STR;

	CString sResult = Attribs[0].sText;
	for (int i = 1; i < Attribs.GetCount(); i++)
		{
		sResult.Append(CONSTLIT(" "));
		sResult.Append(Attribs[i].sText);
		}

	return sResult;
	}

bool CItem::GetEnhancementConferred (const CSpaceObject &TargetObj, const CItem &TargetItem, const CString &sMode, SEnhanceItemResult &retResult, CString *retsError) const

//	GetEnhancementConferred
//
//	Returns the enhancement conferred by this item on the TargetItem (if used).

	{
	//	Pre-init and check for null

	retResult = SEnhanceItemResult();
	retResult.iResult = eisNoEffect;
	if (TargetItem.IsEmpty())
		return true;

	//	If we have a mod code, then use that.

	if (DWORD dwModCode = TargetItem.GetType()->GetModCode())
		{
		retResult.Enhancement = CItemEnhancement(dwModCode);
		retResult.Enhancement.SetEnhancementType(GetType());
		return true;
		}

	//	Otherwise, see if we have properties.

	CCodeChainCtx Ctx(GetUniverse());
	CItemCtx ItemCtx(*this);
	ICCItemPtr pEnhancementDesc(GetItemProperty(Ctx, ItemCtx, PROPERTY_CORE_ENHANCEMENT, false));
	if (pEnhancementDesc && !pEnhancementDesc->IsNil())
		{
		CString sError;
		if (retResult.Enhancement.InitFromDesc(GetUniverse(), *pEnhancementDesc, &sError) != NOERROR)
			{
			if (retsError)
				*retsError = strPatternSubst(CONSTLIT("[%08x %s] core.enhancement is invalid: %s"), GetType()->GetUNID(), GetNounPhrase(), sError);

			return false;
			}

		retResult.Enhancement.SetEnhancementType(GetType());
		return true;
		}

	//	If we still have nothing, call GetEnhancement event.

	SEventHandlerDesc Event;
	if (m_pItemType->FindEventHandler(GET_ENHANCEMENT_EVENT, &Event))
		{
		Ctx.DefineContainingType(m_pItemType);
		Ctx.SaveAndDefineSourceVar(&TargetObj);
		Ctx.SaveAndDefineItemVar(*this);

		ICCItemPtr pTargetItem(::CreateListFromItem(TargetItem));
		ICCItemPtr pData(ICCItem::SymbolTable);
		pData->SetAt(CONSTLIT("targetItem"), pTargetItem);
		pData->SetStringAt(CONSTLIT("mode"), sMode);

		Ctx.SaveAndDefineDataVar(pData);

		ICCItemPtr pResult = Ctx.RunCode(Event);
		if (pResult->IsError())
			{
			if (retsError)
				*retsError = strPatternSubst(CONSTLIT("[%08x %s] <GetEnhancement> error: %s"), GetType()->GetUNID(), GetNounPhrase(), pResult->GetStringValue());

			return false;
			}

		CString sError;
		if (retResult.Enhancement.InitFromDesc(GetUniverse(), *pResult, &sError) != NOERROR)
			{
			if (retsError)
				*retsError = strPatternSubst(CONSTLIT("[%08x %s] <GetEnhancement> returned invalid enhancement: %s"), GetType()->GetUNID(), GetNounPhrase(), sError);

			return false;
			}

		if (retResult.Enhancement.GetEnhancementType() == NULL)
			retResult.Enhancement.SetEnhancementType(GetType());

		retResult.sDesc = pResult->GetStringAt(CONSTLIT("desc"));
		retResult.sNextScreen = pResult->GetStringAt(CONSTLIT("nextScreen"));
		retResult.bDoNotConsume = pResult->GetBooleanAt(CONSTLIT("doNotConsume"));

		//	Done

		return true;
		}

	//	No enhancement (but no error either).

	return true;
	}

TSharedPtr<CItemEnhancementStack> CItem::GetEnhancementStack (void) const

//	GetEnhancementStack
//
//	Returns an allocated enhancement stack (or NULL).

	{
	//	If we have installed armor, then get the enhancement stack from it.

	if (const CInstalledArmor *pArmor = GetInstalledArmor())
		return pArmor->GetEnhancementStack();

	//	If we have an installed device, then get the enhancement stack from it.

	else if (const CInstalledDevice *pDevice = GetInstalledDevice())
		return pDevice->GetEnhancementStack();

	//	Otherwise, we need to create one from mods

	const CItemEnhancement &Mods = GetMods();
	if (Mods.IsEmpty())
		return NULL;

	TSharedPtr<CItemEnhancementStack> pResult(new CItemEnhancementStack);
	pResult->Insert(Mods);
	return pResult;
	}

int CItem::GetApparentLevel (void) const

//	GetApparentLevel
//
//	Returns the apparent level (considering unknown state).

	{
	if (CItemType *pUnknownType = GetUnknownTypeIfUnknown())
		return pUnknownType->GetLevel();

	return GetLevel();
	}

int CItem::GetLevel (void) const

//	GetLevel
//
//	Returns the current level.
//
//	NOTE: We always return the actual level. Unlike methods like GetDesc, this
//	is often called for mechanic reasons (not just appearance). Use 
//	GetApparentLevel for display purposes.

	{
	if (IsEmpty())
		return 0;

	else if (GetType()->IsScalable())
		return Max(GetType()->GetMinLevel(), Min(GetType()->GetMinLevel() + GetScalableLevel(), GetType()->GetMaxLevel()));

	else
		return GetType()->GetMinLevel();
	}

int CItem::GetMassKg (void) const

//	GetMass
//
//	Returns the mass of a single unit of the item type.

	{
	CItemCtx ItemCtx(*this);
	return m_pItemType->GetMassKg(ItemCtx);
	}

int CItem::GetMaxCharges (void) const

//	GetMaxCharges
//
//	Returns the maximum charges for the item.

	{
	return (m_pItemType ? m_pItemType->GetMaxCharges() : 0);
	}

CString CItem::GetNamePattern (DWORD dwNounFormFlags, DWORD *retdwFlags) const

//	GetNamePattern
//
//	Returns the noun pattern

	{
	if (m_pItemType == NULL)
		return NULL_STR;

	bool bActualName = (dwNounFormFlags & nounActual) != 0;
	int iUnknownIndex;
	if (!IsKnown(&iUnknownIndex) && !bActualName)
		{
		const CString &sUnknownName = m_pItemType->GetUnknownName(iUnknownIndex);
		if (!sUnknownName.IsBlank())
			{
			if (retdwFlags)
				*retdwFlags = 0;

			return sUnknownName;
			}
		else
			return m_pItemType->GetUnknownType(iUnknownIndex)->GetNamePattern(dwNounFormFlags, retdwFlags);
		}

	return m_pItemType->GetNamePattern(dwNounFormFlags, retdwFlags);
	}

CString CItem::GetNounPhrase (DWORD dwFlags) const

//	GetNounPhrase
//
//	Returns a noun phrase that represents this item

	{
	CString sName;
	CString sModifier;
	DWORD dwNounFlags = 0;

	//	If we have code, call it to generate the name

	SEventHandlerDesc Event;
	if (m_pItemType->FindEventHandlerItemType(CItemType::evtGetHUDName, &Event)
			&& !(dwFlags & nounNoEvent) && (dwFlags & nounHUDName))
		{
		CCodeChainCtx Ctx(GetUniverse());

		Ctx.SetEvent(eventGetName);
		Ctx.SetItemType(GetType());
		Ctx.DefineContainingType(m_pItemType);
		Ctx.SaveAndDefineSourceVar(NULL);
		Ctx.SaveAndDefineItemVar(*this);
		Ctx.DefineVar(CONSTLIT("aFlags"), CLanguage::GetNounFlags(dwFlags));

		ICCItemPtr pResult = Ctx.RunCode(Event);
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
		}
	else if (m_pItemType->FindEventHandlerItemType(CItemType::evtGetName, &Event)
			 && !(dwFlags & nounNoEvent))
		{
		CCodeChainCtx Ctx(GetUniverse());

		Ctx.SetEvent(eventGetName);
		Ctx.SetItemType(GetType());
		Ctx.DefineContainingType(m_pItemType);
		Ctx.SaveAndDefineSourceVar(NULL);
		Ctx.SaveAndDefineItemVar(*this);
		Ctx.DefineVar(CONSTLIT("aFlags"), CLanguage::GetNounFlags(dwFlags));

		ICCItemPtr pResult = Ctx.RunCode(Event);
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
		}

	//	Otherwise, get it from the item type

	else
		{
		sName = GetNamePattern(dwFlags, &dwNounFlags);

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
			const CInstalledDevice *pDevice;
			if ((pDevice = GetInstalledDevice())
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

int CItem::GetInstallCost (void) const

//	GetInstallCost
//
//	Returns the install cost in default currency (or -1 if the item cannot be
//	installed).

	{
	if (m_pItemType == NULL)
		return -1;

	else if (const CArmorItem ArmorItem = AsArmorItem())
		return ArmorItem.GetInstallCost();

	else if (CDeviceClass *pDevice = m_pItemType->GetDeviceClass())
		{
		CItemCtx Ctx(*this);
		return pDevice->GetInstallCost(Ctx);
		}
	else
		return -1;
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

	if (!IsKnown() && !(dwFlags & CItemType::FLAG_ACTUAL_ITEM))
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
			CCodeChainCtx Ctx(GetUniverse());

			Ctx.SetEvent(eventGetReferenceText);
			Ctx.SetItemType(GetType());
			Ctx.DefineContainingType(m_pItemType);
			Ctx.SaveAndDefineSourceVar(ItemCtx.GetSource());
			Ctx.SaveAndDefineItemVar(*this);

			ICCItemPtr pResult = Ctx.RunCode(Event);
			sReference = pResult->GetStringValue();
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
	CDeviceClass *pDevice;

	//	No reference if unknown

	if (!IsKnown() && !(dwFlags & CItemType::FLAG_ACTUAL_ITEM))
		return false;

	//	Return armor reference, if this is armor

	else if (const CArmorItem ArmorItem = AsArmorItem())
		return ArmorItem.GetReferenceDamageAdj(retiHP, retArray);

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

	if (!IsKnown() && !(dwFlags & CItemType::FLAG_ACTUAL_ITEM))
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

	if (!IsKnown() && !(dwFlags & CItemType::FLAG_ACTUAL_ITEM))
		return false;

	//	If this is armor, ask the armor

	else if (pArmor = m_pItemType->GetArmorClass())
		return pArmor->GetReferenceSpeedBonus(Ctx, retiSpeedBonus);

	//	Otherwise, nothing

	else
		return false;
	}

int CItem::GetRepairLevel () const

//	GetRepairLevel
//
//	Returns the repair level.

	{
	if (const CArmorItem ArmorItem = AsArmorItem())
		return ArmorItem.GetRepairLevel();
	else
		return GetLevel();
	}

CSpaceObject *CItem::GetSource (void) const

//	GetSource
//
//	Returns the object on which this item is installed (or NULL).

	{
	if (m_pExtra == NULL)
		return NULL;

	switch (m_pExtra->m_iInstalled)
		{
		case EInstalled::Armor:
			{
			const CInstalledArmor *pArmor = GetInstalledArmor();
			return pArmor->GetSource();
			}

		case EInstalled::Device:
			{
			const CInstalledDevice *pDevice = GetInstalledDevice();
			return pDevice->GetSource();
			}

		default:
			return NULL;
		}
	}

int CItem::GetTradePrice (const CSpaceObject *pObj, bool bActual) const

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
		CCodeChainCtx Ctx(GetUniverse());

		Ctx.SetEvent(eventGetTradePrice);
		Ctx.SetItemType(GetType());
		Ctx.DefineContainingType(m_pItemType);
		Ctx.SaveAndDefineSourceVar(pObj);
		Ctx.SaveAndDefineItemVar(*this);
		Ctx.DefineString(CONSTLIT("aPriceType"), (bActual ? CONSTLIT("actual") : CONSTLIT("normal")));

		ICCItemPtr pResult = Ctx.RunCode(Event);
		if (pResult->IsError() && pObj)
			pObj->ReportEventError(strPatternSubst(CONSTLIT("Item %x GetTradePrice"), m_pItemType->GetUNID()), pResult);
		int iPrice = pResult->GetIntegerValue();
		return iPrice;
		}
	else
		return GetValue(bActual);
	}

CUniverse &CItem::GetUniverse (void) const

//	GetUniverse
//
//	Returns the universe object.

	{
	return (m_pItemType ? m_pItemType->GetUniverse() : *g_pUniverse);
	}

int CItem::GetUnknownIndex (void) const

//	GetUnknownIndex
//
//	Returns the index of the unknown item type (or -1).

	{
	if (m_pItemType == NULL)
		return -1;

	//	If not unknown, then we're done.

	int iUnknownCount = m_pItemType->GetUnknownTypeCount();
	if (iUnknownCount == 0)
		return -1;

	int iIndex = (int)((m_dwFlags & flagUnknownBit3) | ((m_dwFlags & UNKNOWN_INDEX_LOWER_MASK) >> UNKNOWN_INDEX_LOWER_SHIFT));
	return Max(0, Min(iIndex, iUnknownCount - 1));
	}
	
CItemType *CItem::GetUnknownType (void) const

//	GetUnknownType
//
//	Returns the unknown type (or NULL).

	{
	int iIndex = GetUnknownIndex();
	if (iIndex < 0)
		return NULL;

	return m_pItemType->GetUnknownType(iIndex);
	}

CItemType *CItem::GetUnknownTypeIfUnknown (bool bActual) const

//	GetUnknownTypeIfUnknown
//
//	Returns the unknown type.

	{
	//	NOTE: We add this so that we can do expressions like this:
	//
	//	if (CItemType *pUnknownType = GetUnknownTypeIfUnknown(bActual))
	//		...

	if (bActual)
		return NULL;

	int iIndex = GetUnknownIndex();
	if (iIndex < 0)
		return NULL;
	else if (m_pItemType->IsKnown(iIndex))
		return NULL;
	else
		return m_pItemType->GetUnknownType(iIndex);
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
	if (strStartsWith(sAttrib, SPECIAL_PROPERTY))
		{
		CString sProperty = strSubString(sAttrib, SPECIAL_PROPERTY.GetLength());

		CString sError;
		CPropertyCompare Compare;
		CCodeChainCtx CCX(GetUniverse());
		if (!Compare.Parse(CCX, sProperty, &sError))
			{
			::kernelDebugLogPattern("ERROR: Unable to parse property expression: %s", sError);
			return false;
			}

		CItemCtx ItemCtx(*this);
		ICCItemPtr pValue = ICCItemPtr(GetItemProperty(CCX, ItemCtx, Compare.GetProperty(), false));
		return Compare.Eval(pValue);
		}
	else
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

bool CItem::IsDamaged (int *retiDamagedHP) const

//	IsDamaged
//
//	Returns TRUE if the item is damaged (optionally returns the hit points of 
//	damage).

	{
	if (retiDamagedHP)
		{
		if (m_pExtra)
			*retiDamagedHP = m_pExtra->m_iDamagedHP;
		else
			*retiDamagedHP = 0;
		}

	return (m_dwFlags & flagDamaged ? true : false);
	}

bool CItem::IsDisruptionEqual (DWORD dwNow, DWORD dwD1, DWORD dwD2)

//	IsDisruptionEqual
//
//	Returns TRUE if the two disruption time values are effectively equal.

	{
	if (dwD1 == dwD2)
		return true;

	return (dwD1 < dwNow && dwD2 < dwNow);
	}

bool CItem::IsEnhancementEffective (const CItemEnhancement &Enhancement) const

//	IsEnhancementEffective
//
//	Returns TRUE if the given enhancement would actually affect the item. For
//	example, radiation immunity does not work on armor that is already radiation
//	immune.
//
//	NOTE: We only consider the intrinsic properties of this item, not any 
//	enhancements conferred by the ship or other items.

	{
	if (IsArmor())
		{
		CItem BasicItem(GetType(), 1);
		CArmorItem ArmorItem = BasicItem.AsArmorItemOrThrow();

		switch (Enhancement.GetType())
			{
			case etSpecialDamage:
				if (Enhancement.IsDisadvantage())
					return ArmorItem.IsImmune(Enhancement.GetSpecialDamage());
				else
					return !ArmorItem.IsImmune(Enhancement.GetSpecialDamage());

			default:
				return true;
			}
		}
	else if (IsDevice())
		{
		CItem BasicItem(GetType(), 1);
		const CDeviceItem DeviceItem = BasicItem.AsDeviceItemOrThrow();

		switch (Enhancement.GetType())
			{
			case etOmnidirectional:
				if (Enhancement.IsDisadvantage())
					return (DeviceItem.GetFireArc() > 0);
				else
					return (DeviceItem.GetFireArc() < Enhancement.GetFireArc());

			default:
				return true;
			}
		}
	else
		return false;
	}

bool CItem::IsEqual (const CItem &Item, DWORD dwFlags) const

//	IsEqual
//
//	Returns TRUE if this item is the same as the given
//	item except for the count

	{
	return (m_pItemType == Item.m_pItemType
			&& IsFlagsEqual(Item, dwFlags)
			&& IsExtraEqual(Item.m_pExtra, dwFlags));
	}

bool CItem::IsExtraEmpty (const SExtra *pExtra, DWORD dwFlags, DWORD dwNow)

//	IsExtraEmpty
//
//	Returns TRUE is the given pExtra struct is empty
//	(set to initial values)

	{
	const bool bIgnoreCharges = (dwFlags & FLAG_IGNORE_CHARGES ? true : false);
	const bool bIgnoreData = (dwFlags & FLAG_IGNORE_DATA ? true : false);
	const bool bIgnoreDisrupted = (dwFlags & FLAG_IGNORE_DISRUPTED ? true : false);
	const bool bIgnoreEnhancements = (dwFlags & FLAG_IGNORE_ENHANCEMENTS ? true : false);
	const bool bIgnoreInstalled = (dwFlags & FLAG_IGNORE_INSTALLED ? true : false);

	return ((bIgnoreCharges || pExtra->m_dwCharges == 0)
			&& pExtra->m_dwLevel == 0
			&& pExtra->m_dwVariantCounter == 0
			&& pExtra->m_iDamagedHP == 0
			&& (bIgnoreInstalled || pExtra->m_iInstalledIndex == -1)
			&& (bIgnoreDisrupted || (pExtra->m_dwDisruptedTime == 0 || pExtra->m_dwDisruptedTime < dwNow))
			&& (bIgnoreEnhancements || pExtra->m_Mods.IsEmpty())
			&& (bIgnoreData || pExtra->m_Data.IsEmpty()));
	}

bool CItem::IsExtraEmpty (DWORD dwFlags)
	{
	return !m_pExtra || IsExtraEmpty(m_pExtra, dwFlags, GetUniverse().GetTicks());
	}

bool CItem::IsExtraEqual (SExtra *pSrc, DWORD dwFlags) const

//	IsExtraEqual
//
//	Returns TRUE if this item's Extra struct is the same as the source

	{
	if (IsEmpty())
		return (pSrc == NULL || IsExtraEmpty(pSrc, dwFlags, GetUniverse().GetTicks()));

	//	Both have extra struct

	else if (m_pExtra && pSrc)
		{
		const bool bIgnoreCharges = (dwFlags & FLAG_IGNORE_CHARGES ? true : false);
		const bool bIgnoreData = (dwFlags & FLAG_IGNORE_DATA ? true : false);
		const bool bIgnoreDisrupted = (dwFlags & FLAG_IGNORE_DISRUPTED ? true : false);
		const bool bIgnoreEnhancements = (dwFlags & FLAG_IGNORE_ENHANCEMENTS ? true : false);
		const bool bIgnoreInstalled = (dwFlags & FLAG_IGNORE_INSTALLED ? true : false);

		return ((bIgnoreCharges || m_pExtra->m_dwCharges == pSrc->m_dwCharges)
				&& m_pExtra->m_dwLevel == pSrc->m_dwLevel
				&& m_pExtra->m_dwVariantCounter == pSrc->m_dwVariantCounter
				&& m_pExtra->m_iDamagedHP == pSrc->m_iDamagedHP
				&& (bIgnoreInstalled || m_pExtra->m_iInstalledIndex == pSrc->m_iInstalledIndex)
				&& (bIgnoreDisrupted || IsDisruptionEqual(GetUniverse().GetTicks(), m_pExtra->m_dwDisruptedTime, pSrc->m_dwDisruptedTime))
				&& (bIgnoreEnhancements || m_pExtra->m_Mods.IsEqual(pSrc->m_Mods))
				&& (bIgnoreData || m_pExtra->m_Data.IsEqual(pSrc->m_Data)));
		}

	//	Neither has extra struct

	else if (m_pExtra == pSrc)
		return true;

	//	One has extra struct, but it's empty

	else if (m_pExtra == NULL)
		return IsExtraEmpty(pSrc, dwFlags, GetUniverse().GetTicks());
	else if (pSrc == NULL)
		return IsExtraEmpty(m_pExtra, dwFlags, GetUniverse().GetTicks());

	//	Extra structs don't match at all

	else
		return false;
	}

bool CItem::IsFlagsEqual (const CItem &Src, DWORD dwFlags) const

//	IsFlagsEqual
//
//	Returns TRUE if the flags setting for the two items are equal.

	{
	//	Short-circuit the easy case.

	if (Src.m_dwFlags == m_dwFlags)
		return true;

	//	If enhancements are different, then we're not equal

	const bool bIgnoreEnhancements = (dwFlags & FLAG_IGNORE_ENHANCEMENTS ? true : false);
	if (!bIgnoreEnhancements 
			&& (m_dwFlags & flagEnhanced) != (Src.m_dwFlags & flagEnhanced))
		return false;

	//	Check the damage flag

	if ((m_dwFlags & flagDamaged) != (Src.m_dwFlags & flagDamaged))
		return false;

	//	If we get this far, the difference must be in the unknown type index.
	//	In that case, we're equal only if both items are known to the player.

	return (IsKnown() && Src.IsKnown());
	}

bool CItem::IsKnown (int *retiUnknownIndex) const

//	IsKnown
//
//	Returns TRUE if the item is known to the player.

	{
	int iIndex = GetUnknownIndex();
	if (iIndex < 0)
		return true;

	if (retiUnknownIndex)
		*retiUnknownIndex = iIndex;

	return m_pItemType->IsKnown(iIndex);
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
	if (m_pItemType == NULL)
		return false;

	//	If we've got a filter, then use that

	else if (ICCItem *pFilter = Criteria.GetFilter())
		{
		CCodeChainCtx CCCtx(GetUniverse());

		//	Create a list representing this item

		ICCItemPtr pItem(::CreateListFromItem(*this));

		//	Create an argument list consisting of the item

		ICCItemPtr pArgs = CCCtx.Create(ICCItem::List);
		pArgs->Append(pItem);

		//	Apply the function to the arg list

		ICCItemPtr pResult = CCCtx.RunLambdaCode(pFilter, pArgs);
		bool bMatches = !pResult->IsNil();

		return bMatches;
		}

	//	If we've got an OR expression and it matches, then we're done.

	else if (Criteria.HasORExpression() && MatchesCriteria(Criteria.GetORExpression()))
		return true;

	//	If we've got a lookup, then recurse

	else if (!Criteria.GetLookup().IsBlank())
		{
		const CItemCriteria *pCriteria = GetUniverse().GetDesignCollection().GetDisplayAttributes().FindCriteriaByID(Criteria.GetLookup());
		if (pCriteria == NULL)
			return false;

		return MatchesCriteria(*pCriteria);
		}

	//	Otherwise, process criteria

	else
		{
		//	Match item categories

		if (!Criteria.MatchesItemCategory(*m_pItemType))
			return false;

		//	Match item flags

		if (!Criteria.MatchesFlags(m_dwFlags))
			return false;

		//	Check installed flags

		if (Criteria.MatchesInstalledOnly() && !IsInstalled())
			return false;

		if (Criteria.MatchesNotInstalledOnly() && IsInstalled())
			return false;

		//	Check miscellaneous flags

		CItemType::SUseDesc UseDesc;
		if (Criteria.MatchesUsableOnly() && (!m_pItemType->GetUseDesc(&UseDesc) || UseDesc.pScreenRoot == NULL))
			return false;

		if (Criteria.ExcludesVirtual() && m_pItemType->IsVirtual())
			return false;

		if (Criteria.MatchesLauncherMissilesOnly() && m_pItemType->IsAmmunition())
			return false;

		//	Check required modifiers

		if (!Criteria.MatchesAttributes(*this))
			return false;

		//	Check frequency range

		if (!Criteria.MatchesFrequency((FrequencyTypes)m_pItemType->GetFrequency()))
			return false;

		//	Check for level modifiers

		if (!Criteria.MatchesLevel(GetLevel()))
			return false;

		//	Check for price modifiers

		if (!Criteria.MatchesPrice(GetValue()))
			return false;

		//	Check for mass modifiers

		if (!Criteria.MatchesMass(GetMassKg()))
			return false;

		//	Check for repair level

		if (!Criteria.MatchesRepairLevel(GetRepairLevel()))
			return false;

		//	Otherwise, we match completely

		return true;
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
		dwFlags |= (pItem->GetBooleanAt(CONSTLIT("knownOnly")) ? CItem::FLAG_KNOWN_ONLY : 0);
		}

	return dwFlags;
	}

void CItem::ReadFromCCItem (CDesignCollection &Design, const CSystem *pSystem, const ICCItem *pBuffer)

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
		m_pItemType = CItemType::AsType(Design.FindEntry(dwUNID));
		if (m_pItemType == NULL)
			return;

		//	Next is the count, flags, and installed

		DWORD dwSavedData = (DWORD)pBuffer->GetElement(1)->GetIntegerValue();
		DWORD *pDest = (DWORD *)this;
		pDest[1] = dwSavedData;

		//	Previous version

		if (pBuffer->GetCount() == 3)
			{
			DWORD dwLoad = (DWORD)pBuffer->GetElement(2)->GetIntegerValue();
			if (dwLoad)
				{
				Extra();
				m_pExtra->m_dwCharges = LOBYTE(LOWORD(dwLoad));
				m_pExtra->m_dwLevel = 0;
				m_pExtra->m_dwDisruptedTime = 0;
				m_pExtra->m_Mods = CItemEnhancement(HIWORD(dwLoad));
				m_pExtra->m_iDamagedHP = 0;
				}
			}

		//	Otherwise

		else if (pBuffer->GetCount() > 3)
			{
			int iStart = 2;
			CString sData;

			Extra();

			//	Load the version, if we have it

			DWORD dwVersion = 46;
			if (pBuffer->GetCount() >= 7)
				{
				dwVersion = (DWORD)pBuffer->GetElement(iStart)->GetIntegerValue();
				iStart++;
				}

			//	Installation

			if (dwVersion >= 174)
				{
				DWORD dwObjID = pBuffer->GetElement(iStart)->GetIntegerValue();
				iStart++;

				if (dwObjID > 0)
					{
					int iInstalledIndex;

					if (dwVersion >= 201)
						{
						iInstalledIndex = pBuffer->GetElement(iStart)->GetIntegerValue();
						iStart++;
						}
					else
						iInstalledIndex = (int)(char)(BYTE)((dwSavedData & 0xff000000) >> 24);

					CSpaceObject *pSource;
					if (pSystem && iInstalledIndex != -1 && (pSource = pSystem->FindObject(dwObjID)))
						{
						CArmorSystem *pArmorSys;
						if (IsArmor() 
								&& (pArmorSys = &pSource->GetArmorSystem())
								&& iInstalledIndex >= 0 && iInstalledIndex < pArmorSys->GetSegmentCount())
							{
							m_pExtra->m_iInstalled = EInstalled::Armor;
							m_pExtra->m_pInstalled = &pArmorSys->GetSegment(iInstalledIndex);
							m_pExtra->m_iInstalledIndex = iInstalledIndex;
							}

						else if (IsDevice()
								&& iInstalledIndex >= 0 && iInstalledIndex < pSource->GetDeviceCount())
							{
							m_pExtra->m_iInstalled = EInstalled::Device;
							m_pExtra->m_pInstalled = pSource->GetDevice(iInstalledIndex);
							m_pExtra->m_iInstalledIndex = iInstalledIndex;
							}
						}
					}
				}

			//	Charges

			m_pExtra->m_dwCharges = (DWORD)pBuffer->GetElement(iStart)->GetIntegerValue();

			//	Level

			if (dwVersion >= 170)
				m_pExtra->m_dwLevel = (DWORD)pBuffer->GetElement(iStart+1)->GetIntegerValue();
			else
				{
				DWORD dwVariant = (DWORD)pBuffer->GetElement(iStart+1)->GetIntegerValue();
				m_pExtra->m_dwLevel = HIWORD(dwVariant);
				}

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

			//	Variant number

			if (pBuffer->GetCount() >= 9)
				m_pExtra->m_dwVariantCounter = (DWORD)pBuffer->GetElement(iStart+5)->GetIntegerValue();

			//	Damaged HP

			if (pBuffer->GetCount() >= 10)
				m_pExtra->m_iDamagedHP = pBuffer->GetElement(iStart+6)->GetIntegerValue();
			}
		}
	}

void CItem::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Reads from a stream

	{
	DWORD dwLoad;
	Ctx.pStream->Read(dwLoad);
	m_pItemType = Ctx.GetUniverse().FindItemType(dwLoad);

	Ctx.pStream->Read(dwLoad);
	m_dwCount = LOWORD(dwLoad);
	m_dwFlags = LOBYTE(HIWORD(dwLoad));

	BYTE byOldInstalled = HIBYTE(HIWORD(dwLoad));

	//	Load SExtra struct

	if (Ctx.dwVersion >= 23)
		{
		Ctx.pStream->Read(dwLoad);
		if (dwLoad)
			{
			m_pExtra = new SExtra;

			//	NOTE: We don't need to restore the source or install struct 
			//	because it doesn't exist yet. It will get fixed up later. But
			//	we do need to load the install index because that's what we use
			//	to fix up the connection.

			if (Ctx.dwVersion >= 201)
				Ctx.pStream->Read(m_pExtra->m_iInstalledIndex);
			else
				m_pExtra->m_iInstalledIndex = (int)(char)byOldInstalled;

			if (Ctx.dwVersion >= 71)
				{
				Ctx.pStream->Read(m_pExtra->m_dwCharges);

				if (Ctx.dwVersion >= 170)
					Ctx.pStream->Read(m_pExtra->m_dwLevel);
				else
					{
					DWORD dwVariant;
					Ctx.pStream->Read(dwVariant);
					m_pExtra->m_dwLevel = HIWORD(dwVariant);
					}
				}
			else
				{
				Ctx.pStream->Read(dwLoad);
				m_pExtra->m_dwCharges = LOWORD(dwLoad);
				m_pExtra->m_dwLevel = 0;
				}

			if (Ctx.dwVersion >= 58)
				Ctx.pStream->Read(m_pExtra->m_dwDisruptedTime);
			else
				m_pExtra->m_dwDisruptedTime = 0;

			m_pExtra->m_Mods.ReadFromStream(Ctx);
			m_pExtra->m_Data.ReadFromStream(Ctx);

			if (Ctx.dwVersion >= 169)
				Ctx.pStream->Read(m_pExtra->m_dwVariantCounter);
			else
				m_pExtra->m_dwVariantCounter = 0;

			if (Ctx.dwVersion >= 173)
				Ctx.pStream->Read(m_pExtra->m_iDamagedHP);
			else
				m_pExtra->m_iDamagedHP = 0;
			}
		else
			m_pExtra = NULL;
		}
	else
		{
		Ctx.pStream->Read(dwLoad);
		DWORD dwCharges = LOBYTE(LOWORD(dwLoad));
		DWORD dwCondition = HIBYTE(LOWORD(dwLoad));
		DWORD dwMods = HIWORD(dwLoad);

		if (dwCharges || dwCondition || dwMods)
			{
			m_pExtra = new SExtra;
			m_pExtra->m_dwCharges = dwCharges;
			m_pExtra->m_dwLevel = 0;
			m_pExtra->m_dwDisruptedTime = 0;
			m_pExtra->m_dwVariantCounter = 0;
			m_pExtra->m_iDamagedHP = 0;
			}
		else
			m_pExtra = NULL;
		}

	//	Handle backwards compatibility.

	if (Ctx.dwVersion < 201 && !m_pExtra && byOldInstalled != 0xff)
		{
		m_pExtra = new SExtra;
		m_pExtra->m_iInstalledIndex = (int)(char)byOldInstalled;
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

void CItem::SetDamaged (int iDamagedHP)

//	SetDamaged
//
//	Sets damaged hit points.

	{
	if (iDamagedHP == 0)
		ClearDamaged();
	else
		{
		SetDamaged();
		Extra();
		m_pExtra->m_iDamagedHP = Max(0, iDamagedHP);
		}
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

		DWORD dwNow = (DWORD)GetUniverse().GetTicks();
		if (m_pExtra->m_dwDisruptedTime <= dwNow)
			m_pExtra->m_dwDisruptedTime = dwNow + dwDuration;
		else
			m_pExtra->m_dwDisruptedTime += dwDuration;
		}
	}

void CItem::SetInstalled (CInstalledArmor &Installed)

//	SetInstalled
//
//	Binds to an installed armor object.

	{
	ASSERT(GetCount() == 1);
	ASSERT(m_pItemType && m_pItemType->GetArmorClass() == Installed.GetClass());

	Extra();
	m_pExtra->m_pInstalled = &Installed;
	m_pExtra->m_iInstalled = EInstalled::Armor;
	m_pExtra->m_iInstalledIndex = Installed.GetSect();
	}

void CItem::SetInstalled (CInstalledDevice &Installed)

//	SetInstalled
//
//	Binds to an installed device object.

	{
	ASSERT(GetCount() == 1);
	ASSERT(m_pItemType && m_pItemType->GetDeviceClass() == Installed.GetClass());

	Extra();
	m_pExtra->m_pInstalled = &Installed;
	m_pExtra->m_iInstalled = EInstalled::Device;
	m_pExtra->m_iInstalledIndex = Installed.GetDeviceSlot();
	}

void CItem::SetKnown (bool bKnown) const

//	SetKnown
//
//	Sets the known state of the item.

	{
	int iIndex = GetUnknownIndex();
	if (iIndex < 0)
		return;

	m_pItemType->SetKnown(iIndex, bKnown);
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

	if (iLevel < GetType()->GetMinLevel() || iLevel > GetType()->GetMaxLevel())
		{
		if (retsError)
			*retsError = strPatternSubst(CONSTLIT("Item [%08x] cannot be scaled to level %d."), GetType()->GetUNID(), iLevel);
		return false;
		}

	SetScalableLevel(iLevel - GetType()->GetMinLevel());
	return true;
	}

void CItem::SetPrepareUninstalled (void)

//	SetPrepareUninstalled
//
//	Clear installed connection.

	{
	if (m_pExtra)
		{
		m_pExtra->m_pInstalled = NULL;
		m_pExtra->m_iInstalled = EInstalled::None;
		m_pExtra->m_iInstalledIndex = -2;
		}
	}

void CItem::SetUnknownIndex (int iIndex)

//	SetUnknownIndex
//
//	Sets the unknown index.

	{
	DWORD dwIndex = Min(iIndex, UNKNOWN_INDEX_MAX);
	m_dwFlags &= ~UNKNOWN_INDEX_MASK;
	m_dwFlags |= (dwIndex & flagUnknownBit3) | ((dwIndex & ~flagUnknownBit3) << UNKNOWN_INDEX_LOWER_SHIFT);
	}

void CItem::SetVariantNumber(int iVariantCounter)

//	SetVariantNumber
//
//	Sets the current variant counter

	{
	Extra();
	m_pExtra->m_dwVariantCounter = iVariantCounter;
	}

ICCItem *CItem::WriteToCCItem (void) const

//	WriteToCCItem
//
//	Converts item to a ICCItem

	{
	ICCItemPtr pList = ICCItemPtr(ICCItem::List);

	//	Next integer is the item UNID

	pList->AppendInteger(GetType()->GetUNID());

	//	Next is the count, flags, and installed

	DWORD *pSource = (DWORD *)this;
	pList->AppendInteger(pSource[1]);

	//	Save extra

	if (m_pExtra)
		{
		//	Save the version (starting in v45)

		pList->AppendInteger(CSystem::GetSaveVersion());

		//	Save any installation info

		if (const CInstalledArmor *pArmor = GetInstalledArmor())
			{
			pList->AppendInteger(pArmor->GetSource()->GetID());
			pList->AppendInteger(GetInstalled());
			}
		else if (const CInstalledDevice *pDevice = GetInstalledDevice())
			{
			pList->AppendInteger(pDevice->GetSource()->GetID());
			pList->AppendInteger(GetInstalled());
			}
		else
			pList->AppendInteger(0);

		//	Charges and level

		pList->AppendInteger(m_pExtra->m_dwCharges);
		pList->AppendInteger(m_pExtra->m_dwLevel);

		//	Mods

		CMemoryWriteStream Stream;
		Stream.Create();
		m_pExtra->m_Mods.WriteToStream(&Stream);
		Stream.Close();

		pList->AppendString(CString(Stream.GetPointer(), Stream.GetLength()));

		//	Attribute data block

		Stream.Create();
		m_pExtra->m_Data.WriteToStream(&Stream);
		Stream.Close();

		pList->AppendString(CString(Stream.GetPointer(), Stream.GetLength()));

		//	More

		pList->AppendInteger(m_pExtra->m_dwDisruptedTime);
		pList->AppendInteger(m_pExtra->m_dwVariantCounter);
		pList->AppendInteger(m_pExtra->m_iDamagedHP);
		}

	return pList->Reference();
	}

void CItem::WriteToStream (IWriteStream *pStream) const

//	WriteToStream
//
//	Writes to a stream
//
//	DWORD		item type UNID
//	DWORD		low = m_dwCount; high = (low = m_dwFlags; high = unused)
//	DWORD		non-zero if SExtra follows
//
//	SExtra
//	DWORD		m_iInstalledIndex
//	DWORD		m_dwCharges
//	DWORD		m_dwCondition
//	DWORD		m_dwDisruptedTime
//	CItemEnhancement
//	CAttributeDataBlock
//	DWORD		m_dwVariantCounter
//	DWORD		m_iDamagedHP

	{
	DWORD dwSave = m_pItemType->GetUNID();
	pStream->Write(dwSave);

	dwSave = MAKELONG(m_dwCount, MAKEWORD(m_dwFlags, 0));
	pStream->Write(dwSave);

	//	Save SExtra

	dwSave = (m_pExtra ? 0xffffffff : 0);
	pStream->Write(dwSave);
	if (m_pExtra)
		{
		//	NOTE: We don't need to save the source or install struct because it
		//	it gets fixed up after load. We do need to save the install index 
		//	because that's what we use to fix up the connection.

		pStream->Write(m_pExtra->m_iInstalledIndex);
		pStream->Write(m_pExtra->m_dwCharges);
		pStream->Write(m_pExtra->m_dwLevel);
		pStream->Write(m_pExtra->m_dwDisruptedTime);

		m_pExtra->m_Mods.WriteToStream(pStream);

		//	Save TLisp data.
		//	Note: Currently does not support saving object references

		m_pExtra->m_Data.WriteToStream(pStream);

		//	More variables

		pStream->Write(m_pExtra->m_dwVariantCounter);
		pStream->Write(m_pExtra->m_iDamagedHP);
		}
	}

