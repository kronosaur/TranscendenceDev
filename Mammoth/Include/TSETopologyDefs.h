//	TSETopologyDefs.h
//
//	Defines classes and interfaces for stargate topology.
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CTopology;
class CTopologyNode;
class ITopologyProcessor;

class CTopologyAttributeCriteria
	{
	public:
		CTopologyAttributeCriteria (void) { }
		explicit CTopologyAttributeCriteria (const CString &sCriteria) { Parse(sCriteria); }

		CString AsString (void) const;
		ALERROR Init (const CString &sCriteria) { *this = CTopologyAttributeCriteria(); return Parse(sCriteria); }
		bool IsEmpty (void) const;
		bool Matches (const CTopologyNode &Node) const;

	private:
		ALERROR Parse (const CString &sCriteria);
		ALERROR ParseSubExpression (const char *pPos);
		void WriteSubExpression (CMemoryWriteStream &Stream) const;
		static void WriteAsString (IWriteStream &Stream, const TArray<CString> &Attribs, const CString &sPrefix, bool &iobSpaceNeeded);

		TArray<CString> m_AttribsRequired;			//	Does not match if any of these attribs are missing
		TArray<CString> m_AttribsNotAllowed;		//	Does not match if any of these attribs are present
		TArray<CString> m_SpecialRequired;			//	Special attributes
		TArray<CString> m_SpecialNotAllowed;		//	Special attributes

		CIntegerRangeCriteria m_Level;				//	Required level

		TUniquePtr<CTopologyAttributeCriteria> m_pOr;
	};

class CTopologyNodeCriteria
	{
	public:
		struct SDistanceTo
			{
			CTopologyAttributeCriteria AttribCriteria;
			CString sNodeID;

			int iMinDist = 0;
			int iMaxDist = -1;
			};

		struct SCtx
			{
			SCtx (CTopology &TopologyArg) :
					Topology(TopologyArg)
				{ }

			CTopology &Topology;

			TSortMap<CString, TSortMap<CString, int>> DistanceCache;
			};

		const CTopologyAttributeCriteria &GetAttributeCriteria (void) const { return m_AttribCriteria; }
		int GetMaxInterNodeDist (void) const { return m_iMaxInterNodeDist; }
		int GetMinInterNodeDist (void) const { return m_iMinInterNodeDist; }
		void InitCtx (SCtx &Ctx) const;
		bool IsCriteriaAll (void) const;
		bool Matches (SCtx &Ctx, const CTopologyNode &Node) const;
		ALERROR Parse (const CXMLElement &Crit, CString *retsError = NULL);
		ALERROR Parse (CUniverse &Universe, const ICCItem &Item, CString *retsError = NULL);
		ALERROR Parse (const CString &sCriteria, CString *retsError = NULL);

	private:
		int m_iChance = 100;						//	Probability 0-100 of matching criteria
		int m_iMinStargates = 0;					//	Match if >= this many stargates
		int m_iMaxStargates = -1;					//	Match if <= this many stargates
		int m_iMinInterNodeDist = 0;				//	Used by <DistributeNodes> (maybe move there)
		int m_iMaxInterNodeDist = -1;
		CTopologyAttributeCriteria m_AttribCriteria;
		TArray<SDistanceTo> m_DistanceTo;			//	Matches if node is within the proper distance of another node or nodes

		bool m_bKnownOnly = false;					//	Only nodes that are known to the player
	};
