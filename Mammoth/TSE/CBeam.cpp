//	CBeam.cpp
//
//	CBeam class

#include "PreComp.h"

struct SBeamDrawCtx
	{
	int xFrom;
	int yFrom;
	int xTo;
	int yTo;

	int iIntensity;
	CG32bitPixel rgbBackgroundColor;
	CG32bitPixel rgbPrimaryColor;
	CG32bitPixel rgbSecondaryColor;
	};

void DrawLaserBeam (CG32bitImage &Dest, const SBeamDrawCtx &Ctx);

CBeam::CBeam (CUniverse &Universe) : TSpaceObjectImpl(Universe),
//		m_xPaintFrom(0),
//		m_yPaintFrom(0),
		m_pHit(NULL),
		m_dwSpareFlags(0),
		m_iBonus(0)

//	CBeam constructor

	{
	}

void CBeam::ComputeOffsets (void)

//	ComputeOffsets
//
//	Computes offsets

	{
	Metric rLength = LIGHT_SPEED * g_SecondsPerUpdate / g_KlicksPerPixel;
	CVector vFrom = PolarToVector(m_iRotation, -rLength);

	m_xFromOffset = mathRound(vFrom.GetX());
	m_yFromOffset = -mathRound(vFrom.GetY());
	m_xToOffset = 0;
	m_yToOffset = 0;
	}

void CBeam::CreateReflection (const CVector &vPos, int iDirection, CMissile **retpReflection)

//	CreateReflection
//
//	Creates a new object that is a clone of this one but moving
//	in the given direction.

	{
	}

Metric CBeam::GetAge (void) const

//	GetAge
//
//	Returns the age of the shot as a fraction of its lifetime (0-1.0).

	{
	int iTotalLife = m_iTick + m_iLifeLeft;
	if (iTotalLife > 0)
		return (Metric)m_iTick / (Metric)iTotalLife;
	else
		return 0.0;
	}

CString CBeam::GetNamePattern (DWORD dwNounPhraseFlags, DWORD *retdwFlags) const

//	GetName
//
//	This is only used if the source has been destroyed

	{
	if (retdwFlags)
		*retdwFlags = 0;

	switch (m_pDesc->GetDamage().GetDamageType())
		{
		case damageLaser:
			return CONSTLIT("laser blast");

		case damageParticle:
			return CONSTLIT("particle beam");

		case damageIonRadiation:
			return CONSTLIT("ion beam");

		case damagePositron:
			return CONSTLIT("positron beam");

		case damageGravitonBeam:
			return CONSTLIT("graviton beam");

		default:
			return CONSTLIT("beam"); 
		}
	}

void CBeam::OnMove (SUpdateCtx &Ctx, const CVector &vOldPos, Metric rSeconds)

//	OnMove
//
//	Move our points

	{
	//	See if the beam hit anything after the move

	if (!m_fReflection || m_iTick > 1)
		m_pHit = HitTest(vOldPos, m_pDesc->GetDamage(), &m_vPaintTo, &m_iHitDir);

	if (m_pHit == NULL)
		m_vPaintTo = GetPos();
	}

void CBeam::ObjectDestroyedHook (const SDestroyCtx &Ctx)

//	ObjectDestroyedHook
//
//	Handle it when another object is destroyed

	{
	//	If our source is destroyed, clear it

	m_Source.OnObjDestroyed(Ctx.Obj);

	if (Ctx.Obj == m_pHit)
		m_pHit = NULL;
	}

void CBeam::OnPaint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	OnPaint
//
//	Paint the beam

	{
	//	Setup structure

	SBeamDrawCtx BeamCtx;
	BeamCtx.xFrom = x + m_xFromOffset;
	BeamCtx.yFrom = y + m_yFromOffset;
	if (m_pHit)
		Ctx.XForm.Transform(m_vPaintTo, &BeamCtx.xTo, &BeamCtx.yTo);
	else
		{
		BeamCtx.xTo = x + m_xToOffset;
		BeamCtx.yTo = y + m_yToOffset;
		}
	BeamCtx.iIntensity = 1;
	BeamCtx.rgbBackgroundColor = CG32bitPixel::Null();
	BeamCtx.rgbPrimaryColor = CG32bitPixel(0xf1, 0x5f, 0x2a);
	BeamCtx.rgbSecondaryColor = CG32bitPixel(0xff, 0x00, 0x00);

	//	Paint a line

	DrawLaserBeam(Dest, BeamCtx);

	//	Draw the head of the beam if we have an image

	if (m_pDesc->GetImage().IsLoaded())
		{
		m_pDesc->GetImage().PaintImage(Dest,
				BeamCtx.xTo,
				BeamCtx.yTo,
				m_iTick,
				0);
		}
	}

void CBeam::OnReadFromStream (SLoadCtx &Ctx)

//	OnReadFromStream
//
//	Read object data from a stream
//
//	CString		CWeaponFireDesc UNID
//	DWORD		m_iBonus
//	DWORD		m_iRotation
//	Vector		m_vPaintTo
//	DWORD		m_iTick
//	DWORD		m_iLifeLeft
//	DWORD		m_Source (CSpaceObject Ref)
//	DWORD		m_pSovereign (UNID)
//	DWORD		m_pHit (CSpaceObject Ref)
//	DWORD		m_iHitDir

	{
	DWORD dwLoad;

#ifdef DEBUG_LOAD
	::OutputDebugString("CBeam::OnReadFromStream\n");
#endif
	//	Load descriptor

	CString sDescUNID;
	sDescUNID.ReadFromStream(Ctx.pStream);
	m_pDesc = Ctx.GetUniverse().FindWeaponFireDesc(sDescUNID);

	Ctx.pStream->Read((char *)&m_iBonus, sizeof(DWORD));
	if (Ctx.dwVersion >= 18 && Ctx.dwVersion < 137)
		Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));

	Ctx.pStream->Read((char *)&m_iRotation, sizeof(DWORD));
	Ctx.pStream->Read((char *)&m_vPaintTo, sizeof(CVector));
	Ctx.pStream->Read((char *)&m_iTick, sizeof(DWORD));
	Ctx.pStream->Read((char *)&m_iLifeLeft, sizeof(m_iLifeLeft));
	m_Source.ReadFromStream(Ctx);
	CSystem::ReadSovereignRefFromStream(Ctx, &m_pSovereign);
	CSystem::ReadObjRefFromStream(Ctx, &m_pHit);
	Ctx.pStream->Read((char *)&m_iHitDir, sizeof(DWORD));

	ComputeOffsets();
	}

void CBeam::OnUpdate (SUpdateCtx &Ctx, Metric rSecondsPerTick)

//	OnUpdate
//
//	Update the beam

	{
	DEBUG_TRY

	bool bDestroy = false;

	m_iTick++;

	//	See if the beam hit anything

	if (m_pHit)
		{
		//	Old-style bonus

		TSharedPtr<CItemEnhancementStack> pEnhancements(new CItemEnhancementStack);
		pEnhancements->InsertHPBonus(NULL, m_iBonus);

		//	Tell the object hit that it has been damaged

		SDamageCtx DamageCtx(m_pHit,
				*m_pDesc,
				pEnhancements,
				m_Source,
				this,
				GetAge(),
				AngleMod(m_iHitDir + mathRandom(0, 30) - 15),
				m_vPaintTo);

		EDamageResults result = m_pHit->Damage(DamageCtx);

		//	Set the beam to destroy itself after a hit

		if (m_pDesc->GetPassthrough() == 0
				|| result == damageNoDamage 
				|| result == damageAbsorbedByShields
				|| mathRandom(1, 100) > m_pDesc->GetPassthrough())
			bDestroy = true;
		}

	//	See if the beam has faded out

	if (bDestroy || --m_iLifeLeft <= 0)
		Destroy(removedFromSystem, CDamageSource());

	DEBUG_CATCH
	}

void CBeam::OnWriteToStream (IWriteStream *pStream)

//	OnWriteToStream
//
//	Write the object's data to stream
//
//	CString		CWeaponFireDesc UNID
//	DWORD		m_iBonus
//	DWORD		m_iRotation
//	Vector		m_vPaintTo
//	DWORD		m_iTick
//	DWORD		m_iLifeLeft
//	DWORD		m_Source (CSpaceObject Ref)
//	DWORD		m_pSovereign (UNID)
//	DWORD		m_pHit (CSpaceObject Ref)
//	DWORD		m_iHitDir

	{
	m_pDesc->GetUNID().WriteToStream(pStream);
	pStream->Write((char *)&m_iBonus, sizeof(DWORD));
	pStream->Write((char *)&m_iRotation, sizeof(DWORD));
	pStream->Write((char *)&m_vPaintTo, sizeof(m_vPaintTo));
	pStream->Write((char *)&m_iTick, sizeof(DWORD));
	pStream->Write((char *)&m_iLifeLeft, sizeof(m_iLifeLeft));
	m_Source.WriteToStream(pStream);
	CSystem::WriteSovereignRefToStream(m_pSovereign, pStream);
	WriteObjRefToStream(m_pHit, pStream);
	pStream->Write((char *)&m_iHitDir, sizeof(DWORD));
	}

//	Beam Drawing Routines -----------------------------------------------------

void DrawLaserBeam (CG32bitImage &Dest, const SBeamDrawCtx &Ctx)

//	DrawLaserBeam
//
//	Draws a simple laser beam

	{
	CG32bitPixel rgbGlow = CG32bitPixel(Ctx.rgbSecondaryColor, 100);

	Dest.DrawLine(Ctx.xFrom, Ctx.yFrom,
			Ctx.xTo, Ctx.yTo,
			3,
			rgbGlow);

	Dest.DrawLine(Ctx.xFrom, Ctx.yFrom,
			Ctx.xTo, Ctx.yTo,
			1,
			Ctx.rgbPrimaryColor);
	}
