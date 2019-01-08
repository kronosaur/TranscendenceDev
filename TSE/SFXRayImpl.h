//	SFXRayImpl.h
//
//	Classes to implement various ray/line effects
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved

#pragma once

//  Grayscale value at each point on the 2D ray [length][width]

typedef TArray<BYTE> OpacityArray;
typedef TArray<OpacityArray> OpacityPlane;

//  Color of each point on the 2D ray [length][width]

typedef TArray<CG32bitPixel> ColorArray;
typedef TArray<ColorArray> ColorPlane;

//  Adjustment to width at each point along the length of the ray

typedef TArray<Metric> WidthAdjArray;

template <class BLENDER> class CRayRasterizer : public TLinePainter32<CRayRasterizer<BLENDER>, BLENDER>
    {
    public:
        CRayRasterizer (int iLengthCount, int iWidthCount, ColorPlane &ColorMap, OpacityPlane &OpacityMap, WidthAdjArray &WidthAdjTop, WidthAdjArray &WidthAdjBottom) :
                m_iLengthCount(iLengthCount),
                m_iWidthCount(iWidthCount),
                m_rgbColor(255, 255, 255),
                m_byOpacity(255)
            {
            m_ColorMap.TakeHandoff(ColorMap);
            m_OpacityMap.TakeHandoff(OpacityMap);
            m_WidthAdjTop.TakeHandoff(WidthAdjTop);
            m_WidthAdjBottom.TakeHandoff(WidthAdjBottom);
            }

        virtual void SetParam (const CString &sParam, BYTE byValue) override
            {
            if (strEquals(sParam, CONSTLIT("opacity")))
                m_byOpacity = byValue;
            }

    private:
        inline CG32bitPixel GetPixel (Metric rV, Metric rW)
            {
		    //	Scale v and w to map to our array sizes

		    int v = (int)(m_iLengthCount * rV);

		    //	If necessary, adjust the width of the ray

		    if (m_WidthAdjTop.GetCount() > 0)
			    {
			    //	If this pixel is outside the width adjustment, then we skip it

                if (v < 0 || v >= m_iLengthCount)
                    return CG32bitPixel::Null();

			    //	Positive values are above

			    else if (rW > 0.0)
				    {
				    if (rW > m_WidthAdjTop[v])
					    return CG32bitPixel::Null();

				    rW = (m_WidthAdjTop[v] > 0.0 ? rW / m_WidthAdjTop[v] : 0.0);
				    }

			    //	Otherwise, we use bottom adjustment

			    else
				    {
				    if (-rW > m_WidthAdjBottom[v])
					    return CG32bitPixel::Null();

				    rW = (m_WidthAdjBottom[v] > 0.0 ? rW / m_WidthAdjBottom[v] : 0.0);
				    }
			    }
		    else
			    v = Min(Max(0, v), m_iLengthCount - 1);

		    //	Width adjustment touches Pixel.rW, so we need to do this afterwards.

		    Metric rWPixels = (m_iWidthCount * Absolute(rW));
		    int w = (int)rWPixels;
		    if (w < 0 || w >= m_iWidthCount)
			    return CG32bitPixel::Null();

		    //	Compute color

		    CG32bitPixel rgbColor;
		    if (m_ColorMap.GetCount() == 1)
			    {
			    rgbColor = m_ColorMap[0][w];
			    if (w > 0)
				    {
				    CG32bitPixel rgbAAColor = m_ColorMap[0][w - 1];
				    rgbColor = CG32bitPixel::Composite(rgbAAColor, rgbColor, rWPixels - (Metric)w);
				    }
			    }

            else if (m_ColorMap.GetCount() > 1)
                {
			    rgbColor = m_ColorMap[v][w];
                if (w > 0)
                    {
				    CG32bitPixel rgbAAColor = m_ColorMap[v][w - 1];
				    rgbColor = CG32bitPixel::Composite(rgbAAColor, rgbColor, rWPixels - (Metric)w);
                    }
                }

		    else
			    rgbColor = m_rgbColor;

		    //	Apply opacity, if necessary

            if (m_byOpacity != 255)
                rgbColor = CG32bitPixel::Fade(rgbColor, m_byOpacity);

		    //	Draw

            return rgbColor;
            }

		int m_iLengthCount;					//	Count of cells along ray length
		int m_iWidthCount;					//	Count of cells from ray axis out to edge
        CG32bitPixel m_rgbColor;            //  Solid color only
        BYTE m_byOpacity;                   //  Opacity
		ColorPlane m_ColorMap;		        //	Full color map
		OpacityPlane m_OpacityMap;	        //	Full opacity map
		WidthAdjArray m_WidthAdjTop;	    //	Top width adjustment
		WidthAdjArray m_WidthAdjBottom;	    //	Bottom width adjustment

        friend TLinePainter32;
    };

template <class BLENDER> class CFlareRayRasterizer : public TLinePainter32<CFlareRayRasterizer<BLENDER>, BLENDER>
    {
    private:
        const Metric BLOOM_FACTOR =				1.2;

        inline CG32bitPixel GetEdgePixel (Metric rEdge, Metric rV, Metric rW)
            {
            if (rW < -1.0 || rW > 1.0 || rV < 0.0 || rV > 1.0)
                return CG32bitPixel::Null();

			Metric rDist = (1.0 - (2.0 * Absolute(rV - 0.5)));
			Metric rSpread = (1.0 - Absolute(rW));
			Metric rValue = BLOOM_FACTOR * (rDist * rDist) * (rSpread * rSpread);

            Metric rEdgeOpacity = (255.0 * (rEdge > 1.0 ? 1.0 : rEdge));
			BYTE byOpacity = (BYTE)(rValue >= 1.0 ? rEdgeOpacity : rValue * rEdgeOpacity);

			return CG32bitPixel(byOpacity, byOpacity, byOpacity, byOpacity);
            }

        inline CG32bitPixel GetPixel (Metric rV, Metric rW)
            {
            if (rW < -1.0 || rW > 1.0 || rV < 0.0 || rV > 1.0)
                return CG32bitPixel::Null();

			Metric rDist = (1.0 - (2.0 * Absolute(rV - 0.5)));
			Metric rSpread = (1.0 - Absolute(rW));
			Metric rValue = BLOOM_FACTOR * (rDist * rDist) * (rSpread * rSpread);

			BYTE byOpacity = (rValue >= 1.0 ? 255 : (BYTE)(rValue * 255));

			return CG32bitPixel(byOpacity, byOpacity, byOpacity, byOpacity);
            }

        friend TLinePainter32;
    };

class CLightningBundlePainter : public ILinePainter
    {
    public:
        CLightningBundlePainter (int iBoltCount, CG32bitPixel rgbPrimaryColor, CG32bitPixel rgbSecondaryColor, WidthAdjArray &WidthAdjTop, WidthAdjArray &WidthAdjBottom);

        virtual void Draw (CG32bitImage &Dest, int x1, int y1, int x2, int y2, int iWidth) override;

    private:
        int m_iBoltCount;
        CG32bitPixel m_rgbPrimaryColor;
        CG32bitPixel m_rgbSecondaryColor;
        WidthAdjArray m_WidthAdjTop;
        WidthAdjArray m_WidthAdjBottom;
    };