//	BattleSim.cpp
//
//	Simulates a ship attacking a station.
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

const int DEFAULT_RUN_COUNT = 10;
const int DEFAULT_TIME_OUT = 30 * 60 * 5;
const int DONE_TIME = 30 * 4;
const Metric TEAM_SEPARATION_DISTANCE = (50.0 * LIGHT_SECOND);
const Metric SHIP_SEPARATION_DISTANCE = (10.0 * LIGHT_SECOND);

#define ATTACKERS_ATTRIB					CONSTLIT("attackers")
#define ATTACKER_MULTIPLIER_ATTRIB			CONSTLIT("attackerMultiplier")
#define COUNT_ATTRIB						CONSTLIT("count")
#define DEFENDERS_ATTRIB					CONSTLIT("defenders")
#define DEFENDER_MULTIPLIER_ATTRIB			CONSTLIT("defenderMultiplier")
#define VIEWER_ATTRIB						CONSTLIT("viewer")

enum EResults
{
	resultError,

	resultAttackersDestroyed,
	resultDefendersDestroyed,
	resultTimeout,
};

EResults RunEncounter(CUniverse &Universe, CSimViewer &Viewer, const TArray<CShipClass *> &vDefenderClasses, const TArray<CShipClass *> &vAttackerClasses, int iDefenderMultiplier, int iAttackerMultiplier, CSovereign *pDefenderSovereign, CSovereign *pAttackerSovereign);
ALERROR FindShipClasses(CUniverse &Universe, CXMLElement *pCmdLine, const Kernel::CString &attribute, TArray<CShipClass *> *vShipClasses);
CSovereign *GetAttackerSovereign(CUniverse &Universe, CShipClass *pDefenderClass);

void RunBattleSim(CUniverse &Universe, CXMLElement *pCmdLine)
{
	int iRuns = pCmdLine->GetAttributeIntegerBounded(COUNT_ATTRIB, 1, -1, DEFAULT_RUN_COUNT);
	int iDefenderMultiplier = pCmdLine->GetAttributeIntegerBounded(DEFENDER_MULTIPLIER_ATTRIB, 1, -1, 1);
	int iAttackerMultiplier = pCmdLine->GetAttributeIntegerBounded(ATTACKER_MULTIPLIER_ATTRIB, 1, -1, 1);

	//	Load defender and attacker ships.

	TArray<CShipClass *> vDefenderClasses;
	if (FindShipClasses(Universe, pCmdLine, DEFENDERS_ATTRIB, &vDefenderClasses) != NOERROR)
		{
		printf("ERROR: Error loading defender ship classes.\n");
		return;
		}
	TArray<CShipClass *> vAttackerClasses;
	if (FindShipClasses(Universe, pCmdLine, ATTACKERS_ATTRIB, &vAttackerClasses) != NOERROR)
		{
		printf("ERROR: Error loading attacker ship classes.\n");
		return;
		}
	
	//	Create a viewer, if desired

	CSimViewer Viewer;
	if (pCmdLine->GetAttributeBool(VIEWER_ATTRIB))
		Viewer.Create();

	CSovereign *pAttackerSovereign = GetAttackerSovereign(Universe, vDefenderClasses[0]);
	if (pAttackerSovereign == NULL)
		{
		printf("ERROR: Unable to find enemy sovereign of class: %s [%x].\n", vDefenderClasses[0]->GetNounPhrase().GetASCIIZPointer(), vDefenderClasses[0]->GetUNID());
		return;
		}
	CSovereign *pDefenderSovereign = vDefenderClasses[0]->GetDefaultSovereign();
	if (pAttackerSovereign == NULL)
		{
		printf("ERROR: Unable to find sovereign of class: %s [%x].\n", vDefenderClasses[0]->GetNounPhrase().GetASCIIZPointer(), vDefenderClasses[0]->GetUNID());
		return;
		}

	int iRun;
	int iAttackersWin = 0;
	int iDefendersWin = 0;
	int iTimeOut = 0;
	for (iRun = 0; iRun < iRuns; iRun++)
		{
		EResults iResult = RunEncounter(Universe, Viewer, vDefenderClasses, vAttackerClasses, iDefenderMultiplier, iAttackerMultiplier, pDefenderSovereign, pAttackerSovereign);
		if (iResult == resultError)
			return;
		switch (iResult)
			{
			case resultAttackersDestroyed:
				iDefendersWin += 1;
				printf("Defenders won.\n");
				break;
			case resultDefendersDestroyed:
				iAttackersWin += 1;
				printf("Attackers won.\n");
				break;
			case resultTimeout:
				iTimeOut += 1;
				printf("Timed out.\n");
				break;
			case resultError:
				return;
			}
		}
	printf("Attackers win: %d\n", iAttackersWin);
	printf("Defenders win: %d\n", iDefendersWin);
	printf("Timeouts: %d\n", iTimeOut);
	if (iDefendersWin + iAttackersWin > 0)
		printf("Win percentage: %d\n", 100 * iAttackersWin / (iDefendersWin + iAttackersWin));
	Viewer.Destroy();
}

ALERROR FindShipClasses(CUniverse &Universe, CXMLElement *pCmdLine, const Kernel::CString &attribute, TArray<CShipClass *> *vShipClasses)
	{
	TArray<DWORD> vShipUUIDs;
	if (pCmdLine->GetAttributeIntegerList(attribute, &vShipUUIDs) != NOERROR)
		{
		printf("ERROR: Error parsing the %s UUIDs.\n", attribute.GetASCIIZPointer());
		return ERR_FAIL;
		}
	if (vShipUUIDs.GetCount() == 0)
		{
		printf("ERROR: Need at least one UUID specified for %s.\n", attribute.GetASCIIZPointer());
		return ERR_FAIL;
		}
	int i;
	for (i = 0; i < vShipUUIDs.GetCount(); i++)
		{
		CShipClass *pShipClass = Universe.FindShipClass(vShipUUIDs[i]);
		if (!pShipClass)
			{
			printf("ERROR: Couldn't find ship class %x\n", vShipUUIDs[i]);
			return ERR_FAIL;
			}
		vShipClasses->Insert(pShipClass);
		}
	return NOERROR;
	}

CSovereign *GetAttackerSovereign(CUniverse &Universe, CShipClass *pDefenderClass)
	{
	int i;

	CSovereign *pDefenderSovereign = pDefenderClass->GetDefaultSovereign();
	if (pDefenderSovereign)
	{
		for (i = 0; i < Universe.GetSovereignCount(); i++)
		{
			CSovereign *pSovereign = Universe.GetSovereign(i);
			if (pDefenderSovereign->IsEnemy(pSovereign))
				return pSovereign;
		}
	}

	return NULL;
	}

ALERROR AttackRandomEnemy(TArray<CShip *>& vShips, TArray<CShip *>& vTargets)
	{
	if (vTargets.GetCount() == 0)
		return NOERROR;
	int i;
	for (i = 0; i < vShips.GetCount(); i++)
		{
		CShip* pShip = vShips[i];
		IShipController *pController = pShip->GetController();
		if (pController == NULL)
			{
			printf("ERROR: No controller for ship.\n");
			return ERR_FAIL;
			}
		CSpaceObject *pTarget;
		IShipController::OrderTypes iOrder = pController->GetCurrentOrderEx(&pTarget);
		if (!pTarget || pTarget->IsDestroyed() || iOrder == IShipController::orderNone)
			pTarget = vTargets[mathRandom(0, vTargets.GetCount() - 1)];
			pController->CancelAllOrders();
			pController->AddOrder(IShipController::orderDestroyTarget, pTarget, IShipController::SData());
		}
	return NOERROR;
	}

EResults RunEncounter(CUniverse &Universe, CSimViewer &Viewer, const TArray<CShipClass *> &vDefenderClasses, const TArray<CShipClass *> &vAttackerClasses, int iDefenderMultiplier, int iAttackerMultiplier, CSovereign *pDefenderSovereign, CSovereign *pAttackerSovereign)
	{
	int iTimeOut = DEFAULT_TIME_OUT;

	//	Make sure the universe is clean

	CString sError;
	if (Universe.InitGame(0, &sError) != NOERROR)
		{
		printf("ERROR: %s", sError.GetASCIIZPointer());
		return resultError;
		}

	//	Create an empty system

	CSystem *pSystem;
	if (Universe.CreateEmptyStarSystem(&pSystem) != NOERROR)
		{
		printf("ERROR: Unable to create empty star system.\n");
		return resultError;
		}

	Universe.MarkLibraryBitmaps();

	//	Create defenders
	TArray<CShip* > vDefenders;
	int i;
	for (i = 0; i < vDefenderClasses.GetCount(); i++)
		{
		int j = 0;
		for (j = 0; j < iDefenderMultiplier; j++)
			{
			int iPlace = i * iDefenderMultiplier + j - iDefenderMultiplier * vDefenderClasses.GetCount() / 2;
			CVector vPos = CVector(TEAM_SEPARATION_DISTANCE, iPlace * SHIP_SEPARATION_DISTANCE);
			CShip *pDefender;
			if (pSystem->CreateShip(vDefenderClasses[i]->GetUNID(), NULL, NULL, pDefenderSovereign, vPos, CVector(), 180, NULL, NULL, &pDefender) != NOERROR)
				{
				printf("ERROR: Unable to create defending ship.\n");
				return resultError;
				}
			vDefenders.Insert(pDefender);
			}
		}

	//	Create attackers

	TArray<CShip* > vAttackers;
	for (i = 0; i < vAttackerClasses.GetCount(); i++)
		{
		int j = 0;
		for (j = 0; j < iAttackerMultiplier; j++)
			{
			int iPlace = i * iAttackerMultiplier + j - iAttackerMultiplier * vAttackerClasses.GetCount() / 2;
			CVector vPos = CVector(-TEAM_SEPARATION_DISTANCE, iPlace * SHIP_SEPARATION_DISTANCE);
			CShip *pAttacker;
			if (pSystem->CreateShip(vAttackerClasses[i]->GetUNID(), NULL, NULL, pAttackerSovereign, vPos, CVector(), 0, NULL, NULL, &pAttacker) != NOERROR)
				{
				printf("ERROR: Unable to create defending ship.\n");
				return resultError;
				}
			vAttackers.Insert(pAttacker);
			}
		}

	//	Set the the ships to attack each other

	if (AttackRandomEnemy(vAttackers, vDefenders) != NOERROR || AttackRandomEnemy(vDefenders, vAttackers) != NOERROR)
		return resultError;

	//	Watch the attacker

	Universe.SetPOV(vAttackers[0]);
	pSystem->SetPOVLRS(vAttackers[0]);

	//	Prepare system

	Universe.GarbageCollectLibraryBitmaps();
	Universe.StartGame(true);

	//	Update context

	SSystemUpdateCtx Ctx;
	Ctx.bForceEventFiring = true;
	Ctx.bForcePainted = true;

	//	Now keep updating until either all the attackers/defenders are destroyed or we time-out

	int iTime = 0;
	int iDoneTime = (Viewer.IsEmpty() ? 0 : DONE_TIME);
	bool bDone = false;

	EResults iResult = resultTimeout;
	while (iTime < iTimeOut && (!bDone || iDoneTime > 0))
		{
		iTime++;
		Universe.Update(Ctx);

		for (i = 0; i < vAttackers.GetCount(); i++)
			if (vAttackers[i]->IsDestroyed())
			{
				vAttackers.Delete(i);
				i -= 1;
			}
		for (i = 0; i < vDefenders.GetCount(); i++)
			if (vDefenders[i]->IsDestroyed())
			{
				vDefenders.Delete(i);
				i -= 1;
			}

		if (!Viewer.IsEmpty())
			Viewer.PaintViewport(Universe);

		if ((!Universe.GetPOV() || !Universe.GetPOV()->AsShip()) && !bDone)
			{
			if (vAttackers.GetCount() > 0)
				{
				Universe.SetPOV(vAttackers[0]);
				pSystem->SetPOVLRS(vAttackers[0]);
				}
			}

		if (AttackRandomEnemy(vAttackers, vDefenders) != NOERROR || AttackRandomEnemy(vDefenders, vAttackers) != NOERROR)
			return resultError;

		if (bDone)
			iDoneTime--;
		else if (vAttackers.GetCount() == 0)
			{
			bDone = true;
			iResult = resultAttackersDestroyed;
			}
		else if (vDefenders.GetCount() == 0)
			{
			bDone = true;
			iResult = resultDefendersDestroyed;
			}
		}
	//	Done
	return iResult;
	}