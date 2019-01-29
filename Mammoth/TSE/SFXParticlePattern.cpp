//	SFXParticlePattern.cpp
//
//	Particle Comet SFX
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define PARTICLE_EFFECT_TAG						CONSTLIT("ParticleEffect")

#define JITTER_LENGTH_ATTRIB					CONSTLIT("jitterLength")
#define LENGTH_ATTRIB							CONSTLIT("length")
#define LIFETIME_ATTRIB							CONSTLIT("lifetime")
#define PARTICLE_COUNT_ATTRIB					CONSTLIT("particleCount")
#define PARTICLE_SPEED_ATTRIB					CONSTLIT("particleSpeed")
#define PRIMARY_COLOR_ATTRIB					CONSTLIT("primaryColor")
#define SECONDARY_COLOR_ATTRIB					CONSTLIT("secondaryColor")
#define SPREAD_ANGLE_ATTRIB						CONSTLIT("spreadAngle")
#define STYLE_ATTRIB							CONSTLIT("style")
#define WIDTH_ATTRIB							CONSTLIT("width")

const int POINT_COUNT =							100;
const int FADE_LIFETIME =						10;
const int DEFAULT_LINE_LENGTH =					25;

static LPSTR STYLE_TABLE[] =
	{
	//	Must be same order as EStyles
		"",

		"comet",
		"jet",

		NULL,
	};

class CParticlePatternEffectPainter : public IEffectPainter
	{
	public:
		CParticlePatternEffectPainter (CCreatePainterCtx &Ctx, CParticlePatternEffectCreator *pCreator);
		~CParticlePatternEffectPainter (void);

		//	IEffectPainter virtuals

		virtual bool CanPaintComposite (void) { return true; }
		virtual CEffectCreator *GetCreator (void) { return m_pCreator; }
		virtual int GetFadeLifetime (bool bHit) const override { return (bHit ? 0 : FADE_LIFETIME); }
		virtual int GetLifetime (void) { return m_iLifetime; }
		virtual void GetParam (const CString &sParam, CEffectParamDesc *retValue);
		virtual bool GetParamList (TArray<CString> *retList) const;
		virtual void GetRect (RECT *retRect) const;
		virtual void OnMove (SEffectMoveCtx &Ctx, bool *retbBoundsChanged = NULL) override;
		virtual void OnUpdate (SEffectUpdateCtx &Ctx) override;
		virtual void Paint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);
		virtual void PaintComposite (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);
		virtual void PaintFade (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override { bool bOldFade = Ctx.bFade; Ctx.bFade = true; Paint(Dest, x, y, Ctx); Ctx.bFade = bOldFade; }
		virtual bool PointInImage (int x, int y, int iTick, int iVariant = 0, int iRotation = 0) const;

		static const TArray<CVector> &GetSplinePoints (void) { InitSplinePoints(); return m_Points; }

	protected:
		virtual void OnSetParam (CCreatePainterCtx &Ctx, const CString &sParam, const CEffectParamDesc &Value);

	private:
		enum EStyles
			{
			styleUnknown =					0,
			
			styleComet =					1,
			styleJet =						2,

			styleMax =						2,
			};

		struct SParticle
			{
			int iPos;
			CVector vScale;
			int iRotationAdj;
			};

		bool CalcIntermediates (void);
		static void InitSplinePoints (void);
		inline int GetMaxAge (void) const { return m_Points.GetCount() - 1; }
		bool GetPaintInfo (int iParticle, int xPos, int yPos, SViewportPaintCtx &Ctx, int *retxPos, int *retyPos, int *retiAge);
		void Paint (CG32bitImage &Dest, int xPos, int yPos, SViewportPaintCtx &Ctx, SParticlePaintDesc &Desc);
		void Paint (CG32bitImage &Dest, int xPos, int yPos, SViewportPaintCtx &Ctx, IEffectPainter *pPainter);
		void PaintFireAndSmoke (CG32bitImage &Dest, int xPos, int yPos, SViewportPaintCtx &Ctx, const CFireAndSmokePainter &Painter);
		void PaintGaseous (CG32bitImage &Dest, int xPos, int yPos, SViewportPaintCtx &Ctx, SParticlePaintDesc &Desc);
		void PaintGlitter (CG32bitImage &Dest, int xPos, int yPos, SViewportPaintCtx &Ctx, SParticlePaintDesc &Desc);
		void PaintImage (CG32bitImage &Dest, int xPos, int yPos, SViewportPaintCtx &Ctx, SParticlePaintDesc &Desc);
		void PaintLine (CG32bitImage &Dest, int xPos, int yPos, SViewportPaintCtx &Ctx, SParticlePaintDesc &Desc);

		CEffectCreator *m_pCreator;

		EStyles m_iStyle;
		int m_iLifetime;
		int m_iParticleCount;
		int m_iWidth;
		int m_iLength;						//	Length of each particle path
		int m_iParticleSpeed;				//	Desired speed of particles
		DiceRange m_SpreadAngle;			//	Angle at which particles spread
		DiceRange m_JitterLength;			//	Jitter length by this percent range
		CG32bitPixel m_rgbPrimaryColor;
		CG32bitPixel m_rgbSecondaryColor;
		IEffectPainter *m_pParticlePainter;

		//	Temporary variables based on shape/style/etc.

		int m_iInitializedLength;			//	Temporaries computed for this length (-1 if uninitialized)
		TArray<SParticle> m_Particles;
		Metric m_rTickAdj;					//	Speed adjustment

		static TArray<CVector> m_Points;
	};

//	CParticlePatternEffectCreator object

CParticlePatternEffectCreator::CParticlePatternEffectCreator (void) : 
		m_pParticleEffect(NULL),
		m_pSingleton(NULL)

//	CParticlePatternEffectCreator constructor

	{
	}

CParticlePatternEffectCreator::~CParticlePatternEffectCreator (void)

//	CParticlePatternEffectCreator destructor

	{
	if (m_pSingleton)
		delete m_pSingleton;

	if (m_pParticleEffect)
		delete m_pParticleEffect;
	}

const TArray<CVector> &CParticlePatternEffectCreator::GetSplinePoints (void)

//	GetSplinePoints
//
//	Returns the points

	{
	return CParticlePatternEffectPainter::GetSplinePoints();
	}

IEffectPainter *CParticlePatternEffectCreator::OnCreatePainter (CCreatePainterCtx &Ctx)

//	CreatePainter
//
//	Creates a new painter
	
	{
	//	If we have a singleton, return that

	if (m_pSingleton)
		return m_pSingleton;

	//	Otherwise we need to create a painter with the actual
	//	parameters.

	IEffectPainter *pPainter = new CParticlePatternEffectPainter(Ctx, this);

	//	Initialize the painter parameters

	pPainter->SetParam(Ctx, JITTER_LENGTH_ATTRIB, m_JitterLength);
	pPainter->SetParam(Ctx, LENGTH_ATTRIB, m_Length);
	pPainter->SetParam(Ctx, LIFETIME_ATTRIB, m_Lifetime);
	pPainter->SetParam(Ctx, PARTICLE_COUNT_ATTRIB, m_ParticleCount);
	pPainter->SetParam(Ctx, PARTICLE_SPEED_ATTRIB, m_ParticleSpeed);
	pPainter->SetParam(Ctx, PRIMARY_COLOR_ATTRIB, m_PrimaryColor);
	pPainter->SetParam(Ctx, SECONDARY_COLOR_ATTRIB, m_SecondaryColor);
	pPainter->SetParam(Ctx, SPREAD_ANGLE_ATTRIB, m_SpreadAngle);
	pPainter->SetParam(Ctx, STYLE_ATTRIB, m_Style);
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

ALERROR CParticlePatternEffectCreator::OnEffectBindDesign (SDesignLoadCtx &Ctx)

//	OnEffectBindDesign
//
//	Resolve loading

	{
	ALERROR error;

	//	Clean up, because we might want to recompute for next time.

	if (m_pSingleton)
		{
		delete m_pSingleton;
		m_pSingleton = NULL;
		}

	if (m_pParticleEffect)
		{
		if (error = m_pParticleEffect->BindDesign(Ctx))
			return error;
		}

	return NOERROR;
	}

ALERROR CParticlePatternEffectCreator::OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID)

//	OnEffectCreateFromXML
//
//	Initializes from XML

	{
	ALERROR error;

	if (error = m_JitterLength.InitIntegerFromXML(Ctx, pDesc->GetAttribute(JITTER_LENGTH_ATTRIB)))
		return error;

	if (error = m_Length.InitIntegerFromXML(Ctx, pDesc->GetAttribute(LENGTH_ATTRIB)))
		return error;

	if (error = m_Lifetime.InitIntegerFromXML(Ctx, pDesc->GetAttribute(LIFETIME_ATTRIB)))
		return error;

	if (error = m_ParticleCount.InitIntegerFromXML(Ctx, pDesc->GetAttribute(PARTICLE_COUNT_ATTRIB)))
		return error;

	if (error = m_ParticleSpeed.InitIntegerFromXML(Ctx, pDesc->GetAttribute(PARTICLE_SPEED_ATTRIB)))
		return error;

	if (error = m_PrimaryColor.InitColorFromXML(Ctx, pDesc->GetAttribute(PRIMARY_COLOR_ATTRIB)))
		return error;

	if (error = m_SecondaryColor.InitColorFromXML(Ctx, pDesc->GetAttribute(SECONDARY_COLOR_ATTRIB)))
		return error;

	if (error = m_SpreadAngle.InitIntegerFromXML(Ctx, pDesc->GetAttribute(SPREAD_ANGLE_ATTRIB)))
		return error;

	if (error = m_Style.InitIdentifierFromXML(Ctx, pDesc->GetAttribute(STYLE_ATTRIB), STYLE_TABLE))
		return error;

	if (error = m_Width.InitIntegerFromXML(Ctx, pDesc->GetAttribute(WIDTH_ATTRIB)))
		return error;

	//	Load the effect to use for particles

	CXMLElement *pEffect = pDesc->GetContentElementByTag(PARTICLE_EFFECT_TAG);
	if (pEffect)
		{
		if (error = CEffectCreator::CreateFromXML(Ctx, pEffect, NULL_STR, &m_pParticleEffect))
			return error;
		}
	else
		m_pParticleEffect = NULL;

	return NOERROR;
	}

//	CParticlePatternEffectPainter ------------------------------------------------

TArray<CVector> CParticlePatternEffectPainter::m_Points;

CParticlePatternEffectPainter::CParticlePatternEffectPainter (CCreatePainterCtx &Ctx, CParticlePatternEffectCreator *pCreator) :
		m_pCreator(pCreator),
		m_iStyle(styleComet),
		m_iLifetime(0),
		m_iParticleCount(100),
		m_iParticleSpeed(-1),
		m_iWidth(8),
		m_iLength(100),
		m_rgbPrimaryColor(CG32bitPixel(255, 255, 255)),
		m_rgbSecondaryColor(CG32bitPixel(128, 128, 128)),
		m_JitterLength(1, 71, 79),
		m_iInitializedLength(-1)

//	CParticlePatternEffectPainter constructor

	{
	InitSplinePoints();

	//	Initialize the single particle painter

	CEffectCreator *pEffect = pCreator->GetParticleEffect();
	if (pEffect)
		m_pParticlePainter = pEffect->CreatePainter(Ctx);
	else
		m_pParticlePainter = NULL;
	}

CParticlePatternEffectPainter::~CParticlePatternEffectPainter (void)

//	CParticlePatternEffectPainter destructor

	{
	if (m_pParticlePainter)
		m_pParticlePainter->Delete();
	}

bool CParticlePatternEffectPainter::CalcIntermediates (void)

//	CalcIntermediates
//
//	Calculate intermetiate variables

	{
	int i;

	if (m_iParticleCount <= 0)
		return false;

	if (m_iInitializedLength != m_iLength)
		{
		//	Compute how fast each particle moves along the point array.

		if (m_iParticleSpeed == -1 || m_iLength <= 0)
			m_rTickAdj = 1.0;
		else
			{
			Metric rLengthPerPoint = (m_iLength * g_KlicksPerPixel) / (Metric)POINT_COUNT;
			Metric rSpeedPerTick = (LIGHT_SPEED * m_iParticleSpeed / 100.0) * g_SecondsPerUpdate;
			Metric rPointsPerTick = rSpeedPerTick / rLengthPerPoint;
			m_rTickAdj = rPointsPerTick;
			}

		//	Compute some spread angle values

		int iSpreadAngle = m_SpreadAngle.GetMaxValue();
		int iHalfAngle = (iSpreadAngle + 1) / 2;

		//	We need to increase the width a bit because the template (the spline
		//	points) are not full width.

		int iAdjWidth = 18 * m_iWidth / 8;

		//	Create the particle array

		m_Particles.DeleteAll();
		m_Particles.InsertEmpty(m_iParticleCount);

		for (i = 0; i < m_iParticleCount; i++)
			{
			m_Particles[i].iPos = mathRandom(0, m_Points.GetCount() - 1);

			//	Compute length

			Metric rLength = m_iLength;
			if (!m_JitterLength.IsConstant())
				rLength = rLength * m_JitterLength.Roll() / 100.0;

			//	Scale

			switch (m_iStyle)
				{
				case styleJet:
					m_Particles[i].vScale = CVector(
							mathRandomGaussian() * m_iWidth,
							rLength
							);
					break;

				default:
					m_Particles[i].vScale = CVector(
							PolarToVector(mathRandom(0, 179), (Metric)iAdjWidth).GetX(),
							rLength
							);
					break;
				}

			if (iSpreadAngle == 0)
				m_Particles[i].iRotationAdj = 0;
			else if (m_SpreadAngle.IsConstant())
				m_Particles[i].iRotationAdj = mathRandom(0, iSpreadAngle) - iHalfAngle;
			else
				m_Particles[i].iRotationAdj = m_SpreadAngle.Roll() - iHalfAngle;
			}

		m_iInitializedLength = m_iLength;
		}

	return true;
	}

void CParticlePatternEffectPainter::GetParam (const CString &sParam, CEffectParamDesc *retValue)

//	GetParam
//
//	Returns the given parameter

	{
	if (strEquals(sParam, JITTER_LENGTH_ATTRIB))
		retValue->InitDiceRange(m_JitterLength);

	else if (strEquals(sParam, LENGTH_ATTRIB))
		retValue->InitInteger(m_iLength);

	else if (strEquals(sParam, LIFETIME_ATTRIB))
		retValue->InitInteger(m_iLifetime);

	else if (strEquals(sParam, PARTICLE_COUNT_ATTRIB))
		retValue->InitInteger(m_iParticleCount);

	else if (strEquals(sParam, PARTICLE_SPEED_ATTRIB))
		retValue->InitInteger(m_iParticleSpeed);

	else if (strEquals(sParam, PRIMARY_COLOR_ATTRIB))
		retValue->InitColor(m_rgbPrimaryColor);

	else if (strEquals(sParam, SECONDARY_COLOR_ATTRIB))
		retValue->InitColor(m_rgbSecondaryColor);

	else if (strEquals(sParam, SPREAD_ANGLE_ATTRIB))
		retValue->InitDiceRange(m_SpreadAngle);

	else if (strEquals(sParam, STYLE_ATTRIB))
		retValue->InitInteger(m_iStyle);

	else if (strEquals(sParam, WIDTH_ATTRIB))
		retValue->InitInteger(m_iWidth);

	else
		retValue->InitNull();
	}

bool CParticlePatternEffectPainter::GetParamList (TArray<CString> *retList) const

//	GetParamList
//
//	Returns the list of parameters

	{
	retList->DeleteAll();
	retList->InsertEmpty(10);
	retList->GetAt(0) = JITTER_LENGTH_ATTRIB;
	retList->GetAt(1) = LENGTH_ATTRIB;
	retList->GetAt(2) = LIFETIME_ATTRIB;
	retList->GetAt(3) = PARTICLE_COUNT_ATTRIB;
	retList->GetAt(4) = PARTICLE_SPEED_ATTRIB;
	retList->GetAt(5) = PRIMARY_COLOR_ATTRIB;
	retList->GetAt(6) = SECONDARY_COLOR_ATTRIB;
	retList->GetAt(7) = SPREAD_ANGLE_ATTRIB;
	retList->GetAt(8) = STYLE_ATTRIB;
	retList->GetAt(9) = WIDTH_ATTRIB;

	return true;
	}

bool CParticlePatternEffectPainter::GetPaintInfo (int iParticle, int xPos, int yPos, SViewportPaintCtx &Ctx, int *retxPos, int *retyPos, int *retiAge)

//	GetPaintInfo
//
//	Returns paint information. If we return FALSE, then this particle should not
//	be painted.

	{
	SParticle *pParticle = &m_Particles[iParticle];
	int iAge = ((int)(Ctx.iTick * m_rTickAdj) + pParticle->iPos) % m_Points.GetCount();

	CVector vPos = m_Points[iAge];
	vPos = CVector(
			vPos.GetX() * pParticle->vScale.GetY(),
			vPos.GetY() * pParticle->vScale.GetX()
			);

	vPos = vPos.Rotate(Ctx.iRotation + 180 + pParticle->iRotationAdj);

	//	If we're fading, then some particles wink out

	if (Ctx.bFade && (iParticle % FADE_LIFETIME) < (Ctx.iTick - Ctx.iStartFade))
		return false;

	//	If this position is beyond our length, then we skip it.

	int iLength = (int)vPos.GetX();
	if (Ctx.iMaxLength != -1 && iLength > Ctx.iMaxLength)
		return false;

	*retxPos = xPos + (int)vPos.GetX();
	*retyPos = yPos - (int)vPos.GetY();
	*retiAge = iAge;

	return true;
	}

void CParticlePatternEffectPainter::GetRect (RECT *retRect) const

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

void CParticlePatternEffectPainter::InitSplinePoints (void)

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

void CParticlePatternEffectPainter::OnMove (SEffectMoveCtx &Ctx, bool *retbBoundsChanged)

//	OnMove
//
//	Move the particles

	{
	//	Update the single-particle painter

	if (m_pParticlePainter)
		m_pParticlePainter->OnMove(Ctx);

	if (retbBoundsChanged) 
		*retbBoundsChanged = false; 
	}

void CParticlePatternEffectPainter::OnUpdate (SEffectUpdateCtx &Ctx)

//	OnUpdate
//
//	Update

	{
	//	Update the single-particle painter

	if (m_pParticlePainter)
		m_pParticlePainter->OnUpdate(Ctx);
	}

void CParticlePatternEffectPainter::Paint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	Paint
//
//	Paint the effect

	{
	int i;

	if (!CalcIntermediates())
		return;

	//	If we've got a painter, then use that

	if (m_pParticlePainter)
		{
		//	See if the painter has a paint descriptor (which is faster for us).

		SParticlePaintDesc PaintDesc;
		if (m_pParticlePainter->GetParticlePaintDesc(&PaintDesc))
			{
			PaintDesc.iMaxLifetime = GetMaxAge();
			Paint(Dest, x, y, Ctx, PaintDesc);
			}

		//	Otherwise, we use the painter for each particle

		else
			Paint(Dest, x, y, Ctx, m_pParticlePainter);
		}

	//	If we fade the color then we need a different loop

	else if (!m_rgbSecondaryColor.IsNull() && m_rgbPrimaryColor != m_rgbSecondaryColor)
		{
		int iParticleSize = 2;
		int iMaxAge = GetMaxAge();

		for (i = 0; i < m_Particles.GetCount(); i++)
			{
			int iAge;
			int xParticle;
			int yParticle;
			if (!GetPaintInfo(i, x, y, Ctx, &xParticle, &yParticle, &iAge))
				continue;

			DWORD dwOpacity = 255 - (iAge * 255 / iMaxAge);
			CG32bitPixel rgbColor = CG32bitPixel::Fade(m_rgbPrimaryColor, m_rgbSecondaryColor, 100 * iAge / iMaxAge);

			DrawParticle(Dest,
					xParticle,
					yParticle,
					rgbColor,
					iParticleSize,
					dwOpacity);
			}
		}
	
	//	Otherwise paint all in one color

	else
		{
		int iParticleSize = 2;
		int iMaxAge = GetMaxAge();

		for (i = 0; i < m_Particles.GetCount(); i++)
			{
			int iAge;
			int xParticle;
			int yParticle;
			if (!GetPaintInfo(i, x, y, Ctx, &xParticle, &yParticle, &iAge))
				continue;

			DWORD dwOpacity = 255 - (iAge * 255 / iMaxAge);

			DrawParticle(Dest,
					xParticle,
					yParticle,
					m_rgbPrimaryColor,
					iParticleSize,
					dwOpacity);
			}
		}
	}

void CParticlePatternEffectPainter::Paint (CG32bitImage &Dest, int xPos, int yPos, SViewportPaintCtx &Ctx, SParticlePaintDesc &Desc)

//	Paint
//
//	Paint each particle with a paint effect.

	{
	//	Paint based on style

	switch (Desc.iStyle)
		{
		case paintFlame:
			{
			CFireAndSmokePainter FireAndSmoke(CFireAndSmokePainter::styleFlame, Desc.iMaxLifetime, Desc.iMinWidth, Desc.iMaxWidth);
			PaintFireAndSmoke(Dest, xPos, yPos, Ctx, FireAndSmoke);
			break;
			}

		case paintGlitter:
			PaintGlitter(Dest, xPos, yPos, Ctx, Desc);
			break;

		case paintImage:
			PaintImage(Dest, xPos, yPos, Ctx, Desc);
			break;

		case paintLine:
			PaintLine(Dest, xPos, yPos, Ctx, Desc);
			break;

		case paintSmoke:
			{
			CFireAndSmokePainter FireAndSmoke(CFireAndSmokePainter::styleSmoke, Desc.iMaxLifetime, Desc.iMinWidth, Desc.iMaxWidth);
			PaintFireAndSmoke(Dest, xPos, yPos, Ctx, FireAndSmoke);
			break;
			}

		default:
			PaintGaseous(Dest, xPos, yPos, Ctx, Desc);
			break;
		}
	}

void CParticlePatternEffectPainter::Paint (CG32bitImage &Dest, int xPos, int yPos, SViewportPaintCtx &Ctx, IEffectPainter *pPainter)

//	Paint
//
//	Paint each particle with an effect

	{
	int i;
	int iSavedTick = Ctx.iTick;
	int iSavedDestiny = Ctx.iDestiny;
	int iSavedRotation = Ctx.iRotation;
	int iSavedMaxLength = Ctx.iMaxLength;

	int iMaxAge = GetMaxAge();

	//	Loop

	for (i = 0; i < m_Particles.GetCount(); i++)
		{
		int iAge;
		int xParticle;
		int yParticle;
		if (!GetPaintInfo(i, xPos, yPos, Ctx, &xParticle, &yParticle, &iAge))
			continue;

		//	Paint the particle

		Ctx.iTick = Max(0, iAge);
		Ctx.iDestiny = i;

		pPainter->Paint(Dest, 
				xParticle, 
				yParticle, 
				Ctx);
		}

	//	Clean Up

	Ctx.iTick = iSavedTick;
	Ctx.iDestiny = iSavedDestiny;
	Ctx.iRotation = iSavedRotation;
	Ctx.iMaxLength = iSavedMaxLength;
	}

void CParticlePatternEffectPainter::PaintComposite (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	PaintComposite
//
//	Composite the effect

	{
	}

void CParticlePatternEffectPainter::PaintFireAndSmoke (CG32bitImage &Dest, int xPos, int yPos, SViewportPaintCtx &Ctx, const CFireAndSmokePainter &Painter)

//	PaintFireAndSmoke
//
//	Paints with fire and smoke

	{
	int i;

	for (i = 0; i < m_Particles.GetCount(); i++)
		{
		int iAge;
		int xParticle;
		int yParticle;
		if (!GetPaintInfo(i, xPos, yPos, Ctx, &xParticle, &yParticle, &iAge))
			continue;

		Painter.Paint(Dest,
				xParticle,
				yParticle,
				iAge,
				i
				);
		}
	}

void CParticlePatternEffectPainter::PaintGaseous (CG32bitImage &Dest, int xPos, int yPos, SViewportPaintCtx &Ctx, SParticlePaintDesc &Desc)

//	PaintGaseous
//
//	Paint gaseous particles

	{
	int i;

	CGaseousPainter Painter(GetMaxAge(), Desc.iMinWidth, Desc.iMaxWidth, Desc.rgbPrimaryColor, Desc.rgbSecondaryColor);

	for (i = 0; i < m_Particles.GetCount(); i++)
		{
		int iAge;
		int xParticle;
		int yParticle;
		if (!GetPaintInfo(i, xPos, yPos, Ctx, &xParticle, &yParticle, &iAge))
			continue;

		Painter.Paint(Dest,
				xParticle,
				yParticle,
				iAge
				);
		}
	}

void CParticlePatternEffectPainter::PaintGlitter (CG32bitImage &Dest, int xPos, int yPos, SViewportPaintCtx &Ctx, SParticlePaintDesc &Desc)

//	PaintGlitter
//
//	Paint a glittering particle

	{
	int i;

	CGlitterPainter Painter(Desc.iMaxWidth, Desc.rgbPrimaryColor, Desc.rgbSecondaryColor);

	for (i = 0; i < m_Particles.GetCount(); i++)
		{
		int iAge;
		int xParticle;
		int yParticle;
		if (!GetPaintInfo(i, xPos, yPos, Ctx, &xParticle, &yParticle, &iAge))
			continue;

		Painter.Paint(Dest,
				xParticle,
				yParticle,
				i + Ctx.iTick
				);
		}
	}

void CParticlePatternEffectPainter::PaintImage (CG32bitImage &Dest, int xPos, int yPos, SViewportPaintCtx &Ctx, SParticlePaintDesc &Desc)

//	PaintImage
//
//	Paint image particle

	{
	int i;

	int iRotationFrame = 0;
	if (Desc.bDirectional)
		iRotationFrame = Angle2Direction(Ctx.iRotation, Desc.iVariants);

	for (i = 0; i < m_Particles.GetCount(); i++)
		{
		int iAge;
		int xParticle;
		int yParticle;
		if (!GetPaintInfo(i, xPos, yPos, Ctx, &xParticle, &yParticle, &iAge))
			continue;

		//	Figure out the animation frame to paint

		int iTick;
		if (Desc.bRandomStartFrame)
			iTick = Ctx.iTick + i;
		else
			iTick = Ctx.iTick;

		//	Figure out the rotation or variant to paint

		int iFrame = (Desc.bDirectional ? Angle2Direction(AngleMod(Ctx.iRotation), Desc.iVariants) : (i % Desc.iVariants));

		//	Paint the particle

		Desc.pImage->PaintImage(Dest, 
				xParticle,
				yParticle,
				iTick,
				iFrame);
		}
	}

void CParticlePatternEffectPainter::PaintLine (CG32bitImage &Dest, int xPos, int yPos, SViewportPaintCtx &Ctx, SParticlePaintDesc &Desc)

//	PaintLine
//
//	Paint line particles

	{
	int i;

	//	We want the tail to be transparent and the head to be full.
	//	NOTE: We paint from head to tail.

	CG32bitPixel rgbFrom = Desc.rgbPrimaryColor;
	CG32bitPixel rgbTo = CG32bitPixel(Desc.rgbSecondaryColor, 0);

	for (i = 0; i < m_Particles.GetCount(); i++)
		{
		int iAge;
		int xParticle;
		int yParticle;
		if (!GetPaintInfo(i, xPos, yPos, Ctx, &xParticle, &yParticle, &iAge))
			continue;

		//	Compute the position of the particle

		int xFrom = xParticle;
		int yFrom = yParticle;

		CVector vTail = PolarToVector(Ctx.iRotation, DEFAULT_LINE_LENGTH);

		int xTo = xFrom - (int)vTail.GetX();
		int yTo = yFrom + (int)vTail.GetY();

		//	Paint the particle

		CGDraw::LineGradient(Dest, xFrom, yFrom, xTo, yTo, 1, rgbFrom, rgbTo);
		}
	}

bool CParticlePatternEffectPainter::PointInImage (int x, int y, int iTick, int iVariant, int iRotation) const

//	PointInImage
//
//	Returns TRUE if the point is in the effect

	{
	//	We only intersect if we are inside a box around the center of 1/2 the
	//	width (since a lot of the width is taken up by glow effects).

	int iSize = (m_iWidth / 4);
	return (Absolute(x) <= iSize && Absolute(y) <= iSize);
	}

void CParticlePatternEffectPainter::OnSetParam (CCreatePainterCtx &Ctx, const CString &sParam, const CEffectParamDesc &Value)

//	SetParam
//
//	Sets the given parameter

	{
	if (strEquals(sParam, JITTER_LENGTH_ATTRIB))
		{
		//	For API 37 onward we have to explicitly define this. Previously,
		//	we used a hard-coded jitter, so we default to that.

		if (Ctx.GetAPIVersion() < 37)
			m_JitterLength = Value.EvalDiceRange(1, 71, 79);
		else
			m_JitterLength = Value.EvalDiceRange(0);
		}

	else if (strEquals(sParam, LENGTH_ATTRIB))
		m_iLength = Value.EvalIntegerBounded(0, -1, (int)(STD_SECONDS_PER_UPDATE * LIGHT_SECOND / KLICKS_PER_PIXEL));

	else if (strEquals(sParam, LIFETIME_ATTRIB))
		m_iLifetime = Value.EvalIntegerBounded(0, -1, 0);

	else if (strEquals(sParam, PARTICLE_COUNT_ATTRIB))
		m_iParticleCount = Value.EvalIntegerBounded(0, -1, 100);

	else if (strEquals(sParam, PARTICLE_SPEED_ATTRIB))
		m_iParticleSpeed = Value.EvalIntegerBounded(0, 100, -1);

	else if (strEquals(sParam, PRIMARY_COLOR_ATTRIB))
		m_rgbPrimaryColor = Value.EvalColor();

	else if (strEquals(sParam, SECONDARY_COLOR_ATTRIB))
		m_rgbSecondaryColor = Value.EvalColor();
	
	else if (strEquals(sParam, SPREAD_ANGLE_ATTRIB))
		m_SpreadAngle = Value.EvalDiceRange(0);

	else if (strEquals(sParam, STYLE_ATTRIB))
		m_iStyle = (EStyles)Value.EvalIdentifier(STYLE_TABLE, styleMax, styleComet);

	else if (strEquals(sParam, WIDTH_ATTRIB))
		m_iWidth = Value.EvalIntegerBounded(1, -1, 8);
	}
