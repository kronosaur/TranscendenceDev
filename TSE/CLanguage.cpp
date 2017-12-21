//	CLanguage.cpp
//
//	CLanguage class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "LanguageDefs.h"

#define CUSTOM_PLURAL_ATTRIB				CONSTLIT("customPlural")
#define DEFINITE_ARTICLE_ATTRIB				CONSTLIT("definiteArticle")
#define ES_PLURAL_ATTRIB					CONSTLIT("esPlural")
#define FIRST_PLURAL_ATTRIB					CONSTLIT("firstPlural")
#define NO_ARTICLE_ATTRIB					CONSTLIT("noArticle")
#define PERSONAL_NAME_ATTRIB				CONSTLIT("personalName")
#define SECOND_PLURAL_ATTRIB				CONSTLIT("secondPlural")
#define VOWEL_ARTICLE_ATTRIB				CONSTLIT("reverseArticle")

CString CLanguage::ComposeNounPhrase (const CString &sNoun, int iCount, const CString &sModifier, DWORD dwNounFlags, DWORD dwComposeFlags)

//	ComposeNounPhrase
//
//	Composes a noun phrase based on the appropriate flags

	{
	//	Figure out whether we need to pluralize or not

	bool bPluralize = (dwComposeFlags & nounPlural)
			|| (iCount > 1 
				&& ((dwComposeFlags & nounCount) 
					|| (dwComposeFlags & nounCountOnly) 
					|| (dwComposeFlags & nounDemonstrative) 
					|| (dwComposeFlags & nounCountAlways)
					|| (dwComposeFlags & nounNoDeterminer))
				&& !(dwComposeFlags & nounArticle));

	//	Get the proper noun form

	SNounDesc NounDesc;
	CString sNounForm = ParseNounForm(sNoun, sModifier, dwNounFlags, bPluralize, (dwComposeFlags & nounShort) != 0, &NounDesc);

	//	If we need to strip quotes, do it now

	if ((dwComposeFlags & nounNoQuotes) && NounDesc.bHasQuotes)
		sNounForm = strProcess(sNounForm, STRPROC_NO_DOUBLE_QUOTES);

	//	Get the appropriate article

	CString sArticle;
	if (dwComposeFlags & nounNoDeterminer)
		{ }
	else if ((dwComposeFlags & nounArticle)
			|| ((dwComposeFlags & nounCount) && iCount == 1))
		{
		sArticle = NounDesc.sArticle;
		}
	else if (dwComposeFlags & nounDemonstrative)
		{
		if (NounDesc.sArticle.IsBlank() || *NounDesc.sArticle.GetPointer() == 't')
			sArticle = NounDesc.sArticle;
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
		sNounPhrase = strConvertToToken(sNounPhrase, true);

	if (dwComposeFlags & nounTitleCapitalize)
		return strTitleCapitalize(sNounPhrase, TITLE_CAP_EXCEPTIONS, TITLE_CAP_EXCEPTIONS_COUNT);
	else if (dwComposeFlags & nounCapitalize)
		return strCapitalize(sNounPhrase);
	else
		return sNounPhrase;
	}

CString CLanguage::ComposeNumber (ENumberFormatTypes iFormat, int iNumber)

//	ComposeNumber
//
//	Converts to a string of the appropriate format.

	{
	switch (iFormat)
		{
		case numberInteger:
			return strFormatInteger(iNumber, -1, FORMAT_THOUSAND_SEPARATOR);

		case numberSpeed:
			return strPatternSubst(CONSTLIT(".%02dc"), iNumber);

		//	Defaults to double

		default:
			return ComposeNumber(iFormat, (Metric)iNumber);
		}
	}

CString CLanguage::ComposeNumber (ENumberFormatTypes iFormat, Metric rNumber)

//	ComposeNumber
//
//	Converts to a string of the appropriate format.

	{
	switch (iFormat)
		{
		//	For power, we assume the value in in KWs.

		case numberPower:
			if (rNumber < 9950.0)
				return strPatternSubst(CONSTLIT("%s MW"), strFromDouble(rNumber / 1000.0, 1));
			else if (rNumber < 999500.0)
				return strPatternSubst(CONSTLIT("%d MW"), mathRound(rNumber / 1000.0));
			else if (rNumber < 9950000.0)
				return strPatternSubst(CONSTLIT("%s GW"), strFromDouble(rNumber / 1000000.0, 1));
			else
				return strPatternSubst(CONSTLIT("%d GW"), mathRound(rNumber / 1000000.0));
			break;

		case numberRegenRate:
			if (rNumber <= 0.0)
				return CONSTLIT("none");
			else
				{
				int iRate10 = mathRound(g_TicksPerSecond * rNumber / 18.0);

				if (iRate10 == 0)
					return CONSTLIT("<0.1 hp/sec");
				else if ((iRate10 % 10) == 0)
					return strPatternSubst(CONSTLIT("%d hp/sec"), iRate10 / 10);
				else
					return strPatternSubst(CONSTLIT("%d.%d hp/sec"), iRate10 / 10, iRate10 % 10);
				}
			break;

		case numberSpeed:
			return ComposeNumber(iFormat, (int)mathRound(100.0 * rNumber / LIGHT_SPEED));

		//	Defaults to integer

		default:
			return strFormatInteger((int)mathRound(rNumber), -1, FORMAT_THOUSAND_SEPARATOR);
		}
	}

CString CLanguage::ComposeNumber (ENumberFormatTypes iFormat, ICCItem *pNumber)

//	ComposeNumber
//
//	Converst to a string of the appropriate format.

	{
	if (pNumber->IsDouble())
		return ComposeNumber(iFormat, pNumber->GetDoubleValue());
	else
		return ComposeNumber(iFormat, pNumber->GetIntegerValue());
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

DWORD CLanguage::LoadNameFlags (CXMLElement *pDesc)

//	LoadNameFlags
//
//	Returns flags word with NounFlags

	{
	DWORD dwFlags = 0;

	if (pDesc->GetAttributeBool(DEFINITE_ARTICLE_ATTRIB))
		dwFlags |= nounDefiniteArticle;
	if (pDesc->GetAttributeBool(FIRST_PLURAL_ATTRIB))
		dwFlags |= nounFirstPlural;
	if (pDesc->GetAttributeBool(ES_PLURAL_ATTRIB))
		dwFlags |= nounPluralES;
	if (pDesc->GetAttributeBool(CUSTOM_PLURAL_ATTRIB))
		dwFlags |= nounCustomPlural;
	if (pDesc->GetAttributeBool(SECOND_PLURAL_ATTRIB))
		dwFlags |= nounSecondPlural;
	if (pDesc->GetAttributeBool(VOWEL_ARTICLE_ATTRIB))
		dwFlags |= nounVowelArticle;
	if (pDesc->GetAttributeBool(NO_ARTICLE_ATTRIB))
		dwFlags |= nounNoArticle;
	if (pDesc->GetAttributeBool(PERSONAL_NAME_ATTRIB))
		dwFlags |= nounPersonalName;

	return dwFlags;
	}

void CLanguage::ParseItemName (const CString &sName, CString *retsRoot, CString *retsModifiers)

//	ParseItemName
//
//	Parses an item name like:
//
//	light reactive armor
//
//	And splits it into a root name and a modifier:
//
//	root = "reactive armor"
//	modifier = "light"

	{
	char *pPos = sName.GetASCIIZPointer();
	char *pPosStart = pPos;
	char *pPosEnd = pPos + sName.GetLength();

	char *pModifierEnd = NULL;
	char *pStart = pPos;
	while (pPos < pPosEnd)
		{
		//	If this is whitespace, then we've reached then end of a word.

		if (strIsWhitespace(pPos))
			{
			CString sWord(pStart, (int)(pPos - pStart));

			//	If this word is a known modifier, then we remember the fact that
			//	we've got at least one modifier.

			if (ITEM_MODIFIER_TABLE.GetAt(sWord))
				{
				pModifierEnd = pPos;

				//	Skip until the beginning of the next word.

				while (pPos < pPosEnd && strIsWhitespace(pPos))
					pPos++;

				//	Continue looping.

				pStart = pPos;
				}

			//	Otherwise, we're done.

			else
				{
				break;
				}
			}

		//	Otherwise, next char

		else
			pPos++;
		}

	//	If we have a modifier, then add it.

	if (pModifierEnd && retsModifiers)
		*retsModifiers = CString(pPosStart, (int)(pModifierEnd - pPosStart));

	//	The rest is the root

	if (retsRoot)
		*retsRoot = CString(pStart, (int)(pPosEnd - pStart));
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

CString CLanguage::ParseNounForm (const CString &sNoun, const CString &sModifier, DWORD dwNounFlags, bool bPluralize, bool bShortName, SNounDesc *retDesc)

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
	if (retDesc)
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
					retDesc->sArticle = CONSTLIT("an ");
					break;

				default:
					retDesc->sArticle = CONSTLIT("a ");
					break;
				}

			bNeedArticle = false;
			}

		//	Otherwise, if we've got the article above, use that.

		else if (pArticleStart)
			{
			retDesc->sArticle = CString(pArticleStart, (int)(pArticleEnd - pArticleStart));

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
	bool bHasQuotes = false;
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
				{
				//	See if the noun has embedded quotes

				if (*pPos == '\"')
					bHasQuotes = true;

				//	Add to the result

				*pDest++ = *pPos;
				}
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

	//	Return noun descriptor, if necessary

	if (retDesc)
		{
		//	If we still need an article, compute it now

		if (bNeedArticle)
			{
			if (dwNounFlags & nounNoArticle)
				retDesc->sArticle = NULL_STR;
			else if (dwNounFlags & nounDefiniteArticle)
				retDesc->sArticle = CONSTLIT("the ");
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
							retDesc->sArticle = CONSTLIT("a ");
						else
							retDesc->sArticle = CONSTLIT("an ");
						break;
						}

					default:
						{
						if (dwNounFlags & nounVowelArticle)
							retDesc->sArticle = CONSTLIT("an ");
						else
							retDesc->sArticle = CONSTLIT("a ");
						break;
						}
					}
				}
			}

		//	Other flags

		retDesc->bHasQuotes = bHasQuotes;
		}

	return sDest;
	}

CLanguage::ENumberFormatTypes CLanguage::ParseNumberFormat (const CString &sValue)

//	ParseNumberFormat
//
//	Parses a number format string.

	{
	if (sValue.IsBlank())
		return numberNone;

	const TStaticStringEntry<ENumberFormatTypes> *pEntry = NUMBER_FORMAT_TABLE.GetAt(sValue);
	if (pEntry == NULL)
		return numberError;

	return pEntry->Value;
	}
