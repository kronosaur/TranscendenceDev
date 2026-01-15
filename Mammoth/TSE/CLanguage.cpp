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

// ------------------------------------------ Metric Prefix Helpers

struct SMetricPrefix
	{
	int iLog10Min;
	int iLog10Max;
	char cPrefix;		// '?' means to just use float-style exponent notation, ex e100. '\0' means no prefix.
	CString sName;
	};

constexpr int METRIC_PREFIXES_COUNT = 23;
constexpr int METRIC_PREFIXES_MIDPOINT = 11;

static SMetricPrefix METRIC_PREFIXES[METRIC_PREFIXES_COUNT] = {
	//	Min log10		Max	log10	Prefix		Name
		{INT_MIN,	-31,		'?',		CONSTLIT("")},			//use exponent
		{-30,		-28,		'q',		CONSTLIT("Quecto")},	//quecto
		{-27,		-25,		'r',		CONSTLIT("Ronto")},		//ronto
		{-24,		-22,		'y',		CONSTLIT("Yocto")},		//yocto
		{-21,		-19,		'z',		CONSTLIT("Zepto")},		//zepto
		{-18,		-16,		'a',		CONSTLIT("Atto")},		//atto
		{-15,		-13,		'f',		CONSTLIT("Femto")},		//femto
		{-12,		-10,		'p',		CONSTLIT("Pico")},		//pico
		{-9,		-7,			'n',		CONSTLIT("Nano")},		//nano
		{-6,		-4,			'u',		CONSTLIT("Micro")},		//micro
		{-3,		-1,			'm',		CONSTLIT("Milli")},		//milli
		{0,			2,			'\0',		CONSTLIT("")},			//none
		{3,			5,			'k',		CONSTLIT("Kilo")},		//kilo
		{6,			8,			'M',		CONSTLIT("Mega")},		//mega
		{9,			11,			'G',		CONSTLIT("Giga")},		//giga
		{12,		14,			'T',		CONSTLIT("Tera")},		//tera
		{15,		17,			'P',		CONSTLIT("Peta")},		//peta
		{18,		20,			'E',		CONSTLIT("Exa")},		//exa
		{21,		23,			'Z',		CONSTLIT("Zeta")},		//zeta
		{24,		26,			'Y',		CONSTLIT("Yotta")},		//yotta
		{27,		29,			'R',		CONSTLIT("Ronna")},		//ronna
		{30,		32,			'Q',		CONSTLIT("Quetta")},	//quetta
		{33,		INT_MAX,	'?',		CONSTLIT("")},			//use exponent
	};

//	CalcMetricNumber
//
//	This is a helper function for formatting decimal numbers. We start with a
//	float and convert into a whole number between -999 and 999 and a decimal number
//	between 0 and 999, with the total number of significant figures up to 4.
// 
//	Due to UI constraints, and since this is a videogame not a scientific application
//	we truncate the trailing 0s of the decimal to improve readability and
//	more easily fit text in limited UI space.
// 
//	We return the number of times that we had to divide
//	the original number by 1,000. (ie, log(N,1000))
//  If rNumber is an infinity, this is either INT_MAX or INT_MIN and all ret pointers
//		are 0
//  If rNumber is a NaN this is -1 and all ret pointers are 0
//
int CLanguage::CalcMetricNumber(Metric rNumber, int* retiWhole, int* retiDecimal, CString* retsMetricPrefix, CString* retsMetricPrefixName)
	{

	//	Short circuit 0

	if (rNumber == 0.0)
		{
		if (retiWhole)
			*retiWhole = 0;
		if (retiDecimal)
			*retiDecimal = 0;
		if (retsMetricPrefix)
			*retsMetricPrefix = NULL_STR;
		if (retsMetricPrefixName)
			*retsMetricPrefixName = NULL_STR;
		return 0;
		}

	//	Short circuit infinities

	if (IS_AN_INF(rNumber))
		{
		if (retiWhole)
			*retiWhole = 0;
		if (retiDecimal)
			*retiDecimal = 0;
		if (retsMetricPrefix)
			*retsMetricPrefix = NULL_STR;
		if (retsMetricPrefixName)
			*retsMetricPrefixName = NULL_STR;
		return IS_P_INF(rNumber) ? INT_MAX : INT_MIN;
		}

	if (IS_NAN(rNumber))
		{
		if (retiWhole)
			*retiWhole = 0;
		if (retiDecimal)
			*retiDecimal = 0;
		if (retsMetricPrefix)
			*retsMetricPrefix = NULL_STR;
		if (retsMetricPrefixName)
			*retsMetricPrefixName = NULL_STR;
		return -1;
		}

	Metric rAbsNumber = abs(rNumber);
	bool bNegative = rNumber < 0;
	Metric rPower = floor(log10(rAbsNumber));

	//	Convert to the nearest 3

	int iRetThousandsLog = (int)floor(rPower / 3);
	int iThousandsPower = iRetThousandsLog * 3;
	Metric rDenominator = pow(10, iThousandsPower);

	//	Collect the whole and decimal portions up to 4 sig figs

	Metric rScaled = rAbsNumber / rDenominator;
	int iWhole = (int)floor(rScaled);

	if (retiWhole)
		*retiWhole = bNegative ? -1 * iWhole : iWhole;

	if (retiDecimal)
		{
		int iRawDecimal = (int)floor((rScaled * 1000)) % 1000;
		int iWholeSigFigs = (int)floor(log10(iWhole)) + 1;
		int iDecimalSigFigs = 4 - iWholeSigFigs;

		int iDecimal;

		switch (iDecimalSigFigs)
			{
			case 3:
				iDecimal = iRawDecimal;
				break;
			case 2:
				iDecimal = iRawDecimal / 10;
				break;
			case 1:
				iDecimal = iRawDecimal / 100;
				break;
			case 0:
			default:
				iDecimal = 0;
			}

		//	Remove trailing 0s from the decimal if we have them

		if (iDecimal)
			{
			if (iDecimal % 100 == 0)
				iDecimal /= 100;
			else if (iDecimal % 10 == 0)
				iDecimal /= 10;
			}

		*retiDecimal = iDecimal;
		}

	//	Short circuit for no prefix

	if (!iRetThousandsLog)
		{
		if (retsMetricPrefix)
			*retsMetricPrefix = NULL_STR;
		if (retsMetricPrefixName)
			*retsMetricPrefixName = NULL_STR;
		return 0;
		}

	//	Collect our prefixes

	int iMetricIdx = METRIC_PREFIXES_MIDPOINT + iRetThousandsLog;

	iMetricIdx = iMetricIdx < 0 ? 0 : (iMetricIdx >= METRIC_PREFIXES_COUNT ? METRIC_PREFIXES_COUNT - 1 : iMetricIdx);

	SMetricPrefix MetricPfx = METRIC_PREFIXES[iMetricIdx];

	if (retsMetricPrefixName)
		*retsMetricPrefixName = MetricPfx.sName;

	//	check if we just send an exponent back

	if (MetricPfx.cPrefix == '?')
		{
		if (retsMetricPrefix)
			{
			*retsMetricPrefix = strCat(CONSTLIT("e"), strFromInt(iThousandsPower));
			}

		return iRetThousandsLog;
		}

	if (retsMetricPrefix)
		{
		char aPfx[1] = { MetricPfx.cPrefix };
		*retsMetricPrefix = CString(aPfx);
		}

	return iRetThousandsLog;
	}

CString CLanguage::Compose (const CString &sString, const ICCItem *pArgs)

//	Compose
//
//	Replaces the following variables:
//
//		%name%				player name
//		%he%				he or she
//		%his%				his or her (matching case)
//		%hers%				his or hers (matching case)
//		%him%				him or her (matching case)
//		%sir%				sir or ma'am (matching case)
//		%man%				man or woman (matching case)
//		%brother%			brother or sister (matching case)
//		%%					%
//
//		%he:charGender%		Looks up charGender in pArgs to get the gender and
//							then translates the gendered word appropriately.
//
//		%he@char%			Looks up char in pArgs and expects a character info
//							structure (as returned by rpgCharacterGetInfo). Then
//							we use the gender of the character to translate the
//							gendered word.

	{
	//	Prepare output

	CString sOutput;
	int iOutLeft = sString.GetLength() * 2;
	char *pOut = sOutput.GetWritePointer(iOutLeft);

	//	If pArgs is a structure, then we look up variable names in it.

	bool bHasData = (pArgs && pArgs->IsSymbolTable());
	bool bIsRTF = CTextBlock::IsRTFText(sString);

	//	Compose. Loop once for each segment that we need to add

	bool bDone = false;
	bool bVar = false;
	char *pPos = sString.GetASCIIZPointer();
	while (!bDone)
		{
		CString sVar;
		char *pSeg;
		char *pSegEnd;

		if (bVar)
			{
			ASSERT(*pPos == '%');

			//	Parse the variable and associated info

			pPos++;
			char *pStart = pPos;

			SVarInfo VarInfo;
			sVar = ParseVar(pPos, VarInfo, &pPos);

			bVar = false;
			bDone = VarInfo.bDone;

			//	These variables are used to figure out how to translate the variable.

			const SStaticGenderWord *pGenderedWord = NULL;
			ICCItem *pValue = NULL;
			int iArg = 0;

			//	Setup the segment depending on the variable

			if (sVar.IsBlank())
				sVar = CONSTLIT("%");

			//	If this is a character reference, then we go through a different
			//	path.

			else if (VarInfo.bCharacter)
				sVar = ComposeCharacterReference(*g_pUniverse, VarInfo.sParam, sVar, (bHasData ? pArgs : NULL));

			//	Check to see if this is a variable referencing gData.

			else if (bHasData && (pValue = pArgs->GetElement(sVar)) && !pValue->IsNil())
				sVar = pValue->GetStringValue();

			//	Otherwise we look for standard variables

			else if (strEquals(sVar, CONSTLIT("name")))
				sVar = g_pUniverse->GetPlayerName();

			//	Is this a gendered word?

			else if (pGenderedWord = GENDER_WORD_TABLE.GetAt(sVar))
				sVar = ComposeGenderedWordHelper(*g_pUniverse, sVar, VarInfo.sParam, (bHasData ? pArgs : NULL));

			//	If we still haven't found it, then assume this is an index into 
			//	and array of values.

			else if (pArgs 
					&& pArgs->IsList() 
					&& !pArgs->IsSymbolTable()
					&& (iArg = strToInt(sVar, 0)) != 0
					&& Absolute(iArg) + 1 < pArgs->GetCount())
				{
				if (iArg < 0)
					{
					iArg = -iArg;
					VarInfo.bCapitalize = true;
					}

				ICCItem *pArg = pArgs->GetElement(iArg + 1);
				if (pArg)
					sVar = pArg->GetStringValue();
				}

			//	If we have an error, then show it.

			else if (pArgs && pArgs->IsError())
				sVar = pArgs->GetStringValue();

			//	If we could not find a valid var, then we assume a
			//	single % sign.

			else
				{
				sVar = CONSTLIT("%");
				pPos = pStart;
				bDone = (*pPos == '\0');
				bVar = false;
				VarInfo.bCapitalize = false;
				}

			//	Capitalize, if necessary

			if (VarInfo.bCapitalize)
				sVar = strCapitalize(sVar);

			//	Escape, if necessary

			if (bIsRTF)
				sVar = CTextBlock::Escape(sVar);

			//	Setup segment

			pSeg = sVar.GetASCIIZPointer();
			pSegEnd = pSeg + sVar.GetLength();
			}
		else
			{
			//	Skip to the next variable or the end of the string

			pSeg = pPos;
			while (*pPos != '%' && *pPos != '\0')
				pPos++;

			if (*pPos == '\0')
				bDone = true;
			else
				bVar = true;

			pSegEnd = pPos;
			}

		//	Add the next segment

		int iLen = pSegEnd - pSeg;
		if (iLen > 0)
			{
			if (iLen > iOutLeft)
				{
				int iAlloc = sOutput.GetLength();
				int iCurLen = iAlloc - iOutLeft;
				int iNewAlloc = max(iAlloc * 2, iAlloc + iLen);
				pOut = sOutput.GetWritePointer(iNewAlloc);
				pOut += iCurLen;
				iOutLeft = iNewAlloc - iCurLen;
				}

			while (pSeg < pSegEnd)
				*pOut++ = *pSeg++;

			iOutLeft -= iLen;
			}
		}

	//	Done

	int iAlloc = sOutput.GetLength();
	int iCurLen = iAlloc - iOutLeft;
	sOutput.Truncate(iCurLen);
	return sOutput;
	}

CString CLanguage::ComposeCharacterReference (CUniverse &Universe, const CString &sCharacter, const CString &sField, const ICCItem *pData)

//	ComposeCharacterReference
//
//	Translates to a character-based word or phrase. sCharacter is the field in
//	gData that contains character info (as returned by rpgCharacterGetInfo).
//	sField is the FIELD of the character that we want.
//
//	We support the following kinds of fields:
//
//	*	If FIELD is a gendered word, then we lookup the gender of the character
//		and return the appropriate word.
//
//	*	If FIELD is fullName, friendlyName, or formalName, we lookup that field
//		in the character info.
//
//	*	Otherwise, we assume FIELD is a language ID in the source type for the
//		character (sourceType field in character info).

	{
	ICCItem *pCharInfo;

	if (sField.IsBlank())
		return CONSTLIT("[expected a character reference]");

	else if (pData == NULL || (pCharInfo = pData->GetElement(sCharacter)) == NULL || pCharInfo->IsNil())
		return strPatternSubst(CONSTLIT("[character %s not found]"), sCharacter);

	CString sCharID = pCharInfo->GetStringAt(CONSTLIT("id"));
	if (sCharID.IsBlank())
		return strPatternSubst(CONSTLIT("[character %s invalid: no ID]"), sCharacter);

	//	See if we need to translate a gendered word

	const SStaticGenderWord *pGenderedWord = GENDER_WORD_TABLE.GetAt(sField);
	if (pGenderedWord)
		{
		ICCItem *pGender = pCharInfo->GetElement(CONSTLIT("gender"));
		if (pGender == NULL)
			return strPatternSubst(CONSTLIT("[no gender for character %s]"), sCharID);

		return ComposeGenderedWord(sField, ParseGenomeID(pGender->GetStringValue()));
		}

	//	See if we want the name of the character

	else if (strEquals(sField, CONSTLIT("friendlyName"))
			|| strEquals(sField, CONSTLIT("formalName"))
			|| strEquals(sField, CONSTLIT("fullName"))
			|| strEquals(sField, CONSTLIT("titledName")))
		return pCharInfo->GetStringAt(sField);

	//	Otherwise, we get the source type and translate.

	else
		{
		CDesignType *pSourceType = Universe.FindDesignType(pCharInfo->GetIntegerAt(CONSTLIT("sourceType")));
		if (pSourceType == NULL)
			return strPatternSubst(CONSTLIT("[no sourceType for character %s]"), sCharID);

		CString sText;
		if (!pSourceType->TranslateText(sField, pData, &sText))
			return strPatternSubst(CONSTLIT("[cannot translate text ID %s for character %s]"), sField, sCharID);

		return sText;
		}
	}

CString CLanguage::ComposeGenderedWord (const CString &sWord, GenomeTypes iGender)

//	ComposeGenderedWord
//
//	Returns the given gendered word.

	{
	const SStaticGenderWord *pEntry = GENDER_WORD_TABLE.GetAt(sWord);
	if (pEntry == NULL || iGender < 0 || iGender >= genomeCount)
		return sWord;

	return pEntry->pszText[iGender];
	}

CString CLanguage::ComposeGenderedWordHelper (CUniverse &Universe, const CString &sWord, const CString &sField, const ICCItem *pData)

//	ComposeGenderedWordHelper
//
//	Helper for CLanguage::Compose

	{
	//	If we have a field then we expect it to be a variable in gData
	//	with the gender.

	if (!sField.IsBlank())
		{
		ICCItem *pValue;

		if (pData
				&& (pValue = pData->GetElement(sField)) 
				&& !pValue->IsNil())
			{
			return ComposeGenderedWord(sWord, ParseGenomeID(pValue->GetStringValue()));
			}
		else
			{
			if (Universe.InDebugMode())
				return strPatternSubst(CONSTLIT("[%s not found]"), sField);
			else
				return ComposeGenderedWord(sWord, genomeHumanMale);
			}
		}

	//	Otherwise, we use the player's gender

	else
		return ComposeGenderedWord(sWord, Universe.GetPlayerGenome());
	}

CString CLanguage::ComposeHitPointValue (int iHP, const SHPDisplayOptions &Options)

//	ComposeHitPointValue
//
//	Composes a hit point value, depending on options.

	{
	switch (Options.iType)
		{
		case EHPDisplay::HitPoints:
			return strFormatInteger(iHP, -1, FORMAT_THOUSAND_SEPARATOR);

		case EHPDisplay::Percent:
			return strPatternSubst(CONSTLIT("%d%%"), iHP);

		case EHPDisplay::Scaled:
			return strFormatInteger(mathRound((double)iHP / (Options.iReference ? Options.iReference : 1)), -1, FORMAT_THOUSAND_SEPARATOR);
			
		default:
			return NULL_STR;
		}
	}

CString CLanguage::ComposeNounPhrase (const CString &sNoun, int iCount, const CString &sModifier, DWORD dwNounFlags, DWORD dwComposeFlags)

//	ComposeNounPhrase
//
//	Composes a noun phrase based on the appropriate flags

	{
	//	Figure out whether we need to pluralize or not

	bool bPluralize = (dwComposeFlags & nounPlural)
			|| (iCount != 1 
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

	if (NounDesc.bHasQuotes)
		{
		if (dwComposeFlags & nounNoQuotes)
			sNounForm = strProcess(sNounForm, STRPROC_NO_DOUBLE_QUOTES);
		else if (dwComposeFlags & nounEscapeQuotes)
			sNounForm = strProcess(sNounForm, STRPROC_ESCAPE_DOUBLE_QUOTES);
		}

	//	Get the appropriate article

	CString sArticle;
	if (dwComposeFlags & nounNoDeterminer)
		{ }
	else if ((dwComposeFlags & nounArticle)
			|| ((dwComposeFlags & nounCount) && iCount == 1))
		{
		sArticle = NounDesc.sArticle;
		}
	else if (dwComposeFlags & nounDefinitePhrase)
		{
		if (!NounDesc.sArticle.IsBlank())
			sArticle = CONSTLIT("the ");
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
		//	Automagically come up with precision

		case numberReal:
			if (rNumber >= 100.0)
				return strFromInt((int)mathRound(rNumber));
			else if (rNumber >= 1.0)
				return strFromDouble(rNumber, 1);
			else
				return strFromDouble(rNumber, 2);
			break;

		//	We format to up to 4 sig figs for metric

		case numberMetricUnitless:
		case numberMetricFull:
		case numberMetric:
			{
			int iWhole, iDecimal;
			CString sPrefix, sName;
			int iSteps = CalcMetricNumber(rNumber, &iWhole, &iDecimal, &sPrefix, &sName);

			//	Handle 0 NaNs and infinities
			//	These dont need special formatting beyond adding a space at the end
			//  If our caller wants to use units
			// 
			//	iWhole is never 0 except when rNumber is 0, Nan, or infinite

			if (!iWhole)
				{
				if (iFormat == numberMetricUnitless)
					{
					switch (iSteps)
						{
						case 0:
							return CONSTLIT("0");
						case INT_MAX:
							return CONSTLIT("infinity");
						case INT_MIN:
							return CONSTLIT("-infinity");
						default:
							return CONSTLIT("NaN");
						}
					}

				//	Include a space if our caller wants to add units

				else
					{
					switch (iSteps)
						{
						case 0:
							return CONSTLIT("0 ");
						case INT_MAX:
							return CONSTLIT("infinity ");
						case INT_MIN:
							return CONSTLIT("-infinity ");
						default:
							return CONSTLIT("NaN ");
						}
					}
				}

			//	Handle all other cases which need sting formatting

			CString sFStr;

			//	If we have no prefix to display (due to being 1-999 or using exponents)
			//	we just return the number.
			//	In this case sPrefix is just the exponent or blank
			// 
			//  Note:
			//	If we intend to have a unit then we still need to include a trailing space

			if (sName.IsBlank())
				{
				if (iDecimal)
					{
					sFStr = iFormat == numberMetricUnitless ? "%d.%d%s" : "%d.%d%s ";
					return strPatternSubst(sFStr, iWhole, iDecimal, sPrefix);
					}
				else
					{
					sFStr =  iFormat == numberMetricUnitless ? "%d%s" : "%d%s ";
					return strPatternSubst(sFStr, iWhole, sPrefix);
					}
				}

			//	If our caller wants to put a unit after this, we need to have a space
			//	between the number and the prefix

			if (iDecimal)
				{
				sFStr = iFormat == numberMetricUnitless ? CONSTLIT("%d.%d%s") : CONSTLIT("%d.%d %s");
				return strPatternSubst(sFStr, iWhole, iDecimal, iFormat == numberMetricFull ? sName : sPrefix);
				}
			else
				{
				sFStr = iFormat == numberMetricUnitless ? CONSTLIT("%d%s") : CONSTLIT("%d %s");
				return strPatternSubst(sFStr, iWhole, iFormat == numberMetricFull ? sName : sPrefix);
				}
			}

		case numberFireRate:
			{
			if (rNumber <= 0)
				return CONSTLIT("none");

			int iRate10 = mathRound(10.0 * g_TicksPerSecond / rNumber);

			if (iRate10 == 0)
				return CONSTLIT("<0.1 shots/sec");
			else if ((iRate10 % 10) == 0)
				{
				if ((iRate10 / 10) == 1)
					return strPatternSubst(CONSTLIT("%d shot/sec"), iRate10 / 10);
				else
					return strPatternSubst(CONSTLIT("%d shots/sec"), iRate10 / 10);
				}
			else
				return strPatternSubst(CONSTLIT("%d.%d shots/sec"), iRate10 / 10, iRate10 % 10);
			break;
			}

		case numberMass:
			{
			int iKg = mathRound(rNumber);

			if (iKg < 1000)
				return strPatternSubst(CONSTLIT("%d kg"), iKg);
			else
				{
				int iTons = iKg / 1000;
				int iKgExtra = iKg % 1000;

				if (iTons == 1 && iKgExtra == 0)
					return CONSTLIT("1 ton");
				else if (iKgExtra == 0)
					return strPatternSubst(CONSTLIT("%d tons"), iTons);
				else
					return strPatternSubst(CONSTLIT("%d.%d tons"), iTons, iKgExtra / 100);
				}
			break;
			}

		//	For massTons, we need to convert the number to kgs

		case numberMassTons:
			return ComposeNumber(numberMass, rNumber * 1000.0);

		//	For power, we assume the value in kWs and convert to Ws.

		case numberPower:
			return strCat(ComposeNumber(numberMetric, rNumber * 1000.0), CONSTLIT("W"));

		case numberRealTimeTicks:
			{
			int iSeconds = mathRound(rNumber / g_TicksPerSecond);
			if (iSeconds == 0)
				return CONSTLIT("0 seconds");
			else if (iSeconds == 1)
				return CONSTLIT("1 second");
			else if (iSeconds <= 90)
				return strPatternSubst(CONSTLIT("%,d seconds"), iSeconds);
			else
				{
				int iMinutes = mathRound(iSeconds / 60.0);
				if (iMinutes == 1)
					return CONSTLIT("1 minute");
				else
					return strPatternSubst(CONSTLIT("%,d minutes"), iMinutes);
				}
			break;
			}

		case numberRegenRate:
			{
			if (rNumber <= 0.0)
				return CONSTLIT("none");

			int iRate10 = mathRound(g_TicksPerSecond * rNumber / 18.0);

			if (iRate10 == 0)
				return CONSTLIT("<0.1 hp/sec");
			else if ((iRate10 % 10) == 0)
				return strPatternSubst(CONSTLIT("%d hp/sec"), iRate10 / 10);
			else
				return strPatternSubst(CONSTLIT("%d.%d hp/sec"), iRate10 / 10, iRate10 % 10);
			break;
			}

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

bool CLanguage::FindGenderedWord (const CString &sWord, GenomeTypes iGender, CString *retsResult)

//	FindGenderedWord
//
//	Returns TRUE if the given word is a gendered word.

	{
	const SStaticGenderWord *pEntry = GENDER_WORD_TABLE.GetAt(sWord);
	if (pEntry == NULL || iGender < 0 || iGender >= genomeCount)
		return false;

	if (retsResult)
		*retsResult = pEntry->pszText[iGender];

	return true;
	}

ICCItemPtr CLanguage::GetNounFlags (DWORD dwFlags)

//	GetNounFlags
//
//	Returns an array of noun flags.

	{
	ICCItemPtr pResult(ICCItem::List);

	for (int i = 0; i < NOUN_FLAG_TABLE.GetCount(); i++)
		{
		if (NOUN_FLAG_TABLE[i].Value & dwFlags)
			pResult->AppendString(CString(NOUN_FLAG_TABLE.GetKey(i)));
		}

	if (pResult->GetCount() == 0)
		return ICCItemPtr(ICCItem::Nil);

	return pResult;
	}

DWORD CLanguage::LoadNameFlags (const CXMLElement *pDesc)

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

void CLanguage::ParseLabelDesc (const CString &sLabelDesc, CString *retsLabel, CString *retsKey, int *retiKey, TArray<ELabelAttribs> *retSpecial)

//	ParseLabelDesc
//
//	Parses a label descriptor of the following forms:
//
//	Action:				This is a normal label
//	[PageDown] Action:	This is a multi-key accelerator
//	[A]ction:			A is the special key
//	[Enter]:			Treated as a normal label because key is > 1 character
//	*Action:			This is the default action
//	^Action:			This is the cancel action
//	>Action:			This is the next key
//	<Action:			This is the prev key

	{
	char *pPos = sLabelDesc.GetASCIIZPointer();

	//	Parse any special attribute prefixes

	while (*pPos == '*' || *pPos == '^' || *pPos == '>' || *pPos == '<')
		{
		if (retSpecial)
			{
			switch (*pPos)
				{
				case '*':
					retSpecial->Insert(specialDefault);
					break;

				case '^':
					retSpecial->Insert(specialCancel);
					break;

				case '>':
					retSpecial->Insert(specialNextKey);
					break;

				case '<':
					retSpecial->Insert(specialPrevKey);
					break;
				}
			}

		pPos++;
		}

	//	Parse out the label and any accelerator key

	CString sLabel;
	CString sKey;
	int iKey = -1;

	//	See if we have a multi-key accelerator label

	char *pStart = NULL;
	char *pAccelStart = NULL;
	char *pAccelEnd = NULL;
	if (*pPos == '[')
		{
		pStart = pPos;
		while (*pStart != '\0' && *pStart != ']')
			pStart++;

		if (*pStart == ']' && pStart[1] != '\0')
			{
			pAccelEnd = pStart;

			pStart++;
			while (*pStart == ' ')
				pStart++;

			//	Must be more than 1 character long

			pAccelStart = pPos + 1;
			if ((int)(pAccelEnd - pAccelStart) <= 1)
				pStart = NULL;
			}
		else
			pStart = NULL;
		}

	//	If we found a multi-key accelerator label, use that

	if (pStart)
		{
		sLabel = CString(pStart);

		//	Look up this key by name

		CString sKeyName = CString(pAccelStart, (int)(pAccelEnd - pAccelStart));
		DWORD dwKey = CVirtualKeyData::GetKey(sKeyName);
		
		//	If this is a valid key, then we use it as an accelerator 
		//	and special key.

		if (dwKey != CVirtualKeyData::INVALID_VIRT_KEY)
			{
			sKey = CVirtualKeyData::GetKeyLabel(dwKey);

			//	We only support a limited number of keys

			switch (dwKey)
				{
				case VK_DOWN:
				case VK_RIGHT:
					retSpecial->Insert(specialNextKey);
					break;

				case VK_ESCAPE:
					retSpecial->Insert(specialCancel);
					break;

				case VK_LEFT:
				case VK_UP:
					retSpecial->Insert(specialPrevKey);
					break;

				case VK_NEXT:
					retSpecial->Insert(specialPgDnKey);
					break;

				case VK_PRIOR:
					retSpecial->Insert(specialPgUpKey);
					break;

				case VK_RETURN:
					retSpecial->Insert(specialDefault);
					break;
				}
			}

		//	Otherwise, just an accelerator

		else
			sKey = sKeyName;
		}

	//	Otherwise, parse for an embedded label using the bracket syntax.

	else
		{
		pStart = pPos;

		while (*pPos != '\0')
			{
			if (pPos[0] == '[' && pPos[1] != '\0' && pPos[2] == ']')
				{
				if (pStart)
					sLabel.Append(CString(pStart, (int)(pPos - pStart)));

				pPos++;
				if (*pPos == '\0')
					break;

				if (*pPos != ']')
					{
					iKey = sLabel.GetLength();
					sKey = CString(pPos, 1);
					sLabel.Append(sKey);
					pPos++;
					}

				if (*pPos == ']')
					pPos++;

				pStart = pPos;
				continue;
				}
			else
				pPos++;
			}

		if (pStart != pPos)
			sLabel.Append(CString(pStart, (int)(pPos - pStart)));
		}

	//	Done

	if (retsLabel)
		*retsLabel = sLabel;

	if (retsKey)
		*retsKey = sKey;

	if (retiKey)
		*retiKey = iKey;
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

		while (*pPos != '\0' && *pPos != ' ')
			pPos++;

		//	No article?

		if (pPos == pArticleStart)
			{
			pArticleEnd = pPos;

			if (*pPos == ' ')
				pPos++;
			}

		//	If we hit the end of the noun, then we don't have
		//	an article.

		else if (*pPos == '\0')
			{
			pPos = pArticleStart;
			pArticleEnd = pPos;
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

CString CLanguage::ParseVar (char *pPos, SVarInfo &retVarInfo, char **retpPos)

//	ParseVar
//
//	Parses a variable in CLanguage::Compose

	{
	char *pStart = pPos;

	//	If we've got whitespace, then this is definitely not a variable.

	if (strIsWhitespace(pPos))
		{
		if (retpPos) *retpPos = pPos;
		return NULL_STR;
		}

	//	Parse until we find a delimiter

	while (*pPos != '%' && *pPos != ':' && *pPos != '@' && *pPos != '\0')
		pPos++;

	CString sVar = CString(pStart, pPos - pStart);

	//	If we've got a colon, then we take the remainder as a parameter

	retVarInfo.bCharacter = false;
	if (*pPos == ':' || *pPos == '@')
		{
		retVarInfo.bCharacter = (*pPos == '@');
		pPos++;
		char *pParamStart = pPos;
		while (*pPos != '%' && *pPos != '\0')
			pPos++;

		retVarInfo.sParam = CString(pParamStart, pPos - pParamStart);
		}

	//	Skip the closing %

	if (*pPos == '%')
		{
		pPos++;
		retVarInfo.bDone = false;
		}
	else
		retVarInfo.bDone = true;

	//	If the variable is capitalized, then we capitalize the result.

	retVarInfo.bCapitalize = (*sVar.GetASCIIZPointer() >= 'A' && *sVar.GetASCIIZPointer() <= 'Z');

	//	Done

	if (retpPos) *retpPos = pPos;

	return sVar;
	}

bool CLanguage::ValidateTranslation (const CString &sText)

//	ValidateTranslation
//
//	Make sure that we don't have any untranslated fields. Returns TRUE if we're
//	OK.

	{
	const char *pPos = sText.GetASCIIZPointer();
	bool bPercent = false;

	while (*pPos != '\0')
		{
		//	If the last character was a percent, then make sure it is not 
		//	followed by a character.

		if (bPercent)
			{
			if (*pPos == '%' || strIsAlphaNumeric(pPos))
				return false;

			bPercent = false;
			}
		else if (*pPos == '%')
			{
			bPercent = true;
			}

		pPos++;
		}

	return true;
	}
