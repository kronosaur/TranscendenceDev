//	TSEParticleSystem.h
//
//	Classes and functions for particle system.
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

struct SEffectUpdateCtx;
class CEffectParamDesc;

enum ParticlePaintStyles
	{
	paintUnknown =						0,

	paintPlain =						1,
	paintFlame =						2,
	paintSmoke =						3,
	paintImage =						4,
	paintLine =							5,
	paintGlitter =						6,

	paintMax =							6,
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

class CFireAndSmokePainter
	{
	public:
		enum EStyles
			{
			styleNone,

			styleFlame,
			styleSmoke,
			};

		CFireAndSmokePainter (EStyles iStyle, int iLifetime, int iMinWidth, int iMaxWidth);
		CFireAndSmokePainter (int iLifetime, int iMinWidth, int iMaxWidth, int iCore, int iFlame, int iSmoke, int iSmokeBrightness);

		void Paint (CG32bitImage &Dest, int x, int y, int iAge, int iDestiny) const;

	private:
		void Init (int iLifetime, int iMinWidth, int iMaxWidth, int iCore, int iFlame, int iSmoke, int iSmokeBrightness);

		int m_iLifetime;					//	Maximum lifetime of particles
		int m_iMinWidth;
		int m_iWidthRange;

		int m_iCore;
		int m_iFlame;
		int m_iSmoke;

		int m_iSmokeBrightness;
	};

class CGaseousPainter
	{
	public:
		CGaseousPainter (int iLifetime, int iMinWidth, int iMaxWidth, CG32bitPixel rgbPrimaryColor, CG32bitPixel rgbSecondaryColor);

		void Paint (CG32bitImage &Dest, int x, int y, int iAge) const;

	private:
		int m_iLifetime;
		int m_iMinWidth;
		int m_iWidthRange;

		CG32bitPixel m_rgbPrimary;
		CG32bitPixel m_rgbSecondary;
	};

class CGlitterPainter
	{
	public:
		CGlitterPainter (int iWidth, CG32bitPixel rgbPrimary, CG32bitPixel rgbSecondary);

		void Paint (CG32bitImage &Dest, int x, int y, int iRotation) const;

	private:
		int m_iWidth;

		CG32bitPixel m_rgbPrimary;
		CG32bitPixel m_rgbSecondary;
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
			styleWrithe =					7,
			styleBrownian =					8,

			styleMax =						8,
			};

		CParticleSystemDesc (void);

		void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) { retTypesUsed->SetAt(m_pParticleEffect.GetUNID(), true); }
		ALERROR Bind (SDesignLoadCtx &Ctx) { return m_pParticleEffect.Bind(Ctx); }
		IEffectPainter *CreateParticlePainter (CCreatePainterCtx &Ctx) { return m_pParticleEffect.CreatePainter(Ctx); }
		ALERROR InitFromWeaponDescXML (SDesignLoadCtx &Ctx, const CXMLElement *pDesc, const CString &sUNID);
		ALERROR InitFromWeaponDescXMLCompatible (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, const CXMLElement *pDesc, const CString &sUNID);
		Metric GetCohesionFactor (void) const { return m_rCohesionFactor; }
		int GetCohesionPotential (void) const { return mathRound(m_rCohesionFactor * 100.0); }
		int GetEmitChance (void) const { return m_iEmitChance; }
		const DiceRange &GetEmitLifetime (void) const { return m_EmitLifetime; }
		const DiceRange &GetEmitRate (void) const { return m_EmitRate; }
		const DiceRange &GetEmitSpeed (void) const { return m_EmitSpeed; }
		const DiceRange &GetEmitWidth (void) const { return m_EmitWidth; }
		int GetMissChance (void) const { return m_iMissChance; }
		CEffectCreator *GetParticleEffect (void) { return m_pParticleEffect; }
		const DiceRange &GetParticleLifetime (void) const { return m_ParticleLifetime; }
		const DiceRange &GetRadius (void) const { return m_Radius; }
		int GetSplashChance (void) const { return m_iSplashChance; }
		const DiceRange &GetSpreadAngle (void) const { return m_SpreadAngle; }
		EStyles GetStyle (void) const { return m_iStyle; }
		const DiceRange &GetTangentSpeed (void) const { return m_TangentSpeed; }
		Metric GetWakeFactor (void) const { return m_rWakeFactor; }
		int GetWakePotential (void) const { return mathRound(m_rWakeFactor * 100.0); }
		int GetXformRotation (void) const { return m_iXformRotation; }
		Metric GetXformTime (void) const { return m_rXformTime; }
		bool HasWakeFactor (void) const { return m_bHasWake; }
		bool IsFixedPos (void) const { return m_bFixedPos; }
		bool IsSprayCompatible (void) const { return m_bSprayCompatible; }
		bool IsTrackingObject (void) const { return m_bTrackingObject; }
		void MarkImages (void);
		void SetCohesionPotential (int iValue) { m_rCohesionFactor = Max(0, iValue) / 100.0; }
		void SetEmitChance (int iValue) { m_iEmitChance = iValue; }
		void SetEmitLifetime (const DiceRange &Value) { m_EmitLifetime = Value; }
		void SetEmitRate (const DiceRange &Value) { m_EmitRate = Value; }
		void SetEmitSpeed (const DiceRange &Value) { m_EmitSpeed = Value; }
		void SetEmitWidth (const DiceRange &Value) { m_EmitWidth = Value; }
		void SetFixedPos (bool bValue = true) { m_bFixedPos = bValue; }
		void SetMissChance (int iValue) { m_iMissChance = iValue; }
		void SetParticleLifetime (const DiceRange &Value) { m_ParticleLifetime = Value; }
		void SetRadius (const DiceRange &Value) { m_Radius = Value; }
		void SetSplashChance (int iValue) { m_iSplashChance = iValue; }
		void SetSprayCompatible (bool bValue = true) { m_bSprayCompatible = bValue; }
		void SetSpreadAngle (const DiceRange &Value) { m_SpreadAngle = Value; }
		void SetStyle (EStyles iStyle) { m_iStyle = iStyle; }
		void SetStyle (const CEffectParamDesc &Value);
		void SetTangentSpeed (const DiceRange &Value) { m_TangentSpeed = Value; }
		void SetTrackingObject (bool bValue = true) { m_bTrackingObject = bValue; }
		void SetWakePotential (int iValue) { m_rWakeFactor = Max(0, iValue) / 100.0; m_bHasWake = (m_rWakeFactor > 0.0); }
		void SetWakePotential (Metric rValue) { m_rWakeFactor = Max(0.0, rValue) / 100.0; m_bHasWake = (m_rWakeFactor > 0.0); }
		void SetXformRotation (int iValue) { m_iXformRotation = iValue; }
		void SetXformTime (Metric rValue) { m_rXformTime = rValue; }

		static EStyles ParseStyle (const CString &sValue);

	private:
		EStyles m_iStyle;						//	System style

		//	Emission behavior

		DiceRange m_EmitRate;					//	Number of particles created per tick
		DiceRange m_EmitSpeed;					//	Particle speed (fractions of lightspeed)
		DiceRange m_EmitLifetime;				//	Duration of particle emission
		DiceRange m_EmitWidth;					//	Width of emission line (pixels)
		DiceRange m_ParticleLifetime;			//	Lifetime of a particle, in ticks
		DiceRange m_SpreadAngle;				//	Angle at which particles spread
		DiceRange m_TangentSpeed;				//	Exhaust spread
		int m_iEmitChance;						//	Chance of emitting on a given tick

		//	Collision/motion behavior

		DiceRange m_Radius;						//	System radius (for some styles)
		Metric m_rCohesionFactor;				//	(0-1) Tendency to return to original shape
		Metric m_rWakeFactor;					//	Speed changes when objects pass through
		int m_iSplashChance;					//	Chance of a particle bouncing off
		int m_iMissChance;						//	Chance of a particle missing

		//	Adjustments

		int m_iXformRotation;					//	Rotation
		Metric m_rXformTime;					//	Time adjustment

		//	Particles

		CEffectCreatorRef m_pParticleEffect;	//	Effect for each particle

		//	Flags

		bool m_bHasWake;						//	m_rWakeFactor > 0.0
		bool m_bSprayCompatible;				//	In previous versions we used to vary speed.
		bool m_bFixedPos;						//	Exhaust style particles
		bool m_bTrackingObject;					//	Emitting objects maneuvers
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
			Metric rData;						//	Miscellaneous data for particle

			DWORD fAlive:1;						//	TRUE if particle is alive
			DWORD dwSpare:31;					//	Spare
			};

		CParticleArray (void);
		~CParticleArray (void);

		void Emit (const CParticleSystemDesc &Desc, CSpaceObject *pObj, const CVector &vSource, const CVector &vSourceVel, int iDirection, int iTick, int *retiEmitted = NULL);
		void Paint (const CParticleSystemDesc &Desc, CG32bitImage &Dest, int xPos, int yPos, IEffectPainter *pPainter, SViewportPaintCtx &Ctx);
		void Update (const CParticleSystemDesc &Desc, SEffectUpdateCtx &Ctx);

		void AddParticle (const CVector &vPos, const CVector &vVel, int iLifeLeft = -1, int iRotation = -1, int iDestiny = -1, int iGeneration = 0, Metric rData = 0.0);
		SParticle *GetArray (int *retiCount = NULL) const { if (retiCount) *retiCount = m_iCount; return m_pArray; }
		const RECT &GetBounds (void) const { return m_rcBounds; }
		void GetBounds (CVector *retvUR, CVector *retvLL);
		int GetCount (void) const { return m_iCount; }
		int GetLastEmitDirection (void) const { return m_iLastEmitDirection; }
		const CVector &GetLastEmitPos (void) const { return m_vLastEmitSource; }
		const CVector &GetOrigin (void) const { return m_vOrigin; }
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
		void ResetLastEmit (int iLastDirection, const CVector &vLastEmitPos, const CVector &vLastEmitVel = NullVector);
		void SetOrigin (const CVector &vOrigin) { m_vOrigin = vOrigin; }
		void UpdateMotionLinear (bool *retbAlive = NULL, CVector *retvAveragePos = NULL);
		void UpdateRingCohesion (Metric rRadius, Metric rMinRadius, Metric rMaxRadius, int iCohesion, int iResistance);
		void WriteToStream (IWriteStream *pStream) const;

	private:
		struct SParticle64
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

			int iLifeLeft;						//	Ticks of life left
			int iDestiny;						//	Random number from 1-360
			int iRotation;						//	Particle rotation
			DWORD dwData;						//	Miscellaneous data for particle

			DWORD fAlive:1;						//	TRUE if particle is alive
			DWORD dwSpare:31;					//	Spare
			};

		struct SParticle119
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

		void CleanUp (void);
		void CreateInterpolatedParticles (const CParticleSystemDesc &Desc, CSpaceObject *pObj, int iCount, const CVector &vSource, const CVector &vSourceVel, int iDirection, int iTick);
		void CreateLinearParticles (const CParticleSystemDesc &Desc, CSpaceObject *pObj, int iCount, const CVector &vSource, const CVector &vSourceVel, int iDirection, int iTick);
		void EmitAmorphous (const CParticleSystemDesc &Desc, int iCount, const CVector &vSource, const CVector &vSourceVel, int iDirection, int iTick);
		void EmitCloud (const CParticleSystemDesc &Desc, int iCount, const CVector &vSource, const CVector &vSourceVel, int iDirection, int iTick);
		void EmitComet (const CParticleSystemDesc &Desc, int iCount, const CVector &vSource, const CVector &vSourceVel, int iDirection, int iTick);
		void EmitExhaust (const CParticleSystemDesc &Desc, CSpaceObject *pObj, int iCount, const CVector &vSource, const CVector &vSourceVel, int iDirection, int iTick, CVector *retvLastSource);
		void EmitJet (const CParticleSystemDesc &Desc, CSpaceObject *pObj, int iCount, const CVector &vSource, const CVector &vSourceVel, int iDirection, int iTick);
		void EmitRadiate (const CParticleSystemDesc &Desc, int iCount, const CVector &vSource, const CVector &vSourceVel, int iDirection, int iTick);
		void EmitRadiateArc (const CParticleSystemDesc &Desc, int iCount, const CVector &vSource, const CVector &vSourceVel, int iDirection, int iTick);
		void EmitSpray (const CParticleSystemDesc &Desc, int iCount, const CVector &vSource, const CVector &vSourceVel, int iDirection, int iTick);
		int GetManeuverRate (SEffectUpdateCtx &Ctx) const;
		bool IsTrackingTime (SEffectUpdateCtx &Ctx) const;
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
		void PaintGlitter (CG32bitImage &Dest, int xPos, int yPos, SViewportPaintCtx &Ctx, int iWidth, CG32bitPixel rgbPrimaryColor, CG32bitPixel rgbSecondaryColor);
		void PaintImage (CG32bitImage &Dest, int xPos, int yPos, SViewportPaintCtx &Ctx, SParticlePaintDesc &Desc);
		void PaintLine (CG32bitImage &Dest,
						int xPos,
						int yPos,
						SViewportPaintCtx &Ctx,
						int iLength,
						CG32bitPixel rgbPrimaryColor,
						CG32bitPixel rgbSecondaryColor);
		void PosToXY (const CVector &xy, int *retx, int *rety);
		void UpdateBrownian (const CParticleSystemDesc &Desc, SEffectUpdateCtx &Ctx);
		void UpdateCollisions (const CParticleSystemDesc &Desc, SEffectUpdateCtx &Ctx);
		void UpdateComet (const CParticleSystemDesc &Desc, SEffectUpdateCtx &Ctx);
		void UpdateTrackTarget (const CParticleSystemDesc &Desc, SEffectUpdateCtx &Ctx);
		void UpdateWrithe (SEffectUpdateCtx &UpdateCtx);
		void UseRealCoords (void);
		CVector XYToPos (int x, int y);

		int m_iCount;
		SParticle *m_pArray;
		RECT m_rcBounds;						//	Bounding box in pixels relative to center
		CVector m_vOrigin;						//	Origin position
		CVector m_vCenterOfMass;				//	Center of mass
		CVector m_vUR;							//	Bounds
		CVector m_vLL;							//	Bounds

		CVector m_vLastEmitSource;
		CVector m_vLastEmitSourceVel;
		int m_iLastEmitDirection;
		int m_iLastAdded;						//	Index of last particle added
		bool m_bUseRealCoords;					//	If TRUE, we keep real (instead of int) coordinates
	};
