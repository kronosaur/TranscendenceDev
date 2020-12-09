//	CShipArmorSegmentDesc.cpp
//
//	CShipArmorSegmentDesc class
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define ARMOR_ID_ATTRIB							CONSTLIT("armorID")
#define LEVEL_ATTRIB               				CONSTLIT("level")
#define NON_CRITICAL_ATTRIB						CONSTLIT("nonCritical")
#define SPAN_ATTRIB               				CONSTLIT("span")
#define START_ATTRIB            				CONSTLIT("start")

bool CShipArmorSegmentDesc::AngleInSegment (int iAngle) const

//  AngleInSegment
//
//  Returns TRUE if the given angle is in the segment

    {
	int iStart = m_iStartAt;
    int iEnd = AngleMod(m_iStartAt + m_iSpan);

	if (iEnd > iStart)
		{
		if (iAngle >= iStart && iAngle < iEnd)
			return true;
		}
	else
		{
		if (iAngle < iEnd || iAngle >= iStart)
			return true;
		}

    return false;
    }

void CShipArmorSegmentDesc::ApplyOverride (const CShipArmorSegmentDesc &Override)

//	ApplyOverride
//
//	Apply the override.

	{
	if (Override.m_pArmor.GetUNID())
		m_pArmor.SetUNID(Override.m_pArmor.GetUNID());

	if (Override.m_iLevel != -1)
		m_iLevel = Override.m_iLevel;

	if (!Override.m_Enhanced.IsEmpty())
		m_Enhanced = Override.m_Enhanced;
	}

ALERROR CShipArmorSegmentDesc::Bind (SDesignLoadCtx &Ctx)

//  Bind
//
//  Bind design

    {
    ALERROR error;

    if (error = m_pArmor.Bind(Ctx))
        return error;

	//	Must have armor

	if (m_pArmor == NULL)
		{
		Ctx.sError = CONSTLIT("Armor section must specify valid armor.");
        return ERR_FAIL;
		}

    return NOERROR;
    }

bool CShipArmorSegmentDesc::CreateArmorItem (CItem *retItem, CString *retsError) const

//  CreateArmorItem
//
//  Creates an armor item as specified. Returns TRUE if successful.

    {
    *retItem = CItem(m_pArmor->GetItemType(), 1);
	m_Enhanced.EnhanceItem(*retItem);
    if (m_iLevel != -1)
        {
        if (!retItem->SetLevel(m_iLevel, retsError))
            return false;
        }

    return true;
    }

CItem CShipArmorSegmentDesc::GetArmorItem (void) const

//	GetArmorItem
//
//	Creates an armor item.

	{
	CItem ArmorItem(m_pArmor->GetItemType(), 1);
	m_Enhanced.EnhanceItem(ArmorItem);
	if (m_iLevel != -1)
		ArmorItem.SetLevel(m_iLevel);

	return ArmorItem;
	}

int CShipArmorSegmentDesc::GetLevel (void) const

//  GetLevel
//
//  Returns the armor segment level.

    {
    return (m_iLevel != -1 ? m_iLevel : m_pArmor->GetItemType()->GetLevel());
    }

ALERROR CShipArmorSegmentDesc::Init (int iStartAt, int iSpan, DWORD dwArmorUNID, int iLevel, const CRandomEnhancementGenerator &Enhancement)

//  Init
//
//  Initialize from parameters

    {
    m_iStartAt = AngleMod(iStartAt);
    m_iSpan = Min(Max(0, iSpan), 360);
    m_pArmor.SetUNID(dwArmorUNID);
    m_iLevel = iLevel;
    m_Enhanced = Enhancement;
    m_dwAreaSet = CShipClass::sectCritical;

    return NOERROR;
    }

ALERROR CShipArmorSegmentDesc::InitFromXML (SDesignLoadCtx &Ctx, 
											const CXMLElement &Desc, 
											DWORD dwDefaultUNID, 
											int iDefaultLevel, 
											int iDefaultAngle, 
											const CRandomEnhancementGenerator &DefaultEnhancement, 
											int *retiSpan)

//  InitFromXML
//
//  Initialize from an XML element

    {
    ALERROR error;

	if (!Desc.FindAttributeInteger(START_ATTRIB, &m_iStartAt))
		m_iStartAt = iDefaultAngle;

	m_iSpan = Desc.GetAttributeInteger(SPAN_ATTRIB);
	if (retiSpan)
		*retiSpan = m_iSpan;

	CString sArmorID;
	if (Desc.FindAttribute(ARMOR_ID_ATTRIB, &sArmorID))
		{
		if (error = m_pArmor.LoadUNID(Ctx, sArmorID))
			return error;
		}
	else
		m_pArmor.SetUNID(dwDefaultUNID);

    m_iLevel = Desc.GetAttributeIntegerBounded(LEVEL_ATTRIB, 1, MAX_ITEM_LEVEL, iDefaultLevel);

    if (error = m_Enhanced.InitFromXML(Ctx, &Desc))
        return error;

	if (m_Enhanced.IsEmpty())
		m_Enhanced = DefaultEnhancement;

	m_dwAreaSet = ParseNonCritical(Desc.GetAttribute(NON_CRITICAL_ATTRIB));

    return NOERROR;
    }

DWORD CShipArmorSegmentDesc::ParseNonCritical (const CString &sList)

//	ParseNonCritical
//
//	Returns the set of non-critical areas

	{
	//	These must match the order of VitalSections in TSE.h

	static const char *g_pszNonCritical[] =
		{
		"dev0",
		"dev1",
		"dev2",
		"dev3",
		"dev4",
		"dev5",
		"dev6",
		"dev7",

		"maneuver",
		"drive",
		"scanners",
		"tactical",
		"cargo",

		"",
		};

	//	Blank means critical

	if (sList.IsBlank())
		return CShipClass::sectCritical;

	//	Loop over list

	DWORD dwSet = 0;
	char *pPos = sList.GetASCIIZPointer();
	while (*pPos != '\0')
		{
		//	Trim spaces

		while (*pPos != '\0' && *pPos == ' ')
			pPos++;

		//	Which of the items do we match?

		int i = 0;
		DWORD dwArea = 0x1;
		const char *pFind;
		while (*(pFind = g_pszNonCritical[i]))
			{
			char *pSource = pPos;

			while (*pFind != '\0' && *pFind == *pSource)
				{
				pFind++;
				pSource++;
				}

			//	If we matched then we've got this area

			if (*pFind == '\0' && (*pSource == ' ' || *pSource == ';' || *pSource == '\0'))
				{
				dwSet |= dwArea;
				pPos = pSource;
				break;
				}

			//	Next

			i++;
			dwArea = dwArea << 1;
			}

		//	Skip to the next modifier

		while (*pPos != '\0' && *pPos != ';')
			pPos++;

		if (*pPos == ';')
			pPos++;
		}

	//	Done

	return dwSet;
	}
