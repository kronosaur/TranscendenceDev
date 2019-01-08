//	TSUISettings.h
//
//	Transcendence UI Engine
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

//	Extension List Options ----------------------------------------------------

class CExtensionListMap
	{
	public:
		inline const TSortMap<DWORD, bool> &GetDisabledExtensionList (void) const { return m_Disabled; }
		void GetList (DWORD dwAdventure, bool bDebugMode, TArray<DWORD> *retList) const;
		void GetList (DWORD dwAdventure, const TArray<CExtension *> &Available, bool bDebugMode, TArray<DWORD> *retList) const;
		void SetExtensionEnabled (DWORD dwUNID, bool bEnabled);
		void SetList (DWORD dwAdventure, const TArray<CExtension *> &Available, bool bDebugMode, const TArray<DWORD> &List);
		ALERROR ReadFromXML (CXMLElement *pDesc);
		ALERROR WriteAsXML (IWriteStream *pOutput);

	private:
		struct SEntry
			{
			SEntry (void) :
					m_bDisabledIfNotInList(false),
					m_bDisabledIfNotInDebugList(false)
				{ }

			TSortMap<DWORD, bool> List;			//	State (enabled/disabled) for each extension
			TSortMap<DWORD, bool> DebugList;	//	State for each extension when in debug mode
			bool m_bDisabledIfNotInList;		//	If TRUE, extensions not in list should be disabled
			bool m_bDisabledIfNotInDebugList;	//	If TRUE, extensions not in debug list should be disabled
			};

		ALERROR ReadDefault (CXMLElement *pEntry);
		ALERROR ReadList (const CString &sList, bool bEnabled, TSortMap<DWORD, bool> *retpList, bool *retbDisabledIfNotInList = NULL);
		ALERROR ReadOption (CXMLElement *pEntry);
		ALERROR WriteDefault (IWriteStream &Output, DWORD dwAdventure, const SEntry &Entry) const;
		ALERROR WriteList (IWriteStream &Output, DWORD dwAdventure, bool bDebugMode, const TSortMap<DWORD, bool> &List, bool bDisabledIfNotInList = false) const;
		ALERROR WriteOption (IWriteStream &Output, DWORD dwExtension, const CString &sOption, const CString &sValue) const;

		TSortMap<DWORD, SEntry> m_Map;
		TSortMap<DWORD, bool> m_Disabled;
	};

//	User Settings --------------------------------------------------------------

class CUserSettings
	{
	public:
		enum EOptionTypes
			{
			optionBoolean,
			optionInteger,
			optionString,
			};

		struct SOptionDef
			{
			LPSTR pszID;
			EOptionTypes iType;
			LPSTR pszDefault;
			DWORD dwFlags;
			};

		CUserSettings (void) : 
				m_bModified(false)
			{ }

		bool GetValueBoolean (const CString &sID) const;
		inline const CString &GetUserFolder (void) const { return m_sUserRoot; }
		ALERROR Load (CHumanInterface &HI, const CString &sFilespec, SOptionDef *pDefinitions, CString *retsError = NULL);
		ALERROR LoadCommandLine (char *pszCmdLine, CString *retsError = NULL);

	private:
		struct SOption
			{
			CString sID;					//	Option ID
			EOptionTypes iType;				//	Value type
			CString sDefault;				//	Default value
			DWORD dwFlags;					//	Flags

			bool bValue;					//	Current boolean value
			int iValue;						//	Current integer value
			CString sValue;					//	Current string value

			CString sSettingsValue;			//	Value of this option in settings file
											//	NOTE: This may differ from the current value
											//	above if we don't want to save a setting
			};

		void SetValue (SOption *pOption, const CString &sValue, bool bSetSettings = false);
		void SetValue (const CString &sID, const CString &sValue, bool bSetSettings = false);

		CString m_sUserRoot;				//	Path to writable directory (NULL_STR = current directory)

		TSortMap<CString, SOption> m_Options;	//	List of current options (by ID)
		TArray<CString> m_ExtensionFolders;	//	List of folders for extensions (may be empty)
		CExtensionListMap m_Extensions;		//	List of selected extensions

		bool m_bModified;					//	If TRUE, we need to save out settings
	};
