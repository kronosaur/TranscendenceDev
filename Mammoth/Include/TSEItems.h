//	TSEItems.h
//
//	Defines classes and interfaces for items
//	Copyright (c) 2016 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CInstalledArmor;
class CInstalledDevice;
class CItemList;
class CRepairerClass;
class CShipClass;

//	ITEM -----------------------------------------------------------------------
//
//	CDifferentiatedItem and its subclasses are used to access functionality
//	specific to armor, devices, etc.
//
//	WARNING: These classes are meant to be used as wrappers to an underlying
//	CItem object. The lifetime of a CDifferentiatedItem is tied to the CItem 
//	from which it came. Thus you cannot construct a CItem on the stack and 
//	return a CDifferentiatedItem (because you'll end up with a reference to an
//	invalid stack frame).
//
//	HACK: The pattern below does not deal with const properly. Consider using 
//	the following pattern:
//
//	#include <type_traits>
//	
//	class Foo;
//	
//	template<bool IsConst>
//	class FooWrapper {
//	    using reference_type = std::conditional_t<IsConst, const Foo&, Foo&>;
//	
//	    public:
//	    FooWrapper(reference_type foo) : foo(foo) {}
//	
//	    private:
//	    reference_type foo;
//	};
//	
//	template<typename T> FooWrapper(T& t) -> FooWrapper<std::is_const_v<T>>;
//	
//	void bar(const Foo& foo1, Foo& foo2)
//	{
//	    FooWrapper wrapper1(foo1); // FooWrapper<false>
//	    FooWrapper wrapper2(foo2); // FooWrapper<true>
//	}

class CDifferentiatedItem
	{
	public:
		inline ItemCategories GetCategory (void) const;
		inline int GetCharges (void) const;
		inline CCurrencyAndValue GetCurrencyAndValue (bool bActual = false) const;
		inline const CEconomyType &GetCurrencyType (void) const;
		inline const CObjectImageArray &GetImage (void) const;
		inline int GetLevel (void) const;
		inline int GetMassKg (void) const;
		inline int GetMinLevel (void) const;
		inline CString GetNounPhrase (DWORD dwFlags = 0) const;
		inline ICCItemPtr GetProperty (const CString &sProperty) const;
		inline const CItemType &GetType (void) const;
		inline CItemType &GetType (void);
		inline int GetVariantNumber (void) const;
		inline bool IsLauncher (void) const;
		inline bool IsWeapon (void) const;
		void ReportEventError (const CSpaceObject *pSource, const CString &sEvent, const ICCItem &ErrorItem) const;

	protected:
		CDifferentiatedItem (CItem &Item) :
				m_Item(Item)
			{ }

		//	NOTE: See comment above.

		CDifferentiatedItem (const CItem &Item) :
				m_Item(const_cast<CItem &>(Item))
			{ }

		CItem &m_Item;

		mutable TSharedPtr<CItemEnhancementStack> m_pEnhancements;	//	Only used if we need to cons one up

		static TSharedPtr<CItemEnhancementStack> m_pNullEnhancements;
	};

class CArmorItem : public CDifferentiatedItem
	{
	public:
		struct SBalance
			{
			Metric rBalance = 0.0;				//	Total balance (+100 = 100% overpowered)
			int iLevel = 0;						//	Level for which we balanced
			Metric rHP = 0.0;					//	Max HP for armor (counting bonuses, etc.).
			Metric rStdMass = 0.0;				//	Standard mass (balance = 0.0)
			Metric rStdCost = 0.0;				//	Standard cost for mass.
			Metric rStdHP = 0.0;				//	Standard HP for mass.

			Metric rHPBalance = 0.0;			//	Balance contribution from raw HP
			Metric rDamageAdj = 0.0;			//	Balance contribution from damage adj
			Metric rDamageEffectAdj = 0.0;		//	Balance contribution from damage effect adj (EMP-resist, etc.).
			Metric rRegen = 0.0;				//	Balance from regeneration/decay/distribution
			Metric rRepairAdj = 0.0;			//	Balance from repair level/cost
			Metric rArmorComplete = 0.0;		//	Balance from armor complete bonus
			Metric rStealth = 0.0;				//	Balance from stealth bonus
			Metric rPowerUse = 0.0;				//	Balance from power use
			Metric rSpeedAdj = 0.0;				//	Balance from speed bonus/penalty
			Metric rDeviceBonus = 0.0;			//	Balance from device bonus

			Metric rMass = 0.0;					//	Balance from mass
			Metric rCost = 0.0;					//	Balance from cost
			};

		inline explicit operator bool () const;
		operator const CItem & () const { return m_Item; }
		operator CItem & () { return m_Item; }

		void AccumulateAttributes (TArray<SDisplayAttribute> *retList) const;
		inline int CalcBalance (SBalance &retBalance) const;
		ICCItemPtr FindProperty (const CString &sProperty) const;
		inline const CArmorClass &GetArmorClass (void) const;
		inline CArmorClass &GetArmorClass (void);
		inline int GetDamageAdj (DamageTypes iDamage) const;
		inline int GetDamageAdj (const DamageDesc &Damage) const;
		inline int GetDamageEffectiveness (CSpaceObject *pAttacker, CInstalledDevice *pWeapon) const;
		inline const CItemEnhancementStack &GetEnhancements (void) const;
		int GetHP (int *retiMaxHP = NULL, bool bUninstalled = false) const;
		inline int GetInstallCost (void) const;
		inline const CInstalledArmor *GetInstalledArmor (void) const;
		inline int GetMaxHP (bool bForceComplete = false) const;
		inline bool GetReferenceDamageAdj (int *retiHP, int *retArray) const;
		inline CurrencyValue GetRepairCost (int iHPToRepair = 1) const;
		inline int GetRepairLevel (void) const;
		inline CSpaceObject *GetSource (void) const;
		inline bool IsImmune (SpecialDamageTypes iSpecialDamage) const;

	private:
		CArmorItem (CItem &Item) : CDifferentiatedItem(Item)
			{ }

		CArmorItem (const CItem &Item) : CDifferentiatedItem(Item)
			{ }

		TSharedPtr<CItemEnhancementStack> GetEnhancementStack (void) const;

	friend class CItem;
	};

class CDeviceItem : public CDifferentiatedItem
	{
	public:

		enum ECalcTargetTypes
			{
			calcNoTarget,
			calcControllerTarget,
			calcWeaponTarget,
			};

		inline explicit operator bool () const;
		operator const CItem & () const { return m_Item; }
		operator CItem & () { return m_Item; }

		void AccumulateAttributes (const CItem &Ammo, TArray<SDisplayAttribute> *retList) const;
		ECalcTargetTypes CalcTargetType (void) const;
		TArray<const CItemType *> GetConsumableTypes () const;
		int GetCyberDefenseLevel () const;
		inline const CDeviceClass &GetDeviceClass (void) const;
		inline CDeviceClass &GetDeviceClass (void);
		inline const CRepairerClass &GetDeviceClassRepairer () const;
		inline int GetDeviceSlot (void) const;
		inline const CItemEnhancementStack &GetEnhancements (void) const;
		int GetFireArc (void) const;
		int GetHP (int *retiMaxHP = NULL, bool bUninstalled = false) const;
		inline const CInstalledDevice *GetInstalledDevice (void) const;
		inline CInstalledDevice *GetInstalledDevice (void);
		DWORD GetLinkedFireOptions (void) const;
		Metric GetMaxEffectiveRange (CSpaceObject *pTarget = NULL) const;
		int GetMaxHP (void) const;
		inline CSpaceObject *GetSource (void) const;
		inline DWORD GetTargetTypes (void) const;
		DamageTypes GetWeaponDamageType (void) const;
		inline int GetWeaponEffectiveness (CSpaceObject *pTarget) const;
		inline const CWeaponFireDesc *GetWeaponFireDesc (void) const;
		inline const CWeaponFireDesc *GetWeaponFireDesc (const CItem &Ammo) const;
		inline const CWeaponFireDesc &GetWeaponFireDescForVariant (int iVariant) const;
		inline int GetWeaponVariantCount (void) const;
		inline bool IsAreaWeapon (void) const;
		inline bool IsEnabled (void) const;
		inline bool IsMiningWeapon (void) const;
		inline bool IsTrackingWeapon (void) const;
		bool IsWeaponAligned (CSpaceObject *pTarget, int *retiAimAngle = NULL, int *retiFireAngle = NULL) const;
		inline bool IsWeaponVariantValid (int iVariant) const;
		inline bool NeedsAutoTarget (int *retiMinFireArc = NULL, int *retiMaxFireArc = NULL) const;
		void ReportEventError (const CString &sEvent, const ICCItem &ErrorItem) const { CDifferentiatedItem::ReportEventError(GetSource(), sEvent, ErrorItem); }
		inline void SetData (DWORD dwData);

	private:
		CDeviceItem (CItem &Item) : CDifferentiatedItem(Item)
			{ }

		CDeviceItem (const CItem &Item): CDifferentiatedItem(Item)
			{ }

		TSharedPtr<CItemEnhancementStack> GetEnhancementStack (void) const;

	friend class CItem;
	};

class CMissileItem : public CDifferentiatedItem
	{
	public:
		
		inline explicit operator bool () const;
		operator const CItem & () const { return m_Item; }
		operator CItem & () { return m_Item; }

		TArray<CItem> GetLaunchWeapons (void) const;
		CString GetLaunchedByText (const CItem &Launcher) const;

	private:
		CMissileItem (CItem &Item) : CDifferentiatedItem(Item)
			{ }

		CMissileItem (const CItem &Item) : CDifferentiatedItem(Item)
			{ }

	friend class CItem;
	};

class CItem
	{
	public:
		//	IsEqual

		static constexpr DWORD FLAG_IGNORE_INSTALLED =			0x00000001;
		static constexpr DWORD FLAG_IGNORE_CHARGES =			0x00000002;
		static constexpr DWORD FLAG_IGNORE_DATA =				0x00000004;
		static constexpr DWORD FLAG_IGNORE_DISRUPTED =			0x00000008;
		static constexpr DWORD FLAG_IGNORE_ENHANCEMENTS =		0x00000010;

		static constexpr DWORD FLAG_FIND_MIN_CHARGES =			0x00010000;	//	Item with least number of charges
		static constexpr DWORD FLAG_ACTUAL =					0x00020000;	//	Ignore unknown status

		enum PFlags
			{
			flagDamaged =		0x01,			//	Item is damaged
			flagUnused1 =		0x02,
			flagUnused2 =		0x04,
			flagUnknownBit3 =	0x08,
			flagEnhanced =		0x10,			//	Item is enhanced (Mod 1)

			flagUnknownBit0 =	0x20,
			flagUnknownBit1 =	0x40,
			flagUnknownBit2 =	0x80,
			};

		struct SEnhanceItemResult
			{
			EnhanceItemStatus iResult = eisUnknown;
			CItemEnhancement Enhancement;
			CString sDesc;
			CString sNextScreen;
			bool bDoNotConsume = false;
			};

		CItem (void);
		CItem (const CItem &Copy);
		CItem (CItemType &ItemType, int iCount);
		CItem (CItemType *pItemType, int iCount);

		~CItem (void);
		CItem &operator= (const CItem &Copy);

		explicit operator bool () const { return (m_pItemType != NULL); }

		bool AccumulateEnhancementDisplayAttributes (TArray<SDisplayAttribute> &retList) const;
		DWORD AddEnhancement (const CItemEnhancement &Enhancement);
		CString CalcSortKey (void) const;
		bool CanBeUsed (CString *retsUseKey = NULL) const;
		void ClearDamaged (void);
		void ClearDisrupted (void) { if (m_pExtra) m_pExtra->m_dwDisruptedTime = 0; }
		void ClearEnhanced (void) { m_dwFlags &= ~flagEnhanced; }
		void ClearExtra (void) { delete m_pExtra; m_pExtra = NULL; }
		void ClearInstalled (void);
		static CItem CreateItemByName (CUniverse &Universe, const CString &sName, const CItemCriteria &Criteria, bool bActualName = false);
		bool IsEqual (const CItem &Item, DWORD dwFlags = 0) const;
		bool IsUsed (void) const;
		bool FireCanBeInstalled (CSpaceObject *pSource, int iSlot, CString *retsError) const;
		bool FireCanBeUninstalled (CSpaceObject *pSource, CString *retsError) const;
		bool FireCanEnhanceItem (const CSpaceObject &TargetObj, const CItem &TargetItem, SEnhanceItemResult &retResult, CString *retsError = NULL) const;
		void FireCustomEvent (CItemCtx &ItemCtx, const CString &sEvent, ICCItem *pData, ICCItem **retpResult) const;
		ICCItemPtr FireGetTradeServices (const CSpaceObject &SourceObj) const;
		void FireOnAddedAsEnhancement (CSpaceObject *pSource, const CItem &ItemEnhanced, EnhanceItemStatus iStatus) const;
		bool FireOnDestroyCheck (CItemCtx &ItemCtx, DestructionTypes iCause, const CDamageSource &Attacker) const;
		void FireOnDisabled (CSpaceObject *pSource) const;
		void FireOnDocked (CSpaceObject *pSource, CSpaceObject *pDockedAt) const;
		void FireOnEnabled (CSpaceObject *pSource) const;
		bool FireOnEnhanceItem (const CSpaceObject &TargetObj, const CItem &TargetItem, SEnhanceItemResult &retResult, CString *retsError = NULL) const;
		void FireOnInstall (CSpaceObject *pSource) const;
		void FireOnObjDestroyed (CSpaceObject *pSource, const SDestroyCtx &Ctx) const;
		bool FireOnReactorOverload (CSpaceObject *pSource) const;
		void FireOnRemovedAsEnhancement (CSpaceObject *pSource, const CItem &ItemEnhanced) const;
		void FireOnUninstall (CSpaceObject *pSource) const;
		int GetApparentLevel (void) const;
		inline ItemCategories GetCategory (void) const;
		int GetCharges (void) const { return (m_pExtra ? (int)m_pExtra->m_dwCharges : 0); }
		int GetCount (void) const { return (int)m_dwCount; }
		const CItemList &GetComponents (void) const;
		inline const CEconomyType *GetCurrencyType (void) const;
		int GetDamagedHP (void) const;
		ICCItemPtr GetDataAsItem (const CString &sAttrib) const;
		CString GetDesc (bool bActual = false) const;
		inline CDeviceClass *GetDeviceClass (void) const;
		bool GetDisplayAttributes (TArray<SDisplayAttribute> *retList, ICCItem *pData = NULL, bool bActual = false) const;
		DWORD GetDisruptedDuration (void) const;
		bool GetDisruptedStatus (DWORD *retdwTimeLeft = NULL, bool *retbRepairedEvent = NULL) const;
		CString GetEnhancedDesc (void) const;
		bool GetEnhancementConferred (const CSpaceObject &TargetObj, const CItem &TargetItem, const CString &sMode, SEnhanceItemResult &retResult, CString *retsError = NULL) const;
		TSharedPtr<CItemEnhancementStack> GetEnhancementStack (void) const;
		inline const CObjectImageArray &GetImage (void) const;
		int GetInstallCost (void) const;
		int GetInstalled (void) const { return (m_pExtra ? m_pExtra->m_iInstalledIndex : -1); }
		const CInstalledArmor *GetInstalledArmor (void) const { if (m_pExtra && m_pExtra->m_iInstalled == EInstalled::Armor) return (const CInstalledArmor *)m_pExtra->m_pInstalled; else return NULL; }
		CInstalledArmor *GetInstalledArmor (void) { if (m_pExtra && m_pExtra->m_iInstalled == EInstalled::Armor) return (CInstalledArmor *)m_pExtra->m_pInstalled; else return NULL; }
		const CInstalledDevice *GetInstalledDevice (void) const { if (m_pExtra && m_pExtra->m_iInstalled == EInstalled::Device) return (const CInstalledDevice *)m_pExtra->m_pInstalled; else return NULL; }
		CInstalledDevice *GetInstalledDevice (void) { if (m_pExtra && m_pExtra->m_iInstalled == EInstalled::Device) return (CInstalledDevice *)m_pExtra->m_pInstalled; else return NULL; }
		ICCItem *GetItemProperty (CCodeChainCtx &CCCtx, CItemCtx &Ctx, const CString &sProperty, bool bOnType) const;
		Metric GetItemPropertyDouble (CCodeChainCtx &CCCtx, CItemCtx &Ctx, const CString &sProperty) const;
		int GetItemPropertyInteger (CCodeChainCtx &CCCtx, CItemCtx &Ctx, const CString &sProperty) const;
		CString GetItemPropertyString (CCodeChainCtx &CCCtx, CItemCtx &Ctx, const CString &sProperty) const;
		int GetLevel (void) const;
		Metric GetMass (void) const { return GetMassKg() / 1000.0; }
		int GetMassKg (void) const;
		int GetMaxCharges (void) const;
		const CItemEnhancement &GetMods (void) const { return (m_pExtra ? m_pExtra->m_Mods : m_NullMod); }
		static const CItem &GetNullItem (void) { return m_NullItem; }
		static const CItemEnhancement &GetNullMod (void) { return m_NullMod; }
		CString GetReference (CItemCtx &ItemCtx, const CItem &Ammo = CItem(), DWORD dwFlags = 0) const;
		bool GetReferenceDamageAdj (CSpaceObject *pInstalled, DWORD dwFlags, int *retiHP, int *retArray) const;
		bool GetReferenceDamageType (CItemCtx &Ctx, const CItem &Ammo, DWORD dwFlags, DamageTypes *retiDamage, CString *retsReference) const;
		bool GetReferenceSpeedBonus (CItemCtx &Ctx, DWORD dwFlags, int *retiSpeedBonus) const;
		CSpaceObject *GetSource (void) const;
		CString GetNamePattern (DWORD dwNounFormFlags, DWORD *retdwFlags) const;
		CString GetNounPhrase (DWORD dwFlags = 0) const;
		int GetRawPrice (bool bActual = false) const { return GetValue(bActual); }
		int GetTradePrice (const CSpaceObject *pObj = NULL, bool bActual = false) const;
		CItemType *GetType (void) const { return m_pItemType; }
		CUniverse &GetUniverse (void) const;
		int GetUnknownIndex (void) const;
		CItemType *GetUnknownType (void) const;
		CItemType *GetUnknownTypeIfUnknown (bool bActual = false) const;
		int GetVariantNumber (void) const { return (m_pExtra ? (int)m_pExtra->m_dwVariantCounter : 0); }
		inline bool HasAttribute (const CString &sAttrib) const;
		bool HasComponents (void) const;
		bool HasMods (void) const { return (m_pExtra && m_pExtra->m_Mods.IsNotEmpty()); }
		bool HasSpecialAttribute (const CString &sAttrib) const;
		bool HasUseItemScreen (void) const;
		bool IsDamaged (int *retiDamagedHP = NULL) const;
		bool IsDisrupted (void) const;
		bool IsDisrupted (DWORD dwNow) const { return (m_pExtra ? (m_pExtra->m_dwDisruptedTime >= dwNow) : false); }
		bool IsEmpty (void) const { return (m_pItemType == NULL); }
		bool IsEnhanced (void) const { return (m_dwFlags & flagEnhanced ? true : false); }
		bool IsEnhancementEffective (const CItemEnhancement &Enhancement) const;
		bool IsExtraEmpty (DWORD dwFlags = 0);

		//	NOTE: We use installed index instead of m_iInstalled because during 
		//	load the index is set up, but the enum has not yet been initialized
		//	(because that happens later). In the future we could initialized 
		//	m_iInstalled at load time based on m_iInstalledIndex to some value
		//	that means "in the process of being fixed up". Note that we also 
		//	might want to use a similar technique for SetPreprareUninstalled.

		bool IsInstalled (void) const { return (m_pExtra && m_pExtra->m_iInstalledIndex != -1); }

		bool IsKnown (int *retiUnknownIndex = NULL) const;
		bool IsMarkedForDelete (void) { return (m_dwCount == 0xffff); }
		bool IsVirtual (void) const;
		void MarkForDelete (void) { m_dwCount = 0xffff; }
		bool RemoveEnhancement (DWORD dwID);
		void SetCharges (int iCharges);
		void SetCount (int iCount) { m_dwCount = (DWORD)iCount; }
		void SetDamaged (bool bDamaged = true) { ClearDamaged(); if (bDamaged) m_dwFlags |= flagDamaged; }
		void SetDamaged (int iDamagedHP);
		void SetData (const CString &sAttrib, ICCItem *pData) { Extra(); m_pExtra->m_Data.SetData(sAttrib, pData); }
		void SetDisrupted (DWORD dwDuration);
		void SetEnhanced (void) { m_dwFlags |= flagEnhanced; }
		void SetEnhanced (bool bEnhanced) { ClearEnhanced(); if (bEnhanced) SetEnhanced(); }
		void SetInstalled (CInstalledArmor &Installed);
		void SetInstalled (CInstalledDevice &Installed);
		void SetKnown (bool bKnown = true) const;
		bool SetLevel (int iLevel, CString *retsError = NULL);
		void SetPrepareUninstalled (void);
		ESetPropertyResult SetProperty (CItemCtx &Ctx, const CString &sName, const ICCItem *pValue, bool bOnType, CString *retsError = NULL);
		void SetUnknownIndex (int iIndex);
		void SetVariantNumber (int iVariantCounter);

		//	Conversion to Item Categories
		//
		//	When converting from a CItem to a CArmorItem there are a few common
		//	patterns:
		//
		//	CItem theItem;
		//
		//	if (CArmorItem theArmor = theItem.AsArmorItem())
		//		{
		//		... do something with theArmor
		//		}
		//
		//	if (theItem.IsArmor())
		//		{
		//		some_function(theItem.AsArmorItem());
		//		}
		//
		//	If you're sure that theItem is an armor item, then you can do this:
		//
		//	some_function(theItem.AsArmorItemOrThrow());
		//
		//	Similar patterns apply for other item categories (devices, etc.).

		inline bool IsArmor (void) const;
		const CArmorItem AsArmorItem (void) const { return CArmorItem(IsArmor() ? *this : CItem::m_NullItem); }
		CArmorItem AsArmorItem (void) { return CArmorItem(IsArmor() ? *this : CItem::m_NullItem); }
		const CArmorItem AsArmorItemOrThrow (void) const { if (IsArmor()) return CArmorItem(*this); else throw CException(ERR_FAIL); }
		CArmorItem AsArmorItemOrThrow (void) { if (IsArmor()) return CArmorItem(*this); else throw CException(ERR_FAIL); }

		inline bool IsDevice (void) const;
		inline bool IsLauncher (void) const;
		inline bool IsWeapon (void) const;
		const CDeviceItem AsDeviceItem (void) const { return CDeviceItem(IsDevice() ? *this : NullItem()); }
		CDeviceItem AsDeviceItem (void) { return CDeviceItem(IsDevice() ? *this : CItem::m_NullItem); }
		const CDeviceItem AsDeviceItemOrThrow (void) const { if (IsDevice()) return CDeviceItem(*this); else throw CException(ERR_FAIL); }
		CDeviceItem AsDeviceItemOrThrow (void) { if (IsDevice()) return CDeviceItem(*this); else throw CException(ERR_FAIL); }

		inline bool IsMissile (void) const;
		const CMissileItem AsMissileItem (void) const { return CMissileItem(IsMissile() ? *this : NullItem()); }
		CMissileItem AsMissileItem (void) { return CMissileItem(IsMissile() ? *this : NullItem()); }
		const CMissileItem AsMissileItemOrThrow (void) const { if (IsMissile()) return CMissileItem(*this); else throw CException(ERR_FAIL); }
		CMissileItem AsMissileItemOrThrow (void) { if (IsMissile()) return CMissileItem(*this); else throw CException(ERR_FAIL); }

		//	Item Criteria

		static const CItem &NullItem (void) { return CItem::m_NullItem; }
		static DWORD ParseFlags (ICCItem *pItem);
		bool MatchesCriteria (const CItemCriteria &Criteria) const;

		void ReadFromStream (SLoadCtx &Ctx);
		void WriteToStream (IWriteStream *pStream) const;

		void ReadFromCCItem (CDesignCollection &Design, const CSystem *pSystem, const ICCItem *pBuffer);
		ICCItem *WriteToCCItem (void) const;

	private:
		enum class EInstalled
			{
			None,

			Armor,
			Device,
			};

		static constexpr DWORD UNKNOWN_INDEX_LOWER_MASK = (flagUnknownBit0 | flagUnknownBit1 | flagUnknownBit2);
		static constexpr DWORD UNKNOWN_INDEX_LOWER_SHIFT = 5;
		static constexpr DWORD UNKNOWN_INDEX_MASK = (flagUnknownBit0 | flagUnknownBit1 | flagUnknownBit2 | flagUnknownBit3);
		static constexpr int UNKNOWN_INDEX_MAX = 15;

		struct SExtra
			{
			EInstalled m_iInstalled = EInstalled::None;
			void *m_pInstalled = NULL;			//	Pointer to either CInstalledArmor or CInstalledDevice
			int m_iInstalledIndex = -1;			//	Install location

			DWORD m_dwCharges = 0;				//	Charges for items
			DWORD m_dwLevel = 0;				//	For scalable items, this stores the level
			DWORD m_dwDisruptedTime = 0;		//	If >0, the tick on which disruption expires
			DWORD m_dwVariantCounter = 0;		//	Counter for setting variants
			int m_iDamagedHP = 0;				//	Hit Points of damage taken (for armor, etc.).

			CItemEnhancement m_Mods;			//	Class-specific modifications (e.g., armor enhancements)

			CAttributeDataBlock m_Data;			//	Opaque data
			};

		void AccumulateCustomAttributes (TArray<SDisplayAttribute> *retList, ICCItem *pData) const;
		void Extra (void);
		bool FindCustomProperty (const CString &sProperty, ICCItemPtr &pResult) const;
		int GetScalableLevel (void) const { return (m_pExtra ? (int)m_pExtra->m_dwLevel : 0); }
		int GetValue (bool bActual = false) const;
		bool IsExtraEqual (SExtra *pSrc, DWORD dwFlags) const;
		bool IsFlagsEqual (const CItem &Src, DWORD dwFlags) const;
		bool SetCustomProperty (const CString &sProperty, const ICCItem &Value);
		void SetScalableLevel (int iValue) { Extra(); m_pExtra->m_dwLevel = iValue; }

		static bool IsDisruptionEqual (DWORD dwNow, DWORD dwD1, DWORD dwD2);
		static bool IsExtraEmpty (const SExtra *pExtra, DWORD dwFlags, DWORD dwNow);

		CItemType *m_pItemType = NULL;

		DWORD m_dwCount:16 = 0;					//	Number of items
		DWORD m_dwFlags:8 = 0;					//	Miscellaneous flags
		DWORD m_dwUnused:8 = 0;					//	Spare

		SExtra *m_pExtra = NULL;				//	Extra data (may be NULL)

		static CItem m_NullItem;
		static CItemEnhancement m_NullMod;
	};

//	ITEM LISTS -----------------------------------------------------------------

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
		int GetCount (void) const { return m_List.GetCount(); }
		int GetCountOf (const CItemType &Type) const;
		CItem &GetItem (int iIndex) { return *m_List[iIndex]; }
		const CItem &GetItem (int iIndex) const { return *m_List[iIndex]; }
		void ReadFromStream (SLoadCtx &Ctx);
		void SortItems (void);
		void SortItems (const CItemCriteria &SortFirst);
		void WriteToStream (IWriteStream *pStream);

		static CItemList &Null (void) { return m_Null; }

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

		bool AddDamagedComponents (const CItem &Item, int iDamageChance, int iDamagedHP);
		void AddItem (const CItem &Item);
		bool AddItems (const CItemList &ItemList, int iChance = 100);

		bool FindItem (const CItem &Item, DWORD dwFlags, int *retiCursor) 
			{ int iCursor = FindItem(Item, dwFlags); if (iCursor == -1) return false; if (retiCursor) *retiCursor = iCursor; return true; }
		int GetCount (void) const { return m_ViewMap.GetCount(); }
		int GetCursor (void) const { return m_iCursor; }
		void SetCursor (int iCursor) { m_iCursor = Min(Max(-1, iCursor), GetCount() - 1); }
		bool SetCursorAtItem (const CItem &Item, DWORD dwFlags = 0);
		void SetFilter (const CItemCriteria &Filter);

		static constexpr DWORD FLAG_SORT_ITEMS = 0x00000001;
		bool Refresh (const CItem &Item, DWORD dwFlags = 0);
		bool RefreshAndSortEnabled (const CItem &Item, const CItemCriteria &EnabledItems);

		bool IsCursorValid (void) const { return (m_iCursor != -1 && m_iCursor < m_ItemList.GetCount()); }
		bool MoveCursorBack (void);
		bool MoveCursorForward (void);
		void ResetCursor (void);
		void SyncCursor (void);

		DWORD AddItemEnhancementAtCursor (const CItemEnhancement &Mods, int iCount = 1);
		void ClearDisruptedAtCursor (int iCount = 1);
		void ClearInstalledAtCursor (void);
		void DeleteAtCursor (int iCount);
		void DeleteMarkedItems (void);
		const CItem &GetItemAtCursor (void) const;
		CItem *GetItemPointerAtCursor (void);
		bool IsItemPointerValid (const CItem *pItem) const;
		void MarkDeleteAtCursor (int iCount);
		void RemoveItemEnhancementAtCursor (DWORD dwID, int iCount = 1);
		void SetChargesAtCursor (int iCharges, int iCount = 1);
		void SetCountAtCursor (int iCount);
		void SetDamagedAtCursor (bool bDamaged, int iCount = 1);
		bool SetDamagedAtCursor (int iDamagedHP, int iCount = 1);
		void SetDataAtCursor (const CString &sAttrib, ICCItem *pData, int iCount = 1);
		void SetDisruptedAtCursor (DWORD dwDuration, int iCount = 1);
		void SetEnhancedAtCursor (bool bEnhanced);
		void SetInstalledAtCursor (CInstalledArmor &Installed);
		void SetInstalledAtCursor (CInstalledDevice &Installed);
		void SetPrepareUninstalledAtCursor (void);
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
		CItemCtx (CItemType *pItemType) : m_Item(pItemType, 1), m_pItem(&m_Item) { }
		CItemCtx (const CItemType *pItemType) : m_Item(const_cast<CItemType *>(pItemType), 1), m_pItem(&m_Item) { }
		CItemCtx (const CItem &Item) : m_pItem(&Item) { }
		CItemCtx (const CShipClass *pSource, const CItem &Item) : m_pSourceShipClass(pSource), m_pItem(&Item) { }
		CItemCtx (const CItem *pItem = NULL, CSpaceObject *pSource = NULL) : m_pItem(pItem), m_pSource(pSource) { }
		CItemCtx (const CItem *pItem, CSpaceObject *pSource, CInstalledArmor *pArmor) : m_pItem(pItem), m_pSource(pSource), m_pArmor(pArmor) { }
		CItemCtx (const CItem *pItem, CSpaceObject *pSource, CInstalledDevice *pDevice) : m_pItem(pItem), m_pSource(pSource), m_pDevice(pDevice) { }
		CItemCtx (CSpaceObject *pSource, CInstalledArmor *pArmor) : m_pSource(pSource), m_pArmor(pArmor) { }
		CItemCtx (CSpaceObject *pSource, CInstalledDevice *pDevice) : m_pSource(pSource), m_pDevice(pDevice) { }

		CItemCtx (const CSpaceObject *pSource, const CInstalledArmor *pArmor) : m_pSource(const_cast<CSpaceObject *>(pSource)), m_pArmor(const_cast<CInstalledArmor *>(pArmor)) { }
		CItemCtx (const CSpaceObject *pSource, const CInstalledDevice *pDevice) : m_pSource(const_cast<CSpaceObject *>(pSource)), m_pDevice(const_cast<CInstalledDevice *>(pDevice)) { }

		void ClearItemCache (void);
		ICCItem *CreateItemVariable (CCodeChain &CC);
		CInstalledArmor *GetArmor (void) const;
		CArmorClass *GetArmorClass (void) const;
		CInstalledDevice *GetDevice (void);
		CDeviceClass *GetDeviceClass (void);
		CDeviceItem GetDeviceItem (void);
		TSharedPtr<CItemEnhancementStack> GetEnhancementStack (void);
		const CItemEnhancementStack &GetEnhancements (void) { const CItemEnhancementStack *pStack = GetEnhancementStack(); if (pStack) return *pStack; else return *m_pNullEnhancements; }
		const CItem &GetItem (void) const;
		int GetItemCharges (void);
		int GetItemVariantNumber (void);
		const CItemEnhancement &GetMods (void);
		CSpaceObject *GetSource (void) { return m_pSource; }
		const CShipClass *GetSourceShipClass (void) const;
		int GetVariant (void) const { return m_iVariant; }
		CDeviceClass *GetVariantDevice (void) const { return m_pWeapon; }
		bool IsItemNull (void) { GetItem(); return (m_pItem == NULL || m_pItem->GetType() == NULL); }
		bool IsDeviceDamaged (void);
		bool IsDeviceDisrupted (void);
		bool IsDeviceEnabled (void);
		bool IsDeviceWorking (void);
		bool ResolveVariant (void);

	private:
		const CItem *GetItemPointer (void) const;

		mutable const CItem *m_pItem = NULL;	//	The item
		mutable CItem m_Item;					//	A cached item, if we need to cons one up.
		CSpaceObject *m_pSource = NULL;			//	Where the item is installed (may be NULL)
		const CShipClass *m_pSourceShipClass = NULL;	//	For some ship class functions
		mutable CInstalledArmor *m_pArmor = NULL;		//	Installation structure (may be NULL)
		CInstalledDevice *m_pDevice = NULL;		//	Installation structure (may be NULL)

		CDeviceClass *m_pWeapon = NULL;			//	This is the weapon that uses the given item
		int m_iVariant = -1;					//	NOTE: In this case, m_pItem may be either a
												//	missile or the weapon.
		CItem m_Weapon;

		TSharedPtr<CItemEnhancementStack> m_pEnhancements;	//	Only used if we need to cons one up

		static TSharedPtr<CItemEnhancementStack> m_pNullEnhancements;
	};

//	IItemGenerator -------------------------------------------------------------

struct SItemAddCtx
	{
	SItemAddCtx (CUniverse &UniverseArg) :
			Universe(UniverseArg),
			m_Internal(CItemList::Null()),
			ItemList(m_Internal)
		{ }

	SItemAddCtx (CUniverse &UniverseArg, CItemListManipulator &ItemListArg) :
			Universe(UniverseArg),
			ItemList(ItemListArg),
			m_Internal(CItemList::Null())
		{ }

	SItemAddCtx (CItemListManipulator &theItemList) : 
			Universe(*g_pUniverse),
			ItemList(theItemList),
			m_Internal(CItemList::Null())
		{ }

	CUniverse &GetUniverse (void) { return Universe; }

	CUniverse &Universe;
	CItemListManipulator &ItemList;				//	Item list to add items to

	CSystem *pSystem = NULL;					//	System where we're creating items
	CSpaceObject *pDest = NULL;					//	Object to add to (may be NULL)
	CVector vPos;								//	Position to use (for LocationCriteriaTable)
	int iLevel = 1;								//	Level to use for item create (for LevelTable)

	private:
		CItemListManipulator m_Internal;
	};

class CItemTypeProbabilityTable
	{
	public:
		void Add (CItemType *pType, Metric rProbability = 1.0);
		void Add (const CItemTypeProbabilityTable &Src);
		void DeleteAll (void) { m_Table.DeleteAll(); }
		int GetCount (void) const { return m_Table.GetCount(); }
		Metric GetProbability (int iIndex) const { return m_Table[iIndex]; }
		Metric GetProbability (CItemType *pType) const { const Metric *pProb = m_Table.GetAt(pType); return (pProb ? *pProb : 0.0); }
		CItemType *GetType (int iIndex) const { return m_Table.GetKey(iIndex); }
		void Scale (Metric rProbability);
		void Union (CItemType *pType, Metric rProbability = 1.0);
		void Union (const CItemTypeProbabilityTable &Src);

	private:
		TSortMap<CItemType *, Metric> m_Table;
	};

class IItemGenerator
	{
	public:
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, const CXMLElement *pDesc, IItemGenerator **retpGenerator);
		static ALERROR CreateLookupTable (SDesignLoadCtx &Ctx, DWORD dwUNID, IItemGenerator **retpGenerator);
		static ALERROR CreateRandomItemTable (CUniverse &Universe,
											  const CItemCriteria &Crit, 
											  const CString &sLevelFrequency,
											  IItemGenerator **retpGenerator);

		virtual ~IItemGenerator (void) { }
		virtual void AddItems (SItemAddCtx &Ctx) { }
		virtual void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) { }
		virtual CurrencyValue GetAverageValue (SItemAddCtx &Ctx) { return 0; }
		virtual CCurrencyAndValue GetDesiredValue (SItemAddCtx &Ctx, int iLevel, int *retiLoopCount = NULL, Metric *retrScale = NULL) const { return CCurrencyAndValue(); }
		virtual IItemGenerator *GetGenerator (int iIndex) { return NULL; }
		virtual int GetGeneratorCount (void) { return 0; }
		virtual CItemType *GetItemType (int iIndex) { return NULL; }
		virtual int GetItemTypeCount (void) { return 0; }
		virtual CItemTypeProbabilityTable GetProbabilityTable (SItemAddCtx &Ctx) const { return CItemTypeProbabilityTable(); }
		virtual bool HasItemAttribute (const CString &sAttrib) const { return false; }
		virtual ALERROR LoadFromXML (SDesignLoadCtx &Ctx, const CXMLElement *pDesc) { return NOERROR; }
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) { return NOERROR; }

	protected:
		static int CalcLocationAffinity (SItemAddCtx &Ctx, const CAffinityCriteria &Criteria);
	};
