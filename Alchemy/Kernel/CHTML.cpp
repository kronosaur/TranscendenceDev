//	CHTML.cpp
//
//	CHTML class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Internets.h"

// References:
//
//	Windows western encoding: https://en.wikipedia.org/wiki/Windows-1252
//	XML built-in entities: https://en.wikipedia.org/wiki/List_of_XML_and_HTML_character_entity_references

static std::unordered_map<std::string, std::string> STD_ENTITY_TABLE ={
	{"quot",		"\""},	//x22
	{"amp",			"&"},		//x26
	{"apos",		"\'"},	//x27
	{"lt",			"<"},		//x2C
	{"gt",			">"},		//x2E
	{"euro",		"\x80"},	//Unicode x20AC
	//x81 is a control code
	{"lsquor",		"\x82"},	//Unicode x201A
	{"fnof",		"\x83"},	//Unicode x0192
	{"ldquor",		"\x84"},	//Unicode x201E
	{"hellip",		"\x85"},	//Unicode x2026
	{"dagger",		"\x86"},	//Unicode x2020
	{"ddagger",		"\x87"},	//Unicode x2021
	{"circ",		"\x88"},	//Unicode x02C6
	{"permil",		"\x89"},	//Unicode x2030
	{"Scaron",		"\x8A"},	//Unicode x0160
	{"lsaquo",		"\x8B"},	//Unicode x2039
	{"OElig",		"\x8C"},	//Unicode x0152
	//x8D is a control code
	{"Zcaron",		"\x8E"},	//Unicode x017D
	//x8F is a control code

	//x90 is a control code
	{"lsquo",		"\x91"},	//Unicode x2018
	{"rsquo",		"\x92"},	//Unicode x2019
	{"ldquo",		"\x93"},	//Unicode x201C
	{"rdquo",		"\x94"},	//Unicode x201D
	{"bull",		"\x95"},	//Unicode x2022
	{"ndash",		"\x96"},	//Unicode x2013
	{"mdash",		"\x97"},	//Unicode x2013
	{"tilde",		"\x98"},	//Unicode x02DC
	{"trade",		"\x99"},	//Unicode x2122
	{"scaron",		"\x9A"},	//Unicode x0161
	{"rsaquo",		"\x9B"},	//Unicode x203A
	{"oelig",		"\x9C"},	//Unicode x0153
	//x9D is a control code
	{"zcaron",		"\x9E"},	//Unicode 017E
	{"Yuml",		"\x9F"},	//Unicode 0178

	{"nbsp",		"\xA0"},
	{"iexcl",		"\xA1"},
	{"cent",		"\xA2"},
	{"pound",		"\xA3"},
	{"curren",		"\xA4"},
	{"yen",			"\xA5"},
	{"brvbar",		"\xA6"},
	{"sect",		"\xA7"},
	{"DoubleDot",	"\xA8"},	//Alias of uml
	{"uml",			"\xA8"},
	{"copy",		"\xA9"},
	{"ordf",		"\xAA"},
	{"laquo",		"\xAB"},
	{"not",			"\xAC"},
	{"shy",			"\xAD"},
	{"reg",			"\xAE"},
	{"macr",		"\xAF"},

	{"deg",			"\xB0"},
	{"plusmn",		"\xB1"},
	{"sup2",		"\xB2"},
	{"sup3",		"\xB3"},
	{"acute",		"\xB4"},
	{"micro",		"\xB5"},
	{"para",		"\xB6"},
	{"middot",		"\xB7"},	//Alias of centerdot
	{"centerdot",	"\xB7"},
	{"cedil",		"\xB8"},
	{"sup1",		"\xB9"},
	{"ordm",		"\xBA"},
	{"raquo",		"\xBB"},
	{"frac14",		"\xBC"},
	{"frac12",		"\xBD"},
	{"half",		"\xBD"},	//Alias of frac12
	{"frac34",		"\xBE"},
	{"iquest",		"\xBF"},

	{"Agrave",		"\xC0"},
	{"Aacute",		"\xC1"},
	{"Acirc",		"\xC2"},
	{"Atilde",		"\xC3"},
	{"Auml",		"\xC4"},
	{"Aring",		"\xC5"},
	{"AElig",		"\xC6"},
	{"CCedil",		"\xC7"},
	{"Egrave",		"\xC8"},
	{"Eacute",		"\xC9"},
	{"Ecirc",		"\xCA"},
	{"Euml",		"\xCB"},
	{"Igrave",		"\xCC"},
	{"Iacute",		"\xCD"},
	{"Icirc",		"\xCE"},
	{"Iuml",		"\xCF"},

	{"ETH",			"\xD0"},
	{"Ntilde",		"\xD1"},
	{"Ograve",		"\xD2"},
	{"Oacute",		"\xD3"},
	{"Ocirc",		"\xD4"},
	{"Otilde",		"\xD5"},
	{"Ouml",		"\xD6"},
	{"times",		"\xD7"},
	{"Oslash",		"\xD8"},
	{"Ugrave",		"\xD9"},
	{"Uacute",		"\xDA"},
	{"Ucirc",		"\xDB"},
	{"Uuml",		"\xDC"},
	{"Yacute",		"\xDD"},
	{"THORN",		"\xDE"},
	{"szlig",		"\xDF"},

	{"agrave",		"\xE0"},
	{"aacute",		"\xE1"},
	{"acirc",		"\xE2"},
	{"atilde",		"\xE3"},
	{"auml",		"\xE4"},
	{"aring",		"\xE5"},
	{"aelig",		"\xE6"},
	{"ccedil",		"\xE7"},
	{"egrave",		"\xE8"},
	{"eacute",		"\xE9"},
	{"ecirc",		"\xEA"},
	{"euml",		"\xEB"},
	{"igrave",		"\xEC"},
	{"iacute",		"\xED"},
	{"icric",		"\xEE"},
	{"iuml",		"\xEF"},

	{"eth",			"\xF0"},
	{"ntilde",		"\xF1"},
	{"ograve",		"\xF2"},
	{"oacute",		"\xF3"},
	{"ocirc",		"\xF4"},
	{"otilde",		"\xF5"},
	{"ouml",		"\xF6"},
	{"div",			"\xF7"},
	{"oslash",		"\xF8"},
	{"ugrave",		"\xF9"},
	{"uacute",		"\xFA"},
	{"ucirc",		"\xFB"},
	{"uuml",		"\xFC"},
	{"yacute",		"\xFD"},
	{"thorn",		"\xFE"},
	{"yuml",		"\xFF"},
	};

bool CHTML::FindStdEntity (const CString &sEntity, CString *retsValue)

//	FindStdEntity
//
//	Attempts to translate the standard HTML entity and returns TRUE if 
//	successful.

	{
	CString sValue;

	if (strEquals(CONSTLIT("gt"), sEntity))
		int a = 0;

	if (sValue = CString(STD_ENTITY_TABLE[sEntity.GetASCIIZPointer()].c_str()))
		{
		if (retsValue)
			*retsValue = sValue;
		return true;
		}
	else
		return false;
	}

CString CHTML::TranslateStdEntity (const CString &sEntity)

//	TranslateStdEntity
//
//	Translates the given entity and return the result. If the standard entity is
//	not found, we return the entity itself.

	{
	CString sValue;

	if (strEquals(CONSTLIT("gt"), sEntity))
		int a = 0;

	if (sValue = CString(STD_ENTITY_TABLE[sEntity.GetASCIIZPointer()].c_str()))
		return sValue;
	else
		return sEntity;
	}
