//	CParticleEffect.cpp
//
//	CParticleEffect class

#include "PreComp.h"

#define IMAGE_TAG								CONSTLIT("Image")

#define COUNT_ATTRIB							CONSTLIT("count")
#define DAMAGE_ATTRIB							CONSTLIT("damage")
#define DAMPENING_ATTRIB						CONSTLIT("dampening")
#define MIN_RADIUS_ATTRIB						CONSTLIT("minRadius")
#define NAME_ATTRIB								CONSTLIT("name")
#define NO_WAKE_ATTRIB							CONSTLIT("noWake")
#define RADIUS_ATTRIB							CONSTLIT("radius")

CParticleEffect::CParticleEffect (CUniverse &Universe) : TSpaceObjectImpl(Universe),
		m_pFirstGroup(NULL),
		m_pAnchor(NULL)

//	CParticleEffect constructor

	{
	}

CParticleEffect::~CParticleEffect (void)

//	CParticleEffect destructor

	{
	SParticleArray *pGroup = m_pFirstGroup;
	while (pGroup)
		{
		SParticleArray *pNext = pGroup->pNext;
		delete pGroup;
		pGroup = pNext;
		}
	}

void CParticleEffect::AddGroup (SParticleType *pType, int iCount)

//	AddGroup
//
//	Adds a group of particles. pType must be allocated. This
//	object takes ownership.

	{
	CreateGroup(pType, iCount, NULL);

	//	Make sure the bounds is properly set

	if (GetBounds() < pType->rRadius)
		SetBounds(pType->rRadius);
	}

bool CParticleEffect::CanBeHitBy (const DamageDesc &Damage)

//	CanBeHitBy
//
//	Returns TRUE if the given type of damage can hit this object

	{
	//	Only massless particles can hit a particle field

	return (Damage.GetDamageType() == damageLaser 
			|| Damage.GetDamageType() == damageParticle
			|| Damage.GetDamageType() == damageIonRadiation
			|| Damage.GetDamageType() == damagePositron);
	}

ALERROR CParticleEffect::Create (CSystem &System,
								 CXMLElement *pDesc,
								 const CVector &vPos,
								 const CVector &vVel,
								 CParticleEffect **retpEffect)

//	Create
//
//	Create a new particle effect in the system

	{
	ALERROR error;
	CParticleEffect *pParticles;

	pParticles = new CParticleEffect(System.GetUniverse());
	if (pParticles == NULL)
		return ERR_MEMORY;

	pParticles->Place(vPos, vVel);
	pParticles->SetObjectDestructionHook();

	pParticles->m_sName = pDesc->GetAttribute(NAME_ATTRIB);

	//	Create the type based on the descriptor

	SParticleType *pType = new SParticleType;
	pType->m_fWake = !pDesc->GetAttributeBool(NO_WAKE_ATTRIB);

	pType->m_fMaxRadius = true;
	pType->rRadius = pDesc->GetAttributeInteger(RADIUS_ATTRIB) * LIGHT_SECOND;
	pType->rHoleRadius = pDesc->GetAttributeInteger(MIN_RADIUS_ATTRIB) * LIGHT_SECOND;
	
	int iDampening = pDesc->GetAttributeInteger(DAMPENING_ATTRIB);
	if (iDampening > 0)
		pType->rDampening = iDampening / 1000.0;
	else
		pType->rDampening = 0.9999;

	pType->rAveSpeed = LIGHT_SPEED * 0.1;

	//	Load damage

	CString sDamage;
	if (pDesc->FindAttribute(DAMAGE_ATTRIB, &sDamage))
		{
		SDesignLoadCtx Ctx;
		DamageDesc Damage;
		if (error = Damage.LoadFromXML(Ctx, sDamage))
			return error;

		pType->pDamageDesc = new CWeaponFireDesc;
		pType->pDamageDesc->InitFromDamage(Damage);
		pType->m_fFreeDesc = true;
		}

	//	Set the image

	pType->rgbColor = CG32bitPixel(0, 255, 0);
	CXMLElement *pImage = pDesc->GetContentElementByTag(IMAGE_TAG);
	if (pImage)
		{
		pType->iPaintStyle = paintImage;
		pType->Image.InitFromXML(*pImage);
		}

	//	Miscelleanous settings

	pParticles->SetBounds(pType->rRadius);

	//	Create the group

	SParticleArray *pGroup;
	int iCount = pDesc->GetAttributeInteger(COUNT_ATTRIB);

	pParticles->CreateGroup(pType, iCount, &pGroup);

	//	Add to system

	if (error = pParticles->AddToSystem(System))
		{
		delete pParticles;
		return error;
		}

	//	Done

	if (retpEffect)
		*retpEffect = pParticles;

	return NOERROR;
	}

ALERROR CParticleEffect::CreateEmpty (CSystem &System,
									  CSpaceObject *pAnchor,
									  const CVector &vPos,
									  const CVector &vVel,
									  CParticleEffect **retpEffect)

//	CreateEmpty
//
//	Creates an empty effect. Call AddGroup to add particle groups

	{
	ALERROR error;
	CParticleEffect *pParticles;

	pParticles = new CParticleEffect(System.GetUniverse());
	if (pParticles == NULL)
		return ERR_MEMORY;

	pParticles->Place(vPos, vVel);
	pParticles->SetObjectDestructionHook();
	pParticles->m_pAnchor = pAnchor;

	//	Add to system

	if (error = pParticles->AddToSystem(System))
		{
		delete pParticles;
		return error;
		}

	//	Done

	if (retpEffect)
		*retpEffect = pParticles;

	return NOERROR;
	}

ALERROR CParticleEffect::CreateExplosion (CSystem &System,
										  CSpaceObject *pAnchor,
										  const CVector &vPos,
										  const CVector &vVel,
										  int iCount,
										  Metric rAveSpeed,
										  int iTotalLifespan,
										  int iParticleLifespan,
										  const CObjectImageArray &Image,
										  CParticleEffect **retpEffect)

//	CreateExplosion
//
//	Creates an explosion of particles

	{
	ALERROR error;
	CParticleEffect *pParticles;

	pParticles = new CParticleEffect(System.GetUniverse());
	if (pParticles == NULL)
		return ERR_MEMORY;

	pParticles->Place(vPos, (pAnchor == NULL ? vVel : CVector()));
	pParticles->SetObjectDestructionHook();
	pParticles->m_pAnchor = pAnchor;

	//	Create the type based on the descriptor

	SParticleType *pType = new SParticleType;

	pType->m_fLifespan = true;
	pType->iLifespan = iParticleLifespan;
	if (iTotalLifespan)
		{
		pType->m_fRegenerate = true;
		pType->iRegenerationTimer = iTotalLifespan;
		}

	pType->rAveSpeed = rAveSpeed;

	pType->m_fMaxRadius = false;
	pType->rRadius = pType->iLifespan * rAveSpeed;
	pType->rHoleRadius = 0.0;

	pType->m_fDrag = true;
	pType->rDampening = 0.98;

	pParticles->SetBounds(pType->rRadius);

	//	Set the image

	if (!Image.IsLoaded())
		{
		pType->iPaintStyle = paintSmoke;
		}
	else
		{
		pType->iPaintStyle = paintImage;
		pType->Image = Image;
		}

	//	Create the group

	SParticleArray *pGroup;
	pParticles->CreateGroup(pType, iCount, &pGroup);

	//	Add to system

	if (error = pParticles->AddToSystem(System))
		{
		delete pParticles;
		return error;
		}

	//	Done

	if (retpEffect)
		*retpEffect = pParticles;

	return NOERROR;
	}

ALERROR CParticleEffect::CreateGeyser (CSystem &System,
									   CSpaceObject *pAnchor,
									   const CVector &vPos,
									   const CVector &vVel,
									   int iTotalLifespan,
									   int iParticleCount,
									   ParticlePainting iParticleStyle,
									   int iParticleLifespan,
									   Metric rAveSpeed,
									   int iDirection,
									   int iAngleWidth,
									   CParticleEffect **retpEffect)

//	CreateGeyser
//
//	Creates a particle geyser

	{
	ALERROR error;
	CParticleEffect *pParticles;

	pParticles = new CParticleEffect(System.GetUniverse());
	if (pParticles == NULL)
		return ERR_MEMORY;

	pParticles->Place(vPos, (pAnchor == NULL ? vVel : CVector()));
	pParticles->SetObjectDestructionHook();
	pParticles->m_pAnchor = pAnchor;

	//	Geysers are just effects; they do not block shots. We need this because
	//	otherwise a mining laser might be block by its own geyser.

	pParticles->SetCannotBeHit();

	//	Create the type based on the descriptor

	SParticleType *pType = new SParticleType;
	pType->m_fWake = true;

	pType->m_fLifespan = true;
	pType->iLifespan = iParticleLifespan;

	pType->m_fRegenerate = true;
	pType->iRegenerationTimer = iTotalLifespan;

	pType->iDirection = iDirection;
	pType->iDirRange = iAngleWidth;
	pType->rAveSpeed = rAveSpeed;

	pType->m_fMaxRadius = false;
	pType->rRadius = pType->iLifespan * pType->rAveSpeed;
	pType->rDampening = 0.75;

	pType->iPaintStyle = iParticleStyle;

	pParticles->AddGroup(pType, iParticleCount);

	//	Add to system

	if (error = pParticles->AddToSystem(System))
		{
		delete pParticles;
		return error;
		}

	//	Done

	if (retpEffect)
		*retpEffect = pParticles;

	return NOERROR;
	}

void CParticleEffect::CreateGroup (SParticleType *pType, int iCount, SParticleArray **retpGroup)

//	CreateGroup
//
//	Creates a group of particles

	{
	SParticleArray *pGroup = new SParticleArray;

	pGroup->pType = pType;
	pGroup->iAlive = iCount;
	pGroup->iCount = iCount;
	pGroup->pParticles = new SParticle[iCount];

	pGroup->pNext = m_pFirstGroup;
	m_pFirstGroup = pGroup;

	//	Create the particles

	for (int i = 0; i < iCount; i++)
		{
		pGroup->pParticles[i].iDestiny = mathRandom(0, 360);

		//	Lifespan

		if (pType->m_fLifespan)
			{
			if (pType->m_fRegenerate)
				pGroup->pParticles[i].iLifeLeft = mathRandom(1, pType->iLifespan);
			else
				{
				int iVariation = pType->iLifespan / 10;
				pGroup->pParticles[i].iLifeLeft = pType->iLifespan + mathRandom(-iVariation, iVariation);
				}
			}
		else
			pGroup->pParticles[i].iLifeLeft = 0;

		//	Position

		if (pType->m_fMaxRadius)
			{
			Metric rDist = pType->rHoleRadius + (mathRandom(1, 100) * (pType->rRadius - pType->rHoleRadius) / 100.0);
			pGroup->pParticles[i].vPos = PolarToVector(mathRandom(0, 359), rDist);
			}

		//	Velocity

		SetParticleSpeed(pType, &pGroup->pParticles[i]);
		}

	//	Done

	if (retpGroup)
		*retpGroup = pGroup;
	}

EDamageResults CParticleEffect::OnDamage (SDamageCtx &Ctx)

//	Damage
//
//	Damage the particle field

	{
	DEBUG_TRY

	//	Create hit effect

	Ctx.GetDesc().CreateHitEffect(GetSystem(), Ctx);

	return damagePassthrough;

	DEBUG_CATCH
	}

void CParticleEffect::OnMove (SUpdateCtx &Ctx, const CVector &vOldPos, Metric rSeconds)

//	OnMove
//
//	Move hook

	{
	//	If we've got an anchor then move the effect along with the anchor

	if (m_pAnchor)
		SetPos(GetPos() + (m_pAnchor->GetVel() * rSeconds));
	}

void CParticleEffect::ObjectDestroyedHook (const SDestroyCtx &Ctx)

//	ObjectDestroyedHook
//
//	Handle it when another object is destroyed

	{
	//	If our source is destroyed, clear it

	if (Ctx.Obj == m_pAnchor)
		m_pAnchor = NULL;
	}

void CParticleEffect::OnPaint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	OnPaint
//
//	Paint the effect

	{
	SParticleArray *pGroup = m_pFirstGroup;
	while (pGroup)
		{
		SParticleType *pType = pGroup->pType;

		//	Paint image

		switch (pType->iPaintStyle)
			{
			case paintFlame:
				PaintFlameParticles(pGroup, Dest, x, y, Ctx);
				break;

			case paintImage:
				{
				int iTick = GetSystem()->GetTick();

				SParticle *pParticle = pGroup->pParticles;
				SParticle *pEnd = pParticle + pGroup->iCount;
				while (pParticle < pEnd)
					{
					if (pParticle->IsValid())
						{
						int x, y;

						Ctx.XForm.Transform(GetPos() + pParticle->vPos, &x, &y);
						pType->Image.PaintImage(Dest, x, y, iTick + pParticle->iDestiny, 0);
						}

					pParticle++;
					}
				break;
				}

			case paintSmoke:
				PaintSmokeParticles(pGroup, Dest, x, y, Ctx);
				break;

			default:
				{
				CG32bitPixel rgbColor = CG32bitPixel(0, 255, 0);

				SParticle *pParticle = pGroup->pParticles;
				SParticle *pEnd = pParticle + pGroup->iCount;
				while (pParticle < pEnd)
					{
					if (pParticle->IsValid())
						{
						int x, y;

						Ctx.XForm.Transform(GetPos() + pParticle->vPos, &x, &y);
						Dest.DrawDot(x, y, rgbColor, markerRoundDot);
						}

					pParticle++;
					}
				}
			}

		pGroup = pGroup->pNext;
		}
	}

void CParticleEffect::OnReadFromStream (SLoadCtx &Ctx)

//	OnReadFromStream
//
//	Read object data from a stream
//
//	CString		m_sName
//	DWORD		m_pAnchor (CSpaceObject ref)
//	CDamageSource	m_Source
//
//	DWORD		type: iPaintStyle (or 0xffffffff if no more groups)
//	Image		type: Image
//	DWORD		type: rgbColor
//	DWORD		type: iRegenerationTimer
//	DWORD		type: iLifespan
//	Metric		type: rAveSpeed
//	DWORD		type: iDirection
//	DWORD		type: iDirRange
//	Metric		type: rRadius
//	Metric		type: rHoleRadius
//	Metric		type: rDampening
//	DamageDesc	type: Damage
//	DWORD		type: flags
//
//	DWORD		array: iAlive 
//
//	DWORD		particle: iDestiny
//	DWORD		particle: iLifeLeft
//	Vector		particle: vPos
//	Vector		particle: vVel

	{
#ifdef DEBUG_LOAD
	::OutputDebugString("CParticleEffect::OnReadFromStream\n");
#endif
	m_sName.ReadFromStream(Ctx.pStream);
	CSystem::ReadObjRefFromStream(Ctx, &m_pAnchor);

	if (Ctx.dwVersion >= 186)
		m_Source.ReadFromStream(Ctx);

	DWORD dwLoad;
	DWORD dwNext;
	Ctx.pStream->Read(dwNext);
	SParticleArray *pLastGroup = NULL;
	while (dwNext != 0xffffffff)
		{
		//	Create a new group

		SParticleArray *pGroup = new SParticleArray;
		if (pLastGroup == NULL)
			m_pFirstGroup = pGroup;
		else
			pLastGroup->pNext = pGroup;

		//	Create a new type

		pGroup->pType = new SParticleType;
		pGroup->pType->iPaintStyle = dwNext;
		pGroup->pType->Image.ReadFromStream(Ctx);

		Ctx.pStream->Read(dwLoad);
		if (Ctx.dwVersion >= 110)
			pGroup->pType->rgbColor = CG32bitPixel::FromDWORD(dwLoad);
		else
			pGroup->pType->rgbColor = CG32bitPixel((WORD)dwLoad);

		Ctx.pStream->Read(pGroup->pType->iRegenerationTimer);
		Ctx.pStream->Read(pGroup->pType->iLifespan);
		Ctx.pStream->Read(pGroup->pType->rAveSpeed);
		Ctx.pStream->Read(pGroup->pType->iDirection);
		Ctx.pStream->Read(pGroup->pType->iDirRange);
		Ctx.pStream->Read(pGroup->pType->rRadius);
		Ctx.pStream->Read(pGroup->pType->rHoleRadius);
		Ctx.pStream->Read(pGroup->pType->rDampening);
		DamageDesc Damage;
		Damage.ReadFromStream(Ctx);

		//	Load type flags

		Ctx.pStream->Read(dwLoad);
		pGroup->pType->m_fMaxRadius =	((dwLoad & 0x00000001) ? true : false);
		pGroup->pType->m_fLifespan =	((dwLoad & 0x00000002) ? true : false);
		pGroup->pType->m_fWake =		((dwLoad & 0x00000004) ? true : false);
		pGroup->pType->m_fRegenerate =	((dwLoad & 0x00000008) ? true : false);
		pGroup->pType->m_fDrag =		((dwLoad & 0x00000010) ? true : false);
		bool bHasDamage =				((dwLoad & 0x00000020) ? true : false);

		//	Init damage desc

		if (bHasDamage)
			{
			pGroup->pType->pDamageDesc = new CWeaponFireDesc;
			pGroup->pType->pDamageDesc->InitFromDamage(Damage);
			pGroup->pType->m_fFreeDesc = true;
			}

		//	Load array of particles

		Ctx.pStream->Read(dwLoad);
		pGroup->iAlive = dwLoad;
		pGroup->iCount = dwLoad;
		pGroup->pParticles = new SParticle [pGroup->iCount];

		for (int i = 0; i < pGroup->iAlive; i++)
			{
			Ctx.pStream->Read(pGroup->pParticles[i].iDestiny);
			Ctx.pStream->Read(pGroup->pParticles[i].iLifeLeft);
			pGroup->pParticles[i].vPos.ReadFromStream(*Ctx.pStream);
			pGroup->pParticles[i].vVel.ReadFromStream(*Ctx.pStream);
			}

		//	Next

		pGroup->pNext = NULL;
		pLastGroup = pGroup;
		Ctx.pStream->Read(dwNext);
		}
	}

void CParticleEffect::OnUpdate (SUpdateCtx &Ctx, Metric rSecondsPerTick)

//	OnUpdate
//
//	Update the effect

	{
	int iTick = GetSystem()->GetTick() + GetDestiny();

	//	Do not bother updating everything if we are far from the POV

	bool bFarAway = false;
	if (GetUniverse().GetPOV() 
			&& GetUniverse().GetCurrentSystem() == GetSystem())
		{
		Metric rPOVDist2 = (GetPos() - GetUniverse().GetPOV()->GetPos()).Length2();
		Metric rMaxUpdateDist2 = LIGHT_SECOND * LIGHT_SECOND * 3600;
		bFarAway = (rPOVDist2 > rMaxUpdateDist2);
		}

	//	Update the particles

	SParticleArray *pGroup = m_pFirstGroup;
	while (pGroup)
		{
		SParticleType *pType = pGroup->pType;
		Metric rMinSpeed = pType->rAveSpeed * 0.01;
		Metric rMinSpeed2 = rMinSpeed * rMinSpeed;

		//	Max distance for a particle in this group

		Metric rMaxDist2 = pType->rRadius * pType->rRadius;
		Metric rMinDist2 = pType->rHoleRadius * pType->rHoleRadius;

		//	If the particle field causes damage then we need to
		//	compute its average density

		int iDensity = 0;
		if (pType->pDamageDesc)
			{
			Metric rRadius2 = pType->rRadius * pType->rRadius;
			Metric rArea = rRadius2 / (LIGHT_SECOND * LIGHT_SECOND);
			iDensity = (int)(4 * pGroup->iCount / rArea);
			}

		//	Get an array of objects in the particle field that
		//	may influence the particles

		CSpaceObject *Objects[ctMaxObjsInField];
		int iObjCount = 0;
		if (!bFarAway && (pType->m_fWake || pType->pDamageDesc))
			{
			Metric rMaxInfluenceDist2 = rMaxDist2;
			for (int i = 0; i < GetSystem()->GetObjectCount(); i++)
				{
				CSpaceObject *pObj = GetSystem()->GetObject(i);

				if (pObj 
						&& pObj->GetCategory() == catShip
						&& !pObj->IsIntangible()
						&& pObj->CanBeHit()
						&& pObj != this)
					{
					CVector vDist = GetPos() - pObj->GetPos();
					Metric rDist2 = vDist.Length2();

					if (rDist2 < rMaxInfluenceDist2 
							&& (pObj->GetVel().Length2() > g_KlicksPerPixel)
							&& iObjCount < ctMaxObjsInField)
						{
						Objects[iObjCount++] = pObj;

						//	See if the object should take damage

						if (pType->pDamageDesc)
							{
							CVector vDeltaV = pObj->GetVel() - GetVel();
							int iSpeed = (int)(vDeltaV.Length() / g_KlicksPerPixel);
							if (iSpeed == 0)
								iSpeed = 1;

							if (mathRandom(1, 1000) < (iDensity * iSpeed))
								{
								SDamageCtx DamageCtx(pObj,
										*pType->pDamageDesc,
										NULL,
										CDamageSource(this, killedByDamage),
										this,
										0.0,
										VectorToPolar(vDeltaV),
										pObj->GetPos());

								pObj->Damage(DamageCtx);
								}
							}
						}
					}
				}
			}

		//	If we're computing drag then we need to compute the new velocity
		//	of the whole particle system

		CVector vNewVel;
		if (pType->m_fDrag)
			vNewVel = GetVel() * g_SpaceDragFactor;

		//	Iterate over all particles

		SParticle *pParticle = pGroup->pParticles;
		SParticle *pEnd = pParticle + pGroup->iCount;
		while (pParticle < pEnd)
			{
			if (pParticle->IsValid())
				{
				//	Lifespan. If we're far away and we're regenerating,
				//	then don't bother to compute lifespan.

				if (pType->m_fLifespan 
						&& !(bFarAway && (pType->m_fRegenerate && pType->iRegenerationTimer)))
					{
					if (--pParticle->iLifeLeft == 0)
						{
						//	Do we regenerate?

						if (pType->m_fRegenerate && pType->iRegenerationTimer)
							{
							pParticle->iLifeLeft = pType->iLifespan;
							pParticle->vPos = NullVector;

							//	Speed

							SetParticleSpeed(pType, pParticle);
							}

						//	Otherwise we die

						else
							{
							pParticle->iLifeLeft = -1;
							pGroup->iAlive--;
							pParticle++;
							continue;
							}
						}
					}

				//	Update the position

				if (!bFarAway)
					{
					pParticle->vPos = pParticle->vPos + pParticle->vVel;

					//	Change the velocity to keep the particles within
					//	the radius

					if (pType->m_fMaxRadius)
						{
						Metric rDist2 = pParticle->vPos.Length2();

						if (pType->m_fMaxRadius && rDist2 > rMaxDist2)
							{
							CVector vChange = pParticle->vPos + g_KlicksPerPixel * pParticle->vPos.Perpendicular().Normal();
							pParticle->vVel = pParticle->vVel - (0.00005 * vChange);
							}
						else if (rDist2 < rMinDist2)
							{
							CVector vNormal = pParticle->vPos.Normal();
							CVector vChange = g_KlicksPerPixel * (400 * vNormal - 50 * vNormal.Perpendicular());
							pParticle->vVel = pParticle->vVel + (0.00005 * vChange);
							}

						//	If we're inside the ring and we react to ships passing through
						//	then we dampen appropriately.

						else if (pType->m_fWake)
							pParticle->vVel = pParticle->vVel * pType->rDampening;

						//	Otherwise, keep the particle in motions

						else
							{
							Metric rVel2 = pParticle->vVel.Length2();
							if (rVel2 <= rMinSpeed2)
								SetParticleSpeed(pType, pParticle);
							else
								pParticle->vVel = pParticle->vVel * pType->rDampening;
							}
						}

					if (pType->m_fDrag)
						{
						//	Compute the new absolute velocity (after drag)
						CVector vAbsolute = pType->rDampening * (pParticle->vVel + GetVel());

						//	The particle velocity is the absolute vel minus the
						//	system velocity.
						pParticle->vVel = vAbsolute - vNewVel;
						}

					//	Change the velocity based on influences from other objects

					if (pType->m_fWake && (iTick % 4) == 0)
						{
						for (int i = 0; i < iObjCount; i++)
							{
							Metric rDist2 = (Objects[i]->GetPos() - (pParticle->vPos + GetPos())).Length2();
							if (rDist2 < g_KlicksPerPixel * g_KlicksPerPixel * 1000)
								{
								if (Objects[i]->GetVel().Dot(pParticle->vVel) < Objects[i]->GetVel().Length2())
									pParticle->vVel = pParticle->vVel + 0.2 * Objects[i]->GetVel();
								}
							}
						}
					}
				}

			pParticle++;
			}

		//	Regeneration timer

		if (pType->m_fRegenerate && pType->iRegenerationTimer)
			pType->iRegenerationTimer--;

		//	If there are no more particles left alive in this group then kill
		//	the group

		if (pGroup->iAlive == 0)
			{
			SParticleArray *pNext = pGroup->pNext;
			SParticleArray *pPrev = NULL;

			//	Find the previous group

			SParticleArray *pFind = m_pFirstGroup;
			while (pFind != pGroup)
				{
				if (pPrev)
					pPrev = pPrev->pNext;
				else
					pPrev = m_pFirstGroup;

				pFind = pFind->pNext;
				}

			//	Fix up the linked list

			if (pPrev)
				pPrev->pNext = pNext;
			else
				m_pFirstGroup = pNext;

			//	Delete the group

			delete pGroup;
			pGroup = pNext;
			}

		//	Otherwise, next group

		else
			pGroup = pGroup->pNext;
		}

	//	If we have no more groups then we destroy ourselves

	if (m_pFirstGroup == NULL)
		{
		Destroy(removedFromSystem, CDamageSource());
		return;
		}

	//	If we're moving, slow down

	if (!GetVel().IsNull())
		AddDrag(g_SpaceDragFactor);
	}

void CParticleEffect::PaintFlameParticles (SParticleArray *pGroup, CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	PaintFlameParticles
//
//	Paint particles as flame

	{
	SParticleType *pType = pGroup->pType;
	SParticle *pParticle = pGroup->pParticles;
	SParticle *pEnd = pParticle + pGroup->iCount;
	while (pParticle < pEnd)
		{
		if (pParticle->IsValid())
			{
			//	Compute color based on particle's lifetime

			CG32bitPixel rgbColor;
			int iColor = 0;
			int iFade = 0;
			int iFade2 = 0;
			if (pType->iLifespan)
				{
				iColor = 200 - (200 * pParticle->iLifeLeft / pType->iLifespan);
				iFade = 255 * pParticle->iLifeLeft / pType->iLifespan;
				iFade2 = iFade / 2;
				}

			if (iColor <= 50)
				rgbColor = CG32bitPixel::Fade(CG32bitPixel(255, 255, 255),
						CG32bitPixel(255, 228, 110),
						2 * iColor);
			else if (iColor <= 150)
				rgbColor = CG32bitPixel::Fade(CG32bitPixel(255, 228, 110),
						CG32bitPixel(255, 59, 27),
						iColor - 50);
			else
				rgbColor = CG32bitPixel(255, 59, 27);

			//	Compute the size

			int iSize = (pParticle->iDestiny % 6);

			//	Compute the position

			int x, y;
			Ctx.XForm.Transform(GetPos() + pParticle->vPos, &x, &y);

			//	Paint

			switch (iSize)
				{
				case 0:
					Dest.SetPixelTrans(x, y, rgbColor, iFade);
					break;

				case 1:
					Dest.SetPixelTrans(x, y, rgbColor, iFade);
					Dest.SetPixelTrans(x + 1, y, rgbColor, iFade2);
					Dest.SetPixelTrans(x, y + 1, rgbColor, iFade2);
					break;

				case 2:
					Dest.SetPixelTrans(x, y, rgbColor, iFade);
					Dest.SetPixelTrans(x + 1, y, rgbColor, iFade2);
					Dest.SetPixelTrans(x, y + 1, rgbColor, iFade2);
					Dest.SetPixelTrans(x - 1, y, rgbColor, iFade2);
					Dest.SetPixelTrans(x, y - 1, rgbColor, iFade2);
					break;

				case 3:
					Dest.SetPixelTrans(x, y, rgbColor, iFade);
					Dest.SetPixelTrans(x + 1, y, rgbColor, iFade);
					Dest.SetPixelTrans(x, y + 1, rgbColor, iFade);
					Dest.SetPixelTrans(x - 1, y, rgbColor, iFade);
					Dest.SetPixelTrans(x, y - 1, rgbColor, iFade);
					Dest.SetPixelTrans(x + 1, y + 1, rgbColor, iFade2);
					Dest.SetPixelTrans(x + 1, y - 1, rgbColor, iFade2);
					Dest.SetPixelTrans(x - 1, y + 1, rgbColor, iFade2);
					Dest.SetPixelTrans(x - 1, y - 1, rgbColor, iFade2);
					break;

				case 4:
				default:
					{
					Dest.Fill(x - 1,
							y - 1,
							3,
							3,
							CG32bitPixel(rgbColor, (BYTE)iFade));

					Dest.SetPixelTrans(x + 2, y, rgbColor, iFade2);
					Dest.SetPixelTrans(x, y + 2, rgbColor, iFade2);
					Dest.SetPixelTrans(x - 2, y, rgbColor, iFade2);
					Dest.SetPixelTrans(x, y - 2, rgbColor, iFade2);
					}
				}
			}

		pParticle++;
		}
	}

void CParticleEffect::PaintLRSForeground (CG32bitImage &Dest, int x, int y, const ViewportTransform &Trans)

//	PaintLRSForeground
//
//	Paints the object on an LRS

	{
	SParticleArray *pGroup = m_pFirstGroup;
	while (pGroup)
		{
		CG32bitPixel rgbColor = CG32bitPixel(128, 128, 128);

		for (int i = 0; i < pGroup->iCount; i++)
			if ((i % 10) == 0
					&& pGroup->pParticles[i].IsValid())
			{
			int x, y;

			Trans.Transform(GetPos() + pGroup->pParticles[i].vPos, &x, &y);
			Dest.SetPixel(x, y, rgbColor);
			}

		pGroup = pGroup->pNext;
		}
	}

void CParticleEffect::PaintSmokeParticles (SParticleArray *pGroup, CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	PaintSmokeParticles
//
//	Paint particles as smoke

	{
	SParticleType *pType = pGroup->pType;
	SParticle *pParticle = pGroup->pParticles;
	SParticle *pEnd = pParticle + pGroup->iCount;
	while (pParticle < pEnd)
		{
		if (pParticle->IsValid())
			{
			//	Compute color

			int iColor = 64 + (pParticle->iDestiny % 192);
			CG32bitPixel rgbColor = CG32bitPixel(iColor, iColor, iColor);

			//	Compute how much to fade out the smoke

			int iFade = 0;
			int iSize = 0;
			if (pType->iLifespan)
				{
				iFade = 255 * pParticle->iLifeLeft / pType->iLifespan;
				iSize = 5 - (5 * pParticle->iLifeLeft /  pType->iLifespan);
				}

			int iFade2 = iFade / 2;

			//	Compute the position

			int x, y;
			Ctx.XForm.Transform(GetPos() + pParticle->vPos, &x, &y);

			//	Paint

			switch (iSize)
				{
				case 0:
					Dest.SetPixelTrans(x, y, rgbColor, iFade);
					break;

				case 1:
					Dest.SetPixelTrans(x, y, rgbColor, iFade);
					Dest.SetPixelTrans(x + 1, y, rgbColor, iFade2);
					Dest.SetPixelTrans(x, y + 1, rgbColor, iFade2);
					break;

				case 2:
					Dest.SetPixelTrans(x, y, rgbColor, iFade);
					Dest.SetPixelTrans(x + 1, y, rgbColor, iFade2);
					Dest.SetPixelTrans(x, y + 1, rgbColor, iFade2);
					Dest.SetPixelTrans(x - 1, y, rgbColor, iFade2);
					Dest.SetPixelTrans(x, y - 1, rgbColor, iFade2);
					break;

				case 3:
					Dest.SetPixelTrans(x, y, rgbColor, iFade);
					Dest.SetPixelTrans(x + 1, y, rgbColor, iFade);
					Dest.SetPixelTrans(x, y + 1, rgbColor, iFade);
					Dest.SetPixelTrans(x - 1, y, rgbColor, iFade);
					Dest.SetPixelTrans(x, y - 1, rgbColor, iFade);
					Dest.SetPixelTrans(x + 1, y + 1, rgbColor, iFade2);
					Dest.SetPixelTrans(x + 1, y - 1, rgbColor, iFade2);
					Dest.SetPixelTrans(x - 1, y + 1, rgbColor, iFade2);
					Dest.SetPixelTrans(x - 1, y - 1, rgbColor, iFade2);
					break;

				case 4:
				default:
					{
					Dest.Fill(x - 1,
							y - 1,
							3,
							3,
							CG32bitPixel(192, 192, 192, iFade));

					Dest.SetPixelTrans(x + 2, y, rgbColor, iFade2);
					Dest.SetPixelTrans(x, y + 2, rgbColor, iFade2);
					Dest.SetPixelTrans(x - 2, y, rgbColor, iFade2);
					Dest.SetPixelTrans(x, y - 2, rgbColor, iFade2);
					}
				}
			}

		pParticle++;
		}
	}

bool CParticleEffect::PointInObject (const CVector &vObjPos, const CVector &vPointPos) const

//	PointInObject
//
//	Return true if the point intersects the object. Our answer
//	is probabilistic depending on the density of the field

	{
	DEBUG_TRY

	CVector vDist = vPointPos - vObjPos;
	Metric rDist2 = vDist.Length2();

	SParticleArray *pGroup = m_pFirstGroup;
	while (pGroup)
		{
		SParticleType *pType = pGroup->pType;
		Metric rRadius2 = pType->rRadius * pType->rRadius;
		if (rDist2 < rRadius2)
			{
			Metric rArea = rRadius2 / (LIGHT_SECOND * LIGHT_SECOND);
			int iDensity = (int)(pGroup->iCount / rArea);
			if (mathRandom(1, 1000) < iDensity)
				return true;
			}

		pGroup = pGroup->pNext;
		}

	return false;

	DEBUG_CATCH
	}

void CParticleEffect::OnWriteToStream (IWriteStream *pStream)

//	OnWriteToStream
//
//	Write the object's data to stream
//
//	CString		m_sName
//	DWORD		m_pAnchor (CSpaceObject ref)
//	CDamageSource	m_Source
//
//	DWORD		type: iPaintStyle (or 0xffffffff if no more groups)
//	Image		type: Image
//	DWORD		type: wColor
//	DWORD		type: iRegenerationTimer
//	DWORD		type: iLifespan
//	Metric		type: rAveSpeed
//	DWORD		type: iDirection
//	DWORD		type: iDirRange
//	Metric		type: rRadius
//	Metric		type: rHoleRadius
//	Metric		type: rDampening
//	DamageDesc	type: Damage
//	DWORD		type: flags
//
//	DWORD		array: iAlive 
//
//	DWORD		particle: iDestiny
//	DWORD		particle: iLifeLeft
//	Vector		particle: vPos
//	Vector		particle: vVel

	{
	DWORD dwSave;

	m_sName.WriteToStream(pStream);
	WriteObjRefToStream(m_pAnchor, pStream);

	m_Source.WriteToStream(pStream);

	//	Save each group

	SParticleArray *pGroup = m_pFirstGroup;
	while (pGroup)
		{
		SParticleType *pType = pGroup->pType;

		//	Save the type information

		pStream->Write(pType->iPaintStyle);
		pType->Image.WriteToStream(pStream);

		dwSave = pType->rgbColor.AsDWORD();
		pStream->Write(dwSave);

		pStream->Write(pType->iRegenerationTimer);
		pStream->Write(pType->iLifespan);
		pStream->Write(pType->rAveSpeed);
		pStream->Write(pType->iDirection);
		pStream->Write(pType->iDirRange);
		pStream->Write(pType->rRadius);
		pStream->Write(pType->rHoleRadius);
		pStream->Write(pType->rDampening);
		if (pType->pDamageDesc)
			pType->pDamageDesc->GetDamage().WriteToStream(pStream);
		else
			{
			DamageDesc Dummy;
			Dummy.WriteToStream(pStream);
			}

		DWORD dwSave = 0;
		dwSave |= (pType->m_fMaxRadius ?	0x00000001 : 0);
		dwSave |= (pType->m_fLifespan ?		0x00000002 : 0);
		dwSave |= (pType->m_fWake ?			0x00000004 : 0);
		dwSave |= (pType->m_fRegenerate ?	0x00000008 : 0);
		dwSave |= (pType->m_fDrag ?			0x00000010 : 0);
		dwSave |= (pType->pDamageDesc ?		0x00000020 : 0);
		pStream->Write(dwSave);

		//	Save the array of particles

		pStream->Write(pGroup->iAlive);
		int iCount = pGroup->iAlive;

		SParticle *pParticle = pGroup->pParticles;
		SParticle *pEnd = pParticle + pGroup->iCount;
		while (pParticle < pEnd)
			{
			if (pParticle->IsValid())
				{
				pStream->Write(pParticle->iDestiny);
				pStream->Write(pParticle->iLifeLeft);
				pParticle->vPos.WriteToStream(*pStream);
				pParticle->vVel.WriteToStream(*pStream);
				
				iCount--;
				}

			pParticle++;
			}

		//	iAlive better match the actual count
		ASSERT(iCount == 0);

		pGroup = pGroup->pNext;
		}

	//	Mark the end with 0xffffffff

	dwSave = 0xffffffff;
	pStream->Write(dwSave);
	}

void CParticleEffect::SetParticleSpeed (SParticleType *pType, SParticle *pParticle)

//	SetParticleSpeed
//
//	Sets the speed to default based on the type.

	{
	Metric rSpeed = mathRandom(1, 100) * (pType->rAveSpeed / 100.0);
	if (pType->iDirection == -1)
		pParticle->vVel = PolarToVector(mathRandom(0, 359), rSpeed);
	else
		{
		int iAngle = AngleMod(pType->iDirection + mathRandom(0, 2 * pType->iDirRange) - pType->iDirRange);
		pParticle->vVel = PolarToVector(iAngle, rSpeed);
		}
	}
