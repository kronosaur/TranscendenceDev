//	CGRealChannel.cpp
//
//	CGRealChannel Class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CGRealChannel::CGRealChannel (void) :
		m_pChannel(NULL),
		m_bMarked(false)

//	CGRealChannel constructor

	{
	}

CGRealChannel::CGRealChannel (const CGRealChannel &Src)

//	CGRealChannel constructor

	{
	Copy(Src);
	}

CGRealChannel::~CGRealChannel (void)

//	CGRealChannel destructor

	{
	CleanUp();
	}

CGRealChannel &CGRealChannel::operator= (const CGRealChannel &Src)

//	CGRealChannel operator =

	{
	CleanUp();
	Copy(Src);
	return *this;
	}

void CGRealChannel::CleanUp (void)

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

void CGRealChannel::Copy (const CGRealChannel &Src)

//	Copy
//
//	Copy from source. We assume that we are already clean.

	{
	if (Src.IsEmpty())
		return;

	//	Copy the buffer

	int iSize = CalcBufferSize(Src.m_cxWidth, Src.m_cyHeight);
	m_pChannel = new Metric [iSize];

	Metric *pSrc = Src.m_pChannel;
	Metric *pSrcEnd = pSrc + iSize;
	Metric *pDest = m_pChannel;

	while (pSrc < pSrcEnd)
		*pDest++ = *pSrc++;

	//	Now copy the remaining variable

	m_cxWidth = Src.m_cxWidth;
	m_cyHeight = Src.m_cyHeight;
	m_rcClip = Src.m_rcClip;
	m_bMarked = Src.m_bMarked;
	}

bool CGRealChannel::Create (int cxWidth, int cyHeight, Metric InitialValue)

//	Create
//
//	Creates a blank image

	{
	CleanUp();
	if (cxWidth <= 0 || cyHeight <= 0)
		return false;

	//	Allocate a new buffer

	int iSize = CalcBufferSize(cxWidth, cyHeight);
	m_pChannel = new Metric [iSize];

	//	Initialize

	Metric *pDest = m_pChannel;
	Metric *pDestEnd = pDest + iSize;

	while (pDest < pDestEnd)
		*pDest++ = InitialValue;

	//	Other variables

	m_cxWidth = cxWidth;
	m_cyHeight = cyHeight;
	ResetClipRect();

	return true;
	}

bool CGRealChannel::Create (const CG8bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc)

//	Create
//
//	Creates from an 8-bit image.

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
	m_pChannel = new Metric [iSize];
	m_cxWidth = cxSrc;
	m_cyHeight = cySrc;
	ResetClipRect();

	//	Copy

	BYTE *pSrcRow = Src.GetPixelPos(xSrc, ySrc);
	BYTE *pSrcRowEnd = Src.GetPixelPos(xSrc, ySrc + cySrc);
	Metric *pDestRow = GetPixelPos(0, 0);

	while (pSrcRow < pSrcRowEnd)
		{
		BYTE *pSrcPos = pSrcRow;
		BYTE *pSrcPosEnd = pSrcRow + cxSrc;
		Metric *pDestPos = pDestRow;

		while (pSrcPos < pSrcPosEnd)
			{
			*pDestPos = CGRealRGB::From8bit(*pSrcPos);

			pSrcPos++;
			pDestPos++;
			}

		pSrcRow = Src.NextRow(pSrcRow);
		pDestRow = NextRow(pDestRow);
		}

	return true;
	}

bool CGRealChannel::CreateChannel (ChannelTypes iChannel, const CG32bitImage &Src, int xSrc, int ySrc, int cxSrc, int cySrc)

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
	m_pChannel = new Metric [iSize];
	m_cxWidth = cxSrc;
	m_cyHeight = cySrc;
	ResetClipRect();

	//	Copy data from the channel

	CG32bitPixel *pSrcRow = Src.GetPixelPos(xSrc, ySrc);
	CG32bitPixel *pSrcRowEnd = Src.GetPixelPos(xSrc, ySrc + cySrc);
	Metric *pDestRow = GetPixelPos(0, 0);

	switch (iChannel)
		{
		case channelAlpha:
			while (pSrcRow < pSrcRowEnd)
				{
				CG32bitPixel *pSrcPos = pSrcRow;
				CG32bitPixel *pSrcPosEnd = pSrcRow + cxSrc;
				Metric *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					{
					*pDestPos = CGRealRGB::From8bit(pSrcPos->GetAlpha());

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
				Metric *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					{
					*pDestPos = CGRealRGB::From8bit(pSrcPos->GetRed());

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
				Metric *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					{
					*pDestPos = CGRealRGB::From8bit(pSrcPos->GetGreen());

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
				Metric *pDestPos = pDestRow;

				while (pSrcPos < pSrcPosEnd)
					{
					*pDestPos = CGRealRGB::From8bit(pSrcPos->GetBlue());

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

void CGRealChannel::Fill (int x, int y, int cxWidth, int cyHeight, Metric Value)

//	Fill
//
//	Fill the buffer

	{
	//	Make sure we're in bounds

	if (!AdjustCoords(NULL, NULL, 0, 0, 
			&x, &y, 
			&cxWidth, &cyHeight))
		return;

	Metric *pDestRow = GetPixelPos(x, y);
	Metric *pDestRowEnd = GetPixelPos(x, y + cyHeight);

	while (pDestRow < pDestRowEnd)
		{
		Metric *pDest = pDestRow;
		Metric *pDestEnd = pDestRow + cxWidth;

		while (pDest < pDestEnd)
			*pDest++ = Value;

		pDestRow = NextRow(pDestRow);
		}
	}

