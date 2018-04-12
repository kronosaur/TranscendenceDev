//	CIntegralRotationDesc.cpp
//
//	CIntegralRotationDesc class
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

const Metric MANEUVER_MASS_FACTOR =				1.0;
const Metric MAX_INERTIA_RATIO =				9.0;

CIntegralRotationDesc::SFacingsData CIntegralRotationDesc::m_FacingsData[360 + 1];

CIntegralRotationDesc::CIntegralRotationDesc (void) :
        m_iCount(20),
        m_iMaxRotationRate(0),
        m_iRotationAccel(0),
        m_iRotationAccelStop(0)

//  CIntegralRotationDesc constructor

    {
    }

int CIntegralRotationDesc::CalcFinalRotationFrame (int iRotationFrame, int iRotationSpeed) const

//	CalcFinalRotationFrame
//
//	Try to figure out what our final rotation frame would be if we stopped
//	thrusting and turned on inertia only.

	{
	while (iRotationSpeed > m_iRotationAccelStop || iRotationSpeed < -m_iRotationAccelStop)
		{
		if (iRotationSpeed > 0)
			iRotationSpeed = Max(0, iRotationSpeed - m_iRotationAccelStop);
		else
			iRotationSpeed = Min(0, iRotationSpeed + m_iRotationAccelStop);

		if (iRotationSpeed != 0)
			{
			int iFrameMax = m_iCount * ROTATION_FRACTION;

			iRotationFrame = (iRotationFrame + iRotationSpeed) % iFrameMax;
			if (iRotationFrame < 0)
				iRotationFrame += iFrameMax;
			}
		}

	return iRotationFrame;
	}

int CIntegralRotationDesc::GetManeuverDelay (void) const

//	GetManeuverDelay
//
//	For compatibility we convert from our internal units to old style 
//	maneuverability (ticks per rotation angle)

	{
	return (m_iMaxRotationRate > 0 ? (int)(ROTATION_FRACTION / m_iMaxRotationRate) : 0);
	}

Metric CIntegralRotationDesc::GetMaxRotationSpeedDegrees (void) const

//	GetMaxRotationSpeedDegrees
//
//	Returns the max speed in degrees per tick.

	{
	if (m_iCount == 0)
		return 0.0;

	return 360.0 * m_iMaxRotationRate / (ROTATION_FRACTION * m_iCount); 
	}

void CIntegralRotationDesc::Init (int iFrameCount, Metric rMaxRotation, Metric rAccel, Metric rAccelStop)

//	Init
//
//	Initialize from constants

	{
    m_iCount = iFrameCount;
	m_iMaxRotationRate = Max(1, mathRound(ROTATION_FRACTION * rMaxRotation * m_iCount / 360.0));
	m_iRotationAccel = Max(1, mathRound(ROTATION_FRACTION * rAccel * m_iCount / 360.0));
	m_iRotationAccelStop = Max(1, mathRound(ROTATION_FRACTION * rAccelStop * m_iCount / 360.0));

	InitFacingsData(m_iCount);
	}

bool CIntegralRotationDesc::InitFacingsData (int iCount)

//	InitFacingsData
//
//	Initializes facings data. Return FALSE if iCount is invalid.

	{
	int i;

	if (iCount <= 0 || iCount > 360)
		return false;
		
	if (m_FacingsData[iCount].bInitialized)
		return true;

	m_FacingsData[iCount].FrameIndexToAngle.InsertEmpty(iCount);
	Metric rFrameAngle = 360.0 / iCount;
	for (i = 0; i < iCount; i++)
		m_FacingsData[iCount].FrameIndexToAngle[i] = AngleMod(mathRound(90.0 - i * rFrameAngle));

	m_FacingsData[iCount].AngleToFrameIndex.InsertEmpty(360);
	for (i = 0; i < 360; i++)
		m_FacingsData[iCount].AngleToFrameIndex[i] = mathRound(AngleMod(90 - i) * iCount / 360.0) % iCount;

	m_FacingsData[iCount].bInitialized = true;

	return true;
	}

void CIntegralRotationDesc::InitFromDesc (const CRotationDesc &Desc)

//  InitFromDesc
//
//  Initialize from a descriptor (which uses double precision)

    {
	Init(Desc.GetFrameCount(), Desc.GetMaxRotationPerTick(), Desc.GetRotationAccelPerTick(), Desc.GetRotationAccelStopPerTick());
    }
