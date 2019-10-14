//	CDockSession.cpp
//
//	CDockSession class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define DEFAULT_SCREEN_NAME						CONSTLIT("[DefaultScreen]")

IDockScreenUI CDockSession::m_NullUI;

bool CDockSession::ExitScreen (DWORD dwFlags)

//	ExitScreen
//
//	Exits the current screen and returns TRUE if we're still inside a docking
//	session (FALSE otherwise).

	{
	DEBUG_TRY

	ASSERT(!m_DockFrames.IsEmpty());
	if (m_DockFrames.IsEmpty())
		return false;

	//	If we have another frame, then switch back to that screen

	bool bForceUndock = ((dwFlags & FLAG_FORCE_UNDOCK) ? true : false);
	if (m_DockFrames.GetCount() > 1 && !bForceUndock)
		{
		//	Remove the current frame

		m_DockFrames.Pop();
		return true;
		}

	//	Otherwise, exit

	else
		{
		const SDockFrame &Frame = m_DockFrames.GetCurrent();

		//	Unmark the object

		Frame.pLocation->ClearPlayerDocked();

		//	Clean up

		m_pDockScreenUI = &m_NullUI;
		m_pDefaultScreensRoot = NULL;
		m_DockFrames.DeleteAll();

		return false;
		}

	DEBUG_CATCH
	}

bool CDockSession::FindScreenRoot (const CString &sScreen, CDesignType **retpRoot, CString *retsScreen, ICCItemPtr *retpData) const

//	FindScreenRoot
//
//	Resolves a screen.

	{
	CSpaceObject *pPlayerShip;

	//	If the screen is an UNID, then expect a stand-alone screen

	bool bNotANumber;
	DWORD dwUNID = (DWORD)strToInt(sScreen, 0, &bNotANumber);
	if (!bNotANumber)
		{
		CDesignType *pRoot = GetUniverse().GetDesignCollection().FindDockScreen(dwUNID);
		if (pRoot == NULL)
			return false;

		if (retpRoot)
			*retpRoot = pRoot;

		if (retsScreen)
			*retsScreen = sScreen;

		if (retpData)
			*retpData = NULL;

		return true;
		}

	//	Otherwise, if this is the special name [DefaultScreen] then
	//	we ask the object for the proper screen

	else if (strEquals(sScreen, DEFAULT_SCREEN_NAME))
		{
		CString sScreenActual;
		ICCItemPtr pData;

		ASSERT(!m_DockFrames.IsEmpty());
		const SDockFrame &Frame = m_DockFrames.GetCurrent();
		ASSERT(Frame.pLocation);

		CDesignType *pRoot = Frame.pLocation->GetFirstDockScreen(&sScreenActual, &pData);
		if (pRoot == NULL)
			return false;

		if (retpRoot)
			*retpRoot = pRoot;

		if (retsScreen)
			*retsScreen = sScreenActual;

		if (retpData)
			*retpData = pData;

		return true;
		}

	//	Otherwise, this is a local screen, so we use the default root

	else if (m_pDefaultScreensRoot)
		{
		if (retpRoot)
			*retpRoot = m_pDefaultScreensRoot;

		if (retsScreen)
			*retsScreen = sScreen;

		if (retpData)
			*retpData = NULL;

		return true;
		}

	//	Otherwise, use the player ship

	else if (pPlayerShip = GetUniverse().GetPlayerShip())
		{
		if (retpRoot)
			*retpRoot = pPlayerShip->GetType();

		if (retsScreen)
			*retsScreen = sScreen;

		if (retpData)
			*retpData = NULL;

		return true;
		}

	//	Otherwise, we could not find the root

	else
		return false;
	}

const SScreenSetTab *CDockSession::FindTab (const CString &sID) const

//	FindTab
//
//	Returns the tab by ID.

	{
	if (!InSession())
		return NULL;

	const SDockFrame &Current = m_DockFrames.GetCurrent();

	for (int i = 0; i < Current.ScreenSet.GetCount(); i++)
		if (strEquals(sID, Current.ScreenSet[i].sID))
			return &Current.ScreenSet[i];

	return NULL;
	}

ICCItemPtr CDockSession::GetData (const CString &sAttrib) const

//	GetData
//
//	Returns data for the given attribute. The caller is responsible for 
//	discarding this data.

	{
	if (!InSession())
		return ICCItemPtr(ICCItem::Nil);

	const SDockFrame &Frame = m_DockFrames.GetCurrent();
	if (Frame.pStoredData)
		{
		if (ICCItem *pValue = Frame.pStoredData->GetElement(sAttrib))
			return ICCItemPtr(pValue->Reference());
		}

	if (Frame.pInitialData)
		{
		if (ICCItem *pValue = Frame.pInitialData->GetElement(sAttrib))
			return ICCItemPtr(pValue->Reference());
		}

	return ICCItemPtr(ICCItem::Nil);
	}

ICCItemPtr CDockSession::GetReturnData (const CString &sAttrib) const

//	GetReturnData
//
//	Returns data for the given attribute.

	{
	if (!InSession())
		return ICCItemPtr(ICCItem::Nil);

	const SDockFrame &Frame = m_DockFrames.GetCurrent();
	if (Frame.pReturnData)
		{
		if (ICCItem *pValue = Frame.pReturnData->GetElement(sAttrib))
			return ICCItemPtr(pValue->Reference());
		}

	return ICCItemPtr(ICCItem::Nil);
	}

void CDockSession::IncData (const CString &sAttrib, ICCItem *pOptionalInc, ICCItemPtr *retpResult)

//	IncData
//
//	Increments data

	{
	if (!InSession())
		{
		if (retpResult) *retpResult = ICCItemPtr(ICCItem::Nil);
		return;
		}

    //  If pValue is NULL, we default to 1. We add ref no matter what so that
    //  we can discard unconditionally.

	ICCItemPtr pInc;
	if (pOptionalInc)
		pInc = ICCItemPtr(pOptionalInc->Reference());
	else
		pInc = ICCItemPtr(1);

    //  If the entry is currently blank, then we just take the increment.

	ICCItemPtr pOriginal = GetData(sAttrib);
    ICCItemPtr pResult;
    if (pOriginal->IsNil())
        pResult = pInc;

    //  Otherwise, we need to get the data value

    else
        {
        if (pOriginal->IsDouble() || pInc->IsDouble())
            pResult = ICCItemPtr(pOriginal->GetDoubleValue() + pInc->GetDoubleValue());
        else
            pResult = ICCItemPtr(pOriginal->GetIntegerValue() + pInc->GetIntegerValue());
        }

    //  Store

	SetData(sAttrib, pResult);

    //  Done

    if (retpResult)
        *retpResult = pResult;
	}

bool CDockSession::ModifyItemNotificationNeeded (CSpaceObject *pSource) const

//	ModifyItemNotificationNeeded
//
//	Returns TRUE if we need to notify a dock screen UI that an item on the given
//	source has been modified. We do this strictly for performance.

	{
	//	If not in a dock screen session, then we never need a notification.

	if (!InSession() || pSource == NULL)
		return false;

	//	If we've modified either the player ship or an object that we're docked
	//	with, then we need a notification.

	if (pSource == m_DockFrames.GetCurrent().pLocation
			|| pSource == GetUniverse().GetPlayerShip())
		return true;

	//	Otherwise, no notification.

	return false;
	}

CSpaceObject *CDockSession::OnPlayerDocked (IDockScreenUI &DockScreenUI, CSpaceObject *pObj)

//	OnPlayerDocked
//
//	Player has docked with the given object. We return the object that we 
//	actually docked with (in case of redirect).

	{
	ASSERT(!InSession());
	if (InSession())
		return m_DockFrames.GetCurrent().pLocation;

	//	Remember the UI

	m_pDockScreenUI = &DockScreenUI;

	//	See if the object wants to redirect us. In that case, m_pStation
	//	will remain the original object, but gSource and m_pLocation in the
	//	dock screen will be set to the new object

	CSpaceObject *pDock;
	if (!pObj->FireOnDockObjAdj(&pDock))
		pDock = pObj;

	//	Remember the default screens root.
	//	Note that this can be different from the root obtained above (from
	//	GetDockScreen) because GetDockScreen sometimes returns a top-level
	//	dock screen (in those cases, the default screens root is still
	//	the original object type).

	m_pDefaultScreensRoot = pDock->GetType();

	//	Done

	return pDock;
	}

void CDockSession::OnPlayerShowShipScreen (IDockScreenUI &DockScreenUI, CDesignType *pDefaultScreensRoot)

//	OnPlayerShowShipScreen
//
//	Player has invoked ship screen.

	{
	if (InSession())
		return;

	//	Remember the UI

	m_pDockScreenUI = &DockScreenUI;

	//	If the default root is passed in, use that. Otherwise, we pull local
	//	screens from the ship class.

	if (pDefaultScreensRoot)
		m_pDefaultScreensRoot = pDefaultScreensRoot;
	else
		{
		CSpaceObject *pPlayerShip = GetUniverse().GetPlayerShip();
		if (pPlayerShip)
			m_pDefaultScreensRoot = pPlayerShip->GetType();
		}
	}

bool CDockSession::SetData (const CString &sAttrib, ICCItem *pData)

//	SetData
//
//	Sets data associated with the current frame

	{
	if (!InSession())
		return false;

	//	If necessary, create the stored data block

	SDockFrame &Frame = m_DockFrames.GetCurrent();
	if (!Frame.pStoredData)
		Frame.pStoredData = ICCItemPtr(ICCItem::SymbolTable);

	//	Add the entry

	Frame.pStoredData->SetAt(sAttrib, pData);
	return true;
	}

bool CDockSession::SetReturnData (const CString &sAttrib, ICCItem *pData)

//	SetReturnData
//
//	Sets data associated with previous frame.

	{
	if (!InSession())
		return false;

	//	If necessary, create the stored data block

	SDockFrame &Frame = m_DockFrames.GetCurrent();
	if (!Frame.pReturnData)
		Frame.pReturnData = ICCItemPtr(ICCItem::SymbolTable);

	//	Add the entry

	Frame.pReturnData->SetAt(sAttrib, pData);
	return true;
	}

bool CDockSession::SetScreenSet (const ICCItem &ScreenSet)

//	SetScreenSet
//
//	Sets the current screen set.
//
//	We expect ScreenSet to be an array of structs; each struc has the following
//	fields:
//
//		id: ID of the tab (if Nil, we assign one)
//		label: User-visible tab name
//		screen: Screen to navigate to
//		pane: Pane to navigate to
//		data: Data
//
//	We return FALSE if we could not set the screen set.

	{
	if (!InSession())
		return false;

	SDockFrame &Frame = m_DockFrames.GetCurrent();
	Frame.ScreenSet.DeleteAll();

	Frame.ScreenSet.GrowToFit(ScreenSet.GetCount());
	for (int i = 0; i < ScreenSet.GetCount(); i++)
		{
		const ICCItem &Entry = *ScreenSet.GetElement(i);
		if (Entry.IsNil())
			continue;

		SScreenSetTab &NewTab = *Frame.ScreenSet.Insert();
		NewTab.sID = Entry.GetStringAt(CONSTLIT("id"), strPatternSubst(CONSTLIT("tab.%d"), i));
		NewTab.sName = Entry.GetStringAt(CONSTLIT("label"), strPatternSubst(CONSTLIT("Tab %d"), i));
		NewTab.sScreen = Entry.GetStringAt(CONSTLIT("screen"));
		NewTab.sPane = Entry.GetStringAt(CONSTLIT("pane"));
		ICCItem *pData = Entry.GetElement(CONSTLIT("data"));
		if (pData)
			NewTab.pData = ICCItemPtr(pData->Reference());

		ICCItem *pEnabled = Entry.GetElement(CONSTLIT("enabled"));
		if (pEnabled && pEnabled->IsNil() && Frame.ScreenSet.GetCount() > 1)
			NewTab.bEnabled = false;
		}

	//	For now we assume that the current screen is the first screen in the 
	//	screen set.

	if (Frame.ScreenSet.GetCount() > 0)
		Frame.sCurrentTab = Frame.ScreenSet[0].sID;
	else
		Frame.sCurrentTab = NULL_STR;

	return true;
	}

bool CDockSession::Translate (const CString &sID, ICCItem *pData, ICCItemPtr &pResult, CString *retsError) const

//	Translate
//
//	Translates a text ID. We return Nil if we could not find the text ID.

	{
	//	If not in a screen session, then nothing

	if (!InSession())
		{
		if (retsError) *retsError = CONSTLIT("Not in a dock screen.");
		return false;
		}

	const SDockFrame &Frame = GetCurrentFrame();

	//	First ask the current docking location to translate

	if (Frame.pLocation && Frame.pLocation->Translate(sID, pData, pResult))
		return true;

	//	Otherwise, let the screen translate

	if (Frame.pResolvedRoot && Frame.pResolvedRoot->Translate(Frame.pLocation, sID, pData, pResult))
		return true;

	//	Otherwise, we have no translation

	if (retsError) *retsError = strPatternSubst(CONSTLIT("Unknown Language ID: %s"), sID);
	return false;
	}
