//	TImagePlane.h
//
//	TImagePlane template
//	Copyright (c) 2018 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

template <class VALUE> class TImagePlane
	{
	public:
		TImagePlane (void) { }

		VALUE *AddRef (void) { m_dwRefCount++; return (VALUE *)this; }

		bool AdjustCoords (int *xSrc, int *ySrc, int cxSrc, int cySrc,
						   int *xDest, int *yDest,
						   int *cxWidth, int *cyHeight) const

		//	AdjustCoords
		//
		//	Make sure that the coordinates are in range and adjust
		//	them if they are not.

			{
			if (xSrc && *xSrc < 0)
				{
				*cxWidth += *xSrc;
				*xDest -= *xSrc;
				*xSrc = 0;
				}

			if (ySrc && *ySrc < 0)
				{
				*cyHeight += *ySrc;
				*yDest -= *ySrc;
				*ySrc = 0;
				}

			if (*xDest < m_rcClip.left)
				{
				*cxWidth += (*xDest - m_rcClip.left);
				if (xSrc) *xSrc -= (*xDest - m_rcClip.left);
				*xDest = m_rcClip.left;
				}

			if (*yDest < m_rcClip.top)
				{
				*cyHeight += (*yDest - m_rcClip.top);
				if (ySrc) *ySrc -= (*yDest - m_rcClip.top);
				*yDest = m_rcClip.top;
				}

			*cxWidth = min(*cxWidth, m_rcClip.right - *xDest);
			if (xSrc)
				*cxWidth = min(*cxWidth, cxSrc - *xSrc);

			*cyHeight = min(*cyHeight, m_rcClip.bottom - *yDest);
			if (ySrc)
				*cyHeight = min(*cyHeight, cySrc - *ySrc);

			return (*cxWidth > 0 && *cyHeight > 0);
			}

		bool AdjustScaledCoords (Metric *xSrc, Metric *ySrc, int cxSrc, int cySrc,
								 Metric xSrcInc, Metric ySrcInc,
								 int *xDest, int *yDest,
								 int *cxDest, int *cyDest)

		//	AdjustCoords
		//
		//	Make sure that the coordinates are in range and adjust
		//	them if they are not.

			{
			if (xSrc && *xSrc < 0.0)
				{
				*cxDest += (int)(*xSrc / xSrcInc);
				*xDest -= (int)(*xSrc / xSrcInc);
				*xSrc = 0.0;
				}

			if (ySrc && *ySrc < 0)
				{
				*cyDest += (int)(*ySrc / ySrcInc);
				*yDest -= (int)(*ySrc / ySrcInc);
				*ySrc = 0.0;
				}

			if (*xDest < m_rcClip.left)
				{
				*cxDest += (*xDest - m_rcClip.left);
				if (xSrc) *xSrc -= (*xDest - m_rcClip.left);
				*xDest = m_rcClip.left;
				}

			if (*yDest < m_rcClip.top)
				{
				*cyDest += (*yDest - m_rcClip.top);
				if (ySrc) *ySrc -= (*yDest - m_rcClip.top);
				*yDest = m_rcClip.top;
				}

			*cxDest = Min(*cxDest, (int)(m_rcClip.right - *xDest));
			if (xSrc)
				*cxDest = Min(*cxDest, (int)((cxSrc - *xSrc) / xSrcInc));

			*cyDest = Min(*cyDest, (int)(m_rcClip.bottom - *yDest));
			if (ySrc)
				*cyDest = min(*cyDest, (int)((cySrc - *ySrc) / ySrcInc));

			return (*cxDest > 0 && *cyDest > 0);
			}

		void Delete (void) { if (--m_dwRefCount <= 0) delete (VALUE *)this; }
		const RECT &GetClipRect (void) const { return m_rcClip; }
		int GetHeight (void) const { return m_cyHeight; }
		int GetWidth (void) const { return m_cxWidth; }

		void ResetClipRect (void)

		//	ResetClipRect
		//
		//	Clears the clip rect

			{
			m_rcClip.left = 0;
			m_rcClip.top = 0;
			m_rcClip.right = m_cxWidth;
			m_rcClip.bottom = m_cyHeight;
			}

		void SetClipRect (const RECT &rcClip)

		//	SetClipRect
		//
		//	Sets the clip rect

			{
			m_rcClip.left = min(max(0, rcClip.left), m_cxWidth);
			m_rcClip.top = min(max(0, rcClip.top), m_cyHeight);
			m_rcClip.right = min(max(m_rcClip.left, rcClip.right), m_cxWidth);
			m_rcClip.bottom = min(max(m_rcClip.top, rcClip.bottom), m_cyHeight);
			}

	protected:
		int m_cxWidth = 0;
		int m_cyHeight = 0;

		RECT m_rcClip = { 0 };

		DWORD m_dwRefCount = 1;
	};
