//	CGDraw.cpp
//
//	CGDraw Class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "DrawRegionImpl.h"

void CGDraw::Region (CG32bitImage &Dest, int x, int y, const CGRegion &Region, CG32bitPixel rgbColor, EBlendModes iMode)

//	Region
//
//	Fills a region using the given color and blend mode

	{
	switch (iMode)
		{
		case blendNormal:
			{
			if (rgbColor.GetAlpha() == 0xff)
				{
				TFillRegionSolid<CGBlendCopy> Painter(rgbColor);
				Painter.Draw(Dest, x, y, Region);
				}
			else
				{
				TFillRegionSolid<CGBlendBlend> Painter(rgbColor);
				Painter.Draw(Dest, x, y, Region);
				}
			break;
			}

		case blendHardLight:
			{
			TFillRegionSolid<CGBlendHardLight> Painter(rgbColor);
			Painter.Draw(Dest, x, y, Region);
			break;
			}

		case blendScreen:
			{
			TFillRegionSolid<CGBlendScreen> Painter(rgbColor);
			Painter.Draw(Dest, x, y, Region);
			break;
			}
		}
	}

void CGDraw::Region (CG32bitImage &Dest, int x, int y, const CG16bitBinaryRegion &Region, CG32bitPixel rgbColor, EBlendModes iMode)

//	Region
//
//	Fills a region using the given color and blend mode

	{
	switch (iMode)
		{
		case blendNormal:
			{
			if (rgbColor.GetAlpha() == 0xff)
				{
				TFillRegionSolid<CGBlendCopy> Painter(rgbColor);
				Painter.Draw(Dest, x, y, Region);
				}
			else
				{
				TFillRegionSolid<CGBlendBlend> Painter(rgbColor);
				Painter.Draw(Dest, x, y, Region);
				}
			break;
			}

		case blendHardLight:
			{
			TFillRegionSolid<CGBlendHardLight> Painter(rgbColor);
			Painter.Draw(Dest, x, y, Region);
			break;
			}

		case blendScreen:
			{
			TFillRegionSolid<CGBlendScreen> Painter(rgbColor);
			Painter.Draw(Dest, x, y, Region);
			break;
			}
		}
	}
