//	CSystemSpacePainter.cpp
//
//	CSystemSpacePainter class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

const int STARFIELD_COUNT =						5000;
const int STARFIELD_DENSITY =					300;	//	Lower is denser (0 is invalid)
const int MIN_STAR_DISTANCE =					2;
const int MAX_STAR_DISTANCE =					20;
const int BRIGHT_STAR_CHANCE =					20;

CSystemSpacePainter::CSystemSpacePainter (void) :
		m_bInitialized(false),
		m_cxStarfield(-1),
		m_cyStarfield(-1),
		m_dwBackgroundUNID(0),
		m_pBackgroundImage(NULL)

//	CSystemSpacePainter constructor

	{
	}

void CSystemSpacePainter::CleanUp (void)

//	CleanUp
//
//	Resets all caches

	{
	m_bInitialized = false;

	m_Starfield.DeleteAll();
	m_pBackgroundImage = NULL;
	}

void CSystemSpacePainter::CreateSpaceBackground (DWORD dwBackgroundUNID)

//	CreateSpaceBackground
//
//	Initializes the (optional) system background image

	{
	//	If we don't have a background image, then we're done

	if (dwBackgroundUNID == 0
			|| (m_pBackgroundImage = g_pUniverse->GetLibraryBitmap(dwBackgroundUNID)) == NULL)
		{
		m_dwBackgroundUNID = 0;
		m_pBackgroundImage = NULL;
		return;
		}

	//	Otherwise, we have an image

	m_dwBackgroundUNID = dwBackgroundUNID;
	}

void CSystemSpacePainter::CreateStarfield (int cxField, int cyField)

//	CreateStarfield
//
//	Creates the starfield of the given size.

	{
	int i, j;

	ASSERT(!m_bInitialized);

	//	Compute count

	int iStarCount = cxField * cyField / STARFIELD_DENSITY;

	//	Figure out how many stars at each distance

	int DistCount[MAX_STAR_DISTANCE + 1];
	GenerateSquareDist(iStarCount, MIN_STAR_DISTANCE, MAX_STAR_DISTANCE, DistCount);

	//	Generate stars at each distance

	int iBrightAdj = 180 / MAX_STAR_DISTANCE;
	for (i = 0; i < MAX_STAR_DISTANCE + 1; i++)
		for (j = 0; j < DistCount[i]; j++)
			{
			SStar *pStar = m_Starfield.Insert();

			pStar->x = mathRandom(0, cxField);
			pStar->y = mathRandom(0, cyField);

			pStar->wDistance = i;

			int iBrightness = 225 - i * iBrightAdj;
			int iBlueAdj = 2 * Min(25, MAX_STAR_DISTANCE - i);
			int iRedAdj = 2 * Min(25, i);

			pStar->rgbColor = CG32bitPixel(iBrightness + mathRandom(-25 + iRedAdj, 25),
					iBrightness + mathRandom(-5, 5),
					iBrightness + mathRandom(-25 + iBlueAdj, 25));

			if (pStar->bBrightStar = (mathRandom(1, 100) <= BRIGHT_STAR_CHANCE))
				pStar->rgbSpikeColor = CG32bitPixel::Blend(0, pStar->rgbColor, (BYTE)128);
			}

	m_cxStarfield = cxField;
	m_cyStarfield = cyField;
	}

void CSystemSpacePainter::GenerateSquareDist (int iTotalCount, int iMinValue, int iMaxValue, int *Dist)

//	GenerateSquareDist
//
//	Generates buckets such that:
//
//	1. The sum of the buckets = iTotalCount
//	2. Each bucket has units proportional to the square of its index value
//
//	Dist must be allocated to at least iMaxValue + 1

	{
	int i;

	//	First generate a square distribution

	int iTotalProb = 0;
	for (i = 0; i < iMaxValue + 1; i++)
		{
		if (i >= iMinValue)
			Dist[i] = i * i;
		else
			Dist[i] = 0;

		iTotalProb += Dist[i];
		}

	ASSERT(iTotalProb > 0);
	if (iTotalProb == 0)
		return;

	//	Scale the distribution to the total count

	int iLeft = iTotalCount;
	for (i = 0; i < iMaxValue + 1; i++)
		{
		int iNumerator = Dist[i] * iTotalCount;
		int iBucketCount = iNumerator / iTotalProb;
		int iBucketCountRemainder = iNumerator % iTotalProb;
		if (mathRandom(0, iTotalProb - 1) < iBucketCountRemainder)
			iBucketCount++;

		iBucketCount = Min(iBucketCount, iLeft);
		Dist[i] = iBucketCount;
		iLeft -= iBucketCount;
		}
	}

void CSystemSpacePainter::PaintSpaceBackground (CG32bitImage &Dest, int xCenter, int yCenter, SViewportPaintCtx &Ctx)

//	PaintSpaceBackground
//
//	Paints the background image

	{
	ASSERT(m_pBackgroundImage);

	int cxImage = m_pBackgroundImage->GetWidth();
	int cyImage = m_pBackgroundImage->GetHeight();

	//	Compute the paint positions

	int xOffset = ClockMod(xCenter / 4, cxImage);
	int yOffset = ClockMod(-yCenter / 4, cyImage);

	//	Tile across the entire screen

	int ySrc = yOffset;
	int cySrc = cyImage - ySrc;

	int yDest = Ctx.rcView.top;
	int yDestEnd = Ctx.rcView.bottom;

	while (yDest < yDestEnd)
		{
		int xSrc = xOffset;
		int cxSrc = cxImage - xSrc;

		int xDest = Ctx.rcView.left;
		int xDestEnd = Ctx.rcView.right;

		while (xDest < xDestEnd)
			{
			Dest.Blt(xSrc, ySrc, cxSrc, cySrc, *m_pBackgroundImage, xDest, yDest);

			xDest += cxSrc;
			xSrc = 0;
			cxSrc = cxImage;
			}

		yDest += cySrc;
		ySrc = 0;
		cySrc = cyImage;
		}
	}

void CSystemSpacePainter::PaintStarfield (CG32bitImage &Dest, const RECT &rcView, int xCenter, int yCenter, CG32bitPixel rgbSpaceColor)

//	PaintStarfield
//
//	Paints the nearer stars

	{
	int i;

	int cxField = RectWidth(rcView);
	int cyField = RectHeight(rcView);

	//	Compute the minimum brightness to paint

	BYTE byMaxSpaceValue = rgbSpaceColor.GetMax();

	//	Precompute the star distance adj

	int xDistAdj[MAX_STAR_DISTANCE + 1];
	int yDistAdj[MAX_STAR_DISTANCE + 1];
	xDistAdj[0] = 1;
	yDistAdj[0] = 1;
	for (i = 1; i < MAX_STAR_DISTANCE + 1; i++)
		{
		xDistAdj[i] = 4 * xCenter / (i * i);
		yDistAdj[i] = 4 * yCenter / (i * i);
		}

	//	Paint each star

	CG32bitPixel *pStart = Dest.GetPixelPos(0, 0);
	int cyRow = Dest.GetPixelPos(0, 1) - pStart;

	pStart += cyRow * rcView.top + rcView.left;

	for (i = 0; i < m_Starfield.GetCount(); i++)
		{
		SStar *pStar = &m_Starfield[i];

		//	Adjust the coordinates of the star based on the position
		//	of the center and the distance

		int x = (pStar->x - xDistAdj[pStar->wDistance]) % cxField;
		if (x < 0)
			x += cxField;
		int y = (pStar->y + yDistAdj[pStar->wDistance]) % cyField;
		if (y < 0)
			y += cyField;

		//	Blt the star

		CG32bitPixel *pPixel = pStart + cyRow * y + x;

		//	Test to see if the star is brighter than background

		if (pStar->rgbColor.GetMax() > byMaxSpaceValue)
			{
			if (pStar->bBrightStar && byMaxSpaceValue < pStar->rgbSpikeColor.GetMax())
				{
				if (y < cyField - 1)
					{
					*(pPixel + 1) = pStar->rgbSpikeColor;
					*(pPixel + cyRow) = pStar->rgbSpikeColor;
					}

				if (y > 0)
					{
					*(pPixel - 1) = pStar->rgbSpikeColor;
					*(pPixel - cyRow) = pStar->rgbSpikeColor;
					}
				}

			*pPixel = pStar->rgbColor;
			}
		}
	}

void CSystemSpacePainter::PaintViewport (CG32bitImage &Dest, CSystemType *pType, SViewportPaintCtx &Ctx)

//	PaintViewport
//
//	Paint the system space background.

	{
	//	If we don't want a starfield then we just clear the rect

	if (Ctx.fNoStarfield)
		Dest.Fill(Ctx.rcView.left, Ctx.rcView.top, RectWidth(Ctx.rcView), RectHeight(Ctx.rcView), Ctx.rgbSpaceColor);

	//	Otherwise, we paint a space background

	else
		{
		//	If we haven't yet initialized, do it now

		if (!m_bInitialized)
			{
			CreateStarfield(RectWidth(Ctx.rcView), RectHeight(Ctx.rcView));
			CreateSpaceBackground(pType ? pType->GetBackgroundUNID() : 0);

			m_bInitialized = true;
			}

		//	Get the absolute position of the center. This is just to get our
		//	parallax offset. It's OK if we overflow an integer (because we want
		//	to wrap around anyways).

		int xCenter = (int)(Ctx.pCenter->GetPos().GetX() / g_KlicksPerPixel);
		int yCenter = (int)(Ctx.pCenter->GetPos().GetY() / g_KlicksPerPixel);

		//	If we have an system background image, paint that.

		if (m_pBackgroundImage)
			PaintSpaceBackground(Dest, xCenter, yCenter, Ctx);

		//	Otherwise we just clear the rect with the space color

		else
			{
			Dest.Fill(Ctx.rcView.left, Ctx.rcView.top, RectWidth(Ctx.rcView), RectHeight(Ctx.rcView), Ctx.rgbSpaceColor);

			//	Paint the star field on top

			PaintStarfield(Dest, Ctx.rcView, xCenter, yCenter, Ctx.rgbSpaceColor);
			}
		}
	}

