//	Script.cpp
//
//	Dumps language elements as a script
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Console.h"

#define SCRIPT_ATTRIB						CONSTLIT("script")

void GenerateScript (CUniverse &Universe, CXMLElement *pCmdLine)
	{
	//	NOTE: Blank script matches all.

	CString sScript = pCmdLine->GetAttribute(SCRIPT_ATTRIB);
	if (CXMLElement::IsBoolTrueValue(sScript))
		sScript = CONSTLIT("sotp");

	//	Accumulate language elements for each type.

	TSortMap<CString, CScript::SScriptEntry> Text;
	for (int i = 0; i < Universe.GetDesignTypeCount(); i++)
		{
		const CDesignType *pType = Universe.GetDesignType(i);

		pType->AccumulateScript(sScript, Text);
		}

	//	Build the script

	CScript Script;
	Script.Init(Universe.GetDesignCollection(), Text);

	//	Output

	Script.OutputConsole();
	}
