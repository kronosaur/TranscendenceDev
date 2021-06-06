//	CAttackOrder.cpp
//
//	CAttackOrder class implementation
//	Copyright (c) 2014 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

const Metric SAFE_ORBIT_RANGE =			(30.0 * LIGHT_SECOND);
const Metric THREAT_SENSOR_RANGE =		(10.0 * LIGHT_SECOND);
const Metric THREAT_SENSOR_RANGE2 =		(THREAT_SENSOR_RANGE * THREAT_SENSOR_RANGE);

const Metric WANDER_SAFETY_RANGE =		(20.0 * LIGHT_SECOND);
const Metric WANDER_SAFETY_RANGE2 =		(WANDER_SAFETY_RANGE * WANDER_SAFETY_RANGE);

const Metric STAY_IN_AREA_THRESHOLD =   (50.0 * LIGHT_SECOND);

CAttackOrder::CAttackOrder (IShipController::OrderTypes iOrder) : IOrderModule(OBJ_COUNT),
		m_iOrder(iOrder)

//	CAttackOrder constructor

	{
	switch (m_iOrder)
		{
		case IShipController::orderDestroyTarget:
			break;

		case IShipController::orderAttackOrRetreat:
			m_fRetreatIfNecessary = true;
			break;

		case IShipController::orderAttackNearestEnemy:
			m_fNearestTarget = true;
			break;

		case IShipController::orderAttackArea:
			m_fNearestTarget = true;
			m_fInRangeOfObject = true;
			break;

		case IShipController::orderHoldAndAttack:
			m_fHold = true;
			break;

		default:
			throw CException(ERR_FAIL);
		}
	}

CSpaceObject *CAttackOrder::GetBestTarget (CShip *pShip)

//	GetBestTarget
//
//	Returns the best target that we should pursue right now. We return NULL if
//	there is no appropriate target.

	{
	DEBUG_TRY

	int i;

	if (m_fInRangeOfObject)
		{
		//	Get the range information

		Metric rRange;
		CSpaceObject *pCenter = GetTargetArea(pShip, &rRange);
		if (pCenter == NULL)
			return NULL;

		Metric rRange2 = rRange * rRange;
		CVector vCenter = pCenter->GetPos();

		//	Get the list of all visible enemy objects

		TArray<CSpaceObject *> Targets;
		pShip->GetVisibleEnemies(0, &Targets);

		//	Find the nearest enemy to us that is inside our range

		CSpaceObject *pBestObj = NULL;
		Metric rBestDist2;

		for (i = 0; i < Targets.GetCount(); i++)
			{
			CSpaceObject *pObj = Targets[i];

			//	Make sure this is within the target area

			Metric rDist2 = (pObj->GetPos() - vCenter).Length2();
			if (rDist2 < rRange2)
				{
				//	See if this is closer than our current candidate

				rDist2 = (pObj->GetPos() - pShip->GetPos()).Length2();
				if (pBestObj == NULL || rDist2 < rBestDist2)
					{
					pBestObj = pObj;
					rBestDist2 = rDist2;
					}
				}
			}

		return pBestObj;
		}
	else
		return pShip->GetNearestVisibleEnemy();

	DEBUG_CATCH
	}

CSpaceObject *CAttackOrder::GetTargetArea (CShip *pShip, Metric *retrRange)

//	GetTargetArea
//
//	Returns the target area that we need to clear.

	{
	if (!m_fInRangeOfObject)
		return NULL;

	auto &OrderDesc = pShip->GetCurrentOrderDesc();

	if (OrderDesc.IsIntegerOrPair())
		*retrRange = LIGHT_SECOND * OrderDesc.GetDataInteger();
	else
		*retrRange = LIGHT_SECOND * 100.0;

	return OrderDesc.GetTarget();
	}

bool CAttackOrder::IsBetterTarget (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pOldTarget, CSpaceObject *pNewTarget)

//	IsBetterTarget
//
//	Returns TRUE if pNewTarget is a better target.

	{
	//	Check to see if the new target is generically better.

	if (!Ctx.CalcIsBetterTarget(pShip, pOldTarget, pNewTarget))
		return false;

	//	If we're clearing out an area, then make sure the new target is inside
	//	the area we're trying to clear.

	if (m_fInRangeOfObject && !IsInTargetArea(pShip, pNewTarget))
		return false;

	//	New target is better

	return true;
	}

bool CAttackOrder::IsInTargetArea (CShip *pShip, CSpaceObject *pObj)

//	IsInTargetArea
//
//	Returns TRUE if pObj is in the area that we're trying to clear.

	{
	//	If we're not trying to clear an area, then pObj is always valid.

	if (!m_fInRangeOfObject)
		return true;

	//	Get the original order so we can figure out the area that we're trying
	//	to clear.

	Metric rRange;
	CSpaceObject *pCenter = GetTargetArea(pShip, &rRange);
	if (pCenter == NULL)
		return true;

	//	Check the range

	Metric rDist2 = (pObj->GetPos() - pCenter->GetPos()).Length2();
	if (rDist2 > rRange * rRange)
		return false;

	//	In range

	return true;
	}

bool CAttackOrder::MustRetreat (CShip &Ship) const

//	MustRetreat
//
//	Returns TRUE if we're so damaged that we need to retreat.

	{
	SVisibleDamage Damage;
	Ship.GetVisibleDamageDesc(Damage);

	//	If we have interior HP then we retreat when we get below 50% integrity.

	if (Damage.iHullLevel != -1)
		return (Damage.iHullLevel < 50);

	//	Otherwise, we retreat when below 25% armor integrity.

	return (Damage.iArmorLevel < 25);
	}

void CAttackOrder::OnAttacked (CShip &Ship, CAIBehaviorCtx &Ctx, CSpaceObject &AttackerObj, const SDamageCtx &Damage, bool bFriendlyFire)

//	OnAttacked
//
//	Attacked by something

	{
	DEBUG_TRY

	//	If the ship that attacked us is an enemy and it is closer than the 
	//	current target, then switch to attack it.

	if (m_fNearestTarget 
			&& !bFriendlyFire
			&& AttackerObj != m_Objs[OBJ_TARGET]
			&& IsBetterTarget(&Ship, Ctx, m_Objs[OBJ_TARGET], &AttackerObj))
		m_Objs[OBJ_TARGET] = &AttackerObj;

	DEBUG_CATCH
	}

void CAttackOrder::OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx)

//	OnBehavior
//
//	Do it

	{
	DEBUG_TRY

	if (!pShip)
		throw CException(ERR_FAIL);

	//	See if we need to retreat

	if (m_fRetreatIfNecessary
			&& pShip->IsDestinyTime(11)
			&& MustRetreat(*pShip))
		{
		pShip->CancelCurrentOrder();
		return;
		}

	//	Based on state

	switch (m_iState)
		{
		case EState::AttackingTargetAndAvoiding:
			{
			if (!m_Objs[OBJ_TARGET])
				throw CException(ERR_FAIL);

			Ctx.ImplementAttackTarget(pShip, m_Objs[OBJ_TARGET]);
			Ctx.ImplementFireOnTargetsOfOpportunity(pShip, m_Objs[OBJ_TARGET]);

			//	See if our timer has expired

			if (m_iCountdown != -1 && m_iCountdown-- == 0)
				pShip->CancelCurrentOrder();

			//	Every once in a while check to see if we've wandered near
			//	an enemy station

			else if (pShip->IsDestinyTime(41) && !Ctx.IsImmobile() && m_Objs[OBJ_TARGET]->CanThrust())
				{
				CSpaceObject *pEnemy = pShip->GetNearestEnemyStation(WANDER_SAFETY_RANGE);
				if (pEnemy 
						&& pEnemy != m_Objs[OBJ_TARGET]
						&& m_Objs[OBJ_TARGET]->GetDistance2(pEnemy) < WANDER_SAFETY_RANGE2)
					{
					m_iState = EState::AvoidingEnemyStation;
					m_Objs[OBJ_AVOID] = pEnemy;
					}
				}

			//  See if we've wandered outside our area

			else if (m_fStayInArea && pShip->IsDestinyTime(29) && m_Objs[OBJ_TARGET]->CanThrust())
				{
				Metric rDist;
				CSpaceObject *pCenter = GetTargetArea(pShip, &rDist);
				if (pCenter
						&& pShip->GetDistance(pCenter) > rDist + STAY_IN_AREA_THRESHOLD)
					{
					m_Objs[OBJ_TARGET] = GetBestTarget(pShip);
					if (m_Objs[OBJ_TARGET] == NULL)
						pShip->CancelCurrentOrder();
					}
				}

			break;
			}

		case EState::AttackingTargetAndHolding:
			{
			if (!m_Objs[OBJ_TARGET])
				throw CException(ERR_FAIL);

			Ctx.ImplementHold(pShip);
			Ctx.ImplementAttackTarget(pShip, m_Objs[OBJ_TARGET], true);
			Ctx.ImplementFireOnTargetsOfOpportunity(pShip, m_Objs[OBJ_TARGET]);

			//	See if our timer has expired

			if (m_iCountdown != -1 && m_iCountdown-- == 0)
				pShip->CancelCurrentOrder();

			break;
			}

		case EState::AvoidingEnemyStation:
			{
			if (!m_Objs[OBJ_TARGET] || !m_Objs[OBJ_AVOID])
				throw CException(ERR_FAIL);

			int iTick = pShip->GetSystem()->GetTick();
			CVector vTarget = m_Objs[OBJ_TARGET]->GetPos() - pShip->GetPos();
			Metric rTargetDist2 = vTarget.Length2();
			CVector vDest = m_Objs[OBJ_AVOID]->GetPos() - pShip->GetPos();

			//	We only spiral in/out part of the time (we leave ourselves some time to fight)

			bool bAvoid = (rTargetDist2 > THREAT_SENSOR_RANGE2) || ((iTick + pShip->GetDestiny()) % 91) > 55;
			
			if (bAvoid)
				{
				//	Orbit around the enemy station

				Metric rDestDist2 = vDest.Length2();

				const Metric rMaxDist = SAFE_ORBIT_RANGE * 1.2;
				const Metric rMinDist = SAFE_ORBIT_RANGE * 0.9;

				if (rDestDist2 > (rMaxDist * rMaxDist))
					Ctx.ImplementSpiralIn(pShip, vDest);
				else if (rDestDist2 < (rMinDist * rMinDist))
					Ctx.ImplementSpiralOut(pShip, vDest);
				}

			//	Attack target, if we can

			Ctx.ImplementAttackTarget(pShip, m_Objs[OBJ_TARGET], true);
			Ctx.ImplementFireOnTargetsOfOpportunity(pShip, m_Objs[OBJ_TARGET]);

			//	Check to see if we should do something else

			if (pShip->IsDestinyTime(23))
				{
				int iAngleToStation = VectorToPolar(vDest);

				//	If the target has left the safety of the station, 
				//	and the station is not between the ship and the target, then
				//	we stop avoiding.

				if (rTargetDist2 > WANDER_SAFETY_RANGE2
						&& Absolute(AngleBearing(VectorToPolar(vTarget), iAngleToStation)) > 45)
					{
					//	Note: We don't set stateNone because we want to preserve the timer value

					m_iState = EState::AttackingTargetAndAvoiding;
					m_Objs[OBJ_AVOID] = NULL;
					ASSERT(m_Objs[OBJ_TARGET]);
					}

				//	Otherwise, if we're attacking any target, see if there is something 
				//	else that we can tackle.

				else if (m_fNearestTarget)
					{
					CSpaceObject *pNewTarget = GetBestTarget(pShip);
					if (pNewTarget && pNewTarget != m_Objs[OBJ_TARGET])
						{
						CVector vNewTarget = pNewTarget->GetPos() - pShip->GetPos();
						Metric rNewTargetDist2 = vNewTarget.Length2();
						if (rNewTargetDist2 > WANDER_SAFETY_RANGE2
								&& Absolute(AngleBearing(VectorToPolar(vNewTarget), iAngleToStation)) > 45)
							{
							m_iState = EState::AttackingTargetAndAvoiding;
							m_Objs[OBJ_AVOID] = NULL;
							m_Objs[OBJ_TARGET] = pNewTarget;
							}
						}
					}
				}

			//	See if our timer has expired

			if (m_iCountdown != -1 && m_iCountdown-- == 0)
				pShip->CancelCurrentOrder();

			break;
			}
		}

	DEBUG_CATCH
	}

void CAttackOrder::OnBehaviorStart (CShip &Ship, CAIBehaviorCtx &Ctx, const COrderDesc &OrderDesc)

//	OnBehaviorStart
//
//	Initialize order module

	{
	DEBUG_TRY

	//	Make sure we're undocked because we're going flying

	Ctx.Undock(&Ship);

	//	If we don't have a target and we're looking for the nearest enemy, then
	//	find it now.

	CSpaceObject *pOrderTarget = OrderDesc.GetTarget();
	if (m_fNearestTarget)
		{
		pOrderTarget = GetBestTarget(&Ship);
		if (pOrderTarget == NULL)
			{
			Ship.CancelCurrentOrder();
			return;
			}
		}
	else if (pOrderTarget == NULL || pOrderTarget->IsDestroyed())
		{
		Ship.CancelCurrentOrder();
		return;
		}

#ifdef DEBUG
	if (pOrderTarget->GetSovereign() == Ship.GetSovereign()
			&& !Ship.GetSovereign()->IsEnemy(Ship.GetSovereign()))
		{
		ASSERT(false);
		}
#endif

	//	Set our state

	m_iState = (m_fHold ? EState::AttackingTargetAndHolding : EState::AttackingTargetAndAvoiding);
	m_Objs[OBJ_TARGET] = pOrderTarget;
	m_Objs[OBJ_AVOID] = NULL;
	ASSERT(m_Objs[OBJ_TARGET]);
	ASSERT(m_Objs[OBJ_TARGET]->DebugIsValid() && m_Objs[OBJ_TARGET]->NotifyOthersWhenDestroyed());

	//	See if we have a time limit

	DWORD dwTimer = (m_fInRangeOfObject ? OrderDesc.GetDataInteger2() : OrderDesc.GetDataInteger());
	m_iCountdown = (dwTimer ? 1 + (g_TicksPerSecond * dwTimer) : -1);

	DEBUG_CATCH
	}

CString CAttackOrder::OnDebugCrashInfo (void)

//	OnDebugCrashInfo
//
//	Output crash information

	{
	CString sResult;

	sResult.Append(CONSTLIT("CAttackOrder\r\n"));
	sResult.Append(strPatternSubst(CONSTLIT("m_State: %d\r\n"), m_iState));
	sResult.Append(strPatternSubst(CONSTLIT("m_iCountdown: %d\r\n"), m_iCountdown));

	return sResult;
	}

void CAttackOrder::OnObjDestroyed (CShip *pShip, const SDestroyCtx &Ctx, int iObj, bool *retbCancelOrder)

//	OnObjDestroyed
//
//	Notification that an object was destroyed

	{
	//	If the object we're avoiding was destroyed

	if (iObj == OBJ_AVOID)
		{
		//	No need to avoid anymore. Reset our state

		if (m_iState == EState::AvoidingEnemyStation)
			m_iState = EState::AttackingTargetAndAvoiding;
		ASSERT(m_Objs[OBJ_TARGET]);
		}

	//	If our target was destroyed and we need to attack the nearest
	//	target, then go for it.

	else if (m_fNearestTarget && iObj == OBJ_TARGET)
		{
		CSpaceObject *pNewTarget = GetBestTarget(pShip);
		if (pNewTarget == NULL)
			{
			*retbCancelOrder = true;
			return;
			}

		m_Objs[OBJ_TARGET] = pNewTarget;
		}
	}

void CAttackOrder::OnReadFromStream (SLoadCtx &Ctx, const COrderDesc &OrderDesc)

//	OnReadFromStream
//
//	Load data from saved game

	{
	DWORD dwLoad;

	//	Because of a bug, old versions did not save m_iState

	if (Ctx.dwVersion >= 76)
		{
		Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
		m_iState = (EState)dwLoad;
		}
	else
		{
		if (m_Objs[OBJ_AVOID])
			m_iState = EState::AvoidingEnemyStation;
		else
			m_iState = EState::AttackingTargetAndAvoiding;
		}

	//	Read the rest

	Ctx.pStream->Read((char *)&m_iCountdown, sizeof(DWORD));
	}

void CAttackOrder::OnWriteToStream (IWriteStream *pStream) const

//	OnWriteToStream
//
//	Write data to saved game

	{
	DWORD dwSave;

	dwSave = (DWORD)m_iState;
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	pStream->Write((char *)&m_iCountdown, sizeof(DWORD));
	}

