//	TSELanguage.h
//
//	Language classes and functions
//	Copyright 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

struct SDesignLoadCtx;

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
		enum ENumberFormatTypes
			{
			numberNone,
			numberError,

			numberInteger,					//	1,000,000
			numberPower,					//	1.1 MW
			numberSpeed,					//	.05c
			};

		enum EVerbFlags
			{
			verbPluralize =					0x00000001,	//	Use the plural form of the verb
			};

		static CString ComposeNounPhrase (const CString &sNoun, int iCount, const CString &sModifier, DWORD dwNounFlags, DWORD dwComposeFlags);
		static CString ComposeNumber (ENumberFormatTypes iFormat, int iNumber);
		static CString ComposeNumber (ENumberFormatTypes iFormat, Metric rNumber);
		static CString ComposeNumber (ENumberFormatTypes iFormat, ICCItem *pNumber);
		static CString ComposeVerb (const CString &sVerb, DWORD dwVerbFlags);
		static DWORD LoadNameFlags (CXMLElement *pDesc);
		static DWORD ParseNounFlags (const CString &sValue);
		static CString ParseNounForm (const CString &sNoun, const CString &sModifier, DWORD dwNounFlags, bool bPluralize, bool bShortName, CString *retsArticle = NULL);
		static ENumberFormatTypes ParseNumberFormat (const CString &sValue);
	};

class CLanguageDataBlock
	{
	public:
		~CLanguageDataBlock (void);

		CLanguageDataBlock &operator= (const CLanguageDataBlock &Src);

		void AddEntry (const CString &sID, const CString &sText);
		void DeleteAll (void);
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		void MergeFrom (const CLanguageDataBlock &Source);
		bool Translate (CSpaceObject *pObj, const CString &sID, ICCItem *pData, ICCItem **retpResult) const;
		bool Translate (CSpaceObject *pObj, const CString &sID, ICCItem *pData, TArray<CString> *retText) const;
		bool Translate (CSpaceObject *pObj, const CString &sID, ICCItem *pData, CString *retsText) const;

	private:
		enum ETranslateResult
			{
			resultArray,
			resultString,
			resultCCItem,

			resultFound,
			resultNotFound,
			};

		struct SEntry
			{
			CString sText;
			ICCItem *pCode;
			};

		ICCItem *ComposeCCItem (CCodeChain &CC, ICCItem *pValue, const CString &sPlayerName, GenomeTypes iPlayerGenome, ICCItem *pData) const;
		bool IsCode (const CString &sText) const;
		CString ParseTextBlock (const CString &sText) const;
		ETranslateResult Translate (CSpaceObject *pObj, const CString &sID, ICCItem *pData, TArray<CString> *retText, CString *retsText, ICCItem **retpResult = NULL) const;

		TSortMap<CString, SEntry> m_Data;
	};

class CNameDesc
	{
	public:
		CNameDesc (void);

		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
//		CString GenerateName (CSystem *pSystem, DWORD *retdwNameFlags) const;
		CString GenerateName (TSortMap<CString, CString> *pParams, DWORD *retdwNameFlags) const;
		inline const CString &GetConstantName (DWORD *retdwNameFlags) const { if (retdwNameFlags) *retdwNameFlags = m_dwConstantNameFlags; return m_sConstantName; }
		inline bool IsConstant (void) const { return (m_Names.GetCount() == 0); }
		inline bool IsEmpty (void) const { return m_sConstantName.IsBlank(); }
		void Reinit (void);

	private:
		CString GenerateName (const CString &sName, TSortMap<CString, CString> *pParams) const;

		CString m_sConstantName;						//	Generic name (must be constant)
		DWORD m_dwConstantNameFlags;

		TArray<CString> m_Names;						//	List of name patterns.
		DWORD m_dwNameFlags;							//	For backwards compatibility.
		mutable int m_iNamesGenerated;					//	Number of names generated so far.
	};
