//	Diagnostics.cpp
//
//	Generate game diagnostics.
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void GenerateDiagnostics (CUniverse &Universe, CXMLElement *pCmdLine)
	{
	int i;
	ALERROR error;
	CString sError;
	CSovereign *pPlayer = Universe.FindSovereign(g_PlayerSovereignUNID);
	CDesignCollection::SDiagnosticsCtx DiagnosticsCtx;

	//	Create all systems.

	TSortMap<CString, CSystem *> AllSystems;
	for (i = 0; i < Universe.GetTopologyNodeCount(); i++)
		{
		CTopologyNode *pNode = Universe.GetTopologyNode(i);

		//	Do not try to create end game nodes

		if (pNode->IsEndGame())
			continue;

		//	Create this system

		CSystem *pNewSystem;
		if (error = Universe.CreateStarSystem(pNode, &pNewSystem, &sError))
			{
			printf("Error creating system %s: %s\n", (LPSTR)pNode->GetID(), (LPSTR)sError);
			return;
			}

		AllSystems.SetAt(pNode->GetID(), pNewSystem);

		printf("Created %s\n", (LPSTR)pNode->GetSystemName());
		}

	Universe.StartGame(true);

	//	Calculate the traversal order.

	TArray<CString> Path;
	TArray<CString> Extra;

	//	Start diagnostics are always in the starting system (if available)

	CString sStartingNode = Universe.GetCurrentAdventureDesc().GetStartingNodeID();
	if (!AllSystems.Find(sStartingNode))
		{
		printf("ERROR: Unable to find starting system. Cannot run <OnGlobalStartDiagnostics>\n");
		return;
		}

	Universe.GetTopology().CalcTraversal(sStartingNode, Path, Extra);

	//	Traverse

	for (int i = 0; i < Path.GetCount(); i++)
		{
		CSystem *pSystem;
		if (!AllSystems.Find(Path[i], &pSystem))
			continue;

		Universe.SetNewSystem(*pSystem);
		printf("[%s] %s\n", (LPSTR)pSystem->GetTopology()->GetID(), (LPSTR)pSystem->GetName());

		if (i == 0)
			{
			Universe.GetDesignCollection().FireOnGlobalStartDiagnostics(DiagnosticsCtx);
			Universe.GetDesignCollection().FireOnGlobalRunDiagnostics(DiagnosticsCtx);
			}

		Universe.GetDesignCollection().FireOnGlobalSystemDiagnostics(DiagnosticsCtx);
		}

	//	Visit any extra systems.

	for (int i = 0; i < Extra.GetCount(); i++)
		{
		CSystem *pSystem;
		if (!AllSystems.Find(Extra[i], &pSystem))
			continue;

		Universe.SetNewSystem(*pSystem);
		printf("[%s] %s\n", (LPSTR)pSystem->GetTopology()->GetID(), (LPSTR)pSystem->GetName());

		Universe.GetDesignCollection().FireOnGlobalSystemDiagnostics(DiagnosticsCtx);
		}

	//	Fire OnEndDiagnostics

	Universe.GetDesignCollection().FireOnGlobalEndDiagnostics(DiagnosticsCtx);

	//	Results

	printf("TOTAL DIAGNOSTICS: %d\nTOTAL ERRORS: %d\n", DiagnosticsCtx.iTotalTests, DiagnosticsCtx.iTotalErrors);
	}
