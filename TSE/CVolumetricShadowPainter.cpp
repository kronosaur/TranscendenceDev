//	CVolumetricShadowPainter.cpp
//
//	CVolumetricShadowPainter class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CVolumetricShadowPainter::CVolumetricShadowPainter (CSpaceObject *pStar, int xStar, int yStar, int iShadowAngle, Metric rStarDist, CSpaceObject *pObj, CG8bitSparseImage &DestMask) :
		m_DestMask(DestMask),
		m_bNoShadow(false),
		m_pStar(pStar),
		m_xStar(xStar),
		m_yStar(yStar),
		m_iShadowAngle(iShadowAngle),
		m_rStarDist(rStarDist),
		m_LightingCtx(iShadowAngle),
		m_pObj(pObj)

//	CVolumetricShadowPainter constructor

	{
	int i;

	//	Get the origin, width, and length of the shadow from the object

	int xCenter;
	int yCenter;
	int iShadowWidth;
	int iShadowLength;
	if (!m_pObj->CalcVolumetricShadowLine(m_LightingCtx, &xCenter, &yCenter, &iShadowWidth, &iShadowLength))
		{
		m_bNoShadow = true;
		return;
		}

	m_vShadowObjOrigin = CVector(xCenter, yCenter);
	m_rShadowWidth = iShadowWidth;
	m_rShadowLength = iShadowLength;
	m_rShadowHalfWidth = 0.5 * m_rShadowWidth;
	m_rShadowHalfLength = 0.5 * m_rShadowLength;

	//	Compute the center of the object relative to the volumetric mask

	m_vObjCenter = CVector((m_DestMask.GetWidth() / 2) + (m_pObj->GetPos().GetX() / g_KlicksPerPixel) - m_xStar,
			(m_DestMask.GetHeight() / 2) - (m_pObj->GetPos().GetY() / g_KlicksPerPixel) + m_yStar);

	//	Compute the origin of the shadow in mask coordinates

	m_vShadowOrigin = m_vObjCenter + m_vShadowObjOrigin;

	//	Compute the four corners of the shadow box in mask coordinates.

	CVector Vertex[4];
	Vertex[0] = m_vShadowOrigin + (m_LightingCtx.vSw * 0.5 * m_rShadowWidth);
	Vertex[1] = m_vShadowOrigin - (m_LightingCtx.vSw * 0.5 * m_rShadowWidth);

	//	Now compute the other two points of the shadow box.

	Vertex[2] = Vertex[1] + m_LightingCtx.vSl * m_rShadowLength;
	Vertex[3] = Vertex[0] + m_LightingCtx.vSl * m_rShadowLength;

	//	Compute the four corners of a rect containing the entire shadow (in
	//	mask coordinates)

	m_vUL = Vertex[0];
	m_vLR = Vertex[0];
	for (i = 1; i < 4; i++)
		{
		if (Vertex[i].GetX() < m_vUL.GetX())
			m_vUL.SetX(Vertex[i].GetX());

		if (Vertex[i].GetX() > m_vLR.GetX())
			m_vLR.SetX(Vertex[i].GetX());

		if (Vertex[i].GetY() < m_vUL.GetY())
			m_vUL.SetY(Vertex[i].GetY());

		if (Vertex[i].GetY() > m_vLR.GetY())
			m_vLR.SetY(Vertex[i].GetY());
		}
	}

void CVolumetricShadowPainter::GetShadowRect (RECT *retrcRect) const

//	GetShadowRect
//
//	Returns the shadow rect in mask coordinates

	{
	retrcRect->left = (int)m_vUL.GetX();
	retrcRect->top = (int)m_vUL.GetY();
	retrcRect->right = (int)ceil(m_vLR.GetX());
	retrcRect->bottom = (int)ceil(m_vLR.GetY());
	}

void CVolumetricShadowPainter::PaintShadow (void) const

//	PaintShadow
//
//	Paint the shadow

	{
	//	Compute the upper-left corner of the box in shadow coordinates

	CVector vULRelativeToShadowOrigin = m_vUL - m_vShadowOrigin;
	CVector vShadowRow = CVector(vULRelativeToShadowOrigin.Dot(m_LightingCtx.vSw), vULRelativeToShadowOrigin.Dot(m_LightingCtx.vSl));

	//	Allocate a temporary byte array to store the a row

	int cxRow = (int)m_vLR.GetX() - (int)m_vUL.GetX();
	BYTE *pSrcRow = new BYTE [cxRow];

	//	Loop over all rows in the shadow box

	int yPos = (int)m_vUL.GetY();
	int yPosEnd = (int)m_vLR.GetY();
	while (yPos < yPosEnd)
		{
		int xPos = (int)m_vUL.GetX();
		CVector vShadowPos = vShadowRow;

		BYTE *pSrcPos = pSrcRow;
		BYTE *pSrcPosEnd = pSrcRow + cxRow;

		while (pSrcPos < pSrcPosEnd)
			{
			//	rWFrac is the position of this pixel along the width of the 
			//	shadow.
			//
			//	0.0 = at the center of the shadow.
			//	1.0 = at one of the edges.

			Metric rWFrac = Absolute(vShadowPos.GetX()) / m_rShadowHalfWidth;
			if (rWFrac < 1.0)
				{
				//	rLFrac is the position of this pixel along the length of 
				//	the shadow.
				//
				//	0.0 is at the beginning (near the planet)
				//	1.0 is at the end of the shadow

				Metric rLFrac = vShadowPos.GetY() / m_rShadowLength;

				if (rLFrac > 0.0 && rLFrac < 1.0)
					{
					//	rUFrac is the size of the umbra at this point
					//	(measured along the shadow width axis from the center
					//	of the shadow.
					//
					//	rUFrac is ~1.0 near the beginning of the shadow.
					//	it is ~0.0 near the end of the shadow.

					Metric rUFrac = 1.0 - rLFrac;

					//	Inside the umbra?

					if (rWFrac < rUFrac)
						{
						if (rLFrac < 0.5)
							*pSrcPos = 0x00;
						else
							*pSrcPos = (BYTE)((rLFrac - 0.5) * 510);
						}

					//	In the penumbra

					else
						{
						//	rPFrac is the pixel's position in the penumbra along
						//	the shadow width axis.
						//
						//	0.0 is at the edge of the umbra.
						//	1.0 is at the edge of the shadow.

						Metric rPFrac = (rWFrac - rUFrac) / rLFrac;

						if (rLFrac < 0.5)
							*pSrcPos = (BYTE)(0xff * rPFrac);
						else
							*pSrcPos = (BYTE)(0xff * (1.0 - ((1.0 - ((rLFrac - 0.5) * 2.0)) * (1.0 - rPFrac))));
						}
					}
				else
					*pSrcPos = 0xff;
				}
			else
				*pSrcPos = 0xff;

			pSrcPos++;
			vShadowPos = vShadowPos + m_LightingCtx.vIncX;
			}

		//	Apply the row to the mask

		m_DestMask.FillLine(xPos, yPos, cxRow, pSrcRow);

		vShadowRow = vShadowRow + m_LightingCtx.vIncY;
		yPos++;
		}

	delete [] pSrcRow;
	}
