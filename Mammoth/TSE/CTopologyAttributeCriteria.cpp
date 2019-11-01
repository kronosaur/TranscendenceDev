//	CTopologyAttributeCriteria.cpp
//
//	CTopologyAttributeCriteria class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CString CTopologyAttributeCriteria::AsString (void) const

//	AsString
//
//	Represent as a string.

	{
	CMemoryWriteStream Stream(64 * 1024);
	if (Stream.Create() != NOERROR)
		return NULL_STR;

	WriteSubExpression(Stream);

	return CString(Stream.GetPointer(), Stream.GetLength());
	}

bool CTopologyAttributeCriteria::IsEmpty (void) const

//	IsEmpty
//
//	Return TRUE if empty.

	{
	return (m_AttribsRequired.GetCount() == 0 
			&& m_AttribsNotAllowed.GetCount() == 0 
			&& m_SpecialRequired.GetCount() == 0 
			&& m_SpecialNotAllowed.GetCount() == 0
			&& m_Level.IsEmpty()
			&& !m_pOr);
	}

bool CTopologyAttributeCriteria::Matches (const CTopologyNode &Node) const

//	Matches
//
//	Returns TRUE if the given node matches the criteria

	{
	//	Check the sub-expression first. If it matches then we're done.

	if (m_pOr && m_pOr->Matches(Node))
		return true;

	//	Check required attributes

	for (int i = 0; i < m_AttribsRequired.GetCount(); i++)
		if (!Node.HasAttribute(m_AttribsRequired[i]))
			return false;

	//	Check disallowed attributes

	for (int i = 0; i < m_AttribsNotAllowed.GetCount(); i++)
		if (Node.HasAttribute(m_AttribsNotAllowed[i]))
			return false;

	//	Check special required attributes

	for (int i = 0; i < m_SpecialRequired.GetCount(); i++)
		if (!Node.HasSpecialAttribute(m_SpecialRequired[i]))
			return false;

	//	Check disallowed special attributes

	for (int i = 0; i < m_SpecialNotAllowed.GetCount(); i++)
		if (Node.HasSpecialAttribute(m_SpecialNotAllowed[i]))
			return false;

	//	Check level

	if (!m_Level.Matches(Node.GetLevel()))
		return false;

	return true;
	}

ALERROR CTopologyAttributeCriteria::Parse (const CString &sCriteria)

//	Parse
//
//	Parses a string criteria

	{
	const char *pPos = sCriteria.GetASCIIZPointer();
	return ParseSubExpression(pPos);
	}

ALERROR CTopologyAttributeCriteria::ParseSubExpression (const char *pPos)

//	ParseSubExpression
//
//	Parses a string criteria

	{
	while (*pPos != '\0' && *pPos != '|')
		{
		switch (*pPos)
			{
			case '+':
			case '-':
				{
				bool bRequired = (*pPos == '+');
				bool bBinaryParam;
				CString sParam = ::ParseCriteriaParam(&pPos, false, &bBinaryParam);

				if (bRequired)
					{
					if (bBinaryParam)
						m_SpecialRequired.Insert(sParam);
					else
						m_AttribsRequired.Insert(sParam);
					}
				else
					{
					if (bBinaryParam)
						m_SpecialNotAllowed.Insert(sParam);
					else
						m_AttribsNotAllowed.Insert(sParam);
					}
				break;
				}

			case '=':
			case '>':
			case '<':
			case 'L':
				{
				if (!m_Level.Parse(pPos, &pPos))
					return ERR_FAIL;

				break;
				}
			}

		pPos++;
		}

	//	Recurse if we have an OR expression.

	if (*pPos == '|')
		{
		pPos++;
		m_pOr.Set(new CTopologyAttributeCriteria);
		return m_pOr->ParseSubExpression(pPos);
		}

	return NOERROR;
	}

void CTopologyAttributeCriteria::WriteSubExpression (CMemoryWriteStream &Stream) const

//	WriteSubExpression
//
//	Writes sub-expression to stream.

	{
	bool bSpaceNeeded = false;

	WriteAsString(Stream, m_AttribsRequired, CONSTLIT("+"), bSpaceNeeded);
	WriteAsString(Stream, m_SpecialRequired, CONSTLIT("+"), bSpaceNeeded);
	WriteAsString(Stream, m_AttribsNotAllowed, CONSTLIT("-"), bSpaceNeeded);
	WriteAsString(Stream, m_SpecialNotAllowed, CONSTLIT("-"), bSpaceNeeded);

	if (!m_Level.IsEmpty())
		{
		if (bSpaceNeeded)
			Stream.WriteChar(' ');

		Stream.Write(m_Level.AsString());
		bSpaceNeeded = true;
		}

	//	If we have an OR expression, recurse.

	if (m_pOr)
		{
		Stream.Write(CONSTLIT(" | "));
		m_pOr->WriteSubExpression(Stream);
		}
	}

void CTopologyAttributeCriteria::WriteAsString (IWriteStream &Stream, const TArray<CString> &Attribs, const CString &sPrefix, bool &iobSpaceNeeded)

//	WriteAsString
//
//	Write as a string.

	{
	for (int i = 0; i < Attribs.GetCount(); i++)
		{
		if (iobSpaceNeeded)
			Stream.WriteChar(' ');

		Stream.WriteChars(sPrefix);
		Stream.WriteChars(Attribs[i]);
		Stream.WriteChar(';');

		iobSpaceNeeded = true;
		}
	}

