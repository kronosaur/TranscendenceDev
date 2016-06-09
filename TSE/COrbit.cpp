//	Orbits.cpp
//
//	Orbit class

#include "PreComp.h"
#include "math.h"

COrbit::COrbit (void) : m_rEccentricity(0.0),
		m_rSemiMajorAxis(0.0),
		m_rRotation(0.0),
		m_rPos(0.0)
	{
	}

COrbit::COrbit (const CVector &vCenter, Metric rRadius, Metric rPos) : m_vFocus(vCenter),
		m_rSemiMajorAxis(rRadius),
		m_rEccentricity(0.0),
		m_rRotation(0.0),
		m_rPos(rPos)
	{
	}

COrbit::COrbit (const CVector &vCenter, 
					   Metric rSemiMajorAxis,
					   Metric rEccentricity,
					   Metric rRotation,
					   Metric rPos) :
		m_vFocus(vCenter),
		m_rSemiMajorAxis(rSemiMajorAxis),
		m_rEccentricity(rEccentricity),
		m_rRotation(rRotation),
		m_rPos(rPos)
	{
	}

bool COrbit::operator== (const COrbit &Src) const

//  COrbit operator ==

    {
    return (m_vFocus == Src.m_vFocus
			&& m_rSemiMajorAxis == Src.m_rSemiMajorAxis
			&& m_rEccentricity == Src.m_rEccentricity
			&& m_rRotation == Src.m_rRotation
			&& m_rPos == Src.m_rPos);
    }

CG32bitPixel COrbit::GetColorAtRadiusHD (CG32bitPixel rgbColor, Metric rRadius) const

//  GetColorAtRadiusHD
//
//  Returns the color of the orbit at the given distance. For HD lines, we use
//  transparency instead of fading to black.

    {
#ifdef DEBUG
    return rgbColor;
#else
	Metric rFade = 0.25 + (LIGHT_SECOND * 180.0 / rRadius);
	if (rFade >= 1.0)
        return rgbColor;

	return CG32bitPixel(rgbColor, (BYTE)(DWORD)(0xff * rFade));
#endif
    }

CVector COrbit::GetPoint (Metric rAngle) const
	{
	Metric rRadius = m_rSemiMajorAxis * (1.0 - (m_rEccentricity * m_rEccentricity)) 
			/ (1.0 - m_rEccentricity * cos(rAngle));

	return m_vFocus + CVector(cos(rAngle + m_rRotation) * rRadius, sin(rAngle + m_rRotation) * rRadius);
	}

CVector COrbit::GetPointAndRadius (Metric rAngle, Metric *retrRadius) const
	{
	*retrRadius = m_rSemiMajorAxis * (1.0 - (m_rEccentricity * m_rEccentricity)) 
			/ (1.0 - m_rEccentricity * cos(rAngle));

	return m_vFocus + CVector(cos(rAngle + m_rRotation) * (*retrRadius), sin(rAngle + m_rRotation) * (*retrRadius));
	}

CVector COrbit::GetPointCircular (Metric rAngle) const
	{
	return m_vFocus + CVector(cos(rAngle) * m_rSemiMajorAxis, sin(rAngle) * m_rSemiMajorAxis);
	}

void COrbit::Paint (CMapViewportCtx &Ctx, CG32bitImage &Dest, CG32bitPixel rgbColor)

//	Paint
//
//	Paint the orbit

	{
	DWORD redValue = rgbColor.GetRed();
	DWORD greenValue = rgbColor.GetGreen();
	DWORD blueValue = rgbColor.GetBlue();

	//	Paint circular orbits in a single color; eccentric orbits change color
	//	since they are not equidistant from the sun

	if (m_rEccentricity == 0.0)
		{
		Metric rAngle;
		const Metric rIncrement = PI / 90.0;
		int xPrev, yPrev;
		CG32bitPixel rgbColor;

		//	The orbit color fades depending on the distance from the sun

		Metric rFade = 0.25 + (LIGHT_SECOND * 180.0 / m_rSemiMajorAxis);
		if (rFade < 1.0)
			rgbColor = CG32bitPixel((BYTE)(redValue * rFade), (BYTE)(greenValue * rFade), (BYTE)(blueValue * rFade));
		else
			rgbColor = CG32bitPixel((BYTE)redValue, (BYTE)greenValue, (BYTE)blueValue);

		//	Compute the position of the starting point

		Ctx.Transform(GetPointCircular(0.0), &xPrev, &yPrev);

		//	Paint the orbit in multiple segments

		for (rAngle = rIncrement; rAngle < PI * 2.0; rAngle += rIncrement)
			{
			//	Compute the end point

			int x, y;
			Ctx.Transform(GetPointCircular(rAngle), &x, &y);

			//	Draw a line segment

			Dest.DrawLine(xPrev, yPrev, x, y, 1, rgbColor);

			//	Next point

			xPrev = x;
			yPrev = y;
			}
		}
	else
		{
		Metric rAngle;
		const Metric rIncrement = PI / 90.0;
		int xPrev, yPrev;

		//	Compute the position of the starting point

		Ctx.Transform(GetPoint(0.0), &xPrev, &yPrev);

		//	Paint the orbit in multiple segments

		for (rAngle = rIncrement; rAngle < PI * 2.0; rAngle += rIncrement)
			{
			Metric rRadius;
			CVector vPos = GetPointAndRadius(rAngle, &rRadius);
			CG32bitPixel rgbColor;

			//	Compute the end point

			int x, y;
			Ctx.Transform(vPos, &x, &y);

			//	The orbit color fades depending on the distance from the sun

			Metric rFade = 0.25 + (LIGHT_SECOND * 180.0 / rRadius);
			if (rFade < 1.0)
				rgbColor = CG32bitPixel((BYTE)(redValue * rFade), (BYTE)(greenValue * rFade), (BYTE)(blueValue * rFade));
			else
				rgbColor = CG32bitPixel((BYTE)redValue, (BYTE)greenValue, (BYTE)blueValue);

			//	Draw a line segment

			Dest.DrawLine(xPrev, yPrev, x, y, 1, rgbColor);

			//	Next point

			xPrev = x;
			yPrev = y;
			}
		}
	}

void COrbit::PaintHD (CMapViewportCtx &Ctx, CG32bitImage &Dest, CG32bitPixel rgbColor, CGDraw::EBlendModes iMode)

//	Paint
//
//	Paint the orbit

	{
	//	Paint circular orbits in a single color; eccentric orbits change color
	//	since they are not equidistant from the sun

	if (m_rEccentricity == 0.0)
		{
		Metric rAngle;
		const Metric rIncrement = PI / 90.0;
		int xPrev, yPrev;

		//	The orbit color fades depending on the distance from the sun

		CG32bitPixel rgbOrbit = GetColorAtRadiusHD(rgbColor, m_rSemiMajorAxis);

		//	Compute the position of the starting point

		Ctx.Transform(GetPointCircular(0.0), &xPrev, &yPrev);

		//	Paint the orbit in multiple segments

		for (rAngle = rIncrement; rAngle < PI * 2.0; rAngle += rIncrement)
			{
			//	Compute the end point

			int x, y;
			Ctx.Transform(GetPointCircular(rAngle), &x, &y);

			//	Draw a line segment

			CGDraw::LineHD(Dest, xPrev, yPrev, x, y, 2, rgbOrbit, iMode);

			//	Next point

			xPrev = x;
			yPrev = y;
			}
		}
	else
		{
		Metric rAngle;
		const Metric rIncrement = PI / 90.0;
		int xPrev, yPrev;

		//	Compute the position of the starting point

		Ctx.Transform(GetPoint(0.0), &xPrev, &yPrev);

		//	Paint the orbit in multiple segments

		for (rAngle = rIncrement; rAngle < PI * 2.0; rAngle += rIncrement)
			{
			Metric rRadius;
			CVector vPos = GetPointAndRadius(rAngle, &rRadius);
			CG32bitPixel rgbColor;

			//	Compute the end point

			int x, y;
			Ctx.Transform(vPos, &x, &y);

			//	The orbit color fades depending on the distance from the sun,
            //  so we have to calculated it for each segment.

    		CG32bitPixel rgbOrbit = GetColorAtRadiusHD(rgbColor, m_rSemiMajorAxis);

			//	Draw a line segment

			CGDraw::LineHD(Dest, xPrev, yPrev, x, y, 2, rgbOrbit, iMode);

			//	Next point

			xPrev = x;
			yPrev = y;
			}
		}
	}
