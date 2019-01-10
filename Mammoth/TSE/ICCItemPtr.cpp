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
	CCodeChain &CC = g_pUniverse->GetCC();

	switch (iType)
		{
		case ICCItem::Nil:
			m_pPtr = CC.CreateNil();
			break;

		case ICCItem::True:
			m_pPtr = CC.CreateTrue();
			break;

		case ICCItem::Integer:
			m_pPtr = CC.CreateInteger(0);
			break;

		case ICCItem::String:
			m_pPtr = CC.CreateString(NULL_STR);
			break;

		case ICCItem::List:
			m_pPtr = CC.CreateLinkedList();
			break;

		case ICCItem::Double:
			m_pPtr = CC.CreateDouble(0.0);
			break;

		case ICCItem::SymbolTable:
			m_pPtr = CC.CreateSymbolTable();
			break;

		default:
			ASSERT(false);
			m_pPtr = CC.CreateError(CONSTLIT("Invalid type"));
			break;
		}
	}

ICCItemPtr::ICCItemPtr (const CString &sValue)
	{
	m_pPtr = g_pUniverse->GetCC().CreateString(sValue);
	}

ICCItemPtr::ICCItemPtr (int iValue)
	{
	m_pPtr = g_pUniverse->GetCC().CreateInteger(iValue);
	}

ICCItemPtr::ICCItemPtr (DWORD dwValue)
	{
	m_pPtr = g_pUniverse->GetCC().CreateInteger((int)dwValue);
	}

ICCItemPtr::ICCItemPtr (double rValue)
	{
	m_pPtr = g_pUniverse->GetCC().CreateDouble(rValue);
	}

ICCItemPtr::ICCItemPtr (bool bValue)
	{
	m_pPtr = g_pUniverse->GetCC().CreateBool(bValue);
	}

ICCItemPtr::~ICCItemPtr (void)
	{
	if (m_pPtr)
		m_pPtr->Discard(&g_pUniverse->GetCC());
	}

ICCItemPtr &ICCItemPtr::operator= (const ICCItemPtr &Src)
	{
	if (m_pPtr)
		m_pPtr->Discard(&g_pUniverse->GetCC());

	if (Src.m_pPtr)
		m_pPtr = Src.m_pPtr->Reference();
	else
		m_pPtr = NULL;

	return *this;
	}

ICCItemPtr &ICCItemPtr::operator= (ICCItem *pSrc)
	{
	if (m_pPtr)
		m_pPtr->Discard(&g_pUniverse->GetCC());

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
		m_pPtr->Discard(&g_pUniverse->GetCC());

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

	ICCItem *pCode = g_pUniverse->GetCC().Link(sCode);
	if (pCode->IsError())
		{
		if (retsError)
			*retsError = pCode->GetStringValue();

		pCode->Discard(&g_pUniverse->GetCC());
		return false;
		}

	//	Done

	TakeHandoff(pCode);
	return true;
	}

void ICCItemPtr::TakeHandoff (ICCItem *pPtr)
	{
	if (m_pPtr)
		m_pPtr->Discard(&g_pUniverse->GetCC());

	m_pPtr = pPtr;
	}

void ICCItemPtr::TakeHandoff (ICCItemPtr &Src)
	{
	if (m_pPtr)
		m_pPtr->Discard(&g_pUniverse->GetCC());

	m_pPtr = Src.m_pPtr;
	Src.m_pPtr = NULL;
	}
