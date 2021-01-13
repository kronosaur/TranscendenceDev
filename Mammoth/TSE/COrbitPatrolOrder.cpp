//	COrbitPatrolOrder.cpp
//
//	COrbitPatrolOrder class
//	Copyright (c) 2021 Kronosaur Productions. All Rights Reserved.

#include "PreComp.h"

void COrbitPatrolOrder::BehaviorAttacking (CShip &Ship, CAIBehaviorCtx &Ctx)

//	BehaviorAttacking
//
//	Ship is attacking m_Objs[objTarget].

	{
	Ctx.ImplementAttackTarget(&Ship, m_Objs[objTarget]);
	Ctx.ImplementFireOnTargetsOfOpportunity(&Ship, m_Objs[objTarget]);

	//	Check to see if we've wandered outside our patrol zone

	if (Ship.IsDestinyTime(20))
		{
		const Metric rMaxRange = Max(m_Orbit.GetSemiMajorAxis() + PATROL_SENSOR_RANGE, STOP_ATTACK_RANGE);
		const Metric rMaxRange2 = rMaxRange * rMaxRange;

		const Metric rMinRange = Max(0.0, m_Orbit.GetSemiMajorAxis() - PATROL_SENSOR_RANGE);
		const Metric rMinRange2 = rMinRange * rMinRange;

		const CVector vRange = m_Objs[objBase]->GetPos() - Ship.GetPos();
		const Metric rDistance2 = vRange.Length2();

		//	If we're outside of our patrol range and if we haven't
		//	been hit in a while then stop the attack

		if ((rDistance2 > rMaxRange2 || rDistance2 < rMinRange2)
				&& !Ctx.IsBeingAttacked())
			{
			m_Objs[objTarget] = NULL;
			m_iState = EState::Orbiting;
			}
		}
	}

void COrbitPatrolOrder::BehaviorOrbiting (CShip &Ship, CAIBehaviorCtx &Ctx)

//	BehaviorOrbiting
//
//	Ship is orbiting pricinple, waiting for enemies.

	{
	constexpr Metric MAX_THRESHOLD_ADJ = 1.1;
	constexpr Metric MAX_THRESHOLD_ADJ2 = MAX_THRESHOLD_ADJ * MAX_THRESHOLD_ADJ;

	//	If we're too far away, then use a nav path to get closer

	if ((Ship.GetPos() - m_Objs[objBase]->GetPos()).Length2() > m_rNavThreshold2)
		{
		Ship.AddOrder(COrderDesc(IShipController::orderApproach, m_Objs[objBase], mathRound(m_Orbit.GetSemiMajorAxis() / LIGHT_SECOND)), true);
		return;
		}

	//	Compute the desired position.

	Metric rAngle = ::mathDegreesToRadians(Ship.GetUniverse().GetTicks() * m_rAngularSpeed) + m_Orbit.GetObjectAngle();
	CVector vOrbitPos = m_Orbit.GetPoint(rAngle);
	CVector vPos = m_Objs[objBase]->GetPos() + vOrbitPos;
	CVector vVel = (m_Orbit.GetPoint(rAngle + ::mathDegreesToRadians(m_rAngularSpeed)) - vOrbitPos) / g_SecondsPerUpdate;

	//	If the ship is out of position and needs to be placed back in orbit, 
	//	then we try to ease in.

	const Metric rOrbitSpeed2 = vVel.Length2();
	const Metric rMaxPosChange2 = rOrbitSpeed2 * MAX_THRESHOLD_ADJ2 * g_SecondsPerUpdate * g_SecondsPerUpdate;
	const CVector vPosChange = vPos - Ship.GetPos();
	const Metric rPosChange2 = vPosChange.Length2();

	//	Maneuver

	if (rPosChange2 > rMaxPosChange2)
		{
		const Metric rPosChange = sqrt(rPosChange2);
		const Metric rOrbitSpeed = sqrt(rOrbitSpeed2);

		const Metric EASE_FACTOR = 0.5;
		const Metric rMaxEaseSpeed = Max(rOrbitSpeed, Ship.GetMaxSpeed());
		const Metric rEaseSpeed = Min(EASE_FACTOR * rPosChange / g_SecondsPerUpdate, rMaxEaseSpeed);

		const CVector vPosDir = vPosChange / rPosChange;

		CVector vEaseVel = rEaseSpeed * vPosDir;
		CVector vEasePos = Ship.GetPos() + (g_SecondsPerUpdate * vEaseVel);

		Ship.Place(vEasePos, vEaseVel);
		}
	else
		Ship.Place(vPos, vVel);

	Ctx.ImplementAttackNearestTarget(&Ship, Ctx.GetBestWeaponRange(), &m_Objs[objTarget], NULL, true);
	Ctx.ImplementFireOnTargetsOfOpportunity(&Ship, m_Objs[objTarget]);

	if (!m_Objs[objTarget])
		{
		Ctx.ImplementTurnTo(&Ship, VectorToPolar(vVel));
		}

	//	Check to see if any enemy ships appear

	if (Ship.IsDestinyTime(30))
		{
		CSpaceObject *pTarget = Ship.GetVisibleEnemyInRange(m_Objs[objBase], PATROL_SENSOR_RANGE, false, m_Objs[objBase]);
		if (pTarget)
			{
			m_Objs[objTarget] = pTarget;
			m_iState = EState::Attacking;
			}
		}
	}

void COrbitPatrolOrder::CalcIntermediates ()

//	CalcIntermediates
//
//	Initializes cached calculations:
//
//	m_rNavThreshold2

	{
	static constexpr Metric RADIUS_LIMIT_ADJ = 5.0 * LIGHT_SECOND;
	Metric rRadiusLimit = m_Orbit.GetSemiMajorAxis() + RADIUS_LIMIT_ADJ;
	Metric rRadiusLimit2 = rRadiusLimit * rRadiusLimit;

	m_rNavThreshold2 = Max(rRadiusLimit2, NAV_PATH_THRESHOLD2);
	}

void COrbitPatrolOrder::OnAttacked (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pAttacker, const SDamageCtx &Damage, bool bFriendlyFire)

//	OnAttacked
//
//	Ship was attacked while executing order.

	{
	//	If we have a valid attacker, then we try to respond. We ignore friendly
	//	fire because that's handled by CBaseShipAI.

	if (pAttacker == NULL
			|| !pAttacker->CanAttack()
			|| bFriendlyFire)
		return;

	switch (m_iState)
		{
		case EState::Orbiting:

			m_Objs[objTarget] = pAttacker;
			m_iState = EState::Attacking;

			//	If we were attacked twice (excluding multi-shot weapons)
			//	then we tell our station about this

			CSpaceObject *pOrderGiver = Damage.GetOrderGiver();
			CSpaceObject *pTarget;
			if (Ctx.IsSecondAttack()
					&& m_Objs[objBase]
					&& m_Objs[objBase]->IsAngryAt(pAttacker)
					&& (pTarget = m_Objs[objBase]->CalcTargetToAttack(pAttacker, pOrderGiver)))
				pShip->Communicate(m_Objs[objBase], msgAttackDeter, pTarget);

			break;
		}
	}

void COrbitPatrolOrder::OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx)

//	OnBehavior
//
//	Handle behavior

	{
	switch (m_iState)
		{
		case EState::Orbiting:
			BehaviorOrbiting(*pShip, Ctx);
			break;

		case EState::Attacking:
			BehaviorAttacking(*pShip, Ctx);
			break;

		default:
			throw CException(ERR_FAIL);
		}

	//	See if our timer has expired

	if (m_iCountdown != -1 && m_iCountdown-- == 0)
		pShip->CancelCurrentOrder();
	}

void COrbitPatrolOrder::OnBehaviorStart (CShip &Ship, CAIBehaviorCtx &Ctx, const COrderDesc &OrderDesc)

//	OnBehaviorStart
//
//	Start

	{
	//	Make sure we're undocked because we're going flying

	Ctx.Undock(&Ship);

	//	Must have a center

	CSpaceObject *pOrderTarget = OrderDesc.GetTarget();
	if (pOrderTarget)
		m_Objs[objBase] = pOrderTarget;
	else
		{
		Ship.CancelCurrentOrder();
		return;
		}

	//	Initialize

	m_iState = EState::Orbiting;

	//	Compute the order from our data

	Metric rRadiusLS = COrbitExactOrder::CalcRadiusInLightSeconds(OrderDesc);
	m_Orbit.SetSemiMajorAxis(rRadiusLS * LIGHT_SECOND);
	m_Orbit.SetEccentricity(OrderDesc.GetDataDouble(CONSTLIT("eccentricity"), 0.0));
	m_rAngularSpeed = OrderDesc.GetDataDouble(CONSTLIT("speed"), COrbitExactOrder::DEFAULT_SPEED);

	Metric rAngle;
	if (COrbitExactOrder::IsAutoAngle(OrderDesc, &rAngle))
		{
		TArray<CShip *> OrbitMates = COrbitExactOrder::GetOrbitMates(*pOrderTarget, mathRound(rRadiusLS));

		COrbitExactOrder::DistributeOrbitAngles(Ship, *pOrderTarget, OrbitMates, rAngle);
		m_Orbit.SetObjectAngle(rAngle);
		}
	else
		{
		m_Orbit.SetObjectAngle(rAngle);
		}

	m_iCountdown = OrderDesc.GetDataTicksLeft();

	//	Cache some calculations

	CalcIntermediates();
	}

DWORD COrbitPatrolOrder::OnCommunicate (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2, ICCItem *pData)

//	OnCommunicate
//
//	Process communications.

	{
	switch (iMessage)
		{
		case msgAbort:
			if (m_iState == EState::Attacking)
				{
				m_Objs[objTarget] = NULL;
				m_iState = EState::Orbiting;
				return resAck;
				}
			else
				return resNoAnswer;

		case msgAttack:
		case msgAttackDeter:
			if (pShip->GetDistance2(pParam1) < PATROL_DETER_RANGE2)
				{
				m_Objs[objTarget] = pParam1;
				m_iState = EState::Attacking;
				return resAck;
				}
			else
				return resNoAnswer;

		case msgDestroyBroadcast:
		case msgBaseDestroyedByTarget:
			{
			if (pParam1 == NULL
					|| pParam1->IsDestroyed()
					|| Ctx.IsNonCombatant())
				return resNoAnswer;

			pShip->AddOrder(COrderDesc(IShipController::orderDestroyTarget, pParam1), true);
			return resAck;
			}

		case msgFormUp:
			{
			DWORD dwAngle = dwParam2;
			m_Orbit.SetObjectAngle(::mathDegreesToRadians((Metric)dwAngle));

			return resAck;
			}

		case msgQueryAttackStatus:
			{
			switch (m_iState)
				{
				case EState::Attacking:
					return resAck;

				default:
					return resNoAnswer;
				}
			}

		default:
			return resNoAnswer;
		}
	}

void COrbitPatrolOrder::OnDestroyed (CShip *pShip, SDestroyCtx &Ctx)

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

void COrbitPatrolOrder::OnObjDestroyed (CShip *pShip, const SDestroyCtx &Ctx, int iObj, bool *retbCancelOrder)

//	OnObjDestroyed
//
//	An object was destroyed.

	{
	if (iObj == objTarget)
		{
		switch (m_iState)
			{
			case EState::Attacking:
				{
				//	If a friend destroyed our target then thank them

				if (Ctx.Attacker.IsCausedByFriendOf(pShip) && Ctx.Attacker.GetObj())
					pShip->Communicate(Ctx.Attacker.GetObj(), msgNiceShooting);

				//	Back to orbiting

				m_iState = EState::Orbiting;
				break;
				}
			}
		}
	else if (iObj == objBase)
		{
		//	If we're station guards, and our station got destroyed, then 
		//	attack the nearest enemy. If we could retaliate against the
		//	attacker, we would have gotten msgBaseDestroyedByTarget. The fact
		//	that we're here means that we never got the message, probably 
		//	because the target is out of range or already dead.

		pShip->CancelCurrentOrder();
		pShip->AddOrder(COrderDesc(IShipController::orderAttackNearestEnemy));
		}
	}

void COrbitPatrolOrder::OnReadFromStream (SLoadCtx &Ctx)

//	OnReadFromStream
//
//	Read from stream

	{
	DWORD dwLoad;
	Ctx.pStream->Read(dwLoad);
	m_iState = (EState)dwLoad;

	m_Orbit.ReadFromStream(Ctx);
	Ctx.pStream->Read(m_rAngularSpeed);
	Ctx.pStream->Read(m_iCountdown);

	CalcIntermediates();
	}

void COrbitPatrolOrder::OnWriteToStream (CSystem *pSystem, IWriteStream *pStream)

//	OnWriteToStream
//
//	Write to stream

	{
	DWORD dwSave = (DWORD)m_iState;
	pStream->Write(dwSave);

	m_Orbit.WriteToStream(*pStream);
	pStream->Write(m_rAngularSpeed);
	pStream->Write(m_iCountdown);
	}
