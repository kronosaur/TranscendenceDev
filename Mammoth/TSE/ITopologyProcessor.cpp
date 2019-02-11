//	ITopologyProcessor.cpp
//
//	ITopologyProcessor class

#include "PreComp.h"

#define ATTRIBUTES_TAG						CONSTLIT("Attributes")
#define CONQUER_NODES_TAG					CONSTLIT("ConquerNodes")
#define CRITERIA_TAG						CONSTLIT("Criteria")
#define DISTRIBUTE_NODES_TAG				CONSTLIT("DistributeNodes")
#define FILL_NODES_TAG						CONSTLIT("FillNodes")
#define GROUP_TAG							CONSTLIT("Group")
#define LOCATE_NODES_TAG					CONSTLIT("LocateNodes")
#define PARTITION_NODES_TAG					CONSTLIT("PartitionNodes")
#define RANDOM_POINTS_TAG					CONSTLIT("RandomPoints")
#define SYSTEM_TAG							CONSTLIT("System")
#define TABLE_TAG							CONSTLIT("Table")

#define CRITERIA_ATTRIB						CONSTLIT("criteria")
#define PRIORITY_ATTRIB						CONSTLIT("priority")

static TStaticStringTable<TStaticStringEntry<ITopologyProcessor::EPhase>, ITopologyProcessor::phaseCount> PHASE_TABLE = {
	"formation",			ITopologyProcessor::phaseFormation,
	"lifeforms",			ITopologyProcessor::phaseLifeforms,
	"primaryColony",		ITopologyProcessor::phasePrimaryColony,
	"primaryMap",			ITopologyProcessor::phasePrimaryMap,
	"secondaryColony",		ITopologyProcessor::phaseSecondaryColony,
	"secondaryMap",			ITopologyProcessor::phaseSecondaryMap,
	"tertiaryColony",		ITopologyProcessor::phaseTertiaryColony,
	};

ALERROR ITopologyProcessor::CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID, ITopologyProcessor **retpProc)

//	CreateFromXML
//
//	Creates a new processor based on the XML tag

	{
	ALERROR error;
	ITopologyProcessor *pProc;

	//	Create the approprate class

	if (strEquals(pDesc->GetTag(), ATTRIBUTES_TAG))
		pProc = new CApplySystemProc;
	else if (strEquals(pDesc->GetTag(), CONQUER_NODES_TAG))
		pProc = new CConquerNodesProc;
	else if (strEquals(pDesc->GetTag(), DISTRIBUTE_NODES_TAG))
		pProc = new CDistributeNodesProc;
	else if (strEquals(pDesc->GetTag(), FILL_NODES_TAG))
		pProc = new CFillNodesProc;
	else if (strEquals(pDesc->GetTag(), GROUP_TAG))
		pProc = new CGroupTopologyProc;
	else if (strEquals(pDesc->GetTag(), LOCATE_NODES_TAG))
		pProc = new CLocateNodesProc;
	else if (strEquals(pDesc->GetTag(), PARTITION_NODES_TAG))
		pProc = new CPartitionNodesProc;
	else if (strEquals(pDesc->GetTag(), RANDOM_POINTS_TAG))
		pProc = new CRandomPointsProc;
	else if (strEquals(pDesc->GetTag(), SYSTEM_TAG))
		pProc = new CApplySystemProc;
	else if (strEquals(pDesc->GetTag(), TABLE_TAG))
		pProc = new CTableTopologyProc;
	else
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Unknown topology processor element: <%s>"), pDesc->GetTag());
		return ERR_FAIL;
		}

	//	Load it

	if (error = pProc->InitFromXML(Ctx, pDesc, sUNID))
		return error;

	//	Done

	*retpProc = pProc;

	return NOERROR;
	}

ALERROR ITopologyProcessor::CreateFromXMLAsGroup (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID, ITopologyProcessor **retpProc)

//	CreateFromXMLAsGroup
//
//	Creates a new CGroupTopologyProc from an XML element

	{
	*retpProc = new CGroupTopologyProc;
	return (*retpProc)->InitFromXML(Ctx, pDesc, sUNID);
	}

CTopologyNodeList *ITopologyProcessor::FilterNodes (CTopology &Topology, CTopologyNode::SCriteria &Criteria, CTopologyNodeList &Unfiltered, CTopologyNodeList &Filtered)

//	FilterNodes
//
//	Returns a list of filtered nodes

	{
	CTopologyNodeList *pNodeList = &Unfiltered;
	if (!CTopologyNode::IsCriteriaAll(Criteria))
		{
		//	Context

		CTopologyNode::SCriteriaCtx Ctx(Topology);

		//	Filter

		if (Unfiltered.Filter(Ctx, Criteria, &Filtered) != NOERROR)
			return NULL;

		pNodeList = &Filtered;
		}

	return pNodeList;
	}

ALERROR ITopologyProcessor::InitBaseItemXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	InitBaseItemXML
//
//	Processes a child XML element. We return one of the following:
//
//	NOERROR: Recognized as a base item; continue with next child element.
//	ERR_NOTFOUND: Not a base item; process it.
//	Other error: Error (Ctx.sError is initialized).

	{
	//	If we have a criteria, parse it and remember it
	//	(Note: If multiple criteria are found, we take the latest one).

	if (strEquals(pDesc->GetTag(), CRITERIA_TAG))
		{
		//	Parse the filter

		if (CTopologyNode::ParseCriteria(pDesc, &m_Criteria, &Ctx.sError) != NOERROR)
			return ERR_FAIL;
		}

	//	Our caller should process this item.

	else
		return ERR_NOTFOUND;

	//	We processed this item.

	return NOERROR;
	}

ALERROR ITopologyProcessor::InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID)

//	InitFromXML
//
//	Initialize form XML element

	{
	//	Initialize criteria

	CTopologyNode::ParseCriteria(NULL, &m_Criteria);

	CString sCriteria;
	if (pDesc->FindAttribute(CRITERIA_ATTRIB, &sCriteria))
		{
		if (CTopologyNode::ParseAttributeCriteria(sCriteria, &m_Criteria.AttribCriteria) != NOERROR)
			{
			Ctx.sError = CONSTLIT("Invalid criteria syntax.");
			return ERR_FAIL;
			}
		}

	//	Phase

	m_iPhase = ParsePhase(pDesc->GetAttribute(PRIORITY_ATTRIB));
	if (m_iPhase == phaseError)
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Invalid priority: %s"), pDesc->GetAttribute(PRIORITY_ATTRIB));
		return ERR_FAIL;
		}

	//	Let subclass handle the rest.

	return OnInitFromXML(Ctx, pDesc, sUNID);
	}

ITopologyProcessor::EPhase ITopologyProcessor::ParsePhase (const CString &sValue)

//	ParsePhase
//
//	Parses a phase value.

	{
	if (sValue.IsBlank())
		return phaseDefault;

	auto *pEntry = PHASE_TABLE.GetAt(sValue);
	if (pEntry == NULL)
		return phaseError;

	return pEntry->Value;
	}

void ITopologyProcessor::RestoreMarks (CTopology &Topology, TArray<bool> &Saved)

//	RestoreMarks
//
//	Restore the mark for nodes

	{
	Topology.GetTopologyNodeList().RestoreMarks(Saved);
	}

void ITopologyProcessor::SaveAndMarkNodes (CTopology &Topology, CTopologyNodeList &NodeList, TArray<bool> *retSaved)

//	SaveAndMarkNodes
//
//	Saves the mark for all topology nodes. Then it marks all nodes in the NodeList
//	and clears the mark on all others.

	{
	int i;

	Topology.GetTopologyNodeList().SaveAndSetMarks(false, retSaved);

	for (i = 0; i < NodeList.GetCount(); i++)
		NodeList[i]->SetMarked(true);
	}
