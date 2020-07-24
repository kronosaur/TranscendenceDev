//	TSEMissions.h
//
//	CMission implementation
//	Copyright (c) 2012 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CMission : public TSpaceObjectImpl<OBJID_CMISSION>
	{
	public:
		enum class Status
			{
			open,							//	Mission is open, ready for acceptance
			closed,							//	Mission started without player
			accepted,						//	Mission accepted by player
			playerSuccess,					//	Player completed mission
			playerFailure,					//	Player failed mission
			success,						//	Mission succeeded without player
			failure,						//	Mission failed without player
			};

		struct SCriteria
			{
			bool bIncludeOpen = false;			//	Include open missions
			bool bIncludeUnavailable = false;	//	Include unavailable missions
			bool bIncludeActive = false;		//	Include active missions
			bool bIncludeCompleted = false;		//	Include completed (but not necessarily debriefed missions)
			bool bIncludeRecorded = false;		//	Include recorded missions

			bool bOnlySourceOwner = false;		//	Source must be owner
			bool bOnlySourceDebriefer = false;	//	Source must be debriefer
			bool bPriorityOnly = false;			//	Return highest priority mission
			bool bOnlyMissionArcs = false;		//	Only mission arc missions (latest only)

			TArray<CString> AttribsRequired;	//	Required attributes
			TArray<CString> AttribsNotAllowed;	//	Exclude objects with these attributes
			TArray<CString> SpecialRequired;	//	Special required attributes
			TArray<CString> SpecialNotAllowed;	//	Special excluding attributes
			};

		CMission (CUniverse &Universe);

		static ALERROR Create (CMissionType &Type, CMissionType::SCreateCtx &CreateCtx, CMission **retpMission, CString *retsError = NULL);

		void FireCustomEvent (const CString &sEvent, ICCItem *pData);
		bool CleanNonPlayer (void) const { return m_pType->CleanNonPlayer(); }
		DWORD GetAcceptedOn (void) const { return m_dwAcceptedOn; }
		const CString &GetArc (int *retiSequence = NULL) const;
		int GetArcSequence (void) const { return m_pType->GetArcSequence(); }
		const CString &GetArcTitle (void) const { return m_sArcTitle; }
		const CMissionType &GetMissionType (void) const { return *m_pType; }
		int GetPriority (void) const { return m_pType->GetPriority(); }
		const CString &GetTitle (void) const { return m_sTitle; }
		bool IsAccepted (void) const { return (m_iStatus == Status::accepted); }
		bool IsActive (void) const { return (m_iStatus == Status::accepted || (!m_fDebriefed && (m_iStatus == Status::playerSuccess || m_iStatus == Status::playerFailure))); }
		bool IsClosed (void) const { return (!IsActive() && IsCompleted()); }
		bool IsCompleted (void) const { return (m_iStatus == Status::playerSuccess || m_iStatus == Status::playerFailure || m_iStatus == Status::success || m_iStatus == Status::failure); }
		bool IsCompletedNonPlayer (void) const { return (m_iStatus == Status::success || m_iStatus == Status::failure); }
		bool IsFailure (void) const { return (m_iStatus == Status::failure || m_iStatus == Status::playerFailure); }
		bool IsOpen (void) const { return (m_iStatus == Status::open); }
		bool IsPlayerMission (void) const { return m_fAcceptedByPlayer; }
		bool IsRecorded (void) const { return (m_fDebriefed && (m_iStatus == Status::playerSuccess || m_iStatus == Status::playerFailure)); }
		bool IsSuccess (void) const { return (m_iStatus == Status::success || m_iStatus == Status::playerSuccess); }
		bool IsUnavailable (void) const { return (m_iStatus == Status::closed || m_iStatus == Status::success || m_iStatus == Status::failure); }
		bool KeepsStats (void) const { return m_pType->KeepsStats(); }
		bool MatchesCriteria (const CSpaceObject *pSource, const SCriteria &Criteria) const;
		void OnPlayerEnteredSystem (CSpaceObject *pPlayer);
		bool RefreshSummary (void);
		bool Reward (ICCItem *pData, ICCItem **retpResult = NULL);
		bool SetAccepted (void);
		bool SetDeclined (ICCItem **retpResult = NULL);
		bool SetFailure (ICCItem *pData);
		bool SetPlayerTarget (void);
		bool SetSuccess (ICCItem *pData);
		bool SetUnavailable (void);
		void UpdateExpiration (int iTick);

		static bool ParseCriteria (const CString &sCriteria, SCriteria *retCriteria);

		//	CSpaceObject virtuals

		virtual CMission *AsMission (void) override { return this; }
		virtual Categories GetCategory (void) const override { return catMission; }
		virtual const CObjectImageArray &GetImage (int *retiRotationFrameIndex = NULL) const override { return m_pType->GetImage(); }
		virtual CString GetNamePattern (DWORD dwNounPhraseFlags = 0, DWORD *retdwFlags = NULL) const override { if (retdwFlags) *retdwFlags = 0; return m_pType->GetName(); }
		virtual CDesignType *GetType (void) const override { return m_pType; }
		virtual bool HasAttribute (const CString &sAttribute) const override { return m_pType->HasLiteralAttribute(sAttribute); }
		virtual bool HasSpecialAttribute (const CString &sAttrib) const override;
		virtual bool IsMission (void) const override { return true; }
		virtual bool IsNonSystemObj (void) override { return true; }
		virtual void OnNewSystem (CSystem *pSystem) override;
		virtual bool SetProperty (const CString &sName, ICCItem *pValue, CString *retsError) override;

	protected:
		//	CSpaceObject virtuals

		virtual CString GetObjClassName (void) override { return CONSTLIT("CMission"); }
		virtual void OnDestroyed (SDestroyCtx &Ctx) override;
		virtual ICCItemPtr OnFindProperty (CCodeChainCtx &CCX, const CString &sProperty) const override;
		virtual void OnObjDestroyedNotify (SDestroyCtx &Ctx) override;
		virtual void OnReadFromStream (SLoadCtx &Ctx) override;
		virtual void OnUpdate (SUpdateCtx &Ctx, Metric rSecondsPerTick) override;
		virtual void OnWriteToStream (IWriteStream *pStream) override;

	private:
		enum CompletedReason
			{
			success,
			failure,
			destroyed,
			};

		void CloseMission (void);
		void CompleteMission (CompletedReason iReason);
		void FireOnAccepted (void);
		ICCItem *FireOnDeclined (void);
		ICCItem *FireOnReward (ICCItem *pData);
		void FireOnSetPlayerTarget (const CString &sReason);
		void FireOnStart (void);
		void FireOnStop (const CString &sReason, ICCItem *pData);

		CMissionType *m_pType = NULL;				//	Mission type
		Status m_iStatus = Status::open;			//	Current mission status
		int m_iMissionNumber = 0;					//	Ordinal number for mission of same type.
		CGlobalSpaceObject m_pOwner;				//	Mission owner (may be NULL)
		CGlobalSpaceObject m_pDebriefer;			//	Object at which player debriefs (may be NULL,
													//		in which case we debrief at the owner obj).
		CString m_sNodeID;							//	NodeID of owner
		DWORD m_dwCreatedOn = 0;					//	Mission created on (used to check expiration)
		DWORD m_dwAcceptedOn = 0;					//	Tick on which mission was accepted
		DWORD m_dwLeftSystemOn = 0;					//	Left the system on this tick (only used if the mission
													//		times out when out of the system).
		DWORD m_dwCompletedOn = 0;					//	Tick on which mission was completed

		CString m_sArcTitle;						//	Arc title (if any)
		CString m_sTitle;							//	Mission title
		CString m_sInstructions;					//	Current instructions

		DWORD m_fIntroShown:1 = false;				//	TRUE if player has seen intro
		DWORD m_fDeclined:1 = false;				//	TRUE if player has declined at least once
		DWORD m_fDebriefed:1 = false;				//	TRUE if player has been debriefed
		DWORD m_fInOnCreate:1 = false;				//	TRUE if we're inside OnCreate
		DWORD m_fInMissionSystem:1 = false;			//	TRUE if player is in the proper mission system
		DWORD m_fAcceptedByPlayer:1 = false;		//	TRUE if this is a player mission
		DWORD m_fInMissionCompleteCode:1 = false;	//	TRUE if we're completing (avoids recursion)

		DWORD m_fSpare8:1;
		DWORD m_dwSpare:24;

		//	Property table

		static TPropertyHandler<CMission> m_PropertyTable;
	};

class CMissionList
	{
	public:
		CMissionList (bool bFree = false) : m_bFree(bFree)
			{ }

		~CMissionList (void) { DeleteAll(); }

		const CMission &operator [] (int iIndex) const { return *m_List[iIndex]; }
		CMission &operator [] (int iIndex) { return *m_List[iIndex]; }

		bool CanCreateMissionInArc (const CMissionType &NewMissionType, const CSpaceObject *pSource, const CMission::SCriteria &CreateCriteria) const;
		void CloseMissionsInArc (const CMissionType &NewMissionType);
		void Delete (int iIndex);
		void Delete (CMission *pMission);
		void DeleteAll (void);
		CMissionList Filter (const CSpaceObject *pSource, const CMission::SCriteria &Criteria) const;
		CMissionList FilterByArc (void) const;
		CMissionList FilterByType (CMissionType &Type) const;
		CMission *FindAcceptedArcChapter (const CString &sTargetArc, const CString &sTargetTitle, CMission *pExclude = NULL) const;
		CMission *FindByArc (const CString &sTargetArc) const;
		CMission *FindLatestActivePlayer (void) const;
		CMission *FindHighestPriority (void) const;
		void FireOnSystemStarted (DWORD dwElapsedTime);
		void FireOnSystemStopped (void);
		int GetCount (void) const { return m_List.GetCount(); }
		CMission *GetMission (int iIndex) const { return m_List[iIndex]; }
		CMission *GetMissionByID (DWORD dwID) const;
		void Insert (CMission *pMission);
		bool Matches (const CSpaceObject *pSource, const CMission::SCriteria &Criteria) const;
		void NotifyOnNewSystem (CSystem *pSystem);
		void NotifyOnPlayerEnteredSystem (CSpaceObject *pPlayerShip);
		ALERROR ReadFromStream (SLoadCtx &Ctx, CString *retsError);
		ALERROR WriteToStream (IWriteStream *pStream, CString *retsError);

		static void Sort (TArray<CMissionType *> &Missions);

	private:
		TArray<CMission *> m_List;
		bool m_bFree;						//	If TRUE, free missions when removed
	};

class CTimedMissionEvent : public CSystemEvent
	{
	public:
		CTimedMissionEvent (int iTick,
							int iInterval,
							CMission *pMission,
							const CString &sEvent,
							const CString &sNode);
		CTimedMissionEvent (SLoadCtx &Ctx);

		virtual CString DebugCrashInfo (void) override;
		virtual void DoEvent (DWORD dwTick, CSystem &System) override;
		virtual CString GetEventHandlerName (void) override { return m_sEvent; }
		virtual CSpaceObject *GetEventHandlerObj (void) override { return m_pMission; }

	protected:
		virtual Classes GetClass (void) const override { return cTimedMissionEvent; }
		virtual bool OnIsEqual (CSystemEvent &Src) const override;
		virtual void OnWriteToStream (CSystem *pSystem, IWriteStream *pStream) override;

	private:
		int m_iInterval;			//	0 = not recurring
		CMission *m_pMission;
		CString m_sEvent;

		CString m_sNode;			//	Fire only in this node (blank = all nodes)
	};

