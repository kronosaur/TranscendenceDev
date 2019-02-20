//	CG32bitImage.cpp
//
//	CG32bitImage Class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CG32bitImage::Blt (int xSrc, int ySrc, int cxWidth, int cyHeight, BYTE byOpacity, const CG32bitImage &Source, int xDest, int yDest)

//	Blt
//
//	Blt a 16-bit image to the destination

	{
	//	Make sure we're in bounds

	if (!AdjustCoords(&xSrc, &ySrc, Source.GetWidth(), Source.GetHeight(), 
			&xDest, &yDest,
			&cxWidth, &cyHeight))
		return;

	//	Short-circuit

	if (byOpacity == 0)
		return;

	CG32bitPixel *pSrcRow = Source.GetPixelPos(xSrc, ySrc);
	CG32bitPixel *pSrcRowEnd = Source.GetPixelPos(xSrc, ySrc + cyHeight);
	CG32bitPixel *pDestRow = GetPixelPos(xDest, yDest);

	//	If full opacity then optimize

	if (byOpacity == 0xff)
		{
		switch (Source.GetAlphaType())
			{
			//	If no alpha, then we just copy values directly

			case alphaNone:
				{
				DWORD dwWidth8 = (cxWidth / 8) * 8;

				while (pSrcRow < pSrcRowEnd)
					{
					CG32bitPixel *pSrcPos = pSrcRow;
					CG32bitPixel *pSrcPosBlockEnd = pSrcRow + dwWidth8;
					CG32bitPixel *pSrcPosEnd = pSrcRow + cxWidth;
					CG32bitPixel *pDestPos = pDestRow;

					//	Unrolled loop

					while (pSrcPos < pSrcPosBlockEnd)
						{
						*pDestPos++ = *pSrcPos++;
						*pDestPos++ = *pSrcPos++;
						*pDestPos++ = *pSrcPos++;
						*pDestPos++ = *pSrcPos++;
						*pDestPos++ = *pSrcPos++;
						*pDestPos++ = *pSrcPos++;
						*pDestPos++ = *pSrcPos++;
						*pDestPos++ = *pSrcPos++;
						}

					//	Fill the remainder

					while (pSrcPos < pSrcPosEnd)
						*pDestPos++ = *pSrcPos++;

					pSrcRow = Source.NextRow(pSrcRow);
					pDestRow = NextRow(pDestRow);
					}
				break;
				}

			//	If 1-bit alpha, then we check

			case alpha1:
				while (pSrcRow < pSrcRowEnd)
					{
					CG32bitPixel *pSrcPos = pSrcRow;
					CG32bitPixel *pSrcPosEnd = pSrcRow + cxWidth;
					CG32bitPixel *pDestPos = pDestRow;

					while (pSrcPos < pSrcPosEnd)
						{
						if (pSrcPos->GetAlpha())
							*pDestPos = *pSrcPos;

						pSrcPos++;
						pDestPos++;
						}

					pSrcRow = Source.NextRow(pSrcRow);
					pDestRow = NextRow(pDestRow);
					}
				break;

			//	If 8-bit alpha, then we blend

			case alpha8:
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
							*pDestPos = *pSrcPos;
						else
							{
							BYTE *pAlphaInv = CG32bitPixel::AlphaTable(byAlpha ^ 0xff);	//	Equivalent to 255 - byAlpha

							BYTE byRedResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetRed()] + (WORD)pSrcPos->GetRed()));
							BYTE byGreenResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetGreen()] + (WORD)pSrcPos->GetGreen()));
							BYTE byBlueResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetBlue()] + (WORD)pSrcPos->GetBlue()));

							*pDestPos = CG32bitPixel(byRedResult, byGreenResult, byBlueResult);
							}

						pSrcPos++;
						pDestPos++;
						}

					pSrcRow = Source.NextRow(pSrcRow);
					pDestRow = NextRow(pDestRow);
					}
				break;

			default:
				//	Not Yet Implemented
				ASSERT(false);
			}
		}

	//	Otherwise, it's more complicated

	else
		{
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
						BYTE byRedResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetRed()] + (WORD)pAlpha[pSrcPos->GetRed()]));
						BYTE byGreenResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetGreen()] + (WORD)pAlpha[pSrcPos->GetGreen()]));
						BYTE byBlueResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetBlue()] + (WORD)pAlpha[pSrcPos->GetBlue()]));

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
							BYTE byRedResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetRed()] + (WORD)pAlpha[pSrcPos->GetRed()]));
							BYTE byGreenResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetGreen()] + (WORD)pAlpha[pSrcPos->GetGreen()]));
							BYTE byBlueResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetBlue()] + (WORD)pAlpha[pSrcPos->GetBlue()]));

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
				BYTE *pDefAlpha = CG32bitPixel::AlphaTable(byOpacity);
				BYTE *pDefAlphaInv = CG32bitPixel::AlphaTable(byOpacity ^ 0xff);	//	Equivalent to 255 - byAlpha

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
						else if (bySrcAlpha == 0xff)
							{
							BYTE byRedResult = (BYTE)Min((WORD)0xff, (WORD)(pDefAlphaInv[pDestPos->GetRed()] + (WORD)pDefAlpha[pSrcPos->GetRed()]));
							BYTE byGreenResult = (BYTE)Min((WORD)0xff, (WORD)(pDefAlphaInv[pDestPos->GetGreen()] + (WORD)pDefAlpha[pSrcPos->GetGreen()]));
							BYTE byBlueResult = (BYTE)Min((WORD)0xff, (WORD)(pDefAlphaInv[pDestPos->GetBlue()] + (WORD)pDefAlpha[pSrcPos->GetBlue()]));

							*pDestPos = CG32bitPixel(byRedResult, byGreenResult, byBlueResult);
							}
						else
							{
							BYTE *pAlpha = CG32bitPixel::AlphaTable(byOpacity);
							BYTE *pAlphaInv = CG32bitPixel::AlphaTable((bySrcAlpha * byOpacity / 255) ^ 0xff);

							BYTE byRedResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetRed()] + (WORD)pAlpha[pSrcPos->GetRed()]));
							BYTE byGreenResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetGreen()] + (WORD)pAlpha[pSrcPos->GetGreen()]));
							BYTE byBlueResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetBlue()] + (WORD)pAlpha[pSrcPos->GetBlue()]));

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

void CG32bitImage::Blt (int xSrc, int ySrc, int cxWidth, int cyHeight, BYTE byOpacity, const CG16bitImage &Source, int xDest, int yDest)

//	Blt
//
//	Blt a 16-bit image to the destination

	{
	ASSERT(!Source.IsSprite());

	//	Make sure we're in bounds

	if (!AdjustCoords(&xSrc, &ySrc, Source.GetWidth(), Source.GetHeight(), 
			&xDest, &yDest,
			&cxWidth, &cyHeight))
		return;

	//	Short-circuit

	if (byOpacity == 0)
		return;

	WORD *pSrcRow = Source.GetPixel(Source.GetRowStart(ySrc), xSrc);
	WORD *pSrcRowEnd = Source.GetPixel(Source.GetRowStart(ySrc + cyHeight), xSrc);
	CG32bitPixel *pDestRow = GetPixelPos(xDest, yDest);

	//	If we've got an alpha mask then blt using the transparency
	//	information.

	if (Source.HasAlpha())
		{
		BYTE *pAlphaSrcRow = Source.GetAlphaValue(xSrc, ySrc);

		if (byOpacity == 255)
			{
			while (pSrcRow < pSrcRowEnd)
				{
				BYTE *pAlphaPos = pAlphaSrcRow;
				WORD *pSrcPos = pSrcRow;
				WORD *pSrcPosEnd = pSrcRow + cxWidth;
				CG32bitPixel *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					if (*pAlphaPos == 0)
						{
						pDestPos++;
						pSrcPos++;
						pAlphaPos++;
						}
					else if (*pAlphaPos == 0xff)
						{
						*pDestPos++ = *pSrcPos++;
						pAlphaPos++;
						}
					else
						{
						BYTE *pAlphaInv = CG32bitPixel::AlphaTable((*pAlphaPos) ^ 0xff);	//	Equivalent to 255 - (*pAlphaPos)

						WORD wRedSrc = (((*pSrcPos >> 11) & 0x1f) << 3) | 0x07;
						WORD wGreenSrc = (((*pSrcPos >> 5) & 0x3f) << 2) | 0x03;
						WORD wBlueSrc = (((*pSrcPos) & 0x1f) << 3) | 0x07;

						BYTE byRedResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetRed()] + wRedSrc));
						BYTE byGreenResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetGreen()] + wGreenSrc));
						BYTE byBlueResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetBlue()] + wBlueSrc));

						*pDestPos++ = CG32bitPixel(byRedResult, byGreenResult, byBlueResult);
						pSrcPos++;
						pAlphaPos++;
						}

				pSrcRow = Source.NextRow(pSrcRow);
				pDestRow = NextRow(pDestRow);
				pAlphaSrcRow = Source.NextAlphaRow(pAlphaSrcRow);
				}
			}
		else if (byOpacity > 0)
			{
			while (pSrcRow < pSrcRowEnd)
				{
				BYTE *pAlphaPos = pAlphaSrcRow;
				WORD *pSrcPos = pSrcRow;
				WORD *pSrcPosEnd = pSrcRow + cxWidth;
				CG32bitPixel *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					if (*pAlphaPos == 0)
						{
						pDestPos++;
						pSrcPos++;
						pAlphaPos++;
						}
					else if (*pAlphaPos == 255)
						{
						BYTE *pAlpha5 = g_Alpha5[byOpacity];
						BYTE *pAlpha6 = g_Alpha6[byOpacity];
						BYTE *pAlphaInv = CG32bitPixel::AlphaTable(byOpacity ^ 0xff);

						DWORD sTemp = *pSrcPos;
						BYTE byRedResult = pAlphaInv[pDestPos->GetRed()] + pAlpha5[(sTemp & 0xf800) >> 11];
						BYTE byGreenResult = pAlphaInv[pDestPos->GetGreen()] + pAlpha6[(sTemp & 0x7e0) >> 5];
						BYTE byBlueResult = pAlphaInv[pDestPos->GetBlue()] + pAlpha5[(sTemp & 0x1f)];

						*pDestPos++ = CG32bitPixel(byRedResult, byGreenResult, byBlueResult);
						pSrcPos++;
						pAlphaPos++;
						}
					else
						{
						BYTE byPixelAlpha = (BYTE)(byOpacity * (*pAlphaPos) / 255);

						BYTE *pAlpha5 = g_Alpha5[byPixelAlpha];
						BYTE *pAlpha6 = g_Alpha6[byPixelAlpha];
						BYTE *pAlphaInv = CG32bitPixel::AlphaTable(byPixelAlpha ^ 0xff);

						DWORD sTemp = *pSrcPos;
						BYTE byRedResult = pAlphaInv[pDestPos->GetRed()] + pAlpha5[(sTemp & 0xf800) >> 11];
						BYTE byGreenResult = pAlphaInv[pDestPos->GetGreen()] + pAlpha6[(sTemp & 0x7e0) >> 5];
						BYTE byBlueResult = pAlphaInv[pDestPos->GetBlue()] + pAlpha5[(sTemp & 0x1f)];

						*pDestPos++ = CG32bitPixel(byRedResult, byGreenResult, byBlueResult);
						pSrcPos++;
						pAlphaPos++;
						}

				pSrcRow = Source.NextRow(pSrcRow);
				pDestRow = NextRow(pDestRow);
				pAlphaSrcRow = Source.NextAlphaRow(pAlphaSrcRow);
				}
			}
		}

	//	If we've got constant transparency then use the alpha tables

	else if (Source.IsTransparent())
		{
		//	LATER

		ASSERT(false);
		}

	//	If we have a transparent color, then blt using that

	else if (Source.HasMask())
		{
		WORD wBackColor = Source.GetBackColor();

		if (byOpacity == 255)
			{
			while (pSrcRow < pSrcRowEnd)
				{
				WORD *pSrcPos = pSrcRow;
				WORD *pSrcPosEnd = pSrcRow + cxWidth;
				CG32bitPixel *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					if (*pSrcPos == wBackColor)
						{
						pDestPos++;
						pSrcPos++;
						}
					else
						*pDestPos++ = *pSrcPos++;

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
				CG32bitPixel*pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					if (*pSrcPos == wBackColor)
						{
						pDestPos++;
						pSrcPos++;
						}
					else
						*pDestPos++ = CG32bitPixel::Blend(*pDestPos, *pSrcPos++, byOpacity);

				pSrcRow = Source.NextRow(pSrcRow);
				pDestRow = NextRow(pDestRow);
				}
			}
		}

	//	Otherwise, a plain blt

	else
		{
		if (byOpacity == 255)
			{
			while (pSrcRow < pSrcRowEnd)
				{
				WORD *pSrcPos = pSrcRow;
				WORD *pSrcPosEnd = pSrcRow + cxWidth;
				CG32bitPixel *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					*pDestPos++ = *pSrcPos++;

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
				CG32bitPixel*pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					*pDestPos++ = CG32bitPixel::Blend(*pDestPos, *pSrcPos++, byOpacity);

				pSrcRow = Source.NextRow(pSrcRow);
				pDestRow = NextRow(pDestRow);
				}
			}
		}
	}

void CG32bitImage::BltMask (int xSrc, int ySrc, int cxWidth, int cyHeight, const CG32bitImage &Mask, const CG32bitImage &Source, int xDest, int yDest)

//	BltMask
//
//	Blt the image to the destination (the mask matches the destination).

	{
	//	Make sure we're in bounds

	if (!AdjustCoords(&xSrc, &ySrc, Source.m_cxWidth, Source.m_cyHeight, 
			&xDest, &yDest,
			&cxWidth, &cyHeight))
		return;

	if (!Mask.AdjustCoords(&xSrc, &ySrc, Source.m_cxWidth, Source.m_cyHeight,
			&xDest, &yDest,
			&cxWidth, &cyHeight))
		return;

	//	Do the blt

	CG32bitPixel *pSrcRow = Source.GetPixelPos(xSrc, ySrc);
	CG32bitPixel *pSrcRowEnd = Source.GetPixelPos(xSrc, ySrc + cyHeight);
	CG32bitPixel *pDestRow = GetPixelPos(xDest, yDest);
	CG32bitPixel *pAlphaRow = Mask.GetPixelPos(xDest, yDest);

	while (pSrcRow < pSrcRowEnd)
		{
		CG32bitPixel *pSrcPos = pSrcRow;
		CG32bitPixel *pSrcPosEnd = pSrcRow + cxWidth;
		CG32bitPixel *pDestPos = pDestRow;
		CG32bitPixel *pAlphaPos = pAlphaRow;

		while (pSrcPos < pSrcPosEnd)
			{
			BYTE byAlpha = pAlphaPos->GetAlpha();

			if (byAlpha == 0xff)
				{
				*pDestPos++ = *pSrcPos++;
				pAlphaPos++;
				}
			else if (byAlpha == 0)
				{
				pDestPos++;
				pSrcPos++;
				pAlphaPos++;
				}
			else
				{
				BYTE *pAlpha = CG32bitPixel::AlphaTable(byAlpha);
				BYTE *pAlphaInv = CG32bitPixel::AlphaTable(byAlpha ^ 0xff);

				BYTE byRedResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetRed()] + (WORD)pAlpha[pSrcPos->GetRed()]));
				BYTE byGreenResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetGreen()] + (WORD)pAlpha[pSrcPos->GetGreen()]));
				BYTE byBlueResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDestPos->GetBlue()] + (WORD)pAlpha[pSrcPos->GetBlue()]));

				*pDestPos = CG32bitPixel(byRedResult, byGreenResult, byBlueResult);

				pDestPos++;
				pSrcPos++;
				pAlphaPos++;
				}
			}

		pSrcRow = Source.NextRow(pSrcRow);
		pDestRow = NextRow(pDestRow);
		pAlphaRow = Mask.NextRow(pAlphaRow);
		}
	}

void CG32bitImage::Composite (int xSrc, int ySrc, int cxWidth, int cyHeight, BYTE byOpacity, const CG32bitImage &Source, int xDest, int yDest)

//	Composite
//
//	Composites the source onto the destination

	{
	if (byOpacity == 0xff)
		{
		CFilterNormal Filter;
		Filter.Composite(*this, xDest, yDest, Source, xSrc, ySrc, cxWidth, cyHeight);
		}
	else
		{
		CFilterTrans Filter(byOpacity);
		Filter.Composite(*this, xDest, yDest, Source, xSrc, ySrc, cxWidth, cyHeight);
		}
	}

void CG32bitImage::Copy (int xSrc, int ySrc, int cxWidth, int cyHeight, const CG32bitImage &Source, int xDest, int yDest)

//	Copy
//
//	Copies a region

	{
	//	Make sure we're in bounds

	if (!AdjustCoords(&xSrc, &ySrc, Source.GetWidth(), Source.GetHeight(), 
			&xDest, &yDest,
			&cxWidth, &cyHeight))
		return;

	CG32bitPixel *pSrcRow = Source.GetPixelPos(xSrc, ySrc);
	CG32bitPixel *pSrcRowEnd = Source.GetPixelPos(xSrc, ySrc + cyHeight);
	CG32bitPixel *pDestRow = GetPixelPos(xDest, yDest);

	while (pSrcRow < pSrcRowEnd)
		{
		CG32bitPixel *pSrcPos = pSrcRow;
		CG32bitPixel *pSrcPosEnd = pSrcRow + cxWidth;
		CG32bitPixel *pDestPos = pDestRow;

		while (pSrcPos < pSrcPosEnd)
			*pDestPos++ = *pSrcPos++;

		pSrcRow = Source.NextRow(pSrcRow);
		pDestRow = NextRow(pDestRow);
		}
	}

void CG32bitImage::CopyChannel (ChannelTypes iChannel, int xSrc, int ySrc, int cxWidth, int cyHeight, const CG32bitImage &Source, int xDest, int yDest)

//	CopyChannel
//
//	Copies a channel

	{
	//	Make sure we're in bounds

	if (!AdjustCoords(&xSrc, &ySrc, Source.GetWidth(), Source.GetHeight(), 
			&xDest, &yDest,
			&cxWidth, &cyHeight))
		return;

	CG32bitPixel *pSrcRow = Source.GetPixelPos(xSrc, ySrc);
	CG32bitPixel *pSrcRowEnd = Source.GetPixelPos(xSrc, ySrc + cyHeight);
	CG32bitPixel *pDestRow = GetPixelPos(xDest, yDest);

	switch (iChannel)
		{
		case channelAlpha:
			while (pSrcRow < pSrcRowEnd)
				{
				CG32bitPixel *pSrcPos = pSrcRow;
				CG32bitPixel *pSrcPosEnd = pSrcRow + cxWidth;
				CG32bitPixel *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					{
					pDestPos->SetAlpha(pSrcPos->GetAlpha());

					pSrcPos++;
					pDestPos++;
					}

				pSrcRow = Source.NextRow(pSrcRow);
				pDestRow = NextRow(pDestRow);
				}
			break;

		case channelRed:
			while (pSrcRow < pSrcRowEnd)
				{
				CG32bitPixel *pSrcPos = pSrcRow;
				CG32bitPixel *pSrcPosEnd = pSrcRow + cxWidth;
				CG32bitPixel *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					{
					pDestPos->SetRed(pSrcPos->GetRed());

					pSrcPos++;
					pDestPos++;
					}

				pSrcRow = Source.NextRow(pSrcRow);
				pDestRow = NextRow(pDestRow);
				}
			break;

		case channelGreen:
			while (pSrcRow < pSrcRowEnd)
				{
				CG32bitPixel *pSrcPos = pSrcRow;
				CG32bitPixel *pSrcPosEnd = pSrcRow + cxWidth;
				CG32bitPixel *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					{
					pDestPos->SetGreen(pSrcPos->GetGreen());

					pSrcPos++;
					pDestPos++;
					}

				pSrcRow = Source.NextRow(pSrcRow);
				pDestRow = NextRow(pDestRow);
				}
			break;

		case channelBlue:
			while (pSrcRow < pSrcRowEnd)
				{
				CG32bitPixel *pSrcPos = pSrcRow;
				CG32bitPixel *pSrcPosEnd = pSrcRow + cxWidth;
				CG32bitPixel *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					{
					pDestPos->SetBlue(pSrcPos->GetBlue());

					pSrcPos++;
					pDestPos++;
					}

				pSrcRow = Source.NextRow(pSrcRow);
				pDestRow = NextRow(pDestRow);
				}
			break;
		}
	}

void CG32bitImage::CopyChannel (ChannelTypes iChannel, int xSrc, int ySrc, int cxWidth, int cyHeight, const CG8bitImage &Source, int xDest, int yDest)

//	CopyChannel
//
//	Copies a channel

	{
	//	Make sure we're in bounds

	if (!AdjustCoords(&xSrc, &ySrc, Source.GetWidth(), Source.GetHeight(), 
			&xDest, &yDest,
			&cxWidth, &cyHeight))
		return;

	BYTE *pSrcRow = Source.GetPixelPos(xSrc, ySrc);
	BYTE *pSrcRowEnd = Source.GetPixelPos(xSrc, ySrc + cyHeight);
	CG32bitPixel *pDestRow = GetPixelPos(xDest, yDest);

	switch (iChannel)
		{
		case channelAlpha:
			while (pSrcRow < pSrcRowEnd)
				{
				BYTE *pSrcPos = pSrcRow;
				BYTE *pSrcPosEnd = pSrcRow + cxWidth;
				CG32bitPixel *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					{
					pDestPos->SetAlpha(*pSrcPos);

					pSrcPos++;
					pDestPos++;
					}

				pSrcRow = Source.NextRow(pSrcRow);
				pDestRow = NextRow(pDestRow);
				}
			break;

		case channelRed:
			while (pSrcRow < pSrcRowEnd)
				{
				BYTE *pSrcPos = pSrcRow;
				BYTE *pSrcPosEnd = pSrcRow + cxWidth;
				CG32bitPixel *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					{
					pDestPos->SetRed(*pSrcPos);

					pSrcPos++;
					pDestPos++;
					}

				pSrcRow = Source.NextRow(pSrcRow);
				pDestRow = NextRow(pDestRow);
				}
			break;

		case channelGreen:
			while (pSrcRow < pSrcRowEnd)
				{
				BYTE *pSrcPos = pSrcRow;
				BYTE *pSrcPosEnd = pSrcRow + cxWidth;
				CG32bitPixel *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					{
					pDestPos->SetGreen(*pSrcPos);

					pSrcPos++;
					pDestPos++;
					}

				pSrcRow = Source.NextRow(pSrcRow);
				pDestRow = NextRow(pDestRow);
				}
			break;

		case channelBlue:
			while (pSrcRow < pSrcRowEnd)
				{
				BYTE *pSrcPos = pSrcRow;
				BYTE *pSrcPosEnd = pSrcRow + cxWidth;
				CG32bitPixel *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					{
					pDestPos->SetBlue(*pSrcPos);

					pSrcPos++;
					pDestPos++;
					}

				pSrcRow = Source.NextRow(pSrcRow);
				pDestRow = NextRow(pDestRow);
				}
			break;
		}
	}

void CG32bitImage::IntersectMask (int xMask, int yMask, int cxMask, int cyMask, const CG8bitImage &Mask, int xDest, int yDest)

//	IntersectMask
//
//	Intersects the given mask with this bitmap's current mask.
//
//	xMask, yMask, cxMask, cyMask are the portions of Mask that we want to 
//	intersect.
//
//	xDest and yDest are the coordinates (relative to this bitmap) where we 
//	should align the mask.

	{
	//	Make sure we're in bounds

	if (!AdjustCoords(&xMask, &yMask, Mask.GetWidth(), Mask.GetHeight(), 
			&xDest, &yDest,
			&cxMask, &cyMask))
		return;

	//	Prepare some constants

	int xDestEnd = xDest + cxMask;
	int yDestEnd = yDest + cyMask;

	//	Apply

	int x;
	int y;

	for (y = 0; y < m_cyHeight; y++)
		{
		CG32bitPixel *pDestRow = GetPixelPos(0, y);

		if (y >= yDest && y < yDestEnd)
			{
			BYTE *pSrcAlphaRow = Mask.GetPixelPos(0, yMask + (y - yDest));

			for (x = 0; x < m_cxWidth; x++)
				{
				if (x >= xDest && x < xDestEnd)
					{
					BYTE *pSrcAlpha = pSrcAlphaRow + xMask + (x - xDest);

					pDestRow[x].SetAlpha(CG32bitPixel::BlendAlpha(pDestRow[x].GetAlpha(), *pSrcAlpha));
					}
				else
					pDestRow[x].SetAlpha(0);
				}
			}
		else
			{
			//	If we're outside the mask, then we set to 0

			CG32bitPixel *pDest = pDestRow;
			CG32bitPixel *pDestEnd = pDest + m_cxWidth;
			while (pDest < pDestEnd)
				{
				pDest->SetAlpha(0);
				pDest++;
				}
			}
		}
	}

//	CGDraw ---------------------------------------------------------------------

void CGDraw::Blt (CG32bitImage &Dest, int xDest, int yDest, const CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc, BYTE byOpacity, EBlendModes iMode)

	{
	switch (iMode)
		{
		case blendNormal:
			{
			if (byOpacity == 0xff)
				{
				TBltImageNormal<CGBlendBlend> Painter;
				Painter.Blt(Dest, xDest, yDest, Src, xSrc, ySrc, cxSrc, cySrc);
				}
			else
				{
				TBltImageTrans<CGBlendBlend> Painter(byOpacity);
				Painter.Blt(Dest, xDest, yDest, Src, xSrc, ySrc, cxSrc, cySrc);
				}
			break;
			}

		case blendHardLight:
			{
			if (byOpacity == 0xff)
				{
				TBltImageNormal<CGBlendHardLight> Painter;
				Painter.Blt(Dest, xDest, yDest, Src, xSrc, ySrc, cxSrc, cySrc);
				}
			else
				{
				TBltImageTrans<CGBlendHardLight> Painter(byOpacity);
				Painter.Blt(Dest, xDest, yDest, Src, xSrc, ySrc, cxSrc, cySrc);
				}
			break;
			}

		case blendScreen:
			{
			if (byOpacity == 0xff)
				{
				TBltImageNormal<CGBlendScreen> Painter;
				Painter.Blt(Dest, xDest, yDest, Src, xSrc, ySrc, cxSrc, cySrc);
				}
			else
				{
				TBltImageTrans<CGBlendScreen> Painter(byOpacity);
				Painter.Blt(Dest, xDest, yDest, Src, xSrc, ySrc, cxSrc, cySrc);
				}
			break;
			}

		case blendCompositeNormal:
			{
			if (byOpacity == 0xff)
				{
				TBltImageNormal<CGBlendComposite> Painter;
				Painter.Blt(Dest, xDest, yDest, Src, xSrc, ySrc, cxSrc, cySrc);
				}
			else
				{
				TBltImageTrans<CGBlendComposite> Painter(byOpacity);
				Painter.Blt(Dest, xDest, yDest, Src, xSrc, ySrc, cxSrc, cySrc);
				}
			break;
			}
		}
	}

void CGDraw::BltGray (CG32bitImage &Dest, int xDest, int yDest, CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc, BYTE byOpacity)

	{
	if (byOpacity == 0xff)
		{
		CFilterDesaturate Filter;
		Filter.Blt(Dest, xDest, yDest, Src, xSrc, ySrc, cxSrc, cySrc);
		}
	else
		{
		CFilterDesaturateTrans Filter(byOpacity);
		Filter.Blt(Dest, xDest, yDest, Src, xSrc, ySrc, cxSrc, cySrc);
		}
	}

void CGDraw::BltLighten (CG32bitImage &Dest, int xDest, int yDest, CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc)

	{
	CFilterLighten Filter;
	Filter.Blt(Dest, xDest, yDest, Src, xSrc, ySrc, cxSrc, cySrc);
	}

void CGDraw::BltMask (CG32bitImage &Dest, int xDest, int yDest, CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc, CG8bitImage &Mask, EBlendModes iMode)

	{
	switch (iMode)
		{
		case blendNormal:
			{
			TBltImageNormal<CGBlendBlend> Painter;
			Painter.BltMask(Dest, xDest, yDest, Src, xSrc, ySrc, cxSrc, cySrc, Mask);
			break;
			}

		case blendHardLight:
			{
			TBltImageNormal<CGBlendHardLight> Painter;
			Painter.BltMask(Dest, xDest, yDest, Src, xSrc, ySrc, cxSrc, cySrc, Mask);
			break;
			}

		case blendScreen:
			{
			TBltImageNormal<CGBlendScreen> Painter;
			Painter.BltMask(Dest, xDest, yDest, Src, xSrc, ySrc, cxSrc, cySrc, Mask);
			break;
			}

		case blendCompositeNormal:
			{
			TBltImageNormal<CGBlendComposite> Painter;
			Painter.BltMask(Dest, xDest, yDest, Src, xSrc, ySrc, cxSrc, cySrc, Mask);
			break;
			}
		}
	}

void CGDraw::BltMask0 (CG32bitImage &Dest, int xDest, int yDest, const CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc)

	{
	CFilterMask0 Filter;
	Filter.Blt(Dest, xDest, yDest, Src, xSrc, ySrc, cxSrc, cySrc);
	}

void CGDraw::BltScaled (CG32bitImage &Dest, int xDest, int yDest, int cxDest, int cyDest, CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc)

	{
	CFilterNormal Filter;
	Filter.BltScaled(Dest, xDest, yDest, cxDest, cyDest, Src, xSrc, ySrc, cxSrc, cySrc);
	}

void CGDraw::BltShimmer (CG32bitImage &Dest, int xDest, int yDest, CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc, BYTE byOpacity, DWORD dwSeed)

	{
	CFilterShimmer Filter(byOpacity, dwSeed);
	Filter.Blt(Dest, xDest, yDest, Src, xSrc, ySrc, cxSrc, cySrc);
	}

void CGDraw::BltTiled (CG32bitImage &Dest, int xDest, int yDest, int cxDest, int cyDest, const CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc, int xSrcOffset, int ySrcOffset)

	{
	if (Src.IsEmpty())
		return;

	if (cxSrc <= 0)
		cxSrc = Src.GetWidth();

	if (cySrc <= 0)
		cySrc = Src.GetHeight();

	xSrcOffset = xSrcOffset % cxSrc;
	ySrcOffset = ySrcOffset % cySrc;

	int cyLeft = cyDest;
	int yDestRow = yDest;
	int ySrcRow = ySrc + ySrcOffset;
	int cyRow = Min(cyDest, cySrc - ySrcOffset);

	while (cyLeft > 0)
		{
		int cxLeft = cxDest;
		int xDestTile = xDest;
		int xSrcTile = xSrc + xSrcOffset;
		int cxTile = Min(cxLeft, cxSrc - xSrcOffset);

		while (cxLeft > 0)
			{
			Dest.Blt(xSrcTile, ySrcRow, cxTile, cyRow, Src, xDestTile, yDestRow);

			xDestTile += cxTile;
			cxLeft -= cxTile;
			xSrcTile = xSrc;
			cxTile = Min(cxLeft, cxSrc);
			}

		yDestRow += cyRow;
		cyLeft -= cyRow;
		ySrcRow = ySrc;
		cyRow = Min(cyLeft, cySrc);
		}
	}

void CGDraw::BltTransformed (CG32bitImage &Dest, Metric rX, Metric rY, Metric rScaleX, Metric rScaleY, Metric rRotation, const CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc)

	{
	CFilterNormal Filter;
	Filter.BltTransformed(Dest, rX, rY, rScaleX, rScaleY, rRotation, Src, xSrc, ySrc, cxSrc, cySrc);
	}

void CGDraw::BltTransformedGray (CG32bitImage &Dest, Metric rX, Metric rY, Metric rScaleX, Metric rScaleY, Metric rRotation, const CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc, BYTE byOpacity)

	{
	if (byOpacity == 0xff)
		{
		CFilterDesaturate Filter;
		Filter.BltTransformed(Dest, rX, rY, rScaleX, rScaleY, rRotation, Src, xSrc, ySrc, cxSrc, cySrc);
		}
	else
		{
		CFilterDesaturateTrans Filter(byOpacity);
		Filter.BltTransformed(Dest, rX, rY, rScaleX, rScaleY, rRotation, Src, xSrc, ySrc, cxSrc, cySrc);
		}
	}

void CGDraw::BltTransformedHD (CG32bitImage &Dest, Metric rX, Metric rY, Metric rScaleX, Metric rScaleY, Metric rRotation, const CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc, EBlendModes iMode)

	{
	switch (iMode)
		{
		case blendNormal:
			{
			TBltImageNormal<CGBlendBlend> Painter;
			Painter.BltTransformedHD(Dest, CVector(rX, rY), CVector(rScaleX, rScaleY), rRotation, Src, xSrc, ySrc, cxSrc, cySrc);
			break;
			}

		case blendHardLight:
			{
			TBltImageNormal<CGBlendHardLight> Painter;
			Painter.BltTransformedHD(Dest, CVector(rX, rY), CVector(rScaleX, rScaleY), rRotation, Src, xSrc, ySrc, cxSrc, cySrc);
			break;
			}

		case blendScreen:
			{
			TBltImageNormal<CGBlendScreen> Painter;
			Painter.BltTransformedHD(Dest, CVector(rX, rY), CVector(rScaleX, rScaleY), rRotation, Src, xSrc, ySrc, cxSrc, cySrc);
			break;
			}

		case blendCompositeNormal:
			{
			TBltImageNormal<CGBlendComposite> Painter;
			Painter.BltTransformedHD(Dest, CVector(rX, rY), CVector(rScaleX, rScaleY), rRotation, Src, xSrc, ySrc, cxSrc, cySrc);
			break;
			}
		}
	}

void CGDraw::BltWithBackColor (CG32bitImage &Dest, int xDest, int yDest, CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc, CG32bitPixel rgbBackColor)
	
	{
	if (Src.GetAlphaType() == CG32bitImage::alphaNone)
		{
		CFilterBackColor Filter(rgbBackColor);
		Filter.Blt(Dest, xDest, yDest, Src, xSrc, ySrc, cxSrc, cySrc);
		}
	else
		Dest.Blt(xSrc, ySrc, cxSrc, cySrc, Src, xDest, yDest);
	}

void CGDraw::CopyColorize (CG32bitImage &Dest, int xDest, int yDest, CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc, Metric rHue, Metric rSaturation)

	{
	CFilterColorize Filter(rHue, rSaturation);
	Filter.Copy(Dest, xDest, yDest, Src, xSrc, ySrc, cxSrc, cySrc);
	}
