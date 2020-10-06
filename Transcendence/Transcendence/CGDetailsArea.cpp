//	CGDetailsArea.cpp
//
//	CGDetailsArea class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Transcendence.h"

CGDetailsArea::CGDetailsArea (const CVisualPalette &VI, const CDockScreenVisuals &Theme) : 
		m_VI(VI),
		m_Theme(Theme),
		m_Painter(*g_pUniverse, VI, Theme)

//	CGDetailsArea constructor

	{
	}

void CGDetailsArea::Paint (CG32bitImage &Dest, const RECT &rcRect)

//	Paint
//
//	Paint the detail.

	{
	DEBUG_TRY

	m_Painter.Paint(Dest, rcRect);

	DEBUG_CATCH
	}

