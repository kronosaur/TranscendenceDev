//	CEscortOrder.cpp
//
//	CEscortOrder class implementation
//	Copyright (c) 2014 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

const int ATTACK_TIME_THRESHOLD =		150;
const Metric MAX_FOLLOW_DISTANCE		(g_KlicksPerPixel * 350.0);
const Metric PATROL_SENSOR_RANGE =		(30.0 * LIGHT_SECOND);
const Metric PATROL_SENSOR_RANGE2 =		(PATROL_SENSOR_RANGE * PATROL_SENSOR_RANGE);

CEscortOrder::CEscortOrder (IShipController::OrderTypes iOrder) : IOrderModule(objCount),
		m_iOrder(iOrder)

//	CEscortOrder constructor

	{
	switch (iOrder)
		{
		case IShipController::orderEscort:
			m_fDeterEnemies = true;
			break;

		case IShipController::orderFollow:
			m_fDeterEnemies = false;
			break;

		default:
			ASSERT(false);
			m_fDeterEnemies = false;
		}
	}

void CEscortOrder::OnAttacked (CShip &Ship, CAIBehaviorCtx &Ctx, CSpaceObject &AttackerObj, const SDamageCtx &Damage, bool bFriendlyFire)

//	OnAttacked
//
//	We were attacked.

	{
	DEBUG_TRY

	if (!bFriendlyFire
			&& AttackerObj.CanAttack())
		{
		//	Tell our principal that we were attacked

		if (!m_Objs[objPrincipal]->FireOnSubordinateAttacked(Damage))
			Ship.Communicate(m_Objs[objPrincipal], msgEscortAttacked, &AttackerObj);

		//	Attack the target

		if (!Ctx.IsNonCombatant()
				&& m_fDeterEnemies
				&& (m_iState == stateEscorting
					|| m_iState == stateAttackingThreat))
			{
			m_Objs[objTarget] = &AttackerObj;
			m_iState = stateAttackingThreat;
			}
		}

	DEBUG_CATCH
	}

void CEscortOrder::OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx)

//	OnBehavior
//
//	Do it

	{
	DEBUG_TRY

	switch (m_iState)
		{
		case stateEscorting:
			{
			//	If we're a non-combatant, then we just follow

			if (Ctx.IsNonCombatant())
				{
				CVector vTarget = m_Objs[objPrincipal]->GetPos() - pShip->GetPos();
				Metric rTargetDist2 = vTarget.Dot(vTarget);
				Metric rMaxDist = (MAX_FOLLOW_DISTANCE) + (g_KlicksPerPixel * (pShip->GetDestiny() % 120));

				if (rTargetDist2 > (rMaxDist * rMaxDist))
					Ctx.ImplementCloseOnTarget(pShip, m_Objs[objPrincipal], vTarget, rTargetDist2);
				else if (rTargetDist2 < (g_KlicksPerPixel * g_KlicksPerPixel * 1024.0))
					Ctx.ImplementSpiralOut(pShip, vTarget);
				else
					Ctx.ImplementStop(pShip);
				}

			//	Otherwise, we follow the principal and attack threats

			else
				{
				Ctx.ImplementEscort(pShip, m_Objs[objPrincipal], &m_Objs[objTarget]);

				//	See if we need to deter a threat

				if (m_fDeterEnemies
						&& !Ctx.NoAttackOnThreat() 
						&& pShip->IsDestinyTime(30) 
						&& !Ctx.IsWaitingForShieldsToRegen())
					{
					m_Objs[objTarget] = pShip->GetVisibleEnemyInRange(m_Objs[objPrincipal], PATROL_SENSOR_RANGE);
					if (m_Objs[objTarget])
						{
						m_iState = stateAttackingThreat;
						ASSERT(m_Objs[objTarget]->DebugIsValid() && m_Objs[objTarget]->NotifyOthersWhenDestroyed());
						}
					}
				}

			break;
			}

		case stateAttackingThreat:
			{
			ASSERT(m_Objs[objTarget]);
			Ctx.ImplementAttackTarget(pShip, m_Objs[objTarget]);
			Ctx.ImplementFireOnTargetsOfOpportunity(pShip, m_Objs[objTarget]);

			//	Every once in a while check to see if we've wandered too far from
			//	our base.

			if (pShip->IsDestinyTime(20))
				{
				CVector vRange = m_Objs[objPrincipal]->GetPos() - pShip->GetPos();
				Metric rDistance2 = vRange.Dot(vRange);

				//	If we're outside of our patrol range and if we haven't
				//	been hit in a while then stop the attack.

				if (rDistance2 > (PATROL_SENSOR_RANGE * PATROL_SENSOR_RANGE))
					m_iState = stateEscorting;
				}

			break;
			}

		case stateWaiting:
			{
			Ctx.ImplementHold(pShip);
			Ctx.ImplementFireOnTargetsOfOpportunity(pShip);
			break;
			}
		}

	DEBUG_CATCH
	}

void CEscortOrder::OnBehaviorStart (CShip &Ship, CAIBehaviorCtx &Ctx, const COrderDesc &OrderDesc)

//	OnBehaviorStart
//
//	Initialize order module

	{
	DEBUG_TRY

	if (!OrderDesc.GetTarget())
		throw CException(ERR_FAIL);

	//	Make sure we're undocked because we're going flying

	Ctx.Undock(&Ship);

	//	Set state

	m_iState = stateEscorting;

	//	Remember the principal and report in

	m_Objs[objPrincipal] = OrderDesc.GetTarget();
	Ship.Communicate(OrderDesc.GetTarget(), msgEscortReportingIn, &Ship);

	DEBUG_CATCH
	}

DWORD CEscortOrder::OnCommunicate (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2, ICCItem *pData)

//	OnCommunicate
//
//	Handle communications

	{
	switch (iMessage)
		{
		case msgAbort:
			{
			switch (m_iState)
				{
				case stateAttackingThreat:
					if (pParam1 == NULL || pParam1 == m_Objs[objTarget])
						m_iState = stateEscorting;
					return resAck;

				default:
					return resNoAnswer;
				}
			}

		case msgAttack:
		case msgAttackDeter:
			{
			if (Ctx.IsNonCombatant()
					|| pParam1 == NULL
					|| pParam1->IsDestroyed()
					|| !m_fDeterEnemies)
				return resNoAnswer;

			switch (m_iState)
				{
				case stateEscorting:
				case stateAttackingThreat:
					m_iState = stateAttackingThreat;
					m_Objs[objTarget] = pParam1;
					ASSERT(m_Objs[objTarget]->DebugIsValid() && m_Objs[objTarget]->NotifyOthersWhenDestroyed());
					return resAck;

				default:
					return resNoAnswer;
				}
			}

		case msgBaseDestroyedByTarget:
		case msgDestroyBroadcast:
			{
			if (pParam1 == NULL
					|| pParam1->IsDestroyed())
				return resNoAnswer;

			pShip->GetController()->AddOrder(COrderDesc(IShipController::orderDestroyTarget, pParam1), true);

			return resAck;
			}

		case msgFormUp:
			{
			if (m_iState != stateEscorting)
				{
				m_iState = stateEscorting;
				return resAck;
				}
			else
				return resNoAnswer;
			}

		case msgQueryCommunications:
			{
			if (m_Objs[objPrincipal] != pParam1)
				return resNoAnswer;

			DWORD dwRes = 0;
			if (!Ctx.IsNonCombatant() && m_fDeterEnemies)
				dwRes |= resCanAttack;
			if (m_iState == stateAttackingThreat)
				dwRes |= (resCanAbortAttack | resCanFormUp);
			if (m_iState != stateWaiting)
				dwRes |= resCanWait;
			else
				dwRes |= resCanFormUp;

			return dwRes;
			}

		case msgQueryEscortStatus:
			{
			if (m_Objs[objPrincipal] == pParam1)
				return resAck;
			else
				return resNoAnswer;
			}

		case msgQueryWaitStatus:
			return (m_iState == stateWaiting ? resAck : resNoAnswer);

		case msgQueryAttackStatus:
			return (m_iState == stateAttackingThreat ? resAck : resNoAnswer);

		case msgWait:
			m_iState = stateWaiting;
			return resAck;

		default:
			return resNoAnswer;
		}
	}

CSpaceObject *CEscortOrder::OnGetBase (void)

//	OnGetBase
//
//	Returns our base, which is the base of the principal.

	{
	if (m_Objs[objPrincipal] == NULL)
		return NULL;

	return m_Objs[objPrincipal]->GetBase();
	}

void CEscortOrder::OnObjDestroyed (CShip *pShip, const SDestroyCtx &Ctx, int iObj, bool *retbCancelOrder)

//	OnObjDestroyed
//
//	Object destroyed

	{
	if (iObj == objPrincipal)
		{
		//	Retaliate

		if (m_fDeterEnemies)
			{
			CSpaceObject *pTarget;
			if (Ctx.Attacker.IsCausedByNonFriendOf(pShip) 
					&& Ctx.Attacker.GetObj()
					&& (pTarget = pShip->CalcTargetToAttack(Ctx.Attacker.GetObj(), Ctx.GetOrderGiver())))
				pShip->GetController()->AddOrder(COrderDesc(IShipController::orderDestroyTarget, pTarget));
			else
				pShip->GetController()->AddOrder(COrderDesc(IShipController::orderAttackNearestEnemy));
			}

		//	Cancel our order

		*retbCancelOrder = true;
		}
	else if (iObj == objTarget)
		{
		if (m_iState == stateAttackingThreat)
			m_iState = stateEscorting;
		}
	}

void CEscortOrder::OnReadFromStream (SLoadCtx &Ctx, const COrderDesc &OrderDesc)

//	OnReadFromStream
//
//	Load data from saved game

	{
	DWORD dwLoad;

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	m_iState = (States)dwLoad;
	}

void CEscortOrder::OnWriteToStream (IWriteStream *pStream) const

//	OnWriteToStream
//
//	Write data to saved game

	{
	DWORD dwSave;

	dwSave = (DWORD)m_iState;
	pStream->Write((char *)&dwSave, sizeof(DWORD));
	}

