//	CMissionCriteria.cpp
//
//	CMissionCriteria class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

const CMissionCriteria CMissionCriteria::m_Null;

void CMissionCriteria::Parse (const CString &sCriteria)

//	Parse
//
//	Parse mission criteria.

	{
	//	Initialize

	*this = CMissionCriteria();
	ParseSubExpression(sCriteria.GetASCIIZPointer());
	}

void CMissionCriteria::ParseSubExpression (const char *pPos)

//	ParseSubExpression
//
//	Parses a subexpression.

	{
	//	Parse

	while (*pPos != '\0' && *pPos != '|')
		{
		switch (*pPos)
			{
			case '*':
				m_bIncludeActive = true;
				m_bIncludeOpen = true;
				m_bIncludeRecorded = true;
				m_bIncludeUnavailable = true;
				break;

			case 'a':
				m_bIncludeActive = true;
				break;

			case 'c':
				//	This includes missions that have been completed (successfully
				//	or not). These missions may or may not have been debriefed, thus
				//	some of these missions are Active and some are Recorded.
				//
				//	NOTE: We don't include this category in * because * already
				//	includes all active and all recorded missions.

				m_bIncludeCompleted = true;
				break;

			//	This character is used in typFind to return mission objects. For
			//	compatibility, we allow this character here (even though it does
			//	nothing).

			case 'n':
				break;

			case 'o':
				m_bIncludeOpen = true;
				break;

			case 'r':
				m_bIncludeRecorded = true;
				break;

			case 'u':
				m_bIncludeUnavailable = true;
				break;

			case 'A':
				m_bOnlyMissionArcs = true;
				break;

			case 'D':
				m_bOnlySourceDebriefer = true;
				break;

			case 'P':
				m_bPriorityOnly = true;
				break;

			case 'S':
				m_bOnlySourceOwner = true;
				break;

			case '+':
			case '-':
				{
				bool bRequired = (*pPos == '+');
				bool bBinaryParam;
				CString sParam = ParseCriteriaParam(&pPos, false, &bBinaryParam);

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
			}

		pPos++;
		}

	//	Make sure we include some missions

	if (!m_bIncludeUnavailable
			&& !m_bIncludeActive
			&& !m_bIncludeCompleted
			&& !m_bIncludeRecorded
			&& !m_bIncludeOpen)
		{
		m_bIncludeActive = true;
		m_bIncludeOpen = true;
		m_bIncludeRecorded = true;
		m_bIncludeUnavailable = true;
		}

	//	If we have a sub-expression, recurse

	if (*pPos == '|')
		{
		pPos++;

		m_pOr.Set(new CMissionCriteria);
		m_pOr->ParseSubExpression(pPos);
		}
	}
