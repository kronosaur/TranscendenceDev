//	TSEParticleSystem.h
//
//	Classes and functions for particle system.
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

struct SEffectUpdateCtx;

enum ParticlePaintStyles
	{
	paintPlain,
	paintFlame,
	paintSmoke,
	paintImage,
	paintLine,
	};

struct SParticlePaintDesc
	{
	SParticlePaintDesc (void)
		{
		iStyle = paintPlain;

		pImage = NULL;
		iVariants = 0;
		bRandomStartFrame = false;
		bDirectional = false;

		iMaxLifetime = -1;
		iMinWidth = 4;
		iMaxWidth = 4;

		rgbPrimaryColor = CG32bitPixel(255, 255, 255);
		rgbSecondaryColor = CG32bitPixel(0, 0, 0);
		}

	ParticlePaintStyles iStyle;

	CObjectImageArray *pImage;					//	Image for each particle
	int iVariants;								//	No of image variants
	bool bRandomStartFrame;						//	Start at a random animation
	bool bDirectional;							//	Different images for each direction

	int iMaxLifetime;							//	Max lifetime for particles
	int iMinWidth;								//	Min width of particle
	int iMaxWidth;								//	Max width of particle

	CG32bitPixel rgbPrimaryColor;				//	Primary color 
	CG32bitPixel rgbSecondaryColor;				//	Secondary color

	bool bDensityGrid;							//	Paint to off-screen alpha
	TArray<CG32bitPixel> *pColorTable;			//	Indexed by alpha value
	};

class CParticleSystemDesc
	{
	public:
		enum EStyles
			{
			styleUnknown =					0,
			
			styleAmorphous =				1,
			styleComet =					2,
			styleExhaust =					3,
			styleJet =						4,
			styleRadiate =					5,
			styleSpray =					6,

			styleMax =						6,
			};

		CParticleSystemDesc (void);

		inline void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) { retTypesUsed->SetAt(m_pParticleEffect.GetUNID(), true); }
		inline ALERROR Bind (SDesignLoadCtx &Ctx) { return m_pParticleEffect.Bind(Ctx); }
		inline IEffectPainter *CreateParticlePainter (CCreatePainterCtx &Ctx) { return m_pParticleEffect.CreatePainter(Ctx); }
		ALERROR InitFromWeaponDescXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID);
		inline const DiceRange &GetEmitLifetime (void) const { return m_EmitLifetime; }
		inline const DiceRange &GetEmitRate (void) const { return m_EmitRate; }
		inline const DiceRange &GetEmitSpeed (void) const { return m_EmitSpeed; }
		inline const DiceRange &GetEmitWidth (void) const { return m_EmitWidth; }
		inline CEffectCreator *GetParticleEffect (void) { return m_pParticleEffect; }
		inline const DiceRange &GetParticleLifetime (void) const { return m_ParticleLifetime; }
		inline int GetMissChance (void) const { return m_iMissChance; }
		inline int GetSplashChance (void) const { return m_iSplashChance; }
		inline const DiceRange &GetSpreadAngle (void) const { return m_SpreadAngle; }
		inline EStyles GetStyle (void) const { return m_iStyle; }
		inline int GetXformRotation (void) const { return m_iXformRotation; }
		inline Metric GetXformTime (void) const { return m_rXformTime; }
		inline bool IsSprayCompatible (void) const { return m_bSprayCompatible; }
		void MarkImages (void);
		inline void SetEmitLifetime (const DiceRange &Value) { m_EmitLifetime = Value; }
		inline void SetEmitRate (const DiceRange &Value) { m_EmitRate = Value; }
		inline void SetEmitSpeed (const DiceRange &Value) { m_EmitSpeed = Value; }
		inline void SetEmitWidth (const DiceRange &Value) { m_EmitWidth = Value; }
		inline void SetParticleLifetime (const DiceRange &Value) { m_ParticleLifetime = Value; }
		inline void SetMissChance (int iValue) { m_iMissChance = iValue; }
		inline void SetSplashChance (int iValue) { m_iSplashChance = iValue; }
		inline void SetSprayCompatible (bool bValue = true) { m_bSprayCompatible = bValue; }
		inline void SetSpreadAngle (const DiceRange &Value) { m_SpreadAngle = Value; }
		inline void SetStyle (EStyles iStyle) { m_iStyle = iStyle; }
		inline void SetXformRotation (int iValue) { m_iXformRotation = iValue; }
		inline void SetXformTime (Metric rValue) { m_rXformTime = rValue; }

	private:
		EStyles m_iStyle;						//	System style

		//	Emission behavior

		DiceRange m_EmitRate;					//	Number of particles created per tick
		DiceRange m_EmitSpeed;					//	Particle speed (fractions of lightspeed)
		DiceRange m_EmitLifetime;				//	Duration of particle emission
		DiceRange m_EmitWidth;					//	Width of emission line (pixels)
		DiceRange m_ParticleLifetime;			//	Lifetime of a particle, in ticks
		DiceRange m_SpreadAngle;				//	Angle at which particles spread
		int m_iXformRotation;					//	Rotation
		Metric m_rXformTime;					//	Time adjustment

		//	Collision behavior

		int m_iSplashChance;					//	Chance of a particle bouncing off
		int m_iMissChance;						//	Chance of a particle missing

		//	Particles

		CEffectCreatorRef m_pParticleEffect;	//	Effect for each particle

		//	Flags

		bool m_bSprayCompatible;				//	In previous versions we used to vary speed.
	};

class CParticleArray
	{
	public:
		struct SParticle
			{
			CVector Pos;						//	Position. Valid if we use real coordinates
			CVector Vel;						//	Velocity. Valid if we use real coordinates
												//		NOTE: In Km per tick (unlike normal velocities)

			int x;								//	Offset from center of particle cloud
			int y;								//		(screen-coords, in 256ths of pixels)
												//		(valid in all cases)
			int xVel;							//	Velocity relative to particle cloud
			int yVel;							//		(screen-coords, in 256ths of pixels per tick)
												//		(not valid if using real coordinates)

			int iGeneration;					//	Created on this tick
			int iLifeLeft;						//	Ticks of life left
			int iDestiny;						//	Random number from 1-360
			int iRotation;						//	Particle rotation
			DWORD dwData;						//	Miscellaneous data for particle

			DWORD fAlive:1;						//	TRUE if particle is alive
			DWORD dwSpare:31;					//	Spare
			};

		CParticleArray (void);
		~CParticleArray (void);

		void Emit (const CParticleSystemDesc &Desc, const CVector &vSource, const CVector &vSourceVel, int iDirection, int iTick, int *retiEmitted = NULL);
		void Paint (const CParticleSystemDesc &Desc, CG32bitImage &Dest, int xPos, int yPos, IEffectPainter *pPainter, SViewportPaintCtx &Ctx);
		void Update (const CParticleSystemDesc &Desc, SEffectUpdateCtx &Ctx);

		void AddParticle (const CVector &vPos, const CVector &vVel, int iLifeLeft = -1, int iRotation = -1, int iDestiny = -1, int iGeneration = 0, DWORD dwData = 0);
		const RECT &GetBounds (void) const { return m_rcBounds; }
		void GetBounds (CVector *retvUR, CVector *retvLL);
		inline int GetCount (void) const { return m_iCount; }
		inline SParticle *GetArray (int *retiCount = NULL) const { if (retiCount) *retiCount = m_iCount; return m_pArray; }
		inline const CVector &GetOrigin (void) const { return m_vOrigin; }
		void Init (int iMaxCount, const CVector &vOrigin = NullVector);
		void Move (const CVector &vMove);
		void Paint (CG32bitImage &Dest,
					int xPos,
					int yPos,
					SViewportPaintCtx &Ctx,
					SParticlePaintDesc &Desc);
		void Paint (CG32bitImage &Dest,
					int xPos,
					int yPos,
					SViewportPaintCtx &Ctx,
					IEffectPainter *pPainter,
					Metric rRatedSpeed = 0.0);
		void ReadFromStream (SLoadCtx &Ctx);
		inline void SetOrigin (const CVector &vOrigin) { m_vOrigin = vOrigin; }
		void UpdateMotionLinear (bool *retbAlive = NULL, CVector *retvAveragePos = NULL);
		void UpdateRingCohesion (Metric rRadius, Metric rMinRadius, Metric rMaxRadius, int iCohesion, int iResistance);
		void WriteToStream (IWriteStream *pStream) const;

	private:
		void CleanUp (void);
		void EmitAmorphous (const CParticleSystemDesc &Desc, int iCount, const CVector &vSource, const CVector &vSourceVel, int iDirection, int iTick);
		void EmitComet (const CParticleSystemDesc &Desc, int iCount, const CVector &vSource, const CVector &vSourceVel, int iDirection, int iTick);
		void EmitSpray (const CParticleSystemDesc &Desc, int iCount, const CVector &vSource, const CVector &vSourceVel, int iDirection, int iTick);
		void PaintFireAndSmoke (CG32bitImage &Dest, 
								int xPos, 
								int yPos, 
								SViewportPaintCtx &Ctx, 
								int iLifetime, 
								int iMinWidth,
								int iMaxWidth,
								int iCore,
								int iFlame,
								int iSmoke,
								int iSmokeBrightness);
		void PaintGaseous (CG32bitImage &Dest,
						   int xPos,
						   int yPos,
						   SViewportPaintCtx &Ctx,
						   int iMaxLifetime,
						   int iMinWidth,
						   int iMaxWidth,
						   CG32bitPixel rgbPrimaryColor,
						   CG32bitPixel rgbSecondaryColor);
		void PaintImage (CG32bitImage &Dest, int xPos, int yPos, SViewportPaintCtx &Ctx, SParticlePaintDesc &Desc);
		void PaintLine (CG32bitImage &Dest,
						int xPos,
						int yPos,
						SViewportPaintCtx &Ctx,
						CG32bitPixel rgbPrimaryColor,
						CG32bitPixel rgbSecondaryColor);
		void PosToXY (const CVector &xy, int *retx, int *rety);
		void UpdateCollisions (const CParticleSystemDesc &Desc, SEffectUpdateCtx &Ctx);
		void UpdateTrackTarget (CSpaceObject *pTarget, int iManeuverRate, Metric rMaxSpeed);
		void UseRealCoords (void);
		CVector XYToPos (int x, int y);

		int m_iCount;
		SParticle *m_pArray;
		RECT m_rcBounds;						//	Bounding box in pixels relative to center
		CVector m_vOrigin;						//	Origin position
		CVector m_vCenterOfMass;				//	Center of mass
		CVector m_vUR;							//	Bounds
		CVector m_vLL;							//	Bounds

		int m_iLastAdded;						//	Index of last particle added
		bool m_bUseRealCoords;					//	If TRUE, we keep real (instead of int) coordinates
	};
