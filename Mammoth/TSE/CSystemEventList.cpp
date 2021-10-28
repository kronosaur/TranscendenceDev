//	CSystemEventList.cpp
//
//	CSystemEventList class
//	Copyright (c) 2012 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CSystemEventList::~CSystemEventList (void)

//	CSystemEventList destructor

	{
	DeleteAll();
	}

void CSystemEventList::AddEvent (CSystemEvent *pEvent)

	{
	//	See if we've already got the same event, then we skip.

	if (FindEvent(*pEvent))
		{
		delete pEvent;
		return;
		}

	//	Insert new event.

	m_List.Insert(pEvent);
	}

bool CSystemEventList::CancelEvent (CSpaceObject *pObj, bool bInDoEvent)

//	CancelEvent
//
//	Cancels the given event

	{
	bool bFound = false;

	for (int i = 0; i < GetCount(); i++)
		{
		CSystemEvent *pEvent = GetEvent(i);
		if (pEvent->GetEventHandlerObj() == pObj)
			{
			bFound = true;

			if (bInDoEvent)
				pEvent->SetDestroyed();
			else
				{
				RemoveEvent(i);
				i--;
				}
			}
		}

	return bFound;
	}

bool CSystemEventList::CancelEvent (CSpaceObject *pObj, const CString &sEvent, bool bInDoEvent)

//	CancelEvent
//
//	Cancels the given event

	{
	bool bFound = false;

	for (int i = 0; i < GetCount(); i++)
		{
		CSystemEvent *pEvent = GetEvent(i);
		if (pEvent->GetEventHandlerObj() == pObj 
				&& strEquals(pEvent->GetEventHandlerName(), sEvent))
			{
			bFound = true;

			if (bInDoEvent)
				pEvent->SetDestroyed();
			else
				{
				RemoveEvent(i);
				i--;
				}
			}
		}

	return bFound;
	}

bool CSystemEventList::CancelEvent (CDesignType *pType, const CString &sEvent, bool bInDoEvent)

//	CancelEvent
//
//	Cancels the given event

	{
	bool bFound = false;

	for (int i = 0; i < GetCount(); i++)
		{
		CSystemEvent *pEvent = GetEvent(i);
		if (pEvent->GetEventHandlerType() == pType 
				&& strEquals(pEvent->GetEventHandlerName(), sEvent))
			{
			bFound = true;

			if (bInDoEvent)
				pEvent->SetDestroyed();
			else
				{
				RemoveEvent(i);
				i--;
				}
			}
		}

	return bFound;
	}

void CSystemEventList::DeleteAll (void)

//	DeleteAll
//
//	Delete all events

	{
	for (int i = 0; i < m_List.GetCount(); i++)
		delete m_List[i];

	m_List.DeleteAll();
	}

bool CSystemEventList::FindEvent (CSystemEvent &Src, int *retiIndex) const

//	FindEvent
//
//	Returns TRUE if the given event already exists.

	{
	for (int i = 0; i < m_List.GetCount(); i++)
		if (m_List[i]->IsEqual(Src))
			{
			if (retiIndex)
				*retiIndex = i;
			return true;
			}

	return false;
	}

TArray<CSystemEvent *> CSystemEventList::FindEventsForObj (const CSpaceObject &Obj) const

//	FindEventsForObj
//
//	Returns all events for the given object.

	{
	TArray<CSystemEvent *> Result;

	for (int i = 0; i < GetCount(); i++)
		{
		CSystemEvent *pEvent = GetEvent(i);
		if (pEvent->GetEventHandlerObj() == Obj)
			Result.Insert(pEvent);
		}

	return Result;
	}

void CSystemEventList::MoveEventForObjTo (const CSpaceObject &Obj, CSystemEventList &Dest)

//	MoveEventForObjTo
//
//	Moves all events for the given object to the destination.

	{
	for (int i = 0; i < GetCount(); i++)
		{
		CSystemEvent *pEvent = GetEvent(i);
		if (pEvent->OnObjChangedSystems(Obj))
			{
			MoveEvent(i, Dest);
			i--;
			}
		}
	}

void CSystemEventList::OnObjDestroyed (CSpaceObject *pObj)

//	OnObjDestroyed
//
//	An object has been destroyed

	{
	for (int i = 0; i < GetCount(); i++)
		{
		CSystemEvent *pEvent = GetEvent(i);
		if (pEvent->OnObjDestroyed(pObj))
			pEvent->SetDestroyed();
		}
	}

void CSystemEventList::OnPlayerChangedShips (CSpaceObject &OldShip, CSpaceObject &NewShip, SPlayerChangedShipsCtx &Options)

//	OnPlayerChangedShips
//
//	Player has changed ships.

	{
	for (int i = 0; i < GetCount(); i++)
		{
		CSystemEvent *pEvent = GetEvent(i);
		pEvent->OnPlayerChangedShips(OldShip, NewShip, Options);
		}
	}

void CSystemEventList::OnStationDestroyed (CSpaceObject *pObj)

//	OnStationDestroyed
//
//	A station has been destroyed.

	{
	for (int i = 0; i < GetCount(); i++)
		{
		CSystemEvent *pEvent = GetEvent(i);
		if (pEvent->OnStationDestroyed(pObj))
			pEvent->SetDestroyed();
		}
	}

void CSystemEventList::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Reads from a stream
//
//	DWORD		No of events
//	CSystemEvent	Event

	{
	DWORD dwCount;

	Ctx.pStream->Read((char *)&dwCount, sizeof(DWORD));
	for (int i = 0; i < (int)dwCount; i++)
		{
		CSystemEvent *pEvent;
		CSystemEvent::CreateFromStream(Ctx, &pEvent);
		m_List.Insert(pEvent);
		}
	}

void CSystemEventList::Update (DWORD dwTick, CSystem &System)

//	Update
//
//	Fire events

	{
	DEBUG_TRY

	for (int i = 0; i < GetCount(); i++)
		{
		CSystemEvent *pEvent = GetEvent(i);
		SetProgramEvent(pEvent);

		if (!pEvent->IsDestroyed() && pEvent->GetTick() <= dwTick)
			pEvent->DoEvent(dwTick, System);
		}

	SetProgramEvent(NULL);

	//	Delete events that were destroyed

	for (int i = 0; i < GetCount(); i++)
		{
		CSystemEvent *pEvent = GetEvent(i);
		if (pEvent->IsDestroyed())
			{
			RemoveEvent(i);
			i--;
			}
		}

	DEBUG_CATCH
	}

void CSystemEventList::WriteToStream (CSystem *pSystem, IWriteStream *pStream)

//	WriteToStream
//
//	Writes to a stream
//
//	DWORD		No of events
//	CSystemEvent	Event

	{
	DWORD dwCount = GetCount();
	pStream->Write(dwCount);

	for (int i = 0; i < (int)dwCount; i++)
		{
		CSystemEvent *pEvent = GetEvent(i);
		pEvent->WriteToStream(pSystem, pStream);
		}
	}

