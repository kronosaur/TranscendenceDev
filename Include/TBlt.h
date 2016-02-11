//	TBlt.h
//
//	TBlt template
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class IImagePainter
	{
	public:
		virtual ~IImagePainter (void) { }

		virtual void Blt (CG32bitImage &Dest, int xDest, int yDest, const CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc) = 0;
		virtual void BltMask (CG32bitImage &Dest, int xDest, int yDest, const CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc, CG8bitImage &Mask) = 0;
		virtual void BltTransformed (CG32bitImage &Dest, const CVector &vPos, const CVector &vScale, Metric rRotation, const CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc) = 0;

		virtual void FillMaskTransformed (CG32bitImage &Dest, const CVector &vPos, const CVector &vScale, Metric rRotation, const CG8bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc) = 0;
	};

template <class PAINTER, class BLENDER> class TImagePainter : public IImagePainter
	{
	public:
		virtual void Blt (CG32bitImage &Dest, int xDest, int yDest, const CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc)
			{
			//	Make sure we're in bounds

			if (!Dest.AdjustCoords(&xSrc, &ySrc, Src.GetWidth(), Src.GetHeight(), 
					&xDest, &yDest,
					&cxSrc, &cySrc))
				return;

			CG32bitPixel *pSrcRow = Src.GetPixelPos(xSrc, ySrc);
			CG32bitPixel *pSrcRowEnd = Src.GetPixelPos(xSrc, ySrc + cySrc);
			CG32bitPixel *pDestRow = Dest.GetPixelPos(xDest, yDest);

			while (pSrcRow < pSrcRowEnd)
				{
				CG32bitPixel *pSrc = pSrcRow;
				CG32bitPixel *pSrcEnd = pSrc + cxSrc;
				CG32bitPixel *pDest = pDestRow;

				START_ROW(pSrc, pDest);

				while (pSrc < pSrcEnd)
					{
					CG32bitPixel rgbResult = FILTER(*pSrc, pDest);
					BYTE byAlpha = rgbResult.GetAlpha();

					if (byAlpha == 0x00)
						;
					else if (byAlpha == 0xff)
						*pDest = BLENDER::Copy(*pDest, rgbResult);
					else
						*pDest = BLENDER::Blend(*pDest, rgbResult);

					pDest++;
					pSrc++;
					}

				pSrcRow = Src.NextRow(pSrcRow);
				pDestRow = Dest.NextRow(pDestRow);
				}
			}

		virtual void BltMask (CG32bitImage &Dest, int xDest, int yDest, const CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc, CG8bitImage &Mask)
			{
			//	Make sure we're in bounds

			if (!Dest.AdjustCoords(&xSrc, &ySrc, Src.GetWidth(), Src.GetHeight(), 
					&xDest, &yDest,
					&cxSrc, &cySrc))
				return;

			//	NOTE: We expect the Mask is the same size as the destination,
			//	but we adjust coordinates just in case.

			if (!Mask.AdjustCoords(&xSrc, &ySrc, Src.GetWidth(), Src.GetHeight(),
					&xDest, &yDest,
					&cxSrc, &cySrc))
				return;

			//	Do the blt

			CG32bitPixel *pSrcRow = Src.GetPixelPos(xSrc, ySrc);
			CG32bitPixel *pSrcRowEnd = Src.GetPixelPos(xSrc, ySrc + cySrc);
			CG32bitPixel *pDestRow = Dest.GetPixelPos(xDest, yDest);
			BYTE *pAlphaRow = Mask.GetPixelPos(xDest, yDest);

			while (pSrcRow < pSrcRowEnd)
				{
				CG32bitPixel *pSrcPos = pSrcRow;
				CG32bitPixel *pSrcPosEnd = pSrcRow + cxSrc;
				CG32bitPixel *pDestPos = pDestRow;
				BYTE *pAlphaPos = pAlphaRow;

				START_ROW(pSrcPos, pDestPos);

				while (pSrcPos < pSrcPosEnd)
					{
					BYTE byAlpha = *pAlphaPos;

					if (byAlpha == 0)
						;
					else
						{
						CG32bitPixel rgbSrc = FILTER(*pSrcPos, pDestPos);

						if (rgbSrc.GetAlpha() == 0)
							;
						else if (byAlpha == 0xff)
							{
							if (rgbSrc.GetAlpha() == 0xff)
								*pDestPos = BLENDER::Copy(*pDestPos, rgbSrc);
							else
								*pDestPos = BLENDER::Blend(*pDestPos, rgbSrc);
							}
						else
							{
							if (rgbSrc.GetAlpha() == 0xff)
								*pDestPos = BLENDER::Blend(*pDestPos, CG32bitPixel(rgbSrc, byAlpha));
							else
								*pDestPos = BLENDER::BlendAlpha(*pDestPos, rgbSrc, byAlpha);
							}
						}

					pDestPos++;
					pSrcPos++;
					pAlphaPos++;
					}

				pSrcRow = Src.NextRow(pSrcRow);
				pDestRow = Dest.NextRow(pDestRow);
				pAlphaRow = Mask.NextRow(pAlphaRow);
				}
			}

		virtual void BltTransformed (CG32bitImage &Dest, const CVector &vPos, const CVector &vScale, Metric rRotation, const CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc)
			{
			BltTransformed(Dest, vPos.GetX(), vPos.GetY(), vScale.GetX(), vScale.GetY(), rRotation, Src, xSrc, ySrc, cxSrc, cySrc);
			}

		void BltTransformed (CG32bitImage &Dest, Metric rX, Metric rY, Metric rScaleX, Metric rScaleY, Metric rRotation, const CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc)
			{
			CXForm SrcToDest;
			CXForm DestToSrc;
			RECT rcDest;
			if (!CalcBltTransform(rX, rY, rScaleX, rScaleY, mathRadiansToDegrees(rRotation), xSrc, ySrc, cxSrc, cySrc, &SrcToDest, &DestToSrc, &rcDest))
				return;

			//	Bounds check on the destination

			int cxDest = RectWidth(rcDest);
			int cyDest = RectHeight(rcDest);

			int xDest = rcDest.left;
			if (xDest < 0)
				{
				cxDest += xDest;
				xDest = 0;
				}

			int yDest = rcDest.top;
			if (yDest < 0)
				{
				cyDest += yDest;
				yDest = 0;
				}

			if (xDest + cxDest > Dest.GetWidth())
				cxDest = Dest.GetWidth() - xDest;

			if (yDest + cyDest > Dest.GetHeight())
				cyDest = Dest.GetHeight() - yDest;

			if (cxDest <= 0 || cyDest <= 0)
				return;

			int xSrcEnd = xSrc + cxSrc;
			int ySrcEnd = ySrc + cySrc;

			//	Compute vectors that move us by 1 pixel

			CVector vOrigin = DestToSrc.Transform(CVector(0.0, 0.0));
			CVector vIncX = DestToSrc.Transform(CVector(1.0, 0.0)) - vOrigin;
			CVector vIncY = DestToSrc.Transform(CVector(0.0, 1.0)) - vOrigin;

			int iRowHeight = Src.GetPixelPos(0, 1) - Src.GetPixelPos(0, 0);

			//	Loop over every pixel in the destination

			CVector vSrcRow = DestToSrc.Transform(CVector(xDest, yDest));
			CG32bitPixel *pDestRow = Dest.GetPixelPos(xDest, yDest);
			CG32bitPixel *pDestRowEnd = Dest.GetPixelPos(xDest, yDest + cyDest);
			while (pDestRow < pDestRowEnd)
				{
				CVector vSrcPos = vSrcRow;
				CG32bitPixel *pDestPos = pDestRow;
				CG32bitPixel *pDestPosEnd = pDestRow + cxDest;
				while (pDestPos < pDestPosEnd)
					{
					int xSrcPos = (int)vSrcPos.GetX();
					int ySrcPos = (int)vSrcPos.GetY();

					if (xSrcPos >= xSrc && xSrcPos + 1 < xSrcEnd
							&& ySrcPos >= ySrc && ySrcPos + 1< ySrcEnd)
						{
						CG32bitPixel *pSrcPos = Src.GetPixelPos(xSrcPos, ySrcPos);

						CG32bitPixel rgbA = FILTER(*pSrcPos, pDestPos);
						BYTE byAlpha = rgbA.GetAlpha();

						if (byAlpha == 0x00)
							NULL;
						else
							{
							CG32bitPixel rgbB = FILTER(*(pSrcPos + iRowHeight), pDestPos);
							CG32bitPixel rgbC = FILTER(*(pSrcPos + 1), pDestPos);
							CG32bitPixel rgbD = FILTER(*(pSrcPos + iRowHeight + 1), pDestPos);

							Metric xf = vSrcPos.GetX() - (Metric)(xSrcPos);
							Metric yf = vSrcPos.GetY() - (Metric)(ySrcPos);

							Metric ka = (1.0 - xf) * (1.0 - yf);
							Metric kb = (1.0 - xf) * yf;
							Metric kc = xf * (1.0 - yf);
							Metric kd = xf * yf;

							DWORD red = (DWORD)(ka * rgbA.GetRed()
									+ kb * rgbB.GetRed()
									+ kc * rgbC.GetRed()
									+ kd * rgbD.GetRed());

							DWORD green = (DWORD)(ka * rgbA.GetGreen()
									+ kb * rgbB.GetGreen()
									+ kc * rgbC.GetGreen()
									+ kd * rgbD.GetGreen());

							DWORD blue = (DWORD)(ka * rgbA.GetBlue()
									+ kb * rgbB.GetBlue()
									+ kc * rgbC.GetBlue()
									+ kd * rgbD.GetBlue());

							*pDestPos = BLENDER::Blend(*pDestPos, CG32bitPixel((BYTE)red, (BYTE)green, (BYTE)blue, byAlpha));
							}
						}

					//	Next

					vSrcPos = vSrcPos + vIncX;
					pDestPos++;
					}

				//	Next row

				vSrcRow = vSrcRow + vIncY;
				pDestRow = Dest.NextRow(pDestRow);
				}
			}

		virtual void FillMaskTransformed (CG32bitImage &Dest, const CVector &vPos, const CVector &vScale, Metric rRotation, const CG8bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc)
			{
			FillMaskTransformed(Dest, vPos.GetX(), vPos.GetY(), vScale.GetX(), vScale.GetY(), rRotation, Src, xSrc, ySrc, cxSrc, cySrc);
			}

		void FillMaskTransformed (CG32bitImage &Dest, Metric rX, Metric rY, Metric rScaleX, Metric rScaleY, Metric rRotation, const CG8bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc)
			{
			CXForm SrcToDest;
			CXForm DestToSrc;
			RECT rcDest;
			if (!CalcBltTransform(rX, rY, rScaleX, rScaleY, mathRadiansToDegrees(rRotation), xSrc, ySrc, cxSrc, cySrc, &SrcToDest, &DestToSrc, &rcDest))
				return;

			//	Bounds check on the destination

			int cxDest = RectWidth(rcDest);
			int cyDest = RectHeight(rcDest);

			int xDest = rcDest.left;
			if (xDest < 0)
				{
				cxDest += xDest;
				xDest = 0;
				}

			int yDest = rcDest.top;
			if (yDest < 0)
				{
				cyDest += yDest;
				yDest = 0;
				}

			if (xDest + cxDest > Dest.GetWidth())
				cxDest = Dest.GetWidth() - xDest;

			if (yDest + cyDest > Dest.GetHeight())
				cyDest = Dest.GetHeight() - yDest;

			if (cxDest <= 0 || cyDest <= 0)
				return;

			int xSrcEnd = xSrc + cxSrc;
			int ySrcEnd = ySrc + cySrc;

			//	Compute vectors that move us by 1 pixel

			CVector vOrigin = DestToSrc.Transform(CVector(0.0, 0.0));
			CVector vIncX = DestToSrc.Transform(CVector(1.0, 0.0)) - vOrigin;
			CVector vIncY = DestToSrc.Transform(CVector(0.0, 1.0)) - vOrigin;

			int iRowHeight = Src.GetPixelPos(0, 1) - Src.GetPixelPos(0, 0);

			//	Loop over every pixel in the destination

			CVector vSrcRow = DestToSrc.Transform(CVector(xDest, yDest));
			CG32bitPixel *pDestRow = Dest.GetPixelPos(xDest, yDest);
			CG32bitPixel *pDestRowEnd = Dest.GetPixelPos(xDest, yDest + cyDest);
			while (pDestRow < pDestRowEnd)
				{
				CVector vSrcPos = vSrcRow;
				CG32bitPixel *pDestPos = pDestRow;
				CG32bitPixel *pDestPosEnd = pDestRow + cxDest;
				while (pDestPos < pDestPosEnd)
					{
					int xSrcPos = (int)vSrcPos.GetX();
					int ySrcPos = (int)vSrcPos.GetY();

					if (xSrcPos >= xSrc && xSrcPos + 1 < xSrcEnd
							&& ySrcPos >= ySrc && ySrcPos + 1< ySrcEnd)
						{
						BYTE *pSrcPos = Src.GetPixelPos(xSrcPos, ySrcPos);

						BYTE byA = *pSrcPos;
						BYTE byB = *(pSrcPos + iRowHeight);
						BYTE byC = *(pSrcPos + 1);
						BYTE byD = *(pSrcPos + iRowHeight + 1);

						Metric xf = vSrcPos.GetX() - (Metric)(xSrcPos);
						Metric yf = vSrcPos.GetY() - (Metric)(ySrcPos);

						Metric ka = (1.0 - xf) * (1.0 - yf);
						Metric kb = (1.0 - xf) * yf;
						Metric kc = xf * (1.0 - yf);
						Metric kd = xf * yf;

						Metric rBlend = (ka * byA + kb * byB + kc * byC + kd * byD);
						BYTE byAlpha = (BYTE)(DWORD)Max(0.0, Min(rBlend, 255.0));

						if (byAlpha == 0x00)
							NULL;
						else if (byAlpha == 0xff)
							*pDestPos = BLENDER::Blend(*pDestPos, GET_PIXEL(xSrcPos, ySrcPos));
						else
							*pDestPos = BLENDER::BlendAlpha(*pDestPos, GET_PIXEL(xSrcPos, ySrcPos), byAlpha);
						}

					//	Next

					vSrcPos = vSrcPos + vIncX;
					pDestPos++;
					}

				//	Next row

				vSrcRow = vSrcRow + vIncY;
				pDestRow = Dest.NextRow(pDestRow);
				}
			}

	private:
		inline bool BEGIN_DRAW (void) { return ((PAINTER *)this)->BeginDraw(); }
		inline void END_DRAW (void) { ((PAINTER *)this)->EndDraw(); }
		inline CG32bitPixel FILTER (CG32bitPixel rgbSrc, CG32bitPixel *pDest) { return ((PAINTER *)this)->Filter(rgbSrc, pDest); }
		inline CG32bitPixel GET_PIXEL (int x, int y) { return ((PAINTER *)this)->GetPixelAt(x, y); }
		inline void START_ROW (CG32bitPixel *pSrc, CG32bitPixel *pDest) { return ((PAINTER *)this)->StartRow(pSrc, pDest); }

		//	Default implementation (for optional methods)

		inline bool BeginDraw (void) { return true; }
		inline void EndDraw (void) { }
		inline CG32bitPixel Filter (CG32bitPixel rgbSrc, CG32bitPixel *pDest) { return CG32bitPixel::Null();  }
		inline CG32bitPixel GetPixelAt (int x, int y) { return CG32bitPixel::Null(); }
		inline void StartRow (CG32bitPixel *pSrc, CG32bitPixel *pDest) { }
	};

template <class PAINTER> class TBlt
	{
	public:
		void Blt (CG32bitImage &Dest, int xDest, int yDest, const CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc)
			{
			//	Make sure we're in bounds

			if (!Dest.AdjustCoords(&xSrc, &ySrc, Src.GetWidth(), Src.GetHeight(), 
					&xDest, &yDest,
					&cxSrc, &cySrc))
				return;

			CG32bitPixel *pSrcRow = Src.GetPixelPos(xSrc, ySrc);
			CG32bitPixel *pSrcRowEnd = Src.GetPixelPos(xSrc, ySrc + cySrc);
			CG32bitPixel *pDestRow = Dest.GetPixelPos(xDest, yDest);

			while (pSrcRow < pSrcRowEnd)
				{
				CG32bitPixel *pSrc = pSrcRow;
				CG32bitPixel *pSrcEnd = pSrc + cxSrc;
				CG32bitPixel *pDest = pDestRow;

				START_ROW(pSrc, pDest);

				while (pSrc < pSrcEnd)
					{
					CG32bitPixel rgbResult = FILTER(*pSrc, pDest);
					BYTE byAlpha = rgbResult.GetAlpha();

					if (byAlpha == 0x00)
						;
					else if (byAlpha == 0xff)
						*pDest = rgbResult;
					else
						{
						BYTE *pAlphaInv = CG32bitPixel::AlphaTable(byAlpha ^ 0xff);	//	Equivalent to 255 - byAlpha

						BYTE byRedResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDest->GetRed()] + (WORD)rgbResult.GetRed()));
						BYTE byGreenResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDest->GetGreen()] + (WORD)rgbResult.GetGreen()));
						BYTE byBlueResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDest->GetBlue()] + (WORD)rgbResult.GetBlue()));

						*pDest = CG32bitPixel(byRedResult, byGreenResult, byBlueResult);
						}

					pDest++;
					pSrc++;
					}

				pSrcRow = Src.NextRow(pSrcRow);
				pDestRow = Dest.NextRow(pDestRow);
				}
			}

		void BltScaled (CG32bitImage &Dest, int xDest, int yDest, int cxDest, int cyDest, const CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc)
			{
			if (cxDest <= 0 || cyDest <= 0 || cxSrc <= 0 || cySrc <= 0)
				return;

			//	Compute the increment on the source to cover the entire destination

			Metric xSrcInc = (Metric)cxSrc / (Metric)cxDest;
			Metric ySrcInc = (Metric)cySrc / (Metric)cyDest;

			//	Make sure we're in bounds

			Metric xSrcStart = (Metric)xSrc;
			Metric ySrcStart = (Metric)ySrc;
			if (!Dest.AdjustScaledCoords(&xSrcStart, &ySrcStart, Src.GetWidth(), Src.GetHeight(), 
					xSrcInc, ySrcInc,
					&xDest, &yDest,
					&cxDest, &cyDest))
				return;

			//	Do the blt

			CG32bitPixel *pDestRow = Dest.GetPixelPos(xDest, yDest);
			CG32bitPixel *pDestRowEnd = Dest.GetPixelPos(xDest, yDest + cyDest);

			Metric y = ySrcStart;
			while (pDestRow < pDestRowEnd)
				{
				CG32bitPixel *pDestPos = pDestRow;
				CG32bitPixel *pDestPosEnd = pDestPos + cxDest;

				CG32bitPixel *pSrcRow = Src.GetPixelPos((int)xSrcStart, (int)y);
				Metric xOffset = 0.0;

				while (pDestPos < pDestPosEnd)
					{
					*pDestPos = FILTER(*(pSrcRow + (int)xOffset), pDestPos);

					pDestPos++;
					xOffset += xSrcInc;
					}

				y += ySrcInc;
				pDestRow = Dest.NextRow(pDestRow);
				}
			}

		void BltTransformed (CG32bitImage &Dest, Metric rX, Metric rY, Metric rScaleX, Metric rScaleY, Metric rRotation, const CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc)
			{
			CXForm SrcToDest;
			CXForm DestToSrc;
			RECT rcDest;
			if (!CalcBltTransform(rX, rY, rScaleX, rScaleY, rRotation, xSrc, ySrc, cxSrc, cySrc, &SrcToDest, &DestToSrc, &rcDest))
				return;

			//	Bounds check on the destination

			int cxDest = RectWidth(rcDest);
			int cyDest = RectHeight(rcDest);

			int xDest = rcDest.left;
			if (xDest < 0)
				{
				cxDest += xDest;
				xDest = 0;
				}

			int yDest = rcDest.top;
			if (yDest < 0)
				{
				cyDest += yDest;
				yDest = 0;
				}

			if (xDest + cxDest > Dest.GetWidth())
				cxDest = Dest.GetWidth() - xDest;

			if (yDest + cyDest > Dest.GetHeight())
				cyDest = Dest.GetHeight() - yDest;

			if (cxDest <= 0 || cyDest <= 0)
				return;

			int xSrcEnd = xSrc + cxSrc;
			int ySrcEnd = ySrc + cySrc;

			//	Compute vectors that move us by 1 pixel

			CVector vOrigin = DestToSrc.Transform(CVector(0.0, 0.0));
			CVector vIncX = DestToSrc.Transform(CVector(1.0, 0.0)) - vOrigin;
			CVector vIncY = DestToSrc.Transform(CVector(0.0, 1.0)) - vOrigin;

			int iRowHeight = Src.GetPixelPos(0, 1) - Src.GetPixelPos(0, 0);

			//	Loop over every pixel in the destination

			CVector vSrcRow = DestToSrc.Transform(CVector(xDest, yDest));
			CG32bitPixel *pDestRow = Dest.GetPixelPos(xDest, yDest);
			CG32bitPixel *pDestRowEnd = Dest.GetPixelPos(xDest, yDest + cyDest);
			while (pDestRow < pDestRowEnd)
				{
				CVector vSrcPos = vSrcRow;
				CG32bitPixel *pDestPos = pDestRow;
				CG32bitPixel *pDestPosEnd = pDestRow + cxDest;
				while (pDestPos < pDestPosEnd)
					{
					int xSrcPos = (int)vSrcPos.GetX();
					int ySrcPos = (int)vSrcPos.GetY();

					if (xSrcPos >= xSrc && xSrcPos + 1 < xSrcEnd
							&& ySrcPos >= ySrc && ySrcPos + 1< ySrcEnd)
						{
						CG32bitPixel *pSrcPos = Src.GetPixelPos(xSrcPos, ySrcPos);

						CG32bitPixel rgbA = FILTER(*pSrcPos, pDestPos);
						BYTE byAlpha = rgbA.GetAlpha();

						if (byAlpha == 0x00)
							NULL;
						else
							{
							CG32bitPixel rgbB = FILTER(*(pSrcPos + iRowHeight), pDestPos);
							CG32bitPixel rgbC = FILTER(*(pSrcPos + 1), pDestPos);
							CG32bitPixel rgbD = FILTER(*(pSrcPos + iRowHeight + 1), pDestPos);

							Metric xf = vSrcPos.GetX() - (Metric)(xSrcPos);
							Metric yf = vSrcPos.GetY() - (Metric)(ySrcPos);

							Metric ka = (1.0 - xf) * (1.0 - yf);
							Metric kb = (1.0 - xf) * yf;
							Metric kc = xf * (1.0 - yf);
							Metric kd = xf * yf;

							if (byAlpha == 0xff)
								{
								DWORD red = (DWORD)(ka * rgbA.GetRed()
										+ kb * rgbB.GetRed()
										+ kc * rgbC.GetRed()
										+ kd * rgbD.GetRed());

								DWORD green = (DWORD)(ka * rgbA.GetGreen()
										+ kb * rgbB.GetGreen()
										+ kc * rgbC.GetGreen()
										+ kd * rgbD.GetGreen());

								DWORD blue = (DWORD)(ka * rgbA.GetBlue()
										+ kb * rgbB.GetBlue()
										+ kc * rgbC.GetBlue()
										+ kd * rgbD.GetBlue());

								*pDestPos = CG32bitPixel((BYTE)red, (BYTE)green, (BYTE)blue);
								}
							else
								{
								BYTE *pAlphaInv = CG32bitPixel::AlphaTable(byAlpha ^ 0xff);	//	Equivalent to 255 - byAlpha

								DWORD red = (DWORD)(ka * rgbA.GetRed()
										+ kb * rgbB.GetRed()
										+ kc * rgbC.GetRed()
										+ kd * rgbD.GetRed());

								DWORD green = (DWORD)(ka * rgbA.GetGreen()
										+ kb * rgbB.GetGreen()
										+ kc * rgbC.GetGreen()
										+ kd * rgbD.GetGreen());

								DWORD blue = (DWORD)(ka * rgbA.GetBlue()
										+ kb * rgbB.GetBlue()
										+ kc * rgbC.GetBlue()
										+ kd * rgbD.GetBlue());

								red = red + pAlphaInv[pDestPos->GetRed()];
								green = green + pAlphaInv[pDestPos->GetGreen()];
								blue = blue + pAlphaInv[pDestPos->GetBlue()];
								*pDestPos = CG32bitPixel((BYTE)Min(red, (DWORD)255), (BYTE)Min(green, (DWORD)255), (BYTE)Min(blue, (DWORD)255));
								}
							}
						}

					//	Next

					vSrcPos = vSrcPos + vIncX;
					pDestPos++;
					}

				//	Next row

				vSrcRow = vSrcRow + vIncY;
				pDestRow = Dest.NextRow(pDestRow);
				}
			}

		void Composite (CG32bitImage &Dest, int xDest, int yDest, const CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc)
			{
			//	Make sure we're in bounds

			if (!Dest.AdjustCoords(&xSrc, &ySrc, Src.GetWidth(), Src.GetHeight(), 
					&xDest, &yDest,
					&cxSrc, &cySrc))
				return;

			CG32bitPixel *pSrcRow = Src.GetPixelPos(xSrc, ySrc);
			CG32bitPixel *pSrcRowEnd = Src.GetPixelPos(xSrc, ySrc + cySrc);
			CG32bitPixel *pDestRow = Dest.GetPixelPos(xDest, yDest);

			while (pSrcRow < pSrcRowEnd)
				{
				CG32bitPixel *pSrc = pSrcRow;
				CG32bitPixel *pSrcEnd = pSrc + cxSrc;
				CG32bitPixel *pDest = pDestRow;

				START_ROW(pSrc, pDest);

				while (pSrc < pSrcEnd)
					{
					CG32bitPixel rgbResult = FILTER(*pSrc, pDest);
					BYTE byResultAlpha = rgbResult.GetAlpha();

					if (byResultAlpha == 0x00)
						;

					else if (byResultAlpha == 0xff || pDest->GetAlpha() == 0x00)
						*pDest = rgbResult;

					else
						{
						BYTE *pAlphaInv = CG32bitPixel::AlphaTable(byResultAlpha ^ 0xff);	//	Equivalent to 255 - byAlpha

						BYTE byRedResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDest->GetRed()] + (WORD)rgbResult.GetRed()));
						BYTE byGreenResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDest->GetGreen()] + (WORD)rgbResult.GetGreen()));
						BYTE byBlueResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDest->GetBlue()] + (WORD)rgbResult.GetBlue()));

						*pDest = CG32bitPixel(byRedResult, byGreenResult, byBlueResult, CG32bitPixel::CompositeAlpha(pDest->GetAlpha(), byResultAlpha));
						}

					pDest++;
					pSrc++;
					}

				pSrcRow = Src.NextRow(pSrcRow);
				pDestRow = Dest.NextRow(pDestRow);
				}
			}

		void Copy (CG32bitImage &Dest, int xDest, int yDest, const CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc)
			{
			//	Make sure we're in bounds

			if (!Dest.AdjustCoords(&xSrc, &ySrc, Src.GetWidth(), Src.GetHeight(), 
					&xDest, &yDest,
					&cxSrc, &cySrc))
				return;

			CG32bitPixel *pSrcRow = Src.GetPixelPos(xSrc, ySrc);
			CG32bitPixel *pSrcRowEnd = Src.GetPixelPos(xSrc, ySrc + cySrc);
			CG32bitPixel *pDestRow = Dest.GetPixelPos(xDest, yDest);

			while (pSrcRow < pSrcRowEnd)
				{
				CG32bitPixel *pSrc = pSrcRow;
				CG32bitPixel *pSrcEnd = pSrc + cxSrc;
				CG32bitPixel *pDest = pDestRow;

				START_ROW(pSrc, pDest);

				while (pSrc < pSrcEnd)
					{
					*pDest = FILTER(*pSrc, pDest);

					pDest++;
					pSrc++;
					}

				pSrcRow = Src.NextRow(pSrcRow);
				pDestRow = Dest.NextRow(pDestRow);
				}
			}

	private:
		inline CG32bitPixel FILTER (CG32bitPixel rgbSrc, CG32bitPixel *pDest) { return ((PAINTER *)this)->Filter(rgbSrc, pDest); }
		inline void START_ROW (CG32bitPixel *pSrc, CG32bitPixel *pDest) { return ((PAINTER *)this)->StartRow(pSrc, pDest); }

		inline void StartRow (CG32bitPixel *pSrc, CG32bitPixel *pDest) { }
	};
