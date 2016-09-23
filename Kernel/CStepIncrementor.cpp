//	CStepIncrementor.cpp
//
//	CStepIncrementor class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "Kernel.h"
#include "KernelObjID.h"
#include "Euclid.h"

CStepIncrementor::CStepIncrementor (EStyle iStyle, Metric rStart, Metric rEnd, int iSteps) :
		m_iStyle(iStyle),
		m_rStart(rStart),
		m_iSteps(iSteps)

//	CStepIncrementor constructor

	{
	int i;

	//	Edge conditions

	if (iSteps <= 0)
		{
		m_iSteps = 0;
		m_rRange = 0.0;
		m_rPower = 0.0;
		return;
		}

	switch (iStyle)
		{
		case styleLinear:
			m_rRange = (rEnd - m_rStart) / iSteps;
			break;

		case styleOct:
			m_rRange = (rEnd - m_rStart);
			m_rPower = 8.0;
			break;

		case styleOctRoot:
			m_rRange = (rEnd - m_rStart);
			m_rPower = 0.125;
			break;

		case styleSquare:
			m_rRange = (rEnd - m_rStart);
			m_rPower = 2.0;
			break;

		case styleSquareRoot:
			m_rRange = (rEnd - m_rStart);
			m_rPower = 0.5;
			break;

		case styleQuad:
			m_rRange = (rEnd - m_rStart);
			m_rPower = 4.0;
			break;

		case styleQuadRoot:
			m_rRange = (rEnd - m_rStart);
			m_rPower = 0.25;
			break;

		case styleSin:
			{
			Metric rCenter = 0.5 * (rStart + rEnd);
			Metric rScale = rCenter - rStart;
			Metric rAngle = 0.0;
			Metric rInc = TAU / iSteps;

			m_Cycle.InsertEmpty(iSteps);
			for (i = 0; i < iSteps; i++, rAngle += rInc)
				m_Cycle[i] = rCenter + (rScale * sin(rAngle));

			break;
			}
		}
	}

Metric CStepIncrementor::GetAt (int iStep) const

//	GetAt
//
//	Returns the value at the given step

	{
	if (m_iSteps == 0)
		return m_rStart;

	switch (m_iStyle)
		{
		case styleLinear:
			return m_rStart + (iStep * m_rRange);

		case styleOct:
		case styleOctRoot:
		case styleQuad:
		case styleQuadRoot:
		case styleSquare:
		case styleSquareRoot:
			{
			Metric rStep = (Metric)iStep / (Metric)m_iSteps;
			return m_rStart + (pow(rStep, m_rPower) * m_rRange);
			}

		case styleSin:
			return m_Cycle[iStep % m_iSteps];

		default:
			return 0.0;
		}
	}
