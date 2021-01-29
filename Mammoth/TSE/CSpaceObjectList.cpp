//	CSpaceObjectList.cpp
//
//	CSpaceObjectList class

#include "PreComp.h"

#define ALLOC_GRANULARITY				64

CSpaceObjectList::CSpaceObjectList (void)

//	CSpaceObjectList constructor

	{
	}

CSpaceObjectList::~CSpaceObjectList (void)

//	CSpaceObjectList destructor

	{
	}

void CSpaceObjectList::Add (CSpaceObject *pObj, int *retiIndex)

//	Add
//
//	Add object to the list. If the object is already in the list, we
//	don't add it.

	{
	if (FindObj(pObj))
		return;

	FastAdd(pObj, retiIndex);
	}

void CSpaceObjectList::Add (const CSpaceObjectList &List)

//	Add
//
//	Add objects to the list. If the object is already in the list, we
//	don't add it.

	{
	int i;

	for (i = 0; i < List.GetCount(); i++)
		{
		Add(List.GetObj(i));
		}
	}

bool CSpaceObjectList::Delete (CSpaceObject *pObj)

//	Delete
//
//	Remove the object. Returns TRUE if we removed the object.

	{
	int iIndex;
	if (FindObj(pObj, &iIndex))
		{
		Delete(iIndex);
		return true;
		}

	return false;
	}

void CSpaceObjectList::DeleteSystemObjs (void)

//	DeleteSystemObjs
//
//	Remove objects that are part of the current system.

	{
	int i;

	for (i = 0; i < m_List.GetCount(); i++)
		{
		CSpaceObject *pObj = m_List[i];
		if (!pObj->IsNonSystemObj())
			{
			Delete(i);
			i--;
			}
		}
	}

void CSpaceObjectList::FastAdd (const CSpaceObjectList &List)

//	FastAdd
//
//	Adds objects to the list without checking to see if they already exist.

	{
	int i;

	for (i = 0; i < List.GetCount(); i++)
		{
		FastAdd(List.GetObj(i));
		}
	}

void CSpaceObjectList::NotifyOnObjDestroyed (SDestroyCtx &Ctx)

//	NotifyOnObjDestroyed
//
//	Notify all objects in the list that another object was destroyed.

	{
	DEBUG_TRY

	if (GetCount() > 0)
		{
		//	We make a copy of our list because an event might remove an object
		//	from our list.

 		TArray<CSpaceObject *> List(m_List);

		for (int i = 0; i < List.GetCount(); i++)
			{
			CSpaceObject *pObj = List[i];
			if (!pObj->IsDestroyed())
				pObj->OnObjDestroyedNotify(Ctx);
			}
		}

	DEBUG_CATCH
	}

void CSpaceObjectList::NotifyOnObjDocked (CSpaceObject *pDockingObj, CSpaceObject *pDockTarget)

//	NotifyOnObjDocked
//
//	Notify that an object docked.

	{
	int i;

	if (GetCount() > 0)
		{
		TArray<CSpaceObject *> List(m_List);

		for (i = 0; i < List.GetCount(); i++)
			{
			CSpaceObject *pObj = List[i];

			//	NOTE: We do not notify the dock target because it got notified
			//	separately.

			if (!pObj->IsDestroyed() 
					&& pObj != pDockTarget 
					&& pObj->HasOnObjDockedEvent())
				pObj->FireOnObjDocked(pDockingObj, pDockTarget);
			}
		}
	}

void CSpaceObjectList::NotifyOnObjEnteredGate (CSpaceObject *pGatingObj, CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate)

//	NotifyOnObjEnteredGate
//
//	Notify all objects in the list that the given object has entered a stargate.

	{
	int i;

	if (GetCount() > 0)
		{
		TArray<CSpaceObject *> List(m_List);

		for (i = 0; i < List.GetCount(); i++)
			{
			CSpaceObject *pObj = List[i];
			if (!pObj->IsDestroyed())
				pObj->FireOnObjEnteredGate(pGatingObj, pDestNode, sDestEntryPoint, pStargate);
			}
		}
	}

bool CSpaceObjectList::NotifyOnObjGateCheck (CSpaceObject *pGatingObj, CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pGateObj)

//	NotifyOnObjGateCheck
//
//	Ask all objects if they allow the given object to gate. We guarantee that
//	all objects are called.

	{
	int i;
	bool bResult = true;

	if (GetCount() > 0)
		{
		TArray<CSpaceObject *> List(m_List);

		for (i = 0; i < List.GetCount(); i++)
			{
			CSpaceObject *pObj = List[i];
			if (pObj->IsDestroyed())
				continue;

			if (!pObj->FireOnObjGateCheck(pGatingObj, pDestNode, sDestEntryPoint, pGateObj))
				bResult = false;
			}
		}

	return bResult;
	}

void CSpaceObjectList::NotifyOnObjJumped (CSpaceObject *pJumpObj)

//	NotifyOnObjJumped
//
//	Notify that an object jumped.

	{
	int i;

	if (GetCount() > 0)
		{
		TArray<CSpaceObject *> List(m_List);

		for (i = 0; i < List.GetCount(); i++)
			{
			CSpaceObject *pObj = List[i];
			if (!pObj->IsDestroyed())
				pObj->FireOnObjJumped(pJumpObj);
			}
		}
	}

void CSpaceObjectList::NotifyOnObjReconned (CSpaceObject *pReconnedObj)

//	NotifyOnObjReconned
//
//	Notify that an object was reconned

	{
	int i;

	if (GetCount() > 0)
		{
		TArray<CSpaceObject *> List(m_List);

		for (i = 0; i < List.GetCount(); i++)
			{
			CSpaceObject *pObj = List[i];
			if (!pObj->IsDestroyed())
				pObj->FireOnObjReconned(pReconnedObj);
			}
		}
	}

void CSpaceObjectList::NotifyOnPlayerBlacklisted (CSpaceObject *pBlacklistingObj)

//	NotifyOnPlayerBlacklisted
//
//	Notify all objects that the player was blacklisted

	{
	int i;

	if (GetCount() > 0)
		{
		TArray<CSpaceObject *> List(m_List);

		for (i = 0; i < List.GetCount(); i++)
			{
			CSpaceObject *pObj = List[i];
			if (!pObj->IsDestroyed())
				pObj->FireOnObjBlacklistedPlayer(pBlacklistingObj);
			}
		}
	}

void CSpaceObjectList::ReadFromStream (SLoadCtx &Ctx, bool bIgnoreMissing)

//	ReadFromStream
//
//	Read the list from a stream

	{
	int i;
	ASSERT(m_List.GetCount() == 0);

	DWORD dwCount;
	Ctx.pStream->Read((char *)&dwCount, sizeof(DWORD));
	if (dwCount == 0)
		return;

	//	If we expect all objects to be resolved, then we create the array and
	//	wait for them to be loaded.

	if (!bIgnoreMissing)
		{
		m_List.InsertEmpty(dwCount);

		for (i = 0; i < (int)dwCount; i++)
			CSystem::ReadObjRefFromStream(Ctx, &m_List[i]);
		}

	//	Otherwise, we use the callback method, which allows us to ignore any
	//	unresolved entries.

	else
		{
		for (i = 0; i < (int)dwCount; i++)
			CSystem::ReadObjRefFromStream(Ctx, this, &ResolveObjProc);
		}
	}

void CSpaceObjectList::ReadFromStreamSingle (SLoadCtx &Ctx)

//	ReadFromStreamSingle
//
//	Read a single object into the list (generally used for backwards compatibility)

	{
	ASSERT(m_List.GetCount() == 0);

	m_List.InsertEmpty(1);
	CSystem::ReadObjRefFromStream(Ctx, &m_List[0]);
	}

void CSpaceObjectList::ResolveObjProc (void *pCtx, DWORD dwObjID, CSpaceObject *pObj)

//	ResolveObjProc
//
//	Resolve an object reference

	{
	CSpaceObjectList *pList = (CSpaceObjectList *)pCtx;
	if (pObj && dwObjID && !pObj->IsDestroyed())
		pList->m_List.Insert(pObj);
	}

void CSpaceObjectList::SetAllocSize (int iNewCount)

//	SetAllocSize
//
//	Sets the size of the buffer in preparation for adding objects.
//	NOTE that this also empties the list.

	{
	m_List.DeleteAll();
	m_List.GrowToFit(iNewCount);
	}

void CSpaceObjectList::Subtract (const CSpaceObjectList &List)

//	Subtract
//
//	Removes all objects in List from the current list

	{
	int i;

	//	Mark all current objects

	int iCount = GetCount();
	for (i = 0; i < iCount; i++)
		GetObj(i)->SetMarked(true);

	//	Clear marks on all objects to remove

	for (i = 0; i < List.GetCount(); i++)
		List.GetObj(i)->SetMarked(false);

	//	Create a new list with the remaining objects

	TArray<CSpaceObject *> NewList;
	for (i = 0; i < iCount; i++)
		if (GetObj(i)->IsMarked())
			NewList.Insert(GetObj(i));

	m_List.TakeHandoff(NewList);
	}

void CSpaceObjectList::WriteToStream (IWriteStream *pStream) const

//	WriteToStream
//
//	Write list to stream

	{
	int iCount = GetCount();
	pStream->Write((char *)&iCount, sizeof(int));

	for (int i = 0; i < iCount; i++)
		CSystem::WriteObjRefToStream(*pStream, m_List[i]);
	}
