//	SFXRay.cpp
//
//	CRayEffectCreator class
//	Copyright (c) 2013 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define ANIMATE_ATTRIB			        CONSTLIT("animate")
#define ANIMATE_OPACITY_ATTRIB			CONSTLIT("animateOpacity")
#define BLEND_MODE_ATTRIB   			CONSTLIT("blendMode")
#define INTENSITY_ATTRIB				CONSTLIT("intensity")
#define LENGTH_ATTRIB					CONSTLIT("length")
#define LIFETIME_ATTRIB					CONSTLIT("lifetime")
#define PRIMARY_COLOR_ATTRIB			CONSTLIT("primaryColor")
#define SECONDARY_COLOR_ATTRIB			CONSTLIT("secondaryColor")
#define SHAPE_ATTRIB					CONSTLIT("shape")
#define STYLE_ATTRIB					CONSTLIT("style")
#define WIDTH_ATTRIB					CONSTLIT("width")
#define XFORM_ROTATION_ATTRIB			CONSTLIT("xformRotation")

#define FIELD_OPACITY                   CONSTLIT("opacity")

class CRayEffectPainter : public IEffectPainter
	{
	public:
		CRayEffectPainter (CEffectCreator *pCreator);
		~CRayEffectPainter (void);

		//	IEffectPainter virtuals
		virtual CEffectCreator *GetCreator (void) override { return m_pCreator; }
		virtual int GetLifetime (void) override { return m_iLifetime; }
		virtual void GetParam (const CString &sParam, CEffectParamDesc *retValue) override;
		virtual bool GetParamList (TArray<CString> *retList) const override;
		virtual void GetRect (RECT *retRect) const override;
		virtual void Paint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual void PaintHit (CG32bitImage &Dest, int x, int y, const CVector &vHitPos, SViewportPaintCtx &Ctx) override;
		virtual void PaintLine (CG32bitImage &Dest, const CVector &vHead, const CVector &vTail, SViewportPaintCtx &Ctx) override;
		virtual bool PointInImage (int x, int y, int iTick, int iVariant = 0, int iRotation = 0) const override;

	protected:
		virtual void OnSetParam (CCreatePainterCtx &Ctx, const CString &sParam, const CEffectParamDesc &Value) override;

	private:
		enum EAnimationTypes
			{
			animateNone =			0,

			animateFade =			1,
            animateFlicker =        2,
			animateCycle =			3,

            animateMax =            3,
			};

		enum ERayShapes
			{
			shapeUnknown =			0,

			shapeDiamond =			1,
			shapeOval =				2,
			shapeStraight =			3,
			shapeTapered =			4,
			shapeCone =				5,

			shapeMax =				5,
			};

		enum ERayStyles
			{
			styleUnknown =			0,

			styleBlob =				1,
			styleGlow =				2,
			styleJagged =			3,
			styleGrainy =			4,
			styleLightning =		5,
			styleWhiptail =			6,

			styleMax =				6,
			};

		enum EColorTypes
			{
			colorNone,
			colorGlow,
			};

		enum EOpacityTypes
			{
			opacityNone,
			opacityGlow,
			opacityGrainy,
			opacityTaperedGlow,
			};

		enum EWidthAdjTypes
			{
			widthAdjNone,
			widthAdjBlob,
			widthAdjDiamond,
			widthAdjJagged,
			widthAdjOval,
			widthAdjTapered,
			widthAdjCone,
			widthAdjWhiptail,
			};

		void CalcCone (TArray<Metric> &AdjArray);
		void CalcDiamond (TArray<Metric> &AdjArray);
		void CalcIntermediates (int iLength);
        int CalcLength (SViewportPaintCtx &Ctx) const;
		void CalcOval (TArray<Metric> &AdjArray);
		void CalcRandomWaves (TArray<Metric> &AdjArray, Metric rAmplitude, Metric rWavelength);
		void CalcTaper (TArray<Metric> &AdjArray);
		void CalcWaves (TArray<Metric> &AdjArray, Metric rAmplitude, Metric rWavelength, Metric rDecay, Metric rCyclePos);
		void CleanUpIntermediates (void);
        ILinePainter *CreateRenderer (int iWidth, int iLength, int iIntensity, ERayStyles iStyle, ERayShapes iShape, Metric rCyclePos = 0.0);
		void PaintRay (CG32bitImage &Dest, int xFrom, int yFrom, int xTo, int yTo, SViewportPaintCtx &Ctx);

		CEffectCreator *m_pCreator;

		int m_iLength;
		int m_iWidth;
		ERayShapes m_iShape;
		ERayStyles m_iStyle;
		int m_iIntensity;
		CG32bitPixel m_rgbPrimaryColor;
		CG32bitPixel m_rgbSecondaryColor;
		CGDraw::EBlendModes m_iBlendMode;

		int m_iXformRotation;

		int m_iLifetime;
		EAnimationTypes m_iAnimation;

		//	Temporary variables based on shape/style/etc.

		int m_iInitializedLength;			//	If -1, not yet initialized; otherwise, initialized to the given length
        TArray<ILinePainter *> m_RayRenderer;
        TArray<int> m_Length;               //  Length for each frame (only for multi-frame animations)
	};

const int BLOB_PEAK_FRACTION =			1;
const int TAPER_FRACTION =				3;
const int TAPER_PEAK_FRACTION =			8;
const int JAGGED_PEAK_FRACTION =		3;
const Metric BLOB_WAVE_SIZE =			0.30;
const Metric JAGGED_AMPLITUDE =			0.45;
const Metric JAGGED_WAVELENGTH_FACTOR =	0.33;
const Metric WAVY_WAVELENGTH_FACTOR =	1.0;
const Metric WHIPTAIL_AMPLITUDE =		0.45;
const Metric WHIPTAIL_WAVELENGTH_FACTOR =		1.0;
const Metric WHIPTAIL_DECAY =			0.13;
const int MAX_TAPERED_FADE_TAIL =		300;

const Metric MIN_GLOW_LEVEL =			0.6;
const Metric GLOW_FACTOR =				(0.4 / 100.0);
const Metric BRIGHT_FACTOR =			(0.25 / 100.0);
const Metric SOLID_FACTOR =				(0.40 / 100.0);
const Metric GRAINY_SIGMA =				1.0;

static LPSTR ANIMATION_TABLE[] =
	{
	//	Must be same order as EAnimationTypes
		"",

		"fade",
		"flicker",
		"cycle",

		NULL
	};

static LPSTR SHAPE_TABLE[] =
	{
	//	Must be same order as ERayShapes
		"",

		"diamond",
		"oval",
		"straight",
		"tapered",
		"cone",

		NULL
	};

static LPSTR STYLE_TABLE[] =
	{
	//	Must be same order as ERayStyles
		"",

		"wavy",
		"smooth",
		"jagged",
		"grainy",
		"lightning",
		"whiptail",

		NULL,
	};

CRayEffectCreator::CRayEffectCreator (void) :
			m_pSingleton(NULL)

//	CRayEffectCreator constructor

	{
	}

CRayEffectCreator::~CRayEffectCreator (void)

//	CRayEffectCreator destructor

	{
	if (m_pSingleton)
		delete m_pSingleton;
	}

IEffectPainter *CRayEffectCreator::OnCreatePainter (CCreatePainterCtx &Ctx)

//	CreatePainter
//
//	Creates a new painter
	
	{
	//	If we have a singleton, return that

	if (m_pSingleton)
		return m_pSingleton;

	//	Otherwise we need to create a painter with the actual
	//	parameters.

	IEffectPainter *pPainter = new CRayEffectPainter(this);

	//	Initialize the painter parameters

	pPainter->SetParam(Ctx, ANIMATE_ATTRIB, m_AnimateOpacity);
	pPainter->SetParam(Ctx, BLEND_MODE_ATTRIB, m_BlendMode);
	pPainter->SetParam(Ctx, LENGTH_ATTRIB, m_Length);
	pPainter->SetParam(Ctx, WIDTH_ATTRIB, m_Width);
	pPainter->SetParam(Ctx, SHAPE_ATTRIB, m_Shape);
	pPainter->SetParam(Ctx, STYLE_ATTRIB, m_Style);
	pPainter->SetParam(Ctx, INTENSITY_ATTRIB, m_Intensity);
	pPainter->SetParam(Ctx, PRIMARY_COLOR_ATTRIB, m_PrimaryColor);
	pPainter->SetParam(Ctx, SECONDARY_COLOR_ATTRIB, m_SecondaryColor);
	pPainter->SetParam(Ctx, LIFETIME_ATTRIB, m_Lifetime);
	pPainter->SetParam(Ctx, XFORM_ROTATION_ATTRIB, m_XformRotation);

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

ALERROR CRayEffectCreator::OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID)

//	OnEffectCreateFromXML
//
//	Initialize type-specific data

	{
	ALERROR error;

    CString sAnimate;
    if (!pDesc->FindAttribute(ANIMATE_ATTRIB, &sAnimate))
        sAnimate = pDesc->GetAttribute(ANIMATE_OPACITY_ATTRIB);

	if (error = m_AnimateOpacity.InitIdentifierFromXML(Ctx, sAnimate, ANIMATION_TABLE))
		return error;

	if (error = m_BlendMode.InitBlendModeFromXML(Ctx, pDesc->GetAttribute(BLEND_MODE_ATTRIB)))
		return error;

	if (error = m_Length.InitIntegerFromXML(Ctx, pDesc->GetAttribute(LENGTH_ATTRIB)))
		return error;

	if (error = m_Width.InitIntegerFromXML(Ctx, pDesc->GetAttribute(WIDTH_ATTRIB)))
		return error;

	if (error = m_Shape.InitIdentifierFromXML(Ctx, pDesc->GetAttribute(SHAPE_ATTRIB), SHAPE_TABLE))
		return error;

	if (error = m_Style.InitIdentifierFromXML(Ctx, pDesc->GetAttribute(STYLE_ATTRIB), STYLE_TABLE))
		return error;

	if (error = m_Intensity.InitIntegerFromXML(Ctx, pDesc->GetAttribute(INTENSITY_ATTRIB)))
		return error;

	if (error = m_PrimaryColor.InitColorFromXML(Ctx, pDesc->GetAttribute(PRIMARY_COLOR_ATTRIB)))
		return error;

	if (error = m_SecondaryColor.InitColorFromXML(Ctx, pDesc->GetAttribute(SECONDARY_COLOR_ATTRIB)))
		return error;

	if (error = m_Lifetime.InitIntegerFromXML(Ctx, pDesc->GetAttribute(LIFETIME_ATTRIB)))
		return error;

	if (error = m_XformRotation.InitIntegerFromXML(Ctx, pDesc->GetAttribute(XFORM_ROTATION_ATTRIB)))
		return error;

	return NOERROR;
	}

ALERROR CRayEffectCreator::OnEffectBindDesign (SDesignLoadCtx &Ctx)

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

//	CRayEffectPainter ----------------------------------------------------------

CRayEffectPainter::CRayEffectPainter (CEffectCreator *pCreator) : 
		m_pCreator(pCreator),
		m_iLength((int)(STD_SECONDS_PER_UPDATE * LIGHT_SECOND / KLICKS_PER_PIXEL)),
		m_iWidth(10),
		m_iShape(shapeStraight),
		m_iStyle(styleGlow),
		m_iIntensity(50),
		m_rgbPrimaryColor(CG32bitPixel(255, 255, 255)),
		m_rgbSecondaryColor(CG32bitPixel(128, 128, 128)),
		m_iBlendMode(CGDraw::blendNormal),
		m_iXformRotation(0),
		m_iLifetime(0),
		m_iAnimation(animateNone),
		m_iInitializedLength(-1)

//	CRayEffectCreator constructor

	{
	}

CRayEffectPainter::~CRayEffectPainter (void)

//	CRayEffectCreator destructor

	{
	CleanUpIntermediates();
	}

void CRayEffectPainter::CalcCone (TArray<Metric> &AdjArray)

//	CalcCone
//
//	Fills in the given array with cone widths.

	{
	int i;

	int iEndPoint = AdjArray.GetCount();
	Metric rXInc = (iEndPoint > 0 ? (1.0 / (Metric)iEndPoint) : 0.0);
	Metric rX = 0.0;
	for (i = 0; i < iEndPoint; i++, rX += rXInc)
		AdjArray[i] = rX;
	}

void CRayEffectPainter::CalcDiamond (TArray<Metric> &AdjArray)

//	CalcDiamond
//
//	Fills in the given array with diamond widths.

	{
	int i;

	//	First oval

	int iMidPoint = AdjArray.GetCount() / 2;
	Metric rXInc = (iMidPoint > 0 ? (1.0 / (Metric)iMidPoint) : 0.0);
	Metric rX = 0.0;
	for (i = 0; i < iMidPoint; i++, rX += rXInc)
		AdjArray[i] = rX;

	//	Second oval

	rX = 1.0;
	for (i = iMidPoint; i < AdjArray.GetCount(); i++, rX -= rXInc)
		AdjArray[i] = rX;
	}

void CRayEffectPainter::CalcIntermediates (int iLength)

//	CalcIntermediates
//
//	Calculate intermediate values used for painting.

	{
    int i;

    //  If already initialized, we're done.

    if (m_iInitializedLength == iLength)
        return;
	else if (m_iInitializedLength != -1)
		CleanUpIntermediates();

    //  Create the renderer. Depending on our animation option, we either create
    //  one or more renderers.

    switch (m_iAnimation)
        {
		case animateCycle:
			{
            //  We generate 16 frames. For some styles, such as "wavy", we cycle
			//	through some kind of motion. For others, it's just random.

            int iFrameCount = 16;
            m_RayRenderer.InsertEmpty(iFrameCount);

            for (i = 0; i < m_RayRenderer.GetCount(); i++)
                {
				Metric rAnimation = (Metric)i / (Metric)iFrameCount;
                m_RayRenderer[i] = CreateRenderer(m_iWidth, iLength, m_iIntensity, m_iStyle, m_iShape, rAnimation);
                }
			break;
			}

        case animateFlicker:
            {
            //  We generate 16 frames of random length and intensity

            int iFrameCount = 16;
            m_RayRenderer.InsertEmpty(iFrameCount);
            m_Length.InsertEmpty(iFrameCount);

            for (i = 0; i < m_RayRenderer.GetCount(); i++)
                {
				Metric rFlicker = Max(0.5, Min(1.0 + (0.25 * mathRandomGaussian()), 2.0));
				int iFlickerLength = Max(2, (int)(rFlicker * iLength));
				int iIntensity = (int)(rFlicker * m_iIntensity);

                m_Length[i] = iFlickerLength;
                m_RayRenderer[i] = CreateRenderer(m_iWidth, iFlickerLength, iIntensity, m_iStyle, m_iShape);
                }
            break;
            }

        default:
            m_RayRenderer.InsertEmpty();
            m_RayRenderer[0] = CreateRenderer(m_iWidth, iLength, m_iIntensity, m_iStyle, m_iShape);
            break;
        }

	//	Done

	m_iInitializedLength = iLength;
	}

int CRayEffectPainter::CalcLength (SViewportPaintCtx &Ctx) const

//  CalcLength
//
//  Computes the length of the ray this frame.

    {
    int iLength;

    //  Get the length based on the animation frame

    switch (m_iAnimation)
        {
        case animateFlicker:
            iLength = m_Length[Ctx.iTick % m_Length.GetCount()];
            break;

        default:
            iLength = m_iLength;
        }

    //  Adjust for maximum

	return (Ctx.iMaxLength != -1 ? Min(Ctx.iMaxLength, iLength) : iLength);
    }

void CRayEffectPainter::CalcOval (TArray<Metric> &AdjArray)

//	CalcOval
//
//	Fills in the given array with oval widths.

	{
	int i;

	//	First oval

	int iMidPoint = AdjArray.GetCount() / 2;
	Metric rXInc = (iMidPoint > 0 ? (1.0 / (Metric)iMidPoint) : 0.0);
	Metric rX = 1.0;
	for (i = 0; i < iMidPoint; i++, rX -= rXInc)
		AdjArray[i] = sqrt(1.0 - (rX * rX));

	//	Second oval

	rX = 0.0;
	for (i = iMidPoint; i < AdjArray.GetCount(); i++, rX += rXInc)
		AdjArray[i] = sqrt(1.0 - (rX * rX));
	}

void CRayEffectPainter::CalcRandomWaves (TArray<Metric> &AdjArray, Metric rAmplitude, Metric rWavelength)

//	CalcRandomWaves
//
//	Creates a smooth rolling sine wave envelope.

	{
	int i, v;
	int iLength = AdjArray.GetCount();

	//	Peak size is related to intensity.

	int iStdPeakSize = Max(2, (int)rWavelength);

	//	Cycle around the mid-point

	Metric rPos = 1.0 - rAmplitude;

	//	Keep looping until we're done

	v = 0;
	while (v < iLength)
		{
		//	Pick a random peak size

		int iPeakSize = (int)(mathRandom(50, 250) * iStdPeakSize / 100.0);

		//	Pick the scale and increment

		Metric rScale = rAmplitude * mathRandom(20, 100) / 100.0;
		Metric rInc = 2.0 * PI / iPeakSize;

		//	Generate this cycle

		Metric rAngle = 0.0;
		for (i = 0; i < iPeakSize && v < AdjArray.GetCount(); i++)
			{
			AdjArray[v] = rPos + rScale * sin(rAngle);

			rAngle += rInc;
			v++;
			}
		}
	}

void CRayEffectPainter::CalcTaper (TArray<Metric> &AdjArray)

//	CalcTaper
//
//	Fills in the given array with tapered widths.

	{
	int i;

	//	From the head to the peak, we grow like a circle

	int iPeakPoint = AdjArray.GetCount() / TAPER_PEAK_FRACTION;
	Metric rXInc = (iPeakPoint > 0 ? (1.0 / (Metric)iPeakPoint) : 0.0);
	Metric rX = 1.0;
	for (i = 0; i < iPeakPoint; i++, rX -= rXInc)
		AdjArray[i] = sqrt(1.0 - (rX * rX));

	//	Past the peak we decay linearly

	Metric MIN_WIDTH = (1.0 / Max(1, m_iWidth));
	Metric INC_TOTAL = 1.0 - MIN_WIDTH;
	Metric rY = 1.0;
	Metric rYInc = (AdjArray.GetCount() > 0 ? (INC_TOTAL / (Metric)(AdjArray.GetCount() - iPeakPoint)) : 0.0);
	for (i = iPeakPoint; i < AdjArray.GetCount(); i++, rY -= rYInc)
		AdjArray[i] = rY;
	}

void CRayEffectPainter::CalcWaves (TArray<Metric> &AdjArray, Metric rAmplitude, Metric rWavelength, Metric rDecay, Metric rCyclePos)

//	CalcWaves
//
//	Creates a smooth rolling sine wave envelope.

	{
	int v;
	int iLength = AdjArray.GetCount();

	//	Wavelength will decay (increase with each cycle)

	Metric rStartWavelength = Max(2.0, rWavelength);

	//	Cycle around the mid-point

	Metric rPos = 1.0 - rAmplitude;

	//	Offset the wave to produce an animation

	Metric rAngleStart = 2.0 * PI * (1.0 - rCyclePos);

	//	Keep looping until we're done

	for (v = 0; v < iLength; v++)
		{
		//	Figure out the wavelength at this point. The wavelength increases
		//	according to decay.

		Metric rPointWavelength = (rStartWavelength + ((Metric)v * rDecay));
		Metric rAngle = rAngleStart + (2.0 * PI * (Metric)v / rPointWavelength);

		//	Set the value

		AdjArray[v] = rPos + rAmplitude * sin(rAngle);
		}
	}

ILinePainter *CRayEffectPainter::CreateRenderer (int iWidth, int iLength, int iIntensity, ERayStyles iStyle, ERayShapes iShape, Metric rCyclePos)

//  CreateRenderer
//
//  Creates a single renderer with the given properties.

    {
    int i, v, w;

	EColorTypes iColorTypes = colorNone;
	EOpacityTypes iOpacityTypes = opacityNone;
	EWidthAdjTypes iWidthAdjType = widthAdjNone;
	EWidthAdjTypes iReshape = widthAdjNone;
	EOpacityTypes iTexture = opacityNone;

	int iWidthCount = iWidth;
	int iLengthCount = 2 * iLength;

	//	Every combination requires a slightly different set up

	switch (iShape)
		{
		case shapeCone:
			switch (iStyle)
				{
				case styleBlob:
					iColorTypes = colorGlow;
					iOpacityTypes = opacityTaperedGlow;
					iWidthAdjType = widthAdjBlob;
					iReshape = widthAdjCone;
					break;

				case styleGlow:
					iColorTypes = colorGlow;
					iOpacityTypes = opacityTaperedGlow;
					iWidthAdjType = widthAdjCone;
					break;

				case styleGrainy:
					iColorTypes = colorGlow;
					iOpacityTypes = opacityTaperedGlow;
					iWidthAdjType = widthAdjCone;
					iTexture = opacityGrainy;
					break;

				case styleJagged:
					iColorTypes = colorGlow;
					iOpacityTypes = opacityTaperedGlow;
					iWidthAdjType = widthAdjJagged;
					iReshape = widthAdjCone;
					break;

				case styleLightning:
					iWidthAdjType = widthAdjCone;
					break;

				case styleWhiptail:
					iColorTypes = colorGlow;
					iOpacityTypes = opacityTaperedGlow;
					iWidthAdjType = widthAdjWhiptail;
					iReshape = widthAdjCone;
					break;
				}
			break;

		case shapeDiamond:
			switch (iStyle)
				{
				case styleBlob:
					iColorTypes = colorGlow;
					iOpacityTypes = opacityTaperedGlow;
					iWidthAdjType = widthAdjBlob;
					iReshape = widthAdjDiamond;
					break;

				case styleGlow:
					iColorTypes = colorGlow;
					iOpacityTypes = opacityTaperedGlow;
					iWidthAdjType = widthAdjDiamond;
					break;

				case styleGrainy:
					iColorTypes = colorGlow;
					iOpacityTypes = opacityTaperedGlow;
					iWidthAdjType = widthAdjDiamond;
					iTexture = opacityGrainy;
					break;

				case styleJagged:
					iColorTypes = colorGlow;
					iOpacityTypes = opacityTaperedGlow;
					iWidthAdjType = widthAdjJagged;
					iReshape = widthAdjDiamond;
					break;

				case styleLightning:
					iWidthAdjType = widthAdjDiamond;
					break;

				case styleWhiptail:
					iColorTypes = colorGlow;
					iOpacityTypes = opacityTaperedGlow;
					iWidthAdjType = widthAdjWhiptail;
					iReshape = widthAdjDiamond;
					break;
				}
			break;

		case shapeOval:
			switch (iStyle)
				{
				case styleBlob:
					iColorTypes = colorGlow;
					iOpacityTypes = opacityTaperedGlow;
					iWidthAdjType = widthAdjBlob;
					iReshape = widthAdjOval;
					break;

				case styleGlow:
					iColorTypes = colorGlow;
					iOpacityTypes = opacityTaperedGlow;
					iWidthAdjType = widthAdjOval;
					break;

				case styleGrainy:
					iColorTypes = colorGlow;
					iOpacityTypes = opacityTaperedGlow;
					iWidthAdjType = widthAdjOval;
					iTexture = opacityGrainy;
					break;

				case styleJagged:
					iColorTypes = colorGlow;
					iOpacityTypes = opacityTaperedGlow;
					iWidthAdjType = widthAdjJagged;
					iReshape = widthAdjOval;
					break;

				case styleLightning:
					iWidthAdjType = widthAdjOval;
					break;

				case styleWhiptail:
					iColorTypes = colorGlow;
					iOpacityTypes = opacityTaperedGlow;
					iWidthAdjType = widthAdjWhiptail;
					iReshape = widthAdjOval;
					break;
				}
			break;

		case shapeStraight:
			switch (iStyle)
				{
				case styleBlob:
					iColorTypes = colorGlow;
					iOpacityTypes = opacityGlow;
					iWidthAdjType = widthAdjBlob;
					break;

				case styleGlow:
					iColorTypes = colorGlow;
					iOpacityTypes = opacityGlow;
					break;

				case styleGrainy:
					iColorTypes = colorGlow;
					iOpacityTypes = opacityGlow;
					iTexture = opacityGrainy;
					break;

				case styleJagged:
					iColorTypes = colorGlow;
					iOpacityTypes = opacityGlow;
					iWidthAdjType = widthAdjJagged;
					break;

				case styleLightning:
					break;

				case styleWhiptail:
					iColorTypes = colorGlow;
					iOpacityTypes = opacityTaperedGlow;
					iWidthAdjType = widthAdjWhiptail;
					break;
				}
			break;

		case shapeTapered:
			switch (iStyle)
				{
				case styleBlob:
					iColorTypes = colorGlow;
					iOpacityTypes = opacityTaperedGlow;
					iWidthAdjType = widthAdjBlob;
					iReshape = widthAdjTapered;
					break;

				case styleGlow:
					iColorTypes = colorGlow;
					iOpacityTypes = opacityTaperedGlow;
					iWidthAdjType = widthAdjTapered;
					break;

				case styleGrainy:
					iColorTypes = colorGlow;
					iOpacityTypes = opacityTaperedGlow;
					iWidthAdjType = widthAdjTapered;
					iTexture = opacityGrainy;
					break;

				case styleJagged:
					iColorTypes = colorGlow;
					iOpacityTypes = opacityTaperedGlow;
					iWidthAdjType = widthAdjJagged;
					iReshape = widthAdjTapered;
					break;

				case styleLightning:
					iWidthAdjType = widthAdjTapered;
					break;

				case styleWhiptail:
					iColorTypes = colorGlow;
					iOpacityTypes = opacityTaperedGlow;
					iWidthAdjType = widthAdjWhiptail;
					iReshape = widthAdjTapered;
					break;
				}
			break;
		}

	//	Full color map

    ColorPlane ColorMap;
	switch (iColorTypes)
		{
		case colorGlow:
			{
			ColorMap.InsertEmpty(1);
			ColorMap[0].InsertEmpty(iWidthCount);

			//	The center blends towards white

			CG32bitPixel rgbCenter = CG32bitPixel::Blend(m_rgbPrimaryColor, CG32bitPixel(255, 255, 255), Min(50, iIntensity) / 50.0);

			int iBrightPoint = (int)(BRIGHT_FACTOR * iWidthCount * iIntensity);
			for (i = 0; i < iBrightPoint; i++)
				ColorMap[0][i] = rgbCenter;

			//	The rest fades (linearly) from primary color to secondary color

			Metric rFadeInc = (iWidthCount > 0 ? (1.0 / (iWidthCount - iBrightPoint)) : 0.0);
			Metric rFade = 1.0;
			for (i = iBrightPoint; i < iWidthCount; i++, rFade -= rFadeInc)
				ColorMap[0][i] = CG32bitPixel::Blend(m_rgbSecondaryColor, m_rgbPrimaryColor, rFade);

			break;
			}
		}

	//	Full opacity

    OpacityPlane OpacityMap;
	switch (iOpacityTypes)
		{
		case opacityGlow:
			{
			OpacityMap.InsertEmpty(1);
			OpacityMap[0].InsertEmpty(iWidthCount);

			//	From center to peak we have solid opacity

			int iPeakPoint = (int)(SOLID_FACTOR * iIntensity * iWidthCount);
			for (i = 0; i < iPeakPoint; i++)
				OpacityMap[0][i] = 255;

			//	We decay exponentially to edge

			Metric rGlowLevel = MIN_GLOW_LEVEL + (iIntensity * GLOW_FACTOR);
			Metric rGlowInc = (iWidthCount > 0 ? (1.0 / (iWidthCount - iPeakPoint)) : 0.0);
			Metric rGlow = 1.0;
			for (i = iPeakPoint; i < iWidthCount; i++, rGlow -= rGlowInc)
				OpacityMap[0][i] = (int)(255.0 * rGlowLevel * rGlow * rGlow);

			break;
			}

		case opacityTaperedGlow:
			{
			OpacityMap.InsertEmpty(iLengthCount);
			for (i = 0; i < iLengthCount; i++)
				OpacityMap[i].InsertEmpty(iWidthCount);

			//	From center to peak we have solid opacity

			int iPeakPoint = (int)(SOLID_FACTOR * iIntensity * iWidthCount);

			//	After the 1/3 point start fading out (linearly)

			int iMinFadePoint = iLengthCount - MAX_TAPERED_FADE_TAIL;
			int iFadePoint = Max(iMinFadePoint, iLengthCount / TAPER_FRACTION);
			Metric rTaperInc = (iLengthCount > 0 ? (1.0 / (iLengthCount - iFadePoint)) : 0.0);

			//	From center to peak we have solid opacity plus taper

			for (w = 0; w < iPeakPoint; w++)
				{
				for (v = 0; v < iFadePoint; v++)
					OpacityMap[v][w] = 255;

				Metric rTaper = 1.0;
				for (v = iFadePoint; v < iLengthCount; v++, rTaper -= rTaperInc)
					OpacityMap[v][w] = (int)(255.0 * rTaper);
				}

			//	The glow around the peak decays exponentially

			Metric rGlowLevel = MIN_GLOW_LEVEL + (iIntensity * GLOW_FACTOR);
			Metric rGlowInc = (iWidthCount > 0 ? (1.0 / (iWidthCount - iPeakPoint)) : 0.0);
			Metric rGlow = 1.0;
			for (w = iPeakPoint; w < iWidthCount; w++, rGlow -= rGlowInc)
				{
				Metric rGlowPart = rGlowLevel * rGlow * rGlow;

				for (v = 0; v < iFadePoint; v++)
					OpacityMap[v][w] = (int)(255.0 * rGlowPart);

				Metric rTaper = 1.0;
				for (v = iFadePoint; v < iLengthCount; v++, rTaper -= rTaperInc)
					OpacityMap[v][w] = (int)(255.0 * rGlowPart * rTaper);
				}

			break;
			}
		}

	//	Width adjustments

    WidthAdjArray WidthAdjTop;
    WidthAdjArray WidthAdjBottom;
	switch (iWidthAdjType)
		{
		case widthAdjBlob:
			{
			WidthAdjTop.InsertEmpty(iLengthCount);
			WidthAdjBottom.InsertEmpty(iLengthCount);

			//	Initialize jagged envelope

			CalcRandomWaves(WidthAdjTop, BLOB_WAVE_SIZE, iWidth * WAVY_WAVELENGTH_FACTOR);
			CalcRandomWaves(WidthAdjBottom, BLOB_WAVE_SIZE, iWidth * WAVY_WAVELENGTH_FACTOR);
			break;
			}

		case widthAdjCone:
			{
			WidthAdjTop.InsertEmpty(iLengthCount);

			CalcCone(WidthAdjTop);
			WidthAdjBottom = WidthAdjTop;
			break;
			}

		case widthAdjDiamond:
			{
			WidthAdjTop.InsertEmpty(iLengthCount);

			CalcDiamond(WidthAdjTop);
			WidthAdjBottom = WidthAdjTop;
			break;
			}

		case widthAdjJagged:
			{
			WidthAdjTop.InsertEmpty(iLengthCount);
			WidthAdjBottom.InsertEmpty(iLengthCount);

			//	Initialize jagged envelope

			CalcRandomWaves(WidthAdjTop, JAGGED_AMPLITUDE, iWidth * JAGGED_WAVELENGTH_FACTOR);
			CalcRandomWaves(WidthAdjBottom, JAGGED_AMPLITUDE, iWidth * JAGGED_WAVELENGTH_FACTOR);
			break;
			}

		case widthAdjOval:
			{
			WidthAdjTop.InsertEmpty(iLengthCount);

			CalcOval(WidthAdjTop);
			WidthAdjBottom = WidthAdjTop;
			break;
			}

		case widthAdjTapered:
			{
			WidthAdjTop.InsertEmpty(iLengthCount);

			CalcTaper(WidthAdjTop);
			WidthAdjBottom = WidthAdjTop;
			break;
			}

		case widthAdjWhiptail:
			{
			WidthAdjTop.InsertEmpty(iLengthCount);
			WidthAdjBottom.InsertEmpty(iLengthCount);

			CalcWaves(WidthAdjTop, WHIPTAIL_AMPLITUDE, iWidth * WHIPTAIL_WAVELENGTH_FACTOR, WHIPTAIL_DECAY, rCyclePos);
			CalcWaves(WidthAdjBottom, WHIPTAIL_AMPLITUDE, iWidth * WHIPTAIL_WAVELENGTH_FACTOR, WHIPTAIL_DECAY, rCyclePos + 0.5);
			break;
			}
		}

	//	Adjust shape

	switch (iReshape)
		{
		case widthAdjCone:
			{
			TArray<Metric> TaperAdj;
			TaperAdj.InsertEmpty(iLengthCount);
			CalcCone(TaperAdj);

			for (i = 0; i < iLengthCount; i++)
				{
				WidthAdjTop[i] *= TaperAdj[i];
				WidthAdjBottom[i] *= TaperAdj[i];
				}
			break;
			}

		case widthAdjDiamond:
			{
			TArray<Metric> TaperAdj;
			TaperAdj.InsertEmpty(iLengthCount);
			CalcDiamond(TaperAdj);

			for (i = 0; i < iLengthCount; i++)
				{
				WidthAdjTop[i] *= TaperAdj[i];
				WidthAdjBottom[i] *= TaperAdj[i];
				}
			break;
			}

		case widthAdjOval:
			{
			TArray<Metric> TaperAdj;
			TaperAdj.InsertEmpty(iLengthCount);
			CalcOval(TaperAdj);

			for (i = 0; i < iLengthCount; i++)
				{
				WidthAdjTop[i] *= TaperAdj[i];
				WidthAdjBottom[i] *= TaperAdj[i];
				}
			break;
			}

		case widthAdjTapered:
			{
			TArray<Metric> TaperAdj;
			TaperAdj.InsertEmpty(iLengthCount);
			CalcTaper(TaperAdj);

			for (i = 0; i < iLengthCount; i++)
				{
				WidthAdjTop[i] *= TaperAdj[i];
				WidthAdjBottom[i] *= TaperAdj[i];
				}
			break;
			}
		}

	//	Apply texture

	switch (iTexture)
		{
		case opacityGrainy:
			{
			const int DIAMETER = 11;
			const int START = -(DIAMETER / 2);
			const int CENTER_X = (DIAMETER / 2);
			const int CENTER_Y = (DIAMETER / 2);
			const Metric RADIUS = (DIAMETER / 2.0);

			Metric Adj[DIAMETER][DIAMETER];
			for (v = 0; v < DIAMETER; v++)
				for (w = 0; w < DIAMETER; w++)
					{
					int vDiff = v - CENTER_X;
					int wDiff = w - CENTER_Y;

					Metric rDist = sqrt((Metric)(vDiff * vDiff + wDiff * wDiff)) / RADIUS;
					Adj[v][w] = Max(0.0, 1.0 - rDist);
					}

			int iPeakPoint = (int)(SOLID_FACTOR * iIntensity * iWidthCount);

			int iGrainCount = (int)(4 * sqrt((Metric)iLength * iWidth));
			for (i = 0; i < iGrainCount; i++)
				{
				int vCount = OpacityMap.GetCount();
				int wCount = OpacityMap[0].GetCount();
				int vCenter = mathRandom(0, vCount - 1);
				int wCenter = mathRandom(0, wCount - 1);

				Metric rCenter = GRAINY_SIGMA * mathRandomGaussian();

				for (v = 0; v < DIAMETER; v++)
					{
					int vPos = START + vCenter + v;
					if (vPos < 0 || vPos >= vCount)
						continue;

					for (w = 0; w < DIAMETER; w++)
						{
						int wPos = START + wCenter + w;
						if (wPos < iPeakPoint || wPos >= wCount)
							continue;

						OpacityMap[vPos][wPos] = (BYTE)Min(Max(0, (int)((Metric)OpacityMap[vPos][wPos] * (1.0 + rCenter * Adj[v][w]))), 255);
						}
					}
				}
				
			break;
			}
		}

    //  Combine the opacity map and color map.

    if (OpacityMap.GetCount() > ColorMap.GetCount())
        {
        int iInsert = OpacityMap.GetCount() - ColorMap.GetCount();
        ColorMap.GrowToFit(iInsert);

        //  If we have no color column, then add one using the primary color.

        if (ColorMap.GetCount() == 0)
            {
            ColorArray *pSingleRow = ColorMap.Insert();
            pSingleRow->InsertEmpty(OpacityMap[0].GetCount());
            for (i = 0; i < pSingleRow->GetCount(); i++)
                pSingleRow->GetAt(i) = m_rgbPrimaryColor;

            iInsert--;
            }

        //  Copy the color columns.

        for (i = 0; i < iInsert; i++)
            ColorMap.Insert(ColorMap[0]);
        }
    else if (OpacityMap.GetCount() < ColorMap.GetCount())
        {
        int iInsert = ColorMap.GetCount() - OpacityMap.GetCount();
        OpacityMap.GrowToFit(iInsert);

        if (OpacityMap.GetCount() == 0)
            {
            OpacityArray *pSingleRow = OpacityMap.Insert();
            pSingleRow->InsertEmpty(ColorMap[0].GetCount());
            for (i = 0; i < pSingleRow->GetCount(); i++)
                pSingleRow->GetAt(i) = 255;

            iInsert--;
            }

        for (i = 0; i < iInsert; i++)
            OpacityMap.Insert(OpacityMap[0]);
        }

    //  Apply opacity

    for (v = 0; v < ColorMap.GetCount(); v++)
        for (w = 0; w < ColorMap[0].GetCount(); w++)
            ColorMap[v][w] = CG32bitPixel::PreMult(ColorMap[v][w], OpacityMap[v][w]);

    //  Create the renderer

    if (iStyle == styleLightning)
        {
        int iBoltCount = Max(1, Min(iIntensity / 5, 20));
        return new CLightningBundlePainter(iBoltCount, m_rgbPrimaryColor, m_rgbSecondaryColor, WidthAdjTop, WidthAdjBottom);
        }
    else
        {
        switch (m_iBlendMode)
            {
            case CGDraw::blendNormal:
                return new CRayRasterizer<CGBlendBlend>(iLengthCount, iWidthCount, ColorMap, OpacityMap, WidthAdjTop, WidthAdjBottom);

            case CGDraw::blendHardLight:
                return new CRayRasterizer<CGBlendHardLight>(iLengthCount, iWidthCount, ColorMap, OpacityMap, WidthAdjTop, WidthAdjBottom);

            case CGDraw::blendScreen:
                return new CRayRasterizer<CGBlendScreen>(iLengthCount, iWidthCount, ColorMap, OpacityMap, WidthAdjTop, WidthAdjBottom);

            case CGDraw::blendCompositeNormal:
                return new CRayRasterizer<CGBlendComposite>(iLengthCount, iWidthCount, ColorMap, OpacityMap, WidthAdjTop, WidthAdjBottom);

            default:
				return new CRayRasterizer<CGBlendBlend>(iLengthCount, iWidthCount, ColorMap, OpacityMap, WidthAdjTop, WidthAdjBottom);
            }
        }
    }

void CRayEffectPainter::CleanUpIntermediates (void)

//	CleanUpIntermediates
//
//	Clean up any allocations

	{
    int i;

    for (i = 0; i < m_RayRenderer.GetCount(); i++)
        delete m_RayRenderer[i];

	m_RayRenderer.DeleteAll();
	m_Length.DeleteAll();
	}

void CRayEffectPainter::GetParam (const CString &sParam, CEffectParamDesc *retValue)

//	GetParam
//
//	Returns the parameter

	{
	if (strEquals(sParam, ANIMATE_ATTRIB))
		retValue->InitInteger(m_iAnimation);

	else if (strEquals(sParam, BLEND_MODE_ATTRIB))
		retValue->InitInteger(m_iBlendMode);

	else if (strEquals(sParam, INTENSITY_ATTRIB))
		retValue->InitInteger(m_iIntensity);

	else if (strEquals(sParam, LENGTH_ATTRIB))
		retValue->InitInteger(m_iLength);

	else if (strEquals(sParam, LIFETIME_ATTRIB))
		retValue->InitInteger(m_iLifetime);

	else if (strEquals(sParam, PRIMARY_COLOR_ATTRIB))
		retValue->InitColor(m_rgbPrimaryColor);

	else if (strEquals(sParam, SECONDARY_COLOR_ATTRIB))
		retValue->InitColor(m_rgbSecondaryColor);
	
	else if (strEquals(sParam, SHAPE_ATTRIB))
		retValue->InitInteger(m_iShape);

	else if (strEquals(sParam, STYLE_ATTRIB))
		retValue->InitInteger(m_iStyle);

	else if (strEquals(sParam, WIDTH_ATTRIB))
		retValue->InitInteger(m_iWidth);

	else if (strEquals(sParam, XFORM_ROTATION_ATTRIB))
		retValue->InitInteger(m_iXformRotation);

	else
		retValue->InitNull();
	}

bool CRayEffectPainter::GetParamList (TArray<CString> *retList) const

//	GetParamList
//
//	Returns a list of value parameter names

	{
	retList->DeleteAll();
	retList->InsertEmpty(11);
	retList->GetAt(0) = ANIMATE_ATTRIB;
	retList->GetAt(1) = BLEND_MODE_ATTRIB;
	retList->GetAt(2) = INTENSITY_ATTRIB;
	retList->GetAt(3) = LENGTH_ATTRIB;
	retList->GetAt(4) = LIFETIME_ATTRIB;
	retList->GetAt(5) = PRIMARY_COLOR_ATTRIB;
	retList->GetAt(6) = SECONDARY_COLOR_ATTRIB;
	retList->GetAt(7) = SHAPE_ATTRIB;
	retList->GetAt(8) = STYLE_ATTRIB;
	retList->GetAt(9) = WIDTH_ATTRIB;
	retList->GetAt(10) = XFORM_ROTATION_ATTRIB;

	return true;
	}

void CRayEffectPainter::GetRect (RECT *retRect) const

//	GetRect
//
//	Returns the rect bounds of the image

	{
	int iSize = m_iLength;

	retRect->left = -iSize;
	retRect->right = iSize;
	retRect->top = -iSize;
	retRect->bottom = iSize;
	}

void CRayEffectPainter::Paint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	Paint
//
//	Paint the effect

	{
	DEBUG_TRY

	//	Make sure we've computed all our temporaries

	CalcIntermediates(m_iLength);

	//	Compute the two end points of the line. We paint from the head to the tail.

	CVector vFrom = PolarToVector(AngleMod(Ctx.iRotation + m_iXformRotation), -CalcLength(Ctx));
	int xTo = x + (int)(vFrom.GetX() + 0.5);
	int yTo = y - (int)(vFrom.GetY() + 0.5);
	int xFrom = x;
	int yFrom = y;

	//	Paint the effect

    PaintRay(Dest, xFrom, yFrom, xTo, yTo, Ctx);

	DEBUG_CATCH
	}

void CRayEffectPainter::PaintHit (CG32bitImage &Dest, int x, int y, const CVector &vHitPos, SViewportPaintCtx &Ctx)

//	PaintHit
//
//	Paint the effect when hit

	{
	//	Make sure we've computed all our temporaries

	CalcIntermediates(m_iLength);

	//	Compute the two end points of the line. We paint from the head to the tail.

	CVector vFrom = PolarToVector(AngleMod(Ctx.iRotation + m_iXformRotation), -CalcLength(Ctx));
	int xTo = x + (int)(vFrom.GetX() + 0.5);
	int yTo = y - (int)(vFrom.GetY() + 0.5);

	int xFrom;
	int yFrom;
	Ctx.XFormRel.Transform(vHitPos, &xFrom, &yFrom);

	//	Paint the effect

	PaintRay(Dest, xFrom, yFrom, xTo, yTo, Ctx);
	}

void CRayEffectPainter::PaintLine (CG32bitImage &Dest, const CVector &vHead, const CVector &vTail, SViewportPaintCtx &Ctx)

//	PaintLine
//
//	Paints a line given start and end.

	{
	int iLength = mathRound((vHead - vTail).Length() / g_KlicksPerPixel);

	CalcIntermediates(iLength);

	int xHead, yHead;
	Ctx.XFormRel.Transform(vHead, &xHead, &yHead);

	int xTail, yTail;
	Ctx.XFormRel.Transform(vTail, &xTail, &yTail);

	PaintRay(Dest, xHead, yHead, xTail, yTail, Ctx);
	}

void CRayEffectPainter::PaintRay (CG32bitImage &Dest, int xFrom, int yFrom, int xTo, int yTo, SViewportPaintCtx &Ctx)

//	PaintRay
//
//	Paints the ray

	{
    DEBUG_TRY

    //  If no renderer, nothing to do

    if (m_RayRenderer.GetCount() == 0)
        return;

	//	Paint based on animation

	switch (m_iAnimation)
		{
		case animateFade:
			{
        	DWORD dwOpacity;
			if (m_iLifetime > 0)
				dwOpacity = 255 * (m_iLifetime - (Ctx.iTick % m_iLifetime)) / m_iLifetime;
			else
				dwOpacity = 255;

	        //	Short-circuit

	        if (dwOpacity == 0)
		        return;

            m_RayRenderer[0]->SetParam(FIELD_OPACITY, (BYTE)dwOpacity);
            m_RayRenderer[0]->Draw(Dest, xFrom, yFrom, xTo, yTo, m_iWidth);
			break;
			}

		case animateCycle:
        case animateFlicker:
            m_RayRenderer[Ctx.iTick % m_RayRenderer.GetCount()]->Draw(Dest, xFrom, yFrom, xTo, yTo, m_iWidth);
            break;

		default:
            m_RayRenderer[0]->Draw(Dest, xFrom, yFrom, xTo, yTo, m_iWidth);
		}

	DEBUG_CATCH
	}

bool CRayEffectPainter::PointInImage (int x, int y, int iTick, int iVariant, int iRotation) const

//	PointInImage
//
//	Returns TRUE if the given point is in the image

	{
	//	We only intersect if we are inside a box around the center of 1/2 the
	//	width (since a lot of the width is taken up by glow effects).

	int iSize = (m_iWidth / 4);
	return (Absolute(x) <= iSize && Absolute(y) <= iSize);
	}

void CRayEffectPainter::OnSetParam (CCreatePainterCtx &Ctx, const CString &sParam, const CEffectParamDesc &Value)

//	SetParam
//
//	Sets parameters

	{
    if (strEquals(sParam, ANIMATE_ATTRIB))
        m_iAnimation = (EAnimationTypes)Value.EvalIdentifier(ANIMATION_TABLE, animateMax, animateNone);

	else if (strEquals(sParam, ANIMATE_OPACITY_ATTRIB))
        m_iAnimation = (EAnimationTypes)Value.EvalIdentifier(ANIMATION_TABLE, animateMax, animateNone);

	else if (strEquals(sParam, BLEND_MODE_ATTRIB))
		m_iBlendMode = Value.EvalBlendMode();

	else if (strEquals(sParam, INTENSITY_ATTRIB))
		m_iIntensity = Value.EvalIntegerBounded(0, 100, 50);

	else if (strEquals(sParam, LENGTH_ATTRIB))
		m_iLength = Value.EvalIntegerBounded(1, -1, (int)(STD_SECONDS_PER_UPDATE * LIGHT_SECOND / KLICKS_PER_PIXEL));

	else if (strEquals(sParam, LIFETIME_ATTRIB))
		m_iLifetime = Value.EvalIntegerBounded(0, -1, 0);

	else if (strEquals(sParam, PRIMARY_COLOR_ATTRIB))
		m_rgbPrimaryColor = Value.EvalColor();

	else if (strEquals(sParam, SECONDARY_COLOR_ATTRIB))
		m_rgbSecondaryColor = Value.EvalColor();
	
	else if (strEquals(sParam, SHAPE_ATTRIB))
		m_iShape = (ERayShapes)Value.EvalIdentifier(SHAPE_TABLE, shapeMax, shapeStraight);

	else if (strEquals(sParam, STYLE_ATTRIB))
		m_iStyle = (ERayStyles)Value.EvalIdentifier(STYLE_TABLE, styleMax, styleGlow);

	else if (strEquals(sParam, WIDTH_ATTRIB))
		m_iWidth = Value.EvalIntegerBounded(1, -1, 10);

	else if (strEquals(sParam, XFORM_ROTATION_ATTRIB))
		m_iXformRotation = Value.EvalIntegerBounded(-359, 359, 0);
	}
