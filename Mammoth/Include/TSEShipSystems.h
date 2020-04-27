//	TSEShipSystems.h
//
//	Defines classes and interfaces for ships
//	Copyright (c) 2016 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

#include <iterator>

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

//	Armor ----------------------------------------------------------------------

class CArmorSystem
    {
    public:
		class iterator
			{
			public:
				using iterator_category = std::forward_iterator_tag;

				iterator (void) { }
				iterator (CInstalledArmor *pPos, CInstalledArmor *pEnd) :
						m_pPos(pPos),
						m_pEnd(pEnd)
					{ }

				iterator &operator= (const iterator &Src) { m_pPos = Src.m_pPos; return *this; }
				friend bool operator== (const iterator &lhs, const iterator &rhs) { return lhs.m_pPos == rhs.m_pPos; }
				friend bool operator!= (const iterator &lhs, const iterator &rhs) { return !(lhs == rhs); }

				iterator &operator++ () { m_pPos++; return *this; }
				iterator operator++ (int) {	iterator Old = *this; ++(*this); return Old; }

				CInstalledArmor &operator* () const { return *m_pPos; }
				CInstalledArmor *operator-> () const { return m_pPos; }

				friend void swap (iterator &lhs, iterator &rhs)	{ Swap(lhs.m_pPos, rhs.m_pPos);	Swap(lhs.m_pEnd, rhs.m_pEnd); }

			private:
				CInstalledArmor *m_pPos = NULL;
				CInstalledArmor *m_pEnd = NULL;
			};

		class const_iterator
			{
			public:
				using iterator_category = std::forward_iterator_tag;

				const_iterator (void) { }
				const_iterator (const CInstalledArmor *pPos, const CInstalledArmor *pEnd) :
						m_pPos(pPos),
						m_pEnd(pEnd)
					{ }

				const_iterator &operator= (const const_iterator &Src) { m_pPos = Src.m_pPos; return *this; }
				friend bool operator== (const const_iterator &lhs, const const_iterator &rhs) { return lhs.m_pPos == rhs.m_pPos; }
				friend bool operator!= (const const_iterator &lhs, const const_iterator &rhs) { return !(lhs == rhs); }

				const_iterator &operator++ () { m_pPos++; return *this; }
				const_iterator operator++ (int) { const_iterator Old = *this; ++(*this); return Old; }

				const CInstalledArmor &operator* () const { return *m_pPos; }
				const CInstalledArmor *operator-> () const { return m_pPos; }

				friend void swap (const_iterator &lhs, const_iterator &rhs)	{ Swap(lhs.m_pPos, rhs.m_pPos);	Swap(lhs.m_pEnd, rhs.m_pEnd); }

			private:
				const CInstalledArmor *m_pPos = NULL;
				const CInstalledArmor *m_pEnd = NULL;
			};

		const_iterator begin (void) const {	return cbegin(); }
		const_iterator cbegin (void) const { return (m_Segments.GetCount() == 0 ? const_iterator() : const_iterator(&m_Segments[0], &m_Segments[0] + m_Segments.GetCount())); }
		const_iterator end (void) const { return cend(); }
		const_iterator cend (void) const { return (m_Segments.GetCount() == 0 ? const_iterator() : const_iterator(&m_Segments[0] + m_Segments.GetCount(), &m_Segments[0] + m_Segments.GetCount())); }

		iterator begin (void) {	return (m_Segments.GetCount() == 0 ? iterator() : iterator(&m_Segments[0], &m_Segments[0] + m_Segments.GetCount())); }
		iterator end (void) { return (m_Segments.GetCount() == 0 ? iterator() : iterator(&m_Segments[0] + m_Segments.GetCount(), &m_Segments[0] + m_Segments.GetCount())); }

		static CArmorSystem m_Null;

		void AccumulatePerformance (SShipPerformanceCtx &Ctx) const;
		void AccumulatePowerUsed (SUpdateCtx &Ctx, CSpaceObject *pObj, int &iPowerUsed, int &iPowerGenerated);
		int CalcTotalHitPoints (int *retiMaxHP = NULL) const;
        int GetHealerLeft (void) const { return m_iHealerLeft; }
		CInstalledArmor &GetSegment (int iSeg) { return m_Segments[iSeg]; }
		int GetSegmentCount (void) const { return m_Segments.GetCount(); }
        int IncHealerLeft (int iInc) { SetHealerLeft(m_iHealerLeft + iInc); return m_iHealerLeft; }
        void Install (CSpaceObject &Source, const CShipArmorDesc &Desc, bool bInCreate = false);
		bool IsImmune (SpecialDamageTypes iSpecialDamage) const;
        void ReadFromStream (SLoadCtx &Ctx, CSpaceObject &Source);
		bool RepairAll (CSpaceObject *pSource);
		bool RepairSegment (CSpaceObject *pSource, int iSeg, int iHPToRepair, int *retiHPRepaired = NULL);
        void SetHealerLeft (int iValue) { m_iHealerLeft = Max(0, iValue); }
		void SetTotalHitPoints (CSpaceObject *pSource, int iNewHP);
		bool Update (SUpdateCtx &Ctx, CSpaceObject *pSource, int iTick);
        void WriteToStream (IWriteStream *pStream) const;

    private:
        TArray<CInstalledArmor> m_Segments;         //  Armor segments
        int m_iHealerLeft = 0;						//  HP of healing left (for bioships)
    };

//	Devices --------------------------------------------------------------------

class CDeviceSystem
	{
	public:
		static constexpr DWORD FLAG_NO_NAMED_DEVICES = 0x00000001;
		CDeviceSystem (DWORD dwFlags = 0);

		void AccumulateEnhancementsToArmor (CSpaceObject *pObj, CInstalledArmor *pArmor, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements);
		void AccumulatePerformance (SShipPerformanceCtx &Ctx) const;
		void AccumulatePowerUsed (SUpdateCtx &Ctx, CSpaceObject *pObj, int &iPowerUsed, int &iPowerGenerated);
		int CalcSlotsInUse (int *retiWeaponSlots, int *retiNonWeapon, int *retiLauncherSlots) const;
		void CleanUp (void);
		CInstalledDevice *FindDevice (const CItem &Item);

		static constexpr DWORD FLAG_VALIDATE_ITEM =	0x00000001;
		static constexpr DWORD FLAG_MATCH_BY_TYPE =	0x00000002;
		int FindDeviceIndex (const CItem &Item, DWORD dwFlags = 0) const;

		int FindFreeSlot (void);
		int FindNamedIndex (const CItem &Item) const;
		int FindNextIndex (CSpaceObject *pObj, int iStart, ItemCategories Category, int iDir = 1, bool switchWeapons = false) const;
		int FindRandomIndex (bool bEnabledOnly) const;
		bool FindWeapon (int *retiIndex = NULL) const;
		bool FindWeaponByItem (const CItem &Item, int *retiIndex = NULL, int *retiVariant = NULL) const;
		void FinishInstall (void);
		int GetCount (void) const { return m_Devices.GetCount(); }
		int GetCountByID (const CString &sID) const;
		CInstalledDevice &GetDevice (int iIndex) { return m_Devices[iIndex]; }
		CDeviceItem GetDeviceItem (int iIndex) const { if (!m_Devices[iIndex].IsEmpty()) return m_Devices[iIndex].GetItem()->AsDeviceItem(); else return CItem().AsDeviceItem(); }
		const CInstalledDevice &GetDevice (int iIndex) const { return m_Devices[iIndex]; }
		const CInstalledDevice *GetNamedDevice (DeviceNames iDev) const { if (HasNamedDevices() && m_NamedDevices[iDev] != -1) return &GetDevice(m_NamedDevices[iDev]); else return NULL; }
		CInstalledDevice *GetNamedDevice (DeviceNames iDev) { if (HasNamedDevices() && m_NamedDevices[iDev] != -1) return &GetDevice(m_NamedDevices[iDev]); else return NULL; }
		CDeviceItem GetNamedDeviceItem (DeviceNames iDev) const { if (HasNamedDevices() && m_NamedDevices[iDev] != -1) return GetDevice(m_NamedDevices[iDev]).GetItem()->AsDeviceItem(); else return CItem().AsDeviceItem(); }
		int GetNamedIndex (DeviceNames iDev) const { return (HasNamedDevices() ? m_NamedDevices[iDev] : -1); }
		DWORD GetTargetTypes (void) const;
		bool HasNamedDevices (void) const { return (m_NamedDevices.GetCount() > 0); }
		bool HasShieldsUp (void) const;
		bool Init (CSpaceObject *pObj, const CDeviceDescList &Devices, int iMaxDevices = 0);
		bool Install (CSpaceObject *pObj, CItemListManipulator &ItemList, int iDeviceSlot = -1, int iSlotPosIndex = -1, bool bInCreate = false, int *retiDeviceSlot = NULL);
		bool IsEmpty (void) const { return (m_Devices.GetCount() == 0); }
		bool IsSlotAvailable (ItemCategories iItemCat, int *retiSlot = NULL) const;
		bool IsWeaponRepeating (DeviceNames iDev = devNone) const;
		void MarkImages (void);
		bool OnDestroyCheck (CSpaceObject *pObj, DestructionTypes iCause, const CDamageSource &Attacker);
		void OnSubordinateDestroyed (CSpaceObject &SubordinateObj, const CString &sSubordinateID);
        void ReadFromStream (SLoadCtx &Ctx, CSpaceObject *pObj);
		void ReadyFirstMissile (CSpaceObject *pObj);
		void ReadyFirstWeapon (CSpaceObject *pObj);
		void ReadyNextLauncher (CSpaceObject *pObj, int iDir = 1);
		void ReadyNextMissile (CSpaceObject * pObj, int iDir = 1, bool bUsedLastAmmo = false);
		void ReadyNextWeapon (CSpaceObject *pObj, int iDir = 1);
		DeviceNames SelectWeapon (CSpaceObject *pObj, int iIndex, int iVariant);
		void SetCursorAtDevice (CItemListManipulator &ItemList, int iIndex) const;
		void SetCursorAtNamedDevice (CItemListManipulator &ItemList, DeviceNames iDev) const;
		void SetSecondary (bool bValue = true);
		bool Uninstall (CSpaceObject *pObj, CItemListManipulator &ItemList, ItemCategories *retiCat = NULL);
        void WriteToStream (IWriteStream *pStream);

		class iterator
			{
			public:
				using iterator_category = std::forward_iterator_tag;

				iterator (void) { }
				iterator (CInstalledDevice *pPos, CInstalledDevice *pEnd);

				iterator &operator= (const iterator &Src) { m_pPos = Src.m_pPos; return *this; }
				friend bool operator== (const iterator &lhs, const iterator &rhs) { return lhs.m_pPos == rhs.m_pPos; }
				friend bool operator!= (const iterator &lhs, const iterator &rhs) { return !(lhs == rhs); }

				iterator &operator++ ();
				iterator operator++ (int) {	iterator Old = *this; ++(*this); return Old; }

				CInstalledDevice &operator* () const { return *m_pPos; }
				CInstalledDevice *operator-> () const { return m_pPos; }

				friend void swap (iterator &lhs, iterator &rhs)	{ Swap(lhs.m_pPos, rhs.m_pPos);	Swap(lhs.m_pEnd, rhs.m_pEnd); }

			private:
				CInstalledDevice *m_pPos = NULL;
				CInstalledDevice *m_pEnd = NULL;
			};

		class const_iterator
			{
			public:
				using iterator_category = std::forward_iterator_tag;

				const_iterator (void) { }
				const_iterator (const CInstalledDevice *pPos, const CInstalledDevice *pEnd);

				const_iterator &operator= (const const_iterator &Src) { m_pPos = Src.m_pPos; return *this; }
				friend bool operator== (const const_iterator &lhs, const const_iterator &rhs) { return lhs.m_pPos == rhs.m_pPos; }
				friend bool operator!= (const const_iterator &lhs, const const_iterator &rhs) { return !(lhs == rhs); }

				const_iterator &operator++ ();
				const_iterator operator++ (int) { const_iterator Old = *this; ++(*this); return Old; }

				const CInstalledDevice &operator* () const { return *m_pPos; }
				const CInstalledDevice *operator-> () const { return m_pPos; }

				friend void swap (const_iterator &lhs, const_iterator &rhs)	{ Swap(lhs.m_pPos, rhs.m_pPos);	Swap(lhs.m_pEnd, rhs.m_pEnd); }

			private:
				const CInstalledDevice *m_pPos = NULL;
				const CInstalledDevice *m_pEnd = NULL;
			};

		const_iterator begin (void) const {	return cbegin(); }
		const_iterator cbegin (void) const { return (m_Devices.GetCount() == 0 ? const_iterator() : const_iterator(&m_Devices[0], &m_Devices[0] + m_Devices.GetCount())); }
		const_iterator end (void) const { return cend(); }
		const_iterator cend (void) const { return (m_Devices.GetCount() == 0 ? const_iterator() : const_iterator(&m_Devices[0] + m_Devices.GetCount(), &m_Devices[0] + m_Devices.GetCount())); }

		iterator begin (void) {	return (m_Devices.GetCount() == 0 ? iterator() : iterator(&m_Devices[0], &m_Devices[0] + m_Devices.GetCount())); }
		iterator end (void) { return (m_Devices.GetCount() == 0 ? iterator() : iterator(&m_Devices[0] + m_Devices.GetCount(), &m_Devices[0] + m_Devices.GetCount())); }

		static CDeviceSystem m_Null;

	private:
		DeviceNames GetNamedFromDeviceIndex (int iIndex) const;

		TArray<CInstalledDevice> m_Devices;
		TArray<int> m_NamedDevices;
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
		int GetCount (void) const { return m_Compartments.GetCount(); }
		const SCompartmentDesc &GetCompartment (int iIndex) const { return m_Compartments[iIndex]; }
		int GetHitPoints (void) const;
		const TArray<int> &GetPaintOrder (void) const { return m_PaintOrder; }
		bool HasAttached (void) const { return (m_fHasAttached ? true : false); }
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		bool IsEmpty (void) const { return m_Compartments.GetCount() == 0; }
		bool IsMultiHull (void) const { return (m_fIsMultiHull ? true : false); }

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
		CSpaceObject *GetAttached (int iIndex) const { return m_Compartments[iIndex].pAttached; }
		int GetCount (void) const { return m_Compartments.GetCount(); }
		void GetHitPoints (const CShip &Ship, const CShipInteriorDesc &Desc, int *retiHP, int *retiMaxHP = NULL) const;
		void Init (const CShipInteriorDesc &Desc);
		bool IsEmpty (void) const { return m_Compartments.GetCount() == 0; }
		void OnDestroyed (CShip *pShip, const SDestroyCtx &Ctx);
		void OnNewSystem (CSystem *pSystem, CShip *pShip, const CShipInteriorDesc &Desc);
		void OnPlace (CShip *pShip, const CVector &vOldPos);
		void ReadFromStream (CShip *pShip, const CShipInteriorDesc &Desc, SLoadCtx &Ctx);
		bool RepairHitPoints (CShip *pShip, const CShipInteriorDesc &Desc, int iRepairHP);
		void SetAttached (int iIndex, CSpaceObject *pAttached) { m_Compartments[iIndex].pAttached = pAttached; }
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
		int GetFrameCount (void) const { return m_iCount; }
        Metric GetMaxRotationPerTick (void) const { return m_rDegreesPerTick; }
        Metric GetRotationAccelPerTick (void) const { return m_rAccelPerTick; }
        Metric GetRotationAccelStopPerTick (void) const { return m_rAccelPerTickStop; }
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
        explicit CIntegralRotationDesc (const CRotationDesc &Desc) { InitFromDesc(Desc); }

        int AlignToRotationAngle (int iAngle) const { return GetRotationAngle(GetFrameIndex(iAngle)); }
		int CalcFinalRotationFrame (int iRotationFrame, int iRotationSpeed) const;
		int GetFrameAngle (void) const { return (m_iCount > 0 ? mathRound(360.0 / m_iCount) : 0); }
		int GetFrameCount (void) const { return m_iCount; }
		int GetFrameIndex (int iAngle) const { return (m_iCount > 0 ? (m_FacingsData[m_iCount].AngleToFrameIndex[AngleMod(iAngle)]) : 0); }
		int GetManeuverDelay (void) const;
		Metric GetManeuverRatio (void) const { return (Metric)m_iMaxRotationRate / ROTATION_FRACTION; }
		int GetMaxRotationSpeed (void) const { return m_iMaxRotationRate; }
		Metric GetMaxRotationSpeedDegrees (void) const;
		int GetMaxRotationTimeTicks (void) const { Metric rSpeed = GetMaxRotationSpeedDegrees(); return (rSpeed > 0.0 ? (int)(360.0 / rSpeed) : 0); }
		int GetRotationAccel (void) const { return m_iRotationAccel; }
		int GetRotationAccelStop (void) const { return m_iRotationAccelStop; }
		int GetRotationAngle (int iIndex) const { return (m_iCount > 0 ? m_FacingsData[m_iCount].FrameIndexToAngle[iIndex % m_iCount] : 0); }
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

		int CalcFinalRotationFrame (const CIntegralRotationDesc &Desc) const { return Desc.CalcFinalRotationFrame(m_iRotationFrame, m_iRotationSpeed); }
		int GetFrameIndex (void) const { return GetFrameIndex(m_iRotationFrame); }
		EManeuverTypes GetLastManeuver (void) const { return m_iLastManeuver; }
		EManeuverTypes GetManeuverToFace (const CIntegralRotationDesc &Desc, int iAngle) const;
		int GetRotationAngle (const CIntegralRotationDesc &Desc) const;
		Metric GetRotationSpeedDegrees (const CIntegralRotationDesc &Desc) const;
		void Init (const CIntegralRotationDesc &Desc, int iRotationAngle = -1);
		bool IsPointingTo (const CIntegralRotationDesc &Desc, int iAngle) const { return (GetManeuverToFace(Desc, iAngle) == NoRotation); }
		void ReadFromStream (SLoadCtx &Ctx, const CIntegralRotationDesc &Desc);
		void SetRotationAngle (const CIntegralRotationDesc &Desc, int iAngle);
		void SetRotationSpeedDegrees (const CIntegralRotationDesc &Desc, Metric rDegreesPerTick);
		void Update (const CIntegralRotationDesc &Desc, EManeuverTypes iManeuver);
		void WriteToStream (IWriteStream *pStream) const;

	private:
		int GetFrameIndex (int iFrame) const { return (iFrame / CIntegralRotationDesc::ROTATION_FRACTION); }

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

        int GetCargoSpace (void) const { return m_iCargoSpace; }
        ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
        void Interpolate (const CCargoDesc &From, const CCargoDesc &To, Metric rInterpolate = 0.5);
        bool IsEmpty (void) const { return m_bUninitialized; }
        void SetCargoSpace (int iCargoSpace) { m_iCargoSpace = iCargoSpace; m_bUninitialized = false; }
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
		Metric GetMaxSpeed (void) const { return m_rMaxSpeed; }
		int GetMaxSpeedFrac (void) const { return (m_iMaxSpeedLimit != -1 ? m_iMaxSpeedLimit : mathRound(100.0 * m_rMaxSpeed / LIGHT_SPEED)); }
		int GetMaxSpeedInc (void) const { return m_iMaxSpeedInc; }
		int GetMaxSpeedLimit (void) const { return m_iMaxSpeedLimit; }
        int GetPowerUse (void) const { return m_iPowerUse; }
        int GetThrust (void) const { return m_iThrust; }
        int GetThrustProperty (void) const { return 2 * m_iThrust; }
        DWORD GetUNID (void) const { return m_dwUNID; }
        ALERROR InitFromShipClassXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, DWORD dwUNID, Metric *retrThrustRatio, int *retiMaxSpeed);
        ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, DWORD dwUNID);
		void InitThrustFromXML (SDesignLoadCtx &Ctx, const CString &sValue);
        void Interpolate (const CDriveDesc &From, const CDriveDesc &To, Metric rInterpolate = 0.5);
        bool IsInertialess (void) const { return (m_fInertialess ? true : false); }
        void SetInertialess (bool bValue = true) { m_fInertialess = bValue; }
        void SetMaxSpeed (Metric rSpeed) { m_rMaxSpeed = rSpeed; }
        void SetPowerUse (int iPowerUse) { m_iPowerUse = iPowerUse; }
        void SetThrust (int iThrust) { m_iThrust = iThrust; }
        void SetUNID (DWORD dwUNID) { m_dwUNID = dwUNID; }

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

	    ~CReactorDesc (void) { CleanUp(); }
        CReactorDesc &operator= (const CReactorDesc &Src) { CleanUp(); Copy(Src); return *this; }

        int AdjMaxPower (Metric rAdj);
        Metric AdjEfficiency (Metric rAdj);
        bool FindDataField (const CString &sField, CString *retsValue) const;
        ICCItem *FindProperty (const CString &sProperty) const;
        Metric GetEfficiency (void) const { return m_rPowerPerFuelUnit; }
        int GetEfficiencyBonus (void) const;
        Metric GetFuelCapacity (void) const { return m_rMaxFuel; }
        CString GetFuelCriteriaString (void) const;
        void GetFuelLevel (int *retiMin, int *retiMax) const;
        int GetMaxPower (void) const { return m_iMaxPower; }
        ALERROR InitFromShipClassXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, DWORD dwUNID);
        ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, DWORD dwUNID, bool bShipClass = false);
        ALERROR InitScaled (SDesignLoadCtx &Ctx, const CReactorDesc &Src, int iBaseLevel, int iScaledLevel);
        bool IsFuelCompatible (const CItem &FuelItem) const;
		void SetMaxPower (int iPower) { m_iMaxPower = iPower; }
		bool UsesFuel (void) const { return (m_fNoFuel ? false : true); }

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
		Metric GetFuelLeft (void) const { return (m_fOutOfFuel ? 0.0 : m_rFuelLeft); }
		int GetGraceTimer (void) const { return m_iReactorGraceTimer; }
		int GetPowerConsumed (void) const { return m_iPowerDrain; }
		int GetPowerGenerated (void) const { return m_iPowerGenerated; }
		int GetPowerNeeded (void) { return Max(0, (m_iPowerDrain - m_iPowerGenerated)); }
		bool IsOutOfFuel (void) const { return m_fOutOfFuel; }
		bool IsOutOfPower (void) const { return m_fOutOfPower; }
		void ReadFromStream (SLoadCtx &Ctx);
		void Refuel (Metric rFuel, Metric rMaxFuel);
		void SetFuelLeft (Metric rFuel);
		void SetGraceTimer (int iTime) { m_iReactorGraceTimer = iTime; }
		void SetMaxFuel (Metric rMaxFuel);
		void SetOutOfFuel (bool bValue = true) { m_fOutOfFuel = bValue; }
		void SetOutOfPower (bool bValue = true) { m_fOutOfPower = bValue; }
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

//	CShipUpdateSet ------------------------------------------------------------

class CShipUpdateSet
	{
	public:
		enum EUpdateTypes
			{
			//	These values are not persisted. Insert new values in alpha order
			//	and update numbers and count.

			armorBonus		= 0,
			armorStatus		= 1,
			bounds			= 2,
			cargo			= 3,
			deviceBonus		= 4,
			overlays		= 5,
			performance		= 6,
			weaponStatus	= 7,

			count			= 8,
			};

		bool IsSet (EUpdateTypes iType) const { return m_bSet[iType]; }
		void Set (EUpdateTypes iType, bool bValue = true);

	private:
		bool m_bSet[EUpdateTypes::count] = { false };
	};

//  CShipPerformanceDesc ------------------------------------------------------
//
//	NOTE: The CShipPerformanceDesc class is meant as a cache of current ship
//	performance/capabilities based on other ship state (such as devices, 
//	enhancements, overlays, etc.).
//
//	It is NOT meant to keep persistent state such as radiation, EMP, etc.
//	Although we might eventually want to cache that info here.

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

		const CAbilitySet &GetAbilities (void) const { return m_Abilities; }
        const CCargoDesc &GetCargoDesc (void) const { return m_CargoDesc; }
        const CDriveDesc &GetDriveDesc (void) const { return m_DriveDesc; }
        const CIntegralRotationDesc &GetIntegralRotationDesc (void) const { return m_IntegralRotationDesc; }
        const CReactorDesc &GetReactorDesc (void) const { return m_ReactorDesc; }
        const CRotationDesc &GetRotationDesc (void) const { return m_RotationDesc; }
		bool HasShieldInterference (void) const { return (m_fShieldInterference ? true : false); }
        void Init (SShipPerformanceCtx &Ctx);
		bool IsEmpty (void) const { return (m_fInitialized ? false : true); }

        //  Read-Write versions of accessors

		CAbilitySet &GetAbilities (void) { return m_Abilities; }
        CCargoDesc &GetCargoDesc (void) { return m_CargoDesc; }
        CDriveDesc &GetDriveDesc (void) { return m_DriveDesc; }
        CReactorDesc &GetReactorDesc (void) { return m_ReactorDesc; }
        CIntegralRotationDesc &GetIntegralRotationDesc (void) { return m_IntegralRotationDesc; }

		static const CShipPerformanceDesc &Null (void) { return m_Null; }

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
