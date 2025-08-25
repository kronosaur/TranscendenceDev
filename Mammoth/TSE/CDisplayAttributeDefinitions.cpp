//	CDisplayAttributeDefinitions.cpp
//
//	CDisplayAttributeDefinitions class
//	Copyright (c) 2013 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define ITEM_ATTRIBUTE_TAG						CONSTLIT("ItemAttribute")
#define LOCATION_ATTRIBUTE_TAG					CONSTLIT("LocationAttribute")
#define ON_SHOW_TAG								CONSTLIT("OnShow")

#define ATTRIBUTE_ATTRIB						CONSTLIT("attribute")
#define CRITERIA_ATTRIB							CONSTLIT("criteria")
#define CRITERIA_LABEL_ATTRIB					CONSTLIT("criteriaLabel")
#define ID_ATTRIB								CONSTLIT("id")
#define PERCENT_LOCATIONS_ATTRIB				CONSTLIT("percentLocations")
#define LABEL_ATTRIB							CONSTLIT("label")
#define LABEL_TYPE_ATTRIB						CONSTLIT("labelType")
#define LABEL_COLOR_ATTRIB						CONSTLIT("labelColor")
#define LABEL_TEXT_COLOR_ATTRIB					CONSTLIT("labelTextColor")

#define TYPE_POSITIVE							CONSTLIT("advantage")
#define TYPE_NEGATIVE							CONSTLIT("disadvantage")
#define TYPE_NEUTRAL							CONSTLIT("neutral")

const int DEFAULT_LOCATION_FREQUENCY =			20;
const CDisplayAttributeDefinitions CDisplayAttributeDefinitions::Null;

void CDisplayAttributeDefinitions::AccumulateAttributes (const CItem &Item, TArray<SDisplayAttribute> *retList) const

//	AccumulateAttributes
//
//	Adds display attributes possessed by Item to retList.

	{
	for (int i = 0; i < m_ItemAttribs.GetCount(); i++)
		{
		//	Some display attributes are blank because we use them only for 
		//	balance of trade computations.

		if (m_ItemAttribs[i].sText.IsBlank() && !m_ItemAttribs[i].pOnShow)
			{ }

		//	Skip if we don't match the item criteria

		else if (!Item.MatchesCriteria(m_ItemAttribs[i].Criteria))
			{ }

		//	If we have an OnShow event, call it now

		else if (m_ItemAttribs[i].pOnShow)
			{
			CCodeChainCtx CCX(Item.GetUniverse());

			if (m_ItemAttribs[i].pSourceType)
				CCX.SaveAndDefineType(m_ItemAttribs[i].pSourceType->GetUNID());
			CCX.SaveAndDefineItemVar(Item);

			ICCItemPtr pResult = CCX.RunCode(m_ItemAttribs[i].pOnShow);

			//	If error, display the error as an attribe.

			if (pResult->IsError())
				retList->Insert(SDisplayAttribute(attribNeutral, pResult->GetStringValue(), m_ItemAttribs[i].sID));

			//	Nil means don't show the attribute.

			else if (pResult->IsNil())
				{ }

			//	True means show the attribute label defined in XML

			else if (pResult->IsTrue())
				retList->Insert(SDisplayAttribute(m_ItemAttribs[i].iType, m_ItemAttribs[i].sText, m_ItemAttribs[i].sID, m_ItemAttribs[i].rgbColor, m_ItemAttribs[i].rgbTextColor));

			//	Otherwise, we expect the result to be the attribute label.

			else
				retList->Insert(SDisplayAttribute(m_ItemAttribs[i].iType, pResult->GetStringValue(), m_ItemAttribs[i].sID, m_ItemAttribs[i].rgbColor, m_ItemAttribs[i].rgbTextColor));
			}

		//	Otherwise, we just add it

		else
			retList->Insert(SDisplayAttribute(m_ItemAttribs[i].iType, m_ItemAttribs[i].sText, m_ItemAttribs[i].sID, m_ItemAttribs[i].rgbColor, m_ItemAttribs[i].rgbTextColor));
		}
	}

void CDisplayAttributeDefinitions::Append (const CDisplayAttributeDefinitions &Attribs)

//	Append
//
//	Appends definitions.

	{
	int i;

	if (Attribs.IsEmpty())
		return;

	//	Append item display attributes

	int iDest = m_ItemAttribs.GetCount();
	m_ItemAttribs.InsertEmpty(Attribs.m_ItemAttribs.GetCount());

	for (i = 0; i < Attribs.m_ItemAttribs.GetCount(); i++)
		m_ItemAttribs[iDest++] = Attribs.m_ItemAttribs[i];

	//	Append attribute definitions

	m_Attribs.Merge(Attribs.m_Attribs);
	}

const CDisplayAttributeDefinitions::SItemEntry *CDisplayAttributeDefinitions::FindByCriteria (const CString &sCriteria) const

//	FindByCriteria
//
//	Returns a pointer to the entry that matches the given criteria (or NULL if
//	we cannot find a matching one).

	{
	int i;

	for (i = 0; i < m_ItemAttribs.GetCount(); i++)
		{
		if (strEquals(sCriteria, m_ItemAttribs[i].Criteria.AsString()))
			return &m_ItemAttribs[i];
		}

	return NULL;
	}

const CDisplayAttributeDefinitions::SItemEntry *CDisplayAttributeDefinitions::FindByID (const CString &sID) const

//	FindByID
//
//	Returns a pointer to the entry of the given ID (or NULL if not found).

	{
	int i;

	for (i = 0; i < m_ItemAttribs.GetCount(); i++)
		{
		if (strEquals(sID, m_ItemAttribs[i].sID))
			return &m_ItemAttribs[i];
		}

	return NULL;
	}

const CItemCriteria *CDisplayAttributeDefinitions::FindCriteriaByID (const CString &sID) const

//	FindCriteriaByID
//
//	Finds the criteria for the given ID (NULL if not found).

	{
	const SItemEntry *pEntry = FindByID(sID);
	if (pEntry == NULL)
		return NULL;

	return &pEntry->Criteria;
	}

bool CDisplayAttributeDefinitions::FindCriteriaName (const CString &sCriteria, CString *retsName) const

//	FindCriteriaName
//
//	Looks for the given criteria and returns its name, if found.

	{
	const SItemEntry *pEntry = FindByCriteria(sCriteria);
	if (pEntry == NULL || pEntry->sCriteriaName.IsBlank())
		return false;

	if (retsName)
		*retsName = pEntry->sCriteriaName;

	return true;
	}

bool CDisplayAttributeDefinitions::FindCriteriaNameByID (const CString &sID, CString *retsName) const

//	FindCriteriaNameByID
//
//	Looks for the given criteria and returns its name, if found.

	{
	const SItemEntry *pEntry = FindByID(sID);
	if (pEntry == NULL || pEntry->sCriteriaName.IsBlank())
		return false;

	if (retsName)
		*retsName = pEntry->sCriteriaName;

	return true;
	}

int CDisplayAttributeDefinitions::GetLocationAttribFrequency (const CString &sAttrib) const

//	GetLocationAttribFrequency
//
//	Returns the percent of locations in the universe with the given attribute.

	{
	const SAttribDesc *pEntry = m_Attribs.GetAt(sAttrib);
	if (pEntry == NULL)
		return DEFAULT_LOCATION_FREQUENCY;

	return pEntry->iFrequency;
	}

bool CDisplayAttributeDefinitions::InitFromCCItem (ICCItem *pEntry, SDisplayAttribute &Result)

//	InitFromCCItem
//
//	Initialize from a struct of the form:
//
//	{
//		label: ...
//		labelType: ...
//		}

	{
	CString sLabel;
	EDisplayAttributeTypes iType;
	DWORD dwColor = 0;
	DWORD dwTextColor = 0;
	CString sLoadColor;

	//	If this is a struct, then we expect certain fields.

	if (pEntry->IsSymbolTable())
		{
		sLabel = pEntry->GetStringAt(LABEL_ATTRIB);

		//	Color

		if (dwColor = (DWORD)pEntry->GetIntegerAt(LABEL_COLOR_ATTRIB))
			dwColor = dwColor | 0xff000000;
		else if (sLoadColor = pEntry->GetStringAt(LABEL_COLOR_ATTRIB))
			dwColor = LoadRGBColor(sLoadColor).AsDWORD() | 0xff000000;
		else
			dwColor = 0;

		if (dwTextColor = (DWORD)pEntry->GetIntegerAt(LABEL_TEXT_COLOR_ATTRIB))
			dwTextColor = dwTextColor | 0xff000000;
		else if (sLoadColor = pEntry->GetStringAt(LABEL_TEXT_COLOR_ATTRIB))
			dwTextColor = LoadRGBColor(sLoadColor).AsDWORD() | 0xff000000;
		else
			dwTextColor = 0;

		//	Type

		CString sType = pEntry->GetStringAt(LABEL_TYPE_ATTRIB);

		if (sType.IsBlank() || strEquals(sType, TYPE_NEUTRAL))
			iType = attribNeutral;
		else if (strEquals(sType, TYPE_POSITIVE))
			iType = attribPositive;
		else if (strEquals(sType, TYPE_NEGATIVE))
			iType = attribNegative;
		else
			return false;
		}

	//	Otherwise, this is just a plain attribute

	else
		{
		sLabel = pEntry->GetStringValue();
		iType = attribNeutral;
		}

	//	Done

	if (sLabel.IsBlank())
		return false;

	Result = SDisplayAttribute(iType, sLabel, DWToRGBColor(dwColor), DWToRGBColor(dwTextColor));
	return true;
	}

ALERROR CDisplayAttributeDefinitions::InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	InitFromXML
//
//	Initialize

	{
	CCodeChainCtx CCX(Ctx.GetUniverse());

	for (int i = 0; i < pDesc->GetContentElementCount(); i++)
		{
		CXMLElement *pDef = pDesc->GetContentElement(i);
		if (strEquals(pDef->GetTag(), ITEM_ATTRIBUTE_TAG))
			{
			SItemEntry *pEntry = m_ItemAttribs.Insert();
			pEntry->pSourceType = Ctx.pType;
			pEntry->sID = pDef->GetAttribute(ID_ATTRIB);
			pEntry->sText = pDef->GetAttribute(LABEL_ATTRIB);
			pEntry->sCriteriaName = pDef->GetAttribute(CRITERIA_LABEL_ATTRIB);

			//	Criteria

			pEntry->Criteria.Init(pDef->GetAttribute(CRITERIA_ATTRIB), CItemCriteria::ALL);

			//	Type

			CString sType;
			if (pDef->FindAttribute(LABEL_TYPE_ATTRIB, &sType))
				{
				if (strEquals(sType, TYPE_POSITIVE))
					pEntry->iType = attribPositive;
				else if (strEquals(sType, TYPE_NEGATIVE))
					pEntry->iType = attribNegative;
				else if (strEquals(sType, TYPE_NEUTRAL))
					pEntry->iType = attribNeutral;
				else
					{
					Ctx.sError = strPatternSubst(CONSTLIT("Invalid label type: %s."), sType);
					return ERR_FAIL;
					}
				}
			else
				pEntry->iType = attribNeutral;

			if (Ctx.GetAPIVersion() >= 55)
				{

				//	Color

				CString sLoadColor;

				//if (pDesc->FindAttribute(LABEL_COLOR_ATTRIB, &sLoadColor))
				if (sLoadColor = pDef->GetAttribute(LABEL_COLOR_ATTRIB))
					pEntry->rgbColor = LoadRGBColor(sLoadColor);
				else
					pEntry->rgbColor = DWToARGBColor(0);

				//if (pDesc->FindAttribute(LABEL_TEXT_COLOR_ATTRIB, &sLoadColor))
				if (sLoadColor = pDef->GetAttribute(LABEL_TEXT_COLOR_ATTRIB))
					pEntry->rgbTextColor = LoadRGBColor(sLoadColor);
				else
					pEntry->rgbTextColor = DWToARGBColor(0);

				}

			//	OnShow

			if (CXMLElement *pOnShow = pDef->GetContentElementByTag(ON_SHOW_TAG))
				{
				ICCItemPtr pCode = CCX.LinkCode(pOnShow->GetContentText(0));
				if (pCode->IsError())
					{
					Ctx.sError = strPatternSubst(CONSTLIT("OnShow: %s"), pCode->GetStringValue());
					return ERR_FAIL;
					}

				pEntry->pOnShow = pCode;
				}
			}
		else if (strEquals(pDef->GetTag(), LOCATION_ATTRIBUTE_TAG))
			{
			CString sAttrib = pDef->GetAttribute(ATTRIBUTE_ATTRIB);
			if (sAttrib.IsBlank())
				{
				Ctx.sError = CONSTLIT("Must specify an attribute.");
				return ERR_FAIL;
				}

			SAttribDesc *pEntry = m_Attribs.SetAt(sAttrib);
			pEntry->iType = attribTypeLocation;
			pEntry->sName = sAttrib;
			pEntry->iFrequency = pDef->GetAttributeIntegerBounded(PERCENT_LOCATIONS_ATTRIB, 1, 99, DEFAULT_LOCATION_FREQUENCY);

			if (Ctx.GetAPIVersion() >= 55)
				{

				//	Color

				CString sLoadColor;

				if (pDesc->FindAttribute(LABEL_COLOR_ATTRIB, &sLoadColor))
					pEntry->rgbColor = LoadRGBColor(sLoadColor);
				else
					pEntry->rgbColor = DWToARGBColor(0);

				if (pDesc->FindAttribute(LABEL_TEXT_COLOR_ATTRIB, &sLoadColor))
					pEntry->rgbTextColor = LoadRGBColor(sLoadColor);
				else
					pEntry->rgbTextColor = DWToARGBColor(0);

				}

			}
		else
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Unknown display attribute definition: %s."), pDef->GetTag());
			return ERR_FAIL;
			}
		}

	return NOERROR;
	}
