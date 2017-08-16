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
	return m_pArmorClass->AccumulateEnhancements(CItemCtx(pSource, this), pTarget, EnhancementIDs, pEnhancements);
	}

bool CInstalledArmor::AccumulatePerformance (CItemCtx &ItemCtx, SShipPerformanceCtx &Ctx) const

//	AccumulateEnhancements
//
//	Accumulate enhancements on devices.

	{
    return m_pArmorClass->AccumulatePerformance(ItemCtx, Ctx);
	}

void CInstalledArmor::FinishInstall (CSpaceObject *pSource)

//	FinishInstall
//
//	Finishes the parts of the install that fire events
//	(At object creation time these events fire after the entire object
//	is created).

	{
	DEBUG_TRY

	m_pItem->FireOnInstall(pSource);

	CShip *pShip = pSource->AsShip();
	if (pShip)
		pShip->GetController()->OnItemInstalled(*m_pItem);

	DEBUG_CATCH
	}

int CInstalledArmor::GetHitPointsPercent (CSpaceObject *pSource)

//	GetHitPointsPercent
//
//	Return the armor integrity (hit points) as a % of maximum hp.

	{
	int iMaxHP = GetMaxHP(pSource);
	if (iMaxHP == 0 || iMaxHP <= GetHitPoints())
		return 100;

	return ((1000 * GetHitPoints() / iMaxHP) + 5) / 10;
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

void CInstalledArmor::Install (CSpaceObject *pObj, CItemListManipulator &ItemList, int iSect, bool bInCreate)

//	Install
//
//	Install the armor item

	{
	CItem *pItem = ItemList.GetItemPointerAtCursor();
	ASSERT(pItem);

	CItemCtx ItemCtx(pItem, pObj);

	CItemType *pType = pItem->GetType();
	ASSERT(pType);

	m_pArmorClass = pType->GetArmorClass();
	ASSERT(m_pArmorClass);

	m_iSect = iSect;
	m_fComplete = false;
	m_fPrimeSegment = false;
	m_fConsumePower = false;
	m_iHitPoints = m_pArmorClass->GetMaxHP(ItemCtx);
	if (pItem->IsDamaged())
		m_iHitPoints = m_iHitPoints / 2;

	//	Mark the item as installed

	ItemList.SetInstalledAtCursor(iSect);

	//	Clear the damage bit, since we restore it properly on uninstall.

	ItemList.SetDamagedAtCursor(false);

	//	After we've installed, set the item pointer
	//	(we have to wait until after installation so that we
	//	have the proper pointer)

	m_pItem = ItemList.GetItemPointerAtCursor();

	//	Event (when creating a ship we wait until the
	//	whole ship is created before firing the event)

	if (!bInCreate)
		FinishInstall(pObj);
	}

void CInstalledArmor::ReadFromStream (CSpaceObject *pSource, int iSect, SLoadCtx &Ctx)

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

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	m_pArmorClass = g_pUniverse->FindArmor(dwLoad);

	if (Ctx.dwVersion >= 54)
		{
		Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
		m_iSect = dwLoad;
		}
	else
		m_iSect = iSect;

	//	The caller is responsible for initializing this for earlier versions

	Ctx.pStream->Read((char *)&m_iHitPoints, sizeof(DWORD));

	//	Previous versions saved mods

	if (Ctx.dwVersion < 59)
		{
		CItemEnhancement Dummy;
		Dummy.ReadFromStream(Ctx);
		}

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	m_fComplete =			((dwLoad & 0x00000001) ? true : false);
	m_fPrimeSegment =		((dwLoad & 0x00000002) ? true : false);
	m_fConsumePower =		((dwLoad & 0x00000004) ? true : false);
	bool bHasEnhancements =	((dwLoad & 0x00000008) ? true : false);

	//	Enhancements stack

	if (bHasEnhancements)
		m_pEnhancements = CItemEnhancementStack::ReadFromStream(Ctx);

	//	Fix up the item pointer

	m_pItem = NULL;
	if (m_pArmorClass != NULL && m_iSect != -1)
		{
		CItemListManipulator ItemList(pSource->GetItemList());
		pSource->SetCursorAtArmor(ItemList, this);
		if (ItemList.IsCursorValid())
			m_pItem = ItemList.GetItemPointerAtCursor();
		}
	}

void CInstalledArmor::SetComplete (CSpaceObject *pSource, bool bComplete)

//	SetComplete
//
//	Adds the armor complete bonus

	{
	if (bComplete != m_fComplete)
		{
		m_fComplete = bComplete;

		if (m_fComplete)
			m_iHitPoints += m_pArmorClass->GetCompleteBonus();
		else
			m_iHitPoints = Min(m_iHitPoints, GetMaxHP(pSource));
		}
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
