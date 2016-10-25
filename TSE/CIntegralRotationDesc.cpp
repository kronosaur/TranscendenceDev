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

void CIntegralRotationDesc::InitFromDesc (const CRotationDesc &Desc)

//  InitFromDesc
//
//  Initialize from a descriptor (which uses double precision)

    {
    int i;

    m_iCount = Desc.GetFrameCount();
	m_iMaxRotationRate = Max(1, mathRound(ROTATION_FRACTION * Desc.GetMaxRotationPerTick() * m_iCount / 360.0));
	m_iRotationAccel = Max(1, mathRound(ROTATION_FRACTION * Desc.GetRotationAccelPerTick() * m_iCount / 360.0));
	m_iRotationAccelStop = Max(1, mathRound(ROTATION_FRACTION * Desc.GetRotationAccelStopPerTick() * m_iCount / 360.0));

	if (m_iCount > 0 && m_iCount <= 360 && !m_FacingsData[m_iCount].bInitialized)
		{
		//	Initialize constants arrays for this number of rotation facings.

		m_FacingsData[m_iCount].FrameIndexToAngle.InsertEmpty(m_iCount);
		Metric rFrameAngle = 360.0 / m_iCount;
		for (i = 0; i < m_iCount; i++)
			m_FacingsData[m_iCount].FrameIndexToAngle[i] = AngleMod(mathRound(90.0 - i * rFrameAngle));

		m_FacingsData[m_iCount].AngleToFrameIndex.InsertEmpty(360);
		for (i = 0; i < 360; i++)
			m_FacingsData[m_iCount].AngleToFrameIndex[i] = mathRound(AngleMod(90 - i) * m_iCount / 360.0) % m_iCount;

		m_FacingsData[m_iCount].bInitialized = true;
		}
    }
