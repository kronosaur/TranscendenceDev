//	Painters.h
//
//	Transcendence UI Engine
//	Copyright (c) 2010 by George Moromisato. All Rights Reserved.

#pragma once

class CGalacticMapPainter
	{
	public:
		CGalacticMapPainter (const CVisualPalette &VI, CSystemMap *pMap);
		~CGalacticMapPainter (void);

		void AdjustCenter (const RECT &rcView, int xCenter, int yCenter, int iScale, int *retxCenter, int *retyCenter);
        void GalacticToView (int x, int y, const RECT &rcView, int xCenter, int yCenter, int iScale, int *retx, int *rety) const;
		inline int GetHeight (void) { return m_cyMap; }
		inline int GetWidth (void) { return m_cxMap; }
		void Paint (CG32bitImage &Dest, const RECT &rcView, int xCenter, int yCenter, int iScale);
        void ViewToGalactic (int x, int y, const RECT &rcView, int xCenter, int yCenter, int iScale, int *retx, int *rety) const;

	private:
		void DrawNode (CG32bitImage &Dest, CTopologyNode *pNode, int x, int y, CG32bitPixel rgbColor);
		void Init (void);
		inline SPoint Xform (const SPoint &pt) const { return SPoint(m_xViewCenter + m_iScale * (pt.x - m_xCenter) / 100, m_yViewCenter + m_iScale * (m_yCenter - pt.y) / 100); }

		const CVisualPalette &m_VI;

		CSystemMap *m_pMap;
		int m_cxMap;
		int m_cyMap;

		CG32bitImage *m_pImage;
		bool m_bFreeImage;

		//	Temporaries, while painting

		int m_iScale;
		int m_xCenter;
		int m_yCenter;
		int m_xViewCenter;
		int m_yViewCenter;
	};

class CMapLegendPainter
    {
    public:
        struct SScaleEntry
            {
            Metric rUnits;                  //  Length of scale in units
            char *pszUnitLabel;             //  Name of the unit
            Metric rUnitLength;             //  Length of a unit in logical units
            };

        CMapLegendPainter (const CVisualPalette &VI, SScaleEntry *pScaleDesc = NULL, int iCount = 0);

        inline int GetHeight (void) const { Realize(); return RectHeight(m_rcRect); }
        inline int GetWidth (void) const { return m_cxWidth; }
        void Paint (CG32bitImage &Dest, int x, int y) const;
        inline void SetDesc (const CString &sDesc) { m_sDesc = sDesc; m_bRealized = false; }
        inline void SetScale (int cxScale, const CString &sLabel) { m_cxScale = cxScale; m_sScaleLabel = sLabel; }
        void SetScale (Metric rLogicalUnitsPerPixel);
        inline void SetTitle (const CString &sTitle) { m_sTitle = sTitle; m_bRealized = false; }
        inline void SetWidth (int cxWidth) { m_cxWidth = cxWidth; m_bRealized = false; }

    private:
        void Realize (void) const;

        const CVisualPalette &m_VI;
        SScaleEntry *m_pScaleDesc;
        int m_iScaleCount;
        int m_cxWidth;

        CString m_sTitle;
        CString m_sDesc;
        int m_cxScale;
        CString m_sScaleLabel;

        mutable bool m_bRealized;
        mutable TArray<CString> m_Title;
        mutable TArray<CString> m_Desc;
        mutable RECT m_rcRect;
    };

class CStargateEffectPainter
	{
	public:
		CStargateEffectPainter (void);

		void Paint (CG32bitImage &Dest, const RECT &rcRect);
		void Update (void);

	private:
		struct SWormholePos
			{
			Metric rAngle;
			Metric rRadius;
			Metric rDepth;
			};

		struct STracer
			{
			STracer (void) : Points(0)
				{ }

			TQueue<SWormholePos> Points;
			SWormholePos Vel;
			CG32bitPixel Color;
			};

		struct SPaintCtx
			{
			CG32bitImage *pDest;
			Metric xCenter;
			Metric yCenter;
			Metric cxHalfWidth;
			Metric cyHalfHeight;
			};

		void InitGradientColorTable (int iRadius);
		void InitTracer (STracer &Tracer);
		void PaintTracer (SPaintCtx &Ctx, const STracer &Tracer);
		void TransformPos(SPaintCtx &Ctx, const SWormholePos &Pos, int *retx, int *rety);
		void UpdateTracers (void);

		int m_iInitialUpdates;				//	Number of ticks to update before first paint
		int m_iMaxTracerCount;				//	Max number of tracers
		int m_iMaxTracerLength;				//	Max number of segments per tracer
		int m_iNewTracerCount;				//	Number of new tracers per tick
		BYTE m_byTracerOpacity;				//	Max opacity of tracers
		Metric m_rMaxDepth;					//	Max depth of tracers
		Metric m_rRotationVel;				//	Effect rotation in radians per tick
		Metric m_rGlowAccel;				//	Glow acceleration (pixels per tick)

		TArray<CG32bitPixel> m_GradientColorTable;
		TArray<STracer> m_Tracers;
		Metric m_rRotation;
		Metric m_rGlowVel;
		Metric m_rGlowRadius;
	};