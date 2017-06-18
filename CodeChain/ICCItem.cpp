//	ICCItem.cpp
//
//	Implements ICCItem class

#include "PreComp.h"

#define ITEM_FLAG_QUOTED					0x00000001
#define ITEM_FLAG_ERROR						0x00000002
#define ITEM_FLAG_PARTOFLIST				0x00000004
#define ITEM_FLAG_NO_REF_COUNT				0x00000008
#define ITEM_FLAG_READ_ONLY					0x00000010

ICCItem::ICCItem (IObjectClass *pClass) : CObject(pClass)

//	ICCItem constructor

	{
	ResetItem();
	}

void ICCItem::AppendAt (CCodeChain &CC, const CString &sKey, ICCItem *pValue)

//	AppendAt
//
//	Set key-value pair.

	{
	ICCItem *pExisting = GetElement(sKey);

	//	If an entry does not exist, just add it.

	if (pExisting == NULL)
		SetAt(CC, sKey, pValue);

	//	If this is a list, append to it.

	else if (pExisting->GetValueType() == List)
		pExisting->Append(CC, pValue);

	//	Otherwise, convert into a list

	else
		{
		ICCItem *pList = CC.CreateLinkedList();
		pList->Append(CC, pExisting);
		pList->Append(CC, pValue);

		SetAt(CC, sKey, pList);
		pList->Discard(&CC);
		}
	}

void ICCItem::AppendInteger (CCodeChain &CC, int iValue)

//	AppendInteger
//
//	Inserts an element in a list

	{
	ICCItem *pItem = CC.CreateInteger(iValue);
	Append(CC, pItem);
	pItem->Discard(&CC);
	}

void ICCItem::AppendString (CCodeChain &CC, const CString &sValue)

//	AppendString
//
//	Inserts an element in a list

	{
	ICCItem *pItem = CC.CreateString(sValue);
	Append(CC, pItem);
	pItem->Discard(&CC);
	}

void ICCItem::CloneItem (ICCItem *pItem)

//	CloneItem
//
//	Makes this object take on the characteristics of the
//	given item (except for the ref count)

	{
	//	No need to set the refcount because it has already
	//	been set to 1

	m_bModified = false;
	m_bReadOnly = false;

	m_bQuoted = pItem->m_bQuoted;
	m_bError = pItem->m_bError;
	m_bNoRefCount = pItem->m_bNoRefCount;
	}

void ICCItem::Discard (CCodeChain *pCC)

//	Discard
//
//	Decrement reference count and delete if necessary

	{
	//	By checking for very-high reference counts we can
	//	also prevent discarding items that are already in the free list
	//	(because we use the refcount field to store the free list chain)
	ASSERT(m_bNoRefCount || (m_dwRefCount > 0 && m_dwRefCount < 0x00010000));

	m_dwRefCount--;
	if (m_dwRefCount == 0)
		{
		//	Let subclasses handle this

		DestroyItem(pCC);
		}
	}

ICCItem *ICCItem::Execute (CEvalContext *pCtx, ICCItem *pArgs)

//	Execute
//
//	Execute this function

	{
	return pCtx->pCC->CreateNil();
	}

bool ICCItem::GetBooleanAt (const CString &sKey)

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

ICCItem *ICCItem::GetElement (CCodeChain *pCC, int iIndex)

//	GetElement
//
//	Returns an element that must be discarded

	{
	ICCItem *pItem = GetElement(iIndex);
	if (pItem == NULL)
		return pCC->CreateNil();

	return pItem->Reference();
	}

double ICCItem::GetDoubleAt (const CString &sKey, double rDefault)

//	GetDoubleAt
//
//	Returns a double

	{
	ICCItem *pItem = GetElement(sKey);
	if (pItem == NULL || pItem->IsNil())
		return rDefault;

	return pItem->GetDoubleValue();
	}

int ICCItem::GetIntegerAt (const CString &sKey, int iDefault)

//	GetIntegerAt
//
//	Returns an integer (0 if not found)

	{
	ICCItem *pItem = GetElement(sKey);
	if (pItem == NULL || pItem->IsNil())
		return iDefault;

	return pItem->GetIntegerValue();
	}

CString ICCItem::GetStringAt (const CString &sKey, const CString &sDefault)

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

ICCItem *ICCItem::NotASymbolTable(CCodeChain *pCC)

//	NotASymbolTable
//
//	Returns an error when item is not a symbol table

	{
	return pCC->CreateError(LITERAL("Not a symbol table"));
	}

void ICCItem::ResetItem (void)

//	ResetItem
//
//	Returns item to original condition

	{
	m_dwRefCount = 0;
	m_bQuoted = false;
	m_bError = false;
	m_bModified = false;
	m_bNoRefCount = false;
	m_bReadOnly = false;
	}

void ICCItem::SetAt (CCodeChain &CC, const CString &sKey, ICCItem *pValue)

//	SetAt
//
//	Set key-value pair.

	{
	ICCItem *pKey = CC.CreateString(sKey);
	AddEntry(&CC, pKey, pValue);
	pKey->Discard(&CC);
	}

void ICCItem::SetIntegerAt (CCodeChain &CC, const CString &sKey, int iValue)

//	SetIntegerAt
//
//	Set key-value pair.

	{
	ICCItem *pKey = CC.CreateString(sKey);
	ICCItem *pValue = CC.CreateInteger(iValue);
	AddEntry(&CC, pKey, pValue);
	pKey->Discard(&CC);
	pValue->Discard(&CC);
	}

void ICCItem::SetStringAt (CCodeChain &CC, const CString &sKey, const CString &sValue)

//	SetStringAt
//
//	Set key-value pair.

	{
	ICCItem *pKey = CC.CreateString(sKey);
	ICCItem *pValue = CC.CreateString(sValue);
	AddEntry(&CC, pKey, pValue);
	pKey->Discard(&CC);
	pValue->Discard(&CC);
	}

ICCItem *ICCItem::Stream (CCodeChain *pCC, IWriteStream *pStream)

//	Stream
//
//	Stream the item

	{
	ALERROR error;
	DWORD dwFlags = 0;

	//	Convert the flags into a single DWORD
	//	We do not save PartOfList because streamed elements are
	//	never part of a list until they are loaded back

	if (m_bQuoted)
		dwFlags |= ITEM_FLAG_QUOTED;
	if (m_bError)
		dwFlags |= ITEM_FLAG_ERROR;
	if (m_bNoRefCount)
		dwFlags |= ITEM_FLAG_NO_REF_COUNT;
	if (m_bReadOnly)
		dwFlags |= ITEM_FLAG_READ_ONLY;

	//	Save the flags

	if (error = pStream->Write((char *)&dwFlags, sizeof(dwFlags), NULL))
		return pCC->CreateSystemError(error);

	//	Save the sub-class specific stuff

	return StreamItem(pCC, pStream);
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

ICCItem *ICCItem::Unstream (CCodeChain *pCC, IReadStream *pStream)

//	Unstream
//
//	Unstream the item

	{
	ALERROR error;
	DWORD dwFlags;

	//	Read the flags

	if (error = pStream->Read((char *)&dwFlags, sizeof(dwFlags), NULL))
		return pCC->CreateSystemError(error);

	//	Map flags

	m_bQuoted = ((dwFlags & ITEM_FLAG_QUOTED) ? true : false);
	m_bError = ((dwFlags & ITEM_FLAG_ERROR) ? true : false);
	m_bNoRefCount = ((dwFlags & ITEM_FLAG_NO_REF_COUNT) ? true : false);
	m_bReadOnly = ((dwFlags & ITEM_FLAG_READ_ONLY) ? true : false);

	//	Clear modified

	m_bModified = false;

	//	Load the item specific stuff

	return UnstreamItem(pCC, pStream);
	}

//	IItemTransform -------------------------------------------------------------

ICCItem *IItemTransform::Transform (CCodeChain &CC, ICCItem *pItem)
	{
	return pItem->Reference();
	}
