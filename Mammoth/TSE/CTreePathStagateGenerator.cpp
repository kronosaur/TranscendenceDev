//	CTreePathStargateGenerator.cpp
//
//	CTreePathStargateGenerator class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CTreePathStargateGenerator::CTreePathStargateGenerator (CIntGraph &Graph, const TArray<CTopologyNode *> &Nodes) :
		m_Graph(Graph),
		m_Nodes(Nodes)

//	CTreePathStargateGenerator constructor

	{
	}

DWORD CTreePathStargateGenerator::FindConnection (DWORD dwFrom, const CLargeSet &Unconnected) const

//	FindConnection
//
//	Returns a connection from dwFrom to some node in the Unconnected set.
//	We return CLargeSet::INVALID_VALUE if we cannot find a connection.

	{
	int i;

	//	Loop over all connections

	TArray<DWORD> To;
	m_Graph.GetNodeConnections(dwFrom, &To);

	for (i = 0; i < To.GetCount(); i++)
		{
		//	If this is a connection to the unconnected set, then we've found
		//	it.

		if (Unconnected.IsSet(To[i]))
			return To[i];
		}

	//	Otherwise, not found

	return CLargeSet::INVALID_VALUE;
	}

void CTreePathStargateGenerator::Generate (DWORD dwFirstNode, TArray<CTopologyNode::SStargateRouteDesc> &Routes)

//	Generate
//
//	Generate routes
//
//	LATER: This call alters the internal state so that we cannot call it again. 
//	In the future we should change the code in CIntGraph to generate an external
//	set of connections.

	{
	int i;

	//	Generate a Delaunay graph from all the nodes. This is a fully connected
	//	tessellation of all the nodes.

	m_Graph.GenerateDelaunayConnections();

	//	We start with two sets a connected set and an unconnected set. We
	//	move items from the connected set to the other.

	CLargeSet Connected;
	Connected.Set(dwFirstNode);

	CLargeSet Unconnected;
	for (i = 0; i < m_Nodes.GetCount(); i++)
		{
		DWORD dwNodeID = m_Graph.GetNodeID(i);
		if (dwNodeID != dwFirstNode)
			Unconnected.Set(dwNodeID);
		}

	//	Loop until there are no more unconnected nodes

	DWORD dwNodeID = dwFirstNode;
	while (!Unconnected.IsEmpty())
		{
		//	Find a node in the connected set that has a connection to the 
		//	current node. If we find one, we add it and keep looping.

		DWORD dwTo = FindConnection(dwNodeID, Unconnected);
		if (dwTo != CLargeSet::INVALID_VALUE)
			{
			CTopologyNode::SStargateRouteDesc *pRouteDesc = Routes.Insert();
			pRouteDesc->pFromNode = m_Nodes[m_Graph.GetNodeIndex(dwNodeID)];
			pRouteDesc->pToNode = m_Nodes[m_Graph.GetNodeIndex(dwTo)];

			//	The To node is now connected

			Unconnected.Clear(dwTo);
			Connected.Set(dwTo);

			continue;
			}

		//	If this node has no connections, then try the next node in
		//	the connected set.

		dwNodeID = Connected.GetNextValue(dwNodeID + 1);

		//	If we've hit the end, wrap around. Since we've still got 
		//	unconnected nodes, we must find something.

		if (dwNodeID == CLargeSet::INVALID_VALUE)
			dwNodeID = Connected.GetNextValue(0);
		}
	}
