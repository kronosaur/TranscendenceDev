//	CItemCriteria.cpp
//
//	CItemCriteria class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CItemCriteria::CItemCriteria (const CItemCriteria &Copy)

//	Criteria copy constructor

	{
	dwItemCategories = Copy.dwItemCategories;
	dwExcludeCategories = Copy.dwExcludeCategories;
	dwMustHaveCategories = Copy.dwMustHaveCategories;
	wFlagsMustBeSet = Copy.wFlagsMustBeSet;
	wFlagsMustBeCleared = Copy.wFlagsMustBeCleared;

	bUsableItemsOnly = Copy.bUsableItemsOnly;
	bExcludeVirtual = Copy.bExcludeVirtual;
	bInstalledOnly = Copy.bInstalledOnly;
	bNotInstalledOnly = Copy.bNotInstalledOnly;
	bLauncherMissileOnly = Copy.bLauncherMissileOnly;

	ModifiersRequired = Copy.ModifiersRequired;
	ModifiersNotAllowed = Copy.ModifiersNotAllowed;
	SpecialAttribRequired = Copy.SpecialAttribRequired;
	SpecialAttribNotAllowed = Copy.SpecialAttribNotAllowed;
	Frequency = Copy.Frequency;

	iEqualToLevel = Copy.iEqualToLevel;
	iGreaterThanLevel = Copy.iGreaterThanLevel;
	iLessThanLevel = Copy.iLessThanLevel;

	iEqualToPrice = Copy.iEqualToPrice;
	iGreaterThanPrice = Copy.iGreaterThanPrice;
	iLessThanPrice = Copy.iLessThanPrice;

	iEqualToMass = Copy.iEqualToMass;
	iGreaterThanMass = Copy.iGreaterThanMass;
	iLessThanMass = Copy.iLessThanMass;

	sLookup = Copy.sLookup;
	pFilter = Copy.pFilter;
	if (pFilter)
		pFilter->Reference();
	}

CItemCriteria &CItemCriteria::operator= (const CItemCriteria &Copy)

//	Criteria equals operator

	{
	if (pFilter)
		pFilter->Discard();

	dwItemCategories = Copy.dwItemCategories;
	dwExcludeCategories = Copy.dwExcludeCategories;
	dwMustHaveCategories = Copy.dwMustHaveCategories;
	wFlagsMustBeSet = Copy.wFlagsMustBeSet;
	wFlagsMustBeCleared = Copy.wFlagsMustBeCleared;

	bUsableItemsOnly = Copy.bUsableItemsOnly;
	bExcludeVirtual = Copy.bExcludeVirtual;
	bInstalledOnly = Copy.bInstalledOnly;
	bNotInstalledOnly = Copy.bNotInstalledOnly;
	bLauncherMissileOnly = Copy.bLauncherMissileOnly;

	ModifiersRequired = Copy.ModifiersRequired;
	ModifiersNotAllowed = Copy.ModifiersNotAllowed;
	SpecialAttribRequired = Copy.SpecialAttribRequired;
	SpecialAttribNotAllowed = Copy.SpecialAttribNotAllowed;
	Frequency = Copy.Frequency;

	iEqualToLevel = Copy.iEqualToLevel;
	iGreaterThanLevel = Copy.iGreaterThanLevel;
	iLessThanLevel = Copy.iLessThanLevel;

	iEqualToPrice = Copy.iEqualToPrice;
	iGreaterThanPrice = Copy.iGreaterThanPrice;
	iLessThanPrice = Copy.iLessThanPrice;

	iEqualToMass = Copy.iEqualToMass;
	iGreaterThanMass = Copy.iGreaterThanMass;
	iLessThanMass = Copy.iLessThanMass;

	sLookup = Copy.sLookup;
	pFilter = Copy.pFilter;
	if (pFilter)
		pFilter->Reference();

	return *this;
	}

CItemCriteria::~CItemCriteria (void)

	{
	if (pFilter)
		pFilter->Discard();
	}

bool CItemCriteria::GetExplicitLevelMatched (int *retiMin, int *retiMax) const

//	GetExplicitLevelMatched
//
//	Returns the min and max levels that this criteria matches, assuming that
//	there are explicit level criteria. If not, we return FALSE. If min or max
//	criteria are missing, we return -1.

	{
	if (!sLookup.IsBlank())
		{
		const CItemCriteria *pCriteria = g_pUniverse->GetDesignCollection().GetDisplayAttributes().FindCriteriaByID(sLookup);
		if (pCriteria == NULL)
			return false;

		return pCriteria->GetExplicitLevelMatched(retiMin, retiMax);
		}
	else if (iEqualToLevel != -1)
		{
		*retiMin = iEqualToLevel;
		*retiMax = iEqualToLevel;
		return true;
		}
	else if (iLessThanLevel == -1 && iGreaterThanLevel == -1)
		{ 
		*retiMin = -1;
		*retiMax = -1;
		return false;
		}
	else
		{
		if (iLessThanLevel != -1)
			*retiMax = iLessThanLevel - 1;
		else
			*retiMax = -1;

		if (iGreaterThanLevel != -1)
			*retiMin = iGreaterThanLevel + 1;
		else
			*retiMin = -1;

		return true;
		}
	}

int CItemCriteria::GetMaxLevelMatched (void) const

//	GetMaxLevelMatches
//
//	Returns the maximum item level that this criteria matches. If there is no
//	explicit level match, then we laboriously check for every single
//	item type that matches and return the max level.

	{
	int i;

	if (!sLookup.IsBlank())
		{
		const CItemCriteria *pCriteria = g_pUniverse->GetDesignCollection().GetDisplayAttributes().FindCriteriaByID(sLookup);
		if (pCriteria == NULL)
			return -1;

		return pCriteria->GetMaxLevelMatched();
		}

	if (iEqualToLevel != -1)
		return iEqualToLevel;

	if (iLessThanLevel != -1)
		return iLessThanLevel - 1;

	//	Look at every single item that might match

	int iMaxLevel = -1;
	for (i = 0; i < g_pUniverse->GetItemTypeCount(); i++)
		{
		CItemType *pType = g_pUniverse->GetItemType(i);
		CItem Item(pType, 1);

		if (Item.MatchesCriteria(*this))
			iMaxLevel = Max(iMaxLevel, pType->GetLevel());
		}

	return iMaxLevel;
	}

CString CItemCriteria::GetName (void) const

//	GetName
//
//	Returns the name of this criteria. We return NULL_STR if we cannot find the name.

	{
	//	If this is a lookup, we get the name from that.

	if (!sLookup.IsBlank())
		{
		CString sName;
		if (!g_pUniverse->GetDesignCollection().GetDisplayAttributes().FindCriteriaNameByID(sLookup, &sName))
			return NULL_STR;

		return sName;
		}

	//	Otherwise, we see if this is a shared criteria with a name.

	else
		{
		CString sName;
		CString sCriteria = CItem::GenerateCriteria(*this);
		if (!g_pUniverse->GetDesignCollection().GetDisplayAttributes().FindCriteriaName(sCriteria, &sName))
			return NULL_STR;

		return sName;
		}
	}

bool CItemCriteria::Intersects (const CItemCriteria &Src) const

//	Intersects
//
//	Returns TRUE if the two criterias match at least one item in common.

	{
	for (int i = 0; i < g_pUniverse->GetItemTypeCount(); i++)
		{
		CItemType *pType = g_pUniverse->GetItemType(i);
		CItem Item(pType, 1);

		if (Item.MatchesCriteria(*this) && Item.MatchesCriteria(Src))
			return true;
		}

	return false;
	}
