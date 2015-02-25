//	CG32bitImage.cpp
//
//	CG32bitImage Class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CG32bitImage::Fill (int x, int y, int cxWidth, int cyHeight, CG32bitPixel Value)

//	Fill
//
//	Fill the buffer

	{
	//	Make sure we're in bounds

	if (!AdjustCoords(NULL, NULL, 0, 0, 
			&x, &y, 
			&cxWidth, &cyHeight))
		return;

	BYTE byOpacity = Value.GetAlpha();
	if (byOpacity == 0x00)
		return;

	//	Solid Fill

	if (byOpacity == 0xff)
		{
		CG32bitPixel *pDestRow = GetPixelPos(x, y);
		CG32bitPixel *pDestRowEnd = GetPixelPos(x, y + cyHeight);

		while (pDestRow < pDestRowEnd)
			{
			CG32bitPixel *pDest = pDestRow;
			CG32bitPixel *pDestEnd = pDestRow + cxWidth;

			while (pDest < pDestEnd)
				*pDest++ = Value;

			pDestRow = NextRow(pDestRow);
			}
		}

	//	Transparent Fill

	else
		{
		CG32bitPixel *pDestRow = GetPixelPos(x, y);
		CG32bitPixel *pDestRowEnd = GetPixelPos(x, y + cyHeight);

		while (pDestRow < pDestRowEnd)
			{
			CG32bitPixel *pDest = pDestRow;
			CG32bitPixel *pDestEnd = pDestRow + cxWidth;

			while (pDest < pDestEnd)
				{
				*pDest = CG32bitPixel::Blend(*pDest, Value);
				pDest++;
				}

			pDestRow = NextRow(pDestRow);
			}
		}
	}

void CG32bitImage::FillMask (int xSrc, int ySrc, int cxWidth, int cyHeight, const CG32bitImage &Source, CG32bitPixel rgbColor, int xDest, int yDest)

//	FillMask
//
//	Fills the source mask with the given color

	{
	//	Make sure we're in bounds

	if (!AdjustCoords(&xSrc, &ySrc, Source.GetWidth(), Source.GetHeight(),
			&xDest, &yDest,
			&cxWidth, &cyHeight))
		return;

	//	Compute opacity

	BYTE byOpacity = rgbColor.GetAlpha();
	if (byOpacity == 0x00)
		return;

	CG32bitPixel *pSrcRow = Source.GetPixelPos(xSrc, ySrc);
	CG32bitPixel *pSrcRowEnd = Source.GetPixelPos(xSrc, ySrc + cyHeight);
	CG32bitPixel *pDestRow = GetPixelPos(xDest, yDest);

	//	If full opacity, then optimize

	if (byOpacity == 0xff)
		{
		switch (Source.GetAlphaType())
			{
			case alphaNone:
				while (pSrcRow < pSrcRowEnd)
					{
					CG32bitPixel *pSrcPos = pSrcRow;
					CG32bitPixel *pSrcPosEnd = pSrcRow + cxWidth;
					CG32bitPixel *pDestPos = pDestRow;

					while (pSrcPos < pSrcPosEnd)
						{
						*pDestPos++ = rgbColor;
						pSrcPos++;
						}

					pSrcRow = Source.NextRow(pSrcRow);
					pDestRow = NextRow(pDestRow);
					}
				break;

			case alpha1:
				while (pSrcRow < pSrcRowEnd)
					{
					CG32bitPixel *pSrcPos = pSrcRow;
					CG32bitPixel *pSrcPosEnd = pSrcRow + cxWidth;
					CG32bitPixel *pDestPos = pDestRow;

					while (pSrcPos < pSrcPosEnd)
						{
						if (pSrcPos->GetAlpha())
							*pDestPos = rgbColor;

						pSrcPos++;
						pDestPos++;
						}

					pSrcRow = Source.NextRow(pSrcRow);
					pDestRow = NextRow(pDestRow);
					}
				break;

			case alpha8:
				{
				BYTE colorRed = rgbColor.GetRed();
				BYTE colorGreen = rgbColor.GetGreen();
				BYTE colorBlue = rgbColor.GetBlue();

				while (pSrcRow < pSrcRowEnd)
					{
					CG32bitPixel *pSrcPos = pSrcRow;
					CG32bitPixel *pSrcPosEnd = pSrcRow + cxWidth;
					CG32bitPixel *pDestPos = pDestRow;

					while (pSrcPos < pSrcPosEnd)
						{
						BYTE byAlpha = pSrcPos->GetAlpha();

						if (byAlpha == 0x00)
							;
						else if (byAlpha == 0xff)
							*pDestPos = rgbColor;
						else
							{
							BYTE *pAlpha = CG32bitPixel::AlphaTable(byAlpha);
							BYTE *pAlphaInv = CG32bitPixel::AlphaTable(byAlpha ^ 0xff);	//	Equivalent to 255 - byAlpha

							BYTE byRedResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetRed()] + (WORD)pAlpha[colorRed]));
							BYTE byGreenResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetGreen()] + (WORD)pAlpha[colorGreen]));
							BYTE byBlueResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetBlue()] + (WORD)pAlpha[colorBlue]));

							*pDestPos = CG32bitPixel(byRedResult, byGreenResult, byBlueResult);
							}

						pSrcPos++;
						pDestPos++;
						}

					pSrcRow = Source.NextRow(pSrcRow);
					pDestRow = NextRow(pDestRow);
					}
				break;
				}

			default:
				//	Not Yet Implemented
				ASSERT(false);
			}
		}

	//	Otherwise, it's harder

	else
		{
		BYTE colorRed = rgbColor.GetRed();
		BYTE colorGreen = rgbColor.GetGreen();
		BYTE colorBlue = rgbColor.GetBlue();

		switch (Source.GetAlphaType())
			{
			case alphaNone:
				{
				BYTE *pAlpha = CG32bitPixel::AlphaTable(byOpacity);
				BYTE *pAlphaInv = CG32bitPixel::AlphaTable(byOpacity ^ 0xff);	//	Equivalent to 255 - byAlpha

				while (pSrcRow < pSrcRowEnd)
					{
					CG32bitPixel *pSrcPos = pSrcRow;
					CG32bitPixel *pSrcPosEnd = pSrcRow + cxWidth;
					CG32bitPixel *pDestPos = pDestRow;

					while (pSrcPos < pSrcPosEnd)
						{
						BYTE byRedResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetRed()] + (WORD)pAlpha[colorRed]));
						BYTE byGreenResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetGreen()] + (WORD)pAlpha[colorGreen]));
						BYTE byBlueResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetBlue()] + (WORD)pAlpha[colorBlue]));

						*pDestPos = CG32bitPixel(byRedResult, byGreenResult, byBlueResult);

						pSrcPos++;
						pDestPos++;
						}

					pSrcRow = Source.NextRow(pSrcRow);
					pDestRow = NextRow(pDestRow);
					}
				break;
				}

			case alpha1:
				{
				BYTE *pAlpha = CG32bitPixel::AlphaTable(byOpacity);
				BYTE *pAlphaInv = CG32bitPixel::AlphaTable(byOpacity ^ 0xff);	//	Equivalent to 255 - byAlpha

				while (pSrcRow < pSrcRowEnd)
					{
					CG32bitPixel *pSrcPos = pSrcRow;
					CG32bitPixel *pSrcPosEnd = pSrcRow + cxWidth;
					CG32bitPixel *pDestPos = pDestRow;

					while (pSrcPos < pSrcPosEnd)
						{
						if (pSrcPos->GetAlpha())
							{
							BYTE byRedResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetRed()] + (WORD)pAlpha[colorRed]));
							BYTE byGreenResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetGreen()] + (WORD)pAlpha[colorGreen]));
							BYTE byBlueResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetBlue()] + (WORD)pAlpha[colorBlue]));

							*pDestPos = CG32bitPixel(byRedResult, byGreenResult, byBlueResult);
							}

						pSrcPos++;
						pDestPos++;
						}

					pSrcRow = Source.NextRow(pSrcRow);
					pDestRow = NextRow(pDestRow);
					}
				break;
				}

			case alpha8:
				{
				BYTE *pOpacityMap[256];
				BYTE *pOpacityInvMap[256];
				for (DWORD i = 0; i < 256; i++)
					{
					BYTE byAlpha = (BYTE)((i * byOpacity) / 255);
					BYTE byAlphaInv = (byAlpha ^ 0xff);

					pOpacityMap[i] = CG32bitPixel::AlphaTable(byAlpha);
					pOpacityInvMap[i] = CG32bitPixel::AlphaTable(byAlphaInv);
					}

				while (pSrcRow < pSrcRowEnd)
					{
					CG32bitPixel *pSrcPos = pSrcRow;
					CG32bitPixel *pSrcPosEnd = pSrcRow + cxWidth;
					CG32bitPixel *pDestPos = pDestRow;

					while (pSrcPos < pSrcPosEnd)
						{
						BYTE bySrcAlpha = pSrcPos->GetAlpha();

						if (bySrcAlpha == 0x00)
							;
						else
							{
							BYTE *pAlpha = pOpacityMap[bySrcAlpha];
							BYTE *pAlphaInv = pOpacityInvMap[bySrcAlpha];

							BYTE byRedResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetRed()] + (WORD)pAlpha[colorRed]));
							BYTE byGreenResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetGreen()] + (WORD)pAlpha[colorGreen]));
							BYTE byBlueResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetBlue()] + (WORD)pAlpha[colorBlue]));

							*pDestPos = CG32bitPixel(byRedResult, byGreenResult, byBlueResult);
							}

						pSrcPos++;
						pDestPos++;
						}

					pSrcRow = Source.NextRow(pSrcRow);
					pDestRow = NextRow(pDestRow);
					}
				break;
				}

			default:
				//	Not Yet Implemented
				ASSERT(false);
			}
		}
	}

void CG32bitImage::FillMask (int xSrc, int ySrc, int cxWidth, int cyHeight, const CG16bitImage &Source, CG32bitPixel rgbColor, int xDest, int yDest)

//	FillMask
//
//	Fills the source mask with the given color

	{
	//	Make sure we're in bounds

	if (!AdjustCoords(&xSrc, &ySrc, Source.GetWidth(), Source.GetHeight(),
			&xDest, &yDest,
			&cxWidth, &cyHeight))
		return;

	//	Compute opacity

	DWORD dwOpacity = rgbColor.GetAlpha();

	//	If we've got an alpha mask then blt using the transparency
	//	information.

	CG32bitPixel *pDestRow = GetPixelPos(xDest, yDest);
	if (Source.HasAlpha())
		{
		if (dwOpacity == 0xff)
			{
			BYTE *pAlphaSrcRow = Source.GetAlphaValue(xSrc, ySrc);
			BYTE *pAlphaSrcRowEnd = Source.GetAlphaValue(xSrc, ySrc + cyHeight);

			while (pAlphaSrcRow < pAlphaSrcRowEnd)
				{
				BYTE *pAlphaPos = pAlphaSrcRow;
				BYTE *pAlphaPosEnd = pAlphaPos + cxWidth;
				CG32bitPixel *pDestPos = pDestRow;

				while (pAlphaPos < pAlphaPosEnd)
					if (*pAlphaPos == 0x00)
						{
						pDestPos++;
						pAlphaPos++;
						}
					else if (*pAlphaPos == 0xff)
						{
						*pDestPos++ = rgbColor;
						pAlphaPos++;
						}
					else
						{
						*pDestPos++ = CG32bitPixel::Blend(*pDestPos, rgbColor, *pAlphaPos);
						pAlphaPos++;
						}

				pDestRow = NextRow(pDestRow);
				pAlphaSrcRow = Source.NextAlphaRow(pAlphaSrcRow);
				}
			}
		else
			{
			//	If we've got some transparency, built a table to map opacity

			BYTE dwOpacityMap[256];
			for (DWORD i = 0; i < 256; i++)
				dwOpacityMap[i] = (BYTE)((i * dwOpacity) >> 8);

			BYTE *pAlphaSrcRow = Source.GetAlphaValue(xSrc, ySrc);
			BYTE *pAlphaSrcRowEnd = Source.GetAlphaValue(xSrc, ySrc + cyHeight);

			while (pAlphaSrcRow < pAlphaSrcRowEnd)
				{
				BYTE *pAlphaPos = pAlphaSrcRow;
				BYTE *pAlphaPosEnd = pAlphaPos + cxWidth;
				CG32bitPixel *pDestPos = pDestRow;

				while (pAlphaPos < pAlphaPosEnd)
					if (*pAlphaPos == 0x00)
						{
						pDestPos++;
						pAlphaPos++;
						}
					else if (*pAlphaPos == 0xff)
						{
						*pDestPos++ = CG32bitPixel::Blend(*pDestPos, rgbColor);
						pAlphaPos++;
						}
					else
						{
						*pDestPos++ = CG32bitPixel::Blend(*pDestPos, rgbColor, dwOpacityMap[*pAlphaPos]);
						pAlphaPos++;
						}

				pDestRow = NextRow(pDestRow);
				pAlphaSrcRow = Source.NextAlphaRow(pAlphaSrcRow);
				}
			}
		}
	else if (Source.HasRGB())
		{
		WORD *pSrcRow = Source.GetPixel(Source.GetRowStart(ySrc), xSrc);
		WORD *pSrcRowEnd = Source.GetPixel(Source.GetRowStart(ySrc + cyHeight), xSrc);
		WORD wBackColor = Source.GetBackColor();

		if (dwOpacity == 0xff)
			{
			while (pSrcRow < pSrcRowEnd)
				{
				WORD *pSrcPos = pSrcRow;
				WORD *pSrcPosEnd = pSrcRow + cxWidth;
				CG32bitPixel *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					if (*pSrcPos++ != wBackColor)
						*pDestPos++ = rgbColor;
					else
						pDestPos++;

				pSrcRow = Source.NextRow(pSrcRow);
				pDestRow = NextRow(pDestRow);
				}
			}
		else
			{
			while (pSrcRow < pSrcRowEnd)
				{
				WORD *pSrcPos = pSrcRow;
				WORD *pSrcPosEnd = pSrcRow + cxWidth;
				CG32bitPixel *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					if (*pSrcPos++ != wBackColor)
						*pDestPos++ = CG32bitPixel::Blend(*pDestPos, rgbColor);
					else
						pDestPos++;

				pSrcRow = Source.NextRow(pSrcRow);
				pDestRow = NextRow(pDestRow);
				}
			}
		}
	}

void CG32bitImage::FillMask (int xSrc, int ySrc, int cxWidth, int cyHeight, const CG8bitImage &Source, CG32bitPixel rgbColor, int xDest, int yDest)

//	FillMask
//
//	Fills the source mask with the given color

	{
	//	Make sure we're in bounds

	if (!AdjustCoords(&xSrc, &ySrc, Source.GetWidth(), Source.GetHeight(),
			&xDest, &yDest,
			&cxWidth, &cyHeight))
		return;

	//	Compute opacity

	BYTE byOpacity = rgbColor.GetAlpha();
	if (byOpacity == 0x00)
		return;

	BYTE *pSrcRow = Source.GetPixelPos(xSrc, ySrc);
	BYTE *pSrcRowEnd = Source.GetPixelPos(xSrc, ySrc + cyHeight);
	CG32bitPixel *pDestRow = GetPixelPos(xDest, yDest);

	//	If full opacity, then optimize

	if (byOpacity == 0xff)
		{
		BYTE colorRed = rgbColor.GetRed();
		BYTE colorGreen = rgbColor.GetGreen();
		BYTE colorBlue = rgbColor.GetBlue();

		while (pSrcRow < pSrcRowEnd)
			{
			BYTE *pSrcPos = pSrcRow;
			BYTE *pSrcPosEnd = pSrcRow + cxWidth;
			CG32bitPixel *pDestPos = pDestRow;

			while (pSrcPos < pSrcPosEnd)
				{
				BYTE byAlpha = *pSrcPos;

				if (byAlpha == 0x00)
					;
				else if (byAlpha == 0xff)
					*pDestPos = rgbColor;
				else
					{
					BYTE *pAlpha = CG32bitPixel::AlphaTable(byAlpha);
					BYTE *pAlphaInv = CG32bitPixel::AlphaTable(byAlpha ^ 0xff);	//	Equivalent to 255 - byAlpha

					BYTE byRedResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetRed()] + (WORD)pAlpha[colorRed]));
					BYTE byGreenResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetGreen()] + (WORD)pAlpha[colorGreen]));
					BYTE byBlueResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetBlue()] + (WORD)pAlpha[colorBlue]));

					*pDestPos = CG32bitPixel(byRedResult, byGreenResult, byBlueResult);
					}

				pSrcPos++;
				pDestPos++;
				}

			pSrcRow = Source.NextRow(pSrcRow);
			pDestRow = NextRow(pDestRow);
			}
		}

	//	Otherwise, it's harder

	else
		{
		BYTE colorRed = rgbColor.GetRed();
		BYTE colorGreen = rgbColor.GetGreen();
		BYTE colorBlue = rgbColor.GetBlue();

		BYTE *pOpacityMap[256];
		BYTE *pOpacityInvMap[256];
		for (DWORD i = 0; i < 256; i++)
			{
			BYTE byAlpha = (BYTE)((i * byOpacity) / 255);
			BYTE byAlphaInv = (byAlpha ^ 0xff);

			pOpacityMap[i] = CG32bitPixel::AlphaTable(byAlpha);
			pOpacityInvMap[i] = CG32bitPixel::AlphaTable(byAlphaInv);
			}

		while (pSrcRow < pSrcRowEnd)
			{
			BYTE *pSrcPos = pSrcRow;
			BYTE *pSrcPosEnd = pSrcRow + cxWidth;
			CG32bitPixel *pDestPos = pDestRow;

			while (pSrcPos < pSrcPosEnd)
				{
				BYTE bySrcAlpha = *pSrcPos;

				if (bySrcAlpha == 0x00)
					;
				else
					{
					BYTE *pAlpha = pOpacityMap[bySrcAlpha];
					BYTE *pAlphaInv = pOpacityInvMap[bySrcAlpha];

					BYTE byRedResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetRed()] + (WORD)pAlpha[colorRed]));
					BYTE byGreenResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetGreen()] + (WORD)pAlpha[colorGreen]));
					BYTE byBlueResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetBlue()] + (WORD)pAlpha[colorBlue]));

					*pDestPos = CG32bitPixel(byRedResult, byGreenResult, byBlueResult);
					}

				pSrcPos++;
				pDestPos++;
				}

			pSrcRow = Source.NextRow(pSrcRow);
			pDestRow = NextRow(pDestRow);
			}
		}
	}

void CG32bitImage::Set (CG32bitPixel Value)

//	Set
//
//	Sets the entire image to the given value.

	{
	CG32bitPixel *pDest = GetPixelPos(0, 0);
	CG32bitPixel *pDestEnd = GetPixelPos(0, m_cyHeight);

	while (pDest < pDestEnd)
		*pDest++ = Value;
	}

void CG32bitImage::Set (int x, int y, int cxWidth, int cyHeight, CG32bitPixel Value)

//	Set
//
//	Sets all the pixels in the given rect to the given value.

	{
	CG32bitPixel *pDestRow = GetPixelPos(x, y);
	CG32bitPixel *pDestRowEnd = GetPixelPos(x, y + cyHeight);

	while (pDestRow < pDestRowEnd)
		{
		CG32bitPixel *pDest = pDestRow;
		CG32bitPixel *pDestEnd = pDestRow + cxWidth;

		while (pDest < pDestEnd)
			*pDest++ = Value;

		pDestRow = NextRow(pDestRow);
		}
	}

void CG32bitImage::SetMask (int xSrc, int ySrc, int cxWidth, int cyHeight, const CG32bitImage &Source, CG32bitPixel rgbColor, int xDest, int yDest)

//	SetMask
//
//	Sets all pixels in the source mask to the given color

	{
	//	Make sure we're in bounds

	if (!AdjustCoords(&xSrc, &ySrc, Source.GetWidth(), Source.GetHeight(),
			&xDest, &yDest,
			&cxWidth, &cyHeight))
		return;

	//	Compute opacity

	BYTE byOpacity = rgbColor.GetAlpha();

	CG32bitPixel *pSrcRow = Source.GetPixelPos(xSrc, ySrc);
	CG32bitPixel *pSrcRowEnd = Source.GetPixelPos(xSrc, ySrc + cyHeight);
	CG32bitPixel *pDestRow = GetPixelPos(xDest, yDest);

	//	Set based on source type

	switch (Source.GetAlphaType())
		{
		case alphaNone:
			while (pSrcRow < pSrcRowEnd)
				{
				CG32bitPixel *pSrcPos = pSrcRow;
				CG32bitPixel *pSrcPosEnd = pSrcRow + cxWidth;
				CG32bitPixel *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					{
					*pDestPos++ = rgbColor;
					pSrcPos++;
					}

				pSrcRow = Source.NextRow(pSrcRow);
				pDestRow = NextRow(pDestRow);
				}
			break;

		case alpha1:
			while (pSrcRow < pSrcRowEnd)
				{
				CG32bitPixel *pSrcPos = pSrcRow;
				CG32bitPixel *pSrcPosEnd = pSrcRow + cxWidth;
				CG32bitPixel *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					{
					if (pSrcPos->GetAlpha())
						*pDestPos = rgbColor;

					pSrcPos++;
					pDestPos++;
					}

				pSrcRow = Source.NextRow(pSrcRow);
				pDestRow = NextRow(pDestRow);
				}
			break;

		case alpha8:
			{
			BYTE colorAlpha = rgbColor.GetAlpha();
			BYTE colorRed = rgbColor.GetRed();
			BYTE colorGreen = rgbColor.GetGreen();
			BYTE colorBlue = rgbColor.GetBlue();

			while (pSrcRow < pSrcRowEnd)
				{
				CG32bitPixel *pSrcPos = pSrcRow;
				CG32bitPixel *pSrcPosEnd = pSrcRow + cxWidth;
				CG32bitPixel *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					{
					BYTE byAlpha = pSrcPos->GetAlpha();

					if (byAlpha == 0x00)
						;
					else if (byAlpha == 0xff)
						*pDestPos = rgbColor;
					else
						{
						BYTE *pAlpha = CG32bitPixel::AlphaTable(byAlpha);
						*pDestPos = CG32bitPixel(pAlpha[colorRed], pAlpha[colorGreen], pAlpha[colorBlue], pAlpha[colorAlpha]);
						}

					pSrcPos++;
					pDestPos++;
					}

				pSrcRow = Source.NextRow(pSrcRow);
				pDestRow = NextRow(pDestRow);
				}
			break;
			}

		default:
			//	Not Yet Implemented
			ASSERT(false);
		}
	}

