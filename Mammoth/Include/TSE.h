//	TSE.h
//
//	Transcendence Space Engine
//	Copyright 2003 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

#include "Alchemy.h"
#include "Euclid.h"
#include "Lingua.h"
#include "Internets.h"
#include "Crypto.h"
#include "JPEGUtil.h"
#include "XMLUtil.h"
#include "JSONUtil.h"
#include "DirectXUtil.h"
#include "Reanimator.h"

//  Starting with Part II we use Arabic numerals for levels.

//#define LEVEL_ROMAN_NUMERALS

//	Define some debugging symbols

#ifdef DEBUG
#define DEBUG_ENEMY_CACHE_BUG
//#define DEBUG_ALL_ITEMS
//#define DEBUG_ALL_NODES
//#define DEBUG_ASTAR_PATH
//#define DEBUG_ASTAR_PERF
//#define DEBUG_ATTACK_TARGET
//#define DEBUG_ATTACK_TARGET_MANEUVERS
//#define DEBUG_AVOID_POTENTIAL
//#define DEBUG_BOUNDING_RECT
//#define DEBUG_COMBAT
//#define DEBUG_COMBAT_AI
//#define DEBUG_DOCK_PORT_POS
//#define DEBUG_ENCOUNTER_COUNTS
//#define DEBUG_FIRE_ON_OPPORTUNITY
//#define DEBUG_FORMATION
//#define DEBUG_HENCHMAN
//#define DEBUG_HIT_SCAN
//#define DEBUG_LOAD
//#define DEBUG_NAV_PATH
//#define DEBUG_NEBULA_PAINTING
//#define DEBUG_PARTICLE_BOUNDS
//#define DEBUG_PERFORMANCE
#define DEBUG_PERFORMANCE_COUNTERS
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
#else
//#define DEBUG_PERFORMANCE_COUNTERS
#endif

//	We leave this defined because we want to get traces in the field in case
//	of a crash.
#define DEBUG_PROGRAMSTATE

//	Debugging help

#define DEBUG_CATCH_OBJ(pObj)		} catch (...) \
		{ \
		kernelDebugLogPattern("Crash in %s", CString(__FUNCTION__)); \
		\
		if ((pObj) == NULL) \
			kernelDebugLogPattern("pObj = NULL"); \
		else \
			{ \
			CString sError; \
			ReportCrashObj(&sError, (pObj)); \
			kernelDebugLogPattern("pObj = "); \
			kernelDebugLogString(sError); \
			} \
		\
		throw; \
		}

#define DEBUG_TRY_OBJ_LOOP		CSpaceObject *pObj = NULL; try {
#define DEBUG_CATCH_OBJ_LOOP	} catch (...) \
		{ \
		kernelDebugLogPattern("Crash in %s", CString(__FUNCTION__)); \
		\
		if (pObj == NULL) \
			kernelDebugLogPattern("pObj = NULL"); \
		else \
			{ \
			CString sError; \
			ReportCrashObj(&sError, pObj); \
			kernelDebugLogPattern("pObj = "); \
			kernelDebugLogString(sError); \
			} \
		\
		throw; \
		}

//	If ITEM_REFERENCE is defined, then the player doesn't see the
//	stats for an item until they install it (or get reference info)
//#define ITEM_REFERENCE

//	Forward references
class CAccessibilitySettings;
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
class CShieldClass;
class CShip;
class CShipClass;
class CShipTable;
class CSovereign;
class CSpaceEnvironmentType;
class CSpaceObject;
class CStation;
class CStationType;
class CSystem;
class CSystemEvent;
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

#include "TSEEngineDefs.h"
#include "TSEVersions.h"
#include "TSEGeometry.h"
#include "TSETypes.h"
#include "TSEUtil.h"
#include "TSETransLisp.h"
#include "TSEPropertyHandler.h"
#include "TSELanguage.h"
#include "TSEDesignDefs.h"
#include "TSETopologyDefs.h"
#include "TSEItemDefs.h"
#include "TSEResourceDb.h"
#include "TSEDesign.h"
#include "TSEStorage.h"
#include "TSEMultiverse.h"
#include "TSEPaintUtil.h"
#include "TSESpaceObjectDefs.h"
#include "TSESystem.h"
#include "TSEDockScreenSession.h"

//	Helper functions

CString ComposeDamageAdjReference (int *AdjRow, int *StdRow);

int NLCompare (TArray<CString> &Input, TArray<CString> &Pattern);
void NLSplit (const CString &sPhrase, TArray<CString> *retArray);

void ReportCrashObj (CString *retsMessage, const CSpaceObject *pCrashObj = NULL);

//	Miscellaneous Structures & Classes

class CShockwaveHitTest
	{
	public:
		void Init (int iSegments, int iLives);
		bool IsEmpty (void) const { return (m_Segments.GetCount() == 0); }
		void ReadFromStream (SLoadCtx &Ctx);
		void Update (SEffectUpdateCtx &Ctx, const CVector &vPos, Metric rMinRadius, Metric rMaxRadius);
		void WriteToStream (IWriteStream *pStream) const;

	private:
		void CalcObjBounds (CSpaceObject *pObj, const CVector &vPos, Metric *retrStartAngle, Metric *retrSizeAngle, Metric *retrStartRadius, Metric *retrEndRadius);

		TArray<int> m_Segments;
	};

class CHitCtx
	{
	public:
		CHitCtx (CSpaceObject *pHit = NULL, const CVector &vHitPos = NullVector, int iHitDir = -1) :
				m_pHit(pHit),
				m_vHitPos(vHitPos),
				m_iHitDir(iHitDir)
			{ }

		int GetHitDir (void) const { return m_iHitDir; }
		CSpaceObject *GetHitObj (void) const { return m_pHit; }
		const CVector &GetHitPos (void) const { return m_vHitPos; }

		void ReadFromStream (SLoadCtx &Ctx);
		void WriteToStream (IWriteStream *pStream) const;

	private:
		CSpaceObject *m_pHit;
		CVector m_vHitPos;
		int m_iHitDir;
	};

//	Attack Detector

class CAttackDetector
	{
	public:
		CAttackDetector (void) { }

		ICCItemPtr AsCCItem () const;
		void Blacklist (void) { m_iCounter = -1; }
		void ClearBlacklist (void) { m_iCounter = 0; }
		bool IsBlacklisted (void) const { return m_iCounter == -1; }
		bool IsEmpty () const { return (m_iCounter == 0 && m_iLastHit == 0); }
		bool Hit (int iTick);
		void ReadFromStream (SLoadCtx &Ctx);
		void Update (int iTick) { if ((iTick % DECAY_RATE) == 0) OnUpdate(); }
		void WriteToStream (IWriteStream *pStream);

	private:
		enum Constants
			{
			DECAY_RATE = 150,
			HIT_LIMIT = 3,
			MULTI_HIT_WINDOW = 20,
			};

		void OnUpdate (void);

		int m_iCounter = 0;
		int m_iLastHit = 0;
	};

//	CSpaceObject Definitions ---------------------------------------------------

#include "TSESpaceObjectUtil.h"
#include "TSEOverlays.h"

struct SApplyConditionOptions
	{
	CDamageSource Cause;

	SObjectPartDesc ApplyTo;
	int iTimer = -1;

	bool bNoImmunityCheck = false;
	bool bNoMessage = false;
	};

struct SUpdateCtx
	{
	public:
		void AddPlayerObj (CSpaceObject &Obj) { m_PlayerObjs.Insert(&Obj); }
		int GetLightIntensity (CSpaceObject *pObj) const;
		CAutoDockCalc &GetAutoDock () { return m_AutoDock; }
		CAutoMiningCalc &GetAutoMining () { return m_AutoMining; }
		CAutoTargetCalc &GetAutoTarget () { return m_AutoTarget; }
		CSpaceObject *GetPlayerShip () { return m_pPlayer; }
		CTargetList &GetTargetList ();
		bool IsTimeStopped (void) const { return m_bTimeStopped; }
		void OnEndUpdate () { m_pObj = NULL; }
		void OnStartUpdate (CSpaceObject &Obj);
		bool PlayerHasCommsTarget () const { return m_bPlayerHasCommsTarget; }
		bool PlayerHasSquadron () const { return m_bPlayerHasSquadron; }
		void SetPlayerShip (CSpaceObject &PlayerObj);
		void UpdatePlayerCalc (const CSpaceObject &Obj);

		CSystem *pSystem = NULL;					//	Current system
		SViewportAnnotations *pAnnotations = NULL;	//	Extra structure to deliver to PaintViewport

		//	Misc flags

		bool bGravityWarning = false;				//	Player in a dangerous gravity field
		bool bHasShipBarriers = false;				//	TRUE if the system has ship barriers (e.g., Arena)
		bool bHasGravity = false;					//	TRUE if the system has gravity

	private:

		CSpaceObject *m_pPlayer = NULL;				//	The player
		TArray<CSpaceObject *> m_PlayerObjs;		//	List of player objects, if pPlayer == NULL

		//	About the object being updated

		CSpaceObject *m_pObj = NULL;				//	Object being updated
		CTargetList m_TargetList;					//	Cached target list
		bool m_bTimeStopped = false;				//	Object is currently time-stopped (cached for perf).

		//	Cached computations

		CAutoDockCalc m_AutoDock;					//	Used to compute nearest docking port
		CAutoMiningCalc m_AutoMining;				//	Used to compute nearest minable asteroid
		CAutoTargetCalc m_AutoTarget;				//	Used to compute player's auto target
		bool m_bPlayerHasCommsTarget = false;		//	TRUE if there is an object for the player to communicate
		bool m_bPlayerHasSquadron = false;			//	TRUE if there is at least one ship in the player's squadron

		//	Cached computed values

		mutable CSpaceObject *m_pCacheObj = NULL;	//	Cached values apply to this object.
		mutable int m_iLightIntensity = -1;			//	Light intensity at pCacheObj (-1 if not computed).
		mutable bool m_bTargetListValid = false;	//	TRUE if m_TargetList is initialized
	};

//	CSpaceObject ---------------------------------------------------------------
//
//	SEMANTICS
//
//	CanAttack: If TRUE, this is an active/intelligent object that could pose
//		a threat. E.g., a ship or a station. Generally used to pick out a 
//		possible target. Missiles/beams cannot attack. Wrecks and abandoned
//		stations cannot attack.
//
//	CanBeAttacked: If TRUE, this object has hit points and can be attacked. This
//		is usually the same as CanAttack, but there are some objects (e.g.,
//		Arco Vaughn's habitat) that can be destroyed but cannot attack. We need
//		the distinction because we always auto-target CanAttack over 
//		CanBeAttacked.
//
//		NOTE: This is mostly about targeting, not about being hit. Lots of 
//		objects can be hit but do not need to be targets (e.g., asteroids).
//		In general, every object that CanAttack should be CanBeAttacked (but the
//		reverse need not be true).
//
//	CanBeDestroyed: If TRUE, the object can be destroyed (not just abandoned).
//		E.g., a shipwreck can be destroyed, but an abandoned station cannot be.
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

class CSpaceObject
	{
	public:
		static constexpr DWORD AGGRESSOR_THRESHOLD = 30 * 30;
		static constexpr int ANNOTATION_INNER_SPACING_Y =			2;

		enum Categories
			{
			catShip =			0x00000001,
			catStation =		0x00000002,
			catBeam =			0x00000004,
			catMissile =		0x00000008,
			catFractureEffect =	0x00000010,
			catMarker =			0x00000020,
			catMission =		0x00000040,
			catOther =			0x80000000,

			catMask =			0x8000007F,
			};

		enum StealthAndPerceptionLevels
			{
			stealthMin =			0,
			stealthNormal =			4,
			stealthMax = CPerceptionCalc::RANGE_ARRAY_SIZE - 1,			//	Cloaked

			perceptMin =			0,
			perceptNormal =			4,
			perceptMax = CPerceptionCalc::RANGE_ARRAY_SIZE - 1,
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
			insNoLauncherSlotsLeft,				//	No launcher slots left
			insNotInstallable,					//	Item cannot be installed
			insReactorIncompatible,				//	Reactor is too powerful for ship class
			insReactorOverload,					//	Sum of device power > than reactor output
			insReactorTooWeak,					//	Reactor output too small for device
			insNotCompatible,					//	Item does not match criteria in ship class
			insTooMuchCargo,					//	New device would decrease cargo space below limit

			insReplaceCargo,					//	OK, but cargo device will be replaced
			insReplaceDrive,					//	OK, but drive device will be replaced
			insReplaceLauncher,					//	OK, but launcher will be replaced
			insReplaceOther,					//	OK, but another device will be replaced
			insReplaceReactor,					//	OK, but reactor will be replaced
			insReplaceShields,					//	OK, but shields will be replaced

			insInstallItemResultsCount,			//	Must be last enum
			};

		enum RequestDockResults
			{
			dockingOK,

			dockingNotSupported,				//	No docking ports, or object destroyed
			dockingDisabled,					//	Docking temporarily unavailable
			dockingDenied,						//	Docking request denied
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

		struct SPlayerDestinationOptions
			{
			bool bShowDistanceAndBearing = false;
			bool bAutoClearDestination = false;
			bool bAutoClearOnDestroy = false;
			bool bAutoClearOnDock = false;
			bool bAutoClearOnGate = false;
			bool bShowHighlight = false;
			};

		struct SRefitObjCtx
			{
			bool bIsEmpty () const
				{
				return (iMaxHPToRepair == 0 && !bDecontaminate && !bResupplyAmmo);
				}

			int iMaxRepairLevel = 0;
			int iMaxHPToRepair = 0;			//	-1 = repair all damage

			bool bDecontaminate = false;
			bool bResupplyAmmo = false;
			bool bScrapeOverlays = false;
			};

		struct SUsableItemOptions
			{
			char chUseKey = '\0';
			};

		//	Creation and Lifetime

		CSpaceObject (CUniverse &Universe);
		virtual ~CSpaceObject (void);

		static void CreateFromStream (SLoadCtx &Ctx, CSpaceObject **retpObj);

		ALERROR AddToSystem (CSystem &System, bool bNoGlobalInsert = false);
		void Ascend (void);
		void Destroy (SDamageCtx &Ctx) { Destroy(Ctx.Damage.GetCause(), Ctx.Attacker, &Ctx.GetDesc()); }
		void Destroy (DestructionTypes iCause, SDamageCtx &Ctx) { Destroy(iCause, Ctx.Attacker, &Ctx.GetDesc()); }
		void Destroy (DestructionTypes iCause, const CDamageSource &Attacker, CWeaponFireDesc *pWeaponDesc = NULL, CSpaceObject **retpWreck = NULL);
		CDesignCollection &GetDesign (void) const;
		CSystem *GetSystem (void) const { return m_pSystem; }
		CUniverse &GetUniverse (void) const { return m_Universe; }
		bool IsAscended (void) const { return m_fAscended; }
		void Remove (DestructionTypes iCause, const CDamageSource &Attacker, bool bRemovedByOwner = false);
		void SetAscended (bool bAscended = true) { m_fAscended = bAscended; }
		void SetIndex (int iIndex) { m_iIndex = iIndex; }

		//	Abilities

		virtual AbilityStatus GetAbility (Abilities iAbility) const { return ablUninstalled; }
		virtual bool SetAbility (Abilities iAbility, AbilityModifications iModification, int iDuration, DWORD dwOptions) { return false; }

		//	Basics

		virtual CDesignType *GetType (void) const { return NULL; }
		void SetSovereign (CSovereign *pSovereign);

		//	Bounds

		void GetBoundingRect (CVector *retvUR, CVector *retvLL) const;
		const Metric &GetBounds (void) { return m_rBoundsX; }
		CVector GetBoundsDiag (void) const { return CVector(m_rBoundsX, m_rBoundsY); }
		Metric GetBoundsRadius (void) const { return Max(m_rBoundsX, m_rBoundsY); }

		void GetHitRect (CVector *retvUR, CVector *retvLL) const;
		Metric GetHitSize (void) const { return GetHitSizePixels() * g_KlicksPerPixel; }
		int GetHitSizePixels (void) const;
		int GetHitSizeHalfAngle (Metric rDist) const { return Max((int)(180.0 * atan(0.5 * GetHitSize() / rDist) / PI), 1); }

		bool InBox (const CVector &vUR, const CVector &vLL) const 
			{ return (vUR.GetX() > m_vPos.GetX() - m_rBoundsX) 
					&& (vUR.GetY() > m_vPos.GetY() - m_rBoundsY)
					&& (vLL.GetX() < m_vPos.GetX() + m_rBoundsX)
					&& (vLL.GetY() < m_vPos.GetY() + m_rBoundsY); }
		bool InBoxPoint (const CVector &vUR, const CVector &vLL) const
			{ return (vUR.GetX() > m_vPos.GetX()) 
					&& (vUR.GetY() > m_vPos.GetY())
					&& (vLL.GetX() < m_vPos.GetX())
					&& (vLL.GetY() < m_vPos.GetY()); }
		bool IsPartlyVisibleInBox (const CVector &vUR, const CVector &vLL)
			{
			Metric rHalfSize = 0.25 * GetHitSize();
			return (vUR.GetX() > m_vPos.GetX() - rHalfSize) 
				&& (vUR.GetY() > m_vPos.GetY() - rHalfSize)
				&& (vLL.GetX() < m_vPos.GetX() + rHalfSize)
				&& (vLL.GetY() < m_vPos.GetY() + rHalfSize);
			}
		bool PointInBounds (const CVector &vPos) const
			{
			return (vPos.GetX() < m_vPos.GetX() + m_rBoundsX)
					&& (vPos.GetY() < m_vPos.GetY() + m_rBoundsY)
					&& (vPos.GetX() > m_vPos.GetX() - m_rBoundsX)
					&& (vPos.GetY() > m_vPos.GetY() - m_rBoundsY);
			}

		virtual void RefreshBounds (void) { }

		void SetBounds (Metric rBounds) { m_rBoundsX = rBounds; m_rBoundsY = rBounds; }
		void SetBounds (Metric rBoundsX, Metric rBoundsY) { m_rBoundsX = rBoundsX; m_rBoundsY = rBoundsY; }
		void SetBounds (const RECT &rcRect, Metric rParallaxDist = 1.0)
			{
			m_rBoundsX = Max(1.0, rParallaxDist) * g_KlicksPerPixel * (RectWidth(rcRect) / 2);
			m_rBoundsY = Max(1.0, rParallaxDist) * g_KlicksPerPixel * (RectHeight(rcRect) / 2);
			}
		void SetBounds (IEffectPainter *pPainter)
			{
			RECT rcRect;
			pPainter->GetBounds(&rcRect);
			SetBounds(rcRect);
			}

		//	Characters

		virtual CDesignType *GetCharacter (void) const { return NULL; }

		//	Combat Targeting

		virtual CSpaceObject *GetTarget (DWORD dwFlags = 0) const { return NULL; }
		virtual CTargetList GetTargetList (void) const { return CTargetList(); }

		int CalcFireSolution (CSpaceObject *pTarget, Metric rMissileSpeed) const;
		CSpaceObject *CalcTargetToAttack (CSpaceObject *pAttacker, CSpaceObject *pOrderGiver);
		bool IsLineOfFireClear (const CInstalledDevice *pWeapon, CSpaceObject *pTarget, int iAngle, Metric rDistance = (30.0 * LIGHT_SECOND), CSpaceObject **retpFriend = NULL) const;
		bool IsPlayerTarget (void) const { return m_fPlayerTarget; }
		void SetPlayerTarget (void) { m_fPlayerTarget = true; }

		//	Communications

		bool CanCommunicateWith (const CSpaceObject &SenderObj) const;
		void CommsMessageFrom (CSpaceObject *pSender, int iIndex);
		DWORD Communicate (CSpaceObject *pReceiver, MessageTypes iMessage, CSpaceObject *pParam1 = NULL, DWORD dwParam2 = 0, ICCItem *pData = NULL) const { return pReceiver->OnCommunicate(const_cast<CSpaceObject *>(this), iMessage, pParam1, dwParam2, pData); }
		int FindCommsMessage (const CString &sID);
		int FindCommsMessageByName (const CString &sName);
		CCommunicationsHandler *GetCommsHandler (void);
		const CCommunicationsHandler *GetCommsHandler (void) const;
		int GetCommsMessageCount (void);
		CString GetDesiredCommsKey (void) const;
		DWORD GetSquadronCommsStatus () const;
		bool IsCommsMessageValidFrom (const CSpaceObject &SenderObj, int iIndex, CString *retsMsg = NULL, CString *retsKey = NULL) const;
		void SendSquadronMessage (const CString &sMsg);
		void SendSquadronMessage (CSpaceObject &ReceiverObj, const CString &sMsg);

		//	Conditions

		EConditionResult ApplyCondition (ECondition iCondition, const SApplyConditionOptions &Options);
		EConditionResult CanApplyCondition (ECondition iCondition, const SApplyConditionOptions &Options) const;
		EConditionResult CanRemoveCondition (ECondition iCondition, const SApplyConditionOptions &Options) const;
		EConditionResult RemoveCondition (ECondition iCondition, const SApplyConditionOptions &Options);

		bool GetCondition (ECondition iCondition) const;
		CConditionSet GetConditions (void) const;
		bool IsBlind (void) const { return GetCondition(ECondition::blind); }
		bool IsDisarmed (void) const { return GetCondition(ECondition::disarmed); }
		bool IsImmuneTo (SpecialDamageTypes iSpecialDamage) const { return OnIsImmuneTo(iSpecialDamage); }
		bool IsLRSBlind (void) const { return GetCondition(ECondition::LRSBlind); }
		bool IsParalyzed (void) const { return GetCondition(ECondition::paralyzed); }
		bool IsRadioactive (void) const { return GetCondition(ECondition::radioactive); }
		bool IsTimeStopped (void) const { return GetCondition(ECondition::timeStopped); }
		void SetConditionDueToDamage (SDamageCtx &DamageCtx, ECondition iCondition);

		//	Data

		void ClearObjReferences (void) { m_Data.OnSystemChanged(NULL); }
		CAttributeDataBlock &GetData (void) { return m_Data; }
		const CAttributeDataBlock &GetData (void) const { return m_Data; }
		ICCItemPtr GetData (const CString &sAttrib) const { return m_Data.GetDataAsItem(sAttrib); }
		CString GetDataField (const CString &sField) { CString sValue; FindDataField(sField, &sValue); return sValue; }
		ICCItemPtr GetGlobalData (const CString &sAttribute) const;
		CSpaceObject *GetObjRefData (const CString &sAttrib) { return m_Data.GetObjRefData(sAttrib); }
		ICCItemPtr IncData (const CString &sAttrib, ICCItem *pValue = NULL) { return m_Data.IncData(sAttrib, pValue); }
		void LoadObjReferences (CSystem *pSystem) { m_Data.LoadObjReferences(pSystem); }
		void SetData (const CString &sAttrib, ICCItem *pData) { m_Data.SetData(sAttrib, pData); }
		void SetDataFromDataBlock (const CAttributeDataBlock &Block) { m_Data.MergeFrom(Block); }
		void SetDataFromXML (CXMLElement *pData) { m_Data.SetFromXML(pData); }
		void SetGlobalData (const CString &sAttribute, ICCItem *pData);
		void SetObjRefData (const CString &sAttrib, CSpaceObject *pObj) { m_Data.SetObjRefData(sAttrib, pObj); }

		//	Devices

		virtual bool CanInstallItem (const CItem &Item, const CDeviceSystem::SSlotDesc &Slot = CDeviceSystem::SSlotDesc(), InstallItemResults *retiResult = NULL, CString *retsResult = NULL, CItem *retItemToReplace = NULL);
		virtual void DamageExternalDevice (int iDev, SDamageCtx &Ctx) { }
		virtual void DisableDevice (CInstalledDevice *pDevice) { }
		bool FindDevice (const CItem &Item, CInstalledDevice **retpDevice, CString *retsError);
		virtual CInstalledDevice *FindDevice (const CItem &Item) { return NULL; }
		virtual bool FindDeviceSlotDesc (const CItem &Item, SDeviceDesc *retDesc) { return false; }
		bool FireCanInstallItem (const CItem &Item, const CDeviceSystem::SSlotDesc &Slot, CString *retsResult);
		bool FireCanRemoveItem (const CItem &Item, int iSlot, CString *retsResult);
		virtual CInstalledDevice *GetDevice (int iDev) { return NULL; }
		virtual int GetDeviceCount (void) const { return 0; }
		virtual CDeviceItem GetDeviceItem (int iDev) const { return CItem().AsDeviceItem(); }
		virtual CDeviceSystem &GetDeviceSystem (void) { return CDeviceSystem::m_Null; }
		virtual const CDeviceSystem &GetDeviceSystem (void) const { return CDeviceSystem::m_Null; }
		CItem GetItemForDevice (CInstalledDevice *pDevice);
		virtual const CInstalledDevice *GetNamedDevice (DeviceNames iDev) const { return NULL; }
		virtual CInstalledDevice *GetNamedDevice (DeviceNames iDev) { return NULL; }
		virtual CDeviceItem GetNamedDeviceItem (DeviceNames iDev) const { return CItem().AsDeviceItem(); }
		virtual void OnDeviceStatus (CInstalledDevice *pDev, CDeviceClass::DeviceNotificationTypes iEvent) { }
		bool SetCursorAtDevice (CItemListManipulator &ItemList, int iDevSlot);
		bool SetCursorAtDevice (CItemListManipulator &ItemList, CInstalledDevice *pDevice);

		//	Docking

		virtual void CreateDefaultDockingPorts (void);
		virtual CSpaceObject *GetDockedObj (void) const { return NULL; }
		virtual CVector GetDockingPortOffset (int iRotation) { return NullVector; }
		virtual void OnDocked (CSpaceObject *pObj) { }
		virtual void OnDockingPortDestroyed (void) { }
		virtual void OnDockingStart (void) { }
		virtual void OnDockingStop (void) { }

		//	Docking Ports

		virtual RequestDockResults CanObjRequestDock (CSpaceObject *pObj = NULL) const { return dockingNotSupported; }
		virtual void CreateRandomDockedShips (IShipGenerator *pGenerator, const CShipChallengeDesc &ShipsNeeded = CShipChallengeDesc()) { }
		virtual const CDockingPorts *GetDockingPorts (void) const { return NULL; }
		virtual CDockingPorts *GetDockingPorts (void) { return NULL; }
		virtual void Undock (CSpaceObject *pObj) { }

		int GetDockingPortCount (void) const { const CDockingPorts *pPorts = GetDockingPorts(); return (pPorts ? pPorts->GetPortCount(this) : 0); }
		int GetNearestDockPort (CSpaceObject *pRequestingObj, CVector *retvPort = NULL);
		int GetOpenDockingPortCount (void) const { const CDockingPorts *pPorts = GetDockingPorts(); return (pPorts ? (pPorts->GetPortCount(this) - pPorts->GetPortsInUseCount(this)) : 0); }
		CSpaceObject *GetShipAtDockingPort (int iPort) { CDockingPorts *pPorts = GetDockingPorts(); return (pPorts ? pPorts->GetPortObj(this, iPort) : NULL); }
		bool IsObjDocked (const CSpaceObject *pObj) const { const CDockingPorts *pPorts = GetDockingPorts(); return (pPorts ? pPorts->IsObjDocked(pObj) : false); }
		bool IsObjDockedOrDocking (CSpaceObject *pObj) const { const CDockingPorts *pPorts = GetDockingPorts(); return (pPorts ? pPorts->IsObjDockedOrDocking(pObj) : false); }
		void PlaceAtRandomDockPort (CSpaceObject *pObj) { CDockingPorts *pPorts = GetDockingPorts(); if (pPorts) pPorts->DockAtRandomPort(this, pObj); }
		bool ObjRequestDock (CSpaceObject *pObj, int iPort = -1);
		bool SupportsDockingFast (void) const { return (m_fHasDockScreenMaybe ? true : false); }

		//	Dock Screens

		virtual DWORD GetDefaultBkgnd (void) { return 0; }
		virtual const CSoundResource *GetDockScreenAmbientSound () const { return NULL; }
		virtual CXMLElement *GetScreen (const CString &sName);

		CDesignType *GetFirstDockScreen (CString *retsScreen = NULL, ICCItemPtr *retpData = NULL);
		bool HasDefaultDockScreen (void) const { return (GetDefaultDockScreen() != NULL); }
		bool HasDockScreen (void) const;

		//	Events

		void AddEventSubscriber (CSpaceObject *pObj);
		void AddEventSubscribers (const CSpaceObjectList &Objs);
		bool FindEventHandler (const CString &sEntryPoint, SEventHandlerDesc *retEvent = NULL) const;
		bool FindEventHandler (CDesignType::ECachedHandlers iEvent, SEventHandlerDesc *retEvent = NULL) const;
		bool FindEventSubscriber (const CSpaceObject &Obj) const { return m_SubscribedObjs.FindObj(&Obj); }
		const CSpaceObjectList &GetEventSubscribers (void) const { return m_SubscribedObjs; }
		void RemoveAllEventSubscriptions (CSystem *pSystem, TArray<DWORD> *retRemoved = NULL);
		void RemoveAllEventSubscribers (void) { m_SubscribedObjs.DeleteAll(); }
		void RemoveEventSubscriber (CSpaceObject *pObj) { m_SubscribedObjs.Delete(pObj); }
		void ReportEventError (const CString &sEvent, const ICCItem *pError) const;
		void SetEventFlags (void);

		//	Fuel

		virtual void ConsumeFuel (Metric rFuel, CReactorDesc::EFuelUseTypes iUse = CReactorDesc::fuelConsume) { }
		virtual bool IsOutOfPower (void) { return false; }
		virtual void Refuel (Metric Fuel) { }
		virtual void Refuel (const CItem &Fuel) { }

		bool HasFuelItem (void);

		//	Groups

		virtual const CSquadronID &GetSquadronID () const { return CSquadronID::Null; }
		virtual void SetSquadronID (const CSquadronID &ID) { }

		//	Images

		virtual const CObjectImageArray &GetHeroImage (void) const { static CObjectImageArray NullImage; return NullImage; }
		virtual const CObjectImageArray &GetImage (int *retiRotationFrameIndex = NULL) const;
		virtual const CCompositeImageSelector &GetImageSelector (void) const { return CCompositeImageSelector::Null(); }
		virtual bool HasStarlightImage (void) const { return false; }
		virtual bool HasVolumetricShadow (int *retiStarAngle = NULL, Metric *retrStarDist = NULL) const { return false; }
		virtual void MarkImages (void) { }
		virtual void SetStarlightParams (const CSpaceObject &StarObj, Metric rLightRadius) { }

		int GetImageScale (void) const;

		//	Items

		bool AddItem (const CItem &Item, CItem *retResult = NULL, CString *retsError = NULL);
		EnhanceItemStatus AddItemEnhancement (const CItem &itemToEnhance, CItemType *pEnhancement, int iLifetime, DWORD *retdwID);
		EnhanceItemStatus AddItemEnhancement (CItemListManipulator &ItemList, CItemType *pEnhancement, int iLifetime, DWORD *retdwID);
		static ICCItemPtr AsCCItem (CCodeChainCtx &Ctx, const CItem::SEnhanceItemResult &Result);
		CItem::SEnhanceItemResult CanEnhanceItem (CItemListManipulator &ItemList, const CItem &EnhancementItem, CString *retsError = NULL) const;
		void DamageItem (CInstalledDevice *pDevice);
		void DamageItem (CItemListManipulator &ItemList);
		void DisruptItem (CItemListManipulator &ItemList, DWORD dwDuration);
		EnhanceItemStatus EnhanceItem (CItemListManipulator &ItemList, const CItemEnhancement &Mods, DWORD *retdwID = NULL);
		bool EnhanceItem (CItemListManipulator &ItemList, const CItem &EnhancementItem, CItem::SEnhanceItemResult &retResult, CString *retsError = NULL);
		const CItemList &GetItemList (void) const { return m_ItemList; }
		CItemList &GetItemList (void) { return m_ItemList; }
		ICCItemPtr GetItemProperty (CCodeChainCtx &CCX, const CItem &Item, const CString &sName) const;
		CMenuData GetUsableItems (const SUsableItemOptions &Options) const;
		bool IncItemProperty (const CItem &Item, const CString &sProperty, const ICCItem &Value, int iCount, CItem *retItem = NULL, ICCItemPtr *retNewValue = NULL, CString *retsError = NULL);
		bool RemoveItem (const CItem &Item, DWORD dwItemMatchFlags, int iCount = -1, int *retiCountRemoved = NULL, CString *retsError = NULL);
		void RemoveItemEnhancement (const CItem &itemToEnhance, DWORD dwID, bool bExpiredOnly = false);
		void RepairItem (CItemListManipulator &ItemList);
		void SetCursorAtArmor (CItemListManipulator &ItemList, CInstalledArmor *pArmor);
		void SetCursorAtRandomItem (CItemListManipulator &ItemList, const CItemCriteria &Crit);
		bool SetItemData (const CItem &Item, const CString &sField, ICCItem *pValue, int iCount, CItem *retItem = NULL, CString *retsError = NULL);
		bool SetItemProperty (const CItem &Item, const CString &sName, ICCItem *pValue, int iCount, CItem *retItem, CString *retsError);
		bool Translate (const CString &sID, ICCItem *pData, ICCItemPtr &retResult) const;
		bool TranslateText (const CString &sID, ICCItem *pData, CString *retsText) const;
		bool UseItem (const CItem &Item, CString *retsError = NULL);

		void InvalidateItemListAddRemove (void) { m_fItemEventsValid = false; }
		void InvalidateItemListState (void) { m_fItemEventsValid = false; }
		bool IsItemEventListValid (void) const { return (m_fItemEventsValid ? true : false); }
		void OnModifyItemBegin (IDockScreenUI::SModifyItemCtx &ModifyCtx, const CItem &Item) const;
		void OnModifyItemComplete (IDockScreenUI::SModifyItemCtx &ModifyCtx, const CItem &Result);

		//	Joints
		//
		//	NOTE: "Attached" implies a permanent joint as when a ship is 
		//	composed of multiple sections. An object that is attached is NOT an
		//	independent object.

		virtual CSpaceObject *GetAttachedRoot (void) const { return NULL; }
		virtual bool IsAttached (void) const { return false; }

		CObjectJoint *GetFirstJoint (void) const { return m_pFirstJoint; }
		void SetFirstJoint (CObjectJoint *pJoint) { m_pFirstJoint = pJoint; }

		//	Mining

		int CalcMiningDifficulty (EAsteroidType iType) const;
		virtual bool CanBeMined (void) const { return false; }
		bool HasMinableItem (void) const;

		//	Miscellaneous

		void AddEffect (IEffectPainter *pPainter, const CVector &vPos, int iTick = 0, int iRotation = 0);
		bool Blocks (CSpaceObject *pObj) { return (m_fIsBarrier && CanBlock(pObj)); }
		bool BlocksShips (void) { return (m_fIsBarrier && CanBlockShips()); }
		virtual bool CanBeDestroyedBy (CSpaceObject &Attacker) const { return true; }
		bool CanBeHit (void) const { return (!m_fCannotBeHit && !m_fOutOfPlaneObj); }
		bool CanBeHitByFriends (void) const { return !m_fNoFriendlyTarget; }
		bool CanDetect (int Perception, CSpaceObject *pObj);
		bool CanHitFriends (void) const { return !m_fNoFriendlyFire; }
		void ClearNoFriendlyTarget (void) { m_fNoFriendlyTarget = false; }
		void ClearPlayerDocked (void) { m_fPlayerDocked = false; }
		void ClearPlayerTarget (void) { m_fPlayerTarget = false; }
		void ClearPOVLRS (void) { m_fInPOVLRS = false; }
		void ClearSelection (void) { m_fSelected = false; }
		void ClearShowDamageBar (void) { m_fShowDamageBar = false; }
		void CopyDataFromObj (CSpaceObject *pSource);
		ALERROR CreateRandomItems (CXMLElement *pItems, CSystem *pSystem);
		ALERROR CreateRandomItems (IItemGenerator *pItems, CSystem *pSystem);
		EDamageResults Damage (SDamageCtx &Ctx);
		static CString DebugDescribe (CSpaceObject *pObj);
		bool DebugIsValid (void) { return (DWORD)m_pSystem != 0xdddddddd; }
		static CString DebugLoadError (SLoadCtx &Ctx);
		void EnterGate (CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate, bool bAscend = false);
		bool FireCanDockAsPlayer (CSpaceObject *pDockTarget, CString *retsError);
		void FireCustomEvent (const CString &sEvent, ECodeChainEvents iEvent = eventNone, ICCItem *pData = NULL, ICCItem **retpResult = NULL);
		void FireCustomItemEvent (const CString &sEvent, const CItem &Item, ICCItem *pData, ICCItem **retpResult = NULL);
		void FireCustomOverlayEvent (const CString &sEvent, DWORD dwID, ICCItem *pData, ICCItem **retpResult = NULL);
		void FireCustomShipOrderEvent (const CString &sEvent, CSpaceObject *pShip, ICCItem **retpResult = NULL);
		bool FireGetDockScreen (CDockScreenSys::SSelector *retSelector = NULL) const;
		bool FireGetExplosionType (SExplosionType *retExplosion) const;
		bool FireGetPlayerPriceAdj (STradeServiceCtx &ServiceCtx, ICCItem *pData, int *retiPriceAdj) const;
		void FireItemOnAIUpdate (void);
		void FireItemOnDocked (CSpaceObject *pDockedAt);
		void FireItemOnObjDestroyed (const SDestroyCtx &Ctx);
		void FireItemOnUpdate (void);
		void FireOnAttacked (const SDamageCtx &Ctx);
		void FireOnAttackedByPlayer (void);
		void FireOnCreate (void);
		void FireOnCreate (const SOnCreate &OnCreate);
		void FireOnCreateOrders (CSpaceObject *pBase, CSpaceObject *pTarget);
		int FireOnDamage (SDamageCtx &Ctx, int iDamage);
		void FireOnDeselected (void);
		void FireOnDestroy (const SDestroyCtx &Ctx);
		bool FireOnDestroyCheck (DestructionTypes iCause, const CDamageSource &Attacker);
		void FireOnDestroyObj (const SDestroyCtx &Ctx);
		bool FireOnDockObjAdj (CSpaceObject **retpObj);
		void FireOnDockObjDestroyed (CSpaceObject *pDockTarget, const SDestroyCtx &Ctx);
		void FireOnEnteredGate (CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pGate);
		void FireOnEnteredSystem (CSpaceObject *pGate);
		void FireOnLoad (SLoadCtx &Ctx);
		void FireOnMining (const SDamageCtx &Ctx, EAsteroidType iType);
		void FireOnMissionAccepted (CMission *pMission);
		void FireOnMissionCompleted (CMission *pMission, const CString &sReason);
		void FireOnObjBlacklistedPlayer (CSpaceObject *pObj);
		void FireOnObjDestroyed (const SDestroyCtx &Ctx);
		void FireOnObjDocked (CSpaceObject *pObj, CSpaceObject *pDockTarget);
		void FireOnObjEnteredGate (CSpaceObject *pObj, CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate);
		bool FireOnObjGate (CSpaceObject *pObj);
		bool FireOnObjGateCheck (CSpaceObject *pObj, CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate);
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
		void FireOnSystemObjCreated (const CSpaceObject &Obj);
		void FireOnSystemObjDestroyed (SDestroyCtx &Ctx);
		void FireOnSystemStarted (DWORD dwElapsedTime);
		void FireOnSystemStopped (void);
		void FireOnSystemWeaponFire (CSpaceObject *pShot, CSpaceObject *pSource, DWORD dwItemUNID, int iRepeatingCount);
		bool FireOnTranslateMessage (const CString &sMessage, CString *retsMessage);
		void FireOnUpdate (void);
		DWORD GetAPIVersion (void) const { CDesignType *pType = GetType(); return (pType ? pType->GetAPIVersion() : API_VERSION); }
		int GetDestiny (void) const { return m_iDestiny; }
		Metric GetDetectionRange (int iPerception) const { return CPerceptionCalc::GetRange(GetDetectionRangeIndex(iPerception)); }
		Metric GetDetectionRange2 (int iPerception) const;
		int GetDetectionRangeIndex (int iPerception) const;
		CSovereign::Disposition GetDispositionTowards (const CSovereign &Sovereign) const;
		CSovereign::Disposition GetDispositionTowards (const CSpaceObject &Obj) const;
		Metric GetDistance (const CSpaceObject *pObj) const { return (pObj->GetPos() - GetPos()).Length(); }
		Metric GetDistance2 (const CSpaceObject *pObj) const { return (pObj->GetPos() - GetPos()).Length2(); }
		const CString &GetHighlightText (void) const { return m_sHighlightText; }
		DWORD GetID (void) const { return m_dwID; }
		int GetIndex (void) const { return m_iIndex; }
		CSpaceObject *GetNearestEnemyStation (Metric rMaxRange = g_InfiniteDistance);
		CSpaceObject *GetNearestStargate (bool bExcludeUncharted = false);
		CSpaceObject *GetNearestVisibleEnemy (Metric rMaxRange = g_InfiniteDistance, bool bIncludeStations = false, CSpaceObject *pExcludeObj = NULL);
		CSpaceObject *GetNearestVisibleEnemyInArc (int iMinFireArc, int iMaxFireArc, Metric rMaxRange = g_InfiniteDistance, bool bIncludeStations = false, CSpaceObject *pExcludeObj = NULL);
		CString GetNounPhrase (DWORD dwFlags = 0) const;
		CSpaceObject *GetOrderGiver (DestructionTypes iCause = killedNone);
		CDesignType *GetOverride (void) { return m_pOverride; }
		CSpaceObject *GetPlayerShip (void) const { return (m_pSystem ? m_pSystem->GetPlayerShip() : NULL); }
		const CVector &GetPos (void) const { return m_vPos; }
		CSovereign *GetSovereignToDefend (void) const;
		DWORD GetSovereignUNID (void) const { CSovereign *pSovereign = GetSovereign(); return (pSovereign ? pSovereign->GetUNID() : 0); }
		ICCItemPtr GetStaticData (const CString &sAttrib);
		CG32bitPixel GetSymbolColor (void) const;
		void GetVisibleEnemies (DWORD dwFlags, TArray<CSpaceObject *> *retList, CSpaceObject *pExcludeObj = NULL);
		CSpaceObject *GetVisibleEnemyInRange (CSpaceObject *pCenter, Metric rMaxRange = g_InfiniteDistance, bool bIncludeStations = false, CSpaceObject *pExcludeObj = NULL);
		bool HasBeenHitLately (int iTicks = 30);
		bool HasFiredLately (int iTicks = 30);
		bool HasGetDockScreenEvent (void) const { return (m_fHasGetDockScreenEvent ? true : false); }
		bool HasGravity (void) const { return (m_fHasGravity ? true : false); }
		bool HasInterSystemEvent (void) const { return (m_fHasInterSystemEvent ? true : false); }
		bool HasNonLinearMove (void) const { return (m_fNonLinearMove ? true : false); }
		bool HasOnAttackedEvent (void) const { return (m_fHasOnAttackedEvent ? true : false); }
		bool HasOnAttackedByPlayerEvent (void) const { return (m_fHasOnAttackedByPlayerEvent ? true : false); }
		bool HasOnDamageEvent (void) const { return (m_fHasOnDamageEvent ? true : false); }
		bool HasOnObjDockedEvent (void) const { return (m_fHasOnObjDockedEvent ? true : false); }
		bool HasOnOrderChangedEvent (void) const { return (m_fHasOnOrderChangedEvent ? true : false); }
		bool HasOnOrdersCompletedEvent (void) const { return (m_fHasOnOrdersCompletedEvent ? true : false); }
		bool HasOnSubordinateAttackedEvent (void) const { return (m_fHasOnSubordinateAttackedEvent ? true : false); }
		bool HasRandomEncounters (void) const { int iFreq; return (GetRandomEncounterTable(&iFreq) || iFreq > 0); }
		void Highlight (const CString &sText = NULL_STR);
		void HighlightAppend (const CString &sText = NULL_STR);
		bool InBarrier (const CVector &vPos);
		bool InDebugMode () const { return (m_fDebugMode ? true : false); }
		bool IsAngryAt (const CDamageSource &Obj) const;
		bool IsBarrier (void) const { return (m_fIsBarrier ? true : false); }
		bool IsCollisionTestNeeded (void) const { return m_fCollisionTestNeeded; }
		bool IsCovering (CSpaceObject *pObj);
		bool IsCreated (void) const { return m_fOnCreateCalled; }
		bool IsDestinyTime (int iCycle, int iOffset = 0);
		bool IsDestroyed (void) const { return (m_fDestroyed ? true : false); }
		bool IsEnemy (const CSovereign &Sovereign) const;
		bool IsEnemy (const CSpaceObject *pObj) const;
		bool IsEnemy (const CDamageSource &Obj) const;
		bool IsEnemyInRange (Metric rMaxRange, bool bIncludeStations = false);
		bool IsEscortingFriendOf (const CSpaceObject *pObj) const;
		bool IsEscorting (const CSpaceObject* pObj) const;
		bool IsFriend (const CSpaceObject *pObj) const;
		bool IsNeutral (const CSpaceObject* pObj) const;
		bool IsFriend(const CSovereign &Sovereign) const;
		bool IsNeutral(const CSovereign &Sovereign) const;
		bool IsHighlighted (void) const { return ((m_iHighlightCountdown != 0) || m_fSelected || m_iHighlightChar); }
		bool IsInDamageCode (void) const { return (m_fInDamage ? true : false); }
		bool IsMarked (void) const { return m_fMarked; }
		bool IsNamed (void) const { return m_fHasName; }
		bool IsPlayerAttackJustified (void) const;
		bool IsPlayerDocked (void) { return m_fPlayerDocked; }
		bool IsPlayerEscortTarget (CSpaceObject *pPlayer = NULL);
		bool IsSelected (void) const { return m_fSelected; }
		bool IsShowingDamageBar (void) const { return m_fShowDamageBar; }
		bool IsStargateInRange (Metric rMaxRange);
		bool IsUnderAttack (void) const;
		void NotifyOnNewSystem (CSystem *pNewSystem);
		void NotifyOnObjDestroyed (SDestroyCtx &Ctx);
		void NotifyOnObjDocked (CSpaceObject *pDockTarget);
		bool NotifyOnObjGateCheck (CSpaceObject *pGatingObj, CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pGateObj);
		bool NotifyOthersWhenDestroyed (void) { return (m_fNoObjectDestructionNotify ? false : true); }
		void OnObjDestroyed (const SDestroyCtx &Ctx);
		bool ParseConditionOptions (const ICCItem &Options, SApplyConditionOptions &retOptions) const;
		bool PointInHitSizeBox (const CVector &vPos, Metric rRadius = 0.0) const
			{ 
			CVector vRelPos = vPos - GetPos();
			Metric rHalfSize = 0.5 * GetHitSize() + rRadius;
			return ((Absolute(vRelPos.GetX()) <= rHalfSize) && (Absolute(vRelPos.GetY()) <= rHalfSize));
			}
		bool PosInBox (const CVector &vUR, const CVector &vLL) const
			{ return (vUR.GetX() > m_vPos.GetX()) 
					&& (vUR.GetY() > m_vPos.GetY())
					&& (vLL.GetX() < m_vPos.GetX())
					&& (vLL.GetY() < m_vPos.GetY()); }
		void Reconned (void);
		void RestartTime (void) { m_fTimeStop = false; }
		void SetCollisionTestNeeded (bool bNeeded = true) { m_fCollisionTestNeeded = bNeeded; }
		void SetDebugMode (bool bValue = true) { m_fDebugMode = bValue; }
		void SetDestructionNotify (bool bNotify = true) { m_fNoObjectDestructionNotify = !bNotify; }
		void SetHasGetDockScreenEvent (bool bHasEvent) { m_fHasGetDockScreenEvent = bHasEvent; }
		void SetHasOnAttackedEvent (bool bHasEvent) { m_fHasOnAttackedEvent = bHasEvent; }
		void SetHasOnAttackedByPlayerEvent (bool bHasEvent) { m_fHasOnAttackedByPlayerEvent = bHasEvent; }
		void SetHasOnDamageEvent (bool bHasEvent) { m_fHasOnDamageEvent = bHasEvent; }
		void SetHasInterSystemEvent (bool bHasEvent) { m_fHasInterSystemEvent = bHasEvent; }
		void SetHasOnObjDockedEvent (bool bHasEvent) { m_fHasOnObjDockedEvent = bHasEvent; }
		void SetHasOnOrderChangedEvent (bool bHasEvent) { m_fHasOnOrderChangedEvent = bHasEvent; }
		void SetHasOnOrdersCompletedEvent (bool bHasEvent) { m_fHasOnOrdersCompletedEvent = bHasEvent; }
		void SetHasOnSubordinateAttackedEvent (bool bHasEvent) { m_fHasOnSubordinateAttackedEvent = bHasEvent; }
		void SetHighlightChar (char chChar) { m_iHighlightChar = chChar; }
		void SetMarked (bool bMarked = true) { m_fMarked = bMarked; }
		void SetNamed (bool bNamed = true) { m_fHasName = bNamed; }
		void SetNoFriendlyTarget (bool bValue = true) { m_fNoFriendlyTarget = bValue; }
		void SetOverride (CDesignType *pOverride);

		void SetPlayerDocked (void) { m_fPlayerDocked = true; }
		bool SetPOVLRS (void)
			{
			if (m_fInPOVLRS)
				return false;

			m_fInPOVLRS = true;
			return true;
			}
		void SetSelection (void) { m_fSelected = true; }
		void SetShowDamageBar (void) { m_fShowDamageBar = true; }
		void StopTime (void) { m_fTimeStop = true; }
		void Update (SUpdateCtx &Ctx);
		void UpdateExtended (const CTimeSpan &ExtraTime);
		void UpdatePlayer (SUpdateCtx &Ctx) { OnUpdatePlayer(Ctx); }
		void WriteToStream (IWriteStream *pStream);
		void WriteObjRefToStream (CSpaceObject *pObj, IWriteStream *pStream) const;

		bool MatchesCriteria (const CSpaceObjectCriteria &Crit) const { CSpaceObjectCriteria::SCtx Ctx(NULL, Crit); return MatchesCriteria(Ctx, Crit); }
		bool MatchesCriteria (CSpaceObjectCriteria::SCtx &Ctx, const CSpaceObjectCriteria &Crit) const;
		bool MatchesCriteriaCategory (CSpaceObjectCriteria::SCtx &Ctx, const CSpaceObjectCriteria &Crit) const;

		//	Mission Acceptors

		virtual void OnAcceptedMission (CMission &MissionObj) { }

		//	Motion
		//
		//	CanMove: This returns TRUE if OnMove should be called for the object.
		//		Note that ships need to be called at OnMove even if they are
		//		anchored because they update their particle effects during Move.
		//		But for most cases, using IsAnchored is better than CanMove.
		//
		//	IsAnchored: Anchored objects don't move even if pushed e.g., by
		//		momentum or gravity). Objects can be temporarily anchored
		//		(e.g., when docked), so it is possible for an object to return
		//		TRUE for both CanThrust and IsAnchored.

		virtual bool CanMove (void) const { return !IsAnchored(); }
		virtual bool CanThrust (void) const { return false; }
		virtual Metric GetMaxSpeed (void) const { return (IsAnchored() ? 0.0 : MAX_SYSTEM_SPEED); }
		virtual bool IsAnchored (void) const { return IsManuallyAnchored(); }

		void Accelerate (const CVector &vForce, Metric rSeconds);
		void AccelerateStop (Metric rPush, Metric rSeconds);
		void AddDrag (Metric rDragFactor);
		void AddForce (const CVector &vForce);
		void AddForceFromDeltaV (const CVector &vDeltaV);
		void AddForceLimited (const CVector &vForce);
		void ClearForceDesc (void) { m_ForceDesc.Clear(); }
		void ClipSpeed (Metric rMaxSpeed) { m_vVel.Clip(rMaxSpeed); }
		const CVector &DeltaV (const CVector &vDelta) { m_vVel = m_vVel + vDelta; return m_vVel; }
		const CVector &GetOldPos (void) const { return m_vOldPos; }
		const CVector &GetVel (void) const { return m_vVel; }
		bool IsInsideBarrier (void) const { return m_fInsideBarrier; }
		bool IsManuallyAnchored (void) const { return m_fManualAnchor; }
		void Jump (const CVector &vPos);
		void Move (SUpdateCtx &Ctx, Metric rSeconds);
		void Place (const CVector &vPos, const CVector &vVel = NullVector) { CVector vOldPos = m_vPos; m_vPos = vPos; m_vOldPos = vPos; m_vVel = vVel; OnPlace(vOldPos); }
		void SetInsideBarrier (bool bInside = true) { m_fInsideBarrier = bInside; }
		void SetManualAnchor (bool bAnchored = true) { m_fManualAnchor = bAnchored; }
		void SetPos (const CVector &vPos) { m_vPos = vPos; }
		void SetVel (const CVector &vVel) { m_vVel = vVel; }

		//	Overlays

		virtual void AddOverlay (COverlayType *pType, int iPosAngle, int iPosRadius, int iRotation, int iLifetime, int iPosZ, DWORD *retdwID = NULL) { if (retdwID) *retdwID = 0; }
		virtual COverlayList *GetOverlays (void) { return NULL; }
		virtual const COverlayList *GetOverlays (void) const { return NULL; }
		virtual void OnOverlayConditionChanged (ECondition iCondition, EConditionChange iChange) { }
		virtual void RemoveOverlay (DWORD dwID) { }

		void AddOverlay (COverlayType *pType, const CVector &vPos, int iRotation, int iPosZ, int iLifetime, DWORD *retdwID = NULL);
		void AddOverlay (DWORD dwUNID, int iPosAngle, int iPosRadius, int iRotation, int iPosZ, int iLifetime, DWORD *retdwID = NULL);
		void CalcOverlayPos (COverlayType *pOverlayType, const CVector &vPos, int *retiPosAngle, int *retiPosRadius);
		COverlay *GetOverlay (DWORD dwID) { COverlayList *pOverlays = GetOverlays(); return (pOverlays ? pOverlays->GetOverlay(dwID) : NULL); }
		ICCItemPtr GetOverlayData (DWORD dwID, const CString &sAttrib) const;
		bool GetOverlayImpact (COverlay::SImpactDesc &Impact) { COverlayList *pOverlays = GetOverlays(); if (pOverlays) return pOverlays->GetImpact(this, Impact); else return false; }
		void GetOverlayList (TArray<COverlay *> *retList) { COverlayList *pOverlays = GetOverlays(); if (pOverlays) pOverlays->GetList(retList); else retList->DeleteAll(); }
		void GetOverlayListOfCommandPaneCounters (TArray<COverlay *> *retList) { COverlayList *pOverlays = GetOverlays(); if (pOverlays) pOverlays->GetListOfCommandPaneCounters(retList); else retList->DeleteAll(); }
		CVector GetOverlayPos (DWORD dwID) { COverlayList *pOverlays = GetOverlays(); return (pOverlays ? pOverlays->GetPos(this, dwID) : GetPos()); }
		ICCItem *GetOverlayProperty (CCodeChainCtx *pCCCtx, DWORD dwID, const CString &sName);
		int GetOverlayRotation (DWORD dwID) { COverlayList *pOverlays = GetOverlays(); return (pOverlays ? pOverlays->GetRotation(dwID) : -1); }
		COverlayType *GetOverlayType (DWORD dwID) { COverlayList *pOverlays = GetOverlays(); return (pOverlays ? pOverlays->GetType(dwID) : NULL); }
		void SetOverlayData (DWORD dwID, const CString &sAttribute, ICCItem *pData) { COverlayList *pOverlays = GetOverlays(); if (pOverlays) pOverlays->SetData(dwID, sAttribute, pData); }
		bool SetOverlayEffectProperty (DWORD dwID, const CString &sProperty, ICCItem *pValue) { COverlayList *pOverlays = GetOverlays(); return (pOverlays ? pOverlays->SetEffectProperty(dwID, sProperty, pValue) : false); }
		void SetOverlayPos (DWORD dwID, const CVector &vPos) { COverlayList *pOverlays = GetOverlays(); if (pOverlays) pOverlays->SetPos(this, dwID, vPos); }
		bool SetOverlayProperty (DWORD dwID, const CString &sName, ICCItem *pValue, CString *retsError) { COverlayList *pOverlays = GetOverlays(); return (pOverlays ? pOverlays->SetProperty(this, dwID, sName, pValue) : false); }
		void SetOverlayRotation (DWORD dwID, int iRotation) { COverlayList *pOverlays = GetOverlays(); if (pOverlays) pOverlays->SetRotation(dwID, iRotation); }

		//	Painting

		virtual bool IsPaintDeferred (SViewportPaintCtx &Ctx) const { return false; }
		virtual CSystem::LayerEnum GetPaintLayer (void) const { return CSystem::layerStations; }
		virtual Metric GetVisualMaxRange () const { return CPerceptionCalc::STD_VISUAL_RANGE * LIGHT_SECOND; }
		virtual void PaintLRSBackground (CG32bitImage &Dest, int x, int y, const ViewportTransform &Trans) { }
		virtual void PaintLRSForeground (CG32bitImage &Dest, int x, int y, const ViewportTransform &Trans);

		bool CanBePOV (void) const { return (m_pSystem && !IsDestroyed()); }
		DWORD CalcSRSVisibility (const CSpaceObject &ObserverObj, int iObserverPerception) const;
		void ClearPaintNeeded (void) { m_fPaintNeeded = false; }
		const CImageFilterStack *GetSystemFilters (void) const;
		bool Is3DExtra (void) const { return (m_f3DExtra ? true : false); }
		bool IsOutOfPlaneObj (void) const { return m_fOutOfPlaneObj; }
		bool IsPaintNeeded (void) { return m_fPaintNeeded; }
		void Paint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);
		void PaintAnnotationText (CG32bitImage &Dest, int x, int y, const CString &sText, SViewportPaintCtx &Ctx) const;
		void PaintHighlightText (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx, AlignmentStyles iAlign, CG32bitPixel rgbColor, int *retcyHeight = NULL);
		void PaintMap (CMapViewportCtx &Ctx, CG32bitImage &Dest, int x, int y);
		void PaintSRSEnhancements (CG32bitImage &Dest, SViewportPaintCtx &Ctx) { OnPaintSRSEnhancements(Dest, Ctx); }
		void Set3DExtra (bool bValue = true) { m_f3DExtra = bValue; }
		void SetOutOfPlaneObj (bool bValue = true) { m_fOutOfPlaneObj = bValue; }
		void SetPainted (void) { m_fPainted = true; }
		void SetPaintNeeded (void) { m_fPaintNeeded = true; }
		bool WasPainted (void) const { return m_fPainted; }

		//	Player Destination

		void ClearPlayerDestination (void) { m_fPlayerDestination = false; m_fAutoClearDestination = false; m_fAutoClearDestinationOnDock = false; m_fAutoClearDestinationOnDestroy = false; m_fAutoClearDestinationOnGate = false; m_fShowDistanceAndBearing = false; m_fShowHighlight = false; }
		bool GetPlayerDestinationOptions (SPlayerDestinationOptions &retOptions) const;
		bool IsAutoClearDestination (void) const { return m_fAutoClearDestination; }
		bool IsAutoClearDestinationOnDestroy (void) const { return m_fAutoClearDestinationOnDestroy; }
		bool IsAutoClearDestinationOnDock (void) const { return m_fAutoClearDestinationOnDock; }
		bool IsAutoClearDestinationOnGate (void) const { return m_fAutoClearDestinationOnGate; }
		bool IsPlayerDestination (void) const { return m_fPlayerDestination; }
		bool IsShowingDistanceAndBearing (void) const { return m_fShowDistanceAndBearing; }
		bool IsShowingHighlight (void) const { return m_fShowHighlight; }
		void SetPlayerDestination (const SPlayerDestinationOptions &Options);

		//	Powers

		bool InvokePower (CPower &Power, CSpaceObject *pTarget);

		//	Properties

		ICCItemPtr GetProperty (CCodeChainCtx &CCX, const CString &sProperty) const;
		ICCItemPtr GetProperty (const CString &sProperty) const { CCodeChainCtx CCX(GetUniverse()); return GetProperty(CCX, sProperty); }
		bool IncProperty (const CString &sProperty, ICCItem *pInc, ICCItemPtr &pResult);
		virtual bool SetProperty (const CString &sName, ICCItem *pValue, CString *retsError);

		//	Ships

		virtual void GetHUDTimers (TArray<SHUDTimerDesc> &retTimers) const { }
		virtual const CShipPerformanceDesc &GetShipPerformance (void) const { return CShipPerformanceDesc::Null(); }

		//	Stargates (object is a stargate)

		virtual CString GetStargateID (void) const { return NULL_STR; }
		virtual bool IsActiveStargate (void) const { return false; }
		virtual bool IsStargate (void) const { return false; }
		virtual bool RequestGate (CSpaceObject *pObj);
		virtual bool SupportsGating (void) { return false; }

		Metric GetMaxGateDist2 (void) const;

		//	Statics

		static int ConvertToCompatibleIndex (const CItem &Item, InstallItemResults iResult);
		static CString ConvertToID (InstallItemResults iResult);

		//	Stations

		virtual const CAsteroidDesc &GetAsteroidDesc (void) const { return CAsteroidDesc::Null(); }

		//	Trade

		virtual CTradingDesc *AllocTradeDescOverride (void) { return NULL; }
		virtual const CCurrencyBlock *GetCurrencyBlock (void) const { return NULL; }
		virtual CCurrencyBlock *GetCurrencyBlock (bool bCreate = false) { return NULL; }
		virtual const CRegenDesc &GetDefaultShipRepair () const { return CRegenDesc::Null; }
		virtual CTradingDesc *GetTradeDescOverride (void) const { return NULL; }
		virtual CCurrencyAndValue GetTradePrice (const CSpaceObject *pProvider) const { return CCurrencyAndValue(0, GetDefaultEconomy()); }

		CTradingServices GetTradeServices () const { return CTradingServices(*this); }
		void AddBuyOrder (CItemType *pType, const CString &sCriteria, int iPriceAdj);
		void AddSellOrder (CItemType *pType, const CString &sCriteria, int iPriceAdj);
		void AddTradeDesc (const CTradingDesc &Trade);
		void AddTradeOrder (ETradeServiceTypes iService, const CString &sCriteria, CItemType *pItemType, int iPriceAdj);
		SRefitObjCtx CalcRefitObjCtx (int iTick, int iRepairCycle) const;
		bool CanRefitObj (const CSpaceObject &ShipObj, const SRefitObjCtx &Ctx, int iMinDamage = 0) const;
		CurrencyValue ChargeMoney (DWORD dwEconomyUNID, CurrencyValue iValue);
		CurrencyValue CreditMoney (DWORD dwEconomyUNID, CurrencyValue iValue);
		CurrencyValue GetBalance (DWORD dwEconomyUNID) const;
		int GetBuyPrice (const CItem &Item, DWORD dwFlags, int *retiMaxCount = NULL);
		CItemList GetConsumablesNeeded (const CSpaceObject &Base) const;
		const CEconomyType *GetDefaultEconomy (void) const;
		DWORD GetDefaultEconomyUNID (void) const;
		bool GetRefuelItemAndPrice (CSpaceObject *pObjToRefuel, CItemType **retpItemType, int *retiPrice);
		int GetSellPrice (const CItem &Item, DWORD dwFlags);
		bool GetShipBuyPrice (CShipClass *pClass, DWORD dwFlags, int *retiPrice);
		bool GetShipBuyPrice (CSpaceObject *pShip, DWORD dwFlags, int *retiPrice);
		bool GetShipSellPrice (CShipClass *pClass, DWORD dwFlags, int *retiPrice);
		bool GetShipSellPrice (CSpaceObject *pShip, DWORD dwFlags, int *retiPrice);
		bool HasTradeService (ETradeServiceTypes iService, const CTradingDesc::SHasServiceOptions &Options = CTradingDesc::SHasServiceOptions());
		bool HasTradeUpgradeOnly (ETradeServiceTypes iService) const;
		void RecordBuyItem (CSpaceObject *pSellerObj, const CItem &Item, const CCurrencyAndValue &Price);
		void RefitDockedObjs (int iTick, int iRepairCycle);
		void RefitObj (CSpaceObject &ShipObj, const SRefitObjCtx &Ctx);
		void SetTradeDesc (const CEconomyType *pCurrency, int iMaxCurrency, int iReplenishCurrency);

		//	Wingmen

		bool IsInOurSquadron (const CSpaceObject &Obj) const;
		virtual bool IsPlayerEscort (void) const { return false; }
		virtual bool IsPlayerWingman (void) const { return false; }

		//	DEBUG

		virtual CString DebugCrashInfo (void) const { return NULL_STR; }

#ifdef DEBUG_VECTOR
		void PaintDebugVector (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);
		void SetDebugVector (const CVector &vVector) { m_vDebugVector = vVector; }
#else
		void PaintDebugVector (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) { }
		void SetDebugVector (const CVector &vVector) { }
#endif

		//	Other virtuals to be overridden

		//	...for all objects

		virtual CBoundaryMarker *AsBoundaryMarker (void) { return NULL; }
		virtual CMissile *AsMissile (void) { return NULL; }
		virtual CMission *AsMission (void) { return NULL; }
		virtual CShip *AsShip (void) { return NULL; }
		virtual const CShip *AsShip (void) const { return NULL; }
		virtual CStation *AsStation (void) { return NULL; }
		virtual bool CalcVolumetricShadowLine (SLightingCtx &Ctx, int *retxCenter, int *retyCenter, int *retiWidth, int *retiLength) { return false; }
		virtual bool CanAttack (void) const { return false; }
		virtual bool CanBeAttacked (void) const { return false; }
		virtual bool CanBeDestroyed (void) { return true; }
		virtual bool CanBeHitBy (const DamageDesc &Damage) { return true; }
		virtual bool CanHit (CSpaceObject *pObj) const { return true; }
		virtual bool ClassCanAttack (void) { return false; }
		virtual bool FindDataField (const CString &sField, CString *retsValue) { return false; }
		virtual Categories GetCategory (void) const { return catOther; }
		virtual DWORD GetClassUNID (void) { return 0; }
		virtual Metric GetGravity (Metric *retrRadius) const { return 0.0; }
		CInteractionLevel GetInteraction (void) const { const CWeaponFireDesc *pDesc = GetWeaponFireDesc(); return (pDesc ? pDesc->GetInteraction() : CInteractionLevel(-1)); }
		virtual Metric GetInvMass (void) const { return 0.0; }
		virtual const COrbit *GetMapOrbit (void) const { return NULL; }
		virtual Metric GetMass (void) const { return 0.0; }
		virtual CString GetNamePattern (DWORD dwNounPhraseFlags = 0, DWORD *retdwFlags = NULL) const { if (retdwFlags) *retdwFlags = 0; return LITERAL("unknown object"); }
		virtual CString GetObjClassName (void) const { return CONSTLIT("unknown"); }
		virtual Metric GetParallaxDist (void) { return 0.0; }
		virtual EDamageResults GetPassthroughDefault (void) { return damageNoDamage; }
		virtual int GetPlanetarySize (void) const { return 0; }
		virtual ScaleTypes GetScale (void) const { return scaleFlotsam; }
		virtual CSovereign *GetSovereign (void) const { return NULL; }
		virtual Metric GetStellarMass (void) const { return 0.0; }
		virtual CDesignType *GetWreckType (void) const { return NULL; }
		virtual bool HasAttribute (const CString &sAttribute) const { return sAttribute.IsBlank(); }
		virtual bool HasSpecialAttribute (const CString &sAttrib) const;
		virtual bool IsExplored (void) const { return true; }
		virtual bool IsImmutable (void) const { return false; }
		virtual bool IsKnown (void) const { return true; }
		virtual bool IsMarker (void) const { return false; }
		virtual bool IsMission (void) const { return false; }
		virtual bool IsNonSystemObj (void) { return false; }
		virtual bool IsShownInGalacticMap (void) const { return false; }
		virtual bool IsVirtual (void) const { return false; }
		virtual bool IsWreck (void) const { return false; }
		virtual void OnPlayerChangedShips (CSpaceObject *pOldShip, SPlayerChangedShipsCtx &Options) { }
		virtual void OnSystemCreated (SSystemCreateCtx &CreateCtx) { }
		virtual void OnSystemLoaded (SLoadCtx &Ctx) { }
		virtual bool PointInObject (const CVector &vObjPos, const CVector &vPointPos) const { return false; }
		virtual bool PointInObject (SPointInObjectCtx &Ctx, const CVector &vObjPos, const CVector &vPointPos) const { return PointInObject(vObjPos, vPointPos); }
		virtual void PointInObjectInit (SPointInObjectCtx &Ctx) const { }
		virtual void SetExplored (bool bExplored = true) { }
		virtual void SetKnown (bool bKnown = true) { }
		virtual void SetName (const CString &sName, DWORD dwFlags = 0) { }

		virtual bool IsHidden (void) const { return false; }
		virtual bool IsInactive (void) const { return (IsSuspended() || IsDestroyed()); }
		virtual bool IsIntangible (void) const { return (IsVirtual() || IsSuspended() || IsDestroyed()); }
		virtual bool IsSuspended (void) const { return false; }
		virtual bool IsUnreal (void) const { return (IsSuspended() || IsDestroyed()); }

		//	...for active/intelligent objects (ships, stations, etc.)

		virtual void DeactivateShields (void) { }
		virtual void DepleteShields (void) { }
		virtual CInstalledArmor *FindArmor (const CItem &Item) { return NULL; }
		virtual int GetAISettingInteger (const CString &sSetting) { return 0; }
		virtual CString GetAISettingString (const CString &sSetting) { return NULL_STR; }
		virtual const CArmorSystem &GetArmorSystem (void) const { return CArmorSystem::m_Null; }
		virtual CArmorSystem &GetArmorSystem (void) { return CArmorSystem::m_Null; }
		virtual CurrencyValue GetBalancedTreasure (void) const { return 0; }
		virtual Metric GetCargoSpaceLeft (void) const { return 1000000.0; }
		virtual int GetCombatPower (void) { return 0; }
		virtual int GetCounterIncrementRate (void) const { return 0; }
		virtual bool GetCounterIsHeat (void) const { return false; }
		virtual int GetCounterValue (void) const { return 0; }
		virtual int GetCyberDefenseLevel (void) const { return GetLevel(); }
		virtual int GetDamageEffectiveness (CSpaceObject *pAttacker, CInstalledDevice *pWeapon) { return 0; }
		virtual DamageTypes GetDamageType (void) { return damageGeneric; }
		virtual CSpaceObject *GetDestination (void) const { return NULL; }
		virtual CStationType *GetEncounterInfo (void) { return NULL; }
		virtual CSpaceObject *GetEscortPrincipal (void) const { return NULL; }
		virtual int GetLastFireTime (void) const { return 0; }
		virtual int GetLastHitTime (void) const { return 0; }
		virtual int GetLevel (void) const { return 1; }
		virtual int GetMaxCounterValue (void) const { return 0; }
		virtual int GetMaxPower (void) const { return 0; }
		virtual int GetMaxLightDistance (void) const { return 0; }
		virtual Metric GetMaxWeaponRange (void) const { return 0.0; }
		virtual int GetPerception (void) const { return perceptNormal; }
		virtual int GetScore (void) { return 0; }
		virtual int GetShieldLevel (void) const { return -1; }
		virtual CG32bitPixel GetSpaceColor (void) { return 0; }
		virtual int GetStealth (void) const { return stealthNormal; }
		virtual int GetStealthAdj (void) const { return 0; }
		virtual int GetStealthAdjAtMaxHeat (void) const { return 0; }
		virtual int GetVisibleDamage (void) const { return 0; }
		virtual void GetVisibleDamageDesc (SVisibleDamage &Damage) const { Damage = SVisibleDamage(); }
		virtual void IncCounterValue(int iCounterValue) { }
		virtual bool IsAngry (void) { return false; }
		virtual bool IsAngryAt (const CSpaceObject *pObj) const { return IsEnemy(pObj); }
		virtual bool IsIdentified (void) const { return true; }
		virtual bool IsMultiHull (void) const { return false; }
		virtual bool IsPlayer (void) const { return false; }
		virtual void OnComponentChanged (ObjectComponentTypes iComponent) { }
		virtual bool OnDestroyCheck (DestructionTypes iCause, const CDamageSource &Attacker) { return true; }
		virtual bool OnGateCheck (CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pGateObj) { return true; }
		virtual void OnHitByDeviceDamage (void) { }
		virtual void OnHitByDeviceDisruptDamage (DWORD dwDuration) { }
		virtual void OnMissionCompleted (CMission *pMission, bool bSuccess) { }
		virtual void OnObjHit (SDamageCtx &Ctx) { }
		virtual void OnObjDestroyedNotify (SDestroyCtx &Ctx) { FireOnObjDestroyed(Ctx); }
		virtual void OnObjLeaveGate (CSpaceObject *pObj) { }
		virtual void OnPlayerObj (CSpaceObject *pPlayer) { }
		virtual void OnStationDestroyed (const SDestroyCtx &Ctx) { }
		virtual void OnSubordinateDestroyed (SDestroyCtx &Ctx) { }
		virtual void OnSubordinateHit (SDamageCtx &Ctx) { }
		virtual void ProgramDamage (CSpaceObject *pHacker, const ProgramDesc &Program) { }
		virtual void SendMessage (const CSpaceObject *pSender, const CString &sMsg) const { }
		virtual int SetAISettingInteger (const CString &sSetting, int iValue) { return 0; }
		virtual CString SetAISettingString (const CString &sSetting, const CString &sValue) { return NULL_STR; }
		virtual void SetCounterValue(int iCounterValue) { }
		virtual void SetIdentified (bool bIdentified = true) { }
		virtual void SetMapLabelPos (CMapLabelPainter::EPositions iPos) { }
		virtual void UpdateArmorItems (void) { }

		//	...for objects that can bounce

		bool CanBounce (void) { return m_fCanBounce; }
		virtual bool ImageInObject (const CVector &vObjPos, const CObjectImageArray &Image, int iTick, int iRotation, const CVector &vImagePos) { return PointInObject(vObjPos, vImagePos); }
		virtual bool ObjectInObject (const CVector &vObj1Pos, CSpaceObject *pObj2, const CVector &vObj2Pos) { return PointInObject(vObj1Pos, vObj2Pos); }
		virtual void OnBounce (CSpaceObject *pBarrierObj, const CVector &vPos) { }
		virtual void OnObjBounce (CSpaceObject *pObj, const CVector &vPos) { }

		//	...for objects with devices

		virtual bool AbsorbWeaponFire (CInstalledDevice *pWeapon) { return false; }
		virtual CString GetInstallationPhrase (const CItem &Item) const { return NULL_STR; }
		virtual void SetFireDelay (CInstalledDevice *pWeapon, int iDelay = -1) { }

		//	...for beams, missiles, etc.

		virtual void AddContinuousBeam (const CVector &vPos, const CVector &vVel, int iDirection) { }
		virtual void CreateReflection (const CVector &vPos, int iDirection, CMissile **retpReflection = NULL) { }
		virtual void DetonateNow (CSpaceObject *pHit) { }
		virtual CString GetDamageCauseNounPhrase (DWORD dwFlags) { return GetNounPhrase(dwFlags); }
		virtual const CDamageSource &GetDamageSource (void) const { return CDamageSource::Null(); }
		virtual const CWeaponFireDesc *GetWeaponFireDesc (void) const { return NULL; }
		virtual CSpaceObject *GetSecondarySource (void) const { return NULL; }
		virtual bool IsTargetableProjectile (void) const { return false; }

		//	...for ships

		virtual void Behavior (SUpdateCtx &Ctx) { }
		virtual bool FollowsObjThroughGate (CSpaceObject *pLeader = NULL) { return false; }
		virtual CSpaceObject *GetBase (void) const { return NULL; }
		virtual int GetRotation (void) const { return 0; }
		virtual int GetRotationFrameIndex (void) const { return 0; }
		virtual void RepairDamage (int iHitPoints) { }
		virtual void Resume (void) { }
		virtual void Suspend (void) { }
		void ScrapeOverlays ();

		//	...for stations

		virtual void AddSubordinate (CSpaceObject &SubordinateObj, const CString &sSubordinateID = NULL_STR) { }
		virtual IShipGenerator *GetRandomEncounterTable (int *retiFrequency = NULL) const { if (retiFrequency) *retiFrequency = 0; return NULL; }
		virtual const CString &GetSubordinateID (void) const { return NULL_STR; }
		virtual bool IsAbandoned (void) const { return false; }
		virtual bool IsSatelliteSegmentOf (const CSpaceObject &Base, CPaintOrder::Types *retiPaintOrder = NULL) const { return false; }
		virtual bool RemoveSubordinate (CSpaceObject *pSubordinate) { return false; }
		virtual bool ShowMapLabel (int *retcxLabel = NULL, int *retcyLabel = NULL) const { return false; }
		virtual bool ShowMapOrbit (void) const { return false; }
		virtual bool ShowStationDamage (void) const { return false; }

		//	...for particle effects

		virtual void SetAttractor (CSpaceObject *pObj) { }

		//	...miscellaneous

		virtual void OnLosePOV (void) { }

	protected:

		//	Virtuals to be overridden

		virtual bool CanBlock (CSpaceObject *pObj) { return false; }
		virtual bool CanBlockShips (void) { return false; }
		virtual bool CanFireOn (CSpaceObject *pObj) const { return true; }
		virtual DWORD GetClassID (void) const = 0;
		virtual CDesignType *GetDefaultDockScreen (CString *retsName = NULL, ICCItemPtr *retpData = NULL) const { return NULL; }
		virtual void GateHook (CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate, bool bAscend) { if (!bAscend) Destroy(removedFromSystem, CDamageSource()); }
		virtual CDesignType *GetDefaultOverride (void) const { return NULL; }
		virtual ICCItem *GetPropertyCompatible (CCodeChainCtx &Ctx, const CString &sName) const;
		virtual void ObjectDestroyedHook (const SDestroyCtx &Ctx) { }
		virtual void ObjectEnteredGateHook (CSpaceObject *pObjEnteredGate) { }
		virtual EConditionResult OnApplyCondition (ECondition iCondition, const SApplyConditionOptions &Options) { return EConditionResult::noEffect; }
		virtual void OnAscended (void) { }
		virtual EConditionResult OnCanApplyCondition (ECondition iCondition, const SApplyConditionOptions &Options) const { return EConditionResult::noEffect; }
		virtual EConditionResult OnCanRemoveCondition (ECondition iCondition, const SApplyConditionOptions &Options) const { return EConditionResult::noEffect; }
		virtual DWORD OnCommunicate (CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2, ICCItem *pData) { return resNoAnswer; }
		virtual EDamageResults OnDamage (SDamageCtx &Ctx) { return damageNoDamage; }
		virtual void OnDestroyed (SDestroyCtx &Ctx) { }
		virtual ICCItemPtr OnFindProperty (CCodeChainCtx &CCX, const CString &sProperty) const { return ICCItemPtr(); }
		virtual bool OnGetCondition (ECondition iCondition) const { return false; }
		virtual CSpaceObject *OnGetOrderGiver (void) { return this; }
		virtual bool OnIncProperty (const CString &sProperty, ICCItem *pValue, ICCItemPtr &pResult) { return false; }
		virtual bool OnIsImmuneTo (SpecialDamageTypes iSpecialDamage) const { return false; }
		virtual void OnItemEnhanced (CItemListManipulator &ItemList) { }
		virtual void OnMove (const CVector &vOldPos, Metric rSeconds) { }
		virtual void OnNewSystem (CSystem *pSystem) { }
		virtual void OnObjEnteredGate (CSpaceObject *pObj, CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate) { }
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) { }
		virtual void OnPaintAnnotations (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) { }
		virtual void OnPaintMap (CMapViewportCtx &Ctx, CG32bitImage &Dest, int x, int y) { }
		virtual void OnPaintSRSEnhancements (CG32bitImage &Dest, SViewportPaintCtx &Ctx) { }
		virtual void OnPlace (const CVector &vOldPos) { }
		virtual void OnReadFromStream (SLoadCtx &Ctx) { }
		virtual EConditionResult OnRemoveCondition (ECondition iCondition, const SApplyConditionOptions &Options) { return EConditionResult::noEffect; }
		virtual void OnRemoved (SDestroyCtx &Ctx) { }
		virtual void OnSetEventFlags (void) { }
		virtual void OnSetSovereign (CSovereign *pSovereign) { }
		virtual void OnUpdate (SUpdateCtx &Ctx, Metric rSecondsPerTick) { }
		virtual void OnUpdateExtended (const CTimeSpan &ExtraTime) { }
		virtual void OnUpdatePlayer (SUpdateCtx &Ctx) { }
		virtual void OnWriteToStream (IWriteStream *pStream) { }
		virtual bool OrientationChanged (void) { return false; }
		virtual void RevertOrientationChange (void) { }

		//	Freeze controls (used for docking)

		bool CanBeControlled (void) { return m_iControlsFrozen == 0; }
		void FreezeControls (void) { m_iControlsFrozen++; }
		void UnfreezeControls (void) { m_iControlsFrozen--; }

		//	Helper functions
		void AddEffect (IEffectPainter *pPainter, int xOffset, int yOffset, int iTick = 0, int iRotation = 0);
		void CalcInsideBarrier (void);
		Metric CalculateItemMass (Metric *retrCargoMass = NULL) const;
		bool CanFireOnObjHelper (CSpaceObject *pObj) const;
		void ClearCannotBeHit (void) { m_fCannotBeHit = false; }
		void ClearInDamageCode (void) { m_fInDamage = false; }
		void ClearNoFriendlyFire(void) { m_fNoFriendlyFire = false; }
		void ClearPainted (void) { m_fPainted = false; }
		void DisableObjectDestructionNotify (void) { m_fNoObjectDestructionNotify = true; }
		const CEnhancementDesc *GetSystemEnhancements (void) const;
		ICCItemPtr GetTypeProperty (CCodeChainCtx &CCX, const CString &sProperty) const;
		CSpaceObject *HitTest (const CVector &vStart, const DamageDesc &Damage, CVector *retvHitPos, int *retiHitDir);
		CSpaceObject *HitTestProximity (const CVector &vStart, Metric rMinThreshold, Metric rMaxThreshold, const DamageDesc &Damage, const CTargetList::STargetOptions &TargetOptions, const CSpaceObject *pTarget, CVector *retvHitPos, int *retiHitDir);
		bool ImagesIntersect (const CObjectImageArray &Image1, int iTick1, int iRotation1, const CVector &vPos1,
				const CObjectImageArray &Image2, int iTick2, int iRotation2, const CVector &vPos2);
		bool IsObjectDestructionHooked (void) { return (m_fHookObjectDestruction ? true : false); }
		void ItemEnhancementModified (CItemListManipulator &ItemList) { OnItemEnhanced(ItemList); }
		bool MissileCanHitObj (const CSpaceObject &Obj, const CDamageSource &Source, const CWeaponFireDesc &Desc, CSpaceObject *pTarget = NULL) const;
		void PaintEffects (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);
		void PaintHighlight (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);
		void PaintTargetHighlight (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);
		void SetAutoCreatedPorts (bool bValue = true) { m_fAutoCreatedPorts = bValue; }
		void SetObjectDestructionHook (void) { m_fHookObjectDestruction = true; }
		void SetCannotBeHit (void) { m_fCannotBeHit = true; }
		void SetCanBounce (void) { m_fCanBounce = true; }
		void SetDestroyed (bool bValue = true) { m_fDestroyed = bValue; }
		void SetHasGravity (bool bGravity = true) { m_fHasGravity = bGravity; }
		void SetIsBarrier (void) { m_fIsBarrier = true; }
		void SetInDamageCode (void) { m_fInDamage = true; }
		void SetNoFriendlyFire (void) { m_fNoFriendlyFire = true; }
		void SetNonLinearMove (bool bValue = true) { m_fNonLinearMove = bValue; }
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

		bool FindCustomProperty (const CString &sProperty, ICCItemPtr &pResult) const;
		void InitItemEvents (void) { m_ItemEvents.Init(this); m_fItemEventsValid = true; }
		void UpdateEffects (void);

		static CSpaceObject *CreateFromClassID (CUniverse &Universe, DWORD dwClass);

		CUniverse &m_Universe;							//	Our universe
		CSystem *m_pSystem = NULL;						//	Current system (may be NULL)
		int m_iIndex = -1;								//	Index in system
		DWORD m_dwID = 0;								//	Universal ID
		int m_iDestiny = 0;								//	Random number 0..DestinyRange-1
		CVector m_vPos;									//	Position of object in system
		CVector m_vVel;									//	Velocity of object
		CVector m_vOldPos;								//	Position last tick
		Metric m_rBoundsX = 0.0;						//	Object bounds
		Metric m_rBoundsY = 0.0;						//	Object bounds

		CString m_sHighlightText;						//	Show message text when highlighting
		int m_iHighlightCountdown:16 = 0;				//	Frames left to highlight
		int m_iHighlightChar:8 = 0;						//	Show character when painting (0 = none)
		int m_iDesiredHighlightChar:8 = 0;				//	Desired character

		CItemList m_ItemList;							//	List of items
		CAttributeDataBlock m_Data;						//	Opaque data
		SEffectNode *m_pFirstEffect = NULL;				//	List of effects
		CItemEventDispatcher m_ItemEvents;				//	Item event dispatcher
		CDesignType *m_pOverride = NULL;				//	Override event handler
		CSpaceObjectList m_SubscribedObjs;				//	List of objects to notify when something happens
		CObjectJoint *m_pFirstJoint = NULL;				//	List of joints
		CPhysicsForceDesc m_ForceDesc;					//	Temporary; valid only inside Update.

		int m_iControlsFrozen:8 = 0;					//	Object will not respond to controls
		int m_iSpare:24 = 0;

		DWORD m_fHookObjectDestruction:1 = false;		//	Call the object when another object is destroyed
		DWORD m_fNoObjectDestructionNotify:1 = false;	//	Do not call other objects when this one is destroyed
		DWORD m_fCannotBeHit:1 = false;					//	TRUE if it cannot be hit by other objects
		DWORD m_fSelected:1 = false;					//	TRUE if selected
		DWORD m_fInPOVLRS:1 = false;					//	TRUE if object appears in POV's LRS
		DWORD m_fCanBounce:1 = false;					//	TRUE if object can bounce off others
		DWORD m_fIsBarrier:1 = false;					//	TRUE if objects bounce off this object
		DWORD m_fNoFriendlyFire:1 = false;				//	TRUE if object cannot hit friendly objects

		DWORD m_fTimeStop:1 = false;					//	TRUE if time has stopped for this object
		DWORD m_fPlayerTarget:1 = false;				//	TRUE if this is a target for the player
		DWORD m_fHasOnObjDockedEvent:1 = false;			//	TRUE if this object has an OnObjDocked event
		DWORD m_fOnCreateCalled:1 = false;				//	TURE if OnCreate event has already been called
		DWORD m_fNoFriendlyTarget:1 = false;			//	TRUE if object cannot be hit by friends
		DWORD m_fItemEventsValid:1 = false;				//	TRUE if item event dispatcher is up to date
		DWORD m_fHasOnDamageEvent:1 = false;			//	TRUE if object has OnDamage event
		DWORD m_fHasOnAttackedEvent:1 = false;			//	TRUE if object has OnAttacked event

		DWORD m_fInDamage:1 = false;					//	TRUE if object is inside Damage call
		DWORD m_fDestroyed:1 = false;					//	TRUE if object is destroyed (but not yet deleted)
		DWORD m_fPlayerDestination:1 = false;			//	TRUE if object is a destination for the player
		DWORD m_fShowDistanceAndBearing:1 = false;		//	TRUE if we should show distance and bearing when a player destination
		DWORD m_fHasInterSystemEvent:1 = false;			//	TRUE if object has OnPlayerEnteredSystem or OnPlayerLeftSystem event
		DWORD m_fAutoClearDestination:1 = false;		//	TRUE if m_fPlayerDestination is cleared when object in SRS
		DWORD m_fHasOnOrdersCompletedEvent:1 = false;	//	TRUE if object has OnOrdersCompleted event
		DWORD m_fPlayerDocked:1 = false;				//	TRUE if player is docked with this object

		DWORD m_fPaintNeeded:1 = false;					//	TRUE if object needs to be painted
		DWORD m_fNonLinearMove:1 = false;				//	TRUE if object updates its position inside OnMove
		DWORD m_fHasName:1 = false;						//	TRUE if object has been named (this is an optimization--it may have false positives)
		DWORD m_fMarked:1 = false;						//	Temporary marker for processing lists (not persistent)
		DWORD m_fAscended:1 = false;					//	TRUE if object is ascended (i.e., stored outside a system)
		DWORD m_fOutOfPlaneObj:1 = false;				//	TRUE if object is out of plane
		DWORD m_fPainted:1 = false;						//	TRUE if we painted the object last tick
		DWORD m_fAutoClearDestinationOnDock:1 = false;	//	TRUE if we should clear the destination when player docks

		DWORD m_fShowHighlight:1 = false;				//	TRUE if we should paint a target highlight in SRS
		DWORD m_fAutoClearDestinationOnDestroy:1 = false;//	TRUE if we should clear the destination when station is destroyed
		DWORD m_fShowDamageBar:1 = false;				//	TRUE if we should show damage bar
		DWORD m_fHasGravity:1 = false;					//	TRUE if object has gravity
		DWORD m_fInsideBarrier:1 = false;				//	TRUE if we got created inside a barrier
		DWORD m_fHasOnSubordinateAttackedEvent:1 = false;	//	TRUE if we have a <OnSubordinateAttacked> event
		DWORD m_fHasGetDockScreenEvent:1 = false;		//	TRUE if we have a <GetDockScreen> event
		DWORD m_fHasOnAttackedByPlayerEvent:1 = false;	//	TRUE if we have an <OnAttackedByPlayer> event

		DWORD m_fHasOnOrderChangedEvent:1 = false;		//	TRUE if we have an <OnOrderChanged> event
		DWORD m_fManualAnchor:1 = false;				//	TRUE if object is temporarily anchored
		DWORD m_fCollisionTestNeeded:1 = false;			//	TRUE if object needs to check collisions with barriers
		DWORD m_fHasDockScreenMaybe:1 = false;			//	TRUE if object has a dock screen for player (may be stale)
		DWORD m_fAutoClearDestinationOnGate:1 = false;	//	TRUE if we should clear the destination when player gates
		DWORD m_f3DExtra:1 = false;						//	TRUE if object is an optional 3D extra
		DWORD m_fAutoCreatedPorts:1 = false;			//	TRUE if we have auto created some docking ports
		DWORD m_fDebugMode:1 = false;					//	TRUE if we want to show debug info for this object

		DWORD m_fQuestTarget:1 = false;					//	TRUE if this is a quest target.
		DWORD m_dwSpare:15 = 0;

#ifdef DEBUG_VECTOR
		CVector m_vDebugVector;			//	Draw a vector
#endif

		//	Property table

		static TPropertyHandler<CSpaceObject> m_BasePropertyTable;

		//	Empty list of overlays

		static COverlayList m_NullOverlays;
	};

inline bool operator== (const CSpaceObject &Obj1, const CSpaceObject &Obj2) { return &Obj1 == &Obj2; }
inline bool operator== (const CSpaceObject &Obj1, const CSpaceObject *pObj2) { return &Obj1 == pObj2; }
inline bool operator== (const CSpaceObject *pObj1, const CSpaceObject &Obj2) { return pObj1 == &Obj2; }

inline bool operator!= (const CSpaceObject &Obj1, const CSpaceObject &Obj2) { return !(Obj1 == Obj2); }
inline bool operator!= (const CSpaceObject &Obj1, const CSpaceObject *pObj2) { return !(Obj1 == pObj2); }
inline bool operator!= (const CSpaceObject *pObj1, const CSpaceObject &Obj2) { return !(pObj1 == Obj2); }

class CGlobalSpaceObject
	{
	public:
		CGlobalSpaceObject (void) : m_dwID(OBJID_NULL), m_pObj(NULL)
			{ }

		CGlobalSpaceObject &operator= (CSpaceObject *pObj)
			{
			m_dwID = (pObj ? pObj->GetID() : OBJID_NULL);
			m_pObj = pObj;
			return *this;
			}

		CSpaceObject *operator-> (void) { return m_pObj; }
		operator CSpaceObject * () const { return m_pObj; }

		void CleanUp (void) { m_dwID = OBJID_NULL; m_pObj = NULL; }
		DWORD GetID (void) const { return m_dwID; }
		CSpaceObject *GetObj (void) { if (IsEmpty() && m_dwID != OBJID_NULL) Resolve(); return m_pObj; }
		bool IsEmpty (void) const { return (m_pObj == NULL); }
		void ReadFromStream (SLoadCtx &Ctx);
		void Resolve (void);
		void SetID (DWORD dwID) { m_dwID = dwID; m_pObj = NULL; Resolve(); }
		void WriteToStream (IWriteStream *pStream);

	private:
		DWORD m_dwID;						//	Global ID of space object
		CSpaceObject *m_pObj;				//	Object pointer (may be NULL)
	};

class CObjectTrackerCriteria
	{
	public:
		const CDesignTypeCriteria &GetTypeCriteria (void) const { return m_TypeCriteria; }
		bool NeedsRefresh (void) const { return (m_bActiveOnly || m_bKilledOnly || m_bInPlayerSquadronOnly); }
		bool ParseCriteria (const CString &sCriteria);
		bool SelectsActiveOnly (void) const { return m_bActiveOnly; }
		bool SelectsKilledOnly (void) const { return m_bKilledOnly; }
		bool SelectsKnownOnly (void) const { return m_bKnownOnly; }
		bool SelectsInPlayerSquadronOnly () const { return m_bInPlayerSquadronOnly; }
		bool SelectsUnknownOnly (void) const { return m_bUnknownOnly; }
		
	private:
		CDesignTypeCriteria m_TypeCriteria;
		bool m_bActiveOnly = false;
		bool m_bKilledOnly = false;
		bool m_bKnownOnly = false;
		bool m_bUnknownOnly = false;
		bool m_bInPlayerSquadronOnly = false;
	};

class CObjectTracker
	{
	public:
		struct SObjEntry
			{
			CTopologyNode *pNode = NULL;
			CDesignType *pType = NULL;
			DWORD dwObjID = 0;
			CString sName;
			DWORD dwNameFlags = 0;
			CCompositeImageSelector ImageSel;
			CString sNotes;

			DWORD fKnown:1 = false;
			DWORD fShowDestroyed:1 = false;
			DWORD fShowInMap:1 = false;
			DWORD fFriendly:1 = false;          //  If neither friend or enemy, then neutral
			DWORD fEnemy:1 = false;
			DWORD fInactive:1 = false;			//	pObj->IsInactive()
			DWORD fPlayerBlacklisted:1 = false;	//	TRUE if player is blacklisted
			DWORD fIsStargate:1 = false;		//	TRUE if this is a stargate
			DWORD fInPlayerSquadron:1 = false;	//	TRUE if part of player's squadron
			};

		struct SBackgroundObjEntry
			{
			CStationType *pType = NULL;
			CVector vPos;
			CCompositeImageSelector *pImageSel = NULL;
			};

		~CObjectTracker (void);

		void Delete (CSpaceObject *pObj);
		void DeleteAll (void);
		bool Find (const CString &sNodeID, const CObjectTrackerCriteria &Criteria, TArray<SObjEntry> *retResult);
		void GetGalacticMapObjects (const CTopologyNode &Node, TArray<SObjEntry> &Results) const;
		void GetSystemBackgroundObjects (const CTopologyNode *pNode, TSortMap<Metric, SBackgroundObjEntry> &Results) const;
		void GetSystemStarObjects (const CTopologyNode *pNode, TArray<SBackgroundObjEntry> &Results) const;
		const TArray<COrbit> &GetSystemOrbits (const CTopologyNode *pNode) const;
		void GetTradingObjects (const CTopologyNode *pNode, TArray<SObjEntry> &Results) const;
		void Insert (CSpaceObject *pObj);
		bool IsTracked (CSpaceObject *pObj) { CSpaceObject::Categories iCategory = pObj->GetCategory(); return (iCategory == CSpaceObject::catStation || iCategory == CSpaceObject::catShip); }
		void InsertIfTracked (CSpaceObject *pObj) { if (IsTracked(pObj)) Insert(pObj); }
		void ReadFromStream (SUniverseLoadCtx &Ctx);
		void Refresh (CSystem *pSystem);
		void ReplayCommands (CSystem *pSystem);
		void SetKnown (const CString &sNodeID, const CDesignTypeCriteria &Criteria, bool bKnown = true);
		void WriteToStream (IWriteStream *pStream);

	private:
		enum EAccumulateFlags
			{
			FLAG_ACCUMULATE_NOTES =		0x00000001,	//	Initializes sNotes in SObjEntry (which can be expensive)
			};
		
		enum EDelayedCommandTypes
			{
			cmdUnknown =			-1,

			cmdSetKnown =			0,
			cmdSetUnknown =			1,
			};

		struct SDelayedCommand
			{
			EDelayedCommandTypes iCmd;
			CDesignTypeCriteria Criteria;
			};

		struct SObjExtra
			{
			CString sName;
			DWORD dwNameFlags = 0;
			CCompositeImageSelector ImageSel;
			CString sNotes;
			};

		struct SObjBasics
			{
			void DeleteExtra (void)
				{
				pExtra.Delete();
				}

			SObjExtra &SetExtra (void)
				{
				if (!pExtra)
					pExtra.Set(new SObjExtra);

				return *pExtra;
				}

			CVector vPos;						//  Position of object in its system

			DWORD fKnown:1 = false;             //  TRUE if player knows about this obj
			DWORD fShowDestroyed:1 = false;     //  TRUE if we need to paint station as destroyed
			DWORD fShowInMap:1 = false;         //  TRUE if we can dock with the obj
			DWORD fFriendly:1 = false;          //  If neither friend or enemy, then neutral
			DWORD fEnemy:1 = false;
			DWORD fInactive:1 = false;			//	TRUE if pObj->IsInactive()
			DWORD fPlayerBlacklisted:1 = false;	//	TRUE if player is blacklisted
			DWORD fIsStargate:1 = false;		//	TRUE if this is a stargate
			DWORD fInPlayerSquadron:1 = false;	//	TRUE if part of player's squadron

			DWORD dwSpare:23 = 0;

			TUniquePtr<SObjExtra> pExtra;
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
			TArray<SDelayedCommand> Commands;
			};

		bool AccumulateEntries (TArray<SObjList *> &Table, const CObjectTrackerCriteria &Criteria, DWORD dwFlags, TArray<SObjEntry> *retResult) const;
		void AccumulateEntry (const SObjList &ObjList, DWORD dwObjID, const SObjBasics &ObjData, DWORD dwFlags, TArray<SObjEntry> &Results) const;
		bool Find (CTopologyNode *pNode, CSpaceObject *pObj, SObjBasics **retpObjData = NULL) const;
		bool Find (SNodeData *pNodeData, CSpaceObject *pObj, SObjBasics **retpObjData = NULL) const;
		bool GetCustomDesc (const CSpaceObject &Obj, const SObjBasics &ObjData, CString *retsDesc) const;
		SObjList *GetList (CSpaceObject *pObj) const;
		SObjList *GetList (CTopologyNode *pNode, CDesignType *pType) const;
		void Refresh (const CSpaceObject &Obj, SObjBasics &ObjData, const CSpaceObject *pPlayer);
		void ReplayCommand (const SDelayedCommand &Cmd, CSystem *pSystem);
		void SetKnown (TArray<SObjList *> &Table, const CDesignTypeCriteria &Criteria, bool bKnown);
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

		void DeleteAll (void) { CleanUp(); }
		void Insert (CSpaceObject *pObj) { m_List.Insert(pObj); }
		CSpaceObject *FindByID (DWORD dwID) const;
		int GetCount (void) const { return m_List.GetCount(); }
		CSpaceObject *GetObj (int iIndex) const { return m_List[iIndex]; }
		bool IsEmpty (void) const { return (m_List.GetCount() == 0); }
		void ReadFromStream (SLoadCtx &Ctx);
		CSpaceObject *RemoveByID (DWORD dwID);
		void WriteToStream (IWriteStream *pStream);

	private:
		void CleanUp (void);

		TArray<CSpaceObject *> m_List;
	};

//	Implementations ------------------------------------------------------------

#include "TSEListImpl.h"
#include "TSEDeviceClassesImpl.h"
#include "TSESpaceObjectsImpl.h"
#include "TSEStationImpl.h"
#include "TSEMissions.h"

#include "TSEPlayer.h"
#include "TSEArtifactAwakening.h"
#include "TSEEventsImpl.h"
#include "TSEMapPainters.h"
#include "TSETransLispUtil.h"

#include "TSEAccessibilitySettings.h"
#include "TSEUniverse.h"

#include "TSESpaceObjectsEnum.h"
#include "TSEItemInlines.h"
#include "TSEUniverseInlines.h"

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


EStorageScopes ParseStorageScopeID (const CString &sID);

//	Miscellaneous Helpers

void AppendReferenceString (CString *iosReference, const CString &sString);
int CalcEffectiveHP (int iLevel, int iHP, int *iHPbyDamageType);
Metric CalcLevelDiffStrength (int iDiff);
ICCItem *CreateItemFromDamageEffects (CCodeChain &CC, SDamageCtx &Ctx);
int GetDamageTypeLevel (DamageTypes iDamage);
Metric *GetDestinyToBellCurveArray (void);
void LoadDamageEffectsFromItem (ICCItem *pItem, SDamageCtx &Ctx);
EManeuver CalcTurnManeuver (int iDesired, int iCurrent, int iRotationAngle);
CString ParseCriteriaParam (const char **ioPos, bool bExpectColon = true, bool *retbBinaryParam = NULL);
bool ParseCriteriaParamLevelRange (const char **ioPos, int *retiLow = NULL, int *retiHigh = NULL);
Metric ParseDistance (const CString &sValue, Metric rDefaultScale);

//	CodeChain helper functions (CCUtil.cpp)

#define CCUTIL_FLAG_CHECK_DESTROYED				0x00000001

Metric CalcRandomMetric (CCodeChain &CC, ICCItem *pItem);
bool CreateBinaryFromList (const CString &sClass, const ICCItem &List, void *pvDest);
ICCItem *CreateDamageSource (CCodeChain &CC, const CDamageSource &Source);
CString CreateDataFieldFromItemList (const TArray<CItem> &List);
CString CreateDataFromItem (ICCItem *pItem);
ICCItem *CreateDisposition (CCodeChain &CC, CSovereign::Disposition iDisp);
ICCItem *CreateListFromBinary (const CString &sClass, void const *pvSource, int iLengthBytes);
ICCItem *CreateListFromImage (CCodeChain &CC, const CObjectImageArray &Image, int iRotation = 0);
ICCItem *CreateListFromItem (const CItem &Item);
ICCItem *CreateListFromOrbit (CCodeChain &CC, const COrbit &OrbitDesc);
ICCItem *CreateListFromVector (const CVector &vVector);
ICCItem *CreatePowerResult (double rPowerInKW);
CSpaceObject *CreateObjFromItem (const ICCItem *pItem, DWORD dwFlags = 0);
ICCItem *CreateObjPointer (CCodeChain &CC, CSpaceObject *pObj);
bool CreateOrbitFromList (CCodeChain &CC, ICCItem *pList, COrbit *retOrbitDesc);
ICCItem *CreateResultFromDataField (CCodeChain &CC, const CString &sValue);
CShip *CreateShipObjFromItem (CCodeChain &CC, ICCItem *pArg);
CStation *CreateStationObjFromItem (CCodeChain &CC, ICCItem *pArg);
CVector CreateVectorFromList (CCodeChain &CC, const ICCItem *pList);
CCXMLWrapper *CreateXMLElementFromItem (CCodeChain &CC, ICCItem *pItem);
void DefineGlobalItem (CCodeChain &CC, const CString &sVar, const CItem &Item);
void DefineGlobalSpaceObject (CCodeChain &CC, const CString &sVar, const CSpaceObject *pObj);
void DefineGlobalVector (CCodeChain &CC, const CString &sVar, const CVector &vVector);
void DefineGlobalWeaponType (CCodeChain &CC, const CString &sVar, CItemType *pWeaponType);
CDamageSource GetDamageSourceArg (CCodeChain &CC, ICCItem *pArg);
DamageTypes GetDamageTypeFromArg (CCodeChain &CC, ICCItem *pArg);
const CEconomyType *GetEconomyTypeFromItem (CCodeChain &CC, ICCItem *pItem);
const CEconomyType *GetEconomyTypeFromString (const CString &sCurrency);
ALERROR GetEconomyUNIDOrDefault (CCodeChain &CC, ICCItem *pItem, DWORD *retdwUNID);
void GetImageDescFromList (ICCItem *pList, CG32bitImage **retpBitmap, RECT *retrcRect);
ICCItem *GetImageDescProperty (CCodeChain &CC, ICCItem *pImageDesc, const CString &sProperty);
bool GetLinkedFireOptions (const ICCItem *pArg, DWORD *retdwOptions, CString *retsError);
ALERROR GetPosOrObject (CEvalContext *pEvalCtx, ICCItem *pArg, CVector *retvPos, CSpaceObject **retpObj = NULL, int *retiLocID = NULL, CStationType *pStationToPlace = NULL);
bool IsVectorItem (ICCItem *pItem);
ALERROR LoadCodeBlock (const CString &sCode, ICCItem **retpCode, CString *retsError = NULL);
ICCItem *StdErrorNoSystem (CCodeChain &CC);

//	Inlines

inline CSpaceObject *CInstalledDevice::GetTarget (CSpaceObject *pSource) const { return ((m_dwTargetID && pSource) ? pSource->GetSystem()->FindObject(m_dwTargetID) : NULL); }
inline void CInstalledDevice::SetTarget (CSpaceObject *pObj) { m_dwTargetID = (pObj ? pObj->GetID() : 0); }

inline bool CItem::IsDisrupted (void) const { return (m_pExtra ? (m_pExtra->m_dwDisruptedTime >= (DWORD)GetUniverse().GetTicks()) : false); }

inline int CalcHPDamageAdj (int iHP, int iDamageAdj)
	{ return (iDamageAdj == 0 ? -1 : mathRound(iHP * 100.0 / iDamageAdj)); }
