//	CItemEnhancementStack.cpp
//
//	CItemEnhancementStack class
//	Copyright (c) 2013 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CItemEnhancementStack::AccumulateAttributes (CItemCtx &Ctx, TArray<SDisplayAttribute> *retList) const

//	AccumulateAttributes
//
//	Add enhancement attributes as appropriate.

	{
	int i;

	CDeviceClass *pClass = Ctx.GetDeviceClass();

	//	Add all damage resistance enhancements

	bool bNoDamageAdj = false;
	int iAdj;
	if ((iAdj = GetResistEnergyAdj()) != 100)
		{
		retList->Insert(SDisplayAttribute((iAdj < 100 ? attribPositive : attribNegative), (iAdj < 100 ? CONSTLIT("+energy resist") : CONSTLIT("-energy vulnerable")), true));
		bNoDamageAdj = true;
		}

	if ((iAdj = GetResistMatterAdj()) != 100)
		{
		retList->Insert(SDisplayAttribute((iAdj < 100 ? attribPositive : attribNegative), (iAdj < 100 ? CONSTLIT("+matter resist") : CONSTLIT("-matter vulnerable")), true));
		bNoDamageAdj = true;
		}

	//	Per damage-type bonuses

	if (!bNoDamageAdj)
		{
		for (i = 0; i < damageCount; i++)
			{
			if ((iAdj = GetResistDamageAdj((DamageTypes)i)) != 100)
				retList->Insert(SDisplayAttribute((iAdj < 100 ? attribPositive : attribNegative), GetDamageShortName((DamageTypes)i), true));
			}
		}

	//	Add some binary enhancements

	for (i = 0; i < m_Stack.GetCount(); i++)
		{
		bool bDisadvantage = m_Stack[i].IsDisadvantage();
		EDisplayAttributeTypes iDisplayType = (bDisadvantage ? attribNegative : attribPositive);

		switch (m_Stack[i].GetType())
			{
			case etRegenerate:
			case etHealerRegenerate:
			case etReflect:
			case etRepairOnHit:
			case etSpecialDamage:
			case etConferSpecialDamage:
			case etImmunityIonEffects:
			case etPhotoRegenerate:
			case etPhotoRecharge:
				m_Stack[i].AccumulateAttributes(Ctx, retList);
				break;
			}
		}

	//	Power adjustment

	int iPowerAdj = GetPowerAdj();
	if (iPowerAdj > 100)
		retList->Insert(SDisplayAttribute(attribNegative, CONSTLIT("-drain"), true));
	else if (iPowerAdj < 100)
		retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("+efficient"), true));

	//	Enhancements to fire rate

	int iFireAdj = GetActivateDelayAdj();
	if (iFireAdj > 100)
		retList->Insert(SDisplayAttribute(attribNegative, CONSTLIT("-slow"), true));
	else if (iFireAdj < 100)
		retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("+fast"), true));

	//	Add bonus

	int iBonus = GetBonus();
	if (iBonus < 0)
		retList->Insert(SDisplayAttribute(attribNegative, strPatternSubst(CONSTLIT("%d%%"), iBonus), true));
	else if (iBonus > 0)
		retList->Insert(SDisplayAttribute(attribPositive, strPatternSubst(CONSTLIT("+%d%%"), iBonus), true));

	//	Add tracking

	if (IsTracking() && pClass && !pClass->IsTrackingWeapon(CItemCtx()))
		retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("+tracking"), true));
	}

int CItemEnhancementStack::ApplyDamageAdj (const DamageDesc &Damage, int iDamageAdj) const

//	ApplyDamageAdj
//
//	Adds the stack's damage adj to the given one.

	{
	int i;

	Metric rValue = (Metric)iDamageAdj;

	//	Apply enhancements (including on the item itself)

	for (i = 0; i < m_Stack.GetCount(); i++)
		{
		int iAdj = m_Stack[i].GetDamageAdj(Damage);
		if (iAdj != 100)
			rValue = iAdj * rValue / 100.0;
		}

	if (rValue > CDamageAdjDesc::MAX_DAMAGE_ADJ)
		return CDamageAdjDesc::MAX_DAMAGE_ADJ;

	return (int)(rValue + 0.5);
	}

void CItemEnhancementStack::ApplySpecialDamage (DamageDesc *pDamage) const

//	ApplySpecialDamage
//
//	Adds special damage types to the given damage descriptor.

	{
	int i;

	for (i = 0; i < m_Stack.GetCount(); i++)
		{
		int iLevel;
		SpecialDamageTypes iSpecial;
		if ((iSpecial = m_Stack[i].GetSpecialDamage(&iLevel)) != specialNone)
			{
			if (iLevel > pDamage->GetSpecialDamage(iSpecial))
				pDamage->SetSpecialDamage(iSpecial, iLevel);
			}
		}
	}

int CItemEnhancementStack::CalcActivateDelay (CItemCtx &DeviceCtx) const

//	CalcActivateDelay
//
//	Calculates the activation delay (in ticks) for the given device if we apply
//	this enhancement stack.

	{
	int i;

	CDeviceClass *pClass = DeviceCtx.GetDeviceClass();
    if (pClass == NULL)
        pClass = DeviceCtx.GetVariantDevice();
    if (pClass == NULL)
		return 0;

	//	Get the raw activation delay. NOTE: This DOES NOT include
	//	any enhancements on the item.

	Metric rDelay = pClass->GetActivateDelay(DeviceCtx.GetDevice(), DeviceCtx.GetSource());

	//	Apply enhancements (including on the item itself)

	for (i = 0; i < m_Stack.GetCount(); i++)
		{
		int iMin, iMax;
		int iAdj = m_Stack[i].GetActivateRateAdj(&iMin, &iMax);
		if (iAdj != 100)
			{
			rDelay = iAdj * rDelay / 100.0;
			if (rDelay < (Metric)iMin)
				rDelay = (Metric)iMin;
			else if (iMax > 0 && rDelay > (Metric)iMax)
				rDelay = (Metric)iMax;
			}
		}

	return (int)(rDelay + 0.5);
	}

void CItemEnhancementStack::CalcCache (void) const

//	CalcCache
//
//	Computes accumulated values

	{
	int i;

	m_iBonus = 0;
	m_Damage = DamageDesc(damageGeneric, DiceRange());

	for (i = 0; i < m_Stack.GetCount(); i++)
		{
		m_iBonus += m_Stack[i].GetHPBonus();

		int iLevel;
		SpecialDamageTypes iSpecial;
		if ((iSpecial = m_Stack[i].GetSpecialDamage(&iLevel)) != specialNone)
			m_Damage.SetSpecialDamage(iSpecial, iLevel);
		}

	//	Done

	m_bCacheValid = true;
	}

void CItemEnhancementStack::Delete (void)

//	Delete
//
//	Delete reference

	{
	if (--m_dwRefCount <= 0)
		delete this;
	}

int CItemEnhancementStack::GetAbsorbAdj (const DamageDesc &Damage) const

//	GetAbsorbAdj
//
//	Compute the absorb adjustment (for shields)

	{
	int i;

	Metric rValue = 100.0;

	//	Apply enhancements (including on the item itself)

	for (i = 0; i < m_Stack.GetCount(); i++)
		{
		int iAdj = m_Stack[i].GetAbsorbAdj(Damage);
		if (iAdj != 100)
			rValue = iAdj * rValue / 100.0;
		}

	return (int)(rValue + 0.5);
	}

int CItemEnhancementStack::GetActivateDelayAdj (void) const

//	GetActivateDelayAdj
//
//	Computes the activation delay

	{
	int i;

	Metric rDelay = 100.0;

	//	Apply enhancements (including on the item itself)

	for (i = 0; i < m_Stack.GetCount(); i++)
		{
		int iMin, iMax;
		int iAdj = m_Stack[i].GetActivateRateAdj(&iMin, &iMax);
		if (iAdj != 100)
			rDelay = iAdj * rDelay / 100.0;
		}

	return (int)(rDelay + 0.5);
	}

int CItemEnhancementStack::GetBonus (void) const

//	GetBonus
//
//	Returns the accumulated bonus for all enhancements on the stack.

	{
	if (!m_bCacheValid)
		CalcCache();

	return m_iBonus;
	}

const DamageDesc &CItemEnhancementStack::GetDamage (void) const

//	GetDamage
//
//	Returns special damage.

	{
	if (!m_bCacheValid)
		CalcCache();

	return m_Damage;
	}

int CItemEnhancementStack::GetDamageAdj (const DamageDesc &Damage) const

//	GetDamageAdj
//
//	Compute the damage adjustment.
//
//	NOTE: We are guaranteed to return values from 0 to MAX_DAMAGE_ADJ (100,000)

	{
	int i;

	Metric rValue = 100.0;

	//	Apply enhancements (including on the item itself)

	for (i = 0; i < m_Stack.GetCount(); i++)
		{
		int iAdj = m_Stack[i].GetDamageAdj(Damage);
		if (iAdj != 100)
			rValue = iAdj * rValue / 100.0;
		}

	if (rValue > CDamageAdjDesc::MAX_DAMAGE_ADJ)
		return CDamageAdjDesc::MAX_DAMAGE_ADJ;

	return (int)(rValue + 0.5);
	}

int CItemEnhancementStack::GetManeuverRate (void) const

//	GetManeuverRate
//
//	Returns the tracking maneuver rate.

	{
	int i;

	int iRate = 0;

	for (i = 0; i < m_Stack.GetCount(); i++)
		iRate = Max(iRate, m_Stack[i].GetManeuverRate());

	return iRate;
	}

int CItemEnhancementStack::GetPowerAdj (void) const

//	GetPowerAdj
//
//	Returns the power consumption adjustment

	{
	int i;

	Metric rValue = 100.0;
	for (i = 0; i < m_Stack.GetCount(); i++)
		{
		int iAdj = m_Stack[i].GetPowerAdj();
		if (iAdj != 100)
			rValue = iAdj * rValue / 100.0;
		}

	return (int)(rValue + 0.5);
	}

int CItemEnhancementStack::GetResistDamageAdj (DamageTypes iDamage) const

//	GetResistDamageAdj
//
//	Gets the damage adjustment for the given type

	{
	int i;

	DamageDesc Damage(iDamage, DiceRange(0, 0, 10));

	Metric rValue = 100.0;
	for (i = 0; i < m_Stack.GetCount(); i++)
		{
		int iAdj = m_Stack[i].GetDamageAdj(Damage);
		if (iAdj != 100)
			rValue = iAdj * rValue / 100.0;
		}

	return (int)(rValue + 0.5);
	}

int CItemEnhancementStack::GetResistEnergyAdj (void) const

//	GetResistEnergyAdj
//
//	Gets the damage adjustment for energy. We only return an adjustment if there
//	are no other damage adjustments (otherwise we return 100).

	{
	int i;

	Metric rValue = 100.0;
	for (i = 0; i < m_Stack.GetCount(); i++)
		{
		if (m_Stack[i].HasCustomDamageAdj())
			return 100;

		int iAdj = m_Stack[i].GetResistEnergyAdj();
		if (iAdj != 100)
			rValue = iAdj * rValue / 100.0;
		}

	return (int)(rValue + 0.5);
	}

int CItemEnhancementStack::GetResistMatterAdj (void) const

//	GetResistMatterAdj
//
//	Gets the damage adjustment for matter. We only return an adjustment if there
//	are no other damage adjustments (otherwise we return 100).

	{
	int i;

	Metric rValue = 100.0;
	for (i = 0; i < m_Stack.GetCount(); i++)
		{
		if (m_Stack[i].HasCustomDamageAdj())
			return 100;

		int iAdj = m_Stack[i].GetResistMatterAdj();
		if (iAdj != 100)
			rValue = iAdj * rValue / 100.0;
		}

	return (int)(rValue + 0.5);
	}

void CItemEnhancementStack::Insert (const CItemEnhancement &Mods)

//	Insert
//
//	Appends the given enhancement to the stack.

	{
	if (!Mods.IsEmpty())
		m_Stack.Insert(Mods);
	}

void CItemEnhancementStack::InsertActivateAdj (int iAdj, int iMin, int iMax)

//	InsertActivateAdj
//
//	Appends an activate adjustment enhancement.

	{
	m_Stack.InsertEmpty();
	m_Stack[m_Stack.GetCount() - 1].SetModSpeed(iAdj, iMin, iMax);
	}

void CItemEnhancementStack::InsertHPBonus (int iBonus)

//	InsertHPBonus
//
//	Appends an HP bonus enhancement.

	{
	m_Stack.InsertEmpty();
	m_Stack[m_Stack.GetCount() - 1].SetModBonus(iBonus);
	}

bool CItemEnhancementStack::IsBlindingImmune (void) const

//	IsBlindingImmune
//
//	Returns true if we're immune to blinding

	{
	int i;

	for (i = 0; i < m_Stack.GetCount(); i++)
		if (m_Stack[i].IsBlindingImmune())
			return true;

	return false;
	}

bool CItemEnhancementStack::IsDecaying (void) const

//	IsDecaying
//
//	Returns true if we're decaying

	{
	int i;

	for (i = 0; i < m_Stack.GetCount(); i++)
		if (m_Stack[i].IsDecaying())
			return true;

	return false;
	}

bool CItemEnhancementStack::IsDeviceDamageImmune (void) const

//	IsDeviceDamageImmune
//
//	Returns true if we're immune to device damage

	{
	int i;

	for (i = 0; i < m_Stack.GetCount(); i++)
		if (m_Stack[i].IsDeviceDamageImmune())
			return true;

	return false;
	}

bool CItemEnhancementStack::IsDisintegrationImmune (void) const

//	IsDisintegrationImmune
//
//	Returns true if we're immune to disintegration

	{
	int i;

	for (i = 0; i < m_Stack.GetCount(); i++)
		if (m_Stack[i].IsDisintegrationImmune())
			return true;

	return false;
	}

bool CItemEnhancementStack::IsEMPImmune (void) const

//	IsEMPImmune
//
//	Returns true if we're immune to EMP

	{
	int i;

	for (i = 0; i < m_Stack.GetCount(); i++)
		if (m_Stack[i].IsEMPImmune())
			return true;

	return false;
	}

bool CItemEnhancementStack::IsPhotoRecharging (void) const

//	IsPhotoRecharging
//
//	Returns true if we're photo recharging

	{
	int i;

	for (i = 0; i < m_Stack.GetCount(); i++)
		if (m_Stack[i].IsPhotoRecharge())
			return true;

	return false;
	}

bool CItemEnhancementStack::IsPhotoRegenerating (void) const

//	IsPhotoRegenerating
//
//	Returns true if we're photo regenerating

	{
	int i;

	for (i = 0; i < m_Stack.GetCount(); i++)
		if (m_Stack[i].IsPhotoRegenerating())
			return true;

	return false;
	}

bool CItemEnhancementStack::IsRadiationImmune (void) const

//	IsRadiationImmune
//
//	Returns true if we're immune to radiation

	{
	int i;

	for (i = 0; i < m_Stack.GetCount(); i++)
		if (m_Stack[i].IsRadiationImmune())
			return true;

	return false;
	}

bool CItemEnhancementStack::IsShatterImmune (void) const

//	IsShatterImmune
//
//	Returns true if we're immune to shatter

	{
	int i;

	for (i = 0; i < m_Stack.GetCount(); i++)
		if (m_Stack[i].IsShatterImmune())
			return true;

	return false;
	}

bool CItemEnhancementStack::IsShieldInterfering (void) const

//	IsShieldInterfering
//
//	Returns true if we're shield interfering

	{
	int i;

	for (i = 0; i < m_Stack.GetCount(); i++)
		if (m_Stack[i].IsShieldInterfering())
			return true;

	return false;
	}

TSharedPtr<CItemEnhancementStack> CItemEnhancementStack::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Reads from stream

	{
	int i;

	DWORD dwLoad;
	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	if (dwLoad == 0)
		return NULL;

	TSharedPtr<CItemEnhancementStack> pStack(new CItemEnhancementStack);
	pStack->m_Stack.InsertEmpty(dwLoad);

	for (i = 0; i < pStack->m_Stack.GetCount(); i++)
		pStack->m_Stack[i].ReadFromStream(Ctx);

	return pStack;
	}

bool CItemEnhancementStack::ReflectsDamage (DamageTypes iDamage, int *retiChance) const

//	ReflectsDamage
//
//	Returns TRUE if we reflect the given damage type

	{
	int i;

	if (retiChance)
		*retiChance = 0;

	for (i = 0; i < m_Stack.GetCount(); i++)
		{
		int iChance = m_Stack[i].GetReflectChance(iDamage);
		if (iChance > 0)
			{
			if (retiChance)
				*retiChance = Max(iChance, *retiChance);
			else
				return true;
			}
		}

	return (retiChance ? (*retiChance > 0) : false);
	}

bool CItemEnhancementStack::RepairOnDamage (DamageTypes iDamage) const

//	RepairOnDamage
//
//	Returns TRUE if we repair when hit with damage

	{
	int i;

	for (i = 0; i < m_Stack.GetCount(); i++)
		if (m_Stack[i].IsRepairOnDamage() && m_Stack[i].GetDamageType() == iDamage)
			return true;

	return false;
	}

bool CItemEnhancementStack::UpdateArmorRegen (CItemCtx &ArmorCtx, SUpdateCtx &UpdateCtx, int iTick) const

//	UpdateArmorRegen
//
//	Regenerates the armor if we have regen enhancements. Returns TRUE if we 
//	regenerated anything.

	{
	int i;
	bool bRegenerated = false;

	for (i = 0; i < m_Stack.GetCount(); i++)
		{
		if (m_Stack[i].UpdateArmorRegen(ArmorCtx, UpdateCtx, iTick))
			bRegenerated = true;
		}

	return bRegenerated;
	}

void CItemEnhancementStack::WriteToStream (CItemEnhancementStack *pStack, IWriteStream *pStream)

//	WriteToStream
//
//	Writes to stream
//
//	DWORD			Number of items in stack
//		CItemEnhancement

	{
	int i;

	if (pStack == NULL)
		{
		DWORD dwSave = 0;
		pStream->Write((char *)&dwSave, sizeof(DWORD));
		}
	else
		{
		DWORD dwSave = pStack->m_Stack.GetCount();
		pStream->Write((char *)&dwSave, sizeof(DWORD));
	
		for (i = 0; i < pStack->m_Stack.GetCount(); i++)
			pStack->m_Stack[i].WriteToStream(pStream);
		}
	}
