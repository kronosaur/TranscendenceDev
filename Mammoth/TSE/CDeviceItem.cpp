//	CDeviceItem.cpp
//
//	CDeviceItem class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CDeviceItem::AccumulateAttributes (const CItem &Ammo, TArray<SDisplayAttribute> *retList) const

//	AccumulateAttributes
//
//	Accumulate display attributes.

	{
	GetDeviceClass().AccumulateAttributes(*this, Ammo, retList);
	}

TSharedPtr<CItemEnhancementStack> CDeviceItem::GetEnhancementStack (void) const

//	GetEnhancementStack
//
//	Returns the enhancement stack for this item. If there are no enhancements, 
//	we return NULL.

	{
	//	If we have installed armor, then get the enhancement stack from it.

	if (const CInstalledDevice *pInstalled = m_Item.GetInstalledDevice())
		return pInstalled->GetEnhancementStack();

	//	Otherwise, see if we've got a cached enhancement stack

	if (m_pEnhancements)
		return m_pEnhancements;

	//	Otherwise, we need to create one from mods

	const CItemEnhancement &Mods = m_Item.GetMods();
	if (Mods.IsEmpty())
		return NULL;

	m_pEnhancements.TakeHandoff(new CItemEnhancementStack);
	m_pEnhancements->Insert(Mods);
	return m_pEnhancements;
	}

int CDeviceItem::GetHP (int *retiMaxHP, bool bUninstalled) const

//	GetHP
//
//	Returns current hit point level.

	{
	const CInstalledDevice *pInstalled = GetInstalledDevice();
	if (!bUninstalled && pInstalled)
		{
		CItemCtx ItemCtx(GetSource(), pInstalled);
		return GetDeviceClass().GetHitPoints(ItemCtx, retiMaxHP);
		}
	else
		{
		CItemCtx ItemCtx(*this);
		return GetDeviceClass().GetHitPoints(ItemCtx, retiMaxHP);
		}
	}

DWORD CDeviceItem::GetLinkedFireOptions (void) const

//	GetLinkedFireOptions
//
//	Returns linked fire options.

	{
	//	Options from enhancements

	DWORD dwOptions = GetEnhancements().GetLinkedFireOptions();

	//	Options from the device slot

	if (const CInstalledDevice *pInstalled = m_Item.GetInstalledDevice())
		dwOptions = CDeviceClass::CombineLinkedFireOptions(dwOptions, pInstalled->GetSlotLinkedFireOptions());

	//	Options from device class

	dwOptions = CDeviceClass::CombineLinkedFireOptions(dwOptions, GetDeviceClass().GetLinkedFireOptions());
	
	//	Done

	return dwOptions;
	}

Metric CDeviceItem::GetMaxEffectiveRange (CSpaceObject *pTarget) const

//	GetMaxEffectiveRange
//
//	Returns the maximum effective range.

	{
	if (const CInstalledDevice *pInstalled = GetInstalledDevice())
		{
		return GetDeviceClass().GetMaxEffectiveRange(pInstalled->GetSource(), pInstalled, pTarget);
		}
	else
		{
		return GetDeviceClass().GetMaxEffectiveRange(NULL, NULL, pTarget);
		}
	}

int CDeviceItem::GetMaxHP (void) const

//	GetMaxHP
//
//	Return max hit points.

	{
	int iMaxHP;

	const CInstalledDevice *pInstalled = GetInstalledDevice();
	if (pInstalled)
		{
		CItemCtx ItemCtx(GetSource(), pInstalled);
		GetDeviceClass().GetHitPoints(ItemCtx, &iMaxHP);
		}
	else
		{
		CItemCtx ItemCtx(*this);
		GetDeviceClass().GetHitPoints(ItemCtx, &iMaxHP);
		}

	return iMaxHP;
	}

bool CDeviceItem::IsMissileDefenseWeapon (void) const

//	IsMissileDefenseWeapon
//
//	Returns TRUE if this weapon has missile defense capabilities.
//
//	LATER: Paradoxically, this return FALSE for missile defense devices. In the
//	future we should fix that. The semantics is that we fire on missiles during
//	a normal fire (Activate) call, not during OnUpdate.

	{
	//	See if this is confered via enhancement or via slot property.

	if (const CItemEnhancementStack *pStack = GetEnhancementStack())
		{
		if (pStack->IsMissileDefense())
			return true;
		}

	return false;
	}

bool CDeviceItem::IsTargetableMissileDefenseWeapon (void) const

//	IsTargetableMissileDefenseWeapon
//
//	Returns TRUE if we target specially marked missiles.

	{
	if (const CInstalledDevice *pInstalled = GetInstalledDevice())
		{
		return pInstalled->CanTargetMissiles();
		}
	else
		return false;
	}

bool CDeviceItem::IsWeaponAligned (CSpaceObject *pTarget, int *retiAimAngle, int *retiFireAngle) const

//	IsWeaponAligned
//
//	Returns TRUE if we're aligned.

	{
	if (const CInstalledDevice *pInstalled = GetInstalledDevice())
		{
		return GetDeviceClass().IsWeaponAligned(pInstalled->GetSource(), pInstalled, pTarget, retiAimAngle, retiFireAngle);
		}
	else
		return false;
	}
