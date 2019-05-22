//	CCVector.cpp
//
//	Implements CCVector class

#include "PreComp.h"

//	================ HELPER FUNCTIONS ====================

ICCItem *GetRelevantArrayIndices(CCodeChain *pCC, TArray <int> vShape, ICCItem *pIndices)

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
	int j;
	int k;
	int iDifference;
	int iSkipCount;
	ICCItem *pElement;
	ICCItem *pError;
	ICCItem *pArrayIndices;
	ICCItem *pNewArrayIndices;


	if (pIndices->IsNil())
		{
		pError = pCC->CreateError(CONSTLIT("Empty index list."));
		return pError;
		}
	else if (pIndices->GetCount() > vShape.GetCount())
		{
		pError = pCC->CreateError(CONSTLIT("Too many indices provided."));
		return pError;
		}

	if (pIndices->GetCount() < vShape.GetCount())
		{
		iDifference = vShape.GetCount() - pIndices->GetCount();
		for (i = 0; i < iDifference; i++)
			{
			pIndices->Append(pCC->CreateNil());
			};
		}

	pArrayIndices = pCC->CreateLinkedList();

	for (i = 0; i < pIndices->GetCount(); i++)
		{
		pElement = pIndices->GetElement(i);

		iSkipCount = 1;
		for (j = i + 1; j < vShape.GetCount(); j++)
			{
			iSkipCount *= vShape[j];
			}

		if (i == 0)
			{
			if (pElement->IsNil())
				{
				for (j = 0; j < vShape[i]; j++)
					{
					pArrayIndices->AppendInteger(j*iSkipCount);
					}
				}
			else if (pElement->IsList())
				{
				for (j = 0; j < pElement->GetCount(); j++)
					{
					if (!(pElement->GetElement(j)->IsInteger()))
						{
						pArrayIndices->Discard();
						return pCC->CreateError(CONSTLIT("An element of the index list is not an integer, list of integers, or Nil."));
						}
					else if (pElement->GetElement(j)->GetIntegerValue() >= vShape[i])
						{
						pArrayIndices->Discard();
						return pCC->CreateError(CONSTLIT("Out of bounds."));
						}
					}

				for (j = 0; j < pElement->GetCount(); j++)
					{
					pArrayIndices->AppendInteger(pElement->GetElement(j)->GetIntegerValue()*iSkipCount);
					}
				}
			else if (pElement->IsInteger())
				{
				if (pElement->GetIntegerValue() >= vShape[i])
					{
					pArrayIndices->Discard();
					return pCC->CreateError(CONSTLIT("Out of bounds."));
					}
				pArrayIndices->AppendInteger(pElement->GetIntegerValue()*iSkipCount);
				}
			else
				{
				pArrayIndices->Discard();
				return pCC->CreateError(CONSTLIT("An element of the index list is not an integer, list of integers, or Nil."));
				}
			}
		else
			{
			pNewArrayIndices = pCC->CreateLinkedList();
			if (pElement->IsNil())
				{
				for (j = 0; j < pArrayIndices->GetCount(); j++)
					{
					for (k = 0; k < vShape[i]; k++)
						{
						pNewArrayIndices->AppendInteger(pArrayIndices->GetElement(j)->GetIntegerValue() + iSkipCount*k);
						}
					}
				}
			else if (pElement->IsList())
				{
				for (j = 0; j < pElement->GetCount(); j++)
					{
					if (!(pElement->GetElement(j)->IsInteger()))
						{
						pArrayIndices->Discard();
						pNewArrayIndices->Discard();
						return pCC->CreateError(CONSTLIT("An element of the index list is not an integer, list of integers, or Nil."));
						}
					else if (pElement->GetElement(j)->GetIntegerValue() >= vShape[i])
						{
						pArrayIndices->Discard();
						pNewArrayIndices->Discard();
						return pCC->CreateError(CONSTLIT("Out of bounds."));
						}
					}

				for (j = 0; j < pArrayIndices->GetCount(); j++)
					{
					for (k = 0; k < pElement->GetCount(); k++)
						{
						pNewArrayIndices->AppendInteger(pArrayIndices->GetElement(j)->GetIntegerValue() + iSkipCount*pElement->GetElement(k)->GetIntegerValue());
						}
					}
				}
			else if (pElement->IsInteger())
				{
				if (pElement->GetIntegerValue() >= vShape[i])
					{
					pArrayIndices->Discard();
					pNewArrayIndices->Discard();
					return pCC->CreateError(CONSTLIT("Out of bounds."));
					}

				for (j = 0; j < pArrayIndices->GetCount(); j++)
					{
					pNewArrayIndices->AppendInteger(pArrayIndices->GetElement(j)->GetIntegerValue() + pElement->GetIntegerValue()*iSkipCount);
					}
				}
			else
				{
				pArrayIndices->Discard();
				pNewArrayIndices->Discard();
				return pCC->CreateError(CONSTLIT("An element of the index list is not an integer, list of integers, or Nil."));
				}

			pArrayIndices->Discard();
			pArrayIndices = pNewArrayIndices;
			}
		}

	return pArrayIndices;
	}

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
		throw CException(ERR_MEMORY);

	//	Initialize

	pError = pNewVector->SetSize(pCC, m_iCount);
	if (pError->IsError())
		{
		delete pNewVector;
		return pError;
		}

	pError->Discard();

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
	int iSize = this->GetCount();
	CCVector *pNewVector;
	ICCItem *pError;

	pNewVector = new CCVector(m_pCC);
	if (pNewVector == NULL)
		throw CException(ERR_MEMORY);
	pNewVector->SetContext(m_pCC);

	pError = pNewVector->SetDataArraySize(m_pCC, iSize);
	if (pError->IsError())
		{
		delete pNewVector;
		return pError;
		}

	try
		{
		pNewVector->SetShape(m_pCC, this->GetShapeArray());
		}
	catch (...)
		{
		ICCItem *pError = m_pCC->CreateError(CONSTLIT("Error transferring shape of existing vector to new vector."));
		return pError;
		}

	try
		{
		pNewVector->SetArrayData(m_pCC, this->GetDataArray());
		}
	catch (...)
		{
		ICCItem *pError = m_pCC->CreateError(CONSTLIT("Error transferring data of existing vector to new vector."));
		return pError;
		}

	//	Done

	//  if we have gotten this far, then we are all done
	pError->Discard();
	return pNewVector->Reference();
	}

void CCVectorOld::DestroyItem (void)

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

	CCodeChain::DestroyVector(this);
	}

void CCVector::DestroyItem (void)

//	DestroyItem
//
//	Destroy this item

	{
	//	No need to call delete on members of CCVector, because
	//	none were allocated using new

	//	We need to destroy the reference to the vector in pCC though
	CCodeChain::DestroyVector(this);
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

ICCItem *CCVectorOld::GetElement (int iIndex) const

//	GetElement
//
//	Return the nth element in the vector (0-based)

	{
	ASSERT(m_pCC);

	if (iIndex < 0 || iIndex >= m_iCount)
		return m_pCC->CreateNil();

	return m_pCC->CreateInteger(m_pData[iIndex]);
	}

ICCItem *CCVector::GetElement(int iIndex) const

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

	return this;
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

	for (i = 0; i < pIndices->GetCount(); i++)
		{
		pElement = pIndices->GetElement(i);
		if (pElement->IsNil())
			{
			vResultShape.Insert(vShape[i]);
			}
		else if (pElement->IsInteger())
			{
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
	if (vNewShape.GetCount() == 0 && (pIndexExtractionResult->GetCount() == 1))
		{
		pIndexExtractionResult->Discard();
		return pCC->CreateDouble(m_vData[pIndexExtractionResult->GetElement(0)->GetIntegerValue()]);
		}
	else if (vNewShape.GetCount() == 0)
		{
		pIndexExtractionResult->Discard();
		return pCC->CreateError(CONSTLIT("Error determining shape of extracted vector."));
		}

	if (pIndexExtractionResult->GetCount() != 1)
		{
		TArray<double> vContentArray;
		for (i = 0; i < pIndexExtractionResult->GetCount(); i++)
			{
			iIndex = pIndexExtractionResult->GetElement(i)->GetIntegerValue();
			vContentArray.Insert(this->m_vData[iIndex]);
			};



		pItem = pCC->CreateVectorGivenContent(vNewShape, vContentArray);
		if (pItem->IsError())
			{
			pIndexExtractionResult->Discard();
			return pItem;
			}

		pIndexExtractionResult->Discard();
		return pItem;
		}
	else
		{
		pItem = pCC->CreateDouble(m_vData[pIndexExtractionResult->Head(pCC)->GetIntegerValue()]);
		pIndexExtractionResult->Discard();
		return pItem;
		}
	}

CString CCVectorOld::Print (DWORD dwFlags)

//	Print
//
//	Print a user-visible message

// TODO: get this function to print out shape of the vector instead

	{
	return strPatternSubst(LITERAL("[vector with (%d) elements]"), m_iCount);
	}

CString CCVector::PrintWithoutShape (CCodeChain *pCC, DWORD dwFlags)
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
		sPrintedVector.Append(LITERAL("( "));
		for (i = 0; i < m_vShape[0]; i++)
			{
			ICCItem *pIndices = CCodeChain::CreateLinkedList();
			if (pIndices->IsError())
				{
				return LITERAL("Error printing vector: memory could not be allocated for temporary index list.");
				};
			pIndices->AppendInteger(i);

			ICCItem *pSubVector = IndexVector(pCC, pIndices);
			if (pSubVector->IsError())
				{
				pIndices->Discard();
				sPrintedVector = strPatternSubst(LITERAL("Error printing vector: %s"), pSubVector->GetStringValue());
				pSubVector->Discard();
				return sPrintedVector;
				}

			CString sPrintedSubVector;
			if (pSubVector->IsDouble())
				sPrintedSubVector = pSubVector->Print(dwFlags);
			else
				sPrintedSubVector = (dynamic_cast <CCVector *> (pSubVector))->PrintWithoutShape(pCC, dwFlags);

			sPrintedVector.Append(sPrintedSubVector);

			if (i != m_vShape[0] - 1)
				sPrintedVector.Append(LITERAL(", "));

			pSubVector->Discard();
			pIndices->Discard();
			};
		sPrintedVector.Append(LITERAL(" )"));

		return sPrintedVector;
		}
	else
		{
		return LITERAL("Empty vector.");
		}
	}

CString CCVector::Print (DWORD dwFlags)
	//	Print
	//
	//	Print a user-visible message

	// TODO: get this function to print out shape of the vector instead

	{
	int i;
	CString sPrintedVector;

	sPrintedVector.Append(LITERAL("shape:  ("));
	for (i = 0; i < m_vShape.GetCount(); i++)
		{
		sPrintedVector.Append(strPatternSubst(LITERAL("%d"), m_vShape[i]));
		if (i != m_vShape.GetCount() - 1)
			{
			sPrintedVector.Append(LITERAL(", "));
			}
		}
	sPrintedVector.Append(LITERAL(")\n"));

	sPrintedVector.Append(this->PrintWithoutShape(NULL, dwFlags));

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

bool CCVectorOld::SetElement (int iIndex, int iElement)

//	SetElement
//
//	Sets the nth element in the vector (0-based)

	{
	if (iIndex < 0 || iIndex >= m_iCount)
		return false;

	m_pData[iIndex] = iElement;
	return true;
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
			throw CException(ERR_MEMORY);

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
		throw CException(ERR_MEMORY);
		}
	
	return pCC->CreateTrue();
	}

ICCItem *CCVector::SetShapeArraySize (CCodeChain *pCC, int iNewSize)

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
		}

	//  confirm that this is the right function to use
	m_vShape.InsertEmpty(iExpansion);
	if (allocationError = NOERROR)
		{
		return pCC->CreateTrue();
		}
	else
		{
		throw CException(ERR_MEMORY);
		}
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

