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
		m_cxStarfield(-1),
		m_cyStarfield(-1)

//	CSystemSpacePainter constructor

	{
	}

void CSystemSpacePainter::CleanUp (void)

//	CleanUp
//
//	Resets all caches

	{
	m_cxStarfield = -1;
	m_cyStarfield = -1;

	m_Starfield.DeleteAll();
	}

void CSystemSpacePainter::CreateStarfield (int cxField, int cyField)

//	CreateStarfield
//
//	Creates the starfield of the given size.

	{
	int i, j;

	if (m_cxStarfield == cxField && m_cyStarfield == cyField)
		return;

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

			pStar->wColor = CG16bitImage::RGBValue(iBrightness + mathRandom(-25 + iRedAdj, 25),
					iBrightness + mathRandom(-5, 5),
					iBrightness + mathRandom(-25 + iBlueAdj, 25));

			if (pStar->bBrightStar = (mathRandom(1, 100) <= BRIGHT_STAR_CHANCE))
				pStar->wSpikeColor = CG16bitImage::BlendPixel(0, pStar->wColor, 128);
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

void CSystemSpacePainter::PaintViewport (CG16bitImage &Dest, CSystemType *pType, SViewportPaintCtx &Ctx)

//	PaintViewport
//
//	Paint the system space background.

	{
	//	Clear the rect

	Dest.Fill(Ctx.rcView.left, Ctx.rcView.top, RectWidth(Ctx.rcView), RectHeight(Ctx.rcView), Ctx.wSpaceColor);

	//	Paint the star field

	if (!Ctx.fNoStarfield)
		PaintStarfield(Dest, Ctx.rcView, Ctx.pCenter, g_KlicksPerPixel, Ctx.wSpaceColor);
	}

void CSystemSpacePainter::PaintStarfield (CG16bitImage &Dest, const RECT &rcView, CSpaceObject *pCenter, Metric rKlicksPerPixel, WORD wSpaceColor)

//	PaintStarfield
//
//	Paints the nearer stars

	{
	int i;

	int cxField = RectWidth(rcView);
	int cyField = RectHeight(rcView);

	//	Make sure the star field is created to fit the viewport

	CreateStarfield(cxField, cyField);

	//	Compute the minimum brightness to paint

	WORD wMaxColor = (WORD)(Max(Max(CG16bitImage::RedValue(wSpaceColor), CG16bitImage::GreenValue(wSpaceColor)), CG16bitImage::BlueValue(wSpaceColor)));
	WORD wSpaceValue = CG16bitImage::RGBValue(wMaxColor, wMaxColor, wMaxColor);

	//	Get the absolute position of the center

	int xCenter = (int)(pCenter->GetPos().GetX() / rKlicksPerPixel);
	int yCenter = (int)(pCenter->GetPos().GetY() / rKlicksPerPixel);

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

	WORD *pStart = Dest.GetRowStart(0);
	int cyRow = Dest.GetRowStart(1) - pStart;

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

		WORD *pPixel = pStart + cyRow * y + x;

		//	Cheap (if inaccurate) test to see if the star is brighter than background

		if (wSpaceValue < pStar->wColor)
			{
			if (pStar->bBrightStar && wSpaceValue < pStar->wSpikeColor)
				{
				if (y < cyField - 1)
					{
					*(pPixel + 1) = pStar->wSpikeColor;
					*(pPixel + cyRow) = pStar->wSpikeColor;
					}

				if (y > 0)
					{
					*(pPixel - 1) = pStar->wSpikeColor;
					*(pPixel - cyRow) = pStar->wSpikeColor;
					}
				}

			*pPixel = pStar->wColor;
			}
		}
	}

