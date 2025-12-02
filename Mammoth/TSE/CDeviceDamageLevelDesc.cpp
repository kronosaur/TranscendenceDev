//	CDeviceDamageLevelDesc.cpp
//
//	CDeviceDamageLevelDesc class
//	Copyright (c) 2012 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define DEVICE_DAMAGE_LEVEL_ATTRIB					CONSTLIT("deviceDamageMaxDeviceLevel")
#define DEVICE_DAMAGE_ADJ_ATTRIB					CONSTLIT("deviceDamageTypeAdj")
#define DEVICE_DAMAGE_HIT_CHANCE_ATTRIB				CONSTLIT("deviceHitChance")

int CDeviceDamageLevelDesc::GetMaxDeviceLevel (DamageTypes iDamageType, int iWeaponLevel) const
	{
	ASSERT(iWeaponLevel >= 0 && iWeaponLevel <= MAX_ITEM_LEVEL);

	if (iDamageType >= damageMinListed && iDamageType <= damageMaxListed)
		{
		switch (m_Desc[iDamageType].iAdjType)
			{
			case levelRelative:
				return Min(Max(1, iWeaponLevel + m_Desc[iDamageType].iLevelValue), MAX_ITEM_LEVEL);

			case levelAbsolute:
				return m_Desc[iDamageType].iLevelValue;

			default:
				throw CException(ERR_FAIL);
			}
		}

	//	Null and generic are always set to max level
	//	Null is handled specifically by the device damage functions

	else if (iDamageType == damageGeneric || iDamageType == damageNull)
		return MAX_ITEM_LEVEL;

	else
		return 0;
	}

//	InitFromArray
//
//	Initializes from an array. The array must be at least damageCount entries
//	long.
//
//	In this path there is no need for Bind.
//
ALERROR CDeviceDamageLevelDesc::InitFromArray (const TArray<int>& Levels, const TArray<int>& Adj, int iHitChance)

	{
	for (int i = 0; i < damageCount; i++)
		{
		if (Levels[i] >= 0)
			{
			m_Desc[i].iAdjType = levelAbsolute;
			m_Desc[i].iLevelValue = Levels[i];
			m_Adj[i] = Adj[i];
			}
		else
			{
			m_Desc[i].iAdjType = levelRelative;
			m_Desc[i].iLevelValue = 0;
			m_Adj[i] = Adj[i];
			}
		}

	m_iChanceToHit = iHitChance;

	return NOERROR;
	}

//	InitFromExternalDeviceDamageLevel
//
//	Loads a damage adjustment descriptor as follows:
//
//	Absolute levels:
//	5,5,...
//	laser:5; kinetic:5; ...
// 
//	Relative level to target device:
//	+5,-5,...
//	laser:+5; kinetic:-5; ...
//
//	Absolute max/min:
//  *,0,...
//  laser:*; kinetic:0; ...
//
//
ALERROR CDeviceDamageLevelDesc::InitFromDeviceDamageLevel (SDesignLoadCtx &Ctx, const CString &sLevelAttrib, const CString &sAdjAttrib, int iHitChance)

	{
	ALERROR error;

	m_iChanceToHit = iHitChance;

	//	Short-circuit

	bool bEmptyLevels = sLevelAttrib.IsBlank();
	bool bEmptyAdj = sAdjAttrib.IsBlank();

	if (bEmptyLevels)
		{
		for (int i = 0; i < damageCount; i++)
			{
			m_Desc[i].iAdjType = levelAbsolute;
			m_Desc[i].iLevelValue = MAX_DEVICE_LEVEL;
			}
		}

	if (bEmptyAdj)
		{
		for (int i = 0; i < damageCount; i++)
			{
			m_Adj[i] = 100;
			}
		}

	if (bEmptyAdj && bEmptyLevels)
		return NOERROR;

	//	We expect a list of per damage max ore level values, either with a damageType
	//	label or ordered by damageType.

	TArray<CString> DamageAdj;
	if (!bEmptyAdj && (error = ParseDamageTypeList(sAdjAttrib, &DamageAdj)))
		{
		Ctx.sError = CONSTLIT("Invalid deviceDamageTypeAdj definition.");
		return error;
		}

	TArray<CString> DamageLevel;
	if (!bEmptyLevels && (error = ParseDamageTypeList(sLevelAttrib, &DamageLevel)))
		{
		Ctx.sError = CONSTLIT("Invalid deviceDamageMaxDeviceLevel definition.");
		return error;
		}

	//	Get levels and adj

	for (int i = 0; i < damageCount; i++)
		{

		//	Process Adj

		//	If we have nothing

		if (DamageAdj[i].IsBlank())
			{
			m_Adj[i] = 100;
			}
		else
			{
			int iValue = strToInt(DamageAdj[i], 0);

			if (iValue < 0)
				{
				Ctx.sError = strPatternSubst(CONSTLIT("deviceDamageTypeAdj value is out of range: %d"), iValue);
				return ERR_FAIL;
				}

			m_Adj[i] = iValue;
			}

		//	Process Levels

		//	If we have nothing

		if (DamageLevel[i].IsBlank())
			{
			m_Desc[i].iAdjType = levelAbsolute;
			m_Desc[i].iLevelValue = MAX_DEVICE_LEVEL;
			}
		else
			{
			int iValue = strToInt(DamageLevel[i], 0);

			//	If we have a positive item level offset

			if (strStartsWith(DamageLevel[i], CONSTLIT("+")))
				{

				//	Negative values should not be reachable in this code

				if (iValue > MAX_ITEM_LEVEL)
					{
					Ctx.sError = strPatternSubst(CONSTLIT("deviceDamageMaxDeviceLevel value is out of range: %d."), iValue);
					return ERR_FAIL;
					}

				m_Desc[i].iAdjType = levelRelative;
				m_Desc[i].iLevelValue = iValue;
				}

			//	If we have a negative item level offset

			else if (strStartsWith(DamageLevel[i], CONSTLIT("-")))
				{

				//	Positive values should not be reachable in this code

				if (iValue < -1 * MAX_ITEM_LEVEL)
					{
					Ctx.sError = strPatternSubst(CONSTLIT("deviceDamageMaxDeviceLevel value is out of range: %d."), iValue);
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
					Ctx.sError = strPatternSubst(CONSTLIT("deviceDamageMaxDeviceLevel value is out of range: %d."), iValue);
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

//	InitFromXML
//
//	Initialize from XML.
//
//	If bIsDefault is TRUE then we don't need to bind.
//
ALERROR CDeviceDamageLevelDesc::InitFromXML (SDesignLoadCtx &Ctx, const CXMLElement &XMLDesc)

	{
	ALERROR error;
	CString sLevels;
	CString sAdj;

	if (!XMLDesc.FindAttribute(DEVICE_DAMAGE_LEVEL_ATTRIB, &sLevels))
		{
		Ctx.sError = CONSTLIT("Missing deviceDamageMaxDeviceLevel attribute.");
		return ERR_FAIL;
		}

	if (!XMLDesc.FindAttribute(DEVICE_DAMAGE_ADJ_ATTRIB, &sAdj))
		{
		Ctx.sError = CONSTLIT("Missing deviceDamageTypeAdj attribute.");
		return ERR_FAIL;
		}

	if (error = InitFromDeviceDamageLevel(Ctx, sLevels, sAdj, XMLDesc.GetAttributeInteger(DEVICE_DAMAGE_HIT_CHANCE_ATTRIB)))
		return error;

	//	Done

	return NOERROR;
	}

//	ParseDamageTypeFromProperty
//
//	Returns the damageType from a property encoded as xyz.damageType.
//
//	EXAMPLE
//
//	deviceDamageMaxDeviceLevel.laser
//
//	If no damage type is encoded, we return damageGeneric. If there is a parsing
//	error, we return damageError.
//
DamageTypes CDeviceDamageLevelDesc::ParseDamageTypeFromProperty (const CString &sProperty)

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
