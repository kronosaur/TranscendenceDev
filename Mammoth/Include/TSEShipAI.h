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

enum AICombatStyles
	{
	aicombatStandard =						0,	//	Normal dogfighting
	aicombatStandOff =						1,	//	Missile ship combat
	aicombatFlyby =							2,	//	Maximize relative speed wrt target
	aicombatNoRetreat =						3,	//	Do not turn away from target
	aicombatChase =							4,	//	Get in position behind the target
	aicombatAdvanced =						5,	//	Dogfight, keeping proper distance from target
	};

class CAISettings
	{
	public:
		enum EFlockingStyles
			{
			flockNone =						0,	//	No flocking behavior

			flockCloud =					1,	//	Old-style cloud flocking
			flockCompact =					2,	//	Crowd around leader
			flockRandom =					3,	//	Random, fixed positions around leader
			};

		CAISettings (void);

		bool AscendOnGate (void) const { return m_fAscendOnGate; }
		AICombatStyles GetCombatStyle (void) const { return m_iCombatStyle; }
		int GetFireAccuracy (void) const { return m_iFireAccuracy; }
		int GetFireRangeAdj (void) const { return m_iFireRangeAdj; }
		int GetFireRateAdj (void) const { return m_iFireRateAdj; }
		EFlockingStyles GetFlockingStyle (void) const { return m_iFlockingStyle; }
		Metric GetMinCombatSeparation (void) const { return m_rMinCombatSeparation; }
		int GetPerception (void) const { return m_iPerception; }
		CString GetValue (const CString &sSetting);
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		void InitToDefault (void);
		bool IsAggressor (void) const { return m_fAggressor; }
		bool IsFlocker (void) const { return (m_iFlockingStyle != flockNone); }
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
		void WriteToStream (IWriteStream *pStream);

		static AICombatStyles ConvertToAICombatStyle (const CString &sValue);
		static EFlockingStyles ConvertToFlockingStyle (const CString &sValue);
		static CString ConvertToID (AICombatStyles iStyle);
		static CString ConvertToID (EFlockingStyles iStyle);

	private:
		AICombatStyles m_iCombatStyle;			//	Combat style
		EFlockingStyles m_iFlockingStyle;		//	Flocking style

		int m_iFireRateAdj;						//	Adjustment to weapon's fire rate (10 = normal; 20 = double delay)
		int m_iFireRangeAdj;					//	Adjustment to range (100 = normal; 50 = half range)
		int m_iFireAccuracy;					//	Percent chance of hitting
		int m_iPerception;						//	Perception

		Metric m_rMinCombatSeparation;			//	Min separation from other ships while in combat

		DWORD m_fNoShieldRetreat:1;				//	Ship does not retreat when shields go down
		DWORD m_fNoDogfights:1;					//	Don't turn ship to face target
		DWORD m_fNonCombatant:1;				//	Tries to stay out of trouble
		DWORD m_fNoFriendlyFire:1;				//	Cannot hit friends
		DWORD m_fAggressor:1;					//	Attack targets of opportunity even if they haven't attacked
		DWORD m_fNoFriendlyFireCheck:1;			//	Do not check to see if friends are in line of fire
		DWORD m_fNoOrderGiver:1;				//	Always treated as the decider
		DWORD m_fAscendOnGate:1;				//	If TRUE, we ascend when the ship gates out

		DWORD m_fNoNavPaths:1;					//	If TRUE, do not use nav paths
		DWORD m_fNoAttackOnThreat:1;			//	Do not attack enemies while escorting (unless ordered)
		DWORD m_fNoTargetsOfOpportunity:1;		//	If TRUE, do not attack targets of opportunity
		DWORD m_fIsPlayer:1;					//	If TRUE, we're controlling the player ship (this is usually
												//		for debugging only).
		DWORD m_fSpare5:1;
		DWORD m_fSpare6:1;
		DWORD m_fSpare7:1;
		DWORD m_fSpare8:1;

		DWORD m_dwSpare:16;
	};

//	IShipController ------------------------------------------------------------
//
//	This abstract class is the root of all ship AI classes.

enum EOrderFlags
	{
	ORDER_FLAG_DELETE_ON_STATION_DESTROYED =	0x00000001,	//	Delete the order when target is station destroyed
	ORDER_FLAG_UPDATE_ON_NEW_PLAYER_SHIP =		0x00000002,	//	Update target if player changes ship
	ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED =	0x00000004,	//	Notify controller when any station destroyed
	ORDER_FLAG_DELETE_ON_OLD_SHIP_WAITS =		0x00000008,	//	Delete if player switched ships and old ship is waiting
	};

class IShipController
	{
	public:
		enum EDataTypes
			{
			dataNone,						//	dwData is ignored
			dataInteger,					//	dwData is a 32-bit integer
			dataPair,						//	dwData is two 16-bit integers
			dataString,						//	dwData is a pointer to a CString
			dataVector,						//	dwData is a pointer to a CVector
			dataItem,						//	dwData is a pointer to a CItem
			dataOrbitExact,				//	Encode orbit in SData
			};

		struct SData
			{
			SData (void) : iDataType(dataNone)
				{ }

			SData (DWORD dwData) : iDataType(dataInteger),
					dwData1(dwData)
				{ }

			SData (DWORD dwData1Arg, DWORD dwData2Arg) : iDataType(dataPair),
					dwData1(dwData1Arg),
					dwData2(dwData2Arg)
				{ }

			SData (const CString &sDataArg) : iDataType(dataString),
					sData(sDataArg)
				{ }

			SData (const CVector &vDataArg) : iDataType(dataVector),
					vData(vDataArg)
				{ }

			SData (const CItem &ItemArg) : iDataType(dataItem),
					Item(ItemArg)
				{ }

			DWORD AsInteger (void) const { if (iDataType == dataInteger || iDataType == dataPair) return dwData1; else return 0; }
			DWORD AsInteger2 (void) const { if (iDataType == dataPair) return dwData2; else return 0; }
			const CItem &AsItem (void) const { if (iDataType == dataItem) return Item; else return CItem::NullItem(); }
			bool IsIntegerOrPair (void) const { return (iDataType == dataInteger || iDataType == dataPair); }

			EDataTypes iDataType;

			DWORD dwData1;
			DWORD dwData2;
			CString sData;
			CVector vData;
			CItem Item;
			};

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
			orderOrbitExact,			//	pTarget = center; dwData1 = radius; dwData2 = timer; vData.x = startPosition; vData.y = eccentricity
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

		virtual void AddOrder (OrderTypes Order, CSpaceObject *pTarget, const IShipController::SData &Data, bool bAddBefore = false) { }
		virtual void CancelAllOrders (void) { }
		virtual void CancelCurrentOrder (void) { }
		virtual bool CancelOrder (int iIndex) { return false; }
		virtual DWORD GetCurrentOrderData (void) { return 0; }
		virtual OrderTypes GetCurrentOrderEx (CSpaceObject **retpTarget = NULL, IShipController::SData *retData = NULL) { return orderNone; }
		virtual OrderTypes GetOrder (int iIndex, CSpaceObject **retpTarget = NULL, IShipController::SData *retData = NULL) const { return orderNone; }
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

		static EDataTypes GetOrderDataType (OrderTypes iOrder);
		static DWORD GetOrderFlags (OrderTypes iOrder) { return m_OrderTypes[iOrder].dwFlags; }
		static CString GetOrderName (OrderTypes iOrder) { return CString(m_OrderTypes[iOrder].szName); }
		static OrderTypes GetOrderType (const CString &sString);
		static bool OrderHasTarget (OrderTypes iOrder, bool *retbRequired = NULL);
		static bool ParseOrderData (CCodeChainCtx &CCX, OrderTypes iOrder, const ICCItem &Args, int iFirstArg, SData &retData);
		static bool ParseOrderString (const CString &sValue, OrderTypes *retiOrder, IShipController::SData *retData = NULL);

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

class CShipAIHelper
	{
	public:
		static bool CalcFormationParams (CShip *pShip, const CVector &vDestPos, const CVector &vDestVel, CVector *retvRecommendedVel, Metric *retrDeltaPos2 = NULL, Metric *retrDeltaVel2 = NULL);
	};
