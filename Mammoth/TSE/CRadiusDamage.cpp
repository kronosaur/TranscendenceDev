//	CRadiusDamage.cpp
//
//	CRadiusDamage class

#include "PreComp.h"

CRadiusDamage::CRadiusDamage (CUniverse &Universe) : TSpaceObjectImpl(Universe),
		m_pPainter(NULL)

//	CRadiusDamage constructor

	{
	}

CRadiusDamage::~CRadiusDamage (void)

//	CRadiusDamage destructor

	{
	if (m_pPainter)
		m_pPainter->Delete();
	}

ALERROR CRadiusDamage::Create (CSystem &System, SShotCreateCtx &Ctx, CRadiusDamage **retpObj)

//	Create
//
//	Create the object

	{
	ALERROR error;

	//	Make sure we have a valid CWeaponFireDesc (otherwise we won't be
	//	able to save the descriptor).

	ASSERT(!Ctx.pDesc->GetUNID().IsBlank());

	//	Create the area

	CRadiusDamage *pArea = new CRadiusDamage(System.GetUniverse());
	if (pArea == NULL)
		return ERR_MEMORY;

	pArea->Place(Ctx.vPos, Ctx.vVel);

	//	Get notifications when other objects are destroyed
	pArea->SetObjectDestructionHook();

	pArea->m_iLifeLeft = Max(1, Ctx.pDesc->GetLifetime());
	pArea->m_pDesc = Ctx.pDesc;
	pArea->m_pEnhancements = Ctx.pEnhancements;
	pArea->m_Source = Ctx.Source;
	pArea->m_pTarget = Ctx.pTarget;
	pArea->m_iTick = 0;

	Metric rRadius = Ctx.pDesc->GetMaxRadius();

	//	Friendly fire

	if (!Ctx.pDesc->CanHitFriends())
		pArea->SetNoFriendlyFire();

	//	Remember the sovereign of the source (in case the source is destroyed)

	pArea->m_pSovereign = Ctx.Source.GetSovereign();

	//	Create a painter instance

	pArea->m_pPainter = Ctx.pDesc->CreateEffectPainter(Ctx);
	if (pArea->m_pPainter)
		{
		//	Adjust lifetime of object based on the painter

		pArea->m_iLifeLeft = Max(pArea->m_pPainter->GetLifetime(), pArea->m_iLifeLeft);

		//	Adjust radius of object based on the painter.

		rRadius = Max(pArea->m_pPainter->GetRadius(pArea->m_iTick), rRadius);
		}

	//	Our bounds are based on the max radius

	pArea->SetBounds(rRadius);

	//	Add to system

	if (error = pArea->AddToSystem(System))
		{
		delete pArea;
		return error;
		}

	//	Done

	if (retpObj)
		*retpObj = pArea;

	return NOERROR;
	}

void CRadiusDamage::DamageAll (SUpdateCtx &Ctx)

//	DamageAll
//
//	Damage all objects in the radius

	{
	Metric rMinRadius = m_pDesc->GetMinRadius();
	Metric rMaxRadius = m_pDesc->GetMaxRadius();
	Metric rRadiusRange = rMaxRadius - rMinRadius;

	//	Get the list of objects inside the maximum radius

	SSpaceObjectGridEnumerator i;
	GetSystem()->EnumObjectsInBoxStart(i, GetPos(), rMaxRadius);

	//	Loop over each object in the box

	while (GetSystem()->EnumObjectsInBoxHasMore(i))
		{
		CSpaceObject *pObj = GetSystem()->EnumObjectsInBoxGetNext(i);
		if (!CanHit(pObj)
				|| !pObj->CanBeHit()
				|| !pObj->CanBeHitBy(m_pDesc->GetDamage())
				|| pObj->IsDestroyed()
				|| pObj == this)
			continue;

		//	Compute the distance between this object and the center
		//	of the blast

		CVector vDist = (pObj->GetPos() - GetPos());
		int iAngle = VectorToPolar(vDist);

		//	Figure out where we hit this object

		Metric rObjRadius = 0.5 * pObj->GetHitSize();
		CVector vHitPos = pObj->GetPos() - PolarToVector(iAngle, rObjRadius);
		CVector vInc = PolarToVector(iAngle, 2.0 * g_KlicksPerPixel);
		int iMax = mathRound(2.0 * rObjRadius / (2.0 * g_KlicksPerPixel));
		while (!pObj->PointInObject(pObj->GetPos(), vHitPos) && iMax-- > 0)
			vHitPos = vHitPos + vInc;

		//	The hit position is what we use to figure out damage

		Metric rHitDist = (vHitPos - GetPos()).Length();
		if (rHitDist > rMaxRadius)
			continue;

		//	If we're beyond the minimum radius, then decrease the damage
		//	to account for distance

		int iDamage;
		if (rHitDist > rMinRadius && rRadiusRange > 0.0)
			{
			Metric rMult = (rRadiusRange - (rHitDist - rMinRadius)) / rRadiusRange;
			iDamage = mathRound(rMult * (Metric)m_pDesc->GetDamage().RollDamage());
			}
		else
			iDamage = m_pDesc->GetDamage().RollDamage();

		//	Setup context

		SDamageCtx DamageCtx(pObj,
				m_pDesc,
				m_pEnhancements,
				m_Source,
				this,
				AngleMod(iAngle + 180),
				vHitPos,
				iDamage);

		//	Do damage

		pObj->Damage(DamageCtx);
		}
	}

CString CRadiusDamage::DebugCrashInfo (void)

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
	sResult.Append(strPatternSubst(CONSTLIT("m_iLifeLeft: %d\r\n"), m_iLifeLeft));

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

	return sResult;
	}

CString CRadiusDamage::GetNamePattern (DWORD dwNounPhraseFlags, DWORD *retdwFlags) const

//	GetName
//
//	Returns the name of the object

	{
	//	This name is used only if the source has been destroyed

	if (retdwFlags)
		*retdwFlags = 0;
	return CONSTLIT("enemy weapon");
	}

void CRadiusDamage::OnDestroyed (SDestroyCtx &Ctx)

//	OnDestroyed
//
//	Shot destroyed

	{
	m_pDesc->FireOnDestroyShot(this);
	}

void CRadiusDamage::OnMove (const CVector &vOldPos, Metric rSeconds)

//	OnMove
//
//	Handle moving

	{
	//	Update the painter motion

	if (m_pPainter)
		{
		SEffectMoveCtx Ctx;
		Ctx.pObj = this;

		bool bBoundsChanged;
		m_pPainter->OnMove(Ctx, &bBoundsChanged);

		//	Set bounds, if they've changed

		if (bBoundsChanged)
			SetBounds(m_pPainter);
		}
	}

void CRadiusDamage::ObjectDestroyedHook (const SDestroyCtx &Ctx)

//	ObjectDestroyedHook
//
//	Called when another object is destroyed

	{
	m_Source.OnObjDestroyed(Ctx.Obj);

	if (Ctx.Obj == m_pTarget)
		m_pTarget = NULL;
	}

void CRadiusDamage::OnPaint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	OnPaint
//
//	Paint

	{
	if (m_pPainter)
		{
		CViewportPaintCtxSmartSave Save(Ctx);
		Ctx.iTick = m_iTick;
		Ctx.iVariant = 0;
		Ctx.iRotation = 0;
		Ctx.iDestiny = GetDestiny();

		m_pPainter->Paint(Dest, x, y, Ctx);
		}
	}

void CRadiusDamage::OnReadFromStream (SLoadCtx &Ctx)

//	OnReadFromStream
//
//	Restore from stream

	{
	DWORD dwLoad;

#ifdef DEBUG_LOAD
	::OutputDebugString("CRadiusDamage::OnReadFromStream\n");
#endif

	//	Load descriptor

	CString sDescUNID;
	sDescUNID.ReadFromStream(Ctx.pStream);
	m_pDesc = Ctx.GetUniverse().FindWeaponFireDesc(sDescUNID);

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
	if (Ctx.dwVersion >= 19)
		CSystem::ReadObjRefFromStream(Ctx, &m_pTarget);
	else
		m_pTarget = NULL;

	CSystem::ReadSovereignRefFromStream(Ctx, &m_pSovereign);
	Ctx.pStream->Read((char *)&m_iTick, sizeof(m_iTick));

	//	Load painter

	m_pPainter = CEffectCreator::CreatePainterFromStreamAndCreator(Ctx, m_pDesc->GetEffect());

	//	Enhancements

	if (Ctx.dwVersion >= 92)
		m_pEnhancements = CItemEnhancementStack::ReadFromStream(Ctx);
	}

void CRadiusDamage::OnSystemLoaded (void)

//	OnSystemLoaded
//
//	System done loading

	{
	}

void CRadiusDamage::OnUpdate (SUpdateCtx &Ctx, Metric rSecondsPerTick)

//	OnUpdate
//
//	Update

	{
	DEBUG_TRY

	bool bDestroy = false;

	//	Do damage right away

	if (m_iTick == 0)
		{
		//	Damage all targets in radius

		if (m_pDesc->GetMaxRadius() > 0.0)
			DamageAll(Ctx);

		//	Spawn fragments, if necessary

		if (m_pDesc->HasFragments())
			{
			SShotCreateCtx FragCtx;
			FragCtx.pDesc = m_pDesc;
			FragCtx.pEnhancements = m_pEnhancements;
			FragCtx.Source = m_Source;
			FragCtx.pTarget = m_pTarget;
			FragCtx.vPos = GetPos();
			FragCtx.vVel = GetVel();

			GetSystem()->CreateWeaponFragments(FragCtx, this);
			}
		}

	//	Update the object

	m_iTick++;

	SEffectUpdateCtx EffectCtx;
	EffectCtx.pSystem = GetSystem();
	EffectCtx.pObj = this;
	EffectCtx.iTick = m_iTick;

	if (m_pPainter)
		m_pPainter->OnUpdate(EffectCtx);

	//	Destroy

	if (bDestroy || --m_iLifeLeft <= 0)
		Destroy(removedFromSystem, CDamageSource());

	DEBUG_CATCH
	}

void CRadiusDamage::OnWriteToStream (IWriteStream *pStream)

//	OnWriteToStream
//
//	Write out to stream
//
//	CString			CWeaponFireDesc UNID
//	DWORD			m_iLifeLeft
//	DWORD			m_Source (CSpaceObject ref)
//	DWORD			m_pTarget (CSpaceObject ref)
//	DWORD			m_pSovereign (CSovereign ref)
//	DWORD			m_iTick
//	IEffectPainter	m_pPainter
//
//	CItemEnhancementStack	m_pEnhancements

	{
	m_pDesc->GetUNID().WriteToStream(pStream);
	pStream->Write((char *)&m_iLifeLeft, sizeof(m_iLifeLeft));
	m_Source.WriteToStream(GetSystem(), pStream);
	WriteObjRefToStream(m_pTarget, pStream);
	GetSystem()->WriteSovereignRefToStream(m_pSovereign, pStream);
	pStream->Write((char *)&m_iTick, sizeof(m_iTick));

	CEffectCreator::WritePainterToStream(pStream, m_pPainter);

	//	Enhancements

	CItemEnhancementStack::WriteToStream(m_pEnhancements, pStream);
	}

bool CRadiusDamage::PointInObject (const CVector &vObjPos, const CVector &vPointPos) const

//	PointInObject
//
//	Returns TRUE if the given point is in the object

	{
	return false;
	}
