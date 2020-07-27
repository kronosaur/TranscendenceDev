//	CConsoleFormat.cpp
//
//	CConsoleFormat class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Console.h"

CString CConsoleFormat::CenterLine (const CString &sLine, int iCols)

//	CenterLine
//
//	Centers a line of text in the given number of columns.

	{
	int iLen = sLine.GetLength();
	if (iLen >= iCols)
		return sLine;

	int iOffset = (iCols - iLen) / 2;

	CString sResult;
	char *pDest = sResult.GetWritePointer(iOffset + iLen);
	char *pDestEnd = pDest + iOffset;
	while (pDest < pDestEnd)
		*pDest++ = ' ';

	const char *pSrc = sLine.GetASCIIZPointer();
	const char *pSrcEnd = pSrc + iLen;
	while (pSrc < pSrcEnd)
		*pDest++ = *pSrc++;

	return sResult;
	}

int CConsoleFormat::WrapText (const CString &sText, TArray<CString> &retLines, int iCols)

//	WrapText
//
//	Splits up the text into lines that fit into the given columns.

	{
	int iLinesAdded = 0;
	const char *pSrc = sText.GetASCIIZPointer();
	const char *pSrcEnd = pSrc + sText.GetLength();

	while (pSrc < pSrcEnd)
		{
		//	Skip until we're past all whitespace

		while (pSrc < pSrcEnd && strIsWhitespace(pSrc))
			pSrc++;

		//	Parse a line

		int iLineLen = 0;
		const char *pLineStart = pSrc;
		bool bLineDone = false;
		bool bParagraphBreak = false;

		while (!bLineDone)
			{
			//	Find the next word

			const char *pWordStart = pSrc;
			while (pSrc < pSrcEnd 
					&& !strIsWhitespace(pSrc))
				pSrc++;

			int iWordLen = (int)(pSrc - pWordStart);

			//	If the word fits, add it to the line.

			if (iLineLen + iWordLen <= iCols || iLineLen == 0)
				{
				iLineLen += iWordLen;

				while (pSrc < pSrcEnd && strIsWhitespace(pSrc))
					{
					if (*pSrc == '\n' || *pSrc == '\r')
						{
						bLineDone = true;
						bParagraphBreak = true;
						break;
						}
					else
						{
						pSrc++;
						iLineLen++;
						}
					}

				if (pSrc == pSrcEnd)
					bLineDone = true;
				}

			//	Otherwise, start a new line.

			else
				{
				bLineDone = true;
				}
			}

		//	Add the line

		if (iLineLen > 0)
			{
			retLines.Insert(CString(pLineStart, iLineLen));
			pSrc = pLineStart + iLineLen;
			iLinesAdded++;

			if (bParagraphBreak)
				{
				retLines.Insert(NULL_STR);
				iLinesAdded++;
				bParagraphBreak = false;
				}
			}
		else
			break;
		}

	return iLinesAdded;
	}
