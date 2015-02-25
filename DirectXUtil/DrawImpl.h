//	DrawImpl.h
//
//	Drawing implementation classes
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CGlowingRingPainter
	{
	public:
		CGlowingRingPainter (CG32bitImage &Dest, int iRadius, int iWidth, CG32bitPixel rgbColor);
		CGlowingRingPainter (CG32bitImage &Dest, int iRadius, int iWidth, const TArray<CG32bitPixel> &ColorRamp, BYTE byOpacity = 0xff);

		void Draw (int xCenter, int yCenter);

	private:
		void DrawLine (int x, int y);

		CG32bitImage &m_Dest;
		const RECT &m_rcClip;
		int m_xDest;
		int m_yDest;
		int m_iRadius;
		int m_iWidth;
		const TArray<CG32bitPixel> *m_pColorRamp;

		TArray<CG32bitPixel> m_ColorRamp;
	};

//	Blt Painters ---------------------------------------------------------------

#include "TBlt.h"
#include "NoiseImpl.h"

class CFilterNormal : public TBlt<CFilterNormal>
	{
	public:
		inline CG32bitPixel Filter (CG32bitPixel rgbSrc, CG32bitPixel *pDest) const { return rgbSrc; }
	};

class CFilterBackColor : public TBlt<CFilterBackColor>
	{
	public:
		CFilterBackColor (CG32bitPixel rgbBackColor) : m_rgbBackColor(rgbBackColor)
			{ }

		inline CG32bitPixel Filter (CG32bitPixel rgbSrc, CG32bitPixel *pDest) const
			{
			if (rgbSrc.GetRed() <= 0x01 && rgbSrc.GetGreen() <= 0x01 && rgbSrc.GetBlue() <= 0x01)
				return CG32bitPixel::Null();
			else
				return rgbSrc;
			}

	private:
		CG32bitPixel m_rgbBackColor;
	};

class CFilterTrans : public TBlt<CFilterTrans>
	{
	public:
		CFilterTrans (BYTE byOpacity) : m_byOpacity(byOpacity)
			{ }

		inline CG32bitPixel Filter (CG32bitPixel rgbSrc, CG32bitPixel *pDest) const
			{
			BYTE byAlpha = rgbSrc.GetAlpha();

			if (byAlpha == 0x00)
				return rgbSrc;

			else if (byAlpha == 0xff)
				return CG32bitPixel(CG32bitPixel::Blend(0, rgbSrc, m_byOpacity), m_byOpacity);

			else
				return CG32bitPixel(CG32bitPixel::Blend(0, rgbSrc, m_byOpacity), CG32bitPixel::BlendAlpha(byAlpha, m_byOpacity));
			}

	private:
		BYTE m_byOpacity;
	};

class CFilterDesaturate : public TBlt<CFilterDesaturate>
	{
	public:
		inline CG32bitPixel Filter (CG32bitPixel rgbSrc, CG32bitPixel *pDest) const
			{
			if (rgbSrc.GetAlpha() == 0x00)
				return rgbSrc;
			else
				return CG32bitPixel::Desaturate(rgbSrc);
			}
	};

class CFilterDesaturateTrans : public TBlt<CFilterDesaturate>
	{
	public:
		CFilterDesaturateTrans (BYTE byOpacity) : m_byOpacity(byOpacity)
			{ }

		inline CG32bitPixel Filter (CG32bitPixel rgbSrc, CG32bitPixel *pDest) const
			{
			BYTE byAlpha = rgbSrc.GetAlpha();

			if (byAlpha == 0x00)
				return rgbSrc;

			else if (byAlpha == 0xff)
				return CG32bitPixel(CG32bitPixel::Blend(0, CG32bitPixel::Desaturate(rgbSrc), m_byOpacity), m_byOpacity);

			else
				{
				CG32bitPixel rgbResult = CG32bitPixel::Blend(0, CG32bitPixel::Desaturate(rgbSrc), m_byOpacity);
				return CG32bitPixel(rgbResult, CG32bitPixel::BlendAlpha(byAlpha, m_byOpacity));
				}
			}

	private:
		BYTE m_byOpacity;
	};

class CFilterColorize : public TBlt<CFilterColorize>
	{
	public:
		CFilterColorize (REALPIXEL rHue, REALPIXEL rSaturation) :
				m_rHue(rHue),
				m_rSaturation(rSaturation),
				m_rgbBlack(CG32bitPixel(0, 0, 0)),
				m_rgbWhite(CG32bitPixel(0xff, 0xff, 0xff)),
				m_rgbHue(CGRealRGB::FromHSB(CGRealHSB(rHue, 1.0, 1.0))),
				m_rgbColor(CG32bitPixel::Blend(CG32bitPixel(0x80, 0x80, 0x80), m_rgbHue, rSaturation))
			{
			}

		inline CG32bitPixel Filter (CG32bitPixel rgbSrc, CG32bitPixel *pDest) const
			{
			//	See: Computer Graphics, Foley & van Dam, p.593.

			BYTE byBrightness = CG32bitPixel::Desaturate(rgbSrc).GetRed();
			REALPIXEL rB = byBrightness / 255.0;

			return CG32bitPixel::Blend3(m_rgbBlack, m_rgbColor, m_rgbWhite, 2.0 * (rB - 1.0) + 1.0);
			}

	private:
		REALPIXEL m_rHue;
		REALPIXEL m_rSaturation;

		CG32bitPixel m_rgbBlack;
		CG32bitPixel m_rgbWhite;
		CG32bitPixel m_rgbHue;
		CG32bitPixel m_rgbColor;
	};

class CFilterLighten : public TBlt<CFilterLighten>
	{
	public:
		inline CG32bitPixel Filter (CG32bitPixel rgbSrc, CG32bitPixel *pDest) const
			{
			BYTE bySrcAlpha = rgbSrc.GetAlpha();

			BYTE bySrcRed, bySrcGreen, bySrcBlue;
			if (bySrcAlpha == 0xff)
				{
				bySrcRed = rgbSrc.GetRed();
				bySrcGreen = rgbSrc.GetGreen();
				bySrcBlue = rgbSrc.GetBlue();
				}
			else
				{
				BYTE *pAlphaInv = CG32bitPixel::AlphaTable(bySrcAlpha ^ 0xff);	//	Equivalent to 255 - (*pAlphaPos)

				bySrcRed = pAlphaInv[pDest->GetRed()] + rgbSrc.GetRed();
				bySrcGreen = pAlphaInv[pDest->GetGreen()] + rgbSrc.GetGreen();
				bySrcBlue = pAlphaInv[pDest->GetBlue()] + rgbSrc.GetBlue();
				}

			DWORD dwTotalSrc = bySrcRed + bySrcGreen + bySrcBlue;
			DWORD dwTotalDest = pDest->GetRed() + pDest->GetGreen() + pDest->GetBlue();

			if (dwTotalSrc > dwTotalDest)
				return CG32bitPixel(bySrcRed, bySrcGreen, bySrcBlue);
			else
				return CG32bitPixel::Null();
			}

	};

class CFilterShimmer : public TBlt<CFilterShimmer>
	{
	public:
		CFilterShimmer (BYTE byOpacity, DWORD dwSeed) :
				m_byOpacity(byOpacity),
				m_dwRnd(LARGE_PRIME2 * dwSeed)
			{
			NoiseInit();
			}

		inline CG32bitPixel Filter (CG32bitPixel rgbSrc, CG32bitPixel *pDest) const
			{
			if (PERM((DWORD)pDest * LARGE_PRIME2 + m_dwRnd) < m_byOpacity)
				return rgbSrc;
			else
				return CG32bitPixel::Null();
			}

	private:
		enum EConstants
			{
			LARGE_PRIME1 = 433494437,
			LARGE_PRIME2 = 3010349,
			};

		BYTE m_byOpacity;
		DWORD m_dwRnd;

	};

//	Circle Painters ------------------------------------------------------------

#include "TCirclePainter.h"

class CImageCirclePainter : public TCirclePainter32<CImageCirclePainter>
	{
	public:
		CImageCirclePainter (int iRadius, BYTE byOpacity, const CG32bitImage &Src, int xSrc = 0, int ySrc = 0, int cxSrc = -1, int cySrc = -1);

		inline CG32bitPixel GetColorAt (int iAngle, int iRadius) const 
			{
			ASSERT(iAngle >= 0 && iAngle < m_iAngleRange);
			ASSERT(iRadius >= 0 && iRadius <= m_iRadius);

			int yOffset = m_iRadius - iRadius;
			if (yOffset >= m_cxSrc || m_byOpacity == 0x00)
				return CG32bitPixel::Null();

			CG32bitPixel rgbImage = *m_Src.GetPixelPos(m_xSrc + iAngle, m_ySrc + yOffset);

			if (m_byOpacity == 0xff)
				return rgbImage;
			else
				{
				BYTE byImageAlpha = rgbImage.GetAlpha();

				if (byImageAlpha == 0x00)
					return CG32bitPixel::Null();

				else if (byImageAlpha == 0xff)
					return CG32bitPixel(CG32bitPixel::Blend(0, rgbImage, m_byOpacity), m_byOpacity);
				else
					return CG32bitPixel(CG32bitPixel::Blend(0, rgbImage, m_byOpacity), CG32bitPixel::BlendAlpha(byImageAlpha, m_byOpacity));
				}
			}

	private:
		BYTE m_byOpacity;
		const CG32bitImage &m_Src;
		int m_xSrc;
		int m_ySrc;
		int m_cxSrc;
		int m_cySrc;
	};

class CRadialCirclePainter8 : public TRadialPainter8<CRadialCirclePainter8>
	{
	public:
		CRadialCirclePainter8 (int iRadius, BYTE CenterValue, BYTE EdgeValue);

		inline BYTE GetColorAt (int iRadius) const { return m_Ramp[iRadius]; }

	private:
		TArray<BYTE> m_Ramp;
	};

class CRadialCirclePainter : public TRadialPainter32<CRadialCirclePainter>
	{
	public:
		CRadialCirclePainter (int iRadius, const TArray<CG32bitPixel> &ColorRamp, bool bPreMult = false);

		inline CG32bitPixel GetColorAt (int iRadius) const { return m_pColorRamp->GetAt(iRadius); }

	private:
		const TArray<CG32bitPixel> *m_pColorRamp;
		TArray<CG32bitPixel> m_ColorRamp;
	};

//	Lines ----------------------------------------------------------------------

class CLinePainter
	{
	public:

		void DrawSolid (CG32bitImage &Image, int x1, int y1, int x2, int y2, int iWidth, CG32bitPixel rgbColor);
		void Rasterize (CG32bitImage &Image, int x1, int y1, int x2, int y2, int iWidth, TArray<CGRasterize::SLinePixel> *retPixels);

	private:
		enum ESlopeTypes
			{
			lineNull,						//	No line
			linePoint,						//	Single point
			lineXDominant,
			lineYDominant,
			};

		ESlopeTypes CalcIntermediates (const CG32bitImage &Image, int x1, int y1, int x2, int y2, int iWidth);
		void CalcPixelMapping (int x1, int y1, int x2, int y2, double *retrV, double *retrW);

		inline void NextX (int &x, int &y)
			{
			if (m_d >= 0)
				{
				y = y + m_sy;
				m_d = m_d - m_ax;
				m_rWUp += m_rWUpInc;
				m_rWDown -= m_rWDownDec;
				}

			m_d = m_d + m_ay;
			m_rWUp -= m_rWUpDec;
			m_rWDown += m_rWDownInc;
			}

		inline void NextY (int &x, int &y)
			{
			if (m_d >= 0)
				{
				x = x + m_sx;
				m_d = m_d - m_ay;
				m_rWUp += m_rWUpInc;
				m_rWDown -= m_rWDownDec;
				}

			m_d = m_d + m_ax;
			m_rWUp -= m_rWUpDec;
			m_rWDown += m_rWDownInc;
			}

		inline void NextX (int &x, int &y, double &rV, double &rW)
			{
			if (m_d >= 0)
				{
				y = y + m_sy;
				m_d = m_d - m_ax;
				m_rWUp += m_rWUpInc;
				m_rWDown -= m_rWDownDec;

				rV += m_rVIncY;
				rW += m_rWIncY;
				}

			m_d = m_d + m_ay;
			m_rWUp -= m_rWUpDec;
			m_rWDown += m_rWDownInc;

			rV += m_rVIncX;
			rW += m_rWIncX;
			}

		inline void NextY (int &x, int &y, double &rV, double &rW)
			{
			if (m_d >= 0)
				{
				x = x + m_sx;
				m_d = m_d - m_ay;
				m_rWUp += m_rWUpInc;
				m_rWDown -= m_rWDownDec;

				rV += m_rVIncX;
				rW += m_rWIncX;
				}

			m_d = m_d + m_ax;
			m_rWUp -= m_rWUpDec;
			m_rWDown += m_rWDownInc;

			rV += m_rVIncY;
			rW += m_rWIncY;
			}

		int m_dx;							//	Distance from x1 to x2
		int m_sx;							//	Direction (from x1 to x2)
		int m_ax;							//	absolute distance x (times 2)
		double m_rL;						//	Length of line
		int m_xStart;
		int m_xEnd;

		int m_dy;							//	Distance from y1 to y2
		int m_sy;							//	Direction (from y1 to y2)
		int m_ay;							//	absolute distance y (times 2)
		int m_yStart;
		int m_yEnd;

		int m_d;							//	Discriminator

		double m_rHalfWidth;				//	Half of width;
		int m_iAxisHalfWidth;				//	Half line width aligned on dominant axis
		int m_iAxisWidth;					//	Full width of line aligned on dominant axis

		double m_rWDown;					//	Intermediates for line width
		double m_rWDownDec;
		double m_rWDownInc;
		double m_rWUp;
		double m_rWUpDec;
		double m_rWUpInc;
		int m_wMin;
		int m_wMax;

		int m_iPosRowInc;					//	Image row offset

		double m_rVPerX;					//	Movement along V (line axis) for every X movement
		double m_rVPerY;					//	Movement along V for every Y movement
		double m_rWPerX;					//	Movement along W (perp axis) for every X movement
		double m_rWPerY;					//	Movement along W for every Y movement

		double m_rVIncX;
		double m_rVIncY;
		double m_rWIncX;
		double m_rWIncY;
	};

extern const BYTE g_wSmallRoundMask[9];
