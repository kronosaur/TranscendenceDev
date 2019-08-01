//	CRTFText.cpp
//
//	CRTFText class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CRTFText::CRTFText (const CString &sText, const IFontTable *pFontTable) :
		m_sRTF(GenerateRTFText(sText, SAutoRTFOptions())),
		m_pFontTable(pFontTable)

//	CRTFText constructor

	{
	}

void CRTFText::Format (const RECT &rcRect) const

//	Format
//
//	Format to fit the given rect.

	{
	int cxWidth = RectWidth(rcRect);

	//	If format is valid, then nothing to do.

	if (cxWidth == m_cxFormatted)
		return;

	//	Otherwise, format

	SBlockFormatDesc BlockFormat;

	BlockFormat.cxWidth = cxWidth;
	BlockFormat.cyHeight = -1;
	BlockFormat.iHorzAlign = m_iAlign;
	BlockFormat.iVertAlign = alignTop;
	BlockFormat.iExtraLineSpacing = 0;

	BlockFormat.DefaultFormat.rgbColor = m_rgbDefaultColor;
	BlockFormat.DefaultFormat.pFont = m_pDefaultFont;

	m_RichText.InitFromRTF(m_sRTF, *m_pFontTable, BlockFormat);
	m_cxFormatted = cxWidth;
	}

CString CRTFText::GenerateRTFText (const CString &sText, const SAutoRTFOptions &Options)

//	GenerateRTFText
//
//	Converts from plain text to RTF.

	{
	if (sText.IsBlank())
		return NULL_STR;

	CMemoryWriteStream Output;
	if (Output.Create() != NOERROR)
		return NULL_STR;

	//	If the text already starts with the RTF prefix, then we assume it is
	//	properly formed RTF.

	if (strStartsWith(sText, CONSTLIT("{/rtf ")))
		return sText;

	CG32bitPixel rgbColor = Options.rgbQuoteText;
	CString sCloseQuote = strPatternSubst(CONSTLIT("%&rdquo;}"));

	//	Start with the RTF open

	Output.Write("{/rtf ", 6);

	//	Parse

	bool bInQuotes = false;
	char *pPos = sText.GetASCIIZPointer();
	while (*pPos != '\0')
		{
		if (*pPos == '\"')
			{
			if (Options.bNoQuoteHighlight)
				Output.Write("\"", 2);
			else if (bInQuotes)
				{
				Output.Write(sCloseQuote.GetPointer(), sCloseQuote.GetLength());
				bInQuotes = false;
				}
			else
				{
				CString sQuoteStart = strPatternSubst(CONSTLIT("{/c:#%02x%02x%02x; %&ldquo;"), rgbColor.GetRed(), rgbColor.GetGreen(), rgbColor.GetBlue());
				Output.Write(sQuoteStart.GetASCIIZPointer(), sQuoteStart.GetLength());
				bInQuotes = true;
				}
			}
		else if (*pPos == '\\')
			{
			switch (pPos[1])
				{
				case '\0':
					break;

				case '\\':
					pPos++;
					Output.Write("/\\", 2);
					break;

				default:
					pPos++;
					Output.Write(pPos, 1);
					break;
				}
			}
		else if (*pPos == '/')
			Output.Write("//", 2);
		else if (*pPos == '{')
			Output.Write("/{", 2);
		else if (*pPos == '}')
			Output.Write("/}", 2);
		else if (*pPos == '\n')
			{
			if (bInQuotes)
				{
				//	Look ahead and see if the next paragraph starts with a quote. If it does, then
				//	we close here.

				char *pScan = pPos + 1;
				while (*pScan != '\0' && (*pScan == '\n' || *pScan == ' '))
					pScan++;

				if (*pScan == '\"')
					{
					Output.Write("}", 1);
					bInQuotes = false;
					}
				}

			Output.Write("/n", 2);
			}
		else
			Output.Write(pPos, 1);

		pPos++;
		}

	//	Make sure we always match

	if (bInQuotes)
		Output.Write("}", 1);

	//	Close

	Output.Write("}", 1);

	//	Done

	return CString(Output.GetPointer(), Output.GetLength());
	}

void CRTFText::GetBounds (const RECT &rcRect, RECT *retrcRect) const

//	GetBounds
//
//	Returns the size of the formatted text.

	{
	Format(rcRect);
	m_RichText.GetBounds(retrcRect);
	}

void CRTFText::Paint (CG32bitImage &Dest, const RECT &rcRect) const

//	Paint
//
//	Paint on the given rect.

	{
	int i;

	//	Must have a font table

	if (m_pFontTable == NULL)
		return;

	//	Format, if necessary

	Format(rcRect);

	//	Paint

	for (i = 0; i < m_RichText.GetFormattedSpanCount(); i++)
		{
		const SFormattedTextSpan &Span = m_RichText.GetFormattedSpan(i);

		Span.Format.pFont->DrawText(Dest,
				rcRect.left + Span.x,
				rcRect.top + Span.y,
				Span.Format.rgbColor,
				Span.sText);
		}
	}

void CRTFText::SetText (const CString &sText, const SAutoRTFOptions &Options)

//	SetText
//
//	Set the text.

	{
	m_sRTF = GenerateRTFText(sText, Options);
	InvalidateFormat();
	}
