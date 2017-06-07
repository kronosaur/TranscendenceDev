//	CArmorHUDRectangular.cpp
//
//	CArmorHUDRectangular class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

#define ARMOR_COLOR_ATTRIB						CONSTLIT("armorColor")
#define SHIELDS_COLOR_ATTRIB					CONSTLIT("shieldsColor")

const int CORNER_RADIUS =						8;
const int LABEL_SPACING =						8;
const int LINE_WIDTH =							2;

CArmorHUDRectangular::CArmorHUDRectangular (void) :
		m_bInvalid(true)

//	CArmorHUDRingSegments constructor

	{
	}

ALERROR CArmorHUDRectangular::Bind (SDesignLoadCtx &Ctx)

//	Bind
//
//	Bind design

	{
	return NOERROR;
	}

void CArmorHUDRectangular::GetBounds (int *retWidth, int *retHeight) const

//	GetBounds
//
//	Returns the size

	{
	*retWidth = m_cxDisplay;
	*retHeight = m_cyDisplay;
	}

ALERROR CArmorHUDRectangular::InitFromXML (SDesignLoadCtx &Ctx, CShipClass *pClass, CXMLElement *pDesc)

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

	m_cyRow = 20;

	//	Calculate metrics

	const CVisualPalette &VI = g_pHI->GetVisuals();
	const CG16bitFont &MediumFont = VI.GetFont(fontMedium);

	m_cxDisplay = m_cyRow * 12;
	m_cyDisplay = m_cyRow * 8;

	m_cxShipSize = 10 * m_cyRow;

	//	Measure out metrics for armor/shield integrity text

	m_cxMaxValue = MediumFont.MeasureText(CONSTLIT("100%"), &m_cyMaxValue);

	return NOERROR;
	}

void CArmorHUDRectangular::InitLabelDistribution (void)

//	InitLabelDistribution
//
//	Distributes the label around the display.

	{
	int i;

	int xCenter = m_cxDisplay / 2;
	int yCenter = m_cyDisplay / 2;

	int cxLabel = m_cxMaxValue;
	int cyLabel = m_cyMaxValue;
	int cxHalfLabel = cxLabel / 2;
	int cyHalfLabel = cyLabel / 2;

	//	Define two rows, one above and one below the ship image, where
	//	the labels will go. These metrics represent the mid-point of 
	//	the region.

	int cxHalfRegion = (m_cyRow * 5) / 2;
	int yAbove = yCenter - cxHalfRegion + (cyLabel / 2);
	int yBelow = yCenter + cxHalfRegion - (cyLabel / 2);

	//	Start by placing each label on a single row (either above or below)
	//	as close as possible to the thing it's labelling.
	//
	//	While we're at it, we generate two list of label indices, one for
	//	each row, sorted by x coordinate.

	TSortMap<int, int> TopRow;
	TSortMap<int, int> BottomRow;

	bool bNextBelow = true;
	for (i = 0; i < m_Labels.GetCount(); i++)
		{
		bool bMiddle = (Absolute(m_Labels[i].yPos - yCenter) <= 1);

		//	Decide whether this label should be on the top row or bottom row.

		bool bBottomRow;

		//	If the point is in the middle, then we alternate above and below.

		if (bMiddle)
			{
			bBottomRow = bNextBelow;
			bNextBelow = !bNextBelow;
			}

		//	Otherwise, we go to whichever row is closest

		else
			bBottomRow = m_Labels[i].yPos >= yCenter;

		//	We temporarily store the label center in the handle place. We'll
		//	update this later on.

		m_Labels[i].xHandle = m_Labels[i].xPos;
		m_Labels[i].yHandle = (bBottomRow ? yBelow : yAbove);

		//	Store the lable in one of the arrays

		if (bBottomRow)
			BottomRow.Insert(m_Labels[i].xPos, i);
		else
			TopRow.Insert(m_Labels[i].xPos, i);
		}

	//	Distribute the labels for each row. This will initialize xHandle,yHandle
	//	as the middle of each label block.

	InitLabelDistributionForRow(TopRow);
	InitLabelDistributionForRow(BottomRow);

	//	Initialize the label rects.

	for (i = 0; i < m_Labels.GetCount(); i++)
		{
		SLabelEntry &Label = m_Labels[i];

		Label.rcLabel.left = Label.xHandle - cxHalfLabel;
		Label.rcLabel.right = Label.rcLabel.left + cxLabel;
		Label.rcLabel.top = Label.yHandle - cyHalfLabel;
		Label.rcLabel.bottom = Label.rcLabel.top + cyLabel;

		//	Handle goes either on top or bottom of label, depending.

		if (Label.yHandle > yCenter)
			Label.yHandle = Label.rcLabel.top;
		else
			Label.yHandle = Label.rcLabel.bottom;
		}
	}

void CArmorHUDRectangular::InitLabelDistributionForRow (const TSortMap<int, int> &Row)

//	InitLabelDistributionForRow
//
//	Given a list of labels, distribute them horizontally so they don't overlap.

	{
	int i;

	int xCenter = m_cxDisplay / 2;
	int yCenter = m_cyDisplay / 2;

	int cxLabel = m_cxMaxValue;
	int cyLabel = m_cyMaxValue;

	int cxMinDist = cxLabel + LABEL_SPACING;
	int xMin = LABEL_SPACING + (cxLabel / 2);
	int xMax = m_cxDisplay - LABEL_SPACING - (cxLabel / 2);

	while (true)
		{
		bool bMovedSomething = false;

		//	Loop over each entry in the row and see if they overlap. If they do,
		//	then adjust.

		for (i = 0; i < Row.GetCount() - 1; i++)
			{
			SLabelEntry &ThisEntry = m_Labels[Row[i]];
			SLabelEntry &NextEntry = m_Labels[Row[i + 1]];

			//	Compute our distance from the next label. Since this is a sorted
			//	list, we know our next label is to the right.

			int cxDist = NextEntry.xHandle - ThisEntry.xHandle;

			//	If we don't overlap, then continue to the next pair

			if (cxDist >= cxMinDist)
				continue;

			//	If we are to the left of the center line, then we move ourselves
			//	further to the left.

			if (ThisEntry.xHandle < xCenter)
				{
				//	If we're already at the left edge, then we can't do anything

				if (ThisEntry.xHandle <= xMin)
					continue;

				//	Move ourselves so we don't overlap.

				ThisEntry.xHandle = Max(xMin, NextEntry.xHandle - cxMinDist);
				bMovedSomething = true;
				}

			//	Otherwise, we move the next entry to the right
			
			else
				{
				//	If the entry is already at the right edge, then we can't do
				//	anything.

				if (NextEntry.xHandle >= xMax)
					continue;

				//	Move the next entry further to the right. NOTE: Since we are
				//	to the right of center, the next entry is also to the right
				//	of center, so it should never bounce back and forth.

				NextEntry.xHandle = Min(ThisEntry.xHandle + cxMinDist, xMax);
				bMovedSomething = true;
				}
			}

		//	If nothing moved, then we're done. Either nothing overlaps or we're
		//	already at the limit.

		if (!bMovedSomething)
			break;
		}
	}

void CArmorHUDRectangular::InitShipSectionLabels (const TArray<CShip::SAttachedSectionInfo> &SectionInfo)

//	InitShipSectionLabels
//
//	Initializes labels for ship sections.

	{
	int i;

	int xCenter = m_cxDisplay / 2;
	int yCenter = m_cyDisplay / 2;

	//	Initialize the coordinates that the labels will point to.

	m_Labels.DeleteAll();
	m_Labels.InsertEmpty(SectionInfo.GetCount());
	for (i = 0; i < m_Labels.GetCount(); i++)
		{
		m_Labels[i].xPos = xCenter + (int)mathRound(SectionInfo[i].vPos.GetX() * m_cxShipSize);
		m_Labels[i].yPos = yCenter - (int)mathRound(SectionInfo[i].vPos.GetY() * m_cxShipSize);
		}

	//	Now distribute the labels appropriately (initializing the rect)

	InitLabelDistribution();

	}

void CArmorHUDRectangular::OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx)

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

void CArmorHUDRectangular::PaintArmorSegments (SHUDPaintCtx &Ctx, CShip *pShip)

//	PaintArmorSegments
//
//	Paints all armor segments and their current integrity.

	{
	}

void CArmorHUDRectangular::PaintShipSections (SHUDPaintCtx &Ctx, CShip *pShip)

//	PaintShipSections
//
//	Paints attached sections armor integrity.

	{
	int i;

	const CVisualPalette &VI = g_pHI->GetVisuals();
	const CG16bitFont &MediumFont = VI.GetFont(fontMedium);

	//	Get info on all attached sections

	TArray<CShip::SAttachedSectionInfo> SectionInfo;
	pShip->GetAttachedSectionInfo(SectionInfo);

	//	If we haven't yet initialized the label structure, do it now.

	if (m_Labels.GetCount() != SectionInfo.GetCount())
		InitShipSectionLabels(SectionInfo);

	//	Now paint each label.

	for (i = 0; i < m_Labels.GetCount(); i++)
		{
		const SLabelEntry &Label = m_Labels[i];
		const CShip::SAttachedSectionInfo &Section = SectionInfo[i];

		int iIntegrity = 100 * Section.iHP / Section.iMaxHP;
		CString sHP = strPatternSubst("%d%%", iIntegrity);

		CG32bitPixel rgbBack = m_rgbArmorTextBack;
		CG32bitPixel rgbText = m_rgbArmorText;

		//	Paint connection

		CGDraw::Line(m_Buffer, Label.xHandle, Label.yHandle, Label.xPos, Label.yPos, LINE_WIDTH, rgbBack);
		CGDraw::Circle(m_Buffer, Label.xPos, Label.yPos, LINE_WIDTH * 2, rgbBack);

		//	Paint label

		CGDraw::Fill(m_Buffer, Label.rcLabel.left, Label.rcLabel.top, RectWidth(Label.rcLabel), RectHeight(Label.rcLabel), rgbBack, CGDraw::blendCompositeNormal);
		MediumFont.DrawText(m_Buffer, Label.rcLabel, rgbText, sHP, 0, CG16bitFont::AlignCenter);
		}
	}

void CArmorHUDRectangular::Realize (SHUDPaintCtx &Ctx)

//	Realize
//
//	Paints to the buffer if invalid

	{
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

	//	Create the buffer, if necessary

	if (m_Buffer.IsEmpty())
		m_Buffer.Create(m_cxDisplay, m_cyDisplay, CG32bitImage::alpha8);

	m_Buffer.Set(CG32bitPixel::Null());

	//	Paint the ship

	int xCenter = m_cxDisplay / 2;
	int yCenter = m_cyDisplay / 2;
	pShip->GetClass()->PaintScaled(m_Buffer, xCenter, yCenter, m_cxShipSize, m_cxShipSize, 0, 0);
	CGDraw::RoundedRect(m_Buffer, 0, 0, m_cxDisplay, m_cyDisplay, CORNER_RADIUS, CG32bitPixel(CG32bitPixel::Desaturate(m_rgbArmor), 80), CGDraw::blendCompositeNormal);

	//	For ships with attached compartments, we use a totally different algorithm

	if (pShip->HasAttachedSections())
		PaintShipSections(Ctx, pShip);

	//	Otherwise, normal armor segments

	else
		PaintArmorSegments(Ctx, pShip);

#if 0

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
#endif
	}
