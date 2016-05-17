//	CMapLegendPainter.cpp
//
//	CMapLegendPainter class
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

const int PADDING_X = 10;
const int PADDING_Y = 10;
const int BORDER_RADIUS = 4;
const BYTE PANE_OPACITY = 128;
const int SCALE_HEIGHT = 5;
const int SCALE_PADDING_TOP = 10;
const int SCALE_PADDING_BOTTOM = 4;

CMapLegendPainter::CMapLegendPainter (const CVisualPalette &VI, SScaleEntry *pScaleDesc, int iCount) :
        m_VI(VI),
        m_pScaleDesc(pScaleDesc),
        m_iScaleCount(iCount),
        m_cxWidth(100),
        m_cxScale(0),
        m_bRealized(false)

//  CMapLegendPainter constructor

    {
    }

void CMapLegendPainter::Paint (CG32bitImage &Dest, int x, int y) const

//  Paint
//
//  Paints the legend

    {
	CG32bitPixel rgbBackgroundColor = m_VI.GetColor(colorAreaDeep);
	CG32bitPixel rgbLineColor = m_VI.GetColor(colorLineFrame);
	const CG16bitFont &TitleFont = m_VI.GetFont(fontHeader);
	const CG16bitFont &DescFont = m_VI.GetFont(fontMedium);

    if (!m_bRealized)
        Realize();

    //  Paint the pane background

    CGDraw::RoundedRect(Dest, x, y, RectWidth(m_rcRect), RectHeight(m_rcRect), BORDER_RADIUS, CG32bitPixel(m_VI.GetColor(colorAreaDialog), PANE_OPACITY));
    CGDraw::RoundedRectOutline(Dest, x, y, RectWidth(m_rcRect), RectHeight(m_rcRect), BORDER_RADIUS, 1, m_VI.GetColor(colorLineFrame));

    //  Paint text

	int xPos = x + PADDING_X;
	int yPos = y + PADDING_Y;

    TitleFont.DrawText(Dest, xPos, yPos, m_VI.GetColor(colorTextHighlight), m_Title, 0, 0, &yPos);
    DescFont.DrawText(Dest, xPos, yPos, m_VI.GetColor(colorTextFade), m_Desc, 0, 0, &yPos);

    //  Paint the scale

    if (m_cxScale > 0)
        {
        yPos += SCALE_PADDING_TOP;

        CG32bitPixel rgbScale = m_VI.GetColor(colorTextHighlight);
        Dest.FillLine(xPos, yPos + (SCALE_HEIGHT / 2), m_cxScale, rgbScale);
        Dest.FillColumn(xPos, yPos, SCALE_HEIGHT, rgbScale);
        Dest.FillColumn(xPos + m_cxScale - 1, yPos, SCALE_HEIGHT, rgbScale);

        yPos += SCALE_HEIGHT + SCALE_PADDING_BOTTOM;

	    Dest.DrawText(xPos, yPos, DescFont, rgbScale, m_sScaleLabel);
	    yPos += DescFont.GetHeight();
        }
    }

void CMapLegendPainter::SetScale (Metric rUnitsPerPixel)

//  SetScale
//
//  Sets the scale

    {
    int i;

    //  We'll need to recompute metrics

    m_bRealized = false;

    //  Short-circuit

    if (rUnitsPerPixel <= 0.0)
        {
        m_cxScale = 0;
        return;
        }

    //  Figure out how much room we have for the scale

    int cxMaxLength = m_cxWidth - (2 * PADDING_X);

    //  Loop until we find a scale that fits

    for (i = 0; i < m_iScaleCount; i++)
        {
        m_cxScale = mathRound(m_pScaleDesc[i].rUnits / rUnitsPerPixel);
        if (m_cxScale <= cxMaxLength)
            break;
        }

    //  Not found

    if (i == m_iScaleCount)
        {
        m_cxScale = 0;
        return;
        }

    //  Compose label

    m_sScaleLabel = strPatternSubst("%,d %s", (int)m_pScaleDesc[i].rUnits, CString(m_pScaleDesc[i].pszUnitLabel, -1, TRUE));
    }

void CMapLegendPainter::Realize (void) const

//  Realize
//
//  Wraps lines and computes size of the legend box.

    {
    if (m_bRealized)
        return;

    const CG16bitFont &TitleFont = m_VI.GetFont(fontHeader);
    const CG16bitFont &DescFont = m_VI.GetFont(fontMedium);

    //  Compute the inner width by subtracting padding and format both the
    //  title and description.

    int cxInnerWidth = m_cxWidth - (2 * PADDING_X);
    TitleFont.BreakText(m_sTitle, cxInnerWidth, &m_Title, CG16bitFont::TruncateLine);
    DescFont.BreakText(m_sDesc, cxInnerWidth, &m_Desc, 0);

    //  Account for the scale label.

    int cyScale;
    if (m_cxScale > 0)
        cyScale = SCALE_PADDING_TOP + SCALE_HEIGHT + SCALE_PADDING_BOTTOM + DescFont.GetHeight();
    else
        cyScale = 0;

    //  Compute the rect, now that we know the number of lines.

    m_rcRect.left = 0;
    m_rcRect.right = m_cxWidth;
    m_rcRect.top = 0;
    m_rcRect.bottom = PADDING_Y + TitleFont.GetHeight() + (m_Desc.GetCount() * DescFont.GetHeight()) + cyScale + PADDING_Y;

    //  Done

    m_bRealized = true;
    }
