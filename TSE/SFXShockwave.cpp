//	SFXShockwave.cpp
//
//	Shockwave SFX

#include "PreComp.h"

#define IMAGE_TAG								CONSTLIT("Image")

#define BLEND_MODE_ATTRIB						CONSTLIT("blendMode")
#define FADE_START_ATTRIB						CONSTLIT("fadeStart")
#define GLOW_SIZE_ATTRIB						CONSTLIT("glowSize")
#define IMAGE_ATTRIB							CONSTLIT("image")
#define INTENSITY_ATTRIB						CONSTLIT("intensity")
#define LIFETIME_ATTRIB							CONSTLIT("lifetime")
#define PRIMARY_COLOR_ATTRIB					CONSTLIT("primaryColor")
#define SECONDARY_COLOR_ATTRIB					CONSTLIT("secondaryColor")
#define SPEED_ATTRIB							CONSTLIT("speed")
#define STYLE_ATTRIB							CONSTLIT("style")
#define WAVE_COUNT_ATTRIB						CONSTLIT("waveCount")
#define WAVE_INTERVAL_ATTRIB					CONSTLIT("waveInterval")
#define WAVE_LIFETIME_ATTRIB					CONSTLIT("waveLifetime")
#define WIDTH_ATTRIB							CONSTLIT("width")

const int CLOUD_TEXTURE_SIZE =					512;

class CShockwavePainter : public IEffectPainter
	{
	public:
		enum EStyles
			{
			styleUnknown =				0,

			styleGlowRing =				1,	//	Glowing right
			styleImage =				2,	//	Use an image to paint shockwave
			styleCloud =				3,	//	Fractal cloud

			styleMax =					3,
			};

		CShockwavePainter (CShockwaveEffectCreator *pCreator);

		//	IEffectPainter virtuals
		virtual CEffectCreator *GetCreator (void) { return m_pCreator; }
		virtual int GetLifetime (void) override;
		virtual Metric GetRadius (int iTick) const { return g_KlicksPerPixel * CalcRadius(iTick); }
		virtual void GetParam (const CString &sParam, CEffectParamDesc *retValue);
		virtual bool GetParamList (TArray<CString> *retList) const;
		virtual void GetRect (RECT *retRect) const;
		virtual void OnMove (SEffectMoveCtx &Ctx, bool *retbBoundsChanged = NULL);
		virtual void OnUpdate (SEffectUpdateCtx &Ctx);
		virtual void Paint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);

		static void Mark (EStyles iStyle) { if (iStyle == styleCloud) GetCloudTexture()->SetMarked();  }

	protected:
		virtual void OnReadFromStream (SLoadCtx &Ctx);
		virtual void OnSetParam (CCreatePainterCtx &Ctx, const CString &sParam, const CEffectParamDesc &Value);
		virtual void OnWriteToStream (IWriteStream *pStream);

	private:
		bool CalcIntermediates (void);
		BYTE CalcOpacity (int iTick) const;
		inline int CalcRadius (int iTick) const { return 1 + Max(0, (m_iRadiusInc * iTick)); }
		bool CreateGlowGradient (int iSolidWidth, int iGlowWidth, CG32bitPixel rgbSolidColor, CG32bitPixel rgbGlowColor);
		static CG8bitImage *GetCloudTexture (void);
		inline int GetWaveLifetime (void) const { return (m_iWaveLifetime > 0 ? m_iWaveLifetime : m_iLifetime); }
		void PaintRing (SViewportPaintCtx &Ctx, CG32bitImage &Dest, int x, int y, int iRadius, BYTE byOpacity) const;

		CShockwaveEffectCreator *m_pCreator;

		EStyles m_iStyle;					//	Style
		CObjectImageArray m_Image;			//	Image to use
		int m_iSpeed;						//	Expansion speed
		int m_iLifetime;					//	Lifetime
		int m_iFadeStart;					//	Tick on which we start to fade
		int m_iWidth;						//	Width of central ring
		int m_iIntensity;					//	Brighter
		int m_iGlowWidth;					//	Glow width
		int m_iWaveCount;					//	Number of waves (if multiple)
		int m_iWaveInterval;				//	Ticks between wave
		int m_iWaveLifetime;				//	Lifetime of one wave (if multiple)
		CGDraw::EBlendModes m_iBlendMode;
		CG32bitPixel m_rgbPrimaryColor;		//	Primary color
		CG32bitPixel m_rgbSecondaryColor;	//	Secondary color

		//	Runtime values

		CShockwaveHitTest m_HitTest;

		//	Computed values

		bool m_bInitialized;				//	TRUE if values are valid
		int m_iRadiusInc;					//	Radius increase (pixels per tick)
		int m_iGradientCount;
		TArray<CG32bitPixel> m_ColorGradient;

		static CG8bitImage *m_pCloudTexture;
	};

CG8bitImage *CShockwavePainter::m_pCloudTexture = NULL;

static LPSTR STYLE_TABLE[] =
	{
	//	Must be same order as EStyles
		"",

		"glowRing",
		"image",
		"cloud",

		NULL,
	};

//	CShockwaveEffectCreator object

CShockwaveEffectCreator::CShockwaveEffectCreator (void) :
		m_pSingleton(NULL)

//	CShockwaveEffectCreator constructor

	{
	}

CShockwaveEffectCreator::~CShockwaveEffectCreator (void)

//	CShockwaveEffectCreator destructor

	{
	if (m_pSingleton)
		delete m_pSingleton;
	}

IEffectPainter *CShockwaveEffectCreator::OnCreatePainter (CCreatePainterCtx &Ctx)

//	CreatePainter
//
//	Creates a new painter

	{
	//	If we have a singleton, return that

	if (m_pSingleton)
		return m_pSingleton;

	//	Otherwise we need to create a painter with the actual
	//	parameters.

	IEffectPainter *pPainter = new CShockwavePainter(this);

	//	Initialize the painter parameters

	pPainter->SetParam(Ctx, BLEND_MODE_ATTRIB, m_BlendMode);
	pPainter->SetParam(Ctx, FADE_START_ATTRIB, m_FadeStart);
	pPainter->SetParam(Ctx, GLOW_SIZE_ATTRIB, m_GlowWidth);
	pPainter->SetParam(Ctx, IMAGE_ATTRIB, m_Image);
	pPainter->SetParam(Ctx, INTENSITY_ATTRIB, m_Intensity);
	pPainter->SetParam(Ctx, LIFETIME_ATTRIB, m_Lifetime);
	pPainter->SetParam(Ctx, PRIMARY_COLOR_ATTRIB, m_PrimaryColor);
	pPainter->SetParam(Ctx, SECONDARY_COLOR_ATTRIB, m_SecondaryColor);
	pPainter->SetParam(Ctx, SPEED_ATTRIB, m_Speed);
	pPainter->SetParam(Ctx, STYLE_ATTRIB, m_Style);
	pPainter->SetParam(Ctx, WAVE_COUNT_ATTRIB, m_WaveCount);
	pPainter->SetParam(Ctx, WAVE_INTERVAL_ATTRIB, m_WaveInterval);
	pPainter->SetParam(Ctx, WAVE_LIFETIME_ATTRIB, m_WaveLifetime);
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

ALERROR CShockwaveEffectCreator::OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID)

//	OnEffectCreateFromXML
//
//	Initializes from XML

	{
	ALERROR error;

	if (error = m_BlendMode.InitBlendModeFromXML(Ctx, pDesc->GetAttribute(BLEND_MODE_ATTRIB)))
		return error;

	if (error = m_FadeStart.InitIntegerFromXML(Ctx, pDesc->GetAttribute(FADE_START_ATTRIB)))
		return error;

	if (error = m_GlowWidth.InitIntegerFromXML(Ctx, pDesc->GetAttribute(GLOW_SIZE_ATTRIB)))
		return error;

	if (error = m_Image.InitImageFromXML(Ctx, pDesc->GetContentElementByTag(IMAGE_TAG)))
		return error;

	if (error = m_Intensity.InitIntegerFromXML(Ctx, pDesc->GetAttribute(INTENSITY_ATTRIB)))
		return error;

	if (error = m_Lifetime.InitIntegerFromXML(Ctx, pDesc->GetAttribute(LIFETIME_ATTRIB)))
		return error;

	if (error = m_PrimaryColor.InitColorFromXML(Ctx, pDesc->GetAttribute(PRIMARY_COLOR_ATTRIB)))
		return error;

	if (error = m_SecondaryColor.InitColorFromXML(Ctx, pDesc->GetAttribute(SECONDARY_COLOR_ATTRIB)))
		return error;

	if (error = m_Speed.InitIntegerFromXML(Ctx, pDesc->GetAttribute(SPEED_ATTRIB)))
		return error;

	if (error = m_Style.InitIdentifierFromXML(Ctx, pDesc->GetAttribute(STYLE_ATTRIB), STYLE_TABLE))
		return error;

	if (error = m_WaveCount.InitIntegerFromXML(Ctx, pDesc->GetAttribute(WAVE_COUNT_ATTRIB)))
		return error;

	if (error = m_WaveInterval.InitIntegerFromXML(Ctx, pDesc->GetAttribute(WAVE_INTERVAL_ATTRIB)))
		return error;

	if (error = m_WaveLifetime.InitIntegerFromXML(Ctx, pDesc->GetAttribute(WAVE_LIFETIME_ATTRIB)))
		return error;

	if (error = m_Width.InitIntegerFromXML(Ctx, pDesc->GetAttribute(WIDTH_ATTRIB)))
		return error;

	return NOERROR;
	}

ALERROR CShockwaveEffectCreator::OnEffectBindDesign (SDesignLoadCtx &Ctx)

//	OnEffectBindDesign
//
//	Resolve loading

	{
	ALERROR error;

	if (error = m_Image.Bind(Ctx))
		return error;

	return NOERROR;
	}

void CShockwaveEffectCreator::OnEffectMarkResources (void)

//	OnEffectMarkResources
//
//	Mark images that are in use
	
	{
	m_Image.MarkImage();

	CCreatePainterCtx Ctx;
	CShockwavePainter::EStyles iStyle = (CShockwavePainter::EStyles)m_Style.EvalIdentifier(STYLE_TABLE, CShockwavePainter::styleMax, CShockwavePainter::styleImage);
	CShockwavePainter::Mark(iStyle);
	}

//	CShockwavePainter object

CShockwavePainter::CShockwavePainter (CShockwaveEffectCreator *pCreator) : m_pCreator(pCreator),
		m_iStyle(styleGlowRing),
		m_iSpeed(20),
		m_iLifetime(10),
		m_iFadeStart(100),
		m_iWidth(10),
		m_iIntensity(50),
		m_iGlowWidth(5),
		m_iWaveCount(1),
		m_iWaveInterval(5),
		m_iWaveLifetime(10),
		m_iBlendMode(CGDraw::blendNormal),
		m_rgbPrimaryColor(CG32bitPixel(255, 255, 255)),
		m_rgbSecondaryColor(CG32bitPixel(128, 128, 128)),
		m_bInitialized(false),
		m_iRadiusInc(1)

//	CShockwavePainter constructor

	{
	}

bool CShockwavePainter::CalcIntermediates (void)

//	CalcIntermediates
//
//	Initialize various tables

	{
	int i;

	if (m_bInitialized)
		return true;

	switch (m_iStyle)
		{
		case styleCloud:
			{
			int cxWidth = Max(CLOUD_TEXTURE_SIZE, CalcRadius(GetWaveLifetime()) * 6);
			int cyHeight = m_iWidth;
			if (m_iWidth <= 0)
				return false;

			//	Calculate a table so we can fade the ring

			int iCenter = cyHeight / 4;
			int iTailSize = cyHeight - iCenter;
			CStepIncrementor Head(CStepIncrementor::styleSquare, 0.0, 255.0, iCenter);
			CStepIncrementor Tail(CStepIncrementor::styleSquareRoot, 255.0, 0.0, iTailSize);

			BYTE *pFade = new BYTE [cyHeight];
			for (i = 0; i < cyHeight; i++)
				{
				if (i < iCenter)
					pFade[i] = (BYTE)Head.GetAt(i);
				else
					pFade[i] = (BYTE)Tail.GetAt(i - iCenter);
				}

			//	Compute core glow

			int iBlownRadius = m_iIntensity * cyHeight / 1200;
			int iFringeSize = m_iIntensity * cyHeight / 100;
			int iFringeRadius = iBlownRadius + iFringeSize;

			CG32bitPixel *pGlow = new CG32bitPixel [cyHeight];
			for (i = 0; i < cyHeight; i++)
				{
				int iOffset = Absolute(i - iCenter);
				if (iOffset < iBlownRadius)
					{
					pGlow[i] = CG32bitPixel(255, 255, 255);
					pFade[i] = 255;
					}
				else if (iOffset < iFringeRadius)
					{
					BYTE byAlpha = 255 - (255 * (1 + iOffset - iBlownRadius) / (1 + iFringeSize));
					pGlow[i] = CG32bitPixel(m_rgbPrimaryColor, byAlpha);
					pFade[i] = Max(byAlpha, pFade[i]);
					}
				else
					pGlow[i] = CG32bitPixel::Null();
				}

			//	Create a image to use as the shockwave

			CG32bitPixel rgbTextureColor = (m_rgbSecondaryColor.IsNull() ? m_rgbPrimaryColor : m_rgbSecondaryColor);

			CG32bitImage *pImage = new CG32bitImage;
			pImage->Create(cxWidth, cyHeight, CG32bitImage::alpha8);

			CG8bitImage *pTexture = GetCloudTexture();

			CG32bitPixel *pRow = pImage->GetPixelPos(0, 0);
			CG32bitPixel *pRowEnd = pImage->GetPixelPos(0, cyHeight);
			int y = 0;
			while (pRow < pRowEnd)
				{
				CG32bitPixel *pPos = pRow;
				CG32bitPixel *pPosEnd = pRow + cxWidth;
				int x = 0;
				while (pPos < pPosEnd)
					{
					BYTE byTexture = *(pTexture->GetPixelPos(x % pTexture->GetWidth(), y % pTexture->GetHeight()));
					CG32bitPixel rgbColor = CG32bitPixel(rgbTextureColor, byTexture);
					if (!pGlow[y].IsNull())
						rgbColor = CG32bitPixel::Composite(rgbColor, pGlow[y]);

					*pPos = CG32bitPixel::PreMult(CG32bitPixel(rgbColor, CG32bitPixel::BlendAlpha(rgbColor.GetAlpha(), pFade[y])));

					pPos++;
					x++;
					}

				pRow = pImage->NextRow(pRow);
				y++;
				}

			delete[] pGlow;
			pGlow = NULL;

			delete[] pFade;
			pFade = NULL;

			//	Store in m_Image (which takes ownership of the image).

			RECT rcRect;
			rcRect.left = 0;
			rcRect.right = cxWidth;
			rcRect.top = 0;
			rcRect.bottom = cyHeight;

			if (m_Image.InitFromBitmap(pImage, rcRect, 1, 1, true) != NOERROR)
				{
				delete pImage;
				return false;
				}

			break;
			}

		case styleGlowRing:
			if (!CreateGlowGradient(m_iWidth, m_iGlowWidth, m_rgbPrimaryColor, m_rgbSecondaryColor))
				return false;
			break;
		}

	m_bInitialized = true;
	return true;
	}

BYTE CShockwavePainter::CalcOpacity (int iTick) const

//	CalcOpacity
//
//	Calculate the opacity of a ring at the given tick.

	{
	int iLifetime = GetWaveLifetime();
	int iStartDecay = m_iFadeStart * iLifetime / 100;
	int iDecayRange = iLifetime - iStartDecay;

	if (iTick > iLifetime)
		return 0;
	else if (iTick > iStartDecay && iDecayRange > 0)
		return 255 * (iDecayRange - (iTick - iStartDecay)) / iDecayRange;
	else
		return 255;
	}

bool CShockwavePainter::CreateGlowGradient (int iSolidWidth, int iGlowWidth, CG32bitPixel rgbSolidColor, CG32bitPixel rgbGlowColor)

//	CreateGlowGradient
//
//	Creates a gradient for a glowing ring

	{
	int i;

	ASSERT(iSolidWidth >= 0);
	ASSERT(iGlowWidth >= 0);
	ASSERT(iSolidWidth + iGlowWidth > 0);

	m_iGradientCount = iSolidWidth + 2 * iGlowWidth;
	m_ColorGradient.DeleteAll();
	if (m_iGradientCount <= 0)
		return false;

	m_ColorGradient.InsertEmpty(m_iGradientCount);

	//	Add glow ramp

	for (i = 0; i < iGlowWidth; i++)
		{
		int iFade = (256 * (i + 1) / (iGlowWidth + 1));

		CG32bitPixel rgbColor = CG32bitPixel::Blend(rgbGlowColor, rgbSolidColor, (BYTE)iFade);
		m_ColorGradient[i] = CG32bitPixel(rgbColor, (BYTE)iFade);
		m_ColorGradient[m_iGradientCount - (i + 1)] = CG32bitPixel(rgbColor, (BYTE)iFade);
		}

	//	Add the solid color in the center

	for (i = 0; i < iSolidWidth; i++)
		m_ColorGradient[iGlowWidth + i] = rgbSolidColor;

	return true;
	}

CG8bitImage *CShockwavePainter::GetCloudTexture (void)

//	GetCloudTexture
//
//	Returns a properly initialized cloud texture.

	{
	//	Allocate image, if necessary.

	if (m_pCloudTexture == NULL)
		m_pCloudTexture = g_pUniverse->GetDynamicImageLibrary().Alloc8();

	//	Initialize the texture, if necessary

	if (m_pCloudTexture->IsEmpty())
		{
		SGCloudDesc Desc;

		m_pCloudTexture->Create(CLOUD_TEXTURE_SIZE, CLOUD_TEXTURE_SIZE);
		CGFractal::FillClouds(*m_pCloudTexture, 0, 0, CLOUD_TEXTURE_SIZE, CLOUD_TEXTURE_SIZE, Desc);
		}

	//	Done

	return m_pCloudTexture;
	}

int CShockwavePainter::GetLifetime (void)

//	GetLifetime
//
//	Returns the total lifetime.

	{
	if (m_iWaveCount > 1)
		{
		return (m_iWaveCount * m_iWaveInterval) + GetWaveLifetime();
		}
	else
		return Max(m_iLifetime, m_iWaveLifetime);
	}

void CShockwavePainter::GetParam (const CString &sParam, CEffectParamDesc *retValue)

//	GetParam
//
//	Returns a parameter

	{
	if (strEquals(sParam, BLEND_MODE_ATTRIB))
		retValue->InitInteger(m_iBlendMode);
	else if (strEquals(sParam, FADE_START_ATTRIB))
		retValue->InitInteger(m_iFadeStart);
	else if (strEquals(sParam, GLOW_SIZE_ATTRIB))
		retValue->InitInteger(m_iGlowWidth);
	else if (strEquals(sParam, IMAGE_ATTRIB))
		retValue->InitImage(m_Image);
	else if (strEquals(sParam, INTENSITY_ATTRIB))
		retValue->InitInteger(m_iIntensity);
	else if (strEquals(sParam, LIFETIME_ATTRIB))
		retValue->InitInteger(m_iLifetime);
	else if (strEquals(sParam, PRIMARY_COLOR_ATTRIB))
		retValue->InitColor(m_rgbPrimaryColor);
	else if (strEquals(sParam, SECONDARY_COLOR_ATTRIB))
		retValue->InitColor(m_rgbSecondaryColor);
	else if (strEquals(sParam, SPEED_ATTRIB))
		retValue->InitInteger(m_iSpeed);
	else if (strEquals(sParam, STYLE_ATTRIB))
		retValue->InitInteger(m_iStyle);
	else if (strEquals(sParam, WAVE_COUNT_ATTRIB))
		retValue->InitInteger(m_iWaveCount);
	else if (strEquals(sParam, WAVE_INTERVAL_ATTRIB))
		retValue->InitInteger(m_iWaveInterval);
	else if (strEquals(sParam, WAVE_LIFETIME_ATTRIB))
		retValue->InitInteger(m_iWaveLifetime);
	else if (strEquals(sParam, WIDTH_ATTRIB))
		retValue->InitInteger(m_iWidth);
	}

bool CShockwavePainter::GetParamList (TArray<CString> *retList) const

//	GetParamList
//
//	Returns a list of value parameter names

	{
	retList->DeleteAll();
	retList->InsertEmpty(14);
	retList->GetAt(0) = BLEND_MODE_ATTRIB;
	retList->GetAt(1) = FADE_START_ATTRIB;
	retList->GetAt(2) = GLOW_SIZE_ATTRIB;
	retList->GetAt(3) = IMAGE_ATTRIB;
	retList->GetAt(4) = INTENSITY_ATTRIB;
	retList->GetAt(5) = LIFETIME_ATTRIB;
	retList->GetAt(6) = PRIMARY_COLOR_ATTRIB;
	retList->GetAt(7) = SECONDARY_COLOR_ATTRIB;
	retList->GetAt(8) = SPEED_ATTRIB;
	retList->GetAt(9) = STYLE_ATTRIB;
	retList->GetAt(10) = WAVE_COUNT_ATTRIB;
	retList->GetAt(11) = WAVE_INTERVAL_ATTRIB;
	retList->GetAt(12) = WAVE_LIFETIME_ATTRIB;
	retList->GetAt(13) = WIDTH_ATTRIB;

	return true;
	}

void CShockwavePainter::GetRect (RECT *retRect) const

//	GetRect
//
//	Returns the RECT of the effect centered on 0,0

	{
	int iRadius = CalcRadius(GetWaveLifetime());

	retRect->left = -iRadius;
	retRect->top = -iRadius;
	retRect->right = iRadius + 1;
	retRect->bottom = iRadius + 1;
	}

void CShockwavePainter::OnMove (SEffectMoveCtx &Ctx, bool *retbBoundsChanged)

//	OnMove
//
//	Handle move event

	{
	//	Bounds have changed because we expand.

	if (retbBoundsChanged)
		*retbBoundsChanged = true;
	}

void CShockwavePainter::OnReadFromStream (SLoadCtx &Ctx)

//	OnReadFromStream
//
//	Load from stream
//
//	Params
//	CShockwaveHitTest	m_HitTest

	{
	//	Read the parameters

	if (Ctx.dwVersion >= 121)
		IEffectPainter::OnReadFromStream(Ctx);
	else
		{
		//	No longer needed because we use the tick to compute it.

		int iRadius;
		Ctx.pStream->Read((char *)&iRadius, sizeof(DWORD));

		if (Ctx.dwVersion >= 70)
			Ctx.pStream->Read((char *)&m_iRadiusInc, sizeof(DWORD));
		else
			m_iRadiusInc = 1;

		if (Ctx.dwVersion >= 20)
			{
			int iTick;
			Ctx.pStream->Read((char *)&iTick, sizeof(DWORD));
			}
		}

	//	We've kept track of hit testing.

	if (Ctx.dwVersion >= 69)
		m_HitTest.ReadFromStream(Ctx);
	}

void CShockwavePainter::OnSetParam (CCreatePainterCtx &Ctx, const CString &sParam, const CEffectParamDesc &Value)

//	OnSetParam
//
//	Sets parameters

	{
	if (strEquals(sParam, BLEND_MODE_ATTRIB))
		m_iBlendMode = Value.EvalBlendMode();
	else if (strEquals(sParam, FADE_START_ATTRIB))
		m_iFadeStart = Value.EvalIntegerBounded(0, 100, 100);
	else if (strEquals(sParam, GLOW_SIZE_ATTRIB))
		m_iGlowWidth = Value.EvalIntegerBounded(0, -1, 0);
	else if (strEquals(sParam, INTENSITY_ATTRIB))
		m_iIntensity = Value.EvalIntegerBounded(0, -1, 50);
	else if (strEquals(sParam, IMAGE_ATTRIB))
		m_Image = Value.EvalImage();
	else if (strEquals(sParam, LIFETIME_ATTRIB))
		m_iLifetime = Value.EvalIntegerBounded(0, -1, 0);
	else if (strEquals(sParam, PRIMARY_COLOR_ATTRIB))
		m_rgbPrimaryColor = Value.EvalColor();
	else if (strEquals(sParam, SECONDARY_COLOR_ATTRIB))
		m_rgbSecondaryColor = Value.EvalColor();
	else if (strEquals(sParam, SPEED_ATTRIB))
		{
		m_iSpeed = Value.EvalIntegerBounded(0, 100, 20);
		m_iRadiusInc = Max(1, (int)((m_iSpeed * LIGHT_SPEED * g_SecondsPerUpdate / (100.0 * g_KlicksPerPixel)) + 0.5));
		}
	else if (strEquals(sParam, STYLE_ATTRIB))
		m_iStyle = (EStyles)Value.EvalIdentifier(STYLE_TABLE, styleMax, styleImage);
	else if (strEquals(sParam, WAVE_COUNT_ATTRIB))
		m_iWaveCount = Value.EvalIntegerBounded(1, -1, 1);
	else if (strEquals(sParam, WAVE_INTERVAL_ATTRIB))
		m_iWaveInterval = Value.EvalIntegerBounded(0, -1, 5);
	else if (strEquals(sParam, WAVE_LIFETIME_ATTRIB))
		m_iWaveLifetime = Value.EvalIntegerBounded(0, -1, 0);
	else if (strEquals(sParam, WIDTH_ATTRIB))
		m_iWidth = Value.EvalIntegerBounded(0, -1, 1);
	}

void CShockwavePainter::OnUpdate (SEffectUpdateCtx &Ctx)

//	OnUpdate
//
//	Updates the effect

	{
	//	See if we do damage

	if (Ctx.pDamageDesc)
		{
		int iRadius = CalcRadius(Ctx.iTick);

		//	Initialize hit test

		if (m_HitTest.IsEmpty())
			m_HitTest.Init(Ctx.pDamageDesc->GetAreaDamageDensity(), 1);

		//	Compute the size of the expansion ring

		Metric rMaxRadius = iRadius * g_KlicksPerPixel;
		Metric rMinRadius = (iRadius - m_iRadiusInc) * g_KlicksPerPixel;

		//	See if we intersect any object and do damage

		m_HitTest.Update(Ctx, Ctx.pObj->GetPos(), rMinRadius, rMaxRadius);
		}
	}

void CShockwavePainter::OnWriteToStream (IWriteStream *pStream)

//	OnWriteToStream
//
//	Write to stream

	{
	IEffectPainter::OnWriteToStream(pStream);
	m_HitTest.WriteToStream(pStream);
	}

void CShockwavePainter::Paint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	Paint
//
//	Paint

	{
	int i;

	if (!CalcIntermediates())
		return;

	//	If we have a single ring, then we just paint it.

	if (m_iWaveCount <= 1)
		{
		int iRadius = CalcRadius(Ctx.iTick);
		BYTE byOpacity = CalcOpacity(Ctx.iTick);

		//	Paint

		PaintRing(Ctx, Dest, x, y, iRadius, byOpacity);
		}

	//	Otherwise, we paint multiple rings.

	else
		{
		int iTick = Ctx.iTick;

		for (i = 0; i < m_iWaveCount && iTick >= 0; i++)
			{
			int iRadius = CalcRadius(iTick);
			BYTE byOpacity = CalcOpacity(iTick);

			//	Paint

			PaintRing(Ctx, Dest, x, y, iRadius, byOpacity);

			//	Next ring is offset

			iTick -= m_iWaveInterval;
			}
		}
	}

void CShockwavePainter::PaintRing (SViewportPaintCtx &Ctx, CG32bitImage &Dest, int x, int y, int iRadius, BYTE byOpacity) const

//	PaintRing
//
//	Paints a single ring.

	{
	switch (m_iStyle)
		{
#if 0
		case styleCloud:
			m_Image.PaintImage(Dest, x, y, 0, 0);
			break;
#endif

		case styleCloud:
		case styleImage:
			{
			CG32bitImage &Image = m_Image.GetImage(m_pCreator->GetUNIDString());
			RECT rcImage = m_Image.GetImageRect();

			CGDraw::CircleImage(Dest, x, y, iRadius, (BYTE)byOpacity, Image, m_iBlendMode, rcImage.left, rcImage.top, RectWidth(rcImage), RectHeight(rcImage));
			break;
			}

		case styleGlowRing:
			{
			CGDraw::RingGlowing(Dest, x, y, iRadius, m_iGradientCount, m_ColorGradient, (BYTE)byOpacity);
			break;
			}

		default:
			ASSERT(false);
		}
	}
