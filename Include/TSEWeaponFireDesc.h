//	TSEWeaponFireDesc.h
//
//	Defines classes and interfaces for weapons
//	Copyright (c) 2016 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CParticleSystemDesc;
class CWeaponFireDesc;

enum SpecialDamageTypes
	{
	specialNone				= -1,

	specialRadiation		= 0,
	specialBlinding			= 1,
	specialEMP				= 2,
	specialDeviceDamage		= 3,
	specialDisintegration	= 4,
	specialMomentum			= 5,
	specialShieldDisrupt	= 6,
	specialWMD				= 7,

	specialMining			= 8,
	specialDeviceDisrupt	= 9,
	specialWormhole			= 10,
	specialFuel				= 11,
	specialShatter			= 12,
	specialArmor			= 13,
	specialSensor			= 14,
	specialShieldPenetrator	= 15,
	};

class DamageDesc
	{
	public:
		enum Flags 
			{
			flagAverageDamage =		0x00000001,
			flagNoDamageType =		0x00000002,
			flagShockwaveDamage =	0x00000004,

			//	GetDamage

			flagIncludeBonus =		0x00000008,
			flagWMDAdj =			0x00000010,
			flagMinDamage =			0x00000020,
			flagMaxDamage =			0x00000040,

            //  GetSpecialDamage

            flagSpecialAdj =        0x00000080, //  Returns adjusted value (e.g., GetMassDestructionAdj
                                                //      instead of raw value).
            flagSpecialLevel =      0x00000100, //  Returns display level (e.g., GetMassDestructionLevel)
			};

		DamageDesc (void) { }
		DamageDesc (DamageTypes iType, const DiceRange &Damage) : m_iType(iType),
				m_Damage(Damage),
				m_iBonus(0),
				m_iCause(killedByDamage),
				m_EMPDamage(0),
				m_MomentumDamage(0),
				m_RadiationDamage(0),
				m_DisintegrationDamage(0),
				m_DeviceDisruptDamage(0),
				m_BlindingDamage(0),
				m_SensorDamage(0),
				m_ShieldDamage(0),
				m_ArmorDamage(0),
				m_WormholeDamage(0),
				m_FuelDamage(0),
				m_fNoSRSFlash(0),
				m_fAutomatedWeapon(0),
				m_DeviceDamage(0),
				m_MassDestructionAdj(0),
				m_MiningAdj(0),
				m_ShatterDamage(0),
				m_ShieldPenetratorAdj(0),
				m_dwSpare2(0)
			{ }

		inline void AddBonus (int iBonus) { m_iBonus += iBonus; }
		void AddEnhancements (const CItemEnhancementStack *pEnhancements);
		inline bool CausesSRSFlash (void) const { return (m_fNoSRSFlash ? false : true); }
		ICCItem *FindProperty (const CString &sName) const;
		inline DestructionTypes GetCause (void) const { return m_iCause; }
		inline const DiceRange &GetDamageRange (void) const { return m_Damage; }
		inline DamageTypes GetDamageType (void) const { return m_iType; }
		Metric GetDamageValue (DWORD dwFlags = 0) const;
		CString GetDesc (DWORD dwFlags = 0);
		int GetMinDamage (void) const;
		int GetMaxDamage (void) const;
		int GetSpecialDamage (SpecialDamageTypes iSpecial, DWORD dwFlags = 0) const;
		bool IsAutomatedWeapon (void) const { return (m_fAutomatedWeapon ? true : false); }
		bool IsEnergyDamage (void) const;
		bool IsMatterDamage (void) const;
		ALERROR LoadFromXML (SDesignLoadCtx &Ctx, const CString &sAttrib);
		void ReadFromStream (SLoadCtx &Ctx);
		int RollDamage (void) const;
        inline void ScaleDamage (Metric rAdj) { m_Damage.Scale(rAdj); }
		inline void SetAutomatedWeapon (void) { m_fAutomatedWeapon = true; }
		inline void SetCause (DestructionTypes iCause) { m_iCause = iCause; }
		void SetDamage (int iDamage);
		inline void SetDamageType (DamageTypes iType) { m_iType = iType; }
		inline void SetNoSRSFlash (void) { m_fNoSRSFlash = true; }
		void SetSpecialDamage (SpecialDamageTypes iSpecial, int iLevel);
		void WriteToStream (IWriteStream *pStream) const;

		inline int GetArmorDamageLevel (void) const { return (int)m_ArmorDamage; }
		inline int GetBlindingDamage (void) const { return (int)m_BlindingDamage; }
		inline int GetDeviceDamage (void) const { return (int)m_DeviceDamage; }
		inline int GetDeviceDisruptDamage (void) const { return (int)m_DeviceDisruptDamage; }
		inline int GetDisintegrationDamage (void) const { return (int)m_DisintegrationDamage; }
		inline int GetEMPDamage (void) const { return (int)m_EMPDamage; }
        int GetMassDestructionAdj (void) const;
        int GetMassDestructionLevel (void) const;
		inline int GetMiningAdj (void) const { return (int)(m_MiningAdj ? (2 * (m_MiningAdj * m_MiningAdj) + 2) : 0); }
		inline int GetMomentumDamage (void) const { return (int)m_MomentumDamage; }
		inline int GetRadiationDamage (void) const { return (int)m_RadiationDamage; }
		inline int GetShatterDamage (void) const { return (int)m_ShatterDamage; }
		inline int GetShieldDamageLevel (void) const { return (int)m_ShieldDamage; }
		inline int GetShieldPenetratorAdj (void) const { return (int)(m_ShieldPenetratorAdj ? (2 * (m_ShieldPenetratorAdj * m_ShieldPenetratorAdj) + 2) : 0); }

		static SpecialDamageTypes ConvertPropertyToSpecialDamageTypes (const CString &sValue);
		static SpecialDamageTypes ConvertToSpecialDamageTypes (const CString &sValue);
        static int GetDamageLevel (DamageTypes iType);
        static int GetDamageTier (DamageTypes iType);
		static CString GetSpecialDamageName (SpecialDamageTypes iSpecial);
        static int GetMassDestructionLevelFromValue (int iValue);

	private:
		ALERROR LoadTermFromXML (SDesignLoadCtx &Ctx, const CString &sType, const CString &sArg);
		ALERROR ParseTerm (SDesignLoadCtx &Ctx, char *pPos, CString *retsKeyword, CString *retsValue, char **retpPos);

		DamageTypes m_iType;					//	Type of damage
		DiceRange m_Damage;						//	Amount of damage
		int m_iBonus;							//	Bonus to damage (%)
		DestructionTypes m_iCause;				//	Cause of damage

		//	Extra damage
		DWORD m_EMPDamage:3;					//	Ion (paralysis) damage
		DWORD m_MomentumDamage:3;				//	Momentum damage
		DWORD m_RadiationDamage:3;				//	Radiation damage
		DWORD m_DeviceDisruptDamage:3;			//	Disrupt devices damage
		DWORD m_BlindingDamage:3;				//	Optical sensor damage
		DWORD m_SensorDamage:3;					//	Long-range sensor damage
		DWORD m_WormholeDamage:3;				//	Teleport
		DWORD m_FuelDamage:3;					//	Drain fuel
		DWORD m_DisintegrationDamage:3;			//	Disintegration damage
		DWORD m_ShieldPenetratorAdj:3;			//	Shield penetrator damage	shieldPenetrate:n

		DWORD m_fNoSRSFlash:1;					//	If TRUE, damage should not cause SRS flash
		DWORD m_fAutomatedWeapon:1;				//	TRUE if this damage is caused by automated weapon

		DWORD m_DeviceDamage:3;					//	Damage to devices
		DWORD m_MassDestructionAdj:3;			//	Adj for mass destruction
		DWORD m_MiningAdj:3;					//	Adj for mining capability
		DWORD m_ShatterDamage:3;				//	Shatter damage
		DWORD m_dwSpare2:20;

		BYTE m_ShieldDamage;					//	Shield damage (level)	shield:level
		BYTE m_ArmorDamage;						//	Armor damage (level)
		BYTE m_Spare2;
		BYTE m_Spare3;
	};

enum EDamageResults
	{
	damageNoDamage =				0,
	damageAbsorbedByShields =		1,
	damageArmorHit =				2,
	damageStructuralHit =			3,
	damageDestroyed =				4,
	damagePassthrough =				5,	//	When we hit another missile (or small obj) we pass through
	damagePassthroughDestroyed =	6,	//	Target destroyed, but we pass through
	damageDestroyedAbandoned =		7,	//	Station was abandoned, but object not destroyed
	damageNoDamageNoPassthrough =	8,	//	No damage; stop any passthrough
	damageDisintegrated =			9,
	damageShattered =				10,

	damageResultCount =				11,
	};

struct SDamageCtx
	{
	SDamageCtx (void) :
			pObj(NULL),
			pDesc(NULL),
			iDirection(-1),
			pCause(NULL),
			bNoHitEffect(false),
			bIgnoreOverlays(false),
			bIgnoreShields(false),
			iDamage(0),
			iSectHit(-1),
			iOverlayHitDamage(0),
			iShieldHitDamage(0),
			iArmorHitDamage(0),
			iHPLeft(0),
			iAbsorb(0),
			iShieldDamage(0),
			iOriginalAbsorb(0),
			iOriginalShieldDamage(0),
			iArmorAbsorb(0),
			iArmorDamage(0),
			bBlind(false),
			iBlindTime(0),
			bDeviceDisrupt(false),
			iDisruptTime(0),
			bDeviceDamage(false),
			bDisintegrate(false),
			bParalyze(false),
			iParalyzeTime(0),
			bRadioactive(false),
			bReflect(false),
			bShatter(false)
		{ }

	inline CSpaceObject *GetOrderGiver (void) const { return Attacker.GetOrderGiver(); }

	CSpaceObject *pObj;							//	Object hit
	CWeaponFireDesc *pDesc;						//	WeaponFireDesc
	DamageDesc Damage;							//	Damage
	int iDirection;								//	Direction that hit came from
	CVector vHitPos;							//	Hit at this position
	CSpaceObject *pCause;						//	Object that directly caused the damage
	CDamageSource Attacker;						//	Ultimate attacker
	bool bNoHitEffect;							//	No hit effect
	bool bIgnoreOverlays;						//	Start damage at shields
	bool bIgnoreShields;						//	Start damage at armor

	int iDamage;								//	Damage hp
	int iSectHit;								//	Armor section hit on object

	//	These are some results
	int iOverlayHitDamage;						//	HP that hit overlays
	int iShieldHitDamage;						//	HP that hit shields
	int iArmorHitDamage;						//	HP that hit armor

	//	These are used within armor/shield processing
	int iHPLeft;								//	HP left on shields (before damage)
	int iAbsorb;								//	Damage absorbed by shields
	int iShieldDamage;							//	Damage taken by shields
	int iOriginalAbsorb;						//	Computed absorb value, if shot had not been reflected
	int iOriginalShieldDamage;					//	Computed shield damage value, if shot had not been reflected
	int iArmorAbsorb;							//	Damage absorbed by armor
	int iArmorDamage;							//	Damage taken by armor

	//	Damage effects
	bool bBlind;								//	If true, shot will blind the target
	int iBlindTime;
	bool bDeviceDisrupt;						//	If true, shot will disrupt devices
	int iDisruptTime;
	bool bDeviceDamage;							//	If true, shot will damage devices
	bool bDisintegrate;							//	If true, shot will disintegrate target
	bool bParalyze;								//	If true, shot will paralyze the target
	int iParalyzeTime;
	bool bRadioactive;							//	If true, shot will irradiate the target
	bool bReflect;								//	If true, armor/shields reflected the shot
	bool bShatter;								//	If true, shot will shatter the target
	};

struct SDestroyCtx
	{
	CSpaceObject *GetOrderGiver (void) const;

	CSpaceObject *pObj = NULL;						//	Object destroyed
	CWeaponFireDesc *pDesc = NULL;					//	WeaponFireDesc
	CDamageSource Attacker;							//	Ultimate attacker
	CSpaceObject *pWreck = NULL;					//	Wreck left behind
	DestructionTypes iCause = removedFromSystem;	//	Cause of damage

	bool bResurrectPending = false;					//	TRUE if this object will be resurrected
	bool bRemovedByOwner = false;					//	TRUE if this is an attached obj removed by its parent
	CSpaceObject *pResurrectedObj = NULL;			//	Pointer to resurrected object
	};

class DamageTypeSet
	{
	public:
		DamageTypeSet (void) : m_dwSet(0) { }

		ALERROR InitFromXML (const CString &sAttrib);
		void Add (int iType) { if (iType > damageGeneric) m_dwSet |= (1 << iType); }
		bool InSet (int iType) const { return (iType <= damageGeneric ? false : ((m_dwSet & (1 << iType)) ? true : false)); }
		inline bool IsEmpty (void) const { return (m_dwSet == 0); }
		void Remove (int iType) { if (iType > damageGeneric) m_dwSet &= ~(1 << iType); }

	private:
		DWORD m_dwSet;
	};

struct SShotCreateCtx
	{
	enum Flags
		{
		//	CreateWeaponFire flags
		CWF_WEAPON_FIRE =				0x00000001,	//	Creating a shot from a weapon
		CWF_FRAGMENT =					0x00000002,	//	Creating a fragment
		CWF_EXPLOSION =					0x00000004,	//	Creating an explosion (or fragment of an explosion)
		CWF_EJECTA =					0x00000008,	//	Creating ejecta (or fragments of ejecta)
		CWF_REPEAT =					0x00000010,	//	Mixed with CWF_WEAPON_FIRE to indicate this is a repeat
		CWF_PRIMARY =					0x00000020,	//	The first shot in a multi-shot weapon
		};

	CWeaponFireDesc *pDesc = NULL;
	TSharedPtr<CItemEnhancementStack> pEnhancements;
	CDamageSource Source;
	CVector vPos;
	CVector vVel;
	int iDirection = 0;
	int iRepeatingCount = 0;
	CSpaceObject *pTarget = NULL;
	DWORD dwFlags = 0;
	};

//	WeaponFireDesc

enum FireTypes
	{
	ftArea,
	ftBeam,
	ftContinuousBeam,
	ftMissile,
	ftParticles,
	ftRadius,
	};

struct SExplosionType
	{
	CWeaponFireDesc *pDesc = NULL;					//	Explosion type
	int iBonus = 0;									//	Bonus damage
	DestructionTypes iCause = killedByExplosion;	//	Cause
	};

class CWeaponFireDesc
	{
	public:
		enum ECachedHandlers
			{
			evtOnCreateShot				= 0,
			evtOnDamageAbandoned		= 1,
			evtOnDamageArmor			= 2,
			evtOnDamageOverlay			= 3,
			evtOnDamageShields			= 4,
			evtOnDestroyObj				= 5,
			evtOnDestroyShot			= 6,
			evtOnFragment				= 7,

			evtCount					= 8,
			};

		struct SFragmentDesc
			{
			CWeaponFireDesc *pDesc;			//	Data for fragments
			DiceRange Count;				//	Number of fragments
			bool bMIRV;						//	Fragments seek independent targets

			SFragmentDesc *pNext;
			};

        struct SExhaustDesc
            {
            SExhaustDesc (void) :
                    iExhaustRate(0),
                    iExhaustLifetime(0),
                    rExhaustDrag(0.0)
                { }

		    int iExhaustRate;				//	Ticks per exhaust creation (0 if no exhaust)
		    CObjectImageArray ExhaustImage; //	Image for exhaust
		    int iExhaustLifetime;			//	Ticks that each exhaust particle lasts
		    Metric rExhaustDrag;			//	Coefficient of drag for exhaust particles
            };

		struct SInitOptions
			{
			SInitOptions (void) :
					iLevel(1),
					bDamageOnly(false)
				{ }

			CString sUNID;					//	UNID of weapon fire desc
			int iLevel;						//	Level (for scalable weapons)

			//	Options

			bool bDamageOnly;				//	Defines damage (not entire projectile desc)
			};

        struct SVaporTrailDesc
            {
            SVaporTrailDesc (void) :
                    iVaporTrailWidth(0),
                    iVaporTrailLength(0),
                    iVaporTrailWidthInc(0),
                    rgbVaporTrailColor(CG32bitPixel::Null())
                { }

		    int iVaporTrailWidth;			//	Width of vapor trail (0 = none)
		    int iVaporTrailLength;			//	Number of segments
		    int iVaporTrailWidthInc;		//	Width increment in 100ths of a pixel
		    CG32bitPixel rgbVaporTrailColor;//	Color of vapor trail
            };

		CWeaponFireDesc (void);
		~CWeaponFireDesc (void);

		void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed);
        void ApplyAcceleration (CSpaceObject *pMissile) const;
		inline bool CanAutoTarget (void) const { return (m_fAutoTarget ? true : false); }
        inline bool CanDamageSource (void) const { return (m_fCanDamageSource ? true : false); }
		bool CanHit (CSpaceObject *pObj) const;
		inline bool CanHitFriends (void) const { return !m_fNoFriendlyFire; }
		IEffectPainter *CreateEffectPainter (SShotCreateCtx &CreateCtx);
		void CreateFireEffect (CSystem *pSystem, CSpaceObject *pSource, const CVector &vPos, const CVector &vVel, int iDir);
		void CreateHitEffect (CSystem *pSystem, SDamageCtx &DamageCtx);
		IEffectPainter *CreateParticlePainter (void);
		IEffectPainter *CreateSecondaryPainter (bool bTrackingObj = false, bool bUseObjectCenter = false);
		IEffectPainter *CreateShockwavePainter (bool bTrackingObj = false, bool bUseObjectCenter = false);
		bool FindDataField (const CString &sField, CString *retsValue) const;
		CEffectCreator *FindEffectCreator (const CString &sUNID);
		bool FindEventHandler (const CString &sEvent, SEventHandlerDesc *retEvent = NULL) const;
		inline bool FindEventHandler (ECachedHandlers iEvent, SEventHandlerDesc *retEvent = NULL) const 
			{
			if (!m_CachedEvents[iEvent].pCode)
				return false;

			if (retEvent) *retEvent = m_CachedEvents[iEvent];
			return true;
			}

		ICCItem *FindProperty (const CString &sProperty) const;
		CWeaponFireDesc *FindWeaponFireDesc (const CString &sUNID, char **retpPos = NULL);
		static CWeaponFireDesc *FindWeaponFireDescFromFullUNID (const CString &sUNID);
        ALERROR FinishBindDesign (SDesignLoadCtx &Ctx);
		void FireOnCreateShot (const CDamageSource &Source, CSpaceObject *pShot, CSpaceObject *pTarget);
		bool FireOnDamageAbandoned (SDamageCtx &Ctx);
		bool FireOnDamageArmor (SDamageCtx &Ctx);
		bool FireOnDamageOverlay (SDamageCtx &Ctx, COverlay *pOverlay);
		bool FireOnDamageShields (SDamageCtx &Ctx, int iDevice);
		bool FireOnFragment (const CDamageSource &Source, CSpaceObject *pShot, const CVector &vHitPos, CSpaceObject *pNearestObj, CSpaceObject *pTarget);
		void FireOnDestroyObj (const SDestroyCtx &Ctx);
		void FireOnDestroyShot (CSpaceObject *pShot);
		inline CItemType *GetAmmoType (void) const { return m_pAmmoType; }
        inline DWORD GetAmmoTypeUNID (void) const { return m_pAmmoType.GetUNID(); }
		inline int GetAreaDamageDensity (void) const { return m_AreaDamageDensity.Roll(); }
		inline Metric GetAreaDamageDensityAverage (void) const { return m_AreaDamageDensity.GetAveValueFloat(); }
		inline Metric GetAveDamage (void) const { return m_Damage.GetDamageValue(); }
		inline Metric GetAveExpansionSpeed (void) const { return (m_ExpansionSpeed.GetAveValue() * LIGHT_SPEED / 100.0); }
		Metric GetAveInitialSpeed (void) const;
		inline int GetAveLifetime (void) const { return m_Lifetime.GetAveValue(); }
		Metric GetAveParticleCount (void) const;
        Metric GetAveSpeed (void) const { return 0.5 * (GetRatedSpeed() + m_rMaxMissileSpeed); }
        inline int GetContinuous (void) const { return m_iContinuous; }
		inline int GetContinuousFireDelay (void) const { return m_iContinuousFireDelay; }
		const DamageDesc &GetDamage (void) const { return m_Damage; }
		DamageTypes GetDamageType (void) const;
		inline CEffectCreator *GetEffect (void) const { return m_pEffect; }
        inline Metric GetEffectiveRange (void) const { return m_rMaxEffectiveRange; }
		inline ICCItem *GetEventHandler (const CString &sEvent) const { SEventHandlerDesc Event; if (!FindEventHandler(sEvent, &Event)) return NULL; return Event.pCode; }
        inline const SExhaustDesc &GetExhaust (void) const { return GetOldEffects().Exhaust; }
		inline Metric GetExpansionSpeed (void) const { return (m_ExpansionSpeed.Roll() * LIGHT_SPEED / 100.0); }
		inline CWeaponFireDesc *GetExplosionType (void) const { return m_pExplosionType; }
		inline CExtension *GetExtension (void) const { return m_pExtension; }
		inline int GetFireDelay (void) const { return m_iFireRate; }
		inline FireTypes GetFireType (void) const { return m_iFireType; }
		inline SFragmentDesc *GetFirstFragment (void) const { return m_pFirstFragment; }
		inline Metric GetFragmentationThreshold (void) const { return m_rFragThreshold; }
		inline int GetHitPoints (void) const { return m_iHitPoints; }
        inline const CObjectImageArray &GetImage (void) const { return GetOldEffects().Image; }
		inline int GetInitialDelay (void) const { return m_InitialDelay.Roll(); }
		Metric GetInitialSpeed (void) const;
		inline int GetInteraction (void) const { return m_iInteraction; }
        int GetLevel (void) const;
		inline int GetLifetime (void) const { return m_Lifetime.Roll(); }
		inline int GetManeuverRate (void) const { return m_iManeuverRate; }
		inline int GetMaxLifetime (void) const { return m_Lifetime.GetMaxValue(); }
		inline Metric GetMaxRadius (void) const { return m_rMaxRadius; }
		inline int GetMinDamage (void) const { return m_MinDamage.Roll(); }
		inline Metric GetMinRadius (void) const { return m_rMinRadius; }
        Metric GetMaxRange (void) const;
		CEffectCreator *GetParticleEffect (void) const;
		inline const CParticleSystemDesc *GetParticleSystemDesc (void) const { return m_pParticleDesc; }
		inline int GetPassthrough (void) const { return m_iPassthrough; }
		inline int GetProximityFailsafe (void) const { return m_iProximityFailsafe; }
		inline Metric GetRatedSpeed (void) const { return m_rMissileSpeed; }
        CWeaponFireDesc *GetScaledDesc (int iLevel) const;
        int GetSpecialDamage (SpecialDamageTypes iSpecial, DWORD dwFlags = 0) const;
        inline int GetStealth (void) const { return m_iStealth; }
        inline FireTypes GetType (void) const { return m_iFireType; }
        inline const CString &GetUNID (void) const { return m_sUNID; }
        inline const SVaporTrailDesc &GetVaporTrail (void) const { return GetOldEffects().VaporTrail; }
		CItemType *GetWeaponType (CItemType **retpLauncher = NULL) const;
		inline bool HasEvents (void) const { return !m_Events.IsEmpty(); }
		inline bool HasFragments (void) const { return m_pFirstFragment != NULL; }
		bool HasFragmentInterval (int *retiInterval = NULL) const;
		inline bool HasOnFragmentEvent (void) const { return m_CachedEvents[evtOnFragment].pCode != NULL; }
		void InitFromDamage (const DamageDesc &Damage);
		ALERROR InitFromMissileXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pMissile, const SInitOptions &Options);
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const SInitOptions &Options);
        ALERROR InitScaledStats (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pItem);
		inline bool IsCurvedBeam (void) const { return false; }
        inline bool IsDirectionalImage (void) const { return m_fDirectional; }
        inline bool IsFragment (void) const { return m_fFragment; }
		inline bool IsMIRV (void) const { return (m_pFirstFragment ? m_pFirstFragment->bMIRV : false); }
        inline bool IsScalable (void) const { return (m_pScalable != NULL); }
		inline bool IsTargetRequired (void) const { return (m_fTargetRequired ? true : false); }
		inline bool IsTracking (void) const { return m_iManeuverability != 0; }
		bool IsTrackingOrHasTrackingFragments (void) const;
		inline bool IsTrackingTime (int iTick) const { return (m_iManeuverability > 0 && (iTick % m_iManeuverability) == 0); }
		void MarkImages (void);
		ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx);
		inline void PlayFireSound (CSpaceObject *pSource) { m_FireSound.PlaySound(pSource); }
		inline bool ProximityBlast (void) const { return (m_fProximityBlast ? true : false); }

	private:
        struct SOldEffects
            {
		    CObjectImageArray Image;		//	Image for missile
            SExhaustDesc Exhaust;           //  Exhaust effect
            SVaporTrailDesc VaporTrail;     //  Vapor trail effect
            };

        //  Copying this class is not supported

		CWeaponFireDesc (const CWeaponFireDesc &Desc) = delete;
        CWeaponFireDesc & operator= (const CWeaponFireDesc &Desc) = delete;

        Metric CalcMaxEffectiveRange (void) const;
		Metric CalcSpeed (Metric rPercentOfLight) const;
		CEffectCreator *GetFireEffect (void) const;
        inline SOldEffects &GetOldEffects (void) const { return (m_pOldEffects ? *m_pOldEffects : m_NullOldEffects); }
        inline SOldEffects &SetOldEffects (void) { if (m_pOldEffects == NULL) m_pOldEffects = new SOldEffects; return *m_pOldEffects; }
        ALERROR InitScaledStats (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pItem, const CWeaponFireDesc &Src, int iBaseLevel, int iScaledLevel);

		CExtension *m_pExtension;			//	Extension that defines the weaponfiredesc
		CString m_sUNID;					//	Identification. The format is
											//		uuuuu/n/fi/e
											//
											//		uuuuu = weapon UNID
											//		n = shot data ordinal
											//		fi = fragment, i is index (optional)
											//		e = enhanced (optional)

		//	Basic properties
        int m_iLevel;                       //  Level of desc (missile or weapon or scalable weapon)
		CItemTypeRef m_pAmmoType;			//	item type for this ammo
		FireTypes m_iFireType;				//	beam or missile
		DamageDesc m_Damage;				//	Damage per shot
		int m_iContinuous;					//	repeat for this number of frames
		int m_iContinuousFireDelay;			//	Ticks between continuous fire shots
		int m_iFireRate;					//	Ticks between shots (-1 = default to weapon fire rate)

		Metric m_rMissileSpeed;				//	Speed of missile
		DiceRange m_MissileSpeed;			//	Speed of missile (if random)
		DiceRange m_Lifetime;				//	Lifetime of fire in seconds
		DiceRange m_InitialDelay;			//	Delay for n ticks before starting

		int m_iPassthrough;					//	Chance that the missile will continue through target

        //  Computed properties
		Metric m_rMaxEffectiveRange;		//	Max effective range of weapon

		//	Effects
		CEffectCreatorRef m_pEffect;		//	Effect for the actual bullet/missile/beam
		CEffectCreatorRef m_pHitEffect;		//	Effect when we hit/explode
		CEffectCreatorRef m_pFireEffect;	//	Effect when we fire (muzzle flash)
		CSoundRef m_FireSound;				//	Sound when weapon is fired
        SOldEffects *m_pOldEffects;         //  Non-painter effects.
		CWeaponFireDescRef m_pExplosionType;	//	Explosion to create when ship is destroyed

		//	Missile stuff (m_iFireType == ftMissile)
		int m_iAccelerationFactor;			//	% increase in speed per 10 ticks
		Metric m_rMaxMissileSpeed;			//	Max speed.
		int m_iStealth;						//	Missile stealth
		int m_iHitPoints;					//	HP before dissipating (0 = destroyed by any hit)
		int m_iInteraction;					//	Interaction opacity (0-100)
		int m_iManeuverability;				//	Tracking maneuverability (0 = none)
		int m_iManeuverRate;				//	Angle turned at each maneuverability point

		//	Particles (m_iFireType == ftParticles)
		CParticleSystemDesc *m_pParticleDesc;
		DiceRange m_MinDamage;				//	Minimum damage when hit by particles

		//	Area stuff (m_iFireType == ftArea)
		DiceRange m_ExpansionSpeed;			//	Speed of expansion (% of lightspeed)
		DiceRange m_AreaDamageDensity;		//	Number of points along the edge

		//	Radius stuff (m_iFireType == ftRadius)
		Metric m_rMinRadius;				//	All objects inside this radius take full damage
		Metric m_rMaxRadius;				//	Damage inside this radius is decreased by inverse square law

		//	Fragmentation
		SFragmentDesc *m_pFirstFragment;	//	Pointer to first fragment desc (or NULL)
		int m_iProximityFailsafe;			//	Min ticks before proximity is active
		DiceRange m_FragInterval;			//	If not empty, we keep fragmenting
		Metric m_rFragThreshold;			//	Max fragmentation distance

		//	Events
		CEventHandler m_Events;				//	Events
		SEventHandlerDesc m_CachedEvents[evtCount];

        //  Scalable items
        int m_iBaseLevel;                   //  Base level
        int m_iScaledLevels;                //  Number of scaled levels above base
        CWeaponFireDesc *m_pScalable;       //  Array of entries, one per scaled level above base

		//	Flags
		DWORD m_fVariableInitialSpeed:1;	//	TRUE if initial speed is random
		DWORD m_fNoFriendlyFire:1;			//	TRUE if we cannot hit our friends
		DWORD m_fNoWorldHits:1;				//	If TRUE, we never hit worlds (or stars)
		DWORD m_fNoImmutableHits:1;			//	If TRUE, we never hit immutable objects
		DWORD m_fNoStationHits:1;			//	If TRUE, we never hit station-scale objects
		DWORD m_fNoImmobileHits:1;			//	If TRUE, we never hit immobile objects
		DWORD m_fNoShipHits:1;				//	If TRUE, we never hit ship-scale objects
		DWORD m_fAutoTarget:1;				//	If TRUE, we can acquire new targets after launch

		DWORD m_fCanDamageSource;			//	TRUE if we can damage the source
		DWORD m_fDirectional:1;				//	True if different images for each direction
		DWORD m_fFragment:1;				//	True if this is a fragment of a proximity blast
		DWORD m_fProximityBlast;			//	This is TRUE if we have fragments or if we have
											//		and OnFragment event.
        DWORD m_fRelativisticSpeed:1;		//	If TRUE, adjust speed to simulate for light-lag
        DWORD m_fTargetRequired:1;			//	If TRUE, do not fragment unless we have a target
        DWORD m_fSpare7:1;
        DWORD m_fSpare8:1;

		DWORD m_dwSpare:16;

        static SOldEffects m_NullOldEffects;
	};
