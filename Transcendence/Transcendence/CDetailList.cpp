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

void CDetailList::AlignRect (const RECT &rcFrame, int cxWidth, int cyHeight, DWORD dwAlign, RECT &rcResult)

//	AlignRect
//
//	Aligns a rectangle of the given width and height.

	{
	if (dwAlign & alignRight)
		{
		rcResult.right = rcFrame.right;
		rcResult.left = rcResult.right - cxWidth;
		}
	else if (dwAlign & alignCenter)
		{
		rcResult.left = rcFrame.left + (RectWidth(rcFrame) - cxWidth) / 2;
		rcResult.right = rcResult.left + cxWidth;
		}
	else
		{
		rcResult.left = rcFrame.left;
		rcResult.right = rcResult.left + cxWidth;
		}

	if (dwAlign & alignBottom)
		{
		rcResult.bottom = rcFrame.bottom;
		rcResult.top = rcResult.bottom - cyHeight;
		}
	else if (dwAlign & alignMiddle)
		{
		rcResult.top = rcFrame.top + (RectHeight(rcFrame) - cyHeight) / 2;
		rcResult.bottom = rcResult.top + cyHeight;
		}
	else
		{
		rcResult.top = rcFrame.top;
		rcResult.bottom = rcResult.top + cyHeight;
		}
	}

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

void CDetailList::CalcColumnRects (int cxWidth, int cyHeight, int cxCol, int cyCol1, int cyCol2, DWORD dwFlags, RECT &retCol1, RECT &retCol2) const

//	CalcColumnRects
//
//	Computes the rect for the two columns based on the content height and the
//	format flags.

	{
	//	First figure out the area where the columns will go relative to the full
	//	rect.

	int cyArea = Max(cyCol1, cyCol2);
	RECT rcArea;
	rcArea.left = 0;
	rcArea.right = cxWidth;

	cyHeight = Max(cyHeight, cyArea);

	if (dwFlags & FORMAT_PLACE_BOTTOM)
		{
		rcArea.bottom = cyHeight;
		rcArea.top = cyHeight - cyArea;
		}
	else if (dwFlags & FORMAT_PLACE_CENTER)
		{
		rcArea.top = (cyHeight - cyArea) / 2;
		rcArea.bottom = rcArea.top + cyArea;
		}
	else
		{
		rcArea.top = 0;
		rcArea.bottom = cyArea;
		}

	//	Now calculate the two columns

	CalcColumnRect(rcArea, rcArea.left + SPACING_X, cxCol, cyCol1, dwFlags, retCol1);
	CalcColumnRect(rcArea, rcArea.right - SPACING_X - cxCol, cxCol, cyCol2, dwFlags, retCol2);
	}

void CDetailList::Format (int cxWidth, int cyHeight, DWORD dwFlags, int *retcyHeight)

//	Format
//
//	Initializes rcRect for each detail entry based on the overall rect for the
//	content pane.

	{
	//	Short-circuit

	if (m_List.GetCount() == 0)
		{
		if (retcyHeight)
			*retcyHeight = 0;
		}

	//	Single column

	else if (dwFlags & FORMAT_SINGLE_COLUMN)
		{
		FormatSingleColumn(cxWidth, cyHeight, dwFlags, retcyHeight);
		}

	//	Two columns

	else
		{
		FormatDoubleColumns(cxWidth, cyHeight, dwFlags, retcyHeight);
		}
	}

void CDetailList::FormatColumn (int iStart, int iEnd, const RECT &rcRect, DWORD dwFlags)

//	FormatColumn
//
//	Formats the given column

	{
	const CG16bitFont &MediumBold = m_VI.GetFont(fontMediumBold);

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

		//	Position the icon

		if (bAlignRight)
			AlignRect(m_List[i].rcRect, DETAIL_ICON_WIDTH, DETAIL_ICON_HEIGHT, alignRight | alignTop, m_List[i].rcIcon);
		else
			AlignRect(m_List[i].rcRect, DETAIL_ICON_WIDTH, DETAIL_ICON_HEIGHT, alignLeft | alignTop, m_List[i].rcIcon);

		//	The title and description go in a box, vertically centered.

		RECT rcText;
		int cxText = RectWidth(m_List[i].rcRect) - DETAIL_ICON_WIDTH - SPACING_X;
		if (bAlignRight)
			AlignRect(m_List[i].rcRect, cxText, m_List[i].cyText, alignLeft | alignMiddle, rcText);
		else
			AlignRect(m_List[i].rcRect, cxText, m_List[i].cyText, alignRight | alignMiddle, rcText);

		//	Position the title and RTF description

		if (!m_List[i].sTitle.IsBlank())
			{
			m_List[i].rcTitle = rcText;
			m_List[i].rcTitle.bottom = rcText.top + m_List[i].cyTitle;

			m_List[i].rcDesc = rcText;
			m_List[i].rcDesc.top = m_List[i].rcTitle.bottom;
			}
		else
			{
			m_List[i].rcDesc = rcText;
			}

		y += m_List[i].cyRect + SPACING_Y;
		}
	}

void CDetailList::FormatDoubleColumns (int cxWidth, int cyHeight, DWORD dwFlags, int *retcyHeight)

//	FormatDoubleColumns
//
//	Formats two columns side by side.

	{
	const CG16bitFont &Medium = m_VI.GetFont(fontMedium);
	const CG16bitFont &MediumBold = m_VI.GetFont(fontMediumBold);

	//	We arrange the details along two columns, aligned to the bottom.

	int cxPane = cxWidth;
	int cxColumn = (cxPane - (3 * SPACING_X)) / 2;

	//	The text area is smaller because of the icon.

	int cxText = cxWidth - SPACING_X - DETAIL_ICON_WIDTH;

	//	First we measure the height of each detail entry.

	for (int i = 0; i < m_List.GetCount(); i++)
		{
		int cxTitle = cxColumn - SPACING_X - DETAIL_ICON_WIDTH;
		m_List[i].cyTitle = MediumBold.CalcHeight(m_List[i].sTitle, cxTitle);

		int cyDetails = m_List[i].cyTitle;

		//	Compute height of description

		cyDetails += m_List[i].Desc.CalcHeight(cxText);

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
	for (int i = 0; i < iBreakAt; i++)
		cyCol1 += m_List[i].cyRect;

	int cyCol2 = 0;
	for (int i = iBreakAt; i < m_List.GetCount(); i++)
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
	CalcColumnRects(cxWidth, cyHeight, cxColumn, cyCol1, cyCol2, dwFlags, rcCol1, rcCol2);

	//	Now format all entries. Start with the first column.

	FormatColumn(0, iBreakAt, rcCol1, dwFlags | FORMAT_LEFT_COLUMN);
	FormatColumn(iBreakAt, m_List.GetCount(), rcCol2, dwFlags | FORMAT_RIGHT_COLUMN);

	//	Return the max column height.

	if (retcyHeight)
		*retcyHeight = cyMax;
	}

void CDetailList::FormatSingleColumn (int cxWidth, int cyHeight, DWORD dwFlags, int *retcyHeight)

//	FormatSingleColumn
//
//	Formats a single column with labels on the left and text on the right.

	{
	const CG16bitFont &Medium = m_VI.GetFont(fontMedium);
	const CG16bitFont &MediumBold = m_VI.GetFont(fontMediumBold);

	int cxPane = cxWidth;
	int cxColumn = (cxPane - SPACING_X) / 2;
	int cyArea = 0;

	//	Start by measuring everything.

	for (int i = 0; i < m_List.GetCount(); i++)
		{
		SDetailEntry &Entry = m_List[i];

		//	Calculate the height of the description

		Entry.cyText = Entry.Desc.CalcHeight(cxColumn);
		Entry.cyRect = Entry.cyText;

		//	The left column has the icon and title.

		if (Entry.pIcon)
			Entry.cyRect = Max(Entry.cyRect, DETAIL_ICON_HEIGHT);

		if (!Entry.sTitle.IsBlank())
			{
			Entry.cyTitle = MediumBold.CalcHeight(Entry.sTitle, cxColumn);
			Entry.cyRect = Max(Entry.cyRect, Entry.cyTitle);
			}

		//	Compute the total height

		cyArea += Entry.cyRect;
		}

	//	Compute the starting y coordinate (in case we're formatting vertically).

	cyHeight = Max(cyHeight, cyArea);
	int yRow;
	if (dwFlags & FORMAT_PLACE_BOTTOM)
		yRow = cyHeight - cyArea;

	else if (dwFlags & FORMAT_PLACE_CENTER)
		yRow = (cyHeight - cyArea) / 2;

	else
		yRow = 0;

	//	Format each row

	for (int i = 0; i < m_List.GetCount(); i++)
		{
		SDetailEntry &Entry = m_List[i];

		//	Format the left side.

		RECT rcLeft;
		rcLeft.top = yRow;
		rcLeft.left = 0;
		rcLeft.right = cxColumn;
		rcLeft.bottom = yRow + Entry.cyRect;

		if (dwFlags & FORMAT_ANTI_MIRROR_COLUMNS)
			{
			Entry.rcIcon.left = rcLeft.left;
			Entry.rcIcon.top = rcLeft.top;
			Entry.rcIcon.right = Entry.rcIcon.left + DETAIL_ICON_WIDTH;
			Entry.rcIcon.bottom = Entry.rcIcon.top + DETAIL_ICON_HEIGHT;

			Entry.rcTitle.left = Entry.rcIcon.right + SPACING_X;
			Entry.rcTitle.top = rcLeft.top;
			Entry.rcTitle.right = rcLeft.right;
			Entry.rcTitle.bottom = rcLeft.bottom;

			Entry.bAlignRight = false;
			}
		else if (Entry.pIcon)
			{
			if (!Entry.sTitle.IsBlank())
				{
				int cxTitle = MediumBold.MeasureText(Entry.sTitle);

				Entry.rcIcon.left = rcLeft.right - cxTitle - SPACING_X - DETAIL_ICON_WIDTH;
				}
			else
				{
				Entry.rcIcon.left = rcLeft.right - DETAIL_ICON_WIDTH;
				}

			Entry.rcIcon.right = Entry.rcIcon.left + DETAIL_ICON_WIDTH;
			Entry.rcIcon.top = rcLeft.top;
			Entry.rcIcon.bottom = Entry.rcIcon.top + DETAIL_ICON_HEIGHT;

			Entry.rcTitle = rcLeft;
			Entry.bAlignRight = true;
			}
		else
			{
			Entry.rcTitle = rcLeft;
			Entry.bAlignRight = true;
			}

		//	Format the right side

		RECT rcRight;
		rcRight.top = yRow;
		rcRight.left = cxWidth - cxColumn;
		rcRight.right = cxWidth;
		rcRight.bottom = yRow + Entry.cyRect;

		Entry.rcDesc = rcRight;

		//	Next row

		yRow += Entry.cyRect;
		}

	//	Done

	if (retcyHeight)
		*retcyHeight = cyArea;
	}

void CDetailList::Load (ICCItem *pDetails)

//	Load
//
//	Load details from an TLisp item to an array.

	{
	m_List.DeleteAll();

	if (pDetails == NULL || !pDetails->IsList())
		return;

	for (int i = 0; i < pDetails->GetCount(); i++)
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
			DWORD dwIcon = CTLispConvert::AsImageDesc(pIcon, &pNewEntry->rcIconSrc);
			if (dwIcon)
				pNewEntry->pIcon = g_pUniverse->GetLibraryBitmap(dwIcon); 
			}
		}
	}

void CDetailList::Paint (CG32bitImage &Dest, int x, int y) const

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
			CPaintHelper::PaintScaledImage(Dest, x + Entry.rcIcon.left, y + Entry.rcIcon.top, DETAIL_ICON_WIDTH, DETAIL_ICON_HEIGHT, *Entry.pIcon, Entry.rcIconSrc);

		//	Paint the title

		if (!Entry.sTitle.IsBlank())
			{
			DWORD dwTextFlags = (Entry.bAlignRight ? CG16bitFont::AlignRight : 0);

			RECT rcRect;
			rcRect.left = x + Entry.rcTitle.left;
			rcRect.top = y + Entry.rcTitle.top;
			rcRect.right = x + Entry.rcTitle.right;
			rcRect.bottom = y + Entry.rcTitle.bottom;

			MediumBold.DrawText(Dest, rcRect, m_rgbTextColor, Entry.sTitle, 0, dwTextFlags);
			}

		//	Paint the description

		RECT rcDesc;
		rcDesc.left = x + Entry.rcDesc.left;
		rcDesc.top = y + Entry.rcDesc.top;
		rcDesc.right = x + Entry.rcDesc.right;
		rcDesc.bottom = y + Entry.rcDesc.bottom;

		Entry.Desc.Paint(Dest, rcDesc);
		}
	}
