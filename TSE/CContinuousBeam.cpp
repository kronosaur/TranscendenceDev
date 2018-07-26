//	CContinuousBeam.cpp
//
//	CContinuousBeam class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

static CObjectClass<CContinuousBeam>g_Class(OBJID_CCONTINUOUSBEAM, NULL);

CContinuousBeam::CContinuousBeam (void) : CSpaceObject(&g_Class),
		m_pPainter(NULL)

//	CContinuousBeam constructor

	{
	}

CContinuousBeam::~CContinuousBeam (void)

//	CContinuousBeam destructor

	{
	if (m_pPainter)
		m_pPainter->Delete();
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

ALERROR CContinuousBeam::Create (CSystem *pSystem, SShotCreateCtx &Ctx, CContinuousBeam **retpObj)

//	Create
//
//	Create the object

	{
	ALERROR error;

	//	Make sure we have a valid CWeaponFireDesc (otherwise we won't be
	//	able to save the object).

	ASSERT(!Ctx.pDesc->GetUNID().IsBlank());

	//	Create the object

	CContinuousBeam *pBeam = new CContinuousBeam;
	if (pBeam == NULL)
		return ERR_MEMORY;

	pBeam->Place(Ctx.vPos, CVector());

	//	Get notifications when other objects are destroyed
	pBeam->SetObjectDestructionHook();

	//	Set non-linear move, meaning that we are responsible for
	//	setting the position and velocity in OnMove
	pBeam->SetNonLinearMove();

	pBeam->m_pDesc = Ctx.pDesc;
	pBeam->m_pTarget = Ctx.pTarget;
	pBeam->m_pEnhancements = Ctx.pEnhancements;
	pBeam->m_iLifetime = Ctx.pDesc->GetLifetime();
	pBeam->m_iTick = 0;
	pBeam->m_iLastDirection = Ctx.iDirection;
	pBeam->m_Source = Ctx.Source;

	//	Friendly fire

	if (!Ctx.pDesc->CanHitFriends())
		pBeam->SetNoFriendlyFire();

	//	Create the effect painter, if we've got one

	pBeam->m_pPainter = Ctx.pDesc->CreateEffectPainter(Ctx);

	//	Remember the sovereign of the source (in case the source is destroyed)

	if (Ctx.Source.GetObj())
		pBeam->m_pSovereign = Ctx.Source.GetObj()->GetSovereign();
	else
		pBeam->m_pSovereign = NULL;

	//	Add a segment

	pBeam->AddContinuousBeam(Ctx.vPos, Ctx.vVel, Ctx.iDirection);

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

CString CContinuousBeam::DebugCrashInfo (void)

//	DebugCrashInfo
//
//	Dump debug information

	{
	CString sResult;

	sResult.Append(strPatternSubst(CONSTLIT("m_pDesc: %s\r\n"), m_pDesc->GetUNID()));
	sResult.Append(strPatternSubst(CONSTLIT("m_Source: %s\r\n"), CSpaceObject::DebugDescribe(m_Source.GetObj())));
	sResult.Append(strPatternSubst(CONSTLIT("m_pTarget: %s\r\n"), CSpaceObject::DebugDescribe(m_pTarget)));
	sResult.Append(strPatternSubst(CONSTLIT("m_iDestiny: %d\r\n"), GetDestiny()));
	sResult.Append(strPatternSubst(CONSTLIT("m_iTick: %d\r\n"), m_iTick));

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

	return sResult;
	}

EDamageResults CContinuousBeam::DoDamage (CSpaceObject *pHit, const CVector &vHitPos, int iHitDir)

//	DoDamage
//
//	Do damage to the given object

	{
	SDamageCtx DamageCtx(pHit,
			m_pDesc,
			m_pEnhancements,
			m_Source,
			this,
			AngleMod(iHitDir + mathRandom(0, 30) - 15),
			vHitPos);

	return pHit->Damage(DamageCtx);
	}

CString CContinuousBeam::GetNamePattern (DWORD dwNounPhraseFlags, DWORD *retdwFlags) const

//	GetName
//
//	Returns the name of the object

	{
	//	This name is used only if the source has been destroyed

	if (retdwFlags)
		*retdwFlags = 0;
	return CONSTLIT("enemy weapon");
	}

bool CContinuousBeam::HitTestSegment (SSegment &Segment, CVector *retvHitPos)

//	HitTestSegment
//
//	Hit tests the segment against all objects and initializes the segment's
//	Hit array. If the segment is stopped by a hit (does not pass through) then
//	we return TRUE and retvHitPos is where the segment got stopped.

	{
	int iInteraction = m_pDesc->GetInteraction();
	CVector vNewPos = Segment.vPos + Segment.vDeltaPos;

	//	Get our bounds

	CVector vLL = Segment.vPos;
	CVector vUR = Segment.vPos;
	CGeometry::AccumulateBounds(vNewPos, vLL, vUR);

	//	Compute the segment length (we need this for stepping and for bounds)

	CVector vDiff = (vNewPos - Segment.vPos);
	Metric rLength = vDiff.Length();
	if (rLength == 0.0)
		return false;

	Metric rStep = 3.0 * g_KlicksPerPixel;
	CVector vStep = (rStep / rLength) * vDiff;
	int iHitDir = AngleMod(180 + VectorToPolar(vStep));

	//	Get the list of objects that intersect the object

	SSpaceObjectGridEnumerator i;
	GetSystem()->EnumObjectsInBoxStart(i, vUR, vLL);

	//	If we have passthrough, then we need to get a list of objects hit.

	if (m_pDesc->GetPassthrough() > 0)
		{
		//	We keep an array of hits sorted by ascending order of distance
		//	along the travel direction.

		TSortMap<Metric, CHitCtx> Hits;

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
			CVector vTest = Segment.vPos;
			while (rTest < rLength)
				{
				if (pObj->PointInObject(pObj->GetPos(), vTest))
					{
					Hits.Insert(rTest, CHitCtx(pObj, vTest, iHitDir));
					break;
					}

				rTest += rStep;
				vTest = vTest + vStep;
				}
			}

		//	Loop over all hits in order and see if they passthrough

		bool bHit = false;
		TArray<DWORD> NewHits;
		for (int j = 0; j < Hits.GetCount(); j++)
			{
			//	If this object was hit by this same segment last tick, then
			//	skip it.

			if (Segment.Hits.Find(Hits[j].GetHitObj()->GetID()))
				continue;

			//	Add this hit to the list

			m_Hits.Insert(Hits[j]);
			NewHits.Insert(Hits[j].GetHitObj()->GetID());

			//	If we DO NOT pass through, then we're done

			if (mathRandom(1, 100) > m_pDesc->GetPassthrough()
					|| Hits[j].GetHitObj()->GetPassthroughDefault() == damageNoDamageNoPassthrough)
				{
				bHit = true;
				*retvHitPos = Hits[j].GetHitPos();
				break;
				}
			}

		//	Reset the list of hits for this segment.

		Segment.Hits = NewHits;

		//	Done

		return bHit;
		}

	//	Otherwise, we only have a single object.

	else
		{
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
			CVector vTest = Segment.vPos;
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

		//	If nothing hit, then we're done

		if (pHit == NULL)
			return false;

		//	Remember what we hit.
		//
		//	NOTE: We do not need to remember the object hit in the segment 
		//	because we only use that to compute passthrough.

		m_Hits.Insert(CHitCtx(pHit, vBestHit, iHitDir));

		//	Return the split position

		*retvHitPos = vBestHit;
		return true;
		}
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

	if (m_pPainter)
		{
		SEffectMoveCtx Ctx;
		Ctx.pObj = this;

		m_pPainter->OnMove(Ctx);
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
	int i;

	m_Source.OnObjDestroyed(Ctx.pObj);

	if (Ctx.pObj == m_pTarget)
		m_pTarget = NULL;

	for (i = 0; i < m_Hits.GetCount(); i++)
		if (Ctx.pObj == m_Hits[i].GetHitObj())
			{
			m_Hits.Delete(i);
			i--;
			}
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

	//	Keep painting live segments

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
			m_pPainter->PaintLine(Dest, vHead, Segment.vPos, Ctx);
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
//
//	DWORD			No. of hit objects (only if flag set)
//	CHitCtx			Hit

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

		//	Flags

		Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));

		Segment.fAlive =		((dwLoad & 0x00000001) ? true : false);
		Segment.fHit =			((dwLoad & 0x00000002) ? true : false);
		Segment.fPassthrough =	((dwLoad & 0x00000004) ? true : false);
		bool bHit =				((dwLoad & 0x00000008) ? true : false);

		//	Hits

		if (bHit)
			{
			Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
			Segment.Hits.InsertEmpty(dwLoad);
			Ctx.pStream->Read((char *)&Segment.Hits[0], sizeof(DWORD) * dwLoad);
			}
		}

	//	For now we don't bother saving the effect painter

	m_pPainter = CEffectCreator::CreatePainterFromStreamAndCreator(Ctx, m_pDesc->GetEffect());

	//	Enhancements

	if (Ctx.dwVersion >= 92)
		m_pEnhancements = CItemEnhancementStack::ReadFromStream(Ctx);

	//	Flags

	DWORD dwFlags = 0;
	Ctx.pStream->Read((char *)&dwFlags, sizeof(DWORD));
	bool bHit =			((dwFlags & 0x00000001) ? true : false);

	//	Hit objects

	if (bHit)
		{
		Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
		m_Hits.InsertEmpty(dwLoad);
		for (i = 0; i < m_Hits.GetCount(); i++)
			m_Hits[i].ReadFromStream(Ctx);
		}
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

	if (m_pPainter)
		m_pPainter->OnUpdate();

	//	Do damage

	for (i = 0; i < m_Hits.GetCount(); i++)
		{
		const CHitCtx &Hit = m_Hits[i];

		//	Do damage

		EDamageResults iResult = DoDamage(Hit.GetHitObj(), Hit.GetHitPos(), Hit.GetHitDir());

		//	NOTE: No need to do anything with the result because we've already
		//	determined whether the beam/segment needs to be split. We do not 
		//	check for passthrough here because we already checked in OnMove.
		//	[And we have to check in OnMove because otherwise we would not
		//	paint correctly.]
		}

	//	Update each segment (except for the pseudo segment).

	bool bAlive = false;
	for (i = 0; i < m_Segments.GetCount() - 1; i++)
		{
		SSegment &Segment = m_Segments[i];
		if (!Segment.fAlive)
			continue;

		//	We have at least one valid segment

		bAlive = true;

		//	If we've expired or the segment hit something, then we're dead now

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
//	DWORD			No. of objects hit (only if flag set)
//	DWORD			ObjectID
//
//	IEffectPainter
//
//	CItemEnhancementStack	m_pEnhancements
//
//	DWORD			Flags
//
//	DWORD			No. of hit objects (only if flag set)
//	CHitCtx			Hit

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

		//	Flags

		dwSave = 0;
		dwSave |= (Segment.fAlive ?				0x00000001 : 0);
		dwSave |= (Segment.fHit ?				0x00000002 : 0);
		dwSave |= (Segment.fPassthrough ?		0x00000004 : 0);
		dwSave |= (Segment.Hits.GetCount() ?	0x00000008 : 0);

		pStream->Write((char *)&dwSave, sizeof(DWORD));

		//	Hits

		if (Segment.Hits.GetCount())
			{
			dwSave = Segment.Hits.GetCount();
			pStream->Write((char *)&dwSave, sizeof(DWORD));
			pStream->Write((char *)&Segment.Hits[0], sizeof(DWORD) * dwSave);
			}
		}

	//	Painter

	CEffectCreator::WritePainterToStream(pStream, m_pPainter);

	//	Enhancements

	CItemEnhancementStack::WriteToStream(m_pEnhancements, pStream);

	//	Flags

	dwSave = 0;
	dwSave |= (m_Hits.GetCount() ?	0x00000001 : 0);
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	//	Save any hit objects

	if (m_Hits.GetCount())
		{
		dwSave = m_Hits.GetCount();
		pStream->Write((char *)&dwSave, sizeof(DWORD));

		for (i = 0; i < m_Hits.GetCount(); i++)
			m_Hits[i].WriteToStream(GetSystem(), pStream);
		}
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

bool CContinuousBeam::PointInObject (const CVector &vObjPos, const CVector &vPointPos) const

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

	//	Reset hit information

	m_Hits.DeleteAll();

	//	Keep track of bounds

	CVector vLL = vPos;
	CVector vUR = vPos;

	//	Update motion for all segments (including the pseudo segment).

	int i;
	for (i = 0; i < m_Segments.GetCount(); i++)
		{
		SSegment *pSegment = &m_Segments[i];
		CVector vNewPos = pSegment->vPos + pSegment->vDeltaPos;

		//	See if we hit anything. HitTestSegment adds hits as appropriate, but
		//	it only returns TRUE if we need to split the beam at a hit point.

		CVector vSplitPos;
		if (pSegment->fAlive
				&& HitTestSegment(m_Segments[i], &vSplitPos))
			{
			//	Split the segment in two

			SSegment *pDeadSegment = m_Segments.InsertAt(i);
			i++;
			pSegment = &m_Segments[i];

			pDeadSegment->vPos = pSegment->vPos;
			pDeadSegment->vDeltaPos = pSegment->vDeltaPos;
			pDeadSegment->fAlive = false;

			pSegment->vPos = vSplitPos;
			pSegment->fPassthrough = false;
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

