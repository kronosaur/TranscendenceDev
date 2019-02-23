//	FilterBlur.cpp
//
//	CGFilter Class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CGFilter::Blur (CG8bitImage &Src, const RECT &rcSrc, Metric rRadius, CG8bitImage &Dest, int xDest, int yDest)

//	Blur
//
//	Blurs image.

	{
	int xSrc = rcSrc.left;
	int ySrc = rcSrc.top;
	int cxWidth = RectWidth(rcSrc);
	int cyHeight = RectHeight(rcSrc);

	//	Make sure we're in bounds

	if (!Dest.AdjustCoords(&xSrc, &ySrc, Src.GetWidth(), Src.GetHeight(), 
			&xDest, &yDest,
			&cxWidth, &cyHeight))
		return;

	//	Generate an image plane with real pixels.

	CGRealChannel RealSrc;
	if (!RealSrc.Create(Src, xSrc, ySrc, cxWidth, cyHeight))
		return;

	//	Blur the channel

	CGRealChannel RealBlur;
	CreateBlur(RealSrc, rRadius, RealBlur);

	//	Now write out the real image to the 8 bit image.

	Dest.Blt(0, 0, cxWidth, cyHeight, RealBlur, xDest, yDest);
	}

void CGFilter::CreateBlur (CGRealChannel &Src, Metric rRadius, CGRealChannel &Dest)

//	CreateBlur
//
//	This is a recursive approximation of a Gaussian blur, as presented in:
//	Alexey Lukin's Tips & Tricks: Fast Image Filtering Algorithms.
//
//	Lukin describes a method in:
//	I.T. Young, L.J. van Vliet "Recursive implementation of the gaussian filter"
//	Signal Processing (44), pp. 139-151, 1995

	{
#ifdef DEBUG
	DWORD dwStart = ::GetTickCount();
#endif

	if (Src.IsEmpty())
		return;

	int cxWidth = Src.GetWidth();
	int cyHeight = Src.GetHeight();

	if (cxWidth < 4 || cyHeight < 4)
		return;

	//	We always create a new image

	Dest.Create(cxWidth, cyHeight, 1.0);

	//	Allocate a scratch array that can hold a single line

	Metric *pScratch = new Metric [Max(cxWidth, cyHeight)];

	//	Compute some constants

	Metric rSigma = rRadius;

	Metric q =
		(rSigma >= 2.5	? 0.98711 * rSigma - 0.96330
						: 3.97156 - 4.14554 * sqrt(1.0 - 0.26891 * rSigma));

	Metric q2 = q * q;
	Metric q3 = q2 * q;

	Metric b0 = 1.57825 + 2.44413 * q + 1.4281 * q2 + 0.422205 * q3;
	Metric b1 = (2.44413 * q + 2.85619 * q2 + 1.26661 * q3) / b0;
	Metric b2 = (-1.4281 * q2 - 1.26661 * q3) / b0;
	Metric b3 = (0.422205 * q3) / b0;

	Metric B = 1.0 - (b1 + b2 + b3);

	//	The first pass is by rows

	Metric *pLine = Src.GetPixelPos(0, 0);
	Metric *pLineEnd = Src.GetPixelPos(0, cyHeight);

	Metric *pDestLine = Dest.GetPixelPos(0, 0);

	while (pLine < pLineEnd)
		{
		//	First compute the line in the causal (forward) direction

		Metric *pPos = pLine;
		Metric *pPosEnd = pLine + cxWidth;
		Metric *pDest = pScratch;

		//	Handle the left edge

		pDest[0] = B * pPos[0];
		pDest[1] = B * pPos[1] + b1 * pDest[0];
		pDest[2] = B * pPos[2] + b1 * pDest[1] + b2 * pDest[0];
		pDest[3] = B * pPos[3] + b1 * pDest[2] + b2 * pDest[1] + b3 * pDest[0];

		//	Do the rest of the line

		pPos += 4;
		pDest += 4;
		while (pPos < pPosEnd)
			{
			pDest[0] = B * pPos[0] + b1 * pDest[-1] + b2 * pDest[-2] + b3 * pDest[-3];

			pPos++;
			pDest++;
			}

		//	Now compute in the anti-causal (backwards) direction

		pPos = pScratch + cxWidth - 4;
		pDest = pDestLine + cxWidth - 4;

		//	Handle the right edge

		pDest[3] = B * pPos[3];
		pDest[2] = B * pPos[2] + b1 * pDest[3];
		pDest[1] = B * pPos[1] + b1 * pDest[2] + b2 * pDest[3];
		pDest[0] = B * pPos[0] + b1 * pDest[1] + b2 * pDest[2] + b3 * pDest[3];


		//	Do the rest

		while (pPos > pScratch)
			{
			pPos--;
			pDest--;

			pDest[0] = B * pPos[0] + b1 * pDest[1] + b2 * pDest[2] + b3 * pDest[3];
			}

		pLine += cxWidth;
		pDestLine += cxWidth;
		}

	//	Now do a pass by columns

	Metric *pCol = Dest.GetPixelPos(0, 0);
	Metric *pColEnd = pCol + cxWidth;

	Metric *pDestCol = Dest.GetPixelPos(0, 0);

	while (pCol < pColEnd)
		{
		//	First compute the column in the causal (down) direction

		Metric *pPos = pCol;
		Metric *pPosEnd = pCol + (cxWidth * cyHeight);
		Metric *pDest = pScratch;

		//	Handle the top edge

		pDest[0] = B * pPos[0];
		pDest[1] = B * pPos[    cxWidth] + b1 * pDest[0];
		pDest[2] = B * pPos[2 * cxWidth] + b1 * pDest[1] + b2 * pDest[0];
		pDest[3] = B * pPos[3 * cxWidth] + b1 * pDest[2] + b2 * pDest[1] + b3 * pDest[0];

		//	Do the rest of the line

		pPos += 4 * cxWidth;
		pDest += 4;
		while (pPos < pPosEnd)
			{
			pDest[0] = B * pPos[0] + b1 * pDest[-1] + b2 * pDest[-2] + b3 * pDest[-3];

			pPos += cxWidth;
			pDest++;
			}

		//	Now compute in the anti-causal (backwards) direction

		pPos = pScratch + cyHeight - 4;
		pDest = pCol + cxWidth * (cyHeight - 4);

		//	Handle the right edge

		pDest[3 * cxWidth] = B * pPos[3];
		pDest[2 * cxWidth] = B * pPos[2] + b1 * pDest[3 * cxWidth];
		pDest[    cxWidth] = B * pPos[1] + b1 * pDest[2 * cxWidth] + b2 * pDest[3 * cxWidth];
		pDest[0          ] = B * pPos[0] + b1 * pDest[    cxWidth] + b2 * pDest[2 * cxWidth] + b3 * pDest[3 * cxWidth];

		//	Do the rest

		while (pPos > pScratch)
			{
			pPos--;
			pDest -= cxWidth;

			pDest[0] = B * pPos[0] + b1 * pDest[cxWidth] + b2 * pDest[2 * cxWidth] + b3 * pDest[3 * cxWidth];
			}

		pCol++;
		pDestCol++;
		}

	//	Done

	delete [] pScratch;

#ifdef DEBUG
	char szBuffer[1024];
	wsprintf(szBuffer, "FilterBlur: %d ms\n", ::GetTickCount() - dwStart);
	::OutputDebugString(szBuffer);
#endif
	}
