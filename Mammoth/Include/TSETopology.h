//	TSETopology.h
//
//	Defines classes and interfaces for stargate topology.
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

const int INFINITE_NODE_DIST =					1000000;
const DWORD END_GAME_SYSTEM_UNID =				0x00ffffff;

class CTopology;
class ITopologyProcessor;

class CTopologyNode
	{
	public:
		struct SAttributeCriteria
			{
			inline CString AsString (void) const
				{
				CMemoryWriteStream Stream;
				if (Stream.Create() != NOERROR)
					return NULL_STR;

				CAttributeCriteria::WriteAsString(Stream, AttribsRequired, CONSTLIT("+"));
				CAttributeCriteria::WriteAsString(Stream, SpecialRequired, CONSTLIT("+"));
				CAttributeCriteria::WriteAsString(Stream, AttribsNotAllowed, CONSTLIT("-"));
				CAttributeCriteria::WriteAsString(Stream, SpecialNotAllowed, CONSTLIT("-"));

				return CString(Stream.GetPointer(), Stream.GetLength());
				}

			inline bool IsEmpty (void) const { return (AttribsRequired.GetCount() == 0 && AttribsNotAllowed.GetCount() == 0 && SpecialRequired.GetCount() == 0 && SpecialNotAllowed.GetCount() == 0); }

			TArray<CString> AttribsRequired;			//	Does not match if any of these attribs are missing
			TArray<CString> AttribsNotAllowed;			//	Does not match if any of these attribs are present
			TArray<CString> SpecialRequired;			//	Special attributes
			TArray<CString> SpecialNotAllowed;			//	Special attributes
			};

		struct SDistanceTo
			{
			SAttributeCriteria AttribCriteria;
			CString sNodeID;

			int iMinDist = 0;
			int iMaxDist = -1;
			};

		struct SCriteriaCtx
			{
			SCriteriaCtx (CTopology &TopologyArg) :
					Topology(TopologyArg)
				{ }

			CTopology &Topology;

			TSortMap<CString, TSortMap<CString, int>> DistanceCache;
			};

		struct SCriteria
			{
			int iChance = 100;						//	Probability 0-100 of matching criteria
			int iMinStargates = 0;					//	Match if >= this many stargates
			int iMaxStargates = -1;					//	Match if <= this many stargates
			int iMinInterNodeDist = 0;				//	Used by <DistributeNodes> (maybe move there)
			int iMaxInterNodeDist = -1;
			SAttributeCriteria AttribCriteria;
			TArray<SDistanceTo> DistanceTo;			//	Matches if node is within the proper distance of another node or nodes

			bool bKnownOnly = false;				//	Only nodes that are known to the player
			};

		struct SStargateDesc
			{
			CString sName;							//	Name of the gate
			CString sDestNode;						//	Destination node
			CString sDestName;						//	Destination entry point

			const TArray<SPoint> *pMidPoints = NULL;	//	Gate line mid-points (optional)
			bool bUncharted = false;				//	Gate is uncharted
			};

		struct SStargateRouteDesc
			{
			Metric GetDistance (void) const
				{
				if (m_rDistance == 0.0
						&& pFromNode
						&& pToNode)
					m_rDistance = pFromNode->GetLinearDistanceTo(pToNode);

				return m_rDistance;
				}

			CTopologyNode *pFromNode = NULL;
			CString sFromName;

			CTopologyNode *pToNode = NULL;
			CString sToName;

			TArray<SPoint> MidPoints;
			bool bOneWay = false;
			bool bUncharted = false;

			private:
				mutable Metric m_rDistance = 0.0;
			};

		CTopologyNode (CTopology &Topology, const CString &sID, DWORD SystemUNID, CSystemMap *pMap);
		~CTopologyNode (void);

		static void CreateFromStream (SUniverseLoadCtx &Ctx, CTopologyNode **retpNode);

		void AddAttributes (const CString &sAttribs);
		ALERROR AddStargate (const SStargateDesc &GateDesc);
		ALERROR AddStargateAndReturn (const SStargateDesc &GateDesc);
		int CalcMatchStrength (const CAttributeCriteria &Criteria);
		bool FindStargate (const CString &sName, CString *retsDestNode = NULL, CString *retsEntryPoint = NULL);
		bool FindStargateTo (const CString &sDestNode, CString *retsName = NULL, CString *retsDestGateID = NULL);
		CString FindStargateName (const CString &sDestNode, const CString &sEntryPoint);
		CString GenerateStargateName (void) const;
		inline const CString &GetAttributes (void) const { return m_sAttributes; }
		inline int GetCalcDistance (void) const { return m_iCalcDistance; }
		inline const CString &GetCreatorID (void) const { return (m_sCreatorID.IsBlank() ? m_sID : m_sCreatorID); }
		inline ICCItemPtr GetData (const CString &sAttrib) const { return m_Data.GetDataAsItem(sAttrib); }
		inline CSystemMap *GetDisplayPos (int *retxPos = NULL, int *retyPos = NULL) const;
		inline const CString &GetEndGameReason (void) { return m_sEndGameReason; }
		inline const CString &GetEpitaph (void) { return m_sEpitaph; }
		inline const CString &GetID (void) const { return m_sID; }
		CTopologyNode *GetGateDest (const CString &sName, CString *retsEntryPoint = NULL);
        DWORD GetLastVisitedTime (void) const;
		inline int GetLevel (void) const { return m_iLevel; }
		Metric GetLinearDistanceTo (const CTopologyNode *pNode) const;
		Metric GetLinearDistanceTo2 (const CTopologyNode *pNode) const;
		ICCItemPtr GetProperty (const CString &sName) const;
		inline int GetStargateCount (void) const { return m_NamedGates.GetCount(); }
		CString GetStargate (int iIndex);
		CTopologyNode *GetStargateDest (int iIndex, CString *retsEntryPoint = NULL) const;
		ICCItemPtr GetStargateProperty (const CString &sName, const CString &sProperty) const;
		void GetStargateRouteDesc (int iIndex, SStargateRouteDesc *retRouteDesc);
		inline CSystem *GetSystem (void) { return m_pSystem; }
		inline DWORD GetSystemID (void) { return m_dwID; }
		inline const CString &GetSystemName (void) const { return m_sName; }
		inline DWORD GetSystemTypeUNID (void) const { return m_SystemUNID; }
		inline CTopology &GetTopology (void) const { return m_Topology; }
		inline CTradingEconomy &GetTradingEconomy (void) { return m_Trading; }
		inline const CTradingEconomy &GetTradingEconomy (void) const { return m_Trading; }
		CUniverse &GetUniverse (void) const;
		inline bool HasAttribute (const CString &sAttrib) { return ::HasModifier(m_sAttributes, sAttrib); }
		bool HasSpecialAttribute (const CString &sAttrib) const;
		inline ICCItemPtr IncData (const CString &sAttrib, ICCItem *pValue = NULL) { return m_Data.IncData(sAttrib, pValue); }
		ALERROR InitFromAdditionalXML (CTopology &Topology, CXMLElement *pDesc, CString *retsError);
		ALERROR InitFromAttributesXML (CXMLElement *pAttributes, CString *retsError);
		ALERROR InitFromSystemXML (CTopology &Topology, CXMLElement *pSystem, CString *retsError);
		inline bool IsCreationDeferred (void) const { return m_bDeferCreate; }
		static bool IsCriteriaAll (const SCriteria &Crit);
		inline bool IsEndGame (void) const { return (m_SystemUNID == END_GAME_SYSTEM_UNID); }
		inline bool IsKnown (void) const { return m_bKnown; }
		inline bool IsMarked (void) const { return m_bMarked; }
		inline bool IsPositionKnown (void) const { return (m_bKnown || m_bPosKnown); }
		bool MatchesAttributeCriteria (const SAttributeCriteria &Crit) const;
		bool MatchesCriteria (SCriteriaCtx &Ctx, const SCriteria &Crit);
		inline void SetCalcDistance (int iDist) const { m_iCalcDistance = iDist; }
		inline void SetCreatorID (const CString &sID) { m_sCreatorID = sID; }
		inline void SetData (const CString &sAttrib, ICCItem *pData) { m_Data.SetData(sAttrib, pData); }
		inline void SetDeferCreate (bool bValue = true) { m_bDeferCreate = bValue; }
		inline void SetEndGameReason (const CString &sReason) { m_sEndGameReason = sReason; }
		inline void SetEpitaph (const CString &sEpitaph) { m_sEpitaph = sEpitaph; }
		inline void SetKnown (bool bKnown = true) { m_bKnown = bKnown; }
		inline void SetLevel (int iLevel) { m_iLevel = iLevel; }
		inline void SetMarked (bool bValue = true) const { m_bMarked = bValue; }
		inline void SetName (const CString &sName) { m_sName = sName; }
		inline void SetPos (int xPos, int yPos) { m_xPos = xPos; m_yPos = yPos; }
		inline void SetPositionKnown (bool bKnown = true) { m_bPosKnown = bKnown; }
		bool SetProperty (const CString &sName, ICCItem *pValue, CString *retsError);
		void SetStargateCharted (const CString &sName, bool bCharted = true);
		void SetStargateDest (const CString &sName, const CString &sDestNode, const CString &sEntryPoint);
		inline void SetSystem (CSystem *pSystem) { m_pSystem = pSystem; }
		inline void SetSystemID (DWORD dwID) { m_dwID = dwID; }
		inline void SetSystemUNID (DWORD dwUNID) { m_SystemUNID = dwUNID; }
		void WriteToStream (IWriteStream *pStream);

		inline void AddVariantLabel (const CString &sVariant) { m_VariantLabels.Insert(sVariant); }
		bool HasVariantLabel (const CString &sVariant);

		static void InitCriteriaCtx (SCriteriaCtx &Ctx, const SCriteria &Criteria);
		static ALERROR ParseAttributeCriteria (const CString &sCriteria, SAttributeCriteria *retCrit);
		static ALERROR ParseCriteria (CXMLElement *pCrit, SCriteria *retCrit, CString *retsError = NULL);
		static ALERROR ParseCriteria (CUniverse &Universe, ICCItem *pItem, SCriteria &retCrit, CString *retsError = NULL);
		static ALERROR ParseCriteria (const CString &sCriteria, SCriteria *retCrit, CString *retsError = NULL);
		static ALERROR ParsePointList (const CString &sValue, TArray<SPoint> *retPoints);
		static ALERROR ParsePosition (const CString &sValue, int *retx, int *rety);
		static ALERROR ParseStargateString (const CString &sStargate, CString *retsNodeID, CString *retsGateName);

	private:

		struct SStargateEntry
			{
			SStargateEntry (void) :
					fUncharted(false)
				{ }

			CString sDestNode;
			CString sDestEntryPoint;

			TArray<SPoint> MidPoints;			//	If the stargate line is curved, these are 
												//	the intermediate points.

			DWORD fUncharted:1;					//	If TRUE, this line is only visible if player used it

			DWORD fSpare2:1;
			DWORD fSpare3:1;
			DWORD fSpare4:1;
			DWORD fSpare5:1;
			DWORD fSpare6:1;
			DWORD fSpare7:1;
			DWORD fSpare8:1;
			DWORD dwSpare:24;

			mutable CTopologyNode *pDestNode = NULL;	//	Cached for efficiency (may be NULL)
			};

		CTopology &m_Topology;					//	Topology that we're a part of
		CString m_sID;							//	ID of node
		CString m_sCreatorID;					//	ID of topology desc, if created by a fragment, etc.

		DWORD m_SystemUNID = 0;					//	UNID of system type
		CString m_sName;						//	Name of system
		int m_iLevel = 0;						//	Level of system

		CSystemMap *m_pMap = NULL;				//	May be NULL
		int m_xPos = 0;							//	Position on map (cartessian)
		int m_yPos = 0;

		TSortMap<CString, SStargateEntry> m_NamedGates;	//	Name to StarGateDesc

		CString m_sAttributes;					//	Attributes
		TArray<CString> m_VariantLabels;		//	Variant labels
		CString m_sEpitaph;						//	Epitaph if this is endgame node
		CString m_sEndGameReason;				//	End game reason if this is endgame node

		CTradingEconomy m_Trading;				//	System trading adjustments
		CAttributeDataBlock m_Data;				//	Opaque data

		CSystem *m_pSystem = NULL;				//	NULL if not yet created
		DWORD m_dwID = 0xffffffff;				//	ID of system instance (default = not yet created)

		bool m_bKnown = false;					//	TRUE if node is visible on galactic map
		bool m_bPosKnown = false;				//	TRUE if node is visible, but type/name is unknown
		bool m_bDeferCreate = false;			//	If TRUE, do not create system at game start

		mutable bool m_bMarked = false;			//	Temp variable used during painting
		mutable int m_iCalcDistance = 0;		//	Temp variable used during distance calc
	};

class CTopologyNodeList
	{
	public:
		inline CTopologyNode *operator [] (int iIndex) const { return m_List.GetAt(iIndex); }

		void Delete (CTopologyNode *pNode);
		inline void Delete (int iIndex) { m_List.Delete(iIndex); }
		inline void DeleteAll (void) { m_List.DeleteAll(); }
		ALERROR Filter (CTopologyNode::SCriteriaCtx &Ctx, CXMLElement *pCriteria, CTopologyNodeList *retList, CString *retsError);
		ALERROR Filter (CTopologyNode::SCriteriaCtx &Ctx, CTopologyNode::SCriteria &Crit, CTopologyNodeList *ioList);
		bool FindNode (CTopologyNode *pNode, int *retiIndex = NULL) const;
		bool FindNode (const CString &sID, int *retiIndex = NULL) const;
		inline CTopologyNode *GetAt (int iIndex) const { return m_List.GetAt(iIndex); }
		inline int GetCount (void) const { return m_List.GetCount(); }
		inline void Insert (CTopologyNode *pNode) { m_List.Insert(pNode); }
		bool IsNodeInRangeOf (CTopologyNode *pNode, int iMin, int iMax, const CTopologyNode::SAttributeCriteria &AttribCriteria, CTopologyNodeList &Checked) const;
		void RestoreMarks (TArray<bool> &Saved);
		void SaveAndSetMarks (bool bMark, TArray<bool> *retSaved);
		inline void Shuffle (void) { m_List.Shuffle(); }

	private:
		TArray<CTopologyNode *> m_List;
	};

enum ENodeDescTypes
	{
	ndNode =			0x01,					//	A single node
	ndNodeTable =		0x02,					//	A table of descriptors (each of which can only appear once)
	ndFragment =		0x03,					//	A topology of nodes (used as fragment)
	ndNetwork =			0x04,					//	A network of nodes (used as fragment)
	ndRandom =			0x05,					//	Randomly generated network
	ndNodeGroup =		0x06,					//	A group of nodes with stargates
	};

class CTopologyDesc
	{
	public:
		enum EInitialStates
			{
			stateUnknown,						//	Node is unknown to player at the beginning
			statePositionKnown,					//	Node position is known, but not type or name
			stateKnown,							//	Node type and name is known
			stateExplored,						//	Node is known and all stations known
			};

		CTopologyDesc (void);
		~CTopologyDesc (void);
		ALERROR BindDesign (SDesignLoadCtx &Ctx);
		CEffectCreator *FindEffectCreator (const CString &sUNID);
		CString GetAttributes (void);
		inline CXMLElement *GetDesc (void) const { return m_pDesc; }
		inline const CString &GetID (void) const { return m_sID; }
		inline EInitialStates GetInitialState (void) const { return m_iInitialState; }
		inline CEffectCreator *GetLabelEffect (void) const { return m_pLabelEffect; }
		inline CSystemMap *GetMap (void) const { return m_pMap; }
		inline CEffectCreator *GetMapEffect (void) const { return m_pMapEffect; }
		CXMLElement *GetNameDesc (void) const;
		bool GetPos (int *retx, int *rety);
		CXMLElement *GetSystemDesc (void) const;
		inline CTopologyDesc *GetTopologyDesc (int iIndex) const;
		inline int GetTopologyDescCount (void) const;
		inline CTopologyDescTable *GetTopologyDescTable (void) { return m_pDescList; }
		inline ENodeDescTypes GetType (void) const { return m_iType; }
		inline bool IsAbsoluteNode (void) const { return (*m_sID.GetASCIIZPointer() != '+'); }
		bool IsEndGameNode (CString *retsEpitaph = NULL, CString *retsReason = NULL) const;
		inline bool IsRootNode (void) const { return ((m_dwFlags & FLAG_IS_ROOT_NODE) ? true : false); }
		ALERROR LoadFromXML (SDesignLoadCtx &Ctx, CXMLElement *pXMLDesc, CSystemMap *pMap, const CString &sParentUNID);
		inline void SetRootNode (void) { m_dwFlags |= FLAG_IS_ROOT_NODE; }

	private:
		enum Flags
			{
			FLAG_IS_ROOT_NODE = 0x00000001,
			};

		CSystemMap *m_pMap;						//	Map that contains this descriptor (may be NULL)
		CString m_sID;							//	ID of node
		ENodeDescTypes m_iType;					//	Type of node
		CXMLElement *m_pDesc;					//	XML for node definition
		EInitialStates m_iInitialState;			//	Initial state of node (known to player or not)
		DWORD m_dwFlags;

		CEffectCreatorRef m_pLabelEffect;		//	Effect to paint on label layer
		CEffectCreatorRef m_pMapEffect;			//	Effect to paint on galactic map

		CTopologyDescTable *m_pDescList;		//	Some node types (e.g., ndNetwork) have sub-nodes.
												//		This is a table of all subnodes
	};

class CTopologyDescTable
	{
	public:
		CTopologyDescTable (void);
		~CTopologyDescTable (void);

		ALERROR AddRootNode (SDesignLoadCtx &Ctx, const CString &sNodeID);
		ALERROR BindDesign (SDesignLoadCtx &Ctx);
		void CleanUp (void);
		inline void DeleteIDMap (void) { delete m_pIDToDesc; m_pIDToDesc = NULL; }
		CEffectCreator *FindEffectCreator (const CString &sUNID);
		CTopologyDesc *FindTopologyDesc (const CString &sID);
		CXMLElement *FindTopologyDescXML (const CString &sNodeID);
		CTopologyDesc *FindRootNodeDesc (const CString &sID);
		CXMLElement *FindRootNodeDescXML (const CString &sNodeID);
		inline const CString &GetFirstNodeID (void) { return m_sFirstNode; }
		inline int GetRootNodeCount (void) { return m_RootNodes.GetCount(); }
		inline CTopologyDesc *GetRootNodeDesc (int iIndex) { return m_RootNodes[iIndex]; }
		inline CXMLElement *GetRootNodeDescXML (int iIndex) { return m_RootNodes[iIndex]->GetDesc(); }
		inline CTopologyDesc *GetTopologyDesc (int iIndex) { return m_Table[iIndex]; }
		inline int GetTopologyDescCount (void) { return m_Table.GetCount(); }
		ALERROR LoadFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CSystemMap *pMap, const CString &sParentUNID, bool bAddFirstAsRoot = false);
		ALERROR LoadNodeFromXML (SDesignLoadCtx &Ctx, CXMLElement *pNode, CSystemMap *pMap, const CString &sParentUNID, CTopologyDesc **retpNode = NULL);

	private:
		void InitIDMap (void);

		TArray<CTopologyDesc *> m_Table;
		TMap<CString, CTopologyDesc *> *m_pIDToDesc;
		TArray<CTopologyDesc *> m_RootNodes;

		CString m_sFirstNode;						//	Node where player starts (if not specified elsewhere)
	};

inline CTopologyDesc *CTopologyDesc::GetTopologyDesc (int iIndex) const { return (m_pDescList ? m_pDescList->GetTopologyDesc(iIndex) : NULL); }
inline int CTopologyDesc::GetTopologyDescCount (void) const { return (m_pDescList ? m_pDescList->GetTopologyDescCount() : 0); }

struct STopologyCreateCtx
	{
	CString ExpandNodeID (const CString &sID) const;
	void GetAbsoluteDisplayPos (int x, int y, int *retx, int *rety, int *retiRotation) const;
	void GetFragmentDisplayPos (CTopologyNode *pNode, int *retx, int *rety) const;
	void GetFragmentEntranceDisplayPos (int *retx, int *rety) const;

	CSystemMap *pMap = NULL;						//	Map that we're currently processing
	TArray<CTopologyDescTable *> Tables;			//	List of tables to look up
	CTopologyDescTable *pFragmentTable = NULL;
	CTopologyNodeList *pNodesAdded = NULL;			//	Output of nodes added

	CTopologyNode *pPrevNode = NULL;
	bool bInFragment = false;						//	Fragments use local coordinates
	CString sFragmentPrefix;
	int xOffset = 0;
	int yOffset = 0;
	int iRotation = 0;
	CString sFragmentExitID;
	CString sFragmentExitGate;
	CString sFragmentAttributes;

	CXMLElement *pGateDesc = NULL;					//	Gate descriptor (when adding a node)
	CString sOtherNodeID;
	CString sOtherNodeEntryPoint;

	CString sError;
	};

//	Topology

class CTopology
	{
	public:
		static const int UNKNOWN_DISTANCE =	-1;

		struct SNodeCreateCtx
			{
			SNodeCreateCtx (void) :
					xPos(0),
					yPos(0),
					pSystemDesc(NULL),
					pEffect(NULL),
					iInitialState(CTopologyDesc::stateUnknown),
					bNoMap(false)
				{ }

			int xPos;						//	Position on destination map
			int yPos;

			CString sAttribs;				//	Attributes for new node
			CXMLElement *pSystemDesc;		//	System definition (we sometimes pull properties from here).
			CEffectCreator *pEffect;		//	Annotations on system map for this node

			CTopologyDesc::EInitialStates iInitialState;	//	Is the new node know or unknown
			bool bNoMap;					//	No destination map
			};

		CTopology (CUniverse &Universe);
		~CTopology (void);

		ALERROR AddStargateFromXML (STopologyCreateCtx &Ctx, CXMLElement *pDesc, CTopologyNode *pNode = NULL, bool bRootNode = false);
		ALERROR AddStargateRoute (const CTopologyNode::SStargateRouteDesc &Desc);
		ALERROR AddStargateRoutes (const TArray<CTopologyNode::SStargateRouteDesc> &Gates);
		ALERROR AddTopology (STopologyCreateCtx &Ctx);
		ALERROR AddTopologyDesc (STopologyCreateCtx &Ctx, CTopologyDesc *pNode, CTopologyNode **retpNewNode = NULL);
		ALERROR AddTopologyNode (STopologyCreateCtx &Ctx, const CString &sNodeID, CTopologyNode **retpNewNode = NULL);
		void CalcDistances (const CTopologyNode *pSrc, TSortMap<CString, int> &retDistances) const;
		void CalcDistances (const TArray<const CTopologyNode *> &Src, TSortMap<CString, int> &retDistances) const;
		ALERROR CreateTopologyNode (STopologyCreateCtx &Ctx, const CString &sID, SNodeCreateCtx &NodeCtx, CTopologyNode **retpNode = NULL);
		void DeleteAll (void);
		bool FindNearestNodeCreatedBy (const CString &sID, CTopologyNode *pNode, CTopologyNode **retpNewNode = NULL) const;
		bool FindTopologyDesc (STopologyCreateCtx &Ctx, const CString &sNodeID, CTopologyDesc **retpNode) const;
		CTopologyNode *FindTopologyNode (const CString &sID) const;
		CString GenerateUniquePrefix (const CString &sPrefix, const CString &sTestNodeID);
		int GetDistance (const CTopologyNode *pSrc, const CTopologyNode *pTarget) const;
		int GetDistance (const CString &sSourceID, const CString &sDestID) const;
		int GetDistanceToCriteria (const CTopologyNode *pSrc, const CTopologyNode::SAttributeCriteria &Criteria) const;
		int GetDistanceToCriteriaNoMatch (const CTopologyNode *pSrc, const CTopologyNode::SAttributeCriteria &Criteria) const;
		inline CTopologyNodeList &GetTopologyNodeList (void) { return m_Topology; }
		inline CTopologyNode *GetTopologyNode (int iIndex) const { return m_Topology.GetAt(iIndex); }
		inline int GetTopologyNodeCount (void) const { return m_Topology.GetCount(); }
		inline CUniverse &GetUniverse (void) const { return m_Universe; }
		inline DWORD GetVersion (void) const { return m_dwVersion; }
		bool InDebugMode (void) const;
		ALERROR InitComplexArea (CXMLElement *pAreaDef, int iMinRadius, CComplexArea *retArea, STopologyCreateCtx *pCtx = NULL, CTopologyNode **iopExit = NULL); 
		void ReadFromStream (SUniverseLoadCtx &Ctx);
		ALERROR RunProcessors (CSystemMap *pMap, const TSortMap<int, TArray<ITopologyProcessor *>> &Processors, CTopologyNodeList &Nodes, CString *retsError);

	private:
		enum NodeTypes
			{
			typeStandard,
			typeFragmentStart,
			typeFragment,
			};

		ALERROR AddFragment (STopologyCreateCtx &Ctx, CTopologyDesc *pFragment, CTopologyNode **retpNewNode);
		ALERROR AddNode (STopologyCreateCtx &Ctx, CTopologyDesc *pNode, CTopologyNode **retpNewNode);
		ALERROR AddNodeGroup (STopologyCreateCtx &Ctx, CTopologyDesc *pTable, CTopologyNode **retpNewNode);
		ALERROR AddNodeTable (STopologyCreateCtx &Ctx, CTopologyDesc *pTable, CTopologyNode **retpNewNode);

		ALERROR AddRandomParsePosition (STopologyCreateCtx *pCtx, const CString &sValue, CTopologyNode **iopExit, int *retx, int *rety);

		ALERROR AddStargate (STopologyCreateCtx &Ctx, CTopologyNode *pNode, bool bRootNode, CXMLElement *pGateDesc);
		ALERROR AddTopologyNode (const CString &sID, CTopologyNode *pNode);
		bool FindTopologyDesc (STopologyCreateCtx &Ctx, const CString &sNodeID, CTopologyDesc **retpNode, NodeTypes *retiNodeType) const;
		ALERROR GetOrAddTopologyNode (STopologyCreateCtx &Ctx, const CString &sID, CTopologyNode **retpNode);

		CUniverse &m_Universe;
		CTopologyNodeList m_Topology;
		TSortMap<CString, int> m_IDToNode;
		DWORD m_dwVersion = 1;
	};
