//	SFXFractalImpl.h
//
//	Classes to implement explosion effects
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved

#pragma once

//	CCloudCirclePainter
//
//	We paint a spherical fractal cloud with a color table keyed to radius.
//	rFrame is from 0.0 to 1.0. Use (Metric){frame} / {frameCount}.

class CCloudCirclePainter : public TCirclePainter32<CCloudCirclePainter>
	{
	public:
		CCloudCirclePainter (CFractalTextureLibrary::ETextureTypes iTexture, const TArray<CG32bitPixel> &ColorTable) :
				m_iTexture(iTexture),
				m_pColorTable(&ColorTable),
				m_pTexture(NULL)
			{
			}

	private:
		bool BeginDraw (void)
			{
			//	We need enough angular resolution to reach the pixel level (but
			//	no more).

			m_iAngleRange = (int)(m_iRadius * 2.0 * g_Pi);

			//	Set the texture based on the frame

			m_pTexture = &g_pUniverse->GetFractalTextureLibrary().GetTexture(m_iTexture, m_iFrame);

			//	Success

			return true;
			}

		void EndDraw (void)
			{
			m_pTexture = NULL;
			}

		inline CG32bitPixel GetColorAt (int iAngle, int iRadius) const 

		//	GetColorAt
		//
		//	Returns the color at the given position in the circle. NOTE: We must
		//	return a pre-multiplied pixel.

			{
			Metric rRadius = (Metric)iRadius / m_iRadius;
			Metric rACosR = acos(rRadius);

			//	Map this point to an offset into a sphere map.
			//
			//	Lat goes from 0.0 (North Pole) to 1.0 (South Pole)
			//	Long goes from 0.0 to 1.0 (around the circumference)

			Metric rLat = 0.5 - (rACosR / g_Pi);
			Metric rLong = (Metric)iAngle / m_iAngleRange;

			//	Lookup in the cloud texture coordinates

			int xTexture = (int)(rLong * m_pTexture->GetWidth());
			int yTexture = (int)(rLat * m_pTexture->GetHeight());

			//	Get the alpha value at the texture position.

			BYTE byAlpha = (BYTE)(m_pTexture->GetPixel(xTexture, yTexture));

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

		const CG8bitImage *m_pTexture;

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
		CFireballCirclePainter (CFractalTextureLibrary::ETextureTypes iTexture, const TArray<CG32bitPixel> &ExplosionTable, const TArray<CG32bitPixel> &SmokeTable) :
				m_iTexture(iTexture),
				m_pExplosionTable(&ExplosionTable),
				m_pSmokeTable(&SmokeTable),
				m_pTexture(NULL)
			{
			ASSERT(ExplosionTable.GetCount() == SmokeTable.GetCount());
			}

	private:
		bool BeginDraw (void)
			{
			//	We need enough angular resolution to reach the pixel level (but
			//	no more).

			m_iAngleRange = (int)(m_iRadius * 2.0 * g_Pi);

			//	Set the texture based on the frame

			m_pTexture = &g_pUniverse->GetFractalTextureLibrary().GetTexture(m_iTexture, m_iFrame);

			//	Success

			return true;
			}

		void EndDraw (void)
			{
			m_pTexture = NULL;
			}

		inline CG32bitPixel GetColorAt (int iAngle, int iRadius) const 

		//	GetColorAt
		//
		//	Returns the color at the given position in the circle. NOTE: We must
		//	return a pre-multiplied pixel.

			{
			Metric rRadius = (Metric)iRadius / m_iRadius;
			Metric rACosR = acos(rRadius);

			//	Map this point to an offset into a sphere map.
			//
			//	Lat goes from 0.0 (North Pole) to 1.0 (South Pole)
			//	Long goes from 0.0 to 1.0 (around the circumference)

			Metric rLat = 0.5 - (rACosR / g_Pi);
			Metric rLong = (Metric)iAngle / m_iAngleRange;

			//	Lookup in the cloud texture coordinates

			int xTexture = (int)(rLong * m_pTexture->GetWidth());
			int yTexture = (int)(rLat * m_pTexture->GetHeight());

			//	Get the alpha value at the texture position.

			BYTE byAlpha = (BYTE)(m_pTexture->GetPixel(xTexture, yTexture));

			//	Combine the explosion with the smoke, using the texture as the
			//	discriminator.

			CG32bitPixel rgbSmoke = m_pSmokeTable->GetAt(iRadius);
			CG32bitPixel rgbColor = CG32bitPixel::Composite(m_pExplosionTable->GetAt(iRadius), CG32bitPixel(rgbSmoke, CG32bitPixel::BlendAlpha(rgbSmoke.GetAlpha(), byAlpha)));
				
			//	Return the value (premultiplied)

			return CG32bitPixel::PreMult(rgbColor);
			}

	private:
		CFractalTextureLibrary::ETextureTypes m_iTexture;
		const TArray<CG32bitPixel> *m_pExplosionTable;
		const TArray<CG32bitPixel> *m_pSmokeTable;

		//	Run time parameters for drawing a single frame.

		const CG8bitImage *m_pTexture;

		friend TCirclePainter32;
	};
