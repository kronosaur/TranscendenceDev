//	CG16bitRegion.cpp
//
//	CG16bitRegion class
//	Copyright (c) 2013 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

const DWORD ALLOC_GRANULARITY =				4096;

CG16bitRegion::CG16bitRegion (void) :
		m_cxWidth(0),
		m_cyHeight(0),
		m_dwAlloc(0),
		m_pCode(NULL)

//	CG16bitRegion constructor

	{
	}

void CG16bitRegion::AllocCode (DWORD dwNewAlloc)

//	AllocCode
//
//	Allocate code.

	{
	if (dwNewAlloc > m_dwAlloc)
		{
		WORD *pNewCode = new WORD [dwNewAlloc];

		//	Copy old code

		if (m_pCode)
			{
			WORD *pSrc = m_pCode;
			WORD *pSrcEnd = m_pCode + m_dwAlloc;
			WORD *pDest = pNewCode;

			while (pSrc < pSrcEnd)
				*pDest++ = *pSrc++;
			}

		//	Swap

		if (m_pCode)
			delete [] m_pCode;

		m_pCode = pNewCode;
		m_dwAlloc = dwNewAlloc;
		}
	}

void CG16bitRegion::Blt (CG32bitImage &Dest, int xDest, int yDest, CG32bitImage &Src, int xSrc, int ySrc, int cxWidth, int cyHeight)

//	Blt
//
//	Blt an image through this region.

	{
	int xSrcOriginal = xSrc;
	int ySrcOriginal = ySrc;

	//	Make sure we're in bounds

	if (!Dest.AdjustCoords(&xSrc, &ySrc, Src.GetWidth(), Src.GetHeight(), 
			&xDest, &yDest,
			&cxWidth, &cyHeight))
		return;

	int xMask = (xSrc - xSrcOriginal);
	int yMask = (ySrc - ySrcOriginal);

	//	Blt each line

	CG32bitPixel *pDstRow = Dest.GetPixelPos(xDest, yDest);
	CG32bitPixel *pSrcRow = Src.GetPixelPos(xSrc, ySrc);

	int iLine;
	int iLineEnd = yMask + cyHeight;
	for (iLine = yMask; iLine < iLineEnd; iLine++)
		{
		WORD *pCode = m_pCode + m_LineIndex[iLine];
		int cxOffset = xMask;
		int cxBlt = cxWidth;

		while (cxBlt > 0)
			{
			switch ((*pCode) & codeMask)
				{
				case code00:
					{
					int cxCount = ((*pCode) & countMask);
					pCode++;

					//	Deal with offset

					if (cxOffset)
						{
						int cxConsume = Min(cxCount, cxOffset);
						cxCount -= cxConsume;
						cxOffset -= cxConsume;
						}

					ASSERT(cxCount == 0 || cxOffset == 0);

					//	Skip any remaining

					if (cxCount > 0)
						{
						int cxConsume = Min(cxCount, cxBlt);
						cxBlt -= cxConsume;
						}

					break;
					}

				case codeRun:
					{
					int cxCount = ((*pCode) & countMask);
					int cxOriginalCount = cxCount;
					pCode++;
					BYTE *pRun = (BYTE *)pCode;

					//	Deal with offset

					if (cxOffset)
						{
						int cxConsume = Min(cxCount, cxOffset);
						cxCount -= cxConsume;
						cxOffset -= cxConsume;
						pRun += cxConsume;
						}

					ASSERT(cxCount == 0 || cxOffset == 0);

					//	Blt any remaining

					if (cxCount > 0)
						{
						int cxConsume = Min(cxCount, cxBlt);
						CG32bitPixel *pSrc = pSrcRow + (cxWidth - cxBlt);
						CG32bitPixel *pSrcEnd = pSrc + cxConsume;
						CG32bitPixel *pDest = pDstRow + (cxWidth - cxBlt);

						while (pSrc < pSrcEnd)
							{
							BYTE bySrcAlpha = pSrc->GetAlpha();

							if (bySrcAlpha == 0x00)
								;
							else if (bySrcAlpha == 0xff)
								*pDest = CG32bitPixel::Blend(*pDest, *pSrc, *pRun);
							else
								{
								CG32bitPixel rgbSrcPM = CG32bitPixel::Blend(0, *pSrc, *pRun);
								BYTE byFinalAlpha = CG32bitPixel::BlendAlpha(bySrcAlpha, *pRun);

								if (byFinalAlpha > 0)
									{
									BYTE *pAlphaInv = CG32bitPixel::AlphaTable(byFinalAlpha ^ 0xff);	//	Equivalent to 255 - byAlpha

									BYTE byRedResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDest->GetRed()] + (WORD)rgbSrcPM.GetRed()));
									BYTE byGreenResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDest->GetGreen()] + (WORD)rgbSrcPM.GetGreen()));
									BYTE byBlueResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDest->GetBlue()] + (WORD)rgbSrcPM.GetBlue()));

									*pDest = CG32bitPixel(byRedResult, byGreenResult, byBlueResult);
									}
								}

							pDest++;
							pSrc++;
							pRun++;
							}

						cxBlt -= cxConsume;

						ASSERT(cxBlt == 0 || cxConsume == cxCount);
						}

					pCode += AlignUp(cxOriginalCount, sizeof(WORD)) / 2;
					break;
					}

				case codeFF:
					{
					int cxCount = ((*pCode) & countMask);
					pCode++;

					//	Deal with offset

					if (cxOffset)
						{
						int cxConsume = Min(cxCount, cxOffset);
						cxCount -= cxConsume;
						cxOffset -= cxConsume;
						}

					ASSERT(cxCount == 0 || cxOffset == 0);

					//	Blt any remaining

					if (cxCount > 0)
						{
						int cxConsume = Min(cxCount, cxBlt);
						CG32bitPixel *pSrc = pSrcRow + (cxWidth - cxBlt);
						CG32bitPixel *pSrcEnd = pSrc + cxConsume;
						CG32bitPixel *pDest = pDstRow + (cxWidth - cxBlt);

						while (pSrc < pSrcEnd)
							{
							BYTE bySrcAlpha = pSrc->GetAlpha();

							if (bySrcAlpha == 0x00)
								;
							else if (bySrcAlpha == 0xff)
								*pDest = *pSrc;
							else
								{
								BYTE *pAlphaInv = CG32bitPixel::AlphaTable(bySrcAlpha ^ 0xff);	//	Equivalent to 255 - byAlpha

								BYTE byRedResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDest->GetRed()] + (WORD)pSrc->GetRed()));
								BYTE byGreenResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDest->GetGreen()] + (WORD)pSrc->GetGreen()));
								BYTE byBlueResult = (BYTE)Min((WORD)0xff, (WORD)(pAlphaInv[pDest->GetBlue()] + (WORD)pSrc->GetBlue()));

								*pDest = CG32bitPixel(byRedResult, byGreenResult, byBlueResult);
								}

							pDest++;
							pSrc++;
							}

						cxBlt -= cxConsume;

						ASSERT(cxBlt == 0 || cxConsume == cxCount);
						}

					break;
					}

				case codeEndOfLine:
					{
					cxBlt = 0;
					break;
					}

				default:
					ASSERT(false);
				}
			}

		//	Next row

		pSrcRow = Src.NextRow(pSrcRow);
		pDstRow = Dest.NextRow(pDstRow);
		}
	}

void CG16bitRegion::CleanUp (void)

//	CleanUp
//
//	Clean up the region

	{
	if (m_pCode)
		{
		delete [] m_pCode;
		m_pCode = NULL;
		m_dwAlloc = 0;
		}

	m_LineIndex.DeleteAll();
	m_cxWidth = 0;
	m_cyHeight = 0;
	}

void CG16bitRegion::ColorTransBlt (CG16bitImage &Dest, int xDest, int yDest, CG16bitImage &Src, int xSrc, int ySrc, int cxWidth, int cyHeight)

//	ColorTransBlt
//
//	Paints

	{
	int xSrcOriginal = xSrc;
	int ySrcOriginal = ySrc;

	//	Make sure we're in bounds

	if (!Dest.AdjustCoords(&xSrc, &ySrc, Src.GetWidth(), Src.GetHeight(), 
			&xDest, &yDest,
			&cxWidth, &cyHeight))
		return;

	int xMask = (xSrc - xSrcOriginal);
	int yMask = (ySrc - ySrcOriginal);

	//	Blt based on source image

	if (Src.HasAlpha())
		ColorTransBltAlpha(Dest, xDest, yDest, Src, xSrc, ySrc, cxWidth, cyHeight, xMask, yMask);
	else if (Src.IsTransparent())
		ColorTransBltBackColor(Dest, xDest, yDest, Src, xSrc, ySrc, cxWidth, cyHeight, xMask, yMask);
	else
		ColorTransBltPlain(Dest, xDest, yDest, Src, xSrc, ySrc, cxWidth, cyHeight, xMask, yMask);
	}

void CG16bitRegion::ColorTransBltAlpha (CG16bitImage &Dest, int xDest, int yDest, CG16bitImage &Src, int xSrc, int ySrc, int cxWidth, int cyHeight, int xMask, int yMask)

//	ColorTransBltAlpha
//
//	Paints

	{
	//	Blt each line

	WORD *pDstRow = Dest.GetPixel(Dest.GetRowStart(yDest), xDest);
	WORD *pSrcRow = Src.GetPixel(Src.GetRowStart(ySrc), xSrc);
	BYTE *pSrcAlphaRow = Src.GetAlphaRow(ySrc) + xSrc;

	int iLine;
	int iLineEnd = yMask + cyHeight;
	for (iLine = yMask; iLine < iLineEnd; iLine++)
		{
		WORD *pCode = m_pCode + m_LineIndex[iLine];
		int cxOffset = xMask;
		int cxBlt = cxWidth;

		while (cxBlt > 0)
			{
			switch ((*pCode) & codeMask)
				{
				case code00:
					{
					int cxCount = ((*pCode) & countMask);
					pCode++;

					//	Deal with offset

					if (cxOffset)
						{
						int cxConsume = Min(cxCount, cxOffset);
						cxCount -= cxConsume;
						cxOffset -= cxConsume;
						}

					ASSERT(cxCount == 0 || cxOffset == 0);

					//	Skip any remaining

					if (cxCount > 0)
						{
						int cxConsume = Min(cxCount, cxBlt);
						cxBlt -= cxConsume;
						}

					break;
					}

				case codeRun:
					{
					int cxCount = ((*pCode) & countMask);
					int cxOriginalCount = cxCount;
					pCode++;
					BYTE *pRun = (BYTE *)pCode;

					//	Deal with offset

					if (cxOffset)
						{
						int cxConsume = Min(cxCount, cxOffset);
						cxCount -= cxConsume;
						cxOffset -= cxConsume;
						pRun += cxConsume;
						}

					ASSERT(cxCount == 0 || cxOffset == 0);

					//	Blt any remaining

					if (cxCount > 0)
						{
						int cxConsume = Min(cxCount, cxBlt);
						WORD *pSrc = pSrcRow + (cxWidth - cxBlt);
						WORD *pSrcEnd = pSrc + cxConsume;
						BYTE *pSrcAlpha = pSrcAlphaRow + (cxWidth - cxBlt);
						WORD *pDest = pDstRow + (cxWidth - cxBlt);

						while (pSrc < pSrcEnd)
							{
							if (*pSrcAlpha == 0)
								;
							else if (*pSrcAlpha == 0xFF)
								*pDest = CG16bitImage::BlendPixel(*pDest, *pSrc, *pRun);
							else
								{
								WORD wSrcPM = CG16bitImage::BlendPixel(0, *pSrc, *pRun);
								DWORD dwTrans = (DWORD)(*pSrcAlpha) * (DWORD)(*pRun) / 0xFF;

								if (dwTrans > 0)
									*pDest = CG16bitImage::BlendPixelPM(*pDest, wSrcPM, (BYTE)(dwTrans));
								}

							pDest++;
							pSrc++;
							pSrcAlpha++;
							pRun++;
							}

						cxBlt -= cxConsume;

						ASSERT(cxBlt == 0 || cxConsume == cxCount);
						}

					pCode += AlignUp(cxOriginalCount, sizeof(WORD)) / 2;
					break;
					}

				case codeFF:
					{
					int cxCount = ((*pCode) & countMask);
					pCode++;

					//	Deal with offset

					if (cxOffset)
						{
						int cxConsume = Min(cxCount, cxOffset);
						cxCount -= cxConsume;
						cxOffset -= cxConsume;
						}

					ASSERT(cxCount == 0 || cxOffset == 0);

					//	Blt any remaining

					if (cxCount > 0)
						{
						int cxConsume = Min(cxCount, cxBlt);
						WORD *pSrc = pSrcRow + (cxWidth - cxBlt);
						WORD *pSrcEnd = pSrc + cxConsume;
						BYTE *pSrcAlpha = pSrcAlphaRow + (cxWidth - cxBlt);
						WORD *pDest = pDstRow + (cxWidth - cxBlt);

						while (pSrc < pSrcEnd)
							{
							if (*pSrcAlpha == 0x00)
								;
							else if (*pSrcAlpha == 0xFF)
								*pDest = *pSrc;
							else
								*pDest = CG16bitImage::BlendPixelPM(*pDest, *pSrc, *pSrcAlpha);

							pDest++;
							pSrc++;
							pSrcAlpha++;
							}

						cxBlt -= cxConsume;

						ASSERT(cxBlt == 0 || cxConsume == cxCount);
						}

					break;
					}

				case codeEndOfLine:
					{
					cxBlt = 0;
					break;
					}

				default:
					ASSERT(false);
				}
			}

		//	Next row

		pSrcRow = Src.NextRow(pSrcRow);
		pDstRow = Dest.NextRow(pDstRow);
		pSrcAlphaRow = Src.NextAlphaRow(pSrcAlphaRow);
		}
	}

void CG16bitRegion::ColorTransBltBackColor (CG16bitImage &Dest, int xDest, int yDest, CG16bitImage &Src, int xSrc, int ySrc, int cxWidth, int cyHeight, int xMask, int yMask)

//	ColorTransBltBackColor
//
//	Paints

	{
	//	Blt each line

	WORD wBackColor = Src.GetBackColor();
	WORD *pDstRow = Dest.GetPixel(Dest.GetRowStart(yDest), xDest);
	WORD *pSrcRow = Src.GetPixel(Src.GetRowStart(ySrc), xSrc);

	int iLine;
	int iLineEnd = yMask + cyHeight;
	for (iLine = yMask; iLine < iLineEnd; iLine++)
		{
		WORD *pCode = m_pCode + m_LineIndex[iLine];
		int cxOffset = xMask;
		int cxBlt = cxWidth;

		while (cxBlt > 0)
			{
			switch ((*pCode) & codeMask)
				{
				case code00:
					{
					int cxCount = ((*pCode) & countMask);
					pCode++;

					//	Deal with offset

					if (cxOffset)
						{
						int cxConsume = Min(cxCount, cxOffset);
						cxCount -= cxConsume;
						cxOffset -= cxConsume;
						}

					ASSERT(cxCount == 0 || cxOffset == 0);

					//	Skip any remaining

					if (cxCount > 0)
						{
						int cxConsume = Min(cxCount, cxBlt);
						cxBlt -= cxConsume;
						}

					break;
					}

				case codeRun:
					{
					int cxCount = ((*pCode) & countMask);
					int cxOriginalCount = cxCount;
					pCode++;
					BYTE *pRun = (BYTE *)pCode;

					//	Deal with offset

					if (cxOffset)
						{
						int cxConsume = Min(cxCount, cxOffset);
						cxCount -= cxConsume;
						cxOffset -= cxConsume;
						pRun += cxConsume;
						}

					ASSERT(cxCount == 0 || cxOffset == 0);

					//	Blt any remaining

					if (cxCount > 0)
						{
						int cxConsume = Min(cxCount, cxBlt);
						WORD *pSrc = pSrcRow + (cxWidth - cxBlt);
						WORD *pSrcEnd = pSrc + cxConsume;
						WORD *pDest = pDstRow + (cxWidth - cxBlt);

						while (pSrc < pSrcEnd)
							{
							if (*pSrc != wBackColor)
								*pDest = CG16bitImage::BlendPixel(*pDest, *pSrc, *pRun);

							pDest++;
							pSrc++;
							pRun++;
							}

						cxBlt -= cxConsume;

						ASSERT(cxBlt == 0 || cxConsume == cxCount);
						}

					pCode += AlignUp(cxOriginalCount, sizeof(WORD)) / 2;
					break;
					}

				case codeFF:
					{
					int cxCount = ((*pCode) & countMask);
					pCode++;

					//	Deal with offset

					if (cxOffset)
						{
						int cxConsume = Min(cxCount, cxOffset);
						cxCount -= cxConsume;
						cxOffset -= cxConsume;
						}

					ASSERT(cxCount == 0 || cxOffset == 0);

					//	Blt any remaining

					if (cxCount > 0)
						{
						int cxConsume = Min(cxCount, cxBlt);
						WORD *pSrc = pSrcRow + (cxWidth - cxBlt);
						WORD *pSrcEnd = pSrc + cxConsume;
						WORD *pDest = pDstRow + (cxWidth - cxBlt);

						while (pSrc < pSrcEnd)
							{
							if (*pSrc != wBackColor)
								*pDest = *pSrc;

							pDest++;
							pSrc++;
							}

						cxBlt -= cxConsume;

						ASSERT(cxBlt == 0 || cxConsume == cxCount);
						}

					break;
					}

				case codeEndOfLine:
					{
					cxBlt = 0;
					break;
					}

				default:
					ASSERT(false);
				}
			}

		//	Next row

		pSrcRow = Src.NextRow(pSrcRow);
		pDstRow = Dest.NextRow(pDstRow);
		}
	}

void CG16bitRegion::ColorTransBltPlain (CG16bitImage &Dest, int xDest, int yDest, CG16bitImage &Src, int xSrc, int ySrc, int cxWidth, int cyHeight, int xMask, int yMask)

//	ColorTransBltPlain
//
//	Paints

	{
	//	Blt each line

	WORD *pDstRow = Dest.GetPixel(Dest.GetRowStart(yDest), xDest);
	WORD *pSrcRow = Src.GetPixel(Src.GetRowStart(ySrc), xSrc);

	int iLine;
	int iLineEnd = yMask + cyHeight;
	for (iLine = yMask; iLine < iLineEnd; iLine++)
		{
		WORD *pCode = m_pCode + m_LineIndex[iLine];
		int cxOffset = xMask;
		int cxBlt = cxWidth;

		while (cxBlt > 0)
			{
			switch ((*pCode) & codeMask)
				{
				case code00:
					{
					int cxCount = ((*pCode) & countMask);
					pCode++;

					//	Deal with offset

					if (cxOffset)
						{
						int cxConsume = Min(cxCount, cxOffset);
						cxCount -= cxConsume;
						cxOffset -= cxConsume;
						}

					ASSERT(cxCount == 0 || cxOffset == 0);

					//	Skip any remaining

					if (cxCount > 0)
						{
						int cxConsume = Min(cxCount, cxBlt);
						cxBlt -= cxConsume;
						}

					break;
					}

				case codeRun:
					{
					int cxCount = ((*pCode) & countMask);
					int cxOriginalCount = cxCount;
					pCode++;
					BYTE *pRun = (BYTE *)pCode;

					//	Deal with offset

					if (cxOffset)
						{
						int cxConsume = Min(cxCount, cxOffset);
						cxCount -= cxConsume;
						cxOffset -= cxConsume;
						pRun += cxConsume;
						}

					ASSERT(cxCount == 0 || cxOffset == 0);

					//	Blt any remaining

					if (cxCount > 0)
						{
						int cxConsume = Min(cxCount, cxBlt);
						WORD *pSrc = pSrcRow + (cxWidth - cxBlt);
						WORD *pSrcEnd = pSrc + cxConsume;
						WORD *pDest = pDstRow + (cxWidth - cxBlt);

						while (pSrc < pSrcEnd)
							{
							*pDest = CG16bitImage::BlendPixel(*pDest, *pSrc, *pRun);
							pDest++;
							pSrc++;
							pRun++;
							}

						cxBlt -= cxConsume;

						ASSERT(cxBlt == 0 || cxConsume == cxCount);
						}

					pCode += AlignUp(cxOriginalCount, sizeof(WORD)) / 2;
					break;
					}

				case codeFF:
					{
					int cxCount = ((*pCode) & countMask);
					pCode++;

					//	Deal with offset

					if (cxOffset)
						{
						int cxConsume = Min(cxCount, cxOffset);
						cxCount -= cxConsume;
						cxOffset -= cxConsume;
						}

					ASSERT(cxCount == 0 || cxOffset == 0);

					//	Blt any remaining

					if (cxCount > 0)
						{
						int cxConsume = Min(cxCount, cxBlt);
						WORD *pSrc = pSrcRow + (cxWidth - cxBlt);
						WORD *pSrcEnd = pSrc + cxConsume;
						WORD *pDest = pDstRow + (cxWidth - cxBlt);

						while (pSrc < pSrcEnd)
							*pDest++ = *pSrc++;

						cxBlt -= cxConsume;

						ASSERT(cxBlt == 0 || cxConsume == cxCount);
						}

					break;
					}

				case codeEndOfLine:
					{
					cxBlt = 0;
					break;
					}

				default:
					ASSERT(false);
				}
			}

		//	Next row

		pSrcRow = Src.NextRow(pSrcRow);
		pDstRow = Dest.NextRow(pDstRow);
		}
	}

bool CG16bitRegion::CreateFromMask (const CG8bitImage &Source, int xSrc, int ySrc, int cxWidth, int cyHeight)

//	CreateFromMask
//
//	Creates a region from an 8-bit image

	{
	int i;

	enum EStates
		{
		stateStart,
		state00,
		stateRun,
		stateFF,
		};

	CleanUp();

	//	Make sure we're in bounds

	if (!Source.AdjustCoords(NULL, NULL, 0, 0, 
			&xSrc, &ySrc,
			&cxWidth, &cyHeight))
		return true;

	//	Null case

	if (Source.IsEmpty() || cxWidth == 0 || cyHeight == 0)
		return true;

	//	Get the mask size

	m_cxWidth = cxWidth;
	m_cyHeight = cyHeight;

	//	Allocate destination

	DWORD dwCodeSize = 0;
	AllocCode(ALLOC_GRANULARITY);
	WORD *pCode = m_pCode;
	m_LineIndex.InsertEmpty(m_cyHeight);

	//	Loop over all lines

	for (i = 0; i < m_cyHeight; i++)
		{
		EStates iState = stateStart;
		DWORD dwCount;
		BYTE *pRun;

		//	Remember the offset into the given line

		m_LineIndex[i] = dwCodeSize;

		//	Loop

		BYTE *pSrc = Source.GetPixelPos(xSrc, ySrc + i);
		BYTE *pSrcEnd = pSrc + m_cxWidth;
		while (pSrc < pSrcEnd)
			{
			switch (iState)
				{
				case stateStart:
					if (*pSrc == 0x00)
						{
						dwCount = 1;
						iState = state00;
						}
					else if (*pSrc == 0xFF)
						{
						dwCount = 1;
						iState = stateFF;
						}
					else
						{
						pRun = pSrc;
						iState = stateRun;
						}
					break;

				case state00:
					if (*pSrc == 0x00)
						dwCount++;
					else if (*pSrc == 0xFF)
						{
						WriteCode(pCode, dwCodeSize, code00, dwCount);
						dwCount = 1;
						iState = stateFF;
						}
					else
						{
						WriteCode(pCode, dwCodeSize, code00, dwCount);
						pRun = pSrc;
						iState = stateRun;
						}
					break;

				case stateRun:
					if (*pSrc == 0x00)
						{
						WriteCodeRun(pCode, dwCodeSize, pRun, (DWORD)(pSrc - pRun));
						dwCount = 1;
						iState = state00;
						}
					else if (*pSrc == 0xFF)
						{
						WriteCodeRun(pCode, dwCodeSize, pRun, (DWORD)(pSrc - pRun));
						dwCount = 1;
						iState = stateFF;
						}
					break;

				case stateFF:
					if (*pSrc == 0x00)
						{
						WriteCode(pCode, dwCodeSize, codeFF, dwCount);
						dwCount = 1;
						iState = state00;
						}
					else if (*pSrc == 0xFF)
						dwCount++;
					else
						{
						WriteCode(pCode, dwCodeSize, codeFF, dwCount);
						pRun = pSrc;
						iState = stateRun;
						}
					break;
				}

			pSrc++;
			}

		//	Done with line

		switch (iState)
			{
			case state00:
				WriteCode(pCode, dwCodeSize, code00, dwCount);
				break;

			case stateRun:
				WriteCodeRun(pCode, dwCodeSize, pRun, (DWORD)(pSrc - pRun));
				break;

			case stateFF:
				WriteCode(pCode, dwCodeSize, codeFF, dwCount);
				break;
			}

		WriteCode(pCode, dwCodeSize, codeEndOfLine, 0);
		}

	return true;
	}

bool CG16bitRegion::CreateFromMask (const CG16bitImage &Source, int xSrc, int ySrc, int cxWidth, int cyHeight)

//	CreateFromMask
//
//	Creates a region from an alpha mask.

	{
	int i;

	enum EStates
		{
		stateStart,
		state00,
		stateRun,
		stateFF,
		};

	CleanUp();

	//	Make sure we're in bounds

	if (!Source.AdjustCoords(NULL, NULL, 0, 0, 
			&xSrc, &ySrc,
			&cxWidth, &cyHeight))
		return true;

	//	Null case

	if (!Source.HasAlpha() || cxWidth == 0 || cyHeight == 0)
		return true;

	//	Get the mask size

	m_cxWidth = cxWidth;
	m_cyHeight = cyHeight;

	//	Allocate destination

	DWORD dwCodeSize = 0;
	AllocCode(ALLOC_GRANULARITY);
	WORD *pCode = m_pCode;
	m_LineIndex.InsertEmpty(m_cyHeight);

	//	Loop over all lines

	for (i = 0; i < m_cyHeight; i++)
		{
		EStates iState = stateStart;
		DWORD dwCount;
		BYTE *pRun;

		//	Remember the offset into the given line

		m_LineIndex[i] = dwCodeSize;

		//	Loop

		BYTE *pSrc = Source.GetAlphaRow(ySrc + i) + xSrc;
		BYTE *pSrcEnd = pSrc + m_cxWidth;
		while (pSrc < pSrcEnd)
			{
			switch (iState)
				{
				case stateStart:
					if (*pSrc == 0x00)
						{
						dwCount = 1;
						iState = state00;
						}
					else if (*pSrc == 0xFF)
						{
						dwCount = 1;
						iState = stateFF;
						}
					else
						{
						pRun = pSrc;
						iState = stateRun;
						}
					break;

				case state00:
					if (*pSrc == 0x00)
						dwCount++;
					else if (*pSrc == 0xFF)
						{
						WriteCode(pCode, dwCodeSize, code00, dwCount);
						dwCount = 1;
						iState = stateFF;
						}
					else
						{
						WriteCode(pCode, dwCodeSize, code00, dwCount);
						pRun = pSrc;
						iState = stateRun;
						}
					break;

				case stateRun:
					if (*pSrc == 0x00)
						{
						WriteCodeRun(pCode, dwCodeSize, pRun, (DWORD)(pSrc - pRun));
						dwCount = 1;
						iState = state00;
						}
					else if (*pSrc == 0xFF)
						{
						WriteCodeRun(pCode, dwCodeSize, pRun, (DWORD)(pSrc - pRun));
						dwCount = 1;
						iState = stateFF;
						}
					break;

				case stateFF:
					if (*pSrc == 0x00)
						{
						WriteCode(pCode, dwCodeSize, codeFF, dwCount);
						dwCount = 1;
						iState = state00;
						}
					else if (*pSrc == 0xFF)
						dwCount++;
					else
						{
						WriteCode(pCode, dwCodeSize, codeFF, dwCount);
						pRun = pSrc;
						iState = stateRun;
						}
					break;
				}

			pSrc++;
			}

		//	Done with line

		switch (iState)
			{
			case state00:
				WriteCode(pCode, dwCodeSize, code00, dwCount);
				break;

			case stateRun:
				WriteCodeRun(pCode, dwCodeSize, pRun, (DWORD)(pSrc - pRun));
				break;

			case stateFF:
				WriteCode(pCode, dwCodeSize, codeFF, dwCount);
				break;
			}

		WriteCode(pCode, dwCodeSize, codeEndOfLine, 0);
		}

	return true;
	}

bool CG16bitRegion::CreateFromMask (const CG32bitImage &Source, int xSrc, int ySrc, int cxWidth, int cyHeight)

//	CreateFromMask
//
//	Creates a region from an alpha mask.

	{
	//	Copy the alpha channel of the 32-bit image to an 8-bit channel

	CG8bitImage Mask;
	if (!Mask.Create(cxWidth, cyHeight))
		return false;

	CG32bitPixel *pSrcRow = Source.GetPixelPos(xSrc, ySrc);
	CG32bitPixel *pSrcRowEnd = Source.GetPixelPos(xSrc, ySrc + cyHeight);
	BYTE *pDestRow = Mask.GetPixelPos(0, 0);

	while (pSrcRow < pSrcRowEnd)
		{
		CG32bitPixel *pSrc = pSrcRow;
		CG32bitPixel *pSrcEnd = pSrc + cxWidth;
		BYTE *pDest = pDestRow;
		while (pSrc < pSrcEnd)
			{
			*pDest = pSrc->GetAlpha();
			pSrc++;
			pDest++;
			}

		pSrcRow = Source.NextRow(pSrcRow);
		pDestRow = Mask.NextRow(pDestRow);
		}

	//	Now create from the 8-bit mask

	return CreateFromMask(Mask, 0, 0, cxWidth, cyHeight);
	}

void CG16bitRegion::WriteCode (WORD *&pCode, DWORD &dwCodeSize, DWORD dwCode, DWORD dwCount)

//	WriteCode
//
//	Writes the given code

	{
	if (dwCodeSize + 1 > m_dwAlloc)
		{
		AllocCode(m_dwAlloc + ALLOC_GRANULARITY);
		pCode = m_pCode + dwCodeSize;
		}

	*pCode++ = (WORD)(dwCode | (dwCount & countMask));
	dwCodeSize++;
	}

void CG16bitRegion::WriteCodeRun (WORD *&pCode, DWORD &dwCodeSize, BYTE *pRun, DWORD dwCount)

//	WriteCodeRun
//
//	Writes the given code

	{
	DWORD dwCodeLength = 1 + (AlignUp(dwCount, sizeof(WORD)) / 2);
	if (dwCodeSize + dwCodeLength > m_dwAlloc)
		{
		AllocCode(m_dwAlloc + AlignUp(dwCodeLength, ALLOC_GRANULARITY));
		pCode = m_pCode + dwCodeSize;
		}

	*pCode = (WORD)(codeRun | (dwCount & countMask));

	BYTE *pRunEnd = pRun + dwCount;
	BYTE *pDest = (BYTE *)(pCode + 1);
	while (pRun < pRunEnd)
		*pDest++ = *pRun++;

	dwCodeSize += dwCodeLength;
	pCode += dwCodeLength;
	}
