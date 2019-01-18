//	CCompositeImageSelector.cpp
//
//	CCompositeImageSelector class
//	Copyright (c) 2011 by George Moromisato. All Rights Reserved.

#include "PreComp.h"

static CObjectImageArray EMPTY_IMAGE;
CCompositeImageSelector CCompositeImageSelector::g_NullSelector;

bool CCompositeImageSelector::operator== (const CCompositeImageSelector &Val) const

//	Operator ==

	{
	int i;

	if (m_Sel.GetCount() != Val.m_Sel.GetCount())
		return false;

	for (i = 0; i < m_Sel.GetCount(); i++)
		{
		const SEntry *pThis = &m_Sel[i];
		const SEntry *pVal = &Val.m_Sel[i];

		if (pThis->dwID != pVal->dwID)
			return false;

		if (pThis->iVariant != pVal->iVariant)
			return false;

		if (pThis->dwExtra != pVal->dwExtra)
			return false;
		}

	return true;
	}

void CCompositeImageSelector::AddFlotsam (DWORD dwID, CItemType *pItemType)

//	AddFlotsam
//
//	Adds an item image representation

	{
	SEntry *pEntry = m_Sel.Insert();

	pEntry->dwID = dwID;
	pEntry->dwExtra = (DWORD)pItemType;
	pEntry->iVariant = -1;
	}

void CCompositeImageSelector::AddShipwreck (DWORD dwID, CShipClass *pWreckClass, int iVariant)

//	AddShipwreck
//
//	Adds a shipwreck selection

	{
	SEntry *pEntry = m_Sel.Insert();

	pEntry->dwID = dwID;
	pEntry->dwExtra = (DWORD)pWreckClass;
	pEntry->iVariant = 0;
	}

void CCompositeImageSelector::AddVariant (DWORD dwID, int iVariant)

//	AddVariant
//
//	Adds a variant selection

	{
	SEntry *pEntry = m_Sel.Insert();
	pEntry->dwID = dwID;
	pEntry->iVariant = iVariant;
	pEntry->dwExtra = 0;
	}

const CCompositeImageSelector::SEntry *CCompositeImageSelector::FindEntry (DWORD dwID) const

//	FindEntry
//
//	Finds the entry with the given ID (or NULL)

	{
	int i;

	for (i = 0; i < m_Sel.GetCount(); i++)
		if (m_Sel[i].dwID == dwID)
			return &m_Sel[i];

	return NULL;
	}

CCompositeImageSelector::ETypes CCompositeImageSelector::GetEntryType (const SEntry &Entry) const

//	GetEntryType
//
//	Returns the type of entry

	{
	if (Entry.dwExtra == 0)
		return typeVariant;
	else if (Entry.iVariant == -1)
		return typeItemType;
	else
		return typeShipClass;
	}

CObjectImageArray &CCompositeImageSelector::GetFlotsamImage (DWORD dwID) const

//	GetFlotsamImage
//
//	Returns the image

	{
	CItemType *pItemType = GetFlotsamType(dwID);
	if (pItemType == NULL)
		return EMPTY_IMAGE;

	return pItemType->GetFlotsamImage();
	}

CItemType *CCompositeImageSelector::GetFlotsamType (DWORD dwID) const

//	GetFlotsamType
//
//	Returns the flotsam type

	{
	const SEntry *pEntry = FindEntry(dwID);
	if (pEntry == NULL || GetEntryType(*pEntry) != typeItemType)
		return NULL;

	return (CItemType *)pEntry->dwExtra;
	}

DWORD CCompositeImageSelector::GetHash (void) const

//  GetHash
//
//  Returns a hash for the structure

    {
    int i;

    if (m_Sel.GetCount() == 0)
        return 0;

    DWORD dwHash = GetHash(m_Sel[0]);
    for (i = 1; i < m_Sel.GetCount(); i++)
        dwHash = dwHash ^ (GetHash(m_Sel[i]) << i);

    return dwHash;
    }

DWORD CCompositeImageSelector::GetHash (const SEntry &Entry) const

//  GetHash
//
//  Hashes an entry

    {
    return ((Entry.dwID << 16) | ((DWORD)(Entry.iVariant) & 0xffff)) ^ Entry.dwExtra;
    }

CShipClass *CCompositeImageSelector::GetShipwreckClass (DWORD dwID) const

//	GetShipwreckClass
//
//	Returns the shipwreck class for the given selection

	{
	const SEntry *pEntry = FindEntry(dwID);
	if (pEntry == NULL || GetEntryType(*pEntry) != typeShipClass)
		return NULL;

	return (CShipClass *)pEntry->dwExtra;
	}

CCompositeImageSelector::ETypes CCompositeImageSelector::GetType (DWORD dwID) const

//	GetType
//
//	Returns the type of selector

	{
	const SEntry *pEntry = FindEntry(dwID);
	if (pEntry == NULL)
		return typeNone;

	return GetEntryType(*pEntry);
	}

int CCompositeImageSelector::GetVariant (DWORD dwID) const

//	GetVariant
//
//	Returns the variant for the given selection

	{
	const SEntry *pEntry = FindEntry(dwID);
	if (pEntry == NULL)
		return 0;

	return pEntry->iVariant;
	}

void CCompositeImageSelector::ReadFromItem (ICCItemPtr pData)

//	ReadFromItem
//
//	Reads from a stored item.

	{
	int i;

	CCodeChain &CC = g_pUniverse->GetCC();

	m_Sel.DeleteAll();
	if (pData->IsNil() || pData->GetCount() == 0 || !pData->IsList())
		return;

	m_Sel.InsertEmpty(pData->GetCount());
	for (i = 0; i < m_Sel.GetCount(); i++)
		{
		ICCItem *pEntry = pData->GetElement(i);

		m_Sel[i].dwID = (DWORD)pEntry->GetIntegerAt(CONSTLIT("id"));
		m_Sel[i].iVariant = (DWORD)pEntry->GetIntegerAt(CONSTLIT("variant"));
		m_Sel[i].dwExtra = 0;

		DWORD dwUNID = (DWORD)pEntry->GetIntegerAt(CONSTLIT("itemType"));
		if (dwUNID)
			{
			CItemType *pItemType = g_pUniverse->FindItemType(dwUNID);
			if (pItemType)
				m_Sel[i].dwExtra = (DWORD)pItemType;
			else
				m_Sel[i].dwExtra = 0;
			}
		else
			{
			dwUNID = (DWORD)pEntry->GetIntegerAt(CONSTLIT("shipClass"));
			if (dwUNID)
				{
				CShipClass *pShipClass = g_pUniverse->FindShipClass(dwUNID);
				if (pShipClass)
					m_Sel[i].dwExtra = (DWORD)pShipClass;
				else
					m_Sel[i].dwExtra = 0;
				}
			}
		}
	}

void CCompositeImageSelector::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Load from a stream
//
//	DWORD		No of entries
//	For each
//	DWORD		dwID
//	DWORD		iVariant
//	DWORD		Shipwreck UNID or ItemType UNID

	{
	int i;
	DWORD dwLoad;

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	if (dwLoad > 0)
		{
		ASSERT(m_Sel.GetCount() == 0);
		m_Sel.InsertEmpty(dwLoad);

		for (i = 0; i < m_Sel.GetCount(); i++)
			{
			Ctx.pStream->Read((char *)&m_Sel[i].dwID, sizeof(DWORD));
			Ctx.pStream->Read((char *)&m_Sel[i].iVariant, sizeof(DWORD));

			Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
			if (dwLoad == 0)
				m_Sel[i].dwExtra = 0;
			else if (m_Sel[i].iVariant == -1)
				m_Sel[i].dwExtra = (DWORD)g_pUniverse->FindItemType(dwLoad);
			else
				m_Sel[i].dwExtra = (DWORD)g_pUniverse->FindShipClass(dwLoad);
			}
		}
	}

ICCItemPtr CCompositeImageSelector::WriteToItem (void) const

//	WriteToItem
//
//	Writes to an ICCItem

	{
	int i;

	CCodeChain &CC = g_pUniverse->GetCC();
	if (m_Sel.GetCount() == 0)
		return ICCItemPtr(ICCItem::Nil);

	ICCItemPtr pSel(CC.CreateLinkedList());
	for (i = 0; i < m_Sel.GetCount(); i++)
		{
		ICCItemPtr pEntry(CC.CreateSymbolTable());
		pSel->Append(CC, pEntry);

		pEntry->SetIntegerAt(CC, CONSTLIT("id"), m_Sel[i].dwID);
		pEntry->SetIntegerAt(CC, CONSTLIT("variant"), m_Sel[i].iVariant);

		ETypes iType = GetEntryType(m_Sel[i]);
		switch (iType)
			{
			case typeItemType:
				{
				CItemType *pItemType = (CItemType *)m_Sel[i].dwExtra;
				pEntry->SetIntegerAt(CC, CONSTLIT("itemType"), pItemType->GetUNID());
				break;
				}

			case typeShipClass:
				{
				CShipClass *pWreckClass = (CShipClass *)m_Sel[i].dwExtra;
				pEntry->SetIntegerAt(CC, CONSTLIT("shipClass"), pWreckClass->GetUNID());
				break;
				}
			}
		}

	return pSel;
	}

void CCompositeImageSelector::WriteToStream (IWriteStream *pStream) const

//	WriteToStream
//
//	Writes to a stream
//
//	DWORD		No of entries
//	For each
//	DWORD		dwID
//	DWORD		iVariant
//	DWORD		Shipwreck UNID or ItemType UNID

	{
	int i;
	DWORD dwSave;

	dwSave = m_Sel.GetCount();
	pStream->Write((char *)&dwSave, sizeof(DWORD));
	
	//	Save each entry

	for (i = 0; i < m_Sel.GetCount(); i++)
		{
		pStream->Write((char *)&m_Sel[i].dwID, sizeof(DWORD));
		pStream->Write((char *)&m_Sel[i].iVariant, sizeof(DWORD));

		ETypes iType = GetEntryType(m_Sel[i]);
		switch (iType)
			{
			case typeItemType:
				{
				CItemType *pItemType = (CItemType *)m_Sel[i].dwExtra;
				dwSave = pItemType->GetUNID();
				break;
				}

			case typeShipClass:
				{
				CShipClass *pWreckClass = (CShipClass *)m_Sel[i].dwExtra;
				dwSave = pWreckClass->GetUNID();
				break;
				}

			default:
				dwSave = 0;
			}

		pStream->Write((char *)&dwSave, sizeof(DWORD));
		}
	}
