//	TSEDeviceClassesImpl.h
//
//	Transcendence Device Classes
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CAutoDefenseClass : public CDeviceClass
	{
	public:
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, CDeviceClass **retpDevice);

		//	CDeviceClass virtuals

		virtual int CalcPowerUsed (CInstalledDevice *pDevice, CSpaceObject *pSource) override;
		virtual int GetActivateDelay (CInstalledDevice *pDevice, CSpaceObject *pSource) override;
		virtual ItemCategories GetImplCategory (void) const override { return itemcatMiscDevice; }
		virtual DamageTypes GetDamageType (CItemCtx &Ctx, const CItem &Ammo = CItem()) const override;
		virtual ICCItem *GetItemProperty (CItemCtx &Ctx, const CString &sProperty) override;
		virtual int GetPowerRating (CItemCtx &Ctx) const override;
		virtual bool GetReferenceDamageType (CItemCtx &Ctx, const CItem &Ammo, DamageTypes *retiDamage, CString *retsReference) const override;
		virtual bool IsAutomatedWeapon (void) override { return true; }
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) override;
		virtual void Update (CInstalledDevice *pDevice, 
							 CSpaceObject *pSource, 
							 int iTick,
							 bool *retbSourceDestroyed,
							 bool *retbConsumedItems = NULL) override;

	protected:
		virtual void OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override;
		virtual CString OnGetReference (CItemCtx &Ctx, const CItem &Ammo = CItem(), DWORD dwFlags = 0) override;

	private:
		enum TargetingSystemTypes
			{
			trgMissiles,				//	Target nearest missile
			trgCriteria,				//	Target nearest object that matches criteria
			trgFunction,				//	Target computed by a function
			};

		CAutoDefenseClass (void);

		inline CDeviceClass *GetWeapon (void) const { return m_pWeapon; }

		TargetingSystemTypes m_iTargeting;
		CSpaceObject::Criteria m_TargetCriteria;
		Metric m_rInterceptRange;

		int m_iRechargeTicks;
		CDeviceClassRef m_pWeapon;
	};

class CCargoSpaceClass : public CDeviceClass
	{
	public:
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, CDeviceClass **retpDevice);

		//	CDeviceClass virtuals

		virtual bool CanBeDamaged (void) override { return false; }
		virtual bool CanBeDisabled (CItemCtx &Ctx) override { return false; }
		virtual bool FindDataField (const CString &sField, CString *retsValue) override;
		virtual ItemCategories GetImplCategory (void) const override { return itemcatCargoHold; }
		virtual int GetCargoSpace (void) override { return m_iCargoSpace; }
		virtual void OnInstall (CInstalledDevice *pDevice, CSpaceObject *pSource, CItemListManipulator &ItemList) override;

	protected:
		virtual CString OnGetReference (CItemCtx &Ctx, const CItem &Ammo = CItem(), DWORD dwFlags = 0) override;

	private:
		CCargoSpaceClass (void);

		int m_iCargoSpace;
	};

class CCyberDeckClass : public CDeviceClass
	{
	public:
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, CDeviceClass **retpDevice);

		//	CDeviceClass virtuals

		virtual bool Activate (CInstalledDevice *pDevice, 
							   CSpaceObject *pSource, 
							   CSpaceObject *pTarget,
							   bool *retbSourceDestroyed,
							   bool *retbConsumedItems = NULL) override;
		virtual bool CanHitFriends (void) override { return false; }
		virtual int GetActivateDelay (CInstalledDevice *pDevice, CSpaceObject *pSource) override { return 30; }
		virtual ItemCategories GetImplCategory (void) const override { return itemcatWeapon; }
		virtual DamageTypes GetDamageType (CItemCtx &Ctx, const CItem &Ammo = CItem()) const override { return damageGeneric; }
		virtual Metric GetMaxEffectiveRange (CSpaceObject *pSource, CInstalledDevice *pDevice, CSpaceObject *pTarget);
		virtual void GetSelectedVariantInfo (CSpaceObject *pSource, 
											 CInstalledDevice *pDevice,
											 CString *retsLabel,
											 int *retiAmmoLeft,
											 CItemType **retpType = NULL) override;
		virtual int GetValidVariantCount (CSpaceObject *pSource, CInstalledDevice *pDevice) override { return 1; }
		virtual int GetWeaponEffectiveness (CSpaceObject *pSource, CInstalledDevice *pDevice, CSpaceObject *pTarget) override;
		virtual bool IsVariantSelected (CSpaceObject *pSource, CInstalledDevice *pDevice) override { return true; }
		virtual bool IsWeaponAligned (CSpaceObject *pShip, CInstalledDevice *pDevice, CSpaceObject *pTarget, int *retiAimAngle = NULL, int *retiFireAngle = NULL) override;
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
		virtual ICCItem *GetItemProperty (CItemCtx &Ctx, const CString &sProperty) override;
		virtual int GetPowerRating (CItemCtx &Ctx) const override;
		virtual void OnInstall (CInstalledDevice *pDevice, CSpaceObject *pSource, CItemListManipulator &ItemList) override;

	protected:

		//	CDeviceClass virtuals

		virtual void OnAccumulateAttributes (CItemCtx &ItemCtx, const CItem &Ammo, TArray<SDisplayAttribute> *retList) override;
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
        void InitDamagedDesc (void) const;
        void InitEnhancedDesc (void) const;
        static ALERROR InitStatsFromXML (SDesignLoadCtx &Ctx, int iLevel, DWORD dwUNID, CXMLElement *pDesc, SScalableStats &retStats);

        int m_iLevels;                      //  Total number of levels.
        SScalableStats *m_pDesc;            //  Array of entries, one per scaled level (minimum 1)

        mutable SScalableStats *m_pDamagedDesc;
        mutable SScalableStats *m_pEnhancedDesc;

		CDriveDesc m_DriveDesc;
		CDriveDesc m_DamagedDriveDesc;
		CDriveDesc m_EnhancedDriveDesc;
	};

class CEnhancerClass : public CDeviceClass
	{
	public:
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, CDeviceClass **retpDevice);

		//	CDeviceClass virtuals

		virtual int CalcPowerUsed (CInstalledDevice *pDevice, CSpaceObject *pSource) override;
		virtual ItemCategories GetImplCategory (void) const override { return itemcatMiscDevice; }
		virtual bool GetDeviceEnhancementDesc (CInstalledDevice *pDevice, CSpaceObject *pSource, CInstalledDevice *pWeapon, SDeviceEnhancementDesc *retDesc) override;
		virtual int GetPowerRating (CItemCtx &Ctx) const override { return m_iPowerUse; }

	protected:
		virtual bool OnAccumulateEnhancements (CItemCtx &Device, CInstalledDevice *pTarget, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements) override;

	private:
		CEnhancerClass (void);

		CString m_sEnhancementType;			//	Type of enhancement
		CItemCriteria m_Criteria;			//	Only enhances items that match criteria

		int m_iDamageAdj;					//	Adjustment to damage

		int m_iActivateAdj;					//	Fire rate adj (100 = none)
		int m_iMinActivateDelay;			//	Minimum rate (0 = no min)
		int m_iMaxActivateDelay;			//	Maximum rate (0 = no max)

		bool m_bUseArray;						//	If TRUE, use this array
		int m_iDamageAdjArray[damageCount];		//	Adjustment to weapons damage

		int m_iPowerUse;
	};

class CMiscellaneousClass : public CDeviceClass
	{
	public:
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, CDeviceClass **retpDevice);

		//	CDeviceClass virtuals

		virtual int CalcPowerUsed (CInstalledDevice *pDevice, CSpaceObject *pSource) override;
		virtual int GetActivateDelay (CInstalledDevice *pDevice, CSpaceObject *pSource) override;
		virtual ItemCategories GetImplCategory (void) const override { return itemcatMiscDevice; }
		virtual int GetCounter (CInstalledDevice *pDevice, CSpaceObject *pSource, CounterTypes *retiType = NULL) override;
		virtual int GetPowerRating (CItemCtx &Ctx) const override;
		virtual bool ShowActivationDelayCounter (CSpaceObject *pSource, CInstalledDevice *pDevice) override;
		virtual void Update (CInstalledDevice *pDevice, 
							 CSpaceObject *pSource, 
							 int iTick,
							 bool *retbSourceDestroyed,
							 bool *retbConsumedItems = NULL) override;

	private:
		CMiscellaneousClass (void);

		int GetCapacitorPowerUse (CInstalledDevice *pDevice, CSpaceObject *pSource);

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
		static bool FindDataField (const CReactorDesc &Desc, const CString &sField, CString *retsValue);
        static const CReactorDesc *GetReactorDescForItem (CItemCtx &ItemCtx);

		//	CDeviceClass virtuals

		virtual bool CanBeDisabled (CItemCtx &Ctx) override { return false; }
		virtual bool FindDataField (const CString &sField, CString *retsValue) override;
		virtual ItemCategories GetImplCategory (void) const override { return itemcatReactor; }
		virtual ICCItem *GetItemProperty (CItemCtx &Ctx, const CString &sName) override;
		virtual const CReactorDesc *GetReactorDesc (CItemCtx &Ctx) override;
		virtual bool IsFuelCompatible (CItemCtx &Ctx, const CItem &FuelItem) override { return GetReactorDesc(Ctx)->IsFuelCompatible(FuelItem); }
		virtual void OnInstall (CInstalledDevice *pDevice, CSpaceObject *pSource, CItemListManipulator &ItemList) override;

	protected:
		virtual CString OnGetReference (CItemCtx &Ctx, const CItem &Ammo = CItem(), DWORD dwFlags = 0) override;

	private:
		CReactorClass (void);
        void InitDamagedDesc (void) const;
        void InitEnhancedDesc (void) const;

        //  Scalable items

        int m_iBaseLevel;                   //  Base level.
        int m_iLevels;                      //  Total number of levels.
        CReactorDesc *m_pDesc;              //  Array of entries, one per scaled level (minimum 1)

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
		inline bool FindEventHandlerRepairerClass (ECachedHandlers iEvent, SEventHandlerDesc *retEvent = NULL) const { if (retEvent) *retEvent = m_CachedEvents[iEvent]; return (m_CachedEvents[iEvent].pCode != NULL); }

		//	CDeviceClass virtuals

		virtual int CalcPowerUsed (CInstalledDevice *pDevice, CSpaceObject *pSource) override;
		virtual ItemCategories GetImplCategory (void) const override { return itemcatMiscDevice; }
		virtual int GetPowerRating (CItemCtx &Ctx) const override { return 2 * m_iPowerUse; }
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) override;
		virtual void OnInstall (CInstalledDevice *pDevice, CSpaceObject *pSource, CItemListManipulator &ItemList) override;
		virtual void Update (CInstalledDevice *pDevice, 
							 CSpaceObject *pSource, 
							 int iTick,
							 bool *retbSourceDestroyed,
							 bool *retbConsumedItems = NULL) override;

	private:
		CRepairerClass (void);

		void CalcRegen (CInstalledDevice *pDevice, CShip *pShip, int iSegment, int iTick, int *retiHP, int *retiPower);

		TArray<CRegenDesc> m_Repair;			//	Repair descriptor (by level)
		int m_iPowerUse;						//	Power used for each hp of repair

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

		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, CDeviceClass **retpShield);
		inline bool FindEventHandlerShieldClass (ECachedHandlers iEvent, SEventHandlerDesc *retEvent = NULL) const { if (retEvent) *retEvent = m_CachedEvents[iEvent]; return (m_CachedEvents[iEvent].pCode != NULL); }

		//	CDeviceClass virtuals

		virtual bool AbsorbsWeaponFire (CInstalledDevice *pDevice, CSpaceObject *pSource, CInstalledDevice *pWeapon) override;
		virtual bool AbsorbDamage (CInstalledDevice *pDevice, CSpaceObject *pShip, SDamageCtx &Ctx) override;
		virtual bool Activate (CInstalledDevice *pDevice, 
							   CSpaceObject *pSource, 
							   CSpaceObject *pTarget,
							   bool *retbSourceDestroyed,
							   bool *retbConsumedItems = NULL) override;
		virtual int CalcPowerUsed (CInstalledDevice *pDevice, CSpaceObject *pSource) override;
		virtual void Deplete (CInstalledDevice *pDevice, CSpaceObject *pSource) override;
		virtual bool FindDataField (const CString &sField, CString *retsValue) override;
		virtual ItemCategories GetImplCategory (void) const override { return itemcatShields; }
		virtual ICCItem *GetItemProperty (CItemCtx &Ctx, const CString &sName) override;
		virtual int GetDamageEffectiveness (CSpaceObject *pAttacker, CInstalledDevice *pWeapon) override;
		virtual int GetPowerRating (CItemCtx &Ctx) const override;
		virtual bool GetReferenceDamageAdj (const CItem *pItem, CSpaceObject *pInstalled, int *retiHP, int *retArray) const override;
		virtual void GetStatus (CInstalledDevice *pDevice, CSpaceObject *pSource, int *retiStatus, int *retiMaxStatus) override;
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) override;
		virtual CEffectCreator *OnFindEffectCreator (const CString &sUNID) override;
		virtual void OnInstall (CInstalledDevice *pDevice, CSpaceObject *pSource, CItemListManipulator &ItemList) override;
		virtual void Recharge (CInstalledDevice *pDevice, CShip *pShip, int iStatus) override;
		virtual bool RequiresItems (void) const override;
		virtual void Reset (CInstalledDevice *pDevice, CSpaceObject *pSource) override;
		virtual bool SetItemProperty (CItemCtx &Ctx, const CString &sName, ICCItem *pValue, CString *retsError) override;
		virtual void Update (CInstalledDevice *pDevice, 
							 CSpaceObject *pSource, 
							 int iTick,
							 bool *retbSourceDestroyed,
							 bool *retbConsumedItems = NULL) override;

		static int GetStdCost (int iLevel);
		static int GetStdEffectiveHP (int iLevel);
		static int GetStdHP (int iLevel);
		static int GetStdPower (int iLevel);
		static int GetStdRegen (int iLevel);


	protected:
		virtual void OnAccumulateAttributes (CItemCtx &ItemCtx, const CItem &Ammo, TArray<SDisplayAttribute> *retList) override;
		virtual void OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override;
		virtual CString OnGetReference (CItemCtx &Ctx, const CItem &Ammo = CItem(), DWORD dwFlags = 0) override;
		virtual void OnMarkImages (void) override;

	private:
		CShieldClass (void);

		int CalcBalance (void);
		void CalcMinMaxHP (CItemCtx &Ctx, int iCharges, int iArmorSegs, int iTotalHP, int *retiMin, int *retiMax) const;
		bool IsDepleted (CInstalledDevice *pDevice);
		int FireGetMaxHP (CInstalledDevice *pDevice, CSpaceObject *pSource, int iMaxHP) const;
		void FireOnShieldDamage (CItemCtx &ItemCtx, SDamageCtx &Ctx);
		void FireOnShieldDown (CInstalledDevice *pDevice, CSpaceObject *pSource);
		int GetDamageAdj (const DamageDesc &Damage, const CItemEnhancementStack *pEnhancements) const;
		int GetHPLeft (CItemCtx &Ctx);
		int GetMaxHP (CItemCtx &Ctx);
		bool UpdateDepleted (CInstalledDevice *pDevice);
		void SetDepleted (CInstalledDevice *pDevice, CSpaceObject *pSource);
		void SetHPLeft (CInstalledDevice *pDevice, int iHP);

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

		int m_iMaxCharges;						//	Max charges
		int m_iExtraHPPerCharge;				//	Extra HP for each point of charge
		int m_iExtraPowerPerCharge;				//	Extra power use for each point of charge (1/10 megawatt)
		int m_iExtraRegenPerCharge;				//	Extra regen/180 ticks per point of charge

		SEventHandlerDesc m_CachedEvents[evtCount];		//	Cached events

		CEffectCreatorRef m_pHitEffect;			//	Effect when shield is hit
	};

class CSolarDeviceClass : public CDeviceClass
	{
	public:
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, CDeviceClass **retpDevice);

		//	CDeviceClass virtuals

		virtual bool CanBeDisabled (CItemCtx &Ctx) override { return false; }
		virtual ItemCategories GetImplCategory (void) const override { return itemcatMiscDevice; }
		virtual void Update (CInstalledDevice *pDevice, 
							 CSpaceObject *pSource, 
							 int iTick,
							 bool *retbSourceDestroyed,
							 bool *retbConsumedItems = NULL) override;
		virtual void OnInstall (CInstalledDevice *pDevice, CSpaceObject *pSource, CItemListManipulator &ItemList) override;

	private:
		CSolarDeviceClass (void);

		int m_iRefuel;							//	Units of fuel recharged per 10 ticks
												//	at 100% intensity.
	};

class CWeaponClass : public CDeviceClass
	{
	public:
		enum ECachedHandlers
			{
			evtOnFireWeapon				= 0,

			evtCount					= 1,
			};

        struct SBalance
            {
            SBalance (void) :
                    rBalance(0.0),
                    iLevel(0),
                    rStdDamage180(0.0),
                    rDamageHP(0.0),
                    rDamageMult(0.0),
                    rDamage(0.0),
                    rDamageType(0.0),
                    rStdAmmoCost(0.0),
                    rStdAmmoMass(0.0),
                    rAmmo(0.0),
                    rOmni(0.0),
                    rTracking(0.0),
                    rRange(0.0),
                    rSpeed(0.0),
                    rProjectileHP(0.0),
                    rPower(0.0),
                    rCost(0.0),
                    rSlots(0.0),
                    rExternal(0.0),
                    rLinkedFire(0.0),
                    rRecoil(0.0),
                    rRadiation(0.0),
                    rDeviceDisrupt(0.0),
                    rDeviceDamage(0.0),
                    rDisintegration(0.0),
                    rShieldPenetrate(0.0),
                    rShatter(0.0),
                    rShield(0.0),
                    rArmor(0.0),
                    rWMD(0.0),
                    rMining(0.0)
                { }

            Metric rBalance;        //  Total balance
            int iLevel;             //  Level for which we balanced

            Metric rStdDamage180;   //  Standard damage (for level and fire rate)
            Metric rDamageHP;       //  HP damage per projectile
            Metric rDamageMult;     //  Effective number of projectiles
            Metric rDamage180;      //  Damage per 180 ticks
            Metric rDamage;         //  Damage balance component
            Metric rDamageType;     //  Damage type balance contribution

            Metric rStdAmmoCost;    //  Standard ammo cost (for level and fire rate)
            Metric rStdAmmoMass;    //  Standard ammo mass (for level and fire rate)
            Metric rAmmo;           //  Ammo contribution
            Metric rOmni;           //  Omni and swivel component
            Metric rTracking;       //  Tracking component

            Metric rRange;          //  Range component
            Metric rSpeed;          //  Speed component
            Metric rProjectileHP;   //  HP and interaction balance component
            Metric rPower;          //  Power use component
            Metric rCost;           //  Cost component to balance
            Metric rSlots;          //  Slot component
            Metric rExternal;       //  External component
            Metric rLinkedFire;     //  Weapon is linked-fire
            Metric rRecoil;         //  Weapon has recoil

            Metric rRadiation;      //  Bonus for radiation
            Metric rDeviceDisrupt;  //  Bonus for device disrupt
            Metric rDeviceDamage;   //  Bonus for device damage
            Metric rDisintegration; //  Bonus for disintegration
            Metric rShatter;        //  Bonus for shatter damage
            Metric rShieldPenetrate;//  Bonus for shield penetration
            Metric rShield;         //  Bonus for shield damage
            Metric rArmor;          //  Bonus for armor damage
            Metric rWMD;            //  Bonus for WMD
            Metric rMining;         //  Bonus for mining
            };

		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, CDeviceClass **retpWeapon);
		virtual ~CWeaponClass (void);

		int CalcBalance (CItemCtx &ItemCtx, SBalance &retBalance) const;
		inline bool FindEventHandlerWeaponClass (ECachedHandlers iEvent, SEventHandlerDesc *retEvent = NULL) const { if (retEvent) *retEvent = m_CachedEvents[iEvent]; return (m_CachedEvents[iEvent].pCode != NULL); }
        CItemType *GetAmmoItem (int iIndex) const;
        int GetAmmoItemCount (void) const;
        CWeaponFireDesc *GetWeaponFireDesc (CItemCtx &ItemCtx, const CItem &Ammo = CItem()) const;

		static int GetStdDamage (int iLevel);
		static bool IsStdDamageType (DamageTypes iDamageType, int iLevel);

		//	CDeviceClass virtuals

		virtual bool Activate (CInstalledDevice *pDevice, 
							   CSpaceObject *pSource, 
							   CSpaceObject *pTarget,
							   bool *retbSourceDestroyed,
							   bool *retbConsumedItems = NULL) override;
		virtual CWeaponClass *AsWeaponClass (void) override { return this; }
		virtual int CalcFireSolution (CInstalledDevice *pDevice, CSpaceObject *pSource, CSpaceObject *pTarget) override;
		virtual int CalcPowerUsed (CInstalledDevice *pDevice, CSpaceObject *pSource) override;
		virtual bool CanRotate (CItemCtx &Ctx, int *retiMinFireArc = NULL, int *retiMaxFireArc = NULL) const override;
		virtual int GetActivateDelay (CInstalledDevice *pDevice, CSpaceObject *pSource) override;
        virtual ICCItem *GetAmmoItemProperty (CItemCtx &Ctx, const CItem &Ammo, const CString &sProperty) override;
		virtual int GetAmmoVariant (const CItemType *pItem) const override;
		virtual ItemCategories GetImplCategory (void) const override;
		virtual int GetCounter (CInstalledDevice *pDevice, CSpaceObject *pSource, CounterTypes *retiType = NULL) override;
		virtual int GetDefaultFireAngle (CInstalledDevice *pDevice, CSpaceObject *pSource) const override;
		virtual void Update (CInstalledDevice *pDevice, 
							 CSpaceObject *pSource, 
							 int iTick,
							 bool *retbSourceDestroyed,
							 bool *retbConsumedItems = NULL) override;

		virtual bool FindAmmoDataField (const CItem &Ammo, const CString &sField, CString *retsValue) const override;
		virtual bool FindDataField (const CString &sField, CString *retsValue) override;
		virtual const DamageDesc *GetDamageDesc (CItemCtx &Ctx) override;
		virtual DamageTypes GetDamageType (CItemCtx &Ctx, const CItem &Ammo = CItem()) const override;
		virtual ICCItem *GetItemProperty (CItemCtx &Ctx, const CString &sName) override;
		virtual DWORD GetLinkedFireOptions (CItemCtx &Ctx) override;
		virtual Metric GetMaxEffectiveRange (CSpaceObject *pSource, CInstalledDevice *pDevice, CSpaceObject *pTarget) override;
		virtual int GetPowerRating (CItemCtx &Ctx) const override;
		virtual bool GetReferenceDamageType (CItemCtx &Ctx, const CItem &Ammo, DamageTypes *retiDamage, CString *retsReference) const override;
		virtual void GetSelectedVariantInfo (CSpaceObject *pSource, 
											 CInstalledDevice *pDevice,
											 CString *retsLabel,
											 int *retiAmmoLeft,
											 CItemType **retpType = NULL) override;
		virtual int GetValidVariantCount (CSpaceObject *pSource, CInstalledDevice *pDevice) override;
		virtual int GetWeaponEffectiveness (CSpaceObject *pSource, CInstalledDevice *pDevice, CSpaceObject *pTarget) override;
		virtual bool IsAmmoWeapon (void) override;
		virtual bool IsAreaWeapon (CSpaceObject *pSource, CInstalledDevice *pDevice) override;
		virtual bool IsTrackingWeapon (CItemCtx &Ctx) override;
		virtual bool IsVariantSelected (CSpaceObject *pSource, CInstalledDevice *pDevice) override;
		virtual bool IsWeaponAligned (CSpaceObject *pShip, CInstalledDevice *pDevice, CSpaceObject *pTarget, int *retiAimAngle = NULL, int *retiFireAngle = NULL) override;
		virtual bool NeedsAutoTarget (CItemCtx &Ctx, int *retiMinFireArc = NULL, int *retiMaxFireArc = NULL) override;
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) override;
		virtual bool RequiresItems (void) const override;
		virtual bool SelectFirstVariant (CSpaceObject *pSource, CInstalledDevice *pDevice) override;
		virtual bool SelectNextVariant (CSpaceObject *pSource, CInstalledDevice *pDevice, int iDir = 1) override;
		virtual bool ValidateSelectedVariant (CSpaceObject *pSource, CInstalledDevice *pDevice) override;

	protected:
		virtual void OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override;
		virtual void OnAccumulateAttributes (CItemCtx &ItemCtx, const CItem &Ammo, TArray<SDisplayAttribute> *retList) override;
		virtual CEffectCreator *OnFindEffectCreator (const CString &sUNID) override;
		virtual CString OnGetReference (CItemCtx &Ctx, const CItem &Ammo = CItem(), DWORD dwFlags = 0) override;
		virtual void OnMarkImages (void) override;

	private:
		enum ConfigurationTypes
			{
			ctSingle,				//	single shot
			ctDual,					//	dual, parallel shots
			ctWall,					//	five parallel shots
			ctSpread2,				//	dual, spread shots
			ctSpread3,				//	three, spread shots
			ctSpread5,				//	five, spread shots
			ctDualAlternating,		//	alternate barrels
			ctCustom,				//	custom configuration
			};

		struct SConfigDesc
			{
			DiceRange Angle;		//	Offset from fire angle
			int iPosAngle;			//	Origin of shot
			Metric rPosRadius;		//	Origin of shot
			};

		struct SShotDesc
			{
			CItemTypeRef pAmmoType;	//	ItemType for ammo (may be NULL)
			CWeaponFireDesc *pDesc;	//	Pointer to descriptor (may be external)
			bool bOwned;			//	TRUE if we own the descriptor (otherwise, it
                                    //      may be owned by a missile).
			};

		enum EOnFireWeaponResults
			{
			resDefault,
			resNoShot,
			resShotFired,
			};

		enum Constants
			{
			CONTINUOUS_START = 0xff,
			};

		CWeaponClass (void);

		int CalcActivateDelay (CItemCtx &ItemCtx) const;
		int CalcConfiguration (CItemCtx &ItemCtx, CWeaponFireDesc *pShot, int iFireAngle, CVector *ShotPosOffset, int *ShotDir, bool bSetAlternating) const;
		Metric CalcConfigurationMultiplier (CWeaponFireDesc *pShot = NULL, bool bIncludeFragments = true) const;
		Metric CalcDamage (CWeaponFireDesc *pShot, const CItemEnhancementStack *pEnhancements = NULL, DWORD dwDamageFlags = 0) const;
		Metric CalcDamagePerShot (CWeaponFireDesc *pShot, const CItemEnhancementStack *pEnhancements = NULL, DWORD dwDamageFlags = 0) const;
		int CalcFireAngle (CItemCtx &ItemCtx, Metric rSpeed, CSpaceObject *pTarget, bool *retbOutOfArc);
        int CalcLevel (CWeaponFireDesc *pShot) const;
        int CalcRotateRange (CItemCtx &ItemCtx) const;
		EOnFireWeaponResults FireOnFireWeapon (CItemCtx &ItemCtx, 
											   CWeaponFireDesc *pShot,
											   const CVector &vSource,
											   CSpaceObject *pTarget,
											   int iFireAngle,
											   int iRepeatingCount);
		bool FireWeapon (CInstalledDevice *pDevice, 
						 CWeaponFireDesc *pShot, 
						 CSpaceObject *pSource, 
						 CSpaceObject *pTarget,
						 int iRepeatingCount,
						 bool *retbSourceDestroyed,
						 bool *retbConsumedItems);
		CWeaponFireDesc *GetReferenceShotData (CWeaponFireDesc *pShot, int *retiFragments = NULL) const;
		inline bool IsCapacitorEnabled (void) { return (m_Counter == cntCapacitor); }
		inline bool IsCounterEnabled (void) { return (m_Counter != cntNone); }
		bool IsDirectional (CInstalledDevice *pDevice, int *retiMinFireArc = NULL, int *retiMaxFireArc = NULL);
		bool IsOmniDirectional (CInstalledDevice *pDevice);
		inline bool IsTemperatureEnabled (void) { return (m_Counter == cntTemperature); }
		bool VariantIsValid (CSpaceObject *pSource, CInstalledDevice *pDevice, CWeaponFireDesc &ShotData);

		int GetAlternatingPos (CInstalledDevice *pDevice) const;
		DWORD GetContinuousFire (CInstalledDevice *pDevice) const;
		int GetCurrentVariant (CInstalledDevice *pDevice) const;
		void SetAlternatingPos (CInstalledDevice *pDevice, int iAlternatingPos) const;
		void SetContinuousFire (CInstalledDevice *pDevice, DWORD dwContinuous) const;
		void SetCurrentVariant (CInstalledDevice *pDevice, int iVariant) const;

		int m_iFireRate;						//	Ticks between shots
		int m_iFireRateSecs;					//	Game seconds between shots
		int m_iPowerUse;						//	Power use to recharge capacitors (1/10 megawatt)
		int m_iIdlePowerUse;					//	Power use when capacitors fully charged
		int m_iRecoil;							//	0-7 (as per momentum damage)
		int m_iFailureChance;					//	Chance of failure

		bool m_bLauncher;						//	Generic missile launcher
		bool m_bOmnidirectional;				//	Omnidirectional
		bool m_bMIRV;							//	Each shot seeks an independent target
		bool m_bReportAmmo;						//	Report count of ammo shot even if not a launcher
		int m_iMinFireArc;						//	Min angle of fire arc (degrees)
		int m_iMaxFireArc;						//	Max angle of fire arc (degrees)
		DWORD m_dwLinkedFireOptions;			//	Linked fire options

		TArray<SShotDesc> m_ShotData;			//	Desc for each shot variation

		ConfigurationTypes m_Configuration;		//	Shot configuration;
		int m_iConfigCount;						//	Number of shots for custom configurations
		SConfigDesc *m_pConfig;					//	Custom configuration (may be NULL)
		int m_iConfigAimTolerance;				//	Aim tolerance
		bool m_bConfigAlternating;				//	Fire each shot in turn

		bool m_bCharges;						//	TRUE if weapon has charges instead of ammo

		CounterTypes m_Counter;					//	Counter type
		int m_iCounterUpdateRate;				//	Ticks to update counter
		int m_iCounterUpdate;					//	Inc/dec value per update
		int m_iCounterActivate;					//	Inc/dec value per shot

		bool m_bTargetStationsOnly;				//	Do not target ships

		SEventHandlerDesc m_CachedEvents[evtCount];	//	Cached events

	friend CObjectClass<CWeaponClass>;
	};

