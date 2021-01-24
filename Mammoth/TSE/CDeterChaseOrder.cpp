//	CDeterChaseOrder.cpp
//
//	CDeterChaseOrder class
//	Copyright (c) 2021 Kronosaur Productions. All Rights Reserved.

#include "PreComp.h"

COrderDesc CDeterChaseOrder::Create (CSpaceObject &TargetObj, CSpaceObject *pBase, Metric rMaxRange, int iTimer)

//	Create
//
//	Creates an appropriate order descriptor.

	{
	ICCItemPtr pData(ICCItem::SymbolTable);

	if (pBase)
		pData->SetIntegerAt(CONSTLIT("base"), pBase->GetID());

	if (rMaxRange > 0.0)
		pData->SetIntegerAt(CONSTLIT("radius"), mathRound(rMaxRange / LIGHT_SECOND));

	if (iTimer > 0)
		pData->SetIntegerAt(CONSTLIT("timer"), iTimer);

	return COrderDesc(IShipController::orderDeterChase, &TargetObj, *pData);
	}

void CDeterChaseOrder::OnAttacked (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pAttacker, const SDamageCtx &Damage, bool bFriendlyFire)

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

	//	See if the attacker is a better target.

	if (Ctx.CalcIsBetterTarget(pShip, m_Objs[OBJ_TARGET], pAttacker))
		{
		m_Objs[OBJ_TARGET] = pAttacker;
		}

	//	Tell our base, if necessary.

	CSpaceObject *pOrderGiver = Damage.GetOrderGiver();
	CSpaceObject *pTarget;
	if (Ctx.IsSecondAttack()
			&& m_Objs[OBJ_BASE]
			&& m_Objs[OBJ_BASE]->IsAngryAt(pAttacker)
			&& (pTarget = m_Objs[OBJ_BASE]->CalcTargetToAttack(pAttacker, pOrderGiver)))
		pShip->Communicate(m_Objs[OBJ_BASE], msgAttackDeter, pTarget);
	}

void CDeterChaseOrder::OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx)

//	OnBehavior
//
//	Handle behavior

	{
	if (!m_Objs[OBJ_TARGET] || !m_Objs[OBJ_BASE])
		throw CException(ERR_FAIL);

	//	See if it's time to stop chasing.

	if (pShip->IsDestinyTime(19)) 
		{
		//	If we're too far from our base, then we're done.

		if (m_Objs[OBJ_BASE] 
				&& m_rMaxRange2 > 0.0
				&& pShip->GetDistance2(m_Objs[OBJ_BASE]) > m_rMaxRange2)
			{
			pShip->CancelCurrentOrder();
			return;
			}

		//	Otherwise, if we should stop deterring, then we're done.

		else if (!Ctx.CalcIsDeterNeeded(*pShip, *m_Objs[OBJ_BASE]))
			{
			pShip->CancelCurrentOrder();
			return;
			}
		}

	//	See if our timer has expired

	if (m_iCountdown != -1 && m_iCountdown-- == 0)
		{
		pShip->CancelCurrentOrder();
		return;
		}

	//	Chase

	Ctx.ImplementAttackTarget(pShip, m_Objs[OBJ_TARGET]);
	Ctx.ImplementFireOnTargetsOfOpportunity(pShip, m_Objs[OBJ_TARGET]);
	}

void CDeterChaseOrder::OnBehaviorStart (CShip &Ship, CAIBehaviorCtx &Ctx, const COrderDesc &OrderDesc)

//	OnBehaviorStart
//
//	Start

	{
	//	Make sure we're undocked because we're going flying

	Ctx.Undock(&Ship);

	//	Target is required.

	m_Objs[OBJ_TARGET] = OrderDesc.GetTarget();
	if (!m_Objs[OBJ_TARGET])
		{
		Ship.CancelCurrentOrder();
		return;
		}

	//	Base is optional.

	m_Objs[OBJ_BASE] = OrderDesc.GetDataObject(Ship, CONSTLIT("base"));

	//	Radius is optional

	int iRadius = OrderDesc.GetDataIntegerOptional(CONSTLIT("radius"));
	if (iRadius > 0)
		{
		Metric rMaxRange = iRadius * LIGHT_SECOND;
		m_rMaxRange2 = rMaxRange * rMaxRange;
		}
	else
		m_rMaxRange2 = 0.0;

	//	Timer is optional.

	m_iCountdown = OrderDesc.GetDataTicksLeft();
	}

DWORD CDeterChaseOrder::OnCommunicate (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2, ICCItem *pData)

//	OnCommunicate
//
//	Process communications.

	{
	switch (iMessage)
		{
		case msgAbort:
		case msgFormUp:
			pShip->CancelCurrentOrder();
			return resAck;

		case msgAttack:
		case msgAttackDeter:
		case msgDestroyBroadcast:
		case msgBaseDestroyedByTarget:
			{
			if (pParam1
					&& Ctx.CalcIsBetterTarget(pShip, m_Objs[OBJ_TARGET], pParam1))
				{
				m_Objs[OBJ_TARGET] = pParam1;
				return resAck;
				}
			else
				return resNoAnswer;
			}

		case msgQueryAttackStatus:
			return resAck;

		default:
			return resNoAnswer;
		}
	}

void CDeterChaseOrder::OnDestroyed (CShip *pShip, SDestroyCtx &Ctx)

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

void CDeterChaseOrder::OnObjDestroyed (CShip *pShip, const SDestroyCtx &Ctx, int iObj, bool *retbCancelOrder)

//	OnObjDestroyed
//
//	An object was destroyed.

	{
	if (iObj == OBJ_TARGET)
		{
		//	If a friend destroyed our target then thank them

		if (Ctx.Attacker.IsCausedByFriendOf(pShip) && Ctx.Attacker.GetObj())
			pShip->Communicate(Ctx.Attacker.GetObj(), msgNiceShooting);

		//	Done

		pShip->CancelCurrentOrder();
		}
	else if (iObj == OBJ_BASE)
		{
		//	If our base got destroyed, we continue.
		}
	}

void CDeterChaseOrder::OnReadFromStream (SLoadCtx &Ctx)

//	OnReadFromStream
//
//	Read from stream

	{
	Ctx.pStream->Read(m_rMaxRange2);
	Ctx.pStream->Read(m_iCountdown);
	}

void CDeterChaseOrder::OnWriteToStream (IWriteStream *pStream) const

//	OnWriteToStream
//
//	Write to stream

	{
	pStream->Write(m_rMaxRange2);
	pStream->Write(m_iCountdown);
	}
