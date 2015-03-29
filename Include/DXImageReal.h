//	DXImageReal.h
//
//	Implements floating point images
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CGRealRGB;

class CGRealHSB
	{
	public:
		CGRealHSB (void)
			{ }

		CGRealHSB (REALPIXEL rH, REALPIXEL rS, REALPIXEL rB, REALPIXEL rA = 1.0) :
				m_rH(rH),
				m_rS(rS),
				m_rB(rB),
				m_rA(rA)
			{ }

		inline REALPIXEL GetAlpha (void) const { return m_rA; }
		inline REALPIXEL GetBrightness (void) const { return m_rB; }
		inline REALPIXEL GetHue (void) const { return m_rH; }
		inline REALPIXEL GetSaturation (void) const { return m_rS; }

		static CGRealHSB FromRGB (const CGRealRGB &rgbColor);
		static CGRealHSB FromRGB (CG32bitPixel rgbColor);

	private:
		REALPIXEL m_rH;						//	Hue: 0-360
		REALPIXEL m_rS;						//	Saturation: 0-1
		REALPIXEL m_rB;						//	Brightness: 0-1
		REALPIXEL m_rA;						//	Alpha: 0-1
	};

class CGRealRGB
	{
	public:
		CGRealRGB (void)
			{ }

		CGRealRGB (REALPIXEL rR, REALPIXEL rG, REALPIXEL rB, REALPIXEL rA = 1.0) :
				m_rR(rR),
				m_rG(rG),
				m_rB(rB),
				m_rA(rA)
			{ }

		CGRealRGB (CG32bitPixel rgbColor);

		inline REALPIXEL GetAlpha (void) const { return m_rA; }
		inline REALPIXEL GetBlue (void) const { return m_rB; }
		inline REALPIXEL GetGreen (void) const { return m_rG; }
		inline REALPIXEL GetRed (void) const { return m_rR; }

		static CGRealRGB FromHSB (const CGRealHSB &hsbColor);

	private:
		REALPIXEL m_rR;						//	Red: 0-1
		REALPIXEL m_rG;						//	Green: 0-1
		REALPIXEL m_rB;						//	Blue: 0-1
		REALPIXEL m_rA;						//	Alpha: 0-1
	};