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
	m_rgbShields = ::LoadRGBColor(pDesc->GetAttribute(SHIELDS_COLOR_ATTRIB), CG32bitPixel(103, 204, 0));

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
	CG32bitPixel rgbArmorBack = CG32bitPixel(m_rgbArmor, 128);
	CG32bitPixel rgbShieldsBack = CG32bitPixel(m_rgbShields, 128);

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
//		int iWhole = (iMaxHP == 0 ? 100 : (pArmor->GetHitPoints() * 100) / iMaxHP);
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
				CGDraw::blendNormal, 
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
				CGDraw::blendNormal,
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
