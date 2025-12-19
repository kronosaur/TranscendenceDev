//	SDamageCtx.cpp
//
//	SDamageCtx class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

SDamageCtx::SDamageCtx (void)

//	SDamageCtx constructor

	{
	m_pDesc = new CWeaponFireDesc;
	m_bFreeDesc = true;
	}

SDamageCtx::SDamageCtx (CSpaceObject *pObjHitArg, 
						CWeaponFireDesc &DescArg, 
						const CItemEnhancementStack *pEnhancementsArg,
						const CDamageSource &AttackerArg,
						CSpaceObject *pCauseArg,
						Metric rAgeArg,
						int iDirectionArg,
						const CVector &vHitPosArg,
						int iDamageArg) :
		pObj(pObjHitArg),
		m_pDesc(&DescArg),
		Damage(DescArg.CalcDamageDesc(pEnhancementsArg, AttackerArg, rAgeArg)),
		iDirection(iDirectionArg),
		vHitPos(vHitPosArg),
		pCause(pCauseArg),
		Attacker(AttackerArg),
		iDamage(iDamageArg)

//	SDamageCtx constructor
//
//	NOTE: This initializes SDamageCtx in preparation for calling pObjHit->Damage.
//	All such calls should go through this constructor.

	{
	//	Roll damage

	if (iDamage == -1)
		iDamage = Damage.RollDamage();

	//	Initialize damage effects

	InitDamageEffects(Damage);
	}

SDamageCtx::SDamageCtx (const DamageDesc &DamageArg)

//	SDamageCtx constructor

	{
	//	Initialize a descriptor

	m_pDesc = new CWeaponFireDesc;
	m_bFreeDesc = true;
	m_pDesc->InitFromDamage(DamageArg);

	//	Roll damage

	Damage = DamageArg;
	iDamage = Damage.RollDamage();

	//	Initialize damage effects

	InitDamageEffects(Damage);
	}

SDamageCtx::~SDamageCtx (void)

//	SDamageCtx destructor

	{
	if (m_bFreeDesc)
		delete m_pDesc;
	}

int SDamageCtx::CalcDamageMethodAdjDamageFromLevel(EDamageMethod iMethod, int iLevel, Metric rFortification) const
	{
	return mathRoundStochastic(Max(iDamage * CalcDamageMethodFortifiedAdjFromLevel(iMethod, iLevel, rFortification), g_pUniverse->GetEngineOptions().GetDamageMethodMinDamage()));
	}

int SDamageCtx::CalcDamageMethodAdjDamage(EDamageMethod iMethod, Metric rFortification) const
	{
	return mathRoundStochastic(Max(iDamage * CalcDamageMethodFortifiedAdj(iMethod, rFortification), g_pUniverse->GetEngineOptions().GetDamageMethodMinDamage()));
	}

int SDamageCtx::CalcDamageMethodAdjDamageRaw(EDamageMethod iMethod) const
	{
	return m_pDesc->GetDamage().CalcDamageMethodAdjDamage(iMethod, iDamage);
	}

int SDamageCtx::CalcDamageMethodAdjDamagePrecalc(Metric rPrecalcFortification) const
	{
	return mathRoundStochastic(Max(iDamage * rPrecalcFortification, g_pUniverse->GetEngineOptions().GetDamageMethodMinDamage()));
	}

//	CalcDamageMethodFortifiedAdj
// 
//	Computes a floating point adjusted form of WMD.
//  1.0 is full damage
//  rMinAdj should not be lower than 0.0
//  rMaxAdj should not be lower than rMinAdj
//
Metric SDamageCtx::CalcDamageMethodFortifiedAdj(EDamageMethod iMethod, Metric rFortification) const
	{
	return SDamageCtx::CalcDamageMethodFortifiedAdjFromLevel(iMethod, Damage.GetDamageMethodLevel(iMethod), rFortification);
	}

//	CalcDamageMethodFortifiedAdjFromLevel
// 
//	Computes a floating point adjusted form of a damage method.
//	This is done relative to the adventure or engine defined curve
// 
//	How the adjustment value works:
//  1.0 uses the unmodified adventure-defined curve
//  10.0 means taking 1/10th the damage if attacker does not have the required
//		damage method
//  inf means immunity
// 
//	The output curve is asymptotically reduced towards 0.0 or boosted towards 1.0
// 
//	This exponential algorithm was picked to both provide relatively intuitive math:
//	High fortification (>1) penalizes not having enough of the required damage method
//	Low fortification (<1) rewards having any amount of the required damage method
//
Metric SDamageCtx::CalcDamageMethodFortifiedAdjFromLevel(EDamageMethod iMethod, int iLevel, Metric rFortification)
	{
	//	We only adjust curves for WMD lower than 7, max WMD is always pinned to 1.0;

	if (iLevel == 7)
		return 1.0;

	//	Adjust for fortification 0 and 1 are trivial

	if (!rFortification)
		return 1.0;
	else if (rFortification == 1.0)
		return DamageDesc::GetDamageMethodAdjRealFromValue(iMethod, iLevel);

	//	Otherwise we need to apply a transform
	//	The math is exploded for debug builds, optimized builds collapse a bunch of this math;

	Metric rBaseAdj = DamageDesc::GetDamageMethodAdjRealFromValue(iMethod, 0);
	Metric rRawAdj = DamageDesc::GetDamageMethodAdjRealFromValue(iMethod, iLevel);
	Metric rBaseTarget = rBaseAdj / ((1 - rBaseAdj) * rFortification + rBaseAdj);
	Metric rNormalizedExponent = log(rBaseTarget/log(rBaseAdj));
	return pow(rRawAdj, rNormalizedExponent);
	}

void SDamageCtx::InitDamageEffects (const DamageDesc &DamageArg)

//	InitDamageEffects
//
//	Initialiazes damage effects. NOTE: Callers will later check immunities, etc.
//	and clear out or alter some of these damage effects.

	{
	//	Blinding

	int iBlinding = DamageArg.GetBlindingDamage();
	if (iBlinding)
		{
		//	The chance of being blinded is dependent
		//	on the rating.

		int iChance = 4 * iBlinding * iBlinding;
		m_bBlind = (mathRandom(1, 100) <= iChance);
		m_iBlindTime = iDamage * g_TicksPerSecond / 2;
		}
	else
		m_bBlind = false;

	//	Radiation

	m_bRadioactive = (DamageArg.GetRadiationDamage() > 0);

	//	EMP

	int iEMP = DamageArg.GetEMPDamage();
	if (iEMP)
		{
		//	The chance of being paralyzed is dependent
		//	on the EMP rating.

		int iChance = 4 * iEMP * iEMP;
		m_bParalyze = (mathRandom(1, 100) <= iChance);
		m_iParalyzeTime = iDamage * g_TicksPerSecond / 2;
		}
	else
		m_bParalyze = false;

	//	Device disrupt

	int iDeviceDisrupt = DamageArg.GetDeviceDisruptDamage();
	if (iDeviceDisrupt)
		{
		//	The chance of damaging a device depends on the rating.

		int iChance = 4 * iDeviceDisrupt * iDeviceDisrupt;
		m_bDeviceDisrupt = (mathRandom(1, 100) <= iChance);
		m_iDisruptTime = 2 * iDamage * g_TicksPerSecond;
		}
	else
		m_bDeviceDisrupt = false;

	//	Device damage

	int iDeviceDamage = DamageArg.GetDeviceDamage();
	if (iDeviceDamage)
		{
		//	The chance of damaging a device depends on the rating.

		int iChance = 4 * iDeviceDamage * iDeviceDamage;
		m_bDeviceDamage = (mathRandom(1, 100) <= iChance);
		}
	else
		m_bDeviceDamage = false;

	//	Disintegration

	int iDisintegration = DamageArg.GetDisintegrationDamage();
	if (iDisintegration)
		{
		//	The chance of being disintegrated is dependent on the rating.
		//	Since disintegration is from 1 to 7, chance is from 4 to 100.

		int iChance = (2 * iDisintegration * iDisintegration) + 2;
		m_bDisintegrate = (mathRandom(1, 100) <= iChance);
		}
	else
		m_bDisintegrate = false;

	//	Shatter

	int iShatter = DamageArg.GetShatterDamage();
	if (iShatter)
		{
		//	Compute the threshold mass. Below this size, we shatter the object

		int iMassLimit = 10 * mathPower(5, iShatter);
		m_bShatter = (pObj && pObj->GetMass() < iMassLimit);
		}
	else
		m_bShatter = false;

	//	Time Stop

	m_bTimeStop = (DamageArg.GetTimeStopDamageLevel() > 0);
	}

//	IsDamaging
// 
//	Check if this damage Ctx should be treated as a damaging effect
//
bool SDamageCtx::IsDamaging () const
	{
	//	We check iDamage explicitly because we may be initialized with an invalid
	//	DamageDesc, and some scripts are allowed to edit iDamage without changing
	//	the DamageDesc.

	return Damage.IsDamaging() || (iDamage && Damage.GetDamageType() != damageNull);
	}

//	IsDamageEventFiring
// 
//	Check if this damage Ctx should run scripts for damaging effects
//
bool SDamageCtx::IsDamageEventFiring () const
	{
	//	We check iDamage explicitly because we may be initialized with an invalid
	//	DamageDesc, and some scripts are allowed to edit iDamage without changing
	//	the DamageDesc.

	return IsDamaging() || iDamage || Damage.GetDamageType() == damageNull;
	}

void SDamageCtx::SetHint (EDamageHint iHint)

//	SetHint
//
//	Sets the hint.

	{
	if (iHint == EDamageHint::none || GetDesc().ShowsHint(iHint))
		m_iHint = iHint;
	}
