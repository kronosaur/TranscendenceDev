//	CIntegerRangeCriteria.cpp
//
//	CIntegerRangeCriteria class
//	Copryight (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CString CIntegerRangeCriteria::AsString (char chModifier) const

//	AsString
//
//	Returns criteria as a string.

	{
	CString sModifier;
	if (chModifier != '\0')
		sModifier = CString(&chModifier, 1);

	if (IsEmpty())
		return NULL_STR;
	else if (m_iEqualToValue != -1)
		return strPatternSubst(CONSTLIT("=%s%d;"), sModifier, m_iEqualToValue);
	else if (m_iLessThanValue == -1)
		return strPatternSubst(CONSTLIT(">%s%d;"), sModifier, m_iGreaterThanValue);
	else if (m_iGreaterThanValue == -1)
		return strPatternSubst(CONSTLIT("<%s%d;"), sModifier, m_iLessThanValue);
	else
		return strPatternSubst(CONSTLIT(">%s%d; <%s%d;"), sModifier, m_iGreaterThanValue, m_iLessThanValue);
	}

bool CIntegerRangeCriteria::Matches (int iValue) const

//	Matches
//
//	Returns TRUE if we match the given value.

	{
	if (m_iEqualToValue != -1 && iValue != m_iEqualToValue)
		return false;

	if (m_iGreaterThanValue != -1 && iValue <= m_iGreaterThanValue)
		return false;

	if (m_iLessThanValue != -1 && iValue >= m_iLessThanValue)
		return false;

	return true;
	}

bool CIntegerRangeCriteria::Parse (const char *pPos, const char **retpPos, char *retchModifier)

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

	{
	//	Initialize to defaults and short-circuit.

	*this = CIntegerRangeCriteria();
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
			int iEqualAdj = 0;

			if (*pPos == '\0')
				return false;

			if (*pPos == '=')
				{
				iEqualAdj = 1;
				pPos++;
				}

			if (!(*pPos >= '0' && *pPos <= '9') && *pPos != '-' && *pPos != '+')
				chModifier = *pPos++;

			bool bFailed;
			int iValue = strParseInt(pPos, 0, &pPos, &bFailed);
			if (bFailed)
				return false;

			if (chOp == '=')
				m_iEqualToValue = iValue;
			else if (chOp == '>')
				m_iGreaterThanValue = iValue - iEqualAdj;
			else if (chOp == '<')
				m_iLessThanValue = iValue + iEqualAdj;

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
			int iLowValue = strParseInt(pPos, 0, &pPos, &bFailed);
			if (bFailed)
				return false;

			int iHighValue = -1;
			if (*pPos == '-')
				{
				*pPos++;

				iHighValue = strParseInt(pPos, 0, &pPos, &bFailed);
				if (bFailed)
					iHighValue = LONG_MAX;
				}

			if (iHighValue == -1)
				m_iEqualToValue = iLowValue;
			else if (iHighValue == LONG_MAX)
				m_iGreaterThanValue = iLowValue - 1;
			else
				{
				m_iGreaterThanValue = iLowValue - 1;
				m_iLessThanValue = iHighValue + 1;
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
