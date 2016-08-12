//	TSE.h
//
//	Transcendence Space Engine

#ifndef INCL_TSE
#define INCL_TSE

#ifndef INCL_EUCLID
#include "Euclid.h"
#endif

#ifndef INCL_LINGUA
#include "Lingua.h"
#endif

#ifndef INCL_INTERNETS
#include "Internets.h"
#endif

#ifndef INCL_CRYPTO
#include "Crypto.h"
#endif

#ifndef INCL_JPEGUTIL
#include "JPEGUtil.h"
#endif

#ifndef INCL_XMLUTIL
#include "XMLUtil.h"
#endif

#ifndef INCL_JSONUTIL
#include "JSONUtil.h"
#endif

#ifndef INCL_DIRECTXUTIL
#include "DirectXUtil.h"
#endif

#ifndef INCL_REANIMATOR
#include "Reanimator.h"
#endif

#ifndef INCL_TSE_GEOMETRY
#include "TSEGeometry.h"
#endif

//  Starting with Part II we use Arabic numerals for levels.

//#define LEVEL_ROMAN_NUMERALS

//	Uncomment out the following define when building a stable release

//#define TRANSCENDENCE_STABLE_RELEASE

//	Define some debugging symbols

#ifdef DEBUG
//#define DEBUG_ALL_ITEMS
//#define DEBUG_ALL_NODES
//#define DEBUG_ASTAR_PATH
//#define DEBUG_ASTAR_PERF
//#define DEBUG_ATTACK_TARGET
//#define DEBUG_AVOID_POTENTIAL
//#define DEBUG_BOUNDING_RECT
//#define DEBUG_COMBAT
//#define DEBUG_COMBAT_AI
//#define DEBUG_DOCK_PORT_POS
//#define DEBUG_ENCOUNTER_COUNTS
//#define DEBUG_FIRE_ON_OPPORTUNITY
//#define DEBUG_FORMATION
//#define DEBUG_HENCHMAN
//#define DEBUG_LOAD
//#define DEBUG_NAV_PATH
//#define DEBUG_NEBULA_PAINTING
//#define DEBUG_PERFORMANCE
//#define DEBUG_PROGRAM_UPGRADE
//#define DEBUG_RANDOM_SEED
//#define DEBUG_SHIP
//#define DEBUG_SINGLETON_EFFECTS
//#define DEBUG_SOUNDTRACK
//#define DEBUG_SOUNDTRACK_STATE
//#define DEBUG_SOURCE_LOAD_TRACE
//#define DEBUG_STATION_EXCLUSION_ZONE
//#define DEBUG_TARGET_LIST
#define DEBUG_VECTOR
//#define DEBUG_WEAPON_POS
#endif

//	We leave this defined because we want to get traces in the field in case
//	of a crash.
#define DEBUG_PROGRAMSTATE

//	Debugging help

#define DEBUG_TRY					try {
#define DEBUG_CATCH					} catch (...) { kernelDebugLogMessage("Crash in %s", CString(__FUNCTION__)); throw; }
#define DEBUG_CATCH_CONTINUE		} catch (...) { kernelDebugLogMessage("Crash in %s", CString(__FUNCTION__)); }
#define DEBUG_CATCH_MSG(msg)		} catch (...) { kernelDebugLogMessage((msg)); throw; }
#define DEBUG_CATCH_MSG1(msg,p1)	} catch (...) { kernelDebugLogMessage((msg),(p1)); throw; }

#define DEBUG_TRY_OBJ_LOOP		CSpaceObject *pObj = NULL; try {
#define DEBUG_CATCH_OBJ_LOOP	} catch (...) \
		{ \
		kernelDebugLogMessage("Crash in %s", CString(__FUNCTION__)); \
		\
		if (pObj = NULL) \
			kernelDebugLogMessage("pObj = NULL"); \
		else \
			{ \
			CString sError; \
			ReportCrashObj(&sError, pObj); \
			kernelDebugLogMessage("pObj = "); \
			kernelDebugLogMessage(sError); \
			} \
		\
		throw; \
		}

//	If ITEM_REFERENCE is defined, then the player doesn't see the
//	stats for an item until they install it (or get reference info)
//#define ITEM_REFERENCE

//	Forward references

class CArmorClass;
class CBoundaryMarker;
class CDeviceClass;
class CDockScreenType;
class CEffectCreator;
class COverlay;
class COverlayType;
class CInstalledDevice;
class CItem;
class CItemListManipulator;
class CItemTable;
class CItemType;
class CMarker;
class CMissile;
class CMission;
class CParticleEffect;
class CPower;
class CRandomEntryResults;
class CResourceDb;
class CShip;
class CShipClass;
class CShipTable;
class CSovereign;
class CSpaceEnvironmentType;
class CSpaceObject;
class CStation;
class CStationType;
class CSystem;
class CTimedEvent;
class CTopologyNode;
class CTradingDesc;
class CUniverse;
class CVector;
class CWeaponClass;
class CWeaponFireDesc;
class IEffectPainter;
class IShipController;
class IShipGenerator;
struct SLocationCriteria;

extern CUniverse *g_pUniverse;

//	Include other TSE files

#ifndef INCL_TSE_TYPES
#include "TSETypes.h"
#endif

#ifndef INCL_TSE_UTIL
#include "TSEUtil.h"
#endif

#ifndef INCL_TSE_TRANS_LISP
#include "TSETransLisp.h"
#endif

#ifndef INCL_TSE_DESIGN
#include "TSEDesign.h"
#endif

#include "TSEStorage.h"

#ifndef INCL_TSE_MULTIVERSE
#include "TSEMultiverse.h"
#endif

#ifndef INCL_TSE_PAINT
#include "TSEPaint.h"
#endif

//	CResourceDb

class CResourceDb
	{
	public:
		CResourceDb (const CString &sDefFilespec, bool bExtension = false);
		~CResourceDb (void);

		void ComputeFileDigest (CIntegerIP *retDigest);
		IXMLParserController *GetEntities (void) const { return m_pEntities; }
		IXMLParserController *GetEntitiesHandoff (bool *retbFree) { IXMLParserController *pEntities = m_pEntities; *retbFree = m_bFreeEntities; m_pEntities = NULL; m_bFreeEntities = false; return pEntities; }
		const CString &GetFilespec (void) const { return m_sFilespec; }
		CString GetRootTag (void);
		bool ImageExists (const CString &sFolder, const CString &sFilename);
		inline bool IsTDB (void) const { return (m_pDb != NULL); }
		bool IsUsingExternalGameFile (void) const { return !m_bGameFileInDb; }
		bool IsUsingExternalResources (void) const { return !m_bResourcesInDb; }
		ALERROR LoadEmbeddedGameFile (const CString &sFilename, CXMLElement **retpData, IXMLParserController *pResolver, CExternalEntityTable *ioEntityTable, CString *retsError);
		ALERROR LoadEntities (CString *retsError, CExternalEntityTable **retEntities = NULL);
		ALERROR LoadGameFile (CXMLElement **retpData, IXMLParserController *pEntities, CString *retsError, CExternalEntityTable *ioEntityTable = NULL);
		ALERROR LoadGameFileStub (CXMLElement **retpData, CExternalEntityTable *ioEntityTable, CString *retsError);
		ALERROR LoadImage (const CString &sFolder, const CString &sFilename, HBITMAP *rethImage, EBitmapTypes *retiImageType = NULL);
		ALERROR LoadModule (const CString &sFolder, const CString &sFilename, CXMLElement **retpData, CString *retsError);
		ALERROR LoadModuleEntities (const CString &sFolder, const CString &sFilename, CExternalEntityTable **retpEntities, CString *retsError);
		ALERROR LoadSound (CSoundMgr &SoundMgr, const CString &sFolder, const CString &sFilename, int *retiChannel);
		ALERROR Open (DWORD dwFlags, CString *retsError);
		CString ResolveFilespec (const CString &sFolder, const CString &sFilename) const;
		void SetEntities (IXMLParserController *pEntities, bool bFree = false);

		CString GetResourceFilespec (int iIndex);
		int GetResourceCount (void);
		inline int GetVersion (void) { return m_iVersion; }
		ALERROR ExtractMain (CString *retsData);
		ALERROR ExtractResource (const CString sFilespec, CString *retsData);

	private:
		enum EFlags
			{
			//	SResourceEntry flags
			FLAG_COMPRESS_ZLIB =		0x00000001,
			};

		struct SResourceEntry
			{
			CString sFilename;
			int iEntryID;
			DWORD dwFlags;
			};

		ALERROR OpenDb (void);
		ALERROR ReadEntry (const CString &sFilespec, CString *retsData);

		int m_iVersion;
		bool m_bGameFileInDb;
		bool m_bResourcesInDb;

		//	If we're just using the file system
		CString m_sFilespec;
		CString m_sRoot;
		CString m_sGameFile;

		//	If we're using a TDB
		CDataFile *m_pDb;
		TSortMap<CString, SResourceEntry> m_ResourceMap;
		int m_iGameFile;

		IXMLParserController *m_pEntities;			//	Entities to use in parsing
		bool m_bFreeEntities;						//	If TRUE, we own m_pEntities;
	};

class CAStarPathFinder
	{
	public:
		CAStarPathFinder (void);
		~CAStarPathFinder (void);

		void AddObstacle (const CVector &vUR, const CVector &vLL);
		int FindPath (const CVector &vStart, const CVector &vEnd, CVector **retPathList, bool bTryReverse = true);

	private:
		struct SObstacle
			{
			CVector vUR;
			CVector vLL;
			};

		struct SNode
			{
			int x;
			int y;

			CVector vPos;
			int iCostFromStart;
			int iHeuristic;
			int iTotalCost;

			SNode *pParent;
			SNode *pPrev;
			SNode *pNext;
			};

		typedef SNode *SNodeRoot;

		void AddToClosedList (SNode *pNew);
		void AddToOpenList (SNode *pNew);
		int CalcHeuristic (const CVector &vPos, const CVector &vDest);
		void CollapsePath (TArray<SNode *> &Path, int iStart, int iEnd);
		void CreateInOpenList (const CVector &vEnd, SNode *pCurrent, int xDir, int yDir);
		bool IsInClosedList (int x, int y);
		bool IsPathClear (const CVector &vStart, const CVector &vEnd);
		bool IsPointClear (const CVector &vPos);
		bool LineIntersectsRect (const CVector &vStart, const CVector &vEnd, const CVector &vUR, const CVector &vLL);
		int OptimizePath (const CVector &vEnd, SNode *pFinal, CVector **retPathList);
		void Reset (void);

		void AddToList (SNodeRoot &pRoot, SNode *pNew, SNode *pAddAfter = NULL);
		void DeleteFromList (SNodeRoot &pRoot, SNode *pDelete);
		void DeleteList (SNodeRoot &pRoot);

		TArray<SObstacle> m_Obstacles;
		SNodeRoot m_pOpenList;
		SNodeRoot m_pClosedList;
		CTileMap *m_pClosedMap;

#ifdef DEBUG_ASTAR_PERF
		int m_iCallsToIsPathClear;
		int m_iClosedListCount;
		int m_iOpenListCount;
#endif
	};

//	Paint Utilities

void ComputeLightningPoints (int iCount, CVector *pPoints, Metric rChaos);
void CreateBlasterShape (int iAngle, int iLength, int iWidth, SPoint *Poly);
void DrawItemTypeIcon (CG32bitImage &Dest, int x, int y, CItemType *pType, int cxSize = 0, int cySize = 0, bool bGray = false);
void DrawLightning (CG32bitImage &Dest,
					int xFrom, int yFrom,
					int xTo, int yTo,
					CG32bitPixel rgbColor,
					int iPoints,
					Metric rChaos);
void DrawLightning (CG32bitImage &Dest,
					int xFrom, int yFrom,
					int xTo, int yTo,
					CG32bitPixel rgbFrom,
					CG32bitPixel rgbTo,
					Metric rChaos);
void DrawParticle (CG32bitImage &Dest,
				   int x, int y,
				   CG32bitPixel rgbColor,
				   int iSize,
				   DWORD byOpacity);

//	Helper functions

CString ComposeDamageAdjReference (int *AdjRow, int *StdRow);
CString ComposeNounPhrase (const CString &sNoun, 
						   int iCount, 
						   const CString &sModifier, 
						   DWORD dwNounFlags, 
						   DWORD dwComposeFlags);
void ComputePercentages (int iCount, int *pTable);
CString ParseNounForm (const CString &sNoun, DWORD dwNounFlags, bool bPluralize, bool bShortName);

#define VISUAL_RANGE_INDEX		6
#define RANGE_INDEX_COUNT		9
Metric RangeIndex2Range (int iIndex);

CString ReactorPower2String (int iPower);

int NLCompare (TArray<CString> &Input, TArray<CString> &Pattern);
void NLSplit (const CString &sPhrase, TArray<CString> *retArray);

void ReportCrashObj (CString *retsMessage, CSpaceObject *pCrashObj = NULL);

struct SExplosionType
	{
	CWeaponFireDesc *pDesc;				//	Explosion type
	int iBonus;							//	Bonus damage
	DestructionTypes iCause;			//	Cause
	};

class CWeaponFireDesc
	{
	public:
		enum ECachedHandlers
			{
			evtOnCreateShot				= 0,
			evtOnDamageAbandoned		= 1,
			evtOnDamageArmor			= 2,
			evtOnDamageOverlay			= 3,
			evtOnDamageShields			= 4,
			evtOnDestroyShot			= 5,
			evtOnFragment				= 6,

			evtCount					= 7,
			};

		struct SFragmentDesc
			{
			CWeaponFireDesc *pDesc;			//	Data for fragments
			DiceRange Count;				//	Number of fragments
			bool bMIRV;						//	Fragments seek independent targets

			SFragmentDesc *pNext;
			};

        struct SExhaustDesc
            {
            SExhaustDesc (void) :
                    iExhaustRate(0),
                    iExhaustLifetime(0),
                    rExhaustDrag(0.0)
                { }

		    int iExhaustRate;				//	Ticks per exhaust creation (0 if no exhaust)
		    CObjectImageArray ExhaustImage; //	Image for exhaust
		    int iExhaustLifetime;			//	Ticks that each exhaust particle lasts
		    Metric rExhaustDrag;			//	Coefficient of drag for exhaust particles
            };

        struct SVaporTrailDesc
            {
            SVaporTrailDesc (void) :
                    iVaporTrailWidth(0),
                    iVaporTrailLength(0),
                    iVaporTrailWidthInc(0),
                    rgbVaporTrailColor(CG32bitPixel::Null())
                { }

		    int iVaporTrailWidth;			//	Width of vapor trail (0 = none)
		    int iVaporTrailLength;			//	Number of segments
		    int iVaporTrailWidthInc;		//	Width increment in 100ths of a pixel
		    CG32bitPixel rgbVaporTrailColor;//	Color of vapor trail
            };

		CWeaponFireDesc (void);
		~CWeaponFireDesc (void);

		void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed);
        void ApplyAcceleration (CSpaceObject *pMissile) const;
		inline bool CanAutoTarget (void) const { return (m_fAutoTarget ? true : false); }
        inline bool CanDamageSource (void) const { return (m_fCanDamageSource ? true : false); }
		bool CanHit (CSpaceObject *pObj) const;
		inline bool CanHitFriends (void) const { return !m_fNoFriendlyFire; }
		IEffectPainter *CreateEffectPainter (bool bTrackingObj = false, bool bUseObjectCenter = false);
		void CreateFireEffect (CSystem *pSystem, CSpaceObject *pSource, const CVector &vPos, const CVector &vVel, int iDir);
		void CreateHitEffect (CSystem *pSystem, SDamageCtx &DamageCtx);
		IEffectPainter *CreateParticlePainter (void);
		IEffectPainter *CreateSecondaryPainter (bool bTrackingObj = false, bool bUseObjectCenter = false);
		IEffectPainter *CreateShockwavePainter (bool bTrackingObj = false, bool bUseObjectCenter = false);
		bool FindDataField (const CString &sField, CString *retsValue) const;
		CEffectCreator *FindEffectCreator (const CString &sUNID);
		bool FindEventHandler (const CString &sEvent, SEventHandlerDesc *retEvent = NULL) const;
		inline bool FindEventHandler (ECachedHandlers iEvent, SEventHandlerDesc *retEvent = NULL) const { if (retEvent) *retEvent = m_CachedEvents[iEvent]; return (m_CachedEvents[iEvent].pCode != NULL); }
		ICCItem *FindProperty (const CString &sProperty) const;
		CWeaponFireDesc *FindWeaponFireDesc (const CString &sUNID, char **retpPos = NULL);
		static CWeaponFireDesc *FindWeaponFireDescFromFullUNID (const CString &sUNID);
        ALERROR FinishBindDesign (SDesignLoadCtx &Ctx);
		void FireOnCreateShot (const CDamageSource &Source, CSpaceObject *pShot, CSpaceObject *pTarget);
		bool FireOnDamageAbandoned (SDamageCtx &Ctx);
		bool FireOnDamageArmor (SDamageCtx &Ctx);
		bool FireOnDamageOverlay (SDamageCtx &Ctx, COverlay *pOverlay);
		bool FireOnDamageShields (SDamageCtx &Ctx, int iDevice);
		bool FireOnFragment (const CDamageSource &Source, CSpaceObject *pShot, const CVector &vHitPos, CSpaceObject *pNearestObj, CSpaceObject *pTarget);
		void FireOnDestroyShot (CSpaceObject *pShot);
		inline CItemType *GetAmmoType (void) const { return m_pAmmoType; }
        inline DWORD GetAmmoTypeUNID (void) const { return m_pAmmoType.GetUNID(); }
		inline int GetAreaDamageDensity (void) const { return m_AreaDamageDensity.Roll(); }
		inline Metric GetAreaDamageDensityAverage (void) const { return m_AreaDamageDensity.GetAveValueFloat(); }
		inline Metric GetAveDamage (void) const { return m_Damage.GetDamageValue(); }
		inline Metric GetAveExpansionSpeed (void) const { return (m_ExpansionSpeed.GetAveValue() * LIGHT_SPEED / 100.0); }
		Metric GetAveInitialSpeed (void) const;
		inline int GetAveLifetime (void) const { return m_Lifetime.GetAveValue(); }
		Metric GetAveParticleCount (void) const;
        Metric GetAveSpeed (void) const { return 0.5 * (GetRatedSpeed() + m_rMaxMissileSpeed); }
        inline int GetContinuous (void) const { return m_iContinuous; }
        const DamageDesc &GetDamage (void) const { return m_Damage; }
		DamageTypes GetDamageType (void) const;
		inline CEffectCreator *GetEffect (void) const { return m_pEffect; }
        inline Metric GetEffectiveRange (void) const { return m_rMaxEffectiveRange; }
		inline ICCItem *GetEventHandler (const CString &sEvent) const { SEventHandlerDesc Event; if (!FindEventHandler(sEvent, &Event)) return NULL; return Event.pCode; }
        inline const SExhaustDesc &GetExhaust (void) const { return GetOldEffects().Exhaust; }
		inline Metric GetExpansionSpeed (void) const { return (m_ExpansionSpeed.Roll() * LIGHT_SPEED / 100.0); }
		inline CExtension *GetExtension (void) const { return m_pExtension; }
		inline FireTypes GetFireType (void) const { return m_iFireType; }
		inline SFragmentDesc *GetFirstFragment (void) const { return m_pFirstFragment; }
		inline int GetHitPoints (void) const { return m_iHitPoints; }
        inline const CObjectImageArray &GetImage (void) const { return GetOldEffects().Image; }
		inline int GetInitialDelay (void) const { return m_InitialDelay.Roll(); }
		Metric GetInitialSpeed (void) const;
		inline int GetInteraction (void) const { return m_iInteraction; }
        inline int GetLevel (void) const { return Max(m_iLevel, (m_pAmmoType != NULL ? m_pAmmoType->GetLevel() : 0)); }
		inline int GetLifetime (void) const { return m_Lifetime.Roll(); }
		inline int GetManeuverRate (void) const { return m_iManeuverRate; }
		inline int GetMaxLifetime (void) const { return m_Lifetime.GetMaxValue(); }
		inline Metric GetMaxRadius (void) const { return m_rMaxRadius; }
		inline int GetMinDamage (void) const { return m_MinDamage.Roll(); }
		inline Metric GetMinRadius (void) const { return m_rMinRadius; }
        Metric GetMaxRange (void) const;
		CEffectCreator *GetParticleEffect (void) const;
		inline const CParticleSystemDesc *GetParticleSystemDesc (void) const { return m_pParticleDesc; }
		inline int GetPassthrough (void) const { return m_iPassthrough; }
		inline int GetProximityFailsafe (void) const { return m_iProximityFailsafe; }
		inline Metric GetRatedSpeed (void) const { return m_rMissileSpeed; }
        CWeaponFireDesc *GetScaledDesc (int iLevel) const;
        int GetSpecialDamage (SpecialDamageTypes iSpecial, DWORD dwFlags = 0) const;
        inline int GetStealth (void) const { return m_iStealth; }
        inline FireTypes GetType (void) const { return m_iFireType; }
        inline const CString &GetUNID (void) const { return m_sUNID; }
        inline const SVaporTrailDesc &GetVaporTrail (void) const { return GetOldEffects().VaporTrail; }
		CItemType *GetWeaponType (CItemType **retpLauncher = NULL) const;
		inline bool HasEvents (void) const { return !m_Events.IsEmpty(); }
		inline bool HasFragments (void) const { return m_pFirstFragment != NULL; }
		inline bool HasOnFragmentEvent (void) const { return m_CachedEvents[evtOnFragment].pCode != NULL; }
		void InitFromDamage (const DamageDesc &Damage);
		ALERROR InitFromMissileXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID, CItemType *pMissile);
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID, int iLevel, bool bDamageOnly = false);
        ALERROR InitScaledStats (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pItem);
        inline bool IsDirectionalImage (void) const { return m_fDirectional; }
        inline bool IsFragment (void) const { return m_fFragment; }
        inline bool IsScalable (void) const { return (m_pScalable != NULL); }
		inline bool IsTracking (void) const { return m_iManeuverability != 0; }
		bool IsTrackingOrHasTrackingFragments (void) const;
		inline bool IsTrackingTime (int iTick) const { return (m_iManeuverability > 0 && (iTick % m_iManeuverability) == 0); }
		void MarkImages (void);
		ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx);
		inline void PlayFireSound (CSpaceObject *pSource) { m_FireSound.PlaySound(pSource); }
		inline bool ProximityBlast (void) const { return (m_fProximityBlast ? true : false); }

	private:
        struct SOldEffects
            {
		    CObjectImageArray Image;		//	Image for missile
            SExhaustDesc Exhaust;           //  Exhaust effect
            SVaporTrailDesc VaporTrail;     //  Vapor trail effect
            };

        //  Copying this class is not supported

		CWeaponFireDesc (const CWeaponFireDesc &Desc) = delete;
        CWeaponFireDesc & operator= (const CWeaponFireDesc &Desc) = delete;

        Metric CalcMaxEffectiveRange (void) const;
		CEffectCreator *GetFireEffect (void) const;
        inline SOldEffects &GetOldEffects (void) const { return (m_pOldEffects ? *m_pOldEffects : m_NullOldEffects); }
        inline SOldEffects &SetOldEffects (void) { if (m_pOldEffects == NULL) m_pOldEffects = new SOldEffects; return *m_pOldEffects; }
        ALERROR InitScaledStats (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pItem, const CWeaponFireDesc &Src, int iBaseLevel, int iScaledLevel);

		CExtension *m_pExtension;			//	Extension that defines the weaponfiredesc
		CString m_sUNID;					//	Identification. The format is
											//		uuuuu/n/fi/e
											//
											//		uuuuu = weapon UNID
											//		n = shot data ordinal
											//		fi = fragment, i is index (optional)
											//		e = enhanced (optional)


		//	Basic properties
        int m_iLevel;                       //  Level of desc (missile or weapon or scalable weapon)
		CItemTypeRef m_pAmmoType;			//	item type for this ammo
		FireTypes m_iFireType;				//	beam or missile
		DamageDesc m_Damage;				//	Damage per shot
		int m_iContinuous;					//	repeat for this number of frames

		Metric m_rMissileSpeed;				//	Speed of missile
		DiceRange m_MissileSpeed;			//	Speed of missile (if random)
		DiceRange m_Lifetime;				//	Lifetime of fire in seconds
		DiceRange m_InitialDelay;			//	Delay for n ticks before starting

		int m_iPassthrough;					//	Chance that the missile will continue through target

        //  Computed properties
		Metric m_rMaxEffectiveRange;		//	Max effective range of weapon

		//	Effects
		CEffectCreatorRef m_pEffect;		//	Effect for the actual bullet/missile/beam
		CEffectCreatorRef m_pHitEffect;		//	Effect when we hit/explode
		CEffectCreatorRef m_pFireEffect;	//	Effect when we fire (muzzle flash)
		CSoundRef m_FireSound;				//	Sound when weapon is fired
        SOldEffects *m_pOldEffects;         //  Non-painter effects.

		//	Missile stuff (m_iFireType == ftMissile)
		int m_iAccelerationFactor;			//	% increase in speed per 10 ticks
		Metric m_rMaxMissileSpeed;			//	Max speed.
		int m_iStealth;						//	Missile stealth
		int m_iHitPoints;					//	HP before dissipating (0 = destroyed by any hit)
		int m_iInteraction;					//	Interaction opacity (0-100)
		int m_iManeuverability;				//	Tracking maneuverability (0 = none)
		int m_iManeuverRate;				//	Angle turned at each maneuverability point

		//	Particles (m_iFireType == ftParticles)
		CParticleSystemDesc *m_pParticleDesc;
		DiceRange m_MinDamage;				//	Minimum damage when hit by particles

		//	Area stuff (m_iFireType == ftArea)
		DiceRange m_ExpansionSpeed;			//	Speed of expansion (% of lightspeed)
		DiceRange m_AreaDamageDensity;		//	Number of points along the edge

		//	Radius stuff (m_iFireType == ftRadius)
		Metric m_rMinRadius;				//	All objects inside this radius take full damage
		Metric m_rMaxRadius;				//	Damage inside this radius is decreased by inverse square law

		//	Fragmentation
		SFragmentDesc *m_pFirstFragment;	//	Pointer to first fragment desc (or NULL)
		int m_iProximityFailsafe;			//	Min ticks before proximity is active

		//	Events
		CEventHandler m_Events;				//	Events
		SEventHandlerDesc m_CachedEvents[evtCount];

        //  Scalable items
        int m_iBaseLevel;                   //  Base level
        int m_iScaledLevels;                //  Number of scaled levels above base
        CWeaponFireDesc *m_pScalable;       //  Array of entries, one per scaled level above base

		//	Flags
		DWORD m_fVariableInitialSpeed:1;	//	TRUE if initial speed is random
		DWORD m_fNoFriendlyFire:1;			//	TRUE if we cannot hit our friends
		DWORD m_fNoWorldHits:1;				//	If TRUE, we never hit worlds (or stars)
		DWORD m_fNoImmutableHits:1;			//	If TRUE, we never hit immutable objects
		DWORD m_fNoStationHits:1;			//	If TRUE, we never hit station-scale objects
		DWORD m_fNoImmobileHits:1;			//	If TRUE, we never hit immobile objects
		DWORD m_fNoShipHits:1;				//	If TRUE, we never hit ship-scale objects
		DWORD m_fAutoTarget:1;				//	If TRUE, we can acquire new targets after launch

		DWORD m_fCanDamageSource;			//	TRUE if we can damage the source
		DWORD m_fDirectional:1;				//	True if different images for each direction
		DWORD m_fFragment:1;				//	True if this is a fragment of a proximity blast
		DWORD m_fProximityBlast;			//	This is TRUE if we have fragments or if we have
											//		and OnFragment event.
        DWORD m_fSpare5:1;
        DWORD m_fSpare6:1;
        DWORD m_fSpare7:1;
        DWORD m_fSpare8:1;

		DWORD m_dwSpare:16;

        static SOldEffects m_NullOldEffects;
	};

//	Space environment

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
	SSystemCreateCtx (void) : 
			pExtension(NULL),
			pStats(NULL),
			pStation(NULL), 
			dwLastObjID(0)
		{ }

	//	Context

	CExtension *pExtension;					//	Extension from which the current desc came
	CTopologyNode *pTopologyNode;			//	Topology node
	CSystem *pSystem;						//	System that we're creating

	TArray<CXMLElement *> LocalTables;		//	Stack of local tables

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
			bForcePainted(false)
		{ }

	Metric rSecondsPerTick;
	bool bForceEventFiring;					//	If TRUE, fire events even if no player ship
	bool bForcePainted;						//	If TRUE, mark objects as painted 
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

			//	IsAreaClear flags
			IAC_FIXED_RADIUS =				0x00000001,	//	Don't use station exclusion zone
			IAC_INCLUDE_NON_ENEMIES =		0x00000002,	//	Non-enemies count
			IAC_INCLUDE_NON_ATTACKING =		0x00000004,	//	Include objects that cannot attack

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
								  DestructionTypes iCause,
								  const CDamageSource &Source,
								  const CVector &vPos,
								  const CVector &vVel,
								  int iDirection,
								  CSpaceObject *pTarget,
								  DWORD dwFlags,
								  CSpaceObject **retpShot);
		ALERROR CreateWeaponFragments (CWeaponFireDesc *pDesc,
									   CItemEnhancementStack *pEnhancements,
									   DestructionTypes iCause,
									   const CDamageSource &Source,
									   CSpaceObject *pTarget,
									   const CVector &vPos,
									   const CVector &vVel,
									   CSpaceObject *pMissileSource,
                                       int iFraction = 100);

		ALERROR AddTimedEvent (CTimedEvent *pEvent);
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
		void FireOnSystemWeaponFire (CSpaceObject *pShot, CWeaponFireDesc *pDesc, const CDamageSource &Source);
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
		CSpaceObject *GetObject (int iIndex) const { return (CSpaceObject *)m_AllObjects.GetObject(iIndex); }
		int GetObjectCount (void) const { return m_AllObjects.GetCount(); }
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
		CSpaceObject *HitScan (CSpaceObject *pExclude, const CVector &vStart, const CVector &vEnd, CVector *retvHitPos = NULL);
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
		inline void RegisterForOnSystemCreated (CSpaceObject *pObj) { m_DeferredOnCreate.FastAdd(pObj); }
		void RemoveObject (SDestroyCtx &Ctx);
		void RemoveTimersForObj (CSpaceObject *pObj);
		void RestartTime (void);
		ALERROR SaveToStream (IWriteStream *pStream);
		inline void SetID (DWORD dwID) { m_dwID = dwID; }
		void SetLastUpdated (void);
		void SetObjectSovereign (CSpaceObject *pObj, CSovereign *pSovereign);
		inline void SetPlayerUnderAttack (void) { m_fPlayerUnderAttack = true; }
		void SetPOVLRS (CSpaceObject *pCenter);
		void SetSpaceEnvironment (int xTile, int yTile, CSpaceEnvironmentType *pEnvironment);
		ALERROR StargateCreated (CSpaceObject *pGate, const CString &sStargateID, const CString &sDestNodeID, const CString &sDestEntryPoint);
		void StopTime (const CSpaceObjectList &Targets, int iDuration);
		void StopTimeForAll (int iDuration, CSpaceObject *pExcept);
		CVector TileToVector (int x, int y) const;
		void TransferObjEventsIn (CSpaceObject *pObj, CTimedEventList &ObjEvents);
		void TransferObjEventsOut (CSpaceObject *pObj, CTimedEventList &ObjEvents);
		void UnnameObject (CSpaceObject *pObj);
		void UnregisterEventHandler (CSpaceObject *pObj);
		void Update (SSystemUpdateCtx &SystemCtx, SViewportAnnotations *pAnnotations = NULL);
		void UpdateExtended (const CTimeSpan &ExtraTime);
		void VectorToTile (const CVector &vPos, int *retx, int *rety) const;
		void WriteObjRefToStream (CSpaceObject *pObj, IWriteStream *pStream, CSpaceObject *pReferrer = NULL);
		void WriteSovereignRefToStream (CSovereign *pSovereign, IWriteStream *pStream);

		//	Locations & Territories
		ALERROR AddTerritory (CTerritoryDef *pTerritory);
		inline void BlockOverlappingLocations (void) { m_Locations.FillCloseLocations(); }
		int CalcLocationWeight (CLocationDef *pLoc, const CAttributeCriteria &Criteria);
		ALERROR CreateLocation (const CString &sID, const COrbit &Orbit, const CString &sAttributes, CLocationDef **retpLocation = NULL);
		bool FindRandomLocation (const SLocationCriteria &Criteria, DWORD dwFlags, const COrbit &CenterOrbitDesc, CStationType *pStationToPlace, int *retiLocID);
		int GetEmptyLocationCount (void);
		bool GetEmptyLocations (const SLocationCriteria &Criteria, const COrbit &CenterOrbitDesc, CStationType *pStationToPlace, TProbabilityTable<int> *retTable);
		inline bool GetEmptyLocations (TArray<int> *retList) { return m_Locations.GetEmptyLocations(retList); }
		inline CLocationDef *GetLocation (int iLocID) { return m_Locations.GetLocation(iLocID); }
		inline int GetLocationCount (void) { return m_Locations.GetCount(); }
		bool IsAreaClear (const CVector &vPos, Metric rRadius, DWORD dwFlags, CStationType *pType = NULL);
		inline void SetLocationObjID (int iLocID, DWORD dwObjID) { m_Locations.SetObjID(iLocID, dwObjID); }

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

		CSystem (CUniverse *pUniv, CTopologyNode *pTopology);

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
		void FlushEnemyObjectCache (void);
		inline int GetTimedEventCount (void) { return m_TimedEvents.GetCount(); }
		inline CTimedEvent *GetTimedEvent (int iIndex) { return m_TimedEvents.GetEvent(iIndex); }
		void InitSpaceEnvironment (void) const;
		void InitVolumetricMask (void);
		void PaintDestinationMarker (SViewportPaintCtx &Ctx, CG32bitImage &Dest, int x, int y, CSpaceObject *pObj);
		void PaintViewportAnnotations (CG32bitImage &Dest, SViewportAnnotations &Annotations, SViewportPaintCtx &Ctx);
		void RemoveVolumetricShadow (CSpaceObject *pObj);
		void UpdateGravity (SUpdateCtx &Ctx, CSpaceObject *pGravityObj);
		void UpdateRandomEncounters (void);

		//	Game instance data

		DWORD m_dwID;							//	System ID
		CSystemType *m_pType;					//	System type definition
		CString m_sName;						//	Name of system
		CTopologyNode *m_pTopology;				//	Topology descriptor

		CObjectArray m_AllObjects;				//	Array of CSpaceObject
		TSortMap<CString, CSpaceObject *> m_NamedObjects;			//	Indexed array of named objects (CSpaceObject *)

		CTimedEventList m_TimedEvents;			//	Array of CTimedEvent
		CEnvironmentGrid mutable *m_pEnvironment;		//	Nebulas, etc.
		CSystemEventHandlerNode m_EventHandlers;	//	List of system event handler
		CNavigationPathNode m_NavPaths;			//	List of navigation paths
		CLocationList m_Locations;				//	List of point locations
		CTerritoryList m_Territories;			//	List of defined territories

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
		DWORD m_fSpare8:1;

		DWORD m_fSpare:24;

		//	Support structures

		CThreadPool *m_pThreadPool;				//	Thread pool for painting
		CSpaceObjectList m_EncounterObjs;		//	List of objects that generate encounters
		CSpaceObjectList m_BarrierObjects;		//	List of barrier objects
		CSpaceObjectList m_GravityObjects;		//	List of objects that have gravity
		TArray<SStarDesc> m_Stars;				//	List of stars in the system
		CSpaceObjectGrid m_ObjGrid;				//	Grid to help us hit test
		CSpaceObjectList m_DeletedObjects;		//	List of objects deleted in the current update
		CSpaceObjectList m_LayerObjs[layerCount];	//	List of objects by layer
		CSpaceObjectList m_EnhancedDisplayObjs;	//	List of objects to show in viewport periphery
		CSpaceObjectList m_BackgroundObjs;		//	List of background objects to paint in viewport
		CSpaceObjectList m_ForegroundObjs;		//	List of foreground objects to paint in viewport
		CSpaceObjectList m_DeferredOnCreate;	//	Ordered list of objects that need an OnSystemCreated call
		CSystemSpacePainter m_SpacePainter;		//	Paints space background
		CMapGridPainter m_GridPainter;			//	Structure to paint a grid

		static const Metric g_MetersPerKlick;

	friend CObjectClass<CSystem>;
	};

//	Miscellaneous Structures & Classes

class CShockwaveHitTest
	{
	public:
		void Init (int iSegments, int iLives);
		inline bool IsEmpty (void) const { return (m_Segments.GetCount() == 0); }
		void ReadFromStream (SLoadCtx &Ctx);
		void Update (SEffectUpdateCtx &Ctx, const CVector &vPos, Metric rMinRadius, Metric rMaxRadius);
		void WriteToStream (IWriteStream *pStream) const;

	private:
		void CalcObjBounds (CSpaceObject *pObj, const CVector &vPos, Metric *retrStartAngle, Metric *retrSizeAngle, Metric *retrStartRadius, Metric *retrEndRadius);

		TArray<int> m_Segments;
	};

//	Topology

class CTopology
	{
	public:
		CTopology (void);
		~CTopology (void);

		ALERROR AddStargateFromXML (STopologyCreateCtx &Ctx, CXMLElement *pDesc, CTopologyNode *pNode = NULL, bool bRootNode = false);
		ALERROR AddTopology (STopologyCreateCtx &Ctx);
		ALERROR AddTopologyDesc (STopologyCreateCtx &Ctx, CTopologyDesc *pNode, CTopologyNode **retpNewNode = NULL);
		ALERROR AddTopologyNode (STopologyCreateCtx &Ctx, const CString &sNodeID, CTopologyNode **retpNewNode = NULL);
		void DeleteAll (void);
		CTopologyNode *FindTopologyNode (const CString &sID);
		int GetDistance (const CString &sSourceID, const CString &sDestID);
		inline CTopologyNodeList &GetTopologyNodeList (void) { return m_Topology; }
		inline CTopologyNode *GetTopologyNode (int iIndex) { return m_Topology.GetAt(iIndex); }
		inline int GetTopologyNodeCount (void) { return m_Topology.GetCount(); }
		ALERROR InitComplexArea (CXMLElement *pAreaDef, int iMinRadius, CComplexArea *retArea, STopologyCreateCtx *pCtx = NULL, CTopologyNode **iopExit = NULL); 
		void ReadFromStream (SUniverseLoadCtx &Ctx);

	private:
		enum NodeTypes
			{
			typeStandard,
			typeFragmentStart,
			typeFragment,
			};

		ALERROR AddFragment (STopologyCreateCtx &Ctx, CTopologyDesc *pFragment, CTopologyNode **retpNewNode);
		ALERROR AddNetwork (STopologyCreateCtx &Ctx, CTopologyDesc *pNetwork, CTopologyNode **retpNewNode);
		ALERROR AddNode (STopologyCreateCtx &Ctx, CTopologyDesc *pNode, CTopologyNode **retpNewNode);
		ALERROR AddNodeGroup (STopologyCreateCtx &Ctx, CTopologyDesc *pTable, CTopologyNode **retpNewNode);
		ALERROR AddNodeTable (STopologyCreateCtx &Ctx, CTopologyDesc *pTable, CTopologyNode **retpNewNode);

		ALERROR AddRandom (STopologyCreateCtx &Ctx, CTopologyDesc *pDesc, CTopologyNode **retpNewNode);
		ALERROR AddRandomParsePosition (STopologyCreateCtx *pCtx, const CString &sValue, CTopologyNode **iopExit, int *retx, int *rety);
		ALERROR AddRandomRegion (STopologyCreateCtx &Ctx, CTopologyDesc *pDesc, CXMLElement *pRegionDef, CTopologyNode *&pExitNode, CIntGraph &Graph, TArray<CTopologyNode *> &Nodes);

		ALERROR AddStargate (STopologyCreateCtx &Ctx, CTopologyNode *pNode, bool bRootNode, CXMLElement *pGateDesc);
		ALERROR AddTopologyNode (STopologyCreateCtx &Ctx,
								 CSystemMap *pMap,
								 const CString &sID,
								 bool bNoMap,
								 int x,
								 int y,
								 const CString &sAttribs,
								 CXMLElement *pSystemDesc,
								 CEffectCreator *pEffect,
								 CTopologyNode **retpNode = NULL);
		ALERROR AddTopologyNode (const CString &sID, CTopologyNode *pNode);
		CString ExpandNodeID (STopologyCreateCtx &Ctx, const CString &sID);
		ALERROR FindTopologyDesc (STopologyCreateCtx &Ctx, const CString &sNodeID, CTopologyDesc **retpNode, NodeTypes *retiNodeType = NULL);
		CString GenerateUniquePrefix (const CString &sPrefix, const CString &sTestNodeID);
		void GetAbsoluteDisplayPos (STopologyCreateCtx &Ctx, int x, int y, int *retx, int *rety, int *retiRotation);
		int GetDistance (CTopologyNode *pSource, const CString &sDestID, int iBestDist = -1);
		void GetFragmentDisplayPos (STopologyCreateCtx &Ctx, CTopologyNode *pNode, int *retx, int *rety);
		ALERROR GetOrAddTopologyNode (STopologyCreateCtx &Ctx, const CString &sID, CTopologyNode *pPrevNode, CXMLElement *pGateDesc, CTopologyNode **retpNode);

		CTopologyNodeList m_Topology;
		TSortMap<CString, int> m_IDToNode;
	};

//	Events

class CTimedEncounterEvent : public CTimedEvent
	{
	public:
		CTimedEncounterEvent (void) { } //	Used only for loading
		CTimedEncounterEvent (int iTick,
							  CSpaceObject *pTarget,
							  DWORD dwEncounterTableUNID,
							  CSpaceObject *pGate,
							  Metric rDistance);

		virtual CString DebugCrashInfo (void);
		virtual void DoEvent (DWORD dwTick, CSystem *pSystem);
		virtual bool OnObjDestroyed (CSpaceObject *pObj);

	protected:
		virtual void OnReadFromStream (SLoadCtx &Ctx);
		virtual void OnWriteClassToStream (IWriteStream *pStream);
		virtual void OnWriteToStream (CSystem *pSystem, IWriteStream *pStream);

	private:
		CSpaceObject *m_pTarget;
		DWORD m_dwEncounterTableUNID;
		CSpaceObject *m_pGate;
		Metric m_rDistance;
	};

class CTimedCustomEvent : public CTimedEvent
	{
	public:
		CTimedCustomEvent (void) { }	//	Used only for loading
		CTimedCustomEvent (int iTick,
						   CSpaceObject *pObj,
						   const CString &sEvent);

		virtual CString DebugCrashInfo (void);
		virtual void DoEvent (DWORD dwTick, CSystem *pSystem);
		virtual CString GetEventHandlerName (void) { return m_sEvent; }
		virtual CSpaceObject *GetEventHandlerObj (void) { return m_pObj; }
		virtual bool OnObjChangedSystems (CSpaceObject *pObj);
		virtual bool OnObjDestroyed (CSpaceObject *pObj);

	protected:
		virtual void OnReadFromStream (SLoadCtx &Ctx);
		virtual void OnWriteClassToStream (IWriteStream *pStream);
		virtual void OnWriteToStream (CSystem *pSystem, IWriteStream *pStream);

	private:
		CSpaceObject *m_pObj;
		CString m_sEvent;
	};

class CTimedRecurringEvent : public CTimedEvent
	{
	public:
		CTimedRecurringEvent (void) { }	//	Used only for loading
		CTimedRecurringEvent (int iInterval,
							  CSpaceObject *pObj,
							  const CString &sEvent);

		virtual CString DebugCrashInfo (void);
		virtual void DoEvent (DWORD dwTick, CSystem *pSystem);
		virtual CString GetEventHandlerName (void) { return m_sEvent; }
		virtual CSpaceObject *GetEventHandlerObj (void) { return m_pObj; }
		virtual bool OnObjChangedSystems (CSpaceObject *pObj);
		virtual bool OnObjDestroyed (CSpaceObject *pObj);

	protected:
		virtual void OnReadFromStream (SLoadCtx &Ctx);
		virtual void OnWriteClassToStream (IWriteStream *pStream);
		virtual void OnWriteToStream (CSystem *pSystem, IWriteStream *pStream);

	private:
		int m_iInterval;
		CSpaceObject *m_pObj;
		CString m_sEvent;
	};

class CTimedTypeEvent : public CTimedEvent
	{
	public:
		CTimedTypeEvent (void) { }	//	Used only for loading
		CTimedTypeEvent (int iTick,
						 int iInterval,
						 CDesignType *pType,
						 const CString &sEvent);

		virtual CString DebugCrashInfo (void);
		virtual void DoEvent (DWORD dwTick, CSystem *pSystem);
		virtual CString GetEventHandlerName (void) { return m_sEvent; }
		virtual CDesignType *GetEventHandlerType (void) { return m_pType; }

	protected:
		virtual void OnReadFromStream (SLoadCtx &Ctx);
		virtual void OnWriteClassToStream (IWriteStream *pStream);
		virtual void OnWriteToStream (CSystem *pSystem, IWriteStream *pStream);

	private:
		int m_iInterval;			//	0 = not recurring
		CDesignType *m_pType;
		CString m_sEvent;
	};

//	Docking ports implementation

class CDockingPorts
	{
	public:
		CDockingPorts (void);
		~CDockingPorts (void);

		void DockAtRandomPort (CSpaceObject *pOwner, CSpaceObject *pObj);
		bool DoesPortPaintInFront (CSpaceObject *pOwner, int iPort) const;
		int FindNearestEmptyPort (CSpaceObject *pOwner, CSpaceObject *pRequestingObj, CVector *retvDistance = NULL, int *retiEmptyPortCount = NULL);
		int FindRandomEmptyPort (CSpaceObject *pOwner);
		inline int GetPortCount (CSpaceObject *pOwner) { return m_iPortCount; }
		inline CSpaceObject *GetPortObj (CSpaceObject *pOwner, int iPort) { ASSERT(m_pPort[iPort].pObj == NULL || m_pPort[iPort].iStatus != psEmpty); return m_pPort[iPort].pObj; }
		inline CVector GetPortPos (CSpaceObject *pOwner, int iPort, CSpaceObject *pShip, bool *retbPaintInFront = NULL, int *retiRotation = NULL) { return GetPortPos(pOwner, m_pPort[iPort], pShip, retbPaintInFront, retiRotation); }
		int GetPortsInUseCount (CSpaceObject *pOwner);
		void InitPorts (CSpaceObject *pOwner, int iCount, Metric rRadius);
		void InitPorts (CSpaceObject *pOwner, int iCount, CVector *pPos);
		void InitPorts (CSpaceObject *pOwner, const TArray<CVector> &Desc);
		void InitPortsFromXML (CSpaceObject *pOwner, CXMLElement *pElement, int iScale = -1);
		inline bool IsObjDocked (CSpaceObject *pObj) { return IsDocked(pObj); }
		inline bool IsObjDockedOrDocking (CSpaceObject *pObj) { return IsDockedOrDocking(pObj); }
		inline bool IsPortEmpty (CSpaceObject *pOwner, int iPort) const { return (m_pPort[iPort].iStatus == psEmpty); }
		void MoveAll (CSpaceObject *pOwner);
		void OnDestroyed (void);
		void OnNewSystem (CSystem *pNewSystem);
		void OnObjDestroyed (CSpaceObject *pOwner, CSpaceObject *pObj, bool *retbDestroyed = NULL);
		void ReadFromStream (CSpaceObject *pOwner, SLoadCtx &Ctx);
		void RepairAll (CSpaceObject *pOwner, int iRepairRate);
		bool RequestDock (CSpaceObject *pOwner, CSpaceObject *pObj, int iPort = -1);
		void Undock (CSpaceObject *pOwner, CSpaceObject *pObj, bool *retbWasDocked = NULL);
		void UpdateAll (SUpdateCtx &Ctx, CSpaceObject *pOwner);
		void WriteToStream (CSpaceObject *pOwner, IWriteStream *pStream);

	private:
		enum DockingPortStatus
			{
			psEmpty,
			psDocking,
			psInUse
			};

		enum DockingPortLayer
			{
			plStandard,							//	Depends on position

			plBringToFront,						//	Ship is always in front of station
			plSendToBack,						//	Ship is always behind station
			};

		struct SDockingPort
			{
			SDockingPort (void) :
					iStatus(psEmpty),
					iLayer(plStandard),
					pObj(NULL),
					iRotation(0)
				{ }

			DockingPortStatus iStatus;			//	Status of port
			DockingPortLayer iLayer;			//	Port layer relative to station
			CSpaceObject *pObj;					//	Object docked at this port
			CVector vPos;						//	Position of dock (relative coords)
			int iRotation;						//	Rotation of ship at dock
			};

		CVector GetPortPos (CSpaceObject *pOwner, const SDockingPort &Port, CSpaceObject *pShip, bool *retbPaintInFront = NULL, int *retiRotation = NULL) const;
		bool IsDocked (CSpaceObject *pObj);
		bool IsDockedOrDocking (CSpaceObject *pObj);
		bool ShipsNearPort (CSpaceObject *pOwner, CSpaceObject *pRequestingObj, const CVector &vPortPos);
		void UpdateDockingManeuvers (CSpaceObject *pOwner, SDockingPort &Port);

		int m_iPortCount;						//	Number of docking ports
		SDockingPort *m_pPort;					//	Array of docking ports
		int m_iMaxDist;							//	Max distance for docking (in light-seconds)
	};

//	Attack Detector

class CAttackDetector
	{
	public:
		CAttackDetector (void);

		inline void Blacklist (void) { m_iCounter = -1; }
		inline void ClearBlacklist (void) { m_iCounter = 0; }
		inline bool IsBlacklisted (void) { return m_iCounter == -1; }
		bool Hit (int iTick);
		void ReadFromStream (SLoadCtx &Ctx);
		inline void Update (int iTick) { if ((iTick % DECAY_RATE) == 0) OnUpdate(); }
		void WriteToStream (IWriteStream *pStream);

	private:
		enum Constants
			{
			DECAY_RATE = 150,
			HIT_LIMIT = 3,
			MULTI_HIT_WINDOW = 20,
			};

		void OnUpdate (void);

		int m_iCounter;
		int m_iLastHit;
	};

//	Item Event Handler

enum EItemEventDispatchTypes
	{
	dispatchFireEvent,
	dispatchCheckEnhancementLifetime,
	};

class CItemEventDispatcher
	{
	public:
		CItemEventDispatcher (void);
		~CItemEventDispatcher (void);

		inline void FireEvent (CSpaceObject *pSource, ECodeChainEvents iEvent)	{ if (m_pFirstEntry) FireEventFull(pSource, iEvent); }
		inline void FireUpdateEvents (CSpaceObject *pSource) { if (m_pFirstEntry) FireUpdateEventsFull(pSource); }
		void Init (CSpaceObject *pSource);

	private:
		struct SEntry
			{
			EItemEventDispatchTypes iType;				//	Type of entry
			CItem theItem;								//	Item

			ECodeChainEvents iEvent;					//	Event (if dispatchFireEvent)
			SEventHandlerDesc Event;					//	Code (if dispatchFireEvent)

			DWORD dwEnhancementID;						//	ID of enhancement (if relevant)

			SEntry *pNext;
			};

		void AddEntry (const CString &sEvent, EItemEventDispatchTypes iType, const SEventHandlerDesc &Event, const CItem &Item, DWORD dwEnhancementID);
		SEntry *AddEntry (void);
		void FireEventFull (CSpaceObject *pSource, ECodeChainEvents iEvent);
		void FireUpdateEventsFull (CSpaceObject *pSource);
		void RemoveAll (void);

		SEntry *m_pFirstEntry;
	};

//	Ship classes and types

class COverlay
	{
	public:
		COverlay (void);
		~COverlay (void);
		static void CreateFromType (COverlayType *pType, 
									int iPosAngle,
									int iPosRadius,
									int iRotation,
									int iLifeLeft, 
									COverlay **retpField);

		bool AbsorbDamage (CSpaceObject *pSource, SDamageCtx &Ctx);
		void AccumulateBounds (CSpaceObject *pSource, RECT *ioBounds);
		void Destroy (CSpaceObject *pSource);
		inline bool Disarms (CSpaceObject *pSource) const { return m_pType->Disarms(); }
		void FireCustomEvent (CSpaceObject *pSource, const CString &sEvent, ICCItem *pData, ICCItem **retpResult);
		void FireOnCreate (CSpaceObject *pSource);
		bool FireOnDamage (CSpaceObject *pSource, SDamageCtx &Ctx);
		void FireOnDestroy (CSpaceObject *pSource);
		void FireOnObjDestroyed (CSpaceObject *pSource, const SDestroyCtx &Ctx) const;
		inline int GetCounter (void) const { return m_iCounter; }
		inline const CString &GetData (const CString &sAttrib) { return m_Data.GetData(sAttrib); }
		inline int GetDevice (void) const { return m_iDevice; }
		inline Metric GetDrag (CSpaceObject *pSource) const { return m_pType->GetDrag(); }
		inline DWORD GetID (void) const { return m_dwID; }
		inline const CString &GetMessage (void) const { return m_sMessage; }
		inline COverlay *GetNext (void) const { return m_pNext; }
		CVector GetPos (CSpaceObject *pSource);
		ICCItem *GetProperty (CCodeChainCtx *pCCCtx, CSpaceObject *pSource, const CString &sName);
		inline int GetRotation (void) const { return m_iRotation; }
		inline COverlayType *GetType(void) const { return m_pType; }
        inline void IncData (const CString &sAttrib, ICCItem *pValue = NULL, ICCItem **retpNewValue = NULL) { m_Data.IncData(sAttrib, pValue, retpNewValue); }
		inline bool IsDestroyed (void) const { return (m_fDestroyed ? true : false); }
		inline bool IsFading (void) const { return (m_fFading ? true : false); }
		inline bool IsShieldOverlay (void) const { return m_pType->IsShieldOverlay(); }
		inline bool IsShipScreenDisabled (void) const { return m_pType->IsShipScreenDisabled(); }
		void Paint (CG32bitImage &Dest, int iScale, int x, int y, SViewportPaintCtx &Ctx);
		void PaintAnnotations (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);
		void PaintBackground (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);
		void PaintLRSAnnotations (const ViewportTransform &Trans, CG32bitImage &Dest, int x, int y);
		void PaintMapAnnotations (CMapViewportCtx &Ctx, CG32bitImage &Dest, int x, int y);
		inline bool Paralyzes (CSpaceObject *pSource) const { return m_pType->Paralyzes(); }
		void ReadFromStream (SLoadCtx &Ctx);
		inline void SetData (const CString &sAttrib, const CString &sData) { m_Data.SetData(sAttrib, sData); }
		inline void SetDevice (int iDev) { m_iDevice = iDev; }
		bool SetEffectProperty (const CString &sProperty, ICCItem *pValue);
		inline void SetNext (COverlay *pNext) { m_pNext = pNext; }
		void SetPos (CSpaceObject *pSource, const CVector &vPos);
		bool SetProperty (CSpaceObject *pSource, const CString &sName, ICCItem *pValue);
		inline void SetRotation (int iRotation) { m_iRotation = iRotation; }
		inline bool Spins (CSpaceObject *pSource) const { return m_pType->Spins(); }
		void Update (CSpaceObject *pSource);
		void WriteToStream (IWriteStream *pStream);

	private:
		void FireOnUpdate (CSpaceObject *pSource);
		void CreateHitEffect (CSpaceObject *pSource, SDamageCtx &Ctx);

		COverlayType *m_pType;					//	Type of field
		DWORD m_dwID;							//	Universal ID
		int m_iTick;							//	Overlay tick
		int m_iLifeLeft;						//	Ticks left of energy field life (-1 = permanent)
		int m_iDevice;							//	Index of device that we're associated with (-1 if not a device)

		int m_iPosAngle;						//	Position relative to source (degrees)
		int m_iPosRadius;						//	Position relative to source (pixels)
		int m_iRotation;						//	Overlay orientation (degrees)

		CAttributeDataBlock m_Data;				//	data
		int m_iCounter;							//	Arbitrary counter
		CString m_sMessage;						//	Message text

		IEffectPainter *m_pPainter;				//	Painter

		int m_iPaintHit;						//	If >0 then we paint a hit
		int m_iPaintHitTick;					//	Tick for hit painter
		IEffectPainter *m_pHitPainter;			//	Hit painter

		DWORD m_fDestroyed:1;					//	TRUE if field should be destroyed
		DWORD m_fFading:1;						//	TRUE if we're destroyed, but fading the effect

		COverlay *m_pNext;					//	Next energy field associated with this object
	};

class COverlayList
	{
	public:
		struct SImpactDesc
			{
			SImpactDesc (void) :
					bDisarm(false),
					bParalyze(false),
					bSpin(false),
					bShipScreenDisabled(false),
					rDrag(1.0)
				{ }

			bool bDisarm;					//	TRUE if source is disarmed
			bool bParalyze;					//	TRUE if source is paralyzed.
			bool bSpin;						//	TRUE if source should spin wildly.
			bool bShipScreenDisabled;		//	TRUE if source cannot bring up ship screen.

			Metric rDrag;					//	Drag coefficient (1.0 = no drag)
			};

		COverlayList (void);
		~COverlayList (void);

		void AddField (CSpaceObject *pSource, 
					   COverlayType *pType,
					   int iPosAngle,
					   int iPosRadius,
					   int iRotation,
					   int iLifeLeft, 
					   DWORD *retdwID = NULL);
		bool AbsorbDamage (CSpaceObject *pSource, SDamageCtx &Ctx);
		bool AbsorbsWeaponFire (CInstalledDevice *pDevice);
		void AccumulateBounds (CSpaceObject *pSource, RECT *ioBounds);
		bool Damage (CSpaceObject *pSource, SDamageCtx &Ctx);
		void FireOnObjDestroyed (CSpaceObject *pSource, const SDestroyCtx &Ctx) const;
		int GetCountOfType (COverlayType *pType);
		const CString &GetData (DWORD dwID, const CString &sAttrib);
		void GetImpact (CSpaceObject *pSource, SImpactDesc *retImpact) const;
		void GetList (TArray<COverlay *> *retList);
		void GetListOfCommandPaneCounters (TArray<COverlay *> *retList);
		COverlay *GetOverlay (DWORD dwID) const;
		CVector GetPos (CSpaceObject *pSource, DWORD dwID);
		ICCItem *GetProperty (CCodeChainCtx *pCCCtx, CSpaceObject *pSource, DWORD dwID, const CString &sName);
		int GetRotation (DWORD dwID);
		COverlayType *GetType(DWORD dwID);
		int GetWeaponBonus (CInstalledDevice *pDevice, CSpaceObject *pSource);
        void IncData (DWORD dwID, const CString &sAttrib, ICCItem *pValue = NULL, ICCItem **retpNewValue = NULL);
		inline bool IsEmpty (void) { return (m_pFirst == NULL); }
		void Paint (CG32bitImage &Dest, int iScale, int x, int y, SViewportPaintCtx &Ctx);
		void PaintAnnotations (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);
		void PaintBackground (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);
		void PaintLRSAnnotations (const ViewportTransform &Trans, CG32bitImage &Dest, int x, int y);
		void PaintMapAnnotations (CMapViewportCtx &Ctx, CG32bitImage &Dest, int x, int y);
		void ReadFromStream (SLoadCtx &Ctx, CSpaceObject *pSource);
		void RemoveField (CSpaceObject *pSource, DWORD dwID);
		void SetData (DWORD dwID, const CString &sAttrib, const CString &sData);
		bool SetEffectProperty (DWORD dwID, const CString &sProperty, ICCItem *pValue);
		void SetPos (CSpaceObject *pSource, DWORD dwID, const CVector &vPos);
		bool SetProperty (CSpaceObject *pSource, DWORD dwID, const CString &sName, ICCItem *pValue);
		void SetRotation (DWORD dwID, int iRotation);
		void Update (CSpaceObject *pSource, bool *retbModified);
		void WriteToStream (IWriteStream *pStream);

	private:
		COverlay *m_pFirst;
	};

//	Space Objects

#define MAX_COMMS_CAPS					25

//	CSpaceObject ---------------------------------------------------------------
//
//	SEMANTICS
//
//	CanAttack: If TRUE, this is an active/intelligent object that could pose
//		a threat. E.g., a ship or a station. Generally used to pick out a 
//		possible target. Missiles/beams cannot attack. Wrecks and abandoned
//		stations cannot attack.
//
//	Hidden: Hidden objects are never shown in the system. For example, suspended
//		objects are hidden. Anything that paints, e.g., fading missiles, is not
//		hidden.
//
//	Inactive: This is almost identical to !CanAttack, except that missiles are
//		considered active (unless they are fading out).
//
//	Intangible: If TRUE, this object is not a physical object in the system.
//		Virtual and hidden objects are intangible. So are missiles fading out.

class CSpaceObject : public CObject
	{
	public:
		enum Categories
			{
			catShip =			0x00000001,
			catStation =		0x00000002,
			catBeam =			0x00000004,
			catMissile =		0x00000008,
			catFractureEffect =	0x00000010,
			catMarker =			0x00000020,
			catOther =			0x80000000,
			};

		enum StealthAndPerceptionLevels
			{
			stealthMin =			0,
			stealthNormal =			4,
			stealthMax =			15,			//	Cloaked

			perceptMin =			0,
			perceptNormal =			4,
			perceptMax =			15,
			};

		enum InterSystemResults
			{
			interNoAction =					0,
			interFollowPlayer =				1,
			interWaitForPlayer =			2,
			};

		//	NOTE: Order must match INSTALL_ITEM_RESULTS_TABLE in CSpaceObject

		enum InstallItemResults
			{
			insOK = 0,							//	OK to install

			insArmorTooHeavy,					//	Armor is too heavy for ship
			insCannotInstall,					//	Cannot install (due to custom reason)
			insNoDeviceSlotsLeft,				//	No device slots left
			insNoNonWeaponSlotsLeft,			//	No non-weapon slots left
			insNoWeaponSlotsLeft,				//	No weapon slots left
			insNotInstallable,					//	Item cannot be installed
			insReactorIncompatible,				//	Reactor is too powerful for ship class
			insReactorOverload,					//	Sum of device power > than reactor output
			insReactorTooWeak,					//	Reactor output too small for device

			insReplaceCargo,					//	OK, but cargo device will be replaced
			insReplaceDrive,					//	OK, but drive device will be replaced
			insReplaceLauncher,					//	OK, but launcher will be replaced
			insReplaceOther,					//	OK, but another device will be replaced
			insReplaceReactor,					//	OK, but reactor will be replaced
			insReplaceShields,					//	OK, but shields will be replaced

			insInstallItemResultsCount,			//	Must be last enum
			};

		enum CriteriaSortTypes
			{
			sortNone,
			sortByDistance,
			};

		enum CriteriaPosCheckTypes
			{
			checkNone,
			checkPosIntersect,
			checkLineIntersect,
			};

		struct Criteria
			{
			CSpaceObject *pSource;				//	Source

			DWORD dwCategories;					//	Only these object categories
			bool bSelectPlayer;					//	Select the player
			bool bIncludeVirtual;				//	Include virtual objects
			bool bActiveObjectsOnly;			//	Only active object (e.g., objects that can attack)
			bool bKilledObjectsOnly;			//	Only objects that cannot attack
			bool bFriendlyObjectsOnly;			//	Only friendly to source
			bool bEnemyObjectsOnly;				//	Only enemy to source
			bool bAngryObjectsOnly;				//	Only enemy and angry objects
			bool bManufacturedObjectsOnly;		//	Exclude planets, stars, etc.
			bool bStructureScaleOnly;			//	Only structure-scale objects
			bool bStargatesOnly;				//	Only stargates
			bool bNearestOnly;					//	The nearest object to the source
			bool bFarthestOnly;					//	The fartest object to the source
			bool bNearerThan;					//	Only objects nearer than rMinRadius
			bool bFartherThan;					//	Only objects farther than rMaxRadius
			bool bHomeBaseIsSource;				//	Only objects whose home base is the source
			bool bDockedWithSource;				//	Only objects currently docked with source
			bool bExcludePlayer;				//	Exclude the player
			bool bTargetIsSource;				//	Only objects whose target is the source

			bool bPerceivableOnly;				//	Only objects that can be perceived by the source
			int iPerception;					//	Cached perception of pSource

			bool bSourceSovereignOnly;			//	Only objects the same sovereign as source
			DWORD dwSovereignUNID;				//	Only objects with this sovereign UNID

			CString sData;						//	Only objects with non-Nil data
			CString sStargateID;				//	Only objects with this stargate ID (if non blank)
			Metric rMinRadius;					//	Only objects at or beyond the given radius
			Metric rMaxRadius;					//	Only objects within the given radius
			int iIntersectAngle;				//	Only objects that intersect line from source
			IShipController::OrderTypes iOrder;	//	Only objects with this order

			TArray<CString> AttribsRequired;	//	Required attributes
			TArray<CString> AttribsNotAllowed;	//	Exclude objects with these attributes
			TArray<CString> SpecialRequired;	//	Special required attributes
			TArray<CString> SpecialNotAllowed;	//	Special excluding attributes

			int iEqualToLevel;					//	Objects of this level
			int iGreaterThanLevel;
			int iLessThanLevel;

			CriteriaPosCheckTypes iPosCheck;
			CVector vPos1;
			CVector vPos2;

			CriteriaSortTypes iSort;
			ESortOptions iSortOrder;
			};

		struct SCriteriaMatchCtx
			{
			SCriteriaMatchCtx (const Criteria &Crit);

			//	Computed from criteria
			Metric rMinRadius2;
			Metric rMaxRadius2;

			//	Nearest/farthest object
			CSpaceObject *pBestObj;
			Metric rBestDist2;

			//	Sorted results
			TSortMap<Metric, CSpaceObject *> DistSort;

			//	Temporaries
			bool bCalcPolar;
			bool bCalcDist2;
			};

		struct SOnCreate
			{
			SOnCreate (void) :
					pCreateCtx(NULL),
					pData(NULL),
					pBaseObj(NULL),
					pTargetObj(NULL),
					pOwnerObj(NULL),
					pOrbit(NULL)
				{ }

			SSystemCreateCtx *pCreateCtx;
			ICCItem *pData;
			CSpaceObject *pBaseObj;
			CSpaceObject *pTargetObj;
			CSpaceObject *pOwnerObj;
			const COrbit *pOrbit;
			};

		CSpaceObject (IObjectClass *pClass);
		virtual ~CSpaceObject (void);
		static void CreateFromStream (SLoadCtx &Ctx, CSpaceObject **retpObj);

		void Accelerate (const CVector &vPush, Metric rSeconds);
		void AccelerateStop (Metric rPush, Metric rSeconds);
		void AddEffect (IEffectPainter *pPainter, const CVector &vPos, int iTick = 0, int iRotation = 0);
		void AddEventSubscriber (CSpaceObject *pObj);
		void AddOverlay (COverlayType *pType, const CVector &vPos, int iRotation, int iLifetime, DWORD *retdwID = NULL);
		ALERROR AddToSystem (CSystem *pSystem, bool bNoGlobalInsert = false);
		void Ascend (void);
		inline bool Blocks (CSpaceObject *pObj) { return (m_fIsBarrier && CanBlock(pObj)); }
		inline bool BlocksShips (void) { return (m_fIsBarrier && CanBlockShips()); }
		void CalcOverlayPos (COverlayType *pOverlayType, const CVector &vPos, int *retiPosAngle, int *retiPosRadius);
		CSpaceObject *CalcTargetToAttack (CSpaceObject *pAttacker, CSpaceObject *pOrderGiver);
		inline bool CanBeControlled (void) { return m_iControlsFrozen == 0; }
		inline bool CanBeHit (void) { return (!m_fCannotBeHit && !m_fOutOfPlaneObj); }
		inline bool CanBeHitByFriends (void) { return !m_fNoFriendlyTarget; }
		inline bool CanBounce (void) { return m_fCanBounce; }
		bool CanDetect (int Perception, CSpaceObject *pObj);
		bool CanCommunicateWith (CSpaceObject *pSender);
		inline bool CanHitFriends (void) { return !m_fNoFriendlyFire; }
		inline void ClearPaintNeeded (void) { m_fPaintNeeded = false; }
		inline void ClearPlayerDestination (void) { m_fPlayerDestination = false; m_fAutoClearDestination = false; m_fAutoClearDestinationOnDock = false; m_fAutoClearDestinationOnDestroy = false; m_fShowDistanceAndBearing = false; m_fShowHighlight = false; }
		inline void ClearPlayerDocked (void) { m_fPlayerDocked = false; }
		inline void ClearPlayerTarget (void) { m_fPlayerTarget = false; }
		inline void ClearPOVLRS (void) { m_fInPOVLRS = false; }
		inline void ClearSelection (void) { m_fSelected = false; }
		inline void ClearShowDamageBar (void) { m_fShowDamageBar = false; }
		inline void ClearShowDistanceAndBearing (void) { m_fShowDistanceAndBearing = false; }
		inline void ClipSpeed (Metric rMaxSpeed) { m_vVel.Clip(rMaxSpeed); }
		void CommsMessageFrom (CSpaceObject *pSender, int iIndex);
		inline DWORD Communicate (CSpaceObject *pReceiver, MessageTypes iMessage, CSpaceObject *pParam1 = NULL, DWORD dwParam2 = 0) { return pReceiver->OnCommunicate(this, iMessage, pParam1, dwParam2); }
		void CopyDataFromObj (CSpaceObject *pSource);
		ALERROR CreateRandomItems (CXMLElement *pItems, CSystem *pSystem);
		ALERROR CreateRandomItems (IItemGenerator *pItems, CSystem *pSystem);
		EDamageResults Damage (SDamageCtx &Ctx);
		static CString DebugDescribe (CSpaceObject *pObj);
		inline bool DebugIsValid (void) { return (DWORD)m_pSystem != 0xdddddddd; }
		static CString DebugLoadError (SLoadCtx &Ctx);
		inline const CVector &DeltaV (const CVector &vDelta) { m_vVel = m_vVel + vDelta; return m_vVel; }
		void Destroy (DestructionTypes iCause, const CDamageSource &Attacker, CSpaceObject **retpWreck = NULL);
		void EnterGate (CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate, bool bAscend = false);
		int FindCommsMessage (const CString &sName);
		bool FindDevice (const CItem &Item, CInstalledDevice **retpDevice, CString *retsError);
		bool FindEventHandler (const CString &sEntryPoint, SEventHandlerDesc *retEvent = NULL);
		bool FindEventHandler (CDesignType::ECachedHandlers iEvent, SEventHandlerDesc *retEvent = NULL);
		inline bool FindEventSubscriber (CSpaceObject *pObj) { return m_SubscribedObjs.FindObj(pObj); }
		bool FireCanDockAsPlayer (CSpaceObject *pDockTarget, CString *retsError);
		bool FireCanInstallItem (const CItem &Item, int iSlot, CString *retsResult);
		bool FireCanRemoveItem (const CItem &Item, int iSlot, CString *retsResult);
		void FireCustomEvent (const CString &sEvent, ECodeChainEvents iEvent = eventNone, ICCItem *pData = NULL, ICCItem **retpResult = NULL);
		void FireCustomItemEvent (const CString &sEvent, const CItem &Item, ICCItem *pData, ICCItem **retpResult = NULL);
		void FireCustomOverlayEvent (const CString &sEvent, DWORD dwID, ICCItem *pData, ICCItem **retpResult = NULL);
		void FireCustomShipOrderEvent (const CString &sEvent, CSpaceObject *pShip, ICCItem **retpResult = NULL);
		bool FireGetDockScreen (CString *retsScreen, int *retiPriority, ICCItem **retpData);
		void FireGetExplosionType (SExplosionType *retExplosion);
		void FireOnAttacked (const SDamageCtx &Ctx);
		void FireOnAttackedByPlayer (void);
		void FireOnCreate (void);
		void FireOnCreate (const SOnCreate &OnCreate);
		void FireOnCreateOrders (CSpaceObject *pBase, CSpaceObject *pTarget);
		void FireOnDamage (SDamageCtx &Ctx);
		void FireOnDeselected (void);
		void FireOnDestroy (const SDestroyCtx &Ctx);
		bool FireOnDockObjAdj (CSpaceObject **retpObj);
		void FireOnEnteredGate (CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pGate);
		void FireOnEnteredSystem (CSpaceObject *pGate);
		void FireOnItemAIUpdate (void);
		void FireOnItemObjDestroyed (const SDestroyCtx &Ctx);
		void FireOnItemUpdate (void);
		void FireOnLoad (SLoadCtx &Ctx);
		void FireOnMining (const SDamageCtx &Ctx);
		void FireOnMissionAccepted (CMission *pMission);
		void FireOnMissionCompleted (CMission *pMission, const CString &sReason);
		void FireOnObjBlacklistedPlayer (CSpaceObject *pObj);
		void FireOnObjDestroyed (const SDestroyCtx &Ctx);
		void FireOnObjDocked (CSpaceObject *pObj, CSpaceObject *pDockTarget);
		void FireOnObjEnteredGate (CSpaceObject *pObj, CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate);
		bool FireOnObjGate (CSpaceObject *pObj);
		void FireOnObjJumped (CSpaceObject *pObj);
		bool FireOnObjJumpPosAdj (CSpaceObject *pObj, CVector *iovPos);
		void FireOnObjReconned (CSpaceObject *pObj);
		void FireOnOrderChanged (void);
		void FireOnOrdersCompleted (void);
		void FireOnPlayerBlacklisted (void);
		void FireOnPlayerEnteredShip (CSpaceObject *pOldShip);
		InterSystemResults FireOnPlayerEnteredSystem (CSpaceObject *pPlayer);
		void FireOnPlayerLeftShip (CSpaceObject *pNewShip);
		InterSystemResults FireOnPlayerLeftSystem (CSpaceObject *pPlayer, CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate);
		void FireOnSelected (void);
		bool FireOnSubordinateAttacked (const SDamageCtx &Ctx);
		void FireOnSystemExplosion (CSpaceObject *pExplosion, CSpaceObject *pSource, DWORD dwItemUNID);
		void FireOnSystemObjAttacked (SDamageCtx &Ctx);
		void FireOnSystemObjDestroyed (SDestroyCtx &Ctx);
		void FireOnSystemWeaponFire (CSpaceObject *pShot, CSpaceObject *pSource, DWORD dwItemUNID);
		bool FireOnTranslateMessage (const CString &sMessage, CString *retsMessage);
		inline void FreezeControls (void) { m_iControlsFrozen++; }
		inline DWORD GetAPIVersion (void) const { CDesignType *pType = GetType(); return (pType ? pType->GetAPIVersion() : API_VERSION); }
		void GetBoundingRect (CVector *retvUR, CVector *retvLL);
		inline CVector GetBoundsDiag (void) { return CVector(m_rBoundsX, m_rBoundsY); }
		inline Metric GetBoundsRadius (void) const { return Max(m_rBoundsX, m_rBoundsY); }
		CCommunicationsHandler *GetCommsHandler (void);
		int GetCommsMessageCount (void);
		inline const CString &GetData (const CString &sAttrib) const { return m_Data.GetData(sAttrib); }
		inline CString GetDataField (const CString &sField) { CString sValue; FindDataField(sField, &sValue); return sValue; }
		int GetDataInteger (const CString &sAttrib) const;
		CString GetDesiredCommsKey (void) const;
		inline int GetDestiny (void) const { return m_iDestiny; }
		inline Metric GetDetectionRange (int iPerception) { return RangeIndex2Range(GetDetectionRangeIndex(iPerception)); }
		Metric GetDetectionRange2 (int iPerception) const;
		int GetDetectionRangeIndex (int iPerception) const;
		CSovereign::Disposition GetDispositionTowards (CSpaceObject *pObj);
		Metric GetDistance (CSpaceObject *pObj) const { return (pObj->GetPos() - GetPos()).Length(); }
		Metric GetDistance2 (CSpaceObject *pObj) const { return (pObj->GetPos() - GetPos()).Length2(); }
		CDesignType *GetFirstDockScreen (CString *retsScreen, ICCItem **retpData);
		inline const CString &GetHighlightText (void) const { return m_sHighlightText; }
		void GetHitRect (CVector *retvUR, CVector *retvLL);
		Metric GetHitSize (void) const;
		inline int GetHitSizeHalfAngle (Metric rDist) const { return Max((int)(180.0 * atan(0.5 * GetHitSize() / rDist) / PI), 1); }
		inline DWORD GetID (void) const { return m_dwID; }
		inline int GetIndex (void) const { return m_iIndex; }
		CSpaceObject *GetNearestEnemy (Metric rMaxRange = g_InfiniteDistance, bool bIncludeStations = false);
		CSpaceObject *GetNearestEnemyStation (Metric rMaxRange = g_InfiniteDistance);
		CSpaceObject *GetNearestStargate (bool bExcludeUncharted = false);

#define FLAG_INCLUDE_NON_AGGRESSORS		0x00000001
#define FLAG_INCLUDE_STATIONS			0x00000002
		int GetNearestVisibleEnemies (int iMaxEnemies, 
									  Metric rMaxDist, 
									  TArray<CSpaceObject *> *pretList, 
									  CSpaceObject *pExcludeObj = NULL,
									  DWORD dwFlags = 0);

		CSpaceObject *GetNearestVisibleEnemy (Metric rMaxRange = g_InfiniteDistance, bool bIncludeStations = false, CSpaceObject *pExcludeObj = NULL);
		CSpaceObject *GetNearestVisibleEnemyInArc (int iMinFireArc, int iMaxFireArc, Metric rMaxRange = g_InfiniteDistance, bool bIncludeStations = false, CSpaceObject *pExcludeObj = NULL);
		CString GetNounPhrase (DWORD dwFlags);
		inline CSpaceObject *GetObjRefData (const CString &sAttrib) { return m_Data.GetObjRefData(sAttrib); }
		inline const CVector &GetOldPos (void) const { return m_vOldPos; }
		CSpaceObject *GetOrderGiver (DestructionTypes iCause = killedNone);
		inline CDesignType *GetOverride (void) { return m_pOverride; }
		inline CSpaceObject *GetPlayerShip (void) const { return (m_pSystem ? m_pSystem->GetPlayerShip() : NULL); }
		inline const CVector &GetPos (void) const { return m_vPos; }
		CSovereign *GetSovereignToDefend (void) const;
		const CString &GetStaticData (const CString &sAttrib);
		CG32bitPixel GetSymbolColor (void);
		inline CSystem *GetSystem (void) const { return m_pSystem; }
		inline CUniverse *GetUniverse (void) const { return m_pSystem->GetUniverse(); }
		inline const CVector &GetVel (void) const { return m_vVel; }
		void GetVisibleEnemies (DWORD dwFlags, TArray<CSpaceObject *> *retList, CSpaceObject *pExcludeObj = NULL);
		CSpaceObject *GetVisibleEnemyInRange (CSpaceObject *pCenter, Metric rMaxRange = g_InfiniteDistance, bool bIncludeStations = false, CSpaceObject *pExcludeObj = NULL);
		bool HasBeenHitLately (int iTicks = 30);
		bool HasFiredLately (int iTicks = 30);
		bool HasGravity (void) const { return (m_fHasGravity ? true : false); }
		inline bool HasInterSystemEvent (void) const { return (m_fHasInterSystemEvent ? true : false); }
		inline bool HasNonLinearMove (void) const { return (m_fNonLinearMove ? true : false); }
		inline bool HasOnAttackedEvent (void) const { return (m_fHasOnAttackedEvent ? true : false); }
		inline bool HasOnDamageEvent (void) const { return (m_fHasOnDamageEvent ? true : false); }
		inline bool HasOnObjDockedEvent (void) const { return (m_fHasOnObjDockedEvent ? true : false); }
		inline bool HasOnOrdersCompletedEvent (void) const { return (m_fHasOnOrdersCompletedEvent ? true : false); }
		inline bool HasOnSubordinateAttackedEvent (void) const { return (m_fHasOnSubordinateAttackedEvent ? true : false); }
		inline bool HasRandomEncounters (void) const { int iFreq; return (GetRandomEncounterTable(&iFreq) || iFreq > 0); }
		void Highlight (const CString &sText = NULL_STR);
		inline bool HitSizeInBox (const CVector &vUR, const CVector &vLL)
			{
			Metric rHalfSize = 0.5 * GetHitSize();
			return (vUR.GetX() > m_vPos.GetX() - rHalfSize) 
					&& (vUR.GetY() > m_vPos.GetY() - rHalfSize)
					&& (vLL.GetX() < m_vPos.GetX() + rHalfSize)
					&& (vLL.GetY() < m_vPos.GetY() + rHalfSize);
			}
		bool InBarrier (const CVector &vPos);
		inline bool InBox (const CVector &vUR, const CVector &vLL) const 
			{ return (vUR.GetX() > m_vPos.GetX() - m_rBoundsX) 
					&& (vUR.GetY() > m_vPos.GetY() - m_rBoundsY)
					&& (vLL.GetX() < m_vPos.GetX() + m_rBoundsX)
					&& (vLL.GetY() < m_vPos.GetY() + m_rBoundsY); }
		inline bool InBoxPoint (const CVector &vUR, const CVector &vLL) const
			{ return (vUR.GetX() > m_vPos.GetX()) 
					&& (vUR.GetY() > m_vPos.GetY())
					&& (vLL.GetX() < m_vPos.GetX())
					&& (vLL.GetY() < m_vPos.GetY()); }
		inline void IncData (const CString &sAttrib, ICCItem *pValue = NULL, ICCItem **retpNewValue = NULL) { m_Data.IncData(sAttrib, pValue, retpNewValue); }
		inline bool IsAscended (void) const { return m_fAscended; }
		bool IsAutoClearDestination (void) { return m_fAutoClearDestination; }
		bool IsAutoClearDestinationOnDestroy (void) { return m_fAutoClearDestinationOnDestroy; }
		bool IsAutoClearDestinationOnDock (void) { return m_fAutoClearDestinationOnDock; }
		bool IsAutomatedWeapon (void) const { return (m_fAutomatedWeapon ? true : false); }
		bool IsBarrier (void) const { return (m_fIsBarrier ? true : false); }
		bool IsCommsMessageValidFrom (CSpaceObject *pSender, int iIndex, CString *retsMsg = NULL, CString *retsKey = NULL);
		bool IsCovering (CSpaceObject *pObj);
		bool IsCreated (void) { return m_fOnCreateCalled; }
		bool IsDestinyTime (int iCycle, int iOffset = 0);
		bool IsDestroyed (void) const { return (m_fDestroyed ? true : false); }
		static bool IsDestroyedInUpdate (void) { return m_bObjDestroyed; }
		bool IsEnemy (const CSpaceObject *pObj) const;
		bool IsEnemyInRange (Metric rMaxRange, bool bIncludeStations = false);
		bool IsEscortingFriendOf (const CSpaceObject *pObj) const;
		bool IsFriend (const CSpaceObject *pObj) const;
		inline bool IsHighlighted (void) { return ((m_iHighlightCountdown != 0) || m_fSelected || m_iHighlightChar); }
		bool IsInDamageCode (void) { return (m_fInDamage ? true : false); }
		bool IsLineOfFireClear (CInstalledDevice *pWeapon, CSpaceObject *pTarget, int iAngle, Metric rDistance = (30.0 * LIGHT_SECOND));
		inline bool IsMarked (void) const { return m_fMarked; }
		inline bool IsMobile (void) const { return !m_fCannotMove; }
		inline bool IsNamed (void) const { return m_fHasName; }
		inline bool IsOutOfPlaneObj (void) const { return m_fOutOfPlaneObj; }
		inline bool IsPaintNeeded (void) { return m_fPaintNeeded; }
		inline bool IsPlayerDestination (void) { return m_fPlayerDestination; }
		inline bool IsPlayerDocked (void) { return m_fPlayerDocked; }
		bool IsPlayerEscortTarget (CSpaceObject *pPlayer = NULL);
		inline bool IsPlayerTarget (void) { return m_fPlayerTarget; }
		inline bool IsSelected (void) { return m_fSelected; }
		inline bool IsShowingDamageBar (void) { return m_fShowDamageBar; }
		inline bool IsShowingDistanceAndBearing (void) { return m_fShowDistanceAndBearing; }
		inline bool IsShowingHighlight (void) { return m_fShowHighlight; }
		bool IsStargateInRange (Metric rMaxRange);
		inline bool IsSubscribedToEvents (CSpaceObject *pObj) { return m_SubscribedObjs.FindObj(pObj); }
		inline bool IsTimeStopped (void) { return m_fTimeStop; }
		bool IsUnderAttack (void);
		void Jump (const CVector &vPos);
		inline void LoadObjReferences (CSystem *pSystem) { m_Data.LoadObjReferences(pSystem); }
		void Move (const CSpaceObjectList &Barriers, Metric rSeconds);
		void NotifyOnNewSystem (CSystem *pNewSystem);
		void NotifyOnObjDestroyed (SDestroyCtx &Ctx);
		void NotifyOnObjDocked (CSpaceObject *pDockTarget);
		inline bool NotifyOthersWhenDestroyed (void) { return (m_fNoObjectDestructionNotify ? false : true); }
		void OnObjDestroyed (const SDestroyCtx &Ctx);
		void Paint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);
		void PaintHighlightText (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx, AlignmentStyles iAlign, CG32bitPixel rgbColor, int *retcyHeight = NULL);
		void PaintMap (CMapViewportCtx &Ctx, CG32bitImage &Dest, int x, int y);
		inline void PaintSRSEnhancements (CG32bitImage &Dest, SViewportPaintCtx &Ctx) { OnPaintSRSEnhancements(Dest, Ctx); }
		inline void Place (const CVector &vPos, const CVector &vVel = NullVector) { m_vPos = vPos; m_vOldPos = vPos; m_vVel = vVel; }
		inline bool PosInBox (const CVector &vUR, const CVector &vLL) const
			{ return (vUR.GetX() > m_vPos.GetX()) 
					&& (vUR.GetY() > m_vPos.GetY())
					&& (vLL.GetX() < m_vPos.GetX())
					&& (vLL.GetY() < m_vPos.GetY()); }
		void Reconned (void);
		void Remove (DestructionTypes iCause, const CDamageSource &Attacker);
		void RemoveAllEventSubscriptions (CSystem *pSystem, TArray<DWORD> *retRemoved = NULL);
		inline void RemoveEventSubscriber (CSpaceObject *pObj) { m_SubscribedObjs.Remove(pObj); }
		void ReportEventError (const CString &sEvent, ICCItem *pError);
		inline void RestartTime (void) { m_fTimeStop = false; }
		inline void SetAscended (bool bAscended = true) { m_fAscended = bAscended; }
		inline void SetAutoClearDestination (void) { m_fAutoClearDestination = true; }
		inline void SetAutoClearDestinationOnDestroy (void) { m_fAutoClearDestinationOnDestroy = true; }
		inline void SetAutoClearDestinationOnDock (void) { m_fAutoClearDestinationOnDock = true; }
		inline void SetAutomatedWeapon (void) { m_fAutomatedWeapon = true; }
		inline void SetData (const CString &sAttrib, const CString &sData) { m_Data.SetData(sAttrib, sData); }
		inline void SetDataFromDataBlock (const CAttributeDataBlock &Block) { m_Data.MergeFrom(Block); }
		inline void SetDataFromXML (CXMLElement *pData) { m_Data.SetFromXML(pData); }
		void SetDataInteger (const CString &sAttrib, int iValue);
		inline void SetDestructionNotify (bool bNotify = true) { m_fNoObjectDestructionNotify = !bNotify; }
		void SetEventFlags (void);
		inline void SetHasOnAttackedEvent (bool bHasEvent) { m_fHasOnAttackedEvent = bHasEvent; }
		inline void SetHasOnDamageEvent (bool bHasEvent) { m_fHasOnDamageEvent = bHasEvent; }
		inline void SetHasInterSystemEvent (bool bHasEvent) { m_fHasInterSystemEvent = bHasEvent; }
		inline void SetHasOnObjDockedEvent (bool bHasEvent) { m_fHasOnObjDockedEvent = bHasEvent; }
		inline void SetHasOnOrdersCompletedEvent (bool bHasEvent) { m_fHasOnOrdersCompletedEvent = bHasEvent; }
		inline void SetHasOnSubordinateAttackedEvent (bool bHasEvent) { m_fHasOnSubordinateAttackedEvent = bHasEvent; }
		inline void SetHighlightChar (char chChar) { m_iHighlightChar = chChar; }
		inline void SetMarked (bool bMarked = true) { m_fMarked = bMarked; }
		inline void SetNamed (bool bNamed = true) { m_fHasName = bNamed; }
		inline void SetObjRefData (const CString &sAttrib, CSpaceObject *pObj) { m_Data.SetObjRefData(sAttrib, pObj); }
		inline void SetOutOfPlaneObj (bool bValue = true) { m_fOutOfPlaneObj = bValue; }
		void SetOverride (CDesignType *pOverride);
		inline void SetPainted (void) { m_fPainted = true; }
		inline void SetPaintNeeded (void) { m_fPaintNeeded = true; }
		inline void SetPlayerDestination (void) { m_fPlayerDestination = true; }
		inline void SetPlayerDocked (void) { m_fPlayerDocked = true; }
		inline void SetPlayerTarget (void) { m_fPlayerTarget = true; }
		inline void SetPos (const CVector &vPos) { m_vPos = vPos; }
		inline bool SetPOVLRS (void)
			{
			if (m_fInPOVLRS)
				return false;

			m_fInPOVLRS = true;
			return true;
			}
		inline void SetSelection (void) { m_fSelected = true; }
		inline void SetShowDamageBar (void) { m_fShowDamageBar = true; }
		inline void SetShowDistanceAndBearing (void) { m_fShowDistanceAndBearing = true; }
		inline void SetShowHighlight (void) { m_fShowHighlight = true; }
		inline void SetVel (const CVector &vVel) { m_vVel = vVel; }
		inline void StopTime (void) { m_fTimeStop = true; }
		inline void UnfreezeControls (void) { m_iControlsFrozen--; }
		void Update (SUpdateCtx &Ctx);
		void UpdateExtended (const CTimeSpan &ExtraTime);
		inline void UpdatePlayer (SUpdateCtx &Ctx) { OnUpdatePlayer(Ctx); }
		inline bool WasPainted (void) const { return m_fPainted; }
		void WriteToStream (IWriteStream *pStream);
		inline void WriteObjRefToStream (CSpaceObject *pObj, IWriteStream *pStream) { GetSystem()->WriteObjRefToStream(pObj, pStream, this); }

		bool MatchesCriteria (SCriteriaMatchCtx &Ctx, const Criteria &Crit);
		static void ParseCriteria (CSpaceObject *pSource, const CString &sCriteria, Criteria *retCriteria);
		static void SetCriteriaSource (Criteria &Crit, CSpaceObject *pSource);

#ifdef DEBUG_VECTOR
		void PaintDebugVector (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);
		inline void SetDebugVector (const CVector &vVector) { m_vDebugVector = vVector; }
#else
		inline void PaintDebugVector (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) { }
		inline void SetDebugVector (const CVector &vVector) { }
#endif
		//	Docking

		virtual CSpaceObject *GetDockedObj (void) { return NULL; }
		virtual CVector GetDockingPortOffset (int iRotation) { return NullVector; }
		virtual void OnDocked (CSpaceObject *pObj) { }
		virtual void OnDockedObjChanged (CSpaceObject *pLocation) { }

		//	Docking Ports

		virtual void CreateRandomDockedShips (IShipGenerator *pGenerator, int iCount = 1) { }
		virtual CDockingPorts *GetDockingPorts (void) { return NULL; }
		virtual bool RequestDock (CSpaceObject *pObj, int iPort = -1) { return false; }
		virtual void Undock (CSpaceObject *pObj) { }

		inline int GetDockingPortCount (void) { CDockingPorts *pPorts = GetDockingPorts(); return (pPorts ? pPorts->GetPortCount(this) : 0); }
		int GetNearestDockPort (CSpaceObject *pRequestingObj, CVector *retvPort = NULL);
		inline int GetOpenDockingPortCount (void) { CDockingPorts *pPorts = GetDockingPorts(); return (pPorts ? (pPorts->GetPortCount(this) - pPorts->GetPortsInUseCount(this)) : 0); }
		inline CSpaceObject *GetShipAtDockingPort (int iPort) { CDockingPorts *pPorts = GetDockingPorts(); return (pPorts ? pPorts->GetPortObj(this, iPort) : NULL); }
		inline bool IsObjDocked (CSpaceObject *pObj) { CDockingPorts *pPorts = GetDockingPorts(); return (pPorts ? pPorts->IsObjDocked(pObj) : false); }
		inline bool IsObjDockedOrDocking (CSpaceObject *pObj) { CDockingPorts *pPorts = GetDockingPorts(); return (pPorts ? pPorts->IsObjDockedOrDocking(pObj) : false); }
		inline void PlaceAtRandomDockPort (CSpaceObject *pObj) { CDockingPorts *pPorts = GetDockingPorts(); if (pPorts) pPorts->DockAtRandomPort(this, pObj); }
		inline bool SupportsDocking (bool bPlayer = false) { return ((!bPlayer || GetDefaultDockScreen() != NULL) && GetDockingPortCount() > 0); }

		//	Dock Screens

		virtual DWORD GetDefaultBkgnd (void) { return 0; }
		virtual CDesignType *GetDefaultDockScreen (CString *retsName = NULL) { return NULL; }
		virtual CXMLElement *GetScreen (const CString &sName);

		//	Fuel

		virtual void ConsumeFuel (Metric rFuel) { }
		virtual bool IsOutOfFuel (void) { return false; }
		virtual void Refuel (Metric Fuel) { }
		virtual void Refuel (const CItem &Fuel) { }

		bool HasFuelItem (void);

		//	Item functions

		EnhanceItemStatus AddItemEnhancement (const CItem &itemToEnhance, CItemType *pEnhancement, int iLifetime, DWORD *retdwID);
		EnhanceItemStatus AddItemEnhancement (CItemListManipulator &ItemList, CItemType *pEnhancement, int iLifetime, DWORD *retdwID);
		void DamageItem (CInstalledDevice *pDevice);
		void DamageItem (CItemListManipulator &ItemList);
		void DisruptItem (CItemListManipulator &ItemList, DWORD dwDuration);
		EnhanceItemStatus EnhanceItem (CItemListManipulator &ItemList, const CItemEnhancement &Mods, DWORD *retdwID = NULL);
		CItem GetItemForDevice (CInstalledDevice *pDevice);
		inline CItemList &GetItemList (void) { return m_ItemList; }
		ICCItem *GetItemProperty (CCodeChainCtx *pCCCtx, const CItem &Item, const CString &sName);
		void RemoveItemEnhancement (const CItem &itemToEnhance, DWORD dwID, bool bExpiredOnly = false);
		void RepairItem (CItemListManipulator &ItemList);
		void SetCursorAtArmor (CItemListManipulator &ItemList, CInstalledArmor *pArmor);
		bool SetCursorAtDevice (CItemListManipulator &ItemList, int iDevSlot);
		bool SetCursorAtDevice (CItemListManipulator &ItemList, CInstalledDevice *pDevice);
		void SetCursorAtRandomItem (CItemListManipulator &ItemList, const CItemCriteria &Crit);
		bool SetItemProperty (const CItem &Item, const CString &sName, ICCItem *pValue, int iCount, CItem *retItem, CString *retsError);
		bool Translate (const CString &sID, ICCItem *pData, CString *retsText);
		bool Translate (const CString &sID, ICCItem *pData, ICCItem **retpResult);
		void UseItem (CItem &Item, CString *retsError = NULL);

		inline void InvalidateItemListAddRemove (void) { m_fItemEventsValid = false; }
		inline void InvalidateItemListState (void) { m_fItemEventsValid = false; }
		void ItemsModified (void);

		//	Overlays

		virtual void AddOverlay (COverlayType *pType, int iPosAngle, int iPosRadius, int iRotation, int iLifetime, DWORD *retdwID = NULL) { if (retdwID) *retdwID = 0; }
		virtual COverlayList *GetOverlays (void) { return NULL; }
		virtual void RemoveOverlay (DWORD dwID) { }

		inline COverlay *GetOverlay (DWORD dwID) { COverlayList *pOverlays = GetOverlays(); return (pOverlays ? pOverlays->GetOverlay(dwID) : NULL); }
		inline const CString &GetOverlayData (DWORD dwID, const CString &sAttrib) { COverlayList *pOverlays = GetOverlays(); return (pOverlays ? pOverlays->GetData(dwID, sAttrib) : NULL_STR); }
		inline void GetOverlayImpact (COverlayList::SImpactDesc *retImpact) { COverlayList *pOverlays = GetOverlays(); if (pOverlays) pOverlays->GetImpact(this, retImpact); else *retImpact = COverlayList::SImpactDesc(); }
		inline void GetOverlayList (TArray<COverlay *> *retList) { COverlayList *pOverlays = GetOverlays(); if (pOverlays) pOverlays->GetList(retList); else retList->DeleteAll(); }
		inline void GetOverlayListOfCommandPaneCounters (TArray<COverlay *> *retList) { COverlayList *pOverlays = GetOverlays(); if (pOverlays) pOverlays->GetListOfCommandPaneCounters(retList); else retList->DeleteAll(); }
		inline CVector GetOverlayPos (DWORD dwID) { COverlayList *pOverlays = GetOverlays(); return (pOverlays ? pOverlays->GetPos(this, dwID) : GetPos()); }
		ICCItem *GetOverlayProperty (CCodeChainCtx *pCCCtx, DWORD dwID, const CString &sName);
		inline int GetOverlayRotation (DWORD dwID) { COverlayList *pOverlays = GetOverlays(); return (pOverlays ? pOverlays->GetRotation(dwID) : -1); }
		inline COverlayType *GetOverlayType(DWORD dwID) { COverlayList *pOverlays = GetOverlays(); return (pOverlays ? pOverlays->GetType(dwID) : NULL); }
		inline void SetOverlayData (DWORD dwID, const CString &sAttribute, const CString &sData) { COverlayList *pOverlays = GetOverlays(); if (pOverlays) pOverlays->SetData(dwID, sAttribute, sData); }
		inline bool SetOverlayEffectProperty (DWORD dwID, const CString &sProperty, ICCItem *pValue) { COverlayList *pOverlays = GetOverlays(); return (pOverlays ? pOverlays->SetEffectProperty(dwID, sProperty, pValue) : false); }
		inline void SetOverlayPos (DWORD dwID, const CVector &vPos) { COverlayList *pOverlays = GetOverlays(); if (pOverlays) pOverlays->SetPos(this, dwID, vPos); }
		inline bool SetOverlayProperty (DWORD dwID, const CString &sName, ICCItem *pValue, CString *retsError) { COverlayList *pOverlays = GetOverlays(); return (pOverlays ? pOverlays->SetProperty(this, dwID, sName, pValue) : false); }
		inline void SetOverlayRotation (DWORD dwID, int iRotation) { COverlayList *pOverlays = GetOverlays(); if (pOverlays) pOverlays->SetRotation(dwID, iRotation); }

		//	Trade functions

		virtual CTradingDesc *AllocTradeDescOverride (void) { return NULL; }
		virtual CTradingDesc *GetTradeDescOverride (void) { return NULL; }
		virtual CCurrencyAndValue GetTradePrice (CSpaceObject *pProvider) { return CCurrencyAndValue(0, GetDefaultEconomy()); }

		void AddBuyOrder (CItemType *pType, const CString &sCriteria, int iPriceAdj);
		void AddSellOrder (CItemType *pType, const CString &sCriteria, int iPriceAdj);
		bool GetArmorInstallPrice (const CItem &Item, DWORD dwFlags, int *retiPrice, CString *retsReason = NULL);
		bool GetArmorRepairPrice (CSpaceObject *pSource, const CItem &Item, int iHPToRepair, DWORD dwFlags, int *retiPrice);
		int GetBuyPrice (const CItem &Item, DWORD dwFlags, int *retiMaxCount = NULL);
		CEconomyType *GetDefaultEconomy (void);
		DWORD GetDefaultEconomyUNID (void);
		bool GetDeviceInstallPrice (const CItem &Item, DWORD dwFlags, int *retiPrice, CString *retsReason = NULL, DWORD *retdwPriceFlags = NULL);
		bool GetDeviceRemovePrice (const CItem &Item, DWORD dwFlags, int *retiPrice, DWORD *retdwPriceFlags = NULL);
		bool GetRefuelItemAndPrice (CSpaceObject *pObjToRefuel, CItemType **retpItemType, int *retiPrice);
		int GetSellPrice (const CItem &Item, DWORD dwFlags);
		bool GetShipBuyPrice (CSpaceObject *pShip, DWORD dwFlags, int *retiPrice);
		bool GetShipSellPrice (CSpaceObject *pShip, DWORD dwFlags, int *retiPrice);
		int GetTradeMaxLevel (ETradeServiceTypes iService);
		void SetTradeDesc (CEconomyType *pCurrency, int iMaxCurrency, int iReplenishCurrency);

		//	Statics

		static int ConvertToCompatibleIndex (const CItem &Item, InstallItemResults iResult);
		static CString ConvertToID (InstallItemResults iResult);

		//	Other virtuals to be overridden

		//	...for all objects
		virtual CBoundaryMarker *AsBoundaryMarker (void) { return NULL; }
		virtual CMissile *AsMissile (void) { return NULL; }
		virtual CMission *AsMission (void) { return NULL; }
		virtual CShip *AsShip (void) { return NULL; }
		virtual CStation *AsStation (void) { return NULL; }
		virtual bool CalcVolumetricShadowLine (SLightingCtx &Ctx, int *retxCenter, int *retyCenter, int *retiWidth, int *retiLength) { return false; }
		virtual bool CanAttack (void) const { return false; }
		virtual bool CanBeDestroyed (void) { return true; }
		virtual bool CanBeHitBy (const DamageDesc &Damage) { return true; }
		virtual bool CanHit (CSpaceObject *pObj) { return true; }
		virtual bool CanMove (void) { return false; }
		virtual bool ClassCanAttack (void) { return false; }
		virtual void CreateStarlightImage (int iStarAngle, Metric rStarDist) { }
		virtual CString DebugCrashInfo (void) { return NULL_STR; }
		virtual bool FindDataField (const CString &sField, CString *retsValue) { return false; }
		virtual AbilityStatus GetAbility (Abilities iAbility) { return ablUninstalled; }
		virtual Categories GetCategory (void) const { return catOther; }
		virtual DWORD GetClassUNID (void) { return 0; }
		virtual const CString &GetGlobalData (const CString &sAttribute) { return NULL_STR; }
		virtual Metric GetGravity (Metric *retrRadius) const { return 0.0; }
		virtual const CObjectImageArray &GetHeroImage (void) const { static CObjectImageArray NullImage; return NullImage; }
		virtual const CObjectImageArray &GetImage (void) const;
        virtual const CCompositeImageSelector &GetImageSelector (void) const { return CCompositeImageSelector::Null(); }
		virtual int GetInteraction (void) { return 100; }
		virtual const COrbit *GetMapOrbit (void) const { return NULL; }
		virtual Metric GetMass (void) { return 0.0; }
		virtual Metric GetMaxSpeed (void) { return (IsMobile() ? LIGHT_SPEED : 0.0); }
		virtual CString GetName (DWORD *retdwFlags = NULL) { if (retdwFlags) *retdwFlags = 0; return LITERAL("unknown object"); }
		virtual CString GetObjClassName (void) { return CONSTLIT("unknown"); }
		virtual CSystem::LayerEnum GetPaintLayer (void) { return CSystem::layerStations; }
		virtual Metric GetParallaxDist (void) { return 0.0; }
		virtual EDamageResults GetPassthroughDefault (void) { return damageNoDamage; }
		virtual int GetPlanetarySize (void) const { return 0; }
		virtual ICCItem *GetProperty (CCodeChainCtx &Ctx, const CString &sName);
		virtual ScaleTypes GetScale (void) const { return scaleFlotsam; }
		virtual CSovereign *GetSovereign (void) const { return NULL; }
		virtual Metric GetStellarMass (void) const { return 0.0; }
		virtual CDesignType *GetType (void) const { return NULL; }
		virtual CDesignType *GetWreckType (void) const { return NULL; }
		virtual bool HasAttribute (const CString &sAttribute) const { return sAttribute.IsBlank(); }
		virtual bool HasSpecialAttribute (const CString &sAttrib) const;
		virtual bool HasVolumetricShadow (void) const { return false; }
		virtual bool IsExplored (void) { return true; }
		virtual bool IsImmutable (void) const { return false; }
		virtual bool IsKnown (void) { return true; }
		virtual bool IsMarker (void) { return false; }
		virtual bool IsMission (void) { return false; }
		virtual bool IsNonSystemObj (void) { return false; }
		virtual bool IsShownInGalacticMap (void) const { return false; }
		virtual bool IsVirtual (void) const { return false; }
		virtual bool IsWreck (void) const { return false; }
		virtual void MarkImages (void) { }
		virtual void OnPlayerChangedShips (CSpaceObject *pOldShip) { }
		virtual void OnSystemCreated (SSystemCreateCtx &CreateCtx) { }
		virtual void OnSystemLoaded (void) { }
		virtual void PaintLRSBackground (CG32bitImage &Dest, int x, int y, const ViewportTransform &Trans) { }
		virtual void PaintLRSForeground (CG32bitImage &Dest, int x, int y, const ViewportTransform &Trans);
		virtual bool PointInObject (const CVector &vObjPos, const CVector &vPointPos) { return false; }
		virtual bool PointInObject (SPointInObjectCtx &Ctx, const CVector &vObjPos, const CVector &vPointPos) { return PointInObject(vObjPos, vPointPos); }
		virtual void PointInObjectInit (SPointInObjectCtx &Ctx) { }
		virtual void RefreshBounds (void) { }
		virtual bool SetAbility (Abilities iAbility, AbilityModifications iModification, int iDuration, DWORD dwOptions) { return false; }
		virtual void SetExplored (bool bExplored = true) { }
		virtual void SetGlobalData (const CString &sAttribute, const CString &sData) { }
		virtual void SetKnown (bool bKnown = true) { }
		virtual void SetName (const CString &sName, DWORD dwFlags = 0) { }
		virtual bool SetProperty (const CString &sName, ICCItem *pValue, CString *retsError);
		virtual void SetSovereign (CSovereign *pSovereign) { }

		virtual bool IsHidden (void) const { return false; }
		virtual bool IsInactive (void) const { return IsSuspended(); }
		virtual bool IsIntangible (void) const { return (IsVirtual() || IsSuspended() || IsDestroyed()); }
		virtual bool IsSuspended (void) const { return false; }

		//	...for active/intelligent objects (ships, stations, etc.)
		virtual bool CanInstallItem (const CItem &Item, int iSlot = -1, InstallItemResults *retiResult = NULL, CString *retsResult = NULL, CItem *retItemToReplace = NULL);
		virtual CurrencyValue ChargeMoney (DWORD dwEconomyUNID, CurrencyValue iValue) { return 0; }
		virtual CurrencyValue CreditMoney (DWORD dwEconomyUNID, CurrencyValue iValue) { return 0; }
		virtual void DamageExternalDevice (int iDev, SDamageCtx &Ctx) { }
		virtual void DeactivateShields (void) { }
		virtual void Decontaminate (void) { }
		virtual void DepleteShields (void) { }
		virtual void DisableDevice (CInstalledDevice *pDevice) { }
		virtual CInstalledArmor *FindArmor (const CItem &Item) { return NULL; }
		virtual CInstalledDevice *FindDevice (const CItem &Item) { return NULL; }
		virtual bool FindDeviceSlotDesc (const CItem &Item, SDeviceDesc *retDesc) { return false; }
		virtual int GetAISettingInteger (const CString &sSetting) { return 0; }
		virtual CString GetAISettingString (const CString &sSetting) { return NULL_STR; }
		virtual CArmorSystem *GetArmorSystem (void) { return NULL; }
		virtual CurrencyValue GetBalance (DWORD dwEconomyUNID) { return 0; }
		virtual Metric GetCargoSpaceLeft (void) { return 1000000.0; }
		virtual int GetCombatPower (void) { return 0; }
		virtual int GetCyberDefenseLevel (void) { return GetLevel(); }
		virtual int GetDamageEffectiveness (CSpaceObject *pAttacker, CInstalledDevice *pWeapon) { return 0; }
		virtual DamageTypes GetDamageType (void) { return damageGeneric; }
		virtual CSpaceObject *GetDestination (void) const { return NULL; }
		virtual CInstalledDevice *GetDevice (int iDev) const { return NULL; }
		virtual int GetDeviceCount (void) const { return 0; }
		virtual CStationType *GetEncounterInfo (void) { return NULL; }
		virtual CSpaceObject *GetEscortPrincipal (void) const { return NULL; }
		virtual int GetLastFireTime (void) const { return 0; }
		virtual int GetLastHitTime (void) const { return 0; }
		virtual int GetLevel (void) const { return 1; }
		virtual int GetMaxPower (void) const { return 0; }
		virtual int GetMaxLightDistance (void) { return 0; }
		virtual CInstalledDevice *GetNamedDevice (DeviceNames iDev) { return NULL; }
		virtual int GetPerception (void) { return perceptNormal; }
		virtual CSpaceObject *GetTarget (CItemCtx &ItemCtx, bool bNoAutoTarget = false) const { return NULL; }
		virtual int GetScore (void) { return 0; }
		virtual int GetShieldLevel (void) { return -1; }
		virtual CG32bitPixel GetSpaceColor (void) { return 0; }
		virtual CString GetStargateID (void) const { return NULL_STR; }
		virtual int GetStealth (void) const { return stealthNormal; }
		virtual int GetVisibleDamage (void) { return 0; }
		virtual bool HasMapLabel (void) { return false; }
		virtual bool IsAngry (void) { return false; }
		virtual bool IsAngryAt (CSpaceObject *pObj) { return IsEnemy(pObj); }
		virtual bool IsBlind (void) { return false; }
		virtual bool IsDisarmed (void) { return false; }
		virtual bool IsIdentified (void) { return true; }
		virtual bool IsMultiHull (void) { return false; }
		virtual bool IsParalyzed (void) { return false; }
		virtual bool IsPlayer (void) const { return false; }
		virtual bool IsRadioactive (void) { return false; }
		virtual bool IsTimeStopImmune (void) { return false; }
		virtual void MakeBlind (int iTickCount = -1) { }
		virtual void MakeParalyzed (int iTickCount = -1) { }
		virtual void MakeRadioactive (void) { }
		virtual void OnComponentChanged (ObjectComponentTypes iComponent) { }
		virtual bool OnDestroyCheck (DestructionTypes iCause, const CDamageSource &Attacker) { return true; }
		virtual void OnDeviceStatus (CInstalledDevice *pDev, CDeviceClass::DeviceNotificationTypes iEvent) { }
		virtual void OnHitByDeviceDamage (void) { }
		virtual void OnHitByDeviceDisruptDamage (DWORD dwDuration) { }
		virtual void OnHitByRadioactiveDamage (SDamageCtx &Ctx) { }
		virtual void OnMissionCompleted (CMission *pMission, bool bSuccess) { }
		virtual void OnObjDamaged (SDamageCtx &Ctx) { }
		virtual void OnObjDestroyedNotify (SDestroyCtx &Ctx) { FireOnObjDestroyed(Ctx); }
		virtual void OnObjLeaveGate (CSpaceObject *pObj) { }
		virtual void OnPlayerObj (CSpaceObject *pPlayer) { }
		virtual void OnStationDestroyed (const SDestroyCtx &Ctx) { }
		virtual void ProgramDamage (CSpaceObject *pHacker, const ProgramDesc &Program) { }
		virtual void SendMessage (CSpaceObject *pSender, const CString &sMsg) { }
		virtual int SetAISettingInteger (const CString &sSetting, int iValue) { return 0; }
		virtual CString SetAISettingString (const CString &sSetting, const CString &sValue) { return NULL_STR; }
		virtual void SetIdentified (bool bIdentified = true) { }
		virtual void SetMapLabelPos (int x, int y) { }
		virtual void UpdateArmorItems (void) { }

		//	...for objects that can bounce
		virtual bool ImageInObject (const CVector &vObjPos, const CObjectImageArray &Image, int iTick, int iRotation, const CVector &vImagePos) { return PointInObject(vObjPos, vImagePos); }
		virtual bool ObjectInObject (const CVector &vObj1Pos, CSpaceObject *pObj2, const CVector &vObj2Pos) { return PointInObject(vObj1Pos, vObj2Pos); }
		virtual void OnBounce (CSpaceObject *pBarrierObj, const CVector &vPos) { }
		virtual void OnObjBounce (CSpaceObject *pObj, const CVector &vPos) { }

		//	...for objects with devices
		virtual bool AbsorbWeaponFire (CInstalledDevice *pWeapon) { return false; }
		virtual CString GetInstallationPhrase (const CItem &Item) const { return NULL_STR; }
		virtual void SetFireDelay (CInstalledDevice *pWeapon, int iDelay = -1) { }

		//	...for beams, missiles, etc.
		virtual void CreateReflection (const CVector &vPos, int iDirection) { }
		virtual void DetonateNow (CSpaceObject *pHit) { }
		virtual CString GetDamageCauseNounPhrase (DWORD dwFlags) { return GetNounPhrase(dwFlags); }
		virtual DestructionTypes GetDamageCauseType (void) { return killedByDamage; }
		virtual CWeaponFireDesc *GetWeaponFireDesc (void) { return NULL; }
		virtual CSpaceObject *GetSource (void) { return NULL; }
		virtual CSpaceObject *GetSecondarySource (void) { return NULL; }

		//	...for ships
		virtual void Behavior (SUpdateCtx &Ctx) { }
		virtual bool FollowsObjThroughGate (CSpaceObject *pLeader = NULL) { return false; }
		virtual CSpaceObject *GetBase (void) const { return NULL; }
		virtual int GetRotation (void) const { return 0; }
		virtual void RepairDamage (int iHitPoints) { }
		virtual void Resume (void) { }
		virtual void Suspend (void) { }

		//	...for stations
		virtual void AddSubordinate (CSpaceObject *pSubordinate) { }
		virtual IShipGenerator *GetRandomEncounterTable (int *retiFrequency = NULL) const { if (retiFrequency) *retiFrequency = 0; return NULL; }
		virtual bool IsAbandoned (void) const { return false; }
		virtual bool IsActiveStargate (void) const { return false; }
        virtual bool IsSatelliteSegmentOf (CSpaceObject *pBase) const { return false; }
		virtual bool IsStargate (void) const { return false; }
		virtual bool RemoveSubordinate (CSpaceObject *pSubordinate) { return false; }
		virtual bool RequestGate (CSpaceObject *pObj);
        virtual bool ShowMapOrbit (void) const { return false; }
        virtual bool ShowStationDamage (void) const { return false; }
		virtual bool SupportsGating (void) { return false; }

		//	...for particle effects
		virtual void SetAttractor (CSpaceObject *pObj) { }

		//	...miscellaneous
		virtual void OnLosePOV (void) { }

	protected:

		//	Virtuals to be overridden
		virtual bool CanBlock (CSpaceObject *pObj) { return false; }
		virtual bool CanBlockShips (void) { return false; }
		virtual bool CanFireOn (CSpaceObject *pObj) { return true; }
		virtual void GateHook (CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate, bool bAscend) { Destroy(removedFromSystem, CDamageSource()); }
		virtual void ObjectDestroyedHook (const SDestroyCtx &Ctx) { }
		virtual void ObjectEnteredGateHook (CSpaceObject *pObjEnteredGate) { }
		virtual void OnAscended (void) { }
		virtual DWORD OnCommunicate (CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2) { return resNoAnswer; }
		virtual EDamageResults OnDamage (SDamageCtx &Ctx) { return damageNoDamage; }
		virtual void OnDestroyed (SDestroyCtx &Ctx) { }
		virtual CSpaceObject *OnGetOrderGiver (void) { return this; }
		virtual void OnItemEnhanced (CItemListManipulator &ItemList) { }
		virtual void OnMove (const CVector &vOldPos, Metric rSeconds) { }
		virtual void OnNewSystem (CSystem *pSystem) { }
		virtual void OnObjEnteredGate (CSpaceObject *pObj, CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate) { }
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) { }
		virtual void OnPaintAnnotations (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) { }
		virtual void OnPaintMap (CMapViewportCtx &Ctx, CG32bitImage &Dest, int x, int y) { }
		virtual void OnPaintSRSEnhancements (CG32bitImage &Dest, SViewportPaintCtx &Ctx) { }
		virtual void OnReadFromStream (SLoadCtx &Ctx) { }
		virtual void OnSetEventFlags (void) { }
		virtual void OnUpdate (SUpdateCtx &Ctx, Metric rSecondsPerTick) { }
		virtual void OnUpdateExtended (const CTimeSpan &ExtraTime) { }
		virtual void OnUpdatePlayer (SUpdateCtx &Ctx) { }
		virtual void OnWriteToStream (IWriteStream *pStream) { }
		virtual bool OrientationChanged (void) { return false; }
		virtual void RevertOrientationChange (void) { }

		//	Helper functions
		void AddEffect (IEffectPainter *pPainter, int xOffset, int yOffset, int iTick = 0, int iRotation = 0);
		void CalcInsideBarrier (void);
		Metric CalculateItemMass (Metric *retrCargoMass = NULL);
		bool CanFireOnObjHelper (CSpaceObject *pObj);
		inline void ClearCannotBeHit (void) { m_fCannotBeHit = false; }
		inline void ClearInDamageCode (void) { m_fInDamage = false; }
		inline void ClearInUpdateCode (void) { m_pObjInUpdate = NULL; m_bObjDestroyed = false; }
		inline void ClearObjReferences (void) { m_Data.OnSystemChanged(NULL); }
		inline void ClearPainted (void) { m_fPainted = false; }
		inline void DisableObjectDestructionNotify (void) { m_fNoObjectDestructionNotify = true; }
		inline const Metric &GetBounds (void) { return m_rBoundsX; }
		CSpaceObject *HitTest (const CVector &vStart, Metric rThreshold, const DamageDesc &Damage, CVector *retvHitPos, int *retiHitDir);
		bool ImagesIntersect (const CObjectImageArray &Image1, int iTick1, int iRotation1, const CVector &vPos1,
				const CObjectImageArray &Image2, int iTick2, int iRotation2, const CVector &vPos2);
		inline bool IsObjectDestructionHooked (void) { return (m_fHookObjectDestruction ? true : false); }
		inline void ItemEnhancementModified (CItemListManipulator &ItemList) { OnItemEnhanced(ItemList); }
		bool MissileCanHitObj (CSpaceObject *pObj, CSpaceObject *pSource, CWeaponFireDesc *pDesc);
		void PaintEffects (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);
		void PaintHighlight (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);
		void PaintTargetHighlight (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);
		inline void SetObjectDestructionHook (void) { m_fHookObjectDestruction = true; }
		inline void SetCannotBeHit (void) { m_fCannotBeHit = true; }
		inline void SetCannotMove (void) { m_fCannotMove = true; }
		inline void SetCanBounce (void) { m_fCanBounce = true; }
		inline void SetBounds (Metric rBounds) { m_rBoundsX = rBounds; m_rBoundsY = rBounds; }
		inline void SetBounds (const RECT &rcRect, Metric rParallaxDist = 1.0)
			{
			m_rBoundsX = Max(1.0, rParallaxDist) * g_KlicksPerPixel * (RectWidth(rcRect) / 2);
			m_rBoundsY = Max(1.0, rParallaxDist) * g_KlicksPerPixel * (RectHeight(rcRect) / 2);
			}
		inline void SetBounds (IEffectPainter *pPainter)
			{
			RECT rcRect;
			pPainter->GetBounds(&rcRect);
			SetBounds(rcRect);
			}
		inline void SetHasGravity (bool bGravity = true) { m_fHasGravity = bGravity; }
		inline void SetIsBarrier (void) { m_fIsBarrier = true; }
		inline void SetInDamageCode (void) { m_fInDamage = true; }
		inline void SetInUpdateCode (void) { m_pObjInUpdate = this; m_bObjDestroyed = false; }
		inline void SetNoFriendlyFire (void) { m_fNoFriendlyFire = true; }
		inline void SetNoFriendlyTarget (void) { m_fNoFriendlyTarget = true; }
		inline void SetNonLinearMove (bool bValue = true) { m_fNonLinearMove = bValue; }
		void UpdateTrade (SUpdateCtx &Ctx, int iInventoryRefreshed);
		void UpdateTradeExtended (const CTimeSpan &ExtraTime);

	private:

		struct SEffectNode
			{
			IEffectPainter *pPainter;
			int xOffset;
			int yOffset;
			int iTick;
			int iRotation;

			SEffectNode *pNext;
			};

		CSpaceObject (void);

		inline void InitItemEvents (void) { m_ItemEvents.Init(this); m_fItemEventsValid = true; }

		CSystem *m_pSystem;						//	Current system
		int m_iIndex;							//	Index in system
		DWORD m_dwID;							//	Universal ID
		int m_iDestiny;							//	Random number 0..DestinyRange-1
		CVector m_vPos;							//	Position of object in system
		CVector m_vVel;							//	Velocity of object
		CVector m_vOldPos;						//	Position last tick
		Metric m_rBoundsX;						//	Object bounds
		Metric m_rBoundsY;						//	Object bounds

		CString m_sHighlightText;				//	Show message text when highlighting
		int m_iHighlightCountdown:16;			//	Frames left to highlight
		int m_iHighlightChar:8;					//	Show character when painting (0 = none)
		int m_iDesiredHighlightChar:8;			//	Desired character

		CItemList m_ItemList;					//	List of items
		CAttributeDataBlock m_Data;				//	Opaque data
		SEffectNode *m_pFirstEffect;			//	List of effects
		CItemEventDispatcher m_ItemEvents;		//	Item event dispatcher
		CDesignType *m_pOverride;				//	Override event handler
		CSpaceObjectList m_SubscribedObjs;		//	List of objects to notify when something happens

		int m_iControlsFrozen:8;				//	Object will not respond to controls
		int m_iSpare:24;

		DWORD m_fHookObjectDestruction:1;		//	Call the object when another object is destroyed
		DWORD m_fNoObjectDestructionNotify:1;	//	Do not call other objects when this one is destroyed
		DWORD m_fCannotBeHit:1;					//	TRUE if it cannot be hit by other objects
		DWORD m_fSelected:1;					//	TRUE if selected
		DWORD m_fInPOVLRS:1;					//	TRUE if object appears in POV's LRS
		DWORD m_fCanBounce:1;					//	TRUE if object can bounce off others
		DWORD m_fIsBarrier:1;					//	TRUE if objects bounce off this object
		DWORD m_fCannotMove:1;					//	TRUE if object cannot move
		
		DWORD m_fNoFriendlyFire:1;				//	TRUE if object cannot hit friendly objects
		DWORD m_fTimeStop:1;					//	TRUE if time has stopped for this object
		DWORD m_fPlayerTarget:1;				//	TRUE if this is a target for the player
		DWORD m_fAutomatedWeapon:1;				//	TRUE if this is a shot from automated weapon
		DWORD m_fHasOnObjDockedEvent:1;			//	TRUE if this object has an OnObjDocked event
		DWORD m_fOnCreateCalled:1;				//	TURE if OnCreate event has already been called
		DWORD m_fNoFriendlyTarget:1;			//	TRUE if object cannot be hit by friends
		DWORD m_fItemEventsValid:1;				//	TRUE if item event dispatcher is up to date
		
		DWORD m_fHasOnDamageEvent:1;			//	TRUE if object has OnDamage event
		DWORD m_fHasOnAttackedEvent:1;			//	TRUE if object has OnAttacked event
		DWORD m_fInDamage:1;					//	TRUE if object is inside Damage call
		DWORD m_fDestroyed:1;					//	TRUE if object is destroyed (but not yet deleted)
		DWORD m_fPlayerDestination:1;			//	TRUE if object is a destination for the player
		DWORD m_fShowDistanceAndBearing:1;		//	TRUE if we should show distance and bearing when a player destination
		DWORD m_fHasInterSystemEvent:1;			//	TRUE if object has OnPlayerEnteredSystem or OnPlayerLeftSystem event
		DWORD m_fAutoClearDestination:1;		//	TRUE if m_fPlayerDestination is cleared when object in SRS

		DWORD m_fHasOnOrdersCompletedEvent:1;	//	TRUE if object has OnOrdersCompleted event
		DWORD m_fPlayerDocked:1;				//	TRUE if player is docked with this object
		DWORD m_fPaintNeeded:1;					//	TRUE if object needs to be painted
		DWORD m_fNonLinearMove:1;				//	TRUE if object updates its position inside OnMove
		DWORD m_fHasName:1;						//	TRUE if object has been named (this is an optimization--it may have false positives)
		DWORD m_fMarked:1;						//	Temporary marker for processing lists (not persistent)
		DWORD m_fAscended:1;					//	TRUE if object is ascended (i.e., stored outside a system)
		DWORD m_fOutOfPlaneObj:1;				//	TRUE if object is out of plane

		DWORD m_fPainted:1;						//	TRUE if we painted the object last tick
		DWORD m_fAutoClearDestinationOnDock:1;	//	TRUE if we should clear the destination when player docks
		DWORD m_fShowHighlight:1;				//	TRUE if we should paint a target highlight in SRS
		DWORD m_fAutoClearDestinationOnDestroy:1;//	TRUE if we should clear the destination when station is destroyed
		DWORD m_fShowDamageBar:1;				//	TRUE if we should show damage bar
		DWORD m_fHasGravity:1;					//	TRUE if object has gravity
		DWORD m_fInsideBarrier:1;				//	TRUE if we got created inside a barrier
		DWORD m_fHasOnSubordinateAttackedEvent:1;	//	TRUE if we have a <OnSubordinateAttacked> event

		DWORD m_dwSpare:24;

#ifdef DEBUG_VECTOR
		CVector m_vDebugVector;			//	Draw a vector
#endif

		//	This is a global variable that is set when we update an object.
		//	We use it to detect when an object gets destroyed inside its
		//	own Update method.
		//
		//	Note: Obviously this only works if object updates take place
		//	on the same thread and if they are not re-entrant. (i.e., can't
		//	call Update on a object from inside the Update of a different
		//	object).

		static CSpaceObject *m_pObjInUpdate;
		static bool m_bObjDestroyed;

		//	Empty list of overlays

		static COverlayList m_NullOverlays;

	friend CObjectClass<CSpaceObject>;
	};

class CGlobalSpaceObject
	{
	public:
		CGlobalSpaceObject (void) : m_dwID(OBJID_NULL), m_pObj(NULL)
			{ }

		inline CGlobalSpaceObject &operator= (CSpaceObject *pObj)
			{
			m_dwID = (pObj ? pObj->GetID() : OBJID_NULL);
			m_pObj = pObj;
			return *this;
			}

		inline CSpaceObject *operator-> (void) { return m_pObj; }
		inline operator CSpaceObject * () const { return m_pObj; }

		inline void CleanUp (void) { m_dwID = OBJID_NULL; m_pObj = NULL; }
		inline DWORD GetID (void) const { return m_dwID; }
		inline CSpaceObject *GetObj (void) { if (IsEmpty() && m_dwID != OBJID_NULL) Resolve(); return m_pObj; }
		inline bool IsEmpty (void) const { return (m_pObj == NULL); }
		void ReadFromStream (SLoadCtx &Ctx);
		void Resolve (void);
		inline void SetID (DWORD dwID) { m_dwID = dwID; m_pObj = NULL; Resolve(); }
		void WriteToStream (IWriteStream *pStream);

	private:
		DWORD m_dwID;						//	Global ID of space object
		CSpaceObject *m_pObj;				//	Object pointer (may be NULL)
	};

class CObjectTracker
	{
	public:
		struct SObjEntry
			{
            SObjEntry (void) :
                    fKnown(false),
                    fShowDestroyed(false),
                    fShowInMap(false),
                    fFriendly(false),
                    fEnemy(false)
                { }

			CTopologyNode *pNode;
			CDesignType *pType;
			DWORD dwObjID;
			CString sName;
			DWORD dwNameFlags;
            CCompositeImageSelector ImageSel;
            CString sNotes;

            DWORD fKnown:1;
            DWORD fShowDestroyed:1;
            DWORD fShowInMap:1;
            DWORD fFriendly:1;              //  If neither friend or enemy, then neutral
            DWORD fEnemy:1;
			};

        struct SBackgroundObjEntry
            {
            CStationType *pType;
            CVector vPos;
            CCompositeImageSelector *pImageSel;
            };

		~CObjectTracker (void);

		void Delete (CSpaceObject *pObj);
		void DeleteAll (void);
		bool Find (const CString &sNodeID, const CDesignTypeCriteria &Criteria, TArray<SObjEntry> *retResult);
        void GetGalacticMapObjects (CTopologyNode *pNode, TArray<SObjEntry> &Results) const;
        void GetSystemBackgroundObjects (CTopologyNode *pNode, TSortMap<Metric, SBackgroundObjEntry> &Results) const;
        void GetSystemStarObjects (CTopologyNode *pNode, TArray<SBackgroundObjEntry> &Results) const;
        const TArray<COrbit> &GetSystemOrbits (CTopologyNode *pNode) const;
		void Insert (CSpaceObject *pObj);
		void ReadFromStream (SUniverseLoadCtx &Ctx);
        void Refresh (CSystem *pSystem);
		void WriteToStream (IWriteStream *pStream);

	private:
		struct SObjExtra
			{
            SObjExtra (void)
                { }

			CString sName;
			DWORD dwNameFlags;
            CCompositeImageSelector ImageSel;
            CString sNotes;
			};

        struct SObjBasics
            {
            SObjBasics (void) :
                    fKnown(false),
                    fShowDestroyed(false),
                    fShowInMap(false),
                    fFriendly(false),
                    fEnemy(false),
                    pExtra(NULL)
                { }

            SObjBasics (const SObjBasics &Src) { Copy(Src); }
            ~SObjBasics (void) { CleanUp(); }

            SObjBasics &operator= (const SObjBasics &Src)
                {
                CleanUp();
                Copy(Src);
                return *this;
                }

            void CleanUp (void)
                {
                DeleteExtra();
                }

            void Copy (const SObjBasics &Src)
                {
                vPos = Src.vPos;

                fKnown = Src.fKnown;
                fShowDestroyed = Src.fShowDestroyed;
                fShowInMap = Src.fShowInMap;
                fFriendly = Src.fFriendly;
                fEnemy = Src.fEnemy;

                if (Src.pExtra)
                    pExtra = new SObjExtra(*Src.pExtra);
                else
                    pExtra = NULL;
                }

            void DeleteExtra (void)
                {
                if (pExtra)
                    {
                    delete pExtra;
                    pExtra = NULL;
                    }
                }

            SObjExtra &SetExtra (void)
                {
                if (pExtra == NULL)
                    pExtra = new SObjExtra;

                return *pExtra;
                }

            CVector vPos;                   //  Position of object in its system

            DWORD fKnown:1;                 //  TRUE if player knows about this obj
            DWORD fShowDestroyed:1;         //  TRUE if we need to paint station as destroyed
            DWORD fShowInMap:1;             //  TRUE if we can dock with the obj
            DWORD fFriendly:1;              //  If neither friend or enemy, then neutral
            DWORD fEnemy:1;
            DWORD fSpare6:1;
            DWORD fSpare7:1;
            DWORD fSpare8:1;

            DWORD dwSpare:24;

            SObjExtra *pExtra;
            };

		struct SObjList
			{
			CTopologyNode *pNode;
			CDesignType *pType;
			TSortMap<DWORD, SObjBasics> Objects;
			};

        struct SNodeData
            {
            TArray<SObjList *> ObjLists;
            TArray<COrbit> Orbits;
            };

		bool AccumulateEntries (TArray<SObjList *> &Table, const CDesignTypeCriteria &Criteria, TArray<SObjEntry> *retResult);
        void AccumulateEntry (const SObjList &ObjList, DWORD dwObjID, const SObjBasics &ObjData, TArray<SObjEntry> &Results) const;
        bool Find (CTopologyNode *pNode, CSpaceObject *pObj, SObjBasics **retpObjData = NULL) const;
        bool Find (SNodeData *pNodeData, CSpaceObject *pObj, SObjBasics **retpObjData = NULL) const;
		SObjList *GetList (CSpaceObject *pObj) const;
		SObjList *GetList (CTopologyNode *pNode, CDesignType *pType) const;
        void Refresh (CSpaceObject *pObj, SObjBasics *pObjData, CSpaceObject *pPlayer);
		SObjList *SetList (CSpaceObject *pObj);
		SObjList *SetList (CTopologyNode *pNode, CDesignType *pType);
		SObjList *SetList (SNodeData *pNodeData, CTopologyNode *pNode, CDesignType *pType);

		TArray<SObjList *> m_AllLists;
		TSortMap<CString, SNodeData> m_ByNode;
	};

class CObjectStats
	{
	public:
		struct SEntry
			{
			SEntry (void) :
					iPlayerMissionsGiven(0)
				{ }

			int iPlayerMissionsGiven;		//	Number of player missions given
			};

		const SEntry &GetEntry (DWORD dwObjID) const;
		SEntry &GetEntryActual (DWORD dwObjID);
		void ReadFromStream (SUniverseLoadCtx &Ctx);
		void WriteToStream (IWriteStream *pStream);

	private:
		TSortMap<DWORD, SEntry> m_Stats;	//	Indexed by ObjID

		static SEntry m_Null;
	};

class CAscendedObjectList
	{
	public:
		~CAscendedObjectList (void) { CleanUp(); }

		inline void DeleteAll (void) { CleanUp(); }
		inline void Insert (CSpaceObject *pObj) { m_List.Insert(pObj); }
		inline int GetCount (void) const { return m_List.GetCount(); }
		inline CSpaceObject *GetObj (int iIndex) const { return m_List[iIndex]; }
		inline bool IsEmpty (void) const { return (m_List.GetCount() == 0); }
		void ReadFromStream (SLoadCtx &Ctx);
		CSpaceObject *RemoveByID (DWORD dwID);
		void WriteToStream (IWriteStream *pStream);

	private:
		void CleanUp (void);

		TArray<CSpaceObject *> m_List;
	};

//	Fractal Texture Library ----------------------------------------------------

class CFractalTextureLibrary
	{
	public:
		enum ETextureTypes
			{
			typeNone,

			typeBoilingClouds,
			typeExplosion,
			};

		CFractalTextureLibrary (void) :
				m_bInitialized(false)
			{ }

		const CG8bitImage &GetTexture (ETextureTypes iType, int iFrame) const;
		int GetTextureCount (ETextureTypes iType) const;
		inline int GetTextureIndex (ETextureTypes iType, Metric rFraction) const
			{
			int iMaxFrames = GetTextureCount(iType);
			return Max(0, Min((int)(rFraction * iMaxFrames), iMaxFrames - 1));
			}

		void Init (void);

	private:
		void InitExplosionTextures (void);

		bool m_bInitialized;
		TArray<CG8bitImage> m_BoilingTextures;
		TArray<CG8bitImage> m_ExplosionTextures;
	};

//	IListData implementation ---------------------------------------------------

class CItemListWrapper : public IListData
	{
	public:
		CItemListWrapper (CSpaceObject *pSource);
		CItemListWrapper (CItemList &ItemList);

		virtual void DeleteAtCursor (int iCount) { m_ItemList.DeleteAtCursor(iCount); if (m_pSource) m_pSource->InvalidateItemListAddRemove(); }
		virtual int GetCount (void) { return m_ItemList.GetCount(); }
		virtual int GetCursor (void) { return m_ItemList.GetCursor(); }
		virtual const CItem &GetItemAtCursor (void) { return m_ItemList.GetItemAtCursor(); }
		virtual CItemListManipulator &GetItemListManipulator (void) { return m_ItemList; }
		virtual CSpaceObject *GetSource (void) { return m_pSource; }
		virtual bool IsCursorValid (void) { return m_ItemList.IsCursorValid(); }
		virtual bool MoveCursorBack (void) { return m_ItemList.MoveCursorBack(); }
		virtual bool MoveCursorForward (void) { return m_ItemList.MoveCursorForward(); }
		virtual void ResetCursor (void) { m_ItemList.Refresh(CItem()); }
		virtual void SetCursor (int iCursor) { m_ItemList.SetCursor(iCursor); }
		virtual void SetFilter (const CItemCriteria &Filter) { m_ItemList.SetFilter(Filter); }
		virtual void SyncCursor (void) { m_ItemList.SyncCursor(); }

	private:
		CSpaceObject *m_pSource;
		CItemListManipulator m_ItemList;
	};

class CListWrapper : public IListData
	{
	public:
		CListWrapper (CCodeChain *pCC, ICCItem *pList);
		virtual ~CListWrapper (void) { m_pList->Discard(m_pCC); }

		virtual int GetCount (void) { return m_pList->GetCount(); }
		virtual int GetCursor (void) { return m_iCursor; }
		virtual CString GetDescAtCursor (void);
		virtual ICCItem *GetEntryAtCursor (CCodeChain &CC);
		virtual CString GetTitleAtCursor (void);
		virtual bool IsCursorValid (void) { return (m_iCursor != -1); }
		virtual bool MoveCursorBack (void);
		virtual bool MoveCursorForward (void);
		virtual void PaintImageAtCursor (CG32bitImage &Dest, int x, int y);
		virtual void ResetCursor (void) { m_iCursor = -1; }
		virtual void SetCursor (int iCursor) { m_iCursor = Min(Max(-1, iCursor), GetCount() - 1); }
		virtual void SyncCursor (void);

	private:
		CCodeChain *m_pCC;
		ICCItem *m_pList;

		int m_iCursor;
	};

//	Implementations ------------------------------------------------------------

#include "TSEDeviceClassesImpl.h"
#include "TSESpaceObjectsImpl.h"
#include "TSEMissions.h"
#include "TSEPlayer.h"

//	The Universe ---------------------------------------------------------------
//
//	CREATING THE UNIVERSE
//
//	1.	Call Init() to initialize the universe with the appropriate adventure
//		and extensions. This will call Bind on all design types and initialize
//		any dynamic types.
//		[This may take a while, so it should be called on a background thread.]
//
//	2.	Call InitAdventure(), which calls OnGameStart on the adventure. This is
//		required if you need to show the crawl screen and text, otherwise it may
//		be skipped (e.g., for TransData).
//		[This may be called synchronously.]
///
//	2.	Call InitGame() to initialize the topology, etc.
//		[This should be called on a background thread.]
//
//	3.	Call StartGame(true) to start the game running.
//		[This may be called	synchronously.]

enum EInitFlags
	{
	flagNoResources			= 0x00000001,
	flagNoVersionCheck		= 0x00000002,
	flagNewGame				= 0x00000004,
	};

enum EStorageScopes
	{
	storeUnknown				= -1,

	storeDevice					= 0,
	storeServiceExtension		= 1,
	storeServiceUser			= 2,
	};

class CSFXOptions
	{
	public:
		enum ESFXQuality
			{
			sfxUnknown			= -1,
			sfxMinimum			= 0,
			sfxStandard			= 1,
			sfxMaximum			= 2,
			};

		CSFXOptions (void) { SetSFXQuality(sfxMaximum); }

		inline BYTE GetHUDOpacity (void) const { return (m_bHUDTransparent ? 200 : 255); }
		inline bool Is3DSystemMapEnabled (void) const { return m_b3DSystemMap; }
		inline bool IsManeuveringEffectEnabled (void) const { return m_bManeuveringEffect; }
		inline bool IsSpaceBackgroundEnabled (void) const { return m_bSpaceBackground; }
		inline bool IsStargateTravelEffectEnabled (void) const { return m_bStargateTravelEffect; }
		inline bool IsStarGlowEnabled (void) const { return m_bStarGlow; }
		inline bool IsStarshineEnabled (void) const { return m_bStarshine; }
		inline void Set3DSystemMapEnabled (bool bEnabled = true) { m_b3DSystemMap = bEnabled; }
		inline void SetManeuveringEffectEnabled (bool bEnabled = true) { m_bManeuveringEffect = bEnabled; }
		void SetSFXQuality (ESFXQuality iQuality);
		void SetSFXQualityAuto (void);

	private:
		ESFXQuality m_iQuality;

		bool m_b3DSystemMap;				//	3D effect on system map
		bool m_bHUDTransparent;				//	HUD has transparency effect
		bool m_bManeuveringEffect;			//	Show maneuvering thruster effects
		bool m_bSpaceBackground;			//	Show system image background
		bool m_bStargateTravelEffect;		//	Show effect when changing systems
		bool m_bStarGlow;					//	Show star glow in system map
		bool m_bStarshine;					//	Show starshine effect
	};

class CUniverse
	{
	public:
		class IHost
			{
			public:
				virtual void ConsoleOutput (const CString &sLine) { }
				virtual IPlayerController *CreatePlayerController (void) { return NULL; }
				virtual IShipController *CreateShipController (const CString &sController) { return NULL; }
				virtual void DebugOutput (const CString &sLine) { }
				virtual void GameOutput (const CString &sLine) { }
				virtual const CG16bitFont *GetFont (const CString &sFont) { return NULL; }
			};

		class INotifications
			{
			public:
				virtual void OnObjDestroyed (SDestroyCtx &Ctx) { }
			};

		struct SInitDesc
			{
			SInitDesc (void) :
					pHost(NULL),
					bInLoadGame(false),
					bNoResources(false),
					bNoReload(false),
					bDebugMode(false),
					bDefaultExtensions(false),
					bForceTDB(false),
                    bNoCollectionLoad(false),
					pAdventure(NULL),
					dwAdventure(0)
				{ }

			IHost *pHost;					//	Host
			TArray<SPrimitiveDefTable> CCPrimitives;	//	Additional primitives to define
			CString sFilespec;				//	Filespec of main TDB/XML file.
			CString sSourceFilespec;		//	Filespec of main source XML file (for debugging).
			CString sCollectionFolder;		//	If non-blank, use this as Collection folder (and remember it)
			TArray<CString> ExtensionFolders;	//	Add these as extension folders.

			//	Options

			bool bInLoadGame;				//	We're loading a game, which means don't reset dynamic types
			bool bNoResources;				//	If TRUE, do not bother loaded images
			bool bNoReload;					//	If TRUE, do not reload extensions
			bool bDebugMode;				//	Initialize in debug mode
			bool bDefaultExtensions;		//	If TRUE, we include all appropriate extensions
			bool bForceTDB;					//	If TRUE, use Transcendence.tdb even if XMLs exist
            bool bNoCollectionLoad;         //  If TRUE, don't load collection (only in debug mode)

			//	Adventure to bind to (choose one, in order)

			CExtension *pAdventure;			//	If not NULL, bind to this adventure
			DWORD dwAdventure;				//	It not 0, bind to this adventure
			CString sAdventureFilespec;		//	If not empty, bind to this adventure
			TArray<DWORD> ExtensionUNIDs;	//	Extensions to use

			//	Extension list

			TArray<CExtension *> Extensions;
			};

		enum ENamedFonts
			{
			fontMapLabel =				0,	//	Font for map labels
			fontSign =					1,	//	Font for signs
			fontSRSObjName =			2,	//	Font for name of objects in SRS
			fontSRSMessage =			3,	//	Font for comms messages from objects in SRS
			fontSRSObjCounter =			4,	//	Font for counter labels

			fontCount =					5,
			};

		CUniverse (void);
		virtual ~CUniverse (void);

		ALERROR Init (SInitDesc &Ctx, CString *retsError);
		ALERROR InitAdventure (IPlayerController *pPlayer, CString *retsError);
		ALERROR InitGame (DWORD dwStartingMap, CString *retsError);
		ALERROR LoadFromStream (IReadStream *pStream, DWORD *retdwSystemID, DWORD *retdwPlayerID, CString *retsError);
		void StartGame (bool bNewGame);

		inline void AddAscendedObj (CSpaceObject *pObj) { m_AscendedObjects.Insert(pObj); }
		inline ALERROR AddDynamicType (CExtension *pExtension, DWORD dwUNID, ICCItem *pSource, bool bNewGame, CString *retsError) { return m_Design.AddDynamicType(pExtension, dwUNID, pSource, bNewGame, retsError); }
		void AddEvent (CTimedEvent *pEvent);
		void AddSound (DWORD dwUNID, int iChannel);
		inline void AddTimeDiscontinuity (const CTimeSpan &Duration) { m_Time.AddDiscontinuity(m_iTick++, Duration); }
		ALERROR AddStarSystem (CTopologyNode *pTopology, CSystem *pSystem);
		inline bool CancelEvent (CSpaceObject *pObj, bool bInDoEvent = false) { return m_Events.CancelEvent(pObj, bInDoEvent); }
		inline bool CancelEvent (CSpaceObject *pObj, const CString &sEvent, bool bInDoEvent = false) { return m_Events.CancelEvent(pObj, sEvent, bInDoEvent); }
		inline bool CancelEvent (CDesignType *pType, const CString &sEvent, bool bInDoEvent = false) { return m_Events.CancelEvent(pType, sEvent, bInDoEvent); }
		ALERROR CreateEmptyStarSystem (CSystem **retpSystem);
		inline DWORD CreateGlobalID (void) { return m_dwNextID++; }
		ALERROR CreateMission (CMissionType *pType, CSpaceObject *pOwner, ICCItem *pCreateData, CMission **retpMission, CString *retsError);
		ALERROR CreateRandomItem (const CItemCriteria &Crit, 
								  const CString &sLevelFrequency,
								  CItem *retItem);
		ALERROR CreateRandomMission (const TArray<CMissionType *> &Types, CSpaceObject *pOwner, ICCItem *pCreateData, CMission **retpMission, CString *retsError);
		IShipController *CreateShipController (const CString &sAI);
		ALERROR CreateStarSystem (const CString &sNodeID, CSystem **retpSystem, CString *retsError = NULL, CSystemCreateStats *pStats = NULL);
		ALERROR CreateStarSystem (CTopologyNode *pTopology, CSystem **retpSystem, CString *retsError = NULL, CSystemCreateStats *pStats = NULL);
		void DestroySystem (CSystem *pSystem);
		inline CMission *FindMission (DWORD dwID) const { return m_AllMissions.GetMissionByID(dwID); }
		CSpaceObject *FindObject (DWORD dwID);
		inline void FireOnGlobalPaneInit (void *pScreen, CDesignType *pRoot, const CString &sScreen, const CString &sPane) { m_Design.FireOnGlobalPaneInit(pScreen, pRoot, sScreen, sPane); }
		inline void FireOnGlobalPlayerChangedShips (CSpaceObject *pOldShip) { m_Design.FireOnGlobalPlayerChangedShips(pOldShip); }
		inline void FireOnGlobalPlayerEnteredSystem (void) { m_Design.FireOnGlobalPlayerEnteredSystem(); }
		inline void FireOnGlobalPlayerLeftSystem (void) { m_Design.FireOnGlobalPlayerLeftSystem(); }
		inline void FireOnGlobalSystemCreated (SSystemCreateCtx &SysCreateCtx) { m_Design.FireOnGlobalSystemCreated(SysCreateCtx); }
		inline void FireOnGlobalUniverseCreated (void) { m_Design.FireOnGlobalUniverseCreated(); }
		inline void FireOnGlobalUniverseLoad (void) { m_Design.FireOnGlobalUniverseLoad(); }
		inline void FireOnGlobalUniverseSave (void) { m_Design.FireOnGlobalUniverseSave(); }
		void FlushStarSystem (CTopologyNode *pTopology);
		void GenerateGameStats (CGameStats &Stats);
		inline void GetAllAdventures (TArray<CExtension *> *retList) { CString sError; m_Extensions.ComputeAvailableAdventures((m_bDebugMode ? CExtensionCollection::FLAG_DEBUG_MODE : 0), retList, &sError); }
		const CDamageAdjDesc *GetArmorDamageAdj (int iLevel) const;
		inline CAdventureDesc *GetCurrentAdventureDesc (void) { return m_pAdventure; }
		void GetCurrentAdventureExtensions (TArray<DWORD> *retList);
		CMission *GetCurrentMission (void);
		inline const CDisplayAttributeDefinitions &GetAttributeDesc (void) const { return m_Design.GetDisplayAttributes(); }
		inline CGImageCache &GetDynamicImageLibrary (void) { return m_DynamicImageLibrary; }
		inline CTimeSpan GetElapsedGameTime (void) { return m_Time.GetElapsedTimeAt(m_iTick); }
		inline CTimeSpan GetElapsedGameTimeAt (int iTick) { return m_Time.GetElapsedTimeAt(iTick); }
		inline CExtensionCollection &GetExtensionCollection (void) { return m_Extensions; }
		CString GetExtensionData (EStorageScopes iScope, DWORD dwExtension, const CString &sAttrib);
		CTopologyNode *GetFirstTopologyNode (void);
		inline const CG16bitFont *GetFont (const CString &sFont) { return m_pHost->GetFont(sFont); }
		inline CFractalTextureLibrary &GetFractalTextureLibrary (void) { return m_FractalTextureLibrary; }
        inline CObjectTracker &GetGlobalObjects (void) { return m_Objects; }
		inline IHost *GetHost (void) const { return m_pHost; }
		inline CMission *GetMission (int iIndex) { return m_AllMissions.GetMission(iIndex); }
		inline int GetMissionCount (void) const { return m_AllMissions.GetCount(); }
		void GetMissions (CSpaceObject *pSource, const CMission::SCriteria &Criteria, TArray<CMission *> *retList);
		inline const CG16bitFont &GetNamedFont (ENamedFonts iFont) { return *m_FontTable[iFont]; }
		inline const CObjectStats::SEntry &GetObjStats (DWORD dwObjID) const { return m_ObjStats.GetEntry(dwObjID); }
		inline CObjectStats::SEntry &GetObjStatsActual (DWORD dwObjID) { return m_ObjStats.GetEntryActual(dwObjID); }
		void GetRandomLevelEncounter (int iLevel, CDesignType **retpType, IShipGenerator **retpTable, CSovereign **retpBaseSovereign);
		inline CString GetResourceDb (void) { return m_sResourceDb; }
		inline CCriticalSection &GetSem (void) { return m_cs; }
		inline CSFXOptions &GetSFXOptions (void) { return m_SFXOptions; }
		const CDamageAdjDesc *GetShieldDamageAdj (int iLevel) const;
		inline CSoundMgr *GetSoundMgr (void) { return m_pSoundMgr; }
		DWORD GetSoundUNID (int iChannel);
		inline bool InDebugMode (void) { return m_bDebugMode; }
		inline void InitEntityResolver (CExtension *pExtension, CEntityResolverList *retResolver) { m_Extensions.InitEntityResolver(pExtension, (InDebugMode() ? CExtensionCollection::FLAG_DEBUG_MODE : 0), retResolver); }
		inline bool InResurrectMode (void) { return m_bResurrectMode; }
		bool IsGlobalResurrectPending (CDesignType **retpType);
		inline bool IsRegistered (void) { return m_bRegistered; }
		bool IsStatsPostingEnabled (void);
		inline void NotifyOnObjDeselected (CSpaceObject *pObj) { pObj->FireOnDeselected(); }
		void NotifyOnObjDestroyed (SDestroyCtx &Ctx);
		inline void NotifyOnObjSelected (CSpaceObject *pObj) { pObj->FireOnSelected(); }
		inline ALERROR LoadNewExtension (const CString &sFilespec, const CIntegerIP &FileDigest, CString *retsError) { return m_Extensions.LoadNewExtension(sFilespec, FileDigest, retsError); }
		inline bool LogImageLoad (void) const { return (m_iLogImageLoad == 0); }
		void PlaySound (CSpaceObject *pSource, int iChannel);
		void PutPlayerInSystem (CShip *pPlayerShip, const CVector &vPos, CTimedEventList &SavedEvents);
		void RefreshCurrentMission (void);
		inline void RegisterForNotifications (INotifications *pSubscriber) { m_Subscribers.Insert(pSubscriber); }
		ALERROR Reinit (void);
		inline CSpaceObject *RemoveAscendedObj (DWORD dwObjID) { return m_AscendedObjects.RemoveByID(dwObjID); }
		ALERROR SaveDeviceStorage (void);
		ALERROR SaveToStream (IWriteStream *pStream);
		void SetCurrentSystem (CSystem *pSystem);
		inline void SetDebugMode (bool bDebug = true) { m_bDebugMode = bDebug; }
		bool SetExtensionData (EStorageScopes iScope, DWORD dwExtension, const CString &sAttrib, const CString &sData);
		void SetNewSystem (CSystem *pSystem, CShip *pPlayerShip, CSpaceObject *pPOV);
		void SetPOV (CSpaceObject *pPOV);
		void SetPlayerShip (CSpaceObject *pPlayer);
		inline void SetRegistered (bool bRegistered = true) { m_bRegistered = bRegistered; }
		inline void SetRegisteredExtensions (const CMultiverseCollection &Catalog, TArray<CMultiverseCatalogEntry *> *retNotFound) { m_Extensions.SetRegisteredExtensions(Catalog, retNotFound); }
		inline void SetResurrectMode (bool bResurrect = true) { m_bResurrectMode = bResurrect; }
		inline void SetSound (bool bSound = true) { m_bNoSound = !bSound; }
		inline void SetSoundMgr (CSoundMgr *pSoundMgr) { m_pSoundMgr = pSoundMgr; }
		void StartGameTime (void);
		CTimeSpan StopGameTime (void);
		inline void UnregisterForNotifications (INotifications *pSubscriber) { m_Subscribers.DeleteValue(pSubscriber); }
		static CString ValidatePlayerName (const CString &sName);

		CArmorClass *FindArmor (DWORD dwUNID);
		inline CCompositeImageType *FindCompositeImageType (DWORD dwUNID) { return CCompositeImageType::AsType(m_Design.FindEntry(dwUNID)); }
		CEffectCreator *FindDefaultFireEffect (DamageTypes iDamage);
		CEffectCreator *FindDefaultHitEffect (DamageTypes iDamage);
		inline CDesignType *FindDesignType (DWORD dwUNID) { return m_Design.FindEntry(dwUNID); }
		CDeviceClass *FindDeviceClass (DWORD dwUNID);
		inline CEconomyType *FindEconomyType (const CString &sName) { return m_Design.FindEconomyType(sName); }
		inline CEffectCreator *FindEffectType (DWORD dwUNID) { return CEffectCreator::AsType(m_Design.FindEntry(dwUNID)); }
		inline CShipTable *FindEncounterTable (DWORD dwUNID) { return CShipTable::AsType(m_Design.FindEntry(dwUNID)); }
		bool FindExtension (DWORD dwUNID, DWORD dwRelease, CExtension **retpExtension = NULL) { return m_Extensions.FindBestExtension(dwUNID, dwRelease, (InDebugMode() ? CExtensionCollection::FLAG_DEBUG_MODE : 0), retpExtension); }
		inline CGenericType *FindGenericType (DWORD dwUNID) { return CGenericType::AsType(m_Design.FindEntry(dwUNID)); }
		inline CItemTable *FindItemTable (DWORD dwUNID) { return CItemTable::AsType(m_Design.FindEntry(dwUNID)); }
		inline CItemType *FindItemType (DWORD dwUNID) { return CItemType::AsType(m_Design.FindEntry(dwUNID)); }
		inline CMissionType *FindMissionType (DWORD dwUNID) { return CMissionType::AsType(m_Design.FindEntry(dwUNID)); }
		inline COverlayType *FindOverlayType(DWORD dwUNID) { return COverlayType::AsType(m_Design.FindEntry(dwUNID)); }
		inline CPower *FindPower (DWORD dwUNID) { return CPower::AsType(m_Design.FindEntry(dwUNID)); }
		inline CDockScreenType *FindSharedDockScreen (DWORD dwUNID) { return CDockScreenType::AsType(m_Design.FindEntry(dwUNID)); }
		inline CShipClass *FindShipClass (DWORD dwUNID) { return CShipClass::AsType(m_Design.FindEntry(dwUNID)); }
		CShipClass *FindShipClassByName (const CString &sName);
		inline COverlayType *FindShipEnergyFieldType(DWORD dwUNID) { return COverlayType::AsType(m_Design.FindEntry(dwUNID)); }
		inline int FindSound (DWORD dwUNID) { CObject *pObj; if (!FindByUNID(m_Sounds, dwUNID, &pObj)) return -1; return (int)pObj; }
		inline CSoundType *FindSoundType (DWORD dwUNID) { return CSoundType::AsType(m_Design.FindEntry(dwUNID)); }
		inline CSovereign *FindSovereign (DWORD dwUNID) const { return CSovereign::AsType(m_Design.FindEntry(dwUNID)); }
		inline CSpaceEnvironmentType *FindSpaceEnvironment (DWORD dwUNID) { return CSpaceEnvironmentType::AsType(m_Design.FindEntry(dwUNID)); }
		inline CStationType *FindStationType (DWORD dwUNID) { return CStationType::AsType(m_Design.FindEntry(dwUNID)); }
		inline CXMLElement *FindSystemFragment (const CString &sName, CSystemTable **retpTable = NULL) { return m_Design.FindSystemFragment(sName, retpTable); }
		inline CSystemType *FindSystemType (DWORD dwUNID) { return CSystemType::AsType(m_Design.FindEntry(dwUNID)); }
		inline CTopologyNode *FindTopologyNode (const CString &sName) { return m_Topology.FindTopologyNode(sName); }
		CWeaponFireDesc *FindWeaponFireDesc (const CString &sName);
		inline CCodeChain &GetCC (void) { return m_CC; }
		inline CTopologyNode *GetCurrentTopologyNode (void) { return (m_pCurrentSystem ? m_pCurrentSystem->GetTopology() : NULL); }
		inline CSystem *GetCurrentSystem (void) { return m_pCurrentSystem; }
		inline int GetPaintTick (void) { return m_iPaintTick; }
		inline CSpaceObject *GetPOV (void) const { return m_pPOV; }
		inline IPlayerController *GetPlayer (void) const { return m_pPlayer; }
		GenomeTypes GetPlayerGenome (void) const;
		CString GetPlayerName (void) const;
		inline CSpaceObject *GetPlayerShip (void) const { return m_pPlayerShip; }
		CSovereign *GetPlayerSovereign (void) const;
		inline int GetTicks (void) { return m_iTick; }

		inline void ClearLibraryBitmapMarks (void) { m_Design.ClearImageMarks(); m_DynamicImageLibrary.ClearMarks();  }
		void GarbageCollectLibraryBitmaps (void);
		inline CObjectImage *FindLibraryImage (DWORD dwUNID) { return CObjectImage::AsType(m_Design.FindEntry(dwUNID)); }
		inline CG32bitImage *GetLibraryBitmap (DWORD dwUNID, DWORD dwFlags = 0) { return m_Design.GetImage(dwUNID, dwFlags); }
		inline CG32bitImage *GetLibraryBitmapCopy (DWORD dwUNID) { return m_Design.GetImage(dwUNID, CDesignCollection::FLAG_IMAGE_COPY); }
		inline void MarkLibraryBitmaps (void) { m_Design.MarkGlobalImages(); if (m_pCurrentSystem) m_pCurrentSystem->MarkImages(); }
		inline void SweepLibraryBitmaps (void) { m_Extensions.SweepImages(); m_Design.SweepImages(); m_DynamicImageLibrary.Sweep();  }

		inline CDesignCollection &GetDesignCollection (void) { return m_Design; }
		inline CDesignType *GetDesignType (int iIndex) { return m_Design.GetEntry(iIndex); }
		inline int GetDesignTypeCount (void) { return m_Design.GetCount(); }
		inline const CExtension *GetExtensionDesc (int iIndex) { return m_Design.GetExtension(iIndex); }
		inline int GetExtensionDescCount (void) { return m_Design.GetExtensionCount(); }
		inline CItemType *GetItemType (int iIndex) { return (CItemType *)m_Design.GetEntry(designItemType, iIndex); }
		inline int GetItemTypeCount (void) { return m_Design.GetCount(designItemType); }
		inline CPower *GetPower (int iIndex) { return (CPower *)m_Design.GetEntry(designPower, iIndex); }
		inline int GetPowerCount (void) { return m_Design.GetCount(designPower); }
		inline CShipClass *GetShipClass (int iIndex) { return (CShipClass *)m_Design.GetEntry(designShipClass, iIndex); }
		inline int GetShipClassCount (void) { return m_Design.GetCount(designShipClass); }
		inline CSoundType *GetSoundType (int iIndex) const { return (CSoundType *)m_Design.GetEntry(designSound, iIndex); }
		inline int GetSoundTypeCount (void) const { return m_Design.GetCount(designSound); }
		inline CSovereign *GetSovereign (int iIndex) const { return (CSovereign *)m_Design.GetEntry(designSovereign, iIndex); }
		inline int GetSovereignCount (void) { return m_Design.GetCount(designSovereign); }
		inline CStationType *GetStationType (int iIndex) { return (CStationType *)m_Design.GetEntry(designStationType, iIndex); }
		inline int GetStationTypeCount (void) { return m_Design.GetCount(designStationType); }
		inline CTopology &GetTopology (void) { return m_Topology; }
		inline CTopologyNode *GetTopologyNode (int iIndex) { return m_Topology.GetTopologyNode(iIndex); }
		inline int GetTopologyNodeCount (void) { return m_Topology.GetTopologyNodeCount(); }

		void PaintObject (CG32bitImage &Dest, const RECT &rcView, CSpaceObject *pObj);
		void PaintObjectMap (CG32bitImage &Dest, const RECT &rcView, CSpaceObject *pObj);
		void PaintPOV (CG32bitImage &Dest, const RECT &rcView, DWORD dwFlags);
		void PaintPOVLRS (CG32bitImage &Dest, const RECT &rcView, Metric rScale, DWORD dwFlags, bool *retbNewEnemies = NULL);
		void PaintPOVMap (CG32bitImage &Dest, const RECT &rcView, Metric rMapScale);
		inline void SetLogImageLoad (bool bLog = true) { CSmartLock Lock(m_cs); m_iLogImageLoad += (bLog ? -1 : +1); }
		void Update (SSystemUpdateCtx &Ctx);
		void UpdateExtended (void);

		void DebugOutput (char *pszLine, ...);

	private:
		struct SLevelEncounter
			{
			CStationType *pType;
			int iWeight;
			CSovereign *pBaseSovereign;
			IShipGenerator *pTable;
			};

		struct STransSystemObject
			{
			CSpaceObject *pObj;
			CTopologyNode *pDest;
			int iArrivalTime;

			STransSystemObject *pNext;
			};

		bool FindByUNID (CIDTable &Table, DWORD dwUNID, CObject **retpObj = NULL);
		CObject *FindByUNID (CIDTable &Table, DWORD dwUNID);
		ALERROR InitCodeChain (const TArray<SPrimitiveDefTable> &CCPrimitives);
		ALERROR InitCodeChainPrimitives (void);
		void InitDefaultEffects (void);
		ALERROR InitDeviceStorage (CString *retsError);
		ALERROR InitFonts (void);
		ALERROR InitLevelEncounterTables (void);
		ALERROR InitRequiredEncounters (CString *retsError);
		ALERROR InitTopology (DWORD dwStartingMap, CString *retsError);
		void NotifyMissionsOfNewSystem (CSystem *pSystem);
		inline void SetCurrentAdventureDesc (CAdventureDesc *pAdventure) { m_pAdventure = pAdventure; }
		void SetHost (IHost *pHost);
		void SetPlayer (IPlayerController *pPlayer);
		void UpdateMissions (int iTick, CSystem *pSystem);

		//	Design data

		CExtensionCollection m_Extensions;		//	Loaded extensions
		CDesignCollection m_Design;				//	Design collection
		CDeviceStorage m_DeviceStorage;			//	Local cross-game storage

		CString m_sResourceDb;					//	Resource database

		CIDTable m_Sounds;						//	Array of sound channels (int)
		CObjectArray m_LevelEncounterTables;	//	Array of SLevelEncounter arrays
		bool m_bBasicInit;						//	TRUE if we've initialized CodeChain, etc.

		//	Game instance data

		bool m_bRegistered;						//	If TRUE, this is a registered game
		bool m_bResurrectMode;					//	If TRUE, this session is a game resurrect
		int m_iTick;							//	Ticks since beginning of time
		int m_iPaintTick;						//	Advances only when we paint a frame
		CGameTimeKeeper m_Time;					//	Game time tracker
		CAdventureDesc *m_pAdventure;			//	Current adventure
		CSpaceObject *m_pPOV;					//	Point of view
		IPlayerController *m_pPlayer;			//	Player controller
		CSpaceObject *m_pPlayerShip;			//	Player ship
		CSystem *m_pCurrentSystem;				//	Current star system (used by code)
		TSortMap<DWORD, CSystem *> m_StarSystems;	//	Array of CSystem (indexed by ID)
		CTimeDate m_StartTime;					//	Time when we started the game
		DWORD m_dwNextID;						//	Next universal ID
		CTopology m_Topology;					//	Array of CTopologyNode
		CAscendedObjectList m_AscendedObjects;	//	Ascended objects (objects not in any system)
		CMissionList m_AllMissions;				//	List of all missions
		CTimedEventList m_Events;				//	List of all global events
		CObjectTracker m_Objects;				//	Objects across all systems
		CObjectStats m_ObjStats;				//	Object stats (across all systems)

		//	Support structures

		CCriticalSection m_cs;
		IHost *m_pHost;
		CCodeChain m_CC;
		ICCItem *m_pSavedGlobalSymbols;
		CSoundMgr *m_pSoundMgr;
		const CG16bitFont *m_FontTable[fontCount];
		CG16bitFont m_DefaultFonts[fontCount];
		TArray<INotifications *> m_Subscribers;
		CSFXOptions m_SFXOptions;
		CFractalTextureLibrary m_FractalTextureLibrary;
		CGImageCache m_DynamicImageLibrary;
		SViewportAnnotations m_ViewportAnnotations;

		//	Debugging structures

		bool m_bDebugMode;
		bool m_bNoSound;
		int m_iLogImageLoad;					//	If >0 we disable image load logging
	};

//	String-Constant Helpers

Abilities AbilityDecode (const CString &sString);
AbilityModifications AbilityModificationDecode (const CString &sString);
AbilityModificationOptions AbilityModificationOptionDecode (const CString &sString);
AbilityStatus AbilityStatusDecode (const CString &sString);

CString AbilityEncode (Abilities iValue);
CString AbilityModificationEncode (AbilityModifications iValue);
CString AbilityModificationOptionEncode (AbilityModificationOptions iValue);
CString AbilityStatusEncode (AbilityStatus iValue);

CString GetDamageResultsName (EDamageResults iResult);

DestructionTypes GetDestructionCause (const CString &sString);
CString GetDestructionName (DestructionTypes iCause);

CString GetGenomeID (GenomeTypes iGenome);
CString GetGenomeName (GenomeTypes iGenome);
GenomeTypes ParseGenomeID (const CString &sText);

CString GetPaintLayerID (CSystem::LayerEnum iPaintLayer);
CSystem::LayerEnum ParsePaintLayerID (const CString &sID);

MessageTypes GetMessageFromID (const CString &sID);
CString GetMessageID (MessageTypes iMessage);

CSpaceObject::InterSystemResults GetInterSystemResult (const CString &sString);

DWORD GetOrderFlags (IShipController::OrderTypes iOrder);
CString GetOrderName (IShipController::OrderTypes iOrder);
IShipController::OrderTypes GetOrderType (const CString &sString);
int OrderGetDataCount (IShipController::OrderTypes iOrder);
bool OrderHasDataString (IShipController::OrderTypes iOrder);
bool OrderHasDataVector (IShipController::OrderTypes iOrder);
bool OrderHasTarget (IShipController::OrderTypes iOrder, bool *retbRequired = NULL);
bool ParseOrderString (const CString &sValue, IShipController::OrderTypes *retiOrder, IShipController::SData *retData = NULL);

EStorageScopes ParseStorageScopeID (const CString &sID);

//	Miscellaneous Helpers

void AppendReferenceString (CString *iosReference, const CString &sString);
int CalcEffectiveHP (int iLevel, int iHP, int *iHPbyDamageType);
Metric CalcLevelDiffStrength (int iDiff);
ICCItem *CreateItemFromDamageEffects (CCodeChain &CC, SDamageCtx &Ctx);
int GetDamageTypeLevel (DamageTypes iDamage);
Metric *GetDestinyToBellCurveArray (void);
void LoadDamageEffectsFromItem (ICCItem *pItem, SDamageCtx &Ctx);
EManeuverTypes CalcTurnManeuver (int iDesired, int iCurrent, int iRotationAngle);
CString ParseCriteriaParam (char **ioPos, bool bExpectColon = true, bool *retbBinaryParam = NULL);
bool ParseCriteriaParamLevelRange (char **ioPos, int *retiLow, int *retiHigh);
Metric ParseDistance (const CString &sValue, Metric rDefaultScale);

//	CodeChain helper functions (CCUtil.cpp)

Metric CalcRandomMetric (CCodeChain &CC, ICCItem *pItem);
CString CreateDataFieldFromItemList (const TArray<CItem> &List);
CString CreateDataFromItem (CCodeChain &CC, ICCItem *pItem);
ICCItem *CreateDisposition (CCodeChain &CC, CSovereign::Disposition iDisp);
ICCItem *CreateListFromImage (CCodeChain &CC, const CObjectImageArray &Image, int iRotation = 0);
ICCItem *CreateListFromItem (CCodeChain &CC, const CItem &Item);
ICCItem *CreateListFromOrbit (CCodeChain &CC, const COrbit &OrbitDesc);
ICCItem *CreateListFromVector (CCodeChain &CC, const CVector &vVector);
CItem CreateItemFromList (CCodeChain &CC, ICCItem *pList);
CSpaceObject *CreateObjFromItem (CCodeChain &CC, ICCItem *pItem);
bool CreateOrbitFromList (CCodeChain &CC, ICCItem *pList, COrbit *retOrbitDesc);
ICCItem *CreateResultFromDataField (CCodeChain &CC, const CString &sValue);
CShip *CreateShipObjFromItem (CCodeChain &CC, ICCItem *pArg);
CStation *CreateStationObjFromItem (CCodeChain &CC, ICCItem *pArg);
CVector CreateVectorFromList (CCodeChain &CC, ICCItem *pList);
CCXMLWrapper *CreateXMLElementFromItem (CCodeChain &CC, ICCItem *pItem);
void DefineGlobalItem (CCodeChain &CC, const CString &sVar, const CItem &Item);
void DefineGlobalSpaceObject (CCodeChain &CC, const CString &sVar, CSpaceObject *pObj);
void DefineGlobalVector (CCodeChain &CC, const CString &sVar, const CVector &vVector);
void DefineGlobalWeaponType (CCodeChain &CC, const CString &sVar, CItemType *pWeaponType);
CInstalledArmor *GetArmorSectionArg (CCodeChain &CC, ICCItem *pArg, CSpaceObject *pObj);
CDamageSource GetDamageSourceArg (CCodeChain &CC, ICCItem *pArg);
DamageTypes GetDamageTypeFromArg (CCodeChain &CC, ICCItem *pArg);
CInstalledDevice *GetDeviceFromItem (CCodeChain &CC, CSpaceObject *pObj, ICCItem *pArg);
CEconomyType *GetEconomyTypeFromItem (CCodeChain &CC, ICCItem *pItem);
CEconomyType *GetEconomyTypeFromString (const CString &sCurrency);
ALERROR GetEconomyUNIDOrDefault (CCodeChain &CC, ICCItem *pItem, DWORD *retdwUNID);
void GetImageDescFromList (CCodeChain &CC, ICCItem *pList, CG32bitImage **retpBitmap, RECT *retrcRect);
CItem GetItemFromArg (CCodeChain &CC, ICCItem *pArg);
CItemType *GetItemTypeFromArg (CCodeChain &CC, ICCItem *pArg);
bool GetLinkedFireOptions (ICCItem *pArg, DWORD *retdwOptions, CString *retsError);
ALERROR GetPosOrObject (CEvalContext *pEvalCtx, ICCItem *pArg, CVector *retvPos, CSpaceObject **retpObj = NULL, int *retiLocID = NULL);
CWeaponFireDesc *GetWeaponFireDescArg (ICCItem *pArg);
bool IsVectorItem (ICCItem *pItem);
ALERROR LoadCodeBlock (const CString &sCode, ICCItem **retpCode, CString *retsError = NULL);
ICCItem *StdErrorNoSystem (CCodeChain &CC);

//	Inlines

inline CSpaceObject *CInstalledDevice::GetTarget (CSpaceObject *pSource) const { return ((m_dwTargetID && pSource) ? pSource->GetSystem()->FindObject(m_dwTargetID) : NULL); }
inline void CInstalledDevice::SetTarget (CSpaceObject *pObj) { m_dwTargetID = (pObj ? pObj->GetID() : 0); }

inline bool CItem::IsDisrupted (void) const { return (m_pExtra ? (m_pExtra->m_dwDisruptedTime >= (DWORD)g_pUniverse->GetTicks()) : false); }

inline void CSpaceObjectGrid::AddObject (CSpaceObject *pObj) { GetList(pObj->GetPos()).FastAdd(pObj); }

inline int CalcHPDamageAdj (int iHP, int iDamageAdj)
	{ return (iDamageAdj == 0 ? -1 : (int)((iHP * 100.0 / iDamageAdj) + 0.5)); }

#endif