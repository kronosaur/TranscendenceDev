//	CCVector.cpp
//
//	Implements CCVector class

#include <windows.h>
#include "Kernel.h"
#include "KernelObjID.h"
#include "CodeChain.h"

//	================ HELPER FUNCTIONS ====================

ICCItem *GetRelevantArrayIndices(CCodeChain *pCC, TArray <int> vShape, ICCItem *pIndices, int iShapeCursor = 0, int iIndicesCursor = 0, int iArrayCursor = 0)

	//	GetRelevantArrayIndices
	//
	//	Based on the index list, determines which array elements of the data array are being referred to.
	//
	//	Throughout the documentation, frequent references are made to "hiearchy". Hierarchy refers to 
	//	the levels in the recursive structure of a vector, where every element could possibly be a 
	//	a vector itself.
	//	

	{
	int i;
	ICCItem *pError;

	if (pIndices->IsNil())
	{
		pError = pCC->CreateError(CONSTLIT("Empty index list."));
		return pError;
	}

	ICCItem *pRelevantIndices = pCC->CreateLinkedList();
	if (pRelevantIndices->IsError())
		return pCC->CreateMemoryError();

	//	If pIndices has less elements than the dimension at the current hierarchy
	//	then we assume that the remaining elements are Nil
	if (pIndices->GetCount() - iIndicesCursor < vShape.GetCount() && !(pIndices->GetElement(iIndicesCursor)->IsNil()))
	{
		for (i = pIndices->GetCount() - iIndicesCursor; i < vShape[iShapeCursor]; i++)
		{
			pIndices->Append(*pCC, pCC->CreateNil());
		}
	}


	//	Base case: we are at the lowest level in the hierarchy
	if (vShape.GetCount() - iShapeCursor == 1)
		{
		//	if an index is Nil, that is the same as saying "take every element on this hierarchy level"
		if (pIndices->GetElement(iIndicesCursor)->IsNil())
			{
			for (i = 0; i < vShape[iShapeCursor]; i++)
				{
				pRelevantIndices->AppendInteger(*pCC, iArrayCursor + i);
				};
			}
		else
			{
			for (i = 0; i < pIndices->GetCount(); i++)
				{
				ICCItem *pElement = pIndices->GetElement(i);
				if (!(pElement->IsInteger()))
					{
					pRelevantIndices->Discard(pCC);
					pError = pCC->CreateError(CONSTLIT("Something wasn't a list, a number, or nil inside the index list."));
					}

				pRelevantIndices->AppendInteger(*pCC, iArrayCursor + pElement->GetIntegerValue());
				};
			}
		return pRelevantIndices;
		}
	//	Non-base case: we are at any level in the hierarchy apart from the lowest one
	else if (vShape.GetCount() - iShapeCursor > 1)
		{
		//	If we are at the lowest level in the hierarchy, then we just have to increment by one array element to reach the neighbour element
		//	However, we are not at the lowest level in the hierarchy, so we have to increment by the product of the size of the hierachies below this level
		int iSkipCount = 1;
		for (i = iShapeCursor + 1; i < vShape.GetCount(); i++)
			{
				iSkipCount *= vShape[iShapeCursor];
			};

		//	if an index is Nil, that is the same as saying "take every element on this hierarchy level"
		if (pIndices->GetElement(iIndicesCursor)->IsNil())
			{
			for (i = 0; i < vShape[iShapeCursor]; i++)
				{
				pRelevantIndices->Append(*pCC, GetRelevantArrayIndices(pCC, vShape, pIndices, iShapeCursor + 1, iIndicesCursor + 1, iArrayCursor = iArrayCursor + i*iSkipCount));
				};
			}
		else
			{
			ICCItem *pElement = pIndices->GetElement(iIndicesCursor);
			if (!(pElement->IsInteger()))
				{
				if (pElement->GetValueType() == ICCItem::List)
					{
						for (i = 0; i < pElement->GetCount(); i++)
							{
							ICCItem *pSubElement = pElement->GetElement(i);
							if (pSubElement->IsInteger())
								pRelevantIndices->Append(*pCC, GetRelevantArrayIndices(pCC, vShape, pIndices, iShapeCursor + 1, iIndicesCursor + 1, iArrayCursor + pSubElement->GetIntegerValue()*iSkipCount));
							else
								pRelevantIndices->Discard(pCC);
								pCC->CreateError("Either too much hierarchy was provided in index list, or something wasn't a list, a number, or Nil.");
							};
					}
				else
					{
					pRelevantIndices->Discard(pCC);
					pError = pCC->CreateError(CONSTLIT("Something wasn't a list, a number, or Nil inside the index list."));
					}
				}
			else
				{
				pRelevantIndices->Append(*pCC, GetRelevantArrayIndices(pCC, vShape, pIndices, iShapeCursor + 1, iIndicesCursor + 1, iArrayCursor + pElement->GetIntegerValue()*iSkipCount));
				}
			}
		return pRelevantIndices;
		}
	else
		{
		pRelevantIndices->Discard(pCC);
		return pCC->CreateError(CONSTLIT("Vector is empty, thus cannot be set."));
		}
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
	ICCItem *pElement;

	bool bLastWasInteger = FALSE;
	for (i = 0; i < pIndices->GetCount(); i++)
		{
		pElement = pIndices->GetElement(i);
		if (pElement->IsNil())
			{
			vResultShape.Insert(vShape[i]);
			}
		else if (pElement->IsInteger())
			{
			bLastWasInteger = TRUE;
			}
		else if (pElement->IsList())
			{
			vResultShape.Insert(pElement->GetCount());
			}
		};

	return vResultShape;
};
ICCItem *CCVector::IndexVector(CCodeChain *pCC, ICCItem *pIndices)

//	IndexVector
//
//	Returns a vector, which is produced by indexing this vector
//

{
	ICCItem *pItem;

	ICCItem *pIndexExtractionResult = GetRelevantArrayIndices(pCC, this->GetShapeArray(), pIndices);
	CCLinkedList *pRelevantIndices;
	
	int i;
	int iIndex;

	if (pIndexExtractionResult->IsError())
	{
		return pIndexExtractionResult;
	};

	if (!(pIndexExtractionResult->IsList()))
	{
		return pCC->CreateError(CONSTLIT("Unable to determine list of extraction of indices."));
	};

	TArray <int> vNewShape = GetExtractedVectorShape(pCC, this->GetShapeArray(), dynamic_cast <CCLinkedList *> (pIndices));
	if ((vNewShape.GetCount() == 0) && (pIndices->GetCount() != 0))
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
			iIndex = pRelevantIndices->GetElement(i)->GetIntegerValue();
			vContentArray.Insert(this->m_vData[iIndex]);
		};

		pRelevantIndices->Discard(pCC);

		pItem = pCC->CreateVectorGivenContent(vNewShape, vContentArray);
		if (pItem->IsError())
			return pItem;

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
	double dData;
	CString sPrintedVector;

	if (GetShapeCount() == 1)
	{
		sPrintedVector.Append(LITERAL("("));
		for (i = 0; i < m_vShape[0]; i++)
		{
			dData = m_vData[i];
			sPrintedVector.Append(strPatternSubst(LITERAL("%s"), strFromDouble(dData)));

			if (i != m_vShape[0] - 1)
				sPrintedVector.Append(LITERAL(", "));

		};
		sPrintedVector.Append(LITERAL(")"));

		return sPrintedVector;
	}
	else if (GetShapeCount() > 1)
	{
		ICCItem *pIndices = pCC->CreateLinkedList();
		if (pIndices->IsError())
		{
			return LITERAL("Error printing vector.");
		};

		sPrintedVector.Append(LITERAL("( "));
		for (i = 0; i < m_vShape[0]; i++)
		{
			ICCItem *pIndices = pCC->CreateLinkedList();
			if (pIndices->IsError())
			{
				return LITERAL("Error printing vector: memory could not be allocated for temporary index list.");
			};
			pIndices->AppendInteger(*pCC, i);

			ICCItem *pSubVector = IndexVector(pCC, pIndices);
			if (pSubVector->IsError())
				{
				pIndices->Discard(pCC);
				return LITERAL("Error printing vector: subvectors could not be extracted.");
				}
			CString sPrintedSubVector = pSubVector->Print(pCC);
			sPrintedVector.Append(strPatternSubst(LITERAL("%s"), sPrintedSubVector));
			if (i != m_vShape[0] - 1)
				sPrintedVector.Append(LITERAL(", "));

			pSubVector->Discard(pCC);
			pIndices->Discard(pCC);
		};
		sPrintedVector.Append(LITERAL(" )"));

		return sPrintedVector;
	}
	else
	{
		return LITERAL("Empty vector.");
	}
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
