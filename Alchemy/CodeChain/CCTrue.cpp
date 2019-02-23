//	CCTrue.cpp
//
//	Implements CCTrue class

#include "PreComp.h"

static CObjectClass<CCTrue>g_Class(OBJID_CCTRUE, NULL);

CCTrue::CCTrue (void) : ICCAtom(&g_Class)

//	CCTrue constructor

	{
	SetNoRefCount();
	}

ICCItem *CCTrue::Clone (CCodeChain *pCC)

//	Clone
//
//	Returns a new item with a single ref-count

	{
	return this;
	}

void CCTrue::DestroyItem (void)

//	DestroyItem
//
//	Destroys the item

	{
	}

