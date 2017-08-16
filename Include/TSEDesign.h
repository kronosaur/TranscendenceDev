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
struct STradeServiceCtx;

//	Constants & Enums

const int MAX_OBJECT_LEVEL =			25;	//	Max level for space objects
const int MAX_ITEM_LEVEL =				25;	//	Max level for items

//	Item Criteria --------------------------------------------------------------

#include "TSEItemDefs.h"

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
	designUnused =						19,

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
	//	w
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
		inline void UnbindDesign (void) { m_pInheritFrom = NULL; OnUnbindDesign(); }
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
		void FireOnGlobalEndDiagnostics (const SEventHandlerDesc &Event);
		void FireOnGlobalMarkImages (const SEventHandlerDesc &Event);
		void FireOnGlobalObjDestroyed (const SEventHandlerDesc &Event, SDestroyCtx &Ctx);
		void FireOnGlobalPlayerBoughtItem (const SEventHandlerDesc &Event, CSpaceObject *pSellerObj, const CItem &Item, const CCurrencyAndValue &Price);
		ALERROR FireOnGlobalPlayerChangedShips (CSpaceObject *pOldShip, CString *retsError = NULL);
		ALERROR FireOnGlobalPlayerEnteredSystem (CString *retsError = NULL);
		ALERROR FireOnGlobalPlayerLeftSystem (CString *retsError = NULL);
		void FireOnGlobalPlayerSoldItem (const SEventHandlerDesc &Event, CSpaceObject *pBuyerObj, const CItem &Item, const CCurrencyAndValue &Price);
		ALERROR FireOnGlobalResurrect (CString *retsError = NULL);
		void FireOnGlobalStartDiagnostics (const SEventHandlerDesc &Event);
		void FireOnGlobalSystemDiagnostics (const SEventHandlerDesc &Event);
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
		inline DWORD GetInheritFromUNID (void) const { return m_dwInheritFrom; }
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
		TSortMap<DWORD, DWORD> GetXMLMergeFlags (void) const;
		bool HasAttribute (const CString &sAttrib) const;
		inline bool HasEvents (void) const { return !m_Events.IsEmpty() || (m_pInheritFrom && m_pInheritFrom->HasEvents()); }
		inline bool HasLiteralAttribute (const CString &sAttrib) const { return ::HasModifier(m_sAttributes, sAttrib); }
		bool HasSpecialAttribute (const CString &sAttrib) const;
        inline void IncGlobalData (const CString &sAttrib, ICCItem *pValue = NULL, ICCItem **retpNewValue = NULL) { m_GlobalData.IncData(sAttrib, pValue, retpNewValue); }
		bool InheritsFrom (DWORD dwUNID) const;
		void InitCachedEvents (int iCount, char **pszEvents, SEventHandlerDesc *retEvents);
		inline bool IsClone (void) const { return m_bIsClone; }
		inline bool IsMerged (void) const { return m_bIsMerged; }
		inline bool IsModification (void) const { return m_bIsModification; }
		inline bool IsObsoleteAt (DWORD dwAPIVersion) const { return (m_dwObsoleteVersion > 0 && dwAPIVersion >= m_dwObsoleteVersion); }
		inline void MarkImages (void) { OnMarkImages(); }
		inline void SetGlobalData (const CString &sAttrib, const CString &sData) { m_GlobalData.SetData(sAttrib, sData); }
		inline void SetInheritFrom (CDesignType *pType) { m_pInheritFrom = pType; }
		inline void SetMerged (bool bValue = true) { m_bIsMerged = true; }
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
		virtual void OnAccumulateXMLMergeFlags (TSortMap<DWORD, DWORD> &MergeFlags) const { }
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
		DWORD m_dwVersion;						//	Extension API version
		DWORD m_dwObsoleteVersion;				//	API version on which this type is obsolete (0 = not obsolete)
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
		bool m_bIsMerged;						//	TRUE if we created this type by merging (inheritance)

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

//  Subsidiary Classes and Structures ------------------------------------------

#include "TSEPaint.h"
#include "TSECurrency.h"
#include "TSEImages.h"
#include "TSESounds.h"
#include "TSEComms.h"

//	Items ----------------------------------------------------------------------

#include "TSEWeaponFireDesc.h"
#include "TSEItems.h"

//	Effect Support Structures --------------------------------------------------

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

		virtual void OnAccumulateXMLMergeFlags (TSortMap<DWORD, DWORD> &MergeFlags) const override;
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual void OnUnbindDesign (void) override;

	private:
		CCommunicationsStack m_Comms;
	};

//	Topology Descriptors -------------------------------------------------------
//
//	Defines CTopologyNode::SCriteria, which is needed by station encounter
//	definitions.

#include "TSETopology.h"

//	Ship Systems ---------------------------------------------------------------

#include "TSEArmor.h"
#include "TSEDevices.h"
#include "TSEShipSystems.h"

//	CItemType ------------------------------------------------------------------

#include "TSEItemType.h"

//	Ship Definitions

#include "TSEShipAI.h"
#include "TSEPlayerSettings.h"
#include "TSEShipCreator.h"
#include "TSEDocking.h"
#include "TSEOverlayType.h"

//	CShipClass -----------------------------------------------------------------

#include "TSEShipClass.h"

//	Station Definitions --------------------------------------------------------

#include "TSEDockScreenType.h"
#include "TSETrade.h"
#include "TSEStationType.h"

//	Other Types ----------------------------------------------------------------

#include "TSESovereign.h"
#include "TSEEconomyType.h"
#include "TSEPower.h"
#include "TSESpaceEnvironment.h"
#include "TSEMissionType.h"

//	Adventures -----------------------------------------------------------------

#include "TSEGameRecords.h"
#include "TSEAdventureDesc.h"

//	Systems and System Maps ----------------------------------------------------

#include "TSESystemType.h"
#include "TSESystemMap.h"

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
		ALERROR Merge (const CDesignTable &Source, CDesignList *ioOverride, const TArray<DWORD> *pExtensionsIncluded, const TSortMap<DWORD, bool> *pTypesUsed, DWORD dwAPIVersion);
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
					bNoResources(false),
					bNoDigestCheck(false)
				{ }

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
		DWORDLONG GetXMLMemoryUsage (void) const;
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
		ALERROR DefineType (CExtension *pExtension, DWORD dwUNID, CXMLElement *pSource, CDesignType **retpType = NULL, CString *retsError = NULL);
		void Delete (DWORD dwUNID);
		inline void DeleteAll (void) { CleanUp(); }
		CDesignType *FindType (DWORD dwUNID);
		inline int GetCount (void) const { return m_Table.GetCount(); }
		inline CDesignType *GetType (int iIndex) const { return m_Table[iIndex].pType; }
		int GetXMLMemoryUsage (void) const;
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
			evtGetGlobalAchievements		= 0,
			evtGetGlobalDockScreen			= 1,
			evtGetGlobalPlayerPriceAdj		= 2,
			evtOnGlobalDockPaneInit			= 3,
			evtOnGlobalEndDiagnostics		= 4,

			evtOnGlobalMarkImages			= 5,
			evtOnGlobalObjDestroyed			= 6,
			evtOnGlobalPlayerBoughtItem		= 7,
			evtOnGlobalPlayerSoldItem		= 8,
			evtOnGlobalStartDiagnostics		= 9,

			evtOnGlobalSystemDiagnostics	= 10,
			evtOnGlobalSystemStarted		= 11,
			evtOnGlobalSystemStopped		= 12,
			evtOnGlobalUniverseCreated		= 13,
			evtOnGlobalUniverseLoad			= 14,

			evtOnGlobalUniverseSave			= 15,
			evtOnGlobalUpdate				= 16,

			evtCount						= 17
			};

		enum EFlags
			{
			//	GetImage flags
			FLAG_IMAGE_COPY =			0x00000001,
			FLAG_IMAGE_LOCK =			0x00000002,
			};

		struct SStats
			{
			TArray<CExtension *> Extensions;

			int iAllTypes;					//	Number of bound types (including dynamic)
			int iDynamicTypes;				//	Count of dynamtic types
			int iMergedTypes;				//	Count of merged types
			int iItemTypes;					//	Count of item types
			int iShipClasses;				//	Count of ship classes
			int iStationTypes;				//	Count of station types
			int iResourceTypes;				//	Count of images, sounds, music
			int iDockScreens;				//	Count of dock screen types
			int iMissionTypes;				//	Count of mission types
			int iSovereigns;				//	Count of sovereigns
			int iOverlayTypes;				//	Count of overlays
			int iSystemTypes;				//	Count of system types
			int iEffectTypes;				//	Count of effects
			int iSupportTypes;				//	Count of tables, generic types, etc.

			DWORDLONG dwTotalXMLMemory;		//	Total memory used for XML structures (excluding dynamic)
			};

		CDesignCollection (void);
		~CDesignCollection (void);

		ALERROR AddDynamicType (CExtension *pExtension, DWORD dwUNID, ICCItem *pSource, bool bNewGame, CString *retsError);
		ALERROR BindDesign (const TArray<CExtension *> &BindOrder, const TSortMap<DWORD, bool> &TypesUsed, DWORD dwAPIVersion, bool bNewGame, bool bNoResources, bool bLoadObsoleteTypes, CString *retsError);
		void CleanUp (void);
		void ClearImageMarks (void);
		inline CEconomyType *FindEconomyType (const CString &sID) { CEconomyType **ppType = m_EconomyIndex.GetAt(sID); return (ppType ? *ppType : NULL); }
		inline CDesignType *FindEntry (DWORD dwUNID) const { return m_AllTypes.FindByUNID(dwUNID); }
		CExtension *FindExtension (DWORD dwUNID) const;
		CXMLElement *FindSystemFragment (const CString &sName, CSystemTable **retpTable = NULL) const;
		void FireGetGlobalAchievements (CGameStats &Stats);
		bool FireGetGlobalDockScreen (CSpaceObject *pObj, CString *retsScreen, ICCItem **retpData, int *retiPriority = NULL);
		bool FireGetGlobalPlayerPriceAdj (STradeServiceCtx &ServiceCtx, ICCItem *pData, int *retiPriceAdj);
		void FireOnGlobalEndDiagnostics (void);
		void FireOnGlobalMarkImages (void);
		void FireOnGlobalObjDestroyed (SDestroyCtx &Ctx);
		void FireOnGlobalPaneInit (void *pScreen, CDesignType *pRoot, const CString &sScreen, const CString &sPane);
		void FireOnGlobalPlayerBoughtItem (CSpaceObject *pSellerObj, const CItem &Item, const CCurrencyAndValue &Price);
		void FireOnGlobalPlayerChangedShips (CSpaceObject *pOldShip);
		void FireOnGlobalPlayerEnteredSystem (void);
		void FireOnGlobalPlayerLeftSystem (void);
		void FireOnGlobalPlayerSoldItem (CSpaceObject *pBuyerObj, const CItem &Item, const CCurrencyAndValue &Price);
		void FireOnGlobalStartDiagnostics (void);
		void FireOnGlobalSystemCreated (SSystemCreateCtx &SysCreateCtx);
		void FireOnGlobalSystemDiagnostics (void);
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
		void GetStats (SStats &Result) const;
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
		ALERROR ResolveInheritingType (SDesignLoadCtx &Ctx, CDesignType *pType);
		ALERROR ResolveOverrides (SDesignLoadCtx &Ctx);
		ALERROR ResolveTypeHierarchy (SDesignLoadCtx &Ctx);

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
		CDynamicDesignTable m_HierarchyTypes;
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
inline int CDeviceClass::GetLevel (void) const { return m_pItemType->GetLevel(); }
inline CString CDeviceClass::GetName (void) { return m_pItemType->GetNounPhrase(); }
inline DWORD CDeviceClass::GetUNID (void) { return m_pItemType->GetUNID(); }

inline CDesignType *CItemType::GetUseScreen (CString *retsName) const { return m_pUseScreen.GetDockScreen((CDesignType *)this, retsName); }

inline bool DamageDesc::IsEnergyDamage (void) const { return ::IsEnergyDamage(m_iType); }
inline bool DamageDesc::IsMatterDamage (void) const { return ::IsMatterDamage(m_iType); }

inline void IEffectPainter::PlaySound (CSpaceObject *pSource) { if (!m_bNoSound) GetCreator()->PlaySound(pSource); }

inline CSystemMap *CTopologyNode::GetDisplayPos (int *retxPos, int *retyPos) { if (retxPos) *retxPos = m_xPos; if (retyPos) *retyPos = m_yPos; return (m_pMap ? m_pMap->GetDisplayMap() : NULL); }
