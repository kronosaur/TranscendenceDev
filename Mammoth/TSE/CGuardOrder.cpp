//	CGuardOrder.cpp
//
//	CGuardOrder class
//	Copyright (c) 2021 Kronosaur Productions. All Rights Reserved.

#include "PreComp.h"

void CGuardOrder::Init (const COrderDesc &OrderDesc)

//	Init
//
//	Initializes cached calculations:

	{
	m_rThreatRange = OrderDesc.GetDataDouble(CONSTLIT("threatRange"), DEFAULT_THREAT_RANGE_LS) * LIGHT_SECOND;
	m_rThreatStopRange = OrderDesc.GetDataDouble(CONSTLIT("threatStopRange"), DEFAULT_THREAT_STOP_RANGE_LS) * LIGHT_SECOND;
	}

void CGuardOrder::OnAccumulateAIStatus (const CShip &Ship, const CAIBehaviorCtx &Ctx, ICCItem &Result) const

//	OnAccumulateAIStatus
//
//	Sets AI status.

	{
	Result.SetStringAt(CONSTLIT("class"), CONSTLIT("CGuardOrder"));
	}

void CGuardOrder::OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx)

//	OnBehavior
//
//	Update behavior.

	{
	if (!pShip)
		throw CException(ERR_FAIL);

	//	If we're docked, then nothing to do.

	else if (pShip->GetDockedObj())
		{ }

	//	If we're too far away, then use a nav path to get closer.

	else if ((pShip->GetPos() - m_Objs[OBJ_BASE]->GetPos()).Length2() > NAV_PATH_THRESHOLD2)
		{
		pShip->AddOrder(CApproachOrder::Create(*m_Objs[OBJ_BASE], NAV_PATH_THRESHOLD_LS, CReactionImpl(*this), FLAG_CANCEL_ON_REACTION_ORDER), true);
		}

	//	Dock with base

	else
		{
		pShip->AddOrder(CNavigateOrder::CreateDock(*m_Objs[OBJ_BASE], CReactionImpl(*this), FLAG_CANCEL_ON_REACTION_ORDER), true);
		}
	}

void CGuardOrder::OnBehaviorStart (CShip &Ship, CAIBehaviorCtx &Ctx, const COrderDesc &OrderDesc)

//	OnBehaviorStart
//
//	Initialize order module

	{
	DEBUG_TRY

	//	Setup parameters

	m_Objs[OBJ_BASE] = OrderDesc.GetTarget();
	if (!m_Objs[OBJ_BASE])
		{
		Ship.CancelCurrentOrder();
		return;
		}

	Init(OrderDesc);

	DEBUG_CATCH
	}

void CGuardOrder::OnDestroyed (CShip *pShip, SDestroyCtx &Ctx)

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
