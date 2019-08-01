//	CShieldClass.cpp
//
//	CShieldClass class

#include "PreComp.h"

#define HIT_EFFECT_TAG							CONSTLIT("HitEffect")

#define ABSORB_ADJ_ATTRIB						CONSTLIT("absorbAdj")
#define AIMED_REFLECTION_ATTRIB					CONSTLIT("aimedReflection")
#define ARMOR_SHIELD_ATTRIB						CONSTLIT("armorShield")
#define DAMAGE_ADJ_ATTRIB						CONSTLIT("damageAdj")
#define DAMAGE_ADJ_LEVEL_ATTRIB					CONSTLIT("damageAdjLevel")
#define DEPLETION_DELAY_ATTRIB					CONSTLIT("depletionDelay")
#define HAS_NON_REGEN_HP_ATTRIB					CONSTLIT("hasNonRegenHP")
#define HIT_EFFECT_ATTRIB						CONSTLIT("hitEffect")
#define HIT_POINTS_ATTRIB						CONSTLIT("hitPoints")
#define IDLE_POWER_USE_ATTRIB					CONSTLIT("idlePowerUse")
#define HP_ADJ_PER_CHARGE_ATTRIB				CONSTLIT("HPBonusPerCharge")
#define MAX_CHARGES_ATTRIB						CONSTLIT("maxCharges")
#define POWER_ADJ_PER_CHARGE_ATTRIB				CONSTLIT("powerBonusPerCharge")
#define POWER_USE_ATTRIB						CONSTLIT("powerUse")
#define REFLECT_ATTRIB							CONSTLIT("reflect")
#define REGEN_ATTRIB							CONSTLIT("regen")
#define REGEN_HP_ATTRIB							CONSTLIT("regenHP")
#define REGEN_ADJ_PER_CHARGE_ATTRIB				CONSTLIT("regenHPBonusPerCharge")
#define REGEN_TIME_ATTRIB						CONSTLIT("regenTime")
#define REGEN_TYPE_ATTRIB						CONSTLIT("regenType")
#define WEAPON_SUPPRESS_ATTRIB					CONSTLIT("weaponSuppress")

#define GET_MAX_HP_EVENT						CONSTLIT("GetMaxHP")
#define ON_SHIELD_DAMAGE_EVENT					CONSTLIT("OnShieldDamage")
#define ON_SHIELD_DOWN_EVENT					CONSTLIT("OnShieldDown")

#define FIELD_ADJUSTED_HP						CONSTLIT("adjustedHP")
#define FIELD_BALANCE							CONSTLIT("balance")
#define FIELD_DAMAGE_ADJ						CONSTLIT("damageAdj")
#define FIELD_EFFECTIVE_HP						CONSTLIT("effectiveHP")
#define FIELD_HP								CONSTLIT("hp")
#define FIELD_HP_BONUS							CONSTLIT("hpBonus")
#define FIELD_POWER								CONSTLIT("power")
#define FIELD_REGEN								CONSTLIT("regen")
#define FIELD_WEAPON_SUPPRESS					CONSTLIT("weaponSuppress")

#define PROPERTY_DAMAGE_ADJ						CONSTLIT("damageAdj")
#define PROPERTY_HP								CONSTLIT("hp")
#define PROPERTY_HP_BONUS						CONSTLIT("hpBonus")
#define PROPERTY_MAX_HP							CONSTLIT("maxHP")
#define PROPERTY_REGEN							CONSTLIT("regen")

#define STR_BY_SHIELD_INTEGRITY					CONSTLIT("byShieldIntegrity")
#define STR_SHIELD_REFLECT						CONSTLIT("reflect")
#define STR_STANDARD							CONSTLIT("standard")

#define MAX_REFLECTION_CHANCE					95
const Metric MAX_REFLECTION_TARGET =			50.0 * LIGHT_SECOND;
const Metric STD_FIRE_DELAY_TICKS =				8.0;
const Metric STD_DEFENSE_RATIO =				0.8;
const int STD_DEPLETION_DELAY =					360;

const Metric REFLECTION_BALANCE_BONUS =			50.0;
const Metric BALANCE_COST_RATIO =				-0.5;	//  Each percent of cost above standard is a 0.5%
const Metric BALANCE_RECOVERY_ADJ =				20.0;	//	Balance points if we recover twice as fast as normal.
const Metric BALANCE_POWER_RATIO =				-0.5;	//  Each percent of power consumption above standard
const Metric BALANCE_NO_SLOT =					20.0;	//  Bonus to balance if no slot is used.
const Metric BALANCE_SLOT_FACTOR =				-40.0;	//  Penalty to balance for every extra slot used 
const Metric BALANCE_LEAKAGE_FACTOR =			-25.0;	//	Penalty from allowing some damage through
const Metric BALANCE_LEAKAGE_POWER =			0.5;	//	Curve for leakage
const Metric BALANCE_MAX_DAMAGE_ADJ =			400.0;	//	Max change in balance due to a single damage type

static CShieldClass::SStdStats STD_STATS[MAX_ITEM_LEVEL] =
	{
		//	HP		Regen	Cost			Power
		{	35,		12,		350.0,			10, },
		{	45,		16,		800.0,			20, },
		{	60,		20,		1850.0,			50, },
		{	80,		28,		4000.0,			100, },
		{	100,	36,		8500.0,			200, },

		{	135,	45,		18500.0,		300, },
		{	175,	60,		39000.0,		500, },
		{	225,	80,		78500.0,		1000, },
		{	300,	100,	158000.0,		2000, },
		{	380,	130,	320000.0,		3000, },

		{	500,	170,	640000.0,		5000, },
		{	650,	220,	1300000.0,		8000, },
		{	850,	290,	2700000.0,		12000, },
		{	1100,	370,	5300000.0,		18000, },
		{	1400,	490,	10600000.0,		27500, },

		{	1850,	630,	21300000.0,		40000, },
		{	2400,	820,	42600000.0,		60000, },
		{	3100,	1100,	85200000.0,		90000, },
		{	4000,	1400,	170000000.0,	120000, },
		{	5250,	1800,	341000000.0,	160000, },

		{	6850,	2350,	682000000.0,	220000, },
		{	9000,	3050,	1400000000.0,	300000, },
		{	12000,	4000,	2700000000.0,	400000, },
		{	15000,	5150,	5500000000.0,	500000, },
		{	20000,	6700,	10900000000.0,	600000, },
	};

static char *CACHED_EVENTS[CShieldClass::evtCount] =
	{
		"GetMaxHP",
		"OnShieldDamage",
		"OnShieldDown",
	};

inline CShieldClass::SStdStats *GetStdStats (int iLevel)
	{
	if (iLevel >= 1 && iLevel <= MAX_ITEM_LEVEL)
		return &STD_STATS[iLevel - 1];
	else
		return NULL;
	}

CShieldClass::CShieldClass (void)
	{
	}

bool CShieldClass::AbsorbDamage (CInstalledDevice *pDevice, CSpaceObject *pShip, SDamageCtx &Ctx)

//	AbsorbDamage
//
//	Absorbs damage.
//	NOTE: We always set Ctx.iAbsorb properly, regardless of the return value.

	{
	DEBUG_TRY

	CItemCtx ItemCtx(pShip, pDevice);
	const CItemEnhancementStack *pEnhancements = ItemCtx.GetEnhancementStack();

	//	If we're depleted then we cannot absorb anything

	Ctx.iHPLeft = GetHPLeft(ItemCtx);
	if (Ctx.iHPLeft == 0)
		{
		Ctx.iAbsorb = 0;
		return false;
		}

	//	Calculate how much we will absorb

	int iAbsorbAdj = (Ctx.Damage.GetDamageType() == damageGeneric ? 100 : m_iAbsorbAdj[Ctx.Damage.GetDamageType()]);
	Ctx.iAbsorb = mathAdjust(Ctx.iDamage, iAbsorbAdj);
	if (pEnhancements)
		Ctx.iAbsorb = mathAdjust(Ctx.iAbsorb, pEnhancements->GetAbsorbAdj(Ctx.Damage));

	//	Compute how much damage we take (based on the type of damage)

	int iAdj = GetDamageAdj(Ctx.Damage, pEnhancements);
	Ctx.iShieldDamage = mathAdjust(Ctx.iAbsorb, iAdj);

	//	If shield generator is damaged then sometimes we take extra damage

	if (pDevice->IsDamaged() || pDevice->IsDisrupted())
		{
		int iRoll = mathRandom(1, 100);

		if (iRoll <= 10)
			Ctx.iAbsorb = mathAdjust(Ctx.iAbsorb, 75);
		else if (iRoll <= 25)
			Ctx.iShieldDamage *= 2;
		}

	//	If the amount of shield damage is greater than HP left, then scale
	//	the amount of HP that we aborb

	if (Ctx.iShieldDamage > Ctx.iHPLeft)
		{
		//	ASSERT: We know that iShieldDamage is > 0 because iHPLeft is > 0.

		Ctx.iAbsorb = Ctx.iHPLeft * Ctx.iAbsorb / Ctx.iShieldDamage;
		Ctx.iShieldDamage = Ctx.iHPLeft;
		}

	//	See if we're reflective

	int iReflectChance = 0;
	if (pEnhancements)
		pEnhancements->ReflectsDamage(Ctx.Damage.GetDamageType(), &iReflectChance);
	if (m_Reflective.InSet(Ctx.Damage.GetDamageType()))
		iReflectChance = Max(iReflectChance, MAX_REFLECTION_CHANCE);
	if (iReflectChance 
			&& Ctx.pCause)
		{
		CItemCtx ItemCtx(pShip, pDevice);

		//	If the shot has shield damage, we adjust the chance of reflecting.
		//	Adjust based on difference in level (negative numbers means the shield
		//	is lower than the damage level):
		//
		//	...
		//	<-3 =	No reflect
		//	-3  =	50% reflect adj
		//	-2	=	75% reflect adj
		//	-1	=	90% reflect adj
		//	>=0	=	Full reflect

		if (Ctx.Damage.GetShieldDamageLevel() > 0)
			{
			int iDiff = GetLevel() - Ctx.Damage.GetShieldDamageLevel();
			switch (iDiff)
				{
				case -3:
					iReflectChance = 50 * iReflectChance / 100;
					break;

				case -2:
					iReflectChance = 75 * iReflectChance / 100;
					break;

				case -1:
					iReflectChance = 90 * iReflectChance / 100;
					break;

				default:
					if (iDiff < -3)
						iReflectChance = 0;
					break;
				}
			}

		//	Compute the chance that we will reflect (based on the strength of
		//	our shields)

		int iMaxHP = GetMaxHP(ItemCtx);
		int iEfficiency = (iMaxHP == 0 ? 100 : 50 + (Ctx.iHPLeft * 50 / iMaxHP));
		int iChance = iEfficiency * iReflectChance / 100;

		//	See if we reflect

		Ctx.SetShotReflected(mathRandom(1, 100) <= iChance);
		if (Ctx.IsShotReflected())
			{
			Ctx.iOriginalAbsorb = Ctx.iAbsorb;
			Ctx.iOriginalShieldDamage = Ctx.iShieldDamage;
			Ctx.iAbsorb = Ctx.iDamage;
			Ctx.iShieldDamage = 0;
			}
		}
	else
		Ctx.SetShotReflected(false);

	//	If this damage is a shield penetrator, then we either penetrate the 
	//	shields, or reflect.

	int iPenetrateAdj = Ctx.Damage.GetShieldPenetratorAdj();
	if (iPenetrateAdj > 0 && !Ctx.IsShotReflected())
		{
		//	We compare the HP of damage done to the shields vs. the HP left
		//	on the shields. Bigger values means greater chance of penetrating.
		//	We scale to 0-100.
		//
		//	NOTE: We can guarantee that Ctx.iHPLeft > 0.

		Metric rPenetrate = (200.0 / (Metric)Ctx.iHPLeft) * ((Metric)Ctx.iShieldDamage * (Metric)Ctx.iShieldDamage) / ((Metric)Ctx.iShieldDamage + (Metric)Ctx.iHPLeft);

		//	Adjust for tech level

        int iWeaponLevel = Ctx.pDesc->GetLevel();
		int iShieldLevel = (pDevice ? pDevice->GetLevel() : 1);
		rPenetrate *= pow(1.5, (iWeaponLevel - iShieldLevel));

		//	Add the adjustment

		int iChance = (int)rPenetrate + iPenetrateAdj;

		//	Roll the chance. If we penetrate, then the shields absorb nothing
		//	and the shot goes clean through.

		if (mathRandom(1, 100) <= iChance)
			{
			Ctx.iOriginalAbsorb = Ctx.iAbsorb;
			Ctx.iOriginalShieldDamage = Ctx.iShieldDamage;
			Ctx.iAbsorb = 0;
			Ctx.iShieldDamage = Ctx.iShieldDamage / 4;
			}

		//	Otherwise, we reflect.

		else
			{
			Ctx.SetShotReflected(true);
			Ctx.iOriginalAbsorb = Ctx.iAbsorb;
			Ctx.iOriginalShieldDamage = Ctx.iShieldDamage;
			Ctx.iAbsorb = Ctx.iDamage;
			Ctx.iShieldDamage = 0;
			}
		}

	//	If we have time stop damage, then there is a chance that the shields 
	//	will prevent it.
	//
	//	The chance of negating time stop damage depends on the difference in 
	//	levels.

	if (Ctx.IsTimeStopped()
			&& mathRandom(1, 100) <= Ctx.Damage.GetTimeStopResistChance(GetLevel()))
		{
		Ctx.SetTimeStopped(false);
		}

	//	Give custom damage a chance to react. These events can modify the
	//	following variables:
	//
	//	Ctx.bReflect
	//	Ctx.iAbsorb
	//	Ctx.iShieldDamage
	//
	//	OR
	//
	//	Ctx.iDamage (if we skip further processing)

	if (Ctx.pDesc->FireOnDamageShields(Ctx, pDevice->GetDeviceSlot()))
		return (Ctx.iDamage == 0);

	FireOnShieldDamage(CItemCtx(pShip, pDevice), Ctx);

	//	If we reflect, then create the reflection

	if (Ctx.IsShotReflected())
		{
		int iDirection;

		//	If we aim the reflection, then figure out what to aim at.

		CSpaceObject *pTarget;
		if (m_fAimReflection
				&& (pTarget = pShip->GetNearestVisibleEnemy(MAX_REFLECTION_TARGET))
				&& (iDirection = pShip->CalcFireSolution(pTarget, Ctx.pCause->GetMaxSpeed())) != -1)
			{
			//	iDirection is set
			}

		//	Otherwise, we reflect randomly

		else
			{
			iDirection = AngleMod(Ctx.iDirection + mathRandom(120, 240));
			}

		Ctx.pCause->CreateReflection(Ctx.vHitPos, iDirection);
		}

	//	Create shield effect

	if ((Ctx.iAbsorb || Ctx.IsShotReflected())
			&& m_pHitEffect
			&& !Ctx.bNoHitEffect)
		{
		m_pHitEffect->CreateEffect(pShip->GetSystem(),
				NULL,
				Ctx.vHitPos,
				pShip->GetVel(),
				Ctx.iDirection);
		}

	//	Shield takes damage

	if (Ctx.iShieldDamage > 0)
		{
		if (Ctx.iShieldDamage >= Ctx.iHPLeft)
			SetDepleted(pDevice, pShip);
		else
			SetHPLeft(pDevice, pShip, Ctx.iHPLeft - Ctx.iShieldDamage, true);

		pShip->OnComponentChanged(comShields);
		}

	//	Set the remaining damage

	Ctx.iDamage -= Ctx.iAbsorb;
	return (Ctx.iDamage == 0);

	DEBUG_CATCH
	}

bool CShieldClass::AbsorbsWeaponFire (CInstalledDevice *pDevice, CSpaceObject *pSource, CInstalledDevice *pWeapon)

//	AbsorbsWeaponFire
//
//	Returns TRUE if the shield absorbs fire from the given weapon
//	when installed on the same ship

	{
    int iType = pWeapon->GetDamageType(CItemCtx(pSource, pDevice));
	if (iType != -1 
			&& m_WeaponSuppress.InSet(iType)
			&& pDevice->IsWorking()
			&& !IsDepleted(pDevice))
		{
		//	Create effect

		if (m_pHitEffect)
			m_pHitEffect->CreateEffect(pSource->GetSystem(),
					pSource,
					pWeapon->GetPos(pSource),
					pSource->GetVel(),
					0);

		return true;
		}
	else
		return false;
	}

bool CShieldClass::Activate (CInstalledDevice *pDevice, 
							 CSpaceObject *pSource, 
							 CSpaceObject *pTarget,
							 bool *retbSourceDestroyed,
							 bool *retbConsumedItems)

//	Activate
//
//	Activates device

	{
	return false;
	}

int CShieldClass::CalcBalance (CItemCtx &ItemCtx, SBalance &retBalance) const

//	CalcBalance
//
//	Computes the relative balance of this shield relative to its level

	{
	//	Initialize

	retBalance.iLevel = GetLevel();

	//	Get standard stats at level

	SStdStats *pStd = GetStdStats(retBalance.iLevel);
	if (pStd == NULL)
		return 0;

	//  Get HP/regen stats for the shields

	int iMaxHP;
	CalcMinMaxHP(ItemCtx, ItemCtx.GetItem().GetMaxCharges(), 0, 0, NULL, &iMaxHP);
	Metric rRegen180 = m_Regen.GetHPPer180();

	//	Compute our defense ratio

	CalcBalanceDefense(ItemCtx, retBalance.iLevel, iMaxHP, rRegen180, &retBalance.rDefenseRatio);

	//	Compute balance contribution from just HP (with standard regen)

	retBalance.rHPBalance = CalcBalanceDefense(ItemCtx, retBalance.iLevel, iMaxHP, pStd->iRegen);
	retBalance.rBalance = retBalance.rHPBalance;

	//	Compute balance from regen (with standard HP)

	retBalance.rRegen = CalcBalanceDefense(ItemCtx, retBalance.iLevel, pStd->iHP, rRegen180);
	retBalance.rBalance += retBalance.rRegen;

	//	Damage adjustment

	retBalance.rDamageAdj = CalcBalanceDamageAdj(ItemCtx);
	retBalance.rBalance += retBalance.rDamageAdj;

	//	Depletion recovery adj

	retBalance.rRecoveryAdj = BALANCE_RECOVERY_ADJ * mathLog2((Metric)STD_DEPLETION_DELAY / (Metric)Max(1, m_iDepletionTicks));
	retBalance.rBalance += retBalance.rRecoveryAdj;

	//	Power use

	retBalance.rPowerUse = CalcBalancePowerUse(ItemCtx, *pStd);
	retBalance.rBalance += retBalance.rPowerUse;

	//  Slots

	if (GetSlotsRequired() == 0)
		retBalance.rSlots = BALANCE_NO_SLOT;
	else if (GetSlotsRequired() > 1)
		retBalance.rSlots = BALANCE_SLOT_FACTOR * (GetSlotsRequired() - 1);
	else
		retBalance.rSlots = 0.0;

	retBalance.rBalance += retBalance.rSlots;

	//	Cost

	Metric rCost = (Metric)CEconomyType::ExchangeToCredits(GetItemType()->GetCurrencyAndValue(ItemCtx, true));
	Metric rCostDelta = 100.0 * (rCost - pStd->rCost) / (Metric)pStd->rCost;
	retBalance.rCost = BALANCE_COST_RATIO * rCostDelta;
	retBalance.rBalance += retBalance.rCost;

	//	Done

	return (int)retBalance.rBalance;
	}

Metric CShieldClass::CalcBalanceDamageAdj (CItemCtx &ItemCtx) const

//	CalcBalanceDamageAdj
//
//	Calculate balance contribution from damage adjustment.

	{
	DamageTypes iDamage;

	//	Loop over all damage types and accumulate balance adjustments.

	Metric rTotalBalance = 0.0;
	for (iDamage = damageLaser; iDamage < damageCount; iDamage = (DamageTypes)(iDamage + 1))
		{
		int iAdj;
		int iDefaultAdj;
		m_DamageAdj.GetAdjAndDefault(iDamage, &iAdj, &iDefaultAdj);

		//	Compare the adjustment and the standard adjustment. [We treat 0
		//	as 1 to avoid infinity.]

		Metric rBalance = (100.0 * mathLog2((Metric)Max(1, iDefaultAdj) / (Metric)Max(1, iAdj)));
		if (rBalance > BALANCE_MAX_DAMAGE_ADJ)
			rBalance = BALANCE_MAX_DAMAGE_ADJ;
		else if (rBalance < -BALANCE_MAX_DAMAGE_ADJ)
			rBalance = -BALANCE_MAX_DAMAGE_ADJ;

		//	If we reflect this damage, adjust balance

		if (m_Reflective.InSet(iDamage))
			rBalance += REFLECTION_BALANCE_BONUS;

		//	If we allow some damage through, then we take a penalty

		if (m_iAbsorbAdj[iDamage] < 100)
			{
			Metric rLeakage = (100.0 - m_iAbsorbAdj[iDamage]);
			rBalance += BALANCE_LEAKAGE_FACTOR * pow(rLeakage, BALANCE_LEAKAGE_POWER);
			}

		//	Adjust the based on how common this damage type is at the given 
		//	armor level.

		rBalance *= CDamageAdjDesc::GetDamageTypeFraction(GetLevel(), iDamage);

		//	Accumulate

		rTotalBalance += rBalance;
		}

	//	Done

	return rTotalBalance;
	}

Metric CShieldClass::CalcBalanceDefense (CItemCtx &ItemCtx, int iLevel, Metric rHP, Metric rRegen180, Metric *retrRatio) const

//	CalcBalanceDefense
//
//	Calc defense balance based on level, HP, and regen.

	{
	Metric rCycles = 3.0;
	Metric rEffectiveHP = rHP + (rCycles * rRegen180);

	//	Our defense ratio is the ratio of effective HP to 180-tick std weapon
	//	damage at our level. The standard value for this ratio is STD_DEFENSE_RATIO.

	Metric rDefenseRatio = rEffectiveHP / CWeaponClass::GetStdDamage180(iLevel);

	//	Result

	if (retrRatio)
		*retrRatio = rDefenseRatio;

	return 100.0 * mathLog2(rDefenseRatio / STD_DEFENSE_RATIO);
	}

Metric CShieldClass::CalcBalancePowerUse (CItemCtx &ItemCtx, const SStdStats &Stats) const

//	CalcBalancePowerUse
//
//	Balance contribution from power use.

	{
	int iPower = m_iPowerUse + (m_iExtraPowerPerCharge * ItemCtx.GetItem().GetMaxCharges());
	Metric rPowerDelta = 100.0 * (iPower - Stats.iPower) / (Metric)Stats.iPower;

	return BALANCE_POWER_RATIO * rPowerDelta;
	}

void CShieldClass::CalcMinMaxHP (CItemCtx &Ctx, int iCharges, int iArmorSegs, int iTotalHP, int *retiMin, int *retiMax) const

//	CalcMinMaxHP
//
//	Returns the min and max HP of this shield
//
//	iCharges = m_iMaxCharges or the current charges on item
//	iArmorSegs = count of armor segments on ship (or 0)
//	iTotalHP = current total HP of all armor segments (or 0)

	{
	int iMax = m_iHitPoints;
	int iMin = iMax;

	if (m_iExtraHPPerCharge)
		iMax = Max(0, iMax + (m_iExtraHPPerCharge * iCharges));
	else if (m_fHasNonRegenHPBonus)
		iMax = Max(0, iMax + iCharges);

	if (m_iArmorShield)
		{
		iMin = m_iArmorShield;

		if (iArmorSegs > 0)
			iMax = Min(iMax, ((m_iArmorShield * iTotalHP / iArmorSegs) + 5) / 10);
		}

	//	If we're installed, fire the custom event to get max HPs

	if (Ctx.GetSource() && Ctx.GetDevice())
		iMax = FireGetMaxHP(Ctx.GetDevice(), Ctx.GetSource(), iMax);

	//	Adjust max HP based on enhancements

	const CItemEnhancementStack *pEnhancements = Ctx.GetEnhancementStack();
	if (pEnhancements)
		iMax += (iMax * pEnhancements->GetBonus() / 100);

	//	Done

	if (iMin > iMax)
		iMin = iMax;

	if (retiMin)
		*retiMin = iMin;

	if (retiMax)
		*retiMax = iMax;
	}

int CShieldClass::CalcPowerUsed (SUpdateCtx &UpdateCtx, CInstalledDevice *pDevice, CSpaceObject *pSource)

//	CalcPowerUsed
//
//	Returns the amount of power used by the shields

	{
	CItemCtx Ctx(pSource, pDevice);

	//	Only if enabled

	if (!pDevice->IsEnabled())
		return 0;

	//	Compute power

	int iIdlePower;
	int iPower = GetPowerRating(Ctx, &iIdlePower);

	//	If we're regenerating shields, then we consume more power
	//	otherwise, we only consume half power

	if ((!m_Regen.IsEmpty() || m_iExtraRegenPerCharge > 0) && pDevice->IsRegenerating())
		return iPower;
	else
		return iIdlePower;
	}

Metric CShieldClass::CalcRegen180 (CItemCtx &Ctx, DWORD dwFlags) const

//	CalcRegen180
//
//	Returns the average number of HP regenerated every 180 ticks.
//
//	NOTE: This is used for stats purposes; the actual regeneration uses a 
//	different algorithm.

	{
	//	If there is interference, then we don't regenerate

	CSpaceObject *pSource = Ctx.GetSource();
	if (pSource && pSource->GetShipPerformance().HasShieldInterference())
		return 0.0;

	//	If the device is disabled, then no regeneration

	if (!Ctx.IsDeviceEnabled() && !(dwFlags & FLAG_IGNORE_DISABLED))
		return 0.0;

	//	Compute regeneration

	Metric rRegen = m_Regen.GetHPPer180();
	
	if (m_iExtraHPPerCharge)
		rRegen += m_iExtraHPPerCharge * Ctx.GetItemCharges();

	return rRegen;
	}

ALERROR CShieldClass::CreateFromXML (SDesignLoadCtx &Ctx, SInitCtx &InitCtx, CXMLElement *pDesc, CDeviceClass **retpShield)

//	CreateFromXML
//
//	Creates from an XML element

	{
	ALERROR error;
	CShieldClass *pShield;
	int i;

	pShield = new CShieldClass;
	if (pShield == NULL)
		return ERR_MEMORY;

	if (error = pShield->InitDeviceFromXML(Ctx, pDesc, InitCtx.pType))
		return error;

	pShield->m_iHitPoints = pDesc->GetAttributeInteger(HIT_POINTS_ATTRIB);
	pShield->m_iArmorShield = pDesc->GetAttributeInteger(ARMOR_SHIELD_ATTRIB);
	pShield->m_iPowerUse = pDesc->GetAttributeIntegerBounded(POWER_USE_ATTRIB, 0, -1, 0);
	pShield->m_iIdlePowerUse = pDesc->GetAttributeIntegerBounded(IDLE_POWER_USE_ATTRIB, 0, -1, pShield->m_iPowerUse / 2);

	//	Charges

	pShield->m_iExtraHPPerCharge = pDesc->GetAttributeInteger(HP_ADJ_PER_CHARGE_ATTRIB);
	pShield->m_iExtraPowerPerCharge = pDesc->GetAttributeInteger(POWER_ADJ_PER_CHARGE_ATTRIB);
	pShield->m_iExtraRegenPerCharge = pDesc->GetAttributeInteger(REGEN_ADJ_PER_CHARGE_ATTRIB);
	pShield->m_fHasNonRegenHPBonus = pDesc->GetAttributeBool(HAS_NON_REGEN_HP_ATTRIB);

	//	For backwards compatibility we allow the shield descriptor to set max charges
	//	on the item type.

	InitCtx.iMaxCharges = pDesc->GetAttributeIntegerBounded(MAX_CHARGES_ATTRIB, 0, -1, -1);

	//	Load regen value

	CString sRegen;
	if (pDesc->FindAttribute(REGEN_ATTRIB, &sRegen))
		{
		if (error = pShield->m_Regen.InitFromRegenString(Ctx, sRegen))
			return error;

		int iDepletion;
		if (pDesc->FindAttributeInteger(DEPLETION_DELAY_ATTRIB, &iDepletion))
			pShield->m_iDepletionTicks = Max(1, iDepletion);
		else
			pShield->m_iDepletionTicks = STD_DEPLETION_DELAY;
		}
	else
		{
		int iRegenTime = pDesc->GetAttributeInteger(REGEN_TIME_ATTRIB);

		if (error = pShield->m_Regen.InitFromRegenTimeAndHP(Ctx, iRegenTime, pDesc->GetAttributeInteger(REGEN_HP_ATTRIB)))
			return error;

		int iDepletion;
		if (pDesc->FindAttributeInteger(DEPLETION_DELAY_ATTRIB, &iDepletion))
			pShield->m_iDepletionTicks = Max(1, iDepletion * iRegenTime);
		else
			pShield->m_iDepletionTicks = STD_DEPLETION_DELAY;
		}

	//	Variable regen

	pShield->m_fRegenByShieldLevel = false;

	CString sAttrib;
	if (pDesc->FindAttribute(REGEN_TYPE_ATTRIB, &sAttrib))
		{
		if (strEquals(sAttrib, STR_STANDARD))
			{ }
		else if (strEquals(sAttrib, STR_BY_SHIELD_INTEGRITY))
			pShield->m_fRegenByShieldLevel = true;
		else
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Invalid variable regen: %s"), sAttrib);
			return ERR_FAIL;
			}
		}

	//	Load damage adjustment

	pShield->m_iDamageAdjLevel = pDesc->GetAttributeIntegerBounded(DAMAGE_ADJ_LEVEL_ATTRIB, 1, MAX_ITEM_LEVEL, InitCtx.pType->GetLevel());
	if (error = pShield->m_DamageAdj.InitFromXML(Ctx, pDesc))
		return error;

	//	Load absorb adjustment; if attribute not found, assume 100% for everything

	CString sAbsorbAdj;
	if (pDesc->FindAttribute(ABSORB_ADJ_ATTRIB, &sAbsorbAdj))
		{
		TArray<int> AbsorbAdj;
		if (error = ::ParseAttributeIntegerList(sAbsorbAdj, &AbsorbAdj))
			return error;

		for (i = 0; i < damageCount; i++)
			pShield->m_iAbsorbAdj[i] = (i < AbsorbAdj.GetCount() ? AbsorbAdj[i] : 0);
		}
	else
		{
		for (i = 0; i < damageCount; i++)
			pShield->m_iAbsorbAdj[i] = 100;
		}

	//	Load the weapon suppress

	if (error = pShield->m_WeaponSuppress.InitFromXML(pDesc->GetAttribute(WEAPON_SUPPRESS_ATTRIB)))
		{
		Ctx.sError = CONSTLIT("Unable to load weapon suppress attribute");
		return error;
		}

	//	Load reflection

	if (error = pShield->m_Reflective.InitFromXML(pDesc->GetAttribute(REFLECT_ATTRIB)))
		{
		Ctx.sError = CONSTLIT("Unable to load reflective attribute");
		return error;
		}

	pShield->m_fAimReflection = pDesc->GetAttributeBool(AIMED_REFLECTION_ATTRIB);

	//	Effects

	if (error = pShield->m_pHitEffect.LoadEffect(Ctx,
			strPatternSubst(CONSTLIT("%d:h"), InitCtx.pType->GetUNID()),
			pDesc->GetContentElementByTag(HIT_EFFECT_TAG),
			pDesc->GetAttribute(HIT_EFFECT_ATTRIB)))
		return error;

	//	Done

	*retpShield = pShield;

	return NOERROR;
	}

void CShieldClass::Deplete (CInstalledDevice *pDevice, CSpaceObject *pSource)

//	Disable
//
//	Lower shields

	{
	SetDepleted(pDevice, pSource);
	pSource->OnComponentChanged(comShields);
	}

bool CShieldClass::FindDataField (const CString &sField, CString *retsValue)

//	FindDataField
//
//	Returns meta-data

	{
	int i;

	if (strEquals(sField, FIELD_HP))
		*retsValue = strFromInt(m_iHitPoints);
	else if (strEquals(sField, FIELD_EFFECTIVE_HP))
		{
		int iHP;
		int iHPbyDamageType[damageCount];
		GetReferenceDamageAdj(NULL, NULL, &iHP, iHPbyDamageType);
		*retsValue = strFromInt(::CalcEffectiveHP(GetLevel(), iHP, iHPbyDamageType));
		}
	else if (strEquals(sField, FIELD_REGEN))
		*retsValue = strFromInt((int)m_Regen.GetHPPer180());
	else if (strEquals(sField, FIELD_ADJUSTED_HP))
		{
		int iHP;
		int iHPbyDamageType[damageCount];
		GetReferenceDamageAdj(NULL, NULL, &iHP, iHPbyDamageType);

		CString sResult;
		for (i = 0; i < damageCount; i++)
			{
			if (i > 0)
				sResult.Append(CONSTLIT("\t"));
			sResult.Append(strFromInt(iHPbyDamageType[i]));
			}
		*retsValue = sResult;
		}
	else if (strEquals(sField, FIELD_DAMAGE_ADJ))
		{
		retsValue->Truncate(0);

		for (i = 0; i < damageCount; i++)
			{
			if (i > 0)
				retsValue->Append(CONSTLIT("\t"));

			retsValue->Append(strFromInt(m_DamageAdj.GetAdj((DamageTypes)i)));
			}
		}
	else if (strEquals(sField, FIELD_POWER))
		*retsValue = strFromInt(m_iPowerUse * 100);
	else if (strEquals(sField, FIELD_HP_BONUS))
		{
		CString sResult;

		for (i = 0; i < damageCount; i++)
			{
			if (i > 0)
				sResult.Append(CONSTLIT(", "));

			int iBonus = m_DamageAdj.GetHPBonus((DamageTypes)i);
			if (iBonus == -100)
				sResult.Append(CONSTLIT("***"));
			else
				sResult.Append(strPatternSubst(CONSTLIT("%3d"), iBonus));
			}

		*retsValue = sResult;
		}
	else if (strEquals(sField, FIELD_BALANCE))
		*retsValue = strFromInt(CalcBalance(CItemCtx(), SBalance()));
	else if (strEquals(sField, FIELD_WEAPON_SUPPRESS))
		{
		if (m_WeaponSuppress.IsEmpty())
			*retsValue = NULL_STR;
		else
			{
			*retsValue = CONSTLIT("=(");

			bool bNeedSeparator = false;
			for (i = 0; i < damageCount; i++)
				if (m_WeaponSuppress.InSet(i))
					{
					if (bNeedSeparator)
						retsValue->Append(CONSTLIT(" "));

					retsValue->Append(::GetDamageType((DamageTypes)i));
					bNeedSeparator = true;
					}

			retsValue->Append(CONSTLIT(")"));
			}
		}
	else
		return false;

	return true;
	}

int CShieldClass::FireGetMaxHP (CInstalledDevice *pDevice, CSpaceObject *pSource, int iMaxHP) const

//	FireGetMaxHP
//
//	Fire GetMaxHP event

	{
	SEventHandlerDesc Event;
	if (FindEventHandlerShieldClass(evtGetMaxHP, &Event))
		{
		ASSERT(pSource);
		ASSERT(pDevice);

		CCodeChainCtx Ctx(GetUniverse());

		Ctx.DefineContainingType(GetItemType());
		Ctx.SaveAndDefineSourceVar(pSource);
		Ctx.SaveAndDefineItemVar(pSource->GetItemForDevice(pDevice));
		Ctx.DefineInteger(CONSTLIT("aMaxHP"), iMaxHP);

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			pSource->ReportEventError(GET_MAX_HP_EVENT, pResult);
		else if (!pResult->IsNil())
			iMaxHP = Max(0, pResult->GetIntegerValue());

		Ctx.Discard(pResult);
		}

	return iMaxHP;
	}

void CShieldClass::FireOnShieldDamage (CItemCtx &ItemCtx, SDamageCtx &Ctx)

//	FireOnShieldDamage
//
//	Fire OnShieldDamage

	{
	SEventHandlerDesc Event;
	if (FindEventHandlerShieldClass(evtOnShieldDamage, &Event))
		{
		//	Setup arguments

		CCodeChainCtx CCCtx(GetUniverse());
		CCCtx.DefineContainingType(GetItemType());
		CCCtx.SaveAndDefineSourceVar(ItemCtx.GetSource());
		CCCtx.SaveAndDefineItemVar(ItemCtx);
		CCCtx.DefineDamageCtx(Ctx);

		CCCtx.DefineInteger(CONSTLIT("aShieldHP"), Ctx.iHPLeft);
		CCCtx.DefineInteger(CONSTLIT("aShieldDamageHP"), Ctx.iShieldDamage);
		CCCtx.DefineInteger(CONSTLIT("aArmorDamageHP"), Ctx.iDamage - Ctx.iAbsorb);
		if (Ctx.IsShotReflected())
			{
			CCCtx.DefineString(CONSTLIT("aShieldReflect"), STR_SHIELD_REFLECT);
			CCCtx.DefineInteger(CONSTLIT("aOriginalShieldDamageHP"), Ctx.iOriginalShieldDamage);
			CCCtx.DefineInteger(CONSTLIT("aOriginalArmorDamageHP"), Ctx.iDamage - Ctx.iOriginalAbsorb);
			}
		else
			{
			CCCtx.DefineNil(CONSTLIT("aShieldReflect"));
			CCCtx.DefineInteger(CONSTLIT("aOriginalShieldDamageHP"), Ctx.iShieldDamage);
			CCCtx.DefineInteger(CONSTLIT("aOriginalArmorDamageHP"), Ctx.iDamage - Ctx.iAbsorb);
			}

		ICCItem *pResult = CCCtx.Run(Event);

		//	If we return Nil, then nothing

		if (pResult->IsNil())
			NULL;

		//	If an error, report it

		else if (pResult->IsError())
			ItemCtx.GetSource()->ReportEventError(ON_SHIELD_DAMAGE_EVENT, pResult);

		//	If we return a list, then modify variables

		else if (pResult->IsList())
			{
			//	A single value means we modified the damage to armor

			if (pResult->GetCount() == 1)
				{
				if (strEquals(pResult->GetElement(0)->GetStringValue(), STR_SHIELD_REFLECT))
					{
					Ctx.SetShotReflected(true);
					Ctx.iAbsorb = Ctx.iDamage;
					Ctx.iShieldDamage = 0;
					}
				else
					{
					Ctx.iShieldDamage = Max(0, Min(pResult->GetElement(0)->GetIntegerValue(), Ctx.iHPLeft));
					if (Ctx.IsShotReflected())
						{
						Ctx.SetShotReflected(false);
						Ctx.iAbsorb = Ctx.iOriginalAbsorb;
						}
					}
				}

			//	Two values mean we modified both damage to armor and shield damage

			else if (pResult->GetCount() == 2)
				{
				Ctx.SetShotReflected(false);
				Ctx.iShieldDamage = Max(0, Min(pResult->GetElement(0)->GetIntegerValue(), Ctx.iHPLeft));
				Ctx.iAbsorb = Max(0, Ctx.iDamage - Max(0, pResult->GetElement(1)->GetIntegerValue()));
				}

			//	Otherwise, we deal with reflection

			else
				{
				Ctx.SetShotReflected(strEquals(pResult->GetElement(0)->GetStringValue(), STR_SHIELD_REFLECT));
				Ctx.iShieldDamage = Max(0, Min(pResult->GetElement(1)->GetIntegerValue(), Ctx.iHPLeft));
				Ctx.iAbsorb = Max(0, Ctx.iDamage - Max(0, pResult->GetElement(2)->GetIntegerValue()));
				}
			}

		CCCtx.Discard(pResult);
		}
	}

void CShieldClass::FireOnShieldDown (CInstalledDevice *pDevice, CSpaceObject *pSource)

//	FireOnShieldDown
//
//	Fire OnShieldDown event

	{
	SEventHandlerDesc Event;
	if (FindEventHandlerShieldClass(evtOnShieldDown, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());

		Ctx.DefineContainingType(GetItemType());
		Ctx.SaveAndDefineSourceVar(pSource);
		Ctx.SaveAndDefineItemVar(pSource->GetItemForDevice(pDevice));

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			pSource->ReportEventError(ON_SHIELD_DOWN_EVENT, pResult);
		Ctx.Discard(pResult);
		}
	}

int CShieldClass::GetDamageAdj (const DamageDesc &Damage, const CItemEnhancementStack *pEnhancements) const

//	GetDamageAdj
//
//	Damage adjustment

	{
	//	The adjustment varies by shield class

	int iAdj = m_DamageAdj.GetAdj(Damage.GetDamageType());
	if (iAdj >= CDamageAdjDesc::MAX_DAMAGE_ADJ)
		return CDamageAdjDesc::MAX_DAMAGE_ADJ;

	//	Adjust based on difference in level (negative numbers means the shield
	//	is lower than the damage level):
	//
	//	...
	//	-3	=	4.5x damage
	//	-2	=	4x damage
	//	-1	=	3.5x damage
	//	0	=	3x damage
	//	1	=	2.5x damage
	//	2	=	2x damage
	//	3	=	1.5x damage
	//	>3	=	1x damage

	if (Damage.GetShieldDamageLevel())
		{
		int iShieldAdj = Max(100, 300 + (50 * (Damage.GetShieldDamageLevel() - GetLevel())));
		if (iShieldAdj >= CDamageAdjDesc::MAX_DAMAGE_ADJ)
			return CDamageAdjDesc::MAX_DAMAGE_ADJ;

		iAdj = iAdj * iShieldAdj / 100;
		}

	//	Adjust based on enhancements

	if (pEnhancements)
		{
		int iEnhanceAdj = pEnhancements->GetDamageAdj(Damage);
		if (iEnhanceAdj >= CDamageAdjDesc::MAX_DAMAGE_ADJ)
			return CDamageAdjDesc::MAX_DAMAGE_ADJ;

		iAdj = iAdj * iEnhanceAdj / 100;
		}

	return iAdj;
	}

int CShieldClass::GetDamageEffectiveness (CSpaceObject *pAttacker, CInstalledDevice *pWeapon)

//	GetDamageEffectiveness
//
//	Returns the effectiveness of the given weapon against this shield.
//
//	< 0		The weapon is ineffective against us.
//	0-99	The weapon is less effective than average.
//	100		The weapon has average effectiveness
//	> 100	The weapon is more effective than average.

	{
	const DamageDesc *pDamage = pWeapon->GetDamageDesc(CItemCtx(pAttacker, pWeapon));
	if (pDamage == NULL)
		return 100;

	int iBonus = m_DamageAdj.GetHPBonus(pDamage->GetDamageType());
	if (iBonus <= -100)
		return -1;

	//	Compute score based on bonus

	int iScore;
	if (iBonus <= 0)
		iScore = 100 - iBonus;
	else
		iScore = 100 - Min(100, (iBonus / 2));

	//	See if the weapon does extra damage to shields

	if (pDamage->GetShieldDamageLevel())
		{
		int iAdj = 100 * Max(100, 300 + (50 * (pDamage->GetShieldDamageLevel() - GetLevel()))) / 100;
		iScore += (iAdj / 2);
		}

	//	Done

	return iScore;
	}

int CShieldClass::GetHitPoints (CItemCtx &ItemCtx, int *retiMaxHP) const

//	GetHitPoints
//
//	Returns the number of HP left.

	{
	if (retiMaxHP)
		*retiMaxHP = GetMaxHP(ItemCtx);

	return GetHPLeft(ItemCtx);
	}

int CShieldClass::GetHPLeft (CItemCtx &Ctx) const

//	GetHPLeft
//
//	Returns the number of HP left of shields

	{
	CInstalledDevice *pDevice = Ctx.GetDevice();

	//	If we're not installed, then just return maximum HP

	if (pDevice == NULL)
		return GetMaxHP(Ctx);

	//	Get HP left

	return Max(0, (int)pDevice->GetData());
	}

ICCItem *CShieldClass::FindItemProperty (CItemCtx &Ctx, const CString &sName)

//	FindItemProperty
//
//	Returns a property

	{
	CCodeChain &CC = GetUniverse().GetCC();

	//	Enhancements

	const CItemEnhancementStack *pEnhancements = Ctx.GetEnhancementStack();

	//	Get the property

	if (strEquals(sName, PROPERTY_DAMAGE_ADJ))
		return m_DamageAdj.GetDamageAdjProperty(pEnhancements);

	else if (strEquals(sName, PROPERTY_HP))
		return CC.CreateInteger(GetHPLeft(Ctx));

	else if (strEquals(sName, PROPERTY_HP_BONUS))
		return m_DamageAdj.GetHPBonusProperty(pEnhancements);

	else if (strEquals(sName, PROPERTY_MAX_HP))
		return CC.CreateInteger(GetMaxHP(Ctx));

	else if (strEquals(sName, PROPERTY_REGEN))
		return CC.CreateInteger(mathRound(CalcRegen180(Ctx)));

	//	Otherwise, just get the property from the base class

	else
		return CDeviceClass::FindItemProperty(Ctx, sName);
	}

int CShieldClass::GetMaxHP (CItemCtx &Ctx) const

//	GetMaxHP
//
//	Max HP of shields
	
	{
	CSpaceObject *pSource = Ctx.GetSource();
	CInstalledDevice *pDevice = Ctx.GetDevice();

	int iMax = m_iHitPoints;

	//	Adjust based on charges

	if (m_iExtraHPPerCharge)
		iMax = Max(0, iMax + m_iExtraHPPerCharge * Ctx.GetItemCharges());
	else if (m_fHasNonRegenHPBonus)
		iMax = Max(0, iMax + Ctx.GetItemCharges());

	//	Adjust if shield is based on armor strength

	CShip *pShip;
	if (m_iArmorShield && pSource && (pShip = pSource->AsShip()))
		{
		//	Compute the average HP of all the armor

		int iArmorHP = 0;
		int iArmorCount = pShip->GetArmorSectionCount();
		for (int i = 0; i < iArmorCount; i++)
			iArmorHP += pShip->GetArmorSection(i)->GetHitPoints();

		if (iArmorCount > 0)
			iArmorHP = ((m_iArmorShield * iArmorHP / iArmorCount) + 5) / 10;

		//	Return HP left

		iMax = Min(iMax, iArmorHP);
		}

	//	Fire event

	if (pDevice && pSource)
		iMax = FireGetMaxHP(pDevice, pSource, iMax);

	//	Adjust based on enhancements

	const CItemEnhancementStack *pEnhancements = Ctx.GetEnhancementStack();
	if (pEnhancements)
		iMax = iMax + ((iMax * pEnhancements->GetBonus()) / 100);

	//	Done

	return iMax;
	}

int CShieldClass::GetPowerRating (CItemCtx &Ctx, int *retiIdlePowerUse) const

//	GetPowerRating
//
//	Returns the power rating of the item

	{
	int iPower = m_iPowerUse;
	int iIdlePower = m_iIdlePowerUse;

	if (m_iExtraPowerPerCharge)
		{
		iPower += m_iExtraHPPerCharge * Ctx.GetItemCharges();

		if (m_iPowerUse > 0)
			{
			Metric rAdj = (Metric)m_iIdlePowerUse / m_iPowerUse;
			iIdlePower += mathRound(m_iExtraHPPerCharge * rAdj * Ctx.GetItemCharges());
			}
		else
			iIdlePower += (m_iExtraHPPerCharge * Ctx.GetItemCharges()) / 2;
		}

	const CItemEnhancementStack *pEnhancements = Ctx.GetEnhancementStack();
	if (pEnhancements)
		{
		int iAdj = pEnhancements->GetPowerAdj();
		iPower = iPower * iAdj / 100;
		iIdlePower = iIdlePower * iAdj / 100;
		}

	if (retiIdlePowerUse)
		*retiIdlePowerUse = iIdlePower;

	return iPower;
	}

bool CShieldClass::GetReferenceDamageAdj (const CItem *pItem, CSpaceObject *pInstalled, int *retiHP, int *retArray) const

//	GetReferenceDamageAdj
//
//	Returns an array of damage adj values

	{
	int i;

	CItemCtx Ctx(pItem, pInstalled);
	const CItemEnhancementStack *pEnhancements = Ctx.GetEnhancementStack();

	int iMinHP, iMaxHP;
	CalcMinMaxHP(Ctx, pItem->GetCharges(), 0, 0, &iMinHP, &iMaxHP);

	if (retiHP)
		*retiHP = iMaxHP;

	if (retArray)
		{
		for (i = 0; i < damageCount; i++)
			{
			DamageDesc Damage((DamageTypes)i, DiceRange(0, 0, 0));
			int iAdj = GetDamageAdj(Damage, pEnhancements);
			if (iAdj == 0)
				retArray[i] = -1;
			else
				retArray[i] = mathRound(iMaxHP * 100.0 / iAdj);
			}
		}

	return true;
	}

void CShieldClass::GetStatus (CInstalledDevice *pDevice, CSpaceObject *pSource, int *retiStatus, int *retiMaxStatus)

//	GetStatus
//
//	Returns the status of the shields

	{
	CItemCtx Ctx(pSource, pDevice);

	*retiStatus = GetHPLeft(Ctx);
	*retiMaxStatus = GetMaxHP(Ctx);
	}

Metric CShieldClass::GetStdCost (int iLevel)

//	GetStdCost
//
//	Returns the standard price of a shield at this level

	{
	if (iLevel >= 1 && iLevel <= MAX_ITEM_LEVEL)
		return STD_STATS[iLevel - 1].rCost;
	else
		return 0.0;
	}

int CShieldClass::GetStdEffectiveHP (CUniverse &Universe, int iLevel)

//	GetStdEffectiveHP
//
//	Returns the effective HP of a shield at this level

	{
	if (iLevel < 1 || iLevel > MAX_ITEM_LEVEL)
		return -1;

	int iHPbyDamageType[damageCount];
	for (int i = 0; i < damageCount; i++)
		iHPbyDamageType[i] = CalcHPDamageAdj(STD_STATS[iLevel - 1].iHP, Universe.GetShieldDamageAdj(iLevel)->GetAdj((DamageTypes)i));

	return ::CalcEffectiveHP(iLevel, STD_STATS[iLevel - 1].iHP, iHPbyDamageType);
	}

int CShieldClass::GetStdHP (int iLevel)

//	GetStdHP
//
//	Returns the standard HP for a shield at the given level

	{
	if (iLevel >= 1 && iLevel <= MAX_ITEM_LEVEL)
		return STD_STATS[iLevel - 1].iHP;
	else
		return -1;
	}

int CShieldClass::GetStdPower (int iLevel)

//	GetStdPower
//
//	Returns the standard power consumed by a shield at the given level (in 1/10th MW)

	{
	if (iLevel >= 1 && iLevel <= MAX_ITEM_LEVEL)
		return STD_STATS[iLevel - 1].iPower;
	else
		return -1;
	}

int CShieldClass::GetStdRegen (int iLevel)

//	GetStdRegen
//
//	Returns the standard regen for a shield at the given level (in hp/180 ticks)

	{
	if (iLevel >= 1 && iLevel <= MAX_ITEM_LEVEL)
		return STD_STATS[iLevel - 1].iRegen;
	else
		return -1;
	}

bool CShieldClass::IsDepleted (CInstalledDevice *pDevice)

//	IsDepleted
//
//	Returns TRUE if shields depleted

	{
	return ((int)pDevice->GetData() < 0); 
	}

void CShieldClass::OnAccumulateAttributes (CItemCtx &ItemCtx, const CItem &Ammo, TArray<SDisplayAttribute> *retList)

//	OnAccumulateAttributes
//
//	Returns display attributes

	{
	int i;

	//	Reflection

	for (i = 0; i < damageCount; i++)
		{
		if (m_Reflective.InSet((DamageTypes)i))
			retList->Insert(SDisplayAttribute(attribPositive, strPatternSubst(CONSTLIT("%s reflecting"), GetDamageShortName((DamageTypes)i))));
		}

	//	Weapon suppress

	for (i = 0; i < damageCount; i++)
		{
		if (m_WeaponSuppress.InSet((DamageTypes)i))
			retList->Insert(SDisplayAttribute(attribNegative, strPatternSubst(CONSTLIT("%s suppressing"), GetDamageShortName((DamageTypes)i))));
		}
	}

void CShieldClass::OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed)

//	OnAddTypesUsed
//
//	Adds types used by this class

	{
	retTypesUsed->SetAt(m_pHitEffect.GetUNID(), true);
	}

ALERROR CShieldClass::OnDesignLoadComplete (SDesignLoadCtx &Ctx)

//	OnDesignLoadComplete
//
//	Bind design

	{
	ALERROR error;

	//	Compute shield damage adjustments

	if (error = m_DamageAdj.Bind(Ctx, GetUniverse().GetShieldDamageAdj(m_iDamageAdjLevel)))
		return error;

	//	Load events

	CItemType *pType = GetItemType();
	pType->InitCachedEvents(evtCount, CACHED_EVENTS, m_CachedEvents);

	//	Hit effect

	if (error = m_pHitEffect.Bind(Ctx))
		return error;

	//	If the hit effect is NULL, then use default

	if (m_pHitEffect == NULL)
		m_pHitEffect.Set(GetUniverse().FindEffectType(g_ShieldEffectUNID));

	return NOERROR;
	}

CEffectCreator *CShieldClass::OnFindEffectCreator (const CString &sUNID)

//	OnFindEffectCreator
//
//	Find the effect. We start after the shield class UNID.
//
//	{unid}:h		Hit effect

	{
	//	We start after the shield class UNID

	char *pPos = sUNID.GetASCIIZPointer();
	if (*pPos != ':')
		return NULL;

	pPos++;

	switch (*pPos)
		{
		case 'h':
			return m_pHitEffect;

		default:
			return NULL;
		}
	}

int CShieldClass::GetReferenceDepletionDelay (void) const

//	GetReferenceDepletionDelay
//
//	If the shield's depletion delay is longer than standard, we return the 
//	percent increase. For example, if the depletion delay is twice as long, we
//	return 100 (100% longer).
//
//	If the depletion delay is shorter, we return the percent decrease as a 
//	negative number. For example, if we decrease from 360 to 270 then we are
//	25% shorter and return -25.
//
//	If we have no depletion delay, we return 0.

	{
	if (m_iDepletionTicks > STD_DEPLETION_DELAY)
		return mathRound(100.0 * (m_iDepletionTicks - STD_DEPLETION_DELAY) / STD_DEPLETION_DELAY);
	else if (m_iDepletionTicks > 1)
		return -mathRound(100.0 * (STD_DEPLETION_DELAY - m_iDepletionTicks) / STD_DEPLETION_DELAY);
	else
		return 0;
	}

CString CShieldClass::OnGetReference (CItemCtx &Ctx, const CItem &Ammo, DWORD dwFlags)

//	GetReference
//
//	Returns a string that describes the basic attributes
//	of this shield
//
//	Example:
//
//		20 hp (average regen); 100MW

	{
	CString sReference;

	//	Compute the regeneration

	Metric rRegen = CalcRegen180(Ctx, FLAG_IGNORE_DISABLED);
	if (rRegen <= 0.0)
		sReference = CONSTLIT("manual regen");
	else
		sReference = strPatternSubst("regen @ %s", CLanguage::ComposeNumber(CLanguage::numberRegenRate, rRegen));

	//	If we have a non-standard depletion delay, show that.

	if (m_iDepletionTicks != STD_DEPLETION_DELAY && rRegen > 0.0)
		{
		int iPercent = GetReferenceDepletionDelay();
		if (iPercent == 0)
			AppendReferenceString(&sReference, CONSTLIT("instant recovery time"));
		else if (iPercent >= 100)
			{
			int iMultiple10 = mathRound(10.0 * (iPercent + 100.0) / 100.0);
			if ((iMultiple10 % 10) == 0)
				AppendReferenceString(&sReference, strPatternSubst(CONSTLIT("%d%&times; longer recovery"), iMultiple10 / 10));
			else
				AppendReferenceString(&sReference, strPatternSubst(CONSTLIT("%d.%d%&times; longer recovery"), iMultiple10 / 10, iMultiple10 % 10));
			}
		else if (iPercent > 0)
			AppendReferenceString(&sReference, strPatternSubst(CONSTLIT("%d%% longer recovery"), iPercent));
		else
			AppendReferenceString(&sReference, strPatternSubst(CONSTLIT("%d%% shorter recovery"), -iPercent));
		}

	return sReference;
	}

void CShieldClass::OnInstall (CInstalledDevice *pDevice, CSpaceObject *pSource, CItemListManipulator &ItemList)

//	OnInstall
//
//	Called when the device is installed

	{
	//	NOTE: We in the middle of CInstalledDevice::Install, and not all variables
	//	are set. In particular, the device's m_pItem variable is not initialized yet,
	//	so we pass in the item from the cursor (otherwise we might not get the proper
	//	charges count).
	//
	//	LATER: see if we can reorder CInstalledDevice::Install to define m_pItem
	//	first.

	CItemCtx Ctx(&ItemList.GetItemAtCursor(), pSource, pDevice);

	//	Set shields to max HP

	SetHPLeft(pDevice, pSource, GetMaxHP(Ctx));

	//	Identified

	if (pSource->IsPlayer())
		ItemList.GetItemAtCursor().SetKnown();
	}

void CShieldClass::OnMarkImages (void)

//	OnMarkImages
//
//	Marks images used by shield

	{
	if (m_pHitEffect)
		m_pHitEffect->MarkImages();
	}

void CShieldClass::Recharge (CInstalledDevice *pDevice, CShip *pShip, int iStatus)

//	Recharge
//
//	Recharges hitpoints (this is a manual recharge, so we ignore whether we are
//	depleted or not).

	{
	CItemCtx Ctx(pShip, pDevice);

	int iMaxHP = GetMaxHP(Ctx);
	int iHPLeft = GetHPLeft(Ctx);
	SetHPLeft(pDevice, pShip, Max(0, Min(iMaxHP, iHPLeft + iStatus)));
	pShip->OnComponentChanged(comShields);
	}

bool CShieldClass::RequiresItems (void) const

//	RequiresItems
//
//	Shield requires some other item to function

	{
#ifdef LATER
	//	Need to explicitly list superconducting coils as a required
	//	item for these shields to function
#else
	return m_Regen.IsEmpty();
#endif
	}

void CShieldClass::Reset (CInstalledDevice *pDevice, CSpaceObject *pSource)

//	Reset
//
//	Lower shields

	{
	//	Note: We do not call Deplete because we don't want to invoke the
	//	OnShieldDown event

	SetHPLeft(pDevice, pSource, 0);
	pSource->OnComponentChanged(comShields);
	}

void CShieldClass::SetDepleted (CInstalledDevice *pDevice, CSpaceObject *pSource)

//	SetDepleted
//
//	Makes shields depleted

	{
	pDevice->SetData((DWORD)(-m_iDepletionTicks));

	if (m_fHasNonRegenHPBonus)
		pDevice->SetCharges(pSource, 0);

	//	Fire event (We don't fire the event if we're disabled because we
	//	don't want something like the Invincible deflector to disable the ship
	//	if the shield is not enabled)

	if (pDevice->IsEnabled())
		FireOnShieldDown(pDevice, pSource);
	}

void CShieldClass::SetHPLeft (CInstalledDevice *pDevice, CSpaceObject *pSource, int iHP, bool bConsumeCharges)

//	SetHPLeft
//
//	Sets HP left on shields

	{
	CItemCtx Ctx(pSource, pDevice);
	if (bConsumeCharges 
			&& m_fHasNonRegenHPBonus 
			&& iHP < GetMaxHP(Ctx))
		pDevice->SetCharges(pSource, Max(0, iHP - (GetMaxHP(Ctx) - pDevice->GetCharges(pSource))));
		
	pDevice->SetData((DWORD)iHP);
	}

void CShieldClass::SetHitPoints (CItemCtx &ItemCtx, int iHP)

//	SetHitPoints
//
//	Sets hit points left.

	{
	CSpaceObject *pSource = ItemCtx.GetSource();
	CInstalledDevice *pDevice = ItemCtx.GetDevice();
	if (pSource == NULL || pDevice == NULL)
		return;

	SetHPLeft(pDevice, pSource, Min(GetMaxHP(ItemCtx), iHP), true);
	}

bool CShieldClass::SetItemProperty (CItemCtx &Ctx, const CString &sName, ICCItem *pValue, CString *retsError)

//	SetItemProperty
//
//	Sets an item property

	{
	CSpaceObject *pSource = Ctx.GetSource();
	CInstalledDevice *pDevice = Ctx.GetDevice();

	//	This item must be installed on the given source, otherwise, it desn't
	//	work.

	if (pSource == NULL
			|| pDevice == NULL
			|| !Ctx.GetItem().IsInstalled())
		{
		*retsError = CONSTLIT("Item is not an installed device on object.");
		return false;
		}

	//	Handle it.

	if (strEquals(sName, PROPERTY_HP))
		{
		//	Nil means we're depleting the shields

		if (pValue->IsNil())
			Deplete(pDevice, pSource);

		//	Otherwise, set it to an HP value

		else
			{
			int iHP = pValue->GetIntegerValue();
			int iMaxHP = GetMaxHP(Ctx);

			SetHPLeft(pDevice, pSource, Min(iMaxHP, iHP));
			pSource->OnComponentChanged(comShields);
			}
		}

	//	Let our subclass handle it.

	else
		return CDeviceClass::SetItemProperty(Ctx, sName, pValue, retsError);

	//	Success

	return true;
	}

void CShieldClass::Update (CInstalledDevice *pDevice, CSpaceObject *pSource, SDeviceUpdateCtx &Ctx)

//	Update
//
//	Updates the shield

	{
	DEBUG_TRY

	const CShipPerformanceDesc &Perf = pSource->GetShipPerformance();
	CItemCtx ItemCtx(pSource, pDevice);

	//	Initialize to not regenerating

	pDevice->SetRegenerating(false);

	//	If there's shield interference, then we disable ourselves

	if (Perf.HasShieldInterference() && pDevice->IsEnabled())
		{
		Ctx.bSetDisabled = true;
		}

	//	If we're not enabled then we drop shields

	if (!pDevice->IsEnabled() || Ctx.bSetDisabled)
		{
		if (GetHPLeft(ItemCtx))
			{
			//	Note: We don't call SetDepleted because we don't want to fire the OnShieldDown
			//	event. If necessary, we should add an OnDeviceDisabled event.

			SetHPLeft(pDevice, pSource, 0);
			pSource->OnComponentChanged(comShields);
			}

		return;
		}

	//	If we're damaged, then there is a chance that we might be depleted

	if (pDevice->IsDamaged() || pDevice->IsDisrupted())
		{
		if ((Ctx.iTick % 10) == 0 && mathRandom(1, 100) <= 5)
			{
			Deplete(pDevice, pSource);
			pSource->OnDeviceStatus(pDevice, failShieldFailure);
			}
		}

	//	If we're depleted, then we don't increase
	//	HP (but we still set the regenerating flag
	//	because we will consume power)

	if (UpdateDepleted(pDevice))
		pDevice->SetRegenerating(true);

	//	Otherwise, see if need to regenerate

	else
		{
		int iMaxHP = GetMaxHP(ItemCtx);
		int iHPLeft = GetHPLeft(ItemCtx);

		if (iHPLeft != iMaxHP)
			{
			int iRegenHP = m_Regen.GetRegen(Ctx.iTick);

			//	Figure out how much to regen

			if (m_iExtraRegenPerCharge && ((Ctx.iTick % 30) == 0))
				{
				int iCharges = pDevice->GetCharges(pSource);
				int iExtra180 = (m_iExtraRegenPerCharge * iCharges);
				int iExtra30 = iExtra180 / 6;
				int iExtraRemainder = iExtra180 % 6;

				iRegenHP += iExtra30;
				if (mathRandom(1, 6) <= iExtraRemainder)
					iRegenHP += 1;
				}

			//	If we're adjusting regeneration by shield level, then 
			//	adjust now.

			if (m_fRegenByShieldLevel && iRegenHP > 0)
				{
				constexpr Metric rMinRegen = 0.1;
				constexpr Metric rMaxRegen = 2.0;
				constexpr Metric rRange = rMaxRegen - rMinRegen;

				Metric rRegen = rMinRegen + (rRange * (iHPLeft / (Metric)iMaxHP));
				iRegenHP = mathRoundStochastic(rRegen * iRegenHP);
				}

			//	If we have non-regenerating HP and we're below that level,
			//	then bring it back up. This can happen when we disable and
			//	re-enable a device.

			if (m_fHasNonRegenHPBonus
					&& iHPLeft + iRegenHP < iMaxHP
					&& pDevice->GetCharges(pSource) > 0)
				iRegenHP = iMaxHP - iHPLeft;

			//	Regen

			SetHPLeft(pDevice, pSource, Min(iMaxHP, iHPLeft + iRegenHP));
			pSource->OnComponentChanged(comShields);

			//	Remember that we regenerated this turn (so that we can
			//	consume power)

			pDevice->SetRegenerating(true);
			}
		}

	DEBUG_CATCH
	}

bool CShieldClass::UpdateDepleted (CInstalledDevice *pDevice)

//	UpdateDepleted
//
//	Updates depleted state. Returns FALSE if we are not
//	depleted.

	{
	int iLeft = (int)pDevice->GetData();
	if (iLeft < 0)
		{
		iLeft++;
		pDevice->SetData((DWORD)iLeft);
		return true;
		}
	else
		return false;
	}
