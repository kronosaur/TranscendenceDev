//	CLocationList.cpp
//
//	CLocationList class
//	Copyright (c) 2011 by George Moromisato. All Rights Reserved.

#include "PreComp.h"

const Metric OVERLAP_DIST =	25.0 * LIGHT_SECOND;

void CLocationList::FillCloseLocations (void)

//	FillCloseLocations
//
//	Make sure that all locations are a minimum distance from
//	each other. Fill any that are not.

	{
	int i, j;

	if (!m_bMinDistCheck)
		{
		Metric rMinDist2 = OVERLAP_DIST * OVERLAP_DIST;

		//	Find overlapping locations by looping over all
		//	pair combinations.

		for (i = 0; i < m_List.GetCount(); i++)
			{
			CLocationDef *pL1 = GetLocation(i);

			//	If this location is already blocked, then it means that
			//	it can't block any others (since no object will be placed here).

			if (pL1->IsBlocked())
				continue;

			//	Loop over all other locations that we haven't yet compared

			for (j = i + 1; j < m_List.GetCount(); j++)
				{
				CLocationDef *pL2 = GetLocation(j);

				//	A blocked location can't block us

				if (pL2->IsBlocked())
					continue;

				//	Compute the distance between the two locations

				CVector vDist = pL2->GetOrbit().GetObjectPos() - pL1->GetOrbit().GetObjectPos();
				if (vDist.Length2() < rMinDist2)
					{
					//	pL1 and pL2 are two locations that are too close to each other
					//	and neither is blocked.
					//
					//	If both locations are currently empty, then block one at random

					if (pL1->IsEmpty() && pL2->IsEmpty())
						{
						if (mathRandom(1, 100) <= 50)
							pL1->SetBlocked();
						else
							pL2->SetBlocked();
						}

					//	Otherwise, block the non-empty one

					else if (pL1->IsEmpty())
						pL1->SetBlocked();
					else if (pL2->IsEmpty())
						pL2->SetBlocked();

					//	If neither is empty, then there is nothing we can do

					else
						{
						//	Technically, this should never happen.
						::kernelDebugLogPattern("Found two non-empty locations within 25 light-seconds.");
						}

					//	Continue looping, since there could be another location that is
					//	in range of pL1. [But not if pL1 is blocked.]

					if (pL1->IsBlocked())
						break;
					}
				}
			}

		m_bMinDistCheck = true;
		}
	}

void CLocationList::FillOverlappingWith (CSpaceObject *pObj)

//	FillOverlappingWith
//
//	Fills any locations that overlap with the given object.

	{
	int i;

	for (i = 0; i < m_List.GetCount(); i++)
		{
		CLocationDef *pLoc = GetLocation(i);
		if (pLoc->IsBlocked() || !pLoc->IsEmpty())
			continue;

		CVector vPos = pLoc->GetOrbit().GetObjectPos();

		if (pObj->PointInHitSizeBox(vPos)
				&& pObj->PointInObject(pObj->GetPos(), vPos))
			{
			pLoc->SetBlocked();

#ifdef DEBUG
			::kernelDebugLogPattern("[%s]: Blocked location because it overlaps %s.", pObj->GetSystem()->GetName(), pObj->GetNounPhrase(0));
#endif
			}
		}
	}

bool CLocationList::GetEmptyLocations (TArray<int> *retList)

//	GetEmptyLocations
//
//	Returns a list of empty locations (by index). Return FALSE if there are none.

	{
	int i;
	for (i = 0; i < m_List.GetCount(); i++)
		if (m_List[i].IsEmpty())
			retList->Insert(i);

	return (retList->GetCount() > 0);
	}

const CLocationDef *CLocationList::GetLocationByObjID (DWORD dwObjID) const

//	GetLocationByObjID
//
//	Returns a location definition from an object ID.

	{
	//	Rebuild the index if necessary

	if (m_ObjIndex.GetCount() == 0)
		{
		for (int i = 0; i < m_List.GetCount(); i++)
			{
			if (m_List[i].GetObjID() != 0)
				m_ObjIndex.SetAt(m_List[i].GetObjID(), i);
			}
		}

	//	Do a look up.

	int *pIndex = m_ObjIndex.GetAt(dwObjID);
	if (pIndex == NULL)
		return NULL;

	return &m_List[*pIndex];
	}

CLocationDef *CLocationList::Insert (const CString &sID)

//	Insert
//
//	Insert a new location

	{
	CLocationDef *pLocation = m_List.Insert();
	pLocation->SetID(sID);

	//	Need to check again for min distance

	m_bMinDistCheck = false;

	//	Done

	return pLocation;
	}

void CLocationList::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Reads the structure from a stream
//
//	DWORD			Count
//	CLocationDef	Entries

	{
	int i;
	DWORD dwLoad;

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	m_List.InsertEmpty(dwLoad);

	for (i = 0; i < (int)dwLoad; i++)
		m_List[i].ReadFromStream(Ctx);
	}

void CLocationList::WriteToStream (IWriteStream *pStream)

//	WriteToStream
//
//	Writes the structure to a stream
//
//	DWORD			Count
//	CLocationDef	Entries

	{
	int i;
	DWORD dwSave;

	dwSave = m_List.GetCount();
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	for (i = 0; i < m_List.GetCount(); i++)
		m_List[i].WriteToStream(pStream);
	}
