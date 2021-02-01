//	TSEStationSystems.h
//
//	Defines classes and interfaces for stations
//	Copyright (c) 2021 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CStation;

class CSquadronController
	{
	public:
		static constexpr int STATION_REINFORCEMENT_FREQUENCY =	607;

		struct SUpdateCtx
			{
			SUpdateCtx (CSpaceObject &SourceObjArg, int iTickArg = 0) :
					SourceObj(SourceObjArg),
					iTick(iTickArg)
				{ }

			CSpaceObject &SourceObj;
			int iTick = 0;

			bool bNoConstruction = false;
			bool bNoReinforcements = false;
			};

		void CreateInitialShips (CSpaceObject &SourceObj, const CSquadronDescList &Desc);
		int GetReinforceRequestCount () const;
		ICCItemPtr GetStatus (const CSpaceObject &SourceObj) const;
		void OnObjDestroyed (CSpaceObject &SourceObj, const SDestroyCtx &Ctx);
		void ReadFromStream (SLoadCtx &Ctx, const CSquadronDescList &Desc);
		void SetReinforceRequestCount (int iValue);
		void Update (SUpdateCtx &Ctx, const CSquadronDescList &Desc);
		void WriteToStream (IWriteStream &Stream) const;

	private:
		struct SSquadronEntry
			{
			CString sID;
			const CSquadronDesc *pDesc = NULL;			//	May be NULL
			CSpaceObjectList Squadron;					//	List of squadron members

			DWORD dwLastReinforcementRequestOn = 0;		//	Tick on which we last requested
			int iTotalReinforceCount = 0;				//	Total requests for reinforcements
			int iReinforceRequestCount = 0;				//	Consecutive requests for reinforcements
			int iTotalDestroyed = 0;					//	Ships destroyed in total
			};

		void CreateInitialShips (CSpaceObject &SourceObj, const CSquadronDesc &SquadronDesc);
		SSquadronEntry *GetAt (const CString &sID);
		SSquadronEntry &SetAt (const CSquadronDesc &SquadronDesc);
		void Update (SUpdateCtx &Ctx, const CSquadronDesc &Desc);
		void UpdateConstruction (SUpdateCtx &Ctx, const CSquadronDesc &Desc, const IShipGenerator &ConstructionTable);

		TArray<SSquadronEntry> m_Squadrons;
	};

//	CStation Space Object ------------------------------------------------------

class CStation : public TSpaceObjectImpl<OBJID_CSTATION>
	{
	public:
		CStation (CUniverse &Universe);
		virtual ~CStation (void);

		static ALERROR CreateFromType (CSystem &System,
				CStationType *pType,
				SObjCreateCtx &CreateCtx,
				CStation **retpStation,
				CString *retsError = NULL);

		void Abandon (DestructionTypes iCause, const CDamageSource &Attacker, CWeaponFireDesc *pWeaponDesc = NULL);
		void ClearFireReconEvent (void) { m_fFireReconEvent = false; }
		void ClearReconned (void) { m_fReconned = false; }
		int GetFireRateAdj () const { return m_pType->GetFireRateAdj(); }
		const CStationHull &GetHull () const { return m_Hull; }
		const CStationHullDesc &GetHullDesc () const { return m_pType->GetHullDesc(); }
		int GetImageVariant (void);
		int GetImageVariantCount (void) { return m_pType->GetImageVariants(); }
		int GetSubordinateCount (void) const { return m_Subordinates.GetCount(); }
		CSpaceObject *GetSubordinate (int iIndex) const { return m_Subordinates.GetObj(iIndex); }
		bool IsNameSet (void) const;
		bool IsReconned (void) { return (m_fReconned ? true : false); }
		void SetActive (void) { m_fActive = true; }
		void SetBase (CSpaceObject &BaseObj, const CString &sSubordinateID = NULL_STR) { m_pBase = &BaseObj; m_sSubordinateID = sSubordinateID; }
		void SetFireReconEvent (void) { m_fFireReconEvent = true; }
		void SetFlotsamImage (CItemType *pItemType);
		void SetForceMapLabel (bool bValue = true) { m_fForceMapLabel = bValue; }
		void SetImageVariant (int iVariant);
		void SetImageVariant (const CString &sVariantID);
		void SetInactive (void) { m_fActive = false; }
		void SetMapOrbit (const COrbit &oOrbit);
		void SetMass (Metric rMass) { m_rMass = rMass; }
		void SetNoConstruction (void) { m_fNoConstruction = true; }
		void SetNoReinforcements (void) { m_fNoReinforcements = true; }
		void SetPaintOrder (CPaintOrder::Types iOrder) { m_iPaintOrder = iOrder; }
		void SetReconned (void) { m_fReconned = true; }
		void SetRotation (int iAngle) { if (m_pRotation) m_pRotation->SetRotationAngle(m_pType->GetRotationDesc(), iAngle); }
		void SetSuppressMapLabel (bool bValue = true) { m_fSuppressMapLabel = bValue; }
		void SetStargate (const CString &sDestNode, const CString &sDestEntryPoint);
		void SetStructuralHitPoints (int iHP) { m_Hull.SetStructuralHP(iHP); }

		//	CSpaceObject virtuals

		virtual void AddOverlay (COverlayType *pType, int iPosAngle, int iPosRadius, int iRotation, int iPosZ, int iLifetime, DWORD *retdwID = NULL) override;
		using CSpaceObject::AddOverlay;
		virtual void AddSubordinate (CSpaceObject &SubordinateObj, const CString &sSubordinateID = NULL_STR) override;
		virtual CTradingDesc *AllocTradeDescOverride (void) override;
		virtual CStation *AsStation (void) override { return this; }
		virtual bool CalcVolumetricShadowLine (SLightingCtx &Ctx, int *retxCenter, int *retyCenter, int *retiWidth, int *retiLength) override;
		virtual bool CanAttack (void) const override;
		virtual bool CanBeAttacked (void) const override 
			{
			//	This handles the case (e.g.,) of Abbasid walls, which are 
			//	destroyed when abandoned. We essentially treat the walls as part
			//	of the station.
			//
			//	We want the walls to be targetable as long as the station is 
			//	alive.

			if (m_pType->IsDestroyWhenAbandoned())
				return (CanAttack() || (m_Hull.GetHitPoints() > 0 && GetBase() && !GetBase()->IsAbandoned())); 
			else
				return (m_Hull.GetHitPoints() > 0 || CanAttack()); 
			}
		virtual bool CanBeDestroyed (void) override { return m_Hull.CanBeDestroyed(); }
		virtual bool CanBeDestroyedBy (CSpaceObject &Attacker) const override;
		virtual bool CanBeMined (void) const override { return (m_pType->ShowsUnexploredAnnotation() && !IsOutOfPlaneObj()); }
		virtual bool CanBlock (CSpaceObject *pObj) override;
		virtual bool CanBlockShips (void) override { return m_fBlocksShips; }
		virtual RequestDockResults CanObjRequestDock (CSpaceObject *pObj = NULL) const override;
		virtual bool ClassCanAttack (void) override;
		virtual void CreateRandomDockedShips (IShipGenerator *pGenerator, const CShipChallengeDesc &Needed = CShipChallengeDesc()) override;
		virtual CString DebugCrashInfo (void) const override;
		virtual const CAsteroidDesc &GetAsteroidDesc (void) const { return m_pType->GetAsteroidDesc(); }
		virtual CurrencyValue GetBalancedTreasure (void) const { return m_pType->GetBalancedTreasure(); }
		virtual CSpaceObject *GetBase (void) const override { return m_pBase; }
		virtual Categories GetCategory (void) const override { return catStation; }
		virtual DWORD GetClassUNID (void) override { return m_pType->GetUNID(); }
		virtual const CCurrencyBlock *GetCurrencyBlock (void) const override;
		virtual CCurrencyBlock *GetCurrencyBlock (bool bCreate = false) override;
		virtual int GetDamageEffectiveness (CSpaceObject *pAttacker, CInstalledDevice *pWeapon) override;
		virtual DWORD GetDefaultBkgnd (void) override { return m_pType->GetDefaultBkgnd(); }
		virtual const CRegenDesc &GetDefaultShipRepair () const { return m_pType->GetShipRegenDesc(); }
		virtual CInstalledDevice *GetDevice (int iDev) override { return &m_Devices.GetDevice(iDev); }
		virtual int GetDeviceCount (void) const override { return m_Devices.GetCount(); }
		virtual CDeviceItem GetDeviceItem (int iDev) const override { return m_Devices.GetDeviceItem(iDev); }
		virtual const CDeviceSystem &GetDeviceSystem (void) const { return m_Devices; }
		virtual CDeviceSystem &GetDeviceSystem (void) { return m_Devices; }
		virtual const CDockingPorts *GetDockingPorts (void) const override { return &m_DockingPorts; }
		virtual CDockingPorts *GetDockingPorts (void) override { return &m_DockingPorts; }
		virtual CStationType *GetEncounterInfo (void) override { return m_pType; }
		virtual Metric GetGravity (Metric *retrRadius) const override;
		virtual const CObjectImageArray &GetHeroImage (void) const override { return m_pType->GetHeroImage(CCompositeImageSelector(), CCompositeImageModifiers()); }
		virtual const CObjectImageArray &GetImage (int *retiRotationFrameIndex = NULL) const override { if (retiRotationFrameIndex) *retiRotationFrameIndex = 0; return m_pType->GetImage(m_ImageSelector, CCompositeImageModifiers()); }
		virtual const CCompositeImageSelector &GetImageSelector (void) const override { return m_ImageSelector; }
		virtual Metric GetInvMass (void) const override;
		virtual int GetLevel (void) const override { return m_pType->GetLevel(); }
		virtual const COrbit *GetMapOrbit (void) const override { return m_pMapOrbit; }
		virtual Metric GetMass (void) const override { return m_rMass; }
		virtual int GetMaxLightDistance (void) const override { return m_pType->GetMaxLightDistance(); }
		virtual Metric GetMaxSpeed (void) const { return (IsAnchored() ? 0.0 : 0.25 * LIGHT_SPEED); }
		virtual Metric GetMaxWeaponRange (void) const override;
		virtual CString GetNamePattern (DWORD dwNounPhraseFlags = 0, DWORD *retdwFlags = NULL) const override;
		virtual CString GetObjClassName (void) const override { return CONSTLIT("CStation"); }
		virtual COverlayList *GetOverlays (void) override { return &m_Overlays; }
		virtual const COverlayList *GetOverlays (void) const override { return &m_Overlays; }
		virtual CSystem::LayerEnum GetPaintLayer (void) const override;
		virtual Metric GetParallaxDist (void) override { return m_rParallaxDist; }
		virtual EDamageResults GetPassthroughDefault (void) override { return m_Hull.GetPassthroughDefault(); }
		virtual int GetPlanetarySize (void) const override { return (GetScale() == scaleWorld ? m_pType->GetSize() : 0); }
		virtual ICCItem *GetPropertyCompatible (CCodeChainCtx &Ctx, const CString &sName) const override;
		virtual IShipGenerator *GetRandomEncounterTable (int *retiFrequency = NULL) const override;
		virtual int GetRotation (void) const override;
		virtual ScaleTypes GetScale (void) const override { return m_Scale; }
		virtual CXMLElement *GetScreen (const CString &sName) override { return m_pType->GetScreen(sName); }
		virtual CSovereign *GetSovereign (void) const override { return m_pSovereign; }
		virtual CG32bitPixel GetSpaceColor (void) override { return m_pType->GetSpaceColor(); }
		virtual CString GetStargateID (void) const override;
		virtual int GetStealth (void) const override;
		virtual Metric GetStellarMass (void) const override { return (GetScale() == scaleStar ? m_rMass : 0.0); }
		virtual const CString &GetSubordinateID (void) const override { return m_sSubordinateID; }
		virtual CSpaceObject *GetTarget (DWORD dwFlags = 0) const override;
		virtual CTradingDesc *GetTradeDescOverride (void) const override { return m_pTrade; }
		virtual CDesignType *GetType (void) const override { return m_pType; }
		virtual int GetVisibleDamage (void) override { return m_Hull.GetVisibleDamage(); }
		virtual void GetVisibleDamageDesc (SVisibleDamage &Damage) const override { return m_Hull.GetVisibleDamageDesc(Damage); }
		virtual CDesignType *GetWreckType (void) const override;
		virtual bool HasAttribute (const CString &sAttribute) const override;
		virtual bool HasStarlightImage (void) const override { return (m_rStarlightDist > 0.0); }
		virtual bool HasVolumetricShadow (int *retiStarAngle = NULL, Metric *retrStarDist = NULL) const override;
		virtual bool ImageInObject (const CVector &vObjPos, const CObjectImageArray &Image, int iTick, int iRotation, const CVector &vImagePos) override;
		virtual bool IsAbandoned (void) const override { return m_Hull.IsAbandoned(); }
		virtual bool IsActiveStargate (void) const override { return !m_sStargateDestNode.IsBlank() && m_fActive; }
		virtual bool IsAnchored (void) const override { return (!m_pType->IsMobile() || IsManuallyAnchored()); }
		virtual bool IsAngry (void) override { return (!IsAbandoned() && (m_iAngryCounter > 0)); }
		virtual bool IsAngryAt (const CSpaceObject *pObj) const override { return (IsEnemy(pObj) || IsBlacklisted(pObj)); }
		virtual bool IsExplored (void) const override { return m_fExplored; }
		virtual bool IsIdentified (void) const override { return m_fKnown; }
		virtual bool IsImmutable (void) const override { return m_Hull.IsImmutable(); }
		virtual bool IsInactive (void) const override { return !CanBeAttacked(); }
		virtual bool IsIntangible (void) const override { return (IsVirtual() || IsSuspended() || IsDestroyed() || IsOutOfPlaneObj()); }
		virtual bool IsKnown (void) const override { return m_fKnown; }
		virtual bool IsMultiHull (void) const override { return (m_Hull.GetHullType() != CStationHullDesc::hullSingle); }
		virtual bool IsPaintDeferred (SViewportPaintCtx &Ctx) const override { return (m_iPaintOrder & (CPaintOrder::bringToFront | CPaintOrder::sendToBack)) && !Ctx.bInPaintSubordinate; }
		virtual bool IsSatelliteSegmentOf (const CSpaceObject &Base, CPaintOrder::Types *retiPaintOrder = NULL) const override { if (retiPaintOrder) *retiPaintOrder = m_iPaintOrder; return (m_fIsSegment && (m_pBase == Base)); }
		virtual bool IsShownInGalacticMap (void) const override;
		virtual bool IsStargate (void) const override { return !m_sStargateDestNode.IsBlank(); }
		virtual bool IsUnreal (void) const override { return (IsSuspended() || IsDestroyed()); }
		virtual bool IsVirtual (void) const override { return m_pType->IsVirtual(); }
		virtual bool IsWreck (void) const override { return (m_dwWreckUNID != 0); }
		virtual void MarkImages (void) override;
		virtual bool ObjectInObject (const CVector &vObj1Pos, CSpaceObject *pObj2, const CVector &vObj2Pos) override;
		virtual void OnDestroyed (SDestroyCtx &Ctx) override;
		virtual void OnObjBounce (CSpaceObject *pObj, const CVector &vPos) override;
		virtual void OnObjLeaveGate (CSpaceObject *pObj) override;
		virtual void OnPlayerObj (CSpaceObject *pPlayer) override;
		virtual void OnStationDestroyed (const SDestroyCtx &Ctx) override;
		virtual void OnSubordinateDestroyed (SDestroyCtx &Ctx) override;
		virtual void OnSubordinateHit (SDamageCtx &Ctx) override;
		virtual void OnSystemCreated (SSystemCreateCtx &CreateCtx) override;
		virtual void PaintLRSBackground (CG32bitImage &Dest, int x, int y, const ViewportTransform &Trans) override;
		virtual void PaintLRSForeground (CG32bitImage &Dest, int x, int y, const ViewportTransform &Trans) override;
		virtual bool PointInObject (const CVector &vObjPos, const CVector &vPointPos) const override;
		virtual bool PointInObject (SPointInObjectCtx &Ctx, const CVector &vObjPos, const CVector &vPointPos) const override;
		virtual void PointInObjectInit (SPointInObjectCtx &Ctx) const override;
		virtual void RefreshBounds (void) override { CalcBounds(); }
		virtual void RemoveOverlay (DWORD dwID) override;
		virtual bool RemoveSubordinate (CSpaceObject *pSubordinate) override;
		virtual bool RequestGate (CSpaceObject *pObj) override;
		virtual void SetExplored (bool bExplored = true) override { m_fExplored = bExplored; }
		virtual void SetIdentified (bool bIdentified = true) override { m_fKnown = bIdentified; }
		virtual void SetKnown (bool bKnown = true) override;
		virtual void SetMapLabelPos (CMapLabelPainter::EPositions iPos) override { m_MapLabel.CleanUp(); m_MapLabel.SetPos(iPos); m_fMapLabelInitialized = false; }
		virtual void SetName (const CString &sName, DWORD dwFlags = 0) override;
		virtual bool SetProperty (const CString &sName, ICCItem *pValue, CString *retsError) override;
		virtual void SetStarlightParams (const CSpaceObject &StarObj, Metric rLightRadius) override;
		virtual bool ShowMapLabel (int *retcxLabel = NULL, int *retcyLabel = NULL) const override;
		virtual bool ShowMapOrbit (void) const override { return (m_fShowMapOrbit ? true : false); }
		virtual bool ShowStationDamage (void) const override { return m_Hull.IsWrecked(); }
		virtual bool SupportsGating (void) override { return IsActiveStargate(); }
		virtual void Undock (CSpaceObject *pObj) override;

	protected:

		//	CSpaceObject virtuals
		virtual bool CanFireOn (CSpaceObject *pObj) const override { return CanFireOnObjHelper(pObj); }
		virtual CDesignType *GetDefaultDockScreen (CString *retsName = NULL, ICCItemPtr *retpData = NULL) const override;
		virtual EConditionResult OnApplyCondition (ECondition iCondition, const SApplyConditionOptions &Options) override;
		virtual EConditionResult OnCanApplyCondition (ECondition iCondition, const SApplyConditionOptions &Options) const override;
		virtual EConditionResult OnCanRemoveCondition (ECondition iCondition, const SApplyConditionOptions &Options) const override;
		virtual void OnMove (const CVector &vOldPos, Metric rSeconds) override;
		virtual void ObjectDestroyedHook (const SDestroyCtx &Ctx) override;
		virtual DWORD OnCommunicate (CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2, ICCItem *pData) override;
		virtual void OnComponentChanged (ObjectComponentTypes iComponent) override;
		virtual EDamageResults OnDamage (SDamageCtx &Ctx) override;
		virtual bool OnGetCondition (ECondition iCondition) const override;
		virtual bool OnIsImmuneTo (SpecialDamageTypes iSpecialDamage) const override;
		virtual void OnObjEnteredGate (CSpaceObject *pObj, CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate) override;
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual void OnPaintAnnotations (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual void OnPaintMap (CMapViewportCtx &Ctx, CG32bitImage &Dest, int x, int y) override;
		virtual void OnReadFromStream (SLoadCtx &Ctx) override;
		virtual EConditionResult OnRemoveCondition (ECondition iCondition, const SApplyConditionOptions &Options) override;
		virtual void OnSetEventFlags (void) override;
		virtual void OnSetSovereign (CSovereign *pSovereign) override { m_pSovereign = pSovereign; }
		virtual void OnUpdate (SUpdateCtx &Ctx, Metric rSecondsPerTick) override;
		virtual void OnUpdateExtended (const CTimeSpan &ExtraTime) override;
		virtual void OnWriteToStream (IWriteStream *pStream) override;

	private:
		enum Constants
			{
			maxRegisteredObjects	= 12,
			maxDevices				= 8,
			};

		static constexpr CG32bitPixel RGB_MINING_MARKER_UNEXPORED = CG32bitPixel(128, 128, 128);

		static constexpr int MIN_NAMED_WORLD_SIZE = 1000;	//	Roughly the size of Ceres
		static constexpr int WORLD_SIZE = 5000;				//	Roughly the size of Mercury
		static constexpr int LARGE_WORLD_SIZE = 25000;		//	Roughly twice the size of Earth
															//		(Half the size of Neptune)

		void AvengeAttack (CSpaceObject *pAttacker);
		bool Blacklist (CSpaceObject *pObj);
		int CalcAdjustedDamage (SDamageCtx &Ctx) const;
		int CalcAdjustedDamageAbandoned (SDamageCtx &Ctx) const;
		void CalcBounds (void);
		void CalcDeviceBonus (void);
		void CalcImageModifiers (CCompositeImageModifiers *retModifiers, int *retiTick = NULL) const;
		Metric CalcMaxAttackDist (void) const;
		int CalcNumberOfShips (void);
		bool CanBlacklist (void) const { return (m_pType->IsBlacklistEnabled() && !IsImmutable() && !m_fNoBlacklist); }
		void ClearBlacklist (CSpaceObject *pObj);
		void CreateDestructionEffect (void);
		void CreateEjectaFromDamage (int iDamage, const CVector &vHitPos, int iDirection, const DamageDesc &Damage);
		void CreateStructuralDestructionEffect (SDestroyCtx &Ctx);
		ALERROR CreateMapImage (void) const;
		void DeterAttack (CSpaceObject *pTarget);
		void FinishCreation (SSystemCreateCtx *pSysCreateCtx = NULL);
		Metric GetAttackDistance (void) const;
		const CObjectImageArray &GetImage (bool bFade, int *retiTick = NULL, int *retiVariant = NULL) const;
		bool IsBlacklisted (const CSpaceObject *pObj = NULL) const;
		EDamageResults OnDamageAbandoned (SDamageCtx &Ctx);
		EDamageResults OnDamageImmutable (SDamageCtx &Ctx);
		EDamageResults OnDamageNormal (SDamageCtx &Ctx);
		void OnDestroyedByFriendlyFire (CSpaceObject *pAttacker, CSpaceObject *pOrderGiver);
		void OnDestroyedByHostileFire (CSpaceObject *pAttacker, CSpaceObject *pOrderGiver);
		void OnHitByFriendlyFire (CSpaceObject *pAttacker, CSpaceObject *pOrderGiver);
		void OnHitByHostileFire (CSpaceObject *pAttacker, CSpaceObject *pOrderGiver);
		bool OnMiningDamage (SDamageCtx &Ctx);
		void PaintMarkerIcon (CG32bitImage& Dest, int x, int y);
		void PaintSatellites (CG32bitImage &Dest, int x, int y, DWORD dwPaintOptions, SViewportPaintCtx &Ctx) const;
		void RaiseAlert (CSpaceObject *pTarget);
		void SetAngry (void);
		void SetWreckParams (CShipClass *pWreckClass, CShip *pShip = NULL);
		bool UpdateAttacking (SUpdateCtx &Ctx, int iTick);
		void UpdateDestroyedAnimation (void);
		bool UpdateDevices (SUpdateCtx &Ctx, int iTick, CTargetList &TargetList, bool &iobModified);
		bool UpdateOverlays (SUpdateCtx &Ctx, bool &iobCalcBounds, bool &iobCalcDevices);
		void UpdateReinforcements (int iTick);
		void UpdateTargets (SUpdateCtx &Ctx, Metric rAttackRange);

		CStationType *m_pType = NULL;					//	Station type
		CString m_sName;								//	Station name
		DWORD m_dwNameFlags = 0;						//	Name flags
		CSovereign *m_pSovereign = NULL;				//	Allegiance
		ScaleTypes m_Scale = scaleNone;					//	Scale of station
		Metric m_rMass = 0.0;							//	Mass of station (depends on scale)
		CIntegralRotation *m_pRotation = NULL;			//	Rotation parameters (may be NULL)

		CCompositeImageSelector m_ImageSelector;		//	Image variant to display
		int m_iDestroyedAnimation = 0;					//	Frames left of destroyed animation
		COrbit *m_pMapOrbit = NULL;						//	Orbit to draw on map
		Metric m_rParallaxDist = 1.0;					//	Parallax distance (1.0 = normal; > 1.0 = background; < 1.0 = foreground)
		CPaintOrder::Types m_iPaintOrder = CPaintOrder::none;	//	Paint order instructions
		int m_iStarlightImageRotation = 0;				//	Rotation of starlight image
		Metric m_rStarlightDist = 0.0;					//	Distance from nearest star
		DWORD m_dwWreckUNID = 0;						//	UNID of wreck class (0 if none)

		CString m_sStargateDestNode;					//	Destination node
		CString m_sStargateDestEntryPoint;				//	Destination entry point

		CStationHull m_Hull;							//	Hull and armor
		CDeviceSystem m_Devices;						//	Array of CInstalledDevice
		mutable Metric m_rMaxAttackDist = 0.0;			//	Maximum attack distance
		COverlayList m_Overlays;						//	List of overlays
		CDockingPorts m_DockingPorts;					//	Docking ports

		CSquadronController m_Squadrons;				//	Our ships
		CSpaceObject *m_pBase = NULL;					//	If we're a subordinate, this points to our base
		CString m_sSubordinateID;						//	If we're a subordinate, this is our ID
		CSpaceObjectList m_Subordinates;				//	List of subordinates
		CSpaceObjectList m_Targets;						//	Targets to destroy (by our ships)
		CTargetList m_WeaponTargets;					//	Targets to destroy (by our weapons)

		CAttackDetector m_Blacklist;					//	Player blacklisted
		int m_iAngryCounter = 0;						//	Attack cycles until station is not angry

		CCurrencyBlock *m_pMoney = NULL;				//	Money left (may be NULL)
		CTradingDesc *m_pTrade = NULL;					//	Override of trading desc (may be NULL)

		DWORD m_fArmed:1 = false;						//	TRUE if station has weapons
		DWORD m_fKnown:1 = false;						//	TRUE if known to the player
		DWORD m_fSuppressMapLabel:1 = false;			//	Do not show map label
		DWORD m_fActive:1 = false;						//	TRUE if stargate is active
		DWORD m_fNoReinforcements:1 = false;			//	Do not send reinforcements
		DWORD m_fRadioactive:1 = false;					//	TRUE if radioactive
		DWORD m_fReconned:1 = false;					//	TRUE if reconned by player
		DWORD m_fFireReconEvent:1 = false;				//	If TRUE, fire OnReconned

		DWORD m_fExplored:1 = false;					//	If TRUE, player has docked at least once
		DWORD m_fNoBlacklist:1 = false;					//	If TRUE, do not blacklist player on friendly fire
		DWORD m_fNoConstruction:1 = false;				//	Do not build new ships
		DWORD m_fBlocksShips:1 = false;					//	TRUE if we block ships
		DWORD m_fShowMapOrbit:1 = false;				//	If TRUE, show orbit in map
		DWORD m_fDestroyIfEmpty:1 = false;				//	If TRUE, we destroy the station as soon as it is empty
		DWORD m_fIsSegment:1 = false;                   //  If TRUE, we are a segment of some other object (m_pBase)
		DWORD m_fForceMapLabel:1 = false;				//	Force showing map label

		DWORD m_fHasMissileDefense:1 = false;			//	If TRUE, at least one device is a missile defense weapon
		mutable DWORD m_fMapLabelInitialized:1 = false;	//	If TRUE, we've initialized m_MapLabel
		mutable DWORD m_fMaxAttackDistValid:1 = false;	//	TRUE if m_rMaxAttackDist is valid
		DWORD m_fAnonymous:1 = false;					//	TRUE if world has not been explicitly named
		DWORD m_fFadeImage:1 = false;					//	If TRUE, fade image in stellar light
		DWORD m_fAllowEnemyDocking:1 = false;			//	If TRUE, enemies can dock.

		DWORD m_dwSpare:10;

		//	Cached

		mutable CG32bitImage m_MapImage;				//	Image for the map (if star or world)
		mutable CMapLabelPainter m_MapLabel;			//	Cached info about map label
	};

