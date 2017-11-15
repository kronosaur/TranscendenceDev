//	CTopologySystemDesc.cpp
//
//	CTopologySystemDesc class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.
//
//	SYNTAX
//
//	<System [unid=] [name=] [level=] [attributes=] [variant=]/>
//
//	<System [unid=] [name=] [level=] [attributes=] [variant=]>
//		{generator}
//		<Names>...</Names>
//	</System>
//
//	{generator} =
//
//		<System [unid=] [name=] [level=] [attributes=] [variant=]/>
//
//		<Group>
//			{generator1}
//			{generator2}
//			...
//		</Group>
//
//		<NodeDistanceTable distanceTo="{node-criteria}">
//			{generator distance="..."}
//			{generator distance="..."}
//			...
//		</NodeDistanceTable>
//
//		<Null/>
//
//		<Table>
//			{generator chance="..."}
//			{generator chance="..."}
//			...
//		</Table>

#include "PreComp.h"

#define NAMES_TAG								CONSTLIT("Names")
#define SYSTEM_TAG								CONSTLIT("System")

#define ATTRIBUTES_ATTRIB						CONSTLIT("attributes")
#define LEVEL_ATTRIB							CONSTLIT("level")
#define NAME_ATTRIB								CONSTLIT("name")
#define UNID_ATTRIB								CONSTLIT("UNID")
#define VARIANT_ATTRIB							CONSTLIT("variant")

void CTopologySystemDesc::Apply (CTopology &Topology, CTopologyNode *pNode) const

//	Apply
//
//	Applies the attributes to the given node.

	{
	bool bAddSystemAttributes = false;

	//	Apply the basic attributes

	if (m_dwSystemUNID != 0)
		{
		pNode->SetSystemUNID(m_dwSystemUNID);
		bAddSystemAttributes = true;
		}

	if (!m_sName.IsBlank())
		pNode->SetName(m_sName);

	if (m_iLevel != 0)
		pNode->SetLevel(m_iLevel);

	if (!m_sAttributes.IsBlank())
		pNode->AddAttributes(m_sAttributes);

	if (!m_sVariantFromParent.IsBlank())
		pNode->AddVariantLabel(m_sVariantFromParent);

	if (!m_sVariantFromSub.IsBlank())
		pNode->AddVariantLabel(m_sVariantFromSub);

	//	Apply the generator, if we've got one.

	if (m_pGenerator)
		{
		IElementGenerator::SCtx GenCtx;
		GenCtx.pTopology = &Topology;
		GenCtx.pNode = pNode;

		TArray<CXMLElement *> Results;
		m_pGenerator->Generate(GenCtx, Results);

		if (Results.GetCount() > 0)
			{
			CXMLElement *pSystemXML = Results[0];

			DWORD dwUNID;
			if (pSystemXML->FindAttributeInteger(UNID_ATTRIB, (int *)&dwUNID))
				{
				pNode->SetSystemUNID(dwUNID);
				bAddSystemAttributes = true;
				}

			CString sValue;
			if (pSystemXML->FindAttribute(NAME_ATTRIB, &sValue))
				pNode->SetName(sValue);

			int iLevel = pSystemXML->GetAttributeIntegerBounded(LEVEL_ATTRIB, 1, MAX_SYSTEM_LEVEL, 0);
			if (iLevel != 0)
				pNode->SetLevel(iLevel);

			if (pSystemXML->FindAttribute(ATTRIBUTES_ATTRIB, &sValue))
				pNode->AddAttributes(sValue);

			if (pSystemXML->FindAttribute(VARIANT_ATTRIB, &sValue))
				pNode->AddVariantLabel(sValue);
			}
		}

	//	Apply the name generator, if we've got one.

	if (!m_Names.IsEmpty())
		pNode->SetName(m_Names.GenerateName());

	//	If necessary, add attributes from the system type.
	//
	//	LATER: Note that if the system type gets set multiple times, we will not 
	//	clear out the attributes from the previous one. It might be better to set
	//	these attributes later, after all topology processors are done.

	if (bAddSystemAttributes 
			&& pNode->GetSystemTypeUNID())
		{
		CSystemType *pSystemType = g_pUniverse->FindSystemType(pNode->GetSystemTypeUNID());
		if (pSystemType && !pSystemType->GetAttributes().IsBlank())
			pNode->AddAttributes(pSystemType->GetAttributes());
		}
	}

ALERROR CTopologySystemDesc::InitFromXML (SDesignLoadCtx &LoadCtx, CXMLElement *pDesc)

//	InitFromXML
//
//	Loads from XML.

	{
	ALERROR error;
	int i;

	//	Initialize from the root element

	m_dwSystemUNID = pDesc->GetAttributeInteger(UNID_ATTRIB);
	m_sName = pDesc->GetAttribute(NAME_ATTRIB);
	m_iLevel = pDesc->GetAttributeIntegerBounded(LEVEL_ATTRIB, 1, MAX_SYSTEM_LEVEL, 0);
	m_sAttributes = pDesc->GetAttribute(ATTRIBUTES_ATTRIB);
	m_sVariantFromParent = pDesc->GetAttribute(VARIANT_ATTRIB);

	//	Now loop over our sub-elements and process them

	for (i = 0; i < pDesc->GetContentElementCount(); i++)
		{
		CXMLElement *pSub = pDesc->GetContentElement(i);
		const CString &sTag = pSub->GetTag();

		//	A <System> sub-element means that we just apply value on top.

		if (strEquals(sTag, SYSTEM_TAG))
			{
			pSub->FindAttributeInteger(UNID_ATTRIB, (int *)&m_dwSystemUNID);
			pSub->FindAttribute(NAME_ATTRIB, &m_sName);
			pSub->FindAttributeInteger(LEVEL_ATTRIB, &m_iLevel);

			CString sValue;
			if (pSub->FindAttribute(ATTRIBUTES_ATTRIB, &sValue))
				m_sAttributes = ::AppendModifiers(m_sAttributes, sValue);

			m_sVariantFromSub = pSub->GetAttribute(VARIANT_ATTRIB);
			}

		//	A <Names> tag sets random names

		else if (strEquals(sTag, NAMES_TAG))
			{
			if (error = m_Names.InitFromXML(LoadCtx, pSub))
				return error;
			}

		//	Otherwise, if this is a generator tag, we have a generator.

		else if (IElementGenerator::IsGeneratorTag(sTag))
			{
			if (error = IElementGenerator::CreateFromXML(LoadCtx, pSub, m_pGenerator))
				return error;
			}

		//	Otherwise, we don't recognize this tag

		else
			{
			LoadCtx.sError = strPatternSubst(CONSTLIT("Unknown sub-element for <System>: %s."), sTag);
			return ERR_FAIL;
			}
		}

	m_bEmpty = (m_dwSystemUNID == 0 
			&& m_sName.IsBlank() 
			&& m_iLevel == 0 
			&& m_sAttributes.IsBlank() 
			&& m_sVariantFromParent.IsBlank()
			&& m_sVariantFromSub.IsBlank()
			&& !m_pGenerator
			&& m_Names.IsEmpty());

	//	Success

	return NOERROR;
	}
