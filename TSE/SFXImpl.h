//	TSESFXImpl.h
//
//	Transcendence IEffectPainter classes

#ifndef INCL_TSE_SFX
#define INCL_TSE_SFX

class CBeamEffectCreator : public CEffectCreator,
		public IEffectPainter
	{
	public:
		CBeamEffectCreator (void) : IEffectPainter(true)
			{ }

		static CString GetClassTag (void) { return CONSTLIT("Beam"); }

		virtual CString GetTag (void) override { return GetClassTag(); }

		//	CEffectCreator virtuals
		virtual int GetLifetime (void) override { return 0; }

		//	IEffectPainter virtuals
		virtual CEffectCreator *GetCreator (void) override { return this; }
		virtual void GetRect (RECT *retRect) const override;
		virtual void Paint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual void PaintHit (CG32bitImage &Dest, int x, int y, const CVector &vHitPos, SViewportPaintCtx &Ctx) override;
		virtual bool PointInImage (int x, int y, int iTick, int iVariant = 0, int iRotation = 0) const override;

	protected:
		virtual IEffectPainter *OnCreatePainter (CCreatePainterCtx &Ctx) override { return this; }
		virtual ALERROR OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) override;
		virtual ALERROR OnEffectBindDesign (SDesignLoadCtx &Ctx) override;

	private:
		enum BeamTypes
			{
			beamUnknown =				-1,

			beamHeavyBlaster =			0,
			beamLaser =					1,
			beamLightning =				2,
			beamStarBlaster =			3,

			beamGreenParticle =			4,
			beamBlueParticle =			5,
			beamElectron =				6,
			beamPositron =				7,
			beamGraviton =				8,
			beamBlaster =				9,
			beamGreenLightning =		10,
			beamParticle =				11,
			beamLightningBolt =			12,
			beamJaggedBolt =			13,
			};

		struct SLineDesc
			{
			int xFrom;
			int yFrom;
			int xTo;
			int yTo;
			};

		void CreateLightningGlow (SLineDesc &Line, int iPointCount, CVector *pPoints, int iSize, CG16bitBinaryRegion *retRegion);
		void DrawBeam (CG32bitImage &Dest, SLineDesc &Line, SViewportPaintCtx &Ctx);
		void DrawBeamBlaster (CG32bitImage &Dest, SLineDesc &Line, SViewportPaintCtx &Ctx);
		void DrawBeamHeavyBlaster (CG32bitImage &Dest, SLineDesc &Line, SViewportPaintCtx &Ctx);
		void DrawBeamLaser (CG32bitImage &Dest, SLineDesc &Line, SViewportPaintCtx &Ctx);
		void DrawBeamLightning (CG32bitImage &Dest, SLineDesc &Line, SViewportPaintCtx &Ctx);
		void DrawBeamLightningBolt (CG32bitImage &Dest, SLineDesc &Line, SViewportPaintCtx &Ctx);
		void DrawBeamParticle (CG32bitImage &Dest, SLineDesc &Line, SViewportPaintCtx &Ctx);
		void DrawBeamStarBlaster (CG32bitImage &Dest, SLineDesc &Line, SViewportPaintCtx &Ctx);

		static BeamTypes ParseBeamType (const CString &sValue);

		BeamTypes m_iType;
		int m_iIntensity;
		CG32bitPixel m_rgbPrimaryColor;
		CG32bitPixel m_rgbSecondaryColor;
	};

class CBoltEffectCreator : public CEffectCreator,
		public IEffectPainter
	{
	public:
		CBoltEffectCreator (void) : IEffectPainter(true)
			{ }

		static CString GetClassTag (void) { return CONSTLIT("Bolt"); }
		virtual CString GetTag (void) override { return GetClassTag(); }

		//	CEffectCreator virtuals
		virtual int GetLifetime (void) override { return 0; }

		//	IEffectPainter virtuals
		virtual CEffectCreator *GetCreator (void) override { return this; }
		virtual void GetRect (RECT *retRect) const override;
		virtual void Paint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual bool PointInImage (int x, int y, int iTick, int iVariant = 0, int iRotation = 0) const override;

	protected:
		virtual IEffectPainter *OnCreatePainter (CCreatePainterCtx &Ctx) override { return this; }
		virtual ALERROR OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) override;
		virtual ALERROR OnEffectBindDesign (SDesignLoadCtx &Ctx) override;

	private:
		int m_iLength;
		int m_iWidth;
		CG32bitPixel m_rgbPrimaryColor;
		CG32bitPixel m_rgbSecondaryColor;
	};

class CDisintegrateEffectCreator : public CEffectCreator
	{
	public:
		CDisintegrateEffectCreator (void);
		~CDisintegrateEffectCreator (void);
			
		virtual CString GetTag (void) override { return GetClassTag(); }

		//	CEffectCreator virtuals
		virtual int GetLifetime (void) override { return 0; }

		static CString GetClassTag (void) { return CONSTLIT("Disintegrate"); }

	protected:
		virtual IEffectPainter *OnCreatePainter (CCreatePainterCtx &Ctx) override;
		virtual ALERROR OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) override;
		virtual ALERROR OnEffectBindDesign (SDesignLoadCtx &Ctx) override;

	private:
		CEffectParamDesc m_Style;			//	Disintegrate style
		CEffectParamDesc m_PrimaryColor;	//	primaryColor: Primary color
		CEffectParamDesc m_SecondaryColor;	//	secondaryColor: Secondary color

		IEffectPainter *m_pSingleton;
	};

class CEffectGroupCreator : public CEffectCreator
	{
	public:
		CEffectGroupCreator (void);

		void ApplyOffsets (SViewportPaintCtx *ioCtx, int *retx, int *rety);
		IEffectPainter *CreateSubPainter (CCreatePainterCtx &Ctx, int iIndex) { return m_pCreators[iIndex].CreatePainter(Ctx); }
		inline int GetCount (void) { return m_iCount; }
		inline CEffectCreator *GetCreator (int iIndex) { return m_pCreators[iIndex]; }
		static CString GetClassTag (void) { return CONSTLIT("Group"); }
		CVector GetOffsetPos (int iRotation);
		inline int GetRotationAdj (void) const { return m_iRotationAdj; }
		virtual CString GetTag (void) { return GetClassTag(); }
		inline bool HasOffsets (void) { return m_bHasOffsets; }

		//	Virtuals

		virtual ~CEffectGroupCreator (void);
		virtual ALERROR CreateEffect (CSystem *pSystem,
									  CSpaceObject *pAnchor,
									  const CVector &vPos,
									  const CVector &vVel,
									  int iRotation,
									  int iVariant = 0,
									  ICCItem *pData = NULL,
									  CSpaceObject **retpEffect = NULL) override;
		virtual int GetLifetime (void) override;
		virtual CEffectCreator *GetSubEffect (int iIndex) override { if (iIndex < 0 || iIndex >= m_iCount) return NULL; return m_pCreators[iIndex]; }
		virtual void SetLifetime (int iLifetime) override;
		virtual void SetVariants (int iVariants) override;

	protected:
		virtual IEffectPainter *OnCreatePainter (CCreatePainterCtx &Ctx) override;
		virtual ALERROR OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) override;
		virtual ALERROR OnEffectBindDesign (SDesignLoadCtx &Ctx) override;
		virtual void OnEffectPlaySound (CSpaceObject *pSource) override;
		virtual void OnEffectMarkResources (void) override;

	private:
		int m_iCount;
		CEffectCreatorRef *m_pCreators;

		bool m_bHasOffsets;
		int m_xOffset;						//	Cartessian coords (pixels)
		int m_yOffset;
		int m_iAngleOffset;
		int m_iRadiusOffset;				//	Pixels
		int m_iRotationAdj;
	};

class CEffectSequencerCreator : public CEffectCreator
	{
	public:
		enum TimeTypes
			{
			timePast,
			timeNow,
			timeFuture,
			};

		virtual ~CEffectSequencerCreator (void);

		static CString GetClassTag (void) { return CONSTLIT("Sequencer"); }
		inline int GetCount (void) const { return m_Timeline.GetCount(); }
		inline CEffectCreator *GetCreator (int iIndex) { return m_Timeline[iIndex].pCreator; }
		virtual CString GetTag (void) override { return GetClassTag(); }
		TimeTypes GetTime (int iIndex, int iStart, int iEnd = -1);
		inline bool IsTime (int iIndex, int iStart, int iEnd) { return (iStart <= m_Timeline[iIndex].iTime) && (m_Timeline[iIndex].iTime <= iEnd); }

		//	CEffectCreator virtuals
		virtual ALERROR CreateEffect (CSystem *pSystem,
									  CSpaceObject *pAnchor,
									  const CVector &vPos,
									  const CVector &vVel,
									  int iRotation,
									  int iVariant = 0,
									  ICCItem *pData = NULL,
									  CSpaceObject **retpEffect = NULL) override;
		virtual int GetLifetime (void) override;
		virtual CEffectCreator *GetSubEffect (int iIndex) override { if (iIndex < 0 || iIndex >= m_Timeline.GetCount()) return NULL; return m_Timeline[iIndex].pCreator; }

	protected:
		virtual IEffectPainter *OnCreatePainter (CCreatePainterCtx &Ctx) override { ASSERT(false); return NULL; }
		virtual ALERROR OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) override;
		virtual ALERROR OnEffectBindDesign (SDesignLoadCtx &Ctx) override;
		virtual void OnEffectMarkResources (void) override;

	private:
		struct SEntry
			{
			int iTime;
			CEffectCreator *pCreator;
			};

		TArray<SEntry> m_Timeline;
	};

class CEffectVariantCreator : public CEffectCreator
	{
	public:
		CEffectVariantCreator (void);
		virtual ~CEffectVariantCreator (void);

		static CString GetClassTag (void) { return CONSTLIT("Variants"); }
		virtual CString GetTag (void) override { return GetClassTag(); }

		inline int GetVariantCount (void) const { return m_Effects.GetCount(); }
		inline CEffectCreator *GetVariantCreator (int iIndex) const { return m_Effects[iIndex].pEffect; }
		inline int GetVariantCreatorIndex (int iVariantValue) { int iIndex; ChooseVariant(iVariantValue, &iIndex); return iIndex; }

		//	CEffectCreator methods
		virtual ALERROR CreateEffect (CSystem *pSystem,
									  CSpaceObject *pAnchor,
									  const CVector &vPos,
									  const CVector &vVel,
									  int iRotation,
									  int iVariant = 0,
									  ICCItem *pData = NULL,
									  CSpaceObject **retpEffect = NULL) override;
		virtual int GetLifetime (void) override;
		virtual CEffectCreator *GetSubEffect (int iIndex) override { if (iIndex < 0 || iIndex >= m_Effects.GetCount()) return NULL; return m_Effects[iIndex].pEffect; }
		virtual void SetLifetime (int iLifetime) override;
		virtual void SetVariants (int iVariants) override;

	protected:
		virtual IEffectPainter *OnCreatePainter (CCreatePainterCtx &Ctx) override;
		virtual ALERROR OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) override;
		virtual ALERROR OnEffectBindDesign (SDesignLoadCtx &Ctx) override;
		virtual void OnEffectMarkResources (void) override;

	private:
		struct SEntry
			{
			CEffectCreator *pEffect;
			int iMaxValue;
			};

		SEntry *ChooseVariant (int iVariantValue, int *retiIndex = NULL);

		TArray<SEntry> m_Effects;
	};

class CEllipseEffectCreator : public CEffectCreator,
		public IEffectPainter
	{
	public:
		CEllipseEffectCreator (void) : IEffectPainter(true)
			{ }

		static CString GetClassTag (void) { return CONSTLIT("Ellipse"); }
		virtual CString GetTag (void) override { return GetClassTag(); }

		//	CEffectCreator virtuals
		virtual int GetLifetime (void) override { return m_iLifetime; }

		//	IEffectPainter virtuals
		virtual CEffectCreator *GetCreator (void) override { return this; }
		virtual void GetRect (RECT *retRect) const override;
		virtual void Paint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual bool PointInImage (int x, int y, int iTick, int iVariant = 0, int iRotation = 0) const override;

	protected:
		virtual IEffectPainter *OnCreatePainter (CCreatePainterCtx &Ctx) override { return this; }
		virtual ALERROR OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) override;

	private:
		CVector GetPoint (Metric rAngle) const;

		Metric m_rSemiMajorAxis;
		Metric m_rEccentricity;
		Metric m_rRotation;

		CG32bitPixel m_rgbLineColor;
		int m_iLineWidth;
		CString m_sLineStyle;

		int m_iLifetime;
	};

class CFlareEffectCreator : public CEffectCreator
	{
	public:
		enum Styles
			{
			styleFadingBlast,							//	Starts bright and fades out
			stylePlain,									//	Constant
			styleFlicker,								//	Blink on and off
			};

		static CString GetClassTag (void) { return CONSTLIT("Flare"); }
		virtual CString GetTag (void) override { return GetClassTag(); }

		void CreateFlareSpike (int iAngle, int iLength, int iWidth, SPoint *Poly);
		inline int GetLifetime (void) const { return m_iLifetime; }
		inline CG32bitPixel GetPrimaryColor (void) const { return m_rgbPrimaryColor; }
		inline int GetRadius (void) const { return m_iRadius; }
		inline CG32bitPixel GetSecondaryColor (void) const { return m_rgbSecondaryColor; }
		inline Styles GetStyle (void) const { return m_iStyle; }

		//	CEffectCreator virtuals
		virtual int GetLifetime (void) override { return m_iLifetime; }
		virtual void SetLifetime (int iLifetime) override { m_iLifetime = iLifetime; }

	protected:
		virtual IEffectPainter *OnCreatePainter (CCreatePainterCtx &Ctx) override;
		virtual ALERROR OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) override;

	private:
		Styles m_iStyle;
		int m_iRadius;									//	Radius in pixels
		int m_iLifetime;								//	Lifetime (ticks)
		CG32bitPixel m_rgbPrimaryColor;
		CG32bitPixel m_rgbSecondaryColor;
	};

class CImageEffectCreator : public CEffectCreator,
		public IEffectPainter
	{
	public:
		CImageEffectCreator (void) : IEffectPainter(true)
			{ }

		static CString GetClassTag (void) { return CONSTLIT("Image"); }
		virtual CString GetTag (void) { return GetClassTag(); }

		inline CCompositeImageDesc &GetImage (void) { return m_Image; }
		inline bool HasRandomStartFrame (void) const { return m_bRandomStartFrame; }
		inline bool ImageRotationNeeded (void) const { return m_bRotateImage; }
		inline bool IsDirectional (void) const { return m_bDirectional; }

		//	CEffectCreator virtuals
		virtual int GetLifetime (void) { return m_iLifetime; }
		virtual void SetVariants (int iVariants);

		//	IEffectPainter virtuals
		virtual bool CanPaintComposite (void) override { return true; }
		virtual CEffectCreator *GetCreator (void) override { return this; }
		virtual bool GetParticlePaintDesc (SParticlePaintDesc *retDesc) override;
		virtual void GetRect (RECT *retRect) const override;
		virtual int GetVariants (void) const override { return m_iVariants; }
		virtual void Paint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual void PaintComposite (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual bool PointInImage (int x, int y, int iTick, int iVariant = 0, int iRotation = 0) const override;

	protected:
		virtual IEffectPainter *OnCreatePainter (CCreatePainterCtx &Ctx) override;
		virtual ALERROR OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) override;
		virtual ALERROR OnEffectBindDesign (SDesignLoadCtx &Ctx) override;
		virtual void OnEffectMarkResources (void) override { m_Image.MarkImage(); }

	private:
		CCompositeImageDesc m_Image;
		int m_iLifetime;
		int m_iVariants;
		bool m_bRotateImage;
		bool m_bRandomStartFrame;
		bool m_bDirectional;
	};

class CImageAndTailEffectCreator : public CEffectCreator,
		public IEffectPainter
	{
	public:
		CImageAndTailEffectCreator (void) : IEffectPainter(true)
			{ }

		static CString GetClassTag (void) { return CONSTLIT("ImageAndTail"); }
		virtual CString GetTag (void) override { return GetClassTag(); }

		//	CEffectCreator virtuals
		virtual int GetLifetime (void) override { return m_iLifetime; }
		virtual void SetVariants (int iVariants) override { m_iVariants = iVariants; }

		//	IEffectPainter virtuals
		virtual CEffectCreator *GetCreator (void) override { return this; }
		virtual void GetRect (RECT *retRect) const override { *retRect = m_Image.GetImageRect(); }
		virtual void Paint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual bool PointInImage (int x, int y, int iTick, int iVariant = 0, int iRotation = 0) const override;

	protected:
		virtual IEffectPainter *OnCreatePainter (CCreatePainterCtx &Ctx) override { return this; }
		virtual ALERROR OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) override;
		virtual ALERROR OnEffectBindDesign (SDesignLoadCtx &Ctx) override;
		virtual void OnEffectMarkResources (void) override { m_Image.MarkImage(); }

	private:
		CObjectImageArray m_Image;
		int m_iLifetime;
		int m_iVariants;
		bool m_bRandomStartFrame;

		int m_iLength;
		int m_iWidth;
		CG32bitPixel m_rgbPrimaryColor;
		CG32bitPixel m_rgbSecondaryColor;
	};

class CImageFractureEffectCreator : public CEffectCreator
	{
	public:
		static CString GetClassTag (void) { return CONSTLIT("ImageFracture"); }
		virtual CString GetTag (void) override { return GetClassTag(); }

		//	CEffectCreator virtuals
		virtual ALERROR CreateEffect (CSystem *pSystem,
									  CSpaceObject *pAnchor,
									  const CVector &vPos,
									  const CVector &vVel,
									  int iRotation,
									  int iVariant = 0,
									  ICCItem *pData = NULL,
									  CSpaceObject **retpEffect = NULL) override;

	protected:
		virtual IEffectPainter *OnCreatePainter (CCreatePainterCtx &Ctx) override { ASSERT(false); return NULL; }
		virtual ALERROR OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) override;
		virtual ALERROR OnEffectBindDesign (SDesignLoadCtx &Ctx) override;
		virtual void OnEffectMarkResources (void) override { m_Image.MarkImage(); }

	private:
		CObjectImageArray m_Image;						//	Images
	};

class CLightningStormEffectCreator : public CEffectCreator
	{
	public:
		CLightningStormEffectCreator (void);
		~CLightningStormEffectCreator (void);
			
		virtual CString GetTag (void) override { return GetClassTag(); }

		//	CEffectCreator virtuals
		virtual int GetLifetime (void) override { return 0; }

		static CString GetClassTag (void) { return CONSTLIT("LightningStorm"); }

	protected:
		virtual IEffectPainter *OnCreatePainter (CCreatePainterCtx &Ctx) override;
		virtual ALERROR OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) override;
		virtual ALERROR OnEffectBindDesign (SDesignLoadCtx &Ctx) override;

	private:
		CEffectParamDesc m_Style;			//	style: Style of storm
		CEffectParamDesc m_Intensity;		//	intensity: Intensity of storm 100 = max; 50 = default
		CEffectParamDesc m_PrimaryColor;	//	primaryColor: Primary color
		CEffectParamDesc m_SecondaryColor;	//	secondaryColor: Secondary color

		CEffectParamDesc m_Lifetime;		//	lifetime: Lifetime in ticks (optional)

		IEffectPainter *m_pSingleton;
	};

class CMoltenBoltEffectCreator : public CEffectCreator,
		public IEffectPainter
	{
	public:
		CMoltenBoltEffectCreator (void) : IEffectPainter(true)
			{ }

		static CString GetClassTag (void) { return CONSTLIT("MoltenBolt"); }
		virtual CString GetTag (void) override { return GetClassTag(); }

		//	CEffectCreator virtuals
		virtual int GetLifetime (void) override { return 0; }

		//	IEffectPainter virtuals
		virtual CEffectCreator *GetCreator (void) override { return this; }
		virtual void GetRect (RECT *retRect) const override;
		virtual void Paint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual bool PointInImage (int x, int y, int iTick, int iVariant = 0, int iRotation = 0) const override;

	protected:
		virtual IEffectPainter *OnCreatePainter (CCreatePainterCtx &Ctx) override { return this; }
		virtual ALERROR OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) override;

	private:
		void CreateBoltShape (int iAngle, int iLength, int iWidth, SPoint *Poly);

		int m_iWidth;
		int m_iLength;
		int m_iGrowth;
		CG32bitPixel m_rgbPrimaryColor;
		CG32bitPixel m_rgbSecondaryColor;
	};

class CNullEffectCreator : public CEffectCreator,
		public IEffectPainter
	{
	public:
		CNullEffectCreator (void) : IEffectPainter(true)
			{ }

		static CString GetClassTag (void) { return CONSTLIT("Null"); }
		virtual CString GetTag (void) override { return GetClassTag(); }

		//	CEffectCreator virtuals
		virtual int GetLifetime (void) override { return 0; }

		//	IEffectPainter virtuals
		virtual CEffectCreator *GetCreator (void) override { return this; }
		virtual void Paint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override { }

	protected:
		virtual IEffectPainter *OnCreatePainter (CCreatePainterCtx &Ctx) override { return this; }

	};

class COrbEffectCreator : public CEffectCreator
	{
	public:
		COrbEffectCreator (void);
		~COrbEffectCreator (void);
			
		virtual CString GetTag (void) override { return GetClassTag(); }

		//	CEffectCreator virtuals
		virtual int GetLifetime (void) override { return 0; }

		static CString GetClassTag (void) { return CONSTLIT("Orb"); }

	protected:
		virtual IEffectPainter *OnCreatePainter (CCreatePainterCtx &Ctx) override;
		virtual ALERROR OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) override;
		virtual ALERROR OnEffectBindDesign (SDesignLoadCtx &Ctx) override;

	private:
		CEffectParamDesc m_Radius;			//	radius: Radius of orb (pixels)
		CEffectParamDesc m_Style;			//	style: Style of ray
		CEffectParamDesc m_Intensity;		//	intensity: Intensity of orb
		CEffectParamDesc m_Detail;			//	detail: Level of detail for explosions (0-100)
		CEffectParamDesc m_Distortion;		//	distortion: Distortion of orb (for explosions)
		CEffectParamDesc m_PrimaryColor;	//	primaryColor: Primary color
		CEffectParamDesc m_SecondaryColor;	//	secondaryColor: Secondary color
		CEffectParamDesc m_BlendMode;		//	blendMode: Blend mode
		CEffectParamDesc m_SpikeCount;		//	spikeCount: Optional number of spikes (for flares)

		CEffectParamDesc m_Animate;			//	animate: Animation styles
		CEffectParamDesc m_Lifetime;		//	lifetime: Lifetime in ticks (optional)

		IEffectPainter *m_pSingleton;
	};

class CParticleCloudEffectCreator : public CEffectCreator
	{
	public:
		enum Styles
			{
			styleCloud,
			styleRing,
			styleSplash,
			styleJet,
			styleExhaust,
			};

		CParticleCloudEffectCreator (void) : m_pParticleEffect(NULL)
			{ }

		static CString GetClassTag (void) { return CONSTLIT("ParticleCloud"); }
		virtual CString GetTag (void) override { return GetClassTag(); }

		inline Metric GetDrag (void) const { return m_rDrag; }
		inline int GetCohesion (void) const { return m_iCohesion; }
		inline int GetEmitLifetime (void) const { return m_iEmitLifetime; }
		inline int GetEmitRotation (void) const { return m_iEmitRotation; }
		Metric GetEmitSpeed (void) const;
		inline Metric GetMaxRadius (void) const { return m_rMaxRadius; }
		inline Metric GetMinRadius (void) const { return m_rMinRadius; }
		inline int GetNewParticleCount (void) const { return m_NewParticles.Roll(); }
		inline int GetNewParticleMax (void) const { return m_NewParticles.GetMaxValue(); }
		inline int GetParticleCount (void) const { return m_ParticleCount.Roll(); }
		inline int GetParticleCountMax (void) const { return m_ParticleCount.GetMaxValue(); }
		inline CEffectCreator *GetParticleEffect (void) const { return m_pParticleEffect; }
		inline int GetParticleLifetime (void) const { return (m_rSlowMotionFactor == 1.0 ? m_ParticleLifetime.Roll() : (int)((m_ParticleLifetime.Roll() / m_rSlowMotionFactor))); }
		inline int GetParticleLifetimeMax (void) const { return Max(1, (m_rSlowMotionFactor == 1.0 ? m_ParticleLifetime.GetMaxValue() : (int)(m_ParticleLifetime.GetMaxValue() / m_rSlowMotionFactor))); }
		inline Metric GetRingRadius (void) const { return m_rRingRadius; }
		inline Metric GetSlowMotionFactor (void) const { return m_rSlowMotionFactor; }
		inline int GetSpreadAngle (void) const { return m_Spread.Roll(); }
		inline Styles GetStyle (void) const { return m_iStyle; }
		inline int GetViscosity (void) const { return m_iViscosity; }
		inline int GetWakePotential (void) const { return m_iWakePotential; }

		//	CEffectCreator virtuals
		virtual ~CParticleCloudEffectCreator (void) override;
		virtual int GetLifetime (void) override { return m_Lifetime.Roll(); }
		virtual void SetLifetime (int iLifetime) override { m_Lifetime.SetConstant(iLifetime); }

	protected:
		virtual IEffectPainter *OnCreatePainter (CCreatePainterCtx &Ctx) override;
		virtual ALERROR OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) override;
		virtual ALERROR OnEffectBindDesign (SDesignLoadCtx &Ctx) override;

	private:
		Styles m_iStyle;								//	Effect style
		Metric m_rSlowMotionFactor;						//	Slow motion
		DiceRange m_Lifetime;							//	Lifetime of effect
		DiceRange m_ParticleCount;						//	Initial number of particles
		DiceRange m_ParticleLifetime;					//	Lifetime of each particle

		int m_iEmitLifetime;							//	% time that it emits particles
		int m_iEmitRotation;							//	Rotation
		DiceRange m_NewParticles;						//	Number of new particles per tick
		DiceRange m_InitSpeed;							//	Initial speed of each particle
		DiceRange m_Spread;								//	Spread angle (for jet and exhaust)

		Metric m_rRingRadius;							//	If non-zero, particles form a ring at this radius
		Metric m_rMaxRadius;							//	If RingRadius is non-zero, this is the outer edge of ring
														//		Otherwise, it is the max radius of a sphere
		Metric m_rMinRadius;							//	If RingRadius is non-zero, this is the inner edge of ring
														//		Otherwise, it is ignored

		int m_iCohesion;								//	Strength of force keeping particles together (0-100)
		int m_iViscosity;								//	Drag on particles while inside bounds (0-100)
		int m_iWakePotential;							//	Influence of moving objects (0-100)

		Metric m_rDrag;									//	Drag when source object is moving

		CEffectCreator *m_pParticleEffect;				//	Effect to use to paint particles
	};

class CParticlePatternEffectCreator : public CEffectCreator
	{
	public:
		CParticlePatternEffectCreator (void);
		virtual ~CParticlePatternEffectCreator (void);

		inline CEffectCreator *GetParticleEffect (void) const { return m_pParticleEffect; }

		static CString GetClassTag (void) { return CONSTLIT("ParticlePattern"); }
		virtual CString GetTag (void) override { return GetClassTag(); }

		//	CEffectCreator virtuals
		virtual int GetLifetime (void) override { return -1; }

		static const TArray<CVector> &GetSplinePoints (void);

	protected:
		virtual IEffectPainter *OnCreatePainter (CCreatePainterCtx &Ctx) override;
		virtual ALERROR OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) override;
		virtual ALERROR OnEffectBindDesign (SDesignLoadCtx &Ctx) override;

	private:
		CEffectParamDesc m_Style;			//	Pattern style
		CEffectParamDesc m_Length;
		CEffectParamDesc m_Lifetime;
		CEffectParamDesc m_ParticleCount;
		CEffectParamDesc m_PrimaryColor;
		CEffectParamDesc m_SecondaryColor;
		CEffectParamDesc m_Width;
		CEffectParamDesc m_SpreadAngle;		//	Full angle of spread
		CEffectParamDesc m_ParticleSpeed;	//	Speed of particles along path
		CEffectParamDesc m_JitterLength;

		CEffectCreator *m_pParticleEffect;	//	Effect to use to paint particles (may be NULL)

		IEffectPainter *m_pSingleton;
	};

class CParticleExplosionEffectCreator : public CEffectCreator
	{
	public:
		static CString GetClassTag (void) { return CONSTLIT("ParticleExplosion"); }
		virtual CString GetTag (void) override { return GetClassTag(); }

		//	CEffectCreator virtuals
		virtual ALERROR CreateEffect (CSystem *pSystem,
									  CSpaceObject *pAnchor,
									  const CVector &vPos,
									  const CVector &vVel,
									  int iRotation,
									  int iVariant = 0,
									  ICCItem *pData = NULL,
									  CSpaceObject **retpEffect = NULL) override;

	protected:
		virtual IEffectPainter *OnCreatePainter (CCreatePainterCtx &Ctx) override { return NULL; }
		virtual ALERROR OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) override;
		virtual ALERROR OnEffectBindDesign (SDesignLoadCtx &Ctx) override;
		virtual void OnEffectMarkResources (void) override { m_Image.MarkImage(); }

	private:
		DiceRange m_Lifetime;							//	Total lifetime
		DiceRange m_ParticleCount;						//	Number of particles
		Metric m_rParticleSpeed;						//	Speed of particles
		int m_iParticleLifetime;						//	Particle lifespan
		CObjectImageArray m_Image;						//	Images
	};

class CParticleSystemEffectCreator : public CEffectCreator
	{
	public:
		CParticleSystemEffectCreator (void);
		~CParticleSystemEffectCreator (void);
			
		inline CEffectCreator *GetParticleEffect (void) const { return m_pParticleEffect; }

		virtual CString GetTag (void) override { return GetClassTag(); }

		//	CEffectCreator virtuals
		virtual int GetLifetime (void) override { return 0; }

		static CString GetClassTag (void) { return CONSTLIT("ParticleSystem"); }

	protected:
		virtual IEffectPainter *OnCreatePainter (CCreatePainterCtx &Ctx) override;
		virtual ALERROR OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) override;
		virtual ALERROR OnEffectBindDesign (SDesignLoadCtx &Ctx) override;

	private:
		CEffectParamDesc m_Style;			//	System style
		CEffectParamDesc m_FixedPos;		//	Particles fixed on background (not obj)

		CEffectParamDesc m_Cohesion;		//	Tendency to revert to original shap (0-100)
		CEffectParamDesc m_EmitChance;		//	Probability of emitting on a given tick
		CEffectParamDesc m_EmitRate;		//	Particles to create per tick
		CEffectParamDesc m_EmitSpeed;		//	Particle speed (% of lightspeed)
		CEffectParamDesc m_EmitLifetime;	//	Emissions lasts for this many ticks
		CEffectParamDesc m_EmitWidth;		//	Emission width
		CEffectParamDesc m_ParticleLifetime;//	In ticks
		CEffectParamDesc m_Radius;			//	Radius of system
		CEffectParamDesc m_SpreadAngle;		//	Full angle of spread
		CEffectParamDesc m_TangentSpeed;	//	Trangental speed (if spread angle is omitted)

		CEffectParamDesc m_Lifetime;		//	lifetime: Lifetime in ticks (optional)
		CEffectParamDesc m_WakePotential;	//	Wake when objects move through (0 = none, 100 = max)
		CEffectParamDesc m_XformRotation;	//	XformRotation: Rotations (degrees)
		CEffectParamDesc m_XformTime;		//	XformTime: 100 = normal speed; <100 = slower

		CEffectCreator *m_pParticleEffect;	//	Effect to use to paint particles

		IEffectPainter *m_pSingleton;
	};

class CPlasmaSphereEffectCreator : public CEffectCreator,
		public IEffectPainter
	{
	public:
		CPlasmaSphereEffectCreator (void) : IEffectPainter(true)
			{ }

		static CString GetClassTag (void) { return CONSTLIT("PlasmaSphere"); }
		virtual CString GetTag (void) override { return GetClassTag(); }

		//	CEffectCreator virtuals
		virtual int GetLifetime (void) override { return 0; }

		//	IEffectPainter virtuals
		virtual CEffectCreator *GetCreator (void) override { return this; }
		virtual void GetRect (RECT *retRect) const override;
		virtual void Paint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual bool PointInImage (int x, int y, int iTick, int iVariant = 0, int iRotation = 0) const override;

	protected:
		virtual IEffectPainter *OnCreatePainter (CCreatePainterCtx &Ctx) override { return this; }
		virtual ALERROR OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) override;

	private:
		DiceRange m_Radius;
		DiceRange m_SpikeCount;
		DiceRange m_SpikeLength;
		CG32bitPixel m_rgbPrimaryColor;
		CG32bitPixel m_rgbSecondaryColor;
	};

class CPolyflashEffectCreator : public CEffectCreator
	{
	public:
		static CString GetClassTag (void) { return CONSTLIT("Polyflash"); }
		virtual CString GetTag (void) override { return GetClassTag(); }

		//	CEffectCreator virtuals
		virtual int GetLifetime (void) override { return 1; }

	protected:
		virtual IEffectPainter *OnCreatePainter (CCreatePainterCtx &Ctx) override;
		virtual ALERROR OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) override { return NOERROR; }
	};

class CRayEffectCreator : public CEffectCreator
	{
	public:
		CRayEffectCreator (void);
		~CRayEffectCreator (void);
			
		virtual CString GetTag (void) override { return GetClassTag(); }

		//	CEffectCreator virtuals
		virtual int GetLifetime (void) override { return 0; }

		static CString GetClassTag (void) { return CONSTLIT("Ray"); }

	protected:
		virtual IEffectPainter *OnCreatePainter (CCreatePainterCtx &Ctx) override;
		virtual ALERROR OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) override;
		virtual ALERROR OnEffectBindDesign (SDesignLoadCtx &Ctx) override;

	private:
		CEffectParamDesc m_Length;			//	length: Length of ray (pixels)
		CEffectParamDesc m_Width;			//	width: Width of ray (pixels)
		CEffectParamDesc m_Shape;			//	shape: Shape of ray
		CEffectParamDesc m_Style;			//	style: Style of ray
		CEffectParamDesc m_Intensity;		//	intensity: Intensity of ray
		CEffectParamDesc m_PrimaryColor;	//	primaryColor: Primary color
		CEffectParamDesc m_SecondaryColor;	//	secondaryColor: Secondary color
		CEffectParamDesc m_BlendMode;		//	blendMode: Blend mode

		CEffectParamDesc m_Lifetime;		//	lifetime: Lifetime in ticks (optional)
		CEffectParamDesc m_XformRotation;	//	XformRotation: Rotations (degrees)

		CEffectParamDesc m_AnimateOpacity;	//	animate opacity

		IEffectPainter *m_pSingleton;
	};

class CShapeEffectCreator : public CEffectCreator
	{
	public:
		CShapeEffectCreator (void) : m_Points(NULL), m_TransBuffer(NULL) { }
		static CString GetClassTag (void) { return CONSTLIT("Shape"); }
		virtual CString GetTag (void) override { return GetClassTag(); }

		void CreateShapeRegion (int iAngle, int iLength, int iWidth, CG16bitBinaryRegion *pRegion);
		inline CGDraw::EBlendModes GetBlendMode (void) const { return m_iBlendMode; }
		inline CG32bitPixel GetColor (void) const { return m_rgbColor; }
		inline int GetLength (void) const { return m_iLength; }
		inline int GetLengthInc (void) const { return m_iLengthInc; }
		inline DWORD GetOpacity (void) const { return m_byOpacity; }
		inline int GetWidth (void) const { return m_iWidth; }
		inline int GetWidthInc (void) const { return m_iWidthInc; }
		inline bool IsDirectional (void) const { return m_bDirectional; }

		//	CEffectCreator virtuals
		virtual ~CShapeEffectCreator (void);
		virtual int GetLifetime (void) override { return 0; }

	protected:
		virtual IEffectPainter *OnCreatePainter (CCreatePainterCtx &Ctx) override;
		virtual ALERROR OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) override;

	private:
		int m_iWidth;								//	Scale factor: pixel width of 100 units
		int m_iLength;								//	Scale factor: pixel length of 100 units

		bool m_bDirectional;						//	If TRUE, rotate shape based on Ctx.iRotation
		int m_iWidthInc;							//	Increment width every tick
		int m_iLengthInc;							//	Increment length every tick

		CG32bitPixel m_rgbColor;								//	Shape color
		DWORD m_byOpacity;							//	Shape opacity
		CGDraw::EBlendModes m_iBlendMode;

		int m_iPointCount;
		SPoint *m_Points;
		bool m_bConvexPolygon;						//	TRUE if simple polygon

		SPoint *m_TransBuffer;						//	Buffer used for transforms
	};

class CShockwaveEffectCreator : public CEffectCreator
	{
	public:
		CShockwaveEffectCreator (void);
		~CShockwaveEffectCreator (void);
			
		virtual CString GetTag (void) override { return GetClassTag(); }

		//	CEffectCreator virtuals
		virtual int GetLifetime (void) override { return m_Lifetime.EvalDiceRange().GetAveValue(); }
		virtual void SetLifetime (int iLifetime) override { m_Lifetime.InitInteger(iLifetime); }

		static CString GetClassTag (void) { return CONSTLIT("Shockwave"); }

	protected:
		virtual IEffectPainter *OnCreatePainter (CCreatePainterCtx &Ctx) override;
		virtual ALERROR OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) override;
		virtual ALERROR OnEffectBindDesign (SDesignLoadCtx &Ctx) override;
		virtual void OnEffectMarkResources (void) override;

	private:
		CEffectParamDesc m_Style;						//	Style of effect
		CEffectParamDesc m_Image;						//	Shockwave image
		CEffectParamDesc m_Speed;						//	Expansion speed (% of lightspeed)
		CEffectParamDesc m_Lifetime;					//	Lifetime (ticks)
		CEffectParamDesc m_FadeStart;					//	% of lifetime at which we start to fade
		CEffectParamDesc m_Width;						//	Width of central ring
		CEffectParamDesc m_Intensity;					//	Intensity (0-100)
		CEffectParamDesc m_GlowWidth;					//	Glow width
		CEffectParamDesc m_WaveCount;					//	Number of waves
		CEffectParamDesc m_WaveInterval;				//	Ticks between waves
		CEffectParamDesc m_WaveLifetime;				//	Lifetime of a single wave (ticks)
		CEffectParamDesc m_BlendMode;					//	blendMode: Blend mode
		CEffectParamDesc m_PrimaryColor;
		CEffectParamDesc m_SecondaryColor;

		IEffectPainter *m_pSingleton;
	};

class CSingleParticleEffectCreator : public CEffectCreator
	{
	public:
		CSingleParticleEffectCreator (void);
		~CSingleParticleEffectCreator (void);

		static CString GetClassTag (void) { return CONSTLIT("Particle"); }
		virtual CString GetTag (void) override { return GetClassTag(); }

	protected:
		virtual IEffectPainter *OnCreatePainter (CCreatePainterCtx &Ctx) override;
		virtual ALERROR OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) override;

	private:
		CEffectParamDesc m_Style;
		CEffectParamDesc m_MinWidth;
		CEffectParamDesc m_MaxWidth;
		CEffectParamDesc m_PrimaryColor;
		CEffectParamDesc m_SecondaryColor;

		IEffectPainter *m_pSingleton;
	};

class CSmokeTrailEffectCreator : public CEffectCreator
	{
	public:
		static CString GetClassTag (void) { return CONSTLIT("SmokeTrail"); }
		virtual CString GetTag (void) override { return GetClassTag(); }

		inline int GetEmitLifetime (void) const { return m_iEmitLifetime; }
		Metric GetEmitSpeed (void) const;
		inline int GetNewParticleCount (void) const { return m_NewParticles.Roll(); }
		inline int GetNewParticleMax (void) const { return m_NewParticles.GetMaxValue(); }
		inline CEffectCreator *GetParticleEffect (void) const { return m_pParticleEffect; }
		inline int GetParticleLifetime (void) const { return Max(1, m_ParticleLifetime.Roll()); }
		inline int GetParticleLifetimeMax (void) const { return Max(1, m_ParticleLifetime.GetMaxValue()); }
		inline int GetRotation (void) const { return m_iRotation; }
		inline int GetSpread (void) const { return m_Spread.Roll(); }

		//	CEffectCreator virtuals
		virtual ~CSmokeTrailEffectCreator (void) override;
		virtual int GetLifetime (void) override { return m_iLifetime; }
		virtual void SetLifetime (int iLifetime) override { m_iLifetime = iLifetime; }

	protected:
		virtual IEffectPainter *OnCreatePainter (CCreatePainterCtx &Ctx) override;
		virtual ALERROR OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) override;
		virtual ALERROR OnEffectBindDesign (SDesignLoadCtx &Ctx) override;

	private:
		DiceRange m_Spread;								//	Random spread as % of particle speed (per particle)
		DiceRange m_NewParticles;						//	Number of new particles per tick
		DiceRange m_InitSpeed;							//	Initial speed of each particle
		DiceRange m_ParticleLifetime;					//	Lifetime of each particle

		int m_iLifetime;								//	Lifetime of effect
		int m_iEmitLifetime;							//	% time that it emits particles
		int m_iRotation;								//	Direction of emission (defaults to 180)

		CEffectCreator *m_pParticleEffect;				//	Effect to use to paint particles
	};

class CStarburstEffectCreator : public CEffectCreator,
		public IEffectPainter
	{
	public:
		CStarburstEffectCreator (void) : IEffectPainter(true)
			{ }

		static CString GetClassTag (void) { return CONSTLIT("Starburst"); }
		virtual CString GetTag (void) override { return GetClassTag(); }

		//	CEffectCreator virtuals
		virtual int GetLifetime (void) override { return m_iLifetime; }

		//	IEffectPainter virtuals
		virtual CEffectCreator *GetCreator (void) override { return this; }
		virtual void GetRect (RECT *retRect) const override;
		virtual void Paint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual bool PointInImage (int x, int y, int iTick, int iVariant = 0, int iRotation = 0) const override;

	protected:
		virtual IEffectPainter *OnCreatePainter (CCreatePainterCtx &Ctx) override { return this; }
		virtual ALERROR OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) override;

	private:
		enum Styles
			{
			stylePlain,									//	Random spikes from center point
			styleMorningStar,							//	Sphere with random triangular spikes
			styleLightningStar,							//	Sphere with tail of lightning
			styleFlare,									//	Constant glow plus spikes
			styleBallLightning,							//	Constant glow plus curving lightning
			};

		void CreateDiamondSpike (int iAngle, int iLength, int iWidthAngle, SPoint *Poly);

		Styles m_iStyle;
		DiceRange m_SpikeCount;
		DiceRange m_SpikeLength;
		CG32bitPixel m_rgbPrimaryColor;
		CG32bitPixel m_rgbSecondaryColor;
		int m_iLifetime;
	};

class CTextEffectCreator : public CEffectCreator
	{
	public:
		CTextEffectCreator (void) : m_pFont(NULL),
				m_byOpacity(0xff),
				m_dwAlignment(CG16bitFont::AlignCenter),
				m_iLifetime(-1)
			{ }

		static CString GetClassTag (void) { return CONSTLIT("Text"); }
		virtual CString GetTag (void) override { return GetClassTag(); }

		inline const CG16bitFont *GetFont (void) { return m_pFont; }
		inline DWORD GetFontFlags (void) { return m_dwAlignment; }
		inline DWORD GetOpacity (void) { return m_byOpacity; }
		inline CG32bitPixel GetPrimaryColor (void) { return m_rgbPrimaryColor; }

		//	CEffectCreator virtuals
		virtual int GetLifetime (void) override { return m_iLifetime; }

	protected:
		virtual IEffectPainter *OnCreatePainter (CCreatePainterCtx &Ctx) override;
		virtual ALERROR OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) override;

	private:
		CString m_sDefaultText;

		const CG16bitFont *m_pFont;
		CG32bitPixel m_rgbPrimaryColor;
		DWORD m_byOpacity;
		DWORD m_dwAlignment;

		int m_iLifetime;
	};

//	Space Object Implementations -----------------------------------------------

class CSequencerEffect : public CSpaceObject
	{
	public:
		static ALERROR Create (CSystem *pSystem,
							   CEffectSequencerCreator *pType,
							   CSpaceObject *pAnchor,
							   const CVector &vPos,
							   const CVector &vVel,
							   CSequencerEffect **retpEffect);

	protected:
		//	CSpaceObject virtuals
		virtual bool CanHit (CSpaceObject *pObj) override { return false; }
		virtual CString GetObjClassName (void) override { return CONSTLIT("CSequencerEffect"); }
		virtual void ObjectDestroyedHook (const SDestroyCtx &Ctx) override;
		virtual void OnReadFromStream (SLoadCtx &Ctx) override;
		virtual void OnWriteToStream (IWriteStream *pStream) override;
		virtual void OnUpdate (SUpdateCtx &Ctx, Metric rSecondsPerTick) override;
		virtual void PaintLRSBackground (CG32bitImage &Dest, int x, int y, const ViewportTransform &Trans) override { }
		virtual void PaintLRSForeground (CG32bitImage &Dest, int x, int y, const ViewportTransform &Trans) override { }

	private:
		CSequencerEffect (void);

		CEffectSequencerCreator *m_pType;
		CSpaceObject *m_pAnchor;
		CVector m_vAnchorOffset;
		int m_iStartTime;
		int m_iTimeCursor;

	friend CObjectClass<CSequencerEffect>;
	};

#endif
