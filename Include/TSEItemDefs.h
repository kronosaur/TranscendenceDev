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

struct CItemCriteria
	{
	CItemCriteria (void);
	CItemCriteria (const CItemCriteria &Copy);
	~CItemCriteria (void);

	CItemCriteria &operator= (const CItemCriteria &Copy);

	bool GetExplicitLevelMatched (int *retiMin, int *retiMax) const;
	int GetMaxLevelMatched (void) const;
	CString GetName (void) const;
    inline bool MatchesItemCategory (ItemCategories iCategory) { return ((dwItemCategories & iCategory) && !(dwExcludeCategories & iCategory)); }

	DWORD dwItemCategories;			//	Set of ItemCategories to match on
	DWORD dwExcludeCategories;		//	Categories to exclude
	DWORD dwMustHaveCategories;		//	ANDed categories

	WORD wFlagsMustBeSet;			//	These flags must be set
	WORD wFlagsMustBeCleared;		//	These flags must be cleared

	bool bUsableItemsOnly;			//	Item must be usable
	bool bExcludeVirtual;			//	Exclude virtual items
	bool bInstalledOnly;			//	Item must be installed
	bool bNotInstalledOnly;			//	Item must not be installed
	bool bLauncherMissileOnly;		//	Item must be a missile for a launcher

	TArray<CString> ModifiersRequired;		//	Required modifiers
	TArray<CString> ModifiersNotAllowed;	//	Exclude these modifiers
	TArray<CString> SpecialAttribRequired;	//	Special required attributes
	TArray<CString> SpecialAttribNotAllowed;//	Exclude these special attributes
	CString Frequency;				//	If not blank, only items with these frequencies

	int iEqualToLevel;				//	If not -1, only items of this level
	int iGreaterThanLevel;			//	If not -1, only items greater than this level
	int iLessThanLevel;				//	If not -1, only items less than this level
	int iEqualToPrice;				//	If not -1, only items at this price
	int iGreaterThanPrice;			//	If not -1, only items greater than this price
	int iLessThanPrice;				//	If not -1, only items less than this price
	int iEqualToMass;				//	If not -1, only items of this mass (in kg)
	int iGreaterThanMass;			//	If not -1, only items greater than this mass (in kg)
	int iLessThanMass;				//	If not -1, only items less than this mass (in kg)

	CString sLookup;				//	Look up a shared criteria
	ICCItem *pFilter;				//	Filter returns Nil for excluded items
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
		inline bool IsEmpty (void) const { return ((m_Attribs.GetCount() == 0) && (m_ItemAttribs.GetCount() == 0)); }

		static bool InitFromCCItem (ICCItem *pEntry, SDisplayAttribute &Result);

		static const CDisplayAttributeDefinitions Null;

	private:
		struct SItemEntry
			{
			CString sID;					//	Optional ID
			CItemCriteria Criteria;
			EDisplayAttributeTypes iType;
			CString sText;					//	Text to display on item
			CString sCriteriaName;			//	Name for the criteria
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

