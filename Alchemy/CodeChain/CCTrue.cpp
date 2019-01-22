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
	ICCItem *pNew;
	CCTrue *pClone;

	pNew = pCC->CreateTrue();
	if (pNew->IsError())
		return pNew;

	pClone = dynamic_cast<CCTrue *>(pNew);
	pClone->CloneItem(this);

	return pClone;
	}

void CCTrue::DestroyItem (void)

//	DestroyItem
//
//	Destroys the item

	{
	}

