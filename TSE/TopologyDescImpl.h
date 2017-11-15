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