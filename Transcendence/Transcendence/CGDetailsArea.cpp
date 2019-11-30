//	CGDetailsArea.cpp
//
//	CGDetailsArea class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Transcendence.h"

CGDetailsArea::CGDetailsArea (const CVisualPalette &VI, const CDockScreenVisuals &Theme) : 
		m_VI(VI),
		m_Theme(Theme)

//	CGDetailsArea constructor

	{
	}

void CGDetailsArea::CleanUp (void)

//	CleanUp
//
//	Clean up area

	{
	m_pData.Delete();
	}

void CGDetailsArea::Paint (CG32bitImage &Dest, const RECT &rcRect)

//	Paint
//
//	Paint the detail.

	{
	DEBUG_TRY

	CDetailArea Painter(*g_pUniverse, m_VI, m_Theme);
	Painter.SetBackColor(m_rgbBackColor);
	Painter.SetColor(m_rgbTextColor);
	Painter.SetTabRegion(m_cyTabRegions);

	Painter.Paint(Dest, rcRect, m_pData);

	DEBUG_CATCH
	}

