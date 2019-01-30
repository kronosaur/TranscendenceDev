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
	nounNoDeterminer		= 0x00040000,	//	No count or article, but pluralize if count > 1
	nounNoQuotes			= 0x00080000,	//	Convert double-quotes to single-quotes (for use inside quoted text)
	nounEscapeQuotes		= 0x00100000,	//	Convert double-quotes to escaped double-quotes
	};

class CLanguage
	{
	public:
		enum ENumberFormatTypes
			{
			numberNone,
			numberError,

			numberFireRate,					//	1.5 shots/sec
			numberInteger,					//	1,000,000
			numberMass,						//	500 kg or 1.5 tons
			numberPower,					//	1.1 MW
			numberReal,						//	100 or 10.0 or 1.0 or 0.00
			numberRegenRate,				//	1.0 hp/sec
			numberSpeed,					//	.05c
			};

		enum EVerbFlags
			{
			verbPluralize =					0x00000001,	//	Use the plural form of the verb
			};

		enum ELabelAttribs
			{
			specialNone			= 0x00000000,
			specialAll			= 0xFFFFFFFF,

			specialCancel		= 0x00000001,
			specialDefault		= 0x00000002,
			specialNextKey		= 0x00000004,
			specialPrevKey		= 0x00000008,
			specialPgDnKey		= 0x00000010,
			specialPgUpKey		= 0x00000020,
			};

		struct SNounDesc
			{
			SNounDesc (void) :
					bHasQuotes(false)
				{ }

			CString sArticle;				//	Article to use for noun
			bool bHasQuotes;				//	Noun has embedded quotes
			};

		static int CalcMetricNumber (Metric rNumber, int *retiWhole, int *retiDecimal);
		static CString Compose (const CString &sString, ICCItem *pArgs);
		static CString ComposeGenderedWord (const CString &sWord, GenomeTypes iGender);
		static CString ComposeNounPhrase (const CString &sNoun, int iCount, const CString &sModifier, DWORD dwNounFlags, DWORD dwComposeFlags);
		static CString ComposeNumber (ENumberFormatTypes iFormat, int iNumber);
		static CString ComposeNumber (ENumberFormatTypes iFormat, Metric rNumber);
		static CString ComposeNumber (ENumberFormatTypes iFormat, ICCItem *pNumber);
		static CString ComposeVerb (const CString &sVerb, DWORD dwVerbFlags);
		static bool FindGenderedWord (const CString &sWord, GenomeTypes iGender, CString *retsResult = NULL);
		static ICCItemPtr GetNounFlags (DWORD dwFlags);
		static DWORD LoadNameFlags (CXMLElement *pDesc);
		static void ParseItemName (const CString &sName, CString *retsRoot, CString *retsModifiers);
		static void ParseLabelDesc (const CString &sLabelDesc, CString *retsLabel, CString *retsKey = NULL, int *retiKey = NULL, TArray<ELabelAttribs> *retAttribs = NULL);
		static DWORD ParseNounFlags (const CString &sValue);
		static CString ParseNounForm (const CString &sNoun, const CString &sModifier, DWORD dwNounFlags, bool bPluralize, bool bShortName, SNounDesc *retDesc = NULL);
		static ENumberFormatTypes ParseNumberFormat (const CString &sValue);

	private:
		struct SVarInfo
			{
			CString sParam;
			bool bCharacter = false;
			bool bCapitalize = false;
			bool bDone = false;
			};

		static CString ComposeCharacterReference (CUniverse &Universe, const CString &sCharacter, const CString &sField, ICCItem *pData);
		static CString ComposeGenderedWordHelper (CUniverse &Universe, const CString &sWord, const CString &sField, ICCItem *pData);
		static CString ParseVar (char *pPos, SVarInfo &retVarInfo, char **retpPos);
	};

class CLanguageDataBlock
	{
	public:
		struct SEntryDesc
			{
			CString sID;
			CString sText;
			ICCItemPtr pCode;
			};

		CLanguageDataBlock (void) { }
		CLanguageDataBlock (const CLanguageDataBlock &Src) { Copy(Src); }
		~CLanguageDataBlock (void) { CleanUp(); }

		inline CLanguageDataBlock &operator= (const CLanguageDataBlock &Src) { CleanUp(); Copy(Src); return *this; }

		void AddEntry (const CString &sID, const CString &sText);
		void DeleteAll (void);
		inline int GetCount (void) const { return m_Data.GetCount(); }
		SEntryDesc GetEntry (int iIndex) const;
		inline bool HasEntry (const CString &sID) const { return (m_Data.GetAt(sID) != NULL); }
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		inline bool IsEmpty (void) const { return (m_Data.GetCount() == 0); }
		void MergeFrom (const CLanguageDataBlock &Source);
		bool Translate (const CDesignType &Type, const CString &sID, ICCItem *pData, ICCItemPtr &retResult) const;
		bool Translate (CSpaceObject *pObj, const CString &sID, ICCItem *pData, ICCItemPtr &retResult) const;
		bool TranslateText (const CDesignType &Type, const CString &sID, ICCItem *pData, CString *retsText) const;
		bool TranslateText (CSpaceObject *pObj, const CString &sID, ICCItem *pData, CString *retsText) const;
		bool TranslateText (const CItem &Item, const CString &sID, ICCItem *pData, CString *retsText) const;

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

		inline void CleanUp (void) { DeleteAll(); }
		ICCItemPtr ComposeCCItem (CCodeChain &CC, ICCItem *pValue, ICCItem *pData) const;
		bool ComposeCCResult (ETranslateResult iResult, ICCItem *pData, const TArray<CString> &List, const CString &sText, ICCItem *pCCResult, ICCItemPtr &retResult) const;
		ETranslateResult ComposeResult (ICCItem *pResult, ICCItem *pData, TArray<CString> *retText, CString *retsText, ICCItemPtr *retpResult = NULL) const;
		bool ComposeTextResult (ETranslateResult iResult, const TArray<CString> &List, CString *retsText) const;
		void Copy (const CLanguageDataBlock &Src);
		bool IsCode (const CString &sText) const;
		CString ParseTextBlock (const CString &sText) const;
		ETranslateResult TranslateFull (const CDesignType &Type, const CString &sID, ICCItem *pData, TArray<CString> *retText, CString *retsText, ICCItemPtr *retpResult = NULL) const;
		ETranslateResult TranslateFull (CSpaceObject *pObj, const CString &sID, ICCItem *pData, TArray<CString> *retText, CString *retsText, ICCItemPtr *retpResult = NULL) const;
		ETranslateResult TranslateFull (const CItem &Item, const CString &sID, ICCItem *pData, TArray<CString> *retText, CString *retsText, ICCItemPtr *retpResult = NULL) const;
		const SEntry *TranslateTry (const CString &sID, ICCItem *pData, ETranslateResult &retiResult, TArray<CString> *retText = NULL, CString *retsText = NULL) const;

		TSortMap<CString, SEntry> m_Data;
	};

class CNameDesc
	{
	public:
		CNameDesc (void);

		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		ALERROR InitFromXMLRoot (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
//		CString GenerateName (CSystem *pSystem, DWORD *retdwNameFlags) const;
		CString GenerateName (TSortMap<CString, CString> *pParams = NULL, DWORD *retdwNameFlags = NULL) const;
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

class CVirtualKeyData
	{
	public:
		static constexpr DWORD FLAG_NON_STANDARD =			0x00000001;	//	Not available in keyboard UI
		static constexpr DWORD FLAG_SPECIAL_KEY =			0x00000002;	//	Custom VK code

		static constexpr DWORD INVALID_VIRT_KEY = 0xFFFFFFFF;
		static constexpr DWORD VK_NUMPAD_ENTER = 0xE0;

		static DWORD GetKey (const CString &sKey);
		static DWORD GetKeyFlags (DWORD dwVirtKey);
		static CString GetKeyID (DWORD dwVirtKey);
		static CString GetKeyLabel (DWORD dwVirtKey);
		static DWORD TranslateVirtKey (DWORD dwVirtKey, DWORD dwKeyData);

	private:
		struct SVirtKeyData
			{
			char *pszName;
			char *pszLabel;
			DWORD dwFlags;
			};

		static SVirtKeyData m_VirtKeyData[256];
	};
