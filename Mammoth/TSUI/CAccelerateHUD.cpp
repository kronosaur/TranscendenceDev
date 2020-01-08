//	CAccelerateHUD.cpp
//
//	CAccelerateHUD class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

void CAccelerateHUD::GetBounds (int *retWidth, int *retHeight) const

//	GetBounds
//
//	Return bounds for the HUD

	{
	*retWidth = DISPLAY_WIDTH;
	*retHeight = DISPLAY_HEIGHT;
	}

bool CAccelerateHUD::IsNewMode (CUniverse::EUpdateSpeeds iCurMode, CUniverse::EUpdateSpeeds iNewMode) const

//	IsNewMode
//
//	Returns TRUE if we should consider iNewMode to be different for purposes of
//	resetting the fade-out timer.

	{
	return (iCurMode != iNewMode);
	}

void CAccelerateHUD::OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx)

//	OnPaint
//
//	Paint the HUD

	{
	int xIcon = x + (DISPLAY_WIDTH - ICON_WIDTH) / 2;
	int yIcon = y + (DISPLAY_HEIGHT - ICON_HEIGHT) / 2;

	BYTE byOpacity;

	int iAge = Ctx.Universe.GetPaintTick() - m_iLastModeTick;
	if (iAge > FADE_START + FADE_TIME)
		return;
	else if (iAge > FADE_START)
		{
		Metric rFade = 1.0 - ((iAge - FADE_START) / (Metric)FADE_TIME);
		byOpacity = (BYTE)mathRound(rFade * Ctx.byOpacity);
		}
	else
		byOpacity = Ctx.byOpacity;

	switch (Ctx.Universe.GetLastUpdateSpeed())
		{
		case CUniverse::updateAccelerated:
			Dest.Blt(0, 0, ICON_WIDTH, ICON_HEIGHT, byOpacity, Ctx.VI.GetImage(imageFastForward), xIcon, yIcon);
			break;

		case CUniverse::updatePaused:
			Dest.Blt(0, 0, ICON_WIDTH, ICON_HEIGHT, byOpacity, Ctx.VI.GetImage(imagePause), xIcon, yIcon);
			break;

		case CUniverse::updateSingleFrame:
			Dest.Blt(0, 0, ICON_WIDTH, ICON_HEIGHT, byOpacity, Ctx.VI.GetImage(imagePlay), xIcon, yIcon);
			break;
		}
	}

void CAccelerateHUD::OnUpdate (SHUDUpdateCtx &Ctx)

//	OnUpdate
//
//	Update state.

	{
	//	See if we have a new mode

	CUniverse::EUpdateSpeeds iNewMode = Ctx.Universe.GetLastUpdateSpeed();
	if (IsNewMode(m_iLastMode, iNewMode))
		{
		m_iLastMode = iNewMode;
		m_iLastModeTick = Ctx.Universe.GetPaintTick();
		}
	}
