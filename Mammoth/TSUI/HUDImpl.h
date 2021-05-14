//	HUDImpl.h
//
//	Implements classes to paint various HUDs
//	Copyright (c) 2015 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

//	Default Set ----------------------------------------------------------------
//
//	This is the default set of HUDs, created at the dawn of the game. This set
//	mostly relies on images.

class CAccelerateHUD : public IHUDPainter
	{
	public:
		virtual void GetBounds (int *retWidth, int *retHeight) const override;
		virtual void Invalidate (void) override { }

	protected:
		virtual bool OnCreate (SHUDCreateCtx &CreateCtx, CString *retsError = NULL) override { return true; }
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx) override;
		virtual void OnUpdate (SHUDUpdateCtx &Ctx) override;

	private:
		static constexpr int DISPLAY_HEIGHT = 160;
		static constexpr int DISPLAY_WIDTH = 160;
		static constexpr int FADE_START = 150;
		static constexpr int FADE_TIME = 30;
		static constexpr int ICON_HEIGHT = 128;
		static constexpr int ICON_WIDTH = 128;

		bool IsNewMode (CUniverse::EUpdateSpeeds iCurMode, CUniverse::EUpdateSpeeds iNewMode) const;

		CUniverse::EUpdateSpeeds m_iLastMode = CUniverse::updateNone;
		int m_iLastModeTick = 0;
	};

class CArmorHUDImages : public IHUDPainter
	{
	public:
		CArmorHUDImages (void);

		virtual void GetBounds (int *retWidth, int *retHeight) const override;
		virtual void Invalidate (void) override { m_bInvalid = true;  }

	protected:
		virtual bool OnCreate (SHUDCreateCtx &CreateCtx, CString *retsError = NULL) override;
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx) override;

	private:
		struct SArmorSegmentImageDesc
			{
			CString sName;					//	Name of segment

			int xDest;						//	Coordinates of destination
			int yDest;
			int xHP;						//	Coordinates of HP display
			int yHP;
			int yName;						//	Coordinates of armor name
			int cxNameBreak;				//	Line break
			int xNameDestOffset;
			int yNameDestOffset;

			CObjectImageArray Image;		//	Image for armor damage
			};

		struct STextPaint
			{
			CString sText;
			int x;
			int y;
			const CG16bitFont *pFont;
			CG32bitPixel rgbColor;
			};

		ALERROR Bind (SDesignLoadCtx &Ctx);
		int GetArmorSegment (SDesignLoadCtx &Ctx, const CShipClass &Class, CXMLElement *pDesc) const;
		void Realize (SHUDPaintCtx &Ctx);

		//	Definitions

		CObjectImageArray m_ShipImage;		//	Image for ship (with no armor)
		TSortMap<int, SArmorSegmentImageDesc> m_Segments;

		//	Runtime State

		bool m_bInvalid;
		CG32bitImage m_Buffer;
		TArray<STextPaint> m_Text;
	};

class CArmorHUDShaped : public IHUDPainter
	{
	public:
		CArmorHUDShaped (void);

		virtual void GetBounds (int *retWidth, int *retHeight) const override;
		virtual void Invalidate (void) override { m_bInvalid = true;  }

	protected:
		virtual bool OnCreate (SHUDCreateCtx &CreateCtx, CString *retsError = NULL) override;
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx) override;

	private:
		ALERROR Bind (SDesignLoadCtx &Ctx);
		void Realize (SHUDPaintCtx &Ctx);

		//	Definitions

		CG32bitPixel m_rgbArmor;			//	Color of armor segment
		CG32bitPixel m_rgbArmorText;		//	Color of text
		CG32bitPixel m_rgbArmorTextBack;	//	Armor text background color
		CG32bitPixel m_rgbShields;			//	Color of shields
		CG32bitPixel m_rgbShieldsText;		//	Shield text color
		CG32bitPixel m_rgbShieldsTextBack;	//	Shield text background color

		//	Metrics

		int m_cxDisplay;					//	Total width of display
		int m_cyDisplay;					//	Total height of display
		int m_xCenter;						//	Center of ring
		int m_yCenter;						//	Center of ring
		int m_cxMaxValue;					//	Size of text showing max integrity value (100%)
		int m_cyMaxValue;
		
		//	Runtime State

		bool m_bInvalid;
		CG32bitImage m_Buffer;
	};

class CLRSHUD : public IHUDPainter
	{
	public:
		virtual void GetBounds (int *retWidth, int *retHeight) const override;
		virtual void Invalidate (void) override { m_bInvalid = true; }

	protected:
		virtual bool OnCreate (SHUDCreateCtx &CreateCtx, CString *retsError = NULL) override;
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx) override;
		virtual void OnUpdate (SHUDUpdateCtx &Ctx) override;

	private:
		static constexpr int MIN_LRS_SIZE =	200;
		static constexpr int LRS_SCALE =	27;
		static constexpr int LRS_UPDATE =	5;
		static constexpr CG32bitPixel RGB_DEFAULT_BACKGROUND =	CG32bitPixel(17, 21, 26);

		void Realize (SHUDUpdateCtx &Ctx);

		int m_iDiameter = 0;				//	Diameter of HUD (pixels)
		CG8bitImage m_Mask;
		CG32bitPixel m_rgbBackground = RGB_DEFAULT_BACKGROUND;
		const CG32bitImage *m_pBackground = NULL;
		const CG32bitImage *m_pSnow = NULL;

		bool m_bInvalid = true;
		CG32bitImage m_Buffer;
	};

class CNullHUD : public IHUDPainter
	{
	public:
		virtual void GetBounds (int *retWidth, int *retHeight) const override { *retWidth = 0; *retHeight = 0; }

	protected:
		virtual bool OnCreate (SHUDCreateCtx &CreateCtx, CString *retsError = NULL) override { return true; }
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx) override { }

	private:
	};

class CReactorHUDDefault : public IHUDPainter
	{
	public:
		CReactorHUDDefault (void);
		virtual ~CReactorHUDDefault (void);

		virtual void GetBounds (int *retWidth, int *retHeight) const override;
		virtual void Invalidate (void) override { m_bInvalid = true;  }

	protected:
		virtual bool OnCreate (SHUDCreateCtx &CreateCtx, CString *retsError = NULL) override;
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx) override;
		virtual void OnUpdate (SHUDUpdateCtx &Ctx) override;

	private:
		ALERROR Bind (SDesignLoadCtx &Ctx);
		void Realize (SHUDPaintCtx &Ctx);

		//	Definitions

		CObjectImageArray m_ReactorImage;

		CObjectImageArray m_PowerLevelImage;
		int m_xPowerLevelImage;
		int m_yPowerLevelImage;

		CObjectImageArray m_PowerGenImage;
		int m_xPowerGenImage;
		int m_yPowerGenImage;

		CObjectImageArray m_FuelLevelImage;
		int m_xFuelLevelImage;
		int m_yFuelLevelImage;

		CObjectImageArray m_FuelLowLevelImage;

		RECT m_rcReactorText;
		RECT m_rcPowerLevelText;
		RECT m_rcFuelLevelText;

		//	Runtime State

		bool m_bInvalid;
		CG32bitImage m_Buffer;
		int m_iTick;
	};

class CShieldHUDDefault : public IHUDPainter
	{
	public:
		CShieldHUDDefault (void);
		virtual ~CShieldHUDDefault (void);

		virtual void GetBounds (int *retWidth, int *retHeight) const override;

	protected:
		virtual bool OnCreate (SHUDCreateCtx &CreateCtx, CString *retsError = NULL) override;
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx) override;

	private:
		struct STextPaint
			{
			CString sText;
			int x;
			int y;
			const CG16bitFont *pFont;
			CG32bitPixel rgbColor;
			};

		ALERROR Bind (SDesignLoadCtx &Ctx);

		//	Definitions

		CEffectCreatorRef m_pShieldEffect;	//	Effect for display shields HUD
		CObjectImageArray m_Image;			//	Image for shields

		//	Runtime State

		DWORD m_dwCachedShipID;				//	Cached painters for this ship ID.
		IEffectPainter *m_pShieldPainter;	//	Caches shield painter
		TArray<STextPaint> m_Text;
	};

class CTimersHUD : public IHUDPainter
	{
	public:
		virtual void GetBounds (int *retWidth, int *retHeight) const override;
		virtual void Invalidate (void) override { m_bInvalid = true; }

	protected:
		virtual bool OnCreate (SHUDCreateCtx &CreateCtx, CString *retsError = NULL) override { return true; }
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx) override;
		virtual void OnUpdate (SHUDUpdateCtx &Ctx) override;

	private:
		static constexpr int ICON_HEIGHT = 32;
		static constexpr int ICON_WIDTH = 32;
		static constexpr int SPACING_X = 8;
		static constexpr int SPACING_Y = 8;

		static constexpr int TIMER_HEIGHT = ICON_HEIGHT;
		static constexpr int TIMER_WIDTH = 300;

		static constexpr int MAX_TIMERS = 8;

		static constexpr int DISPLAY_HEIGHT = (TIMER_HEIGHT + SPACING_Y) * MAX_TIMERS;
		static constexpr int DISPLAY_WIDTH = TIMER_WIDTH;

		static void PaintTimer (SHUDPaintCtx &Ctx, CG32bitImage &Dest, SHUDTimerDesc &Timer, int x, int y);
		void Realize (SHUDPaintCtx &Ctx);

		bool m_bInvalid = true;				//	Need to refresh from player ship
		CG32bitImage m_Buffer;

		int m_iTimerCount = 0;				//	Number of timers realized
	};

class CWeaponHUDDefault : public IHUDPainter
	{
	public:
		CWeaponHUDDefault (void);
		virtual ~CWeaponHUDDefault (void);

		virtual void GetBounds (int *retWidth, int *retHeight) const override;
		virtual void Invalidate (void) override { m_bInvalid = true;  }

	protected:
		virtual bool OnCreate (SHUDCreateCtx &CreateCtx, CString *retsError = NULL) override;
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx) override;

	private:
		ALERROR Bind (SDesignLoadCtx &Ctx);
		void PaintDeviceStatus (CShip *pShip, DeviceNames iDev, int x, int y);
		void PaintStat (int x, int y, const CString &sHeader, const CString &sStat, CG32bitPixel rgbColor);
		void Realize (SHUDPaintCtx &Ctx);

		//	Definitions

		CObjectImageArray m_BackImage;		//	Background image

		//	Runtime State

		bool m_bInvalid;
		CG32bitImage m_Buffer;
		CG32bitImage m_Target;				//	Temp image for target.
		CG8bitImage m_TargetMask;			//	Mask to contain target image

		CG32bitImage *m_pDefaultBack;
	};

//	Circular Set ---------------------------------------------------------------
//
//	This is a procedurally drawn set in which each HUD is circular, with various
//	labels and displays projecting outward.

class CArmorHUDRingSegments : public IHUDPainter
	{
	public:
		CArmorHUDRingSegments (void);

		virtual void GetBounds (int *retWidth, int *retHeight) const override;
		virtual void Invalidate (void) override { m_bInvalid = true;  }

	protected:
		virtual bool OnCreate (SHUDCreateCtx &CreateCtx, CString *retsError = NULL) override;
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx) override;

	private:
		ALERROR Bind (SDesignLoadCtx &Ctx);
		void DrawArmorName (CG32bitImage &Dest, int iAngle, int iRadius, CShip *pShip, CArmorItem ArmorItem, CG32bitPixel rgbBack, CG32bitPixel rgbColor);
		void DrawIntegrityBox (CG32bitImage &Dest, int iAngle, int iRadius, const CString &sText, CG32bitPixel rgbBack, CG32bitPixel rgbColor);
		void DrawItemBox (CG32bitImage &Dest, int iAngle, int iRadius, const CString &sName, const TArray<SDisplayAttribute> &Attribs, CG32bitPixel rgbBack, CG32bitPixel rgbColor);
		void DrawShieldsName (CG32bitImage &Dest, int iAngle, int iRadius, CShip *pShip, CDeviceItem ShieldItem, CG32bitPixel rgbBack, CG32bitPixel rgbColor);
		void Realize (SHUDPaintCtx &Ctx);

		//	Definitions

		CG32bitPixel m_rgbArmor = CG32bitPixel(0xff, 0xff, 0xff);			//	Color of armor segment
		CG32bitPixel m_rgbArmorText = CG32bitPixel(0x00, 0x00, 0x00);		//	Color of text
		CG32bitPixel m_rgbArmorTextBack = CG32bitPixel(0xff, 0xff, 0xff);	//	Armor text background color
		CG32bitPixel m_rgbShields = CG32bitPixel(0xff, 0xff, 0xff);			//	Color of shields
		CG32bitPixel m_rgbShieldsText = CG32bitPixel(0x00, 0x00, 0x00);		//	Shield text color
		CG32bitPixel m_rgbShieldsTextBack = CG32bitPixel(0xff, 0xff, 0xff);	//	Shield text background color

		int m_iArmorRingRadius = 100;
		int m_iArmorRingWidth = 10;
		int m_iShieldRingWidth = 10;
		CLanguage::SHPDisplayOptions m_HPDisplay;

		//	Metrics

		int m_cxDisplay = 0;					//	Total width of display
		int m_cyDisplay = 0;					//	Total height of display
		int m_xCenter = 0;						//	Center of ring
		int m_yCenter = 0;						//	Center of ring
		int m_cxMaxValue = 0;					//	Size of text showing max integrity value (100%)
		int m_cyMaxValue = 0;
		
		//	Runtime State

		bool m_bInvalid = true;
		CG32bitImage m_Buffer;
	};

class CReactorHUDCircular : public IHUDPainter
	{
	public:
		CReactorHUDCircular (void);
		virtual ~CReactorHUDCircular (void);

		virtual void GetBounds (int *retWidth, int *retHeight) const override;
		virtual void Invalidate (void) override { m_bInvalid = true;  }

	protected:
		virtual bool OnCreate (SHUDCreateCtx &CreateCtx, CString *retsError = NULL) override;
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx) override;
		virtual void OnUpdate (SHUDUpdateCtx &Ctx) override;

	private:
		ALERROR Bind (SDesignLoadCtx &Ctx);
		void PaintChargesGauge (const SReactorStats &Stats);
		void PaintCounterGauge(CShip * pShip);
		void PaintFuelGauge(const SReactorStats & Stats, bool bCounterGaugePresent);
		void PaintPowerGauge (const SReactorStats &Stats);
		void PaintReactorItem (const SReactorStats &Stats);
		void Realize (SHUDPaintCtx &Ctx);

		//	Definitions

		int m_iGaugeRadius;
		int m_iGaugeWidth;

		CG32bitPixel m_rgbGaugeBack;		//	Background color of gauges
		CG32bitPixel m_rgbFuelGauge;		//	Color of fuel bar
		CG32bitPixel m_rgbPowerGauge;		//	Color of power gauge
		CG32bitPixel m_rgbGaugeWarning;		//	Color when in warning mode
		CG32bitPixel m_rgbOtherPower;		//	Color of other (usually solar) power

		//	Metrics

		int m_cxDisplay;
		int m_cyDisplay;
		int m_xCenter;
		int m_yCenter;

		//	Runtime State

		bool m_bInvalid;
		CG32bitImage m_Buffer;
		int m_iTick;
	};

class CWeaponHUDCircular : public IHUDPainter
	{
	public:
		CWeaponHUDCircular (void);
		virtual ~CWeaponHUDCircular (void);

		virtual void GetBounds (int *retWidth, int *retHeight) const override;
		virtual void Invalidate (void) override { m_bInvalid = true;  }

	protected:
		virtual bool OnCreate (SHUDCreateCtx &CreateCtx, CString *retsError = NULL) override;
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx) override;

	private:
		ALERROR Bind (SDesignLoadCtx &Ctx);
		void PaintTarget (SHUDPaintCtx &Ctx, CShip *pShip, CSpaceObject *pTarget);
		void PaintTargetStat (CG32bitImage &Dest, int x, int y, const CString &sLabel, const CString &sStat);
		void PaintWeaponStatus (CShip *pShip, CInstalledDevice *pDevice, Metric rAngle);
		void Realize (SHUDPaintCtx &Ctx);

		//	Definitions

		int m_iTargetRadius;				//	Radius of target screen

		CG32bitPixel m_rgbTargetBack;		//	Color of background target area
		CG32bitPixel m_rgbTargetText;		//	Color of target text
		CG32bitPixel m_rgbWeaponBack;		//	Color of weapon area background
		CG32bitPixel m_rgbWeaponText;		//	Color of weapon text

		//	Metrics

		int m_cxDisplay;					//	Size of display (calculated in InitFromXML)
		int m_cyDisplay;
		int m_xCenter;						//	Center of circle
		int m_yCenter;

		//	Runtime State

		bool m_bInvalid;
		CG32bitImage m_Buffer;
		CG8bitImage m_TargetMask;			//	Mask for the target
		CGRegion m_InfoArea;				//	Info area semi-circle region
	};

//	Rectangular Set -------------------------------------------------------------
//
//	Procedurally generated HUDs using rounded-rectangles and a horizontal
//	orientation.

class CArmorHUDRectangular : public IHUDPainter
	{
	public:
		CArmorHUDRectangular (void);

		virtual void GetBounds (int *retWidth, int *retHeight) const override;
		virtual void Invalidate (void) override { m_bInvalid = true;  }

	protected:
		virtual bool OnCreate (SHUDCreateCtx &CreateCtx, CString *retsError = NULL) override;
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx) override;

	private:
		struct SLabelEntry
			{
			int xPos;						//	Position that we're marking
			int yPos;						//	(this is where the actual section/armor is)
			RECT rcLabel;					//	Rect for the label
			int xHandle;					//	Point on the label to connect
			int yHandle;
			};

		void InitLabelDistribution (void);
		void InitLabelDistributionForRow (const TSortMap<int, int> &Row);
		void InitShipSectionLabels (const TArray<CShip::SAttachedSectionInfo> &SectionInfo);
		void PaintArmorSegments (SHUDPaintCtx &Ctx, CShip *pShip);
		void PaintBar (int x, int y, int cxWidth, int cyHeight, int iLevel, CG32bitPixel rgbColor);
		void PaintHealerLevel (SHUDPaintCtx &Ctx, CShip *pShip);
		void PaintShipSections (SHUDPaintCtx &Ctx, CShip *pShip);
		void Realize (SHUDPaintCtx &Ctx);

		//	Definitions

		CG32bitPixel m_rgbArmor;			//	Color of armor segment
		CG32bitPixel m_rgbArmorText;		//	Color of text
		CG32bitPixel m_rgbArmorTextBack;	//	Armor text background color
		CG32bitPixel m_rgbShields;			//	Color of shields
		CG32bitPixel m_rgbShieldsText;		//	Shield text color
		CG32bitPixel m_rgbShieldsTextBack;	//	Shield text background color

		//	Metrics

		int m_cxDisplay;					//	Total width of display
		int m_cyDisplay;					//	Total height of display
		int m_cyRow;
		int m_cxShipSize;
		int m_cxMaxValue;
		int m_cyMaxValue;
		TArray<SLabelEntry> m_Labels;
		
		//	Runtime State

		bool m_bInvalid;
		CG32bitImage m_Buffer;
	};

