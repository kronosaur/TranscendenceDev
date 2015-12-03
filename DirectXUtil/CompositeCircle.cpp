//	CGComposite.cpp
//
//	CGComposite Class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CGComposite::Circle (CG32bitImage &Dest, int x, int y, int iRadius, const TArray<CG32bitPixel> &ColorRamp)

//	Circle
//
//	Draws a circle with a color ramp

	{
	CRadialCirclePainter<CGBlendCopy> Painter(iRadius, ColorRamp);
	Painter.Composite(Dest, x, y);
	}
