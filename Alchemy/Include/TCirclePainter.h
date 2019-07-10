//	TCirclePainter.h
//
//	TCirclePainter template
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class ICirclePainter
	{
	public:
		virtual ~ICirclePainter (void) { }

		virtual void Draw (CG32bitImage &Dest, int xCenter, int yCenter, int iRadius = -1, int iFrame = 0) = 0;
		virtual void SetParam (const CString &sParam, const TArray<CG32bitPixel> &ColorTable) { }
	};

template <class PAINTER, class BLENDER> class TCirclePainter32 : public ICirclePainter
	{
	public:

		virtual void Draw (CG32bitImage &Dest, int xCenter, int yCenter, int iRadius = -1, int iFrame = 0)
			{
			m_pDest = &Dest;
			m_rcClip = &Dest.GetClipRect();
			m_xDest = xCenter;
			m_yDest = yCenter;
			if (iRadius != -1)
				m_iRadius = iRadius;
			m_iFrame = iFrame;

			//	Give our specializations a chance to initialize

			if (!BEGIN_DRAW())
				{
				m_pDest = NULL;
				m_rcClip = NULL;
				return;
				}

			//	Draw

			DrawCircle();

			//	Rest

			END_DRAW();
			m_pDest = NULL;
			m_rcClip = NULL;
			}

		//	Default implementation

		bool BeginDraw (void) { return true; }
		void EndDraw (void) { }

	protected:

		TCirclePainter32 (int iAngleRange = 360, int iRadius = 100) : 
				m_iAngleRange(iAngleRange),
				m_iRadius(iRadius),
				m_pDest(NULL)
			{
			}

		void DrawCircle (void)
			{
			//	Deal with edge-conditions

			if (m_iRadius <= 0)
				return;

			//	Initialize some stuff

			int x = 0;
			int y = m_iRadius;
			int d = 1 - m_iRadius;
			int deltaE = 3;
			int deltaSE = -2 * m_iRadius + 5;

			//	Draw central line

			DrawLine(m_iRadius, 0);

			//	Draw lines above and below the center

			int iLastDraw = -1;
			while (y > x)
				{
				if (d < 0)
					{
					d += deltaE;
					deltaE += 2;
					deltaSE += 2;
					}
				else
					{
					d += deltaSE;
					deltaE += 2;
					deltaSE += 4;

					//	Draw lines

					DrawLine(x, y);
					iLastDraw = y;

					//	Next

					y--;
					}

				x++;

				//	Draw lines

				if (x != iLastDraw)
					DrawLine(y, x);
				}
			}

		void DrawLine (int x, int y)

		//	DrawLine
		//
		//	The calculation of the radius uses a modified version of the algorithm
		//	described in page 84 of Foley and van Dam. But because we are computing the
		//	radius as we advance x, the function that we are computing is:
		//
		//	F(x,r) = x^2 - r^2 + Y^2
		//
		//	In which Y^2 is constant (the y coordinate of this line).
		//
		//	The solution leads to the two decision functions:
		//
		//	deltaE = 2xp + 3
		//	deltaSE = 2xp - 2r + 1
		//
		//	The calculation of the angle relies on a fast arctangent approximation
		//	on page 389 of Graphic Gems II.

			{
			int xStart = m_xDest - x;
			int xEnd = m_xDest + x + 1;

			if (xEnd <= m_rcClip->left || xStart >= m_rcClip->right)
				return;

			//	See which lines we need to paint

			int yLine = m_yDest - y;
			bool bPaintTop = (yLine >= m_rcClip->top && yLine < m_rcClip->bottom);
			CG32bitPixel *pCenterTop = m_pDest->GetPixelPos(m_xDest, yLine);

			yLine = m_yDest + y;
			bool bPaintBottom = ((y > 0) && (yLine >= m_rcClip->top && yLine < m_rcClip->bottom));
			CG32bitPixel *pCenterBottom = m_pDest->GetPixelPos(m_xDest, yLine);

			//	Compute radius increment

			int iRadius = y;
			int d = -y;
			int deltaE = 3;
			int deltaSE = -2 * y + 1;

			//	Compute angle increment

			const int iFixedPoint = 8192;
			int iAngle = 2 * iFixedPoint;
			int angle1 = (y == 0 ? (2 * iFixedPoint) : (iFixedPoint / y));
			int num1 = iFixedPoint * y;

			int cxSrcQuadrant = m_iAngleRange / 4;
			int cxSrcHalf = m_iAngleRange / 2;

			//	Loop

			int xPos = 0;

			//	Paint the center point (this is a special case)

			if (y == 0)
				{
				if (m_xDest < m_rcClip->right
						&& m_xDest >= m_rcClip->left
						&& bPaintTop
						&& m_iRadius > 0)
					{
					BLENDER::SetBlendPreMult(pCenterTop, GET_COLOR(0, 0));
					}

				//	Continue

				xPos = 1;
				d += deltaSE;
				deltaE += 2;
				iRadius++;
				}

			//	Blt the line 

			while (xPos <= x)
				{
				//	Figure out the radius of the pixel at this location

				if (d < 0)
					{
					d += deltaE;
					deltaE += 2;
					deltaSE += 2;
					}
				else
					{
					d += deltaSE;
					deltaE += 2;
					//  deltaSE += 0;
					iRadius++;
					}

				//	Compute the transparency based on the radius. If we're outside the
				//	radius, then skip

				if (iRadius >= m_iRadius)
					{
					xPos++;
					continue;
					}

				//	Figure out the angle of the pixel at this location

				if (xPos < y)
					iAngle -= angle1;
				else
					iAngle = num1 / xPos;

				int xOffset = (iAngle * cxSrcQuadrant / (2 * iFixedPoint));

				//	Paint

				if (m_xDest - xPos < m_rcClip->right && m_xDest - xPos >= m_rcClip->left)
					{
					if (bPaintTop)
						{
						CG32bitPixel rgbColor = GET_COLOR(cxSrcHalf - xOffset, iRadius);
						BYTE byOpacity = rgbColor.GetAlpha();

						if (byOpacity == 0x00)
							;
						else if (byOpacity == 0xff)
							BLENDER::SetCopy(pCenterTop - xPos, rgbColor);
						else
							BLENDER::SetBlendPreMult(pCenterTop - xPos, rgbColor);
						}

					if (bPaintBottom)
						{
						CG32bitPixel rgbColor = GET_COLOR(cxSrcHalf + xOffset, iRadius);
						BYTE byOpacity = rgbColor.GetAlpha();

						if (byOpacity == 0x00)
							;
						else if (byOpacity == 0xff)
							BLENDER::SetCopy(pCenterBottom - xPos, rgbColor);
						else
							BLENDER::SetBlendPreMult(pCenterBottom - xPos, rgbColor);
						}
					}

				if (xPos > 0 && m_xDest + xPos < m_rcClip->right && m_xDest + xPos >= m_rcClip->left)
					{
					if (bPaintTop)
						{
						CG32bitPixel rgbColor = GET_COLOR(xOffset, iRadius);
						BYTE byOpacity = rgbColor.GetAlpha();

						if (byOpacity == 0x00)
							;
						else if (byOpacity == 0xff)
							BLENDER::SetCopy(pCenterTop + xPos, rgbColor);
						else
							BLENDER::SetBlendPreMult(pCenterTop + xPos, rgbColor);
						}

					if (bPaintBottom)
						{
						CG32bitPixel rgbColor = GET_COLOR((m_iAngleRange - 1) - xOffset, iRadius);
						BYTE byOpacity = rgbColor.GetAlpha();

						if (byOpacity == 0x00)
							;
						else if (byOpacity == 0xff)
							BLENDER::SetCopy(pCenterBottom + xPos, rgbColor);
						else
							BLENDER::SetBlendPreMult(pCenterBottom + xPos, rgbColor);
						}
					}

				xPos++;
				}
			}

		void DRAW_PIXEL (CG32bitPixel *pPos, CG32bitPixel rgbColor)
			{
			BYTE byOpacity = rgbColor.GetAlpha();
			BYTE *pAlphaInv = CG32bitPixel::AlphaTable(byOpacity ^ 0xff);

			WORD wRed = rgbColor.GetRed();
			WORD wGreen = rgbColor.GetGreen();
			WORD wBlue = rgbColor.GetBlue();

			BYTE byRedResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pPos->GetRed()] + wRed));
			BYTE byGreenResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pPos->GetGreen()] + wGreen));
			BYTE byBlueResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pPos->GetBlue()] + wBlue));

			*pPos = CG32bitPixel(byRedResult, byGreenResult, byBlueResult);
			}

		CG32bitPixel GET_COLOR (int iAngle, int iRadius) { return ((PAINTER *)this)->GetColorAt(iAngle, iRadius); }

		bool BEGIN_DRAW (void) { return ((PAINTER *)this)->BeginDraw(); }
		void END_DRAW (void) { ((PAINTER *)this)->EndDraw(); }

		CG32bitImage *m_pDest;
		const RECT *m_rcClip;
		int m_iAngleRange;
		int m_iRadius;
		int m_iFrame;
		int m_xDest;
		int m_yDest;
	};

template <class PAINTER> class TRadialPainter8
	{
	public:

		void Draw (CG8bitImage &Dest, int xCenter, int yCenter)
			{
			m_pDest = &Dest;
			m_rcClip = &Dest.GetClipRect();
			m_xDest = xCenter;
			m_yDest = yCenter;

			DrawCircle();

			m_pDest = NULL;
			m_rcClip = NULL;
			}

	protected:

		TRadialPainter8 (int iRadius) : 
				m_iRadius(iRadius)
			{
			}

		void DrawCircle (void)
			{
			//	Deal with edge-conditions

			if (m_iRadius <= 0)
				return;

			//	Initialize some stuff

			int x = 0;
			int y = m_iRadius;
			int d = 1 - m_iRadius;
			int deltaE = 3;
			int deltaSE = -2 * m_iRadius + 5;

			//	Draw central line

			DrawLine(m_iRadius, 0);

			//	Draw lines above and below the center

			int iLastDraw = -1;
			while (y > x)
				{
				if (d < 0)
					{
					d += deltaE;
					deltaE += 2;
					deltaSE += 2;
					}
				else
					{
					d += deltaSE;
					deltaE += 2;
					deltaSE += 4;

					//	Draw lines

					DrawLine(x, y);
					iLastDraw = y;

					//	Next

					y--;
					}

				x++;

				//	Draw lines

				if (x != iLastDraw)
					DrawLine(y, x);
				}
			}

		void DrawLine (int x, int y)

		//	DrawLine
		//
		//	Draws the filled circle

			{
			int xStart = m_xDest - x;
			int xEnd = m_xDest + x + 1;

			if (xEnd <= m_rcClip->left || xStart >= m_rcClip->right)
				return;

			//	See which lines we need to paint

			int yLine = m_yDest - y;
			bool bPaintTop = (yLine >= m_rcClip->top && yLine < m_rcClip->bottom);
			BYTE *pCenterTop = m_pDest->GetPixelPos(m_xDest, yLine);

			yLine = m_yDest + y;
			bool bPaintBottom = ((y > 0) && (yLine >= m_rcClip->top && yLine < m_rcClip->bottom));
			BYTE *pCenterBottom = m_pDest->GetPixelPos(m_xDest, yLine);

			//	Compute radius increment

			int iRadius = y;
			int d = -y;
			int deltaE = 3;
			int deltaSE = -2 * y + 1;

			//	Loop

			int xPos = 0;

			//	Paint the center point (this is a special case)

			if (y == 0)
				{
				if (m_xDest < m_rcClip->right
						&& m_xDest >= m_rcClip->left
						&& bPaintTop
						&& m_iRadius > 0)
					{
					*pCenterTop = GET_COLOR(0);
					}

				//	Continue

				xPos = 1;
				d += deltaSE;
				deltaE += 2;
				iRadius++;
				}

			//	Blt the line 

			while (xPos <= x)
				{
				//	Figure out the radius of the pixel at this location

				if (d < 0)
					{
					d += deltaE;
					deltaE += 2;
					deltaSE += 2;
					}
				else
					{
					d += deltaSE;
					deltaE += 2;
					//  deltaSE += 0;
					iRadius++;
					}

				//	Compute the transparency based on the radius. If we're outside the
				//	radius, then skip

				if (iRadius >= m_iRadius)
					{
					xPos++;
					continue;
					}

				BYTE Value = GET_COLOR(iRadius);

				if (m_xDest - xPos < m_rcClip->right && m_xDest - xPos >= m_rcClip->left)
					{
					if (bPaintTop)
						*(pCenterTop - xPos) = Value;

					if (bPaintBottom)
						*(pCenterBottom - xPos) = Value;
					}

				if (xPos > 0 && m_xDest + xPos < m_rcClip->right && m_xDest + xPos >= m_rcClip->left)
					{
					if (bPaintTop)
						*(pCenterTop + xPos) = Value;

					if (bPaintBottom)
						*(pCenterBottom + xPos) = Value;
					}

				xPos++;
				}
			}

		BYTE GET_COLOR (int iRadius) { return ((PAINTER *)this)->GetColorAt(iRadius); }

		CG8bitImage *m_pDest;
		const RECT *m_rcClip;
		int m_iRadius;
		int m_xDest;
		int m_yDest;
	};

template <class PAINTER, class BLENDER> class TRadialPainter32
	{
	public:

		void Composite (CG32bitImage &Dest, int xCenter, int yCenter)
			{
			m_pDest = &Dest;
			m_rcClip = &Dest.GetClipRect();
			m_xDest = xCenter;
			m_yDest = yCenter;

			CompositeCircle();

			m_pDest = NULL;
			m_rcClip = NULL;
			}

		void Draw (CG32bitImage &Dest, int xCenter, int yCenter)
			{
			m_pDest = &Dest;
			m_rcClip = &Dest.GetClipRect();
			m_xDest = xCenter;
			m_yDest = yCenter;

			DrawCircle();

			m_pDest = NULL;
			m_rcClip = NULL;
			}

	protected:

		TRadialPainter32 (int iRadius) : 
				m_iRadius(iRadius)
			{
			}

		void CompositeCircle (void)
			{
			//	Deal with edge-conditions

			if (m_iRadius <= 0)
				return;

			//	Initialize some stuff

			int x = 0;
			int y = m_iRadius;
			int d = 1 - m_iRadius;
			int deltaE = 3;
			int deltaSE = -2 * m_iRadius + 5;

			//	Draw central line

			CompositeLine(m_iRadius, 0);

			//	Draw lines above and below the center

			int iLastDraw = -1;
			while (y > x)
				{
				if (d < 0)
					{
					d += deltaE;
					deltaE += 2;
					deltaSE += 2;
					}
				else
					{
					d += deltaSE;
					deltaE += 2;
					deltaSE += 4;

					//	Draw lines

					CompositeLine(x, y);
					iLastDraw = y;

					//	Next

					y--;
					}

				x++;

				//	Draw lines

				if (x != iLastDraw)
					CompositeLine(y, x);
				}
			}

		void CompositeLine (int x, int y)

		//	DrawLine
		//
		//	Draws the filled circle

			{
			int xStart = m_xDest - x;
			int xEnd = m_xDest + x + 1;

			if (xEnd <= m_rcClip->left || xStart >= m_rcClip->right)
				return;

			//	See which lines we need to paint

			int yLine = m_yDest - y;
			bool bPaintTop = (yLine >= m_rcClip->top && yLine < m_rcClip->bottom);
			CG32bitPixel *pCenterTop = m_pDest->GetPixelPos(m_xDest, yLine);

			yLine = m_yDest + y;
			bool bPaintBottom = ((y > 0) && (yLine >= m_rcClip->top && yLine < m_rcClip->bottom));
			CG32bitPixel *pCenterBottom = m_pDest->GetPixelPos(m_xDest, yLine);

			//	Compute radius increment

			int iRadius = y;
			int d = -y;
			int deltaE = 3;
			int deltaSE = -2 * y + 1;

			//	Loop

			int xPos = 0;

			//	Paint the center point (this is a special case)

			if (y == 0)
				{
				if (m_xDest < m_rcClip->right
						&& m_xDest >= m_rcClip->left
						&& bPaintTop
						&& m_iRadius > 0)
					{
					CG32bitPixel rgbColor = GET_COLOR(0);
					*pCenterTop = CG32bitPixel(CG32bitPixel::Blend(*pCenterTop, rgbColor), rgbColor.GetAlpha());
					}

				//	Continue

				xPos = 1;
				d += deltaSE;
				deltaE += 2;
				iRadius++;
				}

			//	Blt the line 

			while (xPos <= x)
				{
				//	Figure out the radius of the pixel at this location

				if (d < 0)
					{
					d += deltaE;
					deltaE += 2;
					deltaSE += 2;
					}
				else
					{
					d += deltaSE;
					deltaE += 2;
					//  deltaSE += 0;
					iRadius++;
					}

				//	Compute the transparency based on the radius. If we're outside the
				//	radius, then skip

				if (iRadius >= m_iRadius)
					{
					xPos++;
					continue;
					}

				CG32bitPixel rgbColor = GET_COLOR(iRadius);
				BYTE byOpacity;

				//	If transparent, skip

				if ((byOpacity = rgbColor.GetAlpha()) == 0x00)
					{
					//	Skip
					}
				else if (byOpacity == 0xff)
					{
					if (m_xDest - xPos < m_rcClip->right && m_xDest - xPos >= m_rcClip->left)
						{
						if (bPaintTop)
							*(pCenterTop - xPos) = rgbColor;

						if (bPaintBottom)
							*(pCenterBottom - xPos) = rgbColor;
						}

					if (xPos > 0 && m_xDest + xPos < m_rcClip->right && m_xDest + xPos >= m_rcClip->left)
						{
						if (bPaintTop)
							*(pCenterTop + xPos) = rgbColor;

						if (bPaintBottom)
							*(pCenterBottom + xPos) = rgbColor;
						}
					}
				else
					{
					BYTE *pAlphaInv = CG32bitPixel::AlphaTable(byOpacity ^ 0xff);

					//	We expect the color to already be pre-multiplied, so we don't 
					//	need to adjust by opacity.

					WORD wRed = rgbColor.GetRed();
					WORD wGreen = rgbColor.GetGreen();
					WORD wBlue = rgbColor.GetBlue();

					//	Draw transparent

#define DRAW_PIXEL(pos)	\
						{ \
						BYTE byRedResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[(pos)->GetRed()] + wRed)); \
						BYTE byGreenResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[(pos)->GetGreen()] + wGreen)); \
						BYTE byBlueResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[(pos)->GetBlue()] + wBlue)); \
						BYTE byAlphaResult = CG32bitPixel::CompositeAlpha((pos)->GetAlpha(), byOpacity); \
						\
						*(pos) = CG32bitPixel(byRedResult, byGreenResult, byBlueResult, byAlphaResult); \
						}

					if (m_xDest - xPos < m_rcClip->right && m_xDest - xPos >= m_rcClip->left)
						{
						if (bPaintTop)
							DRAW_PIXEL(pCenterTop - xPos);

						if (bPaintBottom)
							DRAW_PIXEL(pCenterBottom - xPos);
						}

					if (xPos > 0 && m_xDest + xPos < m_rcClip->right && m_xDest + xPos >= m_rcClip->left)
						{
						if (bPaintTop)
							DRAW_PIXEL(pCenterTop + xPos);

						if (bPaintBottom)
							DRAW_PIXEL(pCenterBottom + xPos);
						}
#undef DRAW_PIXEL
					}

				xPos++;
				}
			}

		void DrawCircle (void)
			{
			//	Deal with edge-conditions

			if (m_iRadius <= 0)
				return;

			//	Initialize some stuff

			int x = 0;
			int y = m_iRadius;
			int d = 1 - m_iRadius;
			int deltaE = 3;
			int deltaSE = -2 * m_iRadius + 5;

			//	Draw central line

			DrawLine(m_iRadius, 0);

			//	Draw lines above and below the center

			int iLastDraw = -1;
			while (y > x)
				{
				if (d < 0)
					{
					d += deltaE;
					deltaE += 2;
					deltaSE += 2;
					}
				else
					{
					d += deltaSE;
					deltaE += 2;
					deltaSE += 4;

					//	Draw lines

					DrawLine(x, y);
					iLastDraw = y;

					//	Next

					y--;
					}

				x++;

				//	Draw lines

				if (x != iLastDraw)
					DrawLine(y, x);
				}
			}

		void DrawLine (int x, int y)

		//	DrawLine
		//
		//	Draws the filled circle

			{
			int xStart = m_xDest - x;
			int xEnd = m_xDest + x + 1;

			if (xEnd <= m_rcClip->left || xStart >= m_rcClip->right)
				return;

			//	See which lines we need to paint

			int yLine = m_yDest - y;
			bool bPaintTop = (yLine >= m_rcClip->top && yLine < m_rcClip->bottom);
			CG32bitPixel *pCenterTop = m_pDest->GetPixelPos(m_xDest, yLine);

			yLine = m_yDest + y;
			bool bPaintBottom = ((y > 0) && (yLine >= m_rcClip->top && yLine < m_rcClip->bottom));
			CG32bitPixel *pCenterBottom = m_pDest->GetPixelPos(m_xDest, yLine);

			//	Compute radius increment

			int iRadius = y;
			int d = -y;
			int deltaE = 3;
			int deltaSE = -2 * y + 1;

			//	Loop

			int xPos = 0;

			//	Paint the center point (this is a special case)

			if (y == 0)
				{
				if (m_xDest < m_rcClip->right
						&& m_xDest >= m_rcClip->left
						&& bPaintTop
						&& m_iRadius > 0)
					{
					BLENDER::SetBlendPreMult(pCenterTop, GET_COLOR(0));
					}

				//	Continue

				xPos = 1;
				d += deltaSE;
				deltaE += 2;
				iRadius++;
				}

			//	Blt the line 

			while (xPos <= x)
				{
				//	Figure out the radius of the pixel at this location

				if (d < 0)
					{
					d += deltaE;
					deltaE += 2;
					deltaSE += 2;
					}
				else
					{
					d += deltaSE;
					deltaE += 2;
					//  deltaSE += 0;
					iRadius++;
					}

				//	Compute the transparency based on the radius. If we're outside the
				//	radius, then skip

				if (iRadius >= m_iRadius)
					{
					xPos++;
					continue;
					}

				CG32bitPixel rgbColor = GET_COLOR(iRadius);
				BYTE byOpacity = rgbColor.GetAlpha();

				//	If transparent, skip

				if (byOpacity == 0x00)
					{
					//	Skip
					}
				else if (byOpacity == 0xff)
					{
					if (m_xDest - xPos < m_rcClip->right && m_xDest - xPos >= m_rcClip->left)
						{
						if (bPaintTop)
							BLENDER::SetCopy(pCenterTop - xPos, rgbColor);

						if (bPaintBottom)
							BLENDER::SetCopy(pCenterBottom - xPos, rgbColor);
						}

					if (xPos > 0 && m_xDest + xPos < m_rcClip->right && m_xDest + xPos >= m_rcClip->left)
						{
						if (bPaintTop)
							BLENDER::SetCopy(pCenterTop + xPos, rgbColor);

						if (bPaintBottom)
							BLENDER::SetCopy(pCenterBottom + xPos, rgbColor);
						}
					}
				else
					{
					if (m_xDest - xPos < m_rcClip->right && m_xDest - xPos >= m_rcClip->left)
						{
						if (bPaintTop)
							BLENDER::SetBlendPreMult(pCenterTop - xPos, rgbColor);

						if (bPaintBottom)
							BLENDER::SetBlendPreMult(pCenterBottom - xPos, rgbColor);
						}

					if (xPos > 0 && m_xDest + xPos < m_rcClip->right && m_xDest + xPos >= m_rcClip->left)
						{
						if (bPaintTop)
							BLENDER::SetBlendPreMult(pCenterTop + xPos, rgbColor);

						if (bPaintBottom)
							BLENDER::SetBlendPreMult(pCenterBottom + xPos, rgbColor);
						}
					}

				xPos++;
				}
			}

		CG32bitPixel GET_COLOR (int iRadius) { return ((PAINTER *)this)->GetColorAt(iRadius); }

		CG32bitImage *m_pDest;
		const RECT *m_rcClip;
		int m_iRadius;
		int m_xDest;
		int m_yDest;
	};

