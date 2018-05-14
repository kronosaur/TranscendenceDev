//	CItemLevelCriteria.cpp
//
//	CItemLevelCriteria class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define TYPE_LEVEL								CONSTLIT("level")
#define TYPE_REPAIR_LEVEL						CONSTLIT("repairLevel")

ALERROR CItemLevelCriteria::InitFromXML (SDesignLoadCtx &Ctx, const CString &sValue)

//	InitFromXML
//
//	Initializes criteria.

	{
	if (sValue.IsBlank())
		m_iType = checkNone;
	else if (CXMLElement::IsBoolTrueValue(sValue))
		m_iType = checkLessEqualToLevel;
	else if (strEquals(sValue, TYPE_LEVEL))
		m_iType = checkLessEqualToLevel;
	else if (strEquals(sValue, TYPE_REPAIR_LEVEL))
		m_iType = checkLessEqualToRepairLevel;
	else
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Unknown levelCheck: %s"), sValue);
		return ERR_FAIL;
		}

	return NOERROR;
	}

bool CItemLevelCriteria::MatchesCriteria (int iLevel, const CItem &Item) const

//	MatchesCriteria
//
//	Returns TRUE if the given item matches our defined criteria.

	{
	if (Item.IsEmpty())
		return false;

	//	Start by getting the item level that we want to compare.

	int iItemLevel = 0;
	switch (m_iType)
		{
		case checkNone:
			return true;

		case checkLessEqualToLevel:
		case checkLessThanLevel:
		case checkGreaterEqualToLevel:
		case checkGreaterThanLevel:
			iItemLevel = Item.GetLevel();
			break;

		case checkLessEqualToRepairLevel:
		case checkLessThanRepairLevel:
		case checkGreaterEqualToRepairLevel:
		case checkGreaterThanRepairLevel:
			{
			if (!Item.IsArmor())
				return false;

			CArmorClass *pArmor = Item.GetType()->GetArmorClass();
			if (pArmor == NULL)
				return false;

			iItemLevel = pArmor->GetRepairTech();
			break;
			}

		default:
			ASSERT(false);
			return true;
		}

	//	Now do a comparison

	switch (m_iType)
		{
		case checkLessEqualToLevel:
		case checkLessEqualToRepairLevel:
			return (iItemLevel <= iLevel);

		case checkLessThanLevel:
		case checkLessThanRepairLevel:
			return (iItemLevel < iLevel);

		case checkGreaterEqualToLevel:
		case checkGreaterEqualToRepairLevel:
			return (iItemLevel >= iLevel);

		case checkGreaterThanRepairLevel:
		case checkGreaterThanLevel:
			return (iItemLevel > iLevel);

		default:
			ASSERT(false);
			return false;
		}
	}

void CItemLevelCriteria::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	DWORD		m_iType

	{
	DWORD dwLoad;
	Ctx.pStream->Read(dwLoad);

	m_iType = (ETypes)dwLoad;
	}

void CItemLevelCriteria::WriteToStream (IWriteStream &Stream) const

//	WriteToStream
//
//	DWORD		m_iType

	{
	Stream.Write((DWORD)m_iType);
	}
