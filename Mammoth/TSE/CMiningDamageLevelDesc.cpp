//	CMiningDamageLevelDesc.cpp
//
//	CMiningDamageLevelDesc class
//	Copyright (c) 2012 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define MINING_DAMAGE_LEVEL_ATTRIB					CONSTLIT("miningMaxOreLevel")
#define MINING_RELATIVE_OFFSET						2 * MAX_ITEM_LEVEL

static int DAMAGE_LEVEL_API48[damageCount] =
	//	las   kin   par   bls   ion   the   pos   plm   ant   nan   grv   sin   dac   dst   dlg   dfr

	{     5,    5,    8,    8,   11,   11,   14,   14,   17,   17,   20,   20,   23,   23,   25,   25 };

//	This compatibility table is loaded for pre-API48 adventures
static int DAMAGE_LEVEL_COMPATIBILITY[damageCount] =
	//	las   kin   par   bls   ion   the   pos   plm   ant   nan   grv   sin   dac   dst   dlg   dfr

	{    25,   25,   25,   25,   25,   25,   25,   25,   25,   25,   25,   25,   25,   25,   25,   25 };

ALERROR CMiningDamageLevelDesc::Bind (SDesignLoadCtx &Ctx, const CMiningDamageLevelDesc *pDefault)

//	Bind
//
//	Bind the design

	{
	Compute(pDefault);
	return NOERROR;
	}

void CMiningDamageLevelDesc::Compute (const CMiningDamageLevelDesc *pDefault)

//	Compute
//
//	Compute the m_iMiningLevel table based on the descriptors

	{
	int i;

	m_pDefault = pDefault;

	for (i = 0; i < damageCount; i++)
		{
		switch (m_Desc[i].dwLevelType)
			{
			case levelDefault:
				if (pDefault)
					m_iMiningLevel[i] = pDefault->GetMaxOreLevel((DamageTypes)i);
				else
					m_iMiningLevel[i] = MAX_ITEM_LEVEL;
				break;

			case levelAbsolute:
				m_iMiningLevel[i] = (int)(DWORD)m_Desc[i].dwLevelValue;
				break;

			case levelRelative:
				if (pDefault)
					m_iMiningLevel[i] = MINING_RELATIVE_OFFSET + (int)(DWORD)m_Desc[i].dwLevelValue;
				else
					m_iMiningLevel[i] = MAX_ITEM_LEVEL;
				break;

			default:
				ASSERT(false);
			}
		}
	}

void CMiningDamageLevelDesc::GetMaxOreLevelAndDefault (DamageTypes iDamageType, int *retiAdj, int *retiDefault) const

//	GetAdjAndDefault
//
//	Returns the adjustment and the default

	{
	if (retiAdj)
		*retiAdj = GetMaxOreLevel(iDamageType);

	if (retiDefault)
		*retiDefault = (m_pDefault ? m_pDefault->GetMaxOreLevel(iDamageType) : MAX_ITEM_LEVEL);
	}

ALERROR CMiningDamageLevelDesc::InitFromArray (int *pTable)

//	InitFromArray
//
//	Initializes from an array. The array must be at least damageCount entries
//	long.
//
//	In this path there is no need for Bind.

	{
	int i;

	for (i = 0; i < damageCount; i++)
		{
		m_Desc[i].dwLevelType = levelAbsolute;
		m_Desc[i].dwLevelValue = (DWORD)pTable[i];

		m_iMiningLevel[i] = pTable[i];
		}

	return NOERROR;
	}

ALERROR CMiningDamageLevelDesc::InitFromMiningDamageLevel (SDesignLoadCtx &Ctx, const CString &sAttrib, bool bNoDefault)

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
	int i;

	//	Short-circuit

	if (sAttrib.IsBlank())
		{
		for (i = 0; i < damageCount; i++)
			{
			m_Desc[i].dwLevelType = (bNoDefault ? levelAbsolute : levelDefault);
			m_Desc[i].dwLevelValue = (bNoDefault ? 100 : 0);
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

	for (i = 0; i < damageCount; i++)
		{

		//	If we have nothing

		if (DamageAdj[i].IsBlank())
			{
			m_Desc[i].dwLevelType = (bNoDefault ? levelAbsolute : levelDefault);
			m_Desc[i].dwLevelValue = (bNoDefault ? MAX_ITEM_LEVEL : 0);
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

				m_Desc[i].dwLevelType = levelRelative;
				m_Desc[i].dwLevelValue = (WORD)iValue;

				//	If this table doesn't have a default, then it is the default table.
				//	Initialize m_iMiningLevel so we don't have to bind.

				if (bNoDefault)
					m_iMiningLevel[i] = MINING_RELATIVE_OFFSET + iValue;

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

				m_Desc[i].dwLevelType = levelRelative;
				m_Desc[i].dwLevelValue = (WORD)iValue;

				//	If this table doesn't have a default, then it is the default table.
				//	Initialize m_iMiningLevel so we don't have to bind.

				if (bNoDefault)
					m_iMiningLevel[i] = MINING_RELATIVE_OFFSET + iValue;

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

				m_Desc[i].dwLevelType = levelAbsolute;
				m_Desc[i].dwLevelValue = (WORD)iValue;

				//	If this table doesn't have a default, then it is the default table.
				//	Initialize m_iMiningLevel so we don't have to bind.

				if (bNoDefault)
					m_iMiningLevel[i] = iValue;
				}
			}
		}

	//	Done

	return NOERROR;
	}

ALERROR CMiningDamageLevelDesc::InitFromXML (SDesignLoadCtx &Ctx, const CXMLElement &XMLDesc, bool bIsDefault)

//	InitFromXML
//
//	Initialize from XML.
//
//	If bIsDefault is TRUE then we don't need to bind.

	{
	ALERROR error;
	int i;
	CString sValue;

	if (XMLDesc.FindAttribute(MINING_DAMAGE_LEVEL_ATTRIB, &sValue))
		{
		if (error = InitFromMiningDamageLevel(Ctx, sValue, bIsDefault))
			return error;
		}
	else
		{
		if (bIsDefault)
			{
			Ctx.sError = CONSTLIT("Default miningMaxOreLevel tables must have absolute values.");
			return ERR_FAIL;
			}

		for (i = 0; i < damageCount; i++)
			{
			m_Desc[i].dwLevelType = levelDefault;
			m_Desc[i].dwLevelValue = 0;
			}
		}

	//	Done

	return NOERROR;
	}

bool CMiningDamageLevelDesc::IsEmpty (void) const

//	IsEmpty
//
//	Returns TRUE if all values are default.

	{
	int i;

	for (i = 0; i < damageCount; i++)
		if (m_Desc[i].dwLevelType != levelDefault)
			return false;

	return true;
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
