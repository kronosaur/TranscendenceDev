//	CCInteger.cpp
//
//	Implements CCInteger class

#include <windows.h>
#include "Kernel.h"
#include "KernelObjID.h"
#include "CodeChain.h"
#include <math.h>

static CObjectClass<CCInteger>g_Class(OBJID_CCINTEGER, NULL);

CCNumeral *addNumerals(CCNumeral *num1, CCNumeral *num2)
{
	if (num1->IsInteger() && num2->IsInteger())
	{
		CCInteger *pResult = &(CCInteger());
		pResult->SetValue(num1->GetIntegerValue() + num2->GetIntegerValue());
		return pResult;
	};

	CCDouble *pResult = &(CCDouble()); 
	pResult->SetValue(num1->GetDoubleValue() + num2->GetDoubleValue());
	return pResult;
};

CCNumeral *multiplyNumerals(CCNumeral *num1, CCNumeral *num2)
{
	if (num1->IsInteger())
	{
		CCInteger *pResult = &(CCInteger());
		pResult->SetValue(num1->GetIntegerValue() * num2->GetIntegerValue());
		return pResult;
	};

	CCDouble *pResult = &(CCDouble());
	pResult->SetValue(num1->GetDoubleValue() * num2->GetDoubleValue());
	return pResult;
};

CCNumeral *divideNumerals(CCNumeral *num, CCNumeral *den)
{
	if (num->IsInteger() && den->IsInteger())
	{
		CCInteger *pResult = &(CCInteger());
		pResult->SetValue(num->GetIntegerValue() / den->GetIntegerValue());
		return pResult;
	};

	CCDouble *pResult = &(CCDouble());
	pResult->SetValue(num->GetDoubleValue() / den->GetDoubleValue());
	return pResult;
};

CCNumeral *moduloNumerals(CCNumeral *num, CCNumeral *den)
{
	if (num->IsInteger() && den->IsInteger())
	{
		CCInteger *pResult = &(CCInteger());
		pResult->SetValue(num->GetIntegerValue() % den->GetIntegerValue());
		return pResult;
	};

	CCDouble *pResult = &(CCDouble());
	pResult->SetValue(fmod(num->GetDoubleValue(), den->GetDoubleValue()));
	return pResult;
};


CCInteger::CCInteger(void) : ICCInteger(&g_Class),
m_iValue(0)

//	CCInteger constructor

	{
	}

ICCItem *CCInteger::Clone (CCodeChain *pCC)

//	Clone
//
//	Returns a new item with a single ref-count

	{
	ICCItem *pResult;
	CCInteger *pClone;
	
	pResult = pCC->CreateInteger(m_iValue);
	if (pResult->IsError())
		return pResult;

	pClone = (CCInteger *)pResult;
	pClone->CloneItem(this);

	return pClone;
	}

ICCItem *CCIntegerOld::Clone(CCodeChain *pCC)

//	Clone
//
//	Returns a new item with a single ref-count

{
	ICCItem *pResult;
	CCInteger *pClone;

	pResult = pCC->CreateInteger(m_iValue);
	if (pResult->IsError())
		return pResult;

	pClone = (CCInteger *)pResult;
	pClone->CloneItem(this);

	return pClone;
}

void CCInteger::DestroyItem (CCodeChain *pCC)

//	DestroyItem
//
//	Destroys the item

	{
	pCC->DestroyInteger(this);
	}

CString CCInteger::Print (CCodeChain *pCC, DWORD dwFlags)

//	Print
//
//	Returns a text representation of this item

	{
	//	If this is an error code, translate it

	if (IsError())
		{
		switch (m_iValue)
			{
			case CCRESULT_NOTFOUND:
				return strPatternSubst(LITERAL("[%d] Item not found."), m_iValue);

			case CCRESULT_CANCEL:
				return strPatternSubst(LITERAL("[%d] Operation canceled."), m_iValue);

			case CCRESULT_DISKERROR:
				return strPatternSubst(LITERAL("[%d] Disk error."), m_iValue);

			default:
				return strPatternSubst(LITERAL("[%d] Unknown error."), m_iValue);
			}
		}

	//	Otherwise, just print the integer value

	else
		return strFromInt(m_iValue);
	}

void CCInteger::Reset (void)

//	Reset
//
//	Reset to initial conditions

	{
	ASSERT(m_dwRefCount == 0);
	m_iValue = 0;
	}

ICCItem *CCInteger::StreamItem (CCodeChain *pCC, IWriteStream *pStream)

//	StreamItem
//
//	Stream the sub-class specific data

	{
	ALERROR error;

	if (error = pStream->Write((char *)&m_iValue, sizeof(m_iValue), NULL))
		return pCC->CreateSystemError(error);

	return pCC->CreateTrue();
	}

ICCItem *CCInteger::UnstreamItem (CCodeChain *pCC, IReadStream *pStream)

//	UnstreamItem
//
//	Unstream the sub-class specific data

	{
	ALERROR error;

	if (error = pStream->Read((char *)&m_iValue, sizeof(m_iValue), NULL))
		return pCC->CreateSystemError(error);

	return pCC->CreateTrue();
	}
