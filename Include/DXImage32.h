//	DXImage32.h
//
//	Implements 32-bit Images
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CG8bitImage;
class CG16bitFont;
class CG16bitImage;
class CGRealRGB;
class CG16bitBinaryRegion;

class CG32bitPixel
	{
	public:
		CG32bitPixel (void) { }

		CG32bitPixel (BYTE byRed, BYTE byGreen, BYTE byBlue)
			{ m_dwPixel = 0xff000000 | ((DWORD)byRed << 16) | ((DWORD)byGreen << 8) | (DWORD)byBlue; }

		CG32bitPixel (BYTE byRed, BYTE byGreen, BYTE byBlue, BYTE byAlpha)
			{ m_dwPixel = ((DWORD)byAlpha << 24) | ((DWORD)byRed << 16) | ((DWORD)byGreen << 8) | (DWORD)byBlue; }

		CG32bitPixel (const CG32bitPixel &Src, BYTE byNewAlpha)
			{ m_dwPixel = (byNewAlpha << 24) | (Src.m_dwPixel & 0x00ffffff); }

		CG32bitPixel (WORD wPixel);
		CG32bitPixel (const CGRealRGB &rgbColor);

		operator const COLORREF () { return RGB(GetRed(), GetGreen(), GetBlue()); }

		inline bool operator == (const CG32bitPixel &vA) const { return (m_dwPixel == vA.m_dwPixel); }
		inline bool operator != (const CG32bitPixel &vA) const { return (m_dwPixel != vA.m_dwPixel); }

		inline DWORD AsDWORD (void) const { return m_dwPixel; }
		inline DWORD AsR5G5B5 (void) const { return (((m_dwPixel & 0x00f80000) >> 9) | ((m_dwPixel & 0x0000f800) >> 6) | ((m_dwPixel & 0x000000f8) >> 3)); }
		inline DWORD AsR5G6B5 (void) const { return (((m_dwPixel & 0x00f80000) >> 8) | ((m_dwPixel & 0x0000fc00) >> 5) | ((m_dwPixel & 0x000000f8) >> 3)); }
		inline DWORD AsR8G8B8 (void) const { return m_dwPixel; }
		inline BYTE GetAlpha (void) const { return (BYTE)((m_dwPixel & 0xff000000) >> 24); }
		inline BYTE GetBlue (void) const { return (BYTE)(m_dwPixel & 0x000000ff); }
		inline BYTE GetGreen (void) const { return (BYTE)((m_dwPixel & 0x0000ff00) >> 8); }
		inline BYTE GetMax (void) const { return Max(Max(GetRed(), GetGreen()), GetBlue()); }
		inline BYTE GetRed (void) const { return (BYTE)((m_dwPixel & 0x00ff0000) >> 16); }
		inline bool IsNull (void) const { return (m_dwPixel == 0); }
		inline void SetAlpha (BYTE byValue) { m_dwPixel = (m_dwPixel & 0x00ffffff) | ((DWORD)byValue << 24); }
		inline void SetBlue (BYTE byValue) { m_dwPixel = (m_dwPixel & 0xffffff00) | (DWORD)byValue; }
		inline void SetGreen (BYTE byValue) { m_dwPixel = (m_dwPixel & 0xffff00ff) | ((DWORD)byValue << 8); }
		inline void SetRed (BYTE byValue) { m_dwPixel = (m_dwPixel & 0xff00ffff) | ((DWORD)byValue << 16); }

		inline static BYTE *AlphaTable (BYTE byOpacity) { return g_Alpha8[byOpacity]; }
		static CG32bitPixel Blend (CG32bitPixel rgbDest, CG32bitPixel rgbSrc);
		static CG32bitPixel Blend (CG32bitPixel rgbDest, CG32bitPixel rgbSrc, BYTE bySrcAlpha);
		static CG32bitPixel Blend (CG32bitPixel rgbFrom, CG32bitPixel rgbTo, double rFade);
		static CG32bitPixel Blend3 (CG32bitPixel rgbNegative, CG32bitPixel rgbCenter, CG32bitPixel rgbPositive, double rFade);
		inline static BYTE BlendAlpha (BYTE byDest, BYTE bySrc) { return (BYTE)((DWORD)byDest * (DWORD)bySrc / 255); }
		static CG32bitPixel Composite (CG32bitPixel rgbDest, CG32bitPixel rgbSrc);
		static CG32bitPixel Composite (CG32bitPixel rgbFrom, CG32bitPixel rgbTo, double rFade);
		inline static BYTE CompositeAlpha (BYTE byDest, BYTE bySrc) { return (BYTE)255 - (BYTE)(((DWORD)(255 - byDest) * (DWORD)(255 - bySrc)) / 255); }
		static CG32bitPixel Darken (CG32bitPixel rgbSource, BYTE byOpacity);
		static CG32bitPixel Desaturate (CG32bitPixel rgbColor);
		static CG32bitPixel Fade (CG32bitPixel rgbFrom, CG32bitPixel rgbTo, int iPercent);
		inline static CG32bitPixel FromDWORD (DWORD dwValue) { return CG32bitPixel(dwValue, true); }
		inline static CG32bitPixel FromGrayscale (BYTE byValue) { return CG32bitPixel(byValue, byValue, byValue); }
		inline static CG32bitPixel FromGrayscale (BYTE byValue, BYTE byAlpha) { return CG32bitPixel(byValue, byValue, byValue, byAlpha); }
		static bool Init (void);
		inline static CG32bitPixel Null (void) { return CG32bitPixel(0, true); }
		inline static CG32bitPixel PreMult (CG32bitPixel rgbColor) { return PreMult(rgbColor, rgbColor.GetAlpha()); }
		static CG32bitPixel PreMult (CG32bitPixel rgbColor, BYTE byAlpha);
		static CG32bitPixel Screen (CG32bitPixel rgbDest, CG32bitPixel rgbSrc);

	private:
		CG32bitPixel (DWORD dwValue, bool bRaw) : m_dwPixel(dwValue) { }

		DWORD m_dwPixel;

		typedef BYTE AlphaArray8 [256];
		static AlphaArray8 g_Alpha8 [256];
		static bool m_bAlphaInitialized;
	};

//	CG32bitImage
//
//	This class manages a 32-bit image. Each pixel has the following bit-pattern:
//
//	33222222 22221111 11111100 00000000
//	10987654 32109876 54321098 76543210
//
//	AAAAAAAA RRRRRRRR GGGGGGGG BBBBBBBB

class CG32bitImage : public CGImagePlane
	{
	public:
		enum EAlphaTypes
			{
			alphaNone,						//	Ignore alpha byte
			alpha1,							//	1-bit alpha (non-zero = opaque)
			alpha8,							//	8-bit alpha
			};

		enum EFlags
			{
			//	CreateFromBitmap

			FLAG_PRE_MULT_ALPHA =			0x00000001,
			};

		CG32bitImage (void);
		CG32bitImage (const CG32bitImage &Src);
		~CG32bitImage (void);

		CG32bitImage &operator= (const CG32bitImage &Src);

		inline static CG32bitImage &Null (void) { return m_NullImage; }

		//	Basic Interface

		void CleanUp (void);
		bool Create (int cxWidth, int cyHeight, EAlphaTypes AlphaType = alphaNone, CG32bitPixel InitialValue = 0);
		bool CreateFromBitmap (HBITMAP hImage, HBITMAP hMask = NULL, EBitmapTypes iMaskType = bitmapNone, DWORD dwFlags = 0);
		bool CreateFromExternalBuffer (void *pBuffer, int cxWidth, int cyHeight, int iPitch, EAlphaTypes AlphaType = alphaNone);
		bool CreateFromFile (const CString &sImageFilespec, const CString &sMaskFilespec = NULL_STR, DWORD dwFlags = 0);
		bool CreateFromImageTransformed (const CG32bitImage &Source, int xSrc, int ySrc, int cxSrc, int cySrc, Metric rScaleX, Metric rScaleY, Metric rRotation = 0.0);
		inline EAlphaTypes GetAlphaType (void) const { return m_AlphaType; }
		inline CG32bitPixel GetPixel (int x, int y) const { return *GetPixelPos(x, y); }
		inline CG32bitPixel *GetPixelPos (int x, int y) const { return (CG32bitPixel *)((BYTE *)m_pRGBA + (y * m_iPitch)) + x; }
		inline bool IsEmpty (void) const { return (m_pRGBA == NULL); }
		inline CG32bitPixel *NextRow (CG32bitPixel *pPos) const { return (CG32bitPixel *)((BYTE *)pPos + m_iPitch); }

		//	Basic Drawing Interface

		void Blt (int xSrc, int ySrc, int cxWidth, int cyHeight, BYTE byOpacity, const CG32bitImage &Source, int xDest, int yDest);
		inline void Blt (int xSrc, int ySrc, int cxWidth, int cyHeight, const CG32bitImage &Source, int xDest, int yDest) { Blt(xSrc, ySrc, cxWidth, cyHeight, 0xff, Source, xDest, yDest); }
		void BltMask (int xSrc, int ySrc, int cxWidth, int cyHeight, const CG32bitImage &Mask, const CG32bitImage &Source, int xDest, int yDest);
		void Composite (int xSrc, int ySrc, int cxWidth, int cyHeight, BYTE byOpacity, const CG32bitImage &Source, int xDest, int yDest);
		void Copy (int xSrc, int ySrc, int cxWidth, int cyHeight, const CG32bitImage &Source, int xDest, int yDest);
		void CopyChannel (ChannelTypes iChannel, int xSrc, int ySrc, int cxWidth, int cyHeight, const CG32bitImage &Source, int xDest, int yDest);
		void CopyTransformed (const RECT &rcDest, const CG32bitImage &Src, const RECT &rcSrc, const CXForm &SrcToDest, const CXForm &DestToSrc, const RECT &rcDestXForm);
		void DrawDot (int x, int y, CG32bitPixel rgbColor, MarkerTypes iMarker);
		inline void DrawLine (int x1, int y1, int x2, int y2, int iWidth, CG32bitPixel rgbColor);
		inline void DrawText (int x, int y, const CG16bitFont &Font, CG32bitPixel rgbColor, CString sText, DWORD dwFlags = 0, int *retx = NULL);
		void Fill (int x, int y, int cxWidth, int cyHeight, CG32bitPixel Value);
		void Fill (CG32bitPixel Value);
		inline void FillColumn (int x, int y, int cyHeight, CG32bitPixel Value) { Fill(x, y, 1, cyHeight, Value); }
		inline void FillLine (int x, int y, int cxWidth, CG32bitPixel Value) { Fill(x, y, cxWidth, 1, Value); }
		void FillMask (int xSrc, int ySrc, int cxWidth, int cyHeight, const CG32bitImage &Source, CG32bitPixel rgbColor, int xDest, int yDest);
		void Set (CG32bitPixel Value);
		void Set (int x, int y, int cxWidth, int cyHeight, CG32bitPixel Value);
		void Set (CG32bitImage &Src);
		void SetMask (int xSrc, int ySrc, int cxWidth, int cyHeight, const CG32bitImage &Source, CG32bitPixel rgbColor, int xDest, int yDest);
		inline void SetPixel (int x, int y, CG32bitPixel rgbColor) 
			{ if (x >= m_rcClip.left && y >= m_rcClip.top && x < m_rcClip.right && y < m_rcClip.bottom) *GetPixelPos(x, y) = rgbColor; }
		inline void SetPixelTrans (int x, int y, CG32bitPixel rgbColor, BYTE byOpacity)
			{ if (x >= m_rcClip.left && y >= m_rcClip.top && x < m_rcClip.right && y < m_rcClip.bottom) { CG32bitPixel *pDest = GetPixelPos(x, y); *pDest = CG32bitPixel::Blend(*pDest, rgbColor, byOpacity); } }

		//	16-bit Drawing Functions

		void Blt (int xSrc, int ySrc, int cxWidth, int cyHeight, BYTE byOpacity, const CG16bitImage &Source, int xDest, int yDest);
		void FillMask (int xSrc, int ySrc, int cxWidth, int cyHeight, const CG16bitImage &Source, CG32bitPixel rgbColor, int xDest, int yDest);

		//	8-bit Drawing Functions

		void CopyChannel (ChannelTypes iChannel, int xSrc, int ySrc, int cxWidth, int cyHeight, const CG8bitImage &Source, int xDest, int yDest);
		void FillMask (int xSrc, int ySrc, int cxWidth, int cyHeight, const CG8bitImage &Source, CG32bitPixel rgbColor, int xDest, int yDest);
		void FillMaskScaled (int xSrc, int ySrc, int cxSrc, int cySrc, const CG8bitImage &Source, CG32bitPixel rgbColor, int xDest, int yDest, int cxDest, int cyDest);
		void IntersectMask (int xMask, int yMask, int cxMask, int cyMask, const CG8bitImage &Mask, int xDest, int yDest);
		void SetMask (int xSrc, int ySrc, int cxWidth, int cyHeight, const CG8bitImage &Mask, CG32bitPixel rgbColor, int xDest, int yDest);

		//	DX and Window Functions

		void BltToDC (HDC hDC, int x, int y) const;
		void BltToSurface (LPDIRECTDRAWSURFACE7 pSurface, SurfaceTypes iType);
		bool CopyToClipboard (void);
		bool SaveAsWindowsBMP (const CString &sFilespec);
		void SwapBuffers (CG32bitImage &Other);
		bool WriteToWindowsBMP (IWriteStream *pStream);

	private:
		static int CalcBufferSize (int cxWidth, int cyHeight) { return (cxWidth * cyHeight); }
		void Copy (const CG32bitImage &Src);
		void InitBMI (BITMAPINFO **retpbi) const;

		CG32bitPixel *m_pRGBA;
		bool m_bFreeRGBA;					//	If TRUE, we own the memory
		int m_iPitch;						//	Bytes per row
		EAlphaTypes m_AlphaType;

		mutable BITMAPINFO *m_pBMI;			//	Used for blting to a DC
		static CG32bitImage m_NullImage;
	};

//	Drawing Classes ------------------------------------------------------------

class CGComposite
	{
	public:

		//	Circles

		static void Circle (CG32bitImage &Dest, int x, int y, int iRadius, const TArray<CG32bitPixel> &ColorRamp);

	};

class CGDraw
	{
	public:

		enum EBlendModes
			{
			blendNone =					-1,

			blendNormal =				0,	//	Normal drawing
			blendMultiply =				1,	//	Darkens images
			blendOverlay =				2,	//	Combine multiply/screen
			blendScreen =				3,	//	Brightens images
			blendHardLight =			4,

			//	See BlendModes.cpp to add new blend modes

			blendModeCount =			5,
			};

		//	Blts

		static void BltGray (CG32bitImage &Dest, int xDest, int yDest, CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc, BYTE byOpacity = 0xff);
		static void BltLighten (CG32bitImage &Dest, int xDest, int yDest, CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc);
		static void BltMask0 (CG32bitImage &Dest, int xDest, int yDest, const CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc);
		static void BltScaled (CG32bitImage &Dest, int xDest, int yDest, int cxDest, int cyDest, CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc);
		static void BltShimmer (CG32bitImage &Dest, int xDest, int yDest, CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc, BYTE byOpacity, DWORD dwSeed);
		static void BltTiled (CG32bitImage &Dest, int xDest, int yDest, int cxDest, int cyDest, const CG32bitImage &Src, int xSrc = 0, int ySrc = 0, int cxSrc = -1, int cySrc = -1, int xSrcOffset = 0, int ySrcOffset = 0);
		static void BltTransformed (CG32bitImage &Dest, Metric rX, Metric rY, Metric rScaleX, Metric rScaleY, Metric rRotation, const CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc);
		static void BltTransformedGray (CG32bitImage &Dest, Metric rX, Metric rY, Metric rScaleX, Metric rScaleY, Metric rRotation, const CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc, BYTE byOpacity = 0xff);
		static void BltWithBackColor (CG32bitImage &Dest, int xDest, int yDest, CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc, CG32bitPixel rgbBackColor);
		static void CopyColorize (CG32bitImage &Dest, int xDest, int yDest, CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc, Metric rHue, Metric rSaturation);

		//	Lines

		static void Arc (CG32bitImage &Dest, int xCenter, int yCenter, int iRadius, int iStartAngle, int iEndAngle, int iLineWidth, CG32bitPixel rgbColor);
		inline static void Line (CG32bitImage &Dest, int x1, int y1, int x2, int y2, int iWidth, CG32bitPixel rgbColor) 
			{ if (rgbColor.GetAlpha() == 0xff) LineBresenham(Dest, x1, y1, x2, y2, iWidth, rgbColor); else LineBresenhamTrans(Dest, x1, y1, x2, y2, iWidth, rgbColor); }

		static void LineBroken (CG32bitImage &Dest, int xSrc, int ySrc, int xDest, int yDest, int xyBreak, CG32bitPixel rgbColor);
		static void LineDotted (CG32bitImage &Dest, int x1, int y1, int x2, int y2, CG32bitPixel rgbColor);
		static void LineGradient (CG32bitImage &Dest, int x1, int y1, int x2, int y2, int iWidth, CG32bitPixel rgbColor1, CG32bitPixel rgbColor2);
		static void LineHD (CG32bitImage &Dest, int x1, int y1, int x2, int y2, int iWidth, CG32bitPixel rgbColor);

		//	Rects

		static void OctaRectOutline (CG32bitImage &Dest, int x, int y, int cxWidth, int cyHeight, int iCorner, int iLineWidth, CG32bitPixel rgbColor);
		static void RectGradient (CG32bitImage &Dest, int x, int y, int cxWidth, int cyHeight, CG32bitPixel rgbStart, CG32bitPixel rgbEnd, GradientDirections iDir = gradientHorizontal);
		static void RectOutlineDotted (CG32bitImage &Dest, int x, int y, int cxWidth, int cyHeight, CG32bitPixel rgbColor);
		static void RoundedRect (CG32bitImage &Dest, int x, int y, int cxWidth, int cyHeight, int iRadius, CG32bitPixel rgbColor);
		static void RoundedRectOutline (CG32bitImage &Dest, int x, int y, int cxWidth, int cyHeight, int iRadius, int iLineWidth, CG32bitPixel rgbColor);

		//	Regions

		static void Region (CG32bitImage &Dest, int x, int y, const CG16bitBinaryRegion &Region, CG32bitPixel rgbColor, EBlendModes iMode = blendNormal);

		//	Circles

		static void Circle (CG8bitImage &Dest, int x, int y, int iRadius, BYTE Value);
		static void Circle (CG32bitImage &Dest, int x, int y, int iRadius, CG32bitPixel rgbColor);
		static void Circle (CG32bitImage &Dest, int x, int y, int iRadius, const TArray<CG32bitPixel> &ColorRamp, EBlendModes iMode = blendNormal, bool bPreMult = false);
		static void CircleImage (CG32bitImage &Dest, int x, int y, int iRadius, BYTE byOpacity, const CG32bitImage &Image, EBlendModes iMode = blendNormal, int xSrc = 0, int ySrc = 0, int cxSrc = -1, int cySrc = -1);
		static void CircleGradient (CG8bitImage &Dest, int x, int y, int iRadius, BYTE CenterValue, BYTE EdgeValue);
		static void CircleGradient (CG32bitImage &Dest, int x, int y, int iRadius, CG32bitPixel rgbColor, EBlendModes iMode = blendNormal);
		static void RingGlowing (CG32bitImage &Dest, int x, int y, int iRadius, int iWidth, CG32bitPixel rgbColor);
		static void RingGlowing (CG32bitImage &Dest, int x, int y, int iRadius, int iWidth, const TArray<CG32bitPixel> &ColorRamp, BYTE byOpacity = 0xff);

		//	Curves

		static void QuadCurve (CG32bitImage &Dest, int x1, int y1, int x2, int y2, int xMid, int yMid, int iLineWidth, CG32bitPixel rgbColor);

		//	Miscellaneous

		static EBlendModes ParseBlendMode (const CString &sValue);

	private:
		struct SGlowRingLineCtx
			{
			CG32bitImage *pDest;
			int xDest;
			int yDest;
			int iRadius;
			int iWidth;
			const TArray<CG32bitPixel> &ColorRamp;

			DWORD *dwRed;
			DWORD *dwGreen;
			DWORD *dwBlue;
			};

		static void LineBresenham (CG32bitImage &Dest, int x1, int y1, int x2, int y2, int iWidth, CG32bitPixel rgbColor);
		static void LineBresenhamGradient (CG32bitImage &Dest, int x1, int y1, int x2, int y2, int iWidth, CG32bitPixel rgbColor1, CG32bitPixel rgbColor2);
		static void LineBresenhamGradientTrans (CG32bitImage &Dest, int x1, int y1, int x2, int y2, int iWidth, CG32bitPixel rgbColor1, CG32bitPixel rgbColor2);
		static void LineBresenhamTrans (CG32bitImage &Dest, int x1, int y1, int x2, int y2, int iWidth, CG32bitPixel rgbColor);

		static void QuadCurve (CG32bitImage &Dest, const CVector &vP1, const CVector &vP2, const CVector &vC1, int iLineWidth, CG32bitPixel rgbColor);
	};

class CGRasterize
	{
	public:

		//	Lines

		struct SLinePixel
			{
			CG32bitPixel *pPos;				//	Pixel to draw on
			BYTE byAlpha;					//	Line opacity at this point
			int x;							//	x-coordinate on image
			int y;							//	y-coordinate on image

			double rV;						//	Distance from start to end along line (0 to 1.0)
			double rW;						//	Distance perpendicular to line axis (0 to 1.0)
			};

		static void Line (CG32bitImage &Dest, int x1, int y1, int x2, int y2, int iWidth, TArray<SLinePixel> *retPixels);
	};

//	Blending Classes -----------------------------------------------------------
//
//	The following methods are defined:
//
//	Blend: Combines two pixels where rgbSource may have alpha transparency (though it may be 0 or 0xff).
//	BlendPreMult: Same as Blend but we assume that RGB of source has already been multiplied by its alpha.
//	Copy: Combines two pixels where rgbSource is guaranteed to have alpha 0xff.

template <class BLENDER> class TBlendImpl
	{
	public:
		inline static void SetBlend (CG32bitPixel *pDest, CG32bitPixel rgbSource) { *pDest = BLENDER::Blend(*pDest, rgbSource); }
		inline static void SetBlendPreMult (CG32bitPixel *pDest, CG32bitPixel rgbSource) { *pDest = BLENDER::BlendPreMult(*pDest, rgbSource); }
		inline static void SetCopy (CG32bitPixel *pDest, CG32bitPixel rgbSource) { *pDest = BLENDER::Copy(*pDest, rgbSource); }
	};

class CGBlendBlend : public TBlendImpl<CGBlendBlend>
	{
	public:
		inline static CG32bitPixel Blend (CG32bitPixel rgbDest, CG32bitPixel rgbSource) { return CG32bitPixel::Blend(rgbDest, rgbSource); }

		inline static CG32bitPixel BlendPreMult (CG32bitPixel rgbDest, CG32bitPixel rgbSource) 
			{
			BYTE *pAlphaInv = CG32bitPixel::AlphaTable(rgbSource.GetAlpha() ^ 0xff);	//	Equivalent to 255 - rgbSrc.GetAlpha()

			BYTE byRedResult = pAlphaInv[rgbDest.GetRed()] + rgbSource.GetRed();
			BYTE byGreenResult = pAlphaInv[rgbDest.GetGreen()] + rgbSource.GetGreen();
			BYTE byBlueResult = pAlphaInv[rgbDest.GetBlue()] + rgbSource.GetBlue();

			return CG32bitPixel(byRedResult, byGreenResult, byBlueResult);
			}

		inline static CG32bitPixel Copy (CG32bitPixel rgbDest, CG32bitPixel rgbSource) { return rgbSource; }
	};

class CGBlendComposite : public TBlendImpl<CGBlendComposite>
	{
	public:
		//	LATER
		inline static CG32bitPixel Blend (CG32bitPixel rgbDest, CG32bitPixel rgbSource) { return rgbSource; }
		inline static CG32bitPixel BlendPreMult (CG32bitPixel rgbDest, CG32bitPixel rgbSource) { return rgbSource; }
		inline static CG32bitPixel Copy (CG32bitPixel rgbDest, CG32bitPixel rgbSource) { return rgbSource; }
	};

class CGBlendCopy : public TBlendImpl<CGBlendCopy>
	{
	public:
		inline static CG32bitPixel Blend (CG32bitPixel rgbDest, CG32bitPixel rgbSource) { return rgbSource; }
		inline static CG32bitPixel BlendPreMult (CG32bitPixel rgbDest, CG32bitPixel rgbSource) { return rgbSource; }
		inline static CG32bitPixel Copy (CG32bitPixel rgbDest, CG32bitPixel rgbSource) { return rgbSource; }
	};

class CGBlendHardLight : public TBlendImpl<CGBlendHardLight>
	{
	public:
		inline static CG32bitPixel Blend (CG32bitPixel rgbDest, CG32bitPixel rgbSource) 
			{
			BYTE *pAlpha = CG32bitPixel::AlphaTable(rgbSource.GetAlpha());

			BYTE redResult = HardLight(rgbDest.GetRed(), rgbSource.GetRed());
			BYTE greenResult = HardLight(rgbDest.GetGreen(), rgbSource.GetGreen());
			BYTE blueResult = HardLight(rgbDest.GetBlue(), rgbSource.GetBlue());

			if (rgbSource.GetGreen() == 0xff)
				return CG32bitPixel(redResult, greenResult, blueResult);
			else
				return CG32bitPixel::Blend(rgbDest, CG32bitPixel(redResult, greenResult, blueResult), rgbSource.GetAlpha());
			}

		inline static CG32bitPixel BlendPreMult (CG32bitPixel rgbDest, CG32bitPixel rgbSource)
			{
			if (rgbSource.GetAlpha() == 0xff)
				return Copy(rgbDest, rgbSource);
			else
				return CG32bitPixel::Blend(rgbDest, Copy(rgbDest, rgbSource), rgbSource.GetAlpha());
			}

		inline static CG32bitPixel Copy (CG32bitPixel rgbDest, CG32bitPixel rgbSource)
			{
			BYTE redResult = HardLight(rgbDest.GetRed(), rgbSource.GetRed());
			BYTE greenResult = HardLight(rgbDest.GetGreen(), rgbSource.GetGreen());
			BYTE blueResult = HardLight(rgbDest.GetBlue(), rgbSource.GetBlue());

			return CG32bitPixel(redResult, greenResult, blueResult);
			}

	private:
		inline static BYTE HardLight (BYTE byDest, BYTE bySource)
			{
			if (bySource < 0x80)
				return (2 * CG32bitPixel::AlphaTable(byDest)[bySource]);
			else
				return 0xff - (2 * CG32bitPixel::AlphaTable(0xff - byDest)[0xff - bySource]);
			}
	};

class CGBlendScreen : public TBlendImpl<CGBlendScreen>
	{
	public:
		inline static CG32bitPixel Blend (CG32bitPixel rgbDest, CG32bitPixel rgbSource) 
			{
			BYTE *pAlpha = CG32bitPixel::AlphaTable(rgbSource.GetAlpha());

			BYTE redResult = 0xff - CG32bitPixel::AlphaTable(0xff - rgbDest.GetRed())[(0xff - pAlpha[rgbSource.GetRed()])];
			BYTE greenResult = 0xff - CG32bitPixel::AlphaTable(0xff - rgbDest.GetGreen())[(0xff - pAlpha[rgbSource.GetGreen()])];
			BYTE blueResult = 0xff - CG32bitPixel::AlphaTable(0xff - rgbDest.GetBlue())[(0xff - pAlpha[rgbSource.GetBlue()])];

			return CG32bitPixel(redResult, greenResult, blueResult);
			}

		inline static CG32bitPixel BlendPreMult (CG32bitPixel rgbDest, CG32bitPixel rgbSource)
			{
			BYTE redResult = 0xff - CG32bitPixel::AlphaTable(0xff - rgbDest.GetRed())[(0xff - rgbSource.GetRed())];
			BYTE greenResult = 0xff - CG32bitPixel::AlphaTable(0xff - rgbDest.GetGreen())[(0xff - rgbSource.GetGreen())];
			BYTE blueResult = 0xff - CG32bitPixel::AlphaTable(0xff - rgbDest.GetBlue())[(0xff - rgbSource.GetBlue())];

			return CG32bitPixel(redResult, greenResult, blueResult);
			}

		inline static CG32bitPixel Copy (CG32bitPixel rgbDest, CG32bitPixel rgbSource) { return CG32bitPixel::Screen(rgbDest, rgbSource); }
	};

//	Implementation Helpers -----------------------------------------------------

#include "TBlt.h"
#include "TCirclePainter.h"

//	Inlines --------------------------------------------------------------------

inline void CG32bitImage::DrawLine (int x1, int y1, int x2, int y2, int iWidth, CG32bitPixel rgbColor) { CGDraw::Line(*this, x1, y1, x2, y2, iWidth, rgbColor); }

