//	SFXOrb.cpp
//
//	COrbEffectCreator class
//	Copyright (c) 2014 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define ANIMATE_ATTRIB					CONSTLIT("animate")
#define INTENSITY_ATTRIB				CONSTLIT("intensity")
#define LIFETIME_ATTRIB					CONSTLIT("lifetime")
#define PRIMARY_COLOR_ATTRIB			CONSTLIT("primaryColor")
#define RADIUS_ATTRIB					CONSTLIT("radius")
#define SECONDARY_COLOR_ATTRIB			CONSTLIT("secondaryColor")
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
		virtual void SetParam (CCreatePainterCtx &Ctx, const CString &sParam, const CEffectParamDesc &Value);

	private:
		enum EAnimationTypes
			{
			animateNone =			0,

			animateFade =			1,
			};

		enum EOrbStyles
			{
			styleUnknown =			0,

			styleSmooth =			1,
			styleFlare =			2,

			styleMax =				2,
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
		void CalcSphericalColorTable (int iRadius, int iIntensity, CG32bitPixel rgbPrimary, CG32bitPixel rgbSecondary, TArray<CG32bitPixel> *retColorTable);
		void CompositeFlareRay (CG32bitImage &Dest, int xCenter, int yCenter, int iLength, int iWidth, int iAngle, int iIntensity, SViewportPaintCtx &Ctx);
		inline bool HasFlares (void) const { return (m_iStyle == styleFlare); }
		void Invalidate (void);
		void PaintFlareRay (CG32bitImage &Dest, int xCenter, int yCenter, int iLength, int iWidth, int iAngle, int iIntensity, SViewportPaintCtx &Ctx);

		CEffectCreator *m_pCreator;

		int m_iRadius;
		EOrbStyles m_iStyle;
		int m_iIntensity;
		CG32bitPixel m_rgbPrimaryColor;
		CG32bitPixel m_rgbSecondaryColor;

		int m_iLifetime;
		EAnimationTypes m_iAnimation;

		//	Temporary variables based on shape/style/etc.

		bool m_bInitialized;				//	TRUE if values are valid
		TArray<TArray<CG32bitPixel>> m_ColorTable;
	};

static LPSTR ANIMATION_TABLE[] =
	{
	//	Must be same order as EAnimationTypes
		"",

		"fade",

		NULL
	};

static LPSTR STYLE_TABLE[] =
	{
	//	Must be same order as ERayStyles
		"",

		"smooth",
		"flare",

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

IEffectPainter *COrbEffectCreator::CreatePainter (CCreatePainterCtx &Ctx)

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
	pPainter->SetParam(Ctx, RADIUS_ATTRIB, m_Radius);
	pPainter->SetParam(Ctx, STYLE_ATTRIB, m_Style);
	pPainter->SetParam(Ctx, INTENSITY_ATTRIB, m_Intensity);
	pPainter->SetParam(Ctx, PRIMARY_COLOR_ATTRIB, m_PrimaryColor);
	pPainter->SetParam(Ctx, SECONDARY_COLOR_ATTRIB, m_SecondaryColor);
	pPainter->SetParam(Ctx, LIFETIME_ATTRIB, m_Lifetime);

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

	if (error = m_Radius.InitIntegerFromXML(Ctx, pDesc->GetAttribute(RADIUS_ATTRIB)))
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

COrbEffectPainter::COrbEffectPainter (CEffectCreator *pCreator) : 
		m_pCreator(pCreator),
		m_iRadius((int)(STD_SECONDS_PER_UPDATE * LIGHT_SECOND / KLICKS_PER_PIXEL)),
		m_iStyle(styleSmooth),
		m_iIntensity(50),
		m_rgbPrimaryColor(CG32bitPixel(255, 255, 255)),
		m_rgbSecondaryColor(CG32bitPixel(128, 128, 128)),
		m_iLifetime(0),
		m_iAnimation(animateNone),
		m_bInitialized(false)

//	COrbEffectCreator constructor

	{
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

		//	Initialized based on animation property

		switch (m_iAnimation)
			{
			//	Standard fade decreases radius and intensity linearly over the
			//	lifetime of the animation.

			case animateFade:
				{
				int iLifetime = Max(1, m_iLifetime);

				m_ColorTable.InsertEmpty(iLifetime);
				for (i = 0; i < iLifetime; i++)
					{
					Metric rFade = (iLifetime - i) / (Metric)iLifetime;
					int iRadius = (int)(rFade * m_iRadius);
					int iIntensity = (int)(rFade * m_iIntensity);

					CalcSphericalColorTable(iRadius, iIntensity, m_rgbPrimaryColor, m_rgbSecondaryColor, &m_ColorTable[i]);
					}

				break;
				}

			//	By default we only have a singe animation frame.

			default:
				m_ColorTable.InsertEmpty(1);
				CalcSphericalColorTable(m_iRadius, m_iIntensity, m_rgbPrimaryColor, m_rgbSecondaryColor, &m_ColorTable[0]);
				break;
			}

		m_bInitialized = true;
		}

	return (m_ColorTable.GetCount() > 0);
	}

void COrbEffectPainter::CalcSphericalColorTable (int iRadius, int iIntensity, CG32bitPixel rgbPrimary, CG32bitPixel rgbSecondary, TArray<CG32bitPixel> *retColorTable)

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

	switch (m_iStyle)
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
					(*retColorTable)[i] = CG32bitPixel(255, 255, 255);

				else if (i < iFringeMaxRadius && iFringeWidth > 0)
					{
					int iStep = (i - iBlownRadius);
					DWORD dwOpacity = iStep * 255 / iFringeWidth;
					(*retColorTable)[i] = CG32bitPixel::Blend(CG32bitPixel(255, 255, 255), rgbPrimary, (BYTE)dwOpacity);
					}
				else if (iFadeWidth > 0)
					{
					int iStep = (i - iFringeMaxRadius);
					Metric rOpacity = 255.0 - (iStep * 255.0 / iFadeWidth);
					rOpacity = (rOpacity * rOpacity) / 255.0;
					(*retColorTable)[i] = CG32bitPixel(rgbSecondary, (BYTE)(DWORD)rOpacity);
					}
				else
					(*retColorTable)[i] = CG32bitPixel::Null();
				}

			break;
			}

		//	We progress from primary color (at the center) to secondary color
		//	at the edge, and from full opacity to transparent.

		case styleFlare:
			{
			CG32bitPixel rgbCenter = CG32bitPixel::Fade(rgbPrimary, CG32bitPixel(255, 255, 255), iIntensity);
			CG32bitPixel rgbEdge = rgbSecondary;

			for (i = 0; i < iRadius; i++)
				{
				Metric rFade = (Metric)i / (Metric)iRadius;
				CG32bitPixel rgbColor = CG32bitPixel::Blend(rgbCenter, rgbEdge, rFade);
				BYTE byAlpha = (BYTE)(255 - (i * 255 / iRadius));

				(*retColorTable)[i] = CG32bitPixel(rgbColor, byAlpha);
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

void COrbEffectPainter::GetParam (const CString &sParam, CEffectParamDesc *retValue)

//	GetParam
//
//	Returns the parameter

	{
	if (strEquals(sParam, ANIMATE_ATTRIB))
		retValue->InitInteger(m_iAnimation);

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
	retList->InsertEmpty(7);
	retList->GetAt(0) = ANIMATE_ATTRIB;
	retList->GetAt(1) = INTENSITY_ATTRIB;
	retList->GetAt(2) = LIFETIME_ATTRIB;
	retList->GetAt(3) = PRIMARY_COLOR_ATTRIB;
	retList->GetAt(4) = RADIUS_ATTRIB;
	retList->GetAt(5) = SECONDARY_COLOR_ATTRIB;
	retList->GetAt(6) = STYLE_ATTRIB;

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

	TArray<CG32bitPixel> &Table = m_ColorTable[Ctx.iTick % m_ColorTable.GetCount()];
	CGDraw::Circle(Dest, x, y, Table.GetCount(), Table);

	if (HasFlares())
		{
		int iLength = Table.GetCount() * FLARE_MULITPLE;
		int iWidth = Max(1, iLength / FLARE_WIDTH_FRACTION);

		PaintFlareRay(Dest, x, y, iLength, iWidth, 0, m_iIntensity, Ctx);
		PaintFlareRay(Dest, x, y, iLength, iWidth, 60, m_iIntensity, Ctx);
		PaintFlareRay(Dest, x, y, iLength, iWidth, 120, m_iIntensity, Ctx);
		}
	}

void COrbEffectPainter::PaintComposite (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	PaintComposite
//
//	Paint the effect

	{
	if (!CalcIntermediates())
		return;

	TArray<CG32bitPixel> &Table = m_ColorTable[Ctx.iTick % m_ColorTable.GetCount()];
	CGComposite::Circle(Dest, x, y, Table.GetCount(), Table);

	if (HasFlares())
		{
		int iLength = Table.GetCount() * FLARE_MULITPLE;
		int iWidth = Max(1, iLength / FLARE_WIDTH_FRACTION);

		CompositeFlareRay(Dest, x, y, iLength, iWidth, 0, m_iIntensity, Ctx);
		CompositeFlareRay(Dest, x, y, iLength, iWidth, 60, m_iIntensity, Ctx);
		CompositeFlareRay(Dest, x, y, iLength, iWidth, 120, m_iIntensity, Ctx);
		}
	}

void COrbEffectPainter::PaintFlareRay (CG32bitImage &Dest, int xCenter, int yCenter, int iLength, int iWidth, int iAngle, int iIntensity, SViewportPaintCtx &Ctx)

//	PaintFlareRay
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

void COrbEffectPainter::SetParam (CCreatePainterCtx &Ctx, const CString &sParam, const CEffectParamDesc &Value)

//	SetParam
//
//	Sets parameters

	{
	if (strEquals(sParam, ANIMATE_ATTRIB))
		m_iAnimation = (EAnimationTypes)Value.EvalIntegerBounded(Ctx, 0, -1, animateNone);

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
	
	else if (strEquals(sParam, STYLE_ATTRIB))
		m_iStyle = (EOrbStyles)Value.EvalIdentifier(Ctx, STYLE_TABLE, styleMax, styleSmooth);
	}
