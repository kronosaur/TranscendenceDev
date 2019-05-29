//	TSEDockScreenSession.h
//
//	Classes and functions for dock screen types.
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

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

		virtual void OnModifyItemBegin (SModifyItemCtx &Ctx, CSpaceObject *pSource, const CItem &Item) { }
		virtual void OnModifyItemComplete (SModifyItemCtx &Ctx, CSpaceObject *pSource, const CItem &Result) { }
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
		const SScreenSetTab *FindTab (const CString &sID) const;
		ICCItem *GetData (const CString &sAttrib);
		const CString &GetDisplayData (const CString &sID);
		inline int GetCount (void) const { return m_Stack.GetCount(); }
		const SDockFrame &GetCurrent (void) const;
		ICCItem *GetReturnData (const CString &sAttrib);
		void IncData (const CString &sAttrib, ICCItem *pData, ICCItem **retpResult = NULL);
		inline bool IsEmpty (void) const { return (m_Stack.GetCount() == 0); }
		void Push (const SDockFrame &Frame);
		void Pop (void);
		void ResolveCurrent (const SDockFrame &ResolvedFrame);
		void SetCurrent (const SDockFrame &NewFrame, SDockFrame *retPrevFrame = NULL);
		void SetCurrentPane (const CString &sPane);
		void SetData (const CString &sAttrib, ICCItem *pData);
		void SetDisplayData (const CString &sID, const CString &sData);
		void SetLocation (CSpaceObject *pLocation);
		void SetReturnData (const CString &sAttrib, ICCItem *pData);
		void SetScreenSet (const ICCItem &ScreenSet);

	private:
		inline CUniverse &GetUniverse (void) const { return *g_pUniverse; }

		TArray<SDockFrame> m_Stack;

		static const SDockFrame m_NullFrame;
	};

class CDockSession
	{
	public:
		static constexpr DWORD FLAG_FORCE_UNDOCK =	0x00000001;

		//	FLAG_FORCE_UNDOCK

		bool ExitScreen (DWORD dwFlags = 0);
		bool FindScreenRoot (const CString &sScreen, CDesignType **retpRoot, CString *retsScreen, ICCItemPtr *retpData) const;
		const SDockFrame &GetCurrentFrame (void) const { return m_DockFrames.GetCurrent(); }
		CDockScreenStack &GetFrameStack (void) { return m_DockFrames; }
		const CDockScreenStack &GetFrameStack (void) const { return m_DockFrames; }
		inline CUniverse &GetUniverse (void) const { return *g_pUniverse; }
		inline bool InSession (void) const { return !m_DockFrames.IsEmpty(); }
		inline void OnModifyItemBegin (IDockScreenUI::SModifyItemCtx &Ctx, CSpaceObject *pSource, const CItem &Item) { if (ModifyItemNotificationNeeded(pSource)) m_pDockScreenUI->OnModifyItemBegin(Ctx, pSource, Item); }
		inline void OnModifyItemComplete (IDockScreenUI::SModifyItemCtx &Ctx, CSpaceObject *pSource, const CItem &Result) { if (ModifyItemNotificationNeeded(pSource)) m_pDockScreenUI->OnModifyItemComplete(Ctx, pSource, Result); }
		CSpaceObject *OnPlayerDocked (IDockScreenUI &DockScreenUI, CSpaceObject *pObj);
		void OnPlayerShowShipScreen (IDockScreenUI &DockScreenUI, CDesignType *pDefaultScreensRoot);

	private:
		bool ModifyItemNotificationNeeded (CSpaceObject *pSource) const;

		IDockScreenUI *m_pDockScreenUI = NULL;		//	Wormhole to dockscreen UI
		CDesignType *m_pDefaultScreensRoot = NULL;	//	Default root to look for local screens
		CDockScreenStack m_DockFrames;				//	Stack of dock screens
	};
