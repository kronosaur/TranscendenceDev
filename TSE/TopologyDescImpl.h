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
		enum ENetworkTypes
			{
			netUnknown,

			netWeb,							//	Full tessellation
			netTree,						//	Greedy algorithm from starting node
			netLine,						//	Minimal network for full connection
			};

		CRandomTopologyCreator (const CTopologyDesc &Desc);

		ALERROR Create (STopologyCreateCtx &Ctx, CTopology &Topology, CTopologyNode **retpNode = NULL) const;
		ALERROR FindExistingNode (STopologyCreateCtx &Ctx, CTopology &Topology, CTopologyNode *pExistingNode, CTopologyNode **retpNode) const;

	private:
		ALERROR AddRandomRegion (STopologyCreateCtx &Ctx, CTopology &Topology, CXMLElement *pRegionDef, CTopologyNode *&pExitNode, CIntGraph &Graph, TArray<CTopologyNode *> &Nodes) const;
		ALERROR ApplyNodeTemplate (STopologyCreateCtx &Ctx, CTopology &Topology, const TArray<CTopologyNode *> &Nodes, CXMLElement *pNodeTemplate, bool bUnmarkedOnly) const;
		ALERROR ApplyRandomNodeParams (STopologyCreateCtx &Ctx, CTopology &Topology, const TArray<CTopologyNode *> &Nodes) const;

		static ENetworkTypes ParseNetworkType (const CString &sValue);

		const CTopologyDesc &m_Desc;
	};

class CTopologySystemDesc
	{
	public:
		void Apply (CTopology &Topology, CTopologyNode *pNode) const;
		ALERROR InitFromXML (SDesignLoadCtx &LoadCtx, CXMLElement *pDesc);
		inline bool IsEmpty (void) const { return m_bEmpty; }

	private:
		bool m_bEmpty = true;				//	If TRUE, no settings here.
		DWORD m_dwSystemUNID = 0;			//	System UNID (0 = not set)
		CString m_sName;					//	System name 
		int m_iLevel = 0;					//	System level (0 = not set)
		CString m_sAttributes;				//	System attributes
		CString m_sVariantFromParent;		//	System variant (from parent)
		CString m_sVariantFromSub;			//	System variant (from sub-element)

		TUniquePtr<IElementGenerator> m_pGenerator;
		CNameDesc m_Names;
	};

class CDelaunayStargateGenerator
	{
	public:
		CDelaunayStargateGenerator (CIntGraph &Graph, const TArray<CTopologyNode *> &Nodes);

		void Generate (TArray<CTopologyNode::SStargateRouteDesc> &Routes);

	private:
		CIntGraph &m_Graph;
		const TArray<CTopologyNode *> &m_Nodes;
	};

class CSimplePathStargateGenerator
	{
	public:
		CSimplePathStargateGenerator (CIntGraph &Graph, const TArray<CTopologyNode *> &Nodes);

		void Generate (DWORD dwFirstNode, TArray<CTopologyNode::SStargateRouteDesc> &Routes);

	private:
		bool FindConnection (const CLargeSet &Connected, const CLargeSet &Unconnected, DWORD *retdwFrom, DWORD *retdwTo) const;
		Metric GetDistance (DWORD dwFrom, DWORD dwTo) const;

		CIntGraph &m_Graph;
		const TArray<CTopologyNode *> &m_Nodes;

		mutable TSortMap<DWORD, TSortMap<DWORD, Metric>> m_Distance;
	};

class CTreePathStargateGenerator
	{
	public:
		CTreePathStargateGenerator (CIntGraph &Graph, const TArray<CTopologyNode *> &Nodes);

		void Generate (DWORD dwFirstNode, TArray<CTopologyNode::SStargateRouteDesc> &Routes);

	private:
		DWORD FindConnection (DWORD dwFrom, const CLargeSet &Unconnected) const;

		CIntGraph &m_Graph;
		const TArray<CTopologyNode *> &m_Nodes;
	};
