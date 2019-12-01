//	Reference.h
//
//	Utility classes for Reference command
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CReferenceEntry
	{
	public:
		enum ETypes
			{
			refNone,

			refAttribute,			//	An attribute used by a type
			refEvent,				//	An event
			refLanguage,			//	A language element ID
			refProperty,			//	A property used by a type
			};

		void AddType (const CDesignType &Type) { *m_Types.SetAt(&Type) = true; }
		void Init (ETypes iType, const CString &sID, const CString &sDisplayName = NULL_STR, const CString &sDesc = NULL_STR);
		bool IsEmpty (void) const { return m_iType == refNone; }
		void Print (void) const;

		static CString GetReferenceTypeName (ETypes iType);

	private:
		TArray<CString> GetTypesAsEntities (void) const;

		ETypes m_iType = refNone;
		CString m_sID;				//	Unique name
		CString m_sDisplayName;		//	As seen by user
		CString m_sDesc;			//	Description

		TSortMap<const CDesignType *, bool> m_Types;
	};

class CReferenceSection
	{
	public:
		CReferenceEntry &GetEntry (CReferenceEntry::ETypes iType, const CString &sID, bool *retbNewEntry = NULL);
		const CReferenceEntry &GetEntry (const CString &sID) const;
		bool IsEmpty (void) const { return m_Entries.GetCount() == 0; }
		void Print (CReferenceEntry::ETypes iType) const;

	private:
		TSortMap<CString, TUniquePtr<CReferenceEntry>> m_Entries;
	};

class CReferenceLibrary
	{
	public:
		const CReferenceSection &GetSection (CReferenceEntry::ETypes iType) const;
		void Print (void) const;
		bool ScanType (const CDesignType &Type, CString *retsError);

	private:
		CReferenceSection &GetSection (CReferenceEntry::ETypes iType);
		bool ScanLanguage (const CDesignType &Type, CString *retsError);

		TSortMap<CReferenceEntry::ETypes, CReferenceSection> m_Sections;

		static const CReferenceSection m_Null;
	};
