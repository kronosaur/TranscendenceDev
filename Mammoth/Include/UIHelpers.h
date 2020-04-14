//	UIHelpers.h
//
//	Classes and methods for generating appropriate UI.
//	Copyright (c) 2012 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CSmallOptionButtonAnimator
	{
	public:
		CSmallOptionButtonAnimator (IHISession &Session) :
				m_Session(Session)
			{ }

		void Create (CAniVScroller &Root, const CString &sID, const CString &sLabel, int x, int y, int cxWidth, AlignmentStyles iAlign = alignLeft);
		bool IsEditing (void) const { return m_bInEditMode; }
		void SetEnabled (bool bEnable = true);
		void SetImage (const CG32bitImage &Image, bool bFreeImage = false);
		void SetText (const CString &sText);
		void StartEdit (int cxWidth, const CString &sValue);
		void StopEdit (CString *retsValue = NULL);

	private:
		static constexpr int MAJOR_PADDING_BOTTOM =				20;
		static constexpr int MAJOR_PADDING_HORZ =				20;
		static constexpr int MAJOR_PADDING_TOP =				20;
		static constexpr int MAJOR_PADDING_VERT =				20;
		static constexpr int SMALL_BUTTON_HEIGHT =				48;
		static constexpr int SMALL_BUTTON_WIDTH =				48;

		IHISession &m_Session;

		CAniVScroller *m_pRoot = NULL;
		CString m_sID;
		int m_x = 0;
		int m_y = 0;
		int m_cxWidth = 0;
		AlignmentStyles m_iAlign = alignLeft;
		bool m_bInEditMode = false;
	};

class CTextAreaAnimator
	{
	public:
		CTextAreaAnimator (IHISession &Session) :
				m_Session(Session)
			{ }

		void Create (CAniVScroller &Root, const CString &sID, const CString &sText, int x, int y, int cxWidth, AlignmentStyles iAlign = alignLeft);
		void SetText (const CString &sText);

	private:
		IHISession &m_Session;

		CAniVScroller *m_pRoot = NULL;
		CString m_sID;
		int m_x = 0;
		int m_y = 0;
		int m_cxWidth = 0;
		AlignmentStyles m_iAlign = alignLeft;
	};

class CItemDataAnimatron : public IAnimatron
	{
	public:
		struct SDamageAdjCell
			{
			DamageTypes iDamage;
			CString sText;
			};

		CItemDataAnimatron (const CVisualPalette &VI, const CItem &Item);

		inline bool IsEmpty (void) const { return (m_DamageAdj.GetCount() == 0); }

		//	IAnimatron virtuals
		virtual void GetSpacingRect (RECT *retrcRect);
		virtual void Paint (SAniPaintCtx &Ctx);

		static void CalcDamageAdj (const CItem &Item, TArray<SDamageAdjCell> *retDamageAdj);
		static int MeasureDamageAdj (const CG16bitFont &Font, const TArray<SDamageAdjCell> &DamageAdj);

	private:
		const CVisualPalette &m_VI;
		CItem m_Item;

		bool m_bIsWeapon;
		TArray<SDamageAdjCell> m_DamageAdj;
	};

class CListCollectionTask : public IHITask
	{
	public:
		struct SOptions
			{
			DWORD dwSelectUNID = 0;

			RECT rcRect = { 0 };
			TSharedPtr<CG32bitImage> pGenericIcon;

			bool bShowLibraries = false;
			};

		CListCollectionTask (CHumanInterface &HI, 
							 CExtensionCollection &Extensions, 
							 CMultiverseModel &Multiverse, 
							 CCloudService &Service, 
							 const SOptions &Options = SOptions());
		~CListCollectionTask (void);

		const TArray<CMultiverseCatalogEntry> &GetCollection (void) const { return m_Collection; }
		IAnimatron *GetListHandoff (void) { IAnimatron *pResult = m_pList; m_pList = NULL; return pResult; }

		//	IHITask virtuals
		virtual ALERROR OnExecute (ITaskProcessor *pProcessor, CString *retsResult);

	private:
		void CreateEntry (CMultiverseCatalogEntry *pCatalogEntry, int yStart, IAnimatron **retpEntry, int *retcyHeight);
		CG32bitImage *CreateEntryIcon (CMultiverseCatalogEntry &Entry) const;

		CExtensionCollection &m_Extensions;
		CMultiverseModel &m_Multiverse;
		CCloudService &m_Service;
		SOptions m_Options;

		TArray<CMultiverseCatalogEntry> m_Collection;
		CAniListBox *m_pList = NULL;
	};

class CListSaveFilesTask : public IHITask
	{
	public:
		static constexpr int DEFAULT_WIDTH = 720;

		struct SOptions
			{
			CString sUsername;					//	Sign-in player
			int cxWidth = DEFAULT_WIDTH;		//	Width of area
			bool bFilterPermadeath = false;		//	Don't show non-permadeath games
			bool bDebugSaveFiles = false;		//	Add debug info
			};

		CListSaveFilesTask (CHumanInterface &HI, const TArray<CString> &Folders, const SOptions &Options = SOptions());
		~CListSaveFilesTask (void);

		IAnimatron *GetListHandoff (void) { IAnimatron *pResult = m_pList; m_pList = NULL; return pResult; }

		//	IHITask virtuals
		virtual ALERROR OnExecute (ITaskProcessor *pProcessor, CString *retsResult);

	private:
		void CreateFileEntry (CGameFile &GameFile, const CTimeDate &ModifiedTime, int yStart, IAnimatron **retpEntry, int *retcyHeight);

		TArray<CString> m_Folders;
		SOptions m_Options;

		CAniListBox *m_pList = NULL;
	};

class CReadProfileTask : public IHITask
	{
	public:
		CReadProfileTask (CHumanInterface &HI, CCloudService &Service, int cxWidth);
		~CReadProfileTask (void);

		IAnimatron *GetListHandoff (void) { IAnimatron *pResult = m_pList; m_pList = NULL; return pResult; }

		//	IHITask virtuals
		virtual ALERROR OnExecute (ITaskProcessor *pProcessor, CString *retsResult);

	private:
		void CreateAdventureRecordDisplay (CAdventureRecord &Record, int yStart, IAnimatron **retpAni);

		CCloudService &m_Service;
		int m_cxWidth;

		CUserProfile m_Profile;

		CAniVScroller *m_pList;
	};

class CUIHelper
	{
	public:
		//	Generic Options
		static constexpr DWORD OPTION_ALIGN_BOTTOM =	0x00000001;
		static constexpr DWORD OPTION_ALIGN_CENTER =	0x00000002;
		static constexpr DWORD OPTION_ALIGN_MIDDLE =	0x00000004;
		static constexpr DWORD OPTION_ALIGN_RIGHT =		0x00000008;
		static constexpr DWORD OPTION_NO_MARGIN =		0x00000010;
		static constexpr DWORD OPTION_VERTICAL =		0x00000020;

		//	CreateClassInfo???
		static constexpr DWORD OPTION_ITEM_RIGHT_ALIGN =				0x00000001;

		//	CreateSessionFrameBar
		static constexpr DWORD OPTION_FRAME_ALIGN_TOP =					0x00000001;
		static constexpr DWORD OPTION_FRAME_ALIGN_BOTTOM =				0x00000002;

		//	CreateSessionTitle
		static constexpr DWORD OPTION_SESSION_OK_BUTTON =				0x00000001;
		static constexpr DWORD OPTION_SESSION_NO_CANCEL_BUTTON =		0x00000002;
		static constexpr DWORD OPTION_SESSION_ADD_EXTENSION_BUTTON =	0x00000004;
		static constexpr DWORD OPTION_SESSION_NO_HEADER =				0x00000008;

		//	SMenuEntry flags
		static constexpr DWORD MENU_TEXT =								0x00000001;		//	Show on left-hand text menu
		static constexpr DWORD MENU_ALIGN_CENTER =						0x00000002;		//	Align center
		static constexpr DWORD MENU_ALIGN_LEFT =						0x00000004;		//	Align left
		static constexpr DWORD MENU_ALIGN_RIGHT =						0x00000008;		//	Align right
		static constexpr DWORD MENU_HIDDEN =							0x00000010;
		static constexpr DWORD MENU_DISABLED =							0x00000020;
		static constexpr DWORD MENU_IGNORE_DOUBLE_CLICK =				0x00000040;		//	Ignore double-click on button

		struct SMenuEntry
			{
			CString sLabel;
			CString sCommand;
			const CG32bitImage *pIcon = NULL;

			//	MENU_TEXT
			//	MENU_ALIGN_CENTER
			//	MENU_ALIGN_LEFT
			//	MENU_ALIGN_RIGHT

			DWORD dwFlags = 0;
			};

		CUIHelper (CHumanInterface &HI) : m_HI(HI) { }

		//	OPTION_ITEM_RIGHT_ALIGN
		void CreateClassInfoArmor (const CShipClass &Class, int x, int y, int cxWidth, DWORD dwOptions, int *retcyHeight, IAnimatron **retpInfo) const;
		void CreateClassInfoCargo (const CShipClass &Class, const CDeviceDescList &Devices, int x, int y, int cxWidth, DWORD dwOptions, int *retcyHeight, IAnimatron **retpInfo) const;
		void CreateClassInfoDeviceSlots (const CShipClass &Class, const CDeviceDescList &Devices, int x, int y, int cxWidth, DWORD dwOptions, int *retcyHeight, IAnimatron **retpInfo) const;
		void CreateClassInfoDrive (const CShipClass &Class, const CDeviceDescList &Devices, int x, int y, int cxWidth, DWORD dwOptions, int *retcyHeight, IAnimatron **retpInfo) const;
		void CreateClassInfoItem (const CItem &Item, int x, int y, int cxWidth, DWORD dwOptions, const CString &sExtraDesc, int *retcyHeight, IAnimatron **retpInfo) const;
		void CreateClassInfoReactor (const CShipClass &Class, const CDeviceDescList &Devices, int x, int y, int cxWidth, DWORD dwOptions, int *retcyHeight, IAnimatron **retpInfo) const;
		CG32bitImage CreateGlowBackground (void) const;
		CG32bitImage CreateGlowBackground (int cxWidth, int cyHeight, CG32bitPixel rgbCenter, CG32bitPixel rgbEdge) const;
		void CreateInputErrorMessage (IHISession *pSession, const RECT &rcRect, const CString &sTitle, CString &sDesc, IAnimatron **retpMsg = NULL) const;

		//	OPTION_FRAME_ALIGN_TOP
		//	OPTION_FRAME_ALIGN_BOTTOM
		void CreateSessionFrameBar (IHISession *pSession, const TArray<SMenuEntry> *pMenu, DWORD dwOptions, IAnimatron **retpControl) const;

		//	OPTION_SESSION_ADD_EXTENSION_BUTTON
		//	OPTION_SESSION_NO_CANCEL_BUTTON
		//	OPTION_SESSION_NO_HEADER
		//	OPTION_SESSION_OK_BUTTON
		void CreateSessionTitle (IHISession *pSession, 
								 CCloudService &Service, 
								 const CString &sTitle, 
								 const TArray<SMenuEntry> *pMenu,
								 DWORD dwOptions, 
								 IAnimatron **retpControl) const;
		void CreateSessionWaitAnimation (const CString &sID, const CString &sText, IAnimatron **retpControl) const;

		void GenerateDockScreenRTF (const CString &sText, CString *retsRTF) const;

		//	OPTION_ALIGN_BOTTOM
		//	OPTION_ALIGN_RIGHT
		//	OPTION_NO_MARGIN
		//	OPTION_VERTICAL
		void PaintDisplayAttribs (CG32bitImage &Dest, int x, int y, const TArray<SDisplayAttribute> &Attribs, DWORD dwOptions = 0) const;

		void RefreshMenu (IHISession *pSession, IAnimatron *pRoot, const TArray<SMenuEntry> &Menu) const;

		static void PaintRTFText (const CVisualPalette &VI, CG32bitImage &Dest, const RECT &rcRect, const CString &sRTFText, const CG16bitFont &DefaultFont, CG32bitPixel rgbDefaultColor, int *retcyHeight = NULL);
		static int ScrollAnimationDecay (int iOffset);

	private:
		static constexpr int ENHANCEMENT_ICON_HEIGHT = 40;
		static constexpr int ENHANCEMENT_ICON_WIDTH = 40;

		void CreateClassInfoSpecialItem (CItemType *pItemIcon, const CString &sText, int x, int y, int cxWidth, DWORD dwOptions, int *retcyHeight, IAnimatron **retpInfo) const;
		void CreateBarButtons (CAniSequencer *pSeq, const RECT &rcRect, IHISession *pSession, const TArray<SMenuEntry> *pMenu, DWORD dwOptions) const;

		CHumanInterface &m_HI;
	};

class CInputErrorMessageController : public IAnimatron, public IAniCommand
	{
	public:
		CInputErrorMessageController (IHISession *pSession) : m_pSession(pSession) { }

		//	IAnimatron virtuals
		virtual void GetSpacingRect (RECT *retrcRect) { retrcRect->left = 0; retrcRect->right = 0; retrcRect->top = 0; retrcRect->bottom = 0; }
		virtual void Paint (SAniPaintCtx &Ctx) { }

	protected:
		//	IAniCommand virtuals
		virtual void OnAniCommand (const CString &sID, const CString &sEvent, const CString &sCmd, DWORD dwData);

	private:
		IHISession *m_pSession;
	};
