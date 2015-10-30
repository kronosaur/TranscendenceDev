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
#define AREA_DAMAGE_DENSITY_ATTRIB				CONSTLIT("areaDamageDensity")
#define AUTO_TARGET_ATTRIB						CONSTLIT("autoAcquireTarget")
#define CAN_HIT_SOURCE_ATTRIB					CONSTLIT("canHitSource")
#define COUNT_ATTRIB							CONSTLIT("count")
#define EXHAUST_RATE_ATTRIB						CONSTLIT("creationRate")
#define DAMAGE_ATTRIB							CONSTLIT("damage")
#define DIRECTIONAL_ATTRIB						CONSTLIT("directional")
#define EXHAUST_DRAG_ATTRIB						CONSTLIT("drag")
#define EFFECT_ATTRIB							CONSTLIT("effect")
#define EXPANSION_SPEED_ATTRIB					CONSTLIT("expansionSpeed")
#define FAILSAFE_ATTRIB							CONSTLIT("failsafe")
#define FIRE_EFFECT_ATTRIB						CONSTLIT("fireEffect")
#define FRAGMENT_COUNT_ATTRIB					CONSTLIT("fragmentCount")
#define FRAGMENT_TARGET_ATTRIB					CONSTLIT("fragmentTarget")
#define HIT_EFFECT_ATTRIB						CONSTLIT("hitEffect")
#define HIT_POINTS_ATTRIB						CONSTLIT("hitPoints")
#define INITIAL_DELAY_ATTRIB					CONSTLIT("initialDelay")
#define INTERACTION_ATTRIB						CONSTLIT("interaction")
#define EXHAUST_LIFETIME_ATTRIB					CONSTLIT("lifetime")
#define LIFETIME_ATTRIB							CONSTLIT("lifetime")
#define MANEUVERABILITY_ATTRIB					CONSTLIT("maneuverability")
#define MANEUVER_RATE_ATTRIB					CONSTLIT("maneuverRate")
#define MAX_MISSILE_SPEED_ATTRIB				CONSTLIT("maxMissileSpeed")
#define MAX_RADIUS_ATTRIB						CONSTLIT("maxRadius")
#define MIN_RADIUS_ATTRIB						CONSTLIT("minRadius")
#define MISSILE_SPEED_ATTRIB					CONSTLIT("missileSpeed")
#define MULTI_TARGET_ATTRIB						CONSTLIT("multiTarget")
#define NO_FRIENDLY_FIRE_ATTRIB					CONSTLIT("noFriendlyFire")
#define NO_IMMOBILE_HITS_ATTRIB					CONSTLIT("noImmobileHits")
#define NO_IMMUTABLE_HITS_ATTRIB				CONSTLIT("noImmutableHits")
#define NO_SHIP_HITS_ATTRIB						CONSTLIT("noShipHits")
#define NO_STATION_HITS_ATTRIB					CONSTLIT("noStationHits")
#define NO_WORLD_HITS_ATTRIB					CONSTLIT("noWorldHits")
#define PARTICLE_COUNT_ATTRIB					CONSTLIT("particleCount")
#define PARTICLE_EMIT_TIME_ATTRIB				CONSTLIT("particleEmitTime")
#define PARTICLE_MISS_CHANCE_ATTRIB				CONSTLIT("particleMissChance")
#define PARTICLE_SPLASH_CHANCE_ATTRIB			CONSTLIT("particleSplashChance")
#define PARTICLE_SPREAD_ANGLE_ATTRIB			CONSTLIT("particleSpreadAngle")
#define PARTICLE_SPREAD_WIDTH_ATTRIB			CONSTLIT("particleSpreadWidth")
#define PASSTHROUGH_ATTRIB						CONSTLIT("passthrough")
#define BEAM_CONTINUOUS_ATTRIB					CONSTLIT("repeating")
#define SOUND_ATTRIB							CONSTLIT("sound")
#define STEALTH_ATTRIB							CONSTLIT("stealth")
#define TRAIL_ATTRIB							CONSTLIT("trail")
#define FIRE_TYPE_ATTRIB						CONSTLIT("type")
#define VAPOR_TRAIL_ATTRIB						CONSTLIT("vaporTrail")
#define VAPOR_TRAIL_COLOR_ATTRIB				CONSTLIT("vaporTrailColor")
#define VAPOR_TRAIL_LENGTH_ATTRIB				CONSTLIT("vaporTrailLength")
#define VAPOR_TRAIL_WIDTH_ATTRIB				CONSTLIT("vaporTrailWidth")
#define VAPOR_TRAIL_WIDTH_INC_ATTRIB			CONSTLIT("vaporTrailWidthInc")

#define FIELD_PARTICLE_COUNT					CONSTLIT("particleCount")
#define FIELD_SOUND								CONSTLIT("sound")

#define FIRE_TYPE_BEAM							CONSTLIT("beam")
#define FIRE_TYPE_MISSILE						CONSTLIT("missile")
#define FIRE_TYPE_AREA							CONSTLIT("area")
#define FIRE_TYPE_PARTICLES						CONSTLIT("particles")
#define FIRE_TYPE_RADIUS						CONSTLIT("radius")

#define ON_CREATE_SHOT_EVENT					CONSTLIT("OnCreateShot")
#define ON_DAMAGE_OVERLAY_EVENT					CONSTLIT("OnDamageOverlay")
#define ON_DAMAGE_SHIELDS_EVENT					CONSTLIT("OnDamageShields")
#define ON_DAMAGE_ARMOR_EVENT					CONSTLIT("OnDamageArmor")
#define ON_DAMAGE_ABANDONED_EVENT				CONSTLIT("OnDamageAbandoned")
#define ON_DESTROY_SHOT_EVENT					CONSTLIT("OnDestroyShot")
#define ON_FRAGMENT_EVENT						CONSTLIT("OnFragment")

#define STR_SHIELD_REFLECT						CONSTLIT("reflect")

static char *CACHED_EVENTS[CWeaponFireDesc::evtCount] =
	{
		"OnCreateShot",
		"OnDamageAbandoned",
		"OnDamageArmor",
		"OnDamageOverlay",
		"OnDamageShields",
		"OnDestroyShot",
		"OnFragment",
	};

CWeaponFireDesc::CWeaponFireDesc (void) : 
		m_pExtension(NULL),
		m_pEnhanced(NULL),
		m_pParticleDesc(NULL)

//	CWeaponFireDesc constructor

	{
	}

CWeaponFireDesc::CWeaponFireDesc (const CWeaponFireDesc &Desc)

//	CWeaponFireDesc constructor

	{
	*this = Desc;

	//	Fragments

	SFragmentDesc *pNext = Desc.m_pFirstFragment;
	SFragmentDesc *pPrev = NULL;
	while (pNext)
		{
		SFragmentDesc *pNew = new SFragmentDesc(*pNext);
		pNew->pDesc = new CWeaponFireDesc(*(pNext->pDesc));
		
		if (pPrev == NULL)
			m_pFirstFragment = pNew;
		else
			pPrev->pNext = pNew;

		pPrev = pNew;
		}
	if (pPrev)
		pPrev->pNext = NULL;

	if (Desc.m_pParticleDesc)
		m_pParticleDesc = new CParticleSystemDesc(*Desc.m_pParticleDesc);

	//	Other

	if (Desc.m_pEnhanced)
		m_pEnhanced = new CWeaponFireDesc(*Desc.m_pEnhanced);
	}

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

	if (m_pEnhanced)
		delete m_pEnhanced;

	if (m_pParticleDesc)
		delete m_pParticleDesc;
	}

void CWeaponFireDesc::AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed)

//	AddTypesUsed
//
//	Adds types used by this class

	{
	retTypesUsed->SetAt(m_pAmmoType.GetUNID(), true);
	retTypesUsed->SetAt(m_Image.GetBitmapUNID(), true);
	retTypesUsed->SetAt(m_ExhaustImage.GetBitmapUNID(), true);
	if (m_pEnhanced)
		m_pEnhanced->AddTypesUsed(retTypesUsed);

	retTypesUsed->SetAt(m_pEffect.GetUNID(), true);
	retTypesUsed->SetAt(m_pHitEffect.GetUNID(), true);
	retTypesUsed->SetAt(m_pFireEffect.GetUNID(), true);

	if (m_pParticleDesc)
		m_pParticleDesc->AddTypesUsed(retTypesUsed);

	SFragmentDesc *pNext = m_pFirstFragment;
	while (pNext)
		{
		pNext->pDesc->AddTypesUsed(retTypesUsed);

		pNext = pNext->pNext;
		}
	}

bool CWeaponFireDesc::CanHit (CSpaceObject *pObj) const

//	CanHit
//
//	Returns TRUE if we can hit the given object.

	{
	//	Can we hit worlds?
	
	if (m_fNoWorldHits 
			&& (pObj->GetScale() == scaleWorld || pObj->GetScale() == scaleStar))
		return false;

	//	Can we hit immutables?

	if (m_fNoImmutableHits
			&& pObj->IsImmutable())
		return false;

	//	Can we hit stations

	if (m_fNoStationHits
			&& pObj->GetScale() == scaleStructure)
		return false;

	//	Can we hit immobile objects

	if (m_fNoImmobileHits
			&& !pObj->CanMove())
		return false;

	//	Can we hit ships?

	if (m_fNoShipHits
			&& pObj->GetScale() == scaleShip)
		return false;

	//	OK

	return true;
	}

IEffectPainter *CWeaponFireDesc::CreateEffectPainter (bool bTrackingObj, bool bUseObjectCenter)

//	CreateEffectPainter
//
//	Creates an effect to paint the projectile. The caller is responsible for
//	calling Delete on the result.
//
//	NOTE: We may return NULL if the weapon has no effect.

	{
	CCreatePainterCtx Ctx;
	Ctx.SetWeaponFireDesc(this);
	Ctx.SetTrackingObject(bTrackingObj);
	Ctx.SetUseObjectCenter(bUseObjectCenter);

	return m_pEffect.CreatePainter(Ctx);
	}

void CWeaponFireDesc::CreateFireEffect (CSystem *pSystem, CSpaceObject *pSource, const CVector &vPos, const CVector &vVel, int iDir)

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

		IEffectPainter *pPainter = m_pFireEffect.CreatePainter(Ctx, g_pUniverse->FindDefaultFireEffect(m_Damage.GetDamageType()));
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

void CWeaponFireDesc::CreateHitEffect (CSystem *pSystem, SDamageCtx &DamageCtx)

//	CreateHitEffect
//
//	Creates an effect when the weapon hits an object

	{
	//	Create the hit effect painter.

	CCreatePainterCtx Ctx;
	Ctx.SetWeaponFireDesc(this);
	Ctx.SetDamageCtx(DamageCtx);

	IEffectPainter *pPainter = m_pHitEffect.CreatePainter(Ctx, g_pUniverse->FindDefaultHitEffect(m_Damage.GetDamageType()));
	if (pPainter == NULL)
		return;

	//	Now create the effect

	if (CEffect::Create(pPainter,
			pSystem,
			((DamageCtx.pObj && !DamageCtx.pObj->IsDestroyed()) ? DamageCtx.pObj : NULL),
			DamageCtx.vHitPos,
			(DamageCtx.pObj ? DamageCtx.pObj->GetVel() : CVector()),
			DamageCtx.iDirection) != NOERROR)
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

bool CWeaponFireDesc::FindDataField (const CString &sField, CString *retsValue)

//	FindDataField
//
//	Returns data field for a weapon fire descriptor

	{
	ICCItem *pResult;

	if (strEquals(sField, FIELD_PARTICLE_COUNT))
		{
		if (m_pParticleDesc)
			*retsValue = strFromInt((int)(m_pParticleDesc->GetEmitRate().GetAveValueFloat() * m_pParticleDesc->GetEmitLifetime().GetAveValueFloat()));
		else
			*retsValue = NULL_STR;
		}
	else if (strEquals(sField, FIELD_SOUND))
		*retsValue = (m_FireSound.GetSound() != -1 ? strFromInt(m_FireSound.GetUNID(), false) : NULL_STR);

	//	Otherwise, see if the damage can handle this.
	//
	//	LATER: At some point we should move this code to the GetProperty side,
	//	but for now we need it here since this is the only path that handles
	//	missiles.

	else if (pResult = m_Damage.FindProperty(sField))
		{
		CCodeChain &CC = g_pUniverse->GetCC();
		*retsValue = pResult->GetStringValue();
		pResult->Discard(&CC);
		}

	//	Otherwise, nothing

	else
		return false;

	return true;
	}

CEffectCreator *CWeaponFireDesc::FindEffectCreator (const CString &sUNID)

//	FindEffectCreator
//
//	Finds effect creator from a partial UNID

	{
	char *pPos;

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

CWeaponFireDesc *CWeaponFireDesc::FindWeaponFireDesc (const CString &sUNID, char **retpPos)

//	FindWeaponFireDesc
//
//	Finds the weapon fire desc from a partial UNID

	{
	char *pPos = sUNID.GetASCIIZPointer();

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
			if (m_pEnhanced)
				return m_pEnhanced->FindWeaponFireDesc(CString(pPos, -1, TRUE), retpPos);
			else
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
	char *pPos = sUNID.GetPointer();

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
		ASSERT(pItemType);

		CWeaponFireDesc *pMissileDesc;
		CDeviceClass *pDevice;
		
		//	If this is a device, then parse as weapon

		if (pDevice = pItemType->GetDeviceClass())
			{
			CWeaponClass *pClass = pDevice->AsWeaponClass();
			if (pClass == NULL)
				return NULL;

			//	Get the ordinal

			ASSERT(*pPos == '/');
			pPos++;
			int iOrdinal = strParseInt(pPos, 0, &pPos);

			//	Get the weapon fire desc of the ordinal

			CWeaponFireDesc *pDesc = pClass->GetVariant(iOrdinal);
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

		CCodeChainCtx CCCtx;

		CCCtx.SaveAndDefineSourceVar(pShot);
		CCCtx.DefineSpaceObject(CONSTLIT("aTargetObj"), pTarget);
		CCCtx.DefineSpaceObject(CONSTLIT("aAttacker"), Source.GetObj());
		CCCtx.DefineSpaceObject(CONSTLIT("aOrderGiver"), (Source.GetObj() ? Source.GetObj()->GetOrderGiver(Source.GetCause()) : NULL));
		CCCtx.DefineItemType(CONSTLIT("aWeaponType"), GetWeaponType());

		ICCItem *pResult = CCCtx.Run(Event);
		if (pResult->IsError())
			pShot->ReportEventError(ON_CREATE_SHOT_EVENT, pResult);

		CCCtx.Discard(pResult);
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

		CCodeChainCtx CCCtx;

		CCCtx.SaveAndDefineSourceVar(Ctx.pObj);
		CCCtx.DefineSpaceObject(CONSTLIT("aCause"), Ctx.pCause);
		CCCtx.DefineSpaceObject(CONSTLIT("aAttacker"), Ctx.Attacker.GetObj());
		CCCtx.DefineSpaceObject(CONSTLIT("aOrderGiver"), (Ctx.Attacker.GetObj() ? Ctx.Attacker.GetObj()->GetOrderGiver(Ctx.Attacker.GetCause()) : NULL));
		CCCtx.DefineVector(CONSTLIT("aHitPos"), Ctx.vHitPos);
		CCCtx.DefineInteger(CONSTLIT("aHitDir"), Ctx.iDirection);
		CCCtx.DefineInteger(CONSTLIT("aDamageHP"), Ctx.iDamage);
		CCCtx.DefineString(CONSTLIT("aDamageType"), GetDamageShortName(Ctx.Damage.GetDamageType()));
		CCCtx.DefineItemType(CONSTLIT("aWeaponType"), Ctx.pDesc->GetWeaponType());

		ICCItem *pResult = CCCtx.Run(Event);
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

		CCCtx.Discard(pResult);

		return bResult;
		}
	else
		return false;
	}

bool CWeaponFireDesc::FireOnDamageArmor (SDamageCtx &Ctx)

//	FireOnDamageArmor
//
//	Fire OnDamageArmor event. Returns TRUE if we should skip further armor damage

	{
	SEventHandlerDesc Event;
	if (FindEventHandler(evtOnDamageArmor, &Event))
		{
		//	Setup arguments

		CCodeChainCtx CCCtx;

		CCCtx.SaveAndDefineSourceVar(Ctx.pObj);
		CCCtx.DefineInteger(CONSTLIT("aArmorSeg"), Ctx.iSectHit);
		CCCtx.DefineSpaceObject(CONSTLIT("aCause"), Ctx.pCause);
		CCCtx.DefineSpaceObject(CONSTLIT("aAttacker"), Ctx.Attacker.GetObj());
		CCCtx.DefineSpaceObject(CONSTLIT("aOrderGiver"), (Ctx.Attacker.GetObj() ? Ctx.Attacker.GetObj()->GetOrderGiver(Ctx.Attacker.GetCause()) : NULL));
		CCCtx.DefineVector(CONSTLIT("aHitPos"), Ctx.vHitPos);
		CCCtx.DefineInteger(CONSTLIT("aHitDir"), Ctx.iDirection);
		CCCtx.DefineInteger(CONSTLIT("aDamageHP"), Ctx.iDamage);
		CCCtx.DefineString(CONSTLIT("aDamageType"), GetDamageShortName(Ctx.Damage.GetDamageType()));
		CCCtx.DefineItemType(CONSTLIT("aWeaponType"), Ctx.pDesc->GetWeaponType());

		ICCItem *pResult = CCCtx.Run(Event);
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

		CCCtx.Discard(pResult);

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

		CCodeChainCtx CCCtx;

		CCCtx.SaveAndDefineSourceVar(Ctx.pObj);
		CCCtx.DefineInteger(CONSTLIT("aArmorSeg"), Ctx.iSectHit);
		CCCtx.DefineInteger(CONSTLIT("aOverlayID"), pOverlay->GetID());
		CCCtx.DefineSpaceObject(CONSTLIT("aCause"), Ctx.pCause);
		CCCtx.DefineSpaceObject(CONSTLIT("aAttacker"), Ctx.Attacker.GetObj());
		CCCtx.DefineSpaceObject(CONSTLIT("aOrderGiver"), (Ctx.Attacker.GetObj() ? Ctx.Attacker.GetObj()->GetOrderGiver(Ctx.Attacker.GetCause()) : NULL));
		CCCtx.DefineVector(CONSTLIT("aHitPos"), Ctx.vHitPos);
		CCCtx.DefineInteger(CONSTLIT("aHitDir"), Ctx.iDirection);
		CCCtx.DefineInteger(CONSTLIT("aDamageHP"), Ctx.iDamage);
		CCCtx.DefineString(CONSTLIT("aDamageType"), GetDamageShortName(Ctx.Damage.GetDamageType()));
		CCCtx.DefineItemType(CONSTLIT("aWeaponType"), Ctx.pDesc->GetWeaponType());

		ICCItem *pResult = CCCtx.Run(Event);
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

		CCCtx.Discard(pResult);

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

		CCodeChainCtx CCCtx;

		CItemListManipulator ItemList(Ctx.pObj->GetItemList());
		CShip *pShip = Ctx.pObj->AsShip();
		if (pShip)
			pShip->SetCursorAtDevice(ItemList, iDevice);

		CCCtx.SaveAndDefineSourceVar(Ctx.pObj);
		CCCtx.DefineInteger(CONSTLIT("aArmorSeg"), Ctx.iSectHit);
		CCCtx.DefineInteger(CONSTLIT("aDevice"), iDevice);
		CCCtx.DefineItem(CONSTLIT("aDeviceItem"), ItemList.GetItemAtCursor());
		CCCtx.DefineSpaceObject(CONSTLIT("aCause"), Ctx.pCause);
		CCCtx.DefineSpaceObject(CONSTLIT("aAttacker"), Ctx.Attacker.GetObj());
		CCCtx.DefineSpaceObject(CONSTLIT("aOrderGiver"), (Ctx.Attacker.GetObj() ? Ctx.Attacker.GetObj()->GetOrderGiver(Ctx.Attacker.GetCause()) : NULL));
		CCCtx.DefineVector(CONSTLIT("aHitPos"), Ctx.vHitPos);
		CCCtx.DefineInteger(CONSTLIT("aHitDir"), Ctx.iDirection);
		CCCtx.DefineInteger(CONSTLIT("aDamageHP"), Ctx.iDamage);
		CCCtx.DefineString(CONSTLIT("aDamageType"), GetDamageShortName(Ctx.Damage.GetDamageType()));
		CCCtx.DefineItemType(CONSTLIT("aWeaponType"), Ctx.pDesc->GetWeaponType());

		CCCtx.DefineInteger(CONSTLIT("aShieldHP"), Ctx.iHPLeft);
		CCCtx.DefineInteger(CONSTLIT("aShieldDamageHP"), Ctx.iShieldDamage);
		CCCtx.DefineInteger(CONSTLIT("aArmorDamageHP"), Ctx.iDamage - Ctx.iAbsorb);
		if (Ctx.bReflect)
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
					Ctx.bReflect = true;
					Ctx.iAbsorb = Ctx.iDamage;
					Ctx.iShieldDamage = 0;
					}
				else
					{
					Ctx.iShieldDamage = Max(0, Min(pResult->GetElement(0)->GetIntegerValue(), Ctx.iHPLeft));
					if (Ctx.bReflect)
						{
						Ctx.bReflect = false;
						Ctx.iAbsorb = Ctx.iOriginalAbsorb;
						}
					}
				}

			//	Two values mean we modified both damage to armor and shield damage

			else if (pResult->GetCount() == 2)
				{
				Ctx.bReflect = false;
				Ctx.iShieldDamage = Max(0, Min(pResult->GetElement(0)->GetIntegerValue(), Ctx.iHPLeft));
				Ctx.iAbsorb = Max(0, Ctx.iDamage - Max(0, pResult->GetElement(1)->GetIntegerValue()));
				}

			//	Otherwise, we deal with reflection

			else
				{
				Ctx.bReflect = strEquals(pResult->GetElement(0)->GetStringValue(), STR_SHIELD_REFLECT);
				Ctx.iShieldDamage = Max(0, Min(pResult->GetElement(1)->GetIntegerValue(), Ctx.iHPLeft));
				Ctx.iAbsorb = Max(0, Ctx.iDamage - Max(0, pResult->GetElement(2)->GetIntegerValue()));
				}

			//	Proceed with processing

			bResult = false;
			}

		//	If this is the string "reflect" then we reflect

		else if (strEquals(pResult->GetStringValue(), STR_SHIELD_REFLECT))
			{
			Ctx.bReflect = true;
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

		CCCtx.Discard(pResult);

		return bResult;
		}
	else
		return false;
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

		CCodeChainCtx CCCtx;

		CCCtx.SaveAndDefineSourceVar(pShot);
		CCCtx.DefineItemType(CONSTLIT("aWeaponType"), GetWeaponType());
		CCCtx.DefineSpaceObject(CONSTLIT("aAttacker"), pShot->GetSource());
		CCCtx.DefineSpaceObject(CONSTLIT("aOrderGiver"), (pShot->GetSource() ? pShot->GetSource()->GetOrderGiver(pShot->GetDamageCauseType()) : NULL));

		ICCItem *pResult = CCCtx.Run(Event);
		if (pResult->IsError())
			pShot->ReportEventError(ON_DESTROY_SHOT_EVENT, pResult);

		CCCtx.Discard(pResult);
		}
	}

bool CWeaponFireDesc::FireOnFragment (const CDamageSource &Source, CSpaceObject *pShot, const CVector &vHitPos, CSpaceObject *pNearestObj, CSpaceObject *pTarget)

//	FireOnFragment
//
//	Event fires when a shot fragments. If we return TRUE then we skip the default
//	fragmentation event.

	{
	SEventHandlerDesc Event;
	if (FindEventHandler(evtOnFragment, &Event))
		{
		//	Setup arguments

		CCodeChainCtx CCCtx;

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
		CCCtx.DefineSpaceObject(CONSTLIT("aOrderGiver"), (pAttacker ? pAttacker->GetOrderGiver(Source.GetCause()) : NULL));

		ICCItem *pResult = CCCtx.Run(Event);
		if (pResult->IsError())
			pShot->ReportEventError(ON_FRAGMENT_EVENT, pResult);

		//	If we return Nil, then we continue processing

		bool bResult;
		if (pResult->IsNil())
			bResult = false;

		//	Otherwise, we skip fragmentation

		else
			bResult = true;

		CCCtx.Discard(pResult);

		return bResult;
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
		return (m_MissileSpeed.GetAveValueFloat() * LIGHT_SPEED / 100.0);
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

	return g_pUniverse->FindDefaultFireEffect(m_Damage.GetDamageType());
	}

Metric CWeaponFireDesc::GetInitialSpeed (void) const

//	GetInitialSpeed
//
//	Returns the initial speed of the missile (when launched)

	{
	if (m_fVariableInitialSpeed)
		return (double)m_MissileSpeed.Roll() * LIGHT_SPEED / 100;
	else
		return GetRatedSpeed();
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

CItemType *CWeaponFireDesc::GetWeaponType (CItemType **retpLauncher) const

//	GetWeaponType
//
//	Returns the item type that best represents this descriptor. For missiles,
//	we return the missile item UNID; for single-shot weapons (including ammo weapons)
//	we return the weapon item UNID

	{
	char *pPos = m_sUNID.GetPointer();

	//	Get the weapon UNID and the ordinal

	DWORD dwUNID = (DWORD)strParseInt(pPos, 0, &pPos);

	//	Get the type

	CItemType *pItemType = g_pUniverse->FindItemType(dwUNID);
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
			CWeaponFireDesc *pMissileDesc = pClass->GetVariant(iOrdinal);
			if (pMissileDesc == NULL)
				return NULL;

			//	If we have ammo, then return the ammo type

			CItemType *pAmmoType = pMissileDesc->GetAmmoType();
			if (pAmmoType)
				return pAmmoType;

			//	Otherwise return the launcher (e.g., DM600)

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

void CWeaponFireDesc::InitFromDamage (DamageDesc &Damage)

//	InitFromDamage
//
//	Conses up a new CWeaponFireDesc from just a damage structure.

	{
	int i;

	m_bFragment = false;

	//	Load basic attributes

	m_sUNID = NULL_STR;
	m_Lifetime.SetConstant(1);
	m_bCanDamageSource = false;
	m_bAutoTarget = false;
	m_InitialDelay.SetConstant(0);

	//	Hit criteria

	m_fNoFriendlyFire = false;
	m_fNoWorldHits = false;
	m_fNoImmutableHits = false;
	m_fNoStationHits = false;
	m_fNoImmobileHits = false;
	m_fNoShipHits = false;

	//	Load missile speed

	m_fVariableInitialSpeed = false;
	m_MissileSpeed.SetConstant(100);
	m_rMissileSpeed = LIGHT_SPEED;

	//	Effects

	m_pEffect.Set(NULL);
	m_pParticleDesc = NULL;

	//	Load stealth

	m_iStealth = CSpaceObject::stealthNormal;

	//	Load specific properties

	m_iFireType = ftMissile;
	m_bDirectional = false;
	m_iManeuverability = 0;
	m_iManeuverRate = -1;
	m_iAccelerationFactor = 0;
	m_rMaxMissileSpeed = m_rMissileSpeed;

	//	Hit points and interaction

	m_iHitPoints = 0;
	m_iInteraction = 0;

	//	Load exhaust data

	m_iExhaustRate = 0;
	m_iExhaustLifetime = 0;
	m_rExhaustDrag = 0.0;

	//	We initialize this with the UNID, and later resolve the reference
	//	during OnDesignLoadComplete

	m_pAmmoType = NULL;

	m_iContinuous = 0;
	m_iPassthrough = 0;

	//	Load damage

	m_Damage = Damage;

	//	Fragments

	m_pFirstFragment = NULL;
	m_bProximityBlast = false;
	m_iProximityFailsafe = 0;

	//	Compute max effective range

	m_rMaxEffectiveRange = m_rMissileSpeed;

	//	Effects

	m_pHitEffect.Set(NULL);
	m_pFireEffect.Set(NULL);

	//	Vapor trail

	m_iVaporTrailWidth = 0;
	m_iVaporTrailLength = 0;

	//	Sound

	m_FireSound = CSoundRef();

	//	Enhanced

	m_pEnhanced = NULL;

	//	Cached events

	for (i = 0; i < evtCount; i++)
		{
		m_CachedEvents[i].pExtension = NULL;
		m_CachedEvents[i].pCode = NULL;
		}
	}

ALERROR CWeaponFireDesc::InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID, bool bDamageOnly)

//	InitFromXML
//
//	Loads shot data from an element

	{
	ALERROR error;
	int i;

	m_pExtension = Ctx.pExtension;
	m_fVariableInitialSpeed = false;
	m_bFragment = false;

	//	Load basic attributes

	m_sUNID = sUNID;
	m_Lifetime.LoadFromXML(pDesc->GetAttribute(LIFETIME_ATTRIB));
	int iMaxLifetime = m_Lifetime.GetMaxValue();
	m_bCanDamageSource = pDesc->GetAttributeBool(CAN_HIT_SOURCE_ATTRIB);
	m_bAutoTarget = pDesc->GetAttributeBool(AUTO_TARGET_ATTRIB);
	m_InitialDelay.LoadFromXML(pDesc->GetAttribute(INITIAL_DELAY_ATTRIB));

	//	Hit criteria

	m_fNoFriendlyFire = pDesc->GetAttributeBool(NO_FRIENDLY_FIRE_ATTRIB);
	m_fNoWorldHits = pDesc->GetAttributeBool(NO_WORLD_HITS_ATTRIB);
	m_fNoImmutableHits = pDesc->GetAttributeBool(NO_IMMUTABLE_HITS_ATTRIB);
	m_fNoStationHits = pDesc->GetAttributeBool(NO_STATION_HITS_ATTRIB);
	m_fNoImmobileHits = pDesc->GetAttributeBool(NO_IMMOBILE_HITS_ATTRIB);
	m_fNoShipHits = pDesc->GetAttributeBool(NO_SHIP_HITS_ATTRIB);

	//	Load missile speed

	CString sData;
	if (pDesc->FindAttribute(MISSILE_SPEED_ATTRIB, &sData))
		{
		if (error = m_MissileSpeed.LoadFromXML(sData))
			{
			Ctx.sError = CONSTLIT("Invalid missile speed attribute");
			return ERR_FAIL;
			}

		m_fVariableInitialSpeed = !m_MissileSpeed.IsConstant();
		m_rMissileSpeed = m_MissileSpeed.GetAveValueFloat() * LIGHT_SPEED / 100;
		}
	else
		{
		m_MissileSpeed.SetConstant(100);
		m_fVariableInitialSpeed = false;
		m_rMissileSpeed = LIGHT_SPEED;
		}

	//	Load the effect to use

	if (error = m_pEffect.LoadEffect(Ctx, 
			strPatternSubst("%s:e", sUNID),
			pDesc->GetContentElementByTag(EFFECT_TAG),
			pDesc->GetAttribute(EFFECT_ATTRIB)))
		return error;

	//	Load stealth

	m_iStealth = pDesc->GetAttributeInteger(STEALTH_ATTRIB);
	if (m_iStealth == 0)
		m_iStealth = CSpaceObject::stealthNormal;

	//	Other properties

	m_iSplashChance = pDesc->GetAttributeIntegerBounded(PARTICLE_SPLASH_CHANCE_ATTRIB, 0, 100, 0);
	m_iMissChance = pDesc->GetAttributeIntegerBounded(PARTICLE_MISS_CHANCE_ATTRIB, 0, 100, 0);

	//	Initialize some variables not used by all types

	m_iHitPoints = 0;

	//	Load specific properties

	CString sValue = pDesc->GetAttribute(FIRE_TYPE_ATTRIB);
	if (strEquals(sValue, FIRE_TYPE_MISSILE) || strEquals(sValue, FIRE_TYPE_BEAM))
		{
		m_iFireType = (strEquals(sValue, FIRE_TYPE_BEAM) ? ftBeam : ftMissile);

		//	For backwards compatibility, if we don't have an effect, assume
		//	a beam effect.

		if (m_iFireType == ftBeam && m_pEffect.IsEmpty())
			{
			if (error = m_pEffect.CreateBeamEffect(Ctx, pDesc, strPatternSubst("%s:e", sUNID)))
				return error;
			}

		//	Load the image for the missile

		CXMLElement *pImage = pDesc->GetContentElementByTag(IMAGE_TAG);
		if (pImage)
			if (error = m_Image.InitFromXML(Ctx, pImage))
				return error;

		m_bDirectional = pDesc->GetAttributeBool(DIRECTIONAL_ATTRIB);
		if (m_bDirectional && m_pEffect)
			m_pEffect->SetVariants(g_RotationRange);

		m_iAccelerationFactor = pDesc->GetAttributeInteger(ACCELERATION_FACTOR_ATTRIB);
		int iMaxSpeed = pDesc->GetAttributeInteger(MAX_MISSILE_SPEED_ATTRIB);
		if (iMaxSpeed == 0)
			m_rMaxMissileSpeed = m_rMissileSpeed;
		else
			m_rMaxMissileSpeed = (Metric)iMaxSpeed * LIGHT_SPEED / 100.0;

		//	Hit points and interaction

		m_iHitPoints = pDesc->GetAttributeInteger(HIT_POINTS_ATTRIB);
		CString sInteraction;
		if (pDesc->FindAttribute(INTERACTION_ATTRIB, &sInteraction))
			m_iInteraction = strToInt(sInteraction, 100);
		else
			m_iInteraction = (m_iFireType == ftBeam ? 0 : 100);

		//	Load exhaust data

		CXMLElement *pExhaust = pDesc->GetContentElementByTag(MISSILE_EXHAUST_TAG);
		if (pExhaust)
			{
			m_iExhaustRate = pExhaust->GetAttributeInteger(EXHAUST_RATE_ATTRIB);
			m_iExhaustLifetime = pExhaust->GetAttributeInteger(EXHAUST_LIFETIME_ATTRIB);
			m_rExhaustDrag = pExhaust->GetAttributeInteger(EXHAUST_DRAG_ATTRIB) / 100.0;

			CXMLElement *pImage = pExhaust->GetContentElementByTag(IMAGE_TAG);
			if (error = m_ExhaustImage.InitFromXML(Ctx, pImage))
				return error;
			}
		else
			{
			m_iExhaustRate = 0;
			m_iExhaustLifetime = 0;
			m_rExhaustDrag = 0.0;
			}
		}
	else if (strEquals(sValue, FIRE_TYPE_AREA))
		{
		m_iFireType = ftArea;

		m_rMaxMissileSpeed = m_rMissileSpeed;

		//	Load expansion speed

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
		}
	else if (strEquals(sValue, FIRE_TYPE_PARTICLES))
		{
		m_iFireType = ftParticles;

		//	Initialize a particle system descriptor

		m_pParticleDesc = new CParticleSystemDesc;

		//	Look for a <ParticleSystem> definition. If we have it, then we let
		//	it initialize from there.

		CXMLElement *pParticleSystem = pDesc->GetContentElementByTag(PARTICLE_SYSTEM_TAG);
		if (pParticleSystem)
			{
			if (error = m_pParticleDesc->InitFromXML(Ctx, pParticleSystem, sUNID))
				return error;

			//	We take certain values from the particle system.

			m_MissileSpeed = m_pParticleDesc->GetEmitSpeed();
			m_Lifetime.SetConstant(m_pParticleDesc->GetParticleLifetime().GetMaxValue() + (m_pParticleDesc->GetEmitLifetime().GetMaxValue() - 1));
			iMaxLifetime = m_Lifetime.GetMaxValue();
			m_fVariableInitialSpeed = !m_MissileSpeed.IsConstant();
			m_rMissileSpeed = m_MissileSpeed.GetAveValueFloat() * LIGHT_SPEED / 100.0;
			m_iMissChance = m_pParticleDesc->GetMissChance();
			m_iSplashChance = m_pParticleDesc->GetSplashChance();
			}

		//	Otherwise, we initialize from our root (in backwards compatible mode).

		else
			{
			if (error = m_pParticleDesc->InitFromWeaponDescXML(Ctx, pDesc))
				return error;

			//	In this case we honor settings from pDesc, since we're in 
			//	backwards compatible mode. [In the normal case we expect these
			//	settings to be in the <ParticleSystem> element.]

			m_pParticleDesc->SetEmitSpeed(m_MissileSpeed);
			m_pParticleDesc->SetParticleLifetime(m_Lifetime);
			m_pParticleDesc->SetMissChance(m_iMissChance);
			m_pParticleDesc->SetSplashChance(m_iSplashChance);

			//	We always set the old compatibility behavior.

			m_pParticleDesc->SetSprayCompatible();
			}

		//	Initialize other variables

		m_rMaxMissileSpeed = m_rMissileSpeed;
		}
	else if (strEquals(sValue, FIRE_TYPE_RADIUS))
		{
		m_iFireType = ftRadius;

		m_rMaxMissileSpeed = m_rMissileSpeed;

		m_rMinRadius = LIGHT_SECOND * (Metric)pDesc->GetAttributeInteger(MIN_RADIUS_ATTRIB);
		m_rMaxRadius = LIGHT_SECOND * (Metric)pDesc->GetAttributeInteger(MAX_RADIUS_ATTRIB);
		}
	else if (!bDamageOnly)
		{
		Ctx.sError = CONSTLIT("Invalid weapon fire type");
		return ERR_FAIL;
		}

	//	The effect should have the same lifetime as the shot
	//	Note: For radius damage it is the other way around (we set iMaxLifetime based on
	//	the effect)

	if (m_pEffect
			&& m_iFireType != ftRadius)
		m_pEffect->SetLifetime(iMaxLifetime);

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

	//	Load continuous and passthrough

	m_iContinuous = pDesc->GetAttributeInteger(BEAM_CONTINUOUS_ATTRIB);
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

		pNewDesc->pDesc = new CWeaponFireDesc;
		CString sFragUNID = strPatternSubst("%s/f%d", sUNID, iFragCount++);
		if (error = pNewDesc->pDesc->InitFromXML(Ctx, pFragDesc, sFragUNID))
			return error;

		pNewDesc->pDesc->m_bFragment = true;

		//	Set the fragment count

		CString sCount = pFragDesc->GetAttribute(COUNT_ATTRIB);
		if (sCount.IsBlank())
			sCount = pDesc->GetAttribute(FRAGMENT_COUNT_ATTRIB);
		pNewDesc->Count.LoadFromXML(sCount, 1);

		//	Set MIRV flag

		pNewDesc->bMIRV = (pFragDesc->GetAttributeBool(MULTI_TARGET_ATTRIB) 
				|| pDesc->GetAttributeBool(FRAGMENT_TARGET_ATTRIB));
		}

	//	If we have fragments, then set proximity appropriately
	//	NOTE: We set the fail safe value even if we don't set the proximity
	//	blast because we might set m_bProximityBlast later if there
	//	is an OnFragment event.

	m_bProximityBlast = (iFragCount != 0);
	m_iProximityFailsafe = pDesc->GetAttributeInteger(FAILSAFE_ATTRIB);

	//	Compute max effective range

	if (m_iFireType == ftArea)
		m_rMaxEffectiveRange = (m_ExpansionSpeed.GetAveValueFloat() * LIGHT_SECOND / 100.0) * Ticks2Seconds(iMaxLifetime) * 0.75;
	else if (m_iFireType == ftRadius)
		m_rMaxEffectiveRange = m_rMaxRadius;
	else
		{
		Metric rEffectiveLifetime;
		if (m_iManeuverability > 0)
			rEffectiveLifetime = Ticks2Seconds(iMaxLifetime) * 0.75;
		else
			rEffectiveLifetime = Min(Ticks2Seconds(iMaxLifetime), 100.0);

		Metric rSpeed = (m_rMissileSpeed + m_rMaxMissileSpeed) / 2;
		m_rMaxEffectiveRange = rSpeed * rEffectiveLifetime;
		}

	//	If we have fragments, add to the effective range

	if (m_pFirstFragment)
		m_rMaxEffectiveRange += m_pFirstFragment->pDesc->m_rMaxEffectiveRange;

	//	Effects

	if (error = m_pHitEffect.LoadEffect(Ctx,
			strPatternSubst("%s:h", sUNID),
			pDesc->GetContentElementByTag(HIT_EFFECT_TAG),
			pDesc->GetAttribute(HIT_EFFECT_ATTRIB)))
		return error;

	if (error = m_pFireEffect.LoadEffect(Ctx,
			strPatternSubst("%s:f", sUNID),
			pDesc->GetContentElementByTag(FIRE_EFFECT_TAG),
			pDesc->GetAttribute(FIRE_EFFECT_ATTRIB)))
		return error;

	//	Vapor trail

	if (!pDesc->FindAttributeInteger(VAPOR_TRAIL_WIDTH_ATTRIB, &m_iVaporTrailWidth))
		m_iVaporTrailWidth = 100 * pDesc->GetAttributeInteger(VAPOR_TRAIL_ATTRIB);

	if (m_iVaporTrailWidth)
		{
		m_rgbVaporTrailColor = LoadRGBColor(pDesc->GetAttribute(VAPOR_TRAIL_COLOR_ATTRIB));
		m_iVaporTrailLength = pDesc->GetAttributeInteger(VAPOR_TRAIL_LENGTH_ATTRIB);
		if (m_iVaporTrailLength <= 0)
			m_iVaporTrailLength = 64;

		if (!pDesc->FindAttributeInteger(VAPOR_TRAIL_WIDTH_INC_ATTRIB, &m_iVaporTrailWidthInc))
			m_iVaporTrailWidthInc = 25;
		}
	else
		m_iVaporTrailLength = 0;

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

	//	Check to see if this element has an enhanced sub-element. If so, then we
	//	recurse.

	CXMLElement *pEnhanced = pDesc->GetContentElementByTag(ENHANCED_TAG);
	if (pEnhanced)
		{
		m_pEnhanced = new CWeaponFireDesc(*this);
		if (error = m_pEnhanced->OverrideDesc(Ctx, pEnhanced))
			return error;
		}
	else
		m_pEnhanced = NULL;

	return NOERROR;
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

	if (error = m_Image.OnDesignLoadComplete(Ctx))
		return error;

	if (error = m_ExhaustImage.OnDesignLoadComplete(Ctx))
		return error;

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

	if (m_pEnhanced)
		if (error = m_pEnhanced->OnDesignLoadComplete(Ctx))
			return error;

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

	if (HasOnFragmentEvent())
		m_bProximityBlast = true;

	//	Fragment

	SFragmentDesc *pNext = m_pFirstFragment;
	while (pNext)
		{
		if (error = pNext->pDesc->OnDesignLoadComplete(Ctx))
			return error;

		pNext = pNext->pNext;
		}

	return NOERROR;

	DEBUG_CATCH
	}

ALERROR CWeaponFireDesc::OverrideDesc (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	OverrideDesc
//
//	Override shot data (used for enhanced weapons)

	{
	ALERROR error;

	//	Damage

	CString sAttrib;
	if (pDesc->FindAttribute(DAMAGE_ATTRIB, &sAttrib))
		{
		if (error = m_Damage.LoadFromXML(Ctx, sAttrib))
			return error;
		}

	//	Add enhanced UNID

	m_sUNID.Append(CONSTLIT("/e"));

	return NOERROR;
	}

