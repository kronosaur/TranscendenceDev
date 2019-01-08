//	CArtifactProgram.cpp
//
//	CArtifactProgram class
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define EVENT_GET_ARTIFACT_PROGRAM_INFO		CONSTLIT("GetArtifactProgramInfo")

#define FIELD_CRITERIA						CONSTLIT("criteria")
#define FIELD_DEFENSE						CONSTLIT("defense")
#define FIELD_POWERS						CONSTLIT("powers")
#define FIELD_STAT							CONSTLIT("stat")
#define FIELD_STRENGTH						CONSTLIT("strength")
#define FIELD_TYPE							CONSTLIT("type")
#define FIELD_VALUE							CONSTLIT("value")

#define STAT_EGO							CONSTLIT("ego")
#define STAT_INTELLIGENCE					CONSTLIT("intelligence")
#define STAT_WILLPOWER						CONSTLIT("willpower")

#define TYPE_HALT							CONSTLIT("halt")
#define TYPE_LOCUS_BLOCK					CONSTLIT("locusBlock")
#define TYPE_PATCH_DEFENSE					CONSTLIT("patchDefense")
#define TYPE_PATCH_STRENGTH					CONSTLIT("patchStrength")
#define TYPE_TARGET_STAT					CONSTLIT("targetStat")
#define TYPE_WIPE							CONSTLIT("wipe")

#define ERR_MISSING_ARTIFACT_INFO			CONSTLIT("Unable to find <GetArtifactProgramInfo> event for item type: %08x.")
#define ERR_UNKNOWN_POWER_TYPE				CONSTLIT("Unknown power type: %s.")
#define ERR_UNKNOWN_STAT					CONSTLIT("Unknown stat: %s.")

bool CArtifactProgram::Init (EProgramTypes iType, int iLocus, CItemType *pItem, CString *retsError)

//	Init
//
//	Initializes the given program from the item.
//
//	<GetArtifactProgramInfo> should return a struct with the following fields:
//
//	strength: The compute power of the program
//	defense: The fault tolerance (resistance to counters)
//	powers: An array of structs, each struct has the following fields:
//
//		type: The power type:
//			halt: Halt attack on opposing program matching criteria
//			locusBlock: Eliminates one of the opponent's empty loci
//			patchDefense: Changes defense of friendly program matching criteria
//			patchStrength: Changes strength of friendly program matching criteria
//			targetStat: Change stat by program strength
//			wipe: Wipe attack on opposing program matching criteria
//
//		criteria: A criteria string selecting some set of programs
//		stat: One of "ego", "intelligence", or "willpower".
//		value: An integer used by some power types.

	{
	int i;

	ASSERT(pItem);

	m_iType = iType;
	m_iStatus = (iType == typeDaimon ? statusRunning : statusArchived);
	m_iLocusPos = iLocus;
	m_pItem = pItem;
	m_sBaseAttributes = pItem->GetAttributes();
	m_sAttributes = m_sBaseAttributes;

	//	Get the event handler that returns program information

	SEventHandlerDesc Event;
	if (!pItem->FindEventHandler(EVENT_GET_ARTIFACT_PROGRAM_INFO, &Event))
		{
		if (retsError) *retsError = strPatternSubst(ERR_MISSING_ARTIFACT_INFO, pItem->GetUNID());
		return false;
		}

	//	Invoke handler and get a descriptor

	CCodeChainCtx Ctx;

	Ctx.SetItemType(pItem);
	Ctx.SaveAndDefineSourceVar(NULL);
	Ctx.SaveAndDefineItemVar(CItem(pItem, 1));

	ICCItem *pResult = Ctx.Run(Event);

	//	We expect the result to be a struct with various fields (some optional).

	m_iStrength = CArtifactStat(pResult->GetIntegerAt(FIELD_STRENGTH));
	m_iDefense = CArtifactStat(pResult->GetIntegerAt(FIELD_DEFENSE));

	ICCItem *pPowers = pResult->GetElement(FIELD_POWERS);
	if (pPowers)
		{
		for (i = 0; i < pPowers->GetCount(); i++)
			{
			ICCItem *pDef = pPowers->GetElement(i);
			SEffectDesc *pNewEffect = m_Effects.Insert();

			CString sType = pDef->GetStringAt(FIELD_TYPE);
			if (strEquals(sType, TYPE_HALT))
				pNewEffect->iType = effectHalt;
			else if (strEquals(sType, TYPE_LOCUS_BLOCK))
				pNewEffect->iType = effectLocusBlock;
			else if (strEquals(sType, TYPE_PATCH_DEFENSE))
				pNewEffect->iType = effectPatchDefense;
			else if (strEquals(sType, TYPE_PATCH_STRENGTH))
				pNewEffect->iType = effectPatchStrength;
			else if (strEquals(sType, TYPE_TARGET_STAT))
				pNewEffect->iType = effectTargetStat;
			else if (strEquals(sType, TYPE_WIPE))
				pNewEffect->iType = effectWipe;
			else
				{
				delete pNewEffect;
				if (retsError) *retsError = strPatternSubst(ERR_UNKNOWN_POWER_TYPE, sType);
				return false;
				}

			CString sCriteria = pDef->GetStringAt(FIELD_CRITERIA);
			if (!sCriteria.IsBlank())
				{
				if (!ParseCriteria(sCriteria, pNewEffect->Criteria, retsError))
					return false;
				}

			CString sStat = pDef->GetStringAt(FIELD_STAT);
			if (sStat.IsBlank())
				pNewEffect->iStat = CArtifactStat::statNone;
			else if (strEquals(sStat, STAT_EGO))
				pNewEffect->iStat = CArtifactStat::statEgo;
			else if (strEquals(sStat, STAT_INTELLIGENCE))
				pNewEffect->iStat = CArtifactStat::statIntelligence;
			else if (strEquals(sStat, STAT_WILLPOWER))
				pNewEffect->iStat = CArtifactStat::statWillpower;
			else
				{
				delete pNewEffect;
				if (retsError) *retsError = strPatternSubst(ERR_UNKNOWN_STAT, sStat);
				return false;
				}

			pNewEffect->iValue = pDef->GetIntegerAt(FIELD_VALUE);
			}
		}

	//	Done

	Ctx.Discard(pResult);

	return true;
	}

bool CArtifactProgram::MatchesCriteria (const SCriteria &Criteria, DWORD dwFlags) const

//	MatchesCriteria
//
//	Returns TRUE if we matche the criteria

	{
	int i;

	//	Flags

	bool bActiveOnly = ((dwFlags & FLAG_ACTIVE_ONLY) == FLAG_ACTIVE_ONLY);
	bool bRunningLastTurn = ((dwFlags & FLAG_RUNNING_LAST_TURN) == FLAG_RUNNING_LAST_TURN);

	//	Check flags

	if (bActiveOnly && !IsActive())
		return false;

	if (bRunningLastTurn && WasHaltedLastTurn())
		return false;

	//	Attributes

	for (i = 0; i < Criteria.ModifiersRequired.GetCount(); i++)
		if (!::HasModifier(m_sAttributes, Criteria.ModifiersRequired[i]))
			return false;

	for (i = 0; i < Criteria.ModifiersNotAllowed.GetCount(); i++)
		if (::HasModifier(m_sAttributes, Criteria.ModifiersNotAllowed[i]))
			return false;

	return true;
	}

bool CArtifactProgram::ParseCriteria (const CString &sCriteria, SCriteria &Criteria, CString *retsError)

//	ParseCriteria
//
//	Parses a program criteria string.

	{
	//	Reset

	Criteria.ModifiersNotAllowed.DeleteAll();
	Criteria.ModifiersRequired.DeleteAll();

	//	Parse

	char *pPos = sCriteria.GetASCIIZPointer();
	while (*pPos != '\0')
		{
		switch (*pPos)
			{
			case '+':
			case '-':
				{
				bool bSpecialAttrib = false;
				char chChar = *pPos;
				pPos++;

				//	Get the modifier

				char *pStart = pPos;
				while (*pPos != '\0' && *pPos != ';' && *pPos != ' ' && *pPos != '\t')
					{
					if (*pPos == ':')
						bSpecialAttrib = true;
					pPos++;
					}

				CString sModifier = CString(pStart, pPos - pStart);

				//	Required or Not Allowed

				if (chChar == '+')
					{
#if 0
					if (bSpecialAttrib)
						retCriteria->SpecialAttribRequired.Insert(sModifier);
					else
#endif
						Criteria.ModifiersRequired.Insert(sModifier);
					}
				else
					{
#if 0
					if (bSpecialAttrib)
						retCriteria->SpecialAttribNotAllowed.Insert(sModifier);
					else
#endif
						Criteria.ModifiersNotAllowed.Insert(sModifier);
					}

				//	No trailing semi

				if (*pPos == '\0')
					pPos--;

				break;
				}
			}

		pPos++;
		}

	//	Done

	return true;
	}

void CArtifactProgram::ResetStats (void)

//	ResetStats
//
//	Resets stats to original at the start of each turn.

	{
	//	Skip programs that are dead

	if (!IsLive())
		return;

	//	Reset basic properties

	m_sAttributes = m_sBaseAttributes;
	m_iDefense.StartTurn();
	m_iStrength.StartTurn();

	//	Reset success flag

	m_bLastAttackSuccessful = false;

	//	If halted, set to running (but remember that we were halted).

	if (m_bHaltedLastTurn = (m_iStatus == statusHalted))
		m_iStatus = statusRunning;
	}
