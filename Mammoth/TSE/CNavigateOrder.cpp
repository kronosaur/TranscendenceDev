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

const Metric NAV_PATH_MOVE_THRESHOLD =	(10.0 * LIGHT_SECOND);
const Metric NAV_PATH_MOVE_THRESHOLD2 =	(NAV_PATH_MOVE_THRESHOLD * NAV_PATH_MOVE_THRESHOLD);

CNavigateOrder::CNavigateOrder (IShipController::OrderTypes iOrder) : IOrderModule(objCount),
		m_iOrder(iOrder),
		m_Reaction(AIReaction::DeterWithSecondaries, AIReaction::None, AIReaction::None)

//	CNavigateOrder constructor

	{
	switch (m_iOrder)
		{
		case IShipController::orderDock:
			m_fDockAtDestination = true;
			m_fTargetObj = true;
			break;

		case IShipController::orderGate:
			m_fGateAtDestination = true;
			m_fTargetObj = true;
			break;

		case IShipController::orderGoTo:
			m_fTargetObj = true;
			m_fVariableMinDist = true;
			break;

		case IShipController::orderGoToPos:
			m_fTargetVector = true;
			break;

		case IShipController::orderNavPath:
			m_fTargetObj = true;
			m_fNavPathOnly = true;
			break;

		default:
			ASSERT(false);
		}
	}

COrderDesc CNavigateOrder::CreateDock (CSpaceObject &Dest, const CReactionImpl &Reactions, DWORD dwFlags)

//	CreateDock
//
//	Create dock order

	{
	ICCItemPtr pData(ICCItem::SymbolTable);
	Reactions.SetOptions(*pData);

	COrderDesc Result(IShipController::orderDock, &Dest, *pData);

	//	Set some flags

	if (dwFlags & FLAG_CANCEL_ON_REACTION_ORDER)
		Result.SetCancelOnReactionOrder();

	return Result;
	}

void CNavigateOrder::OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx)

//	OnBehavior
//
//	Behavior

	{
	DEBUG_TRY

	//	If we're following a nav path, then do it

	if (m_fIsFollowingNavPath)
		{
		//	Check to see if our destination has moved. If it has, then we stop
		//	following the nav path. This can happen if we pick a ship as a 
		//	destination.

		bool bAtDest = false;
		if (m_Objs[objDest] 
				&& (m_Objs[objDest]->GetPos() - m_vDest).Length2() > NAV_PATH_MOVE_THRESHOLD2)
			bAtDest = true;
		else
			Ctx.ImplementFollowNavPath(pShip, &bAtDest);

		if (bAtDest)
			{
			Ctx.ClearNavPath();
			m_fIsFollowingNavPath = false;

			//	In some cases, we're done

			if (m_fNavPathOnly)
				pShip->CancelCurrentOrder();
			}
		}

	//	If we're docking, then dock

	else if (m_fDockAtDestination)
		{
		//	If we're docked already, then we're done

		if (pShip->GetDockedObj() == m_Objs[objDest])
			pShip->CancelCurrentOrder();

		//	If we have no target, then nothing to do

		else if (m_Objs[objDest] == NULL)
			pShip->CancelCurrentOrder();

		//	Navigate

		else
			{
			Ctx.ImplementDocking(pShip, m_Objs[objDest]);
			}
		}

	//	If we're gating, then gate

	else if (m_fGateAtDestination)
		{
		//	Make sure we have a valid gate. Note that we don't cancel the order
		//	because gating is one of the orders we add if we have no other 
		//	orders. Sometimes this gets added in the intro screen (where we have 
		//	no gates).

		if (m_Objs[objDest] != NULL)
			{
			Ctx.ImplementGating(pShip, m_Objs[objDest]);
			}
		}

	//	Otherwise, continue

	else
		{
		//	Maneuver

		CVector vTarget = m_vDest - pShip->GetPos();
		Metric rTargetDist2 = vTarget.Dot(vTarget);

		//	If we're close enough, we're done

		if (rTargetDist2 < m_rMinDist2)
			pShip->CancelCurrentOrder();

		//	Otherwise, we use formation flying

		else
			Ctx.ImplementFormationManeuver(pShip, m_vDest, NullVector, m_iDestFacing);
		}

	DEBUG_CATCH
	}

void CNavigateOrder::OnBehaviorStart (CShip &Ship, CAIBehaviorCtx &Ctx, const COrderDesc &OrderDesc)

//	OnBehaviorStart
//
//	Start behavior

	{
	DEBUG_TRY

	//	If we want to dock and are already docked, then nothing else to do.

	CSpaceObject *pOrderTarget = OrderDesc.GetTarget();
	if (m_fDockAtDestination
			&& Ship.GetDockedObj() == pOrderTarget)
		{
		Ship.CancelCurrentOrder();
		return;
		}

	//	Make sure we're undocked because we're going flying

	Ctx.Undock(&Ship);

	//	Set our basic data. We initialize the following:
	//
	//	m_vDest is the destination position
	//	m_iDestFacing is the angle the ship should have at the destination.
	//	m_rMinDist2 is the minimum distance we need to reach before we succeed.

	Metric rMinDist;
	if (m_fTargetVector)
		{
		m_vDest = OrderDesc.GetDataVector(CONSTLIT("pos"), true, Ship.GetPos());
		}
	else
		{
		if (pOrderTarget == NULL && m_fGateAtDestination)
			pOrderTarget = Ship.GetNearestStargate(true);

		if (pOrderTarget)
			{
			m_Objs[objDest] = pOrderTarget;
			m_vDest = pOrderTarget->GetPos();
			}
		else
			m_vDest = Ship.GetPos();
		}

	//	Get the minimum distance

	if (m_fVariableMinDist)
		rMinDist = LIGHT_SECOND * Max(1, (int)OrderDesc.GetDataInteger(CONSTLIT("radius"), true, 1));
	else
		rMinDist = LIGHT_SECOND;

	m_rMinDist2 = (rMinDist * rMinDist);
	m_iDestFacing = ::VectorToPolar(m_vDest - Ship.GetPos());

	m_Reaction.Init(OrderDesc);

	//	See if we should take a nav path

	Metric rCurDist2 = (m_vDest - Ship.GetPos()).Length2();
	if (rCurDist2 > NAV_PATH_THRESHOLD2)
		{
		//	If we have a destination object, then calculate a nav path to it.

		if (m_Objs[objDest])
			m_fIsFollowingNavPath = Ctx.CalcNavPath(&Ship, m_Objs[objDest]);

		//	If we don't have a destination object, then we calculate a nav path
		//	based on a position

		else
			m_fIsFollowingNavPath = Ctx.CalcNavPath(&Ship, m_vDest);
		}

	DEBUG_CATCH
	}

CSpaceObject *CNavigateOrder::OnGetBase (void)

//	OnGetBase
//
//	Returns our base

	{
	if (m_fDockAtDestination)
		return m_Objs[objDest];
	else
		return NULL;
	}

void CNavigateOrder::OnObjDestroyed (CShip *pShip, const SDestroyCtx &Ctx, int iObj, bool *retbCancelOrder)

//	OnObjDestroyed
//
//	And object was destroyed.

	{
	//	If our gate got destroyed (for some reason) then we cancel the order. We
	//	need to do this explicitly because sometimes we pick a gate ourselves and
	//	so the gate is not encoded in the order.

	if (m_fGateAtDestination && iObj == objDest)
		*retbCancelOrder = true;
	}

void CNavigateOrder::OnReadFromStream (SLoadCtx &Ctx, const COrderDesc &OrderDesc)

//	OnReadFromStream
//
//	Read

	{
	DWORD dwLoad;

	Ctx.pStream->Read((char *)&m_vDest, sizeof(CVector));
	Ctx.pStream->Read((char *)&m_iDestFacing, sizeof(DWORD));
	Ctx.pStream->Read((char *)&m_rMinDist2, sizeof(Metric));

	//	Flags

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	m_fIsFollowingNavPath = ((dwLoad & 0x00000001) ? true : false);

	m_Reaction.Init(OrderDesc);
	}

void CNavigateOrder::OnWriteToStream (IWriteStream *pStream) const

//	OnWriteToStream
//
//	Write

	{
	DWORD dwSave;

	pStream->Write((char *)&m_vDest, sizeof(CVector));
	pStream->Write((char *)&m_iDestFacing, sizeof(DWORD));
	pStream->Write((char *)&m_rMinDist2, sizeof(Metric));

	//	Flags

	dwSave = 0;
	dwSave |= (m_fIsFollowingNavPath ? 0x00000001 : 0);
	pStream->Write((char *)&dwSave, sizeof(DWORD));
	}
