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

		CGRealHSB (Metric rH, Metric rS, Metric rB, Metric rA = 1.0) :
				m_rH(rH),
				m_rS(rS),
				m_rB(rB),
				m_rA(rA)
			{ }

		inline Metric GetAlpha (void) const { return m_rA; }
		inline Metric GetBrightness (void) const { return m_rB; }
		inline Metric GetHue (void) const { return m_rH; }
		inline Metric GetSaturation (void) const { return m_rS; }

		static CGRealHSB FromRGB (const CGRealRGB &rgbColor);
		static CGRealHSB FromRGB (CG32bitPixel rgbColor);

	private:
		Metric m_rH;						//	Hue: 0-360
		Metric m_rS;						//	Saturation: 0-1
		Metric m_rB;						//	Brightness: 0-1
		Metric m_rA;						//	Alpha: 0-1
	};

class CGRealRGB
	{
	public:
		CGRealRGB (void)
			{ }

		CGRealRGB (Metric rR, Metric rG, Metric rB, Metric rA = 1.0) :
				m_rR(rR),
				m_rG(rG),
				m_rB(rB),
				m_rA(rA)
			{ }

		CGRealRGB (CG32bitPixel rgbColor);

		inline Metric GetAlpha (void) const { return m_rA; }
		inline Metric GetBlue (void) const { return m_rB; }
		inline Metric GetGreen (void) const { return m_rG; }
		inline Metric GetRed (void) const { return m_rR; }

		static CGRealRGB FromHSB (const CGRealHSB &hsbColor);

	private:
		Metric m_rR;						//	Red: 0-1
		Metric m_rG;						//	Green: 0-1
		Metric m_rB;						//	Blue: 0-1
		Metric m_rA;						//	Alpha: 0-1
	};