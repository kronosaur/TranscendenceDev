//	CFireAndSmokePainter.cpp
//
//	CFireAndSmokePainter object
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

const CG32bitPixel FLAME_CORE_COLOR =			CG32bitPixel(255,241,230);
const CG32bitPixel FLAME_MIDDLE_COLOR =			CG32bitPixel(255,208,0);
const CG32bitPixel FLAME_OUTER_COLOR =			CG32bitPixel(255,59,27);

#define PAINT_GASEOUS_PARTICLE(Dest,x,y,iWidth,rgbColor,iFade,iFade2)	\
	{	\
	switch (iWidth)	\
		{	\
		case 0:	\
			Dest.SetPixelTrans((x), (y), (rgbColor), (BYTE)(iFade));	\
			break;	\
\
		case 1:	\
			Dest.SetPixelTrans((x), (y), (rgbColor), (BYTE)(iFade));	\
			Dest.SetPixelTrans((x) + 1, (y), (rgbColor), (BYTE)(iFade2));	\
			Dest.SetPixelTrans((x), (y) + 1, (rgbColor), (BYTE)(iFade2));	\
			break;	\
\
		case 2:	\
			Dest.SetPixelTrans((x), (y), (rgbColor), (BYTE)(iFade));	\
			Dest.SetPixelTrans((x) + 1, (y), (rgbColor), (BYTE)(iFade2));	\
			Dest.SetPixelTrans((x), (y) + 1, (rgbColor), (BYTE)(iFade2));	\
			Dest.SetPixelTrans((x) - 1, (y), (rgbColor), (BYTE)(iFade2));	\
			Dest.SetPixelTrans((x), (y) - 1, (rgbColor), (BYTE)(iFade2));	\
			break;	\
\
		case 3:	\
			Dest.SetPixelTrans((x), (y), (rgbColor), (BYTE)(iFade));	\
			Dest.SetPixelTrans((x) + 1, (y), (rgbColor), (BYTE)(iFade));	\
			Dest.SetPixelTrans((x), (y) + 1, (rgbColor), (BYTE)(iFade));	\
			Dest.SetPixelTrans((x) - 1, (y), (rgbColor), (BYTE)(iFade));	\
			Dest.SetPixelTrans((x), (y) - 1, (rgbColor), (BYTE)(iFade));	\
			Dest.SetPixelTrans((x) + 1, (y) + 1, (rgbColor), (BYTE)(iFade2));	\
			Dest.SetPixelTrans((x) + 1, (y) - 1, (rgbColor), (BYTE)(iFade2));	\
			Dest.SetPixelTrans((x) - 1, (y) + 1, (rgbColor), (BYTE)(iFade2));	\
			Dest.SetPixelTrans((x) - 1, (y) - 1, (rgbColor), (BYTE)(iFade2));	\
			break;	\
\
		default:	\
			CGDraw::Circle(Dest, (x), (y), ((iWidth) + 1) / 2, CG32bitPixel((rgbColor), (BYTE)(iFade)));	\
			break;	\
		}	\
	}

CFireAndSmokePainter::CFireAndSmokePainter (EStyles iStyle, int iLifetime, int iMinWidth, int iMaxWidth)

//	CFireAndSmokePainter constructor

	{
	switch (iStyle)
		{
		case styleSmoke:
			Init(iLifetime, iMinWidth, iMaxWidth, 1, iLifetime / 6, iLifetime / 4, 60);
			break;

		default:
			Init(iLifetime, iMinWidth, iMaxWidth, iLifetime / 6, iLifetime / 2, 3 * iLifetime / 4, 80);
			break;
		}
	}

CFireAndSmokePainter::CFireAndSmokePainter (int iLifetime, int iMinWidth, int iMaxWidth, int iCore, int iFlame, int iSmoke, int iSmokeBrightness)

//	CFireAndSmokePainter constructor

	{
	Init(iLifetime, iMinWidth, iMaxWidth, iCore, iFlame, iSmoke, iSmokeBrightness);
	}

void CFireAndSmokePainter::Init (int iLifetime, int iMinWidth, int iMaxWidth, int iCore, int iFlame, int iSmoke, int iSmokeBrightness)

//	Init
//
//	Initialize from parameters

	{
	m_iLifetime = iLifetime;

	m_iMinWidth = Max(1, iMinWidth);
	iMaxWidth = Max(m_iMinWidth, iMaxWidth);
	m_iWidthRange = (iMaxWidth - m_iMinWidth) + 1;

	m_iCore = Max(iCore, 1);
	m_iFlame = Max(iFlame, iCore + 1);
	m_iSmoke = Max(iSmoke, iFlame + 1);
	m_iSmokeBrightness = iSmokeBrightness;
	}

void CFireAndSmokePainter::Paint (CG32bitImage &Dest, int x, int y, int iAge, int iDestiny) const

//	Paint
//
//	Paints a single particle of the given age.

	{
	//	Compute properties of the particle based on its life

	CG32bitPixel rgbColor = 0;
	int iFade = 0;
	int iFade2 = 0;
	int iWidth = 0;

	if (m_iLifetime > 0)
		{
		//	Particle fades out over time

		int iLifeLeft = Max(0, m_iLifetime - iAge);
		iFade = Max(20, Min(255, (255 * iLifeLeft / m_iLifetime)));
		iFade2 = iFade / 2;

		//	Particle grows over time

		iWidth = m_iMinWidth + (m_iWidthRange * iAge / m_iLifetime);

		//	Smoke color

		int iDarkness = Min(255, m_iSmokeBrightness + (2 * (iDestiny % 25)));
		CG32bitPixel rgbSmokeColor = CG32bitPixel::FromGrayscale(iDarkness);

		//	Some particles are gray

		CG32bitPixel rgbFadeColor;
		if ((iDestiny % 4) != 0)
			rgbFadeColor = FLAME_OUTER_COLOR;
		else
			rgbFadeColor = rgbSmokeColor;

		//	Particle color changes over time

		if (iAge <= m_iCore)
			rgbColor = CG32bitPixel::Fade(FLAME_CORE_COLOR,
					FLAME_MIDDLE_COLOR,
					100 * iAge / m_iCore);
		else if (iAge <= m_iFlame)
			rgbColor = CG32bitPixel::Fade(FLAME_MIDDLE_COLOR,
					rgbFadeColor,
					100 * (iAge - m_iCore) / (m_iFlame - m_iCore));
		else if (iAge <= m_iSmoke)
			rgbColor = CG32bitPixel::Fade(rgbFadeColor,
					rgbSmokeColor,
					100 * (iAge - m_iFlame) / (m_iSmoke - m_iFlame));
		else
			rgbColor = rgbSmokeColor;
		}

	//	Paint the particle

	PAINT_GASEOUS_PARTICLE(Dest, x, y, iWidth, rgbColor, iFade, iFade2);
	}

//	CGaseousPainter ------------------------------------------------------------

CGaseousPainter::CGaseousPainter (int iLifetime, int iMinWidth, int iMaxWidth, CG32bitPixel rgbPrimaryColor, CG32bitPixel rgbSecondaryColor)

//	CGaseousPainter constructor

	{
	m_iLifetime = iLifetime;

	m_iMinWidth = Max(1, iMinWidth);
	iMaxWidth = Max(m_iMinWidth, iMaxWidth);
	m_iWidthRange = (iMaxWidth - m_iMinWidth) + 1;

	m_rgbPrimary = rgbPrimaryColor;
	m_rgbSecondary = rgbSecondaryColor;
	}

void CGaseousPainter::Paint (CG32bitImage &Dest, int x, int y, int iAge) const

//	Paint
//
//	Paint a gaseous particle

	{
	//	Compute properties of the particle based on its life

	CG32bitPixel rgbColor = 0;
	int iFade = 0;
	int iFade2 = 0;
	int iWidth = 0;

	if (m_iLifetime > 0)
		{
		//	Particle fades out over time

		int iLifeLeft = Max(0, m_iLifetime - iAge);
		iFade = Max(20, Min(255, (255 * iLifeLeft / m_iLifetime)));
		iFade2 = iFade / 2;

		//	Particle grows over time

		iWidth = m_iMinWidth + (m_iWidthRange * iAge / m_iLifetime);

		//	Particle color fades from primary to secondary

		rgbColor = CG32bitPixel::Fade(m_rgbPrimary, m_rgbSecondary, 100 * iAge / m_iLifetime);
		}

	//	Paint the particle

	PAINT_GASEOUS_PARTICLE(Dest, x, y, iWidth, rgbColor, iFade, iFade2);
	}

//	CGlitterPainter ------------------------------------------------------------

CGlitterPainter::CGlitterPainter (int iWidth, CG32bitPixel rgbPrimary, CG32bitPixel rgbSecondary) :
		m_iWidth(iWidth),
		m_rgbPrimary(rgbPrimary),
		m_rgbSecondary(rgbSecondary)

//	CGlitterPainter constructor

	{
	}

void CGlitterPainter::Paint (CG32bitImage &Dest, int x, int y, int iRotation) const

//	Paint
//
//	Paint a glittering particle

	{
	const int HIGHLIGHT_ANGLE = 135;
	const int HIGHLIGHT_RANGE = 30;

	//	Color flips to secondary color at certain angles

	int iBearing = Absolute(AngleBearing(HIGHLIGHT_ANGLE, iRotation));
	CG32bitPixel rgbColor = (iBearing < HIGHLIGHT_RANGE ? CG32bitPixel::Fade(m_rgbSecondary, m_rgbPrimary, iBearing * 100 / HIGHLIGHT_RANGE) : m_rgbPrimary);

	//	Draw at appropriate size

	switch (m_iWidth)
		{
		case 0:
			Dest.SetPixel(x, y, rgbColor);
			break;

		case 1:
			Dest.SetPixel(x, y, rgbColor);
			Dest.SetPixelTrans(x + 1, y, rgbColor, 0x80);
			Dest.SetPixelTrans(x, y + 1, rgbColor, 0x80);
			break;

		case 2:
			Dest.SetPixel(x, y, rgbColor);
			Dest.SetPixelTrans(x + 1, y, rgbColor, 0x80);
			Dest.SetPixelTrans(x, y + 1, rgbColor, 0x80);
			Dest.SetPixelTrans(x - 1, y, rgbColor, 0x80);
			Dest.SetPixelTrans(x, y - 1, rgbColor, 0x80);
			break;

		case 3:
			Dest.SetPixel(x, y, rgbColor);
			Dest.SetPixel(x + 1, y, rgbColor);
			Dest.SetPixel(x, y + 1, rgbColor);
			Dest.SetPixel(x - 1, y, rgbColor);
			Dest.SetPixel(x, y - 1, rgbColor);
			Dest.SetPixelTrans(x + 1, y + 1, rgbColor, 0x80);
			Dest.SetPixelTrans(x + 1, y - 1, rgbColor, 0x80);
			Dest.SetPixelTrans(x - 1, y + 1, rgbColor, 0x80);
			Dest.SetPixelTrans(x - 1, y - 1, rgbColor, 0x80);
			break;

		default:
			CGDraw::Circle(Dest, x, y, (m_iWidth + 1) / 2, rgbColor);
			break;
		}
	}
