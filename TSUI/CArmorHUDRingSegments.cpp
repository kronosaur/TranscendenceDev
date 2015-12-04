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

void CArmorHUDRingSegments::DrawArmorIntegrity (CG32bitImage &Dest, int iAngle, int iRadius, const CString &sText, CG32bitPixel rgbColor)

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
	Metric rTextRot;

	//	Figure out if we're drawing the text radially or along the arc.

	if (iAngle <= 45 || (iAngle >= 135 && iAngle <= 225) || iAngle >= 315)
		{
		CVector vInnerPos;
		CVector vOuterPos;
		CVector vMidPoint = CVector::FromPolarInv(rCenterAngle, iRadius);
		int cxTotal = m_cxMaxValue + 2;
		int cxExtra = m_cxMaxValue / 2;

		//	Right side of circle

		if (iAngle <= 45 || iAngle >= 315)
			{
			vInnerPos = vMidPoint - CVector(cxTotal - cxExtra, 0.0);
			vOuterPos = vMidPoint + CVector(cxExtra, 0.0);

			vText = vInnerPos + (0.5 * (vOuterPos - vInnerPos)) + CVector(0.0, -0.5 * m_cyMaxValue);
			rTextRot = 0.0;
			}

		//	Left side of circle

		else
			{
			vInnerPos = vMidPoint + CVector(cxTotal - cxExtra, 0.0);
			vOuterPos = vMidPoint - CVector(cxExtra, 0.0);

			vText = vInnerPos + (0.5 * (vOuterPos - vInnerPos)) + CVector(0.0, -0.5 * m_cyMaxValue);
			rTextRot = 0.0;
			}

		//	Draw the arc

		CVector vCenter(m_xCenter, m_yCenter);
		bool bRightSide = (iAngle <= 45 || iAngle >= 315);
		CGDraw::ArcQuadrilateral(Dest,
				vCenter,
				vInnerPos,
				vOuterPos,
				m_cyMaxValue,
				m_rgbArmorTextBack);
		}

	//	Otherwise, painting along the arc.

	else
		{
		iArcRadius = Max(10, iRadius - m_cyMaxValue);
		iArcWidth = m_cyMaxValue;// +RING_SPACING;

		//	Compute the smallest angle that will fit the length of the text
		//	at the narrow point of the arc.

		rArcSpan = (Metric)m_cxMaxValue / (Metric)iArcRadius;
		rArcStart = mathAngleMod(rCenterAngle - (0.5 * rArcSpan));

		//	Compute the text position

		if (iAngle < 180)
			{
			vText = CVector::FromPolarInv(rCenterAngle, iArcRadius + ((iArcWidth - m_cyMaxValue) / 2) + m_cyMaxValue);
			rTextRot = rCenterAngle - (HALF_PI);
			}
		else
			{
			vText = CVector::FromPolarInv(rCenterAngle, iArcRadius + ((iArcWidth - m_cyMaxValue) / 2) - 1);
			rTextRot = rCenterAngle + (HALF_PI);
			}

		//	Draw the arc

		CGDraw::Arc(Dest, CVector(m_xCenter, m_yCenter), iArcRadius, rArcStart, mathAngleMod(rArcStart + rArcSpan), iArcWidth, m_rgbArmorTextBack, CGDraw::blendNormal, 0, CGDraw::ARC_INNER_RADIUS);
		}

	//	Draw the text

	vText = vText + CVector(m_xCenter, m_yCenter);
	if (rTextRot == 0.0)
		MediumFont.DrawText(Dest, (int)vText.GetX(), (int)vText.GetY(), rgbColor, sText, CG16bitFont::AlignCenter);
	else
		CGDraw::Text(Dest,
				vText,
				sText,
				MediumFont,
				rgbColor,
				CGDraw::blendNormal,
				rTextRot,
				CGDraw::TEXT_ALIGN_CENTER);
	}

void CArmorHUDRingSegments::DrawArmorName (CG32bitImage &Dest, int iAngle, int iRadius, CShip *pShip, CInstalledArmor *pArmor)

	//	DrawArmorName
	//
	//	Draws the name of the given armor segment (and any enhancements).

	{
	const CVisualPalette &VI = g_pHI->GetVisuals();
	const CG16bitFont &MediumFont = VI.GetFont(fontMedium);
	CVector vCenter(m_xCenter, m_yCenter);

	//	We scale down the angle because we want the name to fit on the sides
	//	(not on top or below the circle).

	if (iAngle <= 90)
		iAngle = iAngle / 2;
	else if (iAngle > 270)
		iAngle = 360 - (360 - iAngle) / 2;
	else if (iAngle <= 180)
		iAngle = 180 - (180 - iAngle) / 2;
	else
		iAngle = 180 + (iAngle - 180) / 2;

	Metric rAngle = mathDegreesToRadians(iAngle);

	//	Metrics

	CString sName = pArmor->GetClass()->GetShortName();
	int cyText;
	int cxText = MediumFont.MeasureText(sName, &cyText);
	int cxBackground = cxText + 4 * RING_SPACING;

	//	Paint the background

	CVector vInnerPos = CVector::FromPolarInv(rAngle, iRadius);
	CVector vOuterPos;
	if (iAngle <= 90 || iAngle >= 270)
		vOuterPos = vInnerPos + CVector(cxBackground, 0.0);
	else
		vOuterPos = vInnerPos - CVector(cxBackground, 0.0);

	CGDraw::ArcQuadrilateral(Dest, vCenter, vInnerPos, vOuterPos, cyText, m_rgbArmorTextBack);

	//	Draw text

	CVector vTextPos = vCenter + (vInnerPos + 0.5 * (vOuterPos - vInnerPos)) - CVector(0.0, 0.5 * cyText);
	MediumFont.DrawText(Dest, (int)vTextPos.GetX(), (int)vTextPos.GetY(), m_rgbArmorText, sName, CG16bitFont::AlignCenter);

	//	Paint modifier

	CItemListManipulator ItemList(pShip->GetItemList());
	pShip->SetCursorAtArmor(ItemList, pArmor->GetSect());
	CString sMods = ItemList.GetItemAtCursor().GetEnhancedDesc(pShip);
	int xMod = (int)vTextPos.GetX();
	int yMod = (int)vTextPos.GetY() + cyText;
	DrawModifier(Dest, xMod, yMod, sMods, locAlignCenter);
	}

void CArmorHUDRingSegments::GetBounds (int *retWidth, int *retHeight) const

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
	m_rgbArmorText = CG32bitPixel::Fade(m_rgbArmor, CG32bitPixel(255, 255, 255), 80);
	m_rgbArmorTextBack = CG32bitPixel::Darken(m_rgbArmor, 128);
	m_rgbShields = ::LoadRGBColor(pDesc->GetAttribute(SHIELDS_COLOR_ATTRIB), CG32bitPixel(103, 204, 0));
	m_rgbShieldsText = CG32bitPixel::Fade(m_rgbShields, CG32bitPixel(255, 255, 255), 80);
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

void CArmorHUDRingSegments::OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx)

//	OnPaint
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

	CInstalledDevice *pShield = pShip->GetNamedDevice(devShields);

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
	int iArmorNameRadius = m_iArmorRingRadius + RING_SPACING + m_iShieldRingWidth + RING_SPACING;

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

		int iCenterAngle = AngleMod(90 + (pSect->iStartAt + (pSect->iSpan / 2)));
		DrawArmorIntegrity(m_Buffer, iCenterAngle, iArmorInnerRadius + RING_SPACING, strPatternSubst(CONSTLIT("%d%%"), iWhole), m_rgbArmorText);

		//	Draw armor text

		DrawArmorName(m_Buffer, iCenterAngle, iArmorNameRadius, pShip, pArmor);
		}

	//	Paint shield level

	int iShieldHP = 0;
	int iShieldMaxHP = 0;
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
