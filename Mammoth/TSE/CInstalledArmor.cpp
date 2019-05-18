//	CInstalledArmor.cpp
//
//	CInstalledArmor class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

bool CInstalledArmor::AccumulateEnhancements (CSpaceObject *pSource, CInstalledDevice *pTarget, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements)

//	AccumulateEnhancements
//
//	Accumulate enhancements on devices.

	{
	if (m_pArmorClass == NULL)
		return false;
		
	return m_pArmorClass->AccumulateEnhancements(CItemCtx(pSource, this), pTarget, EnhancementIDs, pEnhancements);
	}

bool CInstalledArmor::AccumulatePerformance (CItemCtx &ItemCtx, SShipPerformanceCtx &Ctx) const

//	AccumulateEnhancements
//
//	Accumulate enhancements on devices.

	{
	if (m_pArmorClass)
		return false;

    return m_pArmorClass->AccumulatePerformance(ItemCtx, Ctx);
	}

void CInstalledArmor::FinishInstall (CSpaceObject &Source)

//	FinishInstall
//
//	Finishes the parts of the install that fire events
//	(At object creation time these events fire after the entire object
//	is created).

	{
	DEBUG_TRY

	m_pItem->FireOnInstall(&Source);

	CShip *pShip = Source.AsShip();
	if (pShip)
		pShip->GetController()->OnItemInstalled(*m_pItem);

	DEBUG_CATCH
	}

int CInstalledArmor::GetHitPointsPercent (CSpaceObject *pSource)

//	GetHitPointsPercent
//
//	Return the armor integrity (hit points) as a % of maximum hp.

	{
	return CArmorClass::CalcIntegrity(GetHitPoints(), GetMaxHP(pSource));
	}

int CInstalledArmor::IncCharges (CSpaceObject *pSource, int iChange)

//	IncCharges
//
//	Increment charges

	{
	CShip *pShip = pSource->AsShip();
	if (pShip == NULL)
		return -1;

	CItemListManipulator ItemList(pSource->GetItemList());
	pShip->SetCursorAtArmor(ItemList, GetSect());
	pShip->RechargeItem(ItemList, iChange);

	return ItemList.GetItemAtCursor().GetCharges();
	}

void CInstalledArmor::Install (CSpaceObject &Source, CItemListManipulator &ItemList, int iSect, bool bInCreate)

//	Install
//
//	Install the armor item

	{
	CItem *pItem = ItemList.GetItemPointerAtCursor();
	if (pItem == NULL)
		throw CException(ERR_FAIL, CONSTLIT("CInstalledArmor::Install: Item is NULL."));

	CItemType *pType = pItem->GetType();
	if (pType == NULL)
		throw CException(ERR_FAIL, CONSTLIT("CInstalledArmor::Install: Item type is NULL."));

	m_pArmorClass = pType->GetArmorClass();
	if (m_pArmorClass == NULL)
		throw CException(ERR_FAIL, CONSTLIT("CInstalledArmor::Install: Item is not armor."));

	//	Set up the enhancements from the armor itself. We will add other 
	//	enhancements later, in CShip::CalcArmorBonus.
	//
	//	NOTE: We need to do this so that the enhancements match the calculation
	//	of hit points below. That way, in CalcArmorBonus we will have the 
	//	correct value for old hit points when we apply other enhancements.

	const CItemEnhancement &Mods = pItem->GetMods();
	if (!Mods.IsEmpty())
		{
		TSharedPtr<CItemEnhancementStack> pEnhancements(new CItemEnhancementStack);
		pEnhancements->Insert(Mods);
		m_pEnhancements = pEnhancements;
		}
	else
		m_pEnhancements = NULL;

	//	Set

	CItemCtx ItemCtx(pItem, &Source);

	m_pSource = &Source;
	m_iSect = iSect;
	m_fComplete = false;
	m_fPrimeSegment = false;
	m_fConsumePower = false;
	m_iHitPoints = pItem->GetHitPoints(ItemCtx);

	//	Mark the item as installed

	ItemList.SetInstalledAtCursor(*this);

	//	After we've installed, set the item pointer
	//	(we have to wait until after installation so that we
	//	have the proper pointer)

	m_pItem = ItemList.GetItemPointerAtCursor();

	//	Event (when creating a ship we wait until the
	//	whole ship is created before firing the event)

	if (!bInCreate)
		FinishInstall(Source);
	}

void CInstalledArmor::ReadFromStream (CSpaceObject &Source, int iSect, SLoadCtx &Ctx)

//	ReadFromStream
//
//	DWORD		class UNID
//	DWORD		m_iSect
//	DWORD		hit points
//	DWORD		mods
//	DWORD		flags
//	CEnhancementStack	m_pEnhancements

	{
	DWORD dwLoad;
	Ctx.pStream->Read(dwLoad);
	m_pArmorClass = Source.GetUniverse().FindArmor(dwLoad);

	if (Ctx.dwVersion >= 54)
		{
		Ctx.pStream->Read(dwLoad);
		m_iSect = dwLoad;
		}
	else
		m_iSect = iSect;

	//	The caller is responsible for initializing this for earlier versions

	Ctx.pStream->Read(m_iHitPoints);

	//	Previous versions saved mods

	if (Ctx.dwVersion < 59)
		{
		CItemEnhancement Dummy;
		Dummy.ReadFromStream(Ctx);
		}

	Ctx.pStream->Read(dwLoad);
	m_fComplete =			((dwLoad & 0x00000001) ? true : false);
	m_fPrimeSegment =		((dwLoad & 0x00000002) ? true : false);
	m_fConsumePower =		((dwLoad & 0x00000004) ? true : false);
	bool bHasEnhancements =	((dwLoad & 0x00000008) ? true : false);

	//	Enhancements stack

	if (bHasEnhancements)
		m_pEnhancements = CItemEnhancementStack::ReadFromStream(Ctx);

	//	Fix up the item pointer

	m_pSource = NULL;
	m_pItem = NULL;
	if (m_pArmorClass != NULL && m_iSect != -1)
		{
		CItemListManipulator ItemList(Source.GetItemList());
		Source.SetCursorAtArmor(ItemList, this);
		if (ItemList.IsCursorValid())
			{
			m_pSource = &Source;
			m_pItem = ItemList.GetItemPointerAtCursor();
			m_pItem->SetInstalled(*this);
			}
		}
	}

void CInstalledArmor::SetComplete (CSpaceObject *pSource, bool bComplete)

//	SetComplete
//
//	Adds the armor complete bonus

	{
	if (bComplete != m_fComplete)
		{
		int iOldMaxHP = GetMaxHP(pSource);

		m_fComplete = bComplete;

		m_iHitPoints = CArmorClass::CalcMaxHPChange(m_iHitPoints, iOldMaxHP, GetMaxHP(pSource));
		}
	}

void CInstalledArmor::SetEnhancements (CSpaceObject *pSource, const TSharedPtr<CItemEnhancementStack> &pStack)

//	SetEnhancements
//
//	Sets the enhancement stack for the armor

	{
	//	Figure out our current max HP, because we might need to change current
	//	hit points.

	int iOldMaxHP = GetMaxHP(pSource);

	//	Reset the stack.

	if (pStack && !pStack->IsEmpty())
		m_pEnhancements = pStack;
	else
		m_pEnhancements.Delete();

	//	Now compute our new maximum and see if we need to adjust current hit 
	//	points.

	m_iHitPoints = CArmorClass::CalcMaxHPChange(m_iHitPoints, iOldMaxHP, GetMaxHP(pSource));
	}

void CInstalledArmor::WriteToStream (IWriteStream *pStream)

//	WriteToStream
//
//	DWORD		class UNID
//	DWORD		m_iSect
//	DWORD		hit points
//	DWORD		flags
//	CEnhancementStack	m_pEnhancements

	{
	DWORD dwSave = m_pArmorClass->GetUNID();
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	dwSave = m_iSect;
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	pStream->Write((char *)&m_iHitPoints, sizeof(DWORD));

	dwSave = 0;
	dwSave |= (m_fComplete ?		0x00000001 : 0);
	dwSave |= (m_fPrimeSegment ?	0x00000002 : 0);
	dwSave |= (m_fConsumePower ?	0x00000004 : 0);
	dwSave |= (m_pEnhancements ?	0x00000008 : 0);
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	if (m_pEnhancements)
		CItemEnhancementStack::WriteToStream(m_pEnhancements, pStream);
	}
