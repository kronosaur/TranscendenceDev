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
