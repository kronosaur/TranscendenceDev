//	GameSessionDisplays.h
//
//	Transcendence session classes
//	Copyright (c) 2017 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CHeadsUpDisplay
	{
	public:
		CHeadsUpDisplay (CHumanInterface &HI, CTranscendenceModel &Model) :
				m_HI(HI),
				m_Model(Model)
			{ }

		CHeadsUpDisplay (const CHeadsUpDisplay &Src) = delete;
		CHeadsUpDisplay (CHeadsUpDisplay &&Src) = delete;

		~CHeadsUpDisplay (void) { CleanUp(); }

		CHeadsUpDisplay &operator= (const CHeadsUpDisplay &Src) = delete;
		CHeadsUpDisplay &operator= (CHeadsUpDisplay &&Src) = delete;

		void CleanUp (void);
		void GetClearHorzRect (RECT *retrcRect) const;
		bool Init (const RECT &rcRect, CString *retsError = NULL);
		void Invalidate (EHUDTypes iHUD = hudNone);
		void Paint (CG32bitImage &Screen, int iTick, bool bInDockScreen = false);
		void SetArmorSelection (int iSelection);
		void Update (int iTick);

	private:
		bool CreateHUD (EHUDTypes iHUD, const CShipClass &SourceClass, const RECT &rcScreen, CString *retsError = NULL);
		void InvalidateHUD (EHUDTypes iHUD) { if (m_pHUD[iHUD]) m_pHUD[iHUD]->Invalidate(); }
		void PaintHUD (EHUDTypes iHUD, CG32bitImage &Screen, SHUDPaintCtx &PaintCtx) const;

		CHumanInterface &m_HI;
		CTranscendenceModel &m_Model;
		RECT m_rcScreen = { 0 };

		//	HUDs

		TUniquePtr<IHUDPainter> m_pHUD[hudCount];
		int m_iSelection = -1;						//  Selected armor seg (or -1)
	};

class CIconBarDisplay
	{
	public:
		CIconBarDisplay (CHumanInterface &HI) :
				m_HI(HI)
			{ }

		CGameKeys::Keys GetLastCommand (void) const { return m_iLastCommand; }
		void Init (const RECT &rcScreen, const CMenuData &Data);
		bool IsEmpty (void) const { return m_Data.IsEmpty(); }
		bool OnLButtonDblClick (int x, int y, DWORD dwFlags) { return OnLButtonDown(x, y, dwFlags); }
		bool OnLButtonDown (int x, int y, DWORD dwFlags);
		bool OnLButtonUp (int x, int y, DWORD dwFlags);
		bool OnMouseMove (int x, int y);
		void Paint (CG32bitImage &Screen, int iTick) const;
		void Refresh (void) { m_iSelected = -1; m_iHover = -1; m_bInvalid =true; }

	private:
		static constexpr int DEFAULT_ICON_HEIGHT = 48;
		static constexpr int DEFAULT_ICON_WIDTH = 48;

		static constexpr int ENTRY_BORDER_RADIUS = 4;

		int HitTest (int x, int y) const;
		void Realize (void) const;

		CHumanInterface &m_HI;
		CMenuData m_Data;

		RECT m_rcScreen = { 0 };					//	Rect of entire screen
		RECT m_rcRect = { 0 };						//	Rect of icon bar
		int m_cxEntry = 0;							//	Width of a single icon
		int m_cyEntry = 0;							//	Height of a single icon

		int m_iSelected = -1;						//	Selected entry (or -1)
		int m_iHover = -1;							//	Entry that we're hovering over (or -1)
		bool m_bDown = false;						//	TRUE if mouse is down over menu.
		CGameKeys::Keys m_iLastCommand = CGameKeys::keyNone;

		mutable CG32bitImage m_Buffer;
		mutable bool m_bInvalid = true;
	};

class CMenuDisplay
	{
	public:
		enum EPositions
			{
			posNone,

			posCenter,
			posRight,
			posLeft,
			};

		struct SOptions
			{
			EPositions iPos = posCenter;
			bool bHideShortCutKeys = false;
			};

		CMenuDisplay (CHumanInterface &HI, CTranscendenceModel &Model) :
				m_HI(HI),
				m_Model(Model)
			{ }

		void Hide (void) { m_Buffer.Delete(); m_bInvalid = true; }
		void Init (const RECT &rcScreen);
		bool OnChar (char chChar, DWORD dwKeyData);
		bool OnKeyDown (int iVirtKey, DWORD dwKeyData) { return false; }
		bool OnLButtonDblClick (int x, int y, DWORD dwFlags) { return OnLButtonDown(x, y, dwFlags); }
		bool OnLButtonDown (int x, int y, DWORD dwFlags);
		bool OnLButtonUp (int x, int y, DWORD dwFlags);
		bool OnMouseMove (int x, int y);
		void Paint (CG32bitImage &Screen, int iTick) const;
		void Show (const CMenuData &Data, const SOptions &Options = SOptions());

	private:
		static constexpr BYTE MENU_BACKGROUND_OPACITY =	200;
		static constexpr int MENU_BORDER_RADIUS =		4;
		static constexpr int MENU_BORDER_WIDTH =		1;

		static constexpr int MENU_ITEM_WIDTH =			300;
		static constexpr int MENU_ITEM_VPADDING =		4;
		static constexpr int MENU_ITEM_HPADDING =		4;

		void DoCommand (int iIndex);
		int HitTest (int x, int y) const;
		void Realize (void) const;

		CHumanInterface &m_HI;
		CTranscendenceModel &m_Model;
		CMenuData m_Data;

		bool m_bHideShortCutKeys = false;			//	Do not show short-cut if using accelerators
		RECT m_rcScreen = { 0 };					//	Rect of entire screen
		RECT m_rcRect = { 0 };						//	Rect of menu
		int m_xFirstEntry = 0;
		int m_yFirstEntry = 0;
		int m_cxEntry = 0;							//	Width of a single entry
		int m_cyEntry = 0;							//	Height of a single entry

		int m_iHover = -1;							//	Entry that we're hovering over (or -1)
		bool m_bDown = false;						//	TRUE if mouse is down over menu.

		mutable CG32bitImage m_Buffer;
		mutable bool m_bInvalid = true;
	};

class CMessageDisplay
	{
	public:
		CMessageDisplay (CHumanInterface &HI) :
				m_HI(HI)
			{ }

		void ClearAll (void);
		void DisplayCommandHint (DWORD dwVirtKey, const CString &sMessage) { AddMessage(dwVirtKey, sMessage); }
		void DisplayMessage (const CString &sMessage) { AddMessage(CVirtualKeyData::INVALID_VIRT_KEY, sMessage); }
		void Init (const RECT &rcScreen);
		void Paint (CG32bitImage &Dest);
		void Update (void);

	private:
		static constexpr int DEFAULT_BLINK_TIME = 15;
		static constexpr int DEFAULT_STEADY_TIME = 150;
		static constexpr int DEFAULT_FADE_TIME = 30;

		static constexpr int DISPLAY_WIDTH = 400;
		static constexpr int DISPLAY_HEIGHT = 128;
		static constexpr int INNER_PADDING_HORZ = 4;

		static constexpr int MESSAGE_QUEUE_SIZE = 5;

		enum State
			{
			stateClear,						//	Blank (stays permanently)
			stateNormal,					//	Normal (stays permanently)
			stateBlinking,					//	Blinking (for m_iBlinkTime)
			stateSteady,					//	Normal (for m_iSteadyTime)
			stateFading						//	Fade to black (for m_iFadeTime)
			};

		struct SMessage
			{
			CString sMessage;				//	Message to paint
			DWORD dwVirtKey = CVirtualKeyData::INVALID_VIRT_KEY;	//	UI key hint (optional)
			State iState = stateNormal;		//	current state (blinking, etc)
			int iTick = 0;					//	Tick count for this message
			CG32bitPixel rgbColor;			//	Color to paint

			mutable int x = -1;				//	Location of message (-1 = not yet computed)
			};

		void AddMessage (DWORD dwVirtKey, const CString &sMessage);
		int Next (int iPos) { return ((iPos + 1) % MESSAGE_QUEUE_SIZE); }
		void PaintMessage (CG32bitImage &Dest, const SMessage &Msg, int y, CG32bitPixel rgbColor) const;
		int Prev (int iPos) { return ((iPos + MESSAGE_QUEUE_SIZE - 1) % MESSAGE_QUEUE_SIZE); }

		CHumanInterface &m_HI;
		RECT m_rcRect = { 0 };

		int m_iFirstMessage = 0;
		int m_iNextMessage = 0;
		SMessage m_Messages[MESSAGE_QUEUE_SIZE];

		int m_cySmoothScroll = 0;
	};

enum class ENarrativeDisplayStyle
	{
	none,

	missionAccept,
	};

class CNarrativeDisplay
	{
	public:
		CNarrativeDisplay (CHumanInterface &HI) :
				m_HI(HI),
				m_Painter(HI.GetVisuals())
			{ }

		void Init (const RECT &rcRect);
		void Paint (CG32bitImage &Screen, int iTick) const;
		void Show (ENarrativeDisplayStyle iStyle, const CTileData &Data);
		void Update (int iTick);

	private:
		static constexpr int FADE_IN_TIME = 5;
		static constexpr int NORMAL_TIME = 300;
		static constexpr int FADE_OUT_TIME = 15;
		static constexpr int DISPLAY_WIDTH = 600;
		static constexpr int DISPLAY_HEIGHT = 96;
		static constexpr int IMAGE_WIDTH = 192;
		static constexpr int IMAGE_HEIGHT = 96;
		static constexpr int MARGIN_X = 20;
		static constexpr int MARGIN_Y = 20;

		static constexpr CG32bitPixel RGB_BACKGROUND_COLOR = CG32bitPixel(80, 80, 80);
		static constexpr BYTE BACKGROUND_OPACITY = 0x40;
		static constexpr int BACKGROUND_CORNER_RADIUS = 4;

		enum EStates
			{
			stateNone,

			stateStart,
			stateFadeIn,
			stateNormal,
			stateFadeOut,
			};

		CHumanInterface &m_HI;

		CTilePainter m_Painter;

		EStates m_iState = stateNone;
		int m_iStartTick = 0;
		BYTE m_byOpacity = 0;

		CG32bitImage m_Buffer;
		RECT m_rcRect = { 0 };
		RECT m_rcIcon = { 0 };
		RECT m_rcText = { 0 };
	};

class CSystemMapDisplay
	{
	public:
		CSystemMapDisplay (CHumanInterface &HI, CTranscendenceModel &Model, CHeadsUpDisplay &HUD);

		bool HandleKeyDown (int iVirtKey, DWORD dwKeyData);
		bool HandleMouseWheel (int iDelta, int x, int y, DWORD dwFlags);
		bool Init (const RECT &rcRect);
		void OnHideMap (void);
		void OnShowMap (void);
		void Paint (CG32bitImage &Screen);

	private:
		static constexpr int DEFAULT_ZOOM = 100;
		static constexpr int MIN_ZOOM = 10;
		static constexpr int MAX_ZOOM = 400;

		Metric GetScaleKlicksPerPixel (int iScale) const;

		CHumanInterface &m_HI;
		CTranscendenceModel &m_Model;
		CHeadsUpDisplay &m_HUD;
		RECT m_rcScreen;

		CMapScaleCounter m_Scale;           //  Track current map scale
		CMapLegendPainter m_HelpPainter;
	};

class CSystemStationsMenu : IAniCommand
	{
	public:
		CSystemStationsMenu (CHumanInterface &HI, CTranscendenceModel &Model, IHISession &Session);

		void Hide (void);
		void SetAsDestination (void);
		bool Show (const RECT &rcRect, const CString &sID);

		//	IAniCommand

		virtual void OnAniCommand (const CString &sID, const CString &sEvent, const CString &sCmd, DWORD dwData) override;

	private:
		static constexpr int DETAIL_PANE_PADDING_RIGHT = 10;
		static constexpr int DETAIL_PANE_HEIGHT = 512;
		static constexpr int DETAIL_PANE_WIDTH = 512;

		void HighlightObject (CSystem &System, DWORD dwObjID);
		void UnhighlightObject (CSystem &System);

		CHumanInterface &m_HI;
		CTranscendenceModel &m_Model;
		IHISession &m_Session;

		CString m_sID;						//	ID of the performance
		IAnimatron *m_pList = NULL;			//	List
		DWORD m_dwCurObjID = 0;				//	Currently selected object ID
	};

class CGameIconBarData
	{
	public:
		CMenuData CreateIconBar (const CDesignCollection &Design, const CGameSettings &Settings) const;
		void Invalidate (void) { m_bValid = false; }

	private:
		static constexpr int DEFAULT_COUNT = 7;
		static constexpr int ICON_WIDTH = 48;
		static constexpr int ICON_HEIGHT = 48;

		struct SEntry
			{
			CGameKeys::Keys iCmd = CGameKeys::keyNone;
			int iImage = -1;
			};

		mutable bool m_bValid = false;
		mutable TSharedPtr<CG32bitImage> m_pImage;
		mutable TArray<CObjectImageArray> m_Images;

		static const SEntry m_Defaults[DEFAULT_COUNT];
	};
