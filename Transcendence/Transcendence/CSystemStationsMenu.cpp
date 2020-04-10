//	CSystemStationsMenu.cpp
//
//	CSystemStationsMenu class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Transcendence.h"

#define EVENT_ON_SELECTION_CHANGED				CONSTLIT("onSelectionChanged")

#define PROP_SELECTION_ID						CONSTLIT("selectionID")

CSystemStationsMenu::CSystemStationsMenu (CHumanInterface &HI, CTranscendenceModel &Model, IHISession &Session) : 
		m_HI(HI),
		m_Model(Model),
		m_Session(Session)

//	CSystemStationsMenu constructor

	{
	}

void CSystemStationsMenu::Hide (void)

//	Hide
//
//	Hide the menu

	{
	if (!m_pList)
		return;

	//	Unhighlight the current object, if any

	CSystem *pSystem = m_Model.GetUniverse().GetCurrentSystem();
	if (pSystem)
		UnhighlightObject(*pSystem);

    //  We always stop the performance, if there is one

	m_Session.StopPerformance(m_sID);
	m_pList = NULL;
	}

void CSystemStationsMenu::HighlightObject (CSystem &System, DWORD dwObjID)

//	HighlightObject
//
//	Highlight the given object.

	{
	if (dwObjID == m_dwCurObjID)
		return;

	if (m_dwCurObjID)
		UnhighlightObject(System);

	CSpaceObject *pObj = System.FindObject(dwObjID);
	if (pObj)
		{
		pObj->SetSelection();
		m_dwCurObjID = pObj->GetID();
		}
	else
		m_dwCurObjID = 0;
	}

void CSystemStationsMenu::UnhighlightObject (CSystem &System)

//	UnhighlightObject
//
//	Unhighlight the currently highlighted object.

	{
	if (m_dwCurObjID)
		{
		CSpaceObject *pObj = System.FindObject(m_dwCurObjID);
		if (pObj)
			{
			pObj->ClearSelection();
			}

		m_dwCurObjID = 0;
		}
	}

void CSystemStationsMenu::OnAniCommand (const CString &sID, const CString &sEvent, const CString &sCmd, DWORD dwData)

//	OnAniCommand
//
//	Handle commands and notifications.

	{
	CSystem *pSystem = m_Model.GetUniverse().GetCurrentSystem();
	if (!pSystem)
		return;

	if (strEquals(sCmd, EVENT_ON_SELECTION_CHANGED))
		{
		//	The ID of the entry is the object ID.

		CString sObjID = m_pList->GetPropertyString(PROP_SELECTION_ID);
		if (sObjID.IsBlank())
			return;

		//	Highlight the selected object on the map.

		HighlightObject(*pSystem, strToInt(sObjID, 0));
		}
	}

bool CSystemStationsMenu::Show (const RECT &rcRect, const CString &sID)

//	Show
//
//	Show the menu. Returns FALSE if we could not show the menu.

	{
	if (m_pList)
		return true;

	CUniverse &Universe = m_Model.GetUniverse();
	CSystem *pSystem = Universe.GetCurrentSystem();
	if (pSystem == NULL)
		return false;

	const CTopologyNode *pNode = Universe.GetCurrentTopologyNode();
	if (!pNode)
		return false;

    //  Refresh global objects

	Universe.GetGlobalObjects().Refresh(pSystem);

	//	Create the pane.

    RECT rcPane = rcRect;
    rcPane.right = rcRect.right - DETAIL_PANE_PADDING_RIGHT;
    rcPane.left = rcPane.right - DETAIL_PANE_WIDTH;
    rcPane.top = rcRect.top + (RectHeight(rcRect) - DETAIL_PANE_HEIGHT) / 2;
    rcPane.bottom = rcPane.top + DETAIL_PANE_HEIGHT;

    CGalacticMapSystemDetails Details(m_HI.GetVisuals(), m_Session.GetReanimator(), rcPane);
	CGalacticMapSystemDetails::SOptions Options;
	Options.bIncludeStargates = true;
	Options.bNoCollapseByType = true;
	Options.pListener = this;
	Options.sOnSelectionChangedCmd = EVENT_ON_SELECTION_CHANGED;

	IAnimatron *pAni;
    if (!Details.CreateDetailsPane(*pNode, Options, &pAni, &m_pList))
		return false;

	m_sID = sID;
    m_Session.StartPerformance(pAni, m_sID, CReanimator::SPR_FLAG_DELETE_WHEN_DONE);

	//	Highlight the current object

	CString sObjID = m_pList->GetPropertyString(PROP_SELECTION_ID);
	if (!sObjID.IsBlank())
		{
		HighlightObject(*pSystem, strToInt(sObjID, 0));
		}

	return true;
	}
