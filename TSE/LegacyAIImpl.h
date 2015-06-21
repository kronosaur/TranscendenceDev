//	LegacyAIImpl.h
//
//	Implements legacy IShipController classes.
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CAutonAI : public CBaseShipAI
	{
	public:
		CAutonAI (void);

		//	IShipController virtuals
		virtual void Behavior (void);
		virtual CString DebugCrashInfo (void);
		virtual CString GetClass (void) { return CONSTLIT("auton"); }
		virtual CSpaceObject *GetTarget (CItemCtx &ItemCtx, bool bNoAutoTarget = false) const;
		virtual DWORD OnCommunicate (CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2);

	protected:
		//	CBaseShipAI overrides
		virtual void OnObjDestroyedNotify (const SDestroyCtx &Ctx);
		virtual void OnOrderChanged (void);
		virtual void OnReadFromStream (SLoadCtx &Ctx);
		virtual void OnSystemLoadedNotify (void);
		virtual void OnWriteToStream (IWriteStream *pStream);

	private:
		enum StateTypes
			{
			stateNone,
			stateEscorting,
			stateAttackingTarget,
			stateOnCourseForStargate,
			stateFollowing,
			stateWaiting,
			};

		void BehaviorStart (void);
		void SetState (StateTypes State);

		StateTypes m_State;
		CSpaceObject *m_pDest;
		CSpaceObject *m_pTarget;
	};

class CFerianShipAI : public CBaseShipAI
	{
	public:
		CFerianShipAI (void);

		//	IShipController virtuals
		virtual void Behavior (void);
		virtual CString DebugCrashInfo (void);
		virtual CSpaceObject *GetBase (void) const { return m_pBase; }
		virtual CString GetClass (void) { return CONSTLIT("ferian"); }
		virtual CSpaceObject *GetTarget (CItemCtx &ItemCtx, bool bNoAutoTarget = false) const { return m_pTarget; }
		virtual DWORD OnCommunicate (CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2);

	protected:
		//	CBaseShipAI overrides
		virtual void OnObjDestroyedNotify (const SDestroyCtx &Ctx);
		virtual void OnOrderChanged (void);
		virtual void OnReadFromStream (SLoadCtx &Ctx);
		virtual void OnWriteToStream (IWriteStream *pStream);

	private:
		enum StateTypes
			{
			stateNone,
			stateOnCourseForMine,
			stateOnCourseForTarget,
			stateMining,
			stateAttackingThreat,
			stateOnCourseForStargate,
			stateAvoidThreat,
			stateAttackingTarget,
			stateWaitForPlayerAtGate,
			};

		void BehaviorStart (void);
		CSpaceObject *FindRandomAsteroid (void);
		bool InRangeOfThreat (CSpaceObject **retpThreat);
		void SetState (StateTypes State);

		StateTypes m_State;						//	Ship's current state
		CSpaceObject *m_pBase;
		CSpaceObject *m_pTarget;
	};

class CFleetCommandAI : public CBaseShipAI
	{
	public:
		CFleetCommandAI (void);
		virtual ~CFleetCommandAI (void);

		//	IShipController virtuals
		virtual void Behavior (void);
		virtual CString DebugCrashInfo (void);
		virtual CString GetClass (void) { return CONSTLIT("fleetcommand"); }
		virtual CSpaceObject *GetTarget (CItemCtx &ItemCtx, bool bNoAutoTarget = false) const { return m_pTarget; }
		virtual DWORD OnCommunicate (CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2);

	protected:
		//	CBaseShipAI overrides
		virtual void OnDockedEvent (CSpaceObject *pObj);
		virtual void OnNewSystemNotify (void);
		virtual void OnObjDestroyedNotify (const SDestroyCtx &Ctx);
		virtual void OnOrderChanged (void);
		virtual void OnReadFromStream (SLoadCtx &Ctx);
		virtual void OnWriteToStream (IWriteStream *pStream);

	private:
		enum StateTypes
			{
			stateNone,
			stateAttackAtWill,
			stateChargeInFormation,
			stateFormAtRallyPoint,
			stateAttackFromRallyPoint,
			stateOnCourseForStargate,
			stateWaiting,
			stateWaitingForThreat,
			};

		struct STarget
			{
			CSpaceObject *pTarget;
			int iAssignedTo;					//	Power points assigned to destroy this target
			int iKilled;						//	Power points killed by this asset
			};

		struct SAsset
			{
			CSpaceObject *pAsset;
			int iFormationPos;
			CSpaceObject *pTarget;				//	Assigned target (NULL = none)
			};

		SAsset *AddAsset (CSpaceObject *pAsset);
		void AddTarget (CSpaceObject *pTarget);
		void BehaviorStart (void);
		void ComputeCombatPower (int *retiAssetPower, int *retiTargetPower);
		bool ComputeFormationReady (void);
		CVector ComputeRallyPoint (CSpaceObject *pTarget, int *retiFacing = NULL);
		CVector ComputeRallyPointEx (int iBearing, CSpaceObject *pTarget, int *retiFacing = NULL);
		SAsset *FindAsset (CSpaceObject *pAsset, int *retiIndex = NULL);
		STarget *FindTarget (CSpaceObject *pTarget, int *retiIndex = NULL);
		void ImplementAttackAtWill (void);
		void ImplementAttackFromRallyPoint (void);
		void ImplementChargeInFormation (void);
		void ImplementFormAtRallyPoint (void);
		void ImplementWaitAtPosition (void);
		void OrderAllFormUp (bool bResetFormation = false);
		void OrderAllOpenFire (void);
		void OrderAttackTarget (CSpaceObject *pTarget);
		void RemoveAsset (int iIndex);
		bool RemoveAssetObj (CSpaceObject *pObj);
		void RemoveTarget (int iIndex);
		void SetState (StateTypes State);
		void UpdateAttackTargets (void);
		void UpdateTargetList (void);
		void UpdateTargetListAndPotentials (void);

		StateTypes m_State;

		CSpaceObject *m_pTarget;
		CSpaceObject *m_pObjective;
		int m_iCounter;

		int m_iStartingAssetCount;
		int m_iAssetCount;
		int m_iAssetAlloc;
		SAsset *m_pAssets;

		int m_iTargetCount;
		int m_iTargetAlloc;
		STarget *m_pTargets;

		CVector m_vThreatPotential;
		CVector m_vRallyPoint;
		int m_iRallyFacing;

		DWORD m_fOpenFireOrdered:1;
		DWORD m_dwSpare:31;
	};

class CFleetShipAI : public CBaseShipAI
	{
	public:
		CFleetShipAI (void);

		//	IShipController virtuals
		virtual void Behavior (void);
		virtual CString DebugCrashInfo (void);
		virtual CString GetClass (void) { return CONSTLIT("fleet"); }
		virtual CSpaceObject *GetTarget (CItemCtx &ItemCtx, bool bNoAutoTarget = false) const;
		virtual DWORD OnCommunicate (CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2);

	protected:
		//	CBaseShipAI overrides
		virtual void OnDockedEvent (CSpaceObject *pObj);
		virtual CSpaceObject *OnGetBase (void) const;
		virtual void OnObjDestroyedNotify (const SDestroyCtx &Ctx);
		virtual void OnOrderChanged (void);
		virtual void OnReadFromStream (SLoadCtx &Ctx);
		virtual void OnWriteToStream (IWriteStream *pStream);

	private:
		enum StateTypes
			{
			stateNone,
			stateKeepFormation,
			stateAttackAtWill,
			stateAttackInFormation,
			stateAttackTarget,
			stateOnCourseForStargate,				//	Heading for m_pDest to gate out
			stateOnCourseForDocking,				//	Heading for m_pDest to dock
			stateWaitForPlayerAtGate,
			stateOnPatrolOrbit,
			stateAttackOnPatrol,
			stateWaiting,
			};

		void BehaviorStart (void);
		CVector ComputeAttackPos (CSpaceObject *pTarget, Metric rDist, int *retiFacing);
		void ImplementFireOnNearestTarget (void);
		void ImplementKeepFormation (bool *retbInFormation = NULL);
		void SetState (StateTypes State);

		StateTypes m_State;						//	Ship's current state
		CSpaceObject *m_pLeader;				//	Group leader
		int m_iFormation;						//	Formation to use
		int m_iPlace;							//	Place in formation
		int m_iCounter;							//	Misc counter
		int m_iAngle;							//	Misc angle
		CVector m_vVector;				//	Misc vector

		CSpaceObject *m_pTarget;
		CSpaceObject *m_pDest;
	};

class CGaianProcessorAI : public CBaseShipAI
	{
	public:
		CGaianProcessorAI (void);

		//	IShipController virtuals
		virtual void Behavior (void);
		virtual CString DebugCrashInfo (void);
		virtual CString GetClass (void) { return CONSTLIT("gaianprocessor"); }
		virtual CSpaceObject *GetTarget (CItemCtx &ItemCtx, bool bNoAutoTarget = false) const;
		virtual DWORD OnCommunicate (CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2);

	protected:
		//	CBaseShipAI overrides
		virtual void OnObjDestroyedNotify (const SDestroyCtx &Ctx);
		virtual void OnOrderChanged (void);
		virtual void OnReadFromStream (SLoadCtx &Ctx);
		virtual void OnSystemLoadedNotify (void);
		virtual void OnWriteToStream (IWriteStream *pStream);

	private:
		enum StateTypes
			{
			stateNone,
			stateLookingForWreck,
			statePulverizingWreck,
			stateConsumingWreck
			};

		void BehaviorStart (void);
		void CalcDevices (void);
		void SetState (StateTypes State);

		StateTypes m_State;						//	Ship's current state
		Metric m_rOrbitRadius;					//	Current orbital radius
		CSpaceObject *m_pTarget;
		CSpaceObject *m_pDest;					//	Scrap to eat
		int m_iStartTime;						//	Start time for various actions

		//	Volatiles
		int m_iDestructorDev;					//	Destructor weapon
		int m_iDisablerDev;						//	Disabling weapon
		DWORD m_dwAmmo;							//	Ammo to manufacture
	};

class CGladiatorAI : public CBaseShipAI
	{
	public:
		CGladiatorAI (void);

		//	IShipController virtuals
		virtual void Behavior (void);
		virtual CString DebugCrashInfo (void);
		virtual CString GetClass (void) { return CONSTLIT("gladiator"); }
		virtual CSpaceObject *GetTarget (CItemCtx &ItemCtx, bool bNoAutoTarget = false) const;

	protected:
		//	CBaseShipAI overrides
		virtual void OnObjDestroyedNotify (const SDestroyCtx &Ctx);
		virtual void OnOrderChanged (void);
		virtual void OnReadFromStream (SLoadCtx &Ctx);
		virtual void OnWriteToStream (IWriteStream *pStream);

	private:
		enum StateTypes
			{
			stateNone,
			stateAttackingTarget,
			stateOnCourseForDocking,
			stateOnCourseForStargate,
			};

		void BehaviorStart (void);
		CVector ComputePotential (void);
		void SetState (StateTypes State);

		StateTypes m_State;
		CSpaceObject *m_pDest;
		CSpaceObject *m_pTarget;
		CVector m_vPotential;
	};

class CStandardShipAI : public CBaseShipAI
	{
	public:
		CStandardShipAI (void);
		virtual ~CStandardShipAI (void);

		static void SetDebugShip (CShip *pShip);

	protected:
		//	CBaseShipAI overrides
		virtual void OnAttackedNotify (CSpaceObject *pAttacker, const DamageDesc &Damage);
		virtual void OnBehavior (void);
		virtual void OnCleanUp (void) { m_State = stateNone; m_pTarget = NULL; m_pDest = NULL; }
		virtual DWORD OnCommunicateNotify (CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2);
		virtual CString OnDebugCrashInfo (void);
		virtual void OnDockedEvent (CSpaceObject *pObj);
		virtual CSpaceObject *OnGetBase (void) const;
		virtual CSpaceObject *OnGetTarget (bool bNoAutoTarget = false) const;
		virtual void OnObjDestroyedNotify (const SDestroyCtx &Ctx);
		virtual void OnOrderChanged (void);
		virtual void OnReadFromStream (SLoadCtx &Ctx);
		virtual void OnWriteToStream (IWriteStream *pStream);

	private:
		enum StateTypes
			{
			stateAttackingOnPatrol,					//	Attacking m_pTarget while on patrol

			stateAttackingTarget,					//	Attacking m_pTarget
			stateAttackingThreat,					//	Attacking m_pTarget to protect principal
			stateDeterTarget,						//	Attack m_pTarget, but stop if target doesn't attack back
			stateDeterTargetNoChase,				//	Attack m_pTarget, but don't chase it
			stateEscorting,							//	Escorting m_pDest
			stateHolding,							//	Doing nothing indefinitely
			stateLookingForLoot,					//	Looking for loot in the system
			stateLooting,							//	Looting stuff from station
			stateLootingOnce,						//	Looting stuff from station
			stateMaintainBearing,					//	Move in m_rDistance (angle) direction
													//		until we run into enemies.
			stateNone,								//	Ship needs to consult orders
			stateOnCourseForDocking,				//	Heading for m_pDest to dock
			stateOnCourseForStargate,				//	Heading for m_pDest to gate out
			stateOnPatrolOrbit,						//	Orbiting m_pDest at m_rDistance
			stateReturningFromThreat,				//	Dock with m_pDest
			stateWaiting,							//	Waiting for countdown (m_iCountdown)
			stateWaitingForThreat,					//	Waiting for a threat to m_pDest
			stateWaitingUnarmed,					//	If threat shows up, we gate out
			stateWaitForPlayerAtGate,				//	Waiting for player at gate
			stateFollowing,

			stateOnCourseForLootDocking,			//	Going to dock with loot
			stateDeterTargetWhileLootDocking,		//	Attack the target, but continue towards loot
			stateFollowNavPath,						//	Follow current nav path
			stateReturningViaNavPath,				//	Follow current nav path
			stateOnCourseForDockingViaNavPath,		//	Follow current nav path
			stateOnCourseForPatrolViaNavPath,		//	Follow current nav path
			stateOnCourseForPoint,					//	Going to a given point
			stateWaitingForTarget,					//	Waiting for m_pTarget
			stateWaitingForEnemy,					//	Waiting for an enemy
			stateAttackingTargetAndAvoiding,		//	Attacking m_pTarget and avoiding enemy stations
			
			stateAvoidingEnemyStation,				//	Attacking m_pTarget and avoiding m_pDest
			stateOnCourseForPointViaNavPath,		//	Follow current nav path
			stateBombarding,						//	Attacking m_pTarget while holding
			stateApproaching,						//	Get within m_rDistance of m_pDest
			stateAimingAtTarget,					//	Aiming at m_pTarget
			stateOnCourseForOrbitViaNavPath,		//	Follow current nav path
			stateOrbit,								//	Orbit m_pTarget
			stateReturningToEscort,					//	Return to m_pDest from an attack
			stateHoldingCourse,						//	Hold course
			stateAttackingPrincipalThreat,			//	Attacking m_pTarget, which attacked principal

			stateTurningTo,							//	Turning to angle
			stateAttackingTargetAndHolding,			//	Attacking m_pTarget while holding position
			};

		struct TacticsSet
			{
			DWORD fCloseOnTarget:1;					//	Header for target
			DWORD fFireAtTarget:1;					//	Fire at target
			DWORD fFireOnOpportunity:1;				//	Fire at any target of opportunity
			DWORD fFlankTarget:1;					//	When closing on a target, aim a little to the side
			DWORD fSpiralToTarget:1;				//	Spiral towards target
			DWORD fSpiralAwayFromTarget:1;			//	Spiral away from target
			DWORD fEscortTarget:1;					//	Follow target
			};

		void AttackShipInRange (CSpaceObject *pCenter, Metric rRange);
		void BehaviorStart (void);
		void CommunicateWithEscorts (MessageTypes iMessage, CSpaceObject *pParam1 = NULL, DWORD dwParam2 = 0);
		void ImplementTactics (TacticsSet &Tactics, CVector &vTarget, Metric rTargetDist2);
		void SetState (StateTypes State);

#ifdef DEBUG_INVALID_DEST
		void SetDebugDest (CSpaceObject *pDest);
#endif

		//	Orders and state
		StateTypes m_State;						//	Ship's current state
		CSpaceObject *m_pDest;					//	Destination
		CSpaceObject *m_pTarget;				//	Ship's target
		Metric m_rDistance;						//	Distance
		int m_iCountdown;						//	Timer
#ifdef DEBUG_INVALID_DEST
		CString m_sDest;						//	Description of m_pDest to help debug
#endif

		//	Flags
		DWORD m_fHasEscorts:1;					//	TRUE if ship has escorts
	};

class CZoanthropeAI : public CBaseShipAI
	{
	public:
		CZoanthropeAI (void);

		//	IShipController virtuals
		virtual void Behavior (void);
		virtual CString DebugCrashInfo (void);
		virtual CSpaceObject *GetBase (void) const;
		virtual CString GetClass (void) { return CONSTLIT("zoanthrope"); }
		virtual CSpaceObject *GetTarget (CItemCtx &ItemCtx, bool bNoAutoTarget = false) const;
		virtual void OnAttacked (CSpaceObject *pAttacker, const DamageDesc &Damage);
		virtual DWORD OnCommunicate (CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2);

	protected:
		//	CBaseShipAI overrides
		virtual void OnDockedEvent (CSpaceObject *pObj);
		virtual void OnObjDestroyedNotify (const SDestroyCtx &Ctx);
		virtual void OnOrderChanged (void);
		virtual void OnReadFromStream (SLoadCtx &Ctx);
		virtual void OnWriteToStream (IWriteStream *pStream);

	private:
		enum StateTypes
			{
			stateNone,
			stateAttackingThreat,
			stateOnCourseForStargate,
			stateReturningFromThreat,
			stateWaitingForThreat,
			stateOnPatrolOrbit,
			stateAttackingOnPatrol,
			stateOnEscortCourse,
			stateWaiting,
			};

		void BehaviorStart (void);
		void ImplementCombatManeuvers (CSpaceObject *pTarget);
		void SetState (StateTypes State, CSpaceObject *pBase = NULL, CSpaceObject *pTarget = NULL);

		StateTypes m_State;						//	Ship's current state
		CSpaceObject *m_pBase;
		CSpaceObject *m_pTarget;

		CVector m_vPotential;
	};

