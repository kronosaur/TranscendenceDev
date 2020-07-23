//	TSESystemCreate.h
//
//	Defines classes and interfaces for creating a star system.
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CStationEncounterTable
	{
	public:
		struct SEntry
			{
			CStationType *pType;
			int iChance;
			};

		struct SInitCtx
			{
			CString sCriteria;				//	Station criteria
			CString sLocationAttribs;		//	Attributes for current location
			CString sExtraLocationAttribs;	//	Attributes for current location
			CVector vPos;					//	Position where we're creating station

			bool bIncludeAll = false;
			};

		const SEntry &operator [] (int iIndex) const { return m_Table[iIndex]; }
		SEntry &operator [] (int iIndex) { return m_Table[iIndex]; }

		void DeleteAll (void) { m_Table.DeleteAll(); }
		const SEntry &GetAt (int iIndex) const { return m_Table[iIndex]; }
		int GetCount (void) const { return m_Table.GetCount(); }
		bool Init (CSystem &System, const CStationEncounterOverrideTable &EncounterTable, const SInitCtx &Options, CString *retsError = NULL, bool *retbAddToCache = NULL);
		void Insert (CStationType &Type, int iChance)
			{
			SEntry *pEntry = m_Table.Insert();
			pEntry->pType = &Type;
			pEntry->iChance = iChance;
			}

	private:
		TArray<SEntry> m_Table;
	};

class CStationTableCache
	{
	public:
		const CStationEncounterTable *AddTable (const CString &sDesc, CStationEncounterTable &Table);
		void DeleteAll (void);
		const CStationEncounterTable *FindTable (const CString &sDesc) const;
		int GetCacheHitRate (void) const;
		int GetCacheSize (void) const { return m_Cache.GetCount(); }

	private:
		TSortMap<CString, TUniquePtr<CStationEncounterTable>> m_Cache;
		mutable int m_iCacheHits = 0;
		mutable int m_iCacheMisses = 0;
	};

class CSystemCreateStats
	{
	public:
		struct SEncounterTable
			{
			int iLevel;
			CSystemType *pSystemType;
			CString sStationCriteria;
			TArray<CString> LabelAttribs;
			int iCount;
			TProbabilityTable<CStationType *> Table;

			bool bHasStation;
			};

		struct SFillLocationsTable
			{
			int iLevel;
			CString sNodeID;
			CString sSystemName;
			CSystemType *pSystemType;
			CString sSystemAttribs;
			CString sStationCriteria;
			TProbabilityMap<CStationType *> SystemProb;			//	Probability for each station, considering only system attribs
			TProbabilityMap<CStationType *> FillProb;			//	Probability including stationCriteria in <FillLocations>
			TProbabilityMap<CStationType *> LocationProb;		//	Probability including available locations

			int iCount;
			};

		CSystemCreateStats (void);
		~CSystemCreateStats (void);

		void AddLabel (const CString &sAttributes);
		void AddFillLocationsTable (CSystem *pSystem, const TProbabilityTable<int> &LocationTable, const CString &sStationCriteria);
		void AddPermuteAttrib (const CString &sAttrib) { m_PermuteAttribs.Insert(sAttrib); }
		void AddStationTable (CSystem *pSystem, const CString &sStationCriteria, const CString &sLocationAttribs, const CStationEncounterTable &Table);
		const SEncounterTable &GetEncounterTable (int iIndex) const { return m_EncounterTables[iIndex]; }
		int GetEncounterTableCount (void) const { return m_EncounterTables.GetCount(); }
		const SFillLocationsTable &GetFillLocationsTable (int iIndex) const { return m_FillLocationsTables[iIndex]; }
		int GetFillLocationsTableCount (void) const { return m_FillLocationsTables.GetCount(); }
		int GetLabelAttributesCount (void) { return m_LabelAttributeCounts.GetCount(); }
		void GetLabelAttributes (int iIndex, CString *retsAttribs, int *retiCount);
		int GetTotalLabelCount (void) { return m_iLabelCount; }
		void SetPermute (bool bValue = true) { m_bPermute = bValue; }

	private:
		struct SLabelAttributeEntry
			{
			CString sAttributes;
			int iCount;
			};

		void AddEntry (const CString &sAttributes);
		void AddEntryPermutations (const CString &sPrefix, const TArray<CString> &Attribs, int iPos);
		void AddLabelAttributes (const CString &sAttributes);
		void AddLabelExpansion (const CString &sAttributes, const CString &sPrefix = NULL_STR);
		bool FindEncounterTable (const CStationEncounterTable &Src, SEncounterTable **retpTable);

		//	Label stats

		bool m_bPermute;
		int m_iLabelCount;
		CSymbolTable m_LabelAttributeCounts;
		TArray<CString> m_PermuteAttribs;

		//	Encounter tables

		TArray<SEncounterTable> m_EncounterTables;
		TSortMap<CString, SFillLocationsTable> m_FillLocationsTables;
	};

class CSystemCreateEvents
	{
	public:
		void AddDeferredEvent (CSpaceObject *pObj, CExtension *pExtension, const CXMLElement *pEventCode);
		ALERROR FireDeferredEvent (const CString &sEvent, CString *retsError);

	private:
		struct SEventDesc
			{
			CSpaceObject *pObj;
			CExtension *pExtension;
			const CXMLElement *pEventCode;
			};

		TArray<SEventDesc> m_Events;
	};

struct SLocationCriteria
	{
	SLocationCriteria (void) { }
	explicit SLocationCriteria (const CAffinityCriteria &Criteria) :
			AttribCriteria(Criteria)
		{ }

	CAffinityCriteria AttribCriteria;		//	Attribute criteria
	Metric rMinDist = 0.0;					//	Minimum distance from source
	Metric rMaxDist = 0.0;					//	Maximum distance from source
	};

class CLocationSelectionTable
	{
	public:
		CLocationSelectionTable (CSystem &System) :
				m_System(System)
			{ }

		explicit operator bool () const { return !IsEmpty(); }

		void DeleteAll (void) { m_Table.DeleteAll(); }
		void DeleteBlocked (void);
		void DeleteInRange (const CVector &vCenter, Metric rRadius);
		void DeleteInRange (const CLocationDef &Loc, Metric rRadius) { DeleteInRange(Loc.GetOrbit().GetObjectPos(), rRadius); }
		void Fill (const CLocationDef &Loc, DWORD dwObjID);
		int GetCount (void) const { return m_Table.GetCount(); }
		const CLocationDef *GetRandom (void) const;
		void Insert (int iIndex, int iProbability) { m_Table.Insert(iIndex, iProbability); }
		bool IsEmpty (void) const { return m_Table.IsEmpty(); }

	private:
		int FindIndex (const CLocationDef &LocToFind) const;

		CSystem &m_System;
		TProbabilityTable<int> m_Table;
	};

struct SZAdjust
	{
	CString sInclination;					//	Override inclination attrib
	CString sRotation;						//	Override rotation attrib
	DiceRange ZOffset;						//	Override Z position (in scale units)
	Metric rScale = LIGHT_SECOND;			//	Scale units

	bool bIgnoreLocations = false;			//	Do not adjust locations
	};

struct SStationTypeTableStats
	{
	TSortMap<const CStationType *, int> Counts;
	};

struct SSystemCreateCtx
	{
	enum EOverlapCheck 
		{
		checkOverlapNone,					//	Don't worry about overlaps
		checkOverlapPlanets,				//	Don't overlap planets (> 1,000 km)
		checkOverlapAsteroids,				//	Don't overlap asteroids or planets
		};

	SSystemCreateCtx (CSystem &SystemArg);

	CSystem &GetSystem (void) { return System; }
	inline CUniverse &GetUniverse (void);

	//	Context

	CExtension *pExtension = NULL;			//	Extension from which the current desc came
	CTopologyNode *pTopologyNode = NULL;	//	Topology node
	CSystem &System;						//	System that we're creating

	TArray<CXMLElement *> LocalTables;		//	Stack of local tables
	TSortMap<CString, CString> NameParams;	//	Parameters passed in to CNameDesc
	CString sLabelAttribs;					//	Inherited label attributes
	CStationEncounterOverrideTable StationEncounterOverrides;

	//	Options

	bool bHasStationCreate = false;			//	TRUE if we've defined some create options
	CStationCreateOptions StationCreate;	//	Options for creating a station

	SZAdjust ZAdjust;						//	Adjust Z
	EOverlapCheck iOverlapCheck = checkOverlapNone;	//	If TRUE, we adjust locations to avoid overlapping an existing object
	bool bIs3DExtra = false;				//	Objects are optional 3D extras

	//	Stats

	CSystemCreateStats *pStats = NULL;		//	System creation stats (may be NULL)
	CSystemCreateEvents Events;				//	System deferred events
	TArray<CString> DebugStack;				//	Stack of directives

	//	Temps and working vars while creating

	CString sError;							//	Creation error
	CString sLocationAttribs;				//	Current location attribs
	TArray<CString> Variants;				//	List of variants set with <Variant> directive
	CSpaceObject *pStation = NULL;			//	Root station when creating satellites
	DWORD dwLastObjID = 0;					//	Object created in last call
											//	NOTE: This is an ID in case the object gets deleted.
	TSortMap<CString, IElementGenerator::STableStats> TableStats;
	TSortMap<CString, SStationTypeTableStats> RandomStationStats;

	CStationTableCache StationTables;		//	Cached station tables
	};

struct SObjCreateCtx
	{
	SObjCreateCtx (SSystemCreateCtx &SystemCtxArg) :
			SystemCtx(SystemCtxArg)
		{ }

	SSystemCreateCtx &SystemCtx;			//	System create context
	CVector vPos;							//	Create at this position. This should
											//		always be set properly, even if orbit
											//		or location is also provided.
	CVector vVel;							//	Initial velocity.
	int iRotation = -1;						//	-1 = default rotation
	Metric rParallax = 1.0;					//	Parallax

	CString sID;							//	Optional object ID
	const CLocationDef *pLoc = NULL;		//	Optional location (may be NULL)
	const COrbit *pOrbit = NULL;			//	Optional orbit (may be NULL)
	CXMLElement *pExtraData = NULL;			//	Extra data for object (may be NULL)
	CDesignType *pEventHandler = NULL;		//	Event handler for object
	CSovereign *pSovereign = NULL;			//	Optional sovereign (may be NULL)
	CShipClass *pWreckClass = NULL;			//	Optional for creating wrecks (may be NULL)
	CShip *pWreckShip = NULL;				//	Optional for creating wrecks (may be NULL)

	bool bCreateSatellites = false;			//	If TRUE, create satellites
	bool bIsSegment = false;				//  If TRUE, we're a satellite segment
	bool bIgnoreLimits = false;				//	If TRUE, create even if we exceed limits
	bool bIs3DExtra = false;				//	If TRUE, this is an optional effect
	};

