//	CNavigateOrder.cpp
//
//	CNavigateOrder class implementation
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

const Metric APPROACH_THRESHOLD =		(20.0 * LIGHT_SECOND);
const Metric APPROACH_THRESHOLD2 =		(APPROACH_THRESHOLD * APPROACH_THRESHOLD);

const Metric PATROL_SENSOR_RANGE =		(30.0 * LIGHT_SECOND);
const Metric NAV_PATH_THRESHOLD =		(2.0 * PATROL_SENSOR_RANGE);
const Metric NAV_PATH_THRESHOLD2 =		(NAV_PATH_THRESHOLD * NAV_PATH_THRESHOLD);

CNavigateOrder::CNavigateOrder (IShipController::OrderTypes iOrder) : IOrderModule(objCount),
		m_iOrder(iOrder)

//	CNavigateOrder constructor

	{
	switch (m_iOrder)
		{
		case IShipController::orderGoToPos:
			m_fTargetVector = true;
			m_fTargetObj = false;
			break;

		default:
			ASSERT(false);
		}
	}

void CNavigateOrder::OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx)

//	OnBehavior
//
//	Behavior

	{
	switch (m_iState)
		{
		case stateOnCourseViaNavPath:
			{
			Ctx.ImplementAttackNearestTarget(pShip, Ctx.GetBestWeaponRange(), &m_Objs[objTarget]);
			Ctx.ImplementFireOnTargetsOfOpportunity(pShip, m_Objs[objTarget]);

			bool bAtDest;
			Ctx.ImplementFollowNavPath(pShip, &bAtDest);
			if (bAtDest)
				{
				Ctx.ClearNavPath();
				m_iState = stateApproachingPos;
				}

			break;
			}

		case stateApproachingPos:
			{
			Ctx.ImplementAttackNearestTarget(pShip, Ctx.GetBestWeaponRange(), &m_Objs[objTarget]);
			Ctx.ImplementFireOnTargetsOfOpportunity(pShip, m_Objs[objTarget]);

			//	Maneuver

			CVector vTarget = m_vDest - pShip->GetPos();
			Metric rTargetDist2 = vTarget.Dot(vTarget);

			//	If we're close enough, we're done

			if (rTargetDist2 < m_rMinDist2)
				pShip->CancelCurrentOrder();

			//	Otherwise, we use formation flying

			else
				Ctx.ImplementFormationManeuver(pShip, m_vDest, NullVector, m_iDestFacing);

			break;
			}
		}
	}

void CNavigateOrder::OnBehaviorStart (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pOrderTarget, const IShipController::SData &Data)

//	OnBehaviorStart
//
//	Start behavior

	{
	//	Make sure we're undocked because we're going flying

	Ctx.Undock(pShip);

	//	Set our basic data. We initialize the following:
	//
	//	m_vDest is the destination position
	//	m_iDestFacing is the angle the ship should have at the destination.
	//	m_rMinDist2 is the minimum distance we need to reach before we succeed.

	Metric rMinDist;
	if (m_fTargetVector)
		{
		if (Data.iDataType == IShipController::dataVector)
			m_vDest = Data.vData;
		else
			m_vDest = pShip->GetPos();

		rMinDist = LIGHT_SECOND;
		}
	else
		{
		if (pOrderTarget)
			{
			m_Objs[objDest] = pOrderTarget;
			m_vDest = pOrderTarget->GetPos();
			}
		else
			m_vDest = pShip->GetPos();

		rMinDist = LIGHT_SECOND * Max(1, (int)Data.AsInteger());
		}

	m_rMinDist2 = (rMinDist * rMinDist);
	m_iDestFacing = ::VectorToPolar(m_vDest - pShip->GetPos());

	//	See if we should take a nav path

	Metric rCurDist2 = (m_vDest - pShip->GetPos()).Length2();
	if (rCurDist2 > NAV_PATH_THRESHOLD2
			&& Ctx.CalcNavPath(pShip, m_vDest))
		m_iState = stateOnCourseViaNavPath;

	//	Otherwise, go there

	else
		m_iState = stateApproachingPos;
	}

void CNavigateOrder::OnReadFromStream (SLoadCtx &Ctx)

//	OnReadFromStream
//
//	Read

	{
	DWORD dwLoad;

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	m_iState = (States)dwLoad;

	Ctx.pStream->Read((char *)&m_vDest, sizeof(CVector));
	Ctx.pStream->Read((char *)&m_iDestFacing, sizeof(DWORD));
	Ctx.pStream->Read((char *)&m_rMinDist2, sizeof(Metric));
	}

void CNavigateOrder::OnWriteToStream (CSystem *pSystem, IWriteStream *pStream)

//	OnWriteToStream
//
//	Write

	{
	DWORD dwSave = (DWORD)m_iState;
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	pStream->Write((char *)&m_vDest, sizeof(CVector));
	pStream->Write((char *)&m_iDestFacing, sizeof(DWORD));
	pStream->Write((char *)&m_rMinDist2, sizeof(Metric));
	}
