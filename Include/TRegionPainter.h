//	TRegionPainter.h
//
//	TRegionPainter template
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class IRegionPainter
	{
	public:
		virtual ~IRegionPainter (void) { }

		virtual void Draw (CG32bitImage &Dest, int x, int y, const CGRegion &Region) = 0;
		virtual void Draw (CG32bitImage &Dest, int x, int y, const CG16bitBinaryRegion &Region) = 0;
		virtual void SetParam (const CString &sParam, const TArray<CG32bitPixel> &ColorTable) { }
	};

template <class PAINTER, class BLENDER> class TRegionPainter32 : public IRegionPainter
	{
	public:

		virtual void Draw (CG32bitImage &Dest, int x, int y, const CGRegion &Region)
			{
			int i;

			if (Region.GetRowCount() == 0)
				return;

			m_pDest = &Dest;
			m_rcClip = &Dest.GetClipRect();
			m_xDest = x;
			m_yDest = y;

			//	Give our specializations a chance to initialize

			if (!BEGIN_DRAW())
				{
				m_pDest = NULL;
				m_rcClip = NULL;
				return;
				}

			//	Paint each pixel in the region

			int xDest = m_xDest + Region.GetXOffset();

			for (i = 0; i < Region.GetRowCount(); i++)
				{
				int yRow;
				DWORD *pRun = Region.GetRow(i, &yRow);
				int yDestRow = m_yDest + yRow;

				//	Make sure this row is in range

				if (yDestRow < m_rcClip->top || yDestRow >= m_rcClip->bottom)
					continue;

				CG32bitPixel *pDestRow = m_pDest->GetPixelPos(xDest, yDestRow);
				CG32bitPixel *pDest = pDestRow;
				int xPos = 0;

				while (true)
					{
					int cxLength;
					DWORD dwOpacity = Region.DecodeRun(*pRun++, &cxLength);

					if (cxLength == 0)
						break;
					else if (dwOpacity == 0)
						{
						pDest += cxLength;
						xPos += cxLength;
						}
					else
						{
						if (xDest + xPos < m_rcClip->left)
							{
							int xSkip = (m_rcClip->left - (xDest + xPos));
							if (xSkip >= cxLength)
								{
								pDest += cxLength;
								xPos += cxLength;
								continue;
								}

							cxLength -= xSkip;
							pDest += xSkip;
							xPos += xSkip;
							}

						int cxLeft = Min(cxLength, (int)m_rcClip->right - (xDest + xPos));
						if (cxLeft <= 0)
							break;

						if (dwOpacity == 0xff)
							{
							CG32bitPixel *pDestEnd = pDest + cxLeft;
							while (pDest < pDestEnd)
								*pDest++ = BLENDER::Blend(*pDest, GET_PIXEL(xPos++, yRow));
							}
						else
							{
							CG32bitPixel *pDestEnd = pDest + cxLeft;
							while (pDest < pDestEnd)
								*pDest++ = BLENDER::BlendAlpha(*pDest, GET_PIXEL(xPos++, yRow), (BYTE)dwOpacity);
							}
						}
					}
				}

			//	Rest

			END_DRAW();
			m_pDest = NULL;
			m_rcClip = NULL;
			}

		virtual void Draw (CG32bitImage &Dest, int x, int y, const CG16bitBinaryRegion &Region)
			{
			if (Region.GetCount() == 0)
				return;

			m_pDest = &Dest;
			m_rcClip = &Dest.GetClipRect();
			m_xDest = x;
			m_yDest = y;

			//	Give our specializations a chance to initialize

			if (!BEGIN_DRAW())
				{
				m_pDest = NULL;
				m_rcClip = NULL;
				return;
				}

			//	Paint each pixel in the region

			const CG16bitBinaryRegion::SRun *pRun = &Region.GetRun(0);
			const CG16bitBinaryRegion::SRun *pRunEnd = pRun + Region.GetCount();
			for (; pRun < pRunEnd; pRun++)
				{
				int yAdj = m_yDest + pRun->y;
				int xStartAdj = m_xDest + pRun->xStart;
				int xEndAdj = m_xDest + pRun->xEnd;

				//	Make sure this row is in range

				if (yAdj < m_rcClip->top || yAdj >= m_rcClip->bottom
						|| xEndAdj <= m_rcClip->left || xStartAdj >= m_rcClip->right)
					continue;

				int xStart = Max(xStartAdj, (int)m_rcClip->left);
				int xEnd = Min(xEndAdj, (int)m_rcClip->right);
				int xPos = xStart - m_xDest;

				//	Get the pointers

				CG32bitPixel *pRow = Dest.GetPixelPos(0, yAdj);
				CG32bitPixel *pPos = pRow + xStart;
				CG32bitPixel *pEnd = pRow + xEnd;

				//	Fill

				while (pPos < pEnd)
					*pPos++ = BLENDER::Blend(*pPos, GET_PIXEL(xPos++, pRun->y));
				}

			//	Rest

			END_DRAW();
			m_pDest = NULL;
			m_rcClip = NULL;
			}

		//	Default implementation

		inline bool BeginDraw (void) { return true; }
		inline void EndDraw (void) { }

	protected:

		inline bool BEGIN_DRAW (void) { return ((PAINTER *)this)->BeginDraw(); }
		inline void END_DRAW (void) { ((PAINTER *)this)->EndDraw(); }
		inline CG32bitPixel GET_PIXEL (int x, int y) { return ((PAINTER *)this)->GetPixelAt(x, y); }

		CG32bitImage *m_pDest;
		const RECT *m_rcClip;
		int m_xDest;
		int m_yDest;
	};
