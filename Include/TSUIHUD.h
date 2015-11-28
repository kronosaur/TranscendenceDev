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

		virtual ~IHUDPainter (void) { }

		virtual ALERROR Bind (SDesignLoadCtx &Ctx) = 0;
		virtual void GetBounds (int *retWidth, int *retHeight) = 0;
		virtual ALERROR InitFromXML (SDesignLoadCtx &Ctx, CShipClass *pClass, CXMLElement *pDesc) = 0;
		virtual void Invalidate (void) { }
		virtual void Paint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx) = 0;

		static IHUDPainter *Create (SDesignLoadCtx &Ctx, CShipClass *pClass, ETypes iType);

	protected:

		//	Helpers

		ALERROR ComposeLoadError (SDesignLoadCtx &Ctx, const CString &sError) const { Ctx.sError = sError; return ERR_FAIL; }
	};

