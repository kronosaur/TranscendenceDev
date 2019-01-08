//	CCircleRadiusDisruptor.cpp
//
//	CCircleRadiusDisruptor class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "SFXFractalImpl.h"

const int MAX_RESOLUTION_ANGLES =		200;

void CCircleRadiusDisruptor::Init (Metric rDisruption, int iRadius, int iAngleRange)

//	Init
//
//	Initializes an array of radius adjustments.

	{
	int i;

	ASSERT(rDisruption >= 0.0);

	//	Initialize the radius table with the full-resolution angles

	if (m_FullRadiusAdj.GetCount() == 0)
		{
		m_FullRadiusAdj.DeleteAll();
		m_FullRadiusAdj.InsertEmpty(MAX_RESOLUTION_ANGLES);

		//	Recursively initialize the array

		m_FullRadiusAdj[0] = RandomPoint(rDisruption);
		InitSegment(0, m_FullRadiusAdj.GetCount(), m_FullRadiusAdj[0], rDisruption);

		//	Compute the minimum an maximum disruption so we can normalize it

		Metric rMax = -1000.0;
		Metric rMin = 1000.0;
		for (i = 0; i < m_FullRadiusAdj.GetCount(); i++)
			{
			if (m_FullRadiusAdj[i] > rMax)
				rMax = m_FullRadiusAdj[i];
			if (m_FullRadiusAdj[i] < rMin)
				rMin = m_FullRadiusAdj[i];
			}

		//	NOTE: rRange can be 0.0 if rDisruption is 0.0

		Metric rRange = rMax - rMin;
		Metric rAdj = (rRange > 0.0 ? rDisruption / rRange : 0.0);

		//	Now adjust so that we end up with a value from 1.0 to 1.0 + disrupt

		for (i = 0; i < m_FullRadiusAdj.GetCount(); i++)
			m_FullRadiusAdj[i] = 1.0 + ((m_FullRadiusAdj[i] - rMin) * rAdj);
		}

	//	Now initialize the table that is scaled to the given angle range.

	if (m_RadiusAdj.GetCount() != iAngleRange)
		{
		m_RadiusAdj.DeleteAll();
		m_RadiusAdj.InsertEmpty(iAngleRange);

		for (i = 0; i < m_RadiusAdj.GetCount(); i++)
			m_RadiusAdj[i] = m_FullRadiusAdj[(int)((Metric)m_FullRadiusAdj.GetCount() * i / iAngleRange)];
		}
	}

void CCircleRadiusDisruptor::InitSegment (int iStart, int iCount, Metric rEndAdj, Metric rDisruption)

//	InitSegment
//
//	Initializes the segment from the given array index.

	{
	if (iCount <= 1)
		return;

	//	Pick a point in the middle and set it

	int iFirstCount = (iCount / 2);
	int iMiddle = iStart + iFirstCount;
	Metric rAverage = (m_FullRadiusAdj[iStart] + rEndAdj) / 2;
	m_FullRadiusAdj[iMiddle] = rAverage + RandomPoint(rDisruption);

	//	Recurse down to each segment

	InitSegment(iStart, iFirstCount, m_FullRadiusAdj[iMiddle], 0.5 * rDisruption);
	InitSegment(iMiddle, iCount - iFirstCount, rEndAdj, 0.5 * rDisruption);
	}
