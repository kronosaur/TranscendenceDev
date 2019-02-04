//	CDesignTable.cpp
//
//	CDesignTable class

#include "PreComp.h"

ALERROR CDesignTable::AddEntry (CDesignType *pEntry)

//	AddEntry
//
//	Adds an entry to the table. Returns ERR_OUTOFROOM if we already have an
//	entry with that UNID.

	{
	ASSERT(pEntry);

	bool bNew;
	m_Table.SetAt(pEntry->GetUNID(), pEntry, &bNew);
	if (!bNew)
		return ERR_OUTOFROOM;

	return NOERROR;
	}

ALERROR CDesignTable::AddOrReplaceEntry (CDesignType *pEntry, CDesignType **retpOldEntry)

//	AddOrReplaceEntry
//
//	Adds or replaces an entry

	{
	bool bAdded;
	CDesignType **pSlot = m_Table.SetAt(pEntry->GetUNID(), &bAdded);

	if (retpOldEntry)
		*retpOldEntry = (!bAdded ? *pSlot : NULL);

	*pSlot = pEntry;

	return NOERROR;
	}

void CDesignTable::Delete (DWORD dwUNID)

//	Delete
//
//	Delete by UNID

	{
	int iIndex;
	if (m_Table.FindPos(dwUNID, &iIndex))
		{
		if (m_bFreeTypes)
			m_Table[iIndex]->Delete();

		m_Table.Delete(iIndex);
		}
	}

void CDesignTable::DeleteAll (void)

//	DeleteAll
//
//	Removes all entries and deletes the object that they point to

	{
	int i;

	if (m_bFreeTypes)
		{
		for (i = 0; i < GetCount(); i++)
			GetEntry(i)->Delete();
		}

	m_Table.DeleteAll();
	}

CDesignType *CDesignTable::FindByUNID (DWORD dwUNID) const

//	FindByUNID
//
//	Returns a pointer to the given entry or NULL

	{
	CDesignType * const *pObj = m_Table.GetAt(dwUNID);
	return (pObj ? *pObj : NULL);
	}

ALERROR CDesignTable::Merge (const CDynamicDesignTable &Source, CDesignList *ioOverride)

//	Merge
//
//	Merge the given table into ours.

	{
	DEBUG_TRY

	int i;

	for (i = 0; i < Source.GetCount(); i++)
		{
		CDesignType *pNewType = Source.GetType(i);

		//	If this is an override then we put it on a different table and
		//	leave the type alone.

		if (pNewType->IsModification())
			{
			if (ioOverride)
				ioOverride->AddEntry(pNewType);
			}

		//	Otherwise, add or replace

		else
			AddOrReplaceEntry(pNewType);
		}

	return NOERROR;

	DEBUG_CATCH
	}

ALERROR CDesignTable::Merge (const CDesignTable &Source, CDesignList &Override, const TArray<DWORD> &ExtensionsIncluded, const TSortMap<DWORD, bool> &TypesUsed, DWORD dwAPIVersion)

//	Merge
//
//	Merge the given table into ours. Entries in Table override our entries
//	if they have the same UNID.

	{
	int i;

	TArray<CDesignType *> Replaced;

	//	We move through both tables in order and handle when we get an 
	//	addition or overlap.

	int iSrcPos = 0;
	int iDestPos = 0;

	//	Merge

	while (iSrcPos < Source.GetCount())
		{
		CDesignType *pNewType = Source.m_Table.GetValue(iSrcPos);

		//	If this is an optional type then we need to figure out whether we're 
		//	going to include it or not.

		bool bMustExist = false;
		bool bExcluded = false;
		if (pNewType->IsOptional())
			{
			bMustExist = (TypesUsed.GetCount() > 0 && TypesUsed.Find(pNewType->GetUNID()));
			bExcluded = !pNewType->IsIncluded(dwAPIVersion, ExtensionsIncluded);
			}

		//	If this is an override type, then we add to a special table.
		//
		//	NOTE: It is OK if we add multiple types of the same UNID. As long
		//	as we add them in order, we're OK.

		if (pNewType->IsModification())
			{
			//	Modifications always rely on some underlying type, so if we're
			//	excluding this modification, we can skip it, even if the type
			//	itself is required by the save file.

			if (!bExcluded)
				Override.AddEntry(pNewType);

			iSrcPos++;
			}

		//	If we're at the end of the destination then just insert

		else if (iDestPos == m_Table.GetCount())
			{
			//	Do not add excluded type, unless required by the save file.
			//	The latter can happen if we make a type optional after a save 
			//	file has been created.

			if (!bExcluded || bMustExist)
				{
				m_Table.InsertSorted(pNewType->GetUNID(), pNewType);
				iDestPos++;
				}

			//	Advance

			iSrcPos++;
			}

		//	Otherwise, see if we need to insert or replace

		else
			{
			int iCompare = AscendingSort * KeyCompare(pNewType->GetUNID(), m_Table.GetKey(iDestPos));

			//	If the same key then we replace

			if (iCompare == 0)
				{
				//	We found the UNID in the destination table, so we don't have 
				//	to worry about the type existing--it does.

				if (!bExcluded)
					{
					//	If we have to free our originals, then remember them here.

					if (m_bFreeTypes)
						{
						CDesignType *pOriginalType = m_Table.GetValue(iDestPos);
						Replaced.Insert(pOriginalType);
						}

					//	If the new type is different than the original, then warn
					//	LATER: This should probably be an error.

					if (m_Table.GetValue(iDestPos)->GetType() != pNewType->GetType())
						::kernelDebugLogPattern("WARNING: Override of %08x is changing the type.", pNewType->GetUNID());

					//	Replace

					m_Table.GetValue(iDestPos) = pNewType;

					//	Advance

					iDestPos++;
					}

				iSrcPos++;
				}

			//	If the source is less than dest then we insert at this
			//	position.

			else if (iCompare == 1)
				{
				if (!bExcluded || bMustExist)
					{
					m_Table.InsertSorted(pNewType->GetUNID(), pNewType, iDestPos);
					iDestPos++;
					}

				//	Advance

				iSrcPos++;
				}

			//	Otherwise, go to the next destination slot

			else
				//	LATER: We could optimize this case by skipping ahead in an
				//	inner loop. Otherwise we're recalculating pNewType and all 
				//	its state (particularly for optional types).

				iDestPos++;
			}
		}

	//	Delete replaced entries

	for (i = 0; i < Replaced.GetCount(); i++)
		Replaced[i]->Delete();

	return NOERROR;
	}
