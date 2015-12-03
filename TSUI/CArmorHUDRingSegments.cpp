//	CArmorHUDRingSegments.cpp
//
//	CArmorHUDRingSegments class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

#define ARMOR_COLOR_ATTRIB						CONSTLIT("armorColor")
#define SHIELDS_COLOR_ATTRIB					CONSTLIT("shieldsColor")

const int RING_SPACING = 2;
const int INTER_SEGMENT_SPACING = 2;
const int NAME_SPACING_X = 20;
const int NAME_WIDTH = 130;

CArmorHUDRingSegments::CArmorHUDRingSegments (void) :
		m_bInvalid(true)

//	CArmorHUDRingSegments constructor

	{
	}

ALERROR CArmorHUDRingSegments::Bind (SDesignLoadCtx &Ctx)

//	Bind
//
//	Bind design

	{
	return NOERROR;
	}

void CArmorHUDRingSegments::DrawArmorIntegrity (CG32bitImage &Dest, int iAngle, int iRadius, const CString &sText)

//	DrawArmorIntegrity
//
//	Draws the armor integrity value on an arc background.

	{
	const CVisualPalette &VI = g_pHI->GetVisuals();
	const CG16bitFont &MediumFont = VI.GetFont(fontMedium);

	Metric rCenterAngle = ::mathDegreesToRadians(iAngle);

	//	We compute the metrics for the arc and the text

	int iArcRadius;
	Metric rArcStart;
	Metric rArcSpan;
	int iArcWidth;

	CVector vText;

	//	Figure out if we're drawing the text radially or along the arc.

	if (iAngle <= 45 || (iAngle >= 135 && iAngle <= 225) || iAngle >= 315)
		{
		iArcRadius = Max(10, iRadius - m_cxMaxValue);
		iArcWidth = m_cxMaxValue + RING_SPACING;

		//	Compute the smallest angle that will fit the height of the text
		//	at the narrow point of the arc.

		rArcSpan = (Metric)m_cyMaxValue / (Metric)iArcRadius;
		rArcStart = mathAngleMod(rCenterAngle - (0.5 * rArcSpan));

		//	Compute a point where we can draw the text. This point will be the top,
		//	center of the text.
		//
		//	Right side of the circle

		Metric rRadiusCenterText = Max(1.0, (Metric)iArcRadius + 0.5 * iArcWidth);
		Metric rAngleTextAdj = (0.5 * m_cyMaxValue) / rRadiusCenterText;
		if (iAngle <= 45 || iAngle >= 315)
			vText = CVector::FromPolarInv(rCenterAngle + rAngleTextAdj, rRadiusCenterText);

		//	Left side of circle

		else
			vText = CVector::FromPolarInv(rCenterAngle - rAngleTextAdj, rRadiusCenterText);
		}

	//	Otherwise, painting along the arc.

	else
		{
		iArcRadius = Max(10, iRadius - m_cyMaxValue);
		iArcWidth = m_cyMaxValue + 2 * RING_SPACING;

		//	Compute the smallest angle that will fit the length of the text
		//	at the narrow point of the arc.

		rArcSpan = (Metric)m_cxMaxValue / (Metric)iArcRadius;
		rArcStart = mathAngleMod(rCenterAngle - (0.5 * rArcSpan));

		//	Compute the text position

		if (iAngle < 180)
			vText = CVector::FromPolarInv(rCenterAngle, iArcRadius + ((iArcWidth - m_cyMaxValue) / 2) + m_cyMaxValue);
		else
			vText = CVector::FromPolarInv(rCenterAngle, iArcRadius + ((iArcWidth - m_cyMaxValue) / 2));
		}

	//	Draw the arc

	CGDraw::Arc(Dest, CVector(m_xCenter, m_yCenter), iArcRadius, rArcStart, mathAngleMod(rArcStart + rArcSpan), iArcWidth, m_rgbArmorTextBack, CGDraw::blendNormal, 0, CGDraw::ARC_INNER_RADIUS);

	//	Draw the text

	MediumFont.DrawText(m_Buffer,
			m_xCenter + (int)vText.GetX(),
			m_yCenter + (int)vText.GetY(),
			CG32bitPixel(255, 255, 255),
			sText,
			CG16bitFont::AlignCenter);
	}

void CArmorHUDRingSegments::GetBounds (int *retWidth, int *retHeight)

//	GetBounds
//
//	Returns the size

	{
	*retWidth = m_cxDisplay;
	*retHeight = m_cyDisplay;
	}

ALERROR CArmorHUDRingSegments::InitFromXML (SDesignLoadCtx &Ctx, CShipClass *pClass, CXMLElement *pDesc)

//	InitFromXML
//
//	Initialize from XML

	{
	m_rgbArmor = ::LoadRGBColor(pDesc->GetAttribute(ARMOR_COLOR_ATTRIB), CG32bitPixel(255, 128, 0));
	m_rgbArmorTextBack = CG32bitPixel::Darken(m_rgbArmor, 128);
	m_rgbShields = ::LoadRGBColor(pDesc->GetAttribute(SHIELDS_COLOR_ATTRIB), CG32bitPixel(103, 204, 0));
	m_rgbShieldsTextBack = CG32bitPixel::Darken(m_rgbShields, 128);

	//	LATER: Load this from definition

	m_iArmorRingRadius = 64;
	m_iArmorRingWidth = 20;
	m_iShieldRingWidth = 20;

	//	Calculate metrics

	const CVisualPalette &VI = g_pHI->GetVisuals();
	const CG16bitFont &MediumFont = VI.GetFont(fontMedium);

	int iTotalRadius = m_iArmorRingRadius + RING_SPACING + m_iShieldRingWidth;
	m_cxDisplay = (2 * iTotalRadius) + (2 * (NAME_SPACING_X + NAME_WIDTH));
	m_cyDisplay = (2 * iTotalRadius) + (3 * MediumFont.GetHeight());

	m_xCenter = m_cxDisplay / 2;
	m_yCenter = (iTotalRadius + 1);

	//	Measure out metrics for armor/shield integrity text

	m_cxMaxValue = MediumFont.MeasureText(CONSTLIT("100%"), &m_cyMaxValue);

	return NOERROR;
	}

void CArmorHUDRingSegments::Paint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx)

//	Paint
//
//	Paint

	{
	if (m_bInvalid)
		{
		Realize(Ctx);
		m_bInvalid = false;
		}

	Dest.Blt(0,
			0,
			m_Buffer.GetWidth(),
			m_Buffer.GetHeight(),
			255,
			m_Buffer,
			x,
			y);
	}

void CArmorHUDRingSegments::Realize (SHUDPaintCtx &Ctx)

//	Realize
//
//	Paints to the buffer if invalid

	{
	int i;

	//	Skip if we don't have a ship

	CShip *pShip;
	if (Ctx.pSource == NULL
			|| (pShip = Ctx.pSource->AsShip()) == NULL)
		return;

	//	Set up some metrics

	const CVisualPalette &VI = g_pHI->GetVisuals();

	const CG16bitFont &SmallFont = VI.GetFont(fontSmall);
	const CG16bitFont &MediumFont = VI.GetFont(fontMedium);
	CG32bitPixel rgbArmorBack = CG32bitPixel(CG32bitPixel::Desaturate(m_rgbArmor), 80);
	CG32bitPixel rgbShieldsBack = CG32bitPixel(CG32bitPixel::Desaturate(m_rgbShields), 80);

	//	Create the buffer, if necessary

	if (m_Buffer.IsEmpty())
		m_Buffer.Create(m_cxDisplay, m_cyDisplay, CG32bitImage::alpha8);

	m_Buffer.Set(CG32bitPixel::Null());

	//	Paint each of the armor segments, one at a time.

	int iArmorInnerRadius = m_iArmorRingRadius - m_iArmorRingWidth;

	for (i = 0; i < pShip->GetArmorSectionCount(); i++)
		{
		CShipClass::HullSection *pSect = pShip->GetClass()->GetHullSection(i);
		CInstalledArmor *pArmor = pShip->GetArmorSection(i);
		int iMaxHP = pArmor->GetMaxHP(pShip);
		int iWhole = (iMaxHP == 0 ? 100 : (pArmor->GetHitPoints() * 100) / iMaxHP);
		int iWidth = (iMaxHP == 0 ? 0 : (pArmor->GetHitPoints() * m_iArmorRingWidth) / iMaxHP);

		//	Draw the full armor size

		CGDraw::Arc(m_Buffer, 
				m_xCenter, 
				m_yCenter, 
				iArmorInnerRadius, 
				AngleMod(90 + pSect->iStartAt), 
				AngleMod(90 + pSect->iStartAt + pSect->iSpan), 
				m_iArmorRingWidth, 
				rgbArmorBack, 
				CGDraw::blendCompositeNormal, 
				INTER_SEGMENT_SPACING,
				CGDraw::ARC_INNER_RADIUS);

		//	Draw an arc representing how much armor we have

		CGDraw::Arc(m_Buffer, 
				m_xCenter, 
				m_yCenter, 
				iArmorInnerRadius, 
				AngleMod(90 + pSect->iStartAt), 
				AngleMod(90 + pSect->iStartAt + pSect->iSpan), 
				iWidth, 
				m_rgbArmor, 
				CGDraw::blendNormal, 
				INTER_SEGMENT_SPACING,
				CGDraw::ARC_INNER_RADIUS);

		//	Draw armor integrity box

		DrawArmorIntegrity(m_Buffer, AngleMod(90 + (pSect->iStartAt + (pSect->iSpan / 2))), iArmorInnerRadius + RING_SPACING, strPatternSubst(CONSTLIT("%d%%"), iWhole));
		}

	//	Paint shield level

	int iShieldHP = 0;
	int iShieldMaxHP = 0;
	CInstalledDevice *pShield = pShip->GetNamedDevice(devShields);
	if (pShield)
		pShield->GetStatus(pShip, &iShieldHP, &iShieldMaxHP);

	if (iShieldMaxHP > 0)
		{
		int iShieldInnerRadius = m_iArmorRingRadius + RING_SPACING;
		int iWidth = (iShieldMaxHP == 0 ? 0 : (iShieldHP * m_iShieldRingWidth) / iShieldMaxHP);

		//	Draw the full shields

		CGDraw::Arc(m_Buffer,
				m_xCenter,
				m_yCenter,
				iShieldInnerRadius,
				0,
				0,
				m_iShieldRingWidth,
				rgbShieldsBack,
				CGDraw::blendCompositeNormal,
				0,
				CGDraw::ARC_INNER_RADIUS);

		//	Draw the shield level

		CGDraw::Arc(m_Buffer,
				m_xCenter,
				m_yCenter,
				iShieldInnerRadius,
				0,
				0,
				iWidth,
				m_rgbShields,
				CGDraw::blendNormal,
				0,
				CGDraw::ARC_INNER_RADIUS);
		}



	}
