//	DockScreen.h
//
//	Dock Screen classes
//	Copyright (c) 2014 by Kronosaur Productions, LLC. All Rights Reserved.

class CGameSession;
class CDockScreen;
class CDockScreenStack;
class CPlayerShipController;
struct SDockFrame;

class IDockScreenDisplay
	{
	public:
		enum EResults
			{
			resultNone,

			//	HandleKeyDown

			resultHandled,
			resultShowPane,
			};

		enum EBackgroundTypes
			{
			backgroundDefault,				//	Use the default specified by the display

			backgroundNone,					//	No background image
			backgroundImage,				//	Use an image (by UNID)
			backgroundObjHeroImage,			//	Use the object's hero image
			backgroundObjSchematicImage,	//	Use the object's top-down image
			};

		struct SInitCtx
			{
			CPlayerShipController *pPlayer = NULL;
			CDockScreen *pDockScreen = NULL;
			CDesignType *pRoot = NULL;
			CXMLElement *pDesc = NULL;
			CXMLElement *pDisplayDesc = NULL;	//	<Display> element
			AGScreen *pScreen = NULL;
			RECT rcScreen = { 0, 0, 0, 0};		//	Entire screen
			RECT rcRect = { 0, 0, 0, 0 };		//	Standard area
			DWORD dwFirstID = 0;
			const CVisualPalette *pVI = NULL;
			const SFontTable *pFontTable = NULL;

			CSpaceObject *pLocation = NULL;
			ICCItem *pData = NULL;
			};

		struct SBackgroundDesc
			{
			EBackgroundTypes iType = backgroundDefault;		//	Type of image defined
			DWORD dwImageID = 0;				//	UNID to use (if iType == backgroundImage)
			CSpaceObject *pObj = NULL;			//	Object to query (if iType == backgroundObjXXX)
			};

		struct SDisplayOptions
			{
			SBackgroundDesc BackgroundDesc;		//	Background specified by screen

			RECT rcControl = { 0, 0, 0, 0 };	//	Position of main control
			int cyTabRegion = 0;				//	Make room for tabs

			CString sType;						//	Display type
			CXMLElement *pOptions = NULL;		//	Element containing options (<List> or <ListOptions> or <Display>)
												//		May be NULL.
			CString sDataFrom;					//	Source of display data (may be a function)
			CString sItemCriteria;				//	Criteria of items to show in list (may be a function)
			CString sCode;						//	Code to generate list
			CString sInitialItemCode;			//	Code to select initial item in list

			//	Item lists

			bool bNoArmorSpeedDisplay = false;	//	Do not show bonus/penalty to speed from armor
			bool bActualItems = false;			//	Show actual items, even if unknown

            //  Custom lists

			CString sRowHeightCode;				//	For custom lists
			int cxIcon = 96;					//	Icon size
			int cyIcon = 96;					//	Icon size
			Metric rIconScale = 1.0;			//	Icon scale

            //  Selectors

            CString sSlotNameCode;				//  Code to name each slot
            bool bNoEmptySlots = false;			//  If TRUE, don't show empty slots
			};

		IDockScreenDisplay (CDockScreen &DockScreen) :
				m_DockScreen(DockScreen)
			{ }

		virtual ~IDockScreenDisplay (void) { }

		static IDockScreenDisplay *Create (CDockScreen &DockScreen, const CString &sType, CString *retsError = NULL);

		EResults AddListFilter (const CString &sID, const CString &sLabel, const CItemCriteria &Filter) { return OnAddListFilter(sID, sLabel, Filter); }
		void DeleteCurrentItem (int iCount) { OnDeleteCurrentItem(iCount); }
		const CItem &GetCurrentItem (void) const { return OnGetCurrentItem(); }
		ICCItem *GetCurrentListEntry (void) const { return OnGetCurrentListEntry(); }
		bool GetDefaultBackground (SBackgroundDesc *retDesc);
		CItemListManipulator &GetItemListManipulator (void) { return OnGetItemListManipulator(); }
		int GetListCursor (void) { return OnGetListCursor(); }
		IListData *GetListData (void) { return OnGetListData(); }
		ICCItemPtr GetProperty (const CString &sProperty) const;
		CSpaceObject *GetSource (void) const { return OnGetSource(); }

		static constexpr DWORD FLAG_UI_ITEM_LIST =			0x00000001;	//	Shows an item list
		static constexpr DWORD FLAG_UI_ITEM_SELECTOR =		0x00000002;	//	Shows an item selector
		DWORD GetUIFlags (void) const { return OnGetUIFlags(); }

		EResults HandleAction (DWORD dwTag, DWORD dwData) { return OnHandleAction(dwTag, dwData); }
		EResults HandleKeyDown (int iVirtKey) { return OnHandleKeyDown(iVirtKey); }
		ALERROR Init (SInitCtx &Ctx, const SDisplayOptions &Options, CString *retsError);
		bool IsCurrentItemValid (void) const { return OnIsCurrentItemValid(); }
		void OnModifyItemBegin (IDockScreenUI::SModifyItemCtx &Ctx, CSpaceObject *pSource, const CItem &Item);
		EResults OnModifyItemComplete (IDockScreenUI::SModifyItemCtx &Ctx, CSpaceObject *pSource, const CItem &Result);
		EResults OnObjDestroyed (const SDestroyCtx &Ctx) { return OnObjDestroyedNotify(Ctx); }
		EResults ResetList (CSpaceObject *pLocation) { return OnResetList(pLocation); }
		EResults SetListCursor (int iCursor) { return OnSetListCursor(iCursor); }
		EResults SetListFilter (const CItemCriteria &Filter) { return OnSetListFilter(Filter); }
		EResults SetLocation (CSpaceObject *pLocation) { m_pLocation = pLocation; return OnSetLocation(pLocation); }
		bool SetProperty (const CString &sProperty, ICCItem &Value);
		bool SelectItem (const CItem &Item) { return OnSelectItem(Item); }
		bool SelectNextItem (void) { return OnSelectNextItem(); }
		bool SelectPrevItem (void) { return OnSelectPrevItem(); }
		void ShowItem (void) { OnShowItem(); }
		void ShowPane (bool bNoListNavigation) { OnShowPane(bNoListNavigation); }

		static bool GetDisplayOptions (SInitCtx &Ctx, SDisplayOptions *retOptions, CString *retsError);
		static bool ParseBackgrounDesc (ICCItem *pDesc, SBackgroundDesc *retDesc);

	protected:
		virtual EResults OnAddListFilter (const CString &sID, const CString &sLabel, const CItemCriteria &Filter) { return resultNone; }
		virtual void OnDeleteCurrentItem (int iCount) { }
		virtual const CItem &OnGetCurrentItem (void) const { return CItem::NullItem(); }
		virtual ICCItem *OnGetCurrentListEntry (void) const { return NULL; }
		virtual bool OnGetDefaultBackground (SBackgroundDesc *retDesc) { return false; }
		virtual CItemListManipulator &OnGetItemListManipulator (void) { return g_DummyItemListManipulator; }
		virtual int OnGetListCursor (void) { return -1; }
		virtual IListData *OnGetListData (void) { return NULL; }
		virtual ICCItemPtr OnGetProperty (const CString &sProperty) const;
		virtual CSpaceObject *OnGetSource (void) const { return NULL; }
		virtual DWORD OnGetUIFlags (void) const { return 0; }
		virtual EResults OnHandleAction (DWORD dwTag, DWORD dwData) { return resultNone; }
		virtual EResults OnHandleKeyDown (int iVirtKey) { return resultNone; }
		virtual ALERROR OnInit (SInitCtx &Ctx, const SDisplayOptions &Options, CString *retsError) { return NOERROR; }
		virtual bool OnIsCurrentItemValid (void) const { return false; }
		virtual EResults OnObjDestroyedNotify (const SDestroyCtx &Ctx) { return resultNone; }
		virtual EResults OnResetList (CSpaceObject *pLocation) { return resultNone; }
		virtual bool OnSelectItem (const CItem &Item) { return false; }
		virtual bool OnSelectNextItem (void) { return false; }
		virtual bool OnSelectPrevItem (void) { return false; }
		virtual EResults OnSetListCursor (int iCursor) { return resultNone; }
		virtual EResults OnSetListFilter (const CItemCriteria &Filter) { return resultNone; }
		virtual EResults OnSetLocation (CSpaceObject *pLocation) { return resultNone; }
		virtual bool OnSetProperty (const CString &sProperty, ICCItem &Value) { return false; }
		virtual void OnShowItem (void) { }
		virtual void OnShowPane (bool bNoListNavigation);

		//	Helpers

		bool EvalBool (const CString &sCode, bool *retbResult, CString *retsError);
		CSpaceObject *EvalListSource (const CString &sString, CString *retsError);
		bool EvalString (const CString &sString, bool bPlain, ECodeChainEvents iEvent, CString *retsResult);
		CDockScreenStack &GetScreenStack (void) const;
		CUniverse &GetUniverse (void) const { return *g_pUniverse; }
        void SelectArmor (int iSelection);

		CDockScreen &m_DockScreen;
		CSpaceObject *m_pLocation;
		CPlayerShipController *m_pPlayer;
		ICCItem *m_pData;
	};

class CDockScreenActions
	{
	public:
		enum EArrangements
			{
			arrangeVertical,				//	Stack buttons in a single column
			arrangeHorizontal,				//	Arrange horizontally
			};

		CDockScreenActions (void) : m_pData(NULL), m_cxJustify(-1) { }
		~CDockScreenActions (void);

		ALERROR AddAction (const CString &sID, int iPos, const CString &sLabel, CExtension *pExtension, ICCItem *pCode, bool bMinor, int *retiAction);
		int CalcAreaHeight (CDesignType *pRoot, EArrangements iArrangement, const RECT &rcFrame);
		void CleanUp (void);
		void CreateButtons (const CDockScreenVisuals &DockScreenVisuals, CGFrameArea *pFrame, CDesignType *pRoot, DWORD dwFirstTag, EArrangements iArrangement, const RECT &rcFrame);
		void Execute (int iAction, CDockScreen *pScreen);
		void ExecuteExitScreen (bool bForceUndock = false);
		void ExecuteShowPane (const CString &sPane);
		bool FindByID (const CString &sID, int *retiAction = NULL);
		bool FindByID (ICCItem *pItem, int *retiAction = NULL);
		bool FindByKey (const CString &sKey, int *retiAction);
		bool FindSpecial (CLanguage::ELabelAttribs iSpecial, int *retiAction);
		int GetCount (void) const { return m_Actions.GetCount(); }
		const CString &GetKey (int iAction) const { return m_Actions[iAction].sKey; }
		const CString &GetLabel (int iAction) const { return m_Actions[iAction].sLabel; }
		int GetVisibleCount (void) const;
		ALERROR InitFromXML (CExtension *pExtension, CXMLElement *pActions, ICCItem *pData, CString *retsError);
		bool IsEnabled (int iAction) const { return m_Actions[iAction].bEnabled; }
		bool IsSpecial (int iAction, CLanguage::ELabelAttribs iSpecial);
		bool IsVisible (int iAction) const { return m_Actions[iAction].bVisible; }
		ALERROR RemoveAction (int iAction);
		void SetDesc (int iAction, const CString &sDesc);
		void SetEnabled (int iAction, bool bEnabled = true);
		void SetLabel (int iAction, const CString &sLabelDesc, const CString &sKey);
		void SetSpecial (int iAction, CLanguage::ELabelAttribs iSpecial, bool bEnabled = true);
		bool SetSpecial (CCodeChain &CC, int iAction, ICCItem *pSpecial, ICCItem **retpError);
		void SetVisible (int iAction, bool bVisible = true);

	private:
		struct SActionDesc
			{
			SActionDesc (void) :
					cyHeight(0),
					rcRect({ 0, 0, 0, 0 }),
					pExtension(NULL),
					pCmd(NULL),
					pCode(NULL),
					bVisible(false),
					bEnabled(false),
					dwSpecial(0),
					bMinor(false),
					iKeyTmp(-1)
				{ }

			CString sID;
			CString sLabel;			//	Label for the action
			CString sKey;			//	Accelerator key
			CString sDescID;		//	Language ID to load description
			CString sDesc;			//	Description
			int cyHeight;			//	Justified height of button
			RECT rcRect;			//	RECT of button

			CExtension *pExtension;	//	Source of the code

			CXMLElement *pCmd;		//	Special commands (e.g., <Exit/>
			CString sCode;			//	Code
			ICCItem *pCode;			//	Code (owned by us)

			bool bVisible;			//	Action is visible
			bool bEnabled;			//	Action is enabled

			DWORD dwSpecial;		//	Special keys
			bool bMinor;			//	This is a minor/option button

			CString sLabelTmp;		//	Temporary cache (after justify)
			CString sKeyTmp;
			int iKeyTmp;
			CString sDescTmp;
			};

		void Arrange (EArrangements iArrangement, CDesignType *pRoot, const RECT &rcFrame);
		void ExecuteCode (CDockScreen *pScreen, const CString &sID, CExtension *pExtension, ICCItem *pCode);
		CLanguage::ELabelAttribs GetSpecialFromName (const CString &sSpecialName);
		int Justify (CDesignType *pRoot, int cxJustify);
		void SetLabelDesc (SActionDesc *pAction, const CString &sLabelDesc, bool bOverrideSpecial = true);
		void SetSpecial (SActionDesc *pAction, const TArray<CLanguage::ELabelAttribs> &Special);
		void SetSpecial (SActionDesc *pAction, CLanguage::ELabelAttribs iSpecial, bool bEnabled);

		TArray<SActionDesc> m_Actions;
		ICCItem *m_pData;			//	Data passed in to scrShowScreen (may be NULL)

		int m_cxJustify;			//	Width that we justified for
		bool m_bLongButtons;		//	If true, we display long buttons
		int m_iMinorButtonCount;	//	Number of minor buttons
		int m_iMajorButtonCount;	//	Number of major buttons
		int m_cyMajorButtons;		//	Total height of major buttons
		int m_cyTotalHeight;		//	Total height of all buttons
		int m_cyMax;				//	Height of tallest button
	};

class CDockPane
	{
	public:
		enum ELayouts
			{
			layoutNone,						//	Unknown or invalid layout

			layoutRight,					//	Right side pane (default)
			layoutLeft,						//	Left side pane
			layoutBottomBar,				//	Thin bottom bar
			};

		CDockPane (void);
		~CDockPane (void);

		void CleanUp (AGScreen *pScreen = NULL);
		void ClearDescriptionError (void) { m_bDescError = false; }
		void ExecuteCancelAction (void);
		void ExecuteShowPane (const CString &sPane, bool bDeferIfNecessary = false);
		CDockScreenActions &GetActions (void) { return m_Actions; }
		const CString &GetDescriptionString (void) const;
		int GetCounterValue (void) const;
		CString GetTextInputValue (void) const;
		bool HandleAction (DWORD dwTag, DWORD dwData);
		bool HandleChar (char chChar);
		bool HandleKeyDown (int iVirtKey);
		ALERROR InitPane (CDockScreen *pDockScreen, CXMLElement *pPaneDesc, const RECT &rcFullRect);
		bool SetControlValue (const CString &sID, ICCItem *pValue);
		void SetCounterValue (int iValue);
		void SetDescription (const CString &sDesc);
		void SetDescriptionError (const CString &sDesc);
		void SetTextInputValue (const CString &sValue);

	private:
		enum EControlTypes
			{
			controlNone,

			controlCounter,
			controlDesc,
			controlItemDisplay,
			controlItemListDisplay,
			controlTextInput,
			};

		struct SControl
			{
			SControl (void) :
					iType(controlNone),
					pArea(NULL),
					cyHeight(0),
					cyMinHeight(0),
					cyMaxHeight(0),
					bReplaceInput(false)
				{ }

			const CGTextArea *AsTextArea (void) const { return (const CGTextArea *)pArea; }
			CGTextArea *AsTextArea (void) { return (CGTextArea *)pArea; }
			CGItemDisplayArea *AsItemDisplayArea (void) { return (iType == controlItemDisplay ? (CGItemDisplayArea *)pArea : NULL); }
			CGItemListDisplayArea *AsItemListDisplayArea (void) { return (iType == controlItemListDisplay ? (CGItemListDisplayArea *)pArea : NULL); }

			EControlTypes iType;
			CString sID;					//	Control ID

			AGArea *pArea;

			int cyHeight;					//	Computed height of control
			int cyMinHeight;				//	Minimum control height
			int cyMaxHeight;				//	Desired control height

			bool bReplaceInput;				//	Keeps track of counter state
			};

		struct SControlStyle
			{
			CG32bitPixel BackColor;

			const CG16bitFont *pTextFont;
			CG32bitPixel TextColor;
			};

		void CreateControl (EControlTypes iType, const CString &sID, const CString &sStyle, RECT rcPane);
		ALERROR CreateControls (RECT rcPane, CString *retsError);
		void ExecuteAction (int iAction);
		bool FindControl (const CString &sID, const SControl **retpControl = NULL) const;
		bool FindControl (const CString &sID, SControl **retpControl = NULL);
		const CGTextArea *GetTextControlByType (EControlTypes iType) const;
		const SControl *GetControlByType (EControlTypes iType) const;
		SControl *GetControlByType (EControlTypes iType);
		void GetControlStyle (const CString &sStyle, SControlStyle *retStyle) const;
		bool InitLayout (const CString &sLayout, const RECT &rcFullRect, CString *retsError = NULL);
		void JustifyControls (int *retcyTotalHeight = NULL);
		void PositionControls (int x, int y);
		void RenderControlsBottomBar (void);
		void RenderControlsColumn (void);
		ALERROR ReportError (const CString &sError);

		CDockScreen *m_pDockScreen;			//	Dock screen object
		CXMLElement *m_pPaneDesc;			//	XML describing pane

		ELayouts m_iLayout;					//	Layout of controls and actions
		RECT m_rcControls;					//	Rectangular region for controls
		RECT m_rcActions;					//	Rectangular region for actions
		CGFrameArea *m_pContainer;			//	Hold all pane areas

		TArray<SControl> m_Controls;
		CDockScreenActions m_Actions;

		CString m_sDesc;					//	Current description
		bool m_bInShowPane;					//	Keep track of re-entrancy
		bool m_bInExecuteAction;			//	Inside m_Actions.Execute
		bool m_bDescError;					//	Main description is an error.
		CString m_sDeferredShowPane;
	};

class CDockScreenLayout
	{
	public:
		static constexpr int CARGO_STATUS_LABEL_WIDTH =	100;
		static constexpr int CARGO_STATUS_WIDTH =		200;
		static constexpr int MIN_DESC_PANE_WIDTH =		408;
		static constexpr int MAX_FRAME_IMAGE_WIDTH =	1500;
		static constexpr int MAX_FRAME_IMAGE_HEIGHT =	648;
		static constexpr int CONTENT_PADDING_LEFT =		8;
		static constexpr int CONTENT_PADDING_RIGHT =	8;
		static constexpr int MAX_CONTENT_WIDTH =		1024 + CONTENT_PADDING_LEFT + CONTENT_PADDING_RIGHT;
		static constexpr int STATUS_AREA_WIDTH =		400;
		static constexpr int STATUS_BAR_HEIGHT =		20;
		static constexpr int TITLE_HEIGHT =				72;
		static constexpr int TITLE_PADDING_LEFT =		8;

		RECT GetCanvasRect (void) const;
		RECT GetCargoStatusLabelRect (void) const;
		RECT GetCargoStatusRect (void) const;
		const RECT &GetContentRect (void) const { return m_rcContent; }
		RECT GetDisplayRect (void) const;
		int GetFrameImageFocusX (void) const { return m_xBackgroundFocus; }
		int GetFrameImageFocusY (void) const { return m_yBackgroundFocus; }
		int GetFrameImageHeight (void) const { return RectHeight(m_rcFrameImage); }
		int GetFrameImageWidth (void) const { return RectWidth(m_rcFrameImage); }
		const RECT &GetFrameRect (void) const { return m_rcFrame; }
		RECT GetMoneyStatusRect (void) const;
		RECT GetStatusBarRect (void) const;
		RECT GetTitleBarRect (void) const;
		RECT GetTitleRect (void) const;
		bool Init (const RECT &rcScreen, const CDockScreenVisuals &Visuals);

	private:
		int GetDisplayWidth (void) const { return m_cxDisplay; }
		int GetPaneWidth (void) const { return m_cxPane; }

		RECT m_rcScreen;						//	RECT of dock screen AGScreen
		RECT m_rcFrame;							//	RECT of background area (constrained to ~1280)
		RECT m_rcFrameImage;					//	RECT of where we paint the frame image
		RECT m_rcContent;						//	RECT of content area (contrained to ~1024)
		int m_xBackgroundFocus = 0;
		int m_yBackgroundFocus = 0;
		int m_cxDisplay = 0;
		int m_cxPane = 0;
		int m_cyStatusFont = 0;
	};

class CDockScreen : public IScreenController,
		public IDockScreenUI
	{
	public:
		static constexpr int FIRST_ACTION_ID =		100;
		static constexpr int LAST_ACTION_ID =		199;

		static constexpr int COUNTER_ID =			200;
		static constexpr int TEXT_INPUT_ID =		201;
		static constexpr int IMAGE_AREA_ID =		202;
		static constexpr int TAB_AREA_ID =			203;

		static constexpr int DISPLAY_ID =			300;

		CDockScreen (CGameSession &Session);
		virtual ~CDockScreen (void);

		void AddListFilter (const CString &sID, const CString &sLabel, const CItemCriteria &Filter);
		void CleanUpScreen (void);
		bool EvalString (const CString &sString, ICCItem *pData = NULL, bool bPlain = false, ECodeChainEvents iEvent = eventNone, CString *retsResult = NULL);
		void ExecuteCancelAction (void) { m_CurrentPane.ExecuteCancelAction(); }
		CDockScreenActions &GetActions (void) { return m_CurrentPane.GetActions(); }
		ICCItem *GetData (void) { return m_pData; }
		CExtension *GetExtension (void) const { return m_pExtension; }
		CSpaceObject *GetLocation (void) { return m_pLocation; }
		CDesignType *GetResolvedRoot (CString *retsResolveScreen = NULL) const;
		CDesignType *GetRoot (void) const { return m_pRoot; }
		AGScreen *GetScreen (void) const { return m_pScreen; }
		CUniverse &GetUniverse (void) const { return *g_pUniverse; }
		void HandleChar (char chChar);
		void HandleKeyDown (int iVirtKey);
		ALERROR InitScreen (HWND hWnd, 
							RECT &rcRect, 
							CDockScreenStack &FrameStack,
							CExtension *pExtension,
							CXMLElement *pDesc, 
							const CString &sInitialPane,
							ICCItem *pData,
							AGScreen **retpScreen,
							CString *retsError = NULL);
		bool InOnInit (void) const { return m_bInOnInit; }
		bool IsFirstOnInit (void) const { return m_bFirstOnInit; }
		bool IsValid (void) const { return (m_pScreen != NULL); }
		ALERROR ReportError (const CString &sError);
		void ResetFirstOnInit (void) { m_bFirstOnInit = true; }
		void SelectListFilter (const CString &sID);
		void SetListFilter (const CItemCriteria &Filter);
		void SetLocation (CSpaceObject *pLocation);
		void Update (int iTick);

		//	Methods used by script code
		void DeleteCurrentItem (int iCount) { m_pDisplay->DeleteCurrentItem(iCount); }
		int GetCounter (void) const { return m_CurrentPane.GetCounterValue(); }
		const CItem &GetCurrentItem (void) const { return m_pDisplay->GetCurrentItem(); }
		ICCItem *GetCurrentListEntry (void);
		const CString &GetDescription (void) const { return m_CurrentPane.GetDescriptionString(); }
		CG32bitImage *GetDisplayCanvas (const CString &sID);
		CItemListManipulator &GetItemListManipulator (void) { return m_pDisplay->GetItemListManipulator(); }
		int GetListCursor (void) const { return m_pDisplay->GetListCursor(); }
		IListData *GetListData (void) const { return m_pDisplay->GetListData(); }
        CGameSession &GetGameSession (void) { return m_Session; }
		ICCItemPtr GetProperty (const CString &sProperty) const;
		CString GetTextInput (void) const { return m_CurrentPane.GetTextInputValue(); }
        const CDockScreenVisuals &GetDockScreenVisuals (void) const;
		const CVisualPalette &GetVisuals (void) const { return g_pHI->GetVisuals(); }
		bool IsCurrentItemValid (void) const { return m_pDisplay->IsCurrentItemValid(); }
		void SelectNextItem (bool *retbMore = NULL);
		void SelectPrevItem (bool *retbMore = NULL);
		bool SelectTab (const CString &sID);
		void SetBackground (const IDockScreenDisplay::SBackgroundDesc &Desc);
		void SetDescription (const CString &sDesc) { m_CurrentPane.SetDescription(sDesc); }
		void SetDescriptionError (const CString &sDesc) { m_CurrentPane.SetDescriptionError(sDesc); }
		ALERROR SetDisplayText (const CString &sID, const CString &sText);
		bool SetControlValue (const CString &sID, ICCItem *pValue) { return m_CurrentPane.SetControlValue(sID, pValue); }
		void SetCounter (int iCount) { m_CurrentPane.SetCounterValue(iCount); }
		void SetListCursor (int iCursor);
		bool SetProperty (const CString &sProperty, ICCItem &Value);
		void SetTextInput (const CString &sText) { m_CurrentPane.SetTextInputValue(sText); }
		void ShowPane (const CString &sName);
		bool Translate (const CString &sTextID, ICCItem *pData, ICCItemPtr &pResult);

		//	IScreenController virtuals
		virtual void Action (DWORD dwTag, DWORD dwData = 0) override;

		//	IDockScreenUI
		virtual void OnModifyItemBegin (SModifyItemCtx &Ctx, CSpaceObject *pSource, const CItem &Item) override;
		virtual void OnModifyItemComplete (SModifyItemCtx &Ctx, CSpaceObject *pSource, const CItem &Result) override;
		virtual void OnObjDestroyed (const SDestroyCtx &Ctx) override;

	private:
		enum EControlTypes
			{
			ctrlText =						1,
			ctrlImage =						2,
			ctrlCanvas =					3,
			ctrlNeurohack =					100,
			};

		struct SDisplayControl
			{
			SDisplayControl (void) : pArea(NULL), pCode(NULL), bAnimate(false)
				{
				}

			~SDisplayControl (void)
				{
				if (pCode)
					pCode->Discard();
				}

			EControlTypes iType;
			CString sID;
			AGArea *pArea;
			ICCItem *pCode;

			bool bAnimate;
			};

		void BltSystemBackground (CSystem *pSystem, const RECT &rcRect);
		void BltToBackgroundImage (const RECT &rcRect, CG32bitImage *pImage, int xSrc, int ySrc, int cxSrc, int cySrc);
		void CleanUpBackgroundImage (void);
		ALERROR CreateBackgroundImage (const IDockScreenDisplay::SBackgroundDesc &Desc, const RECT &rcRect, int xOffset);
		void CreateScreenSetTabs (const IDockScreenDisplay::SInitCtx &Ctx, const IDockScreenDisplay::SDisplayOptions &Options, const TArray<SScreenSetTab> &ScreenSet, const CString &sCurTab);
		ALERROR CreateTitleArea (CXMLElement *pDesc, AGScreen *pScreen);
		bool EvalBool (const CString &sString);
		CString EvalInitialPane (void);
		CString EvalInitialPane (CSpaceObject *pSource, ICCItem *pData);
		SDisplayControl *FindDisplayControl (const CString &sID);
		ALERROR FireOnScreenInit (CSpaceObject *pSource, ICCItem *pData, CString *retsError);
		CString GetScreenName (CXMLElement *pDesc);
		ALERROR InitCodeChain (CTranscendenceWnd *pTrans, CSpaceObject *pStation);
		ALERROR InitDisplay (CXMLElement *pDisplayDesc, AGScreen *pScreen);
		void InitOnUpdate (CXMLElement *pDesc);
		void ShowDisplay (bool bAnimateOnly = false);
		void ShowItem (void) { m_pDisplay->ShowItem(); }
		void UpdateCredits (void);

		void AddDisplayControl (CXMLElement *pDesc, 
								AGScreen *pScreen, 
								SDisplayControl *pParent, 
								const RECT &rcFrame, 
								SDisplayControl **retpDControl = NULL);
		void InitDisplayControlRect (CXMLElement *pDesc, const RECT &rcFrame, RECT *retrcRect);

		const SFontTable *m_pFonts = NULL;
        CGameSession &m_Session;
		CPlayerShipController *m_pPlayer = NULL;
		CDesignType *m_pRoot = NULL;
		CString m_sScreen;
		CSpaceObject *m_pLocation = NULL;
		ICCItem *m_pData = NULL;
		CExtension *m_pExtension = NULL;
		CXMLElement *m_pDesc = NULL;
		bool m_bFirstOnInit = true;
		bool m_bInOnInit = false;

		//	Screen and metrics
		AGScreen *m_pScreen = NULL;
		CDockScreenLayout m_Layout;

		//	Title and header
		CG32bitImage *m_pBackgroundImage = NULL;
		bool m_bFreeBackgroundImage = false;
		CGTextArea *m_pCredits = NULL;
		CGTextArea *m_pCargoSpace = NULL;
		CGTabArea *m_pTabs = NULL;

		//	Display controls
		TArray<SDisplayControl> m_Controls;
		CXMLElement *m_pDisplayInitialize = NULL;
		bool m_bDisplayAnimate = false;

		IDockScreenDisplay *m_pDisplay = NULL;

		//	Panes
		CXMLElement *m_pPanes = NULL;
		CDockPane m_CurrentPane;

		//	Events
		ICCItemPtr m_pOnScreenUpdate;

		//	Runtime
		IDockScreenDisplay::SBackgroundDesc m_DeferredBackground;
		TSortMap<CString, CString> m_DeferredDisplayText;
	};

#include "DockScreenDisplayImpl.h"
#include "DockScreenSubjugate.h"
