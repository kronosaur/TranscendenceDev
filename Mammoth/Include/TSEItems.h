//	TSEItems.h
//
//	Defines classes and interfaces for items
//	Copyright (c) 2016 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CInstalledArmor;
class CInstalledDevice;
class CItemList;
class CShipClass;

//	ITEM -----------------------------------------------------------------------

class CDifferentiatedItem
	{
	public:
		inline int GetCharges (void) const;
		inline int GetLevel (void) const;
		inline int GetMinLevel (void) const;
		inline const CItemType &GetType (void) const;
		inline CItemType &GetType (void);

	protected:
		CDifferentiatedItem (CItem *pItem) :
				m_pCItem(pItem),
				m_pItem(pItem)
			{ }

		CDifferentiatedItem (const CItem *pItem) :
				m_pCItem(pItem)
			{ }

		const CItem *m_pCItem = NULL;
		CItem *m_pItem = NULL;

		mutable TSharedPtr<CItemEnhancementStack> m_pEnhancements;	//	Only used if we need to cons one up

		static TSharedPtr<CItemEnhancementStack> m_pNullEnhancements;
	};

class CArmorItem : public CDifferentiatedItem
	{
	public:
		operator bool () const { return (m_pCItem != NULL); }
		operator bool () { return (m_pItem != NULL); }
		operator const CItem & () const { return *m_pCItem; }
		operator CItem & () { return *m_pItem; }

		inline const CArmorClass &GetArmorClass (void) const;
		inline const CItemEnhancementStack &GetEnhancements (void) const;
		inline const CInstalledArmor *GetInstalledArmor (void) const;
		inline int GetMaxHP (bool bForceComplete = false) const;
		inline CSpaceObject *GetSource (void) const;

	private:
		CArmorItem (CItem *pItem) : CDifferentiatedItem(pItem)
			{ }

		CArmorItem (const CItem *pItem) : CDifferentiatedItem(pItem)
			{ }

		TSharedPtr<CItemEnhancementStack> GetEnhancementStack (void) const;

	friend class CItem;
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

			FLAG_FIND_MIN_CHARGES =			0x00010000,	//	Item with least number of charges
			};

		CItem (void);
		CItem (const CItem &Copy);
		CItem (CItemType *pItemType, int iCount);
		~CItem (void);
		CItem &operator= (const CItem &Copy);

		DWORD AddEnhancement (const CItemEnhancement &Enhancement);
		CString CalcSortKey (void) const;
		bool CanBeUsed (CItemCtx &ItemCtx, CString *retsUseKey = NULL) const;
		void ClearDamaged (void);
		void ClearDisrupted (void) { if (m_pExtra) m_pExtra->m_dwDisruptedTime = 0; }
		void ClearEnhanced (void) { m_dwFlags &= ~flagEnhanced; }
		void ClearInstalled (void);
		static CItem CreateItemByName (CUniverse &Universe, const CString &sName, const CItemCriteria &Criteria, bool bActualName = false);
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
		inline const CEconomyType *GetCurrencyType (void) const;
		int GetDamagedHP (CItemCtx &ItemCtx) const;
		ICCItemPtr GetDataAsItem (const CString &sAttrib) const;
		CString GetDesc (CItemCtx &ItemCtx, bool bActual = false) const;
		bool GetDisplayAttributes (CItemCtx &Ctx, TArray<SDisplayAttribute> *retList, ICCItem *pData = NULL, bool bActual = false) const;
		DWORD GetDisruptedDuration (void) const;
		bool GetDisruptedStatus (DWORD *retdwTimeLeft = NULL, bool *retbRepairedEvent = NULL) const;
		CString GetEnhancedDesc (CSpaceObject *pInstalled = NULL) const;
		int GetHitPoints (CItemCtx &Ctx, int *retiMaxHP = NULL, bool bUninstalled = false) const;
		int GetInstalled (void) const { return (int)(char)m_dwInstalled; }
		const CInstalledArmor *GetInstalledArmor (void) const { if (m_pExtra && m_pExtra->m_iInstalled == installedArmor) return (const CInstalledArmor *)m_pExtra->m_pInstalled; else return NULL; }
		CInstalledArmor *GetInstalledArmor (void) { if (m_pExtra && m_pExtra->m_iInstalled == installedArmor) return (CInstalledArmor *)m_pExtra->m_pInstalled; else return NULL; }
		const CInstalledDevice *GetInstalledDevice (void) const { if (m_pExtra && m_pExtra->m_iInstalled == installedDevice) return (const CInstalledDevice *)m_pExtra->m_pInstalled; else return NULL; }
		CInstalledDevice *GetInstalledDevice (void) { if (m_pExtra && m_pExtra->m_iInstalled == installedDevice) return (CInstalledDevice *)m_pExtra->m_pInstalled; else return NULL; }
		ICCItem *GetItemProperty (CCodeChainCtx &CCCtx, CItemCtx &Ctx, const CString &sProperty) const;
		Metric GetItemPropertyDouble (CCodeChainCtx &CCCtx, CItemCtx &Ctx, const CString &sProperty) const;
		int GetItemPropertyInteger (CCodeChainCtx &CCCtx, CItemCtx &Ctx, const CString &sProperty) const;
		CString GetItemPropertyString (CCodeChainCtx &CCCtx, CItemCtx &Ctx, const CString &sProperty) const;
        int GetLevel (void) const;
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
		CUniverse &GetUniverse (void) const;
		int GetUnknownIndex (void) const { return (int)((m_dwFlags & flagUnknownBit3) | ((m_dwFlags & UNKNOWN_INDEX_LOWER_MASK) >> UNKNOWN_INDEX_LOWER_SHIFT)); }
		inline int GetVariantNumber(void) const { return (m_pExtra ? (int)m_pExtra->m_dwVariantCounter : 0); }
		bool HasComponents (void) const;
		inline bool HasMods (void) const { return (m_pExtra && m_pExtra->m_Mods.IsNotEmpty()); }
		bool HasSpecialAttribute (const CString &sAttrib) const;
		bool HasUseItemScreen (void) const;
		bool IsDamaged (int *retiDamagedHP = NULL) const;
		bool IsDisrupted (void) const;
		bool IsDisrupted (DWORD dwNow) const { return (m_pExtra ? (m_pExtra->m_dwDisruptedTime >= dwNow) : false); }
        bool IsEmpty (void) const { return (m_pItemType == NULL); }
		bool IsEnhanced (void) const { return (m_dwFlags & flagEnhanced ? true : false); }
		bool IsInstalled (void) const { return (m_dwInstalled != 0xff); }
		bool IsKnown (void) const;
		bool IsMarkedForDelete (void) { return (m_dwCount == 0xffff); }
		bool IsVirtual (void) const;
		inline void MarkForDelete (void) { m_dwCount = 0xffff; }
		bool RemoveEnhancement (DWORD dwID);
		void SetCharges (int iCharges);
		inline void SetCount (int iCount) { m_dwCount = (DWORD)iCount; }
		void SetDamaged (bool bDamaged = true) { ClearDamaged(); if (bDamaged) m_dwFlags |= flagDamaged; }
		void SetDamaged (int iDamagedHP);
		inline void SetData (const CString &sAttrib, ICCItem *pData) { Extra(); m_pExtra->m_Data.SetData(sAttrib, pData); }
		void SetDisrupted (DWORD dwDuration);
		inline void SetEnhanced (void) { m_dwFlags |= flagEnhanced; }
		inline void SetEnhanced (bool bEnhanced) { ClearEnhanced(); if (bEnhanced) SetEnhanced(); }
		void SetInstalled (CInstalledArmor &Installed);
		void SetInstalled (CInstalledDevice &Installed);
		void SetKnown (bool bKnown = true) const;
        bool SetLevel (int iLevel, CString *retsError = NULL);
		void SetPrepareUninstalled (void);
		bool SetProperty (CItemCtx &Ctx, const CString &sName, ICCItem *pValue, CString *retsError);
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
		const CArmorItem AsArmorItem (void) const { return CArmorItem(IsArmor() ? this : NULL); }
		CArmorItem AsArmorItem (void) { return CArmorItem(IsArmor() ? this : NULL); }
		const CArmorItem AsArmorItemOrThrow (void) const { if (IsArmor()) return CArmorItem(this); else throw CException(ERR_FAIL); }
		CArmorItem AsArmorItemOrThrow (void) { if (IsArmor()) return CArmorItem(this); else throw CException(ERR_FAIL); }

		//	Item Criteria

		static CString GenerateCriteria (const CItemCriteria &Criteria);
		static void InitCriteriaAll (CItemCriteria *retCriteria);
		static const CItem &NullItem (void) { return CItem::m_NullItem; }
		static void ParseCriteria (const CString &sCriteria, CItemCriteria *retCriteria);
		static DWORD ParseFlags (ICCItem *pItem);
		bool MatchesCriteria (const CItemCriteria &Criteria) const;

		void ReadFromStream (SLoadCtx &Ctx);
		void WriteToStream (IWriteStream *pStream) const;

		void ReadFromCCItem (CDesignCollection &Design, ICCItem *pBuffer);
		ICCItem *WriteToCCItem (void) const;

	private:
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

		enum EInstallTypes
			{
			installedNone,

			installedArmor,
			installedDevice,
			};

		static constexpr DWORD UNKNOWN_INDEX_LOWER_MASK = (flagUnknownBit0 | flagUnknownBit1 | flagUnknownBit2);
		static constexpr DWORD UNKNOWN_INDEX_LOWER_SHIFT = 5;
		static constexpr DWORD UNKNOWN_INDEX_MASK = (flagUnknownBit0 | flagUnknownBit1 | flagUnknownBit2 | flagUnknownBit3);
		static constexpr int UNKNOWN_INDEX_MAX = 15;

		struct SExtra
			{
			EInstallTypes m_iInstalled = installedNone;
			void *m_pInstalled = NULL;			//	Pointer to either CInstalledArmor or CInstalledDevice

			DWORD m_dwCharges = 0;				//	Charges for items
			DWORD m_dwLevel = 0;				//	For scalable items, this stores the level
			DWORD m_dwDisruptedTime = 0;		//	If >0, the tick on which disruption expires
			DWORD m_dwVariantCounter = 0;		//	Counter for setting variants
			int m_iDamagedHP = 0;				//	Hit Points of damage taken (for armor, etc.).

			CItemEnhancement m_Mods;			//	Class-specific modifications (e.g., armor enhancements)

			CAttributeDataBlock m_Data;			//	Opaque data
			};

		void AccumulateCustomAttributes (CItemCtx &Ctx, TArray<SDisplayAttribute> *retList, ICCItem *pData) const;
		void Extra (void);
        int GetScalableLevel (void) const { return (m_pExtra ? (int)m_pExtra->m_dwLevel : 0); }
		int GetValue (bool bActual = false) const;
		bool IsExtraEqual (SExtra *pSrc, DWORD dwFlags) const;
		bool IsFlagsEqual (const CItem &Src, DWORD dwFlags) const;
        void SetScalableLevel (int iValue) { Extra(); m_pExtra->m_dwLevel = iValue; }

		static bool IsDisruptionEqual (DWORD dwNow, DWORD dwD1, DWORD dwD2);
		static bool IsExtraEmpty (const SExtra *pExtra, DWORD dwFlags, DWORD dwNow);

		CItemType *m_pItemType = NULL;

		DWORD m_dwCount:16;						//	Number of items
		DWORD m_dwFlags:8;						//	Miscellaneous flags
		DWORD m_dwInstalled:8;					//	Location where item is installed

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

		bool AddDamagedComponents (const CItem &Item, int iDamageChance, int iDamagedHP);
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
		void ClearInstalledAtCursor (void);
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
		bool GetEnhancementDisplayAttributes (TArray<SDisplayAttribute> *retList);
		TSharedPtr<CItemEnhancementStack> GetEnhancementStack (void);
		const CItemEnhancementStack &GetEnhancements (void) { const CItemEnhancementStack *pStack = GetEnhancementStack(); if (pStack) return *pStack; else return *m_pNullEnhancements; }
		const CItem &GetItem (void) const;
		int GetItemCharges (void);
		int GetItemVariantNumber (void);
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
		const CItem *GetItemPointer (void) const;

		mutable const CItem *m_pItem = NULL;	//	The item
		mutable CItem m_Item;					//	A cached item, if we need to cons one up.
		CSpaceObject *m_pSource = NULL;			//	Where the item is installed (may be NULL)
		const CShipClass *m_pSourceShipClass = NULL;	//	For some ship class functions
		mutable CInstalledArmor *m_pArmor = NULL;		//	Installation structure (may be NULL)
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
		inline Metric GetProbability (CItemType *pType) const { const Metric *pProb = m_Table.GetAt(pType); return (pProb ? *pProb : 0.0); }
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
