//	CContinuousBeam.cpp
//
//	CContinuousBeam class

#include "PreComp.h"

static CObjectClass<CContinuousBeam>g_Class(OBJID_CCONTINUOUSBEAM, NULL);

CContinuousBeam::CContinuousBeam (void) : CSpaceObject(&g_Class),
		m_pEnhancements(NULL),
		m_pEffectPainter(NULL)

//	CContinuousBeam constructor

	{
	}

CContinuousBeam::~CContinuousBeam (void)

//	CContinuousBeam destructor

	{
	if (m_pEffectPainter)
		m_pEffectPainter->Delete();

	if (m_pEnhancements)
		m_pEnhancements->Delete();
	}

void CContinuousBeam::AddContinuousBeam (const CVector &vPos, const CVector &vVel, int iDirection)

//	AddContinousBeam
//
//	Adds another segment to the beam

	{
	if (m_pDesc == NULL)
		return;

	//	Separate the source velocity from the beam

	CVector vSourceVel = (m_Source.GetObj() ? m_Source.GetObj()->GetVel() : NullVector);
	CVector vVelRel = vVel - vSourceVel;

	//	We either add a new segment or replace the last pseudo segment.

	SSegment *pNewSegment;
	if (m_Segments.GetCount() == 0)
		pNewSegment = m_Segments.Insert();
	else
		pNewSegment = &m_Segments[m_Segments.GetCount() - 1];

	pNewSegment->vPos = vPos;
	pNewSegment->vDeltaPos = (vVelRel + vSourceVel) * g_SecondsPerUpdate;
	pNewSegment->dwGeneration = g_pUniverse->GetTicks();
	pNewSegment->iDamage = m_pDesc->GetDamage().RollDamage();
	pNewSegment->fAlive = true;
	pNewSegment->fHit = false;

	//	Now add a pseudo segment.

	SSegment *pPseudoSegment = m_Segments.Insert();

	//	We want the beam to originate at the source, so the pseudo segment
	//	is effectively 1 frame behind. We want the pseudo frame to be
	//	where the source is on the NEXT frame, given the beam velocity.

	pPseudoSegment->vPos = vPos - (vVelRel * g_SecondsPerUpdate);
	pPseudoSegment->vDeltaPos = pNewSegment->vDeltaPos;

	//	Generate and damage don't matter for the pseudo frame.

	pPseudoSegment->dwGeneration = 0;
	pPseudoSegment->iDamage = 0;

	//	Pseudo segment is always dead (but even dead segments
	//	get their position updated).

	pPseudoSegment->fAlive = false;
	}

void CContinuousBeam::AddSegment (const CVector &vPos, const CVector &vVel, int iDamage)

//	AddSegment
//
//	Adds a new segment

	{
	//	We either add a new segment or replace the last pseudo segment.

	SSegment *pNewSegment;
	if (m_Segments.GetCount() == 0)
		pNewSegment = m_Segments.Insert();
	else
		pNewSegment = &m_Segments[m_Segments.GetCount() - 1];

	pNewSegment->vPos = vPos;
	pNewSegment->vDeltaPos = vVel * g_SecondsPerUpdate;
	pNewSegment->dwGeneration = g_pUniverse->GetTicks();
	pNewSegment->iDamage = iDamage;
	pNewSegment->fAlive = true;
	pNewSegment->fHit = false;

	//	Now add a pseudo segment.

	SSegment *pPseudoSegment = m_Segments.Insert();
	pPseudoSegment->vPos = vPos - pNewSegment->vDeltaPos;
	pPseudoSegment->vDeltaPos = pNewSegment->vDeltaPos;
	pPseudoSegment->dwGeneration = pNewSegment->dwGeneration;
	pPseudoSegment->iDamage = 0;

	//	Pseudo segment is always dead (but even dead segments
	//	get their position updated).

	pPseudoSegment->fAlive = false;
	}

ALERROR CContinuousBeam::Create (CSystem *pSystem,
								 CWeaponFireDesc *pDesc,
								 CItemEnhancementStack *pEnhancements,
								 const CDamageSource &Source,
								 const CVector &vPos,
								 const CVector &vVel,
								 int iDirection,
								 CSpaceObject *pTarget,
								 CContinuousBeam **retpObj)

//	Create
//
//	Create the object

	{
	ALERROR error;

	//	Make sure we have a valid CWeaponFireDesc (otherwise we won't be
	//	able to save the object).

	ASSERT(!pDesc->GetUNID().IsBlank());

	//	Create the object

	CContinuousBeam *pBeam = new CContinuousBeam;
	if (pBeam == NULL)
		return ERR_MEMORY;

	pBeam->Place(vPos, CVector());

	//	Get notifications when other objects are destroyed
	pBeam->SetObjectDestructionHook();

	//	Set non-linear move, meaning that we are responsible for
	//	setting the position and velocity in OnMove
	pBeam->SetNonLinearMove();

	pBeam->m_pDesc = pDesc;
	pBeam->m_pTarget = pTarget;
	pBeam->m_pEnhancements = (pEnhancements ? pEnhancements->AddRef() : NULL);
	pBeam->m_iLifetime = pDesc->GetLifetime();
	pBeam->m_iTick = 0;
	pBeam->m_iLastDirection = iDirection;
	pBeam->m_Source = Source;

	//	Friendly fire

	if (!pDesc->CanHitFriends())
		pBeam->SetNoFriendlyFire();

	//	Create the effect painter, if we've got one

	bool bIsTracking = pTarget && pDesc->IsTracking();
	pBeam->m_pEffectPainter = pDesc->CreateEffectPainter(bIsTracking, true);

	//	Remember the sovereign of the source (in case the source is destroyed)

	if (Source.GetObj())
		pBeam->m_pSovereign = Source.GetObj()->GetSovereign();
	else
		pBeam->m_pSovereign = NULL;

	//	Add a segment

	pBeam->AddContinuousBeam(vPos, vVel, iDirection);

	//	Add to system

	if (error = pBeam->AddToSystem(pSystem))
		{
		delete pBeam;
		return error;
		}

	//	Done

	if (retpObj)
		*retpObj = pBeam;

	return NOERROR;
	}

EDamageResults CContinuousBeam::DoDamage (CSpaceObject *pHit, const CVector &vHitPos, int iHitDir, int iDamage)

//	DoDamage
//
//	Do damage to the given object

	{
	SDamageCtx DamageCtx;
	DamageCtx.pObj = pHit;
	DamageCtx.pDesc = m_pDesc;
	DamageCtx.Damage = m_pDesc->GetDamage();
	DamageCtx.Damage.AddEnhancements(m_pEnhancements);
	DamageCtx.Damage.SetCause(m_Source.GetCause());
	if (IsAutomatedWeapon())
		DamageCtx.Damage.SetAutomatedWeapon();
	DamageCtx.iDirection = AngleMod(iHitDir + mathRandom(0, 30) - 15);
	DamageCtx.vHitPos = vHitPos;
	DamageCtx.pCause = this;
	DamageCtx.Attacker = m_Source;

	return pHit->Damage(DamageCtx);
	}

CString CContinuousBeam::GetName (DWORD *retdwFlags)

//	GetName
//
//	Returns the name of the object

	{
	//	This name is used only if the source has been destroyed

	if (retdwFlags)
		*retdwFlags = 0;
	return CONSTLIT("enemy weapon");
	}

bool CContinuousBeam::HitTestSegment (const CVector &vPos, CVector &vNewPos, CSpaceObject **retpHit, int *retiHitDir)

//	HitTestSegment
//
//	If we hit an object, returns the object hit and the new position of the
//	segment (which is the hit position).

	{
	int iInteraction = m_pDesc->GetInteraction();

	//	Get our bounds

	CVector vLL = vPos;
	CVector vUR = vPos;
	CGeometry::AccumulateBounds(vNewPos, vLL, vUR);

	//	Compute the segment length (we need this for stepping and for bounds)

	CVector vDiff = (vNewPos - vPos);
	Metric rLength = vDiff.Length();
	if (rLength == 0.0)
		return false;

	Metric rStep = 3.0 * g_KlicksPerPixel;
	CVector vStep = (rStep / rLength) * vDiff;

	//	Get the list of objects that intersect the object

	SSpaceObjectGridEnumerator i;
	GetSystem()->EnumObjectsInBoxStart(i, vUR, vLL);

	//	Track the best (nearest) object that we hit

	CSpaceObject *pHit = NULL;
	CVector vBestHit;
	Metric rBestDist = rLength;

	//	See if the beam hit anything. We start with a crude first pass.
	//	Any objects near the beam are then analyzed further to see if
	//	the beam hit them.

	while (GetSystem()->EnumObjectsInBoxHasMore(i))
		{
		CSpaceObject *pObj = GetSystem()->EnumObjectsInBoxGetNext(i);
		if (!CanHit(pObj)
				|| !pObj->CanBeHitBy(m_pDesc->GetDamage())
				|| !pObj->InteractsWith(iInteraction)
				|| pObj == this)
			continue;

		//	See where we hit this object (if at all)

		Metric rTest = 0.0;
		CVector vTest = vPos;
		while (rTest < rBestDist)
			{
			if (pObj->PointInObject(pObj->GetPos(), vTest))
				{
				pHit = pObj;
				vBestHit = vTest;
				rBestDist = rTest;
				break;
				}

			rTest += rStep;
			vTest = vTest + vStep;
			}
		}

	//	Done

	if (pHit == NULL)
		return false;

	vNewPos = vBestHit;
	*retpHit = pHit;
	*retiHitDir = AngleMod(180 + VectorToPolar(vStep));

	return true;
	}

void CContinuousBeam::OnDestroyed (SDestroyCtx &Ctx)

//	OnDestroyed
//
//	Shot destroyed

	{
	m_pDesc->FireOnDestroyShot(this);
	}

void CContinuousBeam::OnMove (const CVector &vOldPos, Metric rSeconds)

//	OnMove
//
//	Handle moving

	{
	//	Update the effect painter

	if (m_pEffectPainter)
		{
		SEffectMoveCtx Ctx;
		Ctx.pObj = this;

		m_pEffectPainter->OnMove(Ctx);
		}

	//	Update segment motion

	CVector vNewPos;
	Metric rMaxBoundsX;
	Metric rMaxBoundsY;
	UpdateBeamMotion(rSeconds, &vNewPos, &rMaxBoundsX, &rMaxBoundsY);

	//	Set bounds and position

	SetPos(vNewPos);
	SetBounds(rMaxBoundsX, rMaxBoundsY);
	}

void CContinuousBeam::ObjectDestroyedHook (const SDestroyCtx &Ctx)

//	ObjectDestroyedHook
//
//	Called when another object is destroyed

	{
	m_Source.OnObjDestroyed(Ctx.pObj);

	if (Ctx.pObj == m_pTarget)
		m_pTarget = NULL;
	}

void CContinuousBeam::OnPaint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	OnPaint
//
//	Paint

	{
	int i;

	CViewportPaintCtxSmartSave Save(Ctx);
	Ctx.iTick = m_iTick;
	Ctx.iVariant = 0;
	Ctx.iRotation = m_iLastDirection;
	Ctx.iDestiny = GetDestiny();
	Ctx.iMaxLength = -1;

	//	Keep painting segments live segments

	bool bFoundHead = false;
	CVector vHead;
	for (i = 0; i < m_Segments.GetCount(); i++)
		{
		SSegment &Segment = m_Segments[i];

		//	If we haven't year found the head, and this segment is alive, then we just
		//	found it.

		if (!bFoundHead)
			{
			if (Segment.fAlive)
				{
				vHead = Segment.vPos;
				bFoundHead = true;
				}
			}

		//	Otherwise, if we found a dead segment (remember the pseudo segment is always
		//	dead) then we paint a line.

		else if (!Segment.fAlive)
			{
			m_pEffectPainter->PaintLine(Dest, vHead, Segment.vPos, Ctx);
			bFoundHead = false;
			}
		}
	}

void CContinuousBeam::OnReadFromStream (SLoadCtx &Ctx)

//	OnReadFromStream
//
//	CString			CWeaponFireDesc UNID
//	DWORD			m_iLifetime
//	DWORD			m_Source (CSpaceObject ref)
//	DWORD			m_pSovereign (CSovereign ref)
//	CSpaceObject	m_pTarget
//	DWORD			m_iTick
//	DWORD			m_iLastDirection
//
//	DWORD			No. of segments
//	CVector			vPos
//	CVector			vVel
//	DWORD			dwGeneration
//	DWORD			iDamage
//	DWORD			Flags
//
//	IEffectPainter
//
//	CItemEnhancementStack	m_pEnhancements
//
//	DWORD			Flags

	{
	int i;
	DWORD dwLoad;

#ifdef DEBUG_LOAD
	::OutputDebugString("CContinuousBeam::OnReadFromStream\n");
#endif

	//	Load descriptor

	CString sDescUNID;
	sDescUNID.ReadFromStream(Ctx.pStream);
	m_pDesc = g_pUniverse->FindWeaponFireDesc(sDescUNID);

	Ctx.pStream->Read((char *)&m_iLifetime, sizeof(m_iLifetime));
	m_Source.ReadFromStream(Ctx);
	CSystem::ReadSovereignRefFromStream(Ctx, &m_pSovereign);
	CSystem::ReadObjRefFromStream(Ctx, &m_pTarget);
	Ctx.pStream->Read((char *)&m_iTick, sizeof(m_iTick));
	Ctx.pStream->Read((char *)&m_iLastDirection, sizeof(DWORD));

	//	Load segments

	DWORD dwCount;
	Ctx.pStream->Read((char *)&dwCount, sizeof(DWORD));
	m_Segments.InsertEmpty(dwCount);
	for (i = 0; i < (int)dwCount; i++)
		{
		SSegment &Segment = m_Segments[i];
		Ctx.pStream->Read((char *)&Segment.vPos, sizeof(CVector));
		Ctx.pStream->Read((char *)&Segment.vDeltaPos, sizeof(CVector));
		Ctx.pStream->Read((char *)&Segment.dwGeneration, sizeof(DWORD));
		Ctx.pStream->Read((char *)&Segment.iDamage, sizeof(DWORD));

		Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));

		Segment.fAlive =		((dwLoad & 0x00000001) ? true : false);
		Segment.fHit =			((dwLoad & 0x00000002) ? true : false);
		Segment.fPassthrough =	((dwLoad & 0x00000004) ? true : false);
		}

	//	For now we don't bother saving the effect painter

	m_pEffectPainter = CEffectCreator::CreatePainterFromStreamAndCreator(Ctx, m_pDesc->GetEffect());

	//	Enhancements

	if (Ctx.dwVersion >= 92)
		CItemEnhancementStack::ReadFromStream(Ctx, &m_pEnhancements);

	//	Flags

	DWORD dwFlags = 0;
	Ctx.pStream->Read((char *)&dwFlags, sizeof(DWORD));
	}

void CContinuousBeam::OnUpdate (SUpdateCtx &Ctx, Metric rSecondsPerTick)

//	OnUpdate
//
//	Update

	{
	int i;

	DWORD dwNow = (DWORD)g_pUniverse->GetTicks();
	m_iTick++;

	//	Update the effect painter

	if (m_pEffectPainter)
		m_pEffectPainter->OnUpdate();

	//	Update each segment (except for the pseudo segment).

	bool bAlive = false;
	for (i = 0; i < m_Segments.GetCount() - 1; i++)
		{
		SSegment &Segment = m_Segments[i];
		if (!Segment.fAlive)
			continue;

		//	We have at least one valid segment

		bAlive = true;

		//	See if we've expired

		if (Segment.fHit
				|| dwNow >= Segment.dwGeneration + (DWORD)m_iLifetime)
			Segment.fAlive = false;
		}

	//	If no segments left, then we're done

	if (!bAlive)
		Destroy(removedFromSystem, CDamageSource());
	}

void CContinuousBeam::OnWriteToStream (IWriteStream *pStream)

//	OnWriteToStream
//
//	Write out to stream
//
//	CString			CWeaponFireDesc UNID
//	DWORD			m_iLifetime
//	DWORD			m_Source (CSpaceObject ref)
//	DWORD			m_pSovereign (CSovereign ref)
//	CSpaceObject	m_pTarget
//	DWORD			m_iTick
//	DWORD			m_iLastDirection
//
//	DWORD			No. of segments
//	CVector			vPos
//	CVector			vDeltaPos
//	DWORD			iGeneration
//	DWORD			iDamage
//	DWORD			Flags
//
//	IEffectPainter
//
//	CItemEnhancementStack	m_pEnhancements
//
//	DWORD			Flags

	{
	int i;

	DWORD dwSave;
	m_pDesc->GetUNID().WriteToStream(pStream);
	pStream->Write((char *)&m_iLifetime, sizeof(m_iLifetime));
	m_Source.WriteToStream(GetSystem(), pStream);
	GetSystem()->WriteSovereignRefToStream(m_pSovereign, pStream);
	WriteObjRefToStream(m_pTarget, pStream);
	pStream->Write((char *)&m_iTick, sizeof(m_iTick));
	pStream->Write((char *)&m_iLastDirection, sizeof(DWORD));

	//	Segments

	dwSave = m_Segments.GetCount();
	pStream->Write((char *)&dwSave, sizeof(DWORD));
	for (i = 0; i < m_Segments.GetCount(); i++)
		{
		const SSegment &Segment = m_Segments[i];
		pStream->Write((char *)&Segment.vPos, sizeof(CVector));
		pStream->Write((char *)&Segment.vDeltaPos, sizeof(CVector));
		pStream->Write((char *)&Segment.dwGeneration, sizeof(DWORD));
		pStream->Write((char *)&Segment.iDamage, sizeof(DWORD));

		dwSave = 0;
		dwSave |= (Segment.fAlive ?			0x00000001 : 0);
		dwSave |= (Segment.fHit ?			0x00000002 : 0);
		dwSave |= (Segment.fPassthrough ?	0x00000004 : 0);

		pStream->Write((char *)&dwSave, sizeof(DWORD));
		}

	//	Painter

	CEffectCreator::WritePainterToStream(pStream, m_pEffectPainter);

	//	Enhancements

	CItemEnhancementStack::WriteToStream(m_pEnhancements, pStream);

	//	Flags

	dwSave = 0;
	pStream->Write((char *)&dwSave, sizeof(DWORD));
	}

void CContinuousBeam::PaintSegment (CG32bitImage &Dest, const CVector &vFrom, const CVector &vTo, SViewportPaintCtx &Ctx) const

//	PaintSegment
//
//	Paints a single segment

	{
	//	Convert to screen coordinates

	int xFrom, yFrom;
	Ctx.XForm.Transform(vFrom, &xFrom, &yFrom);

	int xTo, yTo;
	Ctx.XForm.Transform(vTo, &xTo, &yTo);

	//	Paint

	Dest.DrawLine(xFrom, yFrom, xTo, yTo, 2, CG32bitPixel(255, 255, 0));
	}

bool CContinuousBeam::PointInObject (const CVector &vObjPos, const CVector &vPointPos)

//	PointInObject
//
//	Returns TRUE if the given point is in the object

	{
	return false;
	}

void CContinuousBeam::UpdateBeamMotion (Metric rSeconds, CVector *retvNewPos, Metric *retrMaxBoundsX, Metric *retrMaxBoundsY)

//	UpdateBeamMotion
//
//	Updates the movement of each segment of the beam.

	{
	CVector vPos = GetPos();
	bool bFoundFirst = false;

	//	Keep track of bounds

	CVector vLL = vPos;
	CVector vUR = vPos;

	//	Update motion for all segments (including the pseudo segment).

	int i;
	for (i = 0; i < m_Segments.GetCount(); i++)
		{
		SSegment *pSegment = &m_Segments[i];
		CVector vNewPos = pSegment->vPos + pSegment->vDeltaPos;

		//	See if we hit anything

		CSpaceObject *pHit;
		int iHitDir;
		if (pSegment->fAlive
				&& HitTestSegment(pSegment->vPos, vNewPos, &pHit, &iHitDir))
			{
			//	See if we pass through

			if (m_pDesc->GetPassthrough() > 0
					&& mathRandom(1, 100) <= m_pDesc->GetPassthrough()
					&& pHit->GetPassthroughDefault() != damageNoDamageNoPassthrough)
				{
				pSegment->vPos = pSegment->vPos + pSegment->vDeltaPos;
				pSegment->fPassthrough = true;
				}

			//	Otherwise, we split the beam at the point where we hit

			else
				{
				//	Split the segment in two

				SSegment *pDeadSegment = m_Segments.InsertAt(i);
				i++;
				pSegment = &m_Segments[i];

				pDeadSegment->vPos = pSegment->vPos;
				pDeadSegment->vDeltaPos = pSegment->vDeltaPos;
				pDeadSegment->fAlive = false;

				pSegment->vPos = vNewPos;
				pSegment->fPassthrough = false;
				}

			//	Do damage

			EDamageResults iResult = DoDamage(pHit, vNewPos, iHitDir, pSegment->iDamage);

			//	If we passed through completely, then nothing else to do

			if (pSegment->fPassthrough)
				{ }

			//	If we hit something that allowed us to pass through, then chance of 
			//	passthrough.

			else if (iResult == damagePassthrough || iResult == damagePassthroughDestroyed)
				pSegment->fHit = (mathRandom(1, 100) >=50);

			//	Segment is dead next frame

			else
				pSegment->fHit = true;
			}

		//	Update
		//
		//	NOTE: We update all segments, even if dead, because a dead segment
		//	could specify the end point of a live segment.

		else
			pSegment->vPos = vNewPos;

		//	Our position is the first live segment

		if (!bFoundFirst && pSegment->fAlive)
			{
			vPos = pSegment->vPos;

			vLL = pSegment->vPos;
			vUR = pSegment->vPos;
			bFoundFirst = true;
			}

		//	If we've already found the first live segment, then accumulate
		//	positions (either dead or alive).

		else if (bFoundFirst)
			CGeometry::AccumulateBounds(pSegment->vPos, vLL, vUR);
		}

	//	Bounds are relative to the object center

	vLL = vLL - vPos;
	vUR = vUR - vPos;

	//	Done

	if (retvNewPos)
		*retvNewPos = vPos;

	if (retrMaxBoundsX)
		*retrMaxBoundsX = Max(Absolute(vLL.GetX()), Absolute(vUR.GetX()));

	if (retrMaxBoundsY)
		*retrMaxBoundsY = Max(Absolute(vLL.GetY()), Absolute(vUR.GetY()));
	}
