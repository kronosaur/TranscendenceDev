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

void ICCItemPtr::Set (ICCItem *pPtr)
	{
	if (m_pPtr)
		m_pPtr->Discard(&g_pUniverse->GetCC());

	m_pPtr = pPtr;
	}

