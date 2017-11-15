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

	m_pSystemXML = pDesc;
	const CString &sTag = m_pSystemXML->GetTag();

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

	return NOERROR;
	}

ALERROR CApplySystemProc::OnProcess (CSystemMap *pMap, CTopology &Topology, CTopologyNodeList &NodeList, CString *retsError)

//	OnProcess
//
//	Process on topology

	{
	int i;

	//	Apply system properties to all nodes in list

	for (i = 0; i < NodeList.GetCount(); i++)
		{
		CTopologyNode *pNode = NodeList[i];

		if (!m_sAttributes.IsBlank())
			pNode->AddAttributes(m_sAttributes);

		if (!m_SystemDesc.IsEmpty())
			m_SystemDesc.Apply(Topology, pNode);
		}

	//	Remove from the original node list

	NodeList.DeleteAll();

	return NOERROR;
	}
