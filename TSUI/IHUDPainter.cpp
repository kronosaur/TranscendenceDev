//	IHUDPainter.cpp
//
//	IHUDPainter class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

#define STYLE_ATTRIB							CONSTLIT("style")

#define STYLE_DEFAULT							CONSTLIT("default")
#define STYLE_RING_SEGMENTS						CONSTLIT("ringSegments")

IHUDPainter *IHUDPainter::Create (SDesignLoadCtx &Ctx, CShipClass *pClass, ETypes iType)

//	Create
//
//	Creates a new painter of the given type. The caller is responsible for 
//	freeing the result. We return NULL in case of error.

	{
	const CPlayerSettings *pSettings = pClass->GetPlayerSettings();

	//	Create

	IHUDPainter *pPainter;
	switch (iType)
		{
		case hudArmor:
			{
			CXMLElement *pDesc = pSettings->GetArmorDesc();
			if (pDesc == NULL)
				return NULL;

			CString sStyle = pDesc->GetAttribute(STYLE_ATTRIB);
			if (sStyle.IsBlank() || strEquals(sStyle, STYLE_DEFAULT))
				pPainter = new CArmorHUDImages;
			else if (strEquals(sStyle, STYLE_RING_SEGMENTS))
				pPainter = new CArmorHUDRingSegments;
			else
				{
				Ctx.sError = strPatternSubst(CONSTLIT("Invalid armor display style: %s."), sStyle);
				return NULL;
				}

			if (pPainter->InitFromXML(Ctx, pClass, pDesc) != NOERROR)
				{
				delete pPainter;
				return NULL;
				}

			break;
			}

		case hudShields:
			{
			CXMLElement *pDesc = pSettings->GetShieldDesc();
			if (pDesc == NULL)
				return NULL;

			pPainter = new CShieldHUDDefault;
			if (pPainter->InitFromXML(Ctx, pClass, pDesc) != NOERROR)
				{
				delete pPainter;
				return NULL;
				}

			break;
			}

		default:
			ASSERT(false);
			return NULL;
		}

	//	Bind

	if (pPainter->Bind(Ctx) != NOERROR)
		{
		delete pPainter;
		return NULL;
		}

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

void IHUDPainter::SetLocation (const RECT &rcRect, DWORD dwLocation)

//	SetLocation
//
//	Sets the location on the screen.

	{
	if (dwLocation == locNone)
		{
		m_xPos = 0;
		m_yPos = 0;
		return;
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
