//	CCNil.cpp
//
//	Implements CCNil class

#include "PreComp.h"

static CObjectClass<CCNil>g_Class(OBJID_CCNIL, NULL);

CCNil::CCNil (void) : ICCAtom(&g_Class)

//	CCNil constructor

	{
	SetNoRefCount();
	}

ICCItem *CCNil::Clone (CCodeChain *pCC)

//	Clone
//
//	Returns a new item with a single ref-count

	{
	return Reference();
	}

void CCNil::DestroyItem (void)

//	DestroyItem
//
//	Destroys the item

	{
	}

