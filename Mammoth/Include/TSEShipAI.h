//	TSEShipAI.h
//
//	Defines classes and interfaces for ship AI.
//	Copyright (c) 2015 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

struct SCrewMetrics
	{
	static constexpr int MIN_LEVEL =			0;		//	Min
	static constexpr int LOW_LEVEL =			50;		//	Low
	static constexpr int NEUTRAL_LOW_LEVEL =	85;
	static constexpr int NEUTRAL_LEVEL =		100;	//	Normal
	static constexpr int NEUTRAL_HIGH_LEVEL =	115;
	static constexpr int HIGH_LEVEL =			150;	//	High
	static constexpr int PEAK_LEVEL =			180;	//	Peak
	static constexpr int MAX_LEVEL =			200;	//	Max

	SCrewMetrics (void) :
			iCrewCount(0),
			iBelief(0),
			iCohesion(0),
			iLoyalty(0)
		{ }

	int iCrewCount;							//	Total number of crew members
	int iBelief;							//	Belief for all crew members
	int iCohesion;							//	Cohesion for all crew members
	int iLoyalty;							//	Loyalty for all crew members
	};

struct SPlayerChangedShipsCtx
	{
	bool bOldShipWaits = false;				//	If TRUE, old ship waits and orders are set appropriately
	bool bNoOrderTransfer = false;			//	If TRUE, do not change orders to point to new ship
	bool bTransferCargo = false;			//	If TRUE, move cargo from old ship to new ship
	bool bTransferEquipment = false;		//	If TRUE, transfer equipment, such as targeting computer
	bool bTakeDockingPort = false;			//	If TRUE, new ship replaces old ship in docking port
	bool bIdentifyInstalled = false;		//	If TRUE, identify installed items in the new ship
	};

//  CAISettings ----------------------------------------------------------------

//	NOTE: These values are saved; do not change them.

enum class AICombatStyle
	{
	Standard =								0,	//	Normal dogfighting
	StandOff =								1,	//	Missile ship combat
	Flyby =									2,	//	Maximize relative speed wrt target
	NoRetreat =								3,	//	Do not turn away from target
	Chase =									4,	//	Get in position behind the target
	Advanced =								5,	//	Dogfight, keeping proper distance from target
	};

enum class AIFlockingStyle
	{
	None =									0,	//	No flocking behavior

	Cloud =									1,	//	Old-style cloud flocking
	Compact =								2,	//	Crowd around leader
	Random =								3,	//	Random, fixed positions around leader
	};

enum class AIReaction
	{
	Default =								0,	//	Default reaction based on order
	None =									1,	//	Do not react
	DeterWithSecondaries =					2,	//	Deter with secondaries, but do not turn or maneuver
	Deter =									3,	//	Turn to attack attacker, but do not chase
	Chase =									4,	//	Chase attacker
	Destroy =								5,	//	Destroy attacker
	Gate =									6,	//	Gate out
	};

class CAISettings
	{
	public:
		static constexpr int DEFAULT_THREAT_RANGE = 30;

		CAISettings (void) { }

		bool AscendOnGate (void) const { return m_fAscendOnGate; }
		AICombatStyle GetCombatStyle (void) const { return m_iCombatStyle; }
		int GetFireAccuracy (void) const { return m_iFireAccuracy; }
		int GetFireRangeAdj (void) const { return m_iFireRangeAdj; }
		int GetFireRateAdj (void) const { return m_iFireRateAdj; }
		AIFlockingStyle GetFlockingStyle (void) const { return m_iFlockingStyle; }
		Metric GetMinCombatSeparation (void) const { return m_rMinCombatSeparation; }
		int GetPerception (void) const { return m_iPerception; }
		AIReaction GetReactToAttack () const { return m_iReactToAttack; }
		AIReaction GetReactToThreat () const { return m_iReactToThreat; }
		Metric GetThreatRange () const { return m_rThreatRange; }
		CString GetValue (const CString &sSetting);
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		void InitToDefault (void);
		bool IsAggressor (void) const { return m_fAggressor; }
		bool IsFlocker (void) const { return (m_iFlockingStyle != AIFlockingStyle::None); }
		bool IsNonCombatant (void) const { return m_fNonCombatant; }
		bool IsPlayer (void) const { return m_fIsPlayer; }
		bool NoAttackOnThreat (void) const { return m_fNoAttackOnThreat; }
		bool NoDogfights (void) const { return m_fNoDogfights; }
		bool NoFriendlyFire (void) const { return m_fNoFriendlyFire; }
		bool NoFriendlyFireCheck (void) const { return m_fNoFriendlyFireCheck; }
		bool NoNavPaths (void) const { return m_fNoNavPaths; }
		bool NoOrderGiver (void) const { return m_fNoOrderGiver; }
		bool NoShieldRetreat (void) const { return m_fNoShieldRetreat; }
		bool NoTargetsOfOpportunity (void) const { return m_fNoTargetsOfOpportunity; }
		void ReadFromStream (SLoadCtx &Ctx);
		void SetMinCombatSeparation (Metric rValue) { m_rMinCombatSeparation = rValue; }
		CString SetValue (const CString &sSetting, const CString &sValue);
		bool UseAllPrimaryWeapons (void) const { return m_fUseAllPrimaryWeapons; }
		void WriteToStream (IWriteStream *pStream);

		static AICombatStyle ConvertToAICombatStyle (const CString &sValue);
		static AIFlockingStyle ConvertToFlockingStyle (const CString &sValue);
		static AIReaction ConvertToAIReaction (const CString &sValue);
		static CString ConvertToID (AICombatStyle iStyle);
		static CString ConvertToID (AIFlockingStyle iStyle);
		static CString ConvertToID (AIReaction iStyle);

	private:
		AICombatStyle m_iCombatStyle = AICombatStyle::Standard;			//	Combat style
		AIFlockingStyle m_iFlockingStyle = AIFlockingStyle::None;		//	Flocking style
		AIReaction m_iReactToAttack = AIReaction::Default;
		AIReaction m_iReactToThreat = AIReaction::Default;

		int m_iFireRateAdj = 10;					//	Adjustment to weapon's fire rate (10 = normal; 20 = double delay)
		int m_iFireRangeAdj = 100;					//	Adjustment to range (100 = normal; 50 = half range)
		int m_iFireAccuracy = 100;					//	Percent chance of hitting
		int m_iPerception = 4;						//	Perception (LATER: We need to refer to CSpaceObject::perceptNormal)

		Metric m_rThreatRange = DEFAULT_THREAT_RANGE * LIGHT_SECOND;	//	React to threats in this range
		Metric m_rMinCombatSeparation = -1.0;		//	Min separation from other ships while in combat (-1.0 == based on image size)

		DWORD m_fNoShieldRetreat:1 = false;			//	Ship does not retreat when shields go down
		DWORD m_fNoDogfights:1 = false;				//	Don't turn ship to face target
		DWORD m_fNonCombatant:1 = false;			//	Tries to stay out of trouble
		DWORD m_fNoFriendlyFire:1 = false;			//	Cannot hit friends
		DWORD m_fAggressor:1 = false;				//	Attack targets of opportunity even if they haven't attacked
		DWORD m_fNoFriendlyFireCheck:1 = false;		//	Do not check to see if friends are in line of fire
		DWORD m_fNoOrderGiver:1 = false;			//	Always treated as the decider
		DWORD m_fAscendOnGate:1 = false;			//	If TRUE, we ascend when the ship gates out

		DWORD m_fNoNavPaths:1 = false;				//	If TRUE, do not use nav paths
		DWORD m_fNoAttackOnThreat:1 = false;		//	Do not attack enemies while escorting (unless ordered)
		DWORD m_fNoTargetsOfOpportunity:1 = false;	//	If TRUE, do not attack targets of opportunity
		DWORD m_fIsPlayer:1 = false;				//	If TRUE, we're controlling the player ship (this is usually
													//		for debugging only).
		DWORD m_fUseAllPrimaryWeapons:1;		//  If TRUE, we try to shoot all primary weapons at the same time
		DWORD m_fSpare6:1 = false;
		DWORD m_fSpare7:1 = false;
		DWORD m_fSpare8:1 = false;

		DWORD m_dwSpare:16 = 0;
	};

//	IShipController ------------------------------------------------------------
//
//	This abstract class is the root of all ship AI classes.

static constexpr DWORD ORDER_FLAG_DELETE_ON_STATION_DESTROYED =		0x00000001;	//	Delete the order when target is station destroyed
static constexpr DWORD ORDER_FLAG_UPDATE_ON_NEW_PLAYER_SHIP =		0x00000002;	//	Update target if player changes ship
static constexpr DWORD ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED =		0x00000004;	//	Notify controller when any station destroyed
static constexpr DWORD ORDER_FLAG_DELETE_ON_OLD_SHIP_WAITS =		0x00000008;	//	Delete if player switched ships and old ship is waiting

class COrderDesc;

class IShipController
	{
	public:

		//	TO ADD A NEW ORDER:
		//
		//	1.	Add an order to the end of the list (do not add in the middle,
		//		because numbers are stored in save file).
		//
		//	2.	Add order data to m_OrderTypes in IShipController.cpp
		//
		//	3.	Add handler in IOrderModule::Create

		enum OrderTypes
			{
			orderNone,					//	Nothing to do

			orderGuard,					//	pTarget = target to guard
			orderDock,					//	pTarget = station to dock with
			orderDestroyTarget,			//	pTarget = target to destroy; dwData = timer (0 == indefinitely)
			orderWait,					//	dwData = seconds to wait (0 == indefinitely)
			orderGate,					//	Gate out of the system (pTarget = optional gate)

			orderGateOnThreat,			//	Gate out of system if threatened
			orderGateOnStationDestroyed,//	Gate out if station is destroyed
			orderPatrol,				//	pTarget = target to orbit; dwData = radius (light-seconds)
			orderEscort,				//	pTarget = target to escort; dwData1 = angle; dwData2 = radius
			orderScavenge,				//	Scavenge for scraps

			orderFollowPlayerThroughGate,//	Follow the player through the nearest gate
			orderAttackNearestEnemy,	//	Attack the nearest enemy; dwData = timer (0 == indefinitely)
			orderTradeRoute,			//	Moves from station to station
			orderWander,				//	Wander around system, avoiding enemies
			orderLoot,					//	pTarget = target to loot

			orderHold,					//	Stay in place; dwData = seconds (0 == indefinitely)
			orderMine,					//	Mine asteroids; pTarget = base
			orderWaitForPlayer,			//	Wait for player to return to the system
			orderDestroyPlayerOnReturn,	//	Wait to attack player when she returns
			orderFollow,				//	pTarget = target to follow (like escort, but no defense)

			orderNavPath,				//	dwData = nav path ID to follow
			orderGoTo,					//	Get within dwData ls of pTarget (0 = 1 light-second)
			orderWaitForTarget,			//	Hold until pTarget is in range; dwData1 = radius (0 = LRS range); dwData2 = timer
			orderWaitForEnemy,			//	Hold until any enemy is in LRS range (or dwData timer expires)
			orderBombard,				//	Hold and attack target from here; pTarget = target; dwData = time

			orderApproach,				//	Get within dwData ls of pTarget
			orderAimAtTarget,			//	Same as orderDestroyTarget, but we never fire
			orderOrbit,					//	pTarget = target to orbit; dwData1 = radius (light-seconds); dwData2 = timer
			orderHoldCourse,			//	dwData1 = course; dwData2 = distance (light-seconds)
			orderTurnTo,				//	dwData = angle to face

			orderDestroyTargetHold,		//	pTarget = target to destroy; dwData = timer
			orderAttackStation,			//	pTarget = station to destroy
			orderFireEvent,				//	pTarget = object that gets event; Data = event name
			orderWaitForUndock,			//	Wait for pTarget to undock
			orderSendMessage,			//	pTarget = object that gets message; Data = message to send

			orderAttackArea,			//	pTarget = center; dwData1 = radius (light-seconds); dwData2 = timer.
			orderHoldAndAttack,			//	pTarget = target to destroy; dwData = timer.
			orderGoToPos,				//	dwData = vector destination
			orderWaitForThreat,			//	dwData = seconds to wait (0 == indefinitely)
			orderSentry,				//	Sentry mode (for turrets) pTarget = obj to guard (optional); dwData = timer (0 == indefinitely)

			orderFireWeapon,			//	Data = weapon item to fire
			orderUseItem,				//	Data = item to use
			orderOrbitExact,			//	pTarget = center
										//		"radius": Orbit radius (light-seconds)
										//		"speed": Angular speed (degrees)
										//		"eccentricity": Orbital eccentricity
										//		"angle": Starting position in order (degrees) 0 == auto angle.
										//		"timer": Seconds left in order
			orderOrbitPatrol,			//	pTarget = center
										//		"radius": Orbit radius (light-seconds)
										//		"speed": Angular speed (degrees)
										//		"eccentricity": Orbital eccentricity
										//		"angle": Starting position in order (degrees) 0 == auto angle.
										//		"timer": Seconds left in order
			orderDeterChase,			//	pTarget = target to chase
										//		"base": ID of base object (optional)
										//		"radius": Max distance from base (optional)
										//		"timer": Seconds left in order (optional)
			orderAttackOrRetreat,		//	pTarget = target to destroy
										//		"timer": Seconds left in order (optional)
			};

		enum EShipStatusNotifications
			{
			statusArmorRepaired,			//	dwData = segment
			statusFuelLowWarning,			//	dwData = sequence
			statusLifeSupportWarning,		//	dwData = seconds left
			statusRadiationCleared,			//	Decontaminated
			statusRadiationWarning,			//	dwData = ticks left
			statusReactorOverloadWarning,	//	dwData = sequence
			statusReactorPowerFailure,		//	Reactor is dead
			statusReactorRestored,			//	Reactor is functioning normally
			statusTimeStopped,				//	Time stopped
			statusTimeRestored,				//	Time continues
			};

		virtual ~IShipController (void) { }

		virtual void AccumulateCrewMetrics (SCrewMetrics &Metrics) { }
		virtual void Behavior (SUpdateCtx &Ctx) { }
		virtual void CancelDocking (void) { }
		virtual bool CanObjRequestDock (CSpaceObject *pObj = NULL) const { return true; }
		virtual CString DebugCrashInfo (void) { return NULL_STR; }
		virtual void DebugPaintInfo (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) { }
		virtual ICCItem *FindProperty (const CString &sProperty) { return NULL; }
		virtual bool FollowsObjThroughGate (CSpaceObject *pLeader = NULL) { return false; }
		virtual int GetAISettingInteger (const CString &sSetting) { return 0; }
		virtual CString GetAISettingString (const CString &sSetting) { return NULL_STR; }
		virtual const CAISettings *GetAISettings (void) { return NULL; }
		virtual CSpaceObject *GetBase (void) const { return NULL; }
		virtual CString GetClass (void) { return NULL_STR; }
		virtual int GetCombatPower (void) = 0;
		virtual const CCurrencyBlock *GetCurrencyBlock (void) const { return NULL; }
		virtual CCurrencyBlock *GetCurrencyBlock (void) { return NULL; }
		virtual CSpaceObject *GetDestination (void) const { return NULL; }
		virtual bool GetDeviceActivate (void) = 0;
		virtual CSpaceObject *GetEscortPrincipal (void) const { return NULL; }
		virtual int GetFireDelay (void) { return 0; }
		virtual int GetFireRateAdj (void) { return 10; }
		virtual EManeuverTypes GetManeuver (void) = 0;
		virtual CSpaceObject *GetOrderGiver (void) = 0;
		virtual bool GetReverseThrust (void) = 0;
		virtual CSpaceObject *GetShip (void) { return NULL; }
		virtual bool GetStopThrust (void) = 0;

		static constexpr DWORD FLAG_ACTUAL_TARGET =		0x00000001;
		static constexpr DWORD FLAG_NO_AUTO_TARGET =	0x00000002;
		virtual CSpaceObject *GetTarget (const CDeviceItem *pDeviceItem = NULL, DWORD dwFlags = 0) const { return NULL; }

		virtual CTargetList GetTargetList (void) const { return CTargetList(); }
		virtual bool GetThrust (void) = 0;
		virtual bool IsAngryAt (const CSpaceObject *pObj) const { return false; }
		virtual bool IsPlayer (void) const { return false; }
		virtual bool IsPlayerBlacklisted (void) const { return false; }
		virtual bool IsPlayerEscort (void) const { return false; }
		virtual bool IsPlayerWingman (void) const { return false; }
		virtual void ReadFromStream (SLoadCtx &Ctx, CShip *pShip) { ASSERT(false); }
		virtual int SetAISettingInteger (const CString &sSetting, int iValue) { return 0; }
		virtual CString SetAISettingString (const CString &sSetting, const CString &sValue) { return NULL_STR; }
		virtual void SetCommandCode (ICCItem *pCode) { }
		virtual void SetManeuver (EManeuverTypes iManeuver) { }
		virtual void SetShipToControl (CShip *pShip) { }
		virtual void SetThrust (bool bThrust) { }
		virtual void SetPlayerBlacklisted (bool bValue) { }
		virtual void SetPlayerEscort (bool bValue) { }
		virtual void SetPlayerWingman (bool bIsWingman) { }
		virtual ESetPropertyResult SetProperty (const CString &sProperty, const ICCItem &Value, CString *retsError = NULL) { return ESetPropertyResult::notFound; }
		virtual void WriteToStream (IWriteStream *pStream) { ASSERT(false); }

		virtual void AddOrder (const COrderDesc &Order, bool bAddBefore = false) { }
		virtual void CancelAllOrders (void) { }
		virtual void CancelCurrentOrder (void) { }
		virtual bool CancelOrder (int iIndex) { return false; }
		virtual const COrderDesc &GetCurrentOrderDesc () const;
		virtual const COrderDesc &GetOrderDesc (int iIndex) const;
		virtual int GetOrderCount (void) const { return 0; }

		//	Events

		virtual void OnAbilityChanged (Abilities iAbility, AbilityModifications iChange, bool bNoMessage = false) { }
		virtual void OnAcceptedMission (CMission &MissionObj) { }
		virtual void OnAttacked (CSpaceObject *pAttacker, const SDamageCtx &Damage) { }
		virtual DWORD OnCommunicate (CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2, ICCItem *pData) { return resNoAnswer; }
		virtual void OnComponentChanged (ObjectComponentTypes iComponent) { }
		virtual void OnDamaged (const CDamageSource &Cause, CInstalledArmor *pArmor, const DamageDesc &Damage, int iDamage) { }
		virtual void OnDeviceEnabledDisabled (int iDev, bool bEnabled, bool bSilent = false) { }
		virtual void OnDeviceStatus (CInstalledDevice *pDev, CDeviceClass::DeviceNotificationTypes iEvent) { }
		virtual bool OnDestroyCheck (DestructionTypes iCause, const CDamageSource &Attacker) { return true; }
		virtual void OnDestroyed (SDestroyCtx &Ctx) { }
		virtual void OnDocked (CSpaceObject *pObj) { }
		virtual void OnDockingStop (void) { }
		virtual void OnEnterGate (CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate, bool bAscend) { }
		virtual void OnFuelConsumed (Metric rFuel, CReactorDesc::EFuelUseTypes iUse) { }
		virtual void OnHitBarrier (CSpaceObject *pBarrierObj, const CVector &vPos) { CancelDocking(); }
		virtual void OnItemBought (const CItem &Item, CurrencyValue iTotalPrice) { }
		virtual void OnItemDamaged (const CItem &Item, int iHP) { }
		virtual void OnItemFired (const CItem &Item) { }
		virtual void OnItemInstalled (const CItem &Item) { }
		virtual void OnItemSold (const CItem &Item, CurrencyValue iTotalPrice) { }
		virtual void OnItemUninstalled (const CItem &Item) { }
		virtual void OnMissionCompleted (CMission *pMission, bool bSuccess) { }
		virtual void OnNewSystem (CSystem *pSystem) { }
		virtual void OnObjHit (const SDamageCtx &Ctx) { }
		virtual void OnObjDestroyed (const SDestroyCtx &Ctx) { }
		virtual void OnObjEnteredGate (CSpaceObject *pObj, CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate) { }
		virtual void OnOverlayConditionChanged (ECondition iCondition, EConditionChange iChange) { }
		virtual void OnPaintSRSEnhancements (CG32bitImage &Dest, SViewportPaintCtx &Ctx) { }
		virtual void OnPlayerChangedShips (CSpaceObject *pOldShip, SPlayerChangedShipsCtx &Options) { }
		virtual void OnPlayerObj (CSpaceObject *pPlayer) { }
		virtual void OnProgramDamage (CSpaceObject *pHacker, const ProgramDesc &Program) { }
		virtual void OnShipStatus (EShipStatusNotifications iEvent, DWORD dwData = 0) { }
		virtual void OnStatsChanged (void) { }
		virtual void OnStationDestroyed (const SDestroyCtx &Ctx) { }
		virtual void OnSystemLoaded (void) { }
		virtual void OnUpdatePlayer (SUpdateCtx &Ctx) { }
		virtual void OnWeaponStatusChanged (void) { }
		virtual void OnWreckCreated (CSpaceObject *pWreck) { }

		static DWORD GetOrderFlags (OrderTypes iOrder) { return m_OrderTypes[iOrder].dwFlags; }
		static CString GetOrderName (OrderTypes iOrder) { return CString(m_OrderTypes[iOrder].szName); }
		static OrderTypes GetOrderType (const CString &sString);
		static char GetOrderDataType (OrderTypes iOrder);
		static bool OrderHasTarget (OrderTypes iOrder, bool *retbRequired = NULL);

	private:
		struct SOrderTypeData
			{
			const char *szName;
			const char *szTarget;
			//	-		no target
			//	*		optional target
			//	o		required target

			const char *szData;
			//	-		no data
			//	?		any
			//	i		integer (may be optional)
			//	I		CItem
			//	2		two integers (encoded in a DWORD)
			//  s		string data
			//	v		vector data

			DWORD dwFlags;
			};

		static const SOrderTypeData m_OrderTypes[];
		static const int ORDER_TYPES_COUNT;
	};

class COrderDesc
	{
	public:
		COrderDesc () { }
		COrderDesc (const COrderDesc &Src) { Copy(Src); }
		COrderDesc (COrderDesc &&Src) noexcept { Move(Src); }

		COrderDesc (IShipController::OrderTypes iOrder);
		COrderDesc (IShipController::OrderTypes iOrder, CSpaceObject *pTarget);
		COrderDesc (IShipController::OrderTypes iOrder, CSpaceObject *pTarget, int iData);
		COrderDesc (IShipController::OrderTypes iOrder, CSpaceObject *pTarget, int iData1, int iData2);
		COrderDesc (IShipController::OrderTypes iOrder, CSpaceObject *pTarget, const CString &sData);
		COrderDesc (IShipController::OrderTypes iOrder, CSpaceObject *pTarget, const CVector &vData);
		COrderDesc (IShipController::OrderTypes iOrder, CSpaceObject *pTarget, const CItem &Data);
		COrderDesc (IShipController::OrderTypes iOrder, CSpaceObject *pTarget, const ICCItem &Data);

		~COrderDesc () { CleanUp(); }

		COrderDesc &operator= (const COrderDesc &Src) { CleanUp(); Copy(Src); return *this; }
		COrderDesc &operator= (COrderDesc &&Src) noexcept { CleanUp(); Move(Src); return *this; }
		explicit operator bool () const { return !IsEmpty(); }

		ICCItemPtr AsCCItemList () const;
		DiceRange GetDataDiceRange (const CString &sField, int iDefault = 0, CString *retsSuffix = NULL) const;
		Metric GetDataDouble (const CString &sField, Metric rDefault = 0.0) const;
		ICCItemPtr GetDataCCItem () const { if (GetDataType() == EDataType::CCItem) return ICCItemPtr(((ICCItem *)m_pData)->Reference()); else return ICCItemPtr::Nil(); }
		DWORD GetDataInteger () const;
		DWORD GetDataInteger (const CString &sField, bool bDefaultField = false, DWORD dwDefault = 0) const;
		DWORD GetDataInteger2 () const;
		DWORD GetDataIntegerOptional (const CString &sField, DWORD dwDefault = 0) const;
		const CItem &GetDataItem () const { if (GetDataType() == EDataType::Item) return *(CItem *)m_pData; else return CItem::NullItem(); }
		CSpaceObject *GetDataObject (CSpaceObject &SourceObj, const CString &sField) const;
		const CString &GetDataString () const { if (GetDataType() == EDataType::String) return *(CString *)m_pData; else return NULL_STR; }
		int GetDataTicksLeft () const;
		const CVector &GetDataVector () const { if (GetDataType() == EDataType::Vector) return *(CVector *)m_pData; else return NullVector; }
		IShipController::OrderTypes GetOrder () const { return (IShipController::OrderTypes)m_dwOrderType; }
		CSpaceObject *GetTarget () const { return m_pTarget; }
		bool IsCCItem () const { return (GetDataType() == EDataType::CCItem); }
		bool IsEmpty () const { return GetOrder() == IShipController::orderNone; }
		bool IsIntegerOrPair () const { return (GetDataType() == EDataType::Int32 || GetDataType() == EDataType::Int16Pair); }
		bool IsVector () const { return (GetDataType() == EDataType::Vector); }
		void ReadFromStream (SLoadCtx &Ctx);
		void SetDataInteger (DWORD dwData);
		void SetTarget (CSpaceObject *pTarget) { m_pTarget = pTarget; }
		void WriteToStream (IWriteStream &Stream, const CShip &Ship) const;

		static COrderDesc ParseFromCCItem (CCodeChainCtx &CCX, IShipController::OrderTypes iOrder, CSpaceObject *pTarget, const ICCItem &Args, int iFirstArg);
		static COrderDesc ParseFromString (const CString &sValue);

		static COrderDesc Null;

	private:
		enum class EDataType
			{
			None = 0,					//	pData is ignored
			Int32,						//	pData is 32-bit integer
			Int16Pair,					//	pData is two 16-bit integers
			String,						//	pData is a pointer to a CString
			Vector,						//	pData is a pointer to a CVector
			Item,						//	pData is a pointer to a CItem
			CCItem,						//	pData is a pointer to an ICCItem
			};

		static constexpr DWORD COMPATIBLE_DATA_TYPE_NONE = 0;
		static constexpr DWORD COMPATIBLE_DATA_TYPE_INTEGER = 1;
		static constexpr DWORD COMPATIBLE_DATA_TYPE_PAIR = 2;
		static constexpr DWORD COMPATIBLE_DATA_TYPE_STRING = 3;
		static constexpr DWORD COMPATIBLE_DATA_TYPE_VECTOR = 4;
		static constexpr DWORD COMPATIBLE_DATA_TYPE_ITEM = 5;
		static constexpr DWORD COMPATIBLE_DATA_TYPE_ORBIT_EXACT = 6;

		void CleanUp ();
		void Copy (const COrderDesc &Src);
		EDataType GetDataType () const { return (EDataType)m_dwDataType; }
		void Move (COrderDesc &Src);

		DWORD m_dwOrderType:8 = 0;		//	IShipController::OrderTypes
		DWORD m_dwDataType:8 = 0;		//	EDataType
		DWORD m_dwSpare:16 = 0;

		CSpaceObject *m_pTarget = NULL;	//	Order target
		void *m_pData = NULL;			//	Depends on dwDataType
	};

class CShipAIHelper
	{
	public:
		static bool CalcFormationParams (CShip *pShip, const CVector &vDestPos, const CVector &vDestVel, CVector *retvRecommendedVel, Metric *retrDeltaPos2 = NULL, Metric *retrDeltaVel2 = NULL);
	};
