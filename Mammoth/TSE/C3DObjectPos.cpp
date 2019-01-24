//	C3DObjectPos.cpp
//
//	C3DObjectPos class
//	Copyright (c) 2017 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define ORIGIN_X_ATTRIB							CONSTLIT("originX")
#define ORIGIN_Y_ATTRIB							CONSTLIT("originY")
#define POS_ANGLE_ATTRIB						CONSTLIT("posAngle")
#define POS_RADIUS_ATTRIB						CONSTLIT("posRadius")
#define POS_X_ATTRIB							CONSTLIT("posX")
#define POS_Y_ATTRIB							CONSTLIT("posY")
#define POS_Z_ATTRIB							CONSTLIT("posZ")
#define X_ATTRIB								CONSTLIT("x")
#define Y_ATTRIB								CONSTLIT("y")
#define Z_ATTRIB								CONSTLIT("z")

C3DObjectPos::C3DObjectPos (const CVector &vPos, int iZ)

//	C3DObjectPos constructo

	{
	Metric rRadius;
	m_iPosAngle = VectorToPolar(vPos, &rRadius);
	m_iPosRadius = mathRound(rRadius / g_KlicksPerPixel);
	m_iPosZ = iZ;
	}

void C3DObjectPos::CalcCoord (int iScale, CVector *retvPos) const

//	CalcCoord
//
//	Calculates the coordinate

	{
	C3DConversion::CalcCoord(iScale, GetAngle(), GetRadius(), GetZ(), retvPos);
	}

void C3DObjectPos::CalcCoord (int iScale, int iRotation, CVector *retvPos) const

//	CalcCoord
//
//	Calculates the coordinate

	{
	C3DConversion::CalcCoord(iScale, AngleMod(iRotation + GetAngle()), GetRadius(), GetZ(), retvPos);
	}

bool C3DObjectPos::InitFromXY (int iScale, const CVector &vPos, int iZ)

//	InitFromXY
//
//	Reverse engineer a polar position based on XY.

	{
	if (iScale <= 0)
		{
		m_iPosAngle = 0;
		m_iPosRadius = 0;
		m_iPosZ = 0;
		return false;
		}

	Metric rAngle;
	Metric rRadius;
	C3DConversion::CalcPolar(iScale, vPos, iZ, &rAngle, &rRadius);

	m_iPosAngle = AngleMod(mathRound(mathRadiansToDegrees(rAngle)));
	m_iPosRadius = mathRound(rRadius);
	m_iPosZ = iZ;

	return true;
	}

bool C3DObjectPos::InitFromXML (CXMLElement *pDesc, DWORD dwFlags, bool *retb3DPos)

//	InitFromXML
//
//	Initializes from an XML element. We accept the following forms:
//
//	posAngle="nnn"	posRadius="nnn"	posZ="nnn"
//
//	OR
//
//	x="nnn" y="nnn" z="nnn"		-> use the 3D transformation

	{
	//	Initialize based on which of the formats we've got. If we have posAngle
	//	then we have polar coordinates.

	int iAngle;
	if (pDesc->FindAttributeInteger(POS_ANGLE_ATTRIB, &iAngle))
		{
		m_iPosAngle = AngleMod(iAngle);
		m_iPosRadius = pDesc->GetAttributeIntegerBounded(POS_RADIUS_ATTRIB, 0, -1);
		InitPosZFromXML(pDesc, retb3DPos);
		}

	//	If we don't support x,y coords, then we're done

	else if (dwFlags & FLAG_NO_XY)
		{
		m_iPosAngle = 0;
		m_iPosRadius = 0;
		m_iPosZ = 0;
		if (retb3DPos) *retb3DPos = false;
		return false;
		}

	//	Otherwise, we expect Cartessian coordinates

	else
		{
		//	Get the position

		int x;
		if (!pDesc->FindAttributeInteger(POS_X_ATTRIB, &x) && !pDesc->FindAttributeInteger(X_ATTRIB, &x))
			{
			m_iPosAngle = 0;
			m_iPosRadius = 0;
			m_iPosZ = 0;
			if (retb3DPos) *retb3DPos = false;
			return false;
			}

		int y;
		if (!pDesc->FindAttributeInteger(POS_Y_ATTRIB, &y) && !pDesc->FindAttributeInteger(Y_ATTRIB, &y))
			y = 0;
		else
			y = -y;

		InitPosZFromXML(pDesc, retb3DPos);

		//	Convert to polar coordinates

		if (dwFlags & FLAG_CALC_POLAR)
			{
			CVector vPos(x * g_KlicksPerPixel, y * g_KlicksPerPixel);
			Metric rAngle;
			Metric rRadius;
			C3DConversion::CalcPolar(C3DConversion::DEFAULT_SCALE, vPos, m_iPosZ, &rAngle, &rRadius);

			m_iPosAngle = mathRound(mathRadiansToDegrees(rAngle));
			m_iPosRadius = mathRound(rRadius);
			}
		else
			{
			int iRadius;
			m_iPosAngle = IntVectorToPolar(x, y, &iRadius);
			m_iPosRadius = iRadius;
			}
		}

	//	If we have an origin, then adjust the position.

	int xOrigin, yOrigin;
	if (pDesc->FindAttributeInteger(ORIGIN_X_ATTRIB, &xOrigin))
		{
		yOrigin = pDesc->GetAttributeInteger(ORIGIN_Y_ATTRIB);
		CVector vOffset(xOrigin, yOrigin);
		CVector vPos = PolarToVector(m_iPosAngle, m_iPosRadius);
		CVector vResult = vPos + vOffset;

		Metric rRadius;
		m_iPosAngle = VectorToPolar(vResult, &rRadius);
		m_iPosRadius = mathRound(rRadius);
		}

	return true;
	}

void C3DObjectPos::InitPosZFromXML (CXMLElement *pDesc, bool *retb3DPos)

//	InitPosZFromXML
//
//	Helper to load a Z position

	{
	int iPosZ;
	if (pDesc->FindAttributeInteger(POS_Z_ATTRIB, &iPosZ) || pDesc->FindAttributeInteger(Z_ATTRIB, &iPosZ))
		{
		m_iPosZ = iPosZ;
		if (retb3DPos) *retb3DPos = true;
		}
	else
		{
		m_iPosZ = 0;
		if (retb3DPos) *retb3DPos = false;
		}
	}

void C3DObjectPos::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	DWORD		m_iPosAngle, m_iPosRadius
//	DWORD		m_iPosZ, unused

	{
	DWORD dwLoad;
	Ctx.pStream->Read(dwLoad);
	m_iPosAngle = (int)LOWORD(dwLoad);
	m_iPosRadius = (int)HIWORD(dwLoad);

	Ctx.pStream->Read(dwLoad);
	m_iPosZ = (int)LOWORD(dwLoad);
	}

void C3DObjectPos::WriteToStream (IWriteStream &Stream) const

//	WriteToStream
//
//	DWORD		m_iPosAngle, m_iPosRadius
//	DWORD		m_iPosZ, unused

	{
	DWORD dwSave = MAKELONG(m_iPosAngle, m_iPosRadius);
	Stream.Write(dwSave);

	dwSave = MAKELONG(m_iPosZ, 0);
	Stream.Write(dwSave);
	}
