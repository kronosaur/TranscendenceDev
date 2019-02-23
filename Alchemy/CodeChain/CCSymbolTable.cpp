//	CCSymbolTable.cpp
//
//	Implements CCSymbolTable class

#include "PreComp.h"

static CObjectClass<CCSymbolTable>g_Class(OBJID_CCSYMBOLTABLE, NULL);

CCSymbolTable::CCSymbolTable (void) : ICCList(&g_Class),
		m_Symbols(FALSE, FALSE),
		m_pParent(NULL),
		m_bLocalFrame(false),
		m_pDefineHook(NULL)

//	SymbolTable constructor

	{
	}

void CCSymbolTable::AddByOffset (CCodeChain *pCC, int iOffset, ICCItem *pEntry)

//	AddByOffset
//
//	Adds an entry directly

	{
	CObject *pOldEntry;

	m_Symbols.SetValue(iOffset, pEntry->Reference(), &pOldEntry);

	//	Discard old entry

	((ICCItem *)pOldEntry)->Discard();
	}

void CCSymbolTable::AddEntry (ICCItem *pKey, ICCItem *pEntry, bool bForceLocalAdd)

//	AddEntry
//
//	Adds an entry to the symbol table and returns
//	True for success.

	{
	ALERROR error;
	ICCItem *pPrevEntry = NULL;

	//	Transform the value, if necessary

	ICCItem *pTransformed;
	if (m_pDefineHook)
		pTransformed = m_pDefineHook->Transform(pEntry);
	else
		pTransformed = pEntry->Reference();

	//	If this is the global symbol table (no parent) then we add the entry
	//	regardless of whether it already exists or not.

	if (m_pParent == NULL || bForceLocalAdd)
		{
		CObject *pOldEntry;

		if (m_Symbols.ReplaceEntry(pKey->GetStringValue(), pTransformed, true, &pOldEntry) != NOERROR)
			throw CException(ERR_MEMORY);

		//	If we have a previous entry, decrement its refcount since we're
		//	throwing it away

		pPrevEntry = (ICCItem *)pOldEntry;
		}

	//	Otherwise, if this is a local symbol table, try to replace the symbol
	//	but if the symbol is not found, let our parent handle it

	else
		{
		CObject *pOldEntry;

		error = m_Symbols.ReplaceEntry(pKey->GetStringValue(), pTransformed, false, &pOldEntry);
		if (error == ERR_NOTFOUND)
			{
			m_pParent->AddEntry(pKey, pTransformed);
			pTransformed->Discard();
			return;
			}
		else if (error != NOERROR)
			throw CException(ERR_MEMORY);

		//	Get the previous entry

		pPrevEntry = (ICCItem *)pOldEntry;
		}

	//	Delete old entry

	if (pPrevEntry)
		pPrevEntry->Discard();
	}

ICCItem *CCSymbolTable::Clone (CCodeChain *pCC)

//	Clone
//
//	Clone this item

	{
	int i;

	ICCItem *pNew = pCC->CreateSymbolTable();
	CCSymbolTable *pNewTable = dynamic_cast<CCSymbolTable *>(pNew);
	ASSERT(pNewTable);

	//	Add all the items to the table

	for (i = 0; i < m_Symbols.GetCount(); i++)
		{
		CString sKey = m_Symbols.GetKey(i);
		CObject *pValue = m_Symbols.GetValue(i);
		ICCItem *pItem = (ICCItem *)pValue;

		//	Add to the new table

		CObject *pOldEntry;
		if (pNewTable->m_Symbols.ReplaceEntry(sKey, pItem->Reference(), true, &pOldEntry) != NOERROR)
			throw CException(ERR_MEMORY);

		//	We better not have a previous entry (this can only happen if the existing symbol
		//	table has a duplicate entry).

		ASSERT(pOldEntry == NULL);
		}

	//	Clone the parent reference

	if (m_pParent)
		{
		//	Clone local frames, but not the global frame.

		if (m_pParent->IsLocalFrame())
			pNewTable->m_pParent = m_pParent->Clone(pCC);
		else
			pNewTable->m_pParent = m_pParent->Reference();
		}
	else
		pNewTable->m_pParent = NULL;

	pNewTable->m_bLocalFrame = m_bLocalFrame;
	pNewTable->m_pDefineHook = m_pDefineHook;

	return pNewTable;
	}

ICCItem *CCSymbolTable::CloneContainer (void)

//	CloneContainer
//
//	Clone this item

	{
	int i;

	ICCItem *pNew = CCodeChain::CreateSymbolTable();
	CCSymbolTable *pNewTable = dynamic_cast<CCSymbolTable *>(pNew);
	pNewTable->CloneItem(this);
	ASSERT(pNewTable);

	//	Add all the items to the table

	for (i = 0; i < m_Symbols.GetCount(); i++)
		{
		CString sKey = m_Symbols.GetKey(i);
		CObject *pValue = m_Symbols.GetValue(i);
		ICCItem *pItem = (ICCItem *)pValue;

		//	Add to the new table

		CObject *pOldEntry;
		if (pNewTable->m_Symbols.ReplaceEntry(sKey, pItem->CloneContainer(), true, &pOldEntry) != NOERROR)
			throw CException(ERR_MEMORY);

		//	We better not have a previous entry (this can only happen if the existing symbol
		//	table has a duplicate entry).

		ASSERT(pOldEntry == NULL);
		}

	return pNewTable;
	}

ICCItem *CCSymbolTable::CloneDeep (CCodeChain *pCC)

//	CloneDeep
//
//	Clone this item

	{
	int i;

	ICCItem *pNew = pCC->CreateSymbolTable();
	CCSymbolTable *pNewTable = dynamic_cast<CCSymbolTable *>(pNew);
	ASSERT(pNewTable);

	//	Add all the items to the table

	for (i = 0; i < m_Symbols.GetCount(); i++)
		{
		CString sKey = m_Symbols.GetKey(i);
		CObject *pValue = m_Symbols.GetValue(i);
		ICCItem *pItem = (ICCItem *)pValue;

		//	Add to the new table

		CObject *pOldEntry;
		if (pNewTable->m_Symbols.ReplaceEntry(sKey, pItem->CloneDeep(pCC), true, &pOldEntry) != NOERROR)
			throw CException(ERR_MEMORY);

		//	We better not have a previous entry (this can only happen if the existing symbol
		//	table has a duplicate entry).

		ASSERT(pOldEntry == NULL);
		}

	//	Clone the parent reference

	if (m_pParent)
		{
		//	Clone local frames, but not the global frame.

		if (m_pParent->IsLocalFrame())
			pNewTable->m_pParent = m_pParent->Clone(pCC);
		else
			pNewTable->m_pParent = m_pParent->Reference();
		}
	else
		pNewTable->m_pParent = NULL;

	pNewTable->m_bLocalFrame = m_bLocalFrame;
	pNewTable->m_pDefineHook = m_pDefineHook;

	return pNewTable;
	}

void CCSymbolTable::DeleteAll (CCodeChain *pCC, bool bLambdaOnly)

//	DeleteAll
//
//	Delete all entries

	{
	int i;

	for (i = 0; i < m_Symbols.GetCount(); i++)
		{
		CObject *pEntry = m_Symbols.GetValue(i);
		ICCItem *pItem = (ICCItem *)pEntry;
		if (bLambdaOnly && pItem->IsPrimitive())
			continue;

#ifdef DEBUG
		::kernelDebugLogString(m_Symbols.GetKey(i));
#endif
		pItem->Discard();
		m_Symbols.RemoveEntry(i);
		i--;
		}
	}

void CCSymbolTable::DeleteEntry (CCodeChain *pCC, ICCItem *pKey)

//	DeleteEntry
//
//	Deletes the entry

	{
	CObject *pOldEntry;

	if (m_Symbols.RemoveEntry(pKey->GetStringValue(), &pOldEntry) != NOERROR)
		{
		//	We get an error is the key was not found. This is OK.
		return;
		}

	//	If we have a previous entry, decrement its refcount since we're
	//	throwing it away

	ICCItem *pPrevEntry = (ICCItem *)pOldEntry;
	pPrevEntry->Discard();
	}

void CCSymbolTable::DestroyItem (void)

//	DestroyItem
//
//	Destroy this item

	{
	int i;

	//	Release our parent reference

	if (m_pParent)
		m_pParent->Discard();

	//	Release all the entries

	for (i = 0; i < m_Symbols.GetCount(); i++)
		{
		CObject *pValue = m_Symbols.GetValue(i);
		ICCItem *pItem = (ICCItem *)pValue;

		pItem->Discard();
		}

	//	Remove all symbols

	m_Symbols.RemoveAll();

	//	Destroy this item

	CCodeChain::DestroySymbolTable(this);
	}

int CCSymbolTable::FindOffset (CCodeChain *pCC, ICCItem *pKey)

//	FindOffset
//
//	Returns the offset of the given variable (or -1 if not found)
//	Callers must be careful about this usage--the offset may
//	change if any new entries are added to the table.

	{
	int iOffset;

	if (m_Symbols.LookupEx(pKey->GetStringValue(), &iOffset) != NOERROR)
		iOffset = -1;

	return iOffset;
	}

int CCSymbolTable::FindValue (ICCItem *pValue)

//	FindValue
//
//	Returns the entry in the symbol table

	{
	int i;

	for (i = 0; i < m_Symbols.GetCount(); i++)
		{
		if ((CObject *)pValue == m_Symbols.GetValue(i))
			return i;
		}

	return -1;
	}

ICCItem *CCSymbolTable::GetElement (int iIndex)

//	GetElement
//
//	Returns the nth value
//
//	NOTE: No need to discard the result, but be careful of use.

	{
	if (iIndex < 0 || iIndex >= m_Symbols.GetCount())
		return NULL;

	return dynamic_cast<ICCItem *>(m_Symbols.GetValue(iIndex));
	}

ICCItem *CCSymbolTable::GetElement (const CString &sKey)

//	GetElement
//
//	Returns the value for a key (or NULL).
//
//	NOTE: No need to discard the result, but be careful of use.

	{
	ALERROR error;

	CObject *pItem;
	if (error = m_Symbols.Lookup(sKey, &pItem))
		{
		if (error == ERR_NOTFOUND)
			{
			//	If we could not find it in this symbol table, look for
			//	the symbol in the parent

			if (m_pParent)
				return m_pParent->GetElement(sKey);
			else
				return NULL;
			}
		else
			return NULL;
		}

	return dynamic_cast<ICCItem *>(pItem);
	}

ICCItem *CCSymbolTable::GetElement (CCodeChain *pCC, int iIndex)

//	GetElement
//
//	Returns a key/value pair

	{
	if (iIndex < 0 || iIndex >= m_Symbols.GetCount())
		return pCC->CreateNil();

	CCLinkedList *pList = (CCLinkedList *)pCC->CreateLinkedList();
	
	ICCItem *pKey = pCC->CreateString(m_Symbols.GetKey(iIndex));
	pList->Append(pKey);
	pKey->Discard();

	pList->Append(GetElement(iIndex));

	//	Done

	return pList;
	}

bool CCSymbolTable::HasReferenceTo (ICCItem *pSrc)

//	HasReferenceTo
//
//	Returns TRUE if we have a reference to this item.

	{
	int i;

	if (this == pSrc)
		return true;

	for (i = 0; i < GetCount(); i++)
		{
		if (GetElement(i)->HasReferenceTo(pSrc))
			return true;
		}

	return false;
	}

ICCItem *CCSymbolTable::ListSymbols (CCodeChain *pCC)

//	ListSymbols
//
//	Returns a list of all the symbols in the table

	{
	//	If there are no symbols, return Nil

	if (m_Symbols.GetCount() == 0)
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

		for (i = 0; i < m_Symbols.GetCount(); i++)
			{
			ICCItem *pItem;
			CString sKey = m_Symbols.GetKey(i);

			//	Make an item for the symbol

			pItem = pCC->CreateString(sKey);

			//	Add the item to the list

			pList->Append(pItem);
			pItem->Discard();
			}

		return pList;
		}
	}

ICCItem *CCSymbolTable::Lookup (CCodeChain *pCC, ICCItem *pKey)

//	Lookup
//
//	Looks up the key and returns the association. If no
//	Association is found, returns Nil

	{
	return LookupEx(pCC, pKey, NULL);
	}

ICCItem *CCSymbolTable::LookupEx (CCodeChain *pCC, ICCItem *pKey, bool *retbFound)

//	LookupEx
//
//	Looks up the key and returns the association. If no
//	Association is found, we ask the parent. If none is found, returns an error

	{
	ALERROR error;
	CObject *pNew;
	ICCItem *pBinding;

	if (error = m_Symbols.Lookup(pKey->GetStringValue(), &pNew))
		{
		if (error == ERR_NOTFOUND)
			{
			//	If we could not find it in this symbol table, look for
			//	the symbol in the parent

			if (m_pParent)
				return m_pParent->LookupEx(pCC, pKey, retbFound);
			else
				{
				if (retbFound)
					*retbFound = false;

				return pCC->CreateErrorCode(CCRESULT_NOTFOUND);
				}
			}
		else
			throw CException(ERR_MEMORY);
		}

	pBinding = dynamic_cast<ICCItem *>(pNew);
	ASSERT(pBinding);

	if (retbFound)
		*retbFound = true;

	return pBinding->Reference();
	}

ICCItem *CCSymbolTable::LookupByOffset (CCodeChain *pCC, int iOffset)

//	LookupByOffset
//
//	Returns the value at the given offset

	{
	CObject *pNew = m_Symbols.GetValue(iOffset);
	ICCItem *pBinding = dynamic_cast<ICCItem *>(pNew);

	if (pBinding)
		return pBinding->Reference();
	else
		return pCC->CreateErrorCode(CCRESULT_NOTFOUND);
	}

CString CCSymbolTable::Print (DWORD dwFlags)

//	Print
//
//	Render as text

	{
	int i;

	CMemoryWriteStream Stream;
	if (Stream.Create() != NOERROR)
		return CONSTLIT("ERROR-OUT-OF-MEMORY");

	//	Open paren

	Stream.Write("{ ", 2);

	//	Write items

	for (i = 0; i < m_Symbols.GetCount(); i++)
		{
		CString sKey = CCString::Print(m_Symbols.GetKey(i));
		Stream.Write(sKey.GetASCIIZPointer(), sKey.GetLength());
		Stream.Write(":", 1);

		ICCItem *pValue = dynamic_cast<ICCItem *>(m_Symbols.GetValue(i));
		CString sValue = pValue->Print();
		Stream.Write(sValue.GetASCIIZPointer(), sValue.GetLength());

		Stream.Write(" ", 1);
		}

	//	Close paren

	Stream.Write("}", 1);
	return CString(Stream.GetPointer(), Stream.GetLength());
	}

void CCSymbolTable::Reset (void)

//	Reset
//
//	Reset the internal variables

	{
	m_Symbols.RemoveAll();
	m_pParent = NULL;
	m_bLocalFrame = false;
	m_pDefineHook = NULL;
	}

ICCItem *CCSymbolTable::SimpleLookup (CCodeChain *pCC, ICCItem *pKey, bool *retbFound, int *retiOffset)

//	SimpleLookup
//
//	Looks up the key and returns the association. If no
//	Association is found, returns an error

	{
	ALERROR error;
	CObject *pNew;
	ICCItem *pBinding;

	int iOffset;
	if (error = m_Symbols.LookupEx(pKey->GetStringValue(), &iOffset))
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

	pNew = m_Symbols.GetValue(iOffset);
	pBinding = dynamic_cast<ICCItem *>(pNew);
	ASSERT(pBinding);

	if (retbFound)
		*retbFound = true;

	if (retiOffset)
		*retiOffset = iOffset;

	return pBinding->Reference();
	}

