//	TSUIHUD.h
//
//	Defines classes and interfaces for player ship display, etc.
//	Copyright (c) 2015 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class IHUDPainter;

struct SHUDPaintCtx
	{
	enum EPaintModes
		{
		paintNormal,
		paintToBuffer,
		paintOverBuffer,
		};

	SHUDPaintCtx (void) :
			pSource(NULL),
			iMode(paintNormal),
			iSegmentSelected(-1),
			pShieldsHUD(NULL)
		{ }

	CSpaceObject *pSource;
	RECT rcRect;

	EPaintModes iMode;						//	To coordinate painting to a buffer
	int iSegmentSelected;					//	Armor segment selected
	IHUDPainter *pShieldsHUD;				//	Armor display also paints shields
	};

class IHUDPainter
	{
	public:
		enum ETypes
			{
			hudArmor,
			hudShields,
			hudReactor,
			hudTargeting,
			};

		enum ELocations
			{
			locNone =					0x00000000,

			locAlignLeft =				0x00000001,
			locAlignRight =				0x00000002,
			locAlignCenter =			0x00000004,

			locAlignTop =				0x00000010,
			locAlignMiddle =			0x00000020,
			locAlignBottom =			0x00000040,
			};

		virtual ~IHUDPainter (void) { }

		virtual ALERROR Bind (SDesignLoadCtx &Ctx) = 0;
		virtual void GetBounds (int *retWidth, int *retHeight) const = 0;
		inline void GetPos (int *retx, int *rety) const { *retx = m_xPos; *rety = m_yPos; }
		void GetRect (RECT *retrcRect) const;
		virtual ALERROR InitFromXML (SDesignLoadCtx &Ctx, CShipClass *pClass, CXMLElement *pDesc) = 0;
		virtual void Invalidate (void) { }
		inline void Paint (CG32bitImage &Dest, SHUDPaintCtx &Ctx) { OnPaint(Dest, m_xPos, m_yPos, Ctx); }
		inline void Paint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx) { OnPaint(Dest, x, y, Ctx); }
		void SetLocation (const RECT &rcRect, DWORD dwLocation);

		static IHUDPainter *Create (SDesignLoadCtx &Ctx, CShipClass *pClass, ETypes iType);

	protected:

		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx) = 0;

		//	Helpers

		ALERROR ComposeLoadError (SDesignLoadCtx &Ctx, const CString &sError) const { Ctx.sError = sError; return ERR_FAIL; }
		void DrawModifier (CG32bitImage &Dest, int x, int y, const CString &sText, DWORD dwLocation);

	private:
		int m_xPos;
		int m_yPos;
	};

