//	CExplosionColorizer.cpp
//
//	CExplosionColorizer class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "SFXFractalImpl.h"

const Metric SQRT2 =						sqrt(2.0);
const Metric SQRT2PI =						sqrt(2.0 * PI);
const Metric SIGMA_MAX =					0.01;
const Metric SIGMA_DECAY =					1.07;
const Metric MAX_INTENSITY =				100.0;

const Metric CORE_HEAT_LEVEL =				0.5;
const Metric FRINGE_HEAT_LEVEL =			0.35;
const Metric FRINGE_HEAT_RANGE =			(CORE_HEAT_LEVEL - FRINGE_HEAT_LEVEL);
const Metric FLAME_HEAT_LEVEL =				0.25;
const Metric FLAME_HEAT_RANGE =				(FRINGE_HEAT_LEVEL - FLAME_HEAT_LEVEL);
const Metric MIN_HEAT_LEVEL =				0.1;
const Metric MIN_HEAT_RANGE =				(FLAME_HEAT_LEVEL - MIN_HEAT_LEVEL);

CG32bitPixel CExplosionColorizer::GetPixel (int iRadius, int iMaxRadius, int iIntensity, CG32bitPixel rgbPrimary, CG32bitPixel rgbSecondary) const

//	GetPixel
//
//	Returns the pixel value

	{
	if (iRadius < 0 || iIntensity < 0 || iMaxRadius <= 0)
		return CG32bitPixel::Null();

	//	Compute the heat value at this radius

	Metric rHeat = m_Heat[Min(iIntensity, INTENSITY_COUNT - 1)][Min((int)((Metric)RADIUS_COUNT * iRadius / iMaxRadius), RADIUS_COUNT - 1)];
	if (rHeat > CORE_HEAT_LEVEL)
		return CG32bitPixel(255, 255, 255);
	else if (rHeat > FRINGE_HEAT_LEVEL)
		return CG32bitPixel::Blend(rgbPrimary, CG32bitPixel(255, 255, 255), (rHeat - FRINGE_HEAT_LEVEL) / FRINGE_HEAT_RANGE);
	else if (rHeat > FLAME_HEAT_LEVEL)
		{
		Metric rFade = pow((rHeat - FLAME_HEAT_LEVEL) / FLAME_HEAT_RANGE, 0.5);
		return CG32bitPixel::Blend(rgbSecondary, rgbPrimary, rFade);
		}
	else
		return rgbSecondary;
	}

void CExplosionColorizer::Init (void)

//	Init
//
//	We initialize 100 different arrays (one for each intensity level); each
//	array has 100 radius points.

	{
	int i, j;

	if (m_Heat.GetCount() > 0)
		return;

	m_Heat.InsertEmpty(INTENSITY_COUNT);

	for (i = 0; i < INTENSITY_COUNT; i++)
		{
		TArray<Metric> &HeatTable = m_Heat[i];
		HeatTable.InsertEmpty(RADIUS_COUNT);

		//	Compute the sigma^2 that we want based on the given intensity.
		//	For high intensity we want very low sigma^2 (to get a sharper
		//	peak).

		Metric rSigma2 = SIGMA_MAX * pow(SIGMA_DECAY, MAX_INTENSITY - i);
		Metric rSigma = sqrt(rSigma2);

		//	Compute the x (radius) value at which the function value is
		//	at our minimum level. We use this as the maximum radius point.
		//	NOTE: We get this formula by taking the normal distribution
		//	function and solving for x.
		
		Metric rMaxX = SQRT2 * sqrt(rSigma2 * log(1.0 / (rSigma * MIN_HEAT_LEVEL * SQRT2PI)));

		//	Now loop over all radius values

		for (j = 0; j < RADIUS_COUNT; j++)
			{
			Metric rX = rMaxX * ((Metric)j / RADIUS_COUNT);

			//	Compute the heat value at this radius position
			//	(This is the normal distribution function).

			HeatTable[j] = (1.0 / (rSigma * SQRT2PI)) * exp(-(rX * rX) / (2.0 * rSigma2));
			}
		}
	}
