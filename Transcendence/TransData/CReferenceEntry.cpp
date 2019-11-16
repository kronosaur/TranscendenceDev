//	CReferenceEntry.cpp
//
//	CReferenceEntry class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Reference.h"

CString CReferenceEntry::GetReferenceTypeName (ETypes iType)

//	GetReferenceTypeName
//
//	Returns the display name of the given type of reference.

	{
	switch (iType)
		{
		case refAttribute:
			return CONSTLIT("attribute");

		case refEvent:
			return CONSTLIT("event");

		case refLanguage:
			return CONSTLIT("text");

		case refProperty:
			return CONSTLIT("property");

		default:
			return NULL_STR;
		}
	}

TArray<CString> CReferenceEntry::GetTypesAsEntities (void) const

//	GetTypesAsEntities
//
//	Returns the list of types as XML entity names.

	{
	TArray<CString> Result;
	Result.InsertEmpty(m_Types.GetCount());

	for (int i = 0; i < m_Types.GetCount(); i++)
		Result[i] = m_Types.GetKey(i)->GetEntityName();

	return Result;
	}

void CReferenceEntry::Init (ETypes iType, const CString &sID, const CString &sDisplayName, const CString &sDesc)

//	Init
//
//	Initializes

	{
	if (sID.IsBlank())
		throw CException(ERR_FAIL);

	m_iType = iType;
	m_sID = sID;
	m_sDisplayName = sDisplayName;
	m_sDesc = sDesc;
	}

void CReferenceEntry::Print (void) const

//	Print
//
//	Prints the entry to the console.

	{
	auto TypeList = GetTypesAsEntities();

	printf("%s\n", (LPSTR)m_sID);
	printf("%s\n", (LPSTR)GetReferenceTypeName(m_iType));

	auto sTypeList = strJoin(TypeList, CONSTLIT(", "));
	printf("\nTypes:\n\n%s\n", (LPSTR)sTypeList);
	}
