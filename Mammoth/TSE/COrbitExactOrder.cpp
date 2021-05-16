//	COrbitExactOrder.cpp
//
//	COrbitExactOrder class
//	Copyright (c) 2020 Kronosaur Productions. All Rights Reserved.

#include "PreComp.h"

void COrbitExactOrder::CalcIntermediates (const COrderDesc &OrderDesc)

//	CalcIntermediates
//
//	Initializes cached calculations:
//
//	m_rNavThreshold2

	{
	static constexpr Metric RADIUS_LIMIT_ADJ = 5.0 * LIGHT_SECOND;
	Metric rRadiusLimit = m_Orbit.GetSemiMajorAxis() + RADIUS_LIMIT_ADJ;
	Metric rRadiusLimit2 = rRadiusLimit * rRadiusLimit;

	m_rThreatRange = OrderDesc.GetDataDouble(CONSTLIT("threatRange"), DEFAULT_THREAT_RANGE_LS) * LIGHT_SECOND;
	m_rThreatStopRange = OrderDesc.GetDataDouble(CONSTLIT("threatStopRange"), DEFAULT_THREAT_STOP_RANGE_LS) * LIGHT_SECOND;

	m_rNavThreshold2 = Max(rRadiusLimit2, NAV_PATH_THRESHOLD2);
	}

Metric COrbitExactOrder::CalcRadiusInLightSeconds (const COrderDesc &OrderDesc)

//	CalcRadiusInLightSeconds
//
//	Returns the orbit radius in light-seconds. NOTE: This is not in kilometers,
//	like most metric distances. Callers must still multiply by LIGHT_SECOND.

	{
	CString sScale;
	DiceRange RadiusRange = OrderDesc.GetDataDiceRange(CONSTLIT("radius"), DEFAULT_RADIUS, &sScale);

	Metric rRadiusKM = (Metric)RadiusRange.Roll() * CSystemType::ParseScale(sScale);
	return rRadiusKM / LIGHT_SECOND;
	}

bool COrbitExactOrder::IsAutoAngle (const COrderDesc &OrderDesc, Metric *retrAngleInRadians)

//	IsAutoAngle
//
//	Returns TRUE if we're set for auto-angle.

	{
	DiceRange AngleRange = OrderDesc.GetDataDiceRange(CONSTLIT("angle"), -1);
	if (AngleRange.IsConstant() && AngleRange.GetBonus() == -1)
		return true;

	if (retrAngleInRadians)
		*retrAngleInRadians = ::mathDegreesToRadians(AngleRange.Roll());

	return false;
	}

void COrbitExactOrder::OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx)

//	OnBehavior
//
//	Handle behavior

	{
	if (!pShip)
		throw CException(ERR_FAIL);

		//	See if our timer has expired

	else if (m_iCountdown != -1 && m_iCountdown-- == 0)
		{
		pShip->CancelCurrentOrder();
		}

	//	If we're too far away, then use a nav path to get closer.
	//	NOTE: We guarantee that NavThreshold is always greater than orbit 
	//	radius.

	else if ((pShip->GetPos() - m_Objs[OBJ_BASE]->GetPos()).Length2() > m_rNavThreshold2)
		{
		pShip->AddOrder(COrderDesc(IShipController::orderApproach, m_Objs[OBJ_BASE], mathRound(m_Orbit.GetSemiMajorAxis() / LIGHT_SECOND)), true);
		}

	//	Every once in a while we check to see if we need to dock with our base
	//	to resupply.

	else if (pShip->IsDestinyTime(RESUPPLY_CHECK_TIME) 
			&& Ctx.ImplementResupplyCheck(*pShip, *m_Objs[OBJ_BASE]))
		{ }

	//	Otherwise, orbit

	else
		{
		constexpr Metric MAX_THRESHOLD_ADJ = 1.1;
		constexpr Metric MAX_THRESHOLD_ADJ2 = MAX_THRESHOLD_ADJ * MAX_THRESHOLD_ADJ;

		//	Compute the desired position.

		Metric rAngle = ::mathDegreesToRadians(pShip->GetUniverse().GetTicks() * m_rAngularSpeed) + m_Orbit.GetObjectAngle();
		CVector vOrbitPos = m_Orbit.GetPoint(rAngle);
		CVector vPos = m_Objs[OBJ_BASE]->GetPos() + vOrbitPos;
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

		if (!pShip->GetTarget())
			{
			Ctx.ImplementTurnTo(pShip, VectorToPolar(vVel));
			}
		}
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
		m_Objs[OBJ_BASE] = pOrderTarget;
	else
		{
		Ship.CancelCurrentOrder();
		return;
		}

	//	Compute the order from our data

	Metric rRadiusLS = CalcRadiusInLightSeconds(OrderDesc);
	m_Orbit.SetSemiMajorAxis(rRadiusLS * LIGHT_SECOND);
	m_Orbit.SetEccentricity(OrderDesc.GetDataDouble(CONSTLIT("eccentricity"), 0.0));
	m_rAngularSpeed = OrderDesc.GetDataDouble(CONSTLIT("speed"), DEFAULT_SPEED);

	Metric rAngle;
	if (IsAutoAngle(OrderDesc, &rAngle))
		{
		TArray<CShip *> OrbitMates = GetOrbitMates(*pOrderTarget, mathRound(rRadiusLS));

		DistributeOrbitAngles(Ship, *pOrderTarget, OrbitMates, rAngle);
		m_Orbit.SetObjectAngle(rAngle);
		}
	else
		{
		m_Orbit.SetObjectAngle(rAngle);
		}

	m_iCountdown = OrderDesc.GetDataTicksLeft();

	//	Cache some calculations

	CalcIntermediates(OrderDesc);
	}

DWORD COrbitExactOrder::OnCommunicate (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2, ICCItem *pData)

//	OnCommunicate
//
//	Process communications.

	{
	switch (iMessage)
		{
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

void COrbitExactOrder::OnDestroyed (CShip *pShip, SDestroyCtx &Ctx)

//	OnDestroyed
//
//	We've been destroyed.

	{
	//	If we've been destroyed, then ask our station to avenge us

	if (m_Objs[OBJ_BASE]
			&& !m_Objs[OBJ_BASE]->IsEnemy(pShip)
			&& !m_Objs[OBJ_BASE]->IsDestroyed())
		m_Objs[OBJ_BASE]->OnSubordinateDestroyed(Ctx);
	}

AIReaction COrbitExactOrder::OnGetReactToAttack () const

//	OnGetReactToAttack
//
//	Returns our default reaction to an attack.

	{
	switch (m_iOrder)
		{
		case IShipController::orderOrbitExact:
			return AIReaction::Deter;

		case IShipController::orderOrbitPatrol:
			return AIReaction::Chase;

		default:
			throw CException(ERR_FAIL);
		}
	}

AIReaction COrbitExactOrder::OnGetReactToBaseDestroyed () const

//	OnGetReactToBaseDestroyed
//
//	Returns our default reaction to our base getting destroyed.

	{
	switch (m_iOrder)
		{
		case IShipController::orderOrbitExact:
			return AIReaction::Destroy;

		case IShipController::orderOrbitPatrol:
			return AIReaction::DestroyAndRetaliate;

		default:
			throw CException(ERR_FAIL);
		}
	}

AIReaction COrbitExactOrder::OnGetReactToThreat () const

//	OnGetReactToThreat
//
//	Returns our default reaction to a threat.

	{
	switch (m_iOrder)
		{
		case IShipController::orderOrbitExact:
			return AIReaction::Deter;

		case IShipController::orderOrbitPatrol:
			return AIReaction::Chase;

		default:
			throw CException(ERR_FAIL);
		}
	}

Metric COrbitExactOrder::OnGetThreatStopRange (void) const

//	OnGetThreatStopRange
//
//	Returns the range at which we should stop chasing threats.

	{
	return Max(m_Orbit.GetSemiMajorAxis() + GetThreatRange(), m_rThreatStopRange);
	}

void COrbitExactOrder::OnObjDestroyed (CShip *pShip, const SDestroyCtx &Ctx, int iObj, bool *retbCancelOrder)

//	OnObjDestroyed
//
//	An object was destroyed.

	{
	}

void COrbitExactOrder::OnReadFromStream (SLoadCtx &Ctx, const COrderDesc &OrderDesc)

//	OnReadFromStream
//
//	Read from stream

	{
	m_Orbit.ReadFromStream(Ctx);
	Ctx.pStream->Read(m_dwStartTick);
	Ctx.pStream->Read(m_rAngularSpeed);
	Ctx.pStream->Read(m_iCountdown);

	CalcIntermediates(OrderDesc);
	}

void COrbitExactOrder::OnWriteToStream (IWriteStream *pStream) const

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

void COrbitExactOrder::DistributeOrbitAngles (CShip &Ship, CSpaceObject &Source, const TArray<CShip *> &Ships, Metric &retrShipAngle)

//	DistributeAngles
//
//	Distribute all other ships in the same orbit.

	{
	if (!Ships.GetCount())
		{
		retrShipAngle = 0.0;
		return;
		}

	const Metric rInterval = 360.0 / Ships.GetCount();
	Metric rAngle = 0.0;
	for (int i = 0; i < Ships.GetCount(); i++)
		{
		DWORD dwAngle = mathRound(rAngle);
		if (Ships[i] == Ship)
			{
			//	Return the angle desired for the ship.
			retrShipAngle = ::mathDegreesToRadians((Metric)dwAngle);
			}
		else
			Source.Communicate(Ships[i], msgFormUp, &Source, dwAngle);

		rAngle += rInterval;
		}
	}
