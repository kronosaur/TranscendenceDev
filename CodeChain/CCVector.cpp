//	CCVector.cpp
//
//	Implements CCVector class

#include <windows.h>
#include "Kernel.h"
#include "KernelObjID.h"
#include "CodeChain.h"

//	================ HELPER FUNCTIONS ====================

ICCItem *GetRelevantArrayIndices(CCodeChain *pCC, TArray <int> vShape, CCLinkedList *pIndices, int iCurrentOrd = 0, int iCurrentMarker = 0, bool bIsChild = FALSE)

//	GetRelevantArrayIndices
//
//	Based on the index list, determines which array elements of the data array are being referred to.
//

{
	int i;
	ICCItem *pCurrentIndex;
	ICCItem *pResult;
	int iSumRemainingShape = 0;
	CCLinkedList *pRelevantIndices = &(CCLinkedList());

	//	optimization for 1D lists
	if (vShape.GetCount() == 1 && pIndices->GetCount() == 1 && bIsChild == FALSE)
	{
		return pIndices;
	};

	if (pIndices->GetCount() > vShape.GetCount())
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
			if (vShape.GetCount() > 0)
			{
				pIndices->Append(*pCC, pCC->CreateNil());
			}
			else
			{
				pRelevantIndices->AppendInteger(*pCC, iCurrentMarker);
				return pRelevantIndices;
			};
		};
	};

	pCurrentIndex = pIndices->GetElement(iCurrentOrd);

	for (i = iCurrentOrd + 1; i < vShape.GetCount(); i++)
	{
		iSumRemainingShape += vShape[i];
	};

	if (iSumRemainingShape == 0)
	{
		iSumRemainingShape = 1;
	};

	if (pCurrentIndex->IsNil())
	{
		for (i = 0; i < vShape[iCurrentOrd]; i++)
		{
			pResult = GetRelevantArrayIndices(pCC, vShape, pIndices, iCurrentOrd = iCurrentOrd + 1, iCurrentMarker = iCurrentMarker + i*iSumRemainingShape, bIsChild = TRUE);
			if (pResult->IsError())
			{
				return pResult;
			};

			pRelevantIndices->Append(*pCC, pResult);
		};

		return pRelevantIndices;
	};

	if (pCurrentIndex->IsList())
	{
		for (i = 0; i < pCurrentIndex->GetCount(); i++)
		{
			pResult = GetRelevantArrayIndices(pCC, vShape, pIndices, iCurrentOrd = iCurrentOrd + 1, iCurrentMarker = iCurrentMarker + i*iSumRemainingShape, bIsChild = TRUE);
			if (pResult->IsError())
			{
				return pResult;
			};

			pRelevantIndices->Append(*pCC, pResult);
		};

		return pRelevantIndices;
	};

	if (pCurrentIndex->IsInteger())
	{
		int iCurrentIndex = pCurrentIndex->GetIntegerValue();

		if (iCurrentIndex >= vShape[iCurrentOrd])
		{
			return pCC->CreateError(CONSTLIT("Index is out of bounds."));
		};

		pResult = GetRelevantArrayIndices(pCC, vShape, pIndices, iCurrentOrd = iCurrentOrd + 1, iCurrentMarker = iCurrentMarker + iCurrentIndex*iSumRemainingShape, bIsChild = TRUE);
		if (pResult->IsError())
		{
			return pResult;
		};

		pRelevantIndices->Append(*pCC, pResult);
		return pRelevantIndices;
	};

	return pCC->CreateError(CONSTLIT("All control structures passed in GetRelevantIndices."));
};

//	======================================================

static CObjectClass<CCVectorOld>g_ClassOLD(OBJID_CCVECTOROLD, NULL);

static CObjectClass<CCVector>g_Class(OBJID_CCVECTOR, NULL);

CCVector::CCVector(void) : ICCVector(&g_Class),
	m_pCC(NULL)
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
	m_pCC(pCC)
	//	CCVector constructor
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

	//	Create new vector

	pNewVector = new CCVector(pCC);
	if (pNewVector == NULL)
		return pCC->CreateMemoryError();

	//	Initialize
	pNewVector->SetShape(pCC, m_vShape);

	//	Copy the vector
	if (&m_vData)
	{
		//	TODO: Is this the correct way to copy array data?
		pNewVector->m_vData = this->m_vData;
	}

	if (&m_vShape)
	{
		pNewVector->m_vShape = this->m_vShape;
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
	//	No need to call delete on members of CCVector, because
	//	none were allocated using new

	//	We need to destroy the reference to the vector in pCC though
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

	if (iIndex < 0 || iIndex >= m_vData.GetCount())
		return m_pCC->CreateNil();

	return m_pCC->CreateDouble(m_vData[iIndex]);
}

ICCItem *CCVector::SetElement(int iIndex, double dValue)

//	SetElement
//
//	Set the nth element in the data array (0-based)
//

{
	ASSERT(m_pCC);

	if (iIndex < 0 || iIndex >= m_vData.GetCount())
		return m_pCC->CreateNil();

	m_vData[iIndex] = dValue;
	return m_pCC->CreateTrue();
}

ICCItem *CCVector::SetElementsByIndices(CCodeChain *pCC, CCLinkedList *pIndices, CCLinkedList *pData)

//	
//	SetElementsByIndices
//	
//	Sets elements using the given data, based on the given indices. Returns an ICCItem representing 
//	True if successful, and an Error otherwise. This ICCItem must be discarded by the caller.
//	

{
	int i;
	ICCItem *pResult = GetRelevantArrayIndices(pCC, this->GetShapeArray(), pIndices);

	if (pResult->IsError())
		return pCC->CreateError(CONSTLIT("Error understanding list of indices."));
	if (pResult->GetCount() != pData->GetCount())
		return pCC->CreateError(CONSTLIT("Number of indices in given index list do not match number of elements in given data."));

	ICCItem *pElement;
	for (i = 0; i < pResult->GetCount(); i++)
	{
		pElement = pData->GetElement(i);
		if (pElement->IsInteger() || pElement->IsDouble())
		{
			this->SetElement(pResult->GetElement(i)->GetIntegerValue(), pElement->GetDoubleValue());
		}
		else
		{
			return pCC->CreateError(CONSTLIT("A given data element is not a numeral."));
		};
			
	};

	return pCC->CreateTrue();
};


TArray <int> GetExtractedVectorShape(CCodeChain *pCC, TArray <int> vShape, CCLinkedList *pIndices)

//	GetExtractedVectorShape
//
//	Get the shape of a vector extracted from another using indices, based on 
//	the indices used for extraction.
//

{
	int i;
	TArray <int> vResultShape;
	int iLenIndices = pIndices->GetCount();

	for (i = 0; i < vShape.GetCount(); i++)
	{
		if (i >= iLenIndices)
		{
			vResultShape.Insert(vShape[i]);
			continue;
		};

		ICCItem *pCurrentItem = pIndices->GetElement(i);

		if (pCurrentItem->IsList())
		{
			vResultShape.Insert(pCurrentItem->GetCount());
			continue;
		};

		if (pCurrentItem->IsNil())
		{
			vResultShape.Insert(vShape[i]);
			continue;
		};

		if (pCurrentItem->IsInteger())
		{
			int *pInt = vResultShape.Insert();
			*pInt = pCurrentItem->GetIntegerValue();
			continue;
		};

		//	If we got here, then something is wrong -- deleting everything inside vResultShape
		vResultShape.DeleteAll();
		return vResultShape;
	};

	return vResultShape;
};
ICCItem *CCVector::IndexVector(CCodeChain *pCC, CCLinkedList *pIndices)

//	IndexVector
//
//	Returns a vector, which is produced by indexing this vector
//

{
	ICCItem *pItem;

	ICCItem *pIndexExtractionResult = GetRelevantArrayIndices(pCC, this->GetShapeArray(), pIndices);
	CCLinkedList *pRelevantIndices;
	
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
	TArray <int> vNewShape = GetExtractedVectorShape(pCC, this->GetShapeArray(), pIndices);
	if ((vNewShape.GetCount()) == 0 && (pIndices->GetCount() != 0))
		return pCC->CreateError(CONSTLIT("Error determining shape of extracted vector."));

	pItem = (dynamic_cast <CCLinkedList *> (pIndexExtractionResult))->GetFlattened(pCC, NULL);
	if (pItem->IsError())
	{
		pItem->Discard(pCC);
		return pCC->CreateError(("Unable to flatten extracted indices."));
	}
	pRelevantIndices = dynamic_cast<CCLinkedList *> (pItem);

	if (pRelevantIndices->GetCount() != 1)
	{
		TArray<double> vContentArray;
		for (i = 0; i < pRelevantIndices->GetCount(); i++)
		{
			vContentArray.Insert(this->m_vData[pRelevantIndices->GetElement(i)->GetIntegerValue()]);
		};

		pRelevantIndices->Discard(pCC);

		pItem = pCC->CreateVectorGivenContent(vNewShape, vContentArray);
		if (pItem->IsError())
			return pItem;
		pItem->Reset();

		return pItem;
	}
	else
	{
		pItem = GetElement(pRelevantIndices->Head(pCC)->GetIntegerValue());
		pRelevantIndices->Discard(pCC);
		return pItem;
	}
}

CString CCVectorOld::Print (CCodeChain *pCC, DWORD dwFlags)

//	Print
//
//	Print a user-visible message

// TODO: get this function to print out shape of the vector instead

	{
	return strPatternSubst(LITERAL("[vector with (%d) elements]"), m_iCount);
	}

CString CCVector::Print(CCodeChain *pCC, DWORD dwFlags)

//	Print
//
//	Print a user-visible message

// TODO: get this function to print out shape of the vector instead

{
	int i;
	CString sPrintedVector = LITERAL("(");
	int iNumElements = this->GetShapeCount();

	for (i = 0; i < iNumElements; i++)
	{
		sPrintedVector.Append(strPatternSubst(LITERAL("%d"), this->m_vShape[i]));
		if (i != (iNumElements-1))
			sPrintedVector.Append(LITERAL(", "));
	};
	
	sPrintedVector.Append(LITERAL(")"));

	return sPrintedVector;
}

void CCVectorOld::Reset (void)

//	Reset
//
//	Reset

	{
	}

void CCVector::Reset(void)

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

ICCItem *CCVector::SetDataArraySize(CCodeChain *pCC, int iNewSize)
//	SetDataArraySize
//
//	Sets the size of the data vector, preserving any previous data

	{
	int iExpansion;

	int iCurrentSize = m_vData.GetCount();

	if (iNewSize <= iCurrentSize)
		{
			return pCC->CreateNil();
		}
	else
		{
			iExpansion = iNewSize - iCurrentSize;
		};

	try
		{ 
			m_vData.InsertEmpty(iExpansion);
		}
	catch (...)
		{
			return pCC->CreateMemoryError();
		}
	
	return pCC->CreateTrue();
	}

ICCItem *CCVector::SetShapeArraySize(CCodeChain *pCC, int iNewSize)
//	SetShapeArraySize
//
//	Sets the size of the shape vector, preserving any previous data

{
	ALERROR allocationError;
	int iExpansion;

	int iCurrentSize = m_vShape.GetCount();

	if (iNewSize <= iCurrentSize)
	{
		return pCC->CreateNil();
	}
	else
	{
		iExpansion = iNewSize - iCurrentSize;
	};

	//  confirm that this is the right function to use
	m_vShape.InsertEmpty(iExpansion);
	if (allocationError = NOERROR)
	{
		return pCC->CreateTrue();
	}
	else
	{
		return pCC->CreateMemoryError();
	};
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

ICCItem *CCVector::StreamItem(CCodeChain *pCC, IWriteStream *pStream)

//	StreamItem
//
//	Streams the vector to a stream

{
	ALERROR error;
	int iDataCount = this->GetCount();
	int iShapeCount = this->GetShapeCount();

	//  Write out the shape count
	if (error = pStream->Write((char *)&iShapeCount, sizeof(iShapeCount), NULL))
		return pCC->CreateSystemError(error);

	//	Write out the data count
	if (error = pStream->Write((char *)&iDataCount, sizeof(iDataCount), NULL))
		return pCC->CreateSystemError(error);

	//	Write out the shape
	if (&m_vShape)
	{ 
		if (error = pStream->Write((char *)&(m_vShape[0]), (iDataCount * sizeof(int), NULL)))
			return pCC->CreateSystemError(error);
	}

	//	Write out the data
	if (&m_vData)
	{
		if (error = pStream->Write((char *)&(m_vData[0]), (iDataCount * sizeof(double), NULL)))
			return pCC->CreateSystemError(error);
	}

	//	Done

	return pCC->CreateTrue();
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

ICCItem *CCVector::UnstreamItem(CCodeChain *pCC, IReadStream *pStream)

//	UnstreamItem
//
//	Reads the vector from a stream

{
	ALERROR error;
	int iDataCount;
	int iShapeCount;
	ICCItem *pError;

	//	Read the shape count
	if (error = pStream->Read((char *)&iShapeCount, sizeof(iShapeCount), NULL))
		return pCC->CreateSystemError(error);

	//	Read the data count
	if (error = pStream->Read((char *)&iDataCount, sizeof(iDataCount), NULL))
		return pCC->CreateSystemError(error);

	//  Set size of the shape array
	pError = this->SetShapeArraySize(pCC, iShapeCount);
	if (pError->IsError())
		return pError;
	
	pError->Discard(pCC);

	//	Read the data
	if (&m_vShape)
	{
		if (error = pStream->Read((char *)&(m_vShape[0]), iDataCount * sizeof(int), NULL))
			return pCC->CreateSystemError(error);
	}

	//	Set size of the data array
	pError = this->SetDataArraySize(pCC, iDataCount);
	if (pError->IsError())
		return pError;

	pError->Discard(pCC);

	//	Read the data
	if (&m_vData)
	{
		if (error = pStream->Read((char *)&(m_vData[0]), iDataCount * sizeof(double), NULL))
			return pCC->CreateSystemError(error);
	}

	//	Done

	return pCC->CreateTrue();
}
