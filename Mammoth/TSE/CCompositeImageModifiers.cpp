//	CCompositeImageModifiers.cpp
//
//	CCompositeImageModifiers class
//	Copyright (c) 2013 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

bool CCompositeImageModifiers::operator== (const CCompositeImageModifiers &Val) const

//	operator ==

	{
	return (m_iRotation == Val.m_iRotation
			&& m_pFilters == Val.m_pFilters
			&& m_rgbFadeColor == Val.m_rgbFadeColor
			&& m_wFadeOpacity == Val.m_wFadeOpacity
			&& m_bStationDamage == Val.m_bStationDamage
			&& m_bFullImage == Val.m_bFullImage);
	}

void CCompositeImageModifiers::Apply (SGetImageCtx &Ctx, CObjectImageArray *retImage) const

//	Apply
//
//	Apply modifiers to the image.

	{
	RECT rcNewImage;
	CG32bitImage *pNewDest = NULL;

	//	Station damage

	if (m_bStationDamage)
		{
		//	Create a blank bitmap

		if (pNewDest == NULL)
			pNewDest = CreateCopy(retImage, &rcNewImage);

		//	Keep our original mask, because the damage painting routines will
		//	destroy it.

		CG8bitImage Mask;
		Mask.CreateChannel(channelAlpha, *pNewDest);

		//	Add some large damage

		int iCount = (pNewDest->GetWidth() / 32) * (pNewDest->GetHeight() / 32);
		PaintDamage(*pNewDest, rcNewImage, iCount, &Ctx.GetUniverse().GetNamedPainter(CNamedEffects::painterLargeStationDamage));

		//	Add some medium damage

		iCount = (pNewDest->GetWidth() / 4) + (pNewDest->GetHeight() / 4);
		PaintDamage(*pNewDest, rcNewImage, iCount, &Ctx.GetUniverse().GetNamedPainter(CNamedEffects::painterMediumStationDamage));

		//	Reapply the mask to our image

		pNewDest->CopyChannel(channelAlpha, 0, 0, Mask.GetWidth(), Mask.GetHeight(), Mask, 0, 0);
		}

	//	Filters

	if (m_pFilters)
		{
		//	Create a blank bitmap

		if (pNewDest == NULL)
			pNewDest = CreateCopy(retImage, &rcNewImage);

		//	Apply filters

		m_pFilters->ApplyTo(*pNewDest);
		}

	//	Apply wash on top

	if (m_wFadeOpacity != 0)
		{
		//	Create a blank bitmap

		if (pNewDest == NULL)
			pNewDest = CreateCopy(retImage, &rcNewImage);

		//	Fill with color via our own mask

		pNewDest->FillMask(0, 
				0,
				pNewDest->GetWidth(),
				pNewDest->GetHeight(),
				*pNewDest,
				CG32bitPixel(m_rgbFadeColor, (BYTE)m_wFadeOpacity),
				0,
				0);
		}

	//	Replace the image (the result takes ownership of our image).

	if (pNewDest)
		{
		if (m_bFullImage || retImage->IsAnimated())
			retImage->SetImage(TSharedPtr<CObjectImage>(new CObjectImage(pNewDest, true)));
		else
			retImage->InitFromBitmap(pNewDest, rcNewImage, 0, 0, true);
		}
	}

CG32bitImage *CCompositeImageModifiers::CreateCopy (CObjectImageArray *pImage, RECT *retrcNewImage) const

//	CreateCopy
//
//	Creates a copy of the given image. Caller is reponsible for freeing.

	{
	if (m_bFullImage || pImage->IsAnimated())
		{
		CG32bitImage *pNewImage = new CG32bitImage(pImage->GetImage(NULL_STR));

		if (retrcNewImage)
			*retrcNewImage = pImage->GetImageRect();

		return pNewImage;
		}
	else
		{
		RECT rcImage = pImage->GetImageRect();

		int cxWidth = RectWidth(rcImage);
		int cyHeight = RectHeight(rcImage);
		CG32bitImage *pNewImage = new CG32bitImage;
		pNewImage->Create(cxWidth, cyHeight, CG32bitImage::alpha8);

		rcImage.left = 0;
		rcImage.top = 0;
		rcImage.right = cxWidth;
		rcImage.bottom = cyHeight;

		//	Start with undamaged image

		pImage->CopyImage(*pNewImage,
				0,
				0,
				0,
				0);

		if (retrcNewImage)
			*retrcNewImage = rcImage;

		return pNewImage;
		}
	}

void CCompositeImageModifiers::PaintDamage (CG32bitImage &Dest, const RECT &rcDest, int iCount, IEffectPainter *pPainter)

//	PaintDamage
//
//	Applies damage to the image

	{
	if (pPainter == NULL)
		return;

	//	Get some data

	int iVariantCount = pPainter->GetVariants();
	if (iVariantCount == 0)
		return;

	int iLifetime = pPainter->GetLifetime();
	if (iLifetime == 0)
		return;

	//	Create context

	SViewportPaintCtx Ctx;

	//	Paint damage

	for (int i = 0; i < iCount; i++)
		{
		//	Pick a random position

		int x = rcDest.left + mathRandom(0, RectWidth(rcDest)-1);
		int y = rcDest.top + mathRandom(0, RectHeight(rcDest)-1);

		//	Set some parameters

		Ctx.iTick = mathRandom(0, iLifetime - 1);
		Ctx.iVariant = mathRandom(0, iVariantCount - 1);
		Ctx.iRotation = mathRandom(0, 359);
		Ctx.iDestiny = mathRandom(0, 359);

		//	Paint

		pPainter->Paint(Dest, x, y, Ctx);
		}
	}
