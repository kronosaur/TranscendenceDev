//	TSEPaint.h
//
//	Transcendence Paint Utilities
//	Copyright (c) 2013 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CImageArranger
	{
	public:
		struct SArrangeDesc
			{
			int cxDesiredWidth;
			int cxSpacing;
			int cxExtraMargin;
			CG16bitFont *pHeader;
			};

		struct SCellDesc
			{
			SCellDesc (void) :
					cxWidth(0),
					cyHeight(0),
					bStartNewRow(false)
				{ }

			int cxWidth;
			int cyHeight;
			CString sText;
			bool bStartNewRow;
			};

		CImageArranger (void);

		void AddMargin (int cx, int cy);
		void ArrangeByRow (const SArrangeDesc &Desc, const TArray<SCellDesc> &Cells);
		inline int GetHeight (void) const { return m_cyMaxHeight; }
		inline int GetHeight (int iIndex) const { return m_Map[iIndex].cyHeight; }
		inline int GetTextHeight (int iIndex) const { return m_Map[iIndex].cyText; }
		inline int GetTextWidth (int iIndex) const { return m_Map[iIndex].cxText; }
		inline int GetTextX (int iIndex) const { return m_Map[iIndex].xText; }
		inline int GetTextY (int iIndex) const { return m_Map[iIndex].yText; }
		inline int GetWidth (void) const { return m_cxMaxWidth; }
		inline int GetWidth (int iIndex) const { return m_Map[iIndex].cxWidth; }
		inline int GetX (int iIndex) const { return m_Map[iIndex].x; }
		inline int GetY (int iIndex) const { return m_Map[iIndex].y; }
		void Place (int iIndex, int x, int y, int cxWidth, int cyHeight);
		void PlaceText (int iIndex, int x, int y, int cxWidth, int cyHeight);

	private:
		struct SEntry
			{
			int x;
			int y;
			int cxWidth;
			int cyHeight;

			int xText;
			int yText;
			int cxText;
			int cyText;
			};

		void Init (int iCells);

		TArray<SEntry> m_Map;
		int m_cxMaxWidth;
		int m_cyMaxHeight;
	};

class CPaintHelper
	{
	public:
		static void CalcSmoothColorTable (int iRadius, int iIntensity, CG32bitPixel rgbPrimary, CG32bitPixel rgbSecondary, BYTE byOpacity, TArray<CG32bitPixel> *retColorTable);

		static void PaintArrow (CG32bitImage &Dest, int x, int y, int iDirection, CG32bitPixel rgbColor);
		static void PaintArrowText (CG32bitImage &Dest, int x, int y, int iDirection, const CString &sText, const CG16bitFont &Font, CG32bitPixel rgbColor);
		static void PaintScaledImage (CG32bitImage &Dest, int xDest, int yDest, int cxDest, int cyDest, CG32bitImage &Src, const RECT &rcSrc, Metric rScale = 1.0);
		static void PaintStatusBar (CG32bitImage &Dest, int x, int y, int iTick, CG32bitPixel rgbColor, const CString &sLabel, int iPos, int iMaxPos = 100, int *retcyHeight = NULL);
		static void PaintTargetHighlight (CG32bitImage &Dest, int x, int y, int iTick, int iRadius, int iRingSpacing, int iDelay, CG32bitPixel rgbColor);
	};

class CRTFText
	{
	public:
		struct SAutoRTFOptions
			{
			CG32bitPixel rgbQuoteText = CG32bitPixel(255, 255, 255);

			bool bNoQuoteHighlight = false;
			};

		CRTFText (const CString &sText = NULL_STR, const IFontTable *pFontTable = NULL);

		void Format (const RECT &rcRect) const;
		void GetBounds (const RECT &rcRect, RECT *retrcRect) const;
		void Paint (CG32bitImage &Dest, const RECT &rcRect) const;
		inline void SetAlignment (AlignmentStyles iAlign) { m_iAlign = iAlign; InvalidateFormat(); }
		inline void SetDefaultColor (CG32bitPixel rgbColor) { m_rgbDefaultColor = rgbColor; }
		inline void SetDefaultFont (const CG16bitFont *pFont) { m_pDefaultFont = pFont; }
		inline void SetFontTable (const IFontTable *pFontTable) { m_pFontTable = pFontTable; InvalidateFormat(); }
		void SetText (const CString &sText, const SAutoRTFOptions &Options = SAutoRTFOptions());

		static CString GenerateRTFText (const CString &sText, const SAutoRTFOptions &Options);

	private:
		inline void InvalidateFormat (void) { m_cxFormatted = -1; }

		CString m_sRTF;

		const IFontTable *m_pFontTable = NULL;
		AlignmentStyles m_iAlign = alignLeft;
		CG32bitPixel m_rgbDefaultColor = CG32bitPixel(255, 255, 255);
		const CG16bitFont *m_pDefaultFont = NULL;

		mutable CTextBlock m_RichText;					//	Formatted text
		mutable int m_cxFormatted = -1;
	};

class CVolumetricShadowPainter
	{
	public:
		CVolumetricShadowPainter (CSpaceObject *pStar, int xStar, int yStar, int iShadowAngle, Metric rStarDist, CSpaceObject *pObj, CG8bitSparseImage &DestMask);

		void GetShadowRect (RECT *retrcRect) const;
		void PaintShadow (void) const;

	private:
		CG8bitSparseImage &m_DestMask;
		bool m_bNoShadow;					//	If TRUE, we don't have a shadow

		//	Star metrics

		CSpaceObject *m_pStar;
		int m_xStar;						//	Pixel coordinates of star (absolute, Cartessian)
		int m_yStar;
		int m_iShadowAngle;					//	Direction of object with respect to star
		Metric m_rStarDist;					//	Distance of star to object (kilometers)
		SLightingCtx m_LightingCtx;

		//	Object metrics

		CSpaceObject *m_pObj;
		CVector m_vObjCenter;				//	Center of obj relative to volumetric mask

		//	Shadow metrics

		CVector m_vShadowObjOrigin;
		Metric m_rShadowWidth;
		Metric m_rShadowLength;
		Metric m_rShadowHalfWidth;
		Metric m_rShadowHalfLength;
		CVector m_vShadowOrigin;			//	Origin in mask coordinates
		CVector m_vUL;						//	Upper-left of shadow box (mask coords)
		CVector m_vLR;						//	Lower-right of shadow box (mask coords)
	};

class CMapLabelPainter
	{
	public:
		enum EPositions
			{
			posNone					= 0,	//	Unknown

			posRight				= 1,	//	To the right of the object
			posLeft					= 2,	//	To the left of the object
			posBottom				= 3,	//	Centered below the object
			};

		void CleanUp (void) { m_sLabel = NULL_STR; m_pFont = NULL; }
		const CString &GetLabel (void) const { return m_sLabel; }
		const CG16bitFont &GetFont (void) const { return *m_pFont; }
		EPositions GetPos (void) const { return m_iPos; }
		int GetPosX (void) const { return m_xLabel; }
		int GetPosY (void) const { return m_yLabel; }
		bool IsEmpty (void) const { return m_sLabel.IsBlank(); }
		void MeasureLabel (int *retcxLabel = NULL, int *retcyLabel = NULL) const;
		void ReadFromStream (SLoadCtx &Ctx);
		void RealizePos (void);
		void SetLabel (const CString &sLabel, const CG16bitFont &Font) { m_sLabel = sLabel; m_pFont = &Font; }
		void SetPos (EPositions iPos) { m_iPos = iPos; }
		void SetPos (int x, int y) { m_xLabel = x; m_yLabel = y; }
		void WriteToStream (IWriteStream &Stream) const;

		static void CalcLabelRect (int x, int y, int cxLabel, int cyLabel, EPositions iPos, RECT &retrcRect);

	private:
		static constexpr int LABEL_SPACING_X =	8;
		static constexpr int LABEL_SPACING_Y =	4;
		static constexpr int LABEL_OVERLAP_Y =	1;

		static void CalcLabelPos (int cxLabel, int cyLabel, EPositions iPos, int &xMapLabel, int &yMapLabel);
		static void CalcLabelPos (const CString &sLabel, const CG16bitFont &Font, EPositions iPos, int &xMapLabel, int &yMapLabel);
		static EPositions LoadPosition (DWORD dwLoad) { return (EPositions)dwLoad; }
		static DWORD SavePosition (EPositions iPos) { return (DWORD)iPos; }

		CString m_sLabel;					//	Label for map

		EPositions m_iPos = posNone;		//	Position
		const CG16bitFont *m_pFont = NULL;	//	Font for map label 
		int m_xLabel = 0;					//	Name label in map view
		int m_yLabel = 0;
	};

class CMapLabelArranger
	{
	public:
		static void Arrange (CSystem &System);

	private:
		struct SLabelEntry
			{
			CSpaceObject *pObj = NULL;
			int x = 0;
			int y = 0;
			int cxLabel = 0;
			int cyLabel = 0;

			RECT rcLabel = { 0, 0, 0, 0 };
			CMapLabelPainter::EPositions iPosition = CMapLabelPainter::posNone;
			CMapLabelPainter::EPositions iNewPosition  = CMapLabelPainter::posNone;
			};

		static bool CalcOverlap (SLabelEntry *pEntries, int iCount);
		static void SetLabelPos (SLabelEntry &Entry, CMapLabelPainter::EPositions iPos);
	};

//	Paint Utilities

void ComputeLightningPoints (int iCount, CVector *pPoints, Metric rChaos);
void CreateBlasterShape (int iAngle, int iLength, int iWidth, SPoint *Poly);
void DrawItemTypeIcon (CG32bitImage &Dest, int x, int y, const CItemType *pType, int cxSize = 0, int cySize = 0, bool bGray = false);
void DrawLightning (CG32bitImage &Dest,
					int xFrom, int yFrom,
					int xTo, int yTo,
					CG32bitPixel rgbColor,
					int iPoints,
					Metric rChaos);
void DrawLightning (CG32bitImage &Dest,
					int xFrom, int yFrom,
					int xTo, int yTo,
					CG32bitPixel rgbFrom,
					CG32bitPixel rgbTo,
					Metric rChaos);
void DrawParticle (CG32bitImage &Dest,
				   int x, int y,
				   CG32bitPixel rgbColor,
				   int iSize,
				   DWORD byOpacity);
