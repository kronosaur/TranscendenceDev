//	CItemCriteria.cpp
//
//	CItemCriteria class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CItemCriteria::CItemCriteria (DWORD dwSpecial)

//	CItemCriteria constructor

	{
	switch (dwSpecial)
		{
		case NONE:
			break;

		case ALL:
			dwItemCategories = 0xFFFFFFFF;
			break;

		default:
			throw CException(ERR_FAIL);
		}
	}

CItemCriteria::CItemCriteria (const CString &sCriteria, DWORD dwDefault)

//	CItemCriteria constructor

	{
	bool bExclude = false;
	bool bMustHave = false;

	//	Parse string

	const char *pPos = sCriteria.GetASCIIZPointer();

	//	If null, use default

	if (*pPos == '\0')
		{
		*this = CItemCriteria(dwDefault);
		return;
		}

	//	If we start with a brace, then this is a lookup.

	else if (*pPos == '{')
		{
		pPos++;

		const char *pStart = pPos;
		while (*pPos != '\0' && *pPos != '}' )
			pPos++;

		sLookup = CString(pStart, (int)(pPos - pStart));

		//	No other directives are allowed

		return;
		}

	//	Otherwise, parse a criteria.

	while (*pPos != '\0')
		{
		switch (*pPos)
			{
			case '*':
				dwItemCategories = 0xFFFFFFFF;
				break;

			case 'a':
				if (bExclude)
					dwExcludeCategories |= itemcatArmor;
				else if (bMustHave)
					dwMustHaveCategories |= itemcatArmor;
				else
					dwItemCategories |= itemcatArmor;
				break;

			case 'b':
				if (bExclude)
					dwExcludeCategories |= itemcatMiscDevice;
				else if (bMustHave)
					dwMustHaveCategories |= itemcatMiscDevice;
				else
					dwItemCategories |= itemcatMiscDevice;
				break;

			case 'c':
				if (bExclude)
					dwExcludeCategories |= itemcatCargoHold;
				else if (bMustHave)
					dwMustHaveCategories |= itemcatCargoHold;
				else
					dwItemCategories |= itemcatCargoHold;
				break;

			case 'd':
				if (bExclude)
					dwExcludeCategories |= itemcatMiscDevice
					| itemcatWeapon
					| itemcatLauncher
					| itemcatReactor
					| itemcatShields
					| itemcatCargoHold
					| itemcatDrive;
				else if (bMustHave)
					dwMustHaveCategories |= itemcatMiscDevice
					| itemcatWeapon
					| itemcatLauncher
					| itemcatReactor
					| itemcatShields
					| itemcatCargoHold
					| itemcatDrive;
				else
					dwItemCategories |= itemcatMiscDevice
					| itemcatWeapon
					| itemcatLauncher
					| itemcatReactor
					| itemcatShields
					| itemcatCargoHold
					| itemcatDrive;
				break;

			case 'f':
				if (bExclude)
					dwExcludeCategories |= itemcatFuel;
				else if (bMustHave)
					dwMustHaveCategories |= itemcatFuel;
				else
					dwItemCategories |= itemcatFuel;
				break;

			case 'l':
				if (bExclude)
					dwExcludeCategories |= itemcatLauncher;
				else if (bMustHave)
					dwMustHaveCategories |= itemcatLauncher;
				else
					dwItemCategories |= itemcatLauncher;
				break;

			case 'm':
				if (bExclude)
					dwExcludeCategories |= itemcatMissile;
				else if (bMustHave)
					dwMustHaveCategories |= itemcatMissile;
				else
					dwItemCategories |= itemcatMissile;
				break;

			case 'p':
				if (bExclude)
					dwExcludeCategories |= itemcatWeapon;
				else if (bMustHave)
					dwMustHaveCategories |= itemcatWeapon;
				else
					dwItemCategories |= itemcatWeapon;
				break;

			case 'r':
				if (bExclude)
					dwExcludeCategories |= itemcatReactor;
				else if (bMustHave)
					dwMustHaveCategories |= itemcatReactor;
				else
					dwItemCategories |= itemcatReactor;
				break;

			case 's':
				if (bExclude)
					dwExcludeCategories |= itemcatShields;
				else if (bMustHave)
					dwMustHaveCategories |= itemcatShields;
				else
					dwItemCategories |= itemcatShields;
				break;

			case 't':
				if (bExclude)
					dwExcludeCategories |= itemcatMisc;
				else if (bMustHave)
					dwMustHaveCategories |= itemcatMisc;
				else
					dwItemCategories |= itemcatMisc;
				break;

			case 'u':
				if (bExclude)
					dwExcludeCategories |= itemcatUseful;
				else if (bMustHave)
					dwMustHaveCategories |= itemcatUseful;
				else
					dwItemCategories |= itemcatUseful;
				break;

			case 'v':
				if (bExclude)
					dwExcludeCategories |= itemcatDrive;
				else if (bMustHave)
					dwMustHaveCategories |= itemcatDrive;
				else
					dwItemCategories |= itemcatDrive;
				break;

			case 'w':
				if (bExclude)
					dwExcludeCategories |= itemcatWeapon | itemcatLauncher;
				else if (bMustHave)
					dwMustHaveCategories |= itemcatWeapon | itemcatLauncher;
				else
					dwItemCategories |= itemcatWeapon | itemcatLauncher;
				break;

			case 'I':
				bInstalledOnly = true;
				break;

			case 'D':
				wFlagsMustBeSet |= CItem::flagDamaged;
				break;

			case 'F':
				Frequency = ParseCriteriaParam(&pPos);
				break;

			case 'L':
				{
				int iHigh;
				int iLow;

				if (ParseCriteriaParamLevelRange(&pPos, &iLow, &iHigh))
					{
					if (iHigh == -1)
						iEqualToLevel = iLow;
					else
						{
						iGreaterThanLevel = iLow - 1;
						iLessThanLevel = iHigh + 1;
						}
					}

				break;
				}

			case 'M':
				bLauncherMissileOnly = true;
				dwItemCategories |= itemcatMissile;
				break;

			case 'N':
				wFlagsMustBeCleared |= CItem::flagDamaged;
				break;

			case 'S':
				bUsableItemsOnly = true;
				break;

			case 'U':
				bNotInstalledOnly = true;
				break;

			case 'V':
				bExcludeVirtual = false;
				break;

			case '+':
			case '-':
				{
				bool bSpecialAttrib = false;
				char chChar = *pPos;
				pPos++;

				//	Deal with category exclusion

				//	Get the modifier

				const char *pStart = pPos;
				while (*pPos != '\0' && *pPos != ';' && *pPos != ' ' && *pPos != '\t')
					{
					if (*pPos == ':')
						bSpecialAttrib = true;
					pPos++;
					}

				CString sModifier = CString(pStart, pPos - pStart);

				//	Required or Not Allowed

				if (chChar == '+')
					{
					if (bSpecialAttrib)
						SpecialAttribRequired.Insert(sModifier);
					else
						ModifiersRequired.Insert(sModifier);
					}
				else
					{
					if (bSpecialAttrib)
						SpecialAttribNotAllowed.Insert(sModifier);
					else
						ModifiersNotAllowed.Insert(sModifier);
					}

				//	No trailing semi

				if (*pPos == '\0')
					pPos--;

				break;
				}

			case '~':
				bExclude = true;
				break;

			case '^':
				bMustHave = true;
				break;

			case '=':
			case '>':
			case '<':
				{
				char chChar = *pPos;
				pPos++;

				//	<= or >=

				int iEqualAdj;
				if (*pPos == '=')
					{
					pPos++;
					iEqualAdj = 1;
					}
				else
					iEqualAdj = 0;

				//	Is this price?

				char comparison;
				if (*pPos == '$' || *pPos == '#')
					comparison = *pPos++;
				else
					comparison = '\0';

				//	Get the number

				const char *pNewPos;
				int iValue = strParseInt(pPos, 0, &pNewPos);

				//	Back up one because we will increment at the bottom
				//	of the loop.

				if (pPos != pNewPos)
					pPos = pNewPos - 1;

				//	Price limits

				if (comparison == '$')
					{
					if (chChar == '=')
						iEqualToPrice = iValue;
					else if (chChar == '>')
						iGreaterThanPrice = iValue - iEqualAdj;
					else if (chChar == '<')
						iLessThanPrice = iValue + iEqualAdj;
					}

				//	Mass limits

				else if (comparison == '#')
					{
					if (chChar == '=')
						iEqualToMass = iValue;
					else if (chChar == '>')
						iGreaterThanMass = iValue - iEqualAdj;
					else if (chChar == '<')
						iLessThanMass = iValue + iEqualAdj;
					}

				//	Level limits

				else
					{
					if (chChar == '=')
						iEqualToLevel = iValue;
					else if (chChar == '>')
						iGreaterThanLevel = iValue - iEqualAdj;
					else if (chChar == '<')
						iLessThanLevel = iValue + iEqualAdj;
					}

				break;
				}
			}

		pPos++;
		}
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
