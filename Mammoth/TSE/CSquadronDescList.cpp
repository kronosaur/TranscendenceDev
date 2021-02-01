//	CSquadronDescList.cpp
//
//	CSquadronDescList class
//	Copyright (c) 2021 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define CONSTRUCTION_TAG						CONSTLIT("Construction")
#define REINFORCEMENTS_TAG						CONSTLIT("Reinforcements")
#define SHIPS_TAG								CONSTLIT("Ships")
#define SQUADRON_TAG							CONSTLIT("Squadron")
#define SQUADRONS_TAG							CONSTLIT("Squadrons")

#define DEBUG_ATTRIB							CONSTLIT("debug")
#define ID_ATTRIB								CONSTLIT("id")
#define SHIP_REGEN_ATTRIB						CONSTLIT("shipRegen")
#define SHIP_REPAIR_RATE_ATTRIB					CONSTLIT("shipRepairRate")

void CSquadronDescList::AddTypesUsed (TSortMap<DWORD, bool> &retTypesUsed) const

//	AddTypesUsed
//
//	Adds the list of types used.

	{
	for (int i = 0; i < m_Squadrons.GetCount(); i++)
		m_Squadrons[i]->AddTypesUsed(retTypesUsed);
	}

bool CSquadronDescList::BindDesign (SDesignLoadCtx &Ctx)

//	BindDesign
//
//	Bind the design.

	{
	for (int i = 0; i < m_Squadrons.GetCount(); i++)
		if (!m_Squadrons[i]->BindDesign(Ctx))
			return false;

	return true;
	}

Metric CSquadronDescList::CalcDefenderStrength (int iLevel) const

//	CalcDefenderStrength
//
//	Computes the combat strength of all defenders (for purposes of determining
//	the combat strength of a station).

	{
	Metric rTotal = 0.0;

	for (int i = 0; i < m_Squadrons.GetCount(); i++)
		rTotal += m_Squadrons[i]->CalcDefenderStrength(iLevel);

	return rTotal;
	}

const CSquadronDesc *CSquadronDescList::FindMatchingSquadron (const CShipClass &ShipClass) const

//	FindMatchingSquadron
//
//	Returns the first squadron descriptor that includes this ship class.

	{
	for (int i = 0; i < m_Squadrons.GetCount(); i++)
		{
		if (m_Squadrons[i]->HasShipClass(ShipClass))
			return m_Squadrons[i];
		}

	return NULL;
	}

Metric CSquadronDescList::GetChallengeStrength (int iLevel) const

//	GetChallengeStrength
//
//	Returns the total challenge strength.

	{
	Metric rTotal = 0.0;

	for (int i = 0; i < m_Squadrons.GetCount(); i++)
		rTotal += m_Squadrons[i]->GetChallengeStrength(iLevel);

	return rTotal;
	}

ICCItemPtr CSquadronDescList::GetConstructionShipsReferenced (CUniverse &Universe) const

//	GetConstructionShipsReferenced
//
//	Returns a list of all ship types referenced.

	{
	TSortMap<DWORD, bool> AllTypes;
	for (int i = 0; i < m_Squadrons.GetCount(); i++)
		if (const IShipGenerator *pGenerator = m_Squadrons[i]->GetConstructionTable())
			pGenerator->AddTypesUsed(&AllTypes);

	return IShipGenerator::GetShipsReferenced(Universe, AllTypes);
	}

ICCItemPtr CSquadronDescList::GetDesc (CUniverse &Universe) const

//	GetDesc
//
//	Returns a descriptor

	{
	if (m_Squadrons.GetCount() == 0)
		return ICCItemPtr::Nil();

	else if (m_Squadrons.GetCount() == 1)
		return m_Squadrons[0]->GetDesc(Universe);

	else
		{
		ICCItemPtr pResult(ICCItem::List);

		for (int i = 0; i < m_Squadrons.GetCount(); i++)
			pResult->Append(m_Squadrons[i]->GetDesc(Universe));

		return pResult;
		}
	}

ICCItemPtr CSquadronDescList::GetInitialShipsReferenced (CUniverse &Universe) const

//	GetInitialShipsReferenced
//
//	Returns a list of all ship types referenced.

	{
	TSortMap<DWORD, bool> AllTypes;
	for (int i = 0; i < m_Squadrons.GetCount(); i++)
		if (const IShipGenerator *pGenerator = m_Squadrons[i]->GetInitialShips())
			pGenerator->AddTypesUsed(&AllTypes);

	return IShipGenerator::GetShipsReferenced(Universe, AllTypes);
	}

ICCItemPtr CSquadronDescList::GetReinforcementShipsReferenced (CUniverse &Universe) const

//	GetReinforcementShipsReferenced
//
//	Returns a list of all ship types referenced.

	{
	TSortMap<DWORD, bool> AllTypes;
	for (int i = 0; i < m_Squadrons.GetCount(); i++)
		if (const IShipGenerator *pGenerator = m_Squadrons[i]->GetReinforcementsTable())
			pGenerator->AddTypesUsed(&AllTypes);

	return IShipGenerator::GetShipsReferenced(Universe, AllTypes);
	}

int CSquadronDescList::GetConstructionRate () const

//	GetConstructionRate
//
//	Returns the greatest construction rate.

	{
	int iBest = 0;
	for (int i = 0; i < m_Squadrons.GetCount(); i++)
		{
		int iRate = m_Squadrons[i]->GetShipConstructionRate();
		if (iRate && (iBest == 0 || iRate < iBest))
			iBest = iRate;
		}

	return iBest;
	}

int CSquadronDescList::GetConstructionMaxCount () const

//	GetConstructionMaxCount
//
//	Returns the total maximum count.

	{
	int iTotal = 0;
	for (int i = 0; i < m_Squadrons.GetCount(); i++)
		iTotal += m_Squadrons[i]->GetShipConstructionMax();

	return iTotal;
	}

bool CSquadronDescList::InitFromXML (SDesignLoadCtx &Ctx, const CXMLElement &StationXML)

//	InitFromXML
//
//	We expect either a <Squadrons> element or a bunch of separate elements (for
//	backwards compatibility).

	{
	if (const CXMLElement *pSquadrons = StationXML.GetContentElementByTag(SQUADRONS_TAG))
		{
		for (int i = 0; i < pSquadrons->GetContentElementCount(); i++)
			{
			const CXMLElement *pChild = pSquadrons->GetContentElement(i);

			if (!InsertSquadronFromXML(Ctx, *pChild))
				return false;
			}

		m_bDebug = pSquadrons->GetAttributeBool(DEBUG_ATTRIB);
		}
	else if (const CXMLElement *pSquadron = StationXML.GetContentElementByTag(SQUADRON_TAG))
		{
		if (!InsertSquadronFromXML(Ctx, *pSquadron))
			return false;

		m_bDebug = pSquadron->GetAttributeBool(DEBUG_ATTRIB);
		}

	//	Otherwise, we expect the backwards compatible method in which each is
	//	its own element.

	else
		{
		const CXMLElement *pShips = StationXML.GetContentElementByTag(SHIPS_TAG);
		const CXMLElement *pReinforcements = StationXML.GetContentElementByTag(REINFORCEMENTS_TAG);
		const CXMLElement *pConstruction = StationXML.GetContentElementByTag(CONSTRUCTION_TAG);

		//	If all are NULL, then we have nothing.

		if (!pShips && !pReinforcements && !pConstruction)
			return true;

		//	Create a single default entry.

		CString sID = CONSTLIT("0");
		auto pNewEntry = m_Squadrons.Insert(sID);
		pNewEntry->Set(new CSquadronDesc);

		if (!(*pNewEntry)->Init(Ctx, sID, pShips, pReinforcements, pConstruction))
			return false;
		}

	return true;
	}

bool CSquadronDescList::InsertSquadronFromXML (SDesignLoadCtx &Ctx, const CXMLElement &DescXML)

//	InsertSquadronFromXML
//
//	Inserts a squadron definition.

	{
	CString sID = DescXML.GetAttribute(ID_ATTRIB);
	if (sID.IsBlank())
		sID = strFromInt(m_Squadrons.GetCount());

	//	Insert entry

	bool bNew;
	auto pNewEntry = m_Squadrons.SetAt(sID, &bNew);

	//	Must be unique

	if (!bNew)
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Duplicate squadron ID: %s"), sID);
		return false;
		}

	//	Parse

	pNewEntry->Set(new CSquadronDesc);

	if (!(*pNewEntry)->InitFromXML(Ctx, sID, DescXML))
		return false;

	return true;
	}
