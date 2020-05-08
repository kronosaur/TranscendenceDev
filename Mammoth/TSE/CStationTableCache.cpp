//	CStationTableCache.cpp
//
//	CStationTableCache class
//	Copyright (c) 2013 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

const CStationEncounterTable *CStationTableCache::AddTable (const CString &sDesc, CStationEncounterTable &Table)

//	AddTable
//
//	Moves the table to the cache.

	{
	auto pDestTable = m_Cache.SetAt(sDesc);
	(*pDestTable).Set(new CStationEncounterTable(std::move(Table)));
	return *pDestTable;
	}

void CStationTableCache::DeleteAll (void)

//	DeleteAll
//
//	Delete all entries

	{
	m_Cache.DeleteAll();
	}

const CStationEncounterTable *CStationTableCache::FindTable (const CString &sDesc) const

//	GetTable
//
//	If a table with the given description is found in the cache we return TRUE.
//	If not, we return FALSE.
//
//	Either way, retpTable is initialized with a pointer to a table to read 
//	or write.

	{
	auto pTable = m_Cache.GetAt(sDesc);
	if (pTable)
		{
		m_iCacheHits++;
		return *pTable;
		}
	else
		{
		m_iCacheMisses++;
		return NULL;
		}
	}

int CStationTableCache::GetCacheHitRate (void) const

//	GetCacheHitRate
//
//	Returns the current hit rate percentage (0-100)

	{
	int iTotal = m_iCacheHits + m_iCacheMisses;
	return (iTotal > 0 ? mathRound(100.0 * (double)m_iCacheHits / iTotal) : 100);
	}
