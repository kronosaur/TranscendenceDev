//	TLinePainter.h
//
//	TLinePainter template
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CLinePainter;

class ILinePainter
	{
	public:
		virtual ~ILinePainter (void) { }

		virtual void Draw (CG32bitImage &Dest, int x1, int y1, int x2, int y2, int iWidth) = 0;
		virtual void SetParam (const CString &sParam, BYTE byValue) { }
		virtual void SetParam (const CString &sParam, const TArray<CG32bitPixel> &ColorTable) { }
	};

template <class PAINTER, class BLENDER> class TLinePainter32 : public ILinePainter
    {
    public:
        virtual void Draw (CG32bitImage &Dest, int x1, int y1, int x2, int y2, int iWidth) override
            {
	        int x, y;

	        //	Calculate the line type and paint accordingly

            CLinePainter Rasterizer;
	        switch (Rasterizer.CalcIntermediates(Dest, x1, y1, x2, y2, iWidth))
		        {
                case CLinePainter::lineNull:
			        break;

                case CLinePainter::linePoint:
                    DRAW_PIXEL(Dest.GetPixelPos(x1, y1), GET_PIXEL(0.0, 0.0));
			        break;

                case CLinePainter::lineXDominant:
                    {
			        Metric rV, rW;
			        Rasterizer.CalcPixelMappingX(x1, y1, x2, y2, &rV, &rW);

                    int xEnd, xInc;
                    Rasterizer.CalcLoopX(&x, &y, &xEnd, &xInc);

			        for (; x != xEnd; x += xInc)
				        {
				        int w = y;
				        int wLast = w + Rasterizer.GetAxisWidth() + 1;
				        CG32bitPixel *pPos = Dest.GetPixelPos(x, w);

				        Metric rInnerV = rV;
				        Metric rInnerW = rW;

				        //	Draw anti-aliasing above the line

                        Metric rWUp;
                        if (Rasterizer.GetEdgeUp(w, rWUp))
                            DRAW_PIXEL(pPos, GET_EDGE_PIXEL(rWUp, rInnerV, rInnerW));

				        w++;
				        pPos += Rasterizer.GetPosRowInc();
                        Rasterizer.NextVWX(rInnerV, rInnerW);

				        //	Draw the solid part of the line

				        for (; w < wLast; w++, pPos += Rasterizer.GetPosRowInc())
					        {
                            if (Rasterizer.InSolid(w))
                                DRAW_PIXEL(pPos, GET_PIXEL(rInnerV, rInnerW));

                            Rasterizer.NextVWX(rInnerV, rInnerW);
					        }

				        //	Draw anti-aliasing below the line

                        Metric rWDown;
                        if (Rasterizer.GetEdgeDown(w, rWDown))
                            DRAW_PIXEL(pPos, GET_EDGE_PIXEL(rWDown, rInnerV, rInnerW));

				        //	Next point

				        Rasterizer.NextX(x, y, rV, rW);
				        }
			        break;
                    }

                case CLinePainter::lineYDominant:
                    {
                    Metric rV, rW;
                    Rasterizer.CalcPixelMappingY(x1, y1, x2, y2, &rV, &rW);

                    int yEnd, yInc;
                    Rasterizer.CalcLoopY(&x, &y, &yEnd, &yInc);

                    for (; y != yEnd; y += yInc)
                        {
                        int w = x;
                        int wLast = w + Rasterizer.GetAxisWidth() + 1;
                        CG32bitPixel *pPos = Dest.GetPixelPos(w, y);

				        Metric rInnerV = rV;
				        Metric rInnerW = rW;

                        //	Draw anti-aliasing to the left

                        Metric rWUp;
                        if (Rasterizer.GetEdgeUp(w, rWUp))
                            DRAW_PIXEL(pPos, GET_EDGE_PIXEL(rWUp, rInnerV, rInnerW));

                        w++;
                        pPos++;
                        Rasterizer.NextVWY(rInnerV, rInnerW);

                        //	Draw the solid part of the line

                        for (; w < wLast; w++, pPos++)
                            {
                            if (Rasterizer.InSolid(w))
                                DRAW_PIXEL(pPos, GET_PIXEL(rInnerV, rInnerW));

                            Rasterizer.NextVWY(rInnerV, rInnerW);
                            }

                        //	Draw anti-aliasing to the right of the line

                        Metric rWDown;
                        if (Rasterizer.GetEdgeDown(w, rWDown))
                            DRAW_PIXEL(pPos, GET_EDGE_PIXEL(rWDown, rInnerV, rInnerW));

                        //	Next point

                        Rasterizer.NextY(x, y, rV, rW);
                        }
                    break;
                    }
		        }
            }

    protected:
        inline void DRAW_PIXEL (CG32bitPixel *pPos, CG32bitPixel rgbColor)
            {
            if (rgbColor.GetAlpha() == 0)
                ;
            else if (rgbColor.GetAlpha() == 0xff)
                BLENDER::SetCopy(pPos, rgbColor);
            else
                BLENDER::SetBlendPreMult(pPos, rgbColor);
            }

        inline CG32bitPixel GET_EDGE_PIXEL (Metric rEdge, Metric rV, Metric rW) { return ((PAINTER *)this)->GetEdgePixel(rEdge, rV, rW); }
        inline CG32bitPixel GET_PIXEL (Metric rV, Metric rW) { return ((PAINTER *)this)->GetPixel(rV, rW); }

        //  Default implementations

        inline CG32bitPixel GetEdgePixel (Metric rEdge, Metric rV, Metric rW)
            {
            CG32bitPixel rgbColor = GET_PIXEL(rV, rW);
            BYTE byOpacity = (BYTE)(DWORD)(255.0 * (rEdge > 1.0 ? 1.0 : rEdge));
            if (rgbColor.GetAlpha() == 0xff)
                return CG32bitPixel::PreMult(rgbColor, byOpacity);
            else
                return CG32bitPixel(CG32bitPixel::PreMult(rgbColor, byOpacity), CG32bitPixel::BlendAlpha(rgbColor.GetAlpha(), byOpacity));
            }
    };

template <class BLENDER> class TLinePainterSolid : public TLinePainter32<TLinePainterSolid<BLENDER>, BLENDER>
    {
    public:
        TLinePainterSolid (CG32bitPixel rgbColor)
            {
            if (rgbColor.GetAlpha() == 0xff)
                m_rgbColor = rgbColor;
            else
                m_rgbColor = CG32bitPixel::PreMult(rgbColor);
            }

    private:
        inline CG32bitPixel GetPixel (Metric rV, Metric rW) const { return m_rgbColor; }

        CG32bitPixel m_rgbColor;

    friend TLinePainter32;
    };

class CLinePainter
	{
	public:
		void DrawSolid (CG32bitImage &Image, int x1, int y1, int x2, int y2, int iWidth, CG32bitPixel rgbColor);

    public:
        //  This functions are used by TLinePainter32

		enum ESlopeTypes
			{
			lineNull,						//	No line
			linePoint,						//	Single point
			lineXDominant,
			lineYDominant,
			};

		ESlopeTypes CalcIntermediates (const CG32bitImage &Image, int x1, int y1, int x2, int y2, int iWidth);
        void CalcLoopX (int *retx, int *rety, int *retiEnd, int *retiInc);
        void CalcLoopY (int *retx, int *rety, int *retiEnd, int *retiInc);
		void CalcPixelMapping (int x1, int y1, int x2, int y2, double *retrV, double *retrW);

        inline void CalcPixelMappingX (int x1, int y1, int x2, int y2, double *retrV, double *retrW)
            {
			CalcPixelMapping(x1, y1, x2, y2, retrV, retrW);

			*retrV += (-m_iAxisHalfWidth - 1) * m_rVPerY;
			*retrW += (-m_iAxisHalfWidth - 1) * m_rWPerY;
            }

        inline void CalcPixelMappingY (int x1, int y1, int x2, int y2, double *retrV, double *retrW)
            {
			CalcPixelMapping(x1, y1, x2, y2, retrV, retrW);

			*retrV += (-m_iAxisHalfWidth - 1) * m_rVPerX;
			*retrW += (-m_iAxisHalfWidth - 1) * m_rWPerX;
            }

        inline int GetAxisWidth (void) const { return m_iAxisWidth; }

        inline bool GetEdgeDown (int w, Metric &rWDown) const
            {
            if (m_rWDown > 0.0 && w >= m_wMin && w < m_wMax)
                {
                rWDown = m_rWDown;
                return true;
                }
            else
                return false;
            }

        inline bool GetEdgeUp (int w, Metric &rWUp) const
            {
            if (m_rWUp > 0.0 && w >= m_wMin && w < m_wMax)
                {
                rWUp = m_rWUp;
                return true;
                }
            else
                return false;
            }

        inline int GetPosRowInc (void) const { return m_iPosRowInc; }

        inline bool InSolid (int w) const { return (w >= m_wMin && w < m_wMax); }

        inline void NextVWX (Metric &rV, Metric &rW) const
            {
            rV += m_rVPerY;
            rW += m_rWPerY;
            }

        inline void NextVWY (Metric &rV, Metric &rW) const
            {
            rV += m_rVPerX;
            rW += m_rWPerX;
            }

		inline void NextX (int &x, int &y)
			{
			if (m_d >= 0)
				{
				y = y + m_sy;
				m_d = m_d - m_ax;
				m_rWUp += m_rWUpInc;
				m_rWDown -= m_rWDownDec;
				}

			m_d = m_d + m_ay;
			m_rWUp -= m_rWUpDec;
			m_rWDown += m_rWDownInc;
			}

		inline void NextY (int &x, int &y)
			{
			if (m_d >= 0)
				{
				x = x + m_sx;
				m_d = m_d - m_ay;
				m_rWUp += m_rWUpInc;
				m_rWDown -= m_rWDownDec;
				}

			m_d = m_d + m_ax;
			m_rWUp -= m_rWUpDec;
			m_rWDown += m_rWDownInc;
			}

		inline void NextX (int &x, int &y, double &rV, double &rW)
			{
			if (m_d >= 0)
				{
				y = y + m_sy;
				m_d = m_d - m_ax;
				m_rWUp += m_rWUpInc;
				m_rWDown -= m_rWDownDec;

				rV += m_rVIncY;
				rW += m_rWIncY;
				}

			m_d = m_d + m_ay;
			m_rWUp -= m_rWUpDec;
			m_rWDown += m_rWDownInc;

			rV += m_rVIncX;
			rW += m_rWIncX;
			}

		inline void NextY (int &x, int &y, double &rV, double &rW)
			{
			if (m_d >= 0)
				{
				x = x + m_sx;
				m_d = m_d - m_ay;
				m_rWUp += m_rWUpInc;
				m_rWDown -= m_rWDownDec;

				rV += m_rVIncX;
				rW += m_rWIncX;
				}

			m_d = m_d + m_ax;
			m_rWUp -= m_rWUpDec;
			m_rWDown += m_rWDownInc;

			rV += m_rVIncY;
			rW += m_rWIncY;
			}

	private:
		int m_dx;							//	Distance from x1 to x2
		int m_sx;							//	Direction (from x1 to x2)
		int m_ax;							//	absolute distance x (times 2)
		double m_rL;						//	Length of line
		int m_xStart;
		int m_xEnd;

		int m_dy;							//	Distance from y1 to y2
		int m_sy;							//	Direction (from y1 to y2)
		int m_ay;							//	absolute distance y (times 2)
		int m_yStart;
		int m_yEnd;

		int m_d;							//	Discriminator

		double m_rHalfWidth;				//	Half of width;
		int m_iAxisHalfWidth;				//	Half line width aligned on dominant axis
		int m_iAxisWidth;					//	Full width of line aligned on dominant axis

		double m_rWDown;					//	Intermediates for line width
		double m_rWDownDec;
		double m_rWDownInc;
		double m_rWUp;
		double m_rWUpDec;
		double m_rWUpInc;
		int m_wMin;
		int m_wMax;

		int m_iPosRowInc;					//	Image row offset

		double m_rVPerX;					//	Movement along V (line axis) for every X movement
		double m_rVPerY;					//	Movement along V for every Y movement
		double m_rWPerX;					//	Movement along W (perp axis) for every X movement
		double m_rWPerY;					//	Movement along W for every Y movement

		double m_rVIncX;
		double m_rVIncY;
		double m_rWIncX;
		double m_rWIncY;
	};

