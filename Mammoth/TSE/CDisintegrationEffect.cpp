//	CDisintegrationEffect.cpp
//
//	CDisintegrationEffect class

#include "PreComp.h"


#define PARTICLE_COUNT							5000
#define LIFETIME								180
#define DISPERSE_POINT							(LIFETIME / 2)

#define RGB_BASE_COLOR							(CG32bitPixel(0x80, 0xff, 0xff))
#define RGB_PARTICLE_COLOR						(CG32bitPixel(0x80, 0xff, 0xff))

#define FIXED_POINT								256

CDisintegrationEffect::CDisintegrationEffect (CUniverse &Universe) : TSpaceObjectImpl(Universe),
		m_pParticles(NULL),
		m_iParticleCount(0)

//	CDisintegrationEffect constructor

	{
	}

CDisintegrationEffect::~CDisintegrationEffect (void)

//	CDisintegrationEffect destructor

	{
	if (m_pParticles)
		delete [] m_pParticles;
	}

ALERROR CDisintegrationEffect::Create (CSystem &System,
				const CVector &vPos,
				const CVector &vVel,
				const CObjectImageArray &MaskImage,
				int iMaskTick,
				int iMaskRotation,
				CDisintegrationEffect **retpEffect)

//	Create
//
//	Creates a new effects object

	{
	DEBUG_TRY

	ALERROR error;
	CDisintegrationEffect *pEffect;

	pEffect = new CDisintegrationEffect(System.GetUniverse());
	if (pEffect == NULL)
		return ERR_MEMORY;

	pEffect->Place(vPos, vVel);

	ASSERT(MaskImage.GetBitmapUNID());
	pEffect->m_MaskImage = MaskImage;
	pEffect->m_iMaskTick = iMaskTick;
	pEffect->m_iMaskRotation = iMaskRotation;
	pEffect->m_iTick = 0;

	ASSERT(pEffect->m_iMaskTick == 0);
	ASSERT(pEffect->m_iMaskRotation < pEffect->m_MaskImage.GetRotationCount() && pEffect->m_iMaskRotation >= 0);

	//	Create the particles

	pEffect->m_iParticleCount = PARTICLE_COUNT;
	pEffect->m_pParticles = new SParticle [pEffect->m_iParticleCount];
	for (int i = 0; i < pEffect->m_iParticleCount; i++)
		{
		SParticle *pParticle = &pEffect->m_pParticles[i];
		pEffect->InitParticle(pParticle);
		}

	//	Add to system

	if (error = pEffect->AddToSystem(System))
		{
		delete pEffect;
		return error;
		}

	//	Done

	if (retpEffect)
		*retpEffect = pEffect;

	return NOERROR;

	DEBUG_CATCH
	}

void CDisintegrationEffect::InitParticle (SParticle *pParticle)

//	InitParticle
//
//	Initialize a particle

	{
	pParticle->iTicksLeft = mathRandom(10, 30);
	pParticle->x = 0;
	pParticle->y = 0;
	IntPolarToVector(mathRandom(0, 359),
			(Metric)(mathRandom(0, (FIXED_POINT * 2))),
			&pParticle->xV,
			&pParticle->yV);

	int iFade = mathRandom(25, 100);
	pParticle->rgbColor = CG32bitPixel(
			(iFade * 0x80 / 100),
			(iFade * 0xff / 100),
			(iFade * 0xff / 100));
	}

void CDisintegrationEffect::OnMove (const CVector &vOldPos, Metric rSeconds)

//	OnMove
//
//	Move hook

	{
	}

void CDisintegrationEffect::ObjectDestroyedHook (const SDestroyCtx &Ctx)

//	ObjectDestroyedHook
//
//	Handle it when another object is destroyed

	{
	}

void CDisintegrationEffect::OnPaint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	OnPaint
//
//	Paints the effect

	{
	int i;

	//	Paint the flashing silhouette

	if ((m_iTick < (DISPERSE_POINT / 2))
			&& (m_iTick % 2))
		{
		int iFade = 100 - ((m_iTick * 100) / (DISPERSE_POINT / 2));
		CG32bitPixel rgbColor = CG32bitPixel(
				iFade * 0x80 / 100,
				iFade * 0xff / 100,
				iFade * 0xff / 100);
		m_MaskImage.PaintSilhoutte(Dest, x, y, m_iMaskTick, m_iMaskRotation, rgbColor);
		}

	//	Paint the noise

	if (m_iTick < DISPERSE_POINT)
		{
		SParticle *pParticle = m_pParticles;
		for (i = 0; i < m_iParticleCount; i++, pParticle++)
			if (pParticle->iTicksLeft > 0
					&& m_MaskImage.PointInImage(pParticle->x / FIXED_POINT, pParticle->y / FIXED_POINT, m_iMaskTick, m_iMaskRotation))
				Dest.SetPixel(x + (pParticle->x / FIXED_POINT),
						y + (pParticle->y / FIXED_POINT),
						pParticle->rgbColor);
		}
	else if (m_iTick == DISPERSE_POINT)
		{
		SParticle *pParticle = m_pParticles;
		for (i = 0; i < m_iParticleCount; i++, pParticle++)
			if (pParticle->iTicksLeft > 0
					&& m_MaskImage.PointInImage(pParticle->x / FIXED_POINT, pParticle->y / FIXED_POINT, m_iMaskTick, m_iMaskRotation))
				Dest.SetPixel(x + (pParticle->x / FIXED_POINT),
						y + (pParticle->y / FIXED_POINT),
						pParticle->rgbColor);
			else
				pParticle->iTicksLeft = 0;
		}
	else
		{
		SParticle *pParticle = m_pParticles;
		for (i = 0; i < m_iParticleCount; i++, pParticle++)
			if (pParticle->iTicksLeft > 0)
				Dest.SetPixel(x + (pParticle->x / FIXED_POINT),
						y + (pParticle->y / FIXED_POINT),
						pParticle->rgbColor);
		}
	}

void CDisintegrationEffect::OnReadFromStream (SLoadCtx &Ctx)

//	OnReadFromStream
//
//	Read object data from a stream
//
//	Image		m_MaskImage
//	DWORD		m_iMaskTick
//	DWORD		m_iMaskRotation
//	DWORD		m_iTick
//	DWORD		m_iParticleCount
//	SParticle[]

	{
#ifdef DEBUG_LOAD
	::OutputDebugString("CDisintegrationEffect::OnReadFromStream\n");
#endif
	m_MaskImage.ReadFromStream(Ctx);
	Ctx.pStream->Read((char *)&m_iMaskTick, sizeof(DWORD));
	Ctx.pStream->Read((char *)&m_iMaskRotation, sizeof(DWORD));
	Ctx.pStream->Read((char *)&m_iTick, sizeof(DWORD));

	Ctx.pStream->Read((char *)&m_iParticleCount, sizeof(DWORD));
	if (m_iParticleCount)
		{
		m_pParticles = new SParticle [m_iParticleCount];
		Ctx.pStream->Read((char *)m_pParticles, sizeof(SParticle) * m_iParticleCount);
		}
	}

void CDisintegrationEffect::OnUpdate (SUpdateCtx &Ctx, Metric rSecondsPerTick)

//	OnUpdate
//
//	Update the effect

	{
	DEBUG_TRY

	int i;

	//	See if the effect has faded out

	m_iTick++;
	if (m_iTick >= LIFETIME)
		{
		Destroy(removedFromSystem, CDamageSource());
		return;
		}

	//	Otherwise, update the particles

	SParticle *pParticle = m_pParticles;
	for (i = 0; i < m_iParticleCount; i++, pParticle++)
		{
		pParticle->iTicksLeft--;
		pParticle->x += pParticle->xV;
		pParticle->y += pParticle->yV;

		if (m_iTick < DISPERSE_POINT)
			{
			if (pParticle->iTicksLeft <= 0)
				InitParticle(pParticle);
			}
		}

	//	If we're moving, slow down

	UpdateDrag(Ctx, g_SpaceDragFactor);

	DEBUG_CATCH
	}

void CDisintegrationEffect::OnUpdateExtended (const CTimeSpan &ExtraTime)

//	OnUpdateExtended
//
//	Update a prolonged time

	{
	//	Updates don't last for an extended period of time

	Destroy(removedFromSystem, CDamageSource());
	}

void CDisintegrationEffect::OnWriteToStream (IWriteStream *pStream)

//	OnWriteToStream
//
//	Write the object's data to stream
//
//	Image		m_MaskImage
//	DWORD		m_iMaskTick
//	DWORD		m_iMaskRotation
//	DWORD		m_iTick
//	DWORD		m_iParticleCount
//	SParticle[]

	{
	m_MaskImage.WriteToStream(pStream);
	pStream->Write((char *)&m_iMaskTick, sizeof(DWORD));
	pStream->Write((char *)&m_iMaskRotation, sizeof(DWORD));
	pStream->Write((char *)&m_iTick, sizeof(DWORD));

	pStream->Write((char *)&m_iParticleCount, sizeof(DWORD));
	pStream->Write((char *)m_pParticles, sizeof(SParticle) * m_iParticleCount);
	}

