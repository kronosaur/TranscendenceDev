//	CDockScreenDetailsPane.cpp
//
//	CDockScreenDetailsPane class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Transcendence.h"

bool CDockScreenDetailsPane::EvalValue (CString *retsError)

//	EvalValue
//
//	Evalues the details pane value.

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

	//	Set this expression as the list

	m_pControl->SetData(pResult);

	return true;
	}

ICCItem *CDockScreenDetailsPane::OnGetCurrentListEntry (void) const

//	OnGetCurrentListEntry
//
//	Returns the current list entry.

	{
	return m_pControl->GetData();
	}

bool CDockScreenDetailsPane::OnGetDefaultBackground (SDockScreenBackgroundDesc *retDesc)

//	OnGetDefaultBackground

	{
	if (m_pControl == NULL || m_pControl->GetDetail().IsEmpty())
		return false;

	//	Different default background based on style.

	switch (m_pControl->GetDetail().GetStyle())
		{
		//	Stats use the default background (because they are small enough for 
		//	the background to show through). We need this for things like mining
		//	rank stats.

		case CDetailArea::styleStats:
			return false;

		//	No background

		default:
			retDesc->iType = EDockScreenBackground::none;
			return true; 
		}
	}

ALERROR CDockScreenDetailsPane::OnInit (SInitCtx &Ctx, const SDisplayOptions &Options, CString *retsError)

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

	m_pControl = new CGDetailsArea(g_pHI->GetVisuals(), DockScreenVisuals);
	if (m_pControl == NULL)
		{
		*retsError = CONSTLIT("Out of memory.");
		return ERR_MEMORY;
		}

    m_pControl->SetColor(DockScreenVisuals.GetTitleTextColor());
    m_pControl->SetBackColor(DockScreenVisuals.GetTextBackgroundColor());
	m_pControl->SetTabRegion(Options.cyTabRegion);

	//	Create. NOTE: Once we add it to the screen, it takes ownership of it. 
	//	We do not have to free it.

	Ctx.pScreen->AddArea(m_pControl, rcList, m_dwID);

	//	Get the list to show

	m_pExp = CCodeChain::LinkCode(Options.sCode);

	//	Evaluate the function

	if (!EvalValue(retsError))
		return ERR_FAIL;

	return NOERROR;

	DEBUG_CATCH
	}

void CDockScreenDetailsPane::OnShowItem (void)

//	OnShowItem
//
//	Show the current item

	{
	CString sError;
	if (!EvalValue(&sError))
		{
		GetUniverse().LogOutput(strPatternSubst(CONSTLIT("CDockScreenDetailsPane: %s"), sError));
		}
	}

void CDockScreenDetailsPane::OnShowPane (bool bNoListNavigation)

//	OnShowPane
//
//	Handle case where the pane is shown

	{
	ShowItem();
	}
