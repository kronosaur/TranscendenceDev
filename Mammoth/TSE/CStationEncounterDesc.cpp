//	CStationEncounterDesc.cpp
//
//	CStationEncounterDesc class
//	Copyright (c) 2013 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define CRITERIA_TAG							CONSTLIT("Criteria")
#define ENCOUNTER_TAG							CONSTLIT("Encounter")
#define SYSTEM_CRITERIA_TAG						CONSTLIT("SystemCriteria")

#define DISTANCE_CRITERIA_ATTRIB				CONSTLIT("distanceCriteria")
#define DISTANCE_FREQUENCY_ATTRIB				CONSTLIT("distanceFrequency")
#define ENEMY_EXCLUSION_RADIUS_ATTRIB			CONSTLIT("enemyExclusionRadius")
#define EXCLUSION_RADIUS_ATTRIB					CONSTLIT("exclusionRadius")
#define LEVEL_FREQUENCY_ATTRIB					CONSTLIT("levelFrequency")
#define LOCATION_CRITERIA_ATTRIB				CONSTLIT("locationCriteria")
#define MAX_APPEARING_ATTRIB					CONSTLIT("maxAppearing")
#define MAX_IN_SYSTEM_ATTRIB					CONSTLIT("maxInSystem")
#define MIN_APPEARING_ATTRIB					CONSTLIT("minAppearing")
#define NUMBER_APPEARING_ATTRIB					CONSTLIT("numberAppearing")
#define SYSTEM_AFFINITY_ATTRIB					CONSTLIT("systemAffinity")
#define SYSTEM_CRITERIA_ATTRIB					CONSTLIT("systemCriteria")
#define UNIQUE_ATTRIB							CONSTLIT("unique")

#define UNIQUE_IN_SYSTEM						CONSTLIT("inSystem")
#define UNIQUE_IN_UNIVERSE						CONSTLIT("inUniverse")

#define VALUE_FALSE								CONSTLIT("false")
#define VALUE_TRUE								CONSTLIT("true")

const Metric DEFAULT_ENEMY_EXCLUSION =			50.0 * LIGHT_SECOND;

int CStationEncounterDesc::CalcAffinity (CTopologyNode *pNode) const

//	CalcAffinity
//
//	Calculates the affinity for the given node. We return values between
//	ftCommon and 0.

	{
	if (m_SystemAffinity.MatchesAll())
		return ftCommon;

	int iWeight = m_SystemAffinity.CalcNodeWeight(pNode);
	return (ftCommon * iWeight / 1000);
	}

int CStationEncounterDesc::CalcLevelFromFrequency (void) const

//	CalcLevelFromFrequency
//
//	Computes a level from a frequency. Returns 0 if we could not compute
//	the level.

	{
	int iLevel = 1;
	int iTotal = 0;
	int iCount = 0;

	char *pPos = m_sLevelFrequency.GetASCIIZPointer();
	while (*pPos != '\0')
		{
		int iFreq = 0;

		switch (*pPos)
			{
			case 'C':
			case 'c':
				iFreq = ftCommon;
				break;

			case 'U':
			case 'u':
				iFreq = ftUncommon;
				break;

			case 'R':
			case 'r':
				iFreq = ftRare;
				break;

			case 'V':
			case 'v':
				iFreq = ftVeryRare;
				break;
			}

		iTotal += iFreq * iLevel;
		iCount += iFreq;

		pPos++;
		if (*pPos != ' ')
			iLevel++;
		}

	//	Compute the level by averaging things out

	return (iCount > 0 ? mathRound((double)iTotal / (double)iCount) : 0);
	}

int CStationEncounterDesc::GetCountOfRandomEncounterLevels (void) const

//	GetCountOfRandomEncounterLevels
//
//	Returns the number of levels in which the station can be
//	encountered.

	{
	int iCount = 0;

	char *pPos = m_sLevelFrequency.GetASCIIZPointer();
	while (*pPos != '\0')
		{
		if (*pPos != ' ' && *pPos != '-')
			iCount++;

		pPos++;
		}

	return iCount;
	}

void CStationEncounterDesc::GetExclusionDesc (SExclusionDesc &Exclusion) const

//	GetExclusionDesc
//
//	Returns an exclusion descriptor

	{
	Exclusion.rAllExclusionRadius2 = m_rExclusionRadius * m_rExclusionRadius;
	Exclusion.bHasAllExclusion = (m_rExclusionRadius > 0.0);

	Exclusion.rEnemyExclusionRadius2 = m_rEnemyExclusionRadius * m_rEnemyExclusionRadius;
	Exclusion.bHasEnemyExclusion = (m_rEnemyExclusionRadius > m_rExclusionRadius);
	}

int CStationEncounterDesc::GetFrequencyByDistance (int iDistance) const

//	GetFrequencyByDistance
//
//	Returns the frequency of the encounter based on the distance to m_sDistanceCriteria.
//	Distances >= 1 mean that we are that many nodes away from a node that matches
//	m_sDistanceCriteria. Distances <= -1 mean that we are that many nodes away from
//	a node that DOES NOT match m_sDistanceCriteria.
//
//	m_sDistanceFrequency must have two sets of five elements:
//
//	               54321 12345
//                 \___/ \___/
//                   |     |
//	Negative distances     Positive distances

	{
	if (iDistance < -5)
		return ::GetFrequencyByLevel(m_sDistanceFrequency, 1);
	else if (iDistance < 0)
		return ::GetFrequencyByLevel(m_sDistanceFrequency, iDistance + 6);
	else if (iDistance > 5)
		return ftNotRandom;
	else
		return ::GetFrequencyByLevel(m_sDistanceFrequency, iDistance + 5);
	}

int CStationEncounterDesc::GetFrequencyByLevel (int iLevel) const

//	GetFrequencyByLevel
//
//	Returns the frequency of the encounter by level. Note this is used for stats 
//	and random encounters. To determine the frequency of a station in a given
//	system, use GetFrequencyBySystem instead.

	{
	if (!CanBeRandomlyEncountered())
		return 0;

	if (m_sLevelFrequency.IsBlank())
		return ftCommon;
	else
		return ::GetFrequencyByLevel(m_sLevelFrequency, iLevel);
	}

bool CStationEncounterDesc::InitAsOverride (const CStationEncounterDesc &Original, const CXMLElement &Override, CString *retsError)

//	InitAsOverride
//
//	Initializes as an override. We take all data from the Original except where 
//	the Override defines something.

	{
	//	Start as the original

	*this = Original;

	//	Number appearing (at least this number, unique in system)

	CString sAttrib;
	if (Override.FindAttribute(MIN_APPEARING_ATTRIB, &sAttrib)
			|| Override.FindAttribute(NUMBER_APPEARING_ATTRIB, &sAttrib))
		{
		m_bNumberAppearing = true;
		if (m_NumberAppearing.LoadFromXML(sAttrib) != NOERROR)
			{
			if (retsError) *retsError = strPatternSubst(CONSTLIT("Invalid numberAppearing parameter."));
			return false;
			}

		m_iMaxCountInSystem = 1;
		}

	//	Get maximum limit (at most this number, unique in system)

	if (Override.FindAttribute(MAX_APPEARING_ATTRIB, &sAttrib))
		{
		m_bMaxCountLimit = true;
		if (m_MaxAppearing.LoadFromXML(sAttrib) != NOERROR)
			{
			if (retsError) *retsError = strPatternSubst(CONSTLIT("Invalid maxAppearing parameter."));
			return false;
			}

		m_iMaxCountInSystem = 1;
		}

	//	Otherwise, we check uniqueness values

	if (Override.FindAttribute(UNIQUE_ATTRIB, &sAttrib))
		{
		if (strEquals(sAttrib, UNIQUE_IN_SYSTEM))
			{
			m_bMaxCountLimit = false;
			m_iMaxCountInSystem = 1;
			}
		else if (strEquals(sAttrib, UNIQUE_IN_UNIVERSE) || strEquals(sAttrib, VALUE_TRUE))
			{
			m_bMaxCountLimit = true;
			m_MaxAppearing.SetConstant(1);
			m_iMaxCountInSystem = 1;
			}
		}

	//	System criteria

	CXMLElement *pCriteria = Override.GetContentElementByTag(SYSTEM_CRITERIA_TAG);
	if (pCriteria == NULL)
		pCriteria = Override.GetContentElementByTag(CRITERIA_TAG);

	if (pCriteria)
		{
		if (CTopologyNode::ParseCriteria(pCriteria, &m_SystemCriteria, retsError) != NOERROR)
			return false;

		m_bSystemCriteria = true;
		}
	else if (Override.FindAttribute(SYSTEM_CRITERIA_ATTRIB, &sAttrib))
		{
		if (CTopologyNode::ParseCriteria(sAttrib, &m_SystemCriteria, retsError) != NOERROR)
			return false;

		m_bSystemCriteria = true;
		}

	//	Level frequency and criteria

	if (Override.FindAttribute(DISTANCE_CRITERIA_ATTRIB, &sAttrib))
		{
		if (CTopologyNode::ParseAttributeCriteria(sAttrib, &m_DistanceCriteria) != NOERROR)
			return false;
		}

	if (Override.FindAttribute(DISTANCE_FREQUENCY_ATTRIB, &sAttrib))
		m_sDistanceFrequency = sAttrib;

	if (Override.FindAttribute(LEVEL_FREQUENCY_ATTRIB, &sAttrib))
		m_sLevelFrequency = sAttrib;

	if (Override.FindAttribute(LOCATION_CRITERIA_ATTRIB, &sAttrib))
		m_sLocationCriteria = sAttrib;

	if (Override.FindAttribute(SYSTEM_AFFINITY_ATTRIB, &sAttrib))
		{
		if (m_SystemAffinity.Parse(sAttrib, 0, retsError) != NOERROR)
			return false;
		}

	//	Exclusion radius

	int iRadius;
	if (Override.FindAttributeInteger(ENEMY_EXCLUSION_RADIUS_ATTRIB, &iRadius)
			&& iRadius >= 0)
		m_rEnemyExclusionRadius = iRadius * LIGHT_SECOND;

	if (Override.FindAttributeInteger(EXCLUSION_RADIUS_ATTRIB, &iRadius)
			&& iRadius >= 0)
		m_rExclusionRadius = iRadius * LIGHT_SECOND;

	//	Done

	return true;
	}

ALERROR CStationEncounterDesc::InitFromStationTypeXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	InitFromStationTypeXML
//
//	Initialize from <StationType>

	{
	//	If we have an encounter element, then use that.

	CXMLElement *pEncounter = pDesc->GetContentElementByTag(ENCOUNTER_TAG);
	if (pEncounter)
		return InitFromXML(Ctx, pEncounter);

	//	Otherwise, we use information at the root

	return InitFromXML(Ctx, pDesc);
	}

ALERROR CStationEncounterDesc::InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	InitFromXML
//
//	Initialize from an <Encounter> element (or a root <StationType> element).

	{
	ALERROR error;
	bool bNumberSet = false;

	//	Number appearing (at least this number, unique in system)

	CString sAttrib;
	if (pDesc->FindAttribute(MIN_APPEARING_ATTRIB, &sAttrib)
			|| pDesc->FindAttribute(NUMBER_APPEARING_ATTRIB, &sAttrib))
		{
		m_bNumberAppearing = true;
		if (error = m_NumberAppearing.LoadFromXML(sAttrib))
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Invalid numberAppearing parameter."));
			return error;
			}

		m_iMaxCountInSystem = 1;
		bNumberSet = true;
		}

	//	Get maximum limit (at most this number, unique in system)

	if (pDesc->FindAttribute(MAX_APPEARING_ATTRIB, &sAttrib))
		{
		m_bMaxCountLimit = true;
		if (error = m_MaxAppearing.LoadFromXML(sAttrib))
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Invalid maxAppearing parameter."));
			return error;
			}

		m_iMaxCountInSystem = 1;
		bNumberSet = true;
		}

	//	Otherwise, we check uniqueness values

	if (!bNumberSet)
		{
		CString sUnique = pDesc->GetAttribute(UNIQUE_ATTRIB);
		if (strEquals(sUnique, UNIQUE_IN_SYSTEM))
			{
			m_bMaxCountLimit = false;
			m_iMaxCountInSystem = 1;
			}
		else if (strEquals(sUnique, UNIQUE_IN_UNIVERSE) || strEquals(sUnique, VALUE_TRUE))
			{
			m_bMaxCountLimit = true;
			m_MaxAppearing.SetConstant(1);
			m_iMaxCountInSystem = 1;
			}
		else if (pDesc->FindAttributeInteger(MAX_IN_SYSTEM_ATTRIB, &m_iMaxCountInSystem))
			{
			m_bMaxCountLimit = false;
			m_iMaxCountInSystem = Max(1, m_iMaxCountInSystem);
			}
		else
			{
			m_bMaxCountLimit = false;
			m_iMaxCountInSystem = -1;
			}
		}

	//	System criteria

	CXMLElement *pCriteria = pDesc->GetContentElementByTag(SYSTEM_CRITERIA_TAG);
	if (pCriteria == NULL)
		pCriteria = pDesc->GetContentElementByTag(CRITERIA_TAG);

	if (pCriteria)
		{
		if (error = CTopologyNode::ParseCriteria(pCriteria, &m_SystemCriteria, &Ctx.sError))
			return error;

		m_bSystemCriteria = true;
		}
	else if (pDesc->FindAttribute(SYSTEM_CRITERIA_ATTRIB, &sAttrib))
		{
		if (error = CTopologyNode::ParseCriteria(sAttrib, &m_SystemCriteria, &Ctx.sError))
			return error;

		m_bSystemCriteria = true;
		}

	//	Level frequency and criteria

	if (error = CTopologyNode::ParseAttributeCriteria(pDesc->GetAttribute(DISTANCE_CRITERIA_ATTRIB), &m_DistanceCriteria))
		return error;

	m_sDistanceFrequency = pDesc->GetAttribute(DISTANCE_FREQUENCY_ATTRIB);
	m_sLevelFrequency = pDesc->GetAttribute(LEVEL_FREQUENCY_ATTRIB);
	m_sLocationCriteria = pDesc->GetAttribute(LOCATION_CRITERIA_ATTRIB);

	if (pDesc->FindAttribute(SYSTEM_AFFINITY_ATTRIB, &sAttrib))
		{
		if (error = m_SystemAffinity.Parse(sAttrib, 0, &Ctx.sError))
			return error;
		}

	//	Exclusion radius

	int iRadius;
	if (pDesc->FindAttributeInteger(ENEMY_EXCLUSION_RADIUS_ATTRIB, &iRadius)
			&& iRadius >= 0)
		m_rEnemyExclusionRadius = iRadius * LIGHT_SECOND;
	else
		m_rEnemyExclusionRadius = DEFAULT_ENEMY_EXCLUSION;

	if (pDesc->FindAttributeInteger(EXCLUSION_RADIUS_ATTRIB, &iRadius)
			&& iRadius >= 0)
		m_rExclusionRadius = iRadius * LIGHT_SECOND;
	else
		m_rExclusionRadius = 0.0;

	//	Done

	return NOERROR;
	}

void CStationEncounterDesc::InitLevelFrequency (void)

//	InitLevelFrequency
//
//	Initializes m_sLevelFrequency if it is not set and if we have a system
//	criteria. This must be called only after the topology has been initialized.

	{
	int i;

	if ((m_sLevelFrequency.IsBlank() || m_bAutoLevelFrequency)
			&& m_bSystemCriteria)
		{
		//	Start with no encounters

		m_sLevelFrequency = CString("----- ----- ----- ----- -----");

		//	Loop over all nodes and check to see if we appear at the node.
		//	If we do, we mark that level 'common.'

		CTopologyNode::SCriteriaCtx Ctx;
		CTopology &Topology = g_pUniverse->GetTopology();
		for (i = 0; i < Topology.GetTopologyNodeCount(); i++)
			{
			CTopologyNode *pNode = Topology.GetTopologyNode(i);

			if (pNode->MatchesCriteria(Ctx, m_SystemCriteria))
				::SetFrequencyByLevel(m_sLevelFrequency, pNode->GetLevel(), ftCommon);
			}

		//	Remember that we initialized this so that we can save it out.

		m_bAutoLevelFrequency = true;
		}
	}

void CStationEncounterDesc::ReadFromStream (SUniverseLoadCtx &Ctx)

//	ReadFromStream
//
//	Reads from a stream

	{
	CString sLevelFrequency;

	sLevelFrequency.ReadFromStream(Ctx.pStream);
	if (!sLevelFrequency.IsBlank())
		{
		m_sLevelFrequency = sLevelFrequency;
		m_bAutoLevelFrequency = true;
		}
	}

void CStationEncounterDesc::WriteToStream (IWriteStream *pStream)

//	WriteToStream
//
//	Writes to a stream

	{
	//	We only need to write the level frequency if we generated it from
	//	topology.

	if (m_bAutoLevelFrequency)
		m_sLevelFrequency.WriteToStream(pStream);
	else
		NULL_STR.WriteToStream(pStream);
	}

