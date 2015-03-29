//	CGRealRGB.cpp
//
//	CGRealRGB Class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CGRealRGB::CGRealRGB (CG32bitPixel rgbColor) :
		m_rR(rgbColor.GetRed() / 255.0),
		m_rG(rgbColor.GetGreen() / 255.0),
		m_rB(rgbColor.GetBlue() / 255.0),
		m_rA(rgbColor.GetAlpha() / 255.0)

//	CGRealRGB constructor

	{
	}

CGRealRGB CGRealRGB::FromHSB (const CGRealHSB &hsbColor)

//	FromHSB
//
//	Converts from HSB to RGB
//	From: Computer Graphics, Foley & van Dam, p.593

	{
	if (hsbColor.GetSaturation() == 0.0)
		return CGRealRGB(hsbColor.GetBrightness(), hsbColor.GetBrightness(), hsbColor.GetBrightness(), hsbColor.GetAlpha());
	else
		{
		REALPIXEL rH = (hsbColor.GetHue() == 360.0 ? 0.0 : hsbColor.GetHue()) / 60.0;
		REALPIXEL rI = floor(rH);
		REALPIXEL rF = rH - rI;
		REALPIXEL rP = hsbColor.GetBrightness() * (1.0 - hsbColor.GetSaturation());
		REALPIXEL rQ = hsbColor.GetBrightness() * (1.0 - hsbColor.GetSaturation() * rF);
		REALPIXEL rT = hsbColor.GetBrightness() * (1.0 - hsbColor.GetSaturation() * (1.0 - rF));

		switch ((int)rI)
			{
			case 0:
				return CGRealRGB(hsbColor.GetBrightness(), rT, rP, hsbColor.GetAlpha());

			case 1:
				return CGRealRGB(rQ, hsbColor.GetBrightness(), rP, hsbColor.GetAlpha());

			case 2:
				return CGRealRGB(rP, hsbColor.GetBrightness(), rT, hsbColor.GetAlpha());

			case 3:
				return CGRealRGB(rP, rQ, hsbColor.GetBrightness(), hsbColor.GetAlpha());

			case 4:
				return CGRealRGB(rT, rP, hsbColor.GetBrightness(), hsbColor.GetAlpha());

			case 5:
				return CGRealRGB(hsbColor.GetBrightness(), rP, rQ, hsbColor.GetAlpha());

			default:
				return CGRealRGB(0.0, 0.0, 0.0, 0.0);
			}
		}
	}
