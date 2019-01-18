//	CGCloudGenerator3D.cpp
//
//	CGCloudGenerator3D Class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CGCloudGenerator3D::CGCloudGenerator3D (int iScale, int iFrames, int iMaxLevels)

//	CCloudGenerator constructor

	{
	ASSERT(iScale > 0);

	//	Make sure the noise system is initialized.

	NoiseInit();

	//	We go all the way to single pixel resolution.

	m_iScale = iScale;
	m_iFrames = iFrames;
	m_iMaxLevels = iMaxLevels;
	int iMinFreq = 1;

	//	In theory, the noise function returns values from -1.0 to 1.0, but
	//	in practice the values are clutered between -0.5 to 0.5. We multiply
	//	the result by this amplitude adjustment so that we get a greater
	//	dynamic range.

	Metric rAmplitudeAdj = 2.0;

	//	As we get to smaller and smaller detail, the amplitude decreases
	//	(by half for each frequency). But if there are too many levels of
	//	detail, the amplitude decreases so much that it is invisible.
	//	Thus we have a minimum amplitude.

	Metric rMinAmplitude = (Metric)iScale / 32.0;

	//	Compute the maximum possible value (so that our callers can scale the
	//	result appropriately).

	m_rMaxValue = 0.0;

	//	Generate all the noise generators

	int iFreq = iScale;
	while (iFreq >= iMinFreq 
			&& (iMaxLevels == -1 || m_Frequencies.GetCount() < iMaxLevels))
		{
		SFreq *pFreq = m_Frequencies.Insert();

		pFreq->iFreq = iFreq;
		Metric rAmplitude = Max((Metric)iFreq, rMinAmplitude);
		pFreq->rAmplitude = rAmplitudeAdj * rAmplitude;
		pFreq->pNoise = new CNoiseGenerator(iFreq);

		iFreq = iFreq / 2;

		m_rMaxValue += rAmplitude;
		}
	}

Metric CGCloudGenerator3D::GetAtPeriodic (int x, int y, int z) const

//	GetAt
//
//	Returns the value at the given position.

	{
	int i;

	Metric rValue = 0.0;
	for (i = 0; i < m_Frequencies.GetCount(); i++)
		{
		const SFreq &Freq = m_Frequencies[i];

		//	Generate points in noise coordinates

		SNoisePos xNoise;
		Freq.pNoise->Reset(xNoise, x);

		SNoisePos yNoise;
		Freq.pNoise->Reset(yNoise, y);

		SNoisePos zNoise;
		Freq.pNoise->Reset(zNoise, z);

		//	Get the value

		rValue += Freq.pNoise->GetAtPeriodic(m_iFrames, xNoise, yNoise, zNoise) * Freq.rAmplitude;
		}

	return rValue;
	}

