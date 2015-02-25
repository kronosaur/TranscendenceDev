//	CGRealHSB.cpp
//
//	CGRealHSB Class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CGRealHSB CGRealHSB::FromRGB (const CGRealRGB &rgbColor)

//	FromRGB
//
//	Generates a real HSB value from 32-bit RGB
//	From: Computer Graphics, Foley & van Dam, p.592

	{
	enum EColors
		{
		colorRed = 0,
		colorGreen = 1,
		colorBlue = 2,
		};

	//	Figure out which is the primary (brightest) color

	int iPrimary;
	REALPIXEL rMax;
	REALPIXEL rMin;

	//	R G B
	//	R B G
	//	B R G

	if (rgbColor.GetRed() >= rgbColor.GetGreen())
		{
		//	R G B
		//	R B G

		if (rgbColor.GetRed() >= rgbColor.GetBlue())
			{
			iPrimary = colorRed;
			rMax = rgbColor.GetRed();

			if (rgbColor.GetGreen() >= rgbColor.GetBlue())
				rMin = rgbColor.GetBlue();
			else
				rMin = rgbColor.GetGreen();
			}

		//	B R G

		else
			{
			iPrimary = colorBlue;
			rMax = rgbColor.GetBlue();
			rMin = rgbColor.GetGreen();
			}
		}

	//	G R B
	//	G B R
	//	B G R

	else
		{
		//	G R B
		//	G B R

		if (rgbColor.GetGreen() >= rgbColor.GetBlue())
			{
			iPrimary = colorGreen;
			rMax = rgbColor.GetGreen();

			if (rgbColor.GetRed() > rgbColor.GetBlue())
				rMin = rgbColor.GetBlue();
			else
				rMin = rgbColor.GetRed();
			}

		//	B G R

		else
			{
			iPrimary = colorBlue;
			rMax = rgbColor.GetBlue();
			rMin = rgbColor.GetRed();
			}
		}
	
	//	Brightness

	REALPIXEL rBrightness = rMax;

	//	Saturation

	REALPIXEL rDelta = rMax - rMin;
	REALPIXEL rSaturation = (rMax != 0.0 ? (rDelta / rMax) : 0.0);

	//	Hue

	REALPIXEL rHue;
	if (rSaturation == 0.0)
		rHue = 0.0;	//	Undefined, but we just set it to 0
	else
		{
		switch (iPrimary)
			{
			case colorRed:
				rHue = (rgbColor.GetGreen() - rgbColor.GetBlue()) / rDelta;
				break;

			case colorGreen:
				rHue = 2.0 + (rgbColor.GetBlue() - rgbColor.GetRed()) / rDelta;
				break;

			case colorBlue:
				rHue = 4.0 + (rgbColor.GetRed() - rgbColor.GetGreen()) / rDelta;
				break;
			}

		rHue *= 60.0;
		if (rHue < 0.0)
			rHue += 360.0;
		}

	return CGRealHSB(
		rHue,
		rSaturation,
		rBrightness,
		rgbColor.GetAlpha()
		);
	}

CGRealHSB CGRealHSB::FromRGB (CG32bitPixel rgbColor)

//	FromRGB
//
//	Converts from RGB to HSB

	{
	return CGRealHSB::FromRGB(CGRealRGB(rgbColor));
	}
