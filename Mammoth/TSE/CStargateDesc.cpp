//	CStargateDesc.cpp
//
//	CStargateDesc class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define DEST_ENTRY_POINT_ATTRIB					CONSTLIT("destEntryPoint")	
#define DEST_NODE_ATTRIB						CONSTLIT("destNodeID")
#define GATE_EFFECT_ATTRIB						CONSTLIT("gateEffect")

ALERROR CStargateDesc::InitFromStationTypeXML (SDesignLoadCtx &Ctx, const CXMLElement &Desc)

//	InitFromStationTypeXML
//
//	Initializes from XML.

	{
	m_sDestNodeID = Desc.GetAttribute(DEST_NODE_ATTRIB);
	m_sDestEntryPoint = Desc.GetAttribute(DEST_ENTRY_POINT_ATTRIB);

	if (ALERROR error = m_pGateEffect.LoadUNID(Ctx, Desc.GetAttribute(GATE_EFFECT_ATTRIB)))
		return error;

	return NOERROR;
	}
