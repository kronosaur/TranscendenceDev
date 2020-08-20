//	CTopologyNodeCriteria.cpp
//
//	CTopologyNodeCriteria class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define ATTRIBUTES_TAG							CONSTLIT("Attributes")
#define CHANCE_TAG								CONSTLIT("Chance")
#define DISTANCE_BETWEEN_NODES_TAG				CONSTLIT("DistanceBetweenNodes")
#define DISTANCE_TO_TAG							CONSTLIT("DistanceTo")
#define STARGATE_COUNT_TAG						CONSTLIT("StargateCount")

#define CHANCE_ATTRIB							CONSTLIT("chance")
#define CRITERIA_ATTRIB							CONSTLIT("criteria")
#define MAX_ATTRIB								CONSTLIT("max")
#define MIN_ATTRIB								CONSTLIT("min")
#define NODE_ID_ATTRIB							CONSTLIT("nodeID")

#define FIELD_CRITERIA							CONSTLIT("criteria")
#define FIELD_KNOWN_ONLY						CONSTLIT("knownOnly")
#define FIELD_MAX_DIST							CONSTLIT("maxDist")
#define FIELD_MIN_DIST							CONSTLIT("minDist")

void CTopologyNodeCriteria::InitCtx (SCtx &Ctx) const

//	InitCriteriaCtx
//
//	Initializes criteria context. This is needed to optimize distance 
//	calculations.

	{
	//	Initialize the distance cache, if necessary.

	for (int i = 0; i < m_DistanceTo.GetCount(); i++)
		{
		//	If we're restricting nodes to a distance from a particular node, 
		//	then we pre-calculate distances.
	
		if (!m_DistanceTo[i].sNodeID.IsBlank())
			{
			CTopologyNode *pSource = Ctx.Topology.FindTopologyNode(m_DistanceTo[i].sNodeID);
			if (pSource == NULL)
				continue;

			bool bNew;
			TSortMap<CString, int> *pDistMap = Ctx.DistanceCache.SetAt(pSource->GetID(), &bNew);
			if (bNew)
				Ctx.Topology.CalcDistances(*pSource, *pDistMap);
			}
		}
	}

bool CTopologyNodeCriteria::IsCriteriaAll (void) const

//	IsCriteriaAll
//
//	Returns TRUE if we select all nodes.

	{
	return (m_iChance == 100
			&& m_iMaxInterNodeDist == -1
			&& m_iMinInterNodeDist == 0
			&& m_iMaxStargates == -1
			&& m_iMinStargates == 0
			&& m_AttribCriteria.IsEmpty()
			&& m_DistanceTo.GetCount() == 0);
	}

bool CTopologyNodeCriteria::Matches (SCtx &Ctx, const CTopologyNode &Node) const

//	Matches
//
//	Returns TRUE if the given node matches.

	{
	//	Chance

	if (m_iChance < 100 && mathRandom(1, 100) > m_iChance)
		return false;

	//	Check attributes

	if (!m_AttribCriteria.Matches(Node))
		return false;

	//	Stargates

	if (Node.GetStargateCount() < m_iMinStargates)
		return false;

	if (m_iMaxStargates != -1 && Node.GetStargateCount() > m_iMaxStargates)
		return false;

	//	Flags

	if (m_bKnownOnly && !Node.IsKnown())
		return false;

	//	Distance to other nodes

	for (int i = 0; i < m_DistanceTo.GetCount(); i++)
		{
		//	If we don't have a specified nodeID then we need to find the distance
		//	to any node with the appropriate attributes

		if (m_DistanceTo[i].sNodeID.IsBlank())
			{
			CTopologyNodeList Checked;
			if (!Ctx.Topology.GetTopologyNodeList().IsNodeInRangeOf(&Node,
					m_DistanceTo[i].iMinDist,
					m_DistanceTo[i].iMaxDist,
					m_DistanceTo[i].AttribCriteria,
					Checked))
				return false;
			}

		//	Otherwise, find the distance to the given node

		else
			{
			int iDist;

			//	See if we can use the cache to get the distance. If not, then
			//	we just compute it.

			const TSortMap<CString, int> *pDistMap = Ctx.DistanceCache.GetAt(m_DistanceTo[i].sNodeID);
			if (pDistMap == NULL || !pDistMap->Find(Node.GetID(), &iDist))
				iDist = Ctx.Topology.GetDistance(m_DistanceTo[i].sNodeID, Node.GetID());

			//	In range?

			if (iDist != -1 && iDist < m_DistanceTo[i].iMinDist)
				return false;

			if (iDist == -1 || (m_DistanceTo[i].iMaxDist != -1 && iDist > m_DistanceTo[i].iMaxDist))
				return false;
			}
		}

	//	Done

	return true;
	}

ALERROR CTopologyNodeCriteria::Parse (const CString &sCriteria, CString *retsError)

//	ParseCriteria
//
//	Parses a string criteria

	{
	(*this) = CTopologyNodeCriteria();
	return m_AttribCriteria.Init(sCriteria);
	}

ALERROR CTopologyNodeCriteria::Parse (const CXMLElement &Crit, CString *retsError)

//	ParseCriteria
//
//	Parses an XML element into a criteria desc

	{
	m_iChance = 100;
	m_iMaxInterNodeDist = -1;
	m_iMinInterNodeDist = 0;
	m_iMaxStargates = -1;
	m_iMinStargates = 0;

	for (int i = 0; i < Crit.GetContentElementCount(); i++)
		{
		const CXMLElement &Item = *Crit.GetContentElement(i);

		if (strEquals(Item.GetTag(), ATTRIBUTES_TAG))
			{
			CString sCriteria = Item.GetAttribute(CRITERIA_ATTRIB);
			m_AttribCriteria.Init(sCriteria);
			}
		else if (strEquals(Item.GetTag(), CHANCE_TAG))
			{
			m_iChance = Item.GetAttributeIntegerBounded(CHANCE_ATTRIB, 0, 100, 100);
			}
		else if (strEquals(Item.GetTag(), DISTANCE_BETWEEN_NODES_TAG))
			{
			m_iMinInterNodeDist = Item.GetAttributeIntegerBounded(MIN_ATTRIB, 0, -1, 0);
			m_iMaxInterNodeDist = Item.GetAttributeIntegerBounded(MAX_ATTRIB, 0, -1, -1);
			}
		else if (strEquals(Item.GetTag(), DISTANCE_TO_TAG))
			{
			SDistanceTo *pDistTo = m_DistanceTo.Insert();
			pDistTo->iMinDist = Item.GetAttributeIntegerBounded(MIN_ATTRIB, 0, -1, 0);
			pDistTo->iMaxDist = Item.GetAttributeIntegerBounded(MAX_ATTRIB, 0, -1, -1);

			CString sCriteria;
			if (Item.FindAttribute(CRITERIA_ATTRIB, &sCriteria))
				{
				if (pDistTo->AttribCriteria.Init(sCriteria) != NOERROR)
					{
					*retsError = strPatternSubst(CONSTLIT("Unable to parse criteria: %s"), sCriteria);
					return ERR_FAIL;
					}
				}
			else
				pDistTo->sNodeID = Item.GetAttribute(NODE_ID_ATTRIB);
			}
		else if (strEquals(Item.GetTag(), STARGATE_COUNT_TAG))
			{
			m_iMinStargates = Item.GetAttributeIntegerBounded(MIN_ATTRIB, 0, -1, 0);
			m_iMaxStargates = Item.GetAttributeIntegerBounded(MAX_ATTRIB, 0, -1, -1);
			}
		else
			{
			*retsError = strPatternSubst(CONSTLIT("Unknown criteria element: %s"), Item.GetTag());
			return ERR_FAIL;
			}
		}

	return NOERROR;
	}

ALERROR CTopologyNodeCriteria::Parse (CUniverse &Universe, const ICCItem &Item, CString *retsError)

//	ParseCriteria
//
//	Parses a TLisp criteria structure.

	{
	//	Initialize

	(*this) = CTopologyNodeCriteria();

	//	Parse

	if (Item.IsNil())
		return NOERROR;

	else if (Item.IsIdentifier())
		{
		if (ALERROR error = m_AttribCriteria.Init(Item.GetStringValue()))
			return error;
		}

	else if (Item.IsSymbolTable())
		{
		int iMaxDist = -1;
		int iMinDist = 0;

		for (int i = 0; i < Item.GetCount(); i++)
			{
			const CString sKey = Item.GetKey(i);

			if (strEquals(sKey, FIELD_CRITERIA))
				{
				if (ALERROR error = m_AttribCriteria.Init(Item.GetElement(i)->GetStringValue()))
					return error;
				}
			else if (strEquals(sKey, FIELD_MAX_DIST))
				{
				iMaxDist = Item.GetElement(i)->GetIntegerValue();
				}
			else if (strEquals(sKey, FIELD_MIN_DIST))
				{
				iMinDist = Item.GetElement(i)->GetIntegerValue();
				}
			else if (strEquals(sKey, FIELD_KNOWN_ONLY))
				{
				m_bKnownOnly = true;
				}
			else
				{
				if (retsError) *retsError = strPatternSubst(CONSTLIT("Unknown criteria field: %s"), sKey);
				return ERR_FAIL;
				}
			}

		//	Set min/max

		if (iMaxDist != -1 || iMinDist != 0)
			{
			if ((iMaxDist != -1 && iMaxDist < iMinDist)
					|| iMinDist < 0)
				{
				if (retsError) *retsError = strPatternSubst(CONSTLIT("Invalid criteria distance."));
				return ERR_FAIL;
				}

			//	This short-cut only works if we're not using the full-length,
			//	explicit distanceTo criteria (not yet implemented).

			else if (m_DistanceTo.GetCount() != 0)
				{
				if (retsError) *retsError = strPatternSubst(CONSTLIT("maxDist and minDist incompatible with distanceTo."));
				return ERR_FAIL;
				}

			//	Make sure we have a valid node

			else if (Universe.GetCurrentTopologyNode() == NULL)
				{
				if (retsError) *retsError = strPatternSubst(CONSTLIT("No current system."));
				return ERR_FAIL;
				}

			//	Create a new entry

			SDistanceTo *pDistCriteria = m_DistanceTo.Insert();
			pDistCriteria->sNodeID = Universe.GetCurrentTopologyNode()->GetID();
			pDistCriteria->iMaxDist = iMaxDist;
			pDistCriteria->iMinDist = iMinDist;
			}
		}
	else
		{
		if (retsError) *retsError = CONSTLIT("Invalid criteria.");
		return ERR_FAIL;
		}

	return NOERROR;
	}

