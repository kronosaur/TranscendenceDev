//	CItemEnhancement.cpp
//
//	Item enhancement class

#include "PreComp.h"

CItemEnhancement CItemEnhancement::m_Null;

void CItemEnhancement::AccumulateAttributes (const CItem &Item, TArray<SDisplayAttribute> *retList, DWORD dwFlags) const

//	AccumulateAttributes
//
//	Adds attributes

	{
	EDisplayAttributeTypes iDisplayType = (IsDisadvantage() ? attribNegative : attribPositive);

	switch (GetType())
		{
		case etNone:
			break;

		case etHPBonus:
		case etStrengthen:
			if (dwFlags & FLAG_INCLUDE_HP_BONUS)
				retList->Insert(SDisplayAttribute::FromHPBonus(GetHPBonus()));
			break;

		case etResist:
			if (dwFlags & FLAG_INCLUDE_HP_BONUS)
				retList->Insert(SDisplayAttribute::FromResistHPBonus(GetResistHPBonus(), CONSTLIT("vs all attacks")));
			break;

		case etResistEnergy:
			if (dwFlags & FLAG_INCLUDE_HP_BONUS)
				retList->Insert(SDisplayAttribute::FromResistHPBonus(GetResistHPBonus(), CONSTLIT("vs energy attacks")));
			break;

		case etResistMatter:
			if (dwFlags & FLAG_INCLUDE_HP_BONUS)
				retList->Insert(SDisplayAttribute::FromResistHPBonus(GetResistHPBonus(), CONSTLIT("vs matter attacks")));
			break;

		case etResistByDamage:
		case etResistHPBonus:
			if (dwFlags & FLAG_INCLUDE_HP_BONUS)
				retList->Insert(SDisplayAttribute::FromResistHPBonus(GetDamageTypeField(), GetResistHPBonus()));
			break;

		case etResistByLevel:
			if (dwFlags & FLAG_INCLUDE_HP_BONUS)
				{
				retList->Insert(SDisplayAttribute::FromResistHPBonus(GetDamageTypeField(), GetResistHPBonus()));
				retList->Insert(SDisplayAttribute::FromResistHPBonus((DamageTypes)(GetDamageTypeField() + 1), GetResistHPBonus()));
				}
			break;

		case etResistByDamage2:
			if (dwFlags & FLAG_INCLUDE_HP_BONUS)
				{
				retList->Insert(SDisplayAttribute::FromResistHPBonus(GetDamageTypeField(), GetResistHPBonus()));
				retList->Insert(SDisplayAttribute::FromResistHPBonus((DamageTypes)(GetDamageTypeField() + 2), GetResistHPBonus()));
				}
			break;

		case etRegenerate:
		case etHealerRegenerate:
			retList->Insert(SDisplayAttribute(iDisplayType, (IsDisadvantage() ? CONSTLIT("-decay") : CONSTLIT("+regen")), true));
			break;

		case etReflect:
			retList->Insert(SDisplayAttribute(iDisplayType, strPatternSubst((IsDisadvantage() ? CONSTLIT("-%s transparent") : CONSTLIT("+%s reflect")), ::GetDamageShortName(GetDamageTypeField())), true));
			break;

		case etRepairOnHit:
			retList->Insert(SDisplayAttribute(iDisplayType, strPatternSubst(CONSTLIT("+%s regen"), ::GetDamageShortName(GetDamageTypeField())), true));
			break;

		case etSpecialDamage:
			{
			switch (GetLevel2())
				{
				case specialRadiation:
					retList->Insert(SDisplayAttribute(iDisplayType, CONSTLIT("+radiation immune"), true));
					break;

				case specialBlinding:
					retList->Insert(SDisplayAttribute(iDisplayType, CONSTLIT("+blind immune"), true));
					break;

				case specialEMP:
					retList->Insert(SDisplayAttribute(iDisplayType, CONSTLIT("+EMP immune"), true));
					break;

				case specialDeviceDamage:
					retList->Insert(SDisplayAttribute(iDisplayType, CONSTLIT("+device protect"), true));
					break;

				case specialDisintegration:
					retList->Insert(SDisplayAttribute(iDisplayType, CONSTLIT("+disintegration immune"), true));
					break;

				default:
					retList->Insert(SDisplayAttribute(iDisplayType, CONSTLIT("+immune"), true));
					break;
				}

			break;
			}

		case etConferSpecialDamage:
			{
			int iLevel;
			SpecialDamageTypes iSpecial = GetSpecialDamage(&iLevel);
			switch (iSpecial)
				{
				case specialArmor:
					retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("+armor penetrate"), true));
					break;

				case specialBlinding:
					retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("+blinding"), true));
					break;

				case specialDeviceDamage:
					retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("+device damage"), true));
					break;

				case specialDeviceDisrupt:
					retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("+device ionize"), true));
					break;

				case specialDisintegration:
					retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("+disintegration"), true));
					break;

				case specialEMP:
					retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("+EMP"), true));
					break;

				case specialFuel:
					retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("+fuel drain"), true));
					break;

				case specialMining:
					retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("+mining"), true));
					break;

				case specialMomentum:
					retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("+momentum"), true));
					break;

				case specialRadiation:
					retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("+radiation"), true));
					break;

				case specialShatter:
					retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("+shatter"), true));
					break;

				case specialShieldDisrupt:
					retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("+shield buster"), true));
					break;

				case specialShieldPenetrator:
					retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("+shield penetrate"), true));
					break;

				case specialTimeStop:
					retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("+time stop"), true));
					break;

				case specialWMD:
					retList->Insert(SDisplayAttribute(attribPositive, strPatternSubst(CONSTLIT("+WMD %d"), DamageDesc::GetMassDestructionLevelFromValue(iLevel))));
					break;
				}

			break;
			}

		case etImmunityIonEffects:
			retList->Insert(SDisplayAttribute(iDisplayType, (IsDisadvantage() ? CONSTLIT("-no shields") : CONSTLIT("+ionize immune")), true));
			break;

		case etPhotoRegenerate:
			retList->Insert(SDisplayAttribute(iDisplayType, CONSTLIT("+photo-regen"), true));
			break;

		case etPhotoRecharge:
			retList->Insert(SDisplayAttribute(iDisplayType, CONSTLIT("+solar"), true));
			break;

		case etPowerEfficiency:
			retList->Insert(SDisplayAttribute(iDisplayType, (IsDisadvantage() ? CONSTLIT("-drain") : CONSTLIT("+efficient")), true));
			break;

		case etSpeed:
		case etSpeedOld:
			retList->Insert(SDisplayAttribute(iDisplayType, (IsDisadvantage() ? CONSTLIT("-slow") : CONSTLIT("+fast")), true));
			break;

		case etTracking:
			retList->Insert(SDisplayAttribute(iDisplayType, (IsDisadvantage() ? CONSTLIT("-blinded") : CONSTLIT("+tracking")), true));
			break;

		case etOmnidirectional:
			if (IsDisadvantage())
				retList->Insert(SDisplayAttribute(iDisplayType, CONSTLIT("-stuck"), true));
			else if (GetDataX() == 0)
				retList->Insert(SDisplayAttribute(iDisplayType, CONSTLIT("+omnidirectional"), true));
			else
				retList->Insert(SDisplayAttribute(iDisplayType, CONSTLIT("+swivel"), true));
			break;

		default:
			retList->Insert(SDisplayAttribute(iDisplayType, CONSTLIT("+unknown"), true));
			break;
		}

	//	Expiration

	if ((dwFlags & FLAG_INCLUDE_EXPIRATION)
			&& m_iExpireTime != -1)
		{
		int iLifetime = m_iExpireTime - (int)Item.GetUniverse().GetTicks();
		if (iLifetime > 0)
			retList->Insert(SDisplayAttribute(attribNeutral, 
						CLanguage::ComposeNumber(CLanguage::numberRealTimeTicks, iLifetime)
						));
		}
	}

int CItemEnhancement::DamageAdj2Level (int iDamageAdj)

//	DamageAdj2Level
//
//	Converts from a damage adjustment to a level (0-15)

	{
	if (iDamageAdj <= 100)
		return Min(9, 10 - (iDamageAdj + 5) / 10);
	else
		return Min(9, (100 - (10000 / (iDamageAdj + 5))) / 10);
	}

bool CItemEnhancement::CalcNewHPBonus (const CItem &Item, const CItemEnhancement &NewEnhancement, int *retiNewBonus) const

//	CalcNewHPBonus
//
//	Computes the new HP bonus after applying the given new enhancement. We 
//	return TRUE if the new enhancement is different from the current one.

	{
	int iCurBonus = GetHPBonus();

	//	If this is a stacking enhancement, then it's additive

	int iNewBonus;
	if (NewEnhancement.IsStacking())
		{
		int iAddedBonus = (NewEnhancement.IsDisadvantage() ? -10 : 10);
		iNewBonus = iCurBonus + iAddedBonus;
		}

	//	Otherwise, if we're a disadvantage, we only apply the bonus if we're
	//	worse than the current.

	else if (NewEnhancement.IsDisadvantage())
		{
		if (iCurBonus <= 0)
			iNewBonus = Min(NewEnhancement.GetHPBonus(), iCurBonus);
		else
			iNewBonus = 0;
		}

	//	If we're an advantage, then we only apply if we're better

	else
		{
		if (iCurBonus >= 0)
			iNewBonus = Max(NewEnhancement.GetHPBonus(), iCurBonus);
		else
			iNewBonus = 0;
		}

	//	Make sure we're still in range.

	int iMaxBonus = Item.GetType()->GetMaxHPBonus();
	if (iNewBonus < 0)
		iNewBonus = Max(iNewBonus, -iMaxBonus);
	else
		iNewBonus = Min(iNewBonus, iMaxBonus);

	//	Done

	if (retiNewBonus)
		*retiNewBonus = iNewBonus;

	return (iNewBonus != iCurBonus);
	}

bool CItemEnhancement::CalcRegen (CItemCtx &ItemCtx, int iTicksPerUpdate, CRegenDesc &retRegen, ERegenTypes *retiType) const

//	CalcRegen
//
//	Initializes a regen structure (or return FALSE if no regen).

	{
	ERegenTypes iRegenType;

	//	Figure out how much we regenerate

	switch (GetType())
		{
		case etPhotoRegenerate:
			{
			//	Skip disadvantages (decay)

			if (IsDisadvantage())
				return false;

			//	Standard regeneration is 1% of standard armor HP per 180 ticks

			iRegenType = regenSolar;
			retRegen.InitFromRegen(0.01 * CArmorClass::GetStdHP(ItemCtx.GetItem().GetLevel()), iTicksPerUpdate);
			break;
			}

		case etRegenerate:
		case etHealerRegenerate:
			{
			//	Skip disadvantages (decay)

			if (IsDisadvantage())
				return false;

			if (GetType() == etHealerRegenerate)
				iRegenType = regenFromHealer;
			else
				iRegenType = regenStandard;

			//	Get the regen rate.

			int iRegen = GetDataX();

			//	If regen rate is 0, then we use standard regeneration, which is
			//	1% of standard armor HP per 180 ticks.

			if (iRegen == 0)
				retRegen.InitFromRegen(0.01 * CArmorClass::GetStdHP(ItemCtx.GetItem().GetLevel()), iTicksPerUpdate);

			//	Otherwise it is specified

			else
				retRegen.InitFromRegen(iRegen, iTicksPerUpdate);

			break;
			}

		default:
			return false;
		}

	//	Success

	if (retiType)
		*retiType = iRegenType;

	return true;
	}

bool CItemEnhancement::CanBeCombinedWith (const CItemEnhancement &NewEnhancement) const

//	CanBeCombinedWith
//
//	Returns TRUE if we can combine the new enhancement with ours. That is, if
//	the two types are compatible.

	{
	//	Equal enhancements are always compatible

	if (GetType() == NewEnhancement.GetType())
		return true;

	//	If we don't have an enhancement, then we're always compatible.

	else if (GetType() == etNone || NewEnhancement.GetType() == etNone)
		return true;

	//	Regeneration can cure any disadvantage

	else if ((NewEnhancement.GetType() == etRegenerate || NewEnhancement.GetType() == etHealerRegenerate)
			&& !NewEnhancement.IsDisadvantage() && IsDisadvantage())
		return true;

	//	Otherwise, it depends on the enhancement type

	else
		{
		switch (GetType())
			{
			case etStrengthen:
			case etHPBonus:
				return (NewEnhancement.GetType() == etStrengthen || NewEnhancement.GetType() == etHPBonus);

			case etSpeed:
			case etSpeedOld:
				return (NewEnhancement.GetType() == etSpeed || NewEnhancement.GetType() == etSpeedOld);

			default:
				return false;
			}
		}
	}

EnhanceItemStatus CItemEnhancement::Combine (const CItem &Item, const CItemEnhancement &Enhancement)

//	Combine
//
//	Combine the current enhancement with the given one

	{
	DWORD dwNewMods = Enhancement.m_dwMods;

	//	If we're losing the enhancement, then clear it

	if (dwNewMods == etLoseEnhancement)
		{
		if (IsEnhancement())
			{
			*this = CItemEnhancement();
			return eisEnhancementRemoved;
			}
		else
			return eisNoEffect;
		}

	//	If the item is not currently enhanced, then we take the new enhancement

	else if (m_dwMods == etNone)
		{
		//	For strength/hpBonus, use the following rules:
		//
		//	etStrengthen 0 -> Min(10%, maxHPBonus)
		//	etStrengthen {level} -> Min( {level} * 10, maxHPBonus )

		if (Enhancement.GetType() == etStrengthen
					|| Enhancement.GetType() == etHPBonus)
			{
			int iNewBonus;
			if (CalcNewHPBonus(Item, Enhancement, &iNewBonus))
				{
				*this = Enhancement;	//	Take expireTime, etc.
				SetModBonus(iNewBonus);
				return eisOK;
				}
			else
				return eisNoEffect;
			}

		//	For all others, take the enhancement

		else
			*this = Enhancement;

		return eisOK;
		}

	//	If already enhanced

	else if (m_dwMods == dwNewMods)
		{
		if (IsDisadvantage())
			return eisNoEffect;
		else
			return eisAlreadyEnhanced;
		}

	//	We handle each permutation separately.

	else if (IsDisadvantage())
		{
		if (Enhancement.IsDisadvantage())
			{
			if (CanBeCombinedWith(Enhancement))
				return CombineDisadvantageWithDisadvantage(Item, Enhancement);
			else
				return eisNoEffect;
			}
		else
			{
			if (CanBeCombinedWith(Enhancement))
				return CombineDisadvantageWithAdvantage(Item, Enhancement);
			else
				return eisNoEffect;
			}
		}
	else
		{
		if (Enhancement.IsDisadvantage())
			{
			if (CanBeCombinedWith(Enhancement))
				return CombineAdvantageWithDisadvantage(Item, Enhancement);
			else
				{
				if (!CombineExpireTime(Enhancement))
					return eisNoEffect;
				else
					{
					//	A disadvantage removes an advantage

					*this = CItemEnhancement();
					return eisEnhancementRemoved;
					}
				}
			}
		else
			{
			if (CanBeCombinedWith(Enhancement))
				return CombineAdvantageWithAdvantage(Item, Enhancement);
			else
				return eisNoEffect;
			}
		}
	}

EnhanceItemStatus CItemEnhancement::CombineAdvantageWithAdvantage (const CItem &Item, const CItemEnhancement &Enhancement)

//	CombineAdvantageWithAdvantage
//
//	We've already got some enhancement and we're applying a new one.
//	NOTE: We assume the two enhancements are compatible.

	{
	switch (Enhancement.GetType())
		{
		case etHPBonus:
		case etStrengthen:
			{
			int iNewBonus;
			if (CalcNewHPBonus(Item, Enhancement, &iNewBonus))
				{
				if (!CombineExpireTime(Enhancement))
					return eisNoEffect;

				SetModBonus(iNewBonus);
				return eisBetter;
				}
			else
				return eisNoEffect;
			}

		//	If this is the same type of enhancement and it is better,
		//	then take it (otherwise, no effect)

		case etRegenerate:
		case etHealerRegenerate:
		case etResist:
		case etResistEnergy:
		case etResistMatter:
		case etPowerEfficiency:
			{
			if (Enhancement.GetLevel() > GetLevel())
				{
				if (!CombineExpireTime(Enhancement))
					return eisNoEffect;

				m_dwMods = Enhancement.m_dwMods;
				return eisBetter;
				}
			else
				return eisNoEffect;
			}

		case etResistHPBonus:
			{
			if (Enhancement.GetDataX() > GetDataX())
				{
				if (!CombineExpireTime(Enhancement))
					return eisNoEffect;

				m_dwMods = Enhancement.m_dwMods;
				return eisBetter;
				}
			else
				return eisNoEffect;
			}

		case etSpeed:
		case etSpeedOld:
			{
			if (Enhancement.GetActivateRateAdj() < GetActivateRateAdj())
				{
				if (!CombineExpireTime(Enhancement))
					return eisNoEffect;

				m_dwMods = Enhancement.m_dwMods;
				return eisBetter;
				}
			else
				return eisNoEffect;
			}

		case etResistByLevel:
		case etResistByDamage:
		case etResistByDamage2:
			{
			if (Enhancement.GetDamageTypeField() != GetDamageTypeField())
				{
				if (!CombineExpireTime(Enhancement))
					return eisNoEffect;

				m_dwMods = Enhancement.m_dwMods;
				return eisEnhancementReplaced;
				}
			else if (Enhancement.GetLevel() > GetLevel())
				{
				if (!CombineExpireTime(Enhancement))
					return eisNoEffect;

				m_dwMods = Enhancement.m_dwMods;
				return eisBetter;
				}
			else
				return eisNoEffect;
			}

		case etTracking:
			{
			if (Enhancement.GetDataX() > GetDataX())
				{
				if (!CombineExpireTime(Enhancement))
					return eisNoEffect;

				m_dwMods = Enhancement.m_dwMods;
				return eisBetter;
				}
			else
				return eisNoEffect;
			}

		case etOmnidirectional:
			{
			if (Enhancement.GetDataX() > GetDataX())
				{
				if (!CombineExpireTime(Enhancement))
					return eisNoEffect;

				m_dwMods = Enhancement.m_dwMods;
				return eisBetter;
				}
			else
				return eisNoEffect;
			}

		default:
			return eisNoEffect;
		}
	}

EnhanceItemStatus CItemEnhancement::CombineAdvantageWithDisadvantage (const CItem &Item, const CItemEnhancement &Enhancement)

//	CombineAdvantageWithDisadvantage
//
//	We've got some enhancement and are applying a disadvantage

	{
	switch (Enhancement.GetType())
		{
		case etHPBonus:
		case etStrengthen:
			{
			int iNewBonus;
			if (CalcNewHPBonus(Item, Enhancement, &iNewBonus))
				{
				if (!CombineExpireTime(Enhancement))
					return eisNoEffect;

				SetModBonus(iNewBonus);
				return eisWorse;
				}
			else
				return eisNoEffect;
			}

		default:
			return eisNoEffect;
		}
	}

EnhanceItemStatus CItemEnhancement::CombineDisadvantageWithAdvantage (const CItem &Item, const CItemEnhancement &Enhancement)

//	CombineDisadvantageWithAdvantage
//
//	We've got a disadvantage and are applying an enhancement.

	{
	switch (Enhancement.GetType())
		{
		//	Regeneration enhancement always repairs a disadvantage

		case etRegenerate:
		case etHealerRegenerate:
			{
			if (!CombineExpireTime(Enhancement))
				return eisNoEffect;

			m_dwMods = Enhancement.m_dwMods;
			return eisRepaired;
			}

		//	If the enhancement is the opposite of the disadvantage
		//	then the disadvantage is repaired.

		case etResist:
		case etResistEnergy:
		case etResistMatter:
		case etPowerEfficiency:
		case etResistByLevel:
		case etResistByDamage:
		case etResistByDamage2:
		case etResistHPBonus:
		case etReflect:
		case etSpeed:
		case etSpeedOld:
			{
			if (!CombineExpireTime(Enhancement))
				return eisNoEffect;

			m_dwMods = Enhancement.m_dwMods;
			return eisRepaired;
			}

		case etHPBonus:
		case etStrengthen:
			{
			int iNewBonus;
			if (CalcNewHPBonus(Item, Enhancement, &iNewBonus))
				{
				if (!CombineExpireTime(Enhancement))
					return eisNoEffect;

				SetModBonus(iNewBonus);
				return (GetHPBonus() >= 0 ? eisRepaired : eisBetter);
				}
			else
				return eisNoEffect;
			}

		//	Otherwise, no effect

		default:
			return eisNoEffect;
		}
	}

EnhanceItemStatus CItemEnhancement::CombineDisadvantageWithDisadvantage (const CItem &Item, const CItemEnhancement &Enhancement)

//	CombineDisadvantageWithDisadvantage
//
//	We've got a disadvantage and are applying another one.

	{
	switch (Enhancement.GetType())
		{
		//	If we're making the disadvantage worse, then
		//	continue; otherwise, no effect.

		case etRegenerate:
		case etHealerRegenerate:
		case etResist:
		case etResistEnergy:
		case etResistMatter:
		case etPowerEfficiency:
			{
			if (Enhancement.GetLevel() > GetLevel())
				{
				if (!CombineExpireTime(Enhancement))
					return eisNoEffect;

				m_dwMods = Enhancement.m_dwMods;
				return eisWorse;
				}
			else
				return eisNoEffect;
			}

		case etResistHPBonus:
			{
			if (Enhancement.GetDataX() > GetDataX())
				{
				if (!CombineExpireTime(Enhancement))
					return eisNoEffect;

				m_dwMods = Enhancement.m_dwMods;
				return eisWorse;
				}
			else
				return eisNoEffect;
			}

		case etHPBonus:
		case etStrengthen:
			{
			int iNewBonus;
			if (CalcNewHPBonus(Item, Enhancement, &iNewBonus))
				{
				if (!CombineExpireTime(Enhancement))
					return eisNoEffect;

				SetModBonus(iNewBonus);
				return eisWorse;
				}
			else
				return eisNoEffect;
			}

		case etSpeed:
		case etSpeedOld:
			{
			if (Enhancement.GetActivateRateAdj() > GetActivateRateAdj())
				{
				if (!CombineExpireTime(Enhancement))
					return eisNoEffect;

				m_dwMods = Enhancement.m_dwMods;
				return eisWorse;
				}
			else
				return eisNoEffect;
			}

		case etResistByLevel:
		case etResistByDamage:
		case etResistByDamage2:
			{
			if (Enhancement.GetDamageTypeField() == GetDamageTypeField()
					&& Enhancement.GetLevel() > GetLevel())
				{
				if (!CombineExpireTime(Enhancement))
					return eisNoEffect;

				m_dwMods = Enhancement.m_dwMods;
				return eisWorse;
				}
			else
				return eisNoEffect;
			}

		//	Otherwise, a disadvantage does not affect a disadvantage

		default:
			return eisNoEffect;
		}
	}

bool CItemEnhancement::CombineExpireTime (const CItemEnhancement &Enhancement)

//	CombineExpireTime
//
//	Combines expiration time. Returns FALSE if we cannot combine the given 
//	enhancement.

	{
	//	If neither old or new has an expiration time, then we're fine.

	if (m_iExpireTime == -1 && Enhancement.m_iExpireTime == -1)
		;

	//	If new has an expiration time, then we cannot combine with a permanent
	//	enhancement.

	else if (m_iExpireTime == -1)
		return false;

	//	If old has an expiration time, then we become permanent.

	else if (Enhancement.m_iExpireTime == -1)
		m_iExpireTime = -1;

	//	If both have an expiration time, then we take the longest time.

	else
		m_iExpireTime = Max(m_iExpireTime, Enhancement.m_iExpireTime);

	//	Always take the enhancer from the new one, if we have one.

	if (Enhancement.m_pEnhancer)
		m_pEnhancer = Enhancement.m_pEnhancer;

	return true;
	}

DWORD CItemEnhancement::EncodeABC (DWORD dwTypeCode, int A, int B, int C)

//	EncodeABC
//
//	Encodes a mod with data A, B, and C.

	{
	DWORD dwDataA = Max(0, Min(A, (int)etDataAMax));
	DWORD dwDataB = Max(0, Min(B, (int)etDataBMax));
	DWORD dwDataC = Max(0, Min(B, (int)etDataCMax));
	return dwTypeCode
			| dwDataA
			| (dwDataB << 16)
			| (dwDataC << 24);
	}

DWORD CItemEnhancement::EncodeAX (DWORD dwTypeCode, int A, int X)

//	EncodeAX
//
//	Encodes a mod with data A and X.

	{
	DWORD dwDataA = Max(0, Min(A, (int)etDataAMax));
	DWORD dwDataX = Max(0, Min(X, (int)etDataXMax));
	return dwTypeCode
			| dwDataA
			| (dwDataX << 16);
	}

DWORD CItemEnhancement::EncodeDX (DWORD dwTypeCode, DamageTypes iDamageType, int X)

//	EncodeDX
//
//	Encodes a mod with a damage type and X.

	{
	DWORD dwData1 = 0;
	DWORD dwData2 = Max(0, Min((int)iDamageType, (int)0xF));
	DWORD dwDataX = Max(0, Min(X, (int)etDataXMax));
	return dwTypeCode
			| dwData1
			| (dwData2 << 4)
			| (dwDataX << 16);
	}

DWORD CItemEnhancement::Encode12 (DWORD dwTypeCode, int Data1, int Data2)

//	Encode12
//
//	Encodes a mod with data 1 and 2.

	{
	DWORD dwData1 = Max(0, Min(Data1, (int)0xF));
	DWORD dwData2 = Max(0, Min(Data2, (int)0xF));
	return dwTypeCode
			| dwData1
			| (dwData2 << 4);
	}

int CItemEnhancement::GetAbsorbAdj (const DamageDesc &Damage) const

//	GetAbsorbAdj
//
//	Returns damage absorbed

	{
	switch (GetType())
		{
		case etReflect:
			{
			if (IsDisadvantage() && Damage.GetDamageType() == GetDamageTypeField())
				return Level2DamageAdj(GetLevel());
			else
				return 100;
			}

		default:
			return 100;
		}
	}

int CItemEnhancement::GetDamageAdj (void) const

//	GetDamageAdj
//
//	Returns the damage adjustment confered by this mod

	{
	switch (GetType())
		{
		case etResist:
		case etResistEnergy:
		case etResistMatter:
		case etResistByLevel:
		case etResistByDamage:
		case etResistByDamage2:
			return Level2DamageAdj(GetLevel(), IsDisadvantage());

		case etResistHPBonus:
			if (IsDisadvantage())
				return CDamageAdjDesc::GetDamageAdjFromHPBonus(-GetDataX());
			else
				return CDamageAdjDesc::GetDamageAdjFromHPBonus(GetDataX());

		default:
			return 100;
		}
	}

int CItemEnhancement::GetDamageAdj (const DamageDesc &Damage) const

//	GetDamageAdj
//
//	Returns the damage adjustment confered by this mod

	{
	switch (GetType())
		{
		case etResist:
			return Level2DamageAdj(GetLevel(), IsDisadvantage());

		case etResistEnergy:
			return (Damage.IsEnergyDamage() ? Level2DamageAdj(GetLevel(), IsDisadvantage()) : 100);

		case etResistMatter:
			return (Damage.IsMatterDamage() ? Level2DamageAdj(GetLevel(), IsDisadvantage()) : 100);

		case etResistByLevel:
			{
			if (Damage.GetDamageType() == GetDamageTypeField()
					|| Damage.GetDamageType() == GetDamageTypeField() + 1)
				return Level2DamageAdj(GetLevel(), IsDisadvantage());
			else
				return 100;
			}

		case etResistByDamage:
			return (Damage.GetDamageType() == GetDamageTypeField() ? Level2DamageAdj(GetLevel(), IsDisadvantage()) : 100);

		case etResistByDamage2:
			{
			if (Damage.GetDamageType() == GetDamageTypeField())
				//	0 = 100			100
				//	1 = 90			111
				//	2 = 80			125
				//	3 = 70			143
				return Level2DamageAdj(GetLevel(), IsDisadvantage());
			else if (Damage.GetDamageType() == GetDamageTypeField() + 2)
				//	0 = 100			100
				//	1 = 95			105
				//	2 = 90			112
				//	3 = 85			121
				return 100 + ((Level2DamageAdj(GetLevel(), IsDisadvantage()) - 100) / 2);
			else
				return 100;
			}

		case etResistHPBonus:
			if (Damage.GetDamageType() != GetDamageTypeField())
				return 100;
			else if (IsDisadvantage())
				return CDamageAdjDesc::GetDamageAdjFromHPBonus(-GetDataX());
			else
				return CDamageAdjDesc::GetDamageAdjFromHPBonus(GetDataX());

		default:
			return 100;
		}
	}

int CItemEnhancement::GetHPBonus (void) const

//	GetHPBonus
//
//	Returns the damage bonus for the weapon, in percentage points.
//	(e.g., 10 = +10%).

	{
	switch (GetType())
		{
		case etHPBonus:
			return (IsDisadvantage() ? -GetDataX() : GetDataX());

		case etStrengthen:
			return Level2Bonus(GetLevel(), IsDisadvantage());

		default:
			return 0;
		}
	}

int CItemEnhancement::GetActivateRateAdj (int *retiMinDelay, int *retiMaxDelay) const

//	GetActivateRateAdj
//
//	Returns the adj to activate/fire delay

	{
	int iAdj = 100;
	int iMinDelay = 0;
	int iMaxDelay = 0;

	switch (GetType())
		{
		case etSpeed:
			if (IsDisadvantage())
				iAdj = 100 + GetDataA() * 5;
			else
				iAdj = GetDataA();

			iMinDelay = GetDataB();
			iMaxDelay = GetDataC();
			break;

		case etSpeedOld:
			iAdj = Level2DamageAdj(GetLevel(), IsDisadvantage());
			break;

		default:
			iAdj = 100;
		}

	if (retiMinDelay)
		*retiMinDelay = iMinDelay;

	if (retiMaxDelay)
		*retiMaxDelay = iMaxDelay;

	return iAdj;
	}

DamageTypes CItemEnhancement::GetDamageType (void) const

//	GetDamageType
//
//	Returns the encoded damage type, if valid.

	{
	switch (GetType())
		{
		case etReflect:
		case etRepairOnHit:
		case etResistByLevel:
		case etResistByDamage:
		case etResistByDamage2:
		case etResistHPBonus:
			return GetDamageTypeField();

		default:
			return damageError;
		}
	}

int CItemEnhancement::GetEnhancedRate (int iRate) const

//	GetEnhancedRate
//
//	Returns the enhanced rate

	{
	switch (GetType())
		{
		case etSpeed:
			{
			int iMin;
			int iMax;
			int iAdj = GetActivateRateAdj(&iMin, &iMax);
			int iNewRate = Max(iMin, (iRate * iAdj + 50) / 100);
			if (iMax != 0 && iNewRate > iMax)
				return iMax;
			else
				return iNewRate;
			}

		case etSpeedOld:
			return Max(1, (iRate * Level2DamageAdj(GetLevel(), IsDisadvantage()) + 50) / 100);

		default:
			return iRate;
		}
	}

int CItemEnhancement::GetFireArc (void) const

//	GetFireArc
//
//	Returns a fire arc (0 - 360).

	{
	switch (GetType())
		{
		case etOmnidirectional:
			if (IsDisadvantage())
				return -1;
			else if (GetDataX() == 0)
				return 360;
			else
				return GetDataX();

		default:
			return 0;
		}
	}

int CItemEnhancement::GetHPAdj (void) const

//	GetHPAdj
//
//	Get increase/decrease in HP

	{
	switch (GetType())
		{
		case etHPBonus:
			{
			int iData = GetDataX();

			if (IsDisadvantage())
				{
				if (iData >= 0 && iData <= 90)
					return 100 - iData;
				else
					return 10;
				}
			else
				return 100 + iData;
			}

		case etStrengthen:
			{
			int iLevel = GetLevel();

			if (IsDisadvantage())
				{
				if (iLevel >= 0 && iLevel <= 9)
					return 100 - (10 * iLevel);
				else
					return 10;
				}
			else
				return 100 + (10 * iLevel);
			}

		default:
			return 100;
		}
	}

int CItemEnhancement::GetManeuverRate (void) const

//	GetManeuverRate
//
//	Returns the tracking maneuver rate.

	{
	switch (GetType())
		{
		case etTracking:
			return GetDataX();

		default:
			return 0;
		}
	}

int CItemEnhancement::GetPowerAdj (void) const

//	GetPowerAdj
//
//	Get the increase/decrease in power usage

	{
	switch (GetType())
		{
		case etPowerEfficiency:
			{
			int iLevel = GetLevel();

			if (IsDisadvantage())
				return 100 + (10 * iLevel);
			else
				{
				if (iLevel >= 0 && iLevel <= 9)
					return 100 - (10 * iLevel);
				else
					return 10;
				}
			}

		default:
			return 100;
		}
	}

int CItemEnhancement::GetReflectChance (DamageTypes iDamage) const

//	GetReflectChance
//
//	Returns chance of reflecting

	{
	switch (GetType())
		{
		case etReflect:
			{
			if (!IsDisadvantage() && iDamage == GetDamageTypeField())
				return 50 + (GetLevel() * 5);
			else
				return 0;
			}

		default:
			return 0;
		}
	}

Metric CItemEnhancement::GetRegen180 (CItemCtx &Ctx, int iTicksPerUpdate) const

//	GetRegen180
//
//	Returns the number of HP regenerated per 180 ticks.

	{
	CRegenDesc Regen;

	if (!CalcRegen(Ctx, iTicksPerUpdate, Regen))
		return 0.0;

	return Regen.GetHPPer180(iTicksPerUpdate);
	}

int CItemEnhancement::GetResistHPBonus (void) const

//	GetResistHPBonus
//
//	Returns the damage adjustment confered by this mod

	{
	switch (GetType())
		{
		case etResist:
		case etResistEnergy:
		case etResistMatter:
		case etResistByLevel:
		case etResistByDamage:
		case etResistByDamage2:
			return CDamageAdjDesc::GetBonusFromAdj(Level2DamageAdj(GetLevel(), IsDisadvantage()));

		case etResistHPBonus:
			return (IsDisadvantage() ? -GetDataX() : GetDataX());

		default:
			return 0;
		}
	}

SpecialDamageTypes CItemEnhancement::GetSpecialDamage (int *retiLevel) const

//	GetSpecialDamage
//
//	Returns special damage enhancement type (or specialNone)

	{
	SpecialDamageTypes iSpecial;
	int iLevel;

	switch (GetType())
		{
		case etSpecialDamage:
			iSpecial = (SpecialDamageTypes)GetLevel2();
			iLevel = 0;
			break;

		case etConferSpecialDamage:
			iSpecial = (SpecialDamageTypes)GetDataA();
			iLevel = GetDataB();
			break;

		default:
			iSpecial = specialNone;
			iLevel = 0;
		}

	if (retiLevel)
		*retiLevel = iLevel;

	return iSpecial;
	}

int CItemEnhancement::GetValueAdj (const CItem &Item) const

//	GetValueAdj
//
//	Returns adjustment to item value based on modification

	{
	if (IsDisadvantage())
		{
		switch (GetType())
			{
			case etHPBonus:
				return Max(-80, -GetDataX());

			case etStrengthen:
			case etResistByLevel:
			case etResistByDamage:
			case etResistByDamage2:
			case etSpeedOld:
				return Max(-80, -10 * GetLevel());

			case etSpeed:
				return Max(-80, -3 * GetDataA());

			case etResist:
			case etResistMatter:
			case etResistEnergy:
				return Max(-80, -20 * GetLevel());

			case etResistHPBonus:
				return Max(-80, -GetDataX());

			case etPhotoRegenerate:
			case etPhotoRecharge:
			case etRepairOnHit:
			case etPowerEfficiency:
				return -50;

			case etRegenerate:
			case etHealerRegenerate:
			case etReflect:
			case etSpecialDamage:
			case etImmunityIonEffects:
				return -80;

			default:
				return 0;
			}
		}
	else
		{
		switch (GetType())
			{
			case etHPBonus:
				return 2 * GetDataX();

			case etStrengthen:
				return 20 * GetLevel();

			case etPowerEfficiency:
				return 15;

			case etResistByLevel:
			case etResistByDamage:
			case etResistByDamage2:
			case etSpeedOld:
				return 10 * GetLevel();

			case etSpeed:
				return 2 * (100 - GetDataA());

			case etResist:
			case etResistMatter:
			case etResistEnergy:
				return 50 * GetLevel();

			case etResistHPBonus:
				return GetDataX();

			case etPhotoRegenerate:
			case etPhotoRecharge:
			case etRepairOnHit:
				return 30;

			case etRegenerate:
			case etHealerRegenerate:
			case etReflect:
			case etSpecialDamage:
			case etImmunityIonEffects:
			case etTracking:
			case etOmnidirectional:
				return 100;

			default:
				return 0;
			}
		}
	}

bool CItemEnhancement::HasCustomDamageAdj (void) const

//	HasCustomDamageAdj
//
//	Returns TRUE if we have a damage adjustment that's not one of the generic
//	energy/matter adjustments.

	{
	switch (GetType())
		{
		case etResistByLevel:
		case etResistByDamage:
		case etResistByDamage2:
		case etResistHPBonus:
			return true;

		default:
			return false;
		}
	}

ALERROR CItemEnhancement::InitFromDesc (const CString &sDesc, CString *retsError)

//	InitFromDesc
//
//	Initializes from a string of the following forms:
//
//	{number}					Interpret as a mod code
//	+armor:{n}					Add	armor special damage, where n is an item level
//	+efficient:{n}				+/- n% power use.
//									E.g., +efficient:20 = (100 - 20) = 80% power use
//	+hpBonus:{n}				Add hp bonus.
//	+hpBonus:{s}:{n}			DamageAdj for type s set to hpBonus n
//	+immunity:{s}				Immunity to special damage s.
//	+reflect:{s}				Reflects damage type s.
//	+regen						Regenerate
//	+regen:{n}					Regenerate
//	+resist:{s}:{n}				DamageAdj for type s set to n
//	+resistDamageClass:{s}:{n}	DamageAdj for type s (and its next-tier mate) set to n
//	+resistDamageTier:{s}:{n}	DamageAdj for type s (and its tier mate) set to n
//	+resistEnergy:{n}			DamageAdj for energy damage set to n
//	+resistMatter:{n}			DamageAdj for matter damage set to n
//	+shield:{n}					Add shield disrupt special damage, where n is an item level
//	+speed:{n}					Faster. n is new delay value as a percent of normal
//	+tracking:{n}				n is maneuver rate

	{
	//	Initialize

	*this = CItemEnhancement();

	//	If the string is a number then we interpret it as a mod code.

	bool bFailed;
	DWORD dwCode = strToInt(sDesc, 0, &bFailed);
	if (!bFailed)
		{
		m_dwMods = dwCode;
		return NOERROR;
		}

	//	Parse the string

	const char *pPos = sDesc.GetASCIIZPointer();

	//	Expect either "+" or "-" (for disadvantage)

	bool bDisadvantage;
	if (*pPos == '+')
		bDisadvantage = false;
	else if (*pPos == '-')
		bDisadvantage = true;
	else
		{
		if (retsError)
			*retsError = CONSTLIT("Invalid enhancement description: expected '+' or '-'.");
		return ERR_FAIL;
		}

	pPos++;

	//	Parse the enhancement name

	const char *pStart = pPos;
	while (*pPos != ':' && *pPos != '\0')
		pPos++;

	CString sID(pStart, (int)(pPos - pStart));

	//	See if we have a value

	int iValue = 0;
	CString sValue;
	if (*pPos == ':')
		{
		pPos++;

		if (*pPos == '-' || *pPos == '+' || (*pPos >= '0' && *pPos <= '9'))
			iValue = strParseInt(pPos, 0, &pPos);
		else
			{
			const char *pStart = pPos;
			while (*pPos != '\0' && *pPos != ':')
				pPos++;

			sValue = CString(pStart, (int)(pPos - pStart));
			}
		}

	//	See if we have a second value

	int iValue2 = 0;
	if (*pPos == ':')
		{
		pPos++;
		iValue2 = strParseInt(pPos, 0, &pPos);
		}

	//	See if this is an hpBonus

	if (strEquals(sID, CONSTLIT("hpBonus")))
		{
		//	If we have a string for a first value, then this is a damage 
		//	adjustment.

		if (!sValue.IsBlank())
			{
			DamageTypes iDamageType = LoadDamageTypeFromXML(sValue);
			if (iDamageType == damageError)
				{
				if (retsError)
					*retsError = strPatternSubst(CONSTLIT("Invalid damage type: %s"), sValue);
				return ERR_FAIL;
				}

			if (bDisadvantage && iValue2 > 0)
				iValue2 = -iValue2;

			//	Convert the bonus value to a damage adjustment.

			if (iValue2 < -100)
				{
				if (retsError)
					*retsError = CONSTLIT("hpBonus penalty cannot exceed 100%.");
				return ERR_FAIL;
				}
			else if (iValue2 > 1000)
				{
				if (retsError)
					*retsError = CONSTLIT("hpBonus cannot exceed 1000%.");
				return ERR_FAIL;
				}
			else
				SetModResistHPBonus(iDamageType, iValue2);
			}

		//	Otherwise, we have a single number, which means a total HP bonus

		else
			{
			if (bDisadvantage && iValue > 0)
				iValue = -iValue;

			if (iValue < -100)
				{
				if (retsError)
					*retsError = CONSTLIT("hpBonus penalty cannot exceed 100%.");
				return ERR_FAIL;
				}
			else if (iValue < 0)
				SetModCode(EncodeAX(etHPBonus | etDisadvantage, 0, -iValue));
			else if (iValue == 0)
				SetModCode(Encode12(etStrengthen));
			else if (iValue <= 1000)
				SetModCode(EncodeAX(etHPBonus, 0, iValue));
			else
				{
				if (retsError)
					*retsError = CONSTLIT("hpBonus cannot exceed 1000%.");
				return ERR_FAIL;
				}
			}
		}

	//	Immunity

	else if (strEquals(sID, CONSTLIT("immunity")))
		{
		if (strEquals(sValue, CONSTLIT("ionEffects")))
			m_dwMods = Encode12(etImmunityIonEffects);
		else
			{
			SpecialDamageTypes iSpecial = DamageDesc::ConvertToSpecialDamageTypes(sValue);
			switch (iSpecial)
				{
				case specialBlinding:
				case specialDeviceDamage:
				case specialDeviceDisrupt:
				case specialDisintegration:
				case specialEMP:
				case specialMomentum:
				case specialRadiation:
				case specialShatter:
				case specialShieldDisrupt:
					{
					if (bDisadvantage)
						{
						if (retsError)
							*retsError = CONSTLIT("Disadvantage not supported.");
						return ERR_FAIL;
						}

					SetModImmunity(iSpecial);
					break;
					}

				default:
					{
					if (retsError)
						*retsError = strPatternSubst(CONSTLIT("Invalid immunity: %s"), sID);
					return ERR_FAIL;
					}
				}
			}
		}

	//	Reflect bonus

	else if (strEquals(sID, CONSTLIT("reflect")))
		{
		DamageTypes iDamageType = LoadDamageTypeFromXML(sValue);
		if (iDamageType == damageError)
			{
			if (retsError)
				*retsError = strPatternSubst(CONSTLIT("Invalid damage type: %s"), sValue);
			return ERR_FAIL;
			}

		SetModReflect(iDamageType);
		}

	//	Regen

	else if (strEquals(sID, CONSTLIT("regen")))
		{
		m_dwMods = EncodeAX(etRegenerate | (bDisadvantage ? etDisadvantage : 0), 0, iValue);
		}
	else if (strEquals(sID, CONSTLIT("healerRegen")))
		{
		m_dwMods = EncodeAX(etHealerRegenerate | (bDisadvantage ? etDisadvantage : 0), 0, iValue);
		}

	//	Decay

	else if (strEquals(sID, CONSTLIT("decay")))
		{
		m_dwMods = Encode12(etRegenerate | etDisadvantage);
		}

	//	Resist damage

	else if (strEquals(sID, CONSTLIT("resist")))
		{
		DamageTypes iDamageType = LoadDamageTypeFromXML(sValue);
		if (iDamageType == damageError)
			{
			if (retsError)
				*retsError = strPatternSubst(CONSTLIT("Invalid damage type: %s"), sValue);
			return ERR_FAIL;
			}

		SetModResistDamage(iDamageType, iValue2);
		}

	//	Resist damage

	else if (strEquals(sID, CONSTLIT("resistDamageClass")))
		{
		DamageTypes iDamageType = LoadDamageTypeFromXML(sValue);
		if (iDamageType == damageError)
			{
			if (retsError)
				*retsError = strPatternSubst(CONSTLIT("Invalid damage type: %s"), sValue);
			return ERR_FAIL;
			}

		SetModResistDamageClass(iDamageType, iValue2);
		}

	//	Resist damage tier

	else if (strEquals(sID, CONSTLIT("resistDamageTier")))
		{
		DamageTypes iDamageType = LoadDamageTypeFromXML(sValue);
		if (iDamageType == damageError)
			{
			if (retsError)
				*retsError = strPatternSubst(CONSTLIT("Invalid damage type: %s"), sValue);
			return ERR_FAIL;
			}

		SetModResistDamageTier(iDamageType, iValue2);
		}

	//	Resist energy/matter

	else if (strEquals(sID, CONSTLIT("resistEnergy")))
		SetModResistEnergy(iValue);
	else if (strEquals(sID, CONSTLIT("resistMatter")))
		SetModResistMatter(iValue);

	//	Speed bonus

	else if (strEquals(sID, CONSTLIT("speed")))
		{
		if (iValue <= 0)
			{
			if (retsError)
				*retsError = strPatternSubst(CONSTLIT("Invalid speed value: %s."), iValue);
			return ERR_FAIL;
			}
		else
			//	LATER: Support min and max delay limits
			SetModSpeed(iValue);
		}

	//	Efficiency

	else if (strEquals(sID, CONSTLIT("efficient")))
		{
		if (bDisadvantage && iValue > 0)
			iValue = -iValue;

		iValue = mathRound(iValue / 10.0);
		SetModEfficiency(iValue);
		}

	//	Omnidirectional and swivel

	else if (strEquals(sID, CONSTLIT("omnidirectional")))
		{
		if (bDisadvantage || iValue < 0)
			m_dwMods = EncodeAX(etOmnidirectional | etDisadvantage);
		else if (iValue == 0 || iValue >= 360)
			m_dwMods = EncodeAX(etOmnidirectional, 0, 0);
		else
			m_dwMods = EncodeAX(etOmnidirectional, 0, iValue);
		}

	//	Tracking

	else if (strEquals(sID, CONSTLIT("tracking")))
		{
		if (iValue < 0)
			{
			if (retsError)
				*retsError = strPatternSubst(CONSTLIT("Invalid maneuver rate: %s."), iValue);
			return ERR_FAIL;
			}
		else
			SetModTracking(Min(iValue, 180));
		}

	//	Otherwise, see if this is a special damage 

	else
		{
		SpecialDamageTypes iSpecial = DamageDesc::ConvertToSpecialDamageTypes(sID);
		switch (iSpecial)
			{
			case specialArmor:
			case specialBlinding:
			case specialDeviceDamage:
			case specialDeviceDisrupt:
			case specialDisintegration:
			case specialEMP:
			case specialMining:
			case specialMomentum:
			case specialRadiation:
			case specialShatter:
			case specialShieldDisrupt:
			case specialShieldPenetrator:
			case specialTimeStop:
			case specialWMD:
				{
				if (bDisadvantage)
					{
					if (retsError)
						*retsError = CONSTLIT("Disadvantage not supported.");
					return ERR_FAIL;
					}

				if (iValue < 1 || iValue > MAX_ITEM_LEVEL)
					{
					if (retsError)
						*retsError = strPatternSubst(CONSTLIT("Invalid %s damage level: %d"), sID, iValue);
					return ERR_FAIL;
					}

				SetModSpecialDamage(iSpecial, iValue);
				break;
				}

			default:
				{
				if (retsError)
					*retsError = strPatternSubst(CONSTLIT("Invalid enhancement name: %s"), sID);
				return ERR_FAIL;
				}
			}
		}

	//	Done

	return NOERROR;
	}

ALERROR CItemEnhancement::InitFromDesc (CUniverse &Universe, const ICCItem &Item, CString *retsError)

//	InitFromDesc
//
//	Initializes from a CodeChain item

	{
	if (Item.IsSymbolTable())
		{
		//	Enhancement

		CString sMods = Item.GetStringAt(CONSTLIT("enhancement"));
		if (sMods.IsBlank())
			{
			if (retsError) *retsError = CONSTLIT("Must have enhancement field.");
			return ERR_FAIL;
			}

		if (ALERROR error = InitFromDesc(sMods, retsError))
			return error;

		//	Lifetime

		int iLifetime = Item.GetIntegerAt(CONSTLIT("lifetime"), -1);
		m_iExpireTime = (iLifetime != -1 ? Universe.GetTicks() + iLifetime : -1);

		//	Enhancement type

		DWORD dwEnhancementUNID = (DWORD)Item.GetIntegerAt(CONSTLIT("itemType"));
		if (dwEnhancementUNID)
			{
			m_pEnhancer = Universe.FindItemType(dwEnhancementUNID);
			if (m_pEnhancer == NULL)
				{
				if (retsError) *retsError = strPatternSubst(CONSTLIT("Unknown enhancementType: %08x"), dwEnhancementUNID);
				return ERR_FAIL;
				}
			}

		return NOERROR;
		}
	else if (Item.IsInteger())
		{
		*this = CItemEnhancement((DWORD)Item.GetIntegerValue());
		return NOERROR;
		}
	else
		return InitFromDesc(Item.GetStringValue(), retsError);
	}

ALERROR CItemEnhancement::InitFromDesc (SDesignLoadCtx &Ctx, const CString &sDesc)

//	InitFromDesc
//
//	Initializes from a descriptor
	
	{
	return InitFromDesc(sDesc, &Ctx.sError); 
	}

bool CItemEnhancement::IsEqual (const CItemEnhancement &Comp) const

//	IsEqual
//
//	Returns TRUE if enhancements are equal

	{
	return (Comp.m_dwMods == m_dwMods
			&& Comp.m_pEnhancer == m_pEnhancer
			&& Comp.m_iExpireTime == m_iExpireTime); 
	}

bool CItemEnhancement::IsReflective (const DamageDesc &Damage, int *retiReflectChance) const

//	IsReflective
//
//	Returns TRUE if we reflect the given damage

	{
	switch (GetType())
		{
		case etReflect:
			{
			if (!IsDisadvantage() && Damage.GetDamageType() == GetDamageTypeField())
				{
				if (retiReflectChance)
					*retiReflectChance = 50 + (GetLevel() * 5);

				return true;
				}
			else
				return false;
			}

		default:
			return false;
		}
	}

int CItemEnhancement::Level2Bonus (int iLevel, bool bDisadvantage)

//	Level2Bonus
//
//	Convert from a level (0-15) to a bonus adj (+%)

	{
	if (bDisadvantage)
		return -10 * Min(10, iLevel);
	else
		return 10 * iLevel;
	}

int CItemEnhancement::Level2DamageAdj (int iLevel, bool bDisadvantage)

//	Level2DamageAdj
//
//	Convert from a level (0-15) to a damage adj (%)

	{
	if (bDisadvantage)
		return (10000 / Level2DamageAdj(iLevel, false));
	else
		{
		if (iLevel >= 0 && iLevel <= 9)
			return (100 - (10 * iLevel));
		else
			return 10;
		}
	}

void CItemEnhancement::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Reads an enhancement

	{
	ReadFromStream(Ctx.dwVersion, Ctx.pStream);
	}

void CItemEnhancement::ReadFromStream (DWORD dwVersion, IReadStream *pStream)

//	ReadFromStream
//
//	Sometimes we don't have a full Ctx, such as when loading from an ICCItem

	{
	DWORD dwLoad;

	pStream->Read((char *)&m_dwMods, sizeof(DWORD));

	if (dwVersion >= 46)
		{
		pStream->Read((char *)&m_dwID, sizeof(DWORD));

		pStream->Read((char *)&dwLoad, sizeof(DWORD));
		m_pEnhancer = (dwLoad ? g_pUniverse->FindItemType(dwLoad) : NULL);

		pStream->Read((char *)&m_iExpireTime, sizeof(DWORD));
		}

	//	Previous to v46, we did not set m_dwID

	if (dwVersion < 46 && m_dwMods != 0)
		m_dwID = g_pUniverse->CreateGlobalID();
	}

void CItemEnhancement::SetModBonus (int iBonus)

//	SetModBonus
//
//	Sets bonus enhancement

	{
	if (iBonus == 0)
		m_dwMods = 0;
	else if (iBonus > 0)
		m_dwMods = EncodeAX(etHPBonus, 0, iBonus);
	else
		m_dwMods = EncodeAX(etHPBonus | etDisadvantage, 0, -iBonus);
	}

void CItemEnhancement::SetModSpecialDamage (SpecialDamageTypes iSpecial, int iLevel)

//	SetModSpecialDamage
//
//	Sets special damage

	{
	if (iLevel == 0)
		m_dwMods = Encode12(etSpecialDamage, 0, iSpecial);
	else
		m_dwMods = EncodeABC(etConferSpecialDamage, iSpecial, iLevel);
	}

void CItemEnhancement::SetModSpeed (int iAdj, int iMinDelay, int iMaxDelay)

//	SetModSpeed
//
//	Sets a speed adjustment mod.

	{
	if (iAdj <= 100)
		m_dwMods = EncodeABC(etSpeed, iAdj, iMinDelay, iMaxDelay);
	else
		m_dwMods = EncodeABC(etSpeed | etDisadvantage, (iAdj - 100) / 5, iMinDelay, iMaxDelay);
	}

void CItemEnhancement::SetModTracking (int iManeuverRate)

//	SetModTracking
//
//	Sets a maneuver rate enhancement.

	{
	m_dwMods = EncodeAX(etTracking, 0, iManeuverRate);
	}

bool CItemEnhancement::UpdateArmorRegen (CItemCtx &ArmorCtx, SUpdateCtx &UpdateCtx, int iTick) const

//	UpdateArmorRegen
//
//	Updates regeneration for the given armor segment. Returns TRUE if any hit 
//	points were regenerated.

	{
	ERegenTypes iRegenType;
	CRegenDesc Regen;

	if (!CalcRegen(ArmorCtx, CArmorClass::TICKS_PER_UPDATE, Regen, &iRegenType))
		return false;

	//	Regenerate

	return ArmorCtx.GetArmorClass()->UpdateRegen(ArmorCtx, UpdateCtx, Regen, iRegenType, iTick);
	}

void CItemEnhancement::WriteToStream (IWriteStream *pStream) const

//	WriteToStream
//
//	Writes an enhancement

	{
	DWORD dwSave;

	pStream->Write((char *)&m_dwMods, sizeof(DWORD));
	pStream->Write((char *)&m_dwID, sizeof(DWORD));

	dwSave = (m_pEnhancer ? m_pEnhancer->GetUNID() : 0);
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	pStream->Write((char *)&m_iExpireTime, sizeof(DWORD));
	}
