//	Save.cpp
//
//	Implements saving JPEGs with Intel JPEG library
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include <windows.h>

#include "Kernel.h"
#include "KernelObjID.h"
#include "Graphics.h"
#include "IntelJPEGUtil.h"

#include "ijl.h"

ALERROR JPEGSaveToMemory (HBITMAP hBitmap, int iQuality, CString *retsData)

//	JPEGSaveToMemory
//
//	Saves a bitmap image to a memory stream. NOTE: We expect a 24-bit BITMAP.

	{
	ALERROR error;
	IJLERR jerr;

	//	Get the DIB information

	int cxWidth;
	int cyHeight;
	void *pBase;
	int iStride;
	BITMAPINFOHEADER bmih;
	void *pBits;

	if (error = dibGetInfo(hBitmap, &cxWidth, &cyHeight, &pBase, &iStride, &bmih, &pBits))
		return error;

	//	We only support 24-bit bmps

	if (bmih.biBitCount != 24)
		return ERR_FAIL;

	//	Initialize library

	JPEG_CORE_PROPERTIES jcprops;
	jerr = ijlInit(&jcprops);
	if (jerr != IJL_OK)
		return ERR_FAIL;

	//	Allocate a buffer large enough no matter what the compression

	char *pBuffer = retsData->GetWritePointer(cxWidth * cyHeight * 3);

	//	Prepare the properties

	DWORD dwPadBytes = IJL_DIB_PAD_BYTES(bmih.biWidth, 3);

	//	NOTE: Negative heights means bottom-up bits, which is the opposite
	//	of DIBs.

	jcprops.DIBWidth = bmih.biWidth;
	jcprops.DIBHeight = -bmih.biHeight;

	//	Set up information to write from the pixel buffer

	jcprops.DIBBytes = reinterpret_cast<BYTE*>(pBits);
	jcprops.DIBPadBytes = IJL_DIB_PAD_BYTES(bmih.biWidth, 3);
	jcprops.DIBChannels = 3;
	jcprops.DIBColor = IJL_BGR;

	jcprops.JPGWidth = cxWidth;
	jcprops.JPGHeight = cyHeight;
	jcprops.JPGFile = NULL;
	jcprops.JPGBytes = reinterpret_cast<BYTE *>(pBuffer);
	jcprops.JPGSizeBytes = retsData->GetLength();
	jcprops.JPGChannels = 3;
	jcprops.JPGColor = IJL_YCBCR;
	jcprops.JPGSubsampling = IJL_411;
	jcprops.jquality = iQuality;

	//	Write the image

	jerr = ijlWrite(&jcprops, IJL_JBUFF_WRITEWHOLEIMAGE);
	if (jerr != IJL_OK)
		{
		ijlFree(&jcprops);
		return ERR_FAIL;
		}

	//	Truncate to the proper size

	retsData->Truncate(jcprops.JPGSizeBytes);

	//	Done

	ijlFree(&jcprops);
	return NOERROR;
	}
