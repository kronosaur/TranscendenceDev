//	CSimplePathStargateGenerator.cpp
//
//	CSimplePathStargateGenerator class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CSimplePathStargateGenerator::CSimplePathStargateGenerator (CIntGraph &Graph, const TArray<CTopologyNode *> &Nodes) :
		m_Graph(Graph),
		m_Nodes(Nodes)

//	CSimplePathStargateGenerator constructor

	{
	}

bool CSimplePathStargateGenerator::FindConnection (const CLargeSet &Connected, const CLargeSet &Unconnected, DWORD *retdwFrom, DWORD *retdwTo) const

//	FindConnection
//
//	Finds a connection from the connected set to the unconnected set.

	{
	int i;

	//	Loop over nodes in the connected set until we find the shortest connection.

	DWORD dwBestNodeID = CLargeSet::INVALID_VALUE;
	DWORD dwBestToID;
	Metric rBestDist;

	DWORD dwNodeID = Connected.GetNextValue(0);
	while (dwNodeID != CLargeSet::INVALID_VALUE)
		{
		//	Loop over all connections

		TArray<DWORD> To;
		m_Graph.GetNodeConnections(dwNodeID, &To);

		for (i = 0; i < To.GetCount(); i++)
			{
			//	If this is a connection to the unconnected set, then see of it
			//	is shorted than what we've got.

			if (Unconnected.IsSet(To[i]))
				{
				Metric rDist = GetDistance(dwNodeID, To[i]);
				if (dwBestNodeID == CLargeSet::INVALID_VALUE || rDist < rBestDist)
					{
					dwBestNodeID = dwNodeID;
					dwBestToID = To[i];
					rBestDist = rDist;
					}
				}
			}

		//	Keep looping

		dwNodeID = Connected.GetNextValue(dwNodeID + 1);
		}

	//	Return the best node

	if (dwBestNodeID == CLargeSet::INVALID_VALUE)
		return false;

	*retdwFrom = dwBestNodeID;
	*retdwTo = dwBestToID;

	return true;
	}

void CSimplePathStargateGenerator::Generate (DWORD dwFirstNode, TArray<CTopologyNode::SStargateRouteDesc> &Routes)

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

	while (!Unconnected.IsEmpty())
		{
		//	Find a node in the connected set that has a connection to a node
		//	in the unconnected set.

		DWORD dwFrom, dwTo;
		if (!FindConnection(Connected, Unconnected, &dwFrom, &dwTo))
			return;

		//	Add a route

		CTopologyNode::SStargateRouteDesc *pRouteDesc = Routes.Insert();
		pRouteDesc->pFromNode = m_Nodes[m_Graph.GetNodeIndex(dwFrom)];
		pRouteDesc->pToNode = m_Nodes[m_Graph.GetNodeIndex(dwTo)];

		//	The To node is now connected

		Unconnected.Clear(dwTo);
		Connected.Set(dwTo);
		}
	}

Metric CSimplePathStargateGenerator::GetDistance (DWORD dwFrom, DWORD dwTo) const

//	GetDistance
//
//	Returns the linear distance between the two nodes.

	{
	TSortMap<DWORD, Metric> *pTable = m_Distance.SetAt(dwFrom);
	bool bNotFound;
	Metric *pDist = pTable->SetAt(dwTo, &bNotFound);
	if (bNotFound)
		*pDist = m_Nodes[m_Graph.GetNodeIndex(dwFrom)]->GetLinearDistanceTo2(m_Nodes[m_Graph.GetNodeIndex(dwTo)]);

	return *pDist;
	}
