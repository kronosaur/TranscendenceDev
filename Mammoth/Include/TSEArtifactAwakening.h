//	TSEArtifactAwakening.h
//
//	CArtifactAwakening class
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CArtifactStat
	{
	public:
		enum ETypes
			{
			statNone				= -1,
			statEgo					= 0,
			statIntelligence		= 1,
			statWillpower			= 2,

			statCount				= 3,
			statMax					= 24,
			};

		explicit CArtifactStat (int iOriginal = 0, int iMinValue = 0, int iMaxValue = -1) :
				m_iOriginal(iOriginal),
				m_iCurrent(iOriginal),
				m_iMinValue(iMinValue),
				m_iMaxValue(iMaxValue)
			{ }

		inline operator int () const { return m_iCurrent; }

		inline void Inc (int iValue = 1) { m_iCurrent = Max(m_iMinValue, m_iCurrent + iValue); if (m_iMaxValue != -1) m_iCurrent = Min(m_iCurrent, m_iMaxValue); }
		inline void Set (int iValue) { m_iOriginal = iValue; m_iCurrent = iValue; }
		inline void SetRange (int iMinValue, int iMaxValue) { m_iMinValue = iMinValue; m_iMaxValue = iMaxValue; }
		inline void StartTurn (void) { m_iCurrent = m_iOriginal; }

	private:
		int m_iOriginal;					//	Original, unmodified stat
		int m_iCurrent;						//	Current value this turn (after adjustments)

		int m_iMinValue;
		int m_iMaxValue;
	};

class CArtifactProgram
	{
	public:
		enum EFlags
			{
			//	MatchesCriteria

			FLAG_ACTIVE_ONLY =				0x00000001,	//	Only if program is currently active
			FLAG_RUNNING_LAST_TURN =		0x00000002,	//	Only if program was not halted last turn
			};

		enum EProgramTypes
			{
			typeNone,

			typeDaimon,
			typeCountermeasure,
			};

		enum EEffectTypes
			{
			effectNone,

			effectTargetStat,				//	Change iStat by iValue
			effectHalt,						//	Halt attack on opposing program matching sCriteria
			effectWipe,						//	Wipe attack on opposing program matching sCriteria
			effectLocusBlock,				//	Eliminates one of opponent's empty loci.

			effectPatchDefense,				//	Change target's strength by iValue
			effectPatchStrength,			//	Change target's strength by iValue
			};

		struct SCriteria
			{
			TArray<CString> ModifiersRequired;
			TArray<CString> ModifiersNotAllowed;
			};

		struct SEffectDesc
			{
			SEffectDesc (void) :
					iType(effectNone),
					iValue(0),
					iStat(CArtifactStat::statNone)
				{ }

			EEffectTypes iType;
			int iValue;						//	Strength of effect (varies by type)
			CArtifactStat::ETypes iStat;	//	Stat to target (varies by type)
			SCriteria Criteria;				//	Program criteria (varies by type)
			};

		CArtifactProgram (void) :
				m_iType(typeNone),
				m_iStatus(statusNone),
				m_iTurnDeployed(0),
				m_iLocusPos(-1),
				m_dwDestiny(mathRandom()),
				m_pItem(NULL),
				m_bVisible(true),
				m_bLastAttackSuccessful(false),
				m_bHaltedLastTurn(false)
			{ }

		inline void Activate (void) { if (m_iStatus == statusArchived) m_iStatus = statusRunning; }
		inline int GetDefense (void) const { return m_iDefense; }
		inline const TArray<SEffectDesc> &GetEffects (void) const { return m_Effects; }
		inline CItemType *GetItemType (void) const { return m_pItem; }
		inline int GetLocusIndex (void) const { return m_iLocusPos; }
		inline int GetStrength (void) const { return m_iStrength; }
		inline int GetTurnDeployed (void) const { return m_iTurnDeployed; }
		inline EProgramTypes GetType (void) const { return m_iType; }
		inline void Halt (void) { m_iStatus = statusHalted; }
		inline void IncDefense (int iValue) { m_iDefense.Inc(iValue); }
		inline void IncStrength (int iValue) { m_iStrength.Inc(iValue); }
		bool Init (EProgramTypes iType, int iLocus, CItemType *pItem, CString *retsError = NULL);
		inline bool IsActive (void) const { return (m_iStatus == statusRunning); }
		inline bool IsArchived (void) const { return (m_iStatus == statusArchived); }
		inline bool IsLive (void) const { return (m_iStatus != statusWiped && m_iStatus != statusArchived); }
		bool MatchesCriteria (const SCriteria &Criteria, DWORD dwFlags = 0) const;
		void ResetStats (void);
		inline void SetTurnDeployed (int iTurn) { m_iTurnDeployed = iTurn; }
		inline void SetVisible (bool bValue = true) { m_bVisible = bValue; }
		inline bool WasHaltedLastTurn (void) const { return m_bHaltedLastTurn; }

		static bool ParseCriteria (const CString &sCriteria, SCriteria &Criteria, CString *retsError = NULL);

	private:
		enum EProgramStatus
			{
			statusNone,

			statusArchived,					//	Not yet running
			statusRunning,					//	Deployed and running
			statusHalted,					//	Deployed, but currently halted
			statusWiped,					//	Wiped from the core
			};

		CUniverse &GetUniverse (void) const { return *g_pUniverse; }

		EProgramTypes m_iType;
		EProgramStatus m_iStatus;
		int m_iTurnDeployed;			//	Turn on which it was deployed
		int m_iLocusPos;				//	0-5 for daimons; 0-5 for countermeasures (-1 = not in play)
		DWORD m_dwDestiny;				//	Random number
		CItemType *m_pItem;				//	NULL if empty

		CString m_sBaseAttributes;		//	Original attributes
		CString m_sAttributes;			//	Current attributes (may be changed by effects)
		CArtifactStat m_iStrength;		//	Attack power (compute power)
		CArtifactStat m_iDefense;		//	Defense power (fault tolerance)
		TArray<SEffectDesc> m_Effects;	//	Effects

		bool m_bVisible;				//	TRUE if visible to the player.
		bool m_bLastAttackSuccessful;	//	TRUE if our last attack succeeded
		bool m_bHaltedLastTurn;			//	TRUE if we were halted last turn (only valid in NextTurn)
	};

class CArtifactAwakening
	{
	public:
		enum EEventTypes
			{
			eventNone,

			eventActivated,				//	Countermeasure activated
			eventDeployed,				//	Daimon deployed
			eventHalted,				//	Program halted

			eventEgoChanged,
			eventIntelligenceChanged,
			eventWillpowerChanged,

			eventStrengthChanged,
			eventDefenseChanged,
			};

		enum EResultTypes
			{
			resultNone,
			resultError,

			resultBattleContinues,
			resultArtifactSubdued,
			resultPlayerFailed,
			};

		struct SCreateDesc
			{
			int Stat[CArtifactStat::statCount];
			TArray<CItemType *> Countermeasures;
			TArray<CItemType *> Daimons;
			};

		struct SEventDesc
			{
			SEventDesc (void) :
					iEvent(eventNone),
					pSource(NULL),
					pTarget(NULL),
					bAlreadyReported(false)
				{ }

			EEventTypes iEvent;
			CArtifactProgram *pSource;
			CArtifactProgram *pTarget;

			bool bAlreadyReported;
			};

		CArtifactAwakening (void);
		inline ~CArtifactAwakening (void) { CleanUp(); }

		inline int GetCoreStat (CArtifactStat::ETypes iStat) const { return m_Stat[iStat]; }
		const TArray<CItemType *> &GetInitialDaimons (void) const { return m_InitialDaimons; }
		CArtifactProgram *GetLocusProgram (CArtifactProgram::EProgramTypes iType, int iIndex) const;
		EResultTypes GetStatus (void) const;
		inline int GetTurn (void) const { return m_iTurn; }
		bool Init (const SCreateDesc &Desc, CString *retsError = NULL);
		EResultTypes PlayTurn (CItemType *pDaimonToPlay, int iDaimonsLeft, TArray<SEventDesc> &Results);

	private:
		enum EStates
			{
			stateStart,						//	Have not yet deployed a single daimon
			stateInBattle,					//	Have deployed a daimon, but no resolution
			stateSuccess,					//	Artifact subjugated
			stateFailure,					//	Failed to subjugate
			};

		void AddEventResult (EEventTypes iEvent, CArtifactProgram *pSource, CArtifactProgram *pTarget, TArray<SEventDesc> &Results);
		bool DeployDaimon (CItemType *pType, CArtifactProgram **retpNewDaimon = NULL, CString *retsError = NULL);
		int CalcCountermeasureScore (CArtifactProgram &Program);
		CArtifactProgram *CalcCountermeasureToActivate (void);
		int CalcFreeDaimonLoci (void) const;
		TArray<CArtifactProgram *> CalcMatchingPrograms (const TArray<CArtifactProgram *> &Targets, const CArtifactProgram::SCriteria &Criteria, DWORD dwFlags = 0) const;
		CArtifactProgram *CalcProgramTarget (CArtifactProgram &Program, CArtifactProgram::EEffectTypes iEffect, const TArray<CArtifactProgram *> &Targets) const;
		int CalcTargetScore (CArtifactProgram &Program, CArtifactProgram::EEffectTypes iEffect, CArtifactProgram &Target) const;
		int CalcNextDaimonPos (void) const;
		bool CanAffectTarget (CArtifactProgram &Program, CArtifactProgram::EEffectTypes iEffect, CArtifactProgram &Target) const;
		void CleanUp (void);
		void IncDefense (const TArray<CArtifactProgram *> &Programs, int iValue);
		void IncStrength (const TArray<CArtifactProgram *> &Programs, int iValue);
		inline bool IsValid (void) const { return (m_InPlay.GetCount() > 0); }
		void RunAttacks (CArtifactProgram &Program, TArray<CArtifactProgram *> &Targets, TArray<SEventDesc> &Results);
		void RunPatchEffects (CArtifactProgram &Program, TArray<CArtifactProgram *> &Targets, TArray<SEventDesc> &Results);

		EStates m_iState;
		int m_iTurn;						//	Current turn
		CArtifactStat m_Stat[CArtifactStat::statCount];
		TArray<CItemType *> m_InitialDaimons;	//	List of initial (undeployed daimons)

		TArray<CArtifactProgram *> m_InPlay;	//	List of programs in play (we own the pointer)
		TArray<CArtifactProgram *> m_Daimons;	//	Ordered list of daimon by locus position.
		TArray<CArtifactProgram *> m_Countermeasures;	//	Ordered list of countermeasures by locus position.

		TArray<SEventDesc> m_LastResults;
	};
