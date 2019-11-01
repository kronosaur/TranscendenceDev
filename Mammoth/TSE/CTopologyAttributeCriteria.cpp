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
	CMemoryWriteStream Stream;
	if (Stream.Create() != NOERROR)
		return NULL_STR;

	CAttributeCriteria::WriteAsString(Stream, m_AttribsRequired, CONSTLIT("+"));
	CAttributeCriteria::WriteAsString(Stream, m_SpecialRequired, CONSTLIT("+"));
	CAttributeCriteria::WriteAsString(Stream, m_AttribsNotAllowed, CONSTLIT("-"));
	CAttributeCriteria::WriteAsString(Stream, m_SpecialNotAllowed, CONSTLIT("-"));
	Stream.Write(m_Level.AsString());

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
			&& m_SpecialNotAllowed.GetCount() == 0);
	}

bool CTopologyAttributeCriteria::Matches (const CTopologyNode &Node) const

//	Matches
//
//	Returns TRUE if the given node matches the criteria

	{
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
	while (*pPos != '\0')
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

	return NOERROR;
	}

