//	CCrewPsyche.cpp
//
//	CCrewPsyche class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "PsycheDefs.h"

CCrewPsyche::CCrewPsyche (void) :
		m_dwArchetype(archetypeNone),
		m_dwBelief(0),
		m_dwCohesion(0),
		m_dwLoyalty(0)

//	CCrewPsyche constructor

	{
	}

DWORD CCrewPsyche::GenerateNormalLevel (void)

//	GenerateNormalLevel
//
//	Generates a random level that clusters around neutral.

	{
	double rValue = SCrewMetrics::NEUTRAL_LEVEL + (mathRandomGaussian() * (SCrewMetrics::NEUTRAL_HIGH_LEVEL - SCrewMetrics::NEUTRAL_LOW_LEVEL));

	if (rValue <= (double)SCrewMetrics::MIN_LEVEL)
		return 0;
	else if (rValue >= (double)SCrewMetrics::MAX_LEVEL)
		return SCrewMetrics::MAX_LEVEL;
	else
		return (DWORD)rValue;
	}

void CCrewPsyche::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Reads

	{
	DWORD dwLoad;
	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));

	m_dwArchetype = (dwLoad & 0xff000000) >> 24;
	m_dwBelief =	(dwLoad & 0x00ff0000) >> 16;
	m_dwCohesion =	(dwLoad & 0x0000ff00) >> 8;
	m_dwLoyalty =	(dwLoad & 0x000000ff);
	}

void CCrewPsyche::SetArchetype (ECrewArchetypes iArchetype)

//	SetArchetype
//
//	Sets the archetype. This will also initialize belief, cohesion, and loyalty
//	appropriately.

	{
	switch (iArchetype)
		{
		case archetypeNone:
			m_dwArchetype = archetypeNone;
			m_dwBelief = 0;
			m_dwCohesion = 0;
			m_dwLoyalty = 0;
			break;

		case archetypeBrotherhood:
			m_dwArchetype = archetypeBrotherhood;
			m_dwBelief = GenerateNormalLevel();
			m_dwCohesion = SCrewMetrics::HIGH_LEVEL;
			m_dwLoyalty = GenerateNormalLevel();
			break;

		case archetypeOrder:
			m_dwArchetype = archetypeOrder;
			m_dwBelief = GenerateNormalLevel();
			m_dwCohesion = GenerateNormalLevel();
			m_dwLoyalty = SCrewMetrics::HIGH_LEVEL;
			break;

		case archetypeVengeance:
			m_dwArchetype = archetypeVengeance;
			m_dwBelief = SCrewMetrics::HIGH_LEVEL;
			m_dwCohesion = GenerateNormalLevel();
			m_dwLoyalty = GenerateNormalLevel();
			break;
		}
	}

void CCrewPsyche::WriteToStream (IWriteStream *pStream)

//	WriteToStream
//
//	Writes

	{
	DWORD dwSave = (m_dwArchetype << 24) | (m_dwBelief << 16) | (m_dwCohesion << 8) | (m_dwLoyalty);
	pStream->Write((char *)&dwSave, sizeof(DWORD));
	}
