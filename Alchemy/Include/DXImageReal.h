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

		Metric GetAlpha (void) const { return m_rA; }
		Metric GetBrightness (void) const { return m_rB; }
		Metric GetHue (void) const { return m_rH; }
		Metric GetSaturation (void) const { return m_rS; }

		void SetBrightness (Metric rBrightness) { m_rB = Max(0.0, Min(rBrightness, 1.0)); }
		void SetHue (Metric rHue) { m_rH = mathAngleModDegrees(rHue); }
		void SetSaturation (Metric rSaturation) { m_rS = Max(0.0, Min(rSaturation, 1.0)); }

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

		Metric GetAlpha (void) const { return m_rA; }
		Metric GetBlue (void) const { return m_rB; }
		Metric GetGreen (void) const { return m_rG; }
		Metric GetRed (void) const { return m_rR; }

		static CGRealRGB FromHSB (const CGRealHSB &hsbColor);
		static Metric From8bit (BYTE byValue) { return (Metric)byValue / 255.0; }
		static BYTE To8bit (Metric Value) { return (BYTE)(DWORD)(255.0 * Value); }

	private:
		Metric m_rR;						//	Red: 0-1
		Metric m_rG;						//	Green: 0-1
		Metric m_rB;						//	Blue: 0-1
		Metric m_rA;						//	Alpha: 0-1
	};

class CGRealChannel : public TImagePlane<CGRealChannel>
	{
	public:
		CGRealChannel (void);
		CGRealChannel (const CGRealChannel &Src);
		~CGRealChannel (void);

		CGRealChannel &operator= (const CGRealChannel &Src);

		//	Basic Interface

		void CleanUp (void);
		bool Create (int cxWidth, int cyHeight, Metric InitialValue = 0);
		bool Create (const CG8bitImage &Src, int xSrc = 0, int ySrc = 0, int cxSrc = -1, int cySrc = -1);
		bool CreateChannel (ChannelTypes iChannel, const CG32bitImage &Src, int xSrc = 0, int ySrc = 0, int cxSrc = -1, int cySrc = -1);
		Metric GetPixel (int x, int y) const { return *GetPixelPos(x, y); }
		Metric *GetPixelPos (int x, int y) const { return m_pChannel + (y * m_cxWidth) + x; }
		bool IsEmpty (void) const { return (m_pChannel == NULL); }
		bool IsMarked (void) const { return m_bMarked; }
		Metric *NextRow (Metric *pPos) const { return pPos + m_cxWidth; }
		void SetMarked (bool bMarked = true) { m_bMarked = bMarked; }

		//	Basic Drawing Interface

		void Fill (int x, int y, int cxWidth, int cyHeight, Metric Value);
		void FillColumn (int x, int y, int cyHeight, Metric Value) { Fill(x, y, 1, cyHeight, Value); }
		void FillLine (int x, int y, int cxWidth, Metric Value) { Fill(x, y, cxWidth, 1, Value); }
		void SetPixel (int x, int y, Metric Value) 
			{ if (x >= m_rcClip.left && y >= m_rcClip.top && x < m_rcClip.right && y < m_rcClip.bottom) *GetPixelPos(x, y) = Value; }

	private:
		static int CalcBufferSize (int cxWidth, int cyHeight) { return (cxWidth * cyHeight); }
		void Copy (const CGRealChannel &Src);

		Metric *m_pChannel;
		bool m_bMarked;						//	Mark/sweep flag (for use by caller)
	};
