//	CGIconListArea.cpp
//
//	CGIconListArea class
//	Copyright (c) 2021 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Transcendence.h"

#define FIELD_DESC							CONSTLIT("desc")
#define FIELD_ICON							CONSTLIT("icon")
#define FIELD_TITLE							CONSTLIT("title")

bool CGIconListArea::DeselectAll ()

//	DeselectAll
//
//	Deselect all entries. Returns TRUE if anything was selected.

	{
	bool bSelectionChanged = false;

	for (int i = 0; i < m_List.GetCount(); i++)
		{
		if (m_List[i].bSelected)
			{
			m_List[i].bSelected = false;
			bSelectionChanged = true;
			}
		}

	return bSelectionChanged;
	}

bool CGIconListArea::Format (const RECT &rcRect) const

//	Format
//
//	Formats the icon list.

	{
	if (m_bFormatted)
		return true;

	else if (m_List.GetCount() == 0)
		{
		m_rcFrame = rcRect;
		m_bFormatted = true;

		return true;
		}

	else
		{
		const int cxEntry = 2 * m_cxIcon;
		const int cyEntry = cxEntry;

		//	Calc how many icons we can fit per row.

		const int iEntriesPerRow = Max(1, (RectWidth(rcRect) - (2 * FRAME_PADDING_HORZ) + ENTRY_SPACING) / (cxEntry + ENTRY_SPACING));
		const int iRows = AlignUp(m_List.GetCount(), iEntriesPerRow) / iEntriesPerRow;
		const int iLastRow = iRows - 1;
		const int iLastRowCount = m_List.GetCount() % iEntriesPerRow;
		const int cyArea = (iRows * cyEntry) + (iRows - 1) * ENTRY_SPACING;
		const int yStart = rcRect.top + (RectHeight(rcRect) - cyArea) / 2;

		//	Place each icon

		for (int i = 0; i < m_List.GetCount(); i++)
			{
			const SEntry &Entry = m_List[i];
			const int iRow = i / iEntriesPerRow;
			const int iCol = i % iEntriesPerRow;
			const int iRowCount = ((iRow == iLastRow && iLastRowCount) ? iLastRowCount : iEntriesPerRow);
			const int cxRowWidth = (iRowCount * cxEntry) + (iRowCount - 1) * ENTRY_SPACING;
			const int xStart = rcRect.left + (RectWidth(rcRect) - cxRowWidth) / 2;

			Entry.rcRect.left = xStart + (iCol * cxEntry) + (iCol > 0 ? (iCol - 1) * ENTRY_SPACING : 0);
			Entry.rcRect.right = Entry.rcRect.left + cxEntry;
			Entry.rcRect.top = yStart + (iRow * cyEntry) + (iRow > 0 ? (iRow - 1) * ENTRY_SPACING : 0);
			Entry.rcRect.bottom = Entry.rcRect.top + cyEntry;
			}

		m_rcFrame = rcRect;
		m_bFormatted = true;

		return true;
		}
	}

TArray<int> CGIconListArea::GetSelection () const

//	GetSelection
//
//	Returns an array of selected entries.

	{
	TArray<int> Selection;
	
	for (int i = 0; i < m_List.GetCount(); i++)
		{
		if (m_List[i].bSelected)
			Selection.Insert(i);
		}

	return Selection;
	}

ICCItemPtr CGIconListArea::GetSelectionAsCCItem () const

//	GetSelectionAsCCItem
//
//	Returns the selection as either:
//
//	Nil: No selection
//	An entry struct: A single entry selection
//	A list of entry structs: Multiple selection.

	{
	TArray<int> Selection = GetSelection();
	if (Selection.GetCount() == 0)
		return ICCItemPtr::Nil();

	else if (Selection.GetCount() == 1)
		return m_List[0].pData;

	else
		{
		ICCItemPtr pResult(ICCItem::List);

		for (int i = 0; i < m_List.GetCount(); i++)
			{
			pResult->Append(m_List[i].pData);
			}

		return pResult;
		}
	}

int CGIconListArea::HitTestEntry (int x, int y) const

//	HitTestEntry
//
//	Returns an index into the entry at the given point. If none, we return -1.

	{
	if (!m_bFormatted)
		return -1;

	for (int i = 0; i < m_List.GetCount(); i++)
		{
		const SEntry &Entry = m_List[i];
		if (x >= Entry.rcRect.left && x < Entry.rcRect.right
				&& y >= Entry.rcRect.top && y < Entry.rcRect.bottom)
			return i;
		}

	return -1;
	}

bool CGIconListArea::InitEntry (SEntry &Entry, const ICCItem &Data, CString *retsError)

//	InitEntry
//
//	Initializes the given entry from a structure.
//	NOTE: This should only initialize data element; it must not change the
//	selection because we sometimes call this to refresh data.

	{
	Entry.pData = ICCItemPtr(Data.Reference());

	Entry.sTitle = Data.GetStringAt(FIELD_TITLE);

	const ICCItem *pIcon = Data.GetElement(FIELD_ICON);
	if (pIcon)
		{
		DWORD dwIcon = CTLispConvert::AsImageDesc(pIcon, &Entry.rcIconSrc);
		if (dwIcon)
			Entry.pIcon = m_Universe.GetLibraryBitmap(dwIcon);
		}

	return true;
	}

void CGIconListArea::InitFromTheme (const CVisualPalette &VI, const CDockScreenVisuals &Theme)

//	InitFromTheme
//
//	Initializes colors, etc.

	{
	m_rgbText = Theme.GetTitleTextColor();
	m_rgbBack = Theme.GetTextBackgroundColor();
	m_iBorderRadius = Theme.GetBorderRadius();
	}

bool CGIconListArea::LButtonDown (int x, int y)

//	LButtonDown
//
//	Handle mouse button

	{
	//	Internally we use paint coordinates.

	GetParent()->ConvertToPaintCoords(x, y);

	//	See if we hit any entry.

	int iEntry = HitTestEntry(x, y);
	if (iEntry != -1)
		{
		if (!uiIsControlDown())
			DeselectAll();

		m_List[iEntry].bSelected = !m_List[iEntry].bSelected;
		SignalAction(NOTIFY_SELECTION_CHANGED);
		return true;
		}
	else
		{
		//	If we didn't hit an entry, then we clear the selection.

		if (!uiIsControlDown())
			{
			if (DeselectAll())
				{
				SignalAction(NOTIFY_SELECTION_CHANGED);
				return true;
				}
			}
		}

	return false;
	}

void CGIconListArea::Paint (CG32bitImage &Dest, const RECT &rcRect)

//	Paint
//
//	Paint the list.

	{
	if (!Format(rcRect))
		return;

	//	Paint background

	PaintBackground(Dest, rcRect);

	//	Paint each entry.

	for (int i = 0; i < m_List.GetCount(); i++)
		{
		PaintEntry(Dest, m_List[i]);
		}
	}

void CGIconListArea::PaintBackground (CG32bitImage &Dest, const RECT &rcRect) const

//	PaintBackground
//
//	Paints the background frame.

	{
	CG32bitPixel rgbFadeBackColor = CG32bitPixel(CG32bitPixel::Darken(m_rgbBack, 220), 220);
	CGDraw::RoundedRect(Dest,
			rcRect.left,
			rcRect.top + m_cyTabRegion,
			RectWidth(rcRect),
			RectHeight(rcRect) - m_cyTabRegion,
			m_iBorderRadius + 1,
			rgbFadeBackColor);
	}

void CGIconListArea::PaintEntry (CG32bitImage &Dest, const SEntry &Entry) const

//	PaintEntry
//
//	Paints the entry.

	{
	const int iCornerRadius = 4;
	const int xBackMargin = 3;
	const int yBackMargin = 3;
	const int iSelectionWidth = 2;
	CG32bitPixel rgbFadeBackColor = CG32bitPixel(CG32bitPixel::Darken(m_rgbBack, 220), 220);

	RECT rcRegion = Entry.rcRect;

	//	Paint the background of the entry

	CG32bitPixel rgbBackColor = (Entry.bSelected ? m_rgbBack : rgbFadeBackColor);

	CGDraw::RoundedRect(Dest, 
			rcRegion.left - xBackMargin, 
			rcRegion.top - yBackMargin, 
			RectWidth(rcRegion) + 2 * xBackMargin,
			RectHeight(rcRegion) + 2 * yBackMargin,
			iCornerRadius + xBackMargin, 
			rgbBackColor);

	//	Paint the actual region

	int xIcon = rcRegion.left + (RectWidth(rcRegion) - m_cxIcon) / 2;
	int yIcon = rcRegion.top + ENTRY_PADDING_TOP;
	if (Entry.pIcon)
		{
		CGDraw::BltTransformed(Dest,
				xIcon + (m_cxIcon / 2),
				yIcon + (m_cyIcon / 2),
				(Metric)m_cxIcon / (Metric)RectWidth(Entry.rcIconSrc),
				(Metric)m_cyIcon / (Metric)RectHeight(Entry.rcIconSrc),
				0.0,
				*Entry.pIcon,
				Entry.rcIconSrc.left,
				Entry.rcIconSrc.top,
				RectWidth(Entry.rcIconSrc),
				RectHeight(Entry.rcIconSrc));
		}

	//	Paint the name

	RECT rcText;
	rcText.left = rcRegion.left + ENTRY_PADDING_LEFT;
	rcText.right = rcRegion.right - ENTRY_PADDING_RIGHT;
	rcText.top = yIcon + m_cyIcon;
	rcText.bottom = rcRegion.bottom;

	m_VI.GetFont(fontMedium).DrawText(Dest, 
			rcText,
			m_rgbText,
			Entry.sTitle,
			0,
			CG16bitFont::AlignCenter);

	//	Draw outline if we're selected

	if (Entry.bSelected)
		{
		CGDraw::RoundedRectOutline(Dest,
				rcRegion.left,
				rcRegion.top,
				RectWidth(rcRegion),
				RectHeight(rcRegion),
				iCornerRadius,
				iSelectionWidth,
				m_VI.GetColor(colorAreaDialogHighlight));
		}
	else
		{
		CGDraw::RoundedRectOutline(Dest,
				rcRegion.left,
				rcRegion.top,
				RectWidth(rcRegion),
				RectHeight(rcRegion),
				iCornerRadius,
				1,
				m_VI.GetColor(colorTextFade));
		}
	}

bool CGIconListArea::SetData (const ICCItem &List, CString *retsError)

//	SetData
//
//	We expect a list of structs. For each struct we interpret the following fields:
//
//	title: The label for the entry.
//	icon: An icon descriptor.

	{
	if (List.IsError())
		{
		if (retsError)
			*retsError = List.GetStringValue();
		return false;
		}
	else if (!List.IsList())
		{
		if (retsError)
			*retsError = CONSTLIT("List expected.");
		return false;
		}
	else if (List.GetCount() != 0 && !List.GetElement(0)->IsSymbolTable())
		{
		if (retsError)
			*retsError = CONSTLIT("Expected list of entries.");
		return false;
		}
	else if (List.GetCount() == m_List.GetCount())
		{
		//	If we have the same number of icons, we keep the format and just
		//	change the data, if necessary. We do this because we call SetData
		//	when changing panes and we don't necessarily want to reset the
		//	selection.

		for (int i = 0; i < List.GetCount(); i++)
			{
			CString sError;
			if (!InitEntry(m_List[i], *List.GetElement(i), &sError))
				{
				m_List[i].sTitle = sError;
				}
			}

		Invalidate();
		return true;
		}
	else
		{
		m_List.DeleteAll();

		for (int i = 0; i < List.GetCount(); i++)
			{
			SEntry *pEntry = m_List.Insert();

			CString sError;
			if (!InitEntry(*pEntry, *List.GetElement(i), &sError))
				{
				pEntry->sTitle = sError;
				}
			}

		Reformat();
		return true;
		}
	}
