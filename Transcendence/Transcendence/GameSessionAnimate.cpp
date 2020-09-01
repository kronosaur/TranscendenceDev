//	CGameSession.cpp
//
//	CGameSession class
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Transcendence.h"

const CG32bitPixel BAR_COLOR = CG32bitPixel(0, 2, 10);
// TODO: Modify with FG and BG screens. If identical, then should have same behaviour as now.
// Replace existing function with FG/BG screen, and modify calls to call with single FG/BG
void CGameSession::OnAnimate (CG32bitImage &ScreenFG, CG32bitImage &ScreenBG, bool bTopMost)

//  OnAnimate
//
//  Paint once per frame

    {
	DEBUG_TRY

    CSmartLock Lock(g_pUniverse->GetSem());

	bool bFailed = false;

	//	Update context

	SSystemUpdateCtx UpdateCtx;

	try
		{
		SetProgramState(psAnimating);

		//	Do the appropriate thing

		switch (g_pTrans->m_State)
			{
            case CTranscendenceWnd::gsInGame:
			case CTranscendenceWnd::gsDestroyed:
				{
				CPlayerShipController *pPlayer = m_Model.GetPlayer();

				DWORD dwStartTimer;
				if (m_Settings.GetBoolean(CGameSettings::debugVideo))
					dwStartTimer = ::GetTickCount();

				//	Paint the main image

				PaintSRS(ScreenFG, ScreenBG);

				//	Paint various displays

				SetProgramState(psPaintingLRS);

				bool bShowMapHUD = (pPlayer && pPlayer->IsMapHUDActive());
				if (!m_bShowingSystemMap || bShowMapHUD)
					{
                    m_HUD.Update(g_pUniverse->GetFrameTicks());
                    m_HUD.Paint(ScreenFG, g_pUniverse->GetFrameTicks());

					SetProgramState(psPaintingDeviceDisplay);
					g_pTrans->m_DeviceDisplay.Paint(ScreenFG);
					}

				if (!m_bShowingSystemMap && g_pTrans->m_State != CTranscendenceWnd::gsDestroyed)
					{
					m_Narrative.Update(g_pUniverse->GetFrameTicks());
					m_Narrative.Paint(ScreenFG, g_pUniverse->GetFrameTicks());
					}

				if (!IsInPickerCompatible())
					{
					SetProgramState(psPaintingMessageDisplay);
					m_MessageDisplay.Paint(ScreenFG);
					}

				SetProgramState(psAnimating);

				m_IconBar.Paint(ScreenFG, g_pUniverse->GetFrameTicks());
				PaintMenu(ScreenFG);
				m_DebugConsole.Paint(ScreenFG);

				//	If we're in a HUD menu, run quarter speed

				bool bUIPause = (m_CurrentMenu == menuGame || m_CurrentMenu == menuSelfDestructConfirm);
				bool bSlowMotion = (m_CurrentMenu != menuNone);

#ifdef DEBUG_LINE_OF_FIRE
				if (GetPlayer())
					{
					if (!GetPlayer()->GetShip()->IsLineOfFireClear(GetPlayer()->GetShip()->GetPos(),
							NULL,
							GetPlayer()->GetShip()->GetRotation()))
						g_pUniverse->DebugOutput("line of fire blocked");
					}
#endif
#ifdef DEBUG
				g_pTrans->PaintDebugLines();
#endif
				g_pTrans->m_pTC->PaintDebugInfo(ScreenFG, m_rcScreen);

				//	Paint soundtrack info

				if (m_Settings.GetBoolean(CGameSettings::debugSoundtrack)
						&& !m_bShowingSystemMap)
					PaintSoundtrackTitles(ScreenFG);

                //  Paint the mouse cursor, if necessary

                if (pPlayer 
						&& !InMenu()
						&& IsMouseAimEnabled())
                    {
                    int iMouseAimAngle;
                    int xMouse, yMouse;
                    if (g_pHI->GetMousePos(&xMouse, &yMouse))
                        {
                        int xCenter = m_rcScreen.left + RectWidth(m_rcScreen) / 2;
                        int yCenter = m_rcScreen.top + RectHeight(m_rcScreen) / 2;
                        iMouseAimAngle = ::VectorToPolar(CVector(xMouse - xCenter, yCenter - yMouse));
                        CPaintHelper::PaintArrow(ScreenFG, xMouse, yMouse, iMouseAimAngle, g_pHI->GetVisuals().GetColor(colorTextHighlight));

						if (g_pUniverse->GetDebugOptions().IsShowFacingsAngleEnabled())
							{
							const CG16bitFont &DebugFont = m_HI.GetVisuals().GetFont(fontMedium);
							CString sDebugText = strPatternSubst(CONSTLIT("Aim: %d"), iMouseAimAngle);
							CPaintHelper::PaintArrowText(ScreenFG, xMouse, yMouse, iMouseAimAngle, sDebugText, DebugFont, g_pHI->GetVisuals().GetColor(colorTextHighlight));
							}
                        }
                    else
                        iMouseAimAngle = -1;

                    pPlayer->SetMouseAimAngle(iMouseAimAngle);
                    }

				//	Figure out how long it took to paint

				if (m_Settings.GetBoolean(CGameSettings::debugVideo))
					{
					DWORD dwNow = ::GetTickCount();
					g_pTrans->m_iPaintTime[g_pTrans->m_iFrameCount % FRAME_RATE_COUNT] = dwNow - dwStartTimer;
					dwStartTimer = dwNow;
					}

				//	Some debug information

				if (m_Settings.GetBoolean(CGameSettings::debugVideo))
					g_pTrans->PaintFrameRate();

				//	Update the screen

				if (bTopMost)
					g_pHI->GetScreenMgr().Render();

				//	Figure out how long it took to blt

				if (m_Settings.GetBoolean(CGameSettings::debugVideo))
					{
					DWORD dwNow = ::GetTickCount();
					g_pTrans->m_iBltTime[g_pTrans->m_iFrameCount % FRAME_RATE_COUNT] = dwNow - dwStartTimer;
					dwStartTimer = dwNow;
					}

				//	Figure out the update mode

				CUniverse::EUpdateSpeeds iUpdateMode;
				if (g_pTrans->m_bPaused)
					{
					if (g_pTrans->m_bPausedStep)
						{
						iUpdateMode = CUniverse::updateSingleFrame;
						g_pTrans->m_bPausedStep = false;
						}
					else
						iUpdateMode = CUniverse::updatePaused;
					}
				else if (bUIPause)
					iUpdateMode = CUniverse::updatePaused;
				else if (bSlowMotion)
					iUpdateMode = CUniverse::updateSlowMotion;
				else if (g_pTrans->m_bAutopilot)
					iUpdateMode = CUniverse::updateAccelerated;
				else
					iUpdateMode = CUniverse::updateNormal;

				//	Update the universe

				SetProgramState(psUpdating);
				bool bUpdated = g_pUniverse->Update(UpdateCtx, iUpdateMode);
				SetProgramState(psAnimating);

				if (iUpdateMode != CUniverse::updatePaused)
					{
					if (pPlayer)
						{
						pPlayer->Update(g_pUniverse->GetFrameTicks());

						if (pPlayer->GetSelectedTarget())
							m_HUD.Invalidate(hudTargeting);
						}
					}

				m_MessageDisplay.Update();

				//	Figure out how long it took to update

				if (m_Settings.GetBoolean(CGameSettings::debugVideo))
					{
					DWORD dwNow = ::GetTickCount();
					g_pTrans->m_iUpdateTime[g_pTrans->m_iFrameCount % FRAME_RATE_COUNT] = dwNow - dwStartTimer;
					dwStartTimer = dwNow;
					}

				//	Destroyed?

				if (g_pTrans->m_State == CTranscendenceWnd::gsDestroyed)
					{
					if (!g_pTrans->m_bPaused || g_pTrans->m_bPausedStep)
						{
						if (--g_pTrans->m_iCountdown == 0)
							g_pHI->HICommand(CONSTLIT("gameEndDestroyed"));
						g_pTrans->m_bPausedStep = false;
						}
					}

				break;
				}

			case CTranscendenceWnd::gsDocked:
				{
				//	Paint the screen

				bool bShowSRS = g_pUniverse->GetSFXOptions().IsDockScreenTransparent();

				if (bShowSRS)
					{
					PaintSRS(ScreenFG, ScreenBG);
					ScreenFG.Fill(CG32bitPixel(0, 0, 0, ScreenBG.GetMasterRenderQueue() ? 0 : 0x80));
					}

				g_pTrans->m_pCurrentScreen->Paint(ScreenFG);
				g_pTrans->m_pCurrentScreen->Update();

				if (!bShowSRS)
					g_pTrans->PaintMainScreenBorder(m_CurrentDock.GetDockScreenVisuals().GetWindowBackgroundColor());

                //  Paint displays

                m_HUD.Update(g_pUniverse->GetFrameTicks());
                m_HUD.Paint(ScreenFG, g_pUniverse->GetFrameTicks(), true);

				//	Debug console

				m_DebugConsole.Paint(ScreenFG);

				//	Update the screen

				if (bTopMost)
					g_pHI->GetScreenMgr().Render();

				//	Update the universe (at 1/4 rate)

				g_pUniverse->Update(UpdateCtx, CUniverse::updateSlowMotion);
				m_MessageDisplay.Update();
				m_CurrentDock.Update(g_pUniverse->GetFrameTicks());

				//	Never let message redirection last beyond a frame. We do 
				//	this in case a mod forgets to reset it.

				if (auto pPlayer = m_Model.GetPlayer())
					pPlayer->RedirectDisplayMessage(false);

				//	Note: We need to invalidate the whole screen because we're
				//	flipping between two buffers and we need to make sure both
				//	buffers get painted.

				if (g_pTrans->m_pCurrentScreen)
					g_pTrans->m_pCurrentScreen->Invalidate();

				break;
				}

            case CTranscendenceWnd::gsEnteringStargate:
				{
				//	Tell the universe to paint

				g_pUniverse->PaintPOV(ScreenBG, ScreenFG, m_rcScreen, 0);

                m_HUD.Update(g_pUniverse->GetFrameTicks());
                m_HUD.Paint(ScreenFG, g_pUniverse->GetFrameTicks());

				m_MessageDisplay.Paint(ScreenFG);
				g_pTrans->m_DeviceDisplay.Paint(ScreenFG);

				//	Debug information

				if (m_Settings.GetBoolean(CGameSettings::debugVideo))
					g_pTrans->PaintFrameRate();

#ifdef DEBUG
				g_pTrans->PaintDebugLines();
#endif
				g_pTrans->m_pTC->PaintDebugInfo(ScreenFG, m_rcScreen);

				//	Update the screen

				if (bTopMost)
					g_pHI->GetScreenMgr().Render();

				//	Update the universe

				g_pUniverse->Update(UpdateCtx);
				m_MessageDisplay.Update();

				if (--g_pTrans->m_iCountdown == 0)
					{
					g_pHI->HICommand(CONSTLIT("gameInsideStargate"));
					g_pTrans->m_State = CTranscendenceWnd::gsWaitingForSystem;
					}

				break;
				}

            case CTranscendenceWnd::gsWaitingForSystem:
				{
				if (g_pUniverse->GetSFXOptions().IsStargateTravelEffectEnabled())
					{
					if (g_pTrans->m_pStargateEffect == NULL)
						g_pTrans->m_pStargateEffect = new CStargateEffectPainter;

					g_pTrans->m_pStargateEffect->Paint(ScreenFG, m_rcScreen);
					g_pTrans->m_pStargateEffect->Update();
					}
				else
					{
					ScreenFG.Fill(m_rcScreen.left, m_rcScreen.top, RectWidth(m_rcScreen), RectHeight(m_rcScreen), BAR_COLOR);
					}

				g_pTrans->m_pTC->PaintDebugInfo(ScreenFG, m_rcScreen);

				if (bTopMost)
					g_pHI->GetScreenMgr().Render();
				break;
				}

            case CTranscendenceWnd::gsLeavingStargate:
				{
				//	Tell the universe to paint

				g_pUniverse->PaintPOV(ScreenBG, ScreenFG, m_rcScreen, 0);

                m_HUD.Update(g_pUniverse->GetFrameTicks());
                m_HUD.Paint(ScreenFG, g_pUniverse->GetFrameTicks());

				m_MessageDisplay.Paint(ScreenFG);
				g_pTrans->m_DeviceDisplay.Paint(ScreenFG);

				//	Debug information

				if (m_Settings.GetBoolean(CGameSettings::debugVideo))
					g_pTrans->PaintFrameRate();

#ifdef DEBUG
				g_pTrans->PaintDebugLines();
#endif
				g_pTrans->m_pTC->PaintDebugInfo(ScreenFG, m_rcScreen);

				//	Update the screen

				if (bTopMost)
					g_pHI->GetScreenMgr().Render();

				//	Update the universe

				g_pUniverse->Update(UpdateCtx);
				m_MessageDisplay.Update();

				if (--g_pTrans->m_iCountdown == 0)
					{
					g_pHI->HICommand(CONSTLIT("gameLeaveStargate"));
					g_pTrans->m_State = CTranscendenceWnd::gsInGame;
					ExecuteCommandRefresh();
					}
				break;
				}

            case CTranscendenceWnd::gsEndGame:
				{
				g_pHI->HICommand(CONSTLIT("gameEndGame"));
				break;
				}
			}

		//	Flip

		if (bTopMost)
			g_pHI->GetScreenMgr().Flip();

		SetProgramState(psUnknown);
		}
	catch (CException e)
		{
		::kernelDebugLogPattern("CException: %s", e.GetErrorMessage());
		bFailed = true;
		}
	catch (...)
		{
		bFailed = true;
		}

	//	Deal with errors/crashes

	if (bFailed)
		{
		g_pHI->GetScreenMgr().StopDX();
		g_pTrans->ReportCrash();
		}

	DEBUG_CATCH
    }

void CGameSession::PaintMenu (CG32bitImage &Screen)

//	PaintMenu
//
//	Paints the current menu.

	{
	switch (m_CurrentMenu)
		{
		case menuGame:
		case menuSelfDestructConfirm:
			m_MenuDisplay.Paint(Screen, 0);
			break;

		case menuComms:
		case menuCommsSquadron:
		case menuCommsTarget:
		case menuInvoke:
			g_pTrans->m_MenuDisplay.Paint(Screen);
			break;

		case menuEnableDevice:
		case menuUseItem:
			g_pTrans->m_PickerDisplay.Paint(Screen);
			break;

		case menuSystemStations:
			OnPaintReanimator(Screen);
			break;
		}
	}

void CGameSession::PaintSRS (CG32bitImage &ScreenFG, CG32bitImage &ScreenBG)

//	PaintSRS
//
//	Paints the main screen

	{
	DEBUG_TRY

	//	Figure out some stats

	DWORD dwViewportFlags = 0;
	bool bBlind = false;
	bool bShowMapHUD = false;
	CShip *pShip = NULL;
	if (auto pPlayer = m_Model.GetPlayer())
		{
		pShip = pPlayer->GetShip();
		bBlind = pShip->IsBlind();
		bShowMapHUD = pPlayer->IsMapHUDActive();

		if (pShip->IsSRSEnhanced())
			dwViewportFlags |= CSystem::VWP_ENHANCED_DISPLAY;

		if (pShip->GetAbility(ablMiningComputer) == ablInstalled)
			dwViewportFlags |= CSystem::VWP_MINING_DISPLAY;
		}

	//	If we're showing damage flash, fill the screen

	if (m_iDamageFlash > 0 && (m_iDamageFlash % 2) == 0)
		{
		ScreenBG.Set(CG32bitPixel(128,0,0));
		if (pShip && pShip->GetSystem())
			{
			if (m_bShowingSystemMap)
				g_pUniverse->PaintObjectMap(ScreenBG, g_pTrans->m_rcMainScreen, pShip);
			else
				g_pUniverse->PaintObject(ScreenBG, g_pTrans->m_rcMainScreen, pShip);
			}
		}

	//	Otherwise, if we're in map mode, paint the map

	else if (m_bShowingSystemMap)
        m_SystemMap.Paint(ScreenBG);

	//	Otherwise, if we're blind, paint scramble

	else if (bBlind 
			&& (g_pUniverse->GetFrameTicks() % (20 + (((g_pUniverse->GetFrameTicks() / 100) * pShip->GetDestiny()) % 100))) > 15)
		g_pTrans->PaintSRSSnow();

	//	Otherwise, paint the normal SRS screen

	else
		{
		SetProgramState(psPaintingSRS, g_pUniverse->GetPOV());
		g_pUniverse->PaintPOV(ScreenBG, ScreenFG, m_rcScreen, dwViewportFlags);
		SetProgramState(psAnimating);
		}

	if (m_iDamageFlash > 0)
		m_iDamageFlash--;

	DEBUG_CATCH
	}

