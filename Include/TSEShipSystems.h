//	TSEShipSystems.h
//
//	Defines classes and interfaces for ships
//	Copyright (c) 2016 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

struct SShipPerformanceCtx;

//	Basic ship properties ------------------------------------------------------

enum ObjectComponentTypes
	{
	comArmor,
	comCargo,
	comShields,
	comWeapons,
	comDrive,
	comReactor,
	comDeviceCounter,					//	One or more devices need to show a counter
	};

enum ProgramTypes
	{
	progNOP,
	progShieldsDown,
	progReboot,
	progDisarm,

	progCustom,
	};

struct ProgramDesc
	{
	ProgramTypes iProgram;
	CString sProgramName;
	int iAILevel;

	//	Used for custom programs
	CEvalContext *pCtx;
	ICCItem *ProgramCode;
	};

struct STargetingCtx
	{
	STargetingCtx (void) :
			bRecalcTargets(true)
		{ }

	TArray<CSpaceObject *> Targets;
	bool bRecalcTargets;
	};

//	Ship Structure and Compartments --------------------------------------------

enum ECompartmentTypes
	{
	deckUnknown =						-1,

	deckGeneral =						0,	//	General interior compartment or deck
	deckMainDrive =						1,	//	Main drive
	deckCargo =							2,	//	Cargo hold
	};

struct SCompartmentDesc
	{
	SCompartmentDesc (void) :
			iType(deckUnknown),
			iMaxHP(0),
			fDefault(false)
		{
		rcPos.left = 0;
		rcPos.top = 0;
		rcPos.right = 0;
		rcPos.bottom = 0;
		}

	CString sName;							//	User-visible name (e.g., "bridge")
	ECompartmentTypes iType;				//	Type of compartment
	int iMaxHP;								//	Initial HP
	RECT rcPos;								//	Position and size relative to image

	DWORD fDefault:1;						//	Default compartment (any space not used by another compartment)
	};

class CShipInteriorDesc
	{
	public:
		ALERROR BindDesign (SDesignLoadCtx &Ctx);
		inline int GetCount (void) const { return m_Compartments.GetCount(); }
		inline const SCompartmentDesc &GetCompartment (int iIndex) const { return m_Compartments[iIndex]; }
		int GetHitPoints (void) const;
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		inline bool IsEmpty (void) const { return m_Compartments.GetCount() == 0; }

	private:
		TArray<SCompartmentDesc> m_Compartments;
	};

class CShipInterior
	{
	public:
		EDamageResults Damage (CShip *pShip, const CShipInteriorDesc &Desc, SDamageCtx &Ctx);
		void GetHitPoints (CShip *pShip, const CShipInteriorDesc &Desc, int *retiHP, int *retiMaxHP = NULL) const;
		void Init (const CShipInteriorDesc &Desc);
		inline bool IsEmpty (void) const { return m_Compartments.GetCount() == 0; }
		void ReadFromStream (CShip *pShip, const CShipInteriorDesc &Desc, SLoadCtx &Ctx);
		void SetHitPoints (CShip *pShip, const CShipInteriorDesc &Desc, int iHP);
		void WriteToStream (IWriteStream *pStream);

	private:
		struct SCompartmentEntry
			{
			SCompartmentEntry (void) :
					iHP(0)
				{ }

			int iHP;						//	HP left

			//	Temporaries
			bool bHit;						//	TRUE if this compartment got a direct hit
			};

		struct SHitTestCtx
			{
			SHitTestCtx (CShip *pShipArg, const CShipInteriorDesc &DescArg) :
					pShip(pShipArg),
					Desc(DescArg),
					iPos(-1)
				{ }

			CShip *pShip;
			const CShipInteriorDesc &Desc;

			int iPos;
			TSortMap<DWORD, int> HitOrder;
			};

		int FindNextCompartmentHit (SHitTestCtx &HitCtx, int xHitPos, int yHitPos);
		bool PointInCompartment (SHitTestCtx &HitCtx, const SCompartmentDesc &CompDesc, int xHitPos, int yHitPos) const;

		TArray<SCompartmentEntry> m_Compartments;
	};

//	Maneuvering ----------------------------------------------------------------
//
//  We use three different classes for tracking rotations/maneuvering.
//
//  CRotationDesc describes the ideal rotation parameters in terms of floating-
//  point degrees per tick. These do not deal with integral rotation frames
//  (other than for purposes of determining parameters in backwards 
//  compatibility mode).
//
//  CRotationDesc is defined by the ship class, but can be modified by devices
//  or other enhancements to a ship.
//
//  CIntegralRotationDesc describes the integral parameters in terms of frames
//  (or fractions of frames, using fixed-point precision). We do this in order 
//  to remove any possibility of round-off errors (or precision errors) from
//  creeping into our calculations. This makes rotation predictable and
//  reversible.
//
//  At bind-time, CShipClass computes its CIntegralRotationDesc (in the 
//  performance structure) based on its base CRotationDesc and any default
//  devices. This is mostly used to return class performance characteristics,
//  such as maneuverability.
//
//  In general, when asking about CShipClass characteristics, callers should
//  ask the class's CIntegralRotationDesc (in the performance structure) rather
//  than CRotationDesc (because the former accounts for installed devices).
//
//  CShip objects compute their own copy of CIntegralRotationDesc when computing
//  their own performance structure (generally after any device is installed or
//  enhanced). Any of the parameters in CRotationDesc can change after bind-time
//  (except frame count).
//
//  CIntegralRotation holds the current rotation and rotation acceleration of
//  a ship. Only CShip objects have (or need) this class. It is initialized from
//  a CIntegralRotationDesc and generally refers to it when doing calculations.

enum EManeuverTypes
	{
	NoRotation,

	RotateLeft,
	RotateRight,
	};

class CRotationDesc
	{
	public:
		CRotationDesc (void) { }

        void Add (const CRotationDesc &Src);
        bool AdjForShipMass (Metric rHullMass, Metric rItemMass);
		ALERROR Bind (SDesignLoadCtx &Ctx, CObjectImageArray &Image);
		inline int GetFrameCount (void) const { return m_iCount; }
        inline Metric GetMaxRotationPerTick (void) const { return m_rDegreesPerTick; }
        inline Metric GetRotationAccelPerTick (void) const { return m_rAccelPerTick; }
        inline Metric GetRotationAccelStopPerTick (void) const { return m_rAccelPerTickStop; }
		ALERROR InitFromManeuverXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, Metric rDefaultDegreesPerTick = 0.01);
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
        void Interpolate (const CRotationDesc &From, const CRotationDesc &To, Metric rInterpolate = 0.5);


	private:
		struct SEntry
			{
			int iRotation;					//	Angle at this rotation position
			};

		void InitRotationCount (int iCount);

		int m_iCount;						//	Number of rotations
		Metric m_rDegreesPerTick;			//	Rotations per tick
		Metric m_rAccelPerTick;				//	Degrees acceleration per tick
		Metric m_rAccelPerTickStop;			//	Degrees acceleration per tick when stoping rotation
		int m_iManeuverability;				//	Only for backwards compatibility (during InitFromXML)
	};

class CIntegralRotationDesc
    {
    public:
		enum EConstants
			{
			ROTATION_FRACTION =				1024,
			};

        CIntegralRotationDesc (void);
        inline explicit CIntegralRotationDesc (const CRotationDesc &Desc) { InitFromDesc(Desc); }

        inline int AlignToRotationAngle (int iAngle) const { return GetRotationAngle(GetFrameIndex(iAngle)); }
		int CalcFinalRotationFrame (int iRotationFrame, int iRotationSpeed) const;
		inline int GetFrameAngle (void) const { return (m_iCount > 0 ? (int)((360.0 / m_iCount) + 0.5) : 0); }
		inline int GetFrameCount (void) const { return m_iCount; }
		inline int GetFrameIndex (int iAngle) const { return (m_iCount > 0 ? (m_FacingsData[m_iCount].AngleToFrameIndex[AngleMod(iAngle)]) : 0); }
		int GetManeuverDelay (void) const;
		inline Metric GetManeuverRatio (void) const { return (Metric)m_iMaxRotationRate / ROTATION_FRACTION; }
		inline int GetMaxRotationSpeed (void) const { return m_iMaxRotationRate; }
		Metric GetMaxRotationSpeedDegrees (void) const;
		inline int GetMaxRotationTimeTicks (void) const { Metric rSpeed = GetMaxRotationSpeedDegrees(); return (rSpeed > 0.0 ? (int)(360.0 / rSpeed) : 0); }
		inline int GetRotationAccel (void) const { return m_iRotationAccel; }
		inline int GetRotationAccelStop (void) const { return m_iRotationAccelStop; }
		inline int GetRotationAngle (int iIndex) const { return (m_iCount > 0 ? m_FacingsData[m_iCount].FrameIndexToAngle[iIndex % m_iCount] : 0); }
        void InitFromDesc (const CRotationDesc &Desc);

        static int GetRotationAngle (int iCount, int iIndex) { return ((iCount > 0 && iCount <= 360 && m_FacingsData[iCount].bInitialized) ? m_FacingsData[iCount].FrameIndexToAngle[iIndex % iCount] : 0); }

    private:
		struct SEntry
			{
			int iRotation;					//	Angle at this rotation position
			};

		struct SFacingsData
			{
			SFacingsData (void) :
					bInitialized(false)
				{ }

			bool bInitialized;
			TArray<int> AngleToFrameIndex;
			TArray<int> FrameIndexToAngle;
			};

        int m_iCount;                       //  Number of frames
		int m_iMaxRotationRate;				//	Rotations per tick (in 1/1000ths of a rotation)
		int m_iRotationAccel;				//	Rotation acceleration (in 1/1000ths of a rotation)
		int m_iRotationAccelStop;			//	Rotation acceleration when stopping rotation (in 1/1000th of a rotation)

		static SFacingsData m_FacingsData[360 + 1];
    };

class CIntegralRotation
	{
	public:
		CIntegralRotation (void) :
				m_iRotationFrame(0),
				m_iRotationSpeed(0),
				m_iLastManeuver(NoRotation)
			{ }

		~CIntegralRotation (void);

		inline int CalcFinalRotationFrame (const CIntegralRotationDesc &Desc) const { return Desc.CalcFinalRotationFrame(m_iRotationFrame, m_iRotationSpeed); }
		inline int GetFrameIndex (void) const { return GetFrameIndex(m_iRotationFrame); }
		inline EManeuverTypes GetLastManeuver (void) const { return m_iLastManeuver; }
		EManeuverTypes GetManeuverToFace (const CIntegralRotationDesc &Desc, int iAngle) const;
		int GetRotationAngle (const CIntegralRotationDesc &Desc) const;
		void Init (const CIntegralRotationDesc &Desc, int iRotationAngle = -1);
		inline bool IsPointingTo (const CIntegralRotationDesc &Desc, int iAngle) const { return (GetManeuverToFace(Desc, iAngle) == NoRotation); }
		void ReadFromStream (SLoadCtx &Ctx, const CIntegralRotationDesc &Desc);
		void SetRotationAngle (const CIntegralRotationDesc &Desc, int iAngle);
		void Update (const CIntegralRotationDesc &Desc, EManeuverTypes iManeuver);
		void WriteToStream (IWriteStream *pStream) const;

	private:
		inline int GetFrameIndex (int iFrame) const { return (iFrame / CIntegralRotationDesc::ROTATION_FRACTION); }

		int m_iRotationFrame;				//	Current rotation (in 1/1000ths of a rotation)
		int m_iRotationSpeed;				//	Current rotation speed (+ clockwise; - counterclockwise; in 1/1000ths)
		EManeuverTypes m_iLastManeuver;		//	Maneuver on last update
	};

//	Equipment (Abilities) ------------------------------------------------------

enum Abilities
	{
	ablUnknown =				-1,

	ablShortRangeScanner =		0,		//	Main viewscreen
	ablLongRangeScanner =		1,		//	LRS
	ablSystemMap =				2,		//	System map display
	ablAutopilot =				3,		//	Autopilot
	ablExtendedScanner =		4,		//	Extended marks on viewscreen
	ablTargetingSystem =		5,		//	Targeting computer
	ablGalacticMap =			6,		//	Galactic map display
	};

enum AbilityModifications
	{
	ablModificationUnknown =	-1,

	ablInstall =				0,		//	Install the ability
	ablRemove =					1,		//	Remove the ability (if installed)
	ablDamage =					2,		//	Damage the ability (if installed)
	ablRepair =					3,		//	Repair the ability (if damaged)
	};

enum AbilityModificationOptions
	{
	ablOptionUnknown =			0x00000000,

	ablOptionNoMessage =		0x00000001,	//	Do not show a message to player
	};

enum AbilityStatus
	{
	ablStatusUnknown =			-1,

	ablUninstalled =			0,		//	>0 means that is installed (though it could be damaged)
	ablInstalled =				1,
	ablDamaged =				2,
	};

//  Cargo ----------------------------------------------------------------------

class CCargoDesc
    {
    public:
        CCargoDesc (int iCargoSpace = -1) :
                m_iCargoSpace(iCargoSpace)
            { }

        inline int GetCargoSpace (void) const { return m_iCargoSpace; }
        ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
        void Interpolate (const CCargoDesc &From, const CCargoDesc &To, Metric rInterpolate = 0.5);
        inline bool IsEmpty (void) const { return (m_iCargoSpace == -1); }
        inline void SetCargoSpace (int iCargoSpace) { m_iCargoSpace = iCargoSpace; }

    private:
        int m_iCargoSpace;                  //  Cargo space in tons
    };

//  Drive ----------------------------------------------------------------------

class CDriveDesc
	{
    public:
        CDriveDesc (void);

        void Add (const CDriveDesc &Src);
        Metric AdjMaxSpeed (Metric rAdj);
        int AdjPowerUse (Metric rAdj);
        int AdjThrust (Metric rAdj);
        inline Metric GetMaxSpeed (void) const { return m_rMaxSpeed; }
        inline int GetPowerUse (void) const { return m_iPowerUse; }
        inline int GetThrust (void) const { return m_iThrust; }
        inline DWORD GetUNID (void) const { return m_dwUNID; }
        ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, DWORD dwUNID, bool bShipClass = false);
        void Interpolate (const CDriveDesc &From, const CDriveDesc &To, Metric rInterpolate = 0.5);
        inline bool IsInertialess (void) const { return (m_fInertialess ? true : false); }
        inline void SetInertialess (bool bValue = true) { m_fInertialess = bValue; }
        inline void SetMaxSpeed (Metric rSpeed) { m_rMaxSpeed = rSpeed; }
        inline void SetPowerUse (int iPowerUse) { m_iPowerUse = iPowerUse; }
        inline void SetThrust (int iThrust) { m_iThrust = iThrust; }
        inline void SetUNID (DWORD dwUNID) { m_dwUNID = dwUNID; }

    private:
	    DWORD m_dwUNID;						//	UNID source (either ship class or device)
	    Metric m_rMaxSpeed;					//	Max speed (Km/sec)
	    int m_iThrust;						//	Thrust (GigaNewtons--gasp!)
	    int m_iPowerUse;					//	Power used while thrusting (1/10 megawatt)

	    DWORD m_fInertialess:1;				//	Inertialess drive
	    DWORD m_dwSpare:31;
	};

//  Reactor --------------------------------------------------------------------

class CReactorDesc
	{
    public:
        struct SStdStats
            {
            int iMaxPower;                  //  Max power (1/10 MW)
            Metric rFuelDensity;            //  Std fuel rods per 100 Kg
            Metric rCost;                   //  Credits per 100 fuel units
            };

	    CReactorDesc (void);
        CReactorDesc (const CReactorDesc &Src) { Copy(Src); }

	    inline ~CReactorDesc (void) { CleanUp(); }
        inline CReactorDesc &operator= (const CReactorDesc &Src) { CleanUp(); Copy(Src); return *this; }

        int AdjMaxPower (Metric rAdj);
        Metric AdjEfficiency (Metric rAdj);
        bool FindDataField (const CString &sField, CString *retsValue) const;
        ICCItem *FindProperty (const CString &sProperty) const;
        inline Metric GetEfficiency (void) const { return m_rPowerPerFuelUnit; }
        int GetEfficiencyBonus (void) const;
        inline Metric GetFuelCapacity (void) const { return m_rMaxFuel; }
        CString GetFuelCriteriaString (void) const;
        void GetFuelLevel (int *retiMin, int *retiMax) const;
        inline int GetMaxPower (void) const { return m_iMaxPower; }
        ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, DWORD dwUNID, bool bShipClass = false);
        ALERROR InitScaled (SDesignLoadCtx &Ctx, const CReactorDesc &Src, int iBaseLevel, int iScaledLevel);
        bool IsFuelCompatible (const CItem &FuelItem) const;
		inline void SetMaxPower (int iPower) { m_iMaxPower = iPower; }
		inline bool UsesFuel (void) const { return (m_fNoFuel ? false : true); }

        static const SStdStats &GetStdStats (int iLevel);
		static bool IsExportedProperty (const CString &sProperty);

    private:
        void CleanUp (void);
        void Copy (const CReactorDesc &Src);

		DWORD m_dwUNID;						//	UNID source (either ship class or device)
	    int m_iMaxPower;					//	Maximum power output
	    Metric m_rMaxFuel;					//	Maximum fuel space
	    Metric m_rPowerPerFuelUnit;			//	MW/10-tick per fuel unit

	    CItemCriteria *m_pFuelCriteria;
	    int m_iMinFuelLevel;				//	Min tech level of fuel (-1 if using fuelCriteria)
	    int m_iMaxFuelLevel;				//	Max tech level of fuel (-1 if using fuelCriteria)

	    DWORD m_fFreeFuelCriteria:1;		//	TRUE if we own pFuelCriteria
		DWORD m_fNoFuel:1;					//	TRUE if we don't need fuel
		DWORD m_fSpare3:1;
		DWORD m_fSpare4:1;
		DWORD m_fSpare5:1;
		DWORD m_fSpare6:1;
		DWORD m_fSpare7:1;
		DWORD m_fSpare8:1;

	    DWORD m_dwSpare:24;

        static SStdStats m_Stats[MAX_ITEM_LEVEL];
	};

//  Devices --------------------------------------------------------------------

enum DeviceNames
	{
	devNone = -1,

	devFirstName = 0,

	devPrimaryWeapon = 0,
	devMissileWeapon = 1,

	devShields = 8,
	devDrive = 9,
	devCargo = 10,
	devReactor = 11,

	devNamesCount = 12
	};

class CDeviceClass
	{
	public:
		enum CounterTypes
			{
			cntNone,							//	No counter
			cntTemperature,						//	Current device temperature (0-100)
			cntRecharge,						//	Current recharge level (0-100)
			cntCapacitor,						//	Current capacitor level (0-100)
			};

		enum DeviceNotificationTypes
			{
			statusDisruptionRepaired,
			statusUsedLastAmmo,

			failDamagedByDisruption,
			failDeviceHitByDamage,
			failDeviceHitByDisruption,
			failDeviceOverheat,
			failShieldFailure,
			failWeaponExplosion,
			failWeaponJammed,
			failWeaponMisfire,
			};

		enum LinkedFireOptions
			{
			lkfAlways =				0x0000001,	//	Linked to fire button
			lkfTargetInRange =		0x0000002,	//	Fire only if the target is in range
			lkfEnemyInRange =		0x0000004,	//	Fire only an enemy is in range
			};

		enum ECachedHandlers
			{
			evtGetOverlayType			= 0,

			evtCount					= 1,
			};

		CDeviceClass (void) : m_pItemType(NULL) { }
		virtual ~CDeviceClass (void) { }

		void AccumulateAttributes (CItemCtx &ItemCtx, const CItem &Ammo, TArray<SDisplayAttribute> *retList);
		bool AccumulateEnhancements (CItemCtx &Device, CInstalledDevice *pTarget, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements);
        bool AccumulatePerformance (CItemCtx &ItemCtx, SShipPerformanceCtx &Ctx) const;
		void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed);
		ALERROR Bind (SDesignLoadCtx &Ctx);
        inline ALERROR FinishBind (SDesignLoadCtx &Ctx) { return OnFinishBind(Ctx); }
		inline CEffectCreator *FindEffectCreator (const CString &sUNID) { return OnFindEffectCreator(sUNID); }
		inline bool FindEventHandlerDeviceClass (ECachedHandlers iEvent, SEventHandlerDesc *retEvent = NULL) const { if (retEvent) *retEvent = m_CachedEvents[iEvent]; return (m_CachedEvents[iEvent].pCode != NULL); }
		COverlayType *FireGetOverlayType(CItemCtx &Ctx) const;
        bool GetAmmoItemPropertyBool (CItemCtx &Ctx, const CItem &Ammo, const CString &sProperty);
        Metric GetAmmoItemPropertyDouble (CItemCtx &Ctx, const CItem &Ammo, const CString &sProperty);
		inline ItemCategories GetCategory (void) const { return (m_iSlotCategory == itemcatNone ? GetImplCategory() : m_iSlotCategory); }
		inline CString GetDataField (const CString &sField) { CString sValue; FindDataField(sField, &sValue); return sValue; }
		inline int GetDataFieldInteger (const CString &sField) { CString sValue; if (FindDataField(sField, &sValue)) return strToInt(sValue, 0, NULL); else return 0; }
		int GetInstallCost (CItemCtx &ItemCtx);
		inline CItemType *GetItemType (void) const { return m_pItemType; }
		inline int GetLevel (void) const;
		inline int GetMaxHPBonus (void) const { return m_iMaxHPBonus; }
		inline CString GetName (void);
		inline COverlayType *GetOverlayType(void) const { return m_pOverlayType; }
		CString GetReference (CItemCtx &Ctx, const CItem &Ammo = CItem(), DWORD dwFlags = 0);
		CString GetReferencePower (CItemCtx &Ctx);
        inline Metric GetScaledCostAdj (CItemCtx &Ctx) const { return OnGetScaledCostAdj(Ctx); }
		inline int GetSlotsRequired (void) const { return m_iSlots; }
		inline DWORD GetUNID (void);
		inline void MarkImages (void) { DEBUG_TRY OnMarkImages(); DEBUG_CATCH }

		virtual bool AbsorbDamage (CInstalledDevice *pDevice, CSpaceObject *pShip, SDamageCtx &Ctx) { Ctx.iAbsorb = 0; return false; }
		virtual bool AbsorbsWeaponFire (CInstalledDevice *pDevice, CSpaceObject *pSource, CInstalledDevice *pWeapon) { return false; }
		virtual bool Activate (CInstalledDevice *pDevice, 
							   CSpaceObject *pSource, 
							   CSpaceObject *pTarget,
							   bool *retbSourceDestroyed,
							   bool *retbConsumedItems = NULL) { return false; }
		virtual CWeaponClass *AsWeaponClass (void) { return NULL; }
		virtual int CalcFireSolution (CInstalledDevice *pDevice, CSpaceObject *pSource, CSpaceObject *pTarget) { return -1; }
		virtual int CalcPowerUsed (CInstalledDevice *pDevice, CSpaceObject *pSource) { return 0; }
		virtual bool CanBeDamaged (void) { return true; }
		virtual bool CanBeDisabled (CItemCtx &Ctx) { return (GetPowerRating(Ctx) != 0); }
		virtual bool CanHitFriends (void) { return true; }
		virtual bool CanRotate (CItemCtx &Ctx, int *retiMinFireArc = NULL, int *retiMaxFireArc = NULL) const { return false; }
		virtual void Deplete (CInstalledDevice *pDevice, CSpaceObject *pSource) { }
		virtual bool FindAmmoDataField (const CItem &Ammo, const CString &sField, CString *retsValue) const { return false; }
        virtual ICCItem *FindAmmoItemProperty (CItemCtx &Ctx, const CItem &Ammo, const CString &sProperty) { return CDeviceClass::FindItemProperty(Ctx, sProperty); }
		virtual bool FindDataField (const CString &sField, CString *retsValue) { return false; }
		virtual ICCItem *FindItemProperty (CItemCtx &Ctx, const CString &sName);
		virtual int GetActivateDelay (CInstalledDevice *pDevice, CSpaceObject *pSource) { return 0; }
		virtual int GetAmmoVariant (const CItemType *pItem) const { return -1; }
		virtual int GetCounter (CInstalledDevice *pDevice, CSpaceObject *pSource, CounterTypes *retiType = NULL, int *retiLevel = NULL) { return 0; }
		virtual const DamageDesc *GetDamageDesc (CItemCtx &Ctx) { return NULL; }
		virtual int GetDamageEffectiveness (CSpaceObject *pAttacker, CInstalledDevice *pWeapon) { return 0; }
		virtual DamageTypes GetDamageType (CItemCtx &Ctx, const CItem &Ammo = CItem()) const { return damageGeneric; }
		virtual int GetDefaultFireAngle (CInstalledDevice *pDevice, CSpaceObject *pSource) const { return 0; }
		virtual bool GetDeviceEnhancementDesc (CInstalledDevice *pDevice, CSpaceObject *pSource, CInstalledDevice *pWeapon, SDeviceEnhancementDesc *retDesc) { return false; }
		virtual DWORD GetLinkedFireOptions (CItemCtx &Ctx) { return 0; }
		virtual Metric GetMaxEffectiveRange (CSpaceObject *pSource, CInstalledDevice *pDevice, CSpaceObject *pTarget) { return 0.0; }
		virtual Metric GetMaxRange (CItemCtx &ItemCtx) { return 0.0; }
		virtual int GetPowerOutput (CItemCtx &Ctx) const { return 0; }
		virtual int GetPowerRating (CItemCtx &Ctx) const { return 0; }
		virtual bool GetReferenceDamageAdj (const CItem *pItem, CSpaceObject *pInstalled, int *retiHP, int *retArray) const { return false; }
		virtual bool GetReferenceDamageType (CItemCtx &Ctx, const CItem &Ammo, DamageTypes *retiDamage, CString *retsReference) const { return false; }
		virtual void GetSelectedVariantInfo (CSpaceObject *pSource, 
											 CInstalledDevice *pDevice,
											 CString *retsLabel,
											 int *retiAmmoLeft,
											 CItemType **retpType = NULL) { if (retsLabel) *retsLabel = NULL_STR; if (retiAmmoLeft) *retiAmmoLeft = -1; if (retpType) *retpType = NULL; }
		virtual void GetStatus (CInstalledDevice *pDevice, CSpaceObject *pSource, int *retiStatus, int *retiMaxStatus) { *retiStatus = 0; *retiMaxStatus = 0; }
		virtual int GetValidVariantCount (CSpaceObject *pSource, CInstalledDevice *pDevice) { return 0; }
		virtual int GetWeaponEffectiveness (CSpaceObject *pSource, CInstalledDevice *pDevice, CSpaceObject *pTarget) { return 0; }
		virtual bool IsAmmoWeapon (void) { return false; }
		virtual bool IsAreaWeapon (CSpaceObject *pSource, CInstalledDevice *pDevice) { return false; }
		virtual bool IsAutomatedWeapon (void) { return false; }
		virtual bool IsExternal (void) const { return (m_fExternal ? true : false); }
		virtual bool IsFuelCompatible (CItemCtx &Ctx, const CItem &FuelItem) { return false; }
		virtual bool IsTrackingWeapon (CItemCtx &Ctx) { return false; }
		virtual bool IsVariantSelected (CSpaceObject *pSource, CInstalledDevice *pDevice) { return true; }
		virtual bool IsWeaponAligned (CSpaceObject *pShip, CInstalledDevice *pDevice, CSpaceObject *pTarget, int *retiAimAngle = NULL, int *retiFireAngle = NULL) { return false; }
		virtual bool NeedsAutoTarget (CItemCtx &Ctx, int *retiMinFireArc = NULL, int *retiMaxFireArc = NULL) { return false; }
		virtual CString OnGetReference (CItemCtx &Ctx, const CItem &Ammo = CItem(), DWORD dwFlags = 0) { return NULL_STR; }
		virtual void OnInstall (CInstalledDevice *pDevice, CSpaceObject *pSource, CItemListManipulator &ItemList) { }
		virtual void OnUninstall (CInstalledDevice *pDevice, CSpaceObject *pSource, CItemListManipulator &ItemList) { }
		virtual void Recharge (CInstalledDevice *pDevice, CShip *pShip, int iStatus) { }
		virtual bool RequiresItems (void) const { return false; }
		virtual void Reset (CInstalledDevice *pDevice, CSpaceObject *pSource) { }
		virtual bool SelectFirstVariant (CSpaceObject *pSource, CInstalledDevice *pDevice) { return false; }
		virtual bool SelectNextVariant (CSpaceObject *pSource, CInstalledDevice *pDevice, int iDir = 1) { return false; }
		virtual bool SetCounter (CInstalledDevice *pDevice, CSpaceObject *pSource, CounterTypes iCounter, int iLevel) { return false; }
		virtual bool SetItemProperty (CItemCtx &Ctx, const CString &sName, ICCItem *pValue, CString *retsError);
		virtual bool ShowActivationDelayCounter (CSpaceObject *pSource, CInstalledDevice *pDevice) { return false; }
		virtual void Update (CInstalledDevice *pDevice, 
							 CSpaceObject *pSource, 
							 int iTick,
							 bool *retbSourceDestroyed,
							 bool *retbConsumedItems = NULL) { }
		virtual bool ValidateSelectedVariant (CSpaceObject *pSource, CInstalledDevice *pDevice) { return false; }

		static bool FindAmmoDataField (CItemType *pItem, const CString &sField, CString *retsValue);
		static bool FindWeaponFor (CItemType *pItem, CDeviceClass **retpWeapon = NULL, int *retiVariant = NULL, CWeaponFireDesc **retpDesc = NULL);
		static ItemCategories GetItemCategory (DeviceNames iDev);
		static CString GetLinkedFireOptionString (DWORD dwOptions);
		static ALERROR ParseLinkedFireOptions (SDesignLoadCtx &Ctx, const CString &sDesc, DWORD *retdwOptions);
		static int ParseVariantFromPropertyName (const CString &sName, CString *retsName = NULL);

	protected:
		inline ItemCategories GetDefinedSlotCategory (void) { return m_iSlotCategory; }
		virtual ItemCategories GetImplCategory (void) const = 0;
		ALERROR InitDeviceFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType);

		virtual void OnAccumulateAttributes (CItemCtx &ItemCtx, const CItem &Ammo, TArray<SDisplayAttribute> *retList) { }
		virtual bool OnAccumulateEnhancements (CItemCtx &Device, CInstalledDevice *pTarget, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements) { return false; }
        virtual bool OnAccumulatePerformance (CItemCtx &ItemCtx, SShipPerformanceCtx &Ctx) const { return false; }
		virtual void OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) { }
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) { return NOERROR; }
		virtual CEffectCreator *OnFindEffectCreator (const CString &sUNID) { return NULL; }
		virtual ALERROR OnFinishBind (SDesignLoadCtx &Ctx) { return NOERROR; }
        virtual Metric OnGetScaledCostAdj (CItemCtx &Ctx) const;
		virtual void OnMarkImages (void) { }

	private:
		struct SEnhancerDesc
			{
			CString sType;						//	Type of enhancement
			CItemCriteria Criteria;				//	Items that we enhance
			CItemEnhancement Enhancement;		//	Enhancement confered
			};

		CItemType *m_pItemType;					//	Item for device
		int m_iSlots;							//	Number of device slots required
		ItemCategories m_iSlotCategory;			//	Count as this category (for device slot purposes)

		COverlayTypeRef m_pOverlayType;			//	Associated overlay (may be NULL)

		int m_iMaxHPBonus;						//	Max HP bonus for this device
		TArray<SEnhancerDesc> m_Enhancements;	//	Enhancements confered on other items

		SEventHandlerDesc m_CachedEvents[evtCount];	//	Cached events

		DWORD m_fExternal:1;					//	Device is external
		DWORD m_dwSpare:31;
	};

//	IDeviceGenerator -----------------------------------------------------------

struct SDeviceDesc
	{
	SDeviceDesc (void) :
			iPosAngle(0),
			iPosRadius(0),
			iPosZ(0),
			b3DPosition(false),
			bExternal(false),
			bOmnidirectional(false),
			iMinFireArc(0),
			iMaxFireArc(0),
			bSecondary(false),
			dwLinkedFireOptions(0),
			iSlotBonus(0)
		{ }

	CItem Item;

	int iPosAngle;
	int iPosRadius;
	int iPosZ;
	bool b3DPosition;
	bool bExternal;

	bool bOmnidirectional;
	int iMinFireArc;
	int iMaxFireArc;
	bool bSecondary;

	DWORD dwLinkedFireOptions;

	int iSlotBonus;

	CItemList ExtraItems;
	};

class CDeviceDescList
	{
	public:
		CDeviceDescList (void);
		~CDeviceDescList (void);

		void AddDeviceDesc (const SDeviceDesc &Desc);
		inline int GetCount (void) const { return m_List.GetCount(); }
		inline CDeviceClass *GetDeviceClass (int iIndex) const;
		inline const SDeviceDesc &GetDeviceDesc (int iIndex) const { return m_List[iIndex]; }
		const SDeviceDesc *GetDeviceDescByName (DeviceNames iDev) const;
		CDeviceClass *GetNamedDevice (DeviceNames iDev) const;
		void RemoveAll (void);

	private:
		TArray<SDeviceDesc> m_List;
	};

struct SDeviceGenerateCtx
	{
	SDeviceGenerateCtx (void) :
			iLevel(1),
			pRoot(NULL),
			pResult(NULL)
		{ }

	int iLevel;
	IDeviceGenerator *pRoot;

	CDeviceDescList *pResult;
	};

class IDeviceGenerator
	{
	public:
		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, IDeviceGenerator **retpGenerator);

		virtual ~IDeviceGenerator (void) { }
		virtual void AddDevices (SDeviceGenerateCtx &Ctx) { }
		virtual void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) { }
		virtual IDeviceGenerator *GetGenerator (int iIndex) { return NULL; }
		virtual int GetGeneratorCount (void) { return 0; }
		virtual ALERROR LoadFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) { return NOERROR; }
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) { return NOERROR; }

		virtual bool FindDefaultDesc (DeviceNames iDev, SDeviceDesc *retDesc) { return false; }
		virtual bool FindDefaultDesc (const CItem &Item, SDeviceDesc *retDesc) { return false; }

		static ALERROR InitDeviceDescFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, SDeviceDesc *retDesc);
	};

//  CShipPerformanceDesc ------------------------------------------------------

struct SShipPerformanceCtx
    {
    SShipPerformanceCtx (void) :
            pShip(NULL),
            rSingleArmorFraction(0.0),
            rMaxSpeedBonus(0.0),
            bDriveDamaged(false),
            bHalfSpeed(false),
            CargoDesc(0),
            iMaxCargoSpace(0)
        { }

    CShip *pShip;                           //  Target ship
    Metric rSingleArmorFraction;            //  Fraction of all armor segments represented by 1 segment (= 1/segment-count)

    CRotationDesc RotationDesc;             //  Double precision rotation descriptor

	CReactorDesc ReactorDesc;				//	Reactor descriptor

    CDriveDesc DriveDesc;                   //  Drive descriptor
    Metric rMaxSpeedBonus;                  //  % bonus to speed (+/-). 100.0 = +100%
    bool bDriveDamaged;                     //  If TRUE, cut thrust in half
    bool bHalfSpeed;                        //  If TRUE, ship is running at half speed

    CCargoDesc CargoDesc;                   //  Cargo space descriptor
    int iMaxCargoSpace;                     //  Max cargo space limit imposed by class
                                            //      0 = no limit
    };

class CShipPerformanceDesc
    {
    public:
        inline const CCargoDesc &GetCargoDesc (void) const { return m_CargoDesc; }
        inline const CDriveDesc &GetDriveDesc (void) const { return m_DriveDesc; }
        inline const CReactorDesc &GetReactorDesc (void) const { return m_ReactorDesc; }
        inline const CIntegralRotationDesc &GetRotationDesc (void) const { return m_RotationDesc; }
        void Init (SShipPerformanceCtx &Ctx);

        //  Read-Write versions of accessors

        inline CCargoDesc &GetCargoDesc (void) { return m_CargoDesc; }
        inline CDriveDesc &GetDriveDesc (void) { return m_DriveDesc; }
        inline CReactorDesc &GetReactorDesc (void) { return m_ReactorDesc; }
        inline CIntegralRotationDesc &GetRotationDesc (void) { return m_RotationDesc; }

    private:
        CIntegralRotationDesc m_RotationDesc;
		CReactorDesc m_ReactorDesc;
        CDriveDesc m_DriveDesc;
        CCargoDesc m_CargoDesc;
    };
