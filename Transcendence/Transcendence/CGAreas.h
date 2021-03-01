//	CGAreas.h
//
//	CGAreas
//	Copyright (c) 2014 by Kronosaur Productions, LLC. All Rights Reserved.

#define ITEM_LIST_AREA_PAGE_UP_ACTION			(0xffff0001)
#define ITEM_LIST_AREA_PAGE_DOWN_ACTION			(0xffff0002)

class CDetailList
	{
	public:
		static constexpr DWORD FORMAT_ALIGN_TOP =			0x00000000;	//	Defaults to this
		static constexpr DWORD FORMAT_ALIGN_BOTTOM =		0x00000001;	//	Details aligned to bottom of area
		static constexpr DWORD FORMAT_ALIGN_CENTER =		0x00000002;	//	Details aligned to center of area
		static constexpr DWORD FORMAT_PLACE_TOP =			0x00000000;	//	Defaults to this
		static constexpr DWORD FORMAT_PLACE_BOTTOM =		0x00000004;	//	Area placed at bottom of rect
		static constexpr DWORD FORMAT_PLACE_CENTER =		0x00000008;	//	Area placed at center of rect
		static constexpr DWORD FORMAT_ANTI_MIRROR_COLUMNS =	0x00000010;	//	Detail icons are outside
		static constexpr DWORD FORMAT_MIRROR_COLUMNS =		0x00000020;	//	Detail icons are on center line
		static constexpr DWORD FORMAT_SINGLE_COLUMN =		0x00000040;	//	Single column

		CDetailList (const CVisualPalette &VI) : m_VI(VI)
			{ }

		void CleanUp (void) { m_List.DeleteAll(); }
		void Format (int cxWidth, int cyHeight = 0, DWORD dwFlags = 0, int *retcyHeight = NULL);
		void Load (ICCItem *pDetails);
		void Paint (CG32bitImage &Dest, int x, int y) const;
		void SetColor (CG32bitPixel rgbColor) { m_rgbTextColor = rgbColor; }
		
	private:
		struct SDetailEntry
			{
			CString sTitle;
			CG32bitImage *pIcon = NULL;
			RECT rcIconSrc = { 0 };
			CRTFText Desc;

			bool bAlignRight = false;
			RECT rcRect = { 0 };
			int cyText = 0;
			int cyRect = 0;

			RECT rcIcon = { 0 };					//	Where to paint icon
			RECT rcTitle = { 0 };					//	Where to paint title
			RECT rcDesc = { 0 };					//	Where to paint description
			};

		static constexpr int SPACING_X = 8;
		static constexpr int SPACING_Y = 8;
		static constexpr int DETAIL_ICON_HEIGHT = 48;
		static constexpr int DETAIL_ICON_WIDTH = 48;

		static constexpr DWORD FORMAT_LEFT_COLUMN =			0x00000000;
		static constexpr DWORD FORMAT_RIGHT_COLUMN =		0x10000000;

		void CalcColumnRects (int cxWidth, int cyHeight, int cxCol, int cyCol1, int cyCol2, DWORD dwFlags, RECT &retCol1, RECT &retCol2) const;
		void CalcColumnRect (const RECT &rcArea, int xCol, int cxCol, int cyCol, DWORD dwFlags, RECT &retCol) const;
		void FormatColumn (int iStart, int iEnd, const RECT &rcRect, DWORD dwFlags);
		void FormatDoubleColumns (int cxWidth, int cyHeight, DWORD dwFlags = 0, int *retcyHeight = NULL);
		void FormatSingleColumn (int cxWidth, int cyHeight, DWORD dwFlags = 0, int *retcyHeight = NULL);

		static void AlignRect (const RECT &rcFrame, int cxWidth, int cyHeight, DWORD dwAlign, RECT &rcResult);

		const CVisualPalette &m_VI;
		CG32bitPixel m_rgbTextColor = CG32bitPixel(255, 255, 255);
		TArray<SDetailEntry> m_List;
	};

class CDetailArea
	{
	public:
		enum EStyles
			{
			styleDefault,
			styleStacked,
			styleFull,
			styleStats,
			};

		CDetailArea (CUniverse &Universe, const CVisualPalette &VI, const CDockScreenVisuals &Theme) :
				m_Universe(Universe),
				m_VI(VI),
				m_Theme(Theme),
				m_Details(VI)
			{ }

		AlignmentStyles GetAlignment (void) const { return GetAlignment(m_pData); }
		ICCItem *GetData (void) const { return m_pData; }
		bool IsEmpty (void) const { return (!m_pData || m_pData->IsNil()); }
		EStyles GetStyle (void) const { return GetStyle(m_pData); }
		void Paint (CG32bitImage &Dest, const RECT &rcRect) const;
		void SetBackColor (CG32bitPixel rgbColor) { m_rgbBack = rgbColor; }
		void SetColor (CG32bitPixel rgbColor) { m_rgbText = rgbColor; }
		void SetData (ICCItem *pData) { m_pData = pData; m_bFormatted = false; }
		void SetTabRegion (int cyHeight) { m_cyTabRegion = cyHeight; m_bFormatted = false; }

	private:
		static constexpr int BORDER_RADIUS = 4;
		static constexpr int DEFAULT_LARGE_ICON_HEIGHT = 320;
		static constexpr int DEFAULT_LARGE_ICON_WIDTH = 320;
		static constexpr int SPACING_X = 8;
		static constexpr int SPACING_Y = 8;
		static constexpr int BACKGROUND_IMAGE_MARGIN_Y = 24;

		static constexpr CG32bitPixel RGB_BADGE = CG32bitPixel(255, 255, 255, 25);

		bool Format (const RECT &rcRect) const;
		static AlignmentStyles GetAlignment (ICCItem *pData);
		static EStyles GetStyle (ICCItem *pData);
		void PaintBackground (CG32bitImage &Dest, const RECT &rcRect, CG32bitPixel rgbColor) const;
		void PaintBackgroundImage (CG32bitImage &Dest, const RECT &rcRect, ICCItem *pImageDesc, int cyExtraMargin = 0) const;
		void PaintBadgeImage (CG32bitImage &Dest, const RECT &rcDest) const;
		void PaintScaledImage (CG32bitImage &Dest, const RECT &rcDest, const ICCItem &ImageDesc) const;
		void PaintStackedImage (CG32bitImage &Dest, int x, int y, ICCItem *pImageDesc, Metric rScale = 1.0) const;

		CUniverse &m_Universe;
		const CVisualPalette &m_VI;
		const CDockScreenVisuals &m_Theme;

		ICCItemPtr m_pData;

		CG32bitPixel m_rgbText = CG32bitPixel(255, 255, 255);
		CG32bitPixel m_rgbBack = CG32bitPixel(0, 0, 0);
		int m_cxLargeIcon = DEFAULT_LARGE_ICON_WIDTH;
		int m_cyLargeIcon = DEFAULT_LARGE_ICON_HEIGHT;
		int m_cyTabRegion = 0;

		mutable bool m_bFormatted = false;

		//	These are derived from m_pData

		mutable EStyles m_iStyle = styleDefault;
		mutable CString m_sTitle;
		mutable CDetailList m_Details;
		mutable AlignmentStyles m_iAlignment = alignTop;

		//	These are computed in format

		mutable RECT m_rcFrame = { 0 };
		mutable RECT m_rcDetails = { 0 };
		mutable RECT m_rcIcon = { 0 };
		mutable RECT m_rcTitle = { 0 };
	};

class CGCarouselArea : public AGArea
	{
	public:
		enum EStyles
			{
			styleNone,

			styleShipCompare,				//	Compare two ship classes
			styleShipList,					//	Optimized for list of ship to purchase
			};

		CGCarouselArea (const CVisualPalette &VI, const CDockScreenVisuals &Theme);

		void CleanUp (void);
		int GetCursor (void) const { return (m_pListData ? m_pListData->GetCursor() : -1); }
		ICCItem *GetEntryAtCursor (void);
		IListData *GetList (void) const { return m_pListData; }
		CSpaceObject *GetSource (void) { return (m_pListData ? m_pListData->GetSource() : NULL); }
		bool IsCursorValid (void) const { return (m_pListData ? m_pListData->IsCursorValid() : false); }
		bool MoveCursorBack (void);
		bool MoveCursorForward (void);
		void ResetCursor (void) { if (m_pListData) m_pListData->ResetCursor(); Invalidate(); }
		void SetBackColor (CG32bitPixel rgbColor) { m_rgbBackColor = rgbColor; }
		void SetColor (CG32bitPixel rgbColor) { m_rgbTextColor = rgbColor; }
		void SetCursor (int iIndex) { if (m_pListData) m_pListData->SetCursor(iIndex); Invalidate(); }
		void SetList (ICCItem *pList);
		bool SetStyle (const CString &sStyle);
		void SyncCursor (void) { if (m_pListData) m_pListData->SyncCursor(); Invalidate(); }

		//	AGArea virtuals

		virtual bool LButtonDown (int x, int y) override;
		virtual void MouseLeave (void) override;
		virtual void MouseMove (int x, int y) override;
		virtual void MouseWheel (int iDelta, int x, int y, DWORD dwFlags) override;
		virtual void Paint (CG32bitImage &Dest, const RECT &rcRect) override;
		virtual void Update (void) override;

	private:
		static const int BORDER_RADIUS = 4;
		static const int DEFAULT_ICON_HEIGHT = 64;
		static const int DEFAULT_ICON_WIDTH = 64;
		static const int SPACING_X = 8;
		static const int SPACING_Y = 8;
		static const int SELECTION_WIDTH = 2;
		static const int SELECTOR_HEIGHT = DEFAULT_ICON_HEIGHT + 40;
		static const int SELECTOR_WIDTH = 140;
		static const int SELECTOR_AREA_HEIGHT = SELECTOR_HEIGHT + SPACING_Y;
		static const int MOUSE_SCROLL_SENSITIVITY =	30;
		static const int SELECTOR_PADDING_TOP =	2;
		static const int SELECTOR_PADDING_LEFT = 4;
		static const int SELECTOR_PADDING_RIGHT = 4;
		static const int LARGE_ICON_HEIGHT = 320;
		static const int LARGE_ICON_WIDTH = 320;
		static const int DETAIL_ICON_HEIGHT = 48;
		static const int DETAIL_ICON_WIDTH = 48;

		int FindSelector (int x) const;
		void PaintContent (CG32bitImage &Dest, const RECT &rcRect) const;
		void PaintList (CG32bitImage &Dest, const RECT &rcRect);
		void PaintListShipCompare (CG32bitImage &Dest, const RECT &rcRect);
		void PaintSelector (CG32bitImage &Dest, const RECT &rcRect, bool bSelected) const;

		TUniquePtr<IListData> m_pListData;
		EStyles m_iStyle = styleNone;
		int m_iOldCursor = -1;							//	Cursor pos

		const CVisualPalette &m_VI;
		const CDockScreenVisuals &m_Theme;
		CG32bitPixel m_rgbTextColor = CG32bitPixel(255, 255, 255);
		CG32bitPixel m_rgbBackColor = CG32bitPixel(0, 0, 0);
		CG32bitPixel m_rgbDisabledText = CG32bitPixel(128,128,128);
		int m_xOffset = 0;								//	Painting offset for smooth scroll
		int m_xFirst = 0;								//	coord of first row relative to list rect
		int m_cxSelector = SELECTOR_WIDTH;				//	Width of selector cell
		int m_cySelector = SELECTOR_HEIGHT;				//	Height of selector cell
		int m_cySelectorArea = SELECTOR_AREA_HEIGHT;	//	Height of selector area
		int m_cxIcon = DEFAULT_ICON_WIDTH;				//	Icon width
		int m_cyIcon = DEFAULT_ICON_HEIGHT;				//	Icon height
		int m_cxLargeIcon = LARGE_ICON_WIDTH;			//	Large content icon
		int m_cyLargeIcon = LARGE_ICON_HEIGHT;			//	Large content icon
		Metric m_rIconScale = 1.0;						//	Icon scale
	};

class CGDetailsArea : public AGArea
	{
	public:
		CGDetailsArea (const CVisualPalette &VI, const CDockScreenVisuals &Theme);

		void CleanUp (void) { m_Painter.SetData(NULL); }
		ICCItem *GetData (void) const { return m_Painter.GetData(); }
		const CDetailArea &GetDetail (void) const { return m_Painter; }
		void SetBackColor (CG32bitPixel rgbColor) { m_Painter.SetBackColor(rgbColor); }
		void SetColor (CG32bitPixel rgbColor) { m_Painter.SetColor(rgbColor); }
		void SetData (ICCItem *pList) { m_Painter.SetData(pList); }
		void SetTabRegion (int cyHeight) { m_Painter.SetTabRegion(cyHeight); Invalidate(); }

		//	AGArea virtuals

		virtual void Paint (CG32bitImage &Dest, const RECT &rcRect) override;

	private:
		const CVisualPalette &m_VI;
		const CDockScreenVisuals &m_Theme;
		CDetailArea m_Painter;
	};

class CGDrawArea : public AGArea
	{
	public:
		CGDrawArea (void);

		CG32bitImage &GetCanvas (void) { CreateImage(); return m_Image; }

		//	AGArea virtuals
		virtual void Paint (CG32bitImage &Dest, const RECT &rcRect) override;

	private:
		void CreateImage (void);

		CG32bitImage m_Image;

		CG32bitPixel m_rgbBackColor;
		bool m_bTransBackground;
	};

class CGIconListArea : public AGArea
	{
	public:
		static constexpr DWORD NOTIFY_SELECTION_CHANGED =		1;

		CGIconListArea (CUniverse &Universe, const CVisualPalette &VI, const CDockScreenVisuals &Theme) :
				m_Universe(Universe),
				m_VI(VI),
				m_Theme(Theme)
			{
			InitFromTheme(VI, Theme);
			}

		int GetCount () const { return m_List.GetCount(); }
		ICCItemPtr GetEntry (int iIndex) const { if (iIndex < 0 || iIndex >= m_List.GetCount()) throw CException(ERR_FAIL); return m_List[iIndex].pData; }
		ICCItemPtr GetList () const;
		TArray<int> GetSelection () const;
		ICCItemPtr GetSelectionAsCCItem () const;
		void RestoreSelection (const ICCItem &Selection);
		ICCItemPtr SaveSelection () const;
		bool SetData (const ICCItem &List, CString *retsError = NULL);
		void SetTabRegion (int cyHeight) { m_cyTabRegion = cyHeight; }

		//	AGArea virtuals

		virtual bool LButtonDown (int x, int y) override;
		virtual void Paint (CG32bitImage &Dest, const RECT &rcRect) override;

	private:
		static constexpr int DEFAULT_ICON_SIZE = 64;
		static constexpr int FRAME_PADDING_HORZ = 20;
		static constexpr int FRAME_PADDING_VERT = 20;
		static constexpr int ENTRY_SPACING = 10;
		static constexpr int ENTRY_PADDING_TOP = 2;
		static constexpr int ENTRY_PADDING_LEFT = 4;
		static constexpr int ENTRY_PADDING_RIGHT = 4;

		struct SEntry
			{
			ICCItemPtr pData;

			bool bSelected = false;

			CString sTitle;						//	Entry label
			const CG32bitImage *pIcon = NULL;	//	Icon for entry
			RECT rcIconSrc = { 0 };				//	Source RECT for entry
			int iStatusBar = -1;				//	Status bar position (0-100) or -1 = not used.
			CG32bitPixel rgbStatusBar;			//	Status bar color.
			CString sStatusBarLabel;			//	Label for status bar.
			bool bMinor = false;				//	Draw faded

			//	Computed in Format

			mutable RECT rcRect = { 0 };		//	Location, relative to control rect
			};

		int CalcIconSize (int iEntryCount) const;
		bool DeselectAll ();
		bool Format (const RECT &rcRect) const;
		int HitTestEntry (int x, int y) const;
		bool InitEntry (SEntry &Entry, const ICCItem &Data, CString *retsError = NULL);
		void InitFromTheme (const CVisualPalette &VI, const CDockScreenVisuals &Theme);
		void PaintEntry (CG32bitImage &Dest, const SEntry &Entry) const;
		void PaintEntryIcon (CG32bitImage &Dest, const SEntry &Entry, int x, int y) const;
		void Reformat () { m_bFormatted = false; Invalidate(); }

		CUniverse &m_Universe;
		const CVisualPalette &m_VI;
		const CDockScreenVisuals &m_Theme;
		TArray<SEntry> m_List;

		int m_cxIcon = DEFAULT_ICON_SIZE;
		int m_cyIcon = DEFAULT_ICON_SIZE;
		int m_cyTabRegion = 0;
		CG32bitPixel m_rgbText = CG32bitPixel(0, 0, 0);
		CG32bitPixel m_rgbBack = CG32bitPixel(255, 255, 255);
		int m_iBorderRadius = 0;

		//	These are computed in Format

		mutable bool m_bFormatted = false;
		mutable RECT m_rcFrame = { 0 };
	};

class CGItemDisplayArea : public AGArea
	{
	public:
		CGItemDisplayArea (void);

		void SetBackColor (CG32bitPixel rgbColor) { m_rgbBackColor = rgbColor; Invalidate(); }
		void SetColor (CG32bitPixel rgbColor) { m_rgbTextColor = rgbColor; Invalidate(); }
		void SetItem (CSpaceObject *pSource, const CItem &Item) { m_pSource = pSource; m_Item = Item; Invalidate(); }
		void SetText (const CString &sTitle, const CString &sDesc) { m_sTitle = sTitle; m_sDesc = sDesc; Invalidate(); }

		//	AGArea virtuals
		virtual int Justify (const RECT &rcRect) override;
		virtual void Paint (CG32bitImage &Dest, const RECT &rcRect) override;

	private:
		CSpaceObject *m_pSource;
		CItem m_Item;

		CString m_sTitle;					//	If no item
		CString m_sDesc;					//	If no item

		CG32bitPixel m_rgbTextColor;
		CG32bitPixel m_rgbBackColor;

		CItemPainter m_ItemPainter;			//	Used to paint item
	};

class CGItemListDisplayArea : public AGArea
	{
	public:
		CGItemListDisplayArea (void);

		bool InitFromDesc (ICCItem *pDesc);
		void SetBackColor (CG32bitPixel rgbColor) { m_rgbBackColor = rgbColor; }
		void SetColor (CG32bitPixel rgbColor) { m_rgbTextColor = rgbColor; }
		void SetDisplayAsKnown (bool bValue = true) { m_bActualItems = bValue; Invalidate(); }
		void SetItemList (CSpaceObject *pSource, const CItemList &ItemList);
		void SetText (const CString &sTitle, const CString &sDesc) { m_sTitle = sTitle; m_sDesc = sDesc; m_ItemList.DeleteAll(); Invalidate(); }

		//	AGArea virtuals
		virtual int Justify (const RECT &rcRect) override;
		virtual void Paint (CG32bitImage &Dest, const RECT &rcRect) override;

	private:
		struct SEntry
			{
			SEntry (void) :
					bGrayed(false)
				{ }

			CItem Item;
			bool bGrayed;
			};

		CUniverse &GetUniverse (void) const { return *g_pUniverse; }
		bool InitFromItemList (ICCItem *pItemList);
		void SortItemList (void);

		CSpaceObject *m_pSource;
		TArray<SEntry> m_ItemList;

		CString m_sTitle;					//	If no items
		CString m_sDesc;					//	If no items

		CG32bitPixel m_rgbTextColor;
		CG32bitPixel m_rgbBackColor;

		bool m_bActualItems = false;			//	Show actual items, even if unknown

		//	Initialized on Justify

		int m_cxBox;
		int m_cyBox;
		int m_iCols;
		int m_iRows;
		int m_xOffset;
		int m_xLastRowOffset;
	};

class CGItemListArea : public AGArea
	{
	public:
		CGItemListArea (const CVisualPalette &VI);
		~CGItemListArea (void);

		void AddTab (DWORD dwID, const CString &sLabel);
		void CleanUp (void);
		void EnableTab (DWORD dwID, bool bEnabled = true);
		void DeleteAtCursor (int iCount) { if (m_pListData) m_pListData->DeleteAtCursor(iCount); InitRowDesc(); Invalidate(); }
		int GetCursor (void) const { return (m_pListData ? m_pListData->GetCursor() : -1); }
		ICCItem *GetEntryAtCursor (void);
		const CItem &GetItem (int iRow) const;
		const CItem &GetItemAtCursor (void) { return (m_pListData ? m_pListData->GetItemAtCursor() : g_DummyItem); }
		CItemListManipulator &GetItemListManipulator (void) { return (m_pListData ? m_pListData->GetItemListManipulator() : g_DummyItemListManipulator); }
		IListData *GetList (void) const { return m_pListData; }
		bool GetNextTab (DWORD *retdwID) const;
		bool GetPrevTab (DWORD *retdwID) const;
		int GetRowCount (void) const { return m_Rows.GetCount(); }
		CSpaceObject *GetSource (void) { return (m_pListData ? m_pListData->GetSource() : NULL); }
		bool IsCursorValid (void) const { return (m_pListData ? m_pListData->IsCursorValid() : false); }
		bool IsDisplayAsKnown (void) const { return m_bActualItems; }
		bool IsEnabled (int iRow) const { return !m_Rows[iRow].bDisabled; }
		bool MoveCursorBack (void);
		bool MoveCursorForward (void);
		void MoveTabToFront (DWORD dwID);
		void ResetCursor (void) { if (m_pListData) m_pListData->ResetCursor(m_EnabledItems); InitRowDesc(); Invalidate(); }
		void SelectTab (DWORD dwID);
		void SetBackColor (CG32bitPixel rgbColor) { m_rgbBackColor = rgbColor; Invalidate(); }
		void SetColor (CG32bitPixel rgbColor) { m_rgbTextColor = rgbColor; Invalidate(); }
		void SetCursor (int iIndex) { if (m_pListData) m_pListData->SetCursor(iIndex); Invalidate(); }
		void SetDisplayAsKnown (bool bValue = true) { m_bActualItems = bValue; InitRowDesc(); Invalidate(); }
		void SetEnabled (int iRow, bool bEnabled = true) { m_Rows[iRow].bDisabled = !bEnabled; Invalidate(); }
		void SetEnabledCriteria (const CItemCriteria &Criteria);
		void SetFilter (const CItemCriteria &Filter) { if (m_pListData) m_pListData->SetFilter(Filter); InitRowDesc(); Invalidate(); }
		void SetFontTable (const SFontTable *pFonts) { m_pFonts = pFonts; }
		void SetIconHeight (int cyHeight) { m_cyIcon = cyHeight; }
		void SetIconScale (Metric rScale) { m_rIconScale = rScale; }
		void SetIconWidth (int cxWidth) { m_cxIcon = cxWidth; }
		void SetList (ICCItem *pList);
		void SetList (CSpaceObject *pSource);
		void SetList (CItemList &ItemList);
		void SetNoArmorSpeedDisplay (bool bValue = true) { m_bNoArmorSpeedDisplay = bValue; }
		void SetRowHeight (int cyHeight) { m_cyRow = Max(1, cyHeight); }
		void SetTabRegion (int cyHeight) { m_cyTabHeight = cyHeight; }
		void SetUIRes (const CUIResources *pUIRes) { m_pUIRes = pUIRes; }
		void SyncCursor (void) { if (m_pListData) m_pListData->SyncCursor(); Invalidate(); }

		//	AGArea virtuals
		virtual bool LButtonDown (int x, int y) override;
		virtual void MouseLeave (void) override;
		virtual void MouseMove (int x, int y) override;
		virtual void MouseWheel (int iDelta, int x, int y, DWORD dwFlags) override;
		virtual void Paint (CG32bitImage &Dest, const RECT &rcRect) override;
		virtual void Update (void) override;

	private:
		enum ListTypes
			{
			listNone,
			listItem,
			listCustom,
			};

		struct SRowDesc
			{
			int yPos = 0;						//	Position of the row (sum of height of previous rows)
			int cyHeight = 0;					//	Height of this row
			bool bDisabled = false;				//	Entry is disabled

			CItemPainter Painter;				//	Painter used for the item, which keeps some metrics.
			CTilePainter CustomPainter;			//	Painter used for custom entries
			};

		struct STabDesc
			{
			STabDesc (void) :
					bDisabled(false)
				{ }

			DWORD dwID;
			CString sLabel;
			int cxWidth;
			bool bDisabled;
			};

		static constexpr int DEFAULT_ROW_HEIGHT =			96;
		static constexpr int ICON_WIDTH =					96;
		static constexpr int ICON_HEIGHT =					96;

		static constexpr CG32bitPixel RGB_SELECTED_DESC =	CG32bitPixel(200,200,200);
		static constexpr CG32bitPixel RGB_NORMAL_DESC =		CG32bitPixel(128,128,128);

		void InitRowHeight (int iRow, SRowDesc &RowDesc);
		void InitRowDesc (void);
		int FindRow (int y);
		bool FindTab (DWORD dwID, int *retiIndex = NULL) const;
		bool HitTestTabs (int x, int y, int *retiTab);
		void PaintCustom (CG32bitImage &Dest, const SRowDesc &RowDesc, const RECT &rcRect, bool bSelected);
		void PaintItem (CG32bitImage &Dest, const SRowDesc &RowDesc, const RECT &rcRect, bool bSelected);
		void PaintTab (CG32bitImage &Dest, const STabDesc &Tab, const RECT &rcRect, bool bSelected, bool bHover);

		IListData *m_pListData = NULL;
		ListTypes m_iType = listNone;
		CItemCriteria m_EnabledItems;

		const CVisualPalette &m_VI;
		const CUIResources *m_pUIRes = NULL;
		const SFontTable *m_pFonts = NULL;
		CG32bitPixel m_rgbTextColor = CG32bitPixel(255, 255, 255);
		CG32bitPixel m_rgbBackColor = CG32bitPixel(0, 0, 0);
		int m_iOldCursor = -1;					//	Cursor pos
		int m_yOffset = 0;						//	Painting offset for smooth scroll
		int m_yFirst = 0;						//	coord of first row relative to list rect
		int m_cyRow = DEFAULT_ROW_HEIGHT;		//	Row height
		int m_cxIcon = ICON_WIDTH;				//	Icon width
		int m_cyIcon = ICON_HEIGHT;				//	Icon height
		Metric m_rIconScale = 1.0;				//	Icon scale

		int m_cyTotalHeight = 0;				//	Total heigh of all rows
		TArray<SRowDesc> m_Rows;

		TArray<STabDesc> m_Tabs;
		int m_iCurTab = -1;						//	Current selected tab (-1 = none)
		int m_iHoverTab = -1;					//	Hover tab (-1 = none)
		int m_cyTabHeight = 0;					//	Height of tab row

		bool m_bNoArmorSpeedDisplay = false;	//	Do not show armor bonus/penalty to speed
		bool m_bActualItems = false;			//	Show actual items, even if unknown
	};

class CGNeurohackArea : public AGArea
	{
	public:
		CGNeurohackArea (void);
		~CGNeurohackArea (void);

		void CleanUp (void);
		void SetData (int iWillpower, int iDamage);
		void SetFontTable (const SFontTable *pFonts) { m_pFonts = pFonts; }

		//	AGArea virtuals
		virtual void Paint (CG32bitImage &Dest, const RECT &rcRect) override;
		virtual void Update (void) override;

	private:
		struct SNode
			{
			int x;
			int y;
			int iWidth;
			int iSphereSize;
			int iRootDist;
			int iDamageLevel;

			SNode *pParent;
			SNode *pNext;
			SNode *pPrev;
			SNode *pFirstChild;
			};

		void CreateBranch (SNode *pParent, int iDirection, int iGeneration, int iWidth, const RECT &rcRect);
		void CreateNetwork (const RECT &rcRect);
		SNode *CreateNode (SNode *pParent, int x, int y);
		void PaintBranch (CG32bitImage &Dest, SNode *pNode, SNode *pNext = NULL);
		void PaintSphere (CG32bitImage &Dest, int x, int y, int iRadius, CG32bitPixel rgbGlowColor);

		const SFontTable *m_pFonts;

		SNode *m_pNetwork;
		int m_iNodeCount;
		int m_iNodeAlloc;

		SNode **m_pRootNodes;
		int m_iRootCount;
		int m_iRootAlloc;

		SNode **m_pTerminalNodes;
		int m_iTerminalCount;
		int m_iTerminalAlloc;

		SNode **m_pActiveNodes;
		int m_iActiveCount;
		int m_iActiveAlloc;

		int m_iWillpower;
		int m_iDamage;
	};

class CGSelectorArea : public AGArea
	{
	public:
		enum EConfigurations
			{
			configNone,

			configArmor,
			configDevices,
			configMiscDevices,
			configWeapons,
			};

		enum EDirections
			{
			moveDown,						//	Move cursor to region below
			moveLeft,						//	Move cursor to region to the left
			moveRight,						//	Move cursor to region to the right
			moveUp,							//	Move cursor to region above

			moveNext,						//	Move to next region
			movePrev,						//	Move to previous region
			};

		struct SOptions
			{
			EConfigurations iConfig = configDevices;
			CItemCriteria ItemCriteria = CItemCriteria(CItemCriteria::ALL);
			bool bNoEmptySlots = false;
			bool bAlwaysShowShields = false;
			};

		CGSelectorArea (const CVisualPalette &VI, const CDockScreenVisuals &Theme);
		~CGSelectorArea (void);

		int GetCursor (void) const { return m_iCursor; }
		ICCItem *GetEntryAtCursor (void);
		const CItem &GetItemAtCursor (void);
		IListData *GetList (void) const { return NULL; }
		CSpaceObject *GetSource (void) { return m_pSource; }
		bool IsCursorValid (void) const { return (m_iCursor != -1); }
		bool MoveCursor (EDirections iDir);
		void Refresh (void);
		void ResetCursor (void) { m_iCursor = -1; Invalidate(); }
		void RestoreSelection (const ICCItem &Selection);
		ICCItemPtr SaveSelection () const { return ICCItemPtr(m_iCursor); }
		void SetBackColor (CG32bitPixel rgbColor) { m_rgbBackColor = rgbColor; }
		void SetColor (CG32bitPixel rgbColor) { m_rgbTextColor = rgbColor; }
		void SetCursor (int iIndex);
		void SetRegions (CSpaceObject *pSource, const SOptions &Options);
		void SetSlotNameAtCursor (const CString &sName);
		void SetTabRegion (int cyHeight) { m_cyTabRegion = cyHeight; }
		void SyncCursor (void) { if (m_iCursor != -1 && m_iCursor >= m_Regions.GetCount()) m_iCursor = m_Regions.GetCount() - 1; }

		//	AGArea virtuals

		virtual bool LButtonDown (int x, int y) override;
		virtual void Paint (CG32bitImage &Dest, const RECT &rcRect) override;
		virtual void Update (void) override;

	private:
		enum ETypes
			{
			typeNone,

			typeEmptySlot,
			typeInstalledItem,
			};

		struct SEntry
			{
			SEntry (void) :
					iType(typeNone),
					pItemCtx(NULL),
					iSlotType(devNone),
					iSlotPosIndex(-1)
				{ }

			~SEntry (void)
				{
				if (pItemCtx)
					delete pItemCtx;
				}

			ETypes iType;					//	Type of entry
			CItemCtx *pItemCtx;				//	Item represented (may be NULL)
			DeviceNames iSlotType;			//	Type of slot (if empty)
			CString sSlotID;				//	ID of slot (may be NULL_STR)
			CString sSlotName;              //  If empty

			int iSlotPosIndex;				//	Position index
			RECT rcRect;					//	Location of region (always relative to the center
											//	of the area).
			};

		void CalcRegionRect (const SEntry &Entry, int xCenter, int yCenter, RECT *retrcRect);
		void CleanUp (void);
		bool FindLayoutForPos (const CVector &vPos, const TArray<bool> &SlotStatus, int *retiIndex = NULL);
		bool FindNearestRegion (int xCur, int yCur, EDirections iDir, bool bDiagOnly, int *retiIndex) const;
		bool FindRegionInDirection (EDirections iDir, int *retiIndex = NULL) const;
		void PaintBackground (CG32bitImage &Dest, const RECT &rcRect) const;
		void PaintEmptySlot (CG32bitImage &Dest, const RECT &rcRect, const SEntry &Entry);
		void PaintInstalledItem (CG32bitImage &Dest, const RECT &rcRect, const SEntry &Entry);
		void PaintModifier (CG32bitImage &Dest, int x, int y, const CString &sText, CG32bitPixel rgbColor, CG32bitPixel rgbBackColor, int *rety);
		void SetRegionsFromArmor (CSpaceObject *pSource);
		void SetRegionsFromDevices (CSpaceObject *pSource);
		void SetRegionsFromMiscDevices (CSpaceObject *pSource);
		void SetRegionsFromWeapons (CSpaceObject *pSource);

		const CVisualPalette &m_VI;
		const CDockScreenVisuals &m_Theme;
		CG32bitPixel m_rgbTextColor = CG32bitPixel(255, 255, 255);
		CG32bitPixel m_rgbBackColor;
		int m_cyTabRegion = 0;

		CSpaceObject *m_pSource = NULL;
		CItemCriteria m_Criteria;
		EConfigurations m_iConfig = configNone;
		bool m_bNoEmptySlots = false;		//  Do not show empty slots
		bool m_bAlwaysShowShields = false;	//  Always show shields in armor selected, even
											//      if not part of criteria

		TArray<SEntry> m_Regions;
		int m_iCursor = -1;
	};

class CGTabArea : public AGArea
	{
	public:
		CGTabArea (const CVisualPalette &VI):
				m_VI(VI)
			{ }

		void AddTab (const CString &sID, const CString &sLabel);
		int GetTabCount (void) const { return m_Tabs.GetCount(); }
		const CString &GetNextTabID (void) const;
		const CString &GetPrevTabID (void) const;
		const CString &GetTabID (int iIndex) const { return m_Tabs[iIndex].sID; }
		void SetBackColor (CG32bitPixel rgbColor) { m_rgbBackColor = rgbColor; Invalidate(); }
		void SetColor (CG32bitPixel rgbColor) { m_rgbTextColor = rgbColor; Invalidate(); }
		void SetCurTab (const CString &sID);
		void SetNoNavigation (bool bValue = true) { m_bNoNavigation = bValue; }

		//	AGArea virtuals
		virtual bool LButtonDown (int x, int y) override;
		virtual void MouseLeave (void) override;
		virtual void MouseMove (int x, int y) override;
		virtual void Paint (CG32bitImage &Dest, const RECT &rcRect) override;

	private:
		static constexpr int BORDER_RADIUS = 4;
		static constexpr int DEFAULT_TAB_HEIGHT = 24;
		static constexpr int TAB_PADDING_X = 16;

		struct STabDesc
			{
			CString sID;
			CString sLabel;
			int cxWidth = 0;
			bool bDisabled = false;
			};

		bool FindTab (const CString &sID, int *retiIndex = NULL) const;
		bool HitTest (int x, int y, int *retiTab) const;
		void PaintTab (CG32bitImage &Dest, const STabDesc &Tab, const RECT &rcRect, bool bSelected, bool bHover) const;

		const CVisualPalette &m_VI;
		TArray<STabDesc> m_Tabs;
		int m_iCurTab = -1;
		int m_iHoverTab = -1;
		bool m_bNoNavigation = false;

		int m_cyTabHeight = DEFAULT_TAB_HEIGHT;
		CG32bitPixel m_rgbTextColor = CG32bitPixel(255, 255, 255);
		CG32bitPixel m_rgbBackColor = CG32bitPixel(0, 0, 0);
	};

