//	CIconLabelBlock.cpp
//
//	CIconLabelBlock class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "TextFormat.h"

void CIconLabelBlock::Add (const SLabelDesc &Label)

//	Add
//
//	Adds a label.

	{
	SLabel *pEntry = m_Data.Insert();
	pEntry->pIcon = Label.pIcon;
	if (::IsRectEmpty(&Label.rcIconSrc) && pEntry->pIcon)
		{
		pEntry->rcIconSrc.left = 0;
		pEntry->rcIconSrc.top = 0;
		pEntry->rcIconSrc.right = pEntry->pIcon->GetWidth();
		pEntry->rcIconSrc.bottom = pEntry->pIcon->GetHeight();
		}
	else
		pEntry->rcIconSrc = Label.rcIconSrc;

	pEntry->sText = Label.sText;
	pEntry->pFont = Label.pFont;
	pEntry->bNewLine = Label.bNewLine;

	Invalidate();
	}

int CIconLabelBlock::CalcLineHeight (void) const

//	CalcLineHeight
//
//	Computes the line height.

	{
	int cyHeight = 0;

	for (int i = 0; i < m_Data.GetCount(); i++)
		{
		const SLabel &Entry = m_Data[i];
		const CG16bitFont *pFont = GetFont(Entry);
		if (pFont == NULL)
			continue;

		int cyEntry = Max(RectHeight(Entry.rcIconSrc), pFont->GetHeight());
		cyHeight = Max(cyHeight, cyEntry);
		}

	return cyHeight;
	}

void CIconLabelBlock::Format (int cxWidth)

//	Format
//
//	Formats the block.

	{
	if (m_bFormatted)
		return;

	m_cxWidth = cxWidth;
	m_cyLine = CalcLineHeight();
	int cxLeft = cxWidth;
	int x = 0;
	int y = 0;
	bool bIsEmpty = true;

	for (int i = 0; i < m_Data.GetCount(); i++)
		{
		SLabel &Entry = m_Data[i];
		const CG16bitFont *pFont = GetFont(Entry);
		if (pFont == NULL)
			continue;

		int cxEntry = RectWidth(Entry.rcIconSrc) + ICON_MARGIN_X + pFont->MeasureText(Entry.sText);
		int cyEntry = Max(RectHeight(Entry.rcIconSrc), pFont->GetHeight());

		//	Next line?

		if (cxEntry > cxLeft && cxLeft != cxWidth)
			{
			x = 0;
			y += m_cyLine + LINE_SPACING_Y;
			cxLeft = cxWidth;
			}

		m_Data[i].x = x;
		m_Data[i].y = y + (m_cyLine - cyEntry) / 2;
		m_Data[i].cx = cxEntry;
		m_Data[i].cy = cyEntry;

		x += cxEntry + ENTRY_SPACING_X;
		if (Entry.bNewLine)
			cxLeft = 0;
		else
			cxLeft -= cxEntry + ENTRY_SPACING_X;

		bIsEmpty = false;
		}

	if (bIsEmpty)
		m_cyHeight = 0;
	else
		m_cyHeight = y + m_cyLine;

	m_bFormatted = true;
	}

void CIconLabelBlock::Paint (CG32bitImage &Dest, int x, int y, CG32bitPixel rgbText, const SPaintOptions &Options) const

//	Paint
//
//	Paints the block.

	{
	if (!m_bFormatted)
		return;

	for (int i = 0; i < m_Data.GetCount(); i++)
		{
		const SLabel &Entry = m_Data[i];
		const CG16bitFont *pFont = GetFont(Entry);
		if (pFont == NULL)
			continue;

		int xPos = x + Entry.x;
		int yPos = y + Entry.y;

		//	Draw the icon

		if (Entry.pIcon)
			{
			int yIcon = yPos + ((m_cyLine - RectHeight(Entry.rcIconSrc)) / 2);
			if (Options.bDisabled)
				CGDraw::BltGray(Dest, xPos, yIcon, *Entry.pIcon, Entry.rcIconSrc.left, Entry.rcIconSrc.top, RectWidth(Entry.rcIconSrc), RectHeight(Entry.rcIconSrc));
			else
				Dest.Blt(Entry.rcIconSrc.left, Entry.rcIconSrc.top, RectWidth(Entry.rcIconSrc), RectHeight(Entry.rcIconSrc), *Entry.pIcon, xPos, yIcon);

			xPos += RectWidth(Entry.rcIconSrc) + ICON_MARGIN_X;
			}

		//	Draw the text

		int yText = yPos + ((m_cyLine - pFont->GetHeight()) / 2);
		pFont->DrawText(Dest, xPos, yText, rgbText, Entry.sText);
		}
	}
