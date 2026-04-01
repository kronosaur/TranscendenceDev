//	CShipChallengeDesc.cpp
//
//	CShipChallengeDesc class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define TYPE_AUTO								CONSTLIT("auto")
#define TYPE_CHALLENGE							CONSTLIT("challenge")
#define TYPE_NONE								CONSTLIT("none")
#define TYPE_PROPERTY							CONSTLIT("property")

static TStaticStringTable<TStaticStringEntry<CShipChallengeDesc::ECountTypes>, 4> CHALLENGE_TYPE_TABLE = {
	//NOTE: keys must be alphabetical
	"deadly",				CShipChallengeDesc::countChallengeDeadly,
	"easy",					CShipChallengeDesc::countChallengeEasy,
	"hard",					CShipChallengeDesc::countChallengeHard,
	"standard",				CShipChallengeDesc::countChallengeStandard,
	};

Metric CShipChallengeDesc::CalcMinChallengeStrength (ECountTypes iType, int iLevel) const

//	CalcChallengeStrength
//
//	Computes the min desired combat strength
//
	{
	Metric rCombat = CShipClass::GetStdCombatStrength(iLevel);
	switch (iType)
		{
		case countChallengeEasy:
			return 1.0 * rCombat;

		case countChallengeStandard:
			return 2.0 * rCombat;

		case countChallengeHard:
			return 3.0 * rCombat;

		case countChallengeDeadly:
			return 5.0 * rCombat;

		case countChallengeRange:
			return m_rLower * rCombat;

		default:
			return 0.0;
		}
	}

//	CalcChallengeStrength
//
//	Computes the max allowed combat strength
//
Metric CShipChallengeDesc::CalcMaxChallengeStrength (ECountTypes iType, int iLevel) const

	{
	Metric rCombat = CShipClass::GetStdCombatStrength(iLevel);
	switch (iType)
		{
		case countChallengeEasy:
			return 2.0 * rCombat;

		case countChallengeStandard:
			return 3.0 * rCombat;

		case countChallengeHard:
			return 5.0 * rCombat;

		case countChallengeDeadly:
			return 7.0 * rCombat;

		case countChallengeRange:
			return m_rUpper * rCombat;

		default:
			return 0.0;
		}
	}

Metric CShipChallengeDesc::CalcChallengeStrength (ECountTypes iType, int iLevel) const

//	CalcChallengeStrength
//
//	Computes the median desired combat strength.
//
	{
	Metric rMin = CalcMinChallengeStrength(iType, iLevel);
	Metric rMax = CalcMaxChallengeStrength(iType, iLevel);
	return (rMax - rMin) / 2 + rMin;
	}

ICCItemPtr CShipChallengeDesc::GetDesc (const CSpaceObject *pBase) const

//	GetDesc
//
//	Returns a descriptor (usually for debugging).

	{
	//	Add type

	CString sType;
	switch (m_iType)
		{
		case countNone:
			sType = CONSTLIT("none");
			break;

		case countAuto:
			sType = CONSTLIT("auto");
			break;

		case countProperty:
			sType = CONSTLIT("property");
			break;
			
		case countScore:
			sType = CONSTLIT("score");
			break;
			
		case countShips:
			sType = CONSTLIT("ships");
			break;

		case countOnce:
			sType = CONSTLIT("once");
			break;
			
		case countChallengeEasy:
			sType = CONSTLIT("challengeEasy");
			break;
			
		case countChallengeStandard:
			sType = CONSTLIT("challengeStandard");
			break;
			
		case countChallengeHard:
			sType = CONSTLIT("challengeHard");
			break;
			
		case countChallengeDeadly:
			sType = CONSTLIT("challengeDeadly");
			break;

		case countChallengeRange:
			sType = CONSTLIT("challengeRange");
			break;

		default:
			sType = CONSTLIT("unknown");
			break;
		}

	//	Add count

	switch (m_iType)
		{
		case countScore:
			{
			ICCItemPtr pResult(ICCItem::SymbolTable);
			pResult->SetStringAt(CONSTLIT("type"), sType);

			int iCount;
			if (pBase)
				iCount = m_Count.RollSeeded(pBase->GetDestiny());
			else
				iCount = m_Count.GetAveValue();

			pResult->SetIntegerAt(CONSTLIT("score"), iCount);

			return pResult;
			}

		case countShips:
			{
			if (pBase)
				{
				int iCount = m_Count.RollSeeded(pBase->GetDestiny());
				return ICCItemPtr(iCount);
				}
			else
				return ICCItemPtr(m_Count.SaveToXML());
			}

		case countChallengeEasy:
		case countChallengeStandard:
		case countChallengeHard:
		case countChallengeDeadly:
			{
			if (pBase)
				{
				ICCItemPtr pResult(ICCItem::SymbolTable);
				pResult->SetStringAt(CONSTLIT("type"), sType);

				Metric rStrength = CalcChallengeStrength(m_iType, pBase->GetSystem()->GetLevel());
				pResult->SetDoubleAt(CONSTLIT("strength"), rStrength);

				rStrength = CalcMinChallengeStrength(m_iType, pBase->GetSystem()->GetLevel());
				pResult->SetDoubleAt(CONSTLIT("strengthMin"), rStrength);

				rStrength = CalcMaxChallengeStrength(m_iType, pBase->GetSystem()->GetLevel());
				pResult->SetDoubleAt(CONSTLIT("strengthMax"), rStrength);

				return pResult;
				}
			else
				return ICCItemPtr(sType);
			}

		case countProperty:
			return ICCItemPtr(strPatternSubst(CONSTLIT("property:%s"), m_sValue));
			break;

		default:
			return ICCItemPtr(sType);
		}
	}

bool CShipChallengeDesc::Init (ECountTypes iType, int iCount)

//	Init
//
//	Initialize

	{
	switch (iType)
		{
		case countScore:
		case countShips:
			m_iType = iType;
			m_Count.SetConstant(iCount);
			break;

		case countProperty:
			return false;

		default:
			m_iType = iType;
			break;
		}

	return true;
	}

bool CShipChallengeDesc::Init (ECountTypes iType, const CString &sCount)

//	Init
//
//	Initialize

	{
	m_iType = iType;

	switch (iType)
		{
		case countScore:
		case countShips:
			if (m_Count.LoadFromXML(sCount) != NOERROR)
				return false;
			break;

		case countProperty:
			return false;

		default:
			break;
		}

	return true;
	}

//	InitFromChallengeRating
//
//	If sChallenge is a challenge level, then we set m_iType to that. Otherwise,
//	if it is a single number, we assume it is a value score.
//
bool CShipChallengeDesc::InitFromChallengeRating (const CString &sChallenge)

	{
	//	See if it is a standard challenge level or a challenge range.

	ECountTypes iType = ParseChallengeType(sChallenge);
	if (iType != countNone)
		{
		m_iType = iType;
		return true;
		}

	//	Otherwise, we assume it is a score.

	m_iType = countScore;
	if (m_Count.LoadFromXML(sChallenge) != NOERROR)
		return false;

	return true;
	}

bool CShipChallengeDesc::InitFromXML (const CString &sValue)

//	InitFromXML
//
//	Initialize from an XML parameter value. We support the following values:
//
//	"none"
//	"auto"
//	"4"						A number
//	"1d4"					A dice range
//	"challenge:standard"	A challenge rating
//	"property:xyz"			A property

	{
	const char *pPos = sValue.GetASCIIZPointer();

	if (sValue.IsBlank())
		m_iType = countAuto;

	else if (strIsDigit(pPos))
		{
		m_iType = countShips;
		if (m_Count.LoadFromXML(sValue) != NOERROR)
			return false;
		}
	else
		{
		const char *pStart = pPos;
		while (*pPos != ':' && *pPos != '\0')
			pPos++;

		CString sType(pStart, pPos - pStart);
		if (*pPos == ':')
			pPos++;

		CString sParam(pPos);

		if (strEquals(sType, TYPE_AUTO))
			{
			m_iType = countAuto;
			}
		else if (strEquals(sType, TYPE_CHALLENGE))
			{
			if (!InitFromChallengeRating(sParam))
				return false;
			}
		else if (strEquals(sType, TYPE_NONE))
			{
			m_iType = countNone;
			}
		else if (strEquals(sType, TYPE_PROPERTY) && !sParam.IsBlank())
			{
			m_iType = countProperty;
			m_sValue = sParam;
			}
		else
			return false;
		}

	return true;
	}

//	NeedsMoreShips
//
//	Returns TRUE if there are not enough ships in the context.
//
bool CShipChallengeDesc::NeedsMoreShips (CSpaceObject &Base, const CShipChallengeCtx &Ctx) const

	{
	switch (m_iType)
		{
		case countNone:
			return false;

		case countAuto:
			return (Ctx.GetTotalCount() == 0);

		case countShips:
			return (Ctx.GetTotalCount() < m_Count.RollSeeded(Base.GetDestiny()));

		case countOnce:
			return (Ctx.GetTotalRolls() == 0);

		case countProperty:
			{
			ICCItemPtr pResult = Base.GetProperty(m_sValue);
			if (!pResult || pResult->IsNil())
				return false;
			else if (pResult->IsNumber())
				return (Ctx.GetTotalCount() < pResult->GetIntegerValue());
			else
				return (Ctx.GetTotalCount() == 0);
			}

		case countScore:
			return (Ctx.GetTotalScore() < m_Count.RollSeeded(Base.GetDestiny()));

		case countChallengeEasy:
		case countChallengeStandard:
		case countChallengeHard:
		case countChallengeDeadly:
		case countChallengeRange:
			return (Ctx.GetTotalCombat() < CalcMinChallengeStrength(m_iType, Base.GetSystem()->GetLevel()));

		default:
			ASSERT(false);
			return false;
		}
	}

//	CanHaveMoreShips
//
//	Returns a floating point representing how many more ships
//	a challenge-type desc is allowed to have relative to the size of
//  the specified challenge range.
//  1.0 means that the entire challenge range from min to max is available.
//  0.0 means that no more ships can fit in the challenge range.
//  A negative value means that the maximum value was exceeded.
// 
//	All other types return either 0.0 or 1.0 in alignment with NeedsMoreShips
//
Metric CShipChallengeDesc::CanHaveMoreShips (CSpaceObject& Base, const CShipChallengeCtx& Ctx) const
	{
	switch (m_iType)
		{
		case countChallengeEasy:
		case countChallengeStandard:
		case countChallengeHard:
		case countChallengeDeadly:
		case countChallengeRange:
			{
			Metric rMin = CalcMinChallengeStrength(m_iType, Base.GetSystem()->GetLevel());
			Metric rCombatStr = Ctx.GetTotalCombat();
			if (rMin > rCombatStr)
				return 1.0;
			Metric rMax = CalcMaxChallengeStrength(m_iType, Base.GetSystem()->GetLevel());
			return (rMax - rCombatStr) / (rMax - rMin);
			}

		default:
			return NeedsMoreShips(Base, Ctx) ? 1.0 : 0.0;
		}
	}

//	NeedsMoreReinforcements
//
//	Returns TRUE if pBase needs more defenders, as specified by our challenge
//	rating.
//
bool CShipChallengeDesc::NeedsMoreReinforcements (CSpaceObject &Base, const CSpaceObjectList &Current, const CShipChallengeDesc &Reinforce) const

	{
	DEBUG_TRY

	switch (Reinforce.m_iType)
		{
		case countNone:
			return false;

		case countAuto:
			{
			//	Can't both be auto

			if (m_iType == countAuto)
				return false;
			else
				{
				CShipChallengeCtx Ctx(Current);
				return NeedsMoreReinforcements(Base, Ctx, Reinforce);
				}
			}

		case countProperty:
		case countShips:
		case countScore:
		case countChallengeEasy:
		case countChallengeStandard:
		case countChallengeHard:
		case countChallengeDeadly:
		case countChallengeRange:
			{
			CShipChallengeCtx Ctx(Current);
			return NeedsMoreReinforcements(Base, Ctx, Reinforce);
			}

		default:
			throw CException(ERR_FAIL);
		}

	DEBUG_CATCH
	}

//	NeedsMoreReinforcements
//
//	Returns TRUE if we need more reinforcements.
//
bool CShipChallengeDesc::NeedsMoreReinforcements (CSpaceObject &Base, const CShipChallengeCtx &Ctx, const CShipChallengeDesc &Reinforce) const

	{
	switch (Reinforce.m_iType)
		{
		case countNone:
			return false;

		case countAuto:
			{
			//	Can't both be auto

			if (m_iType == countAuto)
				return false;
			else
				{
				return NeedsMoreShips(Base, Ctx);
				}
			}

		case countProperty:
			{
			ICCItemPtr pResult = Base.GetProperty(Reinforce.m_sValue);

			//	If the value of the property is Nil, then we never reinforce
			//	(treat it as none).

			if (!pResult || pResult->IsNil())
				return false;

			//	If we return a number, then we assume this is the minimum number
			//	of ships that we want.

			else if (pResult->IsNumber())
				return (Ctx.GetTotalCount() < pResult->GetIntegerValue());

			//	Otherwise, we treat it as auto.

			else
				return NeedsMoreShips(Base, Ctx);
			}

		case countShips:
		case countScore:
		case countChallengeEasy:
		case countChallengeStandard:
		case countChallengeHard:
		case countChallengeDeadly:
		case countChallengeRange:
			{
			if (Reinforce.NeedsMoreShips(Base, Ctx))
				return true;
			else if (m_iType != countAuto && NeedsMoreShips(Base, Ctx))
				return true;
			else
				return false;
			}

		default:
			throw CException(ERR_FAIL);
		}
	}

//	ParseChallengeType
//
//	Parses a string
//
CShipChallengeDesc::ECountTypes CShipChallengeDesc::ParseChallengeType (const CString &sValue)

	{
	const TStaticStringEntry<CShipChallengeDesc::ECountTypes> *pEntry = CHALLENGE_TYPE_TABLE.GetAt(sValue);

	if (pEntry == NULL)
		{
		//	Check if we have a valid range instead
		//	Values must be 2 positive floats separated by a -
		if (int iDelimPos = strFind(sValue, CONSTLIT("-")) > 0)
			{
			bool bFailed = false;
			//	Try to parse the count and set our min and max
			CString sLower = strSlice(sValue, 0, iDelimPos - 1);
			CString sUpper = strSlice(sValue, iDelimPos + 1);
			m_rLower = strToDouble(sLower, 0.0, &bFailed);
			if (bFailed)
				return countNone;
			m_rUpper = strToDouble(sUpper, 0.0, &bFailed);
			if (bFailed)
				{
				m_rLower = 0.0;
				return countNone;
				}
			if (m_rLower < 0.0 || m_rUpper < m_rLower || m_rUpper < 0.0)
				{
				m_rLower = 0.0;
				m_rUpper = 0.0;
				return countNone;
				}
			return countChallengeRange;
			}
		else
			return countNone;
		}

	return pEntry->Value;
	}

//	CShipChallengeCtx ----------------------------------------------------------

void CShipChallengeCtx::AddShip (CSpaceObject *pObj)

//	AddShip
//
//	Adds a ship to the total

	{
	CShip *pShip = pObj->AsShip();
	if (pShip == NULL)
		return;

	m_iTotalCount++;
	m_iTotalScore += pShip->GetScore();
	m_rTotalCombat += pShip->GetClass()->GetCombatStrength();
	}

void CShipChallengeCtx::AddShips (const CSpaceObjectList &List)

//	AddShips
//
//	Adds ships to the total

	{
	for (int i = 0; i < List.GetCount(); i++)
		AddShip(List.GetObj(i));

	//	Increment roll count

	m_iTotalRolls++;
	}
