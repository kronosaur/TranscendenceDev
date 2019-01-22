//	CCDouble.cpp
//
//	Implements CCDouble class

#include "PreComp.h"

CCDouble::CCDouble (void) : 
		m_dValue(0.0)

//	CCDouble constructor

	{
	}

ICCItem *CCDouble::Clone (CCodeChain *pCC)

//	Clone
//
//	Returns a new item with a single ref-count

	{
	ICCItem *pResult;
	CCDouble *pClone;
	
	pResult = pCC->CreateDouble(this->GetDoubleValue());
	if (pResult->IsError())
		return pResult;

	pClone = (CCDouble *)pResult;
	pClone->CloneItem(this);

	return pClone;
	}

void CCDouble::DestroyItem (CCodeChain *pCC)

//	DestroyItem
//
//	Destroys the item

	{
	pCC->DestroyDouble(this);
	}

CString CCDouble::Print (CCodeChain *pCC, DWORD dwFlags)

//	Print
//
//	Returns a text representation of this item

	{
	//	If this is an error code, translate it

	if (IsError())
		{
		switch (GetIntegerValue())
			{
			case CCRESULT_NOTFOUND:
				return strPatternSubst(LITERAL("[%d] Item not found."), GetIntegerValue());

			case CCRESULT_CANCEL:
				return strPatternSubst(LITERAL("[%d] Operation canceled."), GetIntegerValue());

			case CCRESULT_DISKERROR:
				return strPatternSubst(LITERAL("[%d] Disk error."), GetIntegerValue());

			default:
				return strPatternSubst(LITERAL("[%d] Unknown error."), GetIntegerValue());
			}
		}

	//	Otherwise, just print the integer value

	else
		return strFromDouble(this->GetDoubleValue());
	}

void CCDouble::Reset (void)

//	Reset
//
//	Reset to initial conditions

	{
	ASSERT(m_dwRefCount == 0);
	this->SetValue(0.0);
	}

