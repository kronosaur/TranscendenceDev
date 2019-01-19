//	CDetailArea.cpp
//
//	CDetailArea class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Transcendence.h"

#define FIELD_DETAILS						CONSTLIT("details")
#define FIELD_DETAILS_STYLE					CONSTLIT("detailsStyle")
#define FIELD_LARGE_ICON					CONSTLIT("largeIcon")
#define FIELD_TITLE							CONSTLIT("title")

#define STYLE_STACKED						CONSTLIT("stacked")

CDetailArea::EStyles CDetailArea::GetStyle (ICCItem *pData)

//	GetStyle
//
//	Returns the style from the data element.

	{
	if (pData == NULL)
		return styleDefault;

	CString sStyle = pData->GetStringAt(FIELD_DETAILS_STYLE);
	if (strEquals(sStyle, STYLE_STACKED))
		return styleStacked;
	else
		return styleDefault;
	}

void CDetailArea::Paint (CG32bitImage &Dest, const RECT &rcRect, ICCItem *pData)

//	Paint
//
//	Paint the details area

	{
	CCodeChain &CC = m_Universe.GetCC();

	//	Colors and metrics

    CG32bitPixel rgbFadeBackColor = CG32bitPixel(CG32bitPixel::Darken(m_rgbBack, 220), 185);

	//	If no content, then just a blank pane.

	if (pData == NULL || pData->IsNil() || !pData->IsSymbolTable())
		{
		PaintBackground(Dest, rcRect, rgbFadeBackColor);
		//PaintBackgroundFrame(Dest, rcRect);
		return;
		}

	//	Figure out our basic style

	EStyles iStyle = GetStyle(pData);

	//	The detail list holds all the individual stats. It is responsible for
	//	formatting them.

	CDetailList Details(m_VI);
	Details.SetColor(m_rgbText);

	ICCItem *pDetails = pData->GetElement(FIELD_DETAILS);
	if (pDetails)
		Details.Load(pDetails);

	//	Paint the background, and while we're at it, figure out the formatting
	//	flags for all the details.

	DWORD dwDetailFlags;
	switch (iStyle)
		{
		case styleStacked:
			{
			dwDetailFlags = CDetailList::FORMAT_PLACE_BOTTOM | CDetailList::FORMAT_ALIGN_TOP;

			PaintBackground(Dest, rcRect, rgbFadeBackColor);

			//	Paint the icon first

			int x = rcRect.left + RectWidth(rcRect) / 2;
			int y = rcRect.top + RectHeight(rcRect) / 4;
			ICCItem *pLargeIcon = pData->GetElement(FIELD_LARGE_ICON);
			PaintStackedImage(Dest, x, y, pLargeIcon);

			//	Paint the title

			y = rcRect.top + SPACING_Y;
			const CG16bitFont &SubTitle = m_VI.GetFont(fontSubTitle);
			SubTitle.DrawText(Dest, x, y, m_rgbText, pData->GetStringAt(FIELD_TITLE), CG16bitFont::AlignCenter);

			break;
			}

		default:
			{
			dwDetailFlags = CDetailList::FORMAT_PLACE_BOTTOM | CDetailList::FORMAT_ALIGN_BOTTOM | CDetailList::FORMAT_ANTI_MIRROR_COLUMNS;

			//	Paint the large icon first (we paint the background on top so that 
			//	the details show up.

			ICCItem *pLargeIcon = pData->GetElement(FIELD_LARGE_ICON);
			PaintBackgroundImage(Dest, rcRect, pLargeIcon);

			//	Paint background

			PaintBackground(Dest, rcRect, rgbFadeBackColor);
			break;
			}
		}

	//	Format and paint the details

	int cyDetails;
	Details.Format(rcRect, dwDetailFlags, &cyDetails);
	Details.Paint(Dest);

	//	Paint a frame on top

	//PaintBackgroundFrame(Dest, rcRect);
	}

void CDetailArea::PaintBackground (CG32bitImage &Dest, const RECT &rcRect, CG32bitPixel rgbColor)

//	PaintBackground
//
//	Paints the content background

	{
	CGDraw::RoundedRect(Dest,
			rcRect.left,
			rcRect.top,
			RectWidth(rcRect),
			RectHeight(rcRect),
			BORDER_RADIUS + 1,
			rgbColor);
	}

void CDetailArea::PaintBackgroundFrame (CG32bitImage &Dest, const RECT &rcRect)

//	PaintBackgroundFrame
//
//	Paint the content frame

	{
	CGDraw::RoundedRectOutline(Dest,
			rcRect.left,
			rcRect.top,
			RectWidth(rcRect),
			RectHeight(rcRect),
			BORDER_RADIUS,
			1,
			CG32bitPixel(80,80,80));
	}

void CDetailArea::PaintBackgroundImage (CG32bitImage &Dest, const RECT &rcRect, ICCItem *pImageDesc)

//	PaintBackgroundImage
//
//	Paints a large image in the background.

	{
	if (pImageDesc == NULL)
		return;

	RECT rcImage;
	DWORD dwImage = CTLispConvert::AsImageDesc(pImageDesc, &rcImage);
	CG32bitImage *pImage = m_Universe.GetLibraryBitmap(dwImage);
	if (pImage == NULL)
		return;

	int cxImage = RectWidth(rcImage);
	int cyImage = RectHeight(rcImage);

	int x = rcRect.left + (RectWidth(rcRect) - m_cxLargeIcon) / 2;
	int y = rcRect.top;

	CPaintHelper::PaintScaledImage(Dest, x, y, m_cxLargeIcon, m_cyLargeIcon, *pImage, rcImage);
	}

void CDetailArea::PaintStackedImage (CG32bitImage &Dest, int x, int y, ICCItem *pImageDesc)

//	PaintStackedImage
//
//	Paints the image centered on the given coordinate

	{
	if (pImageDesc == NULL)
		return;

	RECT rcImage;
	DWORD dwImage = CTLispConvert::AsImageDesc(pImageDesc, &rcImage);
	CG32bitImage *pImage = m_Universe.GetLibraryBitmap(dwImage);
	if (pImage == NULL)
		return;

	int cxImage = RectWidth(rcImage);
	int cyImage = RectHeight(rcImage);

	int xPos = x - cxImage / 2;
	int yPos = y - cyImage / 2;

	Dest.Blt(rcImage.left, rcImage.top, cxImage, cyImage, *pImage, xPos, yPos);
	}
