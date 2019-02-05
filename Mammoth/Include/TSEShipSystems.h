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

//	Armor ----------------------------------------------------------------------

class CArmorSystem
    {
    public:
        CArmorSystem (void) :
                m_iHealerLeft(0)
            { }

		void AccumulatePerformance (SShipPerformanceCtx &Ctx) const;
		void AccumulatePowerUsed (SUpdateCtx &Ctx, CSpaceObject *pObj, int &iPowerUsed, int &iPowerGenerated);
		int CalcTotalHitPoints (CSpaceObject *pSource, int *retiMaxHP = NULL) const;
        inline int GetHealerLeft (void) const { return m_iHealerLeft; }
		inline CInstalledArmor &GetSegment (int iSeg) { return m_Segments[iSeg]; }
		inline int GetSegmentCount (void) const { return m_Segments.GetCount(); }
        inline int IncHealerLeft (int iInc) { SetHealerLeft(m_iHealerLeft + iInc); return m_iHealerLeft; }
        void Install (CSpaceObject *pObj, const CShipArmorDesc &Desc, bool bInCreate = false);
		bool IsImmune (CSpaceObject *pObj, SpecialDamageTypes iSpecialDamage) const;
        void ReadFromStream (SLoadCtx &Ctx, CSpaceObject *pObj);
		bool RepairAll (CSpaceObject *pSource);
		bool RepairSegment (CSpaceObject *pSource, int iSeg, int iHPToRepair, int *retiHPRepaired = NULL);
        inline void SetHealerLeft (int iValue) { m_iHealerLeft = Max(0, iValue); }
		void SetTotalHitPoints (CSpaceObject *pSource, int iNewHP);
		bool Update (SUpdateCtx &Ctx, CSpaceObject *pSource, int iTick);
        void WriteToStream (IWriteStream *pStream);

    private:
        TArray<CInstalledArmor> m_Segments;         //  Armor segments
        int m_iHealerLeft;                          //  HP of healing left (for bioships)
    };

//	Devices --------------------------------------------------------------------

class CDeviceSystem
	{
	public:
		CDeviceSystem (void);

		void AccumulateEnhancementsToArmor (CSpaceObject *pObj, CInstalledArmor *pArmor, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements);
		void AccumulatePerformance (SShipPerformanceCtx &Ctx) const;
		void AccumulatePowerUsed (SUpdateCtx &Ctx, CSpaceObject *pObj, int &iPowerUsed, int &iPowerGenerated);
		int CalcSlotsInUse (int *retiWeaponSlots, int *retiNonWeapon) const;
		void CleanUp (void);
		CInstalledDevice *FindDevice (const CItem &Item);

		static constexpr DWORD FLAG_VALIDATE_ITEM =	0x00000001;
		static constexpr DWORD FLAG_MATCH_BY_TYPE =	0x00000002;
		int FindDeviceIndex (const CItem &Item, DWORD dwFlags = 0) const;

		int FindFreeSlot (void);
		int FindNamedIndex (const CItem &Item) const;
		int FindNextIndex (CSpaceObject *pObj, int iStart, ItemCategories Category, int iDir = 1) const;
		int FindRandomIndex (bool bEnabledOnly) const;
		bool FindWeaponByItem (const CItem &Item, int *retiIndex = NULL, int *retiVariant = NULL) const;
		inline int GetCount (void) const { return m_Devices.GetCount(); }
		int GetCountByID (const CString &sID) const;
		inline CInstalledDevice &GetDevice (int iIndex) { return m_Devices[iIndex]; }
		inline const CInstalledDevice &GetDevice (int iIndex) const { return m_Devices[iIndex]; }
		inline CInstalledDevice *GetNamedDevice (DeviceNames iDev) { if (m_NamedDevices[iDev] != -1) return &GetDevice(m_NamedDevices[iDev]); else return NULL; }
		inline int GetNamedIndex (DeviceNames iDev) const { return m_NamedDevices[iDev]; }
		bool Init (CSpaceObject *pObj, const CDeviceDescList &Devices, int iMaxDevices = 0);
		bool Install (CSpaceObject *pObj, CItemListManipulator &ItemList, int iDeviceSlot = -1, int iSlotPosIndex = -1, bool bInCreate = false, int *retiDeviceSlot = NULL);
		bool IsSlotAvailable (ItemCategories iItemCat, int *retiSlot = NULL) const;
		void MarkImages (void);
		bool OnDestroyCheck (CSpaceObject *pObj, DestructionTypes iCause, const CDamageSource &Attacker);
        void ReadFromStream (SLoadCtx &Ctx, CSpaceObject *pObj);
		void ReadyFirstMissile (CSpaceObject *pObj);
		void ReadyFirstWeapon (CSpaceObject *pObj);
		void ReadyNextMissile (CSpaceObject *pObj, int iDir = 1);
		void ReadyNextWeapon (CSpaceObject *pObj, int iDir = 1);
		DeviceNames SelectWeapon (CSpaceObject *pObj, int iIndex, int iVariant);
		void SetCursorAtDevice (CItemListManipulator &ItemList, int iIndex) const;
		void SetCursorAtNamedDevice (CItemListManipulator &ItemList, DeviceNames iDev) const;
		bool Uninstall (CSpaceObject *pObj, CItemListManipulator &ItemList, ItemCategories *retiCat = NULL);
        void WriteToStream (IWriteStream *pStream);

	private:
		DeviceNames GetNamedFromDeviceIndex (int iIndex) const;

		TArray<CInstalledDevice> m_Devices;
		int m_NamedDevices[devNamesCount];
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
			fDefault(false),
			fIsAttached(false)
		{
		rcPos.left = 0;
		rcPos.top = 0;
		rcPos.right = 0;
		rcPos.bottom = 0;
		}

	CString sID;							//	Programmatic ID
	CString sName;							//	User-visible name (e.g., "bridge")
	CShipClassRef Class;					//	For attached sections
	ECompartmentTypes iType;				//	Type of compartment
	int iMaxHP;								//	Initial HP (always 0 for attached)
	RECT rcPos;								//	Position and size relative to image

	CString sAttachID;						//	ID of compartment we're attached to (NULL = root object)
	C3DObjectPos AttachPos;					//	Attach position relative to sAttachID

	DWORD fDefault:1;						//	Default compartment (any space not used by another compartment)
	DWORD fIsAttached:1;					//	TRUE if this is an attached section (a separate CSpaceObject)
	};

class CShipInteriorDesc
	{
	public:
		CShipInteriorDesc (void);

		ALERROR BindDesign (SDesignLoadCtx &Ctx);
		void CalcCompartmentPositions (int iScale, TArray<CVector> &Result) const;
		int CalcImageSize (CShipClass *pClass, CVector *retvOrigin = NULL) const;
		void DebugPaint (CG32bitImage &Dest, int x, int y, int iRotation, int iScale) const;
		inline int GetCount (void) const { return m_Compartments.GetCount(); }
		inline const SCompartmentDesc &GetCompartment (int iIndex) const { return m_Compartments[iIndex]; }
		int GetHitPoints (void) const;
		const TArray<int> &GetPaintOrder (void) const { return m_PaintOrder; }
		inline bool HasAttached (void) const { return (m_fHasAttached ? true : false); }
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		inline bool IsEmpty (void) const { return m_Compartments.GetCount() == 0; }
		inline bool IsMultiHull (void) const { return (m_fIsMultiHull ? true : false); }

		static ECompartmentTypes ParseCompartmentType (const CString &sValue);

	private:
		int CalcPaintOrder (int iIndex, const TSortMap<CString, int> &ByID, TArray<int> &PaintOrder) const;

		TArray<SCompartmentDesc> m_Compartments;
		TArray<int> m_PaintOrder;

		DWORD m_fHasAttached:1;
		DWORD m_fIsMultiHull:1;
	};

class CShipInterior
	{
	public:
		void CreateAttached (CShip *pShip, const CShipInteriorDesc &Desc);
		EDamageResults Damage (CShip *pShip, const CShipInteriorDesc &Desc, SDamageCtx &Ctx);
		bool FindAttachedObject (const CShipInteriorDesc &Desc, const CString &sID, CSpaceObject **retpObj) const;
		inline CSpaceObject *GetAttached (int iIndex) const { return m_Compartments[iIndex].pAttached; }
		inline int GetCount (void) const { return m_Compartments.GetCount(); }
		void GetHitPoints (CShip *pShip, const CShipInteriorDesc &Desc, int *retiHP, int *retiMaxHP = NULL) const;
		void Init (const CShipInteriorDesc &Desc);
		inline bool IsEmpty (void) const { return m_Compartments.GetCount() == 0; }
		void OnDestroyed (CShip *pShip, const SDestroyCtx &Ctx);
		void OnNewSystem (CSystem *pSystem, CShip *pShip, const CShipInteriorDesc &Desc);
		void OnPlace (CShip *pShip, const CVector &vOldPos);
		void ReadFromStream (CShip *pShip, const CShipInteriorDesc &Desc, SLoadCtx &Ctx);
		bool RepairHitPoints (CShip *pShip, const CShipInteriorDesc &Desc, int iRepairHP);
		inline void SetAttached (int iIndex, CSpaceObject *pAttached) { m_Compartments[iIndex].pAttached = pAttached; }
		void SetHitPoints (CShip *pShip, const CShipInteriorDesc &Desc, int iHP);
		void WriteToStream (CShip *pShip, IWriteStream *pStream);

	private:
		struct SCompartmentEntry
			{
			SCompartmentEntry (void) :
					iHP(0),
					pAttached(NULL)
				{ }

			int iHP;						//	HP left (always 0 for attached)
			CSpaceObject *pAttached;		//	May be NULL

			//	Temporaries
			bool bHit;						//	TRUE if this compartment got a direct hit
			bool bMarked;					//	Temporary
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

		void CalcAttachPos (CShip *pShip, const CShipInteriorDesc &Desc, int iIndex, CSpaceObject **retpAttachedTo, CVector *retvPos) const;
		void DetachChain (CShip *pShip, CSpaceObject *pBreak);
		bool FindAttachedObject (CSpaceObject *pAttached, int *retiIndex = NULL) const;
		int FindNextCompartmentHit (SHitTestCtx &HitCtx, int xHitPos, int yHitPos);
		bool MarkIfAttached (CShip *pShip, int iSection, CSpaceObject *pBreak);
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
		inline int GetFrameAngle (void) const { return (m_iCount > 0 ? mathRound(360.0 / m_iCount) : 0); }
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

		static int GetFrameIndex (int iCount, int iAngle) { return (InitFacingsData(iCount) ? m_FacingsData[iCount].AngleToFrameIndex[AngleMod(iAngle)] : 0); }
        static int GetRotationAngle (int iCount, int iIndex) { return (InitFacingsData(iCount) ? m_FacingsData[iCount].FrameIndexToAngle[iIndex % iCount] : 0); }

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

		static bool InitFacingsData (int iCount);
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
        inline int GetThrustProperty (void) const { return 2 * m_iThrust; }
        inline DWORD GetUNID (void) const { return m_dwUNID; }
        ALERROR InitFromShipClassXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, DWORD dwUNID, Metric *retrThrustRatio, int *retiMaxSpeed);
        ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, DWORD dwUNID);
		void InitThrustFromXML (SDesignLoadCtx &Ctx, const CString &sValue);
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
	CString sReactorName;							//	Name of reactor
	const CObjectImageArray *pReactorImage = NULL;	//	Image to use
	TArray<SDisplayAttribute> Enhancements;			//	List of enhancements

	int iFuelLevel = 0;								//	Fuel left 0-100

	int iReactorPower = 0;							//	Max power generated by reactor (1/10th MW)
	int iOtherPower = 0;							//	Power generated by devices/armor (1/10th MW)
	int iPowerConsumed = 0;							//	Total power consumed (1/10th MW)

	int iMaxCharges = 0;							//	Max reactor charges
	int iChargesLeft = 0;							//	Charges left

	bool bUsesCharges = false;						//	TRUE if we use charges instead of fuel
	bool bReactorDamaged = false;					//	TRUE if reactor is damaged
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
        ALERROR InitFromShipClassXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, DWORD dwUNID);
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
		static constexpr int DEFAULT_LIFESUPPORT_POWER_USE = 5;

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
		void SetFuelLeft (Metric rFuel);
		inline void SetGraceTimer (int iTime) { m_iReactorGraceTimer = iTime; }
		void SetMaxFuel (Metric rMaxFuel);
		inline void SetOutOfFuel (bool bValue = true) { m_fOutOfFuel = bValue; }
		inline void SetOutOfPower (bool bValue = true) { m_fOutOfPower = bValue; }
		bool UpdateGraceTimer (void);
		void UpdatePowerUse (int iPowerDrained, int iPowerGenerated, Metric rEfficiency, Metric *retrFuelConsumed = NULL);
		void WriteToStream (CSpaceObject *pObj, IWriteStream &Stream) const;

	private:
		Metric m_rFuelLeft;					//	Fuel left
		int m_iPowerDrain;					//	Power consumed last tick (1/10th MW)
		int m_iPowerGenerated;				//	Power generated last tick (1/10th MW)

		int m_iReactorGraceTimer:16;		//	Ticks left to live when no power or no fuel

		DWORD m_fOutOfFuel:1;				//	TRUE if ship is out of fuel
		DWORD m_fOutOfPower:1;				//	TRUE if reactor generating no power
	};

//  CShipPerformanceDesc ------------------------------------------------------

struct SShipPerformanceCtx
    {
    SShipPerformanceCtx (CShipClass *pClassArg) :
			pClass(pClassArg)
        { }

	CShipClass *pClass = NULL;				//	Class (required)
    CShip *pShip = NULL;					//  Target ship (may be NULL, if computing class perf)
    Metric rSingleArmorFraction = 0.0;		//  Fraction of all armor segments represented by 1 segment (= 1/segment-count)
	TArray<CItemCtx> Armor;					//	Armor installed on ship

	CAbilitySet Abilities;					//	Equipment installed

    CRotationDesc RotationDesc;             //  Double precision rotation descriptor

	CReactorDesc ReactorDesc;				//	Reactor descriptor

    CDriveDesc DriveDesc;                   //  Drive descriptor
	Metric rOperatingSpeedAdj = 1.0;		//	Adjustment to speed based on operations (1.0 = normal)
	Metric rArmorSpeedBonus = 0.0;			//	Increase/decrease in speed
	Metric rMaxSpeedLimit = LIGHT_SPEED;	//	Bonuses should not increase speed above this limit
	bool bDriveDamaged = false;				//  If TRUE, cut thrust in half

    CCargoDesc CargoDesc;                   //  Cargo space descriptor
    int iMaxCargoSpace = 0;					//  Max cargo space limit imposed by class
                                            //      0 = no limit

	bool bShieldInterference = false;		//	Meteorsteel (or something) is interfering
    };

class CShipPerformanceDesc
    {
    public:
		CShipPerformanceDesc (void) : 
				m_fInitialized(false),
				m_fShieldInterference(false)
			{ }

		inline const CAbilitySet &GetAbilities (void) const { return m_Abilities; }
        inline const CCargoDesc &GetCargoDesc (void) const { return m_CargoDesc; }
        inline const CDriveDesc &GetDriveDesc (void) const { return m_DriveDesc; }
        inline const CIntegralRotationDesc &GetIntegralRotationDesc (void) const { return m_IntegralRotationDesc; }
        inline const CReactorDesc &GetReactorDesc (void) const { return m_ReactorDesc; }
        inline const CRotationDesc &GetRotationDesc (void) const { return m_RotationDesc; }
		inline bool HasShieldInterference (void) const { return (m_fShieldInterference ? true : false); }
        void Init (SShipPerformanceCtx &Ctx);
		inline bool IsEmpty (void) const { return (m_fInitialized ? false : true); }

        //  Read-Write versions of accessors

		inline CAbilitySet &GetAbilities (void) { return m_Abilities; }
        inline CCargoDesc &GetCargoDesc (void) { return m_CargoDesc; }
        inline CDriveDesc &GetDriveDesc (void) { return m_DriveDesc; }
        inline CReactorDesc &GetReactorDesc (void) { return m_ReactorDesc; }
        inline CIntegralRotationDesc &GetIntegralRotationDesc (void) { return m_IntegralRotationDesc; }

		inline static const CShipPerformanceDesc &Null (void) { return m_Null; }

    private:
		CRotationDesc m_RotationDesc;
        CIntegralRotationDesc m_IntegralRotationDesc;
		CReactorDesc m_ReactorDesc;
        CDriveDesc m_DriveDesc;
        CCargoDesc m_CargoDesc;
		CAbilitySet m_Abilities;

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
