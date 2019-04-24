//	CItemList.cpp
//
//	CItemList object

#include "PreComp.h"

CItemList CItemList::m_Null;

CItemList::CItemList (void)

//	CItemList constructor

	{
	}

CItemList::CItemList (const CItemList &Src)

//	CItemList constructor

	{
	Copy(Src);
	}

CItemList::~CItemList (void)

//	CItemList destructor

	{
	DeleteAll();
	}

CItemList &CItemList::operator= (const CItemList &Src)

//	CItemList assign operator

	{
	DeleteAll();
	Copy(Src);
	return *this;
	}

void CItemList::AddItem (const CItem &Item)

//	AddItem
//
//	Adds an item to the list

	{
	m_List.Insert(new CItem(Item));
	}

void CItemList::Copy (const CItemList &Src)

//	Copy
//
//	Copy stuff. We assume that we are clean.

	{
	int i;

	if (Src.GetCount() > 0)
		{
		m_List.InsertEmpty(Src.GetCount());

		for (i = 0; i < m_List.GetCount(); i++)
			m_List[i] = new CItem(Src.GetItem(i));
		}
	}

void CItemList::DeleteAll (void)

//	DeleteAll
//
//	Delete all items

	{
	int i;

	for (i = 0; i < m_List.GetCount(); i++)
		delete m_List[i];

	m_List.DeleteAll();
	}

void CItemList::DeleteItem (int iIndex)

//	DeleteItem
//
//	Deletes the item at the index position

	{
	delete m_List[iIndex];
	m_List.Delete(iIndex);
	}

void CItemList::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Reads the item list from a stream
//
//	DWORD		Number of items
//	CItem[number of items]

	{
	int i;
	DWORD dwLoad;

	ASSERT(m_List.GetCount() == 0);

	//	Read the number of items

	DWORD dwCount;
	Ctx.pStream->Read((char *)&dwCount, sizeof(DWORD));

	//	(Previous versions also saved an allocation count)

	if (Ctx.dwVersion < 58)
		Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));

	//	Load the items

	if (dwCount > 0)
		{
		m_List.InsertEmpty(dwCount);

		for (i = 0; i < (int)dwCount; i++)
			{
			m_List[i] = new CItem;
			m_List[i]->ReadFromStream(Ctx);
			}
		}
	}

void CItemList::SortItems (void)

//	SortItems
//
//	Sorts items in order:
//
//	installed/not-installed
//	armor/weapon/device/other

	{
	int i;

	if (GetCount() == 0)
		return;

	TSortMap<CString, int> Sort;
	for (i = 0; i < GetCount(); i++)
		Sort.Insert(GetItem(i).CalcSortKey(), i);

	//	Allocate a new list

	TArray<CItem *> NewList;
	NewList.GrowToFit(GetCount());

	//	Move the items from the old list to the new list in the new order

	for (i = 0; i < GetCount(); i++)
		{
		CItem *pSortedItem = m_List[Sort[i]];
		int iLast = NewList.GetCount() - 1;

		if (iLast >= 0 && NewList[iLast]->IsEqual(*pSortedItem))
			NewList[iLast]->SetCount(NewList[iLast]->GetCount() + pSortedItem->GetCount());
		else
			NewList.Insert(pSortedItem);
		}

	//	Swap

	m_List.TakeHandoff(NewList);
	}

void CItemList::WriteToStream (IWriteStream *pStream)

//	WriteToStream
//
//	Writes the item list to stream
//
//	DWORD		Number of items
//	CItem[number of items]

	{
	int i;

	DWORD dwSave = GetCount();
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	for (i = 0; i < GetCount(); i++)
		GetItem(i).WriteToStream(pStream);
	}
