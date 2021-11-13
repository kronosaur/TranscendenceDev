//	CArmorItem.cpp
//
//	CArmorItem class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define PROPERTY_COMPLETE_HP					CONSTLIT("completeHP")
#define PROPERTY_COMPLETE_SET					CONSTLIT("completeSet")
#define PROPERTY_DAMAGE							CONSTLIT("damage")
#define PROPERTY_HP								CONSTLIT("hp")
#define PROPERTY_MAX_HP							CONSTLIT("maxHP")
#define PROPERTY_REPAIR_COST					CONSTLIT("repairCost")
#define PROPERTY_REPAIR_LEVEL					CONSTLIT("repairLevel")

void CArmorItem::AccumulateAttributes (TArray<SDisplayAttribute> *retList) const

//	AccumulateAttributes
//
//	Accumulate display attributes.

	{
	GetArmorClass().AccumulateAttributes(*this, retList);
	}

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

	else if (strEquals(sProperty, PROPERTY_DAMAGE))
		return ICCItemPtr(Max(0, GetMaxHP() - GetHP()));

	else if (strEquals(sProperty, PROPERTY_HP))
		return ICCItemPtr(GetHP());

	else if (strEquals(sProperty, PROPERTY_MAX_HP))
		return ICCItemPtr(GetMaxHP());

	else if (strEquals(sProperty, PROPERTY_REPAIR_COST))
		return ICCItemPtr((int)GetRepairCost());

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

	if (const CInstalledArmor *pInstalled = m_Item.GetInstalledArmor())
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
		int iDamagedHP = m_Item.GetDamagedHP();

		if (retiMaxHP) *retiMaxHP = iMaxHP;
		return Max(0, iMaxHP - iDamagedHP);
		}
	}

CString CArmorItem::GetHPDisplay (const CLanguage::SHPDisplayOptions &Options, int *retiIntegrity) const

//	GetHPDisplay
//
//	Returns a string representing the armor integrity.

	{
	int iMaxHP;
	int iHP = GetHP(&iMaxHP);

	int iIntegrity = (iMaxHP ? mathRound(100.0 * iHP / iMaxHP) : 0);
	if (retiIntegrity)
		*retiIntegrity = iIntegrity;

	switch (Options.iType)
		{
		case CLanguage::EHPDisplay::Percent:
			return CLanguage::ComposeHitPointValue(iIntegrity, Options);

		default:
			return CLanguage::ComposeHitPointValue(GetHP(), Options);
		}
	}

const CShipArmorSegmentDesc &CArmorItem::GetSegmentDesc () const

//	GetSegmentDesc
//
//	Returns the segment descriptor.

	{
	const CSpaceObject *pSource = GetSource();
	if (!pSource)
		return CShipArmorSegmentDesc::m_Null;

	const CShip *pShip = pSource->AsShip();
	if (!pShip)
		return CShipArmorSegmentDesc::m_Null;
		
	const CShipArmorDesc &ArmorDesc = pShip->GetClass().GetArmorDesc();
	int iSect = GetSegmentIndex();
	if (iSect < 0 || iSect >= ArmorDesc.GetCount())
		return CShipArmorSegmentDesc::m_Null;

	return ArmorDesc.GetSegment(iSect);
	}

int CArmorItem::GetSegmentIndex () const

//	GetSegmentIndex
//
//	Returns the segment index (or -1)

	{
	if (const CInstalledArmor *pInstalled = GetInstalledArmor())
		return pInstalled->GetSect();
	else
		return -1;
	}
