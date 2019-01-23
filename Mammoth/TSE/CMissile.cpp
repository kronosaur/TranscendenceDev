//	CMissile.cpp
//
//	CMissile class

#include "PreComp.h"

#define TRAIL_COUNT								4
#define MAX_TARGET_RANGE						(24.0 * LIGHT_SECOND)

#define PROPERTY_LIFE_LEFT						CONSTLIT("lifeLeft")
#define PROPERTY_ROTATION						CONSTLIT("rotation")
#define PROPERTY_SOURCE							CONSTLIT("source")
#define PROPERTY_TARGET							CONSTLIT("target")

const DWORD VAPOR_TRAIL_OPACITY =				80;

const Metric MAX_MIRV_TARGET_RANGE =			50.0 * LIGHT_SECOND;
const int MIN_MISSILE_INTERACTION =				10;

static CObjectClass<CMissile>g_Class(OBJID_CMISSILE, NULL);

CMissile::CMissile (void) : CSpaceObject(&g_Class),
		m_pExhaust(NULL),
		m_pPainter(NULL),
		m_pVaporTrailRegions(NULL),
		m_iSavedRotationsCount(0),
		m_pSavedRotations(NULL),
		m_pHit(NULL),
		m_iHitDir(-1)

//	CMissile constructor

	{
	}

CMissile::~CMissile (void)

//	CMissile destructor

	{
	if (m_pPainter)
		m_pPainter->Delete();

	if (m_pExhaust)
		delete m_pExhaust;

	if (m_pVaporTrailRegions)
		delete [] m_pVaporTrailRegions;

	if (m_pSavedRotations)
		delete [] m_pSavedRotations;
	}

int CMissile::ComputeVaporTrail (void)

//	ComputeVaporTrail
//
//	Initialize vapor trail regions

	{
	int i;

	//	We handle this differently for maneuverable, vs non-maneuverable missiles

	if (IsTracking())
		{
		if (m_iSavedRotationsCount == 0)
			return 0;

		//	Allocate array of regions

		if (m_pVaporTrailRegions)
			delete [] m_pVaporTrailRegions;

		m_pVaporTrailRegions = new CG16bitBinaryRegion [m_iSavedRotationsCount];

		//	Compute some constants

		Metric rHalfWidth = (Metric)m_pDesc->GetVaporTrail().iVaporTrailWidth / 200.0;
		Metric rWidthInc = m_pDesc->GetVaporTrail().iVaporTrailWidthInc / 100.0;
		Metric rLength = m_pDesc->GetRatedSpeed() * g_SecondsPerUpdate / g_KlicksPerPixel;

		//	Start at the beginning

		int iLeft = (m_pSavedRotations[0] + 90) % 360;
		int iRight = (m_pSavedRotations[0] + 270) % 360;

		CVector vCenter;
		CVector vLeft = PolarToVector(iLeft, rHalfWidth);
		CVector vRight = PolarToVector(iRight, rHalfWidth);

		SPoint Segment[4];
		Segment[0].x = (int)ceil(vLeft.GetX());
		Segment[0].y = -(int)ceil(vLeft.GetY());
		Segment[1].x = (int)ceil(vRight.GetX());
		Segment[1].y = -(int)ceil(vRight.GetY());

		//	Create all the regions

		for (i = 0; i < m_iSavedRotationsCount; i++)
			{
			if (i + 1 < m_iSavedRotationsCount)
				{
				int iAngle;
				int iRange = AngleRange(m_pSavedRotations[i], m_pSavedRotations[i + 1]);
				if (iRange <= 180)
					iAngle = (m_pSavedRotations[i] + (iRange / 2)) % 360;
				else
					iAngle = (m_pSavedRotations[i + 1] + ((360 - iRange) / 2)) % 360;

				iLeft = (iAngle + 90) % 360;
				iRight = (iAngle + 270) % 360;
				}
			else
				{
				iLeft = (m_pSavedRotations[i] + 90) % 360;
				iRight = (m_pSavedRotations[i] + 270) % 360;
				}

			//	Compute the slope the trail

			vCenter = vCenter + PolarToVector(m_pSavedRotations[i], rLength);
			rHalfWidth += rWidthInc;
			CVector vNewLeft = vCenter + PolarToVector(iLeft, rHalfWidth);
			CVector vNewRight = vCenter + PolarToVector(iRight, rHalfWidth);

			Segment[2].x = (int)ceil(vNewRight.GetX());
			Segment[2].y = -(int)ceil(vNewRight.GetY());
			Segment[3].x = (int)ceil(vNewLeft.GetX());
			Segment[3].y = -(int)ceil(vNewLeft.GetY());

			m_pVaporTrailRegions[i].CreateFromConvexPolygon(4, Segment);

			Segment[0] = Segment[3];
			Segment[1] = Segment[2];
			vLeft = vNewLeft;
			vRight = vNewRight;
			}

		return m_iSavedRotationsCount;
		}
	else
		{
		int iCount = m_pDesc->GetVaporTrail().iVaporTrailLength;

		//	For non-maneuverable missiles, only compute the trail once

		if (m_pVaporTrailRegions)
			return Min(iCount, m_iTick);

		//	Allocate array of regions

		m_pVaporTrailRegions = new CG16bitBinaryRegion [iCount];

		int iDirection = (m_iRotation + 180) % 360;
		int iLeft = (iDirection + 90) % 360;
		int iRight = (iDirection + 270) % 360;

		//	We start a few pixels away from the center line

		Metric rHalfWidth = (Metric)m_pDesc->GetVaporTrail().iVaporTrailWidth / 200.0;
		CVector vLeft = PolarToVector(iLeft, rHalfWidth);
		CVector vRight = PolarToVector(iRight, rHalfWidth);

		//	Compute the slope the trail

		Metric rLength = m_pDesc->GetRatedSpeed() * g_SecondsPerUpdate / g_KlicksPerPixel;
		Metric rWidthInc = m_pDesc->GetVaporTrail().iVaporTrailWidthInc / 100.0;
		CVector vLeftInc = PolarToVector(iDirection, rLength) + PolarToVector(iLeft, rWidthInc);
		CVector vRightInc = PolarToVector(iDirection, rLength) + PolarToVector(iRight, rWidthInc);

		//	Start at the beginning

		SPoint Segment[4];
		Segment[0].x = (int)ceil(vLeft.GetX());
		Segment[0].y = -(int)ceil(vLeft.GetY());
		Segment[1].x = (int)ceil(vRight.GetX());
		Segment[1].y = -(int)ceil(vRight.GetY());

		//	Create all the regions

		for (i = 0; i < iCount; i++)
			{
			CVector vNewLeft = vLeft + vLeftInc;
			CVector vNewRight = vRight + vRightInc;

			Segment[2].x = (int)ceil(vNewRight.GetX());
			Segment[2].y = -(int)ceil(vNewRight.GetY());
			Segment[3].x = (int)ceil(vNewLeft.GetX());
			Segment[3].y = -(int)ceil(vNewLeft.GetY());

			//	Note: the ConvexPolygon code yields a much better result than
			//	SimplePolygon.
			m_pVaporTrailRegions[i].CreateFromConvexPolygon(4, Segment);

			Segment[0] = Segment[3];
			Segment[1] = Segment[2];
			vLeft = vNewLeft;
			vRight = vNewRight;
			}

		return Min(iCount, m_iTick);
		}
	}

ALERROR CMissile::Create (CSystem *pSystem, SShotCreateCtx &Ctx, CMissile **retpMissile)

//	Create
//
//	Creates a missile

	{
	ALERROR error;
	CMissile *pMissile;

	pMissile = new CMissile;
	if (pMissile == NULL)
		return ERR_MEMORY;

	pMissile->Place(Ctx.vPos, Ctx.vVel);

	//	We can't save missiles without an UNID
	ASSERT(!Ctx.pDesc->GetUNID().IsBlank());

	//	Target must be valid
	ASSERT(Ctx.pTarget == NULL || !Ctx.pTarget->IsDestroyed());
	if (Ctx.pTarget && Ctx.pTarget->IsDestroyed())
		Ctx.pTarget = NULL;

	//	Don't bother telling others when we are destroyed (Note that
	//	if we do this then we also need to set the CannotBeHit flag;
	//	otherwise we will crash when a beam hits us. This is because
	//	m_pHit is setup in Move and the object can go away between then
	//	and our Update event.)

	if (Ctx.pDesc->GetInteraction() == 0)
		{
		pMissile->DisableObjectDestructionNotify();
		pMissile->SetCannotBeHit();
		}

	//	Get notifications when other objects are destroyed
	pMissile->SetObjectDestructionHook();

	pMissile->m_pDesc = Ctx.pDesc;
	pMissile->m_pEnhancements = Ctx.pEnhancements;
	pMissile->m_iHitPoints = Ctx.pDesc->GetHitPoints();
	pMissile->m_iLifeLeft = Ctx.pDesc->GetLifetime();
	pMissile->m_iTick = 0;
	pMissile->m_Source = Ctx.Source;
	pMissile->m_pHit = NULL;
	pMissile->m_iRotation = Ctx.iDirection;
	pMissile->m_pTarget = Ctx.pTarget;
	pMissile->m_fDestroyOnAnimationDone = false;
	pMissile->m_fReflection = false;
	pMissile->m_fDetonate = false;
	pMissile->m_fPassthrough = false;
	pMissile->m_fPainterFade = false;
	pMissile->m_fFragment = ((Ctx.dwFlags & SShotCreateCtx::CWF_FRAGMENT) ? true : false);
	pMissile->m_dwSpareFlags = 0;

	//	If we've got a detonation interval, then set it up

	int iNext;
	if (Ctx.pDesc->HasFragmentInterval(&iNext))
		pMissile->m_iNextDetonation = Max(Ctx.pDesc->GetProximityFailsafe(), iNext);
	else
		pMissile->m_iNextDetonation = -1;

	//	Friendly fire

	if (!Ctx.pDesc->CanHitFriends())
		pMissile->SetNoFriendlyFire();

	//	Remember the sovereign of the source (in case the source is destroyed)

	pMissile->m_pSovereign = pMissile->m_Source.GetSovereign();

	//	Create a painter instance

	pMissile->m_pPainter = Ctx.pDesc->CreateEffectPainter(Ctx);
	if (pMissile->m_pPainter)
		pMissile->SetBounds(pMissile->m_pPainter);

	//	Create exhaust trail, if necessary

	if (Ctx.pDesc->GetExhaust().iExhaustRate > 0)
		{
		int iCount = (Ctx.pDesc->GetExhaust().iExhaustLifetime / Ctx.pDesc->GetExhaust().iExhaustRate) + 1;
		pMissile->m_pExhaust = new TQueue<SExhaustParticle>(iCount);
		}
	else
		pMissile->m_pExhaust = NULL;

	//	Create vapor trail, if necessary

	if (Ctx.pDesc->GetVaporTrail().iVaporTrailWidth)
		pMissile->SetBounds(2048.0 * g_KlicksPerPixel);

	//	Add to system

	if (error = pMissile->AddToSystem(pSystem))
		{
		delete pMissile;
		return error;
		}

	//	Done

	if (retpMissile)
		*retpMissile = pMissile;

	return NOERROR;
	}

void CMissile::CreateFragments (const CVector &vPos)

//	CreateFragments
//
//	Create fragments

	{
	DEBUG_TRY

	//	If we're need a target to detonate and we don't have one, then we're
	//	done.
	//
	//	NOTE: If the shot is MIRVed, then the check will happen inside
	//	CreateWeaponFragments.

	if (m_pDesc->IsTargetRequired() 
			&& m_pTarget == NULL
			&& !m_pDesc->IsMIRV())
		return;

    //  If we triggering inside an object, then we only create half the number
    //  of fragments (as if it hit on the surface).

    int iFraction = 100;
    if (m_pHit && m_pHit->PointInObject(m_pHit->GetPos(), vPos))
        iFraction = 50;

	//	If there is an event, then let it handle the fragmentation

	if (m_pDesc->FireOnFragment(m_Source, this, vPos, m_pHit, m_pTarget))
		return;

	//	NOTE: Missile fragments don't inherit the velocity of the missile
	//	(otherwise, fragmentation weapons explode too late to do much damage)

	if (m_pDesc->HasFragments())
		{
		SShotCreateCtx FragCtx;
		FragCtx.pDesc = m_pDesc;
		FragCtx.pEnhancements = m_pEnhancements;
		FragCtx.Source = m_Source;
		FragCtx.pTarget = m_pTarget;
		FragCtx.vPos = vPos;

		GetSystem()->CreateWeaponFragments(FragCtx,	this, iFraction);
		}

	//	Create the hit effect

	SDamageCtx DamageCtx(NULL,
			m_pDesc,
			m_pEnhancements,
			m_Source,
			this,
			mathRandom(0, 359),
			vPos);

	m_pDesc->CreateHitEffect(GetSystem(), DamageCtx);

	DEBUG_CATCH
	}

void CMissile::CreateReflection (const CVector &vPos, int iDirection, CMissile **retpReflection)

//	CreateReflection
//
//	Creates a new object that is a clone of this one but moving
//	in the given direction.

	{
	CMissile *pReflection;

	SShotCreateCtx ReflectCtx;
	ReflectCtx.pDesc = m_pDesc;
	ReflectCtx.pEnhancements = m_pEnhancements;
	ReflectCtx.Source = m_Source;
	ReflectCtx.vPos = vPos;
	ReflectCtx.vVel = PolarToVector(iDirection, GetVel().Length());
	ReflectCtx.iDirection = iDirection;

	Create(GetSystem(), ReflectCtx, &pReflection);

	pReflection->m_fReflection = true;
	if (retpReflection)
		{
		*retpReflection = pReflection;
		}
	}

CString CMissile::DebugCrashInfo (void)

//	DebugCrashInfo
//
//	Dump debug information

	{
	CString sResult;

	if (m_fDestroyOnAnimationDone)
		sResult.Append(CONSTLIT("m_fDestroyOnAnimationDone: true\r\n"));
	sResult.Append(strPatternSubst(CONSTLIT("m_pDesc: %s\r\n"), m_pDesc->GetUNID()));
	sResult.Append(strPatternSubst(CONSTLIT("m_Source: %s\r\n"), CSpaceObject::DebugDescribe(m_Source.GetObj())));
	sResult.Append(strPatternSubst(CONSTLIT("m_pHit: %s\r\n"), CSpaceObject::DebugDescribe(m_pHit)));
	sResult.Append(strPatternSubst(CONSTLIT("m_iHitDir: %d\r\n"), m_iHitDir));
	sResult.Append(strPatternSubst(CONSTLIT("m_pTarget: %s\r\n"), CSpaceObject::DebugDescribe(m_pTarget)));
	sResult.Append(strPatternSubst(CONSTLIT("m_iDestiny: %d\r\n"), GetDestiny()));
	sResult.Append(strPatternSubst(CONSTLIT("m_iTick: %d\r\n"), m_iTick));
	sResult.Append(strPatternSubst(CONSTLIT("m_iLifeLeft: %d\r\n"), m_iLifeLeft));

	//	m_pExhaust

	try
		{
		if (m_pExhaust)
			{
			int iExhaustCount = m_pExhaust->GetCount();
			sResult.Append(strPatternSubst(CONSTLIT("m_pExhaust: %d\r\n"), iExhaustCount));
			}
		else
			sResult.Append(CONSTLIT("m_pExhaust: none\r\n"));
		}
	catch (...)
		{
		sResult.Append(strPatternSubst(CONSTLIT("m_pExhaust: %x [invalid]\r\n"), (DWORD)m_pExhaust));
		}

	//	m_pEnhancements

	try
		{
		if (m_pEnhancements)
			sResult.Append(strPatternSubst(CONSTLIT("m_pEnhancements: %d\r\n"), m_pEnhancements->GetCount()));
		else
			sResult.Append(CONSTLIT("m_pEnhancements: none\r\n"));
		}
	catch (...)
		{
		sResult.Append(strPatternSubst(CONSTLIT("m_pEnhancements: %x [invalid]\r\n"), (DWORD)(CItemEnhancementStack *)m_pEnhancements));
		}

	//	m_pPainter

	try
		{
		if (m_pPainter)
			sResult.Append(strPatternSubst(CONSTLIT("m_pPainter: %s\r\n"), m_pPainter->GetCreator()->GetTypeClassName()));
		else
			sResult.Append(CONSTLIT("m_pPainter: none\r\n"));
		}
	catch (...)
		{
		sResult.Append(strPatternSubst(CONSTLIT("m_pPainter: %x [invalid]\r\n"), (DWORD)m_pPainter));
		}

	//	Objects

	if (m_pHit)
		{
		sResult.Append("m_pHit CSpaceObject\r\n");
		ReportCrashObj(&sResult, m_pHit);
		}

	return sResult;
	}

void CMissile::DetonateNow (CSpaceObject *pHit)

//	DetonateNow
//
//	Detonate the fragmentation missile

	{
	m_fDetonate = true;
	}

CSpaceObject::Categories CMissile::GetCategory (void) const

//	GetCategory
//
//	Returns the category of the object.

	{
	//	We count as a beam if we're type="beam"

	return ((m_pDesc->GetFireType() == CWeaponFireDesc::ftBeam || m_pDesc->GetInteraction() < MIN_MISSILE_INTERACTION) ? catBeam : catMissile);
	}

int CMissile::GetManeuverRate (void) const

//	GetManeuverRate
//
//	Returns maneuver rate (degrees per tick).

	{
	int iRate = m_pDesc->GetManeuverRate();

	if (m_pEnhancements && !m_fFragment)
		iRate = Max(iRate, m_pEnhancements->GetManeuverRate());

	return iRate;
	}

CString CMissile::GetNamePattern (DWORD dwNounPhraseFlags, DWORD *retdwFlags) const

//	GetName
//
//	This is only used if the source has been destroyed

	{
	if (retdwFlags)
		*retdwFlags = nounNoArticle;

	return strPatternSubst(CONSTLIT("%s damage"), GetDamageShortName(m_pDesc->GetDamage().GetDamageType()));
	}

ICCItem *CMissile::GetProperty (CCodeChainCtx &Ctx, const CString &sName)

//	GetProperty
//
//	Returns a property

	{
	CCodeChain &CC = g_pUniverse->GetCC();

	if (strEquals(sName, PROPERTY_LIFE_LEFT))
		return (m_fDestroyOnAnimationDone ? CC.CreateInteger(0) : CC.CreateInteger(m_iLifeLeft));

	else if (strEquals(sName, PROPERTY_ROTATION))
		return CC.CreateInteger(GetRotation());

	else if (strEquals(sName, PROPERTY_SOURCE))
		return ::CreateDamageSource(CC, m_Source);

	else if (strEquals(sName, PROPERTY_TARGET))
		return ::CreateObjPointer(CC, m_pTarget);

	else
		return CSpaceObject::GetProperty(Ctx, sName);
	}

int CMissile::GetStealth (void) const

//	GetStealth
//
//	Returns the stealth of the missile

	{
	return m_pDesc->GetStealth();
	}

bool CMissile::HasAttribute (const CString &sAttribute) const

//	HasAttribute
//
//	Returns TRUE if it has the given attribute

	{
	CItemType *pType = m_pDesc->GetWeaponType();
	if (pType == NULL)
		return false;

	return pType->HasLiteralAttribute(sAttribute);
	}

bool CMissile::IsAngryAt (CSpaceObject *pObj) const

//	IsAngryAt
//
//	Returns TRUE if we should detonate at the given object.

	{
	//	If this is the enemy or if this is our target, then we're done.

	if (pObj == m_pTarget || IsEnemy(pObj))
		return true;

	//	Get the source. If the missile source is NOT the player, then we treat 
	//	non-enemies like friends.

	CSpaceObject *pSource = m_Source.GetObj();
	if (pSource == NULL || !pSource->IsPlayer() || pSource == pObj)
		return false;

	//	If this object is angry at the player, then we are angry at it.
	//
	//	NOTE: We exit above if pSource == pObj, otherwise we might recurse
	//	infinitely below.

	if (pObj->IsAngryAt(pSource))
		return true;

	//	Otherwise, we're known friends. Do not detonate.

	return false;
	}

bool CMissile::IsTracking (void) const

//	IsTracking
//
//	Returns TRUE if we are a tracking missile.

	{
	return (m_pDesc->IsTracking()
			|| (!m_fFragment && m_pEnhancements && m_pEnhancements->IsTracking()));
	}

bool CMissile::IsTrackingTime (int iTick) const

//	IsTrackingTime
//
//	Returns TRUE if we should track this tick.

	{
	return (m_pDesc->IsTrackingTime(iTick)
			|| (!m_fFragment && m_pEnhancements && m_pEnhancements->IsTracking()));
	}

EDamageResults CMissile::OnDamage (SDamageCtx &Ctx)

//	Damage
//
//	Object takes damage from the given source

	{
	DEBUG_TRY

	Ctx.iSectHit = -1;

	//	Short-circuit

	bool bDestroy = false;
	if (Ctx.iDamage == 0)
		return damageNoDamage;

	//	If this is a momentum attack then we are pushed

	int iMomentum;
	if (iMomentum = Ctx.Damage.GetMomentumDamage())
		{
		CVector vAccel = PolarToVector(Ctx.iDirection, -10 * iMomentum * iMomentum);
		Accelerate(vAccel, g_MomentumConstant);
		ClipSpeed(GetMaxSpeed());
		}

	//	Create a hit effect

	Ctx.pDesc->CreateHitEffect(GetSystem(), Ctx);

	//	Check for passthrough. If we pass through then we don't take any damage.

	if (mathRandom(1, 100) <= m_pDesc->GetPassthrough())
		return damageArmorHit;

	//	Take damage

	if (Ctx.iDamage < m_iHitPoints)
		{
		m_iHitPoints -= Ctx.iDamage;
		return damageArmorHit;
		}

	//	We are destroyed

	m_iHitPoints = 0;

#if 0
	//	If the missile has 10+ hp, then we create an effect when
	//	it gets destroyed.

	if (m_pDesc->GetHitPoints() >= 10)
		{
		CEffectCreator *pEffect = g_pUniverse->FindEffectType(g_ExplosionUNID);
		if (pEffect)
			pEffect->CreateEffect(GetSystem(),
					NULL,
					GetPos(),
					CVector(),
					0);
		}
#endif

	//	If we've got a vapor trail, then we stick around until the trail is gone,
	//	but otherwise we're destroyed.

	if (SetMissileFade())
		return damagePassthrough;
	else
		{
		Destroy(killedByDamage, Ctx);
		return damagePassthroughDestroyed;
		}

	DEBUG_CATCH_OBJ(this)
	}

void CMissile::OnDestroyed (SDestroyCtx &Ctx)

//	OnDestroyed
//
//	Missile destroyed

	{
	//	If we're destroyed due to weapon malfunction, then we detonate. This
	//	allows events to detonate a missile on command.

	if (Ctx.iCause == killedByWeaponMalfunction)
		CreateFragments(GetPos());

	//	Fire on destroyed

	m_pDesc->FireOnDestroyShot(this);
	}

void CMissile::OnMove (const CVector &vOldPos, Metric rSeconds)

//	OnMove
//
//	Move our points

	{
	DEBUG_TRY

	//	Update the painter motion
	//	Note that we do this even if we're destroyed because we might have
	//	some fading particles.

	if (m_pPainter
			&& WasPainted())
		{
		SEffectMoveCtx Ctx;
		Ctx.pObj = this;
		Ctx.vOldPos = vOldPos;

		bool bBoundsChanged;
		m_pPainter->OnMove(Ctx, &bBoundsChanged);

		//	Set bounds, if they've changed

		if (bBoundsChanged)
			SetBounds(m_pPainter);
		}

	//	If we're destroyed, then no further processing is needed

	if (m_fDestroyOnAnimationDone)
		{
		//	Clear m_pHit so that we can tell the difference between the single
		//	frame in which we hit and any subsequent frames (when we're painting
		//	the exhaust only).

		m_pHit = NULL;
		return;
		}

	//	Fragments and reflections don't hit test on the first move.

	if (m_iTick <= 1 && (m_pDesc->IsFragment() || m_fReflection))
		{ }

	//	If this is a fragmentation missile, then we need to compute proxity 
	//	rather than just hit testing.

	else if (m_pDesc->ProximityBlast() && m_iTick >= m_pDesc->GetProximityFailsafe())
		{
		//	Compute threshold

		Metric rMaxThreshold = m_pDesc->GetFragmentationMaxThreshold();
		Metric rMinThreshold = m_pDesc->GetFragmentationMinThreshold();

		//	Hit test

		m_pHit = HitTestProximity(vOldPos, rMinThreshold, rMaxThreshold, m_pDesc->GetDamage(), &m_vHitPos, &m_iHitDir);

		//	Make sure we are not too close to the source when we trigger
		//	a proximity blast.

		CSpaceObject *pSource;
		if (m_pHit && m_iHitDir == -1 && (pSource = m_Source.GetObj()))
			{
			CVector vDist = m_vHitPos - pSource->GetPos();
			Metric rDist2 = vDist.Length2();

			if (rDist2 < (rMaxThreshold * rMaxThreshold) / 4.0)
				m_pHit = NULL;
			}
		}

	//	Otherwise, normal hit test

	else
		{
		m_pHit = HitTest(vOldPos, m_pDesc->GetDamage(), &m_vHitPos, &m_iHitDir);
		}

	//	See if we pass through

	if (m_pHit 
			&& m_iHitDir != -1 
			&& m_pDesc->GetPassthrough() > 0
			&& mathRandom(1, 100) <= m_pDesc->GetPassthrough()
			&& m_pHit->GetPassthroughDefault() != damageNoDamageNoPassthrough)
		m_fPassthrough = true;
	else
		m_fPassthrough = false;

	DEBUG_CATCH
	}

void CMissile::ObjectDestroyedHook (const SDestroyCtx &Ctx)

//	ObjectDestroyedHook
//
//	Handle it when another object is destroyed

	{
	//	If our source is destroyed, clear it

	m_Source.OnObjDestroyed(Ctx.pObj);

	if (Ctx.pObj == m_pHit)
		m_pHit = NULL;

	if (Ctx.pObj == m_pTarget)
		m_pTarget = NULL;
	}

void CMissile::OnPaint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	OnPaint
//
//	Paint the station

	{
	//	Paint with painter

	if (m_pPainter)
		{
		CViewportPaintCtxSmartSave Save(Ctx);

		Ctx.iTick = m_iTick;
		Ctx.iVariant = 0;
		Ctx.iRotation = m_iRotation;
		Ctx.iDestiny = GetDestiny();
		Ctx.iMaxLength = (int)((g_SecondsPerUpdate * Max(1, m_iTick) * m_pDesc->GetRatedSpeed()) / g_KlicksPerPixel);

		if (!m_fDestroyOnAnimationDone && (m_pHit == NULL || m_fPassthrough))
			m_pPainter->Paint(Dest, x, y, Ctx);
		else if (m_pHit)
			m_pPainter->PaintHit(Dest, x, y, m_vHitPos, Ctx);
		else if (m_fPainterFade)
			{
			//	When fading, we encode the fade lifetime in m_iHitDir

			Ctx.iStartFade = m_iTick + m_iLifeLeft - m_iHitDir;
			m_pPainter->PaintFade(Dest, x, y, Ctx);
			}

		//	For backwards compatibility, we also paint an image for beams
		//	LATER: We should incorporate this into the painter when we
		//	load the CWeaponFireDesc.

		if (m_pDesc->GetFireType() == CWeaponFireDesc::ftBeam && m_pDesc->GetImage().IsLoaded())
			{
			m_pDesc->GetImage().PaintImage(Dest,
					x,
					y,
					m_iTick,
					0);
			}
		}

	//	Paint image (deprecated method)

	else if (!m_fDestroyOnAnimationDone && m_pHit == NULL)
		{
		m_pDesc->GetImage().PaintImage(Dest,
				x,
				y,
				m_iTick,
				(m_pDesc->IsDirectionalImage() ? Angle2Direction(m_iRotation, g_RotationRange) : 0));

		//	Paint exhaust trail

		if (m_pExhaust)
			{
			int iCount = m_pExhaust->GetCount();

			for (int i = 0; i < iCount; i++)
				{
				int xParticle, yParticle;

				Ctx.XForm.Transform(m_pExhaust->GetAt(i).vPos, &xParticle, &yParticle);
				m_pDesc->GetExhaust().ExhaustImage.PaintImage(Dest, 
						xParticle, 
						yParticle, 
						(iCount - i - 1) * m_pDesc->GetExhaust().iExhaustRate, 
						0);
				}
			}
		}

	//	Paint vapor trail

	if (m_pDesc->GetVaporTrail().iVaporTrailLength)
		{
		int iCount = ComputeVaporTrail();
		int iFadeStep = (VAPOR_TRAIL_OPACITY / m_pDesc->GetVaporTrail().iVaporTrailLength);
		int iFade = Max(0, m_pDesc->GetVaporTrail().iVaporTrailLength - (m_iHitDir - m_iLifeLeft));
		int iOpacity = (!m_fDestroyOnAnimationDone ? VAPOR_TRAIL_OPACITY : (iFadeStep * iFade));

		//	HACK: We store the maximum life left in m_iHitDir after the missile
		//	is destroyed. This saves us from having to do a new variable.
		//
		//	NOTE: We still check to make sure we don't go negative so we don't
		//	overflow the array.

		int iStart = Max(0, (!m_fDestroyOnAnimationDone ? 0 : 1 + (m_iHitDir - m_iLifeLeft)));

		for (int i = iStart; i < iCount; i++)
			{
			m_pVaporTrailRegions[i].Fill(Dest, x, y, CG32bitPixel(m_pDesc->GetVaporTrail().rgbVaporTrailColor, (BYTE)iOpacity));
			iOpacity -= iFadeStep;
			if (iOpacity <= 0)
				break;
			}
		}
	}

void CMissile::OnReadFromStream (SLoadCtx &Ctx)

//	OnReadFromStream
//
//	Read object data from a stream
//
//	CString		CWeaponFireDesc UNID
//	DWORD		m_iHitPoints
//	DWORD		m_iLifeLeft
//	DWORD		m_Source (CSpaceObject ref)
//	DWORD		m_pSovereign (CSovereign ref)
//	DWORD		m_pHit (CSpaceObject ref)
//	Vector		m_vHitPos
//	DWORD		m_iHitDir
//	DWORD		m_iRotation
//	DWORD		m_pTarget (CSpaceObject ref)
//	DWORD		m_iTick
//	DWORD		m_iNextDetonation
//
//	IEffectPainter	m_pPainter
//
//	DWORD		Number of exhaust particles
//	Vector		exhaust: vPos
//	Vector		exhaust: vVel
//
//	-- version > 0 -----------------------
//	DWORD		flags
//	DWORD		Number of saved rotations
//	DWORD		rotation[]
//
//	CItemEnhancementStack	m_pEnhancements

	{
	DWORD dwLoad;

#ifdef DEBUG_LOAD
	::OutputDebugString("CMissile::OnReadFromStream\n");
#endif
	//	Load descriptor

	CString sDescUNID;
	sDescUNID.ReadFromStream(Ctx.pStream);
	m_pDesc = g_pUniverse->FindWeaponFireDesc(sDescUNID);

	//	Old style bonus

	if (Ctx.dwVersion < 92)
		{
		int iBonus;
		Ctx.pStream->Read((char *)&iBonus, sizeof(DWORD));
		if (iBonus != 0)
			{
			m_pEnhancements.TakeHandoff(new CItemEnhancementStack);
			m_pEnhancements->InsertHPBonus(iBonus);
			}
		}

	//	Load other stuff

	if (Ctx.dwVersion >= 18 && Ctx.dwVersion < 137)
		Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));

	if (Ctx.dwVersion >= 28)
		Ctx.pStream->Read((char *)&m_iHitPoints, sizeof(DWORD));
	else
		m_iHitPoints = 0;

	Ctx.pStream->Read((char *)&m_iLifeLeft, sizeof(DWORD));
	m_Source.ReadFromStream(Ctx);
	CSystem::ReadSovereignRefFromStream(Ctx, &m_pSovereign);
	CSystem::ReadObjRefFromStream(Ctx, &m_pHit);
	Ctx.pStream->Read((char *)&m_vHitPos, sizeof(CVector));
	Ctx.pStream->Read((char *)&m_iHitDir, sizeof(DWORD));
	Ctx.pStream->Read((char *)&m_iRotation, sizeof(DWORD));
	CSystem::ReadObjRefFromStream(Ctx, &m_pTarget);
	Ctx.pStream->Read((char *)&m_iTick, sizeof(DWORD));

	if (Ctx.dwVersion >= 140)
		Ctx.pStream->Read((char *)&m_iNextDetonation, sizeof(DWORD));
	else
		m_iNextDetonation = -1;

	//	Load painter

	m_pPainter = CEffectCreator::CreatePainterFromStream(Ctx, (m_pDesc->GetEffect() == NULL));

	//	Load exhaust

	if (m_pDesc->GetExhaust().iExhaustRate > 0)
		{
		int iCount = (m_pDesc->GetExhaust().iExhaustLifetime / m_pDesc->GetExhaust().iExhaustRate) + 1;
		m_pExhaust = new TQueue<SExhaustParticle>(iCount);
		}

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	for (int i = 0; i < (int)dwLoad; i++)
		{
		CVector vPos;
		CVector vVel;
		Ctx.pStream->Read((char *)&vPos, sizeof(CVector));
		Ctx.pStream->Read((char *)&vVel, sizeof(CVector));

		if (m_pExhaust && i < m_pExhaust->GetCapacity())
			{
			SExhaustParticle &Particle = m_pExhaust->EnqueueAndOverwrite();
			Particle.vPos = vPos;
			Particle.vVel = vVel;
			}
		}

	//	Load saved rotations

	if (Ctx.dwVersion >= 1)
		{
		Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
		m_fDestroyOnAnimationDone =		((dwLoad & 0x00000001) ? true : false);
		m_fReflection =		((dwLoad & 0x00000002) ? true : false);
		m_fDetonate =		((dwLoad & 0x00000004) ? true : false);
		m_fPassthrough =	((dwLoad & 0x00000008) ? true : false);
		m_fPainterFade =	((dwLoad & 0x00000010) ? true : false);
		m_fFragment =		((dwLoad & 0x00000020) ? true : false);

		Ctx.pStream->Read((char *)&m_iSavedRotationsCount, sizeof(DWORD));
		if (m_iSavedRotationsCount > 0)
			{
			m_pSavedRotations = new int [Max(m_pDesc->GetVaporTrail().iVaporTrailLength, m_iSavedRotationsCount)];
			Ctx.pStream->Read((char *)m_pSavedRotations, sizeof(DWORD) * m_iSavedRotationsCount);
			}
		else
			m_iSavedRotationsCount = 0;
		}

	//	Enhancements

	if (Ctx.dwVersion >= 92)
		m_pEnhancements = CItemEnhancementStack::ReadFromStream(Ctx);
	}

void CMissile::OnUpdate (SUpdateCtx &Ctx, Metric rSecondsPerTick)

//	OnUpdate
//
//	Update the beam

	{
	DEBUG_TRY

	//	If we're already destroyed, then just update the timer until the
	//	vapor trail fades out

	if (m_fDestroyOnAnimationDone)
		{
		//	Update the painter

		if (m_pPainter 
				&& WasPainted())
			{
			SEffectUpdateCtx PainterCtx;
			PainterCtx.pObj = this;
			PainterCtx.iTick = m_iTick;
			PainterCtx.iRotation = GetRotation();
			PainterCtx.bFade = true;

			m_pPainter->OnUpdate(PainterCtx);

			//	LATER: We shouldn't have to update bounds here because
			//	it is set in OnMove.

			SetBounds(m_pPainter);
			}

		//	Done?

		if (--m_iLifeLeft <= 0)
			{
			Destroy(removedFromSystem, CDamageSource());
			return;
			}
		}

	//	Otherwise, update

	else
		{
		int i;
		CSystem *pSystem = GetSystem();
		int iTick = m_iTick + GetDestiny();
		bool bDestroy = false;

		//	Accelerate, if necessary

        if ((iTick % 10) == 0)
            m_pDesc->ApplyAcceleration(this);

		//	If we can choose new targets, see if we need one now

		if (m_pDesc->CanAutoTarget() && m_pTarget == NULL)
			m_pTarget = GetNearestVisibleEnemy(MAX_TARGET_RANGE);

		//	If this is a tracking missile, change direction to face the target

		if (IsTrackingTime(iTick)
				&& m_pTarget)
			{
			//	Get the position and velocity of the target

			CVector vTarget = m_pTarget->GetPos() - GetPos();

			//	We use the system-relative velocity because we're not adding to
			//	the missile's velocity. That is, we're interested in the 
			//	intercept time at the missile's speed.

			CVector vTargetVel = m_pTarget->GetVel();

			//	Figure out which direction to move in

			int iFireAngle;
			Metric rCurrentSpeed = GetVel().Length();
			Metric rTimeToIntercept = CalcInterceptTime(vTarget, vTargetVel, rCurrentSpeed);
			if (rTimeToIntercept > 0.0)
				{
				CVector vInterceptPoint = vTarget + vTargetVel * rTimeToIntercept;
				iFireAngle = VectorToPolar(vInterceptPoint, NULL);
				}
			else
				iFireAngle = VectorToPolar(vTarget);

			//	Turn to desired direction.

			if (!AreAnglesAligned(iFireAngle, m_iRotation, 1))
				{
				int iTurn = (iFireAngle + 360 - m_iRotation) % 360;

				if (iTurn >= 180)
					{
					int iTurnAngle = Min((360 - iTurn), GetManeuverRate());
					m_iRotation = (m_iRotation + 360 - iTurnAngle) % 360;
					}
				else
					{
					int iTurnAngle = Min(iTurn, GetManeuverRate());
					m_iRotation = (m_iRotation + iTurnAngle) % 360;
					}
				}

			SetVel(PolarToVector(m_iRotation, rCurrentSpeed));
			}

		//	Update exhaust

		if (m_pExhaust)
			{
			if (iTick % m_pDesc->GetExhaust().iExhaustRate)
				{
				SExhaustParticle &New = m_pExhaust->EnqueueAndOverwrite();
				New.vPos = GetPos();
				New.vVel = GetVel();
				}

			for (int i = 0; i < m_pExhaust->GetCount(); i++)
				{
				SExhaustParticle &Particle = m_pExhaust->GetAt(i);
				Particle.vVel = m_pDesc->GetExhaust().rExhaustDrag * Particle.vVel;
				Particle.vPos = Particle.vPos + Particle.vVel * g_SecondsPerUpdate;
				}
			}

		//	Update the painter

		if (m_pPainter
				&& WasPainted())
			{
			SEffectUpdateCtx PainterCtx;
			PainterCtx.pObj = this;
			PainterCtx.iTick = m_iTick;
			PainterCtx.iRotation = GetRotation();

			m_pPainter->OnUpdate(PainterCtx);

			//	LATER: We shouldn't have to update bounds here because
			//	it is set in OnMove.

			SetBounds(m_pPainter);
			}

		//	If we have a vapor trail and need to save rotation, do it

		if (m_pDesc->GetVaporTrail().iVaporTrailLength 
				&& IsTracking())
			{
			//	Compute the current rotation

			int iDirection = (m_iRotation + 180) % 360;

			//	Add the current rotation to the list of saved rotations

			if (m_pSavedRotations == NULL)
				{
				m_pSavedRotations = new int [m_pDesc->GetVaporTrail().iVaporTrailLength];
				m_iSavedRotationsCount = 0;
				}

			int iStart = Min(m_iSavedRotationsCount, m_pDesc->GetVaporTrail().iVaporTrailLength - 1);
			for (i = iStart; i > 0; i--)
				m_pSavedRotations[i] = m_pSavedRotations[i - 1];

			m_pSavedRotations[0] = iDirection;
			if (m_iSavedRotationsCount < m_pDesc->GetVaporTrail().iVaporTrailLength)
				m_iSavedRotationsCount++;
			}

		//	Programmed detonation

		if (m_iNextDetonation != -1 && m_iTick >= m_iNextDetonation)
			{
			CreateFragments(GetPos());

			int iNext;
			if (m_pDesc->HasFragmentInterval(&iNext))
				m_iNextDetonation = m_iTick + iNext;
			else
				bDestroy = true;
			}

		//	If we're set to detonate now, detonate

		else if (m_fDetonate && m_pDesc->ProximityBlast())
			{
			CreateFragments(GetPos());
			bDestroy = true;
			}

		//	If we hit something, then do damage

		else if (m_pHit)
			{
			//	If we have fragments, then explode now

			if (m_pDesc->ProximityBlast()
					&& m_iTick >= m_pDesc->GetProximityFailsafe())
				{
				CreateFragments(m_vHitPos);
				bDestroy = true;
				}

			//	Otherwise, if this was a direct hit, then we do damage

			else if (m_iHitDir != -1)
				{
				SDamageCtx DamageCtx(m_pHit,
						m_pDesc,
						m_pEnhancements,
						m_Source,
						this,
						AngleMod(m_iHitDir + mathRandom(0, 30) - 15),
						m_vHitPos);

				EDamageResults result = m_pHit->Damage(DamageCtx);

				//	If we hit another missile (or some small object) there is a chance
				//	that we continue

				if (result == damagePassthrough || result == damagePassthroughDestroyed)
					{
					m_iHitPoints = m_iHitPoints / 2;
					bDestroy = (m_iHitPoints == 0);
					}

				//	Set the missile to destroy itself after a hit, if we did not
				//	pass through

				else if (!m_fPassthrough)
					bDestroy = true;
				}
			}

		//	See if the missile has faded out

		if (bDestroy || --m_iLifeLeft <= 0)
			{
			//	If this is a fragmentation weapon, then we explode at the end of life

			if (!bDestroy && m_pDesc->ProximityBlast())
				CreateFragments(GetPos());

			//	If the missile should fade, then we leave it alive. Otherwise, 
			//	we destroy the missile.

			if (!SetMissileFade())
				{
				Destroy(removedFromSystem, CDamageSource());
				return;
				}
			}
		}

	m_iTick++;

	DEBUG_CATCH
	}

void CMissile::OnWriteToStream (IWriteStream *pStream)

//	OnWriteToStream
//
//	Write the object's data to stream
//
//	CString		CWeaponFireDesc UNID
//	DWORD		m_iHitPoints
//	DWORD		m_iLifeLeft
//	DWORD		m_Source (CSpaceObject ref)
//	DWORD		m_pSovereign (CSovereign ref)
//	DWORD		m_pHit (CSpaceObject ref)
//	Vector		m_vHitPos
//	DWORD		m_iHitDir
//	DWORD		m_iRotation
//	DWORD		m_pTarget (CSpaceObject ref)
//	DWORD		m_iTick
//	DWORD		m_iNextDetonation
//
//	IEffectPainter	m_pPainter
//
//	DWORD		Number of exhaust particles
//	Vector		exhaust: vPos
//	Vector		exhaust: vVel
//
//	DWORD		flags
//	DWORD		Number of saved rotations
//	DWORD		rotation[]
//
//	CItemEnhancementStack	m_pEnhancements

	{
	DWORD dwSave;

	m_pDesc->GetUNID().WriteToStream(pStream);
	pStream->Write((char *)&m_iHitPoints, sizeof(DWORD));
	pStream->Write((char *)&m_iLifeLeft, sizeof(DWORD));
	m_Source.WriteToStream(GetSystem(), pStream);
	GetSystem()->WriteSovereignRefToStream(m_pSovereign, pStream);
	WriteObjRefToStream(m_pHit, pStream);
	pStream->Write((char *)&m_vHitPos, sizeof(m_vHitPos));
	pStream->Write((char *)&m_iHitDir, sizeof(DWORD));
	pStream->Write((char *)&m_iRotation, sizeof(DWORD));
	WriteObjRefToStream(m_pTarget, pStream);
	pStream->Write((char *)&m_iTick, sizeof(DWORD));
	pStream->Write((char *)&m_iNextDetonation, sizeof(DWORD));

	//	Write effect

	CEffectCreator::WritePainterToStream(pStream, m_pPainter);

	//	Exhaust

	if (m_pExhaust)
		{
		dwSave = m_pExhaust->GetCount();
		pStream->Write((char *)&dwSave, sizeof(DWORD));

		for (int i = 0; i < (int)dwSave; i++)
			{
			SExhaustParticle &Particle = m_pExhaust->GetAt(i);
			pStream->Write((char *)&Particle.vPos, sizeof(Particle.vPos));
			pStream->Write((char *)&Particle.vVel, sizeof(Particle.vVel));
			}
		}
	else
		{
		dwSave = 0;
		pStream->Write((char *)&dwSave, sizeof(DWORD));
		}

	//	Flags

	dwSave = 0;
	dwSave |= (m_fDestroyOnAnimationDone ?	0x00000001 : 0);
	dwSave |= (m_fReflection ?	0x00000002 : 0);
	dwSave |= (m_fDetonate ?	0x00000004 : 0);
	dwSave |= (m_fPassthrough ? 0x00000008 : 0);
	dwSave |= (m_fPainterFade ? 0x00000010 : 0);
	dwSave |= (m_fFragment ?	0x00000020 : 0);
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	//	Saved rotations

	pStream->Write((char *)&m_iSavedRotationsCount, sizeof(DWORD));
	if (m_iSavedRotationsCount)
		pStream->Write((char *)m_pSavedRotations, sizeof(DWORD) * m_iSavedRotationsCount);

	//	Enhancements

	CItemEnhancementStack::WriteToStream(m_pEnhancements, pStream);
	}

void CMissile::PaintLRSForeground (CG32bitImage &Dest, int x, int y, const ViewportTransform &Trans)

//	PaintLRSForeground
//
//	Paints the object on an LRS

	{
	if (!m_fDestroyOnAnimationDone)
		Dest.DrawDot(x, y, 
				CG32bitPixel(255, 255, 0), 
				markerSmallRound);
	}

bool CMissile::PointInObject (const CVector &vObjPos, const CVector &vPointPos) const

//	PointInObject
//
//	Returns TRUE if the given point is inside the object

	{
	DEBUG_TRY

	if (m_fDestroyOnAnimationDone)
		return false;

	//	Figure out the coordinates of vPos relative to the center of the
	//	object, in pixels.

	CVector vOffset = vPointPos - vObjPos;
	int x = (int)((vOffset.GetX() / g_KlicksPerPixel) + 0.5);
	int y = -(int)((vOffset.GetY() / g_KlicksPerPixel) + 0.5);

	if (m_pPainter)
		return m_pPainter->PointInImage(x, y, m_iTick, 0, m_iRotation);
	else
		return m_pDesc->GetImage().PointInImage(x, y, m_iTick, (m_pDesc->IsDirectionalImage() ? Angle2Direction(m_iRotation, g_RotationRange) : 0));

	DEBUG_CATCH
	}

bool CMissile::SetMissileFade (void)

//	SetMissileFade
//
//	Missile is destroyed, but we keep it alive to paint any effects that fade.
//	We return TRUE if the missile should be kept alive.

	{
	//	If we've got a vapor trail effect, then keep the missile object alive
	//	but mark it destroyed

	int iFadeLife = m_pDesc->GetVaporTrail().iVaporTrailLength;

	//	If we've got an effect that needs time to fade out, then keep
	//	the missile object alive

	int iPainterFadeLife;
	if (m_pPainter && (iPainterFadeLife = m_pPainter->GetFadeLifetime(m_iLifeLeft > 0)))
		{
		m_pPainter->OnBeginFade();
		m_fPainterFade = true;
		iFadeLife = Max(iPainterFadeLife, iFadeLife);
		}

	//	If we don't have any fade life then we don't need to keep the missile
	//	alive.

	if (iFadeLife <= 0)
		return false;

	//	Otherwise, mark destroyed and remember fade time

	m_fDestroyOnAnimationDone = true;
	m_iLifeLeft = iFadeLife;

	//	HACK: We use m_iHitDir to remember the maximum fade life (we need this
	//	to calculate how to fade the vapor trail).

	m_iHitDir = iFadeLife;

	return true;
	}

bool CMissile::SetProperty (const CString &sName, ICCItem *pValue, CString *retsError)

//	SetProperty
//
//	Sets an object property

	{
	CCodeChain &CC = g_pUniverse->GetCC();

    if (strEquals(sName, PROPERTY_LIFE_LEFT))
        {
        if (!m_fDestroyOnAnimationDone)
            m_iLifeLeft = Max(0, pValue->GetIntegerValue());
        return true;
        }

	else if (strEquals(sName, PROPERTY_ROTATION))
		{
		m_iRotation = AngleMod(pValue->GetIntegerValue());
		return true;
		}

	else if (strEquals(sName, PROPERTY_SOURCE))
		{
		//	NOTE: CDamageSource handles the case where any objects are destroyed.
		//	so we don't need to check anything here.

		m_Source = ::GetDamageSourceArg(CC, pValue);
		return true;
		}

	else if (strEquals(sName, PROPERTY_TARGET))
		{
		m_pTarget = ::CreateObjFromItem(CC, pValue, CCUTIL_FLAG_CHECK_DESTROYED);
		return true;
		}

	else
		return CSpaceObject::SetProperty(sName, pValue, retsError);
	}
