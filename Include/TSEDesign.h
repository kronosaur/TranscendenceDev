//	TSEDesign.h
//
//	Transcendence design classes
//	Copyright 2012 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CCommunicationsHandler;
class CCompositeImageDesc;
class CCreatePainterCtx;
class CCurrencyAndValue;
class CDockScreen;
class CDockingPorts;
class CDynamicDesignTable;
class CEffect;
class CGameStats;
class CGenericType;
class CItem;
class CItemCtx;
class CItemEnhancementStack;
class CObjectImageArray;
class COrbit;
class COrderList;
class CMultiverseCollection;
class CMultiverseCatalogEntry;
class CShipClass;
class CSystem;
class CSystemMap;
class CTopology;
class CTopologyDescTable;
class CTradingDesc;
class IDeviceGenerator;
class IPlayerController;
struct SDestroyCtx;
struct SSystemCreateCtx;

//	Constants & Enums

const int MAX_OBJECT_LEVEL =			25;	//	Max level for space objects
const int MAX_ITEM_LEVEL =				25;	//	Max level for items

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

enum ETradeServiceTypes
	{
	serviceNone =						0,

	serviceBuy =						1,	//	Object buys item from the player
	serviceSell =						2,	//	Object sells item to the player
	serviceAcceptDonations =			3,
	serviceRefuel =						4,
	serviceRepairArmor =				5,
	serviceReplaceArmor =				6,
	serviceInstallDevice =				7,
	serviceRemoveDevice =				8,
	serviceUpgradeDevice =				9,
	serviceEnhanceItem =				10,
	serviceRepairItem =					11,
	serviceCustom =						12,
	serviceBuyShip =					13,	//	Object buys ship from the player
	serviceSellShip =					14,	//	Object sells ship to the player


	serviceCount =						15,
	};

struct STradeServiceCtx
	{
	STradeServiceCtx (void) :
			iService(serviceNone),
			pProvider(NULL),
			pCurrency(NULL),
			iCount(0),
			pItem(NULL),
			pObj(NULL)
		{ }

	ETradeServiceTypes iService;	//	Service
	CSpaceObject *pProvider;		//	Object providing the service
	CEconomyType *pCurrency;		//	Currency to use

	int iCount;						//	Number of items
	const CItem *pItem;				//	For item-based services
	CSpaceObject *pObj;				//	For obj-based services (e.g., serviceSellShip)
	};

//	CFormulaText

class CFormulaText
	{
	public:
		int EvalAsInteger (CSpaceObject *pSource, CString *retsPrefix = NULL, CString *retsSuffix = NULL, CString *retsError = NULL) const;
		inline void InitFromInteger (int iValue) { m_sText = strFromInt(iValue); }
		ALERROR InitFromString (SDesignLoadCtx &Ctx, const CString &sText);
		inline bool IsEmpty (void) const { return m_sText.IsBlank(); }
		void ReadFromStream (SLoadCtx &Ctx);
		inline void SetInteger (int iValue) { m_sText = strFromInt(iValue); }
		void WriteToStream (IWriteStream *pStream) const;

	private:
		CString m_sText;
	};

//	Item Criteria --------------------------------------------------------------

struct CItemCriteria
	{
	CItemCriteria (void);
	CItemCriteria (const CItemCriteria &Copy);
	~CItemCriteria (void);

	CItemCriteria &operator= (const CItemCriteria &Copy);

	bool GetExplicitLevelMatched (int *retiMin, int *retiMax) const;
	int GetMaxLevelMatched (void) const;
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

	RECT rcRect;					//	Reserved for callers
	};

class CDisplayAttributeDefinitions
	{
	public:
		void AccumulateAttributes (const CItem &Item, TArray<SDisplayAttribute> *retList) const;
		void Append (const CDisplayAttributeDefinitions &Attribs);
		inline void DeleteAll (void) { m_Attribs.DeleteAll(); m_ItemAttribs.DeleteAll(); }
		int GetLocationAttribFrequency (const CString &sAttrib) const;
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		inline bool IsEmpty (void) const { return ((m_Attribs.GetCount() == 0) && (m_ItemAttribs.GetCount() == 0)); }

		static bool InitFromCCItem (ICCItem *pEntry, SDisplayAttribute &Result);

	private:
		struct SItemEntry
			{
			CItemCriteria Criteria;
			EDisplayAttributeTypes iType;
			CString sText;
			};

		struct SAttribDesc
			{
			EAttributeTypes iType;
			CString sName;			//	Human readable name

			//	Location attributes

			int iFrequency;			//	% of locations with this attribute. (1-99)
			};

		TSortMap<CString, SAttribDesc> m_Attribs;
		TArray<SItemEntry> m_ItemAttribs;
	};

//	Base Design Type ----------------------------------------------------------
//
//	To add a new DesignType:
//
//	[ ]	Add it to DesignTypes enum
//	[ ] Increment designCount
//	[ ] Add a char??? entry
//	[ ] Add entry to DESIGN_CHAR in CDesignType (make sure it matches the char??? entry)
//	[ ] Add entry to DESIGN_CLASS_NAME in CDesignType
//	[ ] Add case to CDesignTypeCriteria::ParseCriteria (if type should be enumerable)
//	[ ] Add constructor call to CDesignType::CreateFromXML

enum DesignTypes
	{
	designItemType =					0,
	designItemTable =					1,
	designShipClass =					2,
	designOverlayType =				    3,
	designSystemType =					4,
	designStationType =					5,
	designSovereign =					6,
	designDockScreen =					7,
	designEffectType =					8,
	designPower =						9,

	designSpaceEnvironmentType =		10,
	designShipTable =					11,
	designAdventureDesc =				12,
	designGlobals =						13,
	designImage =						14,
	designMusic =						15,
	designMissionType =					16,
	designSystemTable =					17,
	designSystemMap =					18,
	designNameGenerator =				19,

	designEconomyType =					20,
	designTemplateType =				21,
	designGenericType =					22,
	designImageComposite =				23,
	designSound =						24,

	designCount	=						25, 

	designSetAll =						0xffffffff,
	charEconomyType =					'$',
	charAdventureDesc =					'a',
	charItemTable =						'b',
	charEffectType =					'c',
	charDockScreen =					'd',
	charSpaceEnvironmentType =			'e',
	charOverlayType =				    'f',
	charGlobals =						'g',
	charShipTable =						'h',
	charItemType =						'i',
	charSound =							'j',
	//	k
	//	l
	charImage =							'm',
	charMissionType =					'n',
	charImageComposite =				'o',
	charPower =							'p',
	charSystemTable =					'q',
	//	r
	charShipClass =						's',
	charStationType =					't',
	charMusic =							'u',
	charSovereign =						'v',
	charNameGenerator =					'w',
	charGenericType =					'x',

	charSystemType =					'y',
	charSystemMap =						'z',
	charTemplateType =					'_',
	};

class CDesignTypeCriteria
	{
	public:
		CDesignTypeCriteria (void);

		CString AsString (void) const;
		inline bool ChecksLevel (void) const { return (m_iGreaterThanLevel != INVALID_COMPARE || m_iLessThanLevel != INVALID_COMPARE); }
		inline const CString &GetExcludedAttrib (int iIndex) const { return m_sExclude[iIndex]; }
		inline int GetExcludedAttribCount (void) const { return m_sExclude.GetCount(); }
		inline const CString &GetExcludedSpecialAttrib (int iIndex) const { return m_sExcludeSpecial[iIndex]; }
		inline int GetExcludedSpecialAttribCount (void) const { return m_sExcludeSpecial.GetCount(); }
		inline const CString &GetRequiredAttrib (int iIndex) const { return m_sRequire[iIndex]; }
		inline int GetRequiredAttribCount (void) const { return m_sRequire.GetCount(); }
		inline const CString &GetRequiredSpecialAttrib (int iIndex) const { return m_sRequireSpecial[iIndex]; }
		inline int GetRequiredSpecialAttribCount (void) const { return m_sRequireSpecial.GetCount(); }
        inline void IncludeType (DesignTypes iType) { m_dwTypeSet |= (1 << iType); }
		inline bool IncludesVirtual (void) const { return m_bIncludeVirtual; }
        inline bool IsEmpty (void) const { return (m_dwTypeSet == 0); }
		inline bool MatchesDesignType (DesignTypes iType) const
			{ return ((m_dwTypeSet & (1 << iType)) ? true : false); }
		bool MatchesLevel (int iMinLevel, int iMaxLevel) const;
		void ReadFromStream (SLoadCtx &Ctx);
		inline bool StructuresOnly (void) const { return m_bStructuresOnly; }
		void WriteToStream (IWriteStream *pStream);

		static ALERROR ParseCriteria (const CString &sCriteria, CDesignTypeCriteria *retCriteria);

	private:
		enum Flags
			{
			INVALID_COMPARE = -1000,
			};

		DWORD m_dwTypeSet;
		TArray<CString> m_sRequire;
		TArray<CString> m_sExclude;
		TArray<CString> m_sRequireSpecial;
		TArray<CString> m_sExcludeSpecial;

		int m_iGreaterThanLevel;
		int m_iLessThanLevel;

		bool m_bIncludeVirtual;
        bool m_bStructuresOnly;
	};

//	CDesignType

class CDesignType
	{
	public:
		enum ECachedHandlers
			{
			evtCanInstallItem			= 0,
			evtCanRemoveItem			= 1,
			evtOnGlobalTypesInit		= 2,
			evtOnObjDestroyed			= 3,
			evtOnSystemObjAttacked		= 4,
			evtOnSystemWeaponFire		= 5,
			evtOnUpdate					= 6,

			evtCount					= 7,
			};

        struct SMapDescriptionCtx
            {
            SMapDescriptionCtx (void) :
                    bShowDestroyed(false),
                    bEnemy(false),
                    bFriend(false)
                { }

            bool bShowDestroyed;
            bool bEnemy;
            bool bFriend;
            };

		CDesignType (void);
		virtual ~CDesignType (void);
		void CreateClone (CDesignType **retpType);
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CDesignType **retpType);

		ALERROR BindDesign (SDesignLoadCtx &Ctx);
		ALERROR ComposeLoadError (SDesignLoadCtx &Ctx, const CString &sError);
		inline ALERROR FinishBindDesign (SDesignLoadCtx &Ctx) { return OnFinishBindDesign(Ctx); }
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, bool bIsOverride = false);
		inline bool IsIncluded (const TArray<DWORD> &ExtensionsIncluded) const { if (m_Extends.GetCount() == 0) return true; else return MatchesExtensions(ExtensionsIncluded); }
		bool MatchesCriteria (const CDesignTypeCriteria &Criteria);
		void MergeType (CDesignType *pSource);
		ALERROR PrepareBindDesign (SDesignLoadCtx &Ctx);
		inline void PrepareReinit (void) { OnPrepareReinit(); }
		void ReadFromStream (SUniverseLoadCtx &Ctx);
		void Reinit (void);
		inline void UnbindDesign (void) { OnUnbindDesign(); }
		void WriteToStream (IWriteStream *pStream);

		inline void AddExternals (TArray<CString> *retExternals) { OnAddExternals(retExternals); }
		void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed);
		static CDesignType *AsType (CDesignType *pType) { return pType; }
		inline void ClearMark (void) { OnClearMark(); }
		inline CEffectCreator *FindEffectCreatorInType (const CString &sUNID) { return OnFindEffectCreator(sUNID); }
		bool FindEventHandler (const CString &sEvent, SEventHandlerDesc *retEvent = NULL) const;
		inline bool FindEventHandler (ECachedHandlers iEvent, SEventHandlerDesc *retEvent = NULL) const { if (retEvent) *retEvent = m_EventsCache[iEvent]; return (m_EventsCache[iEvent].pCode != NULL); }
		bool FindStaticData (const CString &sAttrib, const CString **retpData) const;
		void FireCustomEvent (const CString &sEvent, ECodeChainEvents iEvent = eventNone, ICCItem *pData = NULL, ICCItem **retpResult = NULL);
		bool FireGetCreatePos (CSpaceObject *pBase, CSpaceObject *pTarget, CSpaceObject **retpGate, CVector *retvPos);
		void FireGetGlobalAchievements (CGameStats &Stats);
		bool FireGetGlobalDockScreen (const SEventHandlerDesc &Event, CSpaceObject *pObj, CString *retsScreen, ICCItem **retpData, int *retiPriority);
		bool FireGetGlobalPlayerPriceAdj (const SEventHandlerDesc &Event, STradeServiceCtx &ServiceCtx, ICCItem *pData, int *retiPriceAdj);
		int FireGetGlobalResurrectPotential (void);
		void FireObjCustomEvent (const CString &sEvent, CSpaceObject *pObj, ICCItem **retpResult);
		ALERROR FireOnGlobalDockPaneInit (const SEventHandlerDesc &Event, void *pScreen, DWORD dwScreenUNID, const CString &sScreen, const CString &sPane, CString *retsError);
		void FireOnGlobalMarkImages (const SEventHandlerDesc &Event);
		void FireOnGlobalObjDestroyed (const SEventHandlerDesc &Event, SDestroyCtx &Ctx);
		void FireOnGlobalPlayerBoughtItem (const SEventHandlerDesc &Event, CSpaceObject *pSellerObj, const CItem &Item, const CCurrencyAndValue &Price);
		ALERROR FireOnGlobalPlayerChangedShips (CSpaceObject *pOldShip, CString *retsError = NULL);
		ALERROR FireOnGlobalPlayerEnteredSystem (CString *retsError = NULL);
		ALERROR FireOnGlobalPlayerLeftSystem (CString *retsError = NULL);
		void FireOnGlobalPlayerSoldItem (const SEventHandlerDesc &Event, CSpaceObject *pBuyerObj, const CItem &Item, const CCurrencyAndValue &Price);
		ALERROR FireOnGlobalResurrect (CString *retsError = NULL);
		ALERROR FireOnGlobalSystemCreated (SSystemCreateCtx &SysCreateCtx, CString *retsError = NULL);
		void FireOnGlobalSystemStarted (const SEventHandlerDesc &Event, DWORD dwElapsedTime);
		void FireOnGlobalSystemStopped (const SEventHandlerDesc &Event);
		ALERROR FireOnGlobalTopologyCreated (CString *retsError = NULL);
		ALERROR FireOnGlobalTypesInit (SDesignLoadCtx &Ctx);
		ALERROR FireOnGlobalUniverseCreated (const SEventHandlerDesc &Event);
		ALERROR FireOnGlobalUniverseLoad (const SEventHandlerDesc &Event);
		ALERROR FireOnGlobalUniverseSave (const SEventHandlerDesc &Event);
		void FireOnGlobalUpdate (const SEventHandlerDesc &Event);
		void FireOnRandomEncounter (CSpaceObject *pObj = NULL);
		inline DWORD GetAPIVersion (void) const { return m_dwVersion; }
		inline const CString &GetAttributes (void) { return m_sAttributes; }
		inline CString GetDataField (const CString &sField) const { CString sValue; FindDataField(sField, &sValue); return sValue; }
		inline int GetDataFieldInteger (const CString &sField) { CString sValue; if (FindDataField(sField, &sValue)) return strToInt(sValue, 0, NULL); else return 0; }
		inline const CDisplayAttributeDefinitions &GetDisplayAttributes (void) const { return m_DisplayAttribs; }
		CString GetEntityName (void) const;
		ICCItem *GetEventHandler (const CString &sEvent) const;
		void GetEventHandlers (const CEventHandler **retHandlers, TSortMap<CString, SEventHandlerDesc> *retInheritedHandlers);
		CExtension *GetExtension (void) const { return m_pExtension; }
		inline const CString &GetGlobalData (const CString &sAttrib) { return m_GlobalData.GetData(sAttrib); }
		inline CDesignType *GetInheritFrom (void) const { return m_pInheritFrom; }
		inline CXMLElement *GetLocalScreens (void) const { return m_pLocalScreens; }
        CString GetMapDescription (SMapDescriptionCtx &Ctx) const;
		CString GetNounPhrase (DWORD dwFlags = 0) const;
		ICCItem *GetProperty (CCodeChainCtx &Ctx, const CString &sProperty) const;
		int GetPropertyInteger (const CString &sProperty);
		CString GetPropertyString (const CString &sProperty);
		CXMLElement *GetScreen (const CString &sUNID);
		const CString &GetStaticData (const CString &sAttrib) const;
		CString GetTypeClassName (void) const;
		inline DWORD GetUNID (void) const { return m_dwUNID; }
		inline CXMLElement *GetXMLElement (void) const { return m_pXML; }
		bool HasAttribute (const CString &sAttrib) const;
		inline bool HasEvents (void) const { return !m_Events.IsEmpty() || (m_pInheritFrom && m_pInheritFrom->HasEvents()); }
		inline bool HasLiteralAttribute (const CString &sAttrib) const { return ::HasModifier(m_sAttributes, sAttrib); }
		bool HasSpecialAttribute (const CString &sAttrib) const;
        inline void IncGlobalData (const CString &sAttrib, ICCItem *pValue = NULL, ICCItem **retpNewValue = NULL) { m_GlobalData.IncData(sAttrib, pValue, retpNewValue); }
		void InitCachedEvents (int iCount, char **pszEvents, SEventHandlerDesc *retEvents);
		inline bool IsClone (void) const { return m_bIsClone; }
		inline bool IsModification (void) const { return m_bIsModification; }
		inline void MarkImages (void) { OnMarkImages(); }
		inline void SetGlobalData (const CString &sAttrib, const CString &sData) { m_GlobalData.SetData(sAttrib, sData); }
		inline void SetUNID (DWORD dwUNID) { m_dwUNID = dwUNID; }
		inline void SetXMLElement (CXMLElement *pDesc) { m_pXML = pDesc; }
		inline void Sweep (void) { OnSweep(); }
		inline void TopologyInitialized (void) { OnTopologyInitialized(); }
		bool Translate (CSpaceObject *pObj, const CString &sID, ICCItem *pData, ICCItem **retpResult) const;
		bool TranslateText (CSpaceObject *pObj, const CString &sID, ICCItem *pData, CString *retsText) const;

		static CString GetTypeChar (DesignTypes iType);

		//	CDesignType overrides

		virtual bool FindDataField (const CString &sField, CString *retsValue) const;
		virtual CCommunicationsHandler *GetCommsHandler (void) { return NULL; }
		virtual CEconomyType *GetEconomyType (void) const;
		virtual CTradingDesc *GetTradingDesc (void) const { return NULL; }
        virtual const CCompositeImageDesc &GetTypeImage (void) const;
		virtual CString GetNamePattern (DWORD dwNounFormFlags = 0, DWORD *retdwFlags = NULL) const { if (retdwFlags) *retdwFlags = 0; return GetDataField(CONSTLIT("name")); }
		virtual int GetLevel (int *retiMinLevel = NULL, int *retiMaxLevel = NULL) const { if (retiMinLevel) *retiMinLevel = -1; if (retiMaxLevel) *retiMaxLevel = -1; return -1; }
		virtual DesignTypes GetType (void) const = 0;
		virtual bool IsVirtual (void) const { return false; }

	protected:
		ALERROR AddEventHandler (const CString &sEvent, const CString &sCode, CString *retsError = NULL) { return m_Events.AddEvent(sEvent, sCode, retsError); }
		ICCItem *FindBaseProperty (CCodeChainCtx &Ctx, const CString &sProperty) const;
		bool IsValidLoadXML (const CString &sTag);
		void ReadGlobalData (SUniverseLoadCtx &Ctx);
		void ReportEventError (const CString &sEvent, ICCItem *pError);

		//	CDesignType overrides
		virtual void OnAddExternals (TArray<CString> *retExternals) { }
		virtual void OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) { }
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) { return NOERROR; }
		virtual void OnClearMark (void) { }
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) { return NOERROR; }
		virtual CEffectCreator *OnFindEffectCreator (const CString &sUNID) { return NULL; }
		virtual bool OnFindEventHandler (const CString &sEvent, SEventHandlerDesc *retEvent = NULL) const { return false; }
		virtual ALERROR OnFinishBindDesign (SDesignLoadCtx &Ctx) { return NOERROR; }
		virtual CString OnGetMapDescriptionMain (SMapDescriptionCtx &Ctx) const { return NULL_STR; }
		virtual ICCItem *OnGetProperty (CCodeChainCtx &Ctx, const CString &sProperty) const { return NULL; }
		virtual bool OnHasSpecialAttribute (const CString &sAttrib) const { return sAttrib.IsBlank(); }
		virtual void OnInitFromClone (CDesignType *pSource) { ASSERT(false); }
		virtual void OnMarkImages (void) { }
		virtual void OnMergeType (CDesignType *pSource) { ASSERT(false); }
		virtual ALERROR OnPrepareBindDesign (SDesignLoadCtx &Ctx) { return NOERROR; }
		virtual void OnPrepareReinit (void) { }
		virtual void OnReadFromStream (SUniverseLoadCtx &Ctx) { }
		virtual void OnReinit (void) { }
		virtual void OnSweep (void) { }
		virtual void OnTopologyInitialized (void) { }
		virtual void OnUnbindDesign (void) { }
		virtual void OnWriteToStream (IWriteStream *pStream) { }

	private:
		void AddUniqueHandlers (TSortMap<CString, SEventHandlerDesc> *retInheritedHandlers);
		inline SEventHandlerDesc *GetInheritedCachedEvent (ECachedHandlers iEvent) { return (m_EventsCache[iEvent].pCode ? &m_EventsCache[iEvent] : (m_pInheritFrom ? m_pInheritFrom->GetInheritedCachedEvent(iEvent) : NULL)); }
		void InitCachedEvents (void);
		bool InSelfReference (CDesignType *pType);
		bool MatchesExtensions (const TArray<DWORD> &ExtensionsIncluded) const;
		void MergeLanguageTo (CLanguageDataBlock &Dest);
		bool TranslateVersion2 (CSpaceObject *pObj, const CString &sID, ICCItem **retpResult) const;

		DWORD m_dwUNID;
		CExtension *m_pExtension;				//	Extension
		DWORD m_dwVersion;						//	Extension version
		TArray<DWORD> m_Extends;				//	Exclude this type from bind unless one of these extensions is present
		CXMLElement *m_pXML;					//	Optional XML for this type

		DWORD m_dwInheritFrom;					//	Inherit from this type
		CDesignType *m_pInheritFrom;			//	Inherit from this type

		CString m_sAttributes;					//	Type attributes
		CAttributeDataBlock m_StaticData;		//	Static data
		CAttributeDataBlock m_GlobalData;		//	Global (variable) data
		CAttributeDataBlock m_InitGlobalData;	//	Initial global data
		CLanguageDataBlock m_Language;			//	Language data
		CEventHandler m_Events;					//	Event handlers
		CXMLElement *m_pLocalScreens;			//	Local dock screen
		CDisplayAttributeDefinitions m_DisplayAttribs;	//	Display attribute definitions

		bool m_bIsModification;					//	TRUE if this modifies the type it overrides
		bool m_bIsClone;						//	TRUE if we cloned this from another type

		SEventHandlerDesc m_EventsCache[evtCount];	//	Cached events
	};

template <class CLASS> class CDesignTypeRef
	{
	public:
		CDesignTypeRef (void) : m_pType(NULL), m_dwUNID(0) { }

		inline operator CLASS *() const { return m_pType; }
		inline CLASS * operator->() const { return m_pType; }

		ALERROR Bind (SDesignLoadCtx &Ctx)
			{
			if (m_dwUNID)
				{
				CDesignType *pBaseType = g_pUniverse->FindDesignType(m_dwUNID);
				if (pBaseType == NULL)
					{
					Ctx.sError = strPatternSubst(CONSTLIT("Unknown design type: %x"), m_dwUNID);
					return ERR_FAIL;
					}

				m_pType = CLASS::AsType(pBaseType);
				if (m_pType == NULL)
					{
					Ctx.sError = strPatternSubst(CONSTLIT("Specified type is invalid: %x"), m_dwUNID);
					return ERR_FAIL;
					}
				}

			return NOERROR;
			}

		inline DWORD GetUNID (void) const { return m_dwUNID; }
		ALERROR LoadUNID (SDesignLoadCtx &Ctx, const CString &sUNID, DWORD dwDefault = 0) { if (!sUNID.IsBlank()) return ::LoadUNID(Ctx, sUNID, &m_dwUNID); else { m_dwUNID = dwDefault; return NOERROR; } }

		void Set (CLASS *pType)
			{
			m_pType = pType;
			if (pType)
				m_dwUNID = pType->GetUNID();
			else
				m_dwUNID = 0;
			}

		void SetUNID (DWORD dwUNID)
			{
			if (dwUNID != m_dwUNID)
				{
				m_dwUNID = dwUNID;
				m_pType = NULL;
				}
			}

	protected:
		CLASS *m_pType;
		DWORD m_dwUNID;
	};

//	Design Type References ----------------------------------------------------

class CItemTypeRef : public CDesignTypeRef<CItemType>
	{
	public:
		CItemTypeRef (void) { }
		CItemTypeRef (CItemType *pType) { Set(pType); }
		inline ALERROR Bind (SDesignLoadCtx &Ctx) { return CDesignTypeRef<CItemType>::Bind(Ctx); }
		ALERROR Bind (SDesignLoadCtx &Ctx, ItemCategories iCategory);
	};

class CArmorClassRef : public CDesignTypeRef<CArmorClass>
	{
	public:
		ALERROR Bind (SDesignLoadCtx &Ctx);
	};

class CDeviceClassRef : public CDesignTypeRef<CDeviceClass>
	{
	public:
		ALERROR Bind (SDesignLoadCtx &Ctx);
		void Set (CDeviceClass *pDevice);
	};

class CWeaponFireDescRef : public CDesignTypeRef<CWeaponFireDesc>
	{
	public:
		ALERROR Bind (SDesignLoadCtx &Ctx);
	};

class CDockScreenTypeRef
	{
	public:
		CDockScreenTypeRef (void) : m_pType(NULL), m_pLocal(NULL) { }
		CDockScreenTypeRef (const CString &sUNID) : m_sUNID(sUNID), m_pType(NULL), m_pLocal(NULL) { }

		inline operator CDockScreenType *() const { return m_pType; }
		inline CDockScreenType * operator->() const { return m_pType; }

		ALERROR Bind (SDesignLoadCtx &Ctx, CXMLElement *pLocalScreens = NULL);
		CXMLElement *GetDesc (void) const;
		CDesignType *GetDockScreen (CDesignType *pRoot, CString *retsName) const;
		CString GetStringUNID (CDesignType *pRoot) const;
		inline const CString &GetUNID (void) const { return m_sUNID; }
		inline bool IsEmpty (void) const { return m_sUNID.IsBlank(); }
		void LoadUNID (SDesignLoadCtx &Ctx, const CString &sUNID);

		ALERROR Bind (CXMLElement *pLocalScreens = NULL);
		inline void LoadUNID (const CString &sUNID) { m_sUNID = sUNID; }

	private:
		CString m_sUNID;
		CDockScreenType *m_pType;
		CXMLElement *m_pLocal;
	};

class CEconomyTypeRef
	{
	public:
		CEconomyTypeRef (void) : m_pType(NULL) { }

		inline operator CEconomyType *() const { return m_pType; }
		inline CEconomyType * operator->() const { return m_pType; }

		ALERROR Bind (SDesignLoadCtx &Ctx);
		inline bool IsEmpty (void) const { return (m_sUNID.IsBlank() && m_pType == NULL); }
		void LoadUNID (const CString &sUNID) { m_sUNID = sUNID; }
		void Set (DWORD dwUNID);
		inline void Set (CEconomyType *pType) { m_pType = pType; }

	private:
		CString m_sUNID;
		CEconomyType *m_pType;
	};

class CEffectCreatorRef : public CDesignTypeRef<CEffectCreator>
	{
	public:
		CEffectCreatorRef (void) : m_pSingleton(NULL), m_bDelete(false) { }
		CEffectCreatorRef (const CEffectCreatorRef &Source);
		~CEffectCreatorRef (void);

		CEffectCreatorRef &operator= (const CEffectCreatorRef &Source);

		ALERROR Bind (SDesignLoadCtx &Ctx);
		ALERROR CreateBeamEffect (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID);
		ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID);
		IEffectPainter *CreatePainter (CCreatePainterCtx &Ctx, CEffectCreator *pDefaultCreator = NULL);
		inline bool IsEmpty (void) const { return (m_dwUNID == 0 && m_pType == NULL); }
		ALERROR LoadEffect (SDesignLoadCtx &Ctx, const CString &sUNID, CXMLElement *pDesc, const CString &sAttrib);
		ALERROR LoadSimpleEffect (SDesignLoadCtx &Ctx, const CString &sUNID, CXMLElement *pDesc);
		void Set (CEffectCreator *pEffect);

	private:
        CAttributeDataBlock m_Data;
		IEffectPainter *m_pSingleton;
		bool m_bDelete;
	};

class CItemTableRef : public CDesignTypeRef<CItemTable>
	{
	};

class CGenericTypeRef : public CDesignTypeRef<CGenericType>
	{
	};

class COverlayTypeRef : public CDesignTypeRef<COverlayType>
	{
	};

class CShipClassRef : public CDesignTypeRef<CShipClass>
	{
	};

class CShipTableRef : public CDesignTypeRef<CShipTable>
	{
	};

class CSovereignRef : public CDesignTypeRef<CSovereign>
	{
	};

class CStationTypeRef : public CDesignTypeRef<CStationType>
	{
	};

class CSystemMapRef : public CDesignTypeRef<CSystemMap>
	{
	};

//	Classes and structs

class CCurrencyAndValue
	{
	public:
		CCurrencyAndValue (CurrencyValue iValue = 0, CEconomyType *pCurrency = NULL);

		void Add (const CCurrencyAndValue &Value);
		inline void Adjust (int iAdj) { m_iValue = iAdj * m_iValue / 100; }
		ALERROR Bind (SDesignLoadCtx &Ctx);
		CurrencyValue GetCreditValue (void) const;
		inline CEconomyType *GetCurrencyType (void) const { return m_pCurrency; }
		inline CurrencyValue GetValue (void) const { return m_iValue; }
		inline void Init (CurrencyValue iValue, const CString &sUNID = NULL_STR) { m_iValue = iValue; m_pCurrency.LoadUNID(sUNID); }
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, const CString &sDesc);
		inline bool IsEmpty (void) const { return (m_pCurrency.IsEmpty() && m_iValue == 0); }
		inline void SetCurrencyType (CEconomyType *pType) { m_pCurrency.Set(pType); }
		inline void SetValue (CurrencyValue iValue) { m_iValue = iValue; }

	private:
		CurrencyValue m_iValue;
		CEconomyTypeRef m_pCurrency;
	};

class CCurrencyAndRange
	{
	public:
		ALERROR Bind (SDesignLoadCtx &Ctx);
		inline CEconomyType *GetCurrencyType (void) const { return m_pCurrency; }
		inline const DiceRange &GetDiceRange (void) const { return m_Value; }
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, const CString &sDesc);
		inline CurrencyValue Roll (void) const { return m_Value.Roll(); }

	private:
		DiceRange m_Value;
		CEconomyTypeRef m_pCurrency;
	};

struct SViewportPaintCtx
	{
	SViewportPaintCtx (void) :
			pCenter(NULL),
			xCenter(0),
			yCenter(0),
			pObj(NULL),
			iPerception(4),				//	LATER: Same as CSpaceObject::perceptNormal (but we haven't included it yet).
			rgbSpaceColor(CG32bitPixel::Null()),
			pStar(NULL),
			pVolumetricMask(NULL),
			pThreadPool(NULL),
			fNoSelection(false),
			fNoRecon(false),
			fNoDockedShips(false),
			fEnhancedDisplay(false),
			fNoStarfield(false),
			fShowManeuverEffects(false),
			fNoStarshine(false),
			fNoSpaceBackground(false),
            fShowSatellites(false),
			bInFront(false),
			bFade(false),
			iTick(0),
			iVariant(0),
			iDestiny(0),
			iRotation(0),
			iMaxLength(-1),
			iStartFade(0)
		{ }

	inline void Save (void)
		{
		SVariants *pFrame = m_SaveStack.Insert();

		pFrame->bFade = bFade;
		pFrame->bInFront = bInFront;
		pFrame->iDestiny = iDestiny;
		pFrame->iMaxLength = iMaxLength;
		pFrame->iRotation = iRotation;
		pFrame->iTick = iTick;
		pFrame->iVariant = iVariant;
		}

	inline void Restore (void)
		{
		int iLastIndex = m_SaveStack.GetCount() - 1;
		if (iLastIndex >= 0)
			{
			SVariants *pFrame = &m_SaveStack[iLastIndex];

			bFade = pFrame->bFade;
			bInFront = pFrame->bInFront;
			iDestiny = pFrame->iDestiny;
			iMaxLength = pFrame->iMaxLength;
			iRotation = pFrame->iRotation;
			iTick = pFrame->iTick;
			iVariant = pFrame->iVariant;

			m_SaveStack.Delete(iLastIndex);
			}
		}

	//	Viewport metrics

	CSpaceObject *pCenter;				//	Center object (viewport perspective)
	CVector vCenterPos;					//	Center of viewport
	RECT rcView;						//	Viewport
	int xCenter;						//	Center of viewport (pixels)
	int yCenter;						//	Center of viewport (pixels)
	ViewportTransform XForm;			//	Converts from object to screen viewport coordinates
										//		Screen viewport coordinates has positive-Y down.
	ViewportTransform XFormRel;			//	In the case of effects, this Xform has been translated
										//		to offset for the effect position

	CVector vDiagonal;					//	Length of 1/2 viewport diagonal (in global coordinates).
	CVector vUR;						//	upper-right and lower-left of viewport in global
	CVector vLL;						//		coordinates.

	CVector vEnhancedDiagonal;			//	Length of 1/2 enhanced viewport diagonal
	CVector vEnhancedUR;
	CVector vEnhancedLL;

	int iPerception;					//	Perception
	Metric rIndicatorRadius;			//	Radius of circle to show target indicators (in pixels)
	CG32bitPixel rgbSpaceColor;			//	Starshine color
	CSpaceObject *pStar;				//	Nearest star to POV
	const CG8bitSparseImage *pVolumetricMask;	//	Volumetric mask for starshine

	CThreadPool *pThreadPool;			//	Thread pool for painting.

	//	Options

	DWORD fNoSelection:1;
	DWORD fNoRecon:1;
	DWORD fNoDockedShips:1;
	DWORD fEnhancedDisplay:1;
	DWORD fNoStarfield:1;
	DWORD fShowManeuverEffects:1;
	DWORD fNoStarshine:1;
	DWORD fNoSpaceBackground:1;

    DWORD fShowSatellites:1;
    DWORD fSpare2:1;
    DWORD fSpare3:1;
    DWORD fSpare4:1;
    DWORD fSpare5:1;
    DWORD fSpare6:1;
    DWORD fSpare7:1;

	DWORD dwSpare:16;

	CSpaceObject *pObj;					//	Current object being painted
	RECT rcObjBounds;					//	Object bounds in screen coordinates.
	int yAnnotations;					//	Start of free area for annotations (This start at the
										//		bottom of the object bounds, and each annotation
										//		should increment the value appropriately.

	//	May be modified by callers

	bool bInFront;						//	If TRUE, paint elements in front of object (otherwise, behind)
	bool bFade;							//	If TRUE, we're painting a fading element
	int iTick;
	int iVariant;
	int iDestiny;
	int iRotation;						//	An angle 0-359
	int iMaxLength;						//	Max length of object (used for projectiles); -1 == no limit
	int iStartFade;						//	If bFade is TRUE this is the tick on which we started fading

	private:
		struct SVariants
			{
			bool bInFront;
			bool bFade;
			int iTick;
			int iVariant;
			int iDestiny;
			int iRotation;
			int iMaxLength;
			};

		//	Stack of modifications

		TArray<SVariants> m_SaveStack;
	};

class CViewportPaintCtxSmartSave
	{
	public:
		CViewportPaintCtxSmartSave (SViewportPaintCtx &Ctx) : m_Ctx(Ctx)
			{
			m_Ctx.Save();
			}

		~CViewportPaintCtxSmartSave (void)
			{
			m_Ctx.Restore();
			}

	private:
		SViewportPaintCtx &m_Ctx;
	};

class CMapViewportCtx
	{
	public:
		CMapViewportCtx (void);
		CMapViewportCtx (CSpaceObject *pCenter, const RECT &rcView, Metric rMapScale);

		inline CSpaceObject *GetCenterObj (void) const { return m_pCenter; }
		inline const CVector &GetCenterPos (void) const { return m_vCenter; }
		inline const RECT &GetViewportRect (void) const { return m_rcView; }
		inline ViewportTransform &GetXform (void) { return m_Trans; }
		bool IsInViewport (CSpaceObject *pObj) const;
		inline bool IsSpaceBackgroundEnabled (void) const { return !m_bNoSpaceBackground; }
		inline void Set3DMapEnabled (bool bEnabled = true) { m_b3DMap = bEnabled; }
		inline void SetSpaceBackgroundEnabled (bool bEnabled = true) { m_bNoSpaceBackground = !bEnabled; }
		void Transform (const CVector &vPos, int *retx, int *rety) const;

	private:
		CSpaceObject *m_pCenter;		//	Center of viewport (may be NULL)
		CVector m_vCenter;				//	Center of viewport in global coordinate
		RECT m_rcView;					//	RECT of viewport
		Metric m_rMapScale;				//	Map scale (klicks per pixel)

		CVector m_vUR;					//	Upper-right of viewport bounds (in global coordinates)
		CVector m_vLL;					//	Lower-left of viewport bounds
		int m_xCenter;					//	Center of viewport
		int m_yCenter;					//		(in viewport coordinate)

		ViewportTransform m_Trans;		//	Transform

		bool m_b3DMap;					//	Use standard 3D projection
		bool m_bNoSpaceBackground;		//	Do not paint a space background
	};

struct SPlayerChangedShipsCtx
	{
	SPlayerChangedShipsCtx (void) :
			bNoOrderTransfer(false)
		{ }

	bool bNoOrderTransfer;			//	If TRUE, do not change orders to point to new ship
	};

//  Subsidiary classes and structures

#include "TSEImages.h"
#include "TSESounds.h"
#include "TSEComms.h"
#include "TSEWeaponFireDesc.h"
#include "TSEItems.h"
#include "TSEShipSystems.h"
#include "TSEArmor.h"

//	Effect Support Structures

#include "TSEParticleSystem.h"
#include "TSEEffects.h"

//	Generic Type ---------------------------------------------------------------

class CGenericType : public CDesignType
	{
	public:
		//	CDesignType overrides

		static CGenericType *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designGenericType) ? (CGenericType *)pType : NULL); }
		virtual CCommunicationsHandler *GetCommsHandler (void) override { return m_Comms.GetCommsHandler(GetInheritFrom()); }
		virtual DesignTypes GetType (void) const override { return designGenericType; }

	protected:

		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual void OnUnbindDesign (void) override;

	private:
		CCommunicationsStack m_Comms;
	};

//	Topology Descriptors -------------------------------------------------------

const int INFINITE_NODE_DIST =					1000000;
const DWORD END_GAME_SYSTEM_UNID =				0x00ffffff;

struct SRequiredEncounterDesc
	{
	CStationType *pType;					//	Encounter to create
	int iLeftToCreate;						//	Left to create
	};

class CTopologyNode
	{
	public:
		struct SDistanceTo
			{
			TArray<CString> AttribsRequired;
			TArray<CString> AttribsNotAllowed;

			CString sNodeID;

			int iMinDist;
			int iMaxDist;
			};

		struct SCriteriaCtx
			{
			SCriteriaCtx (void) :
					pTopology(NULL)
				{ }

			CTopology *pTopology;
			};

		struct SCriteria
			{
			int iChance;								//	Probability 0-100 of matching criteria
			int iMinStargates;							//	Match if >= this many stargates
			int iMaxStargates;							//	Match if <= this many stargates
			int iMinInterNodeDist;						//	Used by <DistributeNodes> (maybe move there)
			int iMaxInterNodeDist;
			TArray<CString> AttribsRequired;			//	Does not match if any of these attribs are missing
			TArray<CString> AttribsNotAllowed;			//	Does not match if any of these attribs are present
			TArray<SDistanceTo> DistanceTo;				//	Matches if node is within the proper distance of another node or nodes
			TArray<CString> SpecialRequired;			//	Special attributes
			TArray<CString> SpecialNotAllowed;			//	Special attributes
			};

		struct SStargateRouteDesc
			{
			SStargateRouteDesc (void) :
					pFromNode(NULL),
					pToNode(NULL),
					bOneWay(false)
				{ }

			CTopologyNode *pFromNode;
			CString sFromName;

			CTopologyNode *pToNode;
			CString sToName;

			TArray<SPoint> MidPoints;
			bool bOneWay;
			};

		CTopologyNode (const CString &sID, DWORD SystemUNID, CSystemMap *pMap);
		~CTopologyNode (void);
		static void CreateFromStream (SUniverseLoadCtx &Ctx, CTopologyNode **retpNode);

		void AddAttributes (const CString &sAttribs);
		ALERROR AddStargate (const CString &sName, const CString &sDestNode, const CString &sDestEntryPoint, const SStargateRouteDesc &RouteDesc);
		ALERROR AddStargateAndReturn (const CString &sGateID, const CString &sDestNodeID, const CString &sDestGateID);
		int CalcMatchStrength (const CAttributeCriteria &Criteria);
		bool FindStargate (const CString &sName, CString *retsDestNode = NULL, CString *retsEntryPoint = NULL);
		bool FindStargateTo (const CString &sDestNode, CString *retsName = NULL, CString *retsDestGateID = NULL);
		CString FindStargateName (const CString &sDestNode, const CString &sEntryPoint);
		inline const CString &GetAttributes (void) const { return m_sAttributes; }
		inline int GetCalcDistance (void) const { return m_iCalcDistance; }
		inline const CString &GetData (const CString &sAttrib) const { return m_Data.GetData(sAttrib); }
		inline CSystemMap *GetDisplayPos (int *retxPos = NULL, int *retyPos = NULL);
		inline const CString &GetEndGameReason (void) { return m_sEndGameReason; }
		inline const CString &GetEpitaph (void) { return m_sEpitaph; }
		inline const CString &GetID (void) const { return m_sID; }
		CTopologyNode *GetGateDest (const CString &sName, CString *retsEntryPoint = NULL);
        DWORD GetLastVisitedTime (void) const;
		inline int GetLevel (void) { return m_iLevel; }
		ICCItem *GetProperty (const CString &sName);
		inline int GetStargateCount (void) { return m_NamedGates.GetCount(); }
		CString GetStargate (int iIndex);
		CTopologyNode *GetStargateDest (int iIndex, CString *retsEntryPoint = NULL);
		void GetStargateRouteDesc (int iIndex, SStargateRouteDesc *retRouteDesc);
		inline CSystem *GetSystem (void) { return m_pSystem; }
		inline DWORD GetSystemID (void) { return m_dwID; }
		inline const CString &GetSystemName (void) { return m_sName; }
		inline DWORD GetSystemTypeUNID (void) { return m_SystemUNID; }
		inline bool HasAttribute (const CString &sAttrib) { return ::HasModifier(m_sAttributes, sAttrib); }
		bool HasSpecialAttribute (const CString &sAttrib) const;
		inline void IncData (const CString &sAttrib, ICCItem *pValue = NULL, ICCItem **retpNewValue = NULL) { m_Data.IncData(sAttrib, pValue, retpNewValue); }
		ALERROR InitFromAdditionalXML (CXMLElement *pDesc, CString *retsError);
		ALERROR InitFromAttributesXML (CXMLElement *pAttributes, CString *retsError);
		ALERROR InitFromSystemXML (CXMLElement *pSystem, CString *retsError);
		static bool IsCriteriaAll (const SCriteria &Crit);
		inline bool IsEndGame (void) const { return (m_SystemUNID == END_GAME_SYSTEM_UNID); }
		inline bool IsKnown (void) const { return m_bKnown; }
		inline bool IsMarked (void) const { return m_bMarked; }
		bool MatchesCriteria (SCriteriaCtx &Ctx, const SCriteria &Crit);
		inline void SetCalcDistance (int iDist) { m_iCalcDistance = iDist; }
		inline void SetData (const CString &sAttrib, const CString &sData) { m_Data.SetData(sAttrib, sData); }
		inline void SetEndGameReason (const CString &sReason) { m_sEndGameReason = sReason; }
		inline void SetEpitaph (const CString &sEpitaph) { m_sEpitaph = sEpitaph; }
		inline void SetKnown (bool bKnown = true) { m_bKnown = bKnown; }
		inline void SetLevel (int iLevel) { m_iLevel = iLevel; }
		inline void SetMarked (bool bValue = true) { m_bMarked = bValue; }
		inline void SetName (const CString &sName) { m_sName = sName; }
		inline void SetPos (int xPos, int yPos) { m_xPos = xPos; m_yPos = yPos; }
		bool SetProperty (const CString &sName, ICCItem *pValue, CString *retsError);
		void SetStargateDest (const CString &sName, const CString &sDestNode, const CString &sEntryPoint);
		inline void SetSystem (CSystem *pSystem) { m_pSystem = pSystem; }
		inline void SetSystemID (DWORD dwID) { m_dwID = dwID; }
		inline void SetSystemUNID (DWORD dwUNID) { m_SystemUNID = dwUNID; }
		void WriteToStream (IWriteStream *pStream);

		inline void AddVariantLabel (const CString &sVariant) { m_VariantLabels.Insert(sVariant); }
		bool HasVariantLabel (const CString &sVariant);

		static ALERROR CreateStargateRoute (const SStargateRouteDesc &Desc);
		static ALERROR ParseCriteria (CXMLElement *pCrit, SCriteria *retCrit, CString *retsError = NULL);
		static ALERROR ParseCriteria (const CString &sCriteria, SCriteria *retCrit, CString *retsError = NULL);
		static ALERROR ParseCriteriaInt (const CString &sCriteria, SCriteria *retCrit);
		static ALERROR ParsePointList (const CString &sValue, TArray<SPoint> *retPoints);
		static ALERROR ParsePosition (const CString &sValue, int *retx, int *rety);
		static ALERROR ParseStargateString (const CString &sStargate, CString *retsNodeID, CString *retsGateName);

	private:

		struct StarGateDesc
			{
			StarGateDesc (void) :
					fMinor(false),
					pDestNode(NULL)
				{ }

			CString sDestNode;
			CString sDestEntryPoint;

			TArray<SPoint> MidPoints;			//	If the stargate line is curved, these are 
												//	the intermediate points.

			DWORD fMinor:1;						//	If TRUE, this is a minor stargate path

			DWORD fSpare2:1;
			DWORD fSpare3:1;
			DWORD fSpare4:1;
			DWORD fSpare5:1;
			DWORD fSpare6:1;
			DWORD fSpare7:1;
			DWORD fSpare8:1;
			DWORD dwSpare:24;

			CTopologyNode *pDestNode;			//	Cached for efficiency (may be NULL)
			};

		CString GenerateStargateName (void);

		CString m_sID;							//	ID of node

		DWORD m_SystemUNID;						//	UNID of system type
		CString m_sName;						//	Name of system
		int m_iLevel;							//	Level of system

		CSystemMap *m_pMap;						//	May be NULL
		int m_xPos;								//	Position on map (cartessian)
		int m_yPos;

		TSortMap<CString, StarGateDesc> m_NamedGates;	//	Name to StarGateDesc

		CString m_sAttributes;					//	Attributes
		TArray<CString> m_VariantLabels;		//	Variant labels
		CString m_sEpitaph;						//	Epitaph if this is endgame node
		CString m_sEndGameReason;				//	End game reason if this is endgame node

		CAttributeDataBlock m_Data;				//	Opaque data

		CSystem *m_pSystem;						//	NULL if not yet created
		DWORD m_dwID;							//	ID of system instance

		bool m_bKnown;							//	TRUE if node is visible on galactic map
		bool m_bMarked;							//	Temp variable used during painting
		int m_iCalcDistance;					//	Temp variable used during distance calc
	};

class CTopologyNodeList
	{
	public:
		inline CTopologyNode *operator [] (int iIndex) const { return m_List.GetAt(iIndex); }

		void Delete (CTopologyNode *pNode);
		inline void Delete (int iIndex) { m_List.Delete(iIndex); }
		inline void DeleteAll (void) { m_List.DeleteAll(); }
		ALERROR Filter (CTopologyNode::SCriteriaCtx &Ctx, CXMLElement *pCriteria, CTopologyNodeList *retList, CString *retsError);
		ALERROR Filter (CTopologyNode::SCriteriaCtx &Ctx, CTopologyNode::SCriteria &Crit, CTopologyNodeList *ioList);
		bool FindNode (CTopologyNode *pNode, int *retiIndex = NULL);
		bool FindNode (const CString &sID, int *retiIndex = NULL);
		inline CTopologyNode *GetAt (int iIndex) const { return m_List.GetAt(iIndex); }
		inline int GetCount (void) const { return m_List.GetCount(); }
		inline void Insert (CTopologyNode *pNode) { m_List.Insert(pNode); }
		bool IsNodeInRangeOf (CTopologyNode *pNode, int iMin, int iMax, const TArray<CString> &AttribsRequired, const TArray<CString> &AttribsNotAllowed, CTopologyNodeList &Checked);
		void RestoreMarks (TArray<bool> &Saved);
		void SaveAndSetMarks (bool bMark, TArray<bool> *retSaved);
		inline void Shuffle (void) { m_List.Shuffle(); }

	private:
		TArray<CTopologyNode *> m_List;
	};

enum ENodeDescTypes
	{
	ndNode =			0x01,					//	A single node
	ndNodeTable =		0x02,					//	A table of descriptors (each of which can only appear once)
	ndFragment =		0x03,					//	A topology of nodes (used as fragment)
	ndNetwork =			0x04,					//	A network of nodes (used as fragment)
	ndRandom =			0x05,					//	Randomly generated network
	ndNodeGroup =		0x06,					//	A group of nodes with stargates
	};

class CTopologyDesc
	{
	public:
		CTopologyDesc (void);
		~CTopologyDesc (void);
		ALERROR BindDesign (SDesignLoadCtx &Ctx);
		CEffectCreator *FindEffectCreator (const CString &sUNID);
		CString GetAttributes (void);
		inline CXMLElement *GetDesc (void) const { return m_pDesc; }
		inline CEffectCreator *GetLabelEffect (void) const { return m_pLabelEffect; }
		inline CSystemMap *GetMap (void) const { return m_pMap; }
		inline CEffectCreator *GetMapEffect (void) const { return m_pMapEffect; }
		inline const CString &GetID (void) const { return m_sID; }
		bool GetPos (int *retx, int *rety);
		CXMLElement *GetSystem (void);
		inline CTopologyDesc *GetTopologyDesc (int iIndex);
		inline int GetTopologyDescCount (void);
		inline CTopologyDescTable *GetTopologyDescTable (void) { return m_pDescList; }
		inline ENodeDescTypes GetType (void) const { return m_iType; }
		inline bool IsAbsoluteNode (void) const { return (*m_sID.GetASCIIZPointer() != '+'); }
		bool IsEndGameNode (CString *retsEpitaph = NULL, CString *retsReason = NULL) const;
		inline bool IsRootNode (void) const { return ((m_dwFlags & FLAG_IS_ROOT_NODE) ? true : false); }
		ALERROR LoadFromXML (SDesignLoadCtx &Ctx, CXMLElement *pXMLDesc, CSystemMap *pMap, const CString &sParentUNID);
		inline void SetRootNode (void) { m_dwFlags |= FLAG_IS_ROOT_NODE; }

	private:
		enum Flags
			{
			FLAG_IS_ROOT_NODE = 0x00000001,
			};

		CSystemMap *m_pMap;						//	Map that contains this descriptor (may be NULL)
		CString m_sID;							//	ID of node
		ENodeDescTypes m_iType;					//	Type of node
		CXMLElement *m_pDesc;					//	XML for node definition
		DWORD m_dwFlags;

		CEffectCreatorRef m_pLabelEffect;		//	Effect to paint on label layer
		CEffectCreatorRef m_pMapEffect;			//	Effect to paint on galactic map

		CTopologyDescTable *m_pDescList;		//	Some node types (e.g., ndNetwork) have sub-nodes.
												//		This is a table of all subnodes
	};

class CTopologyDescTable
	{
	public:
		CTopologyDescTable (void);
		~CTopologyDescTable (void);

		ALERROR AddRootNode (SDesignLoadCtx &Ctx, const CString &sNodeID);
		ALERROR BindDesign (SDesignLoadCtx &Ctx);
		void CleanUp (void);
		inline void DeleteIDMap (void) { delete m_pIDToDesc; m_pIDToDesc = NULL; }
		CEffectCreator *FindEffectCreator (const CString &sUNID);
		CTopologyDesc *FindTopologyDesc (const CString &sID);
		CXMLElement *FindTopologyDescXML (const CString &sNodeID);
		CTopologyDesc *FindRootNodeDesc (const CString &sID);
		CXMLElement *FindRootNodeDescXML (const CString &sNodeID);
		inline const CString &GetFirstNodeID (void) { return m_sFirstNode; }
		inline int GetRootNodeCount (void) { return m_RootNodes.GetCount(); }
		inline CTopologyDesc *GetRootNodeDesc (int iIndex) { return m_RootNodes[iIndex]; }
		inline CXMLElement *GetRootNodeDescXML (int iIndex) { return m_RootNodes[iIndex]->GetDesc(); }
		inline CTopologyDesc *GetTopologyDesc (int iIndex) { return m_Table[iIndex]; }
		inline int GetTopologyDescCount (void) { return m_Table.GetCount(); }
		ALERROR LoadFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CSystemMap *pMap, const CString &sParentUNID, bool bAddFirstAsRoot = false);
		ALERROR LoadNodeFromXML (SDesignLoadCtx &Ctx, CXMLElement *pNode, CSystemMap *pMap, const CString &sParentUNID, CTopologyDesc **retpNode = NULL);

	private:
		void InitIDMap (void);

		TArray<CTopologyDesc *> m_Table;
		TMap<CString, CTopologyDesc *> *m_pIDToDesc;
		TArray<CTopologyDesc *> m_RootNodes;

		CString m_sFirstNode;						//	Node where player starts (if not specified elsewhere)
	};

inline CTopologyDesc *CTopologyDesc::GetTopologyDesc (int iIndex) { return (m_pDescList ? m_pDescList->GetTopologyDesc(iIndex) : NULL); }
inline int CTopologyDesc::GetTopologyDescCount (void) { return (m_pDescList ? m_pDescList->GetTopologyDescCount() : 0); }

struct STopologyCreateCtx
	{
	STopologyCreateCtx (void) :
			pMap(NULL),
			pFragmentTable(NULL),
			pNodesAdded(NULL),
			bInFragment(false),
			xOffset(0),
			yOffset(0),
			iRotation(0)
		{ }

	CSystemMap *pMap;								//	Map that we're currently processing
	TArray<CTopologyDescTable *> Tables;			//	List of tables to look up
	CTopologyDescTable *pFragmentTable;
	CTopologyNodeList *pNodesAdded;					//	Output of nodes added

	bool bInFragment;
	CString sFragmentPrefix;
	int xOffset;
	int yOffset;
	int iRotation;
	CString sFragmentExitID;
	CString sFragmentExitGate;
	CString sFragmentAttributes;

	CString sError;
	};

//	Games and Records ----------------------------------------------------------

struct SBasicGameStats
	{
	int iSystemsVisited;
	int iEnemiesDestroyed;
	DWORD dwBestEnemyDestroyed;
	int iBestEnemyDestroyedCount;
	};

class CGameRecord
	{
	public:
		enum EDescParts
			{
			descCharacter =				0x00000001,
			descShip =					0x00000002,
			descEpitaph =				0x00000004,
			descDate =					0x00000008,
			descPlayTime =				0x00000010,
			descResurrectCount =		0x00000020,

			descAll =					0x0000003F,
			};

		enum Flags
			{
			FLAG_OMIT_WAS =				0x00000001,	//	Remove leading "was" from "was destroyed..."
			};

		CGameRecord (void);

		CString GetAdventureID (void) const;
		inline DWORD GetAdventureUNID (void) const { return m_dwAdventure; }
		CString GetDescription (DWORD dwParts = descAll) const;
		CString GetEndGameEpitaph (DWORD dwFlags = 0) const;
		inline const CString &GetEndGameReason (void) const { return m_sEndGameReason; }
		inline const CString &GetGameID (void) const { return m_sGameID; }
		inline GenomeTypes GetPlayerGenome (void) const { return m_iGenome; }
		inline const CString &GetPlayerName (void) const { return m_sName; }
		inline CString GetPlayTimeString (void) const { return m_Duration.Format(NULL_STR); }
		inline int GetResurrectCount (void) const { return m_iResurrectCount; }
		inline int GetScore (void) const { return m_iScore; }
		inline CString GetShipClass (void) const { return m_sShipClass; }
		inline const CString &GetUsername (void) const { return m_sUsername; }
		ALERROR InitFromJSON (const CJSONValue &Value);
		ALERROR InitFromXML (CXMLElement *pDesc);
		inline bool IsDebug (void) const { return m_bDebugGame; }
		inline bool IsRegistered (void) const { return m_bRegisteredGame; }
		static GenomeTypes LoadGenome (const CString &sAttrib);
		void SaveToJSON (CJSONValue *retOutput) const;
		inline void SetAdventureUNID (DWORD dwUNID) { m_dwAdventure = dwUNID; }
		inline void SetDebug (bool bDebug = true) { m_bDebugGame = bDebug; }
		inline void SetEndGameEpitaph (const CString &sEpitaph) { m_sEpitaph = sEpitaph; }
		inline void SetEndGameReason (const CString &sReason) { m_sEndGameReason = sReason; }
		inline void SetExtensions (const TArray<DWORD> &Extensions) { m_Extensions = Extensions; }
		inline void SetGameID (const CString &sGameID) { m_sGameID = sGameID; }
		inline void SetPlayerGenome (GenomeTypes iGenome) { m_iGenome = iGenome; }
		inline void SetPlayerName (const CString &sName) { m_sName = sName; }
		inline void SetPlayTime (const CTimeSpan &Time) { m_Duration = Time; }
		inline void SetRegistered (bool bRegistered = true) { m_bRegisteredGame = bRegistered; }
		inline void SetResurrectCount (int iCount) { m_iResurrectCount = iCount; }
		inline void SetScore (int iScore) { m_iScore = iScore; }
		void SetShipClass (DWORD dwUNID);
		void SetSystem (CSystem *pSystem);
		inline void SetUsername (const CString &sUsername) { m_sUsername = sUsername; }
		ALERROR WriteToXML (IWriteStream &Stream);

	private:
		//	Basic
		CString m_sUsername;					//	Username
		CString m_sGameID;						//	ID of game played
		DWORD m_dwAdventure;					//	UNID of adventure
		TArray<DWORD> m_Extensions;				//	UNID of included extensions

		CString m_sName;						//	Character name
		GenomeTypes m_iGenome;					//	Character genome

		DWORD m_dwShipClass;					//	Ship class UNID
		CString m_sShipClass;					//	Ship class
		CString m_sSystem;						//	NodeID of current system
		CString m_sSystemName;					//	Name of current system

		CTimeDate m_CreatedOn;					//	Game created on this date (set by server)
		CTimeDate m_ReportedOn;					//	Time/date of latest report (set by server)
		CTimeSpan m_Duration;					//	Time played
		bool m_bRegisteredGame;					//	If TRUE, this is a registered game
		bool m_bDebugGame;						//	If TRUE, this is a debug game

		//	Stats
		int m_iScore;
		int m_iResurrectCount;

		//	End Game
		CString m_sEndGameReason;
		CString m_sEpitaph;
	};

class CGameStats
	{
	public:
		inline void DeleteAll (void) { m_Stats.DeleteAll(); }
		inline int GetCount (void) const { return m_Stats.GetCount(); }
		inline const CString &GetDefaultSectionName (void) const { return m_sDefaultSectionName; }
		void GetEntry (int iIndex, CString *retsStatName, CString *retsStatValue, CString *retsSection) const;
		void Insert (const CString &sStatName, const CString &sStatValue = NULL_STR, const CString &sSection = NULL_STR, const CString &sSortKey = NULL_STR);
		ALERROR LoadFromStream (IReadStream *pStream);
		void SaveToJSON (CJSONValue *retOutput) const;
		ALERROR SaveToStream (IWriteStream *pStream) const;
		inline void SetDefaultSectionName (const CString &sName) { m_sDefaultSectionName = sName; }
		inline void Sort (void) { m_Stats.Sort(); }
		void TakeHandoff (CGameStats &Source);
		ALERROR WriteAsText (IWriteStream *pOutput) const;

	private:
		struct SStat
			{
			//	Needed for TArray's sort method
			bool operator > (const SStat &Right) const { return (strCompare(sSortKey, Right.sSortKey) == 1); }
			bool operator < (const SStat &Right) const { return (strCompare(sSortKey, Right.sSortKey) == -1); }

			CString sSortKey;
			CString sStatName;
			CString sStatValue;
			};

		void ParseSortKey (const CString &sSortKey, CString *retsSection, CString *retsSectionSortKey) const;

		TArray<SStat> m_Stats;
		CString m_sDefaultSectionName;
	};

class CAdventureHighScoreList
	{
	public:
		struct SSelect
			{
			DWORD dwAdventure;
			CString sUsername;
			int iScore;
			};

		CAdventureHighScoreList (DWORD dwAdventure = 0) : m_dwAdventure(dwAdventure),
				m_iSelection(-1)
			{ }

		inline void DeleteAll (void) { m_HighScores.DeleteAll(); m_iSelection = -1; }
		inline int GetCount (void) const { return m_HighScores.GetCount(); }
		inline int GetSelection (void) const { return m_iSelection; }
		ALERROR InitFromJSON (DWORD dwAdventureUNID, const CJSONValue &Value);
		void InsertSimpleScore (const CString &sUsername, int iScore);
		inline const CGameRecord &GetEntry (int iIndex) const { return m_HighScores[iIndex]; }
		void SetSelection (const CString &sUsername, int iScore);

	private:
		DWORD m_dwAdventure;
		TArray<CGameRecord> m_HighScores;
		int m_iSelection;
	};

class CAdventureRecord
	{
	public:
		enum ESpecialScoreIDs
			{
			personalBest =			10000,
			mostRecent =			10001,

			specialIDFirst =		10000,
			specialIDCount =		2,
			};

		CAdventureRecord (void) : m_dwAdventure(0) { }
		CAdventureRecord (DWORD dwAdventure, int iHighScoreCount);

		bool FindRecordByGameID (const CString &sGameID, DWORD *retdwID = NULL) const;
		CString GetAdventureName (void) const;
		DWORD GetAdventureUNID (void) const { return m_dwAdventure; }
		inline int GetHighScoreCount (void) const { return m_HighScores.GetCount(); }
		CGameRecord &GetRecordAt (DWORD dwID);
		void Init (DWORD dwAdventure);
		CGameRecord &InsertHighScore (void) { return *m_HighScores.Insert(); }
		inline bool IsSpecialID (DWORD dwID) const { return (dwID >= specialIDFirst && dwID < (specialIDFirst + specialIDCount)); }

	private:
		DWORD m_dwAdventure;
		TArray<CGameRecord> m_HighScores;
		CGameRecord m_Special[specialIDCount];
	};

class CUserProfile
	{
	public:
		CUserProfile (void) { }

		bool FindAdventureRecord (DWORD dwAdventure, int *retiIndex = NULL);
		inline CAdventureRecord &GetAdventureRecord (int iIndex) { return m_Records[iIndex]; }
		inline int GetAdventureRecordCount (void) { return m_Records.GetCount(); }
		void Init (const CString &sUsername);
		CAdventureRecord &InsertAdventureRecord (DWORD dwAdventure);

	private:
		CString m_sUsername;
		TArray<CAdventureRecord> m_Records;
	};

//	CItemType -----------------------------------------------------------------

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
			evtGetTradePrice			= 3,
			evtOnInstall				= 4,
			evtOnEnabled				= 5,
			evtOnRefuel					= 6,

			evtCount					= 7,
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

		CItemType (void);
		virtual ~CItemType (void);

		inline void AddWeapon (CDeviceClass *pWeapon) { ASSERT(!m_Weapons.Find(pWeapon)); m_Weapons.Insert(pWeapon); }
		inline bool AreChargesAmmo (void) const { return (m_fAmmoCharges ? true : false); }
		inline bool AreChargesValued (void) const { return (m_fValueCharges ? true : false); }
		inline bool CanBeSoldIfUsed (void) const { return (m_fNoSaleIfUsed ? false : true); }
		inline void ClearKnown (void) { m_fKnown = false; }
		inline void ClearShowReference (void) { m_fReference = false; }
		void CreateEmptyFlotsam (CSystem *pSystem, const CVector &vPos, const CVector &vVel, CSovereign *pSovereign, CStation **retpFlotsam);
		inline bool FindEventHandlerItemType (ECachedHandlers iEvent, SEventHandlerDesc *retEvent = NULL) const { if (retEvent) *retEvent = m_CachedEvents[iEvent]; return (m_CachedEvents[iEvent].pCode != NULL); }
		ICCItem *FindItemTypeBaseProperty (CCodeChainCtx &Ctx, const CString &sProperty) const;
		int GetApparentLevel (CItemCtx &Ctx) const;
		CDeviceClass *GetAmmoLauncher (int *retiVariant = NULL) const;
		inline CArmorClass *GetArmorClass (void) const { return m_pArmor; }
		ItemCategories GetCategory (void) const;
		inline int GetCharges (void) const { return (m_fInstanceData ? m_InitDataValue.Roll() : 0); }
		inline const CItemList &GetComponents (void) const { return m_Components; }
        CCurrencyAndValue GetCurrencyAndValue (CItemCtx &Ctx, bool bActual = false) const;
		inline CEconomyType *GetCurrencyType (void) const { return m_iValue.GetCurrencyType(); }
		inline const CString &GetData (void) const { return m_sData; }
		const CString &GetDesc (void) const;
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
		inline int GetMaxCharges (void) const { return (m_fInstanceData ? m_InitDataValue.GetMaxValue() : 0); }
		int GetMaxHPBonus (void) const;
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
		inline bool ShowReference (void) const { return (m_fReference ? true : false); }

		//	CDesignType overrides
		static CItemType *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designItemType) ? (CItemType *)pType : NULL); }
		virtual bool FindDataField (const CString &sField, CString *retsValue) const override;
		virtual int GetLevel (int *retiMinLevel = NULL, int *retiMaxLevel = NULL) const override { if (retiMinLevel) *retiMinLevel = m_iLevel; if (retiMaxLevel) *retiMaxLevel = m_iMaxLevel; return m_iLevel; }
		virtual CString GetNamePattern (DWORD dwNounFormFlags = 0, DWORD *retdwFlags = NULL) const override;
		virtual DesignTypes GetType (void) const override { return designItemType; }
		virtual bool IsVirtual (void) const override { return (m_fVirtual ? true : false); }

		static ItemCategories GetCategoryForNamedDevice (DeviceNames iDev);
		static CString GetItemCategory (ItemCategories iCategory);
		static bool ParseItemCategory (const CString &sCategory, ItemCategories *retCategory = NULL);

	protected:
		//	CDesignType overrides
		virtual void OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override;
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual CEffectCreator *OnFindEffectCreator (const CString &sUNID) override;
        virtual ALERROR OnFinishBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ICCItem *OnGetProperty (CCodeChainCtx &Ctx, const CString &sProperty) const override;
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
		DWORD m_fSpare8:1;

		DWORD m_dwSpare:16;

		CString m_sData;						//	Category-specific data
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

//	Ship Definitions

#include "TSEShipAI.h"
#include "TSEPlayerSettings.h"
#include "TSEShipCreator.h"
#include "TSEDocking.h"

//	CShipClass ----------------------------------------------------------------

class CShipClass : public CDesignType
	{
	public:
		enum EBalanceTypes
			{
			typeUnknown,

			typeMinion,
			typeStandard,
			typeElite,
			typeBoss,
			typeNonCombatant,

			typeTooWeak,
			typeTooStrong,
			typeArmorTooWeak,
			typeArmorTooStrong,
			typeWeaponsTooWeak,
			typeWeaponsTooStrong,
			};

		enum VitalSections
			{
			sectNonCritical	= 0x00000000,		//	Ship is not automatically
												//	destroyed (chance of being
												//	destroyed based on the power
												//	of the blast relative to the
												//	original armor HPs)

			//	NOTE: These must match the order in CShipClass.cpp (ParseNonCritical)

			sectDevice0		= 0x00000001,
			sectDevice1		= 0x00000002,
			sectDevice2		= 0x00000004,
			sectDevice3		= 0x00000008,
			sectDevice4		= 0x00000010,
			sectDevice5		= 0x00000020,
			sectDevice6		= 0x00000040,
			sectDevice7		= 0x00000080,
			sectDeviceMask	= 0x000000ff,

			sectManeuver	= 0x00000100,		//	Maneuvering damaged (quarter speed turn)
			sectDrive		= 0x00000200,		//	Drive damaged (half-speed)
			sectScanners	= 0x00000400,		//	Unable to target
			sectTactical	= 0x00000800,		//	Unable to fire weapons
			sectCargo		= 0x00001000,		//	Random cargo destroyed

			sectCritical	= 0x00010000,		//	Ship destroyed
			};

		CShipClass (void);
		virtual ~CShipClass (void);

		inline int Angle2Direction (int iAngle) const { return m_Perf.GetRotationDesc().GetFrameIndex(iAngle); }
		inline int AlignToRotationAngle (int iAngle) const { return m_Perf.GetRotationDesc().AlignToRotationAngle(iAngle); }
		int CalcArmorSpeedBonus (int iTotalArmorMass) const;
		Metric CalcMass (const CDeviceDescList &Devices) const;
		int CalcScore (void);
		bool CreateEmptyWreck (CSystem *pSystem, CShip *pShip, const CVector &vPos, const CVector &vVel, CSovereign *pSovereign, CStation **retpWreck);
		void CreateExplosion (CShip *pShip, CSpaceObject *pWreck = NULL);
		bool CreateWreck (CShip *pShip, CSpaceObject **retpWreck = NULL);
		inline bool FindDeviceSlotDesc (DeviceNames iDev, SDeviceDesc *retDesc) { return (m_pDevices ? m_pDevices->FindDefaultDesc(iDev, retDesc) : false); }
		inline bool FindDeviceSlotDesc (const CItem &Item, SDeviceDesc *retDesc) { return (m_pDevices ? m_pDevices->FindDefaultDesc(Item, retDesc) : false); }
		void GenerateDevices (int iLevel, CDeviceDescList &Devices);
		CString GenerateShipName (DWORD *retdwFlags) const;
		inline const CAISettings &GetAISettings (void) { return m_AISettings; }
		inline const CItemCriteria &GetArmorCriteria (void) const { return m_ArmorCriteria; }
        inline const CShipArmorDesc &GetArmorDesc (void) const { return m_Armor; }
        const CCargoDesc &GetCargoDesc (const CItem **retpCargoItem = NULL) const;
		inline CGenericType *GetCharacter (void) { return m_Character; }
		inline CGenericType *GetCharacterClass (void) { return m_CharacterClass; }
		inline int GetCyberDefenseLevel (void) { return m_iCyberDefenseLevel; }
		inline DWORD GetDefaultBkgnd (void) { return m_dwDefaultBkgnd; }
		inline CSovereign *GetDefaultSovereign (void) const { return m_pDefaultSovereign; }
		inline const CItemCriteria &GetDeviceCriteria (void) const { return m_DeviceCriteria; }
		inline const CDockingPorts &GetDockingPorts (void) { return m_DockingPorts; }
		CVector GetDockingPortOffset (int iRotation);
        const CDriveDesc &GetDriveDesc (const CItem **retpDriveItem = NULL) const;
		inline CObjectEffectDesc &GetEffectsDesc (void) { return m_Effects; }
		IShipGenerator *GetEscorts (void) { return m_pEscorts; }
		CWeaponFireDesc *GetExplosionType (CShip *pShip) const;
		inline CXMLElement *GetFirstDockScreen (void) { return m_pDefaultScreen.GetDesc(); }
		inline CDesignType *GetFirstDockScreen (CString *retsName) { return m_pDefaultScreen.GetDockScreen(this, retsName); }
        const CObjectImageArray &GetHeroImage (void);
		inline int GetHullCargoSpace (void) { return m_CargoDesc.GetCargoSpace(); }
		inline int GetHullMass (void) const { return m_iMass; }
		inline const CReactorDesc *GetHullReactorDesc (void) { return &m_ReactorDesc; }
		inline const CShipArmorSegmentDesc &GetHullSection (int iIndex) const { return m_Armor.GetSegment(iIndex); }
		int GetHullSectionAtAngle (int iAngle);
		inline int GetHullSectionCount (void) const { return m_Armor.GetCount(); }
		CString GetHullSectionName (int iIndex) const;
		inline const CObjectImageArray &GetImage (void) const { return m_Image.GetSimpleImage(); }
        inline const CAttributeDataBlock &GetInitialData (void) const { return m_InitialData; }
		inline const CShipInteriorDesc &GetInteriorDesc (void) const { return m_Interior; }
		inline int GetMaxArmorMass (void) const { return m_iMaxArmorMass; }
		inline int GetMaxCargoSpace (void) const { return m_iMaxCargoSpace; }
		inline int GetMaxDevices (void) const { return m_iMaxDevices; }
		inline int GetMaxNonWeapons (void) const { return m_iMaxNonWeapons; }
		inline int GetMaxReactorPower (void) const { return m_iMaxReactorPower; }
		int GetMaxStructuralHitPoints (void) const;
		inline int GetMaxWeapons (void) const { return m_iMaxWeapons; }
        const CPlayerSettings *GetPlayerSettings (void) const;
		CString GetPlayerSortString (void) const;
		CVector GetPosOffset (int iAngle, int iRadius, int iPosZ, bool b3DPos = true);
		inline IItemGenerator *GetRandomItemTable (void) const { return m_pItems; }
        const CReactorDesc &GetReactorDesc (const CItem **retpReactorItem = NULL) const;
		inline int GetRotationAngle (void) { return m_Perf.GetRotationDesc().GetFrameAngle(); }
		inline const CIntegralRotationDesc &GetRotationDesc (void) const { return m_Perf.GetRotationDesc(); }
		inline int GetRotationRange (void) { return m_Perf.GetRotationDesc().GetFrameCount(); }
		inline int GetScore (void) { return m_iScore; }
		inline DWORD GetShipNameFlags (void) { return m_dwShipNameFlags; }
		CString GetShortName (void) const;
		inline int GetSize (void) const { return m_iSize; }
		inline const CString &GetClassName (void) const { return m_sName; }
		inline const CString &GetManufacturerName (void) const { return m_sManufacturer; }
		inline const CString &GetShipTypeName (void) const { return m_sTypeName; }
		inline int GetWreckChance (void) { return m_iLeavesWreck; }
		CObjectImageArray &GetWreckImage (void) { if (!m_WreckImage.IsLoaded()) CreateWreckImage(); return m_WreckImage; }
		void GetWreckImage (CObjectImageArray *retWreckImage);
		int GetWreckImageVariants (void);
		inline bool HasDockingPorts (void) { return (m_fHasDockingPorts ? true : false); }
		inline bool HasShipName (void) const { return !m_sShipNames.IsBlank(); }
		void InitEffects (CShip *pShip, CObjectEffectList *retEffects);
        void InitPerformance (SShipPerformanceCtx &Ctx) const;
		void InstallEquipment (CShip *pShip);
        inline bool IsDebugOnly (void) const { const CPlayerSettings *pPlayerSettings = GetPlayerSettings(); return (pPlayerSettings && pPlayerSettings->IsDebugOnly()); }
		inline bool IsIncludedInAllAdventures (void) { const CPlayerSettings *pPlayerSettings = GetPlayerSettings(); return (pPlayerSettings && pPlayerSettings->IsIncludedInAllAdventures()); }
		inline bool IsPlayerShip (void) { return (GetPlayerSettings() != NULL); }
		inline bool IsShownAtNewGame (void) const { const CPlayerSettings *pPlayerSettings = GetPlayerSettings(); return (pPlayerSettings && pPlayerSettings->IsInitialClass() && !IsVirtual()); }
		inline bool IsTimeStopImmune (void) { return (m_fTimeStopImmune ? true : false); }
		void MarkImages (bool bMarkDevices);
		void Paint (CG32bitImage &Dest, 
					int x, 
					int y, 
					const ViewportTransform &Trans, 
					int iDirection, 
					int iTick, 
					bool bThrusting = false,
					bool bRadioactive = false,
					DWORD byInvisible = 0);
		void PaintDockPortPositions (CG32bitImage &Dest, int x, int y, int iShipRotation);
		void PaintMap (CMapViewportCtx &Ctx, 
					CG32bitImage &Dest, 
					int x, 
					int y, 
					int iDirection, 
					int iTick, 
					bool bThrusting,
					bool bRadioactive);
		void PaintScaled (CG32bitImage &Dest,
					int x,
					int y,
					int cxWidth,
					int cyHeight,
					int iDirection,
					int iTick);

		//	CDesignType overrides
		static CShipClass *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designShipClass) ? (CShipClass *)pType : NULL); }
		virtual bool FindDataField (const CString &sField, CString *retsValue) const override;
		virtual CCommunicationsHandler *GetCommsHandler (void) override;
		virtual CEconomyType *GetEconomyType (void) const;
		virtual int GetLevel (int *retiMinLevel = NULL, int *retiMaxLevel = NULL) const override { if (retiMinLevel) *retiMinLevel = m_iLevel; if (retiMaxLevel) *retiMaxLevel = m_iLevel; return m_iLevel; }
		virtual CString GetNamePattern (DWORD dwNounFormFlags = 0, DWORD *retdwFlags = NULL) const override;
		virtual CTradingDesc *GetTradingDesc (void) const override { return m_pTrade; }
		virtual DesignTypes GetType (void) const override { return designShipClass; }
        virtual const CCompositeImageDesc &GetTypeImage (void) const override { return m_Image; }
		virtual bool IsVirtual (void) const override { return (m_fVirtual ? true : false); }

		static Metric GetStdCombatStrength (int iLevel);
		static void Reinit (void);
		static void UnbindGlobal (void);

	protected:
		//	CDesignType overrides
		virtual void OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override;
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual CEffectCreator *OnFindEffectCreator (const CString &sUNID) override;
		virtual ALERROR OnFinishBindDesign (SDesignLoadCtx &Ctx) override;
        virtual CString OnGetMapDescriptionMain (SMapDescriptionCtx &Ctx) const override;
		virtual ICCItem *OnGetProperty (CCodeChainCtx &Ctx, const CString &sProperty) const override;
		virtual bool OnHasSpecialAttribute (const CString &sAttrib) const override;
		virtual void OnInitFromClone (CDesignType *pSource) override;
		virtual void OnMarkImages (void) override { MarkImages(true); }
		virtual void OnMergeType (CDesignType *pSource) override;
		virtual void OnReadFromStream (SUniverseLoadCtx &Ctx) override;
		virtual void OnReinit (void) override;
		virtual void OnSweep (void) override;
		virtual void OnUnbindDesign (void) override;
		virtual void OnWriteToStream (IWriteStream *pStream) override;

	private:
		enum PrivateConstants
			{
			maxExhaustImages = 5
			};

		struct SEquipmentDesc
			{
			Abilities iEquipment;
			bool bInstall;
			};

		struct SExhaustDesc
			{
			C3DConversion PosCalc;
			};

		EBalanceTypes CalcBalanceType (CString *retsDesc = NULL) const;
		CItemType *CalcBestMissile (const SDeviceDesc &Device) const;
		Metric CalcCombatStrength (void) const;
		Metric CalcDamageRate (int *retiAveWeaponLevel = NULL, int *retiMaxWeaponLevel = NULL) const;
		Metric CalcDefenseRate (void) const;
		inline Metric CalcDodgeRate (void) const { return CalcManeuverValue(true); }
		int CalcLevel (void) const;
		Metric CalcManeuverValue (bool bDodge = false) const;
		ICCItem *CalcMaxSpeedByArmorMass (CCodeChainCtx &Ctx) const;
		int CalcMinArmorMassForSpeed (int iSpeed) const;
        void CalcPerformance (void);
		int ComputeDeviceLevel (const SDeviceDesc &Device) const;
		void ComputeMovementStats (int *retiSpeed, int *retiThrust, int *retiManeuver);
		int ComputeScore (int iArmorLevel,
						  int iPrimaryWeapon,
						  int iSpeed,
						  int iThrust,
						  int iManeuver,
						  bool bPrimaryIsLauncher);
		void CreateWreckImage (void);
		void FindBestMissile (CDeviceClass *pLauncher, IItemGenerator *pItems, CItemType **retpMissile) const;
		void FindBestMissile (CDeviceClass *pLauncher, const CItemList &Items, CItemType **retpMissile) const;
		CString GetGenericName (DWORD *retdwFlags = NULL) const;
		inline int GetManeuverDelay (void) const { return m_Perf.GetRotationDesc().GetManeuverDelay(); }
		CStationType *GetWreckDesc (void);
		void InitDefaultArmorLimits (Metric rHullMass, int iMaxSpeed, Metric rThrustRatio);
		void InitShipNamesIndices (void);
		void PaintThrust (CG32bitImage &Dest, 
						int x, 
						int y, 
						const ViewportTransform &Trans, 
						int iDirection, 
						int iTick,
						bool bInFrontOnly);

		static int CalcDefaultSize (const CObjectImageArray &Image);

		CString m_sManufacturer;				//	Name of manufacturer
		CString m_sName;						//	Class name
		CString m_sTypeName;					//	Name of type
		DWORD m_dwClassNameFlags;				//	Flags for class name
		CSovereignRef m_pDefaultSovereign;		//	Sovereign

		CString m_sShipNames;					//	Names to use for individual ship
		DWORD m_dwShipNameFlags;				//	Flags for ship name
		TArray<int> m_ShipNamesIndices;			//	Shuffled indices for ship names
		mutable int m_iShipName;				//	Current ship name index

		int m_iScore;							//	Score when destroyed
		int m_iLevel;							//	Ship class level
		EBalanceTypes m_iLevelType;				//	Type of ships for level

        //  Hull properties

		int m_iMass;							//	Empty mass (tons)
		int m_iSize;							//	Length in meters
		CRotationDesc m_RotationDesc;	        //	Rotation and maneuverability
		double m_rThrustRatio;					//	If non-zero, then m_DriveDesc thrust is set based on this.
		CDriveDesc m_DriveDesc;					//	Drive descriptor
		CReactorDesc m_ReactorDesc;				//	Reactor descriptor
        CCargoDesc m_CargoDesc;                 //  Cargo space descriptor
		int m_iCyberDefenseLevel;				//	Cyber defense level

        //  Class limits

		CItemCriteria m_ArmorCriteria;			//	Allowable armor
		CItemCriteria m_DeviceCriteria;			//	Allowable devices
		int m_iStdArmorMass;					//	No penalty at this armor mass
		int m_iMaxArmorMass;					//	Max mass of single armor segment
		int m_iMaxArmorSpeedPenalty;			//	Change to speed at max armor mass (1/100th light-speed)
		int m_iMinArmorSpeedBonus;				//	Change to speed at 1/2 std armor mass

		int m_iMaxCargoSpace;					//	Max amount of cargo space with expansion (tons)
		int m_iMaxReactorPower;					//	Max compatible reactor power
		int m_iMaxDevices;						//	Max number of devices
		int m_iMaxWeapons;						//	Max number of weapon devices (including launchers)
		int m_iMaxNonWeapons;					//	Max number of non-weapon devices

        //  Wrecks

		int m_iLeavesWreck;						//	Chance that it leaves a wreck
		int m_iStructuralHP;					//	Structual hp of wreck
		CStationTypeRef m_pWreckType;				//	Station type to use as wreck

        //  Armor, Devices, Equipment, Etc.

        CShipArmorDesc m_Armor;                 //  Armor descriptor
		CShipInteriorDesc m_Interior;			//	Interior structure
		IDeviceGenerator *m_pDevices;			//	Generator of devices
		TArray<SEquipmentDesc> m_Equipment;		//	Initial equipment

        //  Performance Stats (after accounting for devices, etc).

        CShipPerformanceDesc m_Perf;            //  Performance based on average devices (only for stats)
		CDeviceDescList m_AverageDevices;		//	Average complement of devices (only for stats)

        //  AI & Player Settings

		CAISettings m_AISettings;				//	AI controller data
		mutable CPlayerSettings *m_pPlayerSettings;		//	Player settings data
		IItemGenerator *m_pItems;				//	Random items
        CAttributeDataBlock m_InitialData;      //  Initial data for ship object

		//	Escorts

		IShipGenerator *m_pEscorts;				//	Escorts

		//	Character

		CGenericTypeRef m_CharacterClass;		//	Character class
		CGenericTypeRef m_Character;			//	Character for ship

		//	Docking

		CDockingPorts m_DockingPorts;			//	Docking port definitions
		CDockScreenTypeRef m_pDefaultScreen;	//	Default screen
		DWORD m_dwDefaultBkgnd;					//	Default background screen
		CTradingDesc *m_pTrade;					//	Trade descriptors

		CCommunicationsHandler m_OriginalCommsHandler;
		CCommunicationsHandler m_CommsHandler;	//	Communications handler

		//	Image

		CCompositeImageDesc m_Image;			//	Image of ship
        CObjectImageArray m_HeroImage;          //  Large image
		CObjectEffectDesc m_Effects;			//	Effects for ship

		//	Wreck image

		CG32bitImage m_WreckBitmap;				//	Image to use when ship is wrecked
		CObjectImageArray m_WreckImage;			//	Image to use when ship is wrecked

		//	Explosion

		CWeaponFireDescRef m_pExplosionType;	//	Explosion to create when ship is destroyed

		//	Exhaust

		CObjectImageArray m_ExhaustImage;		//	Image of drive exhaust
		TArray<SExhaustDesc> m_Exhaust;			//	Drive exhaust painting

		//	Misc

		DWORD m_fRadioactiveWreck:1;			//	TRUE if wreck is always radioactive
		DWORD m_fHasDockingPorts:1;				//	TRUE if ship has docking ports
		DWORD m_fTimeStopImmune:1;				//	TRUE if ship is immune to stop-time
		DWORD m_fSpare4a:1;						//	Unused
		DWORD m_fSpare5a:1;						//	Unused
		DWORD m_fSpare6a:1;						//	Unused
		DWORD m_fCommsHandlerInit:1;			//	TRUE if comms handler has been initialized
		DWORD m_fVirtual:1;						//	TRUE if ship class is virtual (e.g., a base class)

		DWORD m_fOwnPlayerSettings:1;		    //	TRUE if we own m_pPlayerSettings
		DWORD m_fScoreOverride:1;				//	TRUE if score is specified in XML
		DWORD m_fLevelOverride:1;				//	TRUE if level is specified in XML
		DWORD m_fInheritedDevices:1;			//	TRUE if m_pDevices is inherited from another class
		DWORD m_fInheritedItems:1;				//	TRUE if m_pItems is inherited from another class
		DWORD m_fInheritedEscorts:1;			//	TRUE if m_pEscorts is inherited from another class
		DWORD m_fCyberDefenseOverride:1;		//	TRUE if cyberDefenseLevel is specified in XML
		DWORD m_fInheritedTrade:1;				//	TRUE if m_pTrade is inherited from another class

		DWORD m_fSpare1:1;
		DWORD m_fSpare2:1;
		DWORD m_fSpare3:1;
		DWORD m_fSpare4:1;
		DWORD m_fSpare5:1;
		DWORD m_fSpare6:1;
		DWORD m_fSpare7:1;
		DWORD m_fSpare8:1;

		DWORD m_fSpare:8;

		static CPlayerSettings m_DefaultPlayerSettings;
		static bool m_bDefaultPlayerSettingsBound;
	};

//	CEffectCreator ------------------------------------------------------------

class CEffectCreator : public CDesignType
	{
	public:
		enum ECachedHandlers
			{
			evtGetParameters			= 0,

			evtCount					= 1,
			};

		enum EInstanceTypes
			{
			instGame,
			instOwner,
			instCreator,
			};

		CEffectCreator (void);
		virtual ~CEffectCreator (void);

		static ALERROR CreateBeamEffect (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID, CEffectCreator **retpCreator);
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID, CEffectCreator **retpCreator);
		static ALERROR CreateSimpleFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID, CEffectCreator **retpCreator);
		static ALERROR CreateTypeFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CEffectCreator **retpCreator);
		static ALERROR CreateFromTag (const CString &sTag, CEffectCreator **retpCreator);
		static IEffectPainter *CreatePainterFromStream (SLoadCtx &Ctx, bool bNullCreator = false);
		static IEffectPainter *CreatePainterFromStreamAndCreator (SLoadCtx &Ctx, CEffectCreator *pCreator);
		static IEffectPainter *CreatePainterFromTag (const CString &sTag);
		static CEffectCreator *FindEffectCreator (const CString &sUNID);
		static void WritePainterToStream (IWriteStream *pStream, IEffectPainter *pPainter);

		IEffectPainter *CreatePainter (CCreatePainterCtx &Ctx);
		inline bool FindEventHandlerEffectType (ECachedHandlers iEvent, SEventHandlerDesc *retEvent = NULL) const { if (retEvent) *retEvent = m_CachedEvents[iEvent]; return (m_CachedEvents[iEvent].pCode != NULL); }
		inline CWeaponFireDesc *GetDamageDesc (void) { return m_pDamage; }
		inline EInstanceTypes GetInstance (void) const { return m_iInstance; }
		inline const CString &GetUNIDString (void) { return m_sUNID; }
		bool IsValidUNID (void);
		void PlaySound (CSpaceObject *pSource = NULL);

		//	Virtuals

		virtual ALERROR CreateEffect (CSystem *pSystem,
									  CSpaceObject *pAnchor,
									  const CVector &vPos,
									  const CVector &vVel,
									  int iRotation,
									  int iVariant = 0,
									  CSpaceObject **retpEffect = NULL);
		virtual int GetLifetime (void) { return 0; }
		virtual CEffectCreator *GetSubEffect (int iIndex) { return NULL; }
		virtual CString GetTag (void) = 0;
		virtual void SetLifetime (int iLifetime) { }
		virtual void SetVariants (int iVariants) { }

		//	CDesignType overrides
		static CEffectCreator *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designEffectType) ? (CEffectCreator *)pType : NULL); }
		virtual DesignTypes GetType (void) const override { return designEffectType; }

	protected:

		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual CEffectCreator *OnFindEffectCreator (const CString &sUNID) override;
		virtual bool OnFindEventHandler (const CString &sEvent, SEventHandlerDesc *retEvent = NULL) const override;
		virtual void OnMarkImages (void) override { m_Sound.Mark(); OnEffectMarkResources(); }

		//	CEffectCreator virtuals

		virtual IEffectPainter *OnCreatePainter (CCreatePainterCtx &Ctx) = 0;
		virtual ALERROR OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) { return NOERROR; }
		virtual ALERROR OnEffectBindDesign (SDesignLoadCtx &Ctx) { return NOERROR; }
		virtual void OnEffectMarkResources (void) { }
		virtual void OnEffectPlaySound (CSpaceObject *pSource);

		void InitPainterParameters (CCreatePainterCtx &Ctx, IEffectPainter *pPainter);

	private:
		ALERROR InitBasicsFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		ALERROR InitInstanceFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);

		CString m_sUNID;
		CSoundRef m_Sound;
		EInstanceTypes m_iInstance;

		CWeaponFireDesc *m_pDamage;

		//	Events
		CEventHandler m_Events;				//	Local events
		SEventHandlerDesc m_CachedEvents[evtCount];
	};

//	COverlayType ----------------------------------------------------------

class COverlayType : public CDesignType
	{
	public:
		enum ECounterDisplay
			{
			counterNone,						//	Do not show a counter

			counterCommandBarProgress,			//	Show as progress bar in command bar
			counterProgress,					//	Show as progress bar on object
			counterRadius,						//	Show as circle of given radius (pixels)
			counterFlag,						//	Show a flag with counter and label
			};

		COverlayType(void);
		virtual ~COverlayType(void);

		bool AbsorbsWeaponFire (CInstalledDevice *pWeapon);
		inline bool Disarms (void) const { return m_fDisarmShip; }
		inline CG32bitPixel GetCounterColor (void) const { return m_rgbCounterColor; }
		inline const CString &GetCounterLabel (void) const { return m_sCounterLabel; }
		inline int GetCounterMax (void) const { return m_iCounterMax; }
		inline ECounterDisplay GetCounterStyle (void) const { return m_iCounterType; }
		int GetDamageAbsorbed (CSpaceObject *pSource, SDamageCtx &Ctx);
		inline Metric GetDrag (void) const { return m_rDrag; }
		inline CEffectCreator *GetEffectCreator (void) const { return m_pEffect; }
		inline CEffectCreator *GetHitEffectCreator (void) const { return m_pHitEffect; }
		int GetWeaponBonus (CInstalledDevice *pDevice, CSpaceObject *pSource);
		inline bool IsHitEffectAlt (void) { return m_fAltHitEffect; }
		inline bool IsShieldOverlay (void) { return m_fShieldOverlay; }
		inline bool IsShipScreenDisabled (void) { return m_fDisableShipScreen; }
		inline bool IsShownOnMap (void) { return m_fShowOnMap; }
		inline bool Paralyzes (void) const { return m_fParalyzeShip; }
		inline bool RotatesWithShip (void) { return m_fRotateWithShip; }
		inline bool Spins (void) const { return m_fSpinShip; }

		//	CDesignType overrides
		static COverlayType *AsType(CDesignType *pType) { return ((pType && pType->GetType() == designOverlayType) ? (COverlayType *)pType : NULL); }
		virtual bool FindDataField (const CString &sField, CString *retsValue) const override;
		virtual DesignTypes GetType (void) const override { return designOverlayType; }

	protected:
		//	CDesignType overrides
		virtual void OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override;
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual CEffectCreator *OnFindEffectCreator (const CString &sUNID) override;

	private:
		int m_iAbsorbAdj[damageCount];			//	Damage absorbed by the field
		DamageTypeSet m_WeaponSuppress;			//	Types of weapons suppressed
		int m_iBonusAdj[damageCount];			//	Adjustment to weapons damage
		Metric m_rDrag;							//	Drag coefficient (1.0 = no drag)

		CEffectCreator *m_pEffect;				//	Effect for field
		CEffectCreator *m_pHitEffect;			//	Effect when field is hit by damage

		ECounterDisplay m_iCounterType;			//	Type of counter to paint
		CString m_sCounterLabel;				//	Label for counter
		int m_iCounterMax;						//	Max value of counter (for progress bar)
		CG32bitPixel m_rgbCounterColor;					//	Counter color

		DWORD m_fAltHitEffect:1;				//	If TRUE, hit effect replaces normal effect
		DWORD m_fRotateWithShip:1;				//	If TRUE, we rotate along with source rotation
		DWORD m_fShieldOverlay:1;				//	If TRUE, we are above hull/armor
		DWORD m_fParalyzeShip:1;				//	If TRUE, ship is paralyzed
		DWORD m_fDisarmShip:1;					//	If TRUE, ship is disarmed
		DWORD m_fDisableShipScreen:1;			//	If TRUE, player cannot bring up ship screen
		DWORD m_fSpinShip:1;					//	If TRUE, ship spins uncontrollably
		DWORD m_fShowOnMap:1;					//	If TRUE, we show on the system map

		DWORD m_fSpare1:1;
		DWORD m_fSpare2:1;
		DWORD m_fSpare3:1;
		DWORD m_fSpare4:1;
		DWORD m_fSpare5:1;
		DWORD m_fSpare6:1;
		DWORD m_fSpare7:1;
		DWORD m_fSpare8:1;

		DWORD m_dwSpare:16;
	};

//	CSystemType ---------------------------------------------------------------

class CSystemType : public CDesignType
	{
	public:
		enum ETileSize
			{
			sizeUnknown =					-1,

			sizeSmall =						0,		//	128 pixels per tile
			sizeMedium =					1,		//	256 pixels per tile
			sizeLarge =						2,		//	512 pixels per tile
			sizeHuge =						3,		//	1024 pixels per tile

			sizeCount =						4,
			};

		enum ECachedHandlers
			{
			evtOnObjJumpPosAdj			= 0,

			evtCount					= 1,
			};

		CSystemType (void);
		virtual ~CSystemType (void);

		inline bool FindEventHandlerSystemType (ECachedHandlers iEvent, SEventHandlerDesc *retEvent = NULL) const { if (retEvent) *retEvent = m_CachedEvents[iEvent]; return (m_CachedEvents[iEvent].pCode != NULL); }
		ALERROR FireOnCreate (SSystemCreateCtx &SysCreateCtx, CString *retsError = NULL);
		bool FireOnObjJumpPosAdj (CSpaceObject *pPos, CVector *iovPos);
		ALERROR FireSystemCreateCode (SSystemCreateCtx &SysCreateCtx, ICCItem *pCode, const COrbit &OrbitDesc, CString *retsError);
		inline DWORD GetBackgroundUNID (void) { return m_dwBackgroundUNID; }
		inline CXMLElement *GetDesc (void) { return m_pDesc; }
		inline CXMLElement *GetLocalSystemTables (void) { return m_pLocalTables; }
		inline ETileSize GetSpaceEnvironmentTileSize (void) const { return m_iTileSize; }
		inline Metric GetSpaceScale (void) const { return m_rSpaceScale; }
		inline Metric GetTimeScale (void) const { return m_rTimeScale; }
		inline bool HasExtraEncounters (void) const { return !m_bNoExtraEncounters; }
		inline bool HasRandomEncounters (void) const { return !m_bNoRandomEncounters; }

		//	CDesignType overrides
		static CSystemType *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designSystemType) ? (CSystemType *)pType : NULL); }
		virtual DesignTypes GetType (void) const override { return designSystemType; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual void OnMarkImages (void) override;

	private:
		DWORD m_dwBackgroundUNID;
		Metric m_rSpaceScale;				//	Klicks per pixel
		Metric m_rTimeScale;				//	Seconds of game time per real time
		ETileSize m_iTileSize;				//	Tile size for environment

		CXMLElement *m_pDesc;				//	System definition
		CXMLElement *m_pLocalTables;		//	Local system tables

		bool m_bNoRandomEncounters;			//	TRUE if we don't have random encounters
		bool m_bNoExtraEncounters;			//	TRUE if we don't add new encounters to
											//		satisfy minimums.

		SEventHandlerDesc m_CachedEvents[evtCount];
	};

//	CDockScreenType -----------------------------------------------------------

class CDockScreenType : public CDesignType
	{
	public:
		CDockScreenType (void);
		virtual ~CDockScreenType (void);

		static CString GetStringUNID (CDesignType *pRoot, const CString &sScreen);
		inline CXMLElement *GetDesc (void) { return m_pDesc; }
		CXMLElement *GetPane (const CString &sPane);
		static CDesignType *ResolveScreen (CDesignType *pLocalScreen, const CString &sScreen, CString *retsScreenActual = NULL, bool *retbIsLocal = NULL);

		//	CDesignType overrides
		static CDockScreenType *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designDockScreen) ? (CDockScreenType *)pType : NULL); }
		virtual DesignTypes GetType (void) const override { return designDockScreen; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override { return NOERROR; }
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;

	private:
		CXMLElement *m_pDesc;
	};

class CStationEncounterDesc
	{
	public:
		struct SExclusionDesc
			{
			bool bHasAllExclusion;
			bool bHasEnemyExclusion;

			Metric rAllExclusionRadius2;
			Metric rEnemyExclusionRadius2;
			};

		CStationEncounterDesc (void) :
				m_bSystemCriteria(false),
				m_rExclusionRadius(0.0),
				m_rEnemyExclusionRadius(0.0),
				m_bAutoLevelFrequency(false),
				m_bNumberAppearing(false),
				m_bMaxCountLimit(false),
				m_iMaxCountInSystem(-1)
			{ }

		int CalcLevelFromFrequency (void) const;
		bool InitAsOverride (const CStationEncounterDesc &Original, const CXMLElement &Override, CString *retsError);
		ALERROR InitFromStationTypeXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		void InitLevelFrequency (void);
		inline bool CanBeRandomlyEncountered (void) const { return (!m_sLevelFrequency.IsBlank() || m_bNumberAppearing); }
		int GetCountOfRandomEncounterLevels (void) const;
		void GetExclusionDesc (SExclusionDesc &Exclusion) const;
		inline Metric GetExclusionRadius (void) const { return m_rExclusionRadius; }
		inline Metric GetEnemyExclusionRadius (void) const { return m_rEnemyExclusionRadius; }
		int GetFrequencyByLevel (int iLevel) const;
		inline const CString &GetLocationCriteria (void) const { return m_sLocationCriteria; }
		inline int GetMaxAppearing (void) const { return (m_bMaxCountLimit ? m_MaxAppearing.Roll() : -1); }
		inline int GetNumberAppearing (void) const { return (m_bNumberAppearing ? m_NumberAppearing.Roll() : -1); }
        inline bool HasSystemCriteria (const CTopologyNode::SCriteria **retpCriteria = NULL) const 
            {
            if (m_bSystemCriteria) 
                {
                if (retpCriteria) 
                    *retpCriteria = &m_SystemCriteria;
                return m_bSystemCriteria; 
                }
            else
                return false;
            }
		inline bool IsUniqueInSystem (void) const { return (m_iMaxCountInSystem == 1); }
		void ReadFromStream (SUniverseLoadCtx &Ctx);
		void WriteToStream (IWriteStream *pStream);

	private:
		bool m_bSystemCriteria;				//	If TRUE we have system criteria
		CTopologyNode::SCriteria m_SystemCriteria;	//	System criteria

		CString m_sLevelFrequency;			//	String array of frequency distribution by level
		CString m_sLocationCriteria;		//	Criteria for location
		Metric m_rExclusionRadius;			//	No stations of any kind within this radius
		Metric m_rEnemyExclusionRadius;		//	No enemy stations within this radius
		bool m_bAutoLevelFrequency;			//	We generated m_sLevelFrequency and need to save it.

		bool m_bNumberAppearing;			//	If TRUE, must create this exact number in game
		DiceRange m_NumberAppearing;		//	Create this number in the game

		bool m_bMaxCountLimit;				//	If FALSE, no limit
		DiceRange m_MaxAppearing;

		int m_iMaxCountInSystem;			//	-1 means no limit
	};

class CStationEncounterCtx
	{
	public:
		void AddEncounter (CSystem *pSystem);
		bool CanBeEncountered (const CStationEncounterDesc &Desc);
		bool CanBeEncounteredInSystem (CSystem *pSystem, CStationType *pStationType, const CStationEncounterDesc &Desc);
		int GetFrequencyByLevel (int iLevel, const CStationEncounterDesc &Desc);
		int GetFrequencyForNode (CTopologyNode *pNode, CStationType *pStation, const CStationEncounterDesc &Desc);
		int GetFrequencyForSystem (CSystem *pSystem, CStationType *pStation, const CStationEncounterDesc &Desc);
		int GetMinimumForNode (CTopologyNode *pNode, const CStationEncounterDesc &Desc);
		int GetRequiredForNode (CTopologyNode *pNode, const CStationEncounterDesc &Desc);
		inline int GetTotalCount (void) const { return m_Total.iCount; }
		inline int GetTotalLimit (void) const { return m_Total.iLimit; }
		inline int GetTotalMinimum (void) const { return m_Total.iMinimum; }
		void IncMinimumForNode (CTopologyNode *pNode, const CStationEncounterDesc &Desc, int iInc = 1);
		void ReadFromStream (SUniverseLoadCtx &Ctx);
		void Reinit (const CStationEncounterDesc &Desc);
		void WriteToStream (IWriteStream *pStream);

	private:
		struct SEncounterStats
			{
			SEncounterStats (void) :
					iCount(0),
					iLimit(-1),
					iMinimum(0),
                    iNodeCriteria(-1)
				{ }

			int iCount;						//	Number of times encountered
			int iLimit;						//	Encounter limit (-1 = no limit)
			int iMinimum;					//	Minimum encounters (-1 = no limit)

            mutable int iNodeCriteria;      //  Cached frequency for node (-1 = unknown)
			};

		int GetBaseFrequencyForNode (CTopologyNode *pNode, CStationType *pStation, const CStationEncounterDesc &Desc);

		SEncounterStats m_Total;			//	Encounters in entire game
		TSortMap<int, SEncounterStats> m_ByLevel;	//	Encounters by system level
		TSortMap<CString, SEncounterStats> m_ByNode;	//	Encounters by topology node
	};

//	Trading --------------------------------------------------------------------

class CTradingDesc
	{
	public:
		enum Flags
			{
			FLAG_NO_INVENTORY_CHECK =	0x00000001,	//	Do not check to see if item exists
			FLAG_NO_DONATION =			0x00000002,	//	Do not return prices for donations
			FLAG_NO_PLAYER_ADJ =		0x00000004,	//	Do not compute special player adjs
			};

		enum PriceFlags
			{
			PRICE_UPGRADE_INSTALL_ONLY =	0x00000001,	//	Install available only if you buy item
			};

		CTradingDesc (void);
		~CTradingDesc (void);

		inline void AddBuyOrder (CItemType *pType, const CString &sCriteria, int iPriceAdj)
			{ AddOrder(pType, sCriteria, iPriceAdj, FLAG_BUYS); }
		inline void AddSellOrder (CItemType *pType, const CString &sCriteria, int iPriceAdj)
			{ AddOrder(pType, sCriteria, iPriceAdj, FLAG_SELLS); }
		bool Buys (CSpaceObject *pObj, const CItem &Item, DWORD dwFlags, int *retiPrice = NULL, int *retiMaxCount = NULL);
		bool BuysShip (CSpaceObject *pObj, CSpaceObject *pShip, DWORD dwFlags, int *retiPrice = NULL);
		int Charge (CSpaceObject *pObj, int iCharge);
        bool ComposeDescription (CString *retsDesc) const;
		bool GetArmorInstallPrice (CSpaceObject *pObj, const CItem &Item, DWORD dwFlags, int *retiPrice, CString *retsReason = NULL) const;
		bool GetArmorRepairPrice (CSpaceObject *pObj, CSpaceObject *pSource, const CItem &Item, int iHPToRepair, DWORD dwFlags, int *retiPrice) const;
		bool GetDeviceInstallPrice (CSpaceObject *pObj, const CItem &Item, DWORD dwFlags, int *retiPrice, CString *retsReason = NULL, DWORD *retdwPriceFlags = NULL) const;
		bool GetDeviceRemovePrice (CSpaceObject *pObj, const CItem &Item, DWORD dwFlags, int *retiPrice, DWORD *retdwPriceFlags = NULL) const;
		inline CEconomyType *GetEconomyType (void) { return m_pCurrency; }
		inline int GetMaxCurrency (void) { return m_iMaxCurrency; }
		int GetMaxLevelMatched (ETradeServiceTypes iService, bool bDescriptionOnly = false) const;
		bool GetRefuelItemAndPrice (CSpaceObject *pObj, CSpaceObject *pObjToRefuel, DWORD dwFlags, CItemType **retpItemType, int *retiPrice) const;
		inline int GetReplenishCurrency (void) { return m_iReplenishCurrency; }
		bool HasService (ETradeServiceTypes iService) const;
        inline bool HasServices (void) const { return (m_List.GetCount() > 0); }
		bool Sells (CSpaceObject *pObj, const CItem &Item, DWORD dwFlags, int *retiPrice = NULL);
		bool SellsShip (CSpaceObject *pObj, CSpaceObject *pShip, DWORD dwFlags, int *retiPrice = NULL);
		void SetEconomyType (CEconomyType *pCurrency) { m_pCurrency.Set(pCurrency); }
		void SetMaxCurrency (int iMaxCurrency) { m_iMaxCurrency = iMaxCurrency; }
		void SetReplenishCurrency (int iReplenishCurrency) { m_iReplenishCurrency = iReplenishCurrency; }

		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CTradingDesc **retpTrade);
		void OnCreate (CSpaceObject *pObj);
		ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx);
		void OnUpdate (CSpaceObject *pObj);
		void ReadFromStream (SLoadCtx &Ctx);
		void RefreshInventory (CSpaceObject *pObj, int iPercent = 100);
		void WriteToStream (IWriteStream *pStream);

		static int CalcPriceForService (ETradeServiceTypes iService, CSpaceObject *pProvider, const CItem &Item, int iCount, DWORD dwFlags);
		static CString ServiceToString (ETradeServiceTypes iService);

	private:
		enum InternalFlags
			{
			//	Flags for SServiceDesc

			FLAG_ACTUAL_PRICE =			0x00000004,	//	TRUE if we compute actual price
			FLAG_INVENTORY_ADJ =		0x00000008,	//	TRUE if we adjust the inventory
			FLAG_UPGRADE_INSTALL_ONLY =	0x00000020,	//	TRUE if we must purchase an item to install
            FLAG_NO_DESCRIPTION =       0x00000040, //  If TRUE, we exclude this service from ComposeDescription

			//	DEPRECATED: We don't store these flags, but we require the values
			//	for older versions.

			FLAG_SELLS =				0x00000001,	//	TRUE if station sells this item type
			FLAG_BUYS =					0x00000002,	//	TRUE if station buys this item type
			FLAG_ACCEPTS_DONATIONS =	0x00000010,	//	TRUE if we accept donations
			};

		struct SServiceDesc
			{
            SServiceDesc (void) :
                    pPriceAdjCode(NULL)
                { }

            ~SServiceDesc (void);

			ETradeServiceTypes iService;		//	Type of service
			CString sID;						//	ID of order

			CItemTypeRef pItemType;				//	Item type
			CItemCriteria ItemCriteria;			//	If ItemType is NULL, this is the criteria
			CFormulaText InventoryAdj;			//	% of item count found at any one time
			CString sLevelFrequency;			//	Level frequency for inventory adj

			CDesignTypeCriteria TypeCriteria;	//	Type criteria (for selling ships, etc.).

			CFormulaText PriceAdj;				//	Price adjustment
            ICCItem *pPriceAdjCode;             //  Code to adjust price

			CString sMessageID;					//	ID of language element to return if we match.
			DWORD dwFlags;						//	Flags
			};

        struct SServiceInfo
            {
            bool bAvailable;                    //  Service available
            int iMaxLevel;                      //  Max level for this service
            bool bUpdateInstallOnly;            //  Requires purchase
            };

		void AddOrder (CItemType *pItemType, const CString &sCriteria, int iPriceAdj, DWORD dwFlags);
		CString ComputeID (ETradeServiceTypes iService, DWORD dwUNID, const CString &sCriteria, DWORD dwFlags);
		int ComputeMaxCurrency (CSpaceObject *pObj);
		int ComputePrice (STradeServiceCtx &Ctx, DWORD dwFlags);
		bool FindService (ETradeServiceTypes iService, const CItem &Item, const SServiceDesc **retpDesc);
		bool FindService (ETradeServiceTypes iService, CSpaceObject *pShip, const SServiceDesc **retpDesc);
        bool GetServiceInfo (ETradeServiceTypes iService, SServiceInfo &Info) const;
		bool HasServiceDescription (ETradeServiceTypes iService) const;
		bool Matches (const CItem &Item, const SServiceDesc &Commodity) const;
		bool Matches (CDesignType *pType, const SServiceDesc &Commodity) const;
		void ReadServiceFromFlags (DWORD dwFlags, ETradeServiceTypes *retiService, DWORD *retdwFlags);
		bool SetInventoryCount (CSpaceObject *pObj, SServiceDesc &Desc, CItemType *pItemType);

		static int ComputePrice (STradeServiceCtx &Ctx, const SServiceDesc &Commodity, DWORD dwFlags);

		CEconomyTypeRef m_pCurrency;
		int m_iMaxCurrency;
		int m_iReplenishCurrency;

		TArray<SServiceDesc> m_List;
	};

//	CStationType --------------------------------------------------------------

const int STATION_REPAIR_FREQUENCY =	30;

enum ScaleTypes
	{
	scaleNone =						-1,

	scaleStar =						0,
	scaleWorld =					1,
	scaleStructure =				2,
	scaleShip =						3,
	scaleFlotsam =					4,
	};

class CStationType : public CDesignType
	{
	public:
		enum ESizeClass
			{
			sizeNone =				0,

			//	World size classes

			worldSizeA =			1,		//	1-24 km
			worldSizeB =			2,		//	25-74 km
			worldSizeC =			3,		//	75-299 km
			worldSizeD =			4,		//	300-749 km

			worldSizeE =			5,		//	750-1,499 km
			worldSizeF =			6,		//	1,500-2,999 km
			worldSizeG =			7,		//	3,000-4,499 km

			worldSizeH =			8,		//	4,500-7,499 km
			worldSizeI =			9,		//	7,500-14,999 km
			worldSizeJ =			10,		//	15,000-29,999 km

			worldSizeK =			11,		//	30,000-74,999 km
			worldSizeL =			12,		//	75,000-149,999 km
			worldSizeM =			13,		//	150,000+ km
			};

		CStationType (void);
		virtual ~CStationType (void);
		static void Reinit (void);

		inline bool AlertWhenAttacked (void) { return (mathRandom(1, 100) <= m_iAlertWhenAttacked); }
		inline bool AlertWhenDestroyed (void) { return (mathRandom(1, 100) <= m_iAlertWhenDestroyed); }
		inline bool BuildsReinforcements (void) const { return (m_fBuildReinforcements ? true : false); }
		inline bool CanAttack (void) const { return (m_fCanAttack ? true : false); }
		inline bool CanBeEncountered (void) { return m_EncounterRecord.CanBeEncountered(GetEncounterDesc()); }
		inline bool CanBeEncountered (CSystem *pSystem) { return m_EncounterRecord.CanBeEncounteredInSystem(pSystem, this, GetEncounterDesc()); }
		inline bool CanBeEncounteredRandomly (void) const { return GetEncounterDesc().CanBeRandomlyEncountered(); }
		inline bool CanBeHitByFriends (void) { return (m_fNoFriendlyTarget ? false : true); }
		inline bool CanHitFriends (void) { return (m_fNoFriendlyFire ? false : true); }
		CString GenerateRandomName (const CString &sSubst, DWORD *retdwFlags);
		inline CXMLElement *GetAbandonedScreen (void) { return m_pAbandonedDockScreen.GetDesc(); }
		inline CDesignType *GetAbandonedScreen (CString *retsName) { return m_pAbandonedDockScreen.GetDockScreen(this, retsName); }
		inline CArmorClass *GetArmorClass (void) { return (m_pArmor ? m_pArmor->GetArmorClass() : NULL); }
		inline CEffectCreator *GetBarrierEffect (void) { return m_pBarrierEffect; }
		inline IShipGenerator *GetConstructionTable (void) { return m_pConstruction; }
		CSovereign *GetControllingSovereign (void);
		inline DWORD GetDefaultBkgnd (void) { return m_dwDefaultBkgnd; }
		inline CXMLElement *GetDesc (void) { return m_pDesc; }
		inline CString GetDestNodeID (void) { return m_sStargateDestNode; }
		inline CString GetDestEntryPoint (void) { return m_sStargateDestEntryPoint; }
		inline int GetEjectaAdj (void) { return m_iEjectaAdj; }
		CWeaponFireDesc *GetEjectaType (void) { return m_pEjectaType; }
		const CStationEncounterDesc &GetEncounterDesc (void) const;
		inline Metric GetEnemyExclusionRadius (void) const { return GetEncounterDesc().GetEnemyExclusionRadius(); }
		inline void GetExclusionDesc (CStationEncounterDesc::SExclusionDesc &Exclusion) const { GetEncounterDesc().GetExclusionDesc(Exclusion); }
		inline Metric GetExclusionRadius (void) const { return GetEncounterDesc().GetExclusionRadius(); }
		CWeaponFireDesc *GetExplosionType (void) const { return m_pExplosionType; }
		inline int GetEncounterFrequency (void) { return m_iEncounterFrequency; }
		inline int GetEncounterMinimum (CTopologyNode *pNode) { return m_EncounterRecord.GetMinimumForNode(pNode, GetEncounterDesc()); }
		inline CStationEncounterCtx &GetEncounterRecord (void) { return m_EncounterRecord; }
		inline int GetEncounterRequired (CTopologyNode *pNode) { return m_EncounterRecord.GetRequiredForNode(pNode, GetEncounterDesc()); }
		inline IShipGenerator *GetEncountersTable (void) { return m_pEncounters; }
		inline int GetFireRateAdj (void) { return m_iFireRateAdj; }
		inline CXMLElement *GetFirstDockScreen (void) { return m_pFirstDockScreen.GetDesc(); }
		inline CDesignType *GetFirstDockScreen (CString *retsName) { return m_pFirstDockScreen.GetDockScreen(this, retsName); }
		inline int GetFrequencyByLevel (int iLevel) { return m_EncounterRecord.GetFrequencyByLevel(iLevel, GetEncounterDesc()); }
		inline int GetFrequencyForNode (CTopologyNode *pNode) { return m_EncounterRecord.GetFrequencyForNode(pNode, this, GetEncounterDesc()); }
		inline int GetFrequencyForSystem (CSystem *pSystem) { return m_EncounterRecord.GetFrequencyForSystem(pSystem, this, GetEncounterDesc()); }
		inline CEffectCreator *GetGateEffect (void) { return m_pGateEffect; }
		inline Metric GetGravityRadius (void) const { return m_rGravityRadius; }
		inline const CObjectImageArray &GetHeroImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers, int *retiRotation = NULL) { return m_HeroImage.GetImage(Selector, Modifiers, retiRotation); }
		inline const CCompositeImageDesc &GetImage (void) { return m_Image; }
		inline const CObjectImageArray &GetImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers, int *retiRotation = NULL) { return m_Image.GetImage(Selector, Modifiers, retiRotation); }
		inline int GetImageVariants (void) { return m_Image.GetVariantCount(); }
		inline int GetInitialHitPoints (void) { return m_iHitPoints; }
		inline IShipGenerator *GetInitialShips (int iDestiny, int *retiCount) { *retiCount = (!m_ShipsCount.IsEmpty() ? m_ShipsCount.RollSeeded(iDestiny) : 1); return m_pInitialShips; }
		Metric GetLevelStrength (int iLevel);
		inline const CString &GetLocationCriteria (void) const { return GetEncounterDesc().GetLocationCriteria(); }
		inline Metric GetMass (void) { return m_rMass; }
		inline int GetMinShips (int iDestiny) { return (!m_ShipsCount.IsEmpty() ? m_ShipsCount.RollSeeded(iDestiny) : m_iMinShips); }
		inline Metric GetMaxEffectiveRange (void) { return m_rMaxAttackDistance; }
		inline int GetMaxHitPoints (void) { return m_iMaxHitPoints; }
		inline int GetMaxLightDistance (void) { return m_iMaxLightDistance; }
		inline int GetMaxShipConstruction (void) { return m_iMaxConstruction; }
		inline int GetMaxStructuralHitPoints (void) { return m_iMaxStructuralHP; }
		inline DWORD GetNameFlags (void) { return m_dwNameFlags; }
		inline int GetNumberAppearing (void) const { return m_EncounterRecord.GetTotalMinimum(); }
		inline Metric GetParallaxDist (void) const { return m_rParallaxDist; }
		inline IItemGenerator *GetRandomItemTable (void) { return m_pItems; }
		inline DWORD GetRandomNameFlags (void) { return m_dwRandomNameFlags; }
		inline const CRegenDesc &GetRegenDesc (void) { return m_Regen; }
		IShipGenerator *GetReinforcementsTable (void);
		const CIntegralRotationDesc &GetRotationDesc (void);
		inline CXMLElement *GetSatellitesDesc (void) { return m_pSatellitesDesc; }
		inline ScaleTypes GetScale (void) const { return m_iScale; }
		inline int GetSize (void) const { return m_iSize; }
		inline int GetShipConstructionRate (void) { return m_iShipConstructionRate; }
		inline const CRegenDesc &GetShipRegenDesc (void) { return m_ShipRegen; }
		inline CSovereign *GetSovereign (void) const { return m_pSovereign; }
		inline CG32bitPixel GetSpaceColor (void) { return m_rgbSpaceColor; }
		inline int GetStealth (void) const { return m_iStealth; }
		inline int GetStructuralHitPoints (void) { return m_iStructuralHP; }
		inline int GetTempChance (void) const { return m_iChance; }
		inline bool HasAnimations (void) const { return (m_pAnimations != NULL); }
		inline bool HasGravity (void) const { return (m_rGravityRadius > 0.0); }
		inline bool HasRandomNames (void) const { return !m_sRandomNames.IsBlank(); }
		inline bool HasWreckImage (void) const { return (!IsImmutable() && m_iMaxHitPoints > 0); }
		inline void IncEncounterMinimum (CTopologyNode *pNode, int iInc = 1) { m_EncounterRecord.IncMinimumForNode(pNode, GetEncounterDesc(), iInc); }
		inline bool IsActive (void) { return (m_fInactive ? false : true); }
		inline bool IsOutOfPlaneObject (void) { return (m_fOutOfPlane ? true : false); }
		inline bool IsBeacon (void) { return (m_fBeacon ? true : false); }
		inline bool IsBlacklistEnabled (void) { return (m_fNoBlacklist ? false : true); }
		inline bool IsDestroyWhenEmpty (void) { return (m_fDestroyWhenEmpty ? true : false); }
		inline bool IsEnemyDockingAllowed (void) { return (m_fAllowEnemyDocking ? true : false); }
		inline bool IsImmutable (void) const { return (m_fImmutable ? true : false); }
		inline bool IsMultiHull (void) const { return (m_fMultiHull ? true : false); }
		inline bool IsMobile (void) const { return (m_fMobile ? true : false); }
		inline bool IsPaintLayerOverhang (void) const { return (m_fPaintOverhang ? true : false); }
		inline bool IsRadioactive (void) { return (m_fRadioactive ? true : false); }
		inline bool IsSign (void) { return (m_fSign ? true : false); }
		bool IsSizeClass (ESizeClass iClass) const;
		inline bool IsShipEncounter (void) { return (m_fShipEncounter ? true : false); }
		inline bool IsStatic (void) { return (m_fStatic ? true : false); }
		inline bool IsStationEncounter (void) { return (m_fStationEncounter ? true : false); }
		inline bool IsTimeStopImmune (void) { return (m_fTimeStopImmune ? true : false); }
		inline bool IsUniqueInSystem (void) const { return GetEncounterDesc().IsUniqueInSystem(); }
		inline bool IsWall (void) { return (m_fWall ? true : false); }
		void MarkImages (const CCompositeImageSelector &Selector);
		void OnShipEncounterCreated (SSystemCreateCtx &CreateCtx, CSpaceObject *pObj, const COrbit &Orbit);
		void PaintAnimations (CG32bitImage &Dest, int x, int y, int iTick);
		void PaintDevicePositions (CG32bitImage &Dest, int x, int y);
		void PaintDockPortPositions (CG32bitImage &Dest, int x, int y);
		void SetImageSelector (SSelectorInitCtx &InitCtx, CCompositeImageSelector *retSelector);
		inline void SetEncountered (CSystem *pSystem) { m_EncounterRecord.AddEncounter(pSystem); }
		inline void SetTempChance (int iChance) { m_iChance = iChance; }
		inline bool ShowsMapDetails (void) { return (m_fNoMapDetails ? false : true); }
		inline bool ShowsMapIcon (void) { return (m_fNoMapIcon ? false : true); }
		inline bool ShowsMapLabel (void) { return (m_fNoMapLabel ? false : true); }
		inline bool UsesReverseArticle (void) { return (m_fReverseArticle ? true : false); }

		//	CDesignType overrides
		static CStationType *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designStationType) ? (CStationType *)pType : NULL); }
		virtual bool FindDataField (const CString &sField, CString *retsValue) const override;
		virtual CCommunicationsHandler *GetCommsHandler (void) override;
		virtual int GetLevel (int *retiMinLevel = NULL, int *retiMaxLevel = NULL) const override;
		virtual CString GetNamePattern (DWORD dwNounFormFlags = 0, DWORD *retdwFlags = NULL) const override;
		virtual CTradingDesc *GetTradingDesc (void) const override { return m_pTrade; }
		virtual DesignTypes GetType (void) const override { return designStationType; }
        virtual const CCompositeImageDesc &GetTypeImage (void) const override { return m_Image; }
		virtual bool IsVirtual (void) const override { return (m_fVirtual ? true : false); }

		static Metric CalcSatelliteHitsToDestroy (CXMLElement *pSatellites, int iLevel, bool bIgnoreChance = false);
		static Metric CalcSatelliteStrength (CXMLElement *pSatellites, int iLevel, bool bIgnoreChance = false);
		static Metric CalcSatelliteTreasureValue (CXMLElement *pSatellites, int iLevel, bool bIgnoreChance = false);
		static ScaleTypes ParseScale (const CString sValue);
		static ESizeClass ParseSizeClass (const CString sValue);

	protected:
		//	CDesignType overrides
		virtual void OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override;
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual ALERROR OnFinishBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ICCItem *OnGetProperty (CCodeChainCtx &Ctx, const CString &sProperty) const override;
		virtual bool OnHasSpecialAttribute (const CString &sAttrib) const override;
		virtual void OnMarkImages (void) override;
		virtual void OnReadFromStream (SUniverseLoadCtx &Ctx) override;
		virtual void OnReinit (void) override;
		virtual void OnTopologyInitialized (void) override;
		virtual void OnUnbindDesign (void) override;
		virtual void OnWriteToStream (IWriteStream *pStream) override;

	private:
		struct SAnimationSection
			{
			int m_x;
			int m_y;
			CObjectImageArray m_Image;
			};

		void AddTypesUsedByXML (CXMLElement *pElement, TSortMap<DWORD, bool> *retTypesUsed);
		Metric CalcBalance (int iLevel) const;
		Metric CalcDefenderStrength (int iLevel) const;
		int CalcHitsToDestroy (int iLevel) const;
		Metric CalcTreasureValue (int iLevel) const;
		Metric CalcWeaponStrength (int iLevel) const;
		void InitStationDamage (void);

		CXMLElement *m_pDesc;

		//	Basic station descriptors
		CString m_sName;								//	Name
		DWORD m_dwNameFlags;							//	Flags
		CString m_sRandomNames;							//	Random names
		DWORD m_dwRandomNameFlags;						//	Flags
		CSovereignRef m_pSovereign;						//	Sovereign
		ScaleTypes m_iScale;							//	Scale
		Metric m_rParallaxDist;							//	Parallax distance for background objects
		int m_iLevel;									//	Station level
		Metric m_rMass;									//	Mass of station
														//		For stars, this is in solar masses
														//		For worlds, this is in Earth masses
														//		Otherwise, in metric tons
		int m_iSize;									//	Size
														//		For stars and worlds, this is in kilometers
														//		Otherwise, in meters
		int m_iFireRateAdj;								//	Fire rate adjustment

		//	Armor & HP
		CItemTypeRef m_pArmor;							//	Armor class
		int m_iHitPoints;								//	Hit points at creation time
		int m_iMaxHitPoints;							//	Max hit points
		CRegenDesc m_Regen;								//	Repair rate
		int m_iStealth;									//	Stealth

		int m_iStructuralHP;							//	Initial structural hit points
		int m_iMaxStructuralHP;							//	Max structural hp (0 = station is permanent)

		//	Devices
		int m_iDevicesCount;							//	Number of devices in array
		CInstalledDevice *m_Devices;					//	Device array

		//	Items
		IItemGenerator *m_pItems;						//	Random item table
		CTradingDesc *m_pTrade;							//	Trading structure

		DWORD m_fMobile:1;								//	Station moves
		DWORD m_fWall:1;								//	Station is a wall
		DWORD m_fInactive:1;							//	Station starts inactive
		DWORD m_fDestroyWhenEmpty:1;					//	Station is destroyed when last item removed
		DWORD m_fAllowEnemyDocking:1;					//	Station allows enemies to dock
		DWORD m_fNoFriendlyFire:1;						//	Station cannot hit friends
		DWORD m_fSign:1;								//	Station is a text sign
		DWORD m_fBeacon:1;								//	Station is a nav beacon

		DWORD m_fRadioactive:1;							//	Station is radioactive
		DWORD m_fCanAttack:1;							//	Station is active (i.e., will react if attacked)
		DWORD m_fShipEncounter:1;						//	This is a ship encounter
		DWORD m_fImmutable:1;							//	Station can not take damage or become radioactive, etc.
		DWORD m_fNoMapIcon:1;							//	Do not show on map
		DWORD m_fMultiHull:1;							//	Only harmed by WMD damage
		DWORD m_fTimeStopImmune:1;						//	TRUE if station is immune to time-stop
		DWORD m_fNoBlacklist:1;							//	Does not blacklist player if attacked

		DWORD m_fReverseArticle:1;						//	Use "a" instead of "an" and vice versa
		DWORD m_fStatic:1;								//	Use CStatic instead of CStation
		DWORD m_fOutOfPlane:1;							//	Background or foreground object
		DWORD m_fNoFriendlyTarget:1;					//	Station cannot be hit by friends
		DWORD m_fVirtual:1;								//	Virtual stations do not show up
		DWORD m_fPaintOverhang:1;						//	If TRUE, paint in overhang layer
		DWORD m_fCommsHandlerInit:1;					//	TRUE if comms handler has been initialized
		DWORD m_fNoMapDetails:1;                        //  If TRUE, do not show in details pane in galactic map

		DWORD m_fNoMapLabel:1;							//	If TRUE, do not show a label on system map
		DWORD m_fBuildReinforcements:1;					//	If TRUE, reinforcements are built instead of brought in
		DWORD m_fStationEncounter:1;					//	If TRUE, we're just an encounter wrapper that creates stations
		DWORD m_fSpare4:1;
		DWORD m_fSpare5:1;
		DWORD m_fSpare6:1;
		DWORD m_fSpare7:1;
		DWORD m_fSpare8:1;

		//	Images
		CCompositeImageDesc m_Image;
		TArray<DWORD> m_ShipWrecks;						//	Class IDs to use as image (for shipwrecks)
		int m_iAnimationsCount;							//	Number of animation sections
		SAnimationSection *m_pAnimations;				//	Animation sections (may be NULL)
		CCompositeImageDesc m_HeroImage;				//	For use in dock screens and covers

		//	Docking
		CDockScreenTypeRef m_pFirstDockScreen;			//	First screen (may be NULL)
		CDockScreenTypeRef m_pAbandonedDockScreen;		//	Screen to use when abandoned (may be NULL)
		DWORD m_dwDefaultBkgnd;							//	Default background screen

		//	Behaviors
		int m_iAlertWhenAttacked;						//	Chance that station will warn others when attacked
		int m_iAlertWhenDestroyed;						//	Chance that station will warn others when destroyed
		Metric m_rMaxAttackDistance;					//	Max range at which station guns attack

		CCommunicationsHandler m_OriginalCommsHandler;
		CCommunicationsHandler m_CommsHandler;			//	Communications handler

		//	Random occurrence
		CStationEncounterDesc m_RandomPlacement;		//	Random encounter information
		CStationEncounterCtx m_EncounterRecord;			//	Record of encounters so far

		//	Ships
		DiceRange m_ShipsCount;							//	Station should have this number of ship
		IShipGenerator *m_pInitialShips;				//	Ships at creation time
		IShipGenerator *m_pReinforcements;				//	Reinforcements table
		int m_iMinShips;								//	Min ships at station
		IShipGenerator *m_pEncounters;					//	Random encounters table
		int m_iEncounterFrequency;						//	Frequency of random encounter
		CRegenDesc m_ShipRegen;							//	Regen for ships docked with us
		IShipGenerator *m_pConstruction;				//	Ships built by station
		int m_iShipConstructionRate;					//	Ticks between each construction
		int m_iMaxConstruction;							//	Stop building when we get this many ships

		//	Satellites
		CXMLElement *m_pSatellitesDesc;

		//	Explosion
		CWeaponFireDescRef m_pExplosionType;			//	Explosion to create when station is destroyed

		//	Ejecta
		int m_iEjectaAdj;								//	Adjustment to probability for ejecta when hit by weapon
														//		0 = no chance of ejecta
														//		100 = normal chance
														//		>100 = greater than normal chance
		CWeaponFireDescRef m_pEjectaType;				//	Type of ejecta generated

		//	Stellar objects
		CG32bitPixel m_rgbSpaceColor;					//	Space color
		int m_iMaxLightDistance;						//	Max distance at which there is no (effective) light from star
		Metric m_rGravityRadius;						//	Gravity radius

		//	Stargates
		CString m_sStargateDestNode;					//	Dest node
		CString m_sStargateDestEntryPoint;				//	Dest entry point
		CEffectCreatorRef m_pGateEffect;				//	Effect when object gates in/out of station

		//	Miscellaneous
		CEffectCreatorRef m_pBarrierEffect;				//	Effect when object hits station
		CSovereignRef m_pControllingSovereign;			//	If controlled by different sovereign
														//	(e.g., centauri occupation)
		//	Temporary
		int m_iChance;									//	Used when computing chance of encounter

		static CIntegralRotationDesc m_DefaultRotation;
	};

//	CEconomyType --------------------------------------------------------------

class CEconomyType : public CDesignType
	{
	public:
		CEconomyType (void) { }

		CurrencyValue Exchange (CEconomyType *pFrom, CurrencyValue iAmount);
		inline CurrencyValue Exchange (const CCurrencyAndValue &Value) { return Exchange(Value.GetCurrencyType(), Value.GetValue()); }
		inline const CString &GetCurrencyNamePlural (void) { return m_sCurrencyPlural; }
		inline const CString &GetCurrencyNameSingular (void) { return m_sCurrencySingular; }
		inline const CString &GetSID (void) { return m_sSID; }
		inline bool IsCreditEquivalent (void) { return (m_iCreditConversion == 100); }

		static CEconomyType *Default (void);
		static CurrencyValue ExchangeToCredits (CEconomyType *pFrom, CurrencyValue iAmount);
		static CurrencyValue ExchangeToCredits (const CCurrencyAndValue &Value);
		static CString RinHackGet (CSpaceObject *pObj);
		static CurrencyValue RinHackInc (CSpaceObject *pObj, CurrencyValue iInc);
		static void RinHackSet (CSpaceObject *pObj, const CString &sData);

		//	CDesignType overrides
		static CEconomyType *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designEconomyType) ? (CEconomyType *)pType : NULL); }
		virtual bool FindDataField (const CString &sField, CString *retsValue) const override;
		virtual DesignTypes GetType (void) const override { return designEconomyType; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;

	private:
		CString m_sSID;									//	String ID (e.g., "credit")
		CString m_sCurrencyName;						//	Annotated name
		CString m_sCurrencySingular;					//	Singular form: "1 credit"
		CString m_sCurrencyPlural;						//	Plural form: "10 credits"; "You don't have enough credits"

		CurrencyValue m_iCreditConversion;				//	Commonwealth Credits that 100 units of the currency is worth
	};

//	CSovereign ----------------------------------------------------------------

enum DispositionClasses
	{
	alignNone =					-1,

	alignConstructiveChaos =	0,
	alignConstructiveOrder =	1,
	alignNeutral =				2,
	alignDestructiveOrder =		3,
	alignDestructiveChaos =		4,
	};

class CSovereign : public CDesignType
	{
	public:
		enum Alignments
			{
			alignUnknown =				-1,

			alignRepublic =				0,	//	constructive	community		knowledge		evolution		(constructive chaos)
			alignFederation =			1,	//	constructive	community		knowledge		tradition		(constructive order)
			alignUplifter =				2,	//	constructive	community		spirituality	evolution		(constructive chaos)
			alignFoundation =			3,	//	constructive	community		spirituality	tradition		(neutral)
			alignCompetitor =			4,	//	constructive	independence	knowledge		evolution		(neutral)
			alignArchivist =			5,	//	constructive	independence	knowledge		tradition		(constructive order)
			alignSeeker =				6,	//	constructive	independence	spirituality	evolution		(constructive chaos)
			alignHermit =				7,	//	constructive	independence	spirituality	tradition		(constructive order)

			alignCollective =			8,	//	destructive		community		knowledge		evolution		(destructive chaos)
			alignEmpire =				9,	//	destructive		community		knowledge		tradition		(destructive order)
			alignSterelizer =			10,	//	destructive		community		spirituality	evolution		(destructive chaos)
			alignCorrector =			11,	//	destructive		community		spirituality	tradition		(destructive order)
			alignMegalomaniac =			12,	//	destructive		independence	knowledge		evolution		(destructive chaos)
			alignCryptologue =			13,	//	destructive		independence	knowledge		tradition		(destructive order)
			alignPerversion =			14,	//	destructive		independence	spirituality	evolution		(destructive chaos)
			alignSolipsist =			15,	//	destructive		independence	spirituality	tradition		(destructive order)

			alignUnorganized =			16,	//	unorganized group of beings										(neutral)
			alignSubsapient =			17,	//	animals, zoanthropes, cyberorgs, and other creatures			(neutral)
			alignPredator =				18,	//	aggressive subsapient											(destructive chaos)

			alignCount =				19,
			};

		enum Disposition
			{
			dispEnemy = 0,
			dispNeutral = 1,
			dispFriend = 2,
			};

		enum EThreatLevels
			{
			threatNone =				0,	//	Player is not a threat to us
			threatMinorPiracy =			1,	//	Player is a minor threat to ships
			threatMinorRaiding =		2,	//	Player is a minor threat to stations
			threatMajor =				3,	//	Player is a major threat
			threatExistential =			4,	//	Player threatens our existence
			};

		CSovereign (void);
		~CSovereign (void);

		void DeleteRelationships (void);
		inline void FlushEnemyObjectCache (void) { m_pEnemyObjectsSystem = NULL; }
		IPlayerController *GetController (void);
		Disposition GetDispositionTowards (CSovereign *pSovereign, bool bCheckParent = true);
		inline const CSpaceObjectList &GetEnemyObjectList (CSystem *pSystem) { InitEnemyObjectList(pSystem); return m_EnemyObjects; }
		EThreatLevels GetPlayerThreatLevel (void) const;
		bool GetPropertyInteger (const CString &sProperty, int *retiValue);
		bool GetPropertyItemList (const CString &sProperty, CItemList *retItemList);
		bool GetPropertyString (const CString &sProperty, CString *retsValue);
		CString GetText (MessageTypes iMsg);
		inline bool IsEnemy (CSovereign *pSovereign) { return (m_bSelfRel || (pSovereign != this)) && (GetDispositionTowards(pSovereign) == dispEnemy); }
		inline bool IsFriend (CSovereign *pSovereign) { return (!m_bSelfRel && (pSovereign == this)) || (GetDispositionTowards(pSovereign) == dispFriend); }
		void MessageFromObj (CSpaceObject *pSender, const CString &sText);
		void OnObjDestroyedByPlayer (CSpaceObject *pObj);
		static Alignments ParseAlignment (const CString &sAlign);
		void SetDispositionTowards (CSovereign *pSovereign, Disposition iDisp, bool bMutual = false);
		void SetDispositionTowards (Alignments iAlignment, Disposition iDisp, bool bMutual = false);
		bool SetPropertyInteger (const CString &sProperty, int iValue);
		bool SetPropertyItemList (const CString &sProperty, const CItemList &ItemList);
		bool SetPropertyString (const CString &sProperty, const CString &sValue);

		//	CDesignType overrides
		static CSovereign *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designSovereign) ? (CSovereign *)pType : NULL); }
		virtual bool FindDataField (const CString &sField, CString *retsValue) const override;
		virtual CString GetNamePattern (DWORD dwNounFormFlags = 0, DWORD *retdwFlags = NULL) const;
		virtual DesignTypes GetType (void) const override { return designSovereign; }

		static Disposition ParseDisposition (const CString &sValue);

	protected:
		//	CDesignType overrides
		virtual void OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override;
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual ICCItem *OnGetProperty (CCodeChainCtx &Ctx, const CString &sProperty) const override;
		virtual ALERROR OnPrepareBindDesign (SDesignLoadCtx &Ctx) override;
		virtual void OnPrepareReinit (void) override;
		virtual void OnReadFromStream (SUniverseLoadCtx &Ctx) override;
		virtual void OnReinit (void) override;
		virtual void OnWriteToStream (IWriteStream *pStream) override;

	private:
		struct SRelationship
			{
			CSovereign *pSovereign;
			Disposition iDisp;

			SRelationship *pNext;
			};

		bool CalcSelfRel (void);
		SRelationship *FindRelationship (CSovereign *pSovereign, bool bCheckParent = false);
		inline Alignments GetAlignment (void) { return m_iAlignment; }
		void InitEnemyObjectList (CSystem *pSystem);
		void InitRelationships (void);

		CString m_sName;						//	":the United States of America"
		CString m_sShortName;					//	":the USA"
		CString m_sAdjective;					//	"American"
		CString m_sDemonym;						//	":an American(s)"
		bool m_bPluralForm;						//	"The United States ARE..."

		Alignments m_iAlignment;
		CXMLElement *m_pInitialRelationships;

		SRelationship *m_pFirstRelationship;	//	List of individual relationships

		int m_iStationsDestroyedByPlayer;		//	Number of our stations destroyed by the player
		int m_iShipsDestroyedByPlayer;			//	Number of our ships destroyed by the player

		bool m_bSelfRel;						//	TRUE if relationship with itself is not friendly
		CSystem *m_pEnemyObjectsSystem;			//	System that we've cached enemy objects
		CSpaceObjectList m_EnemyObjects;		//	List of enemy objects that can attack
	};

//	CPower --------------------------------------------------------------------

class CPower : public CDesignType
	{
	public:
		CPower (void);
		virtual ~CPower (void);

		inline ICCItem *GetCode (void) { return m_pCode; }
		inline int GetInvokeCost (void) { return m_iInvokeCost; }
		inline const CString &GetInvokeKey (void) { return m_sInvokeKey; }
		inline const CString &GetName (void) { return m_sName; }
		inline ICCItem *GetOnInvokedByPlayer (void) { return m_pOnInvokedByPlayer; }
		inline ICCItem *GetOnShow (void) { return m_pOnShow; }
		void Invoke (CSpaceObject *pSource, CSpaceObject *pTarget, CString *retsError = NULL);
		void InvokeByPlayer (CSpaceObject *pSource, CSpaceObject *pTarget, CString *retsError = NULL);
		bool OnShow (CSpaceObject *pSource, CSpaceObject *pTarget, CString *retsError = NULL);
		bool OnDestroyCheck (CSpaceObject *pSource, DestructionTypes iCause, const CDamageSource &Attacker);

		//	CDesignType overrides
		static CPower *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designPower) ? (CPower *)pType : NULL); }
		virtual DesignTypes GetType (void) const override { return designPower; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override { return NOERROR; }
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;

	private:
		CString m_sName;
		int m_iInvokeCost;
		CString m_sInvokeKey;

		ICCItem *m_pCode;
		ICCItem *m_pOnShow;
		ICCItem *m_pOnInvokedByPlayer;
		ICCItem *m_pOnDestroyCheck;
	};

//	CSpaceEnvironmentType -----------------------------------------------------

class CSpaceEnvironmentType : public CDesignType
	{
	public:
		CSpaceEnvironmentType (void) : m_iTileSize(0), m_bMarked(false) { }

		ALERROR FireOnUpdate (CSpaceObject *pObj, CString *retsError = NULL);
		inline Metric GetDragFactor (void) { return m_rDragFactor; }
		inline bool HasOnUpdateEvent (void) { return m_bHasOnUpdateEvent; }
		inline bool IsLRSJammer (void) { return m_bLRSJammer; }
		inline bool IsShieldJammer (void) { return m_bShieldJammer; }
		inline bool IsSRSJammer (void) { return m_bSRSJammer; }
		void Paint (CG32bitImage &Dest, int x, int y, int xTile, int yTile, DWORD dwEdgeMask);
		void PaintLRS (CG32bitImage &Dest, int x, int y);
		void PaintMap (CG32bitImage &Dest, int x, int y, int cxWidth, int cyHeight, DWORD dwFade, int xTile, int yTile, DWORD dwEdgeMask);

		//	CDesignType overrides
		static CSpaceEnvironmentType *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designSpaceEnvironmentType) ? (CSpaceEnvironmentType *)pType : NULL); }
		virtual DesignTypes GetType (void) const override { return designSpaceEnvironmentType; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual void OnClearMark (void) override { m_bMarked = false; }
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual void OnMarkImages (void) override;
		virtual void OnSweep (void) override;

	private:
		enum EConstants
			{
			TILES_IN_TILE_SET =				15,
			};

		struct STileDesc
			{
			CG8bitImage Mask;			//	Mask for edges
			CG16bitRegion Region;		//	Region created from mask
			};

		enum EEdgeTypes
			{
			edgeNone,

			edgeCloud,					//	Isolated (no edges)
			edgeStraight,				//	Open on one side only
			edgeCorner,					//	Open on two sides next to each other
			edgePeninsula,				//	Open on three sides
			edgeNarrow,					//	Open on two sides opposite each other
			};

		struct SEdgeDesc
			{
			EEdgeTypes iType;			//	Type of edge
			int iRotation;				//	Angle pointing from nebula to space

			//	Transform
			Metric rXx;
			Metric rXy;
			Metric rXc;					//	(as fraction of tile size)
			Metric rYx;
			Metric rYy;
			Metric rYc;					//	(as fraction of tile size)

			//	Wave parameters
			Metric rHalfWaveAngle;		//	Angle in half wave (radians)
			Metric rPixelsPerHalfWave;	//	Size of half wave as fraction of tile size
			Metric rWaveMin;			//	Min value of wave (to scale wave from 0.0-1.0)
			Metric rWaveScale;			//	To scale to 0.0-1.0
			Metric rMinAmplitude;		//	Minimum amplitude
			Metric rMaxAmplitude;		//	Max amplitude
			};

		void CreateAutoTileSet (int iVariants);
		void CreateEdgeTile (const SEdgeDesc &EdgeDesc, STileDesc *retTile);
		void CreateTileSet (const CObjectImageArray &Edges);
		int GetVariantOffset (int xTile, int yTile);

		CObjectImageArray m_Image;
		CObjectImageArray m_EdgeMask;
		int m_iImageTileCount;			//	Tiles in m_Image
		CG32bitPixel m_rgbMapColor;		//	Color of tile on map
		DWORD m_dwOpacity;				//	Opacity (0-255)

		bool m_bLRSJammer;				//	If TRUE, LRS is disabled
		bool m_bShieldJammer;			//	If TRUE, shields are disabled
		bool m_bSRSJammer;				//	If TRUE, SRS is disabled
		bool m_bHasOnUpdateEvent;		//	If TRUE, we have an OnUpdate event
		bool m_bAutoEdges;				//	If TRUE, we automatically created edges
		Metric m_rDragFactor;			//	Coefficient of drag (1.0 = no drag)

		//	We check for damage every 15 ticks
		int m_iDamageChance;			//	Chance of damage
		DamageDesc m_Damage;			//	Damage caused to objects in environment

		//	Generated tiles
		int m_iTileSize;				//	Size of tiles (in pixels)
		int m_iVariantCount;			//	Number of variants
		TArray<STileDesc> m_TileSet;
		bool m_bMarked;					//	Currently in use

		static SEdgeDesc EDGE_DATA[TILES_IN_TILE_SET];
	};

//	CShipTable ----------------------------------------------------------------

class CShipTable : public CDesignType
	{
	public:
		CShipTable (void);
		virtual ~CShipTable (void);

		inline void CreateShips (SShipCreateCtx &Ctx) { if (m_pGenerator) m_pGenerator->CreateShips(Ctx); }
		inline Metric GetAverageLevelStrength (int iLevel) { return (m_pGenerator ? m_pGenerator->GetAverageLevelStrength(iLevel) : 0.0); }
		ALERROR ValidateForRandomEncounter (void) { if (m_pGenerator) return m_pGenerator->ValidateForRandomEncounter(); }

		//	CDesignType overrides
		static CShipTable *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designShipTable) ? (CShipTable *)pType : NULL); }
		virtual DesignTypes GetType (void) const override { return designShipTable; }

	protected:
		//	CDesignType overrides
		virtual void OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override { if (m_pGenerator) m_pGenerator->AddTypesUsed(retTypesUsed); }
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;

	private:
		IShipGenerator *m_pGenerator;
	};

//	Missions ------------------------------------------------------------------

class CMissionType : public CDesignType
	{
	public:
		inline bool CanBeDeclined (void) const { return !m_fNoDecline; }
		inline bool CanBeDeleted (void) const { return m_fAllowDelete; }
		inline bool CanBeEncountered (void) const { return (m_iMaxAppearing == -1 || m_iAccepted < m_iMaxAppearing); }
		inline bool CloseIfOutOfSystem (void) const { return m_fCloseIfOutOfSystem; }
		inline bool FailureWhenOwnerDestroyed (void) const { return !m_fNoFailureOnOwnerDestroyed; }
		inline bool FailureWhenOutOfSystem (void) const { return (m_iFailIfOutOfSystem != -1); }
		inline bool ForceUndockAfterDebrief (void) const { return m_fForceUndockAfterDebrief; }
		inline const CString &GetName (void) const { return m_sName; }
		inline int GetExpireTime (void) const { return m_iExpireTime; }
		inline int GetOutOfSystemTimeOut (void) const { return m_iFailIfOutOfSystem; }
		inline int GetPriority (void) const { return m_iPriority; }
		inline bool HasDebrief (void) const { return !m_fNoDebrief; }
		inline void IncAccepted (void) { m_iAccepted++; }
		inline bool KeepsStats (void) const { return !m_fNoStats; }

		//	CDesignType overrides

		static CMissionType *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designMissionType) ? (CMissionType *)pType : NULL); }
		virtual bool FindDataField (const CString &sField, CString *retsValue) const override;
		virtual int GetLevel (int *retiMinLevel = NULL, int *retiMaxLevel = NULL) const override { if (retiMinLevel) *retiMinLevel = m_iMinLevel; if (retiMaxLevel) *retiMaxLevel = m_iMaxLevel; return (m_iMinLevel + m_iMaxLevel) / 2; }
		virtual DesignTypes GetType (void) const override { return designMissionType; }

	protected:
		//	CDesignType overrides

		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual void OnReadFromStream (SUniverseLoadCtx &Ctx) override;
		virtual void OnReinit (void) override;
		virtual void OnWriteToStream (IWriteStream *pStream) override;

	private:
		//	Basic properties

		CString m_sName;					//	Internal name
		int m_iPriority;					//	Relative priority (default = 1)

		//	Mission creation

		int m_iMinLevel;					//	Minimum system level supported
		int m_iMaxLevel;					//	Maximum system level supported
		DiceRange m_MaxAppearing;			//	Limit to number of times mission has been accepted by player
											//		(NULL if no limit)
		int m_iExpireTime;					//	Mission expires after this amount
											//		of time if not accepted.
		int m_iFailIfOutOfSystem;			//	If player is out of mission system for this amount of time
											//		(in ticks) then mission fails (-1 means on timeout)

		//	Mission stats

		int m_iMaxAppearing;				//	Limit to number of times mission can appear (-1 = no limit)
		int m_iAccepted;					//	Number of times player has accepted this mission type

		DWORD m_fNoFailureOnOwnerDestroyed:1;	//	If TRUE, mission does not fail when owner destroyed
		DWORD m_fNoDebrief:1;				//	If TRUE, mission is closed on success
		DWORD m_fNoStats:1;					//	If TRUE, mission is not included in count of missions assigned/completed
		DWORD m_fCloseIfOutOfSystem:1;		//	If TRUE, mission is closed if player leaves system.
		DWORD m_fForceUndockAfterDebrief:1;	//	If TRUE, default mission screen undocks after debrief
		DWORD m_fAllowDelete:1;				//	If TRUE, player can delete mission
		DWORD m_fNoDecline:1;				//	If TRUE, mission cannot be declined once offered.
		DWORD m_fSpare8:1;

		DWORD m_dwSpare:24;
	};

//	Adventures and Extensions -------------------------------------------------

class CAdventureDesc : public CDesignType
	{
	public:
		CAdventureDesc (void);

		void FireOnGameEnd (const CGameRecord &Game, const SBasicGameStats &BasicStats);
		void FireOnGameStart (void);
		inline const CDamageAdjDesc *GetArmorDamageAdj (int iLevel) const { return &m_ArmorDamageAdj[iLevel - 1]; }
		inline DWORD GetBackgroundUNID (void) { return m_dwBackgroundUNID; }
		CString GetDesc (void);
		const CStationEncounterDesc *GetEncounterDesc (DWORD dwUNID) const;
		inline DWORD GetExtensionUNID (void) { return m_dwExtensionUNID; }
		inline const CString &GetName (void) { return m_sName; }
		inline const CDamageAdjDesc *GetShieldDamageAdj (int iLevel) const { return &m_ShieldDamageAdj[iLevel - 1]; }
		inline DWORD GetStartingMapUNID (void) { return m_dwStartingMap; }
		inline const CString &GetStartingNodeID (void) { return m_sStartingNodeID; }
		inline const CString &GetStartingPos (void) { return m_sStartingPos; }
		ALERROR GetStartingShipClasses (TSortMap<CString, CShipClass *> *retClasses, CString *retsError);
		inline const CString &GetWelcomeMessage (void) { return m_sWelcomeMessage; }
		bool InitEncounterOverrides (CString *retsError = NULL);
		inline bool IsCurrentAdventure (void) { return (m_fIsCurrentAdventure ? true : false); }
		inline bool IsInDefaultResource (void) { return (m_fInDefaultResource ? true : false); }
		bool IsValidStartingClass (CShipClass *pClass);
		inline void SetCurrentAdventure (bool bCurrent = true) { m_fIsCurrentAdventure = bCurrent; }

		//	CDesignType overrides

		static CAdventureDesc *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designAdventureDesc) ? (CAdventureDesc *)pType : NULL); }
		virtual bool FindDataField (const CString &sField, CString *retsValue) const override;
		virtual DesignTypes GetType (void) const override { return designAdventureDesc; }

		//	Helpers

		static const CDamageAdjDesc *GetDefaultArmorDamageAdj (int iLevel);
		static const CDamageAdjDesc *GetDefaultShieldDamageAdj (int iLevel);

	protected:
		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual void OnUnbindDesign (void) override { m_fIsCurrentAdventure = false; }

	private:
		static void InitDefaultDamageAdj (void);

		DWORD m_dwExtensionUNID;

		CString m_sName;						//	Name of adventure
		DWORD m_dwBackgroundUNID;				//	Background image to use for choice screen
		CString m_sWelcomeMessage;				//	Equivalent of "Welcome to Transcendence!"

		CDesignTypeCriteria m_StartingShips;	//	Starting ship criteria
		DWORD m_dwStartingMap;					//	Default system map to load
		CString m_sStartingNodeID;				//	NodeID where we start
		CString m_sStartingPos;					//	Named object at which we start

		CDamageAdjDesc m_ArmorDamageAdj[MAX_ITEM_LEVEL];
		CDamageAdjDesc m_ShieldDamageAdj[MAX_ITEM_LEVEL];

		CXMLElement m_EncounterOverridesXML;
		TSortMap<DWORD, CStationEncounterDesc> m_Encounters;

		DWORD m_fIsCurrentAdventure:1;			//	TRUE if this is the current adventure
		DWORD m_fInDefaultResource:1;			//	TRUE if adventure is a module in the default resource
		DWORD m_fIncludeOldShipClasses:1;		//	TRUE if we should include older extensions (even if 
												//		they don't match starting ship criteria).
		DWORD m_fInInitEncounterOverrides:1;	//	TRUE if we're initializing this.
	};

//	Name Generator -------------------------------------------------------------

class CNameGenerator : public CDesignType
	{
	public:
		CNameGenerator (void);
		virtual ~CNameGenerator (void);

		//	CDesignType overrides
		static CNameGenerator *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designNameGenerator) ? (CNameGenerator *)pType : NULL); }
		virtual DesignTypes GetType (void) const override { return designNameGenerator; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;

	private:
	};

//	System Map -----------------------------------------------------------------

class ITopologyProcessor
	{
	public:
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID, ITopologyProcessor **retpProc);
		static ALERROR CreateFromXMLAsGroup (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID, ITopologyProcessor **retpProc);

		virtual ~ITopologyProcessor (void) { }
		inline ALERROR BindDesign (SDesignLoadCtx &Ctx) { return OnBindDesign(Ctx); }
		inline CEffectCreator *FindEffectCreator (const CString &sUNID) { return OnFindEffectCreator(sUNID); }
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID);
		inline ALERROR Process (CSystemMap *pMap, CTopology &Topology, CTopologyNodeList &NodeList, CString *retsError) { return OnProcess(pMap, Topology, NodeList, retsError); }

	protected:
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) { return NOERROR; }
		virtual CEffectCreator *OnFindEffectCreator (const CString &sUNID) { return NULL; }
		virtual ALERROR OnInitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) { return NOERROR; }
		virtual ALERROR OnProcess (CSystemMap *pMap, CTopology &Topology, CTopologyNodeList &NodeList, CString *retsError) { return NOERROR; }

		CTopologyNodeList *FilterNodes (CTopology &Topology, CTopologyNode::SCriteria &Criteria, CTopologyNodeList &Unfiltered, CTopologyNodeList &Filtered);
		void RestoreMarks (CTopology &Topology, TArray<bool> &Saved);
		void SaveAndMarkNodes (CTopology &Topology, CTopologyNodeList &NodeList, TArray<bool> *retSaved);
	};

class CSystemMap : public CDesignType
	{
	public:
		struct SNodeExtra
			{
			CTopologyNode *pNode;
			int iStargateCount;
			};

		struct SSortEntry
			{
			SNodeExtra *pExtra;
			int iSort;
			};

		CSystemMap (void) : m_bAdded(false), m_bDebugShowAttributes(false) { }
		virtual ~CSystemMap (void);

		void AddAnnotation (CEffectCreator *pEffect, int x, int y, int iRotation, DWORD *retdwID = NULL);
		ALERROR AddFixedTopology (CTopology &Topology, TSortMap<DWORD, CTopologyNodeList> &NodesAdded, CString *retsError);
		bool DebugShowAttributes (void) const { return m_bDebugShowAttributes; }
		CG32bitImage *CreateBackgroundImage (void);
		void GetBackgroundImageSize (int *retcx, int *retcy);
		inline CSystemMap *GetDisplayMap (void) { return (m_pPrimaryMap != NULL ? m_pPrimaryMap : this); }
        inline Metric GetLightYearsPerPixel (void) const { return m_rLightYearsPerPixel; }
		inline const CString &GetName (void) const { return m_sName; }
		inline void GetScale (int *retiInitial, int *retiMin, int *retiMax) { if (retiInitial) *retiInitial = m_iInitialScale; if (retiMin) *retiMin = m_iMinScale; if (retiMax) *retiMax = m_iMaxScale; }
        inline CG32bitPixel GetStargateLineColor (void) const { return m_rgbStargateLines; }
		inline const CString &GetStartingNodeID (void) { return m_FixedTopology.GetFirstNodeID(); }
		inline bool IsPrimaryMap (void) const { return (m_pPrimaryMap == NULL); }
		inline bool IsStartingMap (void) const { return m_bStartingMap; }
		ALERROR ProcessTopology (CTopology &Topology, CSystemMap *pTargetMap, CTopologyNodeList &NodesAdded, CString *retsError);

		//	CDesignType overrides
		static CSystemMap *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designSystemMap) ? (CSystemMap *)pType : NULL); }
		virtual DesignTypes GetType (void) const override { return designSystemMap; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual CEffectCreator *OnFindEffectCreator (const CString &sUNID) override;
		virtual void OnReadFromStream (SUniverseLoadCtx &Ctx) override;
		virtual void OnReinit (void) override;
		virtual void OnWriteToStream (IWriteStream *pStream) override;

	private:
		struct SMapAnnotation
			{
			DWORD dwID;

			IEffectPainter *pPainter;
			int xOffset;
			int yOffset;
			int iTick;
			int iRotation;
			};

		ALERROR ExecuteCreator (STopologyCreateCtx &Ctx, CTopology &Topology, CXMLElement *pCreator);

		CString m_sName;						//	Name of the map (for the player)
		DWORD m_dwBackgroundImage;				//	Background image to use
		int m_iInitialScale;					//	Initial map display scale (100 = 100%)
		int m_iMaxScale;						//	Max zoom
		int m_iMinScale;						//	Min zoom
        Metric m_rLightYearsPerPixel;           //  Number of light years per pixel
        CG32bitPixel m_rgbStargateLines;        //  Color of stargate line

		CSystemMapRef m_pPrimaryMap;			//	If not NULL, place nodes on given map
		TArray<CSystemMapRef> m_Uses;			//	List of maps that we rely on.

		//	Topology generation
		CTopologyDescTable m_FixedTopology;
		TArray<CXMLElement *> m_Creators;
		TArray<ITopologyProcessor *> m_Processors;
		bool m_bStartingMap;					//	Do not load unless specified by player ship

		//	Annotations
		TArray<SMapAnnotation> m_Annotations;

		//	Temporaries
		bool m_bAdded;							//	TRUE if map was added to topology

		//	Debug
		bool m_bDebugShowAttributes;
	};

int KeyCompare (const CSystemMap::SSortEntry &Key1, const CSystemMap::SSortEntry &Key2);

//	System Tables --------------------------------------------------------------

class CSystemTable : public CDesignType
	{
	public:
		CSystemTable (void) : m_pTable(NULL) { }
		virtual ~CSystemTable (void);

		CXMLElement *FindElement (const CString &sElement) const { return (m_pTable ? m_pTable->GetContentElementByTag(sElement) : NULL); }

		//	CDesignType overrides
		static CSystemTable *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designSystemTable) ? (CSystemTable *)pType : NULL); }
		virtual DesignTypes GetType (void) const override { return designSystemTable; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override { m_pTable = pDesc->OrphanCopy(); return NOERROR; }

	private:
		CXMLElement *m_pTable;
	};

//	Template Types -------------------------------------------------------------

class CTemplateType : public CDesignType
	{
	public:
		CTemplateType (void) { }
		virtual ~CTemplateType (void) { }

		//	CDesignType overrides
		static CTemplateType *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designTemplateType) ? (CTemplateType *)pType : NULL); }
		virtual DesignTypes GetType (void) const override { return designTemplateType; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override { return NOERROR; }

	private:
	};

//	Associated Structures ------------------------------------------------------

class CInstalledDevice
	{
	public:
		CInstalledDevice (void);
		~CInstalledDevice (void);

		CInstalledDevice &operator= (const CInstalledDevice &Obj);

		//	Create/Install/uninstall/Save/Load methods

		void FinishInstall (CSpaceObject *pSource);
		inline CDeviceClass *GetClass (void) const { return m_pClass; }
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

		inline int GetCharges (CSpaceObject *pSource) { return (m_pItem ? m_pItem->GetCharges() : 0); }
		inline DWORD GetData (void) const { return m_dwData; }
		inline int GetDeviceSlot (void) const { return m_iDeviceSlot; }
		inline CItemEnhancementStack *GetEnhancements (void) const { return m_pEnhancements; }
		inline int GetFireArc (void) const { return (IsOmniDirectional() ? 360 : AngleRange(m_iMinFireArc, m_iMaxFireArc)); }
		inline int GetFireAngle (void) const { return m_iFireAngle; }
		int GetHitPointsPercent (CSpaceObject *pSource);
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
		void SetEnhancements (CItemEnhancementStack *pStack);
		inline void SetExternal (bool bValue) { m_fExternal = bValue; }
		inline void SetFireAngle (int iAngle) { m_iFireAngle = iAngle; }
		inline void SetFireArc (int iMinFireArc, int iMaxFireArc) { m_iMinFireArc = iMinFireArc; m_iMaxFireArc = iMaxFireArc; }
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
		inline bool CanHitFriends (void) { return m_pClass->CanHitFriends(); }
		inline bool CanRotate (CItemCtx &Ctx) const { return m_pClass->CanRotate(Ctx); }
		inline void Deplete (CSpaceObject *pSource) { m_pClass->Deplete(this, pSource); }
		int GetActivateDelay (CSpaceObject *pSource);
		inline ItemCategories GetCategory (void) const { return m_pClass->GetCategory(); }
		inline int GetCounter (CSpaceObject *pSource, CDeviceClass::CounterTypes *retiCounter = NULL, int *retiLevel = NULL) { return m_pClass->GetCounter(this, pSource, retiCounter, retiLevel); }
		inline const DamageDesc *GetDamageDesc (CItemCtx &Ctx) { return m_pClass->GetDamageDesc(Ctx); }
		inline int GetDamageEffectiveness (CSpaceObject *pAttacker, CInstalledDevice *pWeapon) { return m_pClass->GetDamageEffectiveness(pAttacker, pWeapon); }
		inline int GetDamageType (CItemCtx &Ctx, const CItem &Ammo = CItem()) { return m_pClass->GetDamageType(Ctx, Ammo); }
		inline int GetDefaultFireAngle (CSpaceObject *pSource) { return m_pClass->GetDefaultFireAngle(this, pSource); }
		bool GetDeviceEnhancementDesc (CSpaceObject *pSource, CInstalledDevice *pWeapon, SDeviceEnhancementDesc *retDesc) { return m_pClass->GetDeviceEnhancementDesc(this, pSource, pWeapon, retDesc); }
		CSpaceObject *GetLastShot (CSpaceObject *pSource, int iIndex) const;
		inline Metric GetMaxEffectiveRange (CSpaceObject *pSource, CSpaceObject *pTarget = NULL) { return m_pClass->GetMaxEffectiveRange(pSource, this, pTarget); }
		inline Metric GetMaxRange (CItemCtx &ItemCtx) { return m_pClass->GetMaxRange(ItemCtx); }
		inline CString GetName (void) { return m_pClass->GetName(); }
		CVector GetPos (CSpaceObject *pSource);
		CVector GetPosOffset (CSpaceObject *pSource);
		inline int GetPowerRating (CItemCtx &Ctx) const { return m_pClass->GetPowerRating(Ctx); }
		inline void GetSelectedVariantInfo (CSpaceObject *pSource, 
											CString *retsLabel,
											int *retiAmmoLeft,
											CItemType **retpType = NULL)
			{ m_pClass->GetSelectedVariantInfo(pSource, this, retsLabel, retiAmmoLeft, retpType); }
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
		inline void Recharge (CShip *pShip, int iStatus) { m_pClass->Recharge(this, pShip, iStatus); }
		inline void Reset (CSpaceObject *pShip) { m_pClass->Reset(this, pShip); }
		inline void SelectFirstVariant (CSpaceObject *pSource) { m_pClass->SelectFirstVariant(pSource, this); }
		inline void SelectNextVariant (CSpaceObject *pSource, int iDir = 1) { m_pClass->SelectNextVariant(pSource, this, iDir); }
		void SetCharges (CSpaceObject *pSource, int iCharges);
		void SetLastShot (CSpaceObject *pObj, int iIndex);
		void SetLastShotCount (int iCount);
		inline void SetTarget (CSpaceObject *pObj);
		inline bool ShowActivationDelayCounter (CSpaceObject *pSource) { return m_pClass->ShowActivationDelayCounter(pSource, this); }

		//	These are wrapper methods for the CItem behind this device.

		inline DWORD GetDisruptedDuration (void) const { return (m_pItem ? m_pItem->GetDisruptedDuration() : 0); }
		inline bool GetDisruptedStatus (DWORD *retdwTimeLeft = NULL, bool *retbRepairedEvent = NULL) const { return (m_pItem ? m_pItem->GetDisruptedStatus(retdwTimeLeft, retbRepairedEvent) : false); }
		CString GetEnhancedDesc (CSpaceObject *pSource, const CItem *pItem = NULL);
		inline const CItemEnhancement &GetMods (void) const { return (m_pItem ? m_pItem->GetMods() : CItem::GetNullMod()); }
		inline bool IsDamaged (void) const { return (m_pItem ? m_pItem->IsDamaged() : false); }
		inline bool IsDisrupted (void) const { return (m_pItem ? m_pItem->IsDisrupted() : false); }
		inline bool IsEnhanced (void) const { return (m_pItem ? m_pItem->IsEnhanced() : false); }
		inline bool IsWeaponAligned (CSpaceObject *pShip, CSpaceObject *pTarget, int *retiAimAngle = NULL, int *retiFireAngle = NULL) { return m_pClass->IsWeaponAligned(pShip, this, pTarget, retiAimAngle, retiFireAngle); }

	private:
		CItem *m_pItem;							//	Item installed in this slot
		CDeviceClassRef m_pClass;				//	The device class that is installed here
		COverlay *m_pOverlay;					//	Overlay (if associated)
		DWORD m_dwTargetID;						//	ObjID of target (for tracking secondary weapons)
		CItemEnhancementStack *m_pEnhancements;	//	List of enhancements (may be NULL)
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
		DWORD m_fSpare8:1;

		DWORD m_dwSpare:16;						//	Spare flags
	};

//	CDesignCollection

class CDesignList
	{
	public:
		CDesignList (void) : m_List(128) { }
		~CDesignList (void) { }

		inline void AddEntry (CDesignType *pType) { m_List.Insert(pType); }
		void Delete (DWORD dwUNID);
		void DeleteAll (bool bFree = false);
		inline int GetCount (void) const { return m_List.GetCount(); }
		inline CDesignType *GetEntry (int iIndex) const { return m_List[iIndex]; }

	private:
		TArray<CDesignType *> m_List;
	};

class CDesignTable
	{
	public:
		CDesignTable (bool bFreeTypes = false) : m_bFreeTypes(bFreeTypes) { }
		~CDesignTable (void) { DeleteAll(); }

		ALERROR AddEntry (CDesignType *pEntry);
		ALERROR AddOrReplaceEntry (CDesignType *pEntry, CDesignType **retpOldEntry = NULL);
		void Delete (DWORD dwUNID);
		void DeleteAll (void);
		CDesignType *FindByUNID (DWORD dwUNID) const;
		inline int GetCount (void) const { return m_Table.GetCount(); }
		inline CDesignType *GetEntry (int iIndex) const { return m_Table.GetValue(iIndex); }
		ALERROR Merge (const CDesignTable &Source, CDesignList *ioOverride = NULL, const TArray<DWORD> *pExtensionsIncluded = NULL);
		ALERROR Merge (const CDynamicDesignTable &Source, CDesignList *ioOverride = NULL);

	private:
		TSortMap<DWORD, CDesignType *> m_Table;
		bool m_bFreeTypes;
	};

enum EExtensionTypes
	{
	extUnknown,

	extBase,
	extAdventure,
	extLibrary,
	extExtension,
	};

enum EGameTypes
	{
	gameUnknown,

	gameAmerica,							//	CSC America
	gameTranscendence,						//	Transcendence
	};

class CExtension
	{
	public:
		enum ELoadStates
			{
			loadNone,

			loadEntities,					//	We've loaded the entities, but nothing else.
			loadAdventureDesc,				//	We've loaded adventure descriptors, but no other types
			loadComplete,					//	We've loaded all design types.
			};

		enum EFolderTypes
			{
			folderUnknown,

			folderBase,						//	Base folder (only for base XML)
			folderCollection,				//	Collection folder
			folderExtensions,				//	Extensions folder
			};

		struct SLibraryDesc
			{
			DWORD dwUNID;					//	UNID of library that we use
			DWORD dwRelease;				//	Release of library that we use
			};

		struct SLoadOptions
			{
			SLoadOptions (void) :
					bKeepXML(false),
					bNoResources(false),
					bNoDigestCheck(false)
				{ }

			bool bKeepXML;
			bool bNoResources;
			bool bNoDigestCheck;
			};

		CExtension (void);
		~CExtension (void);

		static ALERROR CreateBaseFile (SDesignLoadCtx &Ctx, EGameTypes iGame, CXMLElement *pDesc, CExternalEntityTable *pEntities, CExtension **retpBase, TArray<CXMLElement *> *retEmbedded);
		static ALERROR CreateExtension (SDesignLoadCtx &Ctx, CXMLElement *pDesc, EFolderTypes iFolder, CExternalEntityTable *pEntities, CExtension **retpExtension);
		static ALERROR CreateExtensionStub (const CString &sFilespec, EFolderTypes iFolder, CExtension **retpExtension, CString *retsError);

		bool CanExtend (CExtension *pAdventure) const;
		void CleanUp (void);
		void CreateIcon (int cxWidth, int cyHeight, CG32bitImage **retpIcon) const;
		ALERROR ExecuteGlobals (SDesignLoadCtx &Ctx);
		inline CAdventureDesc *GetAdventureDesc (void) const { return m_pAdventureDesc; }
		inline DWORD GetAPIVersion (void) const { return m_dwAPIVersion; }
		inline DWORD GetAutoIncludeAPIVersion (void) const { return m_dwAutoIncludeAPIVersion; }
		CG32bitImage *GetCoverImage (void) const;
		inline const TArray<CString> &GetCredits (void) const { return m_Credits; }
		inline const CString &GetDisabledReason (void) const { return m_sDisabledReason; }
		inline CString GetDesc (void) { return (m_pAdventureDesc ? m_pAdventureDesc->GetDesc() : NULL_STR); }
		inline const CDesignTable &GetDesignTypes (void) { return m_DesignTypes; }
		inline const CIntegerIP &GetDigest (void) const { return m_Digest; }
		inline CExternalEntityTable *GetEntities (void) { return m_pEntities; }
		CString GetEntityName (DWORD dwUNID) const;
		inline const TArray<CString> &GetExternalResources (void) const { return m_Externals; }
		inline const CString &GetFilespec (void) const { return m_sFilespec; }
		inline EFolderTypes GetFolderType (void) const { return m_iFolderType; }
		inline const SLibraryDesc &GetLibrary (int iIndex) const { return m_Libraries[iIndex]; }
		inline int GetLibraryCount (void) const { return m_Libraries.GetCount(); }
		inline ELoadStates GetLoadState (void) const { return m_iLoadState; }
		inline DWORD GetMinExtensionAPIVersion (void) const { return m_dwMinExtensionAPIVersion; }
		inline const CTimeDate &GetModifiedTime (void) const { return m_ModifiedTime; }
		inline const CString &GetName (void) const { return m_sName; }
		inline CTopologyDescTable &GetTopology (void) { return m_Topology; }
		inline DWORD GetRelease (void) const { return m_dwRelease; }
		inline EExtensionTypes GetType (void) const { return m_iType; }
		inline DWORD GetUNID (void) const { return m_dwUNID; }
		inline const CString &GetVersion (void) const { return m_sVersion; }
		inline bool IsAutoInclude (void) const { return m_bAutoInclude; }
		inline bool IsDebugOnly (void) const { return m_bDebugOnly; }
		inline bool IsDisabled (void) const { return m_bDisabled; }
		inline bool IsHidden (void) const { return m_bHidden; }
		inline bool IsMarked (void) const { return m_bMarked; }
		inline bool IsOfficial (void) const { return ((m_dwUNID & 0xFF000000) < 0xA0000000); }
		inline bool IsPrivate (void) const { return m_bPrivate; }
		inline bool IsRegistered (void) const { return m_bRegistered; }
		inline bool IsRegistrationVerified (void) { return (m_bRegistered && m_bVerified); }
		ALERROR Load (ELoadStates iDesiredState, IXMLParserController *pResolver, const SLoadOptions &Options, CString *retsError);
		inline void SetDeleted (void) { m_bDeleted = true; }
		inline void SetDisabled (const CString &sReason) { if (!m_bDisabled) { m_sDisabledReason = sReason; m_bDisabled = true; } }
		inline void SetDigest (const CIntegerIP &Digest) { m_Digest = Digest; }
		inline void SetMarked (bool bMarked = true) { m_bMarked = bMarked; }
		inline void SetModifiedTime (const CTimeDate &Time) { m_ModifiedTime = Time; }
		inline void SetName (const CString &sName) { m_sName = sName; }
		inline void SetVerified (bool bVerified = true) { m_bVerified = bVerified; }
		void SweepImages (void);
		inline bool UsesCompatibilityLibrary (void) const { return m_bUsesCompatibilityLibrary; }
		inline bool UsesXML (void) const { return m_bUsesXML; }

		static ALERROR ComposeLoadError (SDesignLoadCtx &Ctx, CString *retsError);
		static void DebugDump (CExtension *pExtension, bool bFull = false);

	private:
		struct SGlobalsEntry
			{
			CString sFilespec;
			ICCItem *pCode;
			};

		static ALERROR CreateExtensionFromRoot (const CString &sFilespec, CXMLElement *pDesc, EFolderTypes iFolder, CExternalEntityTable *pEntities, DWORD dwInheritAPIVersion, CExtension **retpExtension, CString *retsError);

		void AddEntityNames (CExternalEntityTable *pEntities, TSortMap<DWORD, CString> *retMap) const;
		void AddLibraryReference (SDesignLoadCtx &Ctx, DWORD dwUNID = 0, DWORD dwRelease = 0);
		void AddDefaultLibraryReferences (SDesignLoadCtx &Ctx);
		void CleanUpXML (void);
		ALERROR LoadDesignElement (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		ALERROR LoadDesignType (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CDesignType **retpType = NULL);
		ALERROR LoadGlobalsElement (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		ALERROR LoadLibraryElement (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		ALERROR LoadModuleContent (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		ALERROR LoadModuleElement (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		ALERROR LoadModulesElement (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		ALERROR LoadResourcesElement (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		ALERROR LoadSystemTypesElement (SDesignLoadCtx &Ctx, CXMLElement *pDesc);

		CString m_sFilespec;				//	Extension file
		DWORD m_dwUNID;						//	UNID of extension
		EGameTypes m_iGame;					//	Game that this extension belongs to
		EExtensionTypes m_iType;			//	Either adventure, extension, or base

		ELoadStates m_iLoadState;			//	Current load state
		EFolderTypes m_iFolderType;			//	Folder that extension came from
		CTimeDate m_ModifiedTime;			//	Timedate of extension file
		CIntegerIP m_Digest;				//	Digest (for registered files)
		DWORD m_dwAPIVersion;				//	Version of API that we're using
		CExternalEntityTable *m_pEntities;	//	Entities defined by this extension
		CString m_sDisabledReason;			//	Reason why extension is disabled

		CString m_sName;					//	Extension name
		DWORD m_dwRelease;					//	Release number
		CString m_sVersion;					//	User-visible version number
		DWORD m_dwCoverUNID;				//	UNID of cover image
		CDesignTable m_DesignTypes;			//	Design types defined by extension
		CTopologyDescTable m_Topology;		//	Topology defined by extension 
											//		(for backwards compatibility)
		TArray<SGlobalsEntry> m_Globals;	//	Globals
		TArray<CString> m_Credits;			//	List of names for credits

		TArray<SLibraryDesc> m_Libraries;	//	Extensions that we use.
		TArray<DWORD> m_Extends;			//	UNIDs that this extension extends
		DWORD m_dwMinExtensionAPIVersion;	//	Do not allow extensions older than this
		DWORD m_dwAutoIncludeAPIVersion;	//	Library adds compatibility to any
											//		extension at or below this
											//		API version.
		TArray<CString> m_Externals;		//	External resources

		CXMLElement *m_pRootXML;			//	Root XML representation (may be NULL)
		TSortMap<CString, CXMLElement *> m_ModuleXML;	//	XML for modules

		mutable CG32bitImage *m_pCoverImage;	//	Large cover image

		CAdventureDesc *m_pAdventureDesc;	//	If extAdventure, this is the descriptor

		mutable TSortMap<DWORD, CString> m_UNID2EntityName;

		bool m_bMarked;						//	Used by CExtensionCollection for various things
		bool m_bDebugOnly;					//	Only load in debug mode
		bool m_bRegistered;					//	UNID indicates this is a registered extension
		bool m_bVerified;					//	Signature and license verified
		bool m_bPrivate;					//	Do not show in stats
		bool m_bDisabled;					//	Disabled (for some reason)
		bool m_bDeleted;
		bool m_bAutoInclude;				//	Extension should always be included (if appropriate)
		bool m_bUsesXML;					//	Extension uses XML from other extensions
		bool m_bUsesCompatibilityLibrary;	//	Extension needs the compatibility library
		bool m_bHidden;						//	Available only for backwards compatibility
	};

class CExtensionCollection
	{
	public:
		enum Flags
			{
			//	Load

			FLAG_NO_RESOURCES =			0x00000001,	//	Do not load resources.
			FLAG_DEBUG_MODE =			0x00000002,	//	Game run with /debug
			FLAG_DESC_ONLY =			0x00000004,	//	Load adventure descs only
			FLAG_ERROR_ON_DISABLE =		0x00000008,	//	Return an error if an extension was loaded disabled
												//		(due to missing dependencies, etc.)
            FLAG_NO_COLLECTION =		0x00000010, //  Do not load collection
			FLAG_NO_COLLECTION_CHECK =	0x00000020,	//	Do not check signatures on collection

			//	FindExtension

			FLAG_ADVENTURE_ONLY =		0x00000040,	//	Must be an adventure (not found otherwise)

			//	ComputeAvailableExtension

			FLAG_INCLUDE_AUTO =			0x00000080,	//	Include extensions that are automatic
			FLAG_AUTO_ONLY =			0x00000100,	//	Only include extensions that are automatic
			FLAG_ACCUMULATE =			0x00000200,	//	Add to result list
			FLAG_REGISTERED_ONLY =		0x00000400,	//	Only registered extensions

			//	ComputeBindOrder

			FLAG_FORCE_COMPATIBILITY_LIBRARY = 0x00000800,
			};

		CExtensionCollection (void);
		~CExtensionCollection (void);

		inline void AddExtensionFolder (const CString &sFilespec) { m_ExtensionFolders.Insert(sFilespec); }
		void CleanUp (void);
		ALERROR ComputeAvailableAdventures (DWORD dwFlags, TArray<CExtension *> *retList, CString *retsError);
		ALERROR ComputeAvailableExtensions (CExtension *pAdventure, DWORD dwFlags, const TArray<DWORD> &Extensions, TArray<CExtension *> *retList, CString *retsError);
		ALERROR ComputeBindOrder (CExtension *pAdventure, const TArray<CExtension *> &DesiredExtensions, DWORD dwFlags, TArray<CExtension *> *retList, CString *retsError);
		void ComputeCoreLibraries (CExtension *pExtension, TArray<CExtension *> *retList);
		void DebugDump (void);
		bool FindAdventureFromDesc (DWORD dwUNID, DWORD dwFlags = 0, CExtension **retpExtension = NULL);
		bool FindBestExtension (DWORD dwUNID, DWORD dwRelease = 0, DWORD dwFlags = 0, CExtension **retpExtension = NULL);
		bool FindExtension (DWORD dwUNID, DWORD dwRelease, CExtension::EFolderTypes iFolder, CExtension **retpExtension = NULL);
		void FreeDeleted (void);
		CExtension *GetBase (void) const { return m_pBase; }
		CString GetExternalResourceFilespec (CExtension *pExtension, const CString &sFilename) const;
		EGameTypes GetGame (void) const { return m_iGame; }
		bool GetRequiredResources (TArray<CString> *retFilespecs);
		void InitEntityResolver (CExtension *pExtension, DWORD dwFlags, CEntityResolverList *retResolver);
		bool IsRegisteredGame (CExtension *pAdventure, const TArray<CExtension *> &DesiredExtensions, DWORD dwFlags);
		bool IsXMLKept (void) const { return m_bKeepXML; }
		ALERROR Load (const CString &sFilespec, DWORD dwFlags, CString *retsError);
		inline bool LoadedInDebugMode (void) { return m_bLoadedInDebugMode; }
		ALERROR LoadNewExtension (const CString &sFilespec, const CIntegerIP &FileDigest, CString *retsError);
		inline void SetCollectionFolder (const CString &sFilespec) { m_sCollectionFolder = sFilespec; }
		void SetRegisteredExtensions (const CMultiverseCollection &Collection, TArray<CMultiverseCatalogEntry *> *retNotFound);
		void SweepImages (void);
		void UpdateCollectionStatus (CMultiverseCollection &Collection, int cxIconSize, int cyIconSize);

		static int Compare (CExtension *pExt1, CExtension *pExt2, bool bDebugMode);

	private:
		ALERROR AddCompatibilityLibrary (CExtension *pAdventure, const TArray<CExtension *> &Extensions, DWORD dwFlags, const TArray<CExtension *> &Compatibility, TArray<CExtension *> *retList, CString *retsError);
		void AddOrReplace (CExtension *pExtension);
		ALERROR AddToBindList (CExtension *pExtension, DWORD dwFlags, const TArray<CExtension *> &Compatibility, TArray<CExtension *> *retList, CString *retsError);
		void ClearAllMarks (void);
		void ComputeCompatibilityLibraries (CExtension *pAdventure, DWORD dwFlags, TArray<CExtension *> *retList);
		ALERROR ComputeFilesToLoad (const CString &sFilespec, CExtension::EFolderTypes iFolder, TSortMap<CString, int> &List, CString *retsError);
		ALERROR LoadBaseFile (const CString &sFilespec, DWORD dwFlags, CString *retsError);
		ALERROR LoadEmbeddedExtension (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CExtension **retpExtension);
		ALERROR LoadFile (const CString &sFilespec, CExtension::EFolderTypes iFolder, DWORD dwFlags, const CIntegerIP &CheckDigest, bool *retbReload, CString *retsError);
		ALERROR LoadFolderStubsOnly (const CString &sFilespec, CExtension::EFolderTypes iFolder, DWORD dwFlags, CString *retsError);
		bool ReloadDisabledExtensions (DWORD dwFlags);

		EGameTypes m_iGame;					//	Game
		CString m_sCollectionFolder;		//	Path to collection folder
		TArray<CString> m_ExtensionFolders;	//	Paths to extension folders

		CCriticalSection m_cs;				//	Protects modifications
		TArray<CExtension *> m_Extensions;	//	All loaded extensions
		bool m_bReloadNeeded;				//	If TRUE we need to reload our folders
		bool m_bLoadedInDebugMode;			//	If TRUE we loaded in debug mode
		bool m_bKeepXML;					//	If TRUE we need to keep XML for all types

		TArray<CExtension *> m_Deleted;		//	Keep around until next bind

		//	Indices for easy access

		CExtension *m_pBase;				//	Base extension
		TSortMap<DWORD, TArray<CExtension *> > m_ByUNID;
		TSortMap<CString, CExtension *> m_ByFilespec;
	};

class CDynamicDesignTable
	{
	public:
		CDynamicDesignTable (void) { }
		~CDynamicDesignTable (void) { CleanUp(); }

		ALERROR DefineType (CExtension *pExtension, DWORD dwUNID, ICCItem *pSource, CDesignType **retpType = NULL, CString *retsError = NULL);
		void Delete (DWORD dwUNID);
		inline void DeleteAll (void) { CleanUp(); }
		inline int GetCount (void) const { return m_Table.GetCount(); }
		inline CDesignType *GetType (int iIndex) const { return m_Table[iIndex].pType; }
		void ReadFromStream (SUniverseLoadCtx &Ctx);
		void WriteToStream (IWriteStream *pStream);

	private:
		struct SEntry
			{
			SEntry (void) : pExtension(NULL),
					dwUNID(0),
					pSource(NULL),
					pType(NULL)
				{ }

			~SEntry (void)
				{
				if (pType)
					delete pType;

				if (pSource)
					delete pSource;
				}

			CExtension *pExtension;
			DWORD dwUNID;
			CString sSource;
			CXMLElement *pSource;
			CDesignType *pType;
			};

		void CleanUp (void);
		ALERROR Compile (SEntry *pEntry, CString *retsError = NULL);
		ALERROR CreateType (SEntry *pEntry, CXMLElement *pDesc, CDesignType **retpType, CString *retsError = NULL);
		inline SEntry *GetEntry (int iIndex) const { return &m_Table[iIndex]; }

		TSortMap<DWORD, SEntry> m_Table;
	};

struct SDesignLoadCtx
	{
	SDesignLoadCtx (void) :
			pResDb(NULL),
			pExtension(NULL),
			pType(NULL),
			bBindAsNewGame(false),
			bNoResources(false),
			bNoVersionCheck(false),
			bKeepXML(false),
            bLoopImages(false),
			bLoadAdventureDesc(false),
			bLoadModule(false),
			dwInheritAPIVersion(0)
		{ }

	inline DWORD GetAPIVersion (void) const { return (pExtension ? pExtension->GetAPIVersion() : API_VERSION); }

	//	Context
	CString sResDb;							//	ResourceDb filespec
	CResourceDb *pResDb;					//	Open ResourceDb object
	CString sFolder;						//	Folder context (used when loading images)
	CExtension *pExtension;					//	Extension
	CDesignType *pType;						//	Current type being loaded
	bool bLoadAdventureDesc;				//	If TRUE, we are loading an adventure desc only
	bool bLoadModule;						//	If TRUE, we are loading elements in a module
	DWORD dwInheritAPIVersion;				//	APIVersion of parent (if base file)

	//	Options
	bool bBindAsNewGame;					//	If TRUE, then we are binding a new game
	bool bNoResources;
	bool bNoVersionCheck;
	bool bKeepXML;							//	Keep XML around
    bool bLoopImages;                       //  If TRUE, image effects loop by default

	//	Output
	CString sError;
	CString sErrorFilespec;					//	File in which error occurred.
	};

class CDesignCollection
	{
	public:
		enum ECachedHandlers
			{
			evtGetGlobalAchievements	= 0,
			evtGetGlobalDockScreen		= 1,
			evtGetGlobalPlayerPriceAdj	= 2,
			evtOnGlobalDockPaneInit		= 3,
			evtOnGlobalMarkImages		= 4,
			evtOnGlobalObjDestroyed		= 5,
			evtOnGlobalPlayerBoughtItem	= 6,
			evtOnGlobalPlayerSoldItem	= 7,
			evtOnGlobalSystemStarted	= 8,
			evtOnGlobalSystemStopped	= 9,
			evtOnGlobalUniverseCreated	= 10,
			evtOnGlobalUniverseLoad		= 11,
			evtOnGlobalUniverseSave		= 12,
			evtOnGlobalUpdate			= 13,

			evtCount					= 14
			};

		enum EFlags
			{
			//	GetImage flags
			FLAG_IMAGE_COPY =			0x00000001,
			FLAG_IMAGE_LOCK =			0x00000002,
			};

		CDesignCollection (void);
		~CDesignCollection (void);

		ALERROR AddDynamicType (CExtension *pExtension, DWORD dwUNID, ICCItem *pSource, bool bNewGame, CString *retsError);
		ALERROR BindDesign (const TArray<CExtension *> &BindOrder, bool bNewGame, bool bNoResources, CString *retsError);
		void CleanUp (void);
		void ClearImageMarks (void);
		inline CEconomyType *FindEconomyType (const CString &sID) { CEconomyType **ppType = m_EconomyIndex.GetAt(sID); return (ppType ? *ppType : NULL); }
		inline CDesignType *FindEntry (DWORD dwUNID) const { return m_AllTypes.FindByUNID(dwUNID); }
		CExtension *FindExtension (DWORD dwUNID) const;
		CXMLElement *FindSystemFragment (const CString &sName, CSystemTable **retpTable = NULL) const;
		void FireGetGlobalAchievements (CGameStats &Stats);
		bool FireGetGlobalDockScreen (CSpaceObject *pObj, CString *retsScreen, ICCItem **retpData, int *retiPriority = NULL);
		bool FireGetGlobalPlayerPriceAdj (STradeServiceCtx &ServiceCtx, ICCItem *pData, int *retiPriceAdj);
		void FireOnGlobalMarkImages (void);
		void FireOnGlobalObjDestroyed (SDestroyCtx &Ctx);
		void FireOnGlobalPaneInit (void *pScreen, CDesignType *pRoot, const CString &sScreen, const CString &sPane);
		void FireOnGlobalPlayerBoughtItem (CSpaceObject *pSellerObj, const CItem &Item, const CCurrencyAndValue &Price);
		void FireOnGlobalPlayerChangedShips (CSpaceObject *pOldShip);
		void FireOnGlobalPlayerEnteredSystem (void);
		void FireOnGlobalPlayerLeftSystem (void);
		void FireOnGlobalPlayerSoldItem (CSpaceObject *pBuyerObj, const CItem &Item, const CCurrencyAndValue &Price);
		void FireOnGlobalSystemCreated (SSystemCreateCtx &SysCreateCtx);
		void FireOnGlobalSystemStarted (DWORD dwElapsedTime);
		void FireOnGlobalSystemStopped (void);
		ALERROR FireOnGlobalTypesInit (SDesignLoadCtx &Ctx);
		void FireOnGlobalUniverseCreated (void);
		void FireOnGlobalUniverseLoad (void);
		void FireOnGlobalUniverseSave (void);
		void FireOnGlobalUpdate (int iTick);
		inline int GetCount (void) const { return m_AllTypes.GetCount(); }
		inline int GetCount (DesignTypes iType) const { return m_ByType[iType].GetCount(); }
		inline const CDisplayAttributeDefinitions &GetDisplayAttributes (void) const { return m_DisplayAttribs; }
		DWORD GetDynamicUNID (const CString &sName);
		void GetEnabledExtensions (TArray<CExtension *> *retExtensionList);
		inline CDesignType *GetEntry (int iIndex) const { return m_AllTypes.GetEntry(iIndex); }
		inline CDesignType *GetEntry (DesignTypes iType, int iIndex) const { return m_ByType[iType].GetEntry(iIndex); }
		inline CExtension *GetExtension (int iIndex) { return m_BoundExtensions[iIndex]; }
		inline int GetExtensionCount (void) { return m_BoundExtensions.GetCount(); }
		CG32bitImage *GetImage (DWORD dwUNID, DWORD dwFlags = 0);
		CString GetStartingNodeID (void);
		CTopologyDescTable *GetTopologyDesc (void) const { return m_pTopology; }
		inline bool HasDynamicTypes (void) { return (m_DynamicTypes.GetCount() > 0); }
		bool IsAdventureExtensionBound (DWORD dwUNID);
		bool IsRegisteredGame (void);
		void MarkGlobalImages (void);
		void NotifyTopologyInit (void);
		void ReadDynamicTypes (SUniverseLoadCtx &Ctx);
		void Reinit (void);
		void SweepImages (void);
		void WriteDynamicTypes (IWriteStream *pStream);

	private:
		void CacheGlobalEvents (CDesignType *pType);
		ALERROR CreateTemplateTypes (SDesignLoadCtx &Ctx);
		ALERROR ResolveOverrides (SDesignLoadCtx &Ctx);

		//	Loaded types. These are initialized at load-time and never change.

		CDesignTable m_Base;
		CTopologyDescTable m_BaseTopology;
		CExternalEntityTable m_BaseEntities;

		//	Cached data initialized at bind-time

		TArray<CExtension *> m_BoundExtensions;
		CDesignTable m_AllTypes;
		CDesignList m_ByType[designCount];
		CDesignList m_OverrideTypes;
		CDesignList m_CreatedTypes;
		CTopologyDescTable *m_pTopology;
		CExtension *m_pAdventureExtension;
		CAdventureDesc *m_pAdventureDesc;
		TSortMap<CString, CEconomyType *> m_EconomyIndex;
		CDisplayAttributeDefinitions m_DisplayAttribs;
		CGlobalEventCache *m_EventsCache[evtCount];

		//	Dynamic design types

		CDynamicDesignTable m_DynamicTypes;
		TSortMap<CString, CDesignType *> m_DynamicUNIDs;

		//	State

		bool m_bInBindDesign;
	};

//	Utility functions

DWORD ExtensionVersionToInteger (DWORD dwVersion);
CString GenerateLevelFrequency (const CString &sLevelFrequency, int iCenterLevel);
CString GenerateRandomName (const CString &sList, const CString &sSubst);
CString GenerateRandomNameFromTemplate (const CString &sName, const CString &sSubst = NULL_STR);
CString GetRGBColor (CG32bitPixel rgbColor);
CString GetDamageName (DamageTypes iType);
CString GetDamageShortName (DamageTypes iType);
CString GetDamageType (DamageTypes iType);
int GetDiceCountFromAttribute(const CString &sValue);
int GetFrequency (const CString &sValue);
CString GetFrequencyName (FrequencyTypes iFrequency);
int GetFrequencyByLevel (const CString &sLevelFrequency, int iLevel);
CString GetItemCategoryID (ItemCategories iCategory);
CString GetItemCategoryName (ItemCategories iCategory);
bool IsConstantName (const CString &sList);
bool IsEnergyDamage (DamageTypes iType);
bool IsMatterDamage (DamageTypes iType);
ALERROR LoadDamageAdj (CXMLElement *pItem, const CString &sAttrib, int *retiAdj, int *retiCount = NULL);
DamageTypes LoadDamageTypeFromXML (const CString &sAttrib);
DWORD LoadExtensionVersion (const CString &sVersion);
DWORD LoadNameFlags (CXMLElement *pDesc);
CG32bitPixel LoadRGBColor (const CString &sString, CG32bitPixel rgbDefault = CG32bitPixel::Null());
ALERROR LoadUNID (SDesignLoadCtx &Ctx, const CString &sString, DWORD *retdwUNID);
bool SetFrequencyByLevel (CString &sLevelFrequency, int iLevel, int iFreq);

//	Inline implementations

inline EDamageResults CInstalledArmor::AbsorbDamage (CSpaceObject *pSource, SDamageCtx &Ctx) { return m_pArmorClass->AbsorbDamage(CItemCtx(pSource, this), Ctx); }
inline int CInstalledArmor::GetDamageEffectiveness (CSpaceObject *pAttacker, CInstalledDevice *pWeapon) { return m_pArmorClass->GetDamageEffectiveness(pAttacker, pWeapon); }
inline int CInstalledArmor::GetLevel (void) const { return (m_pItem ? m_pItem->GetLevel() : GetClass()->GetItemType()->GetLevel()); }
inline int CInstalledArmor::GetMaxHP (CSpaceObject *pSource) { return m_pArmorClass->GetMaxHP(CItemCtx(pSource, this)); }

inline bool CInstalledDevice::IsSecondaryWeapon (void) const 
	{
	DWORD dwLinkedFire;
	return (m_fSecondaryWeapon 
			|| (dwLinkedFire = m_pClass->GetLinkedFireOptions(CItemCtx(NULL, (CInstalledDevice *)this))) == CDeviceClass::lkfEnemyInRange
			|| dwLinkedFire == CDeviceClass::lkfTargetInRange);
	}

inline bool CItem::IsArmor (void) const { return (m_pItemType && m_pItemType->GetArmorClass()); }
inline bool CItem::IsDevice (void) const { return (m_pItemType && m_pItemType->GetDeviceClass()); }
inline CEconomyType *CItem::GetCurrencyType (void) const { return m_pItemType->GetCurrencyType(); }
inline int CItem::GetLevel (void) const { return m_pItemType->GetLevel(CItemCtx(*this)); }

inline CDeviceClass *CDeviceDescList::GetDeviceClass (int iIndex) const { return m_List[iIndex].Item.GetType()->GetDeviceClass(); }

inline int CArmorClass::GetDamageAdj (CItemCtx &Ctx, DamageTypes iDamage) const { const SScalableStats &Stats = GetScaledStats(Ctx); return Stats.DamageAdj.GetAdj(iDamage); }
inline int CArmorClass::GetInstallCost (CItemCtx &Ctx) const { const SScalableStats &Stats = GetScaledStats(Ctx); return (int)m_pItemType->GetCurrencyType()->Exchange(Stats.InstallCost); }
inline CString CArmorClass::GetName (void) { return m_pItemType->GetNounPhrase(); }
inline int CArmorClass::GetRepairCost (CItemCtx &Ctx) const { const SScalableStats &Stats = GetScaledStats(Ctx); return (int)m_pItemType->GetCurrencyType()->Exchange(Stats.RepairCost); }
inline DWORD CArmorClass::GetUNID (void) { return m_pItemType->GetUNID(); }
inline bool CArmorClass::IsBlindingDamageImmune (CItemCtx &ItemCtx) { const SScalableStats &Stats = GetScaledStats(ItemCtx); return (Stats.iBlindingDamageAdj == 0 || ItemCtx.GetMods().IsBlindingImmune()); }
inline bool CArmorClass::IsDeviceDamageImmune (CItemCtx &ItemCtx) { const SScalableStats &Stats = GetScaledStats(ItemCtx); return (Stats.iDeviceDamageAdj == 0 || ItemCtx.GetMods().IsDeviceDamageImmune()); }
inline bool CArmorClass::IsDisintegrationImmune (CItemCtx &ItemCtx) { return (m_fDisintegrationImmune || ItemCtx.GetMods().IsDisintegrationImmune()); }
inline bool CArmorClass::IsEMPDamageImmune (CItemCtx &ItemCtx) { const SScalableStats &Stats = GetScaledStats(ItemCtx); return (Stats.iEMPDamageAdj == 0 || ItemCtx.GetMods().IsEMPImmune()); }
inline bool CArmorClass::IsRadiationImmune (CItemCtx &ItemCtx) { const SScalableStats &Stats = GetScaledStats(ItemCtx); return (Stats.fRadiationImmune || ItemCtx.GetMods().IsRadiationImmune()); }
inline bool CArmorClass::IsShatterImmune (CItemCtx &ItemCtx) { return (m_fShatterImmune || (ItemCtx.GetMods().IsShatterImmune())); }
inline bool CArmorClass::IsShieldInterfering (CItemCtx &ItemCtx) { return (m_fShieldInterference || ItemCtx.GetMods().IsShieldInterfering()); }

inline int CDeviceClass::GetLevel (void) const { return m_pItemType->GetLevel(); }
inline CString CDeviceClass::GetName (void) { return m_pItemType->GetNounPhrase(); }
inline DWORD CDeviceClass::GetUNID (void) { return m_pItemType->GetUNID(); }

inline CDesignType *CItemType::GetUseScreen (CString *retsName) const { return m_pUseScreen.GetDockScreen((CDesignType *)this, retsName); }

inline bool DamageDesc::IsEnergyDamage (void) const { return ::IsEnergyDamage(m_iType); }
inline bool DamageDesc::IsMatterDamage (void) const { return ::IsMatterDamage(m_iType); }

inline void IEffectPainter::PlaySound (CSpaceObject *pSource) { if (!m_bNoSound) GetCreator()->PlaySound(pSource); }

inline CSystemMap *CTopologyNode::GetDisplayPos (int *retxPos, int *retyPos) { if (retxPos) *retxPos = m_xPos; if (retyPos) *retyPos = m_yPos; return (m_pMap ? m_pMap->GetDisplayMap() : NULL); }
