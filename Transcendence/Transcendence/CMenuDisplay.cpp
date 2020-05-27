//	CMenuDisplay
//
//	CMenuDisplay class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Transcendence.h"

void CMenuDisplay::DoCommand (int iIndex)

//	DoCommand
//
//	Invoke the given menu item.

	{
	IHISession *pSession = m_HI.GetSession();
	if (pSession == NULL)
		return;

	CString sID = m_Data.GetItemID(iIndex);
	if (sID.IsBlank())
		return;

	m_Model.GetUniverse().PlaySound(NULL, m_Model.GetUniverse().FindSound(UNID_DEFAULT_SELECT));
	pSession->HICommand(sID);
	}

void CMenuDisplay::Init (const RECT &rcScreen)

//	Init
//
//	Initialize

	{
	m_rcScreen = rcScreen;
	m_bInvalid = true;
	}

bool CMenuDisplay::OnChar (char chChar, DWORD dwKeyData)

//	OnChar
//
//	Handle a character.

	{
	int iIndex = m_Data.FindItemByKey(CString(&chChar, 1));
	if (iIndex < 0)
		return false;

	DoCommand(iIndex);
	return true;
	}

void CMenuDisplay::Paint (CG32bitImage &Screen, int iTick) const

//	Paint
//
//	Paints the menu.

	{
	if (m_Data.IsEmpty())
		return;
	}

void CMenuDisplay::Realize (void) const

//	Realize
//
//	Paint the buffer.

	{
	if (m_Data.IsEmpty() || !m_bInvalid)
		return;
	}

void CMenuDisplay::Show (const CMenuData &Data, EPositions iPosition)

//	Show
//
//	Shows the menu.

	{
	m_Data = Data;

	}
