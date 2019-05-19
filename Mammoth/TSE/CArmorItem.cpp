//	CArmorItem.cpp
//
//	CArmorItem class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define PROPERTY_COMPLETE_HP					CONSTLIT("completeHP")
#define PROPERTY_COMPLETE_SET					CONSTLIT("completeSet")
#define PROPERTY_HP								CONSTLIT("hp")
#define PROPERTY_MAX_HP							CONSTLIT("maxHP")
#define PROPERTY_REPAIR_COST					CONSTLIT("repairCost")
#define PROPERTY_REPAIR_LEVEL					CONSTLIT("repairLevel")

ICCItemPtr CArmorItem::FindProperty (const CString &sProperty) const

//	FindProperty
//
//	Finds a property.

	{
	if (strEquals(sProperty, PROPERTY_COMPLETE_HP))
		return ICCItemPtr(GetMaxHP(true));

	else if (strEquals(sProperty, PROPERTY_COMPLETE_SET))
		{
		const CInstalledArmor *pInstalled = GetInstalledArmor();
		if (pInstalled == NULL)
			return ICCItemPtr(ICCItem::Nil);

		return ICCItemPtr(pInstalled->IsComplete());
		}

	else if (strEquals(sProperty, PROPERTY_HP))
		return ICCItemPtr(GetHP());

	else if (strEquals(sProperty, PROPERTY_MAX_HP))
		return ICCItemPtr(GetMaxHP());

	else if (strEquals(sProperty, PROPERTY_REPAIR_COST))
		return ICCItemPtr(GetRepairCost());

	else if (strEquals(sProperty, PROPERTY_REPAIR_LEVEL))
		return ICCItemPtr(GetRepairLevel());

	else
		return GetArmorClass().FindItemProperty(*this, sProperty);
	}

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
