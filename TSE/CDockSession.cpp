//	CDockSession.cpp
//
//	CDockSession class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define DEFAULT_SCREEN_NAME						CONSTLIT("[DefaultScreen]")

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
		CDesignType *pRoot = g_pUniverse->FindSharedDockScreen(dwUNID);
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

	else if (pPlayerShip = g_pUniverse->GetPlayerShip())
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

CSpaceObject *CDockSession::OnPlayerDocked (CSpaceObject *pObj)

//	OnPlayerDocked
//
//	Player has docked with the given object. We return the object that we 
//	actually docked with (in case of redirect).

	{
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
