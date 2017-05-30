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
		static void PaintArrow (CG32bitImage &Dest, int x, int y, int iDirection, CG32bitPixel rgbColor);
		static void PaintArrowText (CG32bitImage &Dest, int x, int y, int iDirection, const CString &sText, const CG16bitFont &Font, CG32bitPixel rgbColor);
		static void PaintStatusBar (CG32bitImage &Dest, int x, int y, int iTick, CG32bitPixel rgbColor, const CString &sLabel, int iPos, int iMaxPos = 100, int *retcyHeight = NULL);
		static void PaintTargetHighlight (CG32bitImage &Dest, int x, int y, int iTick, int iRadius, int iRingSpacing, int iDelay, CG32bitPixel rgbColor);
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
