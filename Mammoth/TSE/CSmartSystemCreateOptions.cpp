//	CSmartSystemCreateOptions.cpp
//
//	CSmartSystemCreateOptions class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define DEBUG_BREAK_ATTRIB				CONSTLIT("debugBreak")
#define OPTIONAL_ATTRIB					CONSTLIT("optional")

#define OPTION_3DEXTRA					CONSTLIT("3Dextra")

ALERROR CSmartSystemCreateOptions::Init (const CXMLElement &XMLDesc)

//	Init
//
//	Initializes standard options in SSystemCreateCtx from the XML descriptor.
//	Restores the options when the class is destroyed.

	{
	CString sOptional;
	if (XMLDesc.FindAttribute(OPTIONAL_ATTRIB, &sOptional))
		{
		if (strEquals(sOptional, OPTION_3DEXTRA))
			m_Ctx.bIs3DExtra = true;
		else
			{
			m_Ctx.sError = strPatternSubst(CONSTLIT("Invalid optional: %s"), sOptional);
			return ERR_FAIL;
			}
		}

#ifdef DEBUG
	if (XMLDesc.GetAttributeBool(DEBUG_BREAK_ATTRIB))
		{
		DebugBreak();
		}
#endif

	return NOERROR;
	}
