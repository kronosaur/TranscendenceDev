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
	if (m_pPtr)
		m_pPtr->Discard();

	if (Src.m_pPtr)
		m_pPtr = Src.m_pPtr->Reference();
	else
		m_pPtr = NULL;

	return *this;
	}

ICCItemPtr &ICCItemPtr::operator= (ICCItem *pSrc)
	{
	if (m_pPtr)
		m_pPtr->Discard();

	if (pSrc)
		m_pPtr = pSrc->Reference();
	else
		m_pPtr = NULL;

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

	ICCItem *pCode = CCodeChain::Link(sCode);
	if (pCode->IsError())
		{
		if (retsError)
			*retsError = pCode->GetStringValue();

		pCode->Discard();
		return false;
		}

	//	Done

	TakeHandoff(pCode);
	return true;
	}

void ICCItemPtr::TakeHandoff (ICCItem *pPtr)
	{
	if (m_pPtr)
		m_pPtr->Discard();

	m_pPtr = pPtr;
	}

void ICCItemPtr::TakeHandoff (ICCItemPtr &Src)
	{
	if (m_pPtr)
		m_pPtr->Discard();

	m_pPtr = Src.m_pPtr;
	Src.m_pPtr = NULL;
	}
