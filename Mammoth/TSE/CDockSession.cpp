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
		ClearAmbientSound();

		//	Clean up

		m_pDockScreenUI = &m_NullUI;
		m_pDefaultScreensRoot = NULL;
		m_DockFrames.DeleteAll();

		//	Call exit code, if necessary. We do this before getting rid of 
		//	session data because we might want to access it.

		RunExitCode();

		//	Done

		m_UndockCode.DeleteAll();
		m_pStoredData.Delete();

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

ICCItemPtr CDockSession::GetSessionData (const CString &sAttrib) const

//	GetSessionData
//
//	Returns data for the whole session.

	{
	if (!InSession() || !m_pStoredData)
		return ICCItemPtr(ICCItem::Nil);

	else if (ICCItem *pValue = m_pStoredData->GetElement(sAttrib))
		return ICCItemPtr(pValue->Reference());

	else
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

bool CDockSession::ModifyItemNotificationNeeded (const CSpaceObject &Source) const

//	ModifyItemNotificationNeeded
//
//	Returns TRUE if we need to notify a dock screen UI that an item on the given
//	source has been modified. We do this strictly for performance.

	{
	//	If not in a dock screen session, then we never need a notification.

	if (!InSession())
		return false;

	//	If we've modified either the player ship or an object that we're docked
	//	with, then we need a notification.

	if (Source == m_DockFrames.GetCurrent().pLocation
			|| Source == GetUniverse().GetPlayerShip())
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

void CDockSession::PlayAmbientSound ()

//	PlayAmbientSound
//
//	Plays the currently selected ambient sound.

	{
	if (m_pAmbientSound == m_pAmbientSoundPlaying)
		return;

	if (m_pAmbientSoundPlaying)
		GetUniverse().StopSound(m_pAmbientSoundPlaying->GetSound());

	if (m_pAmbientSound)
		GetUniverse().PlaySound(NULL, m_pAmbientSound->GetSound());

	m_pAmbientSoundPlaying = m_pAmbientSound;
	}

void CDockSession::RunExitCode ()

//	RunExitCode
//
//	Runs all exit code.

	{
	for (int i = 0; i < m_UndockCode.GetCount(); i++)
		{
		CCodeChainCtx CCX(GetUniverse());

		ICCItemPtr pResult = CCX.RunLambdaCode(m_UndockCode[i]);
		if (pResult->IsError())
			{
			CString sError = pResult->GetStringValue();
			::kernelDebugLogString(sError);
			}
		}
	}

void CDockSession::SetAmbientSound (const CSoundResource *pSound)

//	SetAmbientSound
//
//	Sets the current ambient sound.

	{
	if (m_pAmbientSound != pSound)
		{
		m_pAmbientSound = pSound;
		}
	}

bool CDockSession::SetData (const CString &sAttrib, const ICCItem *pData)

//	SetData
//
//	Sets data associated with the current frame

	{
	if (!InSession())
		return false;

	//	If nil, then we delete the entry

	if (pData == NULL || pData->IsNil())
		{
		SDockFrame &Frame = m_DockFrames.GetCurrent();
		if (Frame.pStoredData)
			Frame.pStoredData->DeleteAt(sAttrib);
		}

	//	Otherwise, add entry

	else
		{
		//	If necessary, create the stored data block

		SDockFrame &Frame = m_DockFrames.GetCurrent();
		if (!Frame.pStoredData)
			Frame.pStoredData = ICCItemPtr(ICCItem::SymbolTable);

		//	Clone, because we don't want to inherit any future changes to this
		//	item.

		ICCItemPtr pClone(pData->CloneContainer());

		//	Add the entry

		Frame.pStoredData->SetAt(sAttrib, pClone);
		}

	return true;
	}

bool CDockSession::SetReturnData (const CString &sAttrib, ICCItem *pData)

//	SetReturnData
//
//	Sets data associated with previous frame.

	{
	if (m_DockFrames.GetCount() < 2)
		return false;

	//	If necessary, create the stored data block

	SDockFrame &Frame = m_DockFrames.GetCallingFrame();
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
//	We expect ScreenSet to be an array of structs; each struct has the following
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

bool CDockSession::SetSessionData (const CString &sAttrib, ICCItem *pData)

//	SetSessionData
//
//	Sets data associated with the whole session.

	{
	if (!InSession())
		return false;

	//	If necessary, create the stored data block

	if (!m_pStoredData)
		m_pStoredData = ICCItemPtr(ICCItem::SymbolTable);

	//	Add the entry

	m_pStoredData->SetAt(sAttrib, pData);
	return true;
	}

bool CDockSession::ShowScreen (CSpaceObject &Location, CDesignType *pRoot, const CString &sScreen, const CString &sPane, ICCItem *pData)

//	ShowScreen
//
//	Adds a new frame. Returns TRUE if this is the first frame.

	{
	bool bFirstFrame = m_DockFrames.IsEmpty();

	//	If this is our first frame, then this is the first OnInit

	if (bFirstFrame)
		{
		ClearOnInitFlag();

		//	Mark the object so that it knows that the player is docked with it.
		//	We need this so that the object can tell us if its items change.

		Location.SetPlayerDocked();

		//	Initialize the ambient sound.

		SetAmbientSound(Location.GetDockScreenAmbientSound());
		}

	//	If not the first frame, then save the selection so we can restore it
	//	later when we come back.

	else
		{
		m_DockFrames.GetCurrent().pSavedSelection = m_pDockScreenUI->GetDisplaySelection();
		}

	//	Add a new frame.
	//	Note that pRoot might be NULL and sScreen might be [DefaultScreen] at
	//	this point.

	SDockFrame NewFrame;
	NewFrame.pLocation = &Location;
	NewFrame.pRoot = pRoot;
	NewFrame.sScreen = sScreen;
	NewFrame.sPane = sPane;
	if (pData)
		NewFrame.pInitialData = ICCItemPtr(pData->Reference());
	NewFrame.pResolvedRoot = pRoot;
	NewFrame.sResolvedScreen = (pRoot ? sScreen : NULL_STR);
	m_DockFrames.Push(NewFrame);

	return bFirstFrame;
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

	if (Frame.pResolvedRoot)
		{
		if (Frame.pLocation && Frame.pResolvedRoot->Translate(*Frame.pLocation, sID, pData, pResult))
			return true;
		else if (Frame.pResolvedRoot->Translate(sID, pData, pResult))
			return true;
		}

	//	Otherwise, we have no translation

	if (retsError) *retsError = strPatternSubst(CONSTLIT("Unknown Language ID: %s"), sID);
	return false;
	}
