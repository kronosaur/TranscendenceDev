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
		DWORD GetID (void) const { return m_dwID; }
		int GetNavPointCount (void) const { return m_iWaypointCount; }
		CVector GetNavPoint (int iIndex) const;
		CVector GetPathEnd (void) const { return GetNavPoint(GetNavPointCount() - 1); }
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
		static void Create (CSpaceObject *pObj, Metric rMaxRange, CSystemEventHandler **retpHandler);

		CSpaceObject *GetObj (void) const { return m_pHandler; }
		bool InRange (const CVector &vPos) const;
		bool IsDeleted (void) const { return (m_pHandler == NULL); }
		bool OnObjDestroyed (CSpaceObject *pObjDestroyed);
		void OnReadFromStream (SLoadCtx &Ctx);
		bool OnUnregister (CSpaceObject *pObj);
		void OnWriteToStream (CSystem *pSystem, IWriteStream *pStream) const;
		void SetRange (Metric rMaxRange) { m_rMaxRange2 = rMaxRange * rMaxRange; }

	private:
		CSpaceObject *m_pHandler = NULL;		//	Event handler (may be NULL if object destroyed)
		Metric m_rMaxRange2 = 0.0;				//	Beyond this range from handler, no events seen
	};

typedef TSEListNode<CSystemEventHandler> CSystemEventHandlerNode;

//	System

class COrbit
	{
	public:
		COrbit (void) { }
		COrbit (const CVector &vCenter, Metric rRadius, Metric rPos = 0.0);

		bool operator== (const COrbit &Src) const;

		ICCItemPtr AsItem (void) const;
		const Metric &GetEccentricity (void) const { return m_rEccentricity; }
		const CVector3D &GetFocus3D (void) const { return m_vFocus; }
		const CVector &GetFocus (void) const { return CVector(m_vFocus.GetX(), m_vFocus.GetY()); }
		const Metric &GetInclination (void) const { return m_rInclination; }
		const Metric &GetObjectAngle (void) const { return m_rPos; }
		CVector GetObjectPos (Metric *retrZ = NULL) const { return GetPoint(m_rPos, retrZ); }
		CVector3D GetObjectPos3D (void) const;
		CVector GetPoint (Metric rAngle, Metric *retrZ = NULL) const;
		CVector GetPointAndRadius (Metric rAngle, Metric *retrRadius) const;
		CVector GetPointCircular (Metric rAngle) const;
		const Metric &GetRotation (void) const { return m_rRotation; }
		const Metric &GetSemiMajorAxis (void) const { return m_rSemiMajorAxis; }
		bool IsNull (void) const { return (m_rSemiMajorAxis == 0.0); }
		void Paint (CMapViewportCtx &Ctx, CG32bitImage &Dest, CG32bitPixel rgbColor);
        void PaintHD (CMapViewportCtx &Ctx, CG32bitImage &Dest, CG32bitPixel rgbColor, CGDraw::EBlendModes iMode = CGDraw::blendNormal) const;
		void ReadFromStream (SLoadCtx &Ctx);
		void SetEccentricity (Metric rValue) { m_rEccentricity = rValue; }
		void SetFocus (const CVector &vCenter) { m_vFocus = CVector3D(vCenter.GetX(), vCenter.GetY(), 0.0); }
		void SetFocus (const CVector3D &vCenter) { m_vFocus = vCenter; }
		void SetInclination (Metric rValue) { m_rInclination = rValue; }
		void SetObjectAngle (Metric rValue) { m_rPos = rValue; }
		void SetRotation (Metric rValue) { m_rRotation = rValue; }
		void SetSemiMajorAxis (Metric rValue) { m_rSemiMajorAxis = rValue; }
		void WriteToStream (IWriteStream &Stream) const;

		static bool FromItem (const ICCItem &Item, COrbit &retOrbit);
		static Metric RandomAngle (void) { return mathDegreesToRadians(mathRandom(0,3599) / 10.0); }
		static Metric ZToParallax (Metric rZ);

	private:
		static constexpr Metric CAMERA_DIST = LIGHT_SECOND * 60.0;

		struct SSerialized
			{
			CVector vFocus;
			Metric rEccentricity = 0.0;
			Metric rSemiMajorAxis = 0.0;
			Metric rRotation = 0.0;
			Metric rPos = 0.0;
			Metric rInclination = 0.0;
			Metric rFocusZ = 0.0;
			};

        CG32bitPixel GetColorAtRadiusHD (CG32bitPixel rgbColor, Metric rRadius) const;

		CVector3D m_vFocus;				//	Focus of orbit
		Metric m_rEccentricity = 0.0;	//	Ellipse eccentricity
		Metric m_rSemiMajorAxis = 0.0;	//	Semi-major axis
		Metric m_rRotation = 0.0;		//	Angle of rotation (radians)
		Metric m_rInclination = 0.0;	//	Angle of inclination (radians)

		Metric m_rPos = 0.0;			//	Obj position in orbit (radians)
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
		int GetCacheSize (void) const { return m_Cache.GetCount(); }

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
		void AddStationTable (CSystem *pSystem, const CString &sStationCriteria, const CString &sLocationAttribs, TArray<CStationTableCache::SEntry> &Table);
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
		bool FindEncounterTable (TArray<CStationTableCache::SEntry> &Src, SEncounterTable **retpTable);

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
	SLocationCriteria (void) { }
	explicit SLocationCriteria (const CAffinityCriteria &Criteria) :
			AttribCriteria(Criteria)
		{ }

	CAffinityCriteria AttribCriteria;		//	Attribute criteria
	Metric rMinDist = 0.0;					//	Minimum distance from source
	Metric rMaxDist = 0.0;					//	Maximum distance from source
	};

struct SZAdjust
	{
	CString sInclination;					//	Override inclination attrib
	CString sRotation;						//	Override rotation attrib
	DiceRange ZOffset;						//	Override Z position (in scale units)
	Metric rScale = LIGHT_SECOND;			//	Scale units

	bool bIgnoreLocations = false;			//	Do not adjust locations
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

	//	Options

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
		int GetCount (void) { return m_iCount; }
		CSpaceObject *GetObj (int iIndex) { return m_pArray[iIndex].pObj; }

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

		const CString &GetAttributes (void) const { return m_sAttributes; }
		DWORD GetObjID (void) const { return m_dwObjID; }
		const COrbit &GetOrbit (void) const { return m_OrbitDesc; }
		bool HasAttribute (const CString &sAttrib) const { return ::HasModifier(m_sAttributes, sAttrib); }
		bool IsBlocked (void) const { return m_bBlocked; }
		bool IsEmpty (void) const { return (m_dwObjID == 0 && !m_bBlocked); }
		void SetAttributes (const CString &sAttribs) { m_sAttributes = sAttribs; }
		void SetBlocked (bool bBlocked = true) { m_bBlocked = bBlocked; }
		void SetID (const CString &sID) { m_sID = sID; }
		void SetObjID (DWORD dwObjID) { m_dwObjID = dwObjID; }
		void SetOrbit (const COrbit &Orbit) { m_OrbitDesc = Orbit; }
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
		void FillCloseLocations (void);
		void FillOverlappingWith (CSpaceObject *pObj);
		int GetCount (void) { return m_List.GetCount(); }
		bool GetEmptyLocations (TArray<int> *retList);
		const CLocationDef &GetLocation (int iIndex) const { return m_List[iIndex]; }
		CLocationDef &GetLocation (int iIndex) { return m_List[iIndex]; }
		const CLocationDef *GetLocationByObjID (DWORD dwObjID) const;
		CLocationDef *Insert (const CString &sID = NULL_STR);
		void ReadFromStream (SLoadCtx &Ctx);
		void SetBlocked (int iIndex) { m_List[iIndex].SetBlocked(); }
		void SetObjID (int iIndex, DWORD dwObjID) { m_List[iIndex].SetObjID(dwObjID); InvalidateObjIndex(); }
		void WriteToStream (IWriteStream *pStream);

	private:
		void InvalidateObjIndex (void) { m_ObjIndex.DeleteAll(); }

		TArray<CLocationDef> m_List;
		mutable TSortMap<DWORD, int> m_ObjIndex;	//	Map from Obj ID to location definition//

		bool m_bMinDistCheck = false;		//	If TRUE, then we've checked all locations for min distance
	};

class CTerritoryDef
	{
	public:
		CTerritoryDef (void);

		void AddAttributes (const CString &sAttribs);
		void AddRegion (const COrbit &Orbit, Metric rMinRadius, Metric rMaxRadius);
		static ALERROR CreateFromXML (CXMLElement *pDesc, const COrbit &OrbitDesc, CTerritoryDef **retpTerritory);
		const CString &GetAttributes (void) const { return m_sAttributes; }
		const CString &GetCriteria (void) { return m_sCriteria; }
		bool HasAttribute (const CString &sAttrib);
		bool HasAttribute (const CVector &vPos, const CString &sAttrib);
		bool IsMarked (void) { return m_bMarked; }
		bool PointInTerritory (const CVector &vPos);
		void ReadFromStream (SLoadCtx &Ctx);
		void SetCriteria (const CString &sCriteria) { m_sCriteria = sCriteria; }
		void SetMarked (bool bMarked = true) { m_bMarked = bMarked; }
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
		int GetCount (void) { return m_List.GetCount(); }
		bool HasAttribute (const CVector &vPos, const CString &sAttrib) const;
		void Insert (CTerritoryDef *pTerritory) { m_List.Insert(pTerritory); }
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
		int GetTileSize (void) const { return m_iTileSize; }
		CSpaceEnvironmentType *GetTileType (int xTile, int yTile, DWORD *retdwEdgeMask = NULL);
		bool HasMoreTiles (STileMapEnumerator &i) const { return m_Map.HasMore(i); }
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
		bool InBounds (int xTile, int yTile) const { return (xTile >= 0 && xTile < m_iTileCount && yTile >= 0 && yTile <= m_iTileCount); }
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
		bool IsEmpty (void) const { return (m_HorzLines.GetCount() == 0); }
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
			//	PaintViewport flags
			VWP_ENHANCED_DISPLAY =			0x00000001,	//	Show enhanced display markers
			VWP_NO_STAR_FIELD =				0x00000002,	//	Do not paint star field background
			VWP_MINING_DISPLAY =			0x00000004,	//	Show unexplored asteroids
			};

		struct SDebugInfo
			{
			int iTotalObjs = 0;					//	Total number of non-NULL CSpaceObjects
			int iDestroyedObjs = 0;				//	->IsDestroyed() == true
			int iDeletedObj = 0;				//	In m_DeletedObjects
			int iBadObjs = 0;					//	Crash when trying to access object
			int iStarObjs = 0;					//	Total stars

			bool bBadStarCache = false;			//	m_Stars array is bad.
			};

		//	System methods

		static ALERROR CreateEmpty (CUniverse &Universe, CTopologyNode *pTopology, CSystem **retpSystem);
		static ALERROR CreateFromStream (CUniverse &Universe, 
										 IReadStream *pStream, 
										 CSystem **retpSystem,
										 CString *retsError,
										 DWORD dwObjID = OBJID_NULL,
										 CSpaceObject **retpObj = NULL,
										 CSpaceObject *pPlayerShip = NULL);
		static ALERROR CreateFromXML (CUniverse &Universe, 
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
		ALERROR CreateWeaponFire (SShotCreateCtx &Ctx, CSpaceObject **retpShot = NULL);
		ALERROR CreateWeaponFragments (SShotCreateCtx &Ctx, CSpaceObject *pMissileSource, int iFraction = 100);

		bool AddJoint (CObjectJoint::ETypes iType, CSpaceObject *pFrom, CSpaceObject *pTo, CObjectJoint **retpJoint = NULL);
		bool AddJoint (CObjectJoint::ETypes iType, CSpaceObject *pFrom, CSpaceObject *pTo, ICCItem *pOptions, DWORD *retdwID = NULL);
		ALERROR AddTimedEvent (CSystemEvent *pEvent);
		void AddToDeleteList (CSpaceObject *pObj);
		ALERROR AddToSystem (CSpaceObject *pObj, int *retiIndex);
		bool AscendObject (CSpaceObject *pObj, CString *retsError = NULL);
		int CalculateLightIntensity (const CVector &vPos, CSpaceObject **retpStar = NULL, const CG8bitSparseImage **retpVolumetricMask = NULL);
		CVector CalcRandomEncounterPos (const CSpaceObject &TargetObj, Metric rDistance, const CSpaceObject *pEncounterBase = NULL) const;
		CG32bitPixel CalculateSpaceColor (CSpaceObject *pPOV, CSpaceObject **retpStar = NULL, const CG8bitSparseImage **retpVolumetricMask = NULL);
		void CancelTimedEvent (CSpaceObject *pSource, bool bInDoEvent = false);
		void CancelTimedEvent (CSpaceObject *pSource, const CString &sEvent, bool bInDoEvent = false);
		void CancelTimedEvent (CDesignType *pSource, const CString &sEvent, bool bInDoEvent = false);
		bool DescendObject (DWORD dwObjID, const CVector &vPos, CSpaceObject **retpObj = NULL, CString *retsError = NULL);
		bool EnemiesInLRS (void) const { return m_fEnemiesInLRS; }
		void EnumObjectsInBoxStart (SSpaceObjectGridEnumerator &i, const CVector &vUR, const CVector &vLL, DWORD dwFlags = 0) const { m_ObjGrid.EnumStart(i, vUR, vLL, dwFlags); }
		void EnumObjectsInBoxStart (SSpaceObjectGridEnumerator &i, const CVector &vPos, Metric rRange, DWORD dwFlags = 0) const
			{
			CVector vRange = CVector(rRange, rRange);
			CVector vUR = vPos + vRange;
			CVector vLL = vPos - vRange;
			m_ObjGrid.EnumStart(i, vUR, vLL, dwFlags);
			}
		bool EnumObjectsInBoxHasMore (SSpaceObjectGridEnumerator &i) const { return i.bMore; }
		CSpaceObject *EnumObjectsInBoxGetNext (SSpaceObjectGridEnumerator &i) const { return m_ObjGrid.EnumGetNext(i); }
		CSpaceObject *EnumObjectsInBoxGetNextFast (SSpaceObjectGridEnumerator &i) const { return m_ObjGrid.EnumGetNextFast(i); }
		CSpaceObject *EnumObjectsInBoxPointGetNext (SSpaceObjectGridEnumerator &i) const { return m_ObjGrid.EnumGetNextInBoxPoint(i); }
		CSpaceObject *FindObject (DWORD dwID) const;
		CSpaceObject *FindObjectInRange (const CVector &vCenter, Metric rRange, const CSpaceObjectCriteria &Criteria = CSpaceObjectCriteria()) const;
        CSpaceObject *FindObjectWithOrbit (const COrbit &Orbit) const;
		bool FindObjectName (CSpaceObject *pObj, CString *retsName = NULL);
		void FireOnSystemExplosion (CSpaceObject *pExplosion, CWeaponFireDesc *pDesc, const CDamageSource &Source);
		void FireOnSystemObjAttacked (SDamageCtx &Ctx);
		void FireOnSystemObjDestroyed (SDestroyCtx &Ctx);
		void FireOnSystemWeaponFire (CSpaceObject *pShot, CWeaponFireDesc *pDesc, const CDamageSource &Source, int iRepeatingCount);
		void FireSystemWeaponEvents (CSpaceObject *pShot, CWeaponFireDesc *pDesc, const CDamageSource &Source, int iRepeatingCount, DWORD dwFlags);
		void FlushAllCaches (void);
		void FlushEnemyObjectCache (void);
		CString GetAttribsAtPos (const CVector &vPos);
		void GetDebugInfo (SDebugInfo &Info) const;
		CEnvironmentGrid *GetEnvironmentGrid (void) { InitSpaceEnvironment(); return m_pEnvironment; }
		DWORD GetID (void) { return m_dwID; }
		int GetLastUpdated (void) { return m_iLastUpdated; }
		int GetLevel (void);
		const CLocationList &GetLocations (void) const { return m_Locations; }
		CSpaceObject *GetNamedObject (const CString &sName);
		const CString &GetName (void) const { return m_sName; }
		CNavigationPath *GetNavPath (CSovereign *pSovereign, CSpaceObject *pStart, CSpaceObject *pEnd);
		CNavigationPath *GetNavPathByID (DWORD dwID);
		CSpaceObject *GetObject (int iIndex) const { return m_AllObjects[iIndex]; }
		int GetObjectCount (void) const { return m_AllObjects.GetCount(); }
		const CSpaceObjectGrid &GetObjectGrid (void) const { return m_ObjGrid; }
		void GetObjectsInBox (const CVector &vPos, Metric rRange, CSpaceObjectList &Result)
			{
			CVector vRange = CVector(rRange, rRange);
			CVector vUR = vPos + vRange;
			CVector vLL = vPos - vRange;
			m_ObjGrid.GetObjectsInBox(vUR, vLL, Result);
			}
		void GetObjectsInBox (const CVector &vUR, const CVector &vLL, CSpaceObjectList &Result) { m_ObjGrid.GetObjectsInBox(vUR, vLL, Result); }
		CSpaceObject *GetPlayerShip (void) const;
		static DWORD GetSaveVersion (void);
		Metric GetSpaceScale (void) const { return m_rKlicksPerPixel; }
		int GetTick (void) { return m_iTick; }
		int GetTileSize (void) const;
		Metric GetTimeScale (void) const { return m_rTimeScale; }
		CTopologyNode *GetTopology (void) { return m_pTopology; }
		CSystemType *GetType (void) { return m_pType; }
		CSpaceEnvironmentType *GetSpaceEnvironment (int xTile, int yTile);
		CSpaceEnvironmentType *GetSpaceEnvironment (const CVector &vPos, int *retxTile = NULL, int *retyTile = NULL);
		CTopologyNode *GetStargateDestination (const CString &sStargate, CString *retsEntryPoint);
		CUniverse &GetUniverse (void) const { return m_Universe; }
		bool HasAttribute (const CVector &vPos, const CString &sAttrib) const;
		CSpaceObject *HitScan (CSpaceObject *pExclude, const CVector &vStart, const CVector &vEnd, bool bExcludeWorlds, CVector *retvHitPos = NULL);
		CSpaceObject *HitTest (CSpaceObject *pExclude, const CVector &vPos, bool bExcludeWorlds);
		bool IsCreationInProgress (void) const { return (m_fInCreate ? true : false); }
		bool IsPlayerUnderAttack (void) const { return m_fPlayerUnderAttack; }
		bool IsStarAtPos (const CVector &vPos);
		bool IsStationInSystem (CStationType *pType);
		bool IsTimeStopped (void) { return (m_iTimeStopped != 0); }
		void MarkImages (void);
		void NameObject (const CString &sName, CSpaceObject &Obj);
		CVector OnJumpPosAdj (CSpaceObject *pObj, const CVector &vPos);
		void OnPlayerChangedShips (CSpaceObject &OldShip, CSpaceObject &NewShip, SPlayerChangedShipsCtx &Options);
		void OnStationDestroyed (SDestroyCtx &Ctx);
		void PaintViewport (CG32bitImage &Dest, const RECT &rcView, CSpaceObject *pCenter, DWORD dwFlags, SViewportAnnotations *pAnnotations = NULL);
		void PaintViewportGrid (CMapViewportCtx &Ctx, CG32bitImage &Dest, Metric rGridSize);
		void PaintViewportObject (CG32bitImage &Dest, const RECT &rcView, CSpaceObject *pCenter, CSpaceObject *pObj);
		void PaintViewportLRS (CG32bitImage &Dest, const RECT &rcView, CSpaceObject *pCenter, Metric rScale, DWORD dwFlags, bool *retbNewEnemies);
		void PaintViewportMap (CG32bitImage &Dest, const RECT &rcView, CSpaceObject *pCenter, Metric rMapScale);
		void PaintViewportMapObject (CG32bitImage &Dest, const RECT &rcView, CSpaceObject *pCenter, CSpaceObject *pObj);
		void PlaceInGate (CSpaceObject *pObj, CSpaceObject *pGate);
		void PlayerEntered (CSpaceObject *pPlayer);
		void RegisterEventHandler (CSpaceObject *pObj, Metric rRange);
		void RegisterForOnSystemCreated (CSpaceObject *pObj) { m_DeferredOnCreate.Insert(SDeferredOnCreateCtx(pObj)); }
		void RegisterForOnSystemCreated (CSpaceObject *pObj, CStationType *pEncounter, const COrbit &Orbit);
		void RemoveObject (SDestroyCtx &Ctx);
		void RestartTime (void);
		ALERROR SaveToStream (IWriteStream *pStream);
		void SetID (DWORD dwID) { m_dwID = dwID; }
		void SetLastUpdated (void);
		void SetName (const CString &sName) { m_sName = sName; }
		void SetPlayerUnderAttack (void) { m_fPlayerUnderAttack = true; }
		void SetPOVLRS (CSpaceObject *pCenter);
		void SetSpaceEnvironment (int xTile, int yTile, CSpaceEnvironmentType *pEnvironment);
		ALERROR StargateCreated (CSpaceObject *pGate, const CString &sStargateID, const CString &sDestNodeID, const CString &sDestEntryPoint);
		void StopTime (const CSpaceObjectList &Targets, int iDuration);
		void StopTimeForAll (int iDuration, CSpaceObject *pExcept);
		CVector TileToVector (int x, int y) const;
		void TransferObjEventsIn (CSpaceObject *pObj, CSystemEventList &ObjEvents);
		void TransferObjEventsOut (CSpaceObject *pObj, CSystemEventList &ObjEvents);
		void UnnameObject (CSpaceObject &Obj);
		void UnregisterEventHandler (CSpaceObject *pObj);
		void Update (SSystemUpdateCtx &SystemCtx, SViewportAnnotations *pAnnotations = NULL);
		void UpdateExtended (const CTimeSpan &ExtraTime);
		void ValidateExclusionRadius (void) const;
		void ValidateExclusionRadius (CSpaceObject *pObj, const CStationEncounterDesc::SExclusionDesc &Exclusion) const;
		void VectorToTile (const CVector &vPos, int *retx, int *rety) const;
		void WriteObjRefToStream (CSpaceObject *pObj, IWriteStream *pStream, CSpaceObject *pReferrer = NULL);
		void WriteSovereignRefToStream (CSovereign *pSovereign, IWriteStream *pStream);

		//	Locations & Territories
		ALERROR AddTerritory (CTerritoryDef *pTerritory);
		void BlockOverlappingLocations (void);
		int CalcLocationAffinity (const CAffinityCriteria &Criteria, const CString &sLocationAttribs, const CVector &vPos) const;
		int CalcLocationAffinity (const CLocationDef &Loc, const CAffinityCriteria &Criteria) const { return CalcLocationAffinity(Criteria, Loc.GetAttributes(), Loc.GetOrbit().GetObjectPos()); }
		ALERROR CreateLocation (const CString &sID, const COrbit &Orbit, const CString &sAttributes, CLocationDef **retpLocation = NULL);
		bool FindRandomLocation (const SLocationCriteria &Criteria, DWORD dwFlags, const COrbit &CenterOrbitDesc, CStationType *pStationToPlace, int *retiLocID);
		int GetEmptyLocationCount (void);
		bool GetEmptyLocations (const SLocationCriteria &Criteria, const COrbit &CenterOrbitDesc, CStationType *pStationToPlace, Metric rMinExclusion, TProbabilityTable<int> *retTable);
		bool GetEmptyLocations (TArray<int> *retList) { return m_Locations.GetEmptyLocations(retList); }
		const CLocationDef &GetLocation (int iLocID) const { return m_Locations.GetLocation(iLocID); }
		CLocationDef &GetLocation (int iLocID) { return m_Locations.GetLocation(iLocID); }
		int GetLocationCount (void) { return m_Locations.GetCount(); }
		bool IsExclusionZoneClear (const CVector &vPos, const CStationType &Type) const;
		bool IsExclusionZoneClear (const CVector &vPos, Metric rMinExclusion) const;
		bool MatchesLocationAffinity (const CAffinityCriteria &Criteria, const CString &sLocationAttribs, const CVector &vPos) const;
		void SetLocationObjID (int iLocID, DWORD dwObjID) { m_Locations.SetObjID(iLocID, dwObjID); }

		static Metric CalcApparentSpeedAdj (Metric rSpeed);
		static void GetObjRefFromID (SLoadCtx &Ctx, DWORD dwID, CSpaceObject **retpObj);
		static void ReadObjRefFromStream (SLoadCtx &Ctx, CSpaceObject **retpObj, bool bOptional = false);
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

		CSystem (CUniverse &Universe, CTopologyNode *pTopology);

		void CalcViewportCtx (SViewportPaintCtx &Ctx, const RECT &rcView, CSpaceObject *pCenter, DWORD dwFlags);
		void CalcVolumetricMask (CSpaceObject *pStar, CG8bitSparseImage &VolumetricMask);
		void ComputeRandomEncounters (void);
		void ComputeStars (void);
		ALERROR CreateStationInt (SSystemCreateCtx *pCtx,
								  CStationType *pType,
								  SObjCreateCtx &CreateCtx,
								  CSpaceObject **retpStation,
								  CString *retsError = NULL);
		void FlushDeletedObjects (void);
		int GetTimedEventCount (void) { return m_TimedEvents.GetCount(); }
		CSystemEvent *GetTimedEvent (int iIndex) { return m_TimedEvents.GetEvent(iIndex); }
		void InitSpaceEnvironment (void) const;
		void InitVolumetricMask (void);
		void PaintDestinationMarker (SViewportPaintCtx &Ctx, CG32bitImage &Dest, int x, int y, CSpaceObject *pObj);
		void PaintViewportAnnotations (CG32bitImage &Dest, SViewportAnnotations &Annotations, SViewportPaintCtx &Ctx);
		void RemoveVolumetricShadow (CSpaceObject *pObj);
		void SetPainted (void);
		void SortByPaintOrder (void);
		void UpdateCollisionTesting (SUpdateCtx &Ctx);
		void UpdateGravity (SUpdateCtx &Ctx, CSpaceObject *pGravityObj);
		void UpdateRandomEncounters (void);

		//	Game instance data

		CUniverse &m_Universe;					//	Universe that we belong to
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

		DWORD m_fFlushEventHandlers:1;			//	TRUE if we need to flush m_EventHandlers
		DWORD m_fSpare2:1;
		DWORD m_fSpare3:1;
		DWORD m_fSpare4:1;
		DWORD m_fSpare5:1;
		DWORD m_fSpare6:1;
		DWORD m_fSpare7:1;
		DWORD m_fSpare8:1;

		DWORD m_fSpare:16;

		//	Support structures

		CThreadPool *m_pThreadPool;				//	Thread pool for painting
		CSpaceObjectList m_EncounterObjs;		//	List of objects that generate encounters
		TArray<SStarDesc> m_Stars;				//	List of stars in the system
		CSpaceObjectGrid m_ObjGrid;				//	Grid to help us hit test
		CSpaceObjectList m_DeletedObjects;		//	List of objects deleted in the current update
		TArray<SDeferredOnCreateCtx> m_DeferredOnCreate;	//	Ordered list of objects that need an OnSystemCreated call
		CSystemSpacePainter m_SpacePainter;		//	Paints space background
		CMapGridPainter m_GridPainter;			//	Structure to paint a grid
		CPhysicsContactResolver m_ContactResolver;	//	Resolves physics contacts

		static const Metric g_MetersPerKlick;
	};

inline CUniverse &SSystemCreateCtx::GetUniverse (void) { return System.GetUniverse(); }
