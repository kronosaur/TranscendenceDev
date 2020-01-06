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
		inline void SetItem (const CItem &Item) { m_Item = Item; Invalidate(); }
		inline void SetTitle (const CString &sValue) { m_sTitle = sValue; m_Item = CItem(); Invalidate(); }
		inline void SetTitleColor (const CG32bitPixel rgbValue) { m_rgbTitle = rgbValue; }
		void Show (int x, int y, int cxWidth, const RECT &rcContainer);

	private:
		void InitRects (int cxWidth, int cyHeight) const;
		inline void Invalidate (void) { m_rcRect = { 0, 0, 0, 0 };	}
		inline bool IsInvalid (void) const { return (m_rcRect.left == 0 && m_rcRect.right == 0); }
		void FormatText (void) const;
		void PaintItem (CG32bitImage &Dest) const;
		void PaintText (CG32bitImage &Dest) const;

		const CVisualPalette &m_VI;

		CString m_sTitle;					//	Title to paint
		CString m_sDescription;				//	Description
		CItem m_Item;						//	Item to show (may be empty)
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

class CItemPainter
	{
	public:
		static constexpr int ATTRIB_PADDING_X =					4;
		static constexpr int ATTRIB_PADDING_Y =					1;
		static constexpr int ATTRIB_SPACING_X =					2;
		static constexpr int ATTRIB_SPACING_Y =					2;
		static constexpr int DAMAGE_ADJ_ICON_WIDTH =			16;
		static constexpr int DAMAGE_ADJ_ICON_HEIGHT =			16;
		static constexpr int DAMAGE_ADJ_ICON_SPACING_X =		2;
		static constexpr int DAMAGE_ADJ_SPACING_X =				6;
		static constexpr int DAMAGE_TYPE_ICON_WIDTH =			16;
		static constexpr int DAMAGE_TYPE_ICON_HEIGHT =			16;
		static constexpr int DEFAULT_WIDTH =					200;
		static constexpr int ENHANCEMENT_ICON_HEIGHT =			40;
		static constexpr int ENHANCEMENT_ICON_WIDTH =			40;
		static constexpr int ICON_HEIGHT =						96;
		static constexpr int ICON_WIDTH =						96;
		static constexpr int ITEM_DEFAULT_HEIGHT =				96;
		static constexpr int ITEM_LEFT_PADDING =				8;
		static constexpr int ITEM_RIGHT_PADDING =				8;
		static constexpr int ITEM_TEXT_MARGIN_Y =				4;
		static constexpr int ITEM_TEXT_MARGIN_X =				4;
		static constexpr int ITEM_TEXT_MARGIN_BOTTOM =			10;
		static constexpr int ITEM_TITLE_EXTRA_MARGIN =			4;
		static constexpr int LAUNCHER_ICON_HEIGHT =				16;
		static constexpr int LAUNCHER_ICON_WIDTH =				16;
		static constexpr int SMALL_ICON_HEIGHT =				64;
		static constexpr int SMALL_ICON_WIDTH =					64;

		static constexpr DWORD OPTION_DISPLAY_AS_KNOWN =		0x00000001;
		static constexpr DWORD OPTION_NO_ARMOR_SPEED =			0x00000002;
		static constexpr DWORD OPTION_NO_ICON =					0x00000004;
		static constexpr DWORD OPTION_NO_PADDING =				0x00000008;
		static constexpr DWORD OPTION_SMALL_ICON =				0x00000020;
		static constexpr DWORD OPTION_TITLE =					0x00000040;

		static constexpr CG32bitPixel RGB_NORMAL_TEXT =						CG32bitPixel(220,220,220);	//	H:0   S:0   B:86
		static constexpr CG32bitPixel RGB_MODIFIER_NORMAL_BACKGROUND =		CG32bitPixel(101,101,101);	//	H:0   S:0   B:40
		static constexpr CG32bitPixel RGB_MODIFIER_NORMAL_TEXT =			CG32bitPixel(220,220,220);	//	H:0   S:0   B:86

		struct SOptions
			{
			SOptions (void) { }
			SOptions (DWORD dwOptions) :
					bDisplayAsKnown((dwOptions & OPTION_DISPLAY_AS_KNOWN) ? true : false),
					bNoArmorSpeed((dwOptions & OPTION_NO_ARMOR_SPEED) ? true : false),
					bNoIcon((dwOptions & OPTION_NO_ICON) ? true : false),
					bNoPadding((dwOptions & OPTION_NO_PADDING) ? true : false),
					bSmallIcon((dwOptions & OPTION_SMALL_ICON) ? true : false),
					bTitle((dwOptions & OPTION_TITLE) ? true : false)
				{ }

			bool bDisplayAsKnown = false;
			bool bNoArmorSpeed = false;
			bool bNoIcon = false;
			bool bNoPadding = false;
			bool bSmallIcon = false;
			bool bTitle = false;
			};

		CItemPainter (const CVisualPalette &VI = g_pHI->GetVisuals()) :
				m_VI(VI)
			{ }

		int GetHeight (void) const { return m_cyHeight; }
		void Init (const CItem &Item, int cxWidth, const SOptions &Options);

		static constexpr DWORD OPTION_SELECTED =				0x00000010;
		void Paint (CG32bitImage &Dest, int x, int y, CG32bitPixel rgbTextColor = RGB_NORMAL_TEXT, DWORD dwOptions = 0) const;

	private:
		int CalcItemEntryHeight (int cxWidth) const;
		void InitMetrics (int cxWidth);

		static void FormatDisplayAttributes (const CVisualPalette &VI, TArray<SDisplayAttribute> &Attribs, const RECT &rcRect, CCartoucheBlock &retBlock, int *retcyHeight = NULL);
		static void FormatLaunchers (const CVisualPalette &VI, const CMissileItem &MissileItem, const TArray<CItem> &Launchers, const RECT &rcRect, CIconLabelBlock &retLaunchers);
		static void PaintItemEnhancement (const CVisualPalette &VI, CG32bitImage &Dest, CSpaceObject *pSource, const CItem &Item, const CItemEnhancement &Enhancement, const RECT &rcRect, CG32bitPixel rgbText, int *retcyHeight = NULL);
		static void PaintReferenceDamageAdj (const CVisualPalette &VI, CG32bitImage &Dest, int x, int y, int iLevel, int iHP, const int *iDamageAdj, CG32bitPixel rgbText);
		static void PaintReferenceDamageType (const CVisualPalette &VI, CG32bitImage &Dest, int x, int y, int iDamageType, const CString &sRef, CG32bitPixel rgbText);

		const CVisualPalette &m_VI;
		const CItem *m_pItem = NULL;
		SOptions m_Options;

		//	Computed after Init

		CCartoucheBlock m_AttribBlock;		//	Formatted attributes
		CIconLabelBlock m_Launchers;		//	Formatted list of launchers

		int m_cxWidth = 0;
		int m_cyHeight = 0;

		int m_cxIcon = 0;
		int m_cyIcon = 0;
		RECT m_rcDraw = { 0 };				//	Rect where text goes
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

class CAutomataEffectPainter
	{
	public:
		CAutomataEffectPainter (int width, int height);
		int CountLiveNeighbors (int x, int y);
		void InitGradientColorTable (void);
		bool IsAlive(int x, int y);
		void Paint (CG32bitImage &Dest, const RECT &rcRect);
		void Update (void);

	private:
		struct SCell
			{
			SCell(void) : iOpacity(0), bAlive(false) {}
			SCell(bool alive) : iOpacity(0), bAlive(alive) {}
			BYTE iOpacity;				//	A value from 0 to 255. Increases while the cell is alive and decreases while the cell is dead
			bool bAlive;				//	Whether the cell is alive or dead
			bool bAliveNext;			//	Whether the cell will be alive or dead on the next tick. Used only during Update()
			};

		struct SPaintCtx
			{
			CG32bitImage *pDest;
			Metric xCenter;
			Metric yCenter;
			Metric cxHalfWidth;
			Metric cyHalfHeight;
			};

		int m_iTicks;						//	Number of ticks passed
		int m_iInitialUpdates;				//	Number of ticks to update before first paint
		int m_iOpacityInc;					//	The rate at which a cell changes its opacity
		int m_iWidth;						//	Width of the grid
		int m_iHeight;						//	Height of the grid

		TArray<CG32bitPixel> m_GradientColorTable;
		TArray< TArray<SCell> > m_Grid;
	};
