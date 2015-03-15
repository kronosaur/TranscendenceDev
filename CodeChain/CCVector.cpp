//	CCVectorOld.cpp
//
//	Implements CCVectorOld class
//  Implements CCVectorOld class

#include <windows.h>
#include "Kernel.h"
#include "KernelObjID.h"
#include "CodeChain.h"

static CObjectClass<CCVectorOld>g_ClassOLD(OBJID_CCVECTOROLD, NULL);

static CObjectClass<CCVector>g_Class(OBJID_CCVECTOR, NULL);

CCVector::CCVector(void) : ICCVector(&g_Class),
	m_pCC(NULL),
	m_pData(NULL),
	m_pShape(NULL),
	m_iDtype(-1)
//	CCVector constructor

{
}

CCVectorOld::CCVectorOld(void) : ICCList(&g_ClassOLD),
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
		TArray<double> *pSource = m_pData;
		TArray<double> *pDest = pNewVector->m_pData;

		//Copy

		pDest->Insert(pSource);
	}

	if (m_pShape)
	{
		TArray<int> *pSource = m_pShape;
		TArray<int> *pDest = pNewVector->m_pShape;

		// Copy

		pDest->Insert(pSource);
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
	double *pElement;

	if (iIndex < 0 || iIndex >= m_pData->GetCount())
		return m_pCC->CreateNil();

	pElement = &m_pData->GetAt(iIndex);
	if (this->GetDataType() == 0)
		return m_pCC->CreateInteger(int(*pElement));
	else
		return m_pCC->CreateDouble(*pElement);
}

ICCItem *CCVector::SetElement(int iIndex, CCNumeral *pNumeral)

//	SetElement
//
//	Set the nth element in the data array (0-based)
//

{
	ASSERT(m_pCC);

	if (iIndex < 0 || iIndex >= m_pData->GetCount())
		return m_pCC->CreateNil();

	*(this->m_pData->InsertAt(iIndex)) = (pNumeral->GetDoubleValue());
	return m_pCC->CreateTrue();
}

ICCItem *CCVector::SetElementsByIndex(CCLinkedList *pIndices, CCLinkedList *pData)
{};

ICCItem *GetExtractionIndices(CCodeChain *pCC, TArray <int> *pShape, CCLinkedList *pIndices, int iCurrentOrd = 0, int iCurrentMarker = 0, bool bIsChild = FALSE)

//	GetExtractionIndices
//
//	Based on the index list, determines which indices from the vector being indexed will form the new vector.
//

{
	int i;
	ICCItem *pCurrentIndex;
	ICCItem *pResult;
	int iSumRemainingShape = 0;
	CCLinkedList *pExtractionIndices = &(CCLinkedList());

	//	optimization for 1D lists
	if (pShape->GetCount() == 1 && pIndices->GetCount() == 1 && bIsChild == FALSE)
	{
		return pIndices;
	};

	if (pIndices->GetCount() > pShape->GetCount())
	{
		return pCC->CreateError(CONSTLIT("Too many indices."));
	};

	if (iCurrentOrd > pIndices->GetCount() - 1)
	{
		if (bIsChild == FALSE)
		{
			return pCC->CreateError(CONSTLIT("Index array was empty."));
		}
		else
		{
			if (pShape->GetCount() > 0)
			{
				pIndices->Append(pCC, pCC->CreateNil());
			}
			else
			{
				pExtractionIndices->AppendIntegerValue(pCC, iCurrentMarker);
				return pExtractionIndices;
			};
		};
	};

	pCurrentIndex = pIndices->GetElement(iCurrentOrd);

	for (i = iCurrentOrd + 1; i < pShape->GetCount(); i++)
	{
		iSumRemainingShape += pShape->GetAt(i);
	};

	if (iSumRemainingShape == 0)
	{
		iSumRemainingShape = 1;
	};

	if (pCurrentIndex->IsNil())
	{
		for (i = 0; i < pShape->GetAt(iCurrentOrd); i++)
		{
			pResult = GetExtractionIndices(pCC, pShape, pIndices, iCurrentOrd = iCurrentOrd + 1, iCurrentMarker = iCurrentMarker + i*iSumRemainingShape, bIsChild = TRUE);
			if (pResult->IsError())
			{
				return pResult;
			};

			pExtractionIndices->Append(pCC, pResult);
		};

		return pExtractionIndices;
	};

	if (pCurrentIndex->IsList())
	{
		for (i = 0; i < pCurrentIndex->GetCount(); i++)
		{
			pResult = GetExtractionIndices(pCC, pShape, pIndices, iCurrentOrd = iCurrentOrd + 1, iCurrentMarker = iCurrentMarker + i*iSumRemainingShape, bIsChild = TRUE);
			if (pResult->IsError())
			{
				return pResult;
			};

			pExtractionIndices->Append(pCC, pResult);
		};

		return pExtractionIndices;
	};

	if (pCurrentIndex->IsInteger())
	{
		int iCurrentIndex = pCurrentIndex->GetIntegerValue();

		if (iCurrentIndex >= pShape->GetAt(iCurrentOrd))
		{
			return pCC->CreateError(CONSTLIT("Index is out of bounds."));
		};

		pResult = GetExtractionIndices(pCC, pShape, pIndices, iCurrentOrd = iCurrentOrd + 1, iCurrentMarker = iCurrentMarker + iCurrentIndex*iSumRemainingShape, bIsChild = TRUE);
		if (pResult->IsError())
		{
			return pResult;
		};

		pExtractionIndices->Append(pCC, pResult);
		return pExtractionIndices;
	};

};

int GetExtractedVectorShape(CCodeChain *pCC, TArray <int> *pShape, CCLinkedList *pIndices, TArray <int> *pResultShape)

//	GetExtractedVectorShape
//
//	Get the shape of a vector extracted from another using indices, based on 
//	the indices used for extraction.
//

{
	int i;
	int iLenIndices = pIndices->GetCount();

	for (i = 0; i < pShape->GetCount(); i++)
	{
		if (i >= iLenIndices)
		{
			pResultShape->Insert(pShape->GetAt(i));
			continue;
		};

		ICCItem *pCurrentItem = pIndices->GetElement(i);

		if (pCurrentItem->IsList())
		{
			pResultShape->Insert(pCurrentItem->GetCount());
			continue;
		};

		if (pCurrentItem->IsNil())
		{
			pResultShape->Insert(pShape->GetAt(i));
			continue;
		};

		if (pCurrentItem->IsInteger())
		{
			int *pInt = pResultShape->Insert();
			*pInt = pCurrentItem->GetIntegerValue();
			continue;
		};

		//	If we got here, then something is wrong
		return 0;
	};

	return 1;
};

ICCItem *CCVector::IndexVector(CCodeChain *pCC, CCLinkedList *pIndices)

//	IndexVector
//
//	Returns a vector, which is produced by indexing this vector
//

{
	CCVector *pResultVector = &(CCVector());
	pResultVector->SetDataType(m_iDtype);
	ICCItem *pIndexExtractionResult = GetExtractionIndices(pCC, this->GetShapeArray(), pIndices);
	CCLinkedList *pExtractionIndices;
	int i;

	if (pIndexExtractionResult->IsError())
	{
		return pIndexExtractionResult;
	};

	if (!(pIndexExtractionResult->IsList()))
	{
		return pCC->CreateError(CONSTLIT("Unable to determine list of extraction of indices."));
	};

	// we pass in the pointer to the shape array of the result vector to GetExtractedVectorShape
	int iResult = GetExtractedVectorShape(pCC, this->GetShapeArray(), pIndices, pResultVector->GetShapeArray());
	if (iResult == 0)
		return pCC->CreateError(CONSTLIT("Error determining shape of extracted vector."));

	pExtractionIndices = (dynamic_cast <CCLinkedList *> (pIndexExtractionResult))->GetFlattened(pCC, NULL);
	TArray<double> *pData = pResultVector->GetDataArray();
	for (i = 0; i < pExtractionIndices->GetCount(); i++)
	{
		double *pNumber = pData->Insert();
		double *pElement = &(this->m_pData->GetAt(pExtractionIndices->GetElement(i)->GetIntegerValue()));
		pNumber = pElement;
	};

	// set two remaining data objects
	pResultVector->m_pCC = this->m_pCC;
	pResultVector->m_iDtype = this->m_iDtype;

	return pResultVector;
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

	//  confirm that this is the right function to use
	m_pData->InsertEmpty(iExpansion);
	if (allocationError = NOERROR)
		{
			return pCC->CreateTrue();
		}
	else
		{
			return pCC->CreateMemoryError();
		};
	}

ICCItem *CCVector::SetArrayData(CCodeChain *pCC, TArray<double> *pNewData)
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
