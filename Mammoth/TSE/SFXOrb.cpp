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

constexpr int DEFAULT_FLARE_COUNT =			5;
constexpr int FLARE_MULITPLE =				4;
constexpr int LIGHTNING_MULITPLE =			2;
constexpr int FLARE_WIDTH_FRACTION =		32;
constexpr Metric BLOOM_FACTOR =				1.2;
constexpr int FLARE_ANGLE =					15;
constexpr Metric SHELL_EDGE_WIDTH_RATIO =	0.05;

class COrbEffectPainter : public IEffectPainter
	{
	public:
		COrbEffectPainter (CEffectCreator *pCreator);
		~COrbEffectPainter (void);

		void CleanUp (void);

		//	IEffectPainter virtuals

		virtual bool CanPaintComposite (void) override { return true; }
		virtual CEffectCreator *GetCreator (void) override { return m_pCreator; }
		virtual int GetLifetime (void) override { return m_iLifetime; }
		virtual bool GetParam (const CString &sParam, CEffectParamDesc *retValue) const override;
		virtual bool GetParamList (TArray<CString> *retList) const override;
		virtual void GetRect (RECT *retRect) const override;
		virtual void Paint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual void PaintComposite (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual bool PointInImage (int x, int y, int iTick, int iVariant = 0, int iRotation = 0) const override;

	protected:
		virtual bool OnSetParam (CCreatePainterCtx &Ctx, const CString &sParam, const CEffectParamDesc &Value) override;

	private:
		enum EAnimationTypes
			{
			animateNone =			0,

			animateDissipate =		1,
			animateExplode =		2,
			animateFade =			3,
			animateFlicker =		4,

			animateMax =			4,
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
			styleBlackHole =		8,
			styleLightning =		9,
			styleShell =			10,
			styleCloudshell =		11,

			styleMax =				11,
			};

		struct SFlareDesc
			{
			int iLength;
			int iWidth;
			};

		void CalcAnimationIntermediates (void);
		void CalcBlackHoleColorTable (int iRadius, int iIntensity, CG32bitPixel rgbPrimary, CG32bitPixel rgbSecondary, BYTE byOpacity, TArray<CG32bitPixel> *retColorTable) const;
		bool CalcIntermediates (void);
		void CalcSecondaryColorTable (int iRadius, int iIntensity, BYTE byOpacity, TArray<CG32bitPixel> *retColorTable);
		void CalcShellColorTable (int iRadius, int iIntensity, CG32bitPixel rgbPrimary, CG32bitPixel rgbSecondary, BYTE byOpacity, TArray<CG32bitPixel> *retColorTable) const;
		void CalcShellOpacity (int iRadius, int iShellMaxRadius, int iIntensity, BYTE byOpacity, TArray<BYTE> &retOpacity) const;
		void CalcSphericalColorTable (EOrbStyles iStyle, int iRadius, int iIntensity, CG32bitPixel rgbPrimary, CG32bitPixel rgbSecondary, BYTE byOpacity, TArray<CG32bitPixel> *retColorTable);
		void CalcStyleIntermediates (void);
		void CompositeFlareRay (CG32bitImage &Dest, int xCenter, int yCenter, int iLength, int iWidth, int iAngle, int iIntensity, SViewportPaintCtx &Ctx);
		void CompositeFlares (CG32bitImage &Dest, int xCenter, int yCenter, const SFlareDesc &FlareDesc, SViewportPaintCtx &Ctx);
		inline bool HasFlares (void) const { return (m_iStyle == styleFlare || m_iStyle == styleDiffraction || m_iStyle == styleFireblast || !m_SpikeCount.IsEmpty()); }
		void Invalidate (void);
		void PaintFlareRay (CG32bitImage &Dest, int xCenter, int yCenter, int iLength, int iWidth, int iAngle, int iIntensity, SViewportPaintCtx &Ctx);
		void PaintFlares (CG32bitImage &Dest, int xCenter, int yCenter, const SFlareDesc &FlareDesc, SViewportPaintCtx &Ctx);
		void PaintLightning (CG32bitImage &Dest, int xCenter, int yCenter, const SFlareDesc &FlareDesc, SViewportPaintCtx &Ctx);
		inline bool UsesColorTable2 (void) const { return (m_iStyle == styleCloud || m_iStyle == styleFireblast || m_iStyle == styleFirecloud || m_iStyle == styleSmoke || m_iStyle == styleCloudshell); }
		inline bool UsesTextures (void) const { return (m_iStyle == styleCloud || m_iStyle == styleFireblast || m_iStyle == styleFirecloud || m_iStyle == styleSmoke || m_iStyle == styleCloudshell); }

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
		"explode",
		"fade",
		"flicker",

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
		"blackhole",
		"lightning",
		"shell",
		"cloudshell",

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

	if (m_pSingleton && Ctx.CanCreateSingleton())
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

	if (GetInstance() == instGame
			&& Ctx.CanCreateSingleton())
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
		m_SpikeCount(0, 0, 0),
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

	CleanUp();
	}

void COrbEffectPainter::CalcAnimationIntermediates (void)

//	CalcAnimationIntermediates
//
//	Calculate cached intermediates based on animation type.
//	NOTE: We assume all cached tables have been cleaned up (are empty).

	{
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

			for (int i = 0; i < iLifetime; i++)
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
			CStepIncrementor Heat(CStepIncrementor::styleSquare, 100.0, 0.0, iLifetime);

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

			for (int i = 0; i < iLifetime; i++)
				{
				int iRadius = Max(2, (int)Radius.GetAt(i));
				int iIntensity = (int)Intensity.GetAt(i);
				int iHeat = (int)Heat.GetAt(i);

				BYTE byOpacity;
				if (i > iEndFadeStart)
					{
					Metric rFade = (iEndFade - (i - iEndFadeStart)) / (Metric)iEndFade;
					byOpacity = (BYTE)(255.0 * rFade);
					}
				else
					byOpacity = 255;

				CalcSphericalColorTable(m_iStyle, iRadius, iHeat, m_rgbPrimaryColor, m_rgbSecondaryColor, byOpacity, &m_ColorTable[i]);

				m_FlareDesc[i].iLength = (iIntensity > 60 ? iIntensity * iRadius * FLARE_MULITPLE / 70 : 0);
				m_FlareDesc[i].iWidth = Max(1, m_FlareDesc[i].iLength / FLARE_WIDTH_FRACTION);

				m_TextureFrame[i] = g_pUniverse->GetFractalTextureLibrary().GetTextureIndex(m_iTextureType, Detail.GetAt(i));

				if (UsesColorTable2())
					CalcSecondaryColorTable(iRadius, iHeat, byOpacity, &m_ColorTable2[i]);
				}

			break;
			}

		//	Standard fade decreases radius and intensity linearly over the
		//	lifetime of the animation.

		case animateFade:
			{
			int iLifetime = Max(1, m_iLifetime);
			Metric rDetail = (m_iDetail / 100.0);

			CStepIncrementor Detail(CStepIncrementor::styleLinear, rDetail, rDetail / 10.0, iLifetime);

			m_ColorTable.InsertEmpty(iLifetime);
			m_FlareDesc.InsertEmpty(iLifetime);

			if (UsesColorTable2())
				m_ColorTable2.InsertEmpty(iLifetime);

			//	For cloud and Fireblast we need a repeating animation

			if (UsesTextures())
				{
				m_iTextureType = CFractalTextureLibrary::typeBoilingClouds;
				int iFrames = g_pUniverse->GetFractalTextureLibrary().GetTextureCount(m_iTextureType);

				m_TextureFrame.InsertEmpty(iFrames);
				for (int i = 0; i < iFrames; i++)
					m_TextureFrame[i] = i;
				}
			else
				m_TextureFrame.InsertEmpty(iLifetime);

			for (int i = 0; i < iLifetime; i++)
				{
				Metric rFade = (iLifetime - i) / (Metric)iLifetime;
				int iRadius = (int)(rFade * m_iRadius);
				int iIntensity = (int)(rFade * m_iIntensity);

				CalcSphericalColorTable(m_iStyle, iRadius, iIntensity, m_rgbPrimaryColor, m_rgbSecondaryColor, 255, &m_ColorTable[i]);

				m_FlareDesc[i].iLength = iRadius * FLARE_MULITPLE;
				m_FlareDesc[i].iWidth = Max(1, m_FlareDesc[i].iLength / FLARE_WIDTH_FRACTION);

				if (!UsesTextures())
					m_TextureFrame[i] = g_pUniverse->GetFractalTextureLibrary().GetTextureIndex(m_iTextureType, Detail.GetAt(i));

				if (UsesColorTable2())
					CalcSecondaryColorTable(iRadius, iIntensity, 255, &m_ColorTable2[i]);
				}

			break;
			}

		//	Flicker randomly

		case animateFlicker:
			{
			int iLifetime = (m_iLifetime < 1 ? 120 : m_iLifetime);

			m_ColorTable.InsertEmpty(iLifetime);
			m_FlareDesc.InsertEmpty(iLifetime);

			if (UsesColorTable2())
				m_ColorTable2.InsertEmpty(iLifetime);

			//	For cloud and Fireblast we need a repeating animation

			if (UsesTextures())
				{
				m_iTextureType = CFractalTextureLibrary::typeBoilingClouds;
				int iFrames = g_pUniverse->GetFractalTextureLibrary().GetTextureCount(m_iTextureType);

				m_TextureFrame.InsertEmpty(iFrames);
				for (int i = 0; i < iFrames; i++)
					m_TextureFrame[i] = i;
				}

			for (int i = 0; i < iLifetime; i++)
				{
				Metric rFlicker = Max(0.5, Min(1.0 + (0.25 * mathRandomGaussian()), 2.0));
				int iRadius = Max(2, (int)(rFlicker * m_iRadius));
				int iIntensity = (int)(rFlicker * m_iIntensity);

				CalcSphericalColorTable(m_iStyle, iRadius, iIntensity, m_rgbPrimaryColor, m_rgbSecondaryColor, 255, &m_ColorTable[i]);

				m_FlareDesc[i].iLength = iRadius * FLARE_MULITPLE;
				m_FlareDesc[i].iWidth = Max(1, m_FlareDesc[i].iLength / FLARE_WIDTH_FRACTION);

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

			if (m_iStyle == styleLightning)
				m_FlareDesc[0].iLength = m_iRadius * LIGHTNING_MULITPLE;
			else
				m_FlareDesc[0].iLength = m_iRadius * FLARE_MULITPLE;
			m_FlareDesc[0].iWidth = Max(1, m_FlareDesc[0].iLength / FLARE_WIDTH_FRACTION);

			//	For cloud and Fireblast we need a repeating animation

			if (UsesTextures())
				{
				m_iTextureType = CFractalTextureLibrary::typeBoilingClouds;
				int iFrames = g_pUniverse->GetFractalTextureLibrary().GetTextureCount(m_iTextureType);

				m_TextureFrame.InsertEmpty(iFrames);
				for (int i = 0; i < iFrames; i++)
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
	}

void COrbEffectPainter::CalcBlackHoleColorTable (int iRadius, int iIntensity, CG32bitPixel rgbPrimary, CG32bitPixel rgbSecondary, BYTE byOpacity, TArray<CG32bitPixel> *retColorTable) const

//	CalcBlackHoleColorTable
//
//	Calculates color table for black hole style.

	{
	int i;

	int iFringeMaxRadius = iRadius * iIntensity / 120;
	int iFringeWidth = iFringeMaxRadius / 8;
	int iBlownRadius = iFringeMaxRadius - iFringeWidth;
	int iFadeWidth = iRadius - iFringeMaxRadius;

	if (retColorTable->GetCount() < iRadius)
		retColorTable->InsertEmpty(iRadius - retColorTable->GetCount());

	//	Initialize table

	for (i = 0; i < iRadius; i++)
		{
		if (i < iBlownRadius)
			(*retColorTable)[i] = CG32bitPixel(0, 0, 0, byOpacity);

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
	}

bool COrbEffectPainter::CalcIntermediates (void)

//	CalcIntermediates
//
//	Computes m_ColorTable. Returns TRUE if we have at least one frame of 
//	animation.

	{
	if (!m_bInitialized)
		{
		CleanUp();

		CalcAnimationIntermediates();
		CalcStyleIntermediates();

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
		case styleCloudshell:
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
			CStepIncrementor Intensity(CStepIncrementor::styleSquare, 100, 0, 100);
			int iFade = Min((int)Intensity.GetAt(iIntensity), 100);

			CG32bitPixel rgbCenter = CG32bitPixel::Fade(m_rgbPrimaryColor, m_rgbSecondaryColor, iFade);
			CG32bitPixel rgbEdge = CG32bitPixel::Fade(m_rgbSecondaryColor, CG32bitPixel(0, 0, 0), iFade);

			CStepIncrementor Opacity(CStepIncrementor::styleLinear, byOpacity, 0.0, iRadius);

			CStepIncrementor Radius(CStepIncrementor::styleOctRoot, 0.0, 1.0, iRadius);
			retColorTable->InsertEmpty(iRadius);
			for (i = 0; i < iRadius; i++)
				(*retColorTable)[i] = CG32bitPixel(CG32bitPixel::Blend(rgbCenter, rgbEdge, Radius.GetAt(i)), (BYTE)Opacity.GetAt(i));

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

void COrbEffectPainter::CalcShellColorTable (int iRadius, int iIntensity, CG32bitPixel rgbPrimary, CG32bitPixel rgbSecondary, BYTE byOpacity, TArray<CG32bitPixel> *retColorTable) const

//	CalcShellColorTable
//
//	Calculates a shell.

	{
	ASSERT(iRadius >= 0);

	if (retColorTable->GetCount() < iRadius)
		retColorTable->InsertEmpty(iRadius - retColorTable->GetCount());

	int iEdgeWidth = mathRound(iRadius * SHELL_EDGE_WIDTH_RATIO);
	int iShellMaxRadius = iRadius - iEdgeWidth;
	int iHoleRadius = iShellMaxRadius * iIntensity / 120;

	TArray<BYTE> OpacityRamp;
	CalcShellOpacity(iRadius, iShellMaxRadius, iIntensity, byOpacity, OpacityRamp);

	//	Initialize table

	for (int i = 0; i < iRadius; i++)
		{
		if (i < iHoleRadius)
			(*retColorTable)[i] = CG32bitPixel(rgbSecondary, OpacityRamp[i]);

		else if (i < iShellMaxRadius)
			{
			int iStep = (i - iHoleRadius);
			DWORD dwBlend = 255 * iStep / (iShellMaxRadius - iHoleRadius);
			(*retColorTable)[i] = CG32bitPixel(CG32bitPixel::Blend(rgbSecondary, rgbPrimary, (BYTE)dwBlend), OpacityRamp[i]);
			}
		else
			(*retColorTable)[i] = CG32bitPixel(rgbPrimary, OpacityRamp[i]);
		}
	}

void COrbEffectPainter::CalcShellOpacity (int iRadius, int iShellMaxRadius, int iIntensity, BYTE byOpacity, TArray<BYTE> &retOpacity) const

//	CalcShellOpacity
//
//	Computes the opacity ramp by radius for a shell-type effect.

	{
	ASSERT(iRadius >= 0);
	ASSERT(iShellMaxRadius >= 0);

	iIntensity = Max(0, Min(iIntensity, 100));
	iShellMaxRadius = Min(iShellMaxRadius, iRadius);
	int iEdgeWidth = iRadius - iShellMaxRadius;

	if (retOpacity.GetCount() < iRadius)
		retOpacity.InsertEmpty(iRadius - retOpacity.GetCount());

	CStepIncrementor MaxIntensity(CStepIncrementor::styleOct, 0.0, 1.0, iShellMaxRadius);
	CStepIncrementor MinIntensity(CStepIncrementor::styleLinear, 0.0, 1.0, iShellMaxRadius);
	Metric rMaxIntensityK = iIntensity / 100.0;
	Metric rMinIntensityK = (100 - iIntensity) / 100.0;

	for (int i = 0; i < iShellMaxRadius; i++)
		retOpacity[i] = (BYTE)(byOpacity * ((MaxIntensity.GetAt(i) * rMaxIntensityK) + (MinIntensity.GetAt(i) * rMinIntensityK)));

	CStepIncrementor Fade(CStepIncrementor::styleSquare, 1.0, 0.0, iEdgeWidth + 1);
	for (int i = 0; i < iEdgeWidth; i++)
		retOpacity[iShellMaxRadius + i] = (BYTE)(byOpacity * Fade.GetAt(1 + i));
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
		case styleLightning:
			CPaintHelper::CalcSmoothColorTable(iRadius, iIntensity, rgbPrimary, rgbSecondary, byOpacity, retColorTable);
			break;

		case styleBlackHole:
			CalcBlackHoleColorTable(iRadius, iIntensity, rgbPrimary, rgbSecondary, byOpacity, retColorTable);
			break;

		case styleShell:
			CalcShellColorTable(iRadius, iIntensity, rgbPrimary, rgbSecondary, byOpacity, retColorTable);
			break;

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

		case styleCloudshell:
			{
			int iEdgeWidth = mathRound(iRadius * SHELL_EDGE_WIDTH_RATIO);
			int iShellMaxRadius = iRadius - iEdgeWidth;

			TArray<BYTE> OpacityRamp;
			CalcShellOpacity(iRadius, iShellMaxRadius, iIntensity, byOpacity, OpacityRamp);

			for (int i = 0; i < iRadius; i++)
				(*retColorTable)[i] = CG32bitPixel(255, 255, 255, OpacityRamp[i]);

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
			const int BLOWN_INTENSITY = 95;
			const int BLOWN_SIZE = 100 - BLOWN_INTENSITY;
			const int FIRE_INTENSITY = 80;
			const int FIRE_SIZE = BLOWN_INTENSITY - FIRE_INTENSITY;
			const int GLOW_INTENSITY = 25;
			const int GLOW_SIZE = FIRE_INTENSITY - GLOW_INTENSITY;
			const int FADE_INTENSITY = 0;
			const int FADE_SIZE = GLOW_INTENSITY - FADE_INTENSITY;
			CStepIncrementor Opacity(CStepIncrementor::styleOct, byOpacity, 0.0, iRadius);
			CStepIncrementor Radius(CStepIncrementor::styleQuad, 0.0, 1.0, iRadius);

			CG32bitPixel rgbCenter;
			CG32bitPixel rgbEdge;
			if (iIntensity > BLOWN_INTENSITY)
				{
				CStepIncrementor Intensity(CStepIncrementor::styleOct, 100, 0, BLOWN_SIZE);
				int iFade = Min((int)Intensity.GetAt(iIntensity - BLOWN_INTENSITY), 100);

				rgbCenter = CG32bitPixel::Fade(CG32bitPixel(255, 255, 255), m_rgbPrimaryColor, iFade);
				rgbEdge = CG32bitPixel::Fade(CG32bitPixel(255, 255, 255), m_rgbPrimaryColor, iFade);
				}
			else if (iIntensity > FIRE_INTENSITY)
				{
				CStepIncrementor Intensity(CStepIncrementor::styleOct, 100, 0, FIRE_SIZE);
				int iFade = Min((int)Intensity.GetAt(iIntensity - FIRE_INTENSITY), 100);

				rgbCenter = m_rgbPrimaryColor;
				rgbEdge = CG32bitPixel::Fade(m_rgbPrimaryColor, m_rgbSecondaryColor, iFade);
				}
			else if (iIntensity > GLOW_INTENSITY)
				{
				CStepIncrementor Intensity(CStepIncrementor::styleOct, 100, 0, GLOW_SIZE);
				int iFade = Min((int)Intensity.GetAt(iIntensity - GLOW_INTENSITY), 100);

				rgbCenter = CG32bitPixel::Fade(m_rgbPrimaryColor, m_rgbSecondaryColor, iFade);
				rgbEdge = m_rgbSecondaryColor;
				}
			else
				{
				CStepIncrementor Intensity(CStepIncrementor::styleOct, 100, 0, FADE_SIZE);
				int iFade = Min((int)Intensity.GetAt(iIntensity - FADE_INTENSITY), 100);

				rgbCenter = m_rgbSecondaryColor;
				rgbEdge = CG32bitPixel::Fade(m_rgbSecondaryColor, CG32bitPixel(0, 0, 0), iFade);
				}

			for (i = 0; i < iRadius; i++)
				(*retColorTable)[i] = CG32bitPixel(CG32bitPixel::Blend(rgbCenter, rgbEdge, Radius.GetAt(i)), (BYTE)Opacity.GetAt(i));

			break;
			}

		case styleSmoke:
			{
			CStepIncrementor Opacity(CStepIncrementor::styleQuad, byOpacity, 0.0, iRadius);
			CStepIncrementor CenterAdj(CStepIncrementor::styleSquare, (Metric)Max(0, (70 - iIntensity)) / 100.0, 1.0, iRadius);

			for (i = 0; i < iRadius; i++)
				{
				(*retColorTable)[i] = CG32bitPixel(CG32bitPixel::Blend(rgbPrimary, rgbSecondary, (Metric)i / iRadius), (BYTE)(CenterAdj.GetAt(i) * Opacity.GetAt(i)));
				}

			break;
			}
		}
	}

void COrbEffectPainter::CalcStyleIntermediates (void)

//	CalcStyleIntermediates
//
//	Calculate cached values based on style.
//	NOTE: We assume m_pPainter is NULL (has been cleaned up).

	{
	ASSERT(m_pPainter == NULL);

	switch (m_iStyle)
		{
		case styleCloud:
		case styleCloudshell:
		case styleFirecloud:
		case styleSmoke:
			{
			switch (m_iBlendMode)
				{
				case CGDraw::blendNormal:
					m_pPainter = new CCloudCirclePainter<CGBlendBlend>(m_iTextureType);
					break;

				case CGDraw::blendHardLight:
					m_pPainter = new CCloudCirclePainter<CGBlendHardLight>(m_iTextureType);
					break;

				case CGDraw::blendScreen:
					m_pPainter = new CCloudCirclePainter<CGBlendScreen>(m_iTextureType);
					break;

				case CGDraw::blendCompositeNormal:
					m_pPainter = new CCloudCirclePainter<CGBlendComposite>(m_iTextureType);
					break;
				}
			break;
			}

		case styleDiffraction:
			{
			switch (m_iBlendMode)
				{
				case CGDraw::blendNormal:
					m_pPainter = new CDiffractionCirclePainter<CGBlendBlend>();
					break;

				case CGDraw::blendHardLight:
					m_pPainter = new CDiffractionCirclePainter<CGBlendHardLight>();
					break;

				case CGDraw::blendScreen:
					m_pPainter = new CDiffractionCirclePainter<CGBlendScreen>();
					break;

				case CGDraw::blendCompositeNormal:
					m_pPainter = new CDiffractionCirclePainter<CGBlendComposite>();
					break;
				}
			break;
			}

		case styleFireblast:
			{
			switch (m_iBlendMode)
				{
				case CGDraw::blendNormal:
					m_pPainter = new CFireblastCirclePainter<CGBlendBlend>(m_iTextureType, (Metric)m_iDistortion / 100.0);
					break;

				case CGDraw::blendHardLight:
					m_pPainter = new CFireblastCirclePainter<CGBlendHardLight>(m_iTextureType, (Metric)m_iDistortion / 100.0);
					break;

				case CGDraw::blendScreen:
					m_pPainter = new CFireblastCirclePainter<CGBlendScreen>(m_iTextureType, (Metric)m_iDistortion / 100.0);
					break;

				case CGDraw::blendCompositeNormal:
					m_pPainter = new CFireblastCirclePainter<CGBlendComposite>(m_iTextureType, (Metric)m_iDistortion / 100.0);
					break;
				}
			break;
			}
		}
	}

void COrbEffectPainter::CleanUp (void)

//	CleanUp
//
//	Clean up all structures

	{
	if (m_pPainter)
		{
		delete m_pPainter;
		m_pPainter = NULL;
		}

	m_iTextureType = CFractalTextureLibrary::typeNone;
	m_ColorTable.DeleteAll();
	m_ColorTable2.DeleteAll();
	m_FlareDesc.DeleteAll();
	m_TextureFrame.DeleteAll();
	}

void COrbEffectPainter::CompositeFlareRay (CG32bitImage &Dest, int xCenter, int yCenter, int iLength, int iWidth, int iAngle, int iIntensity, SViewportPaintCtx &Ctx)

//	CompositeFlareRay
//
//	Paints a flare line

	{
	//	Compute the line

	CVector vHalf = PolarToVector(iAngle, iLength / 2.0);
	int xOffset = (int)vHalf.GetX();
	int yOffset = (int)vHalf.GetY();

    //  Paint the line

    CFlareRayRasterizer<CGBlendComposite> Flare;
    Flare.Draw(Dest, xCenter - xOffset, yCenter + yOffset, xCenter + xOffset, yCenter - yOffset, iWidth);
	}

void COrbEffectPainter::CompositeFlares (CG32bitImage &Dest, int xCenter, int yCenter, const SFlareDesc &FlareDesc, SViewportPaintCtx &Ctx)

//	CompositeFlares
//
//	Paints flares

	{
	int i;

	int iFlareCount = m_SpikeCount.Roll() / 2;
	if (iFlareCount <= 0)
		iFlareCount = DEFAULT_FLARE_COUNT;

	int iAngle = 360 / iFlareCount;

	for (i = 0; i < iFlareCount; i++)
		CompositeFlareRay(Dest, xCenter, yCenter, FlareDesc.iLength, FlareDesc.iWidth, AngleMod(FLARE_ANGLE + (iAngle * i)), m_iIntensity, Ctx);
	}

bool COrbEffectPainter::GetParam (const CString &sParam, CEffectParamDesc *retValue) const

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
		return false;

	return true;
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
	m_bInitialized = false;
	}

void COrbEffectPainter::Paint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	Paint
//
//	Paint the effect

	{
	if (!CalcIntermediates())
		return;

	//	Sometimes we start on a random animation frame

	int iTick;
	switch (m_iAnimation)
		{
		case animateFlicker:
			iTick = Ctx.iDestiny + Ctx.iTick;
			break;

		default:
			iTick = Ctx.iTick;
			break;
		}

	//	Paint flares/spikes/lightning

	if (m_iStyle == styleLightning)
		{
		SFlareDesc &Flare = m_FlareDesc[iTick % m_FlareDesc.GetCount()];
		PaintLightning(Dest, x, y, Flare, Ctx);
		}

	//	Paint

	switch (m_iStyle)
		{
		case styleCloud:
		case styleCloudshell:
		case styleFirecloud:
		case styleSmoke:
			{
			TArray<CG32bitPixel> &Table = m_ColorTable[iTick % m_ColorTable.GetCount()];
			m_pPainter->SetParam(CONSTLIT("radiusTable"), Table);
			m_pPainter->SetParam(CONSTLIT("pixelTable"), m_ColorTable2[iTick % m_ColorTable2.GetCount()]);

			m_pPainter->Draw(Dest, x, y, Table.GetCount(), m_TextureFrame[iTick % m_TextureFrame.GetCount()]);
			break;
			}

		case styleDiffraction:
			{
			TArray<CG32bitPixel> &Table = m_ColorTable[iTick % m_ColorTable.GetCount()];
			m_pPainter->SetParam(CONSTLIT("colorTable"), Table);
			if (UsesColorTable2())
				{
				const TArray<CG32bitPixel> &Table2 = m_ColorTable2[iTick % m_ColorTable.GetCount()];
				m_pPainter->SetParam(CONSTLIT("colorTable2"), Table2);
				}

			m_pPainter->Draw(Dest, x, y, Table.GetCount(), 0);
			break;
			}

		case styleFireblast:
			{
			const TArray<CG32bitPixel> &Table = m_ColorTable[iTick % m_ColorTable.GetCount()];
			m_pPainter->SetParam(CONSTLIT("explosionTable"), Table);
			m_pPainter->SetParam(CONSTLIT("smokeTable"), m_ColorTable2[iTick % m_ColorTable2.GetCount()]);
			m_pPainter->Draw(Dest, x, y, Table.GetCount(), m_TextureFrame[iTick % m_TextureFrame.GetCount()]);
			break;
			}

		case styleBlackHole:
		case styleFlare:
		case styleShell:
		case styleSmooth:
		case styleLightning:
			{
			TArray<CG32bitPixel> &Table = m_ColorTable[iTick % m_ColorTable.GetCount()];
			CGDraw::Circle(Dest, x, y, Table.GetCount(), Table, m_iBlendMode);
			break;
			}
		}

	//	Paint flares/spikes/lightning

	if (m_iStyle == styleLightning)
		NULL;
	else if (HasFlares())
		{
		SFlareDesc &Flare = m_FlareDesc[iTick % m_FlareDesc.GetCount()];
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
		case styleBlackHole:
		case styleFlare:
		case styleShell:
		case styleSmooth:
		case styleLightning:
			{
			TArray<CG32bitPixel> &Table = m_ColorTable[Ctx.iTick % m_ColorTable.GetCount()];
			CGComposite::Circle(Dest, x, y, Table.GetCount(), Table);
			break;
			}

		case styleCloud:
		case styleCloudshell:
			break;
		}

	if (m_iStyle == styleLightning)
		{
		//	LATER
		}
	else if (HasFlares())
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
	if (iLength <= 0 || iWidth <= 0)
		return;

	//	Compute the line

	CVector vHalf = PolarToVector(iAngle, iLength / 2.0);
	int xOffset = (int)vHalf.GetX();
	int yOffset = (int)vHalf.GetY();

    //  Paint the line

    CFlareRayRasterizer<CGBlendBlend> Flare;
    Flare.Draw(Dest, xCenter - xOffset, yCenter + yOffset, xCenter + xOffset, yCenter - yOffset, iWidth);
	}

void COrbEffectPainter::PaintFlares (CG32bitImage &Dest, int xCenter, int yCenter, const SFlareDesc &FlareDesc, SViewportPaintCtx &Ctx)

//	PaintFlares
//
//	Paints flares

	{
	int i;

	int iFlareCount = m_SpikeCount.Roll() / 2;
	if (iFlareCount <= 0)
		iFlareCount = DEFAULT_FLARE_COUNT;

	int iAngle = 360 / iFlareCount;

	for (i = 0; i < iFlareCount; i++)
		PaintFlareRay(Dest, xCenter, yCenter, FlareDesc.iLength, FlareDesc.iWidth, AngleMod(FLARE_ANGLE + (iAngle * i)), m_iIntensity, Ctx);
	}

void COrbEffectPainter::PaintLightning (CG32bitImage &Dest, int xCenter, int yCenter, const SFlareDesc &FlareDesc, SViewportPaintCtx &Ctx)

//	PaintLightning
//
//	Paints lightning spikes

	{
	int i;

	int iFlareCount = m_SpikeCount.Roll();
	if (iFlareCount <= 0)
		iFlareCount = 2 * DEFAULT_FLARE_COUNT;

	int iSeparation = 360 / iFlareCount;
	int iAngle = Ctx.iTick * 3;

	for (i = 0; i < iFlareCount; i++)
		{
		int xDest, yDest;
		IntPolarToVector(iAngle, FlareDesc.iLength, &xDest, &yDest);

		DrawLightning(Dest,
				xCenter, yCenter,
				xCenter + xDest, yCenter + yDest,
				m_rgbPrimaryColor,
				CG32bitPixel::Null(),
				0.5);

		iAngle += iSeparation;
		}
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

bool COrbEffectPainter::OnSetParam (CCreatePainterCtx &Ctx, const CString &sParam, const CEffectParamDesc &Value)

//	SetParam
//
//	Sets parameters

	{
	if (strEquals(sParam, ANIMATE_ATTRIB))
		m_iAnimation = (EAnimationTypes)Value.EvalIdentifier(ANIMATION_TABLE, animateMax, animateNone);

	else if (strEquals(sParam, BLEND_MODE_ATTRIB))
		m_iBlendMode = Value.EvalBlendMode();

	else if (strEquals(sParam, DETAIL_ATTRIB))
		m_iDetail = Value.EvalIntegerBounded(0, 100, 25);

	else if (strEquals(sParam, DISTORTION_ATTRIB))
		m_iDistortion = Value.EvalIntegerBounded(0, 100, 0);

	else if (strEquals(sParam, INTENSITY_ATTRIB))
		m_iIntensity = Value.EvalIntegerBounded(0, 100, 50);

	else if (strEquals(sParam, LIFETIME_ATTRIB))
		m_iLifetime = Value.EvalIntegerBounded(0, -1, 0);

	else if (strEquals(sParam, PRIMARY_COLOR_ATTRIB))
		m_rgbPrimaryColor = Value.EvalColor();

	else if (strEquals(sParam, RADIUS_ATTRIB))
		m_iRadius = Value.EvalIntegerBounded(1, -1, (int)(STD_SECONDS_PER_UPDATE * LIGHT_SECOND / KLICKS_PER_PIXEL));

	else if (strEquals(sParam, SECONDARY_COLOR_ATTRIB))
		m_rgbSecondaryColor = Value.EvalColor();
	
	else if (strEquals(sParam, SPIKE_COUNT_ATTRIB))
		m_SpikeCount = Value.EvalDiceRange(0);
	
	else if (strEquals(sParam, STYLE_ATTRIB))
		m_iStyle = (EOrbStyles)Value.EvalIdentifier(STYLE_TABLE, styleMax, styleSmooth);

	else
		return false;

	Invalidate();
	return true;
	}
