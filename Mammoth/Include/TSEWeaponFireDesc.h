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

	specialTimeStop			= 16,
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
				m_TimeStopDamage(0),
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

		void AddEnhancements (const CItemEnhancementStack *pEnhancements);
		bool CausesSRSFlash (void) const { return (m_fNoSRSFlash ? false : true); }
		ICCItem *FindProperty (const CString &sName) const;
		DestructionTypes GetCause (void) const { return m_iCause; }
		const DiceRange &GetDamageRange (void) const { return m_Damage; }
		DamageTypes GetDamageType (void) const { return m_iType; }
		Metric GetDamageValue (DWORD dwFlags = 0) const;
		CString GetDesc (DWORD dwFlags = 0);
		int GetMinDamage (void) const;
		int GetMaxDamage (void) const;
		int GetSpecialDamage (SpecialDamageTypes iSpecial, DWORD dwFlags = 0) const;
		bool IsAutomatedWeapon (void) const { return (m_fAutomatedWeapon ? true : false); }
		bool IsEmpty (void) const { return (m_Damage.IsEmpty() && m_iType == damageGeneric); }
		bool IsEnergyDamage (void) const;
		bool IsMatterDamage (void) const;
		ALERROR LoadFromXML (SDesignLoadCtx &Ctx, const CString &sAttrib);
		void ReadFromStream (SLoadCtx &Ctx);
		int RollDamage (void) const;
        void ScaleDamage (Metric rAdj) { m_Damage.Scale(rAdj); }
		void SetAutomatedWeapon (void) { m_fAutomatedWeapon = true; }
		void SetCause (DestructionTypes iCause) { m_iCause = iCause; }
		void SetDamage (int iDamage);
		void SetDamageType (DamageTypes iType) { m_iType = iType; }
		void SetNoSRSFlash (void) { m_fNoSRSFlash = true; }
		void SetSpecialDamage (SpecialDamageTypes iSpecial, int iLevel);
		void WriteToStream (IWriteStream *pStream) const;

		int GetArmorDamageLevel (void) const { return (int)m_ArmorDamage; }
		int GetBlindingDamage (void) const { return (int)m_BlindingDamage; }
		int GetDeviceDamage (void) const { return (int)m_DeviceDamage; }
		int GetDeviceDisruptDamage (void) const { return (int)m_DeviceDisruptDamage; }
		int GetDisintegrationDamage (void) const { return (int)m_DisintegrationDamage; }
		int GetEMPDamage (void) const { return (int)m_EMPDamage; }
        int GetMassDestructionAdj (void) const;
        int GetMassDestructionLevel (void) const;
		int GetMiningAdj (void) const { return (int)(m_MiningAdj ? (2 * (m_MiningAdj * m_MiningAdj) + 2) : 0); }
		int GetMomentumDamage (void) const { return (int)m_MomentumDamage; }
		int GetRadiationDamage (void) const { return (int)m_RadiationDamage; }
		int GetShatterDamage (void) const { return (int)m_ShatterDamage; }
		int GetShieldDamageLevel (void) const { return (int)m_ShieldDamage; }
		int GetShieldPenetratorAdj (void) const { return (int)(m_ShieldPenetratorAdj ? (2 * (m_ShieldPenetratorAdj * m_ShieldPenetratorAdj) + 2) : 0); }
		int GetTimeStopDamageLevel (void) const { return (int)m_TimeStopDamage; }
		int GetTimeStopResistChance (int iTargetLevel) const;

		static SpecialDamageTypes ConvertPropertyToSpecialDamageTypes (const CString &sValue);
		static SpecialDamageTypes ConvertToSpecialDamageTypes (const CString &sValue);
        static int GetDamageLevel (DamageTypes iType);
        static int GetDamageTier (DamageTypes iType);
		static CString GetSpecialDamageName (SpecialDamageTypes iSpecial);
        static int GetMassDestructionLevelFromValue (int iValue);

	private:
		void AddBonus (int iBonus) { m_iBonus += iBonus; }
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
		BYTE m_TimeStopDamage;					//	Time stop (level)
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
	public:
		SDamageCtx (void) { }
		SDamageCtx (CSpaceObject *pObjHitArg, 
				CWeaponFireDesc *pDescArg, 
				const CItemEnhancementStack *pEnhancementsArg, 
				CDamageSource &AttackerArg, 
				CSpaceObject *pCauseArg, 
				int iDirectionArg, 
				const CVector &vHitPosArg,
				int iDamageArg = -1);
		SDamageCtx (const DamageDesc &DamageArg);
		~SDamageCtx (void);

		void ClearTimeStop (void) { m_bTimeStop = false; }
		int GetBlindTime (void) const { return m_iBlindTime; }
		int GetDeviceDisruptTime (void) const { return m_iDisruptTime; }
		CSpaceObject *GetOrderGiver (void) const { return Attacker.GetOrderGiver(); }
		int GetParalyzedTime (void) const { return m_iParalyzeTime; }
		bool IsBlinded (void) const { return m_bBlind; }
		bool IsDeviceDamaged (void) const { return m_bDeviceDamage; }
		bool IsDeviceDisrupted (void) const { return m_bDeviceDisrupt; }
		bool IsDisintegrated (void) const { return m_bDisintegrate; }
		bool IsParalyzed (void) const { return m_bParalyze; }
		bool IsRadioactive (void) const { return m_bRadioactive; }
		bool IsShattered (void) const { return m_bShatter; }
		bool IsShotReflected (void) const { return m_bReflect; }
		bool IsTimeStopped (void) const { return m_bTimeStop; }
		void SetBlinded (bool bValue = true) { m_bBlind = bValue; }
		void SetBlindedTime (int iTime) { m_iBlindTime = iTime; }
		void SetDeviceDamaged (bool bValue = true) { m_bDeviceDamage = bValue; }
		void SetDeviceDisrupted (bool bValue = true) { m_bDeviceDisrupt = bValue; }
		void SetDeviceDisruptedTime (int iTime) { m_iDisruptTime = iTime; }
		void SetDisintegrated (bool bValue = true) { m_bDisintegrate = bValue; }
		void SetParalyzed (bool bValue = true) { m_bParalyze = bValue; }
		void SetParalyzedTime (int iTime) { m_iParalyzeTime = iTime; }
		void SetRadioactive (bool bValue = true) { m_bRadioactive = bValue; }
		void SetShattered (bool bValue = true) { m_bShatter = bValue; }
		void SetShotReflected (bool bValue = true) { m_bReflect = bValue; }
		void SetTimeStopped (bool bValue = true) { m_bTimeStop = bValue; }

		CSpaceObject *pObj = NULL;					//	Object hit
		CWeaponFireDesc *pDesc = NULL;				//	WeaponFireDesc
		DamageDesc Damage;							//	Damage
		int iDirection = -1;						//	Direction that hit came from
		CVector vHitPos;							//	Hit at this position
		CSpaceObject *pCause = NULL;				//	Object that directly caused the damage
		CDamageSource Attacker;						//	Ultimate attacker
		bool bNoHitEffect = false;					//	No hit effect
		bool bIgnoreOverlays = false;				//	Start damage at shields
		bool bIgnoreShields = false;				//	Start damage at armor

		int iDamage = 0;							//	Damage hp
		int iSectHit = -1;							//	Armor section hit on object

		//	These are some results
		int iOverlayHitDamage = 0;					//	HP that hit overlays
		int iShieldHitDamage = 0;					//	HP that hit shields
		int iArmorHitDamage = 0;					//	HP that hit armor

		//	These are used within armor/shield processing
		int iHPLeft = 0;							//	HP left on shields (before damage)
		int iAbsorb = 0;							//	Damage absorbed by shields
		int iShieldDamage = 0;						//	Damage taken by shields
		int iOriginalAbsorb = 0;					//	Computed absorb value, if shot had not been reflected
		int iOriginalShieldDamage = 0;				//	Computed shield damage value, if shot had not been reflected
		int iArmorAbsorb = 0;						//	Damage absorbed by armor
		int iArmorDamage = 0;						//	Damage taken by armor

	private:
		void InitDamageEffects (const DamageDesc &DamageArg);

        //  Copying this class is not supported

		SDamageCtx (const SDamageCtx &Src) = delete;
        SDamageCtx & operator= (const SDamageCtx &Src) = delete;

		//	Damage effects

		bool m_bBlind = false;						//	If true, shot will blind the target
		int m_iBlindTime = 0;
		bool m_bDeviceDisrupt = false;				//	If true, shot will disrupt devices
		int m_iDisruptTime = 0;
		bool m_bDeviceDamage = false;				//	If true, shot will damage devices
		bool m_bDisintegrate = false;				//	If true, shot will disintegrate target
		bool m_bParalyze = false;					//	If true, shot will paralyze the target
		int m_iParalyzeTime = 0;
		bool m_bRadioactive = false;				//	If true, shot will irradiate the target
		bool m_bReflect = false;					//	If true, armor/shields reflected the shot
		bool m_bShatter = false;					//	If true, shot will shatter the target
		bool m_bTimeStop = false;					//	If TRUE, target will be stopped in time

		bool m_bFreeDesc = false;					//	If TRUE, we own pDesc.
	};

struct SDestroyCtx
	{
	SDestroyCtx (CSpaceObject &ObjArg) :
			Obj(ObjArg)
		{ }

	CSpaceObject *GetOrderGiver (void) const;
	bool WasDestroyed (void) const { return (iCause != enteredStargate && iCause != ascended); }

	CSpaceObject &Obj;								//	Object destroyed
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
		bool IsEmpty (void) const { return (m_dwSet == 0); }
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

struct SExplosionType
	{
	CWeaponFireDesc *pDesc = NULL;					//	Explosion type
	int iBonus = 0;									//	Bonus damage
	DestructionTypes iCause = killedByExplosion;	//	Cause
	};

class CWeaponFireDesc
	{
	public:
		enum FireTypes
			{
			ftArea,
			ftBeam,
			ftContinuousBeam,
			ftMissile,
			ftParticles,
			ftRadius,
			};

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
			CString sUNID;					//	UNID of weapon fire desc
			int iLevel = 1;					//	Level (for scalable weapons)

			//	Options

			bool bDamageOnly = false;		//	Defines damage (not entire projectile desc)
			bool bIsFragment = false;		//	We're initializing a fragment
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
		Metric CalcDamage (DWORD dwDamageFlags = 0) const;
		bool CanAutoTarget (void) const { return (m_fAutoTarget ? true : false); }
        bool CanDamageSource (void) const { return (m_fCanDamageSource ? true : false); }
		bool CanHit (CSpaceObject *pObj) const;
		bool CanHitFriends (void) const { return !m_fNoFriendlyFire; }
		IEffectPainter *CreateEffectPainter (SShotCreateCtx &CreateCtx);
		void CreateFireEffect (CSystem *pSystem, CSpaceObject *pSource, const CVector &vPos, const CVector &vVel, int iDir);
		void CreateHitEffect (CSystem *pSystem, SDamageCtx &DamageCtx);
		IEffectPainter *CreateParticlePainter (void);
		IEffectPainter *CreateSecondaryPainter (bool bTrackingObj = false, bool bUseObjectCenter = false);
		IEffectPainter *CreateShockwavePainter (bool bTrackingObj = false, bool bUseObjectCenter = false);
		bool FindDataField (const CString &sField, CString *retsValue) const;
		CEffectCreator *FindEffectCreator (const CString &sUNID);
		bool FindEventHandler (const CString &sEvent, SEventHandlerDesc *retEvent = NULL) const;
		bool FindEventHandler (ECachedHandlers iEvent, SEventHandlerDesc *retEvent = NULL) const 
			{
			if (!m_CachedEvents[iEvent].pCode)
				return false;

			if (retEvent) *retEvent = m_CachedEvents[iEvent];
			return true;
			}

		ICCItem *FindProperty (const CString &sProperty) const;
		CWeaponFireDesc *FindWeaponFireDesc (const CString &sUNID, const char **retpPos = NULL);
		static CWeaponFireDesc *FindWeaponFireDescFromFullUNID (const CString &sUNID);
		void FireOnCreateShot (const CDamageSource &Source, CSpaceObject *pShot, CSpaceObject *pTarget);
		bool FireOnDamageAbandoned (SDamageCtx &Ctx);
		bool FireOnDamageArmor (SDamageCtx &Ctx);
		bool FireOnDamageOverlay (SDamageCtx &Ctx, COverlay *pOverlay);
		bool FireOnDamageShields (SDamageCtx &Ctx, int iDevice);
		bool FireOnFragment (const CDamageSource &Source, CSpaceObject *pShot, const CVector &vHitPos, CSpaceObject *pNearestObj, CSpaceObject *pTarget);
		void FireOnDestroyObj (const SDestroyCtx &Ctx);
		void FireOnDestroyShot (CSpaceObject *pShot);
		CItemType *GetAmmoType (void) const { return m_pAmmoType; }
        DWORD GetAmmoTypeUNID (void) const { return m_pAmmoType.GetUNID(); }
		int GetAreaDamageDensity (void) const { return m_AreaDamageDensity.Roll(); }
		Metric GetAreaDamageDensityAverage (void) const { return m_AreaDamageDensity.GetAveValueFloat(); }
		Metric GetAveDamage (void) const { return m_Damage.GetDamageValue(); }
		Metric GetAveExpansionSpeed (void) const { return (m_ExpansionSpeed.GetAveValue() * LIGHT_SPEED / 100.0); }
		Metric GetAveInitialSpeed (void) const;
		int GetAveLifetime (void) const { return m_Lifetime.GetAveValue(); }
		Metric GetAveParticleCount (void) const;
        Metric GetAveSpeed (void) const { return 0.5 * (GetRatedSpeed() + m_rMaxMissileSpeed); }
        int GetContinuous (void) const { return m_iContinuous; }
		int GetContinuousFireDelay (void) const { return (m_iContinuous != -1 ? m_iContinuousFireDelay : -1); }
		const DamageDesc &GetDamage (void) const { return m_Damage; }
		DamageTypes GetDamageType (void) const;
		CEffectCreator *GetEffect (void) const { return m_pEffect; }
        Metric GetEffectiveRange (void) const { return m_rMaxEffectiveRange; }
		ICCItem *GetEventHandler (const CString &sEvent) const { SEventHandlerDesc Event; if (!FindEventHandler(sEvent, &Event)) return NULL; return Event.pCode; }
        const SExhaustDesc &GetExhaust (void) const { return GetOldEffects().Exhaust; }
		Metric GetExpansionSpeed (void) const { return (m_ExpansionSpeed.Roll() * LIGHT_SPEED / 100.0); }
		CWeaponFireDesc *GetExplosionType (void) const { return m_pExplosionType; }
		CExtension *GetExtension (void) const { return m_pExtension; }
		int GetFireDelay (void) const { return m_iFireRate; }
		FireTypes GetFireType (void) const { return m_iFireType; }
		SFragmentDesc *GetFirstFragment (void) const { return m_pFirstFragment; }
		Metric GetFragmentationMaxThreshold (void) const { return m_rMaxFragThreshold; }
		Metric GetFragmentationMinThreshold (void) const { return m_rMinFragThreshold; }
		int GetHitPoints (void) const { return m_iHitPoints; }
		int GetIdlePowerUse (void) const { return m_iIdlePowerUse; }
        const CObjectImageArray &GetImage (void) const { return GetOldEffects().Image; }
		int GetInitialDelay (void) const { return m_InitialDelay.Roll(); }
		Metric GetInitialSpeed (void) const;
		int GetInteraction (void) const { return m_iInteraction; }
        int GetLevel (void) const;
		int GetLifetime (void) const { return m_Lifetime.Roll(); }
		int GetManeuverRate (void) const { return m_iManeuverRate; }
		int GetMaxLifetime (void) const { return m_Lifetime.GetMaxValue(); }
		Metric GetMaxRadius (void) const { return m_rMaxRadius; }
		int GetMinDamage (void) const { return m_MinDamage.Roll(); }
		Metric GetMinRadius (void) const { return m_rMinRadius; }
        Metric GetMaxRange (void) const;
		CEffectCreator *GetParticleEffect (void) const;
		const CParticleSystemDesc *GetParticleSystemDesc (void) const { return m_pParticleDesc; }
		int GetPassthrough (void) const { return m_iPassthrough; }
		int GetPowerUse (void) const { return m_iPowerUse; }
		int GetProximityFailsafe (void) const { return m_iProximityFailsafe; }
		Metric GetRatedSpeed (void) const { return m_rMissileSpeed; }
        CWeaponFireDesc *GetScaledDesc (int iLevel) const;
        int GetSpecialDamage (SpecialDamageTypes iSpecial, DWORD dwFlags = 0) const;
        int GetStealth (void) const { return m_iStealth; }
        bool GetTargetable (void) const { return m_fTargetable; }
        FireTypes GetType (void) const { return m_iFireType; }
        const CString &GetUNID (void) const { return m_sUNID; }
        const SVaporTrailDesc &GetVaporTrail (void) const { return GetOldEffects().VaporTrail; }
		CItemType *GetWeaponType (CItemType **retpLauncher = NULL) const;
		bool HasEvents (void) const { return !m_Events.IsEmpty(); }
		bool HasFragments (void) const { return m_pFirstFragment != NULL; }
		bool HasFragmentInterval (int *retiInterval = NULL) const;
		bool HasOnFragmentEvent (void) const { return m_CachedEvents[evtOnFragment].pCode != NULL; }
		void InitFromDamage (const DamageDesc &Damage);
		ALERROR InitFromMissileXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pMissile, const SInitOptions &Options);
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const SInitOptions &Options);
        ALERROR InitScaledStats (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pItem, CWeaponClass *pWeapon);
		bool IsCurvedBeam (void) const { return false; }
        bool IsDirectionalImage (void) const { return m_fDirectional; }
        bool IsFragment (void) const { return m_fFragment; }
		bool IsMIRV (void) const { return (m_pFirstFragment ? m_pFirstFragment->bMIRV : false); }
        bool IsScalable (void) const { return (m_pScalable != NULL); }
		bool IsTargetRequired (void) const { return (m_fTargetRequired ? true : false); }
		bool IsTracking (void) const { return m_iManeuverability != 0; }
		bool IsTracking (const SShotCreateCtx &Ctx) const;
		bool IsTrackingOrHasTrackingFragments (void) const;
		bool IsTrackingTime (int iTick) const { return (m_iManeuverability > 0 && (iTick % m_iManeuverability) == 0); }
		void MarkImages (void);
		ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx);
		void PlayFireSound (CSpaceObject *pSource) { m_FireSound.PlaySound(pSource); }
		bool ProximityBlast (void) const { return (m_fProximityBlast ? true : false); }

	private:
        struct SOldEffects
            {
		    CObjectImageArray Image;			//	Image for missile
            SExhaustDesc Exhaust;				//  Exhaust effect
            SVaporTrailDesc VaporTrail;			//  Vapor trail effect
            };

        //  Copying this class is not supported

		CWeaponFireDesc (const CWeaponFireDesc &Desc) = delete;
        CWeaponFireDesc & operator= (const CWeaponFireDesc &Desc) = delete;

		int CalcDefaultHitPoints (void) const;
		int CalcDefaultInteraction (void) const;
        Metric CalcMaxEffectiveRange (void) const;
		static Metric CalcSpeed (Metric rPercentOfLight, bool bRelativistic);
		CEffectCreator *GetFireEffect (void) const;
        SOldEffects &GetOldEffects (void) const { return (m_pOldEffects ? *m_pOldEffects : m_NullOldEffects); }
		CUniverse &GetUniverse (void) const { return *g_pUniverse; }
        SOldEffects &SetOldEffects (void) { if (m_pOldEffects == NULL) m_pOldEffects = new SOldEffects; return *m_pOldEffects; }
		bool InitHitPoints (SDesignLoadCtx &Ctx, const CXMLElement &XMLDesc);
		bool InitLifetime (SDesignLoadCtx &Ctx, const CXMLElement &XMLDesc);
		bool InitMissileSpeed (SDesignLoadCtx &Ctx, const CXMLElement &XMLDesc);
        ALERROR InitScaledStats (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CWeaponClass *pWeapon, const CWeaponFireDesc &Src, int iBaseLevel, int iScaledLevel);

		static bool LoadFireType (const CString &sValue, FireTypes *retiFireType = NULL);

		CExtension *m_pExtension = NULL;		//	Extension that defines the weaponfiredesc
		CString m_sUNID;						//	Identification. The format is
												//		uuuuu/n/fi/e
												//
												//		uuuuu = weapon UNID
												//		n = shot data ordinal
												//		fi = fragment, i is index (optional)
												//		e = enhanced (optional)

		//	Basic properties
        int m_iLevel = 0;						//  Level of desc (missile or weapon or scalable weapon)
		CItemTypeRef m_pAmmoType;				//	item type for this ammo
		FireTypes m_iFireType = ftMissile;		//	beam or missile
		DamageDesc m_Damage;					//	Damage per shot
		int m_iContinuous = -1;					//	repeat for this number of frames (-1 = default)
		int m_iContinuousFireDelay = -1;		//	Ticks between continuous fire shots (-1 = default)
		int m_iFireRate = -1;					//	Ticks between shots (-1 = default to weapon class)
		int m_iPowerUse = -1;					//	Power use in 1/10th MWs (-1 = default to weapon class)
		int m_iIdlePowerUse = -1;				//	Power use while idle (-1 = default to weapon class)

		Metric m_rMissileSpeed = 0.0;			//	Speed of missile
		DiceRange m_MissileSpeed;				//	Speed of missile (if random)
		DiceRange m_Lifetime;					//	Lifetime of fire in seconds
		DiceRange m_InitialDelay;				//	Delay for n ticks before starting

		int m_iPassthrough = 0;					//	Chance that the missile will continue through target

        //  Computed properties
		Metric m_rMaxEffectiveRange = 0.0;		//	Max effective range of weapon

		//	Effects
		CEffectCreatorRef m_pEffect;			//	Effect for the actual bullet/missile/beam
		CEffectCreatorRef m_pHitEffect;			//	Effect when we hit/explode
		CEffectCreatorRef m_pFireEffect;		//	Effect when we fire (muzzle flash)
		CSoundRef m_FireSound;					//	Sound when weapon is fired
        SOldEffects *m_pOldEffects = NULL;		//  Non-painter effects.
		CWeaponFireDescRef m_pExplosionType;	//	Explosion to create when ship is destroyed

		//	Missile stuff (m_iFireType == ftMissile)
		int m_iAccelerationFactor = 0;			//	% increase in speed per 10 ticks
		Metric m_rMaxMissileSpeed = 0.0;		//	Max speed.
		int m_iStealth = 0;						//	Missile stealth
		int m_iHitPoints = 0;					//	HP before dissipating (0 = destroyed by any hit)
		int m_iInteraction = 0;					//	Interaction opacity (0-100)
		int m_iManeuverability = 0;				//	Tracking maneuverability (0 = none)
		int m_iManeuverRate = 0;				//	Angle turned at each maneuverability point

		//	Particles (m_iFireType == ftParticles)
		CParticleSystemDesc *m_pParticleDesc = NULL;
		DiceRange m_MinDamage;					//	Minimum damage when hit by particles

		//	Area stuff (m_iFireType == ftArea)
		DiceRange m_ExpansionSpeed;				//	Speed of expansion (% of lightspeed)
		DiceRange m_AreaDamageDensity;			//	Number of points along the edge

		//	Radius stuff (m_iFireType == ftRadius)
		Metric m_rMinRadius = 0.0;				//	All objects inside this radius take full damage
		Metric m_rMaxRadius = 0.0;				//	Damage inside this radius is decreased by inverse square law

		//	Fragmentation
		SFragmentDesc *m_pFirstFragment = NULL;	//	Pointer to first fragment desc (or NULL)
		int m_iProximityFailsafe = 0;			//	Min ticks before proximity is active
		DiceRange m_FragInterval;				//	If not empty, we keep fragmenting
		Metric m_rMaxFragThreshold = 0.0;		//	Max fragmentation distance
		Metric m_rMinFragThreshold = 0.0;		//	Min fragmentation distance

		//	Events
		CEventHandler m_Events;					//	Events
		SEventHandlerDesc m_CachedEvents[evtCount];

        //  Scalable items
        int m_iBaseLevel = 0;                   //  Base level
        int m_iScaledLevels = 0;                //  Number of scaled levels above base
        CWeaponFireDesc *m_pScalable = NULL;	//  Array of entries, one per scaled level above base

		//	Flags
		DWORD m_fVariableInitialSpeed:1;		//	TRUE if initial speed is random
		DWORD m_fNoFriendlyFire:1;				//	TRUE if we cannot hit our friends
		DWORD m_fNoWorldHits:1;					//	If TRUE, we never hit worlds (or stars)
		DWORD m_fNoImmutableHits:1;				//	If TRUE, we never hit immutable objects
		DWORD m_fNoStationHits:1;				//	If TRUE, we never hit station-scale objects
		DWORD m_fNoImmobileHits:1;				//	If TRUE, we never hit immobile objects
		DWORD m_fNoShipHits:1;					//	If TRUE, we never hit ship-scale objects
		DWORD m_fAutoTarget:1;					//	If TRUE, we can acquire new targets after launch

		DWORD m_fCanDamageSource;				//	TRUE if we can damage the source
		DWORD m_fDirectional:1;					//	True if different images for each direction
		DWORD m_fFragment:1;					//	True if this is a fragment of a proximity blast
		DWORD m_fProximityBlast;				//	This is TRUE if we have fragments or if we have
												//		and OnFragment event.
        DWORD m_fRelativisticSpeed:1;			//	If TRUE, adjust speed to simulate for light-lag
        DWORD m_fTargetRequired:1;				//	If TRUE, do not fragment unless we have a target
        DWORD m_fTargetable:1;					//	If TRUE, and type is 'missile', the weaponFire can be shot at by weapons with canTargetMissiles=true
        DWORD m_fDefaultInteraction:1;			//	If TRUE, compute default interaction at bind-time.

		DWORD m_fDefaultHitPoints:1;			//	If TRUE, computer hit points at bind-time.
		DWORD m_fSpare2:1;
		DWORD m_fSpare3:1;
		DWORD m_fSpare4:1;
		DWORD m_fSpare5:1;
		DWORD m_fSpare6:1;
		DWORD m_fSpare7:1;
		DWORD m_fSpare8:1;

		DWORD m_dwSpare:8;

        static SOldEffects m_NullOldEffects;
	};
