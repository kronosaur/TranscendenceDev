//	DrawImpl.h
//
//	Drawing implementation classes
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

#include "NoiseImpl.h"

//	Glow Ring Painter ----------------------------------------------------------

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

class CFilterNormal : public TBlt<CFilterNormal>
	{
	public:
		CG32bitPixel Filter (CG32bitPixel rgbSrc, CG32bitPixel *pDest) const { return rgbSrc; }
	};

class CFilterBackColor : public TBlt<CFilterBackColor>
	{
	public:
		CFilterBackColor (CG32bitPixel rgbBackColor) : m_rgbBackColor(rgbBackColor)
			{ }

		CG32bitPixel Filter (CG32bitPixel rgbSrc, CG32bitPixel *pDest) const
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

		CG32bitPixel Filter (CG32bitPixel rgbSrc, CG32bitPixel *pDest) const
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
		CG32bitPixel Filter (CG32bitPixel rgbSrc, CG32bitPixel *pDest) const
			{
			if (rgbSrc.GetAlpha() == 0x00)
				return rgbSrc;
			else
				return CG32bitPixel::Desaturate(rgbSrc);
			}
	};

class CFilterDesaturateTrans : public TBlt<CFilterDesaturateTrans>
	{
	public:
		CFilterDesaturateTrans (BYTE byOpacity) : m_byOpacity(byOpacity)
			{ }

		CG32bitPixel Filter (CG32bitPixel rgbSrc, CG32bitPixel *pDest) const
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
		CFilterColorize (Metric rHue, Metric rSaturation) :
				m_rHue(rHue),
				m_rSaturation(rSaturation),
				m_rgbBlack(CG32bitPixel(0, 0, 0)),
				m_rgbWhite(CG32bitPixel(0xff, 0xff, 0xff)),
				m_rgbHue(CGRealRGB::FromHSB(CGRealHSB(rHue, 1.0, 1.0))),
				m_rgbColor(CG32bitPixel::Blend(CG32bitPixel(0x80, 0x80, 0x80), m_rgbHue, rSaturation))
			{
			}

		CG32bitPixel Filter (CG32bitPixel rgbSrc, CG32bitPixel *pDest) const
			{
			//	See: Computer Graphics, Foley & van Dam, p.593.

			BYTE byBrightness = CG32bitPixel::Desaturate(rgbSrc).GetRed();
			Metric rB = byBrightness / 255.0;

			return CG32bitPixel::Blend3(m_rgbBlack, m_rgbColor, m_rgbWhite, 2.0 * (rB - 1.0) + 1.0);
			}

	private:
		Metric m_rHue;
		Metric m_rSaturation;

		CG32bitPixel m_rgbBlack;
		CG32bitPixel m_rgbWhite;
		CG32bitPixel m_rgbHue;
		CG32bitPixel m_rgbColor;
	};

class CFilterLighten : public TBlt<CFilterLighten>
	{
	public:
		CG32bitPixel Filter (CG32bitPixel rgbSrc, CG32bitPixel *pDest) const
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

class CFilterMask0 : public TBlt<CFilterMask0>
	{
	public:
		CG32bitPixel Filter (CG32bitPixel rgbSrc, CG32bitPixel *pDest) const
			{
			if (Max(Max(rgbSrc.GetRed(), rgbSrc.GetGreen()), rgbSrc.GetBlue()) <= 7)
				return CG32bitPixel::Null();
			else
				return rgbSrc;
			}
	};

class CFilterShimmer : public TBlt<CFilterShimmer>
	{
	public:
		CFilterShimmer (BYTE byOpacity, DWORD dwSeed) :
				m_byOpacity(byOpacity),
				m_dwRnd(LARGE_PRIME2 * (1 + dwSeed))
			{
			NoiseInit();
			}

		CG32bitPixel Filter (CG32bitPixel rgbSrc, CG32bitPixel *pDest) const
			{
			if (PERM(((DWORD)pDest >> 2) * LARGE_PRIME2 + m_dwRnd) < m_byOpacity)
				return rgbSrc;
			else
				return CG32bitPixel::Null();
			}

		void StartRow (CG32bitPixel *pSrc, CG32bitPixel *pDest)
			{
			m_dwRnd *= LARGE_PRIME1;
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

template <class BLENDER> class TBltImageNormal : public TImagePainter<TBltImageNormal<BLENDER>, BLENDER>
	{
	private:
		CG32bitPixel Filter (CG32bitPixel rgbSrc, CG32bitPixel *pDest) const { return rgbSrc; }

	friend TImagePainter;
	};

template <class BLENDER> class TBltImageTrans : public TImagePainter<TBltImageTrans<BLENDER>, BLENDER>
	{
	public:
		TBltImageTrans (BYTE byOpacity) :
				m_byOpacity(byOpacity)
			{ }

	private:
		CG32bitPixel Filter (CG32bitPixel rgbSrc, CG32bitPixel *pDest) const
			{
			BYTE byAlpha = rgbSrc.GetAlpha();

			if (byAlpha == 0x00)
				return rgbSrc;

			else if (byAlpha == 0xff)
				return CG32bitPixel(CG32bitPixel::Blend(0, rgbSrc, m_byOpacity), m_byOpacity);

			else
				return CG32bitPixel(CG32bitPixel::Blend(0, rgbSrc, m_byOpacity), CG32bitPixel::BlendAlpha(byAlpha, m_byOpacity));
			}

		BYTE m_byOpacity;

	friend TImagePainter;
	};

template <class BLENDER> class TFillImageSolid : public TImagePainter<TFillImageSolid<BLENDER>, BLENDER>
	{
	public:
		TFillImageSolid (CG32bitPixel rgbColor) :
				m_rgbColor(rgbColor)
			{ }

	private:
		CG32bitPixel GetPixelAt (int x, int y) const { return m_rgbColor; }

		CG32bitPixel m_rgbColor;

	friend TImagePainter;
	};

//	Fill Painters --------------------------------------------------------------

class CSolidFill
	{
	public:
		CSolidFill (CG32bitPixel rgbColor) :
				m_rgbColor(rgbColor)
			{ }

		CG32bitPixel GetColor (void) const { return m_rgbColor; }

	private:
		CG32bitPixel m_rgbColor;
	};

//	Circle Painters ------------------------------------------------------------

template <class BLENDER> class CImageCirclePainter : public TCirclePainter32<CImageCirclePainter<BLENDER>, BLENDER>
	{
	public:
		CImageCirclePainter (int iRadius, BYTE byOpacity, const CG32bitImage &Src, int xSrc = 0, int ySrc = 0, int cxSrc = -1, int cySrc = -1) : TCirclePainter32((cxSrc >= 0 ? cxSrc : Src.GetWidth()), iRadius),
				m_byOpacity(byOpacity),
				m_Src(Src),
				m_xSrc(xSrc),
				m_ySrc(ySrc),
				m_cxSrc(cxSrc >= 0 ? cxSrc : Src.GetWidth()),
				m_cySrc(cySrc >= 0 ? cySrc : Src.GetHeight())
			{ }


		CG32bitPixel GetColorAt (int iAngle, int iRadius) const 
			{
			ASSERT(iAngle >= 0 && iAngle < m_iAngleRange);
			ASSERT(iRadius >= 0 && iRadius <= m_iRadius);

			int yOffset = m_iRadius - iRadius;
			if (yOffset >= m_cySrc || m_byOpacity == 0x00)
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

		BYTE GetColorAt (int iRadius) const { return m_Ramp[iRadius]; }

	private:
		TArray<BYTE> m_Ramp;
	};

template <class BLENDER> class TFillCircleSolid : public TRadialPainter32<TFillCircleSolid<BLENDER>, BLENDER>
	{
	public:
		TFillCircleSolid (int iRadius, CG32bitPixel rgbColor) : TRadialPainter32(iRadius),
				m_rgbColor(CG32bitPixel::PreMult(rgbColor))
			{ }

	private:
		CG32bitPixel GetColorAt (int iRadius) const { return m_rgbColor; }

		CG32bitPixel m_rgbColor;

	friend TRadialPainter32;
	};

template <class BLENDER> class CRadialCirclePainter : public TRadialPainter32<CRadialCirclePainter<BLENDER>, BLENDER>
	{
	public:
		CRadialCirclePainter (int iRadius, const TArray<CG32bitPixel> &ColorRamp, bool bPreMult = false) : TRadialPainter32(iRadius)
			{
			int i;

			//	If the ramp is not pre-multiplied, then we need to do that now

			if (!bPreMult)
				{
				//	Pre-multiply the color ramp

				m_ColorRamp.InsertEmpty(ColorRamp.GetCount());
				m_pColorRamp = &m_ColorRamp;

				for (i = 0; i < m_ColorRamp.GetCount(); i++)
					m_ColorRamp[i] = CG32bitPixel::PreMult(ColorRamp[i]);
				}

			//	Otherwise we just take the ramp we've been given

			else
				m_pColorRamp = &ColorRamp;
			}

	private:
		CG32bitPixel GetColorAt (int iRadius) const { return m_pColorRamp->GetAt(iRadius); }

		const TArray<CG32bitPixel> *m_pColorRamp;
		TArray<CG32bitPixel> m_ColorRamp;

		friend TRadialPainter32;
	};

extern const BYTE g_wSmallRoundMask[9];
