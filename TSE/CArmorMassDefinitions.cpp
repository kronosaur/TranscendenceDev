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

	m_Definitions.Merge(Src.m_Definitions);
	InvalidateIDIndex();
	}

CArmorMassDefinitions::SArmorMassEntry *CArmorMassDefinitions::FindMassEntryActual (const CItem &Item) const

//	FindMassEntryActual
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

Metric CArmorMassDefinitions::GetFrequencyMax (const CString &sID) const

//	GetFrequencyMax
//
//	Returns the percent of all armor types that are at or below the given mass
//	class. This is used to determine what percent of armor types are usable by
//	a ship class with a given armor mass limit.
//
//	E.g., if sID == "heavy" then we return the percent of all armor types that
//	are either ultraLight, light, medium, or heavy.

	{
	//	Find the armor class by ID

	SArmorMassEntry *pMax;
	if (!m_ByID.Find(sID, &pMax))
		return 0.0;

	int iMaxMass = pMax->iMaxMass;

	//	Now find the definition where this came from

	SArmorMassDefinition *pDef = m_Definitions.GetAt(pMax->sDefinition);
	if (pDef == NULL)
		return 0.0;

	//	Add up counts.

	int iCount = 0;
	int iTotal = 0;
	for (int i = 0; i < pDef->Classes.GetCount(); i++)
		{
		const SArmorMassEntry &Entry = pDef->Classes[i];
		iTotal += Entry.iCount;

		if (Entry.iMaxMass <= iMaxMass)
			iCount += Entry.iCount;
		}

	//	Compute the frequency.

	if (iTotal == 0)
		return 0.0;

	return (Metric)iCount / (Metric)iTotal;
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
	SArmorMassEntry *pEntry;
	if (!m_ByID.Find(sID, &pEntry))
		return NULL_STR;

	return pEntry->sText;
	}

int CArmorMassDefinitions::GetMassClassMass (const CString &sID) const

//	GetMassClassMass
//
//	Returns the mass for the given mass class.

	{
	SArmorMassEntry *pEntry;
	if (!m_ByID.Find(sID, &pEntry))
		return 0;

	return pEntry->iMaxMass;
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
		pMass->sDefinition = sCriteria;
		pMass->sID = pEntry->GetAttribute(ID_ATTRIB);
		pMass->sText = pEntry->GetAttribute(LABEL_ATTRIB);
		pMass->iMaxMass = iMass;
		}

	InvalidateIDIndex();

	return NOERROR;
	}

void CArmorMassDefinitions::OnBindArmor (SDesignLoadCtx &Ctx, const CItem &Item, CString *retsMassClass)

//	OnBindArmor
//
//	This is called when we bind an armor type so that we can keep track of how
//	many armor types for each class.

	{
	SArmorMassEntry *pEntry = FindMassEntryActual(Item);
	if (pEntry == NULL)
		{
		if (retsMassClass) *retsMassClass = NULL_STR;
		return;
		}

	//	Update the counts

	pEntry->iCount++;

	//	Return the mass class

	if (retsMassClass)
		*retsMassClass = pEntry->sID;
	}

void CArmorMassDefinitions::OnInitDone (void)

//	OnInitDone
//
//	This is called just before we start calling Bind on an armor type.

	{
	//	Initialize the index.

	m_ByID.DeleteAll();

	for (int i = 0; i < m_Definitions.GetCount(); i++)
		{
		SArmorMassDefinition &Def = m_Definitions[i];
		for (int j = 0; j < Def.Classes.GetCount(); j++)
			{
			SArmorMassEntry &Entry = Def.Classes[j];

			//	While we're here, we initialize the counts, since we will soon
			//	get called at OnBindArmor.

			Entry.iCount = 0;

			//	Add to the index.

			m_ByID.SetAt(Entry.sID, &Entry);
			}
		}
	}
