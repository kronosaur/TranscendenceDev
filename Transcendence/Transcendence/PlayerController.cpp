//	PlayerController.cpp
//
//	Implements class to control player's ship
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Transcendence.h"

#define INVOKE_REFRESH_INTERVAL					100		//	Ticks to gain 1 point of deity rel (if rel is negative)

#define CMD_PLAYER_COMBAT_ENDED					CONSTLIT("playerCombatEnded")
#define CMD_PLAYER_COMBAT_STARTED				CONSTLIT("playerCombatStarted")

const Metric MAX_IN_COMBAT_RANGE =				LIGHT_SECOND * 50.0;
const int UPDATE_HELP_TIME =					31;
const Metric MAX_AUTO_TARGET_DISTANCE =			LIGHT_SECOND * 30.0;
const Metric MAX_DOCK_DISTANCE =				KLICKS_PER_PIXEL * 256.0;
const Metric MAX_DOCK_DISTANCE2 =				MAX_DOCK_DISTANCE * MAX_DOCK_DISTANCE;
const Metric MIN_PORT_ANIMATION_DIST =			KLICKS_PER_PIXEL * 2;
const Metric MIN_PORT_ANIMATION_DIST2 =			MIN_PORT_ANIMATION_DIST * MIN_PORT_ANIMATION_DIST;

const DWORD FIRE_THRESHOLD =					150;
const DWORD HIT_THRESHOLD =						90;

const DWORD FIRE_THRESHOLD_EXIT =				600;
const DWORD HIT_THRESHOLD_EXIT =				150;

const DWORD DAMAGE_BAR_TIMER =					30 * 5;

#define FIELD_ID								CONSTLIT("id")
#define FIELD_ORE_LEVEL							CONSTLIT("oreLevel")

#define HINT_MINING_NEED_BETTER_WEAPON			CONSTLIT("mining.needBetterWeapon")

#define PROPERTY_CHARACTER_CLASS				CONSTLIT("characterClass")
#define PROPERTY_STARTING_SYSTEM				CONSTLIT("startingSystem")

#define SETTING_ENABLED							CONSTLIT("enabled")
#define SETTING_TRUE							CONSTLIT("true")

CPlayerShipController::CPlayerShipController (CUniverse &Universe) : 
		m_Universe(Universe),
		m_Stats(Universe)

//	CPlayerShipController constructor

	{
	}

CPlayerShipController::~CPlayerShipController (void)

//	CPlayerShipController destructor

	{
	if (m_pDebugNavPath)
		delete m_pDebugNavPath;
	}

void CPlayerShipController::AddOrder (const COrderDesc &OrderDesc, bool bAddBefore)

//	AddOrder
//
//	We get an order

	{
	//	If we've got no system then we ignore this. This can happen if we try to set
	//	an order in the middle of gating. For example, if we fail a mission due
	//	to gating away.

	if (m_pShip->GetSystem() == NULL)
		return;

	//	For now, we only deal with one order at a time

	switch (OrderDesc.GetOrder())
		{
		case orderGuard:
		case orderEscort:
			SetDestination(OrderDesc.GetTarget());
			break;

		case orderDock:
		case orderGate:
			SetDestination(OrderDesc.GetTarget(), OPTION_HIGHLIGHT);
			break;

		case orderAimAtTarget:
		case orderDestroyTarget:
			if (m_pShip->HasTargetingComputer())
				SetTarget(OrderDesc.GetTarget());

			SetDestination(OrderDesc.GetTarget(), OPTION_HIGHLIGHT);
			break;
		}

	//	Remember our orders

	m_OrderDesc = OrderDesc;
	}

bool CPlayerShipController::AreAllDevicesEnabled (void)

//	AreAllDevicesEnabled
//
//	Returns TRUE if all devices are enabled. Returns FALSE is one or more devices
//	are disabled.

	{
	for (CDeviceItem DeviceItem : m_pShip->GetDeviceSystem())
		{
		CInstalledDevice &Device = *DeviceItem.GetInstalledDevice();
		CItemCtx ItemCtx(m_pShip, &Device);
		if (Device.CanBeDisabled(ItemCtx))
			{
			if (!Device.IsEnabled())
				return false;
			}
		}

	return true;
	}

void CPlayerShipController::Behavior (SUpdateCtx &Ctx)

//	Behavior
//
//	Update the behavior of the ship.

	{
	m_ManeuverController.Update(Ctx, m_pShip);
	}

bool CPlayerShipController::CanShowShipStatus (void)

//	CanShowShowStatus
//
//	Returns TRUE if we're allowed to show ship's status screen.

	{
	//	If we're trying to dock, then we can't

	if (DockingInProgress())
		return false;

	//	See if we have an overlay preventing us from bring it up

	COverlay::SImpactDesc Impact;
	if (m_pShip->GetOverlayImpact(Impact) 
			&& Impact.Conditions.IsSet(ECondition::shipScreenDisabled))
		return false;

	//	We're OK

	return true;
	}

void CPlayerShipController::CancelAllOrders (void)

//	CancelAllOrder
//
//	Cancel all orders

	{
	m_OrderDesc = COrderDesc();
	SetDestination(NULL);
	}

void CPlayerShipController::CancelCurrentOrder (void)

//	CancelCurrentOrder
//
//	Cancel current order

	{
	m_OrderDesc = COrderDesc();
	SetDestination(NULL);
	}

bool CPlayerShipController::CancelOrder (int iIndex)

//	CancelOrder
//
//	Cancels an order

	{
	if (iIndex != 0)
		return false;

	CancelCurrentOrder();
	return true;
	}

void CPlayerShipController::CancelDocking (void)

//	CancelDocking
//
//	Cancel docking (if we're in the middle of docking)

	{
	if (m_pStation)
		{
		m_pStation->Undock(m_pShip);
		m_pStation = NULL;
		DisplayTranslate(CONSTLIT("msgDockingCanceled"));
		}
	}

void CPlayerShipController::Communications (CSpaceObject *pObj, 
											MessageTypes iMsg, 
											DWORD dwData,
											DWORD *iodwFormationPlace)

//	Communications
//
//	Send a message to the given object

	{
	switch (iMsg)
		{
		case msgFormUp:
			{
			DWORD dwFormation;
			if (dwData == 0xffffffff)
				dwFormation = dwData;
			else if (iodwFormationPlace)
				dwFormation = MAKELONG(*iodwFormationPlace, (DWORD)dwData);
			else
				dwFormation = MAKELONG(0, (DWORD)dwData);

			DWORD dwRes = m_pShip->Communicate(pObj, iMsg, m_pShip, dwFormation);
			if (dwRes == resAck)
				if (iodwFormationPlace)
					(*iodwFormationPlace)++;
			break;
			}

		case msgAttack:
			{
			if (m_pTarget)
				m_pShip->Communicate(pObj, msgAttack, m_pTarget);
			else
				DisplayTranslate(CONSTLIT("msgNoTargetForFleet"));

			break;
			}

		default:
			m_pShip->Communicate(pObj, iMsg);
			break;
		}
	}

void CPlayerShipController::ClearFireAngle (void)

//	ClearFireAngle
//
//	Clears the fire angle of weapon and launcher

	{
	for (CDeviceItem DeviceItem : m_pShip->GetDeviceSystem())
		{
		CInstalledDevice &Device = *DeviceItem.GetInstalledDevice();
		if ((Device.GetCategory() == itemcatWeapon 
					|| Device.GetCategory() == itemcatLauncher))
			Device.SetFireAngle(-1);
		}
	}

void CPlayerShipController::CycleTarget (int iDir)

//	CycleTarget
//
//	Cycles between enemy target, friendly target, and no target.

	{
	if (!m_pTarget)
		{
		if (iDir == 0)
			SelectNextTarget();
		}
	else if (m_pShip->IsAngryAt(m_pTarget))
		{
		if (iDir == 0)
			{
			SetTarget(NULL);
			SelectNextFriendly();
			}
		else
			SelectNextTarget(iDir);
		}
	else
		{
		if (iDir == 0)
			SetTarget(NULL);
		else
			SelectNextFriendly(iDir);
		}
	}

CString CPlayerShipController::DebugCrashInfo (void)

//	DebugCrashInfo
//
//	Returns debug crash info

	{
	int i;
	CString sResult;

	sResult.Append(CONSTLIT("CPlayerShipController\r\n"));
	sResult.Append(strPatternSubst(CONSTLIT("m_pStation: %s\r\n"), CSpaceObject::DebugDescribe(m_pStation)));
	sResult.Append(strPatternSubst(CONSTLIT("m_pTarget: %s\r\n"), CSpaceObject::DebugDescribe(m_pTarget)));
	sResult.Append(strPatternSubst(CONSTLIT("m_pDestination: %s\r\n"), CSpaceObject::DebugDescribe(m_pDestination)));

	for (i = 0; i < m_TargetList.GetCount(); i++)
		sResult.Append(strPatternSubst(CONSTLIT("m_TargetList[%d]: %s\r\n"), i, CSpaceObject::DebugDescribe(m_TargetList[i])));

	return sResult;
	}

void CPlayerShipController::DebugPaintInfo (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	DebugPaintInfo
//
//	Paint debug information.

	{
#ifdef DEBUG_ASTAR_PATH

	CSystem *pSystem;

	if (m_Universe.GetDebugOptions().IsShowNavPathsEnabled()
			&& m_pTarget
			&& (pSystem = m_Universe.GetCurrentSystem()))
		{
		if ((m_Universe.GetTicks() % 10) == 0)
			{
			if (m_pDebugNavPath)
				delete m_pDebugNavPath;

			CNavigationPath::Create(pSystem, m_pShip->GetSovereign(), m_pShip->GetPos(), m_pTarget->GetPos(), &m_pDebugNavPath);
			}

		if (m_pDebugNavPath)
			m_pDebugNavPath->DebugPaintInfo(*pSystem, Dest, x, y, Ctx.XForm);
		}

#endif
	}

void CPlayerShipController::DisplayCommandHint (CGameKeys::Keys iCmd, const CString &sMessage)

//	DisplayCommandHint
//
//	Displays a hint.

	{
	if (!m_pSession)
		return;

	DWORD dwVirtKey = m_pTrans->GetSettings().GetKeyMap().GetKey(iCmd);
	if (dwVirtKey == CVirtualKeyData::INVALID_VIRT_KEY)
		return;

	m_pSession->GetMessageDisplay().DisplayCommandHint(dwVirtKey, sMessage);
	}

void CPlayerShipController::DisplayMessage (const CString &sMessage)

//	DisplayMessage
//
//	Displays a message to the UI.

	{
	//	If messages redirected, then save them.

	if (m_bRedirectMessages)
		m_sRedirectMessage.Append(sMessage);
		
	//	If the game session is not set up yet, we save the message for later.

	else if (!m_pSession)
		m_SavedMessages.Insert(sMessage);

	//	Otherwise, display.

	else
		m_pSession->DisplayMessage(sMessage);
	}

void CPlayerShipController::DisplayTranslate (const CString &sID, ICCItem *pData)

//	DisplayTranslate
//
//	Displays a translated message.

	{
	DisplayMessage(Translate(sID, pData));
	}

void CPlayerShipController::DisplayTranslate (const CString &sID, const CString &sVar, const CString &sValue)

//	DisplayTranslate
//
//	Displays a translated message with a replaceable parameter.

	{
	ICCItemPtr pData(ICCItem::SymbolTable);
	pData->SetStringAt(sVar, sValue);

	DisplayTranslate(sID, pData);
	}

void CPlayerShipController::Dock (void)

//	Dock
//
//	Docks the ship with the nearest station

	{
	//	If we're already in the middle of docking, then cancel docking

	if (m_pStation)
		{
		Undock();
		DisplayTranslate(CONSTLIT("msgDockingCanceled"));
		return;
		}

	//	Figure out what to dock with. If we have a port to dock with, choose
	//	that. Otherwise, we look for any station with a docking port (even one
	//	that we can't dock with).

	int iDockPort;
	CSpaceObject *pStation;
	if (m_pAutoDock)
		{
		pStation = m_pAutoDock;
		iDockPort = m_iAutoDockPort;
		}
	else if (pStation = FindDockTarget())
		iDockPort = -1;
	else
		{
		DisplayTranslate(CONSTLIT("msgNoStationsInRange"));
		return;
		}

	//	See if the player ship allows us to dock with this station

	CString sError;
	if (!m_pShip->FireCanDockAsPlayer(pStation, &sError))
		{
		DisplayMessage(sError);
		return;
		}

	//	Request docking.
	//
	//	NOTE: The station will do a full check to see if we are allowed to dock
	//	and if there is a screen available. Thus it is OK if we make a request
	//	of a station that just lost a dock screen. This can happen because the
	//	code that looks for a station to dock with uses a heuristic to determine
	//	whether it has a docking screen. The heuristic can sometimes be stale.

	if (!pStation->ObjRequestDock(m_pShip, iDockPort))
		return;

	//	Station has agreed to allow dock...

	m_pStation = pStation;

	SetActivate(false);
	SetFireMain(false);
	SetFireMissile(false);
	SetManeuver(EManeuver::None);
	SetThrust(false);

	SetUIMessageFollowed(uimsgDockHint);
	}

void CPlayerShipController::EnableAllDevices (bool bEnable)

//	EnableAllDevices
//
//	Enables or disables all devices

	{
	for (CDeviceItem DeviceItem : m_pShip->GetDeviceSystem())
		{
		CInstalledDevice &Device = *DeviceItem.GetInstalledDevice();
		CItemCtx ItemCtx(m_pShip, &Device);
		if (Device.CanBeDisabled(ItemCtx) && Device.IsEnabled() != bEnable)
			{
			m_pShip->EnableDevice(Device.GetDeviceSlot(), bEnable);

			//	Note: We will get called at OnDeviceEnabledDisabled for each
			//	device that we touch.
			}
		}
	}

CSpaceObject *CPlayerShipController::FindDockTarget (void)

//	FindDockTarget
//
//	Finds the closest dock target.

	{
	int i;

	CSystem *pSystem = m_pShip->GetSystem();
	CSpaceObject *pStation = NULL;
	Metric rMaxDist2 = MAX_DOCK_DISTANCE * MAX_DOCK_DISTANCE;
	Metric rBestDist = rMaxDist2;

	for (i = 0; i < pSystem->GetObjectCount(); i++)
		{
		CSpaceObject *pObj = pSystem->GetObject(i);

		if (pObj 
				&& pObj->GetDockingPortCount() > 0
				&& !pObj->IsIntangible()
				&& pObj != m_pShip)
			{
			CVector vDist = pObj->GetPos() - m_pShip->GetPos();
			Metric rDist2 = vDist.Length2();

			if (rDist2 < rMaxDist2)
				{
				//	Skip if this station does not have a dock screen that we
				//	can use. It is OK to use the full (expensive) check because
				//	we've already weeded out most objects.
				//
				//	If the station is an enemy, then let it through because we
				//	want the player to get a "docking denied" message. This is safe
				//	because (a) we only check stations with ports and (b) we 
				//	later do a full check to make sure the station has docking
				//	screens.

				CSpaceObject::RequestDockResults iResult = pObj->CanObjRequestDock(m_pShip);
				if (iResult != CSpaceObject::dockingOK 
						&& iResult != CSpaceObject::dockingDenied)
					continue;

				//	If the station is inside the dock distance, check
				//	to see how close we are to a docking position.

				CVector vDockPos;
				if (pObj->GetNearestDockPort(m_pShip, &vDockPos) != -1)
					{
					Metric rDockDist2 = (vDockPos - m_pShip->GetPos()).Length2();

					if (rDockDist2 < rBestDist)
						{
						rBestDist = rDockDist2;
						pStation = pObj;
						}
					}
				}
			}
		}

	return pStation;
	}

ICCItem *CPlayerShipController::FindProperty (const CString &sProperty)

//  FindProperty
//
//  Returns the given property, or NULL if not found. Caller is responsible for
//  discarding the result if not NULL.

	{
	if (strEquals(sProperty, PROPERTY_CHARACTER_CLASS))
		return (m_pCharacterClass ? CCodeChain::CreateInteger(m_pCharacterClass->GetUNID()) : CCodeChain::CreateNil());
	else if (strEquals(sProperty, PROPERTY_STARTING_SYSTEM))
		return CCodeChain::CreateString(m_sStartingSystem);
	else
		return m_Stats.FindProperty(sProperty);
	}

void CPlayerShipController::Gate (void)

//	Gate
//
//	Enter a stargate

	{
	CSystem *pSystem = m_pShip->GetSystem();

	//	Find the stargate closest to the ship

	CSpaceObject *pStargateNearby;
	CSpaceObject *pStargate = pSystem->GetStargateInRange(m_pShip->GetPos(), &pStargateNearby);

	//	Reqest gate

	if (pStargate)
		pStargate->RequestGate(m_pShip);

	//	Else, not found

	else
		{
		if (pStargateNearby)
			DisplayTranslate(CONSTLIT("msgTooFarFromStargate"));
		else
			DisplayTranslate(CONSTLIT("msgNoStargatesInRange"));
		}
	}

void CPlayerShipController::GenerateGameStats (CGameStats &Stats, bool bGameOver)

//	GenerateGameStats
//
//	Add to the list of game stats

	{
	m_Stats.GenerateGameStats(Stats, m_pShip, bGameOver);
	}

CString CPlayerShipController::GetAISettingString (const CString &sSetting)

//	GetAISettingString
//
//	Returns custom settings

	{
	if (strEquals(sSetting, CONSTLIT("underAttack")))
		return (m_bUnderAttack ? CONSTLIT("True") : NULL_STR);
	else
		return NULL_STR;
	}

int CPlayerShipController::GetCombatPower (void)

//	GetCombatPower
//
//	Computes the combat strength of this ship
//
//	CombatPower = ((reactor-power)^0.515) / 2.5

	{
	int iReactorPower = m_pShip->GetMaxPower();
	if (iReactorPower <= 100)
		return 4;
	else if (iReactorPower <= 250)
		return 7;
	else if (iReactorPower <= 500)
		return 10;
	else if (iReactorPower <= 1000)
		return 14;
	else if (iReactorPower <= 2500)
		return 22;
	else if (iReactorPower <= 5000)
		return 32;
	else if (iReactorPower <= 10000)
		return 46;
	else if (iReactorPower <= 25000)
		return 74;
	else
		return 100;
	}

DWORD CPlayerShipController::GetCommsStatus (void)

//	GetCommsStatus
//
//	Returns the comms capabilities of all the ships that are escorting
//	the player.

	{
	int i;
	CSystem *pSystem = m_pShip->GetSystem();
	DWORD dwStatus = 0;

	for (i = 0; i < pSystem->GetObjectCount(); i++)
		{
		CSpaceObject *pObj = pSystem->GetObject(i);

		if (pObj 
				&& !pObj->IsDestroyed()
				&& pObj != m_pShip)
			dwStatus |= m_pShip->Communicate(pObj, msgQueryCommunications);
		}

	return dwStatus;
	}

const COrderDesc &CPlayerShipController::GetCurrentOrderDesc () const

//	GetCurrentOrderEx
//
//	Returns the current order

	{
	return GetOrderDesc(0);
	}

const COrderDesc &CPlayerShipController::GetOrderDesc (int iIndex) const

//	GetOrder
//
//	Returns the order

	{
	if (iIndex > 0)
		return COrderDesc::Null;

	switch (m_OrderDesc.GetOrder())
		{
		case orderGuard:
		case orderEscort:
		case orderDock:
		case orderAimAtTarget:
		case orderDestroyTarget:
			if (m_pDestination == NULL)
				return COrderDesc::Null;
			else if (m_pDestination != m_OrderDesc.GetTarget())
				{
				//	For backwards compatibility.
				const_cast<CPlayerShipController *>(this)->m_OrderDesc.SetTarget(m_pDestination);
				}

			return m_OrderDesc;

		default:
			return COrderDesc::Null;
		}
	}

void CPlayerShipController::Init (CTranscendenceWnd *pTrans)

//	Init
//
//	Initialize object before we play.

	{
	m_pTrans = pTrans;

	m_bDockPortIndicators = (strEquals(pTrans->GetSettings().GetString(CGameSettings::dockPortIndicator), SETTING_ENABLED)
			|| strEquals(pTrans->GetSettings().GetString(CGameSettings::dockPortIndicator), SETTING_TRUE));

	//	If we saved while docked, we undock ourselves. This can happen if we 
	//	call (gamSave) from inside a dock screen. But since we don't save our
	//	dock state, we need to undock when we come back.

	if (m_pStation)
		Undock();
	}

void CPlayerShipController::InitTargetList (ETargetClass iTargetType, bool bUpdate)

//	InitTargetList
//
//	Initializes the target list

	{
	int i;

	//	Make a list of all targets

	if (!bUpdate)
		m_TargetList.DeleteAll();

	//	Make sure we're valid

	CSystem *pSystem;
	if (m_pShip == NULL
			|| (pSystem = m_pShip->GetSystem()) == NULL)
		{
		m_TargetList.DeleteAll();
		return;
		}

	//	Compute some invariants based on our perception

	int iPerception = m_pShip->GetPerception();

	char szBuffer[1024];
	for (i = 0; i < pSystem->GetObjectCount(); i++)
		{
		CSpaceObject *pObj = pSystem->GetObject(i);

		if (pObj 
				&& pObj->CanBeHit()
				&& !pObj->IsDestroyed()
				&& (pObj->GetCategory() == CSpaceObject::catShip || pObj->GetCategory() == CSpaceObject::catStation)
				&& !pObj->IsAttached()
				&& pObj != m_pShip)
			{
			bool bInList = false;

			//	Figure out if we this object matches what we're looking for.
			//	If this is an enemy that can attack, then only show it if
			//	we're looking for enemy targets; otherwise, show it when
			//	we're looking for friendly targets

			int iMainKey = -1;
			if ((iTargetType == ETargetClass::enemies) == (m_pShip->IsAngryAt(pObj) && pObj->CanBeAttacked()))
				{
				if (iTargetType == ETargetClass::enemies)
					{
					if (pObj->GetScale() == scaleShip || pObj->GetScale() == scaleStructure)
						iMainKey = 0;
					}
				else
					{
					if (pObj->CanBeAttacked() || pObj->SupportsDockingFast())
						{
						if (pObj->GetScale() == scaleShip || pObj->GetScale() == scaleStructure)
							iMainKey = 0;
						}
					}
				}

			//	If not what we're looking for, skip

			if (iMainKey != -1)
				{
				//	Figure out the distance to the object

				CVector vDist = pObj->GetPos() - m_pShip->GetPos();
				Metric rDist2 = vDist.Length2();

				//	If we can detect the object then it belongs on the list

				if (rDist2 < pObj->GetDetectionRange2(iPerception) || pObj->IsPlayerDestination())
					{
					//	Compute the distance in light-seconds

					int iDist = mathRound(vDist.Length() / LIGHT_SECOND);

					//	Compute the sort order based on the main key and the distance

					wsprintf(szBuffer, "%d%08d%8x", iMainKey, iDist, (DWORD)pObj);
					bInList = true;
					}
				}

			//	Add or update the list

			if (bUpdate)
				{
				int iIndex;
				bool bFound = m_TargetList.FindByValue(pObj, &iIndex);

				if (bInList)
					{
					if (!bFound)
						m_TargetList.Insert(CString(szBuffer), pObj);
					}
				else
					{
					if (bFound)
						m_TargetList.Delete(iIndex);
					}
				}
			else if (bInList)
				m_TargetList.Insert(CString(szBuffer), pObj);
			}
		}
	}

void CPlayerShipController::InsuranceClaim (void)

//	InsuranceClaim
//
//	Repair the ship and update insurance claim

	{
	DEBUG_TRY

	ASSERT(m_pShip);

	CSystem *pSystem = m_Universe.GetCurrentSystem();
	ASSERT(pSystem);
	if (pSystem == NULL)
		return;

	//	If necessary, remove the ship from the system first. This can happen if
	//	we are imprisoned by a station (instead of destroyed)

	if (m_pShip->GetIndex() != -1)
		m_pShip->Remove(removedFromSystem, CDamageSource());

	//	Place the ship back in the system

	m_pShip->AddToSystem(*pSystem);
	m_pShip->OnNewSystem(pSystem);

	//	Empty out the wreck

	if (m_dwWreckObjID != OBJID_NULL)
		{
		CSpaceObject *pWreck = pSystem->FindObject(m_dwWreckObjID);
		if (pWreck)
			{
			pWreck->GetItemList().DeleteAll();
			pWreck->InvalidateItemListAddRemove();
			}
		}

	DEBUG_CATCH
	}

bool CPlayerShipController::IsAngryAt (const CSpaceObject *pObj) const

//	IsAngryAt
//
//	Returns TRUE if we are angry at the given object.

	{
	//	We're always angry at enemies

	if (m_pShip->IsEnemy(pObj))
		return true;

	//	If the object is angry at us, we reciprocate.
	//	NOTE: If pObj == m_pShip then we could recurse infinitely, so we check
	//	it explicitly.

	return (pObj != m_pShip && pObj->IsAngryAt(m_pShip));
	}

void CPlayerShipController::OnAbilityChanged (Abilities iAbility, AbilityModifications iChange, bool bNoMessage)

//	OnAbilityChanged
//
//	Some ability has changed.

	{
	switch (iAbility)
		{
		case ablLongRangeScanner:
			if (iChange == ablRepair && !bNoMessage)
				{
				//	If we're in a nebula, then we're still blind.

				if (m_pShip->IsLRSBlind())
					DisplayTranslate(CONSTLIT("msgLRSStillInoperative"));
				else
					DisplayTranslate(CONSTLIT("msgLRSRepaired"));
				}
			break;

		case ablShortRangeScanner:
			if (iChange == ablRepair && !bNoMessage)
				DisplayTranslate(CONSTLIT("msgSRSRepaired"));
			break;
		}
	}

DWORD CPlayerShipController::OnCommunicate (CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2, ICCItem *pData)

//	OnCommunicate
//
//	Message from another object

	{
	switch (iMessage)
		{
		case msgDockingSequenceEngaged:
			pSender->Highlight();
			DisplayTranslate(CONSTLIT("msgDockingSequenceEngaged"));
			return resAck;

		case msgOnAsteroidExplored:
			m_Stats.IncSystemStat(CONSTLIT("asteroidsMined"), NULL_STR, 1);
			return resAck;

		case msgOnPlayerHint:
			{
			if (pData == NULL)
				return resNoAnswer;

			CString sHintID = pData->GetStringAt(FIELD_ID);
			if (strEquals(sHintID, HINT_MINING_NEED_BETTER_WEAPON))
				{
				if (m_UIMsgs.ShowMessage(m_Universe, uimsgMiningDamageTypeHint, pSender))
					DisplayTranslate(CONSTLIT("hintMiningOreLevelTooHigh"), pData);
				}
			else
				return resNoAnswer;

			return resAck;
			}

		default:
			{
			bool bHandled;
			CString sMessage;

			//	First ask the sender to translate the message

			CString sID = GetMessageID(iMessage);

			if (pSender)
				{
				if (!sID.IsBlank())
					{
					bHandled = (pSender->TranslateText(strPatternSubst(CONSTLIT("core.%s"), sID), NULL, &sMessage)
							|| pSender->TranslateText(sID, NULL, &sMessage));
					}
				else
					bHandled = false;

				//	If the sender did not handle it and if it is an older version, then use
				//	the now deprecated <OnTranslateMessage>

				if (!bHandled && pSender->GetAPIVersion() < 3)
					bHandled = pSender->FireOnTranslateMessage(sID, &sMessage);
				}
			else
				bHandled = false;

			//	If we got no translation, then see if we can ask the sovereign
			//	to translate

			if (!bHandled)
				{
				CSovereign *pSovereign = NULL;

				if (pSender)
					pSovereign = pSender->GetSovereign();

				//	Make sure we have a sovereign

				if (pSovereign == NULL)
					pSovereign = m_Universe.FindSovereign(g_PlayerSovereignUNID);

				//	Get the message based on the sovereign

				sMessage = pSovereign->GetText(iMessage);
				}

			//	If we have a message, display it

			if (!sMessage.IsBlank())
				{
				if (pSender)
					pSender->Highlight(sMessage);
				else
					DisplayMessage(sMessage);
				}

			return resNoAnswer;
			}
		}
	}

void CPlayerShipController::OnComponentChanged (ObjectComponentTypes iComponent)

//	OnComponentChanged
//
//	Component has changed

	{
	switch (iComponent)
		{
		case comDeviceCounter:
			m_pTrans->UpdateDeviceCounterDisplay();
			break;
		}
	}

void CPlayerShipController::OnDamaged (const CDamageSource &Cause, CInstalledArmor *pArmor, const DamageDesc &Damage, int iDamage)

//	OnDamaged
//
//	We get called here when the ship takes damage

	{
	if (m_pSession == NULL || pArmor == NULL)
		return;

	int iMaxArmorHP = pArmor->GetMaxHP(m_pShip);

	m_pTrans->Autopilot(false);

	//	Heavy damage (>= 10%) causes screen flash

	if (iDamage >= (iMaxArmorHP / 10) && Damage.CausesSRSFlash())
		m_pSession->OnDamageFlash();

	//	If we're down to 25% armor, then warn the player

	if (pArmor->GetHitPoints() < (iMaxArmorHP / 4) && Damage.CausesSRSFlash())
		{
		DisplayTranslate(CONSTLIT("msgHullBreachImminent"));
		m_Universe.PlaySound(NULL, m_Universe.FindSound(UNID_DEFAULT_HULL_BREACH_ALARM));
		}

	//	Update display

	m_pSession->OnArmorDamaged(pArmor->GetSect());

	//	Register stats

	CItem ArmorItem(pArmor->GetClass()->GetItemType(), 1);
	OnItemDamaged(ArmorItem, iDamage);
	}

bool CPlayerShipController::OnDestroyCheck (DestructionTypes iCause, const CDamageSource &Attacker)

//	OnDestroyCheck
//
//	Check to see if the player ship can be destroyed

	{
	int i;

	//	Loop over powers

	for (i = 0; i < m_Universe.GetPowerCount(); i++)
		{
		CPower *pPower = m_Universe.GetPower(i);
		if (!pPower->OnDestroyCheck(m_pShip, iCause, Attacker))
			return false;
		}

	return true;
	}

void CPlayerShipController::OnDestroyed (SDestroyCtx &Ctx)

//	OnDestroyed
//
//	Ship has been destroyed

	{
	DEBUG_TRY

	//	Clear various variables

	Reset();

	//	Reset everything else

	m_iManeuver = EManeuver::None;
	m_bThrust = false;
	m_bStopThrust = false;
	SetFireMain(false);
	SetFireMissile(false);
	m_bActivate = false;

	//	Notify

	CString sEpitaph;
	g_pTrans->GetModel().OnPlayerDestroyed(Ctx, &sEpitaph);

	if (m_pSession)
		m_pSession->OnPlayerDestroyed(Ctx, sEpitaph);

	DEBUG_CATCH
	}

void CPlayerShipController::OnDeviceEnabledDisabled (int iDev, bool bEnable, bool bSilent)

//	OnDeviceEnabledDisabled
//
//	Device has been disabled (generally by reactor overload)

	{
	DEBUG_TRY

	if (CDeviceItem DeviceItem = m_pShip->GetDeviceItem(iDev))
		{
		if (!bEnable)
			{
			if (m_UIMsgs.ShowMessage(m_Universe, uimsgEnableDeviceHint))
				DisplayCommandHint(CGameKeys::keyEnableDevice, Translate(CONSTLIT("hintEnableDevices")));

			if (!bSilent)
				DisplayTranslate(CONSTLIT("msgDeviceDisabled"), CONSTLIT("itemName"), DeviceItem.GetNounPhrase(nounShort | nounNoModifiers));
			}
		else
			{
			if (!bSilent)
				DisplayTranslate(CONSTLIT("msgDeviceEnabled"), CONSTLIT("itemName"), DeviceItem.GetNounPhrase(nounShort | nounNoModifiers));
			}
		}

	DEBUG_CATCH
	}

void CPlayerShipController::OnDeviceStatus (CInstalledDevice *pDev, CDeviceClass::DeviceNotificationTypes iEvent)

//	OnDeviceStatus
//
//	Device has failed in some way

	{
	ASSERT(pDev);
	CDeviceItem DeviceItem = pDev->GetDeviceItem();
	CString sItemName = DeviceItem.GetNounPhrase(nounShort | nounNoModifiers);

	switch (iEvent)
		{
		case CDeviceClass::statusDisruptionRepaired:
			DisplayTranslate(CONSTLIT("msgDeviceRepaired"), CONSTLIT("itemName"), sItemName);
			break;

		case CDeviceClass::statusUsedLastAmmo:
			OnWeaponStatusChanged();
			break;

		case CDeviceClass::failDamagedByDisruption:
			DisplayTranslate(CONSTLIT("msgDeviceDamaged"), CONSTLIT("itemName"), sItemName);
			break;
		case CDeviceClass::failWeaponJammed:
			DisplayTranslate(CONSTLIT("msgWeaponJammed"));
			break;

		case CDeviceClass::failWeaponMisfire:
			DisplayTranslate(CONSTLIT("msgWeaponMisfire"));
			break;

		case CDeviceClass::failWeaponNoFire:
			DisplayTranslate(CONSTLIT("msgWeaponCannotFire"));
			break;

		case CDeviceClass::failWeaponExplosion:
			DisplayTranslate(CONSTLIT("msgWeaponExplosion"));
			break;

		case CDeviceClass::failShieldFailure:
			DisplayTranslate(CONSTLIT("msgShieldFailure"));
			break;

		case CDeviceClass::failDeviceHitByDamage:
			DisplayTranslate(CONSTLIT("msgDeviceDamaged"), CONSTLIT("itemName"), sItemName);
			break;

		case CDeviceClass::failDeviceHitByDisruption:
			DisplayTranslate(CONSTLIT("msgDeviceIonized"), CONSTLIT("itemName"), sItemName);
			break;

		case CDeviceClass::failDeviceOverheat:
			DisplayTranslate(CONSTLIT("msgDeviceDamagedOverheat"), CONSTLIT("itemName"), sItemName);
			break;

		case CDeviceClass::failDeviceDisabledByOverheat:
			DisplayTranslate(CONSTLIT("msgDeviceDisabledOverheat"), CONSTLIT("itemName"), sItemName);
			break;
		}
	}

void CPlayerShipController::OnEnemyShipsDetected (void)

//	OnEnemyShipsDetected
//
//	Enemy ships

	{
	DisplayTranslate(CONSTLIT("msgEnemyShipsDetected"));
	m_Universe.PlaySound(NULL, m_Universe.FindSound(UNID_DEFAULT_ENEMY_SHIP_ALARM));
	}

void CPlayerShipController::OnFuelConsumed (Metric rFuel, CReactorDesc::EFuelUseTypes iUse)

//  OnFuelConsumed
//
//  Fuel consumed

	{
	//	We track fuel consumption (but not fuel drain, such as from pteravores).
	//	Bioships use fuel consumption to track growth.

	if (iUse == CReactorDesc::fuelConsume)
		m_Stats.OnFuelConsumed(m_pShip, rFuel);
	}

void CPlayerShipController::OnEnterGate (CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate, bool bAscend)

//	OnEnterGate
//
//	Enter stargate

	{
	//	Clear our targeting computer (otherwise, we crash since we archive ships
	//	in the old system)

	Reset();

	//	Reset help

	SetUIMessageFollowed(uimsgGateHint);

	//	Let the model handle everything

	g_pTrans->GetModel().OnPlayerEnteredGate(pDestNode, sDestEntryPoint, pStargate);
	}

void CPlayerShipController::OnOverlayConditionChanged (ECondition iCondition, EConditionChange iChange)

//	OnOverlayConditionChanged
//
//	A condition imposed by an overlay has been added or removed.

	{
	switch (iCondition)
		{
		//	Time stopped

		case ECondition::timeStopped:
			{
			//	Time stopped

			if (iChange == EConditionChange::added)
				DisplayTranslate(CONSTLIT("msgTimeStopped"));

			//	If we're no longer time-stopped

			else if (!m_pShip->IsTimeStopped())
				DisplayTranslate(CONSTLIT("msgTimeContinues"));

			//	Otherwise, time is still stopped

			else
				DisplayTranslate(CONSTLIT("msgTimeStillStopped"));

			break;
			}
		}
	}

void CPlayerShipController::OnPaintSRSEnhancements (CG32bitImage &Dest, SViewportPaintCtx &Ctx)

//	OnPaintSRSEnhancements
//
//	Paint SRS enhancements.

	{
	DEBUG_TRY

	//	Skip if we're not in the proper state

	if (m_pShip == NULL
			|| m_pShip->IsDestroyed())
		return;

	//	Paint friendly-fire debugging

	if (m_Universe.GetDebugOptions().IsShowLineOfFireEnabled())
		{
		if (m_pTarget == NULL)
			PaintDebugLineOfFire(Ctx, Dest);
		else if (m_pTarget->GetCategory() == CSpaceObject::catShip)
			PaintDebugLineOfFire(Ctx, Dest, *m_pTarget);
		}

	//	Paint the docking target, if necessary

	if (m_bDockPortIndicators
			&& m_pAutoDock 
			&& (m_vAutoDockPort - m_pShip->GetPos()).Length2() > MIN_PORT_ANIMATION_DIST2)
		PaintDockingPortIndicators(Ctx, Dest);

	//	If we have a target, then paint a target reticle.
	//	NOTE: We do this even if friendly because weapons will still aim at them.

	if (m_pTarget && !m_bTargetOutOfRange)
		PaintTargetingReticle(Ctx, Dest, m_pTarget);

	//	If we have an auto target and we want to show it, paint a reticle

	else if (m_pAutoTarget 
			&& m_bShowAutoTarget)
		PaintTargetingReticle(Ctx, Dest, m_pAutoTarget);

	else if (m_pAutoMining)
		PaintTargetingReticle(Ctx, Dest, const_cast<CSpaceObject *>(m_pAutoMining));

	//	If necessary, show damage bar

	if (m_pAutoDamage
			&& (DWORD)m_Universe.GetTicks() > m_dwAutoDamageExpire)
		{
		m_pAutoDamage->ClearShowDamageBar();
		m_pAutoDamage = NULL;
		}

#ifdef DEBUG_HIT_SCAN
	CVector vStart = m_pShip->GetPos();
	int x1, y1;
	Ctx.XForm.Transform(vStart, &x1, &y1);

	CVector vEnd = vStart + PolarToVector(m_pShip->GetRotation(), LIGHT_SECOND * 50);
	CVector vHitPos;
	int x2, y2;
	CSpaceObject *pHit;
	if (pHit = m_pShip->GetSystem()->HitScan(m_pShip, vStart, vEnd, false, &vHitPos))
		{
		Ctx.XForm.Transform(vHitPos, &x2, &y2);
		CGDraw::Line(Dest, x1, y1, x2, y2, 1, CG32bitPixel(0xff, 0xff, 0x00));
		CGDraw::Circle(Dest, x2, y2, 3, CG32bitPixel(0xff, 0xff, 0x00));
		}
	else
		{
		Ctx.XForm.Transform(vEnd, &x2, &y2);
		CGDraw::Line(Dest, x1, y1, x2, y2, 1, CG32bitPixel(0x00, 0xff, 0x00));
		}
#endif

	DEBUG_CATCH
	}

void CPlayerShipController::OnShipStatus (EShipStatusNotifications iEvent, DWORD dwData)

//	OnShipStatus
//
//	Something has happened to the ship...

	{
	DEBUG_TRY

	switch (iEvent)
		{
		case statusArmorRepaired:
			{
			int iSeg = (int)dwData;

			if (m_pSession)
				m_pSession->OnArmorRepaired(iSeg);
			break;
			}

		case statusFuelLowWarning:
			{
			int iSeq = (int)dwData;

			//	If -1, we are out of fuel

			if (iSeq == -1)
				{
				DisplayTranslate(CONSTLIT("msgOutOfFuel"));

				//	Stop

				SetThrust(false);
				SetManeuver(EManeuver::None);
				SetFireMain(false);
				SetFireMissile(false);
				}

			//	Don't warn the player every time

			else if ((iSeq % 15) == 0)
				{
				if (m_UIMsgs.ShowMessage(m_Universe, uimsgRefuelHint))
					DisplayCommandHint(CGameKeys::keyShipStatus, Translate(CONSTLIT("hintRefuel")));

				DisplayTranslate(CONSTLIT("msgFuelLow"));
				if ((iSeq % 30) == 0)
					m_Universe.PlaySound(NULL, m_Universe.FindSound(UNID_DEFAULT_FUEL_LOW_ALARM));
				}

			break;
			}

		case statusLifeSupportWarning:
			{
			int iSecondsLeft = (int)dwData;

			if (iSecondsLeft > 10 && ((iSecondsLeft % 5) != 0))
				NULL;
			else if (iSecondsLeft > 1)
				DisplayTranslate(CONSTLIT("msgLifeSupportWarningN"), CONSTLIT("time"), strFromInt(iSecondsLeft));
			else if (iSecondsLeft == 1)
				DisplayTranslate(CONSTLIT("msgLifeSupportWarning1"));
			break;
			}

		case statusRadiationCleared:
			DisplayTranslate(CONSTLIT("msgDeconComplete"));
			break;

		case statusRadiationWarning:
			{
			int iTicksLeft = (int)dwData;

			if ((iTicksLeft % 10) == 0)
				{
				int iSecondsLeft = iTicksLeft / g_TicksPerSecond;

				if (iSecondsLeft > 10 && ((iSecondsLeft % 5) != 0))
					NULL;
				else if (iSecondsLeft > 1)
					DisplayTranslate(CONSTLIT("msgRadiationWarningN"), CONSTLIT("time"), strFromInt(iSecondsLeft));
				else if (iSecondsLeft == 1)
					DisplayTranslate(CONSTLIT("msgRadiationWarning1"));
				else
					DisplayTranslate(CONSTLIT("msgRadiationDeath"));

				if ((iTicksLeft % 150) == 0)
					m_Universe.PlaySound(NULL, m_Universe.FindSound(UNID_DEFAULT_RADIATION_ALARM));
				}

			break;
			}

		case statusReactorOverloadWarning:
			{
			int iSeq = (int)dwData;

			//	Warn every 60 ticks

			if ((iSeq % 6) == 0)
				{
				DisplayTranslate(CONSTLIT("msgReactorOverloadWarning"));
				if ((iSeq % 24) == 0)
					m_Universe.PlaySound(NULL, m_Universe.FindSound(UNID_DEFAULT_REACTOR_OVERLOAD_ALARM));
				}
			break;
			}

		case statusReactorPowerFailure:
			DisplayTranslate(CONSTLIT("msgReactorNoPower"));

			SetThrust(false);
			SetManeuver(EManeuver::None);
			SetFireMain(false);
			SetFireMissile(false);
			break;

		case statusReactorRestored:
			DisplayTranslate(CONSTLIT("msgReactorPowerRestored"));
			break;
		}

	DEBUG_CATCH
	}

void CPlayerShipController::OnStartGame (void)

//	OnStartGame
//
//	Game (either new or loaded) has just started

	{
	m_iManeuver = EManeuver::None;
	m_bThrust = false;
	m_bStopThrust = false;
	SetFireMain(false);
	SetFireMissile(false);
	m_bActivate = false;

	//	Clear the POVLRS flag for all objects (so that we don't get the
	//	"Enemy Ships Detected" message when entering a system

	m_pShip->GetSystem()->SetPOVLRS(m_pShip);
	}

void CPlayerShipController::OnStationDestroyed (const SDestroyCtx &Ctx)

//	OnStationDestroyed
//
//	A station has been destroyed

	{
	bool bIsMajorStation = Ctx.Obj.HasAttribute(CONSTLIT("majorStation"));

	//	If we are the cause of the destruction and then record stat

	if (Ctx.Attacker.IsCausedByPlayer())
		{
		m_Stats.OnObjDestroyedByPlayer(Ctx, m_pShip);

		//	If this is a major station then add it to the list of key events.
		//	NOTE: Here we care about enemy instead of angry.

		if (bIsMajorStation)
			m_Stats.OnKeyEvent((Ctx.Obj.IsEnemy(m_pShip) ? CPlayerGameStats::eventEnemyDestroyedByPlayer : CPlayerGameStats::eventFriendDestroyedByPlayer),
					&Ctx.Obj,
					Ctx.Attacker.GetSovereignUNID());
		}

	//	Otherwise, record the event if this is a major station

	else if (bIsMajorStation)
		m_Stats.OnKeyEvent(CPlayerGameStats::eventMajorDestroyed, &Ctx.Obj, Ctx.Attacker.GetSovereignUNID());

	//	If the station was targeted, then clear the target

	if (m_pTarget == Ctx.Obj)
		{
		SetTarget(NULL);
		if (m_pSession)
			m_pSession->OnTargetChanged(m_pTarget);
		}

	if (m_pAutoDamage == Ctx.Obj)
		{
		m_pAutoDamage->ClearShowDamageBar();
		m_pAutoDamage = NULL;
		}
	}

void CPlayerShipController::OnWreckCreated (CSpaceObject *pWreck)

//	OnWreckCreated
//
//	Called when the ship has been destroyed and a wreck has been
//	created.

	{
	ASSERT(pWreck && !pWreck->IsDestroyed());
	if (pWreck == NULL || pWreck->IsDestroyed())
		return;

	//	Change our POV to the wreck

	m_Universe.SetPOV(pWreck);

	//	We remember the wreck, but only by ID because we might not
	//	get any notifications of its destruction after this
	//	(since the player ship has been taken out of the system)

	m_dwWreckObjID = pWreck->GetID();
	}

void CPlayerShipController::PaintDebugLineOfFire (SViewportPaintCtx &Ctx, CG32bitImage &Dest) const

//	PaintDebugLineOfFire
//
//	Paints line of fire for all ships.

	{
	CSystem *pSystem = m_Universe.GetCurrentSystem();
	if (pSystem == NULL)
		return;

	static const Metric DIST = 50.0 * LIGHT_SECOND;
	static const Metric DIST2 = DIST * DIST;

	for (int i = 0; i < pSystem->GetObjectCount(); i++)
		{
		CSpaceObject *pObj = pSystem->GetObject(i);

		if (pObj 
				&& pObj->GetCategory() == CSpaceObject::catShip
				&& !pObj->IsIntangible()
				&& pObj != m_pShip)
			{
			CVector vDist = pObj->GetPos() - m_pShip->GetPos();
			Metric rDist2 = vDist.Length2();

			if (rDist2 < DIST2)
				{
				PaintDebugLineOfFire(Ctx, Dest, *pObj);
				}
			}
		}
	}

void CPlayerShipController::PaintDebugLineOfFire (SViewportPaintCtx &Ctx, CG32bitImage &Dest, CSpaceObject &TargetObj) const

//	PaintDebugLineOfFire
//
//	Paints the line for fire for all weapons on the ship.

	{
	CShip *pShip = TargetObj.AsShip();
	if (pShip == NULL)
		return;

	CInstalledDevice *pDevice = pShip->GetNamedDevice(devPrimaryWeapon);
	if (pDevice && !pDevice->IsSecondaryWeapon())
		PaintDebugLineOfFire(Ctx, Dest, TargetObj, *pDevice);

	for (CDeviceItem DeviceItem : pShip->GetDeviceSystem())
		{
		CInstalledDevice &Device = *DeviceItem.GetInstalledDevice();
		if (Device.IsSecondaryWeapon())
			PaintDebugLineOfFire(Ctx, Dest, TargetObj, Device);
		}
	}

void CPlayerShipController::PaintDebugLineOfFire (SViewportPaintCtx &Ctx, CG32bitImage &Dest, CSpaceObject &TargetObj, CInstalledDevice &Weapon) const

//	PaintDebugLineOfFire
//
//	Paints debug info the the given object and weapon.

	{
	static const Metric DEFAULT_DIST_CHECK = 700.0 * g_KlicksPerPixel;
	static const CG32bitPixel RGB_BLOCKED(255, 0, 0);
	static const CG32bitPixel RGB_CLEAR(0, 255, 255);

	CShip *pShip = TargetObj.AsShip();
	if (pShip == NULL)
		return;

	const CDeviceItem DeviceItem = Weapon.GetItem()->AsDeviceItem();

	//	Compute the fire direction of the weapon.

	int iDir = Weapon.GetFireAngle();
	CWeaponClass *pWeapon = Weapon.GetClass()->AsWeaponClass();
	if (pWeapon)
		{
		int iWeaponMinFireArc, iWeaponMaxFireArc;
		switch (pWeapon->GetRotationType(DeviceItem, &iWeaponMinFireArc, &iWeaponMaxFireArc))
			{
			case CDeviceRotationDesc::rotSwivel:
				iDir = AngleMiddle(iWeaponMinFireArc, iWeaponMaxFireArc);
				break;
			}
		}

	//	First, we compute the line of fire algorithm and highlight the first 
	//	object that blocks us.

	iDir = AngleMod(pShip->GetRotation() + iDir);
	CSpaceObject *pBlock = NULL;
	if (!pShip->IsLineOfFireClear(&Weapon, NULL, iDir, Max(Weapon.GetMaxEffectiveRange(pShip), DEFAULT_DIST_CHECK), &pBlock))
		{
		int x, y;
		Ctx.XForm.Transform(pBlock->GetPos(), &x, &y);

		CGDraw::Arc(Dest, x, y, 60, 0, 0, 1, RGB_BLOCKED);
		}

	//	Paint the line of fire that we're testing.

	CVector vStart = Weapon.GetPos(pShip);
	int xStart, yStart;
	Ctx.XForm.Transform(vStart, &xStart, &yStart);

	CVector vEnd = vStart + PolarToVector(iDir, DEFAULT_DIST_CHECK);
	int xEnd, yEnd;
	Ctx.XForm.Transform(vEnd, &xEnd, &yEnd);

	CGDraw::LineDotted(Dest, xStart, yStart, xEnd, yEnd, CG32bitPixel(255, 255, 0, 128));

	//	Paint the path that a shot would take if fired right now.
	//
	//	We show this relative to the player, which makes it a little easier to
	//	visualize. But in the actual algorithm (inside IsLineOfFireClear) we 
	//	compute the path in absolute terms and adjust the position of objects
	//	(including the player) based on their velocities.

	CItemCtx ItemCtx(pShip, &Weapon);
	Metric rSpeed = Weapon.GetShotSpeed(ItemCtx);
	CVector vVel = pShip->GetVel() + PolarToVector(iDir, rSpeed) - m_pShip->GetVel();
	CVector vPos = vStart;

	CG32bitPixel rgbPath = (pBlock ? RGB_BLOCKED : RGB_CLEAR);

	int xPrev, yPrev;
	Ctx.XForm.Transform(vStart, &xPrev, &yPrev);
	for (int i = 0; i < 10; i++)
		{
		vPos = vPos + (g_SecondsPerUpdate * vVel);

		int xPath, yPath;
		Ctx.XForm.Transform(vPos, &xPath, &yPath);

		CGDraw::LineHD(Dest, xPrev, yPrev, xPath, yPath, 1, rgbPath);

		Dest.DrawDot(xPath, yPath, rgbPath, markerTinyCircle);

		xPrev = xPath;
		yPrev = yPath;
		}
	}

void CPlayerShipController::PaintDockingPortIndicators (SViewportPaintCtx &Ctx, CG32bitImage &Dest) const

//	PaintDockingPortIndicators
//
//	Paints docking port indicators

	{
	int x, y;
	Ctx.XForm.Transform(m_vAutoDockPort, &x, &y);

	int iSpeed = 3;
	int iRange = 10;
	int iMin = 3;

	int iPos = (iRange - 1) - ((m_Universe.GetPaintTick() / iSpeed) % iRange);
	int iSize = iMin + iPos;
	DWORD dwOpacity = 255 - (iPos * 20);

	CG32bitPixel rgbColor = CG32bitPixel(m_pAutoDock->GetSymbolColor(), (BYTE)dwOpacity);

	//	Draw animating brackets

	Dest.FillColumn(x - iSize, y - iSize, iPos + 1, rgbColor);
	Dest.FillColumn(x + iSize - 1, y - iSize, iPos + 1, rgbColor);
	Dest.FillColumn(x - iSize, y + iMin - 1, iPos + 1, rgbColor);
	Dest.FillColumn(x + iSize - 1, y + iMin - 1, iPos + 1, rgbColor);

	Dest.FillLine(x - iSize + 1, y - iSize, iPos, rgbColor);
	Dest.FillLine(x - iSize + 1, y + iSize - 1, iPos, rgbColor);
	Dest.FillLine(x + 2, y - iSize, iPos, rgbColor);
	Dest.FillLine(x + 2, y + iSize - 1, iPos, rgbColor);
	}

void CPlayerShipController::PaintTargetingReticle (SViewportPaintCtx &Ctx, CG32bitImage &Dest, CSpaceObject *pTarget)

//	PaintTargetingReticle
//
//	Paints a targeting reticle on the given object

	{
	int x, y;
	Ctx.XForm.Transform(pTarget->GetPos(), &x, &y);

	CG32bitPixel rgbColor = pTarget->GetSymbolColor();
	int iSize = 8;
	int iIndent = iSize / 4;

	const RECT &rcImage = pTarget->GetImage().GetImageRect();
	int cxWidth = RectWidth(rcImage);
	int cyHeight = RectHeight(rcImage);

	int cxHorz = Max(0, (cxWidth / 2) - iIndent);
	int cyVert = Max(0, (cyHeight / 2) - iIndent);

	//	Draw

	Dest.FillColumn(x, y - cyVert - iSize, iSize, rgbColor);
	Dest.FillColumn(x, y + cyVert, iSize, rgbColor);

	Dest.FillLine(x + cxHorz, y, iSize, rgbColor);
	Dest.FillLine(x - cxHorz - iSize, y, iSize, rgbColor);
	}

void CPlayerShipController::RedirectDisplayMessage (bool bRedirect)

//	RedirectDisplayMessage
//
//	Redirects a display message.

	{
	if (bRedirect)
		{
		m_bRedirectMessages = true;
		m_sRedirectMessage = NULL_STR;
		}
	else
		{
		m_bRedirectMessages = false;
		}
	}

bool CPlayerShipController::ToggleEnableDevice (int iDeviceIndex)

//	ToggleEnableDevice
//
//	Toggles the enable/disable state of a given device. Returns TRUE if the device was toggled.
//	
//	iDeviceIndex is the index of the device as it show up in the
//	enable/disable menu.

	{
	int i;

	//	First we need to look for the device by index. We use the same
	//	order as listed in the Enable/Disable menu.

	CItemList &List = m_pShip->GetItemList();
	List.SortItems();

	for (i = 0; i < List.GetCount(); i++)
		{
		CItem &Item = List.GetItem(i);
		CInstalledDevice *pDevice = m_pShip->FindDevice(Item);
		CItemCtx ItemCtx(m_pShip, pDevice);

		if (pDevice && pDevice->CanBeDisabled(ItemCtx))
			{
			if (iDeviceIndex == 0)
				{
				m_pShip->EnableDevice(Item.GetInstalled(), !pDevice->IsEnabled());
				return true;
				}
			else
				iDeviceIndex--;
			}
		}

	return false;
	}

EManeuver CPlayerShipController::GetManeuver (void) const
	{
	if (m_ManeuverController.IsManeuverActive())
		return m_ManeuverController.GetManeuver(m_pShip);
	else
		return m_iManeuver;
	}

bool CPlayerShipController::GetThrust (void)
	{
	if (m_ManeuverController.IsThrustActive())
		return m_ManeuverController.GetThrust(m_pShip);
	else
		return m_bThrust;
	}

bool CPlayerShipController::GetReverseThrust (void)
	{
	return false;
	}

bool CPlayerShipController::GetStopThrust (void)
	{
	if (m_ManeuverController.IsThrustActive())
		return false;
	else
		return m_bStopThrust;
	}

bool CPlayerShipController::GetDeviceActivate (void)
	{
	return m_bActivate;
	}

CSpaceObject *CPlayerShipController::GetTarget (const CDeviceItem *pDeviceItem, DWORD dwFlags) const

//	GetTarget
//
//	Returns the target for the player ship

	{
	if (dwFlags & FLAG_ACTUAL_TARGET)
		return m_pTarget;

	//	Use the player's designated target if it's in range and it's not a friend,
	//	then we use it as a target.

	else if (m_pTarget 
			&& !m_bTargetOutOfRange
			&& (m_pShip->CanTargetFriendlies() || !m_pTarget->IsFriend(m_pShip) || m_pTarget->IsAngryAt(m_pShip)))
		return m_pTarget;

	//	Otherwise, we use the auto-target.

	else if (!(dwFlags & FLAG_NO_AUTO_TARGET))
		{
		//	Return the autotarget

		int iTick = m_Universe.GetTicks();
		if (iTick == m_iAutoTargetTick)
			return ((m_pAutoTarget && !m_pAutoTarget->IsDestroyed()) ? m_pAutoTarget : NULL);

		m_iAutoTargetTick = iTick;

		//	If we have an auto target, return it.

		if (m_pAutoTarget && !m_pAutoTarget->IsDestroyed())
			return m_pAutoTarget;

		//	If we have a mining target, then use that instead.

		else if (m_pAutoMining 
					&& !m_pAutoMining->IsDestroyed()
					&& pDeviceItem
					&& pDeviceItem->IsMiningWeapon())
			return const_cast<CSpaceObject *>(m_pAutoMining);

		//	Otherwise, no target

		else
			return NULL;
		}

	//	Otherwise, no target

	else
		return NULL;
	}

CTargetList CPlayerShipController::GetTargetList (void) const

//	GetTargetList
//
//	Returns a target list suitable for weapons looking for targets.

	{
	CTargetList::STargetOptions Options;

	Options.rMaxDist = MAX_AUTO_TARGET_DISTANCE;
	Options.bIncludeNonAggressors = true;
	Options.bIncludeStations = true;
	Options.bIncludeSourceTarget = true;
	Options.bNoLineOfFireCheck = true;

	//	Include targeting requirements from weapons

	DWORD dwTargetTypes = m_pShip->GetDeviceSystem().GetTargetTypes();

	if (dwTargetTypes & CTargetList::SELECT_MISSILE)
		Options.bIncludeMissiles = true;

	if (dwTargetTypes & CTargetList::SELECT_MINABLE)
		Options.bIncludeMinable = true;

	//	Done

	return CTargetList(*m_pShip, Options);
	}

void CPlayerShipController::OnDocked (CSpaceObject *pObj)
	{
	ASSERT(pObj == m_pStation);

	//	Set a flag because we don't want to transition to dock screen from
	//	inside this event.

	m_bSignalDock = true;
	}

void CPlayerShipController::OnAcceptedMission (CMission &MissionObj)

//	OnAcceptedMission
//
//	We accepted a mission.

	{
	//	Tell the session so that it shows a mission banner

	if (m_pSession)
		m_pSession->OnAcceptedMission(MissionObj);
	}

void CPlayerShipController::OnMissionCompleted (CMission *pMission, bool bSuccess)

//	OnMissionCompleted
//
//	A player mission has finished.

	{
	if (pMission->KeepsStats())
		{
		if (bSuccess)
			m_Stats.OnKeyEvent(CPlayerGameStats::eventMissionSuccess, pMission, 0);
		else
			m_Stats.OnKeyEvent(CPlayerGameStats::eventMissionFailure, pMission, 0);
		}
	}

void CPlayerShipController::OnNewSystem (CSystem *pSystem)

//	OnNewSystem
//
//	Player has arrived at a new system

	{
	}

void CPlayerShipController::OnObjDestroyed (const SDestroyCtx &Ctx)

//	OnObjDestroyed
//
//	An object was destroyed

	{
	//	If we are the cause of the destruction and the ship is
	//	not a friend, then increase our score
	//
	//	NOTE: We don't care about stations here because those will be
	//	handled in OnStationDestroyed

	if (Ctx.Obj.GetCategory() == CSpaceObject::catShip)
		{
		bool bIsMajorShip = Ctx.Obj.HasAttribute(CONSTLIT("majorShip"));

		if (Ctx.Attacker.IsCausedByPlayer())
			{
			m_Stats.OnObjDestroyedByPlayer(Ctx, m_pShip);

			if (bIsMajorShip)
				{
			//	NOTE: Here we care about enemy instead of angry.

				m_Stats.OnKeyEvent((Ctx.Obj.IsEnemy(m_pShip) ? CPlayerGameStats::eventEnemyDestroyedByPlayer : CPlayerGameStats::eventFriendDestroyedByPlayer),
						&Ctx.Obj,
						Ctx.Attacker.GetSovereignUNID());
				}
			}
		else if (bIsMajorShip)
			m_Stats.OnKeyEvent(CPlayerGameStats::eventMajorDestroyed, &Ctx.Obj, Ctx.Attacker.GetSovereignUNID());
		}

	//	If the object we're docked with got destroyed, then undock

	if (m_pStation == Ctx.Obj || Ctx.Obj.IsPlayerDocked())
		{
		if (Ctx.Obj.IsPlayerDocked())
			g_pTrans->GetModel().ExitScreenSession(true);

		m_pStation = NULL;
		}

	//	Clear out some variables

	if (m_pTarget == Ctx.Obj)
		{
		m_pTarget = NULL;
		ClearFireAngle();
		if (m_pSession)
			m_pSession->OnTargetChanged(m_pTarget);
		}

	if (m_pDestination == Ctx.Obj)
		m_pDestination = NULL;

	if (m_pAutoDock == Ctx.Obj)
		m_pAutoDock = NULL;

	if (m_pAutoTarget == Ctx.Obj)
		m_pAutoTarget = NULL;

	if (m_pAutoMining == Ctx.Obj)
		m_pAutoMining = NULL;

	if (m_pAutoDamage == Ctx.Obj)
		{
		m_pAutoDamage->ClearShowDamageBar();
		m_pAutoDamage = NULL;
		}

	//	Clear out the target list

	int iIndex;
	if (m_TargetList.FindByValue(&Ctx.Obj, &iIndex))
		m_TargetList.Delete(iIndex);

	//	Let the UI deal with destroyed objects

	if (m_pSession)
		m_pSession->OnObjDestroyed(Ctx);

	if (m_OrderDesc.GetTarget() == Ctx.Obj)
		m_OrderDesc = COrderDesc();
	}

void CPlayerShipController::OnObjHit (const SDamageCtx &Ctx)

//	OnObjHit
//
//	We hit an object.

	{
	EDamageHint iHint;

	//	Skip if we're dead

	if (m_pShip == NULL || m_pShip->IsDestroyed())
		return;

	//	Skip if we don't care about these objects.

	else if (Ctx.pObj == NULL 
			|| Ctx.pObj->IsDestroyed() 
			|| !m_pShip->IsAngryAt(Ctx.pObj)
			|| !Ctx.pObj->CanBeAttacked()
			|| Ctx.Attacker.IsAutomatedWeapon())
		return;

	//	If we have a hint, then show it to the player.

	if ((iHint = Ctx.GetHint()) != EDamageHint::none && Ctx.Attacker.IsPlayer())
		{
		if (m_UIMsgs.ShowMessage(m_Universe, uimsgStationDamageHint, Ctx.pObj))
			{
			switch (iHint)
				{
				case EDamageHint::useMining:
					m_pShip->SendMessage(Ctx.pObj, Translate(CONSTLIT("hintMiningNeeded")));
					break;

				case EDamageHint::useMiningOrWMD:
					m_pShip->SendMessage(Ctx.pObj, Translate(CONSTLIT("hintMiningOrWMDNeeded")));
					break;

				case EDamageHint::useWMD:
					m_pShip->SendMessage(Ctx.pObj, Translate(CONSTLIT("hintWMDNeeded")));
					break;

				case EDamageHint::useWMDforShip:
					m_pShip->SendMessage(Ctx.pObj, Translate(CONSTLIT("hintWMDForShipNeeded")));
					break;
				}
			}
		}

	//	Remember that we caused damage to this object (but only if it is an 
	//	enemy station or a capital ship of some sort).

	if (Ctx.pObj->GetCategory() == CSpaceObject::catStation
				|| Ctx.pObj->IsMultiHull())
		{
		if (m_pAutoDamage)
			m_pAutoDamage->ClearShowDamageBar();

		m_pAutoDamage = Ctx.pObj;
		m_pAutoDamage->SetShowDamageBar();
		m_dwAutoDamageExpire = m_Universe.GetTicks() + DAMAGE_BAR_TIMER;
		}
	}

void CPlayerShipController::OnProgramDamage (CSpaceObject *pHacker, const ProgramDesc &Program)

//	OnProgramDamage
//
//	Hit by a program

	{
	if (pHacker)
		pHacker->Highlight();

	DisplayTranslate(CONSTLIT("msgCyberattackDetected"), CONSTLIT("program"), Program.sProgramName);
	}

void CPlayerShipController::OnUpdatePlayer (SUpdateCtx &Ctx)

//	OnUpdatePlayer
//
//	This is called every tick after all other objects have been updated.

	{
	DEBUG_TRY

	//	Remember the AutoTarget.

	m_pAutoTarget = const_cast<CSpaceObject *>(Ctx.GetAutoTarget().GetAutoTarget());
	m_bShowAutoTarget = Ctx.GetAutoTarget().IsAutoTargetNeeded();
	m_bTargetOutOfRange = Ctx.GetAutoTarget().IsPlayerTargetOutOfRange();

	//	Remember auto mining, if necessary

	m_pAutoMining = Ctx.GetAutoMining().GetAutoTarget();

	//	Compute the AutoDock target.
	//
	//	If we're already in the middle of docking then we don't change anything.

	if (m_pStation)
		NULL;

	//	We calculate the best port to dock with at this moment. We start by 
	//	seeing if the player has targeted a friendly station.
	//
	//	NOTE: In this case, we use IsEnemy instead of IsAngryAt because we want
	//	the station to tell the player that they refused ("Docking Request Denied")

	else if (m_pTarget 
			&& m_pTarget != m_pShip 
			&& m_pTarget->SupportsDockingFast()
			&& (!m_pShip->IsEnemy(m_pTarget) || m_pTarget->IsAbandoned())
			&& (m_pTarget->GetPos() - m_pShip->GetPos()).Length2() < MAX_DOCK_DISTANCE2)
		{
		m_pAutoDock = m_pTarget;
		m_iAutoDockPort = m_pTarget->GetNearestDockPort(m_pShip, &m_vAutoDockPort);
		if (m_iAutoDockPort == -1)
			m_pAutoDock = NULL;
		}

	//	Otherwise, if we are close to a port then we use that.

	else if (m_pAutoDock = const_cast<CSpaceObject *>(Ctx.GetAutoDock().GetDockObj()))
		{
		m_iAutoDockPort = Ctx.GetAutoDock().GetDockingPortIndex();
		m_vAutoDockPort = Ctx.GetAutoDock().GetDockingPortPos();
		}

	//	Notify the game controller when we transition in/out of combat.
	//
	//	If we're currently in combat, figure out if we're still in combat. If 
	//	not, then we transition out.

	if (m_bUnderAttack)
		{
		bool bStillUnderAttack = Ctx.pSystem->EnemiesInLRS()
				&& (Ctx.pSystem->IsPlayerUnderAttack()
					|| m_pShip->HasBeenHitLately(HIT_THRESHOLD_EXIT)
					|| m_pShip->HasFiredLately(FIRE_THRESHOLD_EXIT));
		if (!bStillUnderAttack)
			{
			g_pHI->HICommand(CMD_PLAYER_COMBAT_ENDED);
			m_bUnderAttack = false;
			}
		}

	//	Otherwise, if we're not in combat, figure out if we're in combat.
	//	NOTE: We have different criteria for entering combat vs. staying in 
	//	comabt.

	else
		{
		bool bUnderAttack = Ctx.pSystem->EnemiesInLRS()
				&& m_pShip->HasFiredLately(FIRE_THRESHOLD)
				&& (m_pShip->HasBeenHitLately(HIT_THRESHOLD) || Ctx.pSystem->IsPlayerUnderAttack());
		if (bUnderAttack)
			{
			g_pHI->HICommand(CMD_PLAYER_COMBAT_STARTED);
			m_bUnderAttack = true;
			}
		}

	//	Gravity warning

	if (Ctx.bGravityWarning)
		{
		int iTicks = m_Universe.GetTicks();

		if ((iTicks % 150) == 0)
			{
			DisplayTranslate(CONSTLIT("msgGravityWarning"));
			m_Universe.PlaySound(NULL, m_Universe.FindSound(UNID_DEFAULT_GRAVITY_ALARM));
			}
		}

	//	If we've succeeded in docking, then tell the model

	if (m_bSignalDock)
		{
		if (m_pStation && !m_pStation->IsDestroyed())
			g_pTrans->GetModel().OnPlayerDocked(m_pStation);

		m_bSignalDock = false;
		}

	//	Remember some other state

	m_bHasSquadron = Ctx.PlayerHasSquadron();
	m_bHasCommsTarget = Ctx.PlayerHasCommsTarget();

	DEBUG_CATCH
	}

void CPlayerShipController::OnWeaponStatusChanged (void)

//	OnWeaponStatusChanged
//
//	Weapon status has changed

	{
	if (m_pSession)
		m_pSession->OnWeaponStatusChanged();
	}

void CPlayerShipController::ReadFromStream (SLoadCtx &Ctx, CShip *pShip)

//	ReadFromStream
//
//	Reads data from stream
//
//	DWORD		m_iGenome
//	DWORD		m_dwStartingShipClass
//	CString		m_sStartingSystem
//	DWORD		m_dwCharacterClass
//	DWORD		m_pShip (CSpaceObject ref)
//	DWORD		m_pStation (CSpaceObject ref)
//	DWORD		m_pTarget (CSpaceObject ref)
//	DWORD		m_pDestination (CSpaceObject ref)
//	COrderDesc	m_OrderDesc
//	DWORD		m_iManeuver
//	CCurrencyBlock m_Credits
//	CPlayerGameStats m_Stats
//	DWORD		UNUSED (m_iInsuranceClaims)
//	DWORD		flags
//	CUIMessageController
//	CManeuverController

	{
	DWORD dwLoad;

	Ctx.pStream->Read(dwLoad);
	m_iGenome = (GenomeTypes)dwLoad;
	Ctx.pStream->Read(m_dwStartingShipClass);
	
	if (Ctx.dwVersion >= 191)
		m_sStartingSystem.ReadFromStream(Ctx.pStream);
	else
		{
		const CPlayerSettings *pPlayer = pShip->GetClass()->GetPlayerSettings();
		if (pPlayer)
			m_sStartingSystem = pPlayer->GetStartingNode();

		if (m_sStartingSystem.IsBlank())
			m_sStartingSystem = m_Universe.GetCurrentAdventureDesc().GetStartingNodeID();
		}

	if (Ctx.dwVersion >= 141)
		{
		Ctx.pStream->Read(dwLoad);
		m_pCharacterClass = m_Universe.FindGenericType(dwLoad);
		}
	else
		{
		m_pCharacterClass = pShip->GetClass()->GetCharacterClass();
		if (m_pCharacterClass == NULL)
			m_pCharacterClass = m_Universe.FindGenericType(UNID_PILGRIM_CHARACTER_CLASS);
		}

	CSystem::ReadObjRefFromStream(Ctx, (CSpaceObject **)&m_pShip);
	CSystem::ReadObjRefFromStream(Ctx, &m_pStation);
	CSystem::ReadObjRefFromStream(Ctx, &m_pTarget);
	CSystem::ReadObjRefFromStream(Ctx, &m_pDestination, true);

	if (Ctx.dwVersion >= 195)
		m_OrderDesc.ReadFromStream(Ctx);
	else if (Ctx.dwVersion >= 100)
		{
		Ctx.pStream->Read(dwLoad);
		m_OrderDesc = COrderDesc((OrderTypes)dwLoad);
		}
	else
		m_OrderDesc = COrderDesc();

	Ctx.pStream->Read(dwLoad);
	m_iManeuver = (EManeuver)dwLoad;

	if (Ctx.dwVersion >= 49)
		m_Credits.ReadFromStream(Ctx);
	else
		{
		Ctx.pStream->Read(dwLoad);
		m_Credits.SetCredits(CONSTLIT("credit"), dwLoad);
		}

	m_Stats.ReadFromStream(Ctx);

	Ctx.pStream->Read(dwLoad);	//	UNUSED

	//	Flags

	Ctx.pStream->Read(dwLoad);
	m_bThrust = false;
	m_bActivate = false;
	m_bStopThrust = false;
	m_bUnderAttack =			((dwLoad & 0x00000001) ? true : false);
	m_bMapHUD =					((dwLoad & 0x00001000) ? true : false);
	m_iLastHelpTick = 0;

	//	Message controller

	if (Ctx.dwVersion >= 63)
		m_UIMsgs.ReadFromStream(Ctx);
	else
		{
		//	Initialize from flags

		m_UIMsgs.SetEnabled(uimsgDockHint,			((dwLoad & 0x00000020) ? true : false));
		m_UIMsgs.SetEnabled(uimsgMapHint,			((dwLoad & 0x00000040) ? true : false));
		m_UIMsgs.SetEnabled(uimsgAutopilotHint,		((dwLoad & 0x00000080) ? true : false));
		m_UIMsgs.SetEnabled(uimsgGateHint,			((dwLoad & 0x00000100) ? true : false));
		m_UIMsgs.SetEnabled(uimsgUseItemHint,		((dwLoad & 0x00000200) ? true : false));
		m_UIMsgs.SetEnabled(uimsgRefuelHint,		((dwLoad & 0x00000400) ? true : false));
		m_UIMsgs.SetEnabled(uimsgEnableDeviceHint,	((dwLoad & 0x00000800) ? true : false));
		m_UIMsgs.SetEnabled(uimsgSwitchMissileHint,	((dwLoad & 0x00002000) ? true : false));
		m_UIMsgs.SetEnabled(uimsgFireMissileHint,	((dwLoad & 0x00004000) ? true : false));
		m_UIMsgs.SetEnabled(uimsgCommsHint,			((dwLoad & 0x00008000) ? true : false));
		}

	//	Maneuver controller

	if (Ctx.dwVersion >= 150)
		m_ManeuverController.ReadFromStream(Ctx);

	//	OLD: Deities

	if (Ctx.dwVersion < 25)
		{
		int iDummy;

		for (int i = 0; i < 2; i++)
			{
			Ctx.pStream->Read(iDummy);
			Ctx.pStream->Read(iDummy);

			DWORD dwCount;
			Ctx.pStream->Read(dwCount);
			for (int j = 0; j < (int)dwCount; j++)
				Ctx.pStream->Read(dwLoad);
			}
		}

	//	OLD: Deferred destruction string

	if (Ctx.dwVersion >= 3 && Ctx.dwVersion < 50)
		{
		CString sDummy;
		sDummy.ReadFromStream(Ctx.pStream);
		}

	m_pTrans = g_pTrans;
	}

void CPlayerShipController::ReadyNextMissile (int iDir)

//	ReadyNextMissile
//
//	Select the next missile
	
	{
	m_pShip->ReadyNextMissile(iDir);

	CInstalledDevice *pLauncher = m_pShip->GetNamedDevice(devMissileWeapon);
	if (pLauncher)
		{
		if (pLauncher->GetValidVariantCount(m_pShip) == 0)
			DisplayTranslate(CONSTLIT("msgNoMissiles"));
		else
			{
			CString sVariant;
			int iAmmoLeft;
			pLauncher->GetSelectedVariantInfo(m_pShip, &sVariant, &iAmmoLeft);
			if (sVariant.IsBlank())
				sVariant = pLauncher->GetDeviceItem().GetNounPhrase(nounShort | nounNoModifiers);

			DisplayTranslate(CONSTLIT("msgMissileReady"), CONSTLIT("missile"), sVariant);
			}

		if (m_pSession)
			m_pSession->OnWeaponStatusChanged();
		}
	else
		DisplayTranslate(CONSTLIT("msgNoLauncher"));
	}

void CPlayerShipController::ReadyNextWeapon (int iDir)

//	ReadyNextWeapon
//
//	Select the next primary weapon

	{
	CInstalledDevice *pCurWeapon = m_pShip->GetNamedDevice(devPrimaryWeapon);
	CInstalledDevice *pNewWeapon = pCurWeapon;
	DWORD dwLinkedFireSelected = CDeviceClass::lkfSelected | CDeviceClass::lkfSelectedVariant;

	DWORD bCurWeaponLkfSelected = (pCurWeapon != NULL) ? pCurWeapon->GetSlotLinkedFireOptions() & dwLinkedFireSelected : 0x0;
	DWORD bNextWeaponLkfSelected = (pCurWeapon != NULL) ? (pNewWeapon->GetSlotLinkedFireOptions() & dwLinkedFireSelected && pNewWeapon->GetUNID() == pCurWeapon->GetUNID()
		&& (pNewWeapon->GetSlotLinkedFireOptions() & CDeviceClass::lkfSelectedVariant ? CItemCtx(m_pShip, pNewWeapon).GetItemVariantNumber() == CItemCtx(m_pShip, pCurWeapon).GetItemVariantNumber()
			: true)) : 0x0;

	//	Keep switching until we find a weapon that is not disabled
	//  If currently selected weapon has LinkedFireSelectable, make sure next
	//  selected weapon is not (both of the same type AND set as linkedFire="LinkedFireSelected")
	//  Only then do we check to see if the weapon is enabled.

	do
		{
		m_pShip->ReadyNextWeapon(iDir);
		pNewWeapon = m_pShip->GetNamedDevice(devPrimaryWeapon);
		bCurWeaponLkfSelected = (pCurWeapon != NULL) ? pCurWeapon->GetSlotLinkedFireOptions() & dwLinkedFireSelected : 0x0;
		bNextWeaponLkfSelected = (pCurWeapon != NULL) ? (pNewWeapon->GetSlotLinkedFireOptions() & dwLinkedFireSelected && pNewWeapon->GetUNID() == pCurWeapon->GetUNID()
			&& (pNewWeapon->GetSlotLinkedFireOptions() & CDeviceClass::lkfSelectedVariant ? CItemCtx(m_pShip, pNewWeapon).GetItemVariantNumber() == CItemCtx(m_pShip, pCurWeapon).GetItemVariantNumber()
				: true)) : 0x0;
		}
	
	while (pNewWeapon 
			&& pCurWeapon
			&& pNewWeapon != pCurWeapon
			&& (bCurWeaponLkfSelected ? (bNextWeaponLkfSelected ? 
		(pNewWeapon->GetSlotLinkedFireOptions() & CDeviceClass::lkfSelectedVariant ? 
			(CItemCtx(m_pShip, pNewWeapon).GetItemVariantNumber() != CItemCtx(m_pShip, pCurWeapon).GetItemVariantNumber() || pNewWeapon->GetUNID() != pCurWeapon->GetUNID()) :
			pNewWeapon->GetUNID() != pCurWeapon->GetUNID()) : !pNewWeapon->IsEnabled()) : !pNewWeapon->IsEnabled()));

	//	Done

	if (pNewWeapon)
		{
		//	There is a delay in activation (except for linkedFire='whenSelected' guns)

		if (!(pNewWeapon->GetSlotLinkedFireOptions() & dwLinkedFireSelected))
			m_pShip->SetFireDelay(pNewWeapon);

		//	Feedback to player

		CString sItemName = pNewWeapon->GetDeviceItem().GetNounPhrase(nounShort | nounNoModifiers);
		if (pNewWeapon->IsEnabled())
			DisplayTranslate(CONSTLIT("msgWeaponReady"), CONSTLIT("itemName"), sItemName);
		else
			DisplayTranslate(CONSTLIT("msgDisabledWeaponReady"), CONSTLIT("itemName"), sItemName);
		}

	if (m_pSession)
		m_pSession->OnWeaponStatusChanged();
	}

void CPlayerShipController::Reset (void)

//	Reset
//
//	Resets the ship (used when changing systems, etc.)

	{
	DEBUG_TRY

	//	Clear target

	if (m_pTarget)
		{
		SetTarget(NULL);
		if (m_pSession)
			m_pSession->OnTargetChanged(m_pTarget);
		}

	//	Clear destination

	if (m_pDestination)
		SetDestination(NULL);

	//	Clear target list

	m_TargetList.DeleteAll();

	//	Clear orders

	m_OrderDesc = COrderDesc();

	//	Clear auto

	if (m_pAutoDamage)
		{
		m_pAutoDamage->ClearShowDamageBar();
		m_pAutoDamage = NULL;
		}

	m_pAutoDock = NULL;
	m_pAutoMining = NULL;
	m_pAutoTarget = NULL;
	m_bSignalDock = false;
	m_bTargetOutOfRange = false;

	DEBUG_CATCH
	}

void CPlayerShipController::SelectNearestTarget (void)

//	SelectNearestTarget
//
//	Selects the nearest enemy target

	{
	//	Must have a targeting computer

	if (!m_pShip->HasTargetingComputer())
		{
		DisplayTranslate(CONSTLIT("msgNoTargetingComputer"));
		return;
		}

	//	Initialize target list

	InitTargetList(ETargetClass::enemies);
	if (m_TargetList.GetCount() > 0)
		SetTarget(m_TargetList[0]);
	else
		SetTarget(NULL);
	}

void CPlayerShipController::SetDestination (CSpaceObject *pTarget, DWORD dwOptions)

//	SetDestination
//
//	Sets the destination for the player

	{
	//	Select

	if (m_pDestination)
		m_pDestination->ClearPlayerDestination();

	m_pDestination = pTarget;

	if (m_pDestination)
		{
		CSpaceObject::SPlayerDestinationOptions Options;
		if (dwOptions & OPTION_HIGHLIGHT)
			Options.bShowHighlight = true;

		m_pDestination->SetPlayerDestination(Options);
		}
	}

void CPlayerShipController::SetFireMain (bool bFire)

//	SetFireMain
//
//	Fire main weapon

	{
	m_pShip->SetWeaponTriggered(devPrimaryWeapon, bFire);
	}

void CPlayerShipController::SetFireMissile (bool bFire)

//	SetFireMissile
//
//	Fire launcher weapon

	{
	m_pShip->SetWeaponTriggered(devMissileWeapon, bFire);
	}

void CPlayerShipController::SetGameSession (CGameSession *pSession)

//	SetGameSession
//
//	Sets the game session.

	{
	m_pSession = pSession;

	//	If we have saved messages, output now.

	if (m_pSession)
		{
		for (int i = 0; i < m_SavedMessages.GetCount(); i++)
			DisplayMessage(m_SavedMessages[i]);

		m_SavedMessages.DeleteAll();
		}
	}

void CPlayerShipController::SetTarget (CSpaceObject *pTarget)

//	SetTarget
//
//	Sets the target

	{
	//	Select

	if (m_pTarget)
		{
		m_pTarget->ClearSelection();
		m_pTarget->ClearPlayerTarget();
		}

	m_pTarget = pTarget;

	if (m_pTarget)
		{
		m_pTarget->SetSelection();
		m_pTarget->SetPlayerTarget();
		}
	else
		{
		m_TargetList.DeleteAll();
		ClearFireAngle();
		}

	m_bTargetOutOfRange = false;
	}

void CPlayerShipController::SelectNextFriendly (int iDir)

//	SelectNextFriendly
//
//	Sets the target to the next nearest friendly object

	{
	//	Must have a targeting computer

	if (!m_pShip->HasTargetingComputer())
		{
		DisplayTranslate(CONSTLIT("msgNoTargetingComputer"));
		return;
		}

	//	If a friendly is already selected, then cycle
	//	to the next friendly.

	if (m_pTarget && !(m_pShip->IsAngryAt(m_pTarget) && m_pTarget->CanBeAttacked()))
		{
		InitTargetList(ETargetClass::friendlies, true);

		//	If we're going forwards, we default to first target; otherwise
		//	we default to last target
		
		int iDefault = (iDir == 1 ? 0 : m_TargetList.GetCount() - 1);

		//	Select

		if (m_TargetList.GetCount() > 0)
			{
			if (m_pTarget)
				{
				//	Look for the current target

				int iIndex;
				if (m_TargetList.FindByValue(m_pTarget, &iIndex))
					{
					iIndex += iDir;

					if (iIndex >= 0 && iIndex < m_TargetList.GetCount())
						SetTarget(m_TargetList[iIndex]);
					else
						SetTarget(m_TargetList[iDefault]);
					}
				else
					SetTarget(m_TargetList[iDefault]);
				}
			else
				SetTarget(m_TargetList[iDefault]);
			}
		else
			SetTarget(NULL);
		}

	//	Otherwise, select the nearest friendly

	else
		{
		InitTargetList(ETargetClass::friendlies);

		if (m_TargetList.GetCount() > 0)
			SetTarget(m_TargetList[iDir == 1 ? 0 : m_TargetList.GetCount() - 1]);
		else
			SetTarget(NULL);
		}
	}

void CPlayerShipController::SelectNextTarget (int iDir)

//	SelectNextTarget
//
//	Selects the next target

	{
	//	Must have a targeting computer

	if (!m_pShip->HasTargetingComputer())
		{
		DisplayTranslate(CONSTLIT("msgNoTargetingComputer"));
		return;
		}

	//	If an enemy target is already selected, then cycle
	//	to the next enemy.

	if (m_pTarget && m_pShip->IsAngryAt(m_pTarget) && m_pTarget->CanBeAttacked())
		{
		InitTargetList(ETargetClass::enemies, true);

		//	If we're going forwards, we default to first target; otherwise
		//	we default to last target
		
		int iDefault = (iDir == 1 ? 0 : m_TargetList.GetCount() - 1);

		//	Select

		if (m_TargetList.GetCount() > 0)
			{
			if (m_pTarget)
				{
				//	Look for the current target

				int iIndex;
				if (m_TargetList.FindByValue(m_pTarget, &iIndex))
					{
					iIndex += iDir;

					if (iIndex >= 0 && iIndex < m_TargetList.GetCount())
						SetTarget(m_TargetList[iIndex]);
					else
						SetTarget(m_TargetList[iDefault]);
					}
				else
					SetTarget(m_TargetList[iDefault]);
				}
			else
				SetTarget(m_TargetList[iDefault]);
			}
		else
			SetTarget(NULL);
		}

	//	Otherwise, select the nearest enemy target

	else
		{
		InitTargetList(ETargetClass::enemies);

		if (m_TargetList.GetCount() > 0)
			SetTarget(m_TargetList[iDir == 1 ? 0 : m_TargetList.GetCount() - 1]);
		else
			SetTarget(NULL);
		}
	}

ALERROR CPlayerShipController::SwitchShips (CShip *pNewShip, SPlayerChangedShipsCtx &Options)

//	SwitchShips
//
//	Switches the player's ship

	{
	ASSERT(m_pShip);
	CShip *pOldShip = m_pShip;

	//	If the new ship is docked with a station then make sure that it
	//	is not a subordinate of the station.

	CSpaceObject *pNewShipDockedAt = pNewShip->GetDockedObj();
	if (pNewShipDockedAt)
		pNewShipDockedAt->RemoveSubordinate(pNewShip);

	//	If we want to take over the docking port, then do it now.

	if (m_pStation 
			&& Options.bTakeDockingPort)
		{
		CDockingPorts *pDockingPorts = m_pStation->GetDockingPorts();

		//	Figure out the port where we're currently docked.

		int iOldPort;
		if (!pDockingPorts->IsObjDocked(pOldShip, &iOldPort))
			iOldPort = -1;

		//	Make sure both ships are undocked.

		pOldShip->Undock();
		pNewShip->Undock();

		//	Dock the new ship. If we fail, then we need to exit any screen
		//	session.

		if (iOldPort == -1
				|| !pDockingPorts->DockAtPort(m_pStation, pNewShip, iOldPort))
			{
			g_pTrans->GetModel().ExitScreenSession(true);
			pNewShip->Undock();
			}

		//	Success!

		}

	//	If new and old ships don't have the same docking state, then we need to
	//	undock both.

	else if (m_pStation != pNewShipDockedAt)
		{
		if (m_pStation)
			g_pTrans->GetModel().ExitScreenSession(true);

		if (pNewShipDockedAt)
			pNewShip->Undock();
		}

	//	Set a new controller for the old ship (but do not free
	//	the previous controller, which is us)

	pOldShip->SetController(m_Universe.CreateShipController(NULL_STR), false);
	pOldShip->GetController()->AddOrder(COrderDesc(IShipController::orderWait));

	//	Old ship stops tracking fuel (otherwise, it would run out)

	pOldShip->TrackFuel(false);
	pOldShip->TrackMass(false);

	//	Transfer cargo from the old ship to the new one

	if (Options.bTransferCargo)
		{
		CItemListManipulator Src(pOldShip->GetItemList());
		CItemListManipulator Dest(pNewShip->GetItemList());

		Src.ResetCursor();
		while (Src.MoveCursorForward())
			{
			CItem Item = Src.GetItemAtCursor();
			if (Item.IsInstalled() || Item.IsVirtual())
				continue;

			Dest.AddItem(Item);
			Src.DeleteAtCursor(Item.GetCount());
			Src.MoveCursorBack();
			}
		}

	//	Transfer equipment from the old ship to the new one (if necessary).
	//
	//	NOTE: This transfers equipment such as targeting computers that are 
	//	installed on the ship. It does not include (not should it include) 
	//	abilities/equipment conferred by devices or enhancements.

	if (Options.bTransferEquipment)
		{
		pNewShip->GetNativeAbilities().Set(pOldShip->GetNativeAbilities());
		}

	//	Identify items on the new ship

	if (Options.bIdentifyInstalled)
		{
		CItemListManipulator Dest(pNewShip->GetItemList());
		Dest.ResetCursor();
		while (Dest.MoveCursorForward())
			{
			CItem Item = Dest.GetItemAtCursor();
			if (Item.IsInstalled())
				Item.SetKnown();
			}
		}

	//	Now set this controller to drive the new ship. gPlayer and gPlayerShip
	//	will be set inside of SetPlayerShip.

	pNewShip->SetController(this);
	m_pShip = pNewShip;
	m_Universe.SetPlayerShip(pNewShip);
	m_Universe.SetPOV(pNewShip);
	pNewShip->SetSovereign(m_Universe.FindSovereign(g_PlayerSovereignUNID));

	//	Move any data from the old ship to the new ship
	//	(we leave it on the old ship just in case)

	pNewShip->CopyDataFromObj(pOldShip);

	//	New ship gets some properties by default

	pNewShip->TrackFuel();
	pNewShip->TrackMass();

	//	Transfer event registrations

	pNewShip->AddEventSubscribers(pOldShip->GetEventSubscribers());
	pOldShip->RemoveAllEventSubscribers();

	//	Reset all other settings

	SetTarget(NULL);
	SetDestination(NULL);
	m_pAutoDock = NULL;
	m_iManeuver = EManeuver::None;
	m_bThrust = false;
	m_bStopThrust = false;
	SetFireMain(false);
	SetFireMissile(false);
	m_bActivate = false;

	//  Tell the ships that we've switched

	pOldShip->FireOnPlayerLeftShip(pNewShip);
	pNewShip->FireOnPlayerEnteredShip(pOldShip);

	//	Tell the model that we changed ships

	g_pTrans->GetModel().OnPlayerChangedShips(pOldShip, pNewShip, Options);

	//	Update stats

	m_Stats.OnSwitchPlayerShip(*pNewShip, pOldShip);

	//	Update displays

	if (m_pSession)
		m_pSession->OnPlayerChangedShips(pOldShip);

	return NOERROR;
	}

void CPlayerShipController::Undock (void)
	{
	if (m_pStation)
		{
		//	If we're docked, then let the ship undock us.

		if (m_pShip->GetDockedObj())
			m_pShip->Undock();

		//	Otherwise, we're in the middle of docking, so tell the station to
		//	cancel docking (the ship does not yet have m_pDocked set).

		else
			m_pStation->Undock(m_pShip);

		//	Done

		m_pStation = NULL;
		}
	}

void CPlayerShipController::Update (int iTick)

//	Update
//
//	Updates each tick

	{
	DEBUG_TRY

	//	Update help

	if ((iTick % UPDATE_HELP_TIME) == 0)
		UpdateHelp(iTick);

#ifdef DEBUG_TARGET_LIST
//	if ((iTick % 30) == 0)
//		InitTargetList(targetEnemies, true);

	for (int i = 0; i < m_TargetList.GetCount(); i++)
		{
		CSpaceObject *pObj = m_TargetList.Get(i);
		ASSERT(!pObj->IsDestroyed());
		ASSERT(pObj->GetType());

		if ((iTick % 30) == 0)
			::kernelDebugLogPattern("Target %d: %s (%x)", i, pObj->GetNounPhrase(), (DWORD)pObj);
		}
#endif

	DEBUG_CATCH
	}

void CPlayerShipController::UpdateHelp (int iTick)

//	UpdateHelp
//
//	Checks to see if we should show a help message to the player

	{
	//	Must have a session

	if (m_pSession == NULL)
		return;

	//	If we just showed help, then skip

	if ((iTick - m_iLastHelpTick) < 240)
		return;

	//	See if we need to show help and if we can show help at this point

	if (!m_UIMsgs.IsEnabled(uimsgAllHints))
		return;

	//	Don't bother if we're docking, or gating, etc.

	if (!m_pTrans->InGameState() 
			|| DockingInProgress() 
			|| m_pTrans->InAutopilot()
			|| m_pSession->InMenu()
			|| m_pShip->IsDestroyed()
			|| m_pShip->GetSystem() == NULL)
		return;

	//	See if there are enemies in the area

	bool bEnemiesInRange = m_pShip->IsEnemyInRange(MAX_IN_COMBAT_RANGE, true);

	//	If we've never entered a gate, and there is a gate nearby
	//	and we're not in the middle of anything, then tell the player.

	if (m_UIMsgs.IsEnabled(uimsgGateHint))
		{
		if (!bEnemiesInRange
				&& !m_pSession->InSystemMap()
				&& m_pShip->IsStargateInRange(CSystem::MAX_GATE_HELP_RANGE)
				&& m_UIMsgs.ShowMessage(m_Universe, uimsgGateHint))
			{
			if (m_pTrans->GetSettings().GetKeyMap().IsKeyBound(CGameKeys::keyInteract))
				DisplayCommandHint(CGameKeys::keyInteract, Translate(CONSTLIT("hintEnterGate")));
			else
				DisplayCommandHint(CGameKeys::keyEnterGate, Translate(CONSTLIT("hintEnterGate")));

			m_iLastHelpTick = iTick;
			return;
			}
		}

	//	If we've never docked and we're near a dockable station, then tell
	//	the player about docking.

	if (m_UIMsgs.IsEnabled(uimsgDockHint))
		{
		if (!bEnemiesInRange
				&& !m_pSession->InSystemMap()
				&& m_pAutoDock
				&& !m_pStation
				&& !m_pShip->IsStargateInRange(CSystem::MAX_GATE_HELP_RANGE)
				&& m_UIMsgs.ShowMessage(m_Universe, uimsgDockHint))
			{
			if (m_pTrans->GetSettings().GetKeyMap().IsKeyBound(CGameKeys::keyInteract))
				DisplayCommandHint(CGameKeys::keyInteract, Translate(CONSTLIT("hintDock")));
			else
				DisplayCommandHint(CGameKeys::keyDock, Translate(CONSTLIT("hintDock")));

			m_iLastHelpTick = iTick;
			return;
			}
		}

	//	Tell player about thrusting

	if (m_UIMsgs.ShowMessage(m_Universe, uimsgKeyboardManeuverHint))
		{
		DisplayCommandHint(CGameKeys::keyThrustForward, Translate(CONSTLIT("hintThrust")));
		m_iLastHelpTick = iTick;
		return;
		}

	//	Squadron UI

	if (m_UIMsgs.IsEnabled(uimsgSquadronUIHint))
		{
		if (m_bHasSquadron
				&& m_UIMsgs.ShowMessage(m_Universe, uimsgSquadronUIHint)
				&& CanShowShipStatus())
			{
			DisplayCommandHint(CGameKeys::keySquadronUI, Translate(CONSTLIT("hintSquadronUI")));
			m_iLastHelpTick = iTick;
			return;
			}
		}

	//	If we've never used an item, and we've got a usable item in cargo
	//	and we're not in the middle of anything, then tell the player.

	if (m_UIMsgs.IsEnabled(uimsgUseItemHint) && !bEnemiesInRange)
		{
		CItemListManipulator ItemList(m_pShip->GetItemList());
		CItemCriteria UsableItems(CONSTLIT("u"));
		ItemList.SetFilter(UsableItems);
		bool bHasUsableItems = ItemList.MoveCursorForward();

		if (!m_pSession->InSystemMap()
				&& bHasUsableItems
				&& m_UIMsgs.ShowMessage(m_Universe, uimsgUseItemHint))
			{
			DisplayCommandHint(CGameKeys::keyUseItem, Translate(CONSTLIT("hintUseItem")));
			m_iLastHelpTick = iTick;
			return;
			}
		}

	//	If we have something in our cargo hold other than fuel (or usable items)
	//	then tell the player how to view inventory.

	if (m_UIMsgs.IsEnabled(uimsgShipStatusHint) && !bEnemiesInRange)
		{
		CItemListManipulator ItemList(m_pShip->GetItemList());
		CItemCriteria NonFuelItems(CONSTLIT("*~fU -noUseItemHint;"));
		ItemList.SetFilter(NonFuelItems);
		bool bHasNonFuelItems = ItemList.MoveCursorForward();

		if (!m_pSession->InSystemMap()
				&& bHasNonFuelItems
				&& m_UIMsgs.ShowMessage(m_Universe, uimsgShipStatusHint)
				&& CanShowShipStatus())
			{
			DisplayCommandHint(CGameKeys::keyShipStatus, Translate(CONSTLIT("hintShipStatus")));
			m_iLastHelpTick = iTick;
			return;
			}
		}

	//	If we've never used the map, and then tell the player about the map

	if (m_UIMsgs.IsEnabled(uimsgMapHint))
		{
		if (!bEnemiesInRange
				&& !m_pSession->InSystemMap()
				&& m_UIMsgs.ShowMessage(m_Universe, uimsgMapHint))
			{
			DisplayCommandHint(CGameKeys::keyShowMap, Translate(CONSTLIT("hintShowMap")));
			m_iLastHelpTick = iTick;
			return;
			}
		}

	//	If we've never used autopilot, and we're not in the middle of
	//	anything, then tell the player about autopilot

	if (m_UIMsgs.IsEnabled(uimsgAutopilotHint))
		{
		if (!bEnemiesInRange 
				&& m_pShip->HasAutopilot()
				&& m_pShip->GetVel().Length() > 0.9 * m_pShip->GetMaxSpeed()
				&& !g_pTrans->InAutopilot()
				&& m_UIMsgs.ShowMessage(m_Universe, uimsgAutopilotHint))
			{
			DisplayCommandHint(CGameKeys::keyAutopilot, Translate(CONSTLIT("hintAutopilot")));
			m_iLastHelpTick = iTick;
			return;
			}
		}

	//	If we've never communicated and there are objects to communicate with

	if (m_UIMsgs.IsEnabled(uimsgCommsHint))
		{
		if (!bEnemiesInRange
				&& HasCommsTarget()
				&& m_UIMsgs.ShowMessage(m_Universe, uimsgCommsHint))
			{
			DisplayCommandHint(CGameKeys::keyCommunications, Translate(CONSTLIT("hintCommunications")));
			m_iLastHelpTick = iTick;
			return;
			}
		}

	//	If we've never switched missiles and we have more than one

	if (m_UIMsgs.IsEnabled(uimsgSwitchMissileHint))
		{
		if (!bEnemiesInRange
				&& m_pShip->GetMissileCount() > 1
				&& m_UIMsgs.ShowMessage(m_Universe, uimsgSwitchMissileHint))
			{
			DisplayCommandHint(CGameKeys::keyNextMissile, Translate(CONSTLIT("hintNextMissile")));
			m_iLastHelpTick = iTick;
			return;
			}
		}

	//	If we've never fired a weapon and we're under attack, tell the player.

	if (m_UIMsgs.IsEnabled(uimsgFireWeaponHint))
		{
		if (bEnemiesInRange
				&& m_UIMsgs.ShowMessage(m_Universe, uimsgFireWeaponHint))
			{
			DisplayCommandHint(CGameKeys::keyFireWeapon, Translate(CONSTLIT("hintFireWeapon")));
			m_iLastHelpTick = iTick;
			return;
			}
		}

	//	If we've never fired a missile and we have one and we're under attack

	if (m_UIMsgs.IsEnabled(uimsgFireMissileHint))
		{
		if (bEnemiesInRange
				&& m_pShip->GetMissileCount() > 0 
				&& m_UIMsgs.ShowMessage(m_Universe, uimsgFireMissileHint))
			{
			DisplayCommandHint(CGameKeys::keyFireMissile, Translate(CONSTLIT("hintFireMissile")));
			m_iLastHelpTick = iTick;
			return;
			}
		}

	//	If we've never used the galactic map, and then tell the player about the map

	if (m_UIMsgs.IsEnabled(uimsgGalacticMapHint))
		{
		if (!bEnemiesInRange
				&& IsGalacticMapAvailable()
				&& m_Stats.HasVisitedMultipleSystems()
				&& m_UIMsgs.ShowMessage(m_Universe, uimsgGalacticMapHint))
			{
			DisplayCommandHint(CGameKeys::keyShowGalacticMap, Translate(CONSTLIT("hintShowGalacticMap")));
			m_iLastHelpTick = iTick;
			return;
			}
		}
	}

void CPlayerShipController::WriteToStream (IWriteStream *pStream)

//	WriteToStream
//
//	Write data to stream
//
//	DWORD		m_iGenome
//	DWORD		m_dwStartingShipClass
//	CString		m_sStartingSystem
//	DWORD		m_dwCharacterClass
//	DWORD		m_pShip (CSpaceObject ref)
//	DWORD		m_pStation (CSpaceObject ref)
//	DWORD		m_pTarget (CSpaceObject ref)
//	DWORD		m_pDestination (CSpaceObject ref)
//	COrderDesc	m_OrderDesc
//	DWORD		m_iManeuver
//	CCurrencyBlock m_Credits
//	CPlayerGameStats m_Stats
//	DWORD		UNUSED (m_iInsuranceClaims)
//	DWORD		flags
//	CUIMessageController
//	CManeuverController

	{
	DWORD dwSave;

	GetClass().WriteToStream(pStream);

	pStream->Write((DWORD)m_iGenome);
	pStream->Write(m_dwStartingShipClass);
	m_sStartingSystem.WriteToStream(pStream);

	dwSave = (m_pCharacterClass ? m_pCharacterClass->GetUNID() : 0);
	pStream->Write(dwSave);

	m_pShip->WriteObjRefToStream(m_pShip, pStream);
	m_pShip->WriteObjRefToStream(m_pStation, pStream);
	m_pShip->WriteObjRefToStream(m_pTarget, pStream);
	m_pShip->WriteObjRefToStream(m_pDestination, pStream);

	m_OrderDesc.WriteToStream(*pStream, *m_pShip);

	pStream->Write((DWORD)m_iManeuver);
	m_Credits.WriteToStream(pStream);
	m_Stats.WriteToStream(pStream);
	pStream->Write((DWORD)0);

	dwSave = 0;
	dwSave |= (m_bUnderAttack ?			0x00000001 : 0);
	//	0x00000002
	//	0x00000004
	//	0x00000008
	//	0x00000010
	//	0x00000020
	//	0x00000040
	//	0x00000080
	//	0x00000100
	//	0x00000200
	//	0x00000400
	//	0x00000800
	dwSave |= (m_bMapHUD ?				0x00001000 : 0);
	//	0x00002000
	//	0x00004000
	//	0x00008000
	pStream->Write(dwSave);

	m_UIMsgs.WriteToStream(pStream);

	//	Maneuver controller

	m_ManeuverController.WriteToStream(*pStream);
	}
