//	CMiningDamageLevelDesc.cpp
//
//	CMiningDamageLevelDesc class
//	Copyright (c) 2012 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define MINING_DAMAGE_LEVEL_ATTRIB					CONSTLIT("miningMaxOreLevel")

int CMiningDamageLevelDesc::GetMaxOreLevel (DamageTypes iDamageType, int iMiningItemLevel) const
	{
	ASSERT(iMiningItemLevel >= 0 && iMiningItemLevel <= MAX_ITEM_LEVEL);

	if (iDamageType == damageGeneric)
		return MAX_MINING_LEVEL;
	else
		{
		switch (m_Desc[iDamageType].iAdjType)
			{
			case levelRelative:
				return Min(Max(1, iMiningItemLevel + m_Desc[iDamageType].iLevelValue), MAX_ITEM_LEVEL);

			case levelAbsolute:
				return m_Desc[iDamageType].iLevelValue;

			default:
				throw CException(ERR_FAIL);
			}
		}
	}

ALERROR CMiningDamageLevelDesc::InitFromArray (const TArray<int>& Levels)

//	InitFromArray
//
//	Initializes from an array. The array must be at least damageCount entries
//	long.
//
//	In this path there is no need for Bind.

	{
	for (int i = 0; i < damageCount; i++)
		{
		if (i < Levels.GetCount())
			{
			m_Desc[i].iAdjType = levelAbsolute;
			m_Desc[i].iLevelValue = Levels[i];
			}
		else
			{
			m_Desc[i].iAdjType = levelRelative;
			m_Desc[i].iLevelValue = 0;
			}
		}

	return NOERROR;
	}

ALERROR CMiningDamageLevelDesc::InitFromMiningDamageLevel (SDesignLoadCtx &Ctx, const CString &sAttrib)

//	InitFromMiningDamageLevel
//
//	Loads a damage adjustment descriptor as follows:
//
//	Absolute levels:
//	5,5,...
//	laser:5; kinetic:5; ...
// 
//	Relative level to mining item:
//	+5,-5,...
//	laser:+5; kinetic:-5; ...
//
//	Absolute max/min:
//  *,0,...
//  laser:*; kinetic:0; ...
//

	{
	ALERROR error;

	//	Short-circuit

	if (sAttrib.IsBlank())
		{
		for (int i = 0; i < damageCount; i++)
			{
			m_Desc[i].iAdjType = levelRelative;
			m_Desc[i].iLevelValue = 0;
			}
		return NOERROR;
		}

	//	We expect a list of per damage max ore level values, either with a damageType
	//	label or ordered by damageType.

	TArray<CString> DamageAdj;
	if (error = ParseDamageTypeList(sAttrib, &DamageAdj))
		{
		Ctx.sError = CONSTLIT("Invalid miningMaxOreLevel definition.");
		return error;
		}

	//	Get level

	for (int i = 0; i < damageCount; i++)
		{

		//	If we have nothing

		if (DamageAdj[i].IsBlank())
			{
			m_Desc[i].iAdjType = levelRelative;
			m_Desc[i].iLevelValue = 0;
			}
		else
			{
			int iValue = strToInt(DamageAdj[i], 0);

			//	If we have a positive item level offset

			if (strStartsWith(DamageAdj[i], CONSTLIT("+")))
				{

				//	Negative values should not be reachable in this code

				if (iValue > MAX_ITEM_LEVEL)
					{
					Ctx.sError = strPatternSubst(CONSTLIT("miningMaxOreLevel value is out of range: %d."), iValue);
					return ERR_FAIL;
					}

				m_Desc[i].iAdjType = levelRelative;
				m_Desc[i].iLevelValue = iValue;
				}

			//	If we have a negative item level offset

			else if (strStartsWith(DamageAdj[i], CONSTLIT("-")))
				{

				//	Positive values should not be reachable in this code

				if (iValue < -1 * MAX_ITEM_LEVEL)
					{
					Ctx.sError = strPatternSubst(CONSTLIT("miningMaxOreLevel value is out of range: %d."), iValue);
					return ERR_FAIL;
					}

				m_Desc[i].iAdjType = levelRelative;
				m_Desc[i].iLevelValue = iValue;
				}

			//	If we have an absolute item level

			else
				{

				//	Negative values should not be reachable in this code

				if (iValue > MAX_ITEM_LEVEL)
					{
					Ctx.sError = strPatternSubst(CONSTLIT("miningMaxOreLevel value is out of range: %d."), iValue);
					return ERR_FAIL;
					}

				m_Desc[i].iAdjType = levelAbsolute;
				m_Desc[i].iLevelValue = iValue;
				}
			}
		}

	//	Done

	return NOERROR;
	}

ALERROR CMiningDamageLevelDesc::InitFromXML (SDesignLoadCtx &Ctx, const CXMLElement &XMLDesc)

//	InitFromXML
//
//	Initialize from XML.
//
//	If bIsDefault is TRUE then we don't need to bind.

	{
	ALERROR error;
	CString sValue;

	if (XMLDesc.FindAttribute(MINING_DAMAGE_LEVEL_ATTRIB, &sValue))
		{
		if (error = InitFromMiningDamageLevel(Ctx, sValue))
			return error;
		}
	else
		{
		Ctx.sError = CONSTLIT("Missing miningMaxOreLevel attribute.");
		return ERR_FAIL;
		}

	//	Done

	return NOERROR;
	}

DamageTypes CMiningDamageLevelDesc::ParseDamageTypeFromProperty (const CString &sProperty)

//	ParseDamageTypeFromProperty
//
//	Returns the damageType from a property encoded as xyz.damageType.
//
//	EXAMPLE
//
//	miningMaxOreLevel.laser
//
//	If no damage type is encoded, we return damageGeneric. If there is a parsing
//	error, we return damageError.

	{
	const char *pPos = sProperty.GetASCIIZPointer();
	while (*pPos != '.' && *pPos != '\0')
		pPos++;

	if (*pPos == '\0')
		return damageGeneric;

	pPos++;
	CString sDamage(pPos);
	return ::LoadDamageTypeFromXML(sDamage);
	}
