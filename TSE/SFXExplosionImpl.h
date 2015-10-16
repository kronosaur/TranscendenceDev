//	SFXExplosionImpl.h
//
//	Classes to implement explosion effects
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved

#pragma once

class CExplosionCirclePainter : public TCirclePainter32<CExplosionCirclePainter>
	{
	public:
		CExplosionCirclePainter (void);

		inline CG8bitImage &GetTextureMap (void) { return m_CloudTextures[m_iCurFrame]; }

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

			int xTexture = (int)(rLong * m_CloudTextures[m_iCurFrame].GetWidth());
			int yTexture = (int)(rLat * m_CloudTextures[m_iCurFrame].GetHeight());

			//	Compute the thickness of the sphere at this radius

			Metric rDensity = sin(rACosR);
			rDensity *= rDensity;

			//	Adjust for texture

			BYTE byAlpha = (BYTE)(rDensity * m_CloudTextures[m_iCurFrame].GetPixel(xTexture, yTexture));

			return CG32bitPixel::PreMult(CG32bitPixel(255, 255, 0, byAlpha));
			}

		inline void SetFrame (int iFrame)
			{
			if (m_CloudTextures.GetCount() > 0)
				m_iCurFrame = (iFrame % m_CloudTextures.GetCount());
			}


	private:
		int m_iCurFrame;

		static TArray<CG8bitImage> m_CloudTextures;
	};

