//	TSELanguage.h
//
//	Language classes and functions
//	Copyright 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

enum NounFlags
	{
	nounDefiniteArticle		= 0x00000001,	//	Article must be "the"
	nounFirstPlural			= 0x00000002,	//	First word should be pluralized
	nounPluralES			= 0x00000004,	//	Append "es" to pluralize
	nounCustomPlural		= 0x00000008,	//	Plural form follows after ";"
	nounSecondPlural		= 0x00000010,	//	Second word should be pluralized
	nounVowelArticle		= 0x00000020,	//	Should be "a" instead of "an" or vice versa
	nounNoArticle			= 0x00000040,	//	Do not use "a" or "the"
	nounPersonalName		= 0x00000080,	//	This is a human name (e.g., "Arco Vaughn")
	nounPluralizeLongForm	= 0x00000100,	//	OK to pluralize even long form (e.g., "Tripoli-class destroyers")
	};

enum NounPhraseFlags
	{
	nounCapitalize			= 0x00000001,	//	Capitalize the first letter
	nounPlural				= 0x00000002,	//	Pluralize noun phrase (regardless of item count)
	nounArticle				= 0x00000004,	//	Prefix with either "the" or "a" (regardless of item count)
	nounCount				= 0x00000008,	//	Prefix with (if > 1) a count or (if == 1) article
	nounCountOnly			= 0x00000010,	//	Prefix with (if > 1) a count or (if == 1) nothing
	nounNoModifiers			= 0x00000020,	//	Do not prefix with "enhanced" or "damaged"
	nounDemonstrative		= 0x00000040,	//	Prefix with either "the" or "this" or "these"
	nounShort				= 0x00000080,	//	Use short form of name
	nounActual				= 0x00000100,	//	Use actual form (not unidentified form)
	nounNoEvent				= 0x00000200,	//	Do not fire GetName event
	nounTitleCapitalize		= 0x00000400,	//	Title-capitalize
	nounInstalledState		= 0x00000800,	//	Prefix with "installed" if item is installed
	nounCountAlways			= 0x00001000,	//	Always prefix with count.
	nounDuplicateModifier	= 0x00002000,	//	If duplicate device, add disambiguating modifier
	nounGeneric				= 0x00004000,	//	Generic name (e.g., ship class name instead of ship name)
	nounDemonym				= 0x00008000,	//	Demonym (name for a citizen of a sovereign)
	nounAdjective			= 0x00010000,	//	Sovereign name as adjective
	nounTokenize			= 0x00020000,	//	Replace whitespace with underscores
	};

class CLanguage
	{
	public:
		static CString ComposeNounPhrase (const CString &sNoun, int iCount, const CString &sModifier, DWORD dwNounFlags, DWORD dwComposeFlags);
		static DWORD ParseNounFlags (const CString &sValue);
		static CString ParseNounForm (const CString &sNoun, const CString &sModifier, DWORD dwNounFlags, bool bPluralize, bool bShortName, CString *retsArticle = NULL);
	};