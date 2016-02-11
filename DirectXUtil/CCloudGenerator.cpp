//	CCloudGenerator.cpp
//
//	CCloudGenerator Class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CCloudGenerator::CCloudGenerator (int iScale)

//	CCloudGenerator constructor

	{
	ASSERT(iScale > 0);

	//	Make sure the noise system is initialized.

	NoiseInit();

	//	We go all the way to single pixel resolution.

	m_iScale = iScale;
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
	while (iFreq >= iMinFreq)
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

Metric CCloudGenerator::GetNext (void)

//	GetNext
//
//	Returns the current value and advances the X position

	{
	int i;

	Metric rValue = 0.0;
	for (i = 0; i < m_Frequencies.GetCount(); i++)
		{
		SFreq &Freq = m_Frequencies[i];
		rValue += Freq.pNoise->GetAt(Freq.x, Freq.y) * Freq.rAmplitude;

		Freq.pNoise->Next(Freq.x);
		}

	return rValue;
	}

void CCloudGenerator::NextY (void)

//	NextY
//
//	Advances the Y position

	{
	int i;

	for (i = 0; i < m_Frequencies.GetCount(); i++)
		{
		SFreq &Freq = m_Frequencies[i];
		Freq.pNoise->Next(Freq.y);
		}
	}

void CCloudGenerator::ResetX (int x)

//	ResetX
//
//	Sets the X position to the given value

	{
	int i;

	for (i = 0; i < m_Frequencies.GetCount(); i++)
		{
		SFreq &Freq = m_Frequencies[i];
		Freq.pNoise->Reset(Freq.x, x);
		}
	}

void CCloudGenerator::ResetY (int y)

//	ResetY
//
//	Sets the Y position to the given value

	{
	int i;

	for (i = 0; i < m_Frequencies.GetCount(); i++)
		{
		SFreq &Freq = m_Frequencies[i];
		Freq.pNoise->Reset(Freq.y, y);
		}
	}
