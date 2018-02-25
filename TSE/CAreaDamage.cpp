//	CAreaDamage.cpp
//
//	CAreaDamage class

#include "PreComp.h"

#define SPEED_PARAM								CONSTLIT("speed")

static CObjectClass<CAreaDamage>g_Class(OBJID_CAREADAMAGE, NULL);

CAreaDamage::CAreaDamage (void) : CSpaceObject(&g_Class),
		m_pPainter(NULL)

//	CAreaDamage constructor

	{
	}

CAreaDamage::~CAreaDamage (void)

//	CAreaDamage destructor

	{
	if (m_pPainter)
		m_pPainter->Delete();
	}

ALERROR CAreaDamage::Create (CSystem *pSystem, SShotCreateCtx &Ctx, CAreaDamage **retpObj)

//	Create
//
//	Create the object

	{
	ALERROR error;

	//	Make sure we have a valid CWeaponFireDesc (otherwise we won't be
	//	able to save it the area of effect).
	ASSERT(!Ctx.pDesc->GetUNID().IsBlank());

	//	Create the area

	CAreaDamage *pArea = new CAreaDamage;
	if (pArea == NULL)
		return ERR_MEMORY;

	pArea->Place(Ctx.vPos, Ctx.vVel);

	//	Get notifications when other objects are destroyed
	pArea->SetObjectDestructionHook();

	pArea->m_pDesc = Ctx.pDesc;
	pArea->m_pEnhancements = Ctx.pEnhancements;
	pArea->m_iLifeLeft = Ctx.pDesc->GetLifetime();
	pArea->m_Source = Ctx.Source;
	pArea->m_iInitialDelay = Ctx.pDesc->GetInitialDelay();
	pArea->m_iTick = 0;

	//	Friendly fire

	if (!Ctx.pDesc->CanHitFriends())
		pArea->SetNoFriendlyFire();

	//	Remember the sovereign of the source (in case the source is destroyed)

	pArea->m_pSovereign = Ctx.Source.GetSovereign();

	//	Create a painter instance

	pArea->m_pPainter = Ctx.pDesc->CreateShockwavePainter();

	//	Add to system

	if (error = pArea->AddToSystem(pSystem))
		{
		delete pArea;
		return error;
		}

	//	Done

	if (retpObj)
		*retpObj = pArea;

	return NOERROR;
	}

CString CAreaDamage::GetNamePattern (DWORD dwNounPhraseFlags, DWORD *retdwFlags) const

//	GetName
//
//	Returns the name of the object

	{
	//	This name is used only if the source has been destroyed

	if (retdwFlags)
		*retdwFlags = 0;
	return CONSTLIT("enemy weapon");
	}

void CAreaDamage::OnDestroyed (SDestroyCtx &Ctx)

//	OnDestroyed
//
//	Shot destroyed

	{
	m_pDesc->FireOnDestroyShot(this);
	}

void CAreaDamage::OnMove (const CVector &vOldPos, Metric rSeconds)

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

void CAreaDamage::ObjectDestroyedHook (const SDestroyCtx &Ctx)

//	ObjectDestroyedHook
//
//	Called when another object is destroyed

	{
	m_Source.OnObjDestroyed(Ctx.pObj);
	}

void CAreaDamage::OnPaint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	OnPaint
//
//	Paint

	{
	if (m_iInitialDelay > 0)
		return;

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

void CAreaDamage::OnReadFromStream (SLoadCtx &Ctx)

//	OnReadFromStream
//
//	Restore from stream

	{
	DWORD dwLoad;

#ifdef DEBUG_LOAD
	::OutputDebugString("CAreaDamage::OnReadFromStream\n");
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

	if (Ctx.dwVersion >= 21)
		Ctx.pStream->Read((char *)&m_iInitialDelay, sizeof(DWORD));
	else
		m_iInitialDelay = 0;

	Ctx.pStream->Read((char *)&m_iLifeLeft, sizeof(m_iLifeLeft));
	m_Source.ReadFromStream(Ctx);
	CSystem::ReadSovereignRefFromStream(Ctx, &m_pSovereign);
	Ctx.pStream->Read((char *)&m_iTick, sizeof(m_iTick));

	//	Load painter

	m_pPainter = CEffectCreator::CreatePainterFromStreamAndCreator(Ctx, m_pDesc->GetEffect());

	//	Previous versions stored an m_Hit array

	if (Ctx.dwVersion < 70)
		{
		int iCount;
		Ctx.pStream->Read((char *)&iCount, sizeof(DWORD));
		if (iCount)
			Ctx.pStream->Read(NULL, iCount * 2 * sizeof(DWORD));
		}

	//	Enhancements

	if (Ctx.dwVersion >= 92)
		m_pEnhancements = CItemEnhancementStack::ReadFromStream(Ctx);
	}

void CAreaDamage::OnSystemLoaded (void)

//	OnSystemLoaded
//
//	System done loading

	{
	}

void CAreaDamage::OnUpdate (SUpdateCtx &Ctx, Metric rSecondsPerTick)

//	OnUpdate
//
//	Update

	{
	DEBUG_TRY

	if (m_iInitialDelay > 0)
		{
		m_iInitialDelay--;
		return;
		}

	if (m_pPainter == NULL)
		{
		Destroy(removedFromSystem, CDamageSource());
		return;
		}

	//	Next

	m_iTick++;

	//	Set up context block for particle array update

	SEffectUpdateCtx EffectCtx;
	EffectCtx.pSystem = GetSystem();
	EffectCtx.pObj = this;
	EffectCtx.iTick = m_iTick;

	EffectCtx.pDamageDesc = m_pDesc;
	EffectCtx.pEnhancements = m_pEnhancements;
	EffectCtx.iCause = m_Source.GetCause();
	EffectCtx.bAutomatedWeapon = IsAutomatedWeapon();
	EffectCtx.Attacker = m_Source;

	m_pPainter->OnUpdate(EffectCtx);

	//	Destroy

	if (EffectCtx.bDestroy || --m_iLifeLeft <= 0)
		Destroy(removedFromSystem, CDamageSource());

	DEBUG_CATCH
	}

void CAreaDamage::OnWriteToStream (IWriteStream *pStream)

//	OnWriteToStream
//
//	Write out to stream
//
//	CString			CWeaponFireDesc UNID
//	DWORD			m_iInitialDelay
//	DWORD			m_iLifeLeft
//	DWORD			m_Source (CDamageSource)
//	DWORD			m_pSovereign (CSovereign ref)
//	DWORD			m_iTick
//	IEffectPainter	m_pPainter
//
//	CItemEnhancementStack	m_pEnhancements

	{
	m_pDesc->GetUNID().WriteToStream(pStream);
	pStream->Write((char *)&m_iInitialDelay, sizeof(DWORD));
	pStream->Write((char *)&m_iLifeLeft, sizeof(m_iLifeLeft));
	m_Source.WriteToStream(GetSystem(), pStream);
	GetSystem()->WriteSovereignRefToStream(m_pSovereign, pStream);
	pStream->Write((char *)&m_iTick, sizeof(m_iTick));

	CEffectCreator::WritePainterToStream(pStream, m_pPainter);

	//	Enhancements

	CItemEnhancementStack::WriteToStream(m_pEnhancements, pStream);
	}

void CAreaDamage::PaintLRSForeground (CG32bitImage &Dest, int x, int y, const ViewportTransform &Trans)

//	PaintLRSForeground
//
//	Paints the object on an LRS

	{
	if (m_pPainter)
		{
		int i;
		Metric rRadius = m_pPainter->GetRadius(m_iTick);
		int iRadius = (int)((rRadius / g_KlicksPerPixel) + 0.5);
		int iCount = Min(64, 3 * iRadius);

		for (i = 0; i < iCount; i++)
			{
			CVector vPos = GetPos() + PolarToVector(mathRandom(0, 359), rRadius);
			Trans.Transform(vPos, &x, &y);

			int iColor = mathRandom(128, 255);
			Dest.DrawDot(x, y, 
					CG32bitPixel(iColor, iColor, 0), 
					markerSmallRound);
			}
		}
	}

bool CAreaDamage::PointInObject (const CVector &vObjPos, const CVector &vPointPos)

//	PointInObject
//
//	Returns TRUE if the given point is in the object

	{
	DEBUG_TRY

	if (m_pPainter)
		{
		Metric rRadius = m_pPainter->GetRadius(m_iTick);
		Metric rRadius2 = rRadius * rRadius;
		Metric rInnerRadius = rRadius * 0.7;
		Metric rInnerRadius2 = rInnerRadius * rInnerRadius;
		CVector vDist = vObjPos - vPointPos;
		Metric rDist2 = vDist.Length2();
		return (rDist2 < rRadius2 && rDist2 > rInnerRadius2);
		}
	else
		return false;

	DEBUG_CATCH
	}
