//	HUDImpl.h
//
//	Implements classes to paint various HUDs
//	Copyright (c) 2015 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

//	Default Set ----------------------------------------------------------------
//
//	This is the default set of HUDs, created at the dawn of the game. This set
//	mostly relies on images.

class CArmorHUDImages : public IHUDPainter
	{
	public:
		CArmorHUDImages (void);

		virtual ALERROR Bind (SDesignLoadCtx &Ctx);
		virtual void GetBounds (int *retWidth, int *retHeight) const;
		virtual ALERROR InitFromXML (SDesignLoadCtx &Ctx, CShipClass *pClass, CXMLElement *pDesc);
		virtual void Invalidate (void) { m_bInvalid = true;  }

	protected:
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx);

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

		int GetArmorSegment (SDesignLoadCtx &Ctx, CShipClass *pClass, CXMLElement *pDesc) const;
		void Realize (SHUDPaintCtx &Ctx);

		//	Definitions

		CObjectImageArray m_ShipImage;		//	Image for ship (with no armor)
		TSortMap<int, SArmorSegmentImageDesc> m_Segments;

		//	Runtime State

		bool m_bInvalid;
		CG32bitImage m_Buffer;
		TArray<STextPaint> m_Text;
	};

class CReactorHUDDefault : public IHUDPainter
	{
	public:
		CReactorHUDDefault (void);
		virtual ~CReactorHUDDefault (void);

		virtual ALERROR Bind (SDesignLoadCtx &Ctx);
		virtual void GetBounds (int *retWidth, int *retHeight) const;
		virtual ALERROR InitFromXML (SDesignLoadCtx &Ctx, CShipClass *pClass, CXMLElement *pDesc);
		virtual void Invalidate (void) { m_bInvalid = true;  }

	protected:
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx);
		virtual void OnUpdate (SHUDUpdateCtx &Ctx);

	private:
		void Realize (SHUDPaintCtx &Ctx);

		//	Definitions

		CObjectImageArray m_ReactorImage;

		CObjectImageArray m_PowerLevelImage;
		int m_xPowerLevelImage;
		int m_yPowerLevelImage;

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

		virtual ALERROR Bind (SDesignLoadCtx &Ctx);
		virtual void GetBounds (int *retWidth, int *retHeight) const;
		virtual ALERROR InitFromXML (SDesignLoadCtx &Ctx, CShipClass *pClass, CXMLElement *pDesc);

	protected:
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx);

	private:
		struct STextPaint
			{
			CString sText;
			int x;
			int y;
			const CG16bitFont *pFont;
			CG32bitPixel rgbColor;
			};

		//	Definitions

		CEffectCreatorRef m_pShieldEffect;	//	Effect for display shields HUD
		CObjectImageArray m_Image;			//	Image for shields

		//	Runtime State

		DWORD m_dwCachedShipID;				//	Cached painters for this ship ID.
		IEffectPainter *m_pShieldPainter;	//	Caches shield painter
		TArray<STextPaint> m_Text;
	};

class CWeaponHUDDefault : public IHUDPainter
	{
	public:
		CWeaponHUDDefault (void);
		virtual ~CWeaponHUDDefault (void);

		virtual ALERROR Bind (SDesignLoadCtx &Ctx);
		virtual void GetBounds (int *retWidth, int *retHeight) const;
		virtual ALERROR InitFromXML (SDesignLoadCtx &Ctx, CShipClass *pClass, CXMLElement *pDesc);
		virtual void Invalidate (void) { m_bInvalid = true;  }

	protected:
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx);

	private:
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

		virtual ALERROR Bind (SDesignLoadCtx &Ctx);
		virtual void GetBounds (int *retWidth, int *retHeight) const;
		virtual ALERROR InitFromXML (SDesignLoadCtx &Ctx, CShipClass *pClass, CXMLElement *pDesc);
		virtual void Invalidate (void) { m_bInvalid = true;  }

	protected:
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx);

	private:
		void DrawArmorName (CG32bitImage &Dest, int iAngle, int iRadius, CShip *pShip, CInstalledArmor *pArmor, CG32bitPixel rgbBack, CG32bitPixel rgbColor);
		void DrawIntegrityBox (CG32bitImage &Dest, int iAngle, int iRadius, const CString &sText, CG32bitPixel rgbBack, CG32bitPixel rgbColor);
		void DrawItemBox (CG32bitImage &Dest, int iAngle, int iRadius, const CString &sName, const CString &sMods, CG32bitPixel rgbBack, CG32bitPixel rgbColor);
		void DrawShieldsName (CG32bitImage &Dest, int iAngle, int iRadius, CShip *pShip, CInstalledDevice *pShields, CG32bitPixel rgbBack, CG32bitPixel rgbColor);
		void Realize (SHUDPaintCtx &Ctx);

		//	Definitions

		CG32bitPixel m_rgbArmor;			//	Color of armor segment
		CG32bitPixel m_rgbArmorText;		//	Color of text
		CG32bitPixel m_rgbArmorTextBack;	//	Armor text background color
		CG32bitPixel m_rgbShields;			//	Color of shields
		CG32bitPixel m_rgbShieldsText;		//	Shield text color
		CG32bitPixel m_rgbShieldsTextBack;	//	Shield text background color

		int m_iArmorRingRadius;
		int m_iArmorRingWidth;
		int m_iShieldRingWidth;

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

class CReactorHUDCircular : public IHUDPainter
	{
	public:
		CReactorHUDCircular (void);
		virtual ~CReactorHUDCircular (void);

		virtual ALERROR Bind (SDesignLoadCtx &Ctx);
		virtual void GetBounds (int *retWidth, int *retHeight) const;
		virtual ALERROR InitFromXML (SDesignLoadCtx &Ctx, CShipClass *pClass, CXMLElement *pDesc);
		virtual void Invalidate (void) { m_bInvalid = true;  }

	protected:
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx);
		virtual void OnUpdate (SHUDUpdateCtx &Ctx);

	private:
		void PaintChargesGauge (CShip *pShip);
		void PaintFuelGauge (CShip *pShip);
		void PaintPowerGauge (CShip *pShip);
		void PaintReactorItem (CShip *pShip);
		void Realize (SHUDPaintCtx &Ctx);

		//	Definitions

		int m_iGaugeRadius;
		int m_iGaugeWidth;

		CG32bitPixel m_rgbGaugeBack;		//	Background color of gauges
		CG32bitPixel m_rgbFuelGauge;		//	Color of fuel bar
		CG32bitPixel m_rgbPowerGauge;		//	Color of power gauge
		CG32bitPixel m_rgbGaugeWarning;		//	Color when in warning mode

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

		virtual ALERROR Bind (SDesignLoadCtx &Ctx);
		virtual void GetBounds (int *retWidth, int *retHeight) const;
		virtual ALERROR InitFromXML (SDesignLoadCtx &Ctx, CShipClass *pClass, CXMLElement *pDesc);
		virtual void Invalidate (void) { m_bInvalid = true;  }

	protected:
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx);

	private:
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
