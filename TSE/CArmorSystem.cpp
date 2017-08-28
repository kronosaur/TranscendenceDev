//	CArmorSystem.cpp
//
//	CArmorSystem class
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

int CArmorSystem::CalcTotalHitPoints (CSpaceObject *pSource, int *retiMaxHP) const

//	CalcTotalHitPoints
//
//	Computes total hit points (and max hit points) for all armor segments.

	{
	int i;

	int iTotalHP = 0;
	int iMaxHP = 0;

	for (i = 0; i < m_Segments.GetCount(); i++)
		{
		iTotalHP += m_Segments[i].GetHitPoints();
		iMaxHP += m_Segments[i].GetMaxHP(pSource);
		}

	if (retiMaxHP)
		*retiMaxHP = iMaxHP;

	return iTotalHP;
	}

void CArmorSystem::Install (CSpaceObject *pObj, const CShipArmorDesc &Desc, bool bInCreate)

//  Install
//
//  Install armor segments

    {
	DEBUG_TRY

    int i;
    ASSERT(m_Segments.GetCount() == 0);

    //  We insert armor segments too

    CItemListManipulator Items(pObj->GetItemList());

    //  Loop over all sections

	m_Segments.InsertEmpty(Desc.GetCount());
	for (i = 0; i < Desc.GetCount(); i++)
		{
        const CShipArmorSegmentDesc &Sect = Desc.GetSegment(i);

		//	Create an armor segment item

        CItem ArmorItem;
        Sect.CreateArmorItem(&ArmorItem);

        //  Add the item. We leave the cursor on the newly created item.

		Items.AddItem(ArmorItem);

		//	Install

		m_Segments[i].Install(pObj, Items, i, bInCreate);
		}

    //  Initialize other properties

    m_iHealerLeft = 0;

	DEBUG_CATCH
    }

void CArmorSystem::ReadFromStream (SLoadCtx &Ctx, CSpaceObject *pObj)

//  ReadFromStream
//
//  Read from stream

    {
    int i;

	DWORD dwCount;
	Ctx.pStream->Read((char *)&dwCount, sizeof(DWORD));
	m_Segments.InsertEmpty(dwCount);
	for (i = 0; i < (int)dwCount; i++)
		m_Segments[i].ReadFromStream(pObj, i, Ctx);

    //  Read other properties

    if (Ctx.dwVersion >= 128)
        Ctx.pStream->Read((char *)&m_iHealerLeft, sizeof(DWORD));
    else
        m_iHealerLeft = 0;
    }

bool CArmorSystem::Update (SUpdateCtx &Ctx, CSpaceObject *pSource, int iTick)

//	Update
//
//	Must be called once per tick to update the armor system. We return TRUE if 
//	the update modified the properties of the armor (e.g., hit points).

	{
	int i;

	//	We only update armor once every 10 ticks.

    if ((iTick % CArmorClass::TICKS_PER_UPDATE) != 0)
		return false;

	//	Update all segments.

	bool bSystemModified = false;
    for (i = 0; i < GetSegmentCount(); i++)
        {
        CInstalledArmor *pArmor = &GetSegment(i);

		bool bArmorModified;
        pArmor->GetClass()->Update(CItemCtx(pSource, pArmor), Ctx, iTick, &bArmorModified);
        if (bArmorModified)
            bSystemModified = true;
        }

	//	Done

	return bSystemModified;
	}

void CArmorSystem::WriteToStream (IWriteStream *pStream)

//  WriteToStream
//
//  Write

    {
    int i;

    DWORD dwSave = m_Segments.GetCount();
	pStream->Write((char *)&dwSave, sizeof(DWORD));
	for (i = 0; i < m_Segments.GetCount(); i++)
		m_Segments[i].WriteToStream(pStream);

    //  Write other properties

	pStream->Write((char *)&m_iHealerLeft, sizeof(DWORD));
    }
