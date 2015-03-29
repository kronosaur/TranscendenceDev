//	CGRasterize.cpp
//
//	CGRasterize Class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CGRasterize::Line (CG32bitImage &Dest, int x1, int y1, int x2, int y2, int iWidth, TArray<SLinePixel> *retPixels)

//	Line
//
//	Rasterize a line.

	{
	CLinePainter LinePainter;
	LinePainter.Rasterize(Dest, x1, y1, x2, y2, iWidth, retPixels);
	}
