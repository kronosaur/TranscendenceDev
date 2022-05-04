//	CAscendedObjectList.cpp
//
//	CAscendedObjectList class
//	Copyright (c) 2013 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CAscendedObjectList::CleanUp (void)

//	CleanUp
//
//	Clean up the structure. We own all objects.

	{
	for (int i = 0; i < m_List.GetCount(); i++)
		delete m_List[i];

	m_List.DeleteAll();
	m_Events.DeleteAll();
	}

CSpaceObject *CAscendedObjectList::FindByID (DWORD dwID) const

//	FindByID
//
//	Returns the object by ID

	{
	int i;

	for (i = 0; i < m_List.GetCount(); i++)
		if (m_List[i]->GetID() == dwID)
			return m_List[i];

	return NULL;
	}

void CAscendedObjectList::Insert (CSpaceObject &Obj, CSystemEventList &Events)

//	Insert
//
//	Insert a new object to our list.

	{
	m_List.Insert(&Obj);

	Events.MoveEventForObjTo(Obj, m_Events);
	}

void CAscendedObjectList::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Read
//
//	DWORD		Count
//	CSpaceObject
//	CSystemEventList

	{
	CleanUp();

	DWORD dwCount;
	Ctx.pStream->Read(dwCount);
	m_List.InsertEmpty(dwCount);

	for (int i = 0; i < (int)dwCount; i++)
		{
		CSpaceObject *pObj;
		try
			{
			CSpaceObject::CreateFromStream(Ctx, &pObj);
			}
		catch (...)
			{
			CString sError = CSpaceObject::DebugLoadError(Ctx);
			kernelDebugLogString(sError);

			m_List.Delete(i, (int)dwCount - i);
			CleanUp();
			throw;
			}

		//	Add to list

		m_List[i] = pObj;

		//	Add to our map so that the event list can refer to it.

		Ctx.ObjMap.Insert(pObj->GetID(), pObj);
		}

	//	Read events

	if (Ctx.dwVersion >= 210)
		m_Events.ReadFromStream(Ctx);

	//	Load complete

	for (int i = 0; i < m_List.GetCount(); i++)
		{
		m_List[i]->OnObjLoadComplete(Ctx);
		}
	}

CSpaceObject *CAscendedObjectList::RemoveByID (DWORD dwID, CSystemEventList &retEvents)

//	RemoveByID
//
//	Removes the given object by ID. Returns NULL if not found.

	{
	for (int i = 0; i < m_List.GetCount(); i++)
		if (m_List[i]->GetID() == dwID)
			{
			CSpaceObject *pObj = m_List[i];
			m_List.Delete(i);
			m_Events.MoveEventForObjTo(*pObj, retEvents);

			return pObj;
			}

	return NULL;
	}

void CAscendedObjectList::WriteToStream (IWriteStream *pStream)

//	WriteToStream
//
//	Write

	{
	DWORD dwCount = m_List.GetCount();
	pStream->Write(dwCount);

	for (int i = 0; i < (int)dwCount; i++)
		{
		try
			{
			m_List[i]->WriteToStream(pStream);
			}
		catch (...)
			{
			CString sError = CONSTLIT("Unable to save ascended object:\r\n");
			ReportCrashObj(&sError, m_List[i]);
			kernelDebugLogString(sError);
			return;
			}
		}

	m_Events.WriteToStream(NULL, pStream);
	}
