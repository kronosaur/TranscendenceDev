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

int SDamageCtx::CalcWMDAdjustedDamageFromLevel(int iLevel, Metric rWMD0FortificationAdj, Metric rMinAdj) const
	{
	return Max(mathRoundStochastic(iDamage * CalcWMDFortificationAdjFromLevel(iLevel, rWMD0FortificationAdj, rMinAdj)), g_pUniverse->GetEngineOptions().GetMassDestructionAdj()->GetWMDMinDamage());
	}

int SDamageCtx::CalcWMDAdjustedDamage(Metric rWMD0FortificationAdj, Metric rMinAdj) const
	{
	return Max(mathRoundStochastic(iDamage * CalcWMDFortificationAdj(rWMD0FortificationAdj, rMinAdj)), g_pUniverse->GetEngineOptions().GetMassDestructionAdj()->GetWMDMinDamage());
	}

int SDamageCtx::CalcWMDAdjustedDamageRaw() const
	{
	return m_pDesc->GetDamage().CalcWMDAdjustedDamage(iDamage);
	}

//	CalcWMDFortificationAdj
// 
//	Computes a floating point adjusted form of WMD.
//  1.0 is full damage
//  rMinAdj should not be lower than 0.0
//  rMaxAdj should not be lower than rMinAdj
//
Metric SDamageCtx::CalcWMDFortificationAdj(Metric rWMD0FortificationAdj, Metric rMinAdj, Metric rMaxAdj) const
	{
	return SDamageCtx::CalcWMDFortificationAdjFromLevel(Damage.GetMassDestructionLevel(), rWMD0FortificationAdj, rMinAdj, rMaxAdj);
	}

//	CalcWMDFortificationAdj
// 
//	Computes a floating point adjusted form of WMD.
//  1.0 is full damage
//  rMinAdj should not be lower than 0.0
//  rMaxAdj should not be lower than rMinAdj
//
Metric SDamageCtx::CalcWMDFortificationAdjFromLevel(int iLevel, Metric rWMD0FortificationAdj, Metric rMinAdj, Metric rMaxAdj)
	{
	//	We only adjust curves for WMD lower than 7, max WMD is always pinned.

	if (iLevel == 7)
		return 1.0 > rMaxAdj ? rMaxAdj : 1.0;

	//	Adjust for level 0 is trivial, its just rWMD0FortificationAdj

	if (iLevel == 0)
		return rWMD0FortificationAdj < rMinAdj ? rMinAdj : rWMD0FortificationAdj;

	//	Otherwise we need to do a linear transform
	//	The math is exploded for debug builds, optimized builds collapse a bunch of this math;

	Metric rBaseRange = 1.0 - DamageDesc::GetMassDestructionAdjRealFromValue(0);
	Metric rOutRange = 1.0 - rWMD0FortificationAdj;
	Metric rBasePos = 1.0 - DamageDesc::GetMassDestructionAdjRealFromValue(iLevel);
	Metric rTransform = rOutRange / rBaseRange;
	Metric rAdj = 1.0 - (rBasePos * rTransform);

	if (rAdj < rMinAdj)
		return rMinAdj;
	else if (rAdj > rMaxAdj)
		return rMaxAdj;
	else
		return rAdj;
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
