//	CStationEncounterTable.cpp
//
//	CStationEncounterTable class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define MATCH_ALL						CONSTLIT("*")

bool CStationEncounterTable::Init (CSystem &System, const CStationEncounterOverrideTable &EncounterTable, const SInitCtx &Options, CString *retsError, bool *retbAddToCache)

//	Init
//
//	Initializes an encounter table given the criteria in Options.

	{
	CUniverse &Universe = System.GetUniverse();
	CTopologyNode *pNode = System.GetTopology();

	//	Loop over all station types

	int iLevel = System.GetLevel();
	int iCount = Universe.GetStationTypeCount();

	//	Initialize the table
	//
	//	If we're including all it usually means that we're by-passing the normal
	//	encounter probabilities and going straight by station criteria.
	//	(We do this, e.g., to create random asteroids of appropriate type.)

	if (Options.bIncludeAll)
		{
		//	If we're including all, then we ignore the levelFrequency property
		//	of a type.

		for (int i = 0; i < iCount; i++)
			Universe.GetStationType(i)->SetTempChance(1000);
		}
	else
		{
		for (int i = 0; i < iCount; i++)
			{
			CStationType *pType = Universe.GetStationType(i);
			const CStationEncounterDesc &Desc = EncounterTable.GetEncounterDesc(*pType);
			pType->SetTempChance((1000 / ftCommon) * pType->GetFrequencyForSystem(System, Desc));
			}
		}

	//	Loop over each part of the criteria and refine the table

	if (!Options.sCriteria.IsBlank() && !strEquals(Options.sCriteria, MATCH_ALL))
		{
		//	Parse station criteria if we've got it.

		CAffinityCriteria StationCriteria;
		if (StationCriteria.Parse(Options.sCriteria, retsError) != NOERROR)
			return false;

		for (int i = 0; i < iCount; i++)
			{
			CStationType *pType = Universe.GetStationType(i);
			if (pType->GetTempChance())
				{
				int iAdj = pType->CalcAffinity(StationCriteria);
				pType->SetTempChance((pType->GetTempChance() * iAdj) / 1000);
				}
			}
		}

	//	Loop over each station type and adjust for the location that
	//	we want to create the station at

	if (!Options.sLocationAttribs.IsBlank() && !strEquals(Options.sLocationAttribs, MATCH_ALL))
		{
		for (int i = 0; i < iCount; i++)
			{
			CStationType *pType = Universe.GetStationType(i);
			if (pType->GetTempChance())
				{
				const CStationEncounterDesc &Desc = EncounterTable.GetEncounterDesc(*pType);

				const CAffinityCriteria &LocationCriteria = Desc.GetLocationCriteria();
				int iAdj = System.CalcLocationAffinity(LocationCriteria, Options.sExtraLocationAttribs, Options.vPos);
				pType->SetTempChance((pType->GetTempChance() * iAdj) / 1000);
				}
			}
		}

	//	Check to see if any of the selected stations has priority (because of node
	//	minimums). If so, then we need to zero out any other stations.

	bool bPrioritizeRequiredEncounters = false;
	if (pNode)
		{
		for (int i = 0; i < iCount; i++)
			{
			CStationType *pType = Universe.GetStationType(i);

			//	If we need a minimum number of stations in this node, then we
			//	prioritize these types.

			if (pType->GetTempChance() > 0
					&& pType->GetEncounterRequired(*pNode, EncounterTable.GetEncounterDesc(*pType)) > 0)
				{
				bPrioritizeRequiredEncounters = true;
				break;
				}
			}

		//	If we're prioritizing required encounters, then zero-out any probabilities
		//	for non-required types

		if (bPrioritizeRequiredEncounters)
			{
			for (int i = 0; i < iCount; i++)
				{
				CStationType *pType = Universe.GetStationType(i);
				if (pType->GetTempChance() > 0 
						&& pType->GetEncounterRequired(*pNode, EncounterTable.GetEncounterDesc(*pType)) == 0)
					pType->SetTempChance(0);
				}
			}
		}

	//	Now that we've initialized all the types, return the ones with a non-zero
	//	chance.

	DeleteAll();
	for (int i = 0; i < iCount; i++)
		{
		CStationType *pType = Universe.GetStationType(i);
		if (pType->GetTempChance())
			Insert(*pType, pType->GetTempChance());
		}

	//	Done

	*retbAddToCache = !bPrioritizeRequiredEncounters;

	return true;
	}
