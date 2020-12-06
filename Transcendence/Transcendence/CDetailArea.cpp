//	CDetailArea.cpp
//
//	CDetailArea class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Transcendence.h"

#define FIELD_ALIGN							CONSTLIT("align")
#define FIELD_DETAILS						CONSTLIT("details")
#define FIELD_DETAILS_STYLE					CONSTLIT("detailsStyle")
#define FIELD_LARGE_ICON					CONSTLIT("largeIcon")
#define FIELD_LARGE_ICON_SCALE				CONSTLIT("largeIconScale")
#define FIELD_TITLE							CONSTLIT("title")

#define ALIGN_BOTTOM						CONSTLIT("bottom")
#define ALIGN_TOP							CONSTLIT("top")

#define STYLE_FULL							CONSTLIT("full")
#define STYLE_STACKED						CONSTLIT("stacked")
#define STYLE_STATS							CONSTLIT("stats")

bool CDetailArea::Format (const RECT &rcRect) const

//	Format
//
//	Format based on m_pData

	{
	//	If we're already formatted, then nothing to do

	if (m_bFormatted)
		return true;

	//	If we have no data, then we do a null format.

	else if (!m_pData || m_pData->IsNil() || !m_pData->IsSymbolTable())
		{
		m_iStyle = styleDefault;
		m_rcFrame = rcRect;
		return false;
		}

	//	Error case

	else if (m_pData->IsError())
		{
		m_iStyle = styleStacked;
		m_sTitle = m_pData->GetStringValue();
		m_Details.Load(NULL);

		m_rcFrame = rcRect;
		return true;
		}

	//	Load the title

	m_sTitle = m_pData->GetStringAt(FIELD_TITLE);

	//	Load details

	ICCItem *pDetails = m_pData->GetElement(FIELD_DETAILS);
	if (pDetails)
		m_Details.Load(pDetails);
	else
		m_Details.CleanUp();

	//	We need some fonts

	const CG16bitFont &SubTitle = m_VI.GetFont(fontSubTitle);
	const int cyTitle = SubTitle.GetHeight();
	const int iIconSize = cyTitle * 4;
	constexpr int xPadding = 20;
	constexpr int yPadding = 20;

	//	Calculate various metrics based on the style

	m_iStyle = GetStyle(m_pData);
	m_iAlignment = GetAlignment(m_pData);
	DWORD dwDetailFlags;
	int cxDetailsFrame;
	int cyDetailsFrame;

	switch (m_iStyle)
		{
		case styleStacked:
			{
			dwDetailFlags = CDetailList::FORMAT_PLACE_BOTTOM | CDetailList::FORMAT_ALIGN_TOP;
			cxDetailsFrame = RectWidth(rcRect);
			cyDetailsFrame = RectHeight(rcRect);
			break;
			}

		//	Similar to default, but we assume that we have more room and thus 
		//	paint a fully visible large icon.

		case styleFull:
			{
			dwDetailFlags = CDetailList::FORMAT_PLACE_BOTTOM | CDetailList::FORMAT_ALIGN_BOTTOM | CDetailList::FORMAT_ANTI_MIRROR_COLUMNS;
			cxDetailsFrame = RectWidth(rcRect);
			cyDetailsFrame = RectHeight(rcRect);
			break;
			}

		//	This style uses a single column with a icon and title at the top

		case styleStats:
			{
			dwDetailFlags = CDetailList::FORMAT_MIRROR_COLUMNS | CDetailList::FORMAT_SINGLE_COLUMN | CDetailList::FORMAT_ALIGN_TOP;
			cxDetailsFrame = RectWidth(rcRect) - (2 * xPadding) - iIconSize;
			cyDetailsFrame = 0;
			break;
			}

		default:
			{
			dwDetailFlags = CDetailList::FORMAT_PLACE_BOTTOM | CDetailList::FORMAT_ALIGN_BOTTOM | CDetailList::FORMAT_ANTI_MIRROR_COLUMNS;
			cxDetailsFrame = RectWidth(rcRect);
			cyDetailsFrame = RectHeight(rcRect);
			break;
			}
		}

	//	Format the details

	int cyDetails;
	m_Details.Format(cxDetailsFrame, cyDetailsFrame, dwDetailFlags, &cyDetails);
	m_Details.SetColor(m_rgbText);

	//	For certain styles we need more metric calculations

	switch (m_iStyle)
		{
		case styleStats:
			{
			const int cyHalfLineSpacing = SPACING_Y;

			const int cyRight = (iIconSize / 2) + cyHalfLineSpacing + cyDetails;
			const int cyArea = Max(cyRight, iIconSize);

			//	Figure out our frame, based on alignment

			m_rcFrame.left = rcRect.left;
			m_rcFrame.right = rcRect.right;
			if (m_iAlignment == alignBottom)
				{
				m_rcFrame.bottom = rcRect.bottom;
				m_rcFrame.top = m_rcFrame.bottom - (2 * yPadding) - cyArea;
				}
			else
				{
				m_rcFrame.top = rcRect.top;
				m_rcFrame.bottom = m_rcFrame.top + (2 * yPadding) + cyArea;
				}

			//	Place all other components inside the frame.

			m_rcIcon.top = m_rcFrame.top + yPadding;
			m_rcIcon.left = m_rcFrame.left + xPadding;
			m_rcIcon.right = m_rcIcon.left + iIconSize;
			m_rcIcon.bottom = m_rcIcon.top + iIconSize;
			int yIconCenter = m_rcIcon.top + (RectHeight(m_rcIcon) / 2);

			m_rcTitle.left = m_rcIcon.right + SPACING_X;
			m_rcTitle.top = yIconCenter - (cyTitle + cyHalfLineSpacing);
			m_rcTitle.right = m_rcFrame.right - xPadding;
			m_rcTitle.bottom = m_rcTitle.top + cyTitle;

			m_rcDetails.left = m_rcIcon.right;
			m_rcDetails.right = m_rcFrame.right - xPadding;
			m_rcDetails.top = yIconCenter + cyHalfLineSpacing;
			m_rcDetails.bottom = m_rcDetails.top + cyDetails;

			break;
			}

		default:
			m_rcFrame = rcRect;
			m_rcDetails = rcRect;
			break;
		}

	//	Done

	m_bFormatted = true;
	return true;
	}

AlignmentStyles CDetailArea::GetAlignment (ICCItem *pData)

//	GetAlignment
//
//	Returns the defined alignment.

	{
	if (pData)
		{
		CString sAlign = pData->GetStringAt(FIELD_ALIGN);
		if (strEquals(sAlign, ALIGN_BOTTOM))
			return alignBottom;
		else
			return alignTop;
		}
	else
		return alignTop;
	}

CDetailArea::EStyles CDetailArea::GetStyle (ICCItem *pData)

//	GetStyle
//
//	Returns the style from the data element.

	{
	if (pData == NULL)
		return styleDefault;

	CString sStyle = pData->GetStringAt(FIELD_DETAILS_STYLE);
	if (strEquals(sStyle, STYLE_FULL))
		return styleFull;
	else if (strEquals(sStyle, STYLE_STACKED))
		return styleStacked;
	else if (strEquals(sStyle, STYLE_STATS))
		return styleStats;
	else
		return styleDefault;
	}

void CDetailArea::Paint (CG32bitImage &Dest, const RECT &rcRect) const

//	Paint
//
//	Paint the details area

	{
	CCodeChain &CC = m_Universe.GetCC();
	CDockScreenPainter Painter(m_Theme);

	//	Colors and metrics

    CG32bitPixel rgbFadeBackColor = CG32bitPixel(CG32bitPixel::Darken(m_rgbBack, 220), 185);

	if (!Format(rcRect))
		{
#if 0
		PaintBackground(Dest, rcRect, rgbFadeBackColor);
		Painter.PaintDisplayFrame(Dest, rcRect);
#endif
		return;
		}

	//	The detail list holds all the individual stats. It is responsible for
	//	formatting them.

	//	Paint the background, and while we're at it, figure out the formatting
	//	flags for all the details.

	switch (m_iStyle)
		{
		case styleStacked:
			{
			PaintBackground(Dest, m_rcFrame, rgbFadeBackColor);

			//	Paint the icon first

			int x = rcRect.left + RectWidth(rcRect) / 2;
			int y = rcRect.top + RectHeight(rcRect) / 4;
			ICCItem *pLargeIcon = m_pData->GetElement(FIELD_LARGE_ICON);
			Metric rScale = m_pData->GetDoubleAt(FIELD_LARGE_ICON_SCALE, 1.0);

			PaintStackedImage(Dest, x, y, pLargeIcon, rScale);

			//	Paint the title

			y = rcRect.top + SPACING_Y;
			const CG16bitFont &SubTitle = m_VI.GetFont(fontSubTitle);
			SubTitle.DrawText(Dest, x, y, m_rgbText, m_sTitle, CG16bitFont::AlignCenter);

			break;
			}

		//	Similar to default, but we assume that we have more room and thus 
		//	paint a fully visible large icon.

		case styleFull:
			{
			//	Paint background

			PaintBackground(Dest, m_rcFrame, rgbFadeBackColor);

			//	Paint icon

			ICCItem *pLargeIcon = m_pData->GetElement(FIELD_LARGE_ICON);
			PaintBackgroundImage(Dest, rcRect, pLargeIcon, BACKGROUND_IMAGE_MARGIN_Y);
			break;
			}

		//	This style uses a single column with a icon and title at the top

		case styleStats:
			{
			//	Paint background

			PaintBackground(Dest, m_rcFrame, rgbFadeBackColor);

			//	Paint icon

			PaintBadgeImage(Dest, m_rcIcon);

			//	Paint title

			const CG16bitFont &SubTitle = m_VI.GetFont(fontSubTitle);
			SubTitle.DrawText(Dest, m_rcTitle, m_rgbText, m_sTitle, 0, 0);

			//	Paint a line under title

			int xLine = m_rcIcon.right + SPACING_X;
			int yLine = m_rcIcon.top + RectHeight(m_rcIcon) / 2;
			int cxLine = m_rcFrame.right - SPACING_X - xLine;

			Dest.FillLine(xLine, yLine, cxLine, RGB_BADGE);

			break;
			}

		default:
			{
			//	Paint the large icon first (we paint the background on top so that 
			//	the details show up.

			ICCItem *pLargeIcon = m_pData->GetElement(FIELD_LARGE_ICON);
			PaintBackgroundImage(Dest, rcRect, pLargeIcon);

			//	Paint background

			PaintBackground(Dest, m_rcFrame, rgbFadeBackColor);
			break;
			}
		}

	//	Format and paint the details

	m_Details.Paint(Dest, m_rcDetails.left, m_rcDetails.top);

	//	Paint a frame on top

	Painter.PaintDisplayFrame(Dest, m_rcFrame);
	}

void CDetailArea::PaintBackground (CG32bitImage &Dest, const RECT &rcRect, CG32bitPixel rgbColor) const

//	PaintBackground
//
//	Paints the content background

	{
	CGDraw::RoundedRect(Dest,
			rcRect.left,
			rcRect.top + m_cyTabRegion,
			RectWidth(rcRect),
			RectHeight(rcRect) - m_cyTabRegion,
			BORDER_RADIUS + 1,
			rgbColor);
	}

void CDetailArea::PaintBackgroundImage (CG32bitImage &Dest, const RECT &rcRect, ICCItem *pImageDesc, int cyExtraMargin) const

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
	int y = rcRect.top + cyExtraMargin;

	CPaintHelper::PaintScaledImage(Dest, x, y, m_cxLargeIcon, m_cyLargeIcon, *pImage, rcImage);
	}

void CDetailArea::PaintBadgeImage (CG32bitImage &Dest, const RECT &rcDest) const

//	PaintBadgeImage
//
//	Paints the large icon in a circle.

	{
	int iRadius = RectWidth(rcDest) / 2;
	int xCenter = rcDest.left + RectWidth(rcDest) / 2;
	int yCenter = rcDest.top + RectHeight(rcDest) / 2;

	CGDraw::Circle(Dest, xCenter, yCenter, iRadius, RGB_BADGE);

	if (ICCItem *pLargeIcon = m_pData->GetElement(FIELD_LARGE_ICON))
		{
		int iIconSize = 4 * iRadius / 3;

		RECT rcIcon;
		rcIcon.left = rcDest.left + (RectWidth(rcDest) - iIconSize) / 2;
		rcIcon.top = rcDest.top + (RectWidth(rcDest) - iIconSize) / 2;
		rcIcon.right = rcIcon.left + iIconSize;
		rcIcon.bottom = rcIcon.top	+ iIconSize;

		PaintScaledImage(Dest, rcIcon, *pLargeIcon);
		}
	}

void CDetailArea::PaintScaledImage (CG32bitImage &Dest, const RECT &rcDest, const ICCItem &ImageDesc) const

//	PaintScaledImage
//
//	Paints the icon in the given rect, scaling if necessary.

	{
	RECT rcImage;
	DWORD dwImage = CTLispConvert::AsImageDesc(&ImageDesc, &rcImage);
	CG32bitImage *pImage = m_Universe.GetLibraryBitmap(dwImage);
	if (pImage == NULL)
		return;

	int cxImage = RectWidth(rcImage);
	int cyImage = RectHeight(rcImage);

	if (cxImage < RectWidth(rcDest))
		{
		int xDest = rcDest.left + (RectWidth(rcDest) - cxImage) / 2;
		int yDest = rcDest.top + (RectHeight(rcDest) - cyImage) / 2;

		Dest.Blt(rcImage.left, rcImage.top, cxImage, cyImage, *pImage, xDest, yDest);
		}
	else
		CGDraw::BltScaled(Dest, rcDest.left, rcDest.top, RectWidth(rcDest), RectHeight(rcDest), *pImage, rcImage.left, rcImage.top, RectWidth(rcImage), RectHeight(rcImage));
	}

void CDetailArea::PaintStackedImage (CG32bitImage &Dest, int x, int y, ICCItem *pImageDesc, Metric rScale) const

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

	//	Scale, if necessary

	if (rScale != 1.0)
		{
		CPaintHelper::PaintScaledImage(Dest, x, y, 0, 0, *pImage, rcImage, rScale);
		}
	else
		{
		int xPos = x - cxImage / 2;
		int yPos = y - cyImage / 2;

		Dest.Blt(rcImage.left, rcImage.top, cxImage, cyImage, *pImage, xPos, yPos);
		}
	}
