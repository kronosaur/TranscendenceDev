//	DXImage16.h
//
//	Implements 16-bit Images
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CG16bitPixel
	{
	public:
		static inline DWORD GetBlue8bit (WORD wPixel) { return GetBlue5bit(wPixel) << 3; }
		static inline DWORD GetGreen8bit (WORD wPixel) { return GetGreen6bit(wPixel) << 2; }
		static inline DWORD GetRed8bit (WORD wPixel) { return GetRed5bit(wPixel) << 3; }
		static inline CG32bitPixel PixelToRGB (WORD wPixel) { return CG32bitPixel((BYTE)GetRed8bit(wPixel), (BYTE)GetGreen8bit(wPixel), (BYTE)GetBlue8bit(wPixel)); }
		static inline WORD RGBToPixel (DWORD dwRed8bit, DWORD dwGreen8bit, DWORD dwBlue8bit) { return (WORD)((dwBlue8bit >> 3) | ((dwGreen8bit >> 2) << 5) | ((dwRed8bit >> 3) << 11)); }
		static inline WORD RGBToPixel (CG32bitPixel rgbColor) { return RGBToPixel(rgbColor.GetRed(), rgbColor.GetGreen(), rgbColor.GetBlue()); }

	private:
		static inline WORD GetBlue5bit (WORD wPixel) { return (wPixel & 0x1f); }
		static inline WORD GetGreen6bit (WORD wPixel) { return (wPixel & 0x7e0) >> 5; }
		static inline WORD GetRed5bit (WORD wPixel) { return (wPixel & 0xf800) >> 11; }

		WORD m_wPixel;
	};

class CG16bitSprite
	{
	public:
		CG16bitSprite (void);
		~CG16bitSprite (void);

		ALERROR CreateFromImage (const CG16bitImage &Source);
		void ColorTransBlt (CG16bitImage &Dest, int xDest, int yDest, int xSrc, int ySrc, int cxWidth, int cyHeight);
		inline int GetHeight (void) const { return m_cyHeight; }
		inline int GetWidth (void) const { return m_cxWidth; }

	private:
		enum SpriteCodes
			{
			codeSkip =	0x0001,
			codeRun =	0x0002,
			};

		void DeleteData (void);

		int m_cxWidth;
		int m_cyHeight;
		WORD *m_pCode;
		WORD **m_pLineIndex;
	};

class CG16bitImage : public CGImagePlane
	{
	public:
		enum Flags
			{
			cfbPreMultAlpha =		0x00000001,
			cfbDesaturateAlpha =	0x00000002,
			};

		struct SDrawLineCtx
			{
			CG16bitImage *pDest;

			int x1;					//	Start
			int y1;
			int x2;					//	End
			int y2;

			int x;					//	Current
			int y;

			int dx;					//	delta x
			int ax;					//	2 * abs(dx)
			int sx;					//	sign(dx)

			int dy;					//	delta y
			int ay;					//	2 * abs(dy)
			int sy;					//	sign(dy)

			int d;					//	leftover

			inline bool IsXDominant(void) { return (ax > ay); }
			};

		typedef void (*DRAWLINEPROC)(SDrawLineCtx *pCtx);

		CG16bitImage (void);
		CG16bitImage (const CG16bitImage &Src);
		virtual ~CG16bitImage (void);

		CG16bitImage &operator= (const CG16bitImage &Src);

		int AdjustTextX (const CG16bitFont &Font, const CString &sText, AlignmentStyles iAlign, int x);
		void AssociateSurface (LPDIRECTDRAW7 pDD);
		void ConvertToSprite (void);
		ALERROR CreateBlank (int cxWidth, int cyHeight, bool bAlphaMask, WORD wInitColor = 0, BYTE byInitAlpha = 0xff);
		ALERROR CreateBlankAlpha (int cxWidth, int cyHeight);
		ALERROR CreateBlankAlpha (int cxWidth, int cyHeight, BYTE byOpacity);
		ALERROR CreateFromBitmap (HBITMAP hBitmap, HBITMAP hBitmask = NULL, DWORD dwFlags = 0);
		ALERROR CreateFromFile (const CString &sImageFilespec, const CString &sMaskFilespec = NULL_STR, DWORD dwFlags = 0);
		ALERROR CreateFromImage (const CG16bitImage &Image);
		ALERROR CreateFromImageTransformed (const CG16bitImage &Source,
											int xSrc, 
											int ySrc, 
											int cxSrc, 
											int cySrc,
											Metric rScaleX, 
											Metric rScaleY, 
											Metric rRotation);
		inline void Destroy (void) { DeleteData(); }
		void DiscardSurface (void);
		void SetBlending (WORD wAlpha);
		void SetTransparentColor (WORD wColor = DEFAULT_TRANSPARENT_COLOR);

		void Blt (int xSrc, int ySrc, int cxWidth, int cyHeight, const CG16bitImage &Source, int xDest, int yDest);
		void BltGray (int xSrc, int ySrc, int cxWidth, int cyHeight, int iAlpha, const CG16bitImage &Source, int xDest, int yDest);
		void BltLighten (int xSrc, int ySrc, int cxWidth, int cyHeight, int iAlpha, const CG16bitImage &Source, int xDest, int yDest);
		void BltToDC (HDC hDC, int x, int y);
		void BltToSurface (LPDIRECTDRAWSURFACE7 pSurface, SurfaceTypes iType);
		void BltToSurface (LPDIRECTDRAWSURFACE7 pSurface, SurfaceTypes iType, const RECT &rcDest);
		void BltWithMask (int xSrc, int ySrc, int cxWidth, int cyHeight, const CG16bitImage &Mask, const CG16bitImage &Source, int xDest, int yDest);
		void ColorTransBlt (int xSrc, int ySrc, int cxWidth, int cyHeight, DWORD dwOpacity, const CG16bitImage &Source, int xDest, int yDest);
		void CompositeTransBlt (int xSrc, int ySrc, int cxWidth, int cyHeight, DWORD dwOpacity, const CG16bitImage &Source, int xDest, int yDest);
		void CopyAlpha (int xSrc, int ySrc, int cxWidth, int cyHeight, const CG16bitImage &Soruce, int xDest, int yDest);
		ALERROR CopyToClipboard (void);
		void ClearMaskBlt (int xSrc, int ySrc, int cxWidth, int cyHeight, const CG16bitImage &Source, int xDest, int yDest, WORD wColor = DEFAULT_TRANSPARENT_COLOR);
		void DrawDot (int x, int y, WORD wColor, MarkerTypes iMarker);
		inline void DrawLine (int x1, int y1, int x2, int y2, int iWidth, WORD wColor) { BresenhamLineAA(x1, y1, x2, y2, iWidth, wColor); }
		inline void DrawLineTrans (int x1, int y1, int x2, int y2, int iWidth, WORD wColor, DWORD dwOpacity) { BresenhamLineAATrans(x1, y1, x2, y2, iWidth, wColor, dwOpacity); }
		void DrawLineProc (SDrawLineCtx *pCtx, DRAWLINEPROC pfProc);
		void DrawLineProcInit (int x1, int y1, int x2, int y2, SDrawLineCtx *pCtx);
		inline void DrawBiColorLine (int x1, int y1, int x2, int y2, int iWidth, WORD wColor1, WORD wColor2) { BresenhamLineAAFade(x1, y1, x2, y2, iWidth, wColor1, wColor2); }
		inline void DrawPixel (int x, int y, WORD wColor) { if (x >= m_rcClip.left && y >= m_rcClip.top && x < m_rcClip.right && y < m_rcClip.bottom) *GetPixel(GetRowStart(y), x) = wColor; }
		inline void DrawPixelTrans (int x, int y, WORD wColor, BYTE byTrans)
				{
				if (x >= m_rcClip.left && y >= m_rcClip.top && x < m_rcClip.right && y < m_rcClip.bottom) 
					{
					WORD *pPos = GetPixel(GetRowStart(y), x);
					*pPos = BlendPixel(*pPos, wColor, byTrans);
					}
				}
		static inline void DrawPixelTrans (WORD *pPos, WORD wColor, DWORD byTrans)
				{
				*pPos = BlendPixel(*pPos, wColor, byTrans);
				}
		inline void DrawPlainLine (int x1, int y1, int x2, int y2, WORD wColor) { BresenhamLine(x1, y1, x2, y2, wColor); }
		inline void DrawText (int x, int y, const CG16bitFont &Font, WORD wColor, CString sText, DWORD dwFlags = 0, int *retx = NULL);
		void Fill (int x, int y, int cxWidth, int cyHeight, WORD wColor);
		void FillAlpha (int x, int y, int cxWidth, int cyHeight, DWORD byOpacity);
		void FillAlphaMask (int xSrc, int ySrc, int cxWidth, int cyHeight, const CG16bitImage &Source, DWORD byOpacity, int xDest, int yDest);
		void FillRGB (int x, int y, int cxWidth, int cyHeight, COLORREF rgbValue);
		void FillColumn (int x, int y, int cyHeight, WORD wColor);
		void FillColumnTrans (int x, int y, int cyHeight, WORD wColor, DWORD byOpacity);
		void FillLine (int x, int y, int cxWidth, WORD wColor);
		void FillLineGray (int x, int y, int cxWidth, WORD wColor, DWORD byOpacity);
		void FillLineTrans (int x, int y, int cxWidth, WORD wColor, DWORD byOpacity);
		void FillMask (int xSrc, int ySrc, int cxWidth, int cyHeight, const CG16bitImage &Source, WORD wColor, int xDest, int yDest, BYTE byOpacity = 0xff);
		void FillTrans (int x, int y, int cxWidth, int cyHeight, WORD wColor, DWORD byOpacity);
		void FillTransGray (int x, int y, int cxWidth, int cyHeight, WORD wColor, DWORD byOpacity);
		void FillTransRGB (int x, int y, int cxWidth, int cyHeight, COLORREF rgbValue, int iAlpha);
		inline BYTE *GetAlphaRow (int iRow) const { return (BYTE *)(m_pAlpha + (iRow * m_iAlphaRowSize)); }
		inline BYTE *GetAlphaValue (int x, int y) const { return ((BYTE *)(m_pAlpha + y * m_iAlphaRowSize)) + x; }
		inline WORD GetBackColor (void) const { return m_wBackColor; }
		inline WORD *GetPixel (WORD *pRowStart, int x) const { return pRowStart + x; }
		WORD GetPixelAlpha (int x, int y);
		inline WORD *GetRowStart (int y) const { return (WORD *)(m_pRGB + y * m_iRGBRowSize); }
		inline LPDIRECTDRAWSURFACE7 GetSurface (void) const { return m_pSurface; }
		static SurfaceTypes GetSurfaceType (LPDIRECTDRAWSURFACE7 pSurface);
		inline bool HasAlpha (void) const { return (m_pAlpha != NULL); }
		inline bool HasMask (void) const { return m_bHasMask; }
		inline bool HasRGB (void) const { return (m_pRGB != NULL); }
		void IntersectMask (int xMask, int yMask, int cxMask, int cyMask, const CG16bitImage &Mask, int xDest, int yDest);
		inline bool IsEmpty (void) const { return (m_cxWidth == 0 || m_cyHeight == 0); }
		inline bool IsSprite (void) const { return m_pSprite != NULL; }
		inline bool IsTransparent (void) const { return m_pRedAlphaTable != NULL; }
		void MaskedBlt (int xSrc, int ySrc, int cxWidth, int cyHeight, CG16bitImage &Source, int xDest, int yDest);
		inline BYTE *NextAlphaRow (BYTE *pAlpha) const { return (BYTE *)(((DWORD *)pAlpha) + m_iAlphaRowSize); }
		inline WORD *NextRow (WORD *pRow) const { return pRow + (m_iRGBRowSize * 2); }
		ALERROR ReadFromStream (IReadStream *pStream);
		ALERROR SaveAsWindowsBMP (const CString &sFilespec);
		void SetPixelTrans (int x, int y, WORD wColor, DWORD byOpacity);
		void SwapBuffers (CG16bitImage &Other);
		void TransBlt (int xSrc, int ySrc, int cxWidth, int cyHeight, int iAlpha, CG16bitImage &Source, int xDest, int yDest);
		void WriteToStream (IWriteStream *pStream);
		void WriteToWindowsBMP (IWriteStream *pStream);

		static inline BYTE BlendAlpha (BYTE dwOpacity1, BYTE dwOpacity2) { return (BYTE)255 - (BYTE)(((DWORD)(255 - dwOpacity1) * (DWORD)(255 - dwOpacity2)) / 255); }
		static WORD BlendPixel (WORD pxDest, WORD pxSource, DWORD byOpacity);
		static WORD BlendPixelGray (WORD pxDest, WORD pxSource, DWORD byOpacity);
		static WORD BlendPixelPM (DWORD pxDest, DWORD pxSource, DWORD byOpacity);
		static WORD FadeColor (WORD wStart, WORD wEnd, int iFade);
		static inline WORD DarkenPixel (DWORD pxSource, DWORD byOpacity) { return BlendPixel(RGBValue(0,0,0), (WORD)pxSource, byOpacity); }
		static inline WORD DesaturateValue (WORD wColor) { return ((GreenValue(wColor) * 59) + (RedValue(wColor) * 30) + (BlueValue(wColor) * 11)) / 255; }
		static inline WORD LightenPixel (DWORD pxSource, DWORD byOpacity) { return BlendPixel(RGBValue(255,255,255), (WORD)pxSource, byOpacity); }
		static inline bool IsGrayscaleValue (WORD wColor) { return ((BlueValue(wColor) == GreenValue(wColor)) && (GreenValue(wColor) == RedValue(wColor))); }
		static inline WORD GrayscaleValue (WORD wValue) { return ((wValue << 8) & 0xf800) | ((wValue << 3) & 0x7c0) | (wValue >> 3); }
		static inline WORD RGBValue (WORD wRed, WORD wGreen, WORD wBlue) { return ((wRed << 8) & 0xf800) | ((wGreen << 3) & 0x7e0) | (wBlue >> 3); }
		static inline WORD BlueValue (WORD wColor) { return GetBlueValue(wColor) << 3; }
		static inline WORD GreenValue (WORD wColor) { return GetGreenValue(wColor) << 2; }
		static inline WORD RedValue (WORD wColor) { return GetRedValue(wColor) << 3; }
		static inline DWORD PixelFromRGB (COLORREF rgb) { return (GetBValue(rgb) >> 3) | ((GetGValue(rgb) >> 2) << 5) | ((GetRValue(rgb) >> 3) << 11); }
		static inline COLORREF RGBFromPixel (WORD wColor) { return RGB(RedValue(wColor), GreenValue(wColor), BlueValue(wColor)); }

	private:
		struct RealPixel
			{
			Metric rRed;
			Metric rGreen;
			Metric rBlue;
			};

		void BresenhamLine (int x1, int y1, 
						    int x2, int y2,
						    WORD wColor);
		void BresenhamLineAA (int x1, int y1, 
							  int x2, int y2,
							  int iWidth,
							  WORD wColor);
		void BresenhamLineAAFade (int x1, int y1, 
								  int x2, int y2,
								  int iWidth,
								  WORD wColor1,
								  WORD wColor2);
		void BresenhamLineAATrans (int x1, int y1, 
							  int x2, int y2,
							  int iWidth,
							  WORD wColor,
							  DWORD dwOpacity);
		void WuLine (int x0, int y0, 
				     int x1, int y1,
				     int iWidth,
					 WORD wColor1);

		void CopyData (const CG16bitImage &Src);
		void DeleteData (void);
		inline DWORD DoublePixelFromRGB (COLORREF rgb) { return PixelFromRGB(rgb) | (PixelFromRGB(rgb) << 16); }
		inline DWORD *GetPixelDW (DWORD *pRowStart, int x, bool *retbOdd) const { *retbOdd = ((x % 2) == 1); return pRowStart + (x / 2); }
		RealPixel GetRealPixel (const RECT &rcRange, Metric rX, Metric rY, bool *retbBlack = NULL);
		static inline WORD GetBlueValue (WORD wPixel) { return (wPixel & 0x1f); }
		static inline WORD GetGreenValue (WORD wPixel) { return (wPixel & 0x7e0) >> 5; }
		static inline WORD GetRedValue (WORD wPixel) { return (wPixel & 0xf800) >> 11; }
		inline DWORD *GetRowStartDW (int y) const { return m_pRGB + y * m_iRGBRowSize; }
		inline DWORD HighPixelFromRGB (COLORREF rgb) const { return PixelFromRGB(rgb) << 16; }
		inline bool InClipX (int x) const { return (x >= m_rcClip.left && x < m_rcClip.right); }
		inline bool InClipY (int y) const { return (y >= m_rcClip.top && y < m_rcClip.bottom); }
		void InitBMI (BITMAPINFO **retpbi);
		inline DWORD LowPixelFromRGB (COLORREF rgb) const { return PixelFromRGB(rgb); }
		inline WORD MakePixel (WORD wRed, WORD wGreen, WORD wBlue) { return (wRed << 11) | (wGreen << 5) | (wBlue); }
		inline void SetLowPixel (DWORD *pPos, DWORD dwValue) { *pPos = ((*pPos) & 0xFFFF0000) | dwValue; }
		inline void SetHighPixel (DWORD *pPos, DWORD dwValue) { *pPos = ((*pPos) & 0xFFFF) | dwValue; }
		void SetRealPixel (Metric rX, Metric rY, const RealPixel &Value, bool bNotBlack = false);

		int m_iRGBRowSize;			//	Number of DWORDs in an image row
		int m_iAlphaRowSize;		//	Number of DWORDs in an alpha mask row

		//	The image is structured as an array of 16-bit pixels. Each row
		//	in the image is aligned on a DWORD boundary. The image starts with
		//	the upper left corner. Each pixel is a 5-6-5 RGB pixel.
		DWORD *m_pRGB;

		//	The alpha mask is an array of 8-bit mask values. Each row is
		//	aligned on a DWORD boundary. A value of 0 masks out the image
		//	at that location. A value from 1 to 255 blends the image with
		//	the background.
		DWORD *m_pAlpha;

		WORD m_wBackColor;			//	Back color is transparent
		bool m_bHasMask;			//	TRUE if image has m_pAlpha (or used to have m_pAlpha, but was optimized)

		//	We calculate these transparency tables when using blending
		WORD *m_pRedAlphaTable;
		WORD *m_pGreenAlphaTable;
		WORD *m_pBlueAlphaTable;

		//	Sprite (for optimized blting)
		CG16bitSprite *m_pSprite;

		//	DirectDraw
		LPDIRECTDRAWSURFACE7 m_pSurface;
		BITMAPINFO *m_pBMI;			//	Used for blting to a DC
	};

bool CalcBltTransform (Metric rX,
					   Metric rY,
					   Metric rScaleX,
					   Metric rScaleY,
					   Metric rRotation,
					   int xSrc,
					   int ySrc,
					   int cxSrc,
					   int cySrc,
					   CXForm *retSrcToDest, 
					   CXForm *retDestToSrc, 
					   RECT *retrcDest);
void CompositeFilledCircle (CG16bitImage &Dest, int xDest, int yDest, int iRadius, WORD *pColorTable, BYTE *pOpacityTable, bool bStochastic = false);
void CopyBltColorize (CG16bitImage &Dest,
					  int xDest,
					  int yDest,
					  int cxDest,
					  int cyDest,
					  const CG16bitImage &Src,
					  int xSrc,
					  int ySrc,
					  DWORD dwHue,
					  DWORD dwSaturation);
void CopyBltTransformed (CG16bitImage &Dest, 
						 const RECT &rcDest,
						 const CG16bitImage &Src, 
						 const RECT &rcSrc,
						 const CXForm &SrcToDest,
						 const CXForm &DestToSrc,
						 const RECT &rcDestXForm);
void CreateRoundedRectAlpha (int cxWidth, int cyHeight, int iRadius, CG16bitImage *retImage);
void DrawAlphaGradientCircle (CG16bitImage &Dest, 
		int xDest, 
		int yDest, 
		int iRadius,
		WORD wColor);
void DrawArc (CG16bitImage &Dest, int xCenter, int yCenter, int iRadius, int iStartAngle, int iEndAngle, int iLineWidth, WORD wColor);
void DrawBltCircle (CG16bitImage &Dest, 
		int xDest, 
		int yDest, 
		int iRadius,
		const CG16bitImage &Src,
		int xSrc,
		int ySrc,
		int cxSrc,
		int cySrc,
		DWORD byOpacity);
void DrawBltRotated (CG16bitImage &Dest,
					 int xDest,
					 int yDest,
					 int iRotation,
					 const CG16bitImage &Src,
					 int xSrc,
					 int ySrc,
					 int cxSrc,
					 int cySrc);
void DrawBltScaledFast (CG16bitImage &Dest,
						int xDest,
						int yDest,
						int cxDest,
						int cyDest,
						const CG16bitImage &Src,
						int xSrc,
						int ySrc,
						int cxSrc,
						int cySrc);
void DrawBltShimmer (CG16bitImage &Dest,
					 int xDest,
					 int yDest,
					 int cxDest,
					 int cyDest,
					 const CG16bitImage &Src,
					 int xSrc,
					 int ySrc,
					 DWORD byIntensity,
					 DWORD dwSeed = 0);
void DrawBltTransformed (CG16bitImage &Dest, 
						 Metric rX, 
						 Metric rY, 
						 Metric rScaleX, 
						 Metric rScaleY, 
						 Metric rRotation, 
						 const CG16bitImage &Src, 
						 int xSrc, 
						 int ySrc, 
						 int cxSrc, 
						 int cySrc);
void DrawBltTransformedGray (CG16bitImage &Dest, 
						 Metric rX, 
						 Metric rY, 
						 Metric rScaleX, 
						 Metric rScaleY, 
						 Metric rRotation, 
						 const CG16bitImage &Src, 
						 int xSrc, 
						 int ySrc, 
						 int cxSrc, 
						 int cySrc);
void DrawBrokenLine (CG16bitImage &Dest, int xSrc, int ySrc, int xDest, int yDest, int xyBreak, WORD wColor);
void DrawDottedLine (CG16bitImage &Dest, int x1, int y1, int x2, int y2, WORD wColor);
void DrawFilledCircle (CG16bitImage &Dest, int xDest, int yDest, int iRadius, WORD wColor);
void DrawFilledCircle (CG16bitImage &Dest, int xDest, int yDest, int iRadius, WORD *pColorTable, BYTE *pOpacityTable);
void DrawFilledCircleGray (CG16bitImage &Dest, int xDest, int yDest, int iRadius, WORD wColor, DWORD byOpacity);
void DrawFilledCircleTrans (CG16bitImage &Dest, int xDest, int yDest, int iRadius, WORD wColor, DWORD byOpacity);
void DrawGlowRing (CG16bitImage &Dest,
				   int xDest,
				   int yDest,
				   int iRadius,
				   int iRingThickness,
				   WORD wColor,
				   DWORD dwOpacity = 255);
void DrawGlowRing (CG16bitImage &Dest,
				   int xDest,
				   int yDest,
				   int iRadius,
				   int iRingThickness,
				   WORD *wColorRamp,
				   DWORD *byOpacityRamp);
void DrawGradientRectHorz (CG16bitImage &Dest,
		int xDest,
		int yDest,
		int cxDest,
		int cyDest,
		WORD wStartColor,
		WORD wEndColor,
		DWORD dwStartOpacity,
		DWORD dwEndOpacity);
void DrawOctaRectOutline (CG16bitImage &Dest, int x, int y, int cxWidth, int cyHeight, int iCorner, int iLineWidth, WORD wColor);
void DrawOctaRectOutlineAlpha (CG16bitImage &Dest, int x, int y, int cxWidth, int cyHeight, int iCorner, int iLineWidth, BYTE byAlpha);
void DrawRectDotted (CG16bitImage &Dest, int x, int y, int cxWidth, int cyHeight, WORD wColor);
void DrawRoundedRect (CG16bitImage &Dest, int x, int y, int cxWidth, int cyHeight, int iRadius, WORD wColor);
void DrawRoundedRectOutline (CG16bitImage &Dest, int x, int y, int cxWidth, int cyHeight, int iRadius, int iLineWidth, WORD wColor);
void DrawRoundedRectTrans (CG16bitImage &Dest, int x, int y, int cxWidth, int cyHeight, int iRadius, WORD wColor, DWORD byOpacity);

enum EffectTypes
	{
	effectShadow =			0x01,
	};

struct SEffectDesc
	{
	EffectTypes iType;
	WORD wColor1;
	WORD wColor2;
	};

class CG16bitFont : public CObject
	{
	public:
		enum Flags
			{
			SmartQuotes =	0x00000001,
			TruncateLine =	0x00000002,

			AlignCenter =	0x00000004,
			AlignRight =	0x00000008,

			MeasureOnly =	0x00000010,
			AdjustToFit =	0x00000020,

            AlignMiddle =   0x00000040,
			};

		CG16bitFont (void);
		CG16bitFont (const CG16bitFont &Src);

		CG16bitFont &operator= (const CG16bitFont &Src);

		ALERROR Create (const CString &sTypeface, int iSize, bool bBold = false, bool bItalic = false, bool bUnderline = false);
		ALERROR CreateFromFont (HFONT hFont);
		ALERROR CreateFromResource (HINSTANCE hInst, char *pszRes);
		inline void Destroy (void) { m_FontImage.Destroy(); m_Metrics.RemoveAll(); }

		int BreakText (const CString &sText, int cxWidth, TArray<CString> *retLines, DWORD dwFlags = 0) const;
		inline void DrawText (CG16bitImage &Dest, int x, int y, WORD wColor, const CString &sText, DWORD dwFlags = 0, int *retx = NULL) const
			{ DrawText(Dest, x, y, wColor, 255, sText, dwFlags, retx); }
		inline void DrawText (CG16bitImage &Dest, const RECT &rcRect, WORD wColor, const CString &sText, int iLineAdj = 0, DWORD dwFlags = 0, int *retcyHeight = NULL) const
			{ DrawText(Dest, rcRect, wColor, 255, sText, iLineAdj, dwFlags, retcyHeight); }

		void DrawText (CG16bitImage &Dest, 
					   int x, 
					   int y, 
					   WORD wColor, 
					   DWORD byOpacity,
					   const CString &sText, 
					   DWORD dwFlags = 0, 
					   int *retx = NULL) const;
		void DrawText (CG16bitImage &Dest, 
					   const RECT &rcRect, 
					   WORD wColor, 
					   DWORD byOpacity,
					   const CString &sText, 
					   int iLineAdj = 0, 
					   DWORD dwFlags = 0,
					   int *retcyHeight = NULL) const;

		void DrawText (CG32bitImage &Dest, 
					   int x, 
					   int y, 
					   CG32bitPixel rgbColor, 
					   const CString &sText, 
					   DWORD dwFlags = 0, 
					   int *retx = NULL) const;
		void DrawText (CG32bitImage &Dest, 
					   const RECT &rcRect, 
					   CG32bitPixel rgbColor,
					   const CString &sText, 
					   int iLineAdj = 0, 
					   DWORD dwFlags = 0,
					   int *retcyHeight = NULL) const;
        void DrawText (CG32bitImage &Dest, int x, int y, CG32bitPixel rgbColor, const TArray<CString> &Lines, int iLineAdj = 0, DWORD dwFlags = 0, int *rety = NULL) const;

		void DrawTextEffect (CG16bitImage &Dest,
							 int x,
							 int y,
							 WORD wColor,
							 const CString &sText,
							 int iEffectsCount,
							 const SEffectDesc *pEffects,
							 DWORD dwFlags = 0,
							 int *retx = NULL) const;
		void DrawTextEffect (CG32bitImage &Dest,
							 int x,
							 int y,
							 CG32bitPixel rgbColor,
							 const CString &sText,
							 int iEffectsCount,
							 const SEffectDesc *pEffects,
							 DWORD dwFlags = 0,
							 int *retx = NULL) const;
		inline int GetAscent (void) const { return m_cyAscent; }
		inline int GetAverageWidth (void) const { return m_cxAveWidth; }
		const CG16bitImage &GetCharacterImage (char chChar = ' ', int *retx = 0, int *rety = 0, int *retcxWidth = 0, int *retcyHeight = 0, int *retcxAdvance = 0) const;
		inline int GetHeight (void) const { return m_cyHeight; }
		inline const CString &GetTypeface (void) const { return m_sTypeface; }
		inline bool IsBold (void) const { return m_bBold; }
		inline bool IsEmpty (void) const { return m_FontImage.IsEmpty(); }
		inline bool IsItalic (void) const { return m_bItalic; }
		inline bool IsUnderline (void) const { return m_bUnderline; }
		int MeasureText (const CString &sText, int *retcyHeight = NULL, bool bAlwaysAdvance = false) const;
		static bool ParseFontDesc (const CString &sDesc, CString *retsTypeface, int *retiSize, bool *retbBold, bool *retbItalic);
		ALERROR ReadFromStream (IReadStream *pStream);
		void WriteToStream (IWriteStream *pStream);

	private:
		struct CharMetrics
			{
			int cxWidth;
			int cxAdvance;
			};

		CG16bitImage m_FontImage;

		int m_cyHeight;				//	Height of the font
		int m_cyAscent;				//	Height above baseline
		int m_cxAveWidth;			//	Average width of font
		CStructArray m_Metrics;		//	Metrics for each character

		CString m_sTypeface;		//	Name of font selected
		bool m_bBold;				//	Bold
		bool m_bItalic;				//	Italic
		bool m_bUnderline;			//	Underline
	};

class CG16bitRegion
	{
	public:
		CG16bitRegion (void);
		~CG16bitRegion (void) { CleanUp(); }

		void Blt (CG32bitImage &Dest, int xDest, int yDest, CG32bitImage &Src, int xSrc, int ySrc, int cxWidth, int cyHeight);
		void CleanUp (void);
		void ColorTransBlt (CG16bitImage &Dest, int xDest, int yDest, CG16bitImage &Src, int xSrc, int ySrc, int cxWidth, int cyHeight);
		bool CreateFromMask (const CG8bitImage &Source, int xSrc, int ySrc, int cxWidth, int cyHeight);
		bool CreateFromMask (const CG16bitImage &Source, int xSrc, int ySrc, int cxWidth, int cyHeight);
		bool CreateFromMask (const CG32bitImage &Source, int xSrc, int ySrc, int cxWidth, int cyHeight);

	private:
		enum ECodes
			{
			codeMask =			0xC000,
			countMask =			0x3FFF,

			code00 =			0x0000,
			codeRun =			0x4000,
			codeEndOfLine =		0x8000,
			codeFF =			0xC000,
			};

		void AllocCode (DWORD dwNewAlloc);
		void ColorTransBltAlpha (CG16bitImage &Dest, int xDest, int yDest, CG16bitImage &Src, int xSrc, int ySrc, int cxWidth, int cyHeight, int xMask, int yMask);
		void ColorTransBltBackColor (CG16bitImage &Dest, int xDest, int yDest, CG16bitImage &Src, int xSrc, int ySrc, int cxWidth, int cyHeight, int xMask, int yMask);
		void ColorTransBltPlain (CG16bitImage &Dest, int xDest, int yDest, CG16bitImage &Src, int xSrc, int ySrc, int cxWidth, int cyHeight, int xMask, int yMask);
		void WriteCode (WORD *&pCode, DWORD &dwCodeSize, DWORD dwCode, DWORD dwCount);
		void WriteCodeRun (WORD *&pCode, DWORD &dwCodeSize, BYTE *pRun, DWORD dwCount);

		int m_cxWidth;
		int m_cyHeight;
		DWORD m_dwAlloc;
		WORD *m_pCode;
		TArray<DWORD> m_LineIndex;
	};

class CG16bitBinaryRegion
	{
	public:
		struct SRun
			{
			int xStart;
			int xEnd;
			int y;
			};

		CG16bitBinaryRegion (void);
		~CG16bitBinaryRegion (void);

		void CreateFromPolygon (int iVertexCount, SPoint *pVertexList);
		void CreateFromConvexPolygon (int iVertexCount, SPoint *pVertexList);
		void CreateFromSimplePolygon (int iVertexCount, SPoint *pVertexList);
		void Fill (CG32bitImage &Dest, int x, int y, CG32bitPixel rgbColor) const;
		void Fill (CG16bitImage &Dest, int x, int y, WORD wColor) const;
		void FillTrans (CG16bitImage &Dest, int x, int y, WORD wColor, DWORD byOpacity) const;
		void GetBounds (RECT *retrcRect) const;
		inline int GetCount (void) const { return m_iCount; }
		inline const SRun &GetRun (int iIndex) const { return m_pList[iIndex]; }

	private:
		inline SRun &Run (int iIndex) { return m_pList[iIndex]; }
		void ScanEdge (int X1,
					   int Y1,
					   int X2,
					   int Y2,
					   bool bSetX,
					   int iSkipFirst,
					   SRun **iopRun);

		int m_iCount;
		SRun *m_pList;
	};

bool IsConvexPolygon (int iVertexCount, SPoint *pVertexList);
bool IsSimplePolygon (int iVertexCount, SPoint *pVertexList);
int CreateScanLinesFromPolygon (int iVertexCount, SPoint *pVertexList, CG16bitBinaryRegion::SRun **retpLines);
int CreateScanLinesFromSimplePolygon (int iVertexCount, SPoint *pVertexList, CG16bitBinaryRegion::SRun **retpLines);

inline void CG16bitImage::DrawText (int x, int y, const CG16bitFont &Font, WORD wColor, CString sText, DWORD dwFlags, int *retx)
	{
	Font.DrawText(*this, x, y, wColor, sText, dwFlags, retx);
	}

inline void CG32bitImage::DrawText (int x, int y, const CG16bitFont &Font, CG32bitPixel rgbColor, CString sText, DWORD dwFlags, int *retx)
	{
	Font.DrawText(*this, x, y, rgbColor, sText, dwFlags, retx);
	}

class CG16bitLinePainter
	{
	public:
		struct SPixelDesc
			{
			WORD *pPos;						//	Pixel to draw on
			DWORD dwOpacity;				//	Opacity
			int x;							//	x-coordinate on image
			int y;							//	y-coordinate on image

			double rV;						//	Distance from start to end along line (0 to 1.0)
			double rW;						//	Distance perpendicular to line axis (0 to 1.0)
			};

		void PaintSolid (CG16bitImage &Image, int x1, int y1, int x2, int y2, int iWidth, WORD wColor);
		void Rasterize (CG16bitImage &Image, int x1, int y1, int x2, int y2, int iWidth, TArray<SPixelDesc> *retPixels);

	private:
		enum ESlopeTypes
			{
			lineNull,						//	No line
			linePoint,						//	Single point
			lineXDominant,
			lineYDominant,
			};

		ESlopeTypes CalcIntermediates (const CG16bitImage &Image, int x1, int y1, int x2, int y2, int iWidth);
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

