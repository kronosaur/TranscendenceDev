//	CItemEncounterDefinitions.cpp
//
//	CItemEncounterDefinitions class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define FREQUENCY_TAG							CONSTLIT("Frequency")

#define CRITERIA_ATTRIB							CONSTLIT("criteria")
#define FREQUENCY_ATTRIB						CONSTLIT("frequency")
#define SYSTEM_CRITERIA_ATTRIB					CONSTLIT("systemCriteria")

const CItemEncounterDefinitions CItemEncounterDefinitions::Null;

void CItemEncounterDefinitions::AdjustFrequency (SCtx &Ctx, const CTopologyNode &Node, const CItem &Item, int &iFreq) const

//	GetFrequency
//
//	Returns the frequency for the given item.

	{
	if (!Ctx.pNode)
		InitCtx(Ctx, Node);

	for (int i = 0; i < Ctx.Selection.GetCount(); i++)
		{
		const SEntry &Entry = m_Table[Ctx.Selection[i]];
		if (Item.MatchesCriteria(Entry.Criteria))
			{
			iFreq = mathRound(iFreq * Entry.rFreqAdj);
			}
		}
	}

void CItemEncounterDefinitions::InitCtx (SCtx &Ctx, const CTopologyNode &Node) const

//	InitCtx
//
//	Initialize context block.

	{
	Ctx.pNode = &Node;
	Ctx.Selection.DeleteAll();

	CTopologyNodeCriteria::SCtx NodeCtx(Node.GetTopology());

	bool bIgnoreDefaults = false;
	TArray<int> Defaults;
	for (int i = 0; i < m_Table.GetCount(); i++)
		{
		const SEntry &Entry = m_Table[i];
		if (Entry.bDefaultSystems)
			{
			Defaults.Insert(i);
			continue;
			}

		//	NOTE: Null node matches everything

		if (Node.IsNull() || Entry.SystemCriteria.Matches(NodeCtx, Node))
			{
			//	If the entry frequency is common then we don't need to do
			//	anything, but we don't check the defaults either.

			if (Entry.Frequency == ftCommon)
				bIgnoreDefaults = true;
			else
				Ctx.Selection.Insert(i);
			}
		}

	//	If none of the entries match, then take the defaults.

	if (Ctx.Selection.GetCount() == 0 && !bIgnoreDefaults)
		Ctx.Selection = Defaults;
	}

ALERROR CItemEncounterDefinitions::InitFromXML (SDesignLoadCtx &Ctx, const CXMLElement &Desc)

//	InitFromXML
//
//	Initialize from XML.

	{
	for (int i = 0; i < Desc.GetContentElementCount(); i++)
		{
		const CXMLElement &Directive = *Desc.GetContentElement(i);
		if (strEquals(Directive.GetTag(), FREQUENCY_TAG))
			{
			//	Frequency

			CString sAttrib;
			if (!Directive.FindAttribute(FREQUENCY_ATTRIB, &sAttrib))
				{
				Ctx.sError = CONSTLIT("Expected frequency= attribute.");
				return ERR_FAIL;
				}

			int iFreq = ::GetFrequency(sAttrib);

			//	System criteria

			CTopologyNodeCriteria SystemCriteria;
			bool bHasSystemCriteria;
			if (Directive.FindAttribute(SYSTEM_CRITERIA_ATTRIB, &sAttrib))
				{
				if (ALERROR error = SystemCriteria.Parse(sAttrib, &Ctx.sError))
					return error;

				bHasSystemCriteria = true;
				}
			else
				bHasSystemCriteria = false;

			//	Item criteria

			CItemCriteria ItemCriteria(Directive.GetAttribute(CRITERIA_ATTRIB), CItemCriteria::ALL);

			//	If we get this far then we have a valid entry.

			SEntry &Entry = *m_Table.Insert();
			Entry.Criteria = ItemCriteria;
			Entry.SystemCriteria = SystemCriteria;
			Entry.bHasSystemCriteria = bHasSystemCriteria;
			Entry.Frequency = (FrequencyTypes)iFreq;
			Entry.rFreqAdj = (Metric)iFreq / (Metric)ftCommon;
			Entry.bDefaultSystems = !bHasSystemCriteria;
			}
		else
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Unknown element type: %s"), Directive.GetTag());
			return ERR_FAIL;
			}
		}

	return NOERROR;
	}
