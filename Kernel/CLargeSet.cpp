//	CLargeSet.cpp
//
//	CLargeSet class

#include "Kernel.h"
#include "KernelObjID.h"

const int BITS_PER_DWORD =						32;

CLargeSet::CLargeSet (int iSize)

//	CLargeSet constructor

	{
	if (iSize != -1)
		{
		int iCount = AlignUp(iSize, BITS_PER_DWORD) / BITS_PER_DWORD;
		m_Set.InsertEmpty(iCount);
		for (int i = 0; i < iCount; i++)
			m_Set[i] = 0;
		}
	}

void CLargeSet::Clear (DWORD dwValue)

//	Clear
//
//	Remove the value from the set

	{
	DWORD dwPos = dwValue / BITS_PER_DWORD;
	DWORD dwBit = dwValue % BITS_PER_DWORD;

	if (dwPos >= (DWORD)m_Set.GetCount())
		return;

	m_Set[dwPos] = (m_Set[dwPos] & ~(1 << dwBit));
	}

void CLargeSet::ClearAll (void)

//	ClearAll
//
//	Removes all bits

	{
	m_Set.DeleteAll();
	}

bool CLargeSet::InitFromString (const CString &sValue, DWORD dwMaxValue, CString *retsError)

//	InitFromString
//
//	Initializes the set from a string with the following format:
//
//	"5"			->	5 is in the set
//	"1-5"		->	The numbers 1-5 are in the set
//	"4, 6-10"	->	The numbers 4 and 6-10 are in the set.
//	"10+"		->	The numbers from 10 to dwMaxValue are in the set
//
//	NOTE: This datastructure is not sparse, and should only be used with small
//	positive integers.

	{
	//	Set starts out empty

	ClearAll();

	//	Parse
	
	char *pPos = sValue.GetASCIIZPointer();
	char *pPosEnd = pPos + sValue.GetLength();
	while (pPos < pPosEnd)
		{
		//	Ignore whitespace

		if (strIsWhitespace(pPos))
			pPos++;

		//	Minus signs and decimals are not valid in this context, so we
		//	return an error to make the caller aware (rather than fail silently).

		else if (*pPos == '-' || *pPos == '.')
			{
			if (retsError) *retsError = strPatternSubst(CONSTLIT("Only small positive integers are supported: %s"), sValue);
			return false;
			}

		//	If this is a number, then parse it. We only support positive numbers.

		else if (*pPos >= '0' && *pPos <= '9')
			{
			char *pEnd;
			bool bFailed;
			DWORD dwStartValue = (DWORD)strParseInt(pPos, 0, &pEnd, &bFailed);
			if (bFailed)
				{
				if (retsError) *retsError = strPatternSubst(CONSTLIT("Invalid number set format: %s"), sValue);
				return false;
				}

			pPos = pEnd;
			DWORD dwEndValue;

			//	If we have a dash, then we're looking for a range.

			if (*pPos == '-')
				{
				pPos++;
				dwEndValue = (DWORD)strParseInt(pPos, 0, &pEnd, &bFailed);
				if (bFailed)
					{
					if (retsError) *retsError = strPatternSubst(CONSTLIT("Invalid number set format: %s"), sValue);
					return false;
					}

				pPos = pEnd;
				}

			//	If we have a +, then we're going to the max value.

			else if (*pPos == '+')
				{
				pPos++;
				if (dwMaxValue == 0)
					{
					if (retsError) *retsError = strPatternSubst(CONSTLIT("Cannot use + unless max value is defined: %s"), sValue);
					return false;
					}

				dwEndValue = dwMaxValue;
				}

			//	Otherwise, we only have a single number

			else
				dwEndValue = dwStartValue;
			
			//	Now add the numbers to the set

			DWORD i;
			for (i = dwStartValue; i <= dwEndValue; i++)
				Set(i);
			}

		//	Otherwise, continue parsing

		else
			pPos++;
		}

	//	Success

	return true;
	}

bool CLargeSet::IsEmpty (void) const

//	IsEmpty
//
//	Returns TRUE if empty

	{
	int i;

	for (i = 0; i < m_Set.GetCount(); i++)
		if (m_Set[i])
			return false;

	return true;
	}

bool CLargeSet::IsSet (DWORD dwValue) const

//	IsSet
//
//	Returns TRUE if the value is in the set

	{
	DWORD dwPos = dwValue / BITS_PER_DWORD;
	DWORD dwBit = dwValue % BITS_PER_DWORD;

	if (dwPos >= (DWORD)m_Set.GetCount())
		return false;

	return ((m_Set[dwPos] & (1 << dwBit)) ? true : false);
	}

void CLargeSet::Set (DWORD dwValue)

//	Set
//
//	Add the value to the set

	{
	DWORD dwPos = dwValue / BITS_PER_DWORD;
	DWORD dwBit = dwValue % BITS_PER_DWORD;
	
	int iOldCount = m_Set.GetCount();
	if (dwPos >= (DWORD)iOldCount)
		{
		m_Set.InsertEmpty(dwPos - iOldCount + 1);
		for (int i = iOldCount; i < m_Set.GetCount(); i++)
			m_Set[i] = 0;
		}

	m_Set[dwPos] = (m_Set[dwPos] | (1 << dwBit));
	}
