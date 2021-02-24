//	TSEDockScreenSession.h
//
//	Classes and functions for dock screen types.
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.
//
//	USAGE
//
//	CDockSession encapsulates all state for a docking session, including the
//	screen stack, screen data, etc. It has a weak pointer to a dock screen UI
//	object, to which it will send notifications.

#pragma once

class IDockScreenUI
	{
	public:
		enum EListSelectChanges
			{
			selNone,						//	No change

			selOriginal,					//	Select the original item
			selModified,					//	Select the modified item
			selReset,						//	Reset the list
			};

		struct SModifyItemCtx
			{
			EListSelectChanges iSelChange = selNone;

			CItem OriginalItem;
			int iOriginalCursor = -1;
			};

		virtual const CString &GetDescription () const { return NULL_STR; }
		virtual ICCItemPtr GetDisplaySelection () const { return ICCItemPtr::Nil(); }
		virtual void *GetDockScreen () const { return NULL; }
		virtual ICCItemPtr GetListAsCCItem (void) const { return ICCItemPtr::Nil(); }
		virtual int GetListCursor (void) const { return -1; }
		virtual ICCItemPtr GetProperty (const CString &sProperty) const { return NULL; }
		virtual void OnModifyItemBegin (SModifyItemCtx &Ctx, const CSpaceObject &Source, const CItem &Item) const { }
		virtual void OnModifyItemComplete (SModifyItemCtx &Ctx, const CSpaceObject &Source, const CItem &Result) { }
		virtual void OnObjDestroyed (const SDestroyCtx &Ctx) { }
		virtual bool SetProperty (const CString &sProperty, const ICCItem &Value) { return false; }
		virtual void SetSelection (const ICCItem &Selection) { }
	};

enum class EDockScreenBackground
	{
	defaultBackground,				//	Use the default specified by the display

	none,							//	No background image
	image,							//	Use an image (by UNID)
	objHeroImage,					//	Use the object's hero image
	objSchematicImage,				//	Use the object's top-down image
	heroImage,						//	Use specified image as hero image
	};

struct SDockScreenBackgroundDesc
	{
	EDockScreenBackground iType = EDockScreenBackground::defaultBackground;		//	Type of image defined
	DWORD dwImageID = 0;				//	UNID to use (if iType == backgroundImage)
	RECT rcImage = { 0 };				//	Source image rect (if 0, use entire image)
	CSpaceObject *pObj = NULL;			//	Object to query (if iType == backgroundObjXXX)

	RECT rcImagePadding = { 0 };
	DWORD dwImageAlign = 0;				//	AlignmentStyles
	};

struct SScreenSetTab
	{
	CString sID;							//	ID (to refer to it)
	CString sName;							//	User-visible tab name
	CString sScreen;						//	Screen to navigate to
	CString sPane;							//	Pane to navigate to
	ICCItemPtr pData;						//	Data for screen

	bool bEnabled = true;					//	Table is enabled
	};

struct SDockFrame
	{
	CSpaceObject *pLocation = NULL;			//	Current location
	CDesignType *pRoot = NULL;				//	Either a screen or a type with screens
	CString sScreen;						//	Screen name (UNID or name)
	CString sPane;							//	Current pane
	ICCItemPtr pInitialData;				//	Data for the screen
	ICCItemPtr pStoredData;					//	Read-write data
	ICCItemPtr pReturnData;					//	Data returns from a previous screen
	ICCItemPtr pSavedSelection;				//	Saved selection (for when we return to screen)

	SDockScreenBackgroundDesc BackgroundDesc;

	TArray<SScreenSetTab> ScreenSet;		//	Current screen set
	CString sCurrentTab;					//	Current tab in screen set.

	CDesignType *pResolvedRoot = NULL;
	CString sResolvedScreen;

	TSortMap<CString, CString> DisplayData;	//	Opaque data used by displays
	};

struct SShowScreenCtx
	{
	CDesignType *pRoot = NULL;
	CString sScreen;
	CString sPane;
	ICCItemPtr pData;

	CString sTab;
	bool bReturn = false;
	bool bFirstFrame = false;
	};

class CDockScreenStack
	{
	public:
		void DeleteAll (void);
		const CString &GetDisplayData (const CString &sID);
		int GetCount (void) const { return m_Stack.GetCount(); }
		SDockFrame &GetCallingFrame (void) { return (m_Stack.GetCount() < 2 ? const_cast<SDockFrame &>(m_NullFrame) : m_Stack[m_Stack.GetCount() - 2]); }
		SDockFrame &GetCurrent (void) { ASSERT(!IsEmpty()); return (IsEmpty() ? const_cast<SDockFrame &>(m_NullFrame) : m_Stack[m_Stack.GetCount() - 1]); }
		const SDockFrame &GetCurrent (void) const;
		const SDockFrame &GetFrame (int iIndex) const { return m_Stack[iIndex]; }
		bool IsEmpty (void) const { return (m_Stack.GetCount() == 0); }
		void Push (const SDockFrame &Frame);
		void Pop (void);
		void ResolveCurrent (const SDockFrame &ResolvedFrame);
		void SetBackgroundDesc (const SDockScreenBackgroundDesc &BackgroundDesc);
		void SetCurrent (const SDockFrame &NewFrame, SDockFrame *retPrevFrame = NULL);
		void SetCurrentPane (const CString &sPane);
		void SetDisplayData (const CString &sID, const CString &sData);
		void SetLocation (CSpaceObject *pLocation);

		static ICCItemPtr AsCCItem (const SDockFrame &Frame);

	private:
		CUniverse &GetUniverse (void) const { return *g_pUniverse; }

		TArray<SDockFrame> m_Stack;

		static const SDockFrame m_NullFrame;
	};

class CDockSession
	{
	public:
		static constexpr DWORD FLAG_FORCE_UNDOCK =	0x00000001;

		void AddUndockCode (const CString &sID, ICCItem &Code) { *m_UndockCode.SetAt(sID) = ICCItemPtr(Code); }
		void ClearOnInitFlag () { m_bOnInitCalled = false; }

		//	FLAG_FORCE_UNDOCK
		bool ExitScreen (DWORD dwFlags = 0);

		bool FindScreenRoot (const CString &sScreen, CDesignType **retpRoot, CString *retsScreen, ICCItemPtr *retpData) const;
		const SScreenSetTab *FindTab (const CString &sID) const;
		const SDockFrame &GetCurrentFrame (void) const { return m_DockFrames.GetCurrent(); }
		ICCItemPtr GetData (const CString &sAttrib) const;
		CDockScreenStack &GetFrameStack (void) { return m_DockFrames; }
		const CDockScreenStack &GetFrameStack (void) const { return m_DockFrames; }
		ICCItemPtr GetProperty (const CString &sProperty) const;
		ICCItemPtr GetPropertyFrameStack (void) const;
		ICCItemPtr GetReturnData (const CString &sAttrib) const;
		ICCItemPtr GetSessionData (const CString &sAttrib) const;
		IDockScreenUI &GetUI (void) const { return *m_pDockScreenUI; }
		CUniverse &GetUniverse (void) const { return *g_pUniverse; }
		void IncData (const CString &sAttrib, ICCItem *pOptionalInc, ICCItemPtr *retpResult);
		void InitCustomProperties (void);
		bool InSession (void) const { return !m_DockFrames.IsEmpty(); }
		bool IsFirstOnInit () const { return !m_bOnInitCalled; }
		void OnInitCalled () { m_bOnInitCalled = true; }
		void OnModifyItemBegin (IDockScreenUI::SModifyItemCtx &Ctx, const CSpaceObject &Source, const CItem &Item) const { if (ModifyItemNotificationNeeded(Source)) m_pDockScreenUI->OnModifyItemBegin(Ctx, Source, Item); }
		void OnModifyItemComplete (IDockScreenUI::SModifyItemCtx &Ctx, const CSpaceObject &Source, const CItem &Result) { if (ModifyItemNotificationNeeded(Source)) m_pDockScreenUI->OnModifyItemComplete(Ctx, Source, Result); }
		CSpaceObject *OnPlayerDocked (IDockScreenUI &DockScreenUI, CSpaceObject *pObj);
		void OnPlayerShowShipScreen (IDockScreenUI &DockScreenUI, CDesignType *pDefaultScreensRoot);
		void PlayAmbientSound ();
		void SetBackgroundDesc (const SDockScreenBackgroundDesc &BackgroundDesc) { m_DockFrames.SetBackgroundDesc(BackgroundDesc); }
		void SetCurrentPane (const CString &sPane) { m_DockFrames.SetCurrentPane(sPane); }
		bool SetData (const CString &sAttrib, const ICCItem *pValue);
		bool SetProperty (const CString &sProperty, const ICCItem &Value, CString *retsError = NULL);
		bool SetReturnData (const CString &sAttrib, ICCItem *pValues);
		bool SetScreenSet (const ICCItem &ScreenSet);
		bool SetSessionData (const CString &sAttrib, ICCItem *pValues);
		bool ShowScreen (CSpaceObject &Location, CDesignType *pRoot, const CString &sScreen, const CString &sPane, ICCItem *pData);
		void StopAmbientSound () { PlayAmbientSound(); }
		bool Translate (const CString &sID, ICCItem *pData, ICCItemPtr &pResult, CString *retsError = NULL) const;

	private:
		void ClearAmbientSound () { SetAmbientSound(NULL); }
		void InitCustomProperties (const CDesignType &Type, const SDockFrame &Frame);
		bool ModifyItemNotificationNeeded (const CSpaceObject &Source) const;
		void RunExitCode ();
		void SetAmbientSound (const CSoundResource *pSound);

		IDockScreenUI *m_pDockScreenUI = &m_NullUI;		//	Wormhole to dockscreen UI
		CDesignType *m_pDefaultScreensRoot = NULL;		//	Default root to look for local screens
		CDockScreenStack m_DockFrames;					//	Stack of dock screens
		ICCItemPtr m_pStoredData;						//	Session data
		TSortMap<CString, ICCItemPtr> m_UndockCode;		//	Code to run when undocking
		const CSoundResource *m_pAmbientSound = NULL;	//	Current ambient sound
		const CSoundResource *m_pAmbientSoundPlaying = NULL;
		bool m_bOnInitCalled = false;					//	TRUE if we've already called OnInit

		static TPropertyHandler<CDockSession> m_PropertyTable;
		static IDockScreenUI m_NullUI;
	};
