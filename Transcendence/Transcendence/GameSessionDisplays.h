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
		RECT m_rcScreen;

		//	HUDs

		TUniquePtr<IHUDPainter> m_pHUD[hudCount];
		int m_iSelection = -1;						//  Selected armor seg (or -1)
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

		CMenuDisplay (CHumanInterface &HI) :
				m_HI(HI)
			{ }

		void Hide (void) { }
		void Init (const RECT &rcScreen);
		bool OnChar (char chChar, DWORD dwKeyData);
		void Paint (CG32bitImage &Screen, int iTick) const;
		void Show (const CMenuData &Data, EPositions iPosition);

	private:
		void DoCommand (int iIndex);
		void Realize (void) const;

		CHumanInterface &m_HI;
		CMenuData m_Data;
		int m_iSelection = -1;

		RECT m_rcScreen = { 0 };					//	Rect of entire screen
		RECT m_rcRect = { 0 };						//	Rect of menu

		mutable CG32bitImage m_Buffer;
		mutable bool m_bInvalid = true;
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
