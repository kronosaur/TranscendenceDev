//	CLRSHUD.cpp
//
//	CLRSHUD class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

void CLRSHUD::GetBounds (int *retWidth, int *retHeight) const

//	GetBounds
//
//	Return bounds for the HUD

	{
	*retWidth = m_iDiameter;
	*retHeight = m_iDiameter;
	}

bool CLRSHUD::OnCreate (SHUDCreateCtx &CreateCtx, CString *retsError)

//	OnCreate
//
//	Initialize the HUD object.

	{
	m_iDiameter = Max(MIN_LRS_SIZE, LRS_SCALE * RectHeight(CreateCtx.rcScreen) / 100);

	//	Create a buffer

	if (!m_Buffer.Create(m_iDiameter, m_iDiameter, CG32bitImage::alpha8))
		return ComposeLoadError(CONSTLIT("Memory error"), retsError);

	//	Create a circular mask for the LRS

	if (!m_Mask.Create(m_iDiameter, m_iDiameter, 0xff))
		return ComposeLoadError(CONSTLIT("Memory error"), retsError);

	CGDraw::Circle(m_Mask, (m_iDiameter / 2), (m_iDiameter / 2), (m_iDiameter / 2), 0x00);

	//	Load some images

	m_pSnow = CreateCtx.Universe.GetLibraryBitmap(UNID_SRS_SNOW_PATTERN, CDesignCollection::FLAG_IMAGE_LOCK);

	return true;
	}

void CLRSHUD::OnPaint (CG32bitImage &Dest, int x, int y, SHUDPaintCtx &Ctx)

//	OnPaint
//
//	Paint

	{
	Dest.Blt(0,
			0,
			m_iDiameter,
			m_iDiameter,
			200,
			m_Buffer,
			x,
			y);
	}

void CLRSHUD::OnUpdate (SHUDUpdateCtx &Ctx)

//	OnUpdate
//
//	Update once per tick.

	{
	if (m_bInvalid || (Ctx.iTick % LRS_UPDATE) == 0)
		{
		Realize(Ctx);
		m_bInvalid = false;
		}
	}

void CLRSHUD::Realize (SHUDUpdateCtx &Ctx)

//	Realize
//
//	Paint our off-screen buffer.

	{
	DEBUG_TRY

	bool bNewEnemies;
	int cxWidth = m_iDiameter;
	int cyHeight = m_iDiameter;

	if (m_pBackground)
		CGDraw::BltTiled(m_Buffer, 0, 0, cxWidth, cyHeight, *m_pBackground);
	else
		m_Buffer.Fill(m_rgbBackground);

	//	If we're not blind, paint the LRS

	if (!Ctx.Source.IsLRSBlind())
		{
		RECT rcView;
		rcView.left = 0;
		rcView.top = 0;
		rcView.right = cxWidth;
		rcView.bottom = cyHeight;

		Metric rKlicksPerPixel = g_LRSRange * 2 / RectWidth(rcView);
		Ctx.Universe.PaintPOVLRS(m_Buffer, rcView, rKlicksPerPixel, 0, &bNewEnemies);

		//	Notify player of enemies

		if (bNewEnemies)
			{
			Ctx.Source.SendMessage(NULL, CONSTLIT("Enemy ships detected"));
			Ctx.Universe.PlaySound(NULL, Ctx.Universe.FindSound(UNID_DEFAULT_ENEMY_SHIP_ALARM));
			}
		}

	//	If we're blind, paint snow

	else
		{
		if (m_pSnow)
			CGDraw::BltTiled(m_Buffer, 0, 0, cxWidth, cyHeight, *m_pSnow);

		int iCount = mathRandom(1, 8);
		for (int i = 0; i < iCount; i++)
			{
			m_Buffer.Fill(0, mathRandom(0, cyHeight),
					cxWidth,
					mathRandom(1, 20),
					CG32bitPixel(108, 252, 128));
			}
		}

	//	Mask out the border

	m_Buffer.SetMask(0,
			0,
			cxWidth,
			cyHeight,
			m_Mask,
			CG32bitPixel::Null(),
			0,
			0);

	DEBUG_CATCH
	}
