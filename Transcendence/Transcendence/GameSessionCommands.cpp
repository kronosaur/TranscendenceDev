//	CGameSession.cpp
//
//	CGameSession class
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Transcendence.h"

void CGameSession::ExecuteCommand (CPlayerShipController *pPlayer, CGameKeys::Keys iCommand)

//	ExecuteCommand
//
//	Executes the given command.

	{
	ASSERT(pPlayer);
	if (pPlayer == NULL)
		return;

	switch (iCommand)
		{
		case CGameKeys::keyAutopilot:
			g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_SELECT));
			g_pTrans->Autopilot(!g_pTrans->m_bAutopilot);
			pPlayer->SetUIMessageFollowed(uimsgAutopilotHint);
			break;

		case CGameKeys::keyEnableDevice:
			if (!pPlayer->DockingInProgress() 
					&& !pPlayer->GetShip()->IsParalyzed()
					&& !pPlayer->GetShip()->IsOutOfPower()
					&& !pPlayer->GetShip()->IsTimeStopped())
				{
				g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_SELECT));
				g_pTrans->Autopilot(false);
				ShowMenu(menuEnableDevice);
				}
			break;

		case CGameKeys::keyCommunications:
			if (!pPlayer->DockingInProgress()
					&& !pPlayer->GetShip()->IsTimeStopped())
				{
				g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_SELECT));
				g_pTrans->Autopilot(false);
				ShowMenu(menuCommsTarget);
				}
			break;

		case CGameKeys::keyDock:
			if (!pPlayer->GetShip()->IsOutOfPower()
					&& !pPlayer->GetShip()->IsParalyzed()
					&& !pPlayer->GetShip()->IsTimeStopped())
				{
				g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_BUTTON_CLICK));
				g_pTrans->Autopilot(false);
				pPlayer->Dock();
				}
			break;

		case CGameKeys::keyInteract:
			if (pPlayer->DockingInProgress())
				{
				g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_BUTTON_CLICK));
				g_pTrans->Autopilot(false);
				pPlayer->Dock();
				}
			else if (pPlayer->GetShip()->IsParalyzed()
					|| pPlayer->GetShip()->IsOutOfPlaneObj()
					|| pPlayer->GetShip()->IsTimeStopped())
				{ }
			else if (GetUniverse().GetCurrentSystem()->GetStargateInRange(pPlayer->GetShip()->GetPos()))
				{
				g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_BUTTON_CLICK));
				g_pTrans->Autopilot(false);
				pPlayer->Gate();
				}
			else
				{
				g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_BUTTON_CLICK));
				g_pTrans->Autopilot(false);
				pPlayer->Dock();
				}
			break;

		case CGameKeys::keyCycleTarget:
			g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_SELECT));
			pPlayer->CycleTarget(0);
			break;

		case CGameKeys::keyTargetNextEnemy:
			g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_SELECT));
			pPlayer->SelectNextTarget(1);
			break;

		case CGameKeys::keyTargetPrevEnemy:
			g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_SELECT));
			pPlayer->SelectNextTarget(-1);
			break;

		case CGameKeys::keyTargetNextFriendly:
			g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_SELECT));
			pPlayer->SelectNextFriendly(1);
			break;

		case CGameKeys::keyTargetPrevFriendly:
			g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_SELECT));
			pPlayer->SelectNextFriendly(-1);
			break;

		case CGameKeys::keyClearTarget:
			g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_SELECT));
			pPlayer->SetTarget(NULL);
			break;

		case CGameKeys::keyEnterGate:
			if (!pPlayer->DockingInProgress()
					&& !pPlayer->GetShip()->IsParalyzed()
					&& !pPlayer->GetShip()->IsOutOfPower()
					&& !pPlayer->GetShip()->IsTimeStopped())
				{
				g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_BUTTON_CLICK));
				g_pTrans->Autopilot(false);
				pPlayer->Gate();
				}
			break;

		case CGameKeys::keyInvokePower:
			if (!pPlayer->DockingInProgress()
					&& !pPlayer->GetShip()->IsTimeStopped())
				{
				g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_SELECT));
				g_pTrans->Autopilot(false);
				ShowMenu(menuInvoke);
				}
			break;

		case CGameKeys::keyShowMap:
			g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_SELECT));
			if (m_bShowingSystemMap)
				g_pTrans->Autopilot(false);
            ShowSystemMap(!m_bShowingSystemMap);
			pPlayer->SetUIMessageFollowed(uimsgMapHint);
			break;

		case CGameKeys::keyShowGalacticMap:
			g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_SELECT));
			g_pHI->HICommand(CONSTLIT("uiShowGalacticMap"));
			pPlayer->SetUIMessageFollowed(uimsgGalacticMapHint);
			break;

		case CGameKeys::keyPause:
			{
			g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_BUTTON_CLICK));
			g_pTrans->m_bPaused = true;
			if (pPlayer)
				{
				pPlayer->SetThrust(false);
				pPlayer->SetManeuver(EManeuver::None);
				pPlayer->SetFireMain(false);
				pPlayer->SetFireMissile(false);
				}
			g_pHI->HICommand(CONSTLIT("gamePause"));
			break;
			}

		case CGameKeys::keySquadronCommands:
			if (!pPlayer->DockingInProgress()
					&& !pPlayer->GetShip()->IsTimeStopped())
				{
				g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_SELECT));
				g_pTrans->Autopilot(false);
				ShowMenu(menuCommsSquadron);
				}
			break;

		case CGameKeys::keyShipStatus:
			if (pPlayer->CanShowShipStatus())
				{
				g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_SELECT));
				m_Model.ShowShipScreen();
				pPlayer->SetUIMessageFollowed(uimsgShipStatusHint);
				}
			break;

		case CGameKeys::keyUseItem:
			if (!pPlayer->DockingInProgress()
					&& !pPlayer->GetShip()->IsTimeStopped())
				{
				g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_SELECT));
				g_pTrans->Autopilot(false);
				ShowMenu(menuUseItem);
				}
			break;

		case CGameKeys::keyNextWeapon:
			g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_SELECT));
			g_pTrans->Autopilot(false);
			pPlayer->SetFireMain(false);
			pPlayer->ReadyNextWeapon(1);
			break;

		case CGameKeys::keyPrevWeapon:
			g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_SELECT));
			g_pTrans->Autopilot(false);
			pPlayer->SetFireMain(false);
			pPlayer->ReadyNextWeapon(-1);
			break;

		case CGameKeys::keyThrustForward:
			if (!pPlayer->DockingInProgress()
					&& !pPlayer->GetShip()->IsOutOfPower()
					&& !pPlayer->GetShip()->IsTimeStopped())
				{
				g_pTrans->Autopilot(false);
				pPlayer->SetThrust(true);
				pPlayer->SetUIMessageFollowed(uimsgMouseManeuverHint);
				pPlayer->SetUIMessageFollowed(uimsgKeyboardManeuverHint);
				}
			break;

		case CGameKeys::keyRotateLeft:
			if (!pPlayer->GetShip()->IsOutOfPower()
					&& !pPlayer->GetShip()->IsTimeStopped())
				{
				g_pTrans->Autopilot(false);
				pPlayer->SetManeuver(EManeuver::RotateLeft);
				pPlayer->SetUIMessageFollowed(uimsgKeyboardManeuverHint);
				}
			break;

		case CGameKeys::keyRotateRight:
			if (!pPlayer->GetShip()->IsOutOfPower()
					&& !pPlayer->GetShip()->IsTimeStopped())
				{
				g_pTrans->Autopilot(false);
				pPlayer->SetManeuver(EManeuver::RotateRight);
				pPlayer->SetUIMessageFollowed(uimsgKeyboardManeuverHint);
				}
			break;

		case CGameKeys::keyStop:
			if (!pPlayer->DockingInProgress()
					&& !pPlayer->GetShip()->IsOutOfPower()
					&& !pPlayer->GetShip()->IsTimeStopped())
				{
				g_pTrans->Autopilot(false);
				pPlayer->SetStopThrust(true);
				}
			break;

		case CGameKeys::keyFireWeapon:
			if (!pPlayer->DockingInProgress()
					&& !pPlayer->GetShip()->IsOutOfPower()
					&& !pPlayer->GetShip()->IsTimeStopped())
				{
				g_pTrans->Autopilot(false);
				pPlayer->SetFireMain(true);
				pPlayer->SetUIMessageFollowed(uimsgFireWeaponHint);
				}
			break;

		case CGameKeys::keyFireMissile:
			if (!pPlayer->DockingInProgress()
					&& !pPlayer->GetShip()->IsOutOfPower()
					&& !pPlayer->GetShip()->IsTimeStopped())
				{
				g_pTrans->Autopilot(false);
				pPlayer->SetFireMissile(true);
				pPlayer->SetUIMessageFollowed(uimsgFireMissileHint);
				}
			break;

		case CGameKeys::keyNextMissile:
			g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_SELECT));
			g_pTrans->Autopilot(false);
			pPlayer->ReadyNextMissile(1);
			pPlayer->SetUIMessageFollowed(uimsgSwitchMissileHint);
			break;

		case CGameKeys::keyPrevMissile:
			g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_SELECT));
			g_pTrans->Autopilot(false);
			pPlayer->ReadyNextMissile(-1);
			pPlayer->SetUIMessageFollowed(uimsgSwitchMissileHint);
			break;

		case CGameKeys::keyShowHelp:
			g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_SELECT));
			g_pHI->HICommand(CONSTLIT("uiShowHelp"));
			break;

		case CGameKeys::keyShowGameStats:
			g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_SELECT));
			g_pHI->HICommand(CONSTLIT("uiShowGameStats"));
			break;

		case CGameKeys::keyVolumeDown:
			g_pHI->HICommand(CONSTLIT("uiVolumeDown"));
			g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_SELECT));
			break;

		case CGameKeys::keyVolumeUp:
			g_pHI->HICommand(CONSTLIT("uiVolumeUp"));
			g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_SELECT));
			break;

		case CGameKeys::keyShowConsole:
			{
			if (m_Settings.GetBoolean(CGameSettings::debugMode)
					&& !g_pUniverse->IsRegistered())
				{
				g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_SELECT));
				if (m_CurrentMenu == menuDebugConsole)
					HideMenu();
				else
					ShowMenu(menuDebugConsole);
				}
			break;
			}

		case CGameKeys::keyEnableAllDevices:
			if (!pPlayer->GetShip()->IsTimeStopped()
					&& !pPlayer->GetShip()->IsParalyzed()
					&& !pPlayer->GetShip()->IsOutOfPower())
				{
				g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_BUTTON_CLICK));
				pPlayer->SetUIMessageFollowed(uimsgEnableDeviceHint);
				pPlayer->EnableAllDevices(true);
				}
			break;

		case CGameKeys::keyDisableAllDevices:
			if (!pPlayer->GetShip()->IsTimeStopped()
					&& !pPlayer->GetShip()->IsParalyzed()
					&& !pPlayer->GetShip()->IsOutOfPower())
				{
				g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_BUTTON_CLICK));
				pPlayer->SetUIMessageFollowed(uimsgEnableDeviceHint);
				pPlayer->EnableAllDevices(false);
				}
			break;

		case CGameKeys::keyEnableAllDevicesToggle:
			if (!pPlayer->GetShip()->IsTimeStopped()
					&& !pPlayer->GetShip()->IsParalyzed()
					&& !pPlayer->GetShip()->IsOutOfPower())
				{
				g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_BUTTON_CLICK));
				pPlayer->SetUIMessageFollowed(uimsgEnableDeviceHint);
				pPlayer->EnableAllDevices(!pPlayer->AreAllDevicesEnabled());
				}
			break;

		default:
			{
			if (iCommand >= CGameKeys::keyEnableDeviceToggle00 
					&& iCommand <= CGameKeys::keyEnableDeviceToggle31)
				{
				if (!pPlayer->GetShip()->IsTimeStopped()
						&& !pPlayer->GetShip()->IsParalyzed()
						&& !pPlayer->GetShip()->IsOutOfPower())
					{
					int iDevice = (iCommand - CGameKeys::keyEnableDeviceToggle00);

					g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_BUTTON_CLICK));
					pPlayer->SetUIMessageFollowed(uimsgEnableDeviceHint);
					pPlayer->ToggleEnableDevice(iDevice);
					}
				}
			break;
			}
		}
	}

void CGameSession::ExecuteCommandEnd (CPlayerShipController *pPlayer, CGameKeys::Keys iCommand)

//	ExecuteCommandEnd
//
//	For statefull commands (e.g., thrust) this stops the command.

	{
	switch (iCommand)
		{
		case CGameKeys::keyThrustForward:
			pPlayer->SetThrust(false);
			break;

		case CGameKeys::keyRotateLeft:
			if (pPlayer->GetManeuver() == EManeuver::RotateLeft)
				pPlayer->SetManeuver(EManeuver::None);
			break;

		case CGameKeys::keyRotateRight:
			if (pPlayer->GetManeuver() == EManeuver::RotateRight)
				pPlayer->SetManeuver(EManeuver::None);
			break;

		case CGameKeys::keyStop:
			pPlayer->SetStopThrust(false);
			break;

		case CGameKeys::keyFireWeapon:
			pPlayer->SetFireMain(false);
			break;

		case CGameKeys::keyFireMissile:
			pPlayer->SetFireMissile(false);
			break;

		case CGameKeys::keyPause:
			if (g_pTrans->m_bPaused)
				{
				g_pUniverse->PlaySound(NULL, g_pUniverse->FindSound(UNID_DEFAULT_BUTTON_CLICK));

				g_pTrans->m_bPaused = false;
				g_pHI->HICommand(CONSTLIT("gameUnpause"));
				}
			break;
		}
	}

void CGameSession::ExecuteCommandRefresh (void)
	{
	if (CPlayerShipController *pPlayer = m_Model.GetPlayer())
		ExecuteCommandRefresh(*pPlayer);
	}

void CGameSession::ExecuteCommandRefresh (CPlayerShipController &Player)

//	ExecuteCommandRefresh
//
//	For stateful commands, set to the correct state based on the key state.

	{
	const CGameKeys &Keys = m_Settings.GetKeyMap();

	//	If we've changed state due to a key press, then check to see if that key 
	//	also maps to a command. If it does, then we ignore it for purposes of
	//	setting state.

	CGameKeys::Keys iCmdTrigger = Keys.GetGameCommand(m_HI.GetLastVirtualKey());

	Player.SetThrust(Keys.IsKeyDown(CGameKeys::keyThrustForward) && iCmdTrigger != CGameKeys::keyThrustForward);
	Player.SetStopThrust(Keys.IsKeyDown(CGameKeys::keyStop) && iCmdTrigger != CGameKeys::keyStop);
	Player.SetFireMain(Keys.IsKeyDown(CGameKeys::keyFireWeapon) && iCmdTrigger != CGameKeys::keyFireWeapon);
	Player.SetFireMissile(Keys.IsKeyDown(CGameKeys::keyFireMissile) && iCmdTrigger != CGameKeys::keyFireMissile);

	if (Keys.IsKeyDown(CGameKeys::keyRotateLeft))
		Player.SetManeuver(EManeuver::RotateLeft);
	else if (Keys.IsKeyDown(CGameKeys::keyRotateRight))
		Player.SetManeuver(EManeuver::RotateRight);
	else
		Player.SetManeuver(EManeuver::None);
	}
