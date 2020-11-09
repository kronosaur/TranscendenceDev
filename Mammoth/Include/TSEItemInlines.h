//	TSEItemInlines.h
//
//	Inline functions for various item classes.
//	Copyright 2019 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

//	CItem Inlines --------------------------------------------------------------

inline ItemCategories CItem::GetCategory (void) const
	{
	return (m_pItemType ? m_pItemType->GetCategory() : itemcatNone);
	}

inline const CEconomyType *CItem::GetCurrencyType (void) const
	{
	return m_pItemType->GetCurrencyType();
	}

inline CDeviceClass *CItem::GetDeviceClass (void) const
	{
	return (IsDevice() ? m_pItemType->GetDeviceClass() : NULL);
	}

inline const CObjectImageArray &CItem::GetImage (void) const
	{
	return (m_pItemType ? m_pItemType->GetImage() : CObjectImageArray::Null());
	}

inline bool CItem::HasAttribute (const CString &sAttrib) const
	{
	return (m_pItemType ? m_pItemType->HasLiteralAttribute(sAttrib): false);
	}

inline bool CItem::IsArmor (void) const
	{
	return (m_pItemType && m_pItemType->IsArmor());
	}

inline bool CItem::IsDevice (void) const
	{
	return (m_pItemType && m_pItemType->IsDevice());
	}

inline bool CItem::IsLauncher (void) const
	{
	return (m_pItemType && m_pItemType->GetCategory() == itemcatLauncher);
	}

inline bool CItem::IsMissile (void) const
	{
	return (m_pItemType && m_pItemType->IsMissile());
	}

inline bool CItem::IsWeapon (void) const
	{
	return (m_pItemType && (m_pItemType->GetCategory() == itemcatWeapon || m_pItemType->GetCategory() == itemcatLauncher));
	}

//	CItemType Inlines ----------------------------------------------------------

inline CDesignType *CItemType::GetUseScreen (CString *retsName) const
	{
	return m_pUseScreen.GetDockScreen((CDesignType *)this, retsName);
	}

//	CDifferentiatedItem Inlines ------------------------------------------------

inline ItemCategories CDifferentiatedItem::GetCategory (void) const
	{
	return m_Item.GetCategory();
	}

inline int CDifferentiatedItem::GetCharges (void) const
	{
	return m_Item.GetCharges();
	}

inline CCurrencyAndValue CDifferentiatedItem::GetCurrencyAndValue (bool bActual) const
	{
	CItemCtx ItemCtx(m_Item);
	return GetType().GetCurrencyAndValue(ItemCtx, bActual);
	}

inline const CEconomyType &CDifferentiatedItem::GetCurrencyType (void) const
	{
	const CEconomyType *pCurrency = GetType().GetCurrencyType();
	if (pCurrency)
		return *pCurrency;
	else
		return GetType().GetUniverse().GetCreditCurrency();
	}

inline const CObjectImageArray &CDifferentiatedItem::GetImage (void) const
	{
	return m_Item.GetImage();
	}

inline int CDifferentiatedItem::GetLevel (void) const
	{
	return m_Item.GetLevel();
	}

inline int CDifferentiatedItem::GetMassKg (void) const
	{
	return m_Item.GetMassKg();
	}

inline int CDifferentiatedItem::GetMinLevel (void) const
	{
	return GetType().GetMinLevel();
	}

inline CString CDifferentiatedItem::GetNounPhrase (DWORD dwFlags) const
	{
	return m_Item.GetNounPhrase(dwFlags);
	}

inline int CDifferentiatedItem::GetVariantNumber (void) const
	{
	return m_Item.GetVariantNumber();
	}

inline const CItemType &CDifferentiatedItem::GetType (void) const
	{
	return *m_Item.GetType();
	}

inline CItemType &CDifferentiatedItem::GetType (void)
	{
	return *m_Item.GetType();
	}

inline bool CDifferentiatedItem::IsLauncher (void) const
	{
	return m_Item.IsLauncher();
	}

inline bool CDifferentiatedItem::IsWeapon (void) const
	{
	return m_Item.IsWeapon();
	}

//	CArmorClass Inlines --------------------------------------------------------

inline int CArmorClass::GetDamageAdj (const CArmorItem &ArmorItem, DamageTypes iDamage) const
	{
	const SScalableStats &Stats = GetScaledStats(ArmorItem); return Stats.DamageAdj.GetAdj(iDamage);
	}

inline int CArmorClass::GetInstallCost (const CArmorItem &ArmorItem) const
	{
	const SScalableStats &Stats = GetScaledStats(ArmorItem); return (int)m_pItemType->GetCurrencyType()->Exchange(Stats.InstallCost);
	}

inline CString CArmorClass::GetName (void) const
	{
	return m_pItemType->GetNounPhrase();
	}

inline DWORD CArmorClass::GetUNID (void)
	{
	return m_pItemType->GetUNID();
	}

//	CArmorItem Inlines ---------------------------------------------------------

inline CArmorItem::operator bool () const
	{
	return !m_Item.IsEmpty();
	}

inline int CArmorItem::CalcBalance (SBalance &retBalance) const
	{
	return GetArmorClass().CalcBalance(*this, retBalance);
	}

inline const CArmorClass &CArmorItem::GetArmorClass (void) const
	{
	return *GetType().GetArmorClass();
	}

inline CArmorClass &CArmorItem::GetArmorClass (void)
	{
	return *GetType().GetArmorClass();
	}

inline int CArmorItem::GetDamageAdj (DamageTypes iDamage) const
	{
	return GetArmorClass().GetDamageAdj(*this, iDamage);
	}

inline int CArmorItem::GetDamageAdj (const DamageDesc &Damage) const
	{
	return GetArmorClass().GetDamageAdj(*this, Damage);
	}

inline int CArmorItem::GetDamageEffectiveness (CSpaceObject *pAttacker, CInstalledDevice *pWeapon) const
	{
	return GetArmorClass().GetDamageEffectiveness(*this, pAttacker, pWeapon);
	}

inline const CItemEnhancementStack &CArmorItem::GetEnhancements (void) const
	{
	const CItemEnhancementStack *pStack = GetEnhancementStack();
	if (pStack) 
		return *pStack; 
	else 
		return *m_pNullEnhancements;
	}

inline int CArmorItem::GetInstallCost (void) const
	{
	return GetArmorClass().GetInstallCost(*this);
	}

inline const CInstalledArmor *CArmorItem::GetInstalledArmor (void) const
	{
	return m_Item.GetInstalledArmor();
	}

inline int CArmorItem::GetMaxHP (bool bForceComplete) const
	{
	return GetArmorClass().GetMaxHP(*this, bForceComplete);
	}

inline bool CArmorItem::GetReferenceDamageAdj (int *retiHP, int *retArray) const
	{
	return GetArmorClass().GetReferenceDamageAdj(*this, retiHP, retArray);
	}

inline CurrencyValue CArmorItem::GetRepairCost (int iHPToRepair) const
	{
	return GetArmorClass().GetRepairCost(*this, iHPToRepair);
	}

inline int CArmorItem::GetRepairLevel (void) const
	{
	return GetArmorClass().GetRepairLevel(*this);
	}

inline CSpaceObject *CArmorItem::GetSource (void) const
	{
	if (const CInstalledArmor *pInstalled = GetInstalledArmor())
		return pInstalled->GetSource();
	else
		return NULL;
	}

inline bool CArmorItem::IsImmune (SpecialDamageTypes iSpecialDamage) const
	{
	return GetArmorClass().IsImmune(*this, iSpecialDamage);
	}


//	CInstalledArmor Inlines ----------------------------------------------------

inline EDamageResults CInstalledArmor::AbsorbDamage (CSpaceObject *pSource, SDamageCtx &Ctx)
	{
	CItemCtx ItemCtx(pSource, this);
	return m_pArmorClass->AbsorbDamage(ItemCtx, Ctx);
	}

inline int CInstalledArmor::GetDamageEffectiveness (CSpaceObject *pAttacker, CInstalledDevice *pWeapon)
	{
	return m_pItem->AsArmorItemOrThrow().GetDamageEffectiveness(pAttacker, pWeapon);
	}

inline int CInstalledArmor::GetLevel (void) const 
	{ 
	return (m_pItem ? m_pItem->GetLevel() : GetClass()->GetItemType()->GetLevel()); 
	}

inline int CInstalledArmor::GetMaxHP (CSpaceObject *pSource) const 
	{
	return m_pItem->AsArmorItemOrThrow().GetMaxHP();
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

inline DWORD CDeviceClass::GetUNID (void) const
	{
	return m_pItemType->GetUNID();
	}

//	CDeviceItem Inlines --------------------------------------------------------

inline CDeviceItem::operator bool () const
	{
	return !m_Item.IsEmpty();
	}

inline const CDeviceClass &CDeviceItem::GetDeviceClass (void) const
	{
	return *GetType().GetDeviceClass();
	}

inline CDeviceClass &CDeviceItem::GetDeviceClass (void)
	{
	return *GetType().GetDeviceClass();
	}

inline int CDeviceItem::GetDeviceSlot (void) const
	{
	if (const CInstalledDevice *pDevice = GetInstalledDevice())
		return pDevice->GetDeviceSlot();
	else
		return -1;
	}

inline const CItemEnhancementStack &CDeviceItem::GetEnhancements (void) const
	{
	const CItemEnhancementStack *pStack = GetEnhancementStack();
	if (pStack) 
		return *pStack; 
	else 
		return *m_pNullEnhancements;
	}

inline const CInstalledDevice *CDeviceItem::GetInstalledDevice (void) const
	{
	return m_Item.GetInstalledDevice();
	}

inline CInstalledDevice *CDeviceItem::GetInstalledDevice (void)
	{
	return m_Item.GetInstalledDevice();
	}

inline CSpaceObject *CDeviceItem::GetSource (void) const
	{
	if (const CInstalledDevice *pInstalled = GetInstalledDevice())
		return pInstalled->GetSource();
	else
		return NULL;
	}

inline DWORD CDeviceItem::GetTargetTypes (void) const
	{
	return GetType().GetDeviceClass()->GetTargetTypes(*this);
	}

inline int CDeviceItem::GetWeaponEffectiveness (CSpaceObject *pTarget) const
	{
	return GetType().GetDeviceClass()->GetWeaponEffectiveness(*this, pTarget);
	}

inline const CWeaponFireDesc *CDeviceItem::GetWeaponFireDesc (void) const
	{
	return GetType().GetDeviceClass()->GetWeaponFireDesc(*this, CItem());
	}

inline const CWeaponFireDesc *CDeviceItem::GetWeaponFireDesc (const CItem &Ammo) const
	{
	return GetType().GetDeviceClass()->GetWeaponFireDesc(*this, Ammo);
	}

inline const CWeaponFireDesc &CDeviceItem::GetWeaponFireDescForVariant (int iVariant) const
	{
	return GetType().GetDeviceClass()->GetWeaponFireDescForVariant(*this, iVariant);
	}

inline int CDeviceItem::GetWeaponVariantCount (void) const
	{
	return GetType().GetDeviceClass()->GetWeaponVariantCount(*this);
	}

inline bool CDeviceItem::IsAreaWeapon (void) const
	{
	return GetType().GetDeviceClass()->IsAreaWeapon(*this);
	}

inline bool CDeviceItem::IsEnabled (void) const
	{
	if (const CInstalledDevice *pDevice = GetInstalledDevice())
		return pDevice->IsEnabled();
	else
		return false;
	}

inline bool CDeviceItem::IsMiningWeapon (void) const
	{
	if (const CWeaponFireDesc *pShot = GetWeaponFireDesc())
		return (pShot->GetDamage().GetMiningDamage() > 0 || GetEnhancements().HasSpecialDamage(specialMining));
	else
		return false;
	}

inline bool CDeviceItem::IsTrackingWeapon (void) const
	{
	return GetType().GetDeviceClass()->IsTrackingWeapon(*this);
	}

inline bool CDeviceItem::IsWeaponVariantValid (int iVariant) const
	{
	return GetType().GetDeviceClass()->IsWeaponVariantValid(*this, iVariant);
	}

inline bool CDeviceItem::NeedsAutoTarget (int *retiMinFireArc, int *retiMaxFireArc) const
	{
	return GetType().GetDeviceClass()->NeedsAutoTarget(*this, retiMinFireArc, retiMaxFireArc);
	}

//	CInstalledDevice Inlines ---------------------------------------------------

inline bool CInstalledDevice::IsSecondaryWeapon (void) const 
	{
	DWORD dwLinkedFire;
	const CDeviceItem DeviceItem = m_pItem->AsDeviceItemOrThrow();
	return (m_fSecondaryWeapon
			|| (dwLinkedFire = DeviceItem.GetLinkedFireOptions()) == CDeviceClass::lkfEnemyInRange
			|| dwLinkedFire == CDeviceClass::lkfTargetInRange);
	}

//	CDeviceDescList Inlines ----------------------------------------------------

inline CDeviceClass *CDeviceDescList::GetDeviceClass (int iIndex) const
	{
	return m_List[iIndex].Item.GetType()->GetDeviceClass();
	}

//	CMissileItem Inlines -------------------------------------------------------

inline CMissileItem::operator bool () const
	{
	return !m_Item.IsEmpty();
	}

