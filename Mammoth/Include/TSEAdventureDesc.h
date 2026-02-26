//	TSEAdventureDesc.h
//
//	Classes and functions for adventure descriptors.
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

//	Engine Options -------------------------------------------------------------

class CEngineOptions
	{
	public:

		explicit CEngineOptions (int apiVersion = API_VERSION);

		const CDamageAdjDesc* GetArmorDamageAdj (int iLevel) const { if (iLevel < 1 || iLevel > MAX_ITEM_LEVEL) throw CException(ERR_FAIL); return &m_ArmorDamageAdj[iLevel - 1]; }
		int GetDefaultInteraction () const { return m_iDefaultInteraction; }
		int GetDefaultShotHP () const { return m_iDefaultShotHP; }
		const CDeviceDamageLevelDesc* GetExternalDeviceDamageMaxLevels () const { return &m_ExternalDeviceDamageMaxLevels; }
		const CDeviceDamageLevelDesc* GetInternalDeviceDamageMaxLevels () const { return &m_InternalDeviceDamageMaxLevels; }
		const CDamageMethodDesc* GetDamageMethodDesc (EDamageMethod iMethod) const;
		Metric GetDmgMethodFortificationItemArmor (EDamageMethod iMethod) const { return GetDamageMethodFortification(m_DmgMethodItemFortification.Armor, iMethod); };
		Metric GetDmgMethodFortificationItemShield (EDamageMethod iMethod) const { return GetDamageMethodFortification(m_DmgMethodItemFortification.Shield, iMethod); };
		Metric GetDmgMethodFortificationShipArmorCritical (EDamageMethod iMethod) const { return GetDamageMethodFortification(m_DmgMethodShipFortification.Armor.Critical, iMethod); };
		Metric GetDmgMethodFortificationShipArmorCriticalUncrewed (EDamageMethod iMethod) const { return GetDamageMethodFortification(m_DmgMethodShipFortification.Armor.CriticalUncrewed, iMethod); };
		Metric GetDmgMethodFortificationShipArmorNonCritical (EDamageMethod iMethod) const { return GetDamageMethodFortification(m_DmgMethodShipFortification.Armor.NonCritical, iMethod); };
		Metric GetDmgMethodFortificationShipArmorNonCriticalDestruction (EDamageMethod iMethod) const { return GetDamageMethodFortification(m_DmgMethodShipFortification.Armor.NonCriticalDestruction, iMethod); };
		Metric GetDmgMethodFortificationShipArmorNonCriticalDestructionChance () const { return m_DmgMethodShipFortification.Armor.rNonCriticalDestructionChance; };
		Metric GetDmgMethodFortificationShipCompartmentGeneral (EDamageMethod iMethod) const { return GetDamageMethodFortification(m_DmgMethodShipFortification.Compartment.General, iMethod); };
		Metric GetDmgMethodFortificationShipCompartmentMainDrive (EDamageMethod iMethod) const { return GetDamageMethodFortification(m_DmgMethodShipFortification.Compartment.MainDrive, iMethod); };
		Metric GetDmgMethodFortificationShipCompartmentCargo (EDamageMethod iMethod) const { return GetDamageMethodFortification(m_DmgMethodShipFortification.Compartment.Cargo, iMethod); };
		Metric GetDmgMethodFortificationShipCompartmentUncrewed (EDamageMethod iMethod) const { return GetDamageMethodFortification(m_DmgMethodShipFortification.Compartment.Uncrewed, iMethod); };
		Metric GetDmgMethodFortificationStationHullSingle (EDamageMethod iMethod) const { return GetDamageMethodFortification(m_DmgMethodStationFortification.Hull.Single, iMethod); };
		Metric GetDmgMethodFortificationStationHullMulti (EDamageMethod iMethod) const { return GetDamageMethodFortification(m_DmgMethodStationFortification.Hull.Multi, iMethod); };
		Metric GetDmgMethodFortificationStationHullAsteroid (EDamageMethod iMethod) const { return GetDamageMethodFortification(m_DmgMethodStationFortification.Hull.Asteroid, iMethod); };
		Metric GetDmgMethodFortificationStationHullUnderground (EDamageMethod iMethod) const { return GetDamageMethodFortification(m_DmgMethodStationFortification.Hull.Underground, iMethod); };
		Metric GetDmgMethodFortificationStationHullUncrewed (EDamageMethod iMethod) const { return GetDamageMethodFortification(m_DmgMethodStationFortification.Hull.Uncrewed, iMethod); };
		Metric GetDmgMethodFortificationStationHullArmor (EDamageMethod iMethod) const { return GetDamageMethodFortification(m_DmgMethodStationFortification.Hull.Armor, iMethod); };
		Metric GetDamageMethodMinDamage ()	const { return m_rDamageMethodAdjMinDamage; }
		EDamageMethodSystem GetDamageMethodSystem () const { return m_iDamageMethodSystem; }
		Metric GetItemDefaultDensity () const { return m_rDefaultItemDensity; }
		Metric GetItemXMLMassToVolumeRatio () const { return m_rDefaultItemMassToVolume; }
		const CMiningDamageLevelDesc* GetMiningMaxOreLevels () const { return &m_MiningDamageMaxOreLevels; }
		const CDamageAdjDesc* GetShieldDamageAdj (int iLevel) const { if (iLevel < 1 || iLevel > MAX_ITEM_LEVEL) throw CException(ERR_FAIL); return &m_ShieldDamageAdj[iLevel - 1]; }
		Metric GetShieldIdlePowerRatio () const { return m_rDefaultShieldIdlePowerRatio; }
		bool HidesArmorImmunity (SpecialDamageTypes iSpecial) const;
		bool InitArmorDamageAdjFromXML (SDesignLoadCtx &Ctx, const CXMLElement &XMLDesc) { m_bCustomArmorDamageAdj = true; return InitDamageAdjFromXML(Ctx, XMLDesc, m_ArmorDamageAdj); }
		bool InitExternalDeviceDamageMaxLevelsFromXML (SDesignLoadCtx& Ctx, const CXMLElement& XMLDesc);
		bool InitInternalDeviceDamageMaxLevelsFromXML (SDesignLoadCtx& Ctx, const CXMLElement& XMLDesc);
		bool InitFromProperties (SDesignLoadCtx &Ctx, const CDesignType &Type);
		bool InitDamageMethodDescsFromXML (SDesignLoadCtx& Ctx, const CXMLElement& XMLDesc);
		bool InitMiningMaxOreLevelsFromXML (SDesignLoadCtx& Ctx, const CXMLElement& XMLDesc);
		bool InitShieldDamageAdjFromXML (SDesignLoadCtx &Ctx, const CXMLElement &XMLDesc) { m_bCustomShieldDamageAdj = true; return InitDamageAdjFromXML(Ctx, XMLDesc, m_ShieldDamageAdj); }
		bool IsDamageShown () const { return m_bShowDamageDone; }
		void SetShowDamageDone (bool bValue = true) { m_bShowDamageDone = bValue; }

	private:

		//	Damage Method Internal Structs

		struct SDamageMethodItemAdj
			{
			SDamageMethodFortification Armor;
			SDamageMethodFortification Shield;
			};

		struct SDamageMethodShipAdj
			{
			struct SDamageMethodShipArmorAdj
				{
				SDamageMethodFortification Critical;
				SDamageMethodFortification CriticalUncrewed;
				SDamageMethodFortification NonCritical;
				SDamageMethodFortification NonCriticalDestruction;
				Metric rNonCriticalDestructionChance = 0.05;
				};
			struct SDamageMethodShipCompartmentAdj
				{
				SDamageMethodFortification General;
				SDamageMethodFortification MainDrive;
				SDamageMethodFortification Cargo;
				SDamageMethodFortification Uncrewed;
				};
			SDamageMethodShipArmorAdj Armor;
			SDamageMethodShipCompartmentAdj Compartment;
			};

		struct SDamageMethodStationAdj
			{
			struct SDamageMethodStationHullAdj
				{
				SDamageMethodFortification Single;
				SDamageMethodFortification Multi;
				SDamageMethodFortification Asteroid;
				SDamageMethodFortification Underground;
				SDamageMethodFortification Uncrewed;
				SDamageMethodFortification Armor;
				};
			SDamageMethodStationHullAdj Hull;
			};

		//	Damage Method Internal Structs

		struct SDamageMethodDescs
			{
			public:
				const CDamageMethodDesc* GetCrush () const { return &desc[0]; }
				const CDamageMethodDesc* GetPierce () const { return &desc[1]; }
				const CDamageMethodDesc* GetShred () const { return &desc[2]; }
				const CDamageMethodDesc* GetWMD () const { return &desc[0]; }

			protected:
				//	Our protected accessors are non-const
				//	This is so we can call non-const methods on CDamageMethodDesc
				//	We need to do this on custom initialization
				//
				CDamageMethodDesc* Crush () { return &desc[0]; }
				CDamageMethodDesc* Pierce () { return &desc[1]; }
				CDamageMethodDesc* Shred () { return &desc[2]; }
				CDamageMethodDesc* WMD () { return &desc[0]; }

				void SetCrush (CDamageMethodDesc newDesc) { desc[0] = newDesc; }
				void SetPierce (CDamageMethodDesc newDesc) { desc[1] = newDesc; }
				void SetShred (CDamageMethodDesc newDesc) { desc[2] = newDesc; }
				void SetWMD (CDamageMethodDesc newDesc) { desc[0] = newDesc; }

				friend CEngineOptions;
			private:
				CDamageMethodDesc desc[3] = { CDamageMethodDesc(), CDamageMethodDesc(), CDamageMethodDesc()};
			};

		Metric GetDamageMethodFortification (const SDamageMethodFortification &adj, EDamageMethod iMethod) const;
		bool InitDamageAdjFromXML (SDesignLoadCtx &Ctx, const CXMLElement &XMLDesc, CDamageAdjDesc *DestTable);
		bool InitDamageMethodFortificationFromCC (SDesignLoadCtx& Ctx, SDamageMethodFortification& adj, ICCItem* pStruct);

		void InitDefaultGlobals ();
		void InitDefaultDescs ();
		void InitDefaultDamageMethods ();
		static void InitDefaultDamageAdj ();
		static CMiningDamageLevelDesc GetDefaultMiningMaxOreLevels (int apiVersion);
		static CDeviceDamageLevelDesc GetDefaultExternalDeviceDamageLevels (int apiVersion);
		static CDeviceDamageLevelDesc GetDefaultInternalDeviceDamageLevels (int apiVersion);
		static CDamageMethodDesc GetDefaultWMDAdj (int apiVersion);

		int m_iDefaultForAPIVersion = -1;

		//	Default damage adj curves

		CDamageAdjDesc m_ArmorDamageAdj[MAX_ITEM_LEVEL];
		CDamageAdjDesc m_ShieldDamageAdj[MAX_ITEM_LEVEL];

		bool m_bCustomArmorDamageAdj = false;
		bool m_bCustomShieldDamageAdj = false;

		//	Default device damage max item level curve

		CDeviceDamageLevelDesc m_ExternalDeviceDamageMaxLevels;
		CDeviceDamageLevelDesc m_InternalDeviceDamageMaxLevels;

		bool m_bCustomExternalDeviceDamageMaxLevels = false;
		bool m_bCustomInternalDeviceDamageMaxLevels = false;

		//	Default mining damage max ore curve

		CMiningDamageLevelDesc m_MiningDamageMaxOreLevels;

		bool m_bCustomMiningMaxOreLevels = false;

		//	Default shot interaction

		int m_iDefaultInteraction = -1;
		int m_iDefaultShotHP = -1;

		//	Default item legacy mass-volume conversions

		Metric m_rDefaultItemDensity = 1.0;
		Metric m_rDefaultItemMassToVolume = 1.0;

		//	Default power consumption

		Metric m_rDefaultShieldIdlePowerRatio = 0.125;

		//	Default Damage Method behavior

		EDamageMethodSystem m_iDamageMethodSystem = EDamageMethodSystem::dmgMethodSysError;

		SDamageMethodDescs m_DamageMethodDescs;
		bool m_bCustomDamageMethodDescs;

		double m_rDamageMethodAdjMinDamage = 0.0;

		SDamageMethodItemAdj m_DmgMethodItemFortification;
		SDamageMethodShipAdj m_DmgMethodShipFortification;
		SDamageMethodStationAdj m_DmgMethodStationFortification;

		//	Default Item Stat Card UI

		bool m_bHideDisintegrationImmune = false;
		bool m_bHideIonizeImmune = false;
		bool m_bHideRadiationImmune = false;
		bool m_bHideShatterImmune = false;

		bool m_bShowDamageDone = false;
	};

//	CAdventureDesc -------------------------------------------------------------

class CAdventureDesc : public CDesignType
	{
	public:
		CAdventureDesc (void);

		void FireOnGameEnd (const CGameRecord &Game, const SBasicGameStats &BasicStats);
		void FireOnGameStart (void);
		DWORD GetBackgroundUNID (void) const { return m_dwBackgroundUNID; }
		const CEconomyType &GetDefaultCurrency (void) const;
		CString GetDesc (void);
		CDifficultyOptions::ELevel GetDifficulty (void) const { return m_iForceDifficulty; }
		const CXMLElement &GetEncounterOverrideXML (void) const { return m_EncounterOverridesXML; }
		const CEngineOptions &GetEngineOptions (void) const { return m_EngineOptions; }
		DWORD GetExtensionUNID (void) const { return m_dwExtensionUNID; }
		const CString &GetName (void) const { return m_sName; }
		DWORD GetStartingMapUNID (void) const { return m_dwStartingMap; }
		const CString &GetStartingNodeID (void) const { return m_sStartingNodeID; }
		const CString &GetStartingPos (void) const { return m_sStartingPos; }
		ALERROR GetStartingShipClasses (TSortMap<CString, CShipClass *> *retClasses, CString *retsError);
		const CString &GetWelcomeMessage (void) const { return m_sWelcomeMessage; }
		bool IsCurrentAdventure (void) const { return (m_fIsCurrentAdventure ? true : false); }
		bool IsInDefaultResource (void) const { return (m_fInDefaultResource ? true : false); }
		bool IsValidStartingClass (CShipClass *pClass);
		void SetCurrentAdventure (bool bCurrent = true) { m_fIsCurrentAdventure = bCurrent; }

		//	CDesignType overrides

		static CAdventureDesc *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designAdventureDesc) ? (CAdventureDesc *)pType : NULL); }
		virtual bool FindDataField (const CString &sField, CString *retsValue) const override;
		virtual DesignTypes GetType (void) const override { return designAdventureDesc; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual void OnUnbindDesign (void) override { m_fIsCurrentAdventure = false; }

	private:

		DWORD m_dwExtensionUNID = 0;

		CString m_sName;						//	Name of adventure
		DWORD m_dwBackgroundUNID = 0;				//	Background image to use for choice screen
		CString m_sWelcomeMessage;				//	Equivalent of "Welcome to Transcendence!"

		CDesignTypeCriteria m_StartingShips;	//	Starting ship criteria
		DWORD m_dwStartingMap = 0;				//	Default system map to load
		CString m_sStartingNodeID;				//	NodeID where we start
		CString m_sStartingPos;					//	Named object at which we start

		CEngineOptions m_EngineOptions;			//	Options for engine behavior
		CXMLElement m_EncounterOverridesXML;
		CDifficultyOptions::ELevel m_iForceDifficulty = CDifficultyOptions::ELevel::Unknown;

		CEconomyTypeRef m_pDefaultCurrency;		//	Default currency (mostly used for UI)

		DWORD m_fIsCurrentAdventure:1;			//	TRUE if this is the current adventure
		DWORD m_fInDefaultResource:1;			//	TRUE if adventure is a module in the default resource
		DWORD m_fIncludeOldShipClasses:1;		//	TRUE if we should include older extensions (even if 
												//		they don't match starting ship criteria).
	};
