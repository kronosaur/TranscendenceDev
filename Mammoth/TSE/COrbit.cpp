//	Orbits.cpp
//
//	Orbit class

#include "PreComp.h"
#include "math.h"

#define CLASS_CORBIT					CONSTLIT("COrbit")

COrbit::COrbit (const CVector &vCenter, Metric rRadius, Metric rPos) : 
		m_vFocus(CVector3D(vCenter.GetX(), vCenter.GetY(), 0.0)),
		m_rSemiMajorAxis(rRadius),
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
			&& m_rInclination == Src.m_rInclination
			&& m_rPos == Src.m_rPos);
    }

ICCItemPtr COrbit::AsItem (void) const

//	AsItem
//
//	Serializes the orbit to an item.

	{
	SSerialized Serialized;
	Serialized.vFocus = CVector(m_vFocus.GetX(), m_vFocus.GetY());
	Serialized.rFocusZ = m_vFocus.GetZ();
	Serialized.rEccentricity = m_rEccentricity;
	Serialized.rSemiMajorAxis = m_rSemiMajorAxis;
	Serialized.rRotation = m_rRotation;
	Serialized.rInclination = m_rInclination;
	Serialized.rPos = m_rPos;

	return ICCItemPtr(CreateListFromBinary(CLASS_CORBIT, &Serialized, sizeof(Serialized)));
	}

bool COrbit::FromItem (const ICCItem &Item, COrbit &retOrbit)

//	FromItem
//
//	Deserializes from an item.

	{
	retOrbit = COrbit();

	//	Nil means default orbit

	if (Item.IsNil())
		return true;

	//	Must be a list

	else if (!Item.IsList())
		return false;

	//	Load binary from list and check the class

	else
		{
		SSerialized Serialized;
		if (!CreateBinaryFromList(CLASS_CORBIT, Item, &Serialized))
			return false;

		retOrbit.SetFocus(CVector3D(Serialized.vFocus.GetX(), Serialized.vFocus.GetY(), Serialized.rFocusZ));
		retOrbit.SetEccentricity(Serialized.rEccentricity);
		retOrbit.SetSemiMajorAxis(Serialized.rSemiMajorAxis);
		retOrbit.SetRotation(Serialized.rRotation);
		retOrbit.SetInclination(Serialized.rInclination);
		retOrbit.m_rPos = Serialized.rPos;

		return true;
		}
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

CVector3D COrbit::GetObjectPos3D (void) const

//	GetObjectPos3D
//
//	Returns a 3D position

	{
	Metric rPosZ;
	CVector vPos = GetPoint(m_rPos, &rPosZ);
	return CVector3D(vPos.GetX(), vPos.GetY(), rPosZ);
	}

CVector COrbit::GetPoint (Metric rAngle, Metric *retrZ) const
	{
	Metric rRadius = m_rSemiMajorAxis * (1.0 - (m_rEccentricity * m_rEccentricity)) 
			/ (1.0 - m_rEccentricity * cos(rAngle));

	if (m_rInclination == 0.0)
		{
		if (retrZ) *retrZ = m_vFocus.GetZ();
		return CVector(m_vFocus.GetX(), m_vFocus.GetY()) + CVector(cos(rAngle + m_rRotation) * rRadius, sin(rAngle + m_rRotation) * rRadius);
		}
	else
		{
		//	For orbits with inclinations, we first compute the point on the 
		//	orbit in the orbital plane.

		CVector vPointOnPlane = CVector(cos(rAngle) * rRadius, sin(rAngle) * rRadius);

		//	Next we tilt the orbit and end up with X, Y, Z coordinates.

		CVector vResult(vPointOnPlane.GetX() * cos(m_rInclination), vPointOnPlane.GetY());
		if (retrZ) *retrZ = m_vFocus.GetZ() + (vPointOnPlane.GetX() * sin(m_rInclination));

		//	Then we rotate the point around the Z axis in case the orbit has a
		//	rotation.

		if (m_rRotation)
			vResult = vResult.Rotate(m_rRotation);

		//	And finally we can offset from the focus

		return CVector(m_vFocus.GetX(), m_vFocus.GetY()) + vResult;
		}
	}

CVector COrbit::GetPointAndRadius (Metric rAngle, Metric *retrRadius) const
	{
	*retrRadius = m_rSemiMajorAxis * (1.0 - (m_rEccentricity * m_rEccentricity)) 
			/ (1.0 - m_rEccentricity * cos(rAngle));

	return CVector(m_vFocus.GetX(), m_vFocus.GetY()) + CVector(cos(rAngle + m_rRotation) * (*retrRadius), sin(rAngle + m_rRotation) * (*retrRadius));
	}

CVector COrbit::GetPointCircular (Metric rAngle) const
	{
	return CVector(m_vFocus.GetX(), m_vFocus.GetY()) + CVector(cos(rAngle) * m_rSemiMajorAxis, sin(rAngle) * m_rSemiMajorAxis);
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

void COrbit::PaintHD (CMapViewportCtx &Ctx, CG32bitImage &Dest, CG32bitPixel rgbColor, CGDraw::EBlendModes iMode) const

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

void COrbit::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Read from stream.

	{
	//	In previous versions we used a 2D vector

	if (Ctx.dwVersion >= 175)
		m_vFocus.ReadFromStream(*Ctx.pStream);
	else
		{
		CVector vFocus;
		vFocus.ReadFromStream(*Ctx.pStream);
		m_vFocus = CVector3D(vFocus.GetX(), vFocus.GetY(), 0.0);
		}

	//	Read other values

	Ctx.pStream->Read(m_rEccentricity);
	Ctx.pStream->Read(m_rSemiMajorAxis);
	Ctx.pStream->Read(m_rRotation);
	Ctx.pStream->Read(m_rPos);

	//	Added in 175

	if (Ctx.dwVersion >= 175)
		Ctx.pStream->Read(m_rInclination);
	else
		m_rInclination = 0.0;
	}

void COrbit::WriteToStream (IWriteStream &Stream) const

//	WriteToStream
//
//	Write to stream.

	{
	m_vFocus.WriteToStream(Stream);
	Stream.Write(m_rEccentricity);
	Stream.Write(m_rSemiMajorAxis);
	Stream.Write(m_rRotation);
	Stream.Write(m_rPos);
	Stream.Write(m_rInclination);
	}

Metric COrbit::ZToParallax (Metric rZ)

//	ZToParallax
//
//	Convert from Cartessian z position to a parallax.

	{
	if (rZ == 0.0)
		return 1.0;
	else
		return Max(0.01, 1.0 + (rZ / CAMERA_DIST));
	}
