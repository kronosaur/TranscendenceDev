//	CReferenceLibrary.cpp
//
//	CReferenceLibrary class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Reference.h"

const CReferenceSection CReferenceLibrary::m_Null;

const CReferenceSection &CReferenceLibrary::GetSection (CReferenceEntry::ETypes iType) const

//	GetSection
//
//	Returns the given section.

	{
	const CReferenceSection *pSection = m_Sections.GetAt(iType);
	if (pSection)
		return *pSection;
	else
		return m_Null;
	}

CReferenceSection &CReferenceLibrary::GetSection (CReferenceEntry::ETypes iType)

//	GetSection
//
//	Returns the given section.

	{
	return *m_Sections.SetAt(iType);
	}

void CReferenceLibrary::Print (void) const

//	Print
//
//	Print out the library.

	{
	bool bNeedSeparator = false;

	for (int i = 0; i < m_Sections.GetCount(); i++)
		{
		auto SectionType = m_Sections.GetKey(i);
		auto &Section = m_Sections[i];
		if (Section.IsEmpty())
			continue;

		if (bNeedSeparator)
			printf("\n\n");

		Section.Print(SectionType);
		bNeedSeparator = true;
		}
	}

bool CReferenceLibrary::ScanLanguage (const CDesignType &Type, CString *retsError)

//	ScanLanguage
//
//	Scans <Language> elements and adds them.

	{
	auto &Section = GetSection(CReferenceEntry::refLanguage);
	auto Language = Type.GetMergedLanguageBlock();

	for (int i = 0; i < Language.GetCount(); i++)
		{
		auto Src = Language.GetEntry(i);
		if (Src.sID.IsBlank())
			{
			if (retsError) *retsError = CONSTLIT("Missing <Language> textID.");
			return false;
			}

		auto &Dest = Section.GetEntry(CReferenceEntry::refLanguage, Src.sID);
		Dest.AddType(Type);
		}

	return true;
	}

bool CReferenceLibrary::ScanType (const CDesignType &Type, CString *retsError)

//	ScanType
//
//	Scans the given type and adds all references to the library. If we get an 
//	error, we return FALSE.

	{
	if (!ScanLanguage(Type, retsError))
		return false;

	return true;
	}
