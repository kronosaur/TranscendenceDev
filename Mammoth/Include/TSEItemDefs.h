//	TSEItemDefs.h
//
//	Definitions required for items.
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

enum ItemCategories
	{
	itemcatNone =			0xffffffff,

	itemcatMisc =			0x00000001,		//	General item with no object UNID
	itemcatArmor =			0x00000002,		//	Armor items
	itemcatWeapon =			0x00000004,		//	Primary weapons
	itemcatMiscDevice =		0x00000008,		//	Misc device
	itemcatLauncher =		0x00000010,		//	Launcher
	itemcatNano =			0x00000020,
	itemcatReactor =		0x00000040,		//	Reactor device
	itemcatShields =		0x00000080,		//	Shield device
	itemcatCargoHold =		0x00000100,		//	Cargo expansion device
	itemcatFuel =			0x00000200,		//	Fuel items
	itemcatMissile =		0x00000400,		//	Missiles
	itemcatDrive =			0x00000800,		//	Drive device
	itemcatUseful =			0x00001000,		//	Usable item

	itemcatWeaponMask =		0x00000014,		//	Either primary or launcher
	itemcatDeviceMask =		0x000009DC,		//	Any device

	itemcatCount =			13,				//	Total count
	};

enum ItemFates
	{
	fateNone,							//	Default handling when object is destroyed

	fateSurvives,						//	Item always survives
	fateDamaged,						//	Item is always damaged
	fateDestroyed,						//	Item is always destroyed
	fateComponetized,					//	Item is always reduced to components
	};

class CItemCriteria
	{
	public:
		static constexpr DWORD NONE = 0x00000000;
		static constexpr DWORD ALL = 0x00000001;

		CItemCriteria (void) { }
		explicit CItemCriteria (DWORD dwSpecial);
		explicit CItemCriteria (const CString &sCriteria, DWORD dwDefault = NONE);

		bool operator== (const CItemCriteria &Src) const;
		bool operator!= (const CItemCriteria &Src) const { return !(*this == Src); }

		CString AsString (void) const;
		bool ExcludesVirtual (void) const { return m_bExcludeVirtual; }
		bool GetExplicitLevelMatched (int *retiMin, int *retiMax) const;
		ICCItem *GetFilter (void) const { return m_pFilter; }
		const CString &GetLookup (void) const { return m_sLookup; }
		int GetMaxLevelMatched (void) const;
		CString GetName (void) const;
		const CItemCriteria &GetORExpression (void) const { return (m_pOr ? *m_pOr : m_Null); }
		bool HasORExpression (void) const { return m_pOr != NULL; }
		void Init (DWORD dwSpecial = NONE) { *this = CItemCriteria(dwSpecial); }
		void Init (const CString &sCriteria, DWORD dwSpecial = NONE) { *this = CItemCriteria(sCriteria, dwSpecial); }
		bool Intersects (const CItemCriteria &Src) const;
		bool IsEmpty (void) const { return (m_dwItemCategories == 0 && m_sLookup.IsBlank() && !m_pFilter); }
		bool MatchesAttributes (const CItem &Item) const;
		bool MatchesFlags (DWORD dwFlags) const;
		bool MatchesFrequency (FrequencyTypes iFreq) const;
		bool MatchesInstalledOnly (void) const { return m_bInstalledOnly; }
		bool MatchesItemCategory (ItemCategories iCategory) const { return ((m_dwItemCategories & iCategory) && !(m_dwExcludeCategories & iCategory)); }
		bool MatchesItemCategory (const CItemType &ItemType) const;
		bool MatchesLauncherMissilesOnly (void) const { return m_bLauncherMissileOnly; }
		bool MatchesLevel (int iLevel) const;
		bool MatchesMass (int iMassKg) const;
		bool MatchesNotInstalledOnly (void) const { return m_bNotInstalledOnly; }
		bool MatchesPrice (CurrencyValue iValue) const;
		bool MatchesUsableOnly (void) const { return m_bUsableItemsOnly; }
		void SetExcludeVirtual (bool bExclude = true) { m_bExcludeVirtual = bExclude; }
		void SetFilter (ICCItemPtr pFilter) { m_pFilter = pFilter; }

	private:
		void ParseSubExpression (const char *pPos, const char **retpPos = NULL);
		void WriteSubExpression (CMemoryWriteStream &Output) const;
		static void WriteCategoryFlags (CMemoryWriteStream &Output, DWORD dwCategories);

		DWORD m_dwItemCategories = 0;				//	Set of ItemCategories to match on
		DWORD m_dwExcludeCategories = 0;			//	Categories to exclude
		DWORD m_dwMustHaveCategories = 0;			//	ANDed categories

		WORD m_wFlagsMustBeSet = 0;					//	These flags must be set
		WORD m_wFlagsMustBeCleared = 0;				//	These flags must be cleared

		bool m_bUsableItemsOnly = false;			//	Item must be usable
		bool m_bExcludeVirtual = true;				//	Exclude virtual items
		bool m_bInstalledOnly = false;				//	Item must be installed
		bool m_bNotInstalledOnly = false;			//	Item must not be installed
		bool m_bLauncherMissileOnly = false;		//	Item must be a missile for a launcher

		TArray<CString> m_ModifiersRequired;		//	Required modifiers
		TArray<CString> m_ModifiersNotAllowed;		//	Exclude these modifiers
		TArray<CString> m_SpecialAttribRequired;	//	Special required attributes
		TArray<CString> m_SpecialAttribNotAllowed;	//	Exclude these special attributes
		CString m_Frequency;						//	If not blank, only items with these frequencies

		int m_iEqualToLevel = -1;					//	If not -1, only items of this level
		int m_iGreaterThanLevel = -1;				//	If not -1, only items greater than this level
		int m_iLessThanLevel = -1;					//	If not -1, only items less than this level
		int m_iEqualToPrice = -1;					//	If not -1, only items at this price
		int m_iGreaterThanPrice = -1;				//	If not -1, only items greater than this price
		int m_iLessThanPrice = -1;					//	If not -1, only items less than this price
		int m_iEqualToMass = -1;					//	If not -1, only items of this mass (in kg)
		int m_iGreaterThanMass = -1;				//	If not -1, only items greater than this mass (in kg)
		int m_iLessThanMass = -1;					//	If not -1, only items less than this mass (in kg)

		CString m_sLookup;							//	Look up a shared criteria
		ICCItemPtr m_pFilter;						//	Filter returns Nil for excluded items

		TUniquePtr<CItemCriteria> m_pOr;			//	OR sub-expression

		static const CItemCriteria m_Null;
	};

enum EDisplayAttributeTypes
	{
	attribNeutral,

	attribPositive,
	attribNegative,

	attribEnhancement,
	attribWeakness,
	};

enum EAttributeTypes
	{
	attribTypeLocation,
	attribTypeItem,
	};

struct SDisplayAttribute
	{
	SDisplayAttribute (void) :
			iType(attribNeutral)
		{ }

	SDisplayAttribute (EDisplayAttributeTypes iTypeCons, const CString &sTextCons, const CString &sIDCons) :
			iType(iTypeCons),
			sText(sTextCons),
			sID(sIDCons)
		{ }

	SDisplayAttribute (EDisplayAttributeTypes iTypeCons, const CString &sTextCons, bool bDueToEnhancement = false) :
			iType(iTypeCons),
			sText(sTextCons)
		{
		if (bDueToEnhancement)
			{
			switch (iType)
				{
				case attribPositive:
					iType = attribEnhancement;
					if (*sText.GetASCIIZPointer() != '+')
						sText = strPatternSubst(CONSTLIT("+%s"), sText);
					break;

				case attribNegative:
					iType = attribWeakness;
					if (*sText.GetASCIIZPointer() != '-')
						sText = strPatternSubst(CONSTLIT("-%s"), sText);
					break;
				}
			}
		}

	static SDisplayAttribute FromHPBonus (int iHPBonus)
		{
		if (iHPBonus < 0)
			return SDisplayAttribute(attribNegative, strPatternSubst("-%d%%", -iHPBonus));
		else
			return SDisplayAttribute(attribPositive, strPatternSubst("+%d%%", iHPBonus));
		}

	static SDisplayAttribute FromResistHPBonus (int iHPBonus, const CString &sLabel)
		{
		if (iHPBonus < 0)
			return SDisplayAttribute(attribNegative, strPatternSubst("-%d%% %s", -iHPBonus, sLabel));
		else
			return SDisplayAttribute(attribPositive, strPatternSubst("+%d%% %s", iHPBonus, sLabel));
		}

	static SDisplayAttribute FromResistHPBonus (DamageTypes iDamageType, int iHPBonus)
		{
		if (iHPBonus < 0)
			return SDisplayAttribute(attribNegative, strPatternSubst("-%d%% vs %s", -iHPBonus, GetDamageShortName(iDamageType)));
		else
			return SDisplayAttribute(attribPositive, strPatternSubst("+%d%% vs %s", iHPBonus, GetDamageShortName(iDamageType)));
		}

	static bool HasEnhancement (const TArray<SDisplayAttribute> &List)
		{
		int i;
		for (i = 0; i < List.GetCount(); i++)
			if (List[i].iType == attribEnhancement || List[i].iType == attribWeakness)
				return true;

		return false;
		}

	EDisplayAttributeTypes iType;
	CString sText;
	CString sID;

	RECT rcRect;					//	Reserved for callers
	};

class CDisplayAttributeDefinitions
	{
	public:
		void AccumulateAttributes (const CItem &Item, TArray<SDisplayAttribute> *retList) const;
		void Append (const CDisplayAttributeDefinitions &Attribs);
		inline void DeleteAll (void) { m_Attribs.DeleteAll(); m_ItemAttribs.DeleteAll(); }
		const CItemCriteria *FindCriteriaByID (const CString &sID) const;
		bool FindCriteriaName (const CString &sCriteria, CString *retsName = NULL) const;
		bool FindCriteriaNameByID (const CString &sID, CString *retsName = NULL) const;
		int GetLocationAttribFrequency (const CString &sAttrib) const;
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		bool IsEmpty (void) const { return ((m_Attribs.GetCount() == 0) && (m_ItemAttribs.GetCount() == 0)); }

		static bool InitFromCCItem (ICCItem *pEntry, SDisplayAttribute &Result);

		static const CDisplayAttributeDefinitions Null;

	private:
		struct SItemEntry
			{
			const CDesignType *pSourceType = NULL;	//	Type that defined the display attributes

			CString sID;					//	Optional ID
			CItemCriteria Criteria;
			EDisplayAttributeTypes iType;
			CString sText;					//	Text to display on item
			CString sCriteriaName;			//	Name for the criteria

			ICCItemPtr pOnShow;				//	<OnShow> event
			};

		struct SAttribDesc
			{
			EAttributeTypes iType;
			CString sName;			//	Human readable name

			//	Location attributes

			int iFrequency;			//	% of locations with this attribute. (1-99)
			};

		const SItemEntry *FindByCriteria (const CString &sCriteria) const;
		const SItemEntry *FindByID (const CString &sID) const;

		TSortMap<CString, SAttribDesc> m_Attribs;
		TArray<SItemEntry> m_ItemAttribs;
	};

class CArmorMassDefinitions
	{
	public:
		void Append (const CArmorMassDefinitions &Src);
		inline void DeleteAll (void) { m_Definitions.DeleteAll(); InvalidateIDIndex(); }
		bool FindPreviousMassClass (const CString &sID, CString *retsPrevID = NULL, int *retiPrevMass = NULL) const;
		Metric GetFrequencyMax (const CString &sID) const;
		const CString &GetMassClassID (const CItem &Item) const;
		const CString &GetMassClassLabel (const CString &sID) const;
		int GetMassClassMass (const CString &sID) const;
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		inline bool IsEmpty (void) const { return (m_Definitions.GetCount() == 0); }
		void OnBindArmor (SDesignLoadCtx &Ctx, const CItem &Item, CString *retsMassClass = NULL);
		void OnInitDone (void);

		static const CArmorMassDefinitions Null;

	private:

		struct SArmorMassEntry
			{
			CString sDefinition;			//	Index to m_Definitions

			CString sID;					//	Required ID
			int iMaxMass = 0;				//	Maximum mass (kg)
			CString sText;					//	Text to display on item

			int iCount = 0;					//	Number of armor types for this mass
			};

		struct SArmorMassDefinition
			{
			CItemCriteria Criteria;			//	Criteria for armor
			TSortMap<int, SArmorMassEntry> Classes;
			};

		inline const SArmorMassEntry *FindMassEntry (const CItem &Item) const { return const_cast<CArmorMassDefinitions *>(this)->FindMassEntryActual(Item); }
		SArmorMassEntry *FindMassEntryActual (const CItem &Item);
		inline void InvalidateIDIndex (void) { m_ByID.DeleteAll(); }

		TSortMap<CString, SArmorMassDefinition> m_Definitions;
		TSortMap<CString, SArmorMassEntry *> m_ByID;
	};
