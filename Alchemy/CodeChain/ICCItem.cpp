//	ICCItem.cpp
//
//	Implements ICCItem class

#include "PreComp.h"

ICCItem::ICCItem (IObjectClass *pClass) : CObject(pClass)

//	ICCItem constructor

	{
	ResetItem();
	}

void ICCItem::AppendAt (const CString &sKey, ICCItem *pValue)

//	AppendAt
//
//	Set key-value pair.

	{
	ICCItem *pExisting = GetElement(sKey);

	//	If an entry does not exist, just add it.

	if (pExisting == NULL)
		SetAt(sKey, pValue);

	//	If this is a list, append to it.

	else if (pExisting->GetValueType() == List)
		pExisting->Append(pValue);

	//	Otherwise, convert into a list

	else
		{
		ICCItem *pList = CCodeChain::CreateLinkedList();
		pList->Append(pExisting);
		pList->Append(pValue);

		SetAt(sKey, pList);
		pList->Discard();
		}
	}

void ICCItem::AppendInteger (int iValue)

//	AppendInteger
//
//	Inserts an element in a list

	{
	ICCItem *pItem = CCodeChain::CreateInteger(iValue);
	Append(pItem);
	pItem->Discard();
	}

void ICCItem::AppendString (const CString &sValue)

//	AppendString
//
//	Inserts an element in a list

	{
	ICCItem *pItem = CCodeChain::CreateString(sValue);
	Append(pItem);
	pItem->Discard();
	}

void ICCItem::CloneItem (const ICCItem *pItem)

//	CloneItem
//
//	Makes this object take on the characteristics of the
//	given item (except for the ref count)

	{
	//	No need to set the refcount because it has already
	//	been set to 1

	m_bQuoted = pItem->m_bQuoted;
	m_bError = pItem->m_bError;
	m_bNoRefCount = pItem->m_bNoRefCount;
	}

int ICCItem::Compare (ICCItem *pFirst, ICCItem *pSecond)

//	Compare
//
//	Uses HelperCompareItems

	{
	return HelperCompareItems(pFirst, pSecond, HELPER_COMPARE_COERCE_FULL);
	}

void ICCItem::Discard (void)

//	Discard
//
//	Decrement reference count and delete if necessary

	{
	//	By checking for very-high reference counts we can
	//	also prevent discarding items that are already in the free list
	//	(because we use the refcount field to store the free list chain)
	ASSERT(m_dwRefCount > 0 && m_dwRefCount < 0x00010000);

	m_dwRefCount--;
	if (m_dwRefCount == 0)
		{
		//	Let subclasses handle this

		DestroyItem();
		}
	}

ICCItem *ICCItem::Execute (CEvalContext *pCtx, ICCItem *pArgs)

//	Execute
//
//	Execute this function

	{
	return pCtx->pCC->CreateNil();
	}

bool ICCItem::GetBooleanAt (const CString &sKey) const

//	GetBooleanAt
//
//	Returns a boolean value

	{
	if (IsNil())
		return false;
	else if (IsIdentifier())
		return strEquals(sKey, GetStringValue());
	else if (GetValueType() == List)
		{
		int i;
		for (i = 0; i < GetCount(); i++)
			{
			if (strEquals(sKey, GetElement(i)->GetStringValue()))
				return true;
			}
		return false;
		}
	else
		{
		ICCItem *pItem = GetElement(sKey);
		return (pItem && !pItem->IsNil());
		}
	}

ICCItem *ICCItem::GetElement (CCodeChain *pCC, int iIndex) const

//	GetElement
//
//	Returns an element that must be discarded

	{
	ICCItem *pItem = GetElement(iIndex);
	if (pItem == NULL)
		return pCC->CreateNil();

	return pItem->Reference();
	}

double ICCItem::GetDoubleAt (const CString &sKey, double rDefault) const

//	GetDoubleAt
//
//	Returns a double

	{
	ICCItem *pItem = GetElement(sKey);
	if (pItem == NULL || pItem->IsNil())
		return rDefault;

	return pItem->GetDoubleValue();
	}

int ICCItem::GetIntegerAt (const CString &sKey, int iDefault) const

//	GetIntegerAt
//
//	Returns an integer (0 if not found)

	{
	ICCItem *pItem = GetElement(sKey);
	if (pItem == NULL || pItem->IsNil())
		return iDefault;

	return pItem->GetIntegerValue();
	}

CString ICCItem::GetStringAt (const CString &sKey, const CString &sDefault) const

//	GetStringAt
//
//	Returns a string (NULL_STR if not found)

	{
	ICCItem *pItem = GetElement(sKey);
	if (pItem == NULL || pItem->IsNil())
		return sDefault;

	return pItem->GetStringValue();
	}

CString ICCItem::GetTypeOf (void)

//	GetTypeOf
//
//	Returns the type of basic types. Custom types should override this method.

	{
	if (IsError())
		return CONSTLIT("error");
	else if (IsNil())
		return CONSTLIT("nil");
	else if (IsInteger())
		return CONSTLIT("int32");
	else if (IsDouble())
		return CONSTLIT("real");
	else if (IsPrimitive())
		return CONSTLIT("primitive");
	else if (IsLambdaFunction())
		return CONSTLIT("function");
	else if (IsIdentifier())
		return CONSTLIT("string");
	else if (IsSymbolTable())
		return CONSTLIT("struct");
	else if (IsList())
		return CONSTLIT("list");
	else
		return CONSTLIT("true");
	}

bool ICCItem::IsLambdaExpression (void)

//	IsLambdaExpression
//
//	Returns TRUE if this is a lambda expression

	{
	return (IsList() && GetCount() >= 1 && GetElement(0)->IsLambdaSymbol());
	}

bool ICCItem::IsLambdaSymbol (void)

//	IsLambdaSymbol
//
//	Returns TRUE if this is the symbol lambda

	{
	return (IsIdentifier() && strCompareAbsolute(GetStringValue(), CONSTLIT("lambda")) == 0);
	}

ICCItem *ICCItem::NotASymbolTable (void)

//	NotASymbolTable
//
//	Returns an error when item is not a symbol table

	{
	return CCodeChain::CreateError(LITERAL("Not a symbol table"));
	}

void ICCItem::ResetItem (void)

//	ResetItem
//
//	Returns item to original condition

	{
	m_dwRefCount = 0;
	m_bQuoted = false;
	m_bError = false;
	m_bNoRefCount = false;
	}

void ICCItem::DeleteAt (const CString &sKey)

//	DeleteAt
//
//	Deletes a key-value pair.

	{
	ICCItemPtr pKey(sKey);
	DeleteEntry(pKey);
	}

void ICCItem::SetAt (const CString &sKey, ICCItem *pValue)

//	SetAt
//
//	Set key-value pair.

	{
	ICCItem *pKey = CCodeChain::CreateString(sKey);
	AddEntry(pKey, pValue);
	pKey->Discard();
	}

void ICCItem::SetBooleanAt (const CString &sKey, bool bValue)

//	SetBooleanAt
//
//	Set key-value pair.

	{
	ICCItem *pKey = CCodeChain::CreateString(sKey);
	ICCItem *pValue = (bValue ? CCodeChain::CreateTrue() : CCodeChain::CreateNil());
	AddEntry(pKey, pValue);
	pKey->Discard();
	pValue->Discard();
	}

void ICCItem::SetIntegerAt (const CString &sKey, int iValue)

//	SetIntegerAt
//
//	Set key-value pair.

	{
	ICCItem *pKey = CCodeChain::CreateString(sKey);
	ICCItem *pValue = CCodeChain::CreateInteger(iValue);
	AddEntry(pKey, pValue);
	pKey->Discard();
	pValue->Discard();
	}

void ICCItem::SetStringAt (const CString &sKey, const CString &sValue)

//	SetStringAt
//
//	Set key-value pair.

	{
	ICCItem *pKey = CCodeChain::CreateString(sKey);
	ICCItem *pValue = CCodeChain::CreateString(sValue);
	AddEntry(pKey, pValue);
	pKey->Discard();
	pValue->Discard();
	}

ICCItem *ICCAtom::Enum (CEvalContext *pCtx, ICCItem *pCode)

//	Enum
//
//	Enumerates over all elements in the list

	{
	return pCtx->pCC->CreateNil();
	}

ICCItem *ICCAtom::Tail (CCodeChain *pCC)

//	Tail
//
//	Returns the tail of an atom

	{
	return pCC->CreateNil();
	}

//	IItemTransform -------------------------------------------------------------

ICCItem *IItemTransform::Transform (ICCItem *pItem)
	{
	return pItem->Reference();
	}
