//	CGDraw.cpp
//
//	CGDraw Class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CGDraw::Text (CG32bitImage &Dest, const CVector &vPos, const CString &sText, const CG16bitFont &Font, CG32bitPixel rgbColor, EBlendModes iMode, Metric rRotation, DWORD dwFlags)

//	Text
//
//	Draw a single line of text

	{
	//	Create a bitmap containing the entire line of text (we need to do this
	//	because we're going to transform the result, and this is more efficient
	//	than trying to transform each character separately).

	int cyText;
	int cxText = Font.MeasureText(sText, &cyText, true);
	CG8bitImage Line;
	Line.Create(cxText, cyText, 0);

	//	Get the source font image. We expect it to have an alpha channel.

	const CG16bitImage &FontImage = Font.GetCharacterImage();
	BYTE *pFontAlpha = FontImage.GetAlphaRow(0);
	if (pFontAlpha == NULL)
		return;

	int iFontAlphaRow = FontImage.GetAlphaRow(1) - FontImage.GetAlphaRow(0);

	//	Blt the characters to our line image

	BYTE *pDestStart = Line.GetPixelPos(0, 0);

	char *pPos = sText.GetASCIIZPointer();
	char *pPosEnd = pPos + sText.GetLength();
	int xPos = 0;
	while (pPos < pPosEnd)
		{
		int xSrc, ySrc, cxSrc, cySrc, cxAdvance;

		const CG16bitImage &Char = Font.GetCharacterImage(*pPos, &xSrc, &ySrc, &cxSrc, &cySrc, &cxAdvance);

		//	Blt

		BYTE *pSrcRow = pFontAlpha + (ySrc * iFontAlphaRow) + xSrc;
		BYTE *pDestRow = Line.GetPixelPos(xPos, 0);
		BYTE *pDestRowEnd = Line.GetPixelPos(xPos, cyText);
		while (pDestRow < pDestRowEnd)
			{
			BYTE *pSrc = pSrcRow;
			BYTE *pDest = pDestRow;
			BYTE *pDestEnd = pDest + cxSrc;
			while (pDest < pDestEnd)
				*pDest++ = *pSrc++;

			//	Next

			pDestRow = Line.NextRow(pDestRow);
			pSrcRow += iFontAlphaRow;
			}

		//	Next

		xPos += cxAdvance;
		pPos++;
		}

	//	Now compute the upper-left point where we should draw the text

	CVector vText;
	if (dwFlags & TEXT_ALIGN_CENTER)
		vText = vPos + CVector::FromPolarInv(rRotation + PI, (0.5 * cxText));

	else if (dwFlags & TEXT_ALIGN_RIGHT)
		vText = vPos + CVector::FromPolarInv(rRotation + PI, cxText);

	else
		vText = vPos;

	//	Blt is from center of image.

	vText = vText + CVector(0.5 * Line.GetWidth(), 0.5 * Line.GetHeight()).Rotate(-rRotation);

	CVector vScale(1.0, 1.0);

	//	Blt the text based on the mode

	switch (iMode)
		{
		case blendNormal:
			{
			if (rgbColor.GetAlpha() == 0xff)
				{
				TFillImageSolid<CGBlendCopy> Painter(rgbColor);
				Painter.FillMaskTransformed(Dest, vText, vScale, rRotation, Line, 0, 0, Line.GetWidth(), Line.GetHeight());
				}
			else
				{
				TFillImageSolid<CGBlendBlend> Painter(rgbColor);
				Painter.FillMaskTransformed(Dest, vText, vScale, rRotation, Line, 0, 0, Line.GetWidth(), Line.GetHeight());
				}
			break;
			}

		case blendHardLight:
			{
			TFillImageSolid<CGBlendHardLight> Painter(rgbColor);
			Painter.FillMaskTransformed(Dest, vText, vScale, rRotation, Line, 0, 0, Line.GetWidth(), Line.GetHeight());
			break;
			}

		case blendScreen:
			{
			TFillImageSolid<CGBlendScreen> Painter(rgbColor);
			Painter.FillMaskTransformed(Dest, vText, vScale, rRotation, Line, 0, 0, Line.GetWidth(), Line.GetHeight());
			break;
			}

		case blendCompositeNormal:
			{
			if (rgbColor.GetAlpha() == 0xff)
				{
				TFillImageSolid<CGBlendCopy> Painter(rgbColor);
				Painter.FillMaskTransformed(Dest, vText, vScale, rRotation, Line, 0, 0, Line.GetWidth(), Line.GetHeight());
				}
			else
				{
				TFillImageSolid<CGBlendComposite> Painter(rgbColor);
				Painter.FillMaskTransformed(Dest, vText, vScale, rRotation, Line, 0, 0, Line.GetWidth(), Line.GetHeight());
				}
			break;
			}
		}
	}
