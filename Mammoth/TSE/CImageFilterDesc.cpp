//	CImageFilterDesc.cpp
//
//	CImageFilterDesc class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define BLACK_POINT_ATTRIB						CONSTLIT("blackPoint")
#define BLOOM_POINT_ATTRIB						CONSTLIT("bloomPoint")
#define BRIGHTNESS_ATTRIB						CONSTLIT("brightness")
#define GAMMA_ATTRIB							CONSTLIT("gamma")
#define HUE_ATTRIB								CONSTLIT("hue")
#define OPACITY_ATTRIB							CONSTLIT("opacity")
#define SATURATION_ATTRIB						CONSTLIT("saturation")
#define WHITE_POINT_ATTRIB						CONSTLIT("whitePoint")

constexpr int DEFAULT_BLOOM_POINT =		220;
constexpr Metric BLOOM_BLUR_RADIUS =	48.0;

static TStaticStringTable<TStaticStringEntry<CImageFilterDesc::ETypes>, 4> TYPE_TABLE = {
	"Bloom",				CImageFilterDesc::filterBloom,
	"Colorize",				CImageFilterDesc::filterColorize,
	"HueSaturation",		CImageFilterDesc::filterHueSaturation,
	"Levels",				CImageFilterDesc::filterLevels,
	};

CImageFilterDesc::CImageFilterDesc (void) :
		m_iType(filterNone),
		m_iData1(0),
		m_iData2(0),
		m_iData3(0),
		m_rData1(0.0),
		m_byOpacity(0)

//	CImageFilterDesc constructor

	{
	}

bool CImageFilterDesc::operator== (const CImageFilterDesc &Val) const

//	CImageFilterDesc operator ==

	{
	return (m_iType == Val.m_iType
			&& m_iData1 == Val.m_iData1
			&& m_iData2 == Val.m_iData2
			&& m_iData3 == Val.m_iData3
			&& m_rData1 == Val.m_rData1
			&& m_byOpacity == Val.m_byOpacity);
	}

void CImageFilterDesc::ApplyBloom (CG32bitImage &Dest, int iClipPoint) const

//	ApplyBloom
//
//	Bloom effect

	{
	RECT rcRect;
	rcRect.left = 0;
	rcRect.top = 0;
	rcRect.right = Dest.GetWidth();
	rcRect.bottom = Dest.GetHeight();

	//	Start by creating an 8 bit image that will contain the bloom

	CG8bitImage Bloom;
	Bloom.Create(Dest.GetWidth(), Dest.GetHeight());

	//	Now get all values above the clip point and put them in the bloom.

	CGFilter::Threshold(Dest, rcRect, (BYTE)iClipPoint, Bloom);

	//	Blur the bloom

	CGFilter::Blur(Bloom, rcRect, BLOOM_BLUR_RADIUS, Bloom);

	//	Now screen on the original

	BYTE *pSrcRow = Bloom.GetPixelPos(0, 0);
	BYTE *pSrcRowEnd = Bloom.GetPixelPos(0, Bloom.GetHeight());
	CG32bitPixel *pDestRow = Dest.GetPixelPos(0, 0);

	while (pSrcRow < pSrcRowEnd)
		{
		BYTE *pSrcPos = pSrcRow;
		BYTE *pSrcPosEnd = pSrcRow + Bloom.GetWidth();
		CG32bitPixel *pDestPos = pDestRow;

		while (pSrcPos < pSrcPosEnd)
			{
			BYTE byScreen = *pSrcPos;
			if (byScreen > 0 && pDestPos->GetAlpha() > 0)
				*pDestPos = CG32bitPixel::Screen(*pDestPos, byScreen);

			pSrcPos++;
			pDestPos++;
			}

		pSrcRow = Bloom.NextRow(pSrcRow);
		pDestRow = Dest.NextRow(pDestRow);
		}
	}

void CImageFilterDesc::ApplyColorize (CG32bitImage &Dest, int iHue, int iSaturation, int iBrightness, BYTE byOpacity) const

//	ApplyColorize
//
//	Colorize the image.

	{
	Metric rHue = (Metric)iHue;
	Metric rSaturation = (Metric)iSaturation / 100.0;

	CG32bitPixel *pDestRow = Dest.GetPixelPos(0, 0);
	CG32bitPixel *pDestRowEnd = Dest.GetPixelPos(0, Dest.GetHeight());

	while (pDestRow < pDestRowEnd)
		{
		CG32bitPixel *pDestPos = pDestRow;
		CG32bitPixel *pDestPosEnd = pDestRow + Dest.GetWidth();

		while (pDestPos < pDestPosEnd)
			{
			*pDestPos = CG32bitPixel::Colorize(*pDestPos, rHue, rSaturation);
			pDestPos++;
			}

		pDestRow = Dest.NextRow(pDestRow);
		}
	}

void CImageFilterDesc::ApplyHueSaturation (CG32bitImage &Dest, int iHue, int iSaturation, int iBrightness, BYTE byOpacity) const

//	ApplyHueSaturation
//
//	Adjust hue/saturation/brightness

	{
	}

void CImageFilterDesc::ApplyLevels (CG32bitImage &Dest, int iBlack, int iWhite, Metric rGamma) const

//	ApplyLevels
//
//	Adjust image levels.

	{
	}

void CImageFilterDesc::ApplyTo (CG32bitImage &Image) const

//	ApplyTo
//
//	Applies the filter to the given image.

	{
	switch (m_iType)
		{
		case filterBloom:
			ApplyBloom(Image, m_iData1);
			break;

		case filterColorize:
			ApplyColorize(Image, m_iData1, m_iData2, m_iData3, m_byOpacity);
			break;

		case filterHueSaturation:
			ApplyHueSaturation(Image, m_iData1, m_iData2, m_iData3, m_byOpacity);
			break;

		case filterLevels:
			ApplyLevels(Image, m_iData1, m_iData2, m_rData1);
			break;

		default:
			//	Nothing to do
			break;
		}
	}

ALERROR CImageFilterDesc::InitFromXML (SDesignLoadCtx &Ctx, const CXMLElement &Desc)

//	InitFromXML
//
//	Loads from an XML element

	{
	const TStaticStringEntry<ETypes> *pEntry = TYPE_TABLE.GetAt(Desc.GetTag());
	if (pEntry == NULL)
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Unknown filter type: %s"), Desc.GetTag());
		return ERR_FAIL;
		}

	m_iType = pEntry->Value;
	switch (m_iType)
		{
		case filterBloom:
			m_iData1 = Desc.GetAttributeIntegerBounded(BLOOM_POINT_ATTRIB, 1, 255, DEFAULT_BLOOM_POINT);
			break;

		case filterColorize:
			m_iData1 = Desc.GetAttributeIntegerBounded(HUE_ATTRIB, 0, 360, 0);
			m_iData2 = Desc.GetAttributeIntegerBounded(SATURATION_ATTRIB, 0, 100, 30);
			m_iData3 = Desc.GetAttributeIntegerBounded(BRIGHTNESS_ATTRIB, -100, 100, 0);
			break;

		case filterHueSaturation:
			m_iData1 = Desc.GetAttributeIntegerBounded(HUE_ATTRIB, -180, 180, 0);
			m_iData2 = Desc.GetAttributeIntegerBounded(SATURATION_ATTRIB, -100, 100, 0);
			m_iData3 = Desc.GetAttributeIntegerBounded(BRIGHTNESS_ATTRIB, -100, 100, 0);
			break;

		case filterLevels:
			m_iData1 = Desc.GetAttributeIntegerBounded(BLACK_POINT_ATTRIB, 0, 253, 0);
			m_iData2 = Desc.GetAttributeIntegerBounded(WHITE_POINT_ATTRIB, m_iData1 + 2, 255, 255);
			m_rData1 = Desc.GetAttributeDoubleBounded(GAMMA_ATTRIB, 0.01, 9.99, 1.0);
			break;
		}

	//	Several filters use opacity, so we load it here.

	DWORD dwOpacityLevel = Desc.GetAttributeIntegerBounded(OPACITY_ATTRIB, 0, 100, 100);
	m_byOpacity = (BYTE)(dwOpacityLevel * 255 / 100);

	return NOERROR;
	}
