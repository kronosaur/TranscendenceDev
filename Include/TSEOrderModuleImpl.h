//	TSEOrderModuleImpl.h
//
//	Transcendence IOrderModule classes

#ifndef INCL_TSE_ORDER_MODULES
#define INCL_TSE_ORDER_MODULES

class CApproachOrder : public IOrderModule
	{
	public:
		CApproachOrder (void) : IOrderModule(objCount)
			{ }

	protected:
		//	IOrderModule virtuals
		virtual void OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx);
		virtual void OnBehaviorStart (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pOrderTarget, const IShipController::SData &Data);
		virtual IShipController::OrderTypes OnGetOrder (void) { return IShipController::orderApproach; }
		virtual void OnReadFromStream (SLoadCtx &Ctx);
		virtual void OnWriteToStream (CSystem *pSystem, IWriteStream *pStream);

	private:
		enum Objs
			{
			objDest =		0,
			objTarget =		1,

			objCount =		2,
			};

		enum States
			{
			stateOnCourseViaNavPath,
			stateApproaching,
			};

		States m_iState;						//	Current behavior state
		Metric m_rMinDist2;						//	Minimum distance to target
	};

class CAttackOrder : public IOrderModule
	{
	public:
		CAttackOrder (IShipController::OrderTypes iOrder);

	protected:
		//	IOrderModule virtuals
		virtual bool IsAttacking (void) { return true; }
		virtual bool IsTarget (CSpaceObject *pObj) { return (pObj == m_Objs[objTarget]); }
		virtual bool IsTarget (int iObj) { return (iObj == objTarget); }
		virtual void OnAttacked (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pAttacker, const DamageDesc &Damage, bool bFriendlyFire);
		virtual void OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx);
		virtual void OnBehaviorStart (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pOrderTarget, const IShipController::SData &Data);
		virtual CString OnDebugCrashInfo (void);
		virtual IShipController::OrderTypes OnGetOrder (void) { return m_iOrder; }
		virtual CSpaceObject *OnGetTarget (void) { return m_Objs[objTarget]; }
		virtual void OnObjDestroyed (CShip *pShip, const SDestroyCtx &Ctx, int iObj, bool *retbCancelOrder);
		virtual void OnReadFromStream (SLoadCtx &Ctx);
		virtual void OnWriteToStream (CSystem *pSystem, IWriteStream *pStream);

	private:
		enum Objs
			{
			objTarget =		0,
			objAvoid =		1,

			objCount =		2,
			};

		enum States
			{
			stateAttackingTargetAndAvoiding =		0,
			stateAvoidingEnemyStation =				1,
			stateAttackingTargetAndHolding =		2,
			};

		CSpaceObject *GetBestTarget (CShip *pShip);
		CSpaceObject *GetTargetArea (CShip *pShip, Metric *retrRange);
		bool IsBetterTarget (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pOldTarget, CSpaceObject *pNewTarget);
		bool IsInTargetArea (CShip *pShip, CSpaceObject *pObj);

		IShipController::OrderTypes m_iOrder;
		States m_iState;						//	Current behavior state
		int m_iCountdown;						//	Stop attacking after this time (-1 = no limit)

		DWORD m_fNearestTarget:1;				//	If TRUE, continue attacking other targets
		DWORD m_fInRangeOfObject:1;				//	If TRUE, new target must be in range of order object
		DWORD m_fHold:1;						//	If TRUE, hold position

		DWORD m_fSpare4:1;
		DWORD m_fSpare5:1;
		DWORD m_fSpare6:1;
		DWORD m_fSpare7:1;
		DWORD m_fSpare8:1;
		DWORD m_dwSpare:24;
	};

class CAttackStationOrder : public IOrderModule
	{
	public:
		CAttackStationOrder (void) : IOrderModule(objCount)
			{ }

	protected:
		//	IOrderModule virtuals
		virtual bool IsAttacking (void) { return true; }
		virtual bool IsTarget (CSpaceObject *pObj) { return ((pObj == m_Objs[objTarget]) || (pObj == m_Objs[objDefender])); }
		virtual bool IsTarget (int iObj) { return true; }
		virtual void OnAttacked (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pAttacker, const DamageDesc &Damage, bool bFriendlyFire);
		virtual void OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx);
		virtual void OnBehaviorStart (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pOrderTarget, const IShipController::SData &Data);
		virtual CString OnDebugCrashInfo (void);
		virtual IShipController::OrderTypes OnGetOrder (void) { return IShipController::orderAttackStation; }
		virtual CSpaceObject *OnGetTarget (void) { return m_Objs[objTarget]; }
		virtual void OnObjDestroyed (CShip *pShip, const SDestroyCtx &Ctx, int iObj, bool *retbCancelOrder);
		virtual void OnReadFromStream (SLoadCtx &Ctx);
		virtual void OnWriteToStream (CSystem *pSystem, IWriteStream *pStream);

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

class CFireEventOrder : public IOrderModule
	{
	public:
		CFireEventOrder (void) : IOrderModule(0)
			{ }

	protected:
		//	IOrderModule virtuals
		virtual void OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx);
		virtual IShipController::OrderTypes OnGetOrder (void) { return IShipController::orderFireEvent; }
	};

class CEscortOrder : public IOrderModule
	{
	public:
		CEscortOrder (void) : IOrderModule(objCount)
			{ }

	protected:
		//	IOrderModule virtuals
		virtual void OnAttacked (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pAttacker, const DamageDesc &Damage, bool bFriendlyFire);
		virtual void OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx);
		virtual void OnBehaviorStart (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pOrderTarget, const IShipController::SData &Data);
		virtual DWORD OnCommunicate (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2);
		virtual CSpaceObject *OnGetBase (void);
		virtual IShipController::OrderTypes OnGetOrder (void) { return IShipController::orderEscort; }
		virtual void OnObjDestroyed (CShip *pShip, const SDestroyCtx &Ctx, int iObj, bool *retbCancelOrder);
		virtual void OnReadFromStream (SLoadCtx &Ctx);
		virtual void OnWriteToStream (CSystem *pSystem, IWriteStream *pStream);

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

		States m_iState;
	};

class CGuardOrder : public IOrderModule
	{
	public:
		CGuardOrder (void) : IOrderModule(0)
			{ }

	protected:
		//	IOrderModule virtuals
		virtual void OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx);
		virtual void OnBehaviorStart (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pOrderTarget, const IShipController::SData &Data);
		virtual IShipController::OrderTypes OnGetOrder (void) { return IShipController::orderGuard; }
		virtual void OnReadFromStream (SLoadCtx &Ctx);
		virtual void OnWriteToStream (CSystem *pSystem, IWriteStream *pStream);

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

		virtual void OnAttacked (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pAttacker, const DamageDesc &Damage, bool bFriendlyFire);
		virtual void OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx);
		virtual void OnBehaviorStart (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pOrderTarget, const IShipController::SData &Data);
		virtual CSpaceObject *OnGetBase (void);
		virtual IShipController::OrderTypes OnGetOrder (void) { return IShipController::orderApproach; }
		virtual CSpaceObject *OnGetTarget (void) { return m_Objs[objTarget]; }
		virtual void OnObjDestroyed (CShip *pShip, const SDestroyCtx &Ctx, int iObj, bool *retbCancelOrder);
		virtual void OnReadFromStream (SLoadCtx &Ctx);
		virtual void OnWriteToStream (CSystem *pSystem, IWriteStream *pStream);

	private:
		enum Objs
			{
			objDest =		0,
			objTarget =		1,

			objCount =		2,
			};

		IShipController::OrderTypes m_iOrder;
		CVector m_vDest;						//	Destination
		int m_iDestFacing;						//	Ship should face at this angle
		Metric m_rMinDist2;						//	Minimum distance to target

		DWORD m_fTargetVector:1;				//	Destination is m_vDest
		DWORD m_fTargetObj:1;					//	Destination is objDest
		DWORD m_fIsFollowingNavPath:1;			//	We're following the current nav path
		DWORD m_fDockAtDestination:1;			//	When we reach our destination, dock
		DWORD m_fVariableMinDist:1;				//	If TRUE, order parameter specifies min distance
		DWORD m_fNavPathOnly:1;					//	If TRUE, we're done as soon as the nav path completes
		DWORD m_fGateAtDestination:1;			//	When we reach our destination, gate
		DWORD m_fSpare8:1;

		DWORD m_dwSpare:24;
	};

class CSendMessageOrder : public IOrderModule
	{
	public:
		CSendMessageOrder (void) : IOrderModule(0)
			{ }

	protected:
		//	IOrderModule virtuals
		virtual void OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx);
		virtual IShipController::OrderTypes OnGetOrder (void) { return IShipController::orderSendMessage; }
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

		virtual void OnAttacked (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pAttacker, const DamageDesc &Damage, bool bFriendlyFire);
		virtual void OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx);
		virtual void OnBehaviorStart (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pOrderTarget, const IShipController::SData &Data);
		virtual DWORD OnCommunicate (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2);
		virtual IShipController::OrderTypes OnGetOrder (void) { return m_iOrder; }
		virtual CSpaceObject *OnGetTarget (void) { return m_Objs[objTarget]; }
		virtual void OnObjDestroyed (CShip *pShip, const SDestroyCtx &Ctx, int iObj, bool *retbCancelOrder);
		virtual void OnReadFromStream (SLoadCtx &Ctx);
		virtual void OnWriteToStream (CSystem *pSystem, IWriteStream *pStream);

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
		DWORD m_fSpare7:1;
		DWORD m_fSpare8:1;

		DWORD m_dwSpare:24;
	};

#endif
