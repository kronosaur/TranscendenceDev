//	CTopologyNode.cpp
//
//	Star system topology

#include "PreComp.h"

#define ATTRIBUTES_TAG							CONSTLIT("Attributes")
#define CHANCE_TAG								CONSTLIT("Chance")
#define DISTANCE_BETWEEN_NODES_TAG				CONSTLIT("DistanceBetweenNodes")
#define DISTANCE_TO_TAG							CONSTLIT("DistanceTo")
#define STARGATE_COUNT_TAG						CONSTLIT("StargateCount")
#define SYSTEM_TAG								CONSTLIT("System")
#define STARGATES_TAG							CONSTLIT("StarGates")

#define ATTRIBUTES_ATTRIB						CONSTLIT("attributes")
#define CHANCE_ATTRIB							CONSTLIT("chance")
#define CRITERIA_ATTRIB							CONSTLIT("criteria")
#define DEST_FRAGMENT_ROTATION_ATTRIB			CONSTLIT("destFragmentRotation")
#define DESTGATE_ATTRIB							CONSTLIT("destGate")
#define DESTID_ATTRIB							CONSTLIT("destID")
#define EPITAPH_ATTRIB							CONSTLIT("epitaph")
#define END_GAME_ATTRIB							CONSTLIT("endGame")
#define END_GAME_REASON_ATTRIB					CONSTLIT("endGameReason")
#define ID_ATTRIB								CONSTLIT("ID")
#define LEVEL_ATTRIB							CONSTLIT("level")
#define MAX_ATTRIB								CONSTLIT("max")
#define MIN_ATTRIB								CONSTLIT("min")
#define NAME_ATTRIB								CONSTLIT("name")
#define NODE_ID_ATTRIB							CONSTLIT("nodeID")
#define ROOT_NODE_ATTRIB						CONSTLIT("rootNode")
#define UNID_ATTRIB								CONSTLIT("UNID")
#define VARIANT_ATTRIB							CONSTLIT("variant")
#define X_ATTRIB								CONSTLIT("x")
#define Y_ATTRIB								CONSTLIT("y")

#define PREV_DEST								CONSTLIT("[Prev]")

#define PROPERTY_DEST_GATE_ID					CONSTLIT("destGateID")
#define PROPERTY_DEST_ID						CONSTLIT("destID")
#define PROPERTY_GATE_ID						CONSTLIT("gateID")
#define PROPERTY_LEVEL							CONSTLIT("level")
#define PROPERTY_NAME							CONSTLIT("name")
#define PROPERTY_NODE_ID						CONSTLIT("nodeID")
#define PROPERTY_POS							CONSTLIT("pos")
#define PROPERTY_UNCHARTED						CONSTLIT("uncharted")

#define SPECIAL_LEVEL							CONSTLIT("level:")
#define SPECIAL_NODE_ID							CONSTLIT("nodeID:")
#define SPECIAL_SYSTEM_TYPE						CONSTLIT("systemType:")

//	CTopologyNode class --------------------------------------------------------

CTopologyNode::CTopologyNode (const CString &sID, DWORD SystemUNID, CSystemMap *pMap) : m_sID(sID),
		m_SystemUNID(SystemUNID),
		m_iLevel(0),
		m_pMap(pMap),
		m_pSystem(NULL),
		m_dwID(0xffffffff),
		m_bKnown(false),
		m_bPosKnown(false)

//	CTopology constructor

	{
#ifdef DEBUG_ALL_NODES
	m_bKnown = true;
#endif
	}

CTopologyNode::~CTopologyNode (void)

//	CTopology destructor

	{
	}

void CTopologyNode::AddAttributes (const CString &sAttribs)

//	AddAttributes
//
//	Append the given attributes

	{
	m_sAttributes = ::AppendModifiers(m_sAttributes, sAttribs);
	}

ALERROR CTopologyNode::AddStargate (const SStargateDesc &GateDesc)

//	AddStargate
//
//	Adds a new stargate to the topology

	{
	//	Add the gate

	bool bNew;
	SStargateEntry *pDesc = m_NamedGates.SetAt(GateDesc.sName, &bNew);

	//	If not new, then this is an error

	if (!bNew)
		return ERR_FAIL;

	//	Initialize

	pDesc->sDestNode = GateDesc.sDestNode;
	pDesc->sDestEntryPoint = GateDesc.sDestName;

	if (GateDesc.pMidPoints)
		pDesc->MidPoints = *GateDesc.pMidPoints;

	pDesc->fUncharted = GateDesc.bUncharted;

	return NOERROR;
	}

ALERROR CTopologyNode::AddStargateAndReturn (const SStargateDesc &GateDesc)

//	AddStargate
//
//	Adds a new stargate to the topology

	{
	//	Get the destination node

	CTopologyNode *pDestNode = g_pUniverse->FindTopologyNode(GateDesc.sDestNode);
	if (pDestNode == NULL)
		{
		kernelDebugLogPattern("Unable to find destination node: %s", GateDesc.sDestNode);
		return ERR_FAIL;
		}

	//	Look for the destination stargate

	CString sReturnNodeID;
	CString sReturnEntryPoint;
	if (!pDestNode->FindStargate(GateDesc.sDestName, &sReturnNodeID, &sReturnEntryPoint))
		{
		kernelDebugLogPattern("Unable to find destination stargate: %s", GateDesc.sDestName);
		return ERR_FAIL;
		}

	//	Add the gate

	AddStargate(GateDesc);

	//	See if we need to fix up the return gate

	if (strEquals(sReturnNodeID, PREV_DEST))
		pDestNode->SetStargateDest(GateDesc.sDestName, GetID(), GateDesc.sName);

	return NOERROR;
	}

int CTopologyNode::CalcMatchStrength (const CAttributeCriteria &Criteria)

//	CalcMatchStrength
//
//	Calculates the match strength of topology node and the criteria.

	{
	int i;

	int iStrength = 1000;
	for (i = 0; i < Criteria.GetCount(); i++)
		{
		DWORD dwMatchStrength;
		bool bIsSpecial;
		const CString &sAttrib = Criteria.GetAttribAndWeight(i, &dwMatchStrength, &bIsSpecial);

		bool bHasAttrib = (bIsSpecial ? HasSpecialAttribute(sAttrib) : HasAttribute(sAttrib));
		int iAdj = CAttributeCriteria::CalcWeightAdj(bHasAttrib, dwMatchStrength);

		iStrength = iStrength * iAdj / 1000;
		}

	return iStrength;
	}

void CTopologyNode::CreateFromStream (SUniverseLoadCtx &Ctx, CTopologyNode **retpNode)

//	CreateFromStream
//
//	Creates a node from a stream
//
//	CString		m_sID
//	CString		m_sCreatorID
//	DWORD		m_SystemUNID
//	DWORD		m_pMap (UNID)
//	DWORD		m_xPos
//	DWORD		m_yPos
//	CString		m_sName
//	CString		m_sAttributes
//	DWORD		m_iLevel
//	DWORD		m_dwID
//
//	DWORD		No of named gates
//	CString		gate: sName
//	CString		gate: sDestNode
//	CString		gate: sDestEntryPoint
//	DWORD		gate: flags
//	DWORD		gate: xMid
//	DWORD		gate: yMid
//
//	DWORD		No of variant labels
//	CString		variant label
//
//	CAttributeDataBlock	m_Data
//	DWORD		flags
//
//	CString		m_sEpitaph
//	CString		m_sEndGameReason
//
//	CTradingEconomy m_Trading

	{
	int i;
	DWORD dwLoad;
	CTopologyNode *pNode;

	CString sID;
	sID.ReadFromStream(Ctx.pStream);

	CString sCreatorID;
	if (Ctx.dwVersion >= 34)
		sCreatorID.ReadFromStream(Ctx.pStream);

	DWORD dwSystemUNID;
	Ctx.pStream->Read((char *)&dwSystemUNID, sizeof(DWORD));

	CSystemMap *pMap;
	if (Ctx.dwVersion >= 6)
		{
		DWORD dwMapUNID;
		Ctx.pStream->Read((char *)&dwMapUNID, sizeof(DWORD));
		pMap = CSystemMap::AsType(g_pUniverse->FindDesignType(dwMapUNID));
		}
	else
		pMap = NULL;

	pNode = new CTopologyNode(sID, dwSystemUNID, pMap);
	pNode->m_sCreatorID = sCreatorID;

	if (Ctx.dwVersion >= 6)
		{
		Ctx.pStream->Read((char *)&pNode->m_xPos, sizeof(DWORD));
		Ctx.pStream->Read((char *)&pNode->m_yPos, sizeof(DWORD));
		}
	
	pNode->m_sName.ReadFromStream(Ctx.pStream);
	if (Ctx.dwVersion >= 23)
		pNode->m_sAttributes.ReadFromStream(Ctx.pStream);

	Ctx.pStream->Read((char *)&pNode->m_iLevel, sizeof(DWORD));
	Ctx.pStream->Read((char *)&pNode->m_dwID, sizeof(DWORD));

	DWORD dwCount;
	Ctx.pStream->Read((char *)&dwCount, sizeof(DWORD));
	for (i = 0; i < (int)dwCount; i++)
		{
		CString sName;
		sName.ReadFromStream(Ctx.pStream);
		SStargateEntry *pDesc = pNode->m_NamedGates.SetAt(sName);

		pDesc->sDestNode.ReadFromStream(Ctx.pStream);
		pDesc->sDestEntryPoint.ReadFromStream(Ctx.pStream);

		if (Ctx.dwVersion >= 27)
			{
			DWORD dwFlags;
			Ctx.pStream->Read((char *)&dwFlags, sizeof(DWORD));
			bool bCurved =		((dwFlags & 0x00000001) ? true : false);
			pDesc->fUncharted = ((dwFlags & 0x00000002) ? true : false);

			if (bCurved)
				{
				DWORD dwCount;
				Ctx.pStream->Read((char *)&dwCount, sizeof(DWORD));

				pDesc->MidPoints.InsertEmpty(dwCount);
				Ctx.pStream->Read((char *)&pDesc->MidPoints[0], dwCount * sizeof(SPoint));
				}
			}
		}

	Ctx.pStream->Read((char *)&dwCount, sizeof(DWORD));
	for (i = 0; i < (int)dwCount; i++)
		{
		CString sLabel;
		sLabel.ReadFromStream(Ctx.pStream);
		pNode->m_VariantLabels.Insert(sLabel);
		}

	if (Ctx.dwVersion >= 1)
		pNode->m_Data.ReadFromStream(Ctx.pStream);

	//	Flags

	if (Ctx.dwVersion >= 6)
		Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	else
		dwLoad = 0;

	pNode->m_bKnown =		(dwLoad & 0x00000001 ? true : false);
	pNode->m_bPosKnown =	(dwLoad & 0x00000002 ? true : false);
	pNode->m_bMarked = false;

	//	More

	if (Ctx.dwVersion >= 5)
		{
		pNode->m_sEpitaph.ReadFromStream(Ctx.pStream);
		pNode->m_sEndGameReason.ReadFromStream(Ctx.pStream);
		}
	else
		{
		//	For previous version, we forgot to save this, so do it now

		if (pNode->IsEndGame())
			{
			pNode->m_sEpitaph = CONSTLIT("left Human Space on a journey to the Galactic Core");
			pNode->m_sEndGameReason = CONSTLIT("leftHumanSpace");
			}
		}

	if (Ctx.dwVersion >= 32)
		pNode->m_Trading.ReadFromStream(Ctx);

	//	Done

	*retpNode = pNode;
	}

bool CTopologyNode::FindStargate (const CString &sName, CString *retsDestNode, CString *retsEntryPoint)

//	FindStargate
//
//	Looks for the stargate by name and returns the destination node id and entry point

	{
	SStargateEntry *pDesc = m_NamedGates.GetAt(sName);
	if (pDesc == NULL)
		return false;

	if (retsDestNode)
		*retsDestNode = pDesc->sDestNode;

	if (retsEntryPoint)
		*retsEntryPoint = pDesc->sDestEntryPoint;

	return true;
	}

CString CTopologyNode::FindStargateName (const CString &sDestNode, const CString &sEntryPoint)

//	FindStargateName
//
//	Returns the name of the stargate that matches the node and entry point

	{
	int i;

	for (i = 0; i < m_NamedGates.GetCount(); i++)
		{
		SStargateEntry *pDesc = &m_NamedGates[i];
		if (strEquals(pDesc->sDestNode, sDestNode)
				&& strEquals(pDesc->sDestEntryPoint, sEntryPoint))
			return m_NamedGates.GetKey(i);
		}

	return NULL_STR;
	}

bool CTopologyNode::FindStargateTo (const CString &sDestNode, CString *retsName, CString *retsDestGateID)

//	FindStargateTo
//
//	Looks for a stargate to the given node; returns info on the first one.
//	Returns FALSE if none found.

	{
	int i;

	for (i = 0; i < m_NamedGates.GetCount(); i++)
		{
		SStargateEntry *pDesc = &m_NamedGates[i];
		if (strEquals(pDesc->sDestNode, sDestNode))
			{
			if (retsName)
				*retsName = m_NamedGates.GetKey(i);

			if (retsDestGateID)
				*retsDestGateID = pDesc->sDestEntryPoint;

			return true;
			}
		}

	return false;
	}

CString CTopologyNode::GenerateStargateName (void) const

//	GenerateStargateName
//
//	Generates a unique stargate name

	{
	CString sName;
	int iIndex = m_NamedGates.GetCount();

	do
		{
		iIndex++;
		sName = strPatternSubst(CONSTLIT("SG%d"), iIndex);
		}
	while (m_NamedGates.GetAt(sName) != NULL);

	//	Done

	return sName;
	}

CTopologyNode *CTopologyNode::GetGateDest (const CString &sName, CString *retsEntryPoint)

//	GetGateDest
//
//	Get stargate destination

	{
	SStargateEntry *pDesc = m_NamedGates.GetAt(sName);
	if (pDesc == NULL)
		return NULL;

	if (retsEntryPoint)
		*retsEntryPoint = pDesc->sDestEntryPoint;

	if (pDesc->pDestNode == NULL)
		pDesc->pDestNode = g_pUniverse->FindTopologyNode(pDesc->sDestNode);

	return pDesc->pDestNode;
	}

DWORD CTopologyNode::GetLastVisitedTime (void) const

//  GetLastVisitedTime
//
//  Returns the most recent tick when the player was in the given system. If 
//  the player never visited the system, we return 0xFFFFFFFF. If the player
//  is currently in the system, we return the current tick.

    {
    IPlayerController *pPlayer = g_pUniverse->GetPlayer();
    CPlayerGameStats *pStats = (pPlayer ? pPlayer->GetGameStats() : NULL);
    return (pStats ? pStats->GetSystemLastVisitedTime(GetID()) : 0xffffffff);
    }

Metric CTopologyNode::GetLinearDistanceTo (const CTopologyNode *pNode) const

//	GetLinearDistanceTo
//
//	Returns the distance to the given node (or -1 if the node is in a different map).

	{
	Metric rDist2 = GetLinearDistanceTo2(pNode);
	if (rDist2 < 0.0)
		return -1.0;

	return sqrt(rDist2);
	}

Metric CTopologyNode::GetLinearDistanceTo2 (const CTopologyNode *pNode) const

//	GetLinearDistanceTo2
//
//	Returns the distance to the given node (or -1 if the node is in a different map).

	{
	int xSrc, ySrc;
	CSystemMap *pSrcMap = GetDisplayPos(&xSrc, &ySrc);

	int xDest, yDest;
	CSystemMap *pDestMap = pNode->GetDisplayPos(&xDest, &yDest);
	if (pSrcMap != pDestMap)
		return -1.0;

	Metric xDiff = (xSrc - xDest);
	Metric yDiff = (ySrc - yDest);
	return ((xDiff * xDiff) + (yDiff * yDiff));
	}

ICCItem *CTopologyNode::GetProperty (const CString &sName)

//	GetProperty
//
//	Get topology node property

	{
	CCodeChain &CC = g_pUniverse->GetCC();

	if (strEquals(sName, PROPERTY_LEVEL))
		return CC.CreateInteger(GetLevel());
	else if (strEquals(sName, PROPERTY_NAME))
		return CC.CreateString(GetSystemName());
	else if (strEquals(sName, PROPERTY_POS))
		{
		//	If no map, then no position

		if (m_pMap == NULL)
			return CC.CreateNil();

		//	Create a list

		ICCItem *pResult = CC.CreateLinkedList();
		if (pResult->IsError())
			return pResult;

		CCLinkedList *pList = (CCLinkedList *)pResult;

		pList->AppendInteger(CC, m_xPos);
		pList->AppendInteger(CC, m_yPos);

		return pResult;
		}
	else
		return CC.CreateNil();
	}

CString CTopologyNode::GetStargate (int iIndex)

//	GetStargate
//
//	Returns the stargate ID

	{
	return m_NamedGates.GetKey(iIndex);
	}

CTopologyNode *CTopologyNode::GetStargateDest (int iIndex, CString *retsEntryPoint) const

//	GetStargateDest
//
//	Returns the destination node for the given stargate

	{
	SStargateEntry *pDesc = &m_NamedGates[iIndex];
	if (retsEntryPoint)
		*retsEntryPoint = pDesc->sDestEntryPoint;

	if (pDesc->pDestNode == NULL)
		pDesc->pDestNode = g_pUniverse->FindTopologyNode(pDesc->sDestNode);

	return pDesc->pDestNode;
	}

ICCItemPtr CTopologyNode::GetStargateProperty (const CString &sName, const CString &sProperty) const

//	GetStargateProperty
//
//	Returns a property of the given stargate.

	{
	CCodeChain &CC = g_pUniverse->GetCC();

	SStargateEntry *pDesc = m_NamedGates.GetAt(sName);
	if (pDesc == NULL)
		return ICCItemPtr(CC.CreateNil());

	if (strEquals(sProperty, PROPERTY_DEST_GATE_ID))
		return ICCItemPtr(CC.CreateString(pDesc->sDestEntryPoint));

	else if (strEquals(sProperty, PROPERTY_DEST_ID))
		return ICCItemPtr(CC.CreateString(pDesc->sDestNode));

	else if (strEquals(sProperty, PROPERTY_GATE_ID))
		return ICCItemPtr(CC.CreateString(sName));

	else if (strEquals(sProperty, PROPERTY_NODE_ID))
		return ICCItemPtr(CC.CreateString(GetID()));

	else if (strEquals(sProperty, PROPERTY_UNCHARTED))
		return ICCItemPtr(CC.CreateBool(pDesc->fUncharted));

	else
		return ICCItemPtr(CC.CreateNil());
	}

void CTopologyNode::GetStargateRouteDesc (int iIndex, SStargateRouteDesc *retRouteDesc)

//	GetStargateRouteDesc
//
//	Returns route description

	{
	SStargateEntry *pDesc = &m_NamedGates[iIndex];

	retRouteDesc->pFromNode = this;
	retRouteDesc->sFromName = m_NamedGates.GetKey(iIndex);

	if (pDesc->pDestNode == NULL)
		pDesc->pDestNode = g_pUniverse->FindTopologyNode(pDesc->sDestNode);

	retRouteDesc->pToNode = pDesc->pDestNode;
	retRouteDesc->sToName = pDesc->sDestEntryPoint;
	retRouteDesc->MidPoints = pDesc->MidPoints;

	retRouteDesc->bUncharted = pDesc->fUncharted;
	}

bool CTopologyNode::HasSpecialAttribute (const CString &sAttrib) const

//	HasSpecialAttribute
//
//	Returns TRUE if we have the special attribute

	{
	if (strStartsWith(sAttrib, SPECIAL_LEVEL))
		{
		int iLevel = strToInt(strSubString(sAttrib, SPECIAL_LEVEL.GetLength()), -1);
		return (iLevel == GetLevel());
		}
	else if (strStartsWith(sAttrib, SPECIAL_NODE_ID))
		{
		CString sNodeID = strSubString(sAttrib, SPECIAL_NODE_ID.GetLength());
		return strEquals(sNodeID, GetID());
		}
	else if (strStartsWith(sAttrib, SPECIAL_SYSTEM_TYPE))
		{
		DWORD dwUNID = strToInt(strSubString(sAttrib, SPECIAL_SYSTEM_TYPE.GetLength()), 0xffffffff);
		return (dwUNID == m_SystemUNID);
		}
	else
		return false;
	}

bool CTopologyNode::HasVariantLabel (const CString &sVariant)

//	HasVariantLabel
//
//	Returns TRUE if it has the given variant label

	{
	for (int i = 0; i < m_VariantLabels.GetCount(); i++)
		{
		if (strEquals(sVariant, m_VariantLabels[i]))
			return true;
		}

	return false;
	}

ALERROR CTopologyNode::InitFromAdditionalXML (CTopology &Topology, CXMLElement *pDesc, CString *retsError)

//	InitFromAdditionalXML
//
//	Adds additional information

	{
	ALERROR error;

	if (strEquals(pDesc->GetTag(), SYSTEM_TAG))
		{
		if (error = InitFromSystemXML(Topology, pDesc, retsError))
			return error;
		}
	else if (strEquals(pDesc->GetTag(), ATTRIBUTES_TAG))
		{
		if (error = InitFromAttributesXML(pDesc, retsError))
			return error;
		}

	return NOERROR;
	}

ALERROR CTopologyNode::InitFromAttributesXML (CXMLElement *pAttributes, CString *retsError)

//	InitFromAttributesXML
//
//	Adds attributes

	{
	AddAttributes(pAttributes->GetAttribute(ATTRIBUTES_ATTRIB));

	return NOERROR;
	}

ALERROR CTopologyNode::InitFromSystemXML (CTopology &Topology, CXMLElement *pSystem, CString *retsError)

//	InitFromSystemXML
//
//	Initializes the system information based on an XML element.
//	NOTE: We assume the universe is fully bound at this point.

	{
	ALERROR error;

	SDesignLoadCtx LoadCtx;
	CTopologySystemDesc SystemDesc;
	
	if (error = SystemDesc.InitFromXML(LoadCtx, pSystem))
		{
		if (retsError) *retsError = strPatternSubst(CONSTLIT("Topology %s: Unable to load <System> desc: %s"), m_sID, LoadCtx.sError);
		return error;
		}

	SystemDesc.Apply(Topology, this);

	return NOERROR;
	}

bool CTopologyNode::IsCriteriaAll (const SCriteria &Crit)

//	IsCriteriaAll
//
//	Returns TRUE if the criteria matches all nodes

	{
	return (Crit.iChance == 100
			&& Crit.iMaxInterNodeDist == -1
			&& Crit.iMinInterNodeDist == 0
			&& Crit.iMaxStargates == -1
			&& Crit.iMinStargates == 0
			&& Crit.AttribCriteria.AttribsNotAllowed.GetCount() == 0
			&& Crit.AttribCriteria.AttribsRequired.GetCount() == 0
			&& Crit.DistanceTo.GetCount() == 0
			&& Crit.AttribCriteria.SpecialRequired.GetCount() == 0
			&& Crit.AttribCriteria.SpecialNotAllowed.GetCount() == 0);
	}

bool CTopologyNode::MatchesAttributeCriteria (const SAttributeCriteria &Crit) const

//	MatchesAttributeCriteria
//
//	Returns TRUE if this node matches the given criteria

	{
	int i;

	//	Check required attributes

	for (i = 0; i < Crit.AttribsRequired.GetCount(); i++)
		if (!::HasModifier(m_sAttributes, Crit.AttribsRequired[i]))
			return false;

	//	Check disallowed attributes

	for (i = 0; i < Crit.AttribsNotAllowed.GetCount(); i++)
		if (::HasModifier(m_sAttributes, Crit.AttribsNotAllowed[i]))
			return false;

	//	Check special required attributes

	for (i = 0; i < Crit.SpecialRequired.GetCount(); i++)
		if (!HasSpecialAttribute(Crit.SpecialRequired[i]))
			return false;

	//	Check disallowed special attributes

	for (i = 0; i < Crit.SpecialNotAllowed.GetCount(); i++)
		if (HasSpecialAttribute(Crit.SpecialNotAllowed[i]))
			return false;

	return true;
	}

bool CTopologyNode::MatchesCriteria (SCriteriaCtx &Ctx, const SCriteria &Crit)

//	MatchesCriteria
//
//	Returns TRUE if this node matches the given criteria

	{
	int i;

	//	Chance

	if (Crit.iChance < 100 && mathRandom(1, 100) > Crit.iChance)
		return false;

	//	Check attributes

	if (!MatchesAttributeCriteria(Crit.AttribCriteria))
		return false;

	//	Stargates

	if (m_NamedGates.GetCount() < Crit.iMinStargates)
		return false;

	if (Crit.iMaxStargates != -1 && m_NamedGates.GetCount() > Crit.iMaxStargates)
		return false;

	//	Distance to other nodes

	if (Ctx.pTopology)
		{
		for (i = 0; i < Crit.DistanceTo.GetCount(); i++)
			{
			//	If we don't have a specified nodeID then we need to find the distance
			//	to any node with the appropriate attributes

			if (Crit.DistanceTo[i].sNodeID.IsBlank())
				{
				CTopologyNodeList Checked;
				if (!Ctx.pTopology->GetTopologyNodeList().IsNodeInRangeOf(this,
						Crit.DistanceTo[i].iMinDist,
						Crit.DistanceTo[i].iMaxDist,
						Crit.DistanceTo[i].AttribCriteria,
						Checked))
					return false;
				}

			//	Otherwise, find the distance to the given node

			else
				{
				int iDist = Ctx.pTopology->GetDistance(GetID(), Crit.DistanceTo[i].sNodeID);

				if (iDist != -1 && iDist < Crit.DistanceTo[i].iMinDist)
					return false;

				if (iDist == -1 || (Crit.DistanceTo[i].iMaxDist != -1 && iDist > Crit.DistanceTo[i].iMaxDist))
					return false;
				}
			}
		}

	//	Done

	return true;
	}

ALERROR CTopologyNode::ParseAttributeCriteria (const CString &sCriteria, SAttributeCriteria *retCrit)

//	ParseAttributeCriteria
//
//	Parses a string criteria

	{
	char *pPos = sCriteria.GetASCIIZPointer();
	while (*pPos != '\0')
		{
		switch (*pPos)
			{
			case '+':
			case '-':
				{
				bool bRequired = (*pPos == '+');
				bool bBinaryParam;
				CString sParam = ::ParseCriteriaParam(&pPos, false, &bBinaryParam);

				if (bRequired)
					{
					if (bBinaryParam)
						retCrit->SpecialRequired.Insert(sParam);
					else
						retCrit->AttribsRequired.Insert(sParam);
					}
				else
					{
					if (bBinaryParam)
						retCrit->SpecialNotAllowed.Insert(sParam);
					else
						retCrit->AttribsNotAllowed.Insert(sParam);
					}
				break;
				}
			}

		pPos++;
		}

	return NOERROR;
	}

ALERROR CTopologyNode::ParseCriteria (const CString &sCriteria, SCriteria *retCrit, CString *retsError)

//	ParseCriteria
//
//	Parses a string criteria

	{
	retCrit->iChance = 100;
	retCrit->iMaxInterNodeDist = -1;
	retCrit->iMinInterNodeDist = 0;
	retCrit->iMaxStargates = -1;
	retCrit->iMinStargates = 0;

	return ParseAttributeCriteria(sCriteria, &retCrit->AttribCriteria);
	}

ALERROR CTopologyNode::ParseCriteria (CXMLElement *pCrit, SCriteria *retCrit, CString *retsError)

//	ParseCriteria
//
//	Parses an XML element into a criteria desc

	{
	int i;

	retCrit->iChance = 100;
	retCrit->iMaxInterNodeDist = -1;
	retCrit->iMinInterNodeDist = 0;
	retCrit->iMaxStargates = -1;
	retCrit->iMinStargates = 0;

	if (pCrit)
		{
		for (i = 0; i < pCrit->GetContentElementCount(); i++)
			{
			CXMLElement *pItem = pCrit->GetContentElement(i);

			if (strEquals(pItem->GetTag(), ATTRIBUTES_TAG))
				{
				CString sCriteria = pItem->GetAttribute(CRITERIA_ATTRIB);
				ParseAttributeCriteria(sCriteria, &retCrit->AttribCriteria);
				}
			else if (strEquals(pItem->GetTag(), CHANCE_TAG))
				{
				retCrit->iChance = pItem->GetAttributeIntegerBounded(CHANCE_ATTRIB, 0, 100, 100);
				}
			else if (strEquals(pItem->GetTag(), DISTANCE_BETWEEN_NODES_TAG))
				{
				retCrit->iMinInterNodeDist = pItem->GetAttributeIntegerBounded(MIN_ATTRIB, 0, -1, 0);
				retCrit->iMaxInterNodeDist = pItem->GetAttributeIntegerBounded(MAX_ATTRIB, 0, -1, -1);
				}
			else if (strEquals(pItem->GetTag(), DISTANCE_TO_TAG))
				{
				SDistanceTo *pDistTo = retCrit->DistanceTo.Insert();
				pDistTo->iMinDist = pItem->GetAttributeIntegerBounded(MIN_ATTRIB, 0, -1, 0);
				pDistTo->iMaxDist = pItem->GetAttributeIntegerBounded(MAX_ATTRIB, 0, -1, -1);

				CString sCriteria;
				if (pItem->FindAttribute(CRITERIA_ATTRIB, &sCriteria))
					{
					if (ParseAttributeCriteria(sCriteria, &pDistTo->AttribCriteria) != NOERROR)
						{
						*retsError = strPatternSubst(CONSTLIT("Unable to parse criteria: %s"), sCriteria);
						return ERR_FAIL;
						}
					}
				else
					pDistTo->sNodeID = pItem->GetAttribute(NODE_ID_ATTRIB);
				}
			else if (strEquals(pItem->GetTag(), STARGATE_COUNT_TAG))
				{
				retCrit->iMinStargates = pItem->GetAttributeIntegerBounded(MIN_ATTRIB, 0, -1, 0);
				retCrit->iMaxStargates = pItem->GetAttributeIntegerBounded(MAX_ATTRIB, 0, -1, -1);
				}
			else
				{
				*retsError = strPatternSubst(CONSTLIT("Unknown criteria element: %s"), pItem->GetTag());
				return ERR_FAIL;
				}
			}
		}

	return NOERROR;
	}

ALERROR CTopologyNode::ParsePointList (const CString &sValue, TArray<SPoint> *retPoints)

//	ParsePointList
//
//	Parses a list of x,y coordinates

	{
	//	Initialize

	retPoints->DeleteAll();

	//	Keep parsing until we hit the end

	char *pPos = sValue.GetASCIIZPointer();
	bool bInvalid = false;

	while (!bInvalid)
		{
		//	If we don't have any more values, then we're done.

		int x = ::strParseInt(pPos, 0, &pPos, &bInvalid);
		if (bInvalid)
			continue;

		//	Skip whitespace

		while (::strIsWhitespace(pPos))
			pPos++;

		//	Skip delimeter

		if (*pPos != ',')
			return ERR_FAIL;
		pPos++;

		//	Next value

		int y = ::strParseInt(pPos, 0, &pPos, &bInvalid);
		if (bInvalid)
			return ERR_FAIL;

		//	Add the point to the list

		SPoint *pPoint = retPoints->Insert();
		pPoint->x = x;
		pPoint->y = y;

		//	See if we have another point after this

		while (::strIsWhitespace(pPos))
			pPos++;

		//	If we have a delimeter, then skip it.

		if (*pPos == ',' || *pPos == ';')
			pPos++;
		}

	//	Done

	return NOERROR;
	}

ALERROR CTopologyNode::ParsePosition (const CString &sValue, int *retx, int *rety)

//	ParsePosition
//
//	Parse a node position (x,y)

	{
	//	Pre-init

	*retx = 0;
	*rety = 0;

	//	Parse

	char *pPos = sValue.GetASCIIZPointer();

	bool bInvalid;
	*retx = ::strParseInt(pPos, 0, &pPos, &bInvalid);
	if (bInvalid)
		return ERR_FAIL;

	//	Skip whitespace

	while (::strIsWhitespace(pPos))
		pPos++;

	//	Skip delimeter

	if (*pPos != ',')
		return ERR_FAIL;
	pPos++;

	//	Next value

	*rety = ::strParseInt(pPos, 0, &pPos, &bInvalid);
	if (bInvalid)
		return ERR_FAIL;

	return NOERROR;
	}

ALERROR CTopologyNode::ParseStargateString (const CString &sStargate, CString *retsNodeID, CString *retsGateName)

//	ParseStargateString
//
//	Parses stargate from a single string ("nodeID:gateName")
//
//	Note: Callers rely on the fact that a NULL_STR input results in NULL_STR outputs (and NOERROR)

	{
	char *pPos = sStargate.GetASCIIZPointer();
	char *pStart = pPos;
	while (*pPos != ':' && *pPos != '\0')
		pPos++;

	*retsNodeID = CString(pStart, pPos - pStart);

	if (*pPos == ':')
		*retsGateName = CString(pPos + 1);
	else
		*retsGateName = NULL_STR;

	return NOERROR;
	}

bool CTopologyNode::SetProperty (const CString &sName, ICCItem *pValue, CString *retsError)

//	SetProperty
//
//	Set topology node property

	{
	CCodeChain &CC = g_pUniverse->GetCC();

	if (strEquals(sName, PROPERTY_POS))
		{
		if (m_pMap == NULL)
			{
			*retsError = CONSTLIT("Node is not on a system map and cannot be positioned.");
			return false;
			}

		if (pValue->GetCount() < 2)
			{
			*retsError = CONSTLIT("Invalid node coordinate.");
			return false;
			}

		m_xPos = pValue->GetElement(0)->GetIntegerValue();
		m_yPos = pValue->GetElement(1)->GetIntegerValue();

		return true;
		}
	else
		return false;
	}

void CTopologyNode::SetStargateCharted (const CString &sName, bool bCharted)

//	SetStargateCharted
//
//	Sets whether or not the given stargate is charted.

	{
	SStargateEntry *pDesc = m_NamedGates.GetAt(sName);
	if (pDesc == NULL)
		{
		ASSERT(false);
		return;
		}

	pDesc->fUncharted = !bCharted;
	}

void CTopologyNode::SetStargateDest (const CString &sName, const CString &sDestNode, const CString &sEntryPoint)

//	SetStargateDest
//
//	Sets the destination information for the given stargate

	{
	SStargateEntry *pDesc = m_NamedGates.GetAt(sName);
	if (pDesc == NULL)
		{
		ASSERT(false);
		return;
		}

	pDesc->sDestNode = sDestNode;
	pDesc->sDestEntryPoint = sEntryPoint;
	pDesc->pDestNode = NULL;
	}

void CTopologyNode::WriteToStream (IWriteStream *pStream)

//	WriteToStream
//
//	Writes out the variable portions of the node
//
//	CString		m_sID
//	CString		m_sCreatorID
//	DWORD		m_SystemUNID
//	DWORD		m_pMap (UNID)
//	DWORD		m_xPos
//	DWORD		m_yPos
//	CString		m_sName
//	CString		m_sAttributes
//	DWORD		m_iLevel
//	DWORD		m_dwID
//
//	DWORD		No of named gates
//	CString		gate: sName
//	CString		gate: sDestNode
//	CString		gate: sDestEntryPoint
//
//	DWORD		No of variant labels
//	CString		variant label
//
//	CAttributeDataBlock	m_Data
//	DWORD		flags
//
//	CString		m_sEpitaph
//	CString		m_sEndGameReason
//
//	CTradingEconomy m_Trading

	{
	int i;
	DWORD dwSave;

	m_sID.WriteToStream(pStream);
	m_sCreatorID.WriteToStream(pStream);
	pStream->Write((char *)&m_SystemUNID, sizeof(DWORD));

	dwSave = (m_pMap ? m_pMap->GetUNID() : 0);
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	pStream->Write((char *)&m_xPos, sizeof(DWORD));
	pStream->Write((char *)&m_yPos, sizeof(DWORD));
	m_sName.WriteToStream(pStream);
	m_sAttributes.WriteToStream(pStream);
	pStream->Write((char *)&m_iLevel, sizeof(DWORD));
	pStream->Write((char *)&m_dwID, sizeof(DWORD));

	DWORD dwCount = m_NamedGates.GetCount();
	pStream->Write((char *)&dwCount, sizeof(DWORD));
	for (i = 0; i < (int)dwCount; i++)
		{
		SStargateEntry *pDesc = &m_NamedGates[i];
		CString sName = m_NamedGates.GetKey(i);
		sName.WriteToStream(pStream);
		pDesc->sDestNode.WriteToStream(pStream);
		pDesc->sDestEntryPoint.WriteToStream(pStream);

		DWORD dwFlags = 0;
		dwFlags |= (pDesc->MidPoints.GetCount() > 0 ?	0x00000001 : 0);
		dwFlags |= (pDesc->fUncharted ?					0x00000002 : 0);
		pStream->Write((char *)&dwFlags, sizeof(DWORD));

		if (pDesc->MidPoints.GetCount() > 0)
			{
			DWORD dwSave = pDesc->MidPoints.GetCount();
			pStream->Write((char *)&dwSave, sizeof(DWORD));
			pStream->Write((char *)&pDesc->MidPoints[0], sizeof(SPoint) * pDesc->MidPoints.GetCount());
			}
		}

	dwCount = m_VariantLabels.GetCount();
	pStream->Write((char *)&dwCount, sizeof(DWORD));
	for (i = 0; i < (int)dwCount; i++)
		m_VariantLabels[i].WriteToStream(pStream);

	//	Write opaque data

	m_Data.WriteToStream(pStream);

	//	Flags

	dwSave = 0;
	dwSave |= (m_bKnown ?		0x00000001 : 0);
	dwSave |= (m_bPosKnown ?	0x00000002 : 0);
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	//	Write end game data

	m_sEpitaph.WriteToStream(pStream);
	m_sEndGameReason.WriteToStream(pStream);

	//	Write trading data

	m_Trading.WriteToStream(pStream);
	}
