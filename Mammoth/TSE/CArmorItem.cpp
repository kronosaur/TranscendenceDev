//	CArmorItem.cpp
//
//	CArmorItem class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

TSharedPtr<CItemEnhancementStack> CArmorItem::GetEnhancementStack (void) const

//	GetEnhancementStack
//
//	Returns the enhancement stack for this item. If there are no enhancements, 
//	we return NULL.

	{
	//	If we have installed armor, then get the enhancement stack from it.

	if (const CInstalledArmor *pInstalled = m_pCItem->GetInstalledArmor())
		return pInstalled->GetEnhancementStack();

	//	Otherwise, see if we've got a cached enhancement stack

	if (m_pEnhancements)
		return m_pEnhancements;

	//	Otherwise, we need to create one from mods

	const CItemEnhancement &Mods = m_pCItem->GetMods();
	if (Mods.IsEmpty())
		return NULL;

	m_pEnhancements.TakeHandoff(new CItemEnhancementStack);
	m_pEnhancements->Insert(Mods);
	return m_pEnhancements;
	}

int CArmorItem::GetHP (int *retiMaxHP, bool bUninstalled) const

//	GetHP
//
//	Returns the current hit points.

	{
	const CInstalledArmor *pInstalled = GetInstalledArmor();
	if (!bUninstalled && pInstalled)
		{
		if (retiMaxHP) *retiMaxHP = GetMaxHP();
		return pInstalled->GetHitPoints();
		}
	else
		{
		int iMaxHP = GetMaxHP();
		int iDamagedHP = m_pCItem->GetDamagedHP();

		if (retiMaxHP) *retiMaxHP = iMaxHP;
		return Max(0, iMaxHP - iDamagedHP);
		}
	}
