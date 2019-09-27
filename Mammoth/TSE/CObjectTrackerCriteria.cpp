//	CObjectTrackerCriteria.cpp
//
//	CObjectTrackerCriteria class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

bool CObjectTrackerCriteria::ParseCriteria (const CString &sCriteria)

//	ParseCriteria
//
//	Parse criteria.

	{
	//	Initialize back to default values.

	*this = CObjectTrackerCriteria();

	//	Parse the type criteria.

	if (CDesignTypeCriteria::ParseCriteria(sCriteria, &m_TypeCriteria) != NOERROR)
		return false;

	//	Now parse any pieces that we deal with.

	const char *pPos = sCriteria.GetPointer();
	while (*pPos != '\0')
		{
		switch (*pPos)
			{
			case 'A':
				m_bActiveOnly = true;
				break;

			case 'K':
				m_bKilledOnly = true;
				break;

			case 'P':
				m_bKnownOnly = true;
				break;

			//	We need to skip these values that are handled by CDesignTypeCriteria

			case 'L':
				ParseCriteriaParamLevelRange(&pPos);
				break;

            case 'T':
                break;

			case 'V':
				break;

			case '+':
			case '-':
				ParseCriteriaParam(&pPos, false);
				break;
			}

		pPos++;
		}

	//	Done

	return true;
	}
