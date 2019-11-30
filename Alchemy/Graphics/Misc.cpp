//	Misc.cpp
//
//	Implements miscellaneous functions
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

COLORREF strToCOLORREF(const CString &sColor)

//	strToCOLORREF
//
//	Converts a string of the 24-bit hex-representation "rrggbb" into an RGB COLORREF

	{
	const char *pPos = sColor.GetASCIIZPointer();

	//	Skip '#', if there

	if (*pPos == '#')
		pPos++;

	//	Parse value as hex

	DWORD dwValue = (DWORD)strParseIntOfBase(pPos,
			16,
			0xFFFFFF,
			NULL,
			NULL);

	//	Modify format appropriately

	return (COLORREF)(((dwValue & 0x00FF0000) >> 16) | ((dwValue & 0x0000FF00)) | ((dwValue & 0x000000FF) << 16));
	}
