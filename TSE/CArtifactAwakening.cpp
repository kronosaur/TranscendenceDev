//	CArtifactAwakening.cpp
//
//	CArtifactAwakening class
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define ERR_TOO_MANY_COUNTERMEASURES		CONSTLIT("Cannot deploy more than 6 countermeasures.")
#define ERR_INVALID_STAT					CONSTLIT("Stats must be from 1 to 24.")
#define ERR_NO_ROOM_FOR_DAIMON				CONSTLIT("All daimon loci are already filled.")
#define ERR_NOT_IN_BATTLE					CONSTLIT("Battle has already ended.")

const int MAX_COUNTERMEASURE_COUNT =		6;
const int MAX_DAIMON_COUNT =				6;

CArtifactAwakening::CArtifactAwakening (void) :
		m_iState(stateStart),
		m_iTurn(0)

//	CArtifactAwakening constructor

	{
	}

void CArtifactAwakening::AddEventResult (EEventTypes iEvent, CArtifactProgram *pSource, CArtifactProgram *pTarget, TArray<SEventDesc> &Results)

//	AddEventResult
//
//	Adds an event result, making sure to check the history to see if it is the
//	first time.

	{
	int i;

	SEventDesc *pEvent = Results.Insert();
	pEvent->iEvent = iEvent;
	pEvent->pSource = pSource;
	pEvent->pTarget = pTarget;

	//	Look for this event in the previous event list and set the flag to indicate
	//	that we've already reported it.

	for (i = 0; i < m_LastResults.GetCount(); i++)
		{
		if (pEvent->iEvent == m_LastResults[i].iEvent
				&& pEvent->pSource == m_LastResults[i].pSource
				&& pEvent->pTarget == m_LastResults[i].pTarget)
			{
			pEvent->bAlreadyReported = true;
			break;
			}
		}
	}

int CArtifactAwakening::CalcCountermeasureScore (CArtifactProgram &Program)

//	CalcCountermeasureScore
//
//	Returns the score for activating this countermeasure.

	{
	int i, j;
	int iScore = 0;

	//	Loop over all our effects computing a score

	for (i = 0; i < Program.GetEffects().GetCount(); i++)
		{
		const CArtifactProgram::SEffectDesc &Effect = Program.GetEffects()[i];

		switch (Effect.iType)
			{
			case CArtifactProgram::effectHalt:
			case CArtifactProgram::effectWipe:
				{
				//	Loop over all non-halted daimons and see if we can halt one. If so, our
				//	score is inversely proportional to our power (we want to activate the
				//	weakest countermeasure that can halt the daimon).

				for (j = 0; j < m_Daimons.GetCount(); j++)
					{
					CArtifactProgram *pDaimon = m_Daimons[j];
					if (pDaimon == NULL || pDaimon->WasHaltedLastTurn() || !pDaimon->IsActive())
						continue;

					//	See if we can affect this daimon

					if (!pDaimon->MatchesCriteria(Effect.Criteria) 
							|| !CanAffectTarget(Program, Effect.iType, *pDaimon))
						continue;
			
					//	Score

					iScore += 300 - (10 * Program.GetStrength());
					break;
					}

				break;
				}

			default:
				iScore += 5 * Program.GetStrength();
				break;
			}
		}


	return iScore;
	}

CArtifactProgram *CArtifactAwakening::CalcCountermeasureToActivate (void)

//	CalcCountermeasureToActivate
//
//	Loop over all archived countermeasures and pick the best one to activate.

	{
	int i;

	int iBestScore = 0;
	CArtifactProgram *pBestCountermeasure = NULL;
	for (i = 0; i < m_Countermeasures.GetCount(); i++)
		{
		CArtifactProgram *pCountermeasure = m_Countermeasures[i];
		if (pCountermeasure == NULL || !pCountermeasure->IsArchived())
			continue;

		int iScore = CalcCountermeasureScore(*pCountermeasure);
		if (iScore > iBestScore)
			{
			iBestScore = iScore;
			pBestCountermeasure = pCountermeasure;
			}
		}

	return pBestCountermeasure;
	}

int CArtifactAwakening::CalcFreeDaimonLoci (void) const

//	CalcFreeDaimonLoci
//
//	Calculates and returns the number of open loci for daimons.

	{
	int i;

	int iCount = 0;
	for (i = 0; i < m_Daimons.GetCount(); i++)
		if (m_Daimons[i] == NULL)
			iCount++;

	return iCount;
	}

TArray<CArtifactProgram *> CArtifactAwakening::CalcMatchingPrograms (const TArray<CArtifactProgram *> &Targets, const CArtifactProgram::SCriteria &Criteria, DWORD dwFlags) const

//	CalcMatchingPrograms
//
//	Returns a list of program entries that match the criteria.

	{
	int i;

	TArray<CArtifactProgram *> Result;
	Result.GrowToFit(Targets.GetCount());

	//	Match criteria

	for (i = 0; i < Targets.GetCount(); i++)
		{
		//	Targets is generally a list of loci, so sometimes they are empty

		if (Targets[i] == NULL)
			continue;

		//	Check criteria

		if (!Targets[i]->MatchesCriteria(Criteria, dwFlags))
			continue;

		//	Add to list

		Result.Insert(Targets[i]);
		}

	//	Done

	return Result;
	}

int CArtifactAwakening::CalcNextDaimonPos (void) const

//	CalcNextDaimonPos
//
//	Returns the next free position for a daimon.

	{
	int i;

	for (i = 0; i < m_Daimons.GetCount(); i++)
		if (m_Daimons[i] == NULL)
			return i;

	return -1;
	}

CArtifactProgram *CArtifactAwakening::CalcProgramTarget (CArtifactProgram &Program, CArtifactProgram::EEffectTypes iEffect, const TArray<CArtifactProgram *> &Targets) const

//	CalcProgramTarget
//
//	Returns the most appropriate target in the target list for the given effect.
//	This function is deterministic for a given effect and target list.
//
//	We return NULL if none of the programs in the target list are appropriate.

	{
	int i;

	//	Pick the most appropriate program to target.

	int iBestScore = 0;
	CArtifactProgram *pBestProgram = NULL;

	//	Loop over all targets and calculate their score

	for (i = 0; i < Targets.GetCount(); i++)
		{
		CArtifactProgram &Target = *Targets[i];

		int iScore = CalcTargetScore(Program, iEffect, Target);
		if (iScore > iBestScore)
			{
			iBestScore = iScore;
			pBestProgram = &Target;
			}
		}

	//	Done

	return pBestProgram;
	}

int CArtifactAwakening::CalcTargetScore (CArtifactProgram &Program, CArtifactProgram::EEffectTypes iEffect, CArtifactProgram &Target) const

//	CalcTargetScore
//
//	Computes the likelihood that the given program should be a target for the
//	given effect.

	{
	int i;
	int iScore = 0;

	//	If the program cannot effect the given target, then we return 0.

	if (!CanAffectTarget(Program, iEffect, Target))
		return 0;

	//	Loop over all target effects (to see how dangerous it is to us).

	for (i = 0; i < Target.GetEffects().GetCount(); i++)
		{
		const CArtifactProgram::SEffectDesc &TargetEffect = Target.GetEffects().GetAt(i);

		switch (TargetEffect.iType)
			{
			case CArtifactProgram::effectTargetStat:
				iScore += 10 * Program.GetStrength();
				break;

			case CArtifactProgram::effectHalt:
			case CArtifactProgram::effectWipe:
			case CArtifactProgram::effectLocusBlock:
				iScore += 10;
				break;

			default:
				iScore += 1;
				break;
			}
		}

	//	Done

	return iScore;
	}

bool CArtifactAwakening::CanAffectTarget (CArtifactProgram &Program, CArtifactProgram::EEffectTypes iEffect, CArtifactProgram &Target) const

//	CanAffectTarget
//
//	Returns TRUE if we can affect the given target.
//	NOTE: We assume that we've already checked the criteria requirements.

	{
	switch (iEffect)
		{
		case CArtifactProgram::effectHalt:
		case CArtifactProgram::effectWipe:
			return (Program.GetStrength() >= Target.GetDefense());

		default:
			return true;
		}
	}

void CArtifactAwakening::CleanUp (void)

//	CleanUp
//
//	Clean up all allocations

	{
	int i;

	for (i = 0; i < m_InPlay.GetCount(); i++)
		delete m_InPlay[i];

	m_InPlay.DeleteAll();

	m_InitialDaimons.DeleteAll();
	m_Countermeasures.DeleteAll();
	m_Daimons.DeleteAll();
	}

bool CArtifactAwakening::DeployDaimon (CItemType *pType, CArtifactProgram **retpNewDaimon, CString *retsError)

//	DeployDaimon
//
//	Deploys a daimon.

	{
	//	Make sure we're in a valid state

	if (m_iState == stateStart)
		m_iState = stateInBattle;
	else if (m_iState != stateInBattle)
		{
		if (retsError) *retsError = ERR_NOT_IN_BATTLE;
		return false;
		}

	//	Make sure we have room

	int iPos = CalcNextDaimonPos();
	if (iPos == -1)
		{
		if (retsError) *retsError = ERR_NO_ROOM_FOR_DAIMON;
		return false;
		}

	//	Add to in play list

	CArtifactProgram *pNewProgram = new CArtifactProgram;
	if (!pNewProgram->Init(CArtifactProgram::typeDaimon, iPos, pType, retsError))
		{
		delete pNewProgram;
		return false;
		}

	//	Deploy

	pNewProgram->SetTurnDeployed(m_iTurn);
	m_InPlay.Insert(pNewProgram);

	//	Add to locus

	m_Daimons[iPos] = pNewProgram;
	if (retpNewDaimon)
		*retpNewDaimon = pNewProgram;

	return true;
	}

CArtifactProgram *CArtifactAwakening::GetLocusProgram (CArtifactProgram::EProgramTypes iType, int iIndex) const

//	GetLocusProgram
//
//	Returns the program at the given locus (return NULL if there is not program
//	there).

	{
	switch (iType)
		{
		case CArtifactProgram::typeCountermeasure:
			return ((iIndex >= 0 && iIndex < m_Countermeasures.GetCount()) ? m_Countermeasures[iIndex] : NULL);

		case CArtifactProgram::typeDaimon:
			return ((iIndex >= 0 && iIndex < m_Daimons.GetCount()) ? m_Daimons[iIndex] : NULL);

		default:
			return NULL;
		}
	}

CArtifactAwakening::EResultTypes CArtifactAwakening::GetStatus (void) const

//	GetStatus
//
//	Returns the current status

	{
	switch (m_iState)
		{
		case stateStart:
			return resultNone;

		case stateSuccess:
			return resultArtifactSubdued;

		case stateFailure:
			return resultPlayerFailed;

		default:
			return resultBattleContinues;
		}
	}

void CArtifactAwakening::IncDefense (const TArray<CArtifactProgram *> &Programs, int iValue)

//	IncDefense
//
//	Increments the defese value for the given programs.

	{
	int i;

	for (i = 0; i < Programs.GetCount(); i++)
		Programs[i]->IncDefense(iValue);
	}

void CArtifactAwakening::IncStrength (const TArray<CArtifactProgram *> &Programs, int iValue)

//	IncStrength
//
//	Increments the strength value for the given programs.

	{
	int i;

	for (i = 0; i < Programs.GetCount(); i++)
		Programs[i]->IncStrength(iValue);
	}

bool CArtifactAwakening::Init (const SCreateDesc &Desc, CString *retsError)

//	Init
//
//	Initializes the Core.

	{
	int i;

	CleanUp();

	//	Can't exceed the max number of countermeasures

	if (Desc.Countermeasures.GetCount() > MAX_COUNTERMEASURE_COUNT)
		{
		if (retsError) *retsError = ERR_TOO_MANY_COUNTERMEASURES;
		return false;
		}

	//	Initialize the locus arrays

	m_Countermeasures.InsertEmpty(MAX_COUNTERMEASURE_COUNT);
	for (i = 0; i < m_Countermeasures.GetCount(); i++)
		m_Countermeasures[i] = NULL;

	m_Daimons.InsertEmpty(MAX_DAIMON_COUNT);
	for (i = 0; i < m_Daimons.GetCount(); i++)
		m_Daimons[i] = NULL;

	//	Initialize stats

	for (i = 0; i < CArtifactStat::statCount; i++)
		{
		if (Desc.Stat[i] <= 0 || Desc.Stat[i] > CArtifactStat::statMax)
			{
			if (retsError) *retsError = ERR_INVALID_STAT;
			return false;
			}

		m_Stat[i] = CArtifactStat(Desc.Stat[i], 0, CArtifactStat::statMax);
		}

	//	Initialize countermeasures

	for (i = 0; i < Desc.Countermeasures.GetCount(); i++)
		{
		CArtifactProgram *pNewProgram = new CArtifactProgram;
		if (!pNewProgram->Init(CArtifactProgram::typeCountermeasure, i, Desc.Countermeasures[i], retsError))
			{
			delete pNewProgram;
			return false;
			}

		//	Countermeasures start invisible (encrypted)

		pNewProgram->SetVisible(false);

		m_InPlay.Insert(pNewProgram);

		//	Add to our locus array

		m_Countermeasures[i] = pNewProgram;
		}

	//	Keep track of initial daimons. We don't do anything with this list, just 
	//	return it whan someone asks for it.

	m_InitialDaimons = Desc.Daimons;

	//	Done

	m_iState = stateStart;
	m_iTurn = 0;

	return true;
	}

CArtifactAwakening::EResultTypes CArtifactAwakening::PlayTurn (CItemType *pDaimonToPlay, int iDaimonsLeft, TArray<SEventDesc> &Results)

//	PlayTurn
//
//	Computes the next turn and returns a list of events (what happened during 
//	the turn).

	{
	int i;

	//	Turn starts when we deploy a daimon

	m_iTurn++;

	//	Deploy the daimon

	CArtifactProgram *pNewDaimon;
	if (!DeployDaimon(pDaimonToPlay, &pNewDaimon))
		{
		m_iTurn--;
		return resultError;
		}

	SEventDesc *pEvent = Results.Insert();
	pEvent->iEvent = eventDeployed;
	pEvent->pSource = pNewDaimon;

	//	Reset to base state

	m_Stat[CArtifactStat::statEgo].StartTurn();
	m_Stat[CArtifactStat::statIntelligence].StartTurn();
	m_Stat[CArtifactStat::statWillpower].StartTurn();

	for (i = 0; i < m_InPlay.GetCount(); i++)
		m_InPlay[i]->ResetStats();

	//	Start by applying patches, which enhance our own programs

	for (i = 0; i < m_InPlay.GetCount(); i++)
		{
		CArtifactProgram &Program = *m_InPlay[i];
		if (!Program.IsLive() || Program.WasHaltedLastTurn())
			continue;

		//	Run

		if (Program.GetType() == CArtifactProgram::typeDaimon)
			RunPatchEffects(Program, m_Daimons, Results);
		else
			RunPatchEffects(Program, m_Countermeasures, Results);
		}

	//	Pick a countermeasure to activate

	CArtifactProgram *pCountermeasure = CalcCountermeasureToActivate();
	if (pCountermeasure)
		{
		pCountermeasure->Activate();
		pCountermeasure->SetTurnDeployed(GetTurn());
		}

	//	Countermeasures get to run first

	for (i = 0; i < m_InPlay.GetCount(); i++)
		{
		CArtifactProgram &Program = *m_InPlay[i];

		//	Only countermeasures

		if (Program.GetType() != CArtifactProgram::typeCountermeasure)
			continue;

		//	If countermeasure is alive, then add event for activation,
		//	so it shows up.

		if (Program.IsLive())
			AddEventResult(eventActivated, &Program, NULL, Results);

		//	No effect if halted

		if (!Program.IsLive() || Program.WasHaltedLastTurn())
			continue;

		//	Run. These may halt some daimons.

		RunAttacks(Program, m_Daimons, Results);
		}

	//	Now run any active daimons

	for (i = 0; i < m_InPlay.GetCount(); i++)
		{
		CArtifactProgram &Program = *m_InPlay[i];
		if (!Program.IsActive())
			continue;

		//	Only daimons

		if (Program.GetType() != CArtifactProgram::typeDaimon)
			continue;

		//	Run. These may halt some countermeasures and/or change some core 
		//	stats (ego, intelligence, willpower).

		RunAttacks(Program, m_Countermeasures, Results);
		}

	//	If any core stat has been reduced to 0, then the player wins.

	if (m_iState == stateInBattle)
		{
		for (i = 0; i < CArtifactStat::statCount; i++)
			if (m_Stat[i] == 0)
				{
				m_iState = stateSuccess;
				break;
				}
		}

	//	If no more daimons can be deployed (either because the player has none
	//	or because there are no free loci) then the artifact wins.

	if (m_iState == stateInBattle
			&& (CalcFreeDaimonLoci() == 0 || iDaimonsLeft == 0))
		{
		m_iState = stateFailure;
		}

	//	Remember this set of results for the next turn (so we can remove results 
	//	that we already reported).

	m_LastResults = Results;

	//	Return result

	return GetStatus();
	}

void CArtifactAwakening::RunAttacks (CArtifactProgram &Program, TArray<CArtifactProgram *> &Targets, TArray<SEventDesc> &Results)

//	RunAttacks
//
//	Runs any attacks, determines whether succeed, and applies result.

	{
	int i;

	for (i = 0; i < Program.GetEffects().GetCount(); i++)
		{
		CArtifactProgram::SEffectDesc &Effect = Program.GetEffects()[i];

		switch (Effect.iType)
			{
			//	Attacks to core stats always succeed.

			case CArtifactProgram::effectTargetStat:
				m_Stat[Effect.iStat].Inc(-Program.GetStrength());

				if (Effect.iStat == CArtifactStat::statEgo)
					AddEventResult(eventEgoChanged, &Program, NULL, Results);
				else if (Effect.iStat == CArtifactStat::statIntelligence)
					AddEventResult(eventIntelligenceChanged, &Program, NULL, Results);
				else if (Effect.iStat == CArtifactStat::statWillpower)
					AddEventResult(eventWillpowerChanged, &Program, NULL, Results);
				break;

			case CArtifactProgram::effectHalt:
			case CArtifactProgram::effectWipe:
				{
				//	Figure out the target of the program (this also does a strength vs. defense check).

				CArtifactProgram *pTarget = CalcProgramTarget(Program, Effect.iType, CalcMatchingPrograms(Targets, Effect.Criteria, CArtifactProgram::FLAG_ACTIVE_ONLY));
				if (pTarget == NULL)
					continue;

				//	Halt the program

				pTarget->Halt();
				AddEventResult(eventHalted, &Program, pTarget, Results);
				break;
				}
			}
		}
	}

void CArtifactAwakening::RunPatchEffects (CArtifactProgram &Program, TArray<CArtifactProgram *> &Targets, TArray<SEventDesc> &Results)

//	RunPatchEffects
//
//	Runs any patch effects of the given program on the given targets.

	{
	int i, j;

	for (i = 0; i < Program.GetEffects().GetCount(); i++)
		{
		CArtifactProgram::SEffectDesc &Effect = Program.GetEffects()[i];

		switch (Effect.iType)
			{
			case CArtifactProgram::effectPatchDefense:
				{
				TArray<CArtifactProgram *> &Matching = CalcMatchingPrograms(Targets, Effect.Criteria);
				IncDefense(Matching, Program.GetStrength());

				for (j = 0; j < Matching.GetCount(); j++)
					AddEventResult(eventDefenseChanged, &Program, Matching[j], Results);
				break;
				}

			case CArtifactProgram::effectPatchStrength:
				{
				TArray<CArtifactProgram *> &Matching = CalcMatchingPrograms(Targets, Effect.Criteria);
				IncStrength(Matching, Program.GetStrength());

				for (j = 0; j < Matching.GetCount(); j++)
					AddEventResult(eventStrengthChanged, &Program, Matching[j], Results);
				break;
				}
			}
		}
	}
