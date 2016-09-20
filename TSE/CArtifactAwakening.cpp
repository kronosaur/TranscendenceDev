//	CArtifactAwakening.cpp
//
//	CArtifactAwakening class
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define ERR_TOO_MANY_COUNTERMEASURES		CONSTLIT("Cannot deploy more than 6 countermeasures.")
#define ERR_INVALID_STAT					CONSTLIT("Stats must be from 1 to 24.")
#define ERR_NO_ROOM_FOR_DAIMON				CONSTLIT("All daimon loci are already filled.")

const int MAX_COUNTERMEASURE_COUNT =		6;
const int MAX_DAIMON_COUNT =				6;

CArtifactAwakening::CArtifactAwakening (void)

//	CArtifactAwakening constructor

	{
	}

TArray<CArtifactProgram *> CArtifactAwakening::CalcMatchingPrograms (const TArray<CArtifactProgram *> &Targets, const CString &sCriteria, DWORD dwFlags) const

//	CalcMatchingPrograms
//
//	Returns a list of program entries that match the criteria.

	{
	int i;

	TArray<CArtifactProgram *> Result;
	Result.GrowToFit(Targets.GetCount());

	//	Flags

	bool bActiveOnly = ((dwFlags & FLAG_ACTIVE_ONLY) == FLAG_ACTIVE_ONLY);
	bool bRunningLastTurn = ((dwFlags & FLAG_RUNNING_LAST_TURN) == FLAG_RUNNING_LAST_TURN);

	//	Parse the criteria

	CArtifactProgram::SCriteria Criteria;
	if (!CArtifactProgram::ParseCriteria(sCriteria, Criteria))
		return Result;

	//	Match criteria

	for (i = 0; i < Targets.GetCount(); i++)
		{
		//	Check flags

		if (bActiveOnly && !Targets[i]->IsActive())
			continue;

		if (bRunningLastTurn && Targets[i]->WasHaltedLastTurn())
			continue;

		//	Check criteria

		if (!Targets[i]->MatchCriteria(Criteria))
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

CArtifactProgram *CArtifactAwakening::CalcProgramTarget (CArtifactProgram::EEffectTypes iEffect, const TArray<CArtifactProgram *> &Targets) const

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

		int iScore = CalcTargetScore(iEffect, Target);
		if (iScore > iBestScore)
			{
			iBestScore = iScore;
			pBestProgram = &Target;
			}
		}

	//	Done

	return pBestProgram;
	}

int CArtifactAwakening::CalcTargetScore (CArtifactProgram::EEffectTypes iEffect, CArtifactProgram &Target) const

//	CalcTargetScore
//
//	Computes the likelihood that the given program should be a target for the
//	given effect.

	{
	int i;
	int iScore = 0;

	//	Loop over all target effects

	for (i = 0; i < Target.GetEffects().GetCount(); i++)
		{
		const CArtifactProgram::SEffectDesc &TargetEffect = Target.GetEffects().GetAt(i);

		switch (TargetEffect.iType)
			{
			case CArtifactProgram::effectTargetStat:
				iScore += 10 * TargetEffect.iValue;
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

bool CArtifactAwakening::DeployDaimon (CItemType *pType, CString *retsError)

//	DeployDaimon
//
//	Deploys a daimon.

	{
	ASSERT(IsValid());

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

	m_InPlay.Insert(pNewProgram);

	//	Add to locus

	m_Daimons[iPos] = pNewProgram;

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

	return true;
	}

void CArtifactAwakening::NextTurn (TArray<SEventDesc> &Results)

//	NextTurn
//
//	Computes the next turn and returns a list of events (what happened during 
//	the turn).

	{
	int i;

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
			RunPatchEffects(Program, m_Daimons);
		else
			RunPatchEffects(Program, m_Countermeasures);
		}

	//	Countermeasures get to run first

	for (i = 0; i < m_InPlay.GetCount(); i++)
		{
		CArtifactProgram &Program = *m_InPlay[i];
		if (!Program.IsLive() || Program.WasHaltedLastTurn())
			continue;

		//	Only countermeasures

		if (Program.GetType() != CArtifactProgram::typeCountermeasure)
			continue;

		//	Run. These may halt some daimons.

		RunAttacks(Program, m_Daimons);
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

		RunAttacks(Program, m_Countermeasures);
		}

	//	If any core stat has been reduced to 0, then the player wins.

	//	If no more daimons can be deployed (either because the player has none
	//	or because there are no free loci) then the artifact wins.

	ASSERT(IsValid());
	}

void CArtifactAwakening::RunAttacks (CArtifactProgram &Program, TArray<CArtifactProgram *> &Targets)

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
				m_Stat[Effect.iStat].Inc(Program.GetStrength());
				break;

			case CArtifactProgram::effectHalt:
			case CArtifactProgram::effectWipe:
				{
				//	Figure out the target of the program

				CArtifactProgram *pTarget = CalcProgramTarget(Effect.iType, CalcMatchingPrograms(Targets, Effect.sCriteria, FLAG_ACTIVE_ONLY));
				if (pTarget == NULL)
					continue;

				//	Halt the program

				pTarget->Halt();
				break;
				}
			}
		}
	}

void CArtifactAwakening::RunPatchEffects (CArtifactProgram &Program, TArray<CArtifactProgram *> &Targets)

//	RunPatchEffects
//
//	Runs any patch effects of the given program on the given targets.

	{
	int i;

	for (i = 0; i < Program.GetEffects().GetCount(); i++)
		{
		CArtifactProgram::SEffectDesc &Effect = Program.GetEffects()[i];

		switch (Effect.iType)
			{
			case CArtifactProgram::effectPatchDefense:
				IncDefense(CalcMatchingPrograms(Targets, Effect.sCriteria), Effect.iValue);
				break;

			case CArtifactProgram::effectPatchStrength:
				IncStrength(CalcMatchingPrograms(Targets, Effect.sCriteria), Effect.iValue);
				break;
			}
		}
	}
