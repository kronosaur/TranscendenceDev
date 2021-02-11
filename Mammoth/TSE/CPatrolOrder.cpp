//	CPatrolOrder.cpp
//
//	CPatrolOrder class
//	Copyright (c) 2021 Kronosaur Productions. All Rights Reserved.

#include "PreComp.h"

void CPatrolOrder::CalcIntermediates ()

//	CalcIntermediates
//
//	Initializes cached calculations:
//
//	m_rNavThreshold2

	{
	const Metric rPatrolRadiusThreshold = 1.25 * m_rPatrolRadius;
	const Metric rPatrolRadiusThreshold2 = rPatrolRadiusThreshold * rPatrolRadiusThreshold;
	m_rNavThreshold2 = Max(rPatrolRadiusThreshold2, NAV_PATH_THRESHOLD2);
	}

void CPatrolOrder::OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx)

//	OnBehavior
//
//	Update behavior.

	{
	if (!pShip)
		throw CException(ERR_FAIL);

	//	If we're too far away, then use a nav path to get closer.
	//	NOTE: We guarantee that NavThreshold is always greater than 
	//	PatrolRadius.

	else if ((pShip->GetPos() - m_Objs[OBJ_BASE]->GetPos()).Length2() > m_rNavThreshold2)
		{
		pShip->AddOrder(COrderDesc(IShipController::orderApproach, m_Objs[OBJ_BASE], mathRound(m_rPatrolRadius / LIGHT_SECOND)), true);
		}

	//	Every once in a while we check to see if we need to dock with our base
	//	to resupply.

	else if (pShip->IsDestinyTime(RESUPPLY_CHECK_TIME) 
			&& Ctx.ImplementResupplyCheck(*pShip, *m_Objs[OBJ_BASE]))
		{ }

	//	Normal behavior

	else
		{
		CVector vTarget = m_Objs[OBJ_BASE]->GetPos() - pShip->GetPos();
		Metric rTargetDist2 = vTarget.Dot(vTarget);

		Metric rMaxDist = m_rPatrolRadius * 1.1;
		Metric rMinDist = m_rPatrolRadius * 0.9;

		if (rTargetDist2 > (rMaxDist * rMaxDist))
			Ctx.ImplementSpiralIn(pShip, vTarget);
		else if (rTargetDist2 < (rMinDist * rMinDist))
			Ctx.ImplementSpiralOut(pShip, vTarget);
		else
			Ctx.ImplementSpiralOut(pShip, vTarget, 0);
		}
	}

void CPatrolOrder::OnBehaviorStart (CShip &Ship, CAIBehaviorCtx &Ctx, const COrderDesc &OrderDesc)

//	OnBehaviorStart
//
//	Initialize order module

	{
	DEBUG_TRY

	//	Undock, if necessary

	Ctx.Undock(&Ship);

	//	Setup parameters

	m_Objs[OBJ_BASE] = OrderDesc.GetTarget();
	if (!m_Objs[OBJ_BASE])
		{
		Ship.CancelCurrentOrder();
		return;
		}

	m_rPatrolRadius = LIGHT_SECOND * Max(1, (int)OrderDesc.GetDataInteger());

	CalcIntermediates();

	DEBUG_CATCH
	}

void CPatrolOrder::OnDestroyed (CShip *pShip, SDestroyCtx &Ctx)

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

void CPatrolOrder::OnReadFromStream (SLoadCtx &Ctx)

//	OnReadFromStream
//
//	Read from stream

	{
	Ctx.pStream->Read(m_rPatrolRadius);

	CalcIntermediates();
	}

void CPatrolOrder::OnWriteToStream (IWriteStream *pStream) const

//	OnWriteToStream
//
//	Write to stream

	{
	pStream->Write(m_rPatrolRadius);
	}
