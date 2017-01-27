//	CArmorClass.cpp
//
//	CArmorClass class

#include "PreComp.h"

#define BLINDING_DAMAGE_ADJ_ATTRIB				CONSTLIT("blindingDamageAdj")
#define BLINDING_IMMUNE_ATTRIB					CONSTLIT("blindingImmune")
#define CHARGE_DECAY_ATTRIB						CONSTLIT("chargeDecay")
#define CHARGE_REGEN_ATTRIB						CONSTLIT("chargeRegen")
#define COMPLETE_BONUS_ATTRIB					CONSTLIT("completeBonus")
#define DAMAGE_ADJ_LEVEL_ATTRIB					CONSTLIT("damageAdjLevel")
#define DECAY_ATTRIB							CONSTLIT("decay")
#define DECAY_RATE_ATTRIB						CONSTLIT("decayRate")
#define DEVICE_CRITERIA_ATTRIB					CONSTLIT("deviceCriteria")
#define DEVICE_DAMAGE_ADJ_ATTRIB				CONSTLIT("deviceDamageAdj")
#define DEVICE_DAMAGE_IMMUNE_ATTRIB				CONSTLIT("deviceDamageImmune")
#define DEVICE_HP_BONUS_ATTRIB					CONSTLIT("deviceHPBonus")
#define DISTRIBUTE_ATTRIB						CONSTLIT("distribute")
#define DISINTEGRATION_IMMUNE_ATTRIB			CONSTLIT("disintegrationImmune")
#define ENHANCEMENT_TYPE_ATTRIB					CONSTLIT("enhancementType")
#define EMP_DAMAGE_ADJ_ATTRIB					CONSTLIT("EMPDamageAdj")
#define EMP_IMMUNE_ATTRIB						CONSTLIT("EMPImmune")
#define IDLE_POWER_USE_ATTRIB					CONSTLIT("idlePowerUse")
#define INSTALL_COST_ATTRIB						CONSTLIT("installCost")
#define INSTALL_COST_ADJ_ATTRIB					CONSTLIT("installCostAdj")
#define MAX_HP_BONUS_ATTRIB						CONSTLIT("maxHPBonus")
#define MAX_SPEED_BONUS_ATTRIB					CONSTLIT("maxSpeedBonus")
#define PHOTO_RECHARGE_ATTRIB					CONSTLIT("photoRecharge")
#define PHOTO_REPAIR_ATTRIB						CONSTLIT("photoRepair")
#define POWER_USE_ATTRIB						CONSTLIT("powerUse")
#define RADIATION_IMMUNE_ATTRIB					CONSTLIT("radiationImmune")
#define REFLECT_ATTRIB							CONSTLIT("reflect")
#define REGEN_ATTRIB							CONSTLIT("regen")
#define REPAIR_COST_ATTRIB						CONSTLIT("repairCost")
#define REPAIR_COST_ADJ_ATTRIB					CONSTLIT("repairCostAdj")
#define REPAIR_RATE_ATTRIB						CONSTLIT("repairRate")
#define REPAIR_TECH_ATTRIB						CONSTLIT("repairTech")
#define SHATTER_IMMUNE_ATTRIB					CONSTLIT("shatterImmune")
#define SHIELD_INTERFERENCE_ATTRIB				CONSTLIT("shieldInterference")
#define STEALTH_ATTRIB							CONSTLIT("stealth")
#define UNID_ATTRIB								CONSTLIT("unid")
#define HEALER_REGEN_ATTRIB 					CONSTLIT("useHealerToRegen")

#define GET_MAX_HP_EVENT						CONSTLIT("GetMaxHP")
#define ON_ARMOR_DAMAGE_EVENT					CONSTLIT("OnArmorDamage")

#define FIELD_ADJUSTED_HP						CONSTLIT("adjustedHP")
#define FIELD_BALANCE							CONSTLIT("balance")
#define FIELD_DAMAGE_ADJ						CONSTLIT("damageAdj")
#define FIELD_EFFECTIVE_HP						CONSTLIT("effectiveHP")
#define FIELD_HP								CONSTLIT("hp")
#define FIELD_HP_BONUS							CONSTLIT("hpBonus")
#define FIELD_INSTALL_COST						CONSTLIT("installCost")
#define FIELD_REGEN								CONSTLIT("regen")
#define FIELD_REPAIR_COST						CONSTLIT("repairCost")
#define FIELD_SHIELD_INTERFERENCE				CONSTLIT("shieldInterference")

#define PROPERTY_BLINDING_IMMUNE				CONSTLIT("blindingImmune")
#define PROPERTY_COMPLETE_HP					CONSTLIT("completeHP")
#define PROPERTY_COMPLETE_SET					CONSTLIT("completeSet")
#define PROPERTY_DEVICE_DAMAGE_IMMUNE			CONSTLIT("deviceDamageImmune")
#define PROPERTY_DEVICE_DISRUPT_IMMUNE			CONSTLIT("deviceDisruptImmune")
#define PROPERTY_DISINTEGRATION_IMMUNE			CONSTLIT("disintegrationImmune")
#define PROPERTY_EMP_IMMUNE						CONSTLIT("EMPImmune")
#define PROPERTY_HP								CONSTLIT("hp")
#define PROPERTY_HP_BONUS						CONSTLIT("hpBonus")
#define PROPERTY_MAX_HP							CONSTLIT("maxHP")
#define PROPERTY_PRIME_SEGMENT					CONSTLIT("primeSegment")
#define PROPERTY_RADIATION_IMMUNE				CONSTLIT("radiationImmune")
#define PROPERTY_REPAIR_COST					CONSTLIT("repairCost")
#define PROPERTY_REPAIR_LEVEL					CONSTLIT("repairLevel")
#define PROPERTY_SHATTER_IMMUNE					CONSTLIT("shatterImmune")

static char g_HitPointsAttrib[] = "hitPoints";
static char g_DamageAdjAttrib[] = "damageAdj";
static char g_ItemIDAttrib[] = "itemID";
#define MAX_REFLECTION_CHANCE		95

#define MAX_REFLECTION_CHANCE		95

const int BLIND_IMMUNE_LEVEL =					6;
const int RADIATION_IMMUNE_LEVEL =				7;
const int EMP_IMMUNE_LEVEL =					9;
const int DEVICE_DAMAGE_IMMUNE_LEVEL =			11;

const int TICKS_PER_UPDATE =					10;

static CArmorClass::SStdStats STD_STATS[MAX_ITEM_LEVEL] =
	{
		//						Repair	Install
		//	HP		Cost		cost	cost		Mass
		{	35,		50,			1,		10,			2500, },	
		{	45,		100,		1,		20,			2600, },
		{	60,		200,		1,		40,			2800, },
		{	80,		400,		2,		80,			2900, },
		{	100,	800,		3,		160,		3000, },

		{	135,	1600,		4,		320,		3200, },
		{	175,	3200,		6,		640,		3300, },
		{	225,	6500,		9,		1300,		3500, },
		{	300,	13000,		15,		2600,		3700, },
		{	380,	25000,		22,		5000,		3900, },

		{	500,	50000,		35,		10000,		4000, },
		{	650,	100000,		52,		20000,		4300, },
		{	850,	200000,		80,		40000,		4500, },
		{	1100,	410000,		125,	82000,		4700, },
		{	1400,	820000,		190,	164000,		5000, },

		{	1850,	1600000,	300,	320000,		5200, },
		{	2400,	3250000,	450,	650000,		5500, },
		{	3100,	6500000,	700,	1300000,	5700, },
		{	4000,	13000000,	1050,	2600000,	6000, },
		{	5250,	26000000,	1650,	5200000,	6300, },

		{	6850,	52000000,	2540,	10400000,	6600, },
		{	9000,	100000000,	3900,	20000000,	7000, },
		{	12000,	200000000,	6000,	40000000,	7300, },
		{	15000,	400000000,	9300,	80000000,	7700, },
		{	20000,	800000000,	14300,	160000000,	8000, },
	};

static char *CACHED_EVENTS[CArmorClass::evtCount] =
	{
		"GetMaxHP",
		"OnArmorDamage",
	};

CArmorClass::CArmorClass (void) :
		m_pItemType(NULL),
        m_iScaledLevels(0),
        m_pScalable(NULL)

//	CArmorClass constructor

	{
	}

CArmorClass::~CArmorClass (void)

//  CArmorClass destructor

    {
    if (m_pScalable)
        delete[] m_pScalable;
    }

EDamageResults CArmorClass::AbsorbDamage (CItemCtx &ItemCtx, SDamageCtx &Ctx)

//	AbsorbDamage
//
//	Handles getting hit by damage.
//
//	Returns damageNoDamage if all the damage was absorbed and no further processing is necessary
//	Returns damageDestroyed if the source was destroyed
//	Returns damageArmorHit if source was damage and further processing (destroy check) is needed
//
//	Sets Ctx.iDamage to the amount of hit points left after damage absorption.

	{
	DEBUG_TRY

	CSpaceObject *pSource = ItemCtx.GetSource();
	CInstalledArmor *pArmor = ItemCtx.GetArmor();
	if (pSource == NULL || pArmor == NULL)
		return damageNoDamage;

	//	Compute all the effects (this initializes elements in Ctx).

	CalcDamageEffects(ItemCtx, Ctx);

	//	First give custom weapons a chance

	bool bCustomDamage = Ctx.pDesc->FireOnDamageArmor(Ctx);
	if (pSource->IsDestroyed())
		return damageDestroyed;

	//	Damage adjustment

	CalcAdjustedDamage(ItemCtx, Ctx);

	//	If the armor has custom code to deal with damage, handle it here.

	FireOnArmorDamage(ItemCtx, Ctx);
	if (pSource->IsDestroyed())
		return damageDestroyed;

	//	If this armor section reflects this kind of damage then
	//	send the damage on

	if (Ctx.bReflect)
		{
		if (Ctx.pCause)
			Ctx.pCause->CreateReflection(Ctx.vHitPos, (Ctx.iDirection + 120 + mathRandom(0, 120)) % 360);
		return damageNoDamage;
		}

	//	If this is a disintegration attack, then disintegrate the ship

	if (Ctx.bDisintegrate)
		{
		if (!pSource->OnDestroyCheck(killedByDisintegration, Ctx.Attacker))
			return damageNoDamage;

		pSource->Destroy(killedByDisintegration, Ctx.Attacker);
		return damageDestroyed;
		}

	//	If this is a shatter attack, see if the ship is destroyed

	if (Ctx.bShatter)
		{
		if (!pSource->OnDestroyCheck(killedByShatter, Ctx.Attacker))
			return damageNoDamage;

		pSource->Destroy(killedByShatter, Ctx.Attacker);
		return damageDestroyed;
		}

	//	If this is a paralysis attack and we've gotten past the shields
	//	then freeze the ship.

	if (Ctx.bParalyze)
		pSource->MakeParalyzed(Ctx.iParalyzeTime);

	//	If this is blinding damage then our sensors are disabled

	if (Ctx.bBlind)
		pSource->MakeBlind(Ctx.iBlindTime);

	//	If this attack is radioactive, then contaminate the ship

	if (Ctx.bRadioactive)
		pSource->OnHitByRadioactiveDamage(Ctx);

	//	If this is device damage, then see if any device is damaged

	if (Ctx.bDeviceDamage)
		pSource->OnHitByDeviceDamage();

	if (Ctx.bDeviceDisrupt)
		pSource->OnHitByDeviceDisruptDamage(Ctx.iDisruptTime);

	//	Create a hit effect. (Many weapons show an effect even if no damage was
	//	done.)

	if (!Ctx.bNoHitEffect)
		Ctx.pDesc->CreateHitEffect(pSource->GetSystem(), Ctx);

	//	If no damage has reached us, then we're done

	if (Ctx.iDamage == 0 && !bCustomDamage)
		return damageNoDamage;

	//	Give source events a chance to change the damage before we
	//	subtract from armor.

	if (pSource->HasOnDamageEvent())
		{
		pSource->FireOnDamage(Ctx);
		if (pSource->IsDestroyed())
			return damageDestroyed;
		}

	//	Take damage

	if (Ctx.iDamage <= pArmor->GetHitPoints())
		{
		pArmor->IncHitPoints(-Ctx.iDamage);
		Ctx.iDamage = 0;
		}
	else
		{
		Ctx.iDamage -= pArmor->GetHitPoints();
		pArmor->SetHitPoints(0);
		}

	return damageArmorHit;

	DEBUG_CATCH
	}

void CArmorClass::AccumulateAttributes (CItemCtx &ItemCtx, TArray<SDisplayAttribute> *retList)

//	AccumulateAttributes
//
//	Returns list of display attributes. NOTE: We only include our intrinsic 
//	attributes--enhancements are added later by the caller.

	{
	int i;
    const SScalableStats &Stats = GetScaledStats(ItemCtx);

	//	If we require a higher level to repair

	if (GetRepairTech() != Stats.iLevel)
		retList->Insert(SDisplayAttribute(attribNeutral, strPatternSubst(CONSTLIT("repair level %d"), GetRepairTech())));

	//	Radiation 

	if (Stats.fRadiationImmune)
		{
		if (Stats.iLevel < RADIATION_IMMUNE_LEVEL)
			retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("radiation immune")));
		}
	else if (Stats.iLevel >= RADIATION_IMMUNE_LEVEL)
		retList->Insert(SDisplayAttribute(attribNegative, CONSTLIT("radiation vulnerable")));

	//	If we're immune to blinding/EMP/device damage, then collapse
	//	it all under a single entry

	bool bCheckedBlind = false;
	bool bCheckedEMP = false;
	bool bCheckedDevice = false;

	if ((Stats.iBlindingDamageAdj == 0)
			&& (Stats.iEMPDamageAdj == 0)
			&& (Stats.iDeviceDamageAdj < 100))
		{
		if (Stats.iLevel < DEVICE_DAMAGE_IMMUNE_LEVEL)
			retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("ionize immune")));

		bCheckedBlind = true;
		bCheckedEMP = true;
		bCheckedDevice = true;
		}

	//	Blindness

	if (!bCheckedBlind)
		{
		if (Stats.iBlindingDamageAdj == 0)
			{
			if (Stats.iLevel < BLIND_IMMUNE_LEVEL)
				retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("blind immune")));
			}
		else if (Stats.iBlindingDamageAdj < 100)
			{
			if (Stats.iLevel < BLIND_IMMUNE_LEVEL)
				retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("blind resistant")));
			else
				retList->Insert(SDisplayAttribute(attribNegative, CONSTLIT("blind vulnerable")));
			}
		else if (Stats.iLevel >= BLIND_IMMUNE_LEVEL)
			retList->Insert(SDisplayAttribute(attribNegative, CONSTLIT("blind vulnerable")));
		}

	//	EMP

	if (!bCheckedEMP)
		{
		if (Stats.iEMPDamageAdj == 0)
			{
			if (Stats.iLevel < EMP_IMMUNE_LEVEL)
				retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("EMP immune")));
			}
		else if (Stats.iEMPDamageAdj < 100)
			{
			if (Stats.iLevel < EMP_IMMUNE_LEVEL)
				retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("EMP resistant")));
			else
				retList->Insert(SDisplayAttribute(attribNegative, CONSTLIT("EMP vulnerable")));
			}
		else if (Stats.iLevel >= EMP_IMMUNE_LEVEL)
			retList->Insert(SDisplayAttribute(attribNegative, CONSTLIT("EMP vulnerable")));
		}

	//	Device damage

	if (!bCheckedDevice)
		{
		if (Stats.iDeviceDamageAdj < 100)
			{
			if (Stats.iLevel < DEVICE_DAMAGE_IMMUNE_LEVEL)
				retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("device protect")));
			}
		else if (Stats.iLevel >= DEVICE_DAMAGE_IMMUNE_LEVEL)
			retList->Insert(SDisplayAttribute(attribNegative, CONSTLIT("device vulnerable")));
		}

	//	Disintegration

	if (m_fDisintegrationImmune)
		retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("disintegration immune")));

	//	Shatter

	if (m_fShatterImmune)
		retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("shatter immune")));

	//	Shield interference

	if (m_fShieldInterference)
		retList->Insert(SDisplayAttribute(attribNegative, CONSTLIT("no shields")));

	//	Photo repair

	if (m_fPhotoRepair)
		retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("photo-regen")));

	//	Solar power

	if (m_fPhotoRecharge)
		retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("solar")));

	//	Regeneration

	if (!Stats.Regen.IsEmpty() && !m_fPhotoRepair)
		retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("regen")));

	//	Decay

	if (!Stats.Decay.IsEmpty())
		retList->Insert(SDisplayAttribute(attribNegative, CONSTLIT("decay")));

	//	Distribution

	if (!Stats.Distribute.IsEmpty())
		retList->Insert(SDisplayAttribute(attribPositive, CONSTLIT("distributing")));

	//	Per damage-type bonuses

	for (i = 0; i < damageCount; i++)
		{
		if (m_Reflective.InSet((DamageTypes)i))
			retList->Insert(SDisplayAttribute(attribPositive, strPatternSubst(CONSTLIT("%s reflect"), GetDamageShortName((DamageTypes)i))));
		}
	}

bool CArmorClass::AccumulateEnhancements (CItemCtx &ItemCtx, CInstalledDevice *pTarget, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements)

//	AccumulateEnhancements
//
//	Adds enhancements to installed devices

	{
	//	See if we give HP bonus to devices

	if (m_iDeviceBonus != 0)
		{
		CInstalledArmor *pArmor = ItemCtx.GetArmor();
		CSpaceObject *pSource = ItemCtx.GetSource();

		//	If the target item does not match our criteria, then no enhancement

		if (pSource 
				&& pTarget
				&& !pSource->GetItemForDevice(pTarget).MatchesCriteria(m_DeviceCriteria))
			return false;

		//	If this enhancement type has already been applied, then nothing

		if (!m_sEnhancementType.IsBlank()
				&& EnhancementIDs.Find(m_sEnhancementType))
			return false;

		//	Add HP bonus enhancements

		pEnhancements->InsertHPBonus(m_iDeviceBonus);
		if (!m_sEnhancementType.IsBlank())
			EnhancementIDs.Insert(m_sEnhancementType);

		return true;
		}

	return false;
	}

bool CArmorClass::AccumulatePerformance (CItemCtx &ItemCtx, SShipPerformanceCtx &Ctx) const

//  AccumulatePerformance
//
//  Adds performance improvements (if any) to the ship performance context. 
//  Returns TRUE if any improvements were added.

    {
    bool bModified = false;

	CInstalledArmor *pArmor = ItemCtx.GetArmor();
	const CItemEnhancement &Mods = (pArmor ? pArmor->GetMods() : CItemEnhancement::Null());

    //  Adjust max speed.

    if (m_rMaxSpeedBonus != 0.0)
        {
        Ctx.rMaxSpeedBonus += Ctx.rSingleArmorFraction * m_rMaxSpeedBonus;
        bModified = true;
        }

	//	Shield interference

	if (m_fShieldInterference || Mods.IsShieldInterfering())
		{
		Ctx.bShieldInterference = true;
		bModified = true;
		}

    //  Done

    return bModified;
    }

void CArmorClass::AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed)

//	AddTypesUsed
//
//	Adds types used by this class

	{
	}

ALERROR CArmorClass::BindScaledParams (SDesignLoadCtx &Ctx)

//  BindScaledParams
//
//  Bind scaled parameters

    {
    ALERROR error;
    int i;

    if (m_pScalable == NULL)
        return NOERROR;

    for (i = 0; i < m_iScaledLevels - 1; i++)
        {
        SScalableStats &Stats = m_pScalable[i];

        if (error = Stats.DamageAdj.Bind(Ctx, g_pUniverse->GetArmorDamageAdj(Stats.iLevel)))
            return error;

	    if (error = Stats.RepairCost.Bind(Ctx))
		    return error;

	    if (error = Stats.InstallCost.Bind(Ctx))
		    return error;
        }

    return NOERROR;
    }

void CArmorClass::CalcAdjustedDamage (CItemCtx &ItemCtx, SDamageCtx &Ctx)

//	CalcAdjustedDamage
//
//	Modifies Ctx.iDamage to account for damage type adjustments, etc.

	{
	CInstalledArmor *pArmor = ItemCtx.GetArmor();

	Ctx.iUnadjustedDamage = Ctx.iDamage;

	//	Adjust for special armor damage:
	//
	//	<0	=	2.5x damage
	//	0	=	2x damage
	//	1	=	1.5x damage
	//	2	=	1.25x damage
	//	>2	=	1x damage

	int iDamageLevel = Ctx.Damage.GetArmorDamageLevel();
	if (iDamageLevel > 0)
		Ctx.iDamage = mathAdjust(Ctx.iDamage, CalcArmorDamageAdj(ItemCtx, Ctx.Damage));

	//	Adjust for damage type

	int iDamageAdj = GetDamageAdj(ItemCtx, (pArmor ? pArmor->GetMods() : CItemEnhancement()), Ctx.Damage);
	Ctx.iDamage = mathAdjust(Ctx.iDamage, iDamageAdj);
	}

int CArmorClass::CalcArmorDamageAdj (CItemCtx &ItemCtx, const DamageDesc &Damage) const

//	CalcArmorDamageAdj
//
//	Adjust for special armor damage:
//
//	<0	=	2.5x damage
//	0	=	2x damage
//	1	=	1.5x damage
//	2	=	1.25x damage
//	>2	=	1x damage

	{
	int iDamageLevel = Damage.GetArmorDamageLevel();
	if (iDamageLevel <= 0)
		return 100;

	int iDiff = m_pItemType->GetLevel(ItemCtx) - iDamageLevel;

	switch (iDiff)
		{
		case 0:
			return 200;

		case 1:
			return 150;

		case 2:
			return 125;

		default:
			if (iDiff < 0)
				return 250;
			else
				return 100;
		}
	}

int CArmorClass::CalcAverageRelativeDamageAdj (CItemCtx &ItemCtx)

//	CalcAverageRelativeDamageAdj
//
//	Compares this armor against standard damage adj for the level and returns
//	the average damage adj relative to the standard.

	{
	int i;
	Metric rTotalAdj = 0.0;
	int iCount = 0;
	int iArmorLevel = m_pItemType->GetLevel(ItemCtx);

	for (i = damageLaser; i < damageCount; i++)
		{
		//	If this damage type is within two levels of the armor level,
		//	then we include it in our calculations.

		int iDamageLevel = ::GetDamageTypeLevel((DamageTypes)i);
		if (iArmorLevel < iDamageLevel + 5
				&& iArmorLevel > iDamageLevel - 3)
			{
			int iStdAdj = GetStdDamageAdj(iArmorLevel, (DamageTypes)i);
			int iDamageAdj = GetDamageAdj(ItemCtx, (DamageTypes)i);

			rTotalAdj += (iStdAdj > 0.0 ? (Metric)iDamageAdj * 100.0 / iStdAdj : 1000.0);
			iCount++;
			}
		}

	//	Return average

	return (iCount > 0 ? (int)((rTotalAdj / iCount) + 0.5) : 100);
	}

int CArmorClass::CalcBalance (void)

//	CalcBalance
//
//	Determines whether the given item is balanced for its level. Negative numbers
//	mean the item is underpowered. Positive numbers mean the item is overpowered.

	{
	int i;
	int iBalance = 0;
	int iLevel = m_pItemType->GetLevel();

	//	Regeneration

	if (!m_Stats.Regen.IsEmpty())
		{
		if (m_fPhotoRepair)
			iBalance += m_Stats.Regen.GetHPPerEra();
		else
			iBalance += 5 * m_Stats.Regen.GetHPPerEra();
		}

	if (!m_Stats.Distribute.IsEmpty())
		{
		iBalance += m_Stats.Distribute.GetHPPerEra();
		}

	//	Stealth

	if (m_iStealth >= 12)
		iBalance += 4;
	else if (m_iStealth >= 10)
		iBalance += 3;
	else if (m_iStealth >= 8)
		iBalance += 2;
	else if (m_iStealth >= 6)
		iBalance += 1;

	//	Immunities

	if (m_fDisintegrationImmune)
		{
		if (iLevel <= 10)
			iBalance += 3;
		}

	if (m_Stats.iBlindingDamageAdj <= 20 && iLevel < BLIND_IMMUNE_LEVEL)
		iBalance += 1;
	else if (m_Stats.iBlindingDamageAdj > 0 && iLevel >= BLIND_IMMUNE_LEVEL)
		iBalance -= 1;

	if (m_Stats.fRadiationImmune && iLevel < RADIATION_IMMUNE_LEVEL)
		iBalance += 2;
	else if (!m_Stats.fRadiationImmune && iLevel >= RADIATION_IMMUNE_LEVEL)
		iBalance -= 2;

	if (m_Stats.iEMPDamageAdj <= 20 && iLevel < EMP_IMMUNE_LEVEL)
		iBalance += 2;
	else if (m_Stats.iEMPDamageAdj > 0 && iLevel >= EMP_IMMUNE_LEVEL)
		iBalance -= 2;

	if (m_Stats.iDeviceDamageAdj <= 20 && iLevel < DEVICE_DAMAGE_IMMUNE_LEVEL)
		iBalance += 2;
	else if (m_Stats.iDeviceDamageAdj > 0 && iLevel >= DEVICE_DAMAGE_IMMUNE_LEVEL)
		iBalance -= 2;

	if (m_Stats.iBlindingDamageAdj > 20 || m_Stats.iEMPDamageAdj > 20 || m_Stats.iDeviceDamageAdj > 20)
		{
		if (m_Stats.iBlindingDamageAdj <= 33 || m_Stats.iEMPDamageAdj <= 33 || m_Stats.iDeviceDamageAdj <= 33)
			iBalance += 2;
		else if (m_Stats.iBlindingDamageAdj <= 50 || m_Stats.iEMPDamageAdj <= 50 || m_Stats.iDeviceDamageAdj <= 50)
			iBalance += 1;
		}

	if (m_fPhotoRecharge)
		iBalance += 2;

	//	Matched sets

	if (m_iArmorCompleteBonus)
		{
		int iPercent = m_iArmorCompleteBonus * 100 / m_Stats.iHitPoints;
		iBalance += (iPercent + 5) / 10;
		}

	//	Damage Adjustment

	int iBalanceAdj = 0;
	for (i = 0; i < damageCount; i++)
		{
		int iStdAdj;
		int iDamageAdj;
		m_Stats.DamageAdj.GetAdjAndDefault((DamageTypes)i, &iDamageAdj, &iStdAdj);

		if (iStdAdj != iDamageAdj)
			{
			if (iDamageAdj > 0)
				{
				int iBonus = (int)((100.0 * (iStdAdj - iDamageAdj) / iDamageAdj) + 0.5);

				if (iBonus > 0)
					iBalanceAdj += iBonus / 4;
				else
					iBalanceAdj -= ((int)((100.0 * iDamageAdj / iStdAdj) + 0.5) - 100) / 4;
				}
			else if (iStdAdj > 0)
				{
				iBalanceAdj += iStdAdj;
				}
			}
		}

	iBalance += (Max(Min(iBalanceAdj, 100), -100)) / 5;

	//	Reflection

	for (i = 0; i < damageCount; i++)
		{
		if (m_Reflective.InSet((DamageTypes)i))
			iBalance += 8;
		}

	//	Hit Points

	if (m_Stats.iHitPoints > 0)
		{
		int iDiff = (m_Stats.iHitPoints - STD_STATS[iLevel - 1].iHP);
		if (iDiff > 0)
			iBalance += iDiff * 20 / STD_STATS[iLevel - 1].iHP;
		else if (m_Stats.iHitPoints > 0)
			iBalance -= (STD_STATS[iLevel - 1].iHP * 20 / m_Stats.iHitPoints) - 20;
		else
			iBalance -= 40;
		}

	//	Mass

	int iMass = CItem(m_pItemType, 1).GetMassKg();
	if (iMass > 0)
		{
		int iDiff = (iMass - STD_STATS[iLevel - 1].iMass);

		//	Armor twice as massive can have double the hit points

		if (iDiff > 0)
			iBalance -= iDiff * 20 / STD_STATS[iLevel - 1].iMass;

		//	Armor half as massive can have 3/4 hit points

		else if (iMass > 0)
			iBalance += (STD_STATS[iLevel - 1].iMass * 10 / iMass) - 10;
		else
			iBalance += 100;
		}

	//	Repair tech

	int iRepair = iLevel - m_iRepairTech;
	if (iRepair < 0)
		iBalance += 2 * iRepair;
	else if (iRepair > 0)
		iBalance += iRepair;

	//	Repair cost

	int iStdRepairCost = STD_STATS[m_iRepairTech - 1].iRepairCost;
	int iDiff = iStdRepairCost - (int)CEconomyType::Default()->Exchange(m_Stats.RepairCost);
	if (iDiff < 0)
		iBalance += Max(-8, 2 * iDiff / iStdRepairCost);
	else if (iDiff > 0)
		iBalance += 5 * iDiff / iStdRepairCost;

	//	Power consumption

	if (m_iPowerUse)
		{
		int iPercent = m_iPowerUse * 100 / CShieldClass::GetStdPower(iLevel);
		iBalance -= iPercent;
		}

	//	Meteorsteel

	if (m_fShieldInterference)
		iBalance -= 12;

	//	Decay

	if (!m_Stats.Decay.IsEmpty())
		{
		iBalance -= 4 * m_Stats.Decay.GetHPPerEra();
		}

	return 5 * iBalance;
	}

void CArmorClass::CalcDamageEffects (CItemCtx &ItemCtx, SDamageCtx &Ctx)

//	CalcDamageEffects
//
//	Initialize the damage effects based on the damage and on this armor type.

	{
    const SScalableStats &Stats = GetScaledStats(ItemCtx);
	CSpaceObject *pSource = ItemCtx.GetSource();
	CInstalledArmor *pArmor = ItemCtx.GetArmor();

	//	Compute all the effects (if we don't have installed armor, then the 
	//	caller is responsible for setting this).

	if (pArmor)
		Ctx.iHPLeft = pArmor->GetHitPoints();

	//	Reflect

	Ctx.bReflect = (IsReflective(ItemCtx, Ctx.Damage) && Ctx.iDamage > 0);

	//	Disintegration

	int iDisintegration = Ctx.Damage.GetDisintegrationDamage();
	Ctx.bDisintegrate = (iDisintegration > 0 && !IsDisintegrationImmune(ItemCtx));

	//	Shatter

	int iShatter = Ctx.Damage.GetShatterDamage();
	if (iShatter)
		{
		//	Compute the threshold mass. Below this size, we shatter the object

		int iMassLimit = 10 * mathPower(5, iShatter);
		Ctx.bShatter = (pSource && pSource->GetMass() < iMassLimit);
		}
	else
		Ctx.bShatter = false;

	//	Blinding

	int iBlinding = Ctx.Damage.GetBlindingDamage();
	if (iBlinding && !IsBlindingDamageImmune(ItemCtx))
		{
		//	The chance of being blinded is dependent
		//	on the rating.

		int iChance = 4 * iBlinding * iBlinding * Stats.iBlindingDamageAdj / 100;
		Ctx.bBlind = (mathRandom(1, 100) <= iChance);
		Ctx.iBlindTime = Ctx.iDamage * g_TicksPerSecond / 2;
		}
	else
		Ctx.bBlind = false;

	//	EMP

	int iEMP = Ctx.Damage.GetEMPDamage();
	if (iEMP && !IsEMPDamageImmune(ItemCtx))
		{
		//	The chance of being paralyzed is dependent
		//	on the EMP rating.

		int iChance = 4 * iEMP * iEMP * Stats.iEMPDamageAdj / 100;
		Ctx.bParalyze = (mathRandom(1, 100) <= iChance);
		Ctx.iParalyzeTime = Ctx.iDamage * g_TicksPerSecond / 2;
		}
	else
		Ctx.bParalyze = false;

	//	Device disrupt

	int iDeviceDisrupt = Ctx.Damage.GetDeviceDisruptDamage();
	if (iDeviceDisrupt && !IsDeviceDamageImmune(ItemCtx))
		{
		//	The chance of damaging a device depends on the rating.

		int iChance = 4 * iDeviceDisrupt * iDeviceDisrupt * Stats.iDeviceDamageAdj / 100;
		Ctx.bDeviceDisrupt = (mathRandom(1, 100) <= iChance);
		Ctx.iDisruptTime = 2 * Ctx.iDamage * g_TicksPerSecond;
		}
	else
		Ctx.bDeviceDisrupt = false;

	//	Device damage

	int iDeviceDamage = Ctx.Damage.GetDeviceDamage();
	if (iDeviceDamage && !IsDeviceDamageImmune(ItemCtx))
		{
		//	The chance of damaging a device depends on the rating.

		int iChance = 4 * iDeviceDamage * iDeviceDamage * Stats.iDeviceDamageAdj / 100;
		Ctx.bDeviceDamage = (mathRandom(1, 100) <= iChance);
		}
	else
		Ctx.bDeviceDamage = false;

	//	Radiation

	int iRadioactive = Ctx.Damage.GetRadiationDamage();
	Ctx.bRadioactive = (iRadioactive > 0 && !IsRadiationImmune(ItemCtx));

	//	Some effects decrease damage

	if (iBlinding || iEMP)
		Ctx.iDamage = 0;
	else if (iDeviceDamage)
		Ctx.iDamage = Ctx.iDamage / 2;
	}

int CArmorClass::CalcPowerUsed (CInstalledArmor *pArmor)

//	CalcPowerUsed
//
//	Computes the amount of power used by this armor segment (this
//	only applies to powered armor)

	{
	//	If we did work (regenerated), then we use full power. Otherwise, we use
	//	idle power.
	//
	//	NOTE: By default, idle power is the same as full power, but some armors
	//	have different values.

	if (pArmor->ConsumedPower())
		return m_iPowerUse;
	else
		return m_iIdlePowerUse;
	}

ALERROR CArmorClass::CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, CArmorClass **retpArmor)

//	CreateFromXML
//
//	Create armor class desc

	{
	ALERROR error;
	CArmorClass *pArmor;
	CString sValue;

	ASSERT(pType);

	pArmor = new CArmorClass;
	if (pArmor == NULL)
		return ERR_MEMORY;

	int iLevel = (pType ? pType->GetLevel() : 1);

	pArmor->m_pItemType = pType;
	pArmor->m_Stats.iLevel = iLevel;
	pArmor->m_Stats.iHitPoints = pDesc->GetAttributeIntegerBounded(CONSTLIT(g_HitPointsAttrib), 0);
	pArmor->m_iArmorCompleteBonus = pDesc->GetAttributeIntegerBounded(COMPLETE_BONUS_ATTRIB, 0);

	//	Regen & Decay

	if (error = pArmor->m_Stats.Regen.InitFromXML(Ctx, pDesc, REGEN_ATTRIB, REPAIR_RATE_ATTRIB, NULL_STR, TICKS_PER_UPDATE))
		return error;

	if (error = pArmor->m_Stats.Decay.InitFromXML(Ctx, pDesc, DECAY_ATTRIB, DECAY_RATE_ATTRIB, NULL_STR, TICKS_PER_UPDATE))
		return error;

	if (error = pArmor->m_Stats.Distribute.InitFromXML(Ctx, pDesc, DISTRIBUTE_ATTRIB, NULL_STR, NULL_STR, TICKS_PER_UPDATE))
		return error;

	//	We allow for explicit install cost (in which case we expect a currency).
	//	If no cost specified, we take the default (which is in credits).
	//	Either result is adjusted.

	if (error = pArmor->m_Stats.InstallCost.InitFromXML(Ctx, pDesc->GetAttribute(INSTALL_COST_ATTRIB)))
		return error;

	if (pArmor->m_Stats.InstallCost.IsEmpty())
		pArmor->m_Stats.InstallCost.Init(STD_STATS[iLevel - 1].iInstallCost);

	int iInstallCostAdj = pDesc->GetAttributeIntegerBounded(INSTALL_COST_ADJ_ATTRIB, 0, -1, 100);
	if (iInstallCostAdj != 100)
		pArmor->m_Stats.InstallCost.Adjust(iInstallCostAdj);

	//	Repair tech defaults to level

	pArmor->m_iRepairTech = pDesc->GetAttributeIntegerBounded(REPAIR_TECH_ATTRIB, 1, MAX_TECH_LEVEL, iLevel);

	//	Repair cost is based on repair tech

	if (error = pArmor->m_Stats.RepairCost.InitFromXML(Ctx, pDesc->GetAttribute(REPAIR_COST_ATTRIB)))
		return error;

	if (pArmor->m_Stats.RepairCost.IsEmpty())
		pArmor->m_Stats.RepairCost.Init(STD_STATS[pArmor->m_iRepairTech - 1].iRepairCost);

	int iRepairCostAdj = pDesc->GetAttributeIntegerBounded(REPAIR_COST_ADJ_ATTRIB, 0, -1, 100);
	if (iRepairCostAdj != 100)
		pArmor->m_Stats.RepairCost.Adjust(iRepairCostAdj);

	//	Load the new damage adjustment structure

	pArmor->m_iDamageAdjLevel = pDesc->GetAttributeIntegerBounded(DAMAGE_ADJ_LEVEL_ATTRIB, 1, MAX_ITEM_LEVEL, iLevel);
	if (error = pArmor->m_Stats.DamageAdj.InitFromXML(Ctx, pDesc))
		return error;

	//	Blind-immune

	bool bValue;
	if (pDesc->FindAttributeBool(BLINDING_IMMUNE_ATTRIB, &bValue))
		pArmor->m_Stats.iBlindingDamageAdj = (bValue ? 0 : 100);
	else
		pArmor->m_Stats.iBlindingDamageAdj = pDesc->GetAttributeIntegerBounded(BLINDING_DAMAGE_ADJ_ATTRIB, 
				0, 
				-1, 
				iLevel >= BLIND_IMMUNE_LEVEL ? 0 : 100);

	//	Radiation-immune

	if (pDesc->FindAttributeBool(RADIATION_IMMUNE_ATTRIB, &bValue))
		pArmor->m_Stats.fRadiationImmune = bValue;
	else
		pArmor->m_Stats.fRadiationImmune = (iLevel >= RADIATION_IMMUNE_LEVEL ? true : false);

	//	EMP-immune

	if (pDesc->FindAttributeBool(EMP_IMMUNE_ATTRIB, &bValue))
		pArmor->m_Stats.iEMPDamageAdj = (bValue ? 0 : 100);
	else
		pArmor->m_Stats.iEMPDamageAdj = pDesc->GetAttributeIntegerBounded(EMP_DAMAGE_ADJ_ATTRIB, 
				0, 
				-1, 
				iLevel >= EMP_IMMUNE_LEVEL ? 0 : 100);

	//	Device damage immune

	if (pDesc->FindAttributeBool(DEVICE_DAMAGE_IMMUNE_ATTRIB, &bValue))
		pArmor->m_Stats.iDeviceDamageAdj = (bValue ? 0 : 100);
	else
		pArmor->m_Stats.iDeviceDamageAdj = pDesc->GetAttributeIntegerBounded(DEVICE_DAMAGE_ADJ_ATTRIB, 
				0,
				-1, 
				iLevel >= DEVICE_DAMAGE_IMMUNE_LEVEL ? 0 : 100);

	//	Shatter immune

	pArmor->m_fShatterImmune = pDesc->GetAttributeBool(SHATTER_IMMUNE_ATTRIB);

	//	Disintegration immune

	pArmor->m_fDisintegrationImmune = pDesc->GetAttributeBool(DISINTEGRATION_IMMUNE_ATTRIB);

	pArmor->m_fPhotoRepair = pDesc->GetAttributeBool(PHOTO_REPAIR_ATTRIB);
	pArmor->m_fPhotoRecharge = pDesc->GetAttributeBool(PHOTO_RECHARGE_ATTRIB);
	pArmor->m_fShieldInterference = pDesc->GetAttributeBool(SHIELD_INTERFERENCE_ATTRIB);
	pArmor->m_fChargeRepair = pDesc->GetAttributeBool(CHARGE_REGEN_ATTRIB);
	pArmor->m_fChargeDecay = pDesc->GetAttributeBool(CHARGE_DECAY_ATTRIB);
    pArmor->m_fHealerRepair = pDesc->GetAttributeBool(HEALER_REGEN_ATTRIB);

	pArmor->m_iStealth = pDesc->GetAttributeInteger(STEALTH_ATTRIB);
	if (pArmor->m_iStealth == 0)
		pArmor->m_iStealth = CSpaceObject::stealthNormal;

	pArmor->m_iMaxHPBonus = pDesc->GetAttributeIntegerBounded(MAX_HP_BONUS_ATTRIB, 0, -1, 150);

	//	Power use

	pArmor->m_iPowerUse = pDesc->GetAttributeIntegerBounded(POWER_USE_ATTRIB, 0);
	pArmor->m_iIdlePowerUse = pDesc->GetAttributeIntegerBounded(IDLE_POWER_USE_ATTRIB, 0, -1, pArmor->m_iPowerUse);

	//	Load reflection

	if (error = pArmor->m_Reflective.InitFromXML(pDesc->GetAttribute(REFLECT_ATTRIB)))
		return error;

	//	Device bonus

	pArmor->m_sEnhancementType = pDesc->GetAttribute(ENHANCEMENT_TYPE_ATTRIB);
	pArmor->m_iDeviceBonus = pDesc->GetAttributeInteger(DEVICE_HP_BONUS_ATTRIB);
	CString sCriteria;
	if (!pDesc->FindAttribute(DEVICE_CRITERIA_ATTRIB, &sCriteria))
		sCriteria = CONSTLIT("s");

	CItem::ParseCriteria(sCriteria, &pArmor->m_DeviceCriteria);

	//	Ship speed bonus

	pArmor->m_rMaxSpeedBonus = pDesc->GetAttributeDoubleBounded(MAX_SPEED_BONUS_ATTRIB, -100.0, 1000.0, 0.0);

    //  If we're scalable, generate stats for all levels.

    if (pType->IsScalable())
        pArmor->GenerateScaledStats();

	//	Done

	*retpArmor = pArmor;

	return NOERROR;
	}

bool CArmorClass::FindDataField (const CString &sField, CString *retsValue)

//	FindDataField
//
//	Returns meta-data

	{
	int i;

	if (strEquals(sField, FIELD_HP))
		*retsValue = strFromInt(m_Stats.iHitPoints);
	else if (strEquals(sField, FIELD_BALANCE))
		*retsValue = strFromInt(CalcBalance());
	else if (strEquals(sField, FIELD_EFFECTIVE_HP))
		{
		int iHP;
		int iHPbyDamageType[damageCount];
		GetReferenceDamageAdj(NULL, NULL, &iHP, iHPbyDamageType);
		*retsValue = strFromInt(::CalcEffectiveHP(m_pItemType->GetLevel(), iHP, iHPbyDamageType));
		}
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

			retsValue->Append(strFromInt(m_Stats.DamageAdj.GetAdj((DamageTypes)i)));
			}
		}
	else if (strEquals(sField, FIELD_HP_BONUS))
		{
		CString sResult;

		for (i = 0; i < damageCount; i++)
			{
			if (i > 0)
				sResult.Append(CONSTLIT(", "));

			int iBonus = m_Stats.DamageAdj.GetHPBonus((DamageTypes)i);
			if (iBonus == -100)
				sResult.Append(CONSTLIT("***"));
			else
				sResult.Append(strPatternSubst(CONSTLIT("%3d"), iBonus));
			}

		*retsValue = sResult;
		}
	else if (strEquals(sField, FIELD_REPAIR_COST))
		*retsValue = strFromInt(GetRepairCost(CItemCtx()));
	else if (strEquals(sField, FIELD_REGEN))
		*retsValue = strFromInt((int)m_Stats.Regen.GetHPPer180());
	else if (strEquals(sField, FIELD_INSTALL_COST))
		*retsValue = strFromInt(GetInstallCost(CItemCtx()));
	else if (strEquals(sField, FIELD_SHIELD_INTERFERENCE))
		{
		if (m_fShieldInterference)
			*retsValue = CONSTLIT("True");
		else
			*retsValue = NULL_STR;
		}
	else
		return false;

	return true;
	}

int CArmorClass::FireGetMaxHP (CItemCtx &ItemCtx, int iMaxHP) const

//	FireGetMaxHP
//
//	Fire GetMaxHP event

	{
	SEventHandlerDesc Event;
	if (FindEventHandlerArmorClass(evtGetMaxHP, &Event))
		{
		//	Setup arguments

		CCodeChainCtx Ctx;
		Ctx.SaveAndDefineSourceVar(ItemCtx.GetSource());
		Ctx.SaveAndDefineItemVar(ItemCtx);

		Ctx.DefineInteger(CONSTLIT("aMaxHP"), iMaxHP);

		ICCItem *pResult = Ctx.Run(Event);

		if (pResult->IsError())
			ItemCtx.GetSource()->ReportEventError(GET_MAX_HP_EVENT, pResult);
		else if (!pResult->IsNil())
			iMaxHP = Max(0, pResult->GetIntegerValue());

		Ctx.Discard(pResult);
		}

	return iMaxHP;
	}

void CArmorClass::FireOnArmorDamage (CItemCtx &ItemCtx, SDamageCtx &Ctx)

//	FireOnArmorDamage
//
//	Fires OnArmorDamage event

	{
	SEventHandlerDesc Event;
	if (FindEventHandlerArmorClass(evtOnArmorDamage, &Event))
		{
		//	Setup arguments

		CCodeChainCtx CCCtx;
		CCCtx.SaveAndDefineSourceVar(ItemCtx.GetSource());
		CCCtx.SaveAndDefineItemVar(ItemCtx);

		CCCtx.DefineInteger(CONSTLIT("aArmorHP"), Ctx.iHPLeft);
		CCCtx.DefineInteger(CONSTLIT("aArmorSeg"), Ctx.iSectHit);
		CCCtx.DefineSpaceObject(CONSTLIT("aAttacker"), Ctx.Attacker.GetObj());
		CCCtx.DefineSpaceObject(CONSTLIT("aCause"), Ctx.pCause);
		CCCtx.DefineDamageEffects(CONSTLIT("aDamageEffects"), Ctx);
		CCCtx.DefineInteger(CONSTLIT("aFullDamageHP"), Ctx.iUnadjustedDamage);
		CCCtx.DefineInteger(CONSTLIT("aDamageHP"), Ctx.iDamage);
		CCCtx.DefineString(CONSTLIT("aDamageType"), GetDamageShortName(Ctx.Damage.GetDamageType()));
		CCCtx.DefineInteger(CONSTLIT("aHitDir"), Ctx.iDirection);
		CCCtx.DefineVector(CONSTLIT("aHitPos"), Ctx.vHitPos);
		CCCtx.DefineSpaceObject(CONSTLIT("aOrderGiver"), Ctx.GetOrderGiver());
		CCCtx.DefineItemType(CONSTLIT("aWeaponType"), Ctx.pDesc->GetWeaponType());

		ICCItem *pResult = CCCtx.Run(Event);

		//	If we return Nil, then nothing

		if (pResult->IsNil())
			NULL;

		//	If we return an integer, then this is the damage that armor should take

		else if (pResult->IsInteger())
			Ctx.iDamage = pResult->GetIntegerValue();

		//	If we return a list, then we it to be a DamageEffects list (modifications to
		//	aDamageEffects)

		else if (pResult->IsList())
			LoadDamageEffectsFromItem(pResult, Ctx);

		CCCtx.Discard(pResult);
		}
	}

void CArmorClass::GenerateScaledStats (void)

//  GenerateScaledParams
//
//  Generate parameters for all levels.

    {
    int i;

    ASSERT(m_pScalable == NULL);

    int iBaseLevel = m_pItemType->GetLevel();
    m_iScaledLevels = 1 + m_pItemType->GetMaxLevel() - iBaseLevel;
    if (m_iScaledLevels < 2)
        return;

    m_pScalable = new SScalableStats[m_iScaledLevels - 1];

    const SStdStats &BaseStd = GetStdStats(iBaseLevel);

    for (i = 1; i < m_iScaledLevels; i++)
        {
        SScalableStats &Stats = m_pScalable[i - 1];
        Stats.iLevel = iBaseLevel + i;

        const SStdStats &LevelStd = GetStdStats(Stats.iLevel);

        //  Scale hit points using standard scale

        Metric rHPAdj = (Metric)LevelStd.iHP / (Metric)BaseStd.iHP;
        Stats.iHitPoints = (int)(rHPAdj * m_Stats.iHitPoints);

        //  Damage adjust gets initialized at bind time (because the scale is
        //  part of the adventure).

        Stats.DamageAdj = m_Stats.DamageAdj;

        //  Immunities based on level

        Stats.iBlindingDamageAdj = Min(m_Stats.iBlindingDamageAdj, (Stats.iLevel >= BLIND_IMMUNE_LEVEL ? 0 : 100));
		Stats.fRadiationImmune = m_Stats.fRadiationImmune || (Stats.iLevel >= RADIATION_IMMUNE_LEVEL ? true : false);
		Stats.iEMPDamageAdj = Min(m_Stats.iEMPDamageAdj, (Stats.iLevel >= EMP_IMMUNE_LEVEL ? 0 : 100));
		Stats.iDeviceDamageAdj = Min(m_Stats.iDeviceDamageAdj, (Stats.iLevel >= DEVICE_DAMAGE_IMMUNE_LEVEL ? 0 : 100));

        //  Regen and decay

        if (!m_Stats.Regen.IsEmpty())
            Stats.Regen.InitFromRegen(rHPAdj * m_Stats.Regen.GetHPPer180(TICKS_PER_UPDATE), TICKS_PER_UPDATE);

        if (!m_Stats.Decay.IsEmpty())
            Stats.Decay.InitFromRegen(rHPAdj * m_Stats.Decay.GetHPPer180(TICKS_PER_UPDATE), TICKS_PER_UPDATE);

        if (!m_Stats.Distribute.IsEmpty())
            Stats.Distribute.InitFromRegen(rHPAdj * m_Stats.Distribute.GetHPPer180(TICKS_PER_UPDATE), TICKS_PER_UPDATE);

        //  Prices

        Stats.RepairCost = CCurrencyAndValue(CurrencyValue((Metric)m_Stats.RepairCost.GetValue() * (Metric)LevelStd.iRepairCost / (Metric)BaseStd.iRepairCost), m_Stats.RepairCost.GetCurrencyType());
        Stats.InstallCost = CCurrencyAndValue(CurrencyValue((Metric)m_Stats.InstallCost.GetValue() * (Metric)LevelStd.iInstallCost / (Metric)BaseStd.iInstallCost), m_Stats.InstallCost.GetCurrencyType());
        }
    }

int CArmorClass::GetDamageAdj (CItemCtx &ItemCtx, CItemEnhancement Mods, const DamageDesc &Damage) const

//	GetDamageAdj
//
//	Returns the damage adjustment for the given damage type

	{
	int iDamageAdj = GetDamageAdj(ItemCtx, Damage.GetDamageType());

	if (Mods.IsNotEmpty())
		return iDamageAdj * Mods.GetDamageAdj(Damage) / 100;
	else
		return iDamageAdj;
	}

int CArmorClass::GetDamageAdjForWeaponLevel (int iLevel)

//	GetDamageAdjForWeaponLevel
//
//	Returns the least effective damage adjustment of all damage types appropriate
//	to the given level.

	{
	int i;

	int iBestAdj = 0;
	for (i = 0; i < damageCount; i++)
		{
		if (CWeaponClass::IsStdDamageType((DamageTypes)i, iLevel))
			{
			int iDamageAdj = GetDamageAdj(CItemCtx(), (DamageTypes)i);
			if (iDamageAdj > iBestAdj)
				iBestAdj = iDamageAdj;
			}
		}

	return iBestAdj;
	}

int CArmorClass::GetDamageEffectiveness (CSpaceObject *pAttacker, CInstalledDevice *pWeapon)

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

	int iBonus = m_Stats.DamageAdj.GetHPBonus(pDamage->GetDamageType());
	if (iBonus <= -100)
		return -1;

	//	Compute score based on bonus

	int iScore;
	if (iBonus <= 0)
		iScore = 100 - iBonus;
	else
		iScore = 100 - Min(100, (iBonus / 2));

	//	See if we do extra damage

	if (pDamage->GetArmorDamageLevel())
		iScore += (CalcArmorDamageAdj(CItemCtx(), *pDamage) / 2);

	//	Done

	return iScore;
	}

ICCItem *CArmorClass::FindItemProperty (CItemCtx &Ctx, const CString &sName)

//	FindItemProperty
//
//	Returns armor property. NOTE: We DO NOT return generic item properties.

	{
	CCodeChain &CC = g_pUniverse->GetCC();
    const SScalableStats &Stats = GetScaledStats(Ctx);

	//	Enhancements

	const CItemEnhancementStack *pEnhancements = Ctx.GetEnhancementStack();

	//	Get the property

	if (strEquals(sName, PROPERTY_BLINDING_IMMUNE))
		return CC.CreateBool(IsBlindingDamageImmune(Ctx));

	else if (strEquals(sName, PROPERTY_COMPLETE_HP))
		return CC.CreateInteger(GetMaxHP(Ctx, true));

	else if (strEquals(sName, PROPERTY_COMPLETE_SET))
		{
		CInstalledArmor *pArmor = Ctx.GetArmor();
		if (pArmor == NULL)
			return CC.CreateNil();

		return CC.CreateBool(pArmor->IsComplete());
		}

	else if (strEquals(sName, PROPERTY_DEVICE_DAMAGE_IMMUNE))
		return CC.CreateBool(IsDeviceDamageImmune(Ctx));

	else if (strEquals(sName, PROPERTY_DEVICE_DISRUPT_IMMUNE))
		return CC.CreateBool(IsDeviceDamageImmune(Ctx));

	else if (strEquals(sName, PROPERTY_DISINTEGRATION_IMMUNE))
		return CC.CreateBool(IsDisintegrationImmune(Ctx));

	else if (strEquals(sName, PROPERTY_EMP_IMMUNE))
		return CC.CreateBool(IsEMPDamageImmune(Ctx));

	else if (strEquals(sName, PROPERTY_HP))
		{
		CInstalledArmor *pArmor = Ctx.GetArmor();
		if (pArmor)
			return CC.CreateInteger(pArmor->GetHitPoints());
		else
			return CC.CreateInteger(GetMaxHP(Ctx));
		}

	else if (strEquals(sName, PROPERTY_HP_BONUS))
		return Stats.DamageAdj.GetHPBonusProperty(pEnhancements);

	else if (strEquals(sName, PROPERTY_MAX_HP))
		return CC.CreateInteger(GetMaxHP(Ctx));

	else if (strEquals(sName, PROPERTY_PRIME_SEGMENT))
		{
		CInstalledArmor *pArmor = Ctx.GetArmor();
		if (pArmor == NULL)
			return CC.CreateNil();

		return CC.CreateBool(pArmor->IsPrime());
		}

	else if (strEquals(sName, PROPERTY_RADIATION_IMMUNE))
		return CC.CreateBool(IsRadiationImmune(Ctx));

	else if (strEquals(sName, PROPERTY_REPAIR_COST))
		return CC.CreateInteger(GetRepairCost(Ctx));

	else if (strEquals(sName, PROPERTY_REPAIR_LEVEL))
		return CC.CreateInteger(GetRepairTech());

	else if (strEquals(sName, PROPERTY_SHATTER_IMMUNE))
		return CC.CreateBool(IsShatterImmune(Ctx));

	else
		return NULL;
	}

int CArmorClass::GetMaxHP (CItemCtx &ItemCtx, bool bForceComplete)

//	GetMaxHP
//
//	Returns the max HP for this kind of armor

	{
    const SScalableStats &Stats = GetScaledStats(ItemCtx);

	//	Start with hit points defined by the class, scaled if necessary.

    int iHP = Stats.iHitPoints;

	//	Fire event to compute HP, if necessary

	iHP = FireGetMaxHP(ItemCtx, iHP);

	//	Add mods

	const CItemEnhancement &Mods = ItemCtx.GetMods();
	if (Mods.IsNotEmpty())
		iHP = iHP * Mods.GetHPAdj() / 100;

	//	Add complete bonus

	CInstalledArmor *pSect = ItemCtx.GetArmor();
	if (bForceComplete || (pSect && pSect->IsComplete()))
		iHP += m_iArmorCompleteBonus;

	//	Done

	return iHP;
	}

CString CArmorClass::GetReference (CItemCtx &Ctx, const CItem &Ammo)
	
//	GetReference
//
//	Returns a string that describes the basic attributes
//	of this armor.
//
//	Example:
//
//		30 hp; laser-resistant; impact-resistant

	{
	int iKg = m_pItemType->GetMassKg(Ctx);
	int iTons = iKg / 1000;
	int iKgExtra = iKg % 1000;

	if (iTons == 1 && iKgExtra == 0)
		return CONSTLIT("1 ton");
	else if (iKgExtra == 0)
		return strPatternSubst(CONSTLIT("%d tons"), iTons);
	else
		return strPatternSubst(CONSTLIT("%d.%d tons"), iTons, iKgExtra / 100);
	}

bool CArmorClass::GetReferenceDamageAdj (const CItem *pItem, CSpaceObject *pInstalled, int *retiHP, int *retArray)

//	GetReferenceDamageAdj
//
//	Returns armor HP after adjustment for damage type

	{
	int i;

	CItemCtx ItemCtx(pItem, pInstalled);
	int iHP = GetMaxHP(ItemCtx);

	if (retiHP)
		*retiHP = iHP;

	for (i = 0; i < damageCount; i++)
		{
		DamageDesc Damage((DamageTypes)i, DiceRange(0, 0, 0));
		int iAdj = GetDamageAdj(ItemCtx, ItemCtx.GetMods(), Damage);

		if (retArray)
			retArray[i] = CalcHPDamageAdj(iHP, iAdj);
		}

	return true;
	}

Metric CArmorClass::GetScaledCostAdj (CItemCtx &ItemCtx) const

//  GetCost
//
//  Returns the cost of the armor, taking scaling into account.

    {
    if (m_pScalable == NULL || ItemCtx.IsItemNull())
        return 1.0;

    int iLevel = Min(ItemCtx.GetItem().GetVariantHigh(), m_iScaledLevels);
    if (iLevel <= 0)
        return 1.0;

    return (Metric)GetStdStats(iLevel).iCost / (Metric)GetStdStats(m_pItemType->GetLevel()).iCost;
    }

const CArmorClass::SScalableStats &CArmorClass::GetScaledStats (CItemCtx &ItemCtx) const

//  GetScaledStats
//
//  Returns proper stats

    {
    if (m_pScalable == NULL || ItemCtx.IsItemNull())
        return m_Stats;

    int iLevel = Min(ItemCtx.GetItem().GetVariantHigh(), m_iScaledLevels);
    if (iLevel <= 0)
        return m_Stats;

    return m_pScalable[iLevel - 1];
    }

CString CArmorClass::GetShortName (void)

//	GetShortName
//
//	Returns the short name. This will basically omit the leading "segment of"
//	from the item name.

	{
	return m_pItemType->GetNounPhrase(nounShort);
	}

int CArmorClass::GetStdCost (int iLevel)

//	GetStdCost
//
//	Returns standard cost by level

	{
	ASSERT(iLevel >= 1 && iLevel <= MAX_ITEM_LEVEL);
	return STD_STATS[iLevel - 1].iCost;
	}

int CArmorClass::GetStdDamageAdj (int iLevel, DamageTypes iDamage)

//	GetStdDamageAdj
//
//	Returns the standard damage adjustment for the given damage at
//	the given level

	{
	ASSERT(iLevel >= 1 && iLevel <= MAX_ITEM_LEVEL);
	return g_pUniverse->GetArmorDamageAdj(iLevel)->GetAdj(iDamage);
	}

int CArmorClass::GetStdEffectiveHP (int iLevel)

//	GetStdEffectiveHP
//
//	Returns effective HP by level

	{
	int i;
	ASSERT(iLevel >= 1 && iLevel <= MAX_ITEM_LEVEL);

	int iHPbyDamageType[damageCount];
	for (i = 0; i < damageCount; i++)
		iHPbyDamageType[i] = CalcHPDamageAdj(STD_STATS[iLevel - 1].iHP, g_pUniverse->GetArmorDamageAdj(iLevel)->GetAdj((DamageTypes)i));

	return ::CalcEffectiveHP(iLevel, STD_STATS[iLevel - 1].iHP, iHPbyDamageType);
	}

int CArmorClass::GetStdHP (int iLevel)

//	GetStdHP
//
//	Returns standard hp by level

	{
	ASSERT(iLevel >= 1 && iLevel <= MAX_ITEM_LEVEL);
	return STD_STATS[iLevel - 1].iHP;
	}

int CArmorClass::GetStdMass (int iLevel)

//	GetStdMass
//
//	Returns standard mass by level (in kg)

	{
	ASSERT(iLevel >= 1 && iLevel <= MAX_ITEM_LEVEL);
	return STD_STATS[iLevel - 1].iMass;
	}

const CArmorClass::SStdStats &CArmorClass::GetStdStats (int iLevel)

//  GetStdStats
//
//  Returns standard stats

    {
	ASSERT(iLevel >= 1 && iLevel <= MAX_ITEM_LEVEL);
    return STD_STATS[iLevel - 1];
    }

bool CArmorClass::IsReflective (CItemCtx &ItemCtx, const DamageDesc &Damage)

//	IsReflective
//
//	Returns TRUE if the armor reflects this damage

	{
	const CItemEnhancement &Mods = ItemCtx.GetMods();

	int iReflectChance = 0;

	//	Base armor chance

	if (m_Reflective.InSet(Damage.GetDamageType()))
		iReflectChance = MAX_REFLECTION_CHANCE;

	//	Mods

	int iModReflect;
	if (Mods.IsNotEmpty() && Mods.IsReflective(Damage, &iModReflect))
		iReflectChance = Max(iReflectChance, iModReflect);

	//	Done

	if (iReflectChance)
		{
		CInstalledArmor *pSect = ItemCtx.GetArmor();

		int iMaxHP = GetMaxHP(ItemCtx);
		int iHP = (pSect ? pSect->GetHitPoints() : iMaxHP);

		//	Adjust based on how damaged the armor is

		iReflectChance = (iMaxHP > 0 ? iHP * iReflectChance / iMaxHP : iReflectChance);

		return (mathRandom(1, 100) <= iReflectChance);
		}
	else
		return false;
	}

ALERROR CArmorClass::OnBindDesign (SDesignLoadCtx &Ctx)

//	OnBindDesign
//
//	Called on Bind

	{
	ALERROR error;

	//	Compute armor damage adjustments

	if (error = m_Stats.DamageAdj.Bind(Ctx, g_pUniverse->GetArmorDamageAdj(m_iDamageAdjLevel)))
		return error;

	//	Prices

	if (error = m_Stats.InstallCost.Bind(Ctx))
		return error;

	if (error = m_Stats.RepairCost.Bind(Ctx))
		return error;

    //  Bind scaled parameters

    if (error = BindScaledParams(Ctx))
        return error;

	//	Cache some events

	CItemType *pType = GetItemType();
	pType->InitCachedEvents(evtCount, CACHED_EVENTS, m_CachedEvents);

	return NOERROR;
	}

void CArmorClass::Update (CInstalledArmor *pArmor, CSpaceObject *pObj, int iTick, bool *retbModified)

//	Update
//
//	Updates the armor. This should be called once every 10 ticks

	{
    DEBUG_TRY

    ASSERT(pArmor);
    ASSERT(pObj);

	int i;
	bool bModified = false;
	CItemCtx ItemCtx(pObj, pArmor);
    const SScalableStats &Stats = GetScaledStats(ItemCtx);

	//	Default to not consuming power. Below we set the flag is we do any kind 
	//	of regeneration work.

	pArmor->SetConsumePower(false);

	//	Compute total regeneration by adding mods to intrinsic

	if (pArmor->GetMods().IsRegenerating()	
			|| pArmor->GetMods().IsPhotoRegenerating()
			|| !Stats.Regen.IsEmpty())
		{

		int iHPNeeded = GetMaxHP(ItemCtx) - pArmor->GetHitPoints();

		//	If we require charges, then we're limited to the charges we have

        if (m_fChargeRepair)
            iHPNeeded = Min(iHPNeeded, pArmor->GetCharges(pObj));
        else if (m_fHealerRepair && pObj->GetArmorSystem())
            iHPNeeded = Min(iHPNeeded, pObj->GetArmorSystem()->GetHealerLeft());

		//	Regen

		if (iHPNeeded > 0)
			{
			//	Combine all regeneration

			const CRegenDesc *pRegen;
			CRegenDesc RegenWithMod;
			if (pArmor->GetMods().IsRegenerating() || pArmor->GetMods().IsPhotoRegenerating())
				{
				//	Standard regeneration is 1% of armor HP per 180 ticks

				RegenWithMod.InitFromRegen(0.01 * GetStdHP(ItemCtx.GetItem().GetLevel()), TICKS_PER_UPDATE);
				RegenWithMod.Add(Stats.Regen);
				pRegen = &RegenWithMod;
				}
			else
				pRegen = &Stats.Regen;

			//	Compute the HP that we regenerate this cycle

			int iHP = Min(iHPNeeded, pRegen->GetRegen(iTick, TICKS_PER_UPDATE));

			//	If this is photo-repair armor then adjust the cycle
			//	based on how far away we are from the sun.

			if (iHP > 0)
				if (m_fPhotoRepair || pArmor->GetMods().IsPhotoRegenerating())
					{
					int iIntensity = pObj->GetSystem()->CalculateLightIntensity(pObj->GetPos());
					if (mathRandom(1, 100) > iIntensity)
						iHP = 0;
					}

			//	Repair

			if (iHP > 0)
				{
				//	If we require charges to regen, then consume charges

                if (m_fChargeRepair)
                    pArmor->IncCharges(pObj, -iHP);
                else if (m_fHealerRepair && pObj->GetArmorSystem())
                    pObj->GetArmorSystem()->IncHealerLeft(-iHP);

				pArmor->IncHitPoints(iHP);
				pArmor->SetConsumePower(true);
				bModified = true;
				}
			}
		}

	//	See if we're decaying

	if (pArmor->GetHitPoints() > 0
			&& (pArmor->GetMods().IsDecaying() || !Stats.Decay.IsEmpty()))
		{
		//	If we require charges, then we're limited to the charges we have

		int iMaxDecay = pArmor->GetHitPoints();
		if (m_fChargeDecay)
			iMaxDecay = Min(iMaxDecay, pArmor->GetCharges(pObj));

		//	Decay

		if (iMaxDecay > 0)
			{
			//	Combine decay with mod

			const CRegenDesc *pDecay;
			CRegenDesc DecayWithMod;
			if (pArmor->GetMods().IsDecaying())
				{
				DecayWithMod.Init(4);
				DecayWithMod.Add(Stats.Decay);
				pDecay = &DecayWithMod;
				}
			else
				pDecay = &Stats.Decay;

			//	Compute the HP that we decay this cycle

			int iHP = Min(iMaxDecay, pDecay->GetRegen(iTick, TICKS_PER_UPDATE));

			//	Decrement

			if (iHP > 0)
				{
				//	Consume charges

				if (m_fChargeDecay)
					pArmor->IncCharges(pObj, -iHP);

				pArmor->IncHitPoints(-iHP);
				bModified = true;
				}
			}
		}

	//	If this is solar armor then recharge the object

	if (pArmor->GetMods().IsPhotoRecharge() || m_fPhotoRecharge)
		{
		int iIntensity = pObj->GetSystem()->CalculateLightIntensity(pObj->GetPos());

		//	Intensity varies from 0 to 100 so this will recharge up to
		//	100 units of fuel every 10 ticks or 10 units per tick. At 1.5MW per fuel
		//	unit, this means that a single armor plate can support up to 15MW when
		//	right next to the sun.

		pObj->Refuel((Metric)iIntensity);
		}

	//	See if we distribute HPs to other segments of our type

	if (!Stats.Distribute.IsEmpty() 
			&& pArmor->IsPrime())
		{
		//	Only works on ships (with segments).
		//	LATER: Introduce the concept of segments to stations.

		CShip *pShip = pObj->AsShip();

		//	Compute the HP that we distribute this cycle

		int iHP = Stats.Distribute.GetRegen(iTick, TICKS_PER_UPDATE);
		if (pShip && iHP > 0)
			{
			TArray<int> MaxHP;
			MaxHP.InsertEmpty(pShip->GetArmorSectionCount());

			//	Compute some stats for all armor segments of the same type.

			int iSegCount = 0;
			int iTotalMaxHP = 0;
			int iTotalHP = 0;
			int iTotalRepairNeeded = 0;
			int iMinRepairNeeded = 1000000000;
			int iMaxRepairNeeded = 0;
			for (i = 0; i < pShip->GetArmorSectionCount(); i++)
				{
				CInstalledArmor *pDistArmor = pShip->GetArmorSection(i);
				if (pDistArmor->GetClass() != pArmor->GetClass())
					continue;

				CItemCtx ItemCtx(pObj, pDistArmor);

				MaxHP[i] = GetMaxHP(ItemCtx);

				iSegCount++;
				int iRepairNeeded = MaxHP[i] - pDistArmor->GetHitPoints();

				if (iRepairNeeded < iMinRepairNeeded)
					iMinRepairNeeded = iRepairNeeded;
				if (iRepairNeeded > iMaxRepairNeeded)
					iMaxRepairNeeded = iRepairNeeded;

				iTotalRepairNeeded += MaxHP[i] - pDistArmor->GetHitPoints();
				}

			//	If we need repairs, distribute

			int iAverageRepairNeeded = (iTotalRepairNeeded / iSegCount);
			if ((iMaxRepairNeeded > iAverageRepairNeeded)
					&& (iMinRepairNeeded < iAverageRepairNeeded))
				{
				int iHPRemoved = 0;

				//	Loop and remove HP from any armor segment that has more
				//	HP than average, without exceeding our iHP budget.

				for (i = 0; i < pShip->GetArmorSectionCount() && iHPRemoved < iHP; i++)
					{
					CInstalledArmor *pDistArmor = pShip->GetArmorSection(i);
					if (pDistArmor->GetClass() != pArmor->GetClass())
						continue;

					int iRepairNeeded = MaxHP[i] - pDistArmor->GetHitPoints();
					if (iRepairNeeded < iAverageRepairNeeded)
						{
						int iHPToRemove = Min(iHP - iHPRemoved, iAverageRepairNeeded - iRepairNeeded);
						pDistArmor->IncHitPoints(-iHPToRemove);
						iHPRemoved += iHPToRemove;
						}
					}

				//	Now loop and distribute the HP to any armor segment that has
				//	less than average.

				for (i = 0; i < pShip->GetArmorSectionCount() && iHPRemoved > 0; i++)
					{
					CInstalledArmor *pDistArmor = pShip->GetArmorSection(i);
					if (pDistArmor->GetClass() != pArmor->GetClass())
						continue;

					int iRepairNeeded = MaxHP[i] - pDistArmor->GetHitPoints();
					if (iRepairNeeded > iAverageRepairNeeded)
						{
						int iHPToAdd = Min(iHPRemoved, iRepairNeeded - iAverageRepairNeeded);
						pDistArmor->IncHitPoints(iHPToAdd);
						iHPRemoved -= iHPToAdd;
						}
					}

				//	This should never happen, but if we have some HP left, then 
				//	add it back to each segment.

				if (iHPRemoved > 0)
					{
					ASSERT(false);

					for (i = 0; i < pShip->GetArmorSectionCount() && iHPRemoved > 0; i++)
						{
						CInstalledArmor *pDistArmor = pShip->GetArmorSection(i);
						if (pDistArmor->GetClass() != pArmor->GetClass())
							continue;

						int iRepairNeeded = MaxHP[i] - pDistArmor->GetHitPoints();
						if (iRepairNeeded > 0)
							{
							pDistArmor->IncHitPoints(1);
							iHPRemoved -= 1;
							}
						}
					}

				//	We've modified the armor
				//
				//	LATER: For now, only the prime segment will consume power, so we need to
				//	compute the number of distributed segments in the CalcPowerUsed method.

				pArmor->SetConsumePower(true);
				bModified = true;
				}
			}
		}

	//	Done

	*retbModified = bModified;

	DEBUG_CATCH
	}
