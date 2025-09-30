//	TSESFXImpl.h
//
//	Transcendence IEffectPainter classes
//	Copyright (c) 2021 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

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

		BeamTypes m_iType = beamUnknown;
		int m_iIntensity = 100;
		CG32bitPixel m_rgbPrimaryColor = CG32bitPixel(255, 255, 255);
		CG32bitPixel m_rgbSecondaryColor = CG32bitPixel(128, 128, 128);
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
		int m_iLength = 10;
		int m_iWidth = 1;
		CG32bitPixel m_rgbPrimaryColor = CG32bitPixel(255, 255, 255);
		CG32bitPixel m_rgbSecondaryColor = CG32bitPixel(128, 128, 128);
	};

class CDisintegrateEffectCreator : public CEffectCreator
	{
	public:
		CDisintegrateEffectCreator (void) { }
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

		IEffectPainter *m_pSingleton = NULL;
	};

class CEffectGroupCreator : public CEffectCreator
	{
	public:
		CEffectGroupCreator (void) { }

		void ApplyOffsets (SViewportPaintCtx *ioCtx, int *retx, int *rety);
		IEffectPainter *CreateSubPainter (CCreatePainterCtx &Ctx, int iIndex) { return m_pCreators[iIndex].CreatePainter(Ctx); }
		int GetCount (void) { return m_iCount; }
		CEffectCreator *GetCreator (int iIndex) { return m_pCreators[iIndex]; }
		static CString GetClassTag (void) { return CONSTLIT("Group"); }
		CVector GetOffsetPos (int iRotation);
		int GetRotationAdj (void) const { return m_iRotationAdj; }
		virtual CString GetTag (void) override { return GetClassTag(); }
		bool HasOffsets (void) { return m_bHasOffsets; }

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
		virtual void OnEffectPlaySound (CSpaceObject *pSource, SSoundOptions *pOptions) override;
		virtual void OnEffectMarkResources (void) override;

	private:
		int m_iCount = 0;
		CEffectCreatorRef *m_pCreators = NULL;

		bool m_bHasOffsets = false;
		int m_xOffset = 0;						//	Cartessian coords (pixels)
		int m_yOffset = 0;
		int m_iAngleOffset = 0;
		int m_iRadiusOffset = 0;				//	Pixels
		int m_iRotationAdj = 0;
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
		int GetCount (void) const { return m_Timeline.GetCount(); }
		CEffectCreator *GetCreator (int iIndex) { return m_Timeline[iIndex].pCreator; }
		virtual CString GetTag (void) override { return GetClassTag(); }
		TimeTypes GetTime (int iIndex, int iStart, int iEnd = -1);
		bool IsTime (int iIndex, int iStart, int iEnd) { return (iStart <= m_Timeline[iIndex].iTime) && (m_Timeline[iIndex].iTime <= iEnd); }

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
		CEffectVariantCreator (void) { }
		virtual ~CEffectVariantCreator (void);

		static CString GetClassTag (void) { return CONSTLIT("Variants"); }
		virtual CString GetTag (void) override { return GetClassTag(); }

		int GetVariantCount (void) const { return m_Effects.GetCount(); }
		CEffectCreator *GetVariantCreator (int iIndex) const { return m_Effects[iIndex].pEffect; }
		int GetVariantCreatorIndex (int iVariantValue) { int iIndex; ChooseVariant(iVariantValue, &iIndex); return iIndex; }

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

		Metric m_rSemiMajorAxis = 10.0;
		Metric m_rEccentricity = 0.0;
		Metric m_rRotation = 0.0;

		CG32bitPixel m_rgbLineColor = CG32bitPixel(255, 255, 255);
		int m_iLineWidth = 1;
		CString m_sLineStyle;

		int m_iLifetime = 0;
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
		int GetLifetime (void) const { return m_iLifetime; }
		CG32bitPixel GetPrimaryColor (void) const { return m_rgbPrimaryColor; }
		int GetRadius (void) const { return m_iRadius; }
		CG32bitPixel GetSecondaryColor (void) const { return m_rgbSecondaryColor; }
		Styles GetStyle (void) const { return m_iStyle; }

		//	CEffectCreator virtuals
		virtual int GetLifetime (void) override { return m_iLifetime; }
		virtual void SetLifetime (int iLifetime) override { m_iLifetime = iLifetime; }

	protected:
		virtual IEffectPainter *OnCreatePainter (CCreatePainterCtx &Ctx) override;
		virtual ALERROR OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) override;

	private:
		Styles m_iStyle = stylePlain;
		int m_iRadius = 10;									//	Radius in pixels
		int m_iLifetime = 0;								//	Lifetime (ticks)
		CG32bitPixel m_rgbPrimaryColor = CG32bitPixel(255, 255, 255);
		CG32bitPixel m_rgbSecondaryColor = CG32bitPixel(128, 128, 128);
	};

class CGlowEffectCreator : public CEffectCreator
	{
	public:
		CGlowEffectCreator (void) { }
		CGlowEffectCreator (const CGlowEffectCreator &Src) = delete;
		CGlowEffectCreator (CGlowEffectCreator &&Src) = delete;
		~CGlowEffectCreator (void);

		CGlowEffectCreator &operator= (CGlowEffectCreator &&Src) = delete;
		CGlowEffectCreator &operator= (const CGlowEffectCreator &Src) = delete;
			
		virtual CString GetTag (void) override { return GetClassTag(); }

		//	CEffectCreator virtuals
		virtual int GetLifetime (void) override { return 0; }

		static CString GetClassTag (void) { return CONSTLIT("Glow"); }

	protected:
		virtual IEffectPainter *OnCreatePainter (CCreatePainterCtx &Ctx) override;
		virtual ALERROR OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) override;
		virtual ALERROR OnEffectBindDesign (SDesignLoadCtx &Ctx) override;

	private:
		CEffectParamDesc m_Style;			//	style: Effect style
		CEffectParamDesc m_Radius;			//	radius: Radius of glow (pixels)
		CEffectParamDesc m_PrimaryColor;	//	primaryColor: Primary color
		CEffectParamDesc m_SecondaryColor;	//	secondaryColor: Secondary color
		CEffectParamDesc m_BlendMode;		//	blendMode: Blend mode

		CEffectParamDesc m_Lifetime;		//	lifetime: Lifetime in ticks (optional)
		CEffectParamDesc m_Animate;			//	animate: Animation style

		IEffectPainter *m_pSingleton = NULL;
	};

class CImageEffectCreator : public CEffectCreator,
		public IEffectPainter
	{
	public:
		CImageEffectCreator (void) : IEffectPainter(true)
			{ }

		static CString GetClassTag (void) { return CONSTLIT("Image"); }
		virtual CString GetTag (void) override { return GetClassTag(); }

		CCompositeImageDesc &GetImage (void) { return m_Image; }
		bool HasRandomStartFrame (void) const { return m_bRandomStartFrame; }
		bool ImageRotationNeeded (void) const { return m_bRotateImage; }
		bool IsDirectional (void) const { return m_bDirectional; }

		//	CEffectCreator virtuals
		virtual int GetLifetime (void) override { return m_iLifetime; }
		virtual void SetVariants (int iVariants) override;

		//	IEffectPainter virtuals
		virtual bool CanPaintComposite (void) override { return true; }
		virtual CEffectCreator *GetCreator (void) override { return this; }
		virtual const CObjectImageArray &GetImage (int iRotation, int *retiRotationFrameIndex = NULL) const;
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
		int m_iLifetime = 0;
		int m_iVariants = 0;
		bool m_bRotateImage = false;
		bool m_bRandomStartFrame = false;
		bool m_bDirectional = false;
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
		int m_iLifetime = 0;
		int m_iVariants = 0;
		bool m_bRandomStartFrame = false;

		int m_iLength = 10;
		int m_iWidth = 1;
		CG32bitPixel m_rgbPrimaryColor = CG32bitPixel(255, 255, 255);
		CG32bitPixel m_rgbSecondaryColor = CG32bitPixel(128, 128, 128);
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
		CLightningStormEffectCreator (void) { }
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

		IEffectPainter *m_pSingleton = NULL;
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

		int m_iWidth = 1;
		int m_iLength = 10;
		int m_iGrowth = 0;
		CG32bitPixel m_rgbPrimaryColor = CG32bitPixel(255, 255, 255);
		CG32bitPixel m_rgbSecondaryColor = CG32bitPixel(128, 128, 128);
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
		COrbEffectCreator (void) { }
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
		CEffectParamDesc m_Radius;				//	radius: Radius of orb (pixels)
		CEffectParamDesc m_Style;				//	style: Style of ray
		CEffectParamDesc m_Intensity;			//	intensity: Intensity of orb
		CEffectParamDesc m_Detail;				//	detail: Level of detail for explosions (0-100)
		CEffectParamDesc m_Distortion;			//	distortion: Distortion of orb (for explosions)
		CEffectParamDesc m_PrimaryColor;		//	primaryColor: Primary color
		CEffectParamDesc m_SecondaryColor;		//	secondaryColor: Secondary color
		CEffectParamDesc m_Opacity;				//	opacity: 0-255
		CEffectParamDesc m_SecondaryOpacity;	//	secondaryOpacity: 0-255, for textures
		CEffectParamDesc m_BlendMode;			//	blendMode: Blend mode
		CEffectParamDesc m_SpikeCount;			//	spikeCount: Optional number of spikes (for flares)

		CEffectParamDesc m_Animate;				//	animate: Animation styles
		CEffectParamDesc m_Lifetime;			//	lifetime: Lifetime in ticks (optional)

		IEffectPainter *m_pSingleton = NULL;
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

		CParticleCloudEffectCreator (void) { }

		static CString GetClassTag (void) { return CONSTLIT("ParticleCloud"); }
		virtual CString GetTag (void) override { return GetClassTag(); }

		Metric GetDrag (void) const { return m_rDrag; }
		int GetCohesion (void) const { return m_iCohesion; }
		int GetEmitLifetime (void) const { return m_iEmitLifetime; }
		int GetEmitRotation (void) const { return m_iEmitRotation; }
		Metric GetEmitSpeed (void) const;
		Metric GetMaxRadius (void) const { return m_rMaxRadius; }
		Metric GetMinRadius (void) const { return m_rMinRadius; }
		int GetNewParticleCount (void) const { return m_NewParticles.Roll(); }
		int GetNewParticleMax (void) const { return m_NewParticles.GetMaxValue(); }
		int GetParticleCount (void) const { return m_ParticleCount.Roll(); }
		int GetParticleCountMax (void) const { return m_ParticleCount.GetMaxValue(); }
		CEffectCreator *GetParticleEffect (void) const { return m_pParticleEffect; }
		int GetParticleLifetime (void) const { return (m_rSlowMotionFactor == 1.0 ? m_ParticleLifetime.Roll() : (int)((m_ParticleLifetime.Roll() / m_rSlowMotionFactor))); }
		int GetParticleLifetimeMax (void) const { return Max(1, (m_rSlowMotionFactor == 1.0 ? m_ParticleLifetime.GetMaxValue() : (int)(m_ParticleLifetime.GetMaxValue() / m_rSlowMotionFactor))); }
		Metric GetRingRadius (void) const { return m_rRingRadius; }
		Metric GetSlowMotionFactor (void) const { return m_rSlowMotionFactor; }
		int GetSpreadAngle (void) const { return m_Spread.Roll(); }
		Styles GetStyle (void) const { return m_iStyle; }
		int GetViscosity (void) const { return m_iViscosity; }
		int GetWakePotential (void) const { return m_iWakePotential; }

		//	CEffectCreator virtuals
		virtual ~CParticleCloudEffectCreator (void) override;
		virtual int GetLifetime (void) override { return m_Lifetime.Roll(); }
		virtual void SetLifetime (int iLifetime) override { m_Lifetime.SetConstant(iLifetime); }

	protected:
		virtual IEffectPainter *OnCreatePainter (CCreatePainterCtx &Ctx) override;
		virtual ALERROR OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) override;
		virtual ALERROR OnEffectBindDesign (SDesignLoadCtx &Ctx) override;

	private:
		Styles m_iStyle = styleCloud;					//	Effect style
		Metric m_rSlowMotionFactor = 0.0;				//	Slow motion
		DiceRange m_Lifetime;							//	Lifetime of effect
		DiceRange m_ParticleCount;						//	Initial number of particles
		DiceRange m_ParticleLifetime;					//	Lifetime of each particle

		int m_iEmitLifetime = 0;						//	% time that it emits particles
		int m_iEmitRotation = 0;						//	Rotation
		DiceRange m_NewParticles;						//	Number of new particles per tick
		DiceRange m_InitSpeed;							//	Initial speed of each particle
		DiceRange m_Spread;								//	Spread angle (for jet and exhaust)

		Metric m_rRingRadius = 0.0;						//	If non-zero, particles form a ring at this radius
		Metric m_rMaxRadius = 0.0;						//	If RingRadius is non-zero, this is the outer edge of ring
														//		Otherwise, it is the max radius of a sphere
		Metric m_rMinRadius = 0.0;						//	If RingRadius is non-zero, this is the inner edge of ring
														//		Otherwise, it is ignored

		int m_iCohesion = 0;							//	Strength of force keeping particles together (0-100)
		int m_iViscosity = 0;							//	Drag on particles while inside bounds (0-100)
		int m_iWakePotential = 0;						//	Influence of moving objects (0-100)

		Metric m_rDrag = 0.0;							//	Drag when source object is moving

		CEffectCreator *m_pParticleEffect = NULL;		//	Effect to use to paint particles
	};

class CParticlePatternEffectCreator : public CEffectCreator
	{
	public:
		CParticlePatternEffectCreator (void) { }
		virtual ~CParticlePatternEffectCreator (void);

		CEffectCreator *GetParticleEffect (void) const { return m_pParticleEffect; }

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

		CEffectCreator *m_pParticleEffect = NULL;	//	Effect to use to paint particles (may be NULL)

		IEffectPainter *m_pSingleton = NULL;
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
		Metric m_rParticleSpeed = 0.0;					//	Speed of particles
		int m_iParticleLifetime = 0;					//	Particle lifespan
		CObjectImageArray m_Image;						//	Images
	};

class CParticleSystemEffectCreator : public CEffectCreator
	{
	public:
		CParticleSystemEffectCreator (void) { }
		~CParticleSystemEffectCreator (void);
			
		CEffectCreator *GetParticleEffect (void) const { return m_pParticleEffect; }

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

		CEffectCreator *m_pParticleEffect = NULL;	//	Effect to use to paint particles

		IEffectPainter *m_pSingleton = NULL;
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
		CG32bitPixel m_rgbPrimaryColor = CG32bitPixel(255, 255, 255);
		CG32bitPixel m_rgbSecondaryColor = CG32bitPixel(128, 128, 128);
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
		CRayEffectCreator (void) { }
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

		IEffectPainter *m_pSingleton = NULL;
	};

class CShapeEffectCreator : public CEffectCreator
	{
	public:
		CShapeEffectCreator (void) { }
		static CString GetClassTag (void) { return CONSTLIT("Shape"); }
		virtual CString GetTag (void) override { return GetClassTag(); }

		void CreateShapeRegion (int iAngle, int iLength, int iWidth, CG16bitBinaryRegion *pRegion);
		CGDraw::EBlendModes GetBlendMode (void) const { return m_iBlendMode; }
		CG32bitPixel GetColor (void) const { return m_rgbColor; }
		int GetLength (void) const { return m_iLength; }
		int GetLengthInc (void) const { return m_iLengthInc; }
		DWORD GetOpacity (void) const { return m_byOpacity; }
		int GetWidth (void) const { return m_iWidth; }
		int GetWidthInc (void) const { return m_iWidthInc; }
		bool IsDirectional (void) const { return m_bDirectional; }

		//	CEffectCreator virtuals
		virtual ~CShapeEffectCreator (void);
		virtual int GetLifetime (void) override { return 0; }

	protected:
		virtual IEffectPainter *OnCreatePainter (CCreatePainterCtx &Ctx) override;
		virtual ALERROR OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) override;

	private:
		int m_iWidth = 100;								//	Scale factor: pixel width of 100 units
		int m_iLength = 100;							//	Scale factor: pixel length of 100 units

		bool m_bDirectional = false;					//	If TRUE, rotate shape based on Ctx.iRotation
		int m_iWidthInc = 0;							//	Increment width every tick
		int m_iLengthInc = 0;							//	Increment length every tick

		CG32bitPixel m_rgbColor = CG32bitPixel(255, 255, 255);								//	Shape color
		DWORD m_byOpacity = 0xff;						//	Shape opacity
		CGDraw::EBlendModes m_iBlendMode = CGDraw::blendNormal;

		int m_iPointCount = 0;
		SPoint *m_Points = NULL;
		bool m_bConvexPolygon = false;					//	TRUE if simple polygon

		SPoint *m_TransBuffer = NULL;					//	Buffer used for transforms
	};

class CShockwaveEffectCreator : public CEffectCreator
	{
	public:
		CShockwaveEffectCreator (void) { }
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

		IEffectPainter *m_pSingleton = NULL;
	};

class CSingleParticleEffectCreator : public CEffectCreator
	{
	public:
		CSingleParticleEffectCreator (void) { }
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

		IEffectPainter *m_pSingleton = NULL;
	};

class CSmokeTrailEffectCreator : public CEffectCreator
	{
	public:
		static CString GetClassTag (void) { return CONSTLIT("SmokeTrail"); }
		virtual CString GetTag (void) override { return GetClassTag(); }

		int GetEmitLifetime (void) const { return m_iEmitLifetime; }
		Metric GetEmitSpeed (void) const;
		int GetNewParticleCount (void) const { return m_NewParticles.Roll(); }
		int GetNewParticleMax (void) const { return m_NewParticles.GetMaxValue(); }
		CEffectCreator *GetParticleEffect (void) const { return m_pParticleEffect; }
		int GetParticleLifetime (void) const { return Max(1, m_ParticleLifetime.Roll()); }
		int GetParticleLifetimeMax (void) const { return Max(1, m_ParticleLifetime.GetMaxValue()); }
		int GetRotation (void) const { return m_iRotation; }
		int GetSpread (void) const { return m_Spread.Roll(); }

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

		int m_iLifetime = 0;							//	Lifetime of effect
		int m_iEmitLifetime = 0;						//	% time that it emits particles
		int m_iRotation = 0;							//	Direction of emission (defaults to 180)

		CEffectCreator *m_pParticleEffect = NULL;		//	Effect to use to paint particles
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

		Styles m_iStyle = stylePlain;
		DiceRange m_SpikeCount;
		DiceRange m_SpikeLength;
		CG32bitPixel m_rgbPrimaryColor = CG32bitPixel(255, 255, 255);
		CG32bitPixel m_rgbSecondaryColor = CG32bitPixel(128, 128, 128);
		int m_iLifetime = 0;
	};

class CTextEffectCreator : public CEffectCreator
	{
	public:
		CTextEffectCreator (void) { }

		static CString GetClassTag (void) { return CONSTLIT("Text"); }
		virtual CString GetTag (void) override { return GetClassTag(); }

		const CG16bitFont *GetFont (void) { return m_pFont; }
		DWORD GetFontFlags (void) { return m_dwAlignment; }
		DWORD GetOpacity (void) { return m_byOpacity; }
		CG32bitPixel GetPrimaryColor (void) { return m_rgbPrimaryColor; }

		//	CEffectCreator virtuals
		virtual int GetLifetime (void) override { return m_iLifetime; }

	protected:
		virtual IEffectPainter *OnCreatePainter (CCreatePainterCtx &Ctx) override;
		virtual ALERROR OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID) override;

	private:
		CString m_sDefaultText;

		const CG16bitFont *m_pFont = NULL;
		CG32bitPixel m_rgbPrimaryColor = CG32bitPixel(255, 255, 255);
		DWORD m_byOpacity = 0xff;
		DWORD m_dwAlignment = CG16bitFont::AlignCenter;

		int m_iLifetime = -1;
	};

//	Space Object Implementations -----------------------------------------------

class CSequencerEffect : public TSpaceObjectImpl<OBJID_CSEQUENCEREFFECT>
	{
	public:
		CSequencerEffect (CUniverse &Universe) : TSpaceObjectImpl(Universe)
			{ }

		static ALERROR Create (CSystem &System,
							   CEffectSequencerCreator *pType,
							   CSpaceObject *pAnchor,
							   const CVector &vPos,
							   const CVector &vVel,
							   CSequencerEffect **retpEffect);

	protected:
		//	CSpaceObject virtuals
		virtual bool CanHit (CSpaceObject *pObj) const override { return false; }
		virtual CString GetObjClassName (void) const override { return CONSTLIT("CSequencerEffect"); }
		virtual void ObjectDestroyedHook (const SDestroyCtx &Ctx) override;
		virtual void OnReadFromStream (SLoadCtx &Ctx) override;
		virtual void OnWriteToStream (IWriteStream *pStream) override;
		virtual void OnUpdate (SUpdateCtx &Ctx, Metric rSecondsPerTick) override;
		virtual void PaintLRSBackground (CG32bitImage &Dest, int x, int y, const ViewportTransform &Trans) override { }
		virtual void PaintLRSForeground (CG32bitImage &Dest, int x, int y, const ViewportTransform &Trans) override { }

	private:

		CEffectSequencerCreator *m_pType = NULL;
		CSpaceObject *m_pAnchor = NULL;
		CVector m_vAnchorOffset;
		int m_iStartTime = 0;
		int m_iTimeCursor = 0;

	friend CObjectClass<CSequencerEffect>;
	};

