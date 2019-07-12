//	ShipAIImpl.h
//
//	Transcendence IShipController classes

#ifndef INCL_TSE_SHIP_CONTROLLERS
#define INCL_TSE_SHIP_CONTROLLERS

class CBaseShipAI;

class CAIShipControls
	{
	public:
		enum Constants
			{
			constAlwaysThrust = -1,
			constNeverThrust = 1000,
			};

		CAIShipControls (void);

		EManeuverTypes GetManeuver (void) const { return m_iManeuver; }
		inline bool GetThrust (CShip *pShip) const;
		int GetThrustDir (void) const { return m_iThrustDir; }
		void ReadFromStream (SLoadCtx &Ctx);
		void SetManeuver (EManeuverTypes iManeuver) { m_iManeuver = iManeuver; }
		void SetThrust (bool bThrust) { m_iThrustDir = (bThrust ? constAlwaysThrust : constNeverThrust); }
		void SetThrustDir (int iDir) { m_iThrustDir = iDir; }
		void WriteToStream (CSystem *pSystem, IWriteStream *pStream);

	private:
		EManeuverTypes m_iManeuver;	//	Current maneuver (turn)
		int m_iThrustDir;						//	Thrust if facing given direction
	};

class CAIBehaviorCtx
	{
	public:
		CAIBehaviorCtx (void);
		~CAIBehaviorCtx (void);

		bool AscendOnGate (void) const { return m_AISettings.AscendOnGate(); }
		bool AvoidsExplodingStations (void) const { return m_fAvoidExplodingStations; }
		void ClearBestWeapon (void) { m_fRecalcBestWeapon = true; }
		void ClearNavPath (void);
		void DebugPaintInfo (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);
		CString GetAISetting (const CString &sSetting) { return m_AISettings.GetValue(sSetting); }
		const CAISettings &GetAISettings (void) const { return m_AISettings; }
		CInstalledDevice *GetBestWeapon (void) const { return (m_iBestWeapon != devNone ? m_pBestWeapon : NULL); }
		DeviceNames GetBestWeaponIndex (void) const { return m_iBestWeapon; }
		Metric GetBestWeaponRange (void) const { return m_rBestWeaponRange; }
		Metric GetBestWeaponRange2 (void) const { return m_rBestWeaponRange * m_rBestWeaponRange; }
		AICombatStyles GetCombatStyle (void) const { return m_AISettings.GetCombatStyle(); }
		int GetFireAccuracy (void) const { return m_AISettings.GetFireAccuracy(); }
		int GetFireRangeAdj (void) const { return m_AISettings.GetFireRangeAdj(); }
		int GetFireRateAdj (void) const { return m_AISettings.GetFireRateAdj(); }
		Metric GetFlankDist (void) const { return m_rFlankDist; }
		Metric GetFlankRange2 (void) const { return 1.5 * m_rFlankDist * m_rFlankDist; }
		int GetLastAttack (void) const { return m_iLastAttack; }
		EManeuverTypes GetLastTurn (void) const { return m_iLastTurn; }
		int GetLastTurnCount (void) const { return m_iLastTurnCount; }
		EManeuverTypes GetManeuver (void) const { return m_ShipControls.GetManeuver(); }
		int GetMaxTurnCount (void) const { return m_iMaxTurnCount; }
		Metric GetMaxWeaponRange (void) const { return m_rMaxWeaponRange; }
		Metric GetMinCombatSeparation (void) const { return m_AISettings.GetMinCombatSeparation(); }
		CNavigationPath *GetNavPath (void) const { return m_pNavPath; }
		int GetPerception (void) const { return m_AISettings.GetPerception(); }
		const CVector &GetPotential (void) const { return m_vPotential; }
		Metric GetPrimaryAimRange2 (void) const { return m_rPrimaryAimRange2; }
		SUpdateCtx *GetSystemUpdateCtx (void) const { return m_pUpdateCtx;  }
		bool GetThrust (CShip *pShip) const { return m_ShipControls.GetThrust(pShip); }
		int GetThrustDir (void) const { return m_ShipControls.GetThrustDir(); }
		bool HasEscorts (void) const { return m_fHasEscorts; }
		bool HasMultipleWeapons (void) const { return m_fHasMultipleWeapons; }
		bool HasSecondaryWeapons (void) const { return m_fHasSecondaryWeapons; }
		bool HasSuperconductingShields (void) const { return m_fSuperconductingShields; }
		bool IsAggressor (void) const { return m_AISettings.IsAggressor(); }
		bool IsBeingAttacked (DWORD dwThreshold = 150) const;
		bool IsDockingRequested (void) const { return m_fDockingRequested; }
		bool IsImmobile (void) const { return m_fImmobile; }
		bool IsNonCombatant (void) const { return m_AISettings.IsNonCombatant(); }
		bool IsSecondAttack (void) const;
		bool IsWaitingForShieldsToRegen (void) const { return m_fWaitForShieldsToRegen; }
		bool NoAttackOnThreat (void) const { return m_AISettings.NoAttackOnThreat(); }
		bool NoDogfights (void) const { return m_AISettings.NoDogfights(); }
		bool NoFriendlyFire (void) const { return m_AISettings.NoFriendlyFire(); }
		bool NoFriendlyFireCheck (void) const { return m_AISettings.NoFriendlyFireCheck(); }
		bool NoNavPaths (void) const { return m_AISettings.NoNavPaths(); }
		bool NoOrderGiver (void) const { return m_AISettings.NoOrderGiver(); }
		bool NoShieldRetreat (void) const { return m_AISettings.NoShieldRetreat(); }
		void ReadFromStream (SLoadCtx &Ctx);
		void ReadFromStreamAISettings (SLoadCtx &Ctx) { m_AISettings.ReadFromStream(Ctx); }
		CString SetAISetting (const CString &sSetting, const CString &sValue) { return m_AISettings.SetValue(sSetting, sValue); }
		void SetAISettings (const CAISettings &Source) { m_AISettings = Source; }
		void SetBarrierClock (CShip *pShip);
		void SetDockingRequested (bool bValue = true) { m_fDockingRequested = bValue; }
		void SetHasEscorts (bool bValue = true) { m_fHasEscorts = bValue; }
		void SetLastAttack (int iTick);
		void SetLastTurn (EManeuverTypes iTurn) { m_iLastTurn = iTurn; }
		void SetLastTurnCount (int iCount) { m_iLastTurnCount = iCount; }
		void SetManeuver (EManeuverTypes iManeuver) { m_ShipControls.SetManeuver(iManeuver); }
		void SetManeuverCounter (int iCount) { m_iManeuverCounter = iCount; }
		void SetNavPath (CNavigationPath *pNavPath, int iNavPathPos, bool bOwned = false) { ClearNavPath(); m_pNavPath = pNavPath; m_iNavPathPos = iNavPathPos; m_fFreeNavPath = bOwned; }
		void SetPotential (const CVector &vVec) { m_vPotential = vVec; }
		void SetSystemUpdateCtx (SUpdateCtx *pCtx) { m_pUpdateCtx = pCtx;  }
		void SetThrust (bool bThrust) { m_ShipControls.SetThrust(bThrust); }
		void SetThrustDir (int iDir) { m_ShipControls.SetThrustDir(iDir); }
		void SetWaitingForShieldsToRegen (bool bValue = true) { m_fWaitForShieldsToRegen = bValue; }
		bool ThrustsThroughTurn (void) const { return m_fThrustThroughTurn; }
		void Update (CShip *pShip);
		void WriteToStream (CSystem *pSystem, IWriteStream *pStream);

		//	Maneuvers
		CVector CalcManeuverCloseOnTarget (CShip *pShip, CSpaceObject *pTarget, const CVector &vTarget, Metric rTargetDist2, bool bFlank = false);
		CVector CalcManeuverFormation (CShip *pShip, const CVector vDest, const CVector vDestVel, int iDestFacing);
		CVector CalcManeuverSpiralIn (CShip *pShip, const CVector &vTarget, int iTrajectory = 30);
		CVector CalcManeuverSpiralOut (CShip *pShip, const CVector &vTarget, int iTrajectory = 30);
		void ImplementAttackNearestTarget (CShip *pShip, Metric rMaxRange, CSpaceObject **iopTarget, CSpaceObject *pExcludeObj = NULL, bool bTurn = false);
		void ImplementAttackTarget (CShip *pShip, CSpaceObject *pTarget, bool bMaintainCourse = false, bool bDoNotShoot = false);
		void ImplementCloseOnImmobileTarget (CShip *pShip, CSpaceObject *pTarget, const CVector &vTarget, Metric rTargetDist2, Metric rTargetSpeed = 0.0);
		void ImplementCloseOnTarget (CShip *pShip, CSpaceObject *pTarget, const CVector &vTarget, Metric rTargetDist2, bool bFlank = false);
		void ImplementDocking (CShip *pShip, CSpaceObject *pTarget);
		void ImplementEscort (CShip *pShip, CSpaceObject *pBase, CSpaceObject **iopTarget);
		void ImplementEvasiveManeuvers (CShip *pShip, CSpaceObject *pTarget);
		void ImplementFireOnTarget (CShip *pShip, CSpaceObject *pTarget, bool *retbOutOfRange = NULL);
		void ImplementFireOnTargetsOfOpportunity (CShip *pShip, CSpaceObject *pTarget = NULL, CSpaceObject *pExcludeObj = NULL);
		void ImplementFireWeapon (CShip *pShip, DeviceNames iDev = devNone);
		void ImplementFireWeaponOnTarget (CShip *pShip, int iWeapon, int iWeaponVariant, CSpaceObject *pTarget, const CVector &vTarget, Metric rTargetDist2, int *retiFireDir = NULL, bool bDoNotShoot = false);
		void ImplementFollowNavPath (CShip *pShip, bool *retbAtDestination = NULL);
		void ImplementFormationManeuver (CShip *pShip, const CVector vDest, const CVector vDestVel, int iDestFacing, bool *retbInFormation = NULL);
		void ImplementGating (CShip *pShip, CSpaceObject *pTarget);
		void ImplementHeadingManeuver (CShip *pShip, const CVector &vTarget);
		void ImplementHold (CShip *pShip, bool *retbInPlace = NULL);
		void ImplementOrbit (CShip *pShip, CSpaceObject *pBase, Metric rDistance);
		void ImplementManeuver (CShip *pShip, int iDir, bool bThrust, bool bNoThrustThroughTurn = false);
		void ImplementSpiralIn (CShip *pShip, const CVector &vTarget);
		void ImplementSpiralOut (CShip *pShip, const CVector &vTarget, int iTrajectory = 30);
		void ImplementStop (CShip *pShip);
		void ImplementTurnTo (CShip *pShip, int iRotation);

		//	Helpers
		CVector CalcFlankPos (CShip *pShip, const CVector &vInterceptPos);
		bool CalcFormationParams (CShip *pShip, const CVector &vDestPos, const CVector &vDestVel, int iDestAngle, CVector *retvRecommendedVel, Metric *retrDeltaPos2 = NULL, Metric *retrDeltaVel2 = NULL);
		void CalcAvoidPotential (CShip *pShip, CSpaceObject *pTarget);
		void CalcBestWeapon (CShip *pShip, CSpaceObject *pTarget, Metric rTargetDist2);
		bool CalcFlockingFormation (CShip *pShip, CSpaceObject *pLeader, CVector *retvPos, CVector *retvVel, int *retiFacing);
		void CalcInvariants (CShip *pShip);
		bool CalcIsBetterTarget (CShip *pShip, CSpaceObject *pCurTarget, CSpaceObject *pNewTarget) const;
		bool CalcNavPath (CShip *pShip, const CVector &vTo);
		bool CalcNavPath (CShip *pShip, CSpaceObject *pTo);
		void CalcNavPath (CShip *pShip, CSpaceObject *pFrom, CSpaceObject *pTo);
		void CalcNavPath (CShip *pShip, CNavigationPath *pPath, bool bOwned = false);
		void CalcShieldState (CShip *pShip);
		int CalcWeaponScore (CShip *pShip, CSpaceObject *pTarget, CInstalledDevice *pWeapon, Metric rTargetDist2);
		void CancelDocking (CShip *pShip, CSpaceObject *pBase);
		bool CheckForFriendsInLineOfFire (CShip *pShip, CInstalledDevice *pDevice, CSpaceObject *pTarget, int iFireAngle, Metric rMaxRange)
			{ return (NoFriendlyFireCheck() || pShip->IsLineOfFireClear(pDevice, pTarget, iFireAngle, rMaxRange)); }
		CVector CombinePotential (const CVector &vDir)
			{ return GetPotential() + (vDir.Normal() * 100.0 * g_KlicksPerPixel);	}
		void CommunicateWithEscorts (CShip *pShip, MessageTypes iMessage, CSpaceObject *pParam1 = NULL, DWORD dwParam2 = 0);
		void Undock (CShip *pShip);

	private:
		void DebugAIOutput (CShip *pShip, LPCSTR pText);
		void CalcEscortFormation (CShip *pShip, CSpaceObject *pLeader, CVector *retvPos, CVector *retvVel, int *retiFacing);
		bool CalcFlockingFormationCloud (CShip *pShip, CSpaceObject *pLeader, Metric rFOVRange, Metric rSeparationRange, CVector *retvPos, CVector *retvVel, int *retiFacing);
		bool CalcFlockingFormationRandom (CShip *pShip, CSpaceObject *pLeader, CVector *retvPos, CVector *retvVel, int *retiFacing);
		bool ImplementAttackTargetManeuver (CShip *pShip, CSpaceObject *pTarget, const CVector &vTarget, Metric rTargetDist2);

		CAISettings m_AISettings;				//	Settings

		//	Ship control
		CAIShipControls m_ShipControls;			//	Current ship control state

		//	State
		EManeuverTypes m_iLastTurn;				//	Last turn direction
		int m_iLastTurnCount;					//	Number of updates turning
		int m_iManeuverCounter;					//	Counter used by maneuvers
		int m_iLastAttack;						//	Tick of last attack on us
		CVector m_vPotential;					//	Avoid potential
		CNavigationPath *m_pNavPath;			//	Current navigation path
		int m_iNavPathPos:16;					//	-1 = not in nav path
		int m_iBarrierClock:16;					//	We've hit a barrier, so try to recover

		DWORD m_fDockingRequested:1;			//	TRUE if we've requested docking
		DWORD m_fWaitForShieldsToRegen:1;		//	TRUE if ship is waiting for shields to regen
		DWORD m_dwSpare1:30;

		//	Cached values
		SUpdateCtx *m_pUpdateCtx;				//	System update context
		CInstalledDevice *m_pShields;			//	Shields (NULL if none)
		CInstalledDevice *m_pBestWeapon;		//	Best weapon
		DeviceNames m_iBestWeapon;
		Metric m_rBestWeaponRange;				//	Range of best weapon
		Metric m_rPrimaryAimRange2;				//	If further than this, close on target
		Metric m_rFlankDist;					//	Flank distance
		int m_iMaxTurnCount;					//	Max ticks turning in the same direction
		Metric m_rMaxWeaponRange;				//	Range of longest range primary weapon
		int m_iBestNonLauncherWeaponLevel;		//	Level of best non-launcher weapon
		int m_iPrematureFireChance;				//	Chance of firing prematurely

		DWORD m_fImmobile:1;					//	TRUE if ship does not move
		DWORD m_fSuperconductingShields:1;		//	TRUE if ship has superconducting shields
		DWORD m_fHasMultipleWeapons:1;			//	TRUE if ship has more than 1 primary
		DWORD m_fThrustThroughTurn:1;			//	TRUE if ship thrusts through a turn
		DWORD m_fAvoidExplodingStations:1;		//	TRUE if ship avoids exploding stations
		DWORD m_fRecalcBestWeapon:1;			//	TRUE if we need to recalc best weapon
		DWORD m_fHasSecondaryWeapons:1;			//	TRUE if ship has secondary weapons
		DWORD m_fHasEscorts:1;					//	TRUE if ship has escorts

		DWORD m_fHasMultiplePrimaries:1;		//	TRUE if ship has multiple primary weapons (non-launchers)
		DWORD m_fFreeNavPath:1;					//	TRUE if we own the nav path object
		DWORD m_fHasAvoidPotential:1;			//	TRUE if there is something to avoid
		DWORD m_fSpare4:1;
		DWORD m_fSpare5:1;
		DWORD m_fSpare6:1;
		DWORD m_fSpare7:1;
		DWORD m_fSpare8:1;

		DWORD m_dwSpare:16;
	};

const int MAX_OBJS =	4;

class IOrderModule
	{
	public:
		IOrderModule (int iObjCount);
		virtual ~IOrderModule (void);

		void Attacked (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pAttacker, const SDamageCtx &Damage, bool bFriendlyFire);
		void Behavior (CShip *pShip, CAIBehaviorCtx &Ctx) { OnBehavior(pShip, Ctx); }
		void BehaviorStart (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pOrderTarget, const IShipController::SData &Data) { OnBehaviorStart(pShip, Ctx, pOrderTarget, Data); }
		DWORD Communicate (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2);
		static IOrderModule *Create (IShipController::OrderTypes iOrder);
		CString DebugCrashInfo (CShip *pShip);
		void Destroyed (CShip *pShip, SDestroyCtx &Ctx);
		CSpaceObject *GetBase (void) { return OnGetBase(); }
		IShipController::OrderTypes GetOrder (void) { return OnGetOrder(); }
		CSpaceObject *GetTarget (void) { return OnGetTarget(); }
		void ObjDestroyed (CShip *pShip, const SDestroyCtx &Ctx);
		void ReadFromStream (SLoadCtx &Ctx);
		void WriteToStream (CSystem *pSystem, IWriteStream *pStream);

	protected:
		//	IOrderModule virtuals
		virtual bool IsAttacking (void) { return false; }
		virtual void OnAttacked (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pAttacker, const SDamageCtx &Damage, bool bFriendlyFire) { }
		virtual void OnBehavior (CShip *pShip, CAIBehaviorCtx &Ctx) = 0;
		virtual void OnBehaviorStart (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pOrderTarget, const IShipController::SData &Data) { }
		virtual DWORD OnCommunicate (CShip *pShip, CAIBehaviorCtx &Ctx, CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2) { return resNoAnswer; }
		virtual CString OnDebugCrashInfo (void) { return NULL_STR; }
		virtual void OnDestroyed (CShip *pShip, SDestroyCtx &Ctx) { }
		virtual CSpaceObject *OnGetBase (void) { return NULL; }
		virtual IShipController::OrderTypes OnGetOrder (void) = 0;
		virtual CSpaceObject *OnGetTarget (void) { return NULL; }
		virtual void OnObjDestroyed (CShip *pShip, const SDestroyCtx &Ctx, int iObj, bool *retbCancelOrder) { }
		virtual void OnReadFromStream (SLoadCtx &Ctx) { }
		virtual void OnWriteToStream (CSystem *pSystem, IWriteStream *pStream) { }

		int m_iObjCount;
		CSpaceObject *m_Objs[MAX_OBJS];
	};

class COrderList
	{
	public:
		~COrderList (void);

		void Delete (int iIndex);
		void DeleteAll (void);
		void DeleteCurrent (void);
		int GetCount (void) const { return m_List.GetCount(); }
		IShipController::OrderTypes GetCurrentOrder (void) const { return (IShipController::OrderTypes)(GetCurrentEntry().dwOrderType); }
		IShipController::OrderTypes GetCurrentOrder (CSpaceObject **retpTarget, IShipController::SData *retData = NULL) const { return GetOrder(0, retpTarget, retData); }
		DWORD GetCurrentOrderData (void) const { return GetCurrentEntry().dwData; }
		CSpaceObject *GetCurrentOrderTarget (void) const { return GetCurrentEntry().pTarget; }
		IShipController::OrderTypes GetOrder (int iIndex, CSpaceObject **retpTarget = NULL, IShipController::SData *retData = NULL) const;
		void Insert (IShipController::OrderTypes iOrder, CSpaceObject *pTarget, const IShipController::SData &Data, bool bAddBefore = false);
		void OnNewSystem (CSystem *pNewSystem, bool *retbCurrentChanged);
		void OnObjDestroyed (CSpaceObject *pObj, bool *retbCurrentChanged);
		void OnPlayerChangedShips (CSpaceObject *pOldShip, CSpaceObject *pNewShip, SPlayerChangedShipsCtx &Options, bool *retbCurrentChanged);
		void OnStationDestroyed (CSpaceObject *pObj, bool *retbCurrentChanged);
		void ReadFromStream (SLoadCtx &Ctx);
		void SetCurrentOrderData (const IShipController::SData &Data);
		void WriteToStream (IWriteStream *pStream, CSystem *pSystem);

	private:
		struct SOrderEntry
			{
			SOrderEntry (void) :
					dwOrderType(0),
					dwDataType(IShipController::dataNone),
					pTarget(NULL),
					dwData(0)
				{ }

			DWORD dwOrderType:8;			//	IShipController::OrderTypes
			DWORD dwDataType:8;				//	EDataTypes
			DWORD dwSpare:16;

			CSpaceObject *pTarget;
			DWORD dwData;
			};

		void CleanUp (SOrderEntry *pEntry);
		const SOrderEntry &GetCurrentEntry (void) const { return (m_List.GetCount() > 0 ? m_List[0] : m_NullOrder); }
		SOrderEntry &GetCurrentEntryActual (void) { return (m_List.GetCount() > 0 ? m_List[0] : m_NullOrder); }
		void SetEntryData (SOrderEntry *pEntry, const IShipController::SData &Data);

		TArray<SOrderEntry> m_List;

		static SOrderEntry m_NullOrder;
	};

class CBaseShipAI : public IShipController
	{
	public:
		CBaseShipAI (void);
		virtual ~CBaseShipAI (void);

		//	IShipController virtuals
		virtual void Behavior (SUpdateCtx &Ctx) override;
		virtual bool CanObjRequestDock (CSpaceObject *pObj = NULL) const override;
		virtual CString DebugCrashInfo (void) override;
		virtual void DebugPaintInfo (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual bool FollowsObjThroughGate (CSpaceObject *pLeader = NULL) override;
		virtual int GetAISettingInteger (const CString &sSetting) override;
		virtual CString GetAISettingString (const CString &sSetting) override;
		virtual const CAISettings *GetAISettings (void) override { return &m_AICtx.GetAISettings(); }
		virtual CSpaceObject *GetBase (void) const override;
		virtual int GetCombatPower (void) override;
		virtual bool GetDeviceActivate (void) override { return m_fDeviceActivate; }
		virtual CSpaceObject *GetEscortPrincipal (void) const override;
		virtual int GetFireRateAdj (void) override { return m_AICtx.GetFireRateAdj(); }
		virtual EManeuverTypes GetManeuver (void) override { return m_AICtx.GetManeuver(); }
		virtual CSpaceObject *GetOrderGiver (void) override;
		virtual bool GetReverseThrust (void) override { return false; }
		virtual CSpaceObject *GetShip (void) override { return m_pShip; }
		virtual bool GetStopThrust (void) override { return false; }
		virtual CSpaceObject *GetTarget (CItemCtx &ItemCtx, DWORD dwFlags = 0) const override;
		virtual bool GetThrust (void) override { return m_AICtx.GetThrust(m_pShip); }
		virtual void GetWeaponTarget (STargetingCtx &TargetingCtx, CItemCtx &ItemCtx, CSpaceObject **retpTarget, int *retiFireSolution, bool bTargetMissiles = false) override;
		virtual bool IsAngryAt (CSpaceObject *pObj) const override;
		virtual bool IsPlayerBlacklisted (void) const override { return (m_fPlayerBlacklisted ? true : false); }
		virtual bool IsPlayerWingman (void) const override { return (m_fIsPlayerWingman ? true : false); }
		virtual void OnAttacked (CSpaceObject *pAttacker, const SDamageCtx &Damage) override;
		virtual DWORD OnCommunicate (CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2) override;
		virtual void OnDestroyed (SDestroyCtx &Ctx) override;
		virtual void OnDocked (CSpaceObject *pObj) override;
		virtual void OnDockingStop (void) override;
		virtual void OnEnterGate (CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate, bool bAscend) override;
		virtual void OnNewSystem (CSystem *pSystem) override;
		virtual void OnWeaponStatusChanged (void) override { m_AICtx.ClearBestWeapon(); }
		virtual void OnObjEnteredGate (CSpaceObject *pObj, CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate) override;
		virtual void OnObjDestroyed (const SDestroyCtx &Ctx) override;
		virtual void OnPlayerChangedShips (CSpaceObject *pOldShip, SPlayerChangedShipsCtx &Options) override;
		virtual void OnPlayerObj (CSpaceObject *pPlayer) override;
		virtual void OnShipStatus (EShipStatusNotifications iEvent, DWORD dwData = 0) override;
		virtual void OnStationDestroyed (const SDestroyCtx &Ctx) override;
		virtual void OnStatsChanged (void) override { m_AICtx.CalcInvariants(m_pShip); }
		virtual void OnSystemLoaded (void) override { m_AICtx.CalcInvariants(m_pShip); OnSystemLoadedNotify(); }
		virtual int SetAISettingInteger (const CString &sSetting, int iValue) override;
		virtual CString SetAISettingString (const CString &sSetting, const CString &sValue) override;
		virtual void SetCommandCode (ICCItem *pCode) override;
		virtual void SetManeuver (EManeuverTypes iManeuver) override { m_AICtx.SetManeuver(iManeuver); }
		virtual void SetShipToControl (CShip *pShip) override;
		virtual void SetThrust (bool bThrust) override { m_AICtx.SetThrust(bThrust); }
		virtual void SetPlayerBlacklisted (bool bValue) override { m_fPlayerBlacklisted = bValue; }
		virtual void SetPlayerWingman (bool bIsWingman) override { m_fIsPlayerWingman = bIsWingman; }
		virtual void ReadFromStream (SLoadCtx &Ctx, CShip *pShip) override;
		virtual void WriteToStream (IWriteStream *pStream) override;

		virtual void AddOrder (IShipController::OrderTypes Order, CSpaceObject *pTarget, const IShipController::SData &Data, bool bAddBefore = false) override;
		virtual void CancelAllOrders (void) override;
		virtual void CancelCurrentOrder (void) override;
		virtual bool CancelOrder (int iIndex) override;
		virtual DWORD GetCurrentOrderData (void) override { return m_Orders.GetCurrentOrderData(); }
		virtual OrderTypes GetCurrentOrderEx (CSpaceObject **retpTarget = NULL, IShipController::SData *retData = NULL) override { return m_Orders.GetCurrentOrder(retpTarget, retData); }
		virtual OrderTypes GetOrder (int iIndex, CSpaceObject **retpTarget = NULL, IShipController::SData *retData = NULL) const override { return m_Orders.GetOrder(iIndex, retpTarget, retData); }
		virtual int GetOrderCount (void) const override { return m_Orders.GetCount(); }

	protected:
		CSpaceObject *CalcEnemyShipInRange (CSpaceObject *pCenter, Metric rRange, CSpaceObject *pExcludeObj = NULL);
		Metric CalcShipIntercept (const CVector &vRelPos, const CVector &vAbsVel, Metric rMaxSpeed);
		void CancelDocking (CSpaceObject *pTarget);
		bool CheckForEnemiesInRange (CSpaceObject *pCenter, Metric rRange, int iInterval, CSpaceObject **retpTarget);
		bool CheckOutOfRange (CSpaceObject *pTarget, Metric rRange, int iInterval);
		bool CheckOutOfZone (CSpaceObject *pBase, Metric rInnerRadius, Metric rOuterRadius, int iInterval);
		void FireOnOrderChanged (void);
		void FireOnOrdersCompleted (void);
		IShipController::OrderTypes GetCurrentOrder (void) const { return m_Orders.GetCurrentOrder(); }
		CSpaceObject *GetCurrentOrderTarget (void) const { return m_Orders.GetCurrentOrderTarget(); }
		Metric GetDistance (CSpaceObject *pObj) const { return (pObj->GetPos() - m_pShip->GetPos()).Length(); }
		Metric GetDistance2 (CSpaceObject *pObj) const { return (pObj->GetPos() - m_pShip->GetPos()).Length2(); }
		CSpaceObject *GetPlayerOrderGiver (void) const;
		CUniverse &GetUniverse (void) const { return (m_pShip ? m_pShip->GetUniverse() : *g_pUniverse); }
		bool InitOrderModule (void);
		bool IsImmobile (void) const { return m_AICtx.IsImmobile(); }
		bool IsPlayerOrPlayerFollower (CSpaceObject *pObj, int iRecursions = 0);
		bool IsWaitingForShieldsToRegen (void) { return m_AICtx.IsWaitingForShieldsToRegen(); }
		void HandleFriendlyFire (CSpaceObject *pAttacker, CSpaceObject *pOrderGiver);
		bool IsDockingRequested (void) { return m_AICtx.IsDockingRequested(); }
		void ResetBehavior (void);
		void SetCurrentOrderData (const SData &Data);
		void UpgradeShieldBehavior (void);
		void UpgradeWeaponBehavior (void);
		void UseItemsBehavior (void);

		//	CBaseShipAI virtuals
		virtual void OnAttackedNotify (CSpaceObject *pAttacker, const SDamageCtx &Damage) { }
		virtual void OnBehavior (SUpdateCtx &Ctx) { }
		virtual void OnCleanUp (void) { }
		virtual DWORD OnCommunicateNotify (CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2) { return resNoAnswer; }
		virtual CString OnDebugCrashInfo (void) { return NULL_STR; }
		virtual void OnDestroyedNotify (SDestroyCtx &Ctx) { }
		virtual void OnDockedEvent (CSpaceObject *pObj) { }
		virtual bool OnGetAISettingInteger (const CString &sSetting, int *retiValue) { return false; }
		virtual bool OnGetAISettingString (const CString &sSetting, CString *retsValue) { return false; }
		virtual CSpaceObject *OnGetBase (void) const { return NULL; }
		virtual CSpaceObject *OnGetTarget (DWORD dwFlags = 0) const { return NULL; }
		virtual void OnHitBarrier (CSpaceObject *pBarrierObj, const CVector &vPos);
		virtual void OnNewSystemNotify (void) { }
		virtual void OnOrderChanged (void) { }
		virtual void OnObjDestroyedNotify (const SDestroyCtx &Ctx) { }
		virtual void OnReadFromStream (SLoadCtx &Ctx) { }
		virtual bool OnSetAISettingInteger (const CString &sSetting, int iValue) { return false; }
		virtual bool OnSetAISettingString (const CString &sSetting, const CString &sValue) { return false; }
		virtual void OnSystemLoadedNotify (void) { }
		virtual void OnWriteToStream (IWriteStream *pStream) { }

		CShip *m_pShip;							//	Ship that we're controlling
		COrderList m_Orders;					//	Ordered list of orders
		ICCItem *m_pCommandCode;				//	Code to generate orders (may be NULL)

		CAIBehaviorCtx m_AICtx;					//	Ctx for behavior
		IOrderModule *m_pOrderModule;			//	Current order module (may be NULL)

		CAttackDetector m_Blacklist;			//	Player blacklisted

		//	Flags

		DWORD m_fDeviceActivate:1;
		DWORD m_fInOnOrderChanged:1;
		DWORD m_fInOnOrdersCompleted:1;
		DWORD m_fCheckedForWalls:1;				//	TRUE if we searched for walls in the system
		DWORD m_fAvoidWalls:1;					//	TRUE if we need to avoid walls
		DWORD m_fIsPlayerWingman:1;				//	TRUE if this is a wingman for the player
		DWORD m_fOldStyleBehaviors:1;			//	TRUE if we're not using m_pOrderModule
		DWORD m_fPlayerBlacklisted:1;			//	TRUE if we've blacklisted the player (for attacking us)

		DWORD m_fSpare:24;
	};

//	Inlines --------------------------------------------------------------------

inline bool CAIShipControls::GetThrust (CShip *pShip) const 
	{
	switch (m_iThrustDir)
		{
		case constNeverThrust:
			return false;

		case constAlwaysThrust:
			return true;

		default:
			return pShip->IsPointingTo(m_iThrustDir);
		}
	}

#endif
