//	CAffinityCriteria.cpp
//
//	CAttributeCrtieria class
//	Copryight (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define MATCH_ALL						CONSTLIT("*")
#define MATCH_DEFAULT					CONSTLIT("*~")

inline bool IsWeightChar (const char *pPos) { return (*pPos == '+' || *pPos == '-' || *pPos == '*' || *pPos == '!'); }
inline bool IsDelimiterChar (const char *pPos, bool bIsSpecialAttrib = false) { return (*pPos == '\0' || *pPos == ',' || *pPos == ';' || (!bIsSpecialAttrib && strIsWhitespace(pPos))); }

CString CAffinityCriteria::AsString (void) const

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

int CAffinityCriteria::CalcWeight (std::function<bool(const CString &)> fnHasAttrib, 
								   std::function<bool(const CString &)> fnHasSpecialAttrib, 
								   std::function<int(const CString &)> fnGetFreq) const

//	CalcWeight
//
//	Computes the weight using the given functions to determine whether the 
//	attribute exists or not.

	{
	int iWeight;

	//	Special values

	if (MatchesDefault())
		iWeight = 0;

	else if (MatchesAll())
		iWeight = 1000;

	else
		{
		iWeight = 1000;

		//	Compute

		for (int i = 0; i < GetCount(); i++)
			{
			DWORD dwMatchStrength;
			bool bIsSpecial;
			const CString &sAttrib = GetAttribAndWeight(i, &dwMatchStrength, &bIsSpecial);

			bool bHasAttrib = (bIsSpecial && fnHasSpecialAttrib ? fnHasSpecialAttrib(sAttrib) : fnHasAttrib(sAttrib));

			int iAdj;
			if (fnGetFreq)
				iAdj = CalcWeightAdjWithAttribFreq(bHasAttrib, dwMatchStrength, fnGetFreq(sAttrib));
			else
				iAdj = CalcWeightAdj(bHasAttrib, dwMatchStrength);

			iWeight = iWeight * iAdj / 1000;
			}
		}

	//	Recurse, if necessary

	if (m_pOr)
		{
		int iOtherWeight = m_pOr->CalcWeight(fnHasAttrib, fnHasSpecialAttrib, fnGetFreq);
		iWeight = Max(iWeight, iOtherWeight);
		}

	return iWeight;
	}

int CAffinityCriteria::CalcWeightAdj (bool bHasAttrib, DWORD dwMatchStrength)

//	CalcWeightAdj
//
//	OPTION	CODE		HAVE ATTRIB			DON'T HAVE ATTRIB
//	---------------------------------------------------------
//	!		-4			0					1000
//	---		-3			100					1000
//	--		-2			250					1000
//	-		-1			500 				1000
//	+		+1			1000				500
//	++		+2			1000				250
//	+++		+3			1000				100
//	*		+4			1000				0
//
//	NOTE: The above numbers assume iAttribFreq is 20.

	{
	//	Default

	switch (dwMatchStrength)
		{
		case matchExcluded:
			return (bHasAttrib ? 0 : 1000);

		case matchAvoid3:
			return (bHasAttrib ? 100 : 1000);

		case matchAvoid2:
			return (bHasAttrib ? 250 : 1000);

		case matchAvoid1:
			return (bHasAttrib ? 500 : 1000);

		case matchSeek1:
			return (bHasAttrib ? 1000 : 500);

		case matchSeek2:
			return (bHasAttrib ? 1000 : 250);

		case matchSeek3:
			return (bHasAttrib ? 1000 : 100);

		case matchRequired:
			return (bHasAttrib ? 1000 : 0);

		default:
			return CalcWeightAdjCustom(bHasAttrib, dwMatchStrength);
		}
	}

int CAffinityCriteria::CalcWeightAdjCustom (bool bHasAttrib, DWORD dwMatchStrength)

//	CalcWeightAdjCustom
//
//	Handles the case where match strength is custom

	{
	DWORD dwCode = (dwMatchStrength & CODE_MASK);
	DWORD dwValue = (dwMatchStrength & VALUE_MASK);

	switch (dwCode)
		{
		case CODE_INCREASE_IF:
			if (bHasAttrib)
				return 1000 * (100 + dwValue) / 100;
			else
				return 1000;

		case CODE_DECREASE_IF:
			if (bHasAttrib)
				return (dwValue >= 100 ? 0 : 1000 * (100 - dwValue) / 100);
			else
				return 1000;

		case CODE_INCREASE_UNLESS:
			if (bHasAttrib)
				return 1000;
			else
				return 1000 * (100 + dwValue) / 100;

		case CODE_DECREASE_UNLESS:
			if (bHasAttrib)
				return 1000;
			else
				return (dwValue >= 100 ? 0 : 1000 * (100 - dwValue) / 100);

		default:
			return 1000;
		}
	}

int CAffinityCriteria::CalcWeightAdjWithAttribFreq (bool bHasAttrib, DWORD dwMatchStrength, int iAttribFreq)

//	CalcWeightAdjWithAttribFreq
//
//	If iMatchStrength is positive, then it means we want a certain
//	attribute. If we have the attribute, then we increase our probability
//	but if we don't have the attribute, our probability is normal.
//
//	If iMatchStrength is negative, then it means we DO NOT want a
//	certain attribute. If we have the attribute, then decrease our
//	probability. Otherwise, our probability is normal.
//
//	OPTION	CODE		HAVE ATTRIB			DON'T HAVE ATTRIB
//	---------------------------------------------------------
//	!		-4			0					1250
//	---		-3			50					1238
//	--		-2			160					1210
//	-		-1			500 				1125
//	+		+1			3000				500
//	++		+2			4360				160
//	+++		+3			4800				50
//	*		+4			5000				0
//
//	NOTE: The above numbers assume iAttribFreq is 20.

	{
	//	Attribute frequency must be >0 and <100.

	if (iAttribFreq <= 0 || iAttribFreq >= 100)
		{
		::kernelDebugLogPattern("ERROR: Invalid attribute frequency: %d", iAttribFreq);
		return 1000;
		}

	switch (dwMatchStrength)
		{
		//	We only appear at locations with the attribute

		case matchRequired:
			{
			//	If this location has the attribute, then we increase our chance inversely with
			//	the frequency of the attribute. That is, if we MUST be at a location with
			//	a rare attribute, then we INCREASE our probability relative to other
			//	objects, so that we keep our overall frequency relatively constant.

			if (bHasAttrib)
				return mathRound(100000.0 / iAttribFreq);

			//	If this location DOES NOT have the attribute, then we have 0 chance of
			//	appearing here.

			else
				return 0;
			}

		//	We never appear at locations with the attribute

		case matchExcluded:
			{
			//	If the location has the attribute, we have 0 chance of appearing here.

			if (bHasAttrib)
				return 0;

			//	Otherwise, our chance of appearing is proportional to the frequency
			//	of the attribute. 

			else
				return mathRound(100000.0 / (100 - iAttribFreq));
			}

		//	Compute based on match strength

		default:
			{
			//	First we compute the % of objects that should be at locations with the
			//	given attribute, based on the match strength and the global probability
			//	that a location has the given attribute.

			Metric rF;
			switch (dwMatchStrength)
				{
				case matchSeek3:
					rF = 100.0 - ((100 - iAttribFreq) * 0.05);
					break;

				case matchSeek2:
					rF = 100.0 - ((100 - iAttribFreq) * 0.16);
					break;

				case matchSeek1:
					rF = 100.0 - ((100 - iAttribFreq) * 0.5);
					break;

				case matchAvoid1:
					rF = iAttribFreq * 0.5;
					break;

				case matchAvoid2:
					rF = iAttribFreq * 0.16;
					break;

				case matchAvoid3:
					rF = iAttribFreq * 0.05;
					break;

				//	This means we've got a custom match strength

				default:
					return CalcWeightAdjCustom(bHasAttrib, dwMatchStrength);
				}

			//	Compute our probability based on whether we have the attribute our not

			if (bHasAttrib)
				return mathRound(1000.0 * rF / iAttribFreq);
			else
				return mathRound(1000.0 * (100.0 - rF) / (100 - iAttribFreq));
			}
		}
	}

const CString &CAffinityCriteria::GetAttribAndRequired (int iIndex, bool *retbRequired, bool *retbIsSpecial) const

//	GetAttribAndRequired
//
//	Returns the attrib and whether or not it is required

	{
	const SEntry &Entry = m_Attribs[iIndex];

	if (retbRequired)
		{
		switch (Entry.dwMatchStrength & CODE_MASK)
			{
			case CODE_REQUIRED:
			case CODE_SEEK:
			case CODE_INCREASE_IF:
			case CODE_DECREASE_UNLESS:
				*retbRequired = true;
				break;

			default:
				*retbRequired = false;
				break;
			}
		}

	if (retbIsSpecial)
		*retbIsSpecial = Entry.bIsSpecial;

	return Entry.sAttrib;
	}

const CString &CAffinityCriteria::GetAttribAndWeight (int iIndex, DWORD *dwMatchStrength, bool *retbIsSpecial) const

//	GetAttribAndWeight
//
//	Returns the attrib and its weight

	{
	const SEntry &Entry = m_Attribs[iIndex];

	if (dwMatchStrength)
		*dwMatchStrength = Entry.dwMatchStrength;

	if (retbIsSpecial)
		*retbIsSpecial = Entry.bIsSpecial;

	return Entry.sAttrib;
	}

bool CAffinityCriteria::IsCustomAttribOperator (const char *pPos)

//	IsCustomAttribOperator
//
//	Returns TRUE if the character is the = operator (or the | operator, for
//	backwards compatibility.

	{
	if (*pPos == '=')
		return true;
	else if (*pPos == '|')
		{
		//	Skip ahead to make sure this is not an OR operator.

		if (pPos[1] == '\0')
			return false;

		if (pPos[1] != '+' && pPos[1] != '-')
			return false;

		if (pPos[2] < '0' || pPos[2] > '9')
			return false;

		return true;
		}
	else
		return false;
	}

bool CAffinityCriteria::Matches (std::function<bool(const CString &)> fnHasAttrib, std::function<bool(const CString &)> fnHasSpecialAttrib) const

//	Matches
//
//	Returns TRUE if this criteria matches the given position in the given 
//	system.

	{
	bool bMatches;

	if (MatchesDefault())
		bMatches = false;

	else if (MatchesAll())
		bMatches = true;

	else
		{
		bMatches = true;

		for (int i = 0; i < GetCount(); i++)
			{
			bool bRequired;
			bool bIsSpecial;
			const CString &sAttrib = GetAttribAndRequired(i, &bRequired, &bIsSpecial);

			bool bHasAttrib = (bIsSpecial && fnHasSpecialAttrib ? fnHasSpecialAttrib(sAttrib) : fnHasAttrib(sAttrib));
			if (bHasAttrib != bRequired)
				{
				bMatches = false;
				break;
				}
			}
		}

	//	Recurse, if necessary

	if (!bMatches && m_pOr)
		bMatches = m_pOr->Matches(fnHasAttrib, fnHasSpecialAttrib);

	return bMatches;
	}

ALERROR CAffinityCriteria::Parse (const CString &sCriteria, CString *retsError)

//	Parse
//
//	Parses the criteria

	{
	m_Attribs.DeleteAll();
	m_dwFlags = 0;

	//	If we match all, then we have no individual criteria

	if (strEquals(sCriteria, MATCH_ALL))
		{
		}
	else if (strEquals(sCriteria, MATCH_DEFAULT))
		{
		m_dwFlags |= flagDefault;
		}
	else
		{
		const char *pPos = sCriteria.GetASCIIZPointer();
		if (ParseSubExpression(sCriteria) != NOERROR)
			{
			if (retsError) *retsError = strPatternSubst(CONSTLIT("Invalid criteria: %s"), sCriteria);
			return ERR_FAIL;
			}
		}

	return NOERROR;
	}

ALERROR CAffinityCriteria::ParseSubExpression (const char *pPos, CString *retsError)

//	ParseSubExpression
//
//	Parses the criteria

	{
	while (*pPos != '\0' && *pPos != '|')
		{
		if (IsWeightChar(pPos))
			{
			//	Parse the weight

			int iMatchWeight = 0;
			while (IsWeightChar(pPos))
				{
				switch (*pPos)
					{
					case '*':
						iMatchWeight = 4;
						break;

					case '!':
						iMatchWeight = -4;
						break;

					case '+':
						if (iMatchWeight < 3)
							iMatchWeight++;
						break;

					case '-':
						if (iMatchWeight > -3)
							iMatchWeight--;
						break;
					}

				pPos++;
				}

			//	Whitespace or empty attributes are invalid here.

			if (IsDelimiterChar(pPos))
				return ERR_FAIL;

			//	Get the attribute until the delimeter

			bool bIsSpecialAttrib = false;
			const char *pStart = pPos;
			const char *pCustomWeight = NULL;
			while (!IsDelimiterChar(pPos, bIsSpecialAttrib))
				{
				if (*pPos == ':')
					bIsSpecialAttrib = true;
				else if (IsCustomAttribOperator(pPos))
					pCustomWeight = pPos;
				else if (*pPos == '|')
					break;

				pPos++;
				}

			//	Add to our list

			SEntry *pEntry = m_Attribs.Insert();
			pEntry->bIsSpecial = bIsSpecialAttrib;

			//	If we have a custom weight, then we need to parse the weight.

			if (pCustomWeight)
				{
				pEntry->sAttrib = CString(pStart, pCustomWeight - pStart);

				int iWeight = strParseInt(pCustomWeight + 1, 0);
				if (iMatchWeight >= 0)
					{
					if (iWeight > 0)
						pEntry->dwMatchStrength = CODE_INCREASE_IF | (iWeight & VALUE_MASK);
					else
						pEntry->dwMatchStrength = CODE_DECREASE_IF | (-iWeight & VALUE_MASK);
					}
				else
					{
					if (iWeight > 0)
						pEntry->dwMatchStrength = CODE_INCREASE_UNLESS | (iWeight & VALUE_MASK);
					else
						pEntry->dwMatchStrength = CODE_DECREASE_UNLESS | (-iWeight & VALUE_MASK);
					}
				}

			//	Otherwise, we just take match weight

			else
				{
				pEntry->sAttrib = CString(pStart, pPos - pStart);

				switch (iMatchWeight)
					{
					case -4:
						pEntry->dwMatchStrength = matchExcluded;
						break;

					case -3:
						pEntry->dwMatchStrength = matchAvoid3;
						break;

					case -2:
						pEntry->dwMatchStrength = matchAvoid2;
						break;

					case -1:
						pEntry->dwMatchStrength = matchAvoid1;
						break;

					case 1:
						pEntry->dwMatchStrength = matchSeek1;
						break;

					case 2:
						pEntry->dwMatchStrength = matchSeek2;
						break;

					case 3:
						pEntry->dwMatchStrength = matchSeek3;
						break;

					case 4:
						pEntry->dwMatchStrength = matchRequired;
						break;

					default:
						pEntry->dwMatchStrength = 0;
						break;
					}
				}
			}
		else
			pPos++;
		}

	//	Recurse, if necessary

	if (*pPos == '|')
		{
		pPos++;
		m_pOr.Set(new CAffinityCriteria);
		return m_pOr->ParseSubExpression(pPos, retsError);
		}

	return NOERROR;
	}

void CAffinityCriteria::WriteSubExpression (CMemoryWriteStream &Stream) const

//	WriteSubExpression
//
//	Writes out to a string.

	{
	//	If no attributes then we have special syntax

	if (m_Attribs.GetCount() == 0)
		{
		if (MatchesDefault())
			Stream.Write(MATCH_DEFAULT);
		else
			Stream.Write(MATCH_ALL);
		}

	else
		{
		for (int i = 0; i < m_Attribs.GetCount(); i++)
			{
			const SEntry &Entry = m_Attribs[i];

			if (i != 0)
				Stream.WriteChar(' ');

			switch (Entry.dwMatchStrength)
				{
				case matchRequired:
					Stream.Write(strPatternSubst(CONSTLIT("*%s;"), Entry.sAttrib));
					break;

				case matchSeek1:
					Stream.Write(strPatternSubst(CONSTLIT("+%s;"), Entry.sAttrib));
					break;

				case matchSeek2:
					Stream.Write(strPatternSubst(CONSTLIT("++%s;"), Entry.sAttrib));
					break;

				case matchSeek3:
					Stream.Write(strPatternSubst(CONSTLIT("+++%s;"), Entry.sAttrib));
					break;

				case matchAvoid1:
					Stream.Write(strPatternSubst(CONSTLIT("-%s;"), Entry.sAttrib));
					break;

				case matchAvoid2:
					Stream.Write(strPatternSubst(CONSTLIT("--%s;"), Entry.sAttrib));
					break;

				case matchAvoid3:
					Stream.Write(strPatternSubst(CONSTLIT("---%s;"), Entry.sAttrib));
					break;

				case matchExcluded:
					Stream.Write(strPatternSubst(CONSTLIT("!%s;"), Entry.sAttrib));
					break;

				default:
					{
					DWORD dwCode = (Entry.dwMatchStrength & CODE_MASK);
					DWORD dwValue = (Entry.dwMatchStrength & VALUE_MASK);

					switch (dwCode)
						{
						case CODE_INCREASE_IF:
							Stream.Write(strPatternSubst(CONSTLIT("+%s=+%d;"), Entry.sAttrib, dwValue));
							break;

						case CODE_DECREASE_IF:
							Stream.Write(strPatternSubst(CONSTLIT("+%s=-%d;"), Entry.sAttrib, dwValue));
							break;

						case CODE_INCREASE_UNLESS:
							Stream.Write(strPatternSubst(CONSTLIT("-%s=+%d;"), Entry.sAttrib, dwValue));
							break;

						case CODE_DECREASE_UNLESS:
							Stream.Write(strPatternSubst(CONSTLIT("-%s=-%d;"), Entry.sAttrib, dwValue));
							break;
						}
					break;
					}
				}
			}
		}

	//	Recurse, if necessary

	if (m_pOr)
		{
		Stream.Write(CONSTLIT(" | "));
		m_pOr->WriteSubExpression(Stream);
		}
	}
