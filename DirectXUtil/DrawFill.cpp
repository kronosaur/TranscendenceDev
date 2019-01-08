//	DrawFill.cpp
//
//	CGDraw Class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CGDraw::Fill (CG32bitImage &Dest, int x, int y, int cxWidth, int cyHeight, CG32bitPixel rgbColor, EBlendModes iMode)

//	Fill
//
//	Fill a rectangle with a solid color.

	{
	switch (iMode)
		{
		case blendNormal:
			Dest.Fill(x, y, cxWidth, cyHeight, rgbColor);
			break;

		case blendHardLight:
			{
			CSolidFill FillColor(rgbColor);
			TFillPainter32<CSolidFill, CGBlendHardLight> Painter(FillColor);
			Painter.Fill(Dest, x, y, cxWidth, cyHeight);
			break;
			}

		case blendScreen:
			{
			CSolidFill FillColor(rgbColor);
			TFillPainter32<CSolidFill, CGBlendScreen> Painter(FillColor);
			Painter.Fill(Dest, x, y, cxWidth, cyHeight);
			break;
			}

		case blendCompositeNormal:
			{
			CSolidFill FillColor(rgbColor);
			TFillPainter32<CSolidFill, CGBlendComposite> Painter(FillColor);
			Painter.Fill(Dest, x, y, cxWidth, cyHeight);
			break;
			}
		}
	}

void CGDraw::FillColumn (CG32bitImage &Dest, int x, int y, int cyHeight, CG32bitPixel rgbColor, EBlendModes iMode)

//	FillColumn
//
//	Fill a column with a solid color.

	{
	switch (iMode)
		{
		case blendNormal:
			Dest.FillColumn(x, y, cyHeight, rgbColor);
			break;

		case blendHardLight:
			{
			CSolidFill FillColor(rgbColor);
			TFillPainter32<CSolidFill, CGBlendHardLight> Painter(FillColor);
			Painter.FillColumn(Dest, x, y, cyHeight);
			break;
			}

		case blendScreen:
			{
			CSolidFill FillColor(rgbColor);
			TFillPainter32<CSolidFill, CGBlendScreen> Painter(FillColor);
			Painter.FillColumn(Dest, x, y, cyHeight);
			break;
			}

		case blendCompositeNormal:
			{
			CSolidFill FillColor(rgbColor);
			TFillPainter32<CSolidFill, CGBlendComposite> Painter(FillColor);
			Painter.FillColumn(Dest, x, y, cyHeight);
			break;
			}
		}
	}

void CGDraw::FillLine (CG32bitImage &Dest, int x, int y, int cxWidth, CG32bitPixel rgbColor, EBlendModes iMode)

//	FillLine
//
//	Fill a line with a solid color.

	{
	switch (iMode)
		{
		case blendNormal:
			Dest.FillLine(x, y, cxWidth, rgbColor);
			break;

		case blendHardLight:
			{
			CSolidFill FillColor(rgbColor);
			TFillPainter32<CSolidFill, CGBlendHardLight> Painter(FillColor);
			Painter.FillLine(Dest, x, y, cxWidth);
			break;
			}

		case blendScreen:
			{
			CSolidFill FillColor(rgbColor);
			TFillPainter32<CSolidFill, CGBlendScreen> Painter(FillColor);
			Painter.FillLine(Dest, x, y, cxWidth);
			break;
			}

		case blendCompositeNormal:
			{
			CSolidFill FillColor(rgbColor);
			TFillPainter32<CSolidFill, CGBlendComposite> Painter(FillColor);
			Painter.FillLine(Dest, x, y, cxWidth);
			break;
			}
		}
	}
