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

	//	Start diagnostics are always in the starting system (if available)

	CSystem *pSE = NULL;
	if (AllSystems.Find(Universe.GetCurrentAdventureDesc().GetStartingNodeID(), &pSE))
		{
		Universe.SetCurrentSystem(pSE);

		Universe.GetDesignCollection().FireOnGlobalStartDiagnostics(DiagnosticsCtx);
		Universe.GetDesignCollection().FireOnGlobalRunDiagnostics(DiagnosticsCtx);
		}
	else
		printf("WARNING: Unable to find starting system. Cannot run <OnGlobalStartDiagnostics>\n");

	//	Now loop over all systems and invoke OnSystemDiagnostics

	for (i = 0; i < AllSystems.GetCount(); i++)
		{
		if (AllSystems[i] == pSE)
			continue;

		Universe.SetCurrentSystem(AllSystems[i]);

		//	System diagnostics

		Universe.GetDesignCollection().FireOnGlobalSystemDiagnostics(DiagnosticsCtx);
		}

	//	Fire OnEndDiagnostics

	Universe.GetDesignCollection().FireOnGlobalEndDiagnostics(DiagnosticsCtx);

	//	Results

	printf("TOTAL DIAGNOSTICS: %d\nTOTAL ERRORS: %d\n", DiagnosticsCtx.iTotalTests, DiagnosticsCtx.iTotalErrors);
	}
