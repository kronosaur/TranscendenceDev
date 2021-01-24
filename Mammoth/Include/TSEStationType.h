//	TSEStationType.h
//
//	Defines classes and interfaces for station types.
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

//	Armor/Hull System ----------------------------------------------------------

class CStationHullDesc
	{
	public:
		enum EHullTypes
			{
			hullUnknown				= -1,

			hullSingle				= 0,	//	Single hull, normal damage
			hullAsteroid			= 1,	//	Must have either WMD or mining to damage
			hullMultiple			= 2,	//	Must have WMD to damage
			hullUnderground			= 3,	//	Must have mining to damage
			};

		void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) const;
		ALERROR Bind (SDesignLoadCtx &Ctx);
		int CalcDamageEffectiveness (CSpaceObject *pAttacker, CInstalledDevice *pWeapon) const;
		Metric CalcHitsToDestroy (int iLevel) const;
		bool CanBeHit (void) const { return !m_bCannotBeHit; }
		bool CanBeWrecked (void) const { return (!IsImmutable() && GetMaxHitPoints() > 0); }
		bool FindDataField (const CString &sField, CString *retsValue) const;
		CArmorClass *GetArmorClass (void) const { return (m_pArmor ? m_pArmor->GetArmorClass() : NULL); }
		CItem GetArmorItem (void) const;
		int GetArmorLevel (void) const;
		int GetHitPoints (void) const { return m_iHitPoints; }
		EHullTypes GetHullType (void) const { return m_iType; }
		int GetMaxHitPoints (void) const { return m_iMaxHitPoints; }
		int GetMaxStructuralHP (void) const { return m_iMaxStructuralHP; }
		const CRegenDesc &GetRegenDesc (void) const { return m_Regen; }
		int GetStructuralHP (void) const { return m_iStructuralHP; }
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, bool bMultiHullDefault = true);
		ALERROR InitFromStationXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		bool IsImmutable (void) const { return m_bImmutable; }
		void SetImmutable (bool bValue = true) { m_bImmutable = bValue; }

		static CString GetID (EHullTypes iType);
		static EHullTypes ParseHullType (const CString &sValue);
		
	private:
		EHullTypes m_iType;					//	Type of hull
		CItemTypeRef m_pArmor;				//	Armor class
		int m_iArmorLevel = 0;				//	Armor level (for scalable items)
		int m_iHitPoints = 0;				//	Hit points at creation time
		int m_iMaxHitPoints = 0;			//	Max hit points
		CRegenDesc m_Regen;					//	Repair rate

		int m_iStructuralHP = 0;			//	Initial structural hit points
		int m_iMaxStructuralHP = 0;			//	Max structural hp (0 = station is permanent)
		
		bool m_bImmutable = false;			//	Cannot be damaged
		bool m_bCannotBeHit = false;		//	Transparent to projectiles
	};

class CStationHull
	{
	public:
		CStationHull (void);

		bool CanBeDestroyed (void) const { return (m_iStructuralHP > 0); }
		bool CanBeHit (void) const { return !m_fCannotBeHit; }
		ICCItem *FindProperty (const CString &sProperty) const;
		int GetArmorLevel (void) const { return m_iArmorLevel; }
		int GetHitPoints (void) const { return m_iHitPoints; }
		CStationHullDesc::EHullTypes GetHullType (void) const { return m_iType; }
		int GetMaxHitPoints (void) const { return m_iMaxHitPoints; }
		int GetMaxStructuralHP (void) const { return m_iMaxStructuralHP; }
		EDamageResults GetPassthroughDefault (void) const;
		int GetStructuralHP (void) const { return m_iStructuralHP; }
		int GetVisibleDamage (void) const;
		void GetVisibleDamageDesc (SVisibleDamage &Damage) const;
		int IncHitPoints (int iInc) { m_iHitPoints = Max(0, m_iHitPoints + iInc); return m_iHitPoints; }
		int IncStructuralHP (int iInc) { m_iStructuralHP = Max(0, m_iStructuralHP + iInc); return m_iStructuralHP; }
		void Init (const CStationHullDesc &Desc);
		bool IsAbandoned (void) const { return (m_iHitPoints == 0 && !m_fImmutable); }
		bool IsImmuneTo (SpecialDamageTypes iSpecialDamage) const { return false; }
		bool IsImmutable (void) const { return (m_fImmutable ? true : false); }
		bool IsWrecked (void) const { return (IsAbandoned() && m_iMaxHitPoints > 0); }
		void ReadFromStream (SLoadCtx &Ctx);
		void SetArmorLevel (int iValue) { m_iArmorLevel = iValue; }
		void SetHitPoints (int iValue) { m_iHitPoints = Max(0, iValue); }
		void SetImmutable (bool bValue = true) { m_fImmutable = bValue; }
		void SetMaxHitPoints (int iValue) { m_iMaxHitPoints = Max(0, iValue); }
		void SetMaxStructuralHP (int iValue) { m_iMaxStructuralHP = Max(0, iValue); }
		void SetHullType (CStationHullDesc::EHullTypes iType) { m_iType = iType; }
		bool SetPropertyIfFound (const CString &sProperty, ICCItem *pValue, CString *retsError);
		void SetStructuralHP (int iValue) { m_iStructuralHP = Max(0, iValue); }
		void UpdateExtended (const CStationHullDesc &Desc, const CTimeSpan &ExtraTime);
		bool UpdateRepairDamage (const CStationHullDesc &Desc, int iTick);
		void WriteToStream (IWriteStream &Stream, CStation *pStation);

	private:
		static constexpr DWORD HULL_TYPES_MASK =	0x000000f0;
		static constexpr DWORD HULL_TYPES_SHIFT =	4;

		CStationHullDesc::EHullTypes m_iType = CStationHullDesc::hullUnknown;

		int m_iArmorLevel = 0;				//	Armor level (for scalable items)
		int m_iHitPoints = 0;				//	Total hit points (0 = station abandoned)
		int m_iMaxHitPoints = 0;			//	Max hit points (0 = station cannot be abandoned)
		int m_iStructuralHP = 0;			//	Structural hp (0 = station cannot be destroyed)
		int m_iMaxStructuralHP = 0;			//	Max structural hp
		
		DWORD m_fImmutable:1;				//	Cannot be damaged
		DWORD m_fCannotBeHit:1;				//	Cannot be hit
		DWORD m_fSpare3:1;
		DWORD m_fSpare4:1;
		DWORD m_fSpare5:1;
		DWORD m_fSpare6:1;
		DWORD m_fSpare7:1;
		DWORD m_fSpare8:1;

		DWORD m_dwSpare:24;
	};

//	Encounter Info -------------------------------------------------------------

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

		int CalcAffinity (const CTopologyNode &Node) const;
		int CalcFrequencyForNode (const CTopologyNode &Node) const;
		int CalcLevelFromFrequency (void) const;
		bool InitAsOverride (const CStationEncounterDesc &Original, const CXMLElement &Override, CString *retsError);
		ALERROR InitFromStationTypeXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		void InitLevelFrequency (CTopology &Topology);
		bool CanBeRandomlyEncountered (void) const { return (!m_sLevelFrequency.IsBlank() || m_bNumberAppearing); }
		int GetCountOfRandomEncounterLevels (void) const;
		const CTopologyAttributeCriteria &GetDistanceCriteria (void) const { return m_DistanceCriteria; }
		void GetExclusionDesc (SExclusionDesc &Exclusion) const;
		Metric GetExclusionRadius (void) const { return m_rExclusionRadius; }
		Metric GetEnemyExclusionRadius (void) const { return m_rEnemyExclusionRadius; }
		int GetFrequencyByDistance (int iDistance) const;
		int GetFrequencyByLevel (int iLevel) const;
		const CString &GetLevelFrequency (void) const { return m_sLevelFrequency; }
		const CAffinityCriteria &GetLocationCriteria (void) const { return m_LocationCriteria; }
		int GetMaxAppearing (void) const { return (m_bMaxCountLimit ? m_MaxAppearing.Roll() : -1); }
		int GetNumberAppearing (void) const { return (m_bNumberAppearing ? m_NumberAppearing.Roll() : -1); }
		bool HasAutoLevelFrequency (void) const { return m_bAutoLevelFrequency; }
		bool HasSystemCriteria (const CTopologyNodeCriteria **retpCriteria = NULL) const 
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
		bool HasSystemLimit (int *retiLimit = NULL) const { if (retiLimit) *retiLimit = m_iMaxCountInSystem; return (m_iMaxCountInSystem != -1); }
		bool IsUniqueInSystem (void) const { return (m_iMaxCountInSystem == 1); }
		void ReadFromStream (SUniverseLoadCtx &Ctx);
		void WriteToStream (IWriteStream *pStream);

	private:
		bool m_bSystemCriteria = false;				//	If TRUE we have system criteria
		CTopologyNodeCriteria m_SystemCriteria;		//	System criteria

		CTopologyAttributeCriteria m_DistanceCriteria;	//	Criteria for nodes for distance calc
		CString m_sDistanceFrequency;				//	Frequency distribution by distance from criteria
		CString m_sLevelFrequency;					//	String array of frequency distribution by level
		CAffinityCriteria m_SystemAffinity;			//	Adjust frequency based on number of matches

		CAffinityCriteria m_LocationCriteria;		//	Affinity for a location in a system.
		Metric m_rExclusionRadius = 0.0;			//	No stations of any kind within this radius
		Metric m_rEnemyExclusionRadius = 0.0;		//	No enemy stations within this radius
		bool m_bAutoLevelFrequency = false;			//	We generated m_sLevelFrequency and need to save it.

		bool m_bNumberAppearing = false;			//	If TRUE, must create this exact number in game
		DiceRange m_NumberAppearing;				//	Create this number in the game

		bool m_bMaxCountLimit = false;				//	If FALSE, no limit
		DiceRange m_MaxAppearing;

		int m_iMaxCountInSystem = -1;				//	-1 means no limit
	};

class CStationEncounterCtx
	{
	public:
		void AddEncounter (void) { m_Total.iCount++; }
		void AddEncounter (CSystem &System);
		bool CanBeEncountered (const CStationEncounterDesc &Desc) const;
		bool CanBeEncounteredInSystem (CSystem &System, const CStationType &StationType, const CStationEncounterDesc &Desc) const;
		TSortMap<CString, int> GetEncounterCountByNode (void) const;
		int GetFrequencyByLevel (int iLevel, const CStationEncounterDesc &Desc) const;
		int GetFrequencyForNode (CTopologyNode &Node, const CStationType &StationType, const CStationEncounterDesc &Desc) const;
		int GetFrequencyForSystem (CSystem &System, const CStationType &StationType, const CStationEncounterDesc &Desc) const;
		int GetMinimumForNode (CTopologyNode &Node, const CStationEncounterDesc &Desc) const;
		int GetRequiredForNode (CTopologyNode &Node, const CStationEncounterDesc &Desc) const;
		int GetTotalCount (void) const { return m_Total.iCount; }
		int GetTotalLimit (void) const { return m_Total.iLimit; }
		int GetTotalMinimum (void) const { return m_Total.iMinimum; }
		void IncMinimumForNode (CTopologyNode &Node, int iInc = 1);
		bool IsEncounteredIn (const CTopologyNode &Node, const CStationEncounterDesc &Desc) const;
		bool IsEncounteredIn (int iLevel, const CStationEncounterDesc &Dest) const;
		void ReadFromStream (SUniverseLoadCtx &Ctx);
		void Reinit (const CStationEncounterDesc &Desc);
		void WriteToStream (IWriteStream *pStream) const;

		static int CalcDistanceToCriteria (const CTopologyNode &Node, const CTopologyAttributeCriteria &Criteria);

	private:
		struct SEncounterStats
			{
			int iCount = 0;						//	Number of times encountered
			int iLimit = -1;					//	Encounter limit (-1 = no limit)
			int iMinimum = 0;					//	Minimum encounters (-1 = no limit)

			mutable int iNodeCriteria = -1;		//  Cached frequency for node (-1 = unknown)
			};

		int GetBaseFrequencyForNode (const CTopologyNode &Node, const CStationEncounterDesc &Desc) const;
		int GetCountInSystem (CSystem &System, const CStationType &StationType) const;

		SEncounterStats m_Total;			//	Encounters in entire game
		TSortMap<int, SEncounterStats> m_ByLevel;	//	Encounters by system level
		TSortMap<CString, SEncounterStats> m_ByNode;	//	Encounters by topology node
	};

class CStationCreateOptions
	{
	public:
		bool ForceMapLabel (void) const { return m_bForceMapLabel; }
		bool ForceMapOrbit (void) const { return m_bForceMapOrbit; }
		const CString &GetImageVariant (void) const { return m_sImageVariant; }
		const CXMLElement *GetItemsXML (void) const { return m_pItems; }
		const CNameDesc &GetNameDesc (void) const { return m_Name; }
		const CString &GetObjName (void) const { return m_sObjName; }
		const CXMLElement *GetOnCreateXML (void) const { return m_pOnCreate; }
		CPaintOrder::Types GetPaintOrder (void) const { return m_iPaintOrder; }
		const CXMLElement *GetSatellitesXML (void) const { return m_pSatellites; }
		const CXMLElement *GetShipsXML (void) const { return m_pShips; }
		const CXMLElement *GetTradeXML (void) const { return m_pTrade; }
		ALERROR InitFromXML (SSystemCreateCtx &Ctx, const CXMLElement &XMLDesc);
		void Merge (const CStationCreateOptions &Src);
		bool MergeFromSimpleXML (const CXMLElement &XMLDesc, CStationCreateOptions *retOld);
		bool SuppressConstruction (void) const { return m_bSuppressConstruction; }
		bool SuppressMapLabel (void) const { return m_bSuppressMapLabel; }
		bool SuppressReinforcements (void) const { return m_bSuppressReinforcements; }
		
	private:
		void CopyOnWrite (bool &bCopied, CStationCreateOptions *retCopy);

		CNameDesc m_Name;
		CString m_sImageVariant;
		CString m_sObjName;
		CPaintOrder::Types m_iPaintOrder = CPaintOrder::none;

		const CXMLElement *m_pOnCreate = NULL;

		const CXMLElement *m_pItems = NULL;
		const CXMLElement *m_pSatellites = NULL;
		const CXMLElement *m_pShips = NULL;
		const CXMLElement *m_pTrade = NULL;

		bool m_bForceMapLabel = false;
		bool m_bForceMapOrbit = false;
		bool m_bSuppressConstruction = false;
		bool m_bSuppressMapLabel = false;
		bool m_bSuppressMapOrbit = false;
		bool m_bSuppressReinforcements = false;
	};

class CStationEncounterOverrideTable
	{
	public:
		const CStationEncounterDesc &GetEncounterDesc (const CStationType &Type) const;
		bool InitFromXML (CDesignCollection &Design, const CXMLElement &OverrideTableXML, CString *retsError);

	private:
		TSortMap<DWORD, CStationEncounterDesc> m_Table;
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

enum class EAsteroidType
	{
	unknown = -1,			//	Unknown composition
	none = 0,				//	No mining

	icy = 1,
	metallic = 2,
	primordial = 3,
	rocky = 4,
	volcanic = 5,
	};

constexpr int EAsteroidTypeCount = 6;

enum class EMiningMethod
	{
	unknown = -1,

	ablation = 0,
	drill = 1,
	explosion = 2,
	shockwave = 3,
	};

constexpr int EMiningMethodCount = 4;

class CAsteroidDesc
	{
	public:

		//	Composition (for mining purposes)

		struct SMiningStats
			{
			int iSuccessChance = 0;
			int iMaxOreLevel = 1;
			Metric rYieldAdj = 1.0;
			};

		EAsteroidType GetType (void) const { return m_iType; }
		CString GetTypeLabel (CUniverse &Universe) const;
		ALERROR InitFromStationTypeXML (SDesignLoadCtx &Ctx, const CXMLElement &Desc);
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, const CXMLElement &Desc);
		bool IsEmpty (void) const { return (m_bDefault || m_iType == EAsteroidType::unknown); }

		static void CalcMining (int iMiningLevel, int iMiningDifficulty, EAsteroidType iType, const SDamageCtx &DamageCtx, SMiningStats &retMining);
		static EMiningMethod CalcMiningMethod (const CWeaponFireDesc &Desc);
		static CString CompositionID (EAsteroidType iType);
		static int GetDefaultMiningDifficulty (EAsteroidType iType);
		static const CAsteroidDesc &Null (void) { return m_Null; }
		static EAsteroidType ParseComposition (const CString &sValue);

	private:
		struct SCompositionDesc
			{
			LPCSTR pszID = NULL;
			int iMiningDifficulty = 0;

			Metric SuccessAdj[EMiningMethodCount] = { 0.0 };
			Metric YieldAdj[EMiningMethodCount] = { 0.0 };
			};

		static Metric CalcBaseMiningSuccess (int iMiningLevel, int iMiningDifficulty);
		static int CalcMaxOreLevel (DamageTypes iDamageType);
		static const SCompositionDesc &GetCompositionDesc (EAsteroidType iType) { return COMPOSITION_TABLE[static_cast<int>(iType)]; }

		EAsteroidType m_iType = EAsteroidType::unknown;
		bool m_bDefault = false;

		static TStaticStringTable<TStaticStringEntry<EAsteroidType>, 5> COMPOSITION_INDEX;
		static SCompositionDesc COMPOSITION_TABLE[EAsteroidTypeCount];

		static const CAsteroidDesc m_Null;
	};

class CStarDesc
	{
	public:
		Metric GetGravityRadius (void) const { return m_rGravityRadius; }
		int GetMaxLightDistance (void) const { return m_iMaxLightDistance; }
		CG32bitPixel GetSpaceColor (void) const { return m_rgbSpaceColor; }
		bool HasGravity (void) const { return (m_rGravityRadius > 0.0); }
		ALERROR InitFromStationTypeXML (SDesignLoadCtx &Ctx, const CXMLElement &Desc);

	private:
		CG32bitPixel m_rgbSpaceColor;					//	Space color
		int m_iMaxLightDistance = 0;					//	Max distance at which there is no (effective) light from star
		Metric m_rGravityRadius = 0.0;					//	Gravity radius
	};

class CStargateDesc
	{
	public:
		void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) const { retTypesUsed->SetAt(m_pGateEffect.GetUNID(), true); }
		ALERROR Bind (SDesignLoadCtx &Ctx) { return m_pGateEffect.Bind(Ctx); }
		const CString &GetDestEntryPoint (void) const { return m_sDestEntryPoint; }
		const CString &GetDestNodeID (void) const { return m_sDestNodeID; }
		CEffectCreator *GetGateEffect (void) const { return m_pGateEffect; }
		ALERROR InitFromStationTypeXML (SDesignLoadCtx &Ctx, const CXMLElement &Desc);
		void MarkImages (void) { if (m_pGateEffect) m_pGateEffect->MarkImages(); }

	private:
		CString m_sDestNodeID;							//	Dest node
		CString m_sDestEntryPoint;						//	Dest entry point
		CEffectCreatorRef m_pGateEffect;				//	Effect when object gates in/out of station
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

		bool AlertWhenAttacked (void) { return (mathRandom(1, 100) <= m_iAlertWhenAttacked); }
		bool AlertWhenDestroyed (void) { return (mathRandom(1, 100) <= m_iAlertWhenDestroyed); }
		bool BuildsReinforcements (void) const { return (m_fBuildReinforcements ? true : false); }
		bool CanAttack () const;
		bool CanAttackIndependently (void) const { return (m_fNoIndependentAttack ? false : true); }
		bool CanBeEncountered (const CStationEncounterDesc &Desc) const { return m_EncounterRecord.CanBeEncountered(Desc); }
		bool CanBeEncountered (CSystem &System, const CStationEncounterDesc &Desc) const { return m_EncounterRecord.CanBeEncounteredInSystem(System, *this, Desc); }
		bool CanBeHitByFriends (void) { return (m_fNoFriendlyTarget ? false : true); }
		bool CanHitFriends (void) const { return (m_fNoFriendlyFire ? false : true); }
		TSharedPtr<CG32bitImage> CreateFullImage (SGetImageCtx &ImageCtx, const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers, RECT &retrcImage, int &retxCenter, int &retyCenter) const;
		bool ForceCanAttack (void) const { return (m_fCanAttack ? true : false); }
		bool ForceCannotAttack (void) const { return (m_fCannotAttack ? true : false); }
		bool ForceMapLabel (void) const { return m_fForceMapLabel; }
		void GenerateDevices (int iLevel, CDeviceDescList &Devices) const;
		CXMLElement *GetAbandonedScreen (void) { return m_pAbandonedDockScreen.GetDesc(); }
		CDesignType *GetAbandonedScreen (CString *retsName) { return m_pAbandonedDockScreen.GetDockScreen(this, retsName); }
		const CAsteroidDesc &GetAsteroidDesc (void) const;
		CurrencyValue GetBalancedTreasure (void) const;
		CEffectCreator *GetBarrierEffect (void) { return m_pBarrierEffect; }
		int GetChallengeRating (void) const;
		IShipGenerator *GetConstructionTable (void) { return m_pConstruction; }
		CSovereign *GetControllingSovereign (void) const;
		DWORD GetDefaultBkgnd (void) { return m_dwDefaultBkgnd; }
		const CShipChallengeDesc &GetDefenderCount (void) const { return m_DefenderCount; }
		CXMLElement *GetDesc (void) { return m_pDesc; }
		CString GetDestNodeID (void) const { return m_Stargate.GetDestNodeID(); }
		CString GetDestEntryPoint (void) const { return m_Stargate.GetDestEntryPoint(); }
		int GetEjectaAdj (void) { return m_iEjectaAdj; }
		CWeaponFireDesc *GetEjectaType (void) { return m_pEjectaType; }
		const CStationEncounterDesc &GetEncounterDesc (void) const;
		const CStationEncounterDesc &GetEncounterDescConst (void) { return GetEncounterDesc(); }
		CWeaponFireDesc *GetExplosionType (void) const { return m_pExplosionType; }
		int GetEncounterFrequency (void) { return m_iEncounterFrequency; }
		int GetEncounterMinimum (CTopologyNode &Node, const CStationEncounterDesc &Desc) const { return m_EncounterRecord.GetMinimumForNode(Node, Desc); }
		CStationEncounterCtx &GetEncounterRecord (void) { return m_EncounterRecord; }
		int GetEncounterRequired (CTopologyNode &Node, const CStationEncounterDesc &Desc) const { return m_EncounterRecord.GetRequiredForNode(Node, Desc); }
		IShipGenerator *GetEncountersTable (void) { return m_pEncounters; }
		int GetFireRateAdj (void) { return m_iFireRateAdj; }
		CXMLElement *GetFirstDockScreen (void) { return m_pFirstDockScreen.GetDesc(); }
		CDesignType *GetFirstDockScreen (CString *retsName) { return m_pFirstDockScreen.GetDockScreen(this, retsName); }
		int GetFrequencyByLevel (int iLevel, const CStationEncounterDesc &Desc) const { return m_EncounterRecord.GetFrequencyByLevel(iLevel, Desc); }
		int GetFrequencyForNode (CTopologyNode &Node, const CStationEncounterDesc &Desc) const { return m_EncounterRecord.GetFrequencyForNode(Node, *this, Desc); }
		int GetFrequencyForSystem (CSystem &System, const CStationEncounterDesc &Desc) const { return m_EncounterRecord.GetFrequencyForSystem(System, *this, Desc); }
		CEffectCreator *GetGateEffect (void) const { return m_Stargate.GetGateEffect(); }
		Metric GetGravityRadius (void) const { return m_Star.GetGravityRadius(); }
		const CObjectImageArray &GetHeroImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers, int *retiRotation = NULL) { return m_HeroImage.GetImage(SGetImageCtx(GetUniverse()), Selector, Modifiers, retiRotation); }
		const CStationHullDesc &GetHullDesc (void) const { return m_HullDesc; }
		const CCompositeImageDesc &GetImage (void) const { return m_Image; }
		const CObjectImageArray &GetImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers, int *retiRotation = NULL) const { return m_Image.GetImage(SGetImageCtx(GetUniverse()), Selector, Modifiers, retiRotation); }
		int GetImageVariants (void) { return m_Image.GetVariantCount(); }
		IShipGenerator *GetInitialShips (void) const { return m_pInitialShips; }
		Metric GetLevelStrength (int iLevel) const;
		Metric GetMass (void) { return m_rMass; }
		int GetMaxLightDistance (void) const { return m_Star.GetMaxLightDistance(); }
		int GetMaxShipConstruction (void) { return m_iMaxConstruction; }
		const CNameDesc &GetNameDesc (void) const { return m_Name; }
		int GetNumberAppearing (void) const { return m_EncounterRecord.GetTotalMinimum(); }
		CPaintOrder::Types GetPaintOrder (void) const { return m_iPaintOrder; }
		Metric GetParallaxDist (void) const { return m_rParallaxDist; }
		CItem GetPrimaryWeapon (void) const;
		IItemGenerator *GetRandomItemTable (void) { return m_pItems; }
		IShipGenerator *GetReinforcementsTable (void);
		const CIntegralRotationDesc &GetRotationDesc (void);
		const CXMLElement *GetSatellitesDesc (void) const { return m_pSatellitesDesc; }
		ScaleTypes GetScale (void) const { return m_iScale; }
		int GetSize (void) const { return m_iSize; }
		int GetShipConstructionRate (void) { return m_iShipConstructionRate; }
		const CRegenDesc &GetShipRegenDesc (void) { return m_ShipRegen; }
		CSovereign *GetSovereign (void) const { return m_pSovereign; }
		CG32bitPixel GetSpaceColor (void) const { return m_Star.GetSpaceColor(); }
		int GetStealth (void) const { return m_iStealthFromArmor; }
		int GetTempChance (void) const { return m_iChance; }
		bool HasAnimations (void) const { return (m_pAnimations != NULL); }
		bool HasGravity (void) const { return m_Star.HasGravity(); }
		bool HasWreckImage (void) const { return m_HullDesc.CanBeWrecked(); }
		void IncEncounterMinimum (CTopologyNode &Node, int iInc = 1) { m_EncounterRecord.IncMinimumForNode(Node, iInc); }
		bool IsActive (void) const { return (m_fInactive ? false : true); }
		bool IsAnonymous (void) const { return (m_fAnonymous ? true : false); }
		bool IsOutOfPlaneObject (void) { return (m_fOutOfPlane ? true : false); }
		bool IsBeacon (void) { return (m_fBeacon ? true : false); }
		bool IsBlacklistEnabled (void) { return (m_fNoBlacklist ? false : true); }
		bool IsDestroyWhenAbandoned (void) { return (m_fDestroyWhenAbandoned ? true : false); }
		bool IsDestroyWhenEmpty (void) { return (m_fDestroyWhenEmpty ? true : false); }
		bool IsEnemyDockingAllowed (void) { return (m_fAllowEnemyDocking ? true : false); }
		bool IsMobile (void) const { return (m_fMobile ? true : false); }
		bool IsRadioactive (void) { return (m_fRadioactive ? true : false); }
		bool IsSign (void) { return (m_fSign ? true : false); }
		bool IsSizeClass (ESizeClass iClass) const;
		bool IsShipEncounter (void) const { return (m_fShipEncounter ? true : false); }
		bool IsStatic (void) { return (m_fStatic ? true : false); }
		bool IsStationEncounter (void) const { return (m_fStationEncounter ? true : false); }
		bool IsTimeStopImmune (void) const { return (m_fTimeStopImmune ? true : false); }
		bool IsWall (void) { return (m_fWall ? true : false); }
		void MarkImages (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers);
		void OnShipEncounterCreated (SSystemCreateCtx &CreateCtx, CSpaceObject *pObj, const COrbit &Orbit);
		bool OverrideEncounterDesc (const CXMLElement &Override, CString *retsError = NULL);
		void PaintAnimations (CG32bitImage &Dest, int x, int y, int iTick);
		void PaintDevicePositions (CG32bitImage &Dest, int x, int y);
		void PaintDockPortPositions (CG32bitImage &Dest, int x, int y);
		void SetImageSelector (SSelectorInitCtx &InitCtx, CCompositeImageSelector *retSelector);
		void SetEncountered (CSystem &System) { m_EncounterRecord.AddEncounter(System); }
		void SetTempChance (int iChance) { m_iChance = iChance; }
		bool ShowsMapDetails (void) { return (m_fNoMapDetails ? false : true); }
		bool ShowsMapIcon (void) { return (m_fNoMapIcon ? false : true); }
		bool ShowsUnexploredAnnotation (void) const { return (m_fShowsUnexploredAnnotation ? true : false); }
		bool SuppressMapLabel (void) const { return m_fSuppressMapLabel; }
		bool UsesReverseArticle (void) { return (m_fReverseArticle ? true : false); }

		//	CDesignType overrides
		static CStationType *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designStationType) ? (CStationType *)pType : NULL); }
		static const CStationType *AsType (const CDesignType *pType) { return ((pType && pType->GetType() == designStationType) ? (CStationType *)pType : NULL); }
		virtual bool FindDataField (const CString &sField, CString *retsValue) const override;
		virtual CCommunicationsHandler *GetCommsHandler (void) override;
		virtual int GetLevel (int *retiMinLevel = NULL, int *retiMaxLevel = NULL) const override;
		virtual CString GetNamePattern (DWORD dwNounFormFlags = 0, DWORD *retdwFlags = NULL) const override;
		virtual CTradingDesc *GetTradingDesc (void) const override { return m_pTrade; }
		virtual DesignTypes GetType (void) const override { return designStationType; }
		virtual const CCompositeImageDesc &GetTypeImage (void) const override { return m_Image; }
		virtual bool IsVirtual (void) const override { return (m_fVirtual ? true : false); }

		static int CalcChallengeRating (const int iLevel, const Metric rBalance);
		static Metric CalcSatelliteHitsToDestroy (CXMLElement *pSatellites, int iLevel, bool bIgnoreChance = false);
		static Metric CalcSatelliteStrength (CXMLElement *pSatellites, int iLevel, bool bIgnoreChance = false);
		static Metric CalcSatelliteTreasureValue (CXMLElement *pSatellites, int iLevel, bool bIgnoreChance = false);
		static Metric GetStdChallenge (int iLevel);
		static int GetStdChallengeRating (int iLevel) { return CalcChallengeRating(iLevel, 1.0); }
		static ScaleTypes LoadScaleType (DWORD dwLoad) { return (ScaleTypes)dwLoad; }
		static ScaleTypes ParseScale (const CString sValue);
		static ESizeClass ParseSizeClass (const CString sValue);
		static DWORD SaveScaleType (ScaleTypes iScale) { return (DWORD)iScale; }

	protected:
		//	CDesignType overrides
		virtual void OnAccumulateStats (SStats &Stats) const override;
		virtual void OnAccumulateXMLMergeFlags (TSortMap<DWORD, DWORD> &MergeFlags) const override;
		virtual void OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override;
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual ICCItemPtr OnGetProperty (CCodeChainCtx &Ctx, const CString &sProperty) const override;
		virtual bool OnHasSpecialAttribute (const CString &sAttrib) const override;
		virtual void OnInitObjectData (CSpaceObject &Obj, CAttributeDataBlock &Data) const override;
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

		struct SSatImageDesc
			{
			CStationType *pType = NULL;
			const CObjectImageArray *pImage = NULL;
			CCompositeImageSelector Selector;
			int xOffset = 0;
			int yOffset = 0;
			};

		void AddTypesUsedByXML (CXMLElement *pElement, TSortMap<DWORD, bool> *retTypesUsed);
		Metric CalcBalance (void) const;
		Metric CalcBalanceHitsAdj (int iLevel) const;
		Metric CalcDefenderStrength (int iLevel) const;
		int CalcHitsToDestroy (int iLevel) const;
		TArray<SSatImageDesc> CalcSegmentDesc (void) const;
		Metric CalcTreasureValue (int iLevel) const;
		Metric CalcWeaponStrength (int iLevel) const;
		CStationEncounterDesc &GetEncounterDesc (void);

		CXMLElement *m_pDesc = NULL;

		//	Basic station descriptors

		CNameDesc m_Name;								//	Name
		CSovereignRef m_pSovereign;						//	Sovereign
		ScaleTypes m_iScale = scaleNone;				//	Scale
		Metric m_rParallaxDist = 1.0;					//	Parallax distance for background objects
		mutable int m_iLevel = 0;						//	Station level
		int m_iChallengeRating = 0;						//	If non-zero, this overrides calculated value
		Metric m_rMass = 0.0;							//	Mass of station
														//		For stars, this is in solar masses
														//		For worlds, this is in Earth masses
														//		Otherwise, in metric tons
		int m_iSize = 0;								//	Size
														//		For stars and worlds, this is in kilometers
														//		Otherwise, in meters
		int m_iFireRateAdj = 10;						//	Fire rate adjustment

		//	Armor & Hull
		CStationHullDesc m_HullDesc;					//	Hull descriptor
		int m_iStealthFromArmor = 0;								//	Stealth

		//	Devices
		IDeviceGenerator *m_pDevices = NULL;			//	Devices for the station
		CDeviceDescList m_AverageDevices;

		//	Items
		IItemGenerator *m_pItems = NULL;				//	Random item table
		CTradingDesc *m_pTrade = NULL;					//	Trading structure

		DWORD m_fMobile:1 = false;						//	Station moves
		DWORD m_fWall:1 = false;						//	Station is a wall
		DWORD m_fInactive:1 = false;					//	Station starts inactive
		DWORD m_fDestroyWhenAbandoned:1 = false;		//	Station is destroyed when at 0 hit points
		DWORD m_fDestroyWhenEmpty:1 = false;			//	Station is destroyed when last item removed
		DWORD m_fAllowEnemyDocking:1 = false;			//	Station allows enemies to dock
		DWORD m_fNoFriendlyFire:1 = false;				//	Station cannot hit friends
		DWORD m_fSign:1 = false;						//	Station is a text sign

		DWORD m_fBeacon:1 = false;						//	Station is a nav beacon
		DWORD m_fRadioactive:1 = false;					//	Station is radioactive
		DWORD m_fCanAttack:1 = false;					//	Station is active (i.e., will react if attacked)
		DWORD m_fShipEncounter:1 = false;				//	This is a ship encounter
		DWORD m_fNoMapIcon:1 = false;					//	Do not show on map
		DWORD m_fTimeStopImmune:1 = false;				//	TRUE if station is immune to time-stop
		DWORD m_fNoBlacklist:1 = false;					//	Does not blacklist player if attacked
		DWORD m_fReverseArticle:1 = false;				//	Use "a" instead of "an" and vice versa

		DWORD m_fStatic:1 = false;						//	Use CStatic instead of CStation
		DWORD m_fOutOfPlane:1 = false;					//	Background or foreground object
		DWORD m_fNoFriendlyTarget:1 = false;			//	Station cannot be hit by friends
		DWORD m_fVirtual:1 = false;						//	Virtual stations do not show up
		DWORD m_fCommsHandlerInit:1 = false;			//	TRUE if comms handler has been initialized
		DWORD m_fNoMapDetails:1 = false;                //  If TRUE, do not show in details pane in galactic map
		DWORD m_fSuppressMapLabel:1 = false;			//	If TRUE, do not show a label on system map
		DWORD m_fBuildReinforcements:1 = false;			//	If TRUE, reinforcements are built instead of brought in

		DWORD m_fStationEncounter:1 = false;			//	If TRUE, we're just an encounter wrapper that creates stations
		DWORD m_fCalcLevel:1 = false;					//	If TRUE, m_iLevel needs to be computed
		DWORD m_fBalanceValid:1 = false;				//	If TRUE, m_rCombatBalance is valid
		DWORD m_fShowsUnexploredAnnotation:1 = false;	//	If TRUE, we show unexplored annotation (used for asteroids)
		DWORD m_fForceMapLabel:1 = false;				//	If TRUE, show map label, even if we wouldn't by default.
		DWORD m_fAnonymous:1 = false;					//	If TRUE, object is anonymous world/asteroid/etc.
		DWORD m_fNoIndependentAttack:1 = false;			//	If TRUE, we only attack if our base is alive.
		DWORD m_fCannotAttack:1 = false;				//	If TRUE, object cannot attack

		//	Images
		CCompositeImageDesc m_Image;
		TArray<DWORD> m_ShipWrecks;						//	Class IDs to use as image (for shipwrecks)
		int m_iAnimationsCount = 0;						//	Number of animation sections
		SAnimationSection *m_pAnimations = NULL;		//	Animation sections (may be NULL)
		CCompositeImageDesc m_HeroImage;				//	For use in dock screens and covers
		CPaintOrder::Types m_iPaintOrder = CPaintOrder::none;	//	Paint layer (if special)

		//	Docking
		CDockScreenTypeRef m_pFirstDockScreen;			//	First screen (may be NULL)
		CDockScreenTypeRef m_pAbandonedDockScreen;		//	Screen to use when abandoned (may be NULL)
		DWORD m_dwDefaultBkgnd = 0;						//	Default background screen

		//	Behaviors
		int m_iAlertWhenAttacked = 0;					//	Chance that station will warn others when attacked
		int m_iAlertWhenDestroyed = 0;					//	Chance that station will warn others when destroyed

		CCommunicationsHandler m_OriginalCommsHandler;
		CCommunicationsHandler m_CommsHandler;			//	Communications handler

		//	Random occurrence
		CStationEncounterDesc m_EncounterDesc;			//	Random encounter information
		TUniquePtr<CStationEncounterDesc> m_pEncounterDescOverride;		//	Random encounter information
		CStationEncounterCtx m_EncounterRecord;			//	Record of encounters so far

		//	Ships
		CShipChallengeDesc m_DefenderCount;				//	Station should have this number of ships
		IShipGenerator *m_pInitialShips = NULL;			//	Ships at creation time
		IShipGenerator *m_pReinforcements = NULL;		//	Reinforcements table
		IShipGenerator *m_pEncounters = NULL;			//	Random encounters table
		int m_iEncounterFrequency = ftNotRandom;		//	Frequency of random encounter
		CRegenDesc m_ShipRegen;							//	Regen for ships docked with us
		IShipGenerator *m_pConstruction = NULL;			//	Ships built by station
		int m_iShipConstructionRate = 0;				//	Ticks between each construction
		int m_iMaxConstruction = 0;						//	Stop building when we get this many ships

		//	Satellites
		CXMLElement *m_pSatellitesDesc = NULL;

		//	Explosion
		CWeaponFireDescRef m_pExplosionType;			//	Explosion to create when station is destroyed

		//	Ejecta
		int m_iEjectaAdj = 0;							//	Adjustment to probability for ejecta when hit by weapon
														//		0 = no chance of ejecta
														//		100 = normal chance
														//		>100 = greater than normal chance
		CWeaponFireDescRef m_pEjectaType;				//	Type of ejecta generated

		//	Special descriptors
		CAsteroidDesc m_Asteroid;						//	Asteroid parameters
		CStarDesc m_Star;								//	Star parameters
		CStargateDesc m_Stargate;						//	Stargate parameters

		//	Miscellaneous
		CEffectCreatorRef m_pBarrierEffect;				//	Effect when object hits station
		CSovereignRef m_pControllingSovereign;			//	If controlled by different sovereign
														//	(e.g., centauri occupation)

		//	Cached
		mutable Metric m_rCombatBalance = 0.0;			//	Station power relative to level (1.0 == balanced)

		//	Temporary
		int m_iChance = 0;								//	Used when computing chance of encounter

		static CIntegralRotationDesc m_DefaultRotation;
	};
