//	CParticleDamage.cpp
//
//	CParticleDamage class

#include "PreComp.h"

#define LIFETIME_ATTRIB					CONSTLIT("lifetime")

CParticleDamage::CParticleDamage (CUniverse &Universe) : TSpaceObjectImpl(Universe),
		m_pEffectPainter(NULL),
		m_pParticlePainter(NULL)

//	CParticleDamage constructor

	{
	}

CParticleDamage::~CParticleDamage (void)

//	CParticleDamage destructor

	{
	if (m_pEffectPainter)
		m_pEffectPainter->Delete();

	if (m_pParticlePainter)
		m_pParticlePainter->Delete();
	}

ALERROR CParticleDamage::Create (CSystem &System, SShotCreateCtx &Ctx, CParticleDamage **retpObj)

//	Create
//
//	Create the object

	{
	ALERROR error;

	//	We better have a particle description.

	const CParticleSystemDesc *pSystemDesc = Ctx.pDesc->GetParticleSystemDesc();
	ASSERT(pSystemDesc);
	if (pSystemDesc == NULL)
		return ERR_FAIL;

	//	Make sure we have a valid CWeaponFireDesc (otherwise we won't be
	//	able to save the object).

	ASSERT(!Ctx.pDesc->GetUNID().IsBlank());

	//	Create the area

	CParticleDamage *pParticles = new CParticleDamage(System.GetUniverse());
	if (pParticles == NULL)
		return ERR_MEMORY;

	pParticles->Place(Ctx.vPos, Ctx.vVel);

	//	Get notifications when other objects are destroyed
	pParticles->SetObjectDestructionHook();

	//	Set non-linear move, meaning that we are responsible for
	//	setting the position and velocity in OnMove
	pParticles->SetNonLinearMove();

	pParticles->m_pDesc = Ctx.pDesc;
	pParticles->m_pTarget = Ctx.pTarget;
	pParticles->m_pEnhancements = Ctx.pEnhancements;
	pParticles->m_iEmitTime = Max(1, pSystemDesc->GetEmitLifetime().Roll());
	pParticles->m_iLifeLeft = Ctx.pDesc->GetMaxLifetime() + pParticles->m_iEmitTime;
	pParticles->m_iTick = 0;
	pParticles->m_iRotation = Ctx.iDirection;

	pParticles->m_fPainterFade = false;

	//	Keep track of where we emitted particles relative to the source. We
	//	need this so we can continue to emit from this location later.

	pParticles->m_Source = Ctx.Source;
	if (!Ctx.Source.IsEmpty())
		{
		//	Decompose the source position/velocity so that we can continue to
		//	emit later (after the source has changed).
		//
		//	We start by computing the emission offset relative to the source
		//	object when it points at 0 degrees.

		int iSourceRotation = Ctx.Source.GetObj()->GetRotation();
		CVector vPosOffset = (Ctx.vPos - Ctx.Source.GetObj()->GetPos()).Rotate(-iSourceRotation);

		//	Remember these values so we can add them to the new source 
		//	position/velocity.

		pParticles->m_iEmitDirection = Ctx.iDirection - iSourceRotation;
		pParticles->m_vEmitSourcePos = vPosOffset;
		pParticles->m_vEmitSourceVel = CVector();
		}
	else
		{
		pParticles->m_iEmitDirection = Ctx.iDirection;
		pParticles->m_vEmitSourcePos = CVector();
		pParticles->m_vEmitSourceVel = CVector();
		}

	//	Damage

	pParticles->m_iDamage = Ctx.pDesc->GetDamage().RollDamage();

	//	Friendly fire

	if (!Ctx.pDesc->CanHitFriends())
		pParticles->SetNoFriendlyFire();

	//	Create the effect painter, if we've got one

	bool bIsTracking = Ctx.pTarget && Ctx.pDesc->IsTracking();
	pParticles->m_pEffectPainter = Ctx.pDesc->CreateSecondaryPainter(bIsTracking, true);

	//	Particle Painter

	pParticles->m_pParticlePainter = Ctx.pDesc->CreateParticlePainter();

	//	Remember the sovereign of the source (in case the source is destroyed)

	if (Ctx.Source.GetObj())
		pParticles->m_pSovereign = Ctx.Source.GetObj()->GetSovereign();
	else
		pParticles->m_pSovereign = NULL;

	//	Compute the maximum number of particles that we might have

	int iMaxCount = pParticles->m_iEmitTime * pSystemDesc->GetEmitRate().GetMaxValue();
	pParticles->m_Particles.Init(iMaxCount);

	//	NOTE: We use the source velocity (instead of vVel) because Emit expects
	//	to add the particle velocity.

	CVector vInitialVel;
	if (!Ctx.Source.IsEmpty() 
			&& !pParticles->m_pDesc->IsTracking()
			&& !(Ctx.dwFlags & SShotCreateCtx::CWF_FRAGMENT))
		vInitialVel = Ctx.Source.GetObj()->GetVel();

	//	Create the initial particles.

	int iInitCount;
	pParticles->m_Particles.Emit(*pSystemDesc, 
			pParticles,
			Ctx.vPos - pParticles->GetOrigin(), 
			vInitialVel, 
			Ctx.iDirection, 
			0, 
			&iInitCount);

	//	Figure out the number of particles that will cause full damage

	if (pParticles->m_iEmitTime > 1)
		pParticles->m_iParticleCount = pParticles->m_iEmitTime * pSystemDesc->GetEmitRate().GetAveValue();
	else
		pParticles->m_iParticleCount = iInitCount;

	pParticles->m_iParticleCount = Max(1, pParticles->m_iParticleCount);

	//	Add to system

	if (error = pParticles->AddToSystem(System))
		{
		delete pParticles;
		return error;
		}

	//	Done

	if (retpObj)
		*retpObj = pParticles;

	return NOERROR;
	}

CString CParticleDamage::GetNamePattern (DWORD dwNounPhraseFlags, DWORD *retdwFlags) const

//	GetName
//
//	Returns the name of the object

	{
	//	This name is used only if the source has been destroyed

	if (retdwFlags)
		*retdwFlags = 0;
	return CONSTLIT("enemy weapon");
	}

void CParticleDamage::OnDestroyed (SDestroyCtx &Ctx)

//	OnDestroyed
//
//	Shot destroyed

	{
	m_pDesc->FireOnDestroyShot(this);
	}

void CParticleDamage::OnMove (const CVector &vOldPos, Metric rSeconds)

//	OnMove
//
//	Handle moving

	{
	//	Update the painter motion
	//	Note that we do this even if we're destroyed because we might have
	//	some fading particles.

	RECT rcEffectBounds;
	if (m_pEffectPainter
			&& WasPainted())
		{
		SEffectMoveCtx Ctx;
		Ctx.pObj = this;
		Ctx.vOldPos = vOldPos;

		m_pEffectPainter->OnMove(Ctx);

		//	Get the bounds (we need to always get it because we always set it
		//	below).

		m_pEffectPainter->GetBounds(&rcEffectBounds);
		}

	//	If we're destroyed but just waiting for the effect to fade out, then we
	//	update bounds (but otherwise we're done).

	if (m_fPainterFade)
		{
		if (m_pEffectPainter)
			SetBounds(g_KlicksPerPixel * Max(RectWidth(rcEffectBounds), RectHeight(rcEffectBounds)));

		//	Continue moving (otherwise, exhaust trail effects won't work 
		//	properly).

		SetPos(vOldPos + (GetVel() * g_SecondsPerUpdate));
		return;
		}

	//	Update the single particle painter

	if (m_pParticlePainter)
		{
		SEffectMoveCtx Ctx;
		Ctx.pObj = this;

		m_pParticlePainter->OnMove(Ctx);
		}

	//	Update particle motion

	bool bAlive;
	CVector vNewPos;
	m_Particles.UpdateMotionLinear(&bAlive, &vNewPos);

	//	If we're still alive, then set our position and bounds based on the
	//	particles.

	if (bAlive)
		{
		//	Set the position of the object based on the average particle position

		SetPos(vNewPos);

		//	Set the bounds (note, we make the bounds twice as large to deal
		//	with the fact that we're moving).

		RECT rcBounds = m_Particles.GetBounds();
		if (m_pEffectPainter)
			::UnionRect(&rcBounds, &rcBounds, &rcEffectBounds);

		SetBounds(g_KlicksPerPixel * Max(RectWidth(rcBounds), RectHeight(rcBounds)));
		}

	//	If we stay around for some fading effects, then set it up.

	else if (SetMissileFade())
		{
		if (m_pEffectPainter)
			SetBounds(g_KlicksPerPixel * Max(RectWidth(rcEffectBounds), RectHeight(rcEffectBounds)));
		}

	//	Otherwise, we're dead

	else
		Destroy(removedFromSystem, CDamageSource());
	}

void CParticleDamage::ObjectDestroyedHook (const SDestroyCtx &Ctx)

//	ObjectDestroyedHook
//
//	Called when another object is destroyed

	{
	m_Source.OnObjDestroyed(Ctx.pObj);

	if (Ctx.pObj == m_pTarget)
		m_pTarget = NULL;
	}

void CParticleDamage::OnPaint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	OnPaint
//
//	Paint

	{
	const CParticleSystemDesc *pSystemDesc = m_pDesc->GetParticleSystemDesc();
	ASSERT(pSystemDesc);
	if (pSystemDesc == NULL)
		return;

	CViewportPaintCtxSmartSave Save(Ctx);
	Ctx.iTick = m_iTick;
	Ctx.iVariant = 0;
	Ctx.iRotation = 0;
	Ctx.iDestiny = GetDestiny();
	Ctx.iMaxLength = (int)((g_SecondsPerUpdate * Max(1, m_iTick) * m_pDesc->GetRatedSpeed()) / g_KlicksPerPixel);

	//	Start by painting the secondary effect, if any

	if (m_pEffectPainter)
		{
		Ctx.iRotation = m_iRotation;

		if (m_fPainterFade)
			m_pEffectPainter->PaintFade(Dest, x, y, Ctx);
		else
			m_pEffectPainter->Paint(Dest, x, y, Ctx);

		Ctx.iRotation = 0;
		}

	//	If we're just fading out, then we're done

	if (m_fPainterFade)
		return;

	//	Painting is relative to the origin

	int xOrigin, yOrigin;
	Ctx.XForm.Transform(GetOrigin(), &xOrigin, &yOrigin);

	//	If we can get a paint descriptor, use that because it is faster

	m_Particles.Paint(*pSystemDesc, Dest, xOrigin, yOrigin, m_pParticlePainter, Ctx);
	}

void CParticleDamage::OnReadFromStream (SLoadCtx &Ctx)

//	OnReadFromStream
//
//	Restore from stream

	{
	DWORD dwLoad;

#ifdef DEBUG_LOAD
	::OutputDebugString("CParticleDamage::OnReadFromStream\n");
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

	Ctx.pStream->Read((char *)&m_iLifeLeft, sizeof(m_iLifeLeft));
	m_Source.ReadFromStream(Ctx);
	CSystem::ReadSovereignRefFromStream(Ctx, &m_pSovereign);
	Ctx.pStream->Read((char *)&m_iTick, sizeof(m_iTick));

	if (Ctx.dwVersion >= 120)
		Ctx.pStream->Read((char *)&m_iRotation, sizeof(DWORD));
	else
		m_iRotation = VectorToPolar(GetVel());

	Ctx.pStream->Read((char *)&m_iDamage, sizeof(m_iDamage));
	if (Ctx.dwVersion >= 3 && Ctx.dwVersion < 67)
		{
		CVector vDummy;
		Ctx.pStream->Read((char *)&vDummy, sizeof(CVector));
		}

	//	The newer version uses a different particle array

	if (Ctx.dwVersion >= 21)
		{
		Ctx.pStream->Read((char *)&m_vEmitSourcePos, sizeof(CVector));
		Ctx.pStream->Read((char *)&m_vEmitSourceVel, sizeof(CVector));
		Ctx.pStream->Read((char *)&m_iEmitDirection, sizeof(DWORD));
		Ctx.pStream->Read((char *)&m_iEmitTime, sizeof(DWORD));
		Ctx.pStream->Read((char *)&m_iParticleCount, sizeof(DWORD));

		//	Load painter

		m_pParticlePainter = CEffectCreator::CreatePainterFromStreamAndCreator(Ctx, m_pDesc->GetParticleEffect());

		m_Particles.ReadFromStream(Ctx);
		}

	//	Read the previous version, but no need to convert

	else
		{
		DWORD dwCount;
		Ctx.pStream->Read((char *)&dwCount, sizeof(DWORD));
		if (dwCount > 0)
			{
			char *pDummy = new char [5 * sizeof(DWORD) * dwCount];
			Ctx.pStream->Read(pDummy, 5 * sizeof(DWORD) * dwCount);
			delete pDummy;
			}

		m_iEmitTime = 0;
		m_iEmitDirection = -1;
		}

	//	Read the target

	if (Ctx.dwVersion >= 67)
		CSystem::ReadObjRefFromStream(Ctx, &m_pTarget);
	else
		m_pTarget = NULL;

	//	For now we don't bother saving the effect painter

	m_pEffectPainter = NULL;

	//	Enhancements

	if (Ctx.dwVersion >= 92)
		m_pEnhancements = CItemEnhancementStack::ReadFromStream(Ctx);

	//	Flags

	DWORD dwFlags = 0;
	if (Ctx.dwVersion >= 120)
		Ctx.pStream->Read((char *)&dwFlags, sizeof(DWORD));

	m_fPainterFade =	((dwFlags & 0x00000010) ? true : false);
	}

void CParticleDamage::OnUpdate (SUpdateCtx &Ctx, Metric rSecondsPerTick)

//	OnUpdate
//
//	Update

	{
	DEBUG_TRY

	const CParticleSystemDesc *pSystemDesc = m_pDesc->GetParticleSystemDesc();
	ASSERT(pSystemDesc);
	if (pSystemDesc == NULL)
		return;

	m_iTick++;

	//	Set up context block for particle array update

	SEffectUpdateCtx EffectCtx;
	EffectCtx.pSystem = GetSystem();
	EffectCtx.pObj = this;
	EffectCtx.iTick = m_iTick;

	EffectCtx.pDamageDesc = m_pDesc;
	EffectCtx.iTotalParticleCount = m_iParticleCount;
	EffectCtx.pEnhancements = m_pEnhancements;
	EffectCtx.iCause = m_Source.GetCause();
	EffectCtx.Attacker = m_Source;
	EffectCtx.pTarget = m_pTarget;

	//	Update the effect painter

	if (m_pEffectPainter 
			&& WasPainted())
		{
		SEffectUpdateCtx PainterCtx;
		PainterCtx.pObj = this;
		PainterCtx.iTick = m_iTick;
		PainterCtx.bFade = m_fPainterFade;

		m_pEffectPainter->OnUpdate(PainterCtx);
		}

	//	If we're fading, then nothing else to do

	if (m_fPainterFade)
		{
		if (--m_iLifeLeft <= 0)
			Destroy(removedFromSystem, CDamageSource());

		return;
		}

	//	Update the single particle painter

	if (m_pParticlePainter)
		m_pParticlePainter->OnUpdate();

	//	Update (includes doing damage)

	m_Particles.Update(*pSystemDesc, EffectCtx);

	//	Expired?

	if (--m_iLifeLeft <= 0)
		{
		if (SetMissileFade())
			{ }
		else
			Destroy(removedFromSystem, CDamageSource());

		return;
		}

	//	Emit new particles

	if (m_iTick < m_iEmitTime && !m_Source.IsEmpty())
		{
		//	Rotate the offsets appropriately

		int iRotation = m_Source.GetObj()->GetRotation();
		CVector vPos = m_vEmitSourcePos.Rotate(iRotation);
		CVector vVel = m_vEmitSourceVel.Rotate(iRotation);

		//	Emit

		m_Particles.Emit(*pSystemDesc, 
				this,
				m_Source.GetObj()->GetPos() + vPos - GetOrigin(), 
				m_Source.GetObj()->GetVel() + vVel,
				AngleMod(iRotation + m_iEmitDirection),
				m_iTick);
		}

	DEBUG_CATCH
	}

void CParticleDamage::OnWriteToStream (IWriteStream *pStream)

//	OnWriteToStream
//
//	Write out to stream
//
//	CString			CWeaponFireDesc UNID
//	DWORD			m_iLifeLeft
//	DWORD			m_Source (CSpaceObject ref)
//	DWORD			m_pSovereign (CSovereign ref)
//	DWORD			m_iTick
//	DWORD			m_iRotation
//	DWORD			m_iDamage
//
//	CVector			m_vEmitSourcePos
//	CVector			m_vEmitSourceVel
//	DWORD			m_iEmitDirection
//	DWORD			m_iEmitTime
//	DWORD			m_iParticleCount
//	IEffectPainter
//	CParticleArray
//
//	CSpaceObject	m_pTarget
//
//	CItemEnhancementStack	m_pEnhancements
//
//	DWORD			Flags

	{
	DWORD dwSave;
	m_pDesc->GetUNID().WriteToStream(pStream);
	pStream->Write((char *)&m_iLifeLeft, sizeof(m_iLifeLeft));
	m_Source.WriteToStream(GetSystem(), pStream);
	GetSystem()->WriteSovereignRefToStream(m_pSovereign, pStream);
	pStream->Write((char *)&m_iTick, sizeof(m_iTick));
	pStream->Write((char *)&m_iRotation, sizeof(m_iRotation));
	pStream->Write((char *)&m_iDamage, sizeof(m_iDamage));
	pStream->Write((char *)&m_vEmitSourcePos, sizeof(CVector));
	pStream->Write((char *)&m_vEmitSourceVel, sizeof(CVector));
	pStream->Write((char *)&m_iEmitDirection, sizeof(DWORD));
	pStream->Write((char *)&m_iEmitTime, sizeof(DWORD));
	pStream->Write((char *)&m_iParticleCount, sizeof(DWORD));

	CEffectCreator::WritePainterToStream(pStream, m_pParticlePainter);

	m_Particles.WriteToStream(pStream);

	WriteObjRefToStream(m_pTarget, pStream);

	//	Enhancements

	CItemEnhancementStack::WriteToStream(m_pEnhancements, pStream);

	//	Flags

	dwSave = 0;
	dwSave |= (m_fPainterFade ? 0x00000001 : 0);
	pStream->Write((char *)&dwSave, sizeof(DWORD));
	}

bool CParticleDamage::PointInObject (const CVector &vObjPos, const CVector &vPointPos) const

//	PointInObject
//
//	Returns TRUE if the given point is in the object

	{
	return false;
	}

bool CParticleDamage::SetMissileFade (void)

//	SetMissileFade
//
//	Missile is destroyed, but we keep it alive to paint any effects that fade.
//	We return TRUE if the missile should be kept alive.

	{
	//	If we've got an effect that needs time to fade out, then keep
	//	the missile object alive

	int iPainterFadeLife;
	if (m_pEffectPainter && (iPainterFadeLife = m_pEffectPainter->GetFadeLifetime(m_iLifeLeft > 0)))
		{
		m_pEffectPainter->OnBeginFade();
		m_fPainterFade = true;
		m_iLifeLeft = iPainterFadeLife;
		return true;
		}

	//	Otherwise, we don't stay alive

	return false;
	}
