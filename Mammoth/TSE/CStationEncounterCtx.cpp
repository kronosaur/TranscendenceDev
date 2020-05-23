//	CStationEncounterCtx.cpp
//
//	CStationEncounterCtx class
//	Copyright (c) 2013 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CStationEncounterCtx::AddEncounter (CSystem *pSystem)

//	AddEncounter
//
//	Adds an encounter at the given level
	
	{
	m_Total.iCount++;

	//	By Level

	if (pSystem 
			&& pSystem->GetLevel() >= 1 
			&& pSystem->GetLevel() <= MAX_TECH_LEVEL)
		{
		SEncounterStats *pCount = m_ByLevel.SetAt(pSystem->GetLevel());
		pCount->iCount++;
		}

	//	By Node

	CTopologyNode *pNode = (pSystem ? pSystem->GetTopology() : NULL);
	if (pNode)
		{
		SEncounterStats *pCount = m_ByNode.SetAt(pNode->GetID());
		pCount->iCount++;
		}
	}

int CStationEncounterCtx::CalcDistanceToCriteria (const CTopologyNode *pNode, const CTopologyAttributeCriteria &Criteria)

//	CalcDistanceToCriteria
//
//	If pNode does not match AttribCriteria, then we return the number of jumps
//	required to reach a node with the given criteria. If a node with the given
//	criteria cannot be reached, we return 100.
//
//	If pNode does match AttribCriteria, we compute the number of jumps required
//	to reach a node that DOES NOT match the criteria and return that as a 
//	negative number. If we cannot reach such a node, we return -100.

	{
	if (pNode == NULL)
		return 100;

	int iDist = pNode->GetTopology().GetDistanceToCriteria(pNode, Criteria);
	if (iDist == -1)
		return 100;
	else if (iDist > 0)
		return iDist;
	else
		{
		iDist = pNode->GetTopology().GetDistanceToCriteriaNoMatch(pNode, Criteria);
		if (iDist == -1)
			return -100;
		else
			return -iDist;
		}
	}

bool CStationEncounterCtx::CanBeEncountered (const CStationEncounterDesc &Desc) const

//	CanBeEncountered
//
//	Returns TRUE if we can encounter this station type.

	{
	//	If not limits then we're OK

	if (m_Total.iLimit == -1)
		return true;

	//	Otherwise, only if we haven't exceeded the limit

	return (m_Total.iCount < m_Total.iLimit);
	}

bool CStationEncounterCtx::CanBeEncounteredInSystem (CSystem *pSystem, const CStationType *pStationType, const CStationEncounterDesc &Desc) const

//	CanBeEncounteredInSystem
//
//	Returns TRUE if we can encounter the given station type in the system.

	{
	if (pSystem)
		{
		int iLimit;
		if (Desc.HasSystemLimit(&iLimit) && GetCountInSystem(pSystem, pStationType) >= iLimit)
			return false;

		//	Check for a level limit

		const SEncounterStats *pCount = m_ByLevel.GetAt(pSystem->GetLevel());
		if (pCount && pCount->iLimit != -1 && pCount->iCount >= pCount->iLimit)
			return false;
		}

	return CanBeEncountered(Desc);
	}

int CStationEncounterCtx::GetBaseFrequencyForNode (CTopologyNode *pNode, const CStationType *pStation, const CStationEncounterDesc &Desc) const

//  GetBaseFrequencyForNode
//
//  Returns the chance of appearing at this node, given only the encounter
//  descriptor and the node.

	{
	if (pNode == NULL)
		return 0;

	//  See if we've got a cached value for this node. (Even if not, we need
	//  the structure so we can cache it.)
	//
	//	We're caching iNodeCriteria, so we need to be able to add an entry to
	//	m_ByNode. In the future we should store this elsewhere or perhaps deal
	//	with not being able to add it (use GetAt instead).

	const SEncounterStats *pStats = const_cast<CStationEncounterCtx *>(this)->m_ByNode.SetAt(pNode->GetID());
	if (pStats->iNodeCriteria == -1)
		{
		pStats->iNodeCriteria = Desc.CalcFrequencyForNode(*pNode);
		}

	//  Return cached value

	return pStats->iNodeCriteria;
	}

int CStationEncounterCtx::GetCountInSystem (CSystem *pSystem, const CStationType *pStationType) const

//	GetCountInSystem
//
//	Get the number of stations of this type already in the system.

	{
	CTopologyNode *pNode = (pSystem ? pSystem->GetTopology() : NULL);
	if (pNode == NULL)
		return 0;

	const SEncounterStats *pCount = m_ByNode.GetAt(pNode->GetID());
	if (pCount == NULL)
		return 0;

	return pCount->iCount;
	}

TSortMap<CString, int> CStationEncounterCtx::GetEncounterCountByNode (void) const

//	GetEncounterCountByNode
//
//	Returns the count of stations encountered for each node ID.

	{
	TSortMap<CString, int> Result;
	Result.GrowToFit(m_ByNode.GetCount());

	for (int i = 0; i < m_ByNode.GetCount(); i++)
		Result.SetAt(m_ByNode.GetKey(i), m_ByNode[i].iCount);

	return Result;
	}

int CStationEncounterCtx::GetFrequencyByLevel (int iLevel, const CStationEncounterDesc &Desc) const

//	GetFrequencyByLevel
//
//	Returns the chance of an encounter at the given level.

	{
	if (!CanBeEncountered(Desc))
		return 0;

	return Desc.GetFrequencyByLevel(iLevel);
	}

int CStationEncounterCtx::GetFrequencyForNode (CTopologyNode *pNode, const CStationType *pStation, const CStationEncounterDesc &Desc) const

//	GetFrequencyForNode
//
//	Returns the chance that this station type will appear in the given node.

	{
	//	If we cannot be encountered at all, then we're done

	if (!CanBeEncountered(Desc))
		return 0;

	//	Check for a level limit

	const SEncounterStats *pCount = m_ByLevel.GetAt(pNode->GetLevel());
	if (pCount && pCount->iLimit != -1 && pCount->iCount >= pCount->iLimit)
		return 0;

	//  Return based on the frequency for this node

	return GetBaseFrequencyForNode(pNode, pStation, Desc);
	}

int CStationEncounterCtx::GetFrequencyForSystem (CSystem *pSystem, const CStationType *pStation, const CStationEncounterDesc &Desc) const

//	GetFrequencyForSystem
//
//	Returns the chance that this station type will appear in the given system

	{
	//	If we cannot be encountered at all, then we're done

	if (!CanBeEncountered(Desc))
		return 0;

	//	If this station is unique in the system, see if there are other
	//	stations of this type in the system

	int iLimit;
	if (Desc.HasSystemLimit(&iLimit) && GetCountInSystem(pSystem, pStation) >= iLimit)
		return 0;

	//	Check for a level limit

	const SEncounterStats *pCount = m_ByLevel.GetAt(pSystem->GetLevel());
	if (pCount && pCount->iLimit != -1 && pCount->iCount >= pCount->iLimit)
		return 0;

	//	Otherwise, let the descriptor figure out the chance

	return GetBaseFrequencyForNode(pSystem->GetTopology(), pStation, Desc);
	}

int CStationEncounterCtx::GetMinimumForNode (CTopologyNode *pNode, const CStationEncounterDesc &Desc) const

//	GetMinimumForNode
//
//	Returns the currently set minimum value for the given node.
//	0 means no minimum value.

	{
	const SEncounterStats *pStats = m_ByNode.GetAt(pNode->GetID());
	if (pStats == NULL)
		return 0;

	return pStats->iMinimum;
	}

int CStationEncounterCtx::GetRequiredForNode (CTopologyNode *pNode, const CStationEncounterDesc &Desc) const

//	GetRequiredForNode
//
//	Returns the number of encounters that are still needed for this node.

	{
	const SEncounterStats *pStats = m_ByNode.GetAt(pNode->GetID());
	if (pStats == NULL || pStats->iMinimum == 0)
		return 0;

	//	If we're unique in the system and we're already here, then nothing more

	int iLimit;
	if (Desc.HasSystemLimit(&iLimit) && pStats->iCount >= iLimit)
		return 0;

	//	Check node minimums

	if (m_Total.iLimit == -1)
		return Max(0, pStats->iMinimum - pStats->iCount);

	else if (m_Total.iCount < m_Total.iLimit)
		return Max(0, Min(m_Total.iLimit - m_Total.iCount, pStats->iMinimum - pStats->iCount));

	else
		return 0;
	}

void CStationEncounterCtx::IncMinimumForNode (CTopologyNode *pNode, const CStationEncounterDesc &Desc, int iInc)

//	IncMinimumForNode
//
//	Increments the minimum for the node.

	{
	SEncounterStats *pStats = m_ByNode.SetAt(pNode->GetID());
	pStats->iMinimum += iInc;
	}

void CStationEncounterCtx::ReadFromStream (SUniverseLoadCtx &Ctx)

//	ReadFromStream
//
//	Reads from a stream

	{
	int i;

	Ctx.pStream->Read((char *)&m_Total.iCount, sizeof(DWORD));
	Ctx.pStream->Read((char *)&m_Total.iLimit, sizeof(DWORD));
	if (Ctx.dwSystemVersion >= 106)
		Ctx.pStream->Read((char *)&m_Total.iMinimum, sizeof(DWORD));
	else
		m_Total.iMinimum = 0;

	//	m_ByLevel

	int iCount;
	Ctx.pStream->Read((char *)&iCount, sizeof(DWORD));

	m_ByLevel.DeleteAll();
	for (i = 0; i < iCount; i++)
		{
		int iKey;
		Ctx.pStream->Read((char *)&iKey, sizeof(DWORD));

		SEncounterStats *pStat = m_ByLevel.SetAt(iKey);

		Ctx.pStream->Read((char *)&pStat->iCount, sizeof(DWORD));
		Ctx.pStream->Read((char *)&pStat->iLimit, sizeof(DWORD));
		if (Ctx.dwSystemVersion >= 106)
			Ctx.pStream->Read((char *)&pStat->iMinimum, sizeof(DWORD));
		else
			pStat->iMinimum = 0;
		}

	//	m_ByNode

	if (Ctx.dwSystemVersion >= 106)
		{
		Ctx.pStream->Read((char *)&iCount, sizeof(DWORD));

		m_ByNode.DeleteAll();
		for (i = 0; i < iCount; i++)
			{
			CString sKey;
			sKey.ReadFromStream(Ctx.pStream);

			SEncounterStats *pStat = m_ByNode.SetAt(sKey);

			Ctx.pStream->Read((char *)&pStat->iCount, sizeof(DWORD));
			Ctx.pStream->Read((char *)&pStat->iLimit, sizeof(DWORD));
			Ctx.pStream->Read((char *)&pStat->iMinimum, sizeof(DWORD));
			}
		}
	}

void CStationEncounterCtx::Reinit (const CStationEncounterDesc &Desc)

//	Reinit
//
//	Reinitialize

	{
	m_Total.iCount = 0;
	m_Total.iLimit = Desc.GetMaxAppearing();
	m_Total.iMinimum = Desc.GetNumberAppearing();

	//	No level limits for now

	m_ByLevel.DeleteAll();

	//	Initialize node limits

	m_ByNode.DeleteAll();
	}

void CStationEncounterCtx::WriteToStream (IWriteStream *pStream) const

//	WriteToStream
//
//	Writes to a stream
//
//	DWORD			m_Total.iCount
//	DWORD			m_Total.iLimit
//	DWORD			m_Total.iMinimum
//
//	DWORD			Number of level entries
//	For each level entry:
//	DWORD			Level
//	DWORD			iCount
//	DWORD			iLimit
//	DWORD			iMinimum
//
//	DWORD			Number of node entries
//	For each level entry:
//	CString			Node
//	DWORD			iCount
//	DWORD			iLimit
//	DWORD			iMinimum


	{
	int i;
	DWORD dwSave;

	pStream->Write((char *)&m_Total.iCount, sizeof(DWORD));
	pStream->Write((char *)&m_Total.iLimit, sizeof(DWORD));
	pStream->Write((char *)&m_Total.iMinimum, sizeof(DWORD));

	dwSave = m_ByLevel.GetCount();
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	for (i = 0; i < m_ByLevel.GetCount(); i++)
		{
		dwSave = m_ByLevel.GetKey(i);
		pStream->Write((char *)&dwSave, sizeof(DWORD));

		dwSave = m_ByLevel[i].iCount;
		pStream->Write((char *)&dwSave, sizeof(DWORD));

		dwSave = m_ByLevel[i].iLimit;
		pStream->Write((char *)&dwSave, sizeof(DWORD));

		dwSave = m_ByLevel[i].iMinimum;
		pStream->Write((char *)&dwSave, sizeof(DWORD));
		}

	dwSave = m_ByNode.GetCount();
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	for (i = 0; i < m_ByNode.GetCount(); i++)
		{
		m_ByNode.GetKey(i).WriteToStream(pStream);

		dwSave = m_ByNode[i].iCount;
		pStream->Write((char *)&dwSave, sizeof(DWORD));

		dwSave = m_ByNode[i].iLimit;
		pStream->Write((char *)&dwSave, sizeof(DWORD));

		dwSave = m_ByNode[i].iMinimum;
		pStream->Write((char *)&dwSave, sizeof(DWORD));
		}
	}
