//	CFleetCommandAI.cpp
//
//	CFleetCommandAI class

#include "PreComp.h"


#define ATTACK_RANGE					(18.0 * LIGHT_SECOND)

//	At this perception we should be able to detect a normal ship (stealth = 4)
//	at almost 4 light-minutes distance
#define DEFAULT_PERCEPTION				6

#define MAX_DETECTION_RANGE				(240.0 * LIGHT_SECOND)
#define MAX_DETECTION_RANGE2			(MAX_DETECTION_RANGE * MAX_DETECTION_RANGE)
#define MAX_THREAT_RANGE				(120.0 * LIGHT_SECOND)
#define INTERCEPT_THREAT_RANGE			(80.0 * LIGHT_SECOND)
#define INTERCEPT_THREAT_RANGE2			(INTERCEPT_THREAT_RANGE * INTERCEPT_THREAT_RANGE)
#define MIN_THREAT_RANGE				(24.0 * LIGHT_SECOND)
#define MIN_THREAT_RANGE2				(MIN_THREAT_RANGE * MIN_THREAT_RANGE)
#define DEFENSE_RANGE					(18.0 * LIGHT_SECOND)
#define SEPARATION_RANGE				(8.0 * LIGHT_SECOND)
#define SEPARATION_RANGE2				(SEPARATION_RANGE * SEPARATION_RANGE)
#define RALLY_RANGE						(260.0 * LIGHT_SECOND)
#define BREAK_CHARGE_RANGE				(20.0 * LIGHT_SECOND)
#define CHARGE_ATTACK_RANGE				(200.0 * LIGHT_SECOND)

CFleetCommandAI::SAsset *CFleetCommandAI::AddAsset (CSpaceObject *pAsset)

//	AddAsset
//
//	Add an asset to our list

	{
	//	Make sure that we are not adding a duplicate

#ifdef DEBUG
	for (int i = 0; i < m_Assets.GetCount(); i++)
		if (m_Assets[i].pAsset == pAsset)
			throw CException(ERR_FAIL);
#endif

	//	Add the ship

	SAsset *pNewAsset = m_Assets.Insert();
	pNewAsset->pAsset = pAsset;
	pNewAsset->iFormationPos = -1;
	pNewAsset->pTarget = NULL;
	m_iStartingAssetCount++;

	return pNewAsset;
	}

void CFleetCommandAI::AddTarget (CSpaceObject *pTarget)

//	AddTarget
//
//	Add an target to our list

	{
	STarget *pNewTarget = m_Targets.Insert();
	pNewTarget->pTarget = pTarget;
	}

void CFleetCommandAI::Behavior (SUpdateCtx &Ctx)

//	Behavior
//
//	Fly, fight, die

	{
	//	Reset

	ResetBehavior();

	//	Use basic items

	UseItemsBehavior();

	//	Behave according to our state

	switch (m_State)
		{
		case stateNone:
			BehaviorStart();
			break;

		case stateAttackAtWill:
			ImplementAttackAtWill();
			break;

		case stateAttackFromRallyPoint:
			ImplementAttackFromRallyPoint();
			break;

		case stateChargeInFormation:
			ImplementChargeInFormation();
			break;

		case stateFormAtRallyPoint:
			ImplementFormAtRallyPoint();
			m_AICtx.ImplementAttackNearestTarget(m_pShip, ATTACK_RANGE, &m_pTarget);
			m_AICtx.ImplementFireOnTargetsOfOpportunity(m_pShip);
			break;

		case stateOnCourseForStargate:
			m_AICtx.ImplementGating(m_pShip, m_pObjective);
			m_AICtx.ImplementAttackNearestTarget(m_pShip, ATTACK_RANGE, &m_pTarget);
			m_AICtx.ImplementFireOnTargetsOfOpportunity(m_pShip);
			break;

		case stateWaiting:
			ImplementWaitAtPosition();
			break;

		case stateWaitingForThreat:
			{
			ASSERT(m_pObjective);

			//	Compute threat vector

			if (m_pShip->IsDestinyTime(31))
				UpdateTargetListAndPotentials();

			//	Maneuver so that we can defend the station against this threat

			CVector vPos = m_pObjective->GetPos() + (m_vThreatPotential.Normal() * DEFENSE_RANGE);
			m_AICtx.ImplementFormationManeuver(m_pShip, vPos, NullVector, m_pShip->AlignToRotationAngle(VectorToPolar(m_vThreatPotential)));

			//	Loop over all our targets and make sure that they have enough ships
			//	taking care of them.

			if (m_pShip->IsDestinyTime(17))
				UpdateAttackTargets();

			//	Attack as appropriate

			m_AICtx.ImplementAttackNearestTarget(m_pShip, ATTACK_RANGE, &m_pTarget);
			m_AICtx.ImplementFireOnTargetsOfOpportunity(m_pShip);
			break;
			}

		default:
			ASSERT(false);
		}
	}

void CFleetCommandAI::BehaviorStart (void)

//	BehaviorStart
//
//	Initiate behavior state based on orders

	{
	switch (GetCurrentOrder())
		{
		case IShipController::orderNone:
			if (m_pShip->GetDockedObj() == NULL)
				AddOrder(COrderDesc(IShipController::orderGate));
			break;

		case IShipController::orderDestroyTarget:
			{
			CSpaceObject *pTarget = GetCurrentOrderTarget();
			ASSERT(pTarget);

			SetState(stateFormAtRallyPoint);
			m_pObjective = pTarget;
			m_vRallyPoint = ComputeRallyPoint(m_pObjective, &m_iRallyFacing);

			OrderAllFormUp();
			break;
			}

		case IShipController::orderGate:
			{
			//	Look for the gate

			CSpaceObject *pGate = GetCurrentOrderTarget();
			if (pGate == NULL)
				pGate = m_pShip->GetNearestStargate(true);

			//	Head for the gate

			if (pGate)
				{
				SetState(stateOnCourseForStargate);
				m_pObjective = pGate;
				}

			break;
			}

		case IShipController::orderGuard:
			{
			CSpaceObject *pPrincipal = GetCurrentOrderTarget();
			ASSERT(pPrincipal);

			SetState(stateWaitingForThreat);
			m_pObjective = pPrincipal;
			break;
			}

		case IShipController::orderWait:
			{
			DWORD dwWaitTime = GetCurrentOrderDataInteger();

			SetState(stateWaiting);
			if (dwWaitTime == 0)
				m_iCounter = -1;
			else
				m_iCounter = 1 + (dwWaitTime * g_TicksPerSecond);
			break;
			}
		}
	}

void CFleetCommandAI::ComputeCombatPower (int *retiAssetPower, int *retiTargetPower)

//	ComputeCombatPower
//
//	Compute the power of assets and targets

	{
	if (retiAssetPower)
		{
		int iAssetPower = 0;

		for (int i = 0; i < m_Assets.GetCount(); i++)
			iAssetPower += m_Assets[i].pAsset->GetCombatPower();

		*retiAssetPower = iAssetPower;
		}

	if (retiTargetPower)
		{
		int iTargetPower = 0;

		for (int i = 0; i < m_Targets.GetCount(); i++)
			iTargetPower += m_Targets[i].pTarget->GetCombatPower();

		*retiTargetPower = iTargetPower;
		}
	}

bool CFleetCommandAI::ComputeFormationReady (void)

//	ComputeFormationReady
//
//	Returns TRUE if all our ships are in proper formation

	{
	Metric rMaxVel = (g_KlicksPerPixel / 4.0);
	Metric rMaxVel2 = rMaxVel * rMaxVel;

	for (int i = 0; i < m_Assets.GetCount(); i++)
		if (m_Assets[i].pAsset->GetVel().Length2() > rMaxVel2)
			return false;

	return true;
	}

CVector CFleetCommandAI::ComputeRallyPoint (CSpaceObject *pTarget, int *retiFacing)

//	ComputeRallyPoint
//
//	Compute the point at which to rally to attack pTarget.
//	We choose a point that is some distance away and aligned on a directional
//	angle.

	{
	//	Compute the current bearing and distance to the target

	Metric rTargetDist;
	int iTargetBearing = VectorToPolar(pTarget->GetPos() - m_pShip->GetPos(), &rTargetDist);

	//	Compute rally point

	return ComputeRallyPointEx(iTargetBearing, pTarget, retiFacing);
	}

CVector CFleetCommandAI::ComputeRallyPointEx (int iBearing, CSpaceObject *pTarget, int *retiFacing)

//	ComputeRallyPointEx
//
//	Compute the point at which to rally to attack pTarget based on an initial angle

	{
	//	Reverse the angle and align to a direction

	int iAngle = m_pShip->AlignToRotationAngle((iBearing + 180) % 360);

	//	Randomly offset (+/- rotation angle)

	iAngle = ((iAngle + (mathRandom(-1, 1) * 18) + 360) % 360);

	//	Compute the distance from target. If we're a stand-off platform, make sure we
	//	rally inside of our weapon range.

	Metric rDistance;
	if (m_AICtx.GetCombatStyle() == AICombatStyle::StandOff)
		{
		m_AICtx.CalcBestWeapon(m_pShip, NULL, 0.0);
		CInstalledDevice *pBestWeapon = m_AICtx.GetBestWeapon();
		if (pBestWeapon)
			rDistance = pBestWeapon->GetMaxEffectiveRange(m_pShip, pTarget);
		else
			rDistance = RALLY_RANGE;
		}
	else
		rDistance = RALLY_RANGE;

	//	Return the facing

	if (retiFacing)
		*retiFacing = ((iAngle + 180) % 360);

	//	Return vector

	return pTarget->GetPos() + PolarToVector(iAngle, rDistance);
	}

CString CFleetCommandAI::DebugCrashInfo (void)

//	DebugCrashInfo
//
//	Returns debug crash info

	{
	CString sResult;

	sResult.Append(CONSTLIT("CFleetCommandAI\r\n"));
	sResult.Append(strPatternSubst(CONSTLIT("Order: %d\r\n"), (int)GetCurrentOrder()));
	sResult.Append(strPatternSubst(CONSTLIT("m_State: %d\r\n"), m_State));
	sResult.Append(strPatternSubst(CONSTLIT("m_pObjective: %s\r\n"), CSpaceObject::DebugDescribe(m_pObjective)));
	sResult.Append(strPatternSubst(CONSTLIT("m_pTarget: %s\r\n"), CSpaceObject::DebugDescribe(m_pTarget)));

	for (int i = 0; i < m_Assets.GetCount(); i++)
		{
		sResult.Append(strPatternSubst(CONSTLIT("Asset[%d].pAsset: %s\r\n"), i, CSpaceObject::DebugDescribe(m_Assets[i].pAsset)));
		sResult.Append(strPatternSubst(CONSTLIT("Asset[%d].pAsset: %s\r\n"), i, CSpaceObject::DebugDescribe(m_Assets[i].pTarget)));
		}

	for (int i = 0; i < m_Targets.GetCount(); i++)
		sResult.Append(strPatternSubst(CONSTLIT("Target[%d].pTarget: %s\r\n"), i, CSpaceObject::DebugDescribe(m_Targets[i].pTarget)));

	return sResult;
	}

CFleetCommandAI::SAsset *CFleetCommandAI::FindAsset (CSpaceObject *pAsset, int *retiIndex)

//	FindAsset
//
//	Returns the asset record

	{
	for (int i = 0; i < m_Assets.GetCount(); i++)
		if (m_Assets[i].pAsset == pAsset)
			{
			if (retiIndex)
				*retiIndex = i;
			return &m_Assets[i];
			}

	return NULL;
	}

CFleetCommandAI::STarget *CFleetCommandAI::FindTarget (CSpaceObject *pTarget, int *retiIndex)

//	FindTarget
//
//	Returns the target record

	{
	for (int i = 0; i < m_Targets.GetCount(); i++)
		if (m_Targets[i].pTarget == pTarget)
			{
			if (retiIndex)
				*retiIndex = i;
			return &m_Targets[i];
			}

	return NULL;
	}

void CFleetCommandAI::ImplementAttackAtWill (void)

//	ImplementAttackAtWill
//
//	Attack objective and all enemies

	{
	//	Attack our objective

	m_AICtx.ImplementAttackTarget(m_pShip, m_pObjective);

	//	Recalc our targets

	if (m_pShip->IsDestinyTime(31))
		UpdateTargetList();

	//	Loop over all our targets and make sure that they have enough ships
	//	taking care of them.

	if (m_pShip->IsDestinyTime(17))
		{
		UpdateAttackTargets();
		OrderAttackTarget(m_pObjective);
		}

	//	If we are outclassed, then we should re-group

	if (m_pShip->IsDestinyTime(53))
		{
		int iAssetPower, iEnemyPower;
		ComputeCombatPower(&iAssetPower, &iEnemyPower);

		if (iEnemyPower > iAssetPower)
			{
			OrderAllFormUp(true);
			SetState(stateNone);
			}
		}
	}

void CFleetCommandAI::ImplementAttackFromRallyPoint (void)

//	ImplementAttackFromRallyPoint
//
//	Attack objective from rally point

	{
	//	Stay in position

	m_AICtx.ImplementFormationManeuver(m_pShip, m_vRallyPoint, NullVector, m_pShip->AlignToRotationAngle(m_iRallyFacing));

	//	Fire our weapon
	
	m_AICtx.ImplementFireWeapon(m_pShip);

	//	Order all ships to open fire

	if (!m_fOpenFireOrdered)
		{
		OrderAllOpenFire();
		m_fOpenFireOrdered = true;
		}

	//	If we're done, move to a new location

	if (--m_iCounter <= 0)
		{
		SetState(stateFormAtRallyPoint);
		m_pObjective = GetCurrentOrderTarget();

		int iAngle = (m_iRallyFacing + 360 + mathRandom(-90, 90)) % 360;
		m_vRallyPoint = ComputeRallyPointEx(iAngle, m_pObjective, &m_iRallyFacing);

		OrderAllFormUp();
		}
	}

void CFleetCommandAI::ImplementChargeInFormation (void)

//	ImplementChargeInFormation
//
//	Charge towards objective

	{
	m_AICtx.ImplementFormationManeuver(m_pShip, m_pObjective->GetPos(), NullVector, m_pShip->AlignToRotationAngle(m_iRallyFacing));

	//	Compute our distance to the target

	CVector vDist = m_pObjective->GetPos() - m_pShip->GetPos();
	Metric rDist = vDist.Length();

	//	If we're within the break distance, then scatter

	if (rDist < BREAK_CHARGE_RANGE)
		{
#ifdef DEBUG_FLEET_COMMAND
		m_pShip->GetPlayerShip()->SendMessage(m_pShip, CONSTLIT("Break charge range reached!"));
#endif

		//	Order fleet to break and attack targets

		OrderBreakAndAttack();

		//	Any ships with no target should attack the objective 

		OrderAttackTarget(m_pObjective);
		}

	//	Otherwise, fire as appropriate

	else if (rDist < CHARGE_ATTACK_RANGE)
		{
		//	Fire blindly

		m_AICtx.ImplementFireWeapon(m_pShip);

		//	Order all ships to open fire

		if (!m_fOpenFireOrdered)
			{
#ifdef DEBUG_FLEET_COMMAND
			m_pShip->GetPlayerShip()->SendMessage(m_pShip, CONSTLIT("Open fire!"));
#endif

			OrderAllOpenFire();
			m_fOpenFireOrdered = true;
			}
		}
	}

void CFleetCommandAI::ImplementFormAtRallyPoint (void)

//	ImplementFormAtRallyPoint
//
//	Assemble the fleet at the rally point

	{
	//	Move to rally point

	m_AICtx.ImplementFormationManeuver(m_pShip, m_vRallyPoint, NullVector, m_pShip->AlignToRotationAngle(m_iRallyFacing));

	//	Every once in a while check to see if all our ships are in formation

	if (m_pShip->IsDestinyTime(29))
		{
		//	See if we are at the proper position. If not, bail out

		CVector vDist = (m_vRallyPoint - m_pShip->GetPos());
		Metric rDist2 = vDist.Length2();
		if (rDist2 > (4.0 * g_KlicksPerPixel * g_KlicksPerPixel))
			return;

		//	See if all our ships are in proper formation. If not, we're done

		if (!ComputeFormationReady())
			return;

		//	We are at the rally point, do the appropriate action based on 
		//	the kind of ship that we are.

		if (m_AICtx.GetCombatStyle() == AICombatStyle::StandOff)
			{
			SetState(stateAttackFromRallyPoint);
			m_pObjective = GetCurrentOrderTarget();
			m_iCounter = mathRandom(180, 360);
			m_fOpenFireOrdered = false;
			}
		else
			{
			SetState(stateChargeInFormation);
			m_pObjective = GetCurrentOrderTarget();
			m_fOpenFireOrdered = false;
			}

#ifdef DEBUG_FLEET_COMMAND
		m_pShip->GetPlayerShip()->SendMessage(m_pShip, CONSTLIT("Rally point reached"));
#endif
		}
	}

void CFleetCommandAI::ImplementWaitAtPosition (void)

//	ImplementWaitAtPosition
//
//	Wait at the current location

	{
	//	If we've lost a lot of ships, then attack a target

	if (m_iStartingAssetCount > 2 * m_Assets.GetCount())
		{
		if (m_pTarget == NULL)
			m_pTarget = m_pShip->GetNearestVisibleEnemy(ATTACK_RANGE);
		if (m_pTarget)
			m_AICtx.ImplementAttackTarget(m_pShip, m_pTarget);
		else
			m_AICtx.ImplementHold(m_pShip);
		}
	else
		{
		m_AICtx.ImplementAttackNearestTarget(m_pShip, ATTACK_RANGE, &m_pTarget);
		m_AICtx.ImplementFireOnTargetsOfOpportunity(m_pShip);

		//	Avoid target

		if (m_pTarget)
			{
			CVector vTarget = m_pTarget->GetPos() - m_pShip->GetPos();
			m_AICtx.ImplementSpiralOut(m_pShip, vTarget);
			}
		else
			m_AICtx.ImplementHold(m_pShip);
		}

	//	Recalc our targets

	if (m_pShip->IsDestinyTime(31))
		UpdateTargetList();

	//	Loop over all our targets and make sure that they have enough ships
	//	taking care of them.

	if (m_pShip->IsDestinyTime(17))
		UpdateAttackTargets();

	//	See if we're done waiting

	if (m_iCounter > 0)
		{
		if (--m_iCounter == 0)
			{
			if (GetCurrentOrder() == IShipController::orderWait)
				CancelCurrentOrder();

			SetState(stateNone);
			}
		}
	}

DWORD CFleetCommandAI::OnCommunicate (CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2, ICCItem *pData)

//	Communicate
//
//	Handle communications from other objects

	{
	switch (iMessage)
		{
		case msgEscortReportingIn:
			{
			if (FindAsset(pSender) == NULL)
				{
				SAsset *pAsset = AddAsset(pSender);

				//	Enter formation

				pAsset->iFormationPos = m_Assets.GetCount() - 1;
				DWORD dwFormation = MAKELONG(pAsset->iFormationPos, 0);
				m_pShip->Communicate(pSender, msgFormUp, NULL, dwFormation);
				}
			return resAck;
			}

		default:
			return resNoAnswer;
		}
	}

void CFleetCommandAI::OnDockedEvent (CSpaceObject *pObj)

//	OnDockedEvent
//
//	Event when the ship is docked

	{
	SetState(stateNone);
	}

void CFleetCommandAI::OnNewSystemNotify (void)

//	OnNewSystemNotify
//
//	We find ourselves in a different system

	{
	CSystem *pNewSystem = m_pShip->GetSystem();

	//	Remove any assets that are not in the new system

	int iNewCount = m_Assets.GetCount();
	for (int i = 0; i < m_Assets.GetCount(); i++)
		{
		if (m_Assets[i].pAsset->GetSystem() != pNewSystem)
			{
			IShipController::OrderTypes iOrder = m_Assets[i].pAsset->AsShip()->GetController()->GetCurrentOrderDesc().GetOrder();
			m_Assets[i].pAsset = NULL;
			iNewCount--;
			}
		else
			m_Assets[i].pTarget = NULL;
		}

	//	New array

	if (iNewCount != m_Assets.GetCount())
		{
		TArray<SAsset> NewArray;
		NewArray.GrowToFit(iNewCount);

		for (int i = 0; i < m_Assets.GetCount(); i++)
			if (m_Assets[i].pAsset)
				NewArray.Insert(m_Assets[i]);

		m_Assets = std::move(NewArray);
		}

	//	Remove all targets

	m_Targets.DeleteAll();
	}

void CFleetCommandAI::OnObjDestroyedNotify (const SDestroyCtx &Ctx)

//	OnObjDestroyedNotify
//
//	Handle the case where another object is destroyed

	{
	//	Look for the object in the asset and target arrays

	STarget *pTarget;
	int iIndex;

	//	Remove the object from the asset data structure. If the
	//	object was a target, this also clears the pTarget variable
	//	(which makes the ship eligible for re-targeting).

	if (RemoveAssetObj(&Ctx.Obj))
		{
		//	If one of our assets was destroyed, check to see if it was
		//	destroyed by a target.

		if (Ctx.Attacker.GetObj() 
				&& (pTarget = FindTarget(Ctx.Attacker.GetObj())) != NULL)
			pTarget->iKilled += Ctx.Obj.GetCombatPower();

		//	If our asset was attacking a target, remove it from the assignedTo
		//	field. (Obviously this could be a different enemy than the one
		//	that killed the asset).

		if ((pTarget = FindTarget(Ctx.Obj.GetTarget())) != NULL)
			pTarget->iAssignedTo -= Ctx.Obj.GetCombatPower();
		}

	//	Otherwise, check to see if a target was destroyed

	else if ((pTarget = FindTarget(&Ctx.Obj, &iIndex)) != NULL)
		RemoveTarget(iIndex);

	//	Act based on state

	switch (m_State)
		{
		case stateAttackAtWill:
		case stateChargeInFormation:
		case stateFormAtRallyPoint:
		case stateAttackFromRallyPoint:
		case stateWaitingForThreat:
			{
			if (m_pObjective == Ctx.Obj)
				CancelCurrentOrder();
			break;
			}
		}

	//	Generic reset

	if (m_pObjective == Ctx.Obj)
		m_pObjective = NULL;
	if (m_pTarget == Ctx.Obj)
		m_pTarget = NULL;
	}

void CFleetCommandAI::OnOrderChanged (void)

//	OnOrderChanged
//
//	The order list has changed

	{
	SetState(stateNone);
	}

void CFleetCommandAI::OnReadFromStream (SLoadCtx &Ctx)

//	OnReadFromStream
//
//	Read our data
//
//	DWORD		m_State
//	DWORD		m_pTarget (CSpaceObject ref)
//	DWORD		m_pObjective (CSpaceObject ref)
//	DWORD		m_iCounter
//	DWORD		m_iStartingAssetCount
//
//	DWORD		no. of assets
//	SAsset[]
//
//	DWORD		no. of targets
//	STarget[]
//
//	CVector		m_vThreatPotential
//	CVector		m_vRallyPoint
//	DWORD		m_iRallyFacing
//
//	DWORD		flags

	{
	DWORD dwLoad;

	Ctx.pStream->Read(dwLoad);
	m_State = (StateTypes)dwLoad;
	CSystem::ReadObjRefFromStream(Ctx, &m_pTarget);
	CSystem::ReadObjRefFromStream(Ctx, &m_pObjective);
	Ctx.pStream->Read(m_iCounter);
	Ctx.pStream->Read(m_iStartingAssetCount);

	int iAssetCount;
	Ctx.pStream->Read(iAssetCount);
	if (iAssetCount)
		{
		m_Assets.InsertEmpty(iAssetCount);

		for (int i = 0; i < iAssetCount; i++)
			{
			CSystem::ReadObjRefFromStream(Ctx, &m_Assets[i].pAsset);
			Ctx.pStream->Read(m_Assets[i].iFormationPos);
			CSystem::ReadObjRefFromStream(Ctx, &m_Assets[i].pTarget);
			}
		}

	int iTargetCount;
	Ctx.pStream->Read(iTargetCount);
	if (iTargetCount)
		{
		m_Targets.InsertEmpty(iTargetCount);

		for (int i = 0; i < iTargetCount; i++)
			{
			CSystem::ReadObjRefFromStream(Ctx, &m_Targets[i].pTarget);
			Ctx.pStream->Read(m_Targets[i].iAssignedTo);
			Ctx.pStream->Read(m_Targets[i].iKilled);
			}
		}

	m_vThreatPotential.ReadFromStream(*Ctx.pStream);
	m_vRallyPoint.ReadFromStream(*Ctx.pStream);
	Ctx.pStream->Read(m_iRallyFacing);

	Ctx.pStream->Read(dwLoad);
	m_fOpenFireOrdered =		((dwLoad & 0x00000001) ? true : false);
	}

void CFleetCommandAI::OnWriteToStream (IWriteStream *pStream)

//	OnWriteToStream
//
//	Write our data
//
//	DWORD		m_State
//	DWORD		m_pTarget (CSpaceObject ref)
//	DWORD		m_pObjective (CSpaceObject ref)
//	DWORD		m_iCounter
//	DWORD		m_iStartingAssetCount
//
//	DWORD		no. of assets
//	SAsset[]
//
//	DWORD		no. of targets
//	STarget[]
//
//	CVector		m_vThreatPotential
//	CVector		m_vRallyPoint
//	DWORD		m_iRallyFacing
//
//	DWORD		flags

	{
	DWORD dwSave;

	dwSave = m_State;
	pStream->Write(dwSave);
	m_pShip->WriteObjRefToStream(m_pTarget, pStream);
	m_pShip->WriteObjRefToStream(m_pObjective, pStream);
	pStream->Write(m_iCounter);
	pStream->Write(m_iStartingAssetCount);

	pStream->Write(m_Assets.GetCount());
	for (int i = 0; i < m_Assets.GetCount(); i++)
		{
		m_pShip->WriteObjRefToStream(m_Assets[i].pAsset, pStream);
		pStream->Write(m_Assets[i].iFormationPos);
		m_pShip->WriteObjRefToStream(m_Assets[i].pTarget, pStream);
		}

	pStream->Write(m_Targets.GetCount());
	for (int i = 0; i < m_Targets.GetCount(); i++)
		{
		m_pShip->WriteObjRefToStream(m_Targets[i].pTarget, pStream);
		pStream->Write(m_Targets[i].iAssignedTo);
		pStream->Write(m_Targets[i].iKilled);
		}

	m_vThreatPotential.WriteToStream(*pStream);
	m_vRallyPoint.WriteToStream(*pStream);
	pStream->Write(m_iRallyFacing);

	dwSave = 0;
	dwSave |= (m_fOpenFireOrdered ?			0x00000001 : 0);
	pStream->Write(dwSave);
	}

void CFleetCommandAI::OrderAllFormUp (bool bResetFormation)

//	OrderAllFormUp
//
//	Order all ships to form up

	{
	//	Form up

	for (int i = 0; i < m_Assets.GetCount(); i++)
		{
		m_Assets[i].pTarget = NULL;

		if (bResetFormation)
			m_Assets[i].iFormationPos = i;

		DWORD dwFormation = MAKELONG(m_Assets[i].iFormationPos, 0);
		m_pShip->Communicate(m_Assets[i].pAsset, msgFormUp, m_pShip, dwFormation);
		}

	//	No targets are being attacked

	for (int i = 0; i < m_Targets.GetCount(); i++)
		m_Targets[i].iAssignedTo = 0;
	}

void CFleetCommandAI::OrderAllOpenFire (void)

//	OrderAllOpenFire
//
//	Order all ships to open fire in formation

	{
	for (int i = 0; i < m_Assets.GetCount(); i++)
		m_pShip->Communicate(m_Assets[i].pAsset, msgAttackInFormation);
	}

void CFleetCommandAI::OrderAttackTarget (CSpaceObject *pTarget)

//	OrderAttackTarget
//
//	Order all ships that are not assigned to attack the target

	{
	if (pTarget == NULL)
		return;

	for (int i = 0; i < m_Assets.GetCount(); i++)
		if (m_Assets[i].pTarget == NULL)
			{
			m_pShip->Communicate(m_Assets[i].pAsset, msgAttack, pTarget);
			m_Assets[i].pTarget = pTarget;
			}
	}

void CFleetCommandAI::OrderBreakAndAttack ()

//	OrderBreakAndAttack
//
//	Attack at will.

	{
	//	NOTE: This is only valid for destroy target orders because it relies on
	//	the fact that the order target is an enemy. Also, stateAttackAtWill does
	//	not properly handle going back to formation after the enemy is 
	//	destroyed.

	if (GetCurrentOrder() != orderDestroyTarget)
		return;

	SetState(stateAttackAtWill);
	m_pObjective = GetCurrentOrderTarget();
	UpdateTargetList();
	UpdateAttackTargets();
	}

void CFleetCommandAI::RemoveAsset (int iIndex)

//	RemoveAsset
//
//	Removes the entry from the array

	{
	m_Assets.Delete(iIndex);
	}

bool CFleetCommandAI::RemoveAssetObj (CSpaceObject *pObj)

//	RemoveAssetObj
//
//	Removes the object from the asset data structure (either as an asset or
//	as a target). Returns TRUE if an asset was removed.

	{
	for (int i = 0; i < m_Assets.GetCount(); i++)
		{
		if (m_Assets[i].pAsset == pObj)
			{
			RemoveAsset(i);
			return true;
			}
		else if (m_Assets[i].pTarget == pObj)
			{
			m_Assets[i].pTarget = NULL;

			//	Order this asset to form-up

			DWORD dwFormation = MAKELONG(m_Assets[i].iFormationPos, 0);
			m_pShip->Communicate(m_Assets[i].pAsset, msgFormUp, m_pShip, dwFormation);
			}
		}

	return false;
	}

void CFleetCommandAI::RemoveTarget (int iIndex)

//	RemoveTarget
//
//	Removes the entry from the array

	{
	m_Targets.Delete(iIndex);
	}

void CFleetCommandAI::SetState (StateTypes State)

//	SetState
//
//	Sets the current state

	{
	//	Set state (NOTE: We do this before we undock because the Undock
	//	call may destroy the station and cause us to recurse into SetState.
	//	This happens when a ship is guarding an empty cargo crate).

	m_State = State;
	m_pTarget = NULL;
	m_pObjective = NULL;

	//	If we're currently docked and we're changing to a state that
	//	does not support docking, then we undock first.

	if (m_pShip->GetDockedObj()
			&& State != stateNone) 
		m_pShip->Undock();
	}

void CFleetCommandAI::UpdateAttackTargets (void)

//	UpdateAttackTargets
//
//	Updates to make sure that all targets are being attacked

	{
	//	See if any targets have wandered out of our intercept range
	//	If so, call off the assets

	for (int i = 0; i < m_Targets.GetCount(); i++)
		{
		CVector vDist = m_Targets[i].pTarget->GetPos() - m_pShip->GetPos();
		if (vDist.Length2() > INTERCEPT_THREAT_RANGE2)
			{
			RemoveAssetObj(m_Targets[i].pTarget);

			RemoveTarget(i);
			i--;
			}
		}

	//	Make sure all targets are being attacked

	int iNextTarget = 0;
	int iNextAsset = 0;

	while (iNextAsset < m_Assets.GetCount() && iNextTarget < m_Targets.GetCount())
		{
		int iPower = m_Targets[iNextTarget].pTarget->GetCombatPower();
		
		while (iPower > m_Targets[iNextTarget].iAssignedTo
				&& iNextAsset < m_Assets.GetCount())
			{
			if (m_Assets[iNextAsset].pTarget == NULL)
				{
				//	Assign the asset to attack the target

				m_pShip->Communicate(m_Assets[iNextAsset].pAsset, msgAttack, m_Targets[iNextTarget].pTarget);
				m_Assets[iNextAsset].pTarget = m_Targets[iNextTarget].pTarget;
				m_Targets[iNextTarget].iAssignedTo += m_Assets[iNextAsset].pAsset->GetCombatPower();
				}

			iNextAsset++;
			}

		iNextTarget++;
		}
	}

void CFleetCommandAI::UpdateTargetList (void)

//	UpdateTargetList
//
//	Updates the targets array 

	{
	int i;
	CVector vPotential;

	//	Get the sovereign

	CSovereign *pSovereign = m_pShip->GetSovereignToDefend();
	if (pSovereign == NULL || m_pShip->GetSystem() == NULL)
		return;

	//	Get the list of enemy objects

	const CSpaceObjectList &ObjList = pSovereign->GetEnemyObjectList(m_pShip->GetSystem());
	CSpaceObject *pPrincipal = m_pShip->GetEscortPrincipal();

	//	Iterate over all objects

	int iCount = ObjList.GetCount();
	for (i = 0; i < iCount; i++)
		{
		CSpaceObject *pObj = ObjList.GetObj(i);

		if (pObj != m_pObjective
				&& pObj->CanAttack()
				&& pObj != m_pShip)
			{
			CVector vTarget = pObj->GetPos() - m_pShip->GetPos();
			Metric rTargetDist2 = vTarget.Dot(vTarget);

			if (rTargetDist2 < INTERCEPT_THREAT_RANGE2
					&& rTargetDist2 < pObj->GetDetectionRange2(DEFAULT_PERCEPTION)
					&& !pObj->IsEscortingFriendOf(m_pShip)
					&& (pPrincipal == NULL || !pPrincipal->IsFriend(pObj)))
				{
				if (FindTarget(pObj) == NULL)
					AddTarget(pObj);
				}
			}
		}
	}

void CFleetCommandAI::UpdateTargetListAndPotentials (void)

//	UpdateTargetListAndPotentials
//
//	Updates the targets array and the potential threat vector

	{
	int i;
	CVector vPotential;

	//	Iterate over all objects

	for (i = 0; i < m_pShip->GetSystem()->GetObjectCount(); i++)
		{
		CSpaceObject *pObj = m_pShip->GetSystem()->GetObject(i);

		if (pObj  == NULL || pObj == m_pObjective || pObj == m_pShip || !pObj->CanAttack())
			NULL;

		//	Compute the influence of ships (both friendly and enemy)

		else if (pObj->GetCategory() == CSpaceObject::catShip)
			{
			CVector vTarget = pObj->GetPos() - m_pObjective->GetPos();
			Metric rTargetDist2 = vTarget.Dot(vTarget);

			if (rTargetDist2 < MAX_DETECTION_RANGE2)
				{
				//	Friendly ships (that are not part of this fleet) count as
				//	protection.

				if (pObj->GetSovereign() == m_pShip->GetSovereign()
						&& rTargetDist2 < INTERCEPT_THREAT_RANGE2
						&& m_pShip->Communicate(pObj, msgQueryFleetStatus, m_pShip) != resAck)
					{
					CVector vTargetN = vTarget.Normal();
					int iPower = pObj->GetCombatPower();

					//	Reduce threat level in that direction, proportional to power of friend

					vPotential = vPotential - (vTargetN * (Metric)iPower * g_KlicksPerPixel);

					//	If we're really close, then avoid the friendly ship

					CVector vSeparation = pObj->GetPos() - m_pShip->GetPos();
					Metric rSeparationDist2 = vSeparation.Dot(vSeparation);
					if (rSeparationDist2 < SEPARATION_RANGE2)
						{
						Metric rSeparationDist;
						CVector vSeparationN = vSeparation.Normal(&rSeparationDist);
						if (rSeparationDist == 0.0)
							{
							rSeparationDist = g_KlicksPerPixel;
							vSeparationN = PolarToVector(m_pShip->GetDestiny(), 1.0);
							}
						vPotential = vPotential - (vSeparationN * (SEPARATION_RANGE / rSeparationDist) * g_KlicksPerPixel);
						}
					}

				//	Enemy ships within our range of detection count as threats

				else if (pObj->IsEnemy(m_pObjective)
						&& rTargetDist2 < pObj->GetDetectionRange2(DEFAULT_PERCEPTION))
					{
					//	If the threat is inside a certain range, then we add it as a target
					//	to our list. Otherwise, it contributes to the potential threat vector

					if (rTargetDist2 < INTERCEPT_THREAT_RANGE2)
						{
						if (FindTarget(pObj) == NULL)
							AddTarget(pObj);
						}
					else
						{
						Metric rDist;
						CVector vTargetN = vTarget.Normal(&rDist);
						int iPower = pObj->GetCombatPower();

						vPotential = vPotential + (vTargetN * (Metric)iPower * (MAX_THREAT_RANGE / rDist) * g_KlicksPerPixel);
						}
					}
				}
			}
		}

	//	Set threat potential vector

	m_vThreatPotential = vPotential;
	}
