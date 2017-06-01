//	CShipChallengeDesc.cpp
//
//	CShipChallengeDesc class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

static TStaticStringTable<TStaticStringEntry<CShipChallengeDesc::ECountTypes>, 4> CHALLENGE_TYPE_TABLE = {
	"deadly",				CShipChallengeDesc::countChallengeDeadly,
	"easy",					CShipChallengeDesc::countChallengeEasy,
	"hard",					CShipChallengeDesc::countChallengeHard,
	"standard",				CShipChallengeDesc::countChallengeStandard,
	};

Metric CShipChallengeDesc::CalcChallengeStrength (ECountTypes iType, int iLevel)

//	CalcChallengeStrength
//
//	Computes the desired combat strength.

	{
	Metric rCombat = CShipClass::GetStdCombatStrength(iLevel);
	switch (iType)
		{
		case countChallengeEasy:
			return 1.5 * rCombat;

		case countChallengeStandard:
			return 2.5 * rCombat;

		case countChallengeHard:
			return 4.0 * rCombat;

		case countChallengeDeadly:
			return 6.0 * rCombat;

		default:
			return 0.0;
		}
	}

bool CShipChallengeDesc::Init (ECountTypes iType, int iCount)

//	Init
//
//	Initialize

	{
	switch (iType)
		{
		case countReinforcements:
		case countScore:
		case countStanding:
			m_iType = iType;
			m_Count.SetConstant(iCount);
			break;

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
	switch (iType)
		{
		case countReinforcements:
		case countScore:
		case countStanding:
			m_iType = iType;
			if (m_Count.LoadFromXML(sCount) != NOERROR)
				return false;
			break;

		default:
			m_iType = iType;
			break;
		}

	return true;
	}

bool CShipChallengeDesc::InitFromChallengeRating (const CString &sChallenge)

//	InitFromChallengeRating
//
//	If sChallenge is a challenge level, then we set m_iType to that. Otherwise,
//	if it is a number, we assume it is a value score.

	{
	//	See if it is a standard challenge level.

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

bool CShipChallengeDesc::NeedsMoreInitialShips (CSpaceObject *pBase, const CShipChallengeCtx &Ctx) const

//	NeedsMoreInitialShips
//
//	Returns TRUE if there are not enough ships in the context.

	{
	switch (m_iType)
		{
		case countNone:
		case countReinforcements:
			return (Ctx.GetTotalCount() == 0);

		case countStanding:
			return (Ctx.GetTotalCount() < m_Count.RollSeeded(pBase->GetDestiny()));

		case countScore:
			return (Ctx.GetTotalScore() < m_Count.RollSeeded(pBase->GetDestiny()));

		case countChallengeEasy:
		case countChallengeStandard:
		case countChallengeHard:
		case countChallengeDeadly:
			return (Ctx.GetTotalCombat() < CalcChallengeStrength(m_iType, pBase->GetSystem()->GetLevel()));

		default:
			ASSERT(false);
			return false;
		}
	}

bool CShipChallengeDesc::NeedsMoreReinforcements (CSpaceObject *pBase) const

//	NeedsMoreReinforcements
//
//	Returns TRUE if pBase needs more defenders, as specified by our challenge
//	rating.

	{
	DEBUG_TRY

	int i;

	//	If no set count, then we're done.

	if (m_iType == countNone)
		return false;

	//	Add up the number of defenders for this station (and the total score).

	int iCurCount = 0;
	int iCurScore = 0;
	Metric rCurCombat = 0.0;
	CSystem *pSystem = pBase->GetSystem();

	for (i = 0; i < pSystem->GetObjectCount(); i++)
		{
		CSpaceObject *pObj = pSystem->GetObject(i);

		if (pObj
				&& pObj->GetBase() == pBase
				&& pObj->GetCategory() == CSpaceObject::catShip
				&& !pObj->IsIntangible()
				&& pObj != pBase)
			{
			iCurCount++;
			iCurScore += pObj->GetScore();

			CShip *pShip = pObj->AsShip();
			rCurCombat += pShip->GetClass()->GetCombatStrength();
			}
		}

	//	Now figure out if we have enough of them.

	switch (m_iType)
		{
		case countReinforcements:
		case countStanding:
			return (iCurCount < m_Count.RollSeeded(pBase->GetDestiny()));

		case countScore:
			return (iCurScore < m_Count.RollSeeded(pBase->GetDestiny()));

		case countChallengeEasy:
		case countChallengeStandard:
		case countChallengeHard:
		case countChallengeDeadly:
			return (rCurCombat < CalcChallengeStrength(m_iType, pSystem->GetLevel()));
			
		default:
			ASSERT(false);
			return false;
		}

	DEBUG_CATCH
	}

CShipChallengeDesc::ECountTypes CShipChallengeDesc::ParseChallengeType (const CString &sValue)

//	ParseChallengeType
//
//	Parses a string

	{
	const TStaticStringEntry<CShipChallengeDesc::ECountTypes> *pEntry = CHALLENGE_TYPE_TABLE.GetAt(sValue);
	if (pEntry == NULL)
		return countNone;

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

void CShipChallengeCtx::AddShips (CSpaceObjectList &List)

//	AddShips
//
//	Adds ships to the total

	{
	int i;

	for (i = 0; i < List.GetCount(); i++)
		AddShip(List.GetObj(i));
	}
