//	CArmorMassDefintions.cpp
//
//	CArmorMassDefintions class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define CRITERIA_ATTRIB							CONSTLIT("criteria")
#define ID_ATTRIB								CONSTLIT("id")
#define ID_ALIAS_ATTRIB							CONSTLIT("idAlias")
#define LABEL_ATTRIB							CONSTLIT("label")
#define MASS_ATTRIB								CONSTLIT("mass")
#define VOLUME_ATTRIB							CONSTLIT("volume")

#define ARMOR_CLASS_ELEMENT						CONSTLIT("ArmorClass")
#define ARMOR_CLASS_ELEMENT_LEGACY				CONSTLIT("ArmorMass")
#define ARMOR_CLASS_ALIAS						CONSTLIT("ArmorClassAlias")

const CArmorClassDefinitions CArmorClassDefinitions::Null;

void CArmorClassDefinitions::Append (const CArmorClassDefinitions &Src)

//	Append
//
//	Appends the source definitions.

	{
	//	Copy all definitions from source to us.

	m_Definitions.Merge(Src.m_Definitions);
	InvalidateIDIndex();
	}

CArmorClassDefinitions::SArmorClassEntry *CArmorClassDefinitions::FindClassEntryActual (const CItem &Item)

//	FindClassEntryActual
//
//	Returns the entry for the given item.

	{
	Metric rSize = Item.GetVolume();

	//	First look for any definitions with an explicit criteria.

	for (int i = 0; i < m_Definitions.GetCount(); i++)
		{
		if (m_Definitions.GetKey(i).IsBlank())
			continue;

		SArmorClassDefinition &Def = m_Definitions[i];
		if (!Item.MatchesCriteria(Def.Criteria))
			continue;

		for (int j = 0; j < Def.Classes.GetCount(); j++)
			if (rSize <= Def.Classes[j].rMaxSize)
				return &Def.Classes[j];
		}

	//	If not found, look for the default definition

	SArmorClassDefinition *pDef = m_Definitions.GetAt(NULL_STR);
	if (pDef == NULL)
		return NULL;

	for (int j = 0; j < pDef->Classes.GetCount(); j++)
		if (rSize <= pDef->Classes[j].rMaxSize)
			return &pDef->Classes[j];

	return NULL;
	}

//	FindPreviousArmorClass
//
//	Finds the largest armor class definition that is smaller than the given armor class.
//
bool CArmorClassDefinitions::FindPreviousArmorClass (const CString &sID, CString *retsPrevID, Metric *retrPrevSize) const

	{
	//	Find the armor class by ID

	SArmorClassEntry *pMax;
	if (!m_ByID.Find(sID, &pMax))
		return false;

	//	Now find the definition where this came from

	const SArmorClassDefinition *pDef = m_Definitions.GetAt(pMax->sDefinition);
	if (pDef == NULL)
		return false;

	//	Look through the definition for the previous class.

	int iBest = -1;
	Metric rBestSize = 0;
	for (int i = 0; i < pDef->Classes.GetCount(); i++)
		{
		const SArmorClassEntry &Entry = pDef->Classes[i];

		if (Entry.rMaxSize < pMax->rMaxSize
				&& (iBest == -1 || Entry.rMaxSize > rBestSize))
			{
			iBest = i;
			rBestSize = Entry.rMaxSize;
			}
		}

	//	Done

	if (iBest == -1)
		return false;

	if (retsPrevID)
		*retsPrevID = pDef->Classes[iBest].sID;

	if (retrPrevSize)
		*retrPrevSize = pDef->Classes[iBest].rMaxSize;

	return true;
	}

//	GetFrequencyMax
//
//	Returns the percent of all armor types that are at or below the given armor
//	class. This is used to determine what percent of armor types are usable by
//	a ship class with a given armor size limit.
//
//	E.g., if sID == "heavy" then we return the percent of all armor types that
//	are either ultraLight, light, medium, or heavy.
//
//	NOTE: This call is only valid after all armor types have been bound. You may
//	call this after BindDesign
//
Metric CArmorClassDefinitions::GetFrequencyMax (const CString &sID) const

	{
	//	Find the armor class by ID

	SArmorClassEntry *pMax;
	if (!m_ByID.Find(sID, &pMax))
		return 0.0;

	Metric rMaxSize = pMax->rMaxSize;

	//	Now find the definition where this came from

	const SArmorClassDefinition *pDef = m_Definitions.GetAt(pMax->sDefinition);
	if (pDef == NULL)
		return 0.0;

	//	Add up counts.

	int iCount = 0;
	int iTotal = 0;
	for (int i = 0; i < pDef->Classes.GetCount(); i++)
		{
		const SArmorClassEntry &Entry = pDef->Classes[i];
		iTotal += Entry.iCount;

		if (Entry.rMaxSize <= rMaxSize)
			iCount += Entry.iCount;
		}

	//	Compute the frequency.

	if (iTotal == 0)
		return 0.0;

	return (Metric)iCount / (Metric)iTotal;
	}

//	GetArmorClassID
//
//	Finds the item and returns the armor class ID (or NULL_STR if not found).
//
const CString &CArmorClassDefinitions::GetArmorClassID (const CItem &Item) const

	{
	const SArmorClassEntry *pEntry = FindClassEntry(Item);
	if (pEntry == NULL)
		return NULL_STR;

	return pEntry->sID;
	}

//	GetArmorClassLabel
//
//	Returns the text.
//
const CString &CArmorClassDefinitions::GetArmorClassLabel (const CString &sID) const

	{
	SArmorClassEntry *pEntry;
	if (!m_ByID.Find(sID, &pEntry))
		return NULL_STR;

	return pEntry->sText;
	}

//	GetArmorClassSize
//
//	Returns the size for the given armor class.
//
Metric CArmorClassDefinitions::GetArmorClassSize (const CString &sID) const

	{
	SArmorClassEntry *pEntry;
	if (!m_ByID.Find(sID, &pEntry))
		return 0;

	return pEntry->rMaxSize;
	}

//	InitFromXML
//
//	Initialize from XML
//
ALERROR CArmorClassDefinitions::InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

	{
	//	Read the criteria

	CString sCriteria = pDesc->GetAttribute(CRITERIA_ATTRIB);

	//	Add a new definitions, for this criteria

	SArmorClassDefinition *pDef = m_Definitions.SetAt(sCriteria);

	//	Parse the criteria

	pDef->Criteria.Init(sCriteria);

	//	Now read all the mass definitions

	int iArmorClass = 0;
	TSortMap<CString, CString> Aliases;

	for (int i = 0; i < pDesc->GetContentElementCount(); i++)
		{
		CXMLElement *pEntry = pDesc->GetContentElement(i);

		CString sTag = pEntry->GetTag();

		if (strEquals(sTag, ARMOR_CLASS_ELEMENT) || strEquals(sTag, ARMOR_CLASS_ELEMENT_LEGACY) || Ctx.GetAPIVersion() < 59)
			{

			int iMass = pEntry->GetAttributeIntegerBounded(MASS_ATTRIB, 1, -1, 0);
			Metric rVolume = pEntry->GetAttributeDoubleBounded(VOLUME_ATTRIB, g_Epsilon, -1.0, 0.0);

			if (iMass && !rVolume)
				{
				Metric rDefaultMassToVolume = g_pUniverse->GetEngineOptions().GetItemXMLMassToVolumeRatio();
				rVolume = rDefaultMassToVolume * iMass / 1000.0;
				}

			if (rVolume < g_Epsilon)
				{
				Ctx.sError = CONSTLIT("Expected volume or mass attributes.");
				CleanupDefinitions();
				return ERR_FAIL;
				}

			//	Insert

			SArmorClassEntry *pClass = pDef->Classes.SetAt(iArmorClass);
			pClass->sDefinition = sCriteria;
			pClass->sID = pEntry->GetAttribute(ID_ATTRIB);
			pClass->sText = pEntry->GetAttribute(LABEL_ATTRIB);
			pClass->rMaxSize = rVolume;

			pDef->Ids.Insert(pClass->sID, iArmorClass);

			//	Prepare for next loop

			iArmorClass++;
			}
		else if (strEquals(sTag, ARMOR_CLASS_ALIAS))
			{
			CString sId = pEntry->GetAttribute(ID_ATTRIB);
			CString sAlias = pEntry->GetAttribute(ID_ALIAS_ATTRIB);

			if (sId.IsBlank() || sAlias.IsBlank())
				{
				Ctx.sError = strPatternSubst(CONSTLIT("Armor class aliases require both id and idAlias to be defined and non-empty."));
				CleanupDefinitions();
				return ERR_FAIL;
				}

			Aliases.Insert(sAlias, sId);
			}
		else
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Expected ArmorClass or ArmorClassAlias but got %s instead."), sTag);
			CleanupDefinitions();
			return ERR_FAIL;
			}

		}

	for (int i = 0; i < Aliases.GetCount(); i++)
		{
		CString sAlias = Aliases.GetKey(i);
		CString sId = Aliases.GetValue(i);

		if (Aliases.Find(sId))
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Attempted to add an alias (%s) that aliases another alias: %s"), sAlias, sId);
			CleanupDefinitions();
			return ERR_FAIL;
			}
		
		if (pDef->Ids.Find(sAlias))
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Attempted to add an alias (%s) that already exists as an Id"), sAlias);
			CleanupDefinitions();
			return ERR_FAIL;
			}

		int iClass;

		if (!pDef->Ids.Find(sId, &iClass))
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Attempted to add an alias (%s) for an id that does not exist (%s)"), sAlias, sId);
			CleanupDefinitions();
			return ERR_FAIL;
			}

		pDef->Ids.Insert(sAlias, iClass);
		}

	InvalidateIDIndex();

	return NOERROR;
	}

//	OnBindArmor
//
//	This is called when we bind an armor type so that we can keep track of how
//	many armor types for each class.
//
void CArmorClassDefinitions::OnBindArmor (SDesignLoadCtx &Ctx, const CItem &Item, CString *retsArmorClass)

	{
	SArmorClassEntry *pEntry = FindClassEntryActual(Item);
	if (pEntry == NULL)
		{
		if (retsArmorClass) *retsArmorClass = NULL_STR;
		return;
		}

	//	Update the counts

	pEntry->iCount++;

	//	Return the mass class

	if (retsArmorClass)
		*retsArmorClass = pEntry->sID;
	}

//	OnInitDone
//
//	This is called just before we start calling Bind on an armor type.
//
void CArmorClassDefinitions::OnInitDone (void)

	{
	//	Initialize the index.

	m_ByID.DeleteAll();

	for (int i = 0; i < m_Definitions.GetCount(); i++)
		{
		SArmorClassDefinition &Def = m_Definitions[i];
		for (int j = 0; j < Def.Classes.GetCount(); j++)
			{
			SArmorClassEntry &Entry = Def.Classes[j];

			//	While we're here, we initialize the counts, since we will soon
			//	get called at OnBindArmor.

			Entry.iCount = 0;

			//	Add to the index.

			m_ByID.SetAt(Entry.sID, &Entry);
			}
		}
	}
