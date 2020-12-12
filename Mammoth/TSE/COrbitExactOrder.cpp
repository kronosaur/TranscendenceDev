//	COrbitExactOrder.cpp
//
//	COrbitExactOrder class
//	Copyright (c) 2020 Kronosaur Productions. All Rights Reserved.

#include "PreComp.h"

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
	//	Compute the desired position.

	Metric rAngle = ::mathDegreesToRadians(pShip->GetUniverse().GetTicks() * m_rAngularSpeed) + m_Orbit.GetObjectAngle();
	CVector vOrbitPos = m_Orbit.GetPoint(rAngle);
	CVector vPos = m_Objs[objBase]->GetPos() + vOrbitPos;
	CVector vVel = (m_Orbit.GetPoint(rAngle + ::mathDegreesToRadians(m_rAngularSpeed)) - vOrbitPos) / g_SecondsPerUpdate;

	//	Maneuver

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

void COrbitExactOrder::OnBehaviorStart (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pOrderTarget, const IShipController::SData &Data)

//	OnBehaviorStart
//
//	Start

	{
	//	Make sure we're undocked because we're going flying

	Ctx.Undock(pShip);

	//	Must have a center

	if (pOrderTarget)
		m_Objs[objBase] = pOrderTarget;
	else
		{
		pShip->CancelCurrentOrder();
		return;
		}

	//	Compute the order from our data

	if (Data.iDataType == IShipController::dataOrbitExact)
		{
		DWORD dwRadius = LOWORD(Data.dwData1);
		DWORD dwAngle = HIWORD(Data.dwData1);

		m_Orbit.SetSemiMajorAxis(dwRadius * LIGHT_SECOND);
		m_Orbit.SetObjectAngle(::mathDegreesToRadians((Metric)dwAngle));
		m_Orbit.SetEccentricity(Data.vData.GetY());

		m_rAngularSpeed = Data.vData.GetX();
		}
	else
		{
		DWORD dwRadius = Data.AsInteger();
		if (dwRadius == 0)
			dwRadius = 10;

		m_Orbit.SetSemiMajorAxis(dwRadius * LIGHT_SECOND);
		m_Orbit.SetObjectAngle(0.0);
		m_Orbit.SetEccentricity(0.0);

		m_rAngularSpeed = 2.0;
		}

	//	Set our timer

	DWORD dwTimer = Data.dwData2;
	m_iCountdown = (dwTimer ? 1 + (g_TicksPerSecond * dwTimer) : -1);
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
