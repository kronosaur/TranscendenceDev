//	Files.cpp
//
//	Utility functions for dealing with files.
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

bool dxLoadImageFile (const CString &sFilespec, HBITMAP *rethDIB, EBitmapTypes *retiType, CString *retsError)

//	dxLoadImageFile
//
//	Loads either a BMP or a JPEG and returns the type.

	{
	ALERROR error;

	//	We handle this case for optional files.

	if (sFilespec.IsBlank())
		{
		*rethDIB = NULL;
		if (retiType)
			*retiType = bitmapNone;
		return true;
		}

	//	Figure out our type

	CString sType = pathGetExtension(sFilespec);

	//	Load a JPEG

	HBITMAP hImage;
	if (strEquals(sType, CONSTLIT("jpg")))
		{
		if (error = ::JPEGLoadFromFile(sFilespec, JPEG_LFR_DIB, NULL, &hImage))
			{
			if (retsError)
				*retsError = CONSTLIT("Error loading JPEG file.");
			return false;
			}

		if (retiType)
			*retiType = bitmapRGB;
		}

	//	Otherwise, we assume it's a bitmap

	else
		{
		if (error = ::dibLoadFromFile(sFilespec, &hImage, retiType))
			{
			if (retsError)
				*retsError = CONSTLIT("Error loading BMP file.");
			return false;
			}
		}

	//	Done

	*rethDIB = hImage;

	return true;
	}
