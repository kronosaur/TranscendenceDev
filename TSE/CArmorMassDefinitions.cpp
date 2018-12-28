//	CArmorMassDefintions.cpp
//
//	CArmorMassDefintions class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define CRITERIA_ATTRIB							CONSTLIT("criteria")
#define ID_ATTRIB								CONSTLIT("id")
#define LABEL_ATTRIB							CONSTLIT("label")
#define MASS_ATTRIB								CONSTLIT("mass")

const CArmorMassDefinitions CArmorMassDefinitions::Null;

void CArmorMassDefinitions::Append (const CArmorMassDefinitions &Src)

//	Append
//
//	Appends the source definitions.

	{
	//	Copy all definitions from source to us.

	for (int i = 0; i < Src.m_Definitions.GetCount(); i++)
		m_Definitions.SetAt(Src.m_Definitions.GetKey(i), m_Definitions[i]);
	}

void CArmorMassDefinitions::CalcByIDIndex (void)

//	CalcByIDIndex
//
//	Initialized the by ID index.

	{
	m_ByID.DeleteAll();

	for (int i = 0; i < m_Definitions.GetCount(); i++)
		{
		const SArmorMassDefinition &Def = m_Definitions[i];
		for (int j = 0; j < Def.Classes.GetCount(); j++)
			{
			const SArmorMassEntry &Entry = Def.Classes[j];
			m_ByID.SetAt(Entry.sID, Entry);
			}
		}
	}


const CArmorMassDefinitions::SArmorMassEntry *CArmorMassDefinitions::FindMassEntry (const CItem &Item) const

//	FindMassEntry
//
//	Returns the entry for the given item.

	{
	int iMass = Item.GetMassKg();

	//	First look for any definitions with an explicit criteria.

	for (int i = 0; i < m_Definitions.GetCount(); i++)
		{
		if (m_Definitions.GetKey(i).IsBlank())
			continue;

		const SArmorMassDefinition &Def = m_Definitions[i];
		if (!Item.MatchesCriteria(Def.Criteria))
			continue;

		for (int j = 0; j < Def.Classes.GetCount(); j++)
			if (iMass <= Def.Classes[j].iMaxMass)
				return &Def.Classes[j];
		}

	//	If not found, look for the default definition

	const SArmorMassDefinition *pDef = m_Definitions.GetAt(NULL_STR);
	if (pDef == NULL)
		return NULL;

	for (int j = 0; j < pDef->Classes.GetCount(); j++)
		if (iMass <= pDef->Classes[j].iMaxMass)
			return &pDef->Classes[j];

	return NULL;
	}

const CString &CArmorMassDefinitions::GetMassClassID (const CItem &Item) const

//	GetMassClassID
//
//	Finds the item and returns the mass class ID (or NULL_STR if not found).

	{
	const SArmorMassEntry *pEntry = FindMassEntry(Item);
	if (pEntry == NULL)
		return NULL_STR;

	return pEntry->sID;
	}

const CString &CArmorMassDefinitions::GetMassClassLabel (const CString &sID) const

//	GetMassClassLabel
//
//	Returns the text.

	{
	SArmorMassEntry *pEntry = m_ByID.GetAt(sID);
	if (pEntry == NULL)
		return NULL_STR;

	return pEntry->sText;
	}

ALERROR CArmorMassDefinitions::InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	InitFromXML
//
//	Initialize from XML

	{
	//	Read the criteria

	CString sCriteria = pDesc->GetAttribute(CRITERIA_ATTRIB);

	//	Add a new definitions, for this criteria

	SArmorMassDefinition *pDef = m_Definitions.SetAt(sCriteria);

	//	Parse the criteria

	CItem::ParseCriteria(sCriteria, &pDef->Criteria);

	//	Now read all the mass definitions

	for (int i = 0; i < pDesc->GetContentElementCount(); i++)
		{
		CXMLElement *pEntry = pDesc->GetContentElement(i);

		int iMass = pEntry->GetAttributeIntegerBounded(MASS_ATTRIB, 1, -1, 0);
		if (iMass == 0)
			{
			Ctx.sError = CONSTLIT("Expected mass attributes.");
			m_Definitions.DeleteAll();
			return ERR_FAIL;
			}

		//	Insert

		SArmorMassEntry *pMass = pDef->Classes.SetAt(iMass);
		pMass->sID = pEntry->GetAttribute(ID_ATTRIB);
		pMass->sText = pEntry->GetAttribute(LABEL_ATTRIB);
		pMass->iMaxMass = iMass;
		}

	return NOERROR;
	}
