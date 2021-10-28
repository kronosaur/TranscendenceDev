//	CWeaponFireDesc.cpp
//
//	CWeaponFireDesc class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define DAMAGE_TAG								CONSTLIT("Damage")
#define EFFECT_TAG								CONSTLIT("Effect")
#define ENHANCED_TAG							CONSTLIT("Enhanced")
#define EVENTS_TAG								CONSTLIT("Events")
#define MISSILE_EXHAUST_TAG						CONSTLIT("Exhaust")
#define FIRE_EFFECT_TAG							CONSTLIT("FireEffect")
#define FRAGMENT_TAG							CONSTLIT("Fragment")
#define HIT_EFFECT_TAG							CONSTLIT("HitEffect")
#define IMAGE_TAG								CONSTLIT("Image")
#define PARTICLE_SYSTEM_TAG						CONSTLIT("ParticleSystem")

#define ACCELERATION_FACTOR_ATTRIB				CONSTLIT("accelerationFactor")
#define AMMO_ID_ATTRIB							CONSTLIT("ammoID")
#define ARC_ANGLE_ATTRIB						CONSTLIT("arcAngle")
#define AREA_DAMAGE_DENSITY_ATTRIB				CONSTLIT("areaDamageDensity")
#define AUTO_TARGET_ATTRIB						CONSTLIT("autoAcquireTarget")
#define CAN_HIT_SOURCE_ATTRIB					CONSTLIT("canHitSource")
#define COUNT_ATTRIB							CONSTLIT("count")
#define EXHAUST_RATE_ATTRIB						CONSTLIT("creationRate")
#define DAMAGE_ATTRIB							CONSTLIT("damage")
#define DAMAGE_AT_MAX_RANGE_ATTRIB				CONSTLIT("damageAtMaxRange")
#define DIRECTIONAL_ATTRIB						CONSTLIT("directional")
#define DIRECTION_ATTRIB						CONSTLIT("direction")
#define EXHAUST_DRAG_ATTRIB						CONSTLIT("drag")
#define EFFECT_ATTRIB							CONSTLIT("effect")
#define EXPANSION_SPEED_ATTRIB					CONSTLIT("expansionSpeed")
#define EXPLOSION_TYPE_ATTRIB					CONSTLIT("explosionType")
#define FAILSAFE_ATTRIB							CONSTLIT("failsafe")
#define FIRE_EFFECT_ATTRIB						CONSTLIT("fireEffect")
#define FIRE_RATE_ATTRIB						CONSTLIT("fireRate")
#define FRAGMENT_COUNT_ATTRIB					CONSTLIT("fragmentCount")
#define FRAGMENT_INTERVAL_ATTRIB				CONSTLIT("fragmentInterval")
#define FRAGMENT_MAX_RADIUS_ATTRIB				CONSTLIT("fragmentMaxRadius")
#define FRAGMENT_MIN_RADIUS_ATTRIB				CONSTLIT("fragmentMinRadius")
#define FRAGMENT_RADIUS_ATTRIB					CONSTLIT("fragmentRadius")
#define FRAGMENT_TARGET_ATTRIB					CONSTLIT("fragmentTarget")
#define HIT_EFFECT_ATTRIB						CONSTLIT("hitEffect")
#define HIT_POINTS_ATTRIB						CONSTLIT("hitPoints")
#define IDLE_POWER_USE_ATTRIB					CONSTLIT("idlePowerUse")
#define INITIAL_DELAY_ATTRIB					CONSTLIT("initialDelay")
#define INTERACTION_ATTRIB						CONSTLIT("interaction")
#define EXHAUST_LIFETIME_ATTRIB					CONSTLIT("lifetime")
#define LIFETIME_ATTRIB							CONSTLIT("lifetime")
#define MANEUVERABILITY_ATTRIB					CONSTLIT("maneuverability")
#define MANEUVER_RATE_ATTRIB					CONSTLIT("maneuverRate")
#define MAX_MISSILE_SPEED_ATTRIB				CONSTLIT("maxMissileSpeed")
#define MAX_RADIUS_ATTRIB						CONSTLIT("maxRadius")
#define MIN_DAMAGE_ATTRIB						CONSTLIT("minDamage")
#define MIN_RADIUS_ATTRIB						CONSTLIT("minRadius")
#define MISSILE_SPEED_ATTRIB					CONSTLIT("missileSpeed")
#define MULTI_TARGET_ATTRIB						CONSTLIT("multiTarget")
#define NO_DETONATION_ON_END_OF_LIFE_ATTRIB		CONSTLIT("noDetonationOnEndOfLife")
#define NO_FRIENDLY_FIRE_ATTRIB					CONSTLIT("noFriendlyFire")
#define NO_IMMOBILE_HITS_ATTRIB					CONSTLIT("noImmobileHits")
#define NO_IMMUTABLE_HITS_ATTRIB				CONSTLIT("noImmutableHits")
#define NO_MINING_HINT_ATTRIB					CONSTLIT("noMiningHint")
#define NO_SHIP_HITS_ATTRIB						CONSTLIT("noShipHits")
#define NO_STATION_HITS_ATTRIB					CONSTLIT("noStationHits")
#define NO_WMD_HINT_ATTRIB						CONSTLIT("noWMDHint")
#define NO_WORLD_HITS_ATTRIB					CONSTLIT("noWorldHits")
#define PARTICLE_COUNT_ATTRIB					CONSTLIT("particleCount")
#define PARTICLE_EMIT_TIME_ATTRIB				CONSTLIT("particleEmitTime")
#define PARTICLE_MISS_CHANCE_ATTRIB				CONSTLIT("particleMissChance")
#define PARTICLE_SPLASH_CHANCE_ATTRIB			CONSTLIT("particleSplashChance")
#define PARTICLE_SPREAD_ANGLE_ATTRIB			CONSTLIT("particleSpreadAngle")
#define PARTICLE_SPREAD_WIDTH_ATTRIB			CONSTLIT("particleSpreadWidth")
#define PASSTHROUGH_ATTRIB						CONSTLIT("passthrough")
#define POWER_USE_ATTRIB						CONSTLIT("powerUse")
#define RANGE_ATTRIB							CONSTLIT("range")
#define RELATIVISTIC_SPEED_ATTRIB				CONSTLIT("relativisticSpeed")
#define BEAM_CONTINUOUS_ATTRIB					CONSTLIT("repeating")
#define CONTINUOUS_FIRE_DELAY_ATTRIB			CONSTLIT("repeatingDelay")
#define SOUND_ATTRIB							CONSTLIT("sound")
#define SPEED_ATTRIB							CONSTLIT("speed")
#define STEALTH_ATTRIB							CONSTLIT("stealth")
#define TARGET_REQUIRED_ATTRIB					CONSTLIT("targetRequired")
#define TARGETABLE_ATTRIB						CONSTLIT("targetable")
#define TRAIL_ATTRIB							CONSTLIT("trail")
#define FIRE_TYPE_ATTRIB						CONSTLIT("type")
#define VAPOR_TRAIL_ATTRIB						CONSTLIT("vaporTrail")
#define VAPOR_TRAIL_COLOR_ATTRIB				CONSTLIT("vaporTrailColor")
#define VAPOR_TRAIL_LENGTH_ATTRIB				CONSTLIT("vaporTrailLength")
#define VAPOR_TRAIL_WIDTH_ATTRIB				CONSTLIT("vaporTrailWidth")
#define VAPOR_TRAIL_WIDTH_INC_ATTRIB			CONSTLIT("vaporTrailWidthInc")

#define FIELD_PARTICLE_COUNT					CONSTLIT("particleCount")
#define FIELD_SOUND								CONSTLIT("sound")

#define FIRE_TYPE_AREA							CONSTLIT("area")
#define FIRE_TYPE_BEAM							CONSTLIT("beam")
#define FIRE_TYPE_CONTINUOUS_BEAM				CONSTLIT("continuousBeam")
#define FIRE_TYPE_MISSILE						CONSTLIT("missile")
#define FIRE_TYPE_PARTICLES						CONSTLIT("particles")
#define FIRE_TYPE_RADIUS						CONSTLIT("radius")
#define FIRE_TYPE_SHOCKWAVE						CONSTLIT("shockwave")

#define ON_CREATE_SHOT_EVENT					CONSTLIT("OnCreateShot")
#define ON_DAMAGE_OVERLAY_EVENT					CONSTLIT("OnDamageOverlay")
#define ON_DAMAGE_SHIELDS_EVENT					CONSTLIT("OnDamageShields")
#define ON_DAMAGE_ARMOR_EVENT					CONSTLIT("OnDamageArmor")
#define ON_DAMAGE_ABANDONED_EVENT				CONSTLIT("OnDamageAbandoned")
#define ON_DESTROY_OBJ_EVENT					CONSTLIT("OnDestroyObj")
#define ON_DESTROY_SHOT_EVENT					CONSTLIT("OnDestroyShot")
#define ON_FRAGMENT_EVENT						CONSTLIT("OnFragment")

#define PROPERTY_DAMAGE_DESC_AT_PREFIX			CONSTLIT("damageDescAt:")
#define PROPERTY_INTERACTION					CONSTLIT("interaction")
#define PROPERTY_LIFETIME						CONSTLIT("lifetime")
#define PROPERTY_MINING_TYPE					CONSTLIT("miningType")
#define PROPERTY_SHOTS_PER_AMMO_ITEM			CONSTLIT("shotsPerAmmoItem")
#define PROPERTY_STD_HP							CONSTLIT("stdHP")
#define PROPERTY_STD_INTERACTION				CONSTLIT("stdInteraction")
#define PROPERTY_TRACKING						CONSTLIT("tracking")

#define INTERACTION_ALWAYS						CONSTLIT("always")

#define STR_SHIELD_REFLECT						CONSTLIT("reflect")

const CWeaponFireDesc CWeaponFireDesc::m_Null;
CWeaponFireDesc::SOldEffects CWeaponFireDesc::m_NullOldEffects;

static const char *CACHED_EVENTS[CWeaponFireDesc::evtCount] =
	{
		"OnCreateShot",
		"OnDamageAbandoned",
		"OnDamageArmor",
		"OnDamageOverlay",
		"OnDamageShields",
		"OnDestroyObj",
		"OnDestroyShot",
		"OnFragment",
	};

CWeaponFireDesc::~CWeaponFireDesc (void)

//	CWeaponFireDesc destructor

	{
	SFragmentDesc *pNext = m_pFirstFragment;
	while (pNext)
		{
		SFragmentDesc *pDelete = pNext;
		pNext = pNext->pNext;
		delete pDelete->pDesc;
		delete pDelete;
		}

	if (m_pParticleDesc)
		delete m_pParticleDesc;

	if (m_pOldEffects)
		delete m_pOldEffects;

	if (m_pScalable)
		delete[] m_pScalable;
	}

void CWeaponFireDesc::AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed)

//	AddTypesUsed
//
//	Adds types used by this class

	{
	retTypesUsed->SetAt(m_pAmmoType.GetUNID(), true);
	retTypesUsed->SetAt(GetImage().GetBitmapUNID(), true);
	retTypesUsed->SetAt(GetExhaust().ExhaustImage.GetBitmapUNID(), true);
	retTypesUsed->SetAt(m_pEffect.GetUNID(), true);
	retTypesUsed->SetAt(m_pHitEffect.GetUNID(), true);
	retTypesUsed->SetAt(m_pFireEffect.GetUNID(), true);
	retTypesUsed->SetAt(m_pExplosionType.GetUNID(), true);

	if (m_pParticleDesc)
		m_pParticleDesc->AddTypesUsed(retTypesUsed);

	SFragmentDesc *pNext = m_pFirstFragment;
	while (pNext)
		{
		pNext->pDesc->AddTypesUsed(retTypesUsed);

		pNext = pNext->pNext;
		}
	}

void CWeaponFireDesc::ApplyAcceleration (CSpaceObject *pMissile) const

//  ApplyAcceleration
//
//  Accelerates, if necessary. This should be called exactly once per 10 ticks.

	{
	if (m_iAccelerationFactor > 0)
		{
		if (m_iAccelerationFactor < 100
				|| pMissile->GetVel().Length() < m_rMaxMissileSpeed)
			pMissile->SetVel(pMissile->GetVel() * (Metric)(m_iAccelerationFactor / 100.0));
		}
	}

Metric CWeaponFireDesc::CalcDamage (DWORD dwDamageFlags) const

//	CalcDamage
//
//	Computes the total average damage for this shot.

	{
	//	If we have fragments we need to recurse

	if (HasFragments())
		{
		Metric rTotal = 0.0;

		SFragmentDesc *pFragment = GetFirstFragment();
		while (pFragment)
			{
			//	By default, 1/8 of fragments hit, unless the fragments are radius type

			Metric rHitFraction;
			switch (pFragment->pDesc->GetType())
				{
				case ftArea:
				case ftRadius:
					rHitFraction = 1.0;
					break;

				default:
					if (pFragment->pDesc->IsTracking())
						rHitFraction = CWeaponClass::EXPECTED_TRACKING_FRAGMENT_HITS;
					else
						rHitFraction = CWeaponClass::EXPECTED_FRAGMENT_HITS;
					break;
				}

			//  Adjust for passthrough

			if (pFragment->pDesc->GetPassthrough() > 0)
				{
				Metric rPassthroughProb = Min(0.99, pFragment->pDesc->GetPassthrough() / 100.0);
				rHitFraction *= Min(1.0 / (1.0 - rPassthroughProb), CWeaponClass::MAX_EXPECTED_PASSTHROUGH);
				}

			//	Add up values

			rTotal += rHitFraction * pFragment->Count.GetAveValueFloat() * pFragment->pDesc->CalcDamage(dwDamageFlags);

			pFragment = pFragment->pNext;
			}

		return rTotal;
		}
	else
		{
		//	Average damage depends on type

		switch (GetType())
			{
			case ftArea:
				//	Assume 1/8th damage points hit on average
				return CWeaponClass::EXPECTED_SHOCKWAVE_HITS * GetAreaDamageDensityAverage() * GetDamage().GetDamageValue(dwDamageFlags);

			case ftRadius:
				//	Assume average target is far enough away to take half damage
				return CWeaponClass::EXPECTED_RADIUS_DAMAGE * GetDamage().GetDamageValue(dwDamageFlags);

			default:
				return GetDamage().GetDamageValue(dwDamageFlags);
			}
		}
	}

DamageDesc CWeaponFireDesc::CalcDamageDesc (const CItemEnhancementStack *pEnhancements, const CDamageSource &Attacker, Metric rAge) const

//	CalcDamageDesc
//
//	Computes a damage descriptor when we hit something.

	{
	DamageDesc Damage = GetDamage();

	//	If damage changes with age, we alter it here.

	if (!m_DamageAtMaxRange.IsEmpty())
		Damage.InterpolateTo(m_DamageAtMaxRange, rAge);

	if (pEnhancements)
		Damage.AddEnhancements(pEnhancements);

	Damage.SetCause(Attacker.GetCause());
	if (Attacker.IsAutomatedWeapon())
		Damage.SetAutomatedWeapon();

	return Damage;
	}

int CWeaponFireDesc::CalcDefaultHitPoints (void) const

//	CalcDefaultHitPoints
//
//	Computes default hit points.

	{
	int iDefault;

	if ((iDefault = GetUniverse().GetEngineOptions().GetDefaultShotHP()) != -1)
		return iDefault;

	//	Beams never interact

	else if (m_iFireType == ftBeam)
		return 0;

	//	Ammo items get hit points proportional to level and mass.

	else if (m_pAmmoType)
		{
		CItem AmmoItem(m_pAmmoType, 1);
		Metric rStdHP = CWeaponClass::HP_ARMOR_RATIO * CArmorClass::GetStdHP(AmmoItem.GetLevel());
		Metric rMassAdj = AmmoItem.GetMassKg() / CWeaponClass::STD_AMMO_MASS;

		//	Compute how many of these shots are created by one ammo item.

		Metric rShotsPerAmmoItem = CalcShotsPerAmmoItem();
		if (rShotsPerAmmoItem > 0.0)
			rMassAdj /= rShotsPerAmmoItem;

		//	Return hit points

		return mathRound(rMassAdj * rStdHP);
		}

	//	Otherwise, compute based on damage ratio.

	else if (m_iFireType == ftMissile)
		{
		//	Compute the damage of this shot as a ratio of the standard
		//	damage for the level. 

		Metric rStdDamage = CWeaponClass::GetStdDamage(GetLevel());
		Metric rShotDamage = CalcDamage();
		Metric rRatio = rShotDamage / rStdDamage;

		Metric rStdHP = CWeaponClass::HP_ARMOR_RATIO * CArmorClass::GetStdHP(GetLevel());

		return mathRound(CWeaponClass::DEFAULT_HP_DAMAGE_RATIO * rRatio * rStdHP);
		}

	//	No hit points

	else
		return 0;
	}

int CWeaponFireDesc::CalcDefaultInteraction (void) const

//	CalcDefaultInteraction
//
//	Computes the default interaction.

	{
	int iDefault;

	if ((iDefault = GetUniverse().GetEngineOptions().GetDefaultInteraction()) != -1)
		return iDefault;

	else if (m_iFireType == ftBeam || m_iFireType == ftContinuousBeam)
		return 0;

	else if (m_iFireType == ftArea || m_iFireType == ftRadius)
		return -1;

	else if (m_iFireType == ftParticles)
		{
		//	Particle clouds have interaction of 100, decreasing linearly above
		//	0.5c to 0.

		Metric rInteraction = Max(Min(1.0, 2.0 * (1.0 - (GetRatedSpeed() / LIGHT_SPEED))), 0.0);
		return mathRound(100.0 * Max(0.0, Min(rInteraction, 1.0)));
		}

	else if (m_pAmmoType)
		return 100;

	else if (m_iFireType == ftMissile)
		{
		//	Compute the damage of this shot as a ratio of the standard
		//	damage for the level. 

		Metric rStdDamage = CWeaponClass::GetStdDamage(GetLevel());
		Metric rShotDamage = CalcDamage();
		Metric rRatio = rShotDamage / rStdDamage;

		//	Shots above 0.5c (like Flenser) have a maximum interaction, which
		//	linearly decreases from 100 at 0.5c to 0 at 1c.

		Metric rMaxInteraction = Max(Min(1.0, 2.0 * (1.0 - (GetRatedSpeed() / LIGHT_SPEED))), 0.0);

		//	Convert to interaction

		Metric rInteraction = Min(rMaxInteraction,
				pow(rRatio, CWeaponClass::DEFAULT_INTERACTION_EXP)
					- pow(CWeaponClass::DEFAULT_INTERACTION_MIN_RATIO, CWeaponClass::DEFAULT_INTERACTION_EXP));

		return mathRound(100.0 * Max(0.0, Min(rInteraction, 1.0)));
		}

	else
		return 0;
	}

Metric CWeaponFireDesc::CalcMaxEffectiveRange (void) const

//  CalcMaxEffectiveRange
//
//  Computes the max effective range given current stats

	{
	int iMaxLifetime = m_Lifetime.GetMaxValue();

	Metric rRange;
	if (m_iFireType == ftArea)
		rRange = (m_ExpansionSpeed.GetAveValueFloat() * LIGHT_SECOND / 100.0) * Ticks2Seconds(iMaxLifetime) * 0.75;
	else if (m_iFireType == ftRadius)
		rRange = m_rMaxRadius;
	else
		{
		Metric rEffectiveLifetime;
		if (m_iManeuverability > 0)
			rEffectiveLifetime = Ticks2Seconds(iMaxLifetime) * 0.75;
		else
			rEffectiveLifetime = Min(Ticks2Seconds(iMaxLifetime), 200.0);

		Metric rSpeed = (m_rMissileSpeed + m_rMaxMissileSpeed) / 2;
		rRange = rSpeed * rEffectiveLifetime;
		}

	//	If we have fragments, add to the effective range

	if (m_pFirstFragment)
		rRange += m_pFirstFragment->pDesc->m_rMaxEffectiveRange;

	//  Done

	return rRange;
	}

Metric CWeaponFireDesc::CalcShotsPerAmmoItem () const

//	CalcShotsPerAmmoItem
//
//	Returns the number of shots per ammo item. If no ammo, we return 0.0.

	{
	if (!m_pAmmoType)
		return 0.0;

	//	Figure out which weapon launches this shot. We can guarantee that this 
	//	is set because it is initialized in Prebind.

	auto &Weapons = m_pAmmoType->GetLaunchWeapons();
	if (Weapons.GetCount() == 0)
		return 0.0;

	const CWeaponClass *pWeapon = Weapons[0]->AsWeaponClass();
	if (!pWeapon)
		return 0.0;

	//	Compute the multiplier.

	auto &Config = pWeapon->GetConfiguration(*this);
	Metric rShotsPerAmmo = Config.GetMultiplier();

	//	Adjust for repeating
		
	if (int iContinuous = pWeapon->GetContinuous(*this))
		{
		if (!pWeapon->GetContinuousConsumePerShot(*this))
			rShotsPerAmmo *= iContinuous;
		}

	//	If this ammo item is a magazine, then we divide

	if (m_pAmmoType->AreChargesAmmo())
		{
		int iMaxCharges = m_pAmmoType->GetMaxCharges();
		if (iMaxCharges > 0)
			rShotsPerAmmo /= iMaxCharges;
		}

	//	Done

	return rShotsPerAmmo;
	}

Metric CWeaponFireDesc::CalcSpeed (Metric rPercentOfLight, bool bRelativistic)

//	CalcSpeed
//
//	Converts from 0-100 speed to kps. If necessary, account for light-lag.

	{
	Metric rSpeed = rPercentOfLight * LIGHT_SPEED / 100.0;

	//	If specified, we increase speed to simulate light-lag.

	if (bRelativistic)
		rSpeed = CSystem::CalcApparentSpeedAdj(rSpeed) * rSpeed;

	return rSpeed;
	}

bool CWeaponFireDesc::CanHit (const CSpaceObject &Obj) const

//	CanHit
//
//	Returns TRUE if we can hit the given object.

	{
	//	Can we hit worlds?
	
	if (m_fNoWorldHits 
			&& (Obj.GetScale() == scaleWorld || Obj.GetScale() == scaleStar))
		return false;

	//	Can we hit immutables?

	if (m_fNoImmutableHits
			&& Obj.IsImmutable())
		return false;

	//	Can we hit stations

	if (m_fNoStationHits
			&& Obj.GetScale() == scaleStructure)
		return false;

	//	Can we hit immobile objects

	if (m_fNoImmobileHits
			&& !Obj.CanThrust())
		return false;

	//	Can we hit ships?

	if (m_fNoShipHits
			&& Obj.GetScale() == scaleShip)
		return false;

	//	OK

	return true;
	}

IEffectPainter *CWeaponFireDesc::CreateEffectPainter (SShotCreateCtx &CreateCtx)

//	CreateEffectPainter
//
//	Creates an effect to paint the projectile. The caller is responsible for
//	calling Delete on the result.
//
//	NOTE: We may return NULL if the weapon has no effect.

	{
	CCreatePainterCtx PainterCtx;
	PainterCtx.SetWeaponFireDesc(this);
	PainterCtx.SetTrackingObject(IsTracking(CreateCtx));
	PainterCtx.SetUseObjectCenter(true);

	//	If this is an explosion, then we pass the source as an anchor.

	CSpaceObject *pAnchor;
	if ((CreateCtx.dwFlags | SShotCreateCtx::CWF_EXPLOSION)
			&& (pAnchor = CreateCtx.Source.GetObj()))
		{
		PainterCtx.SetAnchor(pAnchor);
		}

	//	We set the default lifetime of the effect to whatever the descriptor defines.

	PainterCtx.SetDefaultParam(LIFETIME_ATTRIB, CEffectParamDesc(m_Lifetime.GetMaxValue()));

	//	Create the painter

	return m_pEffect.CreatePainter(PainterCtx);
	}

void CWeaponFireDesc::CreateFireEffect (CSystem *pSystem, CSpaceObject *pSource, const CVector &vPos, const CVector &vVel, int iDir) const

//	CreateFireEffect
//
//	Creates a fire effect.

	{
	//	If we have a source, then we add the fire effect as an effect on the source.

	if (pSource)
		{
		//	Create a painter.

		CCreatePainterCtx Ctx;
		Ctx.SetWeaponFireDesc(this);

		IEffectPainter *pPainter = m_pFireEffect.CreatePainter(Ctx, &GetUniverse().GetDefaultFireEffect(m_Damage.GetDamageType()));
		if (pPainter == NULL)
			return;

		//	Add the effect

		pSource->AddEffect(pPainter, vPos, 0, iDir);
		}

	//	Otherwise, we add a stand-alone effect

	else
		{
		CEffectCreator *pFireEffect = GetFireEffect();
		if (pFireEffect == NULL)
			return;

		pFireEffect->CreateEffect(pSystem, pSource, vPos, vVel, iDir);
		}
	}

void CWeaponFireDesc::CreateHitEffect (CSystem *pSystem, SDamageCtx &DamageCtx) const

//	CreateHitEffect
//
//	Creates an effect when the weapon hits an object

	{
	if (pSystem == NULL)
		return;

	//	Create the hit effect painter.

	CCreatePainterCtx Ctx;
	Ctx.SetWeaponFireDesc(this);
	Ctx.SetDamageCtx(DamageCtx);

	IEffectPainter *pPainter = m_pHitEffect.CreatePainter(Ctx, &GetUniverse().GetDefaultHitEffect(m_Damage.GetDamageType()));
	if (pPainter == NULL)
		return;

	//	Now create the effect

	CEffect::SCreateOptions Options;
	Options.pAnchor = ((DamageCtx.pObj && !DamageCtx.pObj->IsDestroyed()) ? DamageCtx.pObj : NULL);
	Options.iRotation = DamageCtx.iDirection;

	if (CEffect::Create(*pSystem,
			pPainter,
			DamageCtx.vHitPos,
			(DamageCtx.pObj ? DamageCtx.pObj->GetVel() : CVector()),
			Options) != NOERROR)
		{
		delete pPainter;
		return;
		}
	}

IEffectPainter *CWeaponFireDesc::CreateParticlePainter (void)

//	CreateParticlePainter
//
//	Creates an effect for a particle

	{
	if (m_pParticleDesc == NULL || m_iFireType != ftParticles)
		return NULL;

	CCreatePainterCtx Ctx;
	Ctx.SetWeaponFireDesc(this);

	//	We set the lifetime of the particle to whatever the descriptor defines.

	Ctx.SetDefaultParam(LIFETIME_ATTRIB, CEffectParamDesc(m_pParticleDesc->GetParticleLifetime().GetMaxValue()));

	//	See if the particle descriptor has an effect.

	IEffectPainter *pPainter = m_pParticleDesc->CreateParticlePainter(Ctx);
	if (pPainter)
		return pPainter;

	//	Otherwise, we use the projectile effect (for backwards compatibility)

	return m_pEffect.CreatePainter(Ctx);
	}

IEffectPainter *CWeaponFireDesc::CreateSecondaryPainter (bool bTrackingObj, bool bUseObjectCenter)

//	CreateSecondaryPainter
//
//	Particle damage object use the particle effect to paint each particle, but
//	sometimes they use the main effect as an overall effect. In that case, we 
//	return the effect here.
//
//	NOTE: We may return NULL if the weapon has no effect.

	{
	//	If we DON'T have a particle painter, then we don't have a secondary 
	//	effect (since we're using it for the particle effect).

	if (m_pParticleDesc == NULL || m_pParticleDesc->GetParticleEffect() == NULL)
		return NULL;

	//	Otherwise, create it if we've got it.

	CCreatePainterCtx Ctx;
	Ctx.SetWeaponFireDesc(this);
	Ctx.SetTrackingObject(bTrackingObj);
	Ctx.SetUseObjectCenter(bUseObjectCenter);

	return m_pEffect.CreatePainter(Ctx);
	}

IEffectPainter *CWeaponFireDesc::CreateShockwavePainter (bool bTrackingObj, bool bUseObjectCenter)

//	CreateShockwavePainter
//
//	Creates an effect to paint a shockwave. The caller is responsible for
//	calling Delete on the result.
//
//	NOTE: We may return NULL if the weapon has no effect.

	{
	CCreatePainterCtx Ctx;
	Ctx.SetWeaponFireDesc(this);
	Ctx.SetTrackingObject(bTrackingObj);
	Ctx.SetUseObjectCenter(bUseObjectCenter);
	Ctx.SetNoSingleton(true);

	//	We need to match the expansion speed

	Ctx.SetDefaultParam(SPEED_ATTRIB, CEffectParamDesc(m_ExpansionSpeed.Roll()));

	return m_pEffect.CreatePainter(Ctx);
	}

bool CWeaponFireDesc::FindDataField (const CString &sField, CString *retsValue) const

//	FindDataField
//
//	Returns data field for a weapon fire descriptor

	{
	if (strEquals(sField, FIELD_PARTICLE_COUNT))
		{
		if (m_pParticleDesc)
			*retsValue = strFromInt((int)GetAveParticleCount());
		else
			*retsValue = NULL_STR;
		}
	else if (strEquals(sField, FIELD_SOUND))
		*retsValue = (m_FireSound.GetSound() != -1 ? strFromInt(m_FireSound.GetUNID(), false) : NULL_STR);
	else
		return false;

	return true;
	}

Metric CWeaponFireDesc::GetAveParticleCount (void) const

//	GetAveParticleCount
//
//	If this is a particle effect, then return the average number of particles.

	{
	if (m_pParticleDesc)
		{
		Metric rEmitLifetime = m_pParticleDesc->GetEmitLifetime().GetAveValueFloat();
		if (rEmitLifetime > 0.0)
			return m_pParticleDesc->GetEmitRate().GetAveValueFloat() * rEmitLifetime;
		else
			return m_pParticleDesc->GetEmitRate().GetAveValueFloat();
		}
	else
		return 0.0;
	}

CEffectCreator *CWeaponFireDesc::FindEffectCreator (const CString &sUNID)

//	FindEffectCreator
//
//	Finds effect creator from a partial UNID

	{
	const char *pPos;

	//	Get the appropriate weapon fire desc and the parse position

	CWeaponFireDesc *pDesc = FindWeaponFireDesc(sUNID, &pPos);
	if (pDesc == NULL)
		return NULL;
	
	//	Parse the effect

	if (*pPos == ':')
		{
		pPos++;

		switch (*pPos)
			{
			case 'e':
				return pDesc->m_pEffect;

			case 'h':
				return pDesc->m_pHitEffect;

			case 'f':
				return pDesc->m_pFireEffect;

			case 'p':
				return (pDesc->m_pParticleDesc ? pDesc->m_pParticleDesc->GetParticleEffect() : NULL);

			default:
				return NULL;
			}
		}
	else
		return NULL;
	}

bool CWeaponFireDesc::FindEventHandler (const CString &sEvent, SEventHandlerDesc *retEvent) const

//	FindEventHandler
//
//	Returns an event handler (if found)

	{
	//	Look for an event handler at the weapon fire level

	ICCItem *pCode;
	if (m_Events.FindEvent(sEvent, &pCode))
		{
		if (retEvent)
			{
			retEvent->pExtension = m_pExtension;
			retEvent->sEvent = sEvent;
			retEvent->pCode = pCode;
			}

		return true;
		}

	//	Then look for an event handler at the item level

	CItemType *pDevice;
	CItemType *pAmmo = GetWeaponType(&pDevice);
	if (pAmmo && pAmmo->FindEventHandler(sEvent, retEvent))
		return true;

	if (pDevice && pAmmo != pDevice && pDevice->FindEventHandler(sEvent, retEvent))
		return true;

	//	Otherwise, we have no event

	return false;
	}

ICCItem *CWeaponFireDesc::FindProperty (const CString &sProperty) const

//	FindProperty
//
//	Finds a property. We return NULL if not found.

	{
	CCodeChain &CC = GetUniverse().GetCC();
	ICCItem *pResult;
	CString sValue;
	SpecialDamageTypes iSpecial;

	//	We handle some properties

	if (strStartsWith(sProperty, PROPERTY_DAMAGE_DESC_AT_PREFIX))
		{
		CString sSlider = strSubString(sProperty, PROPERTY_DAMAGE_DESC_AT_PREFIX.GetLength());
		Metric rSlider = strToDouble(sSlider, -1.0);
		if (rSlider < 0.0 || rSlider > 1.0)
			return CC.CreateError(CONSTLIT("Value must be in range 0.0 to 1.0."));

		DamageDesc Result = m_Damage;
		if (!m_DamageAtMaxRange.IsEmpty())
			Result.InterpolateTo(m_DamageAtMaxRange, rSlider);

		return CC.CreateString(Result.AsString());
		}
	else if (strEquals(sProperty, PROPERTY_INTERACTION))
		{
		if (GetInteraction().AlwaysInteracts())
			return CC.CreateString(CONSTLIT("always"));
		else
			return CC.CreateInteger(GetInteraction());
		}

	else if (strEquals(sProperty, PROPERTY_MINING_TYPE))
		{
		if (m_Damage.HasMiningDamage())
			{
			EMiningMethod iMethod = CAsteroidDesc::CalcMiningMethod(*this);
			return CC.CreateString(CAsteroidDesc::MiningMethodID(iMethod));
			}
		else
			return CC.CreateNil();
		}

	else if (strEquals(sProperty, PROPERTY_LIFETIME))
		return CC.CreateNumber(m_Lifetime.GetAveValueFloat());

	else if (strEquals(sProperty, PROPERTY_SHOTS_PER_AMMO_ITEM))
		{
		Metric rShotsPerAmmo = CalcShotsPerAmmoItem();
		if (rShotsPerAmmo > 0.0)
			return CC.CreateNumber(rShotsPerAmmo);
		else
			return CC.CreateNil();
		}

	else if (strEquals(sProperty, PROPERTY_STD_HP))
		return CC.CreateInteger(CalcDefaultHitPoints());

	else if (strEquals(sProperty, PROPERTY_STD_INTERACTION))
		{
		CInteractionLevel Interaction(CalcDefaultInteraction());
		if (Interaction.AlwaysInteracts())
			return CC.CreateString(CONSTLIT("always"));
		else
			return CC.CreateInteger(Interaction);
		}

	else if (strEquals(sProperty, PROPERTY_TRACKING))
		return CC.CreateBool(IsTrackingOrHasTrackingFragments());

	//	See if this is one of the special damage properties

	else if ((iSpecial = DamageDesc::ConvertPropertyToSpecialDamageTypes(sProperty)) != specialNone)
		{
		int iDamage = GetSpecialDamage(iSpecial);
		return (iDamage ? CC.CreateInteger(iDamage) : CC.CreateNil());
		}

	//	Check the damage structure

	else if (pResult = m_Damage.FindProperty(sProperty))
		return pResult;

	//	Otherwise, get it from a data field

	else if (FindDataField(sProperty, &sValue))
		return CreateResultFromDataField(CC, sValue);

	//	Otherwise, not found

	else
		return NULL;
	}

CWeaponFireDesc *CWeaponFireDesc::FindWeaponFireDesc (const CString &sUNID, const char **retpPos)

//	FindWeaponFireDesc
//
//	Finds the weapon fire desc from a partial UNID

	{
	const char *pPos = sUNID.GetASCIIZPointer();

	//	If we're done, then we want this descriptor

	if (*pPos == '\0')
		return this;

	//	If a slash, then we have another weapon fire desc

	else if (*pPos == '/')
		{
		pPos++;

		//	The enhanced section

		if (*pPos == 'e')
			{
			pPos++;
			return NULL;
			}

		//	A fragment

		else if (*pPos == 'f')
			{
			pPos++;

			int iIndex = strParseInt(pPos, 0, &pPos);

			SFragmentDesc *pNext = m_pFirstFragment;
			while (iIndex-- > 0 && pNext)
				pNext = pNext->pNext;

			if (pNext)
				return pNext->pDesc->FindWeaponFireDesc(CString(pPos, -1, TRUE), retpPos);
			else
				return NULL;
			}
		else
			return NULL;
		}

	//	Otherwise, we have an effect

	else if (*pPos == ':')
		{
		if (retpPos)
			*retpPos = pPos;

		return this;
		}
	else
		return NULL;
	}

CWeaponFireDesc *CWeaponFireDesc::FindWeaponFireDescFromFullUNID (const CString &sUNID)

//	FindWeaponFireDesc
//
//	Finds the descriptor by name

	{
	const char *pPos = sUNID.GetPointer();

	//	Get the UNID of the type

	DWORD dwUNID = (DWORD)strParseInt(pPos, 0, &pPos);
	if (dwUNID == 0)
		return NULL;

	//	Get the type

	CDesignType *pType = g_pUniverse->FindDesignType(dwUNID);
	if (pType == NULL)
		return NULL;

	//	If this is an item, then it must be a weapon

	if (pType->GetType() == designItemType)
		{
		CItemType *pItemType = CItemType::AsType(pType);
		if (!pItemType)
			{
			ASSERT(false);
			return NULL;
			}

		CWeaponFireDesc *pMissileDesc;
		CDeviceClass *pDevice;
		
		//	If this is a device, then parse as weapon

		if (pDevice = pItemType->GetDeviceClass())
			{
			CWeaponClass *pClass = pDevice->AsWeaponClass();
			if (pClass == NULL)
				return NULL;

			//	Get the ordinal

			int iOrdinal = 0;
			if (*pPos == '/')
				{
				pPos++;
				iOrdinal = strParseInt(pPos, 0, &pPos);
				}

			//  Convert the ordinal to an ammo type

			CItem Ammo;
			if (iOrdinal < pClass->GetAmmoItemCount())
				Ammo = CItem(pClass->GetAmmoItem(iOrdinal), 1);

			//	Get the weapon fire desc of the ordinal

			CItemCtx ItemCtx;
			CWeaponFireDesc *pDesc = pClass->GetWeaponFireDesc(ItemCtx, Ammo);
			if (pDesc == NULL)
				return NULL;

			//	Continue parsing

			return pDesc->FindWeaponFireDesc(CString(pPos));
			}

		//	Otherwise, see if this is a missile

		else if (pMissileDesc = pItemType->GetMissileDesc())
			return pMissileDesc->FindWeaponFireDesc(CString(pPos));

		//	Nothing

		else
			return NULL;
		}

	//	If this is an effect, then get it from that

	else if (pType->GetType() == designEffectType)
		{
		CEffectCreator *pEffectType = CEffectCreator::AsType(pType);
		ASSERT(pEffectType);

		//	Expect /d

		ASSERT(*pPos == '/');
		pPos++;
		ASSERT(*pPos == 'd');
		pPos++;

		CWeaponFireDesc *pDesc = pEffectType->GetDamageDesc();
		if (pDesc == NULL)
			return NULL;

		//	Continue parsing

		return pDesc->FindWeaponFireDesc(CString(pPos));
		}

	//	Otherwise, we don't know

	else
		return NULL;
	}

void CWeaponFireDesc::FireOnCreateShot (const CDamageSource &Source, CSpaceObject *pShot, CSpaceObject *pTarget)

//	FireOnCreateShot
//
//	Fire OnCreateShot

	{
	SEventHandlerDesc Event;
	if (FindEventHandler(evtOnCreateShot, &Event))
		{
		//	Setup arguments

		CCodeChainCtx CCCtx(GetUniverse());

		CCCtx.DefineContainingType(GetWeaponType());
		CCCtx.SaveAndDefineSourceVar(pShot);
		CCCtx.DefineSpaceObject(CONSTLIT("aTargetObj"), pTarget);
		CCCtx.DefineSpaceObject(CONSTLIT("aAttacker"), Source.GetObj());
		CCCtx.DefineSpaceObject(CONSTLIT("aOrderGiver"), Source.GetOrderGiver());
		CCCtx.DefineItemType(CONSTLIT("aWeaponType"), GetWeaponType());

		ICCItemPtr pResult = CCCtx.RunCode(Event);
		if (pResult->IsError())
			pShot->ReportEventError(ON_CREATE_SHOT_EVENT, pResult);
		}
	}

bool CWeaponFireDesc::FireOnDamageAbandoned (SDamageCtx &Ctx)

//	FireOnDamageAbandoned
//
//	Fire OnDamageAbandoned event. Returns TRUE if we should skip further armor damage

	{
	SEventHandlerDesc Event;
	if (FindEventHandler(evtOnDamageAbandoned, &Event))
		{
		//	Setup arguments

		CCodeChainCtx CCCtx(GetUniverse());

		CCCtx.DefineContainingType(GetWeaponType());
		CCCtx.SaveAndDefineSourceVar(Ctx.pObj);
		CCCtx.DefineDamageCtx(Ctx);

		ICCItemPtr pResult = CCCtx.RunCode(Event);
		if (pResult->IsError())
			Ctx.pObj->ReportEventError(ON_DAMAGE_ABANDONED_EVENT, pResult);

		//	If we return Nil, then we continue processing

		bool bResult;
		if (pResult->IsNil())
			bResult = false;

		//	Otherwise, the result is the damage left

		else
			{
			Ctx.iDamage = pResult->GetIntegerValue();
			bResult = true;
			}

		return bResult;
		}
	else
		return false;
	}

bool CWeaponFireDesc::FireOnDamageArmor (SDamageCtx &Ctx) const

//	FireOnDamageArmor
//
//	Fire OnDamageArmor event. Returns TRUE if we should skip further armor damage

	{
	SEventHandlerDesc Event;
	if (FindEventHandler(evtOnDamageArmor, &Event))
		{
		//	Setup arguments

		CCodeChainCtx CCCtx(GetUniverse());

		CCCtx.DefineContainingType(GetWeaponType());
		CCCtx.SaveAndDefineSourceVar(Ctx.pObj);
		CCCtx.DefineDamageCtx(Ctx);

		ICCItemPtr pResult = CCCtx.RunCode(Event);
		if (pResult->IsError())
			Ctx.pObj->ReportEventError(ON_DAMAGE_ARMOR_EVENT, pResult);

		//	If we return Nil, then we continue processing

		bool bResult;
		if (pResult->IsNil())
			bResult = false;

		//	Otherwise, the result is the damage left

		else
			{
			Ctx.iDamage = pResult->GetIntegerValue();
			bResult = true;
			}

		return bResult;
		}
	else
		return false;
	}

bool CWeaponFireDesc::FireOnDamageOverlay (SDamageCtx &Ctx, COverlay *pOverlay)

//	FireOnDamageOverlay
//
//	Fire OnDamageOverlay event. Returns TRUE if we should skip further overlay damage

	{
	SEventHandlerDesc Event;
	if (FindEventHandler(evtOnDamageOverlay, &Event))
		{
		//	Setup arguments

		CCodeChainCtx CCCtx(GetUniverse());

		CCCtx.DefineContainingType(GetWeaponType());
		CCCtx.SaveAndDefineSourceVar(Ctx.pObj);
		CCCtx.DefineDamageCtx(Ctx);
		CCCtx.DefineInteger(CONSTLIT("aOverlayID"), pOverlay->GetID());

		ICCItemPtr pResult = CCCtx.RunCode(Event);
		if (pResult->IsError())
			Ctx.pObj->ReportEventError(ON_DAMAGE_OVERLAY_EVENT, pResult);

		//	If we return Nil, then we continue processing

		bool bResult;
		if (pResult->IsNil())
			bResult = false;

		//	Otherwise, the result is the damage left

		else
			{
			Ctx.iDamage = pResult->GetIntegerValue();
			bResult = true;
			}

		return bResult;
		}
	else
		return false;
	}

bool CWeaponFireDesc::FireOnDamageShields (SDamageCtx &Ctx, int iDevice)

//	FireOnDamageShields
//
//	Fire OnDamageShields event. Returns TRUE if we should skip further shields damage

	{
	SEventHandlerDesc Event;
	if (FindEventHandler(evtOnDamageShields, &Event))
		{
		//	Setup arguments

		CCodeChainCtx CCCtx(GetUniverse());

		CItemListManipulator ItemList(Ctx.pObj->GetItemList());
		CShip *pShip = Ctx.pObj->AsShip();
		if (pShip)
			pShip->SetCursorAtDevice(ItemList, iDevice);

		CCCtx.DefineContainingType(GetWeaponType());
		CCCtx.SaveAndDefineSourceVar(Ctx.pObj);
		CCCtx.DefineDamageCtx(Ctx);
		CCCtx.DefineInteger(CONSTLIT("aDevice"), iDevice);
		CCCtx.DefineItem(CONSTLIT("aDeviceItem"), ItemList.GetItemAtCursor());

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

		ICCItemPtr pResult = CCCtx.RunCode(Event);

		//	If we return Nil, then we continue processing

		bool bResult;
		if (pResult->IsNil())
			bResult = false;

		//	If this is an integer, we pass damage to armor

		else if (pResult->IsInteger())
			{
			Ctx.iDamage = pResult->GetIntegerValue();
			bResult = true;
			}

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

			//	Proceed with processing

			bResult = false;
			}

		//	If this is the string "reflect" then we reflect

		else if (strEquals(pResult->GetStringValue(), STR_SHIELD_REFLECT))
			{
			Ctx.SetShotReflected(true);
			Ctx.iAbsorb = Ctx.iDamage;
			Ctx.iShieldDamage = 0;
			bResult = false;
			}

		//	Otherwise, error

		else
			{
			Ctx.pObj->ReportEventError(ON_DAMAGE_OVERLAY_EVENT, pResult);
			bResult = true;
			}

		return bResult;
		}
	else
		return false;
	}

void CWeaponFireDesc::FireOnDestroyObj (const SDestroyCtx &Ctx)

//	FireOnDestroyObj
//
//	The weapon destroyed an object.

	{
	SEventHandlerDesc Event;
	if (FindEventHandler(evtOnDestroyObj, &Event))
		{
		//	Setup arguments

		CCodeChainCtx CCCtx(GetUniverse());

		CCCtx.DefineContainingType(GetWeaponType());
		CCCtx.SaveAndDefineSourceVar(Ctx.Attacker.GetObj());
		CCCtx.DefineSpaceObject(CONSTLIT("aObjDestroyed"), Ctx.Obj);
		CCCtx.DefineSpaceObject(CONSTLIT("aDestroyer"), Ctx.Attacker.GetObj());
		CCCtx.DefineSpaceObject(CONSTLIT("aOrderGiver"), Ctx.GetOrderGiver());
		CCCtx.DefineSpaceObject(CONSTLIT("aWreckObj"), Ctx.pWreck);
		CCCtx.DefineBool(CONSTLIT("aDestroy"), Ctx.WasDestroyed());
		CCCtx.DefineString(CONSTLIT("aDestroyReason"), GetDestructionName(Ctx.iCause));

		CCCtx.DefineItemType(CONSTLIT("aWeaponType"), GetWeaponType());
		CCCtx.DefineInteger(CONSTLIT("aWeaponLevel"), GetLevel());

		ICCItemPtr pResult = CCCtx.RunCode(Event);
		if (pResult->IsError())
			Ctx.Obj.ReportEventError(ON_DESTROY_OBJ_EVENT, pResult);
		}
	}

void CWeaponFireDesc::FireOnDestroyShot (CSpaceObject *pShot)

//	FireOnDestroyShot
//
//	Shot is done (either because it hit something or because of lifetime

	{
	SEventHandlerDesc Event;
	if (FindEventHandler(evtOnDestroyShot, &Event))
		{
		//	Setup arguments

		CCodeChainCtx CCCtx(GetUniverse());

		CCCtx.DefineContainingType(GetWeaponType());
		CCCtx.SaveAndDefineSourceVar(pShot);
		CCCtx.DefineItemType(CONSTLIT("aWeaponType"), GetWeaponType());
		CCCtx.DefineSpaceObject(CONSTLIT("aAttacker"), pShot->GetDamageSource().GetObj());
		CCCtx.DefineSpaceObject(CONSTLIT("aOrderGiver"), pShot->GetDamageSource().GetOrderGiver());

		ICCItemPtr pResult = CCCtx.RunCode(Event);
		if (pResult->IsError())
			pShot->ReportEventError(ON_DESTROY_SHOT_EVENT, pResult);
		}
	}

bool CWeaponFireDesc::FireOnFragment (const CDamageSource &Source, CSpaceObject *pShot, const CVector &vHitPos, CSpaceObject *pNearestObj, CSpaceObject *pTarget)

//	FireOnFragment
//
//	Event fires when a shot fragments. If we return TRUE then we skip the default
//	fragmentation event.

	{
	if (!pShot)
		{
		ASSERT(false);
		return false;
		}

	SEventHandlerDesc Event;
	if (FindEventHandler(evtOnFragment, &Event))
		{
		//	Setup arguments

		CCodeChainCtx CCCtx(GetUniverse());

		CCCtx.DefineContainingType(GetWeaponType());
		CCCtx.SaveAndDefineSourceVar(pShot);
		CCCtx.DefineSpaceObject(CONSTLIT("aNearestObj"), pNearestObj);
		CCCtx.DefineSpaceObject(CONSTLIT("aTargetObj"), pTarget);
		CCCtx.DefineVector(CONSTLIT("aHitPos"), vHitPos);
		CCCtx.DefineInteger(CONSTLIT("aHitDir"), (pShot ? pShot->GetRotation() : 0));
		CCCtx.DefineItemType(CONSTLIT("aWeaponType"), GetWeaponType());
		CCCtx.DefineString(CONSTLIT("aWeaponFragment"), m_sUNID);

		CSpaceObject *pAttacker = Source.GetObj();
		CCCtx.DefineSpaceObject(CONSTLIT("aCause"), pShot);
		CCCtx.DefineSpaceObject(CONSTLIT("aAttacker"), pAttacker);
		CCCtx.DefineSpaceObject(CONSTLIT("aOrderGiver"), Source.GetOrderGiver());

		ICCItemPtr pResult = CCCtx.RunCode(Event);
		if (pResult->IsError())
			pShot->ReportEventError(ON_FRAGMENT_EVENT, pResult);

		//	If we return Nil, then we return FALSE, which means continue
		//	processing. Otherwise, we return TRUE, which means skip the default
		//	fragmentation code.

		return !pResult->IsNil();
		}
	else
		return false;
	}

Metric CWeaponFireDesc::GetAveInitialSpeed (void) const

//	GetAveInitialSpeed
//
//	Returns the average initial speed

	{
	if (m_fVariableInitialSpeed)
		return CalcSpeed(m_MissileSpeed.GetAveValueFloat(), m_fRelativisticSpeed);
	else
		return GetRatedSpeed();
	}

DamageTypes CWeaponFireDesc::GetDamageType (void) const

//	GetDamageType
//
//	Returns the damage type (checking for fragments, as necessary).

	{
	//	If the main shot does no damage, then check the fragments.

	DamageTypes iType = m_Damage.GetDamageType();
	if (iType == damageGeneric && m_Damage.GetDamageRange().IsEmpty())
		{
		SFragmentDesc *pNext = m_pFirstFragment;
		while (pNext)
			{
			DamageTypes iFragType = pNext->pDesc->GetDamageType();
			if (iFragType > iType)
				iType = iFragType;

			pNext = pNext->pNext;
			}

		//	Return the best damage we found so far.

		return iType;
		}

	//	Otherwise, we go with the main type

	return iType;
	}

CEffectCreator *CWeaponFireDesc::GetFireEffect (void) const

//	GetFireEffect
//
//	Returns the fire effect creator (or NULL if there is none).
	
	{
	//	If we have a custom fire effect, use that.

	if (m_pFireEffect)
		return m_pFireEffect; 

	//	Otherwise, see if the universe has a default effect for this damage 
	//	type.

	return &GetUniverse().GetDefaultFireEffect(m_Damage.GetDamageType());
	}

Metric CWeaponFireDesc::GetInitialSpeed (void) const

//	GetInitialSpeed
//
//	Returns the initial speed of the missile (when launched)

	{
	if (m_fVariableInitialSpeed)
		return CalcSpeed(m_MissileSpeed.Roll(), m_fRelativisticSpeed);
	else
		return GetRatedSpeed();
	}

int CWeaponFireDesc::GetLevel (void) const

//	GetLevel
//
//	Returns the level of the weapon fire
	
	{
	return Max(m_iLevel, (m_pAmmoType != NULL ? m_pAmmoType->GetLevel() : 0)); 
	}

Metric CWeaponFireDesc::GetMaxRange (void) const

//  GetMaxRange
//
//  Returns the maximum possible range.

	{
	Metric rRange;

	//  Compute lifetime

	Metric rMaxLifetime = Ticks2Seconds(m_Lifetime.GetMaxValue());

	//	Compute max effective range

	if (m_iFireType == ftArea)
		rRange = (m_ExpansionSpeed.GetAveValueFloat() * LIGHT_SECOND / 100.0) * rMaxLifetime;
	else if (m_iFireType == ftRadius)
		rRange = m_rMaxRadius;
	else
		{
		Metric rSpeed = (m_rMissileSpeed + m_rMaxMissileSpeed) / 2;
		rRange = rSpeed * rMaxLifetime;
		}

	//	If we have fragments, add to the effective range

	if (m_pFirstFragment)
		rRange += m_pFirstFragment->pDesc->GetMaxRange();

	//  Done

	return rRange;
	}

CEffectCreator *CWeaponFireDesc::GetParticleEffect (void) const

//	GetParticleEffect
//
//	Returns the particle effect creator

	{
	//	If we have a particle descriptor with an effect, then return it from
	//	there.

	CEffectCreator *pCreator;
	if (m_pParticleDesc
			&& (pCreator = m_pParticleDesc->GetParticleEffect()))
		return pCreator;

	//	Otherwise, we use the main effect.

	return m_pEffect;
	}

CWeaponFireDesc *CWeaponFireDesc::GetScaledDesc (int iLevel) const

//  GetScaledDesc
//
//  Returns the scalable descriptor for the given level.

	{
	//  If we're not scalable, then we just return our stats

	if (m_pScalable == NULL)
		return const_cast<CWeaponFireDesc *>(this);

	//  If we're at or below our base level, then we just return our stats.

	else if (iLevel <= m_iBaseLevel)
		return const_cast<CWeaponFireDesc *>(this);

	//  Otherwise, return a scaled level (but make sure we're in range).

	else
		return &m_pScalable[Min(iLevel - m_iBaseLevel - 1, m_iScaledLevels - 1)];
	}

int CWeaponFireDesc::GetSpecialDamage (SpecialDamageTypes iSpecial, DWORD dwFlags) const

//  GetSpecialDamage
//
//  Returns the value of the given special damage. If a weapon has fragments,
//  we return the highest level for all fragments.

	{
	int iValue = m_Damage.GetSpecialDamage(iSpecial, dwFlags);

	SFragmentDesc *pFrag = m_pFirstFragment;
	while (pFrag)
		{
		iValue = Max(iValue, pFrag->pDesc->GetSpecialDamage(iSpecial, dwFlags));
		pFrag = pFrag->pNext;
		}

	return iValue;
	}

CItemType *CWeaponFireDesc::GetWeaponType (CItemType **retpLauncher) const

//	GetWeaponType
//
//	Returns the item type that best represents this descriptor. For missiles,
//	we return the missile item UNID; for single-shot weapons (including ammo weapons)
//	we return the weapon item UNID

	{
	const char *pPos = m_sUNID.GetPointer();

	//	Get the weapon UNID and the ordinal

	DWORD dwUNID = (DWORD)strParseInt(pPos, 0, &pPos);

	//	Get the type

	CItemType *pItemType = GetUniverse().FindItemType(dwUNID);
	if (pItemType == NULL)
		return NULL;

	//	If this is a device, we expect it to be a weapon

	CWeaponFireDesc *pMissileDesc;
	CDeviceClass *pDeviceClass = pItemType->GetDeviceClass();
	CWeaponClass *pClass = (pDeviceClass ? pDeviceClass->AsWeaponClass() : NULL);

	if (pClass)
		{
		ASSERT(*pPos == '/');
		pPos++;
		int iOrdinal = strParseInt(pPos, 0, &pPos);

		//	Return the device/launcher

		if (retpLauncher)
			*retpLauncher = pItemType;

		//	For launchers, figure out which missile this is

		if (pClass->GetCategory() == itemcatLauncher)
			{
			if (iOrdinal < pClass->GetAmmoItemCount())
				return pClass->GetAmmoItem(iOrdinal);

			//	Otherwise return the launcher (e.g., DM600)

			else
				return pItemType;
			}

		//	Otherwise, return the weapon

		else
			return pItemType;
		}

	//	Otherwise, we expect this to be a missile

	else if (pMissileDesc = pItemType->GetMissileDesc())
		{
		if (retpLauncher)
			{
			CDeviceClass *pLauncher = pItemType->GetAmmoLauncher();
			if (pLauncher)
				*retpLauncher = pLauncher->GetItemType();
			else
				//	Launcher not found
				*retpLauncher = NULL;
			}

		return pItemType;
		}

	//	Otherwise, nothing

	else
		return NULL;
	}

bool CWeaponFireDesc::HasFragmentInterval (int *retiInterval) const

//	HasFragmentInterval
//
//	Returns TRUE if the shot fragments more than once. If so, returns the 
//	interval to the next fragmentation.

	{
	if (m_FragInterval.IsEmpty())
		return false;

	if (retiInterval)
		*retiInterval = m_FragInterval.Roll();

	return true;
	}

void CWeaponFireDesc::InitFromDamage (const DamageDesc &Damage)

//	InitFromDamage
//
//	Conses up a new CWeaponFireDesc from just a damage structure.
//  LATER: This should take a level parameter.

	{
	int i;

	m_iLevel = 13;
	m_fFragment = false;

	//	Load basic attributes

	m_sUNID = NULL_STR;
	m_Lifetime.SetConstant(1);
	m_fCanDamageSource = false;
	m_fAutoTarget = false;
	m_fTargetRequired = false;
	m_fMIRV = false;
	m_fNoMiningHint = false;
	m_fNoWMDHint = false;
	m_fNoDetonationOnEndOfLife = false;
	m_InitialDelay.SetConstant(0);

	//	Hit criteria

	m_fNoFriendlyFire = false;
	m_fNoWorldHits = false;
	m_fNoImmutableHits = false;
	m_fNoStationHits = false;
	m_fNoImmobileHits = false;
	m_fNoShipHits = false;
	m_fTargetable = false;

	//	Load missile speed

	m_fRelativisticSpeed = false;
	m_fVariableInitialSpeed = false;
	m_MissileSpeed.SetConstant(100);
	m_rMissileSpeed = LIGHT_SPEED;

	//	Effects

	m_pEffect.Set(NULL);
	m_pParticleDesc = NULL;

	//	Load stealth

	m_iStealthFromArmor = CSpaceObject::stealthNormal;

	//	Load specific properties

	m_iFireType = ftMissile;
	m_fDirectional = false;
	m_iManeuverability = 0;
	m_iManeuverRate = -1;
	m_iAccelerationFactor = 0;
	m_rMaxMissileSpeed = m_rMissileSpeed;

	//	Hit points and interaction

	m_iHitPoints = 0;
	m_Interaction = CInteractionLevel();
	m_fDefaultHitPoints = false;
	m_fDefaultInteraction = false;

	//	We initialize this with the UNID, and later resolve the reference
	//	during OnDesignLoadComplete

	m_pAmmoType = NULL;

	m_iContinuous = -1;
	m_iContinuousFireDelay = -1;
	m_iPassthrough = 0;
	m_iFireRate = -1;

	//	Load damage

	m_Damage = Damage;
	m_DamageAtMaxRange = DamageDesc();

	//	Fragments

	m_pFirstFragment = NULL;
	m_fProximityBlast = false;
	m_iProximityFailsafe = 0;
	m_rMaxFragThreshold = LIGHT_SECOND * CWeaponClass::DEFAULT_FRAG_THRESHOLD;
	m_rMinFragThreshold = 0.0;
	m_FragInterval.SetConstant(0);

	//	Effects

	m_pHitEffect.Set(NULL);
	m_pFireEffect.Set(NULL);

	//	Old effects

	m_pOldEffects = NULL;

	//	Sound

	m_FireSound = CSoundRef();

	//	Compute max effective range

	m_rMaxEffectiveRange = CalcMaxEffectiveRange();

	//	Cached events

	for (i = 0; i < evtCount; i++)
		{
		m_CachedEvents[i].pExtension = NULL;
		m_CachedEvents[i].pCode = NULL;
		}
	}

ALERROR CWeaponFireDesc::InitFromMissileXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pMissile, const SInitOptions &Options)

//  InitFromMissileXML
//
//  This weapon definition is for a missile

	{
	ALERROR error;

	ASSERT(Options.iLevel == pMissile->GetLevel());

	if (error = InitFromXML(Ctx, pDesc, Options))
		return error;

	m_pAmmoType = pMissile;

	return NOERROR;
	}

ALERROR CWeaponFireDesc::InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const SInitOptions &Options)

//	InitFromXML
//
//	Loads shot data from an element

	{
	ALERROR error;
	int i;

	m_pExtension = Ctx.pExtension;
	m_iLevel = Max(1, Options.iLevel);
	m_fFragment = Options.bIsFragment;

	//	Fire type

	CString sValue = pDesc->GetAttribute(FIRE_TYPE_ATTRIB);
	if (!LoadFireType(sValue, &m_iFireType))
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Invalid weapon fire type: %s"), sValue);
		return ERR_FAIL;
		}

	//	Load basic attributes

	m_sUNID = Options.sUNID;
	m_fCanDamageSource = pDesc->GetAttributeBool(CAN_HIT_SOURCE_ATTRIB);
	m_fAutoTarget = pDesc->GetAttributeBool(AUTO_TARGET_ATTRIB);
	m_fTargetRequired = pDesc->GetAttributeBool(TARGET_REQUIRED_ATTRIB);
	m_InitialDelay.LoadFromXML(pDesc->GetAttribute(INITIAL_DELAY_ATTRIB));

	//	Configuration

	if (ALERROR error = m_Configuration.InitFromWeaponClassXML(Ctx, *pDesc, CConfigurationDesc::ctUnknown))
		return error;

	//	Fire rate

	int iFireRateSecs = pDesc->GetAttributeIntegerBounded(FIRE_RATE_ATTRIB, 0, -1, -1);
	if (iFireRateSecs == -1)
		m_iFireRate = -1;
	else
		m_iFireRate = mathRound(iFireRateSecs / STD_SECONDS_PER_UPDATE);

	//	Power use

	m_iPowerUse = pDesc->GetAttributeIntegerBounded(POWER_USE_ATTRIB, 0, -1, -1);
	m_iIdlePowerUse = pDesc->GetAttributeIntegerBounded(IDLE_POWER_USE_ATTRIB, 0, -1, (m_iPowerUse == -1 ? -1 : m_iPowerUse / 10));

	//	Hit criteria

	m_fNoFriendlyFire = pDesc->GetAttributeBool(NO_FRIENDLY_FIRE_ATTRIB);
	m_fNoWorldHits = pDesc->GetAttributeBool(NO_WORLD_HITS_ATTRIB);
	m_fNoImmutableHits = pDesc->GetAttributeBool(NO_IMMUTABLE_HITS_ATTRIB);
	m_fNoStationHits = pDesc->GetAttributeBool(NO_STATION_HITS_ATTRIB);
	m_fNoImmobileHits = pDesc->GetAttributeBool(NO_IMMOBILE_HITS_ATTRIB);
	m_fNoShipHits = pDesc->GetAttributeBool(NO_SHIP_HITS_ATTRIB);

	//	Load missile speed

	InitMissileSpeed(Ctx, *pDesc);

	//	Initialize lifetime either from an explicit parameter or from range.

	if (!InitLifetime(Ctx, *pDesc))
		return ERR_FAIL;

	//	Load the effect to use. By default we expect images to loop (since they need to last
	//  as long as the missile).

	Ctx.bLoopImages = true;

	error = m_pEffect.LoadEffect(Ctx,
		strPatternSubst("%s:e", m_sUNID),
		pDesc->GetContentElementByTag(EFFECT_TAG),
		pDesc->GetAttribute(EFFECT_ATTRIB));

	Ctx.bLoopImages = false;

	if (error)
		return error;

	//	Load stealth

	m_iStealthFromArmor = pDesc->GetAttributeIntegerBounded(STEALTH_ATTRIB, CSpaceObject::stealthMin, CSpaceObject::stealthMax, CSpaceObject::stealthNormal);

	//	Initialize interaction and hit points

	if (!InitHitPoints(Ctx, *pDesc))
		return ERR_FAIL;

	//	Load specific properties

	switch (m_iFireType)
		{
		case ftBeam:
		case ftMissile:
			{
			//	For backwards compatibility, if we don't have an effect, assume
			//	a beam effect.

			if (m_iFireType == ftBeam && m_pEffect.IsEmpty())
				{
				if (error = m_pEffect.CreateBeamEffect(Ctx, pDesc, strPatternSubst("%s:e", m_sUNID)))
					return error;
				}

			//	Load the image for the missile

			CXMLElement *pImage = pDesc->GetContentElementByTag(IMAGE_TAG);
			if (pImage)
				if (error = SetOldEffects().Image.InitFromXML(Ctx, *pImage))
					return error;

			m_fDirectional = pDesc->GetAttributeBool(DIRECTIONAL_ATTRIB);
			if (m_fDirectional && m_pEffect)
				m_pEffect->SetVariants(g_RotationRange);

			//	Load exhaust data

			CXMLElement *pExhaust = pDesc->GetContentElementByTag(MISSILE_EXHAUST_TAG);
			if (pExhaust)
				{
				SExhaustDesc &Exhaust = SetOldEffects().Exhaust;
				Exhaust.iExhaustRate = pExhaust->GetAttributeInteger(EXHAUST_RATE_ATTRIB);
				Exhaust.iExhaustLifetime = pExhaust->GetAttributeInteger(EXHAUST_LIFETIME_ATTRIB);
				Exhaust.rExhaustDrag = pExhaust->GetAttributeInteger(EXHAUST_DRAG_ATTRIB) / 100.0;

				CXMLElement *pImage = pExhaust->GetContentElementByTag(IMAGE_TAG);
				if (error = Exhaust.ExhaustImage.InitFromXML(Ctx, *pImage))
					return error;
				}

			break;
			}

		case ftArea:
			{
			//	Load expansion speed

			CString sData;
			if (pDesc->FindAttribute(EXPANSION_SPEED_ATTRIB, &sData))
				{
				if (error = m_ExpansionSpeed.LoadFromXML(sData))
					{
					Ctx.sError = CONSTLIT("Invalid expansionSpeed attribute");
					return ERR_FAIL;
					}
				}
			else
				m_ExpansionSpeed.SetConstant(20);

			//	Area damage density

			if (pDesc->FindAttribute(AREA_DAMAGE_DENSITY_ATTRIB, &sData))
				{
				if (error = m_AreaDamageDensity.LoadFromXML(sData))
					{
					Ctx.sError = CONSTLIT("Invalid areaDamageDensity attribute");
					return ERR_FAIL;
					}
				}
			else
				m_AreaDamageDensity.SetConstant(32);

			//	Must have effect

			if (m_pEffect == NULL)
				{
				Ctx.sError = CONSTLIT("Must have <Effect> for area damage.");
				return ERR_FAIL;
				}
			break;
			}

		case ftContinuousBeam:
			break;

		case ftParticles:
			{
			//	Initialize a particle system descriptor

			m_pParticleDesc = new CParticleSystemDesc;

			//	Look for a <ParticleSystem> definition. If we have it, then we let
			//	it initialize from there.

			CXMLElement *pParticleSystem = pDesc->GetContentElementByTag(PARTICLE_SYSTEM_TAG);
			if (pParticleSystem)
				{
				if (error = m_pParticleDesc->InitFromWeaponDescXML(Ctx, pParticleSystem, m_sUNID))
					return error;

				//	We take certain values from the particle system.

				m_Lifetime.SetConstant(m_pParticleDesc->GetParticleLifetime().GetMaxValue() + (m_pParticleDesc->GetEmitLifetime().GetMaxValue() - 1));

				m_MissileSpeed = m_pParticleDesc->GetEmitSpeed();
				m_fVariableInitialSpeed = !m_MissileSpeed.IsConstant();
				m_fRelativisticSpeed = false;
				m_rMissileSpeed = CalcSpeed(m_MissileSpeed.GetAveValueFloat(), m_fRelativisticSpeed);
				m_rMaxMissileSpeed = m_rMissileSpeed;
				}

			//	Otherwise, we initialize from our root (in backwards compatible mode).

			else
				{
				if (error = m_pParticleDesc->InitFromWeaponDescXMLCompatible(Ctx, pDesc))
					return error;

				//	In this case we honor settings from pDesc, since we're in 
				//	backwards compatible mode. [In the normal case we expect these
				//	settings to be in the <ParticleSystem> element.]

				m_pParticleDesc->SetEmitSpeed(m_MissileSpeed);
				m_pParticleDesc->SetParticleLifetime(m_Lifetime);

				//	We always set the old compatibility behavior.

				m_pParticleDesc->SetSprayCompatible();
				}

			//	Minimum damage

			if (error = m_MinDamage.LoadFromXML(pDesc->GetAttribute(MIN_DAMAGE_ATTRIB)))
				{
				Ctx.sError = CONSTLIT("Invalid minDamage attribute");
				return ERR_FAIL;
				}

			break;
			}

		case ftRadius:
			{
			m_rMinRadius = LIGHT_SECOND * (Metric)pDesc->GetAttributeInteger(MIN_RADIUS_ATTRIB);
			m_rMaxRadius = LIGHT_SECOND * (Metric)pDesc->GetAttributeInteger(MAX_RADIUS_ATTRIB);
			break;
			}

		default:
			ASSERT(false);
			return ERR_FAIL;
		}

	//	The effect should have the same lifetime as the shot
	//	Note: For radius damage it is the other way around (we set iMaxLifetime based on
	//	the effect)

	if (m_pEffect
			&& m_iFireType != ftRadius)
		m_pEffect->SetLifetime(m_Lifetime.GetMaxValue());

	//	We initialize this with the UNID, and later resolve the reference
	//	during OnDesignLoadComplete

	if (error = m_pAmmoType.LoadUNID(Ctx, pDesc->GetAttribute(AMMO_ID_ATTRIB)))
		return error;

	//	Maneuverability

	m_iManeuverability = pDesc->GetAttributeInteger(MANEUVERABILITY_ATTRIB);
	m_iManeuverRate = pDesc->GetAttributeIntegerBounded(MANEUVER_RATE_ATTRIB, 1, 180, -1);
	if (m_iManeuverRate == -1 && m_iManeuverability > 0)
		m_iManeuverRate = g_RotationAngle;
	else if (m_iManeuverability == 0 && m_iManeuverRate > 0)
		m_iManeuverability = 1;

	//	Various options

	m_fMIRV = pDesc->GetAttributeBool(MULTI_TARGET_ATTRIB);
	m_fNoMiningHint = pDesc->GetAttributeBool(NO_MINING_HINT_ATTRIB);
	m_fNoWMDHint = pDesc->GetAttributeBool(NO_WMD_HINT_ATTRIB);
	m_fNoDetonationOnEndOfLife = pDesc->GetAttributeBool(NO_DETONATION_ON_END_OF_LIFE_ATTRIB);

	//	Load continuous and passthrough

	m_iContinuous = pDesc->GetAttributeIntegerBounded(BEAM_CONTINUOUS_ATTRIB, 0, -1, -1);
	m_iContinuousFireDelay = pDesc->GetAttributeIntegerBounded(CONTINUOUS_FIRE_DELAY_ATTRIB, 0, -1, -1);

	if (pDesc->FindAttributeInteger(PASSTHROUGH_ATTRIB, &m_iPassthrough))
		{
		//	In previous versions passthrough was a boolean value, so for backwards
		//	compatibility we treat 0 as 50%.
		//
		//	Note: We don't do this for ftArea because we need a way to specify
		//	passthrough=0 (since ftArea defaults to non-zero passthrough). Also,
		//	ftArea has no backwards compatibility issues (passthrough is only
		//	supported for 1.1 and above).

		if (m_iPassthrough == 0 && m_iFireType != ftArea && Ctx.GetAPIVersion() < 3)
			m_iPassthrough = 50;
		}
	else
		{
		//	If this is an area weapon, we set passthrough to a default value
		//	(for backwards compatibility)

		if (m_iFireType == ftArea)
			m_iPassthrough = 80;
		else
			m_iPassthrough = 0;
		}

	//	Load damage

	if (error = m_Damage.LoadFromXML(Ctx, pDesc->GetAttribute(DAMAGE_ATTRIB)))
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Invalid damage specification: %s"), pDesc->GetAttribute(DAMAGE_ATTRIB));
		return error;
		}

	CString sDamageAtMaxRange;
	if (pDesc->FindAttribute(DAMAGE_AT_MAX_RANGE_ATTRIB, &sDamageAtMaxRange))
		{
		if (error = m_DamageAtMaxRange.LoadFromXML(Ctx, sDamageAtMaxRange))
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Invalid damage specification: %s"), pDesc->GetAttribute(DAMAGE_AT_MAX_RANGE_ATTRIB));
			return error;
			}
		}

	//	Fragments

	m_pFirstFragment = NULL;
	SFragmentDesc *pLastFragment = NULL;
	int iFragCount = 0;
	for (i = 0; i < pDesc->GetContentElementCount(); i++)
		{
		CXMLElement *pFragDesc = pDesc->GetContentElement(i);
		if (!strEquals(FRAGMENT_TAG, pFragDesc->GetTag()))
			continue;

		//	Create a new fragmentation descriptor

		SFragmentDesc *pNewDesc = new SFragmentDesc;
		pNewDesc->pNext = NULL;
		if (pLastFragment)
			pLastFragment->pNext = pNewDesc;
		else
			m_pFirstFragment = pNewDesc;

		pLastFragment = pNewDesc;

		//	Load fragment data

		SInitOptions FragOptions;
		FragOptions.sUNID = strPatternSubst("%s/f%d", m_sUNID, iFragCount++);
		FragOptions.iLevel = m_iLevel;
		FragOptions.bIsFragment = true;

		pNewDesc->pDesc = new CWeaponFireDesc;
		if (error = pNewDesc->pDesc->InitFromXML(Ctx, pFragDesc, FragOptions))
			return error;

		//	Set the fragment count

		CString sCount = pFragDesc->GetAttribute(COUNT_ATTRIB);
		if (sCount.IsBlank())
			sCount = pDesc->GetAttribute(FRAGMENT_COUNT_ATTRIB);
		pNewDesc->Count.LoadFromXML(sCount, 1);

		//	Fragmentation Arc

		pNewDesc->Direction.LoadFromXML(pFragDesc->GetAttribute(DIRECTION_ATTRIB), DEFAULT_FRAGMENT_DIRECTION);
		pNewDesc->FragmentArc.LoadFromXML(pFragDesc->GetAttribute(ARC_ANGLE_ATTRIB), 0);

		//	Set MIRV flag

		if (pDesc->GetAttributeBool(FRAGMENT_TARGET_ATTRIB))
			pNewDesc->pDesc->m_fMIRV = true;
		}

	//	If we've got a fragment interval set, then it means we periodically 
	//	fragment (instead of fragmenting on proximity).

	CString sData;
	if (pDesc->FindAttribute(FRAGMENT_INTERVAL_ATTRIB, &sData))
		{
		if (error = m_FragInterval.LoadFromXML(pDesc->GetAttribute(FRAGMENT_INTERVAL_ATTRIB)))
			{
			Ctx.sError = CONSTLIT("Invalid fragmentInterval attribute");
			return ERR_FAIL;
			}

		m_fProximityBlast = false;
		}

	//	Otherwise, initialize proximity fragmentation values
	//
	//	NOTE: We set these values even if we don't have fragments because we 
	//	might have an OnFragment event.

	else
		{
		m_fProximityBlast = (iFragCount != 0);
		m_iProximityFailsafe = pDesc->GetAttributeInteger(FAILSAFE_ATTRIB);

		Metric rMax = pDesc->GetAttributeDoubleBounded(FRAGMENT_MAX_RADIUS_ATTRIB, 0.0, -1.0, -1.0);
		if (rMax == -1.0)
			rMax = pDesc->GetAttributeDoubleBounded(FRAGMENT_RADIUS_ATTRIB, 0.0, -1.0, -1.0);

		Metric rMin = pDesc->GetAttributeDoubleBounded(FRAGMENT_MIN_RADIUS_ATTRIB, 0.0, -1.0, -1.0);

		//	Set defaults, based on which values are defined.

		if (rMax == -1.0 && rMin == -1.0)
			{
			rMax = CWeaponClass::DEFAULT_FRAG_THRESHOLD;
			rMin = CWeaponClass::DEFAULT_FRAG_MIN_THRESHOLD;
			}
		else if (rMin == -1.0)
			{
			rMin = Min(CWeaponClass::DEFAULT_FRAG_MIN_THRESHOLD, rMax);
			}
		else if (rMax == -1.0)
			{
			rMax = Max(rMin, CWeaponClass::DEFAULT_FRAG_THRESHOLD);
			}

		//	Convert to light-seconds

		m_rMaxFragThreshold = LIGHT_SECOND * rMax;
		m_rMinFragThreshold = LIGHT_SECOND * rMin;
		}

	//	Compute max effective range

	m_rMaxEffectiveRange = CalcMaxEffectiveRange();

	//	Effects

	if (error = m_pHitEffect.LoadEffect(Ctx,
			strPatternSubst("%s:h", m_sUNID),
			pDesc->GetContentElementByTag(HIT_EFFECT_TAG),
			pDesc->GetAttribute(HIT_EFFECT_ATTRIB)))
		return error;

	if (error = m_pFireEffect.LoadEffect(Ctx,
			strPatternSubst("%s:f", m_sUNID),
			pDesc->GetContentElementByTag(FIRE_EFFECT_TAG),
			pDesc->GetAttribute(FIRE_EFFECT_ATTRIB)))
		return error;

	//	Explosion

	if (error = m_pExplosionType.LoadUNID(Ctx, pDesc->GetAttribute(EXPLOSION_TYPE_ATTRIB)))
		return error;

	//	Vapor trail

	int iVaporTrailWidth;
	if (!pDesc->FindAttributeInteger(VAPOR_TRAIL_WIDTH_ATTRIB, &iVaporTrailWidth))
		iVaporTrailWidth = 100 * pDesc->GetAttributeInteger(VAPOR_TRAIL_ATTRIB);

	if (iVaporTrailWidth)
		{
		SVaporTrailDesc &VaporTrail = SetOldEffects().VaporTrail;

		VaporTrail.iVaporTrailWidth = iVaporTrailWidth;
		VaporTrail.rgbVaporTrailColor = LoadRGBColor(pDesc->GetAttribute(VAPOR_TRAIL_COLOR_ATTRIB));
		VaporTrail.iVaporTrailLength = pDesc->GetAttributeInteger(VAPOR_TRAIL_LENGTH_ATTRIB);
		if (VaporTrail.iVaporTrailLength <= 0)
			VaporTrail.iVaporTrailLength = 64;

		if (!pDesc->FindAttributeInteger(VAPOR_TRAIL_WIDTH_INC_ATTRIB, &VaporTrail.iVaporTrailWidthInc))
			VaporTrail.iVaporTrailWidthInc = 25;
		}

	//	Sound

	if (error = m_FireSound.LoadUNID(Ctx, pDesc->GetAttribute(SOUND_ATTRIB)))
		return error;

	//	Events

	CXMLElement *pEventsDesc = pDesc->GetContentElementByTag(EVENTS_TAG);
	if (pEventsDesc)
		{
		if (error = m_Events.InitFromXML(Ctx, pEventsDesc))
			return error;
		}

	//  Targetability

	m_fTargetable = pDesc->GetAttributeBool(TARGETABLE_ATTRIB);

	return NOERROR;
	}

bool CWeaponFireDesc::InitHitPoints (SDesignLoadCtx &Ctx, const CXMLElement &XMLDesc)

//	InitHitPoints
//
//	Initializes:
//
//	m_iInteraction
//	m_iHitPoints
//	m_fDefaultInteraction
//	m_fDefaultHitPoints

	{
	//	Beams and missiles get non-zero hit points.

	if (m_iFireType == ftBeam || m_iFireType == ftMissile)
		{
		m_iHitPoints = XMLDesc.GetAttributeIntegerBounded(HIT_POINTS_ATTRIB, 0, -1, -1);
		m_fDefaultHitPoints = (m_iHitPoints == -1);
		}

	//	Otherwise, none

	else
		{
		m_iHitPoints = 0;
		m_fDefaultHitPoints = false;
		}

	//	Load interaction

	CString sInteraction;
	if (XMLDesc.FindAttribute(INTERACTION_ATTRIB, &sInteraction))
		{
		m_fDefaultInteraction = false;

		if (strEquals(sInteraction, INTERACTION_ALWAYS))
			m_Interaction = -1;
		else
			{
			bool bFailed;
			m_Interaction = strToInt(sInteraction, 0, &bFailed);

			if (bFailed)
				m_fDefaultInteraction = true;
			else if (m_Interaction < 0)
				m_Interaction = 0;
			else if (m_Interaction > 100)
				m_Interaction = 100;
			}
		}
	else
		{
		m_Interaction = 0;
		m_fDefaultInteraction = true;
		}

	return true;
	}

bool CWeaponFireDesc::InitLifetime (SDesignLoadCtx &Ctx, const CXMLElement &XMLDesc)

//	InitLifetime
//
//	Initializes m_Lifetime. We must have already called InitMissileSpeed.

	{
	CString sValue;
	if (XMLDesc.FindAttribute(LIFETIME_ATTRIB, &sValue))
		{
		if (m_Lifetime.LoadFromXML(sValue) != NOERROR)
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Invalid lifetime: %s"), sValue);
			return false;
			}
		}
	else
		{
		Metric rRange = XMLDesc.GetAttributeIntegerBounded(RANGE_ATTRIB, 0, -1, 0) * LIGHT_SECOND;
		if (m_rMaxMissileSpeed > 0.0)
			{
			Metric rLifetimeSeconds = rRange / m_rMaxMissileSpeed;
			m_Lifetime.SetConstant(Max(1, Seconds2Ticks(rLifetimeSeconds)));
			}
		else
			m_Lifetime.SetConstant(0);
		}

	return true;
	}

bool CWeaponFireDesc::InitMissileSpeed (SDesignLoadCtx &Ctx, const CXMLElement &XMLDesc)

//	InitMissileSpeed
//
//	Initializes missile speed from XML. Returns FALSE if we could not initialize
//	for some reason.
//
//	Guarantees initialization of:
//
//	m_MissileSpeed
//	m_rMissileSpeed
//	m_rMaxMissileSpeed
//	m_iAccelerationFactor
//	m_fRelativisticSpeed
//	m_fVariableInitialSpeed

	{
	CString sData;

	if (XMLDesc.FindAttribute(RELATIVISTIC_SPEED_ATTRIB, &sData))
		{
		int iSpeed;

		if (CXMLElement::IsBoolTrueValue(sData))
			{
			m_fRelativisticSpeed = true;
			iSpeed = 100;
			}
		else if ((iSpeed = strToInt(sData, -1)) > 0)
			{
			m_fRelativisticSpeed = true;
			}
		else
			{
			m_fRelativisticSpeed = false;
			iSpeed = 100;
			}

		m_MissileSpeed.SetConstant(iSpeed);
		m_fVariableInitialSpeed = false;
		m_rMissileSpeed = CalcSpeed(iSpeed, m_fRelativisticSpeed);
		}
	else if (XMLDesc.FindAttribute(MISSILE_SPEED_ATTRIB, &sData))
		{
		if (ALERROR error = m_MissileSpeed.LoadFromXML(sData))
			{
			Ctx.sError = CONSTLIT("Invalid missile speed attribute");
			return ERR_FAIL;
			}

		m_fRelativisticSpeed = false;
		m_fVariableInitialSpeed = !m_MissileSpeed.IsConstant();
		m_rMissileSpeed = CalcSpeed(m_MissileSpeed.GetAveValueFloat(), false);
		}
	else
		{
		switch (m_iFireType)
			{
			case ftArea:
			case ftRadius:
				m_MissileSpeed.SetConstant(0);
				m_rMissileSpeed = 0.0;
				break;

			default:
				m_MissileSpeed.SetConstant(100);
				m_rMissileSpeed = CalcSpeed(100.0, false);
				break;
			}

		m_fRelativisticSpeed = false;
		m_fVariableInitialSpeed = false;
		}

	//	See if we accelerate and compute maximum speed.

	m_iAccelerationFactor = XMLDesc.GetAttributeInteger(ACCELERATION_FACTOR_ATTRIB);
	int iMaxSpeed = XMLDesc.GetAttributeInteger(MAX_MISSILE_SPEED_ATTRIB);
	if (iMaxSpeed == 0)
		m_rMaxMissileSpeed = m_rMissileSpeed;
	else
		m_rMaxMissileSpeed = CalcSpeed(iMaxSpeed, m_fRelativisticSpeed);

	return true;
	}

ALERROR CWeaponFireDesc::InitScaledStats (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pItem, CWeaponClass *pWeapon)

//  InitScaledStats
//
//  After initializing from XML, this function generates scaled versions of the
//  weapon, based on any scaling parameters.
//
//  NOTE: We don't call this in InitFromXML because we don't want to recurse
//  for fragments (instead, we deal with them here).

	{
	ALERROR error;
	int i;

	ASSERT(m_pScalable == NULL);

	m_iBaseLevel = pItem->GetLevel();
	m_iScaledLevels = pItem->GetMaxLevel() - m_iBaseLevel;
	if (m_iScaledLevels < 1)
		return NOERROR;

	m_pScalable = new CWeaponFireDesc [m_iScaledLevels];

	//  Loop over every descriptor and initialize it.

	for (i = 0; i < m_iScaledLevels; i++)
		{
		CWeaponFireDesc &ScaledDesc = m_pScalable[i];
		int iScaledLevel = m_iBaseLevel + i + 1;

		if (error = ScaledDesc.InitScaledStats(Ctx, pDesc, pWeapon, *this, m_iBaseLevel, iScaledLevel))
			return error;
		}

	//  Done

	return NOERROR;
	}

ALERROR CWeaponFireDesc::InitScaledStats (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CWeaponClass *pWeapon, const CWeaponFireDesc &Src, int iBaseLevel, int iScaledLevel)

//  InitScaledStats
//
//  Initializes a single level

	{
	ALERROR error;

	const CWeaponClass::SStdStats &Base = CWeaponClass::GetStdStats(iBaseLevel);
	const CWeaponClass::SStdStats &Scaled = CWeaponClass::GetStdStats(iScaledLevel);

	//  Set the level

	m_iLevel = iScaledLevel;

	//  Damage scales. We use the bonus parameter to increase it.
	//  We start by figuring out the percent increase in damage at the scaled
	//  level, relative to base.

	Metric rAdj = (Metric)Scaled.iDamage / Base.iDamage;
	m_Damage = Src.m_Damage;
	m_Damage.ScaleDamage(rAdj);
	if (!Src.m_DamageAtMaxRange.IsEmpty())
		{
		m_DamageAtMaxRange = Src.m_DamageAtMaxRange;
		m_DamageAtMaxRange.ScaleDamage(rAdj);
		}

	//	Power scales proportionally

	rAdj = (Metric)Scaled.iPower / Base.iPower;
	CItemCtx ItemCtx;
	int iBasePowerUse = pWeapon->GetPowerRating(ItemCtx);
	m_iPowerUse = mathRound(iBasePowerUse * rAdj);
	m_iIdlePowerUse = mathRound(pWeapon->GetIdlePowerUse() * rAdj);

	//  These stats never scale, so we just copy them

	m_pExtension = Src.m_pExtension;
	m_sUNID = Src.m_sUNID;      //  NOTE: We don't need a unique UNID 
								//      since we're using the same effects
								//      as the base level.

	m_pAmmoType = Src.m_pAmmoType;
	m_iFireType = Src.m_iFireType;
	m_iContinuous = Src.m_iContinuous;
	m_iContinuousFireDelay = Src.m_iContinuousFireDelay;
	m_iFireRate = Src.m_iFireRate;

	m_fRelativisticSpeed = Src.m_fRelativisticSpeed;
	m_rMissileSpeed = Src.m_rMissileSpeed;
	m_MissileSpeed = Src.m_MissileSpeed;
	m_Lifetime = Src.m_Lifetime;
	m_InitialDelay = Src.m_InitialDelay;
	m_iPassthrough = Src.m_iPassthrough;

	m_iAccelerationFactor = Src.m_iAccelerationFactor;
	m_rMaxMissileSpeed = Src.m_rMaxMissileSpeed;
	m_iStealthFromArmor = Src.m_iStealthFromArmor;
	m_iHitPoints = Src.m_iHitPoints;
	m_Interaction = Src.m_Interaction;
	m_iManeuverability = Src.m_iManeuverability;
	m_iManeuverRate = Src.m_iManeuverRate;

	m_pParticleDesc = (Src.m_pParticleDesc ? new CParticleSystemDesc(*Src.m_pParticleDesc) : NULL);

	m_ExpansionSpeed = Src.m_ExpansionSpeed;
	m_AreaDamageDensity = Src.m_AreaDamageDensity;
		
	m_rMinRadius = Src.m_rMinRadius;
	m_rMaxRadius = Src.m_rMaxRadius;

	m_fVariableInitialSpeed = Src.m_fVariableInitialSpeed;
	m_fNoFriendlyFire = Src.m_fNoFriendlyFire;
	m_fNoWorldHits = Src.m_fNoWorldHits;
	m_fNoImmutableHits = Src.m_fNoImmutableHits;
	m_fNoStationHits = Src.m_fNoStationHits;
	m_fNoImmobileHits = Src.m_fNoImmobileHits;
	m_fNoShipHits = Src.m_fNoShipHits;
	m_fAutoTarget = Src.m_fAutoTarget;
	m_fTargetRequired = Src.m_fTargetRequired;
	m_fCanDamageSource = Src.m_fCanDamageSource;
	m_fDirectional = Src.m_fDirectional;
	m_fFragment = Src.m_fFragment;
	m_fProximityBlast = Src.m_fProximityBlast;
	m_FragInterval = Src.m_FragInterval;
	m_fTargetable = Src.m_fTargetable;
	m_fDefaultInteraction = Src.m_fDefaultInteraction;
	m_fDefaultHitPoints = Src.m_fDefaultHitPoints;
	m_fMIRV = Src.m_fMIRV;
	m_fNoMiningHint = Src.m_fNoMiningHint;
	m_fNoWMDHint = Src.m_fNoWMDHint;
	m_fNoDetonationOnEndOfLife = Src.m_fNoDetonationOnEndOfLife;

	m_pEffect = Src.m_pEffect;
	m_pHitEffect = Src.m_pHitEffect;
	m_pFireEffect = Src.m_pFireEffect;
	m_FireSound = Src.m_FireSound;
	m_pOldEffects = (Src.m_pOldEffects ? new SOldEffects(*Src.m_pOldEffects) : NULL);
	m_pExplosionType = Src.m_pExplosionType;

	m_Events = Src.m_Events;

	//  Handle fragments recursively

	m_pFirstFragment = NULL;
	SFragmentDesc *pLastFragment = NULL;
	int iFragCount = 0;
	SFragmentDesc *pSrcFragment = Src.m_pFirstFragment;
	for (int i = 0; i < pDesc->GetContentElementCount() && pSrcFragment; i++, pSrcFragment = pSrcFragment->pNext)
		{
		CXMLElement *pFragDesc = pDesc->GetContentElement(i);
		if (!strEquals(FRAGMENT_TAG, pFragDesc->GetTag()))
			continue;

		//	Create a new fragmentation descriptor

		SFragmentDesc *pNewDesc = new SFragmentDesc;
		pNewDesc->pNext = NULL;
		if (pLastFragment)
			pLastFragment->pNext = pNewDesc;
		else
			m_pFirstFragment = pNewDesc;

		pLastFragment = pNewDesc;

		//	Load fragment data

		pNewDesc->pDesc = new CWeaponFireDesc;
		if (error = pNewDesc->pDesc->InitScaledStats(Ctx, pFragDesc, pWeapon, *pSrcFragment->pDesc, iBaseLevel, iScaledLevel))
			return error;

		//	Set the fragment count

		pNewDesc->Count = pSrcFragment->Count;
		}

	//  Cached values

	m_rMaxEffectiveRange = CalcMaxEffectiveRange();

	//  Done

	return NOERROR;
	}

bool CWeaponFireDesc::IsTracking (const SShotCreateCtx &Ctx) const

//	IsTracking
//
//	Returns TRUE if the shot we're creating tracks.

	{
	return Ctx.pTarget
			&& (IsTracking()
				|| (Ctx.pEnhancements && Ctx.pEnhancements->IsTracking()));
	}

bool CWeaponFireDesc::IsTrackingOrHasTrackingFragments (void) const

//	IsTrackingOrHasTrackingFragments
//
//	Returns TRUE if either the main shot tracks or if it launches tracking
//	fragments.

	{
	SFragmentDesc *pFragment;

	return (IsTracking()
			|| ((pFragment = GetFirstFragment()) && pFragment->pDesc->IsTrackingOrHasTrackingFragments()));
	}

bool CWeaponFireDesc::LoadFireType (const CString &sValue, FireTypes *retiFireType)

//	LoadFireType
//
//	Parses a fire type string value.

	{
	FireTypes iFireType;

	if (strEquals(sValue, FIRE_TYPE_MISSILE) || sValue.IsBlank())
		iFireType = ftMissile;
	else if (strEquals(sValue, FIRE_TYPE_BEAM))
		iFireType = ftBeam;
	else if (strEquals(sValue, FIRE_TYPE_AREA) || strEquals(sValue, FIRE_TYPE_SHOCKWAVE))
		iFireType = ftArea;
	else if (strEquals(sValue, FIRE_TYPE_CONTINUOUS_BEAM))
		iFireType = ftContinuousBeam;
	else if (strEquals(sValue, FIRE_TYPE_PARTICLES))
		iFireType = ftParticles;
	else if (strEquals(sValue, FIRE_TYPE_RADIUS))
		iFireType = ftRadius;
	else
		return false;

	if (retiFireType)
		*retiFireType = iFireType;

	return true;
	}

void CWeaponFireDesc::MarkImages (void)

//	MarkImages
//
//	Marks images used by weapon

	{
	if (m_pEffect)
		m_pEffect->MarkImages();

	if (m_pHitEffect)
		m_pHitEffect->MarkImages();

	if (m_pFireEffect)
		m_pFireEffect->MarkImages();

	if (m_pExplosionType)
		m_pExplosionType->MarkImages();

	m_FireSound.Mark();

	SFragmentDesc *pFragment = m_pFirstFragment;
	while (pFragment)
		{
		pFragment->pDesc->MarkImages();
		pFragment = pFragment->pNext;
		}

	if (m_pParticleDesc)
		m_pParticleDesc->MarkImages();
	}

ALERROR CWeaponFireDesc::OnDesignLoadComplete (SDesignLoadCtx &Ctx)

//	OnDesignLoadComplete
//
//	Done loading all design elements

	{
	DEBUG_TRY

	ALERROR error;
	int i;

	if (m_pOldEffects)
		{
		if (error = SetOldEffects().Image.OnDesignLoadComplete(Ctx))
			return error;

		if (error = SetOldEffects().Exhaust.ExhaustImage.OnDesignLoadComplete(Ctx))
			return error;
		}

	if (error = m_pAmmoType.Bind(Ctx))
		return error;

	if (error = m_pEffect.Bind(Ctx))
		return error;

	if (error = m_pHitEffect.Bind(Ctx))
		return error;

	if (error = m_pFireEffect.Bind(Ctx))
		return error;

	if (error = m_FireSound.Bind(Ctx))
		return error;

	if (m_pParticleDesc)
		if (error = m_pParticleDesc->Bind(Ctx))
			return error;

	if (error = m_pExplosionType.Bind(Ctx))
		return error;

	if (m_fDefaultHitPoints)
		m_iHitPoints = CalcDefaultHitPoints();

	if (m_fDefaultInteraction)
		m_Interaction = CInteractionLevel(CalcDefaultInteraction());

	//	Fragment

	SFragmentDesc *pNext = m_pFirstFragment;
	while (pNext)
		{
		if (error = pNext->pDesc->OnDesignLoadComplete(Ctx))
			return error;

		pNext = pNext->pNext;
		}

	//  Scaled levels

	if (m_pScalable)
		{
		for (i = 0; i < m_iScaledLevels; i++)
			if (error = m_pScalable[i].OnDesignLoadComplete(Ctx))
				return error;
		}

	//	Load some events for efficiency

	for (i = 0; i < evtCount; i++)
		{
		if (!FindEventHandler(CString(CACHED_EVENTS[i], -1, true), &m_CachedEvents[i]))
			{
			m_CachedEvents[i].pExtension = NULL;
			m_CachedEvents[i].pCode = NULL;
			}
		}

	//	If we have an OnFragment event, then we enable proximity blast
	//	(But only if we don't have periodic fragmentation.)

	if (HasOnFragmentEvent() && m_FragInterval.IsEmpty())
		m_fProximityBlast = true;

	return NOERROR;

	DEBUG_CATCH
	}

bool CWeaponFireDesc::ShowsHint (EDamageHint iHint) const

//	ShowsHint
//
//	Returns TRUE if we should show the given hint.

	{
	switch (iHint)
		{
		case EDamageHint::useMining:
			return !m_fNoMiningHint;

		case EDamageHint::useMiningOrWMD:
			return !m_fNoMiningHint || !m_fNoWMDHint;

		case EDamageHint::useWMD:
		case EDamageHint::useWMDforShip:
			return !m_fNoWMDHint;

		default:
			return true;
		}
	}
