//	CApplySystemProc.cpp
//
//	CApplySystemProc class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define ATTRIBUTES_TAG							CONSTLIT("Attributes")
#define SYSTEM_TAG								CONSTLIT("System")

#define ATTRIBUTES_ATTRIB						CONSTLIT("attributes")

CApplySystemProc::~CApplySystemProc (void)

//	CApplySystemProc destructor

	{
	}

ALERROR CApplySystemProc::OnInitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID)

//	OnInitFromXML
//
//	Initialize from XML element

	{
	ALERROR error;

	//	Treat the content as a system definition

	const CString &sTag = pDesc->GetTag();

	//	Load it based on its tag

	if (strEquals(sTag, SYSTEM_TAG))
		{
		if (error = m_SystemDesc.InitFromXML(Ctx, pDesc))
			return error;
		}
	else if (strEquals(sTag, ATTRIBUTES_TAG))
		{
		m_sAttributes = pDesc->GetAttribute(ATTRIBUTES_ATTRIB);
		}

	//	Otherwise, unknown tag. This should never happen because we don't create
	//	this processor unless it's one of these two tags.

	else
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Unknown element: %s"), sTag);
		return ERR_FAIL;
		}

	//	See if we have any children (e.g., <Criteria>)

	for (int i = 0; i < pDesc->GetContentElementCount(); i++)
		{
		CXMLElement *pItem = pDesc->GetContentElement(i);

		//	See if this is an element handled by our base class

		if ((error = InitBaseItemXML(Ctx, pItem)) != ERR_NOTFOUND)
			{
			if (error != NOERROR)
				return error;
			}
		}

	return NOERROR;
	}

ALERROR CApplySystemProc::OnProcess (SProcessCtx &Ctx, CTopologyNodeList &NodeList, CString *retsError)

//	OnProcess
//
//	Process on topology

	{
	CTopologyNodeList Remaining;

	CTopologyNodeCriteria::SCtx MatchCtx(Ctx.Topology);

	//	Apply system properties to all nodes in list

	for (int i = 0; i < NodeList.GetCount(); i++)
		{
		CTopologyNode &Node = NodeList[i];

		//	Make sure we match criteria

		if (!m_Criteria.Matches(MatchCtx, Node))
			{
			//	Add to remaining (unprocessed list).

			if (Ctx.bReduceNodeList)
				Remaining.Insert(&Node);

			continue;
			}

		//	Apply

		if (!m_sAttributes.IsBlank())
			Node.AddAttributes(m_sAttributes);

		if (!m_SystemDesc.IsEmpty())
			m_SystemDesc.Apply(Ctx.Topology, &Node);
		}

	//	Modify original list to have only remaining (unprocessed) nodes.

	if (Ctx.bReduceNodeList)
		NodeList = Remaining;

	return NOERROR;
	}
