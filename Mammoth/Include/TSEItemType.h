//	TSEItemType.h
//
//	Defines classes and interfaces for item types.
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CItemType : public CDesignType
	{
	public:
		enum Flags
			{
			//	Flags for GetReference, 
			//	GetReferenceDamageAdj, 
			//	and GetReferenceDamateType
			FLAG_ACTUAL_ITEM =			0x00000001,	//	Ignore Unknown flag
			};

		enum ECachedHandlers
			{
			//	This list must match CACHED_EVENTS array in CItemType.cpp

			evtGetDescription			= 0,
			evtGetDisplayAttributes		= 1,
			evtGetName					= 2,
			evtGetReferenceText			= 3,
			evtGetTradePrice			= 4,
			evtOnDestroyCheck			= 5,
			evtOnDocked					= 6,
			evtOnEnabled				= 7,
			evtOnInstall				= 8,
			evtOnRefuel					= 9,
			evtGetHUDName				= 10,

			evtCount					= 11,
			};

		struct SUseDesc
			{
			SUseDesc (void)	{ }

			CExtension *pExtension = NULL;			//	Extension the code came from
			ICCItem *pCode = NULL;					//	Use code
			CDesignType *pScreenRoot = NULL;
			CString sScreenName;

			CString sUseKey;
			bool bUsable = false;
			bool bUsableInCockpit = false;
			bool bOnlyIfEnabled = false;
			bool bOnlyIfInstalled = false;
			bool bOnlyIfUninstalled = false;
			bool bOnlyIfUndamaged = false;
			bool bOnlyIfCompleteArmor = false;
			bool bAsArmorSet = false;
			};

		struct SStdStats
			{
			CurrencyValue TreasureValue;	//	Value of std treasure at system level (in credits)
			};

		CItemType ();
		virtual ~CItemType ();

		void AddWeapon (CDeviceClass *pWeapon) { ASSERT(!m_Weapons.Find(pWeapon)); m_Weapons.Insert(pWeapon); }
		bool AreChargesAmmo () const { return (m_fAmmoCharges ? true : false); }
		bool AreChargesValued () const { return (m_fValueCharges ? true : false); }
		bool CanBeSoldIfUsed () const { return (m_fNoSaleIfUsed ? false : true); }
		bool CanBeUnknown () const { return (m_UnknownTypes.GetCount() > 0); }
		void ClearShowReference () { m_fReference = false; }
		void CreateEmptyFlotsam (CSystem &System, const CVector &vPos, const CVector &vVel, CSovereign *pSovereign, CStation **retpFlotsam);
		bool FindEventHandlerItemType (ECachedHandlers iEvent, SEventHandlerDesc *retEvent = NULL) const 
			{
			if (!m_CachedEvents[iEvent].pCode)
				return false;

			if (retEvent) *retEvent = m_CachedEvents[iEvent];
			return true;
			}

		ICCItemPtr FindItemTypeBaseProperty (CCodeChainCtx &Ctx, const CString &sProperty) const;
		CDeviceClass *GetAmmoLauncher (int *retiVariant = NULL) const;
		CArmorClass *GetArmorClass () const { return m_pArmor; }
		ItemCategories GetCategory () const;
		int GetCharges () const { return (m_fInstanceData ? m_InitDataValue.Roll() : 0); }
		int GetChargesLimit () const { return m_iMaxCharges; }
		const CItemList &GetComponents () const { return m_Components; }
		CCurrencyAndValue GetCurrencyAndValue (CItemCtx &Ctx, bool bActual = false) const;
		const CEconomyType *GetCurrencyType () const { return m_iValue.GetCurrencyType(); }
		const CString &GetData () const { return m_sData; }
		const CString &GetDesc (bool bActual = false) const;
		CDeviceClass *GetDeviceClass () const { return m_pDevice; }
		CObjectImageArray &GetFlotsamImage () { if (!m_FlotsamImage.IsLoaded()) CreateFlotsamImage(); return m_FlotsamImage; }
		int GetFrequency () const { return m_Frequency; }
		int GetFrequencyByLevel (int iLevel);
		const CObjectImageArray &GetImage (bool bActual = false) const;
		const TArray<CDeviceClass *> &GetLaunchWeapons () const { return m_Weapons; }
		Metric GetMass (CItemCtx &Ctx) const { return GetMassKg(Ctx) / 1000.0; }
		int GetMassBonusPerCharge () const { return m_iExtraMassPerCharge; }
		int GetMassKg (CItemCtx &Ctx) const;
		int GetMaxCharges () const { return (m_iMaxCharges == -1 ? GetMaxInitialCharges() : m_iMaxCharges); }
		int GetMaxHPBonus () const;
		int GetMaxInitialCharges () const { return (m_fInstanceData ? m_InitDataValue.GetMaxValue() : 0); }
		int GetMaxLevel () const { return m_iMaxLevel; }
		int GetMinLevel () const { return m_iLevel; }
		CWeaponFireDesc *GetMissileDesc () const { return m_pMissile;  }
		DWORD GetModCode () const { return m_dwModCode; }
		const DiceRange &GetNumberAppearing () const { return m_NumberAppearing; }
		int GetRandomUnknownTypeIndex () const;
		CString GetReference (CItemCtx &Ctx, const CItem &Ammo = CItem(), DWORD dwFlags = 0) const;
		const CString &GetRole () const { return m_sRole; }
		CString GetSortName (CItemCtx &Ctx) const;
		const CString &GetUnknownName (int iIndex) const { return m_UnknownTypes[iIndex].sUnknownName; }
		CItemType *GetUnknownType (int iIndex) const { return m_UnknownTypes[iIndex].pUnknownType; }
		int GetUnknownTypeCount () const { return m_UnknownTypes.GetCount(); }
		ICCItem *GetUseCode () const { return m_pUseCode; }
		bool GetUseDesc (SUseDesc *retDesc = NULL) const;
		int GetValue (CItemCtx &Ctx, bool bActual = false) const { return (int)GetCurrencyAndValue(Ctx, bActual).GetValue(); }
		int GetValueBonusPerCharge () const { return m_iExtraValuePerCharge; }
		CWeaponFireDesc *GetWeaponFireDesc (CItemCtx &Ctx, CString *retsError = NULL) const;
		bool HasOnRefuelCode () const { return FindEventHandlerItemType(evtOnRefuel); }
		bool HasOnInstallCode () const { return FindEventHandlerItemType(evtOnInstall); }
		bool IsAmmunition () const;
		bool IsArmor () const { return (m_pArmor != NULL); }
		bool IsDevice () const { return (m_pDevice != NULL); }
		bool IsKnown (int iIndex) const { return m_UnknownTypes[iIndex].bKnown; }
		bool IsFuel () const;
		bool IsMissile () const;
		bool IsScalable () const { return (m_fScalable ? true : false); }
		bool IsUsable () const { return GetUseDesc(NULL); }
		void SetAllKnown (bool bKnown = true);
		void SetKnown (int iIndex, bool bKnown = true) { m_UnknownTypes[iIndex].bKnown = bKnown; }
		void SetShowReference () { m_fReference = true; }
		bool ShowChargesInUseMenu () const { return (m_fShowChargesInUseMenu ? true : false); }
		bool ShowReference () const { return (m_fReference ? true : false); }

		//	CDesignType overrides
		static const CItemType *AsType (const CDesignType *pType) { return ((pType && pType->GetType() == designItemType) ? (CItemType *)pType : NULL); }
		static CItemType *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designItemType) ? (CItemType *)pType : NULL); }
		virtual bool FindDataField (const CString &sField, CString *retsValue) const override;
		virtual int GetLevel (int *retiMinLevel = NULL, int *retiMaxLevel = NULL) const override { if (retiMinLevel) *retiMinLevel = m_iLevel; if (retiMaxLevel) *retiMaxLevel = m_iMaxLevel; return m_iLevel; }
		virtual CString GetNamePattern (DWORD dwNounFormFlags = 0, DWORD *retdwFlags = NULL) const override { if (retdwFlags) *retdwFlags = m_dwNameFlags; return m_sName; }
		virtual CCurrencyAndValue GetTradePrice (const CSpaceObject *pObj = NULL, bool bActual = false) const override;
		virtual DesignTypes GetType () const override { return designItemType; }
		virtual bool IsVirtual () const override { return (m_fVirtual ? true : false); }

		static ItemCategories GetCategoryForNamedDevice (DeviceNames iDev);
		static CString GetItemCategory (ItemCategories iCategory);
		static const SStdStats &GetStdStats (int iLevel);
		static ALERROR ParseFate (SDesignLoadCtx &Ctx, const CString &sDesc, ItemFates *retiFate);
		static bool ParseItemCategory (const CString &sCategory, ItemCategories *retCategory = NULL);

	protected:
		//	CDesignType overrides
		virtual void OnAccumulateXMLMergeFlags (TSortMap<DWORD, DWORD> &MergeFlags) const override;
		virtual void OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override;
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual CEffectCreator *OnFindEffectCreator (const CString &sUNID) override;
		virtual const CEconomyType &OnGetDefaultCurrency () const override;
		virtual ICCItemPtr OnGetProperty (CCodeChainCtx &Ctx, const CString &sProperty) const override;
		virtual bool OnHasSpecialAttribute (const CString &sAttrib) const override;
		virtual ALERROR OnPrepareBindDesign (SDesignLoadCtx &Ctx) override;
		virtual void OnReadFromStream (SUniverseLoadCtx &Ctx) override;
		virtual void OnReinit () override;
		virtual bool OnSetTypeProperty (const CString &sProperty, const ICCItem &Value) override;
		virtual void OnUnbindDesign () override;
		virtual void OnWriteToStream (IWriteStream *pStream) override;

	private:
		struct SUnknownTypeDesc
			{
			CItemTypeRef pUnknownType;			//	An unknown item type
			CString sUnknownName;				//	Our name when unknown
			bool bKnown = false;				//	Identified
			};

		void CreateFlotsamImage ();
		CString GenerateRandomName (const CString &sTemplate, const TArray<CString> &RetiredNames = TArray<CString>(), bool *retbTempletized = NULL) const;
		CStationType *GetFlotsamStationType ();
		CString GenerateUnknownName (int iIndex, DWORD *retdwFlags = NULL);
		CDesignType *GetUseScreen (CString *retsName) const;
		void InitRandomNames ();
		void InitComponents ();
		bool IsUnknownType (DWORD dwUNID, int *retiUnknownIndex = NULL) const;
		void SetUnknownName (int iIndex, const CString &sName) { m_UnknownTypes[iIndex].sUnknownName = sName; }

		CString m_sName;						//	Full name of item
		DWORD m_dwNameFlags;					//	Name flags
		CString m_sSortName;					//	Name to sort by
		CString m_sRole;						//	Role in design (internal)

		int m_iLevel;							//	Level of item
		int m_iMaxLevel;                        //  Max level, for scalable items
		CCurrencyAndValue m_iValue;				//	Value in some currency
		int m_iMass;							//	Mass in kilograms
		FrequencyTypes m_Frequency;				//	Frequency
		DiceRange m_NumberAppearing;			//	Number appearing

		CObjectImageArray m_Image;				//	Image of item
		CString m_sDescription;					//	Description of item
		TArray<SUnknownTypeDesc> m_UnknownTypes;//	List of unknown types (if we are unknown)
		TArray<CString> m_UnknownNames;			//	List of unknown names (if we are the unknown item placeholder)
		DiceRange m_InitDataValue;				//	Initial data value
		int m_iMaxCharges;						//	Do not allow charges above this level (-1 = no limit)

		int m_iExtraMassPerCharge;				//	Extra mass per charge (in kilos)
		int m_iExtraValuePerCharge;				//	Extra value per charge (may be negative)

		//	Components
		IItemGenerator *m_pComponents;			//	Table to generate item components (may be NULL)
		CItemList m_Components;					//	Components (may be empty)

		//	Events
		SEventHandlerDesc m_CachedEvents[evtCount];

		//	Usable items
		CDockScreenTypeRef m_pUseScreen;		//	Screen shown when used (may be NULL)
		ICCItem *m_pUseCode;					//	Code when using item from cockpit
		CString m_sUseKey;						//	Hotkey to invoke usage
		DWORD m_dwModCode;						//	Mod code conveyed to items we enhance

		//	Armor
		CArmorClass *m_pArmor;					//	Armor properties (may be NULL)

		//	Devices
		CDeviceClass *m_pDevice;				//	Device properties (may be NULL)

		//	Missiles
		CWeaponFireDesc *m_pMissile;			//	Missile desc (may be NULL)
		TArray<CDeviceClass *> m_Weapons;		//	Weapons that fire this missile/ammo

		//	Flotsam
		CG32bitImage m_FlotsamBitmap;			//	Image used for flotsam
		CObjectImageArray m_FlotsamImage;		//	Image used for flotsam

		DWORD m_fRandomDamaged:1 = false;		//	Randomly damaged when found
		DWORD m_fReference:1 = false;			//	Does this type show reference info?
		DWORD m_fDefaultReference:1 = false;	//	TRUE if this shows reference by default
		DWORD m_fInstanceData:1 = false;		//	TRUE if we need to set instance data at create time
		DWORD m_fVirtual:1 = false;				//	TRUE if this is a virtual item needed for a weapon that invokes
		DWORD m_fUseInstalled:1 = false;		//	If TRUE, item can only be used when installed
		DWORD m_fValueCharges:1 = false;		//	TRUE if value should be adjusted based on charges
		DWORD m_fUseUninstalled:1 = false;		//	If TRUE, item can only be used when uninstalled

		DWORD m_fUseEnabled:1 = false;			//	If TRUE, item can only be used when enabled
		DWORD m_fScalable:1 = false;			//  If TRUE, VariantHigh adds to level.
		DWORD m_fUseCompleteArmor:1 = false;	//	If TRUE, item can be used as a complete armor set
		DWORD m_fUseAsArmorSet:1 = false;		//	If TRUE, we only show item once for armor set
		DWORD m_fAmmoCharges:1 = false;			//	If TRUE, charges are ammo
		DWORD m_fNoSaleIfUsed:1 = false;		//	If TRUE, cannot be sold once it's been used
		DWORD m_fShowChargesInUseMenu:1 = false;//	If TRUE, the use menu shows charges instead of a count.
		DWORD m_fUseUndamaged:1 = false;		//	If TRUE, item can only be used if undamaged

		DWORD m_dwSpare:16;

		CString m_sData;						//	Category-specific data

		static SStdStats m_Stats[MAX_ITEM_LEVEL];
	};

//	CItemTable ----------------------------------------------------------------

class CItemTable : public CDesignType
	{
	public:
		CItemTable (void);
		virtual ~CItemTable (void);

		void AddItems (SItemAddCtx &Ctx) { if (m_pGenerator) m_pGenerator->AddItems(Ctx); }
		CurrencyValue GetAverageValue (SItemAddCtx &Ctx) const { return (m_pGenerator ? m_pGenerator->GetAverageValue(Ctx) : 0); }
		IItemGenerator *GetGenerator (void) { return m_pGenerator; }
		CItemTypeProbabilityTable GetProbabilityTable (SItemAddCtx &Ctx) const { return m_pGenerator->GetProbabilityTable(Ctx); }
		bool HasItemAttribute (const CString &sAttrib) const { return (m_pGenerator ? m_pGenerator->HasItemAttribute(sAttrib) : false); }

		//	CDesignType overrides
		static CItemTable *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designItemTable) ? (CItemTable *)pType : NULL); }
		virtual bool FindDataField (const CString &sField, CString *retsValue) const override;
		virtual DesignTypes GetType (void) const override { return designItemTable; }

	protected:
		//	CDesignType overrides
		virtual void OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override;
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;

	private:
		IItemGenerator *m_pGenerator;
	};
