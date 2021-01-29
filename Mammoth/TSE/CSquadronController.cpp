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
	for (int i = 0; i < Desc.GetCount(); i++)
		{
		CreateInitialShips(SourceObj, Desc.GetSquadron(i));
		}
	}

void CSquadronController::CreateInitialShips (CSpaceObject &SourceObj, const CSquadronDesc &SquadronDesc)

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

	//	Otherwise, we continue creating until we've got enough

	CShipChallengeCtx CreatedSoFar;

	int iMaxLoops = 20;
	while (iMaxLoops-- > 0 && SquadronDesc.GetChallengeDesc().NeedsMoreInitialShips(&SourceObj, CreatedSoFar))
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
				Entry.Squadron.FastAdd(pNewShip);
			}
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
	for (int i = 0; i < Desc.GetCount(); i++)
		{
		Update(Ctx, Desc.GetSquadron(i));
		}
	}

void CSquadronController::Update (SUpdateCtx &Ctx, const CSquadronDesc &Desc)

//	Update
//
//	Updates a single squadron descriptor.

	{
	//	Construction

	if (Desc.GetShipConstructionRate()
			&& !Ctx.bNoConstruction
			&& (Ctx.iTick % Desc.GetShipConstructionRate()) == 0)
		{
		if (const IShipGenerator *pConstruction = Desc.GetConstructionTable())
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

				pConstruction->CreateShips(CreateCtx);

				//	Add new ships to squadron

				for (int i = 0; i < CreateCtx.Result.GetCount(); i++)
					{
					CSpaceObject *pNewShip = CreateCtx.Result.GetObj(i);
					if (pNewShip->GetSquadronID().GetLeaderID() == Ctx.SourceObj.GetID())
						Entry.Squadron.FastAdd(pNewShip);
					}
				}
			}
		}

	//	Get reinforcements

	if ((Ctx.iTick % STATION_REINFORCEMENT_FREQUENCY) == 0
			&& !Ctx.bNoReinforcements)
		{
		if (const IShipGenerator *pReinforcements = Desc.GetReinforcementsTable())
			{
			SSquadronEntry &Entry = SetAt(Desc);

			if (Desc.GetChallengeDesc().NeedsMoreReinforcements(Ctx.SourceObj, Entry.Squadron))
				{
				//	If we've requested several rounds of reinforcements but have
				//	never received any, then it's likely that they are being
				//	destroyed at the gate, so we stop requesting so many

				if (Entry.iReinforceRequestCount > 0)
					{
					int iLongTick = (Ctx.iTick / STATION_REINFORCEMENT_FREQUENCY);
					int iCycle = Min(32, Entry.iReinforceRequestCount * Entry.iReinforceRequestCount);
					if ((iLongTick % iCycle) != 0)
						return;
					}

				//	We either bring in ships from the nearest gate or we build
				//	them ourselves.

				CSpaceObject *pGate;
				if (Desc.BuildsReinforcements()
						|| (pGate = Ctx.SourceObj.GetNearestStargate(true)) == NULL)
					pGate = &Ctx.SourceObj;

				//	Generate reinforcements

				SShipCreateCtx CreateCtx;
				CreateCtx.pSystem = Ctx.SourceObj.GetSystem();
				CreateCtx.pBase = &Ctx.SourceObj;
				CreateCtx.pGate = pGate;
				CreateCtx.SquadronID = CSquadronID(Ctx.SourceObj.GetID(), Entry.sID);
				CreateCtx.dwFlags = SShipCreateCtx::RETURN_RESULT;

				pReinforcements->CreateShips(CreateCtx);

				//	Add new ships to squadron

				for (int i = 0; i < CreateCtx.Result.GetCount(); i++)
					{
					CSpaceObject *pNewShip = CreateCtx.Result.GetObj(i);
					if (pNewShip->GetSquadronID().GetLeaderID() == Ctx.SourceObj.GetID())
						Entry.Squadron.FastAdd(pNewShip);
					}

				//	Increment counter

				Entry.iReinforceRequestCount++;
				}
			else
				Entry.iReinforceRequestCount = 0;
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
		Stream.Write(m_Squadrons[i].iTotalDestroyed);
		}
	}
