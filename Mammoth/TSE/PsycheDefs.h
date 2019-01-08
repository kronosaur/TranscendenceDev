//	PsycheDefs.h
//
//	CCrewPsyche class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

struct SArchetypeDef
	{
	char *pszID;							//	ID for getting/setting
	char *pszName;							//	Human-readable name
	};

static SArchetypeDef ARCHETYPE_DEFS[] =
	{
		{	NULL, NULL }
	};

const int ARCHETYPE_DEFS_COUNT = (sizeof(ARCHETYPE_DEFS) / sizeof(ARCHETYPE_DEFS[0]));