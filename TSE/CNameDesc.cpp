//	CNameDesc.cpp
//
//	CNameDesc class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define FIELD_NAME								CONSTLIT("name")

#define TAG_NAMES								CONSTLIT("Names")

CNameDesc::CNameDesc (void) :
		m_dwConstantNameFlags(0),
		m_dwNameFlags(0),
		m_iNamesGenerated(0)

//	CNameDesc constructor

	{
	}

ALERROR CNameDesc::InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	InitFromXML
//
//	Initializes the name descriptor.

	{
	m_dwNameFlags = CLanguage::LoadNameFlags(pDesc);

	//	Parse the list into separate names.

	CString sNameList = pDesc->GetContentText(0);
	strDelimitEx(sNameList, ';', DELIMIT_COMMA | DELIMIT_TRIM_WHITESPACE, 0, &m_Names);

	//	If we don't have a constant name, use the first name in the list as
	//	constant/generic name.

	if (m_sConstantName.IsBlank() && m_Names.GetCount() > 0)
		{
		m_sConstantName = m_Names[0];
		m_dwConstantNameFlags = m_dwNameFlags;
		}

	//	Randomize order.

	Reinit();
	return NOERROR;
	}

ALERROR CNameDesc::InitFromXMLRoot (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	InitFromXMLRoot
//
//	Initializes the name descriptor from a parent element of <Names>.

	{
	ALERROR error;

	//	Get the constant name

	m_sConstantName = pDesc->GetAttribute(FIELD_NAME);
	m_dwConstantNameFlags = CLanguage::LoadNameFlags(pDesc);

	//	Load list of random names. These names may have replaceable parameters.

	CXMLElement *pNames = pDesc->GetContentElementByTag(TAG_NAMES);
	if (pNames)
		{
		if (error = InitFromXML(Ctx, pNames))
			return error;
		}

	return NOERROR;
	}

CString CNameDesc::GenerateName (TSortMap<CString, CString> *pParams, DWORD *retdwNameFlags) const

//	GenerateName
//
//	Generates a name.

	{
	if (m_Names.GetCount() == 0)
		{
		if (retdwNameFlags)
			*retdwNameFlags = m_dwConstantNameFlags;

		return m_sConstantName;
		}

	//	Pick the next name in the list.

	const CString &sName = m_Names[m_iNamesGenerated % m_Names.GetCount()];

	//	Advance to the next name (for next time)

	m_iNamesGenerated++;

	//	Done

	if (retdwNameFlags)
		*retdwNameFlags = m_dwNameFlags;

	return GenerateName(sName, pParams);
	}

CString CNameDesc::GenerateName (const CString &sName, TSortMap<CString, CString> *pParams) const

//	GenerateName
//
//	Generate a name by substituting all parameters.

	{
	//	Substitute string

	char szResult[1024];
	char *pDest = szResult;
	char *pDestEnd = pDest + 1023;
	char *pPos = sName.GetASCIIZPointer();

	while (*pPos && pDest < pDestEnd)
		{
		switch (*pPos)
			{
			case '%':
				{
				pPos++;

				char *pStart = pPos;

				//	A double percent character is an escape code.

				if (*pPos == '%' || *pPos == '\0')
					{
					*pDest++ = *pPos++;
					break;
					}

				//	Previous versions used a single character as a replaceable 
				//	parameter.

				pPos++;

				//	If we've got another percent, then it means that there is 
				//	another parameter right after this one.
				//
				//	Also, if we hit the end of the string or if we've got whitespace 
				//	or a delimiter of some sort, then we've hit the end.

				CString sToken;
				if (*pPos == '\0' || *pPos == '%' || *pPos == ' ' || *pPos == '-' 
						|| *pPos == ':'	|| *pPos == '/' || *pPos == '(' || *pPos == ')')
					{
					if (*pStart == 's')
						sToken = CONSTLIT("systemName");
					else
						sToken = CString(pStart, 1);
					}

				//	Otherwise, we assume this is a new style parameter and we 
				//	search for the terminating %.

				else
					{
					while (*pPos != '\0' && *pPos != '%')
						pPos++;

					sToken = CString(pStart, (int)(pPos - pStart));

					if (*pPos == '%')
						pPos++;
					}

				//	Figure out what we're going to replace this token with.

				if (sToken.GetLength() == 1)
					{
					switch (*sToken.GetASCIIZPointer())
						{
						case '0':
							*pDest++ = '0' + mathRandom(0, 9);
							break;

						case '1':
							*pDest++ = '1' + mathRandom(0, 8);
							break;

						case 'A':
							*pDest++ = 'A' + mathRandom(0, 25);
							break;

						case 'a':
							*pDest++ = 'a' + mathRandom(0, 25);
							break;
						}
					}
				else
					{
					CString *pParam = (pParams ? pParams->GetAt(sToken) : NULL);
					if (pParam)
						{
						char *pSrc = pParam->GetASCIIZPointer();
						while (*pSrc && pDest < pDestEnd)
							*pDest++ = *pSrc++;
						}

					//	If this is not a variable in the parameter block, then
					//	interpret the token as a series of alphanumeric codes
					//	(as above).

					else
						{
						char *pSrc = sToken.GetASCIIZPointer();
						while (*pSrc && pDest < pDestEnd)
							{
							switch (*pSrc)
								{
								case '0':
									*pDest++ = '0' + mathRandom(0, 9);
									break;

								case '1':
									*pDest++ = '1' + mathRandom(0, 8);
									break;

								case 'A':
									*pDest++ = 'A' + mathRandom(0, 25);
									break;

								case 'a':
									*pDest++ = 'a' + mathRandom(0, 25);
									break;
								}

							pSrc++;
							}
						}
					}

				break;
				}

			default:
				*pDest++ = *pPos++;
			}
		}

	//	Done

	*pDest = '\0';
	return CString(szResult);
	}

void CNameDesc::Reinit (void)

//	Reinit
//
//	Reinitialize in preparation for a new game.

	{
	m_Names.Shuffle();
	m_iNamesGenerated = 0;
	}
