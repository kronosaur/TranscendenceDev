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

const CG32bitPixel DISABLED_LABEL_COLOR =	CG32bitPixel(128, 0, 0);

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

void CArmorHUDRingSegments::DrawArmorName (CG32bitImage &Dest, int iAngle, int iRadius, CShip *pShip, CInstalledArmor *pArmor, CG32bitPixel rgbBack, CG32bitPixel rgbColor)

	//	DrawArmorName
	//
	//	Draws the name of the given armor segment (and any enhancements).

	{
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

	//	Get the armor name and mods

	CString sName = pArmor->GetClass()->GetShortName();
	CItemListManipulator ItemList(pShip->GetItemList());
	pShip->SetCursorAtArmor(ItemList, pArmor->GetSect());
	CString sMods = ItemList.GetItemAtCursor().GetEnhancedDesc(pShip);

	//	Draw it

	DrawItemBox(Dest, iAngle, iRadius, sName, sMods, rgbBack, rgbColor);
	}

void CArmorHUDRingSegments::DrawIntegrityBox (CG32bitImage &Dest, int iAngle, int iRadius, const CString &sText, CG32bitPixel rgbBack, CG32bitPixel rgbColor)

//	DrawIntegrityBox
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
				rgbBack);
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

		CGDraw::Arc(Dest, CVector(m_xCenter, m_yCenter), iArcRadius, rArcStart, mathAngleMod(rArcStart + rArcSpan), iArcWidth, rgbBack, CGDraw::blendNormal, 0, CGDraw::ARC_INNER_RADIUS);
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

void CArmorHUDRingSegments::DrawItemBox (CG32bitImage &Dest, int iAngle, int iRadius, const CString &sName, const CString &sMods, CG32bitPixel rgbBack, CG32bitPixel rgbColor)

	//	DrawItemBox
	//
	//	Draws a box with an item name and (optionally) enhancement information.

	{
	const CVisualPalette &VI = g_pHI->GetVisuals();
	const CG16bitFont &MediumFont = VI.GetFont(fontMedium);
	CVector vCenter(m_xCenter, m_yCenter);

	Metric rAngle = mathDegreesToRadians(iAngle);

	//	Metrics

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

	CGDraw::ArcQuadrilateral(Dest, vCenter, vInnerPos, vOuterPos, cyText, rgbBack, CGDraw::blendCompositeNormal);

	//	Draw text

	CVector vTextPos = vCenter + (vInnerPos + 0.5 * (vOuterPos - vInnerPos)) - CVector(0.0, 0.5 * cyText);
	MediumFont.DrawText(Dest, (int)vTextPos.GetX(), (int)vTextPos.GetY(), rgbColor, sName, CG16bitFont::AlignCenter);

	//	Paint modifier

	int xMod = (int)vTextPos.GetX();
	int yMod = (int)vTextPos.GetY() + cyText;
	DrawModifier(Dest, xMod, yMod, sMods, locAlignCenter);
	}

void CArmorHUDRingSegments::DrawShieldsName (CG32bitImage &Dest, int iAngle, int iRadius, CShip *pShip, CInstalledDevice *pShields, CG32bitPixel rgbBack, CG32bitPixel rgbColor)

//	DrawShieldsName
//
//	Draws the name and mods for the shields

	{
	CString sName = pShields->GetClass()->GetName();
	CString sMods;

	if (pShields->GetEnhancements() != NULL)
		{
		CItemListManipulator ItemList(pShip->GetItemList());
		pShip->SetCursorAtNamedDevice(ItemList, devShields);
		sMods = pShields->GetEnhancedDesc(pShip, &ItemList.GetItemAtCursor());
		}

	DrawItemBox(Dest, iAngle, iRadius, sName, sMods, rgbBack, rgbColor);
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
	m_cyDisplay = (2 * iTotalRadius) + MediumFont.GetHeight();

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
			Ctx.byOpacity,
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
	CG32bitPixel rgbArmorBack = CG32bitPixel(20, 20, 20, 140);
	CG32bitPixel rgbShieldsBack = CG32bitPixel(20, 20, 20, 140);
	CG32bitPixel rgbSelectionBack = CG32bitPixel::ChangeHue(m_rgbArmorTextBack, 180);

	int iArmorInnerRadius = m_iArmorRingRadius - m_iArmorRingWidth;
	int iArmorNameRadius = m_iArmorRingRadius + RING_SPACING + m_iShieldRingWidth + RING_SPACING;

	//	Create the buffer, if necessary

	if (m_Buffer.IsEmpty())
		m_Buffer.Create(m_cxDisplay, m_cyDisplay, CG32bitImage::alpha8);

	m_Buffer.Set(CG32bitPixel::Null());

	//	Paint the ship

	pShip->GetClass()->PaintScaled(m_Buffer, m_xCenter, m_yCenter, 2 * iArmorInnerRadius, 2 * iArmorInnerRadius, 0, 0);
	CGDraw::Circle(m_Buffer, m_xCenter, m_yCenter, iArmorInnerRadius, CG32bitPixel(CG32bitPixel::Desaturate(m_rgbArmor), 80), CGDraw::blendCompositeNormal);

	//	Paint each of the armor segments, one at a time.

	for (i = 0; i < pShip->GetArmorSectionCount(); i++)
		{
		const CShipArmorSegmentDesc &Sect = pShip->GetClass()->GetHullSection(i);
		CInstalledArmor *pArmor = pShip->GetArmorSection(i);
		int iIntegrity = pArmor->GetHitPointsPercent(pShip);
		int iWidth = (iIntegrity * m_iArmorRingWidth) / 100;

		//	Draw the full armor size

		CGDraw::Arc(m_Buffer, 
				m_xCenter, 
				m_yCenter, 
				iArmorInnerRadius, 
				AngleMod(90 + Sect.GetStartAngle()), 
				AngleMod(90 + Sect.GetStartAngle() + Sect.GetSpan()), 
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
				AngleMod(90 + Sect.GetStartAngle()), 
				AngleMod(90 + Sect.GetStartAngle() + Sect.GetSpan()), 
				iWidth, 
				m_rgbArmor, 
				CGDraw::blendCompositeNormal, 
				INTER_SEGMENT_SPACING,
				CGDraw::ARC_INNER_RADIUS);

		//	Draw armor integrity box

        int iCenterAngle = AngleMod(90 + Sect.GetCenterAngle());
		DrawIntegrityBox(m_Buffer, 
				iCenterAngle, 
				iArmorInnerRadius + RING_SPACING, 
				strPatternSubst(CONSTLIT("%d%%"), iIntegrity), 
				(i == Ctx.iSegmentSelected ? rgbSelectionBack : m_rgbArmorTextBack),
				m_rgbArmorText);

		//	Draw armor text

		DrawArmorName(m_Buffer, 
				iCenterAngle, 
				iArmorNameRadius, 
				pShip, 
				pArmor,
				(i == Ctx.iSegmentSelected ? rgbSelectionBack : m_rgbArmorTextBack),
				m_rgbArmorText);
		}

	//	Paint shield level

	int iShieldIntegrity = (pShield ? pShield->GetHitPointsPercent(pShip) : -1);
	if (iShieldIntegrity != -1)
		{
		int iShieldInnerRadius = m_iArmorRingRadius + RING_SPACING;
		int iWidth = (iShieldIntegrity * m_iShieldRingWidth) / 100;

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
				CGDraw::blendCompositeNormal,
				0,
				CGDraw::ARC_INNER_RADIUS);

		//	Draw shield integrity box

		DrawIntegrityBox(m_Buffer, 
				90, 
				iShieldInnerRadius + m_iShieldRingWidth, 
				strPatternSubst(CONSTLIT("%d%%"), iShieldIntegrity), 
				m_rgbShieldsTextBack,
				m_rgbShieldsText);

		CG32bitPixel rgbText;
		if (pShield->IsEnabled() && !pShield->IsDamaged() && !pShield->IsDisrupted())
			rgbText = m_rgbShieldsText;
		else
			rgbText = DISABLED_LABEL_COLOR;

		DrawShieldsName(m_Buffer,
				298,
				iShieldInnerRadius + m_iShieldRingWidth + RING_SPACING,
				pShip,
				pShield,
				m_rgbShieldsTextBack,
				rgbText);
		}
	}
