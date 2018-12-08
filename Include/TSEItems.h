//	TSEItems.h
//
//	Defines classes and interfaces for items
//	Copyright (c) 2016 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CInstalledArmor;
class CInstalledDevice;
class CItemList;
class CShipClass;

//	Item Types

enum ItemEnhancementTypes
	{
	etNone =							0x0000,
	etBinaryEnhancement =				0x0001,
	etLoseEnhancement =					0x0002,	//	Lose enhancements

	etStrengthen =						0x0100,	//	+hp, data1 = %increase (10% increments)
	etRegenerate =						0x0200,	//	data1 = rate
	etReflect =							0x0300,	//	data2 = damage type reflected
	etRepairOnHit =						0x0400,	//	repair damage on hit, data2 = damage type of hit
	etResist =							0x0500,	//	-damage, data1 = %damage adj
	etResistEnergy =					0x0600,	//	-energy damage, data1 = %damage adj (90%, 80%, etc)
	etResistMatter =					0x0700,	//	-matter damage, data1 = %damage adj (90%, 80%, etc)
	etResistByLevel =					0x0800,	//	-damage, data1 = %damage adj, data2 = damage type
	etResistByDamage =					0x0900,	//	-damage, data1 = %damage adj, data2 = damage type
	etResistByDamage2 =					0x0a00,	//	-damage, data1 = %damage adj, data2 = damage type
	etSpecialDamage =					0x0b00,	//	Immunity to damage effects:
												//		data2 = 0: immune to radiation
												//		data2 = 1: immune to blinding
												//		... see SpecialDamageTypes
	etImmunityIonEffects =				0x0c00,	//	Immunity to ion effects (blinding, EMP, etc.)
												//		(if disadvantage, interferes with shields)
	etPhotoRegenerate =					0x0d00,	//	regen near sun
	etPhotoRecharge =					0x0e00,	//	refuel near sun
	etPowerEfficiency =					0x0f00,	//	power usage decrease, 01 = 90%/110%, 02 = 80%/120%
	etSpeedOld =						0x1000,	//	decrease cycle time
	etTurret =							0x1100,	//	weapon turret, data1 is angle
	etMultiShot =						0x1200,	//	multiple shots, data2 = count, data1 = %weakening
	etSpeed =							0x1300,	//	decrease cycle time
												//		A = adj (0-100)
												//			[if disavantage] adj by 5% starting at 100%.
												//
												//		B = min delay in ticks (do not decrease below this; 0 = no limit)
												//		C = max delay in ticks (do not increase above this; 0 = no limit)
	etConferSpecialDamage =				0x1400,	//	weapon gains special damage
												//		A = SpecialDamageTypes
												//		B = damage level
	etHPBonus =							0x1500,	//	+hp%, like etStrengthen
												//		X = %increase
	etHealerRegenerate =				0x1600,	//	Same as etRegenerate, but uses healer
	etResistHPBonus =					0x1700,	//	resist damage
												//		B = damage type
												//		X = %bonus (if disadvantage, this is decrease)
	etTracking =						0x1800,	//	weapon gains tracking
												//		X = maneuver rate
	etOmnidirectional =					0x1900,	//	weapon gain omnidirectional
												//		X = 0: omni; >0 <360 : swivel

	etData1Mask =						0x000f,	//	4-bits of data (generally for damage adj)
	etData2Mask =						0x00f0,	//	4-bits of data (generally for damage type)
	etTypeMask =						0x7f00,	//	Type
	etDisadvantage =					0x8000,	//	If set, this is a disadvantage

	etDataAMask =					0x000000ff,	//	8-bits of data
	etDataAMax =						   255,
	etDataBMask =					0x00ff0000,	//	8-bits of data
	etDataBMax =						   255,
	etDataCMask =					0xff000000,	//	8-bits of data
	etDataCMax =						   255,
	etDataXMask =					0xffff0000,	//	16-bits of data
	etDataXMax =						 65535,
	};

enum EnhanceItemStatus
	{
	eisOK,										//	Enhancement OK
	eisNoEffect,								//	Nothing happens
	eisAlreadyEnhanced,							//	Already has this exact enhancement
	eisWorse,									//	A disadvantage was made worse
	eisRepaired,								//	Disadvantage was repaired
	eisEnhancementRemoved,						//	Enhancement removed
	eisEnhancementReplaced,						//	Enhancement replaced by another enhancement
	eisBetter,									//	Enhancement made better
	};

enum ERegenTypes
	{
	regenNone						= 0,

	regenStandard					= 1,	//	Standard regeneration
	regenSolar						= 2,	//	Regenerate when near a star
	regenFromShields				= 3,	//	Regenerate by draining shields
	regenFromHealer					= 4,	//	Consume healer to regenerate
	regenFromCharges				= 5,	//	Consume charges to regenerate
	};

class CItemEnhancement
	{
	public:
		CItemEnhancement (void) : m_dwID(OBJID_NULL), m_dwMods(0), m_pEnhancer(NULL), m_iExpireTime(-1) { }
		CItemEnhancement (DWORD dwMods) : m_dwID(OBJID_NULL), m_dwMods(dwMods), m_pEnhancer(NULL), m_iExpireTime(-1) { }

		void AccumulateAttributes (CItemCtx &Ctx, TArray<SDisplayAttribute> *retList) const;
		inline DWORD AsDWORD (void) const { return m_dwMods; }
		EnhanceItemStatus Combine (const CItem &Item, CItemEnhancement Enhancement);
		int GetAbsorbAdj (const DamageDesc &Damage) const;
		int GetActivateRateAdj (int *retiMinDelay = NULL, int *retiMaxDelay = NULL) const;
		int GetDamageAdj (void) const;
		int GetDamageAdj (const DamageDesc &Damage) const;
		DamageTypes GetDamageType (void) const;
		inline int GetDataA (void) const { return (int)(DWORD)(m_dwMods & etDataAMask); }
		inline int GetDataB (void) const { return (int)(DWORD)((m_dwMods & etDataBMask) >> 16); }
		inline int GetDataC (void) const { return (int)(DWORD)((m_dwMods & etDataCMask) >> 24); }
		inline int GetDataX (void) const { return (int)(DWORD)((m_dwMods & etDataXMask) >> 16); }
		int GetEnhancedRate (int iRate) const;
		inline CItemType *GetEnhancementType (void) const { return m_pEnhancer; }
		inline int GetExpireTime (void) const { return m_iExpireTime; }
		int GetFireArc (void) const;
		int GetHPAdj (void) const;
		int GetHPBonus (void) const;
		inline DWORD GetID (void) const { return m_dwID; }
		inline int GetLevel (void) const { return (int)(DWORD)(m_dwMods & etData1Mask); }
		inline int GetLevel2 (void) const { return (int)(DWORD)((m_dwMods & etData2Mask) >> 4); }
		int GetManeuverRate (void) const;
		inline DWORD GetModCode (void) const { return m_dwMods; }
		int GetPowerAdj (void) const;
		int GetReflectChance (DamageTypes iDamage) const;
		Metric GetRegen180 (CItemCtx &Ctx, int iTicksPerUpdate) const;
		int GetResistEnergyAdj (void) const { return (GetType() == etResistEnergy ? Level2DamageAdj(GetLevel(), IsDisadvantage()) : 100); }
		int GetResistHPBonus (void) const;
		int GetResistMatterAdj (void) const { return (GetType() == etResistMatter ? Level2DamageAdj(GetLevel(), IsDisadvantage()) : 100); }
		SpecialDamageTypes GetSpecialDamage (int *retiLevel = NULL) const;
		inline ItemEnhancementTypes GetType (void) const { return (ItemEnhancementTypes)(m_dwMods & etTypeMask); }
		int GetValueAdj (const CItem &Item) const;
		bool HasCustomDamageAdj (void) const;
		ALERROR InitFromDesc (ICCItem *pItem, CString *retsError);
		ALERROR InitFromDesc (const CString &sDesc, CString *retsError);
		ALERROR InitFromDesc (SDesignLoadCtx &Ctx, const CString &sDesc);
		inline bool IsBlindingImmune (void) const { return IsIonEffectImmune() || ((GetType() == etSpecialDamage) && GetLevel2() == specialBlinding && !IsDisadvantage()); }
		inline bool IsDecaying (void) const { return ((GetType() == etRegenerate) && IsDisadvantage()); }
		inline bool IsDeviceDamageImmune (void) const { return IsIonEffectImmune() || ((GetType() == etSpecialDamage) && GetLevel2() == specialDeviceDamage && !IsDisadvantage()); }
		inline bool IsDisadvantage (void) const { return ((m_dwMods & etDisadvantage) ? true : false); }
		inline bool IsDisintegrationImmune (void) const { return ((GetType() == etSpecialDamage) && GetLevel2() == specialDisintegration && !IsDisadvantage()); }
		inline bool IsEMPImmune (void) const { return IsIonEffectImmune() || ((GetType() == etSpecialDamage) && GetLevel2() == specialEMP && !IsDisadvantage()); }
		inline bool IsEmpty (void) const { return (m_dwMods == 0 && m_pEnhancer == NULL); }
		inline bool IsEnhancement (void) const { return (m_dwMods && !IsDisadvantage()); }
		bool IsEqual (const CItemEnhancement &Comp) const;
		inline bool IsNotEmpty (void) const { return !IsEmpty(); }
		inline bool IsPhotoRecharge (void) const { return ((GetType() == etPhotoRecharge) && !IsDisadvantage()); }
		inline bool IsPhotoRegenerating (void) const { return ((GetType() == etPhotoRegenerate) && !IsDisadvantage()); }
		inline bool IsRadiationImmune (void) const { return ((GetType() == etSpecialDamage) && GetLevel2() == 0 && !IsDisadvantage()); }
		inline bool IsReflective (void) const { return ((GetType() == etReflect) && !IsDisadvantage()); }
		bool IsReflective (const DamageDesc &Damage, int *retiReflectChance = NULL) const;
		inline bool IsRepairOnDamage (void) const { return ((GetType() == etRepairOnHit) && !IsDisadvantage()); }
		inline bool IsShatterImmune (void) const { return ((GetType() == etSpecialDamage) && GetLevel2() == specialShatter && !IsDisadvantage()); }
		inline bool IsShieldInterfering (void) const { return ((GetType() == etImmunityIonEffects) && IsDisadvantage()); }
		inline bool IsStacking (void) const { return (GetType() == etStrengthen && GetLevel() == 0); }
		void ReadFromStream (DWORD dwVersion, IReadStream *pStream);
		void ReadFromStream (SLoadCtx &Ctx);
		inline void SetEnhancementType (CItemType *pType) { m_pEnhancer = pType; }
		inline void SetExpireTime (int iTime) { m_iExpireTime = iTime; }
		inline void SetID (DWORD dwID) { m_dwID = dwID; }
		void SetModBonus (int iBonus);
		inline void SetModCode (DWORD dwMods) { m_dwMods = dwMods; }
		inline void SetModImmunity (SpecialDamageTypes iSpecial) { m_dwMods = Encode12(etSpecialDamage, 0, (int)iSpecial); }
		inline void SetModEfficiency (int iAdj) { m_dwMods = (iAdj > 0 ? EncodeABC(etPowerEfficiency, iAdj) : EncodeABC(etPowerEfficiency | etDisadvantage, -iAdj)); }
		inline void SetModOmnidirectional (int iFireArc) { m_dwMods = EncodeAX(etOmnidirectional, 0, Max(0, iFireArc)); }
		inline void SetModReflect (DamageTypes iDamageType) { m_dwMods = Encode12(etReflect, 0, (int)iDamageType); }
		inline void SetModResistDamage (DamageTypes iDamageType, int iAdj) { m_dwMods = Encode12(etResistByDamage | (iAdj > 100 ? etDisadvantage : 0), DamageAdj2Level(iAdj), (int)iDamageType); }
		inline void SetModResistDamageClass (DamageTypes iDamageType, int iAdj) { m_dwMods = Encode12(etResistByDamage2 | (iAdj > 100 ? etDisadvantage : 0), DamageAdj2Level(iAdj), (int)iDamageType); }
		inline void SetModResistDamageTier (DamageTypes iDamageType, int iAdj) { m_dwMods = Encode12(etResistByLevel | (iAdj > 100 ? etDisadvantage : 0), DamageAdj2Level(iAdj), (int)iDamageType); }
		inline void SetModResistEnergy (int iAdj) { m_dwMods = Encode12(etResistEnergy | (iAdj > 100 ? etDisadvantage : 0), DamageAdj2Level(iAdj)); }
		inline void SetModResistHPBonus (DamageTypes iDamageType, int iBonus) { m_dwMods = EncodeDX(etResistHPBonus | (iBonus < 0 ? etDisadvantage : 0), iDamageType, Absolute(iBonus)); }
		inline void SetModResistMatter (int iAdj) { m_dwMods = Encode12(etResistMatter | (iAdj > 100 ? etDisadvantage : 0), DamageAdj2Level(iAdj)); }
		void SetModSpecialDamage (SpecialDamageTypes iSpecial, int iLevel = 0);
		void SetModSpeed (int iAdj, int iMinDelay = 0, int iMaxDelay = 0);
		void SetModTracking (int iManeuverRate);
		bool UpdateArmorRegen (CItemCtx &ArmorCtx, SUpdateCtx &UpdateCtx, int iTick) const;
		void WriteToStream (IWriteStream *pStream) const;

		static const CItemEnhancement &Null (void) { return m_Null; }

	private:
		bool CalcRegen (CItemCtx &ItemCtx, int iTicksPerUpdate, CRegenDesc &retRegen, ERegenTypes *retiType = NULL) const;
		bool CalcNewHPBonus (const CItem &Item, const CItemEnhancement &NewEnhancement, int *retiNewBonus) const;
		bool CanBeCombinedWith (const CItemEnhancement &NewEnhancement) const;
		EnhanceItemStatus CombineAdvantageWithAdvantage (const CItem &Item, CItemEnhancement Enhancement);
		EnhanceItemStatus CombineAdvantageWithDisadvantage (const CItem &Item, CItemEnhancement Enhancement);
		EnhanceItemStatus CombineDisadvantageWithDisadvantage (const CItem &Item, CItemEnhancement Enhancement);
		EnhanceItemStatus CombineDisadvantageWithAdvantage (const CItem &Item, CItemEnhancement Enhancement);
		inline DamageTypes GetDamageTypeField (void) const { return (DamageTypes)(DWORD)((m_dwMods & etData2Mask) >> 4); }
		inline bool IsIonEffectImmune (void) const { return ((GetType() == etImmunityIonEffects) && !IsDisadvantage()); }

		static int DamageAdj2Level (int iDamageAdj);
		static DWORD EncodeABC (DWORD dwTypeCode, int A = 0, int B = 0, int C = 0);
		static DWORD EncodeAX (DWORD dwTypeCode, int A = 0, int X = 0);
		static DWORD EncodeDX (DWORD dwTypeCode, DamageTypes iDamageType, int X = 0);
		static DWORD Encode12 (DWORD dwTypeCode, int Data1 = 0, int Data2 = 0);
		static int Level2Bonus (int iLevel, bool bDisadvantage = false);
		static int Level2DamageAdj (int iLevel, bool bDisadvantage = false);

		DWORD m_dwID;							//	Global ID
		DWORD m_dwMods;							//	Mod code
		CItemType *m_pEnhancer;					//	Item that added this mod (may be NULL)
		int m_iExpireTime;						//	Universe tick when mod expires (-1 == no expiration)

		static CItemEnhancement m_Null;
	};

class CItemEnhancementStack
	{
	public:
		CItemEnhancementStack (void) :
				m_bCacheValid(false),
				m_dwRefCount(1)
			{ }

		void AccumulateAttributes (CItemCtx &Ctx, TArray<SDisplayAttribute> *retList) const;
		inline CItemEnhancementStack *AddRef (void) { m_dwRefCount++; return this; }
		int ApplyDamageAdj (const DamageDesc &Damage, int iDamageAdj) const;
		void ApplySpecialDamage (DamageDesc *pDamage) const;
		int CalcActivateDelay (CItemCtx &DeviceCtx) const;
		Metric CalcRegen180 (CItemCtx &ItemCtx, int iTicksPerUpdate) const;
		void Delete (void);
		int GetAbsorbAdj (const DamageDesc &Damage) const;
		int GetActivateDelayAdj (void) const;
		int GetBonus (void) const;
		inline int GetCount (void) const { return m_Stack.GetCount(); }
		const DamageDesc &GetDamage (void) const;
		int GetDamageAdj (const DamageDesc &Damage) const;
		int GetFireArc (void) const;
		int GetManeuverRate (void) const;
		int GetPowerAdj (void) const;
		int GetResistDamageAdj (DamageTypes iDamage) const;
		int GetResistEnergyAdj (void) const;
		int GetResistMatterAdj (void) const;
		void Insert (const CItemEnhancement &Mods);
		void InsertActivateAdj (int iAdj, int iMin, int iMax);
		void InsertHPBonus (int iBonus);
		inline bool IsEmpty (void) const { return (m_Stack.GetCount() == 0); }
		bool IsBlindingImmune (void) const;
		bool IsDecaying (void) const;
		bool IsDeviceDamageImmune (void) const;
		bool IsDisintegrationImmune (void) const;
		bool IsEMPImmune (void) const;
		bool IsPhotoRegenerating (void) const;
		bool IsPhotoRecharging (void) const;
		bool IsRadiationImmune (void) const;
		bool IsShatterImmune (void) const;
		bool IsShieldInterfering (void) const;
		inline bool IsTracking (void) const { return (GetManeuverRate() > 0); }
		bool ReflectsDamage (DamageTypes iDamage, int *retiChance = NULL) const;
		bool RepairOnDamage (DamageTypes iDamage) const;
		bool UpdateArmorRegen (CItemCtx &ArmorCtx, SUpdateCtx &UpdateCtx, int iTick) const;

		static TSharedPtr<CItemEnhancementStack> ReadFromStream (SLoadCtx &Ctx);
		static void WriteToStream (CItemEnhancementStack *pStack, IWriteStream *pStream);

	private:
		~CItemEnhancementStack (void) { }
		void CalcCache (void) const;

		TArray<CItemEnhancement> m_Stack;

		mutable bool m_bCacheValid;				//	If TRUE, these cache values are OK.
		mutable int m_iBonus;					//	Cached bonus
		mutable DamageDesc m_Damage;			//	Cached damage descriptor

		DWORD m_dwRefCount;
	};

class CRandomEnhancementGenerator
	{
	public:
		CRandomEnhancementGenerator (void) : m_iChance(0), m_pCode(NULL) { }
		~CRandomEnhancementGenerator (void);

		CRandomEnhancementGenerator &operator= (const CRandomEnhancementGenerator &Src);

		void EnhanceItem (CItem &Item) const;
		inline int GetChance (void) const { return m_iChance; }
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		bool IsVariant (void) const;

	private:
		int m_iChance;
		CItemEnhancement m_Mods;
		ICCItem *m_pCode;
	};

class CItem
	{
	public:
		enum EFlags
			{
			//	IsEqual

			FLAG_IGNORE_INSTALLED =			0x00000001,
			FLAG_IGNORE_CHARGES =			0x00000002,
			FLAG_IGNORE_DATA =				0x00000004,
			FLAG_IGNORE_DISRUPTED =			0x00000008,
			FLAG_IGNORE_ENHANCEMENTS =		0x00000010,
			};

		CItem (void);
		CItem (const CItem &Copy);
		CItem (CItemType *pItemType, int iCount);
		~CItem (void);
		CItem &operator= (const CItem &Copy);

		DWORD AddEnhancement (const CItemEnhancement &Enhancement);
		CString CalcSortKey (void) const;
		bool CanBeUsed (CItemCtx &ItemCtx, CString *retsUseKey = NULL) const;
		inline void ClearDamaged (void) { m_dwFlags &= ~flagDamaged; }
		inline void ClearDisrupted (void) { if (m_pExtra) m_pExtra->m_dwDisruptedTime = 0; }
		inline void ClearEnhanced (void) { m_dwFlags &= ~flagEnhanced; }
		static CItem CreateItemByName (const CString &sName, const CItemCriteria &Criteria, bool bActualName = false);
		inline bool IsArmor (void) const;
		inline bool IsDevice (void) const;
		bool IsEqual (const CItem &Item, DWORD dwFlags = 0) const;
		bool IsUsed (void) const;
		bool FireCanBeInstalled (CSpaceObject *pSource, int iSlot, CString *retsError) const;
		bool FireCanBeUninstalled (CSpaceObject *pSource, CString *retsError) const;
		void FireCustomEvent (CItemCtx &ItemCtx, const CString &sEvent, ICCItem *pData, ICCItem **retpResult) const;
		void FireOnAddedAsEnhancement (CSpaceObject *pSource, const CItem &ItemEnhanced, EnhanceItemStatus iStatus) const;
		bool FireOnDestroyCheck (CItemCtx &ItemCtx, DestructionTypes iCause, const CDamageSource &Attacker) const;
		void FireOnDisabled (CSpaceObject *pSource) const;
		void FireOnDocked (CSpaceObject *pSource, CSpaceObject *pDockedAt) const;
		void FireOnEnabled (CSpaceObject *pSource) const;
		void FireOnInstall (CSpaceObject *pSource) const;
		void FireOnObjDestroyed (CSpaceObject *pSource, const SDestroyCtx &Ctx) const;
		bool FireOnReactorOverload (CSpaceObject *pSource) const;
		void FireOnRemovedAsEnhancement (CSpaceObject *pSource, const CItem &ItemEnhanced) const;
		void FireOnUninstall (CSpaceObject *pSource) const;
		inline int GetCharges (void) const { return (m_pExtra ? (int)m_pExtra->m_dwCharges : 0); }
		inline int GetCount (void) const { return (int)m_dwCount; }
		const CItemList &GetComponents (void) const;
		inline CEconomyType *GetCurrencyType (void) const;
//		inline CString GetData (const CString &sAttrib) const { return (m_pExtra ? m_pExtra->m_Data.GetData(sAttrib) : NULL_STR); }
		ICCItemPtr GetDataAsItem (const CString &sAttrib) const;
		CString GetDesc (CItemCtx &ItemCtx, bool bActual = false) const;
		bool GetDisplayAttributes (CItemCtx &Ctx, TArray<SDisplayAttribute> *retList, ICCItem *pData = NULL, bool bActual = false) const;
		DWORD GetDisruptedDuration (void) const;
		bool GetDisruptedStatus (DWORD *retdwTimeLeft = NULL, bool *retbRepairedEvent = NULL) const;
		CString GetEnhancedDesc (CSpaceObject *pInstalled = NULL) const;
		inline int GetInstalled (void) const { return (int)(char)m_dwInstalled; }
		ICCItem *GetItemProperty (CCodeChainCtx &CCCtx, CItemCtx &Ctx, const CString &sProperty) const;
		Metric GetItemPropertyDouble (CCodeChainCtx &CCCtx, CItemCtx &Ctx, const CString &sProperty) const;
		int GetItemPropertyInteger (CCodeChainCtx &CCCtx, CItemCtx &Ctx, const CString &sProperty) const;
		CString GetItemPropertyString (CCodeChainCtx &CCCtx, CItemCtx &Ctx, const CString &sProperty) const;
        inline int GetLevel (void) const;
		inline Metric GetMass (void) const { return GetMassKg() / 1000.0; }
		int GetMassKg (void) const;
		int GetMaxCharges (void) const;
		inline const CItemEnhancement &GetMods (void) const { return (m_pExtra ? m_pExtra->m_Mods : m_NullMod); }
		static const CItem &GetNullItem (void) { return m_NullItem; }
		static const CItemEnhancement &GetNullMod (void) { return m_NullMod; }
		CString GetReference (CItemCtx &ItemCtx, const CItem &Ammo = CItem(), DWORD dwFlags = 0) const;
		bool GetReferenceDamageAdj (CSpaceObject *pInstalled, DWORD dwFlags, int *retiHP, int *retArray) const;
		bool GetReferenceDamageType (CItemCtx &Ctx, const CItem &Ammo, DWORD dwFlags, DamageTypes *retiDamage, CString *retsReference) const;
		bool GetReferenceSpeedBonus (CItemCtx &Ctx, DWORD dwFlags, int *retiSpeedBonus) const;
		CString GetNounPhrase (CItemCtx &Ctx, DWORD dwFlags = 0) const;
		inline int GetRawPrice (bool bActual = false) const { return GetValue(bActual); }
		int GetTradePrice (CSpaceObject *pObj = NULL, bool bActual = false) const;
		inline CItemType *GetType (void) const { return m_pItemType; }
        int GetVariantHigh (void) const { return (m_pExtra ? (int)HIWORD(m_pExtra->m_dwVariant) : 0); }
        int GetVariantLow (void) const { return (m_pExtra ? (int)LOWORD(m_pExtra->m_dwVariant) : 0); }
		bool HasComponents (void) const;
		inline bool HasMods (void) const { return (m_pExtra && m_pExtra->m_Mods.IsNotEmpty()); }
		bool HasSpecialAttribute (const CString &sAttrib) const;
		bool HasUseItemScreen (void) const;
		inline bool IsDamaged (void) const { return (m_dwFlags & flagDamaged ? true : false); }
		inline bool IsDisrupted (void) const;
		inline bool IsDisrupted (DWORD dwNow) const { return (m_pExtra ? (m_pExtra->m_dwDisruptedTime >= dwNow) : false); }
        inline bool IsEmpty (void) const { return (m_pItemType == NULL); }
		inline bool IsEnhanced (void) const { return (m_dwFlags & flagEnhanced ? true : false); }
		inline bool IsInstalled (void) const { return (m_dwInstalled != 0xff); }
		inline bool IsMarkedForDelete (void) { return (m_dwCount == 0xffff); }
		bool IsVirtual (void) const;
		inline void MarkForDelete (void) { m_dwCount = 0xffff; }
		bool RemoveEnhancement (DWORD dwID);
		void SetCharges (int iCharges);
		inline void SetCount (int iCount) { m_dwCount = (DWORD)iCount; }
		inline void SetDamaged (void) { m_dwFlags |= flagDamaged; }
		inline void SetDamaged (bool bDamaged) { ClearDamaged(); if (bDamaged) SetDamaged(); }
		inline void SetData (const CString &sAttrib, ICCItem *pData) { Extra(); m_pExtra->m_Data.SetData(sAttrib, pData); }
		void SetDisrupted (DWORD dwDuration);
		inline void SetEnhanced (void) { m_dwFlags |= flagEnhanced; }
		inline void SetEnhanced (bool bEnhanced) { ClearEnhanced(); if (bEnhanced) SetEnhanced(); }
		inline void SetInstalled (int iInstalled) { m_dwInstalled = (BYTE)(char)iInstalled; }
        bool SetLevel (int iLevel, CString *retsError = NULL);
		bool SetProperty (CItemCtx &Ctx, const CString &sName, ICCItem *pValue, CString *retsError);
        void SetVariantHigh (int iValue) { Extra(); m_pExtra->m_dwVariant = MAKELONG(LOWORD(m_pExtra->m_dwVariant), (WORD)(short)iValue); }
        void SetVariantLow (int iValue) { Extra(); m_pExtra->m_dwVariant = MAKELONG((WORD)(short)iValue, HIWORD(m_pExtra->m_dwVariant)); }

		static CString GenerateCriteria (const CItemCriteria &Criteria);
		static void InitCriteriaAll (CItemCriteria *retCriteria);
		static const CItem &NullItem (void) { return CItem::m_NullItem; }
		static void ParseCriteria (const CString &sCriteria, CItemCriteria *retCriteria);
		static DWORD ParseFlags (ICCItem *pItem);
		bool MatchesCriteria (const CItemCriteria &Criteria) const;

		void ReadFromStream (SLoadCtx &Ctx);
		void WriteToStream (IWriteStream *pStream);

		void ReadFromCCItem (CCodeChain &CC, ICCItem *pBuffer);
		ICCItem *WriteToCCItem (CCodeChain &CC) const;

	private:
		enum PFlags
			{
			flagDamaged =		0x01,			//	Item is damaged

			flagEnhanced =		0x10,			//	Item is enhanced (Mod 1)
			flagMod2 =			0x20,			//	Mod 2
			flagMod3 =			0x40,			//	Mod 3
			flagMod4 =			0x80,			//	Mod 4
			};

		struct SExtra
			{
			DWORD m_dwCharges;					//	Charges for items
			DWORD m_dwVariant;					//	Affects stats based on type (e.g., for armor, this is maxHP)
			DWORD m_dwDisruptedTime;			//	If >0, the tick on which disruption expires

			CItemEnhancement m_Mods;			//	Class-specific modifications (e.g., armor enhancements)

			CAttributeDataBlock m_Data;			//	Opaque data
			};

		void AccumulateCustomAttributes (CItemCtx &Ctx, TArray<SDisplayAttribute> *retList, ICCItem *pData) const;
		void Extra (void);
		int GetValue (bool bActual = false) const;
		bool IsExtraEqual (SExtra *pSrc, DWORD dwFlags) const;

		static bool IsDisruptionEqual (DWORD dwD1, DWORD dwD2);
		static bool IsExtraEmpty (const SExtra *pExtra, DWORD dwFlags);

		CItemType *m_pItemType;

		DWORD m_dwCount:16;						//	Number of items
		DWORD m_dwFlags:8;						//	Miscellaneous flags
		DWORD m_dwInstalled:8;					//	Location where item is installed

		SExtra *m_pExtra;						//	Extra data (may be NULL)

		static CItem m_NullItem;
		static CItemEnhancement m_NullMod;
	};

class CItemList
	{
	public:
		CItemList (void);
		CItemList (const CItemList &Src);
		~CItemList (void);

		CItemList &operator= (const CItemList &Src);

		void AddItem (const CItem &Item);
		void DeleteAll (void);
		void DeleteItem (int iIndex);
		inline int GetCount (void) const { return m_List.GetCount(); }
		inline CItem &GetItem (int iIndex) { return *m_List[iIndex]; }
		inline const CItem &GetItem (int iIndex) const { return *m_List[iIndex]; }
		void ReadFromStream (SLoadCtx &Ctx);
		void SortItems (void);
		void WriteToStream (IWriteStream *pStream);

		inline static CItemList &Null (void) { return m_Null; }

	private:
		void Copy (const CItemList &Src);

		TArray<CItem *> m_List;

		static CItemList m_Null;
	};

class CItemListManipulator
	{
	public:
		CItemListManipulator (CItemList &ItemList);
		~CItemListManipulator (void);

		bool AddDamagedComponents (const CItem &Item, int iDamageChance);
		void AddItem (const CItem &Item);
		bool AddItems (const CItemList &ItemList, int iChance = 100);

		inline bool FindItem (const CItem &Item, DWORD dwFlags, int *retiCursor) 
			{ int iCursor = FindItem(Item, dwFlags); if (iCursor == -1) return false; if (retiCursor) *retiCursor = iCursor; return true; }
		inline int GetCount (void) { return m_ViewMap.GetCount(); }
		inline int GetCursor (void) { return m_iCursor; }
		inline void SetCursor (int iCursor) { m_iCursor = Min(Max(-1, iCursor), GetCount() - 1); }
		bool SetCursorAtItem (const CItem &Item, DWORD dwFlags = 0);
		void SetFilter (const CItemCriteria &Filter);

		static constexpr DWORD FLAG_SORT_ITEMS = 0x00000001;
		bool Refresh (const CItem &Item, DWORD dwFlags = 0);

		inline bool IsCursorValid (void) const { return (m_iCursor != -1 && m_iCursor < m_ItemList.GetCount()); }
		bool MoveCursorBack (void);
		bool MoveCursorForward (void);
		void ResetCursor (void);
		void SyncCursor (void);

		DWORD AddItemEnhancementAtCursor (const CItemEnhancement &Mods, int iCount = 1);
		void ClearDisruptedAtCursor (int iCount = 1);
		void DeleteAtCursor (int iCount);
		void DeleteMarkedItems (void);
		const CItem &GetItemAtCursor (void);
		CItem *GetItemPointerAtCursor (void);
		bool IsItemPointerValid (const CItem *pItem) const;
		void MarkDeleteAtCursor (int iCount);
		void RemoveItemEnhancementAtCursor (DWORD dwID, int iCount = 1);
		void SetChargesAtCursor (int iCharges, int iCount = 1);
		void SetCountAtCursor (int iCount);
		void SetDamagedAtCursor (bool bDamaged, int iCount = 1);
		void SetDataAtCursor (const CString &sAttrib, ICCItem *pData, int iCount = 1);
		void SetDisruptedAtCursor (DWORD dwDuration, int iCount = 1);
		void SetEnhancedAtCursor (bool bEnhanced);
		void SetInstalledAtCursor (int iInstalled);
		bool SetPropertyAtCursor (CSpaceObject *pSource, const CString &sName, ICCItem *pValue, int iCount, CString *retsError);
		void TransferAtCursor (int iCount, CItemList &DestList);

	private:
		int FindItem (const CItem &Item, DWORD dwFlags = 0) const;
		void GenerateViewMap (void);
		void MoveItemTo (const CItem &NewItem, const CItem &OldItem);

		CItemList &m_ItemList;
		int m_iCursor;
		TArray<int> m_ViewMap;

		bool m_bUseFilter;
		CItemCriteria m_Filter;
	};

class CItemCtx
	{
	public:
        CItemCtx (CItemType *pItemType);
		CItemCtx (const CItem &Item) : m_pItem(&Item) { }
		CItemCtx (const CShipClass *pSource, const CItem &Item) : m_pSourceShipClass(pSource), m_pItem(&Item) { }
		CItemCtx (const CItem *pItem = NULL, CSpaceObject *pSource = NULL) : m_pItem(pItem), m_pSource(pSource) { }
		CItemCtx (const CItem *pItem, CSpaceObject *pSource, CInstalledArmor *pArmor) : m_pItem(pItem), m_pSource(pSource), m_pArmor(pArmor) { }
		CItemCtx (const CItem *pItem, CSpaceObject *pSource, CInstalledDevice *pDevice) : m_pItem(pItem), m_pSource(pSource), m_pDevice(pDevice) { }
		CItemCtx (CSpaceObject *pSource, CInstalledArmor *pArmor) : m_pSource(pSource), m_pArmor(pArmor) { }
		CItemCtx (CSpaceObject *pSource, CInstalledDevice *pDevice) : m_pSource(pSource), m_pDevice(pDevice) { }

		void ClearItemCache (void);
		ICCItem *CreateItemVariable (CCodeChain &CC);
		CInstalledArmor *GetArmor (void);
		CArmorClass *GetArmorClass (void);
		CInstalledDevice *GetDevice (void);
		CDeviceClass *GetDeviceClass (void);
		bool GetEnhancementDisplayAttributes (TArray<SDisplayAttribute> *retList);
		TSharedPtr<CItemEnhancementStack> GetEnhancementStack (void);
		const CItemEnhancementStack &GetEnhancements (void) { const CItemEnhancementStack *pStack = GetEnhancementStack(); if (pStack) return *pStack; else return *m_pNullEnhancements; }
		const CItem &GetItem (void);
		int GetItemCharges (void);
		const CItemEnhancement &GetMods (void);
		inline CSpaceObject *GetSource (void) { return m_pSource; }
		const CShipClass *GetSourceShipClass (void) const;
		inline int GetVariant (void) const { return m_iVariant; }
		inline CDeviceClass *GetVariantDevice (void) const { return m_pWeapon; }
        inline const CItem &GetVariantItem (void) const { return m_Variant; }
        inline bool IsItemNull (void) { GetItem(); return (m_pItem == NULL || m_pItem->GetType() == NULL); }
		bool IsDeviceDamaged (void);
		bool IsDeviceDisrupted (void);
        bool IsDeviceEnabled (void);
		bool IsDeviceWorking (void);
		bool ResolveVariant (void);
        inline void SetVariantItem (const CItem &Item) { m_Variant = Item; }

	private:
		const CItem *GetItemPointer (void);

		const CItem *m_pItem = NULL;			//	The item
		CItem m_Item;							//	A cached item, if we need to cons one up.
		CSpaceObject *m_pSource = NULL;			//	Where the item is installed (may be NULL)
		const CShipClass *m_pSourceShipClass = NULL;	//	For some ship class functions
		CInstalledArmor *m_pArmor = NULL;		//	Installation structure (may be NULL)
		CInstalledDevice *m_pDevice = NULL;		//	Installation structure (may be NULL)

        CItem m_Variant;                        //  Stores the selected missile/ammo for a weapon.
		CDeviceClass *m_pWeapon = NULL;			//	This is the weapon that uses the given item
		int m_iVariant = -1;					//	NOTE: In this case, m_pItem may be either a
												//	missile or the weapon.

		TSharedPtr<CItemEnhancementStack> m_pEnhancements;	//	Only used if we need to cons one up

		static TSharedPtr<CItemEnhancementStack> m_pNullEnhancements;
	};

//	IItemGenerator -------------------------------------------------------------

struct SItemAddCtx
	{
	SItemAddCtx (CItemListManipulator &theItemList) : 
			ItemList(theItemList)
		{ }

	CItemListManipulator &ItemList;				//	Item list to add items to

	CSystem *pSystem = NULL;					//	System where we're creating items
	CSpaceObject *pDest = NULL;					//	Object to add to (may be NULL)
	CVector vPos;								//	Position to use (for LocationCriteriaTable)
	int iLevel = 1;								//	Level to use for item create (for LevelTable)
	};

class CItemTypeProbabilityTable
	{
	public:
		void Add (CItemType *pType, Metric rProbability = 1.0);
		void Add (const CItemTypeProbabilityTable &Src);
		inline void DeleteAll (void) { m_Table.DeleteAll(); }
		inline int GetCount (void) const { return m_Table.GetCount(); }
		inline Metric GetProbability (int iIndex) const { return m_Table[iIndex]; }
		inline Metric GetProbability (CItemType *pType) const { Metric *pProb = m_Table.GetAt(pType); return (pProb ? *pProb : 0.0); }
		inline CItemType *GetType (int iIndex) const { return m_Table.GetKey(iIndex); }
		void Scale (Metric rProbability);
		void Union (CItemType *pType, Metric rProbability = 1.0);
		void Union (const CItemTypeProbabilityTable &Src);

	private:
		TSortMap<CItemType *, Metric> m_Table;
	};

class IItemGenerator
	{
	public:
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, IItemGenerator **retpGenerator);
		static ALERROR CreateLookupTable (SDesignLoadCtx &Ctx, DWORD dwUNID, IItemGenerator **retpGenerator);
		static ALERROR CreateRandomItemTable (const CItemCriteria &Crit, 
											  const CString &sLevelFrequency,
											  IItemGenerator **retpGenerator);

		virtual ~IItemGenerator (void) { }
		virtual void AddItems (SItemAddCtx &Ctx) { }
		virtual void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) { }
		virtual CurrencyValue GetAverageValue (int iLevel) { return 0; }
		virtual IItemGenerator *GetGenerator (int iIndex) { return NULL; }
		virtual int GetGeneratorCount (void) { return 0; }
		virtual CItemType *GetItemType (int iIndex) { return NULL; }
		virtual int GetItemTypeCount (void) { return 0; }
		virtual CItemTypeProbabilityTable GetProbabilityTable (SItemAddCtx &Ctx) const { return CItemTypeProbabilityTable(); }
		virtual bool HasItemAttribute (const CString &sAttrib) const { return false; }
		virtual ALERROR LoadFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) { return NOERROR; }
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) { return NOERROR; }
	};
