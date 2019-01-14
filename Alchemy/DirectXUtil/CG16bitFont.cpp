//	CG16bitFont.cpp
//
//	Implementation of raw 16-bit image font object
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

const int FONT_SAVE_VERSION =					1;

#define STR_ELLIPSIS							CONSTLIT("...")

const int g_iStartChar =						' ';
const int g_iCharCount =						0xff - g_iStartChar + 1;

CG16bitFont CG16bitFont::m_DefaultFont;

void FormatLine (char *pPos, int iLen, bool *ioInSmartQuotes, TArray<CString> *retLines);

CG16bitFont::CG16bitFont (void)

//	CG16bitFont constructor

	{
	}

CG16bitFont::CG16bitFont (const CG16bitFont &Src)

	{
	//	Not Yet Implemented
	ASSERT(false); 
	}

CG16bitFont &CG16bitFont:: operator= (const CG16bitFont &Src)

//	CG16bitFont operator =

	{
	//	Not Yet Implemented
	ASSERT(false);
	return *this;
	}

int CG16bitFont::BreakText (const CString &sText, int cxWidth, TArray<CString> *retLines, DWORD dwFlags) const

//	BreakText
//
//	Splits the given string into multiple lines so that they fit
//	in the given width

	{
	char *pPos = sText.GetASCIIZPointer();
	char *pStartLine = pPos;
	int iCharsInLine = 0;
	char *pStartWord = pPos;
	int iCharsInWord = 0;
	int cxWordWidth = 0;
	int cxRemainingWidth = cxWidth;
	int iLines = 0;

    if (retLines)
        retLines->DeleteAll();

	//	Can't handle 0 widths

	if (cxWidth <= 0)
		return 0;

	//	Smart quotes

	bool bInSmartQuotes = false;
	bool *ioInSmartQuotes;
	if (dwFlags & SmartQuotes)
		ioInSmartQuotes = &bInSmartQuotes;
	else
		ioInSmartQuotes = NULL;

	//	If we need to truncate, then we need to adjust the width

	if (dwFlags & TruncateLine)
		{
		cxWidth -= MeasureText(STR_ELLIPSIS);
		if (cxWidth < 0)
			return 0;
		}

	//	Do it

	bool bTruncate = false;
	while (*pPos != '\0')
		{
		//	If we've got a carriage return then we immediately end
		//	the line.

		if (*pPos == '\n')
			{
			//	Add the current word to the line

			iCharsInLine += iCharsInWord;

			//	Add the line to the array

			FormatLine(pStartLine, iCharsInLine, ioInSmartQuotes, retLines);
			iLines++;

			//	Reset the line and word

			pStartLine = pPos + 1;
			pStartWord = pStartLine;
			iCharsInLine = 0;
			iCharsInWord = 0;
			cxWordWidth = 0;
			cxRemainingWidth = cxWidth;

			//	Reset smartquotes (sometimes we end a paragraph without closing
			//	a quote. In that case, we need to start with an open quote).

			bInSmartQuotes = false;

			//	If we're truncating, we're out of here (we don't add an ellipsis)

			if (dwFlags & TruncateLine)
				break;
			}

		//	Otherwise, continue by trying to add the character to the word

		else
			{
			char chChar = *pPos;
			if (chChar == '"' && ioInSmartQuotes) chChar = '“';

			//	Get the metrics for the character

			int iIndex = (int)(BYTE)chChar - g_iStartChar;
			iIndex = max(0, iIndex);

			CharMetrics &Metrics = m_Metrics[iIndex];

			//	Does the character fit in the line?

			bool bCharFits = ((cxRemainingWidth - cxWordWidth) >= Metrics.cxWidth);

			//	If the character doesn't fit, then we've reached the end
			//	of the line.

			if (!bCharFits)
				{
				//	If the character is a space then the entire word should
				//	fit on the line

				if (*pPos == ' ')
					{
					iCharsInLine += iCharsInWord;

					//	Reset the word

					pStartWord = pPos + 1;
					iCharsInWord = 0;
					cxWordWidth = 0;
					}

				//	If this is the first word in the line then we need to break
				//	up the word across lines.

				if (iCharsInLine == 0)
					{
					//	Add what we've got to the array

					FormatLine(pStartWord, iCharsInWord, ioInSmartQuotes, retLines);
					iLines++;

					//	Reset the word

					pStartWord = pPos;
					pStartLine = pStartWord;
					iCharsInWord = 1;
					cxWordWidth = Metrics.cxAdvance;
					cxRemainingWidth = cxWidth - cxWordWidth;
					}

				//	Otherwise, add the line to the array

				else
					{
					FormatLine(pStartLine, iCharsInLine, ioInSmartQuotes, retLines);
					iLines++;

					//	Reset the line

					pStartLine = pStartWord;
					iCharsInLine = 0;
					cxRemainingWidth = cxWidth;

					//	Add the character that didn't fit to the word

					if (*pPos != ' ')
						{
						iCharsInWord++;
						cxWordWidth += Metrics.cxAdvance;
						}
					}

				//	Done if we're truncating

				if (dwFlags & TruncateLine)
					{
					iCharsInLine = 0;
					iCharsInWord = 0;
					bTruncate = true;
					break;
					}
				}

			//	Otherwise, if it does fit, add it to the end of the word

			else
				{
				iCharsInWord++;
				cxWordWidth += Metrics.cxAdvance;

				//	If this character is a space or a hyphen, add it to the
				//	end of the line

				if (*pPos == ' ' || *pPos == '-' || *pPos == '\x97')
					{
					iCharsInLine += iCharsInWord;
					cxRemainingWidth -= cxWordWidth;

					//	Reset the word

					pStartWord = pPos + 1;
					iCharsInWord = 0;
					cxWordWidth = 0;
					}
				}
			}

		//	Next character
		
		pPos++;
		}

	//	Add the remainder

	iCharsInLine += iCharsInWord;
	if (iCharsInLine)
		{
		FormatLine(pStartLine, iCharsInLine, ioInSmartQuotes, retLines);
		iLines++;
		}

	//	Add ellipsis, if necessary

	if (bTruncate && retLines)
		retLines->GetAt(0).Append(STR_ELLIPSIS);

	//	Return the lines

	return iLines;
	}

ALERROR CG16bitFont::Create (const CString &sTypeface, int iSize, bool bBold, bool bItalic, bool bUnderline)

//	Create
//
//	Creates a basic font

	{
	ALERROR error;

	DWORD dwQuality;
	if (Absolute(iSize) <= 10)
		dwQuality = ANTIALIASED_QUALITY;
	else
		dwQuality = PROOF_QUALITY;

	DWORD dwCharSet;
	if (strEquals(sTypeface, CONSTLIT("Wingdings")))
		dwCharSet = SYMBOL_CHARSET;
	else
		dwCharSet = ANSI_CHARSET;

	HFONT hFont = ::CreateFont(iSize,
			0,
			0,
			0,
			(bBold ? FW_BOLD : FW_NORMAL),
			(bItalic ? TRUE : FALSE),
			(bUnderline ? TRUE : FALSE),
			FALSE,
			dwCharSet,
			OUT_TT_ONLY_PRECIS,
			CLIP_DEFAULT_PRECIS,
			dwQuality,
			FF_DONTCARE,
			sTypeface.GetASCIIZPointer());
	error = CreateFromFont(hFont);
	DeleteObject(hFont);

	//	HACK: For now we remember the styles from what was passed in
	//	(In the future, these should be set inside CreateFromFont from
	//	the actual font selected).

	m_bBold = bBold;
	m_bItalic = bItalic;
	m_bUnderline = bUnderline;

	return error;
	}

ALERROR CG16bitFont::CreateFromFont (HFONT hFont)

//	CreatFromFont
//
//	Creates from a GDI font

	{
	ALERROR error;
	HDC hDC = CreateCompatibleDC(NULL);
	HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);
	HBITMAP hTempBmp = NULL;
	HBITMAP hFontBmp = NULL;
	HBITMAP hOldBitmap;
	int i, y;

	//	Get some metrics. For some reason we need to recreate the DC after
	//	the GetTextMetrics or else the fonts won't be anti-aliased!

	TEXTMETRIC tm;
	GetTextMetrics(hDC, &tm);

	SelectObject(hDC, hOldFont);
	DeleteDC(hDC);

	hDC = CreateCompatibleDC(NULL);

	//	Remember some for the whole font metrics

	m_Metrics.DeleteAll();
	m_cyHeight = tm.tmHeight;
	m_cyAscent = tm.tmAscent;
	m_cxAveWidth = tm.tmAveCharWidth;

	//	Create a bitmap that will contain all the font characters

	if (error = dibCreate16bitDIB(tm.tmMaxCharWidth, tm.tmHeight * g_iCharCount, &hFontBmp, NULL))
		goto Fail;

	//	Prepare the DC

	hOldBitmap = (HBITMAP)SelectObject(hDC, hFontBmp);
	hOldFont = (HFONT)SelectObject(hDC, hFont);
	SetTextColor(hDC, RGB(255,255,255));
	SetBkColor(hDC, RGB(0,0,0));
	SetBkMode(hDC, TRANSPARENT);

	//	Get the name of the font that we selected

	char szFontName[256];
	::GetTextFace(hDC, sizeof(szFontName), szFontName);
	m_sTypeface = CString(szFontName);

	//	Write each font character to the bitmap

	y = 0;
	m_Metrics.GrowToFit(g_iCharCount);
	for (i = 0; i < g_iCharCount; i++)
		{
		char chChar = (char)(g_iStartChar + i);

		//	Blt

		TextOut(hDC, 0, y, &chChar, 1);

		//	Remember some metrics

		CharMetrics Metrics;
		ABC abc;
		GetCharABCWidths(hDC, (BYTE)chChar, (BYTE)chChar, &abc);
		Metrics.cxWidth = abc.abcA + abc.abcB;
		Metrics.cxAdvance = abc.abcA + abc.abcB + abc.abcC;
		m_Metrics.Insert(Metrics);

#ifdef MOREDEBUG
		kernelDebugLogPattern("char: %d  width: %d  advance: %d", (int)chChar, Metrics.cxWidth, Metrics.cxAdvance);
#endif

		//	Next

		y += m_cyHeight;
		}

	SelectObject(hDC, hOldBitmap);

	//	Now apply the bitmap to our image

	if (error = m_FontImage.CreateFromBitmap(NULL, hFontBmp, CG16bitImage::cfbDesaturateAlpha))
		goto Fail;

	//	Done

	DeleteObject(hFontBmp);
	SelectObject(hDC, hOldFont);
	DeleteDC(hDC);

	return NOERROR;

Fail:

	if (hFontBmp)
		{
		SelectObject(hDC, hOldBitmap);
		DeleteObject(hFontBmp);
		}

	SelectObject(hDC, hOldFont);
	DeleteDC(hDC);
	return error;
	}

ALERROR CG16bitFont::CreateFromResource (HINSTANCE hInst, char *pszRes)

//	CreateFromResource
//
//	Loads the font from a resource
//	Use DirectXFont to save a font to a file

	{
	ALERROR error;
	HRSRC hRes;
	HGLOBAL hGlobalRes;
	void *pImage;
	int iSize;

	//	Load the resource

	hRes = ::FindResource(hInst, pszRes, "DXFN");
	if (hRes == NULL)
		return ERR_FAIL;

	iSize = ::SizeofResource(hInst, hRes);
	if (iSize == 0)
		return ERR_FAIL;

	hGlobalRes = ::LoadResource(hInst, hRes);
	if (hGlobalRes == NULL)
		return ERR_FAIL;

	pImage = ::LockResource(hGlobalRes);
	if (pImage == NULL)
		return ERR_FAIL;

	//	Load

	CMemoryReadStream Stream((char *)pImage, iSize);
	if (error = Stream.Open())
		return error;

	if (error = ReadFromStream(&Stream))
		return error;

	Stream.Close();

	return NOERROR;
	}

void CG16bitFont::DrawText (CG16bitImage &Dest, 
							int x, 
							int y, 
							WORD wColor, 
							DWORD byOpacity,
							const CString &sText,
							DWORD dwFlags,
							int *retx) const

//	DrawText
//
//	Draws a line of text on the given image

	{
	char *pPos = sText.GetASCIIZPointer();
	char *pEndPos = pPos + sText.GetLength();
	int xPos = x;

	int cxWidth = -1;
	if (dwFlags & AlignCenter)
		{
		cxWidth = MeasureText(sText);
		xPos -= cxWidth / 2;
		}
	else if (dwFlags & AlignRight)
		{
		cxWidth = MeasureText(sText);
		xPos -= cxWidth;
		}
	else if (dwFlags & AdjustToFit)
		cxWidth = MeasureText(sText);

	//	If necessary, adjust our position so we can fit inside the clip region 
	//	of the destination.

	if (dwFlags & AdjustToFit)
		{
		const RECT &rcClip = Dest.GetClipRect();
		xPos = Min(Max((int)rcClip.left, xPos), (int)rcClip.right - cxWidth);
		}

	bool bInQuotes = false;
	while (pPos < pEndPos)
		{
		//	Get metrics

		int iIndex = (int)(BYTE)(*pPos) - g_iStartChar;
		iIndex = max(0, iIndex);

		CharMetrics &Metrics = m_Metrics[iIndex];

		//	Paint

		Dest.FillMask(0,
				iIndex * m_cyHeight,
				Metrics.cxWidth,
				m_cyHeight,
				m_FontImage,
				wColor,
				xPos,
				y,
				(BYTE)byOpacity);

		pPos++;
		xPos += Metrics.cxAdvance;
		}
	
	if (retx)
		*retx = xPos;
	}

void CG16bitFont::DrawText (CG16bitImage &Dest, 
							const RECT &rcRect, 
							WORD wColor, 
							DWORD byOpacity,
							const CString &sText, 
							int iLineAdj, 
							DWORD dwFlags,
							int *retcyHeight) const

//	Draw
//
//	Draws wrapped text

	{
	int i;
	TArray<CString> Lines;

	BreakText(sText, RectWidth(rcRect), &Lines, dwFlags);
	int y = rcRect.top;
	for (i = 0; i < Lines.GetCount(); i++)
		{
		int x = rcRect.left;

		if (dwFlags & AlignCenter)
			{
			int cxWidth = MeasureText(Lines[i]);
			x = rcRect.left + (RectWidth(rcRect) - cxWidth) / 2;
			}
		else if (dwFlags & AlignRight)
			{
			int cxWidth = MeasureText(Lines[i]);
			x = rcRect.right - cxWidth;
			}

		if (!(dwFlags & MeasureOnly))
			DrawText(Dest, x, y, wColor, byOpacity, Lines[i]);

		y += m_cyHeight + iLineAdj;
		}

	if (retcyHeight)
		*retcyHeight = y - rcRect.top;
	}

void CG16bitFont::DrawText (CG32bitImage &Dest, 
							int x, 
							int y, 
							CG32bitPixel rgbColor, 
							const CString &sText,
							DWORD dwFlags,
							int *retx) const

//	DrawText
//
//	Draws a line of text on the given image

	{
	char *pPos = sText.GetASCIIZPointer();
	char *pEndPos = pPos + sText.GetLength();
	int xPos = x;

	int cxWidth = -1;
	if (dwFlags & AlignCenter)
		{
		cxWidth = MeasureText(sText);
		xPos -= cxWidth / 2;
		}
	else if (dwFlags & AlignRight)
		{
		cxWidth = MeasureText(sText);
		xPos -= cxWidth;
		}
	else if (dwFlags & AdjustToFit)
		cxWidth = MeasureText(sText);

	//	If necessary, adjust our position so we can fit inside the clip region 
	//	of the destination.

	if (dwFlags & AdjustToFit)
		{
		const RECT &rcClip = Dest.GetClipRect();
		xPos = Min(Max((int)rcClip.left, xPos), (int)rcClip.right - cxWidth);
		}

	bool bInQuotes = false;
	while (pPos < pEndPos)
		{
		//	Get metrics

		int iIndex = (int)(BYTE)(*pPos) - g_iStartChar;
		iIndex = max(0, iIndex);

		CharMetrics &Metrics = m_Metrics[iIndex];

		//	Paint

		Dest.FillMask(0,
				iIndex * m_cyHeight,
				Metrics.cxWidth,
				m_cyHeight,
				m_FontImage,
				rgbColor,
				xPos,
				y);

		pPos++;
		xPos += Metrics.cxAdvance;
		}
	
	if (retx)
		*retx = xPos;
	}

void CG16bitFont::DrawText (CG32bitImage &Dest, 
							const RECT &rcRect, 
							CG32bitPixel rgbColor,
							const CString &sText, 
							int iLineAdj, 
							DWORD dwFlags,
							int *retcyHeight) const

//	Draw
//
//	Draws wrapped text

	{
	int i;
	TArray<CString> Lines;

    //  Break into lines

	BreakText(sText, RectWidth(rcRect), &Lines, dwFlags);

	int iLines = Lines.GetCount();
	int cyHeight = iLines * m_cyHeight + (iLineAdj * (iLines - 1));

	//	If we're truncating, see if we exceeded the vertical height.

	int iEllipsisLine = -1;
	if (dwFlags & TruncateBlock)
		{
		while (iLines > 1 && cyHeight > RectHeight(rcRect))
			{
			iLines--;
			cyHeight -= (m_cyHeight + iLineAdj);
			}

		//	If necessary, modify the last line to add an ellipsis

		if (Lines.GetCount() != iLines)
			{
			int cxEllipsis = MeasureText(STR_ELLIPSIS);

			CString &sLastLine = Lines[iLines - 1];
			int cxLastLine = MeasureText(sLastLine) + cxEllipsis;

			char *pPos = sLastLine.GetASCIIZPointer();
			char *pPosEnd = pPos + sLastLine.GetLength();

			while (pPosEnd > pPos && cxLastLine > RectWidth(rcRect))
				{
				pPosEnd--;

				const CharMetrics &Metrics = GetCharMetrics(*pPosEnd);
				cxLastLine -= Metrics.cxAdvance;
				}

			CString sNewLine = CString(pPos, (int)(pPosEnd - pPos));
			sNewLine.Append(STR_ELLIPSIS);

			Lines[iLines - 1] = sNewLine;
			}
		}

    //  Compute top alignment

	int y = rcRect.top;
    if (dwFlags & AlignMiddle)
        {
        y += (RectHeight(rcRect) - cyHeight) / 2;
        }

    //  Paint each line

	for (i = 0; i < iLines; i++)
		{
		int x = rcRect.left;

		if (dwFlags & AlignCenter)
			{
			int cxWidth = MeasureText(Lines[i]);
			x = rcRect.left + (RectWidth(rcRect) - cxWidth) / 2;
			}
		else if (dwFlags & AlignRight)
			{
			int cxWidth = MeasureText(Lines[i]);
			x = rcRect.right - cxWidth;
			}

		if (!(dwFlags & MeasureOnly))
			DrawText(Dest, x, y, rgbColor, Lines[i]);

		y += m_cyHeight + iLineAdj;
		}

    //  Done

	if (retcyHeight)
		*retcyHeight = y - rcRect.top;
	}

void CG16bitFont::DrawText (CG32bitImage &Dest, int x, int y, CG32bitPixel rgbColor, const TArray<CString> &Lines, int iLineAdj, DWORD dwFlags, int *rety) const

//  DrawText
//
//  Draws an array of lines.

    {
    int i;

    //  Paint each line

	for (i = 0; i < Lines.GetCount(); i++)
		{
        int xPos;

        if (dwFlags & AlignCenter)
            {
            int cxWidth = MeasureText(Lines[i]);
            xPos = x - (cxWidth / 2);
            }
        else if (dwFlags & AlignRight)
            {
            int cxWidth = MeasureText(Lines[i]);
            xPos = x - cxWidth;
            }
        else
            xPos = x;

		if (!(dwFlags & MeasureOnly))
			DrawText(Dest, x, y, rgbColor, Lines[i]);

		y += m_cyHeight + iLineAdj;
		}

    //  Done

    if (rety)
        *rety = y;
    }

void CG16bitFont::DrawTextEffect (CG16bitImage &Dest,
								  int x,
								  int y,
								  WORD wColor,
								  const CString &sText,
								  int iEffectsCount,
								  const SEffectDesc *pEffects,
								  DWORD dwFlags,
								  int *retx) const

//	DrawTextEffect
//
//	Draw text with effect

	{
	int i;

	//	Paint background effects

	for (i = 0; i < iEffectsCount; i++)
		{
		switch (pEffects[i].iType)
			{
			case effectShadow:
				{
				int xOffset = m_cyHeight / 16;
				int yOffset = m_cyHeight / 16;

				DrawText(Dest,
						x + xOffset,
						y + yOffset, 
						CG16bitImage::RGBValue(0, 0, 0),
						sText,
						dwFlags);
				break;
				}
			}
		}

	//	Paint

	DrawText(Dest, x, y, wColor, sText, dwFlags, retx);
	}

void CG16bitFont::DrawTextEffect (CG32bitImage &Dest,
								  int x,
								  int y,
								  CG32bitPixel rgbColor,
								  const CString &sText,
								  int iEffectsCount,
								  const SEffectDesc *pEffects,
								  DWORD dwFlags,
								  int *retx) const

//	DrawTextEffect
//
//	Draw text with effect

	{
	int i;

	//	Paint background effects

	for (i = 0; i < iEffectsCount; i++)
		{
		switch (pEffects[i].iType)
			{
			case effectShadow:
				{
				int xOffset = m_cyHeight / 16;
				int yOffset = m_cyHeight / 16;

				DrawText(Dest,
						x + xOffset,
						y + yOffset, 
						CG32bitPixel(0, 0, 0),
						sText,
						dwFlags);
				break;
				}
			}
		}

	//	Paint

	DrawText(Dest, x, y, rgbColor, sText, dwFlags, retx);
	}

const CG16bitImage &CG16bitFont::GetCharacterImage (char chChar, int *retx, int *rety, int *retcxWidth, int *retcyHeight, int *retcxAdvance) const

//	GetCharacterImage
//
//	Returns the character image

	{
	//	Get metrics

	int iIndex = (int)(BYTE)(chChar) - g_iStartChar;
	iIndex = Max(0, iIndex);

	CharMetrics &Metrics = m_Metrics[iIndex];

	//	Paint

	if (retx)
		*retx = 0;

	if (rety)
		*rety = iIndex * m_cyHeight;

	if (retcxWidth)
		*retcxWidth = Metrics.cxWidth;

	if (retcyHeight)
		*retcyHeight = m_cyHeight;

	if (retcxAdvance)
		*retcxAdvance = Metrics.cxAdvance;

	return m_FontImage;
	}

const CG16bitFont::CharMetrics &CG16bitFont::GetCharMetrics (char chChar) const

//	GetCharMetrics
//
//	Return metrics for this character.

	{
	int iIndex = (int)(BYTE)chChar - g_iStartChar;
	iIndex = Max(0, iIndex);
	return m_Metrics[iIndex];
	}

const CG16bitFont &CG16bitFont::GetDefault (void)

//	GetDefault
//
//	Returns a default font.

	{
	if (m_DefaultFont.IsEmpty())
		{
		m_DefaultFont.Create(CONSTLIT("Arial"), -16);

		m_DefaultFont.m_sTypeface = NULL_STR;
		}

	return m_DefaultFont;
	}

int CG16bitFont::MeasureText (const CString &sText, int *retcyHeight, bool bAlwaysAdvance) const

//	MeasureText
//
//	Returns the width of the text in pixels and optionally the height

	{
	int cxWidth = 0;
	char *pPos = sText.GetASCIIZPointer();
	char *pEndPos = pPos + sText.GetLength();

	while (pPos != pEndPos)
		{
		int iIndex = (int)(BYTE)(*pPos) - g_iStartChar;
		iIndex = max(0, iIndex);

		CharMetrics &Metrics = m_Metrics[iIndex];

		pPos++;

		//	The last character needs to have the whole width; all
		//	others just use the advance distance.
		//	(Except for spaces, which should always use the advance width
		//	otherwise, the cxWidth seems to be 0)

		if (pPos == pEndPos && iIndex != 0 && !bAlwaysAdvance)
			cxWidth += Metrics.cxWidth;
		else
			cxWidth += Metrics.cxAdvance;
		}

	//	Done

	if (retcyHeight)
		*retcyHeight = m_cyHeight;

	return cxWidth;
	}

bool CG16bitFont::ParseFontDesc (const CString &sDesc, CString *retsTypeface, int *retiSize, bool *retbBold, bool *retbItalic)

//	ParseFontDesc
//
//	Parses a string of the form:
//
//	{typeface} {font-size} [bold] [italic]
//
//	Returns TRUE if valid

	{
	char *pPos = sDesc.GetASCIIZPointer();
	CString sTypeface = CONSTLIT("Arial");
	int iSize = 16;
	bool bBold = false;
	bool bItalic = false;

	//	Skip whitespace

	while (*pPos == ' ' || *pPos == '\t')
		pPos++;

	//	Skip delimiters

	bool bSpaceDelimiter;
	if (*pPos == '\'' || *pPos == '\"')
		{
		bSpaceDelimiter = false;
		pPos++;
		}
	else
		bSpaceDelimiter = true;

	//	Look for typeface

	char *pStart = pPos;
	while (*pPos != '\'' && *pPos != '\"' && (!bSpaceDelimiter || *pPos != ' ') && *pPos != '\0')
		pPos++;

	sTypeface = CString(pStart, pPos - pStart);

	//	Look for size

	while ((*pPos < '0' || *pPos > '9') && *pPos != '\0')
		pPos++;

	if (*pPos == '\0')
		{
		//printf("DirectXFont: Expected font size.");
		return false;
		}

	iSize = strParseInt(pPos, -1, &pPos);
	if (iSize == -1)
		{
		//printf("DirectXFont: Invalid font size.");
		return false;
		}

	//	Skip whitespace

	while (*pPos == ' ' || *pPos == '\t')
		pPos++;

	//	Parse bold/italic

	pStart = pPos;
	bool bDone = (*pPos == '\0');
	while (!bDone)
		{
		if (*pPos == ' ' || *pPos == '\0')
			{
			CString sStyle = CString(pStart, pPos - pStart);
			if (strEquals(sStyle, CONSTLIT("bold")))
				bBold = true;
			else if (strEquals(sStyle, CONSTLIT("italic")))
				bItalic = true;

			//	Skip whitespace

			while (*pPos == ' ')
				pPos++;

			pStart = pPos;
			bDone = (*pPos == '\0');
			}
		else
			pPos++;
		}

	//	Done

	*retsTypeface = sTypeface;
	*retiSize = iSize;
	*retbBold = bBold;
	*retbItalic = bItalic;

	return true;
	}

ALERROR CG16bitFont::ReadFromStream (IReadStream *pStream)

//	ReadFromStream
//
//	Read the font from the stream

	{
	ALERROR error;
	int i;
	DWORD dwVersion;
	DWORD dwLoad;

	pStream->Read((char *)&dwVersion, sizeof(DWORD));
	if (dwVersion > FONT_SAVE_VERSION)
		return ERR_FAIL;

	m_sTypeface.ReadFromStream(pStream);
	pStream->Read((char *)&m_cyHeight, sizeof(DWORD));
	pStream->Read((char *)&m_cyAscent, sizeof(DWORD));
	pStream->Read((char *)&m_cxAveWidth, sizeof(DWORD));

	//	Start char
	pStream->Read((char *)&dwLoad, sizeof(DWORD));

	//	Load metrics
	pStream->Read((char *)&dwLoad, sizeof(DWORD));

	m_Metrics.DeleteAll();
	m_Metrics.InsertEmpty(dwLoad);
	for (i = 0; i < m_Metrics.GetCount(); i++)
		{
		CharMetrics *pMetrics = &m_Metrics[i];

		pStream->Read((char *)&pMetrics->cxWidth, sizeof(DWORD));
		pStream->Read((char *)&pMetrics->cxAdvance, sizeof(DWORD));
		}

	if (error = m_FontImage.ReadFromStream(pStream))
		return error;

	return NOERROR;
	}

void CG16bitFont::WriteToStream (IWriteStream *pStream)

//	WriteToStream
//
//	Writes the font to a stream
//
//	DWORD		version
//	CString		m_sTypeface
//	DWORD		m_cyHeight
//	DWORD		m_cyAscent
//	DWORD		m_cxAveWidth
//
//	DWORD		start char
//	DWORD		count of metrics
//	CharMetrics
//
//	CG16bitImage

	{
	int i;
	DWORD dwSave;

	dwSave = FONT_SAVE_VERSION;
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	m_sTypeface.WriteToStream(pStream);
	pStream->Write((char *)&m_cyHeight, sizeof(DWORD));
	pStream->Write((char *)&m_cyAscent, sizeof(DWORD));
	pStream->Write((char *)&m_cxAveWidth, sizeof(DWORD));

	dwSave = g_iStartChar;
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	dwSave = m_Metrics.GetCount();
	pStream->Write((char *)&dwSave, sizeof(DWORD));
	for (i = 0; i < m_Metrics.GetCount(); i++)
		{
		CharMetrics *pMetrics = &m_Metrics[i];

		pStream->Write((char *)&pMetrics->cxWidth, sizeof(DWORD));
		pStream->Write((char *)&pMetrics->cxAdvance, sizeof(DWORD));
		}

	m_FontImage.WriteToStream(pStream);
	}

//	Internals -----------------------------------------------------------------

void FormatLine (char *pPos, int iLen, bool *ioInSmartQuotes, TArray<CString> *retLines)
	{
	if (retLines == NULL)
		return;

	if (ioInSmartQuotes)
		{
		//	Add a new empty line

		int iIndex = retLines->GetCount();
		retLines->Insert(NULL_STR);
		CString &sLine = retLines->GetAt(iIndex);

		//	Copy the characters to the new empty line
		//	(converting to smart quotes as appropriate)

		char *pEndPos = pPos + iLen;
		char *pStart = pPos;
		while (pPos < pEndPos)
			{
			if (*pPos == '\"')
				{
				if (pStart != pPos)
					sLine.Append(CString(pStart, pPos - pStart, true));

				if (*ioInSmartQuotes)
					sLine.Append(CString("”", 1, true));
				else
					sLine.Append(CString("“", 1, true));

				*ioInSmartQuotes = !(*ioInSmartQuotes);

				pPos++;
				pStart = pPos;
				}
			else
				pPos++;
			}

		if (pStart != pPos)
			sLine.Append(CString(pStart, pPos - pStart, true));
		}
	else
		retLines->Insert(CString(pPos, iLen));
	}

