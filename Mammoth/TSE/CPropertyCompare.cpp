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

bool CPropertyCompare::Parse (const CString &sExpression, CString *retsError)

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
//
//	Returns TRUE if we succeed.

	{
	char *pPos = sExpression.GetASCIIZPointer();

	//	Parse the property name

	char *pStart = pPos;
	while (*pPos != '\0' && !IsOperatorChar(*pPos))
		pPos++;

	CString sProperty = CString(pStart, (int)(pPos - pStart));
	if (sProperty.IsBlank())
		{
		if (retsError) *retsError = CONSTLIT("Property name expected");
		return false;
		}

	//	Parse the operator

	EOperator iOp = opNone;
	switch (*pPos)
		{
		case '\0':
			iOp = opNonNil;
			break;

		case '=':
			pPos++;
			iOp = opEqual;
			break;

		case '!':
			pPos++;
			if (*pPos == '=')
				{
				pPos++;
				iOp = opNotEqual;
				}
			break;

		case '>':
			pPos++;
			if (*pPos == '=')
				{
				pPos++;
				iOp = opGreaterThanOrEqual;
				}
			else
				iOp = opGreaterThan;
			break;

		case '<':
			pPos++;
			if (*pPos == '=')
				{
				pPos++;
				iOp = opLessThanOrEqual;
				}
			else
				iOp = opLessThan;
			break;

		default:
			break;
		}

	//	If no operator, then we have an error.

	ICCItemPtr pValue;
	if (iOp == opNone)
		{
		if (retsError) *retsError = CONSTLIT("Invalid operator");
		return false;
		}

	//	Parse the value

	else if (iOp != opNonNil)
		{
		CString sValue(pPos);
		if (sValue.IsBlank())
			{
			if (retsError) *retsError = CONSTLIT("Value expected");
			return false;
			}

		CCodeChainCtx CCCtx(*g_pUniverse);
		pValue = CCCtx.LinkCode(sValue);
		if (pValue->IsError())
			{
			if (retsError) *retsError = pValue->GetStringValue();
			return false;
			}
		}

	//	Success!

	m_sProperty = sProperty;
	m_iOp = iOp;
	m_pValue = pValue;

	return true;
	}
