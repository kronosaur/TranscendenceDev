//	Diagnostics.cpp
//
//	Generate game diagnostics.
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Diagnostics.h"

void GenerateDiagnostics (CUniverse &Universe, CXMLElement *pCmdLine)
	{
	CConsole Console;
	CDiagnosticsCommand Command(Universe, Console);

	if (!Command.SetOptions(*pCmdLine))
		return;

	Command.Run();
	}
