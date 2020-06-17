//	CPropertyCompare.cpp
//
//	CPropertyCompare class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

bool CPropertyCompare::Eval (ICCItem *pPropertyValue) const

//	Eval
//
//	Returns TRUE or FALSE depending on whether the given property value evalues
//	to TRUE.

	{
	//	A null value never matches (this should not happen).

	if (pPropertyValue == NULL || m_iOp == opNone)
		return false;

	//	If we just care about whether the property exists, we test for Nil.

	else if (m_iOp == opNonNil)
		return !pPropertyValue->IsNil();

	//	See if this is a range compare

	else if (m_iOp == opInRange)
		{
		int iCompareMin = HelperCompareItems(pPropertyValue, m_pValue, HELPER_COMPARE_COERCE_FULL);
		if (iCompareMin < 0)
			return false;

		int iCompareMax = HelperCompareItems(pPropertyValue, m_pValue2, HELPER_COMPARE_COERCE_FULL);
		if (iCompareMax > 0)
			return false;

		return true;
		}

	//	Otherwise we need to compare two value.

	else
		{
		int iCompare = HelperCompareItems(pPropertyValue, m_pValue, HELPER_COMPARE_COERCE_FULL);
		if (iCompare == -2)
			return false;

		switch (m_iOp)
			{
			case opEqual:
				return (iCompare == 0);

			case opNotEqual:
				return (iCompare != 0);

			case opGreaterThan:
				return (iCompare == 1);

			case opGreaterThanOrEqual:
				return (iCompare >= 0);

			case opLessThan:
				return (iCompare == -1);

			case opLessThanOrEqual:
				return (iCompare <= 0);

			default:
				ASSERT(false);
				return false;
			}
		}
	}

bool CPropertyCompare::Parse (CCodeChainCtx &CCX, const CString &sExpression, CString *retsError)

//	Parse
//
//	Parses an expression of the form:
//
//	prop=value
//	prop!=value
//	prop>value
//	prop<value
//	prop>=value
//	prop<=value
//	prop[min:]
//	prop[:max]
//	prop[min:max]
//
//	Returns TRUE if we succeed.

	{
	const char *pPos = sExpression.GetASCIIZPointer();

	//	Parse the property name

	const char *pStart = pPos;
	while (*pPos != '\0' && !IsOperatorChar(*pPos))
		pPos++;

	m_sProperty = CString(pStart, pPos - pStart);
	if (m_sProperty.IsBlank())
		{
		if (retsError) *retsError = CONSTLIT("Property name expected");
		return false;
		}

	//	Parse the operator

	m_iOp = opNone;
	switch (*pPos)
		{
		case '\0':
			m_iOp = opNonNil;
			break;

		case '=':
			pPos++;
			m_iOp = opEqual;
			if (!ParseValue(CCX, pPos, m_pValue, retsError))
				return false;
			break;

		case '!':
			pPos++;
			if (*pPos == '=')
				{
				pPos++;
				m_iOp = opNotEqual;
				if (!ParseValue(CCX, pPos, m_pValue, retsError))
					return false;
				}
			break;

		case '>':
			pPos++;
			if (*pPos == '=')
				{
				pPos++;
				m_iOp = opGreaterThanOrEqual;
				}
			else
				m_iOp = opGreaterThan;

			if (!ParseValue(CCX, pPos, m_pValue, retsError))
				return false;
			break;

		case '<':
			pPos++;
			if (*pPos == '=')
				{
				pPos++;
				m_iOp = opLessThanOrEqual;
				}
			else
				m_iOp = opLessThan;

			if (!ParseValue(CCX, pPos, m_pValue, retsError))
				return false;
			break;

		case '[':
			{
			ICCItemPtr pMinValue;
			ICCItemPtr pMaxValue;
			bool bHasRangeMarker = false;

			pPos++;
			if (*pPos == ':')
				{
				bHasRangeMarker = true;
				pPos++;

				pMinValue = ICCItemPtr::Nil();
				if (!ParseValue(CCX, pPos, pMaxValue, retsError))
					return false;
				}
			else
				{
				if (!ParseValue(CCX, pPos, pMinValue, retsError))
					return false;

				if (*pPos == ':' || *pPos == '-')
					{
					bHasRangeMarker = true;
					pPos++;
					}

				if (!ParseValue(CCX, pPos, pMaxValue, retsError))
					return false;
				}

			//	If we have both min and max, then this is a range compare.

			if (!pMinValue->IsNil() && !pMaxValue->IsNil())
				{
				m_iOp = opInRange;
				m_pValue = pMinValue;
				m_pValue2 = pMaxValue;
				}

			//	If we have a min value, but no max, then this is equivalent to >=
			//	Or to =

			else if (!pMinValue->IsNil())
				{
				if (bHasRangeMarker)
					m_iOp = opGreaterThanOrEqual;
				else
					m_iOp = opEqual;

				m_pValue = pMinValue;
				}

			//	If we have a max value, but no min, then this is equivalent to <=

			else if (!pMaxValue->IsNil())
				{
				m_iOp = opLessThanOrEqual;
				m_pValue = pMaxValue;
				}

			//	Otherwise, it's just non nil

			else
				{
				m_iOp = opNonNil;
				}

			break;
			}

		default:
			break;
		}

	//	If no operator, then we have an error.

	if (m_iOp == opNone)
		{
		if (retsError) *retsError = CONSTLIT("Invalid operator");
		return false;
		}

	//	Success!

	return true;
	}

bool CPropertyCompare::ParseValue (CCodeChainCtx &CCX, const char *&pPos, ICCItemPtr &pValue, CString *retsError)
	{
	while (*pPos != '\0' && strIsWhitespace(pPos))
		pPos++;

	const char *pStart = pPos;
	bool bDone = *pPos == '\0';
	bool bInQuotes = false;
	bool bHasContent = false;
	bool bHasQuotes = false;
	while (!bDone)
		{
		switch (*pPos)
			{
			case '\0':
				bDone = true;
				break;

			//	Delimiters used for range

			case ']':
			case ':':
				if (!bInQuotes)
					bDone = true;
				break;

			//	Dash can be a delimiter if it follows a value, but not at the
			//	beginning of a value (because it could be a minus sign).

			case '-':
				if (!bInQuotes && bHasContent)
					bDone = true;
				break;

			//	Quotes

			case '\"':
				bInQuotes = !bInQuotes;
				bHasContent = true;
				bHasQuotes = true;
				break;

			default:
				bHasContent = true;
				break;
			}

		if (!bDone)
			pPos++;
		}

	CString sValue(pStart, pPos - pStart);
	if (sValue.IsBlank())
		{
		pValue = ICCItemPtr::Nil();
		return true;
		}

	if (bHasQuotes)
		sValue = strProcess(sValue, STRPROC_ESCAPE_DOUBLE_QUOTES);

	pValue = CCodeChain::CreateLiteral(sValue);
	if (pValue->IsError())
		{
		if (retsError) *retsError = pValue->GetStringValue();
		return false;
		}

	return true;
	}
