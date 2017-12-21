//	LanguageDefs.h
//
//	CLanguage class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

//	Convert from a string to NounPhraseFlags

static TStaticStringTable<TStaticStringEntry<DWORD>, 20> NOUN_FLAG_TABLE = {
	"actual",				nounActual,
	"adjective",			nounAdjective,
	"article",				nounArticle,
	"capitalize",			nounCapitalize,
	"count",				nounCount,
	"countAlways",			nounCountAlways,
	"countOnly",			nounCountOnly,
	"demonstrative",		nounDemonstrative,
	"demonym",				nounDemonym,
	"duplicateModifier",	nounDuplicateModifier,
	"generic",				nounGeneric,
	"installedState",		nounInstalledState,
	"noDeterminer",			nounNoDeterminer,
	"noEvent",				nounNoEvent,
	"noModifiers",			nounNoModifiers,
	"noQuotes",				nounNoQuotes,
	"plural",				nounPlural,
	"short",				nounShort,
	"titleCapitalize",		nounTitleCapitalize,
	"tokenize",				nounTokenize,
	};

static TStaticStringTable<TStaticStringEntry<CLanguage::ENumberFormatTypes>, 4> NUMBER_FORMAT_TABLE = {
	"integer",				CLanguage::numberInteger,
	"power",				CLanguage::numberPower,
	"regenRate",			CLanguage::numberRegenRate,
	"speed",				CLanguage::numberSpeed,
	};

//	List of words to NOT capitalize in title capitalization.

static const char *TITLE_CAP_EXCEPTIONS[] =
	{
	"a",
	"an",
	"and",
	"at",
	"by",
	"for",
	"in",
	"near",
	"not",
	"of",
	"on",
	"or",
	"rce",		//	(abbr.) Registerd Corporate Entity 
	"the",
	"to",
	"under",
	"upon",
	};

static int TITLE_CAP_EXCEPTIONS_COUNT = sizeof(TITLE_CAP_EXCEPTIONS) / sizeof(TITLE_CAP_EXCEPTIONS[0]);

//	Verb forms and their plurals

struct SVerbData
	{
	char *pszKey;							//	The root verb form
	char *pszPlural;						//	Plural version
	};

static TStaticStringTable<SVerbData, 3> VERB_FORM_TABLE = {

//	VERB			PLURAL FORM

	"has",			"have",
	"is",			"are",
	"was",			"were",
	};

//	Item Name Parsing

static TStaticStringTable<SSimpleStringEntry, 10> ITEM_MODIFIER_TABLE = {
	"advanced",
	"ancient",
	"double",
	"heavy",
	"light",
	"massive",
	"medium",
	"quad",
	"super-heavy",
	"ultra-light",
	};
