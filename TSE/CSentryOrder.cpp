//	CSentryOrder.cpp
//
//	CSentryOrder class implementation
//	Copyright (c) 2017 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

const int ATTACK_TIME_THRESHOLD =		150;

const Metric ATTACK_RANGE =				(20.0 * LIGHT_SECOND);
const Metric PATROL_SENSOR_RANGE =		(30.0 * LIGHT_SECOND);

CSentryOrder::CSentryOrder (void) : IOrderModule(objCount),
		m_fIsAttacking(false)

//	CSentryOrder constructor

	{
	}

void CSentryOrder::AttackEnemies (CShip *pShip, CAIBehaviorCtx &Ctx)

//	AttackEnemies
//
//	Attack enemies

	{
	//	If we've got a target, then attack it.

	if (m_fIsAttacking && m_Objs[objTarget])
		{
		Ctx.ImplementAttackTarget(pShip, m_Objs[objTarget], true);

		//	If we're out of range, then we cease attack

		if (pShip->IsDestinyTime(19)
				&& pShip->GetDistance2(m_Objs[objTarget]) > (4.0 * Ctx.GetMaxWeaponRange() * Ctx.GetMaxWeaponRange()))
			{
			m_fIsAttacking = false;
			m_Objs[objTarget] = NULL;
			}
		}

	//	Otherwise, we just attack any target

	else
		{
		Ctx.ImplementAttackNearestTarget(pShip, ATTACK_RANGE, &m_Objs[objTarget], NULL, true);

		//	Check to see if there are enemy ships that we need to attack

		if (pShip->IsDestinyTime(30)
				&& Ctx.IsAggressor()
				&& !Ctx.NoAttackOnThreat())
			{
			CSpaceObject *pTarget = pShip->GetVisibleEnemyInRange(pShip, PATROL_SENSOR_RANGE);
			if (pTarget)
				{
				m_fIsAttacking = true;
				m_Objs[objTarget] = pTarget;
				ASSERT(m_Objs[objTarget]->DebugIsValid() && m_Objs[objTarget]->NotifyOthersWhenDestroyed());
				}
			}
		}
	}

void CSentryOrder::OnAttacked (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pAttacker, const SDamageCtx &Damage, bool bFriendlyFire)

//	OnAttacked
//
//	We've been attacked.

	{
	//	If we have a valid attacker, then we try to respond. We ignore friendly
	//	fire because that's handled by CBaseShipAI.

	if (pAttacker == NULL
			|| !pAttacker->CanAttack()
			|| bFriendlyFire)
		return;

	//	Destroy the attacker

	if (m_Objs[objTarget] == NULL || mathRandom(1, 3) == 1)
		{
		m_fIsAttacking = true;
		m_Objs[objTarget] = pAttacker;
		ASSERT(m_Objs[objTarget]->DebugIsValid() && m_Objs[objTarget]->NotifyOthersWhenDestroyed());
		}

	//	If we were attacked twice (excluding multi-shot weapons) then we tell our 
	//	station about this.

	CSpaceObject *pBase;
	CSpaceObject *pTarget;
	CSpaceObject *pOrderGiver = Damage.GetOrderGiver();
	if (Ctx.IsSecondAttack()
			&& (pBase = m_Objs[objBase])
			&& pBase->IsAngryAt(pAttacker)
			&& (pTarget = pBase->CalcTargetToAttack(pAttacker, pOrderGiver)))
		pShip->Communicate(pBase, msgAttackDeter, pTarget);
	}

void CSentryOrder::OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx)

//	OnBehavior
//
//	Do it

	{
	DEBUG_TRY

	//	Handle waiting

	Ctx.ImplementHold(pShip);

	//	Attack while we wait

	AttackEnemies(pShip, Ctx);

	//	Use secondary weapons to attack enemies

	Ctx.ImplementFireOnTargetsOfOpportunity(pShip, m_Objs[objTarget]);

	//	See if our timer has expired

	if (m_iCountdown != -1 && m_iCountdown-- == 0)
		pShip->CancelCurrentOrder();

	DEBUG_CATCH
	}

void CSentryOrder::OnBehaviorStart (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pOrderTarget, const IShipController::SData &Data)

//	OnBehaviorStart
//
//	Initialize order module

	{
	DEBUG_TRY

	//	Undock, if necessary

	Ctx.Undock(pShip);

	//	Set our base

	m_Objs[objBase] = pOrderTarget;

	//	Set the timer in ticks

	DWORD dwTimer = Data.AsInteger();
	m_iCountdown = (dwTimer ? 1 + (g_TicksPerSecond * dwTimer) : -1);

	DEBUG_CATCH
	}

DWORD CSentryOrder::OnCommunicate (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2)

//	OnCommunicate
//
//	Handle communications

	{
	switch (iMessage)
		{
		case msgAttackDeter:
			m_Objs[objTarget] = pParam1;
			m_fIsAttacking = true;
			return resAck;

		case msgBaseDestroyedByTarget:
			{
			if (pParam1 == NULL
					|| pParam1->IsDestroyed()
					|| Ctx.IsNonCombatant())
				return resNoAnswer;

			m_fIsAttacking = true;
			m_Objs[objTarget] = pParam1;
			return resAck;
			}

		default:
			return resNoAnswer;
		}
	}

void CSentryOrder::OnDestroyed (CShip *pShip, SDestroyCtx &Ctx)

//	OnDestroyed
//
//	We've been destroyed.

	{
	//	If we've been destroyed, then ask our station to avenge us

	if (m_Objs[objBase]
			&& !m_Objs[objBase]->IsEnemy(pShip)
			&& !m_Objs[objBase]->IsDestroyed())
		m_Objs[objBase]->OnSubordinateDestroyed(Ctx);
	}

void CSentryOrder::OnObjDestroyed (CShip *pShip, const SDestroyCtx &Ctx, int iObj, bool *retbCancelOrder)

//	OnObjDestroyed
//
//	An object was destroyed

	{
	//	If we're deterring and the target got destroyed, we stop attacking

	if (iObj == objTarget)
		m_fIsAttacking = false;
	}

void CSentryOrder::OnReadFromStream (SLoadCtx &Ctx)

//	OnReadFromStream
//
//	Load data from saved game

	{
	DWORD dwLoad;

	//	Read basic info

	Ctx.pStream->Read((char *)&m_iCountdown, sizeof(DWORD));

	//	Flags

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	m_fIsAttacking = ((dwLoad & 0x00000001) ? true : false);
	}

void CSentryOrder::OnWriteToStream (CSystem *pSystem, IWriteStream *pStream)

//	OnWriteToStream
//
//	Write data to saved game

	{
	pStream->Write((char *)&m_iCountdown, sizeof(DWORD));

	//	Flags

	DWORD dwSave = 0;
	dwSave |= (m_fIsAttacking ? 0x00000001 : 0);

	pStream->Write((char *)&dwSave, sizeof(DWORD));
	}
