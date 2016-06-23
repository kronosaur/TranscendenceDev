//	OrderModules.cpp
//
//	Implementation of IOrderModule classes
//	Copyright (c) 2011 by George Moromisato. All Rights Reserved.

#include "PreComp.h"

IOrderModule::IOrderModule (int iObjCount)

//	IOrderModule constructor

	{
	int i;

	ASSERT(iObjCount <= MAX_OBJS);

	m_iObjCount = iObjCount;
	for (i = 0; i < m_iObjCount; i++)
		m_Objs[i] = NULL;
	}

IOrderModule::~IOrderModule (void)

//	IOrderModule destructor

	{
	}

void IOrderModule::Attacked (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pAttacker, const SDamageCtx &Damage, bool bFriendlyFire)

//	Attacked
//
//	Handle an attack from another object

	{
	//	Tell our escorts that we were attacked, if necessary

	if (pAttacker
			&& !bFriendlyFire
			&& pAttacker->CanAttack())
		Ctx.CommunicateWithEscorts(pShip, msgAttackDeter, pAttacker);

	//	Let our subclass handle it.

	OnAttacked(pShip, Ctx, pAttacker, Damage, bFriendlyFire);
	}

DWORD IOrderModule::Communicate (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2)

//	Communicate
//
//	Handle communications from another ship

	{
	switch (iMessage)
		{
		case msgEscortAttacked:
			{
			//	Treat this as an attack on ourselves

			SDamageCtx Dummy;
			pShip->GetController()->OnAttacked(pParam1, Dummy);
			return resAck;
			}

		case msgEscortReportingIn:
			Ctx.SetHasEscorts(true);
			return resAck;

		case msgQueryAttackStatus:
			return (IsAttacking() ? resAck : resNoAnswer);

		default:
			return OnCommunicate(pShip, Ctx, pSender, iMessage, pParam1, dwParam2);
		}
	}

IOrderModule *IOrderModule::Create (IShipController::OrderTypes iOrder)

//	Create
//
//	Creates an order module

	{
	DEBUG_TRY

	switch (iOrder)
		{
		case IShipController::orderNone:
			return NULL;

		case IShipController::orderApproach:
			return new CApproachOrder;

		case IShipController::orderDestroyTarget:
		case IShipController::orderAttackNearestEnemy:
		case IShipController::orderAttackArea:
		case IShipController::orderHoldAndAttack:
			return new CAttackOrder(iOrder);

		case IShipController::orderAttackStation:
			return new CAttackStationOrder;

		case IShipController::orderEscort:
		case IShipController::orderFollow:
			return new CEscortOrder(iOrder);

		case IShipController::orderFireEvent:
			return new CFireEventOrder;

		case IShipController::orderDock:
		case IShipController::orderGate:
		case IShipController::orderGoTo:
		case IShipController::orderGoToPos:
		case IShipController::orderNavPath:
			return new CNavigateOrder(iOrder);

		case IShipController::orderSendMessage:
			return new CSendMessageOrder;

		case IShipController::orderHold:
		case IShipController::orderWait:
		case IShipController::orderWaitForEnemy:
		case IShipController::orderWaitForPlayer:
		case IShipController::orderWaitForTarget:
		case IShipController::orderWaitForUndock:
			return new CWaitOrder(iOrder);

		case IShipController::orderGuard:
		case IShipController::orderGateOnThreat:
		case IShipController::orderGateOnStationDestroyed:
		case IShipController::orderPatrol:
		case IShipController::orderScavenge:
		case IShipController::orderFollowPlayerThroughGate:
		case IShipController::orderTradeRoute:
		case IShipController::orderWander:
		case IShipController::orderLoot:
		case IShipController::orderMine:
		case IShipController::orderDestroyPlayerOnReturn:
		case IShipController::orderBombard:
		case IShipController::orderAimAtTarget:
		case IShipController::orderOrbit:
		case IShipController::orderHoldCourse:
		case IShipController::orderTurnTo:
		case IShipController::orderDestroyTargetHold:
			return NULL;

		default:
			ASSERT(false);
			return NULL;
		}

	DEBUG_CATCH
	}

CString IOrderModule::DebugCrashInfo (CShip *pShip)

//	DebugCrashInfo
//
//	Output debug info

	{
	int i;

	CString sResult;

	sResult.Append(CONSTLIT("IOrderModule\r\n"));
	sResult.Append(strPatternSubst(CONSTLIT("Order: %d\r\n"), (int)pShip->GetCurrentOrder()));

	for (i = 0; i < m_iObjCount; i++)
		sResult.Append(strPatternSubst(CONSTLIT("m_Objs[%d]: %s\r\n"), i, CSpaceObject::DebugDescribe(m_Objs[i])));

	sResult.Append(OnDebugCrashInfo());

	return sResult;
	}

void IOrderModule::Destroyed (CShip *pShip, SDestroyCtx &Ctx)

//	Destroyed
//
//	We've been destroyed

	{
	OnDestroyed(pShip, Ctx);
	}

void IOrderModule::ObjDestroyed (CShip *pShip, const SDestroyCtx &Ctx)

//	ObjDestroyed
//
//	And object was destroyed

	{
	int i;
	bool bCancelOrder = false;

	for (i = 0; i < m_iObjCount; i++)
		if (Ctx.pObj == m_Objs[i])
			{
			//	If this object is a target, and a friendly ship destroyed it, then
			//	thank the object who helped.

			if (GetTarget() == Ctx.pObj && Ctx.Attacker.IsCausedByFriendOf(pShip) && Ctx.Attacker.GetObj())
				pShip->Communicate(Ctx.Attacker.GetObj(), msgNiceShooting);

			//	Clear out the variable. We do this first because the derrived class
			//	might set it to something else (thus we don't want to clear it after
			//	the OnObjDestroyed call).

			m_Objs[i] = NULL;

			//	Let our derrived class handle it
				
			OnObjDestroyed(pShip, Ctx, i, &bCancelOrder);

			//	If our derrived class wants us to cancel the order, then we're done.
			//	(After we cancel the order, the order module will be invalid, so
			//	we need to leave.

			if (bCancelOrder)
				{
				pShip->CancelCurrentOrder();
				return;
				}
			}
	}

void IOrderModule::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Load save file
	
	{
	int i;

	//	Load the objects

	DWORD dwCount;
	Ctx.pStream->Read((char *)&dwCount, sizeof(DWORD));
	for (i = 0; i < (int)dwCount; i++)
		{
		if (i < m_iObjCount)
			CSystem::ReadObjRefFromStream(Ctx, &m_Objs[i]);
		else
			{
			CSpaceObject *pDummy;
			CSystem::ReadObjRefFromStream(Ctx, &pDummy);
			}
		}

	//	Let our derrived class load

	OnReadFromStream(Ctx); 
	}

void IOrderModule::WriteToStream (CSystem *pSystem, IWriteStream *pStream)

//	WriteToStream
//
//	Write to save file
	
	{
	int i;

	//	Save the objects

	DWORD dwCount = m_iObjCount;
	pStream->Write((char *)&dwCount, sizeof(DWORD));
	for (i = 0; i < (int)dwCount; i++)
		pSystem->WriteObjRefToStream(m_Objs[i], pStream);

	//	Let our derrived class save

	OnWriteToStream(pSystem, pStream);
	}

//	CGuardOrder ----------------------------------------------------------------

void CGuardOrder::OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx)

//	OnBehavior
//
//	Do it

	{
	}

void CGuardOrder::OnBehaviorStart (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pOrderTarget, const IShipController::SData &Data)

//	OnBehaviorStart
//
//	Start/restart behavior

	{
#if 0
	ASSERT(pOrderTarget);
	m_pBase = pOrderTarget;

	//	If we're docked, wait for threat

	if (pShip->GetDockedObj())
		m_iState = stateWaitingForThreat;

	//	If we're very far from our principal and we can use a nav
	//	path, do it

	else if (pShip->GetDistance2(m_pBase) > NAV_PATH_THRESHOLD2
			&& CalcNavPath(m_pBase))
		m_iState = stateReturningViaNavPath;

	//	Otherwise, return directly to base

	else
		m_iState = stateReturningFromThreat;
#endif
	}

void CGuardOrder::OnReadFromStream (SLoadCtx &Ctx)

//	OnReadFromStream
//
//	Load from stream

	{
	DWORD dwLoad;

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	m_iState = (States)dwLoad;

	CSystem::ReadObjRefFromStream(Ctx, &m_pBase);
	}

void CGuardOrder::OnWriteToStream (CSystem *pSystem, IWriteStream *pStream)

//	OnWriteToStream
//
//	Save to stream

	{
	DWORD dwSave;

	dwSave = (DWORD)m_iState;
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	pSystem->WriteObjRefToStream(m_pBase, pStream);
	}
