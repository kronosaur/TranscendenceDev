//	TSEUniverse.h
//
//	Transcendence Space Engine
//	Copyright 2017 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

enum EStorageScopes
	{
	storeUnknown				= -1,

	storeDevice					= 0,
	storeServiceExtension		= 1,
	storeServiceUser			= 2,
	};

//	Fractal Texture Library ----------------------------------------------------

class CFractalTextureLibrary
	{
	public:
		enum ETextureTypes
			{
			typeNone,

			typeBoilingClouds,
			typeExplosion,
			};

		CFractalTextureLibrary (void) :
				m_bInitialized(false)
			{ }

		const CG8bitImage &GetTexture (ETextureTypes iType, int iFrame) const;
		int GetTextureCount (ETextureTypes iType) const;
		int GetTextureIndex (ETextureTypes iType, Metric rFraction) const
			{
			int iMaxFrames = GetTextureCount(iType);
			return Max(0, Min((int)(rFraction * iMaxFrames), iMaxFrames - 1));
			}

		void Init (void);

	private:
		void InitExplosionTextures (void);

		bool m_bInitialized;
		TArray<CG8bitImage> m_BoilingTextures;
		TArray<CG8bitImage> m_ExplosionTextures;
	};

//	Debug Options --------------------------------------------------------------

class CDebugOptions
	{
	public:
		ICCItemPtr GetProperty (const CString &sProperty) const;
		bool IsShowAIDebugEnbled (void) const { return m_bShowAIDebug; }
		bool IsShowBoundsEnabled (void) const { return m_bShowBounds; }
		bool IsShowFacingsAngleEnabled (void) const { return m_bShowFacingsAngle; }
		bool IsShowLineOfFireEnabled (void) const { return m_bShowLineOfFire; }
		bool IsShowNavPathsEnabled (void) const { return m_bShowNavPaths; }
		bool IsShowNodeAttributesEnabled (void) const { return m_bShowNodeAttributes; }
		bool IsVerboseCreate (void) const { return m_bVerboseCreate; }
		bool SetProperty (const CString &sProperty, ICCItem *pValue, CString *retsError = NULL);
		void SetVerboseCreate (bool bValue = true) { m_bVerboseCreate = bValue; }
		
	private:
		ICCItemPtr GetMemoryUse (void) const;

		bool m_bShowAIDebug = false;
		bool m_bShowBounds = false;
		bool m_bShowLineOfFire = false;
		bool m_bShowNavPaths = false;
		bool m_bShowFacingsAngle = false;
		bool m_bShowNodeAttributes = false;
		bool m_bVerboseCreate = false;
	};

//	SFX Options ----------------------------------------------------------------

class CSFXOptions
	{
	public:
		enum ESFXQuality
			{
			sfxUnknown			= -1,
			sfxMinimum			= 0,
			sfxStandard			= 1,
			sfxMaximum			= 2,
			};

		CSFXOptions (void) { SetSFXQuality(sfxMaximum); }

		BYTE GetHUDOpacity (void) const { return (m_bHUDTransparent ? 200 : 255); }
		bool Is3DSystemMapEnabled (void) const { return m_b3DSystemMap; }
		bool IsDockScreenTransparent (void) const { return m_bDockScreenTransparent; }
		bool IsManeuveringEffectEnabled (void) const { return m_bManeuveringEffect; }
		bool IsSpaceBackgroundEnabled (void) const { return m_bSpaceBackground; }
		bool IsStargateTravelEffectEnabled (void) const { return m_bStargateTravelEffect; }
		bool IsStarGlowEnabled (void) const { return m_bStarGlow; }
		bool IsStarshineEnabled (void) const { return m_bStarshine; }
		void Set3DSystemMapEnabled (bool bEnabled = true) { m_b3DSystemMap = bEnabled; }
		void SetManeuveringEffectEnabled (bool bEnabled = true) { m_bManeuveringEffect = bEnabled; }
		void SetSFXQuality (ESFXQuality iQuality);
		void SetSFXQualityAuto (void);

	private:
		ESFXQuality m_iQuality;

		bool m_b3DSystemMap;				//	3D effect on system map
		bool m_bHUDTransparent;				//	HUD has transparency effect
		bool m_bManeuveringEffect;			//	Show maneuvering thruster effects
		bool m_bSpaceBackground;			//	Show system image background
		bool m_bStargateTravelEffect;		//	Show effect when changing systems
		bool m_bStarGlow;					//	Show star glow in system map
		bool m_bStarshine;					//	Show starshine effect
		bool m_bDockScreenTransparent;		//	Show SRS behind dock screen
	};

//	Named Painters -------------------------------------------------------------

class CNamedEffects
	{
	public:
		enum ETypes
			{
			painterNone =					-1,

			painterMediumStationDamage =	0,
			painterLargeStationDamage =		1,

			painterCount =					2,
			};

		CNamedEffects (void) { }
		CNamedEffects (const CNamedEffects &Src) = delete;
		CNamedEffects (CNamedEffects &&Src);
		~CNamedEffects (void) { CleanUp(); }
		CNamedEffects &operator= (const CNamedEffects &Src) = delete;
		CNamedEffects &operator= (CNamedEffects &&Src);

		void CleanUp (void);
		CEffectCreator &GetFireEffect (CDesignCollection &Design, DamageTypes iDamage) const;
		CEffectCreator &GetHitEffect (CDesignCollection &Design, DamageTypes iDamage) const;
		CEffectCreator &GetNullEffect (CDesignCollection &Design) const;
		IEffectPainter &GetPainter (CDesignCollection &Design, ETypes iType) const;

	private:
		static const DWORD UNID_FIRST_DEFAULT_EFFECT =			0x00000010;
		static const DWORD UNID_FIRST_DEFAULT_FIRE_EFFECT =		0x00000070;
		static const DWORD UNID_BASIC_HIT_EFFECT =				0x00009003;
		static const DWORD UNID_NULL_EFFECT =					0x00030003;

		void Move (CNamedEffects &Src);

		mutable CEffectCreator *m_pDefaultFireEffect[damageCount] = { NULL };
		mutable CEffectCreator *m_pDefaultHitEffect[damageCount] = { NULL };
		mutable IEffectPainter *m_pNamedPainter[painterCount] = { NULL };
		mutable CEffectCreator *m_pNullEffect = NULL;

		static DWORD m_NamedPainterUNID[painterCount];
	};

//	The Universe ---------------------------------------------------------------
//
//	CREATING THE UNIVERSE
//
//	1.	Call Init() to initialize the universe with the appropriate adventure
//		and extensions. This will call Bind on all design types and initialize
//		any dynamic types.
//		[This may take a while, so it should be called on a background thread.]
//
//	2.	Call InitAdventure(), which calls OnGameStart on the adventure. This is
//		required if you need to show the crawl screen and text, otherwise it may
//		be skipped (e.g., for TransData).
//		[This may be called synchronously.]
///
//	2.	Call InitGame() to initialize the topology, etc.
//		[This should be called on a background thread.]
//
//	3.	Call StartGame(true) to start the game running.
//		[This may be called	synchronously.]

class CUniverse
	{
	public:
		class IHost
			{
			public:
				virtual void ConsoleOutput (const CString &sLine) { }
				virtual IPlayerController *CreatePlayerController (void) { return NULL; }
				virtual IShipController *CreateShipController (const CString &sController) { return NULL; }
				virtual void DebugOutput (const CString &sLine) { }
				virtual bool FindFont (const CString &sFont, const CG16bitFont **retpFont = NULL) const { return false; }
				virtual void GameOutput (const CString &sLine) { }
				virtual CG32bitPixel GetColor (const CString &sColor) const { return CG32bitPixel(255, 255, 255); }
				virtual const CG16bitFont &GetFont (const CString &sFont) const { const CG16bitFont *pFont; if (!FindFont(sFont, &pFont)) return CG16bitFont::GetDefault(); return *pFont; }
				virtual void LogOutput (const CString &sLine) const { ::kernelDebugLogString(sLine); }
			};

		class INotifications
			{
			public:
				virtual void OnObjDestroyed (SDestroyCtx &Ctx) { }
			};

		struct SInitDesc
			{
			IHost *pHost = NULL;						//	Host
			TArray<SPrimitiveDefTable> CCPrimitives;	//	Additional primitives to define
			CString sFilespec;							//	Filespec of main TDB/XML file.
			CString sSourceFilespec;					//	Filespec of main source XML file (for debugging).
			CString sCollectionFolder;					//	If non-blank, use this as Collection folder (and remember it)
			TArray<CString> ExtensionFolders;			//	Add these as extension folders.
			TSortMap<DWORD, bool> TypesUsed;			//	List of types used (bind these, even if obsolete).
			TSortMap<DWORD, bool> DisabledExtensions;	//	Collection Extensions disabled locally by user
			DWORD dwMinAPIVersion = API_VERSION;		//	Min API version (for compatibility)

			//	Options

			bool bInLoadGame = false;					//	We're loading a game, which means don't reset dynamic types
			bool bNoResources = false;					//	If TRUE, do not bother loaded images
			bool bNoReload = false;						//	If TRUE, do not reload extensions
			bool bDebugMode = false;					//	Initialize in debug mode
			bool bDiagnostics = false;					//	If TRUE, we allow the diagnostics extension
			bool bDefaultExtensions = false;			//	If TRUE, we include all appropriate extensions
			bool bForceTDB = false;						//	If TRUE, use Transcendence.tdb even if XMLs exist
			bool bNoCollectionLoad = false;				//  If TRUE, don't load collection (only in debug mode)
			bool bNoCollectionCheck = false;			//  If TRUE, don't check collection digests
			bool bForceCompatibilityLibrary = false;	//	If TRUE, force include of Compatibility Library
			bool bRecoverAdventureError = false;		//	If TRUE, we try to recover if given adventure not found
			bool bVerboseCreate = false;				//	If TRUE, log debug info on universe creation

			//	Adventure to bind to (choose one, in order)

			CExtension *pAdventure = NULL;				//	If not NULL, bind to this adventure
			DWORD dwAdventure = 0;						//	It not 0, bind to this adventure
			CString sAdventureFilespec;					//	If not empty, bind to this adventure
			TArray<DWORD> ExtensionUNIDs;				//	Extensions to use

			//	Extension list

			TArray<CExtension *> Extensions;
			};

		enum ENamedFonts
			{
			fontMapLabel =				0,	//	Font for map labels
			fontSign =					1,	//	Font for signs
			fontSRSObjName =			2,	//	Font for name of objects in SRS
			fontSRSMessage =			3,	//	Font for comms messages from objects in SRS
			fontSRSObjCounter =			4,	//	Font for counter labels

			fontCount =					5,
			};

		CUniverse (void);
		virtual ~CUniverse (void);

		static void Boot (void);
		ALERROR Init (SInitDesc &Ctx, CString *retsError);
		ALERROR InitAdventure (IPlayerController *pPlayer, CString *retsError);
		ALERROR InitGame (DWORD dwStartingMap, CString *retsError);
		ALERROR LoadFromStream (IReadStream *pStream, DWORD *retdwSystemID, DWORD *retdwPlayerID, CString *retsError);
		void StartGame (bool bNewGame);

		void AddAscendedObj (CSpaceObject *pObj) { m_AscendedObjects.Insert(pObj); }
		ALERROR AddDynamicType (CExtension *pExtension, DWORD dwUNID, ICCItem *pSource, bool bNewGame, CString *retsError) { return m_Design.AddDynamicType(pExtension, dwUNID, pSource, bNewGame, retsError); }
		void AddEvent (CSystemEvent *pEvent);
		void AddTimeDiscontinuity (const CTimeSpan &Duration) { m_Time.AddDiscontinuity(m_iTick++, Duration); }
		ALERROR AddStarSystem (CTopologyNode *pTopology, CSystem *pSystem);
		bool CancelEvent (CSpaceObject *pObj, bool bInDoEvent = false) { return m_Events.CancelEvent(pObj, bInDoEvent); }
		bool CancelEvent (CSpaceObject *pObj, const CString &sEvent, bool bInDoEvent = false) { return m_Events.CancelEvent(pObj, sEvent, bInDoEvent); }
		bool CancelEvent (CDesignType *pType, const CString &sEvent, bool bInDoEvent = false) { return m_Events.CancelEvent(pType, sEvent, bInDoEvent); }
		ALERROR CreateEmptyStarSystem (CSystem **retpSystem);
		DWORD CreateGlobalID (void) { return m_dwNextID++; }
		ALERROR CreateMission (CMissionType *pType, CSpaceObject *pOwner, ICCItem *pCreateData, CMission **retpMission, CString *retsError);
		ALERROR CreateRandomItem (const CItemCriteria &Crit, 
								  const CString &sLevelFrequency,
								  CItem *retItem);
		ALERROR CreateRandomMission (const TArray<CMissionType *> &Types, CSpaceObject *pOwner, ICCItem *pCreateData, CMission **retpMission, CString *retsError);
		IShipController *CreateShipController (const CString &sAI);
		ALERROR CreateStarSystem (const CString &sNodeID, CSystem **retpSystem, CString *retsError = NULL, CSystemCreateStats *pStats = NULL);
		ALERROR CreateStarSystem (CTopologyNode *pTopology, CSystem **retpSystem, CString *retsError = NULL, CSystemCreateStats *pStats = NULL);
		void DestroySystem (CSystem *pSystem);
		bool FindFont (const CString &sFont, const CG16bitFont **retpFont = NULL) const { return m_pHost->FindFont(sFont, retpFont); }
		CMission *FindMission (DWORD dwID) const { return m_AllMissions.GetMissionByID(dwID); }
		CSpaceObject *FindObject (DWORD dwID);
		void FireOnGlobalIntroCommand (const CString &sCommand) { m_Design.FireOnGlobalIntroCommand(sCommand); }
		void FireOnGlobalIntroStarted (void) { m_Design.FireOnGlobalIntroStarted(); }
		void FireOnGlobalPaneInit (void *pScreen, CDesignType *pRoot, const CString &sScreen, const CString &sPane, ICCItem *pData) { m_Design.FireOnGlobalPaneInit(pScreen, pRoot, sScreen, sPane, pData); }
		void FireOnGlobalPlayerBoughtItem (CSpaceObject *pSellerObj, const CItem &Item, const CCurrencyAndValue &Price) { m_Design.FireOnGlobalPlayerBoughtItem(pSellerObj, Item, Price); }
		void FireOnGlobalPlayerChangedShips (CSpaceObject *pOldShip) { m_Design.FireOnGlobalPlayerChangedShips(pOldShip); }
		void FireOnGlobalPlayerEnteredSystem (void) { m_Design.FireOnGlobalPlayerEnteredSystem(); }
		void FireOnGlobalPlayerLeftSystem (void) { m_Design.FireOnGlobalPlayerLeftSystem(); }
		void FireOnGlobalPlayerSoldItem (CSpaceObject *pBuyerObj, const CItem &Item, const CCurrencyAndValue &Price) { m_Design.FireOnGlobalPlayerSoldItem(pBuyerObj, Item, Price); }
		void FireOnGlobalSystemCreated (SSystemCreateCtx &SysCreateCtx) { m_Design.FireOnGlobalSystemCreated(SysCreateCtx); }
		void FireOnGlobalUniverseCreated (void) { m_Design.FireOnGlobalUniverseCreated(); }
		void FireOnGlobalUniverseLoad (void) { m_Design.FireOnGlobalUniverseLoad(); }
		void FireOnGlobalUniverseSave (void) { m_Design.FireOnGlobalUniverseSave(); }
		void FlushStarSystem (CTopologyNode *pTopology);
		void GenerateGameStats (CGameStats &Stats);
		void GetAllAdventures (TArray<CExtension *> *retList) { CString sError; m_Extensions.ComputeAvailableAdventures((m_bDebugMode ? CExtensionCollection::FLAG_DEBUG_MODE : 0), retList, &sError); }
		const CDamageAdjDesc *GetArmorDamageAdj (int iLevel) const;
		CAscendedObjectList &GetAscendedObjects (void) { return m_AscendedObjects; }
		CG32bitPixel GetColor (const CString &sColor) const { return m_pHost->GetColor(sColor); }
		CAdventureDesc &GetCurrentAdventureDesc (void) { return *m_pAdventure; }
		void GetCurrentAdventureExtensions (TArray<DWORD> *retList);
		CMission *GetCurrentMission (void);
		const CDisplayAttributeDefinitions &GetAttributeDesc (void) const { return m_Design.GetDisplayAttributes(); }
		const CEconomyType &GetCreditCurrency (void) const;
		const CDebugOptions &GetDebugOptions (void) const { return m_DebugOptions; }
		ICCItemPtr GetDebugProperty (const CString &sProperty) const { return m_DebugOptions.GetProperty(sProperty); }
		const CEconomyType &GetDefaultCurrency (void) const { return (m_pAdventure ? m_pAdventure->GetDefaultCurrency() : GetCreditCurrency()); }
		CEffectCreator &GetDefaultFireEffect (DamageTypes iDamage);
		CEffectCreator &GetDefaultHitEffect (DamageTypes iDamage);
		CDockSession &GetDockSession (void) { return m_DockSession; }
		const CDockSession &GetDockSession (void) const { return m_DockSession; }
		CGImageCache &GetDynamicImageLibrary (void) { return m_DynamicImageLibrary; }
		CTimeSpan GetElapsedGameTime (void) { return m_Time.GetElapsedTimeAt(m_iTick); }
		CTimeSpan GetElapsedGameTimeAt (int iTick) { return m_Time.GetElapsedTimeAt(iTick); }
		CExtensionCollection &GetExtensionCollection (void) { return m_Extensions; }
		CString GetExtensionData (EStorageScopes iScope, DWORD dwExtension, const CString &sAttrib);
		CTopologyNode *GetFirstTopologyNode (void);
		const CG16bitFont &GetFont (const CString &sFont) const { return m_pHost->GetFont(sFont); }
		CFractalTextureLibrary &GetFractalTextureLibrary (void) { return m_FractalTextureLibrary; }
        CObjectTracker &GetGlobalObjects (void) { return m_Objects; }
        const CObjectTracker &GetGlobalObjects (void) const { return m_Objects; }
		IHost *GetHost (void) const { return m_pHost; }
		CMission *GetMission (int iIndex) { return m_AllMissions.GetMission(iIndex); }
		int GetMissionCount (void) const { return m_AllMissions.GetCount(); }
		CMissionList &GetMissions (void) { return m_AllMissions; }
		void GetMissions (CSpaceObject *pSource, const CMission::SCriteria &Criteria, TArray<CMission *> *retList);
		const CG16bitFont &GetNamedFont (ENamedFonts iFont) { return *m_FontTable[iFont]; }
		IEffectPainter &GetNamedPainter (CNamedEffects::ETypes iPainter) { return m_NamedEffects.GetPainter(m_Design, iPainter); }
		const CObjectStats::SEntry &GetObjStats (DWORD dwObjID) const { return m_ObjStats.GetEntry(dwObjID); }
		CObjectStats::SEntry &GetObjStatsActual (DWORD dwObjID) { return m_ObjStats.GetEntryActual(dwObjID); }
		ICCItemPtr GetProperty (CCodeChainCtx &Ctx, const CString &sProperty);
		void GetRandomLevelEncounter (int iLevel, CDesignType **retpType, IShipGenerator **retpTable, CSovereign **retpBaseSovereign);
		CString GetResourceDb (void) { return m_sResourceDb; }
		CCriticalSection &GetSem (void) { return m_cs; }
		CSFXOptions &GetSFXOptions (void) { return m_SFXOptions; }
		const CDamageAdjDesc *GetShieldDamageAdj (int iLevel) const;
		bool GetSound (void) { return !m_bNoSound; }
		CSoundMgr *GetSoundMgr (void) { return m_pSoundMgr; }
		bool InDebugMode (void) { return m_bDebugMode; }
		void InitEntityResolver (CExtension *pExtension, CEntityResolverList *retResolver) { m_Extensions.InitEntityResolver(pExtension, (InDebugMode() ? CExtensionCollection::FLAG_DEBUG_MODE : 0), retResolver); }
		bool InResurrectMode (void) { return m_bResurrectMode; }
		bool IsGlobalResurrectPending (CDesignType **retpType);
		bool IsRegistered (void) { return m_bRegistered; }
		bool IsStatsPostingEnabled (void);
		void NotifyOnObjDeselected (CSpaceObject *pObj) { pObj->FireOnDeselected(); }
		void NotifyOnObjDestroyed (SDestroyCtx &Ctx);
		void NotifyOnObjSelected (CSpaceObject *pObj) { pObj->FireOnSelected(); }
		void NotifyOnPlayerEnteredGate (CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate);
		ALERROR LoadNewExtension (const CString &sFilespec, const CIntegerIP &FileDigest, CString *retsError) { return m_Extensions.LoadNewExtension(sFilespec, FileDigest, retsError); }
		bool LogImageLoad (void) const { return (m_iLogImageLoad == 0); }
		void LogOutput (const CString &sLine) const { m_pHost->LogOutput(sLine); }
		void PlaySound (CSpaceObject *pSource, int iChannel);
		void PutPlayerInSystem (CShip *pPlayerShip, const CVector &vPos, CSystemEventList &SavedEvents);
		void RefreshCurrentMission (void);
		void RegisterForNotifications (INotifications *pSubscriber) { m_Subscribers.Insert(pSubscriber); }
		ALERROR Reinit (void);
		CSpaceObject *RemoveAscendedObj (DWORD dwObjID) { return m_AscendedObjects.RemoveByID(dwObjID); }
		ALERROR SaveDeviceStorage (void);
		ALERROR SaveToStream (IWriteStream *pStream);
		void SetCurrentSystem (CSystem *pSystem);
		void SetDebugMode (bool bDebug = true) { m_bDebugMode = bDebug; }
		bool SetDebugProperty (const CString &sProperty, ICCItem *pValue, CString *retsError = NULL) { return m_DebugOptions.SetProperty(sProperty, pValue, retsError); }
		bool SetExtensionData (EStorageScopes iScope, DWORD dwExtension, const CString &sAttrib, const CString &sData);
		void SetNewSystem (CSystem *pSystem, CSpaceObject *pPOV);
		bool SetPOV (CSpaceObject *pPOV);
		void SetPlayerShip (CSpaceObject *pPlayer);
		void SetRegistered (bool bRegistered = true) { m_bRegistered = bRegistered; }
		void SetResurrectMode (bool bResurrect = true) { m_bResurrectMode = bResurrect; }
		void SetSound (bool bSound = true) { m_bNoSound = !bSound; }
		void SetSoundMgr (CSoundMgr *pSoundMgr) { m_pSoundMgr = pSoundMgr; }
		void StartGameTime (void);
		CTimeSpan StopGameTime (void);
		void UnregisterForNotifications (INotifications *pSubscriber) { m_Subscribers.DeleteValue(pSubscriber); }
		static CString ValidatePlayerName (const CString &sName);

		CArmorClass *FindArmor (DWORD dwUNID);
		CCompositeImageType *FindCompositeImageType (DWORD dwUNID) { return CCompositeImageType::AsType(m_Design.FindEntry(dwUNID)); }
		const CDesignType *FindDesignType (DWORD dwUNID) const { return m_Design.FindEntry(dwUNID); }
		CDesignType *FindDesignType (DWORD dwUNID) { return m_Design.FindEntry(dwUNID); }
		CDeviceClass *FindDeviceClass (DWORD dwUNID);
		const CEconomyType *FindEconomyType (const CString &sName) { return m_Design.FindEconomyType(sName); }
		CEffectCreator *FindEffectType (DWORD dwUNID) { return CEffectCreator::AsType(m_Design.FindEntry(dwUNID)); }
		CShipTable *FindEncounterTable (DWORD dwUNID) { return CShipTable::AsType(m_Design.FindEntry(dwUNID)); }
		bool FindExtension (DWORD dwUNID, DWORD dwRelease, CExtension **retpExtension = NULL) { return m_Extensions.FindBestExtension(dwUNID, dwRelease, (InDebugMode() ? CExtensionCollection::FLAG_DEBUG_MODE : 0), retpExtension); }
		CGenericType *FindGenericType (DWORD dwUNID) { return CGenericType::AsType(m_Design.FindEntry(dwUNID)); }
		CItemTable *FindItemTable (DWORD dwUNID) { return CItemTable::AsType(m_Design.FindEntry(dwUNID)); }
		CItemType *FindItemType (DWORD dwUNID) { return CItemType::AsType(m_Design.FindEntry(dwUNID)); }
		CMissionType *FindMissionType (DWORD dwUNID) { return CMissionType::AsType(m_Design.FindEntry(dwUNID)); }
		COverlayType *FindOverlayType(DWORD dwUNID) { return COverlayType::AsType(m_Design.FindEntry(dwUNID)); }
		CPower *FindPower (DWORD dwUNID) { return CPower::AsType(m_Design.FindEntry(dwUNID)); }
		CShipClass *FindShipClass (DWORD dwUNID) { return CShipClass::AsType(m_Design.FindEntry(dwUNID)); }
		CShipClass *FindShipClassByName (const CString &sName);
		COverlayType *FindShipEnergyFieldType(DWORD dwUNID) { return COverlayType::AsType(m_Design.FindEntry(dwUNID)); }
		int FindSound (DWORD dwUNID) { CSoundResource *pSound = FindSoundResource(dwUNID); return (pSound ? pSound->GetSound() : -1); }
		CMusicResource *FindMusicResource (DWORD dwUNID) { return CMusicResource::AsType(m_Design.FindEntry(dwUNID)); }
		CSoundResource *FindSoundResource (DWORD dwUNID) { return CSoundResource::AsType(m_Design.FindEntry(dwUNID)); }
		const CSovereign *FindSovereign (DWORD dwUNID) const { return CSovereign::AsType(m_Design.FindEntry(dwUNID)); }
		CSovereign *FindSovereign (DWORD dwUNID) { return CSovereign::AsType(m_Design.FindEntry(dwUNID)); }
		CSpaceEnvironmentType *FindSpaceEnvironment (DWORD dwUNID) { return CSpaceEnvironmentType::AsType(m_Design.FindEntry(dwUNID)); }
		CStationType *FindStationType (DWORD dwUNID) { return CStationType::AsType(m_Design.FindEntry(dwUNID)); }
		CXMLElement *FindSystemFragment (const CString &sName, CSystemTable **retpTable = NULL) { return m_Design.FindSystemFragment(sName, retpTable); }
		CSystemType *FindSystemType (DWORD dwUNID) { return CSystemType::AsType(m_Design.FindEntry(dwUNID)); }
		CTopologyNode *FindTopologyNode (const CString &sName) { return m_Topology.FindTopologyNode(sName); }
		CWeaponFireDesc *FindWeaponFireDesc (const CString &sName);
		CCodeChain &GetCC (void) { return m_CC; }
		CTopologyNode *GetCurrentTopologyNode (void) { return (m_pCurrentSystem ? m_pCurrentSystem->GetTopology() : NULL); }
		CSystem *GetCurrentSystem (void) { return m_pCurrentSystem; }
		IPlayerController::EUIMode GetCurrentUIMode (void) const { return (m_pPlayer ? m_pPlayer->GetUIMode() : IPlayerController::uimodeUnknown); }
		int GetPaintTick (void) { return m_iPaintTick; }
		CSpaceObject *GetPOV (void) const { return m_pPOV; }
		IPlayerController *GetPlayer (void) const { return m_pPlayer; }
		GenomeTypes GetPlayerGenome (void) const;
		CString GetPlayerName (void) const;
		CSpaceObject *GetPlayerShip (void) const { return m_pPlayerShip; }
		CSovereign *GetPlayerSovereign (void) const;
		DWORD GetTicks (void) { return (DWORD)m_iTick; }

		void ClearLibraryBitmapMarks (void) { m_Design.ClearImageMarks(); m_DynamicImageLibrary.ClearMarks();  }
		void GarbageCollectLibraryBitmaps (void);
		TSharedPtr<CObjectImage> FindLibraryImage (DWORD dwUNID) { CObjectImage *pImage = CObjectImage::AsType(m_Design.FindEntry(dwUNID)); return TSharedPtr<CObjectImage>(pImage ? pImage->AddRef() : NULL); }
		CG32bitImage *GetLibraryBitmap (DWORD dwUNID, DWORD dwFlags = 0) { return m_Design.GetImage(dwUNID, dwFlags); }
		CG32bitImage *GetLibraryBitmapCopy (DWORD dwUNID) { return m_Design.GetImage(dwUNID, CDesignCollection::FLAG_IMAGE_COPY); }
		void MarkLibraryBitmaps (void) { m_Design.MarkGlobalImages(); if (m_pCurrentSystem) m_pCurrentSystem->MarkImages(); }
		void SweepLibraryBitmaps (void) { m_Extensions.SweepImages(); m_Design.SweepImages(); m_DynamicImageLibrary.Sweep();  }

		CDesignCollection &GetDesignCollection (void) { return m_Design; }
		CDesignType *GetDesignType (int iIndex) { return m_Design.GetEntry(iIndex); }
		int GetDesignTypeCount (void) { return m_Design.GetCount(); }
		const CExtension *GetExtensionDesc (int iIndex) { return m_Design.GetExtension(iIndex); }
		int GetExtensionDescCount (void) { return m_Design.GetExtensionCount(); }
		CItemType *GetItemType (int iIndex) { return (CItemType *)m_Design.GetEntry(designItemType, iIndex); }
		int GetItemTypeCount (void) { return m_Design.GetCount(designItemType); }
		CPower *GetPower (int iIndex) { return (CPower *)m_Design.GetEntry(designPower, iIndex); }
		int GetPowerCount (void) { return m_Design.GetCount(designPower); }
		CShipClass *GetShipClass (int iIndex) { return (CShipClass *)m_Design.GetEntry(designShipClass, iIndex); }
		int GetShipClassCount (void) { return m_Design.GetCount(designShipClass); }
		CMusicResource *GetMusicResource (int iIndex) const { return (CMusicResource *)m_Design.GetEntry(designMusic, iIndex); }
		int GetMusicResourceCount (void) const { return m_Design.GetCount(designMusic); }
		CSovereign *GetSovereign (int iIndex) const { return (CSovereign *)m_Design.GetEntry(designSovereign, iIndex); }
		int GetSovereignCount (void) { return m_Design.GetCount(designSovereign); }
		CStationType *GetStationType (int iIndex) { return (CStationType *)m_Design.GetEntry(designStationType, iIndex); }
		int GetStationTypeCount (void) { return m_Design.GetCount(designStationType); }
		CTopology &GetTopology (void) { return m_Topology; }
		const CTopology &GetTopology (void) const { return m_Topology; }
		CTopologyNode *GetTopologyNode (int iIndex) { return m_Topology.GetTopologyNode(iIndex); }
		int GetTopologyNodeCount (void) const { return m_Topology.GetTopologyNodeCount(); }

		void PaintObject (CG32bitImage &Dest, const RECT &rcView, CSpaceObject *pObj);
		void PaintObjectMap (CG32bitImage &Dest, const RECT &rcView, CSpaceObject *pObj);
		void PaintPOV (CG32bitImage &Dest, const RECT &rcView, DWORD dwFlags);
		void PaintPOVLRS (CG32bitImage &Dest, const RECT &rcView, Metric rScale, DWORD dwFlags, bool *retbNewEnemies = NULL);
		void PaintPOVMap (CG32bitImage &Dest, const RECT &rcView, Metric rMapScale);
		void SetLogImageLoad (bool bLog = true) { CSmartLock Lock(m_cs); m_iLogImageLoad += (bLog ? -1 : +1); }
		void Update (SSystemUpdateCtx &Ctx);
		void UpdateExtended (void);

		void DebugOutput (char *pszLine, ...);

	private:
		struct SLevelEncounter
			{
			CStationType *pType;
			int iWeight;
			CSovereign *pBaseSovereign;
			IShipGenerator *pTable;
			};

		struct STransSystemObject
			{
			CSpaceObject *pObj;
			CTopologyNode *pDest;
			int iArrivalTime;

			STransSystemObject *pNext;
			};

		bool FindByUNID (CIDTable &Table, DWORD dwUNID, CObject **retpObj = NULL);
		CObject *FindByUNID (CIDTable &Table, DWORD dwUNID);
		ALERROR InitCodeChain (const TArray<SPrimitiveDefTable> &CCPrimitives);
		ALERROR InitCodeChainPrimitives (void);
		ALERROR InitDeviceStorage (CString *retsError);
		ALERROR InitFonts (void);
		ALERROR InitLevelEncounterTables (void);
		ALERROR InitRequiredEncounters (CString *retsError);
		ALERROR InitTopology (DWORD dwStartingMap, CString *retsError);
		void SetHost (IHost *pHost);
		void SetPlayer (IPlayerController *pPlayer);
		void UpdateMissions (int iTick, CSystem *pSystem);
		void UpdateSovereigns (int iTick, CSystem *pSystem);

		//	Design data

		CExtensionCollection m_Extensions;		//	Loaded extensions
		CDesignCollection m_Design;				//	Design collection
		CDeviceStorage m_DeviceStorage;			//	Local cross-game storage

		CString m_sResourceDb;					//	Resource database

		TArray<TArray<SLevelEncounter>> m_LevelEncounterTables;	//	Array of SLevelEncounter arrays
		bool m_bBasicInit;						//	TRUE if we've initialized CodeChain, etc.

		//	Game instance data

		bool m_bRegistered;						//	If TRUE, this is a registered game
		bool m_bResurrectMode;					//	If TRUE, this session is a game resurrect
		int m_iTick;							//	Ticks since beginning of time
		int m_iPaintTick;						//	Advances only when we paint a frame
		CGameTimeKeeper m_Time;					//	Game time tracker
		CAdventureDesc *m_pAdventure;			//	Current adventure
		CSpaceObject *m_pPOV;					//	Point of view
		IPlayerController *m_pPlayer;			//	Player controller
		CSpaceObject *m_pPlayerShip;			//	Player ship
		CSystem *m_pCurrentSystem;				//	Current star system (used by code)
		TSortMap<DWORD, CSystem *> m_StarSystems;	//	Array of CSystem (indexed by ID)
		CTimeDate m_StartTime;					//	Time when we started the game
		DWORD m_dwNextID;						//	Next universal ID
		CTopology m_Topology;					//	Array of CTopologyNode
		CAscendedObjectList m_AscendedObjects;	//	Ascended objects (objects not in any system)
		CMissionList m_AllMissions;				//	List of all missions
		CSystemEventList m_Events;				//	List of all global events
		CObjectTracker m_Objects;				//	Objects across all systems
		CObjectStats m_ObjStats;				//	Object stats (across all systems)

		CDockSession m_DockSession;

		//	Support structures

		CCriticalSection m_cs;
		IHost *m_pHost;
		CCodeChain m_CC;
		ICCItem *m_pSavedGlobalSymbols;
		CSoundMgr *m_pSoundMgr;
		const CG16bitFont *m_FontTable[fontCount];
		CG16bitFont m_DefaultFonts[fontCount];
		TArray<INotifications *> m_Subscribers;
		CSFXOptions m_SFXOptions;
		CDebugOptions m_DebugOptions;
		CFractalTextureLibrary m_FractalTextureLibrary;
		CGImageCache m_DynamicImageLibrary;
		SViewportAnnotations m_ViewportAnnotations;
		CAdventureDesc m_EmptyAdventure;

		//	Cached singletons

		mutable const CEconomyType *m_pCreditCurrency = NULL;
		CNamedEffects m_NamedEffects;

		//	Debugging structures

		bool m_bDebugMode;
		bool m_bNoSound;
		int m_iLogImageLoad;					//	If >0 we disable image load logging
	};

