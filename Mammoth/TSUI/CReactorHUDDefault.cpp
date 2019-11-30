//	CReactorHUDDefault.cpp
//
//	CReactorHUDDefault class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

#define FUEL_LEVEL_IMAGE_TAG				CONSTLIT("FuelLevelImage")
#define FUEL_LEVEL_TEXT_TAG					CONSTLIT("FuelLevelText")
#define FUEL_LOW_LEVEL_IMAGE_TAG			CONSTLIT("FuelLowLevelImage")
#define IMAGE_TAG							CONSTLIT("Image")
#define POWER_GEN_IMAGE_TAG					CONSTLIT("PowerGenImage")
#define POWER_LEVEL_IMAGE_TAG				CONSTLIT("PowerLevelImage")
#define POWER_LEVEL_TEXT_TAG				CONSTLIT("PowerLevelText")
#define REACTOR_DISPLAY_TAG					CONSTLIT("ReactorDisplay")
#define REACTOR_TEXT_TAG					CONSTLIT("ReactorText")

#define DEST_X_ATTRIB						CONSTLIT("destX")
#define DEST_Y_ATTRIB						CONSTLIT("destY")

#define ERR_REACTOR_DISPLAY_NEEDED			CONSTLIT("missing or invalid <ReactorDisplay> element")

const int DISPLAY_HEIGHT =					64;
const int DISPLAY_WIDTH =					256;
const int DISPLAY_OFFSET_X =				12;
const int DISPLAY_OFFSET_Y =				12;

const CG32bitPixel TEXT_COLOR =				CG32bitPixel(150, 180, 255);
const CG32bitPixel DAMAGED_TEXT_COLOR =		CG32bitPixel(128, 0, 0);

CReactorHUDDefault::CReactorHUDDefault (void) :
		m_bInvalid(true),
		m_iTick(0)

//	CReactorHUDDefault constructor

	{
	}

CReactorHUDDefault::~CReactorHUDDefault (void)

//	CReactorHUDDefault destructor

	{
	}

ALERROR CReactorHUDDefault::Bind (SDesignLoadCtx &Ctx)

//	Bind
//
//	Bind design

	{
	ALERROR error;

	if (error = m_ReactorImage.OnDesignLoadComplete(Ctx))
		return error;

	if (error = m_PowerLevelImage.OnDesignLoadComplete(Ctx))
		return error;

	if (error = m_PowerGenImage.OnDesignLoadComplete(Ctx))
		return error;

	if (error = m_FuelLevelImage.OnDesignLoadComplete(Ctx))
		return error;

	if (error = m_FuelLowLevelImage.OnDesignLoadComplete(Ctx))
		return error;

	return NOERROR;
	}

void CReactorHUDDefault::GetBounds (int *retWidth, int *retHeight) const

//	GetBounds
//
//	Returns the size of the HUD

	{
	*retWidth = DISPLAY_WIDTH;
	*retHeight = DISPLAY_HEIGHT;
	}

ALERROR CReactorHUDDefault::InitFromXML (SDesignLoadCtx &Ctx, CShipClass *pClass, CXMLElement *pDesc)

//	InitFromXML
//
//	Initialize definitions

	{
	ALERROR error;

	//	Background image

	if (error = m_ReactorImage.InitFromXML(Ctx,
			pDesc->GetContentElementByTag(IMAGE_TAG)))
		return ComposeLoadError(Ctx, ERR_REACTOR_DISPLAY_NEEDED);

	//	Power level image

	CXMLElement *pImage = pDesc->GetContentElementByTag(POWER_LEVEL_IMAGE_TAG);
	if (pImage == NULL || (error = m_PowerLevelImage.InitFromXML(Ctx, pImage)))
		return ComposeLoadError(Ctx, ERR_REACTOR_DISPLAY_NEEDED);

	m_xPowerLevelImage = pImage->GetAttributeInteger(DEST_X_ATTRIB);
	m_yPowerLevelImage = pImage->GetAttributeInteger(DEST_Y_ATTRIB);

	//	Power generation image

	pImage = pDesc->GetContentElementByTag(POWER_GEN_IMAGE_TAG);
	if (pImage)
		{
		if (error = m_PowerGenImage.InitFromXML(Ctx, pImage))
			return ComposeLoadError(Ctx, ERR_REACTOR_DISPLAY_NEEDED);

		m_xPowerGenImage = pImage->GetAttributeInteger(DEST_X_ATTRIB);
		m_yPowerGenImage = pImage->GetAttributeInteger(DEST_Y_ATTRIB);
		}
	else
		{
		if (m_PowerLevelImage.GetBitmapUNID() == DEFAULT_REACTOR_DISPLAY_IMAGE)
			{
			RECT rcRect;
			rcRect.left = 0;
			rcRect.top = 102;
			rcRect.right = 202;
			rcRect.bottom = 116;

			m_PowerGenImage.Init(Ctx.GetUniverse(), DEFAULT_REACTOR_DISPLAY_IMAGE, rcRect, 1, 1);
			m_xPowerGenImage = m_xPowerLevelImage;
			m_yPowerGenImage = m_yPowerLevelImage;
			}
		else
			{
			m_PowerGenImage = m_PowerLevelImage;
			m_xPowerGenImage = m_xPowerLevelImage;
			m_yPowerGenImage = m_yPowerLevelImage;
			}
		}

	//	Fuel level image

	pImage = pDesc->GetContentElementByTag(FUEL_LEVEL_IMAGE_TAG);
	if (pImage == NULL || (error = m_FuelLevelImage.InitFromXML(Ctx, pImage)))
		return ComposeLoadError(Ctx, ERR_REACTOR_DISPLAY_NEEDED);

	m_xFuelLevelImage = pImage->GetAttributeInteger(DEST_X_ATTRIB);
	m_yFuelLevelImage = pImage->GetAttributeInteger(DEST_Y_ATTRIB);

	//	Low fuel level image

	pImage = pDesc->GetContentElementByTag(FUEL_LOW_LEVEL_IMAGE_TAG);
	if (pImage == NULL || (error = m_FuelLowLevelImage.InitFromXML(Ctx, pImage)))
		return ComposeLoadError(Ctx, ERR_REACTOR_DISPLAY_NEEDED);

	if (error = InitRectFromElement(pDesc->GetContentElementByTag(REACTOR_TEXT_TAG),
			&m_rcReactorText))
		return ComposeLoadError(Ctx, ERR_REACTOR_DISPLAY_NEEDED);

	if (error = InitRectFromElement(pDesc->GetContentElementByTag(POWER_LEVEL_TEXT_TAG),
			&m_rcPowerLevelText))
		return ComposeLoadError(Ctx, ERR_REACTOR_DISPLAY_NEEDED);

	if (error = InitRectFromElement(pDesc->GetContentElementByTag(FUEL_LEVEL_TEXT_TAG),
			&m_rcFuelLevelText))
		return ComposeLoadError(Ctx, ERR_REACTOR_DISPLAY_NEEDED);

	return NOERROR;
	}

void CReactorHUDDefault::OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx)

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
			x + DISPLAY_OFFSET_X,
			y + DISPLAY_OFFSET_Y);

	DEBUG_CATCH
	}

void CReactorHUDDefault::OnUpdate (SHUDUpdateCtx &Ctx)

//	OnUpdate
//
//	Called every tick

	{
	//	Every 7 ticks we update

	if ((Ctx.iTick % 7) == 0)
		{
		m_iTick++;
		Invalidate();
		}
	}

void CReactorHUDDefault::Realize (SHUDPaintCtx &Ctx)

//	Realize
//
//	Update m_Buffer

	{
	//	Skip if we don't have a ship

	CShip *pShip;
	if (Ctx.pSource == NULL
			|| (pShip = Ctx.pSource->AsShip()) == NULL)
		return;

	SReactorStats Stats;
	pShip->GetReactorStats(Stats);

	//	Set up some metrics

	const CVisualPalette &VI = g_pHI->GetVisuals();
	const CG16bitFont &SmallFont = VI.GetFont(fontSmall);
	const CG16bitFont &MediumFont = VI.GetFont(fontMedium);

	int yOffset = (DISPLAY_HEIGHT - RectHeight(m_ReactorImage.GetImageRect())) / 2;

	//	Create the buffer, if necessary

	if (m_Buffer.IsEmpty())
		m_Buffer.Create(DISPLAY_WIDTH, DISPLAY_HEIGHT, CG32bitImage::alpha8);

	m_Buffer.Set(CG32bitPixel::Null());

	//	Paint the background

	m_ReactorImage.PaintImageUL(m_Buffer, 0, yOffset, 0, 0);

	//	Paint the fuel level

	CG32bitImage *pFuelImage = NULL;
	RECT rcSrcRect;
	bool bBlink;
	if (Stats.iFuelLevel < 15)
		{
		pFuelImage = &m_FuelLowLevelImage.GetImage(NULL_STR);
		rcSrcRect = m_FuelLowLevelImage.GetImageRect();
		bBlink = true;
		}
	else
		{
		pFuelImage = &m_FuelLevelImage.GetImage(NULL_STR);
		rcSrcRect = m_FuelLevelImage.GetImageRect();
		bBlink = false;
		}

	if (!bBlink || ((m_iTick % 2) == 0))
		{
		int cxFuelLevel = RectWidth(rcSrcRect) * Stats.iFuelLevel / 100;
		m_Buffer.Blt(rcSrcRect.left,
				rcSrcRect.top,
				cxFuelLevel,
				RectHeight(rcSrcRect),
				255,
				*pFuelImage,
				m_xFuelLevelImage,
				yOffset + m_yFuelLevelImage);
		}

	//	Paint fuel level text

	m_Buffer.Set(m_rcFuelLevelText.left,
			yOffset + m_rcFuelLevelText.top,
			RectWidth(m_rcFuelLevelText),
			RectHeight(m_rcFuelLevelText),
			CG32bitPixel::Null());

	CString sFuelLevel = strPatternSubst(CONSTLIT("fuel"));
	int cxWidth = SmallFont.MeasureText(sFuelLevel);
	SmallFont.DrawText(m_Buffer,
			m_rcFuelLevelText.left,
			yOffset + m_rcFuelLevelText.top,
			VI.GetColor(colorTextDialogLabel),
			sFuelLevel,
			0);

	//	Calculate the power level

	int iPowerLevel;
	if (Stats.iReactorPower)
		iPowerLevel = Min((Stats.iPowerConsumed * 100 / Stats.iReactorPower) + 1, 120);
	else
		iPowerLevel = 0;

	//	If we've got power generation, we paint that.

	if (Stats.iOtherPower > 0)
		{
		CG32bitImage *pGenImage = &m_PowerGenImage.GetImage(NULL_STR);
		RECT rcGenRect = m_PowerGenImage.GetImageRect();

		//	If we're not consuming any reactor power, then just paint that line.

		if (Stats.iOtherPower >= Stats.iPowerConsumed)
			{
			int iGenLevel = (Stats.iReactorPower > 0 ? Min((Stats.iPowerConsumed * 100 / Stats.iReactorPower) + 1, 120) : 0);
			int cxGenLevel = RectWidth(rcGenRect) * iGenLevel / 120;

			m_Buffer.Blt(rcGenRect.left,
					rcGenRect.top,
					cxGenLevel,
					RectHeight(rcGenRect),
					255,
					*pGenImage,
					m_xPowerGenImage,
					yOffset + m_yPowerGenImage);
			}

		//	Otherwise, we need to paint two lines

		else
			{
			//	Paint the power generation

			int iGenLevel = (Stats.iReactorPower > 0 ? Min((Stats.iOtherPower * 100 / Stats.iReactorPower) + 1, 120) : 0);
			int cxGenLevel = RectWidth(rcGenRect) * iGenLevel / 120;

			m_Buffer.Blt(rcGenRect.left,
					rcGenRect.top,
					cxGenLevel,
					RectHeight(rcGenRect),
					255,
					*pGenImage,
					m_xPowerGenImage,
					yOffset + m_yPowerGenImage);

			//	Paint the remainder as normal reactor power

			CG32bitImage *pPowerImage = &m_PowerLevelImage.GetImage(NULL_STR);
			rcSrcRect = m_PowerLevelImage.GetImageRect();
			int cxPowerLevel = (RectWidth(rcSrcRect) * iPowerLevel / 120);

			m_Buffer.Blt(rcSrcRect.left + cxGenLevel,
					rcSrcRect.top,
					cxPowerLevel - cxGenLevel,
					RectHeight(rcSrcRect),
					255,
					*pPowerImage,
					m_xPowerLevelImage + cxGenLevel,
					yOffset + m_yPowerLevelImage);
			}
		}

	//	Otherwise, just paint the normal power level

	else
		{
		bBlink = (iPowerLevel >= 100);

		if (!bBlink || ((m_iTick % 2) == 0))
			{
			CG32bitImage *pPowerImage = &m_PowerLevelImage.GetImage(NULL_STR);
			rcSrcRect = m_PowerLevelImage.GetImageRect();
			int cxPowerLevel = RectWidth(rcSrcRect) * iPowerLevel / 120;

			m_Buffer.Blt(rcSrcRect.left,
					rcSrcRect.top,
					cxPowerLevel,
					RectHeight(rcSrcRect),
					255,
					*pPowerImage,
					m_xPowerLevelImage,
					yOffset + m_yPowerLevelImage);
			}
		}

	//	Paint power level text

	m_Buffer.Set(m_rcPowerLevelText.left,
			yOffset + m_rcPowerLevelText.top,
			RectWidth(m_rcPowerLevelText),
			RectHeight(m_rcPowerLevelText),
			CG32bitPixel::Null());

	CString sPowerLevel = strPatternSubst(CONSTLIT("power usage"));
	int cyHeight;
	cxWidth = SmallFont.MeasureText(sPowerLevel, &cyHeight);
	SmallFont.DrawText(m_Buffer,
			m_rcPowerLevelText.left,
			yOffset + m_rcPowerLevelText.bottom - cyHeight,
			VI.GetColor(colorTextDialogLabel),
			sPowerLevel,
			0);

	//	Paint the reactor name (we paint on top of the levels)

	CG32bitPixel rgbColor;
	if (Stats.bReactorDamaged)
		rgbColor = DAMAGED_TEXT_COLOR;
	else
		rgbColor = VI.GetColor(colorTextHighlight);

	CString sReactorName = strPatternSubst(CONSTLIT("%s (%s)"), 
			Stats.sReactorName,
			CLanguage::ComposeNumber(CLanguage::numberPower, Stats.iReactorPower * 100.0));

	MediumFont.DrawText(m_Buffer,
			m_rcReactorText.left,
			yOffset + m_rcReactorText.top,
			rgbColor,
			sReactorName,
			0);
	}
