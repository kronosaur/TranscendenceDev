//	CCAtomTable.cpp
//
//	Implements CCAtomTable class

#include "PreComp.h"

static CObjectClass<CCAtomTable>g_Class(OBJID_CCATOMTABLE, NULL);

CCAtomTable::CCAtomTable (void) : ICCAtom(&g_Class)

//	CCAtomTable constructor

	{
	}

ICCItem *CCAtomTable::AddEntry (ICCItem *pAtom, ICCItem *pEntry, bool bForceLocalAdd)

//	AddEntry
//
//	Adds an entry to the symbol table and returns
//	True for success.

	{
	ICCItem *pPrevEntry = NULL;
	int iOldEntry;
	bool bAdded;

	if (m_Table.ReplaceEntry(pAtom->GetIntegerValue(), (int)pEntry->Reference(), true, &bAdded, &iOldEntry) != NOERROR)
		throw CException(ERR_MEMORY);

	//	If we have a previous entry, decrement its refcount since we're
	//	throwing it away

	pPrevEntry = (ICCItem *)iOldEntry;
	if (!bAdded && pPrevEntry)
		pPrevEntry->Discard();

	return CCodeChain::CreateTrue();
	}

ICCItem *CCAtomTable::Clone (CCodeChain *pCC)

//	Clone
//
//	Clone this item

	{
	ASSERT(false);
	return pCC->CreateNil();
	}

void CCAtomTable::DestroyItem (void)

//	DestroyItem
//
//	Destroy this item

	{
	int i;

	//	Release all the entries

	for (i = 0; i < m_Table.GetCount(); i++)
		{
		int iKey, iValue;
		ICCItem *pItem;

		m_Table.GetEntry(i, &iKey, &iValue);
		pItem = (ICCItem *)iValue;
		pItem->Discard();
		}

	//	Remove all symbols

	m_Table.RemoveAll();

	//	Destroy this item

	CCodeChain::DestroyAtomTable(this);
	}

ICCItem *CCAtomTable::ListSymbols (CCodeChain *pCC)

//	ListSymbols
//
//	Returns a list of all the atoms in the table

	{
	//	If there are no symbols, return Nil

	if (m_Table.GetCount() == 0)
		return pCC->CreateNil();

	//	Otherwise, make a list

	else
		{
		int i;
		ICCItem *pResult;
		CCLinkedList *pList;

		pResult = pCC->CreateLinkedList();
		if (pResult->IsError())
			return pResult;

		pList = (CCLinkedList *)pResult;

		for (i = 0; i < m_Table.GetCount(); i++)
			{
			ICCItem *pItem;
			int iKey;

			m_Table.GetEntry(i, &iKey, NULL);

			//	Make an item for the symbol

			pItem = pCC->CreateInteger(iKey);

			//	Add the item to the list

			pList->Append(pItem);
			pItem->Discard();
			}

		return pList;
		}
	}

ICCItem *CCAtomTable::Lookup (CCodeChain *pCC, ICCItem *pAtom)

//	Lookup
//
//	Looks up the key and returns the association. If no
//	Association is found, returns Nil

	{
	return LookupEx(pCC, pAtom, NULL);
	}

ICCItem *CCAtomTable::LookupEx (CCodeChain *pCC, ICCItem *pAtom, bool *retbFound)

//	LookupEx
//
//	Looks up the key and returns the association. If no
//	Association is found, returns an error

	{
	ALERROR error;
	int iValue;
	ICCItem *pBinding;

	if (error = m_Table.Find(pAtom->GetIntegerValue(), &iValue))
		{
		if (error == ERR_NOTFOUND)
			{
			if (retbFound)
				*retbFound = false;

			return pCC->CreateErrorCode(CCRESULT_NOTFOUND);
			}
		else
			throw CException(ERR_MEMORY);
		}

	pBinding = (ICCItem *)iValue;
	ASSERT(pBinding);

	if (retbFound)
		*retbFound = true;

	return pBinding->Reference();
	}

CString CCAtomTable::Print (DWORD dwFlags)

//	Print
//
//	Render as text

	{
	return LITERAL("[atom table]");
	}

void CCAtomTable::Reset (void)

//	Reset
//
//	Reset the internal variables

	{
	m_Table.RemoveAll();
	}

