//	ExtensionTable.cpp
//
//	Extension table output
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void GenerateExtensionTable (CUniverse &Universe, CXMLElement *pCmdLine)
	{
	CExtensionCollection &Extensions = Universe.GetExtensionCollection();

	//	Flags for the list

	DWORD dwFlags = CExtensionCollection::FLAG_INCLUDE_AUTO;
	if (Universe.InDebugMode())
		dwFlags |= CExtensionCollection::FLAG_DEBUG_MODE;

	//	Start by getting the list of adventures

	CString sError;
	TArray<CExtension *> List;
	if (Extensions.ComputeAvailableAdventures(dwFlags, &List, &sError) != NOERROR)
		{
		printf("ERROR: %s\n", (LPSTR)sError);
		return;
		}

	//	Add all extensions

	if (Extensions.ComputeAvailableExtensions(NULL, dwFlags | CExtensionCollection::FLAG_ACCUMULATE, TArray<DWORD>(), &List, &sError) != NOERROR)
		{
		printf("ERROR: %s\n", (LPSTR)sError);
		return;
		}

	//	Now output them all

	printf("UNID\tType\tName\tVersion\n");
	for (int i = 0; i < List.GetCount(); i++)
		{
		CExtension *pExtension = List[i];

		printf("%08x\t%s\t%s\t%s\n", pExtension->GetUNID(), (LPSTR)pExtension->GetTypeName(), (LPSTR)pExtension->GetName(), (LPSTR)pExtension->GetVersion());
		}
	}
