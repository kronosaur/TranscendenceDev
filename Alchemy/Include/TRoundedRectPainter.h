//	TRoundedRectPainter.h
//
//	TRoundedRectPainter template
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class IRoundedRectPainter
	{
	public:
		virtual ~IRoundedRectPainter (void) { }

		virtual void Draw (CG32bitImage &Dest, int x, int y, int cxWidth, int cyHeight, int iCornerRadius) = 0;
	};

template <class FILL, class BLENDER> class TRoundedRectPainter32 : public IRoundedRectPainter
	{
	public:
		TRoundedRectPainter32 (FILL &Fill) :
				m_Fill(Fill)
			{ }

		virtual void Draw (CG32bitImage &Dest, int x, int y, int cxWidth, int cyHeight, int iCornerRadius) override
			{
			int i;
			TFillPainter32<FILL, BLENDER> Painter(m_Fill);

			if (iCornerRadius <= 0)
				{
				Painter.Fill(Dest, x, y, cxWidth, cyHeight);
				return;
				}

			//	Generate a set of raster lines for the corner

			int *pSolid = new int [iCornerRadius];
			BYTE *pEdge = new BYTE [iCornerRadius];
			RasterizeQuarterCircle8bit(iCornerRadius, pSolid, pEdge, m_Fill.GetColor().GetAlpha());

			//	Fill in each corner

			for (i = 0; i < iCornerRadius; i++)
				{
				int xOffset = iCornerRadius - pSolid[i];
				int cxLine = cxWidth - (iCornerRadius * 2) + (pSolid[i] * 2);

				//	Top edge

				Painter.FillLine(Dest, x + xOffset, y + i, cxLine);
				Painter.FillPixelTrans(Dest, x + xOffset - 1, y + i, pEdge[i]);
				Painter.FillPixelTrans(Dest, x + cxWidth - xOffset, y + i, pEdge[i]);

				//	Bottom edge

				Painter.FillLine(Dest, x + xOffset, y + cyHeight - i - 1, cxLine);
				Painter.FillPixelTrans(Dest, x + xOffset - 1, y + cyHeight - i - 1, pEdge[i]);
				Painter.FillPixelTrans(Dest, x + cxWidth - xOffset, y + cyHeight - i - 1, pEdge[i]);
				}

			//	Fill the center

			Painter.Fill(Dest, x, y + iCornerRadius, cxWidth, (cyHeight - 2 * iCornerRadius));

			//	Done

			delete [] pSolid;
			delete [] pEdge;
			}

	protected:
		FILL &m_Fill;
	};


