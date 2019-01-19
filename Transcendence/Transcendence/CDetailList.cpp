//	CDetailList.cpp
//
//	CDetailList class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Transcendence.h"

#define FIELD_DESC							CONSTLIT("desc")
#define FIELD_DETAILS						CONSTLIT("details")
#define FIELD_ICON							CONSTLIT("icon")
#define FIELD_LARGE_ICON					CONSTLIT("largeIcon")
#define FIELD_TITLE							CONSTLIT("title")

void CDetailList::CalcColumnRect (const RECT &rcArea, int xCol, int cxCol, int cyCol, DWORD dwFlags, RECT &retCol) const

//	CalcColumnRect
//
//	Computes the rect for a column based on alignment flags.

	{
	retCol.left = xCol;
	retCol.right = xCol + cxCol;

	if (dwFlags & FORMAT_ALIGN_BOTTOM)
		{
		retCol.bottom = rcArea.bottom;
		retCol.top = retCol.bottom - cyCol;
		}
	else if (dwFlags & FORMAT_ALIGN_CENTER)
		{
		retCol.top = rcArea.top + (RectHeight(rcArea) - cyCol) / 2;
		retCol.bottom = retCol.top + cyCol;
		}
	else
		{
		retCol.top = rcArea.top;
		retCol.bottom = retCol.top + cyCol;
		}
	}

void CDetailList::CalcColumnRects (const RECT &rcRect, int cxCol, int cyCol1, int cyCol2, DWORD dwFlags, RECT &retCol1, RECT &retCol2) const

//	CalcColumnRects
//
//	Computes the rect for the two columns based on the content height and the
//	format flags.

	{
	//	First figure out the area where the columns will go relative to the full
	//	rect.

	int cyArea = Max(cyCol1, cyCol2);
	RECT rcArea;
	rcArea.left = rcRect.left;
	rcArea.right = rcRect.right;

	if (dwFlags & FORMAT_PLACE_BOTTOM)
		{
		rcArea.bottom = rcRect.bottom;
		rcArea.top = rcRect.bottom - cyArea;
		}
	else if (dwFlags & FORMAT_PLACE_CENTER)
		{
		rcArea.top = rcRect.top + (RectHeight(rcRect) - cyArea) / 2;
		rcArea.bottom = rcArea.top + cyArea;
		}
	else
		{
		rcArea.top = rcRect.top;
		rcArea.bottom = rcArea.top + cyArea;
		}

	//	Now calculate the two columns

	CalcColumnRect(rcArea, rcArea.left + SPACING_X, cxCol, cyCol1, dwFlags, retCol1);
	CalcColumnRect(rcArea, rcArea.right - SPACING_X - cxCol, cxCol, cyCol2, dwFlags, retCol2);
	}

void CDetailList::Format (const RECT &rcRect, DWORD dwFlags, int *retcyHeight)

//	Format
//
//	Initializes rcRect for each detail entry based on the overall rect for the
//	content pane.

	{
	int i;
	const CG16bitFont &Medium = m_VI.GetFont(fontMedium);
	const CG16bitFont &MediumBold = m_VI.GetFont(fontMediumBold);

	//	Short-circuit

	if (m_List.GetCount() == 0)
		{
		if (retcyHeight) *retcyHeight = 0;
		return;
		}

	//	We arrange the details along two columns, aligned to the bottom.

	int cxPane = RectWidth(rcRect);
	int cxColumn = (cxPane - (3 * SPACING_X)) / 2;

	RECT rcColumn;
	rcColumn.left = 0;
	rcColumn.right = cxColumn;
	rcColumn.top = 0;
	rcColumn.bottom = RectHeight(rcRect);

	//	The text area is smaller because of the icon.

	RECT rcText = rcColumn;
	rcText.right -= SPACING_X + DETAIL_ICON_WIDTH;

	//	First we measure the height of each detail entry.

	for (i = 0; i < m_List.GetCount(); i++)
		{
		int cyDetails = (!m_List[i].sTitle.IsBlank() ? MediumBold.GetHeight() : 0);

		//	Compute height of description

		RECT rcBounds;
		m_List[i].Desc.GetBounds(rcText, &rcBounds);
		cyDetails += RectHeight(rcBounds);

		//	Remember the text height separately.

		m_List[i].cyText = cyDetails;

		//	If the icon is larger, then we use that height.

		m_List[i].cyRect = Max(cyDetails, DETAIL_ICON_HEIGHT);
		}

	//	Figure out where we break the columns. iBreakAt is the index of the 
	//	first detail entry in column 2.

	int iBreakAt = (m_List.GetCount() + 1) / 2;

	//	Add up the total height of each column. NOTE: For this calculation we
	//	ignore any spacing between entries.

	int cyCol1 = 0;
	for (i = 0; i < iBreakAt; i++)
		cyCol1 += m_List[i].cyRect;

	int cyCol2 = 0;
	for (i = iBreakAt; i < m_List.GetCount(); i++)
		cyCol2 += m_List[i].cyRect;

	int cyMax = Max(cyCol1, cyCol2);

	//	See if we can fit better if we move the first entry from column 2 to
	//	column 1.

	if (m_List.GetCount() > 2)
		{
		while (iBreakAt < m_List.GetCount() - 1
				&& Max(cyCol1 + m_List[iBreakAt].cyRect, cyCol2 - m_List[iBreakAt].cyRect) < cyMax)
			{
			cyCol1 += m_List[iBreakAt].cyRect;
			cyCol2 -= m_List[iBreakAt].cyRect;
			cyMax = Max(cyCol1, cyCol2);
			iBreakAt++;
			}

		//	See if we can fit better if we move the last entry from column 1 to
		//	column 2.

		while (iBreakAt > 0
				&& Max(cyCol1 - m_List[iBreakAt - 1].cyRect, cyCol2 + m_List[iBreakAt - 1].cyRect) < cyMax)
			{
			cyCol1 -= m_List[iBreakAt - 1].cyRect;
			cyCol2 += m_List[iBreakAt - 1].cyRect;
			cyMax = Max(cyCol1, cyCol2);
			iBreakAt--;
			}
		}

	//	Now compute column metrics including spacing.

	cyCol1 += (iBreakAt * SPACING_Y) + SPACING_Y;
	cyCol2 += ((m_List.GetCount() - iBreakAt) * SPACING_Y) + SPACING_Y;
	cyMax = Max(cyCol1, cyCol2);

	RECT rcCol1;
	RECT rcCol2;
	CalcColumnRects(rcRect, cxColumn, cyCol1, cyCol2, dwFlags, rcCol1, rcCol2);

	//	Now format all entries. Start with the first column.

	FormatColumn(0, iBreakAt, rcCol1, dwFlags | FORMAT_LEFT_COLUMN);
	FormatColumn(iBreakAt, m_List.GetCount(), rcCol2, dwFlags | FORMAT_RIGHT_COLUMN);

	//	Return the max column height.

	if (retcyHeight)
		*retcyHeight = cyMax;
	}

void CDetailList::FormatColumn (int iStart, int iEnd, const RECT &rcRect, DWORD dwFlags)

//	FormatColumn
//
//	Formats the given column

	{
	bool bAlignRight;
	AlignmentStyles iTextAlign;

	//	Left/right alignment depends on flags

	if (dwFlags & FORMAT_ANTI_MIRROR_COLUMNS)
		{
		bAlignRight = ((dwFlags & FORMAT_RIGHT_COLUMN) != 0);
		iTextAlign = (bAlignRight ? alignRight : alignLeft);
		}
	else if (dwFlags & FORMAT_MIRROR_COLUMNS)
		{
		bAlignRight = ((dwFlags & FORMAT_RIGHT_COLUMN) == 0);
		iTextAlign = (bAlignRight ? alignRight : alignLeft);
		}
	else
		{
		bAlignRight = false;
		iTextAlign = alignLeft;
		}

	//	Format

	int y = rcRect.top;
	for (int i = iStart; i < iEnd; i++)
		{
		m_List[i].bAlignRight = bAlignRight;
		m_List[i].Desc.SetAlignment(iTextAlign);

		m_List[i].rcRect.left = rcRect.left;
		m_List[i].rcRect.right = rcRect.right;
		m_List[i].rcRect.top = y;
		m_List[i].rcRect.bottom = y + m_List[i].cyRect;

		y += m_List[i].cyRect + SPACING_Y;
		}
	}

void CDetailList::Load (ICCItem *pDetails)

//	Load
//
//	Load details from an TLisp item to an array.

	{
	int i;

	m_List.DeleteAll();

	if (pDetails == NULL || !pDetails->IsList())
		return;

	for (i = 0; i < pDetails->GetCount(); i++)
		{
		ICCItem *pEntry = pDetails->GetElement(i);
		if (!pEntry->IsSymbolTable()
				|| pEntry->IsNil())
			continue;

		SDetailEntry *pNewEntry = m_List.Insert();
		pNewEntry->sTitle = pEntry->GetStringAt(FIELD_TITLE);

		//	Load description, which could be RTF

		pNewEntry->Desc.SetFontTable(&m_VI);
		pNewEntry->Desc.SetDefaultFont(&m_VI.GetFont(fontMedium));
		pNewEntry->Desc.SetText(pEntry->GetStringAt(FIELD_DESC));

		//	Load image

		ICCItem *pIcon = pEntry->GetElement(FIELD_ICON);
		if (pIcon)
			{
			DWORD dwIcon = CTLispConvert::AsImageDesc(pIcon, &pNewEntry->rcIcon);
			if (dwIcon)
				pNewEntry->pIcon = g_pUniverse->GetLibraryBitmap(dwIcon); 
			}
		}
	}

void CDetailList::Paint (CG32bitImage &Dest) const

//	PaintDetails
//
//	Paint all detail entries. We must have already called FormatDetails.

	{
	int i;
	const CG16bitFont &Medium = m_VI.GetFont(fontMedium);
	const CG16bitFont &MediumBold = m_VI.GetFont(fontMediumBold);

	for (i = 0; i < m_List.GetCount(); i++)
		{
		const SDetailEntry &Entry = m_List[i];

		//	Paint the icon

		if (Entry.pIcon)
			{
			if (Entry.bAlignRight)
				CPaintHelper::PaintScaledImage(Dest, Entry.rcRect.right - DETAIL_ICON_WIDTH, Entry.rcRect.top, DETAIL_ICON_WIDTH, DETAIL_ICON_HEIGHT, *Entry.pIcon, Entry.rcIcon);
			else
				CPaintHelper::PaintScaledImage(Dest, Entry.rcRect.left, Entry.rcRect.top, DETAIL_ICON_WIDTH, DETAIL_ICON_HEIGHT, *Entry.pIcon, Entry.rcIcon);
			}

		//	Paint text

		int yText = Entry.rcRect.top + (Entry.cyRect - Entry.cyText) / 2;
		DWORD dwTextFlags = (Entry.bAlignRight ? CG16bitFont::AlignRight : 0);

		//	Paint the title

		if (!Entry.sTitle.IsBlank())
			{
			int xText = (Entry.bAlignRight ? Entry.rcRect.right - DETAIL_ICON_WIDTH - SPACING_X : Entry.rcRect.left + DETAIL_ICON_WIDTH + SPACING_X);
			MediumBold.DrawText(Dest, xText, yText, m_rgbTextColor, Entry.sTitle, dwTextFlags);
			yText += MediumBold.GetHeight();
			}

		//	Paint the description

		RECT rcDesc;
		if (Entry.bAlignRight)
			{
			rcDesc.left = Entry.rcRect.left;
			rcDesc.right = Entry.rcRect.right - DETAIL_ICON_WIDTH - SPACING_X;
			}
		else
			{
			rcDesc.left = Entry.rcRect.left + DETAIL_ICON_WIDTH + SPACING_X;
			rcDesc.right = Entry.rcRect.right;
			}
		rcDesc.top = yText;
		rcDesc.bottom = Entry.rcRect.bottom;

		Entry.Desc.Paint(Dest, rcDesc);
		}
	}
