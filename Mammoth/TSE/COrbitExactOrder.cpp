//	COrbitExactOrder.cpp
//
//	COrbitExactOrder class
//	Copyright (c) 2020 Kronosaur Productions. All Rights Reserved.

#include "PreComp.h"

bool COrbitExactOrder::IsAutoAngle (const COrderDesc &OrderDesc, Metric *retrAngleInRadians)

//	IsAutoAngle
//
//	Returns TRUE if we're set for auto-angle.

	{
	if (OrderDesc.IsCCItem())
		{
		ICCItemPtr pData = OrderDesc.GetDataCCItem();

		if (const ICCItem *pAngle = pData->GetElement(CONSTLIT("angle")))
			{
			if (retrAngleInRadians)
				*retrAngleInRadians = ::mathDegreesToRadians(pAngle->GetDoubleValue());
			return false;
			}
		else
			return true;
		}
	else
		return true;
	}

void COrbitExactOrder::OnAttacked (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pAttacker, const SDamageCtx &Damage, bool bFriendlyFire)

//	OnAttacked
//
//	Ship was attacked while executing order.

	{
	}

void COrbitExactOrder::OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx)

//	OnBehavior
//
//	Handle behavior

	{
	constexpr Metric MAX_THRESHOLD_ADJ = 1.1;
	constexpr Metric MAX_THRESHOLD_ADJ2 = MAX_THRESHOLD_ADJ * MAX_THRESHOLD_ADJ;

	//	Compute the desired position.

	Metric rAngle = ::mathDegreesToRadians(pShip->GetUniverse().GetTicks() * m_rAngularSpeed) + m_Orbit.GetObjectAngle();
	CVector vOrbitPos = m_Orbit.GetPoint(rAngle);
	CVector vPos = m_Objs[objBase]->GetPos() + vOrbitPos;
	CVector vVel = (m_Orbit.GetPoint(rAngle + ::mathDegreesToRadians(m_rAngularSpeed)) - vOrbitPos) / g_SecondsPerUpdate;

	//	If the ship is out of position and needs to be placed back in orbit, 
	//	then we try to ease in.

	const Metric rOrbitSpeed2 = vVel.Length2();
	const Metric rMaxPosChange2 = rOrbitSpeed2 * MAX_THRESHOLD_ADJ2 * g_SecondsPerUpdate * g_SecondsPerUpdate;
	const CVector vPosChange = vPos - pShip->GetPos();
	const Metric rPosChange2 = vPosChange.Length2();

	//	Maneuver

	if (rPosChange2 > rMaxPosChange2)
		{
		const Metric rPosChange = sqrt(rPosChange2);
		const Metric rOrbitSpeed = sqrt(rOrbitSpeed2);

		const Metric EASE_FACTOR = 0.5;
		const Metric rMaxEaseSpeed = Max(rOrbitSpeed, pShip->GetMaxSpeed());
		const Metric rEaseSpeed = Min(EASE_FACTOR * rPosChange / g_SecondsPerUpdate, rMaxEaseSpeed);

		const CVector vPosDir = vPosChange / rPosChange;

		CVector vEaseVel = rEaseSpeed * vPosDir;
		CVector vEasePos = pShip->GetPos() + (g_SecondsPerUpdate * vEaseVel);

		pShip->Place(vEasePos, vEaseVel);
		}
	else
		pShip->Place(vPos, vVel);

	Ctx.ImplementAttackNearestTarget(pShip, Ctx.GetBestWeaponRange(), &m_Objs[objTarget], NULL, true);
	Ctx.ImplementFireOnTargetsOfOpportunity(pShip, m_Objs[objTarget]);

	if (!m_Objs[objTarget])
		{
		Ctx.ImplementTurnTo(pShip, VectorToPolar(vVel));
		}

	//	See if our timer has expired

	if (m_iCountdown != -1 && m_iCountdown-- == 0)
		pShip->CancelCurrentOrder();
	}

void COrbitExactOrder::OnBehaviorStart (CShip &Ship, CAIBehaviorCtx &Ctx, const COrderDesc &OrderDesc)

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

	//	Compute the order from our data

	DWORD dwRadius = OrderDesc.GetDataInteger(CONSTLIT("radius"), true, DEFAULT_RADIUS);
	m_Orbit.SetSemiMajorAxis(dwRadius * LIGHT_SECOND);
	m_Orbit.SetEccentricity(OrderDesc.GetDataDouble(CONSTLIT("eccentricity"), 0.0));
	m_rAngularSpeed = OrderDesc.GetDataDouble(CONSTLIT("speed"), DEFAULT_SPEED);

	Metric rAngle;
	if (IsAutoAngle(OrderDesc, &rAngle))
		{
		TArray<CShip *> OrbitMates = GetOrbitMates(*pOrderTarget, dwRadius);
		DistributeOrbitAngles(Ship, *pOrderTarget, OrbitMates);
		}
	else
		{
		m_Orbit.SetObjectAngle(rAngle);
		}

	m_iCountdown = OrderDesc.GetDataTicksLeft();
	}

DWORD COrbitExactOrder::OnCommunicate (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2, ICCItem *pData)

//	OnCommunicate
//
//	Process communications.

	{
	switch (iMessage)
		{
		case msgAttackDeter:
			m_Objs[objTarget] = pParam1;
			return resAck;

		case msgBaseDestroyedByTarget:
			{
			if (pParam1 == NULL
					|| pParam1->IsDestroyed()
					|| Ctx.IsNonCombatant())
				return resNoAnswer;

			m_Objs[objTarget] = pParam1;
			return resAck;
			}

		case msgFormUp:
			{
			DWORD dwAngle = dwParam2;
			m_Orbit.SetObjectAngle(::mathDegreesToRadians((Metric)dwAngle));

			return resAck;
			}

		default:
			return resNoAnswer;
		}
	}

void COrbitExactOrder::OnObjDestroyed (CShip *pShip, const SDestroyCtx &Ctx, int iObj, bool *retbCancelOrder)

//	OnObjDestroyed
//
//	An object was destroyed.

	{
	}

void COrbitExactOrder::OnReadFromStream (SLoadCtx &Ctx)

//	OnReadFromStream
//
//	Read from stream

	{
	m_Orbit.ReadFromStream(Ctx);
	Ctx.pStream->Read(m_dwStartTick);
	Ctx.pStream->Read(m_rAngularSpeed);
	Ctx.pStream->Read(m_iCountdown);
	}

void COrbitExactOrder::OnWriteToStream (CSystem *pSystem, IWriteStream *pStream)

//	OnWriteToStream
//
//	Write to stream

	{
	m_Orbit.WriteToStream(*pStream);
	pStream->Write(m_dwStartTick);
	pStream->Write(m_rAngularSpeed);
	pStream->Write(m_iCountdown);
	}

TArray<CShip *> COrbitExactOrder::GetOrbitMates (CSpaceObject &Source, DWORD dwRadius)

//	GetOrbitMates
//
//	Returns a list of all ships orbiting the given source at the given radius.

	{
	CSystem &System = *Source.GetSystem();

	TArray<CShip *> Result;
	for (int i = 0; i < System.GetObjectCount(); i++)
		{
		CSpaceObject *pObj = System.GetObject(i);
		if (!pObj)
			continue;

		CShip *pShip = pObj->AsShip();
		if (!pShip)
			continue;

		const COrderDesc &OrderDesc = pShip->GetCurrentOrderDesc();
		if (OrderDesc.GetOrder() != IShipController::orderOrbitExact)
			continue;

		if (OrderDesc.GetTarget() != Source)
			continue;

		if (!IsAutoAngle(OrderDesc) || OrderDesc.GetDataInteger(CONSTLIT("radius"), true, DEFAULT_RADIUS) != dwRadius)
			continue;

		Result.Insert(pShip);
		}

	return Result;
	}

void COrbitExactOrder::DistributeOrbitAngles (CShip &Ship, CSpaceObject &Source, const TArray<CShip *> &Ships)

//	DistributeAngles
//
//	Distribute all other ships in the same orbit.

	{
	if (!Ships.GetCount())
		return;

	const Metric rInterval = 360.0 / Ships.GetCount();
	Metric rAngle = 0.0;
	for (int i = 0; i < Ships.GetCount(); i++)
		{
		DWORD dwAngle = mathRound(rAngle);
		if (Ships[i] == Ship)
			m_Orbit.SetObjectAngle(::mathDegreesToRadians((Metric)dwAngle));
		else
			Source.Communicate(Ships[i], msgFormUp, &Source, dwAngle);

		rAngle += rInterval;
		}
	}
