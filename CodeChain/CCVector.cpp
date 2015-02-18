//	CCVectorOld.cpp
//
//	Implements CCVectorOld class
//  Implements CCVectorOld class

#include <windows.h>
#include "Kernel.h"
#include "KernelObjID.h"
#include "CodeChain.h"

static CObjectClass<CCVectorOld>g_Class(OBJID_CCVectorOld, NULL);

static CObjectClass<CCVector>g_Class(OBJID_CCVector, NULL);

CCVector::CCVector(void) : ICCVector(&g_Class),
	m_pCC(NULL),
	m_pData(NULL),
	m_pShape(NULL),
	m_pStrides(NULL),
	m_iDtype(-1)
//	CCVectorOld constructor

{
}

CCVectorOld::CCVectorOld(void) : ICCList(&g_Class),
		m_pCC(NULL),
		m_iCount(0),
		m_pData(NULL)

//	CCVectorOld constructor

	{
	}

CCVectorOld::CCVectorOld (CCodeChain *pCC) : ICCList(&g_Class),
		m_pCC(pCC),
		m_iCount(0),
		m_pData(NULL)

//	CCVectorOld constructor

	{
	}

CCVector::CCVector(CCodeChain *pCC) : ICCVector(&g_Class),
	m_pCC(NULL),
	m_pData(NULL),
	m_pShape(NULL),
	m_pStrides(NULL),
	m_iDtype(-1)

//	CCVectorOld constructor

{
}

CCVectorOld::~CCVectorOld (void)

//	CCVectorOld destructor

	{
	if (m_pData)
		MemFree(m_pData);
	}

CCVector::~CCVector(void)

//	CCVectorOld destructor

{
	if (m_pData)
		MemFree(m_pData);
	if (m_pShape)
		MemFree(m_pShape);
	if (m_pStrides)
		MemFree(m_pStrides);
}

ICCItem *CCVectorOld::Clone (CCodeChain *pCC)

//	Clone
//
//	Clone this item

	{
		CCVectorOld *pNewVector;
	ICCItem *pError;

	//	Create new vector

	pNewVector = new CCVectorOld(pCC);
	if (pNewVector == NULL)
		return pCC->CreateMemoryError();

	//	Initialize

	pError = pNewVector->SetSize(pCC, m_iCount);
	if (pError->IsError())
		{
		delete pNewVector;
		return pError;
		}

	pError->Discard(pCC);

	//	Copy the vector

	if (m_pData)
		{
		int *pSource = m_pData;
		int *pDest = pNewVector->m_pData;
		int *pEnd = pDest + m_iCount;

		//	Copy

		while (pDest < pEnd)
			*pDest++ = *pSource++;
		}

	//	Done

	return pNewVector;
	}

ICCItem *CCVector::Clone(CCodeChain *pCC)

//	Clone
//
//	Clone this item

{
	CCVector *pNewVector;
	ICCItem *pError;

	//	Create new vector

	pNewVector = new CCVector(pCC);
	if (pNewVector == NULL)
		return pCC->CreateMemoryError();

	//	Initialize

	pError = pNewVector->SetShape(pCC, m_pShape);
	if (pError->IsError())
	{
		delete pNewVector;
		return pError;
	}

	pError->Discard(pCC);

	//	Copy the vector

	if (m_pData)
	{
		CIntArray *pSource = m_pData;
		CIntArray *pDest = pNewVector->m_pData;

		//Copy

		pDest->InsertRange(pSource, 0, m_pData->GetCount()-1, 0);
	}

	if (m_pShape)
	{
		CIntArray *pSource = m_pShape;
		CIntArray *pDest = pNewVector->m_pShape;

		// Copy

		pDest->InsertRange(pSource, 0, m_pShape->GetCount()-1, 0);
	}

	if (m_pStrides)
	{
		CIntArray *pSource = m_pStrides;
		CIntArray *pDest = pNewVector->m_pStrides;

		// Copy

		pDest->InsertRange(pSource, 0, m_pStrides->GetCount()-1, 0);
	}

	//	Done

	return pNewVector;
}

void CCVectorOld::DestroyItem (CCodeChain *pCC)

//	DestroyItem
//
//	Destroy this item

	{
	//	Free the vector

	if (m_pData)
		{
		MemFree(m_pData);
		m_pData = NULL;
		}

	//	Delete ourselves

	pCC->DestroyVector(this);
	}

void CCVector::DestroyItem(CCodeChain *pCC)

//	DestroyItem
//
//	Destroy this item

{
	//	Free the vector

	if (m_pData)
	{
		MemFree(m_pData);
		m_pData = NULL;
	}

	if (m_pShape)
	{
		MemFree(m_pShape);
		m_pShape = NULL;
	}

	if (m_pStrides)
	{
		MemFree(m_pStrides);
		m_pStrides = NULL;
	}

	//	Delete ourselves

	pCC->DestroyVector(this);
}

ICCItem *CCVectorOld::Enum (CEvalContext *pCtx, ICCItem *pCode)

//	Enum
//
//	

	{
	return pCtx->pCC->CreateNil();
	}

ICCItem *CCVector::Enum(CEvalContext *pCtx, ICCItem *pCode)

//	Enum
//
//	

{
	return pCtx->pCC->CreateNil();
}


int *CCVectorOld::GetArray (void)

//	GetArray
//
//	

	{
	return m_pData;
	}

CIntArray *CCVector::GetArray(void)

//	GetArray
//
//	

{
	return m_pData;
}

ICCItem *CCVectorOld::GetElement (int iIndex)

//	GetElement
//
//	Return the nth element in the vector (0-based)

	{
	ASSERT(m_pCC);

	if (iIndex < 0 || iIndex >= m_iCount)
		return m_pCC->CreateNil();

	return m_pCC->CreateInteger(m_pData[iIndex]);
	}

ICCItem *CCVector::GetElement(int iIndex)

//	GetElement
//
//	Return the nth element in the vector (0-based)

{
	ASSERT(m_pCC);

	if (iIndex < 0 || iIndex >= m_pData->GetCount())
		return m_pCC->CreateNil();

	return m_pCC->CreateInteger(m_pData->GetElement(iIndex));
}

ICCItem *CCVector::IndexVector(CCLinkedList *pIndices)

//	GetElement
//
//	Return the nth element in the vector (0-based)

{
	ICCItem *pIndex;
	int i;

	for (i = 0; i < pIndices->GetCount(); i++)
	{
		ICCItem *pResult;

		pIndex = pIndices->GetElement(i);

		// test to see if index is nil

		// test to see if index is an integer
			// if it is not, then we have an error


	}

}

CString CCVectorOld::Print (CCodeChain *pCC, DWORD dwFlags)

//	Print
//
//	Print a user-visible message

	{
	return strPatternSubst(LITERAL("[vector of %d elements]"), m_iCount);
	}

void CCVectorOld::Reset (void)

//	Reset
//
//	Reset

	{
	}

BOOL CCVectorOld::SetElement (int iIndex, int iElement)

//	SetElement
//
//	Sets the nth element in the vector (0-based)

	{
	if (iIndex < 0 || iIndex >= m_iCount)
		return FALSE;

	m_pData[iIndex] = iElement;
	return TRUE;
	}

void CCVector::SetDataType(int iDtype)
//  SetDataType
//
//  Sets the data type of the vector
	{
		m_iDtype = iDtype;
	};

ICCItem *CCVectorOld::SetSize (CCodeChain *pCC, int iNewSize)

//	SetSize
//
//	Sets the size of the vector, preserving any previous data

	{
	int *pNewData = NULL;

	//	Allocate a new array

	if (iNewSize > 0)
		{
		int iInitialized;
		int i;

		pNewData = (int *)MemAlloc(iNewSize * sizeof(DWORD));
		if (pNewData == NULL)
			return pCC->CreateMemoryError();

		//	Copy the old array

		if (m_pData)
			{
			int *pSource = m_pData;
			int *pDest = pNewData;
			int *pEnd;

			//	Figure out how much to copy

			iInitialized = min(m_iCount, iNewSize);
			pEnd = pDest + iInitialized;

			//	Copy

			while (pDest < pEnd)
				*pDest++ = *pSource++;
			}
		else
			iInitialized = 0;

		//	Initialize the rest of the array

		for (i = iInitialized; i < iNewSize; i++)
			pNewData[i] = 0;
		}

	//	Free the original

	if (m_pData)
		MemFree(m_pData);

	//	Done

	m_pData = pNewData;
	m_iCount = iNewSize;

	return pCC->CreateTrue();
	}

ICCItem *CCVector::SetArraySize(CCodeChain *pCC, int iNewSize)
//	SetArraySize
//
//	Sets the size of the vector, preserving any previous data

	{
	ALERROR allocationError;
	int iExpansion;

	int iCurrentSize = m_pData->GetCount();

	if (iNewSize <= iCurrentSize)
		{
			return pCC->CreateNil();
		}
	else
		{
			iExpansion = iNewSize - iCurrentSize;
		};

	allocationError = m_pData->ExpandArray(-1, iExpansion);;
	if (allocationError = NOERROR)
		{
			return pCC->CreateTrue();
		}
	else
		{
			return pCC->CreateMemoryError();
		};
	}

ICCItem *CCVector::SetArrayData(CCodeChain *pCC, CIntArray *pNewData)
//	SetArrayData
//
//	Set the vector data pointer to a new location.

	{

	m_pData = pNewData;

	}


ICCItem *CCVectorOld::StreamItem (CCodeChain *pCC, IWriteStream *pStream)

//	StreamItem
//
//	Streams the vector to a stream

	{
	ALERROR error;

	//	Write out the count

	if (error = pStream->Write((char *)&m_iCount, sizeof(m_iCount), NULL))
		return pCC->CreateSystemError(error);

	//	Write out the data

	if (m_pData)
		{
		if (error = pStream->Write((char *)m_pData, m_iCount * sizeof(DWORD), NULL))
			return pCC->CreateSystemError(error);
		}

	//	Done

	return pCC->CreateTrue();
	}

ICCItem *CCVectorOld::Tail (CCodeChain *pCC)

//	Tail
//
//	Returns the tail of the vector

	{
	return pCC->CreateNil();
	}

ICCItem *CCVectorOld::Tail(CCodeChain *pCC)

//	Tail
//
//	Returns the tail of the vector

{
	return pCC->CreateNil();
}

ICCItem *CCVector::Tail(CCodeChain *pCC)

//	Tail
//
//	Returns the tail of the vector

{
	return pCC->CreateNil();
}

ICCItem *CCVectorOld::UnstreamItem (CCodeChain *pCC, IReadStream *pStream)

//	UnstreamItem
//
//	Reads the vector from a stream

	{
	ALERROR error;
	int iCount;
	ICCItem *pError;

	//	Read the count

	if (error = pStream->Read((char *)&iCount, sizeof(iCount), NULL))
		return pCC->CreateSystemError(error);

	//	Resize the vector

	pError = SetSize(pCC, iCount);
	if (pError->IsError())
		return pError;

	pError->Discard(pCC);

	//	Read the data

	if (m_pData)
		{
		if (error = pStream->Read((char *)m_pData, iCount * sizeof(DWORD), NULL))
			return pCC->CreateSystemError(error);
		}

	//	Done

	m_iCount = iCount;

	return pCC->CreateTrue();
	}
