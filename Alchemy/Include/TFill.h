//	TFill.h
//
//	TFill Template
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class IFillPainter
	{
	public:
		virtual ~IFillPainter (void) { }

		virtual void Fill (CG32bitImage &Dest, int x, int y, int cxWidth, int cyHeight) { }
		virtual void FillColumn (CG32bitImage &Dest, int x, int y, int cyHeight) { }
		virtual void FillLine (CG32bitImage &Dest, int x, int y, int cxWidth) { }
		virtual void FillPixel (CG32bitImage &Dest, int x, int y) { }
		virtual void FillPixelTrans (CG32bitImage &Dest, int x, int y, BYTE byOpacity) { }
	};

template <class FILL, class BLENDER> class TFillPainter32 : public IFillPainter
	{
	public:
		TFillPainter32 (FILL &Fill) : m_Fill(Fill)
			{ }

		virtual void Fill (CG32bitImage &Dest, int x, int y, int cxWidth, int cyHeight) override
			{
			//	Make sure we're in bounds

			if (!Dest.AdjustCoords(NULL, NULL, 0, 0, 
					&x, &y, 
					&cxWidth, &cyHeight))
				return;

			CG32bitPixel *pDestRow = Dest.GetPixelPos(x, y);
			CG32bitPixel *pDestRowEnd = Dest.GetPixelPos(x, y + cyHeight);

			while (pDestRow < pDestRowEnd)
				{
				CG32bitPixel *pDest = pDestRow;
				CG32bitPixel *pDestEnd = pDestRow + cxWidth;

				while (pDest < pDestEnd)
					{
					BLENDER::SetBlend(pDest, m_Fill.GetColor());
					pDest++;
					}

				pDestRow = Dest.NextRow(pDestRow);
				}
			}

		virtual void FillColumn (CG32bitImage &Dest, int x, int y, int cyHeight) override
			{
			//	Make sure we're in bounds

			int cxWidth = 1;
			if (!Dest.AdjustCoords(NULL, NULL, 0, 0, 
					&x, &y, 
					&cxWidth, &cyHeight))
				return;

			CG32bitPixel *pDestRow = Dest.GetPixelPos(x, y);
			CG32bitPixel *pDestRowEnd = Dest.GetPixelPos(x, y + cyHeight);

			while (pDestRow < pDestRowEnd)
				{
				BLENDER::SetBlend(pDestRow, m_Fill.GetColor());

				pDestRow = Dest.NextRow(pDestRow);
				}
			}

		virtual void FillLine (CG32bitImage &Dest, int x, int y, int cxWidth) override
			{
			//	Make sure we're in bounds

			int cyHeight = 1;
			if (!Dest.AdjustCoords(NULL, NULL, 0, 0, 
					&x, &y, 
					&cxWidth, &cyHeight))
				return;

			CG32bitPixel *pDest = Dest.GetPixelPos(x, y);
			CG32bitPixel *pDestEnd = pDest + cxWidth;

			while (pDest < pDestEnd)
				{
				BLENDER::SetBlend(pDest, m_Fill.GetColor());
				pDest++;
				}
			}

		virtual void FillPixel (CG32bitImage &Dest, int x, int y) override
			{
			const RECT &rcClip = Dest.GetClipRect();
			if (x >= rcClip.left && y >= rcClip.top && x < rcClip.right && y < rcClip.bottom)
				BLENDER::SetBlend(Dest.GetPixelPos(x, y), m_Fill.GetColor());
			}

		virtual void FillPixelTrans (CG32bitImage &Dest, int x, int y, BYTE byOpacity) override
			{
			const RECT &rcClip = Dest.GetClipRect();
			if (x >= rcClip.left && y >= rcClip.top && x < rcClip.right && y < rcClip.bottom)
				BLENDER::SetBlendAlpha(Dest.GetPixelPos(x, y), m_Fill.GetColor(), byOpacity);
			}

	protected:
		FILL &m_Fill;
	};
