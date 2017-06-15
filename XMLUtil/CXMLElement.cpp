//	CXMLElement.cpp
//
//	CXMLElement class

#include <windows.h>
#include "Alchemy.h"
#include "XMLUtil.h"

CAtomizer CXMLElement::m_Keywords;

CXMLElement::CXMLElement (void) :
		m_dwTag(0),
		m_pParent(NULL)

//	CXMLElement constructor

	{
	}

CXMLElement::CXMLElement (const CXMLElement &Obj)

//	CXMLElement constructor

	{
	int i;

	m_dwTag = Obj.m_dwTag;
	m_pParent = Obj.m_pParent;
	m_Attributes = Obj.m_Attributes;
	m_ContentText = Obj.m_ContentText;

	m_ContentElements.InsertEmpty(Obj.m_ContentElements.GetCount());
	for (i = 0; i < m_ContentElements.GetCount(); i++)
		m_ContentElements[i] = new CXMLElement(*Obj.m_ContentElements[i]);
	}

CXMLElement::CXMLElement (const CString &sTag, CXMLElement *pParent) : 
		m_dwTag(m_Keywords.Atomize(sTag)),
		m_pParent(pParent)

//	CXMLElement constructor

	{
	}

CXMLElement &CXMLElement::operator= (const CXMLElement &Obj)

//	CXMLElement operator=

	{
	int i;

	CleanUp();

	m_dwTag = Obj.m_dwTag;
	m_pParent = Obj.m_pParent;
	m_Attributes = Obj.m_Attributes;
	m_ContentText = Obj.m_ContentText;

	m_ContentElements.InsertEmpty(Obj.m_ContentElements.GetCount());
	for (i = 0; i < m_ContentElements.GetCount(); i++)
		m_ContentElements[i] = new CXMLElement(*Obj.m_ContentElements[i]);

	return *this;
	}

ALERROR CXMLElement::AddAttribute (const CString &sAttribute, const CString &sValue)

//	AddAttribute
//
//	Add the given attribute to our table

	{
	m_Attributes.SetAt(m_Keywords.Atomize(sAttribute), sValue);
	return NOERROR;
	}

ALERROR CXMLElement::AppendContent (const CString &sContent, int iIndex)

//	AppendContent
//
//	Appends some content

	{
	//	Always append to the last content element

	int iCount = m_ContentText.GetCount();
	if (iCount == 0)
		m_ContentText.Insert(sContent);
	else if (iIndex >= 0 && iIndex < iCount)
		m_ContentText[iIndex].Append(sContent);
	else
		m_ContentText[iCount - 1].Append(sContent);

	return NOERROR;
	}

ALERROR CXMLElement::AppendSubElement (CXMLElement *pElement, int iIndex)

//	AppendSubElement
//
//	Append a sub element. We take ownership of pElement.

	{
	//	Are we appending to the end?

	if (iIndex < 0 || iIndex >= m_ContentElements.GetCount())
		{
		//	If this element was previously empty, we have to add
		//	a content text item. [Because we optimize the case where
		//	there are no children.]

		if (m_ContentText.GetCount() == 0)
			m_ContentText.Insert(NULL_STR);

		//	Append the element

		m_ContentElements.Insert(pElement);

		//	We always add a new content text value at the end

		m_ContentText.Insert(NULL_STR);
		}

	//	Otherwise, we're inserting at a position.

	else
		{
		//	If this element was previously empty, we have to add
		//	a content text item. [Because we optimize the case where
		//	there are no children.]

		if (m_ContentText.GetCount() == 0)
			m_ContentText.Insert(NULL_STR);

		m_ContentElements.Insert(pElement, iIndex);
		m_ContentText.Insert(NULL_STR, iIndex + 1);
		}

	return NOERROR;
	}

bool CXMLElement::AttributeExists (const CString &sName)

//	AttributeExists
//
//	Returns TRUE if the attribute exists in the element

	{
	return m_Attributes.Find(m_Keywords.Atomize(sName));
	}

void CXMLElement::CleanUp (void)

//	CleanUp
//
//	Free

	{
	int i;

	for (i = 0; i < m_ContentElements.GetCount(); i++)
		delete m_ContentElements[i];

	m_ContentElements.DeleteAll();
	}

CString CXMLElement::ConvertToString (void)

//	StreamToString
//
//	Convert to a string.

	{
	CMemoryWriteStream Stream;
	if (Stream.Create() != NOERROR)
		return NULL_STR;

	WriteToStream(&Stream);

	return CString(Stream.GetPointer(), Stream.GetLength());
	}

ALERROR CXMLElement::DeleteSubElement (int iIndex)

//	DeleteSubElement
//
//	Deletes the given sub-element

	{
	if (iIndex >= m_ContentElements.GetCount())
		return ERR_FAIL;

	delete m_ContentElements[iIndex];
	m_ContentElements.Delete(iIndex);

	m_ContentText[iIndex].Append(m_ContentText[iIndex + 1]);
	m_ContentText.Delete(iIndex + 1);

	return NOERROR;
	}

ALERROR CXMLElement::DeleteSubElementByTag (DWORD dwID)

//	DeleteSubElementByTag
//
//	Deletes all sub-elements with the given tag.

	{
	int i;

	for (i = 0; i < GetContentElementCount(); i++)
		{
		CXMLElement *pSub = GetContentElement(i);
		if (pSub->m_dwTag == dwID)
			{
			DeleteSubElement(i);
			i--;
			}
		}

	return NOERROR;
	}

bool CXMLElement::FindAttribute (const CString &sName, CString *retsValue) const

//	FindAttribute
//
//	If the attribute exists, returns TRUE and the attribute value.
//	Otherwise, returns FALSE

	{
	return m_Attributes.Find(m_Keywords.Atomize(sName), retsValue);
	}

bool CXMLElement::FindAttributeBool (const CString &sName, bool *retbValue) const

//	FindAttributeBool
//
//	If the attribute exists, returns TRUE and the value
//	Otherwise, returns FALSE

	{
	CString *pValue = m_Attributes.GetAt(m_Keywords.Atomize(sName));
	if (pValue == NULL)
		return false;

	if (retbValue)
		*retbValue = IsBoolTrueValue(*pValue);
	return true;
	}

bool CXMLElement::FindAttributeDouble (const CString &sName, double *retrValue) const

//	FindAttributeDouble
//
//	Finds an attribute.

	{
	CString *pValue = m_Attributes.GetAt(m_Keywords.Atomize(sName));
	if (pValue == NULL)
		return false;

	if (retrValue)
		*retrValue = strToDouble(*pValue, 0.0);
	return true;
	}

bool CXMLElement::FindAttributeInteger (const CString &sName, int *retiValue) const

//	FindAttributeInteger
//
//	If the attribute exists, returns TRUE and the attribute value.
//	Otherwise, returns FALSE

	{
	CString *pValue = m_Attributes.GetAt(m_Keywords.Atomize(sName));
	if (pValue == NULL)
		return false;

	if (retiValue)
		*retiValue = strToInt(*pValue, 0, NULL);
	return true;
	}

CString CXMLElement::GetAttribute (const CString &sName) const

//	GetAttribute
//
//	Returns the attribute

	{
	CString *pValue = m_Attributes.GetAt(m_Keywords.Atomize(sName));
	if (pValue == NULL)
		return NULL_STR;

	return *pValue;
	}

bool CXMLElement::GetAttributeBool (const CString &sName) const

//	GetAttributeBool
//
//	Returns TRUE or FALSE for the attribute

	{
	CString *pValue = m_Attributes.GetAt(m_Keywords.Atomize(sName));
	if (pValue == NULL)
		return false;

	return IsBoolTrueValue(*pValue);
	}

double CXMLElement::GetAttributeDouble (const CString &sName) const

//	GetAttributeDouble
//
//	Returns a double attribute.

	{
	return strToDouble(GetAttribute(sName), 0.0);
	}

double CXMLElement::GetAttributeDoubleBounded (const CString &sName, double rMin, double rMax, double rNull) const

//	GetAttributeDoubleBounded
//
//	Returns a double, insuring that it is in range

	{
	CString sValue;
	if (FindAttribute(sName, &sValue))
		{
		bool bFailed;
		double rValue = strToDouble(sValue, rNull, &bFailed);
		if (bFailed)
			return rNull;

		//	The null value is always valid

		if (rValue == rNull)
			return rValue;

		//	If iMax is less than iMin, then there is no maximum

		else if (rMax < rMin)
			return Max(rValue, rMin);

		//	Bounded

		else
			return Max(Min(rValue, rMax), rMin);
		}
	else
		return rNull;
	}

double CXMLElement::GetAttributeFloat (const CString &sName) const

//	GetAttributeFloat
//
//	Returns a floating point attribute

	{
	return strToDouble(GetAttribute(sName), 0.0);
	}

int CXMLElement::GetAttributeInteger (const CString &sName) const

//	GetAttributeInteger
//
//	Returns an integer attribute

	{
	return strToInt(GetAttribute(sName), 0, NULL);
	}

int CXMLElement::GetAttributeIntegerBounded (const CString &sName, int iMin, int iMax, int iNull) const

//	GetAttributeIntegerBounded
//
//	Returns an integer, insuring that it is in range

	{
	CString sValue;
	if (FindAttribute(sName, &sValue))
		{
		bool bFailed;
		int iValue = strToInt(sValue, iNull, &bFailed);
		if (bFailed)
			return iNull;

		//	The null value is always valid

		if (iValue == iNull)
			return iValue;

		//	If iMax is less than iMin, then there is no maximum

		else if (iMax < iMin)
			return Max(iValue, iMin);

		//	Bounded

		else
			return Max(Min(iValue, iMax), iMin);
		}
	else
		return iNull;
	}

ALERROR CXMLElement::GetAttributeIntegerList (const CString &sName, TArray<int> *pList) const

//	GetAttributeIntegerList
//
//	Appends a list of integers separated by commas

	{
	return ParseAttributeIntegerList(GetAttribute(sName), pList);
	}

ALERROR CXMLElement::GetAttributeIntegerList (const CString &sName, TArray<DWORD> *pList) const

//	GetAttributeIntegerList
//
//	Appends a list of integers separated by commas

	{
	return ParseAttributeIntegerList(GetAttribute(sName), pList);
	}

bool CXMLElement::GetAttributeIntegerRange (const CString &sName, int *retiLow, int *retiHigh, int iMin, int iMax, int iNullLow, int iNullHigh, bool bAllowInverted) const

//  GetAttributeIntegerRange
//
//  Parses two numbers separated by a hyphen. We return TRUE if we have a range.
//  Otherwise, we return FALSE and retiLow is the number.

    {
	CString sValue;
    if (!FindAttribute(sName, &sValue))
        {
        *retiLow = iNullLow;
        *retiHigh = iNullHigh;
        return (iNullLow != iNullHigh);
        }

    //  Parse the first number

    char *pPos = sValue.GetASCIIZPointer();
    bool bNullValue;
	int iValue = strParseInt(pPos, 0, &pPos, &bNullValue);
    if (bNullValue)
        {
        *retiLow = iNullLow;
        *retiHigh = iNullHigh;
        return (iNullLow != iNullHigh);
        }

    //  Make sure we're in range

    int iLow;
    if (iValue == iNullLow)
        iLow = iValue;
    else if (iMax < iMin)
        iLow = Max(iMin, iValue);
    else
        iLow = Min(Max(iMin, iValue), iMax);

    //  If there is no high value, then we just have a single number

    if (*pPos != '-')
        {
        *retiLow = iLow;
        *retiHigh = iLow;
        return false;
        }

    pPos++;

    //  Parse the next number

    int iHigh;
    iValue = strParseInt(pPos, 0, &pPos, &bNullValue);
    if (bNullValue)
        iHigh = iNullHigh;
    else if (iValue == iNullHigh)
        iHigh = iValue;
    else if (iMax < iMin)
        iHigh = Max(iMin, iValue);
    else
        iHigh = Min(Max(iMin, iValue), iMax);

    //  If low > high, then we fail, unless we allow an inverted range.

    if (iLow > iHigh && !bAllowInverted)
        {
        *retiLow = iNullLow;
        *retiHigh = iNullHigh;
        return (iNullLow != iNullHigh);
        }

    //  Otherwise, we're done

    *retiLow = iLow;
    *retiHigh = iHigh;
    return (iLow != iHigh);
    }

CXMLElement *CXMLElement::GetContentElementByTag (const CString &sTag) const

//	GetContentElementByTag
//
//	Returns a sub element of the given tag

	{
	return GetContentElementByTag(m_Keywords.Atomize(sTag));
	}

CXMLElement *CXMLElement::GetContentElementByTag (DWORD dwID) const

//	GetContentElementByTag
//
//	Returns a sub element of the given tag

	{
	for (int i = 0; i < GetContentElementCount(); i++)
		{
		CXMLElement *pElement = GetContentElement(i);

		if (pElement->m_dwTag == dwID)
			return pElement;
		}

	return NULL;
	}

int CXMLElement::GetMemoryUsage (void) const

//	GetMemoryUsage
//
//	Returns the number of bytes allocated for this element (and all its 
//	children), excluding allocation overhead.

	{
	int i;

	int iTotal = sizeof(DWORD);

	iTotal += m_Attributes.GetCount() * sizeof(DWORD);
	for (i = 0; i < m_Attributes.GetCount(); i++)
		iTotal += GetAttribute(i).GetMemoryUsage();

	for (i = 0; i < m_ContentElements.GetCount(); i++)
		iTotal += m_ContentElements[i]->GetMemoryUsage();

	for (i = 0; i < m_ContentText.GetCount(); i++)
		iTotal += m_ContentText[i].GetMemoryUsage();

	return iTotal;
	}

void CXMLElement::InitFromMerge (const CXMLElement &A, const CXMLElement &B, const TSortMap<DWORD, DWORD> &MergeFlags)

//	InitFromMerge
//
//	Merges B into A and initializes this element from the result.
//
//	1.	Root attributes are merged.
//	2.	For each child C in B, we look up C's tag in MergeFlags to figure
//		out what to do:
//
//		MERGE_APPEND: We append C, regardless of existing elements.
//
//		MERGE_APPEND_CHILDREN: We look for the first element A with C's tag.
//			If it exists, we append C's children to A's. Otherwise, we append
//			C.
//
//		MERGE_OVERRIDE: We replace any existing elements with C's tag with
//			C. If C does not exist in Src, we remove it.
//
//		(Default): We replace any existing elements with C's tag with C.

	{
	int i, j;

	CleanUp();

	//	Merge root attributes

	m_dwTag = B.m_dwTag;
	SetAttributesFromMerge(A, B, MergeFlags);

	//	We keep a running list of tags in A that we need to inherit into the 
	//	merged result.

	TSortMap<DWORD, bool> InheritFromA;
	for (i = 0; i < A.GetContentElementCount(); i++)
		{
		CXMLElement *pA = A.GetContentElement(i);

		bool bNew;
		bool *pInherit = InheritFromA.SetAt(pA->m_dwTag, &bNew);

		//	If we haven't seen this type of element before, we need to figure
		//	out what to do.

		if (bNew)
			{
			DWORD dwMerge;
			if (!MergeFlags.Find(pA->m_dwTag, &dwMerge))
				dwMerge = 0;

			//	If B overrides A, then we never inherit

			if (dwMerge & MERGE_OVERRIDE)
				*pInherit = false;

			//	Otherwise, we provisionally decide to inherit.

			else
				*pInherit = true;
			}
		}

	//	Now loop over all of B's elements and figure out what to do.

	for (i = 0; i < B.GetContentElementCount(); i++)
		{
		CXMLElement *pB = B.GetContentElement(i);

		//	Get the merge flags

		DWORD dwMerge;
		if (!MergeFlags.Find(pB->m_dwTag, &dwMerge))
			dwMerge = 0;

		//	If we're appending children, then we need to find A's element and
		//	append.

		if (dwMerge & MERGE_APPEND_CHILDREN)
			{
			CXMLElement *pA = A.GetContentElementByTag(pB->m_dwTag);
			if (pA)
				{
				//	NOTE: This only works if the element is a singleton.

				CXMLElement *pResult = pA->OrphanCopy();
				for (j = 0; j < pB->GetContentElementCount(); j++)
					{
					CXMLElement *pBChild = pB->GetContentElement(j);
					pResult->AppendSubElement(pBChild->OrphanCopy());
					}

				AppendSubElement(pResult);

				//	No need to inherit from A

				InheritFromA.SetAt(pA->m_dwTag, false);
				}
			else
				AppendSubElement(pB->OrphanCopy());
			}

		//	If we're appending, then we take B and later inherit from A.

		else if (dwMerge & MERGE_APPEND)
			AppendSubElement(pB->OrphanCopy());

		//	Otherwise, we take B but we don't inherit from A.

		else
			{
			AppendSubElement(pB->OrphanCopy());
			InheritFromA.SetAt(pB->m_dwTag, false);
			}
		}

	//	Now loop over A's elements one last time and inherit what we need.

	for (i = 0; i < A.GetContentElementCount(); i++)
		{
		CXMLElement *pA = A.GetContentElement(i);

		bool bInherit;
		if (!InheritFromA.Find(pA->m_dwTag, &bInherit) || !bInherit)
			continue;

		//	Inherit

		AppendSubElement(pA->OrphanCopy());
		}
	}

void CXMLElement::Merge (const CXMLElement &Src, const TSortMap<DWORD, DWORD> &MergeFlags)

//	Merge
//
//	Merges Src into our hierarchy. MergeFlags specify how we merge.
//
//	1.	Root attributes are always merged.
//	2.	For each child C in Src, we look up C's tag in MergeFlags to figure
//		out what to do:
//
//		MERGE_APPEND: We append C, regardless of existing elements.
//
//		MERGE_APPEND_CHILDREN: We look for the first element A with C's tag.
//			If it exists, we append C's children to A's. Otherwise, we append
//			C.
//
//		MERGE_OVERRIDE: We replace any existing elements with C's tag with
//			C. If C does not exist in Src, we remove it.
//
//		(Default): We replace any existing elements with C's tag with C.

	{
	int i, j;

	TSortMap<DWORD, bool> Replaced;

	//	Merge the attributes from Src.

	m_dwTag = Src.m_dwTag;
	MergeAttributes(Src);

	//	Loop over our merge flags. For MERGE_OVERRIDE, we delete elements that
	//	are not in Src.

	for (i = 0; i < MergeFlags.GetCount(); i++)
		{
		DWORD dwTag = MergeFlags.GetKey(i);
		DWORD dwMerge = MergeFlags[i];

		if ((dwMerge & MERGE_OVERRIDE)
				&& !Replaced.Find(dwTag))
			{
			DeleteSubElementByTag(dwTag);
			Replaced.Insert(dwTag, true);
			}
		}

	//	Loop over all child elements.

	for (i = 0; i < Src.GetContentElementCount(); i++)
		{
		CXMLElement *pSrcChild = Src.GetContentElement(i);

		//	Get the appropriate flags

		DWORD dwMerge;
		if (!MergeFlags.Find(pSrcChild->m_dwTag, &dwMerge))
			dwMerge = 0;

		//	Handle each case

		if (dwMerge & MERGE_APPEND)
			AppendSubElement(pSrcChild->OrphanCopy());

		else if (dwMerge & MERGE_APPEND_CHILDREN)
			{
			CXMLElement *pTarget = GetContentElementByTag(pSrcChild->m_dwTag);
			if (pTarget)
				{
				for (j = 0; j < pSrcChild->GetContentElementCount(); j++)
					pTarget->AppendSubElement(pSrcChild->GetContentElement(j)->OrphanCopy());
				}
			else
				AppendSubElement(pSrcChild->OrphanCopy());
			}

		//	MERGE_OVERRIDE and no flag (MERGE_REPLACE) handled here.

		else
			{
			//	Remove all existing with this tag.

			if (!Replaced.Find(pSrcChild->m_dwTag))
				{
				DeleteSubElementByTag(pSrcChild->m_dwTag);
				Replaced.Insert(pSrcChild->m_dwTag, true);
				}

			//	Add

			AppendSubElement(pSrcChild->OrphanCopy());
			}
		}
	}

void CXMLElement::MergeAttributes (const CXMLElement &Src)

//	MergeAttributes
//
//	Take all attributes from Src, replacing as appropriate.

	{
	int i;

	//	Copy all attributes (replacing in case of duplication)

	for (i = 0; i < Src.GetAttributeCount(); i++)
		SetAttribute(Src.m_Attributes.GetKey(i), Src.GetAttribute(i));
	}

void CXMLElement::MergeFrom (CXMLElement *pElement)

//	MergeFrom
//
//	Merges the given element into ours.
//
//	Attributes are replaced if they duplicate; child elements are appended to the end.

	{
	int i;

	//	Copy all attributes (replacing in case of duplication)

	for (i = 0; i < pElement->GetAttributeCount(); i++)
		SetAttribute(pElement->GetAttributeName(i), pElement->GetAttribute(i));

	//	If we have at least one text item then append it to our last
	//	text item.

	if (pElement->m_ContentText.GetCount() > 0)
		{
		if (m_ContentText.GetCount() == 0)
			m_ContentText.Insert(pElement->m_ContentText[0]);
		else
			m_ContentText[m_ContentText.GetCount() - 1].Append(pElement->m_ContentText[0]);
		}

	//	Now add each of the child elements

	for (i = 0; i < pElement->GetContentElementCount(); i++)
		{
		m_ContentElements.Insert(pElement->GetContentElement(i)->OrphanCopy());

		//	If we have no text items then we need to insert one (it goes before
		//	this new element). NOTE: This shouldn't happen if pElement is
		//	properly formed.

		if (m_ContentText.GetCount() == 0)
			m_ContentText.Insert(NULL_STR);

		//	Now insert the text that goes AFTER the element that we just
		//	inserted.

		if (i + 1 < pElement->m_ContentText.GetCount())
			m_ContentText.Insert(pElement->m_ContentText[i + 1]);
		else
			m_ContentText.Insert(NULL_STR);
		}
	}

CXMLElement *CXMLElement::OrphanCopy (void)

//	OrphanCopy
//
//	Creates a copy of the element and makes it top-level

	{
	CXMLElement *pCopy = new CXMLElement(*this);
	pCopy->m_pParent = NULL;
	return pCopy;
	}

ALERROR CXMLElement::SetAttribute (const CString &sName, const CString &sValue)

//	SetAttribute
//
//	Sets an attribute on the element.

	{
	m_Attributes.SetAt(m_Keywords.Atomize(sName), sValue);
	return NOERROR;
	}

ALERROR CXMLElement::SetAttribute (DWORD dwID, const CString &sValue)

//	SetAttribute
//
//	Sets an attribute by atom

	{
	m_Attributes.SetAt(dwID, sValue);
	return NOERROR;
	}

void CXMLElement::SetAttributesFromMerge (const CXMLElement &A, const CXMLElement &B, const TSortMap<DWORD, DWORD> &MergeFlags)

//	SetAttributesFromMerge
//
//	Merges attributes from A and B and applies them to this element.

	{
	int iAPos = 0;
	int iBPos = 0;

	//	Attributes are sorted, so we can proceed in order

	while (iAPos < A.m_Attributes.GetCount() || iBPos < B.m_Attributes.GetCount())
		{
		DWORD dwA = (iAPos < A.m_Attributes.GetCount() ? A.m_Attributes.GetKey(iAPos) : 0);
		DWORD dwB = (iBPos < B.m_Attributes.GetCount() ? B.m_Attributes.GetKey(iBPos) : 0);

		//	If we're at the same attribute, check the flags to see what we need
		//	to do.

		if (dwA == dwB)
			{
			//	We always take B's in this case.

			SetAttribute(dwB, B.m_Attributes[iBPos]);

			//	Advance both pointers

			iAPos++;
			iBPos++;
			}

		//	If A is 0 then we've hit the end of A's attributes.

		else if (dwA == 0)
			{
			SetAttribute(dwB, B.m_Attributes[iBPos]);
			iBPos++;
			}

		//	If A > B (and B is valid) then B has an attribute that A does not 
		//	have.

		else if (dwA > dwB && dwB != 0)
			{
			SetAttribute(dwB, B.m_Attributes[iBPos]);
			iBPos++;
			}

		//	Otherwise, A has an attribute that B doesn't, so we need to check
		//	to see if we should take it.

		else
			{
			//	Get the merge flags. Attributes are in a different namespace, just in 
			//	case.

			DWORD dwAttribID = m_Keywords.Atomize(strPatternSubst(CONSTLIT("attrib.%s"), A.GetAttributeName(iAPos)));

			DWORD dwMerge;
			if (!MergeFlags.Find(dwAttribID, &dwMerge))
				dwMerge = 0;

			//	If we're not overriding, then we take A's

			if (!(dwMerge & MERGE_OVERRIDE))
				SetAttribute(dwA, A.m_Attributes[iAPos]);

			iAPos++;
			}
		}
	}

ALERROR CXMLElement::SetContentText (const CString &sContent, int iIndex)

//	SetContentText
//
//	Sets the content

	{
	//	Always append to the last content element

	int iCount = m_ContentText.GetCount();
	if (iCount == 0)
		m_ContentText.Insert(sContent);
	else if (iIndex >= 0 && iIndex < iCount)
		m_ContentText[iIndex] = sContent;
	else
		m_ContentText[iCount - 1] = sContent;

	return NOERROR;
	}

ALERROR CXMLElement::WriteToStream (IWriteStream *pStream)

//	WriteToStream
//
//	Streams out to text XML

	{
	int i;

	//	Open tag

	const CString &sTag = m_Keywords.GetIdentifier(m_dwTag);
	pStream->Write("<", 1);
	pStream->Write(sTag.GetASCIIZPointer(), sTag.GetLength());

	//	If we have attributes, write them out

	if (GetAttributeCount() > 0)
		{
		for (i = 0; i < GetAttributeCount(); i++)
			{
			CString sName = GetAttributeName(i);
			CString sValue = MakeAttribute(GetAttribute(i));

			pStream->Write(" ", 1);
			pStream->Write(sName.GetASCIIZPointer(), sName.GetLength());
			pStream->Write("=\"", 2);
			pStream->Write(sValue.GetASCIIZPointer(), sValue.GetLength());
			pStream->Write("\"", 1);
			}
		}

	//	If we don't have any sub elements, then we're done.

	if (m_ContentText.GetCount() == 0)
		{
		pStream->Write("/>", 2);
		return NOERROR;
		}

	//	End of open tag

	pStream->Write(">", 1);

	//	Output sub-elements and text

	for (i = 0; i < m_ContentElements.GetCount(); i++)
		{
		//	Write the text before this sub-element

		CString sXMLText = strToXMLText(m_ContentText[i], true);
		pStream->Write(sXMLText.GetASCIIZPointer(), sXMLText.GetLength());

		//	Write the element

		m_ContentElements[i]->WriteToStream(pStream);
		}

	//	Output any trailing text

	CString sXMLText = strToXMLText(m_ContentText[i], true);
	pStream->Write(sXMLText.GetASCIIZPointer(), sXMLText.GetLength());

	//	Close the element

	pStream->Write("</", 2);
	pStream->Write(sTag.GetASCIIZPointer(), sTag.GetLength());
	pStream->Write(">", 1);

	//	Done

	return NOERROR;
	}
