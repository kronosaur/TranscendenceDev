//	CG16bitImage.cpp
//
//	Implementation of raw 16-bit image object
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

//#define DEBUG_TIME

const int IMAGE_SAVE_VERSION =				1;
const int RGB_COMPRESSION_NONE =			0;
const int ALPHA_COMPRESSION_NONE =			0;

int g_iTemp;
#define IntMult(wColor, wScale)		(g_iTemp = (DWORD)(wColor) * (DWORD)(wScale) + 0x80, (DWORD)(((g_iTemp >> 8) + g_iTemp) >> 8))

bool InitBlendPixelTables (void);

AlphaArray5 g_Alpha5 [256];
AlphaArray6 g_Alpha6 [256];
bool g_bInit = InitBlendPixelTables();

CG16bitImage::CG16bitImage (void) : 
		m_pRGB(NULL),
		m_pAlpha(NULL),
		m_wBackColor(0),
		m_bHasMask(false),
		m_pRedAlphaTable(NULL),
		m_pGreenAlphaTable(NULL),
		m_pBlueAlphaTable(NULL),
		m_pSprite(NULL),
		m_pBMI(NULL)

//	CG16bitImage constructor

	{
	}

CG16bitImage::CG16bitImage (const CG16bitImage &Src)

//	CG16bitImage copy constructor

	{
	CopyData(Src);
	}

CG16bitImage::~CG16bitImage (void)

//	CG16bitImage destructor

	{
	DeleteData();
	}

CG16bitImage &CG16bitImage::operator= (const CG16bitImage &Src)

//	CG16bitImage operator =

	{
	DeleteData();
	CopyData(Src);
	return *this;
	}

int CG16bitImage::AdjustTextX (const CG16bitFont &Font, const CString &sText, AlignmentStyles iAlign, int x)

//	AdjustTextX
//
//	Adjusts the x coordinate to paint the text at, accounting for:
//
//	1.	alignment, offsetting if necessary
//	2.	clipping, offsetting to fit in clip region.

	{
	int cxText = Font.MeasureText(sText);

	if (iAlign & alignCenter)
		x -= (cxText / 2);
	else if (iAlign & alignRight)
		x -= cxText;

	return Min(Max((int)m_rcClip.left, x), (int)m_rcClip.right - cxText);
	}

WORD CG16bitImage::BlendPixel (WORD pxDest, WORD pxSource, DWORD byOpacity)

//	BlendPixel
//
//	Blends the dest and the source according to byTrans

	{
#ifndef OLD_BLEND
	DWORD dTemp = pxDest;
	DWORD sTemp = pxSource;

	BYTE *pAlpha5 = g_Alpha5[byOpacity];
	BYTE *pAlpha6 = g_Alpha6[byOpacity];
	BYTE *pAlpha5Inv = g_Alpha5[255 - byOpacity];
	BYTE *pAlpha6Inv = g_Alpha6[255 - byOpacity];

	DWORD dwRedResult = pAlpha5Inv[(dTemp & 0xf800) >> 11] + pAlpha5[(sTemp & 0xf800) >> 11];
	DWORD dwGreenResult = pAlpha6Inv[(dTemp & 0x7e0) >> 5] + pAlpha6[(sTemp & 0x7e0) >> 5];
	DWORD dwBlueResult = pAlpha5Inv[(dTemp & 0x1f)] + pAlpha5[(sTemp & 0x1f)];

	return (WORD)((dwRedResult << 11) | (dwGreenResult << 5) | (dwBlueResult));

#else
	DWORD sTemp = pxSource;
	DWORD dTemp = pxDest;

	DWORD sb = sTemp & 0x1f;
	DWORD db = dTemp & 0x1f;
	DWORD sg = (sTemp >> 5) & 0x3f;
	DWORD dg = (dTemp >> 5) & 0x3f;
	DWORD sr = (sTemp >> 11) & 0x1f;
	DWORD dr = (dTemp >> 11) & 0x1f;

	return (WORD)((byOpacity * (sb - db) >> 8) + db |
			((byOpacity * (sg - dg) >> 8) + dg) << 5 |
			((byOpacity * (sr - dr) >> 8) + dr) << 11);
#endif
	}

WORD CG16bitImage::BlendPixelGray (WORD pxDest, WORD pxSource, DWORD byOpacity)

//	BlendPixelGray
//
//	Blends the dest and the source according to byOpacity. Uses only 5-bits of data for the
//	green channel to preserve the relative weight of all channels. [Without this, sometimes
//	gray colors turn green or purple, because of uneven round-off.]

	{
	DWORD dTemp = pxDest;
	DWORD sTemp = pxSource;

	BYTE *pAlpha5 = g_Alpha5[byOpacity];
	BYTE *pAlpha5Inv = g_Alpha5[255 - byOpacity];

	DWORD dwRedResult = pAlpha5Inv[(dTemp & 0xf800) >> 11] + pAlpha5[(sTemp & 0xf800) >> 11];
	DWORD dwGreenResult = pAlpha5Inv[(dTemp & 0x7e0) >> 6] + pAlpha5[(sTemp & 0x7e0) >> 6];
	DWORD dwBlueResult = pAlpha5Inv[(dTemp & 0x1f)] + pAlpha5[(sTemp & 0x1f)];

	return (WORD)((dwRedResult << 11) | (dwGreenResult << 6) | (dwBlueResult));
	}

WORD CG16bitImage::BlendPixelPM (DWORD pxDest, DWORD pxSource, DWORD byOpacity)

//	BlendPixelPM
//
//	Blends the dest and the source according to byTrans assuming that the
//	source and destination use pre-multiplied values

	{
	DWORD dwInvTrans = (byOpacity ^ 0xff);

	DWORD dwRedGreenS = ((pxSource << 8) & 0x00f80000) | (pxSource & 0x000007e0);
	DWORD dwRedGreen = (((((pxDest << 8) & 0x00f80000) | (pxDest & 0x000007e0)) * dwInvTrans) >> 8) + dwRedGreenS;

	DWORD dwBlue = (((pxDest & 0x1f) * dwInvTrans) >> 8) + (pxSource & 0x1f);

	return (WORD)(((dwRedGreen & 0x00f80000) >> 8) | (dwRedGreen & 0x000007e0) | dwBlue);
	}

void CG16bitImage::Blt (int xSrc, int ySrc, int cxWidth, int cyHeight, const CG16bitImage &Source, int xDest, int yDest)

//	Blt
//
//	Blt the image to the destination

	{
	//	Make sure we're in bounds

	if (!AdjustCoords(&xSrc, &ySrc, Source.m_cxWidth, Source.m_cyHeight, 
			&xDest, &yDest,
			&cxWidth, &cyHeight))
		return;

	//	Do the blt

	WORD *pSrcRow = Source.GetPixel(Source.GetRowStart(ySrc), xSrc);
	WORD *pSrcRowEnd = Source.GetPixel(Source.GetRowStart(ySrc + cyHeight), xSrc);
	WORD *pDestRow = GetPixel(GetRowStart(yDest), xDest);

	while (pSrcRow < pSrcRowEnd)
		{
		WORD *pSrcPos = pSrcRow;
		WORD *pSrcPosEnd = pSrcRow + cxWidth;
		WORD *pDestPos = pDestRow;

		while (pSrcPos < pSrcPosEnd)
			*pDestPos++ = *pSrcPos++;

		pSrcRow = Source.NextRow(pSrcRow);
		pDestRow = NextRow(pDestRow);
		}
	}

void CG16bitImage::BltGray (int xSrc, int ySrc, int cxWidth, int cyHeight, int iAlpha, const CG16bitImage &Source, int xDest, int yDest)

//	Blt
//
//	Blt the image to the destination

	{
	//	Deal with sprite sources

	if (Source.m_pSprite)
		{
		Source.m_pSprite->ColorTransBlt(*this,
				xDest,
				yDest,
				xSrc,
				ySrc,
				cxWidth,
				cyHeight);
		return;
		}

	//	Make sure we're in bounds

	if (!AdjustCoords(&xSrc, &ySrc, Source.m_cxWidth, Source.m_cyHeight, 
			&xDest, &yDest,
			&cxWidth, &cyHeight))
		return;

	WORD *pSrcRow = Source.GetPixel(Source.GetRowStart(ySrc), xSrc);
	WORD *pSrcRowEnd = Source.GetPixel(Source.GetRowStart(ySrc + cyHeight), xSrc);
	WORD *pDestRow = GetPixel(GetRowStart(yDest), xDest);

	//	If we've got an alpha mask then blt using the transparency
	//	information.

	if (Source.m_pAlpha)
		{
#ifdef LATER
		BYTE *pAlphaSrcRow = Source.GetAlphaValue(xSrc, ySrc);

		while (pSrcRow < pSrcRowEnd)
			{
			BYTE *pAlphaPos = pAlphaSrcRow;
			WORD *pSrcPos = pSrcRow;
			WORD *pSrcPosEnd = pSrcRow + cxWidth;
			WORD *pDestPos = pDestRow;

			while (pSrcPos < pSrcPosEnd)
				if (*pAlphaPos == 0)
					{
					pDestPos++;
					pSrcPos++;
					pAlphaPos++;
					}
				else if (*pAlphaPos == 255 || *pDestPos == 0)
					{
					*pDestPos++ = *pSrcPos++;
					pAlphaPos++;
					}
				else
					{
					DWORD pxSource = *pSrcPos;
					DWORD pxDest = *pDestPos;
					DWORD dwInvTrans = (((DWORD)(*pAlphaPos)) ^ 0xff);

					DWORD dwRedGreenS = ((pxSource << 8) & 0x00f80000) | (pxSource & 0x000007e0);
					DWORD dwRedGreen = (((((pxDest << 8) & 0x00f80000) | (pxDest & 0x000007e0)) * dwInvTrans) >> 8) + dwRedGreenS;
					DWORD dwBlue = (((pxDest & 0x1f) * dwInvTrans) >> 8) + (pxSource & 0x1f);

					*pDestPos++ = (WORD)(((dwRedGreen & 0x00f80000) >> 8) | (dwRedGreen & 0x000007e0) | dwBlue);

					pSrcPos++;
					pAlphaPos++;
					}

			pSrcRow = Source.NextRow(pSrcRow);
			pDestRow = NextRow(pDestRow);
			pAlphaSrcRow = Source.NextAlphaRow(pAlphaSrcRow);
			}
#endif
		}

	//	If we've got constant transparency then use the alpha tables

	else if (Source.IsTransparent())
		{
#ifdef LATER
		while (pSrcRow < pSrcRowEnd)
			{
			WORD *pSrcPos = pSrcRow;
			WORD *pSrcPosEnd = pSrcRow + cxWidth;
			WORD *pDestPos = pDestRow;

			while (pSrcPos < pSrcPosEnd)
				if (*pSrcPos == m_wBackColor)
					{
					pDestPos++;
					pSrcPos++;
					}
				else
					{
					//	Blend the pixel using the appropriate tables

					WORD rgbRed = Source.m_pRedAlphaTable[((*pSrcPos & 0xf800) >> 6) | ((*pDestPos & 0xf800) >> 11)];
					WORD rgbGreen = Source.m_pGreenAlphaTable[((*pSrcPos & 0x7e0) << 1) | ((*pDestPos & 0x7e0) >> 5)];
					WORD rgbBlue = Source.m_pBlueAlphaTable[(*pSrcPos & 0x1f) << 5 | (*pDestPos & 0x1f)];

					*pDestPos++ = rgbRed | rgbGreen | rgbBlue;
					pSrcPos++;
					}

			pSrcRow = Source.NextRow(pSrcRow);
			pDestRow = NextRow(pDestRow);
			}
#endif
		}

	//	Otherwise just blt

	else
		{
		WORD wSrcBackColor = Source.GetBackColor();

		if (iAlpha == 255)
			{
			while (pSrcRow < pSrcRowEnd)
				{
				WORD *pSrcPos = pSrcRow;
				WORD *pSrcPosEnd = pSrcRow + cxWidth;
				WORD *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					if (*pSrcPos == wSrcBackColor)
						{
						pDestPos++;
						pSrcPos++;
						}
					else
						{
						DWORD dwRed = (*pSrcPos & 0xf800) >> 10;
						DWORD dwGreen = (*pSrcPos & 0x07e0) >> 5;
						DWORD dwBlue = (*pSrcPos & 0x001f) << 1;
						pSrcPos++;

						DWORD dwGray = (dwRed + dwGreen + dwBlue) / 3;

						*pDestPos++ = (WORD)(((dwGray << 10) & 0xf800) | (dwGray << 5) | (dwGray >> 1));
						}

				pSrcRow = Source.NextRow(pSrcRow);
				pDestRow = NextRow(pDestRow);
				}
			}
		else if (iAlpha > 0)
			{
			DWORD dwInvTrans = (((DWORD)(iAlpha)) ^ 0xff);

			while (pSrcRow < pSrcRowEnd)
				{
				WORD *pSrcPos = pSrcRow;
				WORD *pSrcPosEnd = pSrcRow + cxWidth;
				WORD *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					if (*pSrcPos == wSrcBackColor)
						{
						pDestPos++;
						pSrcPos++;
						}
					else
						{
						DWORD dwRed = (*pSrcPos & 0xf800) >> 10;
						DWORD dwGreen = (*pSrcPos & 0x07e0) >> 5;
						DWORD dwBlue = (*pSrcPos & 0x001f) << 1;
						pSrcPos++;

						DWORD dwGray = (iAlpha * (dwRed + dwGreen + dwBlue) / 3) >> 8;
						DWORD pxSource = (WORD)(((dwGray << 10) & 0xf800) | (dwGray << 5) | (dwGray >> 1));

						DWORD dwRedGreenS = ((pxSource << 8) & 0x00f80000) | (pxSource & 0x000007e0);

						DWORD pxDest = *pDestPos;
						DWORD dwRedGreen = (((((pxDest << 8) & 0x00f80000) | (pxDest & 0x000007e0)) * dwInvTrans) >> 8) + dwRedGreenS;
						dwBlue = (((pxDest & 0x1f) * dwInvTrans) >> 8) + (pxSource & 0x1f);

						*pDestPos++ = (WORD)(((dwRedGreen & 0x00f80000) >> 8) | (dwRedGreen & 0x000007e0) | dwBlue);
						}

				pSrcRow = Source.NextRow(pSrcRow);
				pDestRow = NextRow(pDestRow);
				}
			}
		}
	}

void CG16bitImage::BltLighten (int xSrc, int ySrc, int cxWidth, int cyHeight, int iAlpha, const CG16bitImage &Source, int xDest, int yDest)

//	BltLighten
//
//	Blt the image to the destination

	{
	//	Make sure we're in bounds

	if (!AdjustCoords(&xSrc, &ySrc, Source.m_cxWidth, Source.m_cyHeight, 
			&xDest, &yDest,
			&cxWidth, &cyHeight))
		return;

	WORD *pSrcRow = Source.GetPixel(Source.GetRowStart(ySrc), xSrc);
	WORD *pSrcRowEnd = Source.GetPixel(Source.GetRowStart(ySrc + cyHeight), xSrc);
	WORD *pDestRow = GetPixel(GetRowStart(yDest), xDest);

	//	If we've got an alpha mask then blt using the transparency
	//	information.

	if (Source.m_pAlpha)
		{
		BYTE *pAlphaSrcRow = Source.GetAlphaValue(xSrc, ySrc);

		while (pSrcRow < pSrcRowEnd)
			{
			BYTE *pAlphaPos = pAlphaSrcRow;
			WORD *pSrcPos = pSrcRow;
			WORD *pSrcPosEnd = pSrcRow + cxWidth;
			WORD *pDestPos = pDestRow;

			while (pSrcPos < pSrcPosEnd)
				if (*pAlphaPos == 0)
					{
					pDestPos++;
					pSrcPos++;
					pAlphaPos++;
					}
				else
					{
					WORD wColor;

					if (*pAlphaPos == 255)
						wColor = *pSrcPos;
					else
						wColor = BlendPixelPM(*pDestPos, *pSrcPos, *pAlphaPos);

					int iTotalDest = GetRedValue(*pDestPos) + GetGreenValue(*pDestPos) + GetBlueValue(*pDestPos);
					int iTotalSrc = GetRedValue(wColor) + GetGreenValue(wColor) + GetBlueValue(wColor);

					if (iTotalSrc > iTotalDest)
						*pDestPos = wColor;

					pSrcPos++;
					pDestPos++;
					pAlphaPos++;
					}

			pSrcRow = Source.NextRow(pSrcRow);
			pDestRow = NextRow(pDestRow);
			pAlphaSrcRow = Source.NextAlphaRow(pAlphaSrcRow);
			}
		}

	//	If we've got constant transparency then use the alpha tables

	else if (Source.IsTransparent())
		{
		WORD wSrcBackColor = Source.GetBackColor();

		while (pSrcRow < pSrcRowEnd)
			{
			WORD *pSrcPos = pSrcRow;
			WORD *pSrcPosEnd = pSrcRow + cxWidth;
			WORD *pDestPos = pDestRow;

			while (pSrcPos < pSrcPosEnd)
				if (*pSrcPos == wSrcBackColor)
					{
					pDestPos++;
					pSrcPos++;
					}
				else
					{
					//	Blend the pixel using the appropriate tables

					WORD rgbRed = Source.m_pRedAlphaTable[((*pSrcPos & 0xf800) >> 6) | ((*pDestPos & 0xf800) >> 11)];
					WORD rgbGreen = Source.m_pGreenAlphaTable[((*pSrcPos & 0x7e0) << 1) | ((*pDestPos & 0x7e0) >> 5)];
					WORD rgbBlue = Source.m_pBlueAlphaTable[(*pSrcPos & 0x1f) << 5 | (*pDestPos & 0x1f)];

					int iTotalDest = GetRedValue(*pDestPos) + GetGreenValue(*pDestPos) + GetBlueValue(*pDestPos);
					int iTotalSrc = rgbRed + rgbGreen + rgbBlue;

					if (iTotalSrc > iTotalDest)
						*pDestPos = rgbRed | rgbGreen | rgbBlue;

					pDestPos++;
					pSrcPos++;
					}

			pSrcRow = Source.NextRow(pSrcRow);
			pDestRow = NextRow(pDestRow);
			}
		}

	//	Otherwise just blt

	else
		{
		WORD wSrcBackColor = Source.GetBackColor();

		while (pSrcRow < pSrcRowEnd)
			{
			WORD *pSrcPos = pSrcRow;
			WORD *pSrcPosEnd = pSrcRow + cxWidth;
			WORD *pDestPos = pDestRow;

			while (pSrcPos < pSrcPosEnd)
				if (*pSrcPos == wSrcBackColor)
					{
					pDestPos++;
					pSrcPos++;
					}
				else
					{
					int iTotalDest = GetRedValue(*pDestPos) + GetGreenValue(*pDestPos) + GetBlueValue(*pDestPos);
					int iTotalSrc = GetRedValue(*pSrcPos) + GetGreenValue(*pSrcPos) + GetBlueValue(*pSrcPos);

					if (iTotalSrc > iTotalDest)
						*pDestPos = *pSrcPos;

					pDestPos++;
					pSrcPos++;
					}

			pSrcRow = Source.NextRow(pSrcRow);
			pDestRow = NextRow(pDestRow);
			}
		}
	}

void CG16bitImage::BltToDC (HDC hDC, int x, int y)

//	BltToDC
//
//	Blt the surface to a DC

	{
	//	Initialize a bitmap info structure

	if (m_pBMI == NULL)
		InitBMI(&m_pBMI);

	//	Blt

	::SetDIBitsToDevice(hDC,
			x,
			y,
			m_cxWidth,
			m_cyHeight,
			0,
			0,
			0,
			m_cyHeight,
			m_pRGB,
			m_pBMI,
			DIB_RGB_COLORS);
	}

void CG16bitImage::BltWithMask (int xSrc, int ySrc, int cxWidth, int cyHeight, const CG16bitImage &Mask, const CG16bitImage &Source, int xDest, int yDest)

//	BltWithMask
//
//	Blt the image to the destination (the mask matches the destination).

	{
	if (Mask.m_pAlpha == NULL)
		{
		Blt(xSrc, ySrc, cxWidth, cyHeight, Source, xDest, yDest);
		return;
		}

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

	WORD *pSrcRow = Source.GetPixel(Source.GetRowStart(ySrc), xSrc);
	WORD *pSrcRowEnd = Source.GetPixel(Source.GetRowStart(ySrc + cyHeight), xSrc);
	WORD *pDestRow = GetPixel(GetRowStart(yDest), xDest);
	BYTE *pAlphaRow = Mask.GetAlphaValue(xDest, yDest);

	while (pSrcRow < pSrcRowEnd)
		{
		WORD *pSrcPos = pSrcRow;
		WORD *pSrcPosEnd = pSrcRow + cxWidth;
		WORD *pDestPos = pDestRow;
		BYTE *pAlphaPos = pAlphaRow;

		while (pSrcPos < pSrcPosEnd)
			{
			if (*pAlphaPos == 255)
				{
				*pDestPos++ = *pSrcPos++;
				pAlphaPos++;
				}
			else if (*pAlphaPos == 0)
				{
				pDestPos++;
				pSrcPos++;
				pAlphaPos++;
				}
			else
				{
				DWORD dTemp = *pDestPos;
				DWORD sTemp = *pSrcPos;
				DWORD byOpacity = *pAlphaPos;

				BYTE *pAlpha5 = g_Alpha5[byOpacity];
				BYTE *pAlpha6 = g_Alpha6[byOpacity];
				BYTE *pAlpha5Inv = g_Alpha5[255 - byOpacity];
				BYTE *pAlpha6Inv = g_Alpha6[255 - byOpacity];

				DWORD dwRedResult = pAlpha5Inv[(dTemp & 0xf800) >> 11] + pAlpha5[(sTemp & 0xf800) >> 11];
				DWORD dwGreenResult = pAlpha6Inv[(dTemp & 0x7e0) >> 5] + pAlpha6[(sTemp & 0x7e0) >> 5];
				DWORD dwBlueResult = pAlpha5Inv[(dTemp & 0x1f)] + pAlpha5[(sTemp & 0x1f)];

				*pDestPos = (WORD)((dwRedResult << 11) | (dwGreenResult << 5) | (dwBlueResult));

				pDestPos++;
				pSrcPos++;
				pAlphaPos++;
				}
			}

		pSrcRow = Source.NextRow(pSrcRow);
		pDestRow = NextRow(pDestRow);
		pAlphaRow = Mask.NextAlphaRow(pAlphaRow);
		}
	}

void CG16bitImage::ColorTransBlt (int xSrc, int ySrc, int cxWidth, int cyHeight, DWORD dwOpacity, const CG16bitImage &Source, int xDest, int yDest)

//	Blt
//
//	Blt the image to the destination

	{
	//	Deal with sprite sources

	if (Source.m_pSprite)
		{
		Source.m_pSprite->ColorTransBlt(*this,
				xDest,
				yDest,
				xSrc,
				ySrc,
				cxWidth,
				cyHeight);
		return;
		}

	//	Make sure we're in bounds

	if (!AdjustCoords(&xSrc, &ySrc, Source.m_cxWidth, Source.m_cyHeight, 
			&xDest, &yDest,
			&cxWidth, &cyHeight))
		return;

	WORD *pSrcRow = Source.GetPixel(Source.GetRowStart(ySrc), xSrc);
	WORD *pSrcRowEnd = Source.GetPixel(Source.GetRowStart(ySrc + cyHeight), xSrc);
	WORD *pDestRow = GetPixel(GetRowStart(yDest), xDest);

	//	If we've got an alpha mask then blt using the transparency
	//	information.

	if (Source.m_pAlpha)
		{
		BYTE *pAlphaSrcRow = Source.GetAlphaValue(xSrc, ySrc);

		if (dwOpacity == 255)
			{
			while (pSrcRow < pSrcRowEnd)
				{
				BYTE *pAlphaPos = pAlphaSrcRow;
				WORD *pSrcPos = pSrcRow;
				WORD *pSrcPosEnd = pSrcRow + cxWidth;
				WORD *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					if (*pAlphaPos == 0)
						{
						pDestPos++;
						pSrcPos++;
						pAlphaPos++;
						}
					else if (*pAlphaPos == 255 || *pDestPos == 0)
						{
						*pDestPos++ = *pSrcPos++;
						pAlphaPos++;
						}
					else
						{
						DWORD pxSource = *pSrcPos;
						DWORD pxDest = *pDestPos;

						//	x ^ 0xff is the same as 255 - x
						//	| 0x07 so that we round-up
						//	+ 1 because below we divide by 256 instead of 255.
						//	LATER: Use a table lookup

						DWORD dwInvTrans = ((((DWORD)(*pAlphaPos)) ^ 0xff) | 0x07) + 1;

#ifdef BLT_RANGE_CHECK
						WORD wRedSrc = (*pSrcPos >> 11) & 0x1f;
						WORD wGreenSrc = (*pSrcPos >> 5) & 0x3f;
						WORD wBlueSrc = (*pSrcPos) & 0x1f;

						WORD wRedDest = ((WORD)dwInvTrans * ((*pDestPos >> 11) & 0x1f)) >> 8;
						WORD wGreenDest = ((WORD)dwInvTrans * ((*pDestPos >> 5) & 0x3f)) >> 8;
						WORD wBlueDest = ((WORD)dwInvTrans * ((*pDestPos) & 0x1f)) >> 8;

						WORD wRedResult = Min((WORD)0x1f, (WORD)(wRedSrc + wRedDest));
						WORD wGreenResult = Min((WORD)0x3f, (WORD)(wGreenSrc + wGreenDest));
						WORD wBlueResult = Min((WORD)0x1f, (WORD)(wBlueSrc + wBlueDest));

						*pDestPos++ = (wRedResult << 11) | (wGreenResult << 5) | wBlueResult;
#else
						DWORD dwRedGreenS = ((pxSource << 8) & 0x00f80000) | (pxSource & 0x000007e0);
						DWORD dwRedGreen = (((((pxDest << 8) & 0x00f80000) | (pxDest & 0x000007e0)) * dwInvTrans) >> 8) + dwRedGreenS;
						DWORD dwBlue = (((pxDest & 0x1f) * dwInvTrans) >> 8) + (pxSource & 0x1f);

						*pDestPos++ = (WORD)(((dwRedGreen & 0x00f80000) >> 8) | (dwRedGreen & 0x000007e0) | dwBlue);
#endif

						pSrcPos++;
						pAlphaPos++;
						}

				pSrcRow = Source.NextRow(pSrcRow);
				pDestRow = NextRow(pDestRow);
				pAlphaSrcRow = Source.NextAlphaRow(pAlphaSrcRow);
				}
			}
		else if (dwOpacity > 0)
			{
			while (pSrcRow < pSrcRowEnd)
				{
				BYTE *pAlphaPos = pAlphaSrcRow;
				WORD *pSrcPos = pSrcRow;
				WORD *pSrcPosEnd = pSrcRow + cxWidth;
				WORD *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					if (*pAlphaPos == 0)
						{
						pDestPos++;
						pSrcPos++;
						pAlphaPos++;
						}
					else if (*pAlphaPos == 255 || *pDestPos == 0)
						{
						DWORD dTemp = *pDestPos;
						DWORD sTemp = *pSrcPos;

						BYTE *pAlpha5 = g_Alpha5[dwOpacity];
						BYTE *pAlpha6 = g_Alpha6[dwOpacity];
						BYTE *pAlpha5Inv = g_Alpha5[255 - dwOpacity];
						BYTE *pAlpha6Inv = g_Alpha6[255 - dwOpacity];

						DWORD dwRedResult = pAlpha5Inv[(dTemp & 0xf800) >> 11] + pAlpha5[(sTemp & 0xf800) >> 11];
						DWORD dwGreenResult = pAlpha6Inv[(dTemp & 0x7e0) >> 5] + pAlpha6[(sTemp & 0x7e0) >> 5];
						DWORD dwBlueResult = pAlpha5Inv[(dTemp & 0x1f)] + pAlpha5[(sTemp & 0x1f)];

						*pDestPos++ = (WORD)((dwRedResult << 11) | (dwGreenResult << 5) | (dwBlueResult));

						pSrcPos++;
						pAlphaPos++;
						}
					else
						{
						DWORD dwPixelAlpha = dwOpacity * (*pAlphaPos) / 255;
						DWORD dTemp = *pDestPos;
						DWORD sTemp = *pSrcPos;

						BYTE *pAlpha5 = g_Alpha5[dwPixelAlpha];
						BYTE *pAlpha6 = g_Alpha6[dwPixelAlpha];
						BYTE *pAlpha5Inv = g_Alpha5[255 - dwPixelAlpha];
						BYTE *pAlpha6Inv = g_Alpha6[255 - dwPixelAlpha];

						DWORD dwRedResult = pAlpha5Inv[(dTemp & 0xf800) >> 11] + pAlpha5[(sTemp & 0xf800) >> 11];
						DWORD dwGreenResult = pAlpha6Inv[(dTemp & 0x7e0) >> 5] + pAlpha6[(sTemp & 0x7e0) >> 5];
						DWORD dwBlueResult = pAlpha5Inv[(dTemp & 0x1f)] + pAlpha5[(sTemp & 0x1f)];

						*pDestPos++ = (WORD)((dwRedResult << 11) | (dwGreenResult << 5) | (dwBlueResult));

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
		WORD wBackColor = Source.m_wBackColor;

		while (pSrcRow < pSrcRowEnd)
			{
			WORD *pSrcPos = pSrcRow;
			WORD *pSrcPosEnd = pSrcRow + cxWidth;
			WORD *pDestPos = pDestRow;

			while (pSrcPos < pSrcPosEnd)
				if (*pSrcPos == wBackColor)
					{
					pDestPos++;
					pSrcPos++;
					}
				else
					{
					//	Blend the pixel using the appropriate tables

					WORD rgbRed = Source.m_pRedAlphaTable[((*pSrcPos & 0xf800) >> 6) | ((*pDestPos & 0xf800) >> 11)];
					WORD rgbGreen = Source.m_pGreenAlphaTable[((*pSrcPos & 0x7e0) << 1) | ((*pDestPos & 0x7e0) >> 5)];
					WORD rgbBlue = Source.m_pBlueAlphaTable[(*pSrcPos & 0x1f) << 5 | (*pDestPos & 0x1f)];

					*pDestPos++ = rgbRed | rgbGreen | rgbBlue;
					pSrcPos++;
					}

			pSrcRow = Source.NextRow(pSrcRow);
			pDestRow = NextRow(pDestRow);
			}
		}

	//	Otherwise just blt

	else
		{
		WORD wBackColor = Source.m_wBackColor;

		if (dwOpacity == 255)
			{
			while (pSrcRow < pSrcRowEnd)
				{
				WORD *pSrcPos = pSrcRow;
				WORD *pSrcPosEnd = pSrcRow + cxWidth;
				WORD *pDestPos = pDestRow;

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
				WORD *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					if (*pSrcPos == wBackColor)
						{
						pDestPos++;
						pSrcPos++;
						}
					else
						*pDestPos++ = BlendPixel(*pDestPos, *pSrcPos++, dwOpacity);

				pSrcRow = Source.NextRow(pSrcRow);
				pDestRow = NextRow(pDestRow);
				}
			}
		}
	}

void CG16bitImage::CompositeTransBlt (int xSrc, int ySrc, int cxWidth, int cyHeight, DWORD dwOpacity, const CG16bitImage &Source, int xDest, int yDest)

//	CompositeTransBlt
//
//	Blt the image to the destination

	{
	//	Deal with sprite sources
	//	LATER: Deal with compositing sprites

	if (Source.m_pSprite)
		{
		Source.m_pSprite->ColorTransBlt(*this,
				xDest,
				yDest,
				xSrc,
				ySrc,
				cxWidth,
				cyHeight);
		return;
		}

	//	Make sure we're in bounds

	if (!AdjustCoords(&xSrc, &ySrc, Source.m_cxWidth, Source.m_cyHeight, 
			&xDest, &yDest,
			&cxWidth, &cyHeight))
		return;

	WORD *pSrcRow = Source.GetPixel(Source.GetRowStart(ySrc), xSrc);
	WORD *pSrcRowEnd = Source.GetPixel(Source.GetRowStart(ySrc + cyHeight), xSrc);
	WORD *pDestRow = GetPixel(GetRowStart(yDest), xDest);
	BYTE *pDestRowAlpha = GetAlphaRow(yDest) + xDest;

	//	If we've got an alpha mask then blt using the transparency
	//	information.

	if (Source.m_pAlpha)
		{
		BYTE *pAlphaSrcRow = Source.GetAlphaValue(xSrc, ySrc);

		if (dwOpacity == 255)
			{
			while (pSrcRow < pSrcRowEnd)
				{
				BYTE *pAlphaPos = pAlphaSrcRow;
				WORD *pSrcPos = pSrcRow;
				WORD *pSrcPosEnd = pSrcRow + cxWidth;
				WORD *pDestPos = pDestRow;
				BYTE *pDestPosAlpha = pDestRowAlpha;

				while (pSrcPos < pSrcPosEnd)
					if (*pAlphaPos == 0)
						{
						pDestPos++;
						pDestPosAlpha++;
						pSrcPos++;
						pAlphaPos++;
						}
					else if (*pAlphaPos == 255 || *pDestPos == 0)
						{
						*pDestPos++ = *pSrcPos++;
						*pDestPosAlpha++ = 255;
						pAlphaPos++;
						}
					else
						{
						DWORD pxSource = *pSrcPos;
						DWORD pxDest = *pDestPos;

						//	x ^ 0xff is the same as 255 - x
						//	| 0x0f so that we round-up
						//	+ 1 because below we divide by 256 instead of 255.
						//	LATER: Use a table lookup

						DWORD dwInvTrans = ((((DWORD)(*pAlphaPos)) ^ 0xff) | 0x0f) + 1;

#ifdef BLT_RANGE_CHECK
						WORD wRedSrc = (*pSrcPos >> 11) & 0x1f;
						WORD wGreenSrc = (*pSrcPos >> 5) & 0x3f;
						WORD wBlueSrc = (*pSrcPos) & 0x1f;

						WORD wRedDest = ((WORD)dwInvTrans * ((*pDestPos >> 11) & 0x1f)) >> 8;
						WORD wGreenDest = ((WORD)dwInvTrans * ((*pDestPos >> 5) & 0x3f)) >> 8;
						WORD wBlueDest = ((WORD)dwInvTrans * ((*pDestPos) & 0x1f)) >> 8;

						WORD wRedResult = Min((WORD)0x1f, (WORD)(wRedSrc + wRedDest));
						WORD wGreenResult = Min((WORD)0x3f, (WORD)(wGreenSrc + wGreenDest));
						WORD wBlueResult = Min((WORD)0x1f, (WORD)(wBlueSrc + wBlueDest));

						*pDestPos++ = (wRedResult << 11) | (wGreenResult << 5) | wBlueResult;
						*pDestPosAlpha++ = CG16bitImage::BlendAlpha(*pAlphaPos, *pDestPosAlpha);
#else
						DWORD dwRedGreenS = ((pxSource << 8) & 0x00f80000) | (pxSource & 0x000007e0);
						DWORD dwRedGreen = (((((pxDest << 8) & 0x00f80000) | (pxDest & 0x000007e0)) * dwInvTrans) >> 8) + dwRedGreenS;
						DWORD dwBlue = (((pxDest & 0x1f) * dwInvTrans) >> 8) + (pxSource & 0x1f);

						*pDestPos++ = (WORD)(((dwRedGreen & 0x00f80000) >> 8) | (dwRedGreen & 0x000007e0) | dwBlue);
						*pDestPosAlpha++ = CG16bitImage::BlendAlpha(*pAlphaPos, *pDestPosAlpha);
#endif

						pSrcPos++;
						pAlphaPos++;
						}

				pSrcRow = Source.NextRow(pSrcRow);
				pDestRow = NextRow(pDestRow);
				pDestRowAlpha = NextAlphaRow(pDestRowAlpha);
				pAlphaSrcRow = Source.NextAlphaRow(pAlphaSrcRow);
				}
			}
		else if (dwOpacity > 0)
			{
			while (pSrcRow < pSrcRowEnd)
				{
				BYTE *pAlphaPos = pAlphaSrcRow;
				WORD *pSrcPos = pSrcRow;
				WORD *pSrcPosEnd = pSrcRow + cxWidth;
				WORD *pDestPos = pDestRow;
				BYTE *pDestPosAlpha = pDestRowAlpha;

				while (pSrcPos < pSrcPosEnd)
					if (*pAlphaPos == 0)
						{
						pDestPos++;
						pDestPosAlpha++;
						pSrcPos++;
						pAlphaPos++;
						}
					else if (*pAlphaPos == 255 || *pDestPos == 0)
						{
						DWORD dTemp = *pDestPos;
						DWORD sTemp = *pSrcPos;

						BYTE *pAlpha5 = g_Alpha5[dwOpacity];
						BYTE *pAlpha6 = g_Alpha6[dwOpacity];
						BYTE *pAlpha5Inv = g_Alpha5[255 - dwOpacity];
						BYTE *pAlpha6Inv = g_Alpha6[255 - dwOpacity];

						DWORD dwRedResult = pAlpha5Inv[(dTemp & 0xf800) >> 11] + pAlpha5[(sTemp & 0xf800) >> 11];
						DWORD dwGreenResult = pAlpha6Inv[(dTemp & 0x7e0) >> 5] + pAlpha6[(sTemp & 0x7e0) >> 5];
						DWORD dwBlueResult = pAlpha5Inv[(dTemp & 0x1f)] + pAlpha5[(sTemp & 0x1f)];

						*pDestPos++ = (WORD)((dwRedResult << 11) | (dwGreenResult << 5) | (dwBlueResult));
						*pDestPosAlpha++ = CG16bitImage::BlendAlpha(*pDestPosAlpha, (BYTE)dwOpacity);

						pSrcPos++;
						pAlphaPos++;
						}
					else
						{
						DWORD dwPixelAlpha = dwOpacity * (*pAlphaPos) / 255;
						DWORD dTemp = *pDestPos;
						DWORD sTemp = *pSrcPos;

						BYTE *pAlpha5 = g_Alpha5[dwPixelAlpha];
						BYTE *pAlpha6 = g_Alpha6[dwPixelAlpha];
						BYTE *pAlpha5Inv = g_Alpha5[255 - dwPixelAlpha];
						BYTE *pAlpha6Inv = g_Alpha6[255 - dwPixelAlpha];

						DWORD dwRedResult = pAlpha5Inv[(dTemp & 0xf800) >> 11] + pAlpha5[(sTemp & 0xf800) >> 11];
						DWORD dwGreenResult = pAlpha6Inv[(dTemp & 0x7e0) >> 5] + pAlpha6[(sTemp & 0x7e0) >> 5];
						DWORD dwBlueResult = pAlpha5Inv[(dTemp & 0x1f)] + pAlpha5[(sTemp & 0x1f)];

						*pDestPos++ = (WORD)((dwRedResult << 11) | (dwGreenResult << 5) | (dwBlueResult));
						*pDestPosAlpha++ = CG16bitImage::BlendAlpha(*pDestPosAlpha, (BYTE)dwPixelAlpha);

						pSrcPos++;
						pAlphaPos++;
						}

				pSrcRow = Source.NextRow(pSrcRow);
				pDestRow = NextRow(pDestRow);
				pDestRowAlpha = NextAlphaRow(pDestRowAlpha);
				pAlphaSrcRow = Source.NextAlphaRow(pAlphaSrcRow);
				}
			}
		}

	//	If we've got constant transparency then use the alpha tables

	else if (Source.IsTransparent())
		{
		WORD wBackColor = Source.m_wBackColor;

		while (pSrcRow < pSrcRowEnd)
			{
			WORD *pSrcPos = pSrcRow;
			WORD *pSrcPosEnd = pSrcRow + cxWidth;
			WORD *pDestPos = pDestRow;
			BYTE *pDestPosAlpha = pDestRowAlpha;

			while (pSrcPos < pSrcPosEnd)
				if (*pSrcPos == wBackColor)
					{
					pDestPos++;
					pDestPosAlpha++;
					pSrcPos++;
					}
				else
					{
					//	Blend the pixel using the appropriate tables

					WORD rgbRed = Source.m_pRedAlphaTable[((*pSrcPos & 0xf800) >> 6) | ((*pDestPos & 0xf800) >> 11)];
					WORD rgbGreen = Source.m_pGreenAlphaTable[((*pSrcPos & 0x7e0) << 1) | ((*pDestPos & 0x7e0) >> 5)];
					WORD rgbBlue = Source.m_pBlueAlphaTable[(*pSrcPos & 0x1f) << 5 | (*pDestPos & 0x1f)];

					*pDestPos++ = rgbRed | rgbGreen | rgbBlue;
					*pDestPosAlpha++ = 255;
					pSrcPos++;
					}

			pSrcRow = Source.NextRow(pSrcRow);
			pDestRow = NextRow(pDestRow);
			pDestRowAlpha = NextAlphaRow(pDestRowAlpha);
			}
		}

	//	Otherwise just blt

	else
		{
		WORD wBackColor = Source.m_wBackColor;

		if (dwOpacity == 255)
			{
			while (pSrcRow < pSrcRowEnd)
				{
				WORD *pSrcPos = pSrcRow;
				WORD *pSrcPosEnd = pSrcRow + cxWidth;
				WORD *pDestPos = pDestRow;
				BYTE *pDestPosAlpha = pDestRowAlpha;

				while (pSrcPos < pSrcPosEnd)
					if (*pSrcPos == wBackColor)
						{
						pDestPos++;
						pDestPosAlpha++;
						pSrcPos++;
						}
					else
						{
						*pDestPos++ = *pSrcPos++;
						*pDestPosAlpha++ = 255;
						}

				pSrcRow = Source.NextRow(pSrcRow);
				pDestRow = NextRow(pDestRow);
				pDestRowAlpha = NextAlphaRow(pDestRowAlpha);
				}
			}
		else
			{
			while (pSrcRow < pSrcRowEnd)
				{
				WORD *pSrcPos = pSrcRow;
				WORD *pSrcPosEnd = pSrcRow + cxWidth;
				WORD *pDestPos = pDestRow;
				BYTE *pDestPosAlpha = pDestRowAlpha;

				while (pSrcPos < pSrcPosEnd)
					if (*pSrcPos == wBackColor)
						{
						pDestPos++;
						pDestPosAlpha++;
						pSrcPos++;
						}
					else
						{
						*pDestPos++ = BlendPixel(*pDestPos, *pSrcPos++, dwOpacity);
						*pDestPosAlpha++ = BlendAlpha(*pDestPosAlpha, (BYTE)dwOpacity);
						}

				pSrcRow = Source.NextRow(pSrcRow);
				pDestRow = NextRow(pDestRow);
				pDestRowAlpha = NextAlphaRow(pDestRowAlpha);
				}
			}
		}
	}

void CG16bitImage::ClearMaskBlt (int xSrc, int ySrc, int cxWidth, int cyHeight, const CG16bitImage &Source, int xDest, int yDest, WORD wColor)

//	ClearMaskBlt
//
//	Blts all the transparent parts of Source with the given color.

	{
	//	Deal with sprite sources

	if (Source.m_pSprite)
		{
		//	LATER:
		ASSERT(false);
		return;
		}

	//	Make sure we're in bounds

	if (!AdjustCoords(&xSrc, &ySrc, Source.m_cxWidth, Source.m_cyHeight, 
			&xDest, &yDest,
			&cxWidth, &cyHeight))
		return;

	WORD *pSrcRow = Source.GetPixel(Source.GetRowStart(ySrc), xSrc);
	WORD *pSrcRowEnd = Source.GetPixel(Source.GetRowStart(ySrc + cyHeight), xSrc);
	WORD *pDestRow = GetPixel(GetRowStart(yDest), xDest);

	//	If we've got an alpha mask then blt using the transparency
	//	information.

	if (Source.m_pAlpha)
		{
		BYTE *pAlphaSrcRow = Source.GetAlphaValue(xSrc, ySrc);

		while (pSrcRow < pSrcRowEnd)
			{
			BYTE *pAlphaPos = pAlphaSrcRow;
			WORD *pSrcPos = pSrcRow;
			WORD *pSrcPosEnd = pSrcRow + cxWidth;
			WORD *pDestPos = pDestRow;

			while (pSrcPos < pSrcPosEnd)
				if (*pAlphaPos == 0)
					{
					*pDestPos++ = wColor;
					pSrcPos++;
					pAlphaPos++;
					}
				else if (*pAlphaPos == 255)
					{
					pDestPos++;
					pSrcPos++;
					pAlphaPos++;
					}
				else
					{
					DWORD pxSource = wColor;
					DWORD pxDest = *pDestPos;
					DWORD dwTrans = 255 - *pAlphaPos;
					DWORD dwInvTrans = (((DWORD)(dwTrans)) ^ 0xff);

					DWORD dwRedGreenS = ((pxSource << 8) & 0x00f80000) | (pxSource & 0x000007e0);
					DWORD dwRedGreen = (((((pxDest << 8) & 0x00f80000) | (pxDest & 0x000007e0)) * dwInvTrans) >> 8) + dwRedGreenS;
					DWORD dwBlue = (((pxDest & 0x1f) * dwInvTrans) >> 8) + (pxSource & 0x1f);

					*pDestPos++ = (WORD)(((dwRedGreen & 0x00f80000) >> 8) | (dwRedGreen & 0x000007e0) | dwBlue);

					pSrcPos++;
					pAlphaPos++;
					}

			pSrcRow = Source.NextRow(pSrcRow);
			pDestRow = NextRow(pDestRow);
			pAlphaSrcRow = Source.NextAlphaRow(pAlphaSrcRow);
			}
		}

	//	If we've got constant transparency then use the alpha tables

	else
		{
		WORD wBackColor = Source.m_wBackColor;

		while (pSrcRow < pSrcRowEnd)
			{
			WORD *pSrcPos = pSrcRow;
			WORD *pSrcPosEnd = pSrcRow + cxWidth;
			WORD *pDestPos = pDestRow;

			while (pSrcPos < pSrcPosEnd)
				if (*pSrcPos == wBackColor)
					{
					*pDestPos++ = wColor;
					pSrcPos++;
					}
				else
					{
					pDestPos++;
					pSrcPos++;
					}

			pSrcRow = Source.NextRow(pSrcRow);
			pDestRow = NextRow(pDestRow);
			}
		}
	}

void CG16bitImage::ConvertToSprite (void)

//	ConvertToSprite
//
//	Converts to a sprite

	{
	ALERROR error;
	CG16bitSprite *pSprite = new CG16bitSprite;
	if (error = pSprite->CreateFromImage(*this))
		{
		delete pSprite;
		return;
		}

	//	Done

	DeleteData();
	m_pSprite = pSprite;
	m_cxWidth = m_pSprite->GetWidth();
	m_cyHeight = m_pSprite->GetHeight();
	}

void CG16bitImage::CopyAlpha (int xSrc, int ySrc, int cxWidth, int cyHeight, const CG16bitImage &Source, int xDest, int yDest)

//	CopyAlpha
//
//	Copies the alpha mask from the source to this image

	{
	//	Make sure we're in bounds

	if (!AdjustCoords(&xSrc, &ySrc, Source.m_cxWidth, Source.m_cyHeight, 
			&xDest, &yDest,
			&cxWidth, &cyHeight))
		return;

	WORD *pSrcRow = Source.GetPixel(Source.GetRowStart(ySrc), xSrc);
	WORD *pSrcRowEnd = Source.GetPixel(Source.GetRowStart(ySrc + cyHeight), xSrc);
	WORD *pDestRow = GetPixel(GetRowStart(yDest), xDest);

	//	Copy the alpha mask if both images have it

	if (m_pAlpha && Source.m_pAlpha)
		{
		BYTE *pAlphaSrcRow = Source.GetAlphaValue(xSrc, ySrc);
		BYTE *pAlphaSrcRowEnd = Source.GetAlphaValue(xSrc, ySrc + cyHeight);
		BYTE *pDestRow = GetAlphaValue(xDest, yDest);

		while (pAlphaSrcRow < pAlphaSrcRowEnd)
			{
			BYTE *pAlphaPos = pAlphaSrcRow;
			BYTE *pAlphaPosEnd = pAlphaPos + cxWidth;
			BYTE *pDest = pDestRow;

			while (pAlphaPos < pAlphaPosEnd)
				*pDest++ = *pAlphaPos++;

			pAlphaSrcRow = Source.NextAlphaRow(pAlphaSrcRow);
			pDestRow = NextAlphaRow(pDestRow);
			}
		}

	//	Else, if only the destination has alpha, use the backcolor

	else if (m_pAlpha)
		{
		WORD wBackColor = Source.m_wBackColor;

		WORD *pSrcRow = Source.GetPixel(Source.GetRowStart(ySrc), xSrc);
		WORD *pSrcRowEnd = Source.GetPixel(Source.GetRowStart(ySrc + cyHeight), xSrc);
		BYTE *pDestRow = GetAlphaValue(xDest, yDest);

		while (pSrcRow < pSrcRowEnd)
			{
			WORD *pPos = pSrcRow;
			WORD *pPosEnd = pPos + cxWidth;
			BYTE *pDest = pDestRow;

			while (pPos < pPosEnd)
				{
				if (*pPos++ == wBackColor)
					*pDest++ = 0x00;
				else
					*pDest++ = 0xff;
				}

			pSrcRow = Source.NextRow(pSrcRow);
			pDestRow = NextAlphaRow(pDestRow);
			}
		}
	}

void CG16bitImage::CopyData (const CG16bitImage &Src)

//	CopyData
//
//	Copies Src to this image. We assume that our image is initialized.

	{
	m_cxWidth = Src.m_cxWidth;
	m_cyHeight = Src.m_cyHeight;
	m_iRGBRowSize = Src.m_iRGBRowSize;
	m_iAlphaRowSize = Src.m_iAlphaRowSize;

	m_wBackColor = Src.m_wBackColor;
	m_bHasMask = Src.m_bHasMask;
	m_rcClip = Src.m_rcClip;

	if (Src.m_pRGB)
		{
		int iRGBSize = m_cyHeight * m_iRGBRowSize * sizeof(DWORD);
		m_pRGB = (DWORD *)::MemAlloc(iRGBSize);
		::utlMemCopy((char *)Src.m_pRGB, (char *)m_pRGB, iRGBSize);
		}
	else
		m_pRGB = NULL;

	if (Src.m_pAlpha)
		{
		int iAlphaSize = m_cyHeight * m_iAlphaRowSize * sizeof(DWORD);
		m_pAlpha = (DWORD *)::MemAlloc(iAlphaSize);
		::utlMemCopy((char *)Src.m_pAlpha, (char *)m_pAlpha, iAlphaSize);
		}
	else
		m_pAlpha = NULL;

	if (Src.m_pRedAlphaTable)
		{
		m_pRedAlphaTable = (WORD *)MemAlloc(2 * 32 * 32);
		::utlMemCopy((char *)Src.m_pRedAlphaTable, (char *)m_pRedAlphaTable, 2 * 32 * 32);
		m_pGreenAlphaTable = (WORD *)MemAlloc(2 * 64 * 64);
		::utlMemCopy((char *)Src.m_pGreenAlphaTable, (char *)m_pGreenAlphaTable, 2 * 32 * 32);
		m_pBlueAlphaTable = (WORD *)MemAlloc(2 * 32 * 32);
		::utlMemCopy((char *)Src.m_pBlueAlphaTable, (char *)m_pBlueAlphaTable, 2 * 32 * 32);
		}
	else
		{
		m_pRedAlphaTable = NULL;
		m_pGreenAlphaTable = NULL;
		m_pBlueAlphaTable = NULL;
		}

	//	LATER: Deal with sprites

	ASSERT(Src.m_pSprite == NULL);
	m_pSprite = NULL;

	//	LATER: Deal with surfaces

	m_pBMI = NULL;
	}

ALERROR CG16bitImage::CopyToClipboard (void)

//	CopyToClipboard
//
//	Copies the bitmap to the clipboard

	{
	//	Create an HBITMAP

	HWND hDesktopWnd = ::GetDesktopWindow();
	HDC hDesktopDC = ::GetDC(hDesktopWnd);
	HDC hDC = ::CreateCompatibleDC(hDesktopDC);
	HBITMAP hBitmap = ::CreateCompatibleBitmap(hDesktopDC, m_cxWidth, m_cyHeight);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hDC, hBitmap);
	BltToDC(hDC, 0, 0);
	::SelectObject(hDC, hOldBitmap);
	::DeleteDC(hDC);
	::ReleaseDC(hDesktopWnd, hDesktopDC);

	//	Copy to the clipboard

	if (!::OpenClipboard(NULL))
		return ERR_FAIL;

	if (!::EmptyClipboard())
		{
		::CloseClipboard();
		return ERR_FAIL;
		}

	if (!::SetClipboardData(CF_BITMAP, hBitmap))
		{
		::CloseClipboard();
		return ERR_FAIL;
		}

	if (!::CloseClipboard())
		return ERR_FAIL;

	return NOERROR;
	}

ALERROR CG16bitImage::CreateBlank (int cxWidth, int cyHeight, bool bAlphaMask, WORD wInitColor, BYTE byInitAlpha)

//	CreateBlank
//
//	Create a blank bitmap

	{
	ALERROR error;
	int iRGBRowSizeBytes;
	int iAlphaRowSize = 0;
	DWORD *pRGB = NULL;
	DWORD *pAlpha = NULL;

	//	Allocate the main buffer

	iRGBRowSizeBytes = AlignUp(cxWidth * sizeof(WORD), sizeof(DWORD));
	pRGB = (DWORD *)MemAlloc(cyHeight * iRGBRowSizeBytes);
	if (pRGB == NULL)
		{
		error = ERR_MEMORY;
		goto Fail;
		}

	//	Allocate alpha mask

	if (bAlphaMask)
		{
		iAlphaRowSize = AlignUp(cxWidth, sizeof(DWORD)) / sizeof(DWORD);
		pAlpha = (DWORD *)MemAlloc(cyHeight * iAlphaRowSize * sizeof(DWORD));
		utlMemSet(pAlpha, cyHeight * iAlphaRowSize * sizeof(DWORD), (char)byInitAlpha);
		}

	//	Done

	DeleteData();
	m_cxWidth = cxWidth;
	m_cyHeight = cyHeight;
	ResetClipRect();
	m_iRGBRowSize = iRGBRowSizeBytes / sizeof(DWORD);
	m_iAlphaRowSize = iAlphaRowSize;
	m_pRGB = pRGB;
	m_pAlpha = pAlpha;
	m_bHasMask = bAlphaMask;

	if (wInitColor == 0)
		::ZeroMemory(m_pRGB, cyHeight * iRGBRowSizeBytes);
	else
		Fill(0, 0, cxWidth, cyHeight, wInitColor);

	return NOERROR;

Fail:

	if (pRGB)
		MemFree(pRGB);

	if (pAlpha)
		MemFree(pAlpha);

	return error;
	}

ALERROR CG16bitImage::CreateBlankAlpha (int cxWidth, int cyHeight)

//	CreateBlankAlpha
//
//	Creates a blank image with only a mask

	{
	//	Allocate alpha mask

	int iAlphaRowSize = AlignUp(cxWidth, sizeof(DWORD)) / sizeof(DWORD);
	DWORD *pAlpha = (DWORD *)MemAlloc(cyHeight * iAlphaRowSize * sizeof(DWORD));

	//	Done

	DeleteData();
	m_cxWidth = cxWidth;
	m_cyHeight = cyHeight;
	ResetClipRect();
	m_iAlphaRowSize = iAlphaRowSize;
	m_pAlpha = pAlpha;
	m_bHasMask = true;
	m_pRGB = NULL;

	return NOERROR;
	}

ALERROR CG16bitImage::CreateBlankAlpha (int cxWidth, int cyHeight, BYTE byOpacity)

//	CreateBlankAlpha
//
//	Creates a blank image with only a mask

	{
	ALERROR error;

	if (error = CreateBlankAlpha(cxWidth, cyHeight))
		return error;

	utlMemSet(m_pAlpha, m_cyHeight * m_iAlphaRowSize * sizeof(DWORD), (char)byOpacity);

	return NOERROR;
	}

ALERROR CG16bitImage::CreateFromBitmap (HBITMAP hBitmap, HBITMAP hBitmask, DWORD dwFlags)

//	CreateFromBitmap
//
//	Create from a bitmap and mask. If hBitmask is NULL then we have no
//	mask

	{
	ALERROR error;

	try
		{
		int iRGBRowSizeBytes = 0;
		int iAlphaRowSize = 0;
		DWORD *pRGB = NULL;
		DWORD *pAlpha = NULL;

		//	Figure out the width and height of the bitmap first

		int cxWidth = 0;
		int cyHeight = 0;
		void *pBase = NULL;
		int iStride = 0;

		if (hBitmap)
			{
			if (error = dibGetInfo(hBitmap, &cxWidth, &cyHeight, &pBase, &iStride))
				goto Fail;
			}

		//	Now load the alpha channel (if any). We do this first because we
		//	may need to pre-multiply the RGB values by the alpha later

		if (hBitmask)
			{
			void *pBase;
			int iStride;
			int cxAlphaWidth, cyAlphaHeight;

			if (error = dibGetInfo(hBitmask, &cxAlphaWidth, &cyAlphaHeight, &pBase, &iStride))
				goto Fail;

			//	If we only have a mask then initialize the width and height appropriately

			if (hBitmap == NULL)
				{
				cxWidth = cxAlphaWidth;
				cyHeight = cyAlphaHeight;
				}

			//	Allocate a buffer to hold the alpha mask

			iAlphaRowSize = AlignUp(cxWidth, sizeof(DWORD)) / sizeof(DWORD);
			pAlpha = (DWORD *)MemAlloc(cyHeight * iAlphaRowSize * sizeof(DWORD));
			if (pAlpha == NULL)
				{
				error = ERR_MEMORY;
				goto Fail;
				}

			//	Copy bits

			if (dibIs24bit(hBitmask))
				{
				int x, y;
				BYTE *pSource;
				BYTE *pDest;

				for (y = 0; y < cyHeight; y++)
					{
					pSource = (BYTE *)((char *)pBase + iStride * y);
					pDest = (BYTE *)(pAlpha + y * iAlphaRowSize);

					if (y < cyAlphaHeight)
						{
						for (x = 0; x < cxWidth; x++)
							{
							if (x >= cxAlphaWidth)
								*pDest = 0x00;
							else
								*pDest = *pSource;

							pDest++;
							pSource += 3;
							}
						}
					else
						{
						for (x = 0; x < cxWidth; x++)
							*pDest++ = 0;
						}
					}
				}
			else if (dibIs16bit(hBitmask))
				{
				int x, y;
				WORD *pSource;
				BYTE *pDest;

				if (dwFlags & cfbDesaturateAlpha)
					{
					for (y = 0; y < cyHeight; y++)
						{
						pSource = (WORD *)((char *)pBase + iStride * y);
						pDest = (BYTE *)(pAlpha + y * iAlphaRowSize);

						if (y < cyAlphaHeight)
							{
							for (x = 0; x < cxWidth; x++)
								{
								if (x >= cxAlphaWidth || *pSource == 0)
									*pDest = 0x00;
								else if (*pSource == 0xffff)
									*pDest = 0xff;
								else
									{
									DWORD dwSource = *pSource;
									DWORD dwBlue = (((dwSource & 0x001f) << 8) / 31);
									DWORD dwGreen = (((dwSource & 0x07e0) << 3) / 63);
									DWORD dwRed = (((dwSource & 0xf800) >> 3) / 31);

									DWORD dwMaxColor, dwMinColor;
									if (dwBlue > dwGreen)
										{
										if (dwRed > dwBlue)
											{
											dwMaxColor = dwRed;
											dwMinColor = dwGreen;
											}
										else
											{
											dwMaxColor = dwBlue;
											dwMinColor = (dwRed > dwGreen ? dwGreen : dwRed);
											}
										}
									else
										{
										if (dwRed > dwGreen)
											{
											dwMaxColor = dwRed;
											dwMinColor = dwBlue;
											}
										else
											{
											dwMaxColor = dwGreen;
											dwMinColor = (dwRed > dwBlue ? dwBlue : dwRed);
											}
										}

									*pDest = (BYTE)((dwMaxColor + dwMinColor) / 2);
									}

								pDest++;
								pSource++;
								}
							}
						else
							{
							for (x = 0; x < cxWidth; x++)
								*pDest++ = 0;
							}
						}
					}
				else
					{
					for (y = 0; y < cyHeight; y++)
						{
						pSource = (WORD *)((char *)pBase + iStride * y);
						pDest = (BYTE *)(pAlpha + y * iAlphaRowSize);

						if (y < cyAlphaHeight)
							{
							for (x = 0; x < cxWidth; x++)
								{
								if (x >= cxAlphaWidth || *pSource == 0)
									*pDest = 0x00;
								else if (*pSource == 0xffff)
									*pDest = 0xff;
								else
									//	Take the green channel and convert to 0-255
									*pDest = (BYTE)(((*pSource & 0x07e0) << 3) / 63);

								pDest++;
								pSource++;
								}
							}
						else
							{
							for (x = 0; x < cxWidth; x++)
								*pDest++ = 0;
							}
						}
					}
				}
			}

		//	Now we get the bits of the bitmap. This section will initialize
		//	cxWidth, cyHeight, iRGBRowSize, and pRGB.
		//
		//	If this is a DIBSECTION of the correct format then we access
		//	the bits directly.

		if (hBitmap)
			{
			BITMAP bm;

			//	Allocate our own buffer

			iRGBRowSizeBytes = AlignUp(Absolute(iStride), sizeof(DWORD));
			pRGB = (DWORD *)MemAlloc(cyHeight * iRGBRowSizeBytes);
			if (pRGB == NULL)
				{
				error = ERR_MEMORY;
				goto Fail;
				}

			if (dibIs16bit(hBitmap))
				{
				//	Copy bits

				if (!(dwFlags & cfbPreMultAlpha) && pAlpha)
					{
					int x, y;
					BYTE *pAlphaRow = (BYTE *)pAlpha;
					WORD *pSourceRow = (WORD *)pBase;
					WORD *pDestRow = (WORD *)pRGB;
					for (y = 0; y < cyHeight; y++)
						{
						BYTE *pAlphaPos = pAlphaRow;
						WORD *pSourcePos = pSourceRow;
						WORD *pDestPos = pDestRow;
						for (x = 0; x < cxWidth; x++)
							{
							WORD wBlue = (WORD)IntMult(GetBlueValue(*pSourcePos), *pAlphaPos);
							WORD wGreen = (WORD)IntMult(GetGreenValue(*pSourcePos), *pAlphaPos);
							WORD wRed = (WORD)IntMult(GetRedValue(*pSourcePos), *pAlphaPos);

							*pDestPos = wBlue | (wGreen << 5) | (wRed << 11);

							pAlphaPos++;
							pSourcePos++;
							pDestPos++;
							}

						pAlphaRow += iAlphaRowSize * sizeof(DWORD);
						pSourceRow += iStride / sizeof(WORD);
						pDestRow += iRGBRowSizeBytes / sizeof(WORD);
						}
					}
				else
					{
					int i;
					char *pSource = (char *)pBase;
					char *pDest = (char *)pRGB;
					for (i = 0; i < cyHeight; i++)
						{
						utlMemCopy(pSource, pDest, sizeof(WORD) * cxWidth);
						pSource += iStride;
						pDest += iRGBRowSizeBytes;
						}
					}
				}
			else if (dibIs24bit(hBitmap))
				{
				//	Copy bits

				if (!(dwFlags & cfbPreMultAlpha) && pAlpha)
					{
					int x, y;
					BYTE *pAlphaRow = (BYTE *)pAlpha;
					BYTE *pSourceRow = (BYTE *)pBase;
					WORD *pDestRow = (WORD *)pRGB;
					for (y = 0; y < cyHeight; y++)
						{
						BYTE *pAlphaPos = pAlphaRow;
						BYTE *pSourcePos = pSourceRow;
						WORD *pDestPos = pDestRow;
						for (x = 0; x < cxWidth; x++)
							{
							WORD wBlue = (WORD)IntMult((*pSourcePos++) >> 3, *pAlphaPos);
							WORD wGreen = (WORD)IntMult((*pSourcePos++) >> 2, *pAlphaPos);
							WORD wRed = (WORD)IntMult((*pSourcePos++) >> 3, *pAlphaPos);

							*pDestPos = wBlue | (wGreen << 5) | (wRed << 11);

							pAlphaPos++;
							pDestPos++;
							}

						pAlphaRow += iAlphaRowSize * sizeof(DWORD);
						pSourceRow += iStride;
						pDestRow += iRGBRowSizeBytes / sizeof(WORD);
						}
					}
				else
					{
					int x, y;
					BYTE *pSourceRow = (BYTE *)pBase;
					WORD *pDestRow = (WORD *)pRGB;
					for (y = 0; y < cyHeight; y++)
						{
						BYTE *pSourcePos = pSourceRow;
						WORD *pDestPos = pDestRow;
						for (x = 0; x < cxWidth; x++)
							{
							DWORD dwBlue = ((DWORD)*pSourcePos++) >> 3;
							DWORD dwGreen = ((DWORD)*pSourcePos++) >> 2;
							DWORD dwRed = ((DWORD)*pSourcePos++) >> 3;

							*pDestPos = (WORD)(dwBlue | (dwGreen << 5) | (dwRed << 11));

							pDestPos++;
							}

						pSourceRow += iStride;
						pDestRow += iRGBRowSizeBytes / sizeof(WORD);
						}
					}
				}

			//	Otherwise we get the bits using GetDIBits

			else if (GetObject(hBitmap, sizeof(bm), &bm) == sizeof(bm))
				{
	#ifndef LATER
				ASSERT(FALSE);
	#endif
				}
			else
				return ERR_FAIL;
			}

		//	Free the old data and assign the new

		DeleteData();
		m_cxWidth = cxWidth;
		m_cyHeight = cyHeight;
		ResetClipRect();
		m_iRGBRowSize = iRGBRowSizeBytes / sizeof(DWORD);
		m_iAlphaRowSize = iAlphaRowSize;
		m_pRGB = pRGB;
		m_pAlpha = pAlpha;
		m_bHasMask = (pAlpha ? true : false);

		return NOERROR;

	Fail:

		if (pRGB)
			MemFree(pRGB);

		if (pAlpha)
			MemFree(pAlpha);

		return error;
		}
	catch (...)
		{
		::kernelDebugLogPattern("Crash in CG16bitImage::CreateFromBitmap.");
		return ERR_FAIL;
		}
	}

ALERROR CG16bitImage::CreateFromFile (const CString &sImageFilespec, const CString &sMaskFilespec, DWORD dwFlags)

//	CreateFromFile
//
//	Creates from a file

	{
	ALERROR error;

	//	Load the image

	HBITMAP hImage;
	EBitmapTypes iImageType;
	CString sType = pathGetExtension(sImageFilespec);
	if (strEquals(sType, CONSTLIT("jpg")))
		{
		if (error = ::JPEGLoadFromFile(sImageFilespec, JPEG_LFR_DIB, NULL, &hImage))
			return error;

		iImageType = bitmapRGB;
		}
	else
		{
		if (error = ::dibLoadFromFile(sImageFilespec, &hImage, &iImageType))
			return error;
		}

	//	Load the mask, if it exists

	HBITMAP hImageMask;
	EBitmapTypes iImageMaskType;
	if (!sMaskFilespec.IsBlank())
		{
		CString sType = pathGetExtension(sMaskFilespec);
		if (strEquals(sType, CONSTLIT("jpg")))
			{
			if (error = ::JPEGLoadFromFile(sMaskFilespec, JPEG_LFR_DIB, NULL, &hImageMask))
				return error;

			iImageMaskType = bitmapRGB;
			}
		else
			{
			if (error = ::dibLoadFromFile(sMaskFilespec, &hImageMask, &iImageMaskType))
				return error;
			}
		}
	else
		{
		hImageMask = NULL;
		iImageMaskType = bitmapNone;
		}

	//	Create a new CG16BitImage

	error = CreateFromBitmap(hImage, hImageMask, CG16bitImage::cfbPreMultAlpha);

	//	We don't need these bitmaps anymore

	if (hImage)
		::DeleteObject(hImage);

	if (hImageMask)
		::DeleteObject(hImageMask);

	//	Check for error

	if (error)
		return error;

	//	If we have a monochrom mask, then we assume black is the background color

	if (iImageMaskType == bitmapMonochrome)
		SetTransparentColor();

	return NOERROR;
	}

ALERROR CG16bitImage::CreateFromImage (const CG16bitImage &Image)

//	CreateFromImage
//
//	Creates a copy

	{
	DeleteData();
	CopyData(Image);

	return NOERROR;
	}

ALERROR CG16bitImage::CreateFromImageTransformed (const CG16bitImage &Source,
												  int xSrc, 
												  int ySrc, 
												  int cxSrc, 
												  int cySrc,
												  Metric rScaleX, 
												  Metric rScaleY, 
												  Metric rRotation)

//	CreateFromImageTransformed
//
//	Creates a new image from the source after a transformation.

	{
	DeleteData();

	//	Get some metrics

	CXForm SrcToDest;
	CXForm DestToSrc;
	RECT rcDestXForm;
	if (!CalcBltTransform(0.0, 0.0, rScaleX, rScaleY, rRotation, xSrc, ySrc, cxSrc, cySrc, &SrcToDest, &DestToSrc, &rcDestXForm))
		return NOERROR;

	//	Set some rects

	RECT rcSrc;
	rcSrc.left = xSrc;
	rcSrc.top = ySrc;
	rcSrc.right = xSrc + cxSrc;
	rcSrc.bottom = ySrc + cySrc;

	RECT rcDest;
	rcDest.left = 0;
	rcDest.top = 0;
	rcDest.right = RectWidth(rcDestXForm);
	rcDest.bottom = RectHeight(rcDestXForm);

	//	Create the destination image

	CreateBlank(RectWidth(rcDest), RectHeight(rcDest), Source.HasAlpha(), Source.m_wBackColor);
	m_wBackColor = Source.m_wBackColor;

	//	Copy

	CopyBltTransformed(*this,
			rcDest,
			Source,
			rcSrc,
			SrcToDest,
			DestToSrc,
			rcDestXForm);

	return NOERROR;
	}

void CG16bitImage::DeleteData (void)

//	DeleteData
//
//	Cleanup the bitmap

	{
	if (m_pRGB)
		{
		MemFree(m_pRGB);
		m_pRGB = NULL;
		}

	if (m_pAlpha)
		{
		MemFree(m_pAlpha);
		m_pAlpha = NULL;
		m_bHasMask = false;
		}

	if (m_pRedAlphaTable)
		{
		MemFree(m_pRedAlphaTable);
		m_pRedAlphaTable = NULL;
		}

	if (m_pGreenAlphaTable)
		{
		MemFree(m_pGreenAlphaTable);
		m_pGreenAlphaTable = NULL;
		}

	if (m_pBlueAlphaTable)
		{
		MemFree(m_pBlueAlphaTable);
		m_pBlueAlphaTable = NULL;
		}

	if (m_pSprite)
		{
		delete m_pSprite;
		m_pSprite = NULL;
		}

	if (m_pBMI)
		{
		delete m_pBMI;
		m_pBMI = NULL;
		}

	m_cxWidth = 0;
	m_cyHeight = 0;
	}

WORD CG16bitImage::FadeColor (WORD wStart, WORD wEnd, int iFade)

//	FadeColor
//
//	Fades the color from start to end by iFade (percent).
//	0 = wStart
//	100 = wEnd

	{
	if (iFade <= 0)
		return wStart;
	else if (iFade >= 100)
		return wEnd;
	else
		{
		int sTemp = (DWORD)wStart;
		int dTemp = (DWORD)wEnd;

		int sb = sTemp & 0x1f;
		int db = dTemp & 0x1f;
		int sg = (sTemp >> 5) & 0x3f;
		int dg = (dTemp >> 5) & 0x3f;
		int sr = (sTemp >> 11) & 0x1f;
		int dr = (dTemp >> 11) & 0x1f;

		return (WORD)(sb + ((db - sb) * iFade / 100))
				| (((WORD)(sg + ((dg - sg) * iFade / 100))) << 5)
				| (((WORD)(sr + ((dr - sr) * iFade / 100))) << 11);
		}
	}

void CG16bitImage::Fill (int x, int y, int cxWidth, int cyHeight, WORD wColor)

//	Fill
//
//	Fills the image with the given color

	{
	//	Make sure we're in bounds

	if (!AdjustCoords(NULL, NULL, 0, 0, 
			&x, &y, 
			&cxWidth, &cyHeight))
		return;

	DWORD dwValue;
	DWORD *pStart;
	DWORD *pEnd;

	DWORD *pStartRow = GetRowStartDW(y);
	DWORD *pEndRow = GetRowStartDW(y + cyHeight);

	//	We try to do everything in DWORDs so first we have to figure out
	//	if the first column is odd or even

	bool bOddFirstColumn = false;
	bool bOddLastColumn = false;

	pStart = GetPixelDW(pStartRow, x, &bOddFirstColumn);
	if (bOddFirstColumn)
		pStart++;

	pEnd = GetPixelDW(pStartRow, x + cxWidth, &bOddLastColumn);

	dwValue = MAKELONG(wColor, wColor);

	//	Fill the part that we can

	DWORD *pPos = pStart;
	while (pPos < pEndRow)
		{
		while (pPos < pEnd)
			*pPos++ = dwValue;

		pStart += m_iRGBRowSize;
		pEnd += m_iRGBRowSize;
		pPos = pStart;
		}

	//	Now do the first column

	if (bOddFirstColumn)
		{
		pStart = GetPixelDW(pStartRow, x, &bOddFirstColumn);
		dwValue = ((DWORD)wColor) << 16;

		pPos = pStart;
		while (pPos < pEndRow)
			{
			SetHighPixel(pPos, dwValue);
			pPos += m_iRGBRowSize;
			}
		}

	//	Now do the last column

	if (bOddLastColumn)
		{
		pStart = GetPixelDW(pStartRow, x + cxWidth, &bOddLastColumn);
		dwValue = (DWORD)wColor;

		pPos = pStart;
		while (pPos < pEndRow)
			{
			SetLowPixel(pPos, dwValue);
			pPos += m_iRGBRowSize;
			}
		}
	}

void CG16bitImage::FillAlpha (int x, int y, int cxWidth, int cyHeight, DWORD byOpacity)

//	FillAlpha
//
//	Fills the alpha channel.

	{
	//	Must have alpha channel

	if (m_pAlpha == NULL)
		return;

	//	Make sure we're in bounds

	if (!AdjustCoords(NULL, NULL, 0, 0, 
			&x, &y, 
			&cxWidth, &cyHeight))
		return;

	//	Fill

	BYTE byAlpha = (BYTE)byOpacity;
	BYTE *pRow = GetAlphaValue(x, y);
	BYTE *pRowEnd = GetAlphaValue(x, y + cyHeight);

	while (pRow < pRowEnd)
		{
		BYTE *pPos = pRow;
		BYTE *pPosEnd = pRow + cxWidth;

		while (pPos < pPosEnd)
			*pPos++ = byAlpha;

		pRow = NextAlphaRow(pRow);
		}
	}

void CG16bitImage::FillAlphaMask (int xSrc, int ySrc, int cxWidth, int cyHeight, const CG16bitImage &Source, DWORD byOpacity, int xDest, int yDest)

//	FillAlphaMask
//
//	Fills the alpha channel with the give value through the given mask.

	{
	//	Must have alpha channel

	if (m_pAlpha == NULL)
		return;

	//	Make sure we're in bounds

	if (!AdjustCoords(&xSrc, &ySrc, Source.m_cxWidth, Source.m_cyHeight, 
			&xDest, &yDest,
			&cxWidth, &cyHeight))
		return;

	//	Get the alpha mask

	BYTE byAlpha = (BYTE)byOpacity;
	BYTE *pDestRow = GetAlphaValue(xDest, yDest);

	//	Fill through source mask

	BYTE *pAlphaSrcRow = Source.GetAlphaValue(xSrc, ySrc);
	BYTE *pAlphaSrcRowEnd = Source.GetAlphaValue(xSrc, ySrc + cyHeight);

	while (pAlphaSrcRow < pAlphaSrcRowEnd)
		{
		BYTE *pAlphaPos = pAlphaSrcRow;
		BYTE *pAlphaPosEnd = pAlphaPos + cxWidth;
		BYTE *pDestPos = pDestRow;

		while (pAlphaPos < pAlphaPosEnd)
			if (*pAlphaPos == 0x00)
				{
				pDestPos++;
				pAlphaPos++;
				}
			else if (*pAlphaPos == 0xff)
				{
				*pDestPos++ = byAlpha;
				pAlphaPos++;
				}
			else
				{
				*pDestPos++ = byAlpha * (*pAlphaPos) / 255;
				pAlphaPos++;
				}

		pDestRow = NextAlphaRow(pDestRow);
		pAlphaSrcRow = Source.NextAlphaRow(pAlphaSrcRow);
		}
	}

void CG16bitImage::FillRGB (int x, int y, int cxWidth, int cyHeight, COLORREF rgbValue)

//	FillRGB
//
//	Fills the image with the given color

	{
	//	Make sure we're in bounds

	if (!AdjustCoords(NULL, NULL, 0, 0, 
			&x, &y, 
			&cxWidth, &cyHeight))
		return;

	DWORD dwValue;
	DWORD *pStart;
	DWORD *pEnd;

	DWORD *pStartRow = GetRowStartDW(y);
	DWORD *pEndRow = GetRowStartDW(y + cyHeight);

	//	We try to do everything in DWORDs so first we have to figure out
	//	if the first column is odd or even

	bool bOddFirstColumn = false;
	bool bOddLastColumn = false;

	pStart = GetPixelDW(pStartRow, x, &bOddFirstColumn);
	if (bOddFirstColumn)
		pStart++;

	pEnd = GetPixelDW(pStartRow, x + cxWidth, &bOddLastColumn);

	dwValue = DoublePixelFromRGB(rgbValue);

	//	Fill the part that we can

	DWORD *pPos = pStart;
	while (pPos < pEndRow)
		{
		while (pPos < pEnd)
			*pPos++ = dwValue;

		pStart += m_iRGBRowSize;
		pEnd += m_iRGBRowSize;
		pPos = pStart;
		}

	//	Now do the first column

	if (bOddFirstColumn)
		{
		pStart = GetPixelDW(pStartRow, x, &bOddFirstColumn);
		dwValue = HighPixelFromRGB(rgbValue);

		pPos = pStart;
		while (pPos < pEndRow)
			{
			SetHighPixel(pPos, dwValue);
			pPos += m_iRGBRowSize;
			}
		}

	//	Now do the last column

	if (bOddLastColumn)
		{
		pStart = GetPixelDW(pStartRow, x + cxWidth, &bOddLastColumn);
		dwValue = LowPixelFromRGB(rgbValue);

		pPos = pStart;
		while (pPos < pEndRow)
			{
			SetLowPixel(pPos, dwValue);
			pPos += m_iRGBRowSize;
			}
		}
	}

void CG16bitImage::FillColumn (int x, int y, int cyHeight, WORD wColor)

//	FillColumn
//
//	Fills a single column of pixels

	{
	int yEnd = y + cyHeight;

	//	Make sure this row is in range

	if (x < m_rcClip.left || x >= m_rcClip.right
			|| yEnd <= m_rcClip.top || y >= m_rcClip.bottom)
		return;

	int yStart = max(y, m_rcClip.top);
	yEnd = min(yEnd, m_rcClip.bottom);

	//	Get the pointers

	WORD *pPos = GetRowStart(yStart) + x;
	WORD *pPosEnd = GetRowStart(yEnd) + x;

	//	Do it

	while (pPos < pPosEnd)
		{
		*pPos = wColor;
		pPos = NextRow(pPos);
		}
	}

void CG16bitImage::FillColumnTrans (int x, int y, int cyHeight, WORD wColor, DWORD byOpacity)

//	FillColumnTrans
//
//	Fills a single column of pixels

	{
	int yEnd = y + cyHeight;

	//	Make sure this row is in range

	if (x < m_rcClip.left || x >= m_rcClip.right
			|| yEnd <= m_rcClip.top || y >= m_rcClip.bottom)
		return;

	int yStart = max(y, m_rcClip.top);
	yEnd = min(yEnd, m_rcClip.bottom);

	//	Set up

	BYTE *pAlpha5 = g_Alpha5[byOpacity];
	BYTE *pAlpha6 = g_Alpha6[byOpacity];
	BYTE *pAlpha5Inv = g_Alpha5[255 - byOpacity];
	BYTE *pAlpha6Inv = g_Alpha6[255 - byOpacity];
	DWORD sTemp = wColor;

	//	Get the pointers

	WORD *pPos = GetRowStart(yStart) + x;
	WORD *pPosEnd = GetRowStart(yEnd) + x;

	//	Do it

	while (pPos < pPosEnd)
		{
		DWORD dTemp = *pPos;

		DWORD dwRedResult = pAlpha5Inv[(dTemp & 0xf800) >> 11] + pAlpha5[(sTemp & 0xf800) >> 11];
		DWORD dwGreenResult = pAlpha6Inv[(dTemp & 0x7e0) >> 5] + pAlpha6[(sTemp & 0x7e0) >> 5];
		DWORD dwBlueResult = pAlpha5Inv[(dTemp & 0x1f)] + pAlpha5[(sTemp & 0x1f)];

		*pPos = (WORD)((dwRedResult << 11) | (dwGreenResult << 5) | (dwBlueResult));
		pPos = NextRow(pPos);
		}
	}

void CG16bitImage::FillLine (int x, int y, int cxWidth, WORD wColor)

//	FillLine
//
//	Fills a single raster line

	{
	int xEnd = x + cxWidth;

	//	Make sure this row is in range

	if (y < m_rcClip.top || y >= m_rcClip.bottom
			|| xEnd <= m_rcClip.left || x >= m_rcClip.right)
		return;

	int xStart = max(x, m_rcClip.left);
	xEnd = min(xEnd, m_rcClip.right);

	//	Get the pointers

	WORD *pRow = GetRowStart(y);
	WORD *pPos = pRow + xStart;
	WORD *pEnd = pRow + xEnd;

	//	Do the first pixel (if odd)

	if (xStart % 2)
		*pPos++ = wColor;

	if (pPos == pEnd)
		return;

	DWORD *pPosDW = (DWORD *)pPos;

	//	Do the last pixel (if odd)

	if (xEnd % 2)
		{
		pEnd--;
		*pEnd = wColor;
		}

	DWORD *pEndDW = (DWORD *)pEnd;

	//	Do the middle

	DWORD dwColor = MAKELONG(wColor, wColor);
	while (pPosDW < pEndDW)
		*pPosDW++ = dwColor;
	}

void CG16bitImage::FillLineGray (int x, int y, int cxWidth, WORD wColor, DWORD byOpacity)

//	FillLineGray
//
//	Fills a single raster line

	{
	int xEnd = x + cxWidth;

	//	Make sure this row is in range

	if (y < m_rcClip.top || y >= m_rcClip.bottom)
		return;

	int xStart = max(x, m_rcClip.left);
	xEnd = min(xEnd, m_rcClip.right);

	//	Get the pointers

	WORD *pRow = GetRowStart(y);
	WORD *pPos = pRow + xStart;
	WORD *pEnd = pRow + xEnd;

	//	Set up

	BYTE *pAlpha5 = g_Alpha5[byOpacity];
	BYTE *pAlpha5Inv = g_Alpha5[255 - byOpacity];
	DWORD sTemp = wColor;

	//	Blt

	while (pPos < pEnd)
		{
		DWORD dTemp = *pPos;

		DWORD dwRedResult = pAlpha5Inv[(dTemp & 0xf800) >> 11] + pAlpha5[(sTemp & 0xf800) >> 11];
		DWORD dwGreenResult = pAlpha5Inv[(dTemp & 0x7e0) >> 6] + pAlpha5[(sTemp & 0x7e0) >> 6];
		DWORD dwBlueResult = pAlpha5Inv[(dTemp & 0x1f)] + pAlpha5[(sTemp & 0x1f)];

		*pPos++ = (WORD)((dwRedResult << 11) | (dwGreenResult << 6) | (dwBlueResult));
		}
	}

void CG16bitImage::FillLineTrans (int x, int y, int cxWidth, WORD wColor, DWORD byOpacity)

//	FillLineTrans
//
//	Fills a single raster line

	{
	int xEnd = x + cxWidth;

	//	Make sure this row is in range

	if (y < m_rcClip.top || y >= m_rcClip.bottom)
		return;

	int xStart = max(x, m_rcClip.left);
	xEnd = min(xEnd, m_rcClip.right);

	//	Get the pointers

	WORD *pRow = GetRowStart(y);
	WORD *pPos = pRow + xStart;
	WORD *pEnd = pRow + xEnd;

	//	Set up

	BYTE *pAlpha5 = g_Alpha5[byOpacity];
	BYTE *pAlpha6 = g_Alpha6[byOpacity];
	BYTE *pAlpha5Inv = g_Alpha5[255 - byOpacity];
	BYTE *pAlpha6Inv = g_Alpha6[255 - byOpacity];
	DWORD sTemp = wColor;

	//	Blt

	while (pPos < pEnd)
		{
		DWORD dTemp = *pPos;

		DWORD dwRedResult = pAlpha5Inv[(dTemp & 0xf800) >> 11] + pAlpha5[(sTemp & 0xf800) >> 11];
		DWORD dwGreenResult = pAlpha6Inv[(dTemp & 0x7e0) >> 5] + pAlpha6[(sTemp & 0x7e0) >> 5];
		DWORD dwBlueResult = pAlpha5Inv[(dTemp & 0x1f)] + pAlpha5[(sTemp & 0x1f)];

		*pPos++ = (WORD)((dwRedResult << 11) | (dwGreenResult << 5) | (dwBlueResult));
		}
	}

void CG16bitImage::FillMask (int xSrc, int ySrc, int cxWidth, int cyHeight, const CG16bitImage &Source, WORD wColor, int xDest, int yDest, BYTE byOpacity)

//	FillMask
//
//	Fills the source mask with the given color

	{
	//	Make sure we're in bounds

	if (!AdjustCoords(&xSrc, &ySrc, Source.m_cxWidth, Source.m_cyHeight, 
			&xDest, &yDest,
			&cxWidth, &cyHeight))
		return;

	//	Compute opacity

	DWORD dwOpacity = byOpacity;

	//	If we've got some transparency, built a table to map opacity

	DWORD dwOpacityMap[256];
	if (dwOpacity != 0xff)
		for (int i = 0; i < 256; i++)
			dwOpacityMap[i] = ((i * dwOpacity) >> 8);

	//	If we've got an alpha mask then blt using the transparency
	//	information.

	WORD *pDestRow = GetPixel(GetRowStart(yDest), xDest);
	if (Source.m_pAlpha)
		{
		if (dwOpacity == 0xff)
			{
			BYTE *pAlphaSrcRow = Source.GetAlphaValue(xSrc, ySrc);
			BYTE *pAlphaSrcRowEnd = Source.GetAlphaValue(xSrc, ySrc + cyHeight);

			while (pAlphaSrcRow < pAlphaSrcRowEnd)
				{
				BYTE *pAlphaPos = pAlphaSrcRow;
				BYTE *pAlphaPosEnd = pAlphaPos + cxWidth;
				WORD *pDestPos = pDestRow;

				while (pAlphaPos < pAlphaPosEnd)
					if (*pAlphaPos == 0x00)
						{
						pDestPos++;
						pAlphaPos++;
						}
					else if (*pAlphaPos == 0xff)
						{
						*pDestPos++ = wColor;
						pAlphaPos++;
						}
					else
						{
						*pDestPos++ = BlendPixel(*pDestPos, wColor, *pAlphaPos);
						pAlphaPos++;
						}

				pDestRow = NextRow(pDestRow);
				pAlphaSrcRow = Source.NextAlphaRow(pAlphaSrcRow);
				}
			}
		else
			{
			BYTE *pAlphaSrcRow = Source.GetAlphaValue(xSrc, ySrc);
			BYTE *pAlphaSrcRowEnd = Source.GetAlphaValue(xSrc, ySrc + cyHeight);

			while (pAlphaSrcRow < pAlphaSrcRowEnd)
				{
				BYTE *pAlphaPos = pAlphaSrcRow;
				BYTE *pAlphaPosEnd = pAlphaPos + cxWidth;
				WORD *pDestPos = pDestRow;

				while (pAlphaPos < pAlphaPosEnd)
					if (*pAlphaPos == 0x00)
						{
						pDestPos++;
						pAlphaPos++;
						}
					else if (*pAlphaPos == 0xff)
						{
						*pDestPos++ = BlendPixel(*pDestPos, wColor, dwOpacity);
						pAlphaPos++;
						}
					else
						{
						*pDestPos++ = BlendPixel(*pDestPos, wColor, dwOpacityMap[*pAlphaPos]);
						pAlphaPos++;
						}

				pDestRow = NextRow(pDestRow);
				pAlphaSrcRow = Source.NextAlphaRow(pAlphaSrcRow);
				}
			}
		}
	else if (Source.m_pRGB)
		{
		WORD *pSrcRow = Source.GetPixel(Source.GetRowStart(ySrc), xSrc);
		WORD *pSrcRowEnd = Source.GetPixel(Source.GetRowStart(ySrc + cyHeight), xSrc);
		WORD wBackColor = Source.m_wBackColor;

		if (dwOpacity == 0xff)
			{
			while (pSrcRow < pSrcRowEnd)
				{
				WORD *pSrcPos = pSrcRow;
				WORD *pSrcPosEnd = pSrcRow + cxWidth;
				WORD *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					if (*pSrcPos++ != wBackColor)
						*pDestPos++ = wColor;
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
				WORD *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					if (*pSrcPos++ != wBackColor)
						*pDestPos++ = BlendPixel(*pDestPos, wColor, dwOpacity);
					else
						pDestPos++;

				pSrcRow = Source.NextRow(pSrcRow);
				pDestRow = NextRow(pDestRow);
				}
			}
		}
	}

void CG16bitImage::FillTrans (int x, int y, int cxWidth, int cyHeight, WORD wColor, DWORD byOpacity)

//	FillTrans
//
//	Fill with a color and transparency
	
	{
	//	Make sure we're in bounds

	if (!AdjustCoords(NULL, NULL, 0, 0, 
			&x, &y, 
			&cxWidth, &cyHeight))
		return;

	//	Fill

	WORD *pRow = GetPixel(GetRowStart(y), x);
	WORD *pRowEnd = GetPixel(GetRowStart(y + cyHeight), x);

	while (pRow < pRowEnd)
		{
		WORD *pPos = pRow;
		WORD *pPosEnd = pRow + cxWidth;

		while (pPos < pPosEnd)
			*pPos++ = BlendPixel(*pPos, wColor, byOpacity);

		pRow = NextRow(pRow);
		}
	}

void CG16bitImage::FillTransGray (int x, int y, int cxWidth, int cyHeight, WORD wColor, DWORD byOpacity)

//	FillTransGray
//
//	Fill with a color and transparency
	
	{
	//	Make sure we're in bounds

	if (!AdjustCoords(NULL, NULL, 0, 0, 
			&x, &y, 
			&cxWidth, &cyHeight))
		return;

	//	Fill

	WORD *pRow = GetPixel(GetRowStart(y), x);
	WORD *pRowEnd = GetPixel(GetRowStart(y + cyHeight), x);

	while (pRow < pRowEnd)
		{
		WORD *pPos = pRow;
		WORD *pPosEnd = pRow + cxWidth;

		while (pPos < pPosEnd)
			*pPos++ = BlendPixelGray(*pPos, wColor, byOpacity);

		pRow = NextRow(pRow);
		}
	}

void CG16bitImage::FillTransRGB (int x, int y, int cxWidth, int cyHeight, COLORREF rgbValue, int iAlpha)

//	FillTransRGB
//
//	Fill with a color and transparency
	
	{
	//	Make sure we're in bounds

	if (!AdjustCoords(NULL, NULL, 0, 0, 
			&x, &y, 
			&cxWidth, &cyHeight))
		return;

	//	Prepare

	BYTE byAlpha = (BYTE)iAlpha;
	WORD wColor = RGBValue(GetRValue(rgbValue) * byAlpha / 255,
			GetGValue(rgbValue) * byAlpha / 255,
			GetBValue(rgbValue) * byAlpha / 255);

	//	Fill

	WORD *pRow = GetPixel(GetRowStart(y), x);
	WORD *pRowEnd = GetPixel(GetRowStart(y + cyHeight), x);

	while (pRow < pRowEnd)
		{
		WORD *pPos = pRow;
		WORD *pPosEnd = pRow + cxWidth;

		while (pPos < pPosEnd)
			*pPos++ = BlendPixelPM(*pPos, wColor, byAlpha);

		pRow = NextRow(pRow);
		}
	}

WORD CG16bitImage::GetPixelAlpha (int x, int y)

//	GetPixelAlpha
//
//	Returns the alpha value at the given coordinates

	{
	if (x >=0 && y >= 0 && x < m_cxWidth && y < m_cyHeight)
		{
		if (m_pAlpha)
			return *GetAlphaValue(x, y);
		else
			return (*GetPixel(GetRowStart(y), x) == m_wBackColor ? 0x00 : 0xff);
		}
	else
		return 0x00;
	}

CG16bitImage::RealPixel CG16bitImage::GetRealPixel (const RECT &rcRange, Metric rX, Metric rY, bool *retbBlack)

//	GetRealPixel
//
//	Returns the pixel at the given floating point coordinates. This
//	routine checks the bounds and return 0 if we're out of range

	{
	int x = (int)rX;
	int y = (int)rY;

	if (x < rcRange.left || y < rcRange.top || x >= rcRange.right || y >= rcRange.bottom)
		{
		RealPixel Pixel = { 0.0, 0.0, 0.0 };
		if (retbBlack)
			*retbBlack = true;
		return Pixel;
		}
	else
		{
		RealPixel Pixel;
		WORD IntPixel = *GetPixel(GetRowStart(y), x);

		if (retbBlack)
			*retbBlack = (IntPixel == m_wBackColor);

		Pixel.rRed = (Metric)((IntPixel & 0xf800) >> 11) / 31.0f;
		Pixel.rGreen = (Metric)((IntPixel & 0x7e0) >> 5) / 63.0f;
		Pixel.rBlue = (Metric)(IntPixel & 0x1f) / 31.0f;

		return Pixel;
		}
	}

void CG16bitImage::InitBMI (BITMAPINFO **retpbi)

//	InitBMP
//
//	Initializes the m_pBMI structure

	{
	BITMAPINFO *pbmi = (BITMAPINFO *)(new BYTE [sizeof(BITMAPINFO) + 2 * sizeof(DWORD)]);
	utlMemSet(pbmi, sizeof(BITMAPINFO), 0);

	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth = m_cxWidth;
	pbmi->bmiHeader.biHeight = -m_cyHeight;
	pbmi->bmiHeader.biPlanes = 1;
	pbmi->bmiHeader.biBitCount = 16;
	pbmi->bmiHeader.biCompression = BI_BITFIELDS;

	//	Set up the masks that describe the RGB arrangement

	DWORD *pRGBMask = (DWORD *)&pbmi->bmiColors[0];
	pRGBMask[0] = 0xF800;				//	red component
	pRGBMask[1] = 0x07E0;				//	green component
	pRGBMask[2] = 0x001F;				//	blue component

	//	Done

	*retpbi = pbmi;
	}

void CG16bitImage::IntersectMask (int xMask, int yMask, int cxMask, int cyMask, const CG16bitImage &Mask, int xDest, int yDest)

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
	//	Must have an alpha channel

	if (Mask.m_pAlpha == NULL)
		return;

	//	If the destination does not have an alpha channel, we create one

	if (m_pAlpha == NULL)
		{
		m_iAlphaRowSize = AlignUp(m_cxWidth, sizeof(DWORD)) / sizeof(DWORD);
		m_pAlpha = (DWORD *)MemAlloc(m_cyHeight * m_iAlphaRowSize * sizeof(DWORD));

		utlMemSet(m_pAlpha, m_cyHeight * m_iAlphaRowSize * sizeof(DWORD), (char)(BYTE)255);

		//	LATER: If we have a transparent color, apply that.
		}

	//	Make sure we're in bounds

	if (!AdjustCoords(&xMask, &yMask, Mask.m_cxWidth, Mask.m_cyHeight, 
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
		BYTE *pAlphaRow = GetAlphaValue(0, y);

		if (y >= yDest && y < yDestEnd)
			{
			BYTE *pSrcAlphaRow = Mask.GetAlphaValue(0, yMask + (y - yDest));

			for (x = 0; x < m_cxWidth; x++)
				{
				if (x >= xDest && x < xDestEnd)
					{
					BYTE *pSrcAlpha = pSrcAlphaRow + xMask + (x - xDest);

					pAlphaRow[x] = (BYTE)((DWORD)pAlphaRow[x] * (DWORD)pSrcAlpha[0] / 255);
					}
				else
					pAlphaRow[x] = 0;
				}
			}
		else
			{
			//	If we're outside the mask, then we set to 0

			utlMemSet(pAlphaRow, m_cxWidth, 0);
			}
		}
	}

void CG16bitImage::MaskedBlt (int xSrc, int ySrc, int cxWidth, int cyHeight, CG16bitImage &Source, int xDest, int yDest)

//	MaskedBlt
//
//	Blt using a mask. Any alpha value greater than 0 is part of the image

	{
	if (!AdjustCoords(&xSrc, &ySrc, Source.m_cxWidth, Source.m_cyHeight, 
			&xDest, &yDest,
			&cxWidth, &cyHeight))
		return;

	}

ALERROR CG16bitImage::ReadFromStream (IReadStream *pStream)

//	ReadFromStream
//
//	Read image from stream
//
//	DWORD			version
//	DWORD			m_cxWidth
//	DWORD			m_cyHeight
//	DWORD			m_iRGBRowSize
//	DWORD			m_iAlphaRowSize
//	DWORD			m_wBackColor
//	DWORD			flags
//
//	DWORD			RGB compression method
//	DWORD			bytes to follow (DWORD-aligned)
//	RGB bytes
//
//	DWORD			Alpha compression method
//	DWORD			bytes to follow (DWORD-aligned)
//	Alpha bytes

	{
	DWORD dwLoad;

	DeleteData();
	ResetClipRect();

	pStream->Read((char *)&dwLoad, sizeof(DWORD));
	if (dwLoad > IMAGE_SAVE_VERSION)
		return ERR_FAIL;

	pStream->Read((char *)&m_cxWidth, sizeof(DWORD));
	pStream->Read((char *)&m_cyHeight, sizeof(DWORD));
	pStream->Read((char *)&m_iRGBRowSize, sizeof(DWORD));
	pStream->Read((char *)&m_iAlphaRowSize, sizeof(DWORD));

	pStream->Read((char *)&dwLoad, sizeof(DWORD));
	m_wBackColor = (WORD)dwLoad;

	//	Flags

	pStream->Read((char *)&dwLoad, sizeof(DWORD));
	m_bHasMask = (dwLoad & 0x00000001 ? true : false);

	//	Load the RGB block

	DWORD dwCompression;
	pStream->Read((char *)&dwCompression, sizeof(DWORD));
	switch (dwCompression)
		{
		case RGB_COMPRESSION_NONE:
			pStream->Read((char *)&dwLoad, sizeof(DWORD));
			if (dwLoad != (m_cyHeight * m_iRGBRowSize * sizeof(DWORD)))
				return ERR_FAIL;

			if (dwLoad)
				{
				m_pRGB = (DWORD *)MemAlloc(dwLoad);
				if (m_pRGB == NULL)
					return ERR_FAIL;

				pStream->Read((char *)m_pRGB, dwLoad);
				}
			break;

		default:
			return ERR_FAIL;
		}

	//	Load the alpha block

	pStream->Read((char *)&dwCompression, sizeof(DWORD));
	switch (dwCompression)
		{
		case ALPHA_COMPRESSION_NONE:
			pStream->Read((char *)&dwLoad, sizeof(DWORD));
			if (dwLoad != (m_cyHeight * m_iAlphaRowSize * sizeof(DWORD)))
				return ERR_FAIL;

			if (dwLoad)
				{
				m_pAlpha = (DWORD *)MemAlloc(dwLoad);
				if (m_pAlpha == NULL)
					return ERR_FAIL;

				pStream->Read((char *)m_pAlpha, dwLoad);
				}
			break;

		default:
			return ERR_FAIL;
		}
	
	return NOERROR;
	}

ALERROR CG16bitImage::SaveAsWindowsBMP (const CString &sFilespec)

//	SaveAsWindowsBMP
//
//	Save to a file

	{
	ALERROR error;

	CFileWriteStream OutputFile(sFilespec);
	if (error = OutputFile.Create())
		return error;

	WriteToWindowsBMP(&OutputFile);
	OutputFile.Close();

	return NOERROR;
	}

void CG16bitImage::SetRealPixel (Metric rX, Metric rY, const RealPixel &Value, bool bNotBlack)

//	SetRealPixel
//
//	Sets the pixel value

	{
	int x = (int)rX;
	int y = (int)rY;

	if (x < m_rcClip.left || y < m_rcClip.top || x >= m_rcClip.right || y >= m_rcClip.bottom)
		NULL;
	else
		{
		COLORREF rgbValue = RGB(
				mathRound(Value.rRed * 255.0),
				mathRound(Value.rGreen * 255.0),
				mathRound(Value.rBlue * 255.0));

		WORD wPixel = (WORD)PixelFromRGB(rgbValue);
		if (bNotBlack && wPixel == 0)
			wPixel = 0x0001;

		*GetPixel(GetRowStart(y), x) = wPixel;
		}
	}

void CG16bitImage::SetBlending (WORD wAlpha)

//	SetBlending
//
//	Makes the current bitmap transparent when using ColorTransBlt
//	wAlpha is from 0 to 255, where 255 is opaque and 0 is invisible.

	{
	int iSrc, iDest;

	if (wAlpha == 255)
		{
		MemFree(m_pRedAlphaTable);
		m_pRedAlphaTable = NULL;
		MemFree(m_pGreenAlphaTable);
		m_pGreenAlphaTable = NULL;
		MemFree(m_pBlueAlphaTable);
		m_pBlueAlphaTable = NULL;
		}
	else
		{
		//	Allocate the tables

		if (m_pRedAlphaTable == NULL)
			{
			m_pRedAlphaTable = (WORD *)MemAlloc(2 * 32 * 32);
			m_pGreenAlphaTable = (WORD *)MemAlloc(2 * 64 * 64);
			m_pBlueAlphaTable = (WORD *)MemAlloc(2 * 32 * 32);
			}

		//	Initialize the 5 bit tables

		for (iSrc = 0; iSrc < 32; iSrc++)
			for (iDest = 0; iDest < 32; iDest++)
				{
				m_pRedAlphaTable[iSrc * 32 + iDest] =
						(128 + (255 - (BYTE)wAlpha) * iDest + (BYTE)wAlpha * iSrc) / 256;
				m_pBlueAlphaTable[iSrc * 32 + iDest] = m_pRedAlphaTable[iSrc * 32 + iDest];

				m_pRedAlphaTable[iSrc * 32 + iDest] = m_pRedAlphaTable[iSrc * 32 + iDest] << 11;
				}

		//	Initialize the 6 bit table

		for (iSrc = 0; iSrc < 64; iSrc++)
			for (iDest = 0; iDest < 64; iDest++)
				{
				m_pGreenAlphaTable[iSrc * 64 + iDest] =
						(128 + (255 - (BYTE)wAlpha) * iDest + (BYTE)wAlpha * iSrc) / 256;

				m_pGreenAlphaTable[iSrc * 64 + iDest] = m_pGreenAlphaTable[iSrc * 64 + iDest] << 5;
				}
		}
	}

void CG16bitImage::SetPixelTrans (int x, int y, WORD wColor, DWORD byOpacity)

//	SetPixelTrans
//
//	Sets a pixel

	{
	if (x < m_rcClip.left || y < m_rcClip.top || x >= m_rcClip.right || y >= m_rcClip.bottom)
		return;

	WORD *pPos = GetPixel(GetRowStart(y), x);
	*pPos = BlendPixel(*pPos, wColor, byOpacity);
	}

void CG16bitImage::SetTransparentColor (WORD wColor)

//	SetTransparentColor
//
//	Sets the color to use for transparency. If we've got a mask, it
//	uses the mask to fill in the color on the image itself

	{
	try
		{
		//	Modify

		if (m_pAlpha && m_pRGB)
			{
			//	Generate a pixel that is NOT the transparent color.

			WORD wRed = GetRedValue(wColor);
			WORD wGreen = GetGreenValue(wColor);
			WORD wBlue = GetBlueValue(wColor);

			if (wGreen < 63)
				wGreen++;
			else
				wGreen--;

			WORD wReplaceColor = MakePixel(wRed, wGreen, wBlue);

			//	Replace

			for (int y = 0; y < m_cyHeight; y++)
				{
				BYTE *pSource = (BYTE *)(m_pAlpha + m_iAlphaRowSize * y);
				WORD *pDest = (WORD *)(m_pRGB + m_iRGBRowSize * y);

				for (int x = 0; x < m_cxWidth; x++)
					if (pSource[x] == 0)
						pDest[x] = wColor;
					else if (pDest[x] == wColor)
						pDest[x] = wReplaceColor;
				}

			//	Free the mask since we don't need it anymore

			MemFree(m_pAlpha);
			m_pAlpha = NULL;

			//	The semantic of m_bHasMask is that callers should use
			//	ColorTransBlt instead of Blt.

			m_bHasMask = true;
			}

		m_wBackColor = wColor;
		}
	catch (...)
		{
		kernelDebugLogPattern("Crash in CG16bitImage::SetTransparentColor.");
		}
	}

void CG16bitImage::SwapBuffers (CG16bitImage &Other)

//	SwapBuffers
//
//	Swaps buffers

	{
	Swap(m_pRGB, Other.m_pRGB);
	}

void CG16bitImage::TransBlt (int xSrc, int ySrc, int cxWidth, int cyHeight, int iAlpha, CG16bitImage &Source, int xDest, int yDest)

//	TransBlt
//
//	Blt using alpha transparency

	{
	if (!AdjustCoords(&xSrc, &ySrc, Source.m_cxWidth, Source.m_cyHeight, 
			&xDest, &yDest,
			&cxWidth, &cyHeight))
		return;

	}

void CG16bitImage::WriteToStream (IWriteStream *pStream)

//	WriteToStream
//
//	Write the image to a stream
//
//	DWORD			version
//	DWORD			m_cxWidth
//	DWORD			m_cyHeight
//	DWORD			m_iRGBRowSize
//	DWORD			m_iAlphaRowSize
//	DWORD			m_wBackColor
//	DWORD			flags
//
//	DWORD			RGB compression method
//	DWORD			bytes to follow (DWORD-aligned)
//	RGB bytes
//
//	DWORD			Alpha compression method
//	DWORD			bytes to follow (DWORD-aligned)
//	Alpha bytes

	{
	DWORD dwSave;

	//	We don't support some classes of images yet
	ASSERT(m_pSprite == NULL);
	ASSERT(m_pRedAlphaTable == NULL);
	ASSERT(m_pGreenAlphaTable == NULL);
	ASSERT(m_pBlueAlphaTable == NULL);

	//	Save basic stuff

	dwSave = IMAGE_SAVE_VERSION;
	pStream->Write((char *)&dwSave, sizeof(DWORD));
	pStream->Write((char *)&m_cxWidth, sizeof(DWORD));
	pStream->Write((char *)&m_cyHeight, sizeof(DWORD));
	pStream->Write((char *)&m_iRGBRowSize, sizeof(DWORD));
	pStream->Write((char *)&m_iAlphaRowSize, sizeof(DWORD));

	dwSave = (DWORD)m_wBackColor;
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	dwSave = 0;
	dwSave |= (m_bHasMask			? 0x00000001 : 0);
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	//	Write the RGB block

	if (m_pRGB)
		{
		dwSave = RGB_COMPRESSION_NONE;
		pStream->Write((char *)&dwSave, sizeof(DWORD));

		DWORD dwSize = m_cyHeight * m_iRGBRowSize * sizeof(DWORD);
		pStream->Write((char *)&dwSize, sizeof(DWORD));

		pStream->Write((char *)m_pRGB, dwSize);
		}
	else
		{
		dwSave = RGB_COMPRESSION_NONE;
		pStream->Write((char *)&dwSave, sizeof(DWORD));

		dwSave = 0;
		pStream->Write((char *)&dwSave, sizeof(DWORD));
		}

	//	Write the alpha block

	if (m_pAlpha)
		{
		dwSave = ALPHA_COMPRESSION_NONE;
		pStream->Write((char *)&dwSave, sizeof(DWORD));

		DWORD dwSize = m_cyHeight * m_iAlphaRowSize * sizeof(DWORD);
		pStream->Write((char *)&dwSize, sizeof(DWORD));

		pStream->Write((char *)m_pAlpha, dwSize);
		}
	else
		{
		dwSave = ALPHA_COMPRESSION_NONE;
		pStream->Write((char *)&dwSave, sizeof(DWORD));

		dwSave = 0;
		pStream->Write((char *)&dwSave, sizeof(DWORD));
		}
	}

void CG16bitImage::WriteToWindowsBMP (IWriteStream *pStream)

//	WriteToWindowBMP
//
//	Write to a windows BMP file

	{
	//	Compute the total size of the image data (in bytes)

	int iColorTable = 3 * sizeof(DWORD);
	int iImageDataSize = m_cyHeight * m_iRGBRowSize * sizeof(DWORD);

	BITMAPFILEHEADER header;
	header.bfType = 'MB';
	header.bfOffBits = sizeof(header) + sizeof(BITMAPINFOHEADER) + iColorTable;
	header.bfSize = header.bfOffBits + iImageDataSize;
	header.bfReserved1 = 0;
	header.bfReserved2 = 0;
	pStream->Write((char *)&header, sizeof(header));

	BITMAPINFO *pbmi;
	InitBMI(&pbmi);

	//	Bottom up
	pbmi->bmiHeader.biHeight = -pbmi->bmiHeader.biHeight;

	pStream->Write((char *)pbmi, sizeof(BITMAPINFOHEADER) + iColorTable);

	//	Write the bits bottom-up

	for (int y = m_cyHeight - 1; y >= 0; y--)
		{
		WORD *pRow = GetRowStart(y);
		pStream->Write((char *)pRow, m_iRGBRowSize * sizeof(DWORD));
		}

	delete pbmi;
	}

//	BlendPixel initialization

bool InitBlendPixelTables (void)
	{
	int i, j;

	for (i = 0; i < 32; i++)
		for (j = 0; j < 256; j++)
			g_Alpha5[j][i] = (BYTE)((DWORD)(((i << 3) * (j / 255.0f)) + 4.0f) >> 3);

	for (i = 0; i < 64; i++)
		for (j = 0; j < 256; j++)
			g_Alpha6[j][i] = (BYTE)((DWORD)(((i << 2) * (j / 255.0f)) + 2.0f) >> 2);

	return true;
	}


//	Testing and Timings --------------------------------------------------------

#ifdef DEBUG_TIME
class Test
	{
	public:
		Test (void)
			{
			int i;
			DWORD dwStart, dwTime;
			char szBuffer[1024];

			//	Wait a bit for system to quiesce

			::Sleep(1000);

			//	Create a blank 512x512 destination

			CG16bitImage Dest;
			Dest.CreateBlank(512, 512, false);
			WORD *pPos = Dest.GetRowStart(256);
			pPos += 256;

			//	Create an alpha source

			CG16bitImage AlphaSource;
			AlphaSource.CreateBlank(256, 256, true);
			for (i = 0; i < 256; i++)
				{
				WORD *pRow = AlphaSource.GetRowStart(i);
				WORD *pRowEnd = pRow + 256;
				WORD wColor = CG32bitPixel(255, 0, 0);
				BYTE *pRowAlpha = AlphaSource.GetAlphaRow(i);
				BYTE byTrans = 0;

				while (pRow < pRowEnd)
					{
					*pRow++ = wColor;
					*pRowAlpha++ = byTrans++;
					}
				}
			
#if 0
			dwStart = ::GetTickCount();

			for (i = 0; i < 10000000; i++)
				{
				*pPos = CG16bitImage::BlendPixel(i % 64, i % 32, i % 256);
				}

			dwTime = ::GetTickCount() - dwStart;
			wsprintf(szBuffer, "10,000,000 BlendPixel: %dms\n", dwTime);
			::OutputDebugString(szBuffer);
#endif

#if 0
			dwStart = ::GetTickCount();

			for (i = 0; i < 10000000; i++)
				{
				*pPos = CG16bitImage::BlendPixelPM(i % 64, i % 32, i % 256);
				}

			dwTime = ::GetTickCount() - dwStart;
			wsprintf(szBuffer, "10,000,000 BlendPixelPM: %dms\n", dwTime);
			::OutputDebugString(szBuffer);
#endif

#if 0
			dwStart = ::GetTickCount();

			for (i = 0; i < 1000; i++)
				{
				DrawAlphaGradientCircle(Dest, 256, 256, 100, 255);
				}

			dwTime = ::GetTickCount() - dwStart;
			wsprintf(szBuffer, "1,000 DrawAlphaGradientCircle: %dms\n", dwTime);
			::OutputDebugString(szBuffer);
#endif

#if 1
			dwStart = ::GetTickCount();

			for (i = 0; i < 1000; i++)
				{
				Dest.ColorTransBlt(0, 0, 256, 256, 255, AlphaSource, 0, 0);
				}

			dwTime = ::GetTickCount() - dwStart;
			wsprintf(szBuffer, "1,000 ColorTransBlt (8-bit alpha): %dms\n", dwTime);
			::OutputDebugString(szBuffer);
#endif

			DebugBreak();
			}
	};

Test Testing;

#endif

