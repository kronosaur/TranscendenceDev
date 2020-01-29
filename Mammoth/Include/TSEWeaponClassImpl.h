//	TSEWeaponClassImpl.h
//
//	Transcendence Device Classes
//	Copyright (c) 2020 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

struct SShotOrigin
	{
	CVector vPos;
	int iDir = -1;
	};

class CConfigurationDesc
	{
	public:
		enum ETypes
			{
			ctUnknown,

			ctSingle,						//	single shot
			ctDual,							//	dual, parallel shots
			ctWall,							//	five parallel shots
			ctSpread2,						//	dual, spread shots
			ctSpread3,						//	three, spread shots
			ctSpread5,						//	five, spread shots
			ctDualAlternating,				//	alternate barrels
			ctCustom,						//	custom configuration
			};

		TArray<SShotOrigin> CalcOrigins (const CVector &vSource, int iFireAngle = 0, int iPolarity = -1, Metric rScale = 1.0) const;
		int GetAimTolerance (int iFireDelay = 8) const;
		int GetCustomConfigCount (void) const { return m_Custom.GetCount(); }
		int GetCustomConfigFireAngle (int iIndex, int iFireAngle = 0) const { return AngleMod(iFireAngle + m_Custom[iIndex].Angle.Roll()); }
		CVector GetCustomConfigPos (int iIndex, int iFireAngle = 0) const { return PolarToVector(AngleMod(iFireAngle + m_Custom[iIndex].iPosAngle), m_Custom[iIndex].rPosRadius); }
		Metric GetMultiplier (void) const;
		ETypes GetType (void) const { return m_iType; }
		ALERROR InitFromWeaponClassXML (SDesignLoadCtx &Ctx, const CXMLElement &Desc);
		bool IsAlternating (void) const { return (m_bCustomAlternating || m_iType == ctDualAlternating); }
		bool IsSinglePointOrigin (void) const;

	private:
		static constexpr Metric DUAL_SHOT_SEPARATION = 12.0;	//	Radius of dual shot (pixels)

		struct SConfigDesc
			{
			DiceRange Angle;				//	Offset from fire angle
			int iPosAngle = 0;				//	Origin of shot
			Metric rPosRadius = 0.0;		//	Origin of shot
			};

		ETypes m_iType = ctUnknown;			//	Shot configuration;
		int m_iAimTolerance = 0;			//	Aim tolerance

		TArray<SConfigDesc> m_Custom;
		bool m_bCustomAlternating = false;	//	Fire each shot in turn
	};

class CWeaponClass : public CDeviceClass
	{
	public:
		//	Ammo constants

		static constexpr Metric HP_ARMOR_RATIO = 0.1;						//	Ammo HP per standard armor HP of same level
		static constexpr Metric STD_AMMO_MASS =	10.0;						//	Standard ammo mass (in kg)
		static constexpr Metric DEFAULT_HP_DAMAGE_RATIO = 0.5;				//	Used to compute default HP.
																			//		See: CWeaponFireDesc::CalcDefaultHitPoints

		//	Balance computation

		static constexpr Metric EXPECTED_FRAGMENT_HITS = 0.2;				//  Fraction of fragments that hit (for balance purposes)
		static constexpr Metric EXPECTED_RADIUS_DAMAGE = 0.8;				//  Fraction of radius damage (for balance purposes)
		static constexpr Metric EXPECTED_SHOCKWAVE_HITS = 0.2;				//  Fraction of shockwave that hits (for balance purposes)
		static constexpr Metric EXPECTED_TRACKING_FRAGMENT_HITS = 0.9;		//  Fraction of tracking fragments that hit (for balance purposes)
		static constexpr Metric MAX_EXPECTED_PASSTHROUGH = 4.0;

		//	Fragmentation

		static constexpr Metric DEFAULT_FRAG_THRESHOLD = 4.0;				//	4 light-seconds (~95 pixels)

		//	Interaction

		static constexpr Metric DEFAULT_INTERACTION_MIN_RATIO = 0.5;		//	Ratio at which default interaction is 0.
		static constexpr Metric DEFAULT_INTERACTION_EXP = 0.7;				//	Exponent for computing default interaction
																			//		See: CWeaponFireDesc::CalcDefaultInteraction
		enum ECachedHandlers
			{
			evtOnFireWeapon				= 0,
			evtGetAmmoToConsume			= 1,
			evtGetAmmoCountToDisplay	= 2,

			evtCount					= 3,
			};

        struct SBalance
            {
            Metric rBalance = 0.0;			//  Total balance
            int iLevel = 0;					//  Level for which we balanced

            Metric rStdDamage180 = 0.0;		//  Standard damage (for level and fire rate)
            Metric rDamageHP = 0.0;			//  HP damage per projectile
            Metric rDamageMult = 0.0;		//  Effective number of projectiles
            Metric rDamage180 = 0.0;		//  Damage per 180 ticks
            Metric rDamage = 0.0;			//  Damage balance component
            Metric rDamageType = 0.0;		//  Damage type balance contribution

            Metric rStdAmmoCost = 0.0;		//  Standard ammo cost (for level and fire rate)
            Metric rStdAmmoMass = 0.0;		//  Standard ammo mass (for level and fire rate)
            Metric rAmmo = 0.0;				//  Ammo contribution
            Metric rOmni = 0.0;				//  Omni and swivel component
            Metric rTracking = 0.0;			//  Tracking component

            Metric rRange = 0.0;			//  Range component
            Metric rSpeed = 0.0;			//  Speed component
            Metric rProjectileHP = 0.0;		//  HP and interaction balance component
            Metric rPower = 0.0;			//  Power use component
            Metric rCost = 0.0;				//  Cost component to balance
            Metric rSlots = 0.0;			//  Slot component
            Metric rExternal = 0.0;			//  External component
            Metric rLinkedFire = 0.0;		//  Weapon is linked-fire
            Metric rRecoil = 0.0;			//  Weapon has recoil

            Metric rRadiation = 0.0;		//  Bonus for radiation
            Metric rDeviceDisrupt = 0.0;	//  Bonus for device disrupt
            Metric rDeviceDamage = 0.0;		//  Bonus for device damage
            Metric rDisintegration = 0.0;	//  Bonus for disintegration
            Metric rShatter = 0.0;			//  Bonus for shatter damage
            Metric rShieldPenetrate = 0.0;	//  Bonus for shield penetration
            Metric rShield = 0.0;			//  Bonus for shield damage
            Metric rArmor = 0.0;			//  Bonus for armor damage
            Metric rWMD = 0.0;				//  Bonus for WMD
            Metric rMining = 0.0;			//  Bonus for mining
			Metric rTimeStop = 0.0;			//	Bonus for time stop
            };

        struct SStdStats
	        {
	        int iDamage;			//	Average damage at this level
	        int iPower;				//	Power (in tenths of MWs)
            Metric rCost;           //  Weapon cost (credits)
            Metric rAmmoCost;       //  Ammo cost (credits)

	        int iOverTierAdj;		//	Balance points to add to adjust for damage type above level
	        int iUnderTierAdj;		//	Balance points to add to adjust for damage type below level
	        };

		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, CDeviceClass **retpWeapon);
		virtual ~CWeaponClass (void);

		int CalcBalance (CItemCtx &ItemCtx, SBalance &retBalance) const;
		inline bool FindEventHandlerWeaponClass (ECachedHandlers iEvent, SEventHandlerDesc *retEvent = NULL) const
			{
			if (!m_CachedEvents[iEvent].pCode)
				return false;

			if (retEvent) *retEvent = m_CachedEvents[iEvent];
			return true;
			}

        CItemType *GetAmmoItem (int iIndex) const;
        int GetAmmoItemCount (void) const;
		inline int GetIdlePowerUse (void) const { return m_iIdlePowerUse; }
        CWeaponFireDesc *GetWeaponFireDesc (CItemCtx &ItemCtx, const CItem &Ammo = CItem()) const;

        static const SStdStats &GetStdStats (int iLevel);
		static int GetStdDamage (int iLevel);
		static Metric GetStdDamage180 (int iLevel);
		static bool IsStdDamageType (DamageTypes iDamageType, int iLevel);

		//	CDeviceClass virtuals

		virtual bool Activate (CInstalledDevice *pDevice, 
							   CSpaceObject *pSource, 
							   CSpaceObject *pTarget,
							   bool *retbSourceDestroyed,
							   bool *retbConsumedItems = NULL) override;
		virtual CWeaponClass *AsWeaponClass (void) override { return this; }
		virtual int CalcFireSolution (CInstalledDevice *pDevice, CSpaceObject *pSource, CSpaceObject *pTarget) override;
		virtual int CalcPowerUsed (SUpdateCtx &Ctx, CInstalledDevice *pDevice, CSpaceObject *pSource) override;
        virtual ICCItem *FindAmmoItemProperty (CItemCtx &Ctx, const CItem &Ammo, const CString &sProperty) override;
		virtual int GetActivateDelay (CItemCtx &ItemCtx) const override;
		virtual int GetAmmoVariant (const CItemType *pItem) const override;
		virtual ItemCategories GetImplCategory (void) const override;
		virtual int GetCounter (CInstalledDevice *pDevice, CSpaceObject *pSource, CounterTypes *retiType = NULL, int *retiLevel = NULL) override;
		virtual int GetDefaultFireAngle (CInstalledDevice *pDevice, CSpaceObject *pSource) const override;
		virtual void Update (CInstalledDevice *pDevice, CSpaceObject *pSource, SDeviceUpdateCtx &Ctx) override;

		virtual bool FindAmmoDataField (const CItem &Ammo, const CString &sField, CString *retsValue) const override;
		virtual bool FindDataField (const CString &sField, CString *retsValue) override;
		virtual ICCItem *FindItemProperty (CItemCtx &Ctx, const CString &sName) override;
		virtual const DamageDesc *GetDamageDesc (CItemCtx &Ctx) override;
		virtual DamageTypes GetDamageType (CItemCtx &Ctx, const CItem &Ammo = CItem()) const override;
		virtual DWORD GetLinkedFireOptions (void) const override { return m_dwLinkedFireOptions; }
		virtual Metric GetMaxEffectiveRange (CSpaceObject *pSource, const CInstalledDevice *pDevice, CSpaceObject *pTarget) const override;
		virtual Metric GetMaxRange (CItemCtx &ItemCtx) override;
		virtual int GetPowerRating (CItemCtx &Ctx, int *retiIdlePowerUse = NULL) const override;
		virtual bool GetReferenceDamageType (CItemCtx &Ctx, const CItem &Ammo, DamageTypes *retiDamage, CString *retsReference) const override;
		virtual DeviceRotationTypes GetRotationType (const CDeviceItem &DeviceItem, int *retiMinArc = NULL, int *retiMaxArc = NULL) const override;
		virtual void GetSelectedVariantInfo (const CSpaceObject *pSource, 
											 const CInstalledDevice *pDevice,
											 CString *retsLabel,
											 int *retiAmmoLeft,
											 CItemType **retpType = NULL,
											 bool bUseCustomAmmoCountHandler = false) override;
		virtual Metric GetShotSpeed (CItemCtx &Ctx) const override;
		virtual int GetValidVariantCount (CSpaceObject *pSource, CInstalledDevice *pDevice) override;
		virtual int GetWeaponEffectiveness (const CDeviceItem &DeviceItem, CSpaceObject *pTarget) const override;
		virtual const CWeaponFireDesc *GetWeaponFireDesc (const CDeviceItem &DeviceItem, const CItem &Ammo = CItem()) const override;
		virtual bool IsAmmoWeapon (void) override;
		virtual bool IsAreaWeapon (const CDeviceItem &DeviceItem) const override;
		virtual bool IsFirstVariantSelected(CSpaceObject *pSource, CInstalledDevice *pDevice) override;
		virtual bool IsTrackingWeapon (const CDeviceItem &DeviceItem) const override;
		virtual bool IsVariantSelected (CSpaceObject *pSource, CInstalledDevice *pDevice) override;
		virtual bool IsLastVariantSelected (CSpaceObject *pSource, CInstalledDevice *pDevice) override;
		virtual bool IsWeaponAligned (CSpaceObject *pShip, const CInstalledDevice *pDevice, CSpaceObject *pTarget, int *retiAimAngle = NULL, int *retiFireAngle = NULL) const override;
		virtual bool NeedsAutoTarget (const CDeviceItem &DeviceItem, int *retiMinFireArc = NULL, int *retiMaxFireArc = NULL) const override;
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) override;
		virtual bool RequiresItems (void) const override;
		virtual bool SelectFirstVariant (CSpaceObject *pSource, CInstalledDevice *pDevice) override;
		virtual bool SelectNextVariant (CSpaceObject *pSource, CInstalledDevice *pDevice, int iDir = 1) override;
		virtual bool SetCounter (CInstalledDevice *pDevice, CSpaceObject *pSource, CounterTypes iCounter, int iLevel) override;
		virtual bool ValidateSelectedVariant (CSpaceObject *pSource, CInstalledDevice *pDevice) override;

	protected:
		virtual void OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override;
		virtual void OnAccumulateAttributes (const CDeviceItem &DeviceItem, const CItem &Ammo, TArray<SDisplayAttribute> *retList) const override;
		virtual CEffectCreator *OnFindEffectCreator (const CString &sUNID) override;
		virtual CString OnGetReference (CItemCtx &Ctx, const CItem &Ammo = CItem(), DWORD dwFlags = 0) override;
		virtual void OnMarkImages (void) override;
		virtual ALERROR OnPrepareBind (SDesignLoadCtx &Ctx) override;

	private:
		static constexpr int CONTINUOUS_START = 0xff;
		static constexpr int CONTINUOUS_DATA_LIMIT = 0xfe;

		enum EVariantTypes
			{
			varSingle,				//	Single CWeaponFireDesc (may or may not use ammo)
			varLauncher,			//	We're a launcher; each variant is a missile
			varSingleLevelScaling,	//	Single CWeaponFireDesc, scales to levels
			varLevelScaling,		//	Explicit definitions for each level
			varCharges,				//	Current charge determines definition
			varCounter,				//	Current counter value determines definition
			};

		struct SShotDesc
			{
			CWeaponFireDesc *pDesc;	//	Pointer to descriptor (may be external)
			CItemTypeRef pAmmoType;	//	ItemType for ammo, only if referring to a missile
			bool bOwned;			//	TRUE if we own the descriptor (otherwise, it
                                    //      may be owned by a missile).
			};

		enum EOnFireWeaponResults
			{
			resDefault,
			resNoShot,
			resShotFired,
			};

		struct SShotFireResult
			{
			bool bNoShotFired = false;			//	If TRUE, no shot was fired

			bool bNoFireEffect = false;			//	If TRUE, do not create fire effect
			bool bNoSoundEffect = false;		//	If TRUE, do not create sound effect
			bool bNoRecoil = false;				//	If TRUE, do not create recoil
			};

		CWeaponClass (void);

		int CalcActivateDelay (CItemCtx &ItemCtx) const;
		int CalcConfiguration (CItemCtx &ItemCtx, CWeaponFireDesc *pShot, int iFireAngle, TArray<SShotOrigin> &retConfig, bool bSetAlternating = false) const;
		Metric CalcConfigurationMultiplier (CWeaponFireDesc *pShot = NULL, bool bIncludeFragments = true) const;
		Metric CalcDamage (CWeaponFireDesc *pShot, const CItemEnhancementStack *pEnhancements = NULL, DWORD dwDamageFlags = 0) const;
		Metric CalcDamagePerShot (CWeaponFireDesc *pShot, const CItemEnhancementStack *pEnhancements = NULL, DWORD dwDamageFlags = 0) const;
		int CalcFireAngle (CItemCtx &ItemCtx, Metric rSpeed, CSpaceObject *pTarget, bool *retbOutOfArc);
        int CalcLevel (CWeaponFireDesc *pShot) const;
		bool ConsumeAmmo (CItemCtx &ItemCtx, CWeaponFireDesc *pShot, int iRepeatingCount, bool *retbConsumed);
		bool ConsumeCapacitor (CItemCtx &ItemCtx, CWeaponFireDesc *pShot);
		void FailureExplosion (CItemCtx &ItemCtx, CWeaponFireDesc *pShot, bool *retbSourceDestroyed);
		bool FireGetAmmoCountToDisplay (const CDeviceItem &DeviceItem, const CWeaponFireDesc &Shot, int *retiAmmoCount = NULL) const;
		int FireGetAmmoToConsume(CItemCtx &ItemCtx,
							  CWeaponFireDesc *pShot,
							  int iRepeatingCount);
		bool FireOnFireWeapon (CItemCtx &ItemCtx, 
							   CWeaponFireDesc *pShot,
							   const CVector &vSource,
							   CSpaceObject *pTarget,
							   int iFireAngle,
							   int iRepeatingCount,
							   SShotFireResult &retResult);
		bool FireWeapon (CInstalledDevice *pDevice, 
						 CWeaponFireDesc *pShot, 
						 CSpaceObject *pSource, 
						 CSpaceObject *pTarget,
						 int iRepeatingCount,
						 bool *retbSourceDestroyed,
						 bool *retbConsumedItems);
		void FireWeaponShot (CSpaceObject *pSource, 
							 CInstalledDevice *pDevice, 
							 CWeaponFireDesc *pShot, 
							 const CVector &vShotPos, 
							 int iShotDir, 
							 Metric rShotSpeed, 
							 CSpaceObject *pTarget,
							 int iRepeatingCount,
							 int iShotNumber);
		int GetContinuous (const CWeaponFireDesc &Shot) const;
		int GetContinuousFireDelay (const CWeaponFireDesc &Shot) const;
		int GetFireDelay (CWeaponFireDesc *pShot) const;
		const CWeaponFireDesc *GetReferenceShotData (const CWeaponFireDesc *pShot, int *retiFragments = NULL) const;
		int GetSelectVariantCount (void) const;
		bool HasAmmoLeft (CItemCtx &ItemCtx, CWeaponFireDesc *pShot) const;
		ALERROR InitVariantsFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType);
		inline bool IsCapacitorEnabled (void) { return (m_Counter == cntCapacitor); }
		inline bool IsCounterEnabled (void) { return (m_Counter != cntNone); }
		inline bool IsLauncher (void) const { return (m_iVariantType == varLauncher); }
		inline bool IsLauncherWithAmmo (void) const { return (IsLauncher() && m_ShotData[0].pDesc->GetAmmoType() != NULL); }
		bool IsSinglePointOrigin (void) const { return m_Configuration.IsSinglePointOrigin(); }
		inline bool IsTemperatureEnabled (void) { return (m_Counter == cntTemperature); }
		bool IsTracking (const CDeviceItem &DeviceItem, const CWeaponFireDesc *pShot) const;
		bool UpdateShipCounter(CItemCtx &ItemCtx, CWeaponFireDesc *pShot);
		bool UpdateTemperature (CItemCtx &ItemCtx, CWeaponFireDesc *pShot, CFailureDesc::EFailureTypes *retiFailureMode, bool *retbSourceDestroyed);
		inline bool UsesAmmo (void) const { return (m_ShotData.GetCount() > 0 && m_ShotData[0].pDesc->GetAmmoType() != NULL); }
		bool VariantIsValid (CSpaceObject *pSource, CInstalledDevice *pDevice, CWeaponFireDesc &ShotData);

		int GetAlternatingPos (const CInstalledDevice *pDevice) const;
		DWORD GetContinuousFire (const CInstalledDevice *pDevice) const;
		int GetCurrentVariant (const CInstalledDevice *pDevice) const;
		void SetAlternatingPos (CInstalledDevice *pDevice, int iAlternatingPos) const;
		void SetContinuousFire (CInstalledDevice *pDevice, DWORD dwContinuous) const;
		void SetCurrentVariant (CInstalledDevice *pDevice, int iVariant) const;

		int m_iFireRate;						//	Ticks between shots
		int m_iPowerUse;						//	Power use to recharge capacitors (1/10 megawatt)
		int m_iIdlePowerUse;					//	Power use when capacitors fully charged
		int m_iRecoil;							//	0-7 (as per momentum damage)
		int m_iFailureChance;					//	Chance of failure

		bool m_bBurstTracksTargets;				//  If the weapon is continuous, whether or not to track the target during the entire burst
		bool m_bContinuousConsumePerShot;		//	If a continuous weapon, consume ammunition for every shot in burst
		int m_iCounterPerShot;					//	How much to increment the ship's counter by per shot
		bool m_bOmnidirectional;				//	Omnidirectional
		bool m_bMIRV;							//	Each shot seeks an independent target
		bool m_bCanFireWhenBlind;				//	Allow firing this weapon if blinded
		bool m_bReportAmmo;						//	Report count of ammo shot even if not a launcher
		int m_iMinFireArc;						//	Min angle of fire arc (degrees)
		int m_iMaxFireArc;						//	Max angle of fire arc (degrees)
		DWORD m_dwLinkedFireOptions;			//	Linked fire options

		EVariantTypes m_iVariantType;			//	Type of variant
		TArray<SShotDesc> m_ShotData;			//	Desc for each shot variation

		CConfigurationDesc m_Configuration;		//	Shot configuration

		int m_iContinuous = 0;					//	Repeat fire
		int m_iContinuousFireDelay = 0;			//	Delay between shots

		bool m_bCharges;						//	TRUE if weapon has charges instead of ammo

		CounterTypes m_Counter;					//	Counter type
		int m_iCounterUpdateRate;				//	Ticks to update counter
		int m_iCounterUpdate;					//	Inc/dec value per update
		int m_iCounterActivate;					//	Inc/dec value per shot

		bool m_bTargetStationsOnly;				//	Do not target ships

		CFailureDesc m_DamageFailure;			//	Failure mode when damaged/disrupted
		CFailureDesc m_OverheatFailure;

		SEventHandlerDesc m_CachedEvents[evtCount];	//	Cached events

		static CFailureDesc g_DefaultFailure;
	};

