//	SFXOrb.cpp
//
//	COrbEffectCreator class
//	Copyright (c) 2014 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "SFXFractalImpl.h"

#define ANIMATE_ATTRIB					CONSTLIT("animate")
#define BLEND_MODE_ATTRIB				CONSTLIT("blendMode")
#define DETAIL_ATTRIB					CONSTLIT("detail")
#define DISTORTION_ATTRIB				CONSTLIT("distortion")
#define INTENSITY_ATTRIB				CONSTLIT("intensity")
#define LIFETIME_ATTRIB					CONSTLIT("lifetime")
#define PRIMARY_COLOR_ATTRIB			CONSTLIT("primaryColor")
#define RADIUS_ATTRIB					CONSTLIT("radius")
#define SECONDARY_COLOR_ATTRIB			CONSTLIT("secondaryColor")
#define SPIKE_COUNT_ATTRIB				CONSTLIT("spikeCount")
#define STYLE_ATTRIB					CONSTLIT("style")

const int FLARE_MULITPLE =				4;
const int FLARE_WIDTH_FRACTION =		32;
const Metric BLOOM_FACTOR =				1.2;

class COrbEffectPainter : public IEffectPainter
	{
	public:
		COrbEffectPainter (CEffectCreator *pCreator);
		~COrbEffectPainter (void);

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
		enum EAnimationTypes
			{
			animateNone =			0,

			animateDissipate =		1,
			animateFade =			2,
			animateExplode =		3,

			animateMax =			3,
			};

		enum EOrbStyles
			{
			styleUnknown =			0,

			styleSmooth =			1,
			styleFlare =			2,
			styleCloud =			3,
			styleFireblast =		4,
			styleSmoke =			5,
			styleDiffraction =		6,
			styleFirecloud =		7,

			styleMax =				7,
			};

		struct SFlareDesc
			{
			int iLength;
			int iWidth;
			};

		inline CG32bitPixel CalcFlarePoint (CGRasterize::SLinePixel &Pixel)
			{
			Metric rDist = (1.0 - (2.0 * Absolute(Pixel.rV - 0.5)));
			Metric rSpread = (1.0 - Absolute(Pixel.rW));
			Metric rValue = BLOOM_FACTOR * (rDist * rDist) * (rSpread * rSpread);

			BYTE byOpacity = (rValue >= 1.0 ? Pixel.byAlpha : (BYTE)(rValue * Pixel.byAlpha));

			return CG32bitPixel(255, 255, 255, byOpacity);
			}

		bool CalcIntermediates (void);
		void CalcSecondaryColorTable (int iRadius, int iIntensity, BYTE byOpacity, TArray<CG32bitPixel> *retColorTable);
		void CalcSphericalColorTable (EOrbStyles iStyle, int iRadius, int iIntensity, CG32bitPixel rgbPrimary, CG32bitPixel rgbSecondary, BYTE byOpacity, TArray<CG32bitPixel> *retColorTable);
		void CompositeFlareRay (CG32bitImage &Dest, int xCenter, int yCenter, int iLength, int iWidth, int iAngle, int iIntensity, SViewportPaintCtx &Ctx);
		void CompositeFlares (CG32bitImage &Dest, int xCenter, int yCenter, const SFlareDesc &FlareDesc, SViewportPaintCtx &Ctx);
		inline bool HasFlares (void) const { return (m_iStyle == styleFlare || m_iStyle == styleDiffraction || m_iStyle == styleFireblast); }
		void Invalidate (void);
		void PaintFlareRay (CG32bitImage &Dest, int xCenter, int yCenter, int iLength, int iWidth, int iAngle, int iIntensity, SViewportPaintCtx &Ctx);
		void PaintFlares (CG32bitImage &Dest, int xCenter, int yCenter, const SFlareDesc &FlareDesc, SViewportPaintCtx &Ctx);
		inline bool UsesColorTable2 (void) const { return (m_iStyle == styleCloud || m_iStyle == styleFireblast || m_iStyle == styleFirecloud || m_iStyle == styleSmoke); }
		inline bool UsesTextures (void) const { return (m_iStyle == styleCloud || m_iStyle == styleFireblast || m_iStyle == styleFirecloud || m_iStyle == styleSmoke); }

		CEffectCreator *m_pCreator;

		int m_iRadius;
		EOrbStyles m_iStyle;
		int m_iIntensity;
		int m_iDetail;
		int m_iDistortion;
		DiceRange m_SpikeCount;
		CG32bitPixel m_rgbPrimaryColor;
		CG32bitPixel m_rgbSecondaryColor;
		CGDraw::EBlendModes m_iBlendMode;

		int m_iLifetime;
		EAnimationTypes m_iAnimation;

		//	Temporary variables based on shape/style/etc.

		bool m_bInitialized;				//	TRUE if values are valid
		ICirclePainter *m_pPainter;			//	Circle painter
		TArray<TArray<CG32bitPixel>> m_ColorTable;	//	Radial color table (per animation frame)
		TArray<SFlareDesc> m_FlareDesc;		//	Flare spikes (per animation frame)
		CFractalTextureLibrary::ETextureTypes m_iTextureType;
		TArray<int> m_TextureFrame;			//	Texture frame to use

		TArray<TArray<CG32bitPixel>> m_ColorTable2;	//	Additional color table (used by fireblast)

		static CExplosionColorizer m_ExplosionColorizer;
	};

static LPSTR ANIMATION_TABLE[] =
	{
	//	Must be same order as EAnimationTypes
		"",

		"dissipate",
		"fade",
		"explode",

		NULL
	};

static LPSTR STYLE_TABLE[] =
	{
	//	Must be same order as EOrbStyles
		"",

		"smooth",
		"flare",
		"cloud",
		"fireblast",
		"smoke",
		"diffraction",
		"firecloud",

		NULL,
	};

COrbEffectCreator::COrbEffectCreator (void) :
			m_pSingleton(NULL)

//	COrbEffectCreator constructor

	{
	}

COrbEffectCreator::~COrbEffectCreator (void)

//	COrbEffectCreator destructor

	{
	if (m_pSingleton)
		delete m_pSingleton;
	}

IEffectPainter *COrbEffectCreator::OnCreatePainter (CCreatePainterCtx &Ctx)

//	CreatePainter
//
//	Creates a new painter
	
	{
	//	If we have a singleton, return that

	if (m_pSingleton)
		return m_pSingleton;

	//	Otherwise we need to create a painter with the actual
	//	parameters.

	IEffectPainter *pPainter = new COrbEffectPainter(this);

	//	Initialize the painter parameters

	pPainter->SetParam(Ctx, ANIMATE_ATTRIB, m_Animate);
	pPainter->SetParam(Ctx, BLEND_MODE_ATTRIB, m_BlendMode);
	pPainter->SetParam(Ctx, DETAIL_ATTRIB, m_Detail);
	pPainter->SetParam(Ctx, DISTORTION_ATTRIB, m_Distortion);
	pPainter->SetParam(Ctx, INTENSITY_ATTRIB, m_Intensity);
	pPainter->SetParam(Ctx, LIFETIME_ATTRIB, m_Lifetime);
	pPainter->SetParam(Ctx, PRIMARY_COLOR_ATTRIB, m_PrimaryColor);
	pPainter->SetParam(Ctx, RADIUS_ATTRIB, m_Radius);
	pPainter->SetParam(Ctx, SECONDARY_COLOR_ATTRIB, m_SecondaryColor);
	pPainter->SetParam(Ctx, SPIKE_COUNT_ATTRIB, m_SpikeCount);
	pPainter->SetParam(Ctx, STYLE_ATTRIB, m_Style);

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

ALERROR COrbEffectCreator::OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID)

//	OnEffectCreateFromXML
//
//	Initialize type-specific data

	{
	ALERROR error;

	if (error = m_Animate.InitIdentifierFromXML(Ctx, pDesc->GetAttribute(ANIMATE_ATTRIB), ANIMATION_TABLE))
		return error;

	if (error = m_BlendMode.InitBlendModeFromXML(Ctx, pDesc->GetAttribute(BLEND_MODE_ATTRIB)))
		return error;

	if (error = m_Detail.InitIntegerFromXML(Ctx, pDesc->GetAttribute(DETAIL_ATTRIB)))
		return error;

	if (error = m_Distortion.InitIntegerFromXML(Ctx, pDesc->GetAttribute(DISTORTION_ATTRIB)))
		return error;

	if (error = m_Intensity.InitIntegerFromXML(Ctx, pDesc->GetAttribute(INTENSITY_ATTRIB)))
		return error;

	if (error = m_Lifetime.InitIntegerFromXML(Ctx, pDesc->GetAttribute(LIFETIME_ATTRIB)))
		return error;

	if (error = m_PrimaryColor.InitColorFromXML(Ctx, pDesc->GetAttribute(PRIMARY_COLOR_ATTRIB)))
		return error;

	if (error = m_Radius.InitIntegerFromXML(Ctx, pDesc->GetAttribute(RADIUS_ATTRIB)))
		return error;

	if (error = m_SecondaryColor.InitColorFromXML(Ctx, pDesc->GetAttribute(SECONDARY_COLOR_ATTRIB)))
		return error;

	if (error = m_SpikeCount.InitIntegerFromXML(Ctx, pDesc->GetAttribute(SPIKE_COUNT_ATTRIB)))
		return error;

	if (error = m_Style.InitIdentifierFromXML(Ctx, pDesc->GetAttribute(STYLE_ATTRIB), STYLE_TABLE))
		return error;

	return NOERROR;
	}

ALERROR COrbEffectCreator::OnEffectBindDesign (SDesignLoadCtx &Ctx)

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

//	COrbEffectPainter ----------------------------------------------------------

CExplosionColorizer COrbEffectPainter::m_ExplosionColorizer;

COrbEffectPainter::COrbEffectPainter (CEffectCreator *pCreator) : 
		m_pCreator(pCreator),
		m_iRadius((int)(STD_SECONDS_PER_UPDATE * LIGHT_SECOND / KLICKS_PER_PIXEL)),
		m_iStyle(styleSmooth),
		m_iIntensity(50),
		m_iDetail(25),
		m_iDistortion(0),
		m_SpikeCount(0, 0, 6),
		m_rgbPrimaryColor(CG32bitPixel(255, 255, 255)),
		m_rgbSecondaryColor(CG32bitPixel(128, 128, 128)),
		m_iBlendMode(CGDraw::blendNormal),
		m_iLifetime(0),
		m_iAnimation(animateNone),
		m_pPainter(NULL),
		m_bInitialized(false)

//	COrbEffectCreator constructor

	{
	m_ExplosionColorizer.Init();
	}

COrbEffectPainter::~COrbEffectPainter (void)

//	COrbEffectCreator destructor

	{
	//	Clean up temporaries

	Invalidate();
	}

bool COrbEffectPainter::CalcIntermediates (void)

//	CalcIntermediates
//
//	Computes m_ColorTable. Returns TRUE if we have at least one frame of 
//	animation.

	{
	int i;

	if (!m_bInitialized)
		{
		m_ColorTable.DeleteAll();
		m_ColorTable2.DeleteAll();
		m_FlareDesc.DeleteAll();
		m_TextureFrame.DeleteAll();

		//	Initialized based on animation property

		switch (m_iAnimation)
			{
			//	Expand and fade

			case animateDissipate:
				{
				int iLifetime = Max(1, m_iLifetime);
				Metric rDetail = (m_iDetail / 100.0);

				m_iTextureType = CFractalTextureLibrary::typeExplosion;
				CStepIncrementor Radius(CStepIncrementor::styleSquareRoot, 0.2 * m_iRadius, m_iRadius, iLifetime);
				CStepIncrementor Intensity(CStepIncrementor::styleLinear, m_iIntensity, 0.0, iLifetime);
				CStepIncrementor Detail(CStepIncrementor::styleLinear, rDetail, rDetail / 10.0, iLifetime);
				CStepIncrementor ColorFade(CStepIncrementor::styleLinear, 0.0, 1.0, iLifetime);

				m_ColorTable.InsertEmpty(iLifetime);
				m_FlareDesc.InsertEmpty(iLifetime);
				m_TextureFrame.InsertEmpty(iLifetime);

				if (UsesColorTable2())
					m_ColorTable2.InsertEmpty(iLifetime);

				for (i = 0; i < iLifetime; i++)
					{
					int iRadius = Max(2, (int)Radius.GetAt(i));
					int iIntensity = (int)Intensity.GetAt(i);
					CG32bitPixel rgbColor = CG32bitPixel::Blend(m_rgbPrimaryColor, m_rgbSecondaryColor, ColorFade.GetAt(i));

					CalcSphericalColorTable(m_iStyle, iRadius, iIntensity, rgbColor, m_rgbSecondaryColor, 255, &m_ColorTable[i]);

					m_FlareDesc[i].iLength = iRadius * FLARE_MULITPLE;
					m_FlareDesc[i].iWidth = Max(1, m_FlareDesc[i].iLength / FLARE_WIDTH_FRACTION);

					m_TextureFrame[i] = g_pUniverse->GetFractalTextureLibrary().GetTextureIndex(m_iTextureType, Detail.GetAt(i));

					if (UsesColorTable2())
						CalcSecondaryColorTable(iRadius, iIntensity, 255, &m_ColorTable2[i]);
					}

				break;
				}

			//	Increase radius up to maximum

			case animateExplode:
				{
				int iLifetime = Max(1, m_iLifetime);
				CStepIncrementor Radius(CStepIncrementor::styleSquareRoot, 0.2 * m_iRadius, m_iRadius, iLifetime);
				CStepIncrementor Intensity(CStepIncrementor::styleLinear, m_iIntensity, 0.0, iLifetime);

				Metric rDetail = (m_iDetail / 100.0);

				m_iTextureType = CFractalTextureLibrary::typeExplosion;
				CStepIncrementor Detail(CStepIncrementor::styleLinear, rDetail, rDetail / 10.0, iLifetime);

				int iEndFade = iLifetime / 3;
				int iEndFadeStart = iLifetime - iEndFade;

				m_ColorTable.InsertEmpty(iLifetime);
				m_FlareDesc.InsertEmpty(iLifetime);
				m_TextureFrame.InsertEmpty(iLifetime);

				if (UsesColorTable2())
					m_ColorTable2.InsertEmpty(iLifetime);

				for (i = 0; i < iLifetime; i++)
					{
					int iRadius = Max(2, (int)Radius.GetAt(i));
					int iIntensity = (int)Intensity.GetAt(i);

					BYTE byOpacity;
					if (i > iEndFadeStart)
						{
						Metric rFade = (iEndFade - (i - iEndFadeStart)) / (Metric)iEndFade;
						byOpacity = (BYTE)(255.0 * rFade);
						}
					else
						byOpacity = 255;

					CalcSphericalColorTable(m_iStyle, iRadius, iIntensity, m_rgbPrimaryColor, m_rgbSecondaryColor, byOpacity, &m_ColorTable[i]);

					m_FlareDesc[i].iLength = (byOpacity == 255 ? iIntensity * iRadius * FLARE_MULITPLE / 70 : 0);
					m_FlareDesc[i].iWidth = Max(1, m_FlareDesc[i].iLength / FLARE_WIDTH_FRACTION);

					m_TextureFrame[i] = g_pUniverse->GetFractalTextureLibrary().GetTextureIndex(m_iTextureType, Detail.GetAt(i));

					if (UsesColorTable2())
						CalcSecondaryColorTable(iRadius, iIntensity, byOpacity, &m_ColorTable2[i]);
					}

				break;
				}

			//	Standard fade decreases radius and intensity linearly over the
			//	lifetime of the animation.

			case animateFade:
				{
				int iLifetime = Max(1, m_iLifetime);
				Metric rDetail = (m_iDetail / 100.0);

				m_iTextureType = CFractalTextureLibrary::typeExplosion;
				CStepIncrementor Detail(CStepIncrementor::styleLinear, rDetail, rDetail / 10.0, iLifetime);

				m_ColorTable.InsertEmpty(iLifetime);
				m_FlareDesc.InsertEmpty(iLifetime);
				m_TextureFrame.InsertEmpty(iLifetime);

				if (UsesColorTable2())
					m_ColorTable2.InsertEmpty(iLifetime);

				for (i = 0; i < iLifetime; i++)
					{
					Metric rFade = (iLifetime - i) / (Metric)iLifetime;
					int iRadius = (int)(rFade * m_iRadius);
					int iIntensity = (int)(rFade * m_iIntensity);

					CalcSphericalColorTable(m_iStyle, iRadius, iIntensity, m_rgbPrimaryColor, m_rgbSecondaryColor, 255, &m_ColorTable[i]);

					m_FlareDesc[i].iLength = iRadius * FLARE_MULITPLE;
					m_FlareDesc[i].iWidth = Max(1, m_FlareDesc[i].iLength / FLARE_WIDTH_FRACTION);

					m_TextureFrame[i] = g_pUniverse->GetFractalTextureLibrary().GetTextureIndex(m_iTextureType, Detail.GetAt(i));

					if (UsesColorTable2())
						CalcSecondaryColorTable(iRadius, iIntensity, 255, &m_ColorTable2[i]);
					}

				break;
				}

			//	By default the radius does not change.

			default:
				{
				m_ColorTable.InsertEmpty(1);
				m_FlareDesc.InsertEmpty(1);

				CalcSphericalColorTable(m_iStyle, m_iRadius, m_iIntensity, m_rgbPrimaryColor, m_rgbSecondaryColor, 255, &m_ColorTable[0]);

				m_FlareDesc[0].iLength = m_iRadius * FLARE_MULITPLE;
				m_FlareDesc[0].iWidth = Max(1, m_FlareDesc[0].iLength / FLARE_WIDTH_FRACTION);

				//	For cloud and Fireblast we need a repeating animation

				if (UsesTextures())
					{
					m_iTextureType = CFractalTextureLibrary::typeBoilingClouds;
					int iFrames = g_pUniverse->GetFractalTextureLibrary().GetTextureCount(m_iTextureType);

					m_TextureFrame.InsertEmpty(iFrames);
					for (i = 0; i < iFrames; i++)
						m_TextureFrame[i] = i;
					}

				//	For Fireblast, we only need a single color table

				if (UsesColorTable2())
					{
					m_ColorTable2.InsertEmpty(1);
					CalcSecondaryColorTable(m_iRadius, m_iIntensity, 255, &m_ColorTable2[0]);
					}

				break;
				}
			}

		//	Initialize the proper painter

		switch (m_iStyle)
			{
			case styleCloud:
			case styleFirecloud:
			case styleSmoke:
				m_pPainter = new CCloudCirclePainter(m_iTextureType);
				break;

			case styleDiffraction:
				m_pPainter = new CDiffractionCirclePainter();
				break;

			case styleFireblast:
				m_pPainter = new CFireblastCirclePainter(m_iTextureType, (Metric)m_iDistortion / 100.0);
				break;
			}

		m_bInitialized = true;
		}

	return (m_ColorTable.GetCount() > 0);
	}

void COrbEffectPainter::CalcSecondaryColorTable (int iRadius, int iIntensity, BYTE byOpacity, TArray<CG32bitPixel> *retColorTable)

//	CalcSecondaryColorTable
//
//	Calc smoke for Fireblast

	{
	int i;

	switch (m_iStyle)
		{
		//	For cloud and smoke the secondary table is a pixel table progressing
		//	from primary to secondary colors.

		case styleCloud:
		case styleSmoke:
			{
			CStepIncrementor Color(CStepIncrementor::styleLinear, 0.0, 1.0, 256);
			retColorTable->InsertEmpty(256);
			for (i = 0; i < 256; i++)
				(*retColorTable)[i] = CG32bitPixel::Blend(m_rgbSecondaryColor, m_rgbPrimaryColor, Color.GetAt(i));

			break;
			}

		case styleFireblast:
			{
			CStepIncrementor Intensity(CStepIncrementor::styleLinear, 100, 50, 100);
			int iFade = Min((int)Intensity.GetAt(iIntensity), 100);

			CalcSphericalColorTable(styleSmoke, 
					iRadius, 
					iIntensity, 
					m_rgbSecondaryColor,
					CG32bitPixel::Fade(m_rgbSecondaryColor, CG32bitPixel(0, 0, 0), iFade), 
					byOpacity,
					retColorTable);
			break;
			}

		case styleFirecloud:
			{
			retColorTable->InsertEmpty(256);
			for (i = 0; i < 256; i++)
				(*retColorTable)[i] = m_ExplosionColorizer.GetPixel(i, 256, iIntensity, m_rgbPrimaryColor, m_rgbSecondaryColor);

			break;
			}

		default:
			ASSERT(false);
		}
	}

void COrbEffectPainter::CalcSphericalColorTable (EOrbStyles iStyle, int iRadius, int iIntensity, CG32bitPixel rgbPrimary, CG32bitPixel rgbSecondary, BYTE byOpacity, TArray<CG32bitPixel> *retColorTable)

//	CalcSphericalColorTable
//
//	Calculates the color table given these parameters.

	{
	int i;

	retColorTable->DeleteAll();
	if (iRadius <= 0)
		return;

	//	Allocate tables

	retColorTable->InsertEmpty(iRadius);

	//	Table is based on style

	switch (iStyle)
		{
		case styleSmooth:
			{
			int iFringeMaxRadius = iRadius * iIntensity / 120;
			int iFringeWidth = iFringeMaxRadius / 8;
			int iBlownRadius = iFringeMaxRadius - iFringeWidth;
			int iFadeWidth = iRadius - iFringeMaxRadius;

			//	Initialize table

			for (i = 0; i < iRadius; i++)
				{
				if (i < iBlownRadius)
					(*retColorTable)[i] = CG32bitPixel(255, 255, 255, byOpacity);

				else if (i < iFringeMaxRadius && iFringeWidth > 0)
					{
					int iStep = (i - iBlownRadius);
					DWORD dwOpacity = iStep * byOpacity / iFringeWidth;
					(*retColorTable)[i] = CG32bitPixel::Blend(CG32bitPixel(255, 255, 255), rgbPrimary, (BYTE)dwOpacity);
					}
				else if (iFadeWidth > 0)
					{
					int iStep = (i - iFringeMaxRadius);
					Metric rOpacity = 1.0 - ((Metric)iStep / iFadeWidth);
					rOpacity = (rOpacity * rOpacity) * byOpacity;
					(*retColorTable)[i] = CG32bitPixel(rgbSecondary, (BYTE)(DWORD)rOpacity);
					}
				else
					(*retColorTable)[i] = CG32bitPixel::Null();
				}

			break;
			}

		//	For cloud we only use this color table for the opacity. The actual
		//	color information is in the second table (the pixel table).

		case styleCloud:
		case styleFirecloud:
			{
			CStepIncrementor Opacity(CStepIncrementor::styleSquare, byOpacity, 0.0, iRadius);
			for (i = 0; i < iRadius; i++)
				(*retColorTable)[i] = CG32bitPixel(255, 255, 255, (BYTE)Opacity.GetAt(i));

			break;
			}

		//	We progress from primary color (at the center) to secondary color
		//	at the edge, and from full opacity to transparent.

		case styleDiffraction:
		case styleFlare:
			{
			CG32bitPixel rgbCenter = CG32bitPixel::Fade(rgbPrimary, CG32bitPixel(255, 255, 255), iIntensity);
			CG32bitPixel rgbEdge = rgbSecondary;

			for (i = 0; i < iRadius; i++)
				{
				Metric rFade = (Metric)i / (Metric)iRadius;
				CG32bitPixel rgbColor = CG32bitPixel::Blend(rgbCenter, rgbEdge, rFade);
				BYTE byAlpha = (BYTE)(byOpacity - (i * byOpacity / iRadius));

				(*retColorTable)[i] = CG32bitPixel(rgbColor, byAlpha);
				}
			break;
			}

		case styleFireblast:
			{
			CStepIncrementor Opacity(CStepIncrementor::styleSquare, byOpacity, 0.0, iRadius);

			//	Initialize table

			for (i = 0; i < iRadius; i++)
				(*retColorTable)[i] = CG32bitPixel(m_ExplosionColorizer.GetPixel(i, iRadius, iIntensity, rgbPrimary, rgbSecondary), (BYTE)Opacity.GetAt(i));

			break;
			}

		case styleSmoke:
			{
			CStepIncrementor Opacity(CStepIncrementor::styleOct, byOpacity, 0.0, iRadius);
			CStepIncrementor CenterAdj(CStepIncrementor::styleSquare, (Metric)Max(0, (70 - iIntensity)) / 100.0, 1.0, iRadius);

			for (i = 0; i < iRadius; i++)
				{
				(*retColorTable)[i] = CG32bitPixel(CG32bitPixel::Blend(rgbPrimary, rgbSecondary, (Metric)i / iRadius), (BYTE)(CenterAdj.GetAt(i) * Opacity.GetAt(i)));
				}

			break;
			}
		}
	}

void COrbEffectPainter::CompositeFlareRay (CG32bitImage &Dest, int xCenter, int yCenter, int iLength, int iWidth, int iAngle, int iIntensity, SViewportPaintCtx &Ctx)

//	CompositeFlareRay
//
//	Paints a flare line

	{
	int i;

	//	Compute the line

	CVector vHalf = PolarToVector(iAngle, iLength / 2.0);
	int xOffset = (int)vHalf.GetX();
	int yOffset = (int)vHalf.GetY();

	//	Rasterize the line

	TArray<CGRasterize::SLinePixel> Pixels;
	CGRasterize::Line(Dest, xCenter - xOffset, yCenter + yOffset, xCenter + xOffset, yCenter - yOffset, iWidth, &Pixels);

	//	Now fill the line

	for (i = 0; i < Pixels.GetCount(); i++)
		{
		CGRasterize::SLinePixel &Pixel = Pixels[i];

		if (Pixel.rW < -1.0 || Pixel.rW > 1.0 || Pixel.rV < 0.0 || Pixel.rV > 1.0)
			continue;

		CG32bitPixel rgbValue = CalcFlarePoint(Pixel);

		//	Draw

		*(Pixel.pPos) = CG32bitPixel::Composite(*(Pixel.pPos), rgbValue);
		}
	}

void COrbEffectPainter::CompositeFlares (CG32bitImage &Dest, int xCenter, int yCenter, const SFlareDesc &FlareDesc, SViewportPaintCtx &Ctx)

//	CompositeFlares
//
//	Paints flares

	{
	int i;

	int iFlareCount = m_SpikeCount.Roll() / 2;
	if (iFlareCount <= 0)
		return;

	int iAngle = 360 / iFlareCount;

	for (i = 0; i < iFlareCount; i++)
		CompositeFlareRay(Dest, xCenter, yCenter, FlareDesc.iLength, FlareDesc.iWidth, iAngle * i, m_iIntensity, Ctx);
	}

void COrbEffectPainter::GetParam (const CString &sParam, CEffectParamDesc *retValue)

//	GetParam
//
//	Returns the parameter

	{
	if (strEquals(sParam, ANIMATE_ATTRIB))
		retValue->InitInteger(m_iAnimation);

	else if (strEquals(sParam, BLEND_MODE_ATTRIB))
		retValue->InitInteger(m_iBlendMode);

	else if (strEquals(sParam, DETAIL_ATTRIB))
		retValue->InitInteger(m_iDetail);

	else if (strEquals(sParam, DISTORTION_ATTRIB))
		retValue->InitInteger(m_iDistortion);

	else if (strEquals(sParam, INTENSITY_ATTRIB))
		retValue->InitInteger(m_iIntensity);

	else if (strEquals(sParam, LIFETIME_ATTRIB))
		retValue->InitInteger(m_iLifetime);

	else if (strEquals(sParam, PRIMARY_COLOR_ATTRIB))
		retValue->InitColor(m_rgbPrimaryColor);

	else if (strEquals(sParam, RADIUS_ATTRIB))
		retValue->InitInteger(m_iRadius);

	else if (strEquals(sParam, SECONDARY_COLOR_ATTRIB))
		retValue->InitColor(m_rgbSecondaryColor);
	
	else if (strEquals(sParam, SPIKE_COUNT_ATTRIB))
		retValue->InitDiceRange(m_SpikeCount);
	
	else if (strEquals(sParam, STYLE_ATTRIB))
		retValue->InitInteger(m_iStyle);

	else
		retValue->InitNull();
	}

bool COrbEffectPainter::GetParamList (TArray<CString> *retList) const

//	GetParamList
//
//	Returns a list of value parameter names

	{
	retList->DeleteAll();
	retList->InsertEmpty(11);
	retList->GetAt(0) = ANIMATE_ATTRIB;
	retList->GetAt(1) = BLEND_MODE_ATTRIB;
	retList->GetAt(2) = DETAIL_ATTRIB;
	retList->GetAt(3) = DISTORTION_ATTRIB;
	retList->GetAt(4) = INTENSITY_ATTRIB;
	retList->GetAt(5) = LIFETIME_ATTRIB;
	retList->GetAt(6) = PRIMARY_COLOR_ATTRIB;
	retList->GetAt(7) = RADIUS_ATTRIB;
	retList->GetAt(8) = SECONDARY_COLOR_ATTRIB;
	retList->GetAt(9) = SPIKE_COUNT_ATTRIB;
	retList->GetAt(10) = STYLE_ATTRIB;

	return true;
	}

void COrbEffectPainter::GetRect (RECT *retRect) const

//	GetRect
//
//	Returns the rect bounds of the image

	{
	int iSize = m_iRadius;
	if (HasFlares())
		iSize *= FLARE_MULITPLE / 2;

	retRect->left = -iSize;
	retRect->right = iSize;
	retRect->top = -iSize;
	retRect->bottom = iSize;
	}

void COrbEffectPainter::Invalidate (void)

//	Invalidate
//
//	Free up temporaries

	{
	if (m_pPainter)
		{
		delete m_pPainter;
		m_pPainter = NULL;
		}

	m_ColorTable.DeleteAll();
	m_bInitialized = false;
	}

void COrbEffectPainter::Paint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	Paint
//
//	Paint the effect

	{
	if (!CalcIntermediates())
		return;

	switch (m_iStyle)
		{
		case styleCloud:
		case styleFirecloud:
		case styleSmoke:
			{
			TArray<CG32bitPixel> &Table = m_ColorTable[Ctx.iTick % m_ColorTable.GetCount()];
			m_pPainter->SetParam(CONSTLIT("radiusTable"), Table);
			m_pPainter->SetParam(CONSTLIT("pixelTable"), m_ColorTable2[Ctx.iTick % m_ColorTable2.GetCount()]);

			m_pPainter->Draw(Dest, x, y, Table.GetCount(), m_TextureFrame[Ctx.iTick % m_TextureFrame.GetCount()]);
			break;
			}

		case styleDiffraction:
			{
			TArray<CG32bitPixel> &Table = m_ColorTable[Ctx.iTick % m_ColorTable.GetCount()];
			m_pPainter->SetParam(CONSTLIT("colorTable"), Table);
			if (UsesColorTable2())
				{
				const TArray<CG32bitPixel> &Table2 = m_ColorTable2[Ctx.iTick % m_ColorTable.GetCount()];
				m_pPainter->SetParam(CONSTLIT("colorTable2"), Table2);
				}

			m_pPainter->Draw(Dest, x, y, Table.GetCount(), m_TextureFrame[Ctx.iTick % m_TextureFrame.GetCount()]);
			break;
			}

		case styleFireblast:
			{
			const TArray<CG32bitPixel> &Table = m_ColorTable[Ctx.iTick % m_ColorTable.GetCount()];
			m_pPainter->SetParam(CONSTLIT("explosionTable"), Table);
			m_pPainter->SetParam(CONSTLIT("smokeTable"), m_ColorTable2[Ctx.iTick % m_ColorTable2.GetCount()]);
			m_pPainter->Draw(Dest, x, y, Table.GetCount(), m_TextureFrame[Ctx.iTick % m_TextureFrame.GetCount()]);
			break;
			}

		case styleFlare:
		case styleSmooth:
			{
			TArray<CG32bitPixel> &Table = m_ColorTable[Ctx.iTick % m_ColorTable.GetCount()];
			CGDraw::Circle(Dest, x, y, Table.GetCount(), Table, m_iBlendMode);
			break;
			}
		}

	if (HasFlares())
		{
		SFlareDesc &Flare = m_FlareDesc[Ctx.iTick % m_FlareDesc.GetCount()];
		PaintFlares(Dest, x, y, Flare, Ctx);
		}
	}

void COrbEffectPainter::PaintComposite (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	PaintComposite
//
//	Paint the effect

	{
	if (!CalcIntermediates())
		return;

	switch (m_iStyle)
		{
		case styleSmooth:
		case styleFlare:
			{
			TArray<CG32bitPixel> &Table = m_ColorTable[Ctx.iTick % m_ColorTable.GetCount()];
			CGComposite::Circle(Dest, x, y, Table.GetCount(), Table);
			break;
			}

		case styleCloud:
			break;
		}

	if (HasFlares())
		{
		SFlareDesc &Flare = m_FlareDesc[Ctx.iTick % m_FlareDesc.GetCount()];
		CompositeFlares(Dest, x, y, Flare, Ctx);
		}
	}

void COrbEffectPainter::PaintFlareRay (CG32bitImage &Dest, int xCenter, int yCenter, int iLength, int iWidth, int iAngle, int iIntensity, SViewportPaintCtx &Ctx)

//	PaintFlareRay
//
//	Paints a flare line

	{
	int i;

	if (iLength <= 0 || iWidth <= 0)
		return;

	//	Compute the line

	CVector vHalf = PolarToVector(iAngle, iLength / 2.0);
	int xOffset = (int)vHalf.GetX();
	int yOffset = (int)vHalf.GetY();

	//	Rasterize the line

	TArray<CGRasterize::SLinePixel> Pixels;
	CGRasterize::Line(Dest, xCenter - xOffset, yCenter + yOffset, xCenter + xOffset, yCenter - yOffset, iWidth, &Pixels);

	//	Now fill the line

	for (i = 0; i < Pixels.GetCount(); i++)
		{
		CGRasterize::SLinePixel &Pixel = Pixels[i];

		if (Pixel.rW < -1.0 || Pixel.rW > 1.0 || Pixel.rV < 0.0 || Pixel.rV > 1.0)
			continue;

		//	Compute the brightness at this pixel

		CG32bitPixel rgbValue = CalcFlarePoint(Pixel);
		BYTE byOpacity = rgbValue.GetAlpha();

		//	Draw

		if (byOpacity == 0)
			;
		else if (byOpacity == 0xff)
			*(Pixel.pPos) = rgbValue;
		else
			*(Pixel.pPos) = CG32bitPixel::Blend(*(Pixel.pPos), rgbValue, byOpacity);
		}
	}

void COrbEffectPainter::PaintFlares (CG32bitImage &Dest, int xCenter, int yCenter, const SFlareDesc &FlareDesc, SViewportPaintCtx &Ctx)

//	PaintFlares
//
//	Paints flares

	{
	int i;

	int iFlareCount = m_SpikeCount.Roll() / 2;
	if (iFlareCount <= 0)
		return;

	int iAngle = 360 / iFlareCount;

	for (i = 0; i < iFlareCount; i++)
		PaintFlareRay(Dest, xCenter, yCenter, FlareDesc.iLength, FlareDesc.iWidth, iAngle * i, m_iIntensity, Ctx);
	}

bool COrbEffectPainter::PointInImage (int x, int y, int iTick, int iVariant, int iRotation) const

//	PointInImage
//
//	Returns TRUE if the given point is in the image

	{
	//	We only intersect if we are inside a box around the center of 1/2 the
	//	width (since a lot of the width is taken up by glow effects).

	int iSize = (m_iRadius / 2);
	return (Absolute(x) <= iSize && Absolute(y) <= iSize);
	}

void COrbEffectPainter::OnSetParam (CCreatePainterCtx &Ctx, const CString &sParam, const CEffectParamDesc &Value)

//	SetParam
//
//	Sets parameters

	{
	if (strEquals(sParam, ANIMATE_ATTRIB))
		m_iAnimation = (EAnimationTypes)Value.EvalIdentifier(Ctx, ANIMATION_TABLE, animateMax, animateNone);

	else if (strEquals(sParam, BLEND_MODE_ATTRIB))
		m_iBlendMode = Value.EvalBlendMode(Ctx);

	else if (strEquals(sParam, DETAIL_ATTRIB))
		m_iDetail = Value.EvalIntegerBounded(Ctx, 0, 100, 25);

	else if (strEquals(sParam, DISTORTION_ATTRIB))
		m_iDistortion = Value.EvalIntegerBounded(Ctx, 0, 100, 0);

	else if (strEquals(sParam, INTENSITY_ATTRIB))
		m_iIntensity = Value.EvalIntegerBounded(Ctx, 0, 100, 50);

	else if (strEquals(sParam, LIFETIME_ATTRIB))
		m_iLifetime = Value.EvalIntegerBounded(Ctx, 0, -1, 0);

	else if (strEquals(sParam, PRIMARY_COLOR_ATTRIB))
		m_rgbPrimaryColor = Value.EvalColor(Ctx);

	else if (strEquals(sParam, RADIUS_ATTRIB))
		m_iRadius = Value.EvalIntegerBounded(Ctx, 1, -1, (int)(STD_SECONDS_PER_UPDATE * LIGHT_SECOND / KLICKS_PER_PIXEL));

	else if (strEquals(sParam, SECONDARY_COLOR_ATTRIB))
		m_rgbSecondaryColor = Value.EvalColor(Ctx);
	
	else if (strEquals(sParam, SPIKE_COUNT_ATTRIB))
		m_SpikeCount = Value.EvalDiceRange(Ctx, 6);
	
	else if (strEquals(sParam, STYLE_ATTRIB))
		m_iStyle = (EOrbStyles)Value.EvalIdentifier(Ctx, STYLE_TABLE, styleMax, styleSmooth);
	}
