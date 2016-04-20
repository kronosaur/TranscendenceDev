//	SFXRay.cpp
//
//	CRayEffectCreator class
//	Copyright (c) 2013 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

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

//  Grayscale value at each point on the 2D ray [length][width]

typedef TArray<BYTE> OpacityArray;
typedef TArray<OpacityArray> OpacityPlane;

//  Color of each point on the 2D ray [length][width]

typedef TArray<CG32bitPixel> ColorArray;
typedef TArray<ColorArray> ColorPlane;

//  Adjustment to width at each point along the length of the ray

typedef TArray<Metric> WidthAdjArray;

template <class BLENDER> class CRayRasterizer : public TLinePainter32<CRayRasterizer<BLENDER>, BLENDER>
    {
    public:
        CRayRasterizer (int iLengthCount, int iWidthCount, const ColorPlane &ColorMap, const OpacityPlane &OpacityMap, const WidthAdjArray &WidthAdjTop, const WidthAdjArray &WidthAdjBottom) :
                m_iLengthCount(iLengthCount),
                m_iWidthCount(iWidthCount),
                m_rgbColor(255, 255, 255),
                m_byOpacity(255),
                m_ColorMap(ColorMap),
                m_OpacityMap(OpacityMap),
                m_WidthAdjTop(WidthAdjTop),
                m_WidthAdjBottom(WidthAdjBottom)
            { }

        virtual void SetParam (const CString &sParam, BYTE byValue) override
            {
            if (strEquals(sParam, FIELD_OPACITY))
                m_byOpacity = byValue;
            }

    private:
        inline CG32bitPixel GetEdgePixel (Metric rEdge, Metric rV, Metric rW)
            {
            CG32bitPixel rgbColor = GetPixel(rV, rW);
            BYTE byOpacity = (BYTE)(DWORD)(255.0 * (rEdge > 1.0 ? 1.0 : rEdge));
            if (rgbColor.GetAlpha() == 0xff)
                return CG32bitPixel(rgbColor, byOpacity);
            else
                return CG32bitPixel(rgbColor, CG32bitPixel::BlendAlpha(rgbColor.GetAlpha(), byOpacity));
            }

        inline CG32bitPixel GetPixel (Metric rV, Metric rW)
            {
		    //	Scale v and w to map to our array sizes

		    int v = (int)(m_iLengthCount * rV);

		    //	If necessary, adjust the width of the ray

		    if (m_WidthAdjTop.GetCount() > 0)
			    {
			    //	If this pixel is outside the width adjustment, then we skip it

                if (v < 0 || v >= m_iLengthCount)
                    return CG32bitPixel::Null();

			    //	Positive values are above

			    else if (rW > 0.0)
				    {
				    if (rW > m_WidthAdjTop[v])
					    return CG32bitPixel::Null();

				    rW = (m_WidthAdjTop[v] > 0.0 ? rW / m_WidthAdjTop[v] : 0.0);
				    }

			    //	Otherwise, we use bottom adjustment

			    else
				    {
				    if (-rW > m_WidthAdjBottom[v])
					    return CG32bitPixel::Null();

				    rW = (m_WidthAdjBottom[v] > 0.0 ? rW / m_WidthAdjBottom[v] : 0.0);
				    }
			    }
		    else
			    v = Min(Max(0, v), m_iLengthCount - 1);

		    //	Width adjustment touches Pixel.rW, so we need to do this afterwards.

		    Metric rWPixels = (m_iWidthCount * Absolute(rW));
		    int w = (int)rWPixels;
		    if (w < 0 || w >= m_iWidthCount)
			    return CG32bitPixel::Null();

		    //	Compute color

		    CG32bitPixel rgbColor;
		    if (m_ColorMap.GetCount() == 1)
			    {
			    rgbColor = m_ColorMap[0][w];
			    if (w > 0)
				    {
				    CG32bitPixel rgbAAColor = m_ColorMap[0][w - 1];
				    rgbColor = CG32bitPixel::Composite(rgbAAColor, rgbColor, rWPixels - (Metric)w);
				    }
			    }

            else if (m_ColorMap.GetCount() > 1)
                {
			    rgbColor = m_ColorMap[v][w];
                if (w > 0)
                    {
				    CG32bitPixel rgbAAColor = m_ColorMap[v][w - 1];
				    rgbColor = CG32bitPixel::Composite(rgbAAColor, rgbColor, rWPixels - (Metric)w);
                    }
                }

		    else
			    rgbColor = m_rgbColor;

		    //	Apply opacity, if necessary

            if (m_byOpacity != 255)
                rgbColor = CG32bitPixel::Fade(rgbColor, m_byOpacity);

		    //	Draw

            return rgbColor;
            }

		int m_iLengthCount;					//	Count of cells along ray length
		int m_iWidthCount;					//	Count of cells from ray axis out to edge
        CG32bitPixel m_rgbColor;            //  Solid color only
        BYTE m_byOpacity;                   //  Opacity
		const ColorPlane &m_ColorMap;		    //	Full color map
		const OpacityPlane &m_OpacityMap;	    //	Full opacity map
		const WidthAdjArray &m_WidthAdjTop;	    //	Top width adjustment
		const WidthAdjArray &m_WidthAdjBottom;	//	Bottom width adjustment

    friend TLinePainter32;
    };

class CRayEffectPainter : public IEffectPainter
	{
	public:
		CRayEffectPainter (CEffectCreator *pCreator);
		~CRayEffectPainter (void);

		//	IEffectPainter virtuals
		virtual CEffectCreator *GetCreator (void) { return m_pCreator; }
		virtual int GetLifetime (void) { return m_iLifetime; }
		virtual void GetParam (const CString &sParam, CEffectParamDesc *retValue);
		virtual bool GetParamList (TArray<CString> *retList) const;
		virtual void GetRect (RECT *retRect) const;
		virtual void Paint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);
		virtual void PaintHit (CG32bitImage &Dest, int x, int y, const CVector &vHitPos, SViewportPaintCtx &Ctx);
		virtual bool PointInImage (int x, int y, int iTick, int iVariant = 0, int iRotation = 0) const;

	protected:
		virtual void OnSetParam (CCreatePainterCtx &Ctx, const CString &sParam, const CEffectParamDesc &Value);

	private:
		enum EAnimationTypes
			{
			animateNone =			0,

			animateFade =			1,
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

			styleMax =				5,
			};

		void CalcCone (TArray<Metric> &AdjArray);
		void CalcDiamond (TArray<Metric> &AdjArray);
		void CalcIntermediates (void);
		void CalcOval (TArray<Metric> &AdjArray);
		void CalcTaper (TArray<Metric> &AdjArray);
		void CalcWaves (TArray<Metric> &AdjArray, Metric rAmplitude, Metric rWavelength);
		void PaintLightning (CG32bitImage &Dest, int xFrom, int yFrom, int xTo, int yTo, SViewportPaintCtx &Ctx);
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
		EAnimationTypes m_iOpacityAnimation;

		//	Temporary variables based on shape/style/etc.

		bool m_bInitialized;				//	TRUE if values are valid
        ILinePainter *m_pRayRenderer;
		int m_iLengthCount;					//	Count of cells along ray length
		int m_iWidthCount;					//	Count of cells from ray axis out to edge
		ColorPlane m_ColorMap;		        //	Full color map
		OpacityPlane m_OpacityMap;	        //	Full opacity map
		WidthAdjArray m_WidthAdjTop;		//	Top width adjustment
		WidthAdjArray m_WidthAdjBottom;	    //	Bottom width adjustment
	};

const int BLOB_PEAK_FRACTION =			1;
const int TAPER_FRACTION =				3;
const int TAPER_PEAK_FRACTION =			8;
const int JAGGED_PEAK_FRACTION =		3;
const Metric BLOB_WAVE_SIZE =			0.30;
const Metric JAGGED_AMPLITUDE =			0.45;
const Metric JAGGED_WAVELENGTH_FACTOR =	0.33;
const Metric WAVY_WAVELENGTH_FACTOR =	1.0;

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

	pPainter->SetParam(Ctx, ANIMATE_OPACITY_ATTRIB, m_AnimateOpacity);
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

	if (error = m_AnimateOpacity.InitIdentifierFromXML(Ctx, pDesc->GetAttribute(ANIMATE_OPACITY_ATTRIB), ANIMATION_TABLE))
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
		m_iOpacityAnimation(animateNone),
		m_bInitialized(false),
        m_pRayRenderer(NULL)

//	CRayEffectCreator constructor

	{
	}

CRayEffectPainter::~CRayEffectPainter (void)

//	CRayEffectCreator destructor

	{
    if (m_pRayRenderer)
        delete m_pRayRenderer;
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

void CRayEffectPainter::CalcIntermediates (void)

//	CalcIntermediates
//
//	Calculate intermediate values used for painting.

	{
	int i, v, w;

	if (!m_bInitialized)
		{
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
			};

		EColorTypes iColorTypes = colorNone;
		EOpacityTypes iOpacityTypes = opacityNone;
		EWidthAdjTypes iWidthAdjType = widthAdjNone;
		EWidthAdjTypes iReshape = widthAdjNone;
		EOpacityTypes iTexture = opacityNone;

		m_iWidthCount = m_iWidth;
		m_iLengthCount = 2 * m_iLength;

		//	Every combination requires a slightly different set up

		switch (m_iShape)
			{
			case shapeCone:
				switch (m_iStyle)
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
					}
				break;

			case shapeDiamond:
				switch (m_iStyle)
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
					}
				break;

			case shapeOval:
				switch (m_iStyle)
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
					}
				break;

			case shapeStraight:
				switch (m_iStyle)
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
					}
				break;

			case shapeTapered:
				switch (m_iStyle)
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
					}
				break;
			}

		//	Full color map

		switch (iColorTypes)
			{
			case colorGlow:
				{
				m_ColorMap.InsertEmpty(1);
				m_ColorMap[0].InsertEmpty(m_iWidthCount);

				//	The center blends towards white

				CG32bitPixel rgbCenter = CG32bitPixel::Blend(m_rgbPrimaryColor, CG32bitPixel(255, 255, 255), Min(50, m_iIntensity) / 50.0);

				int iBrightPoint = (int)(BRIGHT_FACTOR * m_iWidthCount * m_iIntensity);
				for (i = 0; i < iBrightPoint; i++)
					m_ColorMap[0][i] = rgbCenter;

				//	The rest fades (linearly) from primary color to secondary color

				Metric rFadeInc = (m_iWidthCount > 0 ? (1.0 / (m_iWidthCount - iBrightPoint)) : 0.0);
				Metric rFade = 1.0;
				for (i = iBrightPoint; i < m_iWidthCount; i++, rFade -= rFadeInc)
					m_ColorMap[0][i] = CG32bitPixel::Blend(m_rgbSecondaryColor, m_rgbPrimaryColor, rFade);

				break;
				}
			}

		//	Full opacity

		switch (iOpacityTypes)
			{
			case opacityGlow:
				{
				m_OpacityMap.InsertEmpty(1);
				m_OpacityMap[0].InsertEmpty(m_iWidthCount);

				//	From center to peak we have solid opacity

				int iPeakPoint = (int)(SOLID_FACTOR * m_iIntensity * m_iWidthCount);
				for (i = 0; i < iPeakPoint; i++)
					m_OpacityMap[0][i] = 255;

				//	We decay exponentially to edge

				Metric rGlowLevel = MIN_GLOW_LEVEL + (m_iIntensity * GLOW_FACTOR);
				Metric rGlowInc = (m_iWidthCount > 0 ? (1.0 / (m_iWidthCount - iPeakPoint)) : 0.0);
				Metric rGlow = 1.0;
				for (i = iPeakPoint; i < m_iWidthCount; i++, rGlow -= rGlowInc)
					m_OpacityMap[0][i] = (int)(255.0 * rGlowLevel * rGlow * rGlow);

				break;
				}

			case opacityTaperedGlow:
				{
				m_OpacityMap.InsertEmpty(m_iLengthCount);
				for (i = 0; i < m_iLengthCount; i++)
					m_OpacityMap[i].InsertEmpty(m_iWidthCount);

				//	From center to peak we have solid opacity

				int iPeakPoint = (int)(SOLID_FACTOR * m_iIntensity * m_iWidthCount);

				//	After the 1/3 point start fading out (linearly)

				int iFadePoint = m_iLengthCount / TAPER_FRACTION;
				Metric rTaperInc = (m_iLengthCount > 0 ? (1.0 / (m_iLengthCount - iFadePoint)) : 0.0);

				//	From center to peak we have solid opacity plus taper

				for (w = 0; w < iPeakPoint; w++)
					{
					for (v = 0; v < iFadePoint; v++)
						m_OpacityMap[v][w] = 255;

					Metric rTaper = 1.0;
					for (v = iFadePoint; v < m_iLengthCount; v++, rTaper -= rTaperInc)
						m_OpacityMap[v][w] = (int)(255.0 * rTaper);
					}

				//	The glow around the peak decays exponentially

				Metric rGlowLevel = MIN_GLOW_LEVEL + (m_iIntensity * GLOW_FACTOR);
				Metric rGlowInc = (m_iWidthCount > 0 ? (1.0 / (m_iWidthCount - iPeakPoint)) : 0.0);
				Metric rGlow = 1.0;
				for (w = iPeakPoint; w < m_iWidthCount; w++, rGlow -= rGlowInc)
					{
					Metric rGlowPart = rGlowLevel * rGlow * rGlow;

					for (v = 0; v < iFadePoint; v++)
						m_OpacityMap[v][w] = (int)(255.0 * rGlowPart);

					Metric rTaper = 1.0;
					for (v = iFadePoint; v < m_iLengthCount; v++, rTaper -= rTaperInc)
						m_OpacityMap[v][w] = (int)(255.0 * rGlowPart * rTaper);
					}

				break;
				}
			}

		//	Width adjustments

		switch (iWidthAdjType)
			{
			case widthAdjBlob:
				{
				m_WidthAdjTop.InsertEmpty(m_iLengthCount);
				m_WidthAdjBottom.InsertEmpty(m_iLengthCount);

				//	Initialize jagged envelope

				CalcWaves(m_WidthAdjTop, BLOB_WAVE_SIZE, m_iWidth * WAVY_WAVELENGTH_FACTOR);
				CalcWaves(m_WidthAdjBottom, BLOB_WAVE_SIZE, m_iWidth * WAVY_WAVELENGTH_FACTOR);

				break;
				}

			case widthAdjCone:
				{
				m_WidthAdjTop.InsertEmpty(m_iLengthCount);

				CalcCone(m_WidthAdjTop);
				m_WidthAdjBottom = m_WidthAdjTop;

				break;
				}

			case widthAdjDiamond:
				{
				m_WidthAdjTop.InsertEmpty(m_iLengthCount);

				CalcDiamond(m_WidthAdjTop);
				m_WidthAdjBottom = m_WidthAdjTop;

				break;
				}

			case widthAdjJagged:
				{
				m_WidthAdjTop.InsertEmpty(m_iLengthCount);
				m_WidthAdjBottom.InsertEmpty(m_iLengthCount);

				//	Initialize jagged envelope

				CalcWaves(m_WidthAdjTop, JAGGED_AMPLITUDE, m_iWidth * JAGGED_WAVELENGTH_FACTOR);
				CalcWaves(m_WidthAdjBottom, JAGGED_AMPLITUDE, m_iWidth * JAGGED_WAVELENGTH_FACTOR);

				break;
				}

			case widthAdjOval:
				{
				m_WidthAdjTop.InsertEmpty(m_iLengthCount);

				CalcOval(m_WidthAdjTop);
				m_WidthAdjBottom = m_WidthAdjTop;

				break;
				}

			case widthAdjTapered:
				{
				m_WidthAdjTop.InsertEmpty(m_iLengthCount);

				CalcTaper(m_WidthAdjTop);
				m_WidthAdjBottom = m_WidthAdjTop;

				break;
				}
			}

		//	Adjust shape

		switch (iReshape)
			{
			case widthAdjCone:
				{
				TArray<Metric> TaperAdj;
				TaperAdj.InsertEmpty(m_iLengthCount);
				CalcCone(TaperAdj);

				for (i = 0; i < m_iLengthCount; i++)
					{
					m_WidthAdjTop[i] *= TaperAdj[i];
					m_WidthAdjBottom[i] *= TaperAdj[i];
					}
				break;
				}

			case widthAdjDiamond:
				{
				TArray<Metric> TaperAdj;
				TaperAdj.InsertEmpty(m_iLengthCount);
				CalcDiamond(TaperAdj);

				for (i = 0; i < m_iLengthCount; i++)
					{
					m_WidthAdjTop[i] *= TaperAdj[i];
					m_WidthAdjBottom[i] *= TaperAdj[i];
					}
				break;
				}

			case widthAdjOval:
				{
				TArray<Metric> TaperAdj;
				TaperAdj.InsertEmpty(m_iLengthCount);
				CalcOval(TaperAdj);

				for (i = 0; i < m_iLengthCount; i++)
					{
					m_WidthAdjTop[i] *= TaperAdj[i];
					m_WidthAdjBottom[i] *= TaperAdj[i];
					}
				break;
				}

			case widthAdjTapered:
				{
				TArray<Metric> TaperAdj;
				TaperAdj.InsertEmpty(m_iLengthCount);
				CalcTaper(TaperAdj);

				for (i = 0; i < m_iLengthCount; i++)
					{
					m_WidthAdjTop[i] *= TaperAdj[i];
					m_WidthAdjBottom[i] *= TaperAdj[i];
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

				int iPeakPoint = (int)(SOLID_FACTOR * m_iIntensity * m_iWidthCount);

				int iGrainCount = (int)(4 * sqrt((Metric)m_iLength * m_iWidth));
				for (i = 0; i < iGrainCount; i++)
					{
					int vCount = m_OpacityMap.GetCount();
					int wCount = m_OpacityMap[0].GetCount();
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

							m_OpacityMap[vPos][wPos] = (BYTE)Min(Max(0, (int)((Metric)m_OpacityMap[vPos][wPos] * (1.0 + rCenter * Adj[v][w]))), 255);
							}
						}
					}
				
				break;
				}
			}

        //  Combine the opacity map and color map.

        if (m_OpacityMap.GetCount() > m_ColorMap.GetCount())
            {
            int iInsert = m_OpacityMap.GetCount() - m_ColorMap.GetCount();
            m_ColorMap.GrowToFit(iInsert);

            //  If we have no color column, then add one using the primary color.

            if (m_ColorMap.GetCount() == 0)
                {
                ColorArray *pSingleRow = m_ColorMap.Insert();
                pSingleRow->InsertEmpty(m_OpacityMap[0].GetCount());
                for (i = 0; i < pSingleRow->GetCount(); i++)
                    pSingleRow->GetAt(i) = m_rgbPrimaryColor;

                iInsert--;
                }

            //  Copy the color columns.

            for (i = 0; i < iInsert; i++)
                m_ColorMap.Insert(m_ColorMap[0]);
            }
        else if (m_OpacityMap.GetCount() < m_ColorMap.GetCount())
            {
            int iInsert = m_ColorMap.GetCount() - m_OpacityMap.GetCount();
            m_OpacityMap.GrowToFit(iInsert);

            if (m_OpacityMap.GetCount() == 0)
                {
                OpacityArray *pSingleRow = m_OpacityMap.Insert();
                pSingleRow->InsertEmpty(m_ColorMap[0].GetCount());
                for (i = 0; i < pSingleRow->GetCount(); i++)
                    pSingleRow->GetAt(i) = 255;

                iInsert--;
                }

            for (i = 0; i < iInsert; i++)
                m_OpacityMap.Insert(m_OpacityMap[0]);
            }

        //  Apply opacity

        for (v = 0; v < m_ColorMap.GetCount(); v++)
            for (w = 0; w < m_ColorMap[0].GetCount(); w++)
                m_ColorMap[v][w] = CG32bitPixel::PreMult(m_ColorMap[v][w], m_OpacityMap[v][w]);

        //  Create the painter

        if (m_iStyle != styleLightning)
            {
            switch (m_iBlendMode)
                {
                case CGDraw::blendNormal:
                    m_pRayRenderer = new CRayRasterizer<CGBlendCopy>(m_iLengthCount, m_iWidthCount, m_ColorMap, m_OpacityMap, m_WidthAdjTop, m_WidthAdjBottom);
                    break;

                case CGDraw::blendHardLight:
                    m_pRayRenderer = new CRayRasterizer<CGBlendHardLight>(m_iLengthCount, m_iWidthCount, m_ColorMap, m_OpacityMap, m_WidthAdjTop, m_WidthAdjBottom);
                    break;

                case CGDraw::blendScreen:
                    m_pRayRenderer = new CRayRasterizer<CGBlendScreen>(m_iLengthCount, m_iWidthCount, m_ColorMap, m_OpacityMap, m_WidthAdjTop, m_WidthAdjBottom);
                    break;

                case CGDraw::blendCompositeNormal:
                    m_pRayRenderer = new CRayRasterizer<CGBlendComposite>(m_iLengthCount, m_iWidthCount, m_ColorMap, m_OpacityMap, m_WidthAdjTop, m_WidthAdjBottom);
                    break;
                }
            }

		//	Done

		m_bInitialized = true;
		}
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

	Metric rY = 1.0;
	Metric rYInc = (AdjArray.GetCount() > 0 ? (1.0 / (Metric)(AdjArray.GetCount() - iPeakPoint)) : 0.0);
	for (i = iPeakPoint; i < AdjArray.GetCount(); i++, rY -= rYInc)
		AdjArray[i] = rY;
	}

void CRayEffectPainter::CalcWaves (TArray<Metric> &AdjArray, Metric rAmplitude, Metric rWavelength)

//	CalcWaves
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

void CRayEffectPainter::GetParam (const CString &sParam, CEffectParamDesc *retValue)

//	GetParam
//
//	Returns the parameter

	{
	if (strEquals(sParam, ANIMATE_OPACITY_ATTRIB))
		retValue->InitInteger(m_iOpacityAnimation);

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
	retList->GetAt(0) = ANIMATE_OPACITY_ATTRIB;
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

	CalcIntermediates();

	//	Compute the two end points of the line. We paint from the head to the tail.

	int iLength = (Ctx.iMaxLength != -1 ? Min(Ctx.iMaxLength, m_iLength) : m_iLength);
	CVector vFrom = PolarToVector(AngleMod(Ctx.iRotation + m_iXformRotation), -iLength);
	int xTo = x + (int)(vFrom.GetX() + 0.5);
	int yTo = y - (int)(vFrom.GetY() + 0.5);
	int xFrom = x;
	int yFrom = y;

	//	Paint the effect

	switch (m_iStyle)
		{
		case styleLightning:
			PaintLightning(Dest, xFrom, yFrom, xTo, yTo, Ctx);
			break;

		default:
			PaintRay(Dest, xFrom, yFrom, xTo, yTo, Ctx);
			break;
		}

	DEBUG_CATCH
	}

void CRayEffectPainter::PaintHit (CG32bitImage &Dest, int x, int y, const CVector &vHitPos, SViewportPaintCtx &Ctx)

//	PaintHit
//
//	Paint the effect when hit

	{
	//	Make sure we've computed all our temporaries

	CalcIntermediates();

	//	Compute the two end points of the line. We paint from the head to the tail.

	int iLength = (Ctx.iMaxLength != -1 ? Min(Ctx.iMaxLength, m_iLength) : m_iLength);
	CVector vFrom = PolarToVector(AngleMod(Ctx.iRotation + m_iXformRotation), -iLength);
	int xTo = x + (int)(vFrom.GetX() + 0.5);
	int yTo = y - (int)(vFrom.GetY() + 0.5);

	int xFrom;
	int yFrom;
	Ctx.XFormRel.Transform(vHitPos, &xFrom, &yFrom);

	//	Paint the effect

	switch (m_iStyle)
		{
		case styleLightning:
			PaintLightning(Dest, xFrom, yFrom, xTo, yTo, Ctx);
			break;

		default:
			PaintRay(Dest, xFrom, yFrom, xTo, yTo, Ctx);
			break;
		}
	}

void CRayEffectPainter::PaintLightning (CG32bitImage &Dest, int xFrom, int yFrom, int xTo, int yTo, SViewportPaintCtx &Ctx)

//	PaintLightning
//
//	Paint a lightning effect

	{
	int i;

	//	Figure out how many bolts to paint based on intensity

	int iBoltCount = Max(1, Min(m_iIntensity / 5, 20));
	Metric rChaos = 0.2;

	//	Generate some intermediates

	CVector vCenterLine = CVector(xTo, yTo) - CVector(xFrom, yFrom);
	Metric rLength;
	CVector vAxis = vCenterLine.Normal(&rLength);
	CVector vTangent = vAxis.Perpendicular();

	//	We fade at the end

	CG32bitPixel rgbTo = CG32bitPixel(m_rgbSecondaryColor, 0);

	//	Paint each bolt

	for (i = 0; i < iBoltCount; i++)
		{
		//	Each bolt consists of two segments. We pick a middle point along the ray 
		//	shape to divide the two segments.
		//
		//	Start by picking a value from 0.2 to 0.8

		Metric rMid = mathRandom(20, 80) / 100.0;

		//	Get the width of the shape at this point along the ray

		int iWidthMid = (int)(m_iLengthCount * rMid);
		Metric rWidthAdj = (mathRandom(1, 2) == 1 ? m_WidthAdjTop[iWidthMid] : -m_WidthAdjBottom[iWidthMid]) * m_iWidth * 0.5;

		//	Compute opacity at midpoint and then compute the midpoint color

		CG32bitPixel rgbMid = CG32bitPixel::Composite(m_rgbPrimaryColor, rgbTo, rMid);

		//	vMid is the midpoint, relative to the front of the ray

		CVector vMidLine = vAxis * rMid * rLength;
		CVector vMid = vMidLine + (vTangent * rWidthAdj);

		//	Convert to screen coordinates

		int xMid = xFrom + (int)vMid.GetX();
		int yMid = yFrom + (int)vMid.GetY();

		//	Draw the two bolts

		DrawLightning(Dest, xFrom, yFrom, xMid, yMid, m_rgbPrimaryColor, rgbMid, rChaos);
		DrawLightning(Dest, xMid, yMid, xTo, yTo, rgbMid, rgbTo, rChaos);
		}
	}

void CRayEffectPainter::PaintRay (CG32bitImage &Dest, int xFrom, int yFrom, int xTo, int yTo, SViewportPaintCtx &Ctx)

//	PaintRay
//
//	Paints the ray

	{
	DEBUG_TRY

	//	Compute opacity animation

	DWORD dwOpacity;
	switch (m_iOpacityAnimation)
		{
		case animateFade:
			{
			if (m_iLifetime > 0)
				dwOpacity = 255 * (m_iLifetime - (Ctx.iTick % m_iLifetime)) / m_iLifetime;
			else
				dwOpacity = 255;
			break;
			}

		default:
			dwOpacity = 255;
		}

	//	Short-circuit

	if (dwOpacity == 0)
		return;

    //  Paint based on blend mode

    if (m_pRayRenderer)
        {
        m_pRayRenderer->SetParam(FIELD_OPACITY, (BYTE)dwOpacity);
        m_pRayRenderer->Draw(Dest, xFrom, yFrom, xTo, yTo, m_iWidth);
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
	if (strEquals(sParam, ANIMATE_OPACITY_ATTRIB))
		m_iOpacityAnimation = (EAnimationTypes)Value.EvalIntegerBounded(Ctx, 0, -1, animateNone);

	else if (strEquals(sParam, BLEND_MODE_ATTRIB))
		m_iBlendMode = Value.EvalBlendMode(Ctx);

	else if (strEquals(sParam, INTENSITY_ATTRIB))
		m_iIntensity = Value.EvalIntegerBounded(Ctx, 0, 100, 50);

	else if (strEquals(sParam, LENGTH_ATTRIB))
		m_iLength = Value.EvalIntegerBounded(Ctx, 1, -1, (int)(STD_SECONDS_PER_UPDATE * LIGHT_SECOND / KLICKS_PER_PIXEL));

	else if (strEquals(sParam, LIFETIME_ATTRIB))
		m_iLifetime = Value.EvalIntegerBounded(Ctx, 0, -1, 0);

	else if (strEquals(sParam, PRIMARY_COLOR_ATTRIB))
		m_rgbPrimaryColor = Value.EvalColor(Ctx);

	else if (strEquals(sParam, SECONDARY_COLOR_ATTRIB))
		m_rgbSecondaryColor = Value.EvalColor(Ctx);
	
	else if (strEquals(sParam, SHAPE_ATTRIB))
		m_iShape = (ERayShapes)Value.EvalIdentifier(Ctx, SHAPE_TABLE, shapeMax, shapeStraight);

	else if (strEquals(sParam, STYLE_ATTRIB))
		m_iStyle = (ERayStyles)Value.EvalIdentifier(Ctx, STYLE_TABLE, styleMax, styleGlow);

	else if (strEquals(sParam, WIDTH_ATTRIB))
		m_iWidth = Value.EvalIntegerBounded(Ctx, 1, -1, 10);

	else if (strEquals(sParam, XFORM_ROTATION_ATTRIB))
		m_iXformRotation = Value.EvalIntegerBounded(Ctx, -359, 359, 0);
	}
