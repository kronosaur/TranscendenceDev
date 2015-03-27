//	CStargateEffectPainter.cpp
//
//	CStargateEffectPainter class
//	Copyright (c) 2015 by George Moromisato. All Rights Reserved.

#include "stdafx.h"

const CG32bitPixel RGB_BACKGROUND =			CG32bitPixel(0x19, 0x00, 0x40);
const CG32bitPixel RGB_BLOWN =				CG32bitPixel(0x4c, 0x00, 0xff);
const CG32bitPixel RGB_FRINGE =				CG32bitPixel(0x4c, 0x00, 0xff);
const CG32bitPixel RGB_HALO =				CG32bitPixel(0x4c, 0x00, 0xff);

const CG32bitPixel RGB_CORE =				CG32bitPixel(0xea, 0xbf, 0xff);
const CG32bitPixel RGB_MID =				CG32bitPixel(0xaa, 0x00, 0xff);
const CG32bitPixel RGB_EDGE =				CG32bitPixel(0x4c, 0x00, 0xff);

CStargateEffectPainter::CStargateEffectPainter (void) : 
		m_iInitialUpdates(60),
		m_iMaxTracerCount(1000),
		m_iMaxTracerLength(15),
		m_iNewTracerCount(25),
		m_byTracerOpacity(0x80),
		m_rMaxDepth(5.0),
		m_rRotationVel(0.01),
		m_rGlowAccel(0.05),

		m_Tracers(5000),
		m_rRotation(0.0),
		m_rGlowVel(0.0),
		m_rGlowRadius(1.0)

//	CStargateEffectPainter constructor

	{
	int i;

	for (i = 0; i < m_iInitialUpdates; i++)
		Update();
	}

void CStargateEffectPainter::InitGradientColorTable (int iRadius)

//	InitGradientColorTable
//
//	Initialize the color table used for the gradient

	{
	int i;

	m_GradientColorTable.DeleteAll();
	m_GradientColorTable.InsertEmpty(iRadius);

	//	Compute some temporaries

	int iFringeMaxRadius = iRadius * 2 / 100;
	int iFringeWidth = iFringeMaxRadius / 2;
	int iBlownRadius = iFringeMaxRadius - iFringeWidth;
	int iFadeWidth = iRadius - iFringeMaxRadius;

	//	Initialize table

	for (i = 0; i < iRadius; i++)
		{
		if (i < iBlownRadius)
			m_GradientColorTable[i] = RGB_BLOWN;

		else if (i < iFringeMaxRadius && iFringeWidth > 0)
			{
			int iStep = (i - iBlownRadius);
			DWORD dwOpacity = iStep * 255 / iFringeWidth;
			m_GradientColorTable[i] = CG32bitPixel::Blend(RGB_BLOWN, RGB_FRINGE, (BYTE)dwOpacity);
			}
		else if (iFadeWidth > 0)
			{
			int iStep = (i - iFringeMaxRadius);
			Metric rOpacity = 255.0 - (iStep * 255.0 / iFadeWidth);
			rOpacity = (rOpacity * rOpacity) / 255.0;
			m_GradientColorTable[i] = CG32bitPixel::Blend(RGB_BACKGROUND, RGB_HALO, (BYTE)(DWORD)rOpacity);
			}
		else
			m_GradientColorTable[i] = CG32bitPixel::Null();
		}
	}

void CStargateEffectPainter::InitTracer (STracer &Tracer)

//	InitTracer
//
//	Initialize random tracer

	{
	Tracer.Points.Init(m_iMaxTracerLength);

	SWormholePos &NewPoint = Tracer.Points.EnqueueAndOverwrite();
	NewPoint.rAngle = g_Pi * mathRandom(0, 359) / 180.0;
	NewPoint.rDepth = m_rMaxDepth;
	NewPoint.rRadius = mathRandom(100, 1000) / 1000.0;

	if (mathRandom(1, 100) <= 20)
		Tracer.Vel.rAngle = -0.0005 * mathRandom(1, 100);
	else
		Tracer.Vel.rAngle = 0.0005 * mathRandom(1, 100);

	Tracer.Vel.rDepth = -0.1 * mathRandom(1, 10);
	Tracer.Vel.rRadius = 0.0;

	Tracer.Color = CG32bitPixel::Blend(RGB_CORE, RGB_MID, mathRandom(0, 100) / 100.0);
	}

void CStargateEffectPainter::Paint (CG32bitImage &Dest, const RECT &rcRect)

//	Paint
//
//	Paint the current frame

	{
	int i;

	//	Some metrics

	SPaintCtx Ctx;
	Ctx.pDest = &Dest;
	Ctx.cxHalfWidth = (RectWidth(rcRect) / 2);
	Ctx.cyHalfHeight = (RectHeight(rcRect) / 2);
	Ctx.xCenter = rcRect.left + Ctx.cxHalfWidth;
	Ctx.yCenter = rcRect.top + Ctx.cyHalfHeight;

	//	Erase

	Dest.Fill(rcRect.left, rcRect.top, RectWidth(rcRect), RectHeight(rcRect), RGB_BACKGROUND);

	//	Paint the gradient

	int iRadius = Min(RectWidth(rcRect), RectHeight(rcRect)) / 2;
	if (iRadius != m_GradientColorTable.GetCount())
		InitGradientColorTable(iRadius);

	CGDraw::Circle(Dest, (int)Ctx.xCenter, (int)Ctx.yCenter, iRadius, m_GradientColorTable);

	//	Paint tracers

	for (i = 0; i < m_Tracers.GetCount(); i++)
		{
		STracer &Tracer = m_Tracers[i];
		PaintTracer(Ctx, Tracer);
		}

	//	Paint central glow

	CGDraw::CircleGradient(Dest, (int)Ctx.xCenter, (int)Ctx.yCenter, Min(iRadius, (int)m_rGlowRadius), RGB_CORE);
	}

void CStargateEffectPainter::PaintTracer (SPaintCtx &Ctx, const STracer &Tracer)

//	PaintTracer
//
//	Paints a tracer line

	{
	int i;

	if (Tracer.Points.GetCount() <= 1)
		return;

	//	Start with the head

	int xNext, yNext;
	TransformPos(Ctx, Tracer.Points[0], &xNext, &yNext);

	//	Paint all line segments

	for (i = 1; i < Tracer.Points.GetCount(); i++)
		{
		int xEnd, yEnd;
		TransformPos(Ctx, Tracer.Points[i], &xEnd, &yEnd);

		Metric rDecay = (i - 1) / (Metric)(Tracer.Points.GetCount() - 2);
//		Metric rDecay = (Tracer.Points.GetCount() - (i + 1)) / (Metric)(Tracer.Points.GetCount() - 2);
		int iWidth = Max(1, Min(2, (int)(4.0 * (1.0 - (Tracer.Points[i].rDepth / m_rMaxDepth)))));
		CG32bitPixel rgbColor = CG32bitPixel(CG32bitPixel::Blend(Tracer.Color, RGB_CORE, rDecay), (BYTE)(m_byTracerOpacity * rDecay));
		Ctx.pDest->DrawLine(xNext, yNext, xEnd, yEnd, iWidth, rgbColor);

		xNext = xEnd;
		yNext = yEnd;
		}
	}

void CStargateEffectPainter::TransformPos(SPaintCtx &Ctx, const SWormholePos &Pos, int *retx, int *rety)

//	TransformPos
//
//	Transforms from wormhole coordinates to screen coordinates

	{
	Metric rRadial = (Pos.rDepth > 0.0 ? Min(2000.0, (Ctx.cyHalfHeight * Pos.rRadius / Pos.rDepth)) : 2.0 * Ctx.cxHalfWidth);
	*retx = (int)(Ctx.xCenter + (rRadial * cos(m_rRotation + Pos.rAngle)));
	*rety = (int)(Ctx.yCenter - (rRadial * sin(m_rRotation + Pos.rAngle)));
	}

void CStargateEffectPainter::Update (void)

//	Update
//
//	Update one frame

	{
	int i;

	//	Create a new tracers

	if (m_Tracers.GetCount() < m_iMaxTracerCount)
		{
		for (i = 0; i < m_iNewTracerCount; i++)
			{
			STracer *pNewTracer = m_Tracers.Insert();
			InitTracer(*pNewTracer);
			}
		}

	//	Update new points for each tracer

	for (i = 0; i < m_Tracers.GetCount(); i++)
		{
		STracer &Tracer = m_Tracers[i];

		//	If we're off the screen, then reset

		if (Tracer.Points[0].rDepth <= 0.0)
			InitTracer(Tracer);

		//	Otherwise, move

		else
			{
			SWormholePos &NewPoint = Tracer.Points.EnqueueAndOverwrite();
			SWormholePos &PrevPoint = Tracer.Points[Tracer.Points.GetCount() - 2];
			NewPoint.rAngle = PrevPoint.rAngle + Tracer.Vel.rAngle;
			NewPoint.rDepth = PrevPoint.rDepth + Tracer.Vel.rDepth;
			NewPoint.rRadius = PrevPoint.rRadius + Tracer.Vel.rRadius;
			}
		}

	m_rRotation += m_rRotationVel;

	//	Glow

	m_rGlowVel += m_rGlowAccel;
	m_rGlowRadius += m_rGlowVel;
	}
