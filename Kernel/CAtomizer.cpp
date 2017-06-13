//	CAtomizer.cpp
//
//	CAtomizer class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "Kernel.h"
#include "KernelObjID.h"

CAtomizer::CAtomizer (void) :
		m_dwNextID(1)

//	CAtomizer constructor

	{
	//	Insert an entry because valid atoms start at 1.

	m_AtomToString.Insert(NULL_STR);
	}

DWORD CAtomizer::Atomize (const CString &sIdentifier)

//	Atomize
//
//	Convert from identifier to atom.

	{
	bool bNew;
	DWORD *pAtom = m_StringToAtom.SetAt(sIdentifier, &bNew);

	//	If not found, we need to add it.

	if (bNew)
		{
		*pAtom = m_dwNextID++;
		m_AtomToString.Insert(sIdentifier);
		}

	return *pAtom;
	}

CString CAtomizer::GetIdentifier (DWORD dwAtom) const

//	GetIdentifier
//
//	Convert from atom to identifier

	{
	ASSERT(dwAtom < (DWORD)m_AtomToString.GetCount());
	return m_AtomToString[dwAtom];
	}

int CAtomizer::GetMemoryUsage (void) const

//	GetMemoryUsage
//
//	Returns the amount of memory used.

	{
	int i;
	int iTotal = m_AtomToString.GetCount() * sizeof(DWORD);

	for (i = 0; i < m_AtomToString.GetCount(); i++)
		iTotal += m_AtomToString[i].GetMemoryUsage();

	return iTotal;
	}
