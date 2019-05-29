//	CGTabArea.cpp
//
//	CGTabArea class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Transcendence.h"

void CGTabArea::AddTab (const CString &sID, const CString &sLabel)

//	AddTab
//
//	Adds a tab to the control.

	{
	const CG16bitFont &MediumFont = m_VI.GetFont(fontMedium);

	STabDesc *pNewTab = m_Tabs.Insert();
	pNewTab->sID = sID;
	pNewTab->sLabel = sLabel;
	pNewTab->cxWidth = MediumFont.MeasureText(sLabel) + 2 * TAB_PADDING_X;

	if (m_iCurTab == -1)
		m_iCurTab = 0;
	}

bool CGTabArea::FindTab (const CString &sID, int *retiIndex) const

//	FindTab
//
//	Finds the tab by ID

	{
	for (int i = 0; i < m_Tabs.GetCount(); i++)
		if (strEquals(m_Tabs[i].sID, sID))
			{
			if (retiIndex)
				*retiIndex = i;
			return true;
			}

	return false;
	}

const CString &CGTabArea::GetNextTabID (void) const

//	GetNextTabID
//
//	Returns the next tab ID.

	{
	if (m_Tabs.GetCount() == 0)
		return NULL_STR;

	int iNextTab = (m_iCurTab + 1) % m_Tabs.GetCount();
	return m_Tabs[iNextTab].sID;
	}

const CString &CGTabArea::GetPrevTabID (void) const

//	GetPrevTabID
//
//	Returns the previous tab ID.

	{
	if (m_Tabs.GetCount() == 0)
		return NULL_STR;

	int iPrevTab = (m_iCurTab + m_Tabs.GetCount() - 1) % m_Tabs.GetCount();
	return m_Tabs[iPrevTab].sID;
	}

bool CGTabArea::HitTest (int x, int y, int *retiTab) const

//	HitTest
//
//	See if we hit a tab.

	{
	int i;
	RECT rcRect = GetRect();

	if (y >= rcRect.top && y < rcRect.top + m_cyTabHeight)
		{
		int xTab = rcRect.left;

		for (i = 0; i < m_Tabs.GetCount(); i++)
			{
			if (x >= xTab && x < xTab + m_Tabs[i].cxWidth)
				{
				if (retiTab)
					*retiTab = i;

				return true;
				}

			xTab += m_Tabs[i].cxWidth;
			}
		}

	//	If we get this far, not found.

	return false;
	}

bool CGTabArea::LButtonDown (int x, int y)

//	LButtonDown
//
//	Handle button down

	{
	//	Hit test the tab area

	int iTab;
	if (!HitTest(x, y, &iTab))
		return false;

	//	If the tab is disabled, then we don't need to do anything (but we return
	//	TRUE to indicate that we handled the click).

	if (m_Tabs[iTab].bDisabled)
		return true;

	//	Nothing to do if we haven't changed tabs

	if (iTab == m_iCurTab)
		return true;

	//	Set the current tab and signal an action.
	//
	//	NOTE: We cannot do anything after signalling an action because we might
	//	have been destroyed.

	m_iCurTab = iTab;
	SignalAction(iTab);

	return true;
	}

void CGTabArea::MouseLeave (void)

//	MouseLeave
//
//	Handle hover

	{
	if (m_iHoverTab != -1)
		{
		m_iHoverTab = -1;
		Invalidate();
		}
	}

void CGTabArea::MouseMove (int x, int y)

//	MouseMove
//
//	Handle hover

	{
	int iTab;
	if (!HitTest(x, y, &iTab))
		iTab = -1;

	if (iTab != m_iHoverTab)
		{
		m_iHoverTab = iTab;
		Invalidate();
		}
	}

void CGTabArea::Paint (CG32bitImage &Dest, const RECT &rcRect)

//	Paint
//
//	Paint the tabs

	{
	if (m_Tabs.GetCount() == 0)
		return;

	int x = rcRect.left + 1;
	int y = rcRect.top + 1;

	//	Paint the background of the tab area

	CG32bitPixel rgbBack = m_rgbBackColor;
	CGDraw::RoundedRect(Dest, rcRect.left + 1, rcRect.top + 1, RectWidth(rcRect) - 2, m_cyTabHeight - 2, BORDER_RADIUS - 1, rgbBack);

	Dest.FillLine(rcRect.left, rcRect.top + m_cyTabHeight - 1, RectWidth(rcRect), CG32bitPixel(80, 80, 80));

	//	Paint the tabs

	for (int i = 0; i < m_Tabs.GetCount(); i++)
		{
		RECT rcTab;
		rcTab.left = x;
		rcTab.right = rcTab.left + m_Tabs[i].cxWidth;
		rcTab.top = y;
		rcTab.bottom = y + m_cyTabHeight - 2;

		PaintTab(Dest, m_Tabs[i], rcTab, (i == m_iCurTab), (i == m_iHoverTab));

		x += m_Tabs[i].cxWidth;
		}
	}

void CGTabArea::PaintTab (CG32bitImage &Dest, const STabDesc &Tab, const RECT &rcRect, bool bSelected, bool bHover) const

//	PaintTab
//
//	Paints a tab.

	{
	const CG16bitFont &MediumFont = m_VI.GetFont(fontMedium);

	CG32bitPixel rgbBackColor;
	CG32bitPixel rgbTextColor;
	if (Tab.bDisabled)
		rgbTextColor = CG32bitPixel(m_rgbTextColor, 64);

	else if (bSelected)
		{
		rgbBackColor = CG32bitPixel(m_rgbTextColor, 128);
		rgbTextColor = m_rgbBackColor;
		}
	else if (bHover)
		{
		rgbBackColor = CG32bitPixel(m_rgbTextColor, 30);
		rgbTextColor = CG32bitPixel(m_rgbTextColor, 190);
		}
	else
		rgbTextColor = CG32bitPixel(m_rgbTextColor, 190);

	if (!Tab.bDisabled && (bSelected || bHover))
		CGDraw::RoundedRect(Dest,
				rcRect.left,
				rcRect.top,
				RectWidth(rcRect),
				RectHeight(rcRect),
				BORDER_RADIUS - 1,
				rgbBackColor);

	MediumFont.DrawText(Dest, rcRect, rgbTextColor, Tab.sLabel, 0, CG16bitFont::AlignCenter | CG16bitFont::AlignMiddle);
	}

void CGTabArea::SetCurTab (const CString &sID)

//	SetCurTab
//
//	Sets the current tab, but does not signal an action (used to initialize the
//	tab area).

	{
	int iIndex;
	if (!FindTab(sID, &iIndex))
		return;

	m_iCurTab = iIndex;
	Invalidate();
	}
