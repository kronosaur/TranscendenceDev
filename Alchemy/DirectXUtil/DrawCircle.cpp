//	CGDraw.cpp
//
//	CGDraw Class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CGDraw::Circle (CG8bitImage &Dest, int xCenter, int yCenter, int iRadius, BYTE Value)

//	Circle
//
//	Draws a filled circle

	{
	//	Deal with edge-conditions

	if (iRadius <= 0)
		{
		Dest.SetPixel(xCenter, yCenter, Value);
		return;
		}

	//	Initialize some stuff

	int x = 0;
	int y = iRadius;
	int d = 1 - iRadius;
	int deltaE = 3;
	int deltaSE = -2 * iRadius + 5;

	Dest.FillLine(xCenter - iRadius, yCenter, 1 + 2 * iRadius, Value);

	//	Loop

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

			Dest.FillLine(xCenter - x, yCenter - y, 1 + 2 * x, Value);
			Dest.FillLine(xCenter - x, yCenter + y, 1 + 2 * x, Value);

			y--;
			}

		x++;

		if (y >= x)
			{
			Dest.FillLine(xCenter - y, yCenter - x, 1 + 2 * y, Value);
			Dest.FillLine(xCenter - y, yCenter + x, 1 + 2 * y, Value);
			}
		}
	}

void CGDraw::Circle (CG32bitImage &Dest, int xCenter, int yCenter, int iRadius, CG32bitPixel rgbColor, EBlendModes iMode)

//	Circle
//
//	Draws a filled circle

	{
	//	Deal with edge-conditions

	if (iRadius <= 0)
		return;

	//	Based on mode

	switch (iMode)
		{
		//	Normal mode is optimized

		case blendNormal:
			{
			//	Initialize some stuff

			int x = 0;
			int y = iRadius;
			int d = 1 - iRadius;
			int deltaE = 3;
			int deltaSE = -2 * iRadius + 5;

			Dest.FillLine(xCenter - iRadius, yCenter, 1 + 2 * iRadius, rgbColor);

			//	Loop

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

					Dest.FillLine(xCenter - x, yCenter - y, 1 + 2 * x, rgbColor);
					Dest.FillLine(xCenter - x, yCenter + y, 1 + 2 * x, rgbColor);

					y--;
					}

				x++;

				if (y >= x)
					{
					Dest.FillLine(xCenter - y, yCenter - x, 1 + 2 * y, rgbColor);
					Dest.FillLine(xCenter - y, yCenter + x, 1 + 2 * y, rgbColor);
					}
				}

			break;
			}

		case blendHardLight:
			{
			TFillCircleSolid<CGBlendHardLight> Painter(iRadius, rgbColor);
			Painter.Draw(Dest, xCenter, yCenter);
			break;
			}

		case blendScreen:
			{
			TFillCircleSolid<CGBlendScreen> Painter(iRadius, rgbColor);
			Painter.Draw(Dest, xCenter, yCenter);
			break;
			}

		case blendCompositeNormal:
			{
			TFillCircleSolid<CGBlendComposite> Painter(iRadius, rgbColor);
			Painter.Draw(Dest, xCenter, yCenter);
			break;
			}
		}
	}

void CGDraw::Circle (CG32bitImage &Dest, int x, int y, int iRadius, const TArray<CG32bitPixel> &ColorRamp, EBlendModes iMode, bool bPreMult)

//	Circle
//
//	Draws a circle with a color ramp

	{
	switch (iMode)
		{
		case blendNormal:
			{
			CRadialCirclePainter<CGBlendBlend> Painter(iRadius, ColorRamp, bPreMult);
			Painter.Draw(Dest, x, y);
			break;
			}

		case blendHardLight:
			{
			CRadialCirclePainter<CGBlendHardLight> Painter(iRadius, ColorRamp, bPreMult);
			Painter.Draw(Dest, x, y);
			break;
			}

		case blendScreen:
			{
			CRadialCirclePainter<CGBlendScreen> Painter(iRadius, ColorRamp, bPreMult);
			Painter.Draw(Dest, x, y);
			break;
			}

		case blendCompositeNormal:
			{
			CRadialCirclePainter<CGBlendComposite> Painter(iRadius, ColorRamp, bPreMult);
			Painter.Draw(Dest, x, y);
			break;
			}
		}
	}

void CGDraw::CircleImage (CG32bitImage &Dest, int x, int y, int iRadius, BYTE byOpacity, const CG32bitImage &Image, EBlendModes iMode, int xSrc, int ySrc, int cxSrc, int cySrc)

//	CircleImage
//
//	Draws a circle using the given image.

	{
	switch (iMode)
		{
		case blendNormal:
			{
			CImageCirclePainter<CGBlendBlend> Painter(iRadius, byOpacity, Image, xSrc, ySrc, cxSrc, cySrc);
			Painter.Draw(Dest, x, y);
			break;
			}

		case blendHardLight:
			{
			CImageCirclePainter<CGBlendHardLight> Painter(iRadius, byOpacity, Image, xSrc, ySrc, cxSrc, cySrc);
			Painter.Draw(Dest, x, y);
			break;
			}

		case blendScreen:
			{
			CImageCirclePainter<CGBlendScreen> Painter(iRadius, byOpacity, Image, xSrc, ySrc, cxSrc, cySrc);
			Painter.Draw(Dest, x, y);
			break;
			}

		case blendCompositeNormal:
			{
			CImageCirclePainter<CGBlendComposite> Painter(iRadius, byOpacity, Image, xSrc, ySrc, cxSrc, cySrc);
			Painter.Draw(Dest, x, y);
			break;
			}
		}
	}

void CGDraw::CircleGradient (CG8bitImage &Dest, int x, int y, int iRadius, BYTE CenterValue, BYTE EdgeValue)

//	CircleGradient
//
//	Draws a circle gradient on an 8-bit image

	{
	CRadialCirclePainter8 Painter(iRadius, CenterValue, EdgeValue);
	Painter.Draw(Dest, x, y);
	}

void CGDraw::CircleGradient (CG32bitImage &Dest, int x, int y, int iRadius, CG32bitPixel rgbColor, EBlendModes iBlendMode)

//	CircleGradient
//
//	Draws a circle that is solid at the center and fully transparent at the 
//	edges.

	{
	int i;

	if (iRadius <= 0)
		return;

	//	Generate an appropriate color ramp (pre-multiplied)

	TArray<CG32bitPixel> ColorRamp;
	ColorRamp.InsertEmpty(iRadius);
	for (i = 0; i < iRadius; i++)
		{
		BYTE byAlpha = (BYTE)(255 - (i * 255 / iRadius));
		ColorRamp[i] = CG32bitPixel(CG32bitPixel::Blend(0, rgbColor, byAlpha), byAlpha);
		}

	//	Draw the circle

	Circle(Dest, x, y, iRadius, ColorRamp, iBlendMode, true);
	}

void CGDraw::CircleOutline (CG32bitImage &Dest, int x, int y, int iRadius, int iLineWidth, CG32bitPixel rgbColor)

//	CirculeOutline
//
//	Draws a circle outline.

	{
	constexpr int SEGMENT_COUNT = 64;
	constexpr Metric rInc = 2.0 * PI / SEGMENT_COUNT;

	Metric rAngle = rInc;
	CVector vCenter(x, y);
	int xLast = x + iRadius;
	int yLast = y;

	for (int i = 0; i < SEGMENT_COUNT; i++)
		{
		CVector vPos = vCenter + PolarToVectorRadians(rAngle, iRadius);
		int xPos = mathRound(vPos.GetX());
		int yPos = mathRound(vPos.GetY());

		Line(Dest, xLast, yLast, xPos, yPos, iLineWidth, rgbColor);

		xLast = xPos;
		yLast = yPos;

		rAngle += rInc;
		}
	}

void CGDraw::RingGlowing (CG32bitImage &Dest, int x, int y, int iRadius, int iWidth, CG32bitPixel rgbColor)

//	RingGlowing
//
//	Draw a glowing ring

	{
	CGlowingRingPainter Painter(Dest, iRadius, iWidth, rgbColor);
	Painter.Draw(x, y);
	}

void CGDraw::RingGlowing (CG32bitImage &Dest, int x, int y, int iRadius, int iWidth, const TArray<CG32bitPixel> &ColorRamp, BYTE byOpacity)

//	RingGlowing
//
//	Draw a glowing ring.

	{
	CGlowingRingPainter Painter(Dest, iRadius, iWidth, ColorRamp, byOpacity);
	Painter.Draw(x, y);
	}

//	CRadialCirclePainter8 ------------------------------------------------------

CRadialCirclePainter8::CRadialCirclePainter8 (int iRadius, BYTE CenterValue, BYTE EdgeValue) : TRadialPainter8(iRadius)

//	CRadialCirclePainter8 constructor

	{
	int i;

	if (iRadius <= 0)
		return;

	int iStart = (int)(DWORD)CenterValue;
	int iRange = (int)(DWORD)EdgeValue - (int)(DWORD)CenterValue;

	m_Ramp.InsertEmpty(iRadius);
	for (i = 0; i < m_Ramp.GetCount(); i++)
		m_Ramp[i] = (BYTE)(iStart + (iRange * i / iRadius));
	}

