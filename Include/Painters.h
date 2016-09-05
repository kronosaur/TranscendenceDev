//	Painters.h
//
//	Transcendence UI Engine
//	Copyright (c) 2010 by George Moromisato. All Rights Reserved.

#pragma once

class CHoverDescriptionPainter
	{
	public:
		CHoverDescriptionPainter (const CVisualPalette &VI);

		inline void Hide (void) { m_cxWidth = 0; }
		inline bool IsVisible (void) const { return (m_cxWidth > 0); }
		void Paint (CG32bitImage &Dest) const;
		inline void SetBackColor (const CG32bitPixel rgbValue) { m_rgbBack = rgbValue; }
		void SetDescription (const CString &sValue);
		inline void SetDescriptionColor (const CG32bitPixel rgbValue) { m_rgbDescription = rgbValue; }
		inline void SetTitle (const CString &sValue) { m_sTitle = sValue; Invalidate(); }
		inline void SetTitleColor (const CG32bitPixel rgbValue) { m_rgbTitle = rgbValue; }
		void Show (int x, int y, int cxWidth, const RECT &rcContainer);

	private:
		inline bool IsInvalid (void) const { return (m_rcRect.left == 0 && m_rcRect.right == 0); }
		inline void Invalidate (void) { m_rcRect = { 0, 0, 0, 0 };	}
		void Format (void) const;

		const CVisualPalette &m_VI;

		CString m_sTitle;					//	Title to paint
		CString m_sDescription;				//	Description
		CG32bitPixel m_rgbBack;				//	Background color
		CG32bitPixel m_rgbTitle;			//	Title color
		CG32bitPixel m_rgbDescription;		//	Description color

		int m_xPos;							//	Draw the description at this position, but
		int m_yPos;							//		adjust to fit in the container
		int m_cxWidth;						//	Desired width of description box (0 = hidden)
		RECT m_rcContainer;					//	Position description inside this container

		//	Initialized in Format

		mutable RECT m_rcRect;				//	Rect of entire background
		mutable RECT m_rcText;				//	Rect of text area
		mutable CTextBlock m_DescriptionRTF;	//	Rich text to draw
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