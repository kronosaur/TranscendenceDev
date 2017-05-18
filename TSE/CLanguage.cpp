//	CLanguage.cpp
//
//	CLanguage class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "LanguageDefs.h"

CString CLanguage::ComposeNounPhrase (const CString &sNoun, int iCount, const CString &sModifier, DWORD dwNounFlags, DWORD dwComposeFlags)

//	ComposeNounPhrase
//
//	Composes a noun phrase based on the appropriate flags

	{
	//	Figure out whether we need to pluralize or not

	bool bPluralize = (dwComposeFlags & nounPlural)
			|| (iCount > 1 
				&& ((dwComposeFlags & nounCount) || (dwComposeFlags & nounCountOnly) || (dwComposeFlags & nounDemonstrative))
				&& !(dwComposeFlags & nounArticle));

	//	Get the proper noun form

	CString sDefaultArticle;
	CString sNounForm = ParseNounForm(sNoun, sModifier, dwNounFlags, bPluralize, (dwComposeFlags & nounShort) != 0, &sDefaultArticle);

	//	Get the appropriate article

	CString sArticle;
	if ((dwComposeFlags & nounArticle)
			|| ((dwComposeFlags & nounCount) && iCount == 1))
		{
		sArticle = sDefaultArticle;
		}
	else if (dwComposeFlags & nounDemonstrative)
		{
		if (sDefaultArticle.IsBlank() || *sDefaultArticle.GetPointer() == 't')
			sArticle = sDefaultArticle;
		else
			{
			if (iCount > 1)
				sArticle = CONSTLIT("these ");
			else
				sArticle = CONSTLIT("this ");
			}
		}
	else if (iCount > 1
			&& ((dwComposeFlags & nounCount) || (dwComposeFlags & nounCountOnly)))
		sArticle = strPatternSubst(CONSTLIT("%d "), iCount);

	else if (dwComposeFlags & nounCountAlways)
		sArticle = strPatternSubst(CONSTLIT("%d "), iCount);

	//	Compose

	CString sNounPhrase = strPatternSubst(CONSTLIT("%s%s%s"), 
				sArticle, 
				sModifier, 
				sNounForm);

	if (dwComposeFlags & nounTokenize)
		sNounPhrase = strConvertToToken(sNounPhrase);

	if (dwComposeFlags & nounTitleCapitalize)
		return strTitleCapitalize(sNounPhrase, TITLE_CAP_EXCEPTIONS, TITLE_CAP_EXCEPTIONS_COUNT);
	else if (dwComposeFlags & nounCapitalize)
		return strCapitalize(sNounPhrase);
	else
		return sNounPhrase;
	}

CString CLanguage::ComposeVerb (const CString &sVerb, DWORD dwVerbFlags)

//	ComposeVerb
//
//	Generates the proper verb form based on the given flags.

	{
	const SVerbData *pVerbData = VERB_FORM_TABLE.GetAt(sVerb);
	if (pVerbData == NULL)
		return sVerb;

	if (dwVerbFlags & verbPluralize)
		return CString(pVerbData->pszPlural, -1, TRUE);

	return sVerb;
	}

DWORD CLanguage::ParseNounFlags (const CString &sValue)

//	ParseNounFlags
//
//	Parse the value as a noun flag and returns it. If not found, we return 0.
//	sValue can also be a set of flags, separated by spaces or commas.

	{
	DWORD dwFlags = 0;

	char *pPos = sValue.GetASCIIZPointer();
	while (*pPos != '\0')
		{
		//	Find the end of this flag

		char *pStart = pPos;
		while (*pPos != '\0' && *pPos != ' ' && *pPos != ',' && *pPos != ';')
			pPos++;

		//	Add the flag

		const TStaticStringEntry<DWORD> *pEntry = NOUN_FLAG_TABLE.GetAt(pStart, pPos);
		if (pEntry)
			dwFlags |= pEntry->Value;

		//	Next

		while (*pPos == ' ' || *pPos == ',' || *pPos == ';')
			pPos++;
		}

	return dwFlags;
	}

CString CLanguage::ParseNounForm (const CString &sNoun, const CString &sModifier, DWORD dwNounFlags, bool bPluralize, bool bShortName, CString *retsArticle)

//	ParseNounForm
//
//	Parses a string of the form:
//
//	abc(s)
//	:an abcdef(s)
//	: abcdef(s)
//	[abc]def(s)
//	[abc]def| [hij]klm
//	:the [abc]def| [hij]klm
//
//	Using the noun flags and the required result, it parses out the various component
//	of the noun (plural form, short name) and returns the required noun

	{
	char *pPos = sNoun.GetASCIIZPointer();

	//	Parse the article, if we have one.

	char *pArticleStart = NULL;
	char *pArticleEnd = NULL;
	if (*pPos == ':')
		{
		pPos++;
		pArticleStart = pPos;

		//	Skip to the end of the article

		while (pPos != '\0' && *pPos != ' ')
			pPos++;

		//	No article?

		if (pPos == pArticleStart)
			{
			pArticleEnd = pPos;

			if (*pPos == ' ')
				pPos++;
			}

		//	Otherwise, include a trailing space in the article

		else
			{
			if (*pPos == ' ')
				pPos++;

			pArticleEnd = pPos;
			}
		}

	//	See if we know enough to compute the article now

	bool bNeedArticle = true;
	if (retsArticle)
		{
		//	If we have a modifier, then we always use that to calculate the article.
		//	(And we assume it follows regular rules.)

		if (!sModifier.IsBlank())
			{
			switch (*sModifier.GetASCIIZPointer())
				{
				case 'A':
				case 'a':
				case 'E':
				case 'e':
				case 'I':
				case 'i':
				case 'O':
				case 'o':
				case 'U':
				case 'u':
					*retsArticle = CONSTLIT("an ");
					break;

				default:
					*retsArticle = CONSTLIT("a ");
					break;
				}

			bNeedArticle = false;
			}

		//	Otherwise, if we've got the article above, use that.

		else if (pArticleStart)
			{
			if (retsArticle)
				*retsArticle = CString(pArticleStart, (int)(pArticleEnd - pArticleStart));

			bNeedArticle = false;
			}
		}

	//	Parse the noun

	CString sDest;
	char *pDest = sDest.GetWritePointer(sNoun.GetLength() + 10);

	bool bDestIsSingular = true;
	bool bInPluralSect = false;
	bool bInLongNameSect = false;
	bool bSkipWhitespace = true;
	bool bStartOfWord = true;
    bool bHasLongForm = false;
	int iWord = 1;

	while (*pPos != '\0')
		{
		//	Skip whitespace until we find non-whitespace

		if (bSkipWhitespace)
			{
			if (*pPos == ' ')
				{
				pPos++;
				continue;
				}
			else
				bSkipWhitespace = false;
			}

		//	Begin and end plural addition. Everything inside parentheses
		//	is added only if plural

		if (*pPos == '(' && !bStartOfWord)
			{
			bInPluralSect = true;

			//	If we have a paren, then it means that we can form the
			//	plural version. We don't need any additional auto pluralization

			if (bPluralize)
				bDestIsSingular = false;
			}
		else if (*pPos == ')' && bInPluralSect)
			bInPluralSect = false;

		//	Begin and end long name section. Everything inside
		//	brackets appears only for long names

		else if (*pPos == '[')
            {
			bInLongNameSect = true;
            bHasLongForm = true;
            }
		else if (*pPos == ']')
			bInLongNameSect = false;

		//	Escape code

		else if (*pPos == '\\')
			{
			pPos++;	if (*pPos == '\0') break;

			//	Deal with escape codes

			if (*pPos == 'x' || *pPos == 'X')
				{
				pPos++;	if (*pPos == '\0') break;
				int iFirstDigit = strGetHexDigit(pPos);

				pPos++;	if (*pPos == '\0') break;
				int iSecondDigit = strGetHexDigit(pPos);

				if ((!bInLongNameSect || !bShortName)
						&& (!bInPluralSect || bPluralize))
					*pDest++ = (char)(16 * iFirstDigit + iSecondDigit);
				}

			//	Add, but not if we're in the long names section
			//	and we only want a short name

			else if ((!bInLongNameSect || !bShortName)
					&& (!bInPluralSect || bPluralize))
				*pDest++ = *pPos;
			}

		//	A semi-colon or vertical-bar means that we have two
		//	name: singular followed by plural

		else if (*pPos == ';' || *pPos == '|')
			{
			//	If we don't need the plural form, then we've reached
			//	the end of the singular form, so we're done

			if (!bPluralize)
				break;

			//	Reset the destination so we start capturing from
			//	the plural section

			pDest = sDest.GetASCIIZPointer();
			bDestIsSingular = false;
			bSkipWhitespace = true;
			}
		else
			{
			//	If we've reached the end of a word, see if we need
			//	to add a plural

			if (*pPos == ' ')
				{
				if (bPluralize && bDestIsSingular && !bShortName)
					{
					if ((iWord == 1 && (dwNounFlags & nounFirstPlural))
							|| (iWord == 2 && (dwNounFlags & nounSecondPlural)))
						{
						if (dwNounFlags & nounPluralES)
							{
							*pDest++ = 'e';
							*pDest++ = 's';
							}
						else
							*pDest++ = 's';

						bDestIsSingular = false;
						}
					}

				iWord++;
				bSkipWhitespace = true;
				bStartOfWord = true;
				}
			else
				bStartOfWord = false;

			if ((!bInLongNameSect || !bShortName)
					&& (!bInPluralSect || bPluralize))
				*pDest++ = *pPos;
			}

		pPos++;
		}

	//	Add plural if necessary (short name implies plural because
	//	a short name is always a quantifiable item, e.g., a *mass of* concrete)

	if (bPluralize && bDestIsSingular && (!bHasLongForm || (dwNounFlags & nounPluralizeLongForm)))
		{
		if (dwNounFlags & nounPluralES)
			{
			*pDest++ = 'e';
			*pDest++ = 's';
			}
		else
			*pDest++ = 's';
		}

	//	Done with name

	*pDest++ = '\0';
	sDest.Truncate(pDest - sDest.GetASCIIZPointer() - 1);

	//	If we still need an article, compute it now

	if (bNeedArticle && retsArticle)
		{
		if (dwNounFlags & nounNoArticle)
			*retsArticle = NULL_STR;
		else if (dwNounFlags & nounDefiniteArticle)
			*retsArticle = CONSTLIT("the ");
		else
			{
			switch (*sDest.GetASCIIZPointer())
				{
				case 'A':
				case 'a':
				case 'E':
				case 'e':
				case 'I':
				case 'i':
				case 'O':
				case 'o':
				case 'U':
				case 'u':
					{
					if (dwNounFlags & nounVowelArticle)
						*retsArticle = CONSTLIT("a ");
					else
						*retsArticle = CONSTLIT("an ");
					break;
					}

				default:
					{
					if (dwNounFlags & nounVowelArticle)
						*retsArticle = CONSTLIT("an ");
					else
						*retsArticle = CONSTLIT("a ");
					break;
					}
				}
			}
		}

	return sDest;
	}

