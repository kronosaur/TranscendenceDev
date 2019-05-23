//	SmokeTest.cpp
//
//	Test system creation
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define DEFAULT_SYSTEM_SAMPLE				100
#define DEFAULT_UPDATES						1000

void DoRandomNumberTest (void)
	{
	int i;
	int Results[100];
	for (i = 0; i < 100; i++) 
		Results[i] = 0;

	for (i = 0; i < 1000000; i++)
		Results[mathRandom(1, 100)-1]++;

	for (i = 0; i < 100; i++)
		printf("%d: %d\n", i, Results[i]);
	}

struct STestCtx
	{
	SSystemUpdateCtx UpdateCtx;

	int iSystemSample = 0;
	int iSystemUpdateTime = 0;

	DWORD dwUpdateTime = 0;
	DWORD dwUpdateCount = 0;

	bool bRunStartDiag = false;
	bool bNoDiagnostics = false;
	};

void RunSystem (CUniverse &Universe, CSystem &System, STestCtx &Ctx);

void DoSmokeTest (CUniverse &Universe, CXMLElement *pCmdLine)
	{
	ALERROR error;
	CString sError;
	int i;

	STestCtx Ctx;

	//	Options

	Ctx.iSystemSample = pCmdLine->GetAttributeIntegerBounded(CONSTLIT("count"), 1, -1, DEFAULT_SYSTEM_SAMPLE);
	Ctx.iSystemUpdateTime = pCmdLine->GetAttributeIntegerBounded(CONSTLIT("updates"), 0, -1, DEFAULT_UPDATES);
	Ctx.bNoDiagnostics = pCmdLine->GetAttributeBool(CONSTLIT("noDiagnostics"));

	//	Update context

	Ctx.UpdateCtx.bForceEventFiring = true;
	Ctx.UpdateCtx.bForcePainted = true;

	//	Generate systems for multiple games

	for (i = 0; i < Ctx.iSystemSample; i++)
		{
		Ctx.bRunStartDiag = !Ctx.bNoDiagnostics;
		int iSample = i + 1;
		printf("SAMPLE %d\n", iSample);

		//	Create all systems

		TSortMap<CString, CSystem *> AllSystems;
		for (int iNode = 0; iNode < Universe.GetTopologyNodeCount(); iNode++)
			{
			CTopologyNode *pNode = Universe.GetTopologyNode(iNode);
			if (pNode == NULL || pNode->IsEndGame())
				continue;

			printf("%s\n", (LPSTR)pNode->GetSystemName());

			//	Create the system

			CSystem *pSystem;
			if (error = Universe.CreateStarSystem(pNode, &pSystem, &sError))
				{
				printf("ERROR: Unable to create star system: %s.\n", (LPSTR)sError);
				return;
				}

			AllSystems.SetAt(pNode->GetID(), pSystem);
			}

		//	Start diagnostics are always in SE (if available)

		CSystem *pSE = NULL;
		if (AllSystems.Find(CONSTLIT("SE"), &pSE))
			{
			RunSystem(Universe, *pSE, Ctx);
			}
		else
			printf("WARNING: Unable to find SE. Cannot run <OnGlobalStartDiagnostics>\n");

		//	Now update all system

		for (int iSystem = 0; iSystem < AllSystems.GetCount(); iSystem++)
			{
			CSystem *pSystem = AllSystems[iSystem];
			if (pSystem == pSE)
				continue;

			RunSystem(Universe, *pSystem, Ctx);
			}

		if (!Ctx.bNoDiagnostics)
			{
			printf("END DIAGNOSTICS\n");
			Universe.GetDesignCollection().FireOnGlobalEndDiagnostics();
			}

		Universe.Reinit();

		if (Universe.InitGame(0, &sError) != NOERROR)
			{
			printf("ERROR: %s\n", sError.GetASCIIZPointer());
			return;
			}

		printf("-------------------------------------------------------------------------------\n");
		}

	//	Print update performance

	if (Ctx.dwUpdateCount > 0)
		{
		Metric rTime = (Metric)Ctx.dwUpdateTime / Ctx.dwUpdateCount;
		CString sUpdates = strFormatInteger((int)Ctx.dwUpdateCount, -1, FORMAT_THOUSAND_SEPARATOR);
		printf("Average time per update: %.2f ms [%s updates]\n", rTime, (LPSTR)sUpdates);
		}

	//	Done

	printf("Test complete.\n");
	}

void RunSystem (CUniverse &Universe, CSystem &System, STestCtx &Ctx)
	{
	//	Set the POV

	CSpaceObject *pPOV = System.GetObject(0);
	Universe.SetPOV(pPOV);
	System.SetPOVLRS(pPOV);

	//	Prepare system

	Universe.UpdateExtended();
	Universe.GarbageCollectLibraryBitmaps();

	//	Run diagnostics start

	if (Ctx.bRunStartDiag)
		{
		printf("\nSTART DIAGNOSTICS\n");
		Universe.GetDesignCollection().FireOnGlobalStartDiagnostics();
		Ctx.bRunStartDiag = false;
		}

	//	Update for a while

	for (int i = 0; i < Ctx.iSystemUpdateTime; i++)
		{
		Ctx.dwUpdateCount++;
		DWORD dwStart = ::GetTickCount();

		Universe.Update(Ctx.UpdateCtx);

		Ctx.dwUpdateTime += ::sysGetTicksElapsed(dwStart);
		}

	//	Run diagnostics code

	if (!Ctx.bNoDiagnostics)
		{
		printf("DIAGNOSTICS: %s\n", (LPSTR)System.GetName());
		Universe.GetDesignCollection().FireOnGlobalSystemDiagnostics();
		}

	//	Done with system

	Universe.DestroySystem(&System);
	}
