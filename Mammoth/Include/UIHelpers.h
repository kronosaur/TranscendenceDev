//	UIHelpers.h
//
//	Classes and methods for generating appropriate UI.
//	Copyright (c) 2012 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

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

		//	PaintItemEntry
		static constexpr DWORD OPTION_SELECTED =						0x00000001;
		static constexpr DWORD OPTION_NO_ICON =							0x00000002;
		static constexpr DWORD OPTION_TITLE =							0x00000004;
		static constexpr DWORD OPTION_SMALL_ICON =						0x00000008;		//	Paint icon at 64x64
		static constexpr DWORD OPTION_NO_PADDING =						0x00000010;		//	No padding around item
		static constexpr DWORD OPTION_NO_ARMOR_SPEED_DISPLAY =			0x00000020;		//	Do not show bonus/penalty for armor
		static constexpr DWORD OPTION_KNOWN =							0x00000040;		//	Show items as known

		//	SMenuEntry flags
		static constexpr DWORD MENU_TEXT =								0x00000001;		//	Show on left-hand text menu
		static constexpr DWORD MENU_ALIGN_CENTER =						0x00000002;		//	Align center
		static constexpr DWORD MENU_ALIGN_LEFT =						0x00000004;		//	Align left
		static constexpr DWORD MENU_ALIGN_RIGHT =						0x00000008;		//	Align right
		static constexpr DWORD MENU_HIDDEN =							0x00000010;
		static constexpr DWORD MENU_DISABLED =							0x00000020;

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

		int CalcItemEntryHeight (CSpaceObject *pSource, const CItem &Item, const RECT &rcRect, DWORD dwOptions) const;

		//	OPTION_ITEM_RIGHT_ALIGN
		void CreateClassInfoArmor (CShipClass *pClass, int x, int y, int cxWidth, DWORD dwOptions, int *retcyHeight, IAnimatron **retpInfo) const;
		void CreateClassInfoCargo (CShipClass *pClass, const CDeviceDescList &Devices, int x, int y, int cxWidth, DWORD dwOptions, int *retcyHeight, IAnimatron **retpInfo) const;
		void CreateClassInfoDeviceSlots (CShipClass *pClass, const CDeviceDescList &Devices, int x, int y, int cxWidth, DWORD dwOptions, int *retcyHeight, IAnimatron **retpInfo) const;
		void CreateClassInfoDrive (CShipClass *pClass, const CDeviceDescList &Devices, int x, int y, int cxWidth, DWORD dwOptions, int *retcyHeight, IAnimatron **retpInfo) const;
		void CreateClassInfoItem (const CItem &Item, int x, int y, int cxWidth, DWORD dwOptions, const CString &sExtraDesc, int *retcyHeight, IAnimatron **retpInfo) const;
		void CreateClassInfoReactor (CShipClass *pClass, const CDeviceDescList &Devices, int x, int y, int cxWidth, DWORD dwOptions, int *retcyHeight, IAnimatron **retpInfo) const;
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

		void FormatDisplayAttributes (TArray<SDisplayAttribute> &Attribs, const RECT &rcRect, CCartoucheBlock &retBlock, int *retcyHeight = NULL) const;

		void GenerateDockScreenRTF (const CString &sText, CString *retsRTF) const;

		//	OPTION_ALIGN_BOTTOM
		//	OPTION_ALIGN_RIGHT
		//	OPTION_NO_MARGIN
		//	OPTION_VERTICAL
		void PaintDisplayAttribs (CG32bitImage &Dest, int x, int y, const TArray<SDisplayAttribute> &Attribs, DWORD dwOptions = 0) const;

		//	OPTION_SELECTED
		//	OPTION_NO_ICON
		//	OPTION_TITLE
		//	OPTION_SMALL_ICON
		//	OPTION_NO_PADDING
		//	OPTION_NO_ARMOR_SPEED_DISPLAY
		//	OPTION_KNOWN
		void PaintItemEntry (CG32bitImage &Dest, CSpaceObject *pSource, const CItem &Item, const RECT &rcRect, CG32bitPixel rgbText, DWORD dwOptions) const;
		void PaintReferenceDamageAdj (CG32bitImage &Dest, int x, int y, int iLevel, int iHP, const int *iDamageAdj, CG32bitPixel rgbText) const;
		void PaintReferenceDamageType (CG32bitImage &Dest, int x, int y, int iDamageType, const CString &sRef, CG32bitPixel rgbText) const;
		void PaintRTFText (CG32bitImage &Dest, const RECT &rcRect, const CString &sRTFText, const CG16bitFont &DefaultFont, CG32bitPixel rgbDefaultColor, int *retcyHeight = NULL) const;
		void RefreshMenu (IHISession *pSession, IAnimatron *pRoot, const TArray<SMenuEntry> &Menu) const;

		static int ScrollAnimationDecay (int iOffset);

	private:
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
