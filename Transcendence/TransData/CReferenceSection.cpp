//	CReferenceSection.cpp
//
//	CReferenceSection class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Reference.h"

CReferenceEntry &CReferenceSection::GetEntry (CReferenceEntry::ETypes iType, const CString &sID, bool *retbNewEntry)

//	GetEntry
//
//	Returns a read-write entry, creating it if necessary. If we needed to create
//	the entry, we set retbNewEntry to TRUE.

	{
	if (sID.IsBlank())
		throw CException(ERR_FAIL);

	bool bNew;
	auto pEntryPtr = m_Entries.SetAt(sID, &bNew);
	if (bNew)
		{
		pEntryPtr->Set(new CReferenceEntry);
		(*pEntryPtr)->Init(iType, sID);
		}

	if (retbNewEntry)
		*retbNewEntry = bNew;

	return **(pEntryPtr);
	}

void CReferenceSection::Print (CReferenceEntry::ETypes iType) const

//	Print
//
//	Print the section to the console.

	{
	CString sSeparator = strRepeat(CONSTLIT("-"), 80);

	printf("%s\n\n", (LPSTR)strToUpper(CReferenceEntry::GetReferenceTypeName(iType)));

	bool bNeedSeparator = false;
	for (int i = 0; i < m_Entries.GetCount(); i++)
		{
		auto &pEntry = m_Entries[i];

		if (bNeedSeparator)
			printf("\n%s\n", (LPSTR)sSeparator);

		pEntry->Print();
		bNeedSeparator = true;
		}
	}
