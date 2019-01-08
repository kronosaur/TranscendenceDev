//	CG8bitImage.cpp
//
//	CG8bitImage Class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CG8bitImage::CG8bitImage (void) :
		m_pChannel(NULL),
		m_bMarked(false)

//	CG8bitImage constructor

	{
	}

CG8bitImage::CG8bitImage (const CG8bitImage &Src)

//	CG8bitImage constructor

	{
	Copy(Src);
	}

CG8bitImage::~CG8bitImage (void)

//	CG8bitImage destructor

	{
	CleanUp();
	}

CG8bitImage &CG8bitImage::operator= (const CG8bitImage &Src)

//	CG8bitImage operator =

	{
	CleanUp();
	Copy(Src);
	return *this;
	}

void CG8bitImage::Blt (int xSrc, int ySrc, int cxWidth, int cyHeight, const CGRealChannel &Src, int xDest, int yDest)

//	Blt
//
//	Blt from a real-pixel channel.

	{
	//	Make sure we're in bounds

	if (!AdjustCoords(&xSrc, &ySrc, Src.GetWidth(), Src.GetHeight(), 
			&xDest, &yDest,
			&cxWidth, &cyHeight))
		return;

	Metric *pSrcRow = Src.GetPixelPos(xSrc, ySrc);
	Metric *pSrcRowEnd = Src.GetPixelPos(xSrc, ySrc + cyHeight);
	BYTE *pDestRow = GetPixelPos(xDest, yDest);

	while (pSrcRow < pSrcRowEnd)
		{
		Metric *pSrcPos = pSrcRow;
		Metric *pSrcPosEnd = pSrcRow + cxWidth;
		BYTE *pDestPos = pDestRow;

		while (pSrcPos < pSrcPosEnd)
			*pDestPos++ = CGRealRGB::To8bit(*pSrcPos++);

		pSrcRow = Src.NextRow(pSrcRow);
		pDestRow = NextRow(pDestRow);
		}
	}

void CG8bitImage::CleanUp (void)

//	CleanUp
//
//	Clean up the bitmap

	{
	if (m_pChannel)
		delete [] m_pChannel;

	m_pChannel = NULL;
	m_cxWidth = 0;
	m_cyHeight = 0;
	ResetClipRect();
	}

void CG8bitImage::Copy (const CG8bitImage &Src)

//	Copy
//
//	Copy from source. We assume that we are already clean.

	{
	if (Src.IsEmpty())
		return;

	//	Copy the buffer

	int iSize = CalcBufferSize(Src.m_cxWidth, Src.m_cyHeight);
	m_pChannel = new BYTE [iSize];

	BYTE *pSrc = Src.m_pChannel;
	BYTE *pSrcEnd = pSrc + iSize;
	BYTE *pDest = m_pChannel;

	while (pSrc < pSrcEnd)
		*pDest++ = *pSrc++;

	//	Now copy the remaining variable

	m_cxWidth = Src.m_cxWidth;
	m_cyHeight = Src.m_cyHeight;
	m_rcClip = Src.m_rcClip;
	m_bMarked = Src.m_bMarked;
	}

bool CG8bitImage::Create (int cxWidth, int cyHeight, BYTE InitialValue)

//	Create
//
//	Creates a blank image

	{
	CleanUp();
	if (cxWidth <= 0 || cyHeight <= 0)
		return false;

	//	Allocate a new buffer

	int iSize = CalcBufferSize(cxWidth, cyHeight);
	m_pChannel = new BYTE [iSize];

	//	Initialize

	BYTE *pDest = m_pChannel;
	BYTE *pDestEnd = pDest + iSize;

	while (pDest < pDestEnd)
		*pDest++ = InitialValue;

	//	Other variables

	m_cxWidth = cxWidth;
	m_cyHeight = cyHeight;
	ResetClipRect();

	return true;
	}

bool CG8bitImage::CreateChannel (ChannelTypes iChannel, const CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc)

//	CreateChannel
//
//	Creates from a channel

	{
	CleanUp();

	if (cxSrc == -1)
		cxSrc = Src.GetWidth();

	if (cySrc == -1)
		cySrc = Src.GetHeight();

	//	Make sure we're in bounds

	if (!Src.AdjustCoords(NULL, NULL, 0, 0, 
			&xSrc, &ySrc,
			&cxSrc, &cySrc))
		return true;

	//	Allocate a new buffer

	int iSize = CalcBufferSize(cxSrc, cySrc);
	m_pChannel = new BYTE [iSize];
	m_cxWidth = cxSrc;
	m_cyHeight = cySrc;
	ResetClipRect();

	//	Copy data from the channel

	CG32bitPixel *pSrcRow = Src.GetPixelPos(xSrc, ySrc);
	CG32bitPixel *pSrcRowEnd = Src.GetPixelPos(xSrc, ySrc + cySrc);
	BYTE *pDestRow = GetPixelPos(0, 0);

	switch (iChannel)
		{
		case channelAlpha:
			while (pSrcRow < pSrcRowEnd)
				{
				CG32bitPixel *pSrcPos = pSrcRow;
				CG32bitPixel *pSrcPosEnd = pSrcRow + cxSrc;
				BYTE *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					{
					*pDestPos = pSrcPos->GetAlpha();

					pSrcPos++;
					pDestPos++;
					}

				pSrcRow = Src.NextRow(pSrcRow);
				pDestRow = NextRow(pDestRow);
				}
			break;

		case channelRed:
			while (pSrcRow < pSrcRowEnd)
				{
				CG32bitPixel *pSrcPos = pSrcRow;
				CG32bitPixel *pSrcPosEnd = pSrcRow + cxSrc;
				BYTE *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					{
					*pDestPos = pSrcPos->GetRed();

					pSrcPos++;
					pDestPos++;
					}

				pSrcRow = Src.NextRow(pSrcRow);
				pDestRow = NextRow(pDestRow);
				}
			break;

		case channelGreen:
			while (pSrcRow < pSrcRowEnd)
				{
				CG32bitPixel *pSrcPos = pSrcRow;
				CG32bitPixel *pSrcPosEnd = pSrcRow + cxSrc;
				BYTE *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					{
					*pDestPos = pSrcPos->GetGreen();

					pSrcPos++;
					pDestPos++;
					}

				pSrcRow = Src.NextRow(pSrcRow);
				pDestRow = NextRow(pDestRow);
				}
			break;

		case channelBlue:
			while (pSrcRow < pSrcRowEnd)
				{
				CG32bitPixel *pSrcPos = pSrcRow;
				CG32bitPixel *pSrcPosEnd = pSrcRow + cxSrc;
				BYTE *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					{
					*pDestPos = pSrcPos->GetBlue();

					pSrcPos++;
					pDestPos++;
					}

				pSrcRow = Src.NextRow(pSrcRow);
				pDestRow = NextRow(pDestRow);
				}
			break;
		}

	return true;
	}

bool CG8bitImage::CreateRoundedRect (int cxWidth, int cyHeight, int iRadius, BYTE Foreground, BYTE Background)

//	CreateRoundedRect
//
//	Creates a rounded rect mask

	{
	int i;

	if (cxWidth <= 0 || cyHeight <= 0)
		return false;

	//	Start with a full rectangle

	if (!Create(cxWidth, cyHeight, Foreground))
		return false;

	//	Radius can't be larger than half the dimensions

	iRadius = Min(Min(iRadius, cxWidth / 2), cyHeight / 2);
	if (iRadius <= 0)
		return false;

	//	Generate a set of raster lines for the corner
	//	NOTE: We guarantee that the solid part is always 1 less than the radius.

	int *pSolid = new int [iRadius];
	BYTE *pEdge = new BYTE [iRadius];
	RasterizeQuarterCircle8bit(iRadius, pSolid, pEdge, Foreground);

	//	Mask out the corners

	for (i = 0; i < iRadius; i++)
		{
		BYTE *pTopRow = GetPixelPos(0, i);
		BYTE *pBottomRow = GetPixelPos(0, cyHeight - (i + 1));

		int cxEdge = (iRadius - (pSolid[i] + 1));

		//	Left corners

		if (cxEdge > 0)
			{
			utlMemSet(pTopRow, cxEdge, Background);
			utlMemSet(pBottomRow, cxEdge, Background);
			}

		pTopRow[cxEdge] = pEdge[i];
		pBottomRow[cxEdge] = pEdge[i];

		//	Right corners

		if (cxEdge > 0)
			{
			utlMemSet(pTopRow + cxWidth - cxEdge, cxEdge, Background);
			utlMemSet(pBottomRow + cxWidth - cxEdge, cxEdge, Background);
			}

		pTopRow[cxWidth - cxEdge - 1] = pEdge[i];
		pBottomRow[cxWidth - cxEdge - 1] = pEdge[i];
		}

	//	Clean up

	delete [] pSolid;
	delete [] pEdge;

	return true;
	}

void CG8bitImage::Fill (int x, int y, int cxWidth, int cyHeight, BYTE Value)

//	Fill
//
//	Fill the buffer

	{
	//	Make sure we're in bounds

	if (!AdjustCoords(NULL, NULL, 0, 0, 
			&x, &y, 
			&cxWidth, &cyHeight))
		return;

	BYTE *pDestRow = GetPixelPos(x, y);
	BYTE *pDestRowEnd = GetPixelPos(x, y + cyHeight);

	while (pDestRow < pDestRowEnd)
		{
		BYTE *pDest = pDestRow;
		BYTE *pDestEnd = pDestRow + cxWidth;

		while (pDest < pDestEnd)
			*pDest++ = Value;

		pDestRow = NextRow(pDestRow);
		}
	}

void CG8bitImage::IntersectChannel (ChannelTypes iChannel, const CG32bitImage &Src, int xDest, int yDest, int xSrc, int ySrc, int cxSrc, int cySrc)

//	IntersectMask
//
//	Intersects the given channel with this bitmap using a multiply operation.

	{
	if (cxSrc == -1)
		cxSrc = Src.GetWidth();

	if (cySrc == -1)
		cySrc = Src.GetHeight();

	//	Make sure we're in bounds

	if (!AdjustCoords(&xSrc, &ySrc, Src.GetWidth(), Src.GetHeight(), 
			&xDest, &yDest,
			&cxSrc, &cySrc))
		return;

	//	Apply data from the channel

	CG32bitPixel *pSrcRow = Src.GetPixelPos(xSrc, ySrc);
	CG32bitPixel *pSrcRowEnd = Src.GetPixelPos(xSrc, ySrc + cySrc);
	BYTE *pDestRow = GetPixelPos(xDest, yDest);

	switch (iChannel)
		{
		case channelAlpha:
			while (pSrcRow < pSrcRowEnd)
				{
				CG32bitPixel *pSrcPos = pSrcRow;
				CG32bitPixel *pSrcPosEnd = pSrcRow + cxSrc;
				BYTE *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					{
					*pDestPos = CG32bitPixel::BlendAlpha(*pDestPos, pSrcPos->GetAlpha());

					pSrcPos++;
					pDestPos++;
					}

				pSrcRow = Src.NextRow(pSrcRow);
				pDestRow = NextRow(pDestRow);
				}
			break;

		case channelRed:
			while (pSrcRow < pSrcRowEnd)
				{
				CG32bitPixel *pSrcPos = pSrcRow;
				CG32bitPixel *pSrcPosEnd = pSrcRow + cxSrc;
				BYTE *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					{
					*pDestPos = CG32bitPixel::BlendAlpha(*pDestPos, pSrcPos->GetRed());

					pSrcPos++;
					pDestPos++;
					}

				pSrcRow = Src.NextRow(pSrcRow);
				pDestRow = NextRow(pDestRow);
				}
			break;

		case channelGreen:
			while (pSrcRow < pSrcRowEnd)
				{
				CG32bitPixel *pSrcPos = pSrcRow;
				CG32bitPixel *pSrcPosEnd = pSrcRow + cxSrc;
				BYTE *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					{
					*pDestPos = CG32bitPixel::BlendAlpha(*pDestPos, pSrcPos->GetGreen());

					pSrcPos++;
					pDestPos++;
					}

				pSrcRow = Src.NextRow(pSrcRow);
				pDestRow = NextRow(pDestRow);
				}
			break;

		case channelBlue:
			while (pSrcRow < pSrcRowEnd)
				{
				CG32bitPixel *pSrcPos = pSrcRow;
				CG32bitPixel *pSrcPosEnd = pSrcRow + cxSrc;
				BYTE *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					{
					*pDestPos = CG32bitPixel::BlendAlpha(*pDestPos, pSrcPos->GetBlue());

					pSrcPos++;
					pDestPos++;
					}

				pSrcRow = Src.NextRow(pSrcRow);
				pDestRow = NextRow(pDestRow);
				}
			break;
		}
	}

