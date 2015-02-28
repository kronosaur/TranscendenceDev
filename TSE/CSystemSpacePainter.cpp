//	CSystemSpacePainter.cpp
//
//	CSystemSpacePainter class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#ifdef DEBUG
#define DEBUG_PAINT_TIMINGS
#endif

const int STARFIELD_COUNT =						5000;
const int STARFIELD_DENSITY =					300;	//	Lower is denser (0 is invalid)
const int MIN_STAR_DISTANCE =					2;
const int MAX_STAR_DISTANCE =					20;
const int BRIGHT_STAR_CHANCE =					20;

const CG32bitPixel RGB_DEFAULT_SPACE_COLOR =	CG32bitPixel(0,0,8);

#ifdef DEBUG_PAINT_TIMINGS
static int g_iTimingCount = 0;
static DWORD g_dwTotalTime = 0;
#endif

class CStarshinePainter : public IThreadPoolTask
	{
	public:
		CStarshinePainter (CG32bitImage &Dest, int yStart, int cyHeight, const CG8bitSparseImage &Mask, int xMask, int yMask, CG32bitPixel rgbSpaceColor) :
				m_Dest(Dest),
				m_yStart(yStart),
				m_cyHeight(cyHeight),
				m_rgbSpaceColor(rgbSpaceColor),
				m_Mask(Mask),
				m_xMask(xMask),
				m_yMask(yMask)
			{ }

		virtual void Run (void)
			{
			m_Mask.MaskFill(m_Dest, 0, m_yStart, m_Dest.GetWidth(), m_cyHeight, m_xMask, m_yMask, m_rgbSpaceColor);
			};

	private:
		CG32bitImage &m_Dest;
		int m_yStart;
		int m_cyHeight;
		CG32bitPixel m_rgbSpaceColor;

		const CG8bitSparseImage &m_Mask;
		int m_xMask;
		int m_yMask;
	};

//	CSystemSpacePainter --------------------------------------------------------

CSystemSpacePainter::CSystemSpacePainter (void) :
		m_bStarshineEnabled(true),
		m_bInitialized(false),
		m_cxStarfield(-1),
		m_cyStarfield(-1),
		m_dwCurBackgroundUNID(0),
		m_pBackground(NULL)

//	CSystemSpacePainter constructor

	{
	m_Threads.Boot(sysGetProcessorCount());
	}

void CSystemSpacePainter::CleanUp (void)

//	CleanUp
//
//	Resets all caches

	{
	m_bInitialized = false;

	m_Starfield.DeleteAll();
	m_pBackground = NULL;

	m_dwCurBackgroundUNID = 0;
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

void CSystemSpacePainter::InitSpaceBackground (DWORD dwBackgroundUNID, CG32bitPixel rgbSpaceColor)

//	InitSpaceBackground
//
//	Make sure m_Background is initialize.

	{
	//	If we're initialized, nothing to do.

	if (m_dwCurBackgroundUNID == dwBackgroundUNID)
		return;

	//	Get the background image.

	if (m_pBackground == NULL)
		m_pBackground = g_pUniverse->GetLibraryBitmap(dwBackgroundUNID);

	//	Done

	m_dwCurBackgroundUNID = dwBackgroundUNID;
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
	ASSERT(m_pBackground);

#ifdef DEBUG_PAINT_TIMINGS
	DWORD dwStart = ::GetTickCount();
#endif

	int cxImage = m_pBackground->GetWidth();
	int cyImage = m_pBackground->GetHeight();

	//	Compute the paint positions

	int xOffset = ClockMod(xCenter / 4, cxImage);
	int yOffset = ClockMod(-yCenter / 4, cyImage);

	//	Tile across the entire screen

	PaintTiledBackground(Dest, Ctx.rcView, *m_pBackground, xOffset, yOffset);

	//	Starshine

	if (m_bStarshineEnabled 
			&& Ctx.rgbSpaceColor.GetAlpha() != 0
			&& Ctx.pVolumetricMask)
		{
		int cyLeft = Dest.GetHeight();
		int cyChunk = cyLeft / m_Threads.GetThreadCount();
		int yStart = 0;

		int xStarCenter = (int)(Ctx.pStar->GetPos().GetX() / g_KlicksPerPixel);
		int yStarCenter = (int)(Ctx.pStar->GetPos().GetY() / g_KlicksPerPixel);

		//	Compute the coordinates of the destination rect with respect to the
		//	volumetric mask.

		int xMask = (Ctx.pVolumetricMask->GetWidth() / 2) + xCenter - (RectWidth(Ctx.rcView) / 2) - xStarCenter;
		int yMask = (Ctx.pVolumetricMask->GetHeight() / 2) - yCenter - (RectHeight(Ctx.rcView) / 2) + yStarCenter;

		//	Start asynchronous tasks

		while (yStart < Dest.GetHeight())
			{
			int cyHeight = Min(Dest.GetHeight() - yStart, cyChunk);
			m_Threads.AddTask(new CStarshinePainter(Dest, yStart, cyHeight, *Ctx.pVolumetricMask, xMask, yMask + yStart, Ctx.rgbSpaceColor));

			yStart += cyHeight;
			}

		m_Threads.Run();
		}

#ifdef DEBUG_PAINT_TIMINGS
	g_dwTotalTime += ::GetTickCount() - dwStart;
	g_iTimingCount++;

	if ((g_iTimingCount % 100) == 0)
		::kernelDebugLogMessage("Space background time: %d.%02d", g_dwTotalTime / g_iTimingCount, (100 * g_dwTotalTime / g_iTimingCount) % 100);
#endif
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

		InitSpaceBackground((pType ? pType->GetBackgroundUNID() : UNID_DEFAULT_SYSTEM_BACKGROUND), Ctx.rgbSpaceColor);

		//	Get the absolute position of the center. This is just to get our
		//	parallax offset. It's OK if we overflow an integer (because we want
		//	to wrap around anyways).

		int xCenter = (int)(Ctx.pCenter->GetPos().GetX() / g_KlicksPerPixel);
		int yCenter = (int)(Ctx.pCenter->GetPos().GetY() / g_KlicksPerPixel);

		//	If we have an system background image, paint that.

		if (m_pBackground)
			PaintSpaceBackground(Dest, xCenter, yCenter, Ctx);

		//	Otherwise we just clear the rect with the space color

		else
			{
			if (!m_bInitialized)
				{
				CreateStarfield(RectWidth(Ctx.rcView), RectHeight(Ctx.rcView));
				m_bInitialized = true;
				}

			//	Compute the proper color

			CG32bitPixel rgbSolid = CG32bitPixel::Blend(CG32bitPixel(0, 0, 0), Ctx.rgbSpaceColor, Ctx.rgbSpaceColor.GetAlpha());

			//	Blank

			Dest.Fill(Ctx.rcView.left, Ctx.rcView.top, RectWidth(Ctx.rcView), RectHeight(Ctx.rcView), rgbSolid);

			//	Paint the star field on top

			PaintStarfield(Dest, Ctx.rcView, xCenter, yCenter, rgbSolid);
			}
		}
	}

void CSystemSpacePainter::PaintViewportMap (CG32bitImage &Dest, const RECT &rcView, CSystemType *pType, Metric rMapScale)

//	PaintViewportMap
//
//	Paints the map background

	{
	//	If we have a system background image, paint it.

	if (m_pBackground)
		PaintTiledBackground(Dest, rcView, *m_pBackground, 0, 0);

	//	Otherwise, default fill

	else
		Dest.Fill(rcView.left, rcView.top, RectWidth(rcView), RectHeight(rcView), RGB_DEFAULT_SPACE_COLOR);
	}

void CSystemSpacePainter::PaintTiledBackground (CG32bitImage &Dest, const RECT &rcView, CG32bitImage &Src, int xOffset, int yOffset)

//	PaintTiledBackground
//
//	Paints a tiled background

	{
	int cxImage = Src.GetWidth();
	int cyImage = Src.GetHeight();

	//	Tile across the entire screen

	int ySrc = yOffset;
	int cySrc = cyImage - ySrc;

	int yDest = rcView.top;
	int yDestEnd = rcView.bottom;

	while (yDest < yDestEnd)
		{
		int xSrc = xOffset;
		int cxSrc = cxImage - xSrc;

		int xDest = rcView.left;
		int xDestEnd = rcView.right;

		while (xDest < xDestEnd)
			{
			Dest.Blt(xSrc, ySrc, cxSrc, cySrc, Src, xDest, yDest);

			xDest += cxSrc;
			xSrc = 0;
			cxSrc = cxImage;
			}

		yDest += cySrc;
		ySrc = 0;
		cySrc = cyImage;
		}
	}
