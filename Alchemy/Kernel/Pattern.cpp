//	Pattern.cpp
//
//	String pattern package
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Internets.h"

void WritePadding (CString &sOutput, char chChar, int iLen);

//	strPattern
//
//	Returns a string with a pattern substitution:
//
//	%Type
//	%ModifiersType
//
//	where type is one of the following:
//
//	d	Argument is a signed 32-bit integer. The number is substituted
//		Supports the following modifiers:
// 
//		,	A leading comma means use a thousands separator 
// 
//		l	An l means to treat this as a 64-bit int instead
// 
//		0	Pad width with 0s (requires a width value)
// 
//		1-9	Enforce this width
// 
//	r	Argument is a double (64-bit float). The number is substituted.
//
//	p	If the last numeral argument not 1, then 's' is substituted.
//		This is used to pluralize words in the English language.
//
//	s	Argument is a CString. The string is substituted
//
//	x	Argument is an unsigned 32-bit integer. The hex value is substituted
//		Supports the following modifiers:
// 
//		0	Pad width with 0s (requires a width value)
// 
//		1-9	Enforce this width
//
//	%	Evaluates to a single percent sign
//
//	&	Followed by an XML entity and semicolon
//
CString Kernel::strPattern (const CString &sPattern, LPVOID *pArgs)

	{
	CString sOutput;
	sOutput.GrowToFit(4000);
	const char *pPos = sPattern.GetPointer();
	int iLength = sPattern.GetLength();
	const char *pRunStart;
	int iRunLength;
	int iLastInteger = 1;

	//	Start

	pRunStart = pPos;
	iRunLength = 0;

	//	Loop

	while (iLength > 0)
		{
		if (*pPos == '%')
			{
			//	Save out what we've got til now

			if (iRunLength > 0)
				{
				sOutput.Append(pRunStart, iRunLength, CString::FLAG_ALLOC_EXTRA);
				}

			//	Check the actual pattern code

			pPos++;
			iLength--;
			if (iLength > 0)
				{
				int iMinFieldWidth = 0;
				bool bPadWithZeros = false;
				bool b1000Separator = false;
				bool b64bit = false;

				//	A leading comma means add a thousands separator

				if (*pPos == ',')
					{
					b1000Separator = true;
					pPos++;
					iLength--;
					}

				//	See if this is a long long

				if (*pPos == 'l')
					{
					pPos++;
					iLength--;

					if (*pPos == 'l')
						{
						b64bit = true;
						pPos++;
						iLength--;
						}
					}

				//	See if we've got a field width value

				if (*pPos >= '0' && *pPos <= '9')
					{
					const char *pNewPos;
					bPadWithZeros = (*pPos == '0');
					iMinFieldWidth = strParseInt(pPos, 0, &pNewPos);

					if (iLength > (pNewPos - pPos))
						{
						iLength -= (pNewPos - pPos);
						pPos = pNewPos;
						}
					}

				//	Parse the type

				if (*pPos == 's')
					{
					CString *pParam = (CString *)pArgs;

					if (iMinFieldWidth > 0)
						WritePadding(sOutput, (bPadWithZeros ? '0' : ' '), iMinFieldWidth - pParam->GetLength());

					sOutput.Append(*pParam, CString::FLAG_ALLOC_EXTRA);

					pArgs += AlignUp(sizeof(CString), sizeof(LPVOID)) / sizeof(LPVOID);
					pPos++;
					iLength--;
					}
				else if (*pPos == 'd')
					{
					if (b64bit)
						{
						INT64 *pVar = (INT64 *)pArgs;

						DWORD dwFlags = (b1000Separator ? FORMAT_THOUSAND_SEPARATOR : 0)
								| (bPadWithZeros ? FORMAT_LEADING_ZERO : 0);

						CString sNew = strFormatInteger(*pVar, iMinFieldWidth, dwFlags);

						sOutput.Append(sNew, CString::FLAG_ALLOC_EXTRA);

						//	Remember the last integer (all we care about is whether it
						//	is 1 or not, for pluralization).

						iLastInteger = (*pVar == 1 ? 1 : 0);

						//	Next

						pArgs++;
						pArgs++;
						}
					else
						{
						int *pInt = (int *)pArgs;

						DWORD dwFlags = (b1000Separator ? FORMAT_THOUSAND_SEPARATOR : 0)
								| (bPadWithZeros ? FORMAT_LEADING_ZERO : 0);

						CString sNew = strFormatInteger(*pInt, iMinFieldWidth, dwFlags);

						sOutput.Append(sNew, CString::FLAG_ALLOC_EXTRA);

						//	Remember the last integer

						iLastInteger = *pInt;

						//	Next

						pArgs++;
						}

					pPos++;
					iLength--;
					}
				else if (*pPos == 'r')
					{
					double *pVar = (double *)pArgs;

					DWORD dwFlags = (b1000Separator ? FORMAT_THOUSAND_SEPARATOR : 0)
						| (bPadWithZeros ? FORMAT_LEADING_ZERO : 0);

					//	TODO: make a proper encoder for doubles

					CString sNew = strFromDouble(*pVar);

					sOutput.Append(sNew, CString::FLAG_ALLOC_EXTRA);

					//	Remember the last integer (all we care about is whether it
					//	is 1 or not, for pluralization).

					iLastInteger = (*pVar == 1 ? 1 : 0);

					//	Next

					pArgs++;
					pArgs++;

					pPos++;
					iLength--;
					}
				else if (*pPos == 'x' || *pPos == 'X')
					{
					int *pInt = (int *)pArgs;
					char szBuffer[256];
					int iLen = wsprintf(szBuffer, (*pPos == 'x' ? "%x" : "%X"), *pInt);

					if (iMinFieldWidth > 0)
						WritePadding(sOutput, (bPadWithZeros ? '0' : ' '), iMinFieldWidth - iLen);

					sOutput.Append(szBuffer, iLen, CString::FLAG_ALLOC_EXTRA);

					//	Remember the last integer

					iLastInteger = *pInt;

					//	Next

					pArgs++;
					pPos++;
					iLength--;
					}
				else if (*pPos == 'p')
					{
					if (iLastInteger != 1)
						{
						sOutput.Append("s", 1, CString::FLAG_ALLOC_EXTRA);
						}

					pPos++;
					iLength--;
					}
				else if (*pPos == '&')
					{
					//	Get the entity name

					pPos++;
					iLength--;
					const char *pStart = pPos;
					while (iLength > 0 && *pPos != ';')
						{
						pPos++;
						iLength--;
						}

					CString sEntity(pStart, (int)(pPos - pStart));
					CString sResult = CHTML::TranslateStdEntity(sEntity);

					sOutput.Append(sResult);

					if (iLength > 0 && *pPos == ';')
						{
						pPos++;
						iLength--;
						}
					}
				else if (*pPos == '%')
					{
					sOutput.Append("%", 1, CString::FLAG_ALLOC_EXTRA);

					pPos++;
					iLength--;
					}
				}

			pRunStart = pPos;
			iRunLength = 0;
			}
		else
			{
			iRunLength++;
			iLength--;
			pPos++;
			}
		}

	//	Save out the last run

	if (iRunLength > 0)
		{
		sOutput.Append(pRunStart, iRunLength, CString::FLAG_ALLOC_EXTRA);
		}

	//	Resize to the correct length

	return CString(sOutput.GetPointer(), sOutput.GetLength());
	}

//	strPatternSubst
//
//	Substitutes patterns within a string: %Type or %ModifiersType
//
//	where type is one of the following:
//
//	d	Argument is a signed 32-bit integer. The number is substituted
//		Supports the following modifiers:
// 
//		,	A leading comma means use a thousands separator 
// 
//		l	An l means to treat this as a 64-bit int instead
// 
//		0	Pad width with 0s (requires a width value)
// 
//		1-9	Enforce this width
// 
//	r	Argument is a double (64-bit float). The number is substituted.
//
//	p	If the last numeral argument was not 1, then 's' is substituted.
//		This is used to pluralize words in the English language.
//
//	s	Argument is a CString. The string is substituted
//
//	x	Argument is an unsigned 32-bit integer. The hex value is substituted
//		Supports the following modifiers:
// 
//		0	Pad width with 0s (requires a width value)
// 
//		1-9	Enforce this width
//
//	%	Evaluates to a single percent sign
//
//	&	Followed by an XML entity and semicolon
//
CString Kernel::strPatternSubst (CString sLine, ...)

	{
	char *pArgs;
	CString sParsedLine;

	pArgs = (char *) &sLine + sizeof(sLine);
	sParsedLine = strPattern(sLine, (void **)pArgs);
	return sParsedLine;
	}

void WritePadding (CString &sOutput, char chChar, int iLen)
	{
	if (iLen > 0)
		{
		char szBuffer[256];
		char *pBuffer;
		if (iLen <= sizeof(szBuffer))
			pBuffer = szBuffer;
		else
			pBuffer = new char [iLen];

		char *pPos = pBuffer;
		char *pEndPos = pPos + iLen;
		while (pPos < pEndPos)
			*pPos++ = chChar;

		sOutput.Append(pBuffer, iLen, CString::FLAG_ALLOC_EXTRA);

		if (pBuffer != szBuffer)
			delete [] pBuffer;
		}
	}
