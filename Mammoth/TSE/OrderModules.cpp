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

void IOrderModule::Attacked (CShip &Ship, CAIBehaviorCtx &Ctx, CSpaceObject &AttackerObj, const SDamageCtx &Damage, bool bFriendlyFire)

//	Attacked
//
//	Handle an attack from another object

	{
	DEBUG_TRY

	//	Tell our escorts that we were attacked, if necessary

	if (!bFriendlyFire
			&& AttackerObj.CanAttack())
		Ctx.CommunicateWithEscorts(&Ship, msgAttackDeter, &AttackerObj);

	//	Let our subclass handle it.

	OnAttacked(Ship, Ctx, AttackerObj, Damage, bFriendlyFire);

	DEBUG_CATCH
	}

DWORD IOrderModule::Communicate (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2, ICCItem *pData)

//	Communicate
//
//	Handle communications from another ship

	{
	switch (iMessage)
		{
		case msgEscortAttacked:
			{
			//	Treat this as an attack on ourselves

			if (pParam1)
				{
				SDamageCtx Dummy;
				pShip->GetController()->OnAttacked(*pParam1, Dummy);
				}
			return resAck;
			}

		case msgEscortReportingIn:
			Ctx.SetHasEscorts(true);
			return resAck;

		case msgQueryAttackStatus:
			return (IsAttacking() ? resAck : resNoAnswer);

		default:
			return OnCommunicate(pShip, Ctx, pSender, iMessage, pParam1, dwParam2, pData);
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

		case IShipController::orderAttackArea:
		case IShipController::orderAttackNearestEnemy:
		case IShipController::orderAttackOrRetreat:
		case IShipController::orderDestroyTarget:
		case IShipController::orderHoldAndAttack:
			return new CAttackOrder(iOrder);

		case IShipController::orderAttackStation:
			return new CAttackStationOrder;

		case IShipController::orderDeterChase:
			return new CDeterChaseOrder;

		case IShipController::orderEscort:
		case IShipController::orderFollow:
			return new CEscortOrder(iOrder);

		case IShipController::orderFireEvent:
			return new CFireEventOrder;

		case IShipController::orderGuard:
			return new CGuardOrder;

		case IShipController::orderOrbitExact:
		case IShipController::orderOrbitPatrol:
			return new COrbitExactOrder(iOrder);

		case IShipController::orderPatrol:
			return new CPatrolOrder;

		case IShipController::orderDock:
		case IShipController::orderGate:
		case IShipController::orderGoTo:
		case IShipController::orderGoToPos:
		case IShipController::orderNavPath:
			return new CNavigateOrder(iOrder);

		case IShipController::orderSendMessage:
			return new CSendMessageOrder;

		case IShipController::orderSentry:
			return new CSentryOrder;

		case IShipController::orderFireWeapon:
		case IShipController::orderResupply:
		case IShipController::orderUseItem:
			return new CSimpleOrder(iOrder);

		case IShipController::orderHold:
		case IShipController::orderWait:
		case IShipController::orderWaitForEnemy:
		case IShipController::orderWaitForPlayer:
		case IShipController::orderWaitForTarget:
		case IShipController::orderWaitForThreat:
		case IShipController::orderWaitForUndock:
			return new CWaitOrder(iOrder);

		case IShipController::orderAimAtTarget:
		case IShipController::orderBombard:
		case IShipController::orderDestroyPlayerOnReturn:
		case IShipController::orderDestroyTargetHold:
		case IShipController::orderFollowPlayerThroughGate:
		case IShipController::orderGateOnStationDestroyed:
		case IShipController::orderGateOnThreat:
		case IShipController::orderHoldCourse:
		case IShipController::orderLoot:
		case IShipController::orderMine:
		case IShipController::orderOrbit:
		case IShipController::orderScavenge:
		case IShipController::orderTradeRoute:
		case IShipController::orderTurnTo:
		case IShipController::orderWander:
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
	sResult.Append(strPatternSubst(CONSTLIT("Order: %d\r\n"), (int)pShip->GetCurrentOrderDesc().GetOrder()));

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
		if (Ctx.Obj == m_Objs[i])
			{
			//	If this object is a target, and a friendly ship destroyed it, then
			//	thank the object who helped.

			if (GetTarget() == Ctx.Obj && Ctx.Attacker.IsCausedByFriendOf(pShip) && Ctx.Attacker.GetObj())
				pShip->Communicate(Ctx.Attacker.GetObj(), msgNiceShooting);

			//	Clear out the variable. We do this first because the derived class
			//	might set it to something else (thus we don't want to clear it after
			//	the OnObjDestroyed call).

			m_Objs[i] = NULL;

			//	Let our derived class handle it
				
			OnObjDestroyed(pShip, Ctx, i, &bCancelOrder);

			//	If our derived class wants us to cancel the order, then we're done.
			//	(After we cancel the order, the order module will be invalid, so
			//	we need to leave.

			if (bCancelOrder)
				{
				pShip->CancelCurrentOrder();
				return;
				}
			}
	}

void IOrderModule::ReadFromStream (SLoadCtx &Ctx, const COrderDesc &OrderDesc)

//	ReadFromStream
//
//	Load save file
	
	{
	//	Load the objects

	DWORD dwCount;
	Ctx.pStream->Read((char *)&dwCount, sizeof(DWORD));
	for (int i = 0; i < (int)dwCount; i++)
		{
		if (i < m_iObjCount)
			CSystem::ReadObjRefFromStream(Ctx, &m_Objs[i]);
		else
			{
			CSpaceObject *pDummy;
			CSystem::ReadObjRefFromStream(Ctx, &pDummy);
			}
		}

	//	Let our derived class load

	OnReadFromStream(Ctx, OrderDesc);
	}

void IOrderModule::WriteToStream (IWriteStream *pStream) const

//	WriteToStream
//
//	Write to save file
	
	{
	//	Save the objects

	DWORD dwCount = m_iObjCount;
	pStream->Write((char *)&dwCount, sizeof(DWORD));
	for (int i = 0; i < (int)dwCount; i++)
		CSystem::WriteObjRefToStream(*pStream, m_Objs[i]);

	//	Let our derived class save

	OnWriteToStream(pStream);
	}

