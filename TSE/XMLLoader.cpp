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

ALERROR CUniverse::InitLevelEncounterTables (void)

//	InitLevelEncounterTables
//
//	Initializes the m_LevelEncounterTables array based on the encounter
//	tables of all the stations for each level.

	{
	m_LevelEncounterTables.RemoveAll();

	for (int i = 1; i <= MAX_ITEM_LEVEL; i++)
		{
		CStructArray *pTable = new CStructArray(sizeof(SLevelEncounter), 8);

		for (int j = 0; j < GetStationTypeCount(); j++)
			{
			CStationType *pType = GetStationType(j);

			//	Figure out the frequency of an encounter from this station based
			//	on the frequency of the station at this level and the frequency
			//	of encounters for this station.

			int iEncounterFreq = pType->GetEncounterFrequency();
			int iStationFreq = pType->GetFrequencyByLevel(i);
			int iFreq = iEncounterFreq * iStationFreq / ftCommon;

			//	Add to the table

			if (iFreq > 0)
				{
				pTable->ExpandArray(pTable->GetCount(), 1);
				SLevelEncounter *pEntry = (SLevelEncounter *)pTable->GetStruct(pTable->GetCount()-1);

				pEntry->pType = pType;
				pEntry->iWeight = iFreq;
				pEntry->pBaseSovereign = pType->GetSovereign();
				pEntry->pTable = pType->GetEncountersTable();
				}
			}

		m_LevelEncounterTables.AppendObject(pTable, NULL);
		}

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
