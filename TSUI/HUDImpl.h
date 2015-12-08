//	HUDImpl.h
//
//	Implements classes to paint various HUDs
//	Copyright (c) 2015 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

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

	protected:
		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx);

	private:
		void PaintDeviceStatus (CShip *pShip, DeviceNames iDev, int x, int y);
		void Realize (SHUDPaintCtx &Ctx);

		//	Definitions

		CObjectImageArray m_BackImage;		//	Background image

		//	Runtime State

		bool m_bInvalid;
		CG32bitImage m_Buffer;
		CG32bitImage *m_pDefaultBack;
	};
