//	CTimersHUD.cpp
//
//	CTimersHUD class
//	Copyright (c) 2021 Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

void CTimersHUD::GetBounds (int *retWidth, int *retHeight) const

//	GetBounds
//
//	Return bounds for the HUD

	{
	*retWidth = DISPLAY_WIDTH;
	*retHeight = DISPLAY_HEIGHT;
	}

void CTimersHUD::OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx)

//	OnPaint
//
//	Paint the HUD

	{
	if (m_bInvalid)
		{
		Realize(Ctx);
		m_bInvalid = false;
		}

	Dest.Blt(0,
			0,
			m_Buffer.GetWidth(),
			m_Buffer.GetHeight(),
			Ctx.byOpacity,
			m_Buffer,
			x,
			y + DISPLAY_HEIGHT - m_Buffer.GetHeight());
	}

void CTimersHUD::OnUpdate (SHUDUpdateCtx &Ctx)

//	OnUpdate
//
//	Update state.

	{
	Invalidate();
	}

void CTimersHUD::PaintTimer (SHUDPaintCtx &Ctx, CG32bitImage &Dest, SHUDTimerDesc &Timer, int x, int y)

//	PaintTimer
//
//	Paints the timer at the given position.

	{
	const CG16bitFont &MediumFont = Ctx.VI.GetFont(fontMedium);

	int xPos = x;
	int yPos = y;
	if (Timer.pIcon && !Timer.pIcon->IsEmpty())
		{
		Timer.pIcon->PaintScaledImage(Dest, xPos, yPos, 0, 0, ICON_WIDTH, ICON_HEIGHT, CObjectImageArray::FLAG_UPPER_LEFT);
		xPos += ICON_WIDTH + SPACING_X;
		}

	int cyBar = (TIMER_HEIGHT - MediumFont.GetHeight()) - BAR_SPACING_Y;
	int cxBar = (x + TIMER_WIDTH) - xPos;
	int xBarPos = Min(cxBar, cxBar * Timer.iBar / 100);

	CG32bitPixel rgbLabel = Ctx.VI.GetColor(colorTextNormal);
	CG32bitPixel rgbBar;
	CG32bitPixel rgbBarBackground;
	switch (Timer.iStyle)
		{
		case EHUDTimerStyle::Default:
			rgbBar = CG32bitPixel(0, 255, 0);
			rgbBarBackground = CG32bitPixel(64, 64, 64);
			break;

		case EHUDTimerStyle::Warning:
			rgbBar = CG32bitPixel(255, 255, 0);
			rgbBarBackground = CG32bitPixel(64, 64, 64);
			break;

		case EHUDTimerStyle::Danger:
			rgbBar = CG32bitPixel(255, 0, 0);
			rgbBarBackground = CG32bitPixel(64, 0, 0);
			break;

		default:
			throw CException(ERR_FAIL);
		}

	//	Paint the bar

	Dest.Fill(xPos, yPos, xBarPos, cyBar, rgbBar);
	Dest.Fill(xPos + xBarPos, yPos, cxBar - xBarPos, cyBar, rgbBarBackground);
	yPos += cyBar + BAR_SPACING_Y;

	//	Label

	MediumFont.DrawText(Dest, xPos, yPos, rgbLabel, Timer.sLabel, 0);
	}

void CTimersHUD::Realize (SHUDPaintCtx &Ctx)

//	Realize
//
//	Paint the buffer.

	{
	TArray<SHUDTimerDesc> Timers;
	Ctx.Source.GetHUDTimers(Timers);

	//	Calculate the size of the buffer.

	if (m_iTimerCount != Timers.GetCount())
		{
		m_iTimerCount = Timers.GetCount();

		if (m_iTimerCount == 0)
			m_Buffer.CleanUp();
		else
			m_Buffer.Create(DISPLAY_WIDTH, m_iTimerCount * (TIMER_HEIGHT + SPACING_Y), CG32bitImage::alpha8);
		}

	//	If no timers, then we're done.

	if (m_iTimerCount == 0)
		return;

	//	Paint all timers

	m_Buffer.Set(CG32bitPixel::Null());

	int y = 0;
	for (int i = 0; i < Timers.GetCount(); i++)
		{
		PaintTimer(Ctx, m_Buffer, Timers[i], 0, y);

		y += TIMER_HEIGHT + SPACING_Y;
		}
	}
