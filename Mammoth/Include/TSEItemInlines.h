//	TSEItemInlines.h
//
//	Inline functions for various item classes.
//	Copyright 2019 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

//	CItem Inlines --------------------------------------------------------------

inline bool CItem::IsArmor (void) const
	{
	return (m_pItemType && m_pItemType->GetArmorClass());
	}

inline bool CItem::IsDevice (void) const
	{
	return (m_pItemType && m_pItemType->GetDeviceClass());
	}

inline const CEconomyType *CItem::GetCurrencyType (void) const
	{
	return m_pItemType->GetCurrencyType();
	}

//	CItemType Inlines ----------------------------------------------------------

inline CDesignType *CItemType::GetUseScreen (CString *retsName) const
	{
	return m_pUseScreen.GetDockScreen((CDesignType *)this, retsName);
	}

//	CDifferentiatedItem Inlines ------------------------------------------------

inline int CDifferentiatedItem::GetLevel (void) const
	{
	return m_pCItem->GetLevel();
	}

inline int CDifferentiatedItem::GetMinLevel (void) const
	{
	return GetType().GetMinLevel();
	}

inline const CItemType &CDifferentiatedItem::GetType (void) const
	{
	return *m_pCItem->GetType();
	}

inline CItemType &CDifferentiatedItem::GetType (void)
	{
	return *m_pItem->GetType();
	}

//	CArmorClass Inlines --------------------------------------------------------

inline int CArmorClass::GetDamageAdj (CItemCtx &Ctx, DamageTypes iDamage) const
	{
	const SScalableStats &Stats = GetScaledStats(Ctx.GetItem().AsArmorItemOrThrow()); return Stats.DamageAdj.GetAdj(iDamage);
	}

inline int CArmorClass::GetInstallCost (CItemCtx &Ctx) const
	{
	const SScalableStats &Stats = GetScaledStats(Ctx.GetItem().AsArmorItemOrThrow()); return (int)m_pItemType->GetCurrencyType()->Exchange(Stats.InstallCost);
	}

inline CString CArmorClass::GetName (void)
	{
	return m_pItemType->GetNounPhrase();
	}

inline int CArmorClass::GetRepairCost (CItemCtx &Ctx) const
	{
	const SScalableStats &Stats = GetScaledStats(Ctx.GetItem().AsArmorItemOrThrow()); return (int)m_pItemType->GetCurrencyType()->Exchange(Stats.RepairCost);
	}

inline DWORD CArmorClass::GetUNID (void)
	{
	return m_pItemType->GetUNID();
	}

//	CArmorItem Inlines ---------------------------------------------------------

inline const CArmorClass &CArmorItem::GetArmorClass (void) const
	{
	return *GetType().GetArmorClass();
	}

//	CInstalledArmor Inlines ----------------------------------------------------

inline EDamageResults CInstalledArmor::AbsorbDamage (CSpaceObject *pSource, SDamageCtx &Ctx)
	{
	return m_pArmorClass->AbsorbDamage(CItemCtx(pSource, this), Ctx);
	}

inline int CInstalledArmor::GetDamageEffectiveness (CSpaceObject *pAttacker, CInstalledDevice *pWeapon)
	{
	return m_pArmorClass->GetDamageEffectiveness(pAttacker, pWeapon);
	}

inline int CInstalledArmor::GetLevel (void) const 
	{ 
	return (m_pItem ? m_pItem->GetLevel() : GetClass()->GetItemType()->GetLevel()); 
	}

inline int CInstalledArmor::GetMaxHP (CSpaceObject *pSource) const 
	{
	return m_pArmorClass->GetMaxHP(CItemCtx(pSource, this));
	}

//	CDeviceClass Inlines -------------------------------------------------------

inline int CDeviceClass::GetLevel (void) const
	{
	return m_pItemType->GetLevel();
	}

inline CString CDeviceClass::GetName (void)
	{
	return m_pItemType->GetNounPhrase();
	}

inline DWORD CDeviceClass::GetUNID (void)
	{
	return m_pItemType->GetUNID();
	}

//	CDeviceItem Inlines --------------------------------------------------------

//	CInstalledDevice Inlines ---------------------------------------------------

inline bool CInstalledDevice::IsSecondaryWeapon (void) const 
	{
	DWORD dwLinkedFire;
	return (m_fSecondaryWeapon 
			|| (dwLinkedFire = m_pClass->GetLinkedFireOptions(CItemCtx(NULL, (CInstalledDevice *)this))) == CDeviceClass::lkfEnemyInRange
			|| dwLinkedFire == CDeviceClass::lkfTargetInRange);
	}

//	CDeviceDescList Inlines ----------------------------------------------------

inline CDeviceClass *CDeviceDescList::GetDeviceClass (int iIndex) const
	{
	return m_List[iIndex].Item.GetType()->GetDeviceClass();
	}
