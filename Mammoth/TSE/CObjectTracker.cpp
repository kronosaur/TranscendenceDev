//	CObjectTracker.cpp
//
//	CObjectTracker class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define LANGID_DESC_GALACTIC_MAP_CUSTOM         CONSTLIT("core.mapDescCustom")
#define LANGID_DESC_GALACTIC_MAP_ABANDONED_CUSTOM CONSTLIT("core.mapDescAbandonedCustom")

const int MAX_ALLOC_GRANULARITY =			10000;

CObjectTracker::~CObjectTracker (void)

//	CObjectTracker destructor

	{
	DeleteAll();
	}

bool CObjectTracker::AccumulateEntries (TArray<SObjList *> &Table, const CObjectTrackerCriteria &Criteria, DWORD dwFlags, TArray<SObjEntry> *retResult) const

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

		if (!pList->pType->MatchesCriteria(Criteria.GetTypeCriteria()))
			continue;

		//	If all we care about is whether we have any entries, then we're done.

		if (retResult == NULL)
			return true;

		//	Otherwise, add all objects to the results

        retResult->GrowToFit(pList->Objects.GetCount());
		for (j = 0; j < pList->Objects.GetCount(); j++)
			{
			const SObjBasics &Basics = pList->Objects[j];

			//	If this object does not match, skip

			if (Criteria.SelectsActiveOnly() && Basics.fInactive && !pList->pType->IsVirtual())
				continue;

			if (Criteria.SelectsKilledOnly() && (!Basics.fInactive || pList->pType->IsVirtual()))
				continue;

			//	Otherwise, add

            AccumulateEntry(*pList, pList->Objects.GetKey(j), Basics, dwFlags, *retResult);
			}
		}

	//	Done

	return (retResult && retResult->GetCount() > 0);
	}

void CObjectTracker::AccumulateEntry (const SObjList &ObjList, DWORD dwObjID, const SObjBasics &ObjData, DWORD dwFlags, TArray<SObjEntry> &Results) const

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
	pEntry->fInactive = ObjData.fInactive;

	if (ObjData.pExtra)
		{
		pEntry->sName = ObjData.pExtra->sName;
		pEntry->dwNameFlags = ObjData.pExtra->dwNameFlags;
        pEntry->ImageSel = ObjData.pExtra->ImageSel;
        pEntry->sNotes = ObjData.pExtra->sNotes;
		}
	else
		pEntry->sName = ObjList.pType->GetNamePattern(0, &pEntry->dwNameFlags);

	//	If we don't need notes, then don't initialize them (this can save
	//	execution time).

	if (!(dwFlags & FLAG_ACCUMULATE_NOTES))
		;

    //  If we've got custom notes stored with the object, then use those.

    else if (!pEntry->sNotes.IsBlank())
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

bool CObjectTracker::Find (const CString &sNodeID, const CObjectTrackerCriteria &Criteria, TArray<SObjEntry> *retResult)

//	Find
//
//	Find a set of objects matching the given criteria.

	{
	if (retResult)
		retResult->DeleteAll();

	//	If necessary, refresh current system so we get the latests data.

	CSystem *pSystem = g_pUniverse->GetCurrentSystem();
	if (Criteria.NeedsRefresh()
			&& pSystem
			&& (sNodeID.IsBlank() 
				|| (pSystem->GetTopology() && strEquals(sNodeID, pSystem->GetTopology()->GetID()))))
		{
		Refresh(pSystem);
		}

	//	If no node ID, then we look through all nodes

	if (sNodeID.IsBlank())
		return AccumulateEntries(m_AllLists, Criteria, 0, retResult);

	//	Otherwise, check the specific node

	else
		{
		//	Look for the node ID.

		SNodeData *pNodeData = m_ByNode.GetAt(sNodeID);
		if (pNodeData == NULL)
			return false;

		//	Accumulate entries for this table

		return AccumulateEntries(pNodeData->ObjLists, Criteria, 0, retResult);
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

bool CObjectTracker::Find (SNodeData *pNodeData, CSpaceObject *pObj, SObjBasics **retpObjData) const

//  Find
//
//  Returns a pointer to the object data for the given object.

    {
    int i;

	//	Look for the UNID. If found, return it.

    CDesignType *pType = pObj->GetType();
    SObjList *pList = NULL;
	for (i = 0; i < pNodeData->ObjLists.GetCount(); i++)
		{
		if (pNodeData->ObjLists.GetAt(i)->pType == pType)
            {
            pList = pNodeData->ObjLists.GetAt(i);
            break;
            }
		}

    if (pList == NULL)
        return false;

    SObjBasics *pObjData = pList->Objects.GetAt(pObj->GetID());
    if (pObjData == NULL)
        return false;

    if (retpObjData)
        *retpObjData = pObjData;

    return true;
    }

bool CObjectTracker::GetCustomDesc (CSpaceObject *pObj, const SObjBasics &ObjData, CString *retsDesc) const

//	GetCustomDesc
//
//	Returns a custom description for object services.

	{
	CDesignType *pType = pObj->GetType();
	if (pType == NULL || !pType->HasCustomMapDescLanguage())
		return false;

	//	Compose a data block which has the default description.

	CCodeChain &CC = g_pUniverse->GetCC();
	ICCItemPtr pData = ICCItemPtr(ICCItem::SymbolTable);

	//	Add the default trade description, in case we need it.

    CTradingDesc *pTrade;
    CString sTradeDesc;
	if (!ObjData.fShowDestroyed
			&& (pTrade = pType->GetTradingDesc())
			&& pTrade->ComposeDescription(&sTradeDesc))
		{
		pData->SetStringAt(CC, CONSTLIT("tradeDesc"), sTradeDesc);
		}

	//	Translate

    return pObj->TranslateText((ObjData.fShowDestroyed ? LANGID_DESC_GALACTIC_MAP_ABANDONED_CUSTOM : LANGID_DESC_GALACTIC_MAP_CUSTOM), pData, retsDesc);
	}

void CObjectTracker::GetGalacticMapObjects (const CTopologyNode *pNode, TArray<SObjEntry> &Results) const

//  GetGalacticMapObjects
//
//  Returns the list of objects in this node that should be shown on the 
//  galactic map.

    {
    int i, j;

    //  Initialize

    Results.DeleteAll();

	//	Look in the index of nodes

	const SNodeData *pNodeData = m_ByNode.GetAt(pNode->GetID());
    if (pNodeData == NULL)
        return;

    //  Loop over all objects

    for (i = 0; i < pNodeData->ObjLists.GetCount(); i++)
        {
        const SObjList *pList = pNodeData->ObjLists.GetAt(i);
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

            AccumulateEntry(*pList, pList->Objects.GetKey(j), ObjData, FLAG_ACCUMULATE_NOTES, Results);
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

	const SNodeData *pNodeData = m_ByNode.GetAt(pNode->GetID());
    if (pNodeData == NULL)
        return NULL;

	//	Look for the UNID. If found, return it.

	for (i = 0; i < pNodeData->ObjLists.GetCount(); i++)
		{
		SObjList *pList = pNodeData->ObjLists.GetAt(i);
		if (pList->pType == pType)
			return pList;
		}

	//	Otherwise, not found

	return NULL;
	}

void CObjectTracker::GetSystemBackgroundObjects (const CTopologyNode *pNode, TSortMap<Metric, SBackgroundObjEntry> &Results) const

//  GetSystemObjects
//
//  Returns a list of stars, worlds, and asteroids, sorted by Y-coordinate.

    {
    int i, j;

    Results.DeleteAll();
    Results.GrowToFit(1000);

    const SNodeData *pNodeData = m_ByNode.GetAt(pNode->GetID());
    if (pNodeData == NULL)
        return;

    for (i = 0; i < pNodeData->ObjLists.GetCount(); i++)
        {
        CStationType *pType = CStationType::AsType(pNodeData->ObjLists[i]->pType);
        if (pType == NULL)
            continue;

        if (pType->GetScale() != scaleStar && pType->GetScale() != scaleWorld)
            continue;

        for (j = 0; j < pNodeData->ObjLists[i]->Objects.GetCount(); j++)
            {
            SObjBasics *pObjData = &pNodeData->ObjLists[i]->Objects[j];
            SBackgroundObjEntry *pSysObj = Results.Insert(pObjData->vPos.GetY());
            pSysObj->pType = pType;
            pSysObj->vPos = pObjData->vPos;
            pSysObj->pImageSel = (pObjData->pExtra ? &pObjData->pExtra->ImageSel : NULL);
            }
        }
    }

void CObjectTracker::GetSystemStarObjects (const CTopologyNode *pNode, TArray<SBackgroundObjEntry> &Results) const

//  GetSystemStarObjects
//
//  Returns a list of stars

    {
    int i, j;

    Results.DeleteAll();

    const SNodeData *pNodeData = m_ByNode.GetAt(pNode->GetID());
    if (pNodeData == NULL)
        return;

    for (i = 0; i < pNodeData->ObjLists.GetCount(); i++)
        {
        CStationType *pType = CStationType::AsType(pNodeData->ObjLists[i]->pType);
        if (pType == NULL)
            continue;

        if (pType->GetScale() != scaleStar)
            continue;

        for (j = 0; j < pNodeData->ObjLists[i]->Objects.GetCount(); j++)
            {
            SObjBasics *pObjData = &pNodeData->ObjLists[i]->Objects[j];
            SBackgroundObjEntry *pSysObj = Results.Insert();
            pSysObj->pType = pType;
            pSysObj->vPos = pObjData->vPos;
            pSysObj->pImageSel = (pObjData->pExtra ? &pObjData->pExtra->ImageSel : NULL);
            }
        }
    }

const TArray<COrbit> &CObjectTracker::GetSystemOrbits (const CTopologyNode *pNode) const

//  GetSystemOrbits
//
//  Returns a list of orbits for the system.

    {
    const SNodeData *pNodeData = m_ByNode.GetAt(pNode->GetID());
    ASSERT(pNodeData);

    return pNodeData->Orbits;
    }

void CObjectTracker::GetTradingObjects (const CTopologyNode *pNode, TArray<SObjEntry> &Results) const

//  GetTradingObjects
//
//  Returns the list of objects in this node that have a trade descriptor and 
//	that are friendly to the player.

    {
    int i, j;

    //  Initialize

    Results.DeleteAll();

	//	Look in the index of nodes

	const SNodeData *pNodeData = m_ByNode.GetAt(pNode->GetID());
    if (pNodeData == NULL)
        return;

    //  Loop over all objects

    for (i = 0; i < pNodeData->ObjLists.GetCount(); i++)
        {
        const SObjList *pList = pNodeData->ObjLists.GetAt(i);
        if (pList == NULL)
            continue;

		//	Skip objects that don't have a trade descriptor in their design
		//	type.

		if (pList->pType->GetTradingDesc() == NULL)
			continue;

        //  Add all objects

        for (j = 0; j < pList->Objects.GetCount(); j++)
            {
            const SObjBasics &ObjData = pList->Objects[j];

            //  We only care about friendly objects.

            if (ObjData.fEnemy)
				continue;

            //  Add the object to the result

            AccumulateEntry(*pList, pList->Objects.GetKey(j), ObjData, 0, Results);
            }
        }
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
//	DWORD			No. of nodes
//
//  For each node:
//      CString         NodeID
//      DWORD           No. of lists
//
//	    For each list:
//	    DWORD			Type UNID
//	    DWORD			No. of objects
//
//	    For each object:
//	        DWORD			ObjID
//          CVector         vPos
//          DWORD           Flags
//
//          If extra data
//	        DWORD			Name Flags
//	        CString			Name
//          CCompositeImageSelector
//          CString         Notes
//
//      DWORD           No. of orbits
//      COrbit          List of orbits
//
//		DWORD			No. of commands
//
//		For each command:
//			DWORD				Type
//			CDesignTypeCriteria	Criteria

	{
    SLoadCtx SystemCtx(Ctx);

	int i;
	int j;
	DWORD dwLoad;

	DeleteAll();

    //  New version has a different format

    if (SystemCtx.dwVersion >= 133)
        {
        int iNodeCount;
        Ctx.pStream->Read((char *)&iNodeCount, sizeof(DWORD));

        //  Load all nodes

        int iNode;
        for (iNode = 0; iNode < iNodeCount; iNode++)
            {
            //  Read the node ID

            CString sNodeID;
            sNodeID.ReadFromStream(Ctx.pStream);
            CTopologyNode *pNode = g_pUniverse->FindTopologyNode(sNodeID);
			if (pNode == NULL)
				::kernelDebugLogPattern("Galactic Map: Unable to find nodeID: %s", sNodeID);

            //  Get the node data entry

            SNodeData *pNodeData = (pNode ? m_ByNode.SetAt(pNode->GetID()) : NULL);

            //  Get the number of lists

            int iListCount;
            Ctx.pStream->Read((char *)&iListCount, sizeof(DWORD));

            //  Loop over every list

            for (i = 0; i < iListCount; i++)
                {
                //  Read the UNID for the object in this list

                DWORD dwUNID;
                Ctx.pStream->Read((char *)&dwUNID, sizeof(DWORD));
                CDesignType *pType = g_pUniverse->FindDesignType(dwUNID);
				if (pType == NULL)
					::kernelDebugLogPattern("Galactic Map: Unable to find type: %08x", dwUNID);

                //  Number of objects

                int iObjCount;
                Ctx.pStream->Read((char *)&iObjCount, sizeof(DWORD));

                //	Get the appropriate table

                SObjList *pList = (pNodeData && pType ? SetList(pNodeData, pNode, pType) : NULL);
                if (pList)
                    pList->Objects.GrowToFit(iObjCount);

                //  Loop over all objects in this list

                for (j = 0; j < iObjCount; j++)
                    {
                    Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
                    SObjBasics *pObjData = (pList ? pList->Objects.Insert(dwLoad) : NULL);

                    if (pObjData)
                        {
                        Ctx.pStream->Read((char *)&pObjData->vPos, sizeof(CVector));

                        //  Flags

                        Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
                        pObjData->fKnown =			((dwLoad & 0x00000002) ? true : false);
                        pObjData->fShowDestroyed =	((dwLoad & 0x00000004) ? true : false);
                        pObjData->fShowInMap =		((dwLoad & 0x00000008) ? true : false);
                        pObjData->fFriendly =		((dwLoad & 0x00000010) ? true : false);
                        pObjData->fEnemy =			((dwLoad & 0x00000020) ? true : false);
                        pObjData->fInactive =		((dwLoad & 0x00000040) ? true : false);

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
                    else
                        {
                        CString sDummy;
                        CVector vDummy;
                        CCompositeImageSelector DummySel;
                        Ctx.pStream->Read((char *)&vDummy, sizeof(CVector));
                        Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));

                        if (dwLoad & 0x00000001)
                            {
                            Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
                            sDummy.ReadFromStream(Ctx.pStream);
                            DummySel.ReadFromStream(SystemCtx);
                            sDummy.ReadFromStream(Ctx.pStream);
                            }
                        }
                    }
                }

            //  Read the orbit list

            int iOrbitCount;
            Ctx.pStream->Read((char *)&iOrbitCount, sizeof(DWORD));
            if (pNodeData)
                {
                pNodeData->Orbits.InsertEmpty(iOrbitCount);

                //  Read orbits

                for (i = 0; i < iOrbitCount; i++)
                    Ctx.pStream->Read((char *)&pNodeData->Orbits[i], sizeof(COrbit));
                }
            else
                {
                COrbit Dummy;
                for (i = 0; i < iOrbitCount; i++)
                    Ctx.pStream->Read((char *)&Dummy, sizeof(COrbit));
                }

			//	Read the command list

			if (SystemCtx.dwVersion >= 135)
				{
				int iCommandCount;
				Ctx.pStream->Read((char *)&iCommandCount, sizeof(DWORD));
				if (pNodeData)
					{
					pNodeData->Commands.InsertEmpty(iCommandCount);

					for (i = 0; i < iCommandCount; i++)
						{
						Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
						pNodeData->Commands[i].iCmd = (EDelayedCommandTypes)dwLoad;

						pNodeData->Commands[i].Criteria.ReadFromStream(SystemCtx);
						}
					}
				else
					{
					CDesignTypeCriteria Dummy;
					for (i = 0; i < iCommandCount; i++)
						{
						Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
						Dummy.ReadFromStream(SystemCtx);
						}
					}
				}
            }
        }

    //  Backwards compatibility

    else
        {
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
                    pObjData->fKnown =			((dwLoad & 0x00000002) ? true : false);
                    pObjData->fShowDestroyed =	((dwLoad & 0x00000004) ? true : false);
                    pObjData->fShowInMap =		((dwLoad & 0x00000008) ? true : false);
                    pObjData->fFriendly =		((dwLoad & 0x00000010) ? true : false);
                    pObjData->fEnemy =			((dwLoad & 0x00000020) ? true : false);

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

    SNodeData *pNodeData = m_ByNode.SetAt(pNode->GetID());

    //  Get the player so we can compute disposition

    CSpaceObject *pPlayer = g_pUniverse->GetPlayerShip();

    //  See if we need to accumulate orbit information

    bool bAddOrbits = (pNodeData->Orbits.GetCount() == 0);

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
        if (!Find(pNodeData, pObj, &pObjData))
            {
			//	There are certain (old) bugs which caused us to lose all the 
			//	objects in CObjectTracker. If that happens, we just re-add the
			//	object here.
			//
			//	But this should really never happen.

			Insert(pObj);
			if (!Find(pNodeData, pObj, &pObjData))
				{
				//  Should never happen because objects are added/deleted at the
				//  appropriate times.
				ASSERT(false);
				continue;
				}
            }

        //  If this object has an orbit, store it.

        const COrbit *pOrbit;
        if (bAddOrbits 
                && pObj->ShowMapOrbit()
                && (pOrbit = pObj->GetMapOrbit()))
            pNodeData->Orbits.Insert(*pOrbit);

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

    pObjData->vPos = pObj->GetPos();
    pObjData->fKnown = pObj->IsKnown();
    pObjData->fShowDestroyed = pObj->ShowStationDamage();
    pObjData->fShowInMap = pObj->IsShownInGalacticMap();
	pObjData->fInactive = pObj->IsInactive();

    //  Track our disposition relative to the player

    if (pPlayer)
        {
        CSovereign::Disposition iDisp = pObj->GetDispositionTowards(pPlayer);
        pObjData->fFriendly = (iDisp == CSovereign::dispFriend);
        pObjData->fEnemy = (iDisp == CSovereign::dispEnemy);
        }

    //  See if we have a custom services description

    CString sCustomNotes;
    bool bHasCustomNotes = GetCustomDesc(pObj, *pObjData, &sCustomNotes);

	//	If the name of this object does not match the type, then we store it.
	//
	//	NOTE: We need to pass a flags var to GetTypeName because it has slightly 
	//	different behavior if you omit them.

	DWORD dwNameFlags;
	DWORD dwDummyFlags;
	CString sName = pObj->GetNamePattern(0, &dwNameFlags);
	if (!pType->GetTypeImage().IsConstant()
            || bHasCustomNotes
            || !strEquals(sName, pType->GetNamePattern(0, &dwDummyFlags)))
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

void CObjectTracker::ReplayCommands (CSystem *pSystem)

//	ReplayCommands
//
//	Replay any delayed commands so that the system can be synchronized with any
//	changes made while in a different system.

	{
	int i;

	CTopologyNode *pNode = pSystem->GetTopology();
	if (pNode == NULL)
		return;

	SNodeData *pNodeData = m_ByNode.GetAt(pNode->GetID());
	if (pNodeData == NULL)
		return;

	for (i = 0; i < pNodeData->Commands.GetCount(); i++)
		ReplayCommand(pNodeData->Commands[i], pSystem);

	//	Clear out the commands for this node (so we don't replay them twice).

	pNodeData->Commands.DeleteAll();
	}

void CObjectTracker::ReplayCommand (const SDelayedCommand &Cmd, CSystem *pSystem)

//	ReplayCommand
//
//	Replays a single delayed command.

	{
	int i;

	switch (Cmd.iCmd)
		{
		case cmdSetKnown:
		case cmdSetUnknown:
			{
			for (i = 0; i < pSystem->GetObjectCount(); i++)
				{
				CDesignType *pType;
				CSpaceObject *pObj = pSystem->GetObject(i);
				if (pObj == NULL
						|| (pType = pObj->GetType()) == NULL
						|| pObj->IsDestroyed()
						|| !IsTracked(pObj))
					continue;

				//	Skip objects that don't match the criteria.

				if (!pType->MatchesCriteria(Cmd.Criteria))
					continue;

				//	Set known or unknown

				pObj->SetKnown(Cmd.iCmd == cmdSetKnown);
				}
			break;
			}
		}
	}

void CObjectTracker::SetKnown (const CString &sNodeID, const CDesignTypeCriteria &Criteria, bool bKnown)

//	SetKnown
//
//	Sets the given objects as known or unknown.

	{
	int i;

	//	If no node ID, then we look through all nodes

	if (sNodeID.IsBlank())
		{
		//	Add a delayed command to all nodes so that when we actually enter
		//	it, the objects will be set known appropriately.

		for (i = 0; i < g_pUniverse->GetTopologyNodeCount(); i++)
			{
			CTopologyNode *pNode = g_pUniverse->GetTopologyNode(i);
			if (pNode->IsEndGame())
				continue;

		    SNodeData *pNodeData = m_ByNode.SetAt(pNode->GetID());
			SDelayedCommand *pNewCmd = pNodeData->Commands.Insert();
			pNewCmd->iCmd = (bKnown ? cmdSetKnown : cmdSetUnknown);
			pNewCmd->Criteria = Criteria;
			}

		//	Set all objects known

		return SetKnown(m_AllLists, Criteria, bKnown);
		}

	//	Otherwise, check the specific node

	else
		{
		//	Add a delayed command.

		SNodeData *pNodeData = m_ByNode.SetAt(sNodeID);
		SDelayedCommand *pNewCmd = pNodeData->Commands.Insert();
		pNewCmd->iCmd = (bKnown ? cmdSetKnown : cmdSetUnknown);
		pNewCmd->Criteria = Criteria;

		//	Accumulate entries for this table

		return SetKnown(pNodeData->ObjLists, Criteria, bKnown);
		}
	}

void CObjectTracker::SetKnown (TArray<SObjList *> &Table, const CDesignTypeCriteria &Criteria, bool bKnown)

//	SetKnown
//
//	Sets the objects in the list to known or unknown.

	{
	int i, j;

	for (i = 0; i < Table.GetCount(); i++)
		{
		SObjList *pList = Table[i];

		//	If we don't match, then continue

		if (!pList->pType->MatchesCriteria(Criteria))
			continue;

		//	Look over all objects and make them known

		for (j = 0; j < pList->Objects.GetCount(); j++)
			pList->Objects[j].fKnown = bKnown;
		}
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

//	SetList
//
//	Returns the list that corresponds to the given object's UNID and NodeID.

	{
    SNodeData *pNodeData = m_ByNode.SetAt(pNode->GetID());
    return SetList(pNodeData, pNode, pType);
	}

CObjectTracker::SObjList *CObjectTracker::SetList (SNodeData *pNodeData, CTopologyNode *pNode, CDesignType *pType)

//  SetList
//
//  Returns the list that corresponds to the given object's UNID and node

    {
	int i;

	//	Look for the UNID. If found, return it.

	for (i = 0; i < pNodeData->ObjLists.GetCount(); i++)
		{
		SObjList *pList = pNodeData->ObjLists.GetAt(i);
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

	pNodeData->ObjLists.Insert(pNewList);

	//	Done

	return pNewList;
    }

void CObjectTracker::WriteToStream (IWriteStream *pStream)

//	WriteToStream
//
//	DWORD			No. of nodes
//
//  For each node:
//      CString         NodeID
//      DWORD           No. of lists
//
//	    For each list:
//	    DWORD			Type UNID
//	    DWORD			No. of objects
//
//	    For each object:
//	        DWORD			ObjID
//          CVector         vPos
//          DWORD           Flags
//
//          If extra data
//	        DWORD			Name Flags
//	        CString			Name
//          CCompositeImageSelector
//          CString         Notes
//
//      DWORD           No. of orbits
//      COrbit          List of orbits

	{
	int i;
	int j;
	DWORD dwSave;

    //  Write out the total number of nodes that we know about

    dwSave = m_ByNode.GetCount();
	pStream->Write((char *)&dwSave, sizeof(DWORD));

    //  Loop over each node

    int iNode;
    for (iNode = 0; iNode < m_ByNode.GetCount(); iNode++)
        {
        //  Save the nodeID

        m_ByNode.GetKey(iNode).WriteToStream(pStream);

        //  Write the number of lists for this node.

        dwSave = m_ByNode[iNode].ObjLists.GetCount();
	    pStream->Write((char *)&dwSave, sizeof(DWORD));

        //  Write each of the lists

	    for (i = 0; i < m_ByNode[iNode].ObjLists.GetCount(); i++)
		    {
            SObjList *pList = m_ByNode[iNode].ObjLists[i];

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

                //  Position

                pStream->Write((char *)&ObjData.vPos, sizeof(CVector));

                //  Flags

                dwSave = 0;
                dwSave |= (ObjData.pExtra           ? 0x00000001 : 0);
                dwSave |= (ObjData.fKnown           ? 0x00000002 : 0);
                dwSave |= (ObjData.fShowDestroyed   ? 0x00000004 : 0);
                dwSave |= (ObjData.fShowInMap       ? 0x00000008 : 0);
                dwSave |= (ObjData.fFriendly        ? 0x00000010 : 0);
                dwSave |= (ObjData.fEnemy           ? 0x00000020 : 0);
                dwSave |= (ObjData.fInactive        ? 0x00000040 : 0);
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

        //  Write each of the orbits

        dwSave = m_ByNode[iNode].Orbits.GetCount();
	    pStream->Write((char *)&dwSave, sizeof(DWORD));

        //  Write each of the orbits

        for (i = 0; i < m_ByNode[iNode].Orbits.GetCount(); i++)
            {
            const COrbit &Orbit = m_ByNode[iNode].Orbits[i];
	        pStream->Write((char *)&Orbit, sizeof(COrbit));
            }

		//	Write any delayed commands

		dwSave = m_ByNode[iNode].Commands.GetCount();
	    pStream->Write((char *)&dwSave, sizeof(DWORD));

        for (i = 0; i < m_ByNode[iNode].Commands.GetCount(); i++)
			{
			dwSave = (DWORD)m_ByNode[iNode].Commands[i].iCmd;
	        pStream->Write((char *)&dwSave, sizeof(DWORD));

			m_ByNode[iNode].Commands[i].Criteria.WriteToStream(pStream);
			}
        }
	}

