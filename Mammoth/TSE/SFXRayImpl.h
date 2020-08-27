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

class CRayEffectPainter : public IEffectPainter
{
public:
	CRayEffectPainter(CEffectCreator *pCreator);
	~CRayEffectPainter(void);

	//	IEffectPainter virtuals
	virtual CEffectCreator *GetCreator(void) override { return m_pCreator; }
	virtual int GetLifetime(void) override { return m_iLifetime; }
	virtual bool GetParam(const CString &sParam, CEffectParamDesc *retValue) const override;
	virtual bool GetParamList(TArray<CString> *retList) const override;
	virtual void GetRect(RECT *retRect) const override;
	virtual void Paint(CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
	virtual void PaintHit(CG32bitImage &Dest, int x, int y, const CVector &vHitPos, SViewportPaintCtx &Ctx) override;
	virtual void PaintLine(CG32bitImage &Dest, const CVector &vHead, const CVector &vTail, SViewportPaintCtx &Ctx) override;
	virtual bool PointInImage(int x, int y, int iTick, int iVariant = 0, int iRotation = 0) const override;

protected:
	virtual bool OnSetParam(CCreatePainterCtx &Ctx, const CString &sParam, const CEffectParamDesc &Value) override;

private:
	enum EAnimationTypes
	{
		animateNone = 0,

		animateFade = 1,
		animateFlicker = 2,
		animateCycle = 3,

		animateMax = 3,
	};

	enum ERayShapes
	{
		shapeUnknown = 0,

		shapeDiamond = 1,
		shapeOval = 2,
		shapeStraight = 3,
		shapeTapered = 4,
		shapeCone = 5,
		shapeSword = 6,

		shapeMax = 6,
	};

	enum ERayStyles
	{
		styleUnknown = 0,

		styleBlob = 1,
		styleGlow = 2,
		styleJagged = 3,
		styleGrainy = 4,
		styleLightning = 5,
		styleWhiptail = 6,

		styleMax = 6,
	};

	enum EColorTypes
	{
		colorNone,
		colorGlow,
	};

	enum EOpacityTypes
	{
		opacityNone,
		opacityGlow,
		opacityGrainy,
		opacityTaperedGlow,
		opacityTaperedExponentialGlow,
	};

	enum EWidthAdjTypes
	{
		widthAdjNone,
		widthAdjBlob,
		widthAdjDiamond,
		widthAdjJagged,
		widthAdjOval,
		widthAdjTapered,
		widthAdjCone,
		widthAdjWhiptail,
		widthAdjSword,
	};

	void CalcCone(TArray<Metric> &AdjArray);
	void CalcDiamond(TArray<Metric> &AdjArray);
	void CalcIntermediates(int iLength);
	int CalcLength(SViewportPaintCtx &Ctx) const;
	void CalcOval(TArray<Metric> &AdjArray);
	void CalcRandomWaves(TArray<Metric> &AdjArray, Metric rAmplitude, Metric rWavelength);
	void CalcSword(TArray<Metric> &AdjArray);
	void CalcTaper(TArray<Metric> &AdjArray);
	void CalcWaves(TArray<Metric> &AdjArray, Metric rAmplitude, Metric rWavelength, Metric rDecay, Metric rCyclePos);
	void CleanUpIntermediates(void);
	ILinePainter *CreateRenderer(int iWidth, int iLength, int iIntensity, ERayStyles iStyle, ERayShapes iShape, Metric rCyclePos = 0.0);
	void PaintRay(CG32bitImage &Dest, int xFrom, int yFrom, int xTo, int yTo, int iRotationDegrees, SViewportPaintCtx &Ctx);

	static void CalcTaperedGlow(int Width, int iLength, int iIntensity, Metric rTaperFraction, OpacityPlane &retGlow);
	static void CalcTaperedExponentialGlow(int Width, int iLength, int iIntensity, Metric rTaperFraction, OpacityPlane &retGlow);

	CEffectCreator *m_pCreator;

	int m_iLength;
	int m_iWidth;
	ERayShapes m_iShape;
	ERayStyles m_iStyle;
	int m_iIntensity;
	CG32bitPixel m_rgbPrimaryColor;
	CG32bitPixel m_rgbSecondaryColor;
	CGDraw::EBlendModes m_iBlendMode;

	int m_iXformRotation;

	int m_iLifetime;
	EAnimationTypes m_iAnimation;

	//	Temporary variables based on shape/style/etc.

	int m_iInitializedLength;			//	If -1, not yet initialized; otherwise, initialized to the given length
	TArray<ILinePainter *> m_RayRenderer;
	TArray<int> m_Length;               //  Length for each frame (only for multi-frame animations)

	template<typename T> friend class CRayRasterizer;
	friend class CLightningBundlePainter;
};


template <class BLENDER> class CRayRasterizer : public TLinePainter32<CRayRasterizer<BLENDER>, BLENDER>
    {
    public:
        CRayRasterizer (int iLengthCount, int iWidthCount, ColorPlane &ColorMap, OpacityPlane &OpacityMap, WidthAdjArray &WidthAdjTop, WidthAdjArray &WidthAdjBottom,
			CRayEffectPainter::EColorTypes iColorType, CRayEffectPainter::EOpacityTypes iOpacityType, CRayEffectPainter::EWidthAdjTypes iWidthAdjType, CRayEffectPainter::EWidthAdjTypes iReshape, CRayEffectPainter::EOpacityTypes iTexture,
			CG32bitPixel rgbPrimaryColor, CG32bitPixel rgbSecondaryColor, int iIntensity, Metric rCyclePos) :
                m_iLengthCount(iLengthCount),
                m_iWidthCount(iWidthCount),
                m_rgbColor(255, 255, 255),
                m_byOpacity(255),

				m_iColorType(iColorType),
				m_iOpacityType(iOpacityType),
				m_iWidthAdjType(iWidthAdjType),
				m_iReshape(iReshape),
				m_iTexture(iTexture),
				m_primaryColor(rgbPrimaryColor),
				m_secondaryColor(rgbSecondaryColor),
				m_iIntensity(iIntensity),
			    m_rCyclePos(rCyclePos)
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

		virtual void DrawWithOpenGL (CG32bitImage &Dest, int x1, int y1, int x2, int y2, int iRotDegrees, bool& bSuccess, OpenGLRenderLayer::blendMode blendMode) override
			{
			OpenGLMasterRenderQueue *pRenderQueue = Dest.GetMasterRenderQueue();
			if (!pRenderQueue)
				{
				bSuccess = false;
				return;
				}

			int iDistX = x1 - x2;
			int iDistY = y1 - y2;
			int iCanvasHeight = Dest.GetHeight();
			int iCanvasWidth = Dest.GetWidth();

			float iDist = sqrt(float(iDistX * iDistX) + float(iDistY * iDistY));
			int iPosX = x1 - ((iDistX) / 2);
			int iPosY = y1 - ((iDistY) / 2);
			std::tuple<int, int, int> primaryColor (int(m_primaryColor.GetRed()), int(m_primaryColor.GetGreen()), int(m_primaryColor.GetBlue()));
			std::tuple<int, int, int> secondaryColor (int(m_secondaryColor.GetRed()), int(m_secondaryColor.GetGreen()), int(m_secondaryColor.GetBlue()));

			pRenderQueue->addRayToEffectRenderQueue(iPosX, iPosY, int(iDist) * 2, m_iWidthCount * 2, iCanvasWidth, iCanvasHeight, float(iRotDegrees) * (float(PI) / 180.0f), m_iColorType, m_iOpacityType, m_iWidthAdjType, m_iReshape, m_iTexture,
				primaryColor, secondaryColor, m_iIntensity, float(m_rCyclePos), m_byOpacity, blendMode);

			bSuccess = true;
			return;


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
		
		CRayEffectPainter::EColorTypes m_iColorType;
		CRayEffectPainter::EOpacityTypes m_iOpacityType;
		CRayEffectPainter::EWidthAdjTypes m_iWidthAdjType;
		CRayEffectPainter::EWidthAdjTypes m_iReshape;
		CRayEffectPainter::EOpacityTypes m_iTexture;
		int m_iIntensity;
		Metric m_rCyclePos;

		CG32bitPixel m_primaryColor;
		CG32bitPixel m_secondaryColor;

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
        CLightningBundlePainter (int iBoltCount, CG32bitPixel rgbPrimaryColor, CG32bitPixel rgbSecondaryColor, WidthAdjArray &WidthAdjTop, WidthAdjArray &WidthAdjBottom, int iWidthCount,
			CRayEffectPainter::EWidthAdjTypes iWidthAdjType, CRayEffectPainter::EWidthAdjTypes iReshape);

        virtual void Draw (CG32bitImage &Dest, int x1, int y1, int x2, int y2, int iWidth) override;
		virtual void DrawWithOpenGL(CG32bitImage & Dest, int x1, int y1, int x2, int y2, int iRotDegrees, bool & bSuccess, OpenGLRenderLayer::blendMode blendMode) override;

    private:
        int m_iBoltCount;
        CG32bitPixel m_rgbPrimaryColor;
        CG32bitPixel m_rgbSecondaryColor;
        WidthAdjArray m_WidthAdjTop;
        WidthAdjArray m_WidthAdjBottom;
		int m_iWidthCount;
		CRayEffectPainter::EWidthAdjTypes m_iWidthAdjType;
		CRayEffectPainter::EWidthAdjTypes m_iReshape;
    };
