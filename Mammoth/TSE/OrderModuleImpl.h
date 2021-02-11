//	OrderModuleImpl.h
//
//	Transcendence IOrderModule classes
//	Copyright (c) 2021 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CApproachOrder : public IOrderModule
	{
	public:
		CApproachOrder (void) : IOrderModule(objCount)
			{ }

	protected:
		//	IOrderModule virtuals
		virtual void OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx) override;
		virtual void OnBehaviorStart (CShip &Ship, CAIBehaviorCtx &Ctx, const COrderDesc &OrderDesc) override;
		virtual IShipController::OrderTypes OnGetOrder (void) override { return IShipController::orderApproach; }
		virtual CSpaceObject *OnGetTarget (void) override { return m_Objs[objTarget]; }
		virtual AIReaction OnGetReactToAttack () const override { return AIReaction::DeterWithSecondaries; }
		virtual AIReaction OnGetReactToThreat () const override { return AIReaction::None; }
		virtual void OnReadFromStream (SLoadCtx &Ctx) override;
		virtual void OnWriteToStream (IWriteStream *pStream) const override;

	private:
		enum Objs
			{
			objDest =		0,
			objTarget =		1,

			objCount =		2,
			};

		enum class EState
			{
			None =						-1,
			OnCourseViaNavPath =		0,
			Approaching =				1,
			};

		EState m_iState = EState::None;			//	Current behavior state
		Metric m_rMinDist2 = 0.0;				//	Minimum distance to target
	};

class CAttackOrder : public IOrderModule
	{
	public:
		CAttackOrder (IShipController::OrderTypes iOrder);

	protected:
		//	IOrderModule virtuals
		virtual bool IsAttacking (void) override { return true; }
		virtual void OnAttacked (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pAttacker, const SDamageCtx &Damage, bool bFriendlyFire) override;
		virtual void OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx) override;
		virtual void OnBehaviorStart (CShip &Ship, CAIBehaviorCtx &Ctx, const COrderDesc &OrderDesc) override;
		virtual CString OnDebugCrashInfo (void) override;
		virtual IShipController::OrderTypes OnGetOrder (void) override { return m_iOrder; }
		virtual CSpaceObject *OnGetTarget (void) override { return m_Objs[OBJ_TARGET]; }
		virtual void OnObjDestroyed (CShip *pShip, const SDestroyCtx &Ctx, int iObj, bool *retbCancelOrder) override;
		virtual void OnReadFromStream (SLoadCtx &Ctx) override;
		virtual void OnWriteToStream (IWriteStream *pStream) const override;

	private:
		static constexpr int OBJ_TARGET =	0;
		static constexpr int OBJ_AVOID =	1;
		static constexpr int OBJ_COUNT =	2;

		enum class EState
			{
			None =							-1,
			AttackingTargetAndAvoiding =	0,
			AvoidingEnemyStation =			1,
			AttackingTargetAndHolding =		2,
			};

		CSpaceObject *GetBestTarget (CShip *pShip);
		CSpaceObject *GetTargetArea (CShip *pShip, Metric *retrRange);
		bool IsBetterTarget (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pOldTarget, CSpaceObject *pNewTarget);
		bool IsInTargetArea (CShip *pShip, CSpaceObject *pObj);
		bool MustRetreat (CShip &Ship) const;

		IShipController::OrderTypes m_iOrder = IShipController::orderNone;
		EState m_iState = EState::None;				//	Current behavior state
		int m_iCountdown = -1;						//	Stop attacking after this time (-1 = no limit)

		DWORD m_fNearestTarget:1 = false;			//	If TRUE, continue attacking other targets
		DWORD m_fInRangeOfObject:1 = false;			//	If TRUE, new target must be in range of order object
		DWORD m_fHold:1 = false;					//	If TRUE, hold position
		DWORD m_fStayInArea:1 = false;				//  If TRUE, do not wander more than 50 ls beyond target area
		DWORD m_fRetreatIfNecessary:1 = false;		//	If TRUE, we cancel order after a certain amount of damage

		DWORD m_fSpare6:1 = false;
		DWORD m_fSpare7:1 = false;
		DWORD m_fSpare8:1 = false;
		DWORD m_dwSpare:24 = 0;
	};

class CAttackStationOrder : public IOrderModule
	{
	public:
		CAttackStationOrder (void) : IOrderModule(objCount)
			{ }

	protected:
		//	IOrderModule virtuals
		virtual bool IsAttacking (void) override { return true; }
		virtual void OnAttacked (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pAttacker, const SDamageCtx &Damage, bool bFriendlyFire) override;
		virtual void OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx) override;
		virtual void OnBehaviorStart (CShip &Ship, CAIBehaviorCtx &Ctx, const COrderDesc &OrderDesc) override;
		virtual CString OnDebugCrashInfo (void) override;
		virtual IShipController::OrderTypes OnGetOrder (void) override { return IShipController::orderAttackStation; }
		virtual CSpaceObject *OnGetTarget (void) override { return m_Objs[objTarget]; }
		virtual void OnObjDestroyed (CShip *pShip, const SDestroyCtx &Ctx, int iObj, bool *retbCancelOrder) override;
		virtual void OnReadFromStream (SLoadCtx &Ctx) override;
		virtual void OnWriteToStream (IWriteStream *pStream) const override;

	private:
		enum Objs
			{
			objTarget =		0,
			objDefender =	1,

			objCount =		2,
			};

		enum States
			{
			stateAttackingTarget,
			stateAttackingDefender,
			stateFollowingNavPath,
			};

		States m_iState;						//	Current behavior state
		int m_iCountdown;						//	Stop attacking after this time
	};

class CDeterChaseOrder : public IOrderModule
	{
	public:
		CDeterChaseOrder () : IOrderModule(OBJ_COUNT)
			{ }

		static COrderDesc Create (CSpaceObject &TargetObj, CSpaceObject *pBase, Metric rMaxRange = 0.0, int iTimer = 0);

	protected:

		//	IOrderModule virtuals

		virtual void OnAttacked (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pAttacker, const SDamageCtx &Damage, bool bFriendlyFire) override;
		virtual void OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx) override;
		virtual void OnBehaviorStart (CShip &Ship, CAIBehaviorCtx &Ctx, const COrderDesc &OrderDesc) override;
		virtual DWORD OnCommunicate (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2, ICCItem *pData);
		virtual void OnDestroyed (CShip *pShip, SDestroyCtx &Ctx) override;
		virtual CSpaceObject *OnGetBase (void) override { return m_Objs[OBJ_BASE]; }
		virtual IShipController::OrderTypes OnGetOrder (void) override { return IShipController::orderDeterChase; }
		virtual CSpaceObject *OnGetTarget (void) override { return m_Objs[OBJ_TARGET]; }
		virtual void OnObjDestroyed (CShip *pShip, const SDestroyCtx &Ctx, int iObj, bool *retbCancelOrder) override;
		virtual void OnReadFromStream (SLoadCtx &Ctx) override;
		virtual void OnWriteToStream (IWriteStream *pStream) const override;

	private:
		static constexpr int OBJ_BASE =		0;
		static constexpr int OBJ_TARGET =	1;
		static constexpr int OBJ_COUNT =	2;

		Metric m_rMaxRange2 = 0.0;				//	Stop if we're too far from our base
		int m_iCountdown = 0;					//	Stop after this time.
	};

class CFireEventOrder : public IOrderModule
	{
	public:
		CFireEventOrder (void) : IOrderModule(0)
			{ }

	protected:
		//	IOrderModule virtuals
		virtual void OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx) override;
		virtual IShipController::OrderTypes OnGetOrder (void) override { return IShipController::orderFireEvent; }
	};

class CEscortOrder : public IOrderModule
	{
	public:
		CEscortOrder (IShipController::OrderTypes iOrder);

	protected:
		//	IOrderModule virtuals
		virtual void OnAttacked (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pAttacker, const SDamageCtx &Damage, bool bFriendlyFire) override;
		virtual void OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx) override;
		virtual void OnBehaviorStart (CShip &Ship, CAIBehaviorCtx &Ctx, const COrderDesc &OrderDesc) override;
		virtual DWORD OnCommunicate (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2, ICCItem *pData) override;
		virtual CSpaceObject *OnGetBase (void) override;
		virtual IShipController::OrderTypes OnGetOrder (void) override { return IShipController::orderEscort; }
		virtual CSpaceObject *OnGetTarget (void) override { return m_Objs[objTarget]; }
		virtual void OnObjDestroyed (CShip *pShip, const SDestroyCtx &Ctx, int iObj, bool *retbCancelOrder) override;
		virtual void OnReadFromStream (SLoadCtx &Ctx) override;
		virtual void OnWriteToStream (IWriteStream *pStream) const override;

	private:
		enum Objs
			{
			objPrincipal =	0,
			objTarget =		1,

			objCount =		2,
			};

		enum States
			{
			stateEscorting =					0,
			stateAttackingThreat =				1,
			stateWaiting =						2,
			};

		IShipController::OrderTypes m_iOrder;
		States m_iState;

		DWORD m_fDeterEnemies:1;			//	If TRUE, attack enemies that approach
		DWORD m_fSpare2:1;
		DWORD m_fSpare3:1;
		DWORD m_fSpare4:1;
		DWORD m_fSpare5:1;
		DWORD m_fSpare6:1;
		DWORD m_fSpare7:1;
		DWORD m_fSpare8:1;

		DWORD m_dwSpare:24;
	};

class CGuardOrder : public IOrderModule
	{
	public:
		CGuardOrder (void) : IOrderModule(0)
			{ }

	protected:
		//	IOrderModule virtuals
		virtual void OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx) override;
		virtual void OnBehaviorStart (CShip &Ship, CAIBehaviorCtx &Ctx, const COrderDesc &OrderDesc) override;
		virtual IShipController::OrderTypes OnGetOrder (void) override { return IShipController::orderGuard; }
		virtual void OnReadFromStream (SLoadCtx &Ctx) override;
		virtual void OnWriteToStream (IWriteStream *pStream) const override;

	private:
		enum States
			{
			stateWaitingForThreat,
			stateReturningViaNavPath,
			stateReturningFromThreat,
			};

		States m_iState;						//	Current behavior state
		CSpaceObject *m_pBase;					//	Object that we're guarding
	};

class CNavigateOrder : public IOrderModule
	{
	public:
		CNavigateOrder (IShipController::OrderTypes iOrder);

	protected:

		//	IOrderModule virtuals

		virtual void OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx) override;
		virtual void OnBehaviorStart (CShip &Ship, CAIBehaviorCtx &Ctx, const COrderDesc &OrderDesc) override;
		virtual CSpaceObject *OnGetBase (void) override;
		virtual IShipController::OrderTypes OnGetOrder (void) override { return m_iOrder; }
		virtual CSpaceObject *OnGetTarget (void) override { return m_Objs[objTarget]; }
		virtual AIReaction OnGetReactToAttack () const override { return AIReaction::DeterWithSecondaries; }
		virtual AIReaction OnGetReactToBaseDestroyed () const override { return AIReaction::None; }
		virtual AIReaction OnGetReactToThreat () const override { return AIReaction::None; }
		virtual void OnObjDestroyed (CShip *pShip, const SDestroyCtx &Ctx, int iObj, bool *retbCancelOrder) override;
		virtual void OnReadFromStream (SLoadCtx &Ctx) override;
		virtual void OnWriteToStream (IWriteStream *pStream) const override;

	private:
		enum Objs
			{
			objDest =		0,
			objTarget =		1,

			objCount =		2,
			};

		IShipController::OrderTypes m_iOrder = IShipController::orderNone;
		CVector m_vDest;						//	Destination
		int m_iDestFacing = -1;					//	Ship should face at this angle
		Metric m_rMinDist2 = 0.0;				//	Minimum distance to target

		DWORD m_fTargetVector:1 = false;		//	Destination is m_vDest
		DWORD m_fTargetObj:1 = false;			//	Destination is objDest
		DWORD m_fIsFollowingNavPath:1 = false;	//	We're following the current nav path
		DWORD m_fDockAtDestination:1 = false;	//	When we reach our destination, dock
		DWORD m_fVariableMinDist:1 = false;		//	If TRUE, order parameter specifies min distance
		DWORD m_fNavPathOnly:1 = false;			//	If TRUE, we're done as soon as the nav path completes
		DWORD m_fGateAtDestination:1 = false;	//	When we reach our destination, gate
		DWORD m_fSpare8:1 = false;

		DWORD m_dwSpare:24 = 0;
	};

class COrbitExactOrder : public IOrderModule
	{
	public:
		static constexpr DWORD AUTO_ANGLE = 0xffff;
		static constexpr int DEFAULT_RADIUS = 10;
		static constexpr Metric DEFAULT_SPEED = 1.0;

		COrbitExactOrder (IShipController::OrderTypes iOrder) : IOrderModule(OBJ_COUNT),
				m_iOrder(iOrder)
			{ }

		static TArray<CShip *> GetOrbitMates (CSpaceObject &Source, DWORD dwRadius);
		static Metric CalcRadiusInLightSeconds (const COrderDesc &OrderDesc);
		static bool IsAutoAngle (const COrderDesc &OrderDesc, Metric *retrAngleInRadians = NULL);
		static void DistributeOrbitAngles (CShip &Ship, CSpaceObject &Source, const TArray<CShip *> &Ships, Metric &retrShipAngle);

	protected:

		//	IOrderModule virtuals

		virtual void OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx) override;
		virtual void OnBehaviorStart (CShip &Ship, CAIBehaviorCtx &Ctx, const COrderDesc &OrderDesc) override;
		virtual DWORD OnCommunicate (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2, ICCItem *pData);
		virtual void OnDestroyed (CShip *pShip, SDestroyCtx &Ctx) override;
		virtual CSpaceObject *OnGetBase (void) override { return m_Objs[OBJ_BASE]; }
		virtual IShipController::OrderTypes OnGetOrder (void) override { return m_iOrder; }
		virtual AIReaction OnGetReactToAttack () const override;
		virtual AIReaction OnGetReactToBaseDestroyed () const override;
		virtual AIReaction OnGetReactToThreat () const override;
		virtual CSpaceObject *OnGetTarget (void) override { return m_Objs[OBJ_TARGET]; }
		virtual Metric OnGetThreatRange (void) const override;
		virtual void OnObjDestroyed (CShip *pShip, const SDestroyCtx &Ctx, int iObj, bool *retbCancelOrder) override;
		virtual void OnReadFromStream (SLoadCtx &Ctx) override;
		virtual void OnWriteToStream (IWriteStream *pStream) const override;

	private:
		static constexpr int OBJ_BASE =		0;
		static constexpr int OBJ_TARGET =	1;
		static constexpr int OBJ_COUNT =	2;

		static constexpr Metric PATROL_SENSOR_RANGE =		(30.0 * LIGHT_SECOND);
		static constexpr Metric STOP_ATTACK_RANGE =			(120.0 * LIGHT_SECOND);

		static constexpr Metric NAV_PATH_THRESHOLD =		(50.0 * LIGHT_SECOND);
		static constexpr Metric NAV_PATH_THRESHOLD2 =		(NAV_PATH_THRESHOLD * NAV_PATH_THRESHOLD);

		static constexpr int RESUPPLY_CHECK_TIME =			151;

		void CalcIntermediates ();

		IShipController::OrderTypes m_iOrder = IShipController::orderNone;
		COrbit m_Orbit;							//	Orbit definition
		DWORD m_dwStartTick = 0;				//	Tick at start angle
		Metric m_rAngularSpeed = DEFAULT_SPEED;	//	Orbit speed (degrees per tick)
		int m_iCountdown = 0;					//	Stop after this time.

		Metric m_rNavThreshold2 = 0.0;			//	If further that this from center, nav back
	};

class CPatrolOrder : public IOrderModule
	{
	public:
		CPatrolOrder () : IOrderModule(OBJ_COUNT)
			{ }

	protected:

		//	IOrderModule virtuals

		virtual void OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx) override;
		virtual void OnBehaviorStart (CShip &Ship, CAIBehaviorCtx &Ctx, const COrderDesc &OrderDesc) override;
		virtual void OnDestroyed (CShip *pShip, SDestroyCtx &Ctx) override;
		virtual CSpaceObject *OnGetBase (void) override { return m_Objs[OBJ_BASE]; }
		virtual IShipController::OrderTypes OnGetOrder (void) override { return IShipController::orderPatrol; }
		virtual CSpaceObject *OnGetTarget (void) override { return m_Objs[OBJ_TARGET]; }
		virtual AIReaction OnGetReactToAttack () const override { return AIReaction::Chase; }
		virtual AIReaction OnGetReactToBaseDestroyed () const override { return AIReaction::DestroyAndRetaliate; }
		virtual AIReaction OnGetReactToThreat () const override { return AIReaction::Chase; }
		virtual void OnReadFromStream (SLoadCtx &Ctx) override;
		virtual Metric OnGetThreatRange (void) const override { return Max(m_rPatrolRadius + PATROL_SENSOR_RANGE, STOP_ATTACK_RANGE); }
		virtual DWORD OnGetThreatTargetTypes () const { return ((DWORD)CTargetList::ETargetType::AggressiveShip | (DWORD)CTargetList::ETargetType::NonAggressiveShip); }
		virtual void OnWriteToStream (IWriteStream *pStream) const override;

	private:
		static constexpr int OBJ_BASE =		0;
		static constexpr int OBJ_TARGET =	1;
		static constexpr int OBJ_COUNT =	2;

		static constexpr Metric PATROL_SENSOR_RANGE =		30.0 * LIGHT_SECOND;
		static constexpr Metric STOP_ATTACK_RANGE =			120.0 * LIGHT_SECOND;
		static constexpr Metric PATROL_DETER_RANGE =		80.0 * LIGHT_SECOND;
		static constexpr Metric PATROL_DETER_RANGE2 =		PATROL_DETER_RANGE * PATROL_DETER_RANGE;
		static constexpr Metric NAV_PATH_THRESHOLD =		(4.0 * PATROL_SENSOR_RANGE);
		static constexpr Metric NAV_PATH_THRESHOLD2 =		(NAV_PATH_THRESHOLD * NAV_PATH_THRESHOLD);

		static constexpr int RESUPPLY_CHECK_TIME =			151;

		void CalcIntermediates ();

		Metric m_rPatrolRadius = 0.0;

		Metric m_rNavThreshold2 = 0.0;			//	If further that this from center, nav back
	};

class CSendMessageOrder : public IOrderModule
	{
	public:
		CSendMessageOrder (void) : IOrderModule(0)
			{ }

	protected:
		//	IOrderModule virtuals
		virtual void OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx) override;
		virtual IShipController::OrderTypes OnGetOrder (void) override { return IShipController::orderSendMessage; }
	};

class CSentryOrder : public IOrderModule
	{
	public:
		CSentryOrder (void);

	protected:

		//	IOrderModule virtuals

		virtual void OnAttacked (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pAttacker, const SDamageCtx &Damage, bool bFriendlyFire) override;
		virtual void OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx) override;
		virtual void OnBehaviorStart (CShip &Ship, CAIBehaviorCtx &Ctx, const COrderDesc &OrderDesc) override;
		virtual DWORD OnCommunicate (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2, ICCItem *pData) override;
		virtual void OnDestroyed (CShip *pShip, SDestroyCtx &Ctx) override;
		virtual CSpaceObject *OnGetBase (void) override { return m_Objs[objBase]; }
		virtual IShipController::OrderTypes OnGetOrder (void) override { return IShipController::orderSentry; }
		virtual CSpaceObject *OnGetTarget (void) override { return m_Objs[objTarget]; }
		virtual void OnObjDestroyed (CShip *pShip, const SDestroyCtx &Ctx, int iObj, bool *retbCancelOrder) override;
		virtual void OnReadFromStream (SLoadCtx &Ctx) override;
		virtual void OnWriteToStream (IWriteStream *pStream) const override;

	private:
		enum Objs
			{
			objBase =		0,
			objTarget =		1,

			objCount =		2,
			};

		void AttackEnemies (CShip *pShip, CAIBehaviorCtx &Ctx);

		int m_iCountdown;						//	Stop after this time

		DWORD m_fIsAttacking:1;
		DWORD m_fSpare2:1;
		DWORD m_fSpare3:1;
		DWORD m_fSpare4:1;
		DWORD m_fSpare5:1;
		DWORD m_fSpare6:1;
		DWORD m_fSpare7:1;
		DWORD m_fSpare8:1;

		DWORD m_dwSpare:24;
	};

class CSimpleOrder : public IOrderModule
	{
	public:
		CSimpleOrder (IShipController::OrderTypes iOrder) : IOrderModule(0),
				m_iOrder(iOrder)
			{ }

	protected:
		//	IOrderModule virtuals
		virtual void OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx) override;
		virtual IShipController::OrderTypes OnGetOrder (void) override { return m_iOrder; }

	private:
		void OrderFireWeapon (CShip *pShip, CAIBehaviorCtx &Ctx) const;
		void OrderResupply (CShip &Ship, CAIBehaviorCtx &Ctx) const;
		void OrderUseItem (CShip *pShip, CAIBehaviorCtx &Ctx) const;

		IShipController::OrderTypes m_iOrder;
	};

class CWaitOrder : public IOrderModule
	{
	public:
		enum EWaitTypes
			{
			waitForUndock,					//	Wait for objTarget to undock
			};

		CWaitOrder (IShipController::OrderTypes iOrder);

	protected:

		//	IOrderModule virtuals

		virtual void OnAttacked (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pAttacker, const SDamageCtx &Damage, bool bFriendlyFire) override;
		virtual void OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx) override;
		virtual void OnBehaviorStart (CShip &Ship, CAIBehaviorCtx &Ctx, const COrderDesc &OrderDesc) override;
		virtual DWORD OnCommunicate (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2, ICCItem *pData) override;
		virtual void OnDestroyed (CShip *pShip, SDestroyCtx &Ctx) override;
		virtual IShipController::OrderTypes OnGetOrder (void) override { return m_iOrder; }
		virtual CSpaceObject *OnGetTarget (void) override { return m_Objs[objTarget]; }
		virtual void OnObjDestroyed (CShip *pShip, const SDestroyCtx &Ctx, int iObj, bool *retbCancelOrder) override;
		virtual void OnReadFromStream (SLoadCtx &Ctx) override;
		virtual void OnWriteToStream (IWriteStream *pStream) const override;

	private:
		enum Objs
			{
			objLeader =		0,
			objTarget =		1,

			objCount =		2,
			};

		void AttackEnemies (CShip *pShip, CAIBehaviorCtx &Ctx, bool bReady);
		bool IsLeaderInRange (CShip *pShip);

		IShipController::OrderTypes m_iOrder;
		int m_iCountdown;						//	Stop after this time
		Metric m_rDistance;						//	Required leader distance

		DWORD m_fWaitUntilLeaderUndocks:1;
		DWORD m_fWaitForLeaderToApproach:1;
		DWORD m_fAttackEnemies:1;
		DWORD m_fWaitForEnemy:1;
		DWORD m_fDeterEnemies:1;
		DWORD m_fIsDeterring:1;
		DWORD m_fWaitForThreat:1;
		DWORD m_fSpare8:1;

		DWORD m_dwSpare:24;
	};

