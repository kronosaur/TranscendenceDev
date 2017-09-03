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
#define MAX_SPEED_ATTRIB						CONSTLIT("maxSpeed")
#define MAX_SPEED_INC_ATTRIB					CONSTLIT("maxSpeedInc")
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
#define PROPERTY_DAMAGE_ADJ						CONSTLIT("damageAdj")
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
#define PROPERTY_STD_HP							CONSTLIT("stdHP")

static char g_HitPointsAttrib[] = "hitPoints";
static char g_DamageAdjAttrib[] = "damageAdj";
static char g_ItemIDAttrib[] = "itemID";
#define MAX_REFLECTION_CHANCE		95

#define MAX_REFLECTION_CHANCE		95

const int BLIND_IMMUNE_LEVEL =					6;
const int RADIATION_IMMUNE_LEVEL =				7;
const int EMP_IMMUNE_LEVEL =					9;
const int DEVICE_DAMAGE_IMMUNE_LEVEL =			11;

const Metric PHOTO_REPAIR_ADJ =					0.6;
const Metric RADIATION_IMMUNE_BALANCE_BONUS =	25.0;
const Metric RADIATION_VULNERABLE_BALANCE_BONUS =	-25.0;
const Metric BLIND_IMMUNE_BALANCE_BONUS =		10.0;
const Metric BLIND_VULNERABLE_BALANCE_BONUS =	-10.0;
const Metric EMP_IMMUNE_BALANCE_BONUS =			25.0;
const Metric EMP_VULNERABLE_BALANCE_BONUS =		-25.0;
const Metric DEVICE_DAMAGE_IMMUNE_BALANCE_BONUS =	25.0;
const Metric DEVICE_DAMAGE_VULNERABLE_BALANCE_BONUS =	-25.0;
const Metric SHATTER_IMMUNE_BALANCE_BONUS =		20.0;
const Metric DISINTEGRATION_IMMUNE_BALANCE_BONUS =	10.0;
const Metric HIGHER_REPAIR_LEVEL_BALANCE_BONUS = -5.0;
const Metric LOWER_REPAIR_LEVEL_BALANCE_BONUS =	2.5;
const Metric ARMOR_COMPLETE_BALANCE_ADJ =		-0.20;
const Metric STEALTH_BALANCE_BONUS =			5.0;
const Metric REFLECTION_BALANCE_BONUS =			50.0;
const Metric POWER_USE_BALANCE_ADJ =			-400.0;
const Metric PHOTO_RECHARGE_BALANCE_ADJ =		25.0;
const Metric MAX_SPEED_BALANCE_BONUS =			10;
const Metric SHIELD_INTERFERENCE_BALANCE_BONUS =	-150.0;

const Metric REGEN_BALANCE_ADJ =				10.0;	//	For each percent of regen/stdweapon ratio
const Metric DECAY_BALANCE_ADJ =				-3.0;
const Metric DIST_BALANCE_ADJ =					2.0;
const Metric MAX_REGEN_BALANCE_BONUS = 500.0;

const Metric MASS_BALANCE_K0 =					1.284;
const Metric MASS_BALANCE_K1 =					-0.47;
const Metric MASS_BALANCE_K2 =					0.014;
const Metric MASS_BALANCE_ADJ =					60.0;	//	Linear relationship between curve and mass balance
const Metric MASS_BALANCE_LIMIT =				16.0;	//	Above this mass (in tons) we don't get any additional bonus

const Metric BALANCE_COST_RATIO =				-0.5;	//  Each percent of cost above standard is a 0.5%
const Metric BALANCE_MAX_DAMAGE_ADJ =			400.0;	//	Max change in balance due to a single damage type

static CArmorClass::SStdStats STD_STATS[MAX_ITEM_LEVEL] =
	{
		//						Repair	Install
		//	HP		Cost		cost	cost		Mass
		{	35,		50,			1,		10,			2500, },	
		{	45,		100,		1,		20,			2600, },
		{	60,		200,		1,		40,			2800, },
		{	80,		400,		2,		80,			2900, },
		{	100,	800,		3,		160,		3000, },

		{	135,	1600,		4,		320,		3000, },
		{	175,	3200,		6,		640,		3000, },
		{	225,	6500,		9,		1300,		3000, },
		{	300,	13000,		15,		2600,		3000, },
		{	380,	25000,		22,		5000,		3000, },

		{	500,	50000,		35,		10000,		3000, },
		{	650,	100000,		52,		20000,		3000, },
		{	850,	200000,		80,		40000,		3000, },
		{	1100,	410000,		125,	82000,		3000, },
		{	1400,	820000,		190,	164000,		3000, },

		{	1850,	1600000,	300,	320000,		3000, },
		{	2400,	3250000,	450,	650000,		3000, },
		{	3100,	6500000,	700,	1300000,	3000, },
		{	4000,	13000000,	1050,	2600000,	3000, },
		{	5250,	26000000,	1650,	5200000,	3000, },

		{	6850,	52000000,	2540,	10400000,	3000, },
		{	9000,	100000000,	3900,	20000000,	3000, },
		{	12000,	200000000,	6000,	40000000,	3000, },
		{	15000,	400000000,	9300,	80000000,	3000, },
		{	20000,	800000000,	14300,	160000000,	3000, },
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

	//	Damage adjustment. This initializes Ctx.iArmorDamage.

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

	//	Give source events a chance to change the damage before we
	//	subtract from armor.

	if (pSource->HasOnDamageEvent())
		{
		pSource->FireOnDamage(Ctx);
		if (pSource->IsDestroyed())
			return damageDestroyed;
		}

	//	Compute how much damage the armor absorbs based on how much damage it
	//	took.

	if (pArmor->GetHitPoints() == 0)
		Ctx.iArmorAbsorb = 0;
	else if (Ctx.iArmorDamage <= pArmor->GetHitPoints())
		Ctx.iArmorAbsorb = Ctx.iDamage;
	else
		Ctx.iArmorAbsorb = pArmor->GetHitPoints() * Ctx.iDamage / Ctx.iArmorDamage;

	//	Absorb damage

	Ctx.iDamage = Max(0, Ctx.iDamage - Ctx.iArmorAbsorb);
	pArmor->IncHitPoints(-Ctx.iArmorDamage);

	//	If we took no damage, then say so.

	if (Ctx.iArmorDamage == 0 && !bCustomDamage)
		return damageNoDamage;
	else
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
	int i;
    bool bModified = false;

	CSpaceObject *pSource = ItemCtx.GetSource();
	CInstalledArmor *pArmor = ItemCtx.GetArmor();
	const CItemEnhancementStack &Enhancements = ItemCtx.GetEnhancements();

	//	Increment total armor mass

	Ctx.iArmorMass += ItemCtx.GetItem().GetMassKg();

    //  Adjust max speed.

	CArmorSystem *pArmorSet;
	if (m_iMaxSpeedInc 
			&& pSource 
			&& pArmor 
			&& pArmor->IsPrime()
			&& (pArmorSet = pSource->GetArmorSystem())
			&& pArmorSet->GetSegmentCount() > 0)
		{
		//	Count how many armor segments of this type

		int iCount = 0;
		for (i = 0; i < pArmorSet->GetSegmentCount(); i++)
			if (pArmorSet->GetSegment(i).GetClass() == pArmor->GetClass())
				iCount++;

		//	Compute the fraction of segments that are this type

		Metric rSetFraction = (Metric)iCount / (Metric)pArmorSet->GetSegmentCount();

		//	Prorate the speed bonud

		int iSpeed = mathRound(m_iMaxSpeedInc * rSetFraction);
		if (iSpeed != 0)
			{
			Ctx.DriveDesc.AddMaxSpeed(iSpeed * LIGHT_SECOND / 100.0);

			//	Set the maximum speed

			if (m_iMaxSpeedLimit == -1)
				Ctx.rMaxSpeedLimit = LIGHT_SPEED;
			else
				Ctx.rMaxSpeedLimit = Max(Ctx.rMaxSpeedLimit, m_iMaxSpeedLimit * LIGHT_SPEED / 100.0);
			}
		}

	//	Shield interference

	if (m_fShieldInterference || Enhancements.IsShieldInterfering())
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
//	Initializes Ctx.iArmorDamage to account for damage type adjustments, etc.

	{
	CInstalledArmor *pArmor = ItemCtx.GetArmor();

	//	Adjust for special armor damage:
	//
	//	<0	=	2.5x damage
	//	0	=	2x damage
	//	1	=	1.5x damage
	//	2	=	1.25x damage
	//	>2	=	1x damage

	int iDamageLevel = Ctx.Damage.GetArmorDamageLevel();
	if (iDamageLevel > 0)
		Ctx.iArmorDamage = mathAdjust(Ctx.iDamage, CalcArmorDamageAdj(ItemCtx, Ctx.Damage));
	else
		Ctx.iArmorDamage = Ctx.iDamage;

	//	Adjust for damage type

	int iDamageAdj = GetDamageAdj(ItemCtx, Ctx.Damage);
	Ctx.iArmorDamage = mathAdjust(Ctx.iArmorDamage, iDamageAdj);
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

int CArmorClass::CalcBalance (CItemCtx &ItemCtx, SBalance &retBalance) const

//	CalcBalance
//
//	Determines whether the given item is balanced for its level. Negative numbers
//	mean the item is underpowered. Positive numbers mean the item is overpowered.

	{
	//	Initialize

	const SScalableStats &Stats = GetScaledStats(ItemCtx);
	retBalance.iLevel = Stats.iLevel;

	//	Figure out how may HPs standard armor at this level should have.

	const SStdStats &StdStats = GetStdStats(retBalance.iLevel);

	//  Compute the number of balance points (BP) of the damage. +100 = double
	//  HP relative to standard. -100 = half HP relative to standard.

	retBalance.rHP = (int)GetMaxHP(ItemCtx, true);
	retBalance.rHPBalance = 100.0 * mathLog2(retBalance.rHP / (Metric)StdStats.iHP);
	retBalance.rBalance = retBalance.rHPBalance;

	//	Calculate the balance contribution of damage adjustment

	retBalance.rDamageAdj = CalcBalanceDamageAdj(ItemCtx, Stats);
	retBalance.rBalance += retBalance.rDamageAdj;

	retBalance.rDamageEffectAdj = CalcBalanceDamageEffectAdj(ItemCtx, Stats);
	retBalance.rBalance += retBalance.rDamageEffectAdj;

	//	Calculate regeneration/decay/etc.

	retBalance.rRegen = CalcBalanceRegen(ItemCtx, Stats);
	retBalance.rBalance += retBalance.rRegen;

	//	Repair tech

	retBalance.rRepairAdj = CalcBalanceRepair(ItemCtx, Stats);
	retBalance.rBalance += retBalance.rRepairAdj;

	//	If we have an armor complete bonus, we add a penalty to balance.

	if (m_iArmorCompleteBonus)
		{
		retBalance.rArmorComplete = ARMOR_COMPLETE_BALANCE_ADJ * 100.0 * mathLog2((Metric)(m_iArmorCompleteBonus + StdStats.iHP) / (Metric)StdStats.iHP);
		retBalance.rBalance += retBalance.rArmorComplete;
		}
	else
		retBalance.rArmorComplete = 0.0;

	//	Stealth

	if (m_iStealth >= 12)
		retBalance.rStealth = 4.0 * STEALTH_BALANCE_BONUS;
	else if (m_iStealth >= 10)
		retBalance.rStealth = 3.0 * STEALTH_BALANCE_BONUS;
	else if (m_iStealth >= 8)
		retBalance.rStealth = 2.0 * STEALTH_BALANCE_BONUS;
	else if (m_iStealth >= 6)
		retBalance.rStealth = 1.0 * STEALTH_BALANCE_BONUS;
	else
		retBalance.rStealth = 0.0;

	retBalance.rBalance += retBalance.rStealth;

	//	Power use

	retBalance.rPowerUse = CalcBalancePower(ItemCtx, Stats);
	retBalance.rBalance += retBalance.rPowerUse;

	//	Speed adjustment

	retBalance.rSpeedAdj = MAX_SPEED_BALANCE_BONUS * m_iMaxSpeedInc;
	retBalance.rBalance += retBalance.rSpeedAdj;

	//	Special features

	retBalance.rDeviceBonus = CalcBalanceSpecial(ItemCtx, Stats);
	retBalance.rBalance += retBalance.rDeviceBonus;
	
	//	Mass

	retBalance.rMass = CalcBalanceMass(ItemCtx, Stats);
	retBalance.rBalance += retBalance.rMass;

	//	Cost

	Metric rCost = (Metric)CEconomyType::ExchangeToCredits(m_pItemType->GetCurrencyAndValue(ItemCtx, true));
	Metric rCostDelta = 100.0 * (rCost - StdStats.iCost) / (Metric)StdStats.iCost;
	retBalance.rCost = BALANCE_COST_RATIO * rCostDelta;
	retBalance.rBalance += retBalance.rCost;

	return (int)retBalance.rBalance;
	}

Metric CArmorClass::CalcBalanceDamageAdj (CItemCtx &ItemCtx, const SScalableStats &Stats) const

//	CalcBalanceDamageAdj
//
//	Calculates the balande contribution of damage adjustment. 0 = same as 
//	standard for level.

	{
	DamageTypes iDamage;

	//	Loop over all damage types and accumulate balance adjustments.

	Metric rTotalBalance = 0.0;
	for (iDamage = damageLaser; iDamage < damageCount; iDamage = (DamageTypes)(iDamage + 1))
		{
		int iAdj;
		int iDefaultAdj;
		Stats.DamageAdj.GetAdjAndDefault(iDamage, &iAdj, &iDefaultAdj);

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

		//	Adjust the based on how common this damage type is at the given 
		//	armor level.

		rBalance *= CDamageAdjDesc::GetDamageTypeFraction(Stats.iLevel, iDamage);

		//	Accumulate

		rTotalBalance += rBalance;
		}

	//	Done

	return rTotalBalance;
	}

Metric CArmorClass::CalcBalanceDamageEffectAdj (CItemCtx &ItemCtx, const SScalableStats &Stats) const

//	CalcBalanceDamageEffectAdj
//
//	Return balance based on immunity/vulnerability to special damage effects.

	{
	Metric rBalance = 0.0;

	//	Immune/vulnerable to radiation?

	if (Stats.fRadiationImmune)
		{
		if (Stats.iLevel < RADIATION_IMMUNE_LEVEL)
			rBalance += RADIATION_IMMUNE_BALANCE_BONUS;
		}
	else if (Stats.iLevel >= RADIATION_IMMUNE_LEVEL)
		rBalance += RADIATION_VULNERABLE_BALANCE_BONUS;

	//	Immune/vulnerable to blinding?

	if (Stats.iBlindingDamageAdj < 50)
		{
		if (Stats.iLevel < BLIND_IMMUNE_LEVEL)
			rBalance += BLIND_IMMUNE_BALANCE_BONUS;
		}
	else if (Stats.iLevel >= BLIND_IMMUNE_LEVEL)
		rBalance += BLIND_VULNERABLE_BALANCE_BONUS;

	//	Immune/vulnerable to EMP?

	if (Stats.iEMPDamageAdj < 50)
		{
		if (Stats.iLevel < EMP_IMMUNE_LEVEL)
			rBalance += EMP_IMMUNE_BALANCE_BONUS;
		}
	else if (Stats.iLevel >= EMP_IMMUNE_LEVEL)
		rBalance += EMP_VULNERABLE_BALANCE_BONUS;

	//	Immune/vulnerable to device damage?

	if (Stats.iDeviceDamageAdj < 50)
		{
		if (Stats.iLevel < DEVICE_DAMAGE_IMMUNE_LEVEL)
			rBalance += DEVICE_DAMAGE_IMMUNE_BALANCE_BONUS;
		}
	else if (Stats.iLevel >= DEVICE_DAMAGE_IMMUNE_LEVEL)
		rBalance += DEVICE_DAMAGE_VULNERABLE_BALANCE_BONUS;

	//	Immune/vulnerable to shatter?

	if (m_fShatterImmune)
		rBalance += SHATTER_IMMUNE_BALANCE_BONUS;

	//	Immune/vulnerable to disintegration?

	if (m_fDisintegrationImmune)
		rBalance += DISINTEGRATION_IMMUNE_BALANCE_BONUS;

	return rBalance;
	}

Metric CArmorClass::CalcBalanceMass (CItemCtx &ItemCtx, const SScalableStats &Stats) const

//	CalcBalanceMass
//
//	Calculate balance from armor mass

	{
	//	Mass in metric tons.

	Metric rMass = CItem(m_pItemType, 1).GetMass();
	if (rMass == 0.0)
		return 0.0;

	//	Because this is an x^2 curve, we need a limit on mass or else we will start
	//	to curve up (more mass = bonus, which we don't want).

	rMass = Min(rMass, MASS_BALANCE_LIMIT);

	//	This polynomial generates a balance based on mass.

	return MASS_BALANCE_ADJ * ((MASS_BALANCE_K2 * rMass * rMass) + MASS_BALANCE_K1 * rMass + MASS_BALANCE_K0);
	}

Metric CArmorClass::CalcBalancePower (CItemCtx &ItemCtx, const SScalableStats &Stats) const

//	CalcBalancePower
//
//	Calculate balance from consumption/production of power.

	{
	Metric rTotalBalance = 0.0;

	if (m_iPowerUse)
		{
		//	Compare power consumption to what an average shield requires.

		rTotalBalance += POWER_USE_BALANCE_ADJ * (Metric)m_iPowerUse / CShieldClass::GetStdPower(Stats.iLevel);
		}

	if (m_fPhotoRecharge)
		{
		rTotalBalance += PHOTO_RECHARGE_BALANCE_ADJ * (Metric)150 / CShieldClass::GetStdPower(Stats.iLevel);
		}

	return rTotalBalance;
	}

Metric CArmorClass::CalcBalanceRegen (CItemCtx &ItemCtx, const SScalableStats &Stats) const

//	CalcBalanceRegen
//
//	Calculate balance from regeneration.

	{
	Metric rTotalBalance = 0.0;

	//	Compute the number of HP regenerated in 180 ticks

	Metric rRegenHP = Stats.Regen.GetHPPer180(TICKS_PER_UPDATE);
	if (rRegenHP > 0.0)
		{
		//	Adjust regen based on type. For example, photo-repair is not quite as
		//	good as unconditional repair.

		if (m_fPhotoRepair)
			rRegenHP *= PHOTO_REPAIR_ADJ;

		//	Compare this value against the average weapon damage.

		Metric rWeaponHP = CWeaponClass::GetStdDamage180(Stats.iLevel);

		//	If regen is less than weapon damage, then this is equivalent to
		//	increasing armor HP for some factor.

		if (rRegenHP < rWeaponHP)
			{
			Metric rBalance = REGEN_BALANCE_ADJ * 100.0 * rRegenHP / rWeaponHP;
			rTotalBalance += Min(rBalance, MAX_REGEN_BALANCE_BONUS);
			}

		//	Otherwise, we get a large bonus because the average weapon can't 
		//	hurt us.

		else
			rTotalBalance += MAX_REGEN_BALANCE_BONUS;
		}

	//	Decay is a penalty to balance.

	Metric rDecayHP = Stats.Decay.GetHPPer180(TICKS_PER_UPDATE);
	if (rDecayHP > 0.0)
		{
		//	Compare this value against the average weapon damage.

		Metric rWeaponHP = CWeaponClass::GetStdDamage180(Stats.iLevel);
		Metric rBalance = DECAY_BALANCE_ADJ * 100.0 * rDecayHP / rWeaponHP;
		rTotalBalance += rBalance;
		}

	//	Distribution is a bonus

	Metric rDistHP = Stats.Distribute.GetHPPer180(TICKS_PER_UPDATE);
	if (rDistHP > 0.0)
		{
		//	Compare this value against the average weapon damage.

		Metric rWeaponHP = CWeaponClass::GetStdDamage180(Stats.iLevel);

		//	If regen is less than weapon damage, then this is equivalent to
		//	increasing armor HP for some factor.

		if (rDistHP < rWeaponHP)
			{
			Metric rBalance = DIST_BALANCE_ADJ * 100.0 * rDistHP / rWeaponHP;
			rTotalBalance += Min(rBalance, MAX_REGEN_BALANCE_BONUS);
			}

		//	Otherwise, we get a large bonus because the average weapon can't 
		//	hurt us.

		else
			rTotalBalance += DIST_BALANCE_ADJ * MAX_REGEN_BALANCE_BONUS;
		}

	return rTotalBalance;
	}

Metric CArmorClass::CalcBalanceRepair (CItemCtx &ItemCtx, const SScalableStats &Stats) const

//	CalcBalanceRepair
//
//	Calculates the balance for repairing bonuses

	{
	Metric rTotalBalance = 0.0;

	//	Repair tech (NOTE we use base level as comparison because repair tech
	//	does not currently scale).

	int iRepair = m_pItemType->GetLevel(CItemCtx()) - m_iRepairTech;
	if (iRepair < 0)
		rTotalBalance += HIGHER_REPAIR_LEVEL_BALANCE_BONUS * -iRepair;
	else if (iRepair > 0)
		rTotalBalance += LOWER_REPAIR_LEVEL_BALANCE_BONUS * iRepair;

	//	Adjust for repair cost

	Metric rStdCost = STD_STATS[m_iRepairTech - 1].iRepairCost;
	Metric rDiff = rStdCost - (Metric)CEconomyType::Default()->Exchange(m_Stats.RepairCost);
	if (rDiff < 0.0)
		rTotalBalance += Max(-40.0, 10.0 * rDiff / rStdCost);
	else if (rDiff > 0.0)
		rTotalBalance += 25.0 * rDiff / rStdCost;

	return rTotalBalance;
	}

Metric CArmorClass::CalcBalanceSpecial (CItemCtx &ItemCtx, const SScalableStats &Stats) const

//	CalcBalanceSpecial
//
//	Calc balance of special features.

	{
	Metric rTotalBalance = 0.0;

	//	Device bonus

	if (m_iDeviceBonus)
		{
		rTotalBalance += m_iDeviceBonus;
		}

	//	Shield interference

	if (m_fShieldInterference)
		rTotalBalance += SHIELD_INTERFERENCE_BALANCE_BONUS;

	return rTotalBalance;
	}

void CArmorClass::CalcDamageEffects (CItemCtx &ItemCtx, SDamageCtx &Ctx)

//	CalcDamageEffects
//
//	Initialize the damage effects based on the damage and on this armor type.

	{
    const SScalableStats &Stats = GetScaledStats(ItemCtx);
	CSpaceObject *pSource = ItemCtx.GetSource();
	CInstalledArmor *pArmor = ItemCtx.GetArmor();

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

int CArmorClass::CalcPowerUsed (SUpdateCtx &Ctx, CSpaceObject *pSource, CInstalledArmor *pArmor)

//	CalcPowerUsed
//
//	Computes the amount of power used by this armor segment (this
//	only applies to powered armor). Negative numbers mean we generate power.

	{
	int iTotalPower = 0;

	//	If we did work (regenerated), then we use full power. Otherwise, we use
	//	idle power.
	//
	//	NOTE: By default, idle power is the same as full power, but some armors
	//	have different values.

	if (pArmor->ConsumedPower())
		iTotalPower = m_iPowerUse;
	else
		iTotalPower = m_iIdlePowerUse;

	//	See if we generate solar power

	if (m_iPowerGen > 0)
		{
		if (m_fPhotoRecharge)
			{
			iTotalPower -= (m_iPowerGen * Ctx.GetLightIntensity(pSource) / 100);
			}
		else
			iTotalPower -= m_iPowerGen;
		}

	//	Done

	return iTotalPower;
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

	if (error = pArmor->m_Stats.InstallCost.InitFromXMLAndDefault(Ctx, pDesc->GetAttribute(INSTALL_COST_ATTRIB), CCurrencyAndValue(STD_STATS[iLevel - 1].iInstallCost)))
		return error;

	int iInstallCostAdj = pDesc->GetAttributeIntegerBounded(INSTALL_COST_ADJ_ATTRIB, 0, -1, 100);
	if (iInstallCostAdj != 100)
		pArmor->m_Stats.InstallCost.Adjust(iInstallCostAdj);

	//	Repair tech defaults to level

	pArmor->m_iRepairTech = pDesc->GetAttributeIntegerBounded(REPAIR_TECH_ATTRIB, 1, MAX_TECH_LEVEL, iLevel);

	//	Repair cost is based on repair tech

	if (error = pArmor->m_Stats.RepairCost.InitFromXMLAndDefault(Ctx, pDesc->GetAttribute(REPAIR_COST_ATTRIB), CCurrencyAndValue(STD_STATS[pArmor->m_iRepairTech - 1].iRepairCost)))
		return error;

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
	pArmor->m_fShieldInterference = pDesc->GetAttributeBool(SHIELD_INTERFERENCE_ATTRIB);
	pArmor->m_fChargeRepair = pDesc->GetAttributeBool(CHARGE_REGEN_ATTRIB);
	pArmor->m_fChargeDecay = pDesc->GetAttributeBool(CHARGE_DECAY_ATTRIB);
    pArmor->m_fHealerRepair = pDesc->GetAttributeBool(HEALER_REGEN_ATTRIB);

	//	Solar power

	CString sAttrib = pDesc->GetAttribute(PHOTO_RECHARGE_ATTRIB);
	if (sAttrib.IsBlank())
		{
		pArmor->m_fPhotoRecharge = false;
		pArmor->m_iPowerGen = 0;
		}
	else if (CXMLElement::IsBoolTrueValue(sAttrib))
		{
		pArmor->m_fPhotoRecharge = true;
		pArmor->m_iPowerGen = 0;
		}
	else
		{
		pArmor->m_iPowerGen = Max(0, strToInt(sAttrib, 0));
		pArmor->m_fPhotoRecharge = (pArmor->m_iPowerGen > 0);
		}

	//	Stealth

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

	pArmor->m_iMaxSpeedInc = pDesc->GetAttributeIntegerBounded(MAX_SPEED_INC_ATTRIB, -100, 100, 0);
	pArmor->m_iMaxSpeedLimit = pDesc->GetAttributeIntegerBounded(MAX_SPEED_ATTRIB, 0, 100, -1);

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
		*retsValue = strFromInt(CalcBalance(CItemCtx(), SBalance()));
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
		CInstalledArmor *pArmor = ItemCtx.GetArmor();

		//	Setup arguments

		CCodeChainCtx CCCtx;
		CCCtx.SaveAndDefineSourceVar(ItemCtx.GetSource());
		CCCtx.SaveAndDefineItemVar(ItemCtx);

		CCCtx.DefineDamageCtx(Ctx, Ctx.iArmorDamage);
		CCCtx.DefineInteger(CONSTLIT("aArmorHP"), (pArmor ? pArmor->GetHitPoints() : 0));
		CCCtx.DefineDamageEffects(CONSTLIT("aDamageEffects"), Ctx);
		CCCtx.DefineInteger(CONSTLIT("aFullDamageHP"), Ctx.iDamage);

		ICCItem *pResult = CCCtx.Run(Event);

		//	If we return Nil, then nothing

		if (pResult->IsNil())
			NULL;

		//	If we return an integer, then this is the damage that armor should take

		else if (pResult->IsInteger())
			Ctx.iArmorDamage = pResult->GetIntegerValue();

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

int CArmorClass::GetDamageAdj (CItemCtx &ItemCtx, const DamageDesc &Damage) const

//	GetDamageAdj
//
//	Returns the damage adjustment for the given damage type

	{
	int iDamageAdj = GetDamageAdj(ItemCtx, Damage.GetDamageType());
	if (iDamageAdj >= CDamageAdjDesc::MAX_DAMAGE_ADJ)
		return CDamageAdjDesc::MAX_DAMAGE_ADJ;

	const CItemEnhancementStack &Enhancements = ItemCtx.GetEnhancements();
	if (Enhancements.IsEmpty())
		return iDamageAdj;

	return Enhancements.ApplyDamageAdj(Damage, iDamageAdj);
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

	else if (strEquals(sName, PROPERTY_DAMAGE_ADJ))
		return Stats.DamageAdj.GetDamageAdjProperty(pEnhancements);

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

	else if (strEquals(sName, PROPERTY_STD_HP))
		return CC.CreateInteger(GetStdHP(Stats.iLevel));

	else
		return NULL;
	}

int CArmorClass::GetMaxHP (CItemCtx &ItemCtx, bool bForceComplete) const

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

	const CItemEnhancementStack &Enhancements = ItemCtx.GetEnhancements();
	if (!Enhancements.IsEmpty())
		iHP = iHP + ((iHP * Enhancements.GetBonus()) / 100);

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
		int iAdj = GetDamageAdj(ItemCtx, Damage);

		if (retArray)
			retArray[i] = CalcHPDamageAdj(iHP, iAdj);
		}

	return true;
	}

bool CArmorClass::GetReferenceSpeedBonus (CItemCtx &Ctx, int *retiSpeedBonus) const

//	GetReferenceSpeedBonus
//
//	Returns the speed bonus/penalty (both intrinsic and due to mass).

	{
	int iBonus = m_iMaxSpeedInc;

	//	Include adjustment for mass

	CShipClass *pShipClass;
	if (pShipClass = Ctx.GetSourceShipClass())
		{
		int iArmorMass = m_pItemType->GetMassKg(Ctx);

		//	If this armor is too heavy to be installed in the ship class, then
		//	we return TRUE, but speed bonus = 0.

		if (iArmorMass > pShipClass->GetMaxArmorMass())
			{
			if (retiSpeedBonus)
				*retiSpeedBonus = 0;
			return true;
			}

		//	Otherwise, add to bonus

		iBonus += pShipClass->CalcArmorSpeedBonus(iArmorMass * pShipClass->GetArmorDesc().GetCount());
		}

	//	Done

	if (iBonus == 0)
		return false;

	if (retiSpeedBonus)
		*retiSpeedBonus = iBonus;

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

bool CArmorClass::IsBlindingDamageImmune (CItemCtx &ItemCtx)

//	IsBlindingDamageImmune
//
//	Returns TRUE if we're immune to blinding.

	{
	const SScalableStats &Stats = GetScaledStats(ItemCtx);
	const CItemEnhancementStack &Enhancements = ItemCtx.GetEnhancements();

	return (Stats.iBlindingDamageAdj == 0 || Enhancements.IsBlindingImmune()); 
	}

bool CArmorClass::IsDeviceDamageImmune (CItemCtx &ItemCtx)

//	IsDeviceDamageImmune
//
//	Returns TRUE if we're immune to device damage

	{
	const SScalableStats &Stats = GetScaledStats(ItemCtx);
	const CItemEnhancementStack &Enhancements = ItemCtx.GetEnhancements();

	return (Stats.iDeviceDamageAdj == 0 || Enhancements.IsDeviceDamageImmune()); 
	}

bool CArmorClass::IsDisintegrationImmune (CItemCtx &ItemCtx)

//	IsDisintegrationImmune
//
//	Returns TRUE if we're immune to disintegration.

	{
	const CItemEnhancementStack &Enhancements = ItemCtx.GetEnhancements();
	return (m_fDisintegrationImmune || Enhancements.IsDisintegrationImmune());
	}

bool CArmorClass::IsEMPDamageImmune (CItemCtx &ItemCtx)

//	IsEMPDamageImmune
//
//	Returns TRUE if we are immune to EMP damage.

	{
	const SScalableStats &Stats = GetScaledStats(ItemCtx); 
	const CItemEnhancementStack &Enhancements = ItemCtx.GetEnhancements();

	return (Stats.iEMPDamageAdj == 0 || Enhancements.IsEMPImmune()); 
	}

bool CArmorClass::IsRadiationImmune (CItemCtx &ItemCtx)

//	IsRadiationImmune
//
//	Returns TRUE if we are immune to radiation.

	{
	const SScalableStats &Stats = GetScaledStats(ItemCtx);
	const CItemEnhancementStack &Enhancements = ItemCtx.GetEnhancements();

	return (Stats.fRadiationImmune || Enhancements.IsRadiationImmune());
	}

bool CArmorClass::IsShatterImmune (CItemCtx &ItemCtx)

//	IsShatterImmune
//
//	Returns TRUE if we are immune to shatter.

	{
	const CItemEnhancementStack &Enhancements = ItemCtx.GetEnhancements();
	return (m_fShatterImmune || Enhancements.IsShatterImmune());
	}

bool CArmorClass::IsShieldInterfering (CItemCtx &ItemCtx)

//	IsShieldInterfering
//
//	Returns TRUE if we interfere with shields.

	{
	const CItemEnhancementStack &Enhancements = ItemCtx.GetEnhancements();
	return (m_fShieldInterference || Enhancements.IsShieldInterfering());
	}

bool CArmorClass::IsReflective (CItemCtx &ItemCtx, const DamageDesc &Damage)

//	IsReflective
//
//	Returns TRUE if the armor reflects this damage

	{
	const CItemEnhancementStack &Enhancements = ItemCtx.GetEnhancements();

	int iReflectChance = 0;

	//	Base armor chance

	if (m_Reflective.InSet(Damage.GetDamageType()))
		iReflectChance = MAX_REFLECTION_CHANCE;

	//	Mods

	int iModReflect;
	if (!Enhancements.IsEmpty() && Enhancements.ReflectsDamage(Damage.GetDamageType(), &iModReflect))
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

void CArmorClass::Update (CItemCtx &ItemCtx, SUpdateCtx &UpdateCtx, int iTick, bool *retbModified)

//	Update
//
//	Updates the armor. This should be called once every 10 ticks

	{
    DEBUG_TRY

    const SScalableStats &Stats = GetScaledStats(ItemCtx);
	CSpaceObject *pSource = ItemCtx.GetSource();
	CInstalledArmor *pArmor = ItemCtx.GetArmor();
	const CItemEnhancementStack &Enhancements = ItemCtx.GetEnhancements();

	bool bModified = false;

	//	Default to not consume power. Below we set the flag if we do any kind 
	//	of regeneration work.

	pArmor->SetConsumePower(false);

	//	Compute total regeneration by adding mods to intrinsic. If we regenerate,
	//	then regenerate.

	CRegenDesc Regen;
	if (Enhancements.CalcRegen(ItemCtx, UpdateCtx, Stats.Regen, m_fPhotoRepair, TICKS_PER_UPDATE, Regen))
		{
		if (UpdateRegen(ItemCtx, Regen, iTick))
			bModified = true;
		}

	//	See if we're decaying

	if (pArmor->GetHitPoints() > 0
			&& (Enhancements.IsDecaying() || !Stats.Decay.IsEmpty()))
		{
		if (UpdateDecay(ItemCtx, Stats, iTick))
			bModified = true;
		}

	//	If this is solar armor then recharge the object. This is the old-style
	//	solar armor which creates fuel instead of generating power.

	if ((Enhancements.IsPhotoRecharging() || m_fPhotoRecharge)
			&& m_iPowerGen == 0)
		{
		int iIntensity = UpdateCtx.GetLightIntensity(pSource);
		
		//	Intensity varies from 0 to 100 so this will recharge up to
		//	100 units of fuel every 10 ticks or 10 units per tick. At 1.5MW per fuel
		//	unit, this means that a single armor plate can support up to 15MW when
		//	right next to the sun.

		pSource->Refuel((Metric)iIntensity);
		}

	//	See if we distribute HPs to other segments of our type

	if (!Stats.Distribute.IsEmpty() 
			&& pArmor->IsPrime())
		{
		if (UpdateDistribute(ItemCtx, Stats, iTick))
			bModified = true;
		}

	//	Done

	*retbModified = bModified;

	DEBUG_CATCH
	}

bool CArmorClass::UpdateDecay (CItemCtx &ItemCtx, const SScalableStats &Stats, int iTick)

//	UpdateDecay
//
//	Decays hit points. We return TRUE if any hit points lost.

	{
	CSpaceObject *pObj = ItemCtx.GetSource();
	CInstalledArmor *pArmor = ItemCtx.GetArmor();
	const CItemEnhancementStack &Enhancements = ItemCtx.GetEnhancements();

	//	If we require charges, then we're limited to the charges we have

	int iMaxDecay = pArmor->GetHitPoints();
	if (m_fChargeDecay)
		iMaxDecay = Min(iMaxDecay, pArmor->GetCharges(pObj));

	//	Decay

	if (iMaxDecay <= 0)
		return false;

	//	Combine decay with mod

	const CRegenDesc *pDecay;
	CRegenDesc DecayWithMod;
	if (Enhancements.IsDecaying())
		{
		DecayWithMod.Init(4);
		DecayWithMod.Add(Stats.Decay);
		pDecay = &DecayWithMod;
		}
	else
		pDecay = &Stats.Decay;

	//	Compute the HP that we decay this cycle

	int iHP = Min(iMaxDecay, pDecay->GetRegen(iTick, TICKS_PER_UPDATE));
	if (iHP <= 0)
		return false;

	//	Consume charges

	if (m_fChargeDecay)
		pArmor->IncCharges(pObj, -iHP);

	//	Decrement

	pArmor->IncHitPoints(-iHP);
	return true;
	}

bool CArmorClass::UpdateDistribute (CItemCtx &ItemCtx, const SScalableStats &Stats, int iTick)

//	UpdateDistribute
//
//	Distribute hit points. We return TRUE if any hit points were distributed.

	{
	int i;
	CSpaceObject *pObj = ItemCtx.GetSource();
	CInstalledArmor *pArmor = ItemCtx.GetArmor();

	//	Only works on ships (with segments).
	//	LATER: Introduce the concept of segments to stations.

	CShip *pShip = pObj->AsShip();
	if (pShip == NULL)
		return false;

	//	Compute the HP that we distribute this cycle

	int iHP = Stats.Distribute.GetRegen(iTick, TICKS_PER_UPDATE);
	if (iHP <= 0)
		return false;

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
	if ((iMaxRepairNeeded <= iAverageRepairNeeded)
			|| (iMinRepairNeeded >= iAverageRepairNeeded))
		return false;

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
	return true;
	}

bool CArmorClass::UpdateRegen (CItemCtx &ItemCtx, const CRegenDesc &Regen, int iTick)

//	UpdateRegen
//
//	Regenerates hit points. We return TRUE if any hit points were regenerated.

	{
	CSpaceObject *pObj = ItemCtx.GetSource();
	CInstalledArmor *pArmor = ItemCtx.GetArmor();
	const CItemEnhancementStack &Enhancements = ItemCtx.GetEnhancements();

	int iHPNeeded = GetMaxHP(ItemCtx) - pArmor->GetHitPoints();
	if (iHPNeeded <= 0)
		return false;

	//	Sometimes we need access to the armor system that track healer
	//	points. In the case of ships with multiple sections, we need the
	//	root object.

	CArmorSystem *pHealerSystem = NULL;
	if (m_fHealerRepair)
		{
		CSpaceObject *pRoot = pObj->GetAttachedRoot();
		pHealerSystem = (pRoot ? pRoot->GetArmorSystem() : pObj->GetArmorSystem());
		}

	//	If we require charges, then we're limited to the charges we have

    if (m_fChargeRepair)
        iHPNeeded = Min(iHPNeeded, pArmor->GetCharges(pObj));
    else if (m_fHealerRepair && pHealerSystem)
        iHPNeeded = Min(iHPNeeded, pHealerSystem->GetHealerLeft());

	//	Regen

	if (iHPNeeded <= 0)
		return false;

	//	Compute the HP that we regenerate this cycle

	int iHP = Min(iHPNeeded, Regen.GetRegen(iTick, TICKS_PER_UPDATE));
	if (iHP <= 0)
		return false;

	//	Repair
	//
	//	If we require charges to regen, then consume charges

    if (m_fChargeRepair)
        pArmor->IncCharges(pObj, -iHP);
    else if (m_fHealerRepair && pHealerSystem)
        pHealerSystem->IncHealerLeft(-iHP);

	pArmor->IncHitPoints(iHP);
	pArmor->SetConsumePower(true);

	return true;
	}
