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

			evtCount					= 10,
			};

		struct SUseDesc
			{
			SUseDesc (void) :
					pExtension(NULL),
					pCode(NULL),
					pScreenRoot(NULL),
					bUsable(false),
					bUsableInCockpit(false),
					bOnlyIfEnabled(false),
					bOnlyIfInstalled(false),
					bOnlyIfUninstalled(false),
					bOnlyIfCompleteArmor(false),
					bAsArmorSet(false)
				{ }

			CExtension *pExtension;			//	Extension the code came from
			ICCItem *pCode;					//	Use code
			CDesignType *pScreenRoot;
			CString sScreenName;

			CString sUseKey;
			bool bUsable;
			bool bUsableInCockpit;
			bool bOnlyIfEnabled;
			bool bOnlyIfInstalled;
			bool bOnlyIfUninstalled;
			bool bOnlyIfCompleteArmor;
			bool bAsArmorSet;
			};

		struct SStdStats
			{
			CurrencyValue TreasureValue;	//	Value of std treasure at system level (in credits)
			};

		CItemType (void);
		virtual ~CItemType (void);

		inline void AddWeapon (CDeviceClass *pWeapon) { ASSERT(!m_Weapons.Find(pWeapon)); m_Weapons.Insert(pWeapon); }
		inline bool AreChargesAmmo (void) const { return (m_fAmmoCharges ? true : false); }
		inline bool AreChargesValued (void) const { return (m_fValueCharges ? true : false); }
		inline bool CanBeSoldIfUsed (void) const { return (m_fNoSaleIfUsed ? false : true); }
		inline void ClearKnown (void) { m_fKnown = false; }
		inline void ClearShowReference (void) { m_fReference = false; }
		void CreateEmptyFlotsam (CSystem *pSystem, const CVector &vPos, const CVector &vVel, CSovereign *pSovereign, CStation **retpFlotsam);
		inline bool FindEventHandlerItemType (ECachedHandlers iEvent, SEventHandlerDesc *retEvent = NULL) const 
			{
			if (!m_CachedEvents[iEvent].pCode)
				return false;

			if (retEvent) *retEvent = m_CachedEvents[iEvent];
			return true;
			}

		ICCItem *FindItemTypeBaseProperty (CCodeChainCtx &Ctx, const CString &sProperty) const;
		int GetApparentLevel (CItemCtx &Ctx) const;
		CDeviceClass *GetAmmoLauncher (int *retiVariant = NULL) const;
		inline CArmorClass *GetArmorClass (void) const { return m_pArmor; }
		ItemCategories GetCategory (void) const;
		inline int GetCharges (void) const { return (m_fInstanceData ? m_InitDataValue.Roll() : 0); }
		inline int GetChargesLimit (void) const { return m_iMaxCharges; }
		inline const CItemList &GetComponents (void) const { return m_Components; }
        CCurrencyAndValue GetCurrencyAndValue (CItemCtx &Ctx, bool bActual = false) const;
		inline CEconomyType *GetCurrencyType (void) const { return m_iValue.GetCurrencyType(); }
		inline const CString &GetData (void) const { return m_sData; }
		const CString &GetDesc (bool bActual = false) const;
		inline CDeviceClass *GetDeviceClass (void) const { return m_pDevice; }
		inline CObjectImageArray &GetFlotsamImage (void) { if (!m_FlotsamImage.IsLoaded()) CreateFlotsamImage(); return m_FlotsamImage; }
		inline int GetFrequency (void) const { return m_Frequency; }
		int GetFrequencyByLevel (int iLevel);
		inline const CObjectImageArray &GetImage (void) { return m_Image; }
		int GetInstallCost (CItemCtx &Ctx) const;
		const TArray<CDeviceClass *> &GetLaunchWeapons (void) const { return m_Weapons; }
        int GetLevel (CItemCtx &Ctx) const;
		inline Metric GetMass (CItemCtx &Ctx) const { return GetMassKg(Ctx) / 1000.0; }
		inline int GetMassBonusPerCharge (void) const { return m_iExtraMassPerCharge; }
		int GetMassKg (CItemCtx &Ctx) const;
		inline int GetMaxCharges (void) const { return (m_iMaxCharges == -1 ? GetMaxInitialCharges() : m_iMaxCharges); }
		int GetMaxHPBonus (void) const;
		inline int GetMaxInitialCharges (void) const { return (m_fInstanceData ? m_InitDataValue.GetMaxValue() : 0); }
        inline int GetMaxLevel (void) const { return m_iMaxLevel; }
		inline CWeaponFireDesc *GetMissileDesc (void) const { return m_pMissile;  }
		inline DWORD GetModCode (void) const { return m_dwModCode; }
		inline const DiceRange &GetNumberAppearing (void) const { return m_NumberAppearing; }
		CString GetReference (CItemCtx &Ctx, const CItem &Ammo = CItem(), DWORD dwFlags = 0) const;
		CString GetSortName (void) const;
		inline CItemType *GetUnknownType (void) { return m_pUnknownType; }
		inline ICCItem *GetUseCode (void) const { return m_pUseCode; }
		bool GetUseDesc (SUseDesc *retDesc) const;
        inline int GetValue (CItemCtx &Ctx, bool bActual = false) const { return (int)GetCurrencyAndValue(Ctx, bActual).GetValue(); }
		inline int GetValueBonusPerCharge (void) const { return m_iExtraValuePerCharge; }
        CWeaponFireDesc *GetWeaponFireDesc (CItemCtx &Ctx, CString *retsError = NULL) const;
		inline bool HasOnRefuelCode (void) const { return FindEventHandlerItemType(evtOnRefuel); }
		inline bool HasOnInstallCode (void) const { return FindEventHandlerItemType(evtOnInstall); }
		bool IsAmmunition (void) const;
		inline bool IsArmor (void) const { return (m_pArmor != NULL); }
		inline bool IsDevice (void) const { return (m_pDevice != NULL); }
		inline bool IsKnown (void) const { return (m_fKnown ? true : false); }
		bool IsFuel (void) const;
		bool IsMissile (void) const;
		inline bool IsScalable (void) const { return (m_fScalable ? true : false); }
		inline bool IsUsable (void) const { return GetUseDesc(NULL); }
		inline void SetKnown (bool bKnown = true) { m_fKnown = bKnown; }
		inline void SetShowReference (void) { m_fReference = true; }
		inline bool ShowChargesInUseMenu (void) const { return (m_fShowChargesInUseMenu ? true : false); }
		inline bool ShowReference (void) const { return (m_fReference ? true : false); }

		//	CDesignType overrides
		static CItemType *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designItemType) ? (CItemType *)pType : NULL); }
		virtual bool FindDataField (const CString &sField, CString *retsValue) const override;
		virtual int GetLevel (int *retiMinLevel = NULL, int *retiMaxLevel = NULL) const override { if (retiMinLevel) *retiMinLevel = m_iLevel; if (retiMaxLevel) *retiMaxLevel = m_iMaxLevel; return m_iLevel; }
		virtual CString GetNamePattern (DWORD dwNounFormFlags = 0, DWORD *retdwFlags = NULL) const override;
		virtual CCurrencyAndValue GetTradePrice (CSpaceObject *pObj = NULL, bool bActual = false) const override;
		virtual DesignTypes GetType (void) const override { return designItemType; }
		virtual bool IsVirtual (void) const override { return (m_fVirtual ? true : false); }

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
        virtual ALERROR OnFinishBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ICCItemPtr OnGetProperty (CCodeChainCtx &Ctx, const CString &sProperty) const override;
		virtual bool OnHasSpecialAttribute (const CString &sAttrib) const override;
		virtual ALERROR OnPrepareBindDesign (SDesignLoadCtx &Ctx) override;
		virtual void OnReadFromStream (SUniverseLoadCtx &Ctx) override;
		virtual void OnReinit (void) override;
		virtual void OnWriteToStream (IWriteStream *pStream) override;

	private:
		void CreateFlotsamImage (void);
		CStationType *GetFlotsamStationType (void);
		CString GetUnknownName (int iIndex, DWORD *retdwFlags = NULL);
		inline CDesignType *GetUseScreen (CString *retsName) const;
		void InitRandomNames (void);
		void InitComponents (void);

		CString m_sName;						//	Full name of item
		DWORD m_dwNameFlags;					//	Name flags
		CString m_sUnknownName;					//	Name of item when unknown (may be NULL)
		CString m_sSortName;					//	Name to sort by

		int m_iLevel;							//	Level of item
        int m_iMaxLevel;                        //  Max level, for scalable items
		CCurrencyAndValue m_iValue;				//	Value in some currency
		int m_iMass;							//	Mass in kilograms
		FrequencyTypes m_Frequency;				//	Frequency
		DiceRange m_NumberAppearing;			//	Number appearing

		CObjectImageArray m_Image;				//	Image of item
		CString m_sDescription;					//	Description of item
		CItemTypeRef m_pUnknownType;			//	Type to show if not known
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

		DWORD m_fRandomDamaged:1;				//	Randomly damaged when found
		DWORD m_fKnown:1;						//	Is this type known?
		DWORD m_fReference:1;					//	Does this type show reference info?
		DWORD m_fDefaultReference:1;			//	TRUE if this shows reference by default
		DWORD m_fInstanceData:1;				//	TRUE if we need to set instance data at create time
		DWORD m_fVirtual:1;						//	TRUE if this is a virtual item needed for a weapon that invokes
		DWORD m_fUseInstalled:1;				//	If TRUE, item can only be used when installed
		DWORD m_fValueCharges:1;				//	TRUE if value should be adjusted based on charges

		DWORD m_fUseUninstalled:1;				//	If TRUE, item can only be used when uninstalled
		DWORD m_fUseEnabled:1;					//	If TRUE, item can only be used when enabled
		DWORD m_fScalable:1;                    //  If TRUE, VariantHigh adds to level.
		DWORD m_fUseCompleteArmor:1;			//	If TRUE, item can be used as a complete armor set
		DWORD m_fUseAsArmorSet:1;				//	If TRUE, we only show item once for armor set
		DWORD m_fAmmoCharges:1;					//	If TRUE, charges are ammo
		DWORD m_fNoSaleIfUsed:1;				//	If TRUE, cannot be sold once it's been used
		DWORD m_fShowChargesInUseMenu:1;		//	If TRUE, the use menu shows charges instead of a count.

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

		inline void AddItems (SItemAddCtx &Ctx) { if (m_pGenerator) m_pGenerator->AddItems(Ctx); }
		inline CurrencyValue GetAverageValue (int iLevel) const { return (m_pGenerator ? m_pGenerator->GetAverageValue(iLevel) : 0); }
		inline IItemGenerator *GetGenerator (void) { return m_pGenerator; }
		inline CItemTypeProbabilityTable GetProbabilityTable (SItemAddCtx &Ctx) const { return m_pGenerator->GetProbabilityTable(Ctx); }
		inline bool HasItemAttribute (const CString &sAttrib) const { return (m_pGenerator ? m_pGenerator->HasItemAttribute(sAttrib) : false); }

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

