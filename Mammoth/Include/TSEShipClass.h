//	TSEShipClass.h
//
//	Defines classes and interfaces for ship classes.
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

//	NOTE: We don't add default armor limits. Ship classes without explicit
//	armor limits have no limitation.
//
//#define APPLY_DEFAULT_ARMOR_LIMITS

class CShipClass;

//	Standard Metrics

class CShipStandard : public TRefCounted<CShipStandard>
	{
	public:
		enum EFields
			{
			//	These values should never be saved. It is OK to re-order and
			//	re-number them. Make sure you update m_FieldDesc when you add
			//	an entry here.

			fieldDrivePowerUse =		0,	//	Std drive power use (in 1/10th MWs)
			fieldMinHullPrice =			1,	//	Minimum price for hull (in credits)
			fieldPricePerHullPoint =	2,	//	Price per hull point (in credits)

			fieldCount =				3,
			};

		class TInitDefaults { };

		CShipStandard (void) { }
		CShipStandard (const TInitDefaults &Src);

		Metric GetValue (EFields iField) const { return m_rValue[iField]; }
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement &Desc);

		static const CShipStandard &GetDefaults (void) { return m_Default; }

	private:
		struct SFieldDesc
			{
			const char *pszFieldID;
			Metric rMin;
			Metric rMax;
			Metric rDefault;
			};

		Metric m_rValue[fieldCount] = { 0.0 };

		static SFieldDesc m_FieldDesc[fieldCount];
		static CShipStandard m_Default;
	};

//	Armor Limits

class CArmorLimits
	{
	public:
		static constexpr int INVALID_SPEED_BONUS = -100;

		enum EResults
			{
			resultOK,
			resultTooHeavy,
			resultIncompatible,
			};

		struct SSummary
			{
			int iMaxArmorMass = 0;				//	Max armor mass that can be installed
			int iStdArmorMass = 0;				//	Max armor mass that has no penalty
			int iMaxSpeedBonus = 0;				//	Largest speed bonus (i.e., at min armor)
			int iMaxSpeedPenalty = 0;			//	Largest speed penalty (negative value, at max armor)

			Metric rMaxArmorFrequency = 0.0;	//	Fraction of all armor types that can be installed
			Metric rStdArmorFrequency = 0.0;	//	Fraction of all armor types that have no penalty
			};

		ALERROR Bind (SDesignLoadCtx &Ctx);
		int CalcArmorSpeedBonus (const TArray<CItemCtx> &Armor) const;
		bool CalcArmorSpeedBonus (CItemCtx &ArmorItem, int iSegmentCount, int *retiBonus = NULL) const;
		bool CalcArmorSpeedBonus (const CString &sArmorClassID, int iSegmentCount, int *retiBonus = NULL) const;
		ICCItemPtr CalcMaxSpeedByArmorMass (CCodeChainCtx &Ctx, int iStdSpeed) const;
		void CalcSummary (const CArmorMassDefinitions &Defs, SSummary &Summary) const;
		EResults CanInstallArmor (const CItem &Item) const;
		const CString &GetMaxArmorClass (void) const { return m_sMaxArmorClass; }
		int GetMaxArmorMass (void) const { return m_iMaxArmorMass; }
		int GetMaxArmorSpeedPenalty (void) const { return m_iMaxArmorSpeedPenalty; }
		int GetMinArmorSpeedBonus (void) const { return m_iMinArmorSpeedBonus; }
		const CString &GetStdArmorClass (void) const { return m_sStdArmorClass; }
		int GetStdArmorMass (void) const { return m_iStdArmorMass; }
		bool HasArmorLimits (void) const { return (m_iType != typeNone); }
		void InitDefaultArmorLimits (int iMass, int iMaxSpeed, Metric rThrustRatio);
		ALERROR InitArmorLimitsFromXML (SDesignLoadCtx &Ctx, CXMLElement *pLimits);
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, int iHullMass, int iMaxSpeed);

	private:
		enum ETypes
			{
			typeNone,							//	No armor limits (any armor is valid; no speed adj)

			typeAuto,							//	Specify standard and max armor class and compute speed adj
			typeAutoSpeedAdj,					//	Specify standard and max limits, but calc speed adj
			typeTable,							//	Use an explicit table to set speed adj
			typeCompatible,						//	Specify standard, max, and speed bonus/penalties
			};

		struct SArmorLimits
			{
			CString sClass;
			int iMass = 0;						//	Limit mass, kg (if sClass is blank)
			TUniquePtr<CItemCriteria> pCriteria;

			int iSpeedAdj = 0;					//	Change to speed for this armor class
			};

		int CalcArmorMass (const CItemCtx &ArmorItem) const;
		int CalcArmorSpeedBonus (int iSegmentCount, int iTotalArmorMass) const;
		int CalcMinArmorMassForSpeed (int iSpeed, int iStdSpeed) const;
		bool FindArmorLimits (const CItemCtx &ItemCtx, const SArmorLimits **retpLimits = NULL, bool *retbClassFound = NULL) const;

		ETypes m_iType = typeNone;
		CItemCriteria m_ArmorCriteria;			//	Allowable armor

		CString m_sStdArmorClass;
		CString m_sMaxArmorClass;
		int m_iStdArmorMass = 0;				//	No penalty at this armor mass
		int m_iMaxArmorMass = 0;				//	Max mass of single armor segment
		int m_iMaxArmorSpeedPenalty = 0;		//	Change to speed at max armor mass (1/100th light-speed)
		int m_iMinArmorSpeedBonus = 0;			//	Change to speed at 1/2 std armor mass

		TArray<SArmorLimits> m_ArmorLimits;		//	List of limits by armor class for ship

		int m_iHullMass = 0;					//	Cached and used in various calculations

		const SArmorLimits *m_pMaxArmorLimits = NULL;
		const SArmorLimits *m_pStdArmorLimits = NULL;
	};

//	Hull Descriptor ------------------------------------------------------------

class CHullDesc
	{
	public:

		ALERROR Bind (SDesignLoadCtx &Ctx);
		const CArmorLimits &GetArmorLimits (void) const { return m_ArmorLimits; }
		int GetCargoSpace (void) const { return m_iCargoSpace; }
		int GetCounterIncrementRate(void) const { return m_iCounterIncrementRate; }
		int GetCyberDefenseLevel (void) const { return m_iCyberDefenseLevel; }
		const CItemCriteria &GetDeviceCriteria (void) const { return m_DeviceCriteria; }
		Metric GetExtraPoints (void) const { return m_rExtraPoints; }
		int GetMass (void) const { return m_iMass; }
		int GetMaxCargoSpace (void) const { return m_iMaxCargoSpace; }
		int GetMaxCounter(void) const { return m_iMaxCounter; }
		int GetMaxDevices (void) const { return m_iMaxDevices; }
		int GetMaxLaunchers (void) const { return m_iMaxLaunchers; }
		int GetMaxNonWeapons (void) const { return m_iMaxNonWeapons; }
		int GetMaxReactorPower (void) const { return m_iMaxReactorPower; }
		int GetMaxWeapons (void) const { return m_iMaxWeapons; }
		int GetSize (void) const { return m_iSize; }
		const CCurrencyAndValue &GetValue (void) const { return m_Value; }
		bool HasArmorLimits (void) const { return m_ArmorLimits.HasArmorLimits(); }
		void InitCyberDefenseLevel (int iLevel) { if (m_iCyberDefenseLevel == -1) m_iCyberDefenseLevel = iLevel; }
		void InitDefaultArmorLimits (int iMaxSpeed, Metric rThrustRatio) { m_ArmorLimits.InitDefaultArmorLimits(m_iMass, iMaxSpeed, rThrustRatio); }
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, int iMaxSpeed);
		bool IsTimeStopImmune (void) const { return m_bTimeStopImmune; }
#ifdef APPLY_DEFAULT_ARMOR_LIMITS
		bool NeedsDefaultArmorLimits (void) const { return (m_ArmorLimits.GetMaxArmorMass() == 0 && GetMass() > 0 && GetMass() < 1000); }
#else
		bool NeedsDefaultArmorLimits (void) const { return false; }
#endif
		void SetSize (int iSize) { m_iSize = iSize; }
		void SetMaxCargoSpace (int iCargoSpace) { m_iMaxCargoSpace = iCargoSpace; }
		void SetValue (const CCurrencyAndValue &Value) { m_Value = Value; }

	private:

		int m_iSize = 0;					//	Length of ship in meters
		int m_iMass = 0;					//	Empty hull mass (tons)
		CCurrencyAndValue m_Value;			//	Value of hull alone (excluding any devices/armor)
		int m_iCargoSpace = 0;				//	Default cargo space (tons)
		int m_iCounterIncrementRate = 0;	//  Value by which temperature/capacitor counter is updated every tick

		CItemCriteria m_DeviceCriteria;		//	Allowable devices
		CArmorLimits m_ArmorLimits;			//	Adjustments based on armor

		int m_iMaxCargoSpace = 0;			//	Max amount of cargo space with expansion (tons)
		int m_iMaxCounter = 0;				//  Max value of counter (used for temperature or capacitor)
		int m_iMaxReactorPower = 0;			//	Max compatible reactor power
		int m_iMaxDevices = 0;				//	Max number of devices
		int m_iMaxWeapons = 0;				//	Max number of weapon devices (including launchers)
		int m_iMaxNonWeapons = 0;			//	Max number of non-weapon devices
		int m_iMaxLaunchers = 0;			//	Max number of launchers

		Metric m_rExtraPoints = 0.0;		//	Extra point to calculate hull value

		int m_iCyberDefenseLevel = -1;		//	-1 = same as ship level

		bool m_bTimeStopImmune = false;		//	If TRUE, we're immune to timestop
	};

class CHullPointsCalculator
	{
	public:
		enum EHullPoints
			{
			fieldSlotSum			= 0,
			fieldArmorSum			= 1,
			fieldDriveSum			= 2,
			fieldCargoSum			= 3,
			fieldOtherSum			= 4,

			fieldFullSlots			= 5,
			fieldPartialSlots		= 6,
			fieldCargoSpace			= 7,
			fieldMaxCargoSpace		= 8,
			fieldArmorCount			= 9,
			fieldStdArmorMass		= 10,
			fieldMaxArmorMass		= 11,
			fieldMaxSpeed			= 12,
			fieldThrustRatio		= 13,
			fieldDrivePowerUse		= 14,
			fieldManeuverability	= 15,
			fieldDeviceSlots		= 16,
			fieldExtra				= 17,

			fieldCount				= 18,
			};

		CHullPointsCalculator (const CShipClass &Class, const CShipStandard &Standard);

		Metric GetField (int iIndex) const { return m_Data[iIndex]; }
		int GetFieldCount (void) const { return fieldCount; }
		Metric GetTotalPoints (void) const { return m_rTotalPoints; }
		CurrencyValue GetValueInCredits (void) const;

		static CString GetFieldName (int iIndex);

	private:
		Metric m_rMinPrice = 0.0;
		Metric m_rPricePerPoint = 0.0;
		Metric m_Data[fieldCount] = { 0.0 };
		Metric m_rTotalPoints = 0.0;
	};

//	Wreck Descriptor -----------------------------------------------------------

class CShipwreckDesc
	{
	public:
		void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) const;
		ALERROR Bind (SDesignLoadCtx &Ctx);
		void CleanUp (void);
		void ClearMarks (void);
		bool CreateEmptyWreck (CSystem &System, CShipClass *pClass, CShip *pShip, const CVector &vPos, const CVector &vVel, CSovereign *pSovereign, CStation **retpWreck) const;
		bool CreateWreck (CShip *pShip, CSpaceObject **retpWreck) const;
		CWeaponFireDesc *GetExplosionType (void) const { return m_pExplosionType; }
		size_t GetMemoryUsage (void) const;
		int GetStructuralHP (void) const { return m_iStructuralHP; }
		int GetWreckChance (void) const { return m_iLeavesWreck; }
		CObjectImageArray *GetWreckImage (const CShipClass *pClass, int iRotation) const;
		CStationType *GetWreckType (void) const;
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, Metric rHullMass);
		bool IsDefault (void) const { return m_bIsDefault; }
		bool IsRadioactive (void) const { return m_bRadioactiveWreck; }
		void MarkImages (CShipClass *pClass, int iRotation) const;
		void SweepImages (void);

		static void UnbindGlobal (void);
		
	private:
		void AddItemsToWreck (CShip *pShip, CSpaceObject *pWreck) const;
		int CalcDeviceComponentChance (const CItem &Item, bool bDropDamaged) const;
		int CalcDeviceDestroyChance (void) const { return 100 - Min(GetWreckChance(), 50); }
		ItemFates CalcDeviceFate (CShip *pSource, const CItem &Item, CSpaceObject *pWreck, bool bDropDamaged) const;
		bool CreateWreckImage (const CShipClass *pClass, int iRotationFrame, CObjectImageArray &Result) const;
		void InitDamageImage (void) const;
		void LoadXMLBool (const CXMLElement &Desc, const CString &sAttrib, bool &retbValue);

		static constexpr int DAMAGE_IMAGE_COUNT =		10;
		static constexpr int DAMAGE_IMAGE_WIDTH	=		24;
		static constexpr int DAMAGE_IMAGE_HEIGHT =		24;

		static constexpr int DEVICE_DAMAGED_CHANCE =	80;
		static constexpr int DROP_COMPONENTS_CHANCE =	50;

		int m_iLeavesWreck = 0;					//	Chance that it leaves a wreck
		int m_iStructuralHP = 0;				//	Structual hp of wreck (0 = undefined)
		CStationTypeRef m_pWreckType;			//	Station type to use as wreck
		CWeaponFireDescRef m_pExplosionType;	//	Explosion to create when ship is destroyed

		bool m_bIsDefault = false;				//	TRUE if initialized to defaults
		bool m_bRadioactiveWreck = false;		//	TRUE if wreck is always radioactive
		bool m_bNoItems = false;				//	If TRUE, do not bring items from ship to wreck
		bool m_bNoInstalledItems = false;		//	If TRUE, do not bring installed items from ship to wreck

		mutable TSortMap<int, CObjectImageArray> m_WreckImages;	//	Wreck image for each rotation frame index

		static CG32bitImage *m_pDamageBitmap;
		static CStationType *m_pWreckDesc;
	};

//	Ship Class -----------------------------------------------------------------

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
			typeCapitalShip,
			typeNonCombatant,

			typeTooWeak,
			typeTooStrong,
			typeArmorTooWeak,
			typeArmorTooStrong,
			typeWeaponsTooWeak,
			typeWeaponsTooStrong,
			};

		enum ECategoryFlags
			{
			catCapitalShip = 0x00000001,		//	Capital ship (based on mass)
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

		CShipClass (void) { }
		CShipClass (const CShipClass &Src) = delete;
		CShipClass (CShipClass &&Src) = delete;

		virtual ~CShipClass (void);

		CShipClass &operator= (const CShipClass &Src) = delete;
		CShipClass &operator= (CShipClass &&Src) = delete;

		int Angle2Direction (int iAngle) const { return m_Perf.GetIntegralRotationDesc().GetFrameIndex(iAngle); }
		int AlignToRotationAngle (int iAngle) const { return m_Perf.GetIntegralRotationDesc().AlignToRotationAngle(iAngle); }
		Metric CalcFuelEfficiency (const CDeviceDescList &Devices) const;
		int CalcImageSize (void) const { return m_Interior.CalcImageSize(const_cast<CShipClass *>(this)); }
		Metric CalcMass (const CDeviceDescList &Devices) const;
		int CalcRatedPowerUse (const CDeviceDescList &Devices) const;
		int CalcScore (void);
		bool CreateEmptyWreck (CSystem &System, CShip *pShip, const CVector &vPos, const CVector &vVel, CSovereign *pSovereign, CStation **retpWreck);
		void CreateImage (CG32bitImage &Dest, int iTick, int iRotation, Metric rScale = 1.0);
		void CreateScaledImage (CG32bitImage &Dest, int iTick, int iRotation, int cxWidth, int cyHeight);
		bool CreateWreck (CShip *pShip, CSpaceObject **retpWreck = NULL);
		bool FindDeviceSlotDesc (DeviceNames iDev, SDeviceDesc *retDesc) const;
		bool FindDeviceSlotDesc (CShip *pShip, const CItem &Item, SDeviceDesc *retDesc) const;

		static const DWORD GDFLAG_NO_DEVICE_SLOT_SEARCH = 0x00000001;
		void GenerateDevices (int iLevel, CDeviceDescList &Devices, DWORD dwFlags = 0) const;

		CString GenerateShipName (DWORD *retdwFlags) const;
		const CAISettings &GetAISettings (void) { return m_AISettings; }
		const CShipArmorDesc &GetArmorDesc (void) const { return m_Armor; }
		DWORD GetCategoryFlags (void) const;
		const CCargoDesc &GetCargoDesc (const CItem **retpCargoItem = NULL) const;
		CGenericType *GetCharacter (void) { return m_Character; }
		CGenericType *GetCharacterClass (void) { return m_CharacterClass; }
		Metric GetCombatStrength (void) const { return m_rCombatStrength; }
		int GetCyberDefenseLevel (void) const { return m_Hull.GetCyberDefenseLevel(); }
		DWORD GetDefaultBkgnd (void) const { return m_dwDefaultBkgnd; }
		CDesignType *GetDefaultEventHandler (void) const { return m_EventHandler; }
		CSovereign *GetDefaultSovereign (void) const { return m_pDefaultSovereign; }
		CString GetDesc (void) const;
		IDeviceGenerator *GetDeviceSlots (void) const { return m_pDeviceSlots; }
		const CDockingPorts &GetDockingPorts (void) { return m_DockingPorts; }
		CVector GetDockingPortOffset (int iRotation);
		const CDriveDesc &GetDriveDesc (const CItem **retpDriveItem = NULL) const;
		const CObjectEffectDesc &GetEffectsDesc (void) const { return m_Effects; }
		IShipGenerator *GetEscorts (void) { return m_pEscorts; }
		CWeaponFireDesc *GetExplosionType (CShip *pShip) const;
		CXMLElement *GetFirstDockScreen (void) { return m_pDefaultScreen.GetDesc(); }
		CDesignType *GetFirstDockScreen (CString *retsName) { return m_pDefaultScreen.GetDockScreen(this, retsName); }
		FrequencyTypes GetFrequency (void) const { return m_Frequency; }
		const CObjectImageArray &GetHeroImage (void) const;
		const CHullDesc &GetHullDesc (void) const { return m_Hull; }
		const CDriveDesc &GetHullDriveDesc (void) const { return m_DriveDesc; }
		const CReactorDesc *GetHullReactorDesc (void) { return &m_ReactorDesc; }
		const CRotationDesc &GetHullRotationDesc (void) const { return m_RotationDesc; }
		const CShipArmorSegmentDesc &GetHullSection (int iIndex) const { return m_Armor.GetSegment(iIndex); }
		int GetHullSectionAtAngle (int iAngle);
		int GetHullSectionCount (void) const { return m_Armor.GetCount(); }
		CString GetHullSectionName (int iIndex) const;
		CCurrencyAndValue GetHullValue (CShip *pShip = NULL) const;
		const CObjectImageArray &GetImage (const CImageFilterStack *pFilters = NULL) const;
		int GetImageViewportSize (void) const { return m_Image.GetSimpleImage().GetImageViewportSize(); }
		const CIntegralRotationDesc &GetIntegralRotationDesc (void) const { return m_Perf.GetIntegralRotationDesc(); }
		const CAttributeDataBlock &GetInitialData (void) const { return m_InitialData; }
		const CShipInteriorDesc &GetInteriorDesc (void) const { return m_Interior; }
		int GetMaxStructuralHitPoints (void) const;
		const CPlayerSettings *GetPlayerSettings (void) const;
		CString GetPlayerSortString (void) const;
		CVector GetPosOffset (int iAngle, int iRadius, int iPosZ, bool b3DPos = true);
		IItemGenerator *GetRandomItemTable (void) const { return m_pItems; }
		const CReactorDesc &GetReactorDesc (const CItem **retpReactorItem = NULL) const;
		int GetRotationAngle (void) { return m_Perf.GetIntegralRotationDesc().GetFrameAngle(); }
		const CRotationDesc &GetRotationDesc (void) const { return m_Perf.GetRotationDesc(); }
		int GetRotationRange (void) { return m_Perf.GetIntegralRotationDesc().GetFrameCount(); }
		int GetScore (void) { return m_iScore; }
		DWORD GetShipNameFlags (void) { return m_dwShipNameFlags; }
		CString GetShortName (void) const;
		const CShipStandard &GetStandard (void) const;
		Metric GetThrustRatio (void) const { return m_rThrustRatio; }
		const CString &GetClassName (void) const { return m_sName; }
		const CString &GetManufacturerName (void) const { return m_sManufacturer; }
		const CString &GetShipTypeName (void) const { return m_sTypeName; }
		int GetWreckChance (void) const { return GetWreckDesc().GetWreckChance(); }
		const CShipwreckDesc &GetWreckDesc (void) const;
		const CObjectImageArray &GetWreckImage (int iRotation) const;
		bool HasDockingPorts (void) const { return (m_DockingPorts.GetPortCount() > 0 || !m_pDefaultScreen.IsEmpty()); }
		bool HasShipName (void) const { return !m_sShipNames.IsBlank(); }
		void InitEffects (CShip *pShip, CObjectEffectList *retEffects);
		void InitPerformance (SShipPerformanceCtx &Ctx) const;
		void InstallEquipment (CShip *pShip);
		bool IsDebugOnly (void) const { const CPlayerSettings *pPlayerSettings = GetPlayerSettings(); return (pPlayerSettings && pPlayerSettings->IsDebugOnly()); }
		bool IsIncludedInAllAdventures (void) { const CPlayerSettings *pPlayerSettings = GetPlayerSettings(); return (pPlayerSettings && pPlayerSettings->IsIncludedInAllAdventures()); }
		bool IsPlayerShip (void) { return (GetPlayerSettings() != NULL); }
		bool IsShipSection (void) const { return m_fShipCompartment; }
		bool IsShownAtNewGame (void) const { const CPlayerSettings *pPlayerSettings = GetPlayerSettings(); return (pPlayerSettings && pPlayerSettings->IsInitialClass() && !IsVirtual()); }
		bool IsTimeStopImmune (void) { return m_Hull.IsTimeStopImmune(); }
		void MarkImages (bool bMarkDevices);
		void Paint (CG32bitImage &Dest, 
					int x, 
					int y, 
					const ViewportTransform &Trans, 
					int iDirection, 
					int iTick, 
					bool bThrusting = false,
					bool bRadioactive = false,
					DWORD byInvisible = 0) const;
		void PaintDevicePositions (CG32bitImage &Dest, int x, int y, const CDeviceDescList &Devices, int iShipRotation) const;
		void PaintDockPortPositions (CG32bitImage &Dest, int x, int y, int iShipRotation) const;
		void PaintInteriorCompartments (CG32bitImage &Dest, int x, int y, int iShipRotation) const;
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
					int iRotation,
					int iTick);
		void PaintThrust (CG32bitImage &Dest, 
						int x, 
						int y, 
						const ViewportTransform &Trans, 
						int iDirection, 
						int iTick,
						bool bInFrontOnly) const;
		bool ShowsMapLabel (void) const { return HasDockingPorts(); }

		//	CDesignType overrides
		static const CShipClass *AsType (const CDesignType *pType) { return ((pType && pType->GetType() == designShipClass) ? (CShipClass *)pType : NULL); }
		static CShipClass *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designShipClass) ? (CShipClass *)pType : NULL); }
		virtual bool FindDataField (const CString &sField, CString *retsValue) const override;
		virtual CCommunicationsHandler *GetCommsHandler (void) override;
		virtual const CEconomyType *GetEconomyType (void) const;
		virtual int GetLevel (int *retiMinLevel = NULL, int *retiMaxLevel = NULL) const override { if (retiMinLevel) *retiMinLevel = m_iLevel; if (retiMaxLevel) *retiMaxLevel = m_iLevel; return m_iLevel; }
		virtual CString GetNamePattern (DWORD dwNounFormFlags = 0, DWORD *retdwFlags = NULL) const override;
		virtual CCurrencyAndValue GetTradePrice (const CSpaceObject *pObj = NULL, bool bActual = false) const override;
		virtual CTradingDesc *GetTradingDesc (void) const override { return m_pTrade; }
		virtual DesignTypes GetType (void) const override { return designShipClass; }
		virtual const CCompositeImageDesc &GetTypeImage (void) const override { return m_Image; }
		virtual bool IsVirtual (void) const override { return (m_fVirtual ? true : false); }

		static Metric GetStdCombatStrength (int iLevel);
		static void Reinit (void);
		static void UnbindGlobal (void);

	protected:
		//	CDesignType overrides
		virtual void OnAccumulateStats (SStats &Stats) const;
		virtual void OnAccumulateXMLMergeFlags (TSortMap<DWORD, DWORD> &MergeFlags) const override;
		virtual void OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override;
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual void OnClearMark (void) override { m_WreckDesc.ClearMarks(); }
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual CEffectCreator *OnFindEffectCreator (const CString &sUNID) override;
		virtual CString OnGetMapDescriptionMain (const SMapDescriptionCtx &Ctx) const override;
		virtual ICCItemPtr OnGetProperty (CCodeChainCtx &Ctx, const CString &sProperty) const override;
		virtual bool OnHasSpecialAttribute (const CString &sAttrib) const override;
		virtual void OnInitObjectData (CSpaceObject &Obj, CAttributeDataBlock &Data) const override;
		virtual void OnMarkImages (void) override { MarkImages(true); }
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

		EBalanceTypes CalcBalanceType (CString *retsDesc = NULL, Metric *retrCombatStrength = NULL) const;
		CItemType *CalcBestMissile (const SDeviceDesc &Device) const;
		Metric CalcCombatStrength (void) const;
		Metric CalcDamageRate (int *retiAveWeaponLevel = NULL, int *retiMaxWeaponLevel = NULL) const;
		Metric CalcDefenseRate (void) const;
		Metric CalcDodgeRate (void) const { return CalcManeuverValue(true); }
		CurrencyValue CalcHullValue (const CShipStandard &Standard, Metric *retrPoints = NULL) const;
		int CalcLevel (void) const;
		Metric CalcManeuverValue (bool bDodge = false) const;
		ICCItemPtr CalcMaxSpeedByArmorMass (CCodeChainCtx &Ctx) const;
		void CalcPerformance (void);
		int ComputeDeviceLevel (const SDeviceDesc &Device) const;
		void ComputeMovementStats (int *retiSpeed, int *retiThrust, int *retiManeuver);
		int ComputeScore (int iArmorLevel,
						  int iPrimaryWeapon,
						  int iSpeed,
						  int iThrust,
						  int iManeuver,
						  bool bPrimaryIsLauncher);
		void FindBestMissile (CDeviceClass *pLauncher, IItemGenerator *pItems, CItemType **retpMissile) const;
		void FindBestMissile (CDeviceClass *pLauncher, const CItemList &Items, CItemType **retpMissile) const;
		CString GetGenericName (DWORD *retdwFlags = NULL) const;
		int GetManeuverDelay (void) const { return m_Perf.GetIntegralRotationDesc().GetManeuverDelay(); }
		void InitShipNamesIndices (void);

		static int CalcDefaultSize (const CObjectImageArray &Image);

		CString m_sManufacturer;				//	Name of manufacturer
		CString m_sName;						//	Class name
		CString m_sTypeName;					//	Name of type
		DWORD m_dwClassNameFlags = 0;			//	Flags for class name
		CSovereignRef m_pDefaultSovereign;		//	Sovereign

		CString m_sShipNames;					//	Names to use for individual ship
		DWORD m_dwShipNameFlags = 0;			//	Flags for ship name
		TArray<int> m_ShipNamesIndices;			//	Shuffled indices for ship names
		mutable int m_iShipName = 0;			//	Current ship name index

		int m_iScore = 0;						//	Score when destroyed
		int m_iLevel = 0;						//	Ship class level
		EBalanceTypes m_iLevelType = typeUnknown;	//	Type of ships for level
		Metric m_rCombatStrength = 0.0;			//	Combat strength
		FrequencyTypes m_Frequency = ftNotRandom;	//	Frequency

		//  Hull properties

		TSharedPtr<CShipStandard> m_pStandard;	//	Standard for metrics, etc.
		CHullDesc m_Hull;						//	Basic hull definitions
		CRotationDesc m_RotationDesc;	        //	Rotation and maneuverability
		Metric m_rThrustRatio = 0.0;			//	If non-zero, then m_DriveDesc thrust is set based on this.
		CDriveDesc m_DriveDesc;					//	Drive descriptor
		CReactorDesc m_ReactorDesc;				//	Reactor descriptor
		CShipwreckDesc m_WreckDesc;				//	Wreck descriptor

		//  Armor, Devices, Equipment, Etc.

		CShipArmorDesc m_Armor;                 //  Armor descriptor
		CShipInteriorDesc m_Interior;			//	Interior structure
		IDeviceGenerator *m_pDeviceSlots = NULL;	//	Device slots
		IDeviceGenerator *m_pDevices = NULL;	//	Generator of devices
		TArray<SEquipmentDesc> m_Equipment;		//	Initial equipment

		//  Performance Stats (after accounting for devices, etc).

		CShipPerformanceDesc m_Perf;            //  Performance based on average devices (only for stats)
		CDeviceDescList m_AverageDevices;		//	Average complement of devices (only for stats)

		//  AI & Player Settings

		CAISettings m_AISettings;				//	AI controller data
		mutable CPlayerSettings *m_pPlayerSettings = NULL;	//	Player settings data
		IItemGenerator *m_pItems = NULL;		//	Random items
		CAttributeDataBlock m_InitialData;      //  Initial data for ship object

		//	Escorts

		IShipGenerator *m_pEscorts = NULL;		//	Escorts

		//	Character

		CGenericTypeRef m_EventHandler;			//	Event handler
		CGenericTypeRef m_CharacterClass;		//	Character class
		CGenericTypeRef m_Character;			//	Character for ship

		//	Docking

		CDockingPorts m_DockingPorts;			//	Docking port definitions
		CDockScreenTypeRef m_pDefaultScreen;	//	Default screen
		DWORD m_dwDefaultBkgnd = 0;				//	Default background screen
		CTradingDesc *m_pTrade = NULL;			//	Trade descriptors

		CCommunicationsHandler m_OriginalCommsHandler;
		CCommunicationsHandler m_CommsHandler;	//	Communications handler

		//	Image

		CCompositeImageDesc m_Image;			//	Image of ship
		mutable CObjectImageArray m_HeroImage;	//  Large image
		CObjectEffectDesc m_Effects;			//	Effects for ship

		//	Exhaust

		CObjectImageArray m_ExhaustImage;		//	Image of drive exhaust
		TArray<SExhaustDesc> m_Exhaust;			//	Drive exhaust painting

		//	Misc

		DWORD m_fCommsHandlerInit:1 = false;	//	TRUE if comms handler has been initialized
		DWORD m_fVirtual:1 = false;				//	TRUE if ship class is virtual (e.g., a base class)
		DWORD m_fOwnPlayerSettings:1 = false;	//	TRUE if we own m_pPlayerSettings
		DWORD m_fScoreOverride:1 = false;		//	TRUE if score is specified in XML
		DWORD m_fLevelOverride:1 = false;		//	TRUE if level is specified in XML
		DWORD m_fInheritedDevices:1 = false;	//	TRUE if m_pDevices is inherited from another class
		DWORD m_fInheritedItems:1 = false;		//	TRUE if m_pItems is inherited from another class
		DWORD m_fInheritedEscorts:1 = false;	//	TRUE if m_pEscorts is inherited from another class

		DWORD m_fInheritedTrade:1 = false;		//	TRUE if m_pTrade is inherited from another class
		DWORD m_fShipCompartment:1 = false;		//	TRUE if we represent an attached compartment/segment
		DWORD m_fInheritedDeviceSlots:1 = false;	//	TRUE if m_pDeviceSlots is inherited from another class
		DWORD m_fHullValueOverride:1 = false;	//	TRUE if hull value is specifed in XML
		DWORD m_fInheritedStandard:1 = false;	//	TRUE if m_pStandard is inherited from another class
		DWORD m_fSpare6:1;
		DWORD m_fSpare7:1;
		DWORD m_fSpare8:1;

		DWORD m_fSpare:16;

		static CPlayerSettings m_DefaultPlayerSettings;
		static bool m_bDefaultPlayerSettingsBound;
	};
