//	IHUDPainter.cpp
//
//	IHUDPainter class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

#define HEIGHT_ATTRIB							CONSTLIT("height")
#define WIDTH_ATTRIB							CONSTLIT("width")
#define X_ATTRIB								CONSTLIT("x")
#define Y_ATTRIB								CONSTLIT("y")

#define STYLE_ATTRIB							CONSTLIT("style")

#define STYLE_DEFAULT							CONSTLIT("default")
#define STYLE_CIRCULAR							CONSTLIT("circular")
#define STYLE_RECTANGULAR						CONSTLIT("rectangular")
#define STYLE_SHAPED							CONSTLIT("shaped")

const CXMLElement IHUDPainter::m_NullDesc;

IHUDPainter *IHUDPainter::Create (EHUDTypes iType, CUniverse &Universe, const CVisualPalette &VI, const CShipClass &SourceClass, const RECT &rcScreen, CString *retsError)

//	Create
//
//	Creates a new painter of the given type. The caller is responsible for 
//	freeing the result. We return NULL in case of error.

	{
	const CPlayerSettings *pSettings = SourceClass.GetPlayerSettings();
	const CXMLElement *pDesc = pSettings->GetHUDDesc(iType);
	if (pDesc == NULL)
		pDesc = &m_NullDesc;

	//	Cons up a create context

	SHUDCreateCtx CreateCtx(Universe, VI, SourceClass, *pDesc);
	CreateCtx.rcScreen = rcScreen;

	//	Create the appropriate HUD object.

	IHUDPainter *pPainter;
	switch (iType)
		{
		case hudAccelerate:
			pPainter = new CAccelerateHUD;
			break;

		case hudArmor:
			{
			CString sStyle = CreateCtx.Desc.GetAttribute(STYLE_ATTRIB);
			if (sStyle.IsBlank() || strEquals(sStyle, STYLE_DEFAULT))
				pPainter = new CArmorHUDImages;
			else if (strEquals(sStyle, STYLE_CIRCULAR))
				pPainter = new CArmorHUDRingSegments;
			else if (strEquals(sStyle, STYLE_RECTANGULAR))
				pPainter = new CArmorHUDRectangular;
			else
				{
				if (retsError) *retsError = strPatternSubst(CONSTLIT("Invalid armor display style: %s."), sStyle);
				return NULL;
				}

			break;
			}

		case hudLRS:
			pPainter = new CLRSHUD;
			break;

		case hudReactor:
			{
			CString sStyle = CreateCtx.Desc.GetAttribute(STYLE_ATTRIB);
			if (sStyle.IsBlank() || strEquals(sStyle, STYLE_DEFAULT))
				pPainter = new CReactorHUDDefault;
			else if (strEquals(sStyle, STYLE_CIRCULAR))
				pPainter = new CReactorHUDCircular;
			else
				{
				if (retsError) *retsError = strPatternSubst(CONSTLIT("Invalid reactor display style: %s."), sStyle);
				return NULL;
				}

			break;
			}

		case hudShields:
			{
			if (pDesc->GetTag().IsBlank())
				pPainter = new CNullHUD;
			else
				pPainter = new CShieldHUDDefault;
			break;
			}

		case hudTargeting:
			{
			CString sStyle = CreateCtx.Desc.GetAttribute(STYLE_ATTRIB);
			if (sStyle.IsBlank() || strEquals(sStyle, STYLE_DEFAULT))
				pPainter = new CWeaponHUDDefault;
			else if (strEquals(sStyle, STYLE_CIRCULAR))
				pPainter = new CWeaponHUDCircular;
			else
				{
				if (retsError) *retsError = strPatternSubst(CONSTLIT("Invalid weapon display style: %s."), sStyle);
				return NULL;
				}

			break;
			}

		case hudTimers:
			pPainter = new CTimersHUD;
			break;

		default:
			ASSERT(false);
			return NULL;
		}

	//	Initialize

	if (!pPainter->OnCreate(CreateCtx, retsError))
		{
		delete pPainter;
		return NULL;
		}

	//	If we did not set a location at creation time, then set it to a default.

	if (pPainter->GetLocation() == 0)
		pPainter->SetLocation(rcScreen, GetDefaultLocation(iType));

	//	Success

	return pPainter;
	}

void IHUDPainter::DrawModifier (CG32bitImage &Dest, int x, int y, const CString &sText, DWORD dwLocation)

//	DrawModifier
//
//	Draws a standard modifier

	{
	if (!sText.IsBlank())
		{
		const CVisualPalette &VI = g_pHI->GetVisuals();
		const CG16bitFont &SmallFont = VI.GetFont(fontSmall);

		bool bDisadvantage = (*(sText.GetASCIIZPointer()) == '-');

		int cy;
		int cx = SmallFont.MeasureText(sText, &cy);

		int cxBox = cx + 8;
		int cyBox = cy;

		int xBox;
		int yBox = y;
		if (dwLocation & locAlignCenter)
			xBox = x - (cxBox / 2);
		else if (dwLocation & locAlignRight)
			xBox = x - cxBox;
		else
			xBox = x;

		Dest.Fill(xBox,
				yBox,
				cxBox,
				cyBox,
				(bDisadvantage ? VI.GetColor(colorAreaDisadvantage) : VI.GetColor(colorAreaAdvantage)));

		SmallFont.DrawText(Dest,
				xBox + (cxBox - cx) / 2,
				yBox,
				(bDisadvantage ? VI.GetColor(colorTextDisadvantage) : VI.GetColor(colorTextAdvantage)),
				sText);
		}
	}

DWORD IHUDPainter::GetDefaultLocation (EHUDTypes iType)

//	GetDefaultLocation
//
//	Returns the default location for the given HUD type.

	{
	switch (iType)
		{
		case hudAccelerate:
			return locAlignTop | locAlignCenter;

		case hudArmor:
			return locAlignBottom | locAlignRight;

		case hudLRS:
			return locAlignTop | locAlignRight;

		case hudReactor:
			return locAlignTop | locAlignLeft;

		case hudShields:
			return locNone;

		case hudTargeting:
			return locAlignBottom | locAlignLeft;

		case hudTimers:
			return locAlignBottom | locAlignCenter;

		default:
			ASSERT(false);
			return 0;
		}
	}

void IHUDPainter::GetRect (RECT *retrcRect) const

//	GetRect
//
//	Returns the RECT

	{
	int cxWidth;
	int cyHeight;
	GetBounds(&cxWidth, &cyHeight);

	retrcRect->left = m_xPos;
	retrcRect->top = m_yPos;
	retrcRect->right = m_xPos + cxWidth;
	retrcRect->bottom = m_yPos + cyHeight;
	}

ALERROR IHUDPainter::InitRectFromElement (CXMLElement *pItem, RECT *retRect)

//	InitRectFromElement
//
//	Initializes the rect from x, y, width, height attributes

	{
	if (pItem == NULL)
		return ERR_FAIL;

	retRect->left = pItem->GetAttributeInteger(X_ATTRIB);
	retRect->top = pItem->GetAttributeInteger(Y_ATTRIB);
	retRect->right = retRect->left + pItem->GetAttributeInteger(WIDTH_ATTRIB);
	retRect->bottom = retRect->top + pItem->GetAttributeInteger(HEIGHT_ATTRIB);
	
	return NOERROR;
	}

void IHUDPainter::SetLocation (const RECT &rcRect, DWORD dwLocation)

//	SetLocation
//
//	Sets the location on the screen.

	{
	m_dwLoc = dwLocation;

	if (dwLocation == locNone)
		{
		m_xPos = 0;
		m_yPos = 0;
		}
	else
		{
		int cxDisplay;
		int cyDisplay;
		GetBounds(&cxDisplay, &cyDisplay);

		if (dwLocation & locAlignLeft)
			m_xPos = rcRect.left;
		else if (dwLocation & locAlignRight)
			m_xPos = rcRect.right - cxDisplay;
		else
			m_xPos = rcRect.left + (RectWidth(rcRect) - cxDisplay) / 2;

		if (dwLocation & locAlignTop)
			m_yPos = rcRect.top;
		else if (dwLocation & locAlignBottom)
			m_yPos = rcRect.bottom - cyDisplay;
		else
			m_yPos = rcRect.top + (RectHeight(rcRect) - cyDisplay) / 2;
		}
	}
