//	CShipArmorDesc.cpp
//
//	CShipArmorDesc class
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define ARMOR_ID_ATTRIB							CONSTLIT("armorID")
#define COUNT_ATTRIB							CONSTLIT("count")
#define LEVEL_ATTRIB               				CONSTLIT("level")
#define START_AT_ATTRIB            				CONSTLIT("startAt")

void CShipArmorDesc::AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) const

//  AddTypesUsed
//
//  Adds the set of types used to the list.

    {
    int i;

	for (i = 0; i < GetCount(); i++)
		retTypesUsed->SetAt(GetSegment(i).GetArmorClass()->GetItemType()->GetUNID(), true);
    }

ALERROR CShipArmorDesc::Bind (SDesignLoadCtx &Ctx)

//  Bind
//
//  Bind design

    {
    int i;
    ALERROR error;

	for (i = 0; i < m_Segments.GetCount(); i++)
		{
        if (error = m_Segments[i].Bind(Ctx))
            return error;
		}

    return NOERROR;
    }

Metric CShipArmorDesc::CalcMass (void) const

//  CalcMass
//
//  Calculates the mass of all segments

    {
    int i;

    Metric rMass = 0.0;
	for (i = 0; i < GetCount(); i++)
		{
        CItem ArmorItem;
        GetSegment(i).CreateArmorItem(&ArmorItem);
        
		rMass += ArmorItem.GetMass();
		}

    return rMass;
    }

int CShipArmorDesc::GetSegmentAtAngle (int iAngle) const

//  GetSegmentAtAngle
//
//  Returns the index of the segment at the given angle

    {
	int i;

	for (i = 0; i < GetCount(); i++)
		{
        if (GetSegment(i).AngleInSegment(iAngle))
            return i;
		}

	//	The last hull section may wrap around to the beginning again. If we haven't
	//	found the angle yet, assume it is the last section.

	return GetCount() - 1;
    }

CString CShipArmorDesc::GetSegmentName (int iIndex) const

//  GetSegmentName
//
//  Returns the name of the segment

    {
	if (iIndex < 0 || iIndex >= m_Segments.GetCount())
		return NULL_STR;

    int iCenter = m_Segments[iIndex].GetCenterAngle();
	if (iCenter >= 315)
		return CONSTLIT("forward");
	else if (iCenter >= 225)
		return CONSTLIT("starboard");
	else if (iCenter >= 135)
		return CONSTLIT("aft");
	else if (iCenter >= 45)
		return CONSTLIT("port");
	else
		return CONSTLIT("forward");
    }

ALERROR CShipArmorDesc::InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//  InitFromXML
//
//  Initializes from XML

    {
    int i;
    ALERROR error;

    ASSERT(pDesc);

	//	If no content, then we assume a regular distribution of armor

	if (pDesc->GetContentElementCount() == 0)
		{
		int iSegCount = pDesc->GetAttributeIntegerBounded(COUNT_ATTRIB, 1, -1, 4);
		int iSegSize = Max(1, 360 / iSegCount);

        //  Get all the parameters for the segment

        DWORD dwSegUNID;
        if (error = ::LoadUNID(Ctx, pDesc->GetAttribute(ARMOR_ID_ATTRIB), &dwSegUNID))
            return error;

		int iSegPos;
		if (!pDesc->FindAttributeInteger(START_AT_ATTRIB, &iSegPos))
			iSegPos = 360 - (iSegSize / 2);

        int iSegLevel = pDesc->GetAttributeIntegerBounded(LEVEL_ATTRIB, 1, MAX_ITEM_LEVEL, -1);

        CRandomEnhancementGenerator SegEnhancement;
        if (error = SegEnhancement.InitFromXML(Ctx, pDesc))
            return error;

		m_Segments.InsertEmpty(iSegCount);
		for (i = 0; i < iSegCount; i++)
			{
            if (error = m_Segments[i].Init(iSegPos, iSegSize, dwSegUNID, iSegLevel, SegEnhancement))
                return error;

			iSegPos += iSegSize;
			}
		}

	//	Otherwise, we load each segment separately.

	else
		{
		m_Segments.InsertEmpty(pDesc->GetContentElementCount());
		for (i = 0; i < pDesc->GetContentElementCount(); i++)
			{
            if (error = m_Segments[i].InitFromXML(Ctx, pDesc->GetContentElement(i)))
                return error;
			}
		}

    return NOERROR;
    }
