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

bool CSystemEventList::CancelEvent (CSpaceObject *pObj, bool bInDoEvent)

//	CancelEvent
//
//	Cancels the given event

	{
	int i;
	bool bFound = false;

	for (i = 0; i < GetCount(); i++)
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
	int i;
	bool bFound = false;

	for (i = 0; i < GetCount(); i++)
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
	int i;
	bool bFound = false;

	for (i = 0; i < GetCount(); i++)
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
	int i;

	for (i = 0; i < m_List.GetCount(); i++)
		delete m_List[i];

	m_List.DeleteAll();
	}

void CSystemEventList::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Reads from a stream
//
//	DWORD		No of events
//	CSystemEvent	Event

	{
	int i;
	DWORD dwCount;

	Ctx.pStream->Read((char *)&dwCount, sizeof(DWORD));
	for (i = 0; i < (int)dwCount; i++)
		{
		CSystemEvent *pEvent;
		CSystemEvent::CreateFromStream(Ctx, &pEvent);
		AddEvent(pEvent);
		}
	}

void CSystemEventList::Update (DWORD dwTick, CSystem *pSystem)

//	Update
//
//	Fire events

	{
	DEBUG_TRY

	int i;

	for (i = 0; i < GetCount(); i++)
		{
		CSystemEvent *pEvent = GetEvent(i);
		SetProgramEvent(pEvent);

		if (!pEvent->IsDestroyed() && pEvent->GetTick() <= dwTick)
			pEvent->DoEvent(dwTick, pSystem);
		}

	SetProgramEvent(NULL);

	//	Delete events that were destroyed

	for (i = 0; i < GetCount(); i++)
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
	int i;

	DWORD dwCount = GetCount();
	pStream->Write((char *)&dwCount, sizeof(DWORD));

	for (i = 0; i < (int)dwCount; i++)
		{
		CSystemEvent *pEvent = GetEvent(i);
		pEvent->WriteToStream(pSystem, pStream);
		}
	}

