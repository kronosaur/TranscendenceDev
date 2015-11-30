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
		virtual void GetBounds (int *retWidth, int *retHeight);
		virtual ALERROR InitFromXML (SDesignLoadCtx &Ctx, CShipClass *pClass, CXMLElement *pDesc);
		virtual void Invalidate (void) { m_bInvalid = true;  }
		virtual void Paint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx);

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
		virtual void GetBounds (int *retWidth, int *retHeight);
		virtual ALERROR InitFromXML (SDesignLoadCtx &Ctx, CShipClass *pClass, CXMLElement *pDesc);
		virtual void Invalidate (void) { m_bInvalid = true;  }
		virtual void Paint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx);

	private:
		void Realize (SHUDPaintCtx &Ctx);

		//	Definitions

		int m_iArmorRingRadius;
		int m_iArmorRingWidth;
		int m_iShieldRingWidth;

		//	Metrics

		int m_cxDisplay;					//	Total width of display
		int m_cyDisplay;					//	Total height of display
		int m_xCenter;						//	Center of ring
		int m_yCenter;						//	Center of ring
		
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
		virtual void GetBounds (int *retWidth, int *retHeight);
		virtual ALERROR InitFromXML (SDesignLoadCtx &Ctx, CShipClass *pClass, CXMLElement *pDesc);
		virtual void Paint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx);

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
