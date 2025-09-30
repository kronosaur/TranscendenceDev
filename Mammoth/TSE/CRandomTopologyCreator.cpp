//	CRandomTopologyCreator.cpp
//
//	CRandomTopologyCreator class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define AREA_TAG								CONSTLIT("Area")
#define NAMES_TAG								CONSTLIT("Names")
#define NODE_TEMPLATE_TAG						CONSTLIT("NodeTemplate")
#define SYSTEM_TAG								CONSTLIT("System")

#define ATTRIBUTES_ATTRIB						CONSTLIT("attributes")
#define BEACON_TYPE_ATTRIB						CONSTLIT("beaconType")
#define COUNT_ATTRIB							CONSTLIT("count")
#define DEBUG_ATTRIB							CONSTLIT("debug")
#define GATE_LOCATION_CRITERIA_ATTRIB			CONSTLIT("gateLocationCriteria")
#define GATE_TYPE_ATTRIB						CONSTLIT("gateType")
#define LINK_ATTRIBUTES_ATTRIB					CONSTLIT("linkAttributes")
#define LINK_COLOR_ATTRIB						CONSTLIT("linkColor")
#define MIN_SEPARATION_ATTRIB					CONSTLIT("minSeparation")
#define NETWORK_TYPE_ATTRIB						CONSTLIT("networkType")

const int DEFAULT_MIN_SEPARATION =				40;

static TStaticStringTable<TStaticStringEntry<CRandomTopologyCreator::ENetworkTypes>, 3> NETWORK_TYPE_TABLE = {
	"path",					CRandomTopologyCreator::netLine,
	"tree",					CRandomTopologyCreator::netTree,
	"web",					CRandomTopologyCreator::netWeb,
	};

CRandomTopologyCreator::CRandomTopologyCreator (const CTopologyDesc &Desc) :
		m_Desc(Desc)

//	CRandomTopologyCreator constructor

	{
	}

ALERROR CRandomTopologyCreator::AddRandomRegion (STopologyCreateCtx &Ctx, 
												 CTopology &Topology,
												 CXMLElement *pRegionDef, 
												 CTopologyNode *&pExitNode, 
												 CIntGraph &Graph, 
												 TArray<CTopologyNode *> &Nodes) const

//	AddRandomRegion
//
//	Adds random nodes to a region in a <Random> definition

	{
	ALERROR error;
	int i;

	//	Figure out how many nodes to create

	DiceRange Count;
	if (error = Count.LoadFromXML(pRegionDef->GetAttribute(COUNT_ATTRIB)))
		{
		Ctx.sError = strPatternSubst(CONSTLIT("%s: Invalid count attribute: %s"), m_Desc.GetID(), pRegionDef->GetAttribute(COUNT_ATTRIB));
		return error;
		}

	int iNodeCount = Count.Roll();
	if (iNodeCount <= 0)
		return NOERROR;

	//	Get the minimum separation between nodes

	int iMinNodeDist = pRegionDef->GetAttributeIntegerBounded(MIN_SEPARATION_ATTRIB, 1, -1, DEFAULT_MIN_SEPARATION);

	//	Get the area definition

	CXMLElement *pAreaDef = pRegionDef->GetContentElementByTag(AREA_TAG);
	if (pAreaDef == NULL)
		{
		Ctx.sError = strPatternSubst(CONSTLIT("%s: Unable to find <Area> in <Random> element."), m_Desc.GetID());
		return ERR_FAIL;
		}

	//	Initialize the area

	CComplexArea ValidArea;
	if (error = Topology.InitComplexArea(pAreaDef, iMinNodeDist, &ValidArea, &Ctx, &pExitNode))
		{
		Ctx.sError = strPatternSubst(CONSTLIT("%s: %s"), m_Desc.GetID(), Ctx.sError);
		return error;
		}

	//	If we're debugging, then add the valid area as a highlight (so we can see it
	//	on the map)

	if (Ctx.GetUniverse().InDebugMode()
			&& pAreaDef->GetAttributeBool(DEBUG_ATTRIB))
		{
		Ctx.pMap->AddAreaHighlight(ValidArea);
		}

	//	Initialize an array of random positions within the valid area

	CIntGraph RegionGraph;
	ValidArea.GeneratePointsInArea(iNodeCount, iMinNodeDist, &RegionGraph);
	ASSERT(RegionGraph.GetNodeCount() == iNodeCount);

	//	Generate a unique prefix for this fragment

	CString sPrefix = Topology.GenerateUniquePrefix(m_Desc.GetID(), CONSTLIT("+N0"));
	int iStart = Nodes.GetCount();

	//	Create the required number of nodes in the valid area

	for (i = 0; i < iNodeCount; i++)
		{
		//	Generate a unique node ID

		CString sNodeID = strPatternSubst(CONSTLIT("%s+N%d"), sPrefix, iStart + i);

		//	Get the node position

		int x, y;
		RegionGraph.GetNodePos(RegionGraph.GetNodeID(i), &x, &y);

		//	Add the node.
		//
		//	NOTE: We can use the same context because the caller sets whether this is 
		//	fragment or not. Also, we don't need to set sFragmentPrefix because this
		//	call does not need it (since we're creating everything manually).

		CTopology::SNodeCreateCtx CreateCtx;
		CreateCtx.xPos = x;
		CreateCtx.yPos = y;
		CreateCtx.iInitialState = m_Desc.GetInitialState();

		CTopologyNode *pNode;
		if (error = Topology.CreateTopologyNode(Ctx, sNodeID, CreateCtx, &pNode))
			return error;

		//	Mark this node as created by the given descriptor

		pNode->SetCreatorID(m_Desc.GetID());

		//	Keep track of the node globally

		Nodes.Insert(pNode);
		}

	//	Add the region graph to the overall graph

	Graph.AddGraph(RegionGraph);

	return NOERROR;
	}

ALERROR CRandomTopologyCreator::ApplyNodeTemplate (STopologyCreateCtx &Ctx, CTopology &Topology, const TArray<CTopologyNode *> &Nodes, CXMLElement *pNodeTemplate, bool bUnmarkedOnly) const

//	ApplyNodeTemplate
//
//	Applies the settings in the node template to the given list of nodes.

	{
	ALERROR error;
	int i;

	CString sNodeAttribs = pNodeTemplate->GetAttribute(ATTRIBUTES_ATTRIB);

	//	If we have a <System> element, we use it to initialize data

	CTopologySystemDesc SystemDesc;
	CXMLElement *pSystemXML = pNodeTemplate->GetContentElementByTag(SYSTEM_TAG);
	if (pSystemXML)
		{
		SDesignLoadCtx LoadCtx;

		if (error = SystemDesc.InitFromXML(LoadCtx, pSystemXML))
			{
			Ctx.sError = LoadCtx.sError;
			return error;
			}
		}

	//	If we have a <Names> element, we use it to generate system names.
	//	NOTE: The SystemDesc object could also have a <Names> element, but this
	//	will override it.

	CNameDesc Names;
	CXMLElement *pNamesXML = pNodeTemplate->GetContentElementByTag(NAMES_TAG);
	if (pNamesXML)
		{
		SDesignLoadCtx LoadCtx;

		if (error = Names.InitFromXML(LoadCtx, pNamesXML))
			{
			Ctx.sError = LoadCtx.sError;
			return error;
			}
		}

	//	Loop over all nodes and apply

	for (i = 0; i < Nodes.GetCount(); i++)
		{
		CTopologyNode *pNode = Nodes[i];

		//	Skip marked nodes, if necessary

		if (bUnmarkedOnly && pNode->IsMarked())
			continue;

		//	Apply attributes

		pNode->AddAttributes(sNodeAttribs);

		//	Apply the system descriptor

		if (!SystemDesc.IsEmpty())
			SystemDesc.Apply(Topology, pNode);

		//	Apply names, if we have some

		if (!Names.IsEmpty())
			pNode->SetName(Names.GenerateName());
		}

	//	Done

	return NOERROR;
	}

ALERROR CRandomTopologyCreator::ApplyRandomNodeParams (STopologyCreateCtx &Ctx, CTopology &Topology, const TArray<CTopologyNode *> &Nodes) const

//	ApplyRandomNodeParams
//
//	Applies parameters to random nodes

	{
	ALERROR error;
	int i, j;

	//	Start by marking all nodes and unmarked (we do this so we can tell which 
	//	nodes we've initialized).

	for (i = 0; i < Nodes.GetCount(); i++)
		Nodes[i]->SetMarked(false);

	//	Loop over all set nodes and initialize them appropriately

	for (i = 0; i < m_Desc.GetTopologyDescCount(); i++)
		{
		CTopologyDesc *pSetNode = m_Desc.GetTopologyDesc(i);

		if (pSetNode->GetType() != ndNode)
			{
			Ctx.sError = strPatternSubst(CONSTLIT("%s: Set nodes must be <Node> type."), m_Desc.GetID());
			return ERR_FAIL;
			}

		//	Figure out the position of the set node (which are in fragment-relative coordinates)

		int x, y;
		pSetNode->GetPos(&x, &y);

		//	Now look for the nearest unmarked node to those coordinates

		int iBestDist2;
		CTopologyNode *pBestNode = NULL;
		for (j = 0; j < Nodes.GetCount(); j++)
			{
			CTopologyNode *pNode = Nodes[i];

			if (!pNode->IsMarked())
				{
				int xNode, yNode;
				Ctx.GetFragmentDisplayPos(pNode, &xNode, &yNode);

				int xDiff = (xNode - x);
				int yDiff = (yNode - y);
				int iDist2 = (xDiff * xDiff) + (yDiff * yDiff);

				if (pBestNode == NULL || iDist2 < iBestDist2)
					{
					pBestNode = pNode;
					iBestDist2 = iDist2;
					}
				}
			}

		//	If we found it, apply attributes, system, and effect

		if (pBestNode)
			{
			//	Attributes

			pBestNode->AddAttributes(pSetNode->GetAttributes());
			
			//	System

			CXMLElement *pSystemXML = pSetNode->GetSystemDesc();
			if (pSystemXML)
				{
				if (error = pBestNode->InitFromSystemXML(Topology, pSystemXML, &Ctx.sError))
					return error;
				}

			//	Effect

			CEffectCreator *pEffect = pSetNode->GetMapEffect();
			if (pEffect)
				{
				int xMap, yMap;

				pBestNode->GetDisplayPos(&xMap, &yMap);
				int iRotation = (Ctx.bInFragment ? Ctx.iRotation : 0);

				Ctx.pMap->AddAnnotation(pBestNode->GetID(), pEffect, xMap, yMap, iRotation);
				}

			//	Mark it, so we don't process it later

			pBestNode->SetMarked();
			}

		//	If we found no best node, then we exit, since we have run out of nodes

		else
			break;
		}

	//	Now loop over all unmarked nodes and take attributes from the node template

	CXMLElement *pXML = m_Desc.GetDesc();
	CXMLElement *pNodeTemplate = pXML->GetContentElementByTag(NODE_TEMPLATE_TAG);
	if (pNodeTemplate)
		{
		if (error = ApplyNodeTemplate(Ctx, Topology, Nodes, pNodeTemplate, true))
			return error;
		}

	return NOERROR;
	}

ALERROR CRandomTopologyCreator::Create (STopologyCreateCtx &Ctx, CTopology &Topology, CTopologyNode **retpNode) const

//	Create
//
//	Create a random topology.

	{
	ALERROR error;

	//	If we're not in a fragment, see if we've already added some nodes for 
	//	this descriptor. If so, then we return the nearest existing node to the
	//	previous node.

	CTopologyNode *pExistingNode;
	if (!Ctx.bInFragment 
			&& Topology.FindNearestNodeCreatedBy(m_Desc.GetID(), Ctx.pPrevNode, &pExistingNode))
		{
		//	If we have a previous node, then pExistingNode will be the node that
		//	is closes to it, so we just return it.

		if (Ctx.pPrevNode)
			{
			if (retpNode) *retpNode = pExistingNode;
			return NOERROR;
			}

		return NOERROR;
		}

	//	Initialize

	CXMLElement *pXML = m_Desc.GetDesc();
	if (retpNode)
		*retpNode = NULL;

	//	Get the type of network topology

	ENetworkTypes iNetwork = ParseNetworkType(pXML->GetAttribute(NETWORK_TYPE_ATTRIB));
	if (iNetwork == netUnknown)
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Unknown network type: %s"), pXML->GetAttribute(NETWORK_TYPE_ATTRIB));
		return ERR_FAIL;
		}

	//	Create new nodes by region. We end up with the following:
	//
	//	Nodes: A single array of created topology nodes across all regions.
	//	Graph: A single graph containing all nodes across all regions.
	//	pExitNode: The topology node that we exit to (if we're a fragment and there is an exit)

	TArray<CTopologyNode *> Nodes;
	CIntGraph Graph;
	CTopologyNode *pExitNode = NULL;

	if (error = AddRandomRegion(Ctx, Topology, pXML, pExitNode, Graph, Nodes))
		return error;

	//	Remember the list of nodes

	int iNodeCount = Nodes.GetCount();
	if (iNodeCount == 0)
		{
		if (retpNode)
			*retpNode = NULL;
		return NOERROR;
		}

	//	Get the node that is closest to the entrance node

	int xEntrance, yEntrance;
	Ctx.GetFragmentEntranceDisplayPos(&xEntrance, &yEntrance);

	DWORD dwFirstNode;
	Graph.FindNearestNode(xEntrance, yEntrance, &dwFirstNode);

	//	Create a template for the RouteDesc

	CTopologyNode::SStargateRouteDesc Template = CTopologyNode::SStargateRouteDesc();

	CString sRawAttributes = pXML->GetAttribute(ATTRIBUTES_ATTRIB);
	CString sRandomTopologyAttributes = strCat(CONSTLIT("randomTopology"), sRawAttributes.IsBlank() ? CONSTLIT("") : CONSTLIT(","));

	Template.sFromAttributes = strCat(sRandomTopologyAttributes, sRawAttributes);
	Template.sToAttributes = Template.sFromAttributes;

	Template.sFromLocationCriteria = pXML->GetAttribute(GATE_LOCATION_CRITERIA_ATTRIB);
	Template.sToLocationCriteria = Template.sFromLocationCriteria;

	Template.dwFromGateType = pXML->GetAttributeInteger(GATE_TYPE_ATTRIB);
	Template.dwToGateType = Template.dwFromGateType;
	
	Template.dwFromBeaconType = pXML->GetAttributeInteger(BEACON_TYPE_ATTRIB);
	Template.dwToBeaconType = Template.dwFromBeaconType;

	Template.rgbColor = LoadARGBColor(pXML->GetAttribute(LINK_COLOR_ATTRIB));
	if (Template.rgbColor.AsDWORD() && Template.rgbColor.GetAlpha() == 0)
		Template.rgbColor.SetAlpha(0xFF);

	//	Generate random connections between the nodes based on the network type

	TArray<CTopologyNode::SStargateRouteDesc> Routes;
	switch (iNetwork)
		{
		case netLine:
			{
			CSimplePathStargateGenerator Generator(Graph, Nodes);
			Generator.Generate(dwFirstNode, Routes, Template);
			break;
			}

		case netTree:
			{
			CTreePathStargateGenerator Generator(Graph, Nodes);
			Generator.Generate(dwFirstNode, Routes, Template);
			break;
			}

		case netWeb:
			{
			CDelaunayStargateGenerator Generator(Graph, Nodes);
			Generator.Generate(Routes, Template);
			break;
			}

		default:
			ASSERT(false);
			break;
		}

	//	Create the stargates

	Topology.AddStargateRoutes(Routes);

	//	Connect to the exit node

	if (pExitNode)
		{
		//	Get the position of the exit node

		int xExit, yExit;
		Ctx.GetFragmentDisplayPos(pExitNode, &xExit, &yExit);

		//	Find the nearest node to the exit

		DWORD dwLastNode;
		Graph.FindNearestNode(xExit, yExit, &dwLastNode);

		CTopologyNode *pLastNode = Nodes[Graph.GetNodeIndex(dwLastNode)];

		//	Connect to the exit node (both directions, with autogenerated names)

		CTopologyNode::SStargateRouteDesc RouteDesc;
		RouteDesc.pFromNode = pLastNode;
		RouteDesc.pToNode = pExitNode;

		if (error = Topology.AddStargateRoute(RouteDesc))
			{
			Ctx.sError = CONSTLIT("Error creating a stargate in <Random> generation.");
			::kernelDebugLogPattern(Ctx.sError);
			return error;
			}
		}

	//	Apply node parameters either from the set nodes or from the template.
	//	We need to do this AFTER we create the stargates because the property of
	//	the nodes sometimes depends on their connections.

	if (error = ApplyRandomNodeParams(Ctx, Topology, Nodes))
		return error;

	//	See if we have an effect (and if so, add it)

	CEffectCreator *pEffect = m_Desc.GetMapEffect();
	if (pEffect)
		{
		int xPos;
		int yPos;
		int iRotation;
		Ctx.GetAbsoluteDisplayPos(0, 0, &xPos, &yPos, &iRotation);

		Ctx.pMap->AddAnnotation(pEffect, xPos, yPos, iRotation);
		}

	//	Return the first node

	if (retpNode)
		*retpNode = Nodes[Graph.GetNodeIndex(dwFirstNode)];

	//	Remember the nodes created in the descriptor (we need this in case we
	//	refer to this descriptor again).

	return NOERROR;
	}

ALERROR CRandomTopologyCreator::FindExistingNode (STopologyCreateCtx &Ctx, CTopology &Topology, CTopologyNode *pExistingNode, CTopologyNode **retpNode) const

//	FindExistingNode
//
//	If this set of nodes has already been created and we've created a stargate to it,
//	we return the most appropriate node to connect to.

	{
	//	pExistingNode must be a valid existing node created by this descriptor.
	//	This must exist, otherwise we would be creating a new region instead of
	//	trying to find a node in an existing region.

	ASSERT(pExistingNode);

	//	If we have a previous node, then pExistingNode will be the node that
	//	is closes to it, so we just return it.

	if (Ctx.pPrevNode)
		{
		if (retpNode) *retpNode = pExistingNode;
		return NOERROR;
		}

	//	If we don't have a previous node, then try to figure out what we're linking
	//	to. We use the destination (the other side of the gate) to figure out which 
	//	node in our descriptor to return.
	//
	//	But if we have no other node, then we can't do anything.

	if (Ctx.sOtherNodeID.IsBlank())
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Unable to find appropriate node in existing %s."), m_Desc.GetID());
		return ERR_FAIL;
		}

	//	See if the destination is a real node.

	CTopologyNode *pDestNode = Topology.FindTopologyNode(Ctx.sOtherNodeID);
	if (pDestNode)
		{
		//	Now that we have a real node, we can find which existing node in our region
		//	is closest to that node.

		if (!Topology.FindNearestNodeCreatedBy(m_Desc.GetID(), pDestNode, retpNode))
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Unable to find nearest node in existing %s"), m_Desc.GetID());
			return ERR_FAIL;
			}

		//	Success!

		return NOERROR;
		}

	//	If the destination is not a node, then get the descriptor for it.

	CTopologyDesc *pDestDesc;
	if (!Topology.FindTopologyDesc(Ctx, Ctx.sOtherNodeID, &pDestDesc))
		{
		Ctx.sError = strPatternSubst(CONSTLIT("NodeID not found: %s"), Ctx.sOtherNodeID);
		return ERR_FAIL;
		}

	//	Find the nearest node to the existing node that we've found.

	if (!Topology.FindNearestNodeCreatedBy(pDestDesc->GetID(), pExistingNode, &pDestNode))
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Unable to find existing node in %s"), pDestDesc->GetID());
		return ERR_FAIL;
		}

	//	Now that we have a valid node closest to us, we recompute the node in our 
	//	region (since we now know what node to be close to).

	if (!Topology.FindNearestNodeCreatedBy(m_Desc.GetID(), pDestNode, retpNode))
		{
		ASSERT(false);
		Ctx.sError = strPatternSubst(CONSTLIT("Unable to find existing node in %s. This should never happen."), m_Desc.GetID());
		return ERR_FAIL;
		}

	return NOERROR;
	}

CRandomTopologyCreator::ENetworkTypes CRandomTopologyCreator::ParseNetworkType (const CString &sValue)

//	ParseNetworkType
//
//	Parses the network type

	{
	if (sValue.IsBlank())
		return netLine;

	const TStaticStringEntry<ENetworkTypes> *pEntry = NETWORK_TYPE_TABLE.GetAt(sValue);
	if (pEntry == NULL)
		return netUnknown;

	return pEntry->Value;
	}
