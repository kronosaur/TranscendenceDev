//	XMLLoader.cpp
//
//	Initializes the universe from XML

#include "PreComp.h"

#define STORAGE_FILESPEC					CONSTLIT("Storage.xml")

ALERROR CUniverse::InitCodeChain (const TArray<SPrimitiveDefTable> &CCPrimitives)

//	InitCodeChain
//
//	Initialize CodeChain

	{
	ALERROR error;
	int i;

	//	Initialize code chain

	if (error = m_CC.Boot())
		return error;

	if (error = InitCodeChainPrimitives())
		return error;

	//	Register all additional primitives

	for (i = 0; i < CCPrimitives.GetCount(); i++)
		{
		if (error = m_CC.RegisterPrimitives(CCPrimitives[i]))
			return error;
		}

	//	Now that we've defined all primitives, we create a clone of the global
	//	symbol table. We restore back to this pristine state before each bind.

	ASSERT(m_pSavedGlobalSymbols == NULL);
	m_pSavedGlobalSymbols = m_CC.GetGlobals()->Clone(&m_CC);

	return NOERROR;
	}

ALERROR CUniverse::InitDeviceStorage (CString *retsError)

//	InitDeviceStorage
//
//	Initializes cross-game local storage

	{
	ALERROR error;

	if (error = m_DeviceStorage.Load(STORAGE_FILESPEC, retsError))
		return error;

	return NOERROR;
	}

ALERROR CUniverse::SaveDeviceStorage (void)

//	SaveDeviceStorage
//
//	Saves the cross-game device storage file.

	{
	ALERROR error;

	if (error = m_DeviceStorage.Save(STORAGE_FILESPEC))
		return error;

	return NOERROR;
	}
