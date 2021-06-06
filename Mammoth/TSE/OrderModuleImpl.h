//	OrderModuleImpl.h
//
//	Transcendence IOrderModule classes
//	Copyright (c) 2021 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CReactionImpl
	{
	public:
		CReactionImpl (AIReaction iReactToAttack = AIReaction::None, AIReaction iReactToBaseDestroyed = AIReaction::None, AIReaction iReactToThreat = AIReaction::None) :
				m_iReactToAttack(iReactToAttack),
				m_iReactToBaseDestroyed(iReactToBaseDestroyed),
				m_iReactToThreat(iReactToThreat)
			{ }
				
		CReactionImpl (const IOrderModule &CurrentOrder) :
				m_iReactToAttack(CurrentOrder.GetReactToAttack()),
				m_iReactToBaseDestroyed(CurrentOrder.GetReactToBaseDestroyed()),
				m_iReactToThreat(CurrentOrder.GetReactToThreat()),
				m_rThreatRange(CurrentOrder.GetThreatRange()),
				m_rThreatStopRange(CurrentOrder.GetThreatStopRange())
			{ }

		void Init (const COrderDesc &OrderDesc);

		AIReaction GetReactToAttack () const { return m_iReactToAttack; }
		AIReaction GetReactToBaseDestroyed () const { return m_iReactToBaseDestroyed; }
		AIReaction GetReactToThreat () const { return m_iReactToThreat; }
		Metric GetThreatRange () const { return m_rThreatRange; }
		Metric GetThreatStopRange () const { return m_rThreatStopRange; }
		void SetOptions (ICCItem &Data) const;

	private:
		static AIReaction InitFromOrderDesc (const COrderDesc &OrderDesc, const CString &sField, AIReaction iDefault);

		AIReaction m_iReactToAttack = AIReaction::None;
		AIReaction m_iReactToBaseDestroyed = AIReaction::None;
		AIReaction m_iReactToThreat = AIReaction::None;

		Metric m_rThreatRange = CAISettings::DEFAULT_THREAT_RANGE * LIGHT_SECOND;
		Metric m_rThreatStopRange = CAISettings::DEFAULT_THREAT_RANGE * LIGHT_SECOND;
	};

class CApproachOrder : public IOrderModule
	{
	public:
		CApproachOrder (void) : IOrderModule(objCount),
				m_Reaction(AIReaction::DeterWithSecondaries, AIReaction::None, AIReaction::None)
			{ }

		//	dwFlags:
		//		FLAG_CANCEL_ON_REACTION_ORDER

		static COrderDesc Create (CSpaceObject &Dest, int iDist, const CReactionImpl &Reactions, DWORD dwFlags = 0);

	protected:
		//	IOrderModule virtuals
		virtual void OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx) override;
		virtual void OnBehaviorStart (CShip &Ship, CAIBehaviorCtx &Ctx, const COrderDesc &OrderDesc) override;
		virtual CSpaceObject *OnGetBase (void) override { return (m_bDestIsBase ? m_Objs[objDest] : NULL); }
		virtual IShipController::OrderTypes OnGetOrder (void) override { return IShipController::orderApproach; }
		virtual CSpaceObject *OnGetTarget (void) override { return m_Objs[objTarget]; }
		virtual AIReaction OnGetReactToAttack () const override { return m_Reaction.GetReactToAttack(); }
		virtual AIReaction OnGetReactToBaseDestroyed () const override { return m_Reaction.GetReactToBaseDestroyed(); }
		virtual AIReaction OnGetReactToThreat () const override { return m_Reaction.GetReactToThreat(); }
		virtual Metric OnGetThreatRange (void) const override { return m_Reaction.GetThreatRange(); }
		virtual Metric OnGetThreatStopRange (void) const override { return m_Reaction.GetThreatStopRange(); }
		virtual void OnReadFromStream (SLoadCtx &Ctx, const COrderDesc &OrderDesc) override;
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

		void Init (const COrderDesc &OrderDesc);

		EState m_iState = EState::None;			//	Current behavior state
		Metric m_rMinDist2 = 0.0;				//	Minimum distance to target

		CReactionImpl m_Reaction;

		bool m_bDestIsBase = false;
	};

class CAttackOrder : public IOrderModule
	{
	public:
		CAttackOrder (IShipController::OrderTypes iOrder);

	protected:
		//	IOrderModule virtuals
		virtual bool IsAttacking (void) override { return true; }
		virtual void OnAttacked (CShip &Ship, CAIBehaviorCtx &Ctx, CSpaceObject &AttackerObj, const SDamageCtx &Damage, bool bFriendlyFire) override;
		virtual void OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx) override;
		virtual void OnBehaviorStart (CShip &Ship, CAIBehaviorCtx &Ctx, const COrderDesc &OrderDesc) override;
		virtual CString OnDebugCrashInfo (void) override;
		virtual IShipController::OrderTypes OnGetOrder (void) override { return m_iOrder; }
		virtual CSpaceObject *OnGetTarget (void) override { return m_Objs[OBJ_TARGET]; }
		virtual void OnObjDestroyed (CShip *pShip, const SDestroyCtx &Ctx, int iObj, bool *retbCancelOrder) override;
		virtual void OnReadFromStream (SLoadCtx &Ctx, const COrderDesc &OrderDesc) override;
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
		virtual void OnAttacked (CShip &Ship, CAIBehaviorCtx &Ctx, CSpaceObject &AttackerObj, const SDamageCtx &Damage, bool bFriendlyFire) override;
		virtual void OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx) override;
		virtual void OnBehaviorStart (CShip &Ship, CAIBehaviorCtx &Ctx, const COrderDesc &OrderDesc) override;
		virtual CString OnDebugCrashInfo (void) override;
		virtual IShipController::OrderTypes OnGetOrder (void) override { return IShipController::orderAttackStation; }
		virtual CSpaceObject *OnGetTarget (void) override { return m_Objs[objTarget]; }
		virtual void OnObjDestroyed (CShip *pShip, const SDestroyCtx &Ctx, int iObj, bool *retbCancelOrder) override;
		virtual void OnReadFromStream (SLoadCtx &Ctx, const COrderDesc &OrderDesc) override;
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

		//	dwFlags:
		//		FLAG_CANCEL_ON_REACTION_ORDER

		static COrderDesc Create (CSpaceObject &TargetObj, CSpaceObject *pBase, Metric rMaxRange = 0.0, int iTimer = 0, DWORD dwFlags = 0);

	protected:

		//	IOrderModule virtuals

		virtual void OnAttacked (CShip &Ship, CAIBehaviorCtx &Ctx, CSpaceObject &AttackerObj, const SDamageCtx &Damage, bool bFriendlyFire) override;
		virtual void OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx) override;
		virtual void OnBehaviorStart (CShip &Ship, CAIBehaviorCtx &Ctx, const COrderDesc &OrderDesc) override;
		virtual DWORD OnCommunicate (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2, ICCItem *pData);
		virtual void OnDestroyed (CShip *pShip, SDestroyCtx &Ctx) override;
		virtual CSpaceObject *OnGetBase (void) override { return m_Objs[OBJ_BASE]; }
		virtual IShipController::OrderTypes OnGetOrder (void) override { return IShipController::orderDeterChase; }
		virtual CSpaceObject *OnGetTarget (void) override { return m_Objs[OBJ_TARGET]; }
		virtual void OnObjDestroyed (CShip *pShip, const SDestroyCtx &Ctx, int iObj, bool *retbCancelOrder) override;
		virtual void OnReadFromStream (SLoadCtx &Ctx, const COrderDesc &OrderDesc) override;
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
		virtual void OnAttacked (CShip &Ship, CAIBehaviorCtx &Ctx, CSpaceObject &AttackerObj, const SDamageCtx &Damage, bool bFriendlyFire) override;
		virtual void OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx) override;
		virtual void OnBehaviorStart (CShip &Ship, CAIBehaviorCtx &Ctx, const COrderDesc &OrderDesc) override;
		virtual DWORD OnCommunicate (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2, ICCItem *pData) override;
		virtual CSpaceObject *OnGetBase (void) override;
		virtual IShipController::OrderTypes OnGetOrder (void) override { return IShipController::orderEscort; }
		virtual CSpaceObject *OnGetTarget (void) override { return m_Objs[objTarget]; }
		virtual void OnObjDestroyed (CShip *pShip, const SDestroyCtx &Ctx, int iObj, bool *retbCancelOrder) override;
		virtual void OnReadFromStream (SLoadCtx &Ctx, const COrderDesc &OrderDesc) override;
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
		static constexpr Metric DEFAULT_THREAT_RANGE_LS = 30.0;
		static constexpr Metric DEFAULT_THREAT_STOP_RANGE_LS = 120.0;

		CGuardOrder (void) : IOrderModule(OBJ_COUNT)
			{ }

	protected:
		//	IOrderModule virtuals

		virtual void OnAccumulateAIStatus (const CShip &Ship, const CAIBehaviorCtx &Ctx, ICCItem &Result) const override;
		virtual void OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx) override;
		virtual void OnBehaviorStart (CShip &Ship, CAIBehaviorCtx &Ctx, const COrderDesc &OrderDesc) override;
		virtual void OnDestroyed (CShip *pShip, SDestroyCtx &Ctx) override;
		virtual CSpaceObject *OnGetBase (void) override { return m_Objs[OBJ_BASE]; }
		virtual IShipController::OrderTypes OnGetOrder (void) override { return IShipController::orderGuard; }
		virtual AIReaction OnGetReactToAttack () const override { return AIReaction::ChaseFromBase; }
		virtual AIReaction OnGetReactToBaseDestroyed () const override { return AIReaction::DestroyAndRetaliate; }
		virtual AIReaction OnGetReactToThreat () const override { return AIReaction::ChaseFromBase; }
		virtual Metric OnGetThreatRange (void) const override { return m_rThreatRange; }
		virtual Metric OnGetThreatStopRange (void) const override { return m_rThreatStopRange; }
		virtual DWORD OnGetThreatTargetTypes () const override { return ((DWORD)CTargetList::ETargetType::AggressiveShip | (DWORD)CTargetList::ETargetType::NonAggressiveShip); }
		virtual void OnReadFromStream (SLoadCtx &Ctx, const COrderDesc &OrderDesc) override { Init(OrderDesc); }

	private:
		static constexpr int OBJ_BASE =		0;
		static constexpr int OBJ_COUNT =	1;

		static constexpr int NAV_PATH_THRESHOLD_LS =		60;
		static constexpr Metric NAV_PATH_THRESHOLD =		NAV_PATH_THRESHOLD_LS * LIGHT_SECOND;
		static constexpr Metric NAV_PATH_THRESHOLD2 =		(NAV_PATH_THRESHOLD * NAV_PATH_THRESHOLD);

		void Init (const COrderDesc &OrderDesc);

		Metric m_rThreatRange = 0.0;			//	Range at which we attack threats
		Metric m_rThreatStopRange = 0.0;		//	Range at which we stop chasing threats
	};

class CNavigateOrder : public IOrderModule
	{
	public:
		CNavigateOrder (IShipController::OrderTypes iOrder);

		//	dwFlags:
		//		FLAG_CANCEL_ON_REACTION_ORDER

		static COrderDesc CreateDock (CSpaceObject &Dest, const CReactionImpl &Reactions, DWORD dwFlags = 0);

	protected:

		//	IOrderModule virtuals

		virtual void OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx) override;
		virtual void OnBehaviorStart (CShip &Ship, CAIBehaviorCtx &Ctx, const COrderDesc &OrderDesc) override;
		virtual CSpaceObject *OnGetBase (void) override;
		virtual IShipController::OrderTypes OnGetOrder (void) override { return m_iOrder; }
		virtual CSpaceObject *OnGetTarget (void) override { return m_Objs[objTarget]; }
		virtual AIReaction OnGetReactToAttack () const override { return m_Reaction.GetReactToAttack(); }
		virtual AIReaction OnGetReactToBaseDestroyed () const override { return m_Reaction.GetReactToBaseDestroyed(); }
		virtual AIReaction OnGetReactToThreat () const override { return m_Reaction.GetReactToThreat(); }
		virtual Metric OnGetThreatRange (void) const override { return m_Reaction.GetThreatRange(); }
		virtual Metric OnGetThreatStopRange (void) const override { return m_Reaction.GetThreatStopRange(); }
		virtual void OnObjDestroyed (CShip *pShip, const SDestroyCtx &Ctx, int iObj, bool *retbCancelOrder) override;
		virtual void OnReadFromStream (SLoadCtx &Ctx, const COrderDesc &OrderDesc) override;
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

		CReactionImpl m_Reaction;

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
		static constexpr Metric DEFAULT_THREAT_RANGE_LS = 30.0;
		static constexpr Metric DEFAULT_THREAT_STOP_RANGE_LS = 120.0;

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
		virtual Metric OnGetThreatRange (void) const override { return m_rThreatRange; }
		virtual Metric OnGetThreatStopRange (void) const override;
		virtual void OnObjDestroyed (CShip *pShip, const SDestroyCtx &Ctx, int iObj, bool *retbCancelOrder) override;
		virtual void OnReadFromStream (SLoadCtx &Ctx, const COrderDesc &OrderDesc) override;
		virtual void OnWriteToStream (IWriteStream *pStream) const override;

	private:
		static constexpr int OBJ_BASE =		0;
		static constexpr int OBJ_TARGET =	1;
		static constexpr int OBJ_COUNT =	2;

		static constexpr Metric NAV_PATH_THRESHOLD =		(50.0 * LIGHT_SECOND);
		static constexpr Metric NAV_PATH_THRESHOLD2 =		(NAV_PATH_THRESHOLD * NAV_PATH_THRESHOLD);

		static constexpr int RESUPPLY_CHECK_TIME =			151;

		void CalcIntermediates (const COrderDesc &OrderDesc);

		IShipController::OrderTypes m_iOrder = IShipController::orderNone;
		COrbit m_Orbit;							//	Orbit definition
		DWORD m_dwStartTick = 0;				//	Tick at start angle
		Metric m_rAngularSpeed = DEFAULT_SPEED;	//	Orbit speed (degrees per tick)
		int m_iCountdown = 0;					//	Stop after this time.

		Metric m_rThreatRange = 0.0;			//	Range at which we attack threats
		Metric m_rThreatStopRange = 0.0;		//	Range at which we stop chasing threats

		Metric m_rNavThreshold2 = 0.0;			//	If further that this from center, nav back
	};

class CPatrolOrder : public IOrderModule
	{
	public:
		static constexpr int DEFAULT_RADIUS_LS = 10;
		static constexpr Metric DEFAULT_THREAT_RANGE_LS = 30.0;
		static constexpr Metric DEFAULT_THREAT_STOP_RANGE_LS = 120.0;

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
		virtual AIReaction OnGetReactToAttack () const override { return AIReaction::ChaseFromBase; }
		virtual AIReaction OnGetReactToBaseDestroyed () const override { return AIReaction::DestroyAndRetaliate; }
		virtual AIReaction OnGetReactToThreat () const override { return AIReaction::ChaseFromBase; }
		virtual void OnReadFromStream (SLoadCtx &Ctx, const COrderDesc &OrderDesc) override;
		virtual Metric OnGetThreatRange (void) const override { return m_rThreatRange; }
		virtual Metric OnGetThreatStopRange (void) const override { return Max(m_rPatrolRadius + m_rThreatRange, m_rThreatStopRange); }
		virtual DWORD OnGetThreatTargetTypes () const override { return ((DWORD)CTargetList::ETargetType::AggressiveShip | (DWORD)CTargetList::ETargetType::NonAggressiveShip); }
		virtual void OnWriteToStream (IWriteStream *pStream) const override;

	private:
		static constexpr int OBJ_BASE =		0;
		static constexpr int OBJ_TARGET =	1;
		static constexpr int OBJ_COUNT =	2;

		static constexpr Metric NAV_PATH_THRESHOLD =		60.0 * LIGHT_SECOND;
		static constexpr Metric NAV_PATH_THRESHOLD2 =		(NAV_PATH_THRESHOLD * NAV_PATH_THRESHOLD);

		static constexpr int RESUPPLY_CHECK_TIME =			151;

		void CalcIntermediates (const COrderDesc &OrderDesc);

		Metric m_rPatrolRadius = 0.0;

		Metric m_rThreatRange = 0.0;			//	Range at which we attack threats
		Metric m_rThreatStopRange = 0.0;		//	Range at which we stop chasing threats

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

		virtual void OnAttacked (CShip &Ship, CAIBehaviorCtx &Ctx, CSpaceObject &AttackerObj, const SDamageCtx &Damage, bool bFriendlyFire) override;
		virtual void OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx) override;
		virtual void OnBehaviorStart (CShip &Ship, CAIBehaviorCtx &Ctx, const COrderDesc &OrderDesc) override;
		virtual DWORD OnCommunicate (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2, ICCItem *pData) override;
		virtual void OnDestroyed (CShip *pShip, SDestroyCtx &Ctx) override;
		virtual CSpaceObject *OnGetBase (void) override { return m_Objs[objBase]; }
		virtual IShipController::OrderTypes OnGetOrder (void) override { return IShipController::orderSentry; }
		virtual CSpaceObject *OnGetTarget (void) override { return m_Objs[objTarget]; }
		virtual void OnObjDestroyed (CShip *pShip, const SDestroyCtx &Ctx, int iObj, bool *retbCancelOrder) override;
		virtual void OnReadFromStream (SLoadCtx &Ctx, const COrderDesc &OrderDesc) override;
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

		virtual void OnAttacked (CShip &Ship, CAIBehaviorCtx &Ctx, CSpaceObject &AttackerObj, const SDamageCtx &Damage, bool bFriendlyFire) override;
		virtual void OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx) override;
		virtual void OnBehaviorStart (CShip &Ship, CAIBehaviorCtx &Ctx, const COrderDesc &OrderDesc) override;
		virtual DWORD OnCommunicate (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2, ICCItem *pData) override;
		virtual void OnDestroyed (CShip *pShip, SDestroyCtx &Ctx) override;
		virtual IShipController::OrderTypes OnGetOrder (void) override { return m_iOrder; }
		virtual CSpaceObject *OnGetTarget (void) override { return m_Objs[objTarget]; }
		virtual void OnObjDestroyed (CShip *pShip, const SDestroyCtx &Ctx, int iObj, bool *retbCancelOrder) override;
		virtual void OnReadFromStream (SLoadCtx &Ctx, const COrderDesc &OrderDesc) override;
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

