//	CObjectTracker.cpp
//
//	CObjectTracker class

#include "PreComp.h"

#define LANGID_DESC_GALACTIC_MAP                CONSTLIT("core.mapDesc")
#define LANGID_DESC_GALACTIC_MAP_ABANDONED      CONSTLIT("core.mapDescAbandoned")
#define LANGID_DESC_GALACTIC_MAP_CUSTOM         CONSTLIT("core.mapDescCustom")
#define LANGID_DESC_GALACTIC_MAP_ABANDONED_CUSTOM CONSTLIT("core.mapDescAbandonedCustom")

const int MAX_ALLOC_GRANULARITY =			10000;

CObjectTracker::~CObjectTracker (void)

//	CObjectTracker destructor

	{
	DeleteAll();
	}

bool CObjectTracker::AccumulateEntries (TArray<SObjList *> &Table, const CDesignTypeCriteria &Criteria, TArray<SObjEntry> *retResult)

//	AccumulateEntries
//
//	Looks for objects that match the criteria and adds them to the result list.

	{
	int i;
	int j;

	for (i = 0; i < Table.GetCount(); i++)
		{
		SObjList *pList = Table[i];

		//	If we don't match, then continue

		if (!pList->pType->MatchesCriteria(Criteria))
			continue;

		//	If all we care about is whether we have any entries, then we're done.

		if (retResult == NULL)
			return true;

		//	Otherwise, add all objects to the results

        retResult->GrowToFit(pList->Objects.GetCount());
		for (j = 0; j < pList->Objects.GetCount(); j++)
            AccumulateEntry(*pList, pList->Objects.GetKey(j), pList->Objects[j], *retResult);
		}

	//	Done

	return (retResult && retResult->GetCount() > 0);
	}

void CObjectTracker::AccumulateEntry (const SObjList &ObjList, DWORD dwObjID, const SObjBasics &ObjData, TArray<SObjEntry> &Results) const

//  AccumulateEntry
//
//  Adds the given entry to the list

    {
	SObjEntry *pEntry = Results.Insert();
	pEntry->pNode = ObjList.pNode;
	pEntry->pType = ObjList.pType;
    pEntry->dwObjID = dwObjID;
    pEntry->fKnown = ObjData.fKnown;
    pEntry->fShowDestroyed = ObjData.fShowDestroyed;
    pEntry->fShowInMap = ObjData.fShowInMap;
    pEntry->fFriendly = ObjData.fFriendly;
    pEntry->fEnemy = ObjData.fEnemy;

	if (ObjData.pExtra)
		{
		pEntry->sName = ObjData.pExtra->sName;
		pEntry->dwNameFlags = ObjData.pExtra->dwNameFlags;
        pEntry->ImageSel = ObjData.pExtra->ImageSel;
        pEntry->sNotes = ObjData.pExtra->sNotes;
		}
	else
		pEntry->sName = ObjList.pType->GetTypeName(&pEntry->dwNameFlags);

    //  If we've got custom notes stored with the object, then use those.

    if (!pEntry->sNotes.IsBlank())
        ;

    //  Otherwise, we let the type compose it

    else
        {
        CDesignType::SMapDescriptionCtx Ctx;
        Ctx.bShowDestroyed = pEntry->fShowDestroyed;
        Ctx.bEnemy = pEntry->fEnemy;
        Ctx.bFriend = pEntry->fFriendly;

        pEntry->sNotes = pEntry->pType->GetMapDescription(Ctx);
        if (pEntry->sNotes.IsBlank())
            pEntry->sNotes = CONSTLIT("No services available");
        }
    }

void CObjectTracker::Delete (CSpaceObject *pObj)

//	Delete
//
//	Delete an object

	{
	SObjList *pList = GetList(pObj);
	if (pList == NULL)
		return;

	DWORD dwID = pObj->GetID();
    pList->Objects.DeleteAt(dwID);
	}

void CObjectTracker::DeleteAll (void)

//	DeleteAll
//
//	Delete all entries

	{
	int i;

	for (i = 0; i < m_AllLists.GetCount(); i++)
		delete m_AllLists[i];

	m_AllLists.DeleteAll();
	m_ByNode.DeleteAll();
	}

bool CObjectTracker::Find (const CString &sNodeID, const CDesignTypeCriteria &Criteria, TArray<SObjEntry> *retResult)

//	Find
//
//	Find a set of objects matching the given criteria.

	{
	if (retResult)
		retResult->DeleteAll();

	//	If no node ID, then we look through all nodes

	if (sNodeID.IsBlank())
		return AccumulateEntries(m_AllLists, Criteria, retResult);

	//	Otherwise, check the specific node

	else
		{
		//	Look for the node ID.

		TArray<SObjList *> *pTable = m_ByNode.GetAt(sNodeID);
		if (pTable == NULL)
			return false;

		//	Accumulate entries for this table

		return AccumulateEntries(*pTable, Criteria, retResult);
		}
	}

bool CObjectTracker::Find (CTopologyNode *pNode, CSpaceObject *pObj, SObjBasics **retpObjData) const

//  Find
//
//  Returns a pointer to the object data for the given object.

    {
    SObjList *pList = GetList(pObj);
    if (pList == NULL)
        return false;

    SObjBasics *pObjData = pList->Objects.GetAt(pObj->GetID());
    if (pObjData == NULL)
        return false;

    if (retpObjData)
        *retpObjData = pObjData;

    return true;
    }

void CObjectTracker::GetGalacticMapObjects (CTopologyNode *pNode, TArray<SObjEntry> &Results) const

//  GetGalacticMapObjects
//
//  Returns the list of objects in this node that should be shown on the 
//  galactic map.

    {
    int i, j;

    //  Initialize

    Results.DeleteAll();

	//	Look in the index of nodes

	TArray<SObjList *> *pTable = m_ByNode.GetAt(pNode->GetID());
    if (pTable == NULL)
        return;

    //  Loop over all objects

    for (i = 0; i < pTable->GetCount(); i++)
        {
        const SObjList *pList = pTable->GetAt(i);
        if (pList == NULL)
            continue;

        //  Add all objects

        for (j = 0; j < pList->Objects.GetCount(); j++)
            {
            const SObjBasics &ObjData = pList->Objects[j];

            //  We only care about known, significant objects

            if (!ObjData.fKnown || !ObjData.fShowInMap)
                continue;

            //  Add the object to the result

            AccumulateEntry(*pList, pList->Objects.GetKey(j), ObjData, Results);
            }
        }
    }

CObjectTracker::SObjList *CObjectTracker::GetList (CSpaceObject *pObj) const

//	GetList
//
//	Returns the list that corresponds to the given object's UNID and NodeID.

	{
	CSystem *pSystem = pObj->GetSystem();
	if (pSystem == NULL)
		return NULL;

	CTopologyNode *pNode = pSystem->GetTopology();
	if (pNode == NULL)
		return NULL;

	CDesignType *pType = pObj->GetType();
	if (pType == NULL)
		return NULL;

	//	Done

	return GetList(pNode, pType);
	}

CObjectTracker::SObjList *CObjectTracker::GetList (CTopologyNode *pNode, CDesignType *pType) const

//	GetList
//
//	Returns the list that corresponds to the given object's UNID and NodeID.

	{
	int i;

	//	Look in the index of nodes

	TArray<SObjList *> *pTable = m_ByNode.GetAt(pNode->GetID());
    if (pTable == NULL)
        return NULL;

	//	Look for the UNID. If found, return it.

	for (i = 0; i < pTable->GetCount(); i++)
		{
		SObjList *pList = pTable->GetAt(i);
		if (pList->pType == pType)
			return pList;
		}

	//	Otherwise, not found

	return NULL;
	}

void CObjectTracker::Insert (CSpaceObject *pObj)

//	Insert
//
//	Insert a new object.
//
//	NOTE: We rely on our caller to NOT insert the same object twice.

	{
	SObjList *pList = SetList(pObj);
	if (pList == NULL)
		return;

    //  We need the player ship to track disposition (but it's OK if we get back
    //  NULL).

    CSpaceObject *pPlayer = g_pUniverse->GetPlayerShip();

    //  Insert and update

	SObjBasics *pEntry = pList->Objects.Insert(pObj->GetID());
    Refresh(pObj, pEntry, pPlayer);
	}

void CObjectTracker::ReadFromStream (SUniverseLoadCtx &Ctx)

//	ReadFromStream
//
//	Reads all objects
//
//	DWORD			No. of lists
//
//	For each list:
//	CString			NodeID
//	DWORD			UNID
//	DWORD			No. of objects
//
//	For each object
//	DWORD			ObjID
//
//	DWORD			No. of names
//
//	For each name
//	DWORD			ObjID
//	DWORD			Name Flags
//	CString			Name
//  CCompositeImageSelector
//  CString         Notes
//  DWORD           Object flags

	{
    SLoadCtx SystemCtx(Ctx);

	int i;
	int j;
	DWORD dwLoad;

	DeleteAll();

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	int iListCount = (int)dwLoad;

	for (i = 0; i < iListCount; i++)
		{
		CString sNodeID;

		sNodeID.ReadFromStream(Ctx.pStream);
		CTopologyNode *pNode = g_pUniverse->FindTopologyNode(sNodeID);

		DWORD dwUNID;
		Ctx.pStream->Read((char *)&dwUNID, sizeof(DWORD));
		CDesignType *pType = g_pUniverse->FindDesignType(dwUNID);

		int iObjCount;
		Ctx.pStream->Read((char *)&iObjCount, sizeof(DWORD));

		//	Get the appropriate table

		SObjList *pList = (pNode && pType ? SetList(pNode, pType) : NULL);
        if (pList)
            pList->Objects.GrowToFit(iObjCount);

        //  The new version stores everything in one list

        if (SystemCtx.dwVersion >= 132)
            {
            for (j = 0; j < iObjCount; j++)
                {
                Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
                SObjBasics *pObjData = (pList ? pList->Objects.Insert(dwLoad) : NULL);

                //  Flags

                Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
                pObjData->fKnown =              ((dwLoad & 0x00000002) ? true : false);
                pObjData->fShowDestroyed =      ((dwLoad & 0x00000004) ? true : false);
                pObjData->fShowInMap =          ((dwLoad & 0x00000008) ? true : false);
                pObjData->fFriendly =           ((dwLoad & 0x00000010) ? true : false);
                pObjData->fEnemy =              ((dwLoad & 0x00000020) ? true : false);

                //  Extra, if we've got it

                if (dwLoad & 0x00000001)
                    {
                    SObjExtra &Extra = pObjData->SetExtra();

                    Ctx.pStream->Read((char *)&Extra.dwNameFlags, sizeof(DWORD));
                    Extra.sName.ReadFromStream(Ctx.pStream);
                    Extra.ImageSel.ReadFromStream(SystemCtx);
                    Extra.sNotes.ReadFromStream(Ctx.pStream);
                    }
                }
            }

        //  In old version we read multiple lists

        else
            {
            //	Read all the objects

            for (j = 0; j < iObjCount; j++)
                {
                Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
                if (pList)
                    pList->Objects.Insert(dwLoad);
                }

            //	Read names

            if (Ctx.dwVersion >= 21)
                {
                Ctx.pStream->Read((char *)&iObjCount, sizeof(DWORD));
                for (j = 0; j < iObjCount; j++)
                    {
                    DWORD dwObjID;
                    Ctx.pStream->Read((char *)&dwObjID, sizeof(DWORD));

                    SObjBasics *pObjData;
                    if (pList && (pObjData = pList->Objects.GetAt(dwObjID)))
                        {
                        SObjExtra &Extra = pObjData->SetExtra();

                        Ctx.pStream->Read((char *)&Extra.dwNameFlags, sizeof(DWORD));
                        Extra.sName.ReadFromStream(Ctx.pStream);

                        if (SystemCtx.dwVersion >= 131)
                            {
                            Extra.ImageSel.ReadFromStream(SystemCtx);
                            Extra.sNotes.ReadFromStream(Ctx.pStream);

                            //  Flags

                            Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
                            pObjData->fShowDestroyed = ((dwLoad & 0x00000001) ? true : false);
                            }
                        }
                    else
                        {
                        CString sDummy;
                        Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
                        sDummy.ReadFromStream(Ctx.pStream);

                        if (SystemCtx.dwVersion >= 131)
                            {
                            CCompositeImageSelector Dummy;
                            Dummy.ReadFromStream(SystemCtx);
                            sDummy.ReadFromStream(Ctx.pStream);
                            Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
                            }
                        }
                    }
                }
            }
		}
	}

void CObjectTracker::Refresh (CSystem *pSystem)

//  Refresh
//
//  Refresh from current system, if we're invalid.

    {
    int i;

    CTopologyNode *pNode = pSystem->GetTopology();
    if (pNode == NULL)
        return;

    //  Get the player so we can compute disposition

    CSpaceObject *pPlayer = g_pUniverse->GetPlayerShip();

    //  Loop over all objects and refresh them

    for (i = 0; i < pSystem->GetObjectCount(); i++)
        {
        CSpaceObject *pObj = pSystem->GetObject(i);
        if (pObj == NULL
                || pObj->IsDestroyed()
                || (pObj->GetCategory() != CSpaceObject::catShip && pObj->GetCategory() != CSpaceObject::catStation))
            continue;

        //  Look for the object in our list

        SObjBasics *pObjData;
        if (!Find(pNode, pObj, &pObjData))
            {
            //  Should never happen because objects are added/deleted at the
            //  appropriate times.
            ASSERT(false);
            continue;
            }

        //  Refresh

        Refresh(pObj, pObjData, pPlayer);
        }
    }

void CObjectTracker::Refresh (CSpaceObject *pObj, SObjBasics *pObjData, CSpaceObject *pPlayer)

//  Refresh
//
//  Refresh the object data from the actual object.

    {
	CDesignType *pType = pObj->GetType();
	if (pType == NULL)
		return;

    //  Update flags

    pObjData->fKnown = pObj->IsKnown();
    pObjData->fShowDestroyed = pObj->ShowStationDamage();
    pObjData->fShowInMap = pObj->IsShownInGalacticMap();

    //  Track our disposition relative to the player

    if (pPlayer)
        {
        CSovereign::Disposition iDisp = pObj->GetDispositionTowards(pPlayer);
        pObjData->fFriendly = (iDisp == CSovereign::dispFriend);
        pObjData->fEnemy = (iDisp == CSovereign::dispEnemy);
        }

    //  See if we have a custom services description

    CString sCustomNotes;
    bool bHasCustomNotes = pObj->Translate((pObjData->fShowDestroyed ? LANGID_DESC_GALACTIC_MAP_ABANDONED_CUSTOM : LANGID_DESC_GALACTIC_MAP_CUSTOM), NULL, &sCustomNotes);

	//	If the name of this object does not match the type, then we store it.
	//
	//	NOTE: We need to pass a flags var to GetTypeName because it has slightly 
	//	different behavior if you omit them.

	DWORD dwNameFlags;
	DWORD dwDummyFlags;
	CString sName = pObj->GetName(&dwNameFlags);
	if (!pType->GetTypeImage().IsConstant()
            || bHasCustomNotes
            || !strEquals(sName, pType->GetTypeName(&dwDummyFlags)))
		{
        SObjExtra &Extra = pObjData->SetExtra();
		Extra.sName = sName;
		Extra.dwNameFlags = dwNameFlags;
        Extra.ImageSel = pObj->GetImageSelector();
        Extra.sNotes = sCustomNotes;
		}
    else
        pObjData->DeleteExtra();
    }

CObjectTracker::SObjList *CObjectTracker::SetList (CSpaceObject *pObj)

//	GetList
//
//	Returns the list that corresponds to the given object's UNID and NodeID.

	{
	CSystem *pSystem = pObj->GetSystem();
	if (pSystem == NULL)
		return NULL;

	CTopologyNode *pNode = pSystem->GetTopology();
	if (pNode == NULL)
		return NULL;

	CDesignType *pType = pObj->GetType();
	if (pType == NULL)
		return NULL;

	//	Done

	return SetList(pNode, pType);
	}

CObjectTracker::SObjList *CObjectTracker::SetList (CTopologyNode *pNode, CDesignType *pType)

//	GetList
//
//	Returns the list that corresponds to the given object's UNID and NodeID.

	{
	int i;

	//	Look in the index of nodes

	TArray<SObjList *> *pTable = m_ByNode.SetAt(pNode->GetID());

	//	Look for the UNID. If found, return it.

	for (i = 0; i < pTable->GetCount(); i++)
		{
		SObjList *pList = pTable->GetAt(i);
		if (pList->pType == pType)
			return pList;
		}

	//	Otherwise, we need to add it

	SObjList *pNewList = new SObjList;
	pNewList->pNode = pNode;
	pNewList->pType = pType;

	//	Add to the flat list

	m_AllLists.Insert(pNewList);

	//	Add to the index

	pTable->Insert(pNewList);

	//	Done

	return pNewList;
	}

void CObjectTracker::WriteToStream (IWriteStream *pStream)

//	WriteToStream
//
//	Write out all objects
//
//	DWORD			No. of lists
//
//	For each list:
//	CString			NodeID
//	DWORD			UNID
//	DWORD			No. of objects
//
//	For each object
//	DWORD			ObjID
//  DWORD           Flags
//
//  If extra data
//	DWORD			Name Flags
//	CString			Name
//  CCompositeImageSelector
//  CString         Notes

	{
	int i;
	int j;
	DWORD dwSave;

	dwSave = m_AllLists.GetCount();
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	for (i = 0; i < m_AllLists.GetCount(); i++)
		{
		SObjList *pList = m_AllLists[i];

		pList->pNode->GetID().WriteToStream(pStream);

		dwSave = pList->pType->GetUNID();
		pStream->Write((char *)&dwSave, sizeof(DWORD));

		//	Write out all objects

		dwSave = pList->Objects.GetCount();
		pStream->Write((char *)&dwSave, sizeof(DWORD));

		for (j = 0; j < pList->Objects.GetCount(); j++)
			{
            SObjBasics &ObjData = pList->Objects[j];
            dwSave = pList->Objects.GetKey(j);
			pStream->Write((char *)&dwSave, sizeof(DWORD));

            //  Flags

            dwSave = 0;
            dwSave |= (ObjData.pExtra           ? 0x00000001 : 0);
            dwSave |= (ObjData.fKnown           ? 0x00000002 : 0);
            dwSave |= (ObjData.fShowDestroyed   ? 0x00000004 : 0);
            dwSave |= (ObjData.fShowInMap       ? 0x00000008 : 0);
            dwSave |= (ObjData.fFriendly        ? 0x00000010 : 0);
            dwSave |= (ObjData.fEnemy           ? 0x00000020 : 0);
			pStream->Write((char *)&dwSave, sizeof(DWORD));

            //  If we have extra data, save that

            if (ObjData.pExtra)
                {
			    pStream->Write((char *)&ObjData.pExtra->dwNameFlags, sizeof(DWORD));
			    ObjData.pExtra->sName.WriteToStream(pStream);

                ObjData.pExtra->ImageSel.WriteToStream(pStream);
                ObjData.pExtra->sNotes.WriteToStream(pStream);
                }
			}
		}
	}

