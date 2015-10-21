//	SFXFractalImpl.h
//
//	Classes to implement explosion effects
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved

#pragma once

//	CSphericalTextureMapper
//
//	This helper class is used to paint a spherical UV texture to the points in 
//	a circle, given an angle and radius.

class CSphericalTextureMapper
	{
	public:
		CSphericalTextureMapper (void) :
				m_pTexture(NULL)
			{ }

		inline BYTE GetPixel (int iAngle, int iRadius) const
			{
			return (m_pTexture ? m_pTexture->GetPixel(m_AngleToX[iAngle], m_RadiusToY[iRadius]) : 0);
			}

		void Init (CFractalTextureLibrary::ETextureTypes iTexture, int iFrame, int iRadius, int iAngleRange);

	private:
		const CG8bitImage *m_pTexture;
		TArray<int> m_AngleToX;
		TArray<int> m_RadiusToY;
	};

//	CCircleRadiusDisruptor
//
//	This helper class is used to distort the edges of a circle by adjusting
//	its radius at each angle.

class CCircleRadiusDisruptor
	{
	public:
		inline int GetAdjustedRadius (int iAngle, int iRadius) const
			{
			return (int)(m_RadiusAdj[iAngle] * iRadius);
			}

		void Init (Metric rDisruption, int iRadius, int iAngleRange);

	private:
		void InitSegment (int iStart, int iCount, Metric rEndAdj, Metric rDisruption);
		inline Metric RandomPoint (Metric rRange) { return mathRandomGaussian() * rRange; }

		TArray<Metric> m_RadiusAdj;
		TArray<Metric> m_FullRadiusAdj;
	};

//	CExplosionColorizer
//
//	This helper class generates static tables for the color of an explosion 
//	pixel at various radii and intensities.
//
//	We use a normal probability density function to simulate the distribution
//	of heat across the explosion. That is, we start with a very narrow bell 
//	curve (center is very hot) and proceed to wider and wider versions as the
//	heat distributes to the rest of the fireball.

class CExplosionColorizer
	{
	public:
		CG32bitPixel GetPixel (int iRadius, int iMaxRadius, int iIntensity, CG32bitPixel rgbPrimary, CG32bitPixel rgbSecondary) const;
		void Init (void);

	private:
		enum EConstants
			{
			RADIUS_COUNT =		100,
			INTENSITY_COUNT =	101,	//	0 to 100
			};

		TArray<TArray<Metric>> m_Heat;
	};

//	CCloudCirclePainter
//
//	We paint a spherical fractal cloud with a color table keyed to radius.
//	rFrame is from 0.0 to 1.0. Use (Metric){frame} / {frameCount}.

class CCloudCirclePainter : public TCirclePainter32<CCloudCirclePainter>
	{
	public:
		CCloudCirclePainter (CFractalTextureLibrary::ETextureTypes iTexture) :
				m_iTexture(iTexture),
				m_pColorTable(NULL)
			{
			}

		virtual void SetParam (const CString &sParam, const TArray<CG32bitPixel> &ColorTable)
			{
			if (strEquals(sParam, CONSTLIT("colorTable")))
				m_pColorTable = &ColorTable;
			}

	private:
		bool BeginDraw (void)
			{
			//	We need enough angular resolution to reach the pixel level (but
			//	no more).

			m_iAngleRange = (int)(m_iRadius * 2.0 * g_Pi);

			//	Set the texture based on the frame

			m_Texture.Init(m_iTexture, m_iFrame, m_iRadius, m_iAngleRange);

			//	Success

			return true;
			}

		inline CG32bitPixel GetColorAt (int iAngle, int iRadius) const 

		//	GetColorAt
		//
		//	Returns the color at the given position in the circle. NOTE: We must
		//	return a pre-multiplied pixel.

			{
			//	Get the alpha value at the texture position.

			BYTE byAlpha = m_Texture.GetPixel(iAngle, iRadius);

			//	Look up the pixel in the color table

			CG32bitPixel rgbColor = (m_pColorTable ? m_pColorTable->GetAt(iRadius) : CG32bitPixel(0, 255, 255));

			//	Now combine the alpha.

			byAlpha = CG32bitPixel::BlendAlpha(rgbColor.GetAlpha(), byAlpha);

			//	Return the value (premultiplied)

			return CG32bitPixel::PreMult(CG32bitPixel(rgbColor, byAlpha));
			}

		//	Context

		CFractalTextureLibrary::ETextureTypes m_iTexture;
		const TArray<CG32bitPixel> *m_pColorTable;

		//	Run time parameters for drawing a single frame.

		CSphericalTextureMapper m_Texture;

		friend TCirclePainter32;
	};

//	CFireballCirclePainter
//
//	A fireball has two layers: the bottom layer is a radially symmetric glow 
//	effect (using a given color table). On top we paint a fractal cloud wrapped
//	on a sphere (to simulate the fractal smoke patterns in an explosion). This
//	second layer has its own color table (also keyed to radius).
//
//	For efficiency we paint both layers at the same time, using the fractal 
//	cloud pattern as the discriminator.

class CFireballCirclePainter : public TCirclePainter32<CFireballCirclePainter>
	{
	public:
		CFireballCirclePainter (CFractalTextureLibrary::ETextureTypes iTexture, Metric rDistortion = 0.0) :
				m_iTexture(iTexture),
				m_rDistortion(rDistortion),
				m_pExplosionTable(NULL),
				m_pSmokeTable(NULL)
			{
			}

		virtual void SetParam (const CString &sParam, const TArray<CG32bitPixel> &ColorTable)
			{
			if (strEquals(sParam, CONSTLIT("explosionTable")))
				m_pExplosionTable = &ColorTable;
			else if (strEquals(sParam, CONSTLIT("smokeTable")))
				m_pSmokeTable = &ColorTable;
			}

	private:
		bool BeginDraw (void)
			{
			//	We need enough angular resolution to reach the pixel level (but
			//	no more).

			m_iAngleRange = (int)(m_iRadius * 2.0 * g_Pi);

			//	Set the texture based on the frame

			m_Texture.Init(m_iTexture, m_iFrame, m_iRadius, m_iAngleRange);

			//	Initialize the disruptor

			m_Disruptor.Init(m_rDistortion, m_iRadius, m_iAngleRange);

			//	Success

			return true;
			}

		inline CG32bitPixel GetColorAt (int iAngle, int iRadius) const 

		//	GetColorAt
		//
		//	Returns the color at the given position in the circle. NOTE: We must
		//	return a pre-multiplied pixel.

			{
			//	Adjust the radius based on the disruptor

			int iNewRadius = m_Disruptor.GetAdjustedRadius(iAngle, iRadius);
			if (iNewRadius >= m_iRadius)
				return CG32bitPixel::Null();

			//	Get the alpha value at the texture position.

			BYTE byAlpha = m_Texture.GetPixel(iAngle, iNewRadius);

			//	Combine the explosion with the smoke, using the texture as the
			//	discriminator.

			CG32bitPixel rgbSmoke = m_pSmokeTable->GetAt(iNewRadius);
			CG32bitPixel rgbColor = CG32bitPixel::Composite(m_pExplosionTable->GetAt(iNewRadius), CG32bitPixel(rgbSmoke, CG32bitPixel::BlendAlpha(rgbSmoke.GetAlpha(), byAlpha)));
				
			//	Return the value (premultiplied)

			return CG32bitPixel::PreMult(rgbColor);
			}

	private:
		CFractalTextureLibrary::ETextureTypes m_iTexture;
		Metric m_rDistortion;
		const TArray<CG32bitPixel> *m_pExplosionTable;
		const TArray<CG32bitPixel> *m_pSmokeTable;

		//	Run time parameters for drawing a single frame.

		CSphericalTextureMapper m_Texture;
		CCircleRadiusDisruptor m_Disruptor;

		friend TCirclePainter32;
	};
