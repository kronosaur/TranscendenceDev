//	PlayerShip.h
//
//	Player ship classes
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

enum class ETargetClass
	{
	enemies,
	friendlies,
	};

enum UIMessageTypes
	{
	uimsgUnknown =					-1,
	uimsgEnabledHints =				-2,

	uimsgAllMessages =				0,
	uimsgAllHints =					1,			//	IsEnabled(uimsgAllHints) returns TRUE if ANY hint is enabled
												//	SetEnabled(uimsgAllHints) enables/disables ALL hints

	uimsgCommsHint =				2,
	uimsgDockHint =					3,
	uimsgMapHint =					4,
	uimsgAutopilotHint =			5,
	uimsgGateHint =					6,
	uimsgUseItemHint =				7,
	uimsgRefuelHint =				8,
	uimsgEnableDeviceHint =			9,
	uimsgSwitchMissileHint =		10,
	uimsgFireMissileHint =			11,
	uimsgGalacticMapHint =			12,
	uimsgMouseManeuverHint =		13,
	uimsgKeyboardManeuverHint =		14,
	uimsgStationDamageHint =		15,
	uimsgMiningDamageTypeHint =		16,
	uimsgFireWeaponHint =			17,
	uimsgShipStatusHint =			18,

	uimsgCount =					19,
	};

class CUIMessageController
	{
	public:
		CUIMessageController (void);

		bool CanShow (CUniverse &Universe, UIMessageTypes iMsg, const CSpaceObject *pMsgObj = NULL) const;
		UIMessageTypes Find (const CString &sMessageName) const;
		bool IsEnabled (UIMessageTypes iMsg) const { return m_Messages[iMsg].bEnabled; }
		void OnHintFollowed (UIMessageTypes iMsg, DWORD dwTick = 0);
		void ReadFromStream (SLoadCtx &Ctx);
		void SetEnabled (UIMessageTypes iMsg, bool bEnabled = true);
		bool ShowMessage (CUniverse &Universe, UIMessageTypes iMsg, const CSpaceObject *pMsgObj = NULL);
		void WriteToStream (IWriteStream *pStream) const;

	private:
		struct SMsgEntry
			{
			bool bEnabled = true;				//	Hint is enabled
			int iHintFollowedCount = 0;			//	How many times the player has followed this hint
			DWORD dwLastShown = 0;				//	Frame Tick on which we last showed
												//		this hint.
			DWORD dwLastObjID = 0;				//	Last obj that we showed hint for.
			};

		bool IsHint (UIMessageTypes iMsg);
		void OnMessageShown (CUniverse &Universe, UIMessageTypes iMsg, const CSpaceObject *pMsgObj = NULL);

		static bool IsTime (DWORD dwCurTick, DWORD dwLastTick, DWORD dwInterval)
			{ return (dwLastTick == 0 || (dwCurTick - dwLastTick) >= dwInterval); }

		SMsgEntry m_Messages[uimsgCount];
	};

class CManeuverController
	{
	public:
		enum ECommands
			{
			cmdNone =				0,

			cmdMouseAim =			1,	//	Turn to m_iAngle (used for mouse aiming)
			cmdMoveTo =				2,	//	Move to m_vPos
			cmdDockWith =			3,	//	Dock with m_pTarget
			};

		CManeuverController (void);

		bool CmdCancel (void);
		bool CmdMouseAim (int iAngle);
		bool CmdMoveTo (const CVector &vPos);
		EManeuverTypes GetManeuver (CShip *pShip) const;
		bool GetThrust (CShip *pShip) const;
		bool IsActive (void) const { return m_iCommand != cmdNone; }
		bool IsManeuverActive (void) const { return m_iCommand != cmdNone; }
		bool IsThrustActive (void) const;
		void ReadFromStream (SLoadCtx &Ctx);
		void Update (SUpdateCtx &Ctx, CShip *pShip);
		void WriteToStream (IWriteStream &Stream, CSystem *pSystem);

	private:
		void UpdateMoveTo (SUpdateCtx &Ctx, CShip *pShip);

		ECommands m_iCommand;
		int m_iAngle;
		CVector m_vPos;
		CSpaceObject *m_pTarget;

		//	Temporary variables during update (no need to save)

		EManeuverTypes m_iManeuver;
		bool m_bThrust;
	};

class CPlayerShipController : public IShipController
	{
	public:
		CPlayerShipController (CUniverse &Universe);
		~CPlayerShipController (void);

		CurrencyValue Charge (DWORD dwEconUNID, CurrencyValue iCredits) { return m_Credits.IncCredits(dwEconUNID, -iCredits); }

		bool CanShowShipStatus (void);
		void Communications (CSpaceObject *pObj, MessageTypes iMsg, DWORD dwData = 0, DWORD *iodwFormationPlace = NULL);
		void CycleTarget (int iDir = 1);
		void DisplayCommandHint (CGameKeys::Keys iCmd, const CString &sMessage);
		void DisplayMessage (const CString &sMessage);
		void Dock (void);
		bool DockingInProgress (void) { return m_pStation != NULL; }
		UIMessageTypes FindUIMessage (const CString &sName) { return m_UIMsgs.Find(sName); }
		void Gate (void);
		void GenerateGameStats (CGameStats &Stats, bool bGameOver);
		int GetBestEnemyShipsDestroyed (DWORD *retdwUNID = NULL) { return m_Stats.GetBestEnemyShipsDestroyed(retdwUNID); }
		CurrencyValue GetCredits (DWORD dwEconUNID) { return m_Credits.GetCredits(dwEconUNID); }
		int GetCargoSpace (void) { return mathRound(m_pShip->GetCargoSpaceLeft()); }
		int GetEndGameScore (void) { return m_Stats.CalcEndGameScore(); }
		int GetEnemiesDestroyed (void) { return ::strToInt(m_Stats.GetStatString(CONSTLIT("enemyShipsDestroyed")), 0); }
		CGameSession *GetGameSession (void) { return m_pSession; }
		const CPlayerGameStats &GetGameStats (void) const { return m_Stats; }
		CPlayerGameStats &GetGameStats (void) { return m_Stats; }
		CString GetItemStat (const CString &sStat, ICCItem *pItemCriteria) const { return m_Stats.GetItemStat(sStat, pItemCriteria); }
		CString GetKeyEventStat (const CString &sStat, const CString &sNodeID, const CDesignTypeCriteria &Crit) const { return m_Stats.GetKeyEventStat(sStat, sNodeID, Crit); }
		GenomeTypes GetPlayerGenome (void) const { return m_iGenome; }
		CString GetPlayerName (void) const { return m_sName; }
		const CString &GetRedirectMessage (void) const { return m_sRedirectMessage; }
		int GetResurrectCount (void) const { return ::strToInt(m_Stats.GetStatString(CONSTLIT("resurrectCount")), 0); }
		int GetScore (void) { return ::strToInt(m_Stats.GetStatString(CONSTLIT("score")), 0); }
		CSpaceObject *GetSelectedTarget (void) { return m_pTarget; }
		CShip *GetShip (void) { return m_pShip; }
		DWORD GetStartingShipClass (void) const { return m_dwStartingShipClass; }
		DWORD GetSystemEnteredTime (const CString &sNodeID) { return m_Stats.GetSystemEnteredTime(sNodeID); }
		int GetSystemsVisited (void) { return ::strToInt(m_Stats.GetStatString(CONSTLIT("systemsVisited")), 0); }
		CTranscendenceWnd *GetTrans (void) { return m_pTrans; }
		void IncScore (int iBonus) { m_Stats.IncStat(CONSTLIT("score"), iBonus); }
		void Init (CTranscendenceWnd *pTrans);
		void InsuranceClaim (void);
		bool IsGalacticMapAvailable (void) { return (m_pShip && (m_pShip->GetAbility(::ablGalacticMap) > ::ablUninstalled)); }
		bool IsMapHUDActive (void) { return m_bMapHUD; }
		bool IsUIMessageEnabled (UIMessageTypes iMsg) { return m_UIMsgs.IsEnabled(iMsg); }
		void OnEnemyShipsDetected (void);
		void OnGameEnd (void) { m_Stats.OnGameEnd(m_pShip); }
		void OnMouseAimSetting (bool bEnabled) { if (!bEnabled) m_ManeuverController.CmdCancel(); }
		void OnStartGame (void);
		void OnSystemEntered (CSystem *pSystem, int *retiLastVisit = NULL) { m_Stats.OnSystemEntered(pSystem, retiLastVisit); }
		void OnSystemLeft (CSystem *pSystem) { m_Stats.OnSystemLeft(pSystem); }
		CurrencyValue Payment (DWORD dwEconUNID, CurrencyValue iCredits) { return m_Credits.IncCredits(dwEconUNID, iCredits); }
		void ReadyNextWeapon (int iDir = 1);
		void ReadyNextMissile (int iDir = 1);
		void RedirectDisplayMessage (bool bRedirect);
		void SetCharacterClass (CGenericType *pClass) { m_pCharacterClass = pClass; }
		void SetGameSession (CGameSession *pSession);
		void SetGenome (GenomeTypes iGenome) { m_iGenome = iGenome; }
		void SetMapHUD (bool bActive) { m_bMapHUD = bActive; }
		void SetMouseAimAngle (int iAngle) { m_ManeuverController.CmdMouseAim(iAngle); }
		void SetName (const CString &sName) { m_sName = sName; }
		void SetResurrectCount (int iCount) { m_Stats.SetStat(CONSTLIT("resurrectCount"), ::strFromInt(iCount)); }
		void SetStartingShipClass (DWORD dwUNID) { m_dwStartingShipClass = dwUNID; }
		void SetStartingSystem (const CString &sNode) { m_sStartingSystem = sNode; }
		void SetTarget (CSpaceObject *pTarget);
		void SelectNearestTarget (void);
		void SelectNextFriendly (int iDir = 1);
		void SelectNextTarget (int iDir = 1);
		void SetActivate (bool bActivate) { m_bActivate = bActivate; }
		void SetFireMain (bool bFire);
		void SetFireMissile (bool bFire);
		void SetShip (CShip *pShip) { m_pShip = pShip; }
		void SetStopThrust (bool bStop) { m_bStopThrust = bStop; }
		void SetUIMessageEnabled (UIMessageTypes iMsg, bool bEnabled = true) { m_UIMsgs.SetEnabled(iMsg, bEnabled); }
		void SetUIMessageFollowed (UIMessageTypes iMsg) { m_UIMsgs.OnHintFollowed(iMsg, m_Universe.GetFrameTicks()); }
		ALERROR SwitchShips (CShip *pNewShip, SPlayerChangedShipsCtx &Options);
		void Undock (void);
		void Update (int iTick);

		//	Device methods
		bool AreAllDevicesEnabled (void);
		void EnableAllDevices (bool bEnable = true);
		bool ToggleEnableDevice (int iDeviceIndex);

		//	Fleet formation methods
		DWORD GetCommsStatus (void);
		bool HasFleet (void);

		//	IShipController virtuals

		virtual void AddOrder (OrderTypes Order, CSpaceObject *pTarget, const IShipController::SData &Data, bool bAddBefore = false) override;
		virtual void Behavior (SUpdateCtx &Ctx) override;
		virtual void CancelAllOrders (void) override;
		virtual void CancelCurrentOrder (void) override;
		virtual void CancelDocking (void) override;
		virtual bool CancelOrder (int iIndex) override;
		virtual CString DebugCrashInfo (void) override;
		virtual void DebugPaintInfo (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual ICCItem *FindProperty (const CString &sProperty) override;
		virtual CString GetAISettingString (const CString &sSetting) override;
		virtual CString GetClass (void) override { return CONSTLIT("player"); }
		virtual int GetCombatPower (void) override;
		virtual const CCurrencyBlock *GetCurrencyBlock (void) const override { return &m_Credits; }
		virtual CCurrencyBlock *GetCurrencyBlock (void) override { return &m_Credits; }
		virtual OrderTypes GetCurrentOrderEx (CSpaceObject **retpTarget = NULL, IShipController::SData *retData = NULL) override;
		virtual CSpaceObject *GetDestination (void) const override { return m_pDestination; }
		virtual bool GetDeviceActivate (void) override;
		virtual int GetFireDelay (void) override { return mathRound(5.0 / STD_SECONDS_PER_UPDATE); }
		virtual EManeuverTypes GetManeuver (void) override;
		virtual OrderTypes GetOrder (int iIndex, CSpaceObject **retpTarget = NULL, IShipController::SData *retData = NULL) const override;
		virtual int GetOrderCount (void) const override { return (m_iOrder == IShipController::orderNone ? 0 : 1); }
		virtual CSpaceObject *GetOrderGiver (void) override { return m_pShip; }
		virtual bool GetReverseThrust (void) override;
		virtual bool GetStopThrust (void) override;
		virtual bool GetThrust (void) override;
		virtual CSpaceObject *GetTarget (const CDeviceItem *pDeviceItem = NULL, DWORD dwFlags = 0) const override;
		virtual CTargetList GetTargetList (void) const override;
		virtual bool IsAngryAt (const CSpaceObject *pObj) const override;
		virtual bool IsPlayer (void) const override { return true; }
		virtual void ReadFromStream (SLoadCtx &Ctx, CShip *pShip) override;
		virtual void SetManeuver (EManeuverTypes iManeuver) override { m_iManeuver = iManeuver; }
		virtual ESetPropertyResult SetProperty (const CString &sProperty, const ICCItem &Value, CString *retsError = NULL) override;
		virtual void SetThrust (bool bThrust) override { m_bThrust = bThrust; }
		virtual void WriteToStream (IWriteStream *pStream) override;

		//	Events

		virtual void OnAbilityChanged (Abilities iAbility, AbilityModifications iChange, bool bNoMessage = false) override;
		virtual void OnAcceptedMission (CMission &MissionObj) override;
		virtual DWORD OnCommunicate (CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2, ICCItem *pData) override;
		virtual void OnComponentChanged (ObjectComponentTypes iComponent) override;
		virtual void OnDamaged (const CDamageSource &Cause, CInstalledArmor *pArmor, const DamageDesc &Damage, int iDamage) override;
		virtual bool OnDestroyCheck (DestructionTypes iCause, const CDamageSource &Attacker) override;
		virtual void OnDestroyed (SDestroyCtx &Ctx) override;
		virtual void OnDeviceEnabledDisabled (int iDev, bool bEnable, bool bSilent = false) override;
		virtual void OnDeviceStatus (CInstalledDevice *pDev, CDeviceClass::DeviceNotificationTypes iEvent) override;
		virtual void OnDocked (CSpaceObject *pObj) override;
		virtual void OnEnterGate (CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate, bool bAscend) override;
		virtual void OnFuelConsumed (Metric rFuel, CReactorDesc::EFuelUseTypes iUse) override;
		virtual void OnItemBought (const CItem &Item, CurrencyValue iTotalPrice) override { m_Stats.OnItemBought(Item, iTotalPrice); }
		virtual void OnItemDamaged (const CItem &Item, int iHP) override { m_Stats.OnItemDamaged(Item, iHP); }
		virtual void OnItemFired (const CItem &Item) override { m_Stats.OnItemFired(Item); }
		virtual void OnItemInstalled (const CItem &Item) override { m_Stats.OnItemInstalled(Item); }
		virtual void OnItemSold (const CItem &Item, CurrencyValue iTotalPrice) override { m_Stats.OnItemSold(Item, iTotalPrice); }
		virtual void OnItemUninstalled (const CItem &Item) override { m_Stats.OnItemUninstalled(Item); }
		virtual void OnMissionCompleted (CMission *pMission, bool bSuccess) override;
		virtual void OnNewSystem (CSystem *pSystem) override;
		virtual void OnObjHit (const SDamageCtx &Ctx) override;
		virtual void OnObjDestroyed (const SDestroyCtx &Ctx) override;
		virtual void OnOverlayConditionChanged (ECondition iCondition, EConditionChange iChange) override;
		virtual void OnPaintSRSEnhancements (CG32bitImage &Dest, SViewportPaintCtx &Ctx) override;
		virtual void OnProgramDamage (CSpaceObject *pHacker, const ProgramDesc &Program) override;
		virtual void OnShipStatus (EShipStatusNotifications iEvent, DWORD dwData = 0) override;
		virtual void OnStationDestroyed (const SDestroyCtx &Ctx) override;
		virtual void OnUpdatePlayer (SUpdateCtx &Ctx) override;
		virtual void OnWeaponStatusChanged (void) override;
		virtual void OnWreckCreated (CSpaceObject *pWreck) override;

	private:

		void ClearFireAngle (void);
		void DisplayTranslate (const CString &sID, ICCItem *pData = NULL);
		void DisplayTranslate (const CString &sID, const CString &sVar, const CString &sValue);
		CSpaceObject *FindDockTarget (void);
		bool HasCommsTarget (void);
		void InitTargetList (ETargetClass iTargetType, bool bUpdate = false);
		void PaintDebugLineOfFire (SViewportPaintCtx &Ctx, CG32bitImage &Dest) const;
		void PaintDebugLineOfFire (SViewportPaintCtx &Ctx, CG32bitImage &Dest, CSpaceObject &TargetObj) const;
		void PaintDebugLineOfFire (SViewportPaintCtx &Ctx, CG32bitImage &Dest, CSpaceObject &TargetObj, CInstalledDevice &Weapon) const;
		void PaintDockingPortIndicators (SViewportPaintCtx &Ctx, CG32bitImage &Dest) const;
		void PaintTargetingReticle (SViewportPaintCtx &Ctx, CG32bitImage &Dest, CSpaceObject *pTarget);
		void Reset (void);
		CString Translate (const CString &sID, ICCItem *pData = NULL) const { return m_Universe.TranslateEngineText(sID, pData); }

		static constexpr DWORD OPTION_HIGHLIGHT = 0x00000001;
		void SetDestination (CSpaceObject *pTarget, DWORD dwOptions = 0);

		void UpdateHelp (int iTick);

		CUniverse &m_Universe;
		CTranscendenceWnd *m_pTrans = NULL;
		CGameSession *m_pSession = NULL;            //  Game session
		CShip *m_pShip = NULL;

		OrderTypes m_iOrder = orderNone;			//	Last order
		CSpaceObject *m_pTarget = NULL;
		CSpaceObject *m_pDestination = NULL;
		TSortMap<CString, CSpaceObject *> m_TargetList;

		CSpaceObject *m_pStation = NULL;			//	Station that player is docked with
		bool m_bSignalDock = false;					//	Tell the model to switch to dock screen

		DWORD m_dwWreckObjID = OBJID_NULL;			//	WreckObjID (temp while we resurrect)

		int m_iLastHelpTick = 0;

		CManeuverController m_ManeuverController;
		EManeuverTypes m_iManeuver = NoRotation;
		bool m_bThrust = false;
		bool m_bActivate = false;
		bool m_bStopThrust = false;

		bool m_bMapHUD = true;						//	Show HUD on map
		bool m_bDockPortIndicators = true;			//	Dock ports light up when near by

		int m_iMouseAimAngle = -1;					//  Angle to aim towards

		CCurrencyBlock m_Credits;					//	Money available to player
		CPlayerGameStats m_Stats;					//	Player stats, including score
		CUIMessageController m_UIMsgs;				//	Status of various UI messages, such as hints

		CString m_sName;							//	Player name
		GenomeTypes m_iGenome = genomeUnknown;		//	Player genome
		DWORD m_dwStartingShipClass = 0;			//	Starting ship class
		CString m_sStartingSystem;					//	Starting system
		CGenericType *m_pCharacterClass = NULL;		//	Character class

		bool m_bUnderAttack = false;				//	TRUE if we're currently under attack

		CSpaceObject *m_pAutoDock = NULL;			//	The current station to dock with if we were to 
													//		press 'D' right now. NULL means no station
													//		to dock with.
		int m_iAutoDockPort = 0;					//	The current dock port.
		CVector m_vAutoDockPort;					//	The current dock port position;

		bool m_bShowAutoTarget = false;				//	If TRUE, we show the autotarget
		bool m_bTargetOutOfRange = false;			//	If TRUE, m_pTarget is out of weapon range
		CSpaceObject *m_pAutoTarget = NULL;			//	Saved autotarget.
		const CSpaceObject *m_pAutoMining = NULL;	//	Saved auto mining target
		mutable int m_iAutoTargetTick = 0;

		CSpaceObject *m_pAutoDamage = NULL;			//	Show damage bar for this object
		DWORD m_dwAutoDamageExpire = 0;				//	Stop showing on this tick

		bool m_bRedirectMessages = false;			//	If TRUE, redirect display messages
		CString m_sRedirectMessage;
		TArray<CString> m_SavedMessages;			//	Messages saved while game session not set up

		CNavigationPath *m_pDebugNavPath = NULL;

		static TPropertyHandler<CPlayerShipController> m_PropertyTable;

	friend CObjectClass<CPlayerShipController>;
	};

