//	TSEShipClass.h
//
//	Defines classes and interfaces for ship classes.
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CShipClass : public CDesignType
	{
	public:
		enum EBalanceTypes
			{
			typeUnknown,

			typeMinion,
			typeStandard,
			typeElite,
			typeBoss,
			typeNonCombatant,

			typeTooWeak,
			typeTooStrong,
			typeArmorTooWeak,
			typeArmorTooStrong,
			typeWeaponsTooWeak,
			typeWeaponsTooStrong,
			};

		enum VitalSections
			{
			sectNonCritical	= 0x00000000,		//	Ship is not automatically
												//	destroyed (chance of being
												//	destroyed based on the power
												//	of the blast relative to the
												//	original armor HPs)

			//	NOTE: These must match the order in CShipClass.cpp (ParseNonCritical)

			sectDevice0		= 0x00000001,
			sectDevice1		= 0x00000002,
			sectDevice2		= 0x00000004,
			sectDevice3		= 0x00000008,
			sectDevice4		= 0x00000010,
			sectDevice5		= 0x00000020,
			sectDevice6		= 0x00000040,
			sectDevice7		= 0x00000080,
			sectDeviceMask	= 0x000000ff,

			sectManeuver	= 0x00000100,		//	Maneuvering damaged (quarter speed turn)
			sectDrive		= 0x00000200,		//	Drive damaged (half-speed)
			sectScanners	= 0x00000400,		//	Unable to target
			sectTactical	= 0x00000800,		//	Unable to fire weapons
			sectCargo		= 0x00001000,		//	Random cargo destroyed

			sectCritical	= 0x00010000,		//	Ship destroyed
			};

		CShipClass (void);
		virtual ~CShipClass (void);

		inline int Angle2Direction (int iAngle) const { return m_Perf.GetRotationDesc().GetFrameIndex(iAngle); }
		inline int AlignToRotationAngle (int iAngle) const { return m_Perf.GetRotationDesc().AlignToRotationAngle(iAngle); }
		int CalcArmorSpeedBonus (int iTotalArmorMass) const;
		inline int CalcImageSize (void) const { return m_Interior.CalcImageSize(const_cast<CShipClass *>(this)); }
		Metric CalcMass (const CDeviceDescList &Devices) const;
		int CalcScore (void);
		bool CreateEmptyWreck (CSystem *pSystem, CShip *pShip, const CVector &vPos, const CVector &vVel, CSovereign *pSovereign, CStation **retpWreck);
		void CreateExplosion (CShip *pShip, CSpaceObject *pWreck = NULL);
		void CreateImage (CG32bitImage &Dest, int iTick, int iRotation, Metric rScale = 1.0);
		void CreateScaledImage (CG32bitImage &Dest, int iTick, int iRotation, int cxWidth, int cyHeight);
		bool CreateWreck (CShip *pShip, CSpaceObject **retpWreck = NULL);
		inline bool FindDeviceSlotDesc (DeviceNames iDev, SDeviceDesc *retDesc) { return (m_pDevices ? m_pDevices->FindDefaultDesc(iDev, retDesc) : false); }
		inline bool FindDeviceSlotDesc (const CItem &Item, SDeviceDesc *retDesc) { return (m_pDevices ? m_pDevices->FindDefaultDesc(Item, retDesc) : false); }
		void GenerateDevices (int iLevel, CDeviceDescList &Devices);
		CString GenerateShipName (DWORD *retdwFlags) const;
		inline const CAISettings &GetAISettings (void) { return m_AISettings; }
		inline const CItemCriteria &GetArmorCriteria (void) const { return m_ArmorCriteria; }
        inline const CShipArmorDesc &GetArmorDesc (void) const { return m_Armor; }
        const CCargoDesc &GetCargoDesc (const CItem **retpCargoItem = NULL) const;
		inline CGenericType *GetCharacter (void) { return m_Character; }
		inline CGenericType *GetCharacterClass (void) { return m_CharacterClass; }
		inline Metric GetCombatStrength (void) const { return m_rCombatStrength; }
		inline int GetCyberDefenseLevel (void) { return m_iCyberDefenseLevel; }
		inline DWORD GetDefaultBkgnd (void) { return m_dwDefaultBkgnd; }
		inline CSovereign *GetDefaultSovereign (void) const { return m_pDefaultSovereign; }
		inline const CItemCriteria &GetDeviceCriteria (void) const { return m_DeviceCriteria; }
		inline const CDockingPorts &GetDockingPorts (void) { return m_DockingPorts; }
		CVector GetDockingPortOffset (int iRotation);
        const CDriveDesc &GetDriveDesc (const CItem **retpDriveItem = NULL) const;
		inline const CObjectEffectDesc &GetEffectsDesc (void) const { return m_Effects; }
		IShipGenerator *GetEscorts (void) { return m_pEscorts; }
		CWeaponFireDesc *GetExplosionType (CShip *pShip) const;
		inline CXMLElement *GetFirstDockScreen (void) { return m_pDefaultScreen.GetDesc(); }
		inline CDesignType *GetFirstDockScreen (CString *retsName) { return m_pDefaultScreen.GetDockScreen(this, retsName); }
        const CObjectImageArray &GetHeroImage (void);
		inline int GetHullCargoSpace (void) { return m_CargoDesc.GetCargoSpace(); }
		inline int GetHullMass (void) const { return m_iMass; }
		inline const CReactorDesc *GetHullReactorDesc (void) { return &m_ReactorDesc; }
		inline const CShipArmorSegmentDesc &GetHullSection (int iIndex) const { return m_Armor.GetSegment(iIndex); }
		int GetHullSectionAtAngle (int iAngle);
		inline int GetHullSectionCount (void) const { return m_Armor.GetCount(); }
		CString GetHullSectionName (int iIndex) const;
		inline const CCurrencyAndValue &GetHullValue (void) const { return m_HullValue; }
		inline const CObjectImageArray &GetImage (void) const { return m_Image.GetSimpleImage(); }
        inline const CAttributeDataBlock &GetInitialData (void) const { return m_InitialData; }
		inline const CShipInteriorDesc &GetInteriorDesc (void) const { return m_Interior; }
		inline int GetMaxArmorMass (void) const { return m_iMaxArmorMass; }
		inline int GetMaxCargoSpace (void) const { return m_iMaxCargoSpace; }
		inline int GetMaxDevices (void) const { return m_iMaxDevices; }
		inline int GetMaxNonWeapons (void) const { return m_iMaxNonWeapons; }
		inline int GetMaxReactorPower (void) const { return m_iMaxReactorPower; }
		int GetMaxStructuralHitPoints (void) const;
		inline int GetMaxWeapons (void) const { return m_iMaxWeapons; }
        const CPlayerSettings *GetPlayerSettings (void) const;
		CString GetPlayerSortString (void) const;
		CVector GetPosOffset (int iAngle, int iRadius, int iPosZ, bool b3DPos = true);
		inline IItemGenerator *GetRandomItemTable (void) const { return m_pItems; }
        const CReactorDesc &GetReactorDesc (const CItem **retpReactorItem = NULL) const;
		inline int GetRotationAngle (void) { return m_Perf.GetRotationDesc().GetFrameAngle(); }
		inline const CIntegralRotationDesc &GetRotationDesc (void) const { return m_Perf.GetRotationDesc(); }
		inline int GetRotationRange (void) { return m_Perf.GetRotationDesc().GetFrameCount(); }
		inline int GetScore (void) { return m_iScore; }
		inline DWORD GetShipNameFlags (void) { return m_dwShipNameFlags; }
		CString GetShortName (void) const;
		inline int GetSize (void) const { return m_iSize; }
		inline const CString &GetClassName (void) const { return m_sName; }
		inline const CString &GetManufacturerName (void) const { return m_sManufacturer; }
		inline const CString &GetShipTypeName (void) const { return m_sTypeName; }
		inline int GetWreckChance (void) { return m_iLeavesWreck; }
		CObjectImageArray &GetWreckImage (void) { if (!m_WreckImage.IsLoaded()) CreateWreckImage(); return m_WreckImage; }
		void GetWreckImage (CObjectImageArray *retWreckImage);
		int GetWreckImageVariants (void);
		inline bool HasDockingPorts (void) { return (m_fHasDockingPorts ? true : false); }
		inline bool HasShipName (void) const { return !m_sShipNames.IsBlank(); }
		void InitEffects (CShip *pShip, CObjectEffectList *retEffects);
        void InitPerformance (SShipPerformanceCtx &Ctx) const;
		void InstallEquipment (CShip *pShip);
        inline bool IsDebugOnly (void) const { const CPlayerSettings *pPlayerSettings = GetPlayerSettings(); return (pPlayerSettings && pPlayerSettings->IsDebugOnly()); }
		inline bool IsIncludedInAllAdventures (void) { const CPlayerSettings *pPlayerSettings = GetPlayerSettings(); return (pPlayerSettings && pPlayerSettings->IsIncludedInAllAdventures()); }
		inline bool IsPlayerShip (void) { return (GetPlayerSettings() != NULL); }
		inline bool IsShipSection (void) const { return m_fShipCompartment; }
		inline bool IsShownAtNewGame (void) const { const CPlayerSettings *pPlayerSettings = GetPlayerSettings(); return (pPlayerSettings && pPlayerSettings->IsInitialClass() && !IsVirtual()); }
		inline bool IsTimeStopImmune (void) { return (m_fTimeStopImmune ? true : false); }
		void MarkImages (bool bMarkDevices);
		void Paint (CG32bitImage &Dest, 
					int x, 
					int y, 
					const ViewportTransform &Trans, 
					int iDirection, 
					int iTick, 
					bool bThrusting = false,
					bool bRadioactive = false,
					DWORD byInvisible = 0);
		void PaintDevicePositions (CG32bitImage &Dest, int x, int y, const CDeviceDescList &Devices, int iShipRotation) const;
		void PaintDockPortPositions (CG32bitImage &Dest, int x, int y, int iShipRotation) const;
		void PaintInteriorCompartments (CG32bitImage &Dest, int x, int y, int iShipRotation) const;
		void PaintMap (CMapViewportCtx &Ctx, 
					CG32bitImage &Dest, 
					int x, 
					int y, 
					int iDirection, 
					int iTick, 
					bool bThrusting,
					bool bRadioactive);
		void PaintScaled (CG32bitImage &Dest,
					int x,
					int y,
					int cxWidth,
					int cyHeight,
					int iRotation,
					int iTick);

		//	CDesignType overrides
		static CShipClass *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designShipClass) ? (CShipClass *)pType : NULL); }
		virtual bool FindDataField (const CString &sField, CString *retsValue) const override;
		virtual CCommunicationsHandler *GetCommsHandler (void) override;
		virtual CEconomyType *GetEconomyType (void) const;
		virtual int GetLevel (int *retiMinLevel = NULL, int *retiMaxLevel = NULL) const override { if (retiMinLevel) *retiMinLevel = m_iLevel; if (retiMaxLevel) *retiMaxLevel = m_iLevel; return m_iLevel; }
		virtual CString GetNamePattern (DWORD dwNounFormFlags = 0, DWORD *retdwFlags = NULL) const override;
		virtual CTradingDesc *GetTradingDesc (void) const override { return m_pTrade; }
		virtual DesignTypes GetType (void) const override { return designShipClass; }
        virtual const CCompositeImageDesc &GetTypeImage (void) const override { return m_Image; }
		virtual bool IsVirtual (void) const override { return (m_fVirtual ? true : false); }

		static Metric GetStdCombatStrength (int iLevel);
		static void Reinit (void);
		static void UnbindGlobal (void);

	protected:
		//	CDesignType overrides
		virtual void OnAccumulateXMLMergeFlags (TSortMap<DWORD, DWORD> &MergeFlags) const override;
		virtual void OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override;
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual CEffectCreator *OnFindEffectCreator (const CString &sUNID) override;
		virtual ALERROR OnFinishBindDesign (SDesignLoadCtx &Ctx) override;
        virtual CString OnGetMapDescriptionMain (SMapDescriptionCtx &Ctx) const override;
		virtual ICCItem *OnGetProperty (CCodeChainCtx &Ctx, const CString &sProperty) const override;
		virtual bool OnHasSpecialAttribute (const CString &sAttrib) const override;
		virtual void OnInitFromClone (CDesignType *pSource) override;
		virtual void OnMarkImages (void) override { MarkImages(true); }
		virtual void OnMergeType (CDesignType *pSource) override;
		virtual void OnReadFromStream (SUniverseLoadCtx &Ctx) override;
		virtual void OnReinit (void) override;
		virtual void OnSweep (void) override;
		virtual void OnUnbindDesign (void) override;
		virtual void OnWriteToStream (IWriteStream *pStream) override;

	private:
		enum PrivateConstants
			{
			maxExhaustImages = 5
			};

		struct SEquipmentDesc
			{
			Abilities iEquipment;
			bool bInstall;
			};

		struct SExhaustDesc
			{
			C3DConversion PosCalc;
			};

		EBalanceTypes CalcBalanceType (CString *retsDesc = NULL, Metric *retrCombatStrength = NULL) const;
		CItemType *CalcBestMissile (const SDeviceDesc &Device) const;
		Metric CalcCombatStrength (void) const;
		Metric CalcDamageRate (int *retiAveWeaponLevel = NULL, int *retiMaxWeaponLevel = NULL) const;
		Metric CalcDefenseRate (void) const;
		inline Metric CalcDodgeRate (void) const { return CalcManeuverValue(true); }
		int CalcLevel (void) const;
		Metric CalcManeuverValue (bool bDodge = false) const;
		ICCItem *CalcMaxSpeedByArmorMass (CCodeChainCtx &Ctx) const;
		int CalcMinArmorMassForSpeed (int iSpeed) const;
        void CalcPerformance (void);
		int ComputeDeviceLevel (const SDeviceDesc &Device) const;
		void ComputeMovementStats (int *retiSpeed, int *retiThrust, int *retiManeuver);
		int ComputeScore (int iArmorLevel,
						  int iPrimaryWeapon,
						  int iSpeed,
						  int iThrust,
						  int iManeuver,
						  bool bPrimaryIsLauncher);
		void CreateWreckImage (void);
		void FindBestMissile (CDeviceClass *pLauncher, IItemGenerator *pItems, CItemType **retpMissile) const;
		void FindBestMissile (CDeviceClass *pLauncher, const CItemList &Items, CItemType **retpMissile) const;
		CString GetGenericName (DWORD *retdwFlags = NULL) const;
		inline int GetManeuverDelay (void) const { return m_Perf.GetRotationDesc().GetManeuverDelay(); }
		CStationType *GetWreckDesc (void);
		void InitDefaultArmorLimits (Metric rHullMass, int iMaxSpeed, Metric rThrustRatio);
		void InitShipNamesIndices (void);
		void PaintThrust (CG32bitImage &Dest, 
						int x, 
						int y, 
						const ViewportTransform &Trans, 
						int iDirection, 
						int iTick,
						bool bInFrontOnly);

		static int CalcDefaultSize (const CObjectImageArray &Image);

		CString m_sManufacturer;				//	Name of manufacturer
		CString m_sName;						//	Class name
		CString m_sTypeName;					//	Name of type
		DWORD m_dwClassNameFlags;				//	Flags for class name
		CSovereignRef m_pDefaultSovereign;		//	Sovereign

		CString m_sShipNames;					//	Names to use for individual ship
		DWORD m_dwShipNameFlags;				//	Flags for ship name
		TArray<int> m_ShipNamesIndices;			//	Shuffled indices for ship names
		mutable int m_iShipName;				//	Current ship name index

		int m_iScore;							//	Score when destroyed
		int m_iLevel;							//	Ship class level
		EBalanceTypes m_iLevelType;				//	Type of ships for level
		Metric m_rCombatStrength;				//	Combat strength

        //  Hull properties

		int m_iMass;							//	Empty mass (tons)
		int m_iSize;							//	Length in meters
		CCurrencyAndValue m_HullValue;			//	Value of hull alone (excluding any devices/armor)
		CRotationDesc m_RotationDesc;	        //	Rotation and maneuverability
		double m_rThrustRatio;					//	If non-zero, then m_DriveDesc thrust is set based on this.
		CDriveDesc m_DriveDesc;					//	Drive descriptor
		CReactorDesc m_ReactorDesc;				//	Reactor descriptor
        CCargoDesc m_CargoDesc;                 //  Cargo space descriptor
		int m_iCyberDefenseLevel;				//	Cyber defense level

        //  Class limits

		CItemCriteria m_ArmorCriteria;			//	Allowable armor
		CItemCriteria m_DeviceCriteria;			//	Allowable devices
		int m_iStdArmorMass;					//	No penalty at this armor mass
		int m_iMaxArmorMass;					//	Max mass of single armor segment
		int m_iMaxArmorSpeedPenalty;			//	Change to speed at max armor mass (1/100th light-speed)
		int m_iMinArmorSpeedBonus;				//	Change to speed at 1/2 std armor mass

		int m_iMaxCargoSpace;					//	Max amount of cargo space with expansion (tons)
		int m_iMaxReactorPower;					//	Max compatible reactor power
		int m_iMaxDevices;						//	Max number of devices
		int m_iMaxWeapons;						//	Max number of weapon devices (including launchers)
		int m_iMaxNonWeapons;					//	Max number of non-weapon devices

        //  Wrecks

		int m_iLeavesWreck;						//	Chance that it leaves a wreck
		int m_iStructuralHP;					//	Structual hp of wreck
		CStationTypeRef m_pWreckType;				//	Station type to use as wreck

        //  Armor, Devices, Equipment, Etc.

        CShipArmorDesc m_Armor;                 //  Armor descriptor
		CShipInteriorDesc m_Interior;			//	Interior structure
		IDeviceGenerator *m_pDevices;			//	Generator of devices
		TArray<SEquipmentDesc> m_Equipment;		//	Initial equipment

        //  Performance Stats (after accounting for devices, etc).

        CShipPerformanceDesc m_Perf;            //  Performance based on average devices (only for stats)
		CDeviceDescList m_AverageDevices;		//	Average complement of devices (only for stats)

        //  AI & Player Settings

		CAISettings m_AISettings;				//	AI controller data
		mutable CPlayerSettings *m_pPlayerSettings;		//	Player settings data
		IItemGenerator *m_pItems;				//	Random items
        CAttributeDataBlock m_InitialData;      //  Initial data for ship object

		//	Escorts

		IShipGenerator *m_pEscorts;				//	Escorts

		//	Character

		CGenericTypeRef m_CharacterClass;		//	Character class
		CGenericTypeRef m_Character;			//	Character for ship

		//	Docking

		CDockingPorts m_DockingPorts;			//	Docking port definitions
		CDockScreenTypeRef m_pDefaultScreen;	//	Default screen
		DWORD m_dwDefaultBkgnd;					//	Default background screen
		CTradingDesc *m_pTrade;					//	Trade descriptors

		CCommunicationsHandler m_OriginalCommsHandler;
		CCommunicationsHandler m_CommsHandler;	//	Communications handler

		//	Image

		CCompositeImageDesc m_Image;			//	Image of ship
        CObjectImageArray m_HeroImage;          //  Large image
		CObjectEffectDesc m_Effects;			//	Effects for ship

		//	Wreck image

		CG32bitImage m_WreckBitmap;				//	Image to use when ship is wrecked
		CObjectImageArray m_WreckImage;			//	Image to use when ship is wrecked

		//	Explosion

		CWeaponFireDescRef m_pExplosionType;	//	Explosion to create when ship is destroyed

		//	Exhaust

		CObjectImageArray m_ExhaustImage;		//	Image of drive exhaust
		TArray<SExhaustDesc> m_Exhaust;			//	Drive exhaust painting

		//	Misc

		DWORD m_fRadioactiveWreck:1;			//	TRUE if wreck is always radioactive
		DWORD m_fHasDockingPorts:1;				//	TRUE if ship has docking ports
		DWORD m_fTimeStopImmune:1;				//	TRUE if ship is immune to stop-time
		DWORD m_fSpare4a:1;						//	Unused
		DWORD m_fSpare5a:1;						//	Unused
		DWORD m_fSpare6a:1;						//	Unused
		DWORD m_fCommsHandlerInit:1;			//	TRUE if comms handler has been initialized
		DWORD m_fVirtual:1;						//	TRUE if ship class is virtual (e.g., a base class)

		DWORD m_fOwnPlayerSettings:1;		    //	TRUE if we own m_pPlayerSettings
		DWORD m_fScoreOverride:1;				//	TRUE if score is specified in XML
		DWORD m_fLevelOverride:1;				//	TRUE if level is specified in XML
		DWORD m_fInheritedDevices:1;			//	TRUE if m_pDevices is inherited from another class
		DWORD m_fInheritedItems:1;				//	TRUE if m_pItems is inherited from another class
		DWORD m_fInheritedEscorts:1;			//	TRUE if m_pEscorts is inherited from another class
		DWORD m_fCyberDefenseOverride:1;		//	TRUE if cyberDefenseLevel is specified in XML
		DWORD m_fInheritedTrade:1;				//	TRUE if m_pTrade is inherited from another class

		DWORD m_fShipCompartment:1;				//	TRUE if we represent an attached compartment/segment
		DWORD m_fSpare2:1;
		DWORD m_fSpare3:1;
		DWORD m_fSpare4:1;
		DWORD m_fSpare5:1;
		DWORD m_fSpare6:1;
		DWORD m_fSpare7:1;
		DWORD m_fSpare8:1;

		DWORD m_fSpare:8;

		static CPlayerSettings m_DefaultPlayerSettings;
		static bool m_bDefaultPlayerSettingsBound;
	};

