//	TSEShipAI.h
//
//	Defines classes and interfaces for ship AI.
//	Copyright (c) 2015 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

struct SCrewMetrics
	{
	enum EConstants
		{
		MIN_LEVEL =						  0,	//	Min
		LOW_LEVEL =						 50,	//	Low
		NEUTRAL_LOW_LEVEL =				 85,
		NEUTRAL_LEVEL =					100,	//	Normal
		NEUTRAL_HIGH_LEVEL =			115,
		HIGH_LEVEL =					150,	//	High
		PEAK_LEVEL =					180,	//	Peak
		MAX_LEVEL =						200,	//	Max
		};

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

		inline bool AscendOnGate (void) const { return m_fAscendOnGate; }
		inline AICombatStyles GetCombatStyle (void) const { return m_iCombatStyle; }
		inline int GetFireAccuracy (void) const { return m_iFireAccuracy; }
		inline int GetFireRangeAdj (void) const { return m_iFireRangeAdj; }
		inline int GetFireRateAdj (void) const { return m_iFireRateAdj; }
		inline EFlockingStyles GetFlockingStyle (void) const { return m_iFlockingStyle; }
		inline Metric GetMinCombatSeparation (void) const { return m_rMinCombatSeparation; }
		inline int GetPerception (void) const { return m_iPerception; }
		CString GetValue (const CString &sSetting);
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		void InitToDefault (void);
		inline bool IsAggressor (void) const { return m_fAggressor; }
		inline bool IsFlocker (void) const { return (m_iFlockingStyle != flockNone); }
		inline bool IsNonCombatant (void) const { return m_fNonCombatant; }
		inline bool NoAttackOnThreat (void) const { return m_fNoAttackOnThreat; }
		inline bool NoDogfights (void) const { return m_fNoDogfights; }
		inline bool NoFriendlyFire (void) const { return m_fNoFriendlyFire; }
		inline bool NoFriendlyFireCheck (void) const { return m_fNoFriendlyFireCheck; }
		inline bool NoNavPaths (void) const { return m_fNoNavPaths; }
		inline bool NoOrderGiver (void) const { return m_fNoOrderGiver; }
		inline bool NoShieldRetreat (void) const { return m_fNoShieldRetreat; }
		inline bool NoTargetsOfOpportunity (void) const { return m_fNoTargetsOfOpportunity; }
		void ReadFromStream (SLoadCtx &Ctx);
		inline void SetMinCombatSeparation (Metric rValue) { m_rMinCombatSeparation = rValue; }
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
		DWORD m_fSpare4:1;
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

			inline DWORD AsInteger (void) const { if (iDataType == dataInteger || iDataType == dataPair) return dwData1; else return 0; }
			inline DWORD AsInteger2 (void) const { if (iDataType == dataPair) return dwData2; else return 0; }
			inline bool IsIntegerOrPair (void) const { return (iDataType == dataInteger || iDataType == dataPair); }

			EDataTypes iDataType;

			DWORD dwData1;
			DWORD dwData2;
			CString sData;
			CVector vData;
			};

		//	When adding a new order, also add to:
		//
		//	g_OrderTypes in Utilities.cpp

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
			};

		virtual ~IShipController (void) { }

		virtual void AccumulateCrewMetrics (SCrewMetrics &Metrics) { }
		virtual void Behavior (SUpdateCtx &Ctx) { }
		virtual void CancelDocking (void) { }
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
		virtual CSpaceObject *GetTarget (CItemCtx &ItemCtx, bool bNoAutoTarget = false) const { return NULL; }
		virtual bool GetThrust (void) = 0;
		virtual void GetWeaponTarget (STargetingCtx &TargetingCtx, CItemCtx &ItemCtx, CSpaceObject **retpTarget, int *retiFireSolution) { }
		virtual bool IsAngryAt (CSpaceObject *pObj) const { return false; }
		virtual bool IsPlayer (void) const { return false; }
		virtual bool IsPlayerWingman (void) const { return false; }
		virtual void ReadFromStream (SLoadCtx &Ctx, CShip *pShip) { ASSERT(false); }
		virtual int SetAISettingInteger (const CString &sSetting, int iValue) { return 0; }
		virtual CString SetAISettingString (const CString &sSetting, const CString &sValue) { return NULL_STR; }
		virtual void SetCommandCode (ICCItem *pCode) { }
		virtual void SetManeuver (EManeuverTypes iManeuver) { }
		virtual void SetShipToControl (CShip *pShip) { }
		virtual void SetThrust (bool bThrust) { }
		virtual void SetPlayerWingman (bool bIsWingman) { }
		virtual void WriteToStream (IWriteStream *pStream) { ASSERT(false); }

		virtual void AddOrder (OrderTypes Order, CSpaceObject *pTarget, const IShipController::SData &Data, bool bAddBefore = false) { }
		virtual void CancelAllOrders (void) { }
		virtual void CancelCurrentOrder (void) { }
		virtual DWORD GetCurrentOrderData (void) { return 0; }
		virtual OrderTypes GetCurrentOrderEx (CSpaceObject **retpTarget = NULL, IShipController::SData *retData = NULL) { return orderNone; }

		//	Events

		virtual void OnAttacked (CSpaceObject *pAttacker, const SDamageCtx &Damage) { }
		virtual void OnBlindnessChanged (bool bBlind, bool bNoMessage = false) { }
		virtual DWORD OnCommunicate (CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2) { return resNoAnswer; }
		virtual void OnComponentChanged (ObjectComponentTypes iComponent) { }
		virtual void OnDamaged (const CDamageSource &Cause, CInstalledArmor *pArmor, const DamageDesc &Damage, int iDamage) { }
		virtual void OnDeviceEnabledDisabled (int iDev, bool bEnabled, bool bSilent = false) { }
		virtual void OnDeviceStatus (CInstalledDevice *pDev, CDeviceClass::DeviceNotificationTypes iEvent) { }
		virtual bool OnDestroyCheck (DestructionTypes iCause, const CDamageSource &Attacker) { return true; }
		virtual void OnDestroyed (SDestroyCtx &Ctx) { }
		virtual void OnDocked (CSpaceObject *pObj) { }
		virtual void OnDockedObjChanged (CSpaceObject *pLocation) { }
		virtual void OnEnterGate (CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate, bool bAscend) { }
        virtual void OnFuelConsumed (Metric rFuel) { }
		virtual void OnHitBarrier (CSpaceObject *pBarrierObj, const CVector &vPos) { CancelDocking(); }
		virtual void OnItemDamaged (const CItem &Item, int iHP) { }
		virtual void OnItemFired (const CItem &Item) { }
		virtual void OnItemInstalled (const CItem &Item) { }
		virtual void OnItemUninstalled (const CItem &Item) { }
		virtual void OnMissionCompleted (CMission *pMission, bool bSuccess) { }
		virtual void OnNewSystem (CSystem *pSystem) { }
		virtual void OnObjDamaged (const SDamageCtx &Ctx) { }
		virtual void OnObjDestroyed (const SDestroyCtx &Ctx) { }
		virtual void OnObjEnteredGate (CSpaceObject *pObj, CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate) { }
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
	};

