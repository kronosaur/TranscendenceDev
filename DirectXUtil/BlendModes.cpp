//	CGDraw.cpp
//
//	CGDraw Class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

struct SBlendModeData
	{
	char *pszName;
	};

//	These must match the order of CGDraw::EBlendModes

static SBlendModeData BLEND_MODE_DATA[CGDraw::blendModeCount] = 
	{
		{	"normal", },
		{	"multiply", },
		{	"overlay", },
		{	"screen", },
	};

CGDraw::EBlendModes CGDraw::ParseBlendMode (const CString &sValue)

//	ParseBlendMode
//
//	Returns a blend mode for a name

	{
	int i;

	if (sValue.IsBlank())
		return blendNormal;

	for (i = 0; i < blendModeCount; i++)
		if (strEquals(sValue, CString(BLEND_MODE_DATA[i].pszName, -1, TRUE)))
			return (EBlendModes)i;

	return blendNone;
	}
