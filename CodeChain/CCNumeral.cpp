//	CCNumeral.cpp
//
//	Implements CCInteger class

#include <windows.h>
#include "Kernel.h"
#include "KernelObjID.h"
#include "CodeChain.h"
#include <math.h>

static CObjectClass<CCNumeral>g_Class(OBJID_CCNUMERAL, NULL);

CCNumeral::CCNumeral(void) : ICCAtom(&g_Class)
//	CCNumeral constructor
{

}

ICCItem *CCNumeral::Clone(CCodeChain *pCC)
//	Clone
//
//	Returns a new item with a single ref-count

{
	ICCItem *pResult;
	CCNumeral *pClone;

	if (this->IsInteger())
	{
		pResult = pCC->CreateInteger(this->GetIntegerValue());
		if (pResult->IsError())
			return pResult;

		pClone = (CCInteger *)pResult;
		pClone->CloneItem(this);
	}
	else if (this->IsDouble())
	{
		pResult = pCC->CreateDouble(this->GetDoubleValue());
		if (pResult->IsError())
			return pResult;

		pClone = (CCInteger *)pResult;
		pClone->CloneItem(this);
	};

	return pClone;
}

void CCNumeral::DestroyItem(CCodeChain *pCC)

//	DestroyItem
//
//	Destroys the item

{
	if (this->IsInteger())
		pCC->DestroyInteger(this);
	else
		pCC->DestroyDouble(this);
}

CString CCNumeral::Print(CCodeChain *pCC, DWORD dwFlags)

//	Print
//
//	Returns a text representation of this item

{
	//	If this is an error code, translate it

	if (IsError())
	{
		int iErrCode = this->GetIntegerValue();
		switch (iErrCode)
		{
		case CCRESULT_NOTFOUND:
			return strPatternSubst(LITERAL("[%d] Item not found."), iErrCode);

		case CCRESULT_CANCEL:
			return strPatternSubst(LITERAL("[%d] Operation canceled."), iErrCode);

		case CCRESULT_DISKERROR:
			return strPatternSubst(LITERAL("[%d] Disk error."), iErrCode);

		default:
			return strPatternSubst(LITERAL("[%d] Unknown error."), iErrCode);
		}
	}

	//	Otherwise, just print the integer value

	else
	{
		if (this->IsInteger())
			return strFromInt(this->GetIntegerValue());
		else
			return strFromDouble(this->GetDoubleValue());
	};
		
}

void CCNumeral::Reset(void)

//	Reset
//
//	Reset to initial conditions

{
	ASSERT(m_dwRefCount == 0);

	if (this->IsDouble())
		this->SetValue(0.0);
	else
		this->SetValue(0);
}

ICCItem *CCNumeral::StreamItem(CCodeChain *pCC, IWriteStream *pStream)

//	StreamItem
//
//	Stream the sub-class specific data

{
	ALERROR error;

	if (this->IsInteger())
	{
		int iValue = this->GetIntegerValue();
		if (error = pStream->Write((char *)&iValue, sizeof(iValue), NULL))
		{
			return pCC->CreateSystemError(error);
		};
	}
	else if (this->IsDouble())
	{ 
		double dValue = this->GetDoubleValue();
		if (error = pStream->Write((char *)&dValue, sizeof(dValue), NULL))
		{
			return pCC->CreateSystemError(error);
		};
	};

	return pCC->CreateTrue();
}

ICCItem *CCNumeral::UnstreamItem(CCodeChain *pCC, IReadStream *pStream)

//	UnstreamItem
//
//	Unstream the sub-class specific data

{
	ALERROR error;

	if (this->IsInteger())
	{ 
		int iValue = this->GetIntegerValue();
		if (error = pStream->Read((char *)&iValue, sizeof(iValue), NULL))
		{
			return pCC->CreateSystemError(error);
		};
	};

	if (this->IsDouble())
	{
		double dValue = this->GetDoubleValue();
		if (error = pStream->Read((char *)&dValue, sizeof(dValue), NULL))
		{
			return pCC->CreateSystemError(error);
		};
	};

	return pCC->CreateTrue();
}