//	CDoubleRangeCriteria.cpp
//
//	CDoubleRangeCriteria class
//	Copryight (c) 2026 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

//	AsString
//
//	Returns criteria as a string.
//
CString CDoubleRangeCriteria::AsString (char chModifier) const

	{
	CString sModifier;
	if (chModifier != '\0')
		sModifier = CString(&chModifier, 1);

	if (IsEmpty())
		return NULL_STR;
	else if (!IS_NAN(m_rEqualToValue))
		return strPatternSubst(CONSTLIT(" =%s%d;"), sModifier, m_rEqualToValue);
	else if (IS_NAN(m_rLessThanValue))
		return strPatternSubst(CONSTLIT(" >%s%d;"), sModifier, m_rGreaterThanValue);
	else if (IS_NAN(m_rGreaterThanValue))
		return strPatternSubst(CONSTLIT(" <%s%d;"), sModifier, m_rLessThanValue);
	else
		return strPatternSubst(CONSTLIT(" >%s%d; <%s%d;"), sModifier, m_rGreaterThanValue, sModifier, m_rLessThanValue);
	}

//	GetRange
//
//	Returns the match range.
//
bool CDoubleRangeCriteria::GetRange (Metric *retrMin, Metric *retrMax) const

	{
	if (!IS_NAN(m_rEqualToValue))
		{
		if (retrMin) *retrMin = m_rEqualToValue;
		if (retrMax) *retrMax = m_rEqualToValue;
		return true;
		}
	else if (IS_NAN(m_rLessThanValue) && IS_NAN(m_rGreaterThanValue))
		{ 
		if (retrMin) *retrMin = R_NAN;
		if (retrMax) *retrMax = R_NAN;
		return false;
		}
	else
		{
		if (retrMax)
			{
			if (!IS_NAN(m_rLessThanValue))
				*retrMax = m_rLessThanValue - 1;
			else
				*retrMax = -1;
			}

		if (retrMin)
			{
			if (!IS_NAN(m_rGreaterThanValue))
				*retrMin = m_rGreaterThanValue + 1;
			else
				*retrMin = -1;
			}

		return true;
		}
	}

//	Matches
//
//	Returns TRUE if we match the given value.
//
bool CDoubleRangeCriteria::Matches (Metric rValue) const

	{
	if (!IS_NAN(m_rEqualToValue) && rValue != m_rEqualToValue)
		return false;

	if (!IS_NAN(m_rGreaterThanValue) && rValue <= m_rGreaterThanValue)
		return false;

	if (!IS_NAN(m_rLessThanValue) && rValue >= m_rLessThanValue)
		return false;

	return true;
	}

//	Parse
//
//	Parses a criteria. We accept the following syntax:
//
//	=n
//	>n
//	<n
//	>=n
//	<=n
//
//	=Mn
//	>Mn
//	<Mn
//	>=Mn
//	<=Mn
//
//	M:n
//	M:n-m
//
//	Where M is a modifier character and n is a non-negative integer.
//
bool CDoubleRangeCriteria::Parse (const char *pPos, const char **retpPos, char *retchModifier)

	{
	//	Initialize to defaults and short-circuit.

	*this = CDoubleRangeCriteria();
	if (pPos == NULL || *pPos == '\0')
		return true;

	//	Parse

	char chModifier = '\0';
	switch (*pPos)
		{
		case '=':
		case '>':
		case '<':
		{
		char chOp = *pPos++;
		Metric rEqualAdj = 0;

		if (*pPos == '\0')
			return false;

		if (*pPos == '=')
			{
			rEqualAdj = g_Epsilon;
			pPos++;
			}

		if (!(*pPos >= '0' && *pPos <= '9') && *pPos != '-' && *pPos != '+')
			chModifier = *pPos++;

		bool bFailed;
		Metric rValue = strParseDouble(pPos, 0.0, &pPos, &bFailed);
		if (bFailed)
			return false;

		if (chOp == '=')
			m_rEqualToValue = rValue;
		else if (chOp == '>')
			m_rGreaterThanValue = rValue - rEqualAdj;
		else if (chOp == '<')
			m_rLessThanValue = rValue + rEqualAdj;

		if (retchModifier)
			*retchModifier = chModifier;

		break;
		}

		default:
		{
		chModifier = *pPos++;
		if (*pPos != ':')
			return false;

		pPos++;

		bool bFailed;
		Metric rLowValue = strParseDouble(pPos, 0.0, &pPos, &bFailed);
		if (bFailed)
			return false;

		Metric rHighValue = R_NAN;
		if (*pPos == '-')
			{
			pPos++;

			rHighValue = strParseDouble(pPos, 0.0, &pPos, &bFailed);
			if (bFailed)
				rHighValue = R_INF;
			}
		
		if (IS_NAN(rHighValue))
			m_rEqualToValue = rLowValue;
		else if (IS_P_INF(rHighValue))
			m_rGreaterThanValue = rLowValue - g_Epsilon;
		else
			{
			m_rGreaterThanValue = rLowValue - g_Epsilon;
			m_rLessThanValue = rHighValue + g_Epsilon;
			}

		break;
		}
		}

	if (retpPos)
		*retpPos = pPos;

	if (retchModifier)
		*retchModifier = chModifier;

	return true;
	}
