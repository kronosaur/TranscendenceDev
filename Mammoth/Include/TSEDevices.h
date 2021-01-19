//	TSEDevices.h
//
//	Defines classes and interfaces for devices.
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CCargoDesc;
class CTargetList;
struct SShipPerformanceCtx;
struct SUpdateCtx;

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
			failNoFire =			7,		//	Same as jammed, but increase temp
			};

		CFailureDesc (EProfile iProfile = profileNone);

		EFailureTypes Failure (CSpaceObject *pSource, CInstalledDevice *pDevice) const;
		ALERROR InitFromDefaults (EProfile iDefault);
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, EProfile iDefault = profileNone);
		bool IsEmpty (void) const { return (m_FailTable.GetCount() == 0); }

		static EFailureTypes ParseFailureType (const CString &sValue);

	private:
		TProbabilityTable<EFailureTypes> m_FailTable;
	};

class CItemLevelCriteria
	{
	public:
		enum ETypes
			{
			checkNone =						0,	//	Matches everything

			checkLessEqualToLevel =			1,	//	Target item level is <= to our level
			checkLessThanLevel =			2,	//	Target item level is < than our level
			checkGreaterEqualToLevel =		3,	//	Target item level is >= to our level
			checkGreaterThanLevel =			4,	//	Target item level is > than our level
			checkLessEqualToRepairLevel =	5,	//	Target item repair level is <= to our level
			checkLessThanRepairLevel =		6,	//	Target item repair level is < to our level
			checkGreaterEqualToRepairLevel = 7,	//	Target item repair level is >= to our level
			checkGreaterThanRepairLevel =	8,	//	Target item repair level is > than our level
			};

		ALERROR InitFromXML (SDesignLoadCtx &Ctx, const CString &sValue);
		bool IsEmpty (void) const { return m_iType == checkNone; }
		bool MatchesCriteria (int iLevel, const CItem &Item) const;
		void ReadFromStream (SLoadCtx &Ctx);
		void WriteToStream (IWriteStream &Stream) const;

	private:
		ETypes m_iType = checkNone;
	};

class CEnhancementDesc
	{
	public:
		bool Accumulate (int iLevel, const CItem &Target, CItemEnhancementStack &Enhancements, TArray<CString> *pEnhancementIDs = NULL) const;
		ALERROR Bind (SDesignLoadCtx &Ctx);
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pEnhancerType);
		void InsertHPBonus (int iBonus);
		void InsertMissileDefense (void);
		bool IsEmpty (void) const { return (m_Enhancements.GetCount() == 0); }
		int GetCount (void) const { return m_Enhancements.GetCount(); }
		const CItemEnhancement &GetEnhancement (int iIndex) const { return m_Enhancements[iIndex].Enhancement; }
		void ReadFromStream (SLoadCtx &Ctx);
		void SetCriteria (int iEntry, const CItemCriteria &Criteria);
		void SetLevelCheck (int iEntry, const CItemLevelCriteria &LevelCheck);
		void SetType (int iEntry, const CString &sType);
		void WriteToStream (IWriteStream &Stream) const;

	private:
		struct SEnhancerDesc
			{
			CString sType;						//	Type of enhancement
			CItemCriteria Criteria;				//	Items that we enhance
			CItemLevelCriteria LevelCheck;		//	Criteria for item level
			CItemEnhancement Enhancement;		//	Enhancement conferred
			};

		ALERROR InitFromEnhanceXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pEnhancerType, SEnhancerDesc &Enhance);

		TArray<SEnhancerDesc> m_Enhancements;	//	Enhancements conferred on other items
	};

class CDeviceRotationDesc
	{
	public:
		enum ETypes
			{
			rotUnknown,

			rotNone,							//	Cannot rotate
			rotSwivel,							//	Can swivel but not 360 degrees
			rotOmnidirectional,					//	Omnidirectional
			};

	private:
		ETypes m_iType = rotUnknown;
		int m_iMinFireArc = -1;
		int m_iMaxFireArc = -1;
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
			failWeaponNoFire,
			};

		enum LinkedFireOptions
			{
			lkfAlways =				0x0000001,	//	Linked to fire button
			lkfTargetInRange =		0x0000002,	//	Fire only if the target is in range
			lkfEnemyInRange =		0x0000004,	//	Fire only an enemy is in range
			lkfSelected =			0x0000008,  //  All weapons of this type selectable for linked fire by player
			lkfSelectedVariant =	0x0000010,	//  As above, but applies on combinations of type AND variant, rather than all of a given type regardless of variant
			lkfNever =				0x0000020,  //  Never fire this weapon
			};

		enum ECachedHandlers
			{
			evtGetOverlayType			= 0,

			evtCount					= 1,
			};

		struct SDeviceUpdateCtx
			{
			SDeviceUpdateCtx (SUpdateCtx &Ctx, int iTickArg = 0) :
					m_ObjCtx(Ctx),
					iTick(iTickArg)
				{ }

			SDeviceUpdateCtx (SUpdateCtx &Ctx, CTargetList &TargetListArg, int iTickArg = 0) :
					m_ObjCtx(Ctx),
					m_pTargetList(&TargetListArg),
					iTick(iTickArg)
				{ }

			SUpdateCtx &GetObjCtx () { return m_ObjCtx; }
			CTargetList &GetTargetList ();
			CTargetList *GetTargetListOverride () const { return m_pTargetList; }

			void ResetOutputs (void)
				{
				bConsumedItems = false;
				bDisrupted = false;
				bRepaired = false;
				bSetDisabled = false;
				}
					
			int iTick = 0;

			//	Outputs

			bool bConsumedItems = false;
			bool bDisrupted = false;
			bool bRepaired = false;
			bool bSetDisabled = false;

			private:
				SUpdateCtx &m_ObjCtx;
				CTargetList *m_pTargetList = NULL;	//	Overrides ObjCtx.GetTargetList()
			};

		struct SActivateCtx
			{
			SActivateCtx (SUpdateCtx &Ctx, CSpaceObject *pTargetArg, int iFireAngleArg = -1) :
					m_ObjCtx(Ctx),
					pTarget(pTargetArg),
					iFireAngle(iFireAngleArg)
				{ }

			SActivateCtx (SUpdateCtx &Ctx, CSpaceObject *pTargetArg, CTargetList &TargetListArg, int iFireAngleArg = -1) :
					m_ObjCtx(Ctx),
					pTarget(pTargetArg),
					m_pTargetList(&TargetListArg),
					iFireAngle(iFireAngleArg)
				{ }

			SActivateCtx (SDeviceUpdateCtx &UpdateCtx, CSpaceObject *pTargetArg = NULL, int iFireAngleArg = -1) :
					m_ObjCtx(UpdateCtx.GetObjCtx()),
					m_pTargetList(UpdateCtx.GetTargetListOverride()),
					pTarget(pTargetArg),
					iFireAngle(iFireAngleArg)
				{
				}

			CTargetList &GetTargetList ();
			void SetTargetList (CTargetList &TargetListArg)
				{
				m_pTargetList = &TargetListArg;
				}

			//	Inputs to Activate

			CSpaceObject *pTarget = NULL;
			int iFireAngle = -1;

			//	Used internally

			int iRepeatingCount = 0;

			//	Status results

			bool bConsumedItems = false;

			private:
				SUpdateCtx &m_ObjCtx;
				CTargetList *m_pTargetList = NULL;	//	Overrides ObjCtx.GetTargetList()
			};

		//	NOTES
		//
		//	With the move to use CDeviceItem we're changing some of the standard
		//	calling conventions here. A few guidelines:
		//
		//	If a call is valid for installed and uninstalled devices, then it 
		//	should take a CDeviceItem reference. If the call is only valid for
		//	installed items, then it should take a CInstalledDevice reference.

		CDeviceClass (void) : m_pItemType(NULL) { }
		virtual ~CDeviceClass (void) { }

		void AccumulateAttributes (const CDeviceItem &DeviceItem, const CItem &Ammo, TArray<SDisplayAttribute> *retList) const;
		bool AccumulateEnhancements (CItemCtx &Device, CInstalledArmor *pTarget, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements);
		bool AccumulateEnhancements (CItemCtx &Device, CInstalledDevice *pTarget, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements);
		bool AccumulatePerformance (CItemCtx &ItemCtx, SShipPerformanceCtx &Ctx) const;
		void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed);
		ALERROR Bind (SDesignLoadCtx &Ctx);
		bool CanBeDamaged (void) const { return (!m_fDeviceDamageImmune && (m_fCanBeDamagedOverride || OnCanBeDamaged())); }
		bool CanBeDisabled (CItemCtx &Ctx) const { return m_fCanBeDisabledOverride || OnCanBeDisabled(Ctx); }
		bool CanBeDisrupted (void) const { return (!m_fDeviceDisruptImmune && (m_fCanBeDisruptedOverride || OnCanBeDisrupted())); }
		CEffectCreator *FindEffectCreator (const CString &sUNID) { return OnFindEffectCreator(sUNID); }
		bool FindEventHandlerDeviceClass (ECachedHandlers iEvent, SEventHandlerDesc *retEvent = NULL) const 
			{ 
			if (m_CachedEvents[iEvent].pCode == NULL)
				return false;

			if (retEvent) *retEvent = m_CachedEvents[iEvent];
			return true;
			}

		COverlayType *FireGetOverlayType (CItemCtx &Ctx) const;
		bool GetAmmoItemPropertyBool (CItemCtx &Ctx, const CItem &Ammo, const CString &sProperty);
		Metric GetAmmoItemPropertyDouble (CItemCtx &Ctx, const CItem &Ammo, const CString &sProperty);
		ItemCategories GetCategory (void) const { return (m_iSlotCategory == itemcatNone ? GetImplCategory() : m_iSlotCategory); }
		CString GetDataField (const CString &sField) { CString sValue; FindDataField(sField, &sValue); return sValue; }
		int GetDataFieldInteger (const CString &sField) { CString sValue; if (FindDataField(sField, &sValue)) return strToInt(sValue, 0, NULL); else return 0; }
		int GetInstallCost (CItemCtx &ItemCtx);
		CItemType *GetItemType (void) const { return m_pItemType; }
		inline int GetLevel (void) const;
		int GetMaxHPBonus (void) const { return m_iMaxHPBonus; }
		inline CString GetName (void);
		COverlayType *GetOverlayType(void) const { return m_pOverlayType; }
		CString GetReference (CItemCtx &Ctx, const CItem &Ammo = CItem(), DWORD dwFlags = 0);
		CString GetReferencePower (CItemCtx &Ctx);
		Metric GetScaledCostAdj (CItemCtx &Ctx) const { return OnGetScaledCostAdj(Ctx); }
		int GetSlotsRequired (void) const { return m_iSlots; }
		inline DWORD GetUNID (void) const;
		void MarkImages (void) { DEBUG_TRY OnMarkImages(); DEBUG_CATCH }
		bool OnDestroyCheck (CItemCtx &ItemCtx, DestructionTypes iCause, const CDamageSource &Attacker);
		ALERROR PrepareBind (SDesignLoadCtx &Ctx) { return OnPrepareBind(Ctx); }

		virtual bool AbsorbDamage (CInstalledDevice *pDevice, CSpaceObject *pShip, SDamageCtx &Ctx) { Ctx.iAbsorb = 0; return false; }
		virtual bool AbsorbsWeaponFire (CInstalledDevice *pDevice, CSpaceObject *pSource, CInstalledDevice *pWeapon) { return false; }
		virtual bool Activate (CInstalledDevice &Device, SActivateCtx &ActivateCtx) { return false; }
		virtual CShieldClass *AsShieldClass (void) { return NULL; }
		virtual CWeaponClass *AsWeaponClass (void) { return NULL; }
		virtual bool CalcFireSolution (const CInstalledDevice &Device, CSpaceObject &Target, int *retiAimAngle = NULL, Metric *retrDist = NULL) const { return false; }
		virtual int CalcPowerUsed (SUpdateCtx &Ctx, CInstalledDevice *pDevice, CSpaceObject *pSource) { return 0; }
		virtual bool CanHitFriends (void) const { return true; }
		virtual void Deplete (CInstalledDevice *pDevice, CSpaceObject *pSource) { }
		virtual bool FindAmmoDataField (const CItem &Ammo, const CString &sField, CString *retsValue) const { return false; }
		virtual ICCItem *FindAmmoItemProperty (CItemCtx &Ctx, const CItem &Ammo, const CString &sProperty) { return CDeviceClass::FindItemProperty(Ctx, sProperty); }
		virtual bool FindDataField (const CString &sField, CString *retsValue) { return false; }
		virtual ICCItem *FindItemProperty (CItemCtx &Ctx, const CString &sName);
		virtual int GetActivateDelay (CItemCtx &ItemCtx) const { return 0; }
		virtual int GetAmmoVariant (const CItemType *pItem) const { return -1; }
		virtual int GetCounter (CInstalledDevice *pDevice, CSpaceObject *pSource, CounterTypes *retiType = NULL, int *retiLevel = NULL) { return 0; }
		virtual const CCargoDesc *GetCargoDesc (CItemCtx &Ctx) const { return NULL; }
		virtual const DamageDesc *GetDamageDesc (CItemCtx &Ctx) { return NULL; }
		virtual int GetDamageEffectiveness (CSpaceObject *pAttacker, CInstalledDevice *pWeapon) { return 0; }
		virtual DamageTypes GetDamageType (CItemCtx &Ctx, const CItem &Ammo = CItem()) const { return damageGeneric; }
		virtual int GetDefaultFireAngle (const CDeviceItem &DeviceItem) const { return 0; }
		virtual int GetHitPoints (CItemCtx &ItemCtx, int *retiMaxHP = NULL) const { return 0; }
		virtual DWORD GetLinkedFireOptions (void) const { return 0; }
		virtual Metric GetMaxEffectiveRange (CSpaceObject *pSource, const CInstalledDevice *pDevice, CSpaceObject *pTarget) const { return 0.0; }
		virtual Metric GetMaxRange (CItemCtx &ItemCtx) { return 0.0; }

		static const DWORD GPO_FLAG_NORMAL_POWER = 0x00000001;	//	Power when not damage and not enhanced
		virtual int GetPowerOutput (CItemCtx &Ctx, DWORD dwFlags = 0) const { return 0; }
		virtual int GetPowerRating (CItemCtx &Ctx, int *retiIdlePowerUse = NULL) const { if (retiIdlePowerUse) *retiIdlePowerUse = 0; return 0; }
		virtual bool GetReferenceDamageAdj (const CItem *pItem, CSpaceObject *pInstalled, int *retiHP, int *retArray) const { return false; }
		virtual bool GetReferenceDamageType (CItemCtx &Ctx, const CItem &Ammo, DamageTypes *retiDamage, CString *retsReference) const { return false; }
		virtual int GetReflectChance (const CDeviceItem &DeviceItem, const DamageDesc &Damage) const { return 0; }
		virtual CDeviceRotationDesc::ETypes GetRotationType (const CDeviceItem &DeviceItem, int *retiMinArc = NULL, int *retiMaxArc = NULL) const { return CDeviceRotationDesc::rotNone; }
		virtual void GetSelectedVariantInfo (const CSpaceObject *pSource, 
											 const CInstalledDevice *pDevice,
											 CString *retsLabel,
											 int *retiAmmoLeft,
											 CItemType **retpType = NULL,
											 bool bUseCustomAmmoCountHandler = false) { if (retsLabel) *retsLabel = NULL_STR; if (retiAmmoLeft) *retiAmmoLeft = -1; if (retpType) *retpType = NULL; }
		virtual Metric GetShotSpeed (CItemCtx &Ctx) const { return 0.0; }
		virtual void GetStatus (const CInstalledDevice *pDevice, const CSpaceObject *pSource, int *retiStatus, int *retiMaxStatus) { *retiStatus = 0; *retiMaxStatus = 0; }
		virtual DWORD GetTargetTypes (const CDeviceItem &DeviceItem) const { return 0; }
		virtual int GetValidVariantCount (CSpaceObject *pSource, CInstalledDevice *pDevice) { return 0; }
		virtual int GetWeaponEffectiveness (const CDeviceItem &DeviceItem, CSpaceObject *pTarget) const { return 0; }
		virtual const CWeaponFireDesc *GetWeaponFireDesc (const CDeviceItem &DeviceItem, const CItem &Ammo = CItem()) const { return NULL; }
		virtual const CWeaponFireDesc &GetWeaponFireDescForVariant (const CDeviceItem &DeviceItem, int iVariant) const { return CWeaponFireDesc::Null(); }
		virtual int GetWeaponVariantCount (const CDeviceItem &DeviceItem) const { return 0; }
		virtual bool IsAmmoWeapon (void) { return false; }
		virtual bool IsAreaWeapon (const CDeviceItem &DeviceItem) const { return false; }
		virtual bool IsAutomatedWeapon (void) { return false; }
		virtual bool IsExternal (void) const { return (m_fExternal ? true : false); }
		virtual bool IsFirstVariantSelected(CSpaceObject *pSource, CInstalledDevice *pDevice) { return true; }
		virtual bool IsFuelCompatible (CItemCtx &Ctx, const CItem &FuelItem) { return false; }
		virtual bool IsLastVariantSelected (CSpaceObject *pSource, CInstalledDevice *pDevice) { return true; }
		virtual bool IsTrackingWeapon (const CDeviceItem &DeviceItem) const { return false; }
		virtual bool IsVariantSelected (CSpaceObject *pSource, CInstalledDevice *pDevice) { return true; }
		virtual bool IsWeaponAligned (CSpaceObject *pShip, const CInstalledDevice *pDevice, CSpaceObject *pTarget, int *retiAimAngle = NULL, int *retiFireAngle = NULL) const { return false; }
		virtual bool IsWeaponVariantValid (const CDeviceItem &DeviceItem, int iVariant) const { return false; }
		virtual bool NeedsAutoTarget (const CDeviceItem &DeviceItem, int *retiMinFireArc = NULL, int *retiMaxFireArc = NULL) const { return false; }
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
		virtual ESetPropertyResult SetItemProperty (CItemCtx &Ctx, const CString &sName, const ICCItem *pValue, CString *retsError);
		virtual bool ShowActivationDelayCounter (CSpaceObject *pSource, CInstalledDevice *pDevice) { return false; }
		virtual void Update (CInstalledDevice *pDevice, CSpaceObject *pSource, SDeviceUpdateCtx &Ctx) { }
		virtual bool UsesLauncherControls (void) const { return false; }
		virtual bool ValidateSelectedVariant (CSpaceObject *pSource, CInstalledDevice *pDevice) { return false; }

		static DWORD CombineLinkedFireOptions (DWORD dwSrc1, DWORD dwSrc2);
		static bool FindAmmoDataField (CItemType *pItem, const CString &sField, CString *retsValue);
		static bool FindWeaponFor (CItemType *pItem, CDeviceClass **retpWeapon = NULL, int *retiVariant = NULL, CWeaponFireDesc **retpDesc = NULL);
		static ItemCategories GetItemCategory (DeviceNames iDev);
		static CString GetLinkedFireOptionString (DWORD dwOptions);
		static ALERROR ParseLinkedFireOptions (SDesignLoadCtx &Ctx, const CString &sDesc, DWORD *retdwOptions);
		static int ParseVariantFromPropertyName (const CString &sName, CString *retsName = NULL);

	protected:
		ItemCategories GetDefinedSlotCategory (void) { return m_iSlotCategory; }
		virtual ItemCategories GetImplCategory (void) const = 0;
		CUniverse &GetUniverse (void) const;
		ALERROR InitDeviceFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType);

		virtual void OnAccumulateAttributes (const CDeviceItem &DeviceItem, const CItem &Ammo, TArray<SDisplayAttribute> *retList) const { }
		virtual bool OnAccumulateEnhancements (CItemCtx &Device, CInstalledArmor *pTarget, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements) { return false; }
		virtual bool OnAccumulateEnhancements (CItemCtx &Device, CInstalledDevice *pTarget, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements) { return false; }
		virtual bool OnAccumulatePerformance (CItemCtx &ItemCtx, SShipPerformanceCtx &Ctx) const { return false; }
		virtual void OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) { }
		virtual bool OnCanBeDamaged (void) const { return true; }
		virtual bool OnCanBeDisabled (CItemCtx &Ctx) const { return (GetPowerRating(Ctx) != 0); }
		virtual bool OnCanBeDisrupted(void) const { return true; }
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) { return NOERROR; }
		virtual CEffectCreator *OnFindEffectCreator (const CString &sUNID) { return NULL; }
		virtual Metric OnGetScaledCostAdj (CItemCtx &Ctx) const;
		virtual void OnMarkImages (void) { }
		virtual ALERROR OnPrepareBind (SDesignLoadCtx &Ctx) { return NOERROR; }

	private:
		CItemType *m_pItemType;					//	Item for device
		int m_iSlots;							//	Number of device slots required
		ItemCategories m_iSlotCategory;			//	Count as this category (for device slot purposes)

		COverlayTypeRef m_pOverlayType;			//	Associated overlay (may be NULL)

		int m_iMaxHPBonus;						//	Max HP bonus for this device
		CEnhancementDesc m_Enhancements;		//	Enhancements conferred on other items
		CAbilitySet m_Equipment;				//	Equipment installed by this device

		SEventHandlerDesc m_CachedEvents[evtCount];	//	Cached events

		DWORD m_fExternal:1;					//	Device is external
		DWORD m_fDeviceDamageImmune:1;			//	Prevents this device from being damaged by ion effects. Only applies to devices that can be damaged in the first place. Default: false
		DWORD m_fDeviceDisruptImmune:1;			//	Prevents this device from being disrupted by ion effects. Only applies to devices that can be damaged in the first place. Default: false
		DWORD m_fCanBeDamagedOverride:1;		//	Device can be damaged even if category generally does not allow it (e.g., cargo holds)
		DWORD m_fCanBeDisabledOverride:1;		//	Device can be disabled even if category generally does not allow it (e.g., cargo holds)
		DWORD m_fCanBeDisruptedOverride:1;		//	Device can be disrupted even if category generally does not allow it (e.g., cargo holds)

		DWORD m_dwSpare:26;
	};

//	IDeviceGenerator -----------------------------------------------------------

struct SDeviceDesc
	{
	CString sID;								//	Slot ID

	CItem Item;									//	Item to add
	CItemList ExtraItems;						//	Extra items to add (e.g., missiles)

	int iPosAngle = 0;							//	Slot position
	int iPosRadius = 0;
	int iPosZ = 0;
	bool b3DPosition = false;

	bool bOmnidirectional = false;				//	Slot turret
	int iMinFireArc = 0;						//	Slot swivel
	int iMaxFireArc = 0;
	int iMaxFireRange = 0;						//	Slot range restriction (light-seconds)

	DWORD dwLinkedFireOptions = 0;				//	Slot linked-fire options
	bool bSecondary = false;
	bool bOnSegment = false;					//	Slot is (logically) on segment with ID = sID
	bool bExternal = false;						//	Slot is external
	bool bCannotBeEmpty = false;				//	Slot cannot be empty
	ItemFates iFate = fateNone;					//	Fate of device when ship destroyed

	CEnhancementDesc Enhancements;				//	Slot enhancements to installed device
	int iSlotBonus = 0;
	double rShotSeparationScale = 1.;			//	Governs scaling of shot separation for dual etc weapons
	};

class CDeviceDescList
	{
	public:
		CDeviceDescList (void);
		~CDeviceDescList (void);

		void AddDeviceDesc (const SDeviceDesc &Desc);
		void DeleteAll (void) { m_List.DeleteAll(); }
		int GetCount (void) const { return m_List.GetCount(); }
		int GetCountByID (const CString &sID) const;
		inline CDeviceClass *GetDeviceClass (int iIndex) const;
		SDeviceDesc &GetDeviceDesc (int iIndex) { return m_List[iIndex]; }
		const SDeviceDesc &GetDeviceDesc (int iIndex) const { return m_List[iIndex]; }
		const SDeviceDesc *GetDeviceDescByName (DeviceNames iDev) const;
		CDeviceItem GetDeviceItem (int iIndex) const { return m_List[iIndex].Item.AsDeviceItemOrThrow(); }
		int GetFireArc (int iIndex) const;
		const CItem &GetItem (int iIndex) const { return m_List[iIndex].Item; }
		CDeviceClass *GetNamedDevice (DeviceNames iDev) const;
		void SetSecondary (bool bValue = true);

	private:
		TArray<SDeviceDesc> m_List;
	};

struct SDeviceGenerateCtx
	{
	SDeviceGenerateCtx (CUniverse &UniverseArg) :
			Universe(UniverseArg)
		{ }

	CUniverse &GetUniverse (void) const { return Universe; }

	CUniverse &Universe;
	int iLevel = 1;
	IDeviceGenerator *pRoot = NULL;

	CDeviceDescList *pResult = NULL;
	bool bNoSlotCriteria = false;				//	If TRUE, do not try to place devices in slots by
												//		criteria (we need this inside OnBind because
												//		we're not set up).

	bool bNoDefaultPos = false;					//	If TRUE, do not place device at 20 pixel offset
												//	(we do that for ships for backwards compatibility)
	};

class IDeviceGenerator
	{
	public:
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, IDeviceGenerator **retpGenerator);

		virtual ~IDeviceGenerator (void) { }
		virtual void AddDevices (SDeviceGenerateCtx &Ctx) { }
		virtual void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) { }
		virtual Metric CalcHullPoints (void) const { return 0.0; }
		virtual IDeviceGenerator *GetGenerator (int iIndex) { return NULL; }
		virtual int GetGeneratorCount (void) { return 0; }
		virtual bool HasItemAttribute (const CString &sAttrib) const { return false; }
		virtual bool IsVariant (void) const = 0;
		virtual ALERROR LoadFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) { return NOERROR; }
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) { return NOERROR; }

		virtual bool FindDefaultDesc (SDeviceGenerateCtx &Ctx, DeviceNames iDev, SDeviceDesc *retDesc) const { return false; }
		virtual bool FindDefaultDesc (SDeviceGenerateCtx &Ctx, CSpaceObject *pSource, const CItem &Item, SDeviceDesc *retDesc) const { return false; }
		virtual bool FindDefaultDesc (SDeviceGenerateCtx &Ctx, const CDeviceDescList &DescList, const CItem &Item, SDeviceDesc *retDesc) const { return false; }
		virtual bool FindDefaultDesc (SDeviceGenerateCtx &Ctx, const CDeviceDescList &DescList, const CString &sID, SDeviceDesc *retDesc) const { return false; }

		static ALERROR InitDeviceDescFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, SDeviceDesc *retDesc);
	};

class CInstalledDevice
	{
	public:
		CInstalledDevice (void) { }

		operator CDeviceItem () const { return GetDeviceItem(); }

		//	Create/Install/uninstall/Save/Load methods

		void FinishInstall (void);
		CDeviceClass *GetClass (void) const { return m_pClass; }
		const CDeviceItem GetDeviceItem (void) const { return (m_pItem ? m_pItem->AsDeviceItemOrThrow() : CItem().AsDeviceItem()); }
		CDeviceItem GetDeviceItem (void) { return (m_pItem ? m_pItem->AsDeviceItemOrThrow() : CItem().AsDeviceItem()); }
		CItem *GetItem (void) const { return m_pItem; }
		DWORD GetUNID (void) const { return m_pClass.GetUNID(); }
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		void InitFromDesc (const SDeviceDesc &Desc);
		void Install (CSpaceObject &Source, CItemListManipulator &ItemList, int iDeviceSlot, bool bInCreate = false);
		ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx);
		void ReadFromStream (CSpaceObject &Source, SLoadCtx &Ctx);
		void SetClass (CDeviceClass *pClass) { m_pClass.Set(pClass); }
		void Uninstall (CSpaceObject *pObj, CItemListManipulator &ItemList);
		void Update (CSpaceObject *pSource, CDeviceClass::SDeviceUpdateCtx &Ctx);
		void WriteToStream (IWriteStream *pStream);

		//	These methods are about CInstalledDevice properties; not about properties
		//	of the device class. For example, IsOmniDirectional does not check the
		//	properties of the device class

		bool CanBeEmpty (void) const { return !m_fCannotBeEmpty; }
		bool CanTargetMissiles (void) const { return m_fCanTargetMissiles; }
		bool GetCachedMaxHP (int &retiMaxHP) const;
		int GetCharges (const CSpaceObject *pSource) const { return (m_pItem ? m_pItem->GetCharges() : 0); }
		DWORD GetContinuousFire (void) const { return (int)(DWORD)LOBYTE(LOWORD(m_dwData)); }
		bool GetCycleFireSettings (void) const { return m_fCycleFire; }
		DWORD GetData (void) const { return m_dwData; }
		int GetDeviceSlot (void) const { return m_iDeviceSlot; }
		TSharedPtr<CItemEnhancementStack> GetEnhancementStack (void) const { return m_pEnhancements; }
		int GetExtraPowerUse (void) const { return m_iExtraPowerUse; }
		ItemFates GetFate (void) const;
		int GetFireAngle (void) const { return m_iFireAngle; }
		int GetHitPointsPercent (const CSpaceObject *pSource) const;
		const CString &GetID (void) const { return m_sID; }
		DWORD GetSlotLinkedFireOptions (void) const;
		int GetLevel (void) const { return (m_pItem ? m_pItem->GetLevel() : GetClass()->GetLevel()); }
		int GetMinFireArc (void) const { return m_iMinFireArc; }
		int GetMaxFireArc (void) const { return m_iMaxFireArc; }
		Metric GetMaxFireRange (void) const { return (m_iMaxFireRange == 0 ? g_InfiniteDistance : LIGHT_SECOND * m_iMaxFireRange); }
		int GetMaxFireRangeLS (void) const { return m_iMaxFireRange; }
		COverlay *GetOverlay (void) const { return m_pOverlay; }
		int GetPosAngle (void) const { return m_iPosAngle; }
		int GetPosRadius (void) const { return m_iPosRadius; }
		int GetPosZ (void) const { return m_iPosZ; }
		int GetRotation (void) const { return AngleMiddle(m_iMinFireArc, m_iMaxFireArc); }
		const CString &GetSegmentID (void) const { return (m_fOnSegment ? m_sID : NULL_STR); }
		const CEnhancementDesc &GetSlotEnhancements (void) const { return m_SlotEnhancements; }
		double GetShotSeparationScale(void) const { return (double)m_iShotSeparationScale / 32767.0; }
		int GetSlotPosIndex (void) const { return m_iSlotPosIndex; }
		int GetTemperature (void) const { return m_iTemperature; }
		int GetTimeUntilReady (void) const { return m_iTimeUntilReady; }
		bool Has3DPos (void) const { return m_f3DPosition; }
		void IncTemperature (int iChange) { m_iTemperature += iChange; }
		bool IsDirectional (void) const { return (m_iMinFireArc != m_iMaxFireArc); }
		bool IsDuplicate (void) const { return (m_fDuplicate ? true : false); }
		bool IsEmpty (void) const { return m_pClass == NULL; }
		bool IsEnabled (void) const { return m_fEnabled; }
		bool IsExternal (void) const { return m_fExternal; }
		bool IsLastActivateSuccessful (void) const { return m_fLastActivateSuccessful; }
		bool IsOmniDirectional (void) const { return (m_fOmniDirectional ? true : false); }
		bool IsOnSegment (void) const { return (m_fOnSegment ? true : false); }
		bool IsOnUsedLastAmmoFlagSet (void) const { return (m_fOnUsedLastAmmo ? true : false); }
		bool IsOptimized (void) const { return m_fOptimized; }
		bool IsOverdrive (void) const { return m_fOverdrive; }
		bool IsReady (void) const { return (m_iTimeUntilReady == 0); }
		bool IsRegenerating (void) const { return (m_fRegenerating ? true : false); }
		bool IsTriggered (void) const { return (m_fTriggered ? true : false); }
		bool IsWorking (void) const { return (IsEnabled() && !IsDamaged() && !IsDisrupted()); }
		bool IsWaiting (void) const { return (m_fWaiting ? true : false); }
		void SetActivateDelay (int iDelay) { m_iActivateDelay = iDelay; }
		void SetCachedMaxHP (int iMaxHP);
		void SetCanTargetMissiles (bool bCanTargetMissiles) { m_fCanTargetMissiles = bCanTargetMissiles; }
		void SetCycleFireSettings (bool bCycleFire) { m_fCycleFire = bCycleFire; }
		void SetData (DWORD dwData) { m_dwData = dwData; }
		void SetDeviceSlot (int iDev) { m_iDeviceSlot = iDev; }
		void SetDuplicate (bool bDuplicate = true) { m_fDuplicate = bDuplicate; }
		bool SetEnabled (CSpaceObject *pSource, bool bEnabled);
		void SetEnhancements (const TSharedPtr<CItemEnhancementStack> &pStack);
		void SetExternal (bool bValue) { m_fExternal = bValue; }
		void SetFate (ItemFates iFate);
		void SetFireAngle (int iAngle) { m_iFireAngle = iAngle; }
		void SetFireArc (int iMinFireArc, int iMaxFireArc) { m_iMinFireArc = iMinFireArc; m_iMaxFireArc = iMaxFireArc; }
		void SetID (const CString &sID) { m_sID = sID; }
		void SetLastActivateSuccessful (bool bSuccessful) { m_fLastActivateSuccessful = bSuccessful; }
		void SetLinkedFireOptions (DWORD dwOptions);
		void SetOmniDirectional (bool bOmnidirectional = true) { m_fOmniDirectional = bOmnidirectional; }
		void SetOnUsedLastAmmoFlag (bool bValue = true) { m_fOnUsedLastAmmo = bValue; }
		void SetOptimized (bool bOptimized) { m_fOptimized = bOptimized; }
		void SetOverdrive (bool bOverdrive) { m_fOverdrive = bOverdrive; }
		void SetOverlay (COverlay *pOverlay) { m_pOverlay = pOverlay; }
		void SetPosAngle (int iAngle) { m_iPosAngle = iAngle; }
		void SetPosRadius (int iRadius) { m_iPosRadius = iRadius; }
		void SetPosZ (int iZ) { m_iPosZ = iZ; m_f3DPosition = (iZ != 0); }
		ESetPropertyResult SetProperty (CItemCtx &Ctx, const CString &sName, const ICCItem *pValue, CString *retsError);
		void SetRegenerating (bool bRegenerating) { m_fRegenerating = bRegenerating; }
		void SetShotSeparationScale(double rShotSeparationScale) { m_iShotSeparationScale = (int)(rShotSeparationScale * 32767.0); }
		void SetSecondary (bool bSecondary = true) { m_fSecondaryWeapon = bSecondary; }
		void SetSlotPosIndex (int iIndex) { m_iSlotPosIndex = iIndex; }
		void SetTemperature (int iTemperature) { m_iTemperature = iTemperature; }
		void SetTimeUntilReady (int iDelay) { m_iTimeUntilReady = iDelay; }
		void SetTriggered (bool bTriggered) { m_fTriggered = bTriggered; }
		void SetWaiting (bool bWaiting) const { m_fWaiting = bWaiting; }

		//	These are wrapper methods for a CDeviceClass method of the same name.
		//	We add our object pointer as a parameter to the call.

		bool AbsorbDamage (CSpaceObject *pShip, SDamageCtx &Ctx) { if (!IsEmpty()) return m_pClass->AbsorbDamage(this, pShip, Ctx); else return false; }
		bool AccumulateEnhancements (CSpaceObject *pSource, CInstalledArmor *pTarget, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements) { CItemCtx ItemCtx(pSource, this); return m_pClass->AccumulateEnhancements(ItemCtx, pTarget, EnhancementIDs, pEnhancements); }
		bool AccumulateEnhancements (CSpaceObject *pSource, CInstalledDevice *pTarget, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements) { CItemCtx ItemCtx(pSource, this); return m_pClass->AccumulateEnhancements(ItemCtx, pTarget, EnhancementIDs, pEnhancements); }
		bool AccumulatePerformance (CItemCtx &ItemCtx, SShipPerformanceCtx &Ctx) const { return m_pClass->AccumulatePerformance(ItemCtx, Ctx); }
		bool AccumulateSlotEnhancements (CSpaceObject *pSource, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements) const;
		bool Activate (CDeviceClass::SActivateCtx &ActivateCtx)
			{ return m_pClass->Activate(*this, ActivateCtx); }
		int CalcPowerUsed (SUpdateCtx &Ctx, CSpaceObject *pSource);
		bool CanBeDamaged (void) { return m_pClass->CanBeDamaged(); }
		bool CanBeDisabled (CItemCtx &Ctx) { return m_pClass->CanBeDisabled(Ctx); }
		bool CanBeDisrupted (void) { return m_pClass->CanBeDisrupted(); }
		bool CanHitFriends (void) const { return m_pClass->CanHitFriends(); }
		void Deplete (CSpaceObject *pSource) { m_pClass->Deplete(this, pSource); }
		int GetActivateDelay (CSpaceObject *pSource) const;
		ItemCategories GetCategory (void) const { return m_pClass->GetCategory(); }
		int GetCounter (CSpaceObject *pSource, CDeviceClass::CounterTypes *retiCounter = NULL, int *retiLevel = NULL) { return m_pClass->GetCounter(this, pSource, retiCounter, retiLevel); }
		const DamageDesc *GetDamageDesc (CItemCtx &Ctx) { return m_pClass->GetDamageDesc(Ctx); }
		int GetDamageEffectiveness (CSpaceObject *pAttacker, CInstalledDevice *pWeapon) { return m_pClass->GetDamageEffectiveness(pAttacker, pWeapon); }
		int GetDamageType (CItemCtx &Ctx, const CItem &Ammo = CItem()) { return m_pClass->GetDamageType(Ctx, Ammo); }
		int GetDefaultFireAngle (void) const { return m_pClass->GetDefaultFireAngle(*this); }
		int GetHitPoints (CItemCtx &ItemCtx, int *retiMaxHP = NULL) const { return m_pClass->GetHitPoints(ItemCtx, retiMaxHP); }
		CSpaceObject *GetLastShot (CSpaceObject *pSource, int iIndex) const;
		Metric GetMaxEffectiveRange (CSpaceObject *pSource, CSpaceObject *pTarget = NULL) const { return m_pClass->GetMaxEffectiveRange(pSource, this, pTarget); }
		Metric GetMaxRange (CItemCtx &ItemCtx) const { return m_pClass->GetMaxRange(ItemCtx); }
		CString GetName (void) { return m_pClass->GetName(); }
		CVector GetPos (const CSpaceObject *pSource) const;
		CVector GetPosOffset (CSpaceObject *pSource);
		int GetPowerRating (CItemCtx &Ctx, int *retiIdlePowerUse = NULL) const { return m_pClass->GetPowerRating(Ctx, retiIdlePowerUse); }
		void GetSelectedVariantInfo (const CSpaceObject *pSource, 
											CString *retsLabel,
											int *retiAmmoLeft,
											CItemType **retpType = NULL) const
			{ m_pClass->GetSelectedVariantInfo(pSource, this, retsLabel, retiAmmoLeft, retpType); }
		Metric GetShotSpeed (CItemCtx &Ctx) const { return m_pClass->GetShotSpeed(Ctx); }
		CSpaceObject *GetSource (void) const { return m_pSource; }
		CSpaceObject &GetSourceOrThrow (void) const { if (m_pSource) return *m_pSource; else throw CException(ERR_FAIL); }
		void GetStatus (const CSpaceObject *pSource, int *retiStatus, int *retiMaxStatus) const { m_pClass->GetStatus(this, pSource, retiStatus, retiMaxStatus); }
		CSpaceObject *GetTarget (CSpaceObject *pSource) const;
		int GetValidVariantCount (CSpaceObject *pSource) { return m_pClass->GetValidVariantCount(pSource, this); }
		bool HasLastShots (void) const { return (m_LastShotIDs.GetCount() > 0); }
		int IncCharges (CSpaceObject *pSource, int iChange);
		bool IsAutomatedWeapon (void) { return m_pClass->IsAutomatedWeapon(); }
		bool IsFirstVariantSelected(CSpaceObject *pSource) { return (m_pClass ? m_pClass->IsFirstVariantSelected(pSource, this) : true); }
		bool IsFuelCompatible (CItemCtx &Ctx, const CItem &FuelItem) { return m_pClass->IsFuelCompatible(Ctx, FuelItem); }
		bool IsLastVariantSelected(CSpaceObject *pSource) { return (m_pClass ? m_pClass->IsLastVariantSelected(pSource, this) : true); }
		bool IsLinkedFire (ItemCategories iTriggerCat = itemcatNone) const;
		bool IsSecondaryWeapon (void) const;
		bool IsSelectable (void) const;
		bool IsVariantSelected (CSpaceObject *pSource) { return (m_pClass ? m_pClass->IsVariantSelected(pSource, this) : true); }
		bool OnDestroyCheck (CSpaceObject *pSource, DestructionTypes iCause, const CDamageSource &Attacker) { CItemCtx ItemCtx(pSource, this); return m_pClass->OnDestroyCheck(ItemCtx, iCause, Attacker); }
		void Recharge (CShip *pShip, int iStatus) { m_pClass->Recharge(this, pShip, iStatus); }
		void Reset (CSpaceObject *pShip) { m_pClass->Reset(this, pShip); }
		void SelectFirstVariant (CSpaceObject *pSource) { m_pClass->SelectFirstVariant(pSource, this); }
		void SelectNextVariant (CSpaceObject *pSource, int iDir = 1) { m_pClass->SelectNextVariant(pSource, this, iDir); }
		void SetCharges (CSpaceObject *pSource, int iCharges);
		void SetHitPoints (CItemCtx &ItemCtx, int iHP) { m_pClass->SetHitPoints(ItemCtx, iHP); }
		void SetLastShot (CSpaceObject *pObj, int iIndex);
		void SetLastShotCount (int iCount);
		inline void SetTarget (CSpaceObject *pObj);
		bool ShowActivationDelayCounter (CSpaceObject *pSource) { return m_pClass->ShowActivationDelayCounter(pSource, this); }

		//	These are wrapper methods for the CItem behind this device.

		DWORD GetDisruptedDuration (void) const { return (m_pItem ? m_pItem->GetDisruptedDuration() : 0); }
		bool GetDisruptedStatus (DWORD *retdwTimeLeft = NULL, bool *retbRepairedEvent = NULL) const { return (m_pItem ? m_pItem->GetDisruptedStatus(retdwTimeLeft, retbRepairedEvent) : false); }
		CString GetEnhancedDesc (void);
		bool IsDamaged (void) const { return (m_pItem ? m_pItem->IsDamaged() : false); }
		bool IsDisrupted (void) const { return (m_pItem ? m_pItem->IsDisrupted() : false); }
		bool IsEnhanced (void) const { return (m_pItem ? m_pItem->IsEnhanced() : false); }

		static void PaintDevicePos (const SDeviceDesc &Device, CG32bitImage &Dest, int x, int y, int iScale, int iRotation);

	private:
		const CItemEnhancement &GetMods (void) const { return (m_pItem ? m_pItem->GetMods() : CItem::GetNullMod()); }

		CString m_sID;								//	ID for this slot (may match ID in class slot desc)
		CSpaceObject *m_pSource = NULL;				//	Installed on this object
		CItem *m_pItem = NULL;						//	Item installed in this slot
		CDeviceClassRef m_pClass;					//	The device class that is installed here
		COverlay *m_pOverlay = NULL;				//	Overlay (if associated)
		DWORD m_dwTargetID = 0;						//	ObjID of target (for tracking secondary weapons)
		CEnhancementDesc m_SlotEnhancements;		//	Enhancements conferred by the slot
		TSharedPtr<CItemEnhancementStack> m_pEnhancements;	//	List of enhancements (may be NULL)
		TArray<DWORD> m_LastShotIDs;				//	ObjID of last shots (only for continuous beams)

		DWORD m_dwData = 0;							//	Data specific to device class

		int m_iDeviceSlot:16 = -1;					//	Device slot

		int m_iPosAngle:16 = 0;						//	Position of installation (degrees)
		int m_iPosRadius:16 = 0;					//	Position of installation (pixels)
		int m_iPosZ:16 = 0;							//	Position of installation (height)
		int m_iMinFireArc:16 = 0;					//	Min angle of fire arc (degrees)
		int m_iMaxFireArc:16 = 0;					//	Max angle of fire arc (degrees)

		int m_iShotSeparationScale:16 = 32767;		//	Scaled by 32767. Governs scaling of shot separation for dual etc weapons
		int m_iMaxFireRange:16 = 0;					//	Max effective fire range (in light-seconds); 0 = no limit

		int m_iTimeUntilReady:16 = 0;				//	Timer counting down until ready to activate
		int m_iFireAngle:16 = 0;					//	Last fire angle

		int m_iTemperature:16 = 0;					//	Temperature for weapons
		int m_iActivateDelay:16 = 0;				//	Cached activation delay
		int m_iExtraPowerUse:16 = 0;				//	Additional power use per tick
		int m_iSlotPosIndex:16 = -1;				//	Slot placement

		DWORD m_fOmniDirectional:1 = false;			//	Installed on turret
		DWORD m_fOverdrive:1 = false;				//	Device has overdrive installed
		DWORD m_fOptimized:1 = false;				//	Device is optimized by alien engineers
		DWORD m_fSecondaryWeapon:1 = false;			//	Secondary weapon
		DWORD m_fEnabled:1 = false;					//	Device is enabled
		DWORD m_fExternal:1 = false;				//	Device is external to hull
		mutable DWORD m_fWaiting:1 = false;			//	Waiting for cooldown, etc.
		DWORD m_fTriggered:1 = false;				//	Device trigger is down (e.g., weapon is firing)

		DWORD m_fRegenerating:1 = false;			//	TRUE if we regenerated on the last tick
		DWORD m_fLastActivateSuccessful:1 = false;	//	TRUE if we successfully fired (last time we tried)
		DWORD m_f3DPosition:1 = false;				//	If TRUE we use m_iPosZ to compute position
		DWORD m_fLinkedFireAlways:1 = false;		//	If TRUE, lkfAlways
		DWORD m_fLinkedFireTarget:1 = false;		//	If TRUE, lkfTarget
		DWORD m_fLinkedFireEnemy:1 = false;			//	If TRUE, lkfEnemy
		DWORD m_fDuplicate:1 = false;				//	If TRUE, we have multiple version of the same item type installed
		DWORD m_fCannotBeEmpty:1 = false;			//	If TRUE, slot must always have a device

		DWORD m_fFateSurvives:1 = false;			//	Always survives when ship destroyed
		DWORD m_fFateDamaged:1 = false;				//	Always damaged when ship destroyed
		DWORD m_fFateDestroyed:1 = false;			//	Always destroyed when ship destroyed
		DWORD m_fFateComponetized:1 = false;		//	Always break into components when ship destroyed
		DWORD m_fLinkedFireSelected:1 = false;		//	If TRUE, lkfSelected
		DWORD m_fLinkedFireNever:1 = false;			//	If TRUE, lkfNever
		DWORD m_fLinkedFireSelectedVariants:1 = false;//  If TRUE, lkfSelectedVariant
		DWORD m_fCycleFire:1 = false;				//	If TRUE, then cycle fire through weapons of same type and linked fire settings

		DWORD m_fCanTargetMissiles:1 = false;		//	If TRUE, then this weapon can fire at hostile missiles as well as ships
		DWORD m_fOnSegment:1 = false;				//	If TRUE, then device logically belongs to the segment specified by m_sID.
		DWORD m_fOnUsedLastAmmo:1 = false;			//	If TRUE, remember the send statusUsedLastAmmo when done firing

		DWORD m_dwSpare2:5;
	};
