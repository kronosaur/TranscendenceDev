//	CCartoucheBlock.cpp
//
//	CCartoucheBlock class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "TextFormat.h"

void CCartoucheBlock::Add (const TArray<SCartoucheDesc> &List)

//	Add
//
//	Adds a list of cartouches.

	{
	m_Data.GrowToFit(List.GetCount());
	for (int i = 0; i < List.GetCount(); i++)
		AddCartouche(List[i].sText, List[i].rgbColor, List[i].rgbBack);
	}

void CCartoucheBlock::AddCartouche (const CString &sText, CG32bitPixel rgbColor, CG32bitPixel rgbBack)

//	AddCartouche
//
//	Adds a new cartouche.

	{
	SCartouche *pEntry = m_Data.Insert();
	pEntry->sText = sText;
	pEntry->rgbColor = rgbColor;
	pEntry->rgbBack = rgbBack;

	Invalidate();
	}

void CCartoucheBlock::Format (int cxWidth)

//	Format
//
//	Formats the block to the given width.

	{
	if (m_bFormatted || m_pFont == NULL)
		return;

	m_cxWidth = cxWidth;
	int cxLeft = cxWidth;
	int x = 0;
	int y = 0;

	for (int i = 0; i < m_Data.GetCount(); i++)
		{
		int cxText = (ATTRIB_PADDING_X * 2) + m_pFont->MeasureText(m_Data[i].sText);
		int cyText = (ATTRIB_PADDING_Y * 2) + m_pFont->GetHeight();
		if (cxText > cxLeft && cxLeft != cxWidth)
			{
			x = 0;
			y += cyText + ATTRIB_SPACING_Y;
			cxLeft = cxWidth;
			}

		m_Data[i].x = x;
		m_Data[i].y = y;
		m_Data[i].cx = cxText;
		m_Data[i].cy = cyText;

		x += cxText + ATTRIB_SPACING_X;
		cxLeft -= cxText + ATTRIB_SPACING_X;
		}

	if (m_Data.GetCount() > 0)
		m_cyHeight = y + (ATTRIB_PADDING_Y * 2) + m_pFont->GetHeight();
	else
		m_cyHeight = 0;

	m_bFormatted = true;
	}

RECT CCartoucheBlock::GetBounds (void) const

//	GetBounds
//
//	Returns the rectangular bounds.

	{
	RECT rcRect;

	rcRect.left = 0;
	rcRect.top = 0;
	rcRect.right = m_cxWidth;
	rcRect.bottom = m_cyHeight;

	return rcRect;
	}

void CCartoucheBlock::Paint (CG32bitImage &Dest, int x, int y, const SPaintOptions &Options) const

//	Paint
//
//	Paint at the given position.

	{
	constexpr CG32bitPixel RGB_DISABLED_BACKGROUND(80, 80, 80);
	constexpr CG32bitPixel RGB_DISABLED_TEXT(0, 0, 0);

	if (!m_bFormatted || m_pFont == NULL)
		return;

	for (int i = 0; i < m_Data.GetCount(); i++)
		{
		//	Draw the background

		CGDraw::RoundedRect(Dest, 
				x + m_Data[i].x, 
				y + m_Data[i].y, 
				m_Data[i].cx, 
				m_Data[i].cy, 
				RADIUS, 
				(Options.bDisabled ? RGB_DISABLED_BACKGROUND : m_Data[i].rgbBack));

		//	Draw the text

		m_pFont->DrawText(Dest, 
				x + m_Data[i].x + ATTRIB_PADDING_X, 
				y + m_Data[i].y + ATTRIB_PADDING_Y, 
				(Options.bDisabled ? RGB_DISABLED_TEXT : m_Data[i].rgbColor), 
				m_Data[i].sText);
		}
	}

void CCartoucheBlock::PaintCartouche (CG32bitImage &Dest, int x, int y, const SCartoucheDesc &Desc, const CG16bitFont &Font, DWORD dwAlignment)

//	PaintCartouche
//
//	Paints a single cartouche.

	{
	//	Measure

	int cxText = (ATTRIB_PADDING_X * 2) + Font.MeasureText(Desc.sText);
	int cyText = (ATTRIB_PADDING_Y * 2) + Font.GetHeight();

	//	Align

	int xDest;
	if (dwAlignment & alignCenter)
		xDest = x - (cxText / 2);
	else if (dwAlignment & alignRight)
		xDest = x - cxText;
	else
		xDest = x;

	int yDest;
	if (dwAlignment & alignMiddle)
		yDest = y - (cyText / 2);
	else if (dwAlignment & alignBottom)
		yDest = y - cyText;
	else
		yDest = y;

	//	Draw the background

	CGDraw::RoundedRect(Dest, 
			xDest, 
			yDest, 
			cxText, 
			cyText, 
			RADIUS, 
			Desc.rgbBack);

	//	Draw the text

	Font.DrawText(Dest, 
			xDest + ATTRIB_PADDING_X, 
			yDest + ATTRIB_PADDING_Y, 
			Desc.rgbColor, 
			Desc.sText);
	}
