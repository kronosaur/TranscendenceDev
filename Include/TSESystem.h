//	TSESystem.h
//
//	Defines classes and interfaces for a star system.
//	Copyright (c) 2016 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

//	Include helper classes

#include "TSEEvents.h"
#include "TSEPhysics.h"
#include "TSEObjectJoints.h"

const int MIN_PLANET_SIZE = 1000;			//	Size at which a world is considered planetary size

//	CNavigationPath

class CNavigationPath : public TSEListNode<CNavigationPath>
	{
	public:
		CNavigationPath (void);
		~CNavigationPath (void);

		static void Create (CSystem *pSystem, CSovereign *pSovereign, CSpaceObject *pStart, CSpaceObject *pEnd, CNavigationPath **retpPath);
		static void Create (CSystem *pSystem, CSovereign *pSovereign, const CVector &vStart, const CVector &vEnd, CNavigationPath **retpPath);

		Metric ComputePathLength (CSystem *pSystem) const;
		CVector ComputePointOnPath (CSystem *pSystem, Metric rDist) const;
		static CString DebugDescribe (CSpaceObject *pObj, CNavigationPath *pNavPath);
		void DebugPaintInfo (CG32bitImage &Dest, int x, int y, ViewportTransform &Xform);
		void DebugPaintInfo (CG32bitImage &Dest, int x, int y, const CMapViewportCtx &Ctx);
		inline DWORD GetID (void) const { return m_dwID; }
		inline int GetNavPointCount (void) const { return m_iWaypointCount; }
		CVector GetNavPoint (int iIndex) const;
		bool Matches (CSovereign *pSovereign, CSpaceObject *pStart, CSpaceObject *pEnd);
		void OnReadFromStream (SLoadCtx &Ctx);
		void OnWriteToStream (CSystem *pSystem, IWriteStream *pStream) const;

	private:
		static int ComputePath (CSystem *pSystem, CSovereign *pSovereign, const CVector &vFrom, const CVector &vTo, CVector **retpPoints);
//		static int ComputePath (CSystem *pSystem, CSovereign *pSovereign, const CVector &vFrom, const CVector &vTo, int iDepth, CVector **retpPoints);
		static bool PathIsClear (CSystem *pSystem, CSovereign *pSovereign, const CVector &vFrom, const CVector &vTo, CSpaceObject **retpEnemy, CVector *retvAway);

		DWORD m_dwID;							//	ID of path
		CSovereign *m_pSovereign;				//	Sovereign that will use (NULL if all)
		DWORD m_iStartIndex;					//	Index of start object
		DWORD m_iEndIndex;						//	Index of end object

		int m_iSuccesses;						//	Count of ships that successfully reached end
		int m_iFailures;						//	Count of ships that were destroyed

		CVector m_vStart;						//	Start position
		int m_iWaypointCount;					//	Number of waypoints (excludes start)
		CVector *m_Waypoints;					//	Array of waypoints
	};

typedef TSEListNode<CNavigationPath> CNavigationPathNode;

//	CSystemEventHandler

class CSystemEventHandler : public TSEListNode<CSystemEventHandler>
	{
	public:
		CSystemEventHandler (void);

		static void Create (CSpaceObject *pObj, Metric rMaxRange, CSystemEventHandler **retpHandler);

		inline CSpaceObject *GetObj (void) const { return m_pHandler; }
		bool InRange (const CVector &vPos) const;
		void OnObjDestroyed (CSpaceObject *pObjDestroyed, bool *retbRemoveNode);
		void OnReadFromStream (SLoadCtx &Ctx);
		void OnWriteToStream (CSystem *pSystem, IWriteStream *pStream) const;
		inline void SetRange (Metric rMaxRange) { m_rMaxRange2 = rMaxRange * rMaxRange; }

	private:
		CSpaceObject *m_pHandler;				//	Event handler
		Metric m_rMaxRange2;					//	Beyond this range from handler, no events seen
	};

typedef TSEListNode<CSystemEventHandler> CSystemEventHandlerNode;

//	System

class COrbit
	{
	public:
		COrbit (void);
		COrbit (const CVector &vCenter, Metric rRadius, Metric rPos = 0.0);
		COrbit (const CVector &vCenter, Metric rSemiMajorAxis, Metric rEccentricity, Metric rRotation, Metric rPos = 0.0);

		bool operator== (const COrbit &Src) const;

		inline const Metric &GetEccentricity (void) const { return m_rEccentricity; }
		inline const CVector &GetFocus (void) const { return m_vFocus; }
		inline const Metric &GetObjectAngle (void) const { return m_rPos; }
		inline CVector GetObjectPos (void) const { return GetPoint(m_rPos); }
		CVector GetPoint (Metric rAngle) const;
		CVector GetPointAndRadius (Metric rAngle, Metric *retrRadius) const;
		CVector GetPointCircular (Metric rAngle) const;
		inline const Metric &GetRotation (void) const { return m_rRotation; }
		inline const Metric &GetSemiMajorAxis (void) const { return m_rSemiMajorAxis; }
		inline bool IsNull (void) const { return (m_rSemiMajorAxis == 0.0); }
		void Paint (CMapViewportCtx &Ctx, CG32bitImage &Dest, CG32bitPixel rgbColor);
        void PaintHD (CMapViewportCtx &Ctx, CG32bitImage &Dest, CG32bitPixel rgbColor, CGDraw::EBlendModes iMode = CGDraw::blendNormal);

		static Metric RandomAngle (void) { return mathDegreesToRadians(mathRandom(0,3599) / 10.0); }

	private:
        CG32bitPixel GetColorAtRadiusHD (CG32bitPixel rgbColor, Metric rRadius) const;

		CVector m_vFocus;				//	Focus of orbit
		Metric m_rEccentricity;			//	Ellipse eccentricity
		Metric m_rSemiMajorAxis;		//	Semi-major axis
		Metric m_rRotation;				//	Angle of rotation (radians)

		Metric m_rPos;					//	Obj position in orbit (radians)
	};

class CStationTableCache
	{
	public:
		struct SEntry
			{
			CStationType *pType;
			int iChance;
			};

		CStationTableCache (void) : m_iCacheHits(0), m_iCacheMisses(0) { }
		~CStationTableCache (void) { DeleteAll(); }

		void AddTable (const CString &sDesc, TArray<SEntry> *pTable) { m_Cache.Insert(sDesc, pTable); }
		void DeleteAll (void);
		bool FindTable (const CString &sDesc, TArray<SEntry> **retpTable);
		int GetCacheHitRate (void) const;
		inline int GetCacheSize (void) const { return m_Cache.GetCount(); }

	private:
		TSortMap<CString, TArray<SEntry> *> m_Cache;
		int m_iCacheHits;
		int m_iCacheMisses;
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
			CString sSystemName;
			CSystemType *pSystemType;
			CString sStationCriteria;
			TProbabilityTable<CStationType *> Table;
			};

		CSystemCreateStats (void);
		~CSystemCreateStats (void);

		void AddLabel (const CString &sAttributes);
		void AddFillLocationsTable (CSystem *pSystem, const TProbabilityTable<int> &LocationTable, const CString &sStationCriteria);
		inline void AddPermuteAttrib (const CString &sAttrib) { m_PermuteAttribs.Insert(sAttrib); }
		void AddStationTable (CSystem *pSystem, const CString &sStationCriteria, const CString &sLocationAttribs, TArray<CStationTableCache::SEntry> &Table);
		inline const SEncounterTable &GetEncounterTable (int iIndex) const { return m_EncounterTables[iIndex]; }
		inline int GetEncounterTableCount (void) const { return m_EncounterTables.GetCount(); }
		inline const SFillLocationsTable &GetFillLocationsTable (int iIndex) const { return m_FillLocationsTables[iIndex]; }
		inline int GetFillLocationsTableCount (void) const { return m_FillLocationsTables.GetCount(); }
		inline int GetLabelAttributesCount (void) { return m_LabelAttributeCounts.GetCount(); }
		void GetLabelAttributes (int iIndex, CString *retsAttribs, int *retiCount);
		inline int GetTotalLabelCount (void) { return m_iLabelCount; }
		inline void SetPermute (bool bValue = true) { m_bPermute = bValue; }

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
		bool FindEncounterTable (TArray<CStationTableCache::SEntry> &Src, SEncounterTable **retpTable) const;

		//	Label stats

		bool m_bPermute;
		int m_iLabelCount;
		CSymbolTable m_LabelAttributeCounts;
		TArray<CString> m_PermuteAttribs;

		//	Encounter tables

		TArray<SEncounterTable> m_EncounterTables;
		TArray<SFillLocationsTable> m_FillLocationsTables;
	};

class CSystemCreateEvents
	{
	public:
		void AddDeferredEvent (CSpaceObject *pObj, CExtension *pExtension, CXMLElement *pEventCode);
		ALERROR FireDeferredEvent (const CString &sEvent, CString *retsError);

	private:
		struct SEventDesc
			{
			CSpaceObject *pObj;
			CExtension *pExtension;
			CXMLElement *pEventCode;
			};

		TArray<SEventDesc> m_Events;
	};

struct SLocationCriteria
	{
	SLocationCriteria (void) :
			rMinDist(0.0),
			rMaxDist(0.0)
		{ }

	CAttributeCriteria AttribCriteria;		//	Attribute criteria
	Metric rMinDist;						//	Minimum distance from source
	Metric rMaxDist;						//	Maximum distance from source
	};

struct SSystemCreateCtx
	{
	enum EOverlapCheck 
		{
		checkOverlapNone,					//	Don't worry about overlaps
		checkOverlapPlanets,				//	Don't overlap planets (> 1,000 km)
		checkOverlapAsteroids,				//	Don't overlap asteroids or planets
		};

	SSystemCreateCtx (void) : 
			pExtension(NULL),
			iOverlapCheck(checkOverlapNone),
			pStats(NULL),
			pStation(NULL), 
			dwLastObjID(0)
		{ }

	//	Context

	CExtension *pExtension;					//	Extension from which the current desc came
	CTopologyNode *pTopologyNode;			//	Topology node
	CSystem *pSystem;						//	System that we're creating

	TArray<CXMLElement *> LocalTables;		//	Stack of local tables

	//	Options

	EOverlapCheck iOverlapCheck;			//	If TRUE, we adjust locations to avoid overlapping an existing object

	//	Stats

	CSystemCreateStats *pStats;				//	System creation stats (may be NULL)
	CSystemCreateEvents Events;				//	System deferred events
	TArray<CString> DebugStack;				//	Stack of directives

	//	Temps and working vars while creating

	CString sError;							//	Creation error
	CString sLocationAttribs;				//	Current location attribs
	TArray<CString> Variants;				//	List of variants set with <Variant> directive
	CSpaceObject *pStation;					//	Root station when creating satellites
	DWORD dwLastObjID;						//	Object created in last call
											//	NOTE: This is an ID in case the object gets deleted.

	CStationTableCache StationTables;		//	Cached station tables
	};

struct SSystemUpdateCtx
	{
	SSystemUpdateCtx (void) : rSecondsPerTick(g_SecondsPerUpdate),
			bForceEventFiring(false),
			bForcePainted(false),
			bTrackPlayerObjs(false)
		{ }

	Metric rSecondsPerTick;
	bool bForceEventFiring;					//	If TRUE, fire events even if no player ship
	bool bForcePainted;						//	If TRUE, mark objects as painted 
	bool bTrackPlayerObjs;					//	If TRUE, make a list of player objects
	};

//	CMoveCtx is currently unused; it was part of an experiment to see
//	if I could improve the moving algorithms, but it proved too time-consuming

class CMoveCtx
	{
	public:
		CMoveCtx (int iAlloc);
		~CMoveCtx (void);

		void AddObj (CSpaceObject *pObj);
		void BounceObj (int iIndex);
		inline int GetCount (void) { return m_iCount; }
		inline CSpaceObject *GetObj (int iIndex) { return m_pArray[iIndex].pObj; }

	private:
		struct SEntry
			{
			CSpaceObject *pObj;

			CVector vOldPos;				//	Starting position of obj
			CVector vNewPos;				//	Ending position of obj

			CVector vBoundsUR;				//	Bounds for object across motion
			CVector vBoundsLL;

			bool bMobile;					//	TRUE if obj is capable of moving
			bool bHasMoved;					//	TRUE if obj moved this tick
			bool bBarrier;					//	TRUE if obj is a barrier
			};

		SEntry *m_pArray;
		int m_iCount;
		int m_iAlloc;
	};

class CLocationDef
	{
	public:
		CLocationDef (void);

		inline const CString &GetAttributes (void) const { return m_sAttributes; }
		inline DWORD GetObjID (void) const { return m_dwObjID; }
		inline const COrbit &GetOrbit (void) const { return m_OrbitDesc; }
		inline bool HasAttribute (const CString &sAttrib) const { return ::HasModifier(m_sAttributes, sAttrib); }
		inline bool IsBlocked (void) const { return m_bBlocked; }
		inline bool IsEmpty (void) const { return (m_dwObjID == 0 && !m_bBlocked); }
		inline void SetAttributes (const CString &sAttribs) { m_sAttributes = sAttribs; }
		inline void SetBlocked (bool bBlocked = true) { m_bBlocked = bBlocked; }
		inline void SetID (const CString &sID) { m_sID = sID; }
		inline void SetObjID (DWORD dwObjID) { m_dwObjID = dwObjID; }
		inline void SetOrbit (const COrbit &Orbit) { m_OrbitDesc = Orbit; }
		void ReadFromStream (SLoadCtx &Ctx);
		void WriteToStream (IWriteStream *pStream);

	private:
		CString m_sID;						//	May be blank
		COrbit m_OrbitDesc;
		CString m_sAttributes;

		DWORD m_dwObjID;					//	Object created at this location (or 0)

		bool m_bBlocked;					//	If TRUE, this location is too close to another
	};

class CLocationList
	{
	public:
		CLocationList (void);

		void FillCloseLocations (void);
		void FillOverlappingWith (CSpaceObject *pObj);
		inline int GetCount (void) { return m_List.GetCount(); }
		bool GetEmptyLocations (TArray<int> *retList);
		inline CLocationDef *GetLocation (int iIndex) { return &m_List[iIndex]; }
		CLocationDef *Insert (const CString &sID = NULL_STR);
		void ReadFromStream (SLoadCtx &Ctx);
		inline void SetBlocked (int iIndex) { m_List[iIndex].SetBlocked(); }
		inline void SetObjID (int iIndex, DWORD dwObjID) { m_List[iIndex].SetObjID(dwObjID); }
		void WriteToStream (IWriteStream *pStream);

	private:
		TArray<CLocationDef> m_List;

		bool m_bMinDistCheck;				//	If TRUE, then we've checked all locations for min distance
	};

class CTerritoryDef
	{
	public:
		CTerritoryDef (void);

		void AddAttributes (const CString &sAttribs);
		void AddRegion (const COrbit &Orbit, Metric rMinRadius, Metric rMaxRadius);
		static ALERROR CreateFromXML (CXMLElement *pDesc, const COrbit &OrbitDesc, CTerritoryDef **retpTerritory);
		inline const CString &GetAttributes (void) const { return m_sAttributes; }
		inline const CString &GetCriteria (void) { return m_sCriteria; }
		bool HasAttribute (const CString &sAttrib);
		bool HasAttribute (const CVector &vPos, const CString &sAttrib);
		inline bool IsMarked (void) { return m_bMarked; }
		bool PointInTerritory (const CVector &vPos);
		void ReadFromStream (SLoadCtx &Ctx);
		inline void SetCriteria (const CString &sCriteria) { m_sCriteria = sCriteria; }
		inline void SetMarked (bool bMarked = true) { m_bMarked = bMarked; }
		void WriteToStream (IWriteStream *pStream);

	private:
		struct SRegion
			{
			COrbit OrbitDesc;
			Metric rMinRadius;
			Metric rMaxRadius;

			Metric rMinRadius2;				//	Computed
			Metric rMaxRadius2;				//	Computed
			};

		bool MatchesCriteria (TArray<int> &Exclude, const CVector &vPos, const CString &sCriteria);

		CString m_sID;
		TArray<SRegion> m_Regions;
		CString m_sCriteria;
		CString m_sAttributes;

		bool m_bMarked;						//	Temporary mark
	};

class CTerritoryList
	{
	public:
		~CTerritoryList (void);

		CString GetAttribsAtPos (const CVector &vPos) const;
		inline int GetCount (void) { return m_List.GetCount(); }
		bool HasAttribute (const CVector &vPos, const CString &sAttrib) const;
		inline void Insert (CTerritoryDef *pTerritory) { m_List.Insert(pTerritory); }
		void ReadFromStream (SLoadCtx &Ctx);
		void WriteToStream (IWriteStream *pStream);

	private:
		bool MatchesCriteria (CTerritoryDef *pTerritory, const CVector &vPos) const;

		TArray<CTerritoryDef *> m_List;
	};

class CEnvironmentGrid
	{
	public:
		enum EEdges
			{
			edgeRight =						0x01,
			edgeTop =						0x02,
			edgeLeft =						0x04,
			edgeBottom =					0x08,
			};

		struct STileDesc
			{
			int x;
			int y;
			CSpaceEnvironmentType *pEnv;
			DWORD dwEdgeMask;
			};

		struct SCreateCtx
			{
			SCreateCtx (void) :
					pSystem(NULL),
					pEnv(NULL),
					pOrbitDesc(NULL),
					rWidth(0.0),
					rHeight(0.0),
					iWidthVariation(0),
					iSpan(0),
					iErode(0)
				{ }

			CSystem *pSystem;

			CSpaceEnvironmentType *pEnv;	//	Environment to create
			const COrbit *pOrbitDesc;		//	Orbit (may be NULL)

			Metric rWidth;
			Metric rHeight;
			int iWidthVariation;			//	0-100
			int iSpan;
			int iErode;						//	0-100
			};

		CEnvironmentGrid (CSystemType::ETileSize iTileSize);

		void CreateArcNebula (SCreateCtx &Ctx, TArray<STileDesc> *retTiles);
		void CreateCircularNebula (SCreateCtx &Ctx, TArray<STileDesc> *retTiles);
		void CreateSquareNebula (SCreateCtx &Ctx, TArray<STileDesc> *retTiles);
		void GetNextTileType (STileMapEnumerator &i, int *retx, int *rety, CSpaceEnvironmentType **retpEnv = NULL, DWORD *retdwEdgeMask = NULL) const;
		inline int GetTileSize (void) const { return m_iTileSize; }
		CSpaceEnvironmentType *GetTileType (int xTile, int yTile, DWORD *retdwEdgeMask = NULL);
		inline bool HasMoreTiles (STileMapEnumerator &i) const { return m_Map.HasMore(i); }
		void MarkImages (void);
		void Paint (SViewportPaintCtx &Ctx, CG32bitImage &Dest);
		void PaintMap (CMapViewportCtx &Ctx, CG32bitImage &Dest);
		void ReadFromStream (SLoadCtx &Ctx);
		void SetTileType (int xTile, int yTile, CSpaceEnvironmentType *pEnv);
		CVector TileToVector (int x, int y) const;
		void VectorToTile (const CVector &vPos, int *retx, int *rety) const;
		void WriteToStream (IWriteStream *pStream);

	private:
		enum EConstants
			{
			defaultScale =					2,		//	Two levels (levels = scale + 1)
			defaultSize =					12,		//	Each level is 12x12 tiles
			defaultTileSize =				128,	//	Tile size in pixels

			scaleCompatibile =				1,
			sizeCompatible =				16,
			tileSizeCompatible =			512,	//	Tile size in pixels (pre version 88, when we stored it)
			};

		struct SConfig
			{
			int iScale;						//	Number of levels (levels = scale + 1)
			int iLevelSize;					//	Size of each level (no. of tiles/sub-levels)
			int iTileSize;					//	Tile size in pixels
			};

		DWORD AddEdgeFlag (DWORD dwTile, DWORD dwEdgeFlag) const;
		void ConvertSpaceEnvironmentToPointers (CTileMap &UNIDs);
		bool ErodeCheck (Metric rErode, Metric rRadius, Metric rInnerRadius, Metric rMidRadius, Metric rOuterRadius);
		CSpaceEnvironmentType *GetSpaceEnvironmentFromTileDWORD (DWORD dwTile) const;
		inline bool InBounds (int xTile, int yTile) const { return (xTile >= 0 && xTile < m_iTileCount && yTile >= 0 && yTile <= m_iTileCount); }
		DWORD MakeTileDWORD (CSpaceEnvironmentType *pEnv, DWORD dwEdgeMask);

		CTileMap m_Map;
		TSortMap<CSpaceEnvironmentType *, bool> m_EnvList;
		int m_iTileSize;					//	Size of tile (in pixels)
		int m_iTileCount;					//	Size of grid in tiles

		static SConfig CONFIG_DATA[CSystemType::sizeCount];
	};

class CMapGridPainter
	{
	public:
		void AddRegion (const CVector &vCenter, Metric rWidth, Metric rHeight);
		inline bool IsEmpty (void) const { return (m_HorzLines.GetCount() == 0); }
		void Paint (CG32bitImage &Dest, CMapViewportCtx &PaintCtx);

	private:
		struct SLine
			{
			int xyKey;						//	x of vertical line or y of horizontal line
			int xyFrom;						//	From <= To
			int xyTo;

			CVector vFrom;
			CVector vTo;
			};

		void AddLines (const TArray<SLine> &NewLines, TArray<SLine> *retLines);
		bool FindKey (const TArray<SLine> &Lines, int xyKey, int *retiIndex);
		void Paint (CG32bitImage &Dest, CMapViewportCtx &PaintCtx, const TArray<SLine> &Lines);
		void RecalcGrid (void);

		TArray<SLine> m_HorzLines;
		TArray<SLine> m_VertLines;
		bool m_bRecalcNeeded;
	};

class CSystemSpacePainter
	{
	public:
		CSystemSpacePainter (void);

		void CleanUp (void);
		void PaintViewport (CG32bitImage &Dest, CSystemType *pType, SViewportPaintCtx &Ctx);
		void PaintViewportMap (CG32bitImage &Dest, const RECT &rcView, CSystemType *pType, CMapViewportCtx &Ctx);

	private:
		struct SStar
			{
			int x;
			int y;
			CG32bitPixel rgbColor;
			CG32bitPixel rgbSpikeColor;
			WORD wDistance;
			bool bBrightStar;
			};

		void CreateStarfield (int cxField, int cyField);
		void GenerateSquareDist (int iTotalCount, int iMinValue, int iMaxValue, int *Dist);
		void InitSpaceBackground (DWORD dwBackgroundUNID, CG32bitPixel rgbSpaceColor);
		void PaintSpaceBackground (CG32bitImage &Dest, int xCenter, int yCenter, SViewportPaintCtx &Ctx);
		void PaintStarfield (CG32bitImage &Dest, const RECT &rcView, int xCenter, int yCenter, CG32bitPixel rgbSpaceColor);

		bool m_bInitialized;

		TArray<SStar> m_Starfield;
		int m_cxStarfield;
		int m_cyStarfield;

		CG32bitImage *m_pBackground;
		DWORD m_dwCurBackgroundUNID;
	};

struct SObjCreateCtx
	{
	SObjCreateCtx (void) :
			iRotation(-1),
			rParallax(1.0),
			pLoc(NULL),
			pOrbit(NULL),
			pExtraData(NULL),
			pEventHandler(NULL),
			pSovereign(NULL),
			bCreateSatellites(false),
            bIsSegment(false),
			bIgnoreLimits(false)
		{ }

	CVector vPos;							//	Create at this position. This should
											//		always be set properly, even if orbit
											//		or location is also provided.
	CVector vVel;							//	Initial velocity.
	int iRotation;							//	-1 = default rotation
	Metric rParallax;						//	Parallax

	const CLocationDef *pLoc;				//	Optional location (may be NULL)
	const COrbit *pOrbit;					//	Optional orbit (may be NULL)
	CXMLElement *pExtraData;				//	Extra data for object (may be NULL)
	CDesignType *pEventHandler;				//	Event handler for object
	CSovereign *pSovereign;					//	Optional sovereign (may be NULL)

	bool bCreateSatellites;					//	If TRUE, create satellites
    bool bIsSegment;                        //  If TRUE, we're a satellite segment
	bool bIgnoreLimits;						//	If TRUE, create even if we exceed limits
	};

class CSystem
	{
	public:
		//	Other defines

		enum LayerEnum
			{
			layerFirst = 0,

			layerBackground = 0,
			layerSpace = 1,
			layerStations = 2,
			layerShips = 3,
			layerEffects = 4,
			layerOverhang = 5,

			layerCount = 6
			};

		enum Flags
			{
			//	CreateWeaponFire flags
			CWF_WEAPON_FIRE =				0x00000001,	//	Creating a shot from a weapon
			CWF_FRAGMENT =					0x00000002,	//	Creating a fragment
			CWF_EXPLOSION =					0x00000004,	//	Creating an explosion (or fragment of an explosion)
			CWF_EJECTA =					0x00000008,	//	Creating ejecta (or fragments of ejecta)
			CWF_REPEAT =					0x00000010,	//	Mixed with CWF_WEAPON_FIRE to indicate this is a repeat

			//	PaintViewport flags
			VWP_ENHANCED_DISPLAY =			0x00000001,	//	Show enhanced display markers
			VWP_NO_STAR_FIELD =				0x00000002,	//	Do not paint star field background
			};

		//	System methods

		static ALERROR CreateEmpty (CUniverse *pUniv, CTopologyNode *pTopology, CSystem **retpSystem);
		static ALERROR CreateFromStream (CUniverse *pUniv, 
										 IReadStream *pStream, 
										 CSystem **retpSystem,
										 CString *retsError,
										 DWORD dwObjID = OBJID_NULL,
										 CSpaceObject **retpObj = NULL,
										 CSpaceObject *pPlayerShip = NULL);
		static ALERROR CreateFromXML (CUniverse *pUniv, 
									  CSystemType *pType, 
									  CTopologyNode *pTopology, 
									  CSystem **retpSystem,
									  CString *retsError = NULL,
									  CSystemCreateStats *pStats = NULL);
		virtual ~CSystem (void);

		ALERROR CreateFlotsam (const CItem &Item, const CVector &vPos, const CVector &vVel, CSovereign *pSovereign, CStation **retpFlotsam);
		ALERROR CreateLookup (SSystemCreateCtx *pCtx, const CString &sTable, const COrbit &OrbitDesc, CXMLElement *pSubTables);
		ALERROR CreateMarker (CXMLElement *pDesc, const COrbit &oOrbit, CMarker **retpObj);
		ALERROR CreateParticles (CXMLElement *pDesc, const COrbit &oOrbit, CParticleEffect **retpObj);
		ALERROR CreateRandomEncounter (IShipGenerator *pTable, 
									   CSpaceObject *pBase,
									   CSovereign *pBaseSovereign,
									   CSpaceObject *pTarget,
									   CSpaceObject *pGate = NULL);
		ALERROR CreateShip (DWORD dwClassID,
							IShipController *pController,
							CDesignType *pOverride,
							CSovereign *pSovereign,
							const CVector &vPos,
							const CVector &vVel,
							int iRotation,
							CSpaceObject *pExitGate,
							SShipGeneratorCtx *pCtx,
							CShip **retpShip,
							CSpaceObjectList *retpShipList = NULL);
		ALERROR CreateShipwreck (CShipClass *pClass,
								 const CVector &vPos,
								 const CVector &vVel,
								 CSovereign *pSovereign,
								 CStation **retpWreck);
		ALERROR CreateStargate (CStationType *pType,
								CVector &vPos,
								const CString &sStargateID,
								const CString &sDestNodeID,
								const CString &sDestStargateID,
								CSpaceObject **retpStation = NULL);
		ALERROR CreateStation (CStationType *pType,
							   CDesignType *pEventHandler,
							   CVector &vPos,
							   CSpaceObject **retpStation = NULL);
		ALERROR CreateStation (SSystemCreateCtx *pCtx, 
							   CStationType *pType, 
							   SObjCreateCtx &CreateCtx,
							   CSpaceObject **retpStation = NULL);
		ALERROR CreateWeaponFire (CWeaponFireDesc *pDesc,
								  CItemEnhancementStack *pEnhancements,
								  const CDamageSource &Source,
								  const CVector &vPos,
								  const CVector &vVel,
								  int iDirection,
								  int iRepeatingCount,
								  CSpaceObject *pTarget,
								  DWORD dwFlags,
								  CSpaceObject **retpShot);
		ALERROR CreateWeaponFragments (CWeaponFireDesc *pDesc,
									   CItemEnhancementStack *pEnhancements,
									   const CDamageSource &Source,
									   CSpaceObject *pTarget,
									   const CVector &vPos,
									   const CVector &vVel,
									   CSpaceObject *pMissileSource,
                                       int iFraction = 100);

		bool AddJoint (CObjectJoint::ETypes iType, CSpaceObject *pFrom, CSpaceObject *pTo, ICCItem *pOptions, DWORD *retdwID = NULL);
		ALERROR AddTimedEvent (CSystemEvent *pEvent);
		inline void AddToDeleteList (CSpaceObject *pObj) { m_DeletedObjects.FastAdd(pObj); }
		ALERROR AddToSystem (CSpaceObject *pObj, int *retiIndex);
		bool AscendObject (CSpaceObject *pObj, CString *retsError = NULL);
		int CalculateLightIntensity (const CVector &vPos, CSpaceObject **retpStar = NULL, const CG8bitSparseImage **retpVolumetricMask = NULL);
		inline int CalcMatchStrength (const CAttributeCriteria &Criteria) { return (m_pTopology ? m_pTopology->CalcMatchStrength(Criteria) : (Criteria.MatchesAll() ? 1000 : 0)); }
		CG32bitPixel CalculateSpaceColor (CSpaceObject *pPOV, CSpaceObject **retpStar = NULL, const CG8bitSparseImage **retpVolumetricMask = NULL);
		void CancelTimedEvent (CSpaceObject *pSource, const CString &sEvent, bool bInDoEvent = false);
		void CancelTimedEvent (CDesignType *pSource, const CString &sEvent, bool bInDoEvent = false);
		bool DescendObject (DWORD dwObjID, const CVector &vPos, CSpaceObject **retpObj = NULL, CString *retsError = NULL);
		inline bool EnemiesInLRS (void) const { return m_fEnemiesInLRS; }
		inline void EnumObjectsInBoxStart (SSpaceObjectGridEnumerator &i, const CVector &vUR, const CVector &vLL, DWORD dwFlags = 0) { m_ObjGrid.EnumStart(i, vUR, vLL, dwFlags); }
		inline void EnumObjectsInBoxStart (SSpaceObjectGridEnumerator &i, const CVector &vPos, Metric rRange, DWORD dwFlags = 0)
			{
			CVector vRange = CVector(rRange, rRange);
			CVector vUR = vPos + vRange;
			CVector vLL = vPos - vRange;
			m_ObjGrid.EnumStart(i, vUR, vLL, dwFlags);
			}
		inline bool EnumObjectsInBoxHasMore (SSpaceObjectGridEnumerator &i) { return i.bMore; }
		inline CSpaceObject *EnumObjectsInBoxGetNext (SSpaceObjectGridEnumerator &i) { return m_ObjGrid.EnumGetNext(i); }
		inline CSpaceObject *EnumObjectsInBoxGetNextFast (SSpaceObjectGridEnumerator &i) { return m_ObjGrid.EnumGetNextFast(i); }
		inline CSpaceObject *EnumObjectsInBoxPointGetNext (SSpaceObjectGridEnumerator &i) { return m_ObjGrid.EnumGetNextInBoxPoint(i); }
		CSpaceObject *FindObject (DWORD dwID);
        CSpaceObject *FindObjectWithOrbit (const COrbit &Orbit) const;
		bool FindObjectName (CSpaceObject *pObj, CString *retsName = NULL);
		void FireOnSystemExplosion (CSpaceObject *pExplosion, CWeaponFireDesc *pDesc, const CDamageSource &Source);
		void FireOnSystemObjAttacked (SDamageCtx &Ctx);
		void FireOnSystemObjDestroyed (SDestroyCtx &Ctx);
		void FireOnSystemWeaponFire (CSpaceObject *pShot, CWeaponFireDesc *pDesc, const CDamageSource &Source, int iRepeatingCount);
		void FireSystemWeaponEvents (CSpaceObject *pShot, CWeaponFireDesc *pDesc, const CDamageSource &Source, int iRepeatingCount, DWORD dwFlags);
		void FlushEnemyObjectCache (void);
		CString GetAttribsAtPos (const CVector &vPos);
		inline CSpaceObject *GetDestroyedObject (int iIndex) { return m_DeletedObjects.GetObj(iIndex); }
		inline int GetDestroyedObjectCount (void) { return m_DeletedObjects.GetCount(); }
		inline CEnvironmentGrid *GetEnvironmentGrid (void) { InitSpaceEnvironment(); return m_pEnvironment; }
		inline DWORD GetID (void) { return m_dwID; }
		inline int GetLastUpdated (void) { return m_iLastUpdated; }
		int GetLevel (void);
		CSpaceObject *GetNamedObject (const CString &sName);
		inline const CString &GetName (void) { return m_sName; }
		CNavigationPath *GetNavPath (CSovereign *pSovereign, CSpaceObject *pStart, CSpaceObject *pEnd);
		CNavigationPath *GetNavPathByID (DWORD dwID);
		inline CSpaceObject *GetObject (int iIndex) const { return m_AllObjects[iIndex]; }
		inline int GetObjectCount (void) const { return m_AllObjects.GetCount(); }
		inline void GetObjectsInBox (const CVector &vPos, Metric rRange, CSpaceObjectList &Result)
			{
			CVector vRange = CVector(rRange, rRange);
			CVector vUR = vPos + vRange;
			CVector vLL = vPos - vRange;
			m_ObjGrid.GetObjectsInBox(vUR, vLL, Result);
			}
		inline void GetObjectsInBox (const CVector &vUR, const CVector &vLL, CSpaceObjectList &Result) { m_ObjGrid.GetObjectsInBox(vUR, vLL, Result); }
		CSpaceObject *GetPlayerShip (void) const;
		static DWORD GetSaveVersion (void);
		inline Metric GetSpaceScale (void) const { return m_rKlicksPerPixel; }
		inline int GetTick (void) { return m_iTick; }
		int GetTileSize (void) const;
		inline Metric GetTimeScale (void) const { return m_rTimeScale; }
		inline CTopologyNode *GetTopology (void) { return m_pTopology; }
		inline CSystemType *GetType (void) { return m_pType; }
		CSpaceEnvironmentType *GetSpaceEnvironment (int xTile, int yTile);
		CSpaceEnvironmentType *GetSpaceEnvironment (const CVector &vPos, int *retxTile = NULL, int *retyTile = NULL);
		CTopologyNode *GetStargateDestination (const CString &sStargate, CString *retsEntryPoint);
		inline CUniverse *GetUniverse (void) const { return g_pUniverse; }
		bool HasAttribute (const CVector &vPos, const CString &sAttrib);
		CSpaceObject *HitScan (CSpaceObject *pExclude, const CVector &vStart, const CVector &vEnd, bool bExcludeWorlds, CVector *retvHitPos = NULL);
		inline bool IsCreationInProgress (void) const { return (m_fInCreate ? true : false); }
		inline bool IsPlayerUnderAttack (void) const { return m_fPlayerUnderAttack; }
		bool IsStarAtPos (const CVector &vPos);
		bool IsStationInSystem (CStationType *pType);
		inline bool IsTimeStopped (void) { return (m_iTimeStopped != 0); }
		void MarkImages (void);
		void NameObject (const CString &sName, CSpaceObject *pObj);
		CVector OnJumpPosAdj (CSpaceObject *pObj, const CVector &vPos);
		void PaintViewport (CG32bitImage &Dest, const RECT &rcView, CSpaceObject *pCenter, DWORD dwFlags, SViewportAnnotations *pAnnotations = NULL);
		void PaintViewportGrid (CMapViewportCtx &Ctx, CG32bitImage &Dest, Metric rGridSize);
		void PaintViewportObject (CG32bitImage &Dest, const RECT &rcView, CSpaceObject *pCenter, CSpaceObject *pObj);
		void PaintViewportLRS (CG32bitImage &Dest, const RECT &rcView, CSpaceObject *pCenter, Metric rScale, DWORD dwFlags, bool *retbNewEnemies);
		void PaintViewportMap (CG32bitImage &Dest, const RECT &rcView, CSpaceObject *pCenter, Metric rMapScale);
		void PaintViewportMapObject (CG32bitImage &Dest, const RECT &rcView, CSpaceObject *pCenter, CSpaceObject *pObj);
		void PlaceInGate (CSpaceObject *pObj, CSpaceObject *pGate);
		void PlayerEntered (CSpaceObject *pPlayer);
		void RegisterEventHandler (CSpaceObject *pObj, Metric rRange);
		inline void RegisterForOnSystemCreated (CSpaceObject *pObj) { m_DeferredOnCreate.Insert(SDeferredOnCreateCtx(pObj)); }
		void RegisterForOnSystemCreated (CSpaceObject *pObj, CStationType *pEncounter, const COrbit &Orbit);
		void RemoveObject (SDestroyCtx &Ctx);
		void RemoveTimersForObj (CSpaceObject *pObj);
		void RestartTime (void);
		ALERROR SaveToStream (IWriteStream *pStream);
		inline void SetID (DWORD dwID) { m_dwID = dwID; }
		void SetLastUpdated (void);
		inline void SetPlayerUnderAttack (void) { m_fPlayerUnderAttack = true; }
		void SetPOVLRS (CSpaceObject *pCenter);
		void SetSpaceEnvironment (int xTile, int yTile, CSpaceEnvironmentType *pEnvironment);
		ALERROR StargateCreated (CSpaceObject *pGate, const CString &sStargateID, const CString &sDestNodeID, const CString &sDestEntryPoint);
		void StopTime (const CSpaceObjectList &Targets, int iDuration);
		void StopTimeForAll (int iDuration, CSpaceObject *pExcept);
		CVector TileToVector (int x, int y) const;
		void TransferObjEventsIn (CSpaceObject *pObj, CSystemEventList &ObjEvents);
		void TransferObjEventsOut (CSpaceObject *pObj, CSystemEventList &ObjEvents);
		void UnnameObject (CSpaceObject *pObj);
		void UnregisterEventHandler (CSpaceObject *pObj);
		void Update (SSystemUpdateCtx &SystemCtx, SViewportAnnotations *pAnnotations = NULL);
		void UpdateExtended (const CTimeSpan &ExtraTime);
		void VectorToTile (const CVector &vPos, int *retx, int *rety) const;
		void WriteObjRefToStream (CSpaceObject *pObj, IWriteStream *pStream, CSpaceObject *pReferrer = NULL);
		void WriteSovereignRefToStream (CSovereign *pSovereign, IWriteStream *pStream);

		//	Locations & Territories
		ALERROR AddTerritory (CTerritoryDef *pTerritory);
		void BlockOverlappingLocations (void);
		int CalcLocationWeight (CLocationDef *pLoc, const CAttributeCriteria &Criteria);
		ALERROR CreateLocation (const CString &sID, const COrbit &Orbit, const CString &sAttributes, CLocationDef **retpLocation = NULL);
		bool FindRandomLocation (const SLocationCriteria &Criteria, DWORD dwFlags, const COrbit &CenterOrbitDesc, CStationType *pStationToPlace, int *retiLocID);
		int GetEmptyLocationCount (void);
		bool GetEmptyLocations (const SLocationCriteria &Criteria, const COrbit &CenterOrbitDesc, CStationType *pStationToPlace, TProbabilityTable<int> *retTable);
		inline bool GetEmptyLocations (TArray<int> *retList) { return m_Locations.GetEmptyLocations(retList); }
		inline CLocationDef *GetLocation (int iLocID) { return m_Locations.GetLocation(iLocID); }
		inline int GetLocationCount (void) { return m_Locations.GetCount(); }
		bool IsExclusionZoneClear (const CVector &vPos, CStationType *pType = NULL);
		inline void SetLocationObjID (int iLocID, DWORD dwObjID) { m_Locations.SetObjID(iLocID, dwObjID); }

		static Metric CalcApparentSpeedAdj (Metric rSpeed);
		static void GetObjRefFromID (SLoadCtx &Ctx, DWORD dwID, CSpaceObject **retpObj);
		static void ReadObjRefFromStream (SLoadCtx &Ctx, CSpaceObject **retpObj);
		static void ReadObjRefFromStream (SLoadCtx &Ctx, void *pCtx, PRESOLVEOBJIDPROC pfnResolveProc);
		static void ReadSovereignRefFromStream (SLoadCtx &Ctx, CSovereign **retpSovereign);

	private:
		struct SStarDesc
			{
			SStarDesc (void) : 
					pStarObj(NULL)
				{ }

			CSpaceObject *pStarObj;
			CG8bitSparseImage VolumetricMask;
			};

		struct SDeferredOnCreateCtx
			{
			SDeferredOnCreateCtx (CSpaceObject *pObjArg = NULL) :
					pObj(pObjArg),
					pEncounter(NULL)
				{ }

			CSpaceObject *pObj;				//	Object created

			CStationType *pEncounter;		//	Only for ship encounters
			COrbit Orbit;					//	Only for ship encounters
			};

		CSystem (CUniverse *pUniv, CTopologyNode *pTopology);

		void CalcAutoTarget (SUpdateCtx &Ctx);
		void CalcObjGrid (SUpdateCtx &Ctx);
		void CalcViewportCtx (SViewportPaintCtx &Ctx, const RECT &rcView, CSpaceObject *pCenter, DWORD dwFlags);
		void CalcVolumetricMask (CSpaceObject *pStar, CG8bitSparseImage &VolumetricMask);
		void ComputeMapLabels (void);
		void ComputeRandomEncounters (void);
		void ComputeStars (void);
		ALERROR CreateStationInt (SSystemCreateCtx *pCtx,
								  CStationType *pType,
								  SObjCreateCtx &CreateCtx,
								  CSpaceObject **retpStation,
								  CString *retsError = NULL);
		void FlushDeletedObjects (void);
		inline int GetTimedEventCount (void) { return m_TimedEvents.GetCount(); }
		inline CSystemEvent *GetTimedEvent (int iIndex) { return m_TimedEvents.GetEvent(iIndex); }
		void InitSpaceEnvironment (void) const;
		void InitVolumetricMask (void);
		void PaintDestinationMarker (SViewportPaintCtx &Ctx, CG32bitImage &Dest, int x, int y, CSpaceObject *pObj);
		void PaintViewportAnnotations (CG32bitImage &Dest, SViewportAnnotations &Annotations, SViewportPaintCtx &Ctx);
		void RemoveVolumetricShadow (CSpaceObject *pObj);
		void SetPainted (void);
		void UpdateCollisionTesting (SUpdateCtx &Ctx);
		void UpdateGravity (SUpdateCtx &Ctx, CSpaceObject *pGravityObj);
		void UpdateRandomEncounters (void);

		//	Game instance data

		DWORD m_dwID;							//	System ID
		CSystemType *m_pType;					//	System type definition
		CString m_sName;						//	Name of system
		CTopologyNode *m_pTopology;				//	Topology descriptor

		TArray<CSpaceObject *> m_AllObjects;	//	Array of CSpaceObject
		TSortMap<CString, CSpaceObject *> m_NamedObjects;			//	Indexed array of named objects (CSpaceObject *)

		CSystemEventList m_TimedEvents;			//	Array of CTimedEvent
		CEnvironmentGrid mutable *m_pEnvironment;		//	Nebulas, etc.
		CSystemEventHandlerNode m_EventHandlers;	//	List of system event handler
		CNavigationPathNode m_NavPaths;			//	List of navigation paths
		CLocationList m_Locations;				//	List of point locations
		CTerritoryList m_Territories;			//	List of defined territories
		CObjectJointList m_Joints;				//	List of object joints

		int m_iTick;							//	Ticks since beginning of time
		int m_iNextEncounter;					//	Time of next random encounter
		int m_iTimeStopped;						//	Ticks until time restarts
		Metric m_rKlicksPerPixel;				//	Space scale
		Metric m_rTimeScale;					//	Time scale
		int m_iLastUpdated;						//	Tick on which system was last updated (-1 = never)

		DWORD m_fNoRandomEncounters:1;			//	TRUE if no random encounters in this system
		DWORD m_fInCreate:1;					//	TRUE if system in being created
		DWORD m_fEncounterTableValid:1;			//	TRUE if m_pEncounterObj is valid
		DWORD m_fUseDefaultTerritories:1;		//	TRUE if we should use defaults for innerZone, lifeZone, outerZone
		DWORD m_fEnemiesInLRS:1;				//	TRUE if we found enemies in last LRS update
		DWORD m_fEnemiesInSRS:1;				//	TRUE if we found enemies in last SRS update
		DWORD m_fPlayerUnderAttack:1;			//	TRUE if at least one object has player as target
		DWORD m_fLocationsBlocked:1;			//	TRUE if we're already computed overlapping locations

		DWORD m_fSpare:24;

		//	Support structures

		CThreadPool *m_pThreadPool;				//	Thread pool for painting
		CSpaceObjectList m_EncounterObjs;		//	List of objects that generate encounters
		TArray<SStarDesc> m_Stars;				//	List of stars in the system
		CSpaceObjectGrid m_ObjGrid;				//	Grid to help us hit test
		CSpaceObjectList m_DeletedObjects;		//	List of objects deleted in the current update
		CSpaceObjectList m_LayerObjs[layerCount];	//	List of objects by layer
		CSpaceObjectList m_EnhancedDisplayObjs;	//	List of objects to show in viewport periphery
		CSpaceObjectList m_BackgroundObjs;		//	List of background objects to paint in viewport
		CSpaceObjectList m_ForegroundObjs;		//	List of foreground objects to paint in viewport
		TArray<SDeferredOnCreateCtx> m_DeferredOnCreate;	//	Ordered list of objects that need an OnSystemCreated call
		CSystemSpacePainter m_SpacePainter;		//	Paints space background
		CMapGridPainter m_GridPainter;			//	Structure to paint a grid
		CPhysicsContactResolver m_ContactResolver;	//	Resolves physics contacts

		static const Metric g_MetersPerKlick;
	};

