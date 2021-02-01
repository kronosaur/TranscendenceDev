//	CSquadronController.cpp
//
//	CSquadronController class
//	Copyright (c) 2021 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CSquadronController::CreateInitialShips (CSpaceObject &SourceObj, const CSquadronDescList &Desc)

//	CreateInitial
//
//	Creates the initial ships.

	{
	bool bDebugOutput = Desc.IsDebug() && SourceObj.GetUniverse().InDebugMode();

	for (int i = 0; i < Desc.GetCount(); i++)
		{
		CreateInitialShips(SourceObj, Desc.GetSquadron(i), bDebugOutput);
		}
	}

void CSquadronController::CreateInitialShips (CSpaceObject &SourceObj, const CSquadronDesc &SquadronDesc, bool bDebug)

//	CreateInitialShips
//
//	Creates a new squadron.

	{
	const IShipGenerator *pGenerator = SquadronDesc.GetInitialShips();
	if (!pGenerator)
		return;

	SSquadronEntry &Entry = SetAt(SquadronDesc);

	SShipCreateCtx Ctx;

	Ctx.pSystem = SourceObj.GetSystem();
	Ctx.SquadronID = CSquadronID(SourceObj.GetID(), SquadronDesc.GetID());
	Ctx.pBase = &SourceObj;
	Ctx.vPos = SourceObj.GetPos();
	//	1d8+1 light-second spread
	Ctx.PosSpread = DiceRange(8, 1, 1);
	Ctx.dwFlags = SShipCreateCtx::SHIPS_FOR_STATION | SShipCreateCtx::RETURN_RESULT;

	CShipChallengeCtx CreatedSoFar;

	int iMaxLoops = 20;
	while (iMaxLoops-- > 0 && SquadronDesc.GetChallengeDesc().NeedsMoreShips(SourceObj, CreatedSoFar))
		{
		//	These accumulate, so we need to clear it each time.

		Ctx.Result.DeleteAll();

		//	Create the ships.

		pGenerator->CreateShips(Ctx);

		//	Keep track of the ships we created.

		CreatedSoFar.AddShips(Ctx.Result);

		//	Loop over all ships we created and add the principals (not escorts)
		//	to our squadron list.

		for (int i = 0; i < Ctx.Result.GetCount(); i++)
			{
			CSpaceObject *pNewShip = Ctx.Result.GetObj(i);
			if (pNewShip->GetSquadronID().GetLeaderID() == SourceObj.GetID())
				{
				Entry.Squadron.FastAdd(pNewShip);

				DebugOutput(SourceObj, strPatternSubst(CONSTLIT("Created %s [%d]."), pNewShip->GetNounPhrase(), pNewShip->GetID()), &Entry, bDebug);
				}
			}
		}
	}

void CSquadronController::CreateReinforcements (CSpaceObject &SourceObj, const CSquadronDesc &SquadronDesc, SSquadronEntry &Entry, const IShipGenerator &ShipTable, CSpaceObject &GateObj, bool bDebug)

//	CreateReinforcements
//
//	Creates reinforcements.

	{
	SShipCreateCtx CreateCtx;
	CreateCtx.pSystem = SourceObj.GetSystem();
	CreateCtx.pBase = &SourceObj;
	CreateCtx.pGate = &GateObj;
	CreateCtx.SquadronID = CSquadronID(SourceObj.GetID(), Entry.sID);
	CreateCtx.dwFlags = SShipCreateCtx::RETURN_RESULT;

	CShipChallengeCtx CreatedSoFar(Entry.Squadron);

	int iMaxLoops = 20;
	do
		{
		//	These accumulate, so we need to clear it each time.

		CreateCtx.Result.DeleteAll();

		//	Create the ships.

		ShipTable.CreateShips(CreateCtx);

		//	Keep track of the ships we created.

		CreatedSoFar.AddShips(CreateCtx.Result);

		//	Loop over all ships we created and add the principals (not escorts)
		//	to our squadron list.

		for (int i = 0; i < CreateCtx.Result.GetCount(); i++)
			{
			CSpaceObject *pNewShip = CreateCtx.Result.GetObj(i);
			if (pNewShip->GetSquadronID().GetLeaderID() == SourceObj.GetID())
				{
				Entry.Squadron.FastAdd(pNewShip);

				DebugOutput(SourceObj, strPatternSubst(CONSTLIT("Summoned %s [%d]."), pNewShip->GetNounPhrase(), pNewShip->GetID()), &Entry, bDebug);
				}
			}
		}
	while (--iMaxLoops > 0 && SquadronDesc.GetChallengeDesc().NeedsMoreReinforcements(SourceObj, CreatedSoFar, SquadronDesc.GetReinforceDesc()));

	//	Increment counters

	Entry.dwLastReinforcementRequestOn = SourceObj.GetUniverse().GetTicks();
	Entry.iTotalReinforceCount++;
	Entry.iReinforceRequestCount++;
	}

void CSquadronController::DebugOutput (CSpaceObject &SourceObj, const CString &sLine, const SSquadronEntry *pEntry, bool bDebug)

//	LogOutput
//
//	Logs debug output.

	{
	if (bDebug)
		{
		const CSystem *pSystem = SourceObj.GetSystem();
		const CTopologyNode *pNode = (pSystem ? pSystem->GetTopology() : NULL);
		CString sNodeID = (pNode ? pNode->GetID() : CONSTLIT("NONE"));

		CString sID = (pEntry ? pEntry->sID : CONSTLIT("*"));

		SourceObj.GetUniverse().GetHost()->ConsoleOutput(strPatternSubst(CONSTLIT("%s %d %s: %s"), sNodeID, SourceObj.GetID(), sID, sLine));
		}
	}

void CSquadronController::FixupDefenders (CSpaceObject &SourceObj, const CSquadronDescList &Desc, const CSpaceObjectList &Subordinates)

//	FixupDefenders
//
//	When loading an old version, find guard and patrols and assign them to a
//	squadron.

	{
	for (int i = 0; i < Subordinates.GetCount(); i++)
		{
		CSpaceObject *pObj = Subordinates.GetObj(i);
		CShip *pShip = pObj->AsShip();
		if (!pShip || pShip->GetBase() != SourceObj || !pShip->CanAttack())
			continue;

		//	Find which entry we should match.

		const CSquadronDesc *pSquadronDesc = Desc.FindMatchingSquadron(*pShip->GetClass());
		if (!pSquadronDesc)
			continue;

		//	Set the squadron ID

		pShip->SetSquadronID(CSquadronID(SourceObj.GetID(), pSquadronDesc->GetID()));

		//	Add the ship to the squadron

		SSquadronEntry &Entry = SetAt(*pSquadronDesc);
		Entry.Squadron.Add(pShip);
		}
	}

CSquadronController::SSquadronEntry *CSquadronController::GetAt (const CString &sID)

//	GetAt
//
//	Looks for an entry by ID.

	{
	for (int i = 0; i < m_Squadrons.GetCount(); i++)
		if (strEquals(m_Squadrons[i].sID, sID))
			return &m_Squadrons[i];

	return NULL;
	}

int CSquadronController::GetReinforceRequestCount () const

//	GetReinforceRequestCount
//
//	Returns the total number of times we've requested reinforcements but not
//	yet satisfied the request.

	{
	int iTotal = 0;

	for (int i = 0; i < m_Squadrons.GetCount(); i++)
		iTotal += m_Squadrons[i].iReinforceRequestCount;

	return iTotal;
	}

ICCItemPtr CSquadronController::GetStatus (const CSpaceObject &SourceObj) const

//	GetStatus
//
//	Returns current status. We return a struct indexed by squadron ID. For each
//	squadron we include:
//
//		squadron: List of objects in the squadron
//		shipTable: List of ship classes in table
//		desc: A struct describing reinforcement policy.
//		lastReinforcementRequestOn: Tick on which we last requested
//				reinforcements.
//		reinforcementRequestCount: Consecutive request counts
//		totalDestroyed: Number of ships destroyed.

	{
	ICCItemPtr pResult(ICCItem::SymbolTable);

	for (int i = 0; i < m_Squadrons.GetCount(); i++)
		{
		ICCItemPtr pEntry(ICCItem::SymbolTable);

		//	Squadron

		ICCItemPtr pShipList(ICCItem::List);
		for (int j = 0; j < m_Squadrons[i].Squadron.GetCount(); j++)
			{
			pShipList->Append(CTLispConvert::CreateObject(m_Squadrons[i].Squadron.GetObj(j)));
			}
		pEntry->SetAt(CONSTLIT("squadron"), pShipList);

		//	Other

		pEntry->SetIntegerAt(CONSTLIT("lastReinforcementRequestOn"), m_Squadrons[i].dwLastReinforcementRequestOn);
		pEntry->SetIntegerAt(CONSTLIT("reinforcementRequestCount"), m_Squadrons[i].iReinforceRequestCount);
		pEntry->SetIntegerAt(CONSTLIT("totalDestroyed"), m_Squadrons[i].iTotalDestroyed);

		//	Descriptor elements

		if (m_Squadrons[i].pDesc)
			{
			if (const IShipGenerator *pGenerator = m_Squadrons[i].pDesc->GetInitialShips())
				pEntry->SetAt(CONSTLIT("shipTable"), pGenerator->GetShipsReferenced(SourceObj.GetUniverse()));

			pEntry->SetAt(CONSTLIT("shipDesc"), m_Squadrons[i].pDesc->GetChallengeDesc().GetDesc(&SourceObj));
			pEntry->SetAt(CONSTLIT("reinforceDesc"), m_Squadrons[i].pDesc->GetReinforceDesc().GetDesc(&SourceObj));
			}

		//	Add it

		pResult->SetAt(m_Squadrons[i].sID, pEntry);
		}

	return pResult;
	}

void CSquadronController::OnObjDestroyed (CSpaceObject &SourceObj, const SDestroyCtx &Ctx)

//	OnObjDestroyed
//
//	An object was destroyed and we need to remove it from our lists.

	{
	const CSquadronID &SquadronID = Ctx.Obj.GetSquadronID();
	if (SquadronID.GetLeaderID() != SourceObj.GetID())
		return;

	//	Look for the squadron ID

	SSquadronEntry *pEntry = GetAt(SquadronID.GetID());
	if (!pEntry)
		return;

	if (pEntry->Squadron.Delete(&Ctx.Obj))
		pEntry->iTotalDestroyed++;
	}

void CSquadronController::ReadFromStream (SLoadCtx &Ctx, const CSquadronDescList &Desc)

//	ReadFromStream
//
//	DWORD				Count
//
//	CString				sID
//	CSpaceObjectList	Squadron
//	DWORD				dwLastReinforceRequestOn
//	DWORD				iReinforceRequestCount
//	DWORD				iTotalReinforceCount
//	DWORD				iTotalDestroyed

	{
	DWORD dwLoad;
	Ctx.pStream->Read(dwLoad);
	m_Squadrons.InsertEmpty(dwLoad);

	for (int i = 0; i < m_Squadrons.GetCount(); i++)
		{
		m_Squadrons[i].sID.ReadFromStream(Ctx.pStream);
		m_Squadrons[i].Squadron.ReadFromStream(Ctx, true);
		Ctx.pStream->Read(m_Squadrons[i].dwLastReinforcementRequestOn);
		Ctx.pStream->Read(m_Squadrons[i].iReinforceRequestCount);
		Ctx.pStream->Read(m_Squadrons[i].iTotalReinforceCount);
		Ctx.pStream->Read(m_Squadrons[i].iTotalDestroyed);

		//	NOTE: This might come back NULL if we changed the squadron 
		//	definitions between games.

		m_Squadrons[i].pDesc = Desc.FindSquadron(m_Squadrons[i].sID);
		}
	}

CSquadronController::SSquadronEntry &CSquadronController::SetAt (const CSquadronDesc &SquadronDesc)

//	SetAt
//
//	Returns the entry associated with the given descriptor (creating a new one
//	if necessary).
//
//	NOTE: The pointer returned by this function becomes invalid if we add or
//	remove any entries.

	{
	for (int i = 0; i < m_Squadrons.GetCount(); i++)
		if (m_Squadrons[i].pDesc == &SquadronDesc)
			return m_Squadrons[i];

	SSquadronEntry *pNewEntry = m_Squadrons.Insert();
	pNewEntry->sID = SquadronDesc.GetID();
	pNewEntry->pDesc = &SquadronDesc;

	return *pNewEntry;
	}

void CSquadronController::SetReinforceRequestCount (int iValue)

//	SetReinforceRequestCount
//
//	Used for backward compatibility.

	{
	}

void CSquadronController::Update (SUpdateCtx &Ctx, const CSquadronDescList &Desc)

//	Update
//
//	Called during update.

	{
	//	Normal reinforcements

	if ((Ctx.iTick % STATION_REINFORCEMENT_FREQUENCY) == 0
			&& !Ctx.bNoReinforcements)
		{
		bool bDebugOutput = Desc.IsDebug() && Ctx.SourceObj.GetUniverse().InDebugMode();

		for (int i = 0; i < Desc.GetCount(); i++)
			Update(Ctx, Desc.GetSquadron(i), bDebugOutput);
		}

	//	Backwards compatible construction

	const IShipGenerator *pConstructionTable;
	int iConstructionRate;
	if (Desc.GetCount() == 1 
			&& (pConstructionTable = Desc.GetSquadron(0).GetConstructionTable())
			&& (iConstructionRate = Desc.GetSquadron(0).GetShipConstructionRate())
			&& ((Ctx.iTick % iConstructionRate) == 0)
			&& !Ctx.bNoConstruction)
		{
		UpdateConstruction(Ctx, Desc.GetSquadron(0), *pConstructionTable);
		}
	}

void CSquadronController::Update (SUpdateCtx &Ctx, const CSquadronDesc &Desc, bool bDebug)

//	Update
//
//	Updates a single squadron descriptor.

	{
	const IShipGenerator *pReinforcements = Desc.GetReinforcementsTable();
	if (!pReinforcements)
		{
		DebugOutput(Ctx.SourceObj, CONSTLIT("No reinforcements table."), NULL, bDebug);
		return;
		}

	SSquadronEntry &Entry = SetAt(Desc);

	//	Short-circuit certain cases.

	if (Desc.GetReinforceLimit() && Entry.iTotalReinforceCount >= Desc.GetReinforceLimit())
		{
		DebugOutput(Ctx.SourceObj, strPatternSubst(CONSTLIT("No more reinforcements due to limit: %d"), Desc.GetReinforceLimit()), &Entry, bDebug);
		}
	else if (Entry.dwLastReinforcementRequestOn 
			&& Entry.dwLastReinforcementRequestOn + Desc.GetReinforceInterval() > Ctx.SourceObj.GetUniverse().GetTicks())
		{
		DWORD dwTicksToWait = Entry.dwLastReinforcementRequestOn + Desc.GetReinforceInterval() - Ctx.SourceObj.GetUniverse().GetTicks();
		DebugOutput(Ctx.SourceObj, strPatternSubst(CONSTLIT("Waiting for reinforce interval: %d ticks"), dwTicksToWait), &Entry, bDebug);
		}

	//	If we have successfully received some reinforcements, reset the counter
	//	because it means at least some reinforcements are getting through.

	else if (!Desc.GetChallengeDesc().NeedsMoreReinforcements(Ctx.SourceObj, Entry.Squadron, Desc.GetReinforceDesc()))
		{
		//	We have successfully received some reinforcements, so reset the 
		//	counter.

		Entry.iReinforceRequestCount = 0;

		DebugOutput(Ctx.SourceObj, CONSTLIT("No reinforcements needed."), &Entry, bDebug);
		}

	//	Request reinforcements.

	else
		{
		//	If we've requested several rounds of reinforcements but have
		//	never received any, then it's likely that they are being
		//	destroyed at the gate, so we stop requesting so many

		if (Entry.iReinforceRequestCount > 0)
			{
			int iLongTick = (Ctx.iTick / STATION_REINFORCEMENT_FREQUENCY);
			int iCycle = Min(32, Entry.iReinforceRequestCount * Entry.iReinforceRequestCount);
			if ((iLongTick % iCycle) != 0)
				{
				DebugOutput(Ctx.SourceObj, CONSTLIT("No reinforcements due to too many losses."), &Entry, bDebug);
				return;
				}
			}

		//	We either bring in ships from the nearest gate or we build
		//	them ourselves.

		CSpaceObject *pGate;
		if (Desc.BuildsReinforcements()
				|| (pGate = Ctx.SourceObj.GetNearestStargate(true)) == NULL)
			pGate = &Ctx.SourceObj;

		//	Generate reinforcements

		CreateReinforcements(Ctx.SourceObj, Desc, Entry, *pReinforcements, *pGate, bDebug);
		}
	}

void CSquadronController::UpdateConstruction (SUpdateCtx &Ctx, const CSquadronDesc &Desc, const IShipGenerator &ConstructionTable)

//	UpdateConstruction
//
//	Updates backwards compatible construction.

	{
	SSquadronEntry &Entry = SetAt(Desc);

	//	Iterate over all ships and count the number that are
	//	associated with the station.

	int iCount = Entry.Squadron.GetCount();

	//	If we already have the maximum number, then don't bother

	if (iCount < Desc.GetShipConstructionMax())
		{
		SShipCreateCtx CreateCtx;
		CreateCtx.pSystem = Ctx.SourceObj.GetSystem();
		CreateCtx.pBase = &Ctx.SourceObj;
		CreateCtx.pGate = &Ctx.SourceObj;
		CreateCtx.SquadronID = CSquadronID(Ctx.SourceObj.GetID(), Entry.sID);
		CreateCtx.dwFlags = SShipCreateCtx::RETURN_RESULT;

		ConstructionTable.CreateShips(CreateCtx);

		//	Add new ships to squadron

		for (int i = 0; i < CreateCtx.Result.GetCount(); i++)
			{
			CSpaceObject *pNewShip = CreateCtx.Result.GetObj(i);
			if (pNewShip->GetSquadronID().GetLeaderID() == Ctx.SourceObj.GetID())
				Entry.Squadron.FastAdd(pNewShip);
			}
		}
	}

void CSquadronController::WriteToStream (IWriteStream &Stream) const

//	WriteToStream
//	DWORD				Count
//
//	CString				sID
//	CSpaceObjectList	Squadron
//	DWORD				dwLastReinforceRequestOn
//	DWORD				iReinforceRequestCount
//	DWORD				iTotalReinforceCount
//	DWORD				iTotalDestroyed

	{
	DWORD dwSave = m_Squadrons.GetCount();
	Stream.Write(dwSave);

	for (int i = 0; i < m_Squadrons.GetCount(); i++)
		{
		m_Squadrons[i].sID.WriteToStream(&Stream);
		m_Squadrons[i].Squadron.WriteToStream(&Stream);
		Stream.Write(m_Squadrons[i].dwLastReinforcementRequestOn);
		Stream.Write(m_Squadrons[i].iReinforceRequestCount);
		Stream.Write(m_Squadrons[i].iTotalReinforceCount);
		Stream.Write(m_Squadrons[i].iTotalDestroyed);
		}
	}
