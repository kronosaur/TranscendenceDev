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
#define REINFORCE_ATTRIB						CONSTLIT("reinforce")
#define REINFORCE_FROM_ATTRIB					CONSTLIT("reinforceFrom")
#define REINFORCE_LIMIT_ATTRIB					CONSTLIT("reinforceLimit")
#define REINFORCE_MIN_INTERVAL_ATTRIB			CONSTLIT("reinforceMinInterval")
#define STANDING_COUNT_ATTRIB					CONSTLIT("standingCount")
#define TABLE_ATTRIB							CONSTLIT("table")

#define TYPE_BUILD								CONSTLIT("build")
#define TYPE_GATE								CONSTLIT("gate")

void CSquadronDesc::AddTypesUsed (TSortMap<DWORD, bool> &retTypesUsed) const

//	AddTypesUsed
//
//	Adds the list of types used.

	{
	if (m_pShipTable)
		m_pShipTable->AddTypesUsed(&retTypesUsed);

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
	if (m_pShipTable)
		if (m_pShipTable->OnDesignLoadComplete(Ctx) != NOERROR)
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
	if (m_pShipTable)
		return m_pShipTable->GetAverageLevelStrength(iLevel);
	else
		return 0.0;
	}

void CSquadronDesc::CleanUp ()

//	CleanUp
//
//	Free all resources

	{
	if (m_pShipTable)
		delete m_pShipTable;

	if (m_pReinforcements)
		delete m_pReinforcements;

	if (m_pConstruction)
		delete m_pConstruction;

	m_pShipTable = NULL;
	m_pReinforcements = NULL;
	m_pConstruction = NULL;
	m_iShipConstructionRate = 0;
	m_iMaxConstruction = 0;

	m_bBuildReinforcements = false;
	}

ICCItemPtr CSquadronDesc::GetDesc (CUniverse &Universe) const

//	GetDesc
//
//	Returns a descriptor.

	{
	ICCItemPtr pResult(ICCItem::SymbolTable);

	pResult->SetStringAt(CONSTLIT("id"), m_sID);
	pResult->SetAt(CONSTLIT("count"), m_Count.GetDesc());
	pResult->SetAt(CONSTLIT("reinforce"), m_Reinforcement.GetDesc());

	if (m_iReinforceInterval != DEFAULT_INTERVAL_TICKS)
		pResult->SetIntegerAt(CONSTLIT("reinforceInterval"), m_iReinforceInterval / g_TicksPerSecond);

	if (m_iReinforceLimit)
		pResult->SetIntegerAt(CONSTLIT("reinforceLimit"), m_iReinforceLimit);

	if (m_pShipTable)
		pResult->SetAt(CONSTLIT("shipTable"), m_pShipTable->GetShipsReferenced(Universe));

	if (m_pReinforcements)
		pResult->SetAt(CONSTLIT("reinforcementsTable"), m_pReinforcements->GetShipsReferenced(Universe));

	if (m_pConstruction)
		{
		pResult->SetAt(CONSTLIT("constructionTable"), m_pConstruction->GetShipsReferenced(Universe));
		pResult->SetIntegerAt(CONSTLIT("constructionRate"), m_iShipConstructionRate);
		pResult->SetIntegerAt(CONSTLIT("constructionMaxCount"), m_iMaxConstruction);
		}

	if (m_bBuildReinforcements)
		pResult->SetStringAt(CONSTLIT("reinforceFrom"), CONSTLIT("build"));

	return pResult;
	}

const IShipGenerator *CSquadronDesc::GetReinforcementsTable (void) const

//	GetReinforcementsTable
//
//	Return the reinforcements table.
	
	{
	//	If we have an explicit reinforcements table, use that.

	if (m_pReinforcements)
		return m_pReinforcements;

	//	Otherwise, we use the main table.

	else if (m_Count.GetCountType() != CShipChallengeDesc::countNone
			&& m_Count.GetCountType() != CShipChallengeDesc::countAuto)
		return m_pShipTable;

	else
		return NULL;
	}

bool CSquadronDesc::HasShipClass (const CShipClass &ShipClass) const

//	HasShipClass
//
//	Returns TRUE if we have this ship class.

	{
	if (m_pShipTable && m_pShipTable->HasType(ShipClass))
		return true;
		
	if (m_pReinforcements && m_pReinforcements->HasType(ShipClass))
		return true;

	if (m_pConstruction && m_pConstruction->HasType(ShipClass))
		return true;

	return false;
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
		if (IShipGenerator::CreateFromXMLAsGroup(Ctx, pShips, &m_pShipTable) != NOERROR)
			{
			Ctx.sError = strPatternSubst(CONSTLIT("<Ships>: %s"), Ctx.sError);
			return false;
			}

		//	See if we have a challenge rating; in that case, we use it as both
		//	initial ships and reinforcements.

		if (!InitDefenderCountCompatible(Ctx, *pShips))
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

		if (!m_Reinforcement.Init(CShipChallengeDesc::countShips, pReinforcements->GetAttribute(MIN_SHIPS_ATTRIB)))
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

bool CSquadronDesc::InitDefenderCountCompatible (SDesignLoadCtx &Ctx, const CXMLElement &Desc)

//	InitDefenderCount
//
//	Initializes m_DefenderCount from XML.

	{
	//	See if we have a challenge rating; in that case, we use it as both
	//	initial ships and reinforcements.

	CString sCount;
	if (Desc.FindAttribute(CHALLENGE_ATTRIB, &sCount))
		{
		if (!m_Count.InitFromChallengeRating(sCount))
			{
			Ctx.sError = CONSTLIT("Invalid challenge attribute.");
			return false;
			}

		m_Reinforcement.Init(CShipChallengeDesc::countAuto);
		}

	//	If defined, we use this count to create initial ships AND reinforcements.

	else if (Desc.FindAttribute(STANDING_COUNT_ATTRIB, &sCount))
		{
		if (!m_Count.Init(CShipChallengeDesc::countShips, sCount))
			{
			Ctx.sError = CONSTLIT("Invalid standingCount attribute.");
			return false;
			}

		m_Reinforcement.Init(CShipChallengeDesc::countAuto);
		}

	//	Otherwise, see if we define minShips, in which case we use that value for
	//	reinforcements only.

	else if (Desc.FindAttribute(MIN_COUNT_ATTRIB, &sCount) || Desc.FindAttribute(MIN_SHIPS_ATTRIB, &sCount))
		{
		if (!m_Reinforcement.Init(CShipChallengeDesc::countShips, sCount))
			{
			Ctx.sError = CONSTLIT("Invalid minCount attribute.");
			return false;
			}

		//	Auto means roll once on the table

		m_Count.Init(CShipChallengeDesc::countAuto);
		}
	else
		{
		m_Count.Init(CShipChallengeDesc::countAuto);
		m_Reinforcement.Init(CShipChallengeDesc::countNone);
		}

	return true;
	}

bool CSquadronDesc::InitFromXML (SDesignLoadCtx &Ctx, const CString &sID, const CXMLElement &Desc)

//	InitFromXML
//
//	Initialize from XML element.

	{
	m_sID = sID;

	if (!m_Count.InitFromXML(Desc.GetAttribute(COUNT_ATTRIB)))
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Squadron %s: Invalid count: %s."), sID, Desc.GetAttribute(COUNT_ATTRIB));
		return false;
		}

	if (!m_Reinforcement.InitFromXML(Desc.GetAttribute(REINFORCE_ATTRIB)))
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Squadron %s: Invalid reinforce: %s."), sID, Desc.GetAttribute(REINFORCE_ATTRIB));
		return false;
		}

	//	reinforceFrom=

	CString sBuild = Desc.GetAttribute(REINFORCE_FROM_ATTRIB);
	if (sBuild.IsBlank() || strEquals(sBuild, TYPE_GATE))
		m_bBuildReinforcements = false;
	else if (strEquals(sBuild, TYPE_BUILD))
		m_bBuildReinforcements = true;
	else
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Squadron %s: Invalid reinforceFrom parameter: %s."), sID, sBuild);
		return false;
		}

	m_iReinforceInterval = Desc.GetAttributeIntegerBounded(REINFORCE_MIN_INTERVAL_ATTRIB, 1, -1, DEFAULT_INTERVAL_SECONDS) * g_TicksPerSecond;
	m_iReinforceLimit = Desc.GetAttributeIntegerBounded(REINFORCE_LIMIT_ATTRIB, 0, -1, 0);

	//	If we have a table referenced, then we use this as the ship table.

	CString sTable;
	if (Desc.FindAttribute(TABLE_ATTRIB, &sTable))
		{
		if (IShipGenerator::CreateAsLookup(Ctx, sTable, &m_pShipTable) != NOERROR)
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
	m_pShipTable = Src.m_pShipTable;
	Src.m_pShipTable = NULL;

	m_pReinforcements = Src.m_pReinforcements;
	Src.m_pReinforcements = NULL;

	m_pConstruction = Src.m_pConstruction;
	Src.m_pConstruction = NULL;
	}
