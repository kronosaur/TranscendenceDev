//	CWaitOrder.cpp
//
//	CWaitOrder class implementation
//	Copyright (c) 2014 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

const int ATTACK_TIME_THRESHOLD =		150;

const Metric ATTACK_RANGE =				(20.0 * LIGHT_SECOND);
const Metric PATROL_SENSOR_RANGE =		(30.0 * LIGHT_SECOND);

CWaitOrder::CWaitOrder (IShipController::OrderTypes iOrder) : IOrderModule(objCount),
		m_iOrder(iOrder),
		m_fWaitUntilLeaderUndocks(false),
		m_fWaitForLeaderToApproach(false),
		m_fAttackEnemies(false),
		m_fWaitForEnemy(false),
		m_fDeterEnemies(false),
		m_fIsDeterring(false),
		m_fWaitForThreat(false)

//	CWaitOrder constructor

	{
	switch (iOrder)
		{
		case IShipController::orderHold:
			m_fAttackEnemies = true;
			m_fDeterEnemies = true;
			break;

		case IShipController::orderWaitForEnemy:
			m_fWaitForEnemy = true;
			m_fAttackEnemies = true;
			break;

		case IShipController::orderWaitForTarget:
			m_fWaitForLeaderToApproach = true;
			m_fAttackEnemies = true;
			break;

		case IShipController::orderWaitForThreat:
			m_fWaitForThreat = true;
			break;

		case IShipController::orderWaitForUndock:
			m_fWaitUntilLeaderUndocks = true;
			break;
		}
	}

void CWaitOrder::AttackEnemies (CShip *pShip, CAIBehaviorCtx &Ctx, bool bReady)

//	AttackEnemies
//
//	Attack enemies

	{
	//	If we're deterring an enemy, attack

	if (m_fIsDeterring
			&& m_Objs[objTarget])
		{
		if (bReady)
			Ctx.ImplementAttackNearestTarget(pShip, ATTACK_RANGE, &m_Objs[objTarget], NULL, true);

		//	Check to see if target has hit back. If not, stop the attack

		if (pShip->IsDestinyTime(20)
				&& !Ctx.IsBeingAttacked(3 * ATTACK_TIME_THRESHOLD))
			{
			m_fIsDeterring = false;
			m_Objs[objTarget] = NULL;
			}
		}

	//	Otherwise, we just attack any target

	else
		{
		Ctx.ImplementAttackNearestTarget(pShip, ATTACK_RANGE, &m_Objs[objTarget]);

		//	Check to see if there are enemy ships that we need to attack

		if (m_fDeterEnemies
				&& pShip->IsDestinyTime(30)
				&& !Ctx.NoAttackOnThreat())
			{
			CSpaceObject *pTarget = pShip->GetVisibleEnemyInRange(pShip, PATROL_SENSOR_RANGE);
			if (pTarget)
				{
				m_fIsDeterring = true;
				m_Objs[objTarget] = pTarget;
				ASSERT(m_Objs[objTarget]->DebugIsValid() && m_Objs[objTarget]->NotifyOthersWhenDestroyed());
				}
			}
		}
	}

bool CWaitOrder::IsLeaderInRange (CShip *pShip)

//	IsLeaderInRange
//
//	Returns TRUE if the leader is in range.

	{
	CPerceptionCalc Perception(pShip->GetPerception());
	Metric rRange2 = (m_Objs[objLeader]->GetPos() - pShip->GetPos()).Length2();

	return ((m_rDistance <= 0.0 || rRange2 < (m_rDistance * m_rDistance))
			&& Perception.CanBeTargeted(m_Objs[objLeader], rRange2));
	}

void CWaitOrder::OnAttacked (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pAttacker, const SDamageCtx &Damage, bool bFriendlyFire)

//	OnAttacked
//
//	We've been attacked.

	{
	//	If we're waiting for a threat and we got attacked, then we're done 
	//	waiting.

	if (m_fWaitForThreat
			&& pAttacker
			&& pAttacker->CanAttack())
		pShip->CancelCurrentOrder();

	//	If we're waiting for enemies and someone deliberately hits us, then 
	//	that counts.

	else if (m_fWaitForEnemy
			&& Ctx.IsSecondAttack()
			&& pAttacker
			&& pAttacker->CanAttack()
			&& !bFriendlyFire)
		pShip->CancelCurrentOrder();

	//	If we're waiting for a target, and if we get hit (and it's not friendly) 
	//	then the target is here.
	//
	//	NOTE: We don't debouce hits (with IsBeingAttacked) because even	a stray
	//	shot from the target means that the target is here.

	else if (m_fWaitForLeaderToApproach
			&& pAttacker == m_Objs[objLeader]
			&& !bFriendlyFire)
		pShip->CancelCurrentOrder();

	//	If we're deterring enemies and we don't already have a target, and we
	//	feel like it, then attack this target.

	else if (m_fDeterEnemies
			&& m_Objs[objTarget] == NULL
			&& pAttacker
			&& pAttacker->CanAttack()
			&& !bFriendlyFire
			&& mathRandom(1, 3) == 1)
		{
		m_fIsDeterring = true;
		m_Objs[objTarget] = pAttacker;
		ASSERT(m_Objs[objTarget]->DebugIsValid() && m_Objs[objTarget]->NotifyOthersWhenDestroyed());
		}
	}

void CWaitOrder::OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx)

//	OnBehavior
//
//	Do it

	{
	DEBUG_TRY

	//	Handle waiting

	bool bInPlace;
	Ctx.ImplementHold(pShip, &bInPlace);

	//	Attack while we wait?

	if (m_fAttackEnemies)
		{
		//	If desired, use primary weapons to attack enemies

		AttackEnemies(pShip, Ctx, bInPlace);

		//	Use secondary weapons to attack enemies

		Ctx.ImplementFireOnTargetsOfOpportunity(pShip, m_Objs[objTarget]);
		}

	//	See if our timer has expired

	if (m_iCountdown != -1 && m_iCountdown-- == 0)
		pShip->CancelCurrentOrder();

	//	Otherwise, see if other conditions are fullfilled.

	else if (m_fWaitUntilLeaderUndocks
			&& m_Objs[objLeader]->GetDockedObj() == NULL)
		pShip->CancelCurrentOrder();

	else if (m_fWaitForLeaderToApproach
			&& pShip->IsDestinyTime(17)
			&& IsLeaderInRange(pShip))
		pShip->CancelCurrentOrder();

	else if (m_fWaitForEnemy
			&& pShip->IsDestinyTime(17)
			&& pShip->GetNearestVisibleEnemy())
		pShip->CancelCurrentOrder();

	else if (m_fWaitForThreat
			&& pShip->IsDestinyTime(90)
			&& pShip->GetDockedObj()
			&& (pShip->GetDockedObj()->IsAngry() || pShip->GetDockedObj()->IsAbandoned() || pShip->GetDockedObj()->IsDestroyed()))
		pShip->CancelCurrentOrder();

	DEBUG_CATCH
	}

void CWaitOrder::OnBehaviorStart (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pOrderTarget, const IShipController::SData &Data)

//	OnBehaviorStart
//
//	Initialize order module

	{
	DEBUG_TRY

	DWORD dwTimer;

	//	If we're attacking enemies, then we can't be docked.

	if (m_fAttackEnemies)
		Ctx.Undock(pShip);

	//	Waiting for leader to undock: we need a leader and an optional timer.

	if (m_fWaitUntilLeaderUndocks)
		{
		m_Objs[objLeader] = pOrderTarget;
		ASSERT(m_Objs[objLeader]->DebugIsValid() && m_Objs[objLeader]->NotifyOthersWhenDestroyed());

		dwTimer = Data.AsInteger();
		}

	//	Waiting for leader to approach: we need a leader, a distance (which may be 0)
	//	and an optional timer.

	else if (m_fWaitForLeaderToApproach)
		{
		m_Objs[objLeader] = pOrderTarget;
		ASSERT(m_Objs[objLeader]->DebugIsValid() && m_Objs[objLeader]->NotifyOthersWhenDestroyed());

		m_rDistance = LIGHT_SECOND * Data.AsInteger();
		dwTimer = Data.AsInteger2();
		}

	//	Otherwise, just get a timer.

	else
		dwTimer = Data.AsInteger();

	//	Set the timer in ticks

	m_iCountdown = (dwTimer ? 1 + (g_TicksPerSecond * dwTimer) : -1);

	DEBUG_CATCH
	}

DWORD CWaitOrder::OnCommunicate (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2)

//	OnCommunicate
//
//	Handle communications

	{
	switch (iMessage)
		{
		case msgQueryWaitStatus:
			return resAck;

		default:
			return resNoAnswer;
		}
	}

void CWaitOrder::OnDestroyed (CShip *pShip, SDestroyCtx &Ctx)

//	OnDestroyed
//
//	We've been destroyed.

	{
	CSpaceObject *pTarget;

	//	If we're waiting on a threat at a station, then ask the station to 
	//	avenge us if we die.

	if (m_fWaitForThreat
			&& pShip->GetDockedObj()
			&& Ctx.Attacker.GetObj()
			&& Ctx.Attacker.GetObj()->CanAttack()
			&& !pShip->GetDockedObj()->IsEnemy(pShip)
			&& (pTarget = pShip->CalcTargetToAttack(Ctx.Attacker.GetObj(), Ctx.GetOrderGiver())))
		{
		CSpaceObject *pOrderGiver = Ctx.GetOrderGiver();

		//	If we were attacked by a friend, then we tell our station
		//	so they can be blacklisted.

		if (pOrderGiver 
				&& pShip->IsFriend(pOrderGiver))
			pShip->Communicate(pShip->GetDockedObj(), msgDestroyedByFriendlyFire, pTarget);

		//	Otherwise, we deter the target

		else
			pShip->Communicate(pShip->GetDockedObj(), msgDestroyedByHostileFire, pTarget);
		}
	}

void CWaitOrder::OnObjDestroyed (CShip *pShip, const SDestroyCtx &Ctx, int iObj, bool *retbCancelOrder)

//	OnObjDestroyed
//
//	An object was destroyed

	{
	//	If we're deterring and the target got destroyed, we stop attacking

	if (m_fIsDeterring && iObj == objTarget)
		m_fIsDeterring = false;

	//	If we're waiting for a threat and the station we're docked with got 
	//	destroyed, then we leave.

	if (m_fWaitForThreat
			&& pShip->GetDockedObj() == Ctx.pObj)
		*retbCancelOrder = true;
	}

void CWaitOrder::OnReadFromStream (SLoadCtx &Ctx)

//	OnReadFromStream
//
//	Load data from saved game

	{
	DWORD dwLoad;

	//	Older version

	if (Ctx.dwVersion < 109)
		Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));

	//	Read basic info

	Ctx.pStream->Read((char *)&m_iCountdown, sizeof(DWORD));
	if (Ctx.dwVersion >= 109)
		Ctx.pStream->Read((char *)&m_rDistance, sizeof(Metric));

	//	Flags

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	m_fIsDeterring = ((dwLoad & 0x00000001) ? true : false);
	}

void CWaitOrder::OnWriteToStream (CSystem *pSystem, IWriteStream *pStream)

//	OnWriteToStream
//
//	Write data to saved game

	{
	pStream->Write((char *)&m_iCountdown, sizeof(DWORD));
	pStream->Write((char *)&m_rDistance, sizeof(Metric));

	//	Flags

	DWORD dwSave = 0;
	dwSave |= (m_fIsDeterring ? 0x00000001 : 0);

	pStream->Write((char *)&dwSave, sizeof(DWORD));
	}
