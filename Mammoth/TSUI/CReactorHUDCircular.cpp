//	CReactorHUDCircular.cpp
//
//	CReactorHUDCircular class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

#define BACKGROUND_COLOR_ATTRIB				CONSTLIT("backgroundColor")
#define FUEL_COLOR_ATTRIB					CONSTLIT("fuelColor")
#define POWER_COLOR_ATTRIB					CONSTLIT("powerColor")
#define POWER_GEN_COLOR_ATTRIB				CONSTLIT("powerGenColor")
#define WARNING_COLOR_ATTRIB				CONSTLIT("warningColor")

const int RING_SPACING = 2;
const int LEFT_CONTROL_WIDTH = 64;
const int RIGHT_CONTROL_WIDTH = 200;

const CG32bitPixel DAMAGED_TEXT_COLOR =		CG32bitPixel(128, 0, 0);

CReactorHUDCircular::CReactorHUDCircular (void) :
		m_bInvalid(true),
		m_iTick(0)

//	CReactorHUDCircular constructor

	{
	}
CReactorHUDCircular::~CReactorHUDCircular (void)

//	CReactorHUDCircular destructor

	{
	}

ALERROR CReactorHUDCircular::Bind (SDesignLoadCtx &Ctx)

//	Bind
//
//	Bind design

	{
	return NOERROR;
	}

void CReactorHUDCircular::GetBounds (int *retWidth, int *retHeight) const

//	GetBounds
//
//	Returns the bounds of the HUD

	{
	*retWidth = m_cxDisplay;
	*retHeight = m_cyDisplay;
	}

bool CReactorHUDCircular::OnCreate (SHUDCreateCtx &CreateCtx, CString *retsError)

//	InitFromXML
//
//	Initializes from descriptor

	{
	const CVisualPalette &VI = g_pHI->GetVisuals();

	//	LATER: Load this from definition

	m_iGaugeRadius = 48;
	m_iGaugeWidth = 12;

	//	Colors

	m_rgbGaugeBack = ::LoadRGBColor(CreateCtx.Desc.GetAttribute(BACKGROUND_COLOR_ATTRIB), CG32bitPixel(55, 58, 64));
	m_rgbFuelGauge = ::LoadRGBColor(CreateCtx.Desc.GetAttribute(FUEL_COLOR_ATTRIB), CG32bitPixel(5, 211, 5));
	m_rgbPowerGauge = ::LoadRGBColor(CreateCtx.Desc.GetAttribute(POWER_COLOR_ATTRIB), CG32bitPixel(5, 211, 5));
	m_rgbOtherPower = ::LoadRGBColor(CreateCtx.Desc.GetAttribute(POWER_GEN_COLOR_ATTRIB), CG32bitPixel(4, 108, 212));
	m_rgbGaugeWarning = ::LoadRGBColor(CreateCtx.Desc.GetAttribute(WARNING_COLOR_ATTRIB), CG32bitPixel(211, 5, 5));

	//	Calculate metrics

	int iTotalRadius = m_iGaugeRadius + RING_SPACING + m_iGaugeWidth;
	m_cxDisplay = (2 * iTotalRadius) + 2 * RING_SPACING + LEFT_CONTROL_WIDTH + RIGHT_CONTROL_WIDTH;
	m_cyDisplay = (2 * iTotalRadius) + 2 * RING_SPACING;

	m_xCenter = LEFT_CONTROL_WIDTH + RING_SPACING + iTotalRadius;
	m_yCenter = RING_SPACING + iTotalRadius;

	//	Done

	return true;
	}

void CReactorHUDCircular::OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx)

//	OnPaint
//
//	Paint

	{
	DEBUG_TRY

	if (m_bInvalid)
		{
		Realize(Ctx);
		m_bInvalid = false;
		}

	Dest.Blt(0,
			0,
			m_Buffer.GetWidth(),
			m_Buffer.GetHeight(),
			Ctx.byOpacity,
			m_Buffer,
			x,
			y);

	DEBUG_CATCH
	}

void CReactorHUDCircular::OnUpdate (SHUDUpdateCtx &Ctx)

//	OnUpdate
//
//	Called once per tick

	{
	//	Every 7 ticks we update

	if ((Ctx.iTick % 7) == 0)
		{
		m_iTick++;
		Invalidate();
		}
	}

void CReactorHUDCircular::PaintChargesGauge (const SReactorStats &Stats)

//	PaintChargesGauge
//
//	Paint the number of charges (out of the maximum number of charges possible).
//	This paints on the left arc, instead of the fuel gauge.

	{
	int i;

	const CVisualPalette &VI = g_pHI->GetVisuals();
	const CG16bitFont &SmallFont = VI.GetFont(fontSmall);
	const CG16bitFont &MediumFont = VI.GetFont(fontMedium);

	//	Compute some metrics

	Metric rTotalArc = PI;
	Metric rChargeArc = rTotalArc / Stats.iMaxCharges;
	Metric rStartAngle = 1.5 * PI;
	Metric rAngle = rStartAngle;

	//	Paint all the charges

	for (i = 0; i < Stats.iMaxCharges; i++)
		{
		CG32bitPixel rgbColor = (i < Stats.iChargesLeft ? m_rgbFuelGauge : m_rgbGaugeBack);

		CGDraw::Arc(m_Buffer,
				CVector(m_xCenter, m_yCenter),
				m_iGaugeRadius + RING_SPACING,
				rAngle - rChargeArc,
				rAngle,
				m_iGaugeWidth,
				rgbColor,
				CGDraw::blendCompositeNormal,
				RING_SPACING / 2,
				CGDraw::ARC_INNER_RADIUS);

		rAngle -= rChargeArc;
		}
	}

void CReactorHUDCircular::PaintHeatGauge(CShip *pShip)

//	PaintHeatGauge
//
//	Paints the heat gauge

	{
	const CVisualPalette &VI = g_pHI->GetVisuals();
	const CG16bitFont &SmallFont = VI.GetFont(fontSmall);
	const CG16bitFont &MediumFont = VI.GetFont(fontMedium);

	//	Calculate fuel values

	Metric rHeatValue = ((Metric)pShip->GetHeatValue() / (Metric)pShip->GetMaxHeatValue());
	Metric rBoundedHeatValue = Min(1.0, ((Metric)pShip->GetHeatValue() / (Metric)pShip->GetMaxHeatValue()));

	//	Paint the background

	CGDraw::Arc(m_Buffer,
		CVector(m_xCenter, m_yCenter),
		m_iGaugeRadius + RING_SPACING + RING_SPACING + m_iGaugeWidth,
		0.5 * PI,
		1.5 * PI,
		m_iGaugeWidth,
		m_rgbGaugeBack,
		CGDraw::blendCompositeNormal,
		(m_iGaugeWidth + RING_SPACING) / 2,
		CGDraw::ARC_INNER_RADIUS);

	//	Figure out how we should paint

	CG32bitPixel rgbColor = CG32bitPixel((BYTE)(rBoundedHeatValue * 255), 64, (BYTE)(255 - (rBoundedHeatValue * 255)));

	CGDraw::Arc(m_Buffer,
		CVector(m_xCenter, m_yCenter),
		m_iGaugeRadius + RING_SPACING + RING_SPACING + m_iGaugeWidth,
		0.5 * PI + (PI * (0.95 - rBoundedHeatValue)),
		1.5 * PI,
		m_iGaugeWidth,
		rgbColor,
		CGDraw::blendCompositeNormal,
		(m_iGaugeWidth + RING_SPACING) / 2,
		CGDraw::ARC_INNER_RADIUS);

	//	Paint the fuel percent

	int cxWidth = MediumFont.MeasureText(CONSTLIT("100%")) + 4 * RING_SPACING;
	int cyHeight = MediumFont.GetHeight() + SmallFont.GetHeight();
	CVector vInnerPos = CVector::FromPolarInv(mathDegreesToRadians(210), m_iGaugeRadius + RING_SPACING + m_iGaugeWidth + RING_SPACING + m_iGaugeWidth + RING_SPACING);
	CVector vOuterPos = vInnerPos - CVector(cxWidth, 0.0);

	CGDraw::ArcQuadrilateral(m_Buffer, CVector(m_xCenter, m_yCenter), vInnerPos, vOuterPos, cyHeight, m_rgbGaugeBack, CGDraw::blendCompositeNormal);

	int xText = m_xCenter + (int)vInnerPos.GetX() - 3 * RING_SPACING;
	int yText = m_yCenter + (int)vInnerPos.GetY() - (cyHeight / 2);
	SmallFont.DrawText(m_Buffer, xText, yText, VI.GetColor(colorTextDialogLabel), CONSTLIT("Heat"), CG16bitFont::AlignRight);
	yText += SmallFont.GetHeight();

	MediumFont.DrawText(m_Buffer, xText, yText, VI.GetColor(colorTextHighlight), strPatternSubst(CONSTLIT("%d%%"), mathRound(100.0 * rHeatValue)), CG16bitFont::AlignRight);
	}

void CReactorHUDCircular::PaintFuelGauge (const SReactorStats &Stats, bool bCounterGaugePresent)

//	PaintFuelGauge
//
//	Paints the fuel gauge

	{
	const CVisualPalette &VI = g_pHI->GetVisuals();
	const CG16bitFont &SmallFont = VI.GetFont(fontSmall);
	const CG16bitFont &MediumFont = VI.GetFont(fontMedium);

	//	Calculate fuel values

	Metric rFuelLevel = Stats.iFuelLevel / 100.0;

	//	Paint the background

	CGDraw::Arc(m_Buffer,
			CVector(m_xCenter, m_yCenter),
			m_iGaugeRadius + RING_SPACING,
			0.5 * PI,
			1.5 * PI,
			m_iGaugeWidth,
			m_rgbGaugeBack,
			CGDraw::blendCompositeNormal,
			RING_SPACING / 2,
			CGDraw::ARC_INNER_RADIUS);

	//	Figure out how we should paint

	bool bWarning = (rFuelLevel < 0.15);
	CG32bitPixel rgbColor = (bWarning ? m_rgbGaugeWarning : m_rgbFuelGauge);

	//	If in warning mode, we blink

	if (!bWarning || (m_iTick % 2) == 0)
		{
		CGDraw::Arc(m_Buffer,
				CVector(m_xCenter, m_yCenter),
				m_iGaugeRadius + RING_SPACING,
				0.5 * PI + (PI * (1.0 - rFuelLevel)),
				1.5 * PI,
				m_iGaugeWidth,
				rgbColor,
				CGDraw::blendCompositeNormal,
				RING_SPACING / 2,
				CGDraw::ARC_INNER_RADIUS);
		}

	//	Paint the fuel percent

	int cxWidth = MediumFont.MeasureText(CONSTLIT("100%")) + 4 * RING_SPACING;
	int cyHeight = MediumFont.GetHeight() + SmallFont.GetHeight();
	int innerRadius = innerRadius = m_iGaugeRadius + RING_SPACING + m_iGaugeWidth + RING_SPACING;
	if (bCounterGaugePresent)
		innerRadius = m_iGaugeRadius + RING_SPACING + m_iGaugeWidth + RING_SPACING + m_iGaugeWidth + RING_SPACING;
	CVector vInnerPos = CVector::FromPolarInv(mathDegreesToRadians(150), innerRadius);
	CVector vOuterPos = vInnerPos - CVector(cxWidth, 0.0);

	CGDraw::ArcQuadrilateral(m_Buffer, CVector(m_xCenter, m_yCenter), vInnerPos, vOuterPos, cyHeight, m_rgbGaugeBack, CGDraw::blendCompositeNormal);

	int xText = m_xCenter + (int)vInnerPos.GetX() - 2 * RING_SPACING;
	int yText = m_yCenter + (int)vInnerPos.GetY() - (cyHeight / 2);
	SmallFont.DrawText(m_Buffer, xText, yText, VI.GetColor(colorTextDialogLabel), CONSTLIT("Fuel"), CG16bitFont::AlignRight);
	yText += SmallFont.GetHeight();

	MediumFont.DrawText(m_Buffer, xText, yText, VI.GetColor(colorTextHighlight), strPatternSubst(CONSTLIT("%d%%"), mathRound(100.0 * rFuelLevel)), CG16bitFont::AlignRight);
	}

void CReactorHUDCircular::PaintPowerGauge (const SReactorStats &Stats)

//	PaintPowerGauge
//
//	Paints the power consumption gauge

	{
	const CVisualPalette &VI = g_pHI->GetVisuals();
	const CG16bitFont &SmallFont = VI.GetFont(fontSmall);
	const CG16bitFont &MediumFont = VI.GetFont(fontMedium);

	//	Paint the background

	CGDraw::Arc(m_Buffer,
			CVector(m_xCenter, m_yCenter),
			m_iGaugeRadius + RING_SPACING,
			1.5 * PI,
			0.5 * PI,
			m_iGaugeWidth,
			m_rgbGaugeBack,
			CGDraw::blendCompositeNormal,
			RING_SPACING / 2,
			CGDraw::ARC_INNER_RADIUS);

	//	Compute power usage

	Metric rPowerUsage = Stats.iPowerConsumed;
	Metric rMaxPower = Stats.iReactorPower;
	Metric rPowerLevel = (rMaxPower > 0.0 ? Min(rPowerUsage / rMaxPower, 1.0) : 0.0);

	//	If we're getting some solar power, then we paint a different bar.

	if (Stats.iOtherPower > 0)
		{
		//	If we're getting all our power from solar, then we just need a single bar
		//	(with no chance of overload).

		if (Stats.iPowerConsumed <= Stats.iOtherPower)
			{
			CGDraw::Arc(m_Buffer,
					CVector(m_xCenter, m_yCenter),
					m_iGaugeRadius + RING_SPACING,
					1.5 * PI,
					mathAngleMod(1.5 * PI + (PI * rPowerLevel)),
					m_iGaugeWidth,
					m_rgbOtherPower,
					CGDraw::blendCompositeNormal,
					RING_SPACING / 2,
					CGDraw::ARC_INNER_RADIUS);
			}

		//	Otherwise we need to paint two bars.

		else
			{
			Metric rSolarLevel = (rMaxPower > 0.0 ? Min((Metric)Stats.iOtherPower / rMaxPower, 1.0) : 0.0);
			Metric rSolarArc = PI * rSolarLevel;
			Metric rStart = 1.5 * PI;

			CGDraw::Arc(m_Buffer,
					CVector(m_xCenter, m_yCenter),
					m_iGaugeRadius + RING_SPACING,
					rStart,
					mathAngleMod(rStart + rSolarArc),
					m_iGaugeWidth,
					m_rgbOtherPower,
					CGDraw::blendCompositeNormal,
					RING_SPACING / 2,
					CGDraw::ARC_INNER_RADIUS);

			rStart += rSolarArc;
			Metric rPowerArc = PI * (rPowerLevel - rSolarLevel);

			CGDraw::Arc(m_Buffer,
					CVector(m_xCenter, m_yCenter),
					m_iGaugeRadius + RING_SPACING,
					rStart,
					mathAngleMod(rStart + rPowerArc),
					m_iGaugeWidth,
					m_rgbPowerGauge,
					CGDraw::blendCompositeNormal,
					RING_SPACING / 2,
					CGDraw::ARC_INNER_RADIUS);
			}
		}

	//	Otherwise we just paint power consumption

	else
		{
		//	Figure out how we should paint

		bool bWarning = (rPowerLevel > 0.90);
		CG32bitPixel rgbColor = (bWarning ? m_rgbGaugeWarning : m_rgbPowerGauge);

		//	If in warning mode, we blink

		if (!bWarning || (m_iTick % 2) == 0)
			{
			CGDraw::Arc(m_Buffer,
					CVector(m_xCenter, m_yCenter),
					m_iGaugeRadius + RING_SPACING,
					1.5 * PI,
					mathAngleMod(1.5 * PI + (PI * rPowerLevel)),
					m_iGaugeWidth,
					rgbColor,
					CGDraw::blendCompositeNormal,
					RING_SPACING / 2,
					CGDraw::ARC_INNER_RADIUS);
			}
		}

	//	Paint the power percent

	int cxWidth = MediumFont.MeasureText(CONSTLIT("100%")) + 4 * RING_SPACING;
	int cyHeight = MediumFont.GetHeight() + SmallFont.GetHeight();
	CVector vInnerPos = CVector::FromPolarInv(mathDegreesToRadians(30), m_iGaugeRadius + RING_SPACING + m_iGaugeWidth + RING_SPACING);
	CVector vOuterPos = vInnerPos + CVector(cxWidth, 0.0);

	CGDraw::ArcQuadrilateral(m_Buffer, CVector(m_xCenter, m_yCenter), vInnerPos, vOuterPos, cyHeight, m_rgbGaugeBack, CGDraw::blendCompositeNormal);

	int xText = m_xCenter + (int)vInnerPos.GetX() + 2 * RING_SPACING;
	int yText = m_yCenter + (int)vInnerPos.GetY() - (cyHeight / 2);
	SmallFont.DrawText(m_Buffer, xText, yText, VI.GetColor(colorTextDialogLabel), CONSTLIT("Power"));
	yText += SmallFont.GetHeight();

	MediumFont.DrawText(m_Buffer, xText, yText, VI.GetColor(colorTextHighlight), strPatternSubst(CONSTLIT("%d%%"), mathRound(100.0 * rPowerLevel)));
	}

void CReactorHUDCircular::PaintReactorItem (const SReactorStats &Stats)

//	PaintReactorItem
//
//	Paints the name of the reactor item.

	{
	const CVisualPalette &VI = g_pHI->GetVisuals();
	const CG16bitFont &MediumFont = VI.GetFont(fontMedium);

	//	Paint the icon

	if (Stats.pReactorImage)
		{
		int cxSize = 4 * (m_iGaugeRadius / 3);
		Stats.pReactorImage->PaintScaledImage(m_Buffer, m_xCenter, m_yCenter, 0, 0, cxSize, cxSize, 0);
		}

	//	Paint the name

	CString sReactorName = strPatternSubst(CONSTLIT("%s (%s)"), 
			Stats.sReactorName,
			CLanguage::ComposeNumber(CLanguage::numberPower, Stats.iReactorPower * 100.0));

	int cyHeight;
	int cxWidth = MediumFont.MeasureText(sReactorName, &cyHeight) + 4 * RING_SPACING;
	cyHeight += RING_SPACING;

	if (Stats.Enhancements.GetCount() > 0)
		cyHeight += MediumFont.GetHeight();

	CVector vInnerPos = CVector::FromPolarInv(0.0, m_iGaugeRadius + RING_SPACING + m_iGaugeWidth + RING_SPACING);
	CVector vOuterPos = vInnerPos + CVector(Min(cxWidth, RIGHT_CONTROL_WIDTH), 0.0);

	int xText = m_xCenter + (int)vInnerPos.GetX() + 2 * RING_SPACING;
	int yText = m_yCenter + (int)vInnerPos.GetY() - (cyHeight / 2);

	CGDraw::ArcQuadrilateral(m_Buffer, CVector(m_xCenter, m_yCenter), vInnerPos, vOuterPos, cyHeight, m_rgbGaugeBack, CGDraw::blendCompositeNormal);

	CG32bitPixel rgbColor;
	if (Stats.bReactorDamaged)
		rgbColor = DAMAGED_TEXT_COLOR;
	else
		rgbColor = VI.GetColor(colorTextHighlight);

	MediumFont.DrawText(m_Buffer,
			xText,
			yText,
			rgbColor,
			sReactorName);

	if (Stats.Enhancements.GetCount() > 0)
		{
		CUIHelper Helper(*g_pHI);

		DWORD dwOptions = CUIHelper::OPTION_ALIGN_CENTER;
		Helper.PaintDisplayAttribs(m_Buffer, xText + (cxWidth / 2), yText + MediumFont.GetHeight(), Stats.Enhancements, dwOptions);
		}
	}

void CReactorHUDCircular::Realize (SHUDPaintCtx &Ctx)

//	Realize
//
//	Paint to back buffer

	{
	//	Skip if we don't have a ship

	CShip *pShip = Ctx.Source.AsShip();
	if (pShip == NULL)
		return;

	SReactorStats Stats;
	pShip->GetReactorStats(Stats);
	bool bCounterGaugePresent = (pShip->GetMaxHeatValue() > 0);

	//	Set up some metrics

	const CVisualPalette &VI = g_pHI->GetVisuals();
	const CG16bitFont &SmallFont = VI.GetFont(fontSmall);
	const CG16bitFont &MediumFont = VI.GetFont(fontMedium);

	//	Create the buffer, if necessary

	if (m_Buffer.IsEmpty())
		m_Buffer.Create(m_cxDisplay, m_cyDisplay, CG32bitImage::alpha8);

	m_Buffer.Set(CG32bitPixel::Null());

	//	Paint the circular background

	CGDraw::Circle(m_Buffer, m_xCenter, m_yCenter, m_iGaugeRadius, m_rgbGaugeBack);

	//	Paint the reactor item on top

	PaintReactorItem(Stats);

	//	Paint gauges

	if (bCounterGaugePresent)
		{
		PaintHeatGauge(pShip);
		}

	if (Stats.bUsesCharges)
		PaintChargesGauge(Stats);
	else
		PaintFuelGauge(Stats, bCounterGaugePresent);

	PaintPowerGauge(Stats);
	}
