//	TSESystemMap.h
//
//	Classes and functions for system maps.
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class ITopologyProcessor
	{
	public:
		enum EPhase
			{
			phaseDefault =			-1,
			phaseError =			-2,

			phasePrimaryMap =		0,
			phaseSecondaryMap =		1,

			phaseFormation =		2,
			phaseLifeforms =		3,
			phasePrimaryColony =	4,
			phaseSecondaryColony =	5,
			phaseTertiaryColony =	6,

			phaseCount =			7,
			};

		struct SProcessCtx
			{
			SProcessCtx (CTopology &TopologyArg, CSystemMap *pMapArg) :
					Topology(TopologyArg),
					pMap(pMapArg)
				{ }

			CTopology &Topology;					//	Topology
			CSystemMap *pMap;						//	Map that we're processing

			bool bReduceNodeList = false;			//	Remove nodes when processed
			};

		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID, ITopologyProcessor **retpProc);
		static ALERROR CreateFromXMLAsGroup (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID, ITopologyProcessor **retpProc);

		virtual ~ITopologyProcessor (void) { }
		inline ALERROR BindDesign (SDesignLoadCtx &Ctx) { return OnBindDesign(Ctx); }
		inline CEffectCreator *FindEffectCreator (const CString &sUNID) { return OnFindEffectCreator(sUNID); }
		inline EPhase GetPhase (void) const { return m_iPhase; }
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID);
		inline void Paint (CG32bitImage &Dest, int xCenter, int yCenter, Metric rScale) const { OnPaint(Dest, xCenter, yCenter, rScale); }
		inline ALERROR Process (SProcessCtx &Ctx, CTopologyNodeList &NodeList, CString *retsError) { return OnProcess(Ctx, NodeList, retsError); }

		static EPhase ParsePhase (const CString &sValue);

	protected:
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) { return NOERROR; }
		virtual CEffectCreator *OnFindEffectCreator (const CString &sUNID) { return NULL; }
		virtual ALERROR OnInitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) { return NOERROR; }
		virtual void OnPaint (CG32bitImage &Dest, int xCenter, int yCenter, Metric rScale) const { }
		virtual ALERROR OnProcess (SProcessCtx &Ctx, CTopologyNodeList &NodeList, CString *retsError) { return NOERROR; }

		CTopologyNodeList *FilterNodes (CTopology &Topology, CTopologyNode::SCriteria &Criteria, CTopologyNodeList &Unfiltered, CTopologyNodeList &Filtered);
		ALERROR InitBaseItemXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		void RestoreMarks (CTopology &Topology, TArray<bool> &Saved);
		void SaveAndMarkNodes (CTopology &Topology, CTopologyNodeList &NodeList, TArray<bool> *retSaved);

		CTopologyNode::SCriteria m_Criteria;
		EPhase m_iPhase = phaseDefault;
	};

class CSystemMap : public CDesignType
	{
	public:
		struct SNodeExtra
			{
			CTopologyNode *pNode;
			int iStargateCount;
			};

		struct SSortEntry
			{
			SNodeExtra *pExtra;
			int iSort;
			};

		CSystemMap (void) : m_bAdded(false), m_bDebugShowAttributes(false) { }
		virtual ~CSystemMap (void);

		void AccumulateTopologyProcessors (TSortMap<int, TArray<ITopologyProcessor *>> &Result) const;
		bool AddAnnotation (CEffectCreator *pEffect, int x, int y, int iRotation, DWORD *retdwID = NULL) { return AddAnnotation(NULL_STR, pEffect, x, y, iRotation, retdwID); }
		bool AddAnnotation (const CString &sNodeID, CEffectCreator *pEffect, int x, int y, int iRotation, DWORD *retdwID = NULL);
		inline bool AddAreaHighlight (const CComplexArea &Area) { m_AreaHighlights.Insert(Area); return true; }
		ALERROR AddFixedTopology (CTopology &Topology, TSortMap<DWORD, CTopologyNodeList> &NodesAdded, CString *retsError);
		bool DebugShowAttributes (void) const { return m_bDebugShowAttributes; }
		CG32bitImage *CreateBackgroundImage (Metric *retrImageScale);
		void GetBackgroundImageSize (int *retcx, int *retcy);
		inline CSystemMap *GetDisplayMap (void) { return (m_pPrimaryMap != NULL ? m_pPrimaryMap : this); }
        inline Metric GetLightYearsPerPixel (void) const { return m_rLightYearsPerPixel; }
		inline const CString &GetName (void) const { return m_sName; }
		inline void GetScale (int *retiInitial, int *retiMin = NULL, int *retiMax = NULL) { if (retiInitial) *retiInitial = m_iInitialScale; if (retiMin) *retiMin = m_iMinScale; if (retiMax) *retiMax = m_iMaxScale; }
        inline CG32bitPixel GetStargateLineColor (void) const { return m_rgbStargateLines; }
		inline const CString &GetStartingNodeID (void) { return m_FixedTopology.GetFirstNodeID(); }
		inline bool IsPrimaryMap (void) const { return (m_pPrimaryMap == NULL); }
		inline bool IsStartingMap (void) const { return m_bStartingMap; }

		//	CDesignType overrides
		static CSystemMap *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designSystemMap) ? (CSystemMap *)pType : NULL); }
		virtual DesignTypes GetType (void) const override { return designSystemMap; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual CEffectCreator *OnFindEffectCreator (const CString &sUNID) override;
		virtual void OnReadFromStream (SUniverseLoadCtx &Ctx) override;
		virtual void OnReinit (void) override;
		virtual void OnWriteToStream (IWriteStream *pStream) override;

	private:
		struct SMapAnnotation
			{
			SMapAnnotation (void) :
					pPainter(NULL)
				{ }

			~SMapAnnotation (void)
				{
				if (pPainter)
					pPainter->Delete();
				}

			DWORD dwID;
			CString sNodeID;					//	Optionally associated with a nodeID

			IEffectPainter *pPainter;
			int xOffset;
			int yOffset;
			int iTick;
			int iRotation;

			DWORD fHideIfNodeUnknown:1;			//	If TRUE, do not paint unless node is known
			DWORD fSpare2:1;
			DWORD fSpare3:1;
			DWORD fSpare4:1;
			DWORD fSpare5:1;
			DWORD fSpare6:1;
			DWORD fSpare7:1;
			DWORD fSpare8:1;

			DWORD dwSpare:24;
			};

		ALERROR ExecuteCreator (STopologyCreateCtx &Ctx, CTopology &Topology, CXMLElement *pCreator);

		CString m_sName;						//	Name of the map (for the player)
		int m_iInitialScale;					//	Initial map display scale (100 = 100%)
		int m_iMaxScale;						//	Max zoom
		int m_iMinScale;						//	Min zoom
        Metric m_rLightYearsPerPixel;           //  Number of light years per pixel

		CSystemMapRef m_pPrimaryMap;			//	If not NULL, place nodes on given map
		TArray<CSystemMapRef> m_Uses;			//	List of maps that we rely on.

		//	Map image
		DWORD m_dwBackgroundImage;				//	Background image to use
		Metric m_rBackgroundImageScale;			//	Pixels per galactic unit
        CG32bitPixel m_rgbStargateLines;        //  Color of stargate line
		CEffectCreatorRef m_pBackgroundEffect;	//	Background annotations to paint on galactic map

		//	Topology generation
		CTopologyDescTable m_FixedTopology;
		TArray<CXMLElement *> m_Creators;
		TArray<ITopologyProcessor *> m_Processors;
		bool m_bStartingMap;					//	Do not load unless specified by player ship

		//	Annotations
		TArray<SMapAnnotation> m_Annotations;
		TArray<CComplexArea> m_AreaHighlights;

		//	Temporaries
		bool m_bAdded;							//	TRUE if map was added to topology

		//	Debug
		bool m_bDebugShowAttributes;
	};

int KeyCompare (const CSystemMap::SSortEntry &Key1, const CSystemMap::SSortEntry &Key2);

//	System Tables --------------------------------------------------------------

class CSystemTable : public CDesignType
	{
	public:
		CSystemTable (void) : m_pTable(NULL) { }
		virtual ~CSystemTable (void);

		CXMLElement *FindElement (const CString &sElement) const { return (m_pTable ? m_pTable->GetContentElementByTag(sElement) : NULL); }

		//	CDesignType overrides
		static CSystemTable *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designSystemTable) ? (CSystemTable *)pType : NULL); }
		virtual DesignTypes GetType (void) const override { return designSystemTable; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override { m_pTable = pDesc; return NOERROR; }

	private:
		CXMLElement *m_pTable;
	};

