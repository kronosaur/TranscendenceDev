//	CAniSolidFill.cpp
//
//	CAniSolidFill class
//	Copyright (c) 2011 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Reanimator.h"

#define PROP_COLOR							CONSTLIT("color")
#define PROP_OPACITY						CONSTLIT("opacity")

void CAniSolidFill::Fill (SAniPaintCtx &Ctx, int x, int y, int cxWidth, int cyHeight)

//	Fill
//
//	Fill

	{
	if (m_dwOpacity == 255)
		Ctx.Dest.Fill(x, y, cxWidth, cyHeight, m_rgbColor);
	else if (m_dwOpacity > 0)
		Ctx.Dest.Fill(x, y, cxWidth, cyHeight, CG32bitPixel(m_rgbColor, (BYTE)m_dwOpacity));
	}

void CAniSolidFill::Fill (SAniPaintCtx &Ctx, int x, int y, const TArray<SSimpleRasterLine> &Lines)

//	Fill
//
//	Files the set of lines

	{
	SSimpleRasterLine *pLine = &Lines[0];
	SSimpleRasterLine *pLineEnd = pLine + Lines.GetCount();

	if (m_dwOpacity == 255)
		{
		while (pLine < pLineEnd)
			{
			Ctx.Dest.FillLine(x + pLine->x, y + pLine->y, pLine->cxLength, m_rgbColor);
			if (pLine->byLeftEdge)
				Ctx.Dest.SetPixelTrans(x + pLine->x - 1, y + pLine->y, m_rgbColor, pLine->byLeftEdge);
			if (pLine->byRightEdge)
				Ctx.Dest.SetPixelTrans(x + pLine->x + pLine->cxLength, y + pLine->y, m_rgbColor, pLine->byRightEdge);

			pLine++;
			}
		}
	else if (m_dwOpacity > 0)
		{
		while (pLine < pLineEnd)
			{
			Ctx.Dest.FillLine(x + pLine->x, y + pLine->y, pLine->cxLength, CG32bitPixel(m_rgbColor, (BYTE)m_dwOpacity));

			if (pLine->byLeftEdge)
				{
				DWORD dwOpacity = m_dwOpacity * pLine->byLeftEdge / 255;
				Ctx.Dest.SetPixelTrans(x + pLine->x - 1, y + pLine->y, m_rgbColor, (BYTE)dwOpacity);
				}

			if (pLine->byRightEdge)
				{
				DWORD dwOpacity = m_dwOpacity * pLine->byRightEdge / 255;
				Ctx.Dest.SetPixelTrans(x + pLine->x + pLine->cxLength, y + pLine->y, m_rgbColor, (BYTE)dwOpacity);
				}

			pLine++;
			}
		}
	}

void CAniSolidFill::Fill (SAniPaintCtx &Ctx, int x, int y, const CG16bitBinaryRegion &Region)

//	Fill
//
//	Fills the region

	{
	Region.Fill(Ctx.Dest, x, y, CG32bitPixel(m_rgbColor, (BYTE)m_dwOpacity));
	}

void CAniSolidFill::InitDefaults (CAniPropertySet &Properties)

//	InitDefaults
//
//	Initializes animatron properties to defaults

	{
	if (Properties.FindProperty(PROP_COLOR) == -1)
		Properties.SetColor(PROP_COLOR, 0xffff);

	if (Properties.FindProperty(PROP_OPACITY) == -1)
		Properties.SetOpacity(PROP_OPACITY, 255);
	}

void CAniSolidFill::InitPaint (SAniPaintCtx &Ctx, int xOrigin, int yOrigin, CAniPropertySet &Properties)

//	InitPaint
//
//	Initializes internal cache from properties

	{
	m_rgbColor = Properties.GetColor(PROP_COLOR);
	m_dwOpacity = Properties.GetOpacity(PROP_OPACITY) * Ctx.dwOpacityToDest / 255;
	}

