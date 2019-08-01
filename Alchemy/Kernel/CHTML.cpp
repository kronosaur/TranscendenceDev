//	CHTML.cpp
//
//	CHTML class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Internets.h"

static TStaticStringTable<TStaticStringEntry<SConstString>, 29> STD_ENTITY_TABLE = {
	"Aacute",		CONSTDEFS("\xC1"),
	"Eacute",		CONSTDEFS("\xC9"),
	"Iacute",		CONSTDEFS("\xCD"),
	"Ntilde",		CONSTDEFS("\xD1"),
	"Oacute",		CONSTDEFS("\xD3"),
	"Uacute",		CONSTDEFS("\xDA"),
	"Uuml",			CONSTDEFS("\xDC"),
	"aacute",		CONSTDEFS("\xE1"),
	"amp",			CONSTDEFS("&"),
	"apos",			CONSTDEFS("\'"),

	"bull",			CONSTDEFS("\x95"),
	"copy",			CONSTDEFS("\xA9"),
	"deg",			CONSTDEFS("\xB0"),
	"eacute",		CONSTDEFS("\xE9"),
	"gt",			CONSTDEFS(">"),
	"iacute",		CONSTDEFS("\xED"),
	"ldquo",		CONSTDEFS("\x93"),
	"lt",			CONSTDEFS("<"),
	"mdash",		CONSTDEFS("\x97"),
	"ntilde",		CONSTDEFS("\xF1"),
	"oacute",		CONSTDEFS("\xF3"),

	"plusmn",		CONSTDEFS("\xB1"),
	"quot",			CONSTDEFS("\""),
	"rdquo",		CONSTDEFS("\x94"),
	"reg",			CONSTDEFS("\xAE"),
	"times",		CONSTDEFS("\xD7"),
	"trade",		CONSTDEFS("\x99"),
	"uacute",		CONSTDEFS("\xFA"),
	"uuml",			CONSTDEFS("\xFC"),
	};

bool CHTML::FindStdEntity (const CString &sEntity, CString *retsValue)

//	FindStdEntity
//
//	Attempts to translate the standard HTML entity and returns TRUE if 
//	successful.

	{
	const TStaticStringEntry<SConstString> *pEntry;
	if (pEntry = STD_ENTITY_TABLE.GetAtCase(sEntity))
		{
		if (retsValue)
			*retsValue = CONSTUSE(pEntry->Value);
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
	const TStaticStringEntry<SConstString> *pEntry;
	if (pEntry = STD_ENTITY_TABLE.GetAtCase(sEntity))
		return CONSTUSE(pEntry->Value);
	else
		return sEntity;
	}
