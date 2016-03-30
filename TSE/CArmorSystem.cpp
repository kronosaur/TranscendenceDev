//	CArmorSystem.cpp
//
//	CArmorSystem class
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CArmorSystem::Install (CSpaceObject *pObj, const CShipArmorDesc &Desc, bool bInCreate)

//  Install
//
//  Install armor segments

    {
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
