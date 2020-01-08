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

	SHUDPaintCtx (CUniverse &UniverseArg, const CVisualPalette &VIArg, CSpaceObject &SourceArg) :
			Universe(UniverseArg),
			VI(VIArg),
			Source(SourceArg)
		{ }

	CUniverse &Universe;
	const CVisualPalette &VI;
	CSpaceObject &Source;
	int iTick = 0;
	RECT rcRect = { 0 };

	EPaintModes iMode = paintNormal;					//	To coordinate painting to a buffer
	BYTE byOpacity = 255;								//	Opacity to paint with
	int iSegmentSelected = -1;							//	Armor segment selected
	IHUDPainter *pShieldsHUD = NULL;					//	Armor display also paints shields

	TArray<CShip::SAttachedSectionInfo> SectionInfo;	//	Used during Realize
	};

struct SHUDUpdateCtx
	{
	SHUDUpdateCtx (CUniverse &UniverseArg, CSpaceObject &SourceArg) :
			Universe(UniverseArg),
			Source(SourceArg)
		{ }

	CUniverse &Universe;
	CSpaceObject &Source;
	int iTick = 0;
	};

class IHUDPainter
	{
	public:
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
		DWORD GetLocation (void) const { return m_dwLoc; }
		void GetPos (int *retx, int *rety) const { *retx = m_xPos; *rety = m_yPos; }
		void GetRect (RECT *retrcRect) const;
		virtual ALERROR InitFromXML (SDesignLoadCtx &Ctx, CShipClass *pClass, CXMLElement *pDesc) = 0;
		virtual void Invalidate (void) { }
		void Paint (CG32bitImage &Dest, SHUDPaintCtx &Ctx) { OnPaint(Dest, m_xPos, m_yPos, Ctx); }
		void Paint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx) { OnPaint(Dest, x, y, Ctx); }
		void SetLocation (const RECT &rcRect, DWORD dwLocation);
		void Update (SHUDUpdateCtx &Ctx) { OnUpdate(Ctx); }

		static IHUDPainter *Create (SDesignLoadCtx &Ctx, CShipClass *pClass, EHUDTypes iType);

	protected:

		virtual void OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx) = 0;
		virtual void OnUpdate (SHUDUpdateCtx &Ctx) { }

		//	Helpers

		ALERROR ComposeLoadError (SDesignLoadCtx &Ctx, const CString &sError) const { Ctx.sError = sError; return ERR_FAIL; }
		void DrawModifier (CG32bitImage &Dest, int x, int y, const CString &sText, DWORD dwLocation);
		ALERROR InitRectFromElement (CXMLElement *pItem, RECT *retRect);

	private:
		int m_xPos = 0;
		int m_yPos = 0;

		DWORD m_dwLoc = 0;
	};

