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
		void Init (int iFrameCount, Metric rMaxRotation = 360.0, Metric rAccel = 1.0, Metric rAccelStop = 1.0);

        static int GetRotationAngle (int iCount, int iIndex) { return ((iCount > 0 && iCount <= 360 && m_FacingsData[iCount].bInitialized) ? m_FacingsData[iCount].FrameIndexToAngle[iIndex % iCount] : 0); }

    private:
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
		Metric GetRotationSpeedDegrees (const CIntegralRotationDesc &Desc) const;
		void Init (const CIntegralRotationDesc &Desc, int iRotationAngle = -1);
		inline bool IsPointingTo (const CIntegralRotationDesc &Desc, int iAngle) const { return (GetManeuverToFace(Desc, iAngle) == NoRotation); }
		void ReadFromStream (SLoadCtx &Ctx, const CIntegralRotationDesc &Desc);
		void SetRotationAngle (const CIntegralRotationDesc &Desc, int iAngle);
		void SetRotationSpeedDegrees (const CIntegralRotationDesc &Desc, Metric rDegreesPerTick);
		void Update (const CIntegralRotationDesc &Desc, EManeuverTypes iManeuver);
		void WriteToStream (IWriteStream *pStream) const;

	private:
		inline int GetFrameIndex (int iFrame) const { return (iFrame / CIntegralRotationDesc::ROTATION_FRACTION); }

		int m_iRotationFrame;				//	Current rotation (in 1/1000ths of a rotation)
		int m_iRotationSpeed;				//	Current rotation speed (+ clockwise; - counterclockwise; in 1/1000ths)
		EManeuverTypes m_iLastManeuver;		//	Maneuver on last update
	};

//	Equipment (Abilities) ------------------------------------------------------
//
//	See AbilityTable in ConstantsUtilities.cpp

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
	ablFriendlyFireLock =		7,

	ablCount =					8,
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
        CCargoDesc (int iCargoSpace = 0) :
				m_bUninitialized(iCargoSpace == 0),
                m_iCargoSpace(iCargoSpace)
            { }

        inline int GetCargoSpace (void) const { return m_iCargoSpace; }
        ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
        void Interpolate (const CCargoDesc &From, const CCargoDesc &To, Metric rInterpolate = 0.5);
        inline bool IsEmpty (void) const { return m_bUninitialized; }
        inline void SetCargoSpace (int iCargoSpace) { m_iCargoSpace = iCargoSpace; m_bUninitialized = false; }
		void ValidateCargoSpace (int iMaxCargoSpace);

    private:
        int m_iCargoSpace;                  //  Cargo space in tons

		bool m_bUninitialized;
    };

//  Drive ----------------------------------------------------------------------

class CDriveDesc
	{
    public:
        CDriveDesc (void);

        void Add (const CDriveDesc &Src);
		Metric AddMaxSpeed (Metric rChange);
		Metric AdjMaxSpeed (Metric rAdj);
        int AdjPowerUse (Metric rAdj);
        int AdjThrust (Metric rAdj);
		inline Metric GetMaxSpeed (void) const { return m_rMaxSpeed; }
		inline int GetMaxSpeedFrac (void) const { return (m_iMaxSpeedLimit != -1 ? m_iMaxSpeedLimit : mathRound(100.0 * m_rMaxSpeed / LIGHT_SPEED)); }
		inline int GetMaxSpeedInc (void) const { return m_iMaxSpeedInc; }
		inline int GetMaxSpeedLimit (void) const { return m_iMaxSpeedLimit; }
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

		static int CalcThrust (Metric rThrustRatio, Metric rMassInTons);
		static Metric CalcThrustRatio (int iThrust, Metric rMassInTons);

    private:
	    DWORD m_dwUNID;						//	UNID source (either ship class or device)
		int m_iMaxSpeedInc;					//	Increase in max speed (0 = always increase to m_iMaxSpeedLimit)
		int m_iMaxSpeedLimit;				//	Do not increase above this limit (-1 = no limit)
	    int m_iThrust;						//	Thrust (GigaNewtons--gasp!)
	    int m_iPowerUse;					//	Power used while thrusting (1/10 megawatt)

		Metric m_rMaxSpeed;					//	Computed max speed (Km/sec)

		DWORD m_fInertialess:1;				//	Inertialess drive
	    DWORD m_dwSpare:31;
	};

//  Reactor --------------------------------------------------------------------

struct SReactorStats
	{
	SReactorStats (void) :
			pReactorImage(NULL),
			iFuelLevel(0),
			iReactorPower(0),
			iOtherPower(0),
			iPowerConsumed(0),
			iMaxCharges(0),
			iChargesLeft(0),
			bReactorDamaged(false),
			bUsesCharges(false)
		{ }

	CString sReactorName;					//	Name of reactor
	const CObjectImageArray *pReactorImage;	//	Image to use

	int iFuelLevel;							//	Fuel left 0-100

	int iReactorPower;						//	Max power generated by reactor (1/10th MW)
	int iOtherPower;						//	Power generated by devices/armor (1/10th MW)
	int iPowerConsumed;						//	Total power consumed (1/10th MW)

	int iMaxCharges;						//	Max reactor charges
	int iChargesLeft;						//	Charges left

	bool bUsesCharges;						//	TRUE if we use charges instead of fuel
	bool bReactorDamaged;					//	TRUE if reactor is damaged
	};

class CReactorDesc
	{
    public:
		enum EFuelUseTypes
			{
			fuelNone,

			fuelConsume,						//	Normal fuel consumption
			fuelDrain,							//	Fuel drained/lost by some process
			};

        struct SStdStats
            {
            int iMaxPower;                  //  Max power (1/10 MW)
            Metric rFuelDensity;            //  Std fuel rods per 100 kg
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

class CPowerConsumption
	{
	public:
		CPowerConsumption (void) :
				m_rFuelLeft(0.0),
				m_iPowerDrain(0),
				m_iPowerGenerated(0),
				m_iReactorGraceTimer(0),
				m_fOutOfFuel(false),
				m_fOutOfPower(false)
			{ }

		Metric ConsumeFuel (Metric rFuel, CReactorDesc::EFuelUseTypes iUse);
		inline Metric GetFuelLeft (void) const { return (m_fOutOfFuel ? 0.0 : m_rFuelLeft); }
		inline int GetGraceTimer (void) const { return m_iReactorGraceTimer; }
		inline int GetPowerConsumed (void) const { return m_iPowerDrain; }
		inline int GetPowerGenerated (void) const { return m_iPowerGenerated; }
		inline int GetPowerNeeded (void) { return Max(0, (m_iPowerDrain - m_iPowerGenerated)); }
		inline bool IsOutOfFuel (void) const { return m_fOutOfFuel; }
		inline bool IsOutOfPower (void) const { return m_fOutOfPower; }
		void ReadFromStream (SLoadCtx &Ctx);
		void Refuel (Metric rFuel, Metric rMaxFuel);
		inline void SetFuelLeft (Metric rFuel) { m_rFuelLeft = rFuel; }
		inline void SetGraceTimer (int iTime) { m_iReactorGraceTimer = iTime; }
		void SetMaxFuel (Metric rMaxFuel);
		inline void SetOutOfFuel (bool bValue = true) { m_fOutOfFuel = bValue; }
		inline void SetOutOfPower (bool bValue = true) { m_fOutOfPower = bValue; }
		bool UpdateGraceTimer (void);
		void UpdatePowerUse (int iPowerDrained, int iPowerGenerated, Metric rEfficiency);
		void WriteToStream (CSpaceObject *pObj, IWriteStream &Stream) const;

	private:
		Metric m_rFuelLeft;					//	Fuel left
		int m_iPowerDrain;					//	Power consumed last tick (1/10th MW)
		int m_iPowerGenerated;				//	Power generated last tick (1/10th MW)

		int m_iReactorGraceTimer:16;		//	Ticks left to live when no power or no fuel

		DWORD m_fOutOfFuel:1;				//	TRUE if ship is out of fuel
		DWORD m_fOutOfPower:1;				//	TRUE if reactor generating no power
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

//  CShipPerformanceDesc ------------------------------------------------------

struct SShipPerformanceCtx
    {
    SShipPerformanceCtx (CShipClass *pClassArg) :
			pClass(pClassArg),
            pShip(NULL),
            rSingleArmorFraction(0.0),
			iArmorMass(0),
            rOperatingSpeedAdj(1.0),
			rArmorSpeedBonus(0.0),
			rMaxSpeedLimit(LIGHT_SPEED),
            bDriveDamaged(false),
            CargoDesc(0),
            iMaxCargoSpace(0),
			bShieldInterference(false)
        { }

	CShipClass *pClass;						//	Class (required)
    CShip *pShip;                           //  Target ship (may be NULL, if computing class perf)
    Metric rSingleArmorFraction;            //  Fraction of all armor segments represented by 1 segment (= 1/segment-count)
	int iArmorMass;							//	Total mass of all armor segments (kg)

    CRotationDesc RotationDesc;             //  Double precision rotation descriptor

	CReactorDesc ReactorDesc;				//	Reactor descriptor

    CDriveDesc DriveDesc;                   //  Drive descriptor
	Metric rOperatingSpeedAdj;				//	Adjustment to speed based on operations (1.0 = normal)
	Metric rArmorSpeedBonus;				//	Increase/decrease in speed
	Metric rMaxSpeedLimit;					//	Bonuses should not increase speed above this limit
	bool bDriveDamaged;                     //  If TRUE, cut thrust in half

    CCargoDesc CargoDesc;                   //  Cargo space descriptor
    int iMaxCargoSpace;                     //  Max cargo space limit imposed by class
                                            //      0 = no limit

	bool bShieldInterference;				//	Meteorsteel (or something) is interfering
    };

class CShipPerformanceDesc
    {
    public:
		CShipPerformanceDesc (void) : 
				m_fInitialized(false),
				m_fShieldInterference(false)
			{ }

        inline const CCargoDesc &GetCargoDesc (void) const { return m_CargoDesc; }
        inline const CDriveDesc &GetDriveDesc (void) const { return m_DriveDesc; }
        inline const CReactorDesc &GetReactorDesc (void) const { return m_ReactorDesc; }
        inline const CIntegralRotationDesc &GetRotationDesc (void) const { return m_RotationDesc; }
		inline bool HasShieldInterference (void) const { return (m_fShieldInterference ? true : false); }
        void Init (SShipPerformanceCtx &Ctx);
		inline bool IsEmpty (void) const { return (m_fInitialized ? false : true); }

        //  Read-Write versions of accessors

        inline CCargoDesc &GetCargoDesc (void) { return m_CargoDesc; }
        inline CDriveDesc &GetDriveDesc (void) { return m_DriveDesc; }
        inline CReactorDesc &GetReactorDesc (void) { return m_ReactorDesc; }
        inline CIntegralRotationDesc &GetRotationDesc (void) { return m_RotationDesc; }

		inline static const CShipPerformanceDesc &Null (void) { return m_Null; }

    private:
        CIntegralRotationDesc m_RotationDesc;
		CReactorDesc m_ReactorDesc;
        CDriveDesc m_DriveDesc;
        CCargoDesc m_CargoDesc;

		DWORD m_fInitialized:1;				//	TRUE if Init called
		DWORD m_fShieldInterference:1;		//	TRUE if energy shields are suppressed (e.g., by meteorsteel)
		DWORD m_fSpare3:1;
		DWORD m_fSpare4:1;
		DWORD m_fSpare5:1;
		DWORD m_fSpare6:1;
		DWORD m_fSpare7:1;
		DWORD m_fSpare8:1;

		DWORD dwSpare:24;

		static CShipPerformanceDesc m_Null;
    };
