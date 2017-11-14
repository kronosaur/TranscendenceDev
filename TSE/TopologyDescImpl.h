//	TopologyDescImpl.h
//
//	Topology implementation helpers
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CNetworkTopologyCreator
	{
	public:
		CNetworkTopologyCreator (const CTopologyDesc &Desc);

		ALERROR Create (STopologyCreateCtx &Ctx, CTopology &Topology, CTopologyNode **retpNode = NULL) const;

	private:
		const CTopologyDesc &m_Desc;
	};

class CRandomTopologyCreator
	{
	public:
		CRandomTopologyCreator (const CTopologyDesc &Desc);

		ALERROR Create (STopologyCreateCtx &Ctx, CTopology &Topology, CTopologyNode **retpNode = NULL) const;

	private:
		ALERROR AddRandomRegion (STopologyCreateCtx &Ctx, CTopology &Topology, CXMLElement *pRegionDef, CTopologyNode *&pExitNode, CIntGraph &Graph, TArray<CTopologyNode *> &Nodes) const;
		ALERROR ApplyNodeTemplate (STopologyCreateCtx &Ctx, CTopology &Topology, const TArray<CTopologyNode *> &Nodes, CXMLElement *pNodeTemplate, bool bUnmarkedOnly) const;
		ALERROR ApplyRandomNodeParams (STopologyCreateCtx &Ctx, CTopology &Topology, const TArray<CTopologyNode *> &Nodes) const;

		const CTopologyDesc &m_Desc;
	};
