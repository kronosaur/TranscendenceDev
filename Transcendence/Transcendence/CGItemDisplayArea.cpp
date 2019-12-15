//	CGDrawArea.cpp
//
//	CGDrawArea class

#include "PreComp.h"
#include "Transcendence.h"

const int BORDER_RADIUS =						4;
const int PADDING =								4;

const int ITEM_TEXT_MARGIN_Y =					4;
const int ITEM_TEXT_MARGIN_X =					4;
const int ITEM_TITLE_EXTRA_MARGIN =				4;
const int ITEM_TEXT_MARGIN_BOTTOM =				10;

CGItemDisplayArea::CGItemDisplayArea (void) :
		m_pSource(NULL),
        m_rgbTextColor(255, 255, 255),
		m_ItemPainter(g_pHI->GetVisuals())

//	CGItemDisplayArea constructor

	{
	}

int CGItemDisplayArea::Justify (const RECT &rcRect)

//	Justify
//
//	Figure out how big we are.

	{
	const CVisualPalette &VI = g_pHI->GetVisuals();

	RECT rcInner = rcRect;
	rcInner.left += PADDING;
	rcInner.right -= PADDING;
	rcInner.top += PADDING;
	rcInner.bottom -= PADDING;

	//	If we have an item, format it

	if (m_Item.GetType())
		{
		CItemPainter::SOptions Options;
		Options.bNoIcon = true;
		Options.bTitle = true;
		m_ItemPainter.Init(m_Item, RectWidth(rcInner), Options);

		return (2 * PADDING) + m_ItemPainter.GetHeight();
		}

	//	Otherwise, we expect a title and description

	else if (!m_sTitle.IsBlank() || !m_sDesc.IsBlank())
		{
		const CG16bitFont &LargeBold = VI.GetFont(fontLargeBold);
		const CG16bitFont &Medium = VI.GetFont(fontMedium);

		RECT rcDrawRect = rcInner;
		rcDrawRect.left += ITEM_TEXT_MARGIN_X;
		rcDrawRect.right -= ITEM_TEXT_MARGIN_X;

		int cyHeight = ITEM_TEXT_MARGIN_Y + PADDING;
		cyHeight += LargeBold.GetHeight();
		cyHeight += ITEM_TITLE_EXTRA_MARGIN;	//	A little extra spacing

		int iLines = Medium.BreakText(m_sDesc, RectWidth(rcDrawRect), NULL, 0);
		cyHeight += iLines * Medium.GetHeight();

		//	Margin

		cyHeight += ITEM_TEXT_MARGIN_BOTTOM + PADDING;

		return cyHeight;
		}

	//	Otherwise, if we have nothing, then we're collapsed

	else
		return 0;
	}

void CGItemDisplayArea::Paint (CG32bitImage &Dest, const RECT &rcRect)

//	Paint
//
//	Paint the area

	{
	DEBUG_TRY

	const CVisualPalette &VI = g_pHI->GetVisuals();

	//	Draw background

	CGDraw::RoundedRect(Dest,
			rcRect.left,
			rcRect.top,
			RectWidth(rcRect),
			RectHeight(rcRect),
			BORDER_RADIUS,
			m_rgbBackColor);

	RECT rcInner = rcRect;
	rcInner.left += PADDING;
	rcInner.right -= PADDING;
	rcInner.top += PADDING;
	rcInner.bottom -= PADDING;

	//	Paint the item

	if (m_Item.GetType())
		{
		m_ItemPainter.Paint(Dest, rcInner.left, rcInner.top, m_rgbTextColor);
		}

	//	Paint title and text

	else
		{
		const CG16bitFont &LargeBold = VI.GetFont(fontLargeBold);
		const CG16bitFont &Medium = VI.GetFont(fontMedium);
		CG32bitPixel rgbColorDesc = CG32bitPixel(128,128,128);

		RECT rcDrawRect = rcInner;
		rcDrawRect.left += ITEM_TEXT_MARGIN_X;
		rcDrawRect.right -= ITEM_TEXT_MARGIN_X;
		rcDrawRect.top += ITEM_TEXT_MARGIN_Y;

		int cyHeight = 0;
		LargeBold.DrawText(Dest,
				rcDrawRect,
				m_rgbTextColor,
				m_sTitle,
				0,
				CG16bitFont::SmartQuotes | CG16bitFont::TruncateLine,
				&cyHeight);

		rcDrawRect.top += cyHeight + ITEM_TITLE_EXTRA_MARGIN;

		Medium.DrawText(Dest,
				rcDrawRect,
				rgbColorDesc,
				m_sDesc,
				0,
				CG16bitFont::SmartQuotes,
				&cyHeight);
		}

	DEBUG_CATCH
	}
