//	CBaseShipAI.cpp
//
//	CBaseShipAI class
//	Copyright (c) 2021 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

constexpr int MAX_TARGETS =				10;

#ifdef DEBUG_COMBAT
#define DEBUG_COMBAT_OUTPUT(x)			if (m_pShip->IsSelected()) m_pShip->GetUniverse().DebugOutput("%d> %s", g_iDebugLine++, x)
#else
#define DEBUG_COMBAT_OUTPUT(x)
#endif

extern int g_iDebugLine;

CBaseShipAI::~CBaseShipAI (void)

//	CBaseShipAI destructor

	{
	if (m_pCommandCode)
		m_pCommandCode->Discard();

	if (m_pOrderModule)
		delete m_pOrderModule;
	}

void CBaseShipAI::AddOrder (const COrderDesc &OrderDesc, bool bAddBefore)

//	AddOrder
//
//	Add an order to the list

	{
	CSpaceObject *pTarget = OrderDesc.GetTarget();
	ASSERT(!pTarget || pTarget->NotifyOthersWhenDestroyed());

	//	If the order requires a target, make sure we have one

	bool bRequired;
	if (pTarget == NULL && OrderHasTarget(OrderDesc.GetOrder(), &bRequired) && bRequired)
		{
		ASSERT(false);
		return;
		}

	//	If we're escorting someone, make sure that we are not already
	//	in the escort chain

	if (OrderDesc.GetOrder() == orderEscort || OrderDesc.GetOrder() == orderFollow)
		{
		CSpaceObject *pLeader = pTarget;
		int iLoops = 20;
		while (pLeader && pLeader != m_pShip && iLoops-- > 0)
			{
			CShip *pShip = pLeader->AsShip();
			if (pShip)
				{
				auto &ShipOrderDesc = pShip->GetCurrentOrderDesc();
				if (ShipOrderDesc.GetOrder() == orderEscort || ShipOrderDesc.GetOrder() == orderFollow)
					pLeader = ShipOrderDesc.GetTarget();
				else
					pLeader = NULL;
				}
			else
				pLeader = NULL;
			}

		//	We are in the chain

		if (pLeader == m_pShip)
			return;
		}

	//	Add the order

	bool bChanged = (bAddBefore || m_Orders.GetCount() == 0);
	m_Orders.Insert(OrderDesc, bAddBefore);
	if (bChanged)
		FireOnOrderChanged();
	}

AIReaction CBaseShipAI::AdjReaction (AIReaction iReaction) const

//	AdjReaction
//
//	Adjust reaction to account for setting of the ship.

	{
	switch (iReaction)
		{
		case AIReaction::Chase:
		case AIReaction::Destroy:
		case AIReaction::DestroyAndRetaliate:
			if (m_AICtx.IsNonCombatant())
				return AIReaction::None;
			else if (m_AICtx.IsImmobile())
				return AIReaction::Deter;
			else
				return iReaction;

		case AIReaction::Deter:
		case AIReaction::DeterWithSecondaries:
			if (m_AICtx.IsNonCombatant())
				return AIReaction::None;
			else
				return iReaction;

		case AIReaction::Gate:
			if (m_AICtx.IsImmobile())
				return AIReaction::None;
			else
				return iReaction;

		default:
			return AIReaction::None;
		}
	}

void CBaseShipAI::Behavior (SUpdateCtx &Ctx)

//	Behavior
//
//	Fly, fight, die.
//
//	NOTE: Most custom controllers override this. Only the standard controller 
//	uses this. [And eventually, once we use order modules for everything,
//	we should be able to delete the standard controller.]

	{
	DEBUG_TRY

	//	Reset

	m_AICtx.SetSystemUpdateCtx(&Ctx);
	ResetBehavior();

	//	Use basic items

	UseItemsBehavior();

	//	If we don't have an order then we need to come up with something.

	IShipController::OrderTypes iOrder = GetCurrentOrder();
	if (iOrder == IShipController::orderNone)
		{
		//	If we've got command code, execute it now

		if (m_pCommandCode)
			{
			CCodeChainCtx Ctx(GetUniverse());
			Ctx.DefineContainingType(m_pShip);
			Ctx.SaveAndDefineSourceVar(m_pShip);

			ICCItem *pResult = Ctx.RunLambda(m_pCommandCode);
			if (pResult->IsError())
				m_pShip->ReportEventError(CONSTLIT("CommandCode"), pResult);
			Ctx.Discard(pResult);
			}

		//	If we still got no orders, let the ship class deal with this

		if ((iOrder = GetCurrentOrder()) == IShipController::orderNone)
			FireOnOrdersCompleted();

		//	If we've still got no orders, we need to come up with something

		if ((iOrder = GetCurrentOrder()) == IShipController::orderNone)
			{
			iOrder = ((m_pShip->GetDockedObj() == NULL) ? IShipController::orderGate : IShipController::orderWait);
			AddOrder(COrderDesc(iOrder));
			}
		}

	ASSERT(iOrder != IShipController::orderNone);

	//	If we're using old style behavior, then just call our derived class to
	//	handle it.

	if (m_fOldStyleBehaviors)
		OnBehavior(Ctx);

	//	Otherwise, if we have an order module, then let it handle behavior

	else if (m_pOrderModule)
		m_pOrderModule->Behavior(m_pShip, m_AICtx);

	//	Otherwise, we need to initialize the order module.
	//	NOTE: This should never happen because we initialize the order module
	//	when we add or delete orders.

	else
		{
		if (InitOrderModule())
			m_pOrderModule->Behavior(m_pShip, m_AICtx);

		else if (m_fOldStyleBehaviors)
			OnBehavior(Ctx);
		}

	//	Update reactions (such as deterring target).

	UpdateReactions(Ctx);

	//	Done

	m_AICtx.SetSystemUpdateCtx(NULL);

	DEBUG_CATCH
	}

Metric CBaseShipAI::CalcThreatRange () const

//	CalcThreatRange
//
//	Computes the range at which we stop chasing threats.

	{
	if (m_pOrderModule)
		return Max(m_pOrderModule->GetThreatRange(), m_AICtx.GetAISettings().GetThreatRange());
	else
		return m_AICtx.GetAISettings().GetThreatRange();
	}

bool CBaseShipAI::InitOrderModule (void)

//	InitOrderModule
//
//	Initializes the order module based on the order. We return TRUE if we are
//	using an order module; FALSE otherwise.

	{
	IShipController::OrderTypes iOrder = GetCurrentOrder();

	//	Reset the old style flag because we don't know if the next order
	//	will have an order module or not.

	m_fOldStyleBehaviors = false;

	//	If we've got an existing order module, then we need to either delete it
	//	or re-initialize it.

	if (m_pOrderModule)
		{
		//	If the current order module handles the current order, then we don't have
		//	to reallocate everything; we just need to restart it.

		if (m_pOrderModule->GetOrder() == iOrder)
			{
			m_pOrderModule->BehaviorStart(*m_pShip, m_AICtx, GetCurrentOrderDesc());

			//	NOTE: We cannot make any assumptions after calling BehaviorStart
			//	because we might have added a new order, and m_pOrderModule 
			//	might point to a different order or by NULL.

			return (m_pOrderModule != NULL);
			}

		//	Otherwise, we delete the order module and allow it to be recreated.

		else
			{
			delete m_pOrderModule;
			m_pOrderModule = NULL;
			}
		}

	//	See if we need to create an order module

	if (m_pOrderModule == NULL && iOrder != IShipController::orderNone)
		{
		m_pOrderModule = IOrderModule::Create(iOrder);
		
		//	If there is no module for this order then we need to do the old-
		//	style behavior.

		if (m_pOrderModule == NULL)
			m_fOldStyleBehaviors = true;

		else
			{
			//	Tell our descendants to clean up. We need to do this because we don't
			//	want our descendants to hold on to object pointers (since the order
			//	module will handle everything, including getting destroyed object
			//	notifications).

			OnCleanUp();

			//	Initialize order module

			m_pOrderModule->BehaviorStart(*m_pShip, m_AICtx, GetCurrentOrderDesc());
			}
		}

	//	NOTE: We might have cancelled the order inside BehaviorStart, so we
	//	return FALSE in that case.

	return (m_pOrderModule != NULL);
	}

Metric CBaseShipAI::CalcShipIntercept (const CVector &vRelPos, const CVector &vAbsVel, Metric rMaxSpeed)

//	CalcShipIntercept
//
//	Calculates (heuristically) the time that it will take us
//	to intercept at target at vRelPos (relative to ship) and moving
//	at vAbsVel (in absolute terms). If we return < 0.0 then we
//	cannot intercept even at maximum velocity.

	{
	CVector vInterceptPoint;

	//	Compute the speed with which the target is closing
	//	the distance (this may be a negative number). Note
	//	that we care about the target's absolute velocity
	//	(not its relative velocity because we are trying to
	//	adjust our velocity).

	Metric rClosingSpeed = -vAbsVel.Dot(vRelPos.Normal());

	//	Figure out how long it will take to overtake the target's
	//	current position at maximum speed. (This is just a heuristic
	//	that gives us an approximation of the length of an intercept
	//	course.)

	rClosingSpeed = rClosingSpeed + rMaxSpeed;
	if (rClosingSpeed > 0.0)
		return vRelPos.Length() / (rClosingSpeed);
	else
		return -1.0;
	}

void CBaseShipAI::CancelAllOrders (void)

//	CancelAllOrders
//
//	Cancel orders

	{
	m_AICtx.ClearNavPath();
	m_Orders.DeleteAll();
	m_DeterModule.Cancel();
	FireOnOrderChanged();
	}

void CBaseShipAI::CancelCurrentOrder (void)

//	CancelCurrentOrder
//
//	Cancels the current order

	{
	if (m_Orders.GetCount() > 0)
		{
		m_Orders.DeleteCurrent();
		m_DeterModule.Cancel();
		FireOnOrderChanged();
		}
	}

void CBaseShipAI::CancelDocking (CSpaceObject *pTarget)

//	CancelDocking
//
//	Cancel docking with the given station

	{
	m_AICtx.CancelDocking(m_pShip, pTarget);
	}

bool CBaseShipAI::CancelOrder (int iIndex)

//	CancelOrder
//
//	Cancels the current order by index and returns TRUE if we were able to 
//	cancel it.

	{
	if (m_Orders.GetCount() == 0)
		return false;
	else if (iIndex == 0)
		{
		CancelCurrentOrder();
		return true;
		}
	else if (iIndex < m_Orders.GetCount())
		{
		m_Orders.Delete(iIndex);
		return true;
		}
	else
		return false;
	}

bool CBaseShipAI::CanObjRequestDock (CSpaceObject *pObj) const

//	CanObjRequestDock
//
//	Returns TRUE if another object can request to dock with us (assuming we have
//	docking ports, etc.).

	{
	switch (GetCurrentOrder())
		{
		//	Certain orders keep us immobile, so we can allow others to dock with
		//	us.

		case orderDestroyTargetHold:
		case orderGateOnThreat:
		case orderGateOnStationDestroyed:
		case orderHold:
		case orderHoldAndAttack:
		case orderHoldCourse:
		case orderSentry:
		case orderWait:
		case orderWaitForEnemy:
		case orderWaitForPlayer:
		case orderWaitForTarget:
		case orderWaitForThreat:
		case orderWaitForUndock:
			return true;

		//	If we're escorting or following a principal and we're a non-
		//	combatant, then we allow docking.

		case orderEscort:
		case orderFollow:
			return (pObj && pObj == GetCurrentOrderTarget() && m_AICtx.IsNonCombatant());

		//	Otherwise, if we're immobile, then it doesn't matter what our orders.
		//	But if we're mobile, then we're maneuvering, so we can't allow 
		//	docking.

		default:
			return IsImmobile();
		}
	}

bool CBaseShipAI::CheckForEnemiesInRange (CSpaceObject *pCenter, Metric rRange, int iInterval, CSpaceObject **retpTarget)

//	CheckForEnemiesInRange
//
//	Checks every given interval to see if there are enemy ships in range

	{
	if (m_pShip->IsDestinyTime(iInterval))
		{
		*retpTarget = m_pShip->GetVisibleEnemyInRange(pCenter, rRange);

		if (*retpTarget)
			return true;
		else
			return false;
		}
	else
		return false;
	}

bool CBaseShipAI::CheckOutOfRange (CSpaceObject *pTarget, Metric rRange, int iInterval)

//	CheckOutOfRange
//
//	Checks to see if the ship is out of range. Returns TRUE if we are known to be
//	out of range.

	{
	if (m_pShip->IsDestinyTime(iInterval))
		{
		CVector vRange = pTarget->GetPos() - m_pShip->GetPos();
		Metric rDistance2 = vRange.Length2();

		return (rDistance2 > (rRange * rRange));
		}
	else
		return false;
	}

bool CBaseShipAI::CheckOutOfZone (CSpaceObject *pBase, Metric rInnerRadius, Metric rOuterRadius, int iInterval)

//	CheckOutOfZone
//
//	Checks to see if the ship is out of its zone.

	{
	if (m_pShip->IsDestinyTime(iInterval))
		{
		Metric rMaxRange2 = rOuterRadius * rOuterRadius;
		Metric rMinRange2 = rInnerRadius * rInnerRadius;

		CVector vRange = pBase->GetPos() - m_pShip->GetPos();
		Metric rDistance2 = vRange.Length2();

		return (rDistance2 > rMaxRange2 || rDistance2 < rMinRange2);
		}
	else
		return false;
	}

CString CBaseShipAI::DebugCrashInfo (void)

//	DebugCrashInfo
//
//	Returns debug crash info

	{
	//	If we have an order module then let it dump its data.

	if (m_pOrderModule)
		{
		CString sResult = OnDebugCrashInfo();
		sResult.Append(m_pOrderModule->DebugCrashInfo(m_pShip));
		sResult.Append(strPatternSubst(CONSTLIT("m_pNavPath: %s\r\n"), CNavigationPath::DebugDescribe(m_pShip, m_AICtx.GetNavPath())));
		return sResult;
		}

	//	Otherwise, let our descendant output

	else
		return OnDebugCrashInfo();
	}

void CBaseShipAI::DebugPaintInfo (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	DebugPaintInfo
//
//	Paint debug information

	{
	m_AICtx.DebugPaintInfo(*m_pShip->GetSystem(), Dest, x, y, Ctx);
	}

void CBaseShipAI::FireOnOrderChanged (void)

//	FireOnOrderChanged
//
//	Ship's current order has changed

	{
	DEBUG_TRY

	if (m_pShip->HasOnOrderChangedEvent() && !m_fInOnOrderChanged)
		{
		m_fInOnOrderChanged = true;
		m_pShip->FireOnOrderChanged();
		m_fInOnOrderChanged = false;
		}

	//	Reset some internal variables

	m_AICtx.OnOrderChanged(*m_pShip);

	//	Initialize the order module. We can't call this for descendants (such as
	//	CZoanthropeAI that do not use order modules).

	if (m_fUseOrderModules)
		InitOrderModule();

	//	Give descendants a chance

	OnOrderChanged();

	DEBUG_CATCH
	}

void CBaseShipAI::FireOnOrdersCompleted (void)

//	FireOnOrdersCompleted
//
//	Ship has no more orders

	{
	if (m_pShip->HasOnOrdersCompletedEvent() && !m_fInOnOrdersCompleted)
		{
		m_fInOnOrdersCompleted = true;
		m_pShip->FireOnOrdersCompleted();
		m_fInOnOrdersCompleted = false;
		}
	}

bool CBaseShipAI::FollowsObjThroughGate (CSpaceObject *pLeader)

//	FollowsObjThroughGate
//
//	Returns true if we will follow the leader through a gate

	{
	//	If the leader is the player, then we see if we're following

	if (pLeader == NULL || pLeader->IsPlayer())
		return (GetCurrentOrder() == IShipController::orderFollowPlayerThroughGate);

	//	Otherwise, not

	else
		return false;
	}

int CBaseShipAI::GetAISettingInteger (const CString &sSetting)

//	GetAISettingInteger
//
//	Returns the given value
	
	{
	//	First ask our subclass

	int iValue;
	if (OnGetAISettingInteger(sSetting, &iValue))
		return iValue;

	//	Basic settings

	return strToInt(m_AICtx.GetAISetting(sSetting), 0); 
	}

CString CBaseShipAI::GetAISettingString (const CString &sSetting)

//	GetAISettingString
//
//	Returns the given setting
	
	{
	//	First ask our subclass

	CString sValue;
	if (OnGetAISettingString(sSetting, &sValue))
		return sValue;

	//	We convert integer values to strings (because CCExtensions.cpp expects 
	//	this). In the future CCExtensions should call a new function that
	//	returns an ICCItem

	int iValue;
	if (OnGetAISettingInteger(sSetting, &iValue))
		return strFromInt(iValue);

	//	Basic settings

	return m_AICtx.GetAISetting(sSetting); 
	}

CSpaceObject *CBaseShipAI::GetBase (void) const

//	GetBase
//
//	Returns this ship's base

	{
	if (m_pOrderModule)
		return m_pOrderModule->GetBase();
	else
		{
		switch (GetCurrentOrder())
			{
			//	There are some edge conditions in which we don't have an order
			//	module, but no longer handle the orders in the base class
			//	(this mostly happens when upgrading save file versions).
			//	In that case, we handle it here.

			case IShipController::orderDock:
			case IShipController::orderPatrol:
				return GetCurrentOrderTarget();

			default:
				return OnGetBase();
			}
		}
	}

int CBaseShipAI::GetCombatPower (void)

//	GetCombatPower
//
//	Returns the relative strength of this object in combat
//	Scale from 0 to 100 (Note: This works for ships up to
//	level XV).

	{
	int iPower = m_pShip->GetScore() / 100;
	if (iPower > 100)
		return 100;
	else if (iPower == 0)
		return 1;
	else
		return iPower;
	}

CSpaceObject *CBaseShipAI::GetEscortPrincipal (void) const

//	GetEscortPrincipal
//
//	Returns the ship that is being escorted (or NULL)

	{
	if (m_fIsPlayerWingman)
		return m_pShip->GetUniverse().GetPlayerShip();

	switch (GetCurrentOrder())
		{
		case IShipController::orderEscort:
		case IShipController::orderFollow:
		case IShipController::orderFollowPlayerThroughGate:
			return GetCurrentOrderTarget();

		default:
			return NULL;
		}
	}

CSpaceObject *CBaseShipAI::GetOrderGiver (void)

//	GetOrderGiver
//
//	Returns the ship that orders this one (if the ship is
//	independent, then we return m_pShip)

	{
	//	If there is no order giver then we are responsible

	if (m_AICtx.NoOrderGiver())
		return m_pShip;

	//	If we're the player's wingman, then the player is
	//	the order-giver.

	if (m_fIsPlayerWingman)
		return GetPlayerOrderGiver();

	//	Figure it out based on the order

	switch (GetCurrentOrder())
		{
		case IShipController::orderEscort:
			return GetCurrentOrderTarget();

		case IShipController::orderFollowPlayerThroughGate:
		case IShipController::orderWaitForPlayer:
			return GetPlayerOrderGiver();

		default:
			return m_pShip;
		}
	}

CSpaceObject *CBaseShipAI::GetPlayerOrderGiver (void) const

//	GetPlayerOrderGiver
//
//	If the player is in the system, returns the player; otherwise
//	we return the ship.

	{
	CSpaceObject *pPlayer = m_pShip->GetPlayerShip();
	if (pPlayer)
		return pPlayer;
	else
		return m_pShip;
	}

AIReaction CBaseShipAI::GetReactToAttack () const

//	GetReactToAttack
//
//	Returns the current setting for reacting to an attack.

	{
	//	If our current order does not support reactions, then we don't do 
	//	anything. This can happen if (e.g.,) the current order is itself a 
	//	reaction (such as CDeterChaseOrder).

	if (!m_pOrderModule || !m_pOrderModule->SupportsReactions())
		return AIReaction::None;

	//	Otherwise, get reaction from settings.

	else
		{
		AIReaction iReaction = m_AICtx.GetAISettings().GetReactToAttack();
		if (iReaction == AIReaction::Default)
			iReaction = m_pOrderModule->GetReactToAttack();

		return AdjReaction(iReaction);
		}
	}

AIReaction CBaseShipAI::GetReactToBaseDestroyed () const

//	GetReactToBaseDestroyed
//
//	Returns the current setting for reacting to our base being destroyed.

	{
	//	If our current order does not support reactions, then we don't do 
	//	anything. This can happen if (e.g.,) the current order is itself a 
	//	reaction (such as CDeterChaseOrder).

	if (!m_pOrderModule || !m_pOrderModule->SupportsReactions())
		return AIReaction::None;

	//	Otherwise, get reaction from settings.

	else
		{
		AIReaction iReaction = m_pOrderModule->GetReactToBaseDestroyed();
		return AdjReaction(iReaction);
		}
	}

AIReaction CBaseShipAI::GetReactToThreat () const

//	GetReactToThreat
//
//	Returns the current setting for reacting to a threat.

	{
	//	If our current order does not support reactions, then we don't do 
	//	anything. This can happen if (e.g.,) the current order is itself a 
	//	reaction (such as CDeterChaseOrder).

	if (!m_pOrderModule || !m_pOrderModule->SupportsReactions())
		return AIReaction::None;

	//	Otherwise, get reaction from settings.

	else
		{
		AIReaction iReaction = m_AICtx.GetAISettings().GetReactToThreat();
		if (iReaction == AIReaction::Default)
			iReaction = m_pOrderModule->GetReactToThreat();

		return AdjReaction(iReaction);
		}
	}

CSpaceObject *CBaseShipAI::GetTarget (const CDeviceItem *pDeviceItem, DWORD dwFlags) const

//	GetTarget
//
//	Returns the target that this ship is attacking
	
	{
	if (m_DeterModule.IsEnabled())
		return m_DeterModule.GetTarget();
	else if (m_pOrderModule)
		return m_pOrderModule->GetTarget();
	else
		return OnGetTarget(dwFlags);
	}

CTargetList CBaseShipAI::GetTargetList (void) const

//	GetTargetList
//
//	Returns an initialized (though not yet realized) target list, suitable for
//	weapons to find appropriate targets.

	{
	CTargetList::STargetOptions Options;
	
	//	Range

	Options.rMaxDist = Max(m_AICtx.GetBestWeaponRange(), m_AICtx.GetAISettings().GetThreatRange());

	//	Include our target

	Options.bIncludeSourceTarget = true;

	//	If we are aggressive, then include ships that haven't fired 
	//	their weapons recently

	DWORD dwTypes = GetThreatTargetTypes();
	if (dwTypes & (DWORD)CTargetList::ETargetType::NonAggressiveShip)
		Options.bIncludeNonAggressors = true;

	//	Include the player if they are blacklisted

	if (m_fPlayerBlacklisted)
		Options.bIncludePlayer = true;

	//	Always exclude our base (even if it is an enemy)

	Options.pExcludeObj = GetBase();

	//	Include missiles if we have anti-missile defense

	if (m_AICtx.ShootsAllMissiles())
		Options.bIncludeMissiles = true;
	else if (m_AICtx.ShootsTargetableMissiles())
		Options.bIncludeTargetableMissiles = true;

	//	Done

	return CTargetList(*m_pShip, Options);
	}

DWORD CBaseShipAI::GetThreatTargetTypes () const

//	GetThreatTargetTypes
//
//	Returns the set of targets that we consider threatening.

	{
	//	See if the order module defines this.

	DWORD dwTypes;
	if (m_pOrderModule
			&& (dwTypes = m_pOrderModule->GetThreatTargetTypes()))
		{
		return dwTypes;
		}

	//	Otherwise, default threat

	else
		{
		DWORD dwTypes = (DWORD)CTargetList::ETargetType::AggressiveShip;
		if (m_AICtx.IsAggressor())
			dwTypes |= (DWORD)CTargetList::ETargetType::NonAggressiveShip;

		return dwTypes;
		}
	}

void CBaseShipAI::HandleFriendlyFire (CSpaceObject *pAttacker, CSpaceObject *pOrderGiver)

//	HandleFriendlyFire
//
//	Ship has been hit by friendly fire

	{
	//	If an NPC attacked us, then we don't count it as a deliberate attack
	//	unless they were targeting us.

	if (!pAttacker->IsPlayer() 
			&& pAttacker->GetTarget() != m_pShip)
		NULL;

	//	If the player hit us (and it seems to be on purpose) then raise an event

	else if (pOrderGiver->IsPlayer() 
			&& m_Blacklist.Hit(m_pShip->GetSystem()->GetTick())
			&& m_pShip->HasOnAttackedByPlayerEvent())
		m_pShip->FireOnAttackedByPlayer();

	//	Otherwise, send the standard message

	else
		m_pShip->Communicate(pOrderGiver, msgWatchTargets);
	}

bool CBaseShipAI::IsAngryAt (const CSpaceObject *pObj) const

//	IsAngryAt
//
//	Returns TRUE if we're angry at the given object

	{
	//	We're always angry at enemies

	if (m_pShip->IsEnemy(pObj))
		return true;

	//	If we're attacking the object, then we're angry at it.

	if (GetTarget() == pObj)
		return true;

	//	Are we angry at the player?

	if (m_fPlayerBlacklisted && pObj->IsPlayer())
		return true;

	//	If our base is angry at the object, then we're angry too

	switch (GetCurrentOrder())
		{
		case IShipController::orderGuard:
		case IShipController::orderPatrol:
		case IShipController::orderOrbitExact:
		case IShipController::orderOrbitPatrol:
		case IShipController::orderSentry:
		case IShipController::orderDeterChase:
			{
			CSpaceObject *pBase = GetBase();

			return (pBase && pBase->IsAngryAt(pObj));
			}

		default:
			return false;
		}
	}

bool CBaseShipAI::IsPlayerOrPlayerFollower (CSpaceObject *pObj, int iRecursions)

//	IsPlayerOrPlayerFollower
//
//	Returns TRUE if pObj is the player or some other object that
//	follows the player through gates

	{
	if (pObj->IsPlayer())
		return true;

	//	If we've recursed too much, then we are not the player. This can
	//	happen if there is a bug that makes two ships follow each other

	if (iRecursions > 20)
		return false;

	//	Get the orders for this ship

	CShip *pShip = pObj->AsShip();
	if (pShip)
		{
		auto &OrderDesc = pShip->GetCurrentOrderDesc();
		switch (OrderDesc.GetOrder())
			{
			case IShipController::orderFollowPlayerThroughGate:
				return true;

			case IShipController::orderEscort:
			case IShipController::orderFollow:
				return IsPlayerOrPlayerFollower(OrderDesc.GetTarget(), iRecursions + 1);
			}
		}

	return false;
	}

void CBaseShipAI::OnAttacked (CSpaceObject *pAttacker, const SDamageCtx &Damage)

//	OnAttacked
//
//	Our ship was attacked. Note that pAttacker can either be an enemy ship/station
//	or it can be the missile/beam that hit us.

	{
	DEBUG_TRY

	bool bFriendlyFire = false;

	if (pAttacker)
		{
		CSpaceObject *pOrderGiver = Damage.GetOrderGiver();

		//	If we were attacked by a friend, then warn them off
		//	(Unless we're explicitly targeting the friend)

		if (pOrderGiver 
				&& m_pShip->IsFriend(pOrderGiver) 
				&& !m_pShip->IsAngryAt(pOrderGiver))
			{
			//	We deal with the order giver instead of the attacker because we want to get
			//	at the root problem (the player instead of her autons)
			//
			//	Also, we ignore damage from automated weapons
			//	[LATER: If we could pass CDamageSource instead of pAttacker, we could
			//	use the source to figure out if this is an automated weapon.]

			if (!Damage.Damage.IsAutomatedWeapon())
				HandleFriendlyFire(pAttacker, pOrderGiver);

			bFriendlyFire = true;
			}

		//	Otherwise, react to an attack

		else
			ReactToAttack(*pAttacker, Damage);
		}

	//	Notify our order module (or derived class if we're doing it old-style)

	if (m_pOrderModule)
		m_pOrderModule->Attacked(m_pShip, m_AICtx, pAttacker, Damage, bFriendlyFire);
	else
		OnAttackedNotify(pAttacker, Damage);

	//	Remember the last time we were attacked (debounce quick hits)

	m_AICtx.SetLastAttack(m_pShip->GetUniverse().GetTicks());

	DEBUG_CATCH
	}

DWORD CBaseShipAI::OnCommunicate (CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2, ICCItem *pData)

//	Communicate
//
//	Handle communications from other objects

	{
	if (!m_pOrderModule)
		return OnCommunicateNotify(pSender, iMessage, pParam1, dwParam2, pData);

	//	Some messages we handle ourselves

	switch (iMessage)
		{
		case msgAttack:
		case msgAttackDeter:
			{
			if (m_pOrderModule->SupportsReactions() && pParam1)
				{
				if (ReactToDeterMessage(*pParam1))
					return resAck;
				else
					return resNoAnswer;
				}
			else
				return m_pOrderModule->Communicate(m_pShip, m_AICtx, pSender, iMessage, pParam1, dwParam2, pData);
			}

		case msgDestroyBroadcast:
		case msgBaseDestroyedByTarget:
			{
			if (m_pOrderModule->SupportsReactions() && pParam1)
				{
				if (ReactToBaseDestroyed(*pParam1))
					return resAck;
				else
					return resNoAnswer;
				}
			else
				return m_pOrderModule->Communicate(m_pShip, m_AICtx, pSender, iMessage, pParam1, dwParam2, pData);
			}

		case msgQueryAttackStatus:
			{
			if (m_pOrderModule->SupportsReactions() && m_DeterModule.IsEnabled())
				return resAck;
			else
				return m_pOrderModule->Communicate(m_pShip, m_AICtx, pSender, iMessage, pParam1, dwParam2, pData);
			}

		default:
			return m_pOrderModule->Communicate(m_pShip, m_AICtx, pSender, iMessage, pParam1, dwParam2, pData);
		}
	}

void CBaseShipAI::OnDestroyed (SDestroyCtx &Ctx)

//	OnDestroyed
//
//	The ship was destroyed

	{
	//	Notify our order module (or derived class if we're doing it old-style)

	if (m_pOrderModule)
		m_pOrderModule->Destroyed(m_pShip, Ctx);
	else
		OnDestroyedNotify(Ctx);
	}

void CBaseShipAI::OnDocked (CSpaceObject *pObj)

//	OnDocked
//
//	Event when the ship is docked

	{
	//	NOTE: At SystemCreate time we dock ships to their station without them
	//	requesting it, thus we cannot assume that m_bDockingRequested is TRUE.
	//	(However, we set state to None no matter what).

	m_AICtx.SetDockingRequested(false);

	//	Call descendants

	OnDockedEvent(pObj);
	}

void CBaseShipAI::OnDockingStop (void)

//	OnDockingStop
//
//	Docking maneuvers have ended, either because we docked or because we 
//	aborted docking.

	{
	m_AICtx.SetDockingRequested(false);
	}

void CBaseShipAI::OnEnterGate (CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate, bool bAscend)

//	OnEnterGate
//
//	Ship enters a stargate
	
	{
	//	If we're ascending, then leave the ship alone (it will get pulled out of
	//	the system by our caller).

	if (bAscend)
		;

	//	If our orders are to follow the player through a gate, then suspend
	//	until the player removes us from the system in TransferGateFollowers

	else if (GetCurrentOrder() == IShipController::orderFollowPlayerThroughGate)
		m_pShip->Suspend();

	//	Ask the target what to do. If it handles it, then we do nothing.

	else if (pStargate 
			&& pStargate->FireOnObjGate(m_pShip))
		{ }

	//	Otherwise, we destroy ourselves

	else
		m_pShip->SetDestroyInGate();
	}

void CBaseShipAI::OnHitBarrier (CSpaceObject *pBarrierObj, const CVector &vPos)

//	OnHitBarrier
//
//	The ship hit a barrier

	{
	m_AICtx.SetBarrierClock(m_pShip);
	}

void CBaseShipAI::OnNewSystem (CSystem *pSystem)

//	OnNewSystem
//
//	Ship has moved from one system to another

	{
	//	Reset the nav path (because it was for the last system)

	m_AICtx.ClearNavPath();

	//	Let our subclass deal with it

	OnNewSystemNotify();

	//	Loop over all our future orders and make sure that we
	//	delete any that refer objects that are not in the new system
	//
	//	Note that this is called before the player is in the system
	//	but after all other player followers have been transfered.

	bool bChanged;
	m_Orders.OnNewSystem(pSystem, &bChanged);
	if (bChanged)
		FireOnOrderChanged();
	}

void CBaseShipAI::OnObjEnteredGate (CSpaceObject *pObj, CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate)

//	OnObjEnteredGate
//
//	Handle the case where another object entered a stargate

	{
	//	If the player has left and we have inter system events, then see if we
	//	need to follow the player

	if (pObj->IsPlayer() && m_pShip->HasInterSystemEvent())
		{
		CSpaceObject::InterSystemResults iResult = m_pShip->FireOnPlayerLeftSystem(pObj, pDestNode, sDestEntryPoint, pStargate);
		switch (iResult)
			{
			case CSpaceObject::interFollowPlayer:
				AddOrder(COrderDesc(IShipController::orderFollowPlayerThroughGate, pObj), true);
				break;

			case CSpaceObject::interWaitForPlayer:
				AddOrder(COrderDesc(IShipController::orderWaitForPlayer), true);
				break;
			}
		}

	//	Otherwise, figure out what to do based on our order

	switch (GetCurrentOrder())
		{
		case IShipController::orderEscort:
		case IShipController::orderFollow:
			if (pObj->IsPlayer() && IsPlayerOrPlayerFollower(GetCurrentOrderTarget()))
				{
				//	If the player just entered a stargate then see if we need to follow
				//	her or wait for her to return.

				if (m_pShip->Communicate(m_pShip, msgQueryWaitStatus) == resAck)
					AddOrder(COrderDesc(IShipController::orderWaitForPlayer), true);
				else
					AddOrder(COrderDesc(IShipController::orderFollowPlayerThroughGate, pObj), true);
				}
			else if (pObj == GetCurrentOrderTarget())
				{
				//	Otherwise, we cancel our order and follow the ship through the gate

				CancelCurrentOrder();
				AddOrder(COrderDesc(IShipController::orderGate, pStargate), true);
				}
			break;

		//	If we're trying to destroy the player, then wait until she returns.

		case IShipController::orderDestroyTarget:
			if (pObj->IsPlayer() && pObj == GetCurrentOrderTarget())
				AddOrder(COrderDesc(IShipController::orderDestroyPlayerOnReturn), true);
			break;
		}
	}

void CBaseShipAI::OnObjDestroyed (const SDestroyCtx &Ctx)

//	OnObjDestroyed
//
//	Handle the case where an object has been removed from the system

	{
	//	If our current order is to follow the player through and we get
	//	a notification that the player has been removed, then do nothing
	//	(we will be updated when we enter the new system).

	if (GetCurrentOrder() == IShipController::orderFollowPlayerThroughGate
			&& Ctx.Obj == GetCurrentOrderTarget())
		return;

	//	If we're following the player through a gate and the object
	//	got destroyed because it is also following the player, then we don't
	//	need to do anything.

	if (GetCurrentOrder() == IShipController::orderFollowPlayerThroughGate
			&& Ctx.iCause == enteredStargate
			&& Ctx.Obj.FollowsObjThroughGate(NULL))
		return;

	//	If we are following an object and it is about to be resurrected,
	//	then don't cancel the escort order

	if (GetCurrentOrder() == IShipController::orderEscort
			&& Ctx.Obj == GetCurrentOrderTarget()
			&& Ctx.bResurrectPending)
		return;

	//	If we have an order module, let it handle it...

	if (m_pOrderModule)
		m_pOrderModule->ObjDestroyed(m_pShip, Ctx);

	//	Otherwise, let our subclass deal with it...

	else
		OnObjDestroyedNotify(Ctx);

	//	Deter module

	m_DeterModule.OnObjDestroyed(*m_pShip, Ctx);

	//	Loop over all our future orders and make sure that we
	//	delete any that refer to this object.

	bool bChanged;
	m_Orders.OnObjDestroyed(&Ctx.Obj, &bChanged);
	if (bChanged)
		FireOnOrderChanged();
	}

void CBaseShipAI::OnPlayerChangedShips (CSpaceObject *pOldShip, SPlayerChangedShipsCtx &Options)

//	OnPlayerChangedShips
//
//	Player has changed ships

	{
	//	Get the new player ship

	CSpaceObject *pPlayerShip = m_pShip->GetUniverse().GetPlayerShip();
	if (pPlayerShip == NULL)
		{
		ASSERT(false);
		return;
		}

	//	Loop over all orders and see if we need to change the target

	if (!Options.bNoOrderTransfer)
		{
		bool bChanged;
		m_Orders.OnPlayerChangedShips(pOldShip, pPlayerShip, Options, &bChanged);
		if (bChanged)
			FireOnOrderChanged();
		}
	}

void CBaseShipAI::OnPlayerObj (CSpaceObject *pPlayer)

//	OnPlayerObj
//
//	Player has entered the system

	{
	IShipController::OrderTypes iOrder = GetCurrentOrder();

	//	First, cancel the order that was making us wait for the player

	switch (iOrder)
		{
		case IShipController::orderDestroyPlayerOnReturn:
		case IShipController::orderWaitForPlayer:
			CancelCurrentOrder();
			break;
		}

	//	If we have events, then let the event handle everything

	bool bHandled = false;
	if (m_pShip->HasInterSystemEvent())
		{
		m_pShip->FireOnPlayerEnteredSystem(pPlayer);
		bHandled = (GetCurrentOrder() != IShipController::orderNone);
		}

	//	If not handled, add orders

	if (!bHandled)
		{
		switch (iOrder)
			{
			case IShipController::orderDestroyPlayerOnReturn:
				AddOrder(COrderDesc(IShipController::orderDestroyTarget, pPlayer), true);
				break;

			case IShipController::orderWaitForPlayer:
				//	We assume that if the ship is a wingman of the player, then
				//	it is responsible for setting its own orders. All other ships
				//	need to have an escort order (for backwards compatibility).

				if (!m_fIsPlayerWingman)
					{
					AddOrder(COrderDesc(IShipController::orderEscort, pPlayer), true);
					m_pShip->Communicate(m_pShip, msgWait);
					}
				break;
			}
		}
	}

void CBaseShipAI::OnShipStatus (EShipStatusNotifications iEvent, DWORD dwData)

//	OnShipStatus
//
//	Status has changed

	{
	//	If we're just a section, notify our main ship.

	if (m_pShip->IsShipSection())
		{
		CSpaceObject *pRoot = m_pShip->GetAttachedRoot();
		CShip *pRootShip = (pRoot ? pRoot->AsShip() : NULL);
		if (pRootShip)
			pRootShip->GetController()->OnShipStatus(iEvent, dwData);
		}
	}

void CBaseShipAI::OnStationDestroyed (const SDestroyCtx &Ctx)

//	OnStationDestroyed
//
//	Station has been destroyed

	{
	ASSERT(!Ctx.Obj.IsDestroyed());

	//	In some cases we ignore the notification because the station still
	//	exists

	DWORD dwFlags = IShipController::GetOrderFlags(GetCurrentOrder());
	if (dwFlags & (ORDER_FLAG_DELETE_ON_STATION_DESTROYED | ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED))
		{
		if (m_pOrderModule)
			m_pOrderModule->ObjDestroyed(m_pShip, Ctx);
		else
			OnObjDestroyedNotify(Ctx);
		}

	//	Remove orders as appropriate

	bool bChanged;
	m_Orders.OnStationDestroyed(&Ctx.Obj, &bChanged);
	if (bChanged)
		FireOnOrderChanged();
	}

bool CBaseShipAI::React (AIReaction iReaction)

//	React
//
//	React

	{
	switch (iReaction)
		{
		case AIReaction::None:
			return false;

		case AIReaction::Gate:
			AddOrder(COrderDesc(IShipController::orderGate), true);
			return true;

		default:
			throw CException(ERR_FAIL);
		}
	}

bool CBaseShipAI::React (AIReaction iReaction, CSpaceObject &TargetObj)

//	React
//
//	React

	{
	switch (iReaction)
		{
		case AIReaction::None:
			return false;

		case AIReaction::Chase:
			{
			int iMaxTime = (GetBase() ? 0 : CAIBehaviorCtx::DETER_CHASE_MAX_TIME);
			AddOrder(CDeterChaseOrder::Create(TargetObj, GetBase(), CalcThreatRange(), iMaxTime), true);
			return true;
			}

		case AIReaction::Destroy:
		case AIReaction::DestroyAndRetaliate:
			AddOrder(COrderDesc(IShipController::orderDestroyTarget, &TargetObj), true);
			return true;

		case AIReaction::Deter:
			m_DeterModule.BehaviorStart(*m_pShip, m_AICtx, TargetObj, false);
			return true;

		case AIReaction::DeterWithSecondaries:
			m_DeterModule.BehaviorStart(*m_pShip, m_AICtx, TargetObj, true);
			return true;

		default:
			return React(iReaction);
		}
	}

void CBaseShipAI::ReactToAttack (CSpaceObject &AttackerObj, const SDamageCtx &Damage)

//	ReactToAttack
//
//	React to an attack.

	{
	AIReaction iReaction = GetReactToAttack();
	switch (iReaction)
		{
		case AIReaction::None:
			break;

		case AIReaction::Chase:
		case AIReaction::Destroy:
		case AIReaction::DestroyAndRetaliate:
		case AIReaction::Deter:
		case AIReaction::DeterWithSecondaries:
			{
			//	Continue attacks.

			if (iReaction == AIReaction::DestroyAndRetaliate)
				AddOrder(COrderDesc(IShipController::orderAttackNearestEnemy));

			//	If the attacker is a valid threat, then add an order

			if (m_AICtx.CalcIsDeterNeeded(*m_pShip, AttackerObj))
				React(iReaction, AttackerObj);

			m_AICtx.CommunicateWithBaseAttackDeter(*m_pShip, AttackerObj, Damage.GetOrderGiver());
			break;
			}

		case AIReaction::Gate:
			React(iReaction);
			break;

		default:
			throw CException(ERR_FAIL);
		}
	}

bool CBaseShipAI::ReactToBaseDestroyed (CSpaceObject &AttackerObj)

//	ReactToBaseDestroyed
//
//	React to our base being destroyed

	{
	AIReaction iReaction = GetReactToBaseDestroyed();
	switch (iReaction)
		{
		case AIReaction::None:
			return false;

		case AIReaction::Chase:
		case AIReaction::Destroy:
		case AIReaction::DestroyAndRetaliate:
		case AIReaction::Deter:
		case AIReaction::DeterWithSecondaries:
			{
			//	Continue attacks.

			if (iReaction == AIReaction::DestroyAndRetaliate)
				AddOrder(COrderDesc(IShipController::orderAttackNearestEnemy));

			//	Enable deter module

			if (m_AICtx.CalcIsPossibleTarget(*m_pShip, AttackerObj))
				{
				React(iReaction, AttackerObj);
				return true;
				}
			else
				return false;
			}

		case AIReaction::Gate:
			React(iReaction);
			return true;

		default:
			throw CException(ERR_FAIL);
		}
	}

bool CBaseShipAI::ReactToDeterMessage (CSpaceObject &AttackerObj)

//	ReactToDeterMessage
//
//	React to a message from our station to deter a target.

	{
	AIReaction iReaction = GetReactToAttack();
	switch (iReaction)
		{
		case AIReaction::None:
			return false;

		case AIReaction::Chase:
		case AIReaction::Destroy:
		case AIReaction::DestroyAndRetaliate:
		case AIReaction::Deter:
		case AIReaction::DeterWithSecondaries:
			{
			//	Continue attacks.

			if (iReaction == AIReaction::DestroyAndRetaliate)
				AddOrder(COrderDesc(IShipController::orderAttackNearestEnemy));

			//	If the attacker is a valid threat, then react

			if (m_AICtx.CalcIsDeterNeeded(*m_pShip, AttackerObj))
				{
				React(iReaction, AttackerObj);
				return true;
				}
			else
				return false;
			}

		case AIReaction::Gate:
			React(iReaction);
			return true;

		default:
			throw CException(ERR_FAIL);
		}
	}

void CBaseShipAI::ReadFromStream (SLoadCtx &Ctx, CShip *pShip)

//	ReadFromStream
//
//	Reads controller data from stream
//
//	DWORD		Controller class
//	DWORD		ship class UNID (needed to set AISettings)
//	DWORD		m_pShip (CSpaceObject ref)
//	DWORD		m_Blacklist
//
//	DWORD		order (for order module)
//	IOrderModule
//	CDeterModule	m_DeterModule
//
//	DWORD		No of orders
//	DWORD		order: Order
//	DWORD		order: pTarget
//	DWORD		order: dwData
//
//	CString		m_pCommandCode (unlinked)
//
//	DWORD		flags

	{
	DWORD dwLoad;

	//	We already read the controller class (we need it to construct the 
	//	object).

	//	Read stuff

	Ctx.pStream->Read(dwLoad);
	CShipClass *pClass = Ctx.GetUniverse().FindShipClass(dwLoad);
	CSystem::ReadObjRefFromStream(Ctx, (CSpaceObject **)&m_pShip);
	if (m_pShip == NULL)
		m_pShip = pShip;

	//	Ship controls moved to m_AICtx

	if (Ctx.dwVersion < 75)
		{
		Ctx.pStream->Read(dwLoad);
		m_AICtx.SetManeuver((EManeuver)dwLoad);

		Ctx.pStream->Read(dwLoad);
		m_AICtx.SetThrustDir((int)dwLoad);

		Ctx.pStream->Read(dwLoad);
		m_AICtx.SetLastTurn((EManeuver)dwLoad);

		Ctx.pStream->Read(dwLoad);
		m_AICtx.SetLastTurnCount(dwLoad);
		}

	//	Read blacklist

	if (Ctx.dwVersion >= 6)
		m_Blacklist.ReadFromStream(Ctx);

	//	Older versions

	if (Ctx.dwVersion < 75)
		{
		//	Read manuever counter

		if (Ctx.dwVersion >= 33)
			{
			Ctx.pStream->Read(dwLoad);
			m_AICtx.SetManeuverCounter(dwLoad);
			}

		//	Read potential 

		if (Ctx.dwVersion >= 34)
			{
			CVector vPotential;
			vPotential.ReadFromStream(*Ctx.pStream);
			m_AICtx.SetPotential(vPotential);
			}
		}

	//	AI settings

	if (Ctx.dwVersion >= 75)
		;//	No need to read because this is part of m_AICtx
	else if (Ctx.dwVersion >= 68)
		m_AICtx.ReadFromStreamAISettings(Ctx);
	else
		m_AICtx.SetAISettings(pClass->GetAISettings());

	//	AI context

	if (Ctx.dwVersion >= 75)
		m_AICtx.ReadFromStream(Ctx);

	//	Order module

	if (Ctx.dwVersion >= 75)
		{
		Ctx.pStream->Read(dwLoad);
		m_pOrderModule = IOrderModule::Create((IShipController::OrderTypes)dwLoad);
		if (m_pOrderModule)
			m_pOrderModule->ReadFromStream(Ctx);
		}

	//	Deter module

	if (Ctx.dwVersion >= 197)
		m_DeterModule.ReadFromStream(Ctx);

	//	Read navpath info

	if (Ctx.dwVersion < 75)
		{
		if (Ctx.dwVersion >= 11)
			{
			CNavigationPath *pNavPath;
			int iNavPathPos;

			Ctx.pStream->Read(dwLoad);
			if (dwLoad)
				pNavPath = Ctx.pSystem->GetNavPathByID(dwLoad);
			else
				pNavPath = NULL;

			Ctx.pStream->Read(iNavPathPos);
			if (pNavPath == NULL)
				iNavPathPos = -1;

			m_AICtx.SetNavPath(pNavPath, iNavPathPos);
			}
		}

	//	Read orders

	m_Orders.ReadFromStream(Ctx);

	//	Command code

	if (Ctx.dwVersion >= 75)
		{
		CString sCode;
		sCode.ReadFromStream(Ctx.pStream);
		if (!sCode.IsBlank())
			m_pCommandCode = CCodeChain::LinkCode(sCode)->Reference();
		else
			m_pCommandCode = NULL;
		}
	else
		m_pCommandCode = NULL;

	//	Flags

	Ctx.pStream->Read(dwLoad);
	m_fDeviceActivate =			((dwLoad & 0x00000001) ? true : false);
	m_fPlayerBlacklisted =		((dwLoad & 0x00000002) && (Ctx.dwVersion >= 75) ? true : false);
	//	0x00000004 unused at 75
	m_fCheckedForWalls =		((dwLoad & 0x00000008) ? true : false);
	m_fAvoidWalls =				((dwLoad & 0x00000010) ? true : false);
	m_fIsPlayerWingman =		((dwLoad & 0x00000020) ? true : false);
	m_fIsPlayerEscort =			((dwLoad & 0x00000040) ? true : false);

	//	Unfortunately, the only way to tell if we're using an order
	//	modules it to try to create one. [There is an edge condition
	//	if we saved the game before we got to create an order module,
	//	which happens when saving missions.]

	if (m_pOrderModule)
		m_fOldStyleBehaviors = false;
	else
		{
		IOrderModule *pDummy = IOrderModule::Create(GetCurrentOrder());
		if (pDummy)
			{
			m_fOldStyleBehaviors = false;
			delete pDummy;
			}
		else
			m_fOldStyleBehaviors = true;
		}

	//	In version 75 some flags were moved to the AI context

	if (Ctx.dwVersion < 75)
		{
		m_AICtx.SetDockingRequested((dwLoad & 0x00000002) ? true : false);
		m_AICtx.SetWaitingForShieldsToRegen((dwLoad & 0x00000004) ? true : false);
		}

	//	Subclasses

	OnReadFromStream(Ctx);
	}

void CBaseShipAI::ResetBehavior (void)

//	ResetBehavior
//
//	Resets fire and motion

	{
	m_AICtx.Update(m_pShip);
	m_pShip->ClearAllTriggered();
	m_Blacklist.Update(m_pShip->GetUniverse().GetTicks());
	}

int CBaseShipAI::SetAISettingInteger (const CString &sSetting, int iValue)

//	SetAISettingInteger
//
//	Sets an AI setting

	{
	//	Let our sub-classes handle it first.

	if (OnSetAISettingInteger(sSetting, iValue))
		return iValue;

	//	Otherwise, this is a basic setting

	CString sNew = m_AICtx.SetAISetting(sSetting, strFromInt(iValue));
	m_AICtx.CalcInvariants(m_pShip);
	return strToInt(sNew, 0);
	}

CString CBaseShipAI::SetAISettingString (const CString &sSetting, const CString &sValue)

//	SetAISettingString
//
//	Sets an AI setting

	{
	//	Let our sub-classes handle it first.

	if (OnSetAISettingString(sSetting, sValue))
		return sValue;

	//	Otherwise, this is a basic setting

	CString sNew = m_AICtx.SetAISetting(sSetting, sValue);
	m_AICtx.CalcInvariants(m_pShip);
	return sNew; 
	}

void CBaseShipAI::SetCommandCode (ICCItem *pCode)

//	SetCommandCode
//
//	Sets the command code for the ship

	{
	if (m_pCommandCode)
		{
		m_pCommandCode->Discard();
		m_pCommandCode = NULL;
		}

	if (pCode)
		m_pCommandCode = pCode->Reference();
	}

void CBaseShipAI::SetShipToControl (CShip *pShip)

//	SetShipToControl
//
//	Set the ship that this controller will be controlling

	{
	ASSERT(m_pShip == NULL);

	m_pShip = pShip;
	m_AICtx.SetAISettings(pShip->GetClass()->GetAISettings());
	}

void CBaseShipAI::UpdateReactions (SUpdateCtx &Ctx)

//	UpdateReactions
//
//	Deal with threats (e.g., enemy ships in range).

	{
	//	Update reaction if we've got a current reaction. For example, if we're 
	//	currently deterring an enemy ship, this will turn to fire at the enemy.

	if (m_DeterModule.Behavior(*m_pShip, m_AICtx))
		{ }

	//	Otherwise, calc reaction to threats

	else
		{
		AIReaction iReaction = GetReactToThreat();
		switch (iReaction)
			{
			case AIReaction::None:
				break;

			case AIReaction::Chase:
			case AIReaction::Destroy:
			case AIReaction::DestroyAndRetaliate:
			case AIReaction::Deter:
			case AIReaction::DeterWithSecondaries:
			case AIReaction::Gate:
				{
				//	Every once in a while, look for a target

				if (m_pShip->IsDestinyTime(11))
					{
					if (CSpaceObject *pTarget = Ctx.GetTargetList().FindBestTarget(GetThreatTargetTypes()))
						{
						React(iReaction, *pTarget);
						}
					}

				break;
				}

			default:
				throw CException(ERR_FAIL);
			}
		}
	}

void CBaseShipAI::UpgradeShieldBehavior (void)

//	UpgradeShieldBehavior
//
//	Upgrade the ship's shields with a better one in cargo

	{
	CItem BestItem;
	int iBestLevel = 0;
	bool bReplace = false;

	//	Figure out the currently installed shields

	CInstalledDevice *pDevice = m_pShip->GetNamedDevice(devShields);
	if (pDevice)
		{
		iBestLevel = pDevice->GetLevel();
		bReplace = true;
		}

	//	Look for better shields

	CItemListManipulator ItemList(m_pShip->GetItemList());
	while (ItemList.MoveCursorForward())
		{
		const CItem &Item = ItemList.GetItemAtCursor();
		if (Item.GetType()->GetCategory() == itemcatShields
				&& !Item.IsInstalled()
				&& !Item.IsDamaged()
				&& Item.GetLevel() >= iBestLevel)
			{
			//	If this device requires some item, then skip it for now
			//	(later we can add code to figure out if we've got enough
			//	of the item)

			if (Item.GetType()->GetDeviceClass()->RequiresItems())
				;

			//	If we cannot install this item, skip it

			else if (!m_pShip->CanInstallItem(Item))
				;

			//	Otherwise, remember this item

			else
				{
				BestItem = Item;
				iBestLevel = Item.GetLevel();
				}
			}
		}

	//	If we found a better device, upgrade

	if (BestItem.GetType())
		{
		//	Uninstall the device weapon

		if (pDevice)
			{
			m_pShip->SetCursorAtNamedDevice(ItemList, devShields);
			m_pShip->RemoveItemAsDevice(ItemList);
			}

		//	Install the new item

		ItemList.SetCursorAtItem(BestItem);
		m_pShip->InstallItemAsDevice(ItemList);

		//	Recalc stuff

		m_AICtx.CalcInvariants(m_pShip);
		}
	}

void CBaseShipAI::UpgradeWeaponBehavior (void)

//	UpgradeWeaponBehavior
//
//	Upgrade the ship's weapon with a better one in cargo

	{
	//	Loop over all currently installed weapons

	bool bWeaponsInstalled = false;
	for (CDeviceItem DeviceItem : m_pShip->GetDeviceSystem())
		{
		if (DeviceItem.GetCategory() == itemcatWeapon)
			{
			//	Loop over all uninstalled weapons and see if we can
			//	find something better than this one.

			CItem BestItem;
			int iBestLevel = DeviceItem.GetLevel();

			CItemListManipulator ItemList(m_pShip->GetItemList());
			while (ItemList.MoveCursorForward())
				{
				const CItem &Item = ItemList.GetItemAtCursor();
				if (Item.GetType()->GetCategory() == itemcatWeapon
						&& !Item.IsInstalled()
						&& !Item.IsDamaged()
						&& Item.GetLevel() >= iBestLevel)
					{
					//	If this weapon is an ammo weapon, then skip it for now
					//	(later we can add code to figure out if we've got enough
					//	ammo for the weapon)

					if (Item.GetType()->GetDeviceClass()->RequiresItems())
						;

					//	If we cannot install this item, skip it

					else if (!m_pShip->CanInstallItem(Item))
						;

					//	Otherwise, remember this item

					else
						{
						BestItem = Item;
						iBestLevel = Item.GetLevel();
						}
					}
				}

			//	If we found a better weapon, upgrade

			if (!BestItem.IsEmpty())
				{
				int iSlot = DeviceItem.GetInstalledDevice()->GetDeviceSlot();

				//	Uninstall the previous weapon

				m_pShip->SetCursorAtDevice(ItemList, iSlot);
				m_pShip->RemoveItemAsDevice(ItemList);

				//	Install the new item

				ItemList.SetCursorAtItem(BestItem);
				m_pShip->InstallItemAsDevice(ItemList, iSlot);

				bWeaponsInstalled = true;
				}
			}
		}

	//	If we installed weapons, recalc invariants

	if (bWeaponsInstalled)
		m_AICtx.CalcInvariants(m_pShip);
	}

void CBaseShipAI::UseItemsBehavior (void)

//	UseItemsBehavior
//
//	Use various items appropriately

	{
	DEBUG_TRY

	if (m_pShip->IsDestinyTime(ITEM_ON_AI_UPDATE_CYCLE, ITEM_ON_AI_UPDATE_OFFSET))
		m_pShip->FireItemOnAIUpdate();

	if (m_AICtx.HasSuperconductingShields()
			&& m_pShip->IsDestinyTime(61)
			&& m_pShip->GetShieldLevel() < 40)
		{
		//	Look for superconducting coils

		CItemType *pType = m_pShip->GetUniverse().FindItemType(g_SuperconductingCoilUNID);
		if (pType)
			{
			CItem Coils(pType, 1);
			CItemListManipulator ItemList(m_pShip->GetItemList());
			
			if (ItemList.SetCursorAtItem(Coils))
				{
				m_pShip->UseItem(Coils);
				m_pShip->OnComponentChanged(comCargo);
				}
			}
		}

	DEBUG_CATCH
	}

void CBaseShipAI::WriteToStream (IWriteStream *pStream)

//	WriteToStream
//
//	Save the AI data to a stream
//
//	CString		Controller class
//	DWORD		ship class UNID (needed to set AISettings)
//	DWORD		m_pShip (CSpaceObject ref)
//	DWORD		m_Blacklist
//
//	DWORD		order (for order module)
//	IOrderModule
//	CDeterModule	m_DeterModule
//
//	DWORD		No of orders
//	DWORD		order: Order
//	DWORD		order: pTarget
//	DWORD		order: dwData
//
//	CString		m_pCommandCode (unlinked)
//
//	DWORD		flags
//
//	Subclasses...

	{
	DWORD dwSave;

	GetClass().WriteToStream(pStream);

	dwSave = m_pShip->GetClass()->GetUNID();
	pStream->Write(dwSave);

	m_pShip->WriteObjRefToStream(m_pShip, pStream);
	m_Blacklist.WriteToStream(pStream);

	//	Context

	m_AICtx.WriteToStream(pStream);

	//	Order module

	dwSave = (DWORD)(m_pOrderModule ? m_pOrderModule->GetOrder() : IShipController::orderNone);
	pStream->Write(dwSave);
	if (m_pOrderModule)
		m_pOrderModule->WriteToStream(pStream);

	//	Deter module

	m_DeterModule.WriteToStream(*pStream);

	//	Orders

	m_Orders.WriteToStream(*pStream, *m_pShip);

	//	Command code

	CString sCode;
	if (m_pCommandCode)
		sCode = CCodeChain::Unlink(m_pCommandCode);
	sCode.WriteToStream(pStream);

	//	Flags

	dwSave = 0;
	dwSave |= (m_fDeviceActivate ?			0x00000001 : 0);
	dwSave |= (m_fPlayerBlacklisted ?		0x00000002 : 0);
	//	0x00000004 unused at version 75.
	dwSave |= (m_fCheckedForWalls ?			0x00000008 : 0);
	dwSave |= (m_fAvoidWalls ?				0x00000010 : 0);
	dwSave |= (m_fIsPlayerWingman ?			0x00000020 : 0);
	dwSave |= (m_fIsPlayerEscort ?			0x00000040 : 0);
	pStream->Write(dwSave);

	//	Subclasses

	OnWriteToStream(pStream);
	}

