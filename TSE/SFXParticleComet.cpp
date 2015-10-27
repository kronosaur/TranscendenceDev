//	SFXParticleComet.cpp
//
//	Particle Comet SFX

#include "PreComp.h"

#define LENGTH_ATTRIB							CONSTLIT("length")
#define LIFETIME_ATTRIB							CONSTLIT("lifetime")
#define PARTICLE_COUNT_ATTRIB					CONSTLIT("particleCount")
#define PRIMARY_COLOR_ATTRIB					CONSTLIT("primaryColor")
#define SECONDARY_COLOR_ATTRIB					CONSTLIT("secondaryColor")
#define WIDTH_ATTRIB							CONSTLIT("width")

const int POINT_COUNT =							100;

class CParticleCometEffectPainter : public IEffectPainter
	{
	public:
		CParticleCometEffectPainter (CEffectCreator *pCreator);
		~CParticleCometEffectPainter (void);

		//	IEffectPainter virtuals

		virtual bool CanPaintComposite (void) { return true; }
		virtual CEffectCreator *GetCreator (void) { return m_pCreator; }
		virtual int GetLifetime (void) { return m_iLifetime; }
		virtual void GetParam (const CString &sParam, CEffectParamDesc *retValue);
		virtual bool GetParamList (TArray<CString> *retList) const;
		virtual void GetRect (RECT *retRect) const;
		virtual void Paint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);
		virtual void PaintComposite (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);
		virtual bool PointInImage (int x, int y, int iTick, int iVariant = 0, int iRotation = 0) const;

	protected:
		virtual void OnSetParam (CCreatePainterCtx &Ctx, const CString &sParam, const CEffectParamDesc &Value);

	private:
		struct SParticle
			{
			int iPos;
			CVector vScale;
			};

		bool CalcIntermediates (void);
		void InitSplinePoints (void);
		inline int GetMaxAge (void) const { return m_Points.GetCount() - 1; }
		CVector GetParticlePos (int iParticle, int iTick, int iDirection, int *retiAge = NULL, int *retiLength = NULL);

		CEffectCreator *m_pCreator;

		int m_iLifetime;
		int m_iParticleCount;
		int m_iWidth;
		int m_iLength;
		CG32bitPixel m_rgbPrimaryColor;
		CG32bitPixel m_rgbSecondaryColor;

		//	Temporary variables based on shape/style/etc.

		bool m_bInitialized;				//	TRUE if values are valid
		TArray<SParticle> m_Particles;

		static TArray<CVector> m_Points;
	};

//	CParticleCometEffectCreator object

CParticleCometEffectCreator::CParticleCometEffectCreator (void) : 
		m_pSingleton(NULL)

//	CParticleCometEffectCreator constructor

	{
	}

CParticleCometEffectCreator::~CParticleCometEffectCreator (void)

//	CParticleCometEffectCreator destructor

	{
	if (m_pSingleton)
		delete m_pSingleton;
	}

IEffectPainter *CParticleCometEffectCreator::OnCreatePainter (CCreatePainterCtx &Ctx)

//	CreatePainter
//
//	Creates a new painter
	
	{
	//	If we have a singleton, return that

	if (m_pSingleton)
		return m_pSingleton;

	//	Otherwise we need to create a painter with the actual
	//	parameters.

	IEffectPainter *pPainter = new CParticleCometEffectPainter(this);

	//	Initialize the painter parameters

	pPainter->SetParam(Ctx, LENGTH_ATTRIB, m_Length);
	pPainter->SetParam(Ctx, LIFETIME_ATTRIB, m_Lifetime);
	pPainter->SetParam(Ctx, PARTICLE_COUNT_ATTRIB, m_ParticleCount);
	pPainter->SetParam(Ctx, PRIMARY_COLOR_ATTRIB, m_PrimaryColor);
	pPainter->SetParam(Ctx, SECONDARY_COLOR_ATTRIB, m_SecondaryColor);
	pPainter->SetParam(Ctx, WIDTH_ATTRIB, m_Width);

	//	Initialize via GetParameters, if necessary

	InitPainterParameters(Ctx, pPainter);

	//	If we are a singleton, then we only need to create this once.

	if (GetInstance() == instGame)
		{
		pPainter->SetSingleton(true);
		m_pSingleton = pPainter;
		}

	return pPainter;
	}

ALERROR CParticleCometEffectCreator::OnEffectBindDesign (SDesignLoadCtx &Ctx)

//	OnEffectBindDesign
//
//	Resolve loading

	{
	//	Clean up, because we might want to recompute for next time.

	if (m_pSingleton)
		{
		delete m_pSingleton;
		m_pSingleton = NULL;
		}

	return NOERROR;
	}

ALERROR CParticleCometEffectCreator::OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID)

//	OnEffectCreateFromXML
//
//	Initializes from XML

	{
	ALERROR error;

	if (error = m_Length.InitIntegerFromXML(Ctx, pDesc->GetAttribute(LENGTH_ATTRIB)))
		return error;

	if (error = m_Lifetime.InitIntegerFromXML(Ctx, pDesc->GetAttribute(LIFETIME_ATTRIB)))
		return error;

	if (error = m_ParticleCount.InitIntegerFromXML(Ctx, pDesc->GetAttribute(PARTICLE_COUNT_ATTRIB)))
		return error;

	if (error = m_PrimaryColor.InitColorFromXML(Ctx, pDesc->GetAttribute(PRIMARY_COLOR_ATTRIB)))
		return error;

	if (error = m_SecondaryColor.InitColorFromXML(Ctx, pDesc->GetAttribute(SECONDARY_COLOR_ATTRIB)))
		return error;

	if (error = m_Width.InitIntegerFromXML(Ctx, pDesc->GetAttribute(WIDTH_ATTRIB)))
		return error;

	return NOERROR;
	}

//	CParticleCometEffectPainter ------------------------------------------------

TArray<CVector> CParticleCometEffectPainter::m_Points;

CParticleCometEffectPainter::CParticleCometEffectPainter (CEffectCreator *pCreator) :
		m_pCreator(pCreator),
		m_iLifetime(0),
		m_iParticleCount(100),
		m_iWidth(8),
		m_iLength(100),
		m_rgbPrimaryColor(CG32bitPixel(255, 255, 255)),
		m_rgbSecondaryColor(CG32bitPixel(128, 128, 128)),
		m_bInitialized(false)

//	CParticleCometEffectPainter constructor

	{
	InitSplinePoints();
	}

CParticleCometEffectPainter::~CParticleCometEffectPainter (void)

//	CParticleCometEffectPainter destructor

	{
	}

bool CParticleCometEffectPainter::CalcIntermediates (void)

//	CalcIntermediates
//
//	Calculate intermetiate variables

	{
	int i;

	if (m_iParticleCount <= 0)
		return false;

	if (!m_bInitialized)
		{
		//	We need to increase the width a bit because the template (the spline
		//	points) are not full width.

		int iAdjWidth = 18 * m_iWidth / 8;

		//	Create the particle array

		m_Particles.DeleteAll();
		m_Particles.InsertEmpty(m_iParticleCount);

		for (i = 0; i < m_iParticleCount; i++)
			{
			m_Particles[i].iPos = mathRandom(0, m_Points.GetCount() - 1);
			m_Particles[i].vScale = CVector(
					PolarToVector(mathRandom(0, 179), (Metric)iAdjWidth).GetX(),
					m_iLength * mathRandom(80, 150) / 100.0
					);
			}

		m_bInitialized = true;
		}

	return true;
	}

void CParticleCometEffectPainter::GetParam (const CString &sParam, CEffectParamDesc *retValue)

//	GetParam
//
//	Returns the given parameter

	{
	if (strEquals(sParam, LENGTH_ATTRIB))
		retValue->InitInteger(m_iLength);

	else if (strEquals(sParam, LIFETIME_ATTRIB))
		retValue->InitInteger(m_iLifetime);

	else if (strEquals(sParam, PARTICLE_COUNT_ATTRIB))
		retValue->InitInteger(m_iParticleCount);

	else if (strEquals(sParam, PRIMARY_COLOR_ATTRIB))
		retValue->InitColor(m_rgbPrimaryColor);

	else if (strEquals(sParam, SECONDARY_COLOR_ATTRIB))
		retValue->InitColor(m_rgbSecondaryColor);

	else if (strEquals(sParam, WIDTH_ATTRIB))
		retValue->InitInteger(m_iWidth);

	else
		retValue->InitNull();
	}

bool CParticleCometEffectPainter::GetParamList (TArray<CString> *retList) const

//	GetParamList
//
//	Returns the list of parameters

	{
	retList->DeleteAll();
	retList->InsertEmpty(6);
	retList->GetAt(0) = LENGTH_ATTRIB;
	retList->GetAt(1) = LIFETIME_ATTRIB;
	retList->GetAt(2) = PARTICLE_COUNT_ATTRIB;
	retList->GetAt(3) = PRIMARY_COLOR_ATTRIB;
	retList->GetAt(4) = SECONDARY_COLOR_ATTRIB;
	retList->GetAt(5) = WIDTH_ATTRIB;

	return true;
	}

CVector CParticleCometEffectPainter::GetParticlePos (int iParticle, int iTick, int iDirection, int *retiAge, int *retiLength)

//	GetParticlePos
//
//	Returns the position of the given particle

	{
	int iAge = (iTick + m_Particles[iParticle].iPos) % m_Points.GetCount();

	CVector vPos = m_Points[iAge];
	vPos = CVector(
			vPos.GetX() * m_Particles[iParticle].vScale.GetY(),
			vPos.GetY() * m_Particles[iParticle].vScale.GetX()
			);

	if (retiAge)
		*retiAge = iAge;

	if (retiLength)
		*retiLength = (int)vPos.GetX();

	return vPos.Rotate(iDirection + 180);
	}

void CParticleCometEffectPainter::GetRect (RECT *retRect) const

//	GetRect
//
//	Returns the current rect

	{
	int iSize = m_iLength;

	retRect->left = -iSize;
	retRect->right = iSize;
	retRect->top = -iSize;
	retRect->bottom = iSize;
	}

void CParticleCometEffectPainter::InitSplinePoints (void)

//	InitSplinePoints
//
//	Initializes m_Points
//
//	Spline Equation:
//
//	x(t) = N1x t^3 + 3 C1x t^2 (1-t) + 3 C2x t (1-t)^2 + N2x (1 - t)^3
//	y(t) = N1y t^3 + 3 C1y t^2 (1-t) + 3 C2y t (1-t)^2 + N2y (1 - t)^3
//
//	control point xy are absolute (not relative to node)
//	at t=0, we are at N2

	{
	int i;

	if (m_Points.GetCount() > 0)
		return;

	m_Points.InsertEmpty(POINT_COUNT);

	Metric rHeadWidth = 0.5;
	Metric rTailLength = 0.5;
	Metric rDeltaT = 1.0 / ((Metric)POINT_COUNT);

	//	Create the vectors

	CVector vNode[2];
	CVector vControl[2];

	vNode[0] = CVector(0.0, 0.0);
	vControl[0] = CVector(0.0, rHeadWidth);
	vNode[1] = CVector(1.0, 0.0);
	vControl[1] = CVector(1.0 - rTailLength, 0.0);

	//	Initialize array

	Metric rT = 1.0;
	for (i = 0; i < m_Points.GetCount(); i++)
		{
		Metric rT2 = (rT * rT);
		Metric rT3 = (rT2 * rT);
		Metric rOneMinusT = (1.0 - rT);
		Metric rOneMinusT2 = (rOneMinusT * rOneMinusT);
		Metric rOneMinusT3 = (rOneMinusT2 * rOneMinusT);
		Metric r3TOneMinusT2 = (3.0 * rT * rOneMinusT2);

		m_Points[i] = (vNode[0] * rT3)
				+ (vControl[0] * 3.0 * rT2 * rOneMinusT)
				+ (vControl[1] * 3.0 * rT * rOneMinusT2)
				+ (vNode[1] * rOneMinusT3);

		//	Next

		rT -= rDeltaT;
		}
	}

void CParticleCometEffectPainter::Paint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	Paint
//
//	Paint the effect

	{
	int i;

	if (!CalcIntermediates())
		return;

	int iParticleSize = 2;
	int iMaxAge = GetMaxAge();

	//	If we fade the color then we need a different loop

	if (!m_rgbSecondaryColor.IsNull() && m_rgbPrimaryColor != m_rgbSecondaryColor)
		{
		for (i = 0; i < m_Particles.GetCount(); i++)
			{
			int iAge;
			int iLengthPos;
			CVector vPos = GetParticlePos(i, Ctx.iTick, Ctx.iRotation, &iAge, &iLengthPos);
			if (Ctx.iMaxLength != -1 && iLengthPos > Ctx.iMaxLength)
				continue;

			DWORD dwOpacity = 255 - (iAge * 255 / iMaxAge);
			CG32bitPixel rgbColor = CG32bitPixel::Fade(m_rgbPrimaryColor, m_rgbSecondaryColor, 100 * iAge / iMaxAge);

			DrawParticle(Dest,
					x + (int)vPos.GetX(),
					y - (int)vPos.GetY(),
					rgbColor,
					iParticleSize,
					dwOpacity);
			}
		}
	
	//	Otherwise paint all in one color

	else
		{
		for (i = 0; i < m_Particles.GetCount(); i++)
			{
			int iAge;
			int iLengthPos;
			CVector vPos = GetParticlePos(i, Ctx.iTick, Ctx.iRotation, &iAge, &iLengthPos);
			if (Ctx.iMaxLength != -1 && iLengthPos > Ctx.iMaxLength)
				continue;

			DWORD dwOpacity = 255 - (iAge * 255 / iMaxAge);

			DrawParticle(Dest,
					x + (int)vPos.GetX(),
					y - (int)vPos.GetY(),
					m_rgbPrimaryColor,
					iParticleSize,
					dwOpacity);
			}
		}
	}

void CParticleCometEffectPainter::PaintComposite (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	PaintComposite
//
//	Composite the effect

	{
	if (!CalcIntermediates())
		return;
	}

bool CParticleCometEffectPainter::PointInImage (int x, int y, int iTick, int iVariant, int iRotation) const

//	PointInImage
//
//	Returns TRUE if the point is in the effect

	{
	//	We only intersect if we are inside a box around the center of 1/2 the
	//	width (since a lot of the width is taken up by glow effects).

	int iSize = (m_iWidth / 4);
	return (Absolute(x) <= iSize && Absolute(y) <= iSize);
	}

void CParticleCometEffectPainter::OnSetParam (CCreatePainterCtx &Ctx, const CString &sParam, const CEffectParamDesc &Value)

//	SetParam
//
//	Sets the given parameter

	{
	if (strEquals(sParam, LENGTH_ATTRIB))
		m_iLength = Value.EvalIntegerBounded(Ctx, 1, -1, (int)(STD_SECONDS_PER_UPDATE * LIGHT_SECOND / KLICKS_PER_PIXEL));

	else if (strEquals(sParam, LIFETIME_ATTRIB))
		m_iLifetime = Value.EvalIntegerBounded(Ctx, 0, -1, 0);

	else if (strEquals(sParam, PARTICLE_COUNT_ATTRIB))
		m_iParticleCount = Value.EvalIntegerBounded(Ctx, 0, -1, 100);

	else if (strEquals(sParam, PRIMARY_COLOR_ATTRIB))
		m_rgbPrimaryColor = Value.EvalColor(Ctx);

	else if (strEquals(sParam, SECONDARY_COLOR_ATTRIB))
		m_rgbSecondaryColor = Value.EvalColor(Ctx);
	
	else if (strEquals(sParam, WIDTH_ATTRIB))
		m_iWidth = Value.EvalIntegerBounded(Ctx, 1, -1, 8);
	}
