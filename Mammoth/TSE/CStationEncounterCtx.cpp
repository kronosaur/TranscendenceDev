//	CStationEncounterCtx.cpp
//
//	CStationEncounterCtx class
//	Copyright (c) 2013 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CStationEncounterCtx::AddEncounter (CSystem &System)

//	AddEncounter
//
//	Adds an encounter at the given level
	
	{
	AddEncounter();

	//	By Level

	if (System.GetLevel() >= 1 
			&& System.GetLevel() <= MAX_TECH_LEVEL)
		{
		SEncounterStats *pCount = m_ByLevel.SetAt(System.GetLevel());
		pCount->iCount++;
		}

	//	By Node

	CTopologyNode *pNode = System.GetTopology();
	if (pNode)
		{
		SEncounterStats *pCount = m_ByNode.SetAt(pNode->GetID());
		pCount->iCount++;
		}
	}

int CStationEncounterCtx::CalcDistanceToCriteria (const CTopologyNode &Node, const CTopologyAttributeCriteria &Criteria)

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
	int iDist = Node.GetTopology().GetDistanceToCriteria(&Node, Criteria);
	if (iDist == -1)
		return 100;
	else if (iDist > 0)
		return iDist;
	else
		{
		iDist = Node.GetTopology().GetDistanceToCriteriaNoMatch(&Node, Criteria);
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

bool CStationEncounterCtx::CanBeEncounteredInSystem (CSystem &System, const CStationType &StationType, const CStationEncounterDesc &Desc) const

//	CanBeEncounteredInSystem
//
//	Returns TRUE if we can encounter the given station type in the system.

	{
	int iLimit;
	if (Desc.HasSystemLimit(&iLimit) && GetCountInSystem(System, StationType) >= iLimit)
		return false;

	//	Check for a level limit

	const SEncounterStats *pCount = m_ByLevel.GetAt(System.GetLevel());
	if (pCount && pCount->iLimit != -1 && pCount->iCount >= pCount->iLimit)
		return false;

	return CanBeEncountered(Desc);
	}

int CStationEncounterCtx::GetBaseFrequencyForNode (const CTopologyNode &Node, const CStationEncounterDesc &Desc) const

//  GetBaseFrequencyForNode
//
//  Returns the chance of appearing at this node, given only the encounter
//  descriptor and the node.

	{
	//  See if we've got a cached value for this node. (Even if not, we need
	//  the structure so we can cache it.)
	//
	//	We're caching iNodeCriteria, so we need to be able to add an entry to
	//	m_ByNode. In the future we should store this elsewhere or perhaps deal
	//	with not being able to add it (use GetAt instead).

	const SEncounterStats *pStats = const_cast<CStationEncounterCtx *>(this)->m_ByNode.SetAt(Node.GetID());
	if (pStats->iNodeCriteria == -1)
		{
		pStats->iNodeCriteria = Desc.CalcFrequencyForNode(Node);
		}

	//  Return cached value

	return pStats->iNodeCriteria;
	}

int CStationEncounterCtx::GetCountInSystem (CSystem &System, const CStationType &StationType) const

//	GetCountInSystem
//
//	Get the number of stations of this type already in the system.

	{
	CTopologyNode *pNode = System.GetTopology();
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

int CStationEncounterCtx::GetFrequencyForNode (CTopologyNode &Node, const CStationType &StationType, const CStationEncounterDesc &Desc) const

//	GetFrequencyForNode
//
//	Returns the chance that this station type will appear in the given node.

	{
	//	If we cannot be encountered at all, then we're done

	if (!CanBeEncountered(Desc))
		return 0;

	//	Check for a level limit

	const SEncounterStats *pCount = m_ByLevel.GetAt(Node.GetLevel());
	if (pCount && pCount->iLimit != -1 && pCount->iCount >= pCount->iLimit)
		return 0;

	//  Return based on the frequency for this node

	return GetBaseFrequencyForNode(Node, Desc);
	}

int CStationEncounterCtx::GetFrequencyForSystem (CSystem &System, const CStationType &StationType, const CStationEncounterDesc &Desc) const

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
	if (Desc.HasSystemLimit(&iLimit) && GetCountInSystem(System, StationType) >= iLimit)
		return 0;

	//	Check for a level limit

	const SEncounterStats *pCount = m_ByLevel.GetAt(System.GetLevel());
	if (pCount && pCount->iLimit != -1 && pCount->iCount >= pCount->iLimit)
		return 0;

	//	Otherwise, let the descriptor figure out the chance

	if (CTopologyNode *pNode = System.GetTopology())
		return GetBaseFrequencyForNode(*pNode, Desc);
	else
		return 0;
	}

int CStationEncounterCtx::GetMinimumForNode (CTopologyNode &Node, const CStationEncounterDesc &Desc) const

//	GetMinimumForNode
//
//	Returns the currently set minimum value for the given node.
//	0 means no minimum value.

	{
	const SEncounterStats *pStats = m_ByNode.GetAt(Node.GetID());
	if (pStats == NULL)
		return 0;

	return pStats->iMinimum;
	}

int CStationEncounterCtx::GetRequiredForNode (CTopologyNode &Node, const CStationEncounterDesc &Desc) const

//	GetRequiredForNode
//
//	Returns the number of encounters that are still needed for this node.

	{
	const SEncounterStats *pStats = m_ByNode.GetAt(Node.GetID());
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

void CStationEncounterCtx::IncMinimumForNode (CTopologyNode &Node, int iInc)

//	IncMinimumForNode
//
//	Increments the minimum for the node.

	{
	SEncounterStats *pStats = m_ByNode.SetAt(Node.GetID());
	pStats->iMinimum += iInc;
	}

bool CStationEncounterCtx::IsEncounteredIn (const CTopologyNode &Node, const CStationEncounterDesc &Desc) const

//	IsEncounteredIn
//
//	Returns TRUE if we can be randomly encountered in the given node.
//
//	NOTE: We ignore current limits. If the station is ever found in this node,
//	then we return TRUE, regardless of whether we can create a new station
//	right now (for that semantic, use CanBeEncountered).

	{
	return (GetBaseFrequencyForNode(Node, Desc) > 0);
	}

bool CStationEncounterCtx::IsEncounteredIn (int iLevel, const CStationEncounterDesc &Desc) const

//	IsEncounteredIn
//
//	Returns TRUE if we can be randomly encountered in systems of the given 
//	level.

	{
	return (Desc.GetFrequencyByLevel(iLevel) > 0);
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
