//	TSESystem.h
//
//	Defines classes and interfaces for a star system.
//	Copyright (c) 2016 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

//	Include helper classes

#include "TSEEvents.h"
#include "TSEPhysics.h"
#include "TSEObjectJoints.h"
#include "TSESystemDefs.h"
#include "TSESystemCreate.h"
#include "TSESystemRandomEncounters.h"

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
		void DebugPaintInfo (CSystem &System, CG32bitImage &Dest, int x, int y, ViewportTransform &Xform);
		void DebugPaintInfo (CG32bitImage &Dest, int x, int y, const CMapViewportCtx &Ctx);
		DWORD GetID (void) const { return m_dwID; }
		int GetNavPointCount (void) const { return m_Waypoints.GetCount(); }
		CVector GetNavPoint (int iIndex, Metric *retrDist2 = NULL) const;
		CVector GetPathEnd (void) const { return GetNavPoint(GetNavPointCount() - 1); }
		bool Matches (CSovereign *pSovereign, CSpaceObject *pStart, CSpaceObject *pEnd);
		void OnReadFromStream (SLoadCtx &Ctx);
		void OnWriteToStream (CSystem *pSystem, IWriteStream *pStream) const;

	private:
		static constexpr Metric DEFAULT_NAV_POINT_RADIUS = 24.0 * LIGHT_SECOND;
		static constexpr Metric DEFAULT_NAV_POINT_RADIUS2 = DEFAULT_NAV_POINT_RADIUS * DEFAULT_NAV_POINT_RADIUS;

		struct SWaypoint
			{
			CVector vPos;
			Metric rRange2 = 0.0;				//	How close to waypoint to count as success
			};

		void ComputeWaypointRange (void);
		static int ComputePath (CSystem *pSystem, CSovereign *pSovereign, const CVector &vFrom, const CVector &vTo, TArray<SWaypoint> &retWaypoints, TArray<CVector> *retPointsChecked);
		static bool IsLineBlocked (CSystem &System, const CVector &vFrom, const CVector &vTo);
		static bool PathIsClear (CSystem *pSystem, CSovereign *pSovereign, const CVector &vFrom, const CVector &vTo, CSpaceObject **retpEnemy, CVector *retvAway);

		DWORD m_dwID;							//	ID of path
		CSovereign *m_pSovereign;				//	Sovereign that will use (NULL if all)
		DWORD m_iStartIndex;					//	Index of start object
		DWORD m_iEndIndex;						//	Index of end object

		int m_iSuccesses;						//	Count of ships that successfully reached end
		int m_iFailures;						//	Count of ships that were destroyed

		CVector m_vStart;						//	Start position
		TArray<SWaypoint> m_Waypoints;

#ifdef DEBUG_ASTAR_PATH
		TArray<CVector> m_PointsChecked;
#endif
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

class CLocationList
	{
	public:
		void FillCloseLocations (void);
		void FillInRange (const CVector &vPos, Metric rExclusionRadius);
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
		static constexpr Metric OVERLAP_DIST = 25.0 * LIGHT_SECOND;
		static constexpr Metric OVERLAP_DIST2 = OVERLAP_DIST * OVERLAP_DIST;

		void InvalidateObjIndex (void) { m_ObjIndex.DeleteAll(); }

		TArray<CLocationDef> m_List;
		mutable TSortMap<DWORD, int> m_ObjIndex;	//	Map from Obj ID to location definition//

		bool m_bMinDistCheck = false;		//	If TRUE, then we've checked all locations for min distance
	};

class CTerritoryList
	{
	public:
		~CTerritoryList (void);

		void DebugPaint (CG32bitImage &Dest, CMapViewportCtx &PaintCtx, const CG16bitFont &LabelFont) const;
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
		void PaintSpaceBackground (CG32bitImage &Dest, CSystemType *pType, SViewportPaintCtx &Ctx);
		void PaintStarshine (CG32bitImage &Dest, CSystemType *pType, SViewportPaintCtx &Ctx);
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
		void PaintStarshine (CG32bitImage &Dest, int xCenter, int yCenter, SViewportPaintCtx &Ctx);

		bool m_bInitialized;

		TArray<SStar> m_Starfield;
		int m_cxStarfield;
		int m_cyStarfield;

		CG32bitImage *m_pBackground;
		DWORD m_dwCurBackgroundUNID;
	};

class CGateTimerManager
	{
	public:
		int GetTick (DWORD dwGateID, DWORD dwNow);

	private:
		DWORD m_dwTick = 0;
		TSortMap<DWORD, int> m_Timers;
	};

class CSystem
	{
	public:
		static constexpr Metric MAX_GATE_RANGE =		150.0 * KLICKS_PER_PIXEL;
		static constexpr Metric MAX_GATE_HELP_RANGE =	256.0 * KLICKS_PER_PIXEL;

		//	Other defines

		enum LayerEnum
			{
			layerFirst = 0,

			layerBackground = 0,
			layerSpace = 1,
			layerStations = 2,
			layerBGWeaponFire = 3,
			layerShips = 4,
			layerFGWeaponFire = 5,
			layerEffects = 6,
			layerOverhang = 7,

			layerCount = 8
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
									  CSystemType &Type, 
									  CTopologyNode &NodeID, 
									  CSystem **retpSystem,
									  CString *retsError = NULL,
									  CSystemCreateStats *pStats = NULL);
		virtual ~CSystem (void);

		ALERROR CreateFlotsam (const CItem &Item, const CVector &vPos, const CVector &vVel, CSovereign *pSovereign, CStation **retpFlotsam);
		ALERROR CreateLookup (SSystemCreateCtx *pCtx, const CString &sTable, const COrbit &OrbitDesc, CXMLElement *pSubTables);
		ALERROR CreateMarker (CXMLElement *pDesc, const COrbit &oOrbit, CMarker **retpObj);
		ALERROR CreateParticles (CXMLElement *pDesc, const COrbit &oOrbit, CParticleEffect **retpObj);
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
								const CVector &vPos,
								const CString &sStargateID,
								const CString &sDestNodeID,
								const CString &sDestStargateID,
								CSpaceObject **retpStation = NULL);
		ALERROR CreateStation (CStationType *pType,
							   CDesignType *pEventHandler,
							   const CVector &vPos,
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
		int CalculateLightIntensity (const CVector &vPos, CSpaceObject **retpStar = NULL, const CG8bitSparseImage **retpVolumetricMask = NULL) const;
		CVector CalcRandomEncounterPos (const CSpaceObject &TargetObj, Metric rDistance, const CSpaceObject *pEncounterBase = NULL) const;
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
		CSpaceObject *FindObjectInRange (CSpaceObject *pSource, const CVector &vCenter, Metric rRange, const CSpaceObjectCriteria &Criteria = CSpaceObjectCriteria()) const;
		CSpaceObject *FindObjectWithOrbit (const COrbit &Orbit) const;
		bool FindObjectName (const CSpaceObject *pObj, CString *retsName = NULL);
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
		CPhysicsForceResolver &GetForceResolver (void) { return m_ForceResolver; }
		DWORD GetID (void) { return m_dwID; }
		int GetLastUpdated (void) { return m_iLastUpdated; }
		int GetLevel (void) const;
		const CLocationList &GetLocations (void) const { return m_Locations; }
		CLocationList &GetLocations (void) { return m_Locations; }
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
		ICCItemPtr GetProperty (CCodeChainCtx &CCX, const CString &sProperty) const;
		const CRandomEncounterObjTable &GetRandomEncounterObjTable (void) const { return m_EncounterObjTable; }
		const CRandomEncounterTypeTable &GetRandomEncounterTypeTable (void) const { return m_EncounterTypeTable; }
		static DWORD GetSaveVersion (void);
		CG32bitPixel GetSpaceColor (void) const { return (m_pType ? m_pType->GetSpaceColor() : CSystemType::DEFAULT_SPACE_COLOR); }
		Metric GetSpaceScale (void) const { return m_rKlicksPerPixel; }
		int GetTick (void) { return m_iTick; }
		int GetTileSize (void) const;
		Metric GetTimeScale (void) const { return m_rTimeScale; }
		CTopologyNode *GetTopology (void) const { return m_pTopology; }
		CSystemType *GetType (void) const { return m_pType; }
		CSpaceEnvironmentType *GetSpaceEnvironment (int xTile, int yTile);
		CSpaceEnvironmentType *GetSpaceEnvironment (const CVector &vPos, int *retxTile = NULL, int *retyTile = NULL);
		CTopologyNode *GetStargateDestination (const CString &sStargate, CString *retsEntryPoint);
		CSpaceObject *GetStargateInRange (const CVector &vPos, CSpaceObject **retpStargateNearby = NULL) const;
		CUniverse &GetUniverse (void) const { return m_Universe; }
		bool HasAttribute (const CVector &vPos, const CString &sAttrib) const;
		bool HasRandomEncounters (void) const { return !m_fNoRandomEncounters; }
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
		void PaintViewport (CG32bitImage &DestBG, CG32bitImage& DestFG, const RECT &rcView, CSpaceObject *pCenter, DWORD dwFlags, SViewportAnnotations *pAnnotations = NULL);
		void PaintViewportGrid (CMapViewportCtx &Ctx, CG32bitImage &Dest, Metric rGridSize);
		void PaintViewportObject (CG32bitImage &Dest, const RECT &rcView, CSpaceObject *pCenter, CSpaceObject *pObj);
		void PaintViewportLRS (CG32bitImage &Dest, const RECT &rcView, CSpaceObject *pCenter, Metric rScale, DWORD dwFlags, bool *retbNewEnemies);

		static constexpr DWORD FLAG_VIEWPORT_MAP_SHOW_ZONES =		0x00000001;
		void PaintViewportMap (CG32bitImage &Dest, const RECT &rcView, CSpaceObject *pCenter, Metric rMapScale, DWORD dwFlags = 0);
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

		static constexpr DWORD FLAG_REQUIRED_LOCATION = 0x00000001;
		ALERROR CreateLocation (const CString &sID, const COrbit &Orbit, const CString &sAttributes, DWORD dwFlags = 0, CLocationDef **retpLocation = NULL);

		bool FindRandomLocation (const SLocationCriteria &Criteria, DWORD dwFlags, const COrbit &CenterOrbitDesc, CStationType *pStationToPlace, int *retiLocID);
		int GetEmptyLocationCount (void);
		CLocationSelectionTable GetEmptyLocations (const SLocationCriteria &Criteria, const COrbit &CenterOrbitDesc, CStationType *pStationToPlace, Metric rMinExclusion);
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
		static constexpr int LEVEL_ENCOUNTER_CHANCE = 10;

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

		CG32bitPixel CalcStarshineColor (CSpaceObject *pPOV, CSpaceObject **retpStar = NULL, const CG8bitSparseImage **retpVolumetricMask = NULL) const;
		void CalcViewportCtx (SViewportPaintCtx &Ctx, const RECT &rcView, CSpaceObject *pCenter, DWORD dwFlags);
		void CalcVolumetricMask (CSpaceObject *pStar, CG8bitSparseImage &VolumetricMask);
		void ComputeStars (void);
		ALERROR CreateStationInt (SSystemCreateCtx *pCtx,
								  CStationType *pType,
								  SObjCreateCtx &CreateCtx,
								  CSpaceObject **retpStation,
								  CString *retsError = NULL);
		const SStarDesc *FindNearestStar (const CVector &vPos, int *retiDist = NULL) const;
		void FlushDeletedObjects (void);
		int GetTimedEventCount (void) { return m_TimedEvents.GetCount(); }
		CSystemEvent *GetTimedEvent (int iIndex) { return m_TimedEvents.GetEvent(iIndex); }
		void InitSpaceEnvironment (void) const;
		void InitVolumetricMask (void);
		void PaintDestinationMarker (SViewportPaintCtx &Ctx, CG32bitImage &Dest, int x, int y, CSpaceObject *pObj);
		void PaintViewportAnnotations (CG32bitImage &Dest, SViewportAnnotations &Annotations, SViewportPaintCtx &Ctx);
		void RemoveVolumetricShadow (CSpaceObject *pObj);
		void SetPainted (void);
		void SetStarlightParams (void);
		void SetStarlightParams (const CSpaceObject &StarObj);
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
		DWORD m_fUseDefaultTerritories:1;		//	TRUE if we should use defaults for innerZone, lifeZone, outerZone
		DWORD m_fEnemiesInLRS:1;				//	TRUE if we found enemies in last LRS update
		DWORD m_fEnemiesInSRS:1;				//	TRUE if we found enemies in last SRS update
		DWORD m_fPlayerUnderAttack:1;			//	TRUE if at least one object has player as target
		DWORD m_fLocationsBlocked:1;			//	TRUE if we're already computed overlapping locations
		DWORD m_fFlushEventHandlers:1;			//	TRUE if we need to flush m_EventHandlers

		DWORD m_fSpare1:1;
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
		CRandomEncounterObjTable m_EncounterObjTable;
		CRandomEncounterTypeTable m_EncounterTypeTable;

		TArray<SStarDesc> m_Stars;				//	List of stars in the system
		CSpaceObjectGrid m_ObjGrid;				//	Grid to help us hit test
		CSpaceObjectList m_DeletedObjects;		//	List of objects deleted in the current update
		TArray<SDeferredOnCreateCtx> m_DeferredOnCreate;	//	Ordered list of objects that need an OnSystemCreated call
		CSystemSpacePainter m_SpacePainter;		//	Paints space background
		CMapGridPainter m_GridPainter;			//	Structure to paint a grid
		CPhysicsContactResolver m_ContactResolver;	//	Resolves physics contacts
		CPhysicsForceResolver m_ForceResolver;	//	Resolves physics forces
		CGateTimerManager m_GateTimer;			//	Assign delay when ships exit a gate

		//	Property table

		static TPropertyHandler<CSystem> m_PropertyTable;
	};

inline CUniverse &SSystemCreateCtx::GetUniverse (void) { return System.GetUniverse(); }
