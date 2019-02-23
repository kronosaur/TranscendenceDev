//	IPaintList.cpp
//
//	IPaintList class and implementations
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "SystemPaintImpl.h"

void CDepthPaintList::Paint (CG32bitImage &Dest, SViewportPaintCtx &Ctx) const

//	Paint
//
//	Paint all objects in our list.

	{
	for (int i = 0; i < m_List.GetCount(); i++)
		{
		CSpaceObject *pObj = m_List[i];

		if (pObj->IsPaintNeeded())
			{
			//	Figure out the position of the object in pixels

			int x, y;
			Ctx.XForm.Transform(pObj->GetPos(), &x, &y);

			//	Paint the object in the viewport

			SetProgramState(psPaintingSRS, pObj);

			Ctx.pObj = pObj;
			pObj->Paint(Dest, 
					x,
					y,
					Ctx);

			SetProgramState(psPaintingSRS);
			}

		//	Clear destination, if necessary

		if (pObj->IsAutoClearDestination())
			pObj->ClearPlayerDestination();
		}
	}

void CMarkerPaintList::Paint (CG32bitImage &Dest, SViewportPaintCtx &Ctx) const

//	Paint
//
//	Paint all objects in our list.

	{
	for (int i = 0; i < m_List.GetCount(); i++)
		{
		CSpaceObject *pObj = m_List[i];

		//	If this is a destination marker then we paint it at the circumference
		//	of a circle around the center.

		if (pObj->IsPlayerTarget()
				|| pObj->IsPlayerDestination()
				|| pObj->IsHighlighted())
			{
			CVector vDir = (pObj->GetPos() - Ctx.vCenterPos).Normal();

			PaintDestinationMarker(Ctx,
					Dest, 
					Ctx.xCenter + (int)(Ctx.rIndicatorRadius * vDir.GetX()), 
					Ctx.yCenter - (int)(Ctx.rIndicatorRadius * vDir.GetY()),
					pObj);
			}

		//	Otherwise we paint this as a block at the edge of the screen

		else
			{
			//	Figure out the position of the object in pixels
			//	relative to the center of the screen

			int x, y;
			Ctx.XForm.Transform(pObj->GetPos(), &x, &y);
			x = x - Ctx.xCenter;
			y = y - Ctx.yCenter;

			//	Now clip the position to the side of the screen

			if (x >= m_rcBounds.right)
				{
				y = y * (m_rcBounds.right - 1) / x;
				x = m_rcBounds.right - 1;
				}
			else if (x < m_rcBounds.left)
				{
				y = y * (m_rcBounds.left) / x;
				x = m_rcBounds.left;
				}

			if (y >= m_rcBounds.bottom)
				{
				x = x * (m_rcBounds.bottom - 1) / y;
				y = m_rcBounds.bottom - 1;
				}
			else if (y < m_rcBounds.top)
				{
				x = x * m_rcBounds.top / y;
				y = m_rcBounds.top;
				}

			//	Draw the indicator

			CG32bitPixel rgbColor = pObj->GetSymbolColor();

			Dest.Fill(Ctx.xCenter + x - (ENHANCED_SRS_BLOCK_SIZE / 2), 
					Ctx.yCenter + y - (ENHANCED_SRS_BLOCK_SIZE / 2),
					ENHANCED_SRS_BLOCK_SIZE, 
					ENHANCED_SRS_BLOCK_SIZE, 
					rgbColor);
			}
		}

	}

void CMarkerPaintList::PaintDestinationMarker (SViewportPaintCtx &Ctx, CG32bitImage &Dest, int x, int y, CSpaceObject *pObj) const

//	PaintDestinationMarker
//
//	Paints a directional indicator

	{
	DEBUG_TRY

	CVector vPos;

	//	Figure out the bearing for the destination object.

	int iBearing = VectorToPolar(pObj->GetPos() - (Ctx.pCenter ? Ctx.pCenter->GetPos() : NullVector));
	CG32bitPixel rgbColor = pObj->GetSymbolColor();

	//	Paint the arrow

	CPaintHelper::PaintArrow(Dest, x, y, iBearing, rgbColor);

	//	Paint the text

	vPos = PolarToVector(iBearing, 5 * ENHANCED_SRS_BLOCK_SIZE);
	int xText = x - (int)vPos.GetX();
	int yText = y + (int)vPos.GetY();

	DWORD iAlign = alignCenter;
	if (iBearing <= 180)
		yText += 2 * ENHANCED_SRS_BLOCK_SIZE;
	else
		{
		yText -= (2 * ENHANCED_SRS_BLOCK_SIZE);
		iAlign |= alignBottom;
		}

	pObj->PaintHighlightText(Dest, xText, yText, Ctx, (AlignmentStyles)iAlign, rgbColor);

	DEBUG_CATCH
	}

void CParallaxPaintList::Paint (CG32bitImage &Dest, SViewportPaintCtx &Ctx) const

//	Paint
//
//	Paint all objects in our list.

	{
	ViewportTransform SavedXForm = Ctx.XForm;

	for (int i = 0; i < m_List.GetCount(); i++)
		{
		CSpaceObject *pObj = m_List[i];

		//	Adjust the transform to deal with parallax

		Ctx.XForm = ViewportTransform(Ctx.vCenterPos, pObj->GetParallaxDist() * g_KlicksPerPixel, Ctx.xCenter, Ctx.yCenter);
		Ctx.XFormRel = Ctx.XForm;

		//	Figure out the position of the object in pixels

		int x, y;
		Ctx.XForm.Transform(pObj->GetPos(), &x, &y);

		//	Paint the object in the viewport

		SetProgramState(psPaintingSRS, pObj);

		Ctx.pObj = pObj;
		pObj->Paint(Dest, 
				x,
				y,
				Ctx);

		SetProgramState(psPaintingSRS);
		}

	Ctx.XForm = SavedXForm;
	Ctx.XFormRel = Ctx.XForm;
	}

void CUnorderedPaintList::Paint (CG32bitImage &Dest, SViewportPaintCtx &Ctx) const

//	Paint
//
//	Paint all objects in our list.

	{
	for (int i = 0; i < m_List.GetCount(); i++)
		{
		CSpaceObject *pObj = m_List[i];

		if (pObj->IsPaintNeeded())
			{
			//	Figure out the position of the object in pixels

			int x, y;
			Ctx.XForm.Transform(pObj->GetPos(), &x, &y);

			//	Paint the object in the viewport

			SetProgramState(psPaintingSRS, pObj);

			Ctx.pObj = pObj;
			pObj->Paint(Dest, 
					x,
					y,
					Ctx);

			SetProgramState(psPaintingSRS);
			}

		//	Clear destination, if necessary

		if (pObj->IsAutoClearDestination())
			pObj->ClearPlayerDestination();
		}
	}
