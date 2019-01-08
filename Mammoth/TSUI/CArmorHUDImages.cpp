//	CArmorHUDImages.cpp
//
//	CArmorHUDImages class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

#define ARMOR_SECTION_TAG						CONSTLIT("ArmorSection")
#define SHIP_IMAGE_TAG							CONSTLIT("ShipImage")

#define DEST_X_ATTRIB							CONSTLIT("destX")
#define DEST_Y_ATTRIB							CONSTLIT("destY")
#define HP_X_ATTRIB								CONSTLIT("hpX")
#define HP_Y_ATTRIB								CONSTLIT("hpY")
#define NAME_ATTRIB								CONSTLIT("name")
#define NAME_BREAK_WIDTH						CONSTLIT("nameBreakWidth")
#define NAME_DEST_X_ATTRIB						CONSTLIT("nameDestX")
#define NAME_DEST_Y_ATTRIB						CONSTLIT("nameDestY")
#define NAME_Y_ATTRIB							CONSTLIT("nameY")
#define SEGMENT_ATTRIB							CONSTLIT("segment")

#define ERR_ARMOR_DISPLAY_NEEDED				CONSTLIT("missing or invalid <ArmorDisplay> element")
#define ERR_SHIP_IMAGE_NEEDED					CONSTLIT("invalid <ShipImage> element")
#define ERR_UNKNOWN_ELEMENT						CONSTLIT("unknown <ArmorDisplay> subelement: %s")
#define ERR_INSUFFICIENT_SEGMENTS				CONSTLIT("not enough <ArmorSection> elements")

const int DISPLAY_WIDTH = 360;
const int DISPLAY_HEIGHT = 136;
const int ARMOR_IMAGE_WIDTH = 136;
const int DESCRIPTION_WIDTH = (DISPLAY_WIDTH - ARMOR_IMAGE_WIDTH);
const int ARMOR_ENHANCE_X = 224;

const int HP_DISPLAY_WIDTH = 26;
const int HP_DISPLAY_HEIGHT = 14;

const CG32bitPixel ARMOR_LINE_COLOR = CG32bitPixel(0, 23, 167);
const CG32bitPixel HP_DISPLAY_BACK_COLOR = CG32bitPixel(0, 23, 167);
const CG32bitPixel HP_DISPLAY_TEXT_COLOR = CG32bitPixel(150, 180, 255);

CArmorHUDImages::CArmorHUDImages (void) :
		m_bInvalid(true)

//	CArmorHUDImages constructor

	{
	}

ALERROR CArmorHUDImages::Bind (SDesignLoadCtx &Ctx)

//	Bind
//
//	Bind design

	{
	ALERROR error;
	int i;

	if (error = m_ShipImage.OnDesignLoadComplete(Ctx))
		return error;

	for (i = 0; i < m_Segments.GetCount(); i++)
		{
		SArmorSegmentImageDesc &ArmorDesc = m_Segments[i];

		if (error = ArmorDesc.Image.OnDesignLoadComplete(Ctx))
			return error;
		}

	return NOERROR;
	}

int CArmorHUDImages::GetArmorSegment (SDesignLoadCtx &Ctx, CShipClass *pClass, CXMLElement *pDesc) const

//	GetArmorSegment
//
//	While loading the armor segment descriptors, returns the segment number for 
//	this descriptor.

	{
	//	For API versions 21 and higher, we figure out the segment version based
	//	on an explicit attribute.

	if (Ctx.GetAPIVersion() >= 21)
		{
		int iSegment = pDesc->GetAttributeIntegerBounded(SEGMENT_ATTRIB, 0, -1, -1);
		return (iSegment != -1 ? iSegment : m_Segments.GetCount());
		}

	//	Otherwise, it is based on the order of elements in the XML

	else
		return m_Segments.GetCount();
	}

void CArmorHUDImages::GetBounds (int *retWidth, int *retHeight) const

//	GetBounds
//
//	Returns the desired width/height of this display

	{
	*retWidth = DISPLAY_WIDTH;
	*retHeight = DISPLAY_HEIGHT;
	}

ALERROR CArmorHUDImages::InitFromXML (SDesignLoadCtx &Ctx, CShipClass *pClass, CXMLElement *pDesc)

//	InitFromXML
//
//	Initializes from XML

	{
	ALERROR error;
	int i;

	//	Loop over all sub elements

	for (i = 0; i < pDesc->GetContentElementCount(); i++)
		{
		CXMLElement *pSub = pDesc->GetContentElement(i);

		if (strEquals(pSub->GetTag(), ARMOR_SECTION_TAG))
			{
			int iSegment = GetArmorSegment(Ctx, pClass, pSub);
			SArmorSegmentImageDesc &ArmorDesc = *m_Segments.SetAt(iSegment);

			if (error = ArmorDesc.Image.InitFromXML(Ctx, pSub))
				return ComposeLoadError(Ctx, ERR_ARMOR_DISPLAY_NEEDED);

			ArmorDesc.sName = pSub->GetAttribute(NAME_ATTRIB);
			ArmorDesc.xDest = pSub->GetAttributeInteger(DEST_X_ATTRIB);
			ArmorDesc.yDest = pSub->GetAttributeInteger(DEST_Y_ATTRIB);
			ArmorDesc.xHP = pSub->GetAttributeInteger(HP_X_ATTRIB);
			ArmorDesc.yHP = pSub->GetAttributeInteger(HP_Y_ATTRIB);
			ArmorDesc.yName = pSub->GetAttributeInteger(NAME_Y_ATTRIB);
			ArmorDesc.cxNameBreak = pSub->GetAttributeInteger(NAME_BREAK_WIDTH);
			ArmorDesc.xNameDestOffset = pSub->GetAttributeInteger(NAME_DEST_X_ATTRIB);
			ArmorDesc.yNameDestOffset = pSub->GetAttributeInteger(NAME_DEST_Y_ATTRIB);
			}
		else if (strEquals(pSub->GetTag(), SHIP_IMAGE_TAG))
			{
			if (error = m_ShipImage.InitFromXML(Ctx, pSub))
				return ComposeLoadError(Ctx, ERR_SHIP_IMAGE_NEEDED);
			}
		else
			return ComposeLoadError(Ctx, strPatternSubst(ERR_UNKNOWN_ELEMENT, pSub->GetTag()));
		}

	//	Make sure we have valid data

	if (m_Segments.GetCount() != pClass->GetHullSectionCount())
		{
		if (pClass->GetAPIVersion() >= 29)
			return ComposeLoadError(Ctx, ERR_INSUFFICIENT_SEGMENTS);
		}

	return NOERROR;
	}

void CArmorHUDImages::OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx)

//	OnPaint
//
//	Paint to the destination bitmap

	{
	DEBUG_TRY

	int i;

	if (m_bInvalid)
		Realize(Ctx);

	Dest.Blt(0,
			0,
			m_Buffer.GetWidth(),
			m_Buffer.GetHeight(),
			Ctx.byOpacity,
			m_Buffer,
			x,
			y);

	//	We paint some text on top so that we get the proper antialiasing

	for (i = 0; i < m_Text.GetCount(); i++)
		{
		STextPaint *pPaint = &m_Text[i];
		pPaint->pFont->DrawText(Dest,
				x + pPaint->x,
				y + pPaint->y,
				pPaint->rgbColor,
				pPaint->sText);
		}

	//	Let the shields paint too, if they need antialiasing

	if (Ctx.pShieldsHUD)
		{
		SHUDPaintCtx::EPaintModes iOldMode = Ctx.iMode;
		Ctx.iMode = SHUDPaintCtx::paintOverBuffer;

		Ctx.pShieldsHUD->Paint(Dest, x, y, Ctx);

		Ctx.iMode = iOldMode;
		}

	DEBUG_CATCH
	}

void CArmorHUDImages::Realize (SHUDPaintCtx &Ctx)

//	Realize
//
//	Initialize all runtime elements if invalid.

	{
	int i;

	//	If we're already valid, then we're done

	if (!m_bInvalid)
		return;

	//	Create the buffer, if necessary

	if (m_Buffer.IsEmpty())
		m_Buffer.Create(DISPLAY_WIDTH, DISPLAY_HEIGHT, CG32bitImage::alpha8);

	m_Buffer.Set(CG32bitPixel::Null());

	//	Reset the text buffer. Since these lines are painted on top of the
	//	SRS, we have to wait to paint them on each frame so that anti-aliasing 
	//	works correctly.

	m_Text.DeleteAll();

	//	We're valid (even if we exit early)

	m_bInvalid = true;

	//	Skip if we don't have a ship

	CShip *pShip;
	if (Ctx.pSource == NULL
			|| (pShip = Ctx.pSource->AsShip()) == NULL)
		return;

	const CVisualPalette &VI = g_pHI->GetVisuals();

	const CG16bitFont &SmallFont = VI.GetFont(fontSmall);
	const CG16bitFont &MediumFont = VI.GetFont(fontMedium);
	CG32bitPixel SelectBackground = CG32bitPixel::Darken(VI.GetColor(colorAreaDialogHighlight), 128);

	CItemListManipulator ItemList(pShip->GetItemList());
	m_Text.DeleteAll();

	//	Draw the base ship image, if we have it

	if (m_ShipImage.IsLoaded())
		{
		const RECT &rcShip = m_ShipImage.GetImageRect();

		//	For backwards compatibility, we blt with black as a back color if
		//	we don't have a mask.

		CGDraw::BltWithBackColor(m_Buffer,
				DESCRIPTION_WIDTH + ((ARMOR_IMAGE_WIDTH - RectWidth(rcShip)) / 2),
				(DISPLAY_HEIGHT - RectHeight(rcShip)) / 2,
				m_ShipImage.GetImage(NULL_STR),
				rcShip.left, 
				rcShip.top, 
				RectWidth(rcShip), 
				RectHeight(rcShip),
				CG32bitPixel(0, 0, 0));
		}

	//	Draw armor

	for (i = 0; i < pShip->GetArmorSectionCount(); i++)
		{
		const SArmorSegmentImageDesc *pImage = m_Segments.GetAt(i);
		if (pImage == NULL)
			continue;

		CInstalledArmor *pArmor = pShip->GetArmorSection(i);
		int iMaxHP = pArmor->GetMaxHP(pShip);
		int iWhole = (iMaxHP == 0 ? 100 : (pArmor->GetHitPoints() * 100) / iMaxHP);
		int iIndex = (100 - iWhole) / 20;
		
		if (iIndex < 5)
			{
			const RECT &rcImage = pImage->Image.GetImageRect();

			CGDraw::BltWithBackColor(m_Buffer,
					DESCRIPTION_WIDTH + pImage->xDest,
					pImage->yDest,
					pImage->Image.GetImage(NULL_STR),
					rcImage.left,
					rcImage.top + iIndex * RectHeight(rcImage),
					RectWidth(rcImage),
					RectHeight(rcImage),
					CG32bitPixel(0, 0, 0));
			}
		}

	//	Draw the shields

	if (Ctx.pShieldsHUD)
		{
		SHUDPaintCtx::EPaintModes iOldMode = Ctx.iMode;
		Ctx.iMode = SHUDPaintCtx::paintToBuffer;

		Ctx.pShieldsHUD->Paint(m_Buffer, 0, 0, Ctx);

		Ctx.iMode = iOldMode;
		}

	//	Draw armor names

	for (i = 0; i < pShip->GetArmorSectionCount(); i++)
		{
		const SArmorSegmentImageDesc *pImage = m_Segments.GetAt(i);
		if (pImage == NULL)
			continue;

		CInstalledArmor *pArmor = pShip->GetArmorSection(i);
		CItemCtx ItemCtx(pShip, pArmor);

		//	Paint the HPs

		if (i == Ctx.iSegmentSelected)
			{
			m_Buffer.Fill(DESCRIPTION_WIDTH + pImage->xHP - 1, 
					pImage->yHP - 1, 
					HP_DISPLAY_WIDTH + 2, 
					HP_DISPLAY_HEIGHT + 2,
					SelectBackground);
			}
		else
			{
			m_Buffer.Fill(DESCRIPTION_WIDTH + pImage->xHP, 
					pImage->yHP, 
					HP_DISPLAY_WIDTH, 
					HP_DISPLAY_HEIGHT,
					HP_DISPLAY_BACK_COLOR);
			}

		CString sHP = strFromInt(pArmor->GetHitPoints());
		int cxWidth = MediumFont.MeasureText(sHP, NULL);
		MediumFont.DrawText(m_Buffer,
				DESCRIPTION_WIDTH + pImage->xHP + (HP_DISPLAY_WIDTH - cxWidth) / 2,
				pImage->yHP - 1,
				VI.GetColor(colorTextHighlight),
				sHP);

		//	Paint the armor name line

		CGDraw::LineBroken(m_Buffer,
				0,
				pImage->yName + MediumFont.GetHeight(),
				DESCRIPTION_WIDTH + pImage->xHP + pImage->xNameDestOffset,
				pImage->yHP + pImage->yNameDestOffset,
				pImage->cxNameBreak,
				(i == Ctx.iSegmentSelected ? SelectBackground : ARMOR_LINE_COLOR));

		//	Paint the armor names

		CString sName = pArmor->GetClass()->GetShortName();
		int cy;
		int cx = MediumFont.MeasureText(sName, &cy) + 4;
		if (i == Ctx.iSegmentSelected)
			{
			m_Buffer.Fill(0, 
					pImage->yName, 
					cx, 
					cy,
					SelectBackground);
			}

		STextPaint *pPaint = m_Text.Insert();
		pPaint->sText = sName;
		pPaint->x = 2;
		pPaint->y = pImage->yName;
		pPaint->pFont = &MediumFont;
		pPaint->rgbColor = VI.GetColor(colorTextHighlight);

		//	Paint the modifiers

		TArray<SDisplayAttribute> Attribs;
		if (ItemCtx.GetEnhancementDisplayAttributes(&Attribs))
			{
			CUIHelper Helper(*g_pHI);

			DWORD dwOptions = CUIHelper::OPTION_ALIGN_BOTTOM 
					| CUIHelper::OPTION_ALIGN_RIGHT;

			Helper.PaintDisplayAttribs(m_Buffer, ARMOR_ENHANCE_X, pImage->yName + MediumFont.GetHeight(), Attribs, dwOptions);
			}
		}
	}
