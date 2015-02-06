//	TSEShipAI.h
//
//	Defines classes and interfaces for ship AI.
//	Copyright (c) Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

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

			DWORD AsInteger (void) const { if (iDataType == dataInteger || iDataType == dataPair) return dwData1; else return 0; }
			DWORD AsInteger2 (void) const { if (iDataType == dataPair) return dwData2; else return 0; }

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
			orderAttackNearestEnemy,	//	Attack the nearest enemy
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
			};

		virtual ~IShipController (void) { }

		virtual void Behavior (void) { }
		virtual void CancelDocking (void) { }
		virtual CString DebugCrashInfo (void) { return NULL_STR; }
		virtual void DebugPaintInfo (CG16bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) { }
		virtual bool FollowsObjThroughGate (CSpaceObject *pLeader) { return false; }
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

		virtual void OnArmorRepaired (int iSection) { }
		virtual void OnAttacked (CSpaceObject *pAttacker, const DamageDesc &Damage) { }
		virtual void OnBlindnessChanged (bool bBlind, bool bNoMessage = false) { }
		virtual DWORD OnCommunicate (CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2) { return resNoAnswer; }
		virtual void OnComponentChanged (ObjectComponentTypes iComponent) { }
		virtual void OnDamaged (const CDamageSource &Cause, CInstalledArmor *pArmor, const DamageDesc &Damage, int iDamage) { }
		virtual void OnDeviceEnabledDisabled (int iDev, bool bEnabled, bool bSilent = false) { }
		virtual void OnDeviceStatus (CInstalledDevice *pDev, int iEvent) { }
		virtual bool OnDestroyCheck (DestructionTypes iCause, const CDamageSource &Attacker) { return true; }
		virtual void OnDestroyed (SDestroyCtx &Ctx) { }
		virtual void OnDocked (CSpaceObject *pObj) { }
		virtual void OnDockedObjChanged (CSpaceObject *pLocation) { }
		virtual void OnEnterGate (CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate, bool bAscend) { }
		virtual void OnFuelLowWarning (int iSeq) { }
		virtual void OnItemDamaged (const CItem &Item, int iHP) { }
		virtual void OnItemFired (const CItem &Item) { }
		virtual void OnItemInstalled (const CItem &Item) { }
		virtual void OnItemUninstalled (const CItem &Item) { }
		virtual void OnLifeSupportWarning (int iSecondsLeft) { }
		virtual void OnMissionCompleted (CMission *pMission, bool bSuccess) { }
		virtual void OnNewSystem (CSystem *pSystem) { }
		virtual void OnObjDamaged (const SDamageCtx &Ctx) { }
		virtual void OnObjDestroyed (const SDestroyCtx &Ctx) { }
		virtual void OnObjEnteredGate (CSpaceObject *pObj, CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate) { }
		virtual void OnPaintSRSEnhancements (CG16bitImage &Dest, SViewportPaintCtx &Ctx) { }
		virtual void OnPlayerChangedShips (CSpaceObject *pOldShip) { }
		virtual void OnPlayerObj (CSpaceObject *pPlayer) { }
		virtual void OnProgramDamage (CSpaceObject *pHacker, const ProgramDesc &Program) { }
		virtual void OnRadiationWarning (int iTicksLeft) { }
		virtual void OnRadiationCleared (void) { }
		virtual void OnReactorOverloadWarning (int iSeq) { }
		virtual void OnStatsChanged (void) { }
		virtual void OnStationDestroyed (const SDestroyCtx &Ctx) { }
		virtual void OnSystemLoaded (void) { }
		virtual void OnUpdatePlayer (SUpdateCtx &Ctx) { }
		virtual void OnWeaponStatusChanged (void) { }
		virtual void OnWreckCreated (CSpaceObject *pWreck) { }
	};

