//	CArmorHUDShaped.cpp
//
//	CArmorHUDShaped class
//	Copyright (c) 2017 by Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

#define ARMOR_COLOR_ATTRIB						CONSTLIT("armorColor")
#define SHIELDS_COLOR_ATTRIB					CONSTLIT("shieldsColor")

const int RING_SPACING = 2;
const int INTER_SEGMENT_SPACING = 2;
const int NAME_SPACING_X = 20;
const int NAME_WIDTH = 130;

const CG32bitPixel DISABLED_LABEL_COLOR =	CG32bitPixel(128, 0, 0);

CArmorHUDShaped::CArmorHUDShaped (void) :
		m_bInvalid(true)

//	CArmorHUDShaped constructor

	{
	}

ALERROR CArmorHUDShaped::Bind (SDesignLoadCtx &Ctx)

//	Bind
//
//	Bind design

	{
	return NOERROR;
	}

void CArmorHUDShaped::GetBounds (int *retWidth, int *retHeight) const

//	GetBounds
//
//	Returns the size

	{
	*retWidth = m_cxDisplay;
	*retHeight = m_cyDisplay;
	}

ALERROR CArmorHUDShaped::InitFromXML (SDesignLoadCtx &Ctx, CShipClass *pClass, CXMLElement *pDesc)

//	InitFromXML
//
//	Initialize from XML

	{
	/*
	m_rgbArmor = ::LoadRGBColor(pDesc->GetAttribute(ARMOR_COLOR_ATTRIB), CG32bitPixel(255, 128, 0));
	m_rgbArmorText = CG32bitPixel::Fade(m_rgbArmor, CG32bitPixel(255, 255, 255), 80);
	m_rgbArmorTextBack = CG32bitPixel::Darken(m_rgbArmor, 128);
	m_rgbShields = ::LoadRGBColor(pDesc->GetAttribute(SHIELDS_COLOR_ATTRIB), CG32bitPixel(103, 204, 0));
	m_rgbShieldsText = CG32bitPixel::Fade(m_rgbShields, CG32bitPixel(255, 255, 255), 80);
	m_rgbShieldsTextBack = CG32bitPixel::Darken(m_rgbShields, 128);

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
	*/

	return NOERROR;
	}

void CArmorHUDShaped::OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx)

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

void CArmorHUDShaped::Realize (SHUDPaintCtx &Ctx)

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
	}
