//	CNetworkTopologyCreator.cpp
//
//	CNetworkTopologyCreator class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define ENTRANCE_NODE_TAG						CONSTLIT("FragmentEntrance")
#define STARGATES_TAG							CONSTLIT("Stargates")

#define ID_ATTRIB								CONSTLIT("ID")
#define NODE_ID_ATTRIB							CONSTLIT("nodeID")

CNetworkTopologyCreator::CNetworkTopologyCreator (const CTopologyDesc &Desc) :
		m_Desc(Desc)

//	CNetworkTopologyCreator constructor

	{
	}

ALERROR CNetworkTopologyCreator::Create (STopologyCreateCtx &Ctx, CTopology &Topology, CTopologyNode **retpNode) const

//	Create
//
//	Create the topology

	{
	ALERROR error;
	int i;

	CXMLElement *pNetworkXML = m_Desc.GetDesc();

	//	Initialize to NULL (in case we exit early with no nodes).

	if (retpNode)
		*retpNode = NULL;

	//	Generate a unique prefix for the nodes in this network

	CString sPrefix = pNetworkXML->GetAttribute(ID_ATTRIB);
	if (!sPrefix.IsBlank() && m_Desc.GetTopologyDescCount() > 0)
		{
		CString sPartialID = m_Desc.GetTopologyDesc(0)->GetID();
		CString sOriginalPrefix = sPrefix;
		int iNumber = 2;
		while (Topology.FindTopologyNode(strPatternSubst(CONSTLIT("%s%s"), sPrefix, sPartialID)))
			{
			sPrefix = strPatternSubst(CONSTLIT("%s%d"), sOriginalPrefix, iNumber++);
			}
		}

	//	Prepare context

	STopologyCreateCtx NewCtx;
	NewCtx = Ctx;
	//	Ideally, the fragment property would be set by the caller, but until
	//	then we assume that all <Network> elements are fragments
	NewCtx.bInFragment = true;
	NewCtx.pFragmentTable = NULL;
	NewCtx.sFragmentPrefix = sPrefix;

	//	Add all the nodes in the network

	for (i = 0; i < m_Desc.GetTopologyDescCount(); i++)
		{
		CTopologyDesc *pNodeDesc = m_Desc.GetTopologyDesc(i);

		//	If this is a relative node, of if the node doesn't exist,
		//	then we need to create it.

		CTopologyNode *pNewNode;
		if (!pNodeDesc->IsAbsoluteNode()
				|| (pNewNode = Topology.FindTopologyNode(pNodeDesc->GetID())) == NULL)
			{
			if (error = Topology.AddTopologyDesc(NewCtx, pNodeDesc, &pNewNode))
				{
				Ctx.sError = NewCtx.sError;
				return error;
				}
			}

		//	We return the first node

		if (i == 0 && retpNode)
			*retpNode = pNewNode;
		}

	//	Now create the stargate connections

	CXMLElement *pStargateList = pNetworkXML->GetContentElementByTag(STARGATES_TAG);
	if (pStargateList == 0 || pStargateList->GetContentElementCount() == 0)
		return NOERROR;

	//	Always treat it as a group

	IElementGenerator::SCtx GenCtx;
	GenCtx.pTopology = &Topology;

	TArray<CXMLElement *> Stargates;
	CString sError;
	if (!IElementGenerator::GenerateAsGroup(GenCtx, pStargateList, Stargates, &sError))
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Topology %s: Unable to generate random stargate table: %s"), pNetworkXML->GetAttribute(ID_ATTRIB), sError);
		return ERR_FAIL;
		}

	for (i = 0; i < Stargates.GetCount(); i++)
		{
		CXMLElement *pGate = Stargates[i];

		//	If this is a directive to set the return node, then process it

		if (strEquals(pGate->GetTag(), ENTRANCE_NODE_TAG))
			{
			CString sNodeID = pGate->GetAttribute(NODE_ID_ATTRIB);
			CTopologyNode *pEntranceNode = Topology.FindTopologyNode(NewCtx.ExpandNodeID(sNodeID));
			if (pEntranceNode == NULL)
				{
				Ctx.sError = strPatternSubst(CONSTLIT("Topology %s: Unable to find entrance node: %s"), pNetworkXML->GetAttribute(ID_ATTRIB), sNodeID);
				return ERR_FAIL;
				}

			if (retpNode)
				*retpNode = pEntranceNode;
			}

		//	This will add the stargate and recurse into AddTopologyDesc
		//	(if necessary).

		else
			{
			if (error = Topology.AddStargateFromXML(NewCtx, pGate))
				{
				Ctx.sError = NewCtx.sError;
				return error;
				}
			}
		}

	return NOERROR;
	}
