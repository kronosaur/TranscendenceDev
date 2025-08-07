//	LibJPEGTurboUtil.cpp
//
//	Implements loading & saving JPEGs with LibJPEG-Turbo library
//	Copyright (c) 2022 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

ALERROR JPEGLoadFromFile(CString sFilename, DWORD dwFlags, HPALETTE hPalette, HBITMAP* rethBitmap)

//	JPEGLoadFromFile
//
//	Load from a JPEG file

{
	ALERROR error;
	CFileReadBlock JPEGFile(sFilename);

	if (error = JPEGFile.Open())
		return error;

	error = JPEGLoadFromMemory(JPEGFile.GetPointer(0, JPEGFile.GetLength()), JPEGFile.GetLength(), dwFlags, hPalette, rethBitmap);
	JPEGFile.Close();
	if (error)
		return error;

	return NOERROR;
}

ALERROR JPEGLoadFromMemory(char* pImage, int iSize, DWORD dwFlags, HPALETTE hPalette, HBITMAP* rethBitmap)

//	JPEGLoadFromMemory
//
//	Load from a memory stream

{
	ALERROR error;
	DWORD dwJpegSize = (DWORD)iSize;
	int iJpegSubsamp, width, height;

	tjhandle hJpegDecompressor = tjInitDecompress();

	if (TJERROR == tjDecompressHeader2(hJpegDecompressor, (unsigned char*)pImage, dwJpegSize, &width, &height, &iJpegSubsamp))
	{
		tjDestroy(hJpegDecompressor);
		return ERR_FAIL;
	}

	HBITMAP hBitmap;
	BYTE* p24BitPixel;//uninitualized pixel buffer for the DIB
	if (error = dibCreate24bitDIB(width,
		height,
		0,
		&hBitmap,
		&p24BitPixel))
	{
		tjDestroy(hJpegDecompressor);
		return error;
	}
	if (TJERROR == tjDecompress2(hJpegDecompressor, (unsigned char*)pImage, dwJpegSize, p24BitPixel, width, 0/*pitch*/, height, TJPF_BGR, TJFLAG_FASTDCT | TJFLAG_BOTTOMUP))//24bit dib format is bottom up, 8:8:8 BGR
	{
		tjDestroy(hJpegDecompressor);
		::DeleteObject(hBitmap);
		return ERR_FAIL;
	}

	tjDestroy(hJpegDecompressor);

	//	If we want a DIB, just return that; otherwise,
	//	convert to DDB and return that

	if (dwFlags & JPEG_LFR_DIB)
		*rethBitmap = hBitmap;
	else
	{
		HBITMAP hDDB;
		error = dibConvertToDDB(hBitmap, hPalette, &hDDB);
		::DeleteObject(hBitmap);
		if (error)
			return error;

		*rethBitmap = hDDB;
	}

	return NOERROR;
}

ALERROR JPEGLoadFromResource(HINSTANCE hInst, char* pszRes, DWORD dwFlags, HPALETTE hPalette, HBITMAP* rethBitmap)

//	JPEGLoadFromResource
//
//	Load from a resource

{
	HRSRC hRes;
	HGLOBAL hGlobalRes;
	void* pImage;
	int iSize;

	hRes = FindResource(hInst, (LPCSTR)pszRes, (LPCSTR)"JPEG");
	if (hRes == NULL)
		return ERR_NOTFOUND;

	iSize = SizeofResource(hInst, hRes);
	if (iSize == 0)
		return ERR_FAIL;

	hGlobalRes = LoadResource(hInst, hRes);
	if (hGlobalRes == NULL)
		return ERR_FAIL;

	pImage = LockResource(hGlobalRes);
	if (pImage == NULL)
		return ERR_FAIL;

	//	Done

	return JPEGLoadFromMemory((char*)pImage, iSize, dwFlags, hPalette, rethBitmap);
}

ALERROR JPEGSaveToMemory(HBITMAP hBitmap, int iQuality, CString* retsData)

//	JPEGSaveToMemory
//
//	Saves a bitmap image to a memory stream. NOTE: We expect a 24-bit BITMAP.

{
	ALERROR error;

	//	Get the DIB information

	int cxWidth, cyHeight, iStride;
	DWORD dwJpegSize;
	void* pBase;
	BITMAPINFOHEADER bmih;
	void* pBits;

	if (error = dibGetInfo(hBitmap, &cxWidth, &cyHeight, &pBase, &iStride, &bmih, &pBits))
		return error;

	//	Allocate a buffer large enough no matter what the compression

	unsigned char* pBuffer;

	//	We only support 24-bit bmps

	if (bmih.biBitCount != 24)
		return ERR_FAIL;

	//	Initialize library

	tjhandle hJpegCompressor = tjInitCompress();
	
	if (TJERROR == tjCompress2(hJpegCompressor, (const unsigned char*)pBits, cxWidth, 0, cyHeight, TJPF_RGB, &pBuffer, &dwJpegSize, TJ_411, iQuality, TJFLAG_FASTDCT | TJFLAG_BOTTOMUP))//24bit dib format is bottom up, 8:8:8 BGR
	{
		tjDestroy(hJpegCompressor);
		delete pBuffer;
		return ERR_FAIL;
	}

	//write buffer to ret CString
	retsData->Transcribe((const char*)pBuffer, dwJpegSize);

	//cleanup
	tjDestroy(hJpegCompressor);
	tjFree(pBuffer);

	return NOERROR;
}
