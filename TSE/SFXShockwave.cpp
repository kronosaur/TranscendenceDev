//	SFXShockwave.cpp
//
//	Shockwave SFX

#include "PreComp.h"

#define IMAGE_TAG								CONSTLIT("Image")

#define FADE_START_ATTRIB						CONSTLIT("fadeStart")
#define GLOW_SIZE_ATTRIB						CONSTLIT("glowSize")
#define IMAGE_ATTRIB							CONSTLIT("image")
#define LIFETIME_ATTRIB							CONSTLIT("lifetime")
#define PRIMARY_COLOR_ATTRIB					CONSTLIT("primaryColor")
#define SECONDARY_COLOR_ATTRIB					CONSTLIT("secondaryColor")
#define SPEED_ATTRIB							CONSTLIT("speed")
#define STYLE_ATTRIB							CONSTLIT("style")
#define WIDTH_ATTRIB							CONSTLIT("width")

class CShockwavePainter : public IEffectPainter
	{
	public:
		CShockwavePainter (CShockwaveEffectCreator *pCreator);

		//	IEffectPainter virtuals
		virtual CEffectCreator *GetCreator (void) { return m_pCreator; }
		virtual Metric GetRadius (int iTick) const { return g_KlicksPerPixel * CalcRadius(iTick); }
		virtual void GetParam (const CString &sParam, CEffectParamDesc *retValue);
		virtual bool GetParamList (TArray<CString> *retList) const;
		virtual void GetRect (RECT *retRect) const;
		virtual void OnMove (SEffectMoveCtx &Ctx, bool *retbBoundsChanged = NULL);
		virtual void OnUpdate (SEffectUpdateCtx &Ctx);
		virtual void Paint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);

	protected:
		virtual void OnReadFromStream (SLoadCtx &Ctx);
		virtual void OnSetParam (CCreatePainterCtx &Ctx, const CString &sParam, const CEffectParamDesc &Value);
		virtual void OnWriteToStream (IWriteStream *pStream);

	private:
		enum EStyles
			{
			styleUnknown =				0,

			styleGlowRing =				1,	//	Glowing right
			styleImage =				2,	//	Use an image to paint shockwave

			styleMax =					2,
			};

		bool CalcIntermediates (void);
		inline int CalcRadius (int iTick) const { return 1 + Max(0, (m_iRadiusInc * iTick)); }
		bool CreateGlowGradient (int iSolidWidth, int iGlowWidth, CG32bitPixel rgbSolidColor, CG32bitPixel rgbGlowColor);

		CShockwaveEffectCreator *m_pCreator;

		EStyles m_iStyle;					//	Style
		CObjectImageArray m_Image;			//	Image to use
		int m_iSpeed;						//	Expansion speed
		int m_iLifetime;					//	Lifetime
		int m_iFadeStart;					//	Tick on which we start to fade
		int m_iWidth;						//	Width of central ring
		int m_iGlowWidth;					//	Glow width
		CG32bitPixel m_rgbPrimaryColor;		//	Primary color
		CG32bitPixel m_rgbSecondaryColor;	//	Secondary color

		//	Runtime values

		CShockwaveHitTest m_HitTest;

		//	Computed values

		bool m_bInitialized;				//	TRUE if values are valid
		int m_iRadiusInc;					//	Radius increase (pixels per tick)
		int m_iGradientCount;
		TArray<CG32bitPixel> m_ColorGradient;
	};

static LPSTR STYLE_TABLE[] =
	{
	//	Must be same order as EStyles
		"",

		"glowRing",
		"image",

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

	pPainter->SetParam(Ctx, FADE_START_ATTRIB, m_FadeStart);
	pPainter->SetParam(Ctx, GLOW_SIZE_ATTRIB, m_GlowWidth);
	pPainter->SetParam(Ctx, IMAGE_ATTRIB, m_Image);
	pPainter->SetParam(Ctx, LIFETIME_ATTRIB, m_Lifetime);
	pPainter->SetParam(Ctx, PRIMARY_COLOR_ATTRIB, m_PrimaryColor);
	pPainter->SetParam(Ctx, SECONDARY_COLOR_ATTRIB, m_SecondaryColor);
	pPainter->SetParam(Ctx, SPEED_ATTRIB, m_Speed);
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

ALERROR CShockwaveEffectCreator::OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID)

//	OnEffectCreateFromXML
//
//	Initializes from XML

	{
	ALERROR error;

	if (error = m_FadeStart.InitIntegerFromXML(Ctx, pDesc->GetAttribute(FADE_START_ATTRIB)))
		return error;

	if (error = m_GlowWidth.InitIntegerFromXML(Ctx, pDesc->GetAttribute(GLOW_SIZE_ATTRIB)))
		return error;

	if (error = m_Image.InitImageFromXML(Ctx, pDesc->GetContentElementByTag(IMAGE_TAG)))
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

//	CShockwavePainter object

CShockwavePainter::CShockwavePainter (CShockwaveEffectCreator *pCreator) : m_pCreator(pCreator),
		m_iStyle(styleGlowRing),
		m_iSpeed(20),
		m_iLifetime(10),
		m_iFadeStart(100),
		m_iWidth(10),
		m_iGlowWidth(5),
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
	if (m_bInitialized)
		return true;

	switch (m_iStyle)
		{
		case styleGlowRing:
			if (!CreateGlowGradient(m_iWidth, m_iGlowWidth, m_rgbPrimaryColor, m_rgbSecondaryColor))
				return false;
			break;
		}

	m_bInitialized = true;
	return true;
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

void CShockwavePainter::GetParam (const CString &sParam, CEffectParamDesc *retValue)

//	GetParam
//
//	Returns a parameter

	{
	if (strEquals(sParam, FADE_START_ATTRIB))
		retValue->InitInteger(m_iFadeStart);
	else if (strEquals(sParam, GLOW_SIZE_ATTRIB))
		retValue->InitInteger(m_iGlowWidth);
	else if (strEquals(sParam, IMAGE_ATTRIB))
		retValue->InitImage(m_Image);
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
	else if (strEquals(sParam, WIDTH_ATTRIB))
		retValue->InitInteger(m_iWidth);
	}

bool CShockwavePainter::GetParamList (TArray<CString> *retList) const

//	GetParamList
//
//	Returns a list of value parameter names

	{
	retList->DeleteAll();
	retList->InsertEmpty(9);
	retList->GetAt(0) = FADE_START_ATTRIB;
	retList->GetAt(1) = GLOW_SIZE_ATTRIB;
	retList->GetAt(2) = IMAGE_ATTRIB;
	retList->GetAt(3) = LIFETIME_ATTRIB;
	retList->GetAt(4) = PRIMARY_COLOR_ATTRIB;
	retList->GetAt(5) = SECONDARY_COLOR_ATTRIB;
	retList->GetAt(6) = SPEED_ATTRIB;
	retList->GetAt(7) = STYLE_ATTRIB;
	retList->GetAt(8) = WIDTH_ATTRIB;

	return true;
	}

void CShockwavePainter::GetRect (RECT *retRect) const

//	GetRect
//
//	Returns the RECT of the effect centered on 0,0

	{
	int iRadius = CalcRadius(m_iLifetime);

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
	if (strEquals(sParam, FADE_START_ATTRIB))
		m_iFadeStart = Value.EvalIntegerBounded(Ctx, 0, 100, 100);
	else if (strEquals(sParam, GLOW_SIZE_ATTRIB))
		m_iGlowWidth = Value.EvalIntegerBounded(Ctx, 0, -1, 0);
	else if (strEquals(sParam, IMAGE_ATTRIB))
		m_Image = Value.EvalImage(Ctx);
	else if (strEquals(sParam, LIFETIME_ATTRIB))
		m_iLifetime = Value.EvalIntegerBounded(Ctx, 0, -1, 0);
	else if (strEquals(sParam, PRIMARY_COLOR_ATTRIB))
		m_rgbPrimaryColor = Value.EvalColor(Ctx);
	else if (strEquals(sParam, SECONDARY_COLOR_ATTRIB))
		m_rgbSecondaryColor = Value.EvalColor(Ctx);
	else if (strEquals(sParam, SPEED_ATTRIB))
		{
		m_iSpeed = Value.EvalIntegerBounded(Ctx, 0, 100, 20);
		m_iRadiusInc = Max(1, (int)((m_iSpeed * LIGHT_SPEED * g_SecondsPerUpdate / (100.0 * g_KlicksPerPixel)) + 0.5));
		}
	else if (strEquals(sParam, STYLE_ATTRIB))
		m_iStyle = (EStyles)Value.EvalIdentifier(Ctx, STYLE_TABLE, styleMax, styleImage);
	else if (strEquals(sParam, WIDTH_ATTRIB))
		m_iWidth = Value.EvalIntegerBounded(Ctx, 0, -1, 1);
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
	if (!CalcIntermediates())
		return;

	int iRadius = CalcRadius(Ctx.iTick);
	int iLifetime = m_iLifetime;
	int iStartDecay = m_iFadeStart * iLifetime / 100;
	int iDecayRange = iLifetime - iStartDecay;

	DWORD byOpacity = 255;
	if (Ctx.iTick > iLifetime)
		byOpacity = 0;
	else if (Ctx.iTick > iStartDecay && iDecayRange > 0)
		byOpacity = byOpacity * (iDecayRange - (Ctx.iTick - iStartDecay)) / iDecayRange;

	switch (m_iStyle)
		{
		case styleImage:
			{
			CG32bitImage &Image = m_Image.GetImage(m_pCreator->GetUNIDString());
			RECT rcImage = m_Image.GetImageRect();

			CGDraw::CircleImage(Dest, x, y, iRadius, (BYTE)byOpacity, Image, CGDraw::blendNormal, rcImage.left, rcImage.top, RectWidth(rcImage), RectHeight(rcImage));
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

