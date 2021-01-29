//	CSquadronDesc.cpp
//
//	CSquadronDesc class
//	Copyright (c) 2021 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define BUILD_REINFORCEMENTS_ATTRIB				CONSTLIT("buildReinforcements")
#define CHALLENGE_ATTRIB						CONSTLIT("challenge")
#define CONSTRUCTION_RATE_ATTRIB				CONSTLIT("constructionRate")
#define COUNT_ATTRIB							CONSTLIT("count")
#define MIN_COUNT_ATTRIB						CONSTLIT("minCount")
#define MIN_SHIPS_ATTRIB						CONSTLIT("minShips")
#define MAX_CONSTRUCTION_ATTRIB					CONSTLIT("maxConstruction")
#define STANDING_COUNT_ATTRIB					CONSTLIT("standingCount")
#define TABLE_ATTRIB							CONSTLIT("table")

void CSquadronDesc::AddTypesUsed (TSortMap<DWORD, bool> &retTypesUsed) const

//	AddTypesUsed
//
//	Adds the list of types used.

	{
	if (m_pInitialShips)
		m_pInitialShips->AddTypesUsed(&retTypesUsed);

	if (m_pReinforcements)
		m_pReinforcements->AddTypesUsed(&retTypesUsed);

	if (m_pConstruction)
		m_pConstruction->AddTypesUsed(&retTypesUsed);
	}

bool CSquadronDesc::BindDesign (SDesignLoadCtx &Ctx)

//	BindDesign
//
//	Bind the design.

	{
	if (m_pInitialShips)
		if (m_pInitialShips->OnDesignLoadComplete(Ctx) != NOERROR)
			return false;

	if (m_pReinforcements)
		if (m_pReinforcements->OnDesignLoadComplete(Ctx) != NOERROR)
			return false;

	if (m_pConstruction)
		if (m_pConstruction->OnDesignLoadComplete(Ctx) != NOERROR)
			return false;

	return true;
	}

Metric CSquadronDesc::CalcDefenderStrength (int iLevel) const

//	CalcDefenderStrength
//
//	Computes the combat strength of all defenders (for purposes of determining
//	the combat strength of a station).

	{
	if (m_pInitialShips)
		return m_pInitialShips->GetAverageLevelStrength(iLevel);
	else
		return 0.0;
	}

void CSquadronDesc::CleanUp ()

//	CleanUp
//
//	Free all resources

	{
	if (m_pInitialShips)
		delete m_pInitialShips;

	if (m_pReinforcements)
		delete m_pReinforcements;

	if (m_pConstruction)
		delete m_pConstruction;

	m_pInitialShips = NULL;
	m_pReinforcements = NULL;
	m_pConstruction = NULL;
	m_iShipConstructionRate = 0;
	m_iMaxConstruction = 0;

	m_bBuildReinforcements = false;
	}

const IShipGenerator *CSquadronDesc::GetReinforcementsTable (void) const

//	GetReinforcementsTable
//
//	Return the reinforcements table.
	
	{
	//	If we have a ship count structure, then we always use the main ship 
	//	table.

	if (m_DefenderCount.GetCountType() == CShipChallengeDesc::countStanding)
		return m_pInitialShips;

	//	Otherwise, if we have an explicit reinforcements table, use that.

	else if (m_pReinforcements)
		return m_pReinforcements;

	//	Otherwise, we use the main table.

	else if (m_DefenderCount.GetCountType() != CShipChallengeDesc::countNone)
		return m_pInitialShips;

	else
		return NULL;
	}

bool CSquadronDesc::Init (SDesignLoadCtx &Ctx, const CString &sID, const CXMLElement *pShips, const CXMLElement *pReinforcements, const CXMLElement *pConstruction)

//	Init
//
//	Initializes from backwards compatible elements.

	{
	CleanUp();

	m_sID = sID;

	if (pShips)
		{
		if (IShipGenerator::CreateFromXMLAsGroup(Ctx, pShips, &m_pInitialShips) != NOERROR)
			{
			Ctx.sError = strPatternSubst(CONSTLIT("<Ships>: %s"), Ctx.sError);
			return false;
			}

		//	See if we have a challenge rating; in that case, we use it as both
		//	initial ships and reinforcements.

		if (!InitDefenderCount(Ctx, *pShips))
			{
			Ctx.sError = strPatternSubst(CONSTLIT("<Ships>: %s"), Ctx.sError);
			return false;
			}

		//	Build instead of gate in

		bool bValue;
		if (pShips->FindAttributeBool(BUILD_REINFORCEMENTS_ATTRIB, &bValue) && bValue)
			m_bBuildReinforcements = true;
		}

	//	Load reinforcements

	if (pReinforcements)
		{
		if (IShipGenerator::CreateFromXMLAsGroup(Ctx, pReinforcements, &m_pReinforcements) != NOERROR)
			{
			Ctx.sError = strPatternSubst(CONSTLIT("<Reinforcements>: %s"), Ctx.sError);
			return false;
			}

		//	Figure out the minimum number of reinforcements at this base

		if (!m_DefenderCount.Init(CShipChallengeDesc::countReinforcements, pReinforcements->GetAttribute(MIN_SHIPS_ATTRIB)))
			{
			Ctx.sError = CONSTLIT("Invalid count attribute in <Reinforcements>");
			return false;
			}

		//	Build instead of gate in

		bool bValue;
		if (pReinforcements->FindAttributeBool(BUILD_REINFORCEMENTS_ATTRIB, &bValue) && bValue)
			m_bBuildReinforcements = true;
		}

	//	Load construction table

	if (pConstruction)
		{
		if (IShipGenerator::CreateFromXMLAsGroup(Ctx, pConstruction, &m_pConstruction) != NOERROR)
			{
			Ctx.sError = strPatternSubst(CONSTLIT("<Construction>: %s"), Ctx.sError);
			return false;
			}

		m_iShipConstructionRate = pConstruction->GetAttributeInteger(CONSTRUCTION_RATE_ATTRIB);
		m_iMaxConstruction = pConstruction->GetAttributeInteger(MAX_CONSTRUCTION_ATTRIB);
		}

	return true;
	}

bool CSquadronDesc::InitDefenderCount (SDesignLoadCtx &Ctx, const CXMLElement &Desc)

//	InitDefenderCount
//
//	Initializes m_DefenderCount from XML.

	{
	//	See if we have a challenge rating; in that case, we use it as both
	//	initial ships and reinforcements.

	CString sCount;
	if (Desc.FindAttribute(CHALLENGE_ATTRIB, &sCount))
		{
		if (!m_DefenderCount.InitFromChallengeRating(sCount))
			{
			Ctx.sError = CONSTLIT("Invalid challenge attribute.");
			return false;
			}
		}

	//	If defined, we use this count to create initial ships AND reinforcements.

	else if (Desc.FindAttribute(STANDING_COUNT_ATTRIB, &sCount))
		{
		if (!m_DefenderCount.Init(CShipChallengeDesc::countStanding, sCount))
			{
			Ctx.sError = CONSTLIT("Invalid standingCount attribute.");
			return false;
			}
		}

	//	Otherwise, see if we define minShips, in which case we use that value for
	//	reinforcements only.

	else if (Desc.FindAttribute(MIN_COUNT_ATTRIB, &sCount) || Desc.FindAttribute(MIN_SHIPS_ATTRIB, &sCount))
		{
		if (!m_DefenderCount.Init(CShipChallengeDesc::countReinforcements, sCount))
			{
			Ctx.sError = CONSTLIT("Invalid minCount attribute.");
			return false;
			}
		}

	//	If all we have is a count, then we create this number of initial ships
	//	but never request reinforcements.

	else if (Desc.FindAttribute(COUNT_ATTRIB, &sCount))
		{
		if (!m_DefenderCount.Init(CShipChallengeDesc::countInitial, sCount))
			{
			Ctx.sError = CONSTLIT("Invalid count attribute.");
			return false;
			}
		}

	return true;
	}

bool CSquadronDesc::InitFromXML (SDesignLoadCtx &Ctx, const CString &sID, const CXMLElement &Desc)

//	InitFromXML
//
//	Initialize from XML element.

	{
	m_sID = sID;

	if (!InitDefenderCount(Ctx, Desc))
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Squadron %s: %s"), sID, Ctx.sError);
		return false;
		}

	m_bBuildReinforcements = Desc.GetAttributeBool(BUILD_REINFORCEMENTS_ATTRIB);

	//	If we have a table referenced, then we use this as the ship table.

	CString sTable;
	if (Desc.FindAttribute(TABLE_ATTRIB, &sTable))
		{
		if (IShipGenerator::CreateAsLookup(Ctx, sTable, &m_pInitialShips) != NOERROR)
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Squadron %s: %s"), sID, Ctx.sError);
			return false;
			}
		}
	else
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Squadron %s: Missing table reference."), sID);
		return false;
		}

	return true;
	}

void CSquadronDesc::Move (CSquadronDesc &Src) noexcept

//	Move
//
//	Take ownership of resources.

	{
	m_pInitialShips = Src.m_pInitialShips;
	Src.m_pInitialShips = NULL;

	m_pReinforcements = Src.m_pReinforcements;
	Src.m_pReinforcements = NULL;

	m_pConstruction = Src.m_pConstruction;
	Src.m_pConstruction = NULL;
	}
