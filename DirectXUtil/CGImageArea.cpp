//	CGImageArea.cpp
//
//	Implementation of CGImageArea class

#include <windows.h>
#include <ddraw.h>
#include "Alchemy.h"
#include "DirectXUtil.h"

CGImageArea::CGImageArea (void) : m_pImage(NULL),
		m_rgbBackColor(CG32bitPixel(0,0,0)),
		m_bTransBackground(false),
		m_dwStyles(alignLeft | alignTop)

//	CGImageArea constructor

	{
	}

void CGImageArea::Paint (CG32bitImage &Dest, const RECT &rcRect)

//	Paint
//
//	Handle paint

	{
	if (!m_bTransBackground)
		Dest.Fill(rcRect.left,
				rcRect.top,
				RectWidth(rcRect),
				RectHeight(rcRect),
				m_rgbBackColor);

	if (m_pImage)
		{
		int x, y;

		//	Compute position of image in the area rect

		if (m_dwStyles & alignCenter)
			x = rcRect.left + (RectWidth(rcRect) - RectWidth(m_rcImage)) / 2;
		else if (m_dwStyles & alignRight)
			x = rcRect.left + (RectWidth(rcRect) - RectWidth(m_rcImage));
		else
			x = rcRect.left;

		if (m_dwStyles & alignMiddle)
			y = rcRect.top + (RectHeight(rcRect) - RectHeight(m_rcImage)) / 2;
		else if (m_dwStyles & alignBottom)
			y = rcRect.top + (RectHeight(rcRect) - RectHeight(m_rcImage));
		else
			y = rcRect.top;

		//	Blt
		//
		//	For compatibility with previous (Transcendence) releases, we assume
		//	black is transparent if the image has no alpha channel.

		CGDraw::BltWithBackColor(Dest,
				x,
				y,
				*m_pImage,
				m_rcImage.left,
				m_rcImage.top,
				RectWidth(m_rcImage),
				RectHeight(m_rcImage),
				CG32bitPixel(0, 0, 0));
		}
	}

