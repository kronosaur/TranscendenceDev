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
			m_dwItemCategories = 0xFFFFFFFF;
			break;

		default:
			throw CException(ERR_FAIL);
		}
	}

CItemCriteria::CItemCriteria (const CString &sCriteria, DWORD dwDefault)

//	CItemCriteria constructor

	{
	const char *pPos = sCriteria.GetASCIIZPointer();

	//	If null, use default

	if (*pPos == '\0')
		{
		*this = CItemCriteria(dwDefault);
		}

	//	Otherwise, parse

	else
		{
		ParseSubExpression(pPos);
		}
	}

bool CItemCriteria::operator== (const CItemCriteria &Src) const

//	operator ==
//
//	Compare two criteria.

	{
	//	For now we convert to string first.
	return strEquals(AsString(), Src.AsString());
	}

CString CItemCriteria::AsString (void) const

//	AsString
//
//	Converts to a criteria string.

	{
	CMemoryWriteStream Output(64 * 1024);
	if (Output.Create() != NOERROR)
		return NULL_STR;

	WriteSubExpression(Output);

	//	Done

	return CString(Output.GetPointer(), Output.GetLength());
	}

bool CItemCriteria::GetExplicitLevelMatched (int *retiMin, int *retiMax) const

//	GetExplicitLevelMatched
//
//	Returns the min and max levels that this criteria matches, assuming that
//	there are explicit level criteria. If not, we return FALSE. If min or max
//	criteria are missing, we return -1.

	{
	if (!m_sLookup.IsBlank())
		{
		const CItemCriteria *pCriteria = g_pUniverse->GetDesignCollection().GetDisplayAttributes().FindCriteriaByID(m_sLookup);
		if (pCriteria == NULL)
			return false;

		return pCriteria->GetExplicitLevelMatched(retiMin, retiMax);
		}
	else if (m_iEqualToLevel != -1)
		{
		*retiMin = m_iEqualToLevel;
		*retiMax = m_iEqualToLevel;
		return true;
		}
	else if (m_iLessThanLevel == -1 && m_iGreaterThanLevel == -1)
		{ 
		*retiMin = -1;
		*retiMax = -1;
		return false;
		}
	else
		{
		if (m_iLessThanLevel != -1)
			*retiMax = m_iLessThanLevel - 1;
		else
			*retiMax = -1;

		if (m_iGreaterThanLevel != -1)
			*retiMin = m_iGreaterThanLevel + 1;
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

	if (!m_sLookup.IsBlank())
		{
		const CItemCriteria *pCriteria = g_pUniverse->GetDesignCollection().GetDisplayAttributes().FindCriteriaByID(m_sLookup);
		if (pCriteria == NULL)
			return -1;

		return pCriteria->GetMaxLevelMatched();
		}

	if (m_iEqualToLevel != -1)
		return m_iEqualToLevel;

	if (m_iLessThanLevel != -1)
		return m_iLessThanLevel - 1;

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

	if (!m_sLookup.IsBlank())
		{
		CString sName;
		if (!g_pUniverse->GetDesignCollection().GetDisplayAttributes().FindCriteriaNameByID(m_sLookup, &sName))
			return NULL_STR;

		return sName;
		}

	//	Otherwise, we see if this is a shared criteria with a name.

	else
		{
		CString sName;
		CString sCriteria = AsString();
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

bool CItemCriteria::MatchesAttributes (const CItem &Item) const

//	MatchesAttributes
//
//	Returns TRUE if we match the attributes of the item.

	{
	//	Check required modifiers

	for (int i = 0; i < m_ModifiersRequired.GetCount(); i++)
		if (!Item.HasAttribute(m_ModifiersRequired[i]))
			return false;

	//	Check modifiers not allowed

	for (int i = 0; i < m_ModifiersNotAllowed.GetCount(); i++)
		if (Item.HasAttribute(m_ModifiersNotAllowed[i]))
			return false;

	//	Check required special attributes

	for (int i = 0; i < m_SpecialAttribRequired.GetCount(); i++)
		if (!Item.HasSpecialAttribute(m_SpecialAttribRequired[i]))
			return false;

	//	Check special attributes not allowed

	for (int i = 0; i < m_SpecialAttribNotAllowed.GetCount(); i++)
		if (Item.HasSpecialAttribute(m_SpecialAttribNotAllowed[i]))
			return false;

	//	Match

	return true;
	}

bool CItemCriteria::MatchesFlags (DWORD dwFlags) const

//	MatchesFlags
//
//	Returns TRUE if we match the given flags.

	{
	//	If any of the flags that must be set is not set, then
	//	we do not match.

	if ((m_wFlagsMustBeSet & dwFlags) != m_wFlagsMustBeSet)
		return false;

	//	If any of the flags that must be cleared is set, then
	//	we do not match.

	if ((m_wFlagsMustBeCleared & dwFlags) != 0)
		return false;

	//	Match

	return true;
	}

bool CItemCriteria::MatchesFrequency (FrequencyTypes iFreq) const

//	MatchesFrequency
//
//	Returns TRUE if we match the given frequency.

	{
	if (m_Frequency.IsBlank())
		return true;

	char *pPos = m_Frequency.GetASCIIZPointer();
	while (*pPos != '\0')
		{
		switch (*pPos)
			{
			case 'c':
			case 'C':
				if (iFreq == ftCommon)
					return true;
				break;

			case 'u':
			case 'U':
				if (iFreq == ftUncommon)
					return true;
				break;

			case 'r':
			case 'R':
				if (iFreq == ftRare)
					return true;
				break;

			case 'v':
			case 'V':
				if (iFreq == ftVeryRare)
					return true;
				break;

			case '-':
			case 'n':
			case 'N':
				if (iFreq == ftNotRandom)
					return true;
				break;
			}

		pPos++;
		}

	return false;
	}

bool CItemCriteria::MatchesItemCategory (const CItemType &ItemType) const

//	MatchesItemCategory
//
//	Returns TRUE if the given item type matches the categories specified by this
//	criteria.

	{
	//	If we're looking for anything, then continue

	if (m_dwItemCategories == 0xFFFFFFFF)
		{ }

	//	If we're looking for fuel and this item is fuel, then
	//	we continue

	else if ((m_dwItemCategories & itemcatFuel)
			&& ItemType.IsFuel())
		{ }

	//	If we're looking for missiles and this item is a
	//	missile, then we continue.

	else if ((m_dwItemCategories & itemcatMissile)
			&& ItemType.IsMissile())
		{ }

	//	If we're looking for usable items and this item is
	//	isable, then we continue

	else if ((m_dwItemCategories & itemcatUseful)
			&& ItemType.IsUsable())
		{ }

	//	Otherwise, if this is not one of the required categories, bail out

	else if (!(m_dwItemCategories & ItemType.GetCategory()))
		return false;

	//	Deal with exclusion

	if (m_dwExcludeCategories == 0)
		{ }
	else if ((m_dwExcludeCategories & itemcatFuel) && ItemType.IsFuel())
		return false;

	else if ((m_dwExcludeCategories & itemcatMissile) && ItemType.IsMissile())
		return false;

	else if ((m_dwExcludeCategories & itemcatUseful) && ItemType.IsUsable())
		return false;

	else if (m_dwExcludeCategories & ItemType.GetCategory())
		return false;

	//	Deal with must have

	if (m_dwMustHaveCategories != 0)
		{
		if ((m_dwMustHaveCategories & itemcatFuel) && !ItemType.IsFuel())
			return false;

		if ((m_dwMustHaveCategories & itemcatMissile) && !ItemType.IsMissile())
			return false;

		if ((m_dwMustHaveCategories & itemcatUseful) && !ItemType.IsUsable())
			return false;

		if ((m_dwMustHaveCategories & itemcatDeviceMask) == itemcatDeviceMask)
			{
			if (!(ItemType.GetCategory() & itemcatDeviceMask))
				return false;
			}
		else if ((m_dwMustHaveCategories & itemcatWeaponMask) == itemcatWeaponMask)
			{
			if (!(ItemType.GetCategory() & itemcatWeaponMask))
				return false;
			}
		else
			{
			DWORD dwCat = 1;
			for (int i = 0; i < itemcatCount; i++)
				{
				if (dwCat != itemcatFuel && dwCat != itemcatMissile && dwCat != itemcatUseful
						&& (m_dwMustHaveCategories & dwCat)
						&& ItemType.GetCategory() != dwCat)
					return false;
				
				dwCat = dwCat << 1;
				}
			}
		}

	//	If we get this far, then we match.

	return true;
	}

bool CItemCriteria::MatchesLevel (int iLevel) const

//	MatchesLevel
//
//	Returns TRUE if we match the given level.

	{
	if (m_iEqualToLevel != -1 && iLevel != m_iEqualToLevel)
		return false;

	if (m_iGreaterThanLevel != -1 && iLevel <= m_iGreaterThanLevel)
		return false;

	if (m_iLessThanLevel != -1 && iLevel >= m_iLessThanLevel)
		return false;

	return true;
	}

bool CItemCriteria::MatchesMass (int iMassKg) const

//	MatchesMass
//
//	Returns TRUE if we match the given mass.

	{
	if (m_iEqualToMass != -1 && iMassKg != m_iEqualToMass)
		return false;

	if (m_iGreaterThanMass != -1 && iMassKg <= m_iGreaterThanMass)
		return false;

	if (m_iLessThanMass != -1 && iMassKg >= m_iLessThanMass)
		return false;

	return true;
	}

bool CItemCriteria::MatchesPrice (CurrencyValue iValue) const

//	MatchesPrice
//
//	Returns TRUE if we match the given price.

	{
	if (m_iEqualToPrice != -1 && iValue != m_iEqualToPrice)
		return false;

	if (m_iGreaterThanPrice != -1 && iValue <= m_iGreaterThanPrice)
		return false;

	if (m_iLessThanPrice != -1 && iValue >= m_iLessThanPrice)
		return false;

	return true;
	}

void CItemCriteria::ParseSubExpression (const char *pPos, const char **retpPos)

//	ParseSubExpression
//
//	Parses a sub-expression and returns the first character after the end of the
//	sub-expression.
//
//	NOTE: We assume that we are currently in the default state.

	{
	//	Skip leading whitespace

	while (strIsWhitespace(pPos))
		pPos++;

	//	If we start with a brace, then this is a lookup.

	if (*pPos == '{')
		{
		pPos++;

		const char *pStart = pPos;
		while (*pPos != '\0' && *pPos != '}' )
			pPos++;

		m_sLookup = CString(pStart, (int)(pPos - pStart));

		//	No other directives are allowed

		if (*pPos == '}')
			pPos++;

		if (retpPos)
			*retpPos = pPos;
		}

	//	Otherwise, parse a criteria.

	else
		{
		bool bExclude = false;
		bool bMustHave = false;

		while (*pPos != '\0')
			{
			switch (*pPos)
				{
				case '*':
					m_dwItemCategories = 0xFFFFFFFF;
					break;

				case 'a':
					if (bExclude)
						m_dwExcludeCategories |= itemcatArmor;
					else if (bMustHave)
						m_dwMustHaveCategories |= itemcatArmor;
					else
						m_dwItemCategories |= itemcatArmor;
					break;

				case 'b':
					if (bExclude)
						m_dwExcludeCategories |= itemcatMiscDevice;
					else if (bMustHave)
						m_dwMustHaveCategories |= itemcatMiscDevice;
					else
						m_dwItemCategories |= itemcatMiscDevice;
					break;

				case 'c':
					if (bExclude)
						m_dwExcludeCategories |= itemcatCargoHold;
					else if (bMustHave)
						m_dwMustHaveCategories |= itemcatCargoHold;
					else
						m_dwItemCategories |= itemcatCargoHold;
					break;

				case 'd':
					if (bExclude)
						m_dwExcludeCategories |= itemcatMiscDevice
						| itemcatWeapon
						| itemcatLauncher
						| itemcatReactor
						| itemcatShields
						| itemcatCargoHold
						| itemcatDrive;
					else if (bMustHave)
						m_dwMustHaveCategories |= itemcatMiscDevice
						| itemcatWeapon
						| itemcatLauncher
						| itemcatReactor
						| itemcatShields
						| itemcatCargoHold
						| itemcatDrive;
					else
						m_dwItemCategories |= itemcatMiscDevice
						| itemcatWeapon
						| itemcatLauncher
						| itemcatReactor
						| itemcatShields
						| itemcatCargoHold
						| itemcatDrive;
					break;

				case 'f':
					if (bExclude)
						m_dwExcludeCategories |= itemcatFuel;
					else if (bMustHave)
						m_dwMustHaveCategories |= itemcatFuel;
					else
						m_dwItemCategories |= itemcatFuel;
					break;

				case 'l':
					if (bExclude)
						m_dwExcludeCategories |= itemcatLauncher;
					else if (bMustHave)
						m_dwMustHaveCategories |= itemcatLauncher;
					else
						m_dwItemCategories |= itemcatLauncher;
					break;

				case 'm':
					if (bExclude)
						m_dwExcludeCategories |= itemcatMissile;
					else if (bMustHave)
						m_dwMustHaveCategories |= itemcatMissile;
					else
						m_dwItemCategories |= itemcatMissile;
					break;

				case 'p':
					if (bExclude)
						m_dwExcludeCategories |= itemcatWeapon;
					else if (bMustHave)
						m_dwMustHaveCategories |= itemcatWeapon;
					else
						m_dwItemCategories |= itemcatWeapon;
					break;

				case 'r':
					if (bExclude)
						m_dwExcludeCategories |= itemcatReactor;
					else if (bMustHave)
						m_dwMustHaveCategories |= itemcatReactor;
					else
						m_dwItemCategories |= itemcatReactor;
					break;

				case 's':
					if (bExclude)
						m_dwExcludeCategories |= itemcatShields;
					else if (bMustHave)
						m_dwMustHaveCategories |= itemcatShields;
					else
						m_dwItemCategories |= itemcatShields;
					break;

				case 't':
					if (bExclude)
						m_dwExcludeCategories |= itemcatMisc;
					else if (bMustHave)
						m_dwMustHaveCategories |= itemcatMisc;
					else
						m_dwItemCategories |= itemcatMisc;
					break;

				case 'u':
					if (bExclude)
						m_dwExcludeCategories |= itemcatUseful;
					else if (bMustHave)
						m_dwMustHaveCategories |= itemcatUseful;
					else
						m_dwItemCategories |= itemcatUseful;
					break;

				case 'v':
					if (bExclude)
						m_dwExcludeCategories |= itemcatDrive;
					else if (bMustHave)
						m_dwMustHaveCategories |= itemcatDrive;
					else
						m_dwItemCategories |= itemcatDrive;
					break;

				case 'w':
					if (bExclude)
						m_dwExcludeCategories |= itemcatWeapon | itemcatLauncher;
					else if (bMustHave)
						m_dwMustHaveCategories |= itemcatWeapon | itemcatLauncher;
					else
						m_dwItemCategories |= itemcatWeapon | itemcatLauncher;
					break;

				case 'I':
					m_bInstalledOnly = true;
					break;

				case 'D':
					m_wFlagsMustBeSet |= CItem::flagDamaged;
					break;

				case 'F':
					m_Frequency = ParseCriteriaParam(&pPos);
					break;

				case 'L':
					{
					int iHigh;
					int iLow;

					if (ParseCriteriaParamLevelRange(&pPos, &iLow, &iHigh))
						{
						if (iHigh == -1)
							m_iEqualToLevel = iLow;
						else
							{
							m_iGreaterThanLevel = iLow - 1;
							m_iLessThanLevel = iHigh + 1;
							}
						}

					break;
					}

				case 'M':
					m_bLauncherMissileOnly = true;
					m_dwItemCategories |= itemcatMissile;
					break;

				case 'N':
					m_wFlagsMustBeCleared |= CItem::flagDamaged;
					break;

				case 'S':
					m_bUsableItemsOnly = true;
					break;

				case 'U':
					m_bNotInstalledOnly = true;
					break;

				case 'V':
					m_bExcludeVirtual = false;
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
							m_SpecialAttribRequired.Insert(sModifier);
						else
							m_ModifiersRequired.Insert(sModifier);
						}
					else
						{
						if (bSpecialAttrib)
							m_SpecialAttribNotAllowed.Insert(sModifier);
						else
							m_ModifiersNotAllowed.Insert(sModifier);
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
							m_iEqualToPrice = iValue;
						else if (chChar == '>')
							m_iGreaterThanPrice = iValue - iEqualAdj;
						else if (chChar == '<')
							m_iLessThanPrice = iValue + iEqualAdj;
						}

					//	Mass limits

					else if (comparison == '#')
						{
						if (chChar == '=')
							m_iEqualToMass = iValue;
						else if (chChar == '>')
							m_iGreaterThanMass = iValue - iEqualAdj;
						else if (chChar == '<')
							m_iLessThanMass = iValue + iEqualAdj;
						}

					//	Level limits

					else
						{
						if (chChar == '=')
							m_iEqualToLevel = iValue;
						else if (chChar == '>')
							m_iGreaterThanLevel = iValue - iEqualAdj;
						else if (chChar == '<')
							m_iLessThanLevel = iValue + iEqualAdj;
						}

					break;
					}
				}

			pPos++;
			}

		//	Done

		if (retpPos)
			*retpPos = pPos;
		}
	}

void CItemCriteria::WriteCategoryFlags (CMemoryWriteStream &Output, DWORD dwCategories)
	{
	if (dwCategories & itemcatArmor)
		Output.Write("a", 1);
	if (dwCategories & itemcatMiscDevice)
		Output.Write("b", 1);
	if (dwCategories & itemcatCargoHold)
		Output.Write("c", 1);
	if (dwCategories & itemcatFuel)
		Output.Write("f", 1);
	if (dwCategories & itemcatLauncher)
		Output.Write("l", 1);
	if (dwCategories & itemcatMissile)
		Output.Write("m", 1);
	if (dwCategories & itemcatWeapon)
		Output.Write("p", 1);
	if (dwCategories & itemcatReactor)
		Output.Write("r", 1);
	if (dwCategories & itemcatShields)
		Output.Write("s", 1);
	if (dwCategories & itemcatMisc)
		Output.Write("t", 1);
	if (dwCategories & itemcatUseful)
		Output.Write("u", 1);
	if (dwCategories & itemcatDrive)
		Output.Write("v", 1);
	}

void CItemCriteria::WriteSubExpression (CMemoryWriteStream &Output) const

//	WriteSubExpression
//
//	Writes the sub-expression.

	{

	if (!m_sLookup.IsBlank())
		Output.Write(strPatternSubst(CONSTLIT("{%s}"), m_sLookup));

	else
		{
		CString sTerm;

		if (m_dwItemCategories == 0xFFFFFFFF)
			Output.Write("*", 1);
		else
			WriteCategoryFlags(Output, m_dwItemCategories);

		if (m_dwExcludeCategories != 0)
			{
			Output.Write("~", 1);
			WriteCategoryFlags(Output, m_dwExcludeCategories);
			}

		if (m_dwMustHaveCategories != 0)
			{
			Output.Write("&", 1);
			WriteCategoryFlags(Output, m_dwMustHaveCategories);
			}

		if (m_wFlagsMustBeSet & CItem::flagDamaged)
			Output.Write("D", 1);

		if (m_wFlagsMustBeCleared & CItem::flagDamaged)
			Output.Write("N", 1);

		if (m_bUsableItemsOnly)
			Output.Write("S", 1);

		if (m_bInstalledOnly)
			Output.Write("I", 1);

		if (m_bNotInstalledOnly)
			Output.Write("U", 1);

		if (m_bLauncherMissileOnly)
			Output.Write("M", 1);

		if (!m_bExcludeVirtual)
			Output.Write("V", 1);

		for (int i = 0; i < m_ModifiersRequired.GetCount(); i++)
			{
			sTerm = strPatternSubst(CONSTLIT(" +%s;"), m_ModifiersRequired[i]);
			Output.Write(sTerm.GetPointer(), sTerm.GetLength());
			}

		for (int i = 0; i < m_ModifiersNotAllowed.GetCount(); i++)
			{
			sTerm = strPatternSubst(CONSTLIT(" -%s;"), m_ModifiersNotAllowed[i]);
			Output.Write(sTerm.GetPointer(), sTerm.GetLength());
			}

		for (int i = 0; i < m_SpecialAttribRequired.GetCount(); i++)
			{
			sTerm = strPatternSubst(CONSTLIT(" +%s;"), m_SpecialAttribRequired[i]);
			Output.Write(sTerm.GetPointer(), sTerm.GetLength());
			}

		for (int i = 0; i < m_SpecialAttribNotAllowed.GetCount(); i++)
			{
			sTerm = strPatternSubst(CONSTLIT(" +%s;"), m_SpecialAttribNotAllowed[i]);
			Output.Write(sTerm.GetPointer(), sTerm.GetLength());
			}

		if (!m_Frequency.IsBlank())
			{
			sTerm = strPatternSubst(CONSTLIT(" F:%s;"), m_Frequency);
			Output.Write(sTerm.GetPointer(), sTerm.GetLength());
			}

		sTerm = NULL_STR;
		if (m_iEqualToLevel != -1)
			sTerm = strPatternSubst(CONSTLIT(" =%d"), m_iEqualToLevel);
		else if (m_iGreaterThanLevel != -1 && m_iLessThanLevel != -1)
			sTerm = strPatternSubst(CONSTLIT(" L:%d-%d;"), m_iGreaterThanLevel + 1, m_iLessThanLevel - 1);
		else
			{
			if (m_iGreaterThanLevel != -1)
				sTerm = strPatternSubst(CONSTLIT(" >%d"), m_iGreaterThanLevel);
			if (m_iLessThanLevel != -1)
				sTerm = strPatternSubst(CONSTLIT(" <%d"), m_iLessThanLevel);
			}

		if (!sTerm.IsBlank())
			Output.Write(sTerm.GetPointer(), sTerm.GetLength());

		sTerm = NULL_STR;
		if (m_iEqualToPrice != -1)
			sTerm = strPatternSubst(CONSTLIT(" =$%d"), m_iEqualToPrice);
		else
			{
			if (m_iGreaterThanPrice != -1)
				sTerm = strPatternSubst(CONSTLIT(" >=$%d"), m_iGreaterThanPrice + 1);
			if (m_iLessThanPrice != -1)
				sTerm = strPatternSubst(CONSTLIT(" <=$%d"), m_iLessThanPrice - 1);
			}

		if (!sTerm.IsBlank())
			Output.Write(sTerm.GetPointer(), sTerm.GetLength());

		sTerm = NULL_STR;
		if (m_iEqualToMass != -1)
			sTerm = strPatternSubst(CONSTLIT(" =#%d"), m_iEqualToMass);
		else
			{
			if (m_iGreaterThanMass != -1)
				sTerm = strPatternSubst(CONSTLIT(" >=#%d"), m_iGreaterThanMass + 1);
			if (m_iLessThanMass != -1)
				sTerm = strPatternSubst(CONSTLIT(" <=#%d"), m_iLessThanMass - 1);
			}

		if (!sTerm.IsBlank())
			Output.Write(sTerm.GetPointer(), sTerm.GetLength());
		}
	}
