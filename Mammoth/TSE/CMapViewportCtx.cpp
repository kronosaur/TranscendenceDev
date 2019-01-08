//	CMapViewportCtx.cpp
//
//	CMapViewportCtx Class
//	Copyright (c) 2014 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "math.h"

const int VIEWPORT_EXTRA =								256;
const CG32bitPixel RGB_GRID_LINE =						CG32bitPixel(43, 45, 51);

CMapViewportCtx::CMapViewportCtx (CSpaceObject *pCenter, const RECT &rcView, Metric rMapScale) :
		m_pCenter(pCenter),
		m_rcView(rcView),
		m_rMapScale(rMapScale)

//	CMapViewportCtx constructor

	{
	CVector vCenter = (m_pCenter ? m_pCenter->GetPos() : NullVector);

	//	Round the center on a pixel boundary

	m_vCenter = CVector(floor(vCenter.GetX() / rMapScale) * rMapScale, floor(vCenter.GetY() / rMapScale) * rMapScale);

	//	Figure out the boundary of the viewport in system coordinates

	m_xCenter = m_rcView.left + RectWidth(m_rcView) / 2;
	m_yCenter = m_rcView.top + RectHeight(m_rcView) / 2;

	CVector vDiagonal(m_rMapScale * (Metric)(RectWidth(rcView) + VIEWPORT_EXTRA) / 2,
				m_rMapScale * (Metric)(RectHeight(rcView) + VIEWPORT_EXTRA) / 2);
	m_vUR = m_vCenter + vDiagonal;
	m_vLL = m_vCenter - vDiagonal;

	//	Compute the transformation to map world coordinates to the viewport

	m_Trans = ViewportTransform(m_vCenter, 
			m_rMapScale, 
			m_rMapScale,
			m_xCenter,
			m_yCenter);
	}

bool CMapViewportCtx::IsInViewport (CSpaceObject *pObj) const 

//	IsInViewport
//
//	Returns TRUE if the given object is in the viewport
	
	{
	return pObj->InBox(m_vUR, m_vLL); 
	}

void CMapViewportCtx::Transform (const CVector &vPos, int *retx, int *rety) const

//	Transform
//
//	Transforms a global coordinate point to a screen coordinate.

	{
	if (m_b3DMap)
		{
		Metric rScale = RectWidth(m_rcView) * m_rMapScale / 2.0;

		CVector vTrans;
		C3DConversion::CalcCoord(rScale, vPos - m_vCenter, 0.0, &vTrans);

		*retx = (int)(m_xCenter + (vTrans.GetX() / m_rMapScale));
		*rety = (int)(m_yCenter - (vTrans.GetY() / m_rMapScale));
		}
	else
		return m_Trans.Transform(vPos, retx, rety);
	}
