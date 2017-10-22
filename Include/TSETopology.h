//	TSETopology.h
//
//	Defines classes and interfaces for stargate topology.
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

const int INFINITE_NODE_DIST =					1000000;
const DWORD END_GAME_SYSTEM_UNID =				0x00ffffff;

class CTopologyNode
	{
	public:
		struct SDistanceTo
			{
			TArray<CString> AttribsRequired;
			TArray<CString> AttribsNotAllowed;

			CString sNodeID;

			int iMinDist;
			int iMaxDist;
			};

		struct SCriteriaCtx
			{
			SCriteriaCtx (void) :
					pTopology(NULL)
				{ }

			CTopology *pTopology;
			};

		struct SCriteria
			{
			int iChance;								//	Probability 0-100 of matching criteria
			int iMinStargates;							//	Match if >= this many stargates
			int iMaxStargates;							//	Match if <= this many stargates
			int iMinInterNodeDist;						//	Used by <DistributeNodes> (maybe move there)
			int iMaxInterNodeDist;
			TArray<CString> AttribsRequired;			//	Does not match if any of these attribs are missing
			TArray<CString> AttribsNotAllowed;			//	Does not match if any of these attribs are present
			TArray<SDistanceTo> DistanceTo;				//	Matches if node is within the proper distance of another node or nodes
			TArray<CString> SpecialRequired;			//	Special attributes
			TArray<CString> SpecialNotAllowed;			//	Special attributes
			};

		struct SStargateRouteDesc
			{
			SStargateRouteDesc (void) :
					pFromNode(NULL),
					pToNode(NULL),
					bOneWay(false)
				{ }

			CTopologyNode *pFromNode;
			CString sFromName;

			CTopologyNode *pToNode;
			CString sToName;

			TArray<SPoint> MidPoints;
			bool bOneWay;
			};

		CTopologyNode (const CString &sID, DWORD SystemUNID, CSystemMap *pMap);
		~CTopologyNode (void);
		static void CreateFromStream (SUniverseLoadCtx &Ctx, CTopologyNode **retpNode);

		void AddAttributes (const CString &sAttribs);
		ALERROR AddStargate (const CString &sName, const CString &sDestNode, const CString &sDestEntryPoint, const SStargateRouteDesc &RouteDesc);
		ALERROR AddStargateAndReturn (const CString &sGateID, const CString &sDestNodeID, const CString &sDestGateID);
		int CalcMatchStrength (const CAttributeCriteria &Criteria);
		bool FindStargate (const CString &sName, CString *retsDestNode = NULL, CString *retsEntryPoint = NULL);
		bool FindStargateTo (const CString &sDestNode, CString *retsName = NULL, CString *retsDestGateID = NULL);
		CString FindStargateName (const CString &sDestNode, const CString &sEntryPoint);
		inline const CString &GetAttributes (void) const { return m_sAttributes; }
		inline int GetCalcDistance (void) const { return m_iCalcDistance; }
		inline const CString &GetData (const CString &sAttrib) const { return m_Data.GetData(sAttrib); }
		inline CSystemMap *GetDisplayPos (int *retxPos = NULL, int *retyPos = NULL);
		inline const CString &GetEndGameReason (void) { return m_sEndGameReason; }
		inline const CString &GetEpitaph (void) { return m_sEpitaph; }
		inline const CString &GetID (void) const { return m_sID; }
		CTopologyNode *GetGateDest (const CString &sName, CString *retsEntryPoint = NULL);
        DWORD GetLastVisitedTime (void) const;
		inline int GetLevel (void) { return m_iLevel; }
		ICCItem *GetProperty (const CString &sName);
		inline int GetStargateCount (void) { return m_NamedGates.GetCount(); }
		CString GetStargate (int iIndex);
		CTopologyNode *GetStargateDest (int iIndex, CString *retsEntryPoint = NULL);
		void GetStargateRouteDesc (int iIndex, SStargateRouteDesc *retRouteDesc);
		inline CSystem *GetSystem (void) { return m_pSystem; }
		inline DWORD GetSystemID (void) { return m_dwID; }
		inline const CString &GetSystemName (void) { return m_sName; }
		inline DWORD GetSystemTypeUNID (void) { return m_SystemUNID; }
		inline bool HasAttribute (const CString &sAttrib) { return ::HasModifier(m_sAttributes, sAttrib); }
		bool HasSpecialAttribute (const CString &sAttrib) const;
		inline void IncData (const CString &sAttrib, ICCItem *pValue = NULL, ICCItem **retpNewValue = NULL) { m_Data.IncData(sAttrib, pValue, retpNewValue); }
		ALERROR InitFromAdditionalXML (CXMLElement *pDesc, CString *retsError);
		ALERROR InitFromAttributesXML (CXMLElement *pAttributes, CString *retsError);
		ALERROR InitFromSystemXML (CXMLElement *pSystem, CString *retsError);
		static bool IsCriteriaAll (const SCriteria &Crit);
		inline bool IsEndGame (void) const { return (m_SystemUNID == END_GAME_SYSTEM_UNID); }
		inline bool IsKnown (void) const { return m_bKnown; }
		inline bool IsMarked (void) const { return m_bMarked; }
		inline bool IsPositionKnown (void) const { return (m_bKnown || m_bPosKnown); }
		bool MatchesCriteria (SCriteriaCtx &Ctx, const SCriteria &Crit);
		inline void SetCalcDistance (int iDist) { m_iCalcDistance = iDist; }
		inline void SetData (const CString &sAttrib, const CString &sData) { m_Data.SetData(sAttrib, sData); }
		inline void SetEndGameReason (const CString &sReason) { m_sEndGameReason = sReason; }
		inline void SetEpitaph (const CString &sEpitaph) { m_sEpitaph = sEpitaph; }
		inline void SetKnown (bool bKnown = true) { m_bKnown = bKnown; }
		inline void SetLevel (int iLevel) { m_iLevel = iLevel; }
		inline void SetMarked (bool bValue = true) { m_bMarked = bValue; }
		inline void SetName (const CString &sName) { m_sName = sName; }
		inline void SetPos (int xPos, int yPos) { m_xPos = xPos; m_yPos = yPos; }
		inline void SetPositionKnown (bool bKnown = true) { m_bPosKnown = bKnown; }
		bool SetProperty (const CString &sName, ICCItem *pValue, CString *retsError);
		void SetStargateDest (const CString &sName, const CString &sDestNode, const CString &sEntryPoint);
		inline void SetSystem (CSystem *pSystem) { m_pSystem = pSystem; }
		inline void SetSystemID (DWORD dwID) { m_dwID = dwID; }
		inline void SetSystemUNID (DWORD dwUNID) { m_SystemUNID = dwUNID; }
		void WriteToStream (IWriteStream *pStream);

		inline void AddVariantLabel (const CString &sVariant) { m_VariantLabels.Insert(sVariant); }
		bool HasVariantLabel (const CString &sVariant);

		static ALERROR CreateStargateRoute (const SStargateRouteDesc &Desc);
		static ALERROR ParseCriteria (CXMLElement *pCrit, SCriteria *retCrit, CString *retsError = NULL);
		static ALERROR ParseCriteria (const CString &sCriteria, SCriteria *retCrit, CString *retsError = NULL);
		static ALERROR ParseCriteriaInt (const CString &sCriteria, SCriteria *retCrit);
		static ALERROR ParsePointList (const CString &sValue, TArray<SPoint> *retPoints);
		static ALERROR ParsePosition (const CString &sValue, int *retx, int *rety);
		static ALERROR ParseStargateString (const CString &sStargate, CString *retsNodeID, CString *retsGateName);

	private:

		struct StarGateDesc
			{
			StarGateDesc (void) :
					fMinor(false),
					pDestNode(NULL)
				{ }

			CString sDestNode;
			CString sDestEntryPoint;

			TArray<SPoint> MidPoints;			//	If the stargate line is curved, these are 
												//	the intermediate points.

			DWORD fMinor:1;						//	If TRUE, this is a minor stargate path

			DWORD fSpare2:1;
			DWORD fSpare3:1;
			DWORD fSpare4:1;
			DWORD fSpare5:1;
			DWORD fSpare6:1;
			DWORD fSpare7:1;
			DWORD fSpare8:1;
			DWORD dwSpare:24;

			CTopologyNode *pDestNode;			//	Cached for efficiency (may be NULL)
			};

		CString GenerateStargateName (void);

		CString m_sID;							//	ID of node

		DWORD m_SystemUNID;						//	UNID of system type
		CString m_sName;						//	Name of system
		int m_iLevel;							//	Level of system

		CSystemMap *m_pMap;						//	May be NULL
		int m_xPos;								//	Position on map (cartessian)
		int m_yPos;

		TSortMap<CString, StarGateDesc> m_NamedGates;	//	Name to StarGateDesc

		CString m_sAttributes;					//	Attributes
		TArray<CString> m_VariantLabels;		//	Variant labels
		CString m_sEpitaph;						//	Epitaph if this is endgame node
		CString m_sEndGameReason;				//	End game reason if this is endgame node

		CAttributeDataBlock m_Data;				//	Opaque data

		CSystem *m_pSystem;						//	NULL if not yet created
		DWORD m_dwID;							//	ID of system instance

		bool m_bKnown;							//	TRUE if node is visible on galactic map
		bool m_bPosKnown;						//	TRUE if node is visible, but type/name is unknown

		bool m_bMarked;							//	Temp variable used during painting
		int m_iCalcDistance;					//	Temp variable used during distance calc
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
		bool FindNode (CTopologyNode *pNode, int *retiIndex = NULL);
		bool FindNode (const CString &sID, int *retiIndex = NULL);
		inline CTopologyNode *GetAt (int iIndex) const { return m_List.GetAt(iIndex); }
		inline int GetCount (void) const { return m_List.GetCount(); }
		inline void Insert (CTopologyNode *pNode) { m_List.Insert(pNode); }
		bool IsNodeInRangeOf (CTopologyNode *pNode, int iMin, int iMax, const TArray<CString> &AttribsRequired, const TArray<CString> &AttribsNotAllowed, CTopologyNodeList &Checked);
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
		inline CEffectCreator *GetLabelEffect (void) const { return m_pLabelEffect; }
		inline CSystemMap *GetMap (void) const { return m_pMap; }
		inline CEffectCreator *GetMapEffect (void) const { return m_pMapEffect; }
		inline const CString &GetID (void) const { return m_sID; }
		inline EInitialStates GetInitialState (void) const { return m_iInitialState; }
		bool GetPos (int *retx, int *rety);
		CXMLElement *GetSystem (void);
		inline CTopologyDesc *GetTopologyDesc (int iIndex);
		inline int GetTopologyDescCount (void);
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

inline CTopologyDesc *CTopologyDesc::GetTopologyDesc (int iIndex) { return (m_pDescList ? m_pDescList->GetTopologyDesc(iIndex) : NULL); }
inline int CTopologyDesc::GetTopologyDescCount (void) { return (m_pDescList ? m_pDescList->GetTopologyDescCount() : 0); }

struct STopologyCreateCtx
	{
	STopologyCreateCtx (void) :
			pMap(NULL),
			pFragmentTable(NULL),
			pNodesAdded(NULL),
			bInFragment(false),
			xOffset(0),
			yOffset(0),
			iRotation(0)
		{ }

	CSystemMap *pMap;								//	Map that we're currently processing
	TArray<CTopologyDescTable *> Tables;			//	List of tables to look up
	CTopologyDescTable *pFragmentTable;
	CTopologyNodeList *pNodesAdded;					//	Output of nodes added

	bool bInFragment;
	CString sFragmentPrefix;
	int xOffset;
	int yOffset;
	int iRotation;
	CString sFragmentExitID;
	CString sFragmentExitGate;
	CString sFragmentAttributes;

	CString sError;
	};

