//	Reference.cpp
//
//	Generate reference documentation
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Reference.h"

void GenerateReference (CUniverse &Universe, CXMLElement *pCmdLine)
	{
	CReferenceLibrary Library;

	//	Scan all types.

	for (int i = 0; i < Universe.GetDesignTypeCount(); i++)
		{
		const CDesignType *pType = Universe.GetDesignType(i);
		if (pType == NULL)
			throw CException(ERR_FAIL);

		CString sError;
		if (!Library.ScanType(*pType, &sError))
			{
			printf("%s\n", (LPSTR)sError);
			return;
			}
		}

	//	Output reference.

	Library.Print();
	}
