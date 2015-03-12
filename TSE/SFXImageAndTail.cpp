//	SFXImageAndTail.cpp
//
//	Paints and image with a trailing tail

#include "PreComp.h"


#define VARIANTS_ATTRIB							CONSTLIT("imageVariants")
#define RANDOM_START_FRAME_ATTRIB				CONSTLIT("randomStartFrame")

#define LENGTH_ATTRIB							(CONSTLIT("length"))
#define WIDTH_ATTRIB							(CONSTLIT("width"))
#define PRIMARY_COLOR_ATTRIB					(CONSTLIT("primaryColor"))
#define SECONDARY_COLOR_ATTRIB					(CONSTLIT("secondaryColor"))

ALERROR CImageAndTailEffectCreator::OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID)

//	OnEffectCreateFromXML
//
//	Initialize type-specific data

	{
	ALERROR error;

	if (error = m_Image.InitFromXML(Ctx, pDesc))
		return error;

	m_iLifetime = m_Image.GetFrameCount() * m_Image.GetTicksPerFrame();
	m_iVariants = pDesc->GetAttributeInteger(VARIANTS_ATTRIB);
	if (m_iVariants <= 0)
		m_iVariants = 1;
	m_bRandomStartFrame = pDesc->GetAttributeBool(RANDOM_START_FRAME_ATTRIB);

	m_iLength = pDesc->GetAttributeInteger(LENGTH_ATTRIB);
	m_iWidth = pDesc->GetAttributeInteger(WIDTH_ATTRIB);
	m_rgbPrimaryColor = ::LoadRGBColor(pDesc->GetAttribute(PRIMARY_COLOR_ATTRIB));
	m_rgbSecondaryColor = ::LoadRGBColor(pDesc->GetAttribute(SECONDARY_COLOR_ATTRIB));

	return NOERROR;
	}

ALERROR CImageAndTailEffectCreator::OnEffectBindDesign (SDesignLoadCtx &Ctx)

//	OnEffectBindDesign
//
//	Resolve loading

	{
	ALERROR error;

	if (error = m_Image.OnDesignLoadComplete(Ctx))
		return error;

	return NOERROR;
	}

void CImageAndTailEffectCreator::Paint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	Paint
//
//	Paint the effect

	{
	int iTick = Ctx.iTick;
	if (m_bRandomStartFrame)
		iTick += Ctx.iDestiny;

	//	Paint the tail

	CG32bitPixel rgbStart, rgbEnd;
	int xStart, yStart;
	CVector vTail = PolarToVector(Ctx.iRotation, m_iLength);

	if (m_iWidth >= 3)
		{
		xStart = x - (int)(vTail.GetX() + 0.5);
		yStart = y + (int)(vTail.GetY() + 0.5);

		rgbStart = CG32bitPixel(m_rgbSecondaryColor, 155);
		rgbEnd = CG32bitPixel(m_rgbSecondaryColor, 0);
		CGDraw::LineGradient(Dest, xStart, yStart, x, y, m_iWidth, rgbEnd, rgbStart);
		}

	vTail = vTail / 2.0;
	xStart = x - (int)(vTail.GetX() + 0.5);
	yStart = y + (int)(vTail.GetY() + 0.5);

	rgbStart = m_rgbPrimaryColor;
	rgbEnd = CG32bitPixel(m_rgbSecondaryColor, 200);
	CGDraw::LineGradient(Dest, xStart, yStart, x, y, Max(1, m_iWidth / 2), rgbEnd, rgbStart);

	//	Paint the image

	m_Image.PaintImage(Dest, x, y, iTick, (Ctx.iVariant % m_iVariants));
	}

bool CImageAndTailEffectCreator::PointInImage (int x, int y, int iTick, int iVariant, int iRotation) const

//	PointInImage
//
//	Returns TRUE if the given point is in the image

	{
	return m_Image.PointInImage(x, y, iTick, (iVariant % m_iVariants));
	}
