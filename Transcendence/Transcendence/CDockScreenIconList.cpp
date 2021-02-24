//	CDockScreenIconList.cpp
//
//	CDockScreenIconList class
//	Copyright (c) 2021 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Transcendence.h"

bool CDockScreenIconList::EvalList (CString *retsError)

//	EvalList
//
//	Evalues the current code and sets the list.

	{
	if (!m_pExp)
		throw CException(ERR_FAIL);

	//	Evaluate the function

	CCodeChainCtx CCX(GetUniverse());
	CCX.DefineContainingType(m_DockScreen.GetRoot());
	CCX.SaveAndDefineSourceVar(m_pLocation);
	CCX.SaveAndDefineDataVar(m_pData);

	ICCItemPtr pResult = CCX.RunCode(m_pExp);	//	LATER:Event
	if (pResult->IsError())
		{
		if (retsError) *retsError = pResult->GetStringValue();
		return false;
		}

	//	We expect the result to be a list of entries.

	return m_pControl->SetData(*pResult, retsError);
	}

ICCItem *CDockScreenIconList::OnGetCurrentListEntry (void) const

//	OnGetCurrentListEntry
//
//	Returns the current list entry.

	{
	return m_pControl->GetSelectionAsCCItem()->Reference();
	}

bool CDockScreenIconList::OnGetDefaultBackground (SDockScreenBackgroundDesc *retDesc)

//	OnGetDefaultBackground

	{
	if (m_pControl == NULL || m_pControl->GetCount() == 0)
		return false;

	//	No background

	retDesc->iType = EDockScreenBackground::none;
	return true;
	}

ICCItemPtr CDockScreenIconList::OnGetListAsCCItem (void) const

//	OnGetListAsCCItem
//
//	Returns the entire list.

	{
	if (m_pControl)
		return m_pControl->GetList();
	else
		return ICCItemPtr::Nil();
	}

IDockScreenDisplay::EResults CDockScreenIconList::OnHandleAction (DWORD dwTag, DWORD dwData)

//	OnHandleAction
//
//	Handle an action from our control

	{
	if (dwTag == m_dwID)
		{
		if (dwData == CGIconListArea::NOTIFY_SELECTION_CHANGED)
			{
			g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_SELECT));
			return resultShowPane;
			}
		else
			return resultHandled;
		}
	else
		return resultNone;
	}

ALERROR CDockScreenIconList::OnInit (SInitCtx &Ctx, const SDisplayOptions &Options, CString *retsError)

//	OnInit
//
//	Initialize

	{
	DEBUG_TRY

	const CDockScreenVisuals &DockScreenVisuals = Ctx.pDockScreen->GetDockScreenVisuals();

	m_dwID = Ctx.dwFirstID;

	//	Calculate some basic metrics

	RECT rcList = Ctx.rcRect;
	rcList.left += Options.rcDisplay.left;
	rcList.right = rcList.left + RectWidth(Options.rcDisplay);
	rcList.top += Options.rcDisplay.top;
	rcList.bottom = rcList.top + RectHeight(Options.rcDisplay);

	//	Create the picker control

	m_pControl = new CGIconListArea(GetUniverse(), g_pHI->GetVisuals(), DockScreenVisuals);
	if (m_pControl == NULL)
		{
		*retsError = CONSTLIT("Out of memory.");
		return ERR_MEMORY;
		}

	m_pControl->SetTabRegion(Options.cyTabRegion);

	//	Create. NOTE: Once we add it to the screen, it takes ownership of it. 
	//	We do not have to free it.

	Ctx.pScreen->AddArea(m_pControl, rcList, m_dwID);

	//	Get the list to show

	m_pExp = CCodeChain::LinkCode(Options.sCode);

	//	The display can optional specify the data source. This helps us to 
	//	refresh appropriately when items or something changes. It is OK if this
	//	is NULL.

	m_pSource = EvalListSource(Options.sDataFrom, retsError);

	//	Evaluate the function

	if (!EvalList(retsError))
		return ERR_FAIL;

	//	Set the selection, if saved

	if (Ctx.pSelection)
		m_pControl->RestoreSelection(*Ctx.pSelection);

	return NOERROR;

	DEBUG_CATCH
	}

IDockScreenDisplay::EResults CDockScreenIconList::OnResetList (CSpaceObject *pLocation)

//	OnResetList
//
//	Refresh the list because it has changed.

	{
	if (pLocation == m_pSource)
		{
		ShowItem();
		return resultShowPane;
		}
	else
		return resultNone;
	}

void CDockScreenIconList::OnShowItem (void)

//	OnShowItem
//
//	Show the current item

	{
	CString sError;
	if (!EvalList(&sError))
		{
		GetUniverse().LogOutput(strPatternSubst(CONSTLIT("CDockScreenDetailsPane: %s"), sError));
		}
	}

void CDockScreenIconList::OnShowPane (bool bNoListNavigation)

//	OnShowPane
//
//	Handle case where the pane is shown

	{
	ShowItem();
	}
