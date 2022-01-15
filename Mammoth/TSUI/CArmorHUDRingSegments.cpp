//	CArmorHUDRingSegments.cpp
//
//	CArmorHUDRingSegments class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

#define ARMOR_COLOR_ATTRIB						CONSTLIT("armorColor")
#define HP_ANGLE_ATTRIB							CONSTLIT("hpAngle")
#define LABEL_ANGLE_ATTRIB						CONSTLIT("labelAngle")
#define SCALE_ATTRIB							CONSTLIT("scale")
#define SEGMENT_ATTRIB							CONSTLIT("segment")
#define SHIELDS_COLOR_ATTRIB					CONSTLIT("shieldsColor")

#define SCALE_HP								CONSTLIT("hp")
#define SCALE_PERCENT							CONSTLIT("percent")

#define TAG_ARMOR_SECTION						CONSTLIT("ArmorSection")

const int RING_SPACING = 2;
const int INTER_SEGMENT_SPACING = 2;
const int NAME_SPACING_X = 20;
const int NAME_WIDTH = 130;

const CG32bitPixel DISABLED_LABEL_COLOR =	CG32bitPixel(128, 0, 0);

ALERROR CArmorHUDRingSegments::Bind (SDesignLoadCtx &Ctx)

//	Bind
//
//	Bind design

	{
	return NOERROR;
	}

void CArmorHUDRingSegments::CalcHPLabelBox (int iAngle, int iRadius, SSegment &Seg)

//	CalcHPLabelBox
//
//	Compose the position of the HP label.

	{
	Metric rCenterAngle = ::mathDegreesToRadians(iAngle);

	//	Figure out if we're drawing the text radially or along the arc.

	if (iAngle <= 45 || (iAngle >= 135 && iAngle <= 225) || iAngle >= 315)
		{
		Seg.bArcHPLabel = false;

		CVector vMidPoint = CVector::FromPolarInv(rCenterAngle, iRadius);
		int cxTotal = m_cxMaxValue + 2;
		int cxExtra = m_cxMaxValue / 2;
		int cxInner = cxTotal - cxExtra;

		//	Right side of circle

		if (iAngle <= 45 || iAngle >= 315)
			{
			Seg.vInnerPos = vMidPoint - CVector(cxInner, 0.0);
			Seg.vOuterPos = vMidPoint + CVector(cxExtra, 0.0);

			Seg.vHPText = Seg.vInnerPos + (0.5 * (Seg.vOuterPos - Seg.vInnerPos)) + CVector(0.0, -0.5 * m_cyMaxValue);
			Seg.rHPTextRotation = 0.0;
			}

		//	Left side of circle

		else
			{
			Seg.vInnerPos = vMidPoint + CVector(cxInner, 0.0);
			Seg.vOuterPos = vMidPoint - CVector(cxExtra, 0.0);

			Seg.vHPText = Seg.vInnerPos + (0.5 * (Seg.vOuterPos - Seg.vInnerPos)) + CVector(0.0, -0.5 * m_cyMaxValue);
			Seg.rHPTextRotation = 0.0;
			}
		}

	//	Otherwise, painting along the arc.

	else
		{
		Seg.bArcHPLabel = true;

		Seg.iArcRadius = Max(10, iRadius - m_cyMaxValue);
		Seg.iArcWidth = m_cyMaxValue;// +RING_SPACING;

		//	Compute the smallest angle that will fit the length of the text
		//	at the narrow point of the arc.

		Seg.rArcSpan = (Metric)m_cxMaxValue / (Metric)Seg.iArcRadius;
		Seg.rArcStart = mathAngleMod(rCenterAngle - (0.5 * Seg.rArcSpan));

		//	Compute the text position

		if (iAngle < 180)
			{
			int iTextRadius = Seg.iArcRadius + ((Seg.iArcWidth - m_cyMaxValue) / 2) + m_cyMaxValue;
			Seg.vHPText = CVector::FromPolarInv(rCenterAngle, iTextRadius);
			Seg.rHPTextRotation = rCenterAngle - (HALF_PI);
			}
		else
			{
			int iTextRadius = Seg.iArcRadius + ((Seg.iArcWidth - m_cyMaxValue) / 2) - 1;
			Seg.vHPText = CVector::FromPolarInv(rCenterAngle, iTextRadius);
			Seg.rHPTextRotation = rCenterAngle + (HALF_PI);
			}
		}
	}

void CArmorHUDRingSegments::DrawArmorName (CG32bitImage &Dest, int iAngle, int iRadius, CShip *pShip, CArmorItem ArmorItem, CG32bitPixel rgbBack, CG32bitPixel rgbColor)

	//	DrawArmorName
	//
	//	Draws the name of the given armor segment (and any enhancements).

	{
	DEBUG_TRY

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

	CString sName = ArmorItem.GetNounPhrase(nounNoModifiers | nounShort);
	CItemListManipulator ItemList(pShip->GetItemList());
	pShip->SetCursorAtArmor(ItemList, ArmorItem.GetSegmentIndex());

	TArray<SDisplayAttribute> Attribs;
	ArmorItem.AccumulateEnhancementDisplayAttributes(Attribs);

	//	Draw it

	DrawItemBox(Dest, iAngle, iRadius, sName, Attribs, rgbBack, rgbColor);

	DEBUG_CATCH
	}

void CArmorHUDRingSegments::DrawIntegrityBox (CG32bitImage &Dest, const SSegment &Seg, CG32bitPixel rgbBack) const

//	DrawIntegrityBox
//
//	Draws the armor integrity value on an arc background.

	{
	DEBUG_TRY

	CVector vCenter(m_xCenter, m_yCenter);

	if (Seg.bArcHPLabel)
		{
		CGDraw::Arc(Dest, 
				vCenter, 
				Seg.iArcRadius, 
				Seg.rArcStart, 
				mathAngleMod(Seg.rArcStart + Seg.rArcSpan), 
				Seg.iArcWidth, 
				rgbBack, 
				CGDraw::blendNormal, 
				0, 
				CGDraw::ARC_INNER_RADIUS);
		}
	else
		{
		CGDraw::ArcQuadrilateral(Dest,
				vCenter,
				Seg.vInnerPos,
				Seg.vOuterPos,
				m_cyMaxValue,
				rgbBack);
		}

	DEBUG_CATCH
	}

void CArmorHUDRingSegments::DrawIntegrityBoxText (CG32bitImage &Dest, const SSegment &Seg, CG32bitPixel rgbColor) const

//	DrawIntegrityBoxText
//
//	Draws the armor integrity value on an arc background.

	{
	DEBUG_TRY

	const CVisualPalette &VI = g_pHI->GetVisuals();
	const CG16bitFont &MediumFont = VI.GetFont(fontMedium);

	//	Draw the text

	CVector vCenter(m_xCenter, m_yCenter);
	CVector vText = vCenter + Seg.vHPText;

	if (Seg.rHPTextRotation == 0.0)
		MediumFont.DrawText(Dest, (int)vText.GetX(), (int)vText.GetY(), rgbColor, Seg.sHP, CG16bitFont::AlignCenter);
	else
		CGDraw::Text(Dest,
				vText,
				Seg.sHP,
				MediumFont,
				rgbColor,
				CGDraw::blendNormal,
				Seg.rHPTextRotation,
				CGDraw::TEXT_ALIGN_CENTER);

	DEBUG_CATCH
	}

void CArmorHUDRingSegments::DrawItemBox (CG32bitImage &Dest, int iAngle, int iRadius, const CString &sName, const TArray<SDisplayAttribute> &Attribs, CG32bitPixel rgbBack, CG32bitPixel rgbColor)

	//	DrawItemBox
	//
	//	Draws a box with an item name and (optionally) enhancement information.

	{
	DEBUG_TRY

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

	if (Attribs.GetCount() > 0)
		{
		CUIHelper Helper(*g_pHI);

		DWORD dwOptions = CUIHelper::OPTION_ALIGN_CENTER;

		Helper.PaintDisplayAttribs(Dest, (int)vTextPos.GetX(), (int)vTextPos.GetY() + cyText, Attribs, dwOptions);
		}

	DEBUG_CATCH
	}

void CArmorHUDRingSegments::DrawShieldsIntegrity (CG32bitImage &Dest, int iAngle, int iRadius, const CString &sHP, CG32bitPixel rgbBack, CG32bitPixel rgbColor) const

//	DrawShieldIntegrity
//
//	Draws the HP box.

	{
	const CVisualPalette &VI = g_pHI->GetVisuals();
	const CG16bitFont &MediumFont = VI.GetFont(fontMedium);

	CVector vCenter(m_xCenter, m_yCenter);
	Metric rCenterAngle = ::mathDegreesToRadians(iAngle);

	int iArcRadius = Max(10, iRadius - m_cyMaxValue);
	int iArcWidth = m_cyMaxValue;// +RING_SPACING;

	//	Compute the smallest angle that will fit the length of the text
	//	at the narrow point of the arc.

	Metric rArcSpan = (Metric)m_cxMaxValue / (Metric)iArcRadius;
	Metric rArcStart = mathAngleMod(rCenterAngle - (0.5 * rArcSpan));

	//	Draw

	CGDraw::Arc(Dest, 
			vCenter, 
			iArcRadius, 
			rArcStart, 
			mathAngleMod(rArcStart + rArcSpan), 
			iArcWidth, 
			rgbBack, 
			CGDraw::blendNormal, 
			0, 
			CGDraw::ARC_INNER_RADIUS);

	//	Compute the text position

	int iTextRadius = iArcRadius + ((iArcWidth - m_cyMaxValue) / 2) + m_cyMaxValue;
	CVector vText = vCenter + CVector::FromPolarInv(rCenterAngle, iTextRadius);
	MediumFont.DrawText(Dest, (int)vText.GetX(), (int)vText.GetY(), rgbColor, sHP, CG16bitFont::AlignCenter);
	}

void CArmorHUDRingSegments::DrawShieldsName (CG32bitImage &Dest, int iAngle, int iRadius, CShip *pShip, CDeviceItem ShieldItem, CG32bitPixel rgbBack, CG32bitPixel rgbColor)

//	DrawShieldsName
//
//	Draws the name and mods for the shields

	{
	DEBUG_TRY

	CString sName = ShieldItem.GetNounPhrase(nounNoModifiers);

	TArray<SDisplayAttribute> Attribs;
	ShieldItem.AccumulateEnhancementDisplayAttributes(Attribs);

	DrawItemBox(Dest, iAngle, iRadius, sName, Attribs, rgbBack, rgbColor);

	DEBUG_CATCH
	}

void CArmorHUDRingSegments::GetBounds (int *retWidth, int *retHeight) const

//	GetBounds
//
//	Returns the size

	{
	*retWidth = m_cxDisplay;
	*retHeight = m_cyDisplay;
	}

bool CArmorHUDRingSegments::OnCreate (SHUDCreateCtx &CreateCtx, CString *retsError)

//	InitFromXML
//
//	Initialize from XML

	{
	const CVisualPalette &VI = g_pHI->GetVisuals();
	const CG16bitFont &MediumFont = VI.GetFont(fontMedium);

	m_rgbArmor = ::LoadRGBColor(CreateCtx.Desc.GetAttribute(ARMOR_COLOR_ATTRIB), CG32bitPixel(255, 128, 0));
	m_rgbArmorText = CG32bitPixel::Fade(m_rgbArmor, CG32bitPixel(255, 255, 255), 80);
	m_rgbArmorTextBack = CG32bitPixel::Darken(m_rgbArmor, 128);
	m_rgbShields = ::LoadRGBColor(CreateCtx.Desc.GetAttribute(SHIELDS_COLOR_ATTRIB), CG32bitPixel(103, 204, 0));
	m_rgbShieldsText = CG32bitPixel::Fade(m_rgbShields, CG32bitPixel(255, 255, 255), 80);
	m_rgbShieldsTextBack = CG32bitPixel::Darken(m_rgbShields, 128);

	CString sScale = CreateCtx.Desc.GetAttribute(SCALE_ATTRIB);
	if (sScale.IsBlank() || strEquals(sScale, SCALE_PERCENT))
		m_HPDisplay = CLanguage::SHPDisplayOptions(CLanguage::EHPDisplay::Percent);
	else if (strEquals(sScale, SCALE_HP))
		m_HPDisplay = CLanguage::SHPDisplayOptions(CLanguage::EHPDisplay::HitPoints);
	else
		{
		if (retsError) *retsError = strPatternSubst(CONSTLIT("Invalid scale: %s"), sScale);
		return false;
		}

	//	LATER: Load this from definition

	m_iArmorRingRadius = 64;
	m_iArmorRingWidth = 20;
	m_iShieldRingWidth = 20;

	m_iArmorInnerRadius = m_iArmorRingRadius - m_iArmorRingWidth;
	m_iArmorNameRadius = m_iArmorRingRadius + RING_SPACING + m_iShieldRingWidth + RING_SPACING;

	//	Calculate metrics

	int iTotalRadius = m_iArmorRingRadius + RING_SPACING + m_iShieldRingWidth;
	m_cxDisplay = (2 * iTotalRadius) + (2 * (NAME_SPACING_X + NAME_WIDTH));
	m_cyDisplay = (2 * iTotalRadius) + MediumFont.GetHeight();

	m_xCenter = m_cxDisplay / 2;
	m_yCenter = (iTotalRadius + 1);

	//	Measure out metrics for armor/shield integrity text

	m_cxMaxValue = MediumFont.MeasureText(CONSTLIT("100%"), &m_cyMaxValue);

	//	Initialize armor segment metrics

	auto &ArmorDesc = CreateCtx.Class.GetArmorDesc();
	m_Segments.DeleteAll();
	m_Segments.InsertEmpty(ArmorDesc.GetCount());
	for (int i = 0; i < m_Segments.GetCount(); i++)
		{
		auto &SegmentDesc = ArmorDesc.GetSegment(i);

		m_Segments[i].iStartAngle = AngleMod(90 + SegmentDesc.GetStartAngle());
		m_Segments[i].iEndAngle = AngleMod(90 + SegmentDesc.GetStartAngle() + SegmentDesc.GetSpan());
		m_Segments[i].iLabelAngle = AngleMod(90 + SegmentDesc.GetCenterAngle());
		m_Segments[i].iHPLabelAngle = AngleMod(90 + SegmentDesc.GetCenterAngle());
		}

	//	See if the definition overrides some angles.

	for (int i = 0; i < CreateCtx.Desc.GetContentElementCount(); i++)
		{
		const CXMLElement &Child = *CreateCtx.Desc.GetContentElement(i);
		if (strEquals(Child.GetTag(), TAG_ARMOR_SECTION))
			{
			int iSeg = Child.GetAttributeInteger(SEGMENT_ATTRIB);
			if (iSeg < 0 || iSeg >= ArmorDesc.GetCount())
				{
				if (retsError) *retsError = strPatternSubst(CONSTLIT("Invalid segment ID: %d"), iSeg);
				return false;
				}

			int iValue;
			if (Child.FindAttributeInteger(HP_ANGLE_ATTRIB, &iValue))
				m_Segments[iSeg].iHPLabelAngle = AngleMod(90 + iValue);

			if (Child.FindAttributeInteger(LABEL_ANGLE_ATTRIB, &iValue))
				m_Segments[iSeg].iLabelAngle = AngleMod(90 + iValue);
			}
		}

	//	Calc metrics of HP label based on angle

	for (int i = 0; i < m_Segments.GetCount(); i++)
		{
		CalcHPLabelBox(m_Segments[i].iHPLabelAngle, m_iArmorInnerRadius + RING_SPACING, m_Segments[i]);
		}

	return true;
	}

void CArmorHUDRingSegments::OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx)

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

void CArmorHUDRingSegments::Realize (SHUDPaintCtx &Ctx)

//	Realize
//
//	Paints to the buffer if invalid

	{
	DEBUG_TRY

	//	Skip if we don't have a ship

	CShip *pShip = Ctx.Source.AsShip();
	if (pShip == NULL)
		return;

	//	Set up some metrics

	const CVisualPalette &VI = g_pHI->GetVisuals();

	const CG16bitFont &SmallFont = VI.GetFont(fontSmall);
	const CG16bitFont &MediumFont = VI.GetFont(fontMedium);
	CG32bitPixel rgbArmorBack = CG32bitPixel(20, 20, 20, 140);
	CG32bitPixel rgbShieldsBack = CG32bitPixel(20, 20, 20, 140);
	CG32bitPixel rgbSelectionBack = CG32bitPixel::ChangeHue(m_rgbArmorTextBack, 180);

	//	Create the buffer, if necessary

	if (m_Buffer.IsEmpty())
		m_Buffer.Create(m_cxDisplay, m_cyDisplay, CG32bitImage::alpha8);

	m_Buffer.Set(CG32bitPixel::Null());

	//	Paint the ship

	pShip->GetClass()->PaintScaled(m_Buffer, m_xCenter, m_yCenter, 2 * m_iArmorInnerRadius, 2 * m_iArmorInnerRadius, 90, 0);
	CGDraw::Circle(m_Buffer, m_xCenter, m_yCenter, m_iArmorInnerRadius, CG32bitPixel(CG32bitPixel::Desaturate(m_rgbArmor), 80), CGDraw::blendCompositeNormal);

	//	Paint each of the armor segments, one at a time.

	for (CArmorItem ArmorItem : pShip->GetArmorSystem())
		{
		SSegment &Seg = m_Segments[ArmorItem.GetSegmentIndex()];
		bool bSelected = (ArmorItem.GetSegmentIndex() == Ctx.iSegmentSelected);

		Seg.sHP = ArmorItem.GetHPDisplay(m_HPDisplay, &Seg.iIntegrity);
		int iWidth = (Seg.iIntegrity * m_iArmorRingWidth) / 100;

		//	Draw the full armor size

		CGDraw::Arc(m_Buffer, 
				m_xCenter, 
				m_yCenter, 
				m_iArmorInnerRadius, 
				Seg.iStartAngle, 
				Seg.iEndAngle, 
				m_iArmorRingWidth, 
				rgbArmorBack, 
				CGDraw::blendCompositeNormal, 
				INTER_SEGMENT_SPACING,
				CGDraw::ARC_INNER_RADIUS);

		//	Draw an arc representing how much armor we have

		CGDraw::Arc(m_Buffer, 
				m_xCenter, 
				m_yCenter, 
				m_iArmorInnerRadius, 
				Seg.iStartAngle, 
				Seg.iEndAngle, 
				iWidth, 
				m_rgbArmor, 
				CGDraw::blendCompositeNormal, 
				INTER_SEGMENT_SPACING,
				CGDraw::ARC_INNER_RADIUS);

		//	Draw armor text

		DrawArmorName(m_Buffer, 
				Seg.iLabelAngle, 
				m_iArmorNameRadius, 
				pShip, 
				ArmorItem,
				(bSelected ? rgbSelectionBack : m_rgbArmorTextBack),
				m_rgbArmorText);

		//	Draw armor integrity box

		if (!bSelected)
			DrawIntegrityBox(m_Buffer, Seg, m_rgbArmorTextBack);
		}

	//	Paint the selected HP box

	if (Ctx.iSegmentSelected != -1)
		{
		SSegment &Seg = m_Segments[Ctx.iSegmentSelected];

		DrawIntegrityBox(m_Buffer, Seg, rgbSelectionBack);
		}

	//	Paint the HP values

	for (int i = 0; i < m_Segments.GetCount(); i++)
		{
		SSegment &Seg = m_Segments[i];

		DrawIntegrityBoxText(m_Buffer, Seg, m_rgbArmorText);
		}

	//	Paint shield level

	CDeviceItem ShieldItem = pShip->GetNamedDeviceItem(devShields);
	if (ShieldItem)
		{
		int iShieldIntegrity;
		CString sHP = ShieldItem.GetHPDisplay(m_HPDisplay, &iShieldIntegrity);

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

		DrawShieldsIntegrity(m_Buffer, 
				90, 
				iShieldInnerRadius + m_iShieldRingWidth, 
				sHP, 
				m_rgbShieldsTextBack,
				m_rgbShieldsText);

		CG32bitPixel rgbText;
		if (ShieldItem.IsWorking())
			rgbText = m_rgbShieldsText;
		else
			rgbText = DISABLED_LABEL_COLOR;

		DrawShieldsName(m_Buffer,
				298,
				iShieldInnerRadius + m_iShieldRingWidth + RING_SPACING,
				pShip,
				ShieldItem,
				m_rgbShieldsTextBack,
				rgbText);
		}

	DEBUG_CATCH
	}
