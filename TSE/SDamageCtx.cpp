//	SDamageCtx.cpp
//
//	SDamageCtx class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

SDamageCtx::SDamageCtx (CSpaceObject *pObjHitArg, 
						CWeaponFireDesc *pDescArg, 
						const CItemEnhancementStack *pEnhancementsArg,
						CDamageSource &AttackerArg,
						CSpaceObject *pCauseArg,
						int iDirectionArg,
						const CVector &vHitPosArg,
						int iDamageArg) :
		pObj(pObjHitArg),
		pDesc(pDescArg),
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
	//	Initialize damage structure

	Damage = pDescArg->GetDamage();
	if (pEnhancementsArg)
		Damage.AddEnhancements(pEnhancementsArg);
	Damage.SetCause(AttackerArg.GetCause());
	if (AttackerArg.IsAutomatedWeapon())
		Damage.SetAutomatedWeapon();

	//	Roll damage

	if (iDamage == -1)
		iDamage = Damage.RollDamage();

	//	Initialize damage effects

	InitDamageEffects(Damage);
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
