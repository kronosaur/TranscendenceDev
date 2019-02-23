//	LoadApp.cpp
//
//	Implementation of LoadApp method for CCodeChain

#include "PreComp.h"

#define OUTPUT_SIGNATURE							'CCDO'
#define OUTPUT_VERSION								1

typedef struct
	{
	DWORD dwSignature;								//	Always 'CCDO'
	DWORD dwVersion;								//	Version of format
	DWORD dwEntryCount;								//	Number of entries
	} HEADERSTRUCT, *PHEADERSTRUCT;

