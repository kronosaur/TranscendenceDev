//	CGPNG.cpp
//
//	CGPNG Class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "lodepng\lodepng.h"

bool CGPNG::Load (IReadBlock &Data, CG32bitImage &Image, CString *retsError)

//	Load
//
//	Loads a PNG into the given image. Returns FALSE if error.

	{
	unsigned char *pOutput;
	unsigned int dwWidth;
	unsigned int dwHeight;

	unsigned error = lodepng_decode32(&pOutput, &dwWidth, &dwHeight, (unsigned char *)Data.GetPointer(0), Data.GetLength());
	if (error)
		{
		if (retsError) *retsError = CString(lodepng_error_text(error));
		return false;
		}

	//	Copy the buffer to the new image now that we have the bits and the size.
	//	We also need to flip from RGBA to BGRA

	Image.Create(dwWidth, dwHeight, CG32bitImage::alpha8);
	CG32bitPixel *pDestRow = Image.GetPixelPos(0, 0);
	CG32bitPixel *pDestRowEnd = Image.GetPixelPos(0, dwHeight);

	DWORD *pSrcRow = (DWORD *)pOutput;

	while (pDestRow < pDestRowEnd)
		{
		CG32bitPixel *pDest = pDestRow;
		CG32bitPixel *pDestEnd = pDestRow + dwWidth;
		DWORD *pSrc = pSrcRow;

		while (pDest < pDestEnd)
			{
			BYTE *pPixel = (BYTE *)pSrc;

			//	PNG has transparency channel, but does not premultiply the color
			//	values, so we do that here.

			*pDest++ = CG32bitPixel::PreMult(pPixel[0], pPixel[1], pPixel[2], pPixel[3]);
			pSrc++;
			}

		pDestRow = Image.NextRow(pDestRow);
		pSrcRow += dwWidth;
		}

	free(pOutput);

	return true;
	}

bool CGPNG::Save (IWriteStream &Stream, const CG32bitImage &Image, CString *retsError)

//	Save
//
//	Encodes the given image as PNG and writes it to the stream. Returns FALSE if
//	error.

	{
	int cxWidth = Image.GetWidth();
	int cyHeight = Image.GetHeight();
	if (cxWidth <= 0 || cyHeight <= 0)
		{
		if (retsError) *retsError = CONSTLIT("Invalid image size.");
		return false;
		}

	DWORDLONG cbBuffer = (DWORDLONG)cxWidth * (DWORDLONG)cyHeight * 4;
	if (cbBuffer == 0)
		{
		if (retsError) *retsError = CONSTLIT("Invalid image size.");
		return false;
		}

	if (cbBuffer > (DWORDLONG)INT_MAX)
		{
		if (retsError) *retsError = CONSTLIT("Image too large to encode.");
		return false;
		}

	TArray<BYTE> RGBAData;
	RGBAData.InsertEmpty((int)cbBuffer);
	BYTE* pRGBA = &RGBAData[0];
	if (pRGBA == NULL)
		{
		if (retsError) *retsError = CONSTLIT("Unable to allocate image buffer.");
		return false;
		}

	BYTE *pDest = pRGBA;
	const CG32bitPixel *pSrcRow = Image.GetPixelPos(0, 0);
	const CG32bitPixel *pSrcRowEnd = Image.GetPixelPos(0, cyHeight);

	while (pSrcRow < pSrcRowEnd)
		{
		const CG32bitPixel *pSrc = pSrcRow;
		const CG32bitPixel *pSrcEnd = pSrcRow + cxWidth;

		while (pSrc < pSrcEnd)
			{
			CG32bitPixel rgbPixel = *pSrc++;

			BYTE byAlpha = rgbPixel.GetAlpha();
			BYTE byRed;
			BYTE byGreen;
			BYTE byBlue;

			if (byAlpha == 0)
				{
				byRed = 0;
				byGreen = 0;
				byBlue = 0;
				}
			else if (byAlpha == 0xff)
				{
				byRed = rgbPixel.GetRed();
				byGreen = rgbPixel.GetGreen();
				byBlue = rgbPixel.GetBlue();
				}
			else
				{
				int iAlpha = byAlpha;
				int iRed = (rgbPixel.GetRed() * 255 + (iAlpha / 2)) / iAlpha;
				int iGreen = (rgbPixel.GetGreen() * 255 + (iAlpha / 2)) / iAlpha;
				int iBlue = (rgbPixel.GetBlue() * 255 + (iAlpha / 2)) / iAlpha;

				if (iRed > 255) iRed = 255;
				if (iGreen > 255) iGreen = 255;
				if (iBlue > 255) iBlue = 255;

				byRed = (BYTE)iRed;
				byGreen = (BYTE)iGreen;
				byBlue = (BYTE)iBlue;
				}

			*pDest++ = byRed;
			*pDest++ = byGreen;
			*pDest++ = byBlue;
			*pDest++ = byAlpha;
			}

		pSrcRow = (const CG32bitPixel *)Image.NextRow(const_cast<CG32bitPixel *>(pSrcRow));
		}

	unsigned char *pPNG = NULL;
	size_t cbPNG = 0;
	unsigned int dwWidth = (unsigned int)cxWidth;
	unsigned int dwHeight = (unsigned int)cyHeight;

	unsigned int error = lodepng_encode32(&pPNG, &cbPNG, (const unsigned char *)pRGBA, dwWidth, dwHeight);
	if (error)
		{
		if (pPNG) free(pPNG);
		if (retsError) *retsError = CString(lodepng_error_text(error));
		return false;
		}

	if (cbPNG > (size_t)INT_MAX)
		{
		free(pPNG);
		if (retsError) *retsError = CONSTLIT("Encoded PNG too large to write.");
		return false;
		}

	if (Stream.Write((const char *)pPNG, (int)cbPNG) != NOERROR)
		{
		free(pPNG);
		if (retsError) *retsError = CONSTLIT("Unable to write PNG data.");
		return false;
		}

	free(pPNG);

	return true;
	}
