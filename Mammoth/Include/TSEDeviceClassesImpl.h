//	TSEDeviceClassesImpl.h
//
//	Transcendence Device Classes
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CWeaponTargetDefinition
	{
	//  Weapon target definition that is attached to installed weapon devices.
	//  Written and read from stream independently using a similar manner to item data.
	//  Should only be attached to weapon devices through a unique_ptr.
	//  Note that we use the range, fire arc, and all other property of the attached weapon, since all this does
	//  is tell our attached weapon what to shoot at.
	public:
		class CWeaponTargetDefinition () { };
		class CWeaponTargetDefinition (Kernel::CString sCriteria, bool bCheckLineOfFire = false) : m_bCheckLineOfFire(bCheckLineOfFire), m_CriteriaString(sCriteria) { m_TargetCriteria.Init(sCriteria); };
		bool MatchesTarget (CSpaceObject* pSource, CSpaceObject* pTarget) const;
		CSpaceObject* FindTarget (CWeaponClass* pWeapon, CInstalledDevice* pDevice, CSpaceObject* pSource, CItemCtx& ItemCtx) const;
		bool AimAndFire (CWeaponClass* pWeapon, CInstalledDevice* pDevice, CSpaceObject* pSource, CDeviceClass::SDeviceUpdateCtx& Ctx) const;
		bool GetCheckLineOfFire () { return m_bCheckLineOfFire; };
		CSpaceObjectCriteria GetTargetCriteria () { return m_TargetCriteria; };
		Kernel::CString GetTargetCriteriaString () { return Kernel::CString(m_CriteriaString.c_str()); };
		void SetCheckLineOfFire (bool bCheckLineOfFire) { m_bCheckLineOfFire = bCheckLineOfFire; };
		void SetTargetCriteria (Kernel::CString sCriteria) { m_TargetCriteria.Init(sCriteria); m_CriteriaString = sCriteria; };

		static std::unique_ptr<CWeaponTargetDefinition> ReadFromStream (SLoadCtx& Ctx);
		void WriteToStream (IWriteStream* pStream) const;
	private:
		std::string m_CriteriaString = "";
		CSpaceObjectCriteria m_TargetCriteria;
		bool m_bCheckLineOfFire = false;		//	Check line of fire for friendlies
	};

class CAutoDefenseClass : public CDeviceClass
	{
	public:
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, CDeviceClass **retpDevice);

		//	CDeviceClass virtuals

		virtual int CalcPowerUsed (SUpdateCtx &Ctx, CInstalledDevice *pDevice, CSpaceObject *pSource) override;
		virtual ICCItem *FindItemProperty (CItemCtx &Ctx, const CString &sProperty) override;
		virtual int GetActivateDelay (CItemCtx &ItemCtx) const override;
		virtual ItemCategories GetImplCategory (void) const override { return itemcatMiscDevice; }
		virtual DamageTypes GetDamageType (CItemCtx &Ctx, const CItem &Ammo = CItem()) const override;
		virtual Metric GetShotSpeed (CItemCtx &Ctx) const override;
		virtual int GetPowerRating (CItemCtx &Ctx, int *retiIdlePowerUse = NULL) const override;
		virtual bool GetReferenceDamageType (CItemCtx &Ctx, const CItem &Ammo, DamageTypes *retiDamage, CString *retsReference) const override;
		virtual DWORD GetTargetTypes (const CDeviceItem &DeviceItem) const override;
		virtual bool IsAreaWeapon (const CDeviceItem &DeviceItem) const override;
		virtual bool IsAutomatedWeapon (void) const override { return true; }
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) override;
		virtual void Update (CInstalledDevice *pDevice, CSpaceObject *pSource, SDeviceUpdateCtx &Ctx) override;

	protected:
		virtual void OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override;
		virtual CString OnGetReference (CItemCtx &Ctx, const CItem &Ammo = CItem(), DWORD dwFlags = 0) override;

	private:
		enum TargetingSystemTypes
			{
			trgNone,

			trgMissiles,				//	Target nearest missile
			trgCriteria,				//	Target nearest object that matches criteria
			trgFunction,				//	Target computed by a function
			};

		CAutoDefenseClass (void);

		CDeviceClass *GetWeapon (void) const { return m_pWeapon; }
		bool IsDirectional (CInstalledDevice *pDevice, int *retiMinFireArc, int *retiMaxFireArc);
		bool IsOmniDirectional (CInstalledDevice *pDevice);
		CSpaceObject *FindTarget (CInstalledDevice *pDevice, CSpaceObject *pSource);

		TargetingSystemTypes m_iTargeting = trgNone;
		CSpaceObjectCriteria m_TargetCriteria;
		Metric m_rInterceptRange = 0.0;
		Metric m_rMinSourceRange2 = 0.0;
		bool m_bCheckLineOfFire = false;		//	Check line of fire for friendlies

		bool m_bOmnidirectional = false;		//	Omnidirectional
		int m_iMinFireArc = 0;					//	Min angle of fire arc (degrees)
		int m_iMaxFireArc = 0;					//	Max angle of fire arc (degrees)
		int m_iRechargeTicks = 0;

		CDeviceClassRef m_pWeapon;
	};

class CCargoSpaceClass : public CDeviceClass
	{
	public:
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, CDeviceClass **retpDevice);

		//	CDeviceClass virtuals

		virtual bool FindDataField (const CString &sField, CString *retsValue) override;
		virtual const CCargoDesc *GetCargoDesc (CItemCtx &Ctx) const override { return GetDesc(Ctx); }
		virtual ItemCategories GetImplCategory (void) const override { return itemcatCargoHold; }
		virtual void OnInstall (CInstalledDevice *pDevice, CSpaceObject *pSource, CItemListManipulator &ItemList) override;

	protected:
		virtual bool OnAccumulatePerformance (CItemCtx &ItemCtx, SShipPerformanceCtx &Ctx) const override;
		virtual bool OnCanBeDamaged (void) const override { return false; }
		virtual bool OnCanBeDisabled (CItemCtx &Ctx) const override { return false; }
		virtual bool OnCanBeDisrupted (void) const override { return false; }
		virtual CString OnGetReference (CItemCtx &Ctx, const CItem &Ammo = CItem(), DWORD dwFlags = 0) override;

	private:
		CCargoSpaceClass (void);
		const CCargoDesc *GetDesc (CItemCtx &Ctx) const;

		TArray<CCargoDesc> m_Desc;
	};

class CCyberDeckClass : public CDeviceClass
	{
	public:
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, CDeviceClass **retpDevice);

		//	CDeviceClass virtuals

		virtual bool Activate (CInstalledDevice &Device, SActivateCtx &ActivateCtx) override;
		virtual bool CanHitFriends (void) const override { return false; }
		virtual int GetActivateDelay (CItemCtx &ItemCtx) const override { return 30; }
		virtual ItemCategories GetImplCategory (void) const override { return itemcatWeapon; }
		virtual DamageTypes GetDamageType (CItemCtx &Ctx, const CItem &Ammo = CItem()) const override { return damageGeneric; }
		virtual Metric GetMaxEffectiveRange (CSpaceObject *pSource, const CInstalledDevice *pDevice, CSpaceObject *pTarget) const override;
		virtual Metric GetMaxRange (CItemCtx &ItemCtx) override;
		virtual void GetSelectedVariantInfo (const CSpaceObject *pSource, 
											 const CInstalledDevice *pDevice,
											 CString *retsLabel,
											 int *retiAmmoLeft,
											 CItemType **retpType = NULL,
											 bool bUseCustomAmmoCountHandler = false) override;
		virtual DWORD GetTargetTypes (const CDeviceItem &DeviceItem) const override { return CTargetList::SELECT_ATTACKERS | CTargetList::SELECT_FORTIFICATION; }
		virtual int GetValidVariantCount (CSpaceObject *pSource, CInstalledDevice *pDevice) override { return 1; }
		virtual int GetWeaponEffectiveness (const CDeviceItem &DeviceItem, CSpaceObject *pTarget) const override;
		virtual bool IsFirstVariantSelected(CSpaceObject *pSource, CInstalledDevice *pDevice) override { return true; }
		virtual bool IsLastVariantSelected (CSpaceObject *pSource, CInstalledDevice *pDevice) override { return true; }
		virtual bool IsVariantSelected (CSpaceObject *pSource, CInstalledDevice *pDevice) override { return true; }
		virtual bool IsWeaponAligned (CSpaceObject *pShip, const CInstalledDevice *pDevice, CSpaceObject *pTarget, int *retiAimAngle = NULL, int *retiFireAngle = NULL) const override;
		virtual bool SelectFirstVariant (CSpaceObject *pSource, CInstalledDevice *pDevice) override { return true; }
		virtual bool SelectNextVariant (CSpaceObject *pSource, CInstalledDevice *pDevice, int iDir = 1) override { return true; }
		virtual bool ValidateSelectedVariant (CSpaceObject *pSource, CInstalledDevice *pDevice) override { return true; }

	protected:

		//	CDeviceClass virtuals

		virtual CString OnGetReference (CItemCtx &Ctx, const CItem &Ammo = CItem(), DWORD dwFlags = 0) override;

	private:
		CCyberDeckClass (void);

		int m_iRange;							//	in light-seconds
		int m_iAttackChance;					//	%chance of success per attack

		ProgramDesc m_Program;					//	Program description
	};

class CDriveClass : public CDeviceClass
	{
	public:
		~CDriveClass (void);

		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, CDeviceClass **retpDrive);
		static ICCItem *GetDriveProperty (const CDriveDesc &Desc, const CString &sProperty);

		//	CDeviceClass virtuals

		virtual bool FindDataField (const CString &sField, CString *retsValue) override;
		virtual ItemCategories GetImplCategory (void) const override { return itemcatDrive; }
		virtual ICCItem *FindItemProperty (CItemCtx &Ctx, const CString &sProperty) override;
		virtual int GetPowerRating (CItemCtx &Ctx, int *retiIdlePowerUse = NULL) const override;
		virtual void OnInstall (CInstalledDevice *pDevice, CSpaceObject *pSource, CItemListManipulator &ItemList) override;

	protected:

		//	CDeviceClass virtuals

		virtual void OnAccumulateAttributes (const CDeviceItem &DeviceItem, const CItem &Ammo, TArray<SDisplayAttribute> *retList) const override;
		virtual bool OnAccumulatePerformance (CItemCtx &ItemCtx, SShipPerformanceCtx &Ctx) const override;
		virtual CString OnGetReference (CItemCtx &Ctx, const CItem &Ammo = CItem(), DWORD dwFlags = 0) override;

	private:
		struct SScalableStats
			{
			SScalableStats (void) : 
					iLevel(-1)
				{ }

			int iLevel;
			CDriveDesc DriveDesc;
			CRotationDesc ManeuverDesc;
			};

		CDriveClass (void);
		CDriveClass (const CDriveClass &Desc) = delete;
		CDriveClass &operator= (const CDriveClass &Desc) = delete;

		const SScalableStats *GetDesc (CItemCtx &Ctx) const;
		const SScalableStats *GetDesc (const CDeviceItem &DeviceItem) const;
		void InitDamagedDesc (void) const;
		void InitEnhancedDesc (void) const;
		static ALERROR InitStatsFromXML (SDesignLoadCtx &Ctx, int iLevel, DWORD dwUNID, CXMLElement *pDesc, SScalableStats &retStats);

		int m_iLevels;                      //  Total number of levels.
		SScalableStats *m_pDesc;            //  Array of entries, one per scaled level (minimum 1)

		mutable SScalableStats *m_pDamagedDesc;
		mutable SScalableStats *m_pEnhancedDesc;
	};

class CEnhancerClass : public CDeviceClass
	{
	public:
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, CDeviceClass **retpDevice);

		//	CDeviceClass virtuals

		virtual int CalcPowerUsed (SUpdateCtx &Ctx, CInstalledDevice *pDevice, CSpaceObject *pSource) override;
		virtual ICCItem *FindItemProperty (CItemCtx &Ctx, const CString &sName) override;
		virtual ItemCategories GetImplCategory (void) const override { return itemcatMiscDevice; }
		virtual int GetPowerRating (CItemCtx &Ctx, int *retiIdlePowerUse = NULL) const override;

	protected:
		virtual bool OnAccumulateEnhancements (CItemCtx &Device, CInstalledArmor *pTarget, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements) override;
		virtual bool OnAccumulateEnhancements (CItemCtx &Device, CInstalledDevice *pTarget, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements) override;

	private:
		struct SScalableStats
			{
			int iLevel;
			CEnhancementDesc Enhancements;	//	List of enhancements applied
			int iPowerUse;					//	Power use at this level
			};

		struct SInheritedStats
			{
			CString sType;
			CString sCriteria;
			CItemCriteria Criteria;
			CItemLevelCriteria LevelCheck;
			int iPowerUse;
			};

		bool AccumulateOldStyle (CItemCtx &Device, CInstalledDevice *pTarget, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements);
		void ApplyInherited (SScalableStats &Stats, const SInheritedStats &RootStats);
		ICCItem *FindEnhancementProperty (CItemCtx &Ctx, const CItemEnhancement &Enhancement, const CString &sName) const;
		const SScalableStats *GetStats (CItemCtx &Ctx) const;
		ALERROR InitFromEnhanceXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, const SInheritedStats &RootStats);
		ALERROR InitFromScalingXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, const SInheritedStats &RootStats);

		int m_iLevels;						//	Total number of levels (0 = old-style)
		TUniquePtr<SScalableStats[]> m_pDesc;

		//	Old-style enhancements

		CString m_sEnhancementType;			//	Type of enhancement
		CItemCriteria m_Criteria;			//	Only enhances items that match criteria

		int m_iDamageAdj;					//	Adjustment to damage

		int m_iActivateAdj;					//	Fire rate adj (100 = none)
		int m_iMinActivateDelay;			//	Minimum rate (0 = no min)
		int m_iMaxActivateDelay;			//	Maximum rate (0 = no max)

		bool m_bUseArray;					//	If TRUE, use this array
		int m_iDamageAdjArray[damageCount];	//	Adjustment to weapons damage

		int m_iPowerUse;
	};

class CMiscellaneousClass : public CDeviceClass
	{
	public:
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, CDeviceClass **retpDevice);

		//	CDeviceClass virtuals

		virtual int CalcPowerUsed (SUpdateCtx &Ctx, CInstalledDevice *pDevice, CSpaceObject *pSource) override;
		virtual int GetActivateDelay (CItemCtx &ItemCtx) const override;
		virtual ItemCategories GetImplCategory (void) const override { return itemcatMiscDevice; }
		virtual int GetCounter (CInstalledDevice *pDevice, CSpaceObject *pSource, CounterTypes *retiType = NULL, int *retiLevel = NULL) override;
		virtual int GetPowerRating (CItemCtx &Ctx, int *retiIdlePowerUse = NULL) const override;
		virtual bool SetCounter (CInstalledDevice *pDevice, CSpaceObject *pSource, CounterTypes iCounter, int iLevel) override;
		virtual bool ShowActivationDelayCounter (CSpaceObject *pSource, CInstalledDevice *pDevice) override;
		virtual void Update (CInstalledDevice *pDevice, CSpaceObject *pSource, SDeviceUpdateCtx &Ctx) override;

	private:
		CMiscellaneousClass (void);

		int GetCapacitorPowerUse (CInstalledDevice *pDevice, CSpaceObject *pSource) const;

		int m_iPowerRating;					//	If >0, power rating for device
											//		This value is required if
											//		m_iPowerForCapacitor is negative
		int m_iPowerUse;					//	If >0, power used per update cycle
		int m_iPowerToActivate;				//	If >0, power used per activation
		int m_iPowerForCapacitor;			//	Required if m_iPowerToActivate is >0
											//		This is power used per cycle
											//		to charge for activation. If
											//		<0, this is a percent of max
											//		reactor power.
	};

class CReactorClass : public CDeviceClass
	{
	public:
		~CReactorClass (void);

		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, CDeviceClass **retpDevice);

		//	CDeviceClass virtuals

		virtual bool FindDataField (const CString &sField, CString *retsValue) override;
		virtual ICCItem *FindItemProperty (CItemCtx &Ctx, const CString &sName) override;
		virtual ItemCategories GetImplCategory (void) const override { return itemcatReactor; }
		virtual int GetPowerOutput (CItemCtx &Ctx, DWORD dwFlags = 0) const override;
		virtual bool IsFuelCompatible (CItemCtx &Ctx, const CItem &FuelItem) override { return GetReactorDesc(Ctx)->IsFuelCompatible(FuelItem); }
		virtual void OnInstall (CInstalledDevice *pDevice, CSpaceObject *pSource, CItemListManipulator &ItemList) override;

	protected:
		virtual bool OnAccumulatePerformance (CItemCtx &ItemCtx, SShipPerformanceCtx &Ctx) const override;
		virtual bool OnCanBeDisabled (CItemCtx &Ctx) const override { return false; }
		virtual CString OnGetReference (CItemCtx &Ctx, const CItem &Ammo = CItem(), DWORD dwFlags = 0) override;

	private:
		CReactorClass (void);

		const CReactorDesc *GetReactorDesc (CItemCtx &Ctx, DWORD dwFlags = 0) const;
		int GetMaxPower (CItemCtx &ItemCtx, const CReactorDesc &Desc) const;
		void InitDamagedDesc (void) const;
		void InitEnhancedDesc (void) const;

		//  Descriptors (optional scaling)

		int m_iBaseLevel;                   //  Base level.
		int m_iLevels;                      //  Total number of levels.
		CReactorDesc *m_pDesc;              //  Array of entries, one per scaled level (minimum 1)

		//	Other properties

		int m_iExtraPowerPerCharge;			//	Adds to max power

		//  Damaged/enhanced
		//  We allocate these as needed; the array matches m_pDesc

		mutable CReactorDesc *m_pDamagedDesc;
		mutable CReactorDesc *m_pEnhancedDesc;
	};

class CRepairerClass : public CDeviceClass
	{
	public:
		enum ECachedHandlers
			{
			evtGetArmorRegen			= 0,

			evtCount					= 1,
			};

		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, CDeviceClass **retpDevice);
		bool FindEventHandlerRepairerClass (ECachedHandlers iEvent, SEventHandlerDesc *retEvent = NULL) const 
			{
			if (!m_CachedEvents[iEvent].pCode)
				return false;

			if (retEvent) *retEvent = m_CachedEvents[iEvent];
			return true;
			}

		//	CDeviceClass virtuals

		virtual int CalcPowerUsed (SUpdateCtx &Ctx, CInstalledDevice *pDevice, CSpaceObject *pSource) override;
		virtual ItemCategories GetImplCategory (void) const override { return itemcatMiscDevice; }
		virtual int GetPowerRating (CItemCtx &Ctx, int *retiIdlePowerUse = NULL) const override;
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) override;
		virtual void OnInstall (CInstalledDevice *pDevice, CSpaceObject *pSource, CItemListManipulator &ItemList) override;
		virtual void Update (CInstalledDevice *pDevice, CSpaceObject *pSource, SDeviceUpdateCtx &Ctx) override;

	private:
		CRepairerClass (void);

		void CalcRegen (CInstalledDevice *pDevice, CShip *pShip, int iSegment, int iTick, int *retiHP, int *retiPower);
		bool RepairShipArmor (CInstalledDevice *pDevice, CShip *pShip, SDeviceUpdateCtx &Ctx);
		bool RepairShipAttachedSections (CInstalledDevice *pDevice, CShip *pShip, SDeviceUpdateCtx &Ctx);
		bool RepairShipInterior (CInstalledDevice *pDevice, CShip *pShip, SDeviceUpdateCtx &Ctx);

		TArray<CRegenDesc> m_Repair;			//	Repair descriptor (by level)
		CRegenDesc m_CompartmentRepair;			//	Repair compartments
		int m_iPowerUse;						//	Power used while repairing

		SEventHandlerDesc m_CachedEvents[evtCount];		//	Cached events
	};

class CShieldClass : public CDeviceClass
	{
	public:
		enum ECachedHandlers
			{
			evtGetMaxHP					= 0,
			evtOnShieldDamage			= 1,
			evtOnShieldDown				= 2,

			evtCount					= 3,
			};

		struct SBalance
			{
			SBalance (void) :
					rBalance(0.0),
					iLevel(0),
					rDefenseRatio(0.0),
					rHPBalance(0.0),
					rDamageAdj(0.0),
					rRecoveryAdj(0.0),
					rPowerUse(0.0),
					rSlots(0.0),
					rSpecial(0.0),
					rCost(0.0)
				{ }

			Metric rBalance;				//	Total balance
			int iLevel;						//	Level for which we balanced
			Metric rDefenseRatio;			//	Ratio of effective HP to std weapons damage.

			Metric rHPBalance;				//	Balance contribution from raw HP
			Metric rRegen;					//	Balance contribution from regen
			Metric rDamageAdj;				//	Balance contribution from damage adj (including reflection)
			Metric rRecoveryAdj;			//	Balance contribution from fast depletion recovery
			Metric rPowerUse;				//	Balance from power use
			Metric rSlots;					//	Balance from slot count
			Metric rSpecial;				//	Balance from special abilities.

			Metric rCost;					//	Balance from cost
			};

		struct SStdStats
			{
			int iHP;									//	HP for std shield at this level
			int iRegen;									//	HP regen each 180 ticks
			Metric rCost;								//	Cost for std shield (credits)
			int iPower;									//	Power (in tenths of MWs)
			};

		struct SInitCtx
			{
			CItemType *pType = NULL;

			int iMaxCharges = -1;			//	CreateFromXML may set this to affect its CItemType
			};

		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, SInitCtx &InitCtx, CXMLElement *pDesc, CDeviceClass **retpShield);

		int CalcBalance (CItemCtx &ItemCtx, SBalance &retBalance) const;
		int CalcBalance (CItemCtx &ItemCtx) const { SBalance Balance; return CalcBalance(ItemCtx, Balance); }
		bool FindEventHandlerShieldClass (ECachedHandlers iEvent, SEventHandlerDesc *retEvent = NULL) const
			{
			if (!m_CachedEvents[iEvent].pCode)
				return false;

			if (retEvent) *retEvent = m_CachedEvents[iEvent];
			return true;
			}

		//	CDeviceClass virtuals

		virtual bool AbsorbsWeaponFire (CInstalledDevice *pDevice, CSpaceObject *pSource, CInstalledDevice *pWeapon) override;
		virtual bool AbsorbDamage (CInstalledDevice *pDevice, CSpaceObject *pShip, SDamageCtx &Ctx) override;
		virtual CShieldClass *AsShieldClass (void) override { return this; }
		virtual int CalcPowerUsed (SUpdateCtx &Ctx, CInstalledDevice *pDevice, CSpaceObject *pSource) override;
		virtual void Deplete (CInstalledDevice *pDevice, CSpaceObject *pSource) override;
		virtual bool FindDataField (const CString &sField, CString *retsValue) override;
		virtual ICCItem *FindItemProperty (CItemCtx &Ctx, const CString &sName) override;
		virtual int GetDamageEffectiveness (CSpaceObject *pAttacker, CInstalledDevice *pWeapon) override;
		virtual int GetHitPoints (CItemCtx &ItemCtx, int *retiMaxHP = NULL) const override;
		virtual ItemCategories GetImplCategory (void) const override { return itemcatShields; }
		virtual int GetPowerRating (CItemCtx &Ctx, int *retiIdlePowerUse = NULL) const override;
		virtual bool GetReferenceDamageAdj (const CItem *pItem, CSpaceObject *pInstalled, int *retiHP, int *retArray) const override;
		virtual int GetReflectChance (const CDeviceItem &DeviceItem, const DamageDesc &Damage) const override;
		virtual void GetStatus (const CInstalledDevice *pDevice, const CSpaceObject *pSource, int *retiStatus, int *retiMaxStatus) override;
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) override;
		virtual CEffectCreator *OnFindEffectCreator (const CString &sUNID) override;
		virtual void OnInstall (CInstalledDevice *pDevice, CSpaceObject *pSource, CItemListManipulator &ItemList) override;
		virtual void Recharge (CInstalledDevice *pDevice, CShip *pShip, int iStatus) override;
		virtual bool RequiresItems (void) const override;
		virtual void Reset (CInstalledDevice *pDevice, CSpaceObject *pSource) override;
		virtual void SetHitPoints (CItemCtx &ItemCtx, int iHP) override;
		virtual ESetPropertyResult SetItemProperty (CItemCtx &Ctx, const CString &sName, const ICCItem *pValue, CString *retsError) override;
		virtual void Update (CInstalledDevice *pDevice, CSpaceObject *pSource, SDeviceUpdateCtx &Ctx) override;

		static Metric GetStdCost (int iLevel);
		static int GetStdEffectiveHP (CUniverse &Universe, int iLevel);
		static int GetStdHP (int iLevel);
		static int GetStdPower (int iLevel);
		static int GetStdRegen (int iLevel);


	protected:
		virtual void OnAccumulateAttributes (const CDeviceItem &DeviceItem, const CItem &Ammo, TArray<SDisplayAttribute> *retList) const override;
		virtual void OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override;
		virtual CString OnGetReference (CItemCtx &Ctx, const CItem &Ammo = CItem(), DWORD dwFlags = 0) override;
		virtual void OnMarkImages (void) override;

	private:
		CShieldClass (void);

		Metric CalcBalanceDamageAdj (CItemCtx &ItemCtx) const;
		Metric CalcBalanceDefense (CItemCtx &ItemCtx, int iLevel, Metric rHP, Metric rRegen180, Metric *retrRatio = NULL) const;
		Metric CalcBalancePowerUse (CItemCtx &ItemCtx, const SStdStats &Stats) const;
		void CalcMinMaxHP (CItemCtx &Ctx, int iCharges, int iArmorSegs, int iTotalHP, int *retiMin, int *retiMax) const;

		static const DWORD FLAG_IGNORE_DISABLED = 0x00000001;
		Metric CalcRegen180 (CItemCtx &Ctx, DWORD dwFlags = 0) const;

		void CreateHitEffect (CInstalledDevice &Device, CSpaceObject &Ship, SDamageCtx &DamageCtx, CEffectCreator &Effect, const CVector &vPos) const;
		bool IsDepleted (CInstalledDevice *pDevice);
		int FireGetMaxHP (CInstalledDevice *pDevice, CSpaceObject *pSource, int iMaxHP) const;
		void FireOnShieldDamage (CItemCtx &ItemCtx, SDamageCtx &Ctx);
		void FireOnShieldDown (CInstalledDevice *pDevice, CSpaceObject *pSource);
		int GetAbsorbAdj (const CDeviceItem &DeviceItem, const CItemEnhancementStack &Enhancements, const DamageDesc &Damage) const;
		int GetDamageAdj (const DamageDesc &Damage, const CItemEnhancementStack *pEnhancements) const;
		int GetHPLeft (CItemCtx &Ctx) const;
		int GetMaxHP (CItemCtx &Ctx) const;
		int GetReferenceDepletionDelay (void) const;
		int GetReflectChance (const CDeviceItem &DeviceItem, const CItemEnhancementStack &Enhancements, const DamageDesc &Damage, int iHP, int iMaxHP) const;
		void SetDepleted (CInstalledDevice *pDevice, CSpaceObject *pSource);
		void SetHPLeft (CInstalledDevice *pDevice, CSpaceObject *pSource, int iHP, bool bConsumeCharges = false);
		bool UpdateDepleted (CInstalledDevice *pDevice);

		int m_iHitPoints;						//	Max HP
		int m_iArmorShield;						//	If non-zero then this is the
												//		This is the number of shield
												//		HPs that are generated by
												//		10 HP of average armor
		int m_iAbsorbAdj[damageCount];			//	Absorbtion adjustments
		int m_iDamageAdjLevel;					//	Level to use for intrinsic damage adj
		CDamageAdjDesc m_DamageAdj;				//	Adjustments for damage type
		CRegenDesc m_Regen;						//	Regeneration rate
		int m_iDepletionTicks;					//	Ticks to recover from full depletion
		int m_iPowerUse;						//	Power used during regeneration (1/10 megawatt)
		int m_iIdlePowerUse;					//	Power used to maintain shields
		DamageTypeSet m_WeaponSuppress;			//	Types of weapons suppressed
		DamageTypeSet m_Reflective;				//	Types of damage reflected
		int m_iTimeBetweenFlashEffects;			//  Minimum time between flash effects in ticks

		int m_iExtraHPPerCharge;				//	Extra HP for each point of charge
		int m_iExtraPowerPerCharge;				//	Extra power use for each point of charge (1/10 megawatt)
		int m_iExtraRegenPerCharge;				//	Extra regen/180 ticks per point of charge

		DWORD m_fHasNonRegenHPBonus:1;			//	If TRUE, charges are non-regenerating HP
		DWORD m_fRegenByShieldLevel:1;			//	If TRUE, regen decreases with low shield level
		DWORD m_fAimReflection:1;				//	If TRUE, reflected shots are aimed at nearest enemy
		DWORD m_fSpare4:1;
		DWORD m_fSpare5:1;
		DWORD m_fSpare6:1;
		DWORD m_fSpare7:1;
		DWORD m_fSpare8:1;
		DWORD m_dwSpare:24;

		SEventHandlerDesc m_CachedEvents[evtCount];		//	Cached events

		CEffectCreatorRef m_pHitEffect;				//	Effect when shield is hit, appearing at hit location
		CEffectCreatorRef m_pFlashEffect;			//	Effect when shield is hit, appearing on ship
	};

class CSolarDeviceClass : public CDeviceClass
	{
	public:
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, CDeviceClass **retpDevice);

		//	CDeviceClass virtuals

		virtual int CalcPowerUsed (SUpdateCtx &Ctx, CInstalledDevice *pDevice, CSpaceObject *pSource) override;
		virtual ItemCategories GetImplCategory (void) const override { return itemcatMiscDevice; }
		virtual void Update (CInstalledDevice *pDevice, CSpaceObject *pSource, SDeviceUpdateCtx &Ctx) override;
		virtual void OnInstall (CInstalledDevice *pDevice, CSpaceObject *pSource, CItemListManipulator &ItemList) override;

	protected:

		virtual bool OnCanBeDisabled (CItemCtx &Ctx) const override { return false; }

	private:
		CSolarDeviceClass (void);

		int m_iPowerGen;						//	Power generated at maximum solar intensity (1/10th MW).
		int m_iRefuel;							//	Units of fuel recharged per 10 ticks
												//	at 100% intensity.
	};

#include "TSEWeaponClassImpl.h"
