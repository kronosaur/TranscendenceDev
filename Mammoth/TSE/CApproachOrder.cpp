//	CApproachOrder.cpp
//
//	CApproachOrder class implementation
//	Copyright (c) 2014 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

const Metric APPROACH_THRESHOLD =		(50.0 * LIGHT_SECOND);
const Metric APPROACH_THRESHOLD2 =		(APPROACH_THRESHOLD * APPROACH_THRESHOLD);

const Metric PATROL_SENSOR_RANGE =		(30.0 * LIGHT_SECOND);
const Metric NAV_PATH_THRESHOLD =		(3.0 * PATROL_SENSOR_RANGE);
const Metric NAV_PATH_THRESHOLD2 =		(NAV_PATH_THRESHOLD * NAV_PATH_THRESHOLD);

const Metric NAV_PATH_RECALC_THRESHOLD = (50.0 * LIGHT_SECOND);
const Metric NAV_PATH_RECALC_THRESHOLD2 = (NAV_PATH_RECALC_THRESHOLD * NAV_PATH_RECALC_THRESHOLD);

void CApproachOrder::OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx)

//	OnBehavior
//
//	Behavior

	{
	DEBUG_TRY

	switch (m_iState)
		{
		case stateOnCourseViaNavPath:
			{
			Metric rDest2;

			Ctx.ImplementAttackNearestTarget(pShip, Ctx.GetBestWeaponRange(), &m_Objs[objTarget]);
			Ctx.ImplementFireOnTargetsOfOpportunity(pShip, m_Objs[objTarget]);

			bool bAtDest;
			Ctx.ImplementFollowNavPath(pShip, &bAtDest);
			if (bAtDest)
				{
				Ctx.ClearNavPath();
				m_iState = stateApproaching;
				}
			else if ((rDest2 = (m_Objs[objDest]->GetPos() - pShip->GetPos()).Length2()) < m_rMinDist2)
				{
				Ctx.ClearNavPath();
				pShip->CancelCurrentOrder();
				}

			//	If our destination can move and it has moved away from the nav path
			//	destination, then we need to recalc.

			else if (m_Objs[objDest]->CanThrust() 
					&& (m_Objs[objDest]->GetPos() - Ctx.GetNavPath()->GetPathEnd()).Length2() > NAV_PATH_RECALC_THRESHOLD2)
				{
				//	If we're still far enough away that we need a new nav path, then
				//	recalc.

				if (rDest2 > NAV_PATH_THRESHOLD2
						&& Ctx.CalcNavPath(pShip, m_Objs[objDest]))
					;

				//	Otherwise, done with nav path.

				else
					{
					Ctx.ClearNavPath();
					m_iState = stateApproaching;
					}
				}

			break;
			}

		case stateApproaching:
			{
			Ctx.ImplementAttackNearestTarget(pShip, Ctx.GetBestWeaponRange(), &m_Objs[objTarget]);
			Ctx.ImplementFireOnTargetsOfOpportunity(pShip, m_Objs[objTarget]);

			//	Maneuver

			CVector vTarget = m_Objs[objDest]->GetPos() - pShip->GetPos();
			Metric rTargetDist2 = vTarget.Dot(vTarget);

			//	If we're close enough, we're done

			if (rTargetDist2 < m_rMinDist2)
				pShip->CancelCurrentOrder();

			//	Otherwise, if we're close, we use formation flying

			else if (rTargetDist2 < APPROACH_THRESHOLD2)
				Ctx.ImplementFormationManeuver(pShip, m_Objs[objDest]->GetPos(), m_Objs[objDest]->GetVel(), pShip->GetRotation());

			//	Otherwise, just close on target

			else
				Ctx.ImplementCloseOnTarget(pShip, m_Objs[objDest], vTarget, rTargetDist2);

			break;
			}
		}

	DEBUG_CATCH
	}

void CApproachOrder::OnBehaviorStart (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pOrderTarget, const IShipController::SData &Data)

//	OnBehaviorStart
//
//	Start behavior

	{
	DEBUG_TRY

	ASSERT(pOrderTarget);

	//	Make sure we're undocked because we're going flying

	Ctx.Undock(pShip);

	//	Set our basic data

	m_Objs[objDest] = pOrderTarget;
	m_rMinDist2 = LIGHT_SECOND * Max(1, (int)Data.AsInteger());
	m_rMinDist2 *= m_rMinDist2;

	//	See if we should take a nav path

	if (pShip->GetDistance2(pOrderTarget) > NAV_PATH_THRESHOLD2
			&& Ctx.CalcNavPath(pShip, pOrderTarget))
		m_iState = stateOnCourseViaNavPath;

	//	Otherwise, go there

	else
		m_iState = stateApproaching;

	DEBUG_CATCH
	}

void CApproachOrder::OnReadFromStream (SLoadCtx &Ctx)

//	OnReadFromStream
//
//	Read

	{
	DWORD dwLoad;

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	m_iState = (States)dwLoad;

	Ctx.pStream->Read((char *)&m_rMinDist2, sizeof(Metric));
	}

void CApproachOrder::OnWriteToStream (CSystem *pSystem, IWriteStream *pStream)

//	OnWriteToStream
//
//	Write

	{
	DWORD dwSave;

	dwSave = (DWORD)m_iState;
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	pStream->Write((char *)&m_rMinDist2, sizeof(Metric));
	}
