//	ICCItemPtr.cpp
//
//	ICCItemPtr class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

ICCItemPtr::ICCItemPtr (const ICCItemPtr &Src)
	{
	if (Src.m_pPtr)
		m_pPtr = Src.m_pPtr->Reference();
	else
		m_pPtr = NULL;
	}

ICCItemPtr::ICCItemPtr (ICCItem::ValueTypes iType)
	{
	switch (iType)
		{
		case ICCItem::Nil:
			m_pPtr = CCodeChain::CreateNil();
			break;

		case ICCItem::True:
			m_pPtr = CCodeChain::CreateTrue();
			break;

		case ICCItem::Integer:
			m_pPtr = CCodeChain::CreateInteger(0);
			break;

		case ICCItem::String:
			m_pPtr = CCodeChain::CreateString(NULL_STR);
			break;

		case ICCItem::List:
			m_pPtr = CCodeChain::CreateLinkedList();
			break;

		case ICCItem::Double:
			m_pPtr = CCodeChain::CreateDouble(0.0);
			break;

		case ICCItem::SymbolTable:
			m_pPtr = CCodeChain::CreateSymbolTable();
			break;

		default:
			ASSERT(false);
			m_pPtr = CCodeChain::CreateError(CONSTLIT("Invalid type"));
			break;
		}
	}

ICCItemPtr::ICCItemPtr (const CString &sValue)
	{
	m_pPtr = CCodeChain::CreateString(sValue);
	}

ICCItemPtr::ICCItemPtr (int iValue)
	{
	m_pPtr = CCodeChain::CreateInteger(iValue);
	}

ICCItemPtr::ICCItemPtr (DWORD dwValue)
	{
	m_pPtr = CCodeChain::CreateInteger((int)dwValue);
	}

ICCItemPtr::ICCItemPtr (double rValue)
	{
	m_pPtr = CCodeChain::CreateDouble(rValue);
	}

ICCItemPtr::ICCItemPtr (bool bValue)
	{
	m_pPtr = CCodeChain::CreateBool(bValue);
	}

ICCItemPtr::~ICCItemPtr (void)
	{
	if (m_pPtr)
		m_pPtr->Discard();
	}

ICCItemPtr &ICCItemPtr::operator= (const ICCItemPtr &Src)
	{
	ICCItem *pOld = m_pPtr;

	if (Src.m_pPtr)
		m_pPtr = Src.m_pPtr->Reference();
	else
		m_pPtr = NULL;

	//	Discard at the end, in case Src.m_pPtr == m_pPtr.

	if (pOld)
		pOld->Discard();

	return *this;
	}

ICCItemPtr &ICCItemPtr::operator= (ICCItem *pSrc)
	{
	ICCItem *pOld = m_pPtr;

	if (pSrc)
		m_pPtr = pSrc->Reference();
	else
		m_pPtr = NULL;

	//	Discard at the end, in case Src.m_pPtr == m_pPtr.

	if (pOld)
		pOld->Discard();

	return *this;
	}

ICCItemPtr &ICCItemPtr::operator= (const ICCItem &Value)
	{
	ICCItem *pOld = m_pPtr;

	m_pPtr = Value.Reference();

	//	Discard at the end, in case Value.m_pPtr == m_pPtr.

	if (pOld)
		pOld->Discard();

	return *this;
	}

void ICCItemPtr::Delete (void)

//	Delete
//
//	Delete

	{
	if (m_pPtr)
		m_pPtr->Discard();

	m_pPtr = NULL;
	}

ICCItemPtr ICCItemPtr::DoubleOrInt (double rValue)

//	DoubleOrInt
//
//	Returns either a double or an integer.

	{
	if ((double)(int)rValue == rValue)
		return ICCItemPtr((int)rValue);
	else
		return ICCItemPtr(rValue);
	}

ICCItemPtr ICCItemPtr::Error (const CString &sError, const ICCItem *pData)

//	Error
//
//	Creates an error

	{
	CString sArg;
	CString sErrorLine;

	//	Convert the argument to a string

	if (pData)
		{
		sArg = pData->Print();
		sErrorLine = strPatternSubst(CONSTLIT("%s [%s]"), sError, sArg);
		}
	else
		sErrorLine = sError;

	//	Create the error

	ICCItemPtr pError(sErrorLine);
	pError->SetError();
	return pError;
	}

bool ICCItemPtr::Load (const CString &sCode, CString *retsError)

//	Load
//
//	Load from string.

	{
	//	Null case

	if (sCode.IsBlank())
		{
		Delete();
		return true;
		}

	//	Compile the code

	ICCItemPtr pCode = CCodeChain::LinkCode(sCode);
	if (pCode->IsError())
		{
		if (retsError)
			*retsError = pCode->GetStringValue();

		return false;
		}

	//	Done

	*this = pCode;
	return true;
	}

