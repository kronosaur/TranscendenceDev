//	GameSession.h
//
//	Transcendence session classes
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

#include "GameSessionDisplays.h"

class CGameSession : public IHISession
	{
	public:
		enum EMenuTypes
			{
			menuNone,

			menuComms,						//	Choose message to send
			menuCommsSquadron,				//	Choose message for squadron
			menuCommsTarget,				//	Choose target for comms
			menuDebugConsole,				//	Debug console
			menuEnableDevice,				//	Choose device to enable/disable
			menuGame,						//	Game menu
			menuInvoke,						//	Invoke power
			menuSelfDestructConfirm,		//	Confirm self-destruct
			menuSystemStations,				//	List of stations in system
			menuUseItem,					//	Choose item to use
			};

		CGameSession (STranscendenceSessionCtx &CreateCtx);

		void ExecuteCommand (CPlayerShipController *pPlayer, CGameKeys::Keys iCommand);
		void ExecuteCommandEnd (CPlayerShipController *pPlayer, CGameKeys::Keys iCommand);
		void ShowStationList (bool bShow = true);
		void ShowSystemMap (bool bShow = true);

		//  Notifications from player ship, etc.

		void OnAcceptedMission (CMission &MissionObj);
		void OnArmorDamaged (int iSegment) { m_HUD.Invalidate(hudArmor); }
		void OnArmorRepaired (int iSegment) { m_HUD.Invalidate(hudArmor); }
		void OnArmorSelected (int iSelection) { m_HUD.SetArmorSelection(iSelection); }
		void OnDamageFlash (void) { m_iDamageFlash = Min(2, m_iDamageFlash + 2); }
		void OnExecuteActionDone (void) { m_Model.OnExecuteActionDone(); }
		void OnKeyboardMappingChanged (void);
		void OnObjDestroyed (const SDestroyCtx &Ctx);
		void OnPlayerChangedShips (CSpaceObject *pOldShip) { InitUI(); m_HUD.Init(m_rcScreen); g_pTrans->InitDisplays(); }
		void OnPlayerDestroyed (SDestroyCtx &Ctx, const CString &sEpitaph);
		void OnPlayerEnteredStargate (CTopologyNode *pNode);
		void OnShowDockScreen (bool bShow);
		void OnTargetChanged (CSpaceObject *pTarget) { m_HUD.Invalidate(hudTargeting); }
		void OnWeaponStatusChanged (void) { m_HUD.Invalidate(hudTargeting); }

		//	IHISession virtuals

		virtual CReanimator &GetReanimator (void) override { return g_pTrans->GetReanimator(); }
		virtual void OnActivate (void) override;
		virtual void OnAnimate (CG32bitImage &Screen, bool bTopMost) override;
		virtual void OnChar (char chChar, DWORD dwKeyData) override;
		virtual void OnCleanUp (void);
		virtual ALERROR OnCommand (const CString &sCmd, void *pData = NULL) override;
		virtual ALERROR OnInit (CString *retsError) override;
		virtual void OnKeyDown (int iVirtKey, DWORD dwKeyData) override;
		virtual void OnKeyUp (int iVirtKey, DWORD dwKeyData) override;
		virtual void OnLButtonDblClick (int x, int y, DWORD dwFlags) override;
		virtual void OnLButtonDown (int x, int y, DWORD dwFlags, bool *retbCapture) override;
		virtual void OnLButtonUp (int x, int y, DWORD dwFlags) override;
		virtual void OnMButtonDown (int x, int y, DWORD dwFlags, bool *retbCapture) override;
		virtual void OnMButtonUp (int x, int y, DWORD dwFlags) override;
		virtual void OnMouseMove (int x, int y, DWORD dwFlags) override;
		virtual void OnMouseWheel (int iDelta, int x, int y, DWORD dwFlags) override;
		virtual void OnMove (int x, int y) override { g_pTrans->WMMove(x, y); }
		virtual void OnRButtonDblClick (int x, int y, DWORD dwFlags) override;
		virtual void OnRButtonDown (int x, int y, DWORD dwFlags) override;
		virtual void OnRButtonUp (int x, int y, DWORD dwFlags) override;
		virtual void OnReportHardCrash (CString *retsMessage) override { *retsMessage = g_pTrans->GetCrashInfo(); }
		virtual void OnSize (int cxWidth, int cyHeight) override { g_pTrans->WMSize(cxWidth, cyHeight, 0); }

		//	Helpers

		void DisplayMessage (const CString &sMessage) { m_MessageDisplay.DisplayMessage(sMessage); }
		CDockScreen &GetDockScreen (void) { return m_CurrentDock; }
		CGalacticMapSession::SOptions &GetGalacticMapSettings (void) { return m_GalacticMapSettings; }
		CGameSettings &GetGameSettings (void) { return m_Settings; }
		CMessageDisplay &GetMessageDisplay (void) { return m_MessageDisplay; }
		IPlayerController::EUIMode GetUIMode (void) const;
		CUniverse &GetUniverse (void) const { return m_Model.GetUniverse(); }
		bool InMenu (void) { return (m_CurrentMenu != menuNone); }
		bool InSystemMap (void) const { return m_bShowingSystemMap; }
		bool IsMouseAimEnabled (void) const { return m_bMouseAim; }
		void PaintInfoText (CG32bitImage &Dest, const CString &sTitle, const TArray<CString> &Body, bool bAboveTargeting = true);
		void PaintSoundtrackTitles (CG32bitImage &Dest);

	private:

		CMenuData CreateGameMenu (void) const;
		CMenuData CreateSelfDestructMenu (void) const;
		void DismissMenu (void);
		void ExecuteCommandRefresh (void);
		void ExecuteCommandRefresh (CPlayerShipController &Player);
		void HideMenu (void);
		void InitUI (void);
		bool IsIconBarShown (void) const;
		bool IsInPickerCompatible (void) const
			{
			return (m_CurrentMenu == menuEnableDevice || m_CurrentMenu == menuUseItem);
			}

		bool IsInMenu (void) const
			{
			return (m_CurrentMenu == menuGame || m_CurrentMenu == menuSelfDestructConfirm);
			}

		bool IsInMenuCompatible (void) const 
			{
			return (m_CurrentMenu == menuComms 
					|| m_CurrentMenu == menuCommsSquadron 
					|| m_CurrentMenu == menuCommsTarget 
					|| m_CurrentMenu == menuInvoke);
			}

		bool IsMouseAimConfigured (void) const;

		void PaintMenu (CG32bitImage &Screen);
		void PaintSRS (CG32bitImage &Screen);
		void SetMouseAimEnabled (bool bEnabled = true);
		bool ShowMenu (EMenuTypes iMenu);
		void SyncMouseToPlayerShip (void);

		CGameSettings &m_Settings;
		CTranscendenceModel &m_Model;
		CCommandLineDisplay &m_DebugConsole;
		CSoundtrackManager &m_Soundtrack;
		RECT m_rcScreen = { 0 };				//	RECT of main screen within window.

		//	UI state

		EUITypes m_iUI = uiNone;				//	Basic flying UI
		bool m_bMouseAim = true;				//	Player aims with the mouse

		EMenuTypes m_CurrentMenu = menuNone;	//	Current menu being displayed
		CMenuDisplay m_MenuDisplay;				//	Menu
		CSpaceObject *m_pCurrentComms = NULL;	//	Object that we're currently communicating with

		CIconBarDisplay m_IconBar;				//	Icons to access various screens
		CGameIconBarData m_IconBarData;			//	Data for icon bar

		//	HUD state

		CHeadsUpDisplay m_HUD;					//  Paint the HUD
		int m_iDamageFlash = 0;					//	0 = no flash; odd = recover; even = flash;
		bool m_bIgnoreButtonUp = false;			//	If we closed a menu on button down, ignore button up
		bool m_bIgnoreMouseMove = false;		//	Ignore mouse move, for purposes of enabling mouse flight

		//	Map state

		bool m_bShowingSystemMap = false;		//  If TRUE, show map
		CSystemMapDisplay m_SystemMap;			//  Helps to paint the system map
		CSystemStationsMenu m_SystemStationsMenu;
		CGalacticMapSession::SOptions m_GalacticMapSettings;

		//	Other displays

		CMessageDisplay m_MessageDisplay;
		CNarrativeDisplay m_Narrative;

		//	Dock screen state

		CDockScreen m_CurrentDock;				//	Current dock screen
	};
