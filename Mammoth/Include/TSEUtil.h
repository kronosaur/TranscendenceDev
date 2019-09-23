//	TSEUtil.h
//
//	Transcendence Utilities
//	Copyright 2012 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CDesignType;
class CEconomyType;
class CItemCtx;
class CItemEnhancementStack;
class CExtension;
class CLocationDef;
class COrbit;
class CTopology;
class CTopologyNode;
struct CItemCriteria;
struct SDesignLoadCtx;
struct SDamageCtx;
struct SDestroyCtx;
struct SSystemCreateCtx;
struct SSpaceObjectGridEnumerator;

//	Utility inlines

inline int Angle2Direction (int iAngle, int iRotationCount)
	{
	return ((iRotationCount - (iAngle / (360 / iRotationCount))) + (iRotationCount / 4)) % iRotationCount;
	}
inline int Direction2Angle (int iDirection, int iRotationCount)
	{
	return (720 - (360 * (iDirection - (iRotationCount / 4) + iRotationCount) / iRotationCount)) % 360;
	}
inline int DirectionFacesUp (int iDirection, int iRotationCount)
	{
	int iQuarter = (iRotationCount / 4);
	return (iDirection < iQuarter) || (iDirection > (iRotationCount - iQuarter));
	}
inline bool AreAnglesAligned (int iDesiredAngle, int iActualAngle, int iTolerance)
	{
	if (iActualAngle < iDesiredAngle)
		iActualAngle += 360;

	int iOffset = 180 - abs((iActualAngle - iDesiredAngle) - 180);

	return (iOffset <= iTolerance);
	}
inline int AngleOffset (int iAngle1, int iAngle2)
	{
	if (iAngle1 < iAngle2)
		iAngle1 += 360;

	return 180 - abs((iAngle1 - iAngle2) - 180);
	}
inline int AngleRange (int iMinAngle, int iMaxAngle)
	{
	if (iMinAngle > iMaxAngle)
		return (iMaxAngle + 360 - iMinAngle);
	else
		return (iMaxAngle - iMinAngle);
	}
inline int AngleBearing (int iDir, int iTarget)
	{
	int iBearing = AngleRange(iDir, iTarget);
	if (iBearing > 180)
		return iBearing - 360;
	else
		return iBearing;
	}
inline Metric Ticks2Seconds (int iTicks) { return (Metric)iTicks * g_SecondsPerUpdate; }

//	Debugging Support

enum ProgramStates
	{
	psUnknown,							//	No idea what the program is doing now, but
										//		it is none of the other states.
	psAnimating,						//	Program is in Animate() function, but not in
										//		anything more specific.
	psPaintingSRS,						//	We are in Universe.PaintPOV()
	psUpdating,							//	In Universe.Update()
	psUpdatingEvents,					//	In Universe.Update(); updating event objects
	psUpdatingBehavior,					//	In Universe.Update(); updating obj behavior
	psUpdatingObj,						//	In Universe.Update(); updating obj
	psUpdatingHitTest,					//	In Universe.Update(); updating hit test
	psUpdatingMove,						//	In Universe.Update(); updating obj motion
	psUpdatingEncounters,				//	In Universe.Update(); updating random encounters
	psUpdatingExtended,					//	In System.UpdateExtended(); updating obj
	psDestroyNotify,					//	Calling OnObjDestroyed
	psUpdatingReactorDisplay,			//	In CReactorDisplay::Update
	psPaintingMap,						//	In CTranscendenceWnd::PaintMap
	psPaintingLRS,						//	In CTranscendenceWnd::PaintLRS
	psPaintingArmorDisplay,				//	In CArmorDisplay::Paint
	psPaintingMessageDisplay,			//	In CMessageDisplay::Paint
	psPaintingReactorDisplay,			//	In CReactorDisplay::Paint
	psPaintingTargetDisplay,			//	In CTargetDisplay::Paint
	psPaintingDeviceDisplay,			//	In CDeviceCounterDisplay::Paint
	psStargateEnter,					//	In CTranscendenceWnd::EnterStargate
	psStargateEndGame,					//	In EnterStargate, end game
	psStargateLoadingSystem,			//	In EnterStargate, loading system
	psStargateCreatingSystem,			//	In EnterStargate, creating system
	psStargateUpdateExtended,			//	In EnterStargate, updating system
	psStargateTransferringGateFollowers,//	In EnterStargate, transferring gate followers
	psStargateSavingSystem,				//	In EnterStargate, saving the system
	psStargateFlushingSystem,			//	In EnterStargate, flushing old system
	psStargateGarbageCollecting,		//	In EnterStargate, garbage collecting images
	psStargateLoadingBitmaps,			//	In EnterStargate, loading images
	psStargateEnterDone,				//	In EnterStargate, done
	psOnObjDestroyed,					//	In OnObjDestroyed
	psLoadingImages,					//	In CSpaceObject::LoadImages
	psCustomError,						//	g_sProgramError is set
	};

#ifdef DEBUG_PROGRAMSTATE
extern ProgramStates g_iProgramState;
extern CSpaceObject *g_pProgramObj;
extern CSystemEvent *g_pProgramEvent;
extern CString *g_sProgramError;
inline void SetProgramState (ProgramStates iState) { g_iProgramState = iState; g_pProgramObj = NULL; }
inline void SetProgramState (ProgramStates iState, CSpaceObject *pObj) { g_iProgramState = iState; g_pProgramObj = pObj; }
inline void SetProgramObj (CSpaceObject *pObj) { g_pProgramObj = pObj; }
inline void SetProgramEvent (CSystemEvent *pEvent) { g_pProgramEvent = pEvent; }
inline void SetProgramError (const CString &sError) { if (g_sProgramError) delete g_sProgramError; g_sProgramError = new CString(sError); g_iProgramState = psCustomError; }

#define DEBUG_SAVE_PROGRAMSTATE		\
	ProgramStates iDEBUG_SavedState = g_iProgramState;	\
	CSpaceObject *pDEBUG_SavedObj = g_pProgramObj;	\
	CSystemEvent *pDEBUG_SavedEvent = g_pProgramEvent;

#define DEBUG_RESTORE_PROGRAMSTATE	\
	g_iProgramState = iDEBUG_SavedState;	\
	g_pProgramObj = pDEBUG_SavedObj;	\
	g_pProgramEvent = pDEBUG_SavedEvent;

#else
inline void SetProgramState (ProgramStates iState) { }
inline void SetProgramState (ProgramStates iState, CSpaceObject *pObj) { }
inline void SetProgramObj (CSpaceObject *pObj) { }
inline void SetProgramEvent (CSystemEvent *pEvent) { }
inline void SetProgramError (const CString &sError) { }
#define DEBUG_SAVE_PROGRAMSTATE
#define DEBUG_RESTORE_PROGRAMSTATE
#endif

#ifdef DEBUG_PERFORMANCE
extern DWORD g_dwPerformanceTimer;
inline void DebugStartTimer (void) { g_dwPerformanceTimer = ::GetTickCount(); }
inline void DebugStopTimer (char *szTiming)
	{
	char szBuffer[1024];
	wsprintf(szBuffer, "%s: %d ms\n", szTiming, ::GetTickCount() - g_dwPerformanceTimer);
	::OutputDebugString(szBuffer);
	}
#else
inline void DebugStartTimer (void) { }
inline void DebugStopTimer (char *szTiming) { }
#endif

#define TRY(f)	{try { error = f; } catch (...) { error = ERR_FAIL; }}

//	Game load/save structures

#ifdef TRANSCENDENCE_STABLE_RELEASE
#define UPGRADE_ENTRY_UNID								CONSTLIT("Engine:Transcendence.stable")
#else
#define UPGRADE_ENTRY_UNID								CONSTLIT("Engine:Transcendence.next")
#endif

struct SUniverseLoadCtx
	{
	inline CUniverse &GetUniverse (void) const { return *g_pUniverse; }

	DWORD dwVersion;					//	See CUniverse.cpp for version history
	DWORD dwSystemVersion;				//	System version when universe was saved

	IReadStream *pStream;				//	Stream to load from
	};

enum ELoadStates
	{
	loadStateUnknown,					//	Unknown load state
	loadStateObject,					//	Inside CSpaceObect::CreateFromStream
	loadStateObjData,					//	Loading obj opaque data
	loadStateObjEffects,				//	Loading the list of effects
	loadStateObjSubClass,				//	Load sub-class specific data
	loadStateEffect,					//	Loading an effect

	//	Make sure you add to the table in Utilities.cpp when adding new 
	//	entries to this enum.
	};

typedef void (*PRESOLVEOBJIDPROC) (void *pCtx, DWORD dwObjID, CSpaceObject *pObj);

class CSpaceObjectAddressResolver
	{
	public:
		bool HasUnresolved (void);
		void InsertRef (DWORD dwObjID, void *pCtx, PRESOLVEOBJIDPROC pfnResolveProc);
		void InsertRef (DWORD dwObjID, CSpaceObject **ppAddr);
		void ResolveRefs (DWORD dwObjID, CSpaceObject *pObj);

	private:
		struct SEntry
			{
			PRESOLVEOBJIDPROC pfnResolveProc;	//	If NULL, then pCtx is an address to fix up.
			void *pCtx;
			};

		TSortMap<DWORD, TArray<SEntry>> m_List;
	};

struct SLoadCtx
	{
	SLoadCtx (CUniverse &UniverseArg) : 
			m_Universe(UniverseArg),
			dwVersion(SYSTEM_SAVE_VERSION),
			pStream(NULL),
			pSystem(NULL),
			iLoadState(loadStateUnknown),
			dwObjClassID(0)
		{ }

	SLoadCtx (const SUniverseLoadCtx &Ctx) :
			m_Universe(Ctx.GetUniverse()),
			dwVersion(Ctx.dwSystemVersion),
			pStream(Ctx.pStream),
			pSystem(NULL),
			iLoadState(loadStateUnknown),
			dwObjClassID(0)
		{ }

	inline CUniverse &GetUniverse (void) { return m_Universe; }

	DWORD dwVersion;					//	See CSystem.cpp for version history

	IReadStream *pStream;				//	Stream to load from
	CSystem *pSystem;					//	System to load into

	TSortMap<DWORD, CSpaceObject *> ObjMap;			//	Map of ID to objects.
	CSpaceObjectAddressResolver ForwardReferences;

	//	For backwards compatibility we keep track of the list of objects
	//	that want a subscription to the given ObjID.

	TSortMap<DWORD, TArray<CSpaceObject *>> Subscribed;

	//	Diagnostics

	ELoadStates iLoadState;				//	Current load state
	DWORD dwObjClassID;					//	ClassID that we're trying to load
	CString sEffectUNID;				//	UNID of effect we're loading

	private:
		CUniverse &m_Universe;
	};

struct SViewportAnnotations
	{
	SViewportAnnotations (void)
		{
		Init();
		}

	void Init (void)
		{
		bDummy = false;

#ifdef DEBUG_FORMATION
		bDebugFormation = false;
#endif
		}

	bool bDummy;

#ifdef DEBUG_FORMATION
	bool bDebugFormation;
	int iFormationAngle;
	CVector vFormationPos;
	CVector vFormationCurPos;
#endif
	};

struct SUpdateCtx
	{
	public:
		int GetLightIntensity (CSpaceObject *pObj) const;
		inline bool IsTimeStopped (void) const { return m_bTimeStopped; }
		inline void SetTimeStopped (bool bValue = true) { m_bTimeStopped = bValue; }

		CSystem *pSystem = NULL;					//	Current system
		CSpaceObject *pPlayer = NULL;				//	The player
		TArray<CSpaceObject *> PlayerObjs;			//	List of player objects, if pPlayer == NULL
		SViewportAnnotations *pAnnotations = NULL;	//	Extra structure to deliver to PaintViewport

		//	Used to compute nearest docking port to player

		CSpaceObject *pDockingObj = NULL;			//	If non-null, nearest object to dock with
		int iDockingPort = -1;						//	Nearest docking port
		CVector vDockingPort;						//	Position of docking port (absolute)
		Metric rDockingPortDist2 = 0.0;				//	Distance from player to docking port

		//	Used to compute player's auto target

		bool bNeedsAutoTarget = false;				//	TRUE if player's weapon needs an autotarget
		bool bPlayerTargetOutOfRange = false;		//	TRUE if player's current target is unreachable
		int iPlayerPerception = 0;					//	Player's perception

		CSpaceObject *pPlayerTarget = NULL;			//	Current player target (may be NULL)
		CSpaceObject *pTargetObj = NULL;			//	If non-null, nearest possible target for player
		Metric rTargetDist2 = g_InfiniteDistance2;	//	Distance from player to target
		int iMinFireArc = 0;						//	Fire arc of primary weapon
		int iMaxFireArc = 0;

		//	Misc flags

		bool bGravityWarning = false;				//	Player in a dangerous gravity field
		bool bHasShipBarriers = false;				//	TRUE if the system has ship barriers (e.g., Arena)
		bool bHasGravity = false;					//	TRUE if the system has gravity

	private:

		//	About the object being updated

		bool m_bTimeStopped = false;				//	Object is currently time-stopped (cached for perf).

		//	Cached computed values

		mutable CSpaceObject *m_pCacheObj = NULL;	//	Cached values apply to this object.
		mutable int m_iLightIntensity = -1;			//	Light intensity at pCacheObj (-1 if not computed).
	};

//	Utility classes

class CIDCounter
	{
	public:
		CIDCounter (DWORD dwFirstID = 1) : m_dwNextID(dwFirstID) { }
		DWORD GetID (void) { return m_dwNextID++; }

	private:
		DWORD m_dwNextID;
	};

class CAttributeCriteria
	{
	public:
		enum EMatchStrength
			{
			matchRequired =					0x00010000,
			matchExcluded =					0x00020000,
			};

		enum EFlags
			{
			flagDefault =					0x00000001,
			};

		CAttributeCriteria (void) :
				m_dwFlags(0)
			{ }

		int AdjLocationWeight (CSystem *pSystem, CLocationDef *pLoc, int iOriginalWeight = 1000) const;
		int AdjStationWeight (CStationType *pType, int iOriginalWeight = 1000) const;
		int CalcLocationWeight (CSystem *pSystem, const CString &sLocationAttribs, const CVector &vPos) const;
		int CalcNodeWeight (CTopologyNode *pNode) const;
		inline int GetCount (void) const { return m_Attribs.GetCount(); }
		const CString &GetAttribAndRequired (int iIndex, bool *retbRequired) const;
		const CString &GetAttribAndWeight (int iIndex, DWORD *retdwMatchStrength, bool *retbIsSpecial = NULL) const;
		inline bool MatchesAll (void) const { return (GetCount() == 0); }
		inline bool MatchesDefault (void) const { return (m_dwFlags & flagDefault); }
		ALERROR Parse (const CString &sCriteria, DWORD dwFlags = 0, CString *retsError = NULL);

		static int CalcLocationWeight (CSystem *pSystem, const CString &sLocationAttribs, const CVector &vPos, const CString &sAttrib, DWORD dwMatchStrength);
		static int CalcWeightAdj (bool bHasAttrib, DWORD dwMatchStrength, int iAttribFreq = -1);
		static void WriteAsString (IWriteStream &Stream, const TArray<CString> &Attribs, const CString &sPrefix);

	private:
		enum MatchStrengthEncoding
			{
			CODE_MASK =						0xFFFF0000,
			VALUE_MASK =					0x0000FFFF,

			CODE_REQUIRED =					0x00010000,
			CODE_EXCLUDED =					0x00020000,
			CODE_SEEK =						0x00030000,		//	Value = 1-3
			CODE_AVOID =					0x00040000,		//	Value = 1-3
			CODE_INCREASE_IF =				0x00050000,		//	Value = % to increase
			CODE_INCREASE_UNLESS =			0x00060000,		//	Value = % to increase
			CODE_DECREASE_IF =				0x00070000,		//	Value = % to decrease
			CODE_DECREASE_UNLESS =			0x00080000,		//	Value = % to decrease

			matchSeek1 =					0x00030001,
			matchSeek2 =					0x00030002,
			matchSeek3 =					0x00030003,

			matchAvoid1 =					0x00040001,
			matchAvoid2 =					0x00040002,
			matchAvoid3 =					0x00040003,
			};

		struct SEntry
			{
			CString sAttrib;
			DWORD dwMatchStrength;
			bool bIsSpecial;
			};

		static int CalcWeightAdjCustom (bool bHasAttrib, DWORD dwMatchStrength);
		static int CalcWeightAdjWithAttribFreq (bool bHasAttrib, DWORD dwMatchStrength, int iAttribFreq);

		TArray<SEntry> m_Attribs;
		DWORD m_dwFlags;
	};

class DiceRange
	{
	public:
		DiceRange (void) : m_iFaces(0), m_iCount(0), m_iBonus(0) { }
		DiceRange (int iFaces, int iCount, int iBonus);

		inline int GetAveValue (void) const { return (m_iCount * (m_iFaces + 1) / 2) + m_iBonus; }
		inline Metric GetAveValueFloat (void) const { return (m_iFaces > 0 ? ((m_iCount * (m_iFaces + 1.0) / 2.0) + m_iBonus) : m_iBonus); }
		inline int GetBonus (void) const { return m_iBonus; }
		inline int GetCount (void) const { return m_iCount; }
		inline int GetFaces (void) const { return m_iFaces; }
		inline int GetMaxValue (void) const { return m_iFaces * m_iCount + m_iBonus; }
		inline int GetMinValue (void) const { return m_iCount + m_iBonus; }
		inline bool IsConstant (void) const { return (m_iFaces * m_iCount) == 0; }
		inline bool IsEmpty (void) const { return (m_iFaces == 0 && m_iCount == 0 && m_iBonus == 0); }
		int Roll (void) const;
		int RollSeeded (int iSeed) const;
		ALERROR LoadFromXML (const CString &sAttrib, int iDefault, CString *retsSuffix = NULL);
		inline ALERROR LoadFromXML (const CString &sAttrib, CString *retsSuffix = NULL) { return LoadFromXML(sAttrib, 0, retsSuffix); }
		void ReadFromStream (SLoadCtx &Ctx);
		CString SaveToXML (void) const;
		void Scale (Metric rScale);
		void SetConstant (int iValue) { m_iFaces = 0; m_iCount = 0; m_iBonus = iValue; }
		void WriteToStream (IWriteStream *pStream) const;

		static bool LoadIfValid (const CString &sAttrib, DiceRange *retValue);

	private:
		int m_iFaces;
		int m_iCount;
		int m_iBonus;
	};

class CCurrencyBlock
	{
	public:
		CurrencyValue GetCredits (DWORD dwEconomyUNID);
		CurrencyValue GetCredits (const CString &sCurrency);
		CurrencyValue IncCredits (DWORD dwEconomyUNID, CurrencyValue iInc);
		CurrencyValue IncCredits (const CString &sCurrency, CurrencyValue iInc);
		void ReadFromStream (SLoadCtx &Ctx);
		void SetCredits (DWORD dwEconomyUNID, CurrencyValue iValue);
		void SetCredits (const CString &sCurrency, CurrencyValue iValue);
		void WriteToStream (IWriteStream *pStream);

	private:
		struct SEntry
			{
			CurrencyValue iValue;
			};

		TSortMap<DWORD, SEntry> m_Block;
	};

struct SEventHandlerDesc
	{
    SEventHandlerDesc (void) :
            pExtension(NULL),
            pCode(NULL)
        { }

	CExtension *pExtension;
	ICCItem *pCode;
	};

class CEventHandler
	{
	public:
		CEventHandler (void);
		~CEventHandler (void);

		CEventHandler &operator= (const CEventHandler &Src);

		void AddEvent (const CString &sEvent, ICCItem *pCode);
		ALERROR AddEvent (const CString &sEvent, const CString &sCode, CString *retsError = NULL);
		ALERROR AddEvent (CXMLElement *pEventXML, CString *retsError = NULL);
		void DeleteAll (void);
		bool FindEvent (const CString &sEvent, ICCItem **retpCode) const;
		inline int GetCount (void) const { return m_Handlers.GetCount(); }
		const CString &GetEvent (int iIndex, ICCItem **retpCode = NULL) const;
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		inline bool IsEmpty (void) const { return m_Handlers.GetCount() == 0; }
		void MergeFrom (const CEventHandler &Src);

		static bool IsDiagnosticsEvent (const CString &sEvent);

	private:
		TSortMap<CString, ICCItem *> m_Handlers;
	};

class CGlobalEventCache
	{
	public:
		CGlobalEventCache (const CString &sEvent) : m_sEvent(sEvent) { }

		inline void DeleteAll (void) { m_Cache.DeleteAll(); }
		inline int GetCount (void) const { return m_Cache.GetCount(); }
		inline CDesignType *GetEntry (int iIndex, SEventHandlerDesc *retEvent = NULL) const
			{
			if (retEvent)
				*retEvent = m_Cache[iIndex].Event;

			return m_Cache[iIndex].pType;
			}
		bool Insert (CDesignType *pType, const CString &sEvent, const SEventHandlerDesc &Event);

	private:
		struct SEntry
			{
			CDesignType *pType;
			SEventHandlerDesc Event;
			};

		CString m_sEvent;
		TArray<SEntry> m_Cache;
	};

template <typename EVENT_ENUM, size_t N> class TEventHandlerCache
	{
	public:
		bool FindEventHandler (EVENT_ENUM iEvent, SEventHandlerDesc *retEvent = NULL) const
			{
			if (!m_Cache[iEvent].pCode)
				return false;

			if (retEvent)
				{
				retEvent->pExtension = m_Cache[iEvent].pExtension;
				retEvent->pCode = m_Cache[iEvent].pCode;
				}

			return true;
			}

		void Init (CDesignType *pType, LPSTR pEvents[N])
			{
			for (int i = 0; i < N; i++)
				{
				SEventHandlerDesc Handler;
				if (pType->FindEventHandler(CString(pEvents[i], -1, true), &Handler))
					{
					m_Cache[i].pExtension = Handler.pExtension;
					m_Cache[i].pCode = (Handler.pCode ? Handler.pCode->Reference() : NULL);
					}
				else
					{
					m_Cache[i].pExtension = NULL;
					m_Cache[i].pCode = NULL;
					}
				}
			}

	private:
		struct SEntry
			{
			CExtension *pExtension = NULL;
			ICCItemPtr pCode;
			};

		SEntry m_Cache[N];
	};

enum DestructionTypes
	{
	removedFromSystem				= 0,

	killedByDamage					= 1,	//	Ship destroyed
	killedByRunningOutOfFuel		= 2,	//	Ran out of fuel
	killedByRadiationPoisoning		= 3,	//	Radiation poisoning
	killedBySelf					= 4,	//	Self-destruct
	killedByDisintegration			= 5,	//	Ship disintegrated
	killedByWeaponMalfunction		= 6,	//	Weapon malfunction
	killedByEjecta					= 7,	//	Killed by ejecta
	killedByExplosion				= 8,	//	Killed when a station (or ship) exploded
	killedByShatter					= 9,	//	Killed by shatter effect
	killedByPlayerCreatedExplosion	= 10,	//	Killed by explosion created by the player
	enteredStargate					= 11,	//	Entered a stargate
	killedByOther					= 12,	//	Custom death
	killedByGravity					= 13,	//	Killed by white dwarf/neutron star/black hole
	killedByPowerFailure			= 14,	//	Reactor stopped working 
	ascended						= 15,	//	Only for NPCs

	killedNone						= -1,
	killedCount						= 16
	};

class CDamageSource
	{
	public:
		CDamageSource (void) : m_pSource(NULL), m_iCause(removedFromSystem), m_dwFlags(0), m_pSecondarySource(NULL) { }
		CDamageSource (CSpaceObject *pSource, DestructionTypes iCause = killedByDamage, CSpaceObject *pSecondarySource = NULL, const CString &sSourceName = NULL_STR, DWORD dwSourceFlags = 0);

		bool CanHit (CSpaceObject *pTarget) const;
		bool CanHitFriends (void) const;
		inline DestructionTypes GetCause (void) const { return m_iCause; }
		CString GetDamageCauseNounPhrase (DWORD dwFlags);
		CSpaceObject *GetObj (void) const;
		DWORD GetObjID (void) const;
		CSpaceObject *GetOrderGiver (void) const;
		inline CSpaceObject *GetSecondaryObj (void) const { return m_pSecondarySource; }
		inline const CString &GetSourceName (DWORD *retdwNameFlags) const { if (retdwNameFlags) *retdwNameFlags = m_dwSourceNameFlags; return m_sSourceName; }
		CSovereign *GetSovereign (void) const;
		DWORD GetSovereignUNID (void) const;
		inline bool HasDamageCause (void) const { return ((m_pSource && !IsObjID()) || !m_sSourceName.IsBlank()); }
		bool HasSource (void) const;
		inline bool IsAutomatedWeapon (void) const { return ((m_dwFlags & FLAG_IS_AUTOMATED_WEAPON) ? true : false); }
		bool IsCausedByEnemyOf (CSpaceObject *pObj) const;
		bool IsCausedByFriendOf (CSpaceObject *pObj) const;
		bool IsCausedByNonFriendOf (CSpaceObject *pObj) const;
		inline bool IsCausedByPlayer (void) const { return ((m_dwFlags & FLAG_IS_PLAYER_CAUSED) ? true : false); }
		inline bool IsEmpty (void) const { return (GetObj() == NULL); }
		bool IsEnemy (CDamageSource &Src) const;
		bool IsEqual (const CDamageSource &Src) const;
		bool IsEqual (CSpaceObject *pSrc) const;
		bool IsFriend (CSovereign *pSovereign) const;
		inline bool IsPlayer (void) const { return ((m_dwFlags & FLAG_IS_PLAYER) ? true : false); }
		void OnLeaveSystem (void);
		void OnObjDestroyed (CSpaceObject &ObjDestroyed);
		void ReadFromStream (SLoadCtx &Ctx);
		inline void SetAutomatedWeapon (bool bValue = true) { if (bValue) m_dwFlags |= FLAG_IS_AUTOMATED_WEAPON; else m_dwFlags &= FLAG_IS_AUTOMATED_WEAPON; }
		inline void SetCause (DestructionTypes iCause) { m_iCause = iCause; }
		void SetObj (CSpaceObject *pSource);
		void WriteToStream (CSystem *pSystem, IWriteStream *pStream);

		static const CDamageSource &Null (void) { return m_Null; }
		
	private:
		enum Flags
			{
			FLAG_IS_PLAYER					= 0x00000001,
			FLAG_IS_PLAYER_SUBORDINATE		= 0x00000002,
			FLAG_IS_PLAYER_CAUSED			= 0x00000004,
			FLAG_OBJ_ID						= 0x00000008,	//	m_pSource is an ID, not a pointer

			FLAG_IS_AUTOMATED_WEAPON		= 0x00000010,	//	Source is a missile-defense system.
			};

		inline DWORD GetRawObjID (void) const { return (DWORD)m_pSource; }
		inline bool IsObjPointer (void) const { return (m_pSource && !IsObjID()); }
		inline bool IsObjID (void) const { return ((m_dwFlags & FLAG_OBJ_ID) == FLAG_OBJ_ID); }

		CSpaceObject *m_pSource;
		DestructionTypes m_iCause;
		DWORD m_dwFlags;

		CString m_sSourceName;
		DWORD m_dwSourceNameFlags;

		CSpaceObject *m_pSecondarySource;

		static CDamageSource m_Null;
	};

class CDamageAdjDesc
	{
	public:
		enum EConstants
			{
			MAX_DAMAGE_ADJ = 100000,
			};

		CDamageAdjDesc (void) : m_pDefault(NULL)
			{ }

		ALERROR Bind (SDesignLoadCtx &Ctx, const CDamageAdjDesc *pDefault);
		int GetAbsorbAdj (DamageTypes iDamageType) const;
		inline int GetAdj (DamageTypes iDamageType) const { return (iDamageType == damageGeneric ? 100 : m_iDamageAdj[iDamageType]); }
		void GetAdjAndDefault (DamageTypes iDamageType, int *retiAdj, int *retiDefault) const;
		int GetHPBonus (DamageTypes iDamageType) const;
		ICCItem *GetDamageAdjProperty (const CItemEnhancementStack *pEnhancements = NULL) const;
		ICCItem *GetHPBonusProperty (const CItemEnhancementStack *pEnhancements = NULL) const;
		ALERROR InitFromArray (int *pTable);
		ALERROR InitFromDamageAdj (SDesignLoadCtx &Ctx, const CString &sAttrib, bool bNoDefault);
		ALERROR InitFromHPBonus (SDesignLoadCtx &Ctx, const CString &sAttrib);
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, bool bIsDefault = false);
		bool IsEmpty (void) const;

		static int GetBonusFromAdj (int iDamageAdj, int iDefault = 100);
		static int GetDamageAdjFromHPBonus (int iBonus);
		static Metric GetDamageTypeFraction (int iLevel, DamageTypes iDamageType);

	private:
		enum EAdjustmentTypes
			{
			adjDefault,							//	Use default table
			adjAbsolute,						//	dwAdjValue is an absolute adjustment
			adjRelative,						//	dwAdjValue is a relative percent of default
			};

		struct SAdjDesc
			{
			DWORD dwAdjType:16;					//	Type of adjustment
			DWORD dwAdjValue:16;				//	Adjustment value
			};

		void Compute (const CDamageAdjDesc *pDefault);

		SAdjDesc m_Desc[damageCount];			//	Descriptor for computing adjustment
		int m_iDamageAdj[damageCount];			//	Computed adjustment for type

		const CDamageAdjDesc *m_pDefault;		//	Default table
	};

struct SVisibleDamage
	{
	SVisibleDamage (void) :
			iShieldLevel(-1),
			iArmorLevel(-1),
			iHullLevel(-1)
		{ }

	int iShieldLevel;				//	0-100: shield level; -1 = no shields
	int iArmorLevel;				//	0-100: armor integrity; -1 = no armor
	int iHullLevel;					//	0-100: hull integrity; -1 = no interior compartments
	};

class CPerceptionCalc
	{
	public:
		enum EConstants
			{
			RANGE_ARRAY_SIZE = 16,
			};

		CPerceptionCalc (int iPerception = -1);

		bool CanBeTargeted (CSpaceObject *pTarget, Metric rTargetDist2) const;
		bool CanBeTargetedAtDist (CSpaceObject *pTarget, Metric rTargetDist) const;
		Metric GetMaxDist (CSpaceObject *pTarget) const;
		Metric GetMaxDist2 (CSpaceObject *pTarget) const;
		inline int GetPerception (void) const { return m_iPerception; }
		inline int GetRangeIndex (int iStealth) const { return GetRangeIndex(iStealth, m_iPerception); }
		bool IsVisibleInLRS (CSpaceObject *pSource, CSpaceObject *pTarget) const;
		inline void SetPerception (int iPerception) { m_iPerception = iPerception; }

		inline static Metric GetRange (int iIndex) { return (iIndex < 0 ? g_InfiniteDistance : (iIndex >= RANGE_ARRAY_SIZE ? 0.0 : m_rRange[iIndex])); }
		inline static Metric GetRange2 (int iIndex) { return (iIndex < 0 ? g_InfiniteDistance : (iIndex >= RANGE_ARRAY_SIZE ? 0.0 : m_rRange2[iIndex])); }
		static int GetRangeIndex (int iStealth, int iPerception);

	private:
		bool IsVisibleDueToAttack (CSpaceObject *pTarget) const;

		static void InitRangeTable (void);

		int m_iPerception;
		DWORD m_dwLastAttackThreshold;			//	Last attack on or after means visible

		static bool m_bRangeTableInitialized;
		static Metric m_rRange[RANGE_ARRAY_SIZE];
		static Metric m_rRange2[RANGE_ARRAY_SIZE];
	};

class CRandomEntryResults
	{
	public:
		CRandomEntryResults (void);
		~CRandomEntryResults (void);

		void AddResult (CXMLElement *pElement, int iCount);
		inline int GetCount (void) { return m_Results.GetCount(); }
		CXMLElement *GetResult (int iIndex);
		int GetResultCount (int iIndex);

	private:
		struct SResultEntry
			{
			int iCount;
			CXMLElement *pElement;
			};

		TArray<SResultEntry> m_Results;
	};

class IElementGenerator
	{
	public:
		struct SCtx
			{
			const CTopology *pTopology = NULL;
			const CTopologyNode *pNode = NULL;
			};

		struct SResult
			{
			int iCount = 0;
			CXMLElement *pElement = NULL;
			};

		enum EGeneratorTypes
			{
			typeGroup,
			typeNodeDistanceTable,
			typeNull,
			typeTable,

			typeElement,
			};

		virtual ~IElementGenerator (void) { }

		virtual void Generate (SCtx &Ctx, TArray<SResult> &retResults) const = 0;
		void Generate (SCtx &Ctx, TArray<CXMLElement *> &retResults) const;

		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, TUniquePtr<IElementGenerator> &retGenerator);
		static ALERROR CreateFromXMLAsGroup (SDesignLoadCtx &Ctx, CXMLElement *pDesc, TUniquePtr<IElementGenerator> &retGenerator);
		static ALERROR CreateFromXMLAsTable (SDesignLoadCtx &Ctx, CXMLElement *pDesc, TUniquePtr<IElementGenerator> &retGenerator);
		static bool Generate (SCtx &Ctx, CXMLElement *pDesc, TArray<SResult> &retResult, CString *retsError = NULL);
		static bool Generate (SCtx &Ctx, CXMLElement *pDesc, TArray<CXMLElement *> &retResult, CString *retsError = NULL);
		static bool GenerateAsGroup (SCtx &Ctx, CXMLElement *pDesc, TArray<SResult> &retResult, CString *retsError = NULL);
		static bool GenerateAsGroup (SCtx &Ctx, CXMLElement *pDesc, TArray<CXMLElement *> &retResult, CString *retsError = NULL);
		static bool GenerateAsTable (SCtx &Ctx, CXMLElement *pDesc, TArray<SResult> &retResult, CString *retsError = NULL);
		static bool GenerateAsTable (SCtx &Ctx, CXMLElement *pDesc, TArray<CXMLElement *> &retResult, CString *retsError = NULL);
		inline static bool IsGeneratorTag (const CString &sTag) { return (GetGeneratorType(sTag) != typeElement); }

	protected:
		static EGeneratorTypes GetGeneratorType (const CString &sTag);

		DiceRange m_Count;

	private:
		ALERROR InitFromXMLInternal (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
	};

class CSpaceObjectList
	{
	public:
		CSpaceObjectList (void);
		~CSpaceObjectList (void);

		void Add (CSpaceObject *pObj, int *retiIndex = NULL);
		void Add (const CSpaceObjectList &List);
		inline void CleanUp (void) { m_List.DeleteAll(); }
		inline void Delete (int iIndex) { m_List.Delete(iIndex); }
		bool Delete (CSpaceObject *pObj);
		void DeleteSystemObjs (void);
		inline void DeleteAll (void) { m_List.DeleteAll(); }
		inline void FastAdd (CSpaceObject *pObj, int *retiIndex = NULL) { if (retiIndex) *retiIndex = m_List.GetCount(); m_List.Insert(pObj); }
		void FastAdd (const CSpaceObjectList &List);
		inline bool FindObj (CSpaceObject *pObj, int *retiIndex = NULL) const { return m_List.Find(pObj, retiIndex); }
		inline int GetCount (void) const { return m_List.GetCount(); }
		inline CSpaceObject *GetObj (int iIndex) const { return m_List[iIndex]; }
		inline CSpaceObject *GetRandomObj (void) const { return (m_List.GetCount() == 0 ? NULL : m_List[mathRandom(0, m_List.GetCount() - 1)]); }
		inline TArray<CSpaceObject *> &GetRawList (void) { return m_List; }
		inline bool IsEmpty (void) const { return (m_List.GetCount() == 0); }
		void NotifyOnObjDestroyed (SDestroyCtx &Ctx);
		void NotifyOnObjDocked (CSpaceObject *pDockingObj, CSpaceObject *pDockTarget);
		void NotifyOnObjEnteredGate (CSpaceObject *pGatingObj, CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate);
		bool NotifyOnObjGateCheck (CSpaceObject *pGatingObj, CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pGateObj);
		void NotifyOnObjJumped (CSpaceObject *pJumpObj);
		void NotifyOnObjReconned (CSpaceObject *pReconnedObj);
		void NotifyOnPlayerBlacklisted (CSpaceObject *pBlacklistingObj);
		void ReadFromStream (SLoadCtx &Ctx, bool bIgnoreMissing = false);
		void ReadFromStreamSingle (SLoadCtx &Ctx);
		void SetAllocSize (int iNewCount);
		inline void SetObj (int iIndex, CSpaceObject *pObj) { m_List[iIndex] = pObj; }
		void Subtract (const CSpaceObjectList &List);
		void WriteToStream (CSystem *pSystem, IWriteStream *pStream);

	private:
		static void ResolveObjProc (void *pCtx, DWORD dwObjID, CSpaceObject *pObj);

		TArray<CSpaceObject *> m_List;
	};

struct SDeviceEnhancementDesc
	{
	SDeviceEnhancementDesc (void) :
		iBonus(0),
		iActivateAdj(100),
		iMinActivateDelay(0),
		iMaxActivateDelay(0) { }

	CString sID;						//	ID of entity conferring bonus

	int iBonus;							//	%increase for hp/damage/etc (0 = none)

	int iActivateAdj;					//	Rate adjustment (100 = no adj)
	int iMinActivateDelay;				//	Do not decrease delay below this value (0 if no limit)
	int iMaxActivateDelay;				//	Do not increase delay above this value (0 if no limit)
	};

//	CTileMap (for nebulosity)

class CTileMapSection;

class CTile
	{
	public:
		CTile (void) : m_dwData(0) { }

		inline DWORD GetTile (void) { return m_dwData; }
		inline DWORD *GetTilePointer (void) { return &m_dwData; }
		inline CTileMapSection *GetTileMapSection (void) { return (CTileMapSection *)m_dwData; }
		inline void SetTile (DWORD dwTile) { m_dwData = dwTile; }
		inline void SetTileMapSection (CTileMapSection *pMap) { m_dwData = (DWORD)pMap; }

	private:
		DWORD m_dwData;
	};

class CTileMapSection
	{
	public:
		CTileMapSection (int iCount) { m_pMap = new CTile [iCount]; }
		~CTileMapSection (void) { delete [] m_pMap; }

		inline DWORD GetTile (int iIndex) { return m_pMap[iIndex].GetTile(); }
		inline DWORD *GetTilePointer (int iIndex) { return m_pMap[iIndex].GetTilePointer(); }
		inline CTileMapSection *GetTileMapSection (int iIndex) { return m_pMap[iIndex].GetTileMapSection(); }
		inline ALERROR ReadFromStream (int iCount, IReadStream *pStream) { return pStream->Read((char *)m_pMap, iCount * sizeof(CTile)); }
		inline void SetTile (int iIndex, DWORD dwTile) { m_pMap[iIndex].SetTile(dwTile); }
		inline void SetTileMapSection (int iIndex, CTileMapSection *pMap) { m_pMap[iIndex].SetTileMapSection(pMap); }
		inline void WriteToStream (int iCount, IWriteStream *pStream) const { pStream->Write((char *)m_pMap, iCount * sizeof(CTile)); }

	private:
		CTile *m_pMap;
	};

struct STileMapSectionPos
	{
	CTileMapSection *pMap;
	int iIndex;
	STileMapSectionPos *pParent;
	};

struct STileMapEnumerator
	{
	STileMapEnumerator (void) : pCurPos(NULL), bDone(false) { }

	STileMapSectionPos *pCurPos;
	bool bDone;
	};

class CTileMap
	{
	public:
		CTileMap (void);
		CTileMap (int iSize, int iScale);
		~CTileMap (void);

		static ALERROR CreateFromStream (IReadStream *pStream, CTileMap **retpMap);
		void GetNext (STileMapEnumerator &i, int *retx, int *rety, DWORD *retdwTile) const;
		DWORD GetTile (int x, int y) const;
		DWORD *GetTilePointer (int x, int y);
		inline int GetScale (void) const { return m_iScale; }
		int GetTotalSize (void) const;
		inline int GetSize (void) const { return m_iSize; }
		bool HasMore (STileMapEnumerator &i) const;
		void Init (int iSize, int iScale);
		ALERROR ReadFromStream (IReadStream *pStream);
		void SetTile (int x, int y, DWORD dwTile);
		void WriteToStream (IWriteStream *pStream) const;

	private:
		void ComputeDenominator (void);
		void CleanUpMapSection (CTileMapSection *pMap, int iScale);
		ALERROR ReadMapSection (IReadStream *pStream, int iScale, CTileMapSection **retpSection);
		bool SelectNext (STileMapEnumerator &i) const;
		void WriteMapSection (CTileMapSection *pMap, int iScale, IWriteStream *pStream) const;

		int m_iSize;
		int m_iScale;
		int m_iFirstDenominator;
		CTileMapSection *m_pMap;
	};

enum SpaceObjectGridFlags
	{
	gridNoBoxCheck			= 0x00000001,	//	Do not check for objects in the box
	gridNoCellBorder		= 0x00000002,	//	Do not add an extra border when checking
	};

class CSpaceObjectPool
	{
	public:
		struct SNode
			{
			//	No initializers to save on performance.

			CSpaceObject *pObj;
			SNode *pNext;
			};

		CSpaceObjectPool (void) { }
		~CSpaceObjectPool (void) { DeleteAll(); }
		CSpaceObjectPool (const CSpaceObjectPool &Src) = delete;
		CSpaceObjectPool &operator= (const CSpaceObjectPool &Src) = delete;

		SNode *AllocObj (SNode *pList, CSpaceObject *pObj);
		void DeleteAll (void);
		SNode *DeleteObj (SNode *pList, CSpaceObject *pObj, bool *retbDeleted = false);
		bool FindObj (SNode *pList, CSpaceObject *pObj) const;
		void Init (int iSize);

	private:
		SNode *m_Pool = NULL;
		int m_iSize = 0;
		int m_iNextNode = 0;
	};

class CSpaceObjectGrid
	{
	public:
		CSpaceObjectGrid (int iGridSize, Metric rCellSize, Metric rCellBorder);
		~CSpaceObjectGrid (void);

		void DebugObjDeleted (CSpaceObject *pObj) const;
		void Delete (CSpaceObject *pObj);
		void DeleteAll (void);
		void EnumStart (SSpaceObjectGridEnumerator &i, const CVector &vUR, const CVector &vLL, DWORD dwFlags) const;
		inline bool EnumHasMore (SSpaceObjectGridEnumerator &i) const;
		CSpaceObject *EnumGetNext (SSpaceObjectGridEnumerator &i) const;
		CSpaceObject *EnumGetNextFast (SSpaceObjectGridEnumerator &i) const;
		CSpaceObject *EnumGetNextInBoxPoint (SSpaceObjectGridEnumerator &i) const;
		void GetObjectsInBox (const CVector &vUR, const CVector &vLL, CSpaceObjectList &Result);
		void Init (CSystem *pSystem, SUpdateCtx &Ctx);

	private:
		struct SList
			{
			CSpaceObjectPool::SNode *pList;
			};

		void AddObject (CSpaceObject *pObj);
		bool EnumGetNextList (SSpaceObjectGridEnumerator &i) const;
		bool GetGridCoord (const CVector &vPos, int *retx, int *rety) const;
		const SList &GetList (const CVector &vPos) const;
		SList &GetList (const CVector &vPos);
		inline const SList &GetList (int x, int y) const { ASSERT(y * m_iGridSize + x < m_iGridSize * m_iGridSize); return m_pGrid[y * m_iGridSize + x]; }
		inline SList &GetList (int x, int y) { ASSERT(y * m_iGridSize + x < m_iGridSize * m_iGridSize); return m_pGrid[y * m_iGridSize + x]; }

		CSpaceObjectPool m_Pool;
		SList *m_pGrid;
		SList m_Outer;

		int m_iGridSize;
		CVector m_vGridSize;
		Metric m_rCellSize;
		Metric m_rCellBorder;
		CVector m_vLL;
		CVector m_vUR;

		friend struct SSpaceObjectGridEnumerator;
	};

struct SSpaceObjectGridEnumerator
	{
	SSpaceObjectGridEnumerator (void) : pGridIndexList(NULL) { }
	~SSpaceObjectGridEnumerator (void) { if (pGridIndexList) delete [] pGridIndexList; }

	CSpaceObject *pObj;						//	Current object
	int iGridIndex;							//	Current grid cell to search
	const CSpaceObjectGrid::SList *pList;						//	Current list
	const CSpaceObjectPool::SNode *pNode;						//	Current node
	bool bMore;								//	TRUE if there is more

	int iGridIndexCount;					//	Number of grid indices to traverse
	const CSpaceObjectGrid::SList **pGridIndexList;			//	Array of grid indices to traverse

	bool bCheckBox;							//	If TRUE, only return objects in box
	CVector vLL;							//	Box to check
	CVector vUR;
	};

class CGameTimeKeeper
	{
	public:
		void AddDiscontinuity (int iTick, const CTimeSpan &Duration);
		inline void DeleteAll (void) { m_Discontinuities.DeleteAll(); }
		CTimeSpan GetElapsedTimeAt (int iTick);
		void ReadFromStream (IReadStream *pStream);
		void WriteToStream (IWriteStream *pStream);

	private:
		struct SDiscontinuity
			{
			LONGLONG iDuration;
			int iTick;
			};

		TArray<SDiscontinuity> m_Discontinuities;
	};

//	Linked-list template class

template <class TYPE> class TSEListNode
	{
	public:
		TSEListNode (void) : m_pNext(NULL) { }
		~TSEListNode (void)
			{
			TYPE *pNext = GetNext();
			while (pNext)
				{
				TYPE *pDelete = pNext;
				pNext = pNext->GetNext();

				pDelete->m_pNext = NULL;
				delete pDelete;
				}
			}

		void FlushDeletedObjs (void)
			{
			TSEListNode<TYPE> *pPrev = this;
			TYPE *pNext = GetNext();
			while (pNext)
				{
				if (pNext->IsDeleted())
					{
					pPrev->m_pNext = pNext->GetNext();
					TYPE *pDelete = pNext;
					pNext = pNext->GetNext();

					pDelete->m_pNext = NULL;
					delete pDelete;
					}
				else
					{
					pPrev = pNext;
					pNext = pNext->GetNext();
					}
				}
			}

		int GetCount (void)
			{
			int iCount = 0;
			TSEListNode<TYPE> *pNext = m_pNext;
			while (pNext)
				{
				iCount++;
				pNext = pNext->m_pNext;
				}
			return iCount;
			}

		inline TYPE *GetNext (void) { return (TYPE *)m_pNext; }

		void Insert (TSEListNode<TYPE> *pNewNode)
			{
			pNewNode->m_pNext = m_pNext;
			m_pNext = pNewNode;
			}

		bool IsEmpty (void) const
			{
			return (m_pNext == NULL);
			}

		bool ObjDestroyed (CSpaceObject *pObj)
			{
			bool bDeleted = false;

			TYPE *pNext = GetNext();
			while (pNext)
				{
				if (pNext->OnObjDestroyed(pObj))
					bDeleted = true;

				pNext = pNext->GetNext();
				}

			return bDeleted;
			}

		void ReadFromStream (SLoadCtx &Ctx)
			{
			int i;
			DWORD dwCount;

			Ctx.pStream->Read((char *)&dwCount, sizeof(DWORD));
			TSEListNode<TYPE> *pInsertAt = this;
			for (i = 0; i < (int)dwCount; i++)
				{
				TYPE *pNew = new TYPE;
				pNew->OnReadFromStream(Ctx);
				pInsertAt->Insert(pNew);
				pInsertAt = pNew;
				}
			}

		void WriteToStream (CSystem *pSystem, IWriteStream *pStream)
			{
			DWORD dwCount = GetCount();
			pStream->Write((char *)&dwCount, sizeof(DWORD));
			TYPE *pNext = GetNext();
			while (pNext)
				{
				pNext->OnWriteToStream(pSystem, pStream);
				pNext = pNext->GetNext();
				}
			}

	private:
		TSEListNode<TYPE> *m_pNext;
	};

//	Regen class ----------------------------------------------------------------

class CRegenDesc
	{
	public:
		CRegenDesc (void) : m_bEmpty(true), m_iHPPerCycle(0), m_iHPPerEraRemainder(0), m_iCyclesPerBurst(1) { }
		CRegenDesc (int iHPPerEra);

		void Add (const CRegenDesc &Desc);
		double GetHPPer180 (int iTicksPerCycle = 1) const;
		int GetHPPerEra (void) const;
		int GetRegen (int iTick, int iTicksPerCycle = 1) const;
		void Init (int iHPPerEra, int iCyclesPerBurst = 1);
		void InitFromRegen (double rRegen, int iTicksPerCycle = 1);
		ALERROR InitFromRegenString (SDesignLoadCtx &Ctx, const CString &sRegen, int iTicksPerCycle = 1);
		ALERROR InitFromRegenTimeAndHP (SDesignLoadCtx &Ctx, int iRegenTime, int iRegenHP, int iTicksPerCycle = 1);
		ALERROR InitFromRepairRateString (SDesignLoadCtx &Ctx, const CString &sRepairRate, int iTicksPerCycle = 1);
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, 
							 CXMLElement *pDesc, 
							 const CString &sRegenAttrib, 
							 const CString &sRegenRate,
							 const CString &sRegenHP,
							 int iTicksPerCycle = 1);
		inline bool IsEmpty (void) const { return m_bEmpty; }

	private:
		int m_iHPPerCycle;					//	HP gained per cycle
		int m_iHPPerEraRemainder;			//	Extra HP to gain per era (1 era = 360 cycles)

		int m_iCyclesPerBurst;				//	Regen in bursts; each burst is this many cycles

		bool m_bEmpty;						//	If TRUE, no regen
	};

//	CZoneGrid ------------------------------------------------------------------

class CZoneGrid
	{
	public:
		CZoneGrid (void) : m_cxSize(-1), m_cySize(-1), m_iCellSize(-1), m_pDesc(NULL) { }

		ALERROR CreateFromXML (CXMLElement *pDesc, int cxWidth, int cyHeight, int iCellSize);
		bool ChooseRandomPoint (int *retx, int *rety) const;
		ALERROR CreateZone (int cxSize = -1, int cySize = -1, int iCellSize = 1);
		inline const CString &GetName (void) const { return m_sName; }
		int GetValue (int x, int y) const;
		ALERROR LoadFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);

	private:
		ALERROR ApplyZoneOperation (CXMLElement *pOp, CG8bitImage &DestMap);
		ALERROR CreateZoneMap (CG8bitImage &ZoneMap);
		int GetValueRaw (int x, int y) const;
		void MapCoord (int x, int y, int *retx, int *rety) const;
		inline int MapDim (int x) const { return x / m_iCellSize; }
		ALERROR ZoneOpCircle (CXMLElement *pOp, CG8bitImage &DestMap);
		ALERROR ZoneOpCircleGradient (CXMLElement *pOp, CG8bitImage &DestMap);
		ALERROR ZoneOpMultiply (CXMLElement *pOp, CG8bitImage &DestMap);
		ALERROR ZoneOpNoise (CXMLElement *pOp, CG8bitImage &DestMap);

		CString m_sName;
		int m_cxSize;						//	Size of grid in virtual units
		int m_cySize;
		int m_iCellSize;					//	Cell size in virtual units
											//		(1 cell = 1 pixel in m_ZoneMap)

		CG8bitImage m_ZoneMap;				//	Alpha channel used as a BYTE grid

		CXMLElement *m_pDesc;
	};

//	CBitGrid -------------------------------------------------------------------

class CBitGrid
	{
	public:
		CBitGrid (int cxWidth, int cyHeight) : m_cxWidth(cxWidth), m_cyHeight(cyHeight), m_Grid(cxWidth * cyHeight)
			{ }

		inline void Clear (int x, int y) { if (InBounds(x, y)) m_Grid.Clear(Coord(x, y)); }
		inline bool InBounds (int x, int y)
			{
			int xGrid = (x + m_cxWidth / 2);
			int yGrid = (m_cyHeight / 2 - y);
			return (xGrid >= 0 && xGrid < m_cxWidth && yGrid >= 0 && yGrid < m_cyHeight);
			}
		inline bool IsSet (int x, int y) { return m_Grid.IsSet(Coord(x, y)); }
		inline void Set (int x, int y) { if (InBounds(x, y)) m_Grid.Set(Coord(x, y)); }

	private:
		inline DWORD Coord (int x, int y) { return ((m_cyHeight / 2 - y) * m_cxWidth) + (x + m_cxWidth / 2); }

		int m_cxWidth;
		int m_cyHeight;
		CLargeSet m_Grid;
	};

//	C2DFunction ----------------------------------------------------------------

class I2DFunction
	{
	public:
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, I2DFunction **retpFunc);

		virtual ~I2DFunction (void) { }

		inline Metric Eval (Metric x, Metric y) { return OnEval(x, y); }
		inline ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) { return OnInitFromXML(Ctx, pDesc); }

	protected:
		virtual Metric OnEval (Metric x, Metric y) { return 0.0f; }
		virtual ALERROR OnInitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) { return NOERROR; }
	};

class CNoise2DFunc : public I2DFunction
	{
	protected:
		virtual Metric OnEval (Metric x, Metric y);
		virtual ALERROR OnInitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);

	private:
		int m_iScale;
	};

//	CComplexArea ---------------------------------------------------------------

class CComplexArea
	{
	public:
		CComplexArea (void);

		inline void ExcludeCircle (int x, int y, int iRadius) { AddCircle(m_ExcludedCircles, x, y, iRadius); }
		inline void ExcludeRect (int x, int y, int cxWidth, int cyHeight, int iRotation = 0) { AddRect(m_ExcludedRects, x, y, cxWidth, cyHeight, iRotation); }
		bool GeneratePointsInArea (int iCount, int iMinSeparation, TArray<int> *retX, TArray<int> *retY);
		bool GeneratePointsInArea (int iCount, int iMinSeparation, CIntGraph *retGraph);
		bool InArea (int x, int y);
		inline void IncludeCircle (int x, int y, int iRadius) { AddCircle(m_IncludedCircles, x, y, iRadius); }
		inline void IncludeRect (int x, int y, int cxWidth, int cyHeight, int iRotation = 0) { AddRect(m_IncludedRects, x, y, cxWidth, cyHeight, iRotation); }
		void Paint (CG32bitImage &Dest, int xCenter, int yCenter, Metric rScale = 1.0) const;
		bool RandomPointInArea (int *retx, int *rety);
		void ReadFromStream (IReadStream &Stream);
		void WriteToStream (IWriteStream &Stream) const;

	private:
		struct SCircle
			{
			int x;
			int y;
			int iRadius2;
			};

		struct SRect
			{
			int x;
			int y;
			int cxWidth;
			int cyHeight;
			int iRotation;
			};

		void AddCircle (TArray<SCircle> &Array, int x, int y, int iRadius);
		void AddRect (TArray<SRect> &Array, int x, int y, int cxWidth, int cyHeight, int iRotation);
		void AddToBounds (int xLeft, int yTop, int xRight, int yBottom);
		bool InCircle (SCircle &Circle, int x, int y);
		bool InRect (SRect &Rect, int x, int y);
		void PaintCircle (CG32bitImage &Dest, int xCenter, int yCenter, Metric rScale, const SCircle &Circle, CG32bitPixel rgbColor) const;
		void PaintRect (CG32bitImage &Dest, int xCenter, int yCenter, Metric rScale, const SRect &Rect, CG32bitPixel rgbColor) const;
		void ReadCircleArray (IReadStream &Stream, TArray<SCircle> &Result);
		void ReadRectArray (IReadStream &Stream, TArray<SRect> &Result);
		void WriteCircleArray (IWriteStream &Stream, const TArray<SCircle> &Array) const;
		void WriteRectArray (IWriteStream &Stream, const TArray<SRect> &Array) const;

		inline static int XToImageCoords (int xCenter, int x, Metric rScale) { return xCenter + (int)(x * rScale); }
		inline static int YToImageCoords (int yCenter, int y, Metric rScale) { return yCenter - (int)(y * rScale); }

		TArray<SCircle> m_ExcludedCircles;
		TArray<SRect> m_ExcludedRects;
		TArray<SCircle> m_IncludedCircles;
		TArray<SRect> m_IncludedRects;

		RECT m_rcBounds;
	};

//	C3DConversion --------------------------------------------------------------

class C3DObjectPos
	{
	public:
		static constexpr DWORD FLAG_NO_XY =				0x00000001;
		static constexpr DWORD FLAG_CALC_POLAR =		0x00000002;	//	Convert X,Y to polar using C3DConversion

		C3DObjectPos (int iAngle = 0, int iRadius = 0, int iZ = 0) :
				m_iPosAngle(iAngle),
				m_iPosRadius(iRadius),
				m_iPosZ(iZ)
			{ }

		explicit C3DObjectPos (const CVector &vPos, int iZ = 0);

		void CalcCoord (int iScale, CVector *retvPos) const;
		void CalcCoord (int iScale, int iRotation, CVector *retvPos) const;
		inline int GetAngle (void) const { return m_iPosAngle; }
		inline int GetRadius (void) const { return m_iPosRadius; }
		inline int GetZ (void) const { return m_iPosZ; }
		bool InitFromXY (int iScale, const CVector &vPos, int iZ = 0);
		bool InitFromXML (CXMLElement *pDesc, DWORD dwFlags = 0, bool *retb3DPos = NULL);
		inline bool IsEmpty (void) const { return m_iPosRadius == 0; }
		void ReadFromStream (SLoadCtx &Ctx);
		inline void SetAngle (int iAngle) { m_iPosAngle = AngleMod(iAngle); }
		void WriteToStream (IWriteStream &Stream) const;

	private:
		void InitPosZFromXML (CXMLElement *pDesc, bool *retb3DPos = NULL);

		int m_iPosAngle:16;					//	Angle relative to obj center (degrees)
		int m_iPosRadius:16;				//	Distance relative to obj center (pixels)
		int m_iPosZ:16;						//	Height relative to obj center
	};

class C3DConversion
	{
	public:
		static constexpr int DEFAULT_SCALE = 256;

		static void CalcCoord (int iScale, int iAngle, int iRadius, int iZ, int *retx, int *rety);
		static void CalcCoord (int iScale, int iAngle, int iRadius, int iZ, CVector *retvPos);
		static void CalcCoord (Metric rScale, const CVector &vPos, Metric rPosZ, CVector *retvPos);
		static void CalcCoordCompatible (int iAngle, int iRadius, int *retx, int *rety);
		static void CalcPolar (int iScale, const CVector &vPos, int iZ, Metric *retrAngle, Metric *retrRadius);

		inline void CleanUp (void) { m_Cache.DeleteAll(); }
		ALERROR Init (CXMLElement *pDesc);
		ALERROR Init (CXMLElement *pDesc, int iDirectionCount, int iScale, int iFacing);
		void Init (int iDirectionCount, int iScale, int iAngle, int iRadius, int iZ, int iFacing);
		void InitCompatible (int iDirectionCount, int iAngle, int iRadius, int iFacing);
		void InitCompatibleXY (int iDirectionCount, int iX, int iY, int iFacing);
		void InitComplete (int iDirectionCount, int iScale, int iFacing);
		void InitXY (int iDirectionCount, int iScale, int iX, int iY, int iZ, int iFacing);
		inline bool IsEmpty (void) const { return (m_Cache.GetCount() == 0); }
		void GetCoord (int iRotation, int *retx, int *rety) const;
		void GetCoordFromDir (int iDirection, int *retx, int *rety) const;
		inline bool PaintFirst (int iDirection) const { return m_Cache[iDirection].bPaintFirst; }

	private:
		struct SEntry
			{
			int x;
			int y;
			bool bPaintFirst;
			};

		bool InitCache (int iDirectionCount);
		ALERROR OverridePaintFirst (int iDirectionCount, const CString &sAttrib, bool bPaintFirstValue);

		int m_iAngle;
		int m_iRadius;
		int m_iZ;
		bool m_bUseCompatible;
		CString m_sBringToFront;
		CString m_sSendToBack;

		TArray<SEntry> m_Cache;
	};

//	Wave Generators ------------------------------------------------------------

class CWaveGenerator2
	{
	public:
		CWaveGenerator2 (Metric rWave0Amp, Metric rWave0Cycles, Metric rWave1Amp, Metric rWave1Cycles, bool b0to1 = true);

		Metric GetValue (Metric rAngle);

	private:
		Metric m_rWave0Amp;
		Metric m_rWave0Cycles;
		Metric m_rWave1Amp;
		Metric m_rWave1Cycles;

		Metric m_rOffset;
	};

//	Local device storage class -------------------------------------------------

class CDeviceStorage
	{
	public:
		CDeviceStorage (void) : m_bModified(false) { }

		const CString &GetData (DWORD dwExtension, const CString &sAttrib) const;
		bool FindData (DWORD dwExtension, const CString &sAttrib, CString *retsData = NULL) const;
		ALERROR Load (const CString &sFilespec, CString *retsError = NULL);
		ALERROR Save (const CString &sFilespec);
		bool SetData (DWORD dwExtension, const CString &sAttrib, const CString &sData);

	private:
		CString MakeKey (DWORD dwExtension, const CString &sAttrib) const;
		bool ValidateAndMakeKey (DWORD dwExtension, const CString &sAttrib, CString *retsKey = NULL) const;

		TSortMap<CString, CString> m_Storage;
		bool m_bModified;
	};

//	IListData ------------------------------------------------------------------

extern const CItem g_DummyItem;
extern CItemListManipulator g_DummyItemListManipulator;

class IListData
	{
	public:
		virtual ~IListData (void) { }
		virtual void DeleteAtCursor (int iCount) { }
		virtual bool FindItem (const CItem &Item, int *retiCursor = NULL) { return false; }
		virtual int GetCount (void) const { return 0; }
		virtual int GetCursor (void) { return -1; }
		virtual CString GetDescAtCursor (void) { return NULL_STR; }
		virtual ICCItem *GetEntryAtCursor (void) { return CCodeChain::CreateNil(); }
		virtual const CItem &GetItemAtCursor (void) { return g_DummyItem; }
		virtual CItemListManipulator &GetItemListManipulator (void) { return g_DummyItemListManipulator; }
		virtual CSpaceObject *GetSource (void) { return NULL; }
		virtual CString GetTitleAtCursor (void) { return NULL_STR; }
		virtual bool IsCursorValid (void) const { return false; }
		virtual bool MoveCursorBack (void) { return false; }
		virtual bool MoveCursorForward (void) { return false; }
		virtual void PaintImageAtCursor (CG32bitImage &Dest, int x, int y, int cxWidth, int cyHeight, Metric rScale) { }
		virtual void ResetCursor (void) { }
		virtual void SetCursor (int iCursor) { }
		virtual void SetFilter (const CItemCriteria &Filter) { }
		virtual void SyncCursor (void) { }
	};

//	CFormulaText ---------------------------------------------------------------

class CFormulaText
	{
	public:
		int EvalAsInteger (CSpaceObject *pSource, CString *retsPrefix = NULL, CString *retsSuffix = NULL, CString *retsError = NULL) const;
		inline void InitFromInteger (int iValue) { m_sText = strFromInt(iValue); }
		ALERROR InitFromString (SDesignLoadCtx &Ctx, const CString &sText);
		inline bool IsEmpty (void) const { return m_sText.IsBlank(); }
		void ReadFromStream (SLoadCtx &Ctx);
		inline void SetInteger (int iValue) { m_sText = strFromInt(iValue); }
		void WriteToStream (IWriteStream *pStream) const;

	private:
		CString m_sText;
	};

//	Miscellaneous utility functions

CString AppendModifiers (const CString &sModifierList1, const CString &sModifierList2);
CString GetLoadStateString (ELoadStates iState);
Metric GetScale (CXMLElement *pObj);
bool HasModifier (const CString &sModifierList, const CString &sModifier);
inline bool IsOfficialUNID (DWORD dwUNID) { return ((dwUNID & 0xF0000000) < 0xA0000000); }
inline bool IsRegisteredUNID (DWORD dwUNID) { return ((dwUNID & 0xF0000000) != 0xD0000000) && ((dwUNID & 0xF0000000) != 0xE0000000); }
inline bool IsReservedUNID (DWORD dwUNID) { return ((dwUNID & 0xF0000000) == 0xF0000000); }

ALERROR ParseDamageTypeList (const CString &sList, TArray<CString> *retList);
void ParseKeyValuePair (const CString &sString, DWORD dwFlags, CString *retsKey, CString *retsValue);

const DWORD PSL_FLAG_ALLOW_WHITESPACE =		0x00000001;
const DWORD PUL_FLAG_HEX =					0x00000002;
void ParseStringList (const CString &sList, DWORD dwFlags, TArray<CString> *retList);
inline void ParseAttributes (const CString &sAttribs, TArray<CString> *retAttribs) { ParseStringList(sAttribs, 0, retAttribs); }
void ParseIntegerList (const CString &sList, DWORD dwFlags, TArray<int> *retList);

void ParseUNIDList (const CString &sList, DWORD dwFlags, TArray<DWORD> *retList);

inline bool CSpaceObjectGrid::EnumHasMore (SSpaceObjectGridEnumerator &i) const { return i.bMore; }

#ifdef LEVEL_ROMAN_NUMERALS
inline CString strLevel (int iLevel) { return strRomanNumeral(iLevel); }
#else
inline CString strLevel (int iLevel) { return strFromInt(iLevel); }
#endif
