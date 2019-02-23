//	SystemPaintImpl.h
//
//	Helpers for painting a system
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class IPaintList
	{
	public:
		virtual void Insert (CSpaceObject &Obj) = 0;
		virtual void Paint (CG32bitImage &Dest, SViewportPaintCtx &Ctx) const = 0;
	};

class CDepthPaintList : public IPaintList
	{
	public:
		CDepthPaintList (Metric rTopEdge) :
				m_rTopEdge(rTopEdge)
			{ }

		virtual void Insert (CSpaceObject &Obj) override { m_List.Insert(GetPaintOrderKey(Obj), &Obj); Obj.SetPaintNeeded(); }
		virtual void Paint (CG32bitImage &Dest, SViewportPaintCtx &Ctx) const override;

	private:
		inline DWORD GetPaintOrderKey (CSpaceObject &Obj) const { return (DWORD)(int)((m_rTopEdge - Obj.GetPos().Y()) / g_KlicksPerPixel); }

		TSortMap<DWORD, CSpaceObject *> m_List;
		Metric m_rTopEdge = 0.0;
	};

class CMarkerPaintList : public IPaintList
	{
	public:
		static constexpr int ENHANCED_SRS_BLOCK_SIZE = 6;

		CMarkerPaintList (const RECT &rcBounds) :
				m_rcBounds(rcBounds)
			{ }

		virtual void Insert (CSpaceObject &Obj) override { m_List.Insert(&Obj); }
		virtual void Paint (CG32bitImage &Dest, SViewportPaintCtx &Ctx) const override;

	private:
		void PaintDestinationMarker (SViewportPaintCtx &Ctx, CG32bitImage &Dest, int x, int y, CSpaceObject *pObj) const;

		TArray<CSpaceObject *> m_List;
		RECT m_rcBounds;
	};

class CParallaxPaintList : public IPaintList
	{
	public:
		virtual void Insert (CSpaceObject &Obj) override { m_List.Insert(&Obj); Obj.SetPainted(); }
		virtual void Paint (CG32bitImage &Dest, SViewportPaintCtx &Ctx) const override;

	private:
		TArray<CSpaceObject *> m_List;
	};

class CUnorderedPaintList : public IPaintList
	{
	public:
		virtual void Insert (CSpaceObject &Obj) override { m_List.Insert(&Obj); Obj.SetPaintNeeded(); }
		virtual void Paint (CG32bitImage &Dest, SViewportPaintCtx &Ctx) const override;

	private:
		TArray<CSpaceObject *> m_List;
	};
