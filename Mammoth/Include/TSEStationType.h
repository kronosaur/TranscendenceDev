//	TSEStationType.h
//
//	Defines classes and interfaces for station types.
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

//	Armor/Hull System ----------------------------------------------------------

class CStationHullDesc
	{
	public:
		void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) const;
		ALERROR Bind (SDesignLoadCtx &Ctx);
		int CalcDamageEffectiveness (CSpaceObject *pAttacker, CInstalledDevice *pWeapon) const;
		Metric CalcHitsToDestroy (int iLevel) const;
		inline bool CanBeHit (void) const { return !m_bCannotBeHit; }
		inline bool CanBeWrecked (void) const { return (!IsImmutable() && GetMaxHitPoints() > 0); }
		bool FindDataField (const CString &sField, CString *retsValue) const;
		inline CArmorClass *GetArmorClass (void) const { return (m_pArmor ? m_pArmor->GetArmorClass() : NULL); }
		int GetArmorLevel (void) const;
		inline int GetHitPoints (void) const { return m_iHitPoints; }
		inline int GetMaxHitPoints (void) const { return m_iMaxHitPoints; }
		inline int GetMaxStructuralHP (void) const { return m_iMaxStructuralHP; }
		inline const CRegenDesc &GetRegenDesc (void) const { return m_Regen; }
		inline int GetStructuralHP (void) const { return m_iStructuralHP; }
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, bool bMultiHullDefault = true);
		ALERROR InitFromStationXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		inline bool IsImmutable (void) const { return m_bImmutable; }
		inline bool IsMultiHull (void) const { return m_bMultiHull; }
		inline void SetImmutable (bool bValue = true) { m_bImmutable = bValue; }
		
	private:
		CItemTypeRef m_pArmor;				//	Armor class
		int m_iArmorLevel;					//	Armor level (for scalable items)
		int m_iHitPoints;					//	Hit points at creation time
		int m_iMaxHitPoints;				//	Max hit points
		CRegenDesc m_Regen;					//	Repair rate

		int m_iStructuralHP;				//	Initial structural hit points
		int m_iMaxStructuralHP;				//	Max structural hp (0 = station is permanent)
		
		bool m_bMultiHull;					//	Must have WMD to damage
		bool m_bImmutable;					//	Cannot be damaged
		bool m_bCannotBeHit;				//	Transparent to projectiles
	};

class CStationHull
	{
	public:
		CStationHull (void);

		inline bool CanBeDestroyed (void) const { return (m_iStructuralHP > 0); }
		inline bool CanBeHit (void) const { return !m_fCannotBeHit; }
		ICCItem *FindProperty (const CString &sProperty) const;
		inline int GetArmorLevel (void) const { return m_iArmorLevel; }
		inline int GetHitPoints (void) const { return m_iHitPoints; }
		inline int GetMaxHitPoints (void) const { return m_iMaxHitPoints; }
		inline int GetMaxStructuralHP (void) const { return m_iMaxStructuralHP; }
		EDamageResults GetPassthroughDefault (void) const;
		inline int GetStructuralHP (void) const { return m_iStructuralHP; }
		int GetVisibleDamage (void) const;
		void GetVisibleDamageDesc (SVisibleDamage &Damage) const;
		inline int IncHitPoints (int iInc) { m_iHitPoints = Max(0, m_iHitPoints + iInc); return m_iHitPoints; }
		inline int IncStructuralHP (int iInc) { m_iStructuralHP = Max(0, m_iStructuralHP + iInc); return m_iStructuralHP; }
		void Init (const CStationHullDesc &Desc);
		inline bool IsAbandoned (void) const { return (m_iHitPoints == 0 && !m_fImmutable); }
		inline bool IsImmutable (void) const { return (m_fImmutable ? true : false); }
		inline bool IsMultiHull (void) const { return (m_fMultiHull ? true : false); }
		inline bool IsWrecked (void) const { return (IsAbandoned() && m_iMaxHitPoints > 0); }
		void ReadFromStream (SLoadCtx &Ctx);
		inline void SetArmorLevel (int iValue) { m_iArmorLevel = iValue; }
		inline void SetHitPoints (int iValue) { m_iHitPoints = Max(0, iValue); }
		inline void SetImmutable (bool bValue = true) { m_fImmutable = bValue; }
		inline void SetMaxHitPoints (int iValue) { m_iMaxHitPoints = Max(0, iValue); }
		inline void SetMaxStructuralHP (int iValue) { m_iMaxStructuralHP = Max(0, iValue); }
		inline void SetMultiHull (bool bValue = true) { m_fMultiHull = bValue; }
		bool SetPropertyIfFound (const CString &sProperty, ICCItem *pValue, CString *retsError);
		inline void SetStructuralHP (int iValue) { m_iStructuralHP = Max(0, iValue); }
		void UpdateExtended (const CStationHullDesc &Desc, const CTimeSpan &ExtraTime);
		bool UpdateRepairDamage (const CStationHullDesc &Desc, int iTick);
		void WriteToStream (IWriteStream &Stream, CStation *pStation);

	private:
		int m_iArmorLevel;					//	Armor level (for scalable items)
		int m_iHitPoints;					//	Total hit points (0 = station abandoned)
		int m_iMaxHitPoints;				//	Max hit points (0 = station cannot be abandoned)
		int m_iStructuralHP;				//	Structural hp (0 = station cannot be destroyed)
		int m_iMaxStructuralHP;				//	Max structural hp
		
		DWORD m_fMultiHull:1;				//	Must have WMD to damage
		DWORD m_fImmutable:1;				//	Cannot be damaged
		DWORD m_fCannotBeHit:1;				//	Cannot be hit
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

		int CalcAffinity (CTopologyNode *pNode) const;
		int CalcLevelFromFrequency (void) const;
		bool InitAsOverride (const CStationEncounterDesc &Original, const CXMLElement &Override, CString *retsError);
		ALERROR InitFromStationTypeXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		void InitLevelFrequency (CTopology &Topology);
		inline bool CanBeRandomlyEncountered (void) const { return (!m_sLevelFrequency.IsBlank() || m_bNumberAppearing); }
		int GetCountOfRandomEncounterLevels (void) const;
		inline const CTopologyNode::SAttributeCriteria &GetDistanceCriteria (void) const { return m_DistanceCriteria; }
		void GetExclusionDesc (SExclusionDesc &Exclusion) const;
		inline Metric GetExclusionRadius (void) const { return m_rExclusionRadius; }
		inline Metric GetEnemyExclusionRadius (void) const { return m_rEnemyExclusionRadius; }
		int GetFrequencyByDistance (int iDistance) const;
		int GetFrequencyByLevel (int iLevel) const;
		inline const CString &GetLevelFrequency (void) const { return m_sLevelFrequency; }
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
		inline bool HasSystemLimit (int *retiLimit = NULL) const { if (retiLimit) *retiLimit = m_iMaxCountInSystem; return (m_iMaxCountInSystem != -1); }
		inline bool IsUniqueInSystem (void) const { return (m_iMaxCountInSystem == 1); }
		void ReadFromStream (SUniverseLoadCtx &Ctx);
		void WriteToStream (IWriteStream *pStream);

	private:
		bool m_bSystemCriteria = false;				//	If TRUE we have system criteria
		CTopologyNode::SCriteria m_SystemCriteria;	//	System criteria

		CTopologyNode::SAttributeCriteria m_DistanceCriteria;	//	Criteria for nodes for distance calc
		CString m_sDistanceFrequency;				//	Frequency distribution by distance from criteria
		CString m_sLevelFrequency;					//	String array of frequency distribution by level
		CAttributeCriteria m_SystemAffinity;		//	Adjust frequency based on number of matches

		CString m_sLocationCriteria;				//	Criteria for location
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

		static int CalcDistanceToCriteria (CTopologyNode *pNode, const CTopologyNode::SAttributeCriteria &Criteria);

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
		int GetCountInSystem (CSystem *pSystem, CStationType *pStationType) const;

		SEncounterStats m_Total;			//	Encounters in entire game
		TSortMap<int, SEncounterStats> m_ByLevel;	//	Encounters by system level
		TSortMap<CString, SEncounterStats> m_ByNode;	//	Encounters by topology node
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
		inline CXMLElement *GetAbandonedScreen (void) { return m_pAbandonedDockScreen.GetDesc(); }
		inline CDesignType *GetAbandonedScreen (CString *retsName) { return m_pAbandonedDockScreen.GetDockScreen(this, retsName); }
		CurrencyValue GetBalancedTreasure (void) const;
		inline CEffectCreator *GetBarrierEffect (void) { return m_pBarrierEffect; }
		inline IShipGenerator *GetConstructionTable (void) { return m_pConstruction; }
		CSovereign *GetControllingSovereign (void);
		inline DWORD GetDefaultBkgnd (void) { return m_dwDefaultBkgnd; }
		inline const CShipChallengeDesc &GetDefenderCount (void) const { return m_DefenderCount; }
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
		inline const CStationHullDesc &GetHullDesc (void) const { return m_HullDesc; }
		inline const CCompositeImageDesc &GetImage (void) { return m_Image; }
		inline const CObjectImageArray &GetImage (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers, int *retiRotation = NULL) { return m_Image.GetImage(Selector, Modifiers, retiRotation); }
		inline int GetImageVariants (void) { return m_Image.GetVariantCount(); }
		inline IShipGenerator *GetInitialShips (void) const { return m_pInitialShips; }
		Metric GetLevelStrength (int iLevel);
		inline const CString &GetLocationCriteria (void) const { return GetEncounterDesc().GetLocationCriteria(); }
		inline Metric GetMass (void) { return m_rMass; }
		inline Metric GetMaxEffectiveRange (void) { if (m_fCalcMaxAttackDist) CalcMaxAttackDistance(); return m_rMaxAttackDistance; }
		inline int GetMaxLightDistance (void) { return m_iMaxLightDistance; }
		inline int GetMaxShipConstruction (void) { return m_iMaxConstruction; }
		inline const CNameDesc &GetNameDesc (void) const { return m_Name; }
		inline int GetNumberAppearing (void) const { return m_EncounterRecord.GetTotalMinimum(); }
		inline Metric GetParallaxDist (void) const { return m_rParallaxDist; }
		inline IItemGenerator *GetRandomItemTable (void) { return m_pItems; }
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
		inline int GetTempChance (void) const { return m_iChance; }
		inline bool HasAnimations (void) const { return (m_pAnimations != NULL); }
		inline bool HasGravity (void) const { return (m_rGravityRadius > 0.0); }
		inline bool HasWreckImage (void) const { return m_HullDesc.CanBeWrecked(); }
		inline void IncEncounterMinimum (CTopologyNode *pNode, int iInc = 1) { m_EncounterRecord.IncMinimumForNode(pNode, GetEncounterDesc(), iInc); }
		inline bool IsActive (void) { return (m_fInactive ? false : true); }
		inline bool IsOutOfPlaneObject (void) { return (m_fOutOfPlane ? true : false); }
		inline bool IsBeacon (void) { return (m_fBeacon ? true : false); }
		inline bool IsBlacklistEnabled (void) { return (m_fNoBlacklist ? false : true); }
		inline bool IsDestroyWhenEmpty (void) { return (m_fDestroyWhenEmpty ? true : false); }
		inline bool IsEnemyDockingAllowed (void) { return (m_fAllowEnemyDocking ? true : false); }
		inline bool IsMobile (void) const { return (m_fMobile ? true : false); }
		inline bool IsPaintLayerOverhang (void) const { return (m_fPaintOverhang ? true : false); }
		inline bool IsRadioactive (void) { return (m_fRadioactive ? true : false); }
		inline bool IsSign (void) { return (m_fSign ? true : false); }
		bool IsSizeClass (ESizeClass iClass) const;
		inline bool IsShipEncounter (void) const { return (m_fShipEncounter ? true : false); }
		inline bool IsStatic (void) { return (m_fStatic ? true : false); }
		inline bool IsStationEncounter (void) const { return (m_fStationEncounter ? true : false); }
		inline bool IsTimeStopImmune (void) const { return (m_fTimeStopImmune ? true : false); }
		inline bool IsUniqueInSystem (void) const { return GetEncounterDesc().IsUniqueInSystem(); }
		inline bool IsWall (void) { return (m_fWall ? true : false); }
		void MarkImages (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers);
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
		inline bool ShowsUnexploredAnnotation (void) const { return (m_fShowsUnexploredAnnotation ? true : false); }
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
		static inline ScaleTypes LoadScaleType (DWORD dwLoad) { return (ScaleTypes)dwLoad; }
		static ScaleTypes ParseScale (const CString sValue);
		static ESizeClass ParseSizeClass (const CString sValue);
		static inline DWORD SaveScaleType (ScaleTypes iScale) { return (DWORD)iScale; }

	protected:
		//	CDesignType overrides
		virtual void OnAccumulateStats (SStats &Stats) const override;
		virtual void OnAccumulateXMLMergeFlags (TSortMap<DWORD, DWORD> &MergeFlags) const override;
		virtual void OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override;
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual ALERROR OnFinishBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ICCItemPtr OnGetProperty (CCodeChainCtx &Ctx, const CString &sProperty) const override;
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
		Metric CalcBalance (void) const;
		Metric CalcDefenderStrength (int iLevel) const;
		int CalcHitsToDestroy (int iLevel) const;
		Metric CalcMaxAttackDistance (void);
		Metric CalcTreasureValue (int iLevel) const;
		Metric CalcWeaponStrength (int iLevel) const;
		void InitStationDamage (void);

		CXMLElement *m_pDesc;

		//	Basic station descriptors

		CNameDesc m_Name;								//	Name
		CSovereignRef m_pSovereign;						//	Sovereign
		ScaleTypes m_iScale;							//	Scale
		Metric m_rParallaxDist;							//	Parallax distance for background objects
		mutable int m_iLevel;							//	Station level
		Metric m_rMass;									//	Mass of station
														//		For stars, this is in solar masses
														//		For worlds, this is in Earth masses
														//		Otherwise, in metric tons
		int m_iSize;									//	Size
														//		For stars and worlds, this is in kilometers
														//		Otherwise, in meters
		int m_iFireRateAdj;								//	Fire rate adjustment

		//	Armor & Hull
		CStationHullDesc m_HullDesc;					//	Hull descriptor
		int m_iStealth;									//	Stealth

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
		DWORD m_fNoMapIcon:1;							//	Do not show on map
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
		DWORD m_fCalcLevel:1;							//	If TRUE, m_iLevel needs to be computed
		DWORD m_fBalanceValid:1;						//	If TRUE, m_rCombatBalance is valid
		DWORD m_fShowsUnexploredAnnotation:1;			//	If TRUE, we show unexplored annotation (used for asteroids)
		DWORD m_fCalcMaxAttackDist:1;					//	If TRUE, we need to compute m_rMaxAttackDistance
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
		CShipChallengeDesc m_DefenderCount;				//	Station should have this number of ships
		IShipGenerator *m_pInitialShips;				//	Ships at creation time
		IShipGenerator *m_pReinforcements;				//	Reinforcements table
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

		//	Cached
		mutable Metric m_rCombatBalance = 0.0;			//	Station power relative to level (1.0 == balanced)

		//	Temporary
		int m_iChance;									//	Used when computing chance of encounter

		static CIntegralRotationDesc m_DefaultRotation;
	};
