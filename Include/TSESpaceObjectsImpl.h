//	TSESpaceObjectsImpl.h
//
//	Transcendence CSpaceObject classes
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CAreaDamage : public CSpaceObject
	{
	public:
		static ALERROR Create (CSystem *pSystem, SShotCreateCtx &Ctx, CAreaDamage **retpObj);
		~CAreaDamage (void);

		//	CSpaceObject virtuals

		virtual CString GetDamageCauseNounPhrase (DWORD dwFlags) override { return m_Source.GetDamageCauseNounPhrase(dwFlags); }
		virtual const CDamageSource &GetDamageSource (void) const override { return m_Source; }
		virtual Metric GetMaxSpeed (void) override { return m_pDesc->GetAveExpansionSpeed(); }
		virtual CString GetNamePattern (DWORD dwNounPhraseFlags = 0, DWORD *retdwFlags = NULL) const override;
		virtual CString GetObjClassName (void) override { return CONSTLIT("CAreaDamage"); }
		virtual CSystem::LayerEnum GetPaintLayer (void) override { return CSystem::layerEffects; }
		virtual CSpaceObject *GetSecondarySource (void) override { return m_Source.GetSecondaryObj(); }
		virtual CSovereign *GetSovereign (void) const override { return m_pSovereign; }
		virtual CDesignType *GetType (void) const override { return m_pDesc->GetWeaponType(); }
		virtual CWeaponFireDesc *GetWeaponFireDesc (void) override { return m_pDesc; }
		virtual void OnMove (const CVector &vOldPos, Metric rSeconds) override;
		virtual void OnSystemLoaded (void) override;
		virtual void PaintLRSForeground (CG32bitImage &Dest, int x, int y, const ViewportTransform &Trans) override;
		virtual bool PointInObject (const CVector &vObjPos, const CVector &vPointPos) override;

	protected:
		//	Virtuals to be overridden
		virtual bool CanHit (CSpaceObject *pObj) override { return MissileCanHitObj(pObj, m_Source, m_pDesc); }
		virtual void ObjectDestroyedHook (const SDestroyCtx &Ctx) override;
		virtual EDamageResults OnDamage (SDamageCtx &Ctx) override { return damagePassthrough; }
		virtual void OnDestroyed (SDestroyCtx &Ctx) override;
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual void OnReadFromStream (SLoadCtx &Ctx) override;
		virtual void OnUpdate (SUpdateCtx &Ctx, Metric rSecondsPerTick) override;
		virtual void OnWriteToStream (IWriteStream *pStream) override;

	private:
		CAreaDamage (void);

		CWeaponFireDesc *m_pDesc;				//	Weapon descriptor
		TSharedPtr<CItemEnhancementStack> m_pEnhancements;	//	Stack of enhancements
		IEffectPainter *m_pPainter;				//	Effect painter
		int m_iInitialDelay;					//	Delay before start
		int m_iTick;							//	Counter
		int m_iLifeLeft;						//	Ticks left
		CDamageSource m_Source;					//	Object that fired the beam
		CSovereign *m_pSovereign;				//	Sovereign

	friend CObjectClass<CAreaDamage>;
	};

class CBeam : public CSpaceObject
	{
	public:
		CBeam (void);

		//	CSpaceObject virtuals

		virtual void CreateReflection (const CVector &vPos, int iDirection, CMissile **retpReflection = NULL) override;
		virtual Categories GetCategory (void) const override { return catBeam; }
		virtual CString GetDamageCauseNounPhrase (DWORD dwFlags) override { return m_Source.GetDamageCauseNounPhrase(dwFlags); }
		virtual const CDamageSource &GetDamageSource (void) const override { return m_Source; }
		virtual int GetInteraction (void) const override { return 0; }
		virtual Metric GetMaxSpeed (void) override { return m_pDesc->GetRatedSpeed(); }
		virtual CString GetNamePattern (DWORD dwNounPhraseFlags = 0, DWORD *retdwFlags = NULL) const override;
		virtual CString GetObjClassName (void) override { return CONSTLIT("CBeam"); }
		virtual CSystem::LayerEnum GetPaintLayer (void) override { return CSystem::layerStations; }
		virtual CSpaceObject *GetSecondarySource (void) override { return m_Source.GetSecondaryObj(); }
		virtual CSovereign *GetSovereign (void) const override { return m_pSovereign; }
		virtual CWeaponFireDesc *GetWeaponFireDesc (void) override { return m_pDesc; }
		virtual void OnMove (const CVector &vOldPos, Metric rSeconds) override;

	protected:
		//	Virtuals to be overridden
		virtual bool CanHit (CSpaceObject *pObj) override { return MissileCanHitObj(pObj, m_Source, m_pDesc); }
		virtual void ObjectDestroyedHook (const SDestroyCtx &Ctx) override;
		virtual EDamageResults OnDamage (SDamageCtx &Ctx) override { return damagePassthrough; }
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual void OnReadFromStream (SLoadCtx &Ctx) override;
		virtual void OnUpdate (SUpdateCtx &Ctx, Metric rSecondsPerTick) override;
		virtual void OnWriteToStream (IWriteStream *pStream) override;

	private:
		void ComputeOffsets (void);

		CWeaponFireDesc *m_pDesc;				//	Weapon descriptor
		int m_iBonus;							//	Bonus damage
		int m_iRotation;						//	Direction
		CVector m_vPaintTo;						//	Paint from old position to this location
		int m_iTick;							//	Ticks that we have been alive
		int m_iLifeLeft;						//	Ticks left
		CDamageSource m_Source;					//	Object that fired the beam
		CSovereign *m_pSovereign;				//	Sovereign
		CSpaceObject *m_pHit;					//	Object that was hit
		int m_iHitDir;							//	Direction from which object was hit

		int m_xFromOffset;						//	Offsets for painting (volatile)
		int m_yFromOffset;
		int m_xToOffset;
		int m_yToOffset;

		DWORD m_fReflection:1;					//	TRUE if this is a reflection
		DWORD m_dwSpareFlags:31;				//	Flags

	friend CObjectClass<CBeam>;
	};

class CBoundaryMarker : public CSpaceObject
	{
	public:
		static ALERROR Create (CSystem *pSystem,
							   const CVector &vStartPos,
							   CBoundaryMarker **retpMarker);

		void AddSegment (const CVector &vEndpoint);
		void CloseBoundary (void);
		bool FindIntersectSegment (const CVector &vStart, const CVector &vEnd, CVector *retvSegInt, CVector *retvSegEnd, int *retiSeg);
		void GetSegment (int iSeg, CVector *retvStart, CVector *retvEnd);

		//	CSpaceObject virtuals

		virtual CBoundaryMarker *AsBoundaryMarker (void) override { return this; }
		virtual bool IsAnchored (void) const override { return true; }

	protected:
		//	CSpaceObject virtuals
		virtual bool CanHit (CSpaceObject *pObj) override { return false; }
		virtual CString GetObjClassName (void) override { return CONSTLIT("CBoundaryMarker"); }
		virtual void OnReadFromStream (SLoadCtx &Ctx) override;
		virtual void OnWriteToStream (IWriteStream *pStream) override;
		virtual void PaintLRSForeground (CG32bitImage &Dest, int x, int y, const ViewportTransform &Trans) override;

	private:
		CBoundaryMarker (void);

		TArray<CVector> m_Path;

	friend CObjectClass<CBoundaryMarker>;
	};

class CContinuousBeam : public CSpaceObject
	{
	public:
		static ALERROR Create (CSystem *pSystem, SShotCreateCtx &Ctx, CContinuousBeam **retpObj);
		~CContinuousBeam (void);

		//	CSpaceObject virtuals

		virtual void AddContinuousBeam (const CVector &vPos, const CVector &vVel, int iDirection) override;
		virtual CString DebugCrashInfo (void) override;
		virtual CString GetDamageCauseNounPhrase (DWORD dwFlags) override { return m_Source.GetDamageCauseNounPhrase(dwFlags); }
		virtual const CDamageSource &GetDamageSource (void) const override { return m_Source; }
		virtual int GetInteraction (void) const override { return m_pDesc->GetInteraction(); }
		virtual Metric GetMaxSpeed (void) override { return m_pDesc->GetRatedSpeed(); }
		virtual CString GetNamePattern (DWORD dwNounPhraseFlags = 0, DWORD *retdwFlags = NULL) const override;
		virtual CString GetObjClassName (void) override { return CONSTLIT("CContinuousBeam"); }
		virtual CSystem::LayerEnum GetPaintLayer (void) override { return CSystem::layerEffects; }
		virtual int GetRotation (void) const override { return m_iLastDirection; }
		virtual CSpaceObject *GetSecondarySource (void) override { return m_Source.GetSecondaryObj(); }
		virtual CSovereign *GetSovereign (void) const override { return m_pSovereign; }
		virtual CDesignType *GetType (void) const override { return m_pDesc->GetWeaponType(); }
		virtual CWeaponFireDesc *GetWeaponFireDesc (void) override { return m_pDesc; }
		virtual void OnMove (const CVector &vOldPos, Metric rSeconds) override;
		virtual bool PointInObject (const CVector &vObjPos, const CVector &vPointPos) override;

	protected:
		//	Virtuals to be overridden
		virtual bool CanHit (CSpaceObject *pObj) override { return MissileCanHitObj(pObj, m_Source, m_pDesc); }
		virtual void ObjectDestroyedHook (const SDestroyCtx &Ctx) override;
		virtual EDamageResults OnDamage (SDamageCtx &Ctx) override { return damagePassthrough; }
		virtual void OnDestroyed (SDestroyCtx &Ctx) override;
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual void OnReadFromStream (SLoadCtx &Ctx) override;
		virtual void OnUpdate (SUpdateCtx &Ctx, Metric rSecondsPerTick) override;
		virtual void OnWriteToStream (IWriteStream *pStream) override;

	private:
		struct SSegment
			{
			SSegment (void) :
					fAlive(true),
					fHit(false),
					fPassthrough(false)
				{ 
				}

			CVector vPos;					//	Position of head of segment
			CVector vDeltaPos;				//	Change in position per tick
			DWORD dwGeneration;				//	Created on this tick
			int iDamage;					//	Damage in points
			TArray<DWORD> Hits;				//	Object ID of what we hit last tick
			
			DWORD fAlive:1;					//	Segment is still alive
			DWORD fHit:1;					//	We hit something last frame
			DWORD fPassthrough:1;			//	We passed through.
			DWORD dwSpare:29;
			};

		CContinuousBeam (void);
		void AddSegment (const CVector &vPos, const CVector &vVel, int iDamage);
		EDamageResults DoDamage (CSpaceObject *pHit, const CVector &vHitPos, int iHitDir);
		bool HitTestSegment (SSegment &Segment, CVector *retvHitPos);
		void PaintSegment (CG32bitImage &Dest, const CVector &vFrom, const CVector &vTo, SViewportPaintCtx &Ctx) const;
		void UpdateBeamMotion (Metric rSeconds, CVector *retvNewPos, Metric *retrMaxBoundsX, Metric *retrMaxBoundsY);

		//	This is the origin of the particle array in object coordinates. We 
		//	always use a fixed anchor because the motion of the particles 
		//	determines the position of the particle damage object (and not vice
		//	versa, as in SFXParticleSystem).
		//
		//	We use 0,0 as a fixed point, which works as long as we can still 
		//	convert kilometers to pixels and still be in range. We're good for
		//	at least 1,000 light-minutes. [But if we ever need to scale beyond 
		//	that, then we should set the origin to the original shot position.]

		inline const CVector &GetOrigin (void) const { return NullVector; }

		CWeaponFireDesc *m_pDesc;				//	Weapon descriptor
		TSharedPtr<CItemEnhancementStack> m_pEnhancements;	//	Stack of enhancements
		CSpaceObject *m_pTarget;				//	Target
		int m_iTick;							//	Counter
		int m_iLifetime;						//	Lifetime of any one segment
		int m_iLastDirection;					//	Most recent direction
		CDamageSource m_Source;					//	Object that fired the beam
		CSovereign *m_pSovereign;				//	Sovereign

		TArray<SSegment> m_Segments;			//	All beam segments
		TArray<CHitCtx> m_Hits;					//	Objects hit by this segment last tick

		IEffectPainter *m_pPainter;				//	Effect for beam

		DWORD m_fSpare1:1;
		DWORD m_fSpare2:1;
		DWORD m_fSpare3:1;
		DWORD m_fSpare4:1;
		DWORD m_fSpare5:1;
		DWORD m_fSpare6:1;
		DWORD m_fSpare7:1;
		DWORD m_fSpare8:1;
		DWORD m_dwSpare:24;

	friend CObjectClass<CContinuousBeam>;
	};

class CDisintegrationEffect : public CSpaceObject
	{
	public:
		static ALERROR Create (CSystem *pSystem,
				const CVector &vPos,
				const CVector &vVel,
				const CObjectImageArray &MaskImage,
				int iMaskTick,
				int iMaskRotation,
				CDisintegrationEffect **retpEffect);
		virtual ~CDisintegrationEffect (void);

		//	CSpaceObject virtuals

		virtual CString GetObjClassName (void) override { return CONSTLIT("CDisintegrationEffect"); }
		virtual CSystem::LayerEnum GetPaintLayer (void) override { return CSystem::layerEffects; }

	protected:

		//	CSpaceObject virtuals
		virtual void ObjectDestroyedHook (const SDestroyCtx &Ctx) override;
		virtual EDamageResults OnDamage (SDamageCtx &Ctx) override { return damagePassthrough; }
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual void OnReadFromStream (SLoadCtx &Ctx) override;
		virtual void OnUpdate (SUpdateCtx &Ctx, Metric rSecondsPerTick) override;
		virtual void OnUpdateExtended (const CTimeSpan &ExtraTime) override;
		virtual void OnWriteToStream (IWriteStream *pStream) override;
		virtual void OnMove (const CVector &vOldPos, Metric rSeconds) override;

	private:
		struct SParticle
			{
			int x;							//	Position in 256th of a pixel
			int y;
			int xV;							//	Velocity in 256th of a pixel
			int yV;							//	per tick
			int iTicksLeft;
			CG32bitPixel rgbColor;
			WORD wSpare;
			};

		CDisintegrationEffect (void);
		void InitParticle (SParticle *pParticle);

		int m_iTick;
		CObjectImageArray m_MaskImage;
		int m_iMaskTick;
		int m_iMaskRotation;

		//	particles
		int m_iParticleCount;
		SParticle *m_pParticles;

	friend CObjectClass<CDisintegrationEffect>;
	};

class CEffect : public CSpaceObject
	{
	public:
		static ALERROR Create (IEffectPainter *pPainter,
				CSystem *pSystem,
				CSpaceObject *pAnchor,
				const CVector &vPos,
				const CVector &vVel,
				int iRotation,
				CEffect **retpEffect = NULL);
		virtual ~CEffect (void);

		//	CSpaceObject virtuals

		virtual bool CanMove (void) const { return true; }
		virtual bool IsAnchored (void) const override { return (m_pAnchor != NULL); }
		virtual CString GetObjClassName (void) override { return CONSTLIT("CEffect"); }
		virtual CSystem::LayerEnum GetPaintLayer (void) override { return CSystem::layerEffects; }

	protected:

		//	CSpaceObject virtuals
		virtual void ObjectDestroyedHook (const SDestroyCtx &Ctx) override;
		virtual EDamageResults OnDamage (SDamageCtx &Ctx) override { return damagePassthrough; }
		virtual void OnMove (const CVector &vOldPos, Metric rSeconds) override;
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual void OnReadFromStream (SLoadCtx &Ctx) override;
		virtual void OnUpdate (SUpdateCtx &Ctx, Metric rSecondsPerTick) override;
		virtual void OnUpdateExtended (const CTimeSpan &ExtraTime) override;
		virtual void OnWriteToStream (IWriteStream *pStream) override;

	private:
		CEffect (void);

		CEffectCreator *GetEffectCreator (void);

		IEffectPainter *m_pPainter;
		CSpaceObject *m_pAnchor;
		int m_iLifetime;
		int m_iRotation;
		int m_iTick;

	friend CObjectClass<CEffect>;
	};

class CFractureEffect : public CSpaceObject
	{
	public:
		enum Style
			{
			styleExplosion,
			styleLinearSweep,
			};

		static ALERROR CreateExplosion (CSystem *pSystem,
				const CVector &vPos,
				const CVector &vVel,
				const CObjectImageArray &Image,
				int iImageTick,
				int iImageRotation,
				CFractureEffect **retpEffect);
		static ALERROR CreateLinearSweep(CSystem *pSystem,
			const CVector &vPos,
			const CVector &vVel,
			const CObjectImageArray &Image,
			int iImageTick,
			int iImageRotation,
			Metric rSweepDirection,
			CFractureEffect **retpEffect);
		virtual ~CFractureEffect (void);

		//	CSpaceObject virtuals

		virtual Categories GetCategory (void) const override { return catFractureEffect; }
		virtual CString GetObjClassName (void) override { return CONSTLIT("CFractureEffect"); }
		virtual CSystem::LayerEnum GetPaintLayer (void) override { return CSystem::layerEffects; }
		virtual void SetAttractor (CSpaceObject *pObj) override;

	protected:

		//	CSpaceObject virtuals
		virtual void ObjectDestroyedHook (const SDestroyCtx &Ctx) override;
		virtual EDamageResults OnDamage (SDamageCtx &Ctx) override { return damagePassthrough; }
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual void OnReadFromStream (SLoadCtx &Ctx) override;
		virtual void OnUpdate (SUpdateCtx &Ctx, Metric rSecondsPerTick) override;
		virtual void OnUpdateExtended (const CTimeSpan &ExtraTime) override;
		virtual void OnWriteToStream (IWriteStream *pStream) override;
		virtual void OnMove (const CVector &vOldPos, Metric rSeconds) override;

	private:
		struct SParticle
			{
			int x;							//	Position in 256th of a pixel
			int y;
			int xV;							//	Velocity in 256th of a pixel
			int yV;							//	per tick
			int iSleepTicks;				//	Stay still for this many ticks before moving

			int xSrc;						//	Position in source
			int ySrc;
			int iShape;						//	Index of shape (-1 = destroyed)
			};

		CFractureEffect (void);
		void InitParticleArray (void);

		int m_iTick;
		int m_iLifeTime;
		CObjectImageArray m_Image;
		int m_iImageTick;
		int m_iImageRotation;
		int m_iStyle;
		int m_iCellSize;

		int m_iParticleCount;
		SParticle *m_pParticles;

		CSpaceObject *m_pAttractor;

		double m_rSweepDirection;

	friend CObjectClass<CFractureEffect>;
	};

class CMarker : public CSpaceObject
	{
	public:
        ~CMarker (void);

		static ALERROR Create (CSystem *pSystem,
							   CSovereign *pSovereign,
							   const CVector &vPos,
							   const CVector &vVel,
							   const CString &sName,
							   CMarker **retpMarker);

        void SetOrbit (const COrbit &Orbit);

		//	CSpaceObject virtuals

		virtual Categories GetCategory (void) const override { return catMarker; }
		virtual const COrbit *GetMapOrbit (void) const override { return m_pMapOrbit; }
		virtual CString GetNamePattern (DWORD dwNounPhraseFlags = 0, DWORD *retdwFlags = NULL) const override { if (retdwFlags) *retdwFlags = 0; return m_sName; }
		virtual CSystem::LayerEnum GetPaintLayer (void) override { return CSystem::layerEffects; }
		virtual ICCItem *GetProperty (CCodeChainCtx &Ctx, const CString &sName) override;
		virtual bool IsAnchored (void) const override { return true; }
		virtual bool IsMarker (void) override { return true; }
		virtual void OnObjLeaveGate (CSpaceObject *pObj) override;
		virtual bool SetProperty (const CString &sName, ICCItem *pValue, CString *retsError) override;
        virtual bool ShowMapOrbit (void) const override { return (m_pMapOrbit != NULL); }

	protected:
		virtual bool CanHit (CSpaceObject *pObj) override { return false; }
		virtual CSovereign *GetSovereign (void) const override;
		virtual CString GetObjClassName (void) override { return CONSTLIT("CMarker"); }
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual void OnPaintMap (CMapViewportCtx &Ctx, CG32bitImage &Dest, int x, int y) override;
		virtual void OnReadFromStream (SLoadCtx &Ctx) override;
		virtual void OnWriteToStream (IWriteStream *pStream) override;
		virtual void PaintLRSForeground (CG32bitImage &Dest, int x, int y, const ViewportTransform &Trans) override { }

	private:
        enum EStyles
            {
            styleNone =                     0,  //  Invisible
            styleSmallCross =               1,  //  Paint small cross
            };

		CMarker (void);

		CString m_sName;						//	Name
		CSovereign *m_pSovereign;				//	Sovereign
        EStyles m_iStyle;                       //  Paint style

		COrbit *m_pMapOrbit;					//	Orbit to draw on map (may be NULL)

	friend CObjectClass<CMarker>;
	};

class CMissile : public CSpaceObject
	{
	public:
		static ALERROR Create (CSystem *pSystem, SShotCreateCtx &Ctx, CMissile **retpMissile);
		~CMissile (void);

		//	CSpaceObject virtuals

		virtual CMissile *AsMissile (void) override { return this; }
		virtual bool CanThrust (void) const { return (m_pDesc->GetManeuverRate() > 0); }
		virtual void CreateReflection (const CVector &vPos, int iDirection, CMissile **retpReflection = NULL) override;
		virtual CString DebugCrashInfo (void) override;
		virtual void DetonateNow (CSpaceObject *pHit) override;
        virtual CSpaceObject *GetBase (void) const override { return m_Source.GetObj(); }
		virtual Categories GetCategory (void) const override;
		virtual CString GetDamageCauseNounPhrase (DWORD dwFlags) override { return m_Source.GetDamageCauseNounPhrase(dwFlags); }
		virtual const CDamageSource &GetDamageSource (void) const override { return m_Source; }
		virtual int GetInteraction (void) const override { return m_pDesc->GetInteraction(); }
		virtual int GetLevel (void) const override { return m_pDesc->GetLevel(); }
		virtual Metric GetMaxSpeed (void) override { return m_pDesc->GetRatedSpeed(); }
		virtual CString GetNamePattern (DWORD dwNounPhraseFlags = 0, DWORD *retdwFlags = NULL) const override;
		virtual CString GetObjClassName (void) override { return CONSTLIT("CMissile"); }
		virtual CSystem::LayerEnum GetPaintLayer (void) override { return (m_pDesc->GetPassthrough() > 0 ? CSystem::layerEffects : CSystem::layerStations); }
		virtual ICCItem *GetProperty (CCodeChainCtx &Ctx, const CString &sName) override;
		virtual int GetRotation (void) const override { return m_iRotation; }
		virtual CSpaceObject *GetSecondarySource (void) override { return m_Source.GetSecondaryObj(); }
		virtual CSovereign *GetSovereign (void) const override { return m_pSovereign; }
		virtual int GetStealth (void) const override;
        virtual CSpaceObject *GetTarget (CItemCtx &ItemCtx, bool bNoAutoTarget = false) const override { return m_pTarget; }
		virtual CDesignType *GetType (void) const override { return m_pDesc->GetWeaponType(); }
		virtual CWeaponFireDesc *GetWeaponFireDesc (void) override { return m_pDesc; }
		virtual bool HasAttribute (const CString &sAttribute) const override;
		virtual bool IsAngryAt (CSpaceObject *pObj) const override;
		virtual bool IsInactive (void) const override { return (m_fDestroyOnAnimationDone ? true : false); }
		virtual bool IsIntangible (void) const { return ((m_fDestroyOnAnimationDone || IsDestroyed()) ? true : false); }
		virtual void OnMove (const CVector &vOldPos, Metric rSeconds) override;
		virtual void PaintLRSForeground (CG32bitImage &Dest, int x, int y, const ViewportTransform &Trans) override;
		virtual bool PointInObject (const CVector &vObjPos, const CVector &vPointPos) override;
		virtual bool SetProperty (const CString &sName, ICCItem *pValue, CString *retsError) override;

	protected:

		//	Virtuals to be overridden

		virtual bool CanHit (CSpaceObject *pObj) override { return MissileCanHitObj(pObj, m_Source, m_pDesc); }
		virtual void ObjectDestroyedHook (const SDestroyCtx &Ctx) override;
		virtual EDamageResults OnDamage (SDamageCtx &Ctx) override;
		virtual void OnDestroyed (SDestroyCtx &Ctx) override;
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual void OnReadFromStream (SLoadCtx &Ctx) override;
		virtual void OnUpdate (SUpdateCtx &Ctx, Metric rSecondsPerTick) override;
		virtual void OnWriteToStream (IWriteStream *pStream) override;

	private:
		struct SExhaustParticle
			{
			CVector vPos;				//	Position of particle
			CVector vVel;				//	Velocity of particle
			};

		CMissile (void);

		int ComputeVaporTrail (void);
		void CreateFragments (const CVector &vPos);
		int GetManeuverRate (void) const;
		bool IsTracking (void) const;
		bool IsTrackingTime (int iTick) const;
		bool SetMissileFade (void);

		CWeaponFireDesc *m_pDesc;				//	Weapon descriptor
		TSharedPtr<CItemEnhancementStack> m_pEnhancements;	//	Stack of enhancements
		int m_iLifeLeft;						//	Ticks left
		int m_iHitPoints;						//	HP left
		IEffectPainter *m_pPainter;				//	Effect painter
		CDamageSource m_Source;					//	Object that fired missile
		CSovereign *m_pSovereign;				//	Sovereign
		CSpaceObject *m_pHit;					//	Object hit
		CVector m_vHitPos;						//	Position hit
		int m_iHitDir;							//	Direction hit
		int m_iNextDetonation;					//	Detonate on this tick (-1 = none)
		int m_iRotation;						//	Current rotation (degrees)
		CSpaceObject *m_pTarget;				//	Target
		int m_iTick;							//	Number of ticks of life so far
		TQueue<SExhaustParticle> *m_pExhaust;	//	Array of exhaust particles
		CG16bitBinaryRegion *m_pVaporTrailRegions;	//	Array of vapor trail regions
		int m_iSavedRotationsCount;				//	Number of saved rotations
		int *m_pSavedRotations;					//	Array of saved rotation angles

		DWORD m_fDestroyOnAnimationDone:1;		//	TRUE if destroyed (only around to paint effect)
		DWORD m_fReflection:1;					//	TRUE if this is a reflection
		DWORD m_fDetonate:1;					//	TRUE if we should detonate on next update
		DWORD m_fPassthrough:1;					//	TRUE if shot passed through
		DWORD m_fPainterFade:1;					//	TRUE if we need to paint a fading painter
		DWORD m_fFragment:1;					//	TRUE if we're a fragment

		DWORD m_dwSpareFlags:26;				//	Flags

	friend CObjectClass<CMissile>;
	};

class CParticleDamage : public CSpaceObject
	{
	public:
		static ALERROR Create (CSystem *pSystem, SShotCreateCtx &Ctx, CParticleDamage **retpObj);
		~CParticleDamage (void);

		//	CSpaceObject virtuals

		virtual bool CanThrust (void) const { return (m_pDesc->GetManeuverRate() > 0); }
		virtual CString GetDamageCauseNounPhrase (DWORD dwFlags) override { return m_Source.GetDamageCauseNounPhrase(dwFlags); }
		virtual const CDamageSource &GetDamageSource (void) const override { return m_Source; }
		virtual Metric GetMaxSpeed (void) override { return m_pDesc->GetRatedSpeed(); }
		virtual CString GetNamePattern (DWORD dwNounPhraseFlags = 0, DWORD *retdwFlags = NULL) const override;
		virtual CString GetObjClassName (void) override { return CONSTLIT("CParticleDamage"); }
		virtual CSystem::LayerEnum GetPaintLayer (void) override { return CSystem::layerEffects; }
		virtual CSpaceObject *GetSecondarySource (void) override { return m_Source.GetSecondaryObj(); }
		virtual CSovereign *GetSovereign (void) const override { return m_pSovereign; }
		virtual CDesignType *GetType (void) const override { return m_pDesc->GetWeaponType(); }
		virtual CWeaponFireDesc *GetWeaponFireDesc (void) override { return m_pDesc; }
		virtual void OnMove (const CVector &vOldPos, Metric rSeconds) override;
		virtual bool PointInObject (const CVector &vObjPos, const CVector &vPointPos) override;

	protected:
		//	Virtuals to be overridden
		virtual bool CanHit (CSpaceObject *pObj) override { return MissileCanHitObj(pObj, m_Source, m_pDesc); }
		virtual void ObjectDestroyedHook (const SDestroyCtx &Ctx) override;
		virtual EDamageResults OnDamage (SDamageCtx &Ctx) override { return damagePassthrough; }
		virtual void OnDestroyed (SDestroyCtx &Ctx) override;
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual void OnReadFromStream (SLoadCtx &Ctx) override;
		virtual void OnUpdate (SUpdateCtx &Ctx, Metric rSecondsPerTick) override;
		virtual void OnWriteToStream (IWriteStream *pStream) override;

	private:
		CParticleDamage (void);

		//	This is the origin of the particle array in object coordinates. We 
		//	always use a fixed anchor because the motion of the particles 
		//	determines the position of the particle damage object (and not vice
		//	versa, as in SFXParticleSystem).
		//
		//	We use 0,0 as a fixed point, which works as long as we can still 
		//	convert kilometers to pixels and still be in range. We're good for
		//	at least 1,000 light-minutes. [But if we ever need to scale beyond 
		//	that, then we should set the origin to the original shot position.]

		inline const CVector &GetOrigin (void) const { return NullVector; }
		bool SetMissileFade (void);

		CWeaponFireDesc *m_pDesc;				//	Weapon descriptor
		TSharedPtr<CItemEnhancementStack> m_pEnhancements;	//	Stack of enhancements
		CSpaceObject *m_pTarget;				//	Target
		int m_iTick;							//	Counter
		int m_iLifeLeft;						//	Ticks left
		int m_iRotation;						//	Initial rotation
		int m_iEmitTime;						//	Ticks to emit new particles
		CDamageSource m_Source;					//	Object that fired the beam
		CSovereign *m_pSovereign;				//	Sovereign
		int m_iDamage;							//	Damage in points
		int m_iParticleCount;					//	Number of particles generated
		CVector m_vEmitSourcePos;				//	Emit source position
		CVector m_vEmitSourceVel;				//	Emit source velocity
		int m_iEmitDirection;					//	Emit direction

		IEffectPainter *m_pEffectPainter;		//	Additional effect
		IEffectPainter *m_pParticlePainter;		//	Painter to use for each particle
		CParticleArray m_Particles;

		DWORD m_fPainterFade:1;
		DWORD m_fSpare2:1;
		DWORD m_fSpare3:1;
		DWORD m_fSpare4:1;
		DWORD m_fSpare5:1;
		DWORD m_fSpare6:1;
		DWORD m_fSpare7:1;
		DWORD m_fSpare8:1;
		DWORD m_dwSpare:24;

	friend CObjectClass<CParticleDamage>;
	friend struct SParticle;
	};

class CParticleEffect : public CSpaceObject
	{
	public:
		enum ParticlePainting
			{
			paintDot,
			paintImage,
			paintSmoke,
			paintFlame,
			};

		struct SParticleType
			{
			SParticleType (void) :
					iPaintStyle(paintDot),
					rgbColor(CG32bitPixel(0xff, 0xff, 0xff)),
					iRegenerationTimer(0),
					iLifespan(0),
					rAveSpeed(0.0),
					iDirection(-1),
					iDirRange(0),
					rRadius(0.0),
					rHoleRadius(0.0),
					rDampening(1.0f),
					pDamageDesc(NULL),
					m_fMaxRadius(false),
					m_fLifespan(false),
					m_fWake(false),
					m_fRegenerate(false),
					m_fDrag(false),
					m_fFreeDesc(false)
				{ }

			~SParticleType (void)
				{
				if (m_fFreeDesc && pDamageDesc)
					delete pDamageDesc;
				}

			int iPaintStyle;			//	Painting style
			CObjectImageArray Image;	//	Image for each particle
			CG32bitPixel rgbColor;		//	Color of particle

			int iRegenerationTimer;		//	Max lifespan of entire group
			int iLifespan;				//	Max lifespan (in ticks)

			Metric rAveSpeed;			//	Average speed of particles
			int iDirection;				//	Direction (-1 == all directions)
			int iDirRange;				//	Span to either side (in degrees)

			Metric rRadius;				//	Radius of particle field
			Metric rHoleRadius;			//	Keep particles out of the center
			Metric rDampening;			//	Velocity dampening constant

			CWeaponFireDesc *pDamageDesc;	//	Damage caused to objects in field

			DWORD m_fMaxRadius:1;		//	TRUE if we should keep particles in radius
			DWORD m_fLifespan:1;		//	TRUE if particles have a lifespan
			DWORD m_fWake:1;			//	TRUE if particles are affected by passing objects
			DWORD m_fRegenerate:1;		//	TRUE if particles regenerate when they die
			DWORD m_fDrag:1;			//	TRUE if particles are subject to drag
			DWORD m_fFreeDesc:1;		//	TRUE if we own the pDesc structure
			};

		static ALERROR Create (CSystem *pSystem,
							   CXMLElement *pDesc,
							   const CVector &vPos,
							   const CVector &vVel,
							   CParticleEffect **retpEffect);
		static ALERROR CreateEmpty (CSystem *pSystem,
									CSpaceObject *pAnchor,
									const CVector &vPos,
									const CVector &vVel,
									CParticleEffect **retpEffect);
		static ALERROR CreateExplosion (CSystem *pSystem,
										CSpaceObject *pAnchor,
										const CVector &vPos,
										const CVector &vVel,
										int iCount,
										Metric rAveSpeed,
										int iTotalLifespan,
										int iParticleLifespan,
										const CObjectImageArray &Image,
										CParticleEffect **retpEffect);
		static ALERROR CreateGeyser (CSystem *pSystem,
									 CSpaceObject *pAnchor,
									 const CVector &vPos,
									 const CVector &vVel,
									 int iTotalLifespan,
									 int iParticleCount,
									 ParticlePainting iParticleStyle,
									 int iParticleLifespan,
									 Metric rAveSpeed,
									 int iDirection,
									 int iAngleWidth,
									 CParticleEffect **retpEffect);

		~CParticleEffect (void);

		void AddGroup (SParticleType *pType, int iCount);

		//	CSpaceObject virtuals

		virtual bool CanBeHitBy (const DamageDesc &Damage) override;
		virtual bool CanMove (void) const { return true; }
		virtual CString GetNamePattern (DWORD dwNounPhraseFlags = 0, DWORD *retdwFlags = NULL) const override { if (retdwFlags) *retdwFlags = 0; return m_sName; }
		virtual CString GetObjClassName (void) override { return CONSTLIT("CParticleEffect"); }
		virtual CSystem::LayerEnum GetPaintLayer (void) override { return CSystem::layerSpace; }
		virtual bool IsAnchored (void) const override { return (m_pAnchor != NULL); }
		virtual void PaintLRSForeground (CG32bitImage &Dest, int x, int y, const ViewportTransform &Trans) override;

	protected:

		//	Virtuals to be overridden
		virtual bool CanHit (CSpaceObject *pObj) override { return false; }
		virtual void ObjectDestroyedHook (const SDestroyCtx &Ctx) override;
		virtual EDamageResults OnDamage (SDamageCtx &Ctx) override;
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual void OnReadFromStream (SLoadCtx &Ctx) override;
		virtual void OnUpdate (SUpdateCtx &Ctx, Metric rSecondsPerTick) override;
		virtual void OnWriteToStream (IWriteStream *pStream) override;
		virtual void OnMove (const CVector &vOldPos, Metric rSeconds) override;
		virtual bool PointInObject (const CVector &vObjPos, const CVector &vPointPos) override;

	private:

		enum Constants
			{
			ctMaxParticleGroups =	8,
			ctMaxObjsInField =		16,
			};

		struct SParticle
			{
			inline bool IsValid (void) { return iLifeLeft != -1; }

			int iDestiny;
			int iLifeLeft;
			CVector vPos;
			CVector vVel;
			};

		struct SParticleArray
			{
			~SParticleArray (void)
				{
				if (pType)
					delete pType;

				if (pParticles)
					delete [] pParticles;
				}

			SParticleType *pType;
			int iAlive;
			int iCount;
			SParticle *pParticles;

			SParticleArray *pNext;
			};

		CParticleEffect (void);

		void CreateGroup (SParticleType *pType, int iCount, SParticleArray **retpGroup);
		void PaintFlameParticles (SParticleArray *pGroup, CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);
		void PaintSmokeParticles (SParticleArray *pGroup, CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx);
		void SetParticleSpeed (SParticleType *pType, SParticle *pParticle);

		CString m_sName;
		SParticleArray *m_pFirstGroup;
		CSpaceObject *m_pAnchor;

	friend CObjectClass<CParticleEffect>;
	};

class CPOVMarker : public CSpaceObject
	{
	public:
		static ALERROR Create (CSystem *pSystem,
							   const CVector &vPos,
							   const CVector &vVel,
							   CPOVMarker **retpMarker);

		virtual bool IsAnchored (void) const override { return true; }

	protected:
		virtual bool CanHit (CSpaceObject *pObj) override { return false; }
		virtual CSovereign *GetSovereign (void) const override;
		virtual void OnLosePOV (void) override;
		virtual CString GetObjClassName (void) override { return CONSTLIT("CPOVMarker"); }
		virtual void PaintLRSForeground (CG32bitImage &Dest, int x, int y, const ViewportTransform &Trans) override { }

	private:
		CPOVMarker (void);

	friend CObjectClass<CPOVMarker>;
	};

class CRadiusDamage : public CSpaceObject
	{
	public:
		static ALERROR Create (CSystem *pSystem, SShotCreateCtx &Ctx, CRadiusDamage **retpObj);
		~CRadiusDamage (void);

		//	CSpaceObject virtuals

		virtual CString DebugCrashInfo (void) override;
		virtual CString GetDamageCauseNounPhrase (DWORD dwFlags) override { return m_Source.GetDamageCauseNounPhrase(dwFlags); }
		virtual const CDamageSource &GetDamageSource (void) const override { return m_Source; }
		virtual CString GetNamePattern (DWORD dwNounPhraseFlags = 0, DWORD *retdwFlags = NULL) const override;
		virtual CString GetObjClassName (void) override { return CONSTLIT("CRadiusDamage"); }
		virtual CSystem::LayerEnum GetPaintLayer (void) override { return CSystem::layerEffects; }
		virtual CSpaceObject *GetSecondarySource (void) override { return m_Source.GetSecondaryObj(); }
		virtual CSovereign *GetSovereign (void) const override { return m_pSovereign; }
		virtual CDesignType *GetType (void) const override { return m_pDesc->GetWeaponType(); }
		virtual CWeaponFireDesc *GetWeaponFireDesc (void) override { return m_pDesc; }
		virtual void OnMove (const CVector &vOldPos, Metric rSeconds) override;
		virtual void OnSystemLoaded (void) override;
		virtual bool PointInObject (const CVector &vObjPos, const CVector &vPointPos) override;

	protected:
		//	Virtuals to be overridden
		virtual bool CanHit (CSpaceObject *pObj) override { return MissileCanHitObj(pObj, m_Source, m_pDesc); }
		virtual void ObjectDestroyedHook (const SDestroyCtx &Ctx) override;
		virtual EDamageResults OnDamage (SDamageCtx &Ctx) override { return damagePassthrough; }
		virtual void OnDestroyed (SDestroyCtx &Ctx) override;
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual void OnReadFromStream (SLoadCtx &Ctx) override;
		virtual void OnUpdate (SUpdateCtx &Ctx, Metric rSecondsPerTick) override;
		virtual void OnWriteToStream (IWriteStream *pStream) override;

	private:
		CRadiusDamage (void);

		void DamageAll (SUpdateCtx &Ctx);

		CWeaponFireDesc *m_pDesc;				//	Weapon descriptor
		TSharedPtr<CItemEnhancementStack> m_pEnhancements;	//	Stack of enhancements
		IEffectPainter *m_pPainter;				//	Effect painter
		int m_iTick;							//	Counter
		int m_iLifeLeft;						//	Ticks left
		CDamageSource m_Source;					//	Object that fired the shot
		CSpaceObject *m_pTarget;				//	Target
		CSovereign *m_pSovereign;				//	Sovereign

	friend CObjectClass<CRadiusDamage>;
	};

class CShip : public CSpaceObject
	{
	public:
		enum RemoveDeviceStatus
			{
			remOK					= 0,	//	Can remove this device
			remTooMuchCargo			= 1,	//	Can't remove cargo expansion (because too much cargo)
			remNotInstalled			= 2,	//	Device is not installed
			remReplaceOnly			= 3,	//	Device can be replaced but not removed
			remCannotRemove			= 4,	//	Custom reason
			};

		struct SAttachedSectionInfo
			{
			CShip *pObj;
			int iHP;						//	Total hit points (all armor segments)
			int iMaxHP;						//	Max hit points
			CVector vPos;					//	Origin is center of image, scaled to unit vector
			};

		static ALERROR CreateFromClass (CSystem *pSystem, 
										CShipClass *pClass, 
										IShipController *pController,
										CDesignType *pOverride,
										CSovereign *pSovereign,
										const CVector &vPos, 
										const CVector &vVel, 
										int iRotation, 
										SShipGeneratorCtx *pCtx,
										CShip **retpShip);
		virtual ~CShip (void);

		//	Orders
		inline void CancelCurrentOrder (void) { m_pController->CancelCurrentOrder(); }
		inline IShipController::OrderTypes GetCurrentOrder (CSpaceObject **retpTarget = NULL, IShipController::SData *retData = NULL) const { return m_pController->GetCurrentOrderEx(retpTarget, retData); }
		inline DWORD GetCurrentOrderData (void) { return m_pController->GetCurrentOrderData(); }

		//	Armor methods
		inline CInstalledArmor *GetArmorSection (int iSect) { return &m_Armor.GetSegment(iSect); }
		inline int GetArmorSectionCount (void) { return m_Armor.GetSegmentCount(); }
		int DamageArmor (int iSect, DamageDesc &Damage);
		void InstallItemAsArmor (CItemListManipulator &ItemList, int iSect);
		bool IsArmorDamaged (int iSect);
		bool IsArmorRepairable (int iSect);
		void RepairArmor (int iSect, int iHitPoints, int *retiHPRepaired = NULL);
		bool SetCursorAtArmor (CItemListManipulator &ItemList, int iSect) const;
		void UninstallArmor (CItemListManipulator &ItemList);

		//	Compartments
		virtual bool IsMultiHull (void) override { return m_pClass->GetInteriorDesc().IsMultiHull(); }

		void GetAttachedSectionInfo (TArray<SAttachedSectionInfo> &Result) const;
		inline bool HasAttachedSections (void) const { return m_fHasShipCompartments; }
		inline bool IsShipSection (void) const { return m_fShipCompartment; }
		void SetAsShipSection (CShip *pMain);

		//	Device methods
		int CalcDeviceSlotsInUse (int *retiWeaponSlots = NULL, int *retiNonWeapon = NULL) const;
		RemoveDeviceStatus CanRemoveDevice (const CItem &Item, CString *retsResult);
		void ClearAllTriggered (void);
		void DamageCargo (SDamageCtx &Ctx);
		void DamageDevice (CInstalledDevice *pDevice, SDamageCtx &Ctx);
		void DamageDrive (SDamageCtx &Ctx);
		void DisableAllDevices (void);
		void EnableDevice (int iDev, bool bEnable = true, bool bSilent = false);
		bool FindDeviceAtPos (const CVector &vPos, CInstalledDevice **retpDevice);
		DeviceNames GetDeviceNameForCategory (ItemCategories iCategory);
		int GetItemDeviceName (const CItem &Item) const;
		CItem GetNamedDeviceItem (DeviceNames iDev);
		bool HasNamedDevice (DeviceNames iDev) const;
		void InstallItemAsDevice (CItemListManipulator &ItemList, int iDeviceSlot = -1, int iSlotPosIndex = -1);
		bool IsDeviceSlotAvailable (ItemCategories iItemCat = itemcatNone, int *retiSlot = NULL);
		void ReadyFirstWeapon (void);
		void ReadyNextWeapon (int iDir = 1);
		void ReadyFirstMissile (void);
		void ReadyNextMissile (int iDir = 1);
		void RechargeItem (CItemListManipulator &ItemList, int iCharges);
		int GetMissileCount (void);
		ALERROR RemoveItemAsDevice (CItemListManipulator &ItemList);
		DeviceNames SelectWeapon (int iDev, int iVariant);
		void SetCursorAtDevice (CItemListManipulator &ItemList, int iDev);
		void SetCursorAtNamedDevice (CItemListManipulator &ItemList, DeviceNames iDev);
		void SetWeaponTriggered (DeviceNames iDev, bool bTriggered = true);
		void SetWeaponTriggered (CInstalledDevice *pWeapon, bool bTriggered = true);
		CDeviceClass *GetNamedDeviceClass (DeviceNames iDev);
		bool GetWeaponIsReady (DeviceNames iDev);
        Metric GetWeaponRange (DeviceNames iDev);
		bool IsWeaponAligned (DeviceNames iDev, CSpaceObject *pTarget, int *retiAimAngle = NULL, int *retiFireAngle = NULL, int *retiFacingAngle = NULL);

		//	Settings
		CAbilitySet &GetNativeAbilities (void) { return m_Abilities; }
		inline bool HasAutopilot (void) { return (GetAbility(ablAutopilot) == ablInstalled); }

		void ClearBlindness (bool bNoMessage = false);

		void ClearDisarmed (void);
		void MakeDisarmed (int iTickCount = -1);

		void ClearLRSBlindness (void);
		inline bool IsLRSBlind (void) { return m_fLRSDisabledByNebula || (m_iLRSBlindnessTimer != 0); }
		void MakeLRSBlind (int iTickCount = -1);

		void ClearParalyzed (void);

		bool IsRadiationImmune (void);

		inline bool IsInGate (void) const { return m_iExitGateTimer > 0; }
		void SetInGate (CSpaceObject *pGate, int iTickCount);

		inline bool CanTargetFriendlies (void) const { return (GetAbility(ablFriendlyFireLock) != ablInstalled); }
		inline bool HasTargetingComputer (void) const { return (GetAbility(ablTargetingSystem) == ablInstalled); }

		inline void ClearSRSEnhanced (void) { SetAbility(ablExtendedScanner, ablRemove, -1, 0); }
		inline bool IsSRSEnhanced (void) { return (GetAbility(ablExtendedScanner) == ablInstalled); }
		inline void SetSRSEnhanced (void) { SetAbility(ablExtendedScanner, ablInstall, -1, 0); }

		//	Reactor methods
		inline Metric GetFuelLeft (void) { return (m_pPowerUse ? m_pPowerUse->GetFuelLeft() : 0.0); }
		Metric GetMaxFuel (void) const;
		inline const CReactorDesc &GetReactorDesc (void) const { return m_Perf.GetReactorDesc(); }
		void GetReactorStats (SReactorStats &Stats) const;
		void TrackFuel (bool bTrack = true);
		inline void TrackMass (bool bTrack = true) { m_fTrackMass = bTrack; }
		int GetPowerConsumption (void);
		bool IsFuelCompatible (const CItem &Item);

		//	Drive methods
		inline int Angle2Direction (int iAngle) const { return m_pClass->Angle2Direction(iAngle); }
		inline int AlignToRotationAngle (int iAngle) const { return m_pClass->AlignToRotationAngle(iAngle); }
		inline int GetRotationAngle (void) { return m_pClass->GetRotationAngle(); }
        inline const CIntegralRotationDesc &GetRotationDesc (void) const { return m_Perf.GetRotationDesc(); }
		inline int GetRotationRange (void) { return m_pClass->GetRotationRange(); }
		inline const CIntegralRotation &GetRotationState (void) const { return m_Rotation; }
		inline EManeuverTypes GetManeuverToFace (int iAngle) const { return m_Rotation.GetManeuverToFace(m_Perf.GetRotationDesc(), iAngle); }
		inline Metric GetThrust (void) const { return m_Perf.GetDriveDesc().GetThrust(); }
		inline Metric GetThrustProperty (void) const { return m_Perf.GetDriveDesc().GetThrustProperty(); }
		Metric GetMaxAcceleration (void);
		inline bool IsInertialess (void) { return m_Perf.GetDriveDesc().IsInertialess(); }
		inline bool IsMainDriveDamaged (void) const { return m_iDriveDamagedTimer != 0; }
		inline bool IsPointingTo (int iAngle) { return m_Rotation.IsPointingTo(m_Perf.GetRotationDesc(), iAngle); }
		inline void SetMaxSpeedEmergency (void) { m_fHalfSpeed = false; m_fEmergencySpeed = true; m_fQuarterSpeed = false; CalcPerformance(); }
        inline void SetMaxSpeedHalf (void) { m_fHalfSpeed = true; m_fEmergencySpeed = false; m_fQuarterSpeed = false; CalcPerformance(); }
        inline void SetMaxSpeedQuarter (void) { m_fHalfSpeed = false; m_fEmergencySpeed = false; m_fQuarterSpeed = true; CalcPerformance(); }
        inline void ResetMaxSpeed (void) { m_fHalfSpeed = false; m_fEmergencySpeed = false; m_fQuarterSpeed = false; CalcPerformance(); }

		//	Miscellaneous
		inline IShipController *GetController (void) { return m_pController; }
		inline CShipClass *GetClass (void) { return m_pClass; }
		void SetController (IShipController *pController, bool bFreeOldController = true);
		inline void SetControllerEnabled (bool bEnabled = true) { m_fControllerDisabled = !bEnabled; }
		void SetCommandCode (ICCItem *pCode);
		inline void SetDestroyInGate (void) { m_fDestroyInGate = true; }
		inline void SetEncounterInfo (CStationType *pEncounterInfo) { m_pEncounterInfo = pEncounterInfo; }
		inline void SetPlayerWingman (bool bIsWingman) const { m_pController->SetPlayerWingman(bIsWingman); }
		inline void SetRotation (int iAngle) { m_Rotation.SetRotationAngle(m_Perf.GetRotationDesc(), iAngle); }
		void Undock (void);

		//	CSpaceObject virtuals

		virtual bool AbsorbWeaponFire (CInstalledDevice *pWeapon) override;
		virtual void AddOverlay (COverlayType *pType, int iPosAngle, int iPosRadius, int iRotation, int iLifetime, DWORD *retdwID = NULL) override;
		virtual CTradingDesc *AllocTradeDescOverride (void) override;
		virtual CShip *AsShip (void) override { return this; }
		virtual void Behavior (SUpdateCtx &Ctx) override;
		virtual bool CanAttack (void) const override;
		virtual bool CanInstallItem (const CItem &Item, int iSlot = -1, InstallItemResults *retiResult = NULL, CString *retsResult = NULL, CItem *retItemToReplace = NULL) override;
		virtual bool CanMove (void) const override { return true; }
		virtual RequestDockResults CanObjRequestDock (CSpaceObject *pObj = NULL) const override;
		virtual bool CanThrust (void) const override { return (GetThrust() > 0.0); }
		virtual bool ClassCanAttack (void) override { return true; }
		virtual void ConsumeFuel (Metric rFuel, CReactorDesc::EFuelUseTypes iUse = CReactorDesc::fuelConsume) override;
		virtual void DamageExternalDevice (int iDev, SDamageCtx &Ctx) override;
		virtual void DeactivateShields (void) override;
		virtual CString DebugCrashInfo (void) override;
		virtual void Decontaminate (void) override;
		virtual void DepleteShields (void) override;
		virtual void DisableDevice (CInstalledDevice *pDevice) override;
		virtual CInstalledArmor *FindArmor (const CItem &Item) override;
		virtual bool FindDataField (const CString &sField, CString *retsValue) override;
		virtual CInstalledDevice *FindDevice (const CItem &Item) override;
		virtual bool FindDeviceSlotDesc (const CItem &Item, SDeviceDesc *retDesc) override { return m_pClass->FindDeviceSlotDesc(this, Item, retDesc); }
		virtual bool FollowsObjThroughGate (CSpaceObject *pLeader = NULL) override;
		virtual AbilityStatus GetAbility (Abilities iAbility) const override;
		virtual int GetAISettingInteger (const CString &sSetting) override { return m_pController->GetAISettingInteger(sSetting); }
		virtual CString GetAISettingString (const CString &sSetting) override { return m_pController->GetAISettingString(sSetting); }
		virtual CArmorSystem *GetArmorSystem (void) override { return &m_Armor; }
		virtual CSpaceObject *GetAttachedRoot (void) const;
		virtual CSpaceObject *GetBase (void) const override;
		virtual Metric GetCargoSpaceLeft (void) override;
		virtual Categories GetCategory (void) const override { return catShip; }
		virtual CDesignType *GetCharacter (void) const override { return m_pCharacter; }
		virtual DWORD GetClassUNID (void) override { return m_pClass->GetUNID(); }
		virtual int GetCombatPower (void) override;
		virtual CCurrencyBlock *GetCurrencyBlock (bool bCreate = false) override;
		virtual int GetCyberDefenseLevel (void) override { return m_pClass->GetCyberDefenseLevel(); }
		virtual int GetDamageEffectiveness (CSpaceObject *pAttacker, CInstalledDevice *pWeapon) override;
		virtual DamageTypes GetDamageType (void) override;
		virtual DWORD GetDefaultBkgnd (void) override { return m_pClass->GetDefaultBkgnd(); }
		virtual CSpaceObject *GetDestination (void) const override { return m_pController->GetDestination(); }
		virtual CSpaceObject *GetDockedObj (void) const override { return (m_fShipCompartment ? NULL : m_pDocked); }
		virtual CDockingPorts *GetDockingPorts (void) override { return &m_DockingPorts; }
		virtual CInstalledDevice *GetDevice (int iDev) override { return &m_Devices.GetDevice(iDev); }
		virtual int GetDeviceCount (void) const override { return m_Devices.GetCount(); }
		virtual CDeviceSystem *GetDeviceSystem (void) { return &m_Devices; }
		virtual CVector GetDockingPortOffset (int iRotation) override { return m_pClass->GetDockingPortOffset(iRotation); }
		virtual CStationType *GetEncounterInfo (void) override { return m_pEncounterInfo; }
		virtual CSpaceObject *GetEscortPrincipal (void) const override;
        virtual const CObjectImageArray &GetHeroImage (void) const override { return m_pClass->GetHeroImage(); }
		virtual const CObjectImageArray &GetImage (void) const override;
		virtual CString GetInstallationPhrase (const CItem &Item) const override;
		virtual Metric GetInvMass (void) const override;
		virtual int GetLastFireTime (void) const override { return m_iLastFireTime; }
		virtual int GetLastHitTime (void) const override { return m_iLastHitTime; }
		virtual int GetLevel (void) const override { return m_pClass->GetLevel(); }
		virtual Metric GetMass (void) const override;
		virtual int GetMaxPower (void) const override;
		virtual CString GetNamePattern (DWORD dwNounPhraseFlags = 0, DWORD *retdwFlags = NULL) const override;
		virtual const CInstalledDevice *GetNamedDevice (DeviceNames iDev) const override;
		virtual CInstalledDevice *GetNamedDevice (DeviceNames iDev) override;
		virtual CString GetObjClassName (void) override { return CONSTLIT("CShip"); }
		virtual COverlayList *GetOverlays (void) override { return &m_Overlays; }
		virtual const COverlayList *GetOverlays (void) const override { return &m_Overlays; }
		virtual CSystem::LayerEnum GetPaintLayer (void) override { return (m_fShipCompartment ? CSystem::layerOverhang : CSystem::layerShips); }
		virtual int GetPerception (void) override;
		virtual ICCItem *GetProperty (CCodeChainCtx &Ctx, const CString &sName) override;
		virtual int GetRotation (void) const override { return m_Rotation.GetRotationAngle(m_Perf.GetRotationDesc()); }
		virtual ScaleTypes GetScale (void) const override { return scaleShip; }
		virtual int GetScore (void) override { return m_pClass->GetScore(); }
		virtual CXMLElement *GetScreen (const CString &sName) override { return m_pClass->GetScreen(sName); }
		virtual int GetShieldLevel (void) override;
		virtual const CShipPerformanceDesc &GetShipPerformance (void) const override { return m_Perf; }
		virtual CSovereign *GetSovereign (void) const override { return m_pSovereign; }
		virtual int GetStealth (void) const override;
		virtual Metric GetMaxSpeed (void) override { return m_Perf.GetDriveDesc().GetMaxSpeed(); }
		virtual Metric GetMaxWeaponRange (void) const override;
		virtual CSpaceObject *GetTarget (CItemCtx &ItemCtx, bool bNoAutoTarget = false) const override;
		virtual CTradingDesc *GetTradeDescOverride (void) override { return m_pTrade; }
		virtual CCurrencyAndValue GetTradePrice (CSpaceObject *pProvider) override;
		virtual CDesignType *GetType (void) const override { return m_pClass; }
		virtual int GetVisibleDamage (void) override;
		virtual void GetVisibleDamageDesc (SVisibleDamage &Damage) override;
		virtual bool HasAttribute (const CString &sAttribute) const override;
		virtual bool ImageInObject (const CVector &vObjPos, const CObjectImageArray &Image, int iTick, int iRotation, const CVector &vImagePos) override;
		virtual bool IsAnchored (void) const override { return (GetDockedObj() != NULL) || IsManuallyAnchored(); }
		virtual bool IsAngryAt (CSpaceObject *pObj) const override;
		virtual bool IsAttached (void) const override { return m_fShipCompartment; }
		virtual bool IsBlind (void) const override { return m_iBlindnessTimer != 0; }
		virtual bool IsDisarmed (void) override { return m_fDisarmedByOverlay || m_iDisarmedTimer != 0; }
		virtual bool IsEscortingPlayer (void) const override;
		virtual bool IsHidden (void) const override { return (m_fManualSuspended || m_iExitGateTimer > 0); }
		virtual bool IsIdentified (void) override { return m_fIdentified; }
		virtual bool IsInactive (void) const override { return (m_fManualSuspended || m_iExitGateTimer > 0); }
		virtual bool IsIntangible (void) const { return (m_fManualSuspended || m_iExitGateTimer > 0 || IsDestroyed() || IsVirtual()); }
		virtual bool IsKnown (void) override { return m_fKnown; }
		virtual bool IsOutOfPower (void) override { return (m_pPowerUse && (m_pPowerUse->IsOutOfPower() || m_pPowerUse->IsOutOfFuel())); }
		virtual bool IsParalyzed (void) override { return m_fParalyzedByOverlay || m_iParalysisTimer != 0; }
		virtual bool IsPlayer (void) const override;
		virtual bool IsPlayerWingman (void) const override { return m_pController->IsPlayerWingman(); }
		virtual bool IsRadioactive (void) override { return (m_fRadioactive ? true : false); }
		virtual bool IsShownInGalacticMap (void) const override { return m_pClass->HasDockingPorts(); }
		virtual bool IsSuspended (void) const override { return m_fManualSuspended; }
		virtual bool IsTimeStopImmune (void) override { return m_pClass->IsTimeStopImmune(); }
		virtual bool IsVirtual (void) const override { return m_pClass->IsVirtual(); }
		virtual void MarkImages (void) override;
		virtual bool ObjectInObject (const CVector &vObj1Pos, CSpaceObject *pObj2, const CVector &vObj2Pos) override;
		virtual void OnAscended (void) override;
		virtual void OnBounce (CSpaceObject *pBarrierObj, const CVector &vPos) override;
		virtual void OnComponentChanged (ObjectComponentTypes iComponent) override;
		virtual void OnDeviceStatus (CInstalledDevice *pDev, CDeviceClass::DeviceNotificationTypes iEvent) override;
		virtual bool OnDestroyCheck (DestructionTypes iCause, const CDamageSource &Attacker) override;
		virtual void OnDocked (CSpaceObject *pObj) override;
		virtual void OnDockedObjChanged (CSpaceObject *pLocation) override;
		virtual void OnDockingPortDestroyed (void) override;
		virtual bool OnGateCheck (CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pGateObj) override;
		virtual void OnHitByDeviceDamage (void) override;
		virtual void OnHitByDeviceDisruptDamage (DWORD dwDuration) override;
		virtual void OnHitByRadioactiveDamage (SDamageCtx &Ctx) override;
		virtual void OnItemEnhanced (CItemListManipulator &ItemList) override;
		virtual void OnMissionCompleted (CMission *pMission, bool bSuccess) override { m_pController->OnMissionCompleted(pMission, bSuccess); }
		virtual void OnMove (const CVector &vOldPos, Metric rSeconds) override;
		virtual void OnNewSystem (CSystem *pSystem) override;
		virtual void OnObjDamaged (SDamageCtx &Ctx) override { m_pController->OnObjDamaged(Ctx); }
		virtual void OnPlayerChangedShips (CSpaceObject *pOldShip, SPlayerChangedShipsCtx &Options) override;
		virtual void OnPlayerObj (CSpaceObject *pPlayer) override;
		virtual void OnStationDestroyed (const SDestroyCtx &Ctx) override;
		virtual void OnSystemCreated (SSystemCreateCtx &CreateCtx) override;
		virtual void OnSystemLoaded (void) override;
		virtual void MakeBlind (int iTickCount = -1) override;
		virtual void MakeParalyzed (int iTickCount = -1) override;
		virtual void MakeRadioactive (void) override;
		virtual void PaintLRSBackground (CG32bitImage &Dest, int x, int y, const ViewportTransform &Trans) override;
		virtual void PaintLRSForeground (CG32bitImage &Dest, int x, int y, const ViewportTransform &Trans) override;
		virtual bool PointInObject (const CVector &vObjPos, const CVector &vPointPos) override;
		virtual bool PointInObject (SPointInObjectCtx &Ctx, const CVector &vObjPos, const CVector &vPointPos) override;
		virtual void PointInObjectInit (SPointInObjectCtx &Ctx) override;
		virtual void ProgramDamage (CSpaceObject *pHacker, const ProgramDesc &Program) override;
		virtual void RefreshBounds (void) override { CalcBounds(); }
		virtual void Refuel (Metric rFuel) override;
		virtual void Refuel (const CItem &Fuel) override;
		virtual void RemoveOverlay (DWORD dwID) override;
		virtual void RepairDamage (int iHitPoints) override;
		virtual void Resume (void) override { m_fManualSuspended = false; if (!IsInGate()) ClearCannotBeHit(); m_pController->OnStatsChanged(); }
		virtual void SendMessage (CSpaceObject *pSender, const CString &sMsg) override;
		virtual bool SetAbility (Abilities iAbility, AbilityModifications iModification, int iDuration, DWORD dwOptions) override;
		virtual int SetAISettingInteger (const CString &sSetting, int iValue) override { return m_pController->SetAISettingInteger(sSetting, iValue); }
		virtual CString SetAISettingString (const CString &sSetting, const CString &sValue) override { return m_pController->SetAISettingString(sSetting, sValue); }
		virtual void SetFireDelay (CInstalledDevice *pWeapon, int iDelay = -1) override;
		virtual void SetIdentified (bool bIdentified = true) override { m_fIdentified = bIdentified; }
        virtual void SetKnown (bool bKnown = true) override { m_fKnown = bKnown; }
		virtual void SetName (const CString &sName, DWORD dwFlags = 0) override;
		virtual bool SetProperty (const CString &sName, ICCItem *pValue, CString *retsError) override;
		virtual void Suspend (void) override { Undock(); m_fManualSuspended = true; SetCannotBeHit(); }
		virtual void Undock (CSpaceObject *pObj) override;
		virtual void UpdateArmorItems (void) override;
		void UpdateNoFriendlyFire(void);

	protected:

		//	CSpaceObject virtuals
		virtual bool CanFireOn (CSpaceObject *pObj) override { return CanFireOnObjHelper(pObj); }
		virtual void GateHook (CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate, bool bAscend) override;
		virtual CDesignType *GetDefaultDockScreen (CString *retsName = NULL) const override;
		virtual void ObjectDestroyedHook (const SDestroyCtx &Ctx) override;
		virtual DWORD OnCommunicate (CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2) override;
		virtual EDamageResults OnDamage (SDamageCtx &Ctx) override;
		virtual void OnDestroyed (SDestroyCtx &Ctx) override;
		virtual CSpaceObject *OnGetOrderGiver (void) override;
		virtual void OnObjEnteredGate (CSpaceObject *pObj, CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate) override;
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual void OnPaintAnnotations (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual void OnPaintMap (CMapViewportCtx &Ctx, CG32bitImage &Dest, int x, int y) override;
		virtual void OnPaintSRSEnhancements (CG32bitImage &Dest, SViewportPaintCtx &Ctx) override { m_pController->OnPaintSRSEnhancements(Dest, Ctx); }
		virtual void OnPlace (const CVector &vOldPos) override;
		virtual void OnReadFromStream (SLoadCtx &Ctx) override;
		virtual void OnRemoved (SDestroyCtx &Ctx) override;
		virtual void OnSetEventFlags (void) override;
        virtual void OnSetSovereign (CSovereign *pSovereign) override { m_pSovereign = pSovereign; }
		virtual void OnUpdate (SUpdateCtx &Ctx, Metric rSecondsPerTick) override;
		virtual void OnUpdatePlayer (SUpdateCtx &Ctx) override { m_pController->OnUpdatePlayer(Ctx); }
		virtual void OnWriteToStream (IWriteStream *pStream) override;
		virtual bool OrientationChanged (void) override;
		virtual void RevertOrientationChange (void) override;

	private:
		CShip (void);

		void AccumulateDeviceEnhancementsToArmor (CInstalledArmor *pArmor, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements);
		void CalcArmorBonus (void);
		void CalcBounds (void);
		int CalcMaxCargoSpace (void) const;
		void CalcDeviceBonus (void);
		bool CalcDeviceTarget (STargetingCtx &Ctx, CItemCtx &ItemCtx, CSpaceObject **retpTarget, int *retiFireSolution);
		InstallItemResults CalcDeviceToReplace (const CItem &Item, int iSuggestedSlot, int *retiSlot = NULL);
		DWORD CalcEffectsMask (void);
		void CalcOverlayImpact (void);
        void CalcPerformance (void);
		int CalcPowerUsed (SUpdateCtx &Ctx, int *retiPowerGenerated = NULL);
		void CreateExplosion (SDestroyCtx &Ctx);
		int FindDeviceIndex (CInstalledDevice *pDevice) const;
		int FindFreeDeviceSlot (void);
		bool FindInstalledDeviceSlot (const CItem &Item, int *retiDev = NULL);
		int FindNextDevice (int iStart, ItemCategories Category, int iDir = 1);
		int FindRandomDevice (bool bEnabledOnly = false);
		void FinishCreation (SShipGeneratorCtx *pCtx = NULL, SSystemCreateCtx *pSysCreateCtx = NULL);
		Metric GetCargoMass (void);
		CCurrencyAndValue GetHullValue (void) const;
		Metric GetItemMass (void) const;
		int GetTotalArmorHP (int *retiMaxHP = NULL) const;
		bool IsSingletonDevice (ItemCategories iItemCat);
		void PaintMapShipCompartments (CG32bitImage &Dest, int x, int y, CMapViewportCtx &Ctx);
		void PaintShipCompartments (CG32bitImage &Dest, SViewportPaintCtx &Ctx);
		void ReactorOverload (int iPowerDrain);
        ALERROR ReportCreateError (const CString &sError) const;
		void SetOrdersFromGenerator (SShipGeneratorCtx &Ctx);
		void SetTotalArmorHP (int iNewHP);
		inline bool ShowParalyzedEffect (void) const { return (m_iParalysisTimer != 0 || m_iDisarmedTimer > 0 || m_fDeviceDisrupted); }
		void UpdateDestroyInGate (void);
		bool UpdateFuel (SUpdateCtx &Ctx, int iTick);
		void UpdateInactive (void);

		CShipClass *m_pClass;					//	Ship class
		IShipController *m_pController;			//	Controller
		CSovereign *m_pSovereign;				//	Allegiance
		CString m_sName;						//	Ship's name
		DWORD m_dwNameFlags;					//	Name flags
		CString m_sMapLabel;					//	Map label

		CArmorSystem m_Armor;		            //	Array of CInstalledArmor
		CDeviceSystem m_Devices;				//	Array of CInstalledDevice
		CIntegralRotation m_Rotation;			//	Ship rotation
		CObjectEffectList m_Effects;			//	List of effects to paint
		CShipInterior m_Interior;				//	Interior decks and compartments (optionally)
		COverlayList m_Overlays;		        //	List of energy fields

		CGenericType *m_pCharacter;				//	Character (may be NULL)
		CAbilitySet m_Abilities;				//	Installed abilities
		CDockingPorts m_DockingPorts;			//	Docking ports (optionally)
		CStationType *m_pEncounterInfo;			//	Pointer back to encounter type (generally NULL)
		CTradingDesc *m_pTrade;					//	Override of trading desc (may be NULL)
		CCurrencyBlock *m_pMoney;				//	Store of money (may be NULL)
		CPowerConsumption *m_pPowerUse;			//	Power consumption variables (may be NULL if not tracking fuel)

		int m_iFireDelay:16;					//	Ticks until next fire
		int m_iMissileFireDelay:16;				//	Ticks until next missile fire
		int m_iContaminationTimer:16;			//	Ticks left to live
		int m_iBlindnessTimer:16;				//	Ticks until blindness wears off
												//	(-1 = permanent)
		int m_iParalysisTimer:16;				//	Ticks until paralysis wears off
												//	(-1 = permanent)
		int m_iExitGateTimer:16;				//	Ticks until ship exits gate (in gate until then)
		int m_iDisarmedTimer:16;				//	Ticks until ship can use weapons
												//	(-1 = permanent)
		int m_iLRSBlindnessTimer:16;			//	Ticks until LRS blindness wears off
												//	(-1 = permanent)
		int m_iDriveDamagedTimer:16;			//	Ticks until drive repaired
												//	(-1 = permanent)
		int m_iLastFireTime;					//	Tick when we last fired a weapon
		int m_iLastHitTime;						//	Tick when we last got hit by something

		mutable Metric m_rItemMass;				//	Total mass of all items (including installed)
		mutable Metric m_rCargoMass;			//	Mass of cargo items (not including installed)
		int m_iStealth;							//	Computed stealth

		CSpaceObject *m_pDocked;				//	If not NULL, object we are docked to.
		CSpaceObject *m_pExitGate;				//	If not NULL, gate we are about to exit.
		CDamageSource *m_pIrradiatedBy;			//	If not NULL, object that irradiated us
		SShipGeneratorCtx *m_pDeferredOrders;	//	Defer orders until system done being created.

        CShipPerformanceDesc m_Perf;            //  Computed performance parameters (not saved)

		DWORD m_fRadioactive:1;					//	TRUE if radioactive
		DWORD m_fDestroyInGate:1;				//	TRUE if ship has entered a gate
		DWORD m_fHalfSpeed:1;					//	TRUE if ship is at half speed
		DWORD m_fDeviceDisrupted:1;				//	TRUE if at least one device is disrupted
		DWORD m_fKnown:1;						//	TRUE if we know about this ship
		DWORD m_fHiddenByNebula:1;				//	TRUE if ship is hidden by nebula
		DWORD m_fTrackMass:1;					//	TRUE if ship keeps track of mass to compute performance
		DWORD m_fIdentified:1;					//	TRUE if player can see ship class, etc.

		DWORD m_fManualSuspended:1;				//	TRUE if ship is suspended
		mutable DWORD m_fRecalcItemMass:1;		//	TRUE if we need to recalculate m_rImageMass
		DWORD m_fDockingDisabled:1;				//	TRUE if docking is disabled
		DWORD m_fControllerDisabled:1;			//	TRUE if we want to disable controller
		DWORD m_fRecalcRotationAccel:1;			//	TRUE if we need to recalc rotation acceleration
		DWORD m_fParalyzedByOverlay:1;			//	TRUE if one or more overlays paralyze the ship.
		DWORD m_fDisarmedByOverlay:1;			//	TRUE if one or more overlays disarmed the ship.
		DWORD m_fSpinningByOverlay:1;			//	TRUE if we should spin wildly

		DWORD m_fDragByOverlay:1;				//	TRUE if overlay imposes drag
		DWORD m_fAlwaysLeaveWreck:1;			//	TRUE if we always leave a wreck
		DWORD m_fEmergencySpeed:1;				//	TRUE if we're operating at 1.5x max speed
		DWORD m_fQuarterSpeed:1;				//	TRUE if we're operating at 0.25x max speed
		DWORD m_fLRSDisabledByNebula:1;			//	TRUE if LRS is disabled due to environment
		DWORD m_fShipCompartment:1;				//	TRUE if we're part of another ship (m_pDocked is the root ship)
		DWORD m_fHasShipCompartments:1;			//	TRUE if we have ship compartment objects attached
		DWORD m_fAutoCreatedPorts:1;			//	TRUE if we have auto created some docking ports

		DWORD m_fNameBlanked:1;					//	TRUE if name has been blanked; show generic name
		DWORD m_fSpare2:1;
		DWORD m_fSpare3:1;
		DWORD m_fSpare4:1;
		DWORD m_fSpare5:1;
		DWORD m_fSpare6:1;
		DWORD m_fSpare7:1;
		DWORD m_fSpare8:1;

	friend CObjectClass<CShip>;
	};

class CStaticEffect : public CSpaceObject
	{
	public:
		static ALERROR Create (CEffectCreator *pType,
							   CSystem *pSystem,
							   const CVector &vPos,
							   CStaticEffect **retpEffect);
		virtual ~CStaticEffect (void);

		//	CSpaceObject virtuals

		virtual CString GetObjClassName (void) override { return CONSTLIT("CStaticEffect"); }
		virtual CSystem::LayerEnum GetPaintLayer (void) override { return CSystem::layerEffects; }
		virtual bool IsAnchored (void) const override { return true; }
		virtual void PaintLRSForeground (CG32bitImage &Dest, int x, int y, const ViewportTransform &Trans) override { }

	protected:

		//	CSpaceObject virtuals
		virtual bool CanHit (CSpaceObject *pObj) override { return false; }
		virtual EDamageResults OnDamage (SDamageCtx &Ctx) override { return damagePassthrough; }
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual void OnReadFromStream (SLoadCtx &Ctx) override;
		virtual void OnWriteToStream (IWriteStream *pStream) override;

	private:
		CStaticEffect (void);

		IEffectPainter *m_pPainter;

	friend CObjectClass<CStaticEffect>;
	};

class CStation : public CSpaceObject
	{
	public:
		static ALERROR CreateFromType (CSystem *pSystem,
				CStationType *pType,
				SObjCreateCtx &CreateCtx,
				CStation **retpStation,
				CString *retsError = NULL);
		virtual ~CStation (void);

		inline void ClearFireReconEvent (void) { m_fFireReconEvent = false; }
		inline void ClearReconned (void) { m_fReconned = false; }
		inline const CStationHull &GetHull (void) const { return m_Hull; }
		int GetImageVariant (void);
		inline int GetImageVariantCount (void) { return m_pType->GetImageVariants(); }
		inline int GetSubordinateCount (void) { return m_Subordinates.GetCount(); }
		inline CSpaceObject *GetSubordinate (int iIndex) { return m_Subordinates.GetObj(iIndex); }
		bool IsNameSet (void) const;
		inline bool IsReconned (void) { return (m_fReconned ? true : false); }
		inline void SetActive (void) { m_fActive = true; }
		inline void SetBase (CSpaceObject *pBase) { m_pBase = pBase; }
		inline void SetFireReconEvent (void) { m_fFireReconEvent = true; }
		void SetFlotsamImage (CItemType *pItemType);
		void SetImageVariant (int iVariant);
		inline void SetInactive (void) { m_fActive = false; }
		void SetMapOrbit (const COrbit &oOrbit);
		inline void SetMass (Metric rMass) { m_rMass = rMass; }
		inline void SetNoConstruction (void) { m_fNoConstruction = true; }
		inline void SetNoMapLabel (void) { m_fNoMapLabel = true; }
		inline void SetNoReinforcements (void) { m_fNoReinforcements = true; }
		inline void SetPaintOverhang (bool bOverhang = true) { m_fPaintOverhang = bOverhang; }
		inline void SetReconned (void) { m_fReconned = true; }
		inline void SetRotation (int iAngle) { if (m_pRotation) m_pRotation->SetRotationAngle(m_pType->GetRotationDesc(), iAngle); }
		inline void SetShowMapLabel (bool bShow = true) { m_fNoMapLabel = !bShow; }
		void SetStargate (const CString &sDestNode, const CString &sDestEntryPoint);
		inline void SetStructuralHitPoints (int iHP) { m_Hull.SetStructuralHP(iHP); }
		void SetWreckImage (CShipClass *pWreckClass);
		void SetWreckParams (CShipClass *pWreckClass, CShip *pShip = NULL);

		//	CSpaceObject virtuals

		virtual void AddOverlay (COverlayType *pType, int iPosAngle, int iPosRadius, int iRotation, int iLifetime, DWORD *retdwID = NULL) override;
		virtual void AddSubordinate (CSpaceObject *pSubordinate) override;
		virtual CTradingDesc *AllocTradeDescOverride (void) override;
		virtual CStation *AsStation (void) override { return this; }
		virtual bool CalcVolumetricShadowLine (SLightingCtx &Ctx, int *retxCenter, int *retyCenter, int *retiWidth, int *retiLength) override;
		virtual bool CanAttack (void) const override;
		virtual bool CanBeDestroyed (void) override { return m_Hull.CanBeDestroyed(); }
		virtual bool CanBlock (CSpaceObject *pObj) override;
		virtual bool CanBlockShips (void) override { return m_fBlocksShips; }
		virtual RequestDockResults CanObjRequestDock (CSpaceObject *pObj = NULL) const override;
		virtual bool ClassCanAttack (void) override;
		virtual void CreateRandomDockedShips (IShipGenerator *pGenerator, const CShipChallengeDesc &Needed = CShipChallengeDesc()) override;
		virtual void CreateStarlightImage (int iStarAngle, Metric rStarDist) override;
		virtual CString DebugCrashInfo (void) override;
		virtual void Decontaminate (void) override { m_fRadioactive = false; }
		virtual CurrencyValue GetBalancedTreasure (void) const { return m_pType->GetBalancedTreasure(); }
        virtual CSpaceObject *GetBase (void) const override { return m_pBase; }
		virtual Categories GetCategory (void) const override { return catStation; }
		virtual DWORD GetClassUNID (void) override { return m_pType->GetUNID(); }
		virtual CCurrencyBlock *GetCurrencyBlock (bool bCreate = false) override;
		virtual int GetDamageEffectiveness (CSpaceObject *pAttacker, CInstalledDevice *pWeapon) override;
		virtual DWORD GetDefaultBkgnd (void) override { return m_pType->GetDefaultBkgnd(); }
		virtual CInstalledDevice *GetDevice (int iDev) override { return &m_pDevices[iDev]; }
		virtual int GetDeviceCount (void) const override { return (m_pDevices ? maxDevices : 0); }
		virtual CDockingPorts *GetDockingPorts (void) override { return &m_DockingPorts; }
		virtual CStationType *GetEncounterInfo (void) override { return m_pType; }
		virtual Metric GetGravity (Metric *retrRadius) const override;
		virtual const CObjectImageArray &GetHeroImage (void) const override { return m_pType->GetHeroImage(CCompositeImageSelector(), CCompositeImageModifiers()); }
		virtual const CObjectImageArray &GetImage (void) const override { return m_pType->GetImage(m_ImageSelector, CCompositeImageModifiers()); }
        virtual const CCompositeImageSelector &GetImageSelector (void) const override { return m_ImageSelector; }
		virtual Metric GetInvMass (void) const override;
		virtual int GetLevel (void) const override { return m_pType->GetLevel(); }
		virtual const COrbit *GetMapOrbit (void) const override { return m_pMapOrbit; }
		virtual Metric GetMass (void) const override { return m_rMass; }
		virtual int GetMaxLightDistance (void) override { return m_pType->GetMaxLightDistance(); }
		virtual Metric GetMaxWeaponRange (void) const override;
		virtual CString GetNamePattern (DWORD dwNounPhraseFlags = 0, DWORD *retdwFlags = NULL) const override;
		virtual CString GetObjClassName (void) override { return CONSTLIT("CStation"); }
		virtual COverlayList *GetOverlays (void) override { return &m_Overlays; }
		virtual const COverlayList *GetOverlays (void) const override { return &m_Overlays; }
		virtual CSystem::LayerEnum GetPaintLayer (void) override;
		virtual Metric GetParallaxDist (void) override { return m_rParallaxDist; }
		virtual EDamageResults GetPassthroughDefault (void) override { return m_Hull.GetPassthroughDefault(); }
		virtual int GetPlanetarySize (void) const override { return (GetScale() == scaleWorld ? m_pType->GetSize() : 0); }
		virtual ICCItem *GetProperty (CCodeChainCtx &Ctx, const CString &sName) override;
		virtual IShipGenerator *GetRandomEncounterTable (int *retiFrequency = NULL) const override;
		virtual int GetRotation (void) const override { return (m_pRotation ? m_pRotation->GetRotationAngle(m_pType->GetRotationDesc()) : 0); }
		virtual ScaleTypes GetScale (void) const override { return m_Scale; }
		virtual CXMLElement *GetScreen (const CString &sName) override { return m_pType->GetScreen(sName); }
		virtual CSovereign *GetSovereign (void) const override { return m_pSovereign; }
		virtual CG32bitPixel GetSpaceColor (void) override { return m_pType->GetSpaceColor(); }
		virtual CString GetStargateID (void) const override;
		virtual int GetStealth (void) const override;
		virtual Metric GetStellarMass (void) const override { return (GetScale() == scaleStar ? m_rMass : 0.0); }
		virtual CSpaceObject *GetTarget (CItemCtx &ItemCtx, bool bNoAutoTarget = false) const override;
		virtual CTradingDesc *GetTradeDescOverride (void) override { return m_pTrade; }
		virtual CDesignType *GetType (void) const override { return m_pType; }
		virtual int GetVisibleDamage (void) override { return m_Hull.GetVisibleDamage(); }
		virtual void GetVisibleDamageDesc (SVisibleDamage &Damage) override { return m_Hull.GetVisibleDamageDesc(Damage); }
		virtual CDesignType *GetWreckType (void) const override;
		virtual bool HasAttribute (const CString &sAttribute) const override;
		virtual bool HasMapLabel (void) override;
		virtual bool HasVolumetricShadow (void) const override { return (GetScale() == scaleWorld && !IsOutOfPlaneObj()); }
		virtual bool ImageInObject (const CVector &vObjPos, const CObjectImageArray &Image, int iTick, int iRotation, const CVector &vImagePos) override;
		virtual bool IsAbandoned (void) const override { return m_Hull.IsAbandoned(); }
		virtual bool IsActiveStargate (void) const override { return !m_sStargateDestNode.IsBlank() && m_fActive; }
		virtual bool IsAnchored (void) const override { return (!m_pType->IsMobile() || IsManuallyAnchored()); }
		virtual bool IsAngry (void) override { return (!IsAbandoned() && (m_iAngryCounter > 0)); }
		virtual bool IsAngryAt (CSpaceObject *pObj) const override { return (IsEnemy(pObj) || IsBlacklisted(pObj)); }
		virtual bool IsDisarmed (void) override { return m_fDisarmedByOverlay; }
		virtual bool IsExplored (void) override { return m_fExplored; }
		virtual bool IsIdentified (void) override { return m_fKnown; }
		virtual bool IsImmutable (void) const override { return m_Hull.IsImmutable(); }
		virtual bool IsInactive (void) const override { return !CanAttack(); }
		virtual bool IsIntangible (void) const { return (IsVirtual() || IsSuspended() || IsDestroyed()); }
		virtual bool IsKnown (void) override { return m_fKnown; }
		virtual bool IsMultiHull (void) override { return m_Hull.IsMultiHull(); }
		virtual bool IsParalyzed (void) override { return m_fParalyzedByOverlay; }
		virtual bool IsRadioactive (void) override { return (m_fRadioactive ? true : false); }
        virtual bool IsSatelliteSegmentOf (CSpaceObject *pBase) const override { return (m_fIsSegment && (m_pBase == pBase)); }
        virtual bool IsShownInGalacticMap (void) const override;
		virtual bool IsStargate (void) const override { return !m_sStargateDestNode.IsBlank(); }
		virtual bool IsTimeStopImmune (void) override { return m_pType->IsTimeStopImmune(); }
		virtual bool IsVirtual (void) const override { return m_pType->IsVirtual(); }
		virtual bool IsWreck (void) const override { return (m_dwWreckUNID != 0); }
		virtual void MakeRadioactive (void) override { m_fRadioactive = true; }
		virtual void MarkImages (void) override { m_pType->MarkImages(m_ImageSelector); }
		virtual bool ObjectInObject (const CVector &vObj1Pos, CSpaceObject *pObj2, const CVector &vObj2Pos) override;
		virtual void OnDestroyed (SDestroyCtx &Ctx) override;
		virtual void OnObjBounce (CSpaceObject *pObj, const CVector &vPos) override;
		virtual void OnObjLeaveGate (CSpaceObject *pObj) override;
		virtual void OnPlayerObj (CSpaceObject *pPlayer) override;
		virtual void OnStationDestroyed (const SDestroyCtx &Ctx) override;
		virtual void OnSubordinateDestroyed (SDestroyCtx &Ctx) override;
		virtual void OnSubordinateHit (SDamageCtx &Ctx) override;
		virtual void OnSystemCreated (SSystemCreateCtx &CreateCtx) override;
		virtual void PaintLRSBackground (CG32bitImage &Dest, int x, int y, const ViewportTransform &Trans) override;
		virtual void PaintLRSForeground (CG32bitImage &Dest, int x, int y, const ViewportTransform &Trans) override;
		virtual bool PointInObject (const CVector &vObjPos, const CVector &vPointPos) override;
		virtual bool PointInObject (SPointInObjectCtx &Ctx, const CVector &vObjPos, const CVector &vPointPos) override;
		virtual void PointInObjectInit (SPointInObjectCtx &Ctx) override;
		virtual void RefreshBounds (void) override { CalcBounds(); }
		virtual void RemoveOverlay (DWORD dwID) override;
		virtual bool RemoveSubordinate (CSpaceObject *pSubordinate) override;
		virtual bool RequestGate (CSpaceObject *pObj) override;
		virtual void SetExplored (bool bExplored = true) override { m_fExplored = bExplored; }
		virtual void SetIdentified (bool bIdentified = true) override { m_fKnown = bIdentified; }
		virtual void SetKnown (bool bKnown = true) override;
		virtual void SetMapLabelPos (int x, int y) override { m_xMapLabel = x; m_yMapLabel = y; }
		virtual void SetName (const CString &sName, DWORD dwFlags = 0) override;
		virtual bool SetProperty (const CString &sName, ICCItem *pValue, CString *retsError) override;
        virtual bool ShowMapOrbit (void) const override { return (m_fShowMapOrbit ? true : false); }
        virtual bool ShowStationDamage (void) const override { return m_Hull.IsWrecked(); }
		virtual bool SupportsGating (void) override { return IsActiveStargate(); }
		virtual void Undock (CSpaceObject *pObj) override;

	protected:

		//	CSpaceObject virtuals
		virtual bool CanFireOn (CSpaceObject *pObj) override { return CanFireOnObjHelper(pObj); }
		virtual CDesignType *GetDefaultDockScreen (CString *retsName = NULL) const override;
		virtual void OnMove (const CVector &vOldPos, Metric rSeconds) override;
		virtual void ObjectDestroyedHook (const SDestroyCtx &Ctx) override;
		virtual DWORD OnCommunicate (CSpaceObject *pSender, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2) override;
		virtual void OnComponentChanged (ObjectComponentTypes iComponent) override;
		virtual EDamageResults OnDamage (SDamageCtx &Ctx) override;
		virtual void OnObjEnteredGate (CSpaceObject *pObj, CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate) override;
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual void OnPaintAnnotations (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual void OnPaintMap (CMapViewportCtx &Ctx, CG32bitImage &Dest, int x, int y) override;
		virtual void OnReadFromStream (SLoadCtx &Ctx) override;
		virtual void OnSetEventFlags (void) override;
        virtual void OnSetSovereign (CSovereign *pSovereign) override { m_pSovereign = pSovereign; }
		virtual void OnUpdate (SUpdateCtx &Ctx, Metric rSecondsPerTick) override;
		virtual void OnUpdateExtended (const CTimeSpan &ExtraTime) override;
		virtual void OnWriteToStream (IWriteStream *pStream) override;

	private:
		enum Constants
			{
			maxRegisteredObjects	= 12,
			maxDevices				= 8,
			};

		CStation (void);

		void AvengeAttack (CSpaceObject *pAttacker);
		bool Blacklist (CSpaceObject *pObj);
		void CalcBounds (void);
		void CalcImageModifiers (CCompositeImageModifiers *retModifiers, int *retiTick = NULL);
		int CalcNumberOfShips (void);
		void CalcOverlayImpact (void);
		inline bool CanBlacklist (void) const { return (m_pType->IsBlacklistEnabled() && !IsImmutable() && !m_fNoBlacklist); }
		void ClearBlacklist (CSpaceObject *pObj);
		void CreateDestructionEffect (void);
		void CreateEjectaFromDamage (int iDamage, const CVector &vHitPos, int iDirection, const DamageDesc &Damage);
		void CreateStructuralDestructionEffect (SDestroyCtx &Ctx);
		ALERROR CreateMapImage (void);
		void DeterAttack (CSpaceObject *pTarget);
		void FinishCreation (SSystemCreateCtx *pSysCreateCtx = NULL);
		Metric GetAttackDistance (void) const;
		const CObjectImageArray &GetImage (bool bFade, int *retiTick = NULL, int *retiVariant = NULL);
		bool IsBlacklisted (CSpaceObject *pObj = NULL) const;
		void OnDestroyedByFriendlyFire (CSpaceObject *pAttacker, CSpaceObject *pOrderGiver);
		void OnDestroyedByHostileFire (CSpaceObject *pAttacker, CSpaceObject *pOrderGiver);
		void OnHitByFriendlyFire (CSpaceObject *pAttacker, CSpaceObject *pOrderGiver);
		void OnHitByHostileFire (CSpaceObject *pAttacker, CSpaceObject *pOrderGiver);
		void RaiseAlert (CSpaceObject *pTarget);
		void SetAngry (void);
		void UpdateAttacking (SUpdateCtx &Ctx, int iTick);
		void UpdateReinforcements (int iTick);
		void UpdateTargets (SUpdateCtx &Ctx, Metric rAttackRange);

		CStationType *m_pType;					//	Station type
		CString m_sName;						//	Station name
		DWORD m_dwNameFlags;					//	Name flags
		CSovereign *m_pSovereign;				//	Allegiance
		ScaleTypes m_Scale;						//	Scale of station
		Metric m_rMass;							//	Mass of station (depends on scale)
		CIntegralRotation *m_pRotation;			//	Rotation parameters (may be NULL)

		CCompositeImageSelector m_ImageSelector;//	Image variant to display
		int m_iDestroyedAnimation;				//	Frames left of destroyed animation
		COrbit *m_pMapOrbit;					//	Orbit to draw on map
		int m_xMapLabel;						//	Name label in map view
		int m_yMapLabel;
		Metric m_rParallaxDist;					//	Parallax distance (1.0 = normal; > 1.0 = background; < 1.0 = foreground)

		CString m_sStargateDestNode;			//	Destination node
		CString m_sStargateDestEntryPoint;		//	Destination entry point

		CStationHull m_Hull;					//	Hull and armor
		CInstalledDevice *m_pDevices;			//	Array of devices
		COverlayList m_Overlays;				//	List of overlays
		CDockingPorts m_DockingPorts;			//	Docking ports

		CSpaceObject *m_pBase;					//	If we're a subordinate, this points to our base
		CSpaceObjectList m_Subordinates;		//	List of subordinates
		CSpaceObjectList m_Targets;				//	Targets to destroy (by our ships)
		TArray<CSpaceObject *> m_WeaponTargets;	//	Targets to destroy (by our weapons)

		CAttackDetector m_Blacklist;			//	Player blacklisted
		int m_iAngryCounter;					//	Attack cycles until station is not angry
		int m_iReinforceRequestCount;			//	Number of times we've requested reinforcements

		CCurrencyBlock *m_pMoney;				//	Money left (may be NULL)
		CTradingDesc *m_pTrade;					//	Override of trading desc (may be NULL)

		DWORD m_fArmed:1;						//	TRUE if station has weapons
		DWORD m_fKnown:1;						//	TRUE if known to the player
		DWORD m_fNoMapLabel:1;					//	Do not show map label
		DWORD m_fActive:1;						//	TRUE if stargate is active
		DWORD m_fNoReinforcements:1;			//	Do not send reinforcements
		DWORD m_fRadioactive:1;					//	TRUE if radioactive
		DWORD m_fReconned:1;					//	TRUE if reconned by player
		DWORD m_fFireReconEvent:1;				//	If TRUE, fire OnReconned

		DWORD m_fExplored:1;					//	If TRUE, player has docked at least once
		DWORD m_fDisarmedByOverlay:1;			//	If TRUE, an overlay has disarmed us
		DWORD m_fParalyzedByOverlay:1;			//	If TRUE, an overlay has paralyzed us
		DWORD m_fNoBlacklist:1;					//	If TRUE, do not blacklist player on friendly fire
		DWORD m_fNoConstruction:1;				//	Do not build new ships
		DWORD m_fBlocksShips:1;					//	TRUE if we block ships
		DWORD m_fPaintOverhang:1;				//	If TRUE, paint above player ship
		DWORD m_fShowMapOrbit:1;				//	If TRUE, show orbit in map

		DWORD m_fDestroyIfEmpty:1;				//	If TRUE, we destroy the station as soon as it is empty
		DWORD m_fIsSegment:1;                   //  If TRUE, we are a segment of some other object (m_pBase)
		DWORD m_fSpare3:1;
		DWORD m_fSpare4:1;
		DWORD m_fSpare5:1;
		DWORD m_fSpare6:1;
		DWORD m_fSpare7:1;
		DWORD m_fSpare8:1;

		DWORD m_dwSpare:8;

		//	Wreck image
		DWORD m_dwWreckUNID;					//	UNID of wreck class (0 if none)

		CG32bitImage m_MapImage;				//	Image for the map (if star or world)
		CString m_sMapLabel;					//	Label for map

		CObjectImageArray m_StarlightImage;		//	Image rotated for proper lighting.

	friend CObjectClass<CStation>;
	};


