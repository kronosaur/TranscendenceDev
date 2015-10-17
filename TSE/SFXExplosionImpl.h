//	SFXExplosionImpl.h
//
//	Classes to implement explosion effects
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved

#pragma once

class CExplosionCirclePainter : private TCirclePainter32<CExplosionCirclePainter>
	{
	public:
		CExplosionCirclePainter (void);

		void DrawFrame (CG32bitImage &Dest, int x, int y, int iRadius, Metric rDetail, const TArray<CG32bitPixel> &ColorTable);

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

			//	Get the alpha value at the texture position.

			BYTE byAlpha = (BYTE)(m_CloudTextures[m_iCurFrame].GetPixel(xTexture, yTexture));

			//	Look up the pixel in the color table

			CG32bitPixel rgbColor = (m_pColorTable ? m_pColorTable->GetAt(iRadius) : CG32bitPixel(0, 255, 255));

			//	Now combine the alpha.

			byAlpha = CG32bitPixel::BlendAlpha(rgbColor.GetAlpha(), byAlpha);

			//	Return the value (premultiplied)

			return CG32bitPixel::PreMult(CG32bitPixel(rgbColor, byAlpha));
			}

		static void Init (void);

	private:
		//	Run time parameters for drawing a single frame.

		int m_iCurFrame;
		const TArray<CG32bitPixel> *m_pColorTable;

		//	Static textures

		static TArray<CG8bitImage> m_CloudTextures;
	};

