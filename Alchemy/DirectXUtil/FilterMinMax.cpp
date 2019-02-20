//	FilterMinMax.cpp
//
//	CGFilter Class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

class MaxCompare
	{
	public:
		static constexpr bool Compare (BYTE a, BYTE b) { return (a > b); }
		static constexpr bool CompareEQ (BYTE a, BYTE b) { return (a >= b); }
	};

class MinCompare
	{
	public:
		static constexpr bool Compare (BYTE a, BYTE b) { return (a < b); }
		static constexpr bool CompareEQ (BYTE a, BYTE b) { return (a <= b); }
	};

void CGFilter::MaxPixel (const CG8bitImage &Src, const RECT &rcSrc, int iRadius, CG8bitImage &Dest, int xDest, int yDest)

//	MaxPixel
//
//	Generates an image in which each pixel is set to the maximum value of all 
//	pixels within a certain (square) radius.

	{
	MinMaxPixel<MaxCompare>(Src, rcSrc, iRadius, Dest, xDest, yDest);
	}

void CGFilter::MinPixel (const CG8bitImage &Src, const RECT &rcSrc, int iRadius, CG8bitImage &Dest, int xDest, int yDest)

//	MinPixel
//
//	Generates an image in which each pixel is set to the minimum value of all 
//	pixels within a certain (square) radius.

	{
	MinMaxPixel<MinCompare>(Src, rcSrc, iRadius, Dest, xDest, yDest);
	}

template <class T>
void CGFilter::MinMaxPixel (const CG8bitImage &Src, const RECT &rcSrc, int iRadius, CG8bitImage &Dest, int xDest, int yDest)

//	MinMaxPixel
//
//	Generates an image in which each pixel is set to the minimum/maximum value 
//	of all pixels within a certain (square) radius.
//
//	Adapted from 2D Minimum and Maximum Filters: Algorithms and Implementation
//	Issues by Herman Tulleken (http://www.code-spot.co.za).

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

	//	If the destination is the same size as the source, then we can do this
	//	in place. Otherwise, we need a temporary buffer.

	CG8bitImage *pDest;
	CG8bitImage Temp;
	if (Dest.GetWidth() == cxWidth && Dest.GetHeight() == cyHeight
			&& xDest == 0 && yDest == 0)
		pDest = &Dest;
	else
		{
		Temp.Create(cxWidth, cyHeight);
		pDest = &Temp;
		}

	//	Filter rows.

	CG8bitImage Intermediate;
	Intermediate.Create(cxWidth, cyHeight);
	MinMaxPixelRows<T>(Src, xSrc, ySrc, cxWidth, cyHeight, iRadius, Intermediate);

	//	Now filter by columns.

	MinMaxPixelCols<T>(Intermediate, xSrc, ySrc, cxWidth, cyHeight, iRadius, *pDest);

	//	If necessary, we copy back to the destination.

	if (pDest != &Dest)
		{
		Dest.Copy(*pDest, 0, 0, cxWidth, cyHeight, xDest, yDest);
		}
	}

template <class T>
void CGFilter::MinMaxPixelCols (const CG8bitImage &Src, int xSrc, int ySrc, int cxWidth, int cyHeight, int iRadius, CG8bitImage &Dest)

//	MaxPixelCols
//
//	Iterates over all columns and sets the destination pixel with the maximum 
//	value.

	{
	BYTE *pSrcCol = Src.GetPixelPos(xSrc, ySrc);
	BYTE *pSrcColEnd = Src.GetPixelPos(xSrc + cxWidth, ySrc);
	BYTE *pDestCol = Dest.GetPixelPos(0, 0);

	//	Loop over all rows

	while (pSrcCol < pSrcColEnd)
		{
		BYTE *pSrcRowEnd = pSrcCol + Src.GetWidth() * cyHeight;
		BYTE *pReadPos = pSrcCol;
		BYTE *pStartPos = pSrcCol;		//	Start of implicit queue
		BYTE *pMaxPos = pSrcCol;		//	Max value in window
		BYTE *pWritePos = pDestCol;
		BYTE *pWritePosEnd = pDestCol + (cxWidth * cyHeight);
		BYTE *pSrcWindowEnd = pSrcCol + Src.GetWidth() * (2 * iRadius + 1);

		//	Read values to fill the window

		BYTE *pSrcStartEnd = pSrcCol + Src.GetWidth() * iRadius;
		while (pReadPos < pSrcStartEnd)
			{
			if (T::Compare(*pReadPos, *pMaxPos))
				pMaxPos = pReadPos;

			pReadPos = Src.NextRow(pReadPos);
			}

		//	Continue to the end of the column writing out to the destination.

		while (pReadPos < pSrcRowEnd)
			{
			if (T::CompareEQ(*pReadPos, *pMaxPos))
				pMaxPos = pReadPos;

			//	Write to destination.

			*pWritePos = *pMaxPos;

			//	If a pixel is moving out of the window, then we need to find the
			//	next max value.

			if (Src.NextRow(pReadPos) >= pSrcWindowEnd)
				{
				//	The maximum pixel is moving out of the window next loop, so
				//	we need to find the next maximum.

				if (pStartPos == pMaxPos)
					{
					//	Start with maximum at first position in the scan.

					pMaxPos = Src.NextRow(pStartPos);

					//	Scan up to the current read position (inclusive)

					BYTE *pScanPos = Src.NextRow(pMaxPos);
					BYTE *pScanPosEnd = Src.NextRow(pReadPos);

					//	See if we have a better maximum (but we're done if we 
					//	ever find 255, which is always the maximum value for 
					//	8-bit pixels.

					while (*pMaxPos < 255 && pScanPos < pScanPosEnd)
						{
						if (T::CompareEQ(*pScanPos, *pMaxPos))
							pMaxPos = pScanPos;

						pScanPos = Src.NextRow(pScanPos);
						}
					}

				pStartPos = Src.NextRow(pStartPos);
				}

			//	Next

			pReadPos = Src.NextRow(pReadPos);
			pWritePos = Dest.NextRow(pWritePos);
			}

		//	Write out the end of the row

		while (pWritePos < pWritePosEnd)
			{
			*pWritePos = *pMaxPos;

			if (pStartPos == pMaxPos)
				{
				pMaxPos = Src.NextRow(pStartPos);

				BYTE *pScanPos = Src.NextRow(pMaxPos);
				BYTE *pScanPosEnd = pSrcRowEnd;

				while (*pMaxPos < 255 && pScanPos < pScanPosEnd)
					{
					if (T::CompareEQ(*pScanPos, *pMaxPos))
						pMaxPos = pScanPos;

					pScanPos = Src.NextRow(pScanPos);
					}
				}

			pWritePos = Src.NextRow(pWritePos);
			pStartPos = Src.NextRow(pStartPos);
			}

		//	Next col

		pSrcCol++;
		pDestCol++;
		}
	}

template <class T>
void CGFilter::MinMaxPixelRows (const CG8bitImage &Src, int xSrc, int ySrc, int cxWidth, int cyHeight, int iRadius, CG8bitImage &Dest)

//	MaxPixelRows
//
//	Iterates over all rows and sets the destination pixel with the maximum value
//	over a 1D windows of size iRadius.

	{
	BYTE *pSrcRow = Src.GetPixelPos(xSrc, ySrc);
	BYTE *pSrcRowEnd = Src.GetPixelPos(xSrc, ySrc + cyHeight);
	BYTE *pDestRow = Dest.GetPixelPos(0, 0);

	//	Loop over all rows

	while (pSrcRow < pSrcRowEnd)
		{
		BYTE *pSrcPosEnd = pSrcRow + cxWidth;
		BYTE *pReadPos = pSrcRow;
		BYTE *pStartPos = pSrcRow;		//	Start of implicit queue
		BYTE *pMaxPos = pSrcRow;		//	Max value in window
		BYTE *pWritePos = pDestRow;
		BYTE *pWritePosEnd = pDestRow + cxWidth;
		BYTE *pSrcWindowEnd = pSrcRow + (2 * iRadius + 1);

		//	Read values to fill the window

		BYTE *pSrcStartEnd = pSrcRow + iRadius;
		while (pReadPos < pSrcStartEnd)
			{
			if (T::Compare(*pReadPos, *pMaxPos))
				pMaxPos = pReadPos;

			pReadPos++;
			}

		//	Continue to the end of the row writing out to the destination.

		while (pReadPos < pSrcPosEnd)
			{
			if (T::CompareEQ(*pReadPos, *pMaxPos))
				pMaxPos = pReadPos;

			//	Write to destination.

			*pWritePos = *pMaxPos;

			//	If a pixel is moving out of the window, then we need to find the
			//	next max value.

			if (pReadPos + 1 >= pSrcWindowEnd)
				{
				//	The maximum pixel is moving out of the window next loop, so
				//	we need to find the next maximum.

				if (pStartPos == pMaxPos)
					{
					//	Start with maximum at first position in the scan.

					pMaxPos = pStartPos + 1;

					//	Scan up to the current read position (inclusive)

					BYTE *pScanPos = pMaxPos + 1;
					BYTE *pScanPosEnd = pReadPos + 1;

					//	See if we have a better maximum (but we're done if we 
					//	ever find 255, which is always the maximum value for 
					//	8-bit pixels.

					while (*pMaxPos < 255 && pScanPos < pScanPosEnd)
						{
						if (T::CompareEQ(*pScanPos, *pMaxPos))
							pMaxPos = pScanPos;

						pScanPos++;
						}
					}

				pStartPos++;
				}

			//	Next

			pReadPos++;
			pWritePos++;
			}

		//	Write out the end of the row

		while (pWritePos < pWritePosEnd)
			{
			*pWritePos = *pMaxPos;

			if (pStartPos == pMaxPos)
				{
				pMaxPos = pStartPos + 1;

				BYTE *pScanPos = pMaxPos + 1;
				BYTE *pScanPosEnd = pSrcPosEnd;

				while (*pMaxPos < 255 && pScanPos < pScanPosEnd)
					{
					if (T::CompareEQ(*pScanPos, *pMaxPos))
						pMaxPos = pScanPos;

					pScanPos++;
					}
				}

			pWritePos++;
			pStartPos++;
			}

		//	Next row

		pSrcRow = Src.NextRow(pSrcRow);
		pDestRow = Dest.NextRow(pDestRow);
		}
	}
