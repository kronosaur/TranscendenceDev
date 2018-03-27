//	TSEDevices.h
//
//	Defines classes and interfaces for devices.
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CCargoDesc;
struct SShipPerformanceCtx;

enum DeviceNames
	{
	devNone = -1,

	devFirstName = 0,

	devPrimaryWeapon = 0,
	devMissileWeapon = 1,

	devShields = 8,
	devDrive = 9,
	devCargo = 10,
	devReactor = 11,

	devNamesCount = 12
	};

class CFailureDesc
	{
	public:
		enum EProfile
			{
			profileNone,

			profileWeaponFailure,
			profileWeaponOverheat,
			};

		enum EFailureTypes
			{
			failError =				-1,
			failNone =				0,

			failExplosion =			1,
			failHeatDamage =		2,
			failJammed =			3,
			failMisfire =			4,
			failSafeMode =			5,		//	Device disabled

			failCustom =			6,		//	Fire OnFailure event
			};

		CFailureDesc (EProfile iProfile = profileNone);

		EFailureTypes Failure (CSpaceObject *pSource, CInstalledDevice *pDevice) const;
		ALERROR InitFromDefaults (EProfile iDefault);
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, EProfile iDefault = profileNone);
		inline bool IsEmpty (void) const { return (m_FailTable.GetCount() == 0); }

		static EFailureTypes ParseFailureType (const CString &sValue);

	private:
		TProbabilityTable<EFailureTypes> m_FailTable;
	};

class CItemLevelCriteria
	{
	public:
		enum ETypes
			{
			checkNone,							//	Matches everything

			checkLessEqualToLevel,				//	Target item level is <= to our level
			checkLessThanLevel,					//	Target item level is < than our level
			checkGreaterEqualToLevel,			//	Target item level is >= to our level
			checkGreaterThanLevel,				//	Target item level is > than our level
			checkLessEqualToRepairLevel,		//	Target item repair level is <= to our level
			checkLessThanRepairLevel,			//	Target item repair level is < to our level
			checkGreaterEqualToRepairLevel,		//	Target item repair level is >= to our level
			checkGreaterThanRepairLevel,		//	Target item repair level is > than our level
			};

		ALERROR InitFromXML (SDesignLoadCtx &Ctx, const CString &sValue);
		inline bool IsEmpty (void) const { return m_iType == checkNone; }
		bool MatchesCriteria (int iLevel, const CItem &Item) const;

	private:
		ETypes m_iType = checkNone;
	};

class CEnhancementDesc
	{
	public:
		bool Accumulate (CItemCtx &Ctx, const CItem &Target, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements) const;
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		inline int GetCount (void) const { return m_Enhancements.GetCount(); }
		inline const CItemEnhancement &GetEnhancement (int iIndex) const { return m_Enhancements[iIndex].Enhancement; }
		void SetCriteria (int iEntry, const CItemCriteria &Criteria);
		void SetLevelCheck (int iEntry, const CItemLevelCriteria &LevelCheck);
		void SetType (int iEntry, const CString &sType);

	private:
		struct SEnhancerDesc
			{
			CString sType;						//	Type of enhancement
			CItemCriteria Criteria;				//	Items that we enhance
			CItemLevelCriteria LevelCheck;		//	Criteria for item level
			CItemEnhancement Enhancement;		//	Enhancement confered
			};

		ALERROR InitFromEnhanceXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, SEnhancerDesc &Enhance);

		TArray<SEnhancerDesc> m_Enhancements;	//	Enhancements confered on other items
	};

class CDeviceClass
	{
	public:
		enum CounterTypes
			{
			cntNone,							//	No counter
			cntTemperature,						//	Current device temperature (0-100)
			cntRecharge,						//	Current recharge level (0-100)
			cntCapacitor,						//	Current capacitor level (0-100)
			};

		enum DeviceNotificationTypes
			{
			statusDisruptionRepaired,
			statusUsedLastAmmo,

			failDamagedByDisruption,
			failDeviceHitByDamage,
			failDeviceHitByDisruption,
			failDeviceOverheat,
			failDeviceDisabledByOverheat,
			failShieldFailure,
			failWeaponExplosion,
			failWeaponJammed,
			failWeaponMisfire,
			};

		enum LinkedFireOptions
			{
			lkfAlways =				0x0000001,	//	Linked to fire button
			lkfTargetInRange =		0x0000002,	//	Fire only if the target is in range
			lkfEnemyInRange =		0x0000004,	//	Fire only an enemy is in range
			};

		enum ECachedHandlers
			{
			evtGetOverlayType			= 0,

			evtCount					= 1,
			};

		struct SDeviceUpdateCtx
			{
			SDeviceUpdateCtx (int iTickArg = 0) :
					iTick(iTickArg)
				{
				ResetOutputs();
				}

			inline void ResetOutputs (void)
				{
				bSourceDestroyed = false;
				bConsumedItems = false;
				bDisrupted = false;
				bRepaired = false;
				bSetDisabled = false;
				}
					
			int iTick;

			//	Outputs

			bool bSourceDestroyed;
			bool bConsumedItems;
			bool bDisrupted;
			bool bRepaired;
			bool bSetDisabled;
			};

		CDeviceClass (void) : m_pItemType(NULL) { }
		virtual ~CDeviceClass (void) { }

		void AccumulateAttributes (CItemCtx &ItemCtx, const CItem &Ammo, TArray<SDisplayAttribute> *retList);
		bool AccumulateEnhancements (CItemCtx &Device, CInstalledArmor *pTarget, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements);
		bool AccumulateEnhancements (CItemCtx &Device, CInstalledDevice *pTarget, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements);
        bool AccumulatePerformance (CItemCtx &ItemCtx, SShipPerformanceCtx &Ctx) const;
		void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed);
		ALERROR Bind (SDesignLoadCtx &Ctx);
        inline ALERROR FinishBind (SDesignLoadCtx &Ctx) { return OnFinishBind(Ctx); }
		inline CEffectCreator *FindEffectCreator (const CString &sUNID) { return OnFindEffectCreator(sUNID); }
		inline bool FindEventHandlerDeviceClass (ECachedHandlers iEvent, SEventHandlerDesc *retEvent = NULL) const { if (retEvent) *retEvent = m_CachedEvents[iEvent]; return (m_CachedEvents[iEvent].pCode != NULL); }
		COverlayType *FireGetOverlayType(CItemCtx &Ctx) const;
        bool GetAmmoItemPropertyBool (CItemCtx &Ctx, const CItem &Ammo, const CString &sProperty);
        Metric GetAmmoItemPropertyDouble (CItemCtx &Ctx, const CItem &Ammo, const CString &sProperty);
		inline ItemCategories GetCategory (void) const { return (m_iSlotCategory == itemcatNone ? GetImplCategory() : m_iSlotCategory); }
		inline CString GetDataField (const CString &sField) { CString sValue; FindDataField(sField, &sValue); return sValue; }
		inline int GetDataFieldInteger (const CString &sField) { CString sValue; if (FindDataField(sField, &sValue)) return strToInt(sValue, 0, NULL); else return 0; }
		int GetInstallCost (CItemCtx &ItemCtx);
		inline CItemType *GetItemType (void) const { return m_pItemType; }
		inline int GetLevel (void) const;
		inline int GetMaxHPBonus (void) const { return m_iMaxHPBonus; }
		inline CString GetName (void);
		inline COverlayType *GetOverlayType(void) const { return m_pOverlayType; }
		CString GetReference (CItemCtx &Ctx, const CItem &Ammo = CItem(), DWORD dwFlags = 0);
		CString GetReferencePower (CItemCtx &Ctx);
        inline Metric GetScaledCostAdj (CItemCtx &Ctx) const { return OnGetScaledCostAdj(Ctx); }
		inline int GetSlotsRequired (void) const { return m_iSlots; }
		inline DWORD GetUNID (void);
		inline void MarkImages (void) { DEBUG_TRY OnMarkImages(); DEBUG_CATCH }
		bool OnDestroyCheck (CItemCtx &ItemCtx, DestructionTypes iCause, const CDamageSource &Attacker);

		virtual bool AbsorbDamage (CInstalledDevice *pDevice, CSpaceObject *pShip, SDamageCtx &Ctx) { Ctx.iAbsorb = 0; return false; }
		virtual bool AbsorbsWeaponFire (CInstalledDevice *pDevice, CSpaceObject *pSource, CInstalledDevice *pWeapon) { return false; }
		virtual bool Activate (CInstalledDevice *pDevice, 
							   CSpaceObject *pSource, 
							   CSpaceObject *pTarget,
							   bool *retbSourceDestroyed,
							   bool *retbConsumedItems = NULL) { return false; }
		virtual CShieldClass *AsShieldClass (void) { return NULL; }
		virtual CWeaponClass *AsWeaponClass (void) { return NULL; }
		virtual int CalcFireSolution (CInstalledDevice *pDevice, CSpaceObject *pSource, CSpaceObject *pTarget) { return -1; }
		virtual int CalcPowerUsed (SUpdateCtx &Ctx, CInstalledDevice *pDevice, CSpaceObject *pSource) { return 0; }
		virtual bool CanBeDamaged (void) { return !m_fDeviceDamageImmune; }
		virtual bool CanBeDisabled (CItemCtx &Ctx) { return (GetPowerRating(Ctx) != 0); }
		virtual bool CanBeDisrupted(void) { return !m_fDeviceDisruptImmune; }
		virtual bool CanHitFriends (void) { return true; }
		virtual bool CanRotate (CItemCtx &Ctx, int *retiMinFireArc = NULL, int *retiMaxFireArc = NULL) const { return false; }
		virtual void Deplete (CInstalledDevice *pDevice, CSpaceObject *pSource) { }
		virtual bool FindAmmoDataField (const CItem &Ammo, const CString &sField, CString *retsValue) const { return false; }
        virtual ICCItem *FindAmmoItemProperty (CItemCtx &Ctx, const CItem &Ammo, const CString &sProperty) { return CDeviceClass::FindItemProperty(Ctx, sProperty); }
		virtual bool FindDataField (const CString &sField, CString *retsValue) { return false; }
		virtual ICCItem *FindItemProperty (CItemCtx &Ctx, const CString &sName);
		virtual int GetActivateDelay (CInstalledDevice *pDevice, CSpaceObject *pSource) const { return 0; }
		virtual int GetAmmoVariant (const CItemType *pItem) const { return -1; }
		virtual int GetCounter (CInstalledDevice *pDevice, CSpaceObject *pSource, CounterTypes *retiType = NULL, int *retiLevel = NULL) { return 0; }
		virtual const CCargoDesc *GetCargoDesc (CItemCtx &Ctx) const { return NULL; }
		virtual const DamageDesc *GetDamageDesc (CItemCtx &Ctx) { return NULL; }
		virtual int GetDamageEffectiveness (CSpaceObject *pAttacker, CInstalledDevice *pWeapon) { return 0; }
		virtual DamageTypes GetDamageType (CItemCtx &Ctx, const CItem &Ammo = CItem()) const { return damageGeneric; }
		virtual int GetDefaultFireAngle (CInstalledDevice *pDevice, CSpaceObject *pSource) const { return 0; }
		virtual int GetHitPoints (CItemCtx &ItemCtx, int *retiMaxHP = NULL) const { return 0; }
		virtual DWORD GetLinkedFireOptions (CItemCtx &Ctx) { return 0; }
		virtual Metric GetMaxEffectiveRange (CSpaceObject *pSource, CInstalledDevice *pDevice, CSpaceObject *pTarget) { return 0.0; }
		virtual Metric GetMaxRange (CItemCtx &ItemCtx) { return 0.0; }

		static const DWORD GPO_FLAG_NORMAL_POWER = 0x00000001;	//	Power when not damage and not enhanced
		virtual int GetPowerOutput (CItemCtx &Ctx, DWORD dwFlags = 0) const { return 0; }
		virtual int GetPowerRating (CItemCtx &Ctx) const { return 0; }
		virtual bool GetReferenceDamageAdj (const CItem *pItem, CSpaceObject *pInstalled, int *retiHP, int *retArray) const { return false; }
		virtual bool GetReferenceDamageType (CItemCtx &Ctx, const CItem &Ammo, DamageTypes *retiDamage, CString *retsReference) const { return false; }
		virtual void GetSelectedVariantInfo (CSpaceObject *pSource, 
											 CInstalledDevice *pDevice,
											 CString *retsLabel,
											 int *retiAmmoLeft,
											 CItemType **retpType = NULL) { if (retsLabel) *retsLabel = NULL_STR; if (retiAmmoLeft) *retiAmmoLeft = -1; if (retpType) *retpType = NULL; }
		virtual Metric GetShotSpeed (CItemCtx &Ctx) const { return 0.0; }
		virtual void GetStatus (CInstalledDevice *pDevice, CSpaceObject *pSource, int *retiStatus, int *retiMaxStatus) { *retiStatus = 0; *retiMaxStatus = 0; }
		virtual int GetValidVariantCount (CSpaceObject *pSource, CInstalledDevice *pDevice) { return 0; }
		virtual int GetWeaponEffectiveness (CSpaceObject *pSource, CInstalledDevice *pDevice, CSpaceObject *pTarget) { return 0; }
		virtual bool IsAmmoWeapon (void) { return false; }
		virtual bool IsAreaWeapon (CSpaceObject *pSource, CInstalledDevice *pDevice) { return false; }
		virtual bool IsAutomatedWeapon (void) { return false; }
		virtual bool IsExternal (void) const { return (m_fExternal ? true : false); }
		virtual bool IsFuelCompatible (CItemCtx &Ctx, const CItem &FuelItem) { return false; }
		virtual bool IsTrackingWeapon (CItemCtx &Ctx) { return false; }
		virtual bool IsVariantSelected (CSpaceObject *pSource, CInstalledDevice *pDevice) { return true; }
		virtual bool IsWeaponAligned (CSpaceObject *pShip, CInstalledDevice *pDevice, CSpaceObject *pTarget, int *retiAimAngle = NULL, int *retiFireAngle = NULL) { return false; }
		virtual bool NeedsAutoTarget (CItemCtx &Ctx, int *retiMinFireArc = NULL, int *retiMaxFireArc = NULL) { return false; }
		virtual CString OnGetReference (CItemCtx &Ctx, const CItem &Ammo = CItem(), DWORD dwFlags = 0) { return NULL_STR; }
		virtual void OnInstall (CInstalledDevice *pDevice, CSpaceObject *pSource, CItemListManipulator &ItemList) { }
		virtual void OnUninstall (CInstalledDevice *pDevice, CSpaceObject *pSource, CItemListManipulator &ItemList) { }
		virtual void Recharge (CInstalledDevice *pDevice, CShip *pShip, int iStatus) { }
		virtual bool RequiresItems (void) const { return false; }
		virtual void Reset (CInstalledDevice *pDevice, CSpaceObject *pSource) { }
		virtual bool SelectFirstVariant (CSpaceObject *pSource, CInstalledDevice *pDevice) { return false; }
		virtual bool SelectNextVariant (CSpaceObject *pSource, CInstalledDevice *pDevice, int iDir = 1) { return false; }
		virtual bool SetCounter (CInstalledDevice *pDevice, CSpaceObject *pSource, CounterTypes iCounter, int iLevel) { return false; }
		virtual void SetHitPoints (CItemCtx &ItemCtx, int iHP) { }
		virtual bool SetItemProperty (CItemCtx &Ctx, const CString &sName, ICCItem *pValue, CString *retsError);
		virtual bool ShowActivationDelayCounter (CSpaceObject *pSource, CInstalledDevice *pDevice) { return false; }
		virtual void Update (CInstalledDevice *pDevice, CSpaceObject *pSource, SDeviceUpdateCtx &Ctx) { }
		virtual bool ValidateSelectedVariant (CSpaceObject *pSource, CInstalledDevice *pDevice) { return false; }

		static bool FindAmmoDataField (CItemType *pItem, const CString &sField, CString *retsValue);
		static bool FindWeaponFor (CItemType *pItem, CDeviceClass **retpWeapon = NULL, int *retiVariant = NULL, CWeaponFireDesc **retpDesc = NULL);
		static ItemCategories GetItemCategory (DeviceNames iDev);
		static CString GetLinkedFireOptionString (DWORD dwOptions);
		static ALERROR ParseLinkedFireOptions (SDesignLoadCtx &Ctx, const CString &sDesc, DWORD *retdwOptions);
		static int ParseVariantFromPropertyName (const CString &sName, CString *retsName = NULL);

	protected:
		inline ItemCategories GetDefinedSlotCategory (void) { return m_iSlotCategory; }
		virtual ItemCategories GetImplCategory (void) const = 0;
		ALERROR InitDeviceFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType);

		virtual void OnAccumulateAttributes (CItemCtx &ItemCtx, const CItem &Ammo, TArray<SDisplayAttribute> *retList) { }
		virtual bool OnAccumulateEnhancements (CItemCtx &Device, CInstalledArmor *pTarget, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements) { return false; }
		virtual bool OnAccumulateEnhancements (CItemCtx &Device, CInstalledDevice *pTarget, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements) { return false; }
        virtual bool OnAccumulatePerformance (CItemCtx &ItemCtx, SShipPerformanceCtx &Ctx) const { return false; }
		virtual void OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) { }
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) { return NOERROR; }
		virtual CEffectCreator *OnFindEffectCreator (const CString &sUNID) { return NULL; }
		virtual ALERROR OnFinishBind (SDesignLoadCtx &Ctx) { return NOERROR; }
        virtual Metric OnGetScaledCostAdj (CItemCtx &Ctx) const;
		virtual void OnMarkImages (void) { }

	private:
		CItemType *m_pItemType;					//	Item for device
		int m_iSlots;							//	Number of device slots required
		ItemCategories m_iSlotCategory;			//	Count as this category (for device slot purposes)

		COverlayTypeRef m_pOverlayType;			//	Associated overlay (may be NULL)

		int m_iMaxHPBonus;						//	Max HP bonus for this device
		CEnhancementDesc m_Enhancements;		//	Enhancements confered on other items
		CAbilitySet m_Equipment;				//	Equipment installed by this device

		SEventHandlerDesc m_CachedEvents[evtCount];	//	Cached events

		DWORD m_fExternal:1;					//	Device is external
		DWORD m_fDeviceDamageImmune:1;			//	Prevents this device from being damaged by ion effects. Only applies to devices that can be damaged in the first place. Default: false
		DWORD m_fDeviceDisruptImmune:1;			//	Prevents this device from being disrupted by ion effects. Only applies to devices that can be damaged in the first place. Default: false

		DWORD m_dwSpare:29;
	};

//	IDeviceGenerator -----------------------------------------------------------

struct SDeviceDesc
	{
	CString sID;

	CItem Item;

	int iPosAngle = 0;
	int iPosRadius = 0;
	int iPosZ = 0;
	bool b3DPosition = false;
	bool bExternal = false;
	bool bCannotBeEmpty = false;
	ItemFates iFate = fateNone;

	bool bOmnidirectional = false;
	int iMinFireArc = 0;
	int iMaxFireArc = 0;
	bool bSecondary = false;

	DWORD dwLinkedFireOptions = 0;

	int iSlotBonus = 0;

	CItemList ExtraItems;
	};

class CDeviceDescList
	{
	public:
		CDeviceDescList (void);
		~CDeviceDescList (void);

		void AddDeviceDesc (const SDeviceDesc &Desc);
		inline void DeleteAll (void) { m_List.DeleteAll(); }
		inline int GetCount (void) const { return m_List.GetCount(); }
		int GetCountByID (const CString &sID) const;
		inline CDeviceClass *GetDeviceClass (int iIndex) const;
		inline const SDeviceDesc &GetDeviceDesc (int iIndex) const { return m_List[iIndex]; }
		const SDeviceDesc *GetDeviceDescByName (DeviceNames iDev) const;
		CDeviceClass *GetNamedDevice (DeviceNames iDev) const;

	private:
		TArray<SDeviceDesc> m_List;
	};

struct SDeviceGenerateCtx
	{
	SDeviceGenerateCtx (void) :
			iLevel(1),
			pRoot(NULL),
			pResult(NULL)
		{ }

	int iLevel;
	IDeviceGenerator *pRoot;

	CDeviceDescList *pResult;
	};

class IDeviceGenerator
	{
	public:
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, IDeviceGenerator **retpGenerator);

		virtual ~IDeviceGenerator (void) { }
		virtual void AddDevices (SDeviceGenerateCtx &Ctx) { }
		virtual void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) { }
		virtual IDeviceGenerator *GetGenerator (int iIndex) { return NULL; }
		virtual int GetGeneratorCount (void) { return 0; }
		virtual bool IsVariant (void) const = 0;
		virtual ALERROR LoadFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) { return NOERROR; }
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) { return NOERROR; }

		virtual bool FindDefaultDesc (DeviceNames iDev, SDeviceDesc *retDesc) const { return false; }
		virtual bool FindDefaultDesc (CSpaceObject *pSource, const CItem &Item, SDeviceDesc *retDesc) const { return false; }
		virtual bool FindDefaultDesc (const CDeviceDescList &DescList, const CItem &Item, SDeviceDesc *retDesc) const { return false; }

		static ALERROR InitDeviceDescFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, SDeviceDesc *retDesc);
	};

class CInstalledDevice
	{
	public:
		CInstalledDevice (void);

		//	Create/Install/uninstall/Save/Load methods

		void FinishInstall (CSpaceObject *pSource);
		inline CDeviceClass *GetClass (void) const { return m_pClass; }
		inline DWORD GetUNID (void) const { return m_pClass.GetUNID(); }
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		void InitFromDesc (const SDeviceDesc &Desc);
		void Install (CSpaceObject *pObj, CItemListManipulator &ItemList, int iDeviceSlot, bool bInCreate = false);
		ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx);
		void ReadFromStream (CSpaceObject *pSource, SLoadCtx &Ctx);
		inline void SetClass (CDeviceClass *pClass) { m_pClass.Set(pClass); }
		void Uninstall (CSpaceObject *pObj, CItemListManipulator &ItemList);
		void Update (CSpaceObject *pSource, CDeviceClass::SDeviceUpdateCtx &Ctx);
		void WriteToStream (IWriteStream *pStream);

		//	These methods are about CInstalledDevice properties; not about properties
		//	of the device class. For example, IsOmniDirectional does not check the
		//	properties of the device class

		inline bool CanBeEmpty (void) const { return !m_fCannotBeEmpty; }
		inline int GetCharges (CSpaceObject *pSource) { return (m_pItem ? m_pItem->GetCharges() : 0); }
		inline DWORD GetData (void) const { return m_dwData; }
		inline int GetDeviceSlot (void) const { return m_iDeviceSlot; }
		inline TSharedPtr<CItemEnhancementStack> GetEnhancementStack (void) const { return m_pEnhancements; }
		ItemFates GetFate (void) const;
		inline int GetFireArc (void) const { return (IsOmniDirectional() ? 360 : AngleRange(m_iMinFireArc, m_iMaxFireArc)); }
		inline int GetFireAngle (void) const { return m_iFireAngle; }
		int GetHitPointsPercent (CSpaceObject *pSource);
		inline const CString &GetID (void) const { return m_sID; }
		inline CItem *GetItem (void) const { return m_pItem; }
		DWORD GetLinkedFireOptions (void) const;
        inline int GetLevel (void) const { return (m_pItem ? m_pItem->GetLevel() : GetClass()->GetLevel()); }
		inline int GetMinFireArc (void) const { return m_iMinFireArc; }
		inline int GetMaxFireArc (void) const { return m_iMaxFireArc; }
		inline COverlay *GetOverlay (void) const { return m_pOverlay; }
		inline int GetPosAngle (void) const { return m_iPosAngle; }
		inline int GetPosRadius (void) const { return m_iPosRadius; }
		inline int GetPosZ (void) const { return m_iPosZ; }
		inline int GetRotation (void) const { return AngleMiddle(m_iMinFireArc, m_iMaxFireArc); }
		inline int GetSlotBonus (void) const { return m_iSlotBonus; }
		inline int GetSlotPosIndex (void) const { return m_iSlotPosIndex; }
		inline int GetTemperature (void) const { return m_iTemperature; }
		inline int GetTimeUntilReady (void) const { return m_iTimeUntilReady; }
		inline bool Has3DPos (void) const { return m_f3DPosition; }
		inline void IncTemperature (int iChange) { m_iTemperature += iChange; }
		inline bool IsDirectional (void) const { return (m_iMinFireArc != m_iMaxFireArc); }
		inline bool IsDuplicate (void) const { return (m_fDuplicate ? true : false); }
		inline bool IsEmpty (void) const { return m_pClass == NULL; }
		inline bool IsEnabled (void) const { return m_fEnabled; }
		inline bool IsExternal (void) const { return m_fExternal; }
		inline bool IsLastActivateSuccessful (void) const { return m_fLastActivateSuccessful; }
		inline bool IsOmniDirectional (void) const { return (m_fOmniDirectional ? true : false); }
		inline bool IsOptimized (void) const { return m_fOptimized; }
		inline bool IsOverdrive (void) const { return m_fOverdrive; }
		inline bool IsReady (void) const { return (m_iTimeUntilReady == 0); }
		inline bool IsRegenerating (void) const { return (m_fRegenerating ? true : false); }
		inline bool IsTriggered (void) const { return (m_fTriggered ? true : false); }
		inline bool IsWaiting (void) const { return (m_fWaiting ? true : false); }
		inline void SetActivateDelay (int iDelay) { m_iActivateDelay = iDelay; }
		inline void SetData (DWORD dwData) { m_dwData = dwData; }
		inline void SetDeviceSlot (int iDev) { m_iDeviceSlot = iDev; }
		inline void SetDuplicate (bool bDuplicate = true) { m_fDuplicate = bDuplicate; }
		bool SetEnabled (CSpaceObject *pSource, bool bEnabled);
		void SetEnhancements (const TSharedPtr<CItemEnhancementStack> &pStack);
		inline void SetExternal (bool bValue) { m_fExternal = bValue; }
		void SetFate (ItemFates iFate);
		inline void SetFireAngle (int iAngle) { m_iFireAngle = iAngle; }
		inline void SetFireArc (int iMinFireArc, int iMaxFireArc) { m_iMinFireArc = iMinFireArc; m_iMaxFireArc = iMaxFireArc; }
		inline void SetID (const CString &sID) { m_sID = sID; }
		inline void SetLastActivateSuccessful (bool bSuccessful) { m_fLastActivateSuccessful = bSuccessful; }
		void SetLinkedFireOptions (DWORD dwOptions);
		inline void SetOmniDirectional (bool bOmnidirectional = true) { m_fOmniDirectional = bOmnidirectional; }
		inline void SetOptimized (bool bOptimized) { m_fOptimized = bOptimized; }
		inline void SetOverdrive (bool bOverdrive) { m_fOverdrive = bOverdrive; }
		inline void SetOverlay (COverlay *pOverlay) { m_pOverlay = pOverlay; }
		inline void SetPosAngle (int iAngle) { m_iPosAngle = iAngle; }
		inline void SetPosRadius (int iRadius) { m_iPosRadius = iRadius; }
		inline void SetPosZ (int iZ) { m_iPosZ = iZ; m_f3DPosition = (iZ != 0); }
		inline void SetRegenerating (bool bRegenerating) { m_fRegenerating = bRegenerating; }
		inline void SetSecondary (bool bSecondary = true) { m_fSecondaryWeapon = bSecondary; }
		inline void SetSlotBonus (int iBonus) { m_iSlotBonus = iBonus; }
		inline void SetSlotPosIndex (int iIndex) { m_iSlotPosIndex = iIndex; }
		inline void SetTemperature (int iTemperature) { m_iTemperature = iTemperature; }
		inline void SetTimeUntilReady (int iDelay) { m_iTimeUntilReady = iDelay; }
		inline void SetTriggered (bool bTriggered) { m_fTriggered = bTriggered; }
		inline void SetWaiting (bool bWaiting) { m_fWaiting = bWaiting; }

		//	These are wrapper methods for a CDeviceClass method of the same name.
		//	We add our object pointer as a parameter to the call.

		inline bool AbsorbDamage (CSpaceObject *pShip, SDamageCtx &Ctx) { if (!IsEmpty()) return m_pClass->AbsorbDamage(this, pShip, Ctx); else return false; }
		inline bool AccumulateEnhancements (CSpaceObject *pSource, CInstalledArmor *pTarget, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements) { return m_pClass->AccumulateEnhancements(CItemCtx(pSource, this), pTarget, EnhancementIDs, pEnhancements); }
		inline bool AccumulateEnhancements (CSpaceObject *pSource, CInstalledDevice *pTarget, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements) { return m_pClass->AccumulateEnhancements(CItemCtx(pSource, this), pTarget, EnhancementIDs, pEnhancements); }
        inline bool AccumulatePerformance (CItemCtx &ItemCtx, SShipPerformanceCtx &Ctx) const { return m_pClass->AccumulatePerformance(ItemCtx, Ctx); }
		inline bool Activate (CSpaceObject *pSource, 
							  CSpaceObject *pTarget,
							  bool *retbSourceDestroyed,
							  bool *retbConsumedItems = NULL)
			{ return m_pClass->Activate(this, pSource, pTarget, retbSourceDestroyed, retbConsumedItems); }
		int CalcPowerUsed (SUpdateCtx &Ctx, CSpaceObject *pSource);
		inline bool CanBeDamaged (void) { return m_pClass->CanBeDamaged(); }
		inline bool CanBeDisabled (CItemCtx &Ctx) { return m_pClass->CanBeDisabled(Ctx); }
		inline bool CanBeDisrupted (void) { return m_pClass->CanBeDisrupted(); }
		inline bool CanHitFriends (void) { return m_pClass->CanHitFriends(); }
		inline bool CanRotate (CItemCtx &Ctx) const { return m_pClass->CanRotate(Ctx); }
		inline void Deplete (CSpaceObject *pSource) { m_pClass->Deplete(this, pSource); }
		int GetActivateDelay (CSpaceObject *pSource) const;
		inline ItemCategories GetCategory (void) const { return m_pClass->GetCategory(); }
		inline int GetCounter (CSpaceObject *pSource, CDeviceClass::CounterTypes *retiCounter = NULL, int *retiLevel = NULL) { return m_pClass->GetCounter(this, pSource, retiCounter, retiLevel); }
		inline const DamageDesc *GetDamageDesc (CItemCtx &Ctx) { return m_pClass->GetDamageDesc(Ctx); }
		inline int GetDamageEffectiveness (CSpaceObject *pAttacker, CInstalledDevice *pWeapon) { return m_pClass->GetDamageEffectiveness(pAttacker, pWeapon); }
		inline int GetDamageType (CItemCtx &Ctx, const CItem &Ammo = CItem()) { return m_pClass->GetDamageType(Ctx, Ammo); }
		inline int GetDefaultFireAngle (CSpaceObject *pSource) { return m_pClass->GetDefaultFireAngle(this, pSource); }
		inline int GetHitPoints (CItemCtx &ItemCtx, int *retiMaxHP = NULL) const { return m_pClass->GetHitPoints(ItemCtx, retiMaxHP); }
		CSpaceObject *GetLastShot (CSpaceObject *pSource, int iIndex) const;
		inline Metric GetMaxEffectiveRange (CSpaceObject *pSource, CSpaceObject *pTarget = NULL) { return m_pClass->GetMaxEffectiveRange(pSource, this, pTarget); }
		inline Metric GetMaxRange (CItemCtx &ItemCtx) const { return m_pClass->GetMaxRange(ItemCtx); }
		inline CString GetName (void) { return m_pClass->GetName(); }
		CVector GetPos (CSpaceObject *pSource);
		CVector GetPosOffset (CSpaceObject *pSource);
		inline int GetPowerRating (CItemCtx &Ctx) const { return m_pClass->GetPowerRating(Ctx); }
		inline void GetSelectedVariantInfo (CSpaceObject *pSource, 
											CString *retsLabel,
											int *retiAmmoLeft,
											CItemType **retpType = NULL)
			{ m_pClass->GetSelectedVariantInfo(pSource, this, retsLabel, retiAmmoLeft, retpType); }
		inline Metric GetShotSpeed (CItemCtx &Ctx) const { return m_pClass->GetShotSpeed(Ctx); }
		inline void GetStatus (CSpaceObject *pSource, int *retiStatus, int *retiMaxStatus) { m_pClass->GetStatus(this, pSource, retiStatus, retiMaxStatus); }
		inline CSpaceObject *GetTarget (CSpaceObject *pSource) const;
		inline int GetValidVariantCount (CSpaceObject *pSource) { return m_pClass->GetValidVariantCount(pSource, this); }
		inline int GetWeaponEffectiveness (CSpaceObject *pSource, CSpaceObject *pTarget) { return m_pClass->GetWeaponEffectiveness(pSource, this, pTarget); }
		inline bool HasLastShots (void) const { return (m_LastShotIDs.GetCount() > 0); }
		int IncCharges (CSpaceObject *pSource, int iChange);
		inline bool IsAutomatedWeapon (void) { return m_pClass->IsAutomatedWeapon(); }
		inline bool IsAreaWeapon (CSpaceObject *pSource) { return m_pClass->IsAreaWeapon(pSource, this); }
		inline bool IsFuelCompatible (CItemCtx &Ctx, const CItem &FuelItem) { return m_pClass->IsFuelCompatible(Ctx, FuelItem); }
		bool IsLinkedFire (CItemCtx &Ctx, ItemCategories iTriggerCat = itemcatNone) const;
		inline bool IsSecondaryWeapon (void) const;
		bool IsSelectable (CItemCtx &Ctx) const;
		inline bool IsVariantSelected (CSpaceObject *pSource) { return (m_pClass ? m_pClass->IsVariantSelected(pSource, this) : true); }
		inline bool OnDestroyCheck (CSpaceObject *pSource, DestructionTypes iCause, const CDamageSource &Attacker) { return m_pClass->OnDestroyCheck(CItemCtx(pSource, this), iCause, Attacker); }
		inline void Recharge (CShip *pShip, int iStatus) { m_pClass->Recharge(this, pShip, iStatus); }
		inline void Reset (CSpaceObject *pShip) { m_pClass->Reset(this, pShip); }
		inline void SelectFirstVariant (CSpaceObject *pSource) { m_pClass->SelectFirstVariant(pSource, this); }
		inline void SelectNextVariant (CSpaceObject *pSource, int iDir = 1) { m_pClass->SelectNextVariant(pSource, this, iDir); }
		void SetCharges (CSpaceObject *pSource, int iCharges);
		inline void SetHitPoints (CItemCtx &ItemCtx, int iHP) { m_pClass->SetHitPoints(ItemCtx, iHP); }
		void SetLastShot (CSpaceObject *pObj, int iIndex);
		void SetLastShotCount (int iCount);
		inline void SetTarget (CSpaceObject *pObj);
		inline bool ShowActivationDelayCounter (CSpaceObject *pSource) { return m_pClass->ShowActivationDelayCounter(pSource, this); }

		//	These are wrapper methods for the CItem behind this device.

		inline DWORD GetDisruptedDuration (void) const { return (m_pItem ? m_pItem->GetDisruptedDuration() : 0); }
		inline bool GetDisruptedStatus (DWORD *retdwTimeLeft = NULL, bool *retbRepairedEvent = NULL) const { return (m_pItem ? m_pItem->GetDisruptedStatus(retdwTimeLeft, retbRepairedEvent) : false); }
		CString GetEnhancedDesc (CSpaceObject *pSource, const CItem *pItem = NULL);
		inline bool IsDamaged (void) const { return (m_pItem ? m_pItem->IsDamaged() : false); }
		inline bool IsDisrupted (void) const { return (m_pItem ? m_pItem->IsDisrupted() : false); }
		inline bool IsEnhanced (void) const { return (m_pItem ? m_pItem->IsEnhanced() : false); }
		inline bool IsWeaponAligned (CSpaceObject *pShip, CSpaceObject *pTarget, int *retiAimAngle = NULL, int *retiFireAngle = NULL) { return m_pClass->IsWeaponAligned(pShip, this, pTarget, retiAimAngle, retiFireAngle); }

		static void PaintDevicePos (const SDeviceDesc &Device, CG32bitImage &Dest, int x, int y, int iScale, int iRotation);

	private:
		inline const CItemEnhancement &GetMods (void) const { return (m_pItem ? m_pItem->GetMods() : CItem::GetNullMod()); }

		CString m_sID;							//	ID for this slot (may match ID in class slot desc)
		CItem *m_pItem;							//	Item installed in this slot
		CDeviceClassRef m_pClass;				//	The device class that is installed here
		COverlay *m_pOverlay;					//	Overlay (if associated)
		DWORD m_dwTargetID;						//	ObjID of target (for tracking secondary weapons)
		TSharedPtr<CItemEnhancementStack> m_pEnhancements;	//	List of enhancements (may be NULL)
		TArray<DWORD> m_LastShotIDs;			//	ObjID of last shots (only for continuous beams)

		DWORD m_dwData;							//	Data specific to device class

		int m_iDeviceSlot:16;					//	Device slot

		int m_iPosAngle:16;						//	Position of installation (degrees)
		int m_iPosRadius:16;					//	Position of installation (pixels)
		int m_iPosZ:16;							//	Position of installation (height)
		int m_iMinFireArc:16;					//	Min angle of fire arc (degrees)
		int m_iMaxFireArc:16;					//	Max angle of fire arc (degrees)

		int m_iTimeUntilReady:16;				//	Timer counting down until ready to activate
		int m_iFireAngle:16;					//	Last fire angle

		int m_iTemperature:16;					//	Temperature for weapons
		int m_iActivateDelay:16;				//	Cached activation delay
		int m_iSlotBonus:16;					//	Bonus from device slot itself
		int m_iSlotPosIndex:16;					//	Slot placement

		DWORD m_fOmniDirectional:1;				//	Installed on turret
		DWORD m_fOverdrive:1;					//	Device has overdrive installed
		DWORD m_fOptimized:1;					//	Device is optimized by alien engineers
		DWORD m_fSecondaryWeapon:1;				//	Secondary weapon
		DWORD m_fEnabled:1;						//	Device is enabled
		DWORD m_fExternal:1;					//	Device is external to hull
		DWORD m_fWaiting:1;						//	Waiting for cooldown, etc.
		DWORD m_fTriggered:1;					//	Device trigger is down (e.g., weapon is firing)

		DWORD m_fRegenerating:1;				//	TRUE if we regenerated on the last tick
		DWORD m_fLastActivateSuccessful:1;		//	TRUE if we successfully fired (last time we tried)
		DWORD m_f3DPosition:1;					//	If TRUE we use m_iPosZ to compute position
		DWORD m_fLinkedFireAlways:1;			//	If TRUE, lkfAlways
		DWORD m_fLinkedFireTarget:1;			//	If TRUE, lkfTarget
		DWORD m_fLinkedFireEnemy:1;				//	If TRUE, lkfEnemy
		DWORD m_fDuplicate:1;					//	If TRUE, we have multiple version of the same item type installed
		DWORD m_fCannotBeEmpty:1;				//	If TRUE, slot must always have a device

		DWORD m_fFateSurvives:1;				//	Always survives when ship destroyed
		DWORD m_fFateDamaged:1;					//	Always damaged when ship destroyed
		DWORD m_fFateDestroyed:1;				//	Always destroyed when ship destroyed
		DWORD m_fFateComponetized:1;			//	Always break into components when ship destroyed
		DWORD m_fSpare5:1;
		DWORD m_fSpare6:1;
		DWORD m_fSpare7:1;
		DWORD m_fSpare8:1;

		DWORD m_dwSpare:8;
	};
