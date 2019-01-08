//	CGlowingRingPainter.cpp
//
//	CGlowingRingPainter Class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CGlowingRingPainter::CGlowingRingPainter (CG32bitImage &Dest, int iRadius, int iWidth, CG32bitPixel rgbColor) :
		m_Dest(Dest),
		m_rcClip(Dest.GetClipRect()),
		m_iRadius(iRadius),
		m_iWidth(iWidth)

//	CGlowingRingPainter constructor

	{
	int i;

	if (m_iWidth <= 0)
		{
		m_pColorRamp = NULL;
		return;
		}

	//	Generate a color ramp

	int iCenter = m_iWidth / 2;
	int iExtra = m_iWidth % 2;
	int iOuter = iCenter - 1;
	int iInner = iCenter + iExtra;

	m_ColorRamp.InsertEmpty(m_iWidth);

	//	We expect the color ramp to be pre-multiplied

	CG32bitPixel rgbPreMult = CG32bitPixel::PreMult(rgbColor);

	//	Init the center

	if (iExtra)
		m_ColorRamp[iCenter] = rgbPreMult;

	//	Edges

	for (i = 0; i < iCenter; i++)
		{
		ASSERT(iOuter >= 0 && iOuter < m_iWidth);
		ASSERT(iInner >=0 && iInner < m_iWidth);

		BYTE byAlpha = (BYTE)Min((WORD)0xff, (WORD)(rgbPreMult.GetAlpha() * (iCenter - i) / (iCenter + 1)));
		BYTE byRed = (BYTE)Min((WORD)0xff, (WORD)(rgbPreMult.GetRed() * (iCenter - i) / (iCenter + 1)));
		BYTE byGreen = (BYTE)Min((WORD)0xff, (WORD)(rgbPreMult.GetGreen() * (iCenter - i) / (iCenter + 1)));
		BYTE byBlue = (BYTE)Min((WORD)0xff, (WORD)(rgbPreMult.GetBlue() * (iCenter - i) / (iCenter + 1)));

		m_ColorRamp[iOuter] = CG32bitPixel(byRed, byGreen, byBlue, byAlpha);
		m_ColorRamp[iInner] = CG32bitPixel(byRed, byGreen, byBlue, byAlpha);

		iOuter--;
		iInner++;
		}

	m_pColorRamp = &m_ColorRamp;
	}

CGlowingRingPainter::CGlowingRingPainter (CG32bitImage &Dest, int iRadius, int iWidth, const TArray<CG32bitPixel> &ColorRamp, BYTE byOpacity) :
		m_Dest(Dest),
		m_rcClip(Dest.GetClipRect()),
		m_iRadius(iRadius),
		m_iWidth(iWidth)

//	CGlowingRingPainter constructor

	{
	int i;

	//	Pre-multiply the color ramp

	m_ColorRamp.InsertEmpty(ColorRamp.GetCount());
	m_pColorRamp = &m_ColorRamp;

	if (byOpacity == 0xff)
		{
		for (i = 0; i < m_ColorRamp.GetCount(); i++)
			m_ColorRamp[i] = CG32bitPixel::PreMult(ColorRamp[i]);
		}
	else
		{
		for (i = 0; i < m_ColorRamp.GetCount(); i++)
			{
			CG32bitPixel rgbPreMult = CG32bitPixel::PreMult(ColorRamp[i]);
			m_ColorRamp[i] = CG32bitPixel(CG32bitPixel::Blend(0, rgbPreMult, byOpacity), CG32bitPixel::BlendAlpha(rgbPreMult.GetAlpha(), byOpacity));
			}
		}
	}

void CGlowingRingPainter::Draw (int xCenter, int yCenter)

//	Draw
//
//	Draw the ring

	{
	//	Deal with edge-conditions

	if (m_iWidth <= 0 || m_pColorRamp == NULL || m_iRadius <= 0)
		return;

	//	Initialize some stuff

	int x = 0;
	int y = m_iRadius;
	int d = 1 - m_iRadius;
	int deltaE = 3;
	int deltaSE = -2 * m_iRadius + 5;

	//	Prepare struct

	m_xDest = xCenter;
	m_yDest = yCenter;

	//	Draw central line

	DrawLine(m_iRadius, 0);

	//	Draw lines above and below the center

	int iLastDraw = -1;
	while (y > x)
		{
		if (d < 0)
			{
			d += deltaE;
			deltaE += 2;
			deltaSE += 2;
			}
		else
			{
			d += deltaSE;
			deltaE += 2;
			deltaSE += 4;

			//	Draw lines

			DrawLine(x, y);
			iLastDraw = y;

			//	Next

			y--;
			}

		x++;

		//	Draw lines

		if (x != iLastDraw)
			DrawLine(y, x);
		}
	}

void CGlowingRingPainter::DrawLine (int x, int y)

//	DrawLine
//
//	Draws a line

	{
	int xStart = m_xDest - x;
	int xEnd = m_xDest + x + 1;

	if (xEnd <= m_rcClip.left || xStart >= m_rcClip.right)
		return;

	//	See which lines we need to paint

	int yLine = m_yDest - y;
	bool bPaintTop = (yLine >= m_rcClip.top && yLine < m_rcClip.bottom);
	CG32bitPixel *pCenterTop = m_Dest.GetPixelPos(m_xDest, yLine);

	yLine = m_yDest + y;
	bool bPaintBottom = ((y > 0) && (yLine >= m_rcClip.top && yLine < m_rcClip.bottom));
	CG32bitPixel *pCenterBottom = m_Dest.GetPixelPos(m_xDest, yLine);

	//	Compute radius increment

	int iRadius = y;
	int d = -y;
	int deltaE = 3;
	int deltaSE = -2 * y + 1;

	//	Loop

	int xPos = 0;

	//	This will skip the center pixel in the circle (avoids a divide by
	//	zero in the inner loop).

	if (y == 0)
		{
		xPos = 1;
		d += deltaSE;
		deltaE += 2;
		iRadius++;
		}

	//	Blt the line 

	while (xPos <= x)
		{
		//	Figure out the radius of the pixel at this location

		if (d < 0)
			{
			d += deltaE;
			deltaE += 2;
			deltaSE += 2;
			}
		else
			{
			d += deltaSE;
			deltaE += 2;
			//  deltaSE += 0;
			iRadius++;
			}

		//	Compute the index into the ramp based on radius and ring thickness
		//	(If we're outside the ramp, then continue)

		int iIndex = m_iRadius - iRadius;
		if (iIndex >= m_iWidth || iIndex < 0)
			{
			xPos++;
			continue;
			}

		//	Compute the transparency

		CG32bitPixel rgbColor = m_pColorRamp->GetAt(iIndex);
		BYTE byOpacity = rgbColor.GetAlpha();

		//	Optimize opaque painting

		if (byOpacity == 0x00)
			;
		else if (byOpacity == 0xff)
			{
			if (m_xDest - xPos < m_rcClip.right && m_xDest - xPos >= m_rcClip.left)
				{
				if (bPaintTop)
					*(pCenterTop - xPos) = rgbColor;

				if (bPaintBottom)
					*(pCenterBottom - xPos) = rgbColor;
				}

			if (xPos > 0 && m_xDest + xPos < m_rcClip.right && m_xDest + xPos >= m_rcClip.left)
				{
				if (bPaintTop)
					*(pCenterTop + xPos) = rgbColor;

				if (bPaintBottom)
					*(pCenterBottom + xPos) = rgbColor;
				}
			}
		else
			{
			BYTE *pAlphaInv = CG32bitPixel::AlphaTable(byOpacity ^ 0xff);

			WORD wRed = rgbColor.GetRed();
			WORD wGreen = rgbColor.GetGreen();
			WORD wBlue = rgbColor.GetBlue();

			//	Draw transparent

#define DRAW_PIXEL(pos)	\
			{ \
			BYTE byRedResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[(pos)->GetRed()] + wRed)); \
			BYTE byGreenResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[(pos)->GetGreen()] + wGreen)); \
			BYTE byBlueResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[(pos)->GetBlue()] + wBlue)); \
			\
			*(pos) = CG32bitPixel(byRedResult, byGreenResult, byBlueResult); \
			}

			//	Paint

			if (m_xDest - xPos < m_rcClip.right && m_xDest - xPos >= m_rcClip.left)
				{
				if (bPaintTop)
					DRAW_PIXEL(pCenterTop - xPos);

				if (bPaintBottom)
					DRAW_PIXEL(pCenterBottom - xPos);
				}

			if (xPos > 0 && m_xDest + xPos < m_rcClip.right && m_xDest + xPos >= m_rcClip.left)
				{
				if (bPaintTop)
					DRAW_PIXEL(pCenterTop + xPos);

				if (bPaintBottom)
					DRAW_PIXEL(pCenterBottom + xPos);
				}
#undef DRAW_PIXEL
			}

		xPos++;
		}
	}
