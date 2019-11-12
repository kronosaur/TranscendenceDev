//	TSEArmor.h
//
//	Defines classes and interfaces for armor segments
//	Copyright (c) 2016 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CInstalledArmor;
struct SShipPerformanceCtx;

//  Armor Class ----------------------------------------------------------------
//
//  CArmorClass defines the properties of a particular type of armor. This is
//  included in a CItemType.

class CArmorClass
	{
	public:
		static constexpr int TICKS_PER_UPDATE = 10;

		enum ECachedHandlers
			{
			evtGetMaxHP					= 0,
			evtOnArmorConsumePower		= 1,
			evtOnArmorDamage			= 2,

			evtCount					= 3,
			};

		struct SMassClassDesc
			{
			char *pszID;
			char *pszName;
			int iMaxMassKg;
			};

        struct SStdStats
	        {
	        int iHP;								//	HP for std armor at this level
	        int iCost;								//	Std cost at this level
	        int iRepairCost;						//	Cost to repair 1 hp
	        int iInstallCost;						//	Cost to install
	        int iMass;								//	Standard mass
	        };

        ~CArmorClass (void);

		void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed);
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, CArmorClass **retpArmor);
		bool FindDataField (const CString &sField, CString *retsValue);
		bool FindEventHandlerArmorClass (ECachedHandlers iEvent, SEventHandlerDesc *retEvent = NULL) const 
			{
			if (m_CachedEvents[iEvent].pCode == NULL)
				return false;

			if (retEvent) *retEvent = m_CachedEvents[iEvent];
			return true;
			}

        ALERROR FinishBindDesign (SDesignLoadCtx &Ctx) { return NOERROR; }
		int GetCompleteBonus (void) { return m_iArmorCompleteBonus; }
		int GetDamageAdjForWeaponLevel (int iLevel);
		CItemType *GetItemType (void) const { return m_pItemType; }
		int GetMaxHPBonus (void) const { return m_iMaxHPBonus; }
		inline CString GetName (void) const;
		CString GetShortName (void);
		int GetStealth (void) const { return m_iStealth; }
		DWORD GetUNID (void);
        bool IsScalable (void) const { return (m_pScalable != NULL); }
		ALERROR OnBindDesign (SDesignLoadCtx &Ctx);

		EDamageResults AbsorbDamage (CItemCtx &ItemCtx, SDamageCtx &Ctx);
		void AccumulateAttributes (const CArmorItem &ArmorItem, TArray<SDisplayAttribute> *retList) const;
		bool AccumulateEnhancements (CItemCtx &ItemCtx, CInstalledDevice *pTarget, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements);
        bool AccumulatePerformance (CItemCtx &ItemCtx, SShipPerformanceCtx &Ctx) const;
		void AccumulatePowerUsed (CItemCtx &ItemCtx, SUpdateCtx &Ctx, int &iPowerUsed, int &iPowerGenerated) const;
		void CalcAdjustedDamage (CItemCtx &ItemCtx, SDamageCtx &Ctx);
		int CalcAverageRelativeDamageAdj (CItemCtx &ItemCtx);
		void CalcDamageEffects (CItemCtx &ItemCtx, SDamageCtx &Ctx) const;
		const CString &GetMassClass (const CItemCtx &ItemCtx) const;
		int GetPowerOutput (CItemCtx &ItemCtx) const;
		int GetPowerRating (CItemCtx &ItemCtx, int *retiIdlePower = NULL) const;
		CString GetReference (CItemCtx &Ctx);
		bool GetReferenceSpeedBonus (CItemCtx &Ctx, int *retiSpeedBonus) const;
        Metric GetScaledCostAdj (CItemCtx &ItemCtx) const;
		bool IsImmune (CItemCtx &ItemCtx, SpecialDamageTypes iSpecialDamage) const;
		bool IsReflective (const CArmorItem &ArmorItem, const DamageDesc &Damage, int *retiChance = NULL) const;
		bool IsShieldInterfering (CItemCtx &ItemCtx);
		ESetPropertyResults SetItemProperty (CItemCtx &Ctx, CItem &Item, const CString &sProperty, const ICCItem &Value, CString *retsError = NULL);
		void Update (CItemCtx &ItemCtx, SUpdateCtx &UpdateCtx, int iTick, bool *retbModified);
		bool UpdateRegen (CItemCtx &ItemCtx, SUpdateCtx &UpdateCtx, const CRegenDesc &Regen, ERegenTypes iRegenType, int iTick);

		static int CalcIntegrity (int iHP, int iMaxHP);
		static int CalcMaxHPChange (int iCurHP, int iCurMaxHP, int iNewMaxHP);
		static int GetStdCost (int iLevel);
		static int GetStdDamageAdj (CUniverse &Universe, int iLevel, DamageTypes iDamage);
		static int GetStdEffectiveHP (CUniverse &Universe, int iLevel);
		static int GetStdHP (int iLevel);
		static int GetStdMass (int iLevel);
        static const SStdStats &GetStdStats (int iLevel);

	private:

        struct SScalableStats
            {
            int iLevel;

            int iHitPoints;
            CDamageAdjDesc DamageAdj;
            int iBlindingDamageAdj;
            int iEMPDamageAdj;
            int iDeviceDamageAdj;

			ERegenTypes iRegenType;
            CRegenDesc Regen;

            CRegenDesc Decay;
            CRegenDesc Distribute;
            
            CCurrencyAndValue RepairCost;
            CCurrencyAndValue InstallCost;

            DWORD fRadiationImmune:1;
            DWORD fDisintegrationImmune:1;
            DWORD fShatterImmune:1;
            };

		CArmorClass (void);

        ALERROR BindScaledParams (SDesignLoadCtx &Ctx);
		int CalcArmorDamageAdj (const CArmorItem &ArmorItem, const DamageDesc &Damage) const;
		int CalcBalance (const CArmorItem &ArmorItem, CArmorItem::SBalance &retBalance) const;
		Metric CalcBalanceDamageAdj (const CArmorItem &ArmorItem, const SScalableStats &Stats) const;
		Metric CalcBalanceDamageEffectAdj (const CArmorItem &ArmorItem, const SScalableStats &Stats) const;
		Metric CalcBalanceMass (const CArmorItem &ArmorItem, const SScalableStats &Stats, Metric *retrStdMass) const;
		Metric CalcBalancePower (const CArmorItem &ArmorItem, const SScalableStats &Stats) const;
		Metric CalcBalanceRegen (const CArmorItem &ArmorItem, const SScalableStats &Stats) const;
		Metric CalcBalanceRepair (const CArmorItem &ArmorItem, const SScalableStats &Stats) const;
		Metric CalcBalanceSpecial (const CArmorItem &ArmorItem, const SScalableStats &Stats) const;
		ICCItemPtr FindItemProperty (const CArmorItem &ArmorItem, const CString &sProperty) const;
		void GenerateScaledStats (void);
        inline int GetDamageAdj (const CArmorItem &ArmorItem, DamageTypes iDamage) const;
		int GetDamageAdj (const CArmorItem &ArmorItem, const DamageDesc &Damage) const;
		int GetDamageEffectiveness (const CArmorItem &ArmorItem, CSpaceObject *pAttacker, CInstalledDevice *pWeapon) const;
		inline int GetInstallCost (const CArmorItem &ArmorItem) const;
		bool GetReferenceDamageAdj (const CArmorItem &ArmorItem, int *retiHP, int *retArray) const;
		int FireGetMaxHP (const CArmorItem &ArmorItem, int iMaxHP) const;
		int GetMaxHP (const CArmorItem &ArmorItem, bool bForceComplete = false) const;
		CurrencyValue GetRepairCost (const CArmorItem &ArmorItem, int iHPToRepair = 1) const;
		int GetRepairLevel (const CArmorItem &ArmorItem) const;
        const SScalableStats &GetScaledStats (const CArmorItem &ArmorItem) const;
		CUniverse &GetUniverse (void) const;

		Metric CalcRegen180 (CItemCtx &ItemCtx) const;
		void FireOnArmorDamage (CItemCtx &ItemCtx, SDamageCtx &Ctx);
		int UpdateCustom (CInstalledArmor *pArmor, CSpaceObject *pSource, SEventHandlerDesc Event) const;
		bool UpdateDecay (CItemCtx &ItemCtx, const SScalableStats &Stats, int iTick);
		bool UpdateDistribute (CItemCtx &ItemCtx, const SScalableStats &Stats, int iTick);

        SScalableStats m_Stats;                 //  Base stats capable of being scaled

		int m_iRepairTech;						//	Tech required to repair
		int m_iArmorCompleteBonus;				//	Extra HP if armor is complete
		int m_iHPBonusPerCharge;				//	Extra HP for each charge
		int m_iStealth;							//	Stealth level
		int m_iPowerUse;						//	Power consumed (1/10th MWs)
		int m_iIdlePowerUse;					//	Power consumed when not regenerating
		int m_iPowerGen;						//	Power generation, usually solar (1/10th MWs)
		int m_iMaxHPBonus;						//	Max HP bonus allowed for this armor
		int m_iMaxSpeedInc;						//	Add/subtract to ship's max speed
		int m_iMaxSpeedLimit;					//	Do not increase speed above this value
		CString m_sEnhancementType;				//	Type of enhancements
		int m_iDeviceBonus;						//	Bonus to devices
		CItemCriteria m_DeviceCriteria;			//	Only enhances devices that match criteria
		int m_iDamageAdjLevel;					//	Level to use for intrinsic damage adj
		DamageTypeSet m_Reflective;				//	Types of damage reflected
		CString m_sMassClass;					//	Computed mass class (computed in Bind)
		int m_iBalanceAdj;						//	Manual adjustment to balance calculation

		DWORD m_fPhotoRecharge:1;				//	TRUE if refuels when near a star
		DWORD m_fShieldInterference:1;			//	TRUE if armor interferes with shields
		DWORD m_fChargeDecay:1;					//	If TRUE, we decay while we have charges left
		DWORD m_fSpare4:1;
		DWORD m_fSpare5:1;
		DWORD m_fSpare6:1;
		DWORD m_fSpare7:1;
		DWORD m_fSpare8:1;

		DWORD m_dwSpare:24;

		CItemType *m_pItemType;					//	Item for this armor

        int m_iScaledLevels;                    //  Number of levels above first
        SScalableStats *m_pScalable;            //  Params for higher level versions of this armor

		SEventHandlerDesc m_CachedEvents[evtCount];

	friend class CArmorItem;
	};

//  Ship Armor Segments --------------------------------------------------------
//
//  CShipArmorDesc is a descriptor, generally held in CShipClass, that defines
//  the number and distribution of armor segmens for a ship.

class CShipArmorSegmentDesc
    {
    public:
        bool AngleInSegment (int iAngle) const;
        ALERROR Bind (SDesignLoadCtx &Ctx);
        bool CreateArmorItem (CItem *retItem, CString *retsError = NULL) const;
        inline CArmorClass *GetArmorClass (void) const { return m_pArmor; }
		CItem GetArmorItem (void) const;
        inline int GetCenterAngle (void) const { return AngleMod(m_iStartAt + m_iSpan / 2); }
        DWORD GetCriticalArea (void) const { return m_dwAreaSet; }
        int GetLevel (void) const;
        inline int GetSpan (void) const { return m_iSpan; }
        inline int GetStartAngle (void) const { return m_iStartAt; }
        ALERROR Init (int iStartAt, int iSpan, DWORD dwArmorUNID, int iLevel, const CRandomEnhancementGenerator &Enhancement);
        ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);

    private:
        static DWORD ParseNonCritical (const CString &sList);

		int m_iStartAt;						//	Start of section in degrees
		int m_iSpan;						//	Size of section in degrees
		CArmorClassRef m_pArmor;			//	Type of armor for hull
        int m_iLevel;                       //  For scalable armor
		CRandomEnhancementGenerator m_Enhanced;//	Mods
		DWORD m_dwAreaSet;					//	Areas that this section protects
    };

class CShipArmorDesc
    {
    public:
        void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) const;
        ALERROR Bind (SDesignLoadCtx &Ctx);
        Metric CalcMass (void) const;
        inline int GetCount (void) const { return m_Segments.GetCount(); }
        inline const CShipArmorSegmentDesc &GetSegment (int iIndex) const { ASSERT(iIndex >= 0 && iIndex < m_Segments.GetCount()); return m_Segments[iIndex]; }
        int GetSegmentAtAngle (int iAngle) const;
        CString GetSegmentName (int iIndex) const;
        ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);

    private:
        TArray<CShipArmorSegmentDesc> m_Segments;
    };

//  Installed Armor ------------------------------------------------------------
//
//  CArmorSystem is a class that holds all installed armor segments on a ship.
//  CInstalledArmor represents a single installed armor segment, including its
//  hit points and other state information.

class CInstalledArmor
	{
	public:
		const CArmorItem AsArmorItem (void) const { return m_pItem->AsArmorItemOrThrow(); }
		CArmorItem AsArmorItem (void) { return m_pItem->AsArmorItemOrThrow(); }
		inline EDamageResults AbsorbDamage (CSpaceObject *pSource, SDamageCtx &Ctx);
		bool AccumulateEnhancements (CSpaceObject *pSource, CInstalledDevice *pTarget, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements);
        bool AccumulatePerformance (CItemCtx &ItemCtx, SShipPerformanceCtx &Ctx) const;
		bool ConsumedPower (void) const { return (m_fConsumePower ? true : false); }
		void FinishInstall (CSpaceObject &Source);
		int GetCharges (CSpaceObject *pSource) { return (m_pItem ? m_pItem->GetCharges() : 0); }
		CArmorClass *GetClass (void) const { return m_pArmorClass; }
		inline int GetDamageEffectiveness (CSpaceObject *pAttacker, CInstalledDevice *pWeapon);
		TSharedPtr<CItemEnhancementStack> GetEnhancementStack (void) const { return m_pEnhancements; }
		int GetHitPoints (void) const { return m_iHitPoints; }
		int GetHitPointsPercent (CSpaceObject *pSource);
		CItem *GetItem (void) const { return m_pItem; }
        inline int GetLevel (void) const;
		inline int GetMaxHP (CSpaceObject *pSource) const;
		int GetSect (void) const { return m_iSect; }
		CSpaceObject *GetSource (void) const { return m_pSource; }
		int IncCharges (CSpaceObject *pSource, int iChange);
		int IncHitPoints (int iChange) { m_iHitPoints = Max(0, m_iHitPoints + iChange); return m_iHitPoints; }
		void Install (CSpaceObject &Source, CItemListManipulator &ItemList, int iSect, bool bInCreate = false);
		bool IsComplete (void) const { return (m_fComplete ? true : false); }
		bool IsPrime (void) const { return (m_fPrimeSegment ? true : false); }
		void SetComplete (CSpaceObject *pSource, bool bComplete = true);
		void SetConsumePower (bool bValue = true) { m_fConsumePower = bValue; }
		void SetEnhancements (CSpaceObject *pSource, const TSharedPtr<CItemEnhancementStack> &pStack);
		void SetPrime (CSpaceObject *pSource, bool bPrime = true) { m_fPrimeSegment = bPrime; }
		void SetHitPoints (int iHP) { m_iHitPoints = iHP; }
		void SetSect (int iSect) { m_iSect = iSect; }
		void ReadFromStream (CSpaceObject &Source, int iSect, SLoadCtx &Ctx);
		void WriteToStream (IWriteStream *pStream);

	private:
		CSpaceObject *m_pSource = NULL;				//	Installed on this object
		CItem *m_pItem = NULL;						//	Item
		CArmorClass *m_pArmorClass = NULL;			//	Armor class used
		int m_iHitPoints = 0;						//	Hit points left
		TSharedPtr<CItemEnhancementStack> m_pEnhancements;		//	List of enhancements (may be NULL)

		int m_iSect:16;								//	Armor section
		int m_iSpare:16;

		DWORD m_fComplete:1;						//	All armor segments the same
		DWORD m_fPrimeSegment:1;					//	Prime segment for all segments of the same type.
		DWORD m_fConsumePower:1;					//	If TRUE, we should consume power this tick
		DWORD m_fSpare:29;
	};

